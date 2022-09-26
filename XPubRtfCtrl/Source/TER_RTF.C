/*==============================================================================
   TER_RTF.C
   RTF read for TER files/buffers

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1992)
   ----------------------------------
   This license agreement allows the purchaser the right to modify the
   source code to incorporate in an application larger than the editor itself.
   The target application must not be a programmer's utility 'like' a text editor.
   Sub Systems, Inc. reserves the right to prosecute anybody found to be
   making illegal copies of the executable software or compiling the source
   code for the purpose of selling there after.
===============================================================================*/

#define  PREFIX extern
#include "ter_hdr.h"


#define RtfCmp(x,y) (x[0]!=y[0]?1:lstrcmp(x,y))

/*****************************************************************************
    ImportRtfData:
    This routine is called by the rtf reader (RtfRead) to process the rtf
    data as it reads from the file.  It is also called to initialize and
    end the reading process.
******************************************************************************/
BOOL ImportRtfData(PTERWND w,int action, struct StrRtfGroup far *CurGroup,struct StrRtfGroup far *NextGroup, LPBYTE data)
{
    BYTE PitchFamily,family,CurChar;
    struct StrRtfPict far *pic;
    struct StrRtfObject far *obj;
    struct StrRtf far *rtf;
    int    i,NewSect,CurFont;
    long   l;
    BOOL   ParaFramesFound=FALSE;


    // common activity for most actions
    if (action!=RTF_BEGIN && action!=RTF_END && CurGroup) {
        SetRtfTableInfo(w,CurGroup);
        
        //rtf=CurGroup->rtf;
        //OurPrintf("r: %d, c: %d, l: %d, pc: %d, t: (%s)",RtfCurRowId,RtfCurCellId,
        //   rtf->CurTblLevel,rtf->CurTblLevel<=0?0:rtf->TblLevel[rtf->CurTblLevel-1].CurCellId,rtf->OutBuf);
    }


    
    // perform specific actions
    switch (action) {
       case RTF_BEGIN:              // inialize the character, paragraph properties
          InRtfRead=TRUE;           // in rtf read now

          rtf=CurGroup->rtf;        // pointer to the rtf data structure
          rtf->FirstLine=CurLine;   // line where the text insertion begins

          lstrcpy(CurGroup->TypeFace,TerArg.FontTypeFace);
          // find the font family
          GetCharSet(w,hTerDC,TerArg.FontTypeFace,&PitchFamily,NULL,NULL);
          family=(BYTE)(PitchFamily&0xF0);
          if      (family==FF_ROMAN)      lstrcpy(CurGroup->FontFamily,"Roman");
          else if (family==FF_SWISS)      lstrcpy(CurGroup->FontFamily,"swiss");
          else if (family==FF_MODERN)     lstrcpy(CurGroup->FontFamily,"modern");
          else if (family==FF_SCRIPT)     lstrcpy(CurGroup->FontFamily,"script");
          else if (family==FF_DECORATIVE) lstrcpy(CurGroup->FontFamily,"decor");
          else                            lstrcpy(CurGroup->FontFamily,"nil");

          CurGroup->PointSize2=2*TerArg.PointSize;
          CurGroup->TextColor=TerFont[0].TextColor; // default text color
          CurGroup->TextBkColor=CLR_WHITE;  // default background color
          CurGroup->UlineColor=CLR_AUTO;    // underline color

          RtfInitCellId=RtfCurCellId=RtfCurRowId=RtfLastCellX=0;
          rtf->OpenCellId=rtf->OpenRowId=rtf->OpenLastCellX=0;
          RtfInitLevel=0;
          rtf->EmbedTable=TRUE;  // embed any table into the current text or table

          if (RtfInput==RTF_FILE || RtfInput==RTF_BUF) {
             for (l=0;l<TotalLines;l++) FreeLine(w,l);
             TotalLines=0;
             FileFormat=SAVE_RTF;          // file in rtf format
             if (CurGroup->TextColor==0) CurGroup->TextColor=CLR_AUTO;
             RtfInHdrFtr=0;
             rtf->InitSect=0;
          }
          else {
             RtfInHdrFtr=PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR;
             rtf->flags1|=RFLAG1_FIRST_TABLE;           // turn off after a table row is found

             CurGroup->flags|=RtfInHdrFtr;
             RtfInitCellId=RtfCurCellId=cid(CurLine);
             if (InUndo && CurUndoType==UNDO_ROW_DEL) RtfCurCellId=0;
             if (RtfCurCellId>0) {
                RtfCurRowId=cell[RtfCurCellId].row;
                CurGroup->InTable=TRUE;
                rtf->flags|=RFLAG_CURS_IN_TABLE;
                rtf->InitTblCol=GetCellColumn(w,RtfCurCellId,TRUE);
                if (!ClipEmbTable && (ClipTblLevel<=0 || cell[RtfCurCellId].level==(ClipTblLevel-1))) {  // data copied from the same table level
                    rtf->EmbedTable=FALSE;               // pasting at the same level
                    if (cell[RtfCurCellId].PrevCell>0 || !(TerFlags&TFLAG_ROW_PASTE))
                       rtf->PastingColumn=TRUE;          // pasting table column
                }
                if (ClipTblLevel<0) ClipTblLevel=1;      // default


                if (!LineInfo(w,CurLine,INFO_ROW)) rtf->InsertBefCell=RtfCurCellId;  // insert the data before this cell
                RtfInitLevel=cell[RtfCurCellId].level+1;
                CurGroup->level=rtf->CurTblLevel=RtfInitLevel;
                
                SaveRtfLevelInfo(w,rtf,rtf->CurTblLevel);  // save the table varaibles in the level structure
                if (rtf->CurTblLevel>0) rtf->TblLevel[rtf->CurTblLevel-1].CurCellId=cell[RtfCurCellId].ParentCell;  // parent cell is needed to create cells at CurTblLevel
             }

             RtfParaFID=fid(CurLine);
             if (RtfParaFID>0) {
                RtfParaFrameInfo.x=ParaFrame[RtfParaFID].x;
                RtfParaFrameInfo.y=ParaFrame[RtfParaFID].ParaY;
                RtfParaFrameInfo.width=ParaFrame[RtfParaFID].width;
                RtfParaFrameInfo.height=ParaFrame[RtfParaFID].MinHeight;
                RtfParaFrameInfo.DistFromText=ParaFrame[RtfParaFID].DistFromText;
                RtfParaFrameInfo.ZOrder=ParaFrame[RtfParaFID].ZOrder;

                FarMove(&RtfParaFrameInfo,&(CurGroup->ParaFrameInfo),sizeof(RtfParaFrameInfo));
                CurGroup->FrmFlags|=PARA_FRAME_USED;
                rtf->flags1|=RFLAG1_PASTING_IN_PARA_FRAME;
             }

             rtf->InitSect=GetSection(w,CurLine);
             PaperOrient=TerSect[rtf->InitSect].orient;   // 20050922

             // Disable reading of header/footer groups in the current section
             if ((!rtf->EmptyDoc && !(TerFlags3&TFLAG3_GET_BUF_HDR_FTR)) 
                   || RtfHdrFtrExists(w,rtf->InitSect)) {
                rtf->flags|=(RFLAG_HDR_FTR_FOUND|RFLAG_SECT_HDR_FOUND|RFLAG_SECT_FTR_FOUND);
                rtf->flags1|=(RFLAG1_SECT_FHDR_FOUND|RFLAG1_SECT_FFTR_FOUND);
             }

             // record the initial field id
             CurFont=GetCurCfmt(w,CurLine,CurCol);
             CurChar=GetCurChar(w,CurLine,CurCol);
             if ((TerFont[CurFont].FieldId==FIELD_NAME && CurChar!='{') || TerFont[CurFont].FieldId==FIELD_DATA)
                rtf->InitFieldId=TerFont[CurFont].FieldId;

             // record the initial para style id so the fonts gets created with initial para style id
             CurGroup->ParaStyId=PfmtId[pfmt(CurLine)].StyId;

             // record the tracking variables
             if (TrackChanges) {
                CurGroup->revised=true;
                CurGroup->InsRev=TrackRev;
                CurGroup->InsTime=TrackTime;
             } 
          }
          
          RepageBeginLine=CurLine;        // repagination to occur from here

          break;


       case RTF_END:
          dm("RTF_END");

          rtf=(struct StrRtf far *)data;            // pointer to the rtf data structure

          // delete rows not used
          for (i=0;i<TotalTableRows;i++) {
             if (TableRow[i].InUse && TableAux[i].flags&TAUX_RTF_DEL) DeleteRtfRow(w,rtf,i);
          }
         
          if (TotalLines==0) {
             TotalLines=1;
             InitLine(w,0);
          }

          // merge line pieces together in non-wordwrap mode
          if (!TerArg.WordWrap && RtfInput<RTF_CB)
             MergeRtfLinePieces(w,0,TotalLines-1); // construct lines

          if ((TerFlags4&TFLAG4_HIDE_HIDDEN_PARA_MARK) && RtfInput<RTF_CB) HideHiddenParaMarkers(w);

          if (RtfInput>=RTF_CB && TerFlags4&TFLAG4_PASTE_LAST_PARA_PROP && CurLine<TotalLines && CurLine>0) {
             if (LineLen(CurLine)==1 && LineFlags(CurLine)&LFLAG_PARA) pfmt(CurLine)=pfmt(CurLine-1);
          } 
          
          // set the default section
          if (RtfInput<RTF_CB || (TerFlags3&TFLAG3_GET_BUF_HDR_FTR)) {
             FarMove(&(rtf->sect),&TerSect[rtf->InitSect],sizeof(struct StrSect)); //Initialize from the current section
             if ((rtf->flags2&RFLAG2_PAPER_DIM_FOUND) && TerSect[rtf->InitSect].orient==DMORIENT_LANDSCAPE) {   // PprWidth does not change with orienatation, it a width in the protrait orientation
                float temp=TerSect[rtf->InitSect].PprWidth;
                TerSect[rtf->InitSect].PprWidth=TerSect[rtf->InitSect].PprHeight;
                TerSect[rtf->InitSect].PprHeight=temp;
             } 
          }
          FixNegativeIndents(w,rtf);                   // fix negative left indentations

          if (pDeviceMode && True(TerFlags&TFLAG_APPLY_PRT_PROPS) && (RtfInput==RTF_FILE || RtfInput==RTF_BUF))
              ApplyPrinterSetting(w,pDeviceMode->dmOrientation,pDeviceMode->dmDefaultSource,PaperSize,PageWidth,PageHeight);
          if (TotalSects>1) RecreateSections(w);   // create section boundaries
          SetSectPageSize(w);                      // set the page size for sections

          if (TerArg.PrintView) RepairTable(w);     // create other table structures


          if (RtfInput==RTF_FILE || RtfInput==RTF_BUF) {
             // set the paper size and orientation
             PaperOrient=TerSect[0].orient;        // default orientation

             CurLine=CurRow=CurCol=0;
             RepageBeginLine=0;
             if (TotalParaFrames==1) WrapAddLines=50;   // for faster initial wrapping
             

             if (TerArg.PageMode) {
                int  PageLimit=TotalParaFrames>500?2:30;
                BOOL FullRepage=(BatchMode || TotalLines<SMALL_FILE);
                if (TotalParaFrames>500) FullRepage=false;  // likely an exported report with many text boxes
                if (!FullRepage) for (i=0;i<TotalFonts;i++) if (TerFont[i].InUse && TerFont[i].FieldId==FIELD_TOC) FullRepage=TRUE;
                if (!FullRepage) for (i=0;i<TotalPfmts;i++) if (TerBlt[PfmtId[i].BltId].ls>0) FullRepage=TRUE;
                if (TerFlags5&TFLAG5_FULL_REPAGINATE) FullRepage=TRUE;

                if (FullRepage) Repaginate(w,FALSE,FALSE,0,TRUE);     // wrap all lines
                else            Repaginate(w,FALSE,FALSE,PageLimit,TRUE);    // wrap only first 30 pages now
             }
             else if (TerArg.WordWrap) WordWrap(w,0,TotalLines); // wrap the rtf text

             // check if we set for repagination in idle time
             if (TerArg.PageMode) {
                for (i=1;i<TotalParaFrames;i++) {
                   if (ParaFrame[i].InUse && !(ParaFrame[i].flags&PARA_FRAME_WRAP_THRU)) {
                      ParaFramesFound=TRUE;
                      break;
                   }
                }
             }
          }
          else CurCol=0;


          // header/footer processing

          if (TerArg.WordWrap) ReposPageHdrFtr(w,FALSE);
          if ((rtf->flags&RFLAG_PARA_FRAME_FOUND || rtf->flags1&RFLAG1_NEW_HDR_FTR)
              && !ViewPageHdrFtr && TerArg.PageMode && !(TerFlags2&TFLAG2_NO_AUTO_HDR_FTR)) {
             BOOL SavePaintEnabled=PaintEnabled;
             PaintEnabled=FALSE;
             ToggleViewHdrFtr(w);
             PaintEnabled=SavePaintEnabled;
          }

          if (RtfInput==RTF_FILE || RtfInput==RTF_BUF) {
             if (ParaFramesFound) {    // repaginate one more time
                TerRepaginate(hTerWnd,TRUE);
                //RepageBeginLine=0;
             }
             if (ViewPageHdrFtr && TerArg.PageMode) {   // position at the first header line
                BOOL LineSet=FALSE;
                for (l=0;l<TotalLines;l++) {
                   if ((PfmtId[pfmt(l)].flags&PAGE_HDR) && !(LineFlags(l)&LFLAG_HDRS)) {
                      if (!LineSet) CurLine=l;
                      LineSet=TRUE;
                      if (l>0 && LineFlags(l-1)&LFLAG_FHDR) {
                         CurLine=l;
                         break;
                      }
                   }
                   if (!(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)) break;
                }
             }

             TerArg.modified=0;
          }

          if (RtfInput<RTF_CB && rtf->EnableTracking) TerEnableTracking(hTerWnd,true,"",true,0,CLR_AUTO,0,CLR_AUTO);

          UpdateToolBar(w,FALSE);
          RequestPagination(w,TRUE);

          InRtfRead=FALSE;
          

          dm("RTF_END - exit");

          break;

       case RTF_SECT:                              // process section break
          rtf=(struct StrRtf far *)data;           // pointer to the rtf data structure
          // create a new section
          for (NewSect=0;NewSect<TotalSects;NewSect++) if (!TerSect[NewSect].InUse) break;
          if (NewSect==TotalSects) {
             if (TotalSects>=MaxSects) {
                ExpandSectArray(w,-1);
                if (TotalSects>=MaxSects) return PrintError(w,MSG_SECTION_FULL,NULL);
             }
             TotalSects++;
          }
          FarMove(&(rtf->sect),&TerSect[NewSect],sizeof(struct StrSect)); //Initialize from the current section
          if ((rtf->flags2&RFLAG2_PAPER_DIM_FOUND) && TerSect[NewSect].orient==DMORIENT_LANDSCAPE) {   // PprWidth is the width in portrait orientation
             float temp=TerSect[NewSect].PprWidth;
             TerSect[NewSect].PprWidth=TerSect[NewSect].PprHeight;
             TerSect[NewSect].PprHeight=temp;
          } 
          WrapSect=NewSect;                        // pass using global variable
          return ImportRtfTextLine(w,&(rtf->group[rtf->GroupLevel]),rtf->OutBuf); // insert the section break character

       case RTF_TEXT:
          return ImportRtfTextLine(w,CurGroup,data);

       case RTF_PICT:                              // insert a picture
          pic=(struct StrRtfPict far *)data;
          return ImportRtfPicture(w,CurGroup,pic);

       case RTF_OBJECT:                            // insert an OLE object
          obj=(struct StrRtfObject far *)data;
          DlgLong=(DWORD)obj;                      // workaround MSVC1.5 compiler optimization bug, force compiler to use 'obj' variable
          TerFont[obj->pict].ObjectType=obj->ObjectType;
          TerFont[obj->pict].ObjectSize=obj->ObjectSize;
          TerFont[obj->pict].hObject=obj->hObject;
          TerFont[obj->pict].ObjectAspect=obj->ObjectAspect;
          TerFont[obj->pict].ObjectUpdate=obj->ObjectUpdate;
          return TerOleLoad(w,obj->pict);          // load the ole object

       default:
          break;
    }

    return TRUE;
}

 
/*****************************************************************************
    RtfRead:
    Read the input file or parse the input buffer to extract RTF code
    and text.  If the file is used for input, specify the file name (arg #2).
    Otherwise specify the pointer to the input buffer.  If the length of
    the buffer is known, specify it using the 'BufLen' argument, otherwise
    set the 'BufLen' argument to -1.
******************************************************************************/
BOOL RtfRead(PTERWND w,int input, LPBYTE InFile,BYTE huge *InBuf,long BufLen)
{
    HANDLE  hFile=INVALID_HANDLE_VALUE;
    struct StrRtf far *rtf=NULL;
    struct StrRtfGroup far *group=NULL;
    BOOL   result=TRUE;
    int    i,sect;
    BOOL   SavePaintEnabled;
    HCURSOR hSaveCursor=NULL;
    BYTE   ErrorMsg[300];

    dm("RtfRead");

    RtfInput=input;                       // input type RTF_FILE, RTF_BUF, RTF_CB, RTF_CB_BUF, RTF_FRAME_BUF

    FootnoteRest[0]=0;                    // reset the footnote marker string
    FootnoteRestFont=0;

    // open the input file
    if (input==RTF_FILE) {               // data in the file
         //lstrupr(w,InFile);
         rTrim(InFile);                   // trim any spaces on right
         if (lstrlen(InFile)==0) return FALSE;
         if ((hFile=CreateFile(InFile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) return FALSE;
    }

    // set the preexist flag to the existing table rows and cells
    for (i=0;i<TotalTableRows;i++) {
        if (TableRow[i].InUse) TableRow[i].flags|=ROWFLAG_PREEXIST;
        ResetUintFlag(TableAux[i].flags,(TAUX_TROWD_DONE|TAUX_REUSE_CELL_CHAIN|TAUX_HAS_NEW_TEXT|TAUX_RTF_DEL|TAUX_COPIED));
    }
    for (i=0;i<TotalCells;i++) {
        if (cell[i].InUse) {
           cell[i].flags|=CFLAG_PREEXIST;
           ResetUintFlag(CellAux[i].flags,CAUX_CLOSED);
        }
    }

    // set revieer table index
    for (i=0;i<TotalReviewers;i++) reviewer[i].RtfId=i;

    // allocate space for the rtf info structure
    if (NULL==(rtf=(struct StrRtf far *)OurAlloc(sizeof(struct StrRtf)))) {
       return PrintError(w,MSG_OUT_OF_MEM,"RtfRead(rtf)");
    }
    FarMemSet(rtf,0,sizeof(struct StrRtf));  // initialize with zeros
    sect=GetSection(w,CurLine);
    rtf->PaperWidth=(int)InchesToTwips(TerSect1[sect].PgWidth);   // default width
    rtf->PaperHeight=(int)InchesToTwips(TerSect1[sect].PgHeight); // default height
    rtf->InitTblCol=-1;
    rtf->CurTabType=TAB_LEFT;
    rtf->lang=DefLang;
    if (TotalLines==1 && LineLen(0)<=1) rtf->EmptyDoc=TRUE;

    if (input==RTF_FILE) rtf->hFile=hFile;
    else {
       rtf->buf=InBuf;
       rtf->BufLen=BufLen;
       rtf->hFile=INVALID_HANDLE_VALUE;
    }

    // allocate rtf group table
    if (NULL==(group=rtf->group=(struct StrRtfGroup far *)OurAlloc(sizeof(struct StrRtfGroup)*MAX_RTF_GROUPS))) {
       return PrintError(w,MSG_OUT_OF_MEM,"RtfRead(group)");
    }
    FarMemSet(group,0,sizeof(struct StrRtfGroup)*MAX_RTF_GROUPS);  // initialize with zeros
    group[0].rtf=rtf;                                              // save the rtf pointer
    group[0].CharStyId=1;                                          // default character style id
    group[0].CharSet=DEFAULT_CHARSET;                              // default character set
    group[0].CellPatBC=CLR_WHITE;                                  // background color pattern
    group[0].ParaBkColor=CLR_WHITE;
    group[0].CharPatBC=CLR_WHITE;                                  // default pattern background color
    group[0].shape.WrapType=SWRAP_NO_WRAP;                         // shape alone on the line
    group[0].shape.WrapSide=WRAPSIDE_BOTH;                         // wrap both side if wrapping enabled
    group[0].shape.type=SHPTYPE_NONE;                              // default number of characters to be ignore when a unicode character is encountered
    group[0].UcIgnoreCount=1;
    group[0].TextBoxMargin=-1;                                     // to indicate the default value
    group[0].OutlineLevel=-1;                                      // -1 = no outline level
    group[0].ParaBorderColor=CLR_AUTO;                             // paragraph border color
    for (i=0;i<MAX_BORDERS;i++) group[0].BorderColor[i]=CLR_AUTO;
    FarMove(&(TerBlt[0]),&(group[0].blt),sizeof(struct StrBlt));   // initialize the bullet structure

    // initialize other rtf control variables
    RtfCurRowId=RtfCurCellId=RtfLastCellX=RtfParaFID=0;
    RtfInEquation=FALSE;
    FarMemSet(&RtfParaFrameInfo,0,sizeof(RtfParaFrameInfo));
    FirstFreeCellId=0;

    // disable painting during the read
    SavePaintEnabled=PaintEnabled;       // disable painting during the read
    PaintEnabled=FALSE;

    // check for rtf signature
    result=FALSE;
    if (!GetRtfWord(w,rtf) || !(rtf->GroupBegin)) goto END_FILE;
    if ( !GetRtfWord(w,rtf) || !(rtf->IsControlWord)) goto END_FILE;
    lstrupr(w,rtf->CurWord);
    if (lstrcmpi("RTF",rtf->CurWord)!=0) goto END_FILE;

    InitRtfGroup(w,rtf,rtf->GroupLevel);       // initialize the \rtf group

    // initialize the rtf data importer
    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while reading

    if (!ImportRtfData(w,RTF_BEGIN,&(group[rtf->GroupLevel]),NULL,NULL)) goto END_FILE;

    FarMove(&TerSect[rtf->InitSect],&(rtf->sect),sizeof(struct StrSect));  // default section properties
    if (True(TerFlags3&TFLAG3_GET_BUF_HDR_FTR)) rtf->sect.orient=DMORIENT_PORTRAIT;  // set to default orientation to start with

    for (i=0;i<rtf->GroupLevel;i++) {
       group[i].flags=group[rtf->GroupLevel].flags;    // set the initial flags to all initial levels
       group[i].FrmFlags=group[rtf->GroupLevel].FrmFlags;
       group[i].level=group[rtf->GroupLevel].level;
       FarMove(&(group[rtf->GroupLevel].ParaFrameInfo),&(group[i].ParaFrameInfo),sizeof(struct StrRtfParaFrameInfo));
    }
    rtf->InitialCell=RtfCurCellId;            // value of the initial cell id
    rtf->InitialParaFID=RtfParaFID;

    // Read next word and process
    result=TRUE;

    NEXT_WORD:
    if (!GetRtfWord(w,rtf)) goto END_FILE;    // read next word

    if (rtf->IsControlWord) {                 // process the control word
       int ReturnCode=ProcessRtfControl(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) goto FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    goto SYNTAX_ERROR;
       else if (ReturnCode==RTF_END_FILE)        goto END_FILE;
       else if (rtf->SuspendReading)             goto END_FILE;
    }
    else if (rtf->WordLen>0) {
       if (!CopyToOutBuf(w,rtf)) goto END_FILE;   // copy word to the output buffer
    }


    goto NEXT_WORD;


    FILE_INCOMPLETE:
    wsprintf(ErrorMsg,"pos: %ld, control: %d, beg group: %d, end group: %d, word: %.40s, word len: %d, param: %d",
               rtf->FilePos,rtf->IsControlWord,rtf->GroupBegin,rtf->GroupEnd,rtf->CurWord,rtf->WordLen,rtf->IntParam);
    PrintError(w,MSG_UNEXPECTED_EOF,ErrorMsg);
    goto END_FILE;

    SYNTAX_ERROR:
    wsprintf(ErrorMsg,"pos: %ld, control: %d, beg group: %d, end group: %d, word: %.40s, word len: %d, param: %d",
               rtf->FilePos,rtf->IsControlWord,rtf->GroupBegin,rtf->GroupEnd,rtf->CurWord,rtf->WordLen,rtf->IntParam);
    PrintError(w,MSG_READ_ABORTED,ErrorMsg);

    END_FILE:
    SendRtfText(w,rtf);                       // send the remaining text if any
    
    if (rtf->OpenRowId>0) DeleteRtfRow(w,rtf,rtf->OpenRowId);  // delete the last empty row

    if (result) ImportRtfData(w,RTF_END,NULL,NULL,(LPBYTE)rtf);  // terminate the rtf data importer
    else {                                    // not an rtf file
       InRtfRead=FALSE;
    }

    if (hSaveCursor) SetCursor(hSaveCursor);   // load the regular cursor

    if (hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile);
    if (group)     OurFree(group);    // free the rtf group table
    if (rtf->FieldCode) OurFree(rtf->FieldCode);
    if (rtf)       OurFree(rtf);      // free the rtf structure

    PaintEnabled=SavePaintEnabled;   // enable the screen painting

    return result;
}

/*****************************************************************************
    CopyToOutBuf:
    Copy the text to output buffer.
******************************************************************************/
CopyToOutBuf(PTERWND w,struct StrRtf far *rtf)
{
    struct StrRtfGroup far *group=rtf->group;
    int i;

    // check if any letters needs to be ignored for unicode
    if (group[rtf->GroupLevel].IgnoreCount>0) {
       int discard=(group[rtf->GroupLevel].IgnoreCount<rtf->WordLen)?group[rtf->GroupLevel].IgnoreCount:rtf->WordLen;
       for (i=0;i<(rtf->WordLen-discard);i++) rtf->CurWord[i]=rtf->CurWord[i+discard];
       rtf->WordLen-=discard;
       rtf->CurWord[rtf->WordLen]=0;
       group[rtf->GroupLevel].IgnoreCount-=discard;
    }
    // send previous text if buffer full
    if (rtf->WordLen>0) {
      if (rtf->OutBufLen+rtf->WordLen>RTF_LINE_LEN || group[rtf->GroupLevel].UcBase>0) 
          if (!SendRtfText(w,rtf)) return false;
      lstrcat(rtf->OutBuf,rtf->CurWord);
      rtf->OutBufLen+=rtf->WordLen;
    }

    return true;
}

/*****************************************************************************
    DeleteRtfRow:
    Setup the group that begins an RTF group.
******************************************************************************/
DeleteRtfRow(PTERWND w,struct StrRtf far *rtf, int row)
{
    int PrevRow=TableRow[row].PrevRow;
    int cl=TableRow[row].FirstCell;

    if (PrevRow>0) TableRow[PrevRow].NextRow=-1; 
    TableRow[row].InUse=FALSE;
    while (cl>0) {
       cell[cl].InUse=FALSE;
       cl=cell[cl].NextCell;
    } 
    
    return TRUE;
}
    
/*****************************************************************************
    ProcessRtfControl:
    Process an rtf control.
    This function returns 0 when successful.  Otherwise it returns
    RTF_FILE_INCOMPLETE, RTF_SYNTAX_ERROR or RTF_END_FILE
******************************************************************************/
#pragma optimize("g",off)  // turn-off global optimization - takes too much time
int ProcessRtfControl(PTERWND w,struct StrRtf far *rtf)
{
    int    i,count,ReturnCode,CurGroup,BorderType;
    BOOL   ControlProcessed;
    struct StrRtfGroup far *group=rtf->group;
    BYTE   CurWord[MAX_WIDTH+1];

    lstrcpy(CurWord,rtf->CurWord);
    lstrlwr(w,CurWord);

    CurGroup=rtf->GroupLevel;
    group[CurGroup].IgnoreCount=0;  // a regular character does not follow a unicode tag

    if (RtfCmp(CurWord,"par")!=0
     && RtfCmp(CurWord,"line")!=0
     && RtfCmp(CurWord,"lbr")!=0      // special line break
     && RtfCmp(CurWord,"cell")!=0
     && RtfCmp(CurWord,"nestcell")!=0
     && RtfCmp(CurWord,"tab")!=0
     && RtfCmp(CurWord,"tx")!=0
     && RtfCmp(CurWord,"u")!=0
     && !SendRtfText(w,rtf)) return RTF_END_FILE;// send the previous text if any

    ControlProcessed=TRUE;

    // begin an embedded rtf group
    if (RtfCmp(CurWord,"rtf")==0){// embedded rtf group
       InitRtfGroup(w,rtf,rtf->GroupLevel);       // initialize the \rtf group
    }
    // Extract the document information
    else if (RtfCmp(CurWord,"info")==0){// left margin
       if (RtfInput<RTF_CB) ReadRtfInfo(w,rtf);
       else SkipRtfGroup(w,rtf);
    }
    // Extract the reviewer table
    else if (RtfCmp(CurWord,"revtbl")==0){// reviewers
       if (RtfInput<RTF_CB) ReadRtfReviewers(w,rtf);
       else SkipRtfGroup(w,rtf);
    }
    else if (RtfCmp(CurWord,"revised")==0){  // text inserted
       group[CurGroup].revised=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"deleted")==0){  // text inserted
       group[CurGroup].deleted=(int)rtf->IntParam;
    }
    else if ( RtfCmp(CurWord,"revauth")==0   // reviewer authoer
           || RtfCmp(CurWord,"revauthdel")==0){ // reviewer authoer
       int rev=(int)rtf->IntParam;
       for (i=0;i<TotalReviewers;i++) if (reviewer[i].RtfId==rev) break;
       if (i<TotalReviewers) {   // author found
          if (RtfCmp(CurWord,"revauth")==0) group[CurGroup].InsRev=i;
          else                              group[CurGroup].DelRev=i;
       } 
    }
    else if ( RtfCmp(CurWord,"revdttm")==0      // insertion time
           || RtfCmp(CurWord,"revdttmdel")==0){ // deletion time
       DWORD DateTime=(DWORD)(rtf->IntParam);
       DWORD  min=GetBitVal(DateTime,5,6);        // 6 bits at ending offset 5
       DWORD  hr=GetBitVal(DateTime,10,5);        // 5 bits at ending offset 10
       DWORD  day=GetBitVal(DateTime,15,5);       // 5 bits at ending offset 15
       DWORD  mon=GetBitVal(DateTime,19,4);       // 4 bits at ending offset 19
       DWORD  yr=GetBitVal(DateTime,28,9)+1900;   // 9 bits at ending offset 28

       DateTime=(min<<5)|(hr<<11)|(day<<16)|(mon<<21)|((yr-1980)<<25); // starting offsets min=5, hr=11, day=16, mon=21, yr=25
       if (RtfCmp(CurWord,"revdttm")==0) group[CurGroup].InsTime=DateTime;
       else                              group[CurGroup].DelTime=DateTime;
    }
    else if (RtfCmp(CurWord,"revisions")==0){  // enable tracking
       if (RtfInput<RTF_CB) rtf->EnableTracking=true;
    }

    // language information
    else if (RtfCmp(CurWord,"deflang")==0){// default language
       DWORD SaveLang=ReqLang;
       byte  SaveCharSet=ReqCharSet;

       rtf->lang=(int)rtf->IntParam;
       if (RtfInput<RTF_CB) DefLang=rtf->lang;

       SetCurLang(w,(HKL)rtf->lang);   // get the character set
       rtf->CharSet=ReqCharSet;
       ReqCharSet=SaveCharSet;   // restore
       ReqLang=SaveLang;

       if (!mbcs && rtf->CharSet!=DEFAULT_CHARSET && rtf->CharSet!=ANSI_CHARSET 
            && rtf->CharSet!=SYMBOL_CHARSET  && IsMbcsCharSet(w,rtf->CharSet,NULL)) EnableMbcs(w,rtf->CharSet);
    }
    else if (RtfCmp(CurWord,"lang")==0){// current language
       group[CurGroup].lang=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"mac")==0){// is a mac file
       rtf->mac=TRUE;
    }

    // Extract background color
    else if (RtfCmp(CurWord,"viewkind")==0){// left margin
       if (RtfInput<RTF_CB) ViewKind=(int)rtf->IntParam;
    } 
    else if (RtfCmp(CurWord,"background")==0){// left margin
       if (RtfInput<RTF_CB /*&& ViewKind==VKIND_OUTLINE_LAYOUT*/) group[CurGroup].gflags|=GFLAG_BACKGROUND;  // in background group
       else SkipRtfGroup(w,rtf);
    }
    else if (RtfCmp(CurWord,"formprot")==0){// turn on protected form mode
       if (RtfInput<RTF_CB) {
          ProtectForm=true;
          TerArg.ReadOnly=true;
       } 
    }
    else if (RtfCmp(CurWord,"deftab")==0){//  default tab width
       if (RtfInput<RTF_CB) DefTabWidth=(int)rtf->IntParam;
    }

    // Extract document margins
    else if (RtfCmp(CurWord,"margl")==0){// left margin
       if (RtfInput<RTF_CB) {
          if (rtf->IntParam<0) rtf->IntParam=720;
          TerSect[0].LeftMargin=rtf->sect.LeftMargin=TwipsToInches(rtf->IntParam);
          rtf->flags2|=RFLAG2_LEFT_MARG_FOUND;
       }
    }
    else if (RtfCmp(CurWord,"margr")==0){// right margin
       if (RtfInput<RTF_CB) {
          if (rtf->IntParam<0) rtf->IntParam=720;
          TerSect[0].RightMargin=rtf->sect.RightMargin=TwipsToInches(rtf->IntParam);
       }
    }
    else if (RtfCmp(CurWord,"margt")==0){// top margin
       if (RtfInput<RTF_CB) {
          if (rtf->IntParam<0) {
            TerSect[0].flags|=SECT_EXACT_MARGT;
            rtf->sect.flags|=SECT_EXACT_MARGT;
            rtf->IntParam=-rtf->IntParam;  //720;
          }
          else {
            ResetUintFlag(TerSect[0].flags,SECT_EXACT_MARGT);
            ResetUintFlag(rtf->sect.flags,SECT_EXACT_MARGT);
          } 
          TerSect[0].TopMargin=rtf->sect.TopMargin=TwipsToInches(rtf->IntParam);
       }
    }
    else if (RtfCmp(CurWord,"margb")==0){// bottom margin
       if (RtfInput<RTF_CB) {
          if (rtf->IntParam<0) {
            TerSect[0].flags|=SECT_EXACT_MARGB;
            rtf->sect.flags|=SECT_EXACT_MARGB;
            rtf->IntParam=-rtf->IntParam;  // 720;
          }
          else {
            ResetUintFlag(TerSect[0].flags,SECT_EXACT_MARGB);
            ResetUintFlag(rtf->sect.flags,SECT_EXACT_MARGB);
          } 
          TerSect[0].BotMargin=rtf->sect.BotMargin=TwipsToInches(rtf->IntParam);
       }
    }
    else if (RtfCmp(CurWord,"paperw")==0     // paper width
          || RtfCmp(CurWord,"paperh")==0 ){  // page height
       if (RtfCmp(CurWord,"paperw")==0) rtf->PaperWidth=(int)rtf->IntParam;
       if (RtfCmp(CurWord,"paperh")==0) rtf->PaperHeight=(int)rtf->IntParam;
       SetRtfDocPaperSize(w,rtf);

       if (RtfCmp(CurWord,"paperw")==0) rtf->sect.PprWidth=TwipsToInches(rtf->IntParam);
       if (RtfCmp(CurWord,"paperh")==0) rtf->sect.PprHeight=TwipsToInches(rtf->IntParam);
       SetRtfSectPaperSize(w,rtf);
       rtf->flags2|=RFLAG2_PAPER_DIM_FOUND;
    }
    else if (RtfCmp(CurWord,"landscape")==0){  // landscape
       PaperOrient=rtf->sect.orient=DMORIENT_LANDSCAPE;
    }

    // footnote number format types
    else if (RtfCmp(CurWord,"ftnnar")==0){  // arabic (decimal)
       if (RtfInput<RTF_CB) FootnoteNumFmt=NBR_DEC;
    } 
    else if (RtfCmp(CurWord,"ftnnalc")==0){  // lowercase letter
       if (RtfInput<RTF_CB) FootnoteNumFmt=NBR_LWR_ALPHA;
    } 
    else if (RtfCmp(CurWord,"ftnnauc")==0){  // uppercase letter
       if (RtfInput<RTF_CB) FootnoteNumFmt=NBR_UPR_ALPHA;
    } 
    else if (RtfCmp(CurWord,"ftnnrlc")==0){  // lowercase roman
       if (RtfInput<RTF_CB) FootnoteNumFmt=NBR_LWR_ROMAN;
    } 
    else if (RtfCmp(CurWord,"ftnnruc")==0){  // uppercase roman
       if (RtfInput<RTF_CB) FootnoteNumFmt=NBR_UPR_ROMAN;
    } 
    
    else if (RtfCmp(CurWord,"aftnnar")==0){  // arabic (decimal)
       if (RtfInput<RTF_CB) EndnoteNumFmt=NBR_DEC;
    } 
    else if (RtfCmp(CurWord,"aftnnalc")==0){  // lowercase letter
       if (RtfInput<RTF_CB) EndnoteNumFmt=NBR_LWR_ALPHA;
    } 
    else if (RtfCmp(CurWord,"aftnnauc")==0){  // uppercase letter
       if (RtfInput<RTF_CB) EndnoteNumFmt=NBR_UPR_ALPHA;
    } 
    else if (RtfCmp(CurWord,"aftnnrlc")==0){  // lowercase roman
       if (RtfInput<RTF_CB) EndnoteNumFmt=NBR_LWR_ROMAN;
    } 
    else if (RtfCmp(CurWord,"aftnnruc")==0){  // uppercase roman
       if (RtfInput<RTF_CB) EndnoteNumFmt=NBR_UPR_ROMAN;
    } 

    else if (RtfCmp(CurWord,"aenddoc")==0){  // endnote at the end of the document
       if (RtfInput<RTF_CB) EndnoteAtSect=FALSE;
    } 
    else if (RtfCmp(CurWord,"notabind")==0){  // no tab indent flag
       if (RtfInput<RTF_CB) NoTabIndent=TRUE;
    } 

    // rtl/ltr text flow
    else if (RtfCmp(CurWord,"rtldoc")==0){  // right-to-left
       if (RtfInput<RTF_CB) DocTextFlow=FLOW_RTL;
       rtf->DocFlow=FLOW_RTL;
    } 
    else if (RtfCmp(CurWord,"ltrdoc")==0){  // left-to-right
       if (RtfInput<RTF_CB) DocTextFlow=FLOW_LTR;
       rtf->DocFlow=FLOW_LTR;
    } 
    
    else if (RtfCmp(CurWord,"sspagecb")==0){  // page background color
       if (RtfInput<RTF_CB) {
          struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
          struct StrRtfColor far *color=pRtfGroup->color;

          CurGroup=rtf->GroupLevel;
          if (rtf->param[0]==0) PageBkColor=TextDefBkColor;  // default
          else if (rtf->IntParam>=0 && rtf->IntParam<MaxRtfColors) PageBkColor=color[(int)(rtf->IntParam)].color;
       } 
    } 

    // Store section properties
    else if (RtfCmp(CurWord,"sectd")==0){ // set to default section properties
       FarMove(&TerSect[rtf->InitSect],&(rtf->sect),sizeof(struct StrSect));
       group[CurGroup].BorderType=0;
       for (i=BORDER_SECT_TOP;i<=BORDER_SECT_RIGHT;i++) {
          group[CurGroup].BorderWidth[i]=group[CurGroup].BorderSpace[i]=0;
          group[CurGroup].BorderColor[i]=CLR_AUTO;
       }
       // set default paper size for the section
       if (rtf->PaperWidth>0 && rtf->PaperHeight>0) {  // page information found in rtf stream
          if (PaperOrient!=DMORIENT_LANDSCAPE && PaperOrient!=DMORIENT_PORTRAIT) PaperOrient=DMORIENT_PORTRAIT;
          rtf->sect.PprWidth=TwipsToInches(rtf->PaperWidth);
          rtf->sect.PprHeight=TwipsToInches(rtf->PaperHeight);
          rtf->sect.orient=PaperOrient;
          SetRtfSectPaperSize(w,rtf);
       }
       else if (rtf->sect.orient==DMORIENT_LANDSCAPE) {  // swap width/height because that is how it is found in the rtf section information
          float temp=rtf->sect.PprWidth;
          rtf->sect.PprWidth=rtf->sect.PprHeight;
          rtf->sect.PprHeight=temp;
       }
       rtf->SectFlow=FLOW_DEF;
    }
    else if (RtfCmp(CurWord,"linemod")==0){// enable line numbering
       rtf->sect.flags|=SECT_LINE;
    }
    else if (RtfCmp(CurWord,"marglsxn")==0){// left margin
       if (rtf->IntParam<0) rtf->IntParam=720;
       rtf->sect.LeftMargin=TwipsToInches(rtf->IntParam);
       rtf->flags2|=RFLAG2_LEFT_MARG_FOUND;
    }
    else if (RtfCmp(CurWord,"margrsxn")==0){// right margin
       if (rtf->IntParam<0) rtf->IntParam=720;
       rtf->sect.RightMargin=TwipsToInches(rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"margtsxn")==0){// top margin
       if (rtf->IntParam<0) {
          rtf->IntParam=-rtf->IntParam;
          rtf->sect.flags|=SECT_EXACT_MARGT;
       }
       else ResetUintFlag(rtf->sect.flags,SECT_EXACT_MARGT);

       rtf->sect.TopMargin=TwipsToInches(rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"margbsxn")==0){// bottom margin
       if (rtf->IntParam<0) {
          rtf->IntParam=-rtf->IntParam;
          rtf->sect.flags|=SECT_EXACT_MARGB;
       }
       else ResetUintFlag(rtf->sect.flags,SECT_EXACT_MARGB);

       rtf->sect.BotMargin=TwipsToInches(rtf->IntParam);
    }
    
    else if (RtfCmp(CurWord,"pgbrdrt")==0){   // top border
       group[CurGroup].BorderType=BORDER_SECT_TOP;
    }
    else if (RtfCmp(CurWord,"pgbrdrb")==0){   // bottom border
       group[CurGroup].BorderType=BORDER_SECT_BOT;
    }
    else if (RtfCmp(CurWord,"pgbrdrl")==0){   // left border
       group[CurGroup].BorderType=BORDER_SECT_LEFT;
    }
    else if (RtfCmp(CurWord,"pgbrdrr")==0){   // right border
       group[CurGroup].BorderType=BORDER_SECT_RIGHT;
    }
    else if (RtfCmp(CurWord,"pgbrdrhead")==0){   // border around header
       rtf->sect.flags|=SECT_BRDR_HEADER;
    }
    else if (RtfCmp(CurWord,"pgbrdrfoot")==0){   // border around footer
       rtf->sect.flags|=SECT_BRDR_FOOTER;
    }
    else if (RtfCmp(CurWord,"pgbrdropt")==0){   // border options
       rtf->sect.BorderOpts=(int)(rtf->IntParam);
    }

    else if (RtfCmp(CurWord,"headery")==0){// header distance
       if (rtf->IntParam<0) rtf->IntParam=720;
       rtf->sect.HdrMargin=TwipsToInches(rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"footery")==0){// header distance
       if (rtf->IntParam<0) rtf->IntParam=720;
       rtf->sect.FtrMargin=TwipsToInches(rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"sbknone")==0){// no page break
       rtf->sect.flags=ResetUintFlag(rtf->sect.flags,SECT_NEW_PAGE);
    }
    else if (RtfCmp(CurWord,"sbkpage")==0){// new page
       rtf->sect.flags|=SECT_NEW_PAGE;
    }
    else if (RtfCmp(CurWord,"pgnstarts")==0){// first page number
       rtf->sect.FirstPageNo=(short)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"pgnrestart")==0){  // restart page number
       rtf->sect.flags|=SECT_RESTART_PAGE_NO;
    }
    else if (RtfCmp(CurWord,"titlepg")==0){  // title page
       if (RtfInput<RTF_CB) rtf->sect.flags|=SECT_HAS_TITLE_PAGE;  // create first page header/footer if not found in the file
    }
    else if (RtfCmp(CurWord,"pgncont")==0){  // continuous page numbering
       rtf->sect.FirstPageNo=0;
       rtf->sect.flags=ResetUintFlag(rtf->sect.flags,SECT_RESTART_PAGE_NO);
    }
    else if (RtfCmp(CurWord,"vertal")==0){  // page bottom atligned
       if (!(group[CurGroup].gflags&GFLAG_IN_FRAME)) {
          ResetUintFlag(rtf->sect.flags,(SECT_VALIGN_CTR|SECT_VALIGN_BOT));
          rtf->sect.flags|=SECT_VALIGN_BOT;
       }
    }
    else if (RtfCmp(CurWord,"vertalc")==0){  // page vertically centered
       if (!(group[CurGroup].gflags&GFLAG_IN_FRAME)) {
         ResetUintFlag(rtf->sect.flags,(SECT_VALIGN_CTR|SECT_VALIGN_BOT));
         rtf->sect.flags|=SECT_VALIGN_CTR;
       }
    }
    
    else if (RtfCmp(CurWord,"pgndec")==0){  // page number format - decimal
       rtf->sect.PageNumFmt=NBR_DEC;
    }
    else if (RtfCmp(CurWord,"pgnucrm")==0){  // page number format - uppercase roman
       rtf->sect.PageNumFmt=NBR_UPR_ROMAN;
    }
    else if (RtfCmp(CurWord,"pgnlcrm")==0){  // page number format - lowercase roman
       rtf->sect.PageNumFmt=NBR_LWR_ROMAN;
    }
    else if (RtfCmp(CurWord,"pgnucltr")==0){  // page number format - uppercase letter
       rtf->sect.PageNumFmt=NBR_UPR_ALPHA;
    }
    else if (RtfCmp(CurWord,"pgnlcltr")==0){  // page number format - lowercase letter
       rtf->sect.PageNumFmt=NBR_LWR_ALPHA;
    }
    else if (RtfCmp(CurWord,"cols")==0){   // number of columns
       rtf->sect.columns=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"colsx")==0){  // space between columns
       rtf->sect.ColumnSpace=TwipsToInches(rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"pgwsxn")==0     // section paper width
          || RtfCmp(CurWord,"pghsxn")==0 ){  // section page height
       if (RtfCmp(CurWord,"pgwsxn")==0) rtf->sect.PprWidth=TwipsToInches((int)rtf->IntParam);
       if (RtfCmp(CurWord,"pghsxn")==0) rtf->sect.PprHeight=TwipsToInches((int)rtf->IntParam);
       SetRtfSectPaperSize(w,rtf);
       rtf->flags2|=RFLAG2_PAPER_DIM_FOUND;
    }
    else if (RtfCmp(CurWord,"lndscpsxn")==0){  // section landscape
       rtf->sect.orient=DMORIENT_LANDSCAPE;
    }
    else if (RtfCmp(CurWord,"binfsxn")==0){    // first page bin
       rtf->sect.FirstPageBin=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"binsxn")==0){    // remaining page bin
       rtf->sect.bin=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"rtlsect")==0){    // rtl text flow
       rtf->sect.flow=FLOW_RTL;
       rtf->SectFlow=FLOW_RTL;
    }
    else if (RtfCmp(CurWord,"ltrsect")==0){    // ltr text flow
       rtf->sect.flow=FLOW_LTR;
       rtf->SectFlow=FLOW_LTR;
    }

    // Handle section break
    else if (RtfCmp(CurWord,"sect")==0){   // end previous section
       if (!(group[CurGroup].InTable) && !RtfInHdrFtr) {
          if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any

          rtf->OutBuf[0]=SECT_CHAR;
          rtf->OutBuf[1]=0;
          rtf->OutBufLen=1;
          if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send the section break character

          SetRtfParaDefault(w,rtf,group);   // set paragraph default
          
          rtf->flags=ResetUintFlag(rtf->flags,(RFLAG_SECT_HDR_FOUND|RFLAG_SECT_FTR_FOUND)); //reset the header/footer flag
          rtf->flags1=ResetUintFlag(rtf->flags1,(RFLAG1_SECT_FHDR_FOUND|RFLAG1_SECT_FFTR_FOUND)); //reset the first page header/footer flag
       }
    }

    // page header/footer
    else if (  RtfCmp(CurWord,"header")==0     // page header
            || RtfCmp(CurWord,"headerl")==0    // left page header
            || RtfCmp(CurWord,"headerr")==0){  // right page header
       if (rtf->flags&RFLAG_SECT_HDR_FOUND || rtf->flags1&RFLAG1_PASTING_IN_PARA_FRAME)
          ReturnCode=SkipRtfGroup(w,rtf);  // header already processed
       else {
          if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any
          CurGroup=rtf->GroupLevel;
          group[CurGroup].flags=group[CurGroup].flags|PAGE_HDR;
          rtf->OutBuf[0]=rtf->HdrFtrChar=HDR_CHAR;
          rtf->OutBuf[1]=0;
          rtf->OutBufLen=1;
          if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send the section break character

          RtfInHdrFtr=PAGE_HDR;
          rtf->flags=ResetUintFlag(rtf->flags,RFLAG_HDRFTR_TEXT_FOUND);  // to ensure that rtf section should have some text
          rtf->flags|=(RFLAG_HDR_FTR_FOUND|RFLAG_SECT_HDR_FOUND);
          rtf->flags1|=RFLAG1_NEW_HDR_FTR;
      }
    }
    else if (RtfCmp(CurWord,"headerf")==0) {     // first page page header
       if (rtf->flags1&RFLAG1_SECT_FHDR_FOUND || rtf->flags1&RFLAG1_PASTING_IN_PARA_FRAME)
          ReturnCode=SkipRtfGroup(w,rtf);  // header already processed
       else {
          if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any
          CurGroup=rtf->GroupLevel;
          group[CurGroup].flags=group[CurGroup].flags|PAGE_HDR;
          rtf->OutBuf[0]=rtf->HdrFtrChar=FHDR_CHAR;
          rtf->OutBuf[1]=0;
          rtf->OutBufLen=1;
          if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send the section break character

          RtfInHdrFtr=PAGE_HDR;
          rtf->flags=ResetUintFlag(rtf->flags,RFLAG_HDRFTR_TEXT_FOUND);  // to ensure that rtf section should have some text
          rtf->flags|=RFLAG_HDR_FTR_FOUND;
          rtf->flags1|=(RFLAG1_SECT_FHDR_FOUND|RFLAG1_NEW_HDR_FTR);
      }
    }
    else if (  RtfCmp(CurWord,"footer")==0     // page footer
            || RtfCmp(CurWord,"footerl")==0    // left page footer
            || RtfCmp(CurWord,"footerr")==0){  // right page footer
       if (rtf->flags&RFLAG_SECT_FTR_FOUND || rtf->flags1&RFLAG1_PASTING_IN_PARA_FRAME) 
          SkipRtfGroup(w,rtf);  // footer already processed
       else {
          if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any
          CurGroup=rtf->GroupLevel;
          group[CurGroup].flags=group[CurGroup].flags|PAGE_FTR;
          rtf->OutBuf[0]=rtf->HdrFtrChar=FTR_CHAR;
          rtf->OutBuf[1]=0;
          rtf->OutBufLen=1;
          if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send the section break character

          RtfInHdrFtr=PAGE_FTR;
          rtf->flags=ResetUintFlag(rtf->flags,RFLAG_HDRFTR_TEXT_FOUND);  // to ensure that rtf section should have some text
          rtf->flags|=(RFLAG_HDR_FTR_FOUND|RFLAG_SECT_FTR_FOUND);
          rtf->flags1|=RFLAG1_NEW_HDR_FTR;
       }
    }
    else if (  RtfCmp(CurWord,"footerf")==0) { // first page footer
       if (rtf->flags1&RFLAG1_SECT_FFTR_FOUND || rtf->flags1&RFLAG1_PASTING_IN_PARA_FRAME)
          SkipRtfGroup(w,rtf);  // footer already processed
       else {
          if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any
          CurGroup=rtf->GroupLevel;
          group[CurGroup].flags=group[CurGroup].flags|PAGE_FTR;
          rtf->OutBuf[0]=rtf->HdrFtrChar=FFTR_CHAR;
          rtf->OutBuf[1]=0;
          rtf->OutBufLen=1;
          if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send the section break character

          RtfInHdrFtr=PAGE_FTR;
          rtf->flags=ResetUintFlag(rtf->flags,RFLAG_HDRFTR_TEXT_FOUND);  // to ensure that rtf section should have some text
          rtf->flags|=RFLAG_HDR_FTR_FOUND;
          rtf->flags1|=(RFLAG1_NEW_HDR_FTR|RFLAG1_SECT_FFTR_FOUND);
       }
    }

    // unicode tags
    else if (RtfCmp(CurWord,"uc")==0) {
       if (Win32) group[CurGroup].UcIgnoreCount=(int)(rtf->IntParam);  // number of regular characters to be ignored
    } 
    else if (RtfCmp(CurWord,"u")==0) {         // unicode character
       if (Win32 && !(group[CurGroup].gflags&GFLAG_NO_UNICODE)) {
          WORD uchr,UcBase;
          BYTE chr;
          uchr=(WORD)(DWORD)(rtf->IntParam);
          if (TRUE || uchr>=256) {
             CrackUnicode(w,uchr,&UcBase,&chr);          // crach unicode to base and chracter
             if (UcBase!=group[CurGroup].UcBase || UcBase==0 || (rtf->OutBufLen+1)>RTF_LINE_LEN) {
                if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any
                group[CurGroup].UcBase=UcBase;         // unicode base
             }
       
             rtf->OutBuf[rtf->OutBufLen]=chr;          // regular character component
             rtf->OutBufLen++;
             rtf->OutBuf[rtf->OutBufLen]=0;

             group[CurGroup].IgnoreCount=group[CurGroup].UcIgnoreCount;  // ignore the next character
          }
       }
    } 

    // Read font table, color table
    else if (RtfCmp(CurWord,"fonttbl")==0) {
       ReturnCode=ReadRtfFontTable(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }
    else if (RtfCmp(CurWord,"colortbl")==0) {
       ReturnCode=ReadRtfColorTable(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }
    else if (RtfCmp(CurWord,"deff")==0) { // register the default font
       int CurFont,FontId=(int)(rtf->IntParam);
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);

       CurGroup=rtf->GroupLevel;
       if (FontId>=0 && FontId<MAX_RTF_SEQ_FONTS) CurFont=FontId;  // lower font ids are stored sequentially
       else for (CurFont=MAX_RTF_SEQ_FONTS;CurFont<pRtfGroup->MaxRtfFonts;CurFont++) {
          if (pRtfGroup->font[CurFont].InUse && pRtfGroup->font[CurFont].FontId==FontId) break;
       }

       group[CurGroup].DefFont=CurFont;
    }

    // stylesheet controls
    else if (RtfCmp(CurWord,"stylesheet")==0) {  // begins the stylesheet group
       group[CurGroup].gflags|=GFLAG_IN_STYLESHEET;
    }
    else if (RtfCmp(CurWord,"s")==0) {     // paragraph type stylesheet
       if (rtf->IntParam<0) rtf->IntParam=0;
       if (group[CurGroup].gflags&GFLAG_IN_STYLESHEET) group[CurGroup].ParaStyId=(int)(rtf->IntParam);
       else InitGroupFromStyle(w,rtf,(int)(rtf->IntParam),SSTYPE_PARA,FALSE);
    }
    else if (RtfCmp(CurWord,"cs")==0) {    // character type stylesheet
       if (group[CurGroup].gflags&GFLAG_IN_STYLESHEET) {
          group[CurGroup].CharStyId=(int)(rtf->IntParam);
          group[CurGroup].gflags|=GFLAG_CHAR_STYLE;
       }
       else InitGroupFromStyle(w,rtf,(int)(rtf->IntParam),SSTYPE_CHAR,FALSE);
    }
    else if (RtfCmp(CurWord,"snext")==0) {    // next style
       if (group[CurGroup].gflags&GFLAG_IN_STYLESHEET) {
          group[CurGroup].NextStyId=(int)(rtf->IntParam);
       }
    }


    // Read object data
    else if (RtfCmp(CurWord,"object")==0) {
       ReturnCode=ReadRtfObject(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }

    // Read shape data
    else if (RtfCmp(CurWord,"shp")==0){                       // shape
       rtf->ShpGroup=CurGroup;
       group[CurGroup].gflags|=GFLAG_IN_SHP;
    }
    else if (RtfCmp(CurWord,"shpgrp")==0){                       // shape
       rtf->ShpGroup=CurGroup;
      group[CurGroup].gflags|=GFLAG_IN_SHPGRP;
    }
    else if (RtfCmp(CurWord,"shpinst")==0) {
      BOOL ReadShape=true;
      //if (True(group[CurGroup].gflags&GFLAG_IN_FRAME)) ReadShape=false;
      if (ReadShape) {
         if (True(group[CurGroup].gflags&GFLAG_IN_SHP)) ReadRtfShape(w,rtf);
         else if (True(group[CurGroup].gflags&GFLAG_IN_SHPGRP)) ReadRtfShpGrp(w,rtf);
      }
    }
    else if (RtfCmp(CurWord,"shprslt")==0) {
       if ((group[CurGroup].gflags&GFLAG_IN_FRAME) && False(group[CurGroup].gflags&GFLAG_IN_SHP)) {  // this shp was found within a frame
          ExtractRtfPict(w,rtf);                       // extract the picture and put it in the current frame
       }
       else if (True(group[CurGroup].gflags&GFLAG_IN_SHPGRP)) SkipRtfGroup(w,rtf);  // do not read shpgrp result
       else if (!(rtf->flags&RFLAG_READ_SHAPE_PICT)) SkipRtfGroup(w,rtf);   // shpinst picture was read instead, so ignore this group
       else if (FALSE && rtf->flags1&RFLAG1_PNG_FOUND) {   // extract only the picture from this group to substiture for the png format picture that we don't yet support
          ExtractRtfPict(w,rtf);
       }
       else {
         if (!group[CurGroup].InTable && RtfCurRowId>0) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
         if (RtfCurRowId>0) SkipRtfGroup(w,rtf);         // frame inside table not supported
         else group[CurGroup].gflags|=GFLAG_IN_SHPRSLT;  // in shape result
       }
    }

    // Read shape picture data
    else if (RtfCmp(CurWord,"shppict")==0) {
       if (group[CurGroup].gflags&GFLAG_IN_SHP) {
          if (TRUE || !(group[CurGroup].gflags&GFLAG_IN_FRAME)) ReadRtfShape(w,rtf);
       } 
       else rtf->flags|=RFLAG_READ_SHAPE_PICT;
    }
    else if (RtfCmp(CurWord,"nonshppict")==0) {
       if (!(rtf->flags&RFLAG_READ_SHAPE_PICT)) SkipRtfGroup(w,rtf);   // shppict was read instead, so ignore this group
    }
    else if (RtfCmp(CurWord,"sn")==0) {   // shape filled with picture
       BYTE PropName[MAX_WIDTH];
       ReadRtfShapeParam(w,rtf,PropName);
       if (lstrcmpi(PropName,"fillBlip")==0) {
         if (CurGroup>0) group[CurGroup-1].shape.FrmFlags|=PARA_FRAME_FILL_PICT;  // sp group
         group[CurGroup].shape.FrmFlags|=PARA_FRAME_FILL_PICT;
       }
    } 
    else if (RtfCmp(CurWord,"sv")==0) {      // shape property
       if (rtf->flags&RFLAG_READ_SHAPE_PICT) {
          ReadRtfShapeProp(w,rtf,"pict");
          if (True(group[CurGroup].shape.FrmFlags&PARA_FRAME_FILL_PICT) && group[CurGroup].shape.FillPict>0) {
              if (RtfParaFID>0) {
                 ParaFrame[RtfParaFID].ShapeType=0;
                 ResetUintFlag(ParaFrame[RtfParaFID].flags,PARA_FRAME_BOXED);
                 ParaFrame[RtfParaFID].flags|=PARA_FRAME_FILL_PICT;
                 ParaFrame[RtfParaFID].FillPict=group[CurGroup].shape.FillPict;
              }
          }
       }
       else SkipRtfGroup(w,rtf);   // shppict was read instead, so ignore this group
    }

    // Read animation information
    else if ( RtfCmp(CurWord,"ssanimseq")==0) {
       group[CurGroup].gflags|=GFLAG_ANIM_SEQ;
    }
    else if ( RtfCmp(CurWord,"ssanimloops")==0) {  // animatino loop count
       group[CurGroup].AnimLoops=rtf->IntParam;
       group[CurGroup].gflags|=GFLAG_ANIM_INFO;    // animation information found
    }
    else if ( RtfCmp(CurWord,"ssanimdelay")==0) {  // animation delay
       group[CurGroup].AnimDelay=(int)(rtf->IntParam);
       group[CurGroup].gflags|=GFLAG_ANIM_INFO;    // animation information found
    }

    // Read picture data
    else if ( RtfCmp(CurWord,"pict")==0
           || RtfCmp(CurWord,"sscontrol")==0 ){
       ReturnCode=ReadRtfPicture(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }
    else if ( RtfCmp(CurWord,"sslinkpictw")==0) {  // width of the following linked picture
       group[CurGroup].LinkPictWidth=(int)(rtf->IntParam);
    }
    else if ( RtfCmp(CurWord,"sslinkpicth")==0) {  // width of the following linked picture
       group[CurGroup].LinkPictHeight=(int)(rtf->IntParam);
    }

    else if (RtfCmp(CurWord,"subpictid")==0) { // stored just the picture id
       if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any
       CurGroup=rtf->GroupLevel;
       group[CurGroup].PictId=(int)rtf->IntParam;
       rtf->OutBuf[0]='-';                        // picture place holder
       rtf->OutBuf[1]=0;
       rtf->OutBufLen=1;
       if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send the section break character
       group[CurGroup].PictId=0;                  // reset the picture id
    }

    // Set table-of-content tagsCharacter properties
    else if (RtfCmp(CurWord,"tc")==0) {            //  field TC
       group[CurGroup].FieldId=FIELD_TC;
       if (rtf->FieldCode) {                       //  prepare field code string to accept field code for this field
          OurFree(rtf->FieldCode);
          rtf->FieldCode=NULL;
       }
       ResetUintFlag(group[CurGroup].style,HIDDEN);    // any hidden style outside the group not applicable to  'tc' text
    }
    else if (RtfCmp(CurWord,"tcl")==0) {           //  tc level
       if (group[CurGroup].FieldId==FIELD_TC) {
          BYTE str[10];
          int i,len;
          wsprintf(str,"\\tcl%d ",(int)rtf->IntParam);
          len=lstrlen(str);
          rtf->FieldCode=OurAlloc(len+1);
          lstrcpy(rtf->FieldCode,str);
          
          for (i=CurGroup;i>=0 && group[i].FieldId==FIELD_TC;i--)  // do it for all group till the parent tc group beginning
              ResetUintFlag(group[i].style,HIDDEN);    // any hidden style outside the group not applicable to  'tc' text
       }
    }

    // Index entry field
    else if (RtfCmp(CurWord,"xe")==0) {        //  field XE
       if (rtf->PrevField==FIELD_XE) {                // insert a space separator
           rtf->OutBuf[rtf->OutBufLen]=' ';    // font does not need to be specified because previous field terminated descending down to the parent group
           rtf->OutBufLen++;
           rtf->OutBuf[rtf->OutBufLen]=0;
           if (!SendRtfText(w,rtf)) return RTF_END_FILE;
       } 
       group[CurGroup].FieldId=FIELD_XE;
    }

    // Set Character properties
    else if (RtfCmp(CurWord,"plain")==0) { // reset fonts
       SetRtfFontDefault(w,rtf,group);
    }
    else if (RtfCmp(CurWord,"loch")==0) {   // one byte low ascii characters
       group[CurGroup].CharType=CHAR_LO;
    }
    else if (RtfCmp(CurWord,"hich")==0) {   // one byte high ascii characters
       group[CurGroup].CharType=CHAR_HI;
    }
    else if (RtfCmp(CurWord,"dbch")==0) {   // one byte high ascii characters
       group[CurGroup].CharType=CHAR_DB;    // double byte characters
    }
    else if (RtfCmp(CurWord,"f")==0 /*|| RtfCmp(CurWord,"af")==0*/) {   // new font
       int CurFont,FontId=(int)(rtf->IntParam);
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);

       CurGroup=rtf->GroupLevel;
       if (FontId>=0 && FontId<MAX_RTF_SEQ_FONTS) CurFont=FontId;  // lower font ids are stored sequentially
       else for (CurFont=MAX_RTF_SEQ_FONTS;CurFont<pRtfGroup->MaxRtfFonts;CurFont++) {
          if (pRtfGroup->font[CurFont].InUse && pRtfGroup->font[CurFont].FontId==FontId) break;
       }

       lstrcpy(group[CurGroup].TypeFace,pRtfGroup->font[CurFont].name); // font name
       lstrcpy(group[CurGroup].FontFamily,pRtfGroup->font[CurFont].family); // font name
       group[CurGroup].CharSet=pRtfGroup->font[CurFont].CharSet;
    }
    else if (RtfCmp(CurWord,"fs")==0) {  // new point size
       CurGroup=rtf->GroupLevel;
       group[CurGroup].PointSize2=(int)(rtf->IntParam);// point size
    }
    else if (RtfCmp(CurWord,"sscharaux")==0) {  // character aux id
       CurGroup=rtf->GroupLevel;
       if (!HtmlMode && group[CurGroup].FieldId!=FIELD_TEXTBOX) group[CurGroup].AuxId=(int)(rtf->IntParam); // aux id , text box uses AuxId as field id, so don't change it here
    }
    else if (RtfCmp(CurWord,"cf")==0) {  // new text color
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
       struct StrRtfColor far *color=pRtfGroup->color;

       CurGroup=rtf->GroupLevel;
       if (rtf->param[0]==0) rtf->IntParam=0;  // default
       if (rtf->IntParam>=0 && rtf->IntParam<MaxRtfColors) group[CurGroup].TextColor=color[(int)(rtf->IntParam)].color;
    }
    else if (RtfCmp(CurWord,"ulc")==0) {  // underline color
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
       struct StrRtfColor far *color=pRtfGroup->color;

       CurGroup=rtf->GroupLevel;
       if (rtf->param[0]==0) rtf->IntParam=0;  // default
       if (rtf->IntParam>=0 && rtf->IntParam<MaxRtfColors) group[CurGroup].UlineColor=color[(int)(rtf->IntParam)].color;
    }
    else if (   RtfCmp(CurWord,"chbghoriz")==0
             || RtfCmp(CurWord,"chbgvert")==0
             || RtfCmp(CurWord,"chbgfdiag")==0
             || RtfCmp(CurWord,"chbgbdiag")==0
             || RtfCmp(CurWord,"chbgcross")==0
             || RtfCmp(CurWord,"chbgdcross")==0
             || RtfCmp(CurWord,"chbgdkhoriz")==0
             || RtfCmp(CurWord,"chbgdkvert")==0
             || RtfCmp(CurWord,"chbgdkfdiag")==0
             || RtfCmp(CurWord,"chbgdkbdiag")==0
             || RtfCmp(CurWord,"chbgdkcross")==0
             || RtfCmp(CurWord,"chbgdkdcross")==0) { // character background pattern specified
       
       group[CurGroup].CharBkPat=PAT_OTHER;

    }
    else if (RtfCmp(CurWord,"chcfpat")==0 || RtfCmp(CurWord,"chcbpat")==0) {  // charcter background pattern color - foreground color
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
       struct StrRtfColor far *color=pRtfGroup->color;

       CurGroup=rtf->GroupLevel;
       if (rtf->param[0]==0) rtf->IntParam=0;  // default
       if (rtf->IntParam>=0 && rtf->IntParam<MaxRtfColors) {
          if (RtfCmp(CurWord,"chcfpat")==0) group[CurGroup].CharPatFC=color[(int)(rtf->IntParam)].color;
          else                              group[CurGroup].CharPatBC=color[(int)(rtf->IntParam)].color;
       }
    } 

    else if (RtfCmp(CurWord,"cb")==0
          || RtfCmp(CurWord,"highlight")==0 ){
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
       struct StrRtfColor far *color=pRtfGroup->color;

       CurGroup=rtf->GroupLevel;
       if (rtf->param[0]==0) rtf->IntParam=0;  // default
       if (rtf->IntParam==0) group[CurGroup].TextBkColor=TextDefBkColor;
       else if (rtf->IntParam>=0 && rtf->IntParam<MaxRtfColors) group[CurGroup].TextBkColor=color[(int)(rtf->IntParam)].color;
    }
    else if (RtfCmp(CurWord,"b")==0) {   // bold
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|BOLD;
       else {
          group[CurGroup].style=group[CurGroup].style&(~(UINT)BOLD);
          group[CurGroup].StyleOff|=BOLD;    // style explictly turned off
       }
    }
    else if (RtfCmp(CurWord,"caps")==0 && !Win32) { // capital letter
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].gflags|=GFLAG_CAPS;
       else               ResetLongFlag(group[CurGroup].gflags,GFLAG_CAPS);
    }
    else if (RtfCmp(CurWord,"sshlink")==0) { // subsystem's hyperlink style
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|HLINK;
       else               group[CurGroup].style=group[CurGroup].style&(~(UINT)HLINK);
    }
    else if (RtfCmp(CurWord,"i")==0) {   // italic
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|ITALIC;
       else  {
          group[CurGroup].style=group[CurGroup].style&(~(UINT)ITALIC);
          group[CurGroup].StyleOff|=ITALIC;    // style explictly turned off
       }
    }
    else if (RtfCmp(CurWord,"v")==0) {   // hidden text
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam && group[CurGroup].FieldId==FIELD_TC) {
          int i;
          for (i=CurGroup;i>=0 && group[i].FieldId==FIELD_TC;i--)  // do it for all group till the parent tc group beginning
              group[i].style|=HIDDEN; 
       }
       else { 
         if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|HIDDEN;
         else               group[CurGroup].style=group[CurGroup].style&(~(UINT)HIDDEN);
       }
    }
    else if (RtfCmp(CurWord,"dn")==0) {   // subscript
       CurGroup=rtf->GroupLevel;
       if (lstrcmpi(rtf->param,"0")==0) group[CurGroup].offset=0;   // set to 0
       else {
          if (rtf->IntParam==0) rtf->IntParam=6;   // rtf default
          group[CurGroup].offset=-PointsToTwips((int)rtf->IntParam)/2;
       }
    }
    else if (RtfCmp(CurWord,"sub")==0) {     // subscript w/ font reduction
       CurGroup=rtf->GroupLevel;
       group[CurGroup].style=group[CurGroup].style|SUBSCR;
    }
    else if (RtfCmp(CurWord,"nosupersub")==0) {     // turnoff superscript subscripting
       group[CurGroup].style=ResetUintFlag(group[CurGroup].style,(SUBSCR|SUPSCR));
    }
    else if (RtfCmp(CurWord,"strike")==0) {   // subscript
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|STRIKE;
       else {
          group[CurGroup].style=group[CurGroup].style&(~(UINT)STRIKE);
          group[CurGroup].StyleOff|=STRIKE;    // style explictly turned off
       }
    }
    else if (RtfCmp(CurWord,"protect")==0) {   // protected text
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|PROTECT;
       else {
          group[CurGroup].style=group[CurGroup].style&(~(UINT)PROTECT);
          group[CurGroup].StyleOff|=PROTECT;    // style explictly turned off
       }
    }
    else if ( RtfCmp(CurWord,"ul")==0
           || RtfCmp(CurWord,"uld")==0
           || RtfCmp(CurWord,"ulw")==0  ) {   // underline on/off
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|ULINE;
       else {
          group[CurGroup].style=group[CurGroup].style&(~(UINT)ULINE);
          group[CurGroup].StyleOff|=ULINE;    // style explictly turned off
       }
    }
    else if (RtfCmp(CurWord,"uldb")==0 ) {   // double underline on/off
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|ULINED;
       else {
          group[CurGroup].style=group[CurGroup].style&(~(UINT)ULINED);
          group[CurGroup].StyleOff|=ULINED;    // style explictly turned off
       }
    }
    else if (RtfCmp(CurWord,"ulnone")==0) {   // all underline off
       CurGroup=rtf->GroupLevel;
       group[CurGroup].style=group[CurGroup].style&(~((UINT)ULINE|ULINED));
       group[CurGroup].StyleOff|=(ULINE|ULINED);    // style explictly turned off
    }
    else if (RtfCmp(CurWord,"expnd")==0) {   // character space expansion
       group[CurGroup].expand=(int)(rtf->IntParam*20/4);  // specified in 1/4 of points
    }
    else if (RtfCmp(CurWord,"expndtw")==0) { // character space expansion
       group[CurGroup].expand=(int)(rtf->IntParam);       // specified in twips
    }
    else if (RtfCmp(CurWord,"charscalex")==0) {   // character scaling - limited support
       group[CurGroup].CharScaleX=(int)(rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"up")==0) {        // super script
       CurGroup=rtf->GroupLevel;
       if (lstrcmpi(rtf->param,"0")==0) group[CurGroup].offset=0;   // set to 0
       else {
          if (rtf->IntParam==0) rtf->IntParam=6;   // rtf default
          group[CurGroup].offset=PointsToTwips((int)rtf->IntParam)/2;
       }
    }
    else if (RtfCmp(CurWord,"super")==0) {     // super script w/ font reduction
       CurGroup=rtf->GroupLevel;
       group[CurGroup].style=group[CurGroup].style|SUPSCR;
    }
    else if (RtfCmp(CurWord,"scaps")==0
          || RtfCmp(CurWord,"caps")==0 ) {     // one-time conversion only
       CurGroup=rtf->GroupLevel;
       if (Win32) {
          UINT style=(RtfCmp(CurWord,"scaps")==0)?SCAPS:CAPS;
          if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|style;
          else {
             group[CurGroup].style=group[CurGroup].style&(~(UINT)style);
             group[CurGroup].StyleOff|=style;    // style explictly turned off
          }
       }
       else {
          if (rtf->IntParam) group[CurGroup].caps=TRUE;
          else               group[CurGroup].caps=FALSE;
       } 
    }

    // Translate tab character
    else if (RtfCmp(CurWord,"tab")==0) {// insert a tab
       rtf->OutBuf[rtf->OutBufLen]=9;      // ASC9 is tab
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;      // NULL terination
    }

    else if (RtfCmp(CurWord,"chpgn")==0) {// print page number
       if (rtf->OutBufLen>0) if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE; // send previous text if any
       rtf->OutBuf[0]='1';
       rtf->OutBuf[1]=0;
       rtf->OutBufLen=1;
       rtf->group[CurGroup].FieldId=FIELD_PAGE_NUMBER;
       if (!SendRtfText(w,rtf)) return RTF_FILE_INCOMPLETE;    // send the page number
       rtf->group[CurGroup].FieldId=0;
    }

    // Translate hard page break character
    else if (RtfCmp(CurWord,"page")==0) {// insert a page break
       if (!(group[CurGroup].InTable) && !RtfInHdrFtr) {
          rtf->OutBuf[rtf->OutBufLen]=PAGE_CHAR;   // page break character
          rtf->OutBufLen++;
          rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
          if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send page break
       }
    }

    // Translate hard column break character
    else if (RtfCmp(CurWord,"column")==0) {// insert a column break
       if (!(group[CurGroup].InTable)) {
          rtf->OutBuf[rtf->OutBufLen]=COL_CHAR;    // column break character
          rtf->OutBufLen++;
          rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
       }
    }
    
    else if ( RtfCmp(CurWord,"zwnj")==0) {   //  zero-width non-joiner
       rtf->OutBuf[rtf->OutBufLen]=ZWNJ_CHAR; 
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;          // NULL termination
       if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send this line
    }


    // Paragraph properties
    else if ( RtfCmp(CurWord,"par")==0   //  new paragraph
           || RtfCmp(CurWord,"line")==0  // line break
           || RtfCmp(CurWord,"lbr")==0 ) {// spcial line break

       if (group[CurGroup].UcBase!=0 && rtf->OutBufLen>0) {
          if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send previous text if any
          group[CurGroup].UcBase=0;         // regular character
       }

       if (RtfCmp(CurWord,"par")==0)
          rtf->OutBuf[rtf->OutBufLen]=ParaChar;   // insert a para marker
       else  {
          rtf->OutBuf[rtf->OutBufLen]=LINE_CHAR;  // line break
          if (RtfCmp(CurWord,"lbr")==0) group[CurGroup].CharId=(int)(rtf->IntParam);
       }
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;          // NULL termination
       if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send this line
       group[CurGroup].CharId=0;               // reset
    }
    else if (RtfCmp(CurWord,"pard")==0){// reset para properties
       SetRtfParaDefault(w,rtf,group);
    }
    else if (RtfCmp(CurWord,"ql")==0){     // left aligned
       CurGroup=rtf->GroupLevel;
       group[CurGroup].flags=group[CurGroup].flags&(~((UINT)(CENTER|RIGHT_JUSTIFY|JUSTIFY)));
       group[CurGroup].flags=group[CurGroup].flags|LEFT;
    }
    else if (RtfCmp(CurWord,"qr")==0){     // right justifield
       CurGroup=rtf->GroupLevel;
       group[CurGroup].flags=group[CurGroup].flags&(~((UINT)(LEFT|CENTER|JUSTIFY)));
       group[CurGroup].flags=group[CurGroup].flags|RIGHT_JUSTIFY;
    }
    else if (RtfCmp(CurWord,"qc")==0){     // center text
       CurGroup=rtf->GroupLevel;
       group[CurGroup].flags=group[CurGroup].flags&(~((UINT)(LEFT|RIGHT_JUSTIFY|JUSTIFY)));
       group[CurGroup].flags=group[CurGroup].flags|CENTER;
    }
    else if (RtfCmp(CurWord,"rtlpar")==0){     // right-to-left para
       group[CurGroup].flow=FLOW_RTL;
    }
    else if (RtfCmp(CurWord,"ltrpar")==0){     // left-to-right para
       group[CurGroup].flow=FLOW_LTR;
    }
    else if ( RtfCmp(CurWord,"qj")==0 
           || RtfCmp(CurWord,"qk")==0){     // justified text
       CurGroup=rtf->GroupLevel;
       group[CurGroup].flags=group[CurGroup].flags&(~((UINT)(LEFT|RIGHT_JUSTIFY|CENTER)));
       group[CurGroup].flags=group[CurGroup].flags|JUSTIFY;
    }
    else if (RtfCmp(CurWord,"fi")==0){     // first line indent
       CurGroup=rtf->GroupLevel;
       group[CurGroup].FirstIndent=(int)(rtf->IntParam); // twips
    }
    else if (RtfCmp(CurWord,"li")==0){            // left indent
       CurGroup=rtf->GroupLevel;
       group[CurGroup].LeftIndent=(int)(rtf->IntParam);  // twips
    }
    else if (RtfCmp(CurWord,"ri")==0){            // left indent
       CurGroup=rtf->GroupLevel;
       group[CurGroup].RightIndent=(int)(rtf->IntParam); // twips
       if (group[CurGroup].RightIndent>32000) group[CurGroup].RightIndent=0;  // MS Rich Text Box sometimes emits a large value
    }
    else if (RtfCmp(CurWord,"sb")==0){            // space before paragraph
       CurGroup=rtf->GroupLevel;
       group[CurGroup].SpaceBefore=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"sbauto")==0){        // space before is auto - we will use a fixed value
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam==1) group[CurGroup].SpaceBefore=270;
    }
    else if (RtfCmp(CurWord,"sa")==0){            // space after paragraph
       CurGroup=rtf->GroupLevel;
       group[CurGroup].SpaceAfter=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"saauto")==0){        // space after is auto - we will use a fixed value
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam==1) group[CurGroup].SpaceAfter=270;
    }
    else if (RtfCmp(CurWord,"sl")==0){            // variable line spacing
       group[CurGroup].flags=ResetUintFlag(group[CurGroup].flags,DOUBLE_SPACE);
       group[CurGroup].SpaceBetween=(int)(rtf->IntParam);
       if (group[CurGroup].SpaceBetween==1) group[CurGroup].SpaceBetween=0;  // single line spacing
    }
    else if (RtfCmp(CurWord,"slmult")==0){        // double line spacing
       CurGroup=rtf->GroupLevel;
       if (rtf->IntParam==1L) {
          if (group[CurGroup].SpaceBetween==480) {
             group[CurGroup].flags|=DOUBLE_SPACE;
             group[CurGroup].LineSpacing=0;
          }
          else if (group[CurGroup].SpaceBetween>0) {
             group[CurGroup].LineSpacing=MulDiv(group[CurGroup].SpaceBetween-240,100,240);  // 0=single line, 100=double line
             ResetUintFlag(group[CurGroup].flags,DOUBLE_SPACE);
          } 
          group[CurGroup].SpaceBetween=0;                // reset current value
       }
    }
    else if (RtfCmp(CurWord,"keep")==0){         // keep together
       CurGroup=rtf->GroupLevel;
       group[CurGroup].flags|=PARA_KEEP;
    }
    else if (RtfCmp(CurWord,"keepn")==0){        // keep with next together
       CurGroup=rtf->GroupLevel;
       group[CurGroup].flags|=PARA_KEEP_NEXT;
    }
    else if (RtfCmp(CurWord,"widowctrl")==0){    // widow/orphan at document level
       rtf->SetWidowOrphan=TRUE;
       group[CurGroup].pflags|=PFLAG_WIDOW;
    }
    else if (RtfCmp(CurWord,"widctlpar")==0){    // widow/orphan at para graph level
       group[CurGroup].pflags|=PFLAG_WIDOW;
    }
    else if (RtfCmp(CurWord,"ssparnw")==0){      // paragraph no wrap
       group[CurGroup].pflags|=PFLAG_NO_WRAP;
    }
    else if (RtfCmp(CurWord,"nowidctlpar")==0){    // widow/orphan at para graph level
       group[CurGroup].pflags=ResetUintFlag(group[CurGroup].pflags,PFLAG_WIDOW);
    }
    else if (RtfCmp(CurWord,"pagebb")==0){        // page break before paragraph
       group[CurGroup].pflags|=PFLAG_PAGE_BREAK;
    }
    else if (RtfCmp(CurWord,"outlinelevel")==0){  // outline level
       group[CurGroup].OutlineLevel=(int)(rtf->IntParam);
    }

    // paragraph bullet
    else if (RtfCmp(CurWord,"pn")==0) { // paragraph bullet and numbering
       ReturnCode=ReadRtfBullet(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }

    // list table
    else if (RtfCmp(CurWord,"listtable")==0) { // extract the list table
       group[CurGroup].flags|=GFLAG_IN_LIST;
       ReturnCode=ReadRtfList(w,rtf);                 // read the list table
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    } 
    else if (RtfCmp(CurWord,"listoverridetable")==0) { // extract the list override table
       group[CurGroup].flags|=GFLAG_IN_LIST_OR;
       ReturnCode=ReadRtfListOr(w,rtf);                 // read the list table
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    } 
    
    else if (RtfCmp(CurWord,"ls")==0) { // list number 
       group[CurGroup].RtfLs=(int)rtf->IntParam;
    } 
    else if (RtfCmp(CurWord,"ilvl")==0) { // level number
       group[CurGroup].ListLvl=(int)rtf->IntParam;
    } 
    else if (RtfCmp(CurWord,"listtext")==0) { // ignore list text since we are support list table
       SkipRtfGroup(w,rtf);
    } 


    // character borders
    else if (RtfCmp(CurWord,"chbrdr")==0){     // paragraph border
       if (rtf->IntParam && False(group[CurGroup].flags&(GFLAG_IN_LIST|GFLAG_IN_LIST_OR))) {
          group[CurGroup].BorderType=BORDER_CHAR; // all borders
          group[CurGroup].style|=CHAR_BOX;
       }
       else {
          group[CurGroup].style=group[CurGroup].style&(~(UINT)CHAR_BOX);
          group[CurGroup].StyleOff|=CHAR_BOX;    // style explictly turned off
       }
    }

    // paragraph borders
    else if (RtfCmp(CurWord,"box")==0){     // paragraph border
       group[CurGroup].BorderType=BORDER_PARA_ALL; // all borders
    }
    else if (RtfCmp(CurWord,"brdrt")==0){   // top border
       group[CurGroup].BorderType=BORDER_PARA_TOP;
    }
    else if (RtfCmp(CurWord,"brdrb")==0){   // bottom border
       group[CurGroup].BorderType=BORDER_PARA_BOT;
    }
    else if (RtfCmp(CurWord,"brdrl")==0){   // left border
       group[CurGroup].BorderType=BORDER_PARA_LEFT;
    }
    else if (RtfCmp(CurWord,"brdrr")==0){   // right border
       group[CurGroup].BorderType=BORDER_PARA_RIGHT;
    }
    else if (RtfCmp(CurWord,"brdrbtw")==0){   // border between paragraph
       group[CurGroup].BorderType=BORDER_PARA_BETWEEN;
    }
    else if (RtfCmp(CurWord,"brdrs")==0){  // single line border
       BorderType=group[CurGroup].BorderType;
       SetRtfParaBorders(w,&(group[CurGroup]),BorderType);
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_SINGLE;
       }
    }  
    else if (RtfCmp(CurWord,"brdrnone")==0){  // no border
       BorderType=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[BorderType]=0;
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_NONE;
       }
    }
    else if (RtfCmp(CurWord,"brdrsh")==0){  // shadow border
       BorderType=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[BorderType]=0;
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_SHADOW;
       }
    }
    else if (RtfCmp(CurWord,"brdrsh")==0){  // shadow border
       BorderType=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[BorderType]=0;
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_SHADOW;
       }
    }
    else if (RtfCmp(CurWord,"brdrtnthsg")==0 ||
             RtfCmp(CurWord,"brdrtnthmg")==0 ||
             RtfCmp(CurWord,"brdrtnthlg")==0){  // shadow border
       BorderType=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[BorderType]=0;
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_THIN_THICK;
       }
    }
    else if (RtfCmp(CurWord,"brdrtnthtnsg")==0 ||
             RtfCmp(CurWord,"brdrtnthtnmg")==0 ||
             RtfCmp(CurWord,"brdrtnthtnlg")==0){
       BorderType=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[BorderType]=0;
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_THIN_THICK_THIN;
       }
    }
    else if (RtfCmp(CurWord,"brdrthtnsg")==0 ||
             RtfCmp(CurWord,"brdrthtnmg")==0 ||
             RtfCmp(CurWord,"brdrthtnlg")==0){
       BorderType=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[BorderType]=0;
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_THICK_THIN;
       }
    }
    else if (RtfCmp(CurWord,"brdrthtnthg")==0 ||
             RtfCmp(CurWord,"brdrthtnthmg")==0 ||
             RtfCmp(CurWord,"brdrthtnthlg")==0){
       BorderType=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[BorderType]=0;
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_THICK_THIN_THICK;
       }
    }
    else if (RtfCmp(CurWord,"brdrth")==0){  // thick border
       BorderType=group[CurGroup].BorderType;
       SetRtfParaBorders(w,&(group[CurGroup]),BorderType);
       group[CurGroup].flags=group[CurGroup].flags|PARA_BOX_THICK;
       group[CurGroup].BorderWidth[BorderType]=50; //thick border (twips)
       if (IsSectionBorder(BorderType)) SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
    }
    else if (RtfCmp(CurWord,"brdrw")==0){   // border of specified thickness
       BorderType=group[CurGroup].BorderType;
       SetRtfParaBorders(w,&(group[CurGroup]),BorderType);
       if (rtf->IntParam>20) group[CurGroup].flags=group[CurGroup].flags|PARA_BOX_THICK;  // paragraph border
       group[CurGroup].BorderWidth[BorderType]=(int)(rtf->IntParam);
       if (IsSectionBorder(BorderType)) SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
    }
    else if (RtfCmp(CurWord,"brdrdb")==0){  // double line border
       BorderType=group[CurGroup].BorderType;
       SetRtfParaBorders(w,&(group[CurGroup]),BorderType);
       group[CurGroup].flags=group[CurGroup].flags|PARA_BOX_DOUBLE;
       group[CurGroup].BorderWidth[BorderType]=50; // treat it as a thick border
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_DBL;
       }
    }
    else if (RtfCmp(CurWord,"brdrtriple")==0){  // triple line border
       BorderType=group[CurGroup].BorderType;
       SetRtfParaBorders(w,&(group[CurGroup]),BorderType);
       group[CurGroup].flags=group[CurGroup].flags|PARA_BOX_DOUBLE;
       group[CurGroup].BorderWidth[BorderType]=50; // treat it as a thick border
       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderType=BRDRTYPE_TRIPLE;
       }
    }
    else if (RtfCmp(CurWord,"brdrcf")==0){   // border color
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
       struct StrRtfColor far *color=pRtfGroup->color;

       if (rtf->param[0]==0) rtf->IntParam=0;  // default
       BorderType=group[CurGroup].BorderType;
       if (rtf->IntParam>=0 && rtf->IntParam<MaxRtfColors) group[CurGroup].BorderColor[BorderType]=color[(int)(rtf->IntParam)].color;

       SetRtfParaBorders(w,&(group[CurGroup]),BorderType);
       if (True(group[CurGroup].flags&PARA_BOX)) group[CurGroup].ParaBorderColor=color[(int)(rtf->IntParam)].color;

       if (IsSectionBorder(BorderType)) {
          SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
          rtf->sect.BorderColor=color[(int)(rtf->IntParam)].color;    // double border
       }
    }
    else if (RtfCmp(CurWord,"brsp")==0) {     // border space
       CurGroup=rtf->GroupLevel;
       BorderType=group[CurGroup].BorderType;
       SetRtfParaBorders(w,&(group[CurGroup]),BorderType);
       group[CurGroup].BorderSpace[BorderType]=(int)(rtf->IntParam); // treat it as a thick border
       group[CurGroup].BorderMargin=(int)(rtf->IntParam); // treat it as a thick border
       if (IsSectionBorder(BorderType)) SetRtfSectBorders(w,&(group[CurGroup]),BorderType);
    }
    else if (RtfCmp(CurWord,"shading")==0){  // paragraph shading
       CurGroup=rtf->GroupLevel;
       group[CurGroup].ParShading=(int)rtf->IntParam; // paragraph shading
    }
    else if (RtfCmp(CurWord,"cbpat")==0){  // construct shading from the background patten color
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
       struct StrRtfColor far *color=pRtfGroup->color;

       group[CurGroup].ParaBkColor=color[(int)rtf->IntParam].color;
       //group[CurGroup].ParShading=0;
    }
    else if (RtfCmp(CurWord,"cfpat")==0){  // construct shading from the background patten color
       struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
       struct StrRtfColor far *color=pRtfGroup->color;

       /*if (group[CurGroup].ParShading==0) */ group[CurGroup].ParaBkColor=color[(int)rtf->IntParam].color;
       group[CurGroup].ParShading=0;
    }
    else ControlProcessed=FALSE;            // create a break to help the compiler stack overflow

    if (ControlProcessed) return 0;

    // process more controls
    if (RtfCmp(CurWord,"phpg")==0){  // frame x relative to the page
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&GFLAG_DRAW_OBJECT)) {  // can't embed draw object in a frame   
          CurGroup=rtf->GroupLevel;
          group[CurGroup].FrmFlags|=(PARA_FRAME_HPAGE|PARA_FRAME_USED);
          group[CurGroup].HPageGroup=CurGroup;  // group where PARA_FRAME_HPAGE was set
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"pvpg")==0){  // frame y relative to the page
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT|GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].FrmFlags|=(PARA_FRAME_VPAGE|PARA_FRAME_USED);
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"pvmrg")==0){  // frame y relative to the top margin
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT|GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].FrmFlags|=(PARA_FRAME_VMARG|PARA_FRAME_USED);
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"posxr")==0){  // right justified frame
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT|GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].FrmFlags|=(PARA_FRAME_RIGHT_JUST|PARA_FRAME_USED);
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"posxc")==0){  // centered frame frame
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT||GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].FrmFlags|=(PARA_FRAME_CENTER|PARA_FRAME_USED);
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"posyc")==0){  // y centered frame frame
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (true || !(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT||GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].FrmFlags|=(PARA_FRAME_VCENTER|PARA_FRAME_USED);
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"posx")==0      // frame x relative to the page
          || RtfCmp(CurWord,"posnegx")==0 ){// frame x relative to the page

       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT||GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].ParaFrameInfo.x=(int)rtf->IntParam;
          group[CurGroup].FrmFlags|=PARA_FRAME_USED;
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"posxi")==0){  // left-justified to the reference frame
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT||GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].ParaFrameInfo.x=0;
          group[CurGroup].FrmFlags|=PARA_FRAME_USED;
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"posy")==0       // frame y relative to the page
          || RtfCmp(CurWord,"posnegy")==0 ){ // frame y relative to the page
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT||GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].ParaFrameInfo.y=(int)rtf->IntParam;
          group[CurGroup].FrmFlags|=PARA_FRAME_USED;
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"posyt")==0){   // frame y at the top margin
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       //if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
       if (!(group[CurGroup].gflags&(GFLAG_DRAW_OBJECT||GFLAG_IN_SHP))) {  // can't embed draw object in a frame   
          group[CurGroup].ParaFrameInfo.y=0;
          group[CurGroup].FrmFlags|=PARA_FRAME_USED;
          if (!(group[CurGroup].FrmFlags&PARA_FRAME_VPAGE)) group[CurGroup].FrmFlags|=PARA_FRAME_VMARG;
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"absw")==0){  // frame width
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
          group[CurGroup].ParaFrameInfo.width=(int)rtf->IntParam;
          group[CurGroup].FrmFlags|=PARA_FRAME_USED;
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);

          if (FALSE && RtfCurRowId==0) {
             group[CurGroup].InTable=FALSE;                         // turnoff any continuing cell
             rtf->InitialCell=0;
          }
       }
    }
    else if (RtfCmp(CurWord,"absh")==0){  // frame height
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
          group[CurGroup].ParaFrameInfo.height=abs((int)rtf->IntParam);
          if (rtf->IntParam<0) group[CurGroup].FrmFlags|=PARA_FRAME_FIXED_HEIGHT;
          group[CurGroup].FrmFlags|=PARA_FRAME_USED;
          group[CurGroup].gflags|=(GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
       }
    }
    else if (RtfCmp(CurWord,"dfrmtxtx")==0){  // not strictly a frame control - distance from text
       if (!group[CurGroup].InTable && RtfCurRowId>0 && !rtf->TableInFrame) RtfCurRowId=RtfCurCellId=RtfLastCellX=0;
       if (TRUE || RtfCurRowId==0 || rtf->TableInFrame) {
          group[CurGroup].ParaFrameInfo.DistFromText=abs((int)rtf->IntParam);
       }
    }
    else if (RtfCmp(CurWord,"nowrap")==0){   // no wrapping of text around the frame
       group[CurGroup].FrmFlags|=PARA_FRAME_NO_WRAP;
    }
    else if (  RtfCmp(CurWord,"frmtxtblr")==0    // top-to-bottom text flow, left-to-right line flow
            || RtfCmp(CurWord,"frmtxtbrl")==0    // top-to-bottom text flow, right-to-left line flow
            || RtfCmp(CurWord,"frmtxtbrlv")==0 ){// top-to-bottom text flow, right-to-left line flow
       group[CurGroup].TextAngle=270;
    }
    else if (RtfCmp(CurWord,"frmtxbtlr")==0){   // bottom-to-top text flow
       group[CurGroup].TextAngle=90;
    }

    // drawing object
    else if (RtfCmp(CurWord,"do")==0){       // drawing object
       if (FALSE && RtfParaFID>0 && group[CurGroup].gflags&GFLAG_IN_FRAME) SkipRtfGroup(w,rtf);
       else {
          group[CurGroup].gflags|=(GFLAG_INSERT_BEF_PARA|GFLAG_DRAW_OBJECT|GFLAG_NEW_FRAME|GFLAG_IN_FRAME);
          rtf->flags|=RFLAG_NEW_FRAME;                 // force a new dob frame
          RtfParaFID=0;                                // new paraid will be assigned
          group[CurGroup].TextAngle=0;                 // reset text angle
          //group[CurGroup].TextBoxMargin=MIN_FRAME_MARGIN;
       }
    }
    else if (RtfCmp(CurWord,"dptxbtlr")==0){  // bottom-to-top, left-to-right
       group[CurGroup].TextAngle=90;
    }
    else if (RtfCmp(CurWord,"dptxtbrl")==0){  // top-to-bottom
       group[CurGroup].TextAngle=270;
    }
    else if (RtfCmp(CurWord,"dobxpage")==0){  // object x position relative to page
       group[CurGroup].FrmFlags|=PARA_FRAME_HPAGE;
       group[CurGroup].HPageGroup=CurGroup;  // group where PARA_FRAME_HPAGE was set
    }
    else if (RtfCmp(CurWord,"dobypage")==0){  // object y position relative to page
       group[CurGroup].FrmFlags|=PARA_FRAME_VPAGE;
    }
    else if (RtfCmp(CurWord,"dobymargin")==0){  // object y position relative to top margin
       group[CurGroup].FrmFlags|=PARA_FRAME_VMARG;
    }
    else if (RtfCmp(CurWord,"dodhgt")==0){     // Z order for the drawing object
       group[CurGroup].ParaFrameInfo.ZOrder=(int)rtf->IntParam;
       if (group[CurGroup].ParaFrameInfo.ZOrder==0) group[CurGroup].ParaFrameInfo.ZOrder=-1; // For word/TE compatibility, see Zorder write in ter_rtf3.c
    }
    else if (RtfCmp(CurWord,"dptxbxmar")==0){  // text box margin
       group[CurGroup].TextBoxMargin=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"dptxbx")==0){    // object is a text box
       group[CurGroup].FrmFlags|=(PARA_FRAME_TEXT_BOX|PARA_FRAME_USED);
    }
    else if (RtfCmp(CurWord,"dpgroup")==0){    // new dp group
       group[CurGroup].gflags|=GFLAG_IN_DPGROUP;
       if (group[CurGroup].DpGroupCount<MAX_DP_GROUPS) group[CurGroup].DpGroupCount++;
    }
    else if (RtfCmp(CurWord,"dpendgroup")==0){ // end of a dpgroup construct
       RtfParaFID=0;                           // finish any drawing object element
       if (group[CurGroup].DpGroupCount>0) group[CurGroup].DpGroupCount--;
       ResetLongFlag(group[CurGroup].gflags,GFLAG_IN_DPGROUP);
    }
    else if (RtfCmp(CurWord,"dpline")==0){    // object is a line
       RtfParaFID=0;                                // new paraid will be assigned
       
       ResetLongFlag(group[CurGroup].gflags,GFLAG_IN_DPGROUP);
       group[CurGroup].FrmFlags|=(PARA_FRAME_LINE|PARA_FRAME_USED);
       rtf->OutBuf[rtf->OutBufLen]=ParaChar;         // insert a para marker
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;                // NULL termination
       RtfParaFID=0;                                // reset any previous para fid
       if (!SendRtfText(w,rtf)) return RTF_END_FILE;       // send this line
       // initialize the coordinates
       ParaFrame[RtfParaFID].x=ParaFrame[RtfParaFID].ParaY=-1;
       ParaFrame[RtfParaFID].width=ParaFrame[RtfParaFID].height=1;
       ParaFrame[RtfParaFID].LineType=DOL_HORZ;
    }
    else if (RtfCmp(CurWord,"dprect")==0){    // object is a rectangle
       RtfParaFID=0;                                // new paraid will be assigned
       
       ResetLongFlag(group[CurGroup].gflags,GFLAG_IN_DPGROUP);
       group[CurGroup].FrmFlags|=(PARA_FRAME_RECT|PARA_FRAME_USED);
       rtf->OutBuf[rtf->OutBufLen]=ParaChar;         // insert a para marker
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;                // NULL termination
       if (!SendRtfText(w,rtf)) return RTF_END_FILE;       // send this line
    }
    else if (RtfCmp(CurWord,"dptxbxtext")==0){ // drawing object text follows
       RtfParaFID=0;                                // new paraid will be assigned
       group[CurGroup].gflags|=GFLAG_DO_TEXT_READ;
       ResetLongFlag(group[CurGroup].gflags,GFLAG_IN_DPGROUP);
    }
    else if (RtfCmp(CurWord,"dpptx")==0 && RtfParaFID>0){  // x cordinates of the line point
       int DpGrp=group[CurGroup].DpGroupCount-1;
       if (DpGrp>=0 && !(group[CurGroup].gflags&GFLAG_IN_DPGROUP)) rtf->IntParam+=group[CurGroup].DpGroupX[DpGrp];
       
       if (ParaFrame[RtfParaFID].x<0) ParaFrame[RtfParaFID].x=(int)rtf->IntParam;  // first point
       else {         // second point
          ParaFrame[RtfParaFID].width=(int)rtf->IntParam-ParaFrame[RtfParaFID].x;
          group[CurGroup].gflags|=GFLAG_GOT_WIDTH;          // got the width numbers
          if (group[CurGroup].gflags&GFLAG_GOT_HEIGHT) SetRtfLineOrient(w,rtf,FALSE,FALSE);  // set the line orientation
       }
    }
    else if (RtfCmp(CurWord,"dppty")==0 && RtfParaFID>0){  // y cordinates of the line point
       int DpGrp=group[CurGroup].DpGroupCount-1;
       if (DpGrp>=0 && !(group[CurGroup].gflags&GFLAG_IN_DPGROUP)) rtf->IntParam+=group[CurGroup].DpGroupX[DpGrp];
       
       if (ParaFrame[RtfParaFID].ParaY<0) ParaFrame[RtfParaFID].ParaY=(int)rtf->IntParam;  // first point
       else {         // second point
          ParaFrame[RtfParaFID].height=ParaFrame[RtfParaFID].MinHeight=(int)rtf->IntParam-ParaFrame[RtfParaFID].ParaY;
          group[CurGroup].gflags|=GFLAG_GOT_HEIGHT;     // got the HEIGHT numbers
          if (group[CurGroup].gflags&GFLAG_GOT_WIDTH) SetRtfLineOrient(w,rtf,FALSE,FALSE);  // set the line orientation
       }
    }
    else if (RtfCmp(CurWord,"dpx")==0) {     // object x position
       int DpGrp=group[CurGroup].DpGroupCount-1;
       if (group[CurGroup].gflags&GFLAG_IN_DPGROUP) {
          if (DpGrp>=0) group[CurGroup].DpGroupX[DpGrp]=(int)rtf->IntParam;
          if ((DpGrp-1)>=0) group[CurGroup].DpGroupX[DpGrp]+=group[CurGroup].DpGroupX[DpGrp-1];
       }
       else if (RtfParaFID>0) { 
          if (ParaFrame[RtfParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_RECT)) ParaFrame[RtfParaFID].x=0;
          if (DpGrp>=0) ParaFrame[RtfParaFID].x=group[CurGroup].DpGroupX[DpGrp];
          ParaFrame[RtfParaFID].x+=(int)rtf->IntParam;

          if (ParaFrame[RtfParaFID].flags&PARA_FRAME_HPAGE) {
             ParaFrame[RtfParaFID].x-=((int)InchesToTwips(rtf->sect.LeftMargin));
             ParaFrame[RtfParaFID].flags=ResetUintFlag(ParaFrame[RtfParaFID].flags,PARA_FRAME_HPAGE);  // reset to avoid multiple application
          }
       }
    }
    else if (RtfCmp(CurWord,"dpy")==0){     // object y position
       int DpGrp=group[CurGroup].DpGroupCount-1;
       if (group[CurGroup].gflags&GFLAG_IN_DPGROUP) {
          if (DpGrp>=0) group[CurGroup].DpGroupY[DpGrp]=(int)rtf->IntParam;
          if ((DpGrp-1)>=0) group[CurGroup].DpGroupY[DpGrp]+=group[CurGroup].DpGroupY[DpGrp-1];
       }
       else if (RtfParaFID>0) { 
          if (ParaFrame[RtfParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_RECT)) ParaFrame[RtfParaFID].ParaY=0;
          if (DpGrp>=0) ParaFrame[RtfParaFID].ParaY=group[CurGroup].DpGroupY[DpGrp];
          ParaFrame[RtfParaFID].ParaY+=(int)rtf->IntParam;
       }
    }
    else if (RtfCmp(CurWord,"dpxsize")==0 && RtfParaFID>0){ // object width
       //if (TerFlags&TFLAG_COMP_WORD97 || ParaFrame[RtfParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_RECT)) {
          CurGroup=rtf->GroupLevel;
          ParaFrame[RtfParaFID].width=(int)rtf->IntParam;
          ResetUintFlag(ParaFrame[RtfParaFID].flags,PARA_FRAME_SET_WIDTH);
       //}
    }
    else if (RtfCmp(CurWord,"dpysize")==0 && RtfParaFID>0){ // object height
       //if (ParaFrame[RtfParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_RECT)) {
          CurGroup=rtf->GroupLevel;
          ParaFrame[RtfParaFID].height=ParaFrame[RtfParaFID].MinHeight=(int)rtf->IntParam;
       //}
    }
    else if (RtfCmp(CurWord,"dplinehollow")==0 && RtfParaFID>0){ // no border
       group[CurGroup].gflags|=GFLAG_DPLINEHOLLOW;
    }
    else if (RtfCmp(CurWord,"dplinesolid")==0 && RtfParaFID>0){ // set borders
       ParaFrame[RtfParaFID].flags|=PARA_FRAME_BOXED;
    }
    else if (RtfCmp(CurWord,"dplinedot")==0 && RtfParaFID>0){ // set borders
       ParaFrame[RtfParaFID].flags|=(PARA_FRAME_BOXED|PARA_FRAME_DOTTED);
    }
    else if (RtfCmp(CurWord,"dplinew")==0 && RtfParaFID>0){ // set line width
       ParaFrame[RtfParaFID].LineWdth=(int)rtf->IntParam;
       if ((int)rtf->IntParam>0 && !(group[CurGroup].gflags&GFLAG_DPLINEHOLLOW)) ParaFrame[RtfParaFID].flags|=PARA_FRAME_BOXED;
    }
    else if (RtfCmp(CurWord,"dplinecor")==0 && RtfParaFID>0){ // background red
       COLORREF color=ParaFrame[RtfParaFID].LineColor;
       ParaFrame[RtfParaFID].LineColor=RGB((int)rtf->IntParam,GetGValue(color),GetBValue(color));
    }
    else if (RtfCmp(CurWord,"dplinecog")==0 && RtfParaFID>0){ // background green
       COLORREF color=ParaFrame[RtfParaFID].LineColor;
       ParaFrame[RtfParaFID].LineColor=RGB(GetRValue(color),(int)rtf->IntParam,GetBValue(color));
    }
    else if (RtfCmp(CurWord,"dplinecob")==0 && RtfParaFID>0){ // background blue
       COLORREF color=ParaFrame[RtfParaFID].LineColor;
       ParaFrame[RtfParaFID].LineColor=RGB(GetRValue(color),GetGValue(color),(int)rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"dpfillbgcr")==0 && RtfParaFID>0){ // background red
       COLORREF color=ParaFrame[RtfParaFID].BackColor;
       ParaFrame[RtfParaFID].BackColor=RGB((int)rtf->IntParam,GetGValue(color),GetBValue(color));
    }
    else if (RtfCmp(CurWord,"dpfillbgcg")==0 && RtfParaFID>0){ // background green
       COLORREF color=ParaFrame[RtfParaFID].BackColor;
       ParaFrame[RtfParaFID].BackColor=RGB(GetRValue(color),(int)rtf->IntParam,GetBValue(color));
    }
    else if (RtfCmp(CurWord,"dpfillbgcb")==0 && RtfParaFID>0){ // background blue
       COLORREF color=ParaFrame[RtfParaFID].BackColor;
       ParaFrame[RtfParaFID].BackColor=RGB(GetRValue(color),GetGValue(color),(int)rtf->IntParam);
    }
    else if (RtfCmp(CurWord,"dpfillbggray")==0 && RtfParaFID>0){ // number of half percentage of gray
       int ColorVal=255-(int)((rtf->IntParam*255)/200);
       ParaFrame[RtfParaFID].BackColor=RGB(ColorVal,ColorVal,ColorVal);
    }
    else if (RtfCmp(CurWord,"dpfillpat")==0 && RtfParaFID>0){ // fill pattern
       ParaFrame[RtfParaFID].FillPattern=(int)rtf->IntParam;
    }


    // create tab stops
    else if (RtfCmp(CurWord,"tqr")==0) {    // right tab
       rtf->CurTabType=TAB_RIGHT;
    }
    else if (RtfCmp(CurWord,"tqdec")==0) {  // decimal tab
       rtf->CurTabType=TAB_DECIMAL;
    }
    else if (RtfCmp(CurWord,"tqc")==0){     // center tab
       rtf->CurTabType=TAB_CENTER;
    }
    else if (RtfCmp(CurWord,"tldot")==0){   // dot leader
       rtf->CurTabFlags=TAB_DOT;
    }
    else if (RtfCmp(CurWord,"tlhyph")==0){   // hyphen leader
       rtf->CurTabFlags=TAB_HYPH;
    }
    else if (RtfCmp(CurWord,"tlul")==0){     // underline leader
       rtf->CurTabFlags=TAB_ULINE;
    }
    else if (RtfCmp(CurWord,"tx")==0){          // set tab position
       CurGroup=rtf->GroupLevel;
       count=group[CurGroup].tab.count;                // existing number of tab postions
       if (count<TE_MAX_TAB_STOPS) {
          for (i=0;i<count;i++) {
             if (group[CurGroup].tab.pos[i]==(int)rtf->IntParam && group[CurGroup].tab.type[i]==rtf->CurTabType) break;
          }
          if (i==count) {     // new tab position
             group[CurGroup].tab.pos[count]=(int)rtf->IntParam; // tab position in twips
             group[CurGroup].tab.type[count]=rtf->CurTabType;
             group[CurGroup].tab.flags[count]=rtf->CurTabFlags;
             group[CurGroup].tab.count++;                 // increment tab position count
          }
       }
       rtf->CurTabType=TAB_LEFT; // reset for the text tab
       rtf->CurTabFlags=0;
    }

    // table row control words
    else if (RtfCmp(CurWord,"sssubtable")==0){   // embedded table group
       int level=group[CurGroup].level;
       rtf->flags1|=RFLAG1_SSSUBTABLE_USED;      // old style subtables found
       SetRtfTblLevel(w,rtf,CurGroup,level+1,level);
    }    
    else if (RtfCmp(CurWord,"itap")==0) {   // embedded table group
       int NewLevel=(int)rtf->IntParam;
       BOOL InFnote=(group[CurGroup].style&(FNOTEALL));
       if (!InFnote) {
          if (NewLevel<0) NewLevel=0;
          group[CurGroup].level=RtfInitLevel+NewLevel;
          if (!rtf->EmbedTable && RtfCurCellId!=RtfInitCellId) group[CurGroup].level--;  // pasting begins at the parent level (and not inside parent)
       }
    } 
    else if (RtfCmp(CurWord,"nonesttables")==0){// this control word is for compatibility with older rtf readers
       SkipRtfGroup(w,rtf);
    }
    else if (RtfCmp(CurWord,"nesttableprops")==0){    // destination group, don't let it be ignored
       group[CurGroup].gflags|=GFLAG_IN_NESTTABLEPROPS;
    }
    else if (RtfCmp(CurWord,"trowd")==0){       // create a table row id

       if (group[CurGroup].FrmFlags&PARA_FRAME_USED) rtf->TableInFrame=TRUE;

       // check if a para marker needs to be inserted before pasting a table
       if (RtfInput>=RTF_CB && RtfInput!=RTF_CB_APPEND
          && !(rtf->flags1&RFLAG1_TEXT_FOUND) && RtfCurCellId==0
          && CurLine>0 && LineLen(CurLine-1)>0 && cid(CurLine-1)==0
          && (!(LineFlags(CurLine-1)&(LFLAG_PARA|LFLAG_LINE|LFLAG_BREAK))) ){

          rtf->OutBuf[rtf->OutBufLen]=ParaChar;   // terminate the previous paragrapb
          rtf->OutBufLen++;
          rtf->OutBuf[rtf->OutBufLen]=0;          // NULL termination
          if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send this line
       }

       if (rtf->PastingColumn && rtf->SomeTextRead && !(rtf->TableRead)) {
          rtf->SuspendReading=TRUE;             // while pasting within a table some non-table text preceeded a table
          MessageBeep(0);
          return 0;
       }
       
       // change level to root if not in nested tables
       if (!(group[CurGroup].gflags&GFLAG_IN_NESTTABLEPROPS) && !(rtf->flags1&RFLAG1_SSSUBTABLE_USED)) {
          group[CurGroup].level=RtfInitLevel+1;
          if (!rtf->EmbedTable) group[CurGroup].level--;
             
          // change level to root if not in nested tables
          if (rtf->CurTblLevel!=group[CurGroup].level) 
               SetRtfTblLevel(w,rtf,CurGroup,group[CurGroup].level,rtf->CurTblLevel);
       } 

       if (!SetRtfRowDefault(w,rtf,group,CurGroup)) return RTF_SYNTAX_ERROR;
    }
    else if ((RtfCmp(CurWord,"row")==0 || RtfCmp(CurWord,"nestrow")==0) 
             && !(rtf->flags&RFLAG_END_TABLE)){        // end of the current row
       int CurRowId;

       group[CurGroup].InTable=TRUE;           // in table flag set

       // insert the row break character
       if (!rtf->PastingColumn || !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST)) {
          rtf->OutBuf[rtf->OutBufLen]=ROW_CHAR;   // insert a row break char
          rtf->OutBufLen++;
          rtf->OutBuf[rtf->OutBufLen]=0;          // NULL termination
          if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send this line
       }

       // row border
       CurRowId=RtfCurRowId;
       TableRow[CurRowId].border=0;
       if (group[CurGroup].BorderWidth[BORDER_ROW_TOP])   TableRow[CurRowId].border|=BORDER_TOP;
       if (group[CurGroup].BorderWidth[BORDER_ROW_BOT])   TableRow[CurRowId].border|=BORDER_BOT;
       if (group[CurGroup].BorderWidth[BORDER_ROW_LEFT])  TableRow[CurRowId].border|=BORDER_LEFT;
       if (group[CurGroup].BorderWidth[BORDER_ROW_RIGHT]) TableRow[CurRowId].border|=BORDER_RIGHT;

       if (TableRow[CurRowId].border&BORDER_TOP)   TableRow[CurRowId].BorderWidth[BORDER_INDEX_TOP]  =group[CurGroup].BorderWidth[BORDER_ROW_TOP];
       if (TableRow[CurRowId].border&BORDER_BOT)   TableRow[CurRowId].BorderWidth[BORDER_INDEX_BOT]  =group[CurGroup].BorderWidth[BORDER_ROW_BOT];
       if (TableRow[CurRowId].border&BORDER_LEFT)  TableRow[CurRowId].BorderWidth[BORDER_INDEX_LEFT] =group[CurGroup].BorderWidth[BORDER_ROW_LEFT];
       if (TableRow[CurRowId].border&BORDER_RIGHT) TableRow[CurRowId].BorderWidth[BORDER_INDEX_RIGHT]=group[CurGroup].BorderWidth[BORDER_ROW_RIGHT];

       if (!CopyRtfRow(w,rtf,group,CurGroup)) return RTF_SYNTAX_ERROR;

       ResetUintFlag(TableAux[RtfCurRowId].flags,(TAUX_TROWD_DONE|TAUX_REUSE_CELL_CHAIN));  // trowd not found for this new row yet
    }
    else if (RtfCmp(CurWord,"intbl")==0){      // header type row
       if (rtf->IntParam) {
          group[CurGroup].InTable=TRUE;
          if (!(rtf->flags1&RFLAG1_SSSUBTABLE_USED)) {
             group[CurGroup].level=RtfInitLevel+1;
             if (!rtf->EmbedTable) group[CurGroup].level--;   // pasting at the same level
          }
          if (!rtf->EmbedTable) rtf->InitialCell=0;           // reset initial cell
       }
       else group[CurGroup].InTable=false;
    }
    else if (RtfCmp(CurWord,"sstrid")==0){      // table id
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST)) {
          int i,id=(int)rtf->IntParam;
          for (i=1;i<TotalTableRows;i++) {    // check if this id already exists
             if (TableRow[i].flags&ROWFLAG_PREEXIST && TableRow[i].id==id) {
                id=0;                // this id is already in use, use a default value of 0
                break;
             } 
          } 
          TableRow[RtfCurRowId].id=id;
       }
    }
    else if (RtfCmp(CurWord,"trhdr")==0){      // header type row
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST))
          TableRow[RtfCurRowId].flags|=ROWFLAG_HDR;
    }
    else if (RtfCmp(CurWord,"trkeep")==0){      // keep the row together
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST))
          TableRow[RtfCurRowId].flags|=ROWFLAG_KEEP;
    }
    else if (RtfCmp(CurWord,"trgaph")==0){      // cell margin
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST))
          TableRow[RtfCurRowId].CellMargin=(int)rtf->IntParam;
    }
    else if (RtfCmp(CurWord,"trqr")==0){      // cell margin
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST))
          TableRow[RtfCurRowId].flags|=RIGHT_JUSTIFY;
    }
    else if (RtfCmp(CurWord,"trqc")==0){      // cell margin
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST))
          TableRow[RtfCurRowId].flags|=CENTER;
    }
    else if (RtfCmp(CurWord,"trleft")==0){      // cell margin
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST)) {
          int indent=(int)rtf->IntParam;
          if ((indent<0) && False(TerFlags4&TFLAG4_DONT_FIX_NEG_INDENT)) { // adjust to keep indent positive
             TableRow[RtfCurRowId].AddedIndent=-indent;
             indent=0;
             if (RtfInput>=RTF_CB) TableRow[RtfCurRowId].AddedIndent=0;   // to keep the existing row to from sliding toward right in FixNegativeIndent function
          }
          TableRow[RtfCurRowId].indent=indent;
       }
       rtf->PrevCellX=(int)rtf->IntParam;       // to calculate cell width
    }
    else if (RtfCmp(CurWord,"trrh")==0){      // minmum row height
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST)) {
          TableRow[RtfCurRowId].MinHeight=(int)rtf->IntParam;
          if (TerFlags5&TFLAG5_NO_EXACT_ROW_HEIGHT) TableRow[RtfCurRowId].MinHeight=abs(TableRow[RtfCurRowId].MinHeight);
       }
    }
    else if (RtfCmp(CurWord,"trbrdrt")==0){    // top row border
       group[CurGroup].BorderType=BORDER_ROW_TOP;
       group[CurGroup].BorderWidth[BORDER_ROW_TOP]=ScrToTwipsY(1); // default thickness
    }
    else if (RtfCmp(CurWord,"trbrdrb")==0){    // bottom row border
       group[CurGroup].BorderType=BORDER_ROW_BOT;
       group[CurGroup].BorderWidth[BORDER_ROW_BOT]=ScrToTwipsY(1); // default thickness
    }
    else if (RtfCmp(CurWord,"trbrdrl")==0){    // left row border
       group[CurGroup].BorderType=BORDER_ROW_LEFT;
       group[CurGroup].BorderWidth[BORDER_ROW_LEFT]=ScrToTwipsX(1); // default thickness
    }
    else if (RtfCmp(CurWord,"trbrdrr")==0){    // right row border
       group[CurGroup].BorderType=BORDER_ROW_RIGHT;
       group[CurGroup].BorderWidth[BORDER_ROW_RIGHT]=ScrToTwipsX(1); // default thickness
    }
    else if (RtfCmp(CurWord,"rtlrow")==0){       // rtl flow within the cell
       rtf->CellFlow=FLOW_RTL;
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST))
          TableRow[RtfCurRowId].flow=FLOW_RTL;
    }
    else if (RtfCmp(CurWord,"ltrrow")==0){       // ltr flow within the cell
       rtf->CellFlow=FLOW_LTR;
       if (RtfCurRowId>0 && !(TableRow[RtfCurRowId].flags&ROWFLAG_PREEXIST))
          TableRow[RtfCurRowId].flow=FLOW_LTR;
    }

    // table cell control words
    else if (RtfCmp(CurWord,"ssdelcell")==0   // undoing cell deletion
          || RtfCmp(CurWord,"sswholetablerows")==0){ // pasting whole table rows
       rtf->InitTblCol=-1;                    // do not treat it as column pasting
       rtf->PastingColumn=FALSE;
       if (RtfCmp(CurWord,"ssdelcell")==0) rtf->EmbedTable=FALSE;
       rtf->InsertBefCell=rtf->InsertAftCell=0;
    }
    else if (RtfCmp(CurWord,"sstblend")==0){     // end of table
       if (rtf->flags&RFLAG_CURS_IN_TABLE) rtf->flags|=RFLAG_END_TABLE;
    }
    else if (RtfCmp(CurWord,"cellx")==0){     // cell right position
       if (!CreateRtfCell(w,rtf,group,CurGroup)) return RTF_SYNTAX_ERROR;
    }
    else if (RtfCmp(CurWord,"clmrg")==0){     // this cell merged with the previous cell
       rtf->flags|=RFLAG_CELL_MERGED;
    }
    else if (RtfCmp(CurWord,"clvmrg")==0){    // this cell is row-spanned
       rtf->flags1|=RFLAG1_ROW_SPANNED;
    }
    else if ((RtfCmp(CurWord,"cell")==0 || RtfCmp(CurWord,"nestcell")==0) 
             && !(rtf->flags&RFLAG_END_TABLE)){     // next cell begins
       int CurCellId;
       
       if (rtf->OutBufLen>0 && group[rtf->GroupLevel].UcBase>0) if (!SendRtfText(w,rtf)) return false;  // send any unicode text

       // get the new cell if the previous cell cloesd - this is needed here because for merged cell SendRtfText is not called since \cell is ignored
       if (RtfCurCellId>0 && CellAux[RtfCurCellId].flags&CAUX_CLOSED) 
           SetRtfTableInfo(w,&(group[CurGroup]));
       
       CurCellId=RtfCurCellId;

       if (group[CurGroup].InTable && RtfCurRowId<=0 && rtf->OpenRowId>0) { // begin a table row
          RtfCurRowId=rtf->OpenRowId;       // make use of the previously opened row
          RtfCurCellId=CurCellId=rtf->OpenCellId;
          RtfLastCellX=rtf->OpenLastCellX;
       }

       if (true || CurCellId==0 || !(cell[CurCellId].flags&CFLAG_MERGED)) {    // CurCellId can be zero because may be the table row has not been defined yet
          rtf->OutBuf[rtf->OutBufLen]=CellChar;   // insert a row break char
          rtf->OutBufLen++;
          rtf->OutBuf[rtf->OutBufLen]=0;          // NULL termination
       }
       if (!SendRtfText(w,rtf)) return RTF_END_FILE;    // send even 0 length string

       // mark this cell as closed
       if (RtfCurCellId>0) CellAux[RtfCurCellId].flags|=CAUX_CLOSED;      // mark it as closed
    }
    else if (RtfCmp(CurWord,"clpadt")==0){  // control word supported beginning with Word2000
       group[CurGroup].CellMargin=(int)rtf->IntParam;
       group[CurGroup].gflags|=GFLAG_CELL_MARGIN_OVERRIDE;
    }
    else if (RtfCmp(CurWord,"clvertalt")==0){  // vertical top aligned cell
       group[CurGroup].CellFlags=ResetUintFlag(group[CurGroup].CellFlags,(CFLAG_VALIGN_CTR|CFLAG_VALIGN_BOT));
    }
    else if (RtfCmp(CurWord,"clvertalc")==0){  // vertical center aligned cell
       group[CurGroup].CellFlags|=CFLAG_VALIGN_CTR;
    }
    else if (RtfCmp(CurWord,"clvertalb")==0){  // vertical bottom aligned cell
       group[CurGroup].CellFlags|=CFLAG_VALIGN_BOT;
    }
    else if (RtfCmp(CurWord,"ssclvertalbs")==0){  // vertical base line alignment
       group[CurGroup].CellFlags|=CFLAG_VALIGN_BASE;
    }
    else if (RtfCmp(CurWord,"clbrdrt")==0){    // top row border
       group[CurGroup].BorderType=BORDER_CELL_TOP;
       group[CurGroup].BorderWidth[BORDER_CELL_TOP]=0; //ScrToTwipsY(1); // default thickness
    }
    else if (RtfCmp(CurWord,"clbrdrb")==0){    // bottom row border
       group[CurGroup].BorderType=BORDER_CELL_BOT;
       group[CurGroup].BorderWidth[BORDER_CELL_BOT]=0; //ScrToTwipsY(1); // default thickness
    }
    else if (RtfCmp(CurWord,"clbrdrl")==0){    // left row border
       group[CurGroup].BorderType=BORDER_CELL_LEFT;
       group[CurGroup].BorderWidth[BORDER_CELL_LEFT]=0; //ScrToTwipsX(1); // default thickness
    }
    else if (RtfCmp(CurWord,"clbrdrr")==0){    // right row border
       group[CurGroup].BorderType=BORDER_CELL_RIGHT;
       group[CurGroup].BorderWidth[BORDER_CELL_RIGHT]=0; // ScrToTwipsX(1); // default thickness
    }
    else if (RtfCmp(CurWord,"clshdng")==0){    // right row border
       //if (!(group[CurGroup].CellFlags&CFLAG_FORCE_BKND_CLR))
          group[CurGroup].CellShading=(int)(rtf->IntParam/100);
    }
    else if (RtfCmp(CurWord,"clcfpat")==0      // forground color of the background pattern
            || RtfCmp(CurWord,"clcbpat")==0){  // forground color of the background pattern
       if (rtf->IntParam>=0 && rtf->IntParam<MaxRtfColors) {
          struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);
          struct StrRtfColor far *color=pRtfGroup->color;

          if (RtfCmp(CurWord,"clcfpat")==0) group[CurGroup].CellPatFC=color[(int)(rtf->IntParam)].color;
          else  {
             group[CurGroup].CellPatBC=color[(int)(rtf->IntParam)].color;
             if (group[CurGroup].CellPatBC==CLR_AUTO) group[CurGroup].CellPatBC=CLR_WHITE;
          }
          group[CurGroup].CellFlags|=CFLAG_FORCE_BKND_CLR;
       }
    }
    else if (RtfCmp(CurWord,"sscolspan")==0){    // column spanning
       group[CurGroup].CellColSpan=(int)(rtf->IntParam);     // column spanning
    }
    else if (RtfCmp(CurWord,"cltxbtlr")==0      // text angle
          || RtfCmp(CurWord,"cltxbtrl")==0) {   // text angle
          group[CurGroup].TextAngle=90;   
    }
    else if (RtfCmp(CurWord,"cltxtblr")==0      // text angle
          || RtfCmp(CurWord,"cltxtbrl")==0) {   // text angle
       group[CurGroup].TextAngle=270;   
    }

    // row or cell borders
    else if (RtfCmp(CurWord,"brdrdb")==0){     // double thickness border
       int type=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[type]=2;
    }
    else if (RtfCmp(CurWord,"brdrw")==0){     // double thickness border
       int type=group[CurGroup].BorderType;
       group[CurGroup].BorderWidth[type]=(int)(rtf->IntParam);
    }

    // bookmark/tag support
    else if (RtfCmp(CurWord,"bkmkstart")==0) { // bookmark begin
       ReturnCode=ReadRtfBookmark(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }
    else if (RtfCmp(CurWord,"sstag")==0) { // tag begin
       ReturnCode=ReadRtfTag(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }

    // limited field support
    else if (RtfCmp(CurWord,"field")==0) { // field to read special characters
       ReturnCode=ReadRtfField(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }
    else if (RtfCmp(CurWord,"fldrslt")==0) { // field result to capture field data
       if (group[CurGroup].gflags&GFLAG_IN_MERGE_FIELD) group[CurGroup].FieldId=FIELD_DATA;
       else SkipRtfGroup(w,rtf);
    }
    else if (RtfCmp(CurWord,"formfield")==0) { // read formfield information
       ReturnCode=ReadRtfFormField(w,rtf);
       if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
       else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
    }

    // footnote
    else if (RtfCmp(CurWord,"footnote")==0) {// footnote
       group[CurGroup].style=group[CurGroup].style|FNOTETEXT;
       ResetUintFlag(group[CurGroup].style,FNOTEREST);
       if (CurGroup>0) group[CurGroup-1].style|=FNOTEREST; // fnoterest if follows will inherrit from the previous group 
    }
    else if (RtfCmp(CurWord,"ftnalt")==0) {  // endnote
       group[CurGroup].style=group[CurGroup].style|ENOTETEXT;
       
       if (rtf->flags2&RFLAG2_CHFTN_FOUND) {  // calculate automatic numbering for endnote
          rtf->FootnoteNo--;                  // reset the previous increment since it is not a footnote, but an endnote
                       
          rtf->EndnoteNo++;
          FmtRtfFootnoteNbr(w,rtf,TempString,rtf->EndnoteNo,EndnoteNumFmt);
          group[CurGroup].EndnoteMarker=TempString[0];
          lstrcpy(FootnoteRest,&(TempString[1]));     // copy the remaining part

          // fix the previous line
          if (rtf->InsLine>=0 && rtf->InsLine<TotalLines && LineLen(rtf->InsLine)==1) 
             pText(rtf->InsLine)[0]=TempString[0];

          ResetUintFlag(rtf->flags2,RFLAG2_CHFTN_FOUND);
       } 
    }
    else if (RtfCmp(CurWord,"chftn")==0) {// auto footnote numbering
       if (!(group[CurGroup].style&(FNOTETEXT))) { // new footnote
           rtf->FootnoteNo++;
           
           FmtRtfFootnoteNbr(w,rtf,&(rtf->OutBuf[rtf->OutBufLen]),rtf->FootnoteNo,FootnoteNumFmt);
           rtf->OutBufLen=lstrlen(rtf->OutBuf);
           rtf->OutBuf[rtf->OutBufLen]=0;
           //lstrcpy(FootnoteRest,&(TempString[1]));     // copy the remaining part

           if (!SendRtfText(w,rtf)) return RTF_END_FILE; // send the footnote marker

           if (lstrlen(FootnoteRest)>0) group[CurGroup].style|=FNOTEREST;  // to handle remaining part of the marker

           rtf->flags2|=RFLAG2_CHFTN_FOUND;
       }
       else {                                          // in footnote text body
           if (group[CurGroup].style&ENOTETEXT) 
                FmtRtfFootnoteNbr(w,rtf,&(rtf->OutBuf[rtf->OutBufLen]),rtf->EndnoteNo,EndnoteNumFmt);
           else FmtRtfFootnoteNbr(w,rtf,&(rtf->OutBuf[rtf->OutBufLen]),rtf->FootnoteNo,FootnoteNumFmt);
           rtf->OutBufLen=lstrlen(rtf->OutBuf);
       }
    }

    // special characters
    else if (RtfCmp(CurWord,"bullet")==0) {// bullet
       rtf->OutBuf[rtf->OutBufLen]=(BYTE)149;
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
    }
    else if (RtfCmp(CurWord,"emdash")==0) {// long dash character
       rtf->OutBuf[rtf->OutBufLen]=mbcs?'-':(BYTE)151;
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
    }
    else if (RtfCmp(CurWord,"endash")==0) {// short dash character
       rtf->OutBuf[rtf->OutBufLen]=mbcs?'-':(BYTE)150;
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
    }
    else if (RtfCmp(CurWord,"lquote")==0) {// left single quote
       rtf->OutBuf[rtf->OutBufLen]=mbcs?'\'':(BYTE)145;
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
    }
    else if (RtfCmp(CurWord,"rquote")==0) {// right single quote
       rtf->OutBuf[rtf->OutBufLen]=mbcs?'\'':(BYTE)146;
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
    }
    else if (RtfCmp(CurWord,"ldblquote")==0) {// right double quote
       rtf->OutBuf[rtf->OutBufLen]=mbcs?'"':(BYTE)147;
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
    }
    else if (RtfCmp(CurWord,"rdblquote")==0) {// right double quote
       rtf->OutBuf[rtf->OutBufLen]=mbcs?'"':(BYTE)148;
       rtf->OutBufLen++;
       rtf->OutBuf[rtf->OutBufLen]=0;           // NULL terination
    }

    // Skip unused control words
    else  {  // no-op control
       // skip these control
       if (group[rtf->GroupLevel].ControlCount==1 && rtf->IgnoreText) {// first control word in the group
          // skip these control words
          if (RtfCmp(CurWord,"caps")==0)    goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"deleted")==0) goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"expnd")==0)   goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"expndtw")==0) goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"kerning")==0) goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"outl")==0)    goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"revised")==0) goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"revauth")==0) goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"revdttm")==0) goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"scaps")==0)   goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"shad")==0)    goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"rtlch")==0)   goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"ltrch")==0)   goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"cchs")==0)    goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"lang")==0)    goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"widctlpar")==0) goto SKIP_CONTROL;
          if (RtfCmp(CurWord,"background")==0) goto SKIP_CONTROL;

          // skip the entire group
          ReturnCode=SkipRtfGroup(w,rtf);
          if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
          else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
       }
       else if (group[rtf->GroupLevel].ControlCount==1) {
          if (  RtfCmp(CurWord,"pntext")==0
             || RtfCmp(CurWord,"pntextb")==0) {
             // skip the entire group
             ReturnCode=SkipRtfGroup(w,rtf);
             if      (ReturnCode==RTF_FILE_INCOMPLETE) return RTF_FILE_INCOMPLETE;
             else if (ReturnCode==RTF_SYNTAX_ERROR)    return RTF_SYNTAX_ERROR;
          }
       }

       SKIP_CONTROL:
       ;
    }


    return 0;
}
#pragma optimize("",off)  // restore optimization

