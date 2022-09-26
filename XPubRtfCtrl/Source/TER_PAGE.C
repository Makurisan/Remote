/*==============================================================================
   TER_PAGE.C
   TER Page mode display functions.

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1991)
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

/******************************************************************************
    RequestPagination:
    Set the repagination flags
*******************************************************************************/
RequestPagination(PTERWND w, BOOL full)
{
    if (full) RepageBeginLine=0;
    PageModifyCount=TerArg.modified-1;        // to force repagination even if the document not modified

    PostMessage(hTerWnd,TER_IDLE,0,0L);  // induce idle time activity

    return TRUE;
}

/******************************************************************************
    TerRepaginate:
    Repaginate the document.
*******************************************************************************/
BOOL WINAPI _export TerRepaginate(HWND hWnd,BOOL repaint)
{
    PTERWND w;


    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    dm("TerRepaginate");

    if (TerFlags4&TFLAG4_NO_REPAGINATE) return TRUE;

    if (TerArg.PrintView) Repaginate(w,false,false,0,false);
    else                  TerRewrap(hWnd);

    if (repaint) PaintTer(w);

    dm("TerRepaginate - End");

    return TRUE;
}

/******************************************************************************
    Repaginate:
    Do the full word wrap of the entire file and calculate lines in each
    page. When the 'yield' flag is TRUE, this process is suspended if a
    keystroke message is pending. A TRUE value for the 'selective' argument
    causes only the pages between the hard page break or section break to be
    rewrapped.
    The 'LastPage' determines the page number when the pagination halts.
    Set 'LastPage' to 0 to paginate all pages
*******************************************************************************/
Repaginate(PTERWND w,BOOL yield,BOOL selective,int LastPage, BOOL repaint)
{
    long  l,CursorPos;
    int   SaveModified,PageNo;
    HCURSOR hSaveCursor=NULL;
    MSG   msg;
    long  InitRepageBeginLine=RepageBeginLine;
    int   i,ParaFID,sect,FirstPage,CellId;
    int   SaveCurPage=CurPage;      // save the current page number
    int   SaveFirstFramePage,SaveLastFramePage;
    BOOL  FullRepage=FALSE,completed=TRUE;
    long  HilightBeg,HilightEnd;
    BOOL  BegHilightAtLineEnd=FALSE,EndHilightAtLineEnd=FALSE,SelectAll=FALSE;
    LPWORD fmt;
    int   InitTotalPages=TotalPages;

    dm("Repaginate");

    if (!TerArg.PrintView) return FALSE;

    // update the window dimension in FittedView
    if (TerArg.FittedView && !yield && !selective && LastPage==0) GetWinDimension(w);


    if (!yield && !(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor); // show hour glass

    if (RepageBeginLine<0) RepageBeginLine=0;

    if (yield && TerArg.modified==PageModifyCount) return FALSE;  // pagination current
    if (yield && RepageBeginLine>=TotalLines) return FALSE; // completely repaginated
    if (yield && MessagePending(w)) {
        RepagePending=TRUE;
        //if (MessageId!=TER_IDLE) PostMessage(hTerWnd,TER_IDLE,0,0L);  // may cause looping in certain situation
        return FALSE;       // key/mouse message pending
    }

    // create table of contents and endnotes
    if (!yield) CreateToc(w);
    if (!yield || TerOpFlags&TOFLAG_UPDATE_ENOTE) CreateEndnote(w);
    if ((!yield && !InRtfRead) || TerOpFlags2&TOFLAG2_REPOS_PICT_FRAMES) ReposPictFrames(w);

    // save absolute cursor position
    CursorPos=RowColToAbs(w,CurLine,CurCol);

    // save the highlight position
    SaveWrapHilight(w,&HilightBeg,&HilightEnd,&BegHilightAtLineEnd,&EndHilightAtLineEnd,&SelectAll);

    PrevTotalPages=TotalPages;               // used for determining the length of the page fields
    SaveModified=TerArg.modified;


    if (!yield && !selective && LastPage==0) FullRepage=TRUE;
    if (TotalLines==1) FullRepage=TRUE;
    if (FullRepage) yield=FALSE;

    
    // decide beginning line of word wrapping
    FirstPage=0;            // starting page
    if (yield) {
       FirstPage=PageFromLine(w,RepageBeginLine,-1);
       CellId=cid(RepageBeginLine);
       if (FirstPage> 0 && CellId>0) FirstPage=TableAux[cell[CellId].row].FirstPage;

       for (i=0;i<TotalPages;i++) if (PageInfo[i].FirstLine>=TotalLines) break;  // this can happen if line count reduces sharply
       if (i<TotalPages) FirstPage=0;
       if (FirstPage>0 && RepageBeginLine==PageInfo[FirstPage].FirstLine) FirstPage--;   // do one more page
    }
    else  {
       if (selective) {     // start from the previous hard page break
          int pg;
          PageNo=PageFromLine(w,CurLine,-1);
          for (pg=PageNo;pg>0;pg--) if (PageInfo[pg].flags&PAGE_HARD) break;  // hard page break;
          FirstPage=pg-1;
          if (FirstPage<0) FirstPage=0;

          for (pg=(PageNo+1);pg<TotalPages;pg++) if (PageInfo[pg].flags&PAGE_HARD) break;  // look for next hard page break;
          LastPage=pg-1;
       }
    }

    // initialize the variables
    repaginating=TRUE;       // in repagination now
    RepagePending=FALSE;
    SaveFirstFramePage=FirstFramePage;  // save the first and last frame pages
    SaveLastFramePage=LastFramePage;


    SetSectPageSize(w);      // set the page size for the sections

    sect=GetSection(w,0);
    if (TerSect[sect].flags&SECT_RESTART_PAGE_NO) PageInfo[0].DispNbr=TerSect[sect].FirstPageNo;
    else                                          PageInfo[0].DispNbr=1;
    if (FullRepage) TerSect1[sect].PrevSect=-1;   // first section of the document

    if (FirstPage==0) {
       for (i=0;i<TotalSects;i++) {
         if (FullRepage || i==sect) {
            TerSect1[i].hdr.height=(int)(PrtResY*(TerSect[i].TopMargin-TerSect[i].HdrMargin));
            TerSect1[i].ftr.height=(int)(PrtResY*(TerSect[i].BotMargin-TerSect[i].FtrMargin));
            if (TerSect1[i].hdr.height<0) TerSect1[i].hdr.height=0;
            if (TerSect1[i].ftr.height<0) TerSect1[i].ftr.height=0;
            TerSect1[i].ftr.TextHeight=0;
         }
       }
    }


    // initialization when doing full repagination
    if (FullRepage) {
       for (i=1;i<TotalParaFrames;i++) {
           ParaFrame[i].InUse=FALSE; //reset temporarily
           ResetUintFlag(ParaFrame[i].flags,(PARA_FRAME_POSITIONED|PARA_FRAME_PAGE_BREAK));
       }
    }
    else {  // set PARA_FRAME_POSITIONED flags for all the frames found in text
       for (i=1;i<TotalParaFrames;i++) if (!(ParaFrame[i].flags&PARA_FRAME_OBJECT)) break;
       if (i<TotalParaFrames) {  // moveable paraframe used
          for (i=1;i<TotalParaFrames;i++) ParaFrame[i].InUse=FALSE;
          for (l=0;l<TotalLines;l++) {  // set PARA_FRAME_POSITIONED flag for all the frames in use
             if (fid(l)) ParaFrame[fid(l)].InUse=TRUE;
             if (LineFlags(l)&LFLAG_PICT) {
                 fmt=OpenCfmt(w,l);
                 for (i=0;i<LineLen(l);i++) {
                    if (IsFramePict(w,fmt[i])) {
                       ParaFID=TerFont[fmt[i]].ParaFID;
                       ParaFrame[ParaFID].InUse=TRUE;
                    }
                 }
                 CloseCfmt(w,l);
             }
          }
       }
    }

    // create pages
    LastWrappedLine=-1;
    PageNo=FirstPage;
    LastPageCreated=FALSE;
    while (!LastPageCreated && (LastPage==0 || PageNo<=LastPage)) {
       CreateOnePage(w,PageNo,FullRepage);
       PageNo++;

       // yield for caret blink and set cursor shape
       if (yield && PeekMessage(&msg,hTerWnd,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_REMOVE)) {
          if (IgnoreMouseMove && ShowHyperlinkCursor) TerSetCursorShape(w,msg.lParam,FALSE);
       }

       // check if we need to yield to other events
       if (yield) {
          if (TotalLines<SMALL_FILE) {
             if (MessagePending(w)) {
                RepagePending=TRUE;
                break;
             }
          }
          else {
            while (PeekMessage(&msg,hTerWnd,WM_TIMER,WM_TIMER,PM_REMOVE));  // remove any timer messages

            if ( PeekMessage(&msg,hTerWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE|PM_NOYIELD)
              || PeekMessage(&msg,hTerWnd,WM_KEYDOWN,WM_KEYDOWN,PM_NOREMOVE|PM_NOYIELD)
              || PeekMessage(&msg,hTerWnd,WM_NCHITTEST,WM_NCHITTEST,PM_NOREMOVE|PM_NOYIELD)
              || PeekMessage(&msg,hTerWnd,WM_SETCURSOR,WM_SETCURSOR,PM_NOREMOVE|PM_NOYIELD)
              || PeekMessage(&msg,hTerWnd,WM_NCMOUSEMOVE,WM_NCMOUSEMOVE,PM_NOREMOVE|PM_NOYIELD)
              || PeekMessage(&msg,0,WM_MOUSEFIRST,WM_MOUSELAST,PM_NOREMOVE|PM_NOYIELD) ) {
                RepagePending=TRUE;
                completed=FALSE;
                //if (MessageId!=TER_IDLE) PostMessage(hTerWnd,TER_IDLE,0,0L);  // may cause looping in certain situation
                break;
             }
          }
       }
    }


    // restore cursor position
    AbsToRowCol(w,CursorPos,&CurLine,&CurCol);
    if (BeginLine>=TotalLines) BeginLine=TotalLines-1;
    if (BeginLine>CurLine) BeginLine=CurLine;
    CurRow = CurLine-BeginLine;
    if (CurRow>=WinHeight) {CurRow=WinHeight-1;BeginLine=CurLine-CurRow;}

    // restore the hilight position
    RestoreWrapHilight(w,HilightBeg,HilightEnd,BegHilightAtLineEnd,EndHilightAtLineEnd,SelectAll);


    // update table of content
    UpdateToc(w);        // update table of contents

    // Repair table structure if necessary
    RepairTable(w); // Repair table structure
    RecreateSections(w);

    // repagination completed?
    if (LastPageCreated) {
       PageModifyCount=TerArg.modified=SaveModified;
       RepageBeginLine=TotalLines;  // must be before PaintTer because PaintTer->ReposPageHdrFtr->RecreateSections can modify RepageBeginLine
       if (TerOpFlags&TOFLAG_UPDATE_ENOTE) ResetLongFlag(TerOpFlags,TOFLAG_UPDATE_ENOTE);
    }
    else {
       TerArg.modified=SaveModified;
       if (PageNo==TotalPages) RepageBeginLine=TotalLines;
       else                    RepageBeginLine=PageInfo[PageNo].FirstLine;
    }

    // test if need to repaged
    if (hSaveCursor) SetCursor(hSaveCursor);

    // get the current page
    CurPage=SaveCurPage;
    if (CurPage>=TotalPages) CurPage=TotalPages-1;
    CurPage=PageFromLine(w,CurLine,CurPage);
    PrevTotalPages=TotalPages;               // used for determining the length of the page fields

    repaginating=FALSE;                      // out of repagination process now

    if (CurPage>=SaveFirstFramePage && CurPage<=SaveLastFramePage) 
         CreateFrames(w,FALSE,SaveFirstFramePage,SaveLastFramePage);   // recreate the page frames
    else {
       if (SaveCurPage==(CurPage-1)) CreateFrames(w,FALSE,CurPage-1,CurPage);   // create the current page frame
       else                          CreateFrames(w,FALSE,CurPage,CurPage);   // create the current page frame
    }

    if (InitTotalPages==1 && PageResized(w)) return true;  // check if the page size needs to be resized - when TFLAG5_VARIABLE_PAGE_SIZE set

    if (repaint)  PaintTer(w);

    dm("Repaginate - End");

    return completed;
}

/******************************************************************************
    CreateOnePage:
    This routine calculate individual page boundaries.  This functino returns
    TRUE if repainting is needed.
*******************************************************************************/
CreateOnePage(PTERWND w, int PageNo, BOOL FullRepage)
{
   long l,FirstSectLine,FirstColLine,FirstRowGroupLine[MAX_TBL_LEVELS],LineDone=-1,
        PageLastLine,FirstRowLine[MAX_TBL_LEVELS];
   int  i,sect,CurPgHeight,PrintHeight,PageColHeight,CellHeight[MAX_TBL_LEVELS],
        SectBeginY,CurSectCol,CurCell[MAX_TBL_LEVELS],CurRowId[MAX_TBL_LEVELS],
        HdrHeight,FtrHeight,FirstGroupRow[MAX_TBL_LEVELS];
   BOOL NewPage,PagingEnabled,HasFrames=FALSE,restart,RowSpanned=FALSE;
   UINT pflags;
   BYTE InHdrFtr,CurHdrFtr,PrevHdrFtr;
   int  ParaFID,PrevParaFID,SkipCell[MAX_TBL_LEVELS];
   int  PrevSect,PrevCellsWidth[MAX_TBL_LEVELS];
   int  BefHfCurPgHeight,BefHfPageColHeight,BefHfSectBeginY;
   int  BefFrmCurPgHeight,BefFrmPageColHeight,BefFrmSectBeginY,BefFrmSectCol;
   int  TopSect,HoldCurPgHeight,FrmSpcBef,SkippingCell=0;
   short RestartPageNo;
   int  CurX,CurY,ColumnWidth,ColumnSpace,YBefHdr,TblHdrHt;
   long FirstLine,LastLine,FirstSkippedLine=0;
   int  pass=0,SpcBefRow,RowBeginY[MAX_TBL_LEVELS],MinCellHeight[MAX_TBL_LEVELS],FnoteHt;
   BOOL FirstSectPage,KeepRowTogether[MAX_TBL_LEVELS],InPartBotRow[MAX_TBL_LEVELS],InPartTopRow[MAX_TBL_LEVELS];
   int  SaveLineY,SubTableHt,CurLineHt,level,PrevLevel,SkipLevel,TableX[MAX_TBL_LEVELS],TempCol;
   long LastLineDone,LastPictLine,LineAftTableHdr,TempLine,LimitFtrLine,LastPictPos;
   BOOL AdjustLastLine,BeginNewCells;
   int  FirstLevel,LastLevel,EndPage,EndCell,HeightFromPageTop,LimitFtrHt,HalfPgHeight;
   BOOL PageBreakBeforePara,CheckPageBreakAfterFrame;
   float FtrMargin;

   dm("CreateOnePage");

   // enlarge page structure if necessary
   CheckPageSpace(w,PageNo);

   // initiaze for page creation
   LastPageCreated=FALSE;
   LastPictLine=LastPictPos=0;

   // get the first page line number for the page
   FirstLine=PageInfo[PageNo].FirstLine;
   if ((PageNo+1)>=TotalPages) LastLine=TotalLines-1;
   else                        LastLine=PageInfo[PageNo].LastLine;


   // set line and para frame flags
   if (!FullRepage) {
      for (i=1;i<TotalParaFrames;i++) {
         if (ParaFrame[i].PageNo==PageNo) {
            //ParaFrame[i].InUse=FALSE; //reset temporarily
            ResetUintFlag(ParaFrame[i].flags,(PARA_FRAME_POSITIONED|PARA_FRAME_PAGE_BREAK));
         }
      }
      LastWrappedLine=FirstLine-1;
   }
   ResetLinePictPos(w,FirstLine);  // the framed picture on the first line might have been set on the previous page

   CalcTblHdrHt(w,PageNo);// calculate the height of any table header rows on the top of the page


   INITIALIZE:

   if ((LastLine+1)>=TotalLines) LastLine=TotalLines-1;
   for (l=FirstLine;l<=LastLine;l++) {
      int FrameId=fid(l);
      
      if (LinePage(l)!=PageNo) continue;   // line not on this page
      ResetLongFlag(LineFlags(l),(LFLAG_SOFT_COL|LFLAG_PICT_SPACE));   // reset the soft column break
      if (FrameId>0 && FrameId<TotalParaFrames) ParaFrame[FrameId].InUse=TRUE;
   }
   PageLastLine=FirstLine;


   // get section and the header height
   sect=TopSect=GetSection(w,FirstLine); // section for the first line, and top of the page

   PageInfo[PageNo].TopSect=TopSect;
   if (PageNo==0 || (PageInfo[PageNo-1].TopSect!=TopSect && sect!=GetSection(w,FirstLine-1)))  // first page header/footer for the subsequent section is printed only if section starts on the new page
         PageInfo[PageNo].flags|=PAGE_FIRST_SECT_PAGE;
   else  ResetUintFlag(PageInfo[PageNo].flags,PAGE_FIRST_SECT_PAGE);

   FirstSectPage=(PageInfo[PageNo].flags&PAGE_FIRST_SECT_PAGE);
   HdrHeight=PageHdrHeight2(w,PageNo,FALSE,true);
   FtrHeight=PageFtrHeight(w,PageNo,FALSE);

   ParaFID=0;                       // para frame id
   RestartPageNo=0;                 // non-zero to start at a specific number
   BefHfCurPgHeight=0;              // save header/footer cur page height
   BefHfPageColHeight=0;
   BefHfSectBeginY=0;
   BefFrmCurPgHeight=0;
   BefFrmPageColHeight=0;
   BefFrmSectBeginY=0;
   BefFrmSectCol=0;
   PagingEnabled=TRUE;              // pagination enabled
   FirstSectLine=FirstLine;         // first line of this section
   FirstColLine=FirstLine;          // first line of the column
   restart=FALSE;                   // restart this page
   PageInfo[PageNo].FnoteHt=FnoteHt=0; // footnote height
   PageInfo[PageNo].FirstRow=PageInfo[PageNo].LastRow=0;
   LineAftTableHdr=-1;              // not set yet
   SubTableHt=0;                    // sub table height
   level=PrevLevel=TableLevel(w,FirstLine);
   SkipLevel=0;                     // levels to be ignored, 0 means not to ignore any level
   DoExtraPass=FALSE;               // TRUE to do an extra pass during pagination
   LimitFtrLine=0;                  // last footer line that can be contained on the page
   CurHdrFtr=0;                     // header/footer status
   SkippingCell=0;                  // The cell being skipped currently

   for (i=0;i<MAX_TBL_LEVELS;i++) {    // initialize table information at each level
      FirstRowGroupLine[i]=FirstLine;
      FirstRowLine[i]=FirstLine;
      FirstGroupRow[i]=0;              // first row of the row group
      PrevCellsWidth[i]=0;             // width of the previous cells in the table row
      SkipCell[i]=0;                   // cell to skip at the level
      CurCell[i]=0;                    // current cell
      CurRowId[i]=0;                   // current row id
      CellHeight[i]=0;                 // cell height
      TableX[i]=0;                     // Distance of the tableX from the margin (or Frame begin x)
      KeepRowTogether[i]=0;
      InPartTopRow[i]=FALSE;
      InPartBotRow[i]=FALSE;
      
      //if (i<=level) InPartTopRow[i]=(FirstLine>0 && cid(FirstLine)>0 && LevelCell(w,i,FirstLine-1)==LevelCell(w,i,FirstLine));  // PageNo>0 && PageInfo[PageNo-1].BotRow>0);
      
      if (i<=level && FirstLine>0 && cid(FirstLine)>0) {
          int CurPageCell=LevelCell(w,i,FirstLine);      // cell on the current page
          int PrevPageCell=LevelCell(w,i,FirstLine-1);   // cell on the previous page
          if (cell[CurPageCell].row==cell[PrevPageCell].row) InPartTopRow[i]=TRUE;
      }
   } 

   //if (InPartTopRow) PageInfo[PageNo].TopRow=cell[cid(FirstLine)].row;

   HalfPgHeight=(int)((TerSect1[sect].PgHeight/2)*PrtResY);  // half page height
   
   FtrMargin=TerSect[sect].FtrMargin;
   if (FtrHeight==0) FtrMargin=TerSect[sect].BotMargin;     // no footer, so footer margin not used

   PrintHeight=HeightFromPageTop=(int)((TerSect1[sect].PgHeight-FtrMargin)*PrtResY);  // height of the print area
   if (HdrHeight>0) PrintHeight-=(int)(TerSect[sect].HdrMargin*PrtResY);
   else             PrintHeight-=(int)(TerSect[sect].TopMargin*PrtResY);

   if (TerArg.FittedView && !InPrinting) PrintHeight*=GetPageMultiple(w);  // double up for display

   PageInfo[PageNo].BodyHt=(PrintHeight-FtrHeight-FnoteHt-HdrHeight);

   GetSectColWidthSpace(w,sect,sect,&ColumnWidth,&ColumnSpace,&CurX,&YBefHdr);  // section column width and space

   CurSectCol=0;            // current column for the section
   PageColHeight=0;         // height of the text on all columns on the page
   NewPage=FALSE;           // new page indicator
   InHdrFtr=0;              // TRUE when in page header or footer
   PrevHdrFtr=0;            // hdr/ftr status of the previous line

   TblHdrHt=PageInfo[PageNo].TblHdrHt;
   if (TblHdrHt>=(PrintHeight-HdrHeight-FtrHeight-TwipsToPrtY(1440))) TblHdrHt=0;
   PageInfo[PageNo].TblHdrHt=TblHdrHt;

   CurPgHeight=SectBeginY=HdrHeight+TblHdrHt; // hard section break pushed to the next page

   PosWatermarkFrame(w,PageNo);                 // calculate the position of the watermark para-frame

   for (l=FirstLine;l<TotalLines;l++) {
      if (l>LastWrappedLine) WrapMoreLines(w,sect);     // wrap more lines

      if (l>=TotalLines) break;

      if (l>0) PrevLevel=TableLevel(w,l-1);        // previous table level
      level=TableLevel(w,l);                       // current table level

      // check if the current cell is being skipped
      if (SkippingCell>0) {        // this logic is used to skip the cell for part-bot-row (not for part-top-row)
         if (cid(l)==SkippingCell && LineInfo(w,l,INFO_CELL)) SkippingCell=0;  // no longer being skipped
         else {
            if (cid(l)==SkippingCell || level>cell[SkippingCell].level) {  // same cell or children of the same cell 
              if (LinePage(l)<=PageNo && l>=FirstSkippedLine) LinePage(l)=PageNo+1;    // ? don't ues the <= condition otherwise the lines for the previous pages will also be pushed to next page
              continue;
            }
            else SkippingCell=0;
         }
      }


      if (SkipCellLine(w,l,level,PrevLevel,InPartBotRow,InPartTopRow,PageNo, 
             SkipCell,&SkipLevel,CurCell,CurRowId,&PageColHeight,
             &CurPgHeight,TableX,PrevCellsWidth,&EndPage,CellHeight,&EndCell)) {
          if (EndCell) EndCellOnPage(w,l,PageNo,InPartTopRow[level],InPartBotRow[level],&CurCell[level],&MinCellHeight[level],
                                     &PageColHeight,&CurPgHeight,CellHeight[level],&RowSpanned);
          if (InPartBotRow[level] && !LineInfo(w,l,INFO_CELL)) {
              SkippingCell=cid(l);  // this cell has not ended yet, so skip the remaining lines of this cell
              FirstSkippedLine=l;
          }
          continue;
      }
      if (EndPage) {
         l=AdjustForParaKeep(w,l,FirstColLine,sect==TopSect);  // adjust for paragraph keep together/next
         CurLineHt=LineHt(l);                    // update CurLineHt variable
         AdjustLastLine=TRUE;

         CheckPageSpace(w,PageNo+1);       // enlarge page structre if necessary
         PageInfo[PageNo].LastLine=PageLastLine;
         PageInfo[PageNo+1].FirstLine=l;   // line where new page begins
         ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);
         PageInfo[PageNo+1].TopSect=PageInfo[PageNo].TopSect;
         break;
      }

      // process the Subtable
      CurLineHt=LineHt(l);
      AdjustLastLine=FALSE;


      if (LineFlags(l)&LFLAG_FNOTETEXT) {
         FnoteHt+=ExtractFootnote(w,hTerDC,0,0,l,sect,FALSE,FALSE);  // get the footnote height
         if (PageInfo[PageNo].FnoteHt==0 && FnoteHt>0) FnoteHt+=2*TwipsToPrtY(FNOTE_MARGIN);      // add top and bottom margin
         PageInfo[PageNo].FnoteHt=FnoteHt;
      }

      PrevParaFID=ParaFID;
      PrevHdrFtr=CurHdrFtr;

      pflags=PfmtId[pfmt(l)].flags;
      ParaFID=fid(l);
      CurHdrFtr=0;
      if (pflags&PAGE_HDR_FTR) {  // find out which header/footer we are in now
         CurHdrFtr=InHdrFtr;
         if (LineFlags(l)&LFLAG_HDR)  CurHdrFtr=HDR_CHAR;
         if (LineFlags(l)&LFLAG_FTR)  CurHdrFtr=FTR_CHAR;
         if (LineFlags(l)&LFLAG_FHDR) CurHdrFtr=FHDR_CHAR;
         if (LineFlags(l)&LFLAG_FFTR) CurHdrFtr=FFTR_CHAR;
      }


      if (pflags&PAGE_HDR_FTR || ParaFID>0) PagingEnabled=FALSE;
      else                                  PagingEnabled=TRUE;

      // save and restore the height variables on header/footer break
      HoldCurPgHeight=CurPgHeight;  // copy before it is change here

      // *** Restore height when exiting header/footer/frames ***

      // exiting a frame to no-frame, restore the height variables
      if (ParaFID==0 && PrevParaFID>0) {
         CurPgHeight=BefFrmCurPgHeight; // restore the height variables
         PageColHeight=BefFrmPageColHeight;
         SectBeginY=BefFrmSectBeginY;
         CurSectCol=BefFrmSectCol;
      }

      // exiting a frame to another frame
      if (ParaFID>0 && PrevParaFID>0 && ParaFID!=PrevParaFID) {
         CurPgHeight=BefFrmCurPgHeight; // restore the height variables
         PageColHeight=BefFrmPageColHeight;
         SectBeginY=BefFrmSectBeginY;
         CurSectCol=BefFrmSectCol;
         ParaFrame[ParaFID].TextLine=l;    // first text line for the frame
      }

      // exiting header or footer
      if (InHdrFtr && CurHdrFtr==0) {     // going into regular text
         if (TerSect1[sect].PrevSect<0 || TerSect[sect].flags&SECT_NEW_PAGE) {
            if (InHdrFtr==HDR_CHAR && CurHdrFtr!=HDR_CHAR) {
               TerSect1[sect].hdr.height=HoldCurPgHeight;
               if ((PrtToInchesY(TerSect1[sect].hdr.height)+TerSect[sect].HdrMargin)<TerSect[sect].TopMargin)
                  TerSect1[sect].hdr.height=(int)(PrtResY*(TerSect[sect].TopMargin-TerSect[sect].HdrMargin));
            }
            if (InHdrFtr==FHDR_CHAR && CurHdrFtr!=FHDR_CHAR) {
               TerSect1[sect].fhdr.height=HoldCurPgHeight;
               if ((PrtToInchesY(TerSect1[sect].fhdr.height)+TerSect[sect].HdrMargin)<TerSect[sect].TopMargin)
                  TerSect1[sect].fhdr.height=(int)(PrtResY*(TerSect[sect].TopMargin-TerSect[sect].HdrMargin));
            }
            
            HdrHeight=PageHdrHeight2(w,PageNo,FALSE,true);  // refresh the header height variable
            if (HdrHeight>0) PrintHeight=HeightFromPageTop-(int)(TerSect[sect].HdrMargin*PrtResY);
            FtrHeight=PageFtrHeight(w,PageNo,FALSE);   // refresh the footer height variable

            CurPgHeight=SectBeginY=(HdrHeight+TblHdrHt);
            PageColHeight=0;
         }
         else {          // resume the page
            CurPgHeight=BefHfCurPgHeight;           // restore the height variables
            PageColHeight=BefHfPageColHeight;
            SectBeginY=BefHfSectBeginY;
         }
      }


      // **** Save height when entering header/footer/frame ***

      // entering a frame from no-frame, save the height variables
      if (ParaFID && ParaFID!=PrevParaFID && ParaFrame[ParaFID].PageNo!=PageNo) {
          ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_POSITIONED);        // reposition
      }

       
      if (ParaFID>0 && PrevParaFID==0) {
         BefFrmCurPgHeight=CurPgHeight; // save the height variables
         BefFrmPageColHeight=PageColHeight;
         BefFrmSectBeginY=SectBeginY;
         BefFrmSectCol=CurSectCol;
         ParaFrame[ParaFID].TextLine=l;    // first text line for the frame
      }

      // save variable when entering a header/footer area
      if (CurHdrFtr && !InHdrFtr) {
         BefHfCurPgHeight=CurPgHeight; // save the height variables
         BefHfPageColHeight=PageColHeight;
         BefHfSectBeginY=SectBeginY;
      }

      // initialize variables at header/footer break
      if (  (!InHdrFtr && CurHdrFtr)            // entering header/footer
         || (InHdrFtr==HDR_CHAR && CurHdrFtr!=HDR_CHAR)     // exiting header
         || (InHdrFtr==FTR_CHAR && CurHdrFtr!=FTR_CHAR)     // exiting footer
         || (InHdrFtr==FHDR_CHAR && CurHdrFtr!=FHDR_CHAR)   // exiting first header
         || (InHdrFtr==FFTR_CHAR && CurHdrFtr!=FFTR_CHAR)   // exiting first footer
         || (ParaFID !=PrevParaFID && ParaFID>0) ) {      // entering a frame

         CurSectCol=0;            // current column for the section
         CellHeight[level]=0;     // height of table row
         CurCell[level]=0;
         CurRowId[level]=0;              // current row id

         // record the header/footer height
         if (InHdrFtr==HDR_CHAR && CurHdrFtr!=HDR_CHAR) {
            TerSect1[sect].hdr.height=HoldCurPgHeight;
            if ((PrtToInchesY(TerSect1[sect].hdr.height)+TerSect[sect].HdrMargin)<TerSect[sect].TopMargin)
               TerSect1[sect].hdr.height=(int)(PrtResY*(TerSect[sect].TopMargin-TerSect[sect].HdrMargin));
            HdrHeight=PageHdrHeight2(w,PageNo,FALSE,true);  // refresh the header height variable
         }
         if (InHdrFtr==FHDR_CHAR && CurHdrFtr!=FHDR_CHAR) {
            TerSect1[sect].fhdr.height=HoldCurPgHeight;
            if ((PrtToInchesY(TerSect1[sect].fhdr.height)+TerSect[sect].HdrMargin)<TerSect[sect].TopMargin)
               TerSect1[sect].fhdr.height=(int)(PrtResY*(TerSect[sect].TopMargin-TerSect[sect].HdrMargin));
            HdrHeight=PageHdrHeight2(w,PageNo,FALSE,true);  // refresh the header height variable
         }
         if (InHdrFtr==FTR_CHAR && CurHdrFtr!=FTR_CHAR) {
            TerSect1[sect].ftr.height=TerSect1[sect].ftr.TextHeight=(LimitFtrHt?LimitFtrHt:HoldCurPgHeight);
            TerSect1[sect].ftr.LimitFtrLine=LimitFtrLine;

            if ((PrtToInchesY(TerSect1[sect].ftr.height)+TerSect[sect].FtrMargin)<TerSect[sect].BotMargin)
                TerSect1[sect].ftr.height=(int)(PrtResY*(TerSect[sect].BotMargin-TerSect[sect].FtrMargin));
            FtrHeight=PageFtrHeight(w,PageNo,FALSE);   // refresh the footer height variable
         }
         if (InHdrFtr==FFTR_CHAR && CurHdrFtr!=FFTR_CHAR) {
            TerSect1[sect].fftr.height=TerSect1[sect].fftr.TextHeight=(LimitFtrHt?LimitFtrHt:HoldCurPgHeight);
            TerSect1[sect].fftr.LimitFtrLine=LimitFtrLine;
            
            if ((PrtToInchesY(TerSect1[sect].fftr.height)+TerSect[sect].FtrMargin)<TerSect[sect].BotMargin)
                TerSect1[sect].fftr.height=(int)(PrtResY*(TerSect[sect].BotMargin-TerSect[sect].FtrMargin));
            FtrHeight=PageFtrHeight(w,PageNo,FALSE);   // refresh the footer height variable
         }
         if (HdrHeight>0) PrintHeight=HeightFromPageTop-(int)(TerSect[sect].HdrMargin*PrtResY);

         // set the new CurPgHeight
         if (CurHdrFtr) {
            CurPgHeight=PageColHeight=SectBeginY=LimitFtrHt=0;
            LimitFtrLine=0;
         }

      }

      // record page header and footer
      if (CurHdrFtr==HDR_CHAR) {
         if (l!=(TerSect1[sect].hdr.LastLine+1)) TerSect1[sect].hdr.FirstLine=-1;  // apply contiguous condition
         if (TerSect1[sect].hdr.FirstLine<0) TerSect1[sect].hdr.FirstLine=l;
         TerSect1[sect].hdr.LastLine=l;
         InHdrFtr=HDR_CHAR;
      }
      else if (CurHdrFtr==FHDR_CHAR) {
         if (l!=(TerSect1[sect].fhdr.LastLine+1)) TerSect1[sect].fhdr.FirstLine=-1;  // apply contiguous condition
         if (TerSect1[sect].fhdr.FirstLine<0) TerSect1[sect].fhdr.FirstLine=l;
         TerSect1[sect].fhdr.LastLine=l;
         InHdrFtr=FHDR_CHAR;
      }
      else if (CurHdrFtr==FTR_CHAR) {
         if (l!=(TerSect1[sect].ftr.LastLine+1)) TerSect1[sect].ftr.FirstLine=-1;  // apply contiguous condition
         if (TerSect1[sect].ftr.FirstLine<0) TerSect1[sect].ftr.FirstLine=l;
         TerSect1[sect].ftr.LastLine=l;
         InHdrFtr=FTR_CHAR;
      }
      else if (CurHdrFtr==FFTR_CHAR) {
         if (l!=(TerSect1[sect].fftr.LastLine+1)) TerSect1[sect].fftr.FirstLine=-1;  // apply contiguous condition
         if (TerSect1[sect].fftr.FirstLine<0) TerSect1[sect].fftr.FirstLine=l;
         TerSect1[sect].fftr.LastLine=l;
         InHdrFtr=FFTR_CHAR;
      }
      else InHdrFtr=0;


      // assign page number to the frame
      if (ParaFID>0 && ParaFID!=PrevParaFID) {
         if (!(ParaFrame[ParaFID].flags&PARA_FRAME_OBJECT)) {  // entering a frame
            // check if the non-frame line beyond this frame can fit on this page, otherwise move frame to the next page
            long AfterLine;
            for (AfterLine=l+1;AfterLine<TotalLines;AfterLine++) if (fid(AfterLine)==0) break;
            if (AfterLine<TotalLines
                 && ((LineHt(AfterLine)+CurPgHeight)>(PrintHeight-FtrHeight-FnoteHt))
                 && InHdrFtr==0
                 && cid(AfterLine)==0
                 && l>FirstColLine           // at least one line in the current column
                 && CurSectCol==(TerSect[sect].columns-1)
                 && PageNo<(MAX_PAGES-1)) {  // next page begins, not allowed within a table row or hdr footer
         
               LinePage(l)=PageNo+1;                     // pushed to the next page
               PageLastLine--;

               PageInfo[PageNo+1].FirstLine=l;   // create a soft page break here.
               PageInfo[PageNo].LastLine=PageLastLine;
               ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);
               break;
            }
         }    
       
         ParaFrame[ParaFID].PageNo=PageNo;
      }

      // position the previous frames
      if (PrevParaFID>0 && PrevParaFID!=ParaFID && !(ParaFrame[PrevParaFID].flags&PARA_FRAME_WRAP_THRU)) {
         if (!(ParaFrame[PrevParaFID].flags&PARA_FRAME_POSITIONED)
            && (!PrevHdrFtr) && (!CurHdrFtr || ViewPageHdrFtr)) restart=TRUE;
         HasFrames=TRUE;
      }

      // check if the pict frames caues a page break
      if (l>FirstLine && LineFlags(l)&LFLAG_PICT && IsPictPageBreak(w,l,PageNo)
         && !(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) && fid(l)==0 && cid(l)==0) {
         LinePage(l)=PageNo+1;                     // pushed to the next page
         PageLastLine=l-1;

         PageInfo[PageNo+1].FirstLine=l;   // create a soft page break here.
         PageInfo[PageNo].LastLine=PageLastLine;
         ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);
         break;
      }

      if (l>FirstLine && LineFlags(l-1)&LFLAG_PICT && HasUnpositionedPict(w,l-1,PageNo)
         && (!(PfmtId[pfmt(l-1)].flags&PAGE_HDR_FTR) || ViewPageHdrFtr)) {
           restart=TRUE;
           LastPictPos=RowColToAbs(w,l-1,0);
      }

       
      // restart to position the frames
      if (restart && ParaFID==0 && !CurHdrFtr && l>LineDone) {
         PageInfo[PageNo+1].FirstLine=l;
         PageInfo[PageNo].LastLine=PageLastLine;
         ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);

         if (TotalPages<(PageNo+2)) TotalPages=PageNo+2;
         CreateFrames(w,FALSE,PageNo,PageNo);   // create the last page frame

         PageInfo[PageNo+1].FirstLine=TotalLines;   // to allow the CalcFrameSpace page number calculation to work temporarily
         PageInfo[PageNo].LastLine=TotalLines-1;
         ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);

         LastWrappedLine=FirstLine-1;     // restart word wrapping
         HasFrames=TRUE;
         LineDone=l;
         goto INITIALIZE;
      }

      // check for hard page break, section break and row break
      PageBreakBeforePara=(PfmtId[pfmt(l)].pflags&PFLAG_PAGE_BREAK) && (LineFlags(l)&LFLAG_PARA_FIRST) && !cid(l) && !fid(l) && PagingEnabled;
      if (l>0 && (LinePage(l-1)<PageNo || LineFlags(l-1)&LFLAG_HDRS_FTRS)) PageBreakBeforePara=FALSE;
      if ((tabw(l) && tabw(l)->type&(INFO_SECT|INFO_PAGE|INFO_ROW|INFO_COL))) PageBreakBeforePara = FALSE;
      if (l==FirstColLine) PageBreakBeforePara=false;

      NewPage=FALSE;
      CheckPageBreakAfterFrame=TRUE;


      if ((tabw(l) && tabw(l)->type&(INFO_SECT|INFO_PAGE|INFO_ROW|INFO_COL)) || PageBreakBeforePara) {
         int PgHeightBefBreak=CurPgHeight;
         FrmSpcBef=GetFrmSpcBef(w,l,FALSE);
         CurPgHeight+=(CurLineHt+FrmSpcBef);    // add the break line height
         PageColHeight+=(CurLineHt+FrmSpcBef);
         PageLastLine=l;
         LinePage(l)=PageNo;

         // process section break
         if (LineInfo(w,l,INFO_SECT) && PagingEnabled && CurRowId[level]==0) {
            // calculate the total height of the section on this page
            BOOL PageOverflow=FALSE;
            long SaveLastLine=l;
            int  ColAdjStep=PrtResY/4,NewPageColHeight;  // adjust column length by 1/2 inches steps
            int  MaxColHeight=PrintHeight-FtrHeight-SectBeginY;

            // adjust the column length
            if (TerSect[sect].columns>1) {
               PageColHeight=PageColHeight/TerSect[sect].columns+1;
               while (PageColHeight<=MaxColHeight) {
                  l=SaveLastLine;
                  NewPageColHeight=AdjustSectColHeight(w,TerSect[sect].columns,PageColHeight,FirstSectLine,&l,MaxColHeight,sect==TopSect);
                  CurLineHt=LineHt(l);        // update CurLineHt variable
                  if (l==SaveLastLine) break; // last line (section break) contained in the page

                  if (PageColHeight>=MaxColHeight) {
                     AdjustLastLine=TRUE;
                     break;
                  }

                  PageColHeight+=ColAdjStep; // section break being pushed to the next page
                  if (PageColHeight>MaxColHeight) PageColHeight=MaxColHeight;
               }
               PageColHeight=NewPageColHeight;
               if (l<SaveLastLine) {
                  if (l>FirstColLine) PageOverflow=TRUE;  // section break pushed to the next page
                  else {
                     l=SaveLastLine;
                     CurLineHt=LineHt(l);   // update CurLineHt variable
                  }
               }
            }
            else {                          // check if section line is too tall to fit on this page

               PageOverflow=(PgHeightBefBreak>(PrintHeight-FtrHeight-FnoteHt))
                             && PagingEnabled && l>FirstColLine;           // at least one line in the current column
               if (PageOverflow) {
                  LinePage(l)=PageNo+1;                     // pushed to the next page
                  PageLastLine--;
               }
            } 

            // hard section break pushed to the next page?
            if (PageOverflow) {                  // the hard section break being pushed to the next page
               PageInfo[PageNo+1].FirstLine=l;   // create a soft page break here.
               PageInfo[PageNo].LastLine=PageLastLine;
               ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);

               break;
            }
            else {
               TerSect[sect].LastPage=PageNo;   // last page for the section
               TerSect1[sect].LastPageHeight=PageColHeight;
               TerSect[sect].LastLine=l;

               // begin new section
               PrevSect=sect;
               sect=TerSect1[PrevSect].NextSect;
               if (sect<0) {
                  RecreateSections(w);
                  sect=GetSection(w,l+1); // section for the next line
               }

               TerSect[sect].FirstLine=l+1;      // first line of this section
               TerSect[sect].LastPage=-1;        // init last page of the section
               if ((l+1)<TotalLines && !(PfmtId[pfmt(l+1)].flags&PAGE_HDR_FTR)) {
                  TerSect1[sect].hdr.height=TerSect1[PrevSect].hdr.height; // current page header height
                  if (!HdrFtrExists(w,&TerSect1[PrevSect].hdr)) {
                     ResetHdrFtr(w,&TerSect1[sect].hdr);
                     TerSect1[sect].hdr.height=(int)((TerSect[sect].TopMargin-TerSect[sect].HdrMargin)*PrtResY);
                  }

                  TerSect1[sect].ftr.height=TerSect1[PrevSect].ftr.height; // current page header height
                  TerSect1[sect].ftr.TextHeight=TerSect1[PrevSect].ftr.TextHeight; // current page header height
                  if (!HdrFtrExists(w,&TerSect1[PrevSect].ftr)) {
                     ResetHdrFtr(w,&TerSect1[sect].ftr);
                     TerSect1[sect].ftr.height=(int)((TerSect[sect].BotMargin-TerSect[sect].FtrMargin)*PrtResY);
                  }
                  
                  TerSect1[sect].fhdr.height=TerSect1[PrevSect].fhdr.height; // current page header height
                  if (!HdrFtrExists(w,&TerSect1[PrevSect].fhdr)) ResetHdrFtr(w,&TerSect1[sect].fhdr);
                  
                  TerSect1[sect].fftr.height=TerSect1[PrevSect].fftr.height; // current page header height
                  TerSect1[sect].fftr.TextHeight=TerSect1[PrevSect].fftr.TextHeight; // current page header height
                  if (!HdrFtrExists(w,&TerSect1[PrevSect].fftr)) ResetHdrFtr(w,&TerSect1[sect].fftr);
               }

               if (TerSect[sect].flags&SECT_RESTART_PAGE_NO)
                     RestartPageNo=TerSect[sect].FirstPageNo;
               else  RestartPageNo=0;

               CurPgHeight=SectBeginY=SectBeginY+PageColHeight; // Y position of the new section

               FirstSectLine=l+1;                // first line of the section on the new page
               FirstColLine=l+1;                 // first line of the next column

               GetSectColWidthSpace(w,TopSect,sect,&ColumnWidth,&ColumnSpace,&CurX,&YBefHdr);  // section column width and space
            }

            PrintHeight=(int)((TerSect1[TopSect].PgHeight-TerSect[TopSect].HdrMargin-TerSect[sect].FtrMargin)*PrtResY);  // height of the print area
            if (TerArg.FittedView && !InPrinting) PrintHeight*=GetPageMultiple(w);  // double up for display

            PageColHeight=0;                  // height of the text for all column for the page and current section
            CurSectCol=0;                     // current column of the section
            CellHeight[level]=0;              // current cell height
            CurCell[level]=0;                 // current cell number
            CurRowId[level]=0;                // current table row number

            if (TerSect[sect].flags&SECT_NEW_PAGE && !PageOverflow && !TerArg.FittedView) 
                 NewPage=TRUE;      // simulate page break;
            else continue;
         }

         // process hard column break
         if (LineInfo(w,l,INFO_COL) && CurRowId[level]==0) {
            if ((CurSectCol+1)<TerSect[sect].columns) {
               CurPgHeight=SectBeginY;           // next column begins at this y
               FirstColLine=l+1;                 // first line of the next column
               CurSectCol++;
               CellHeight[level]=0;              // height of the current table row
               CurCell[level]=0;                 // current cell number
               CurRowId[level]=0;                // current table row number
               CurX+=ColumnWidth+ColumnSpace;    // advance the x position
               continue;
            }
            else NewPage=TRUE;                   // this column break results in a new page
         }


         // process hard page
         if (NewPage) PageBreakBeforePara=FALSE;
         if (PageBreakBeforePara && l>0) {
            LinePage(l)=PageNo+1;
            l--;
            PageLastLine=l;
         }

         if ( (NewPage || PageBreakBeforePara || (tabw(l)->type&INFO_PAGE && !TerArg.FittedView))
            && PageNo<(MAX_PAGES-1) && PagingEnabled && CurRowId[level]==0) {

            CheckPageSpace(w,PageNo+1);         // enlarge page structre if necessary
            PageInfo[PageNo].LastLine=PageLastLine;
            PageInfo[PageNo+1].FirstLine=(l+1); // hard page break as stored as negative number
            PageInfo[PageNo+1].flags|=PAGE_HARD;
            PageInfo[PageNo+1].TopSect=PageInfo[PageNo].TopSect;
            break;
         }


         // process row break
         if (tabw(l)->type&INFO_ROW && CurRowId[level]>0) {
            int CurRowHt;
            BOOL CanBreak=TRUE;

            if      (InPartTopRow[level]) CurRowHt=TableAux[CurRowId[level]].TopRowHt;
            else if (InPartBotRow[level]) CurRowHt=TableAux[CurRowId[level]].BotRowHt;
            else {
                AdjustRowHeightForBaseAlign(w,CurRowId[level]);
                CurRowHt=TableRow[CurRowId[level]].height;
            }

            // position the Y pointer at the end of the row
            if ( (CurRowHt+CurPgHeight)>(PrintHeight-FtrHeight-FnoteHt)
               && CurPgHeight>HdrHeight
               && (!HtmlMode || level==0 || InPrinting)  // in html mode only level 0 table can be split across page
               && PagingEnabled && CanBreak && PageNo<(MAX_PAGES-1)
               && (!InPartTopRow[level]) && (!InPartBotRow[level])
               && FirstRowLine[level]>FirstColLine) {
               long FirstLine=(LineAftTableHdr>FirstColLine?LineAftTableHdr:FirstColLine);
               int  cl=cid(l);

               NewPage=TRUE;
               if (FirstRowGroupLine[level]>FirstLine) l=FirstRowGroupLine[level];   // create a page break before this row
               else                                    l=FirstRowLine[level];
               CurLineHt=LineHt(l);                    // update CurLineHt variable

               // skip the parent cells
               cl=cell[cl].ParentCell;
               for (i=level-1;i>=0 && cl>0;i--) {
                    SkipCell[i]=cl;
                    InPartBotRow[i]=TRUE;
                    TableRow[CurRowId[i]].flags|=ROWFLAG_SPLIT;   // this row is split
                    TableAux[CurRowId[i]].LastPage=PageNo;        // record the last page number
                    cl=cell[cl].ParentCell;
               }
               
               CurRowId[level]=0;
               CurCell[level]=0;
            }
            else {
               PageColHeight+=CurRowHt;
               CurPgHeight+=CurRowHt;
               if (level>0) CellHeight[level-1]+=CurRowHt;
                  
               if (InPartBotRow[level]) {
                  if (level==0) break;
                  else {
                     int NextCell=cid(l+1);
                     int NextRow=cell[NextCell].row;

                     if (NextCell>0 && cell[NextCell].level==level && IsSpannedRow(w,NextRow)) {   // move previous rows to the next page as well
                         long m;
                         for (m=FirstRowGroupLine[level];m<=l;m++) LinePage(m)=PageNo+1;
                     } 
                     SkipLevel=level;        // skip over rows of this level
                  }
               }
               CurRowId[level]=0;
               CurCell[level]=0;
               InPartTopRow[level]=InPartBotRow[level]=FALSE;
               continue;
            }
         }
      }


      CHECK_PAGE_BREAK:
      // calculate the page break
      if ( (NewPage || ((CurLineHt+CurPgHeight)>(PrintHeight-FtrHeight-FnoteHt)))
         && PagingEnabled            // must not be in the header/footer or frame
         && (!HtmlMode || level==0 || InPrinting)  // in html mode only level 0 table can be split across page 
         && (CurRowId[level]==0 || !KeepRowTogether[level]) // must not be within a table
         && CurPgHeight > HdrHeight  // a page should have atleast one line
         && l>FirstColLine           // at least one line in the current column
         && PageNo<(MAX_PAGES-1)) {  // next page begins, not allowed within a table row or hdr footer
         
         // check if the remainder of the cell is to be ignored - going on the next page
         if (CurRowId[level]>0) {
           int cl=cid(l);
           bool skip;
           for (i=level;i>=0 && cl>0;i--) {
              skip=true;
              if (i==level && LineInfo(w,l,INFO_CELL) && !LineInfo(w,l+1,INFO_ROW)) skip=false;  // this cell finished, can't yet skip the cell on the next column of this level
              if (skip) SkipCell[i]=cl;
              else      SkipCell[i]=0;
              
              InPartBotRow[i]=TRUE;
              TableRow[CurRowId[i]].flags|=ROWFLAG_SPLIT;   // this row is split

              TableAux[CurRowId[i]].LastPage=PageNo;        // record the last page number
              cl=cell[cl].ParentCell;
           }

           EndCellOnPage(w,l,PageNo,InPartTopRow[level],InPartBotRow[level],&CurCell[level],&MinCellHeight[level],
                    &PageColHeight,&CurPgHeight,CellHeight[level],&RowSpanned);

           // skip the remaining lines in this cell
           SkippingCell=cid(l);  // skip this cell lines
           FirstSkippedLine=l;
           if (LinePage(l)<=PageNo) LinePage(l)=PageNo+1;
           
           continue;
         }
         else {          // after the completion of the previous row
            long FirstLine=(LineAftTableHdr>FirstColLine?LineAftTableHdr:FirstColLine);
            long  OrigLine=l;
            int   OrigLevel=level;
         
            if (level>0 && cid(l) && IsSpannedRow(w,-cid(l)) && FirstRowGroupLine[level]>FirstLine) {
               l=FirstRowGroupLine[level];
               CurLineHt=LineHt(l);                    // update CurLineHt variable
            }
            
            // check if any of the parent level have KeepRowTogether
            if (level>0 && cid(l)) {
               for (i=level-1;i>=0 && cell[cid(l)].ParentCell;i--) {
                  if (KeepRowTogether[i]) {
                     if (FirstRowGroupLine[i]>FirstLine) l=FirstRowGroupLine[i];   // create a page break before this row
                     else                                l=FirstRowLine[i];
                     CurLineHt=LineHt(l);                    // update CurLineHt variable
                     level=i;
                  } 
               } 
            } 

            if (level>0 && CurSectCol>=(TerSect[sect].columns-1)) {
               int cl=cid(OrigLine);  // cid(l); 
               level=TableLevel(w,OrigLine/*l*/);
               SkipLevel=OrigLevel;                                  // skip remaining rows in this tale
               cl=cell[cl].ParentCell;
               for (i=level-1;i>=0 && cl>0;i--) {                   // skip the parent cells
                   SkipCell[i]=cl;
                   InPartBotRow[i]=TRUE;
                   TableRow[CurRowId[i]].flags|=ROWFLAG_SPLIT;   // this row is split

                   TableAux[CurRowId[i]].LastPage=PageNo;        // record the last page number
                   cl=cell[cl].ParentCell;
               }
               if (LinePage(l)<=PageNo) LinePage(l)=PageNo+1;
               
               // skip all lines to the end of the level 0 cell
               if (l<OrigLine) {     // Row grouping (keep together or row spanning) encounterd, so skip up to the remaing level 0 cell
                  for (;l<TotalLines;l++) {
                     if (LinePage(l)<=PageNo) LinePage(l)=PageNo+1;
                     if (TableLevel(w,l)==0 && LineInfo(w,l,INFO_CELL)) break;
                  }
                  SkipLevel=0;
               }    

               continue;
            }    
         
         }

         if (CurSectCol>=(TerSect[sect].columns-1)) { // last colunm of the section
            l=AdjustForParaKeep(w,l,FirstColLine,sect==TopSect);  // adjust for paragraph keep together/next
            CurLineHt=LineHt(l);                    // update CurLineHt variable
            AdjustLastLine=TRUE;

            CheckPageSpace(w,PageNo+1);       // enlarge page structre if necessary
            PageInfo[PageNo].LastLine=PageLastLine;
            PageInfo[PageNo+1].FirstLine=l;   // line where new page begins
            ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);
            PageInfo[PageNo+1].TopSect=PageInfo[PageNo].TopSect;

            break;
         }
         else {                               // advance the column

            l=AdjustForParaKeep(w,l,FirstColLine,sect==TopSect);  // adjust the soft column break
            CurLineHt=LineHt(l);                    // update CurLineHt variable

            CurSectCol++;
            if (l>FirstColLine) {
               LineFlags(l)|=LFLAG_SOFT_COL; // soft column break at this line
               CurX+=ColumnWidth+ColumnSpace;
            }
            FirstColLine=l;                   // first line of the next column
            CurPgHeight=SectBeginY;           // begin the next column on the top
         }
      }


      // check if new cell begins
      BeginNewCells=FALSE;
      FirstLevel=0;
      LastLevel=level;
      if (CurCell[level]==0 && cid(l)>0) {
         int cl=cid(l);
         BeginNewCells=TRUE;
         for (i=level;i>=0 && cl>0 && CurCell[i]==0;i--) {   // do until the last level where the cell is already initialized
            CurCell[i]=cl;
            cl=cell[cl].ParentCell;
         }
         FirstLevel=i+1;

         // get the base height of the first line
         if (cell[cid(l)].flags&CFLAG_VALIGN_BASE) {
            CellAux[cid(l)].BaseHeight=BaseLineHt(l);
            CellAux[cid(l)].SpaceBefore=0;   // will be calculated after the row is done
         }
      }  

      for (level=FirstLevel;level<=LastLevel && BeginNewCells;level++) {
         UINT border;
         int TopWidth,BotWidth,CellBorderHeight;
         int cl=CurCell[level];

         if (TableAux[cell[cl].row].FirstPage==PageNo) cell[cl].FirstLine=l;
         cell[cl].LastLine=l;
         ResetUintFlag(CellAux[cl].flags,CAUX_PAGINATION_DONE);  // will be turned on in EndCellOnPage function
         CellHeight[level]=0;
         SkipCell[level]=0;
         if (level==0) TableX[level]=0;
         else          TableX[level]=TableX[level-1]+TableRow[CurRowId[level-1]].CurIndent+PrevCellsWidth[level-1];

         CellAux[cl].PrevColCell=CellAux[cl].NextColCell=0;  // need to recalculate
         if (CurRowId[level]==0) RowBeginY[level]=CurPgHeight+YBefHdr-(ViewPageHdrFtr?0:HdrHeight);   // row begin Y relative to the top of the frame page

         // set the top/bottom border width
         border=cell[cl].border;
         TopWidth=GetCellFrameTopWidth(w,cl,&border,PageNo,NULL);
         BotWidth=GetCellFrameBotWidth(w,cl,&border,PageNo,NULL);
            
         if (FALSE && level<LastLevel) {                         // if the next level starts immediately, then reserve some space at the top to separate the next level
            TopWidth=cell[cl].margin;                   // use the space space as the cell margin
            CellAux[cl].flags|=CAUX_SET_TOP_SPACE;
         }
         else ResetUintFlag(CellAux[cl].flags,CAUX_SET_TOP_SPACE);

         CellBorderHeight=TwipsToPrtY(TopWidth+BotWidth);

         // adjust height variables
         CurPgHeight+=CellBorderHeight;    // height of the text for this column
         PageColHeight+=CellBorderHeight;  // height of the text for all columns
         CellHeight[level]+=CellBorderHeight;
         MinCellHeight[level]=0;

         if (CurRowId[level]==0) {   // first cell of the table
            int row=cell[cl].row;
            CurRowId[level]=row;
            if (level>0) AdjustTableRowWidth(w,row);   // ensure that subtable is fully contained in the parent cell

            if (!InPartTopRow[level]) {                            // first page for the row
                ResetUintFlag(TableRow[row].flags,ROWFLAG_SPLIT);  // reset the split flag

                TableAux[row].FirstPage=PageNo;       // this row starts on this page
                TableAux[row].BotRowHt=0;
            }
            TableAux[row].TopRowHt=0;                 // only valid for the last page
            TableAux[row].LastPage=PageNo;

            KeepRowTogether[level]=(TableRow[row].flags&(ROWFLAG_KEEP|ROWFLAG_HDR)
                          || TerSect[sect].columns>1 
                          || InHdrFtr
                          || TableRow[row].MinHeight<0
                          || IsKeepNextRow(w,l,NULL)     // 20050929
                          || (level>0 && IsSpannedRow(w,row))
                          || (level>0 && IsSpanningRow(w,row))
                          || (level>0 && KeepRowTogether[level-1]));     // if parent row needs to be kept together, then don't let child create a page break
            if (l==FirstColLine) KeepRowTogether[level]=FALSE;  // row starts on the top of the page, let it break if it needs to

            if (TableRow[row].MinHeight<0)
                 TableRow[row].height=TwipsToPrtY(-TableRow[row].MinHeight);  // set it to the minimum height
            else TableRow[row].height=TwipsToPrtY(TableRow[row].MinHeight);  // set it to the minimum height
            TableRow[row].MinPictHeight=0;

            if (!IsSpannedRow(w,row)) {
               FirstGroupRow[level]=row;
               FirstRowGroupLine[level]=l;   // first line of this row
            }
            FirstRowLine[level]=l;

            if (TableRow[row].flags&ROWFLAG_HDR) LineAftTableHdr=-1;
            else if (LineAftTableHdr==-1)        LineAftTableHdr=l;
         }

         // add to cummulative cell width
         if (cell[cl].PrevCell>0) {
            int PrevCell=cell[cl].PrevCell;
            PrevCellsWidth[level]+=TwipsToPrtX(cell[PrevCell].width);
         }
      }
      level=LastLevel;


      // save the current position
      SaveLineY=LineY(l);

      // update the line position
      LineX(l)=CurX;
      CurY=CurPgHeight+YBefHdr;
      if (!ViewPageHdrFtr && !BorderShowing) CurY-=HdrHeight;
      //if (!BorderShowing)  CurY-=YBefHdr;
      LineY(l)=CurY;                   // LineY is relative to the top of the current frame page

      // Get the frame space before the table row  and row indentation
      if (fid(l)==0 && cid(l) && l==FirstRowGroupLine[level]) {
         int i,ColWidth;   // level
         
         SpcBefRow=CalcFrmSpcBefRow(w,l,sect);
         LineY(l)=CurY+SpcBefRow;  // add the page space before the header
         for (i=level;i>=0 && l==FirstRowGroupLine[i];i--) {   // do all levels that start with this line
            TableRow[CurRowId[i]].FrmSpcBef=SpcBefRow;
            RowBeginY[i]+=SpcBefRow;                    // update the row begin y

            if (i==0) ColWidth=ColumnWidth;         // find the width allowed for the table
            else {
               int ParentCell=CurCell[i-1];
               ColWidth=TwipsToPrtX(cell[ParentCell].width-2*cell[ParentCell].margin);
            }    

            // calculate the row indentation
            SetRowIndent(w,l,CurRowId[i],sect,ColWidth);
            
            PrevCellsWidth[i]=0;                        // width of previous cells in the row
         }
      }
      else {
         SpcBefRow=0;
         if (cid(l) && fid(l)==0) {
            int FirstRow=FirstGroupRow[level];        // first group row at this level
            TableRow[CurRowId[level]].CurIndent=TableRow[FirstRow].CurIndent;  // same indentation for all rows in the group
            TableRow[CurRowId[level]].indent=TableRow[FirstRow].indent;
         }
      }

      // update line x
      if (cid(l)) LineX(l)+=(TableX[level]+TableRow[CurRowId[level]].CurIndent+PrevCellsWidth[level]);

      // Get the frame space before the line
      if (fid(l)==0) FrmSpcBef=CalcFrmSpcBef(w,l,sect,FALSE,PageNo);
      else           FrmSpcBef=0;


      if (FrmSpcBef>0 && CheckPageBreakAfterFrame 
          && ((CurLineHt+CurPgHeight+FrmSpcBef)>(PrintHeight-FtrHeight-FnoteHt))
          && PagingEnabled) {
          
          while ((l-1)>FirstLine && fid(l-1)>0) {
             int CurFID=fid(l-1);
             ResetUintFlag(ParaFrame[CurFID].flags,PARA_FRAME_POSITIONED);  // will be repositioned

             l--;  // take any immediately preceeding frame to the next page as well
          }
          NewPage=TRUE;
          CheckPageBreakAfterFrame=FALSE;   // check it only once for a line
          goto CHECK_PAGE_BREAK;
      }


      // add the line height
      
      LineY(l)+=FrmSpcBef;

      // check if frame displacement for the line changed
      if (fid(l)==0 && HasFrames && LineY(l)!=SaveLineY && l>LineDone) {
         int SaveFrameX=0,SaveFrameWidth=0,FrameX=0,FrameWidth=0;
         if (tabw(l)) {
            SaveFrameX=tabw(l)->FrameX;
            SaveFrameWidth=tabw(l)->FrameWidth;
         }
         GetFrameSpace(w,l,NULL,&FrameX,&FrameWidth,NULL);
         if (FrameX!=SaveFrameX || FrameWidth!=SaveFrameWidth) { // frame displacement changed - restart
            LastWrappedLine=FirstLine-1;     // restart word wrapping
            LineDone=l;
            goto INITIALIZE;
         }
      }

      
      if (pflags&PAGE_FTR && LimitFtrHt==0 && fid(l)==0 
          && (CurPgHeight+CurLineHt+FrmSpcBef+SpcBefRow)>HalfPgHeight) {
          LimitFtrHt=CurPgHeight;
          LimitFtrLine=l-1;
      }

      CurPgHeight+=(CurLineHt+FrmSpcBef+SpcBefRow);    // height of the text for this column
      PageColHeight+=(CurLineHt+FrmSpcBef+SpcBefRow);  // height of the text for all columns
      CellHeight[level]+=(CurLineHt+FrmSpcBef);

      

      // update the minimum cell height when frame picts are encountered
      if (cid(l) && LineFlags(l)&LFLAG_PICT) {
         int LastY=GetPictLastY(w,l)+TwipsToPrtY(CELL_MARGIN);   // provide some tolerance
         if ((LastY-RowBeginY[level])>MinCellHeight[level]) MinCellHeight[level]=LastY-RowBeginY[level];
      }


      // process the end of the cell break
      if (tabw(l) && tabw(l)->type&INFO_CELL && CurCell[level]>0 && CurRowId[level]>0) 
         EndCellOnPage(w,l,PageNo,InPartTopRow[level],InPartBotRow[level],&CurCell[level],&MinCellHeight[level],&PageColHeight,&CurPgHeight,CellHeight[level],&RowSpanned);

      UpdateBookmark(w,l,PageNo);        // update the page number for the bookmark on this line

      PageLastLine=l;
      LinePage(l)=PageNo;
   }


   PageInfo[PageNo].BodyHt=(PrintHeight-FtrHeight-FnoteHt-HdrHeight);

   // reset the picture positioned flag for the lines pushed to the next page
   if (LastPictPos==0) LastPictLine=0;
   else                AbsToRowCol(w,LastPictPos,&LastPictLine,&TempCol);
   if (l<=LastPictLine) {
      long m;
      BOOL first=TRUE;
      for (m=l;m<=LastPictLine;m++) if (LinePage(m)==PageNo && LineFlags(m)&LFLAG_PICT) {
         ResetLinePictPos(w,m);
         if (first) SetPictPageBreak(w,m,PageNo+1);  // the current page will break on this line because this picture would not fit on this page
         first=FALSE;  
      }
   } 
   
   // adjust the last line
   if (AdjustLastLine && l<=PageLastLine) {
      long m;
      for (m=l;m<=PageLastLine;m++) if (LinePage(m)==PageNo) {
         LinePage(m)=PageNo+1;
         ParaFID=fid(m);
         if (ParaFID) {
           ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_POSITIONED);
           ParaFrame[ParaFID].PageNo=PageNo+1;
         }   
      }
      PageLastLine=l-1;
   }

   // create the last page
   if (PageLastLine>=TotalLines) PageLastLine=TotalLines-1;
   PageInfo[PageNo].LastLine=PageLastLine;
   LastLineDone=PageInfo[0].LastLine;
   for (i=1;i<=PageNo;i++) {
      if (PageInfo[i].LastLine>=TotalLines) PageInfo[i].LastLine=TotalLines-1;  // this can happen in Fitted view when the window is widened and repagination does not start from the first page
      if (PageInfo[i].LastLine>LastLineDone) LastLineDone=PageInfo[i].LastLine;
   }
   for (l=PageInfo[PageNo].FirstLine+1;l<=LastLineDone;l++) if (LinePage(l)>PageNo) break;
   if (l>TotalLines) l=TotalLines;
   PageInfo[PageNo+1].FirstLine=l;
   LastPageCreated=(l==TotalLines);

   if (LastPageCreated) {
      TotalPages=PageNo+1;                    // total pages generated
      PageInfo[TotalPages].LastLine=TotalLines;
      ResetUintFlag(PageInfo[TotalPages].flags,PAGE_HARD);
   }
   else {
      PageInfo[PageNo+1].DispNbr=RestartPageNo?RestartPageNo:(PageInfo[PageNo].DispNbr+1);
      if ((PageNo+2)>TotalPages) TotalPages=PageNo+2;  // total number of pages created now
   }

   // determine first and last row of the page
   for (l=PageInfo[PageNo].FirstLine;l<=PageInfo[PageNo].LastLine;l++) if (cid(l) && LinePage(l)==PageNo) break;
   if (l<=PageInfo[PageNo].LastLine) PageInfo[PageNo].FirstRow=cell[cid(l)].row;
   for (l=PageInfo[PageNo].LastLine;l>=PageInfo[PageNo].FirstLine;l--) if (cid(l) && LinePage(l)==PageNo) break;
   if (l>=PageInfo[PageNo].FirstLine) PageInfo[PageNo].LastRow=cell[cid(l)].row;

   // set the PageNo for the table rows
   for (l=PageInfo[PageNo].FirstLine;l<=PageInfo[PageNo].LastLine;l++) {
      int CurCell=cid(l);
      if (CurCell==0 || LinePage(l)!=PageNo) continue;
      TableRow[cell[CurCell].row].PageNo=PageNo;
   }

   // check if the first line on the next page caused the page break due to picture frames
   if ((LastLineDone+1)<TotalLines && LineFlags(LastLineDone+1)&LFLAG_PICT 
      && fid(LastLineDone+1)==0 && cid(LastLineDone+1)==0) {
      SetPictPageBreak(w,LastLineDone+1,PageNo+1);
      restart=TRUE;                                      // rewrap the line on this page without this picture
   }

   // check last line for the frames
   if (LastPageCreated) TempLine=TotalLines-1;
   else                 TempLine=PageInfo[PageNo].LastLine;
   if (LineFlags(TempLine)&LFLAG_PICT && HasUnpositionedPict(w,TempLine,PageNo)) restart=HasFrames=TRUE;   // F991129
   

   if (HasFrames || TerArg.FittedView || restart || LastPageCreated) CreateFrames(w,FALSE,PageNo,PageNo);   // create the last page frame

   if (restart && PageInfo[PageNo].LastLine>LineDone) {
      LastWrappedLine=FirstLine-1;     // restart word wrapping
      LineDone=PageInfo[PageNo].LastLine;
      goto INITIALIZE;
   }

   pass++;

   if ((HasFrames || RowSpanned || DoExtraPass || DocHasToc || True(PageInfo[PageNo].flags&PAGE_REPAGE)) && pass<2) {   // do another pass
      LastWrappedLine=FirstLine-1;
      LineDone=-1;             // redo frames also
      goto INITIALIZE;
   }

   if (HasFrames && LastWrappedLine>=PageInfo[PageNo+1].FirstLine) {
      LastWrappedLine=PageInfo[PageNo+1].FirstLine-1;  // these lines might have been subject to the frames of the previous page.
   }

   return TRUE;
}

/******************************************************************************
    PageResized:
    Check if the page needs to be resized and return TRUE if the page is resized.
*******************************************************************************/
BOOL PageResized(PTERWND w)
{
    if (True(TerFlags5&TFLAG5_VARIABLE_PAGE_SIZE)) {
       int TextHeight=TerGetTextHeight(hTerWnd);
       int sect=PageInfo[0].TopSect;
       int NewPgHeight=TextHeight+InchesToTwips(TerSect[sect].TopMargin+TerSect[sect].BotMargin);
       BOOL adjust=(TotalPages>1);
       if (!adjust) {
          int CurPgHeight=InchesToTwips(TerSect1[sect].PgHeight);
          if ((NewPgHeight+PageHeightAdj+2*RULER_TOLERANCE)<CurPgHeight) adjust=true;  // page size decreasing
          if (NewPgHeight>CurPgHeight) adjust=true;  // page size inceasing
       } 
       if (adjust) {
          int SaveNewPgHeight=NewPgHeight;
          SendMessageToParent(w,TER_PAGE_SIZE_CHANGING,(WPARAM)hTerWnd,(LPARAM)(DWORD)&NewPgHeight,FALSE);

          if (NewPgHeight!=0) {
             if (NewPgHeight<SaveNewPgHeight) NewPgHeight=SaveNewPgHeight;
             PageHeightAdj=NewPgHeight-SaveNewPgHeight;  // adjustment done by the calling application
             return TerSetSectPageSize(hTerWnd,0,0,InchesToTwips(TerSect1[sect].PgWidth),NewPgHeight+RULER_TOLERANCE/4,TRUE);
          }
       }
    } 
 
    return FALSE;
} 
   

/******************************************************************************
    SkipCellLine:
    Returns TRUE if the specified cell line needs to be skipped.
*******************************************************************************/
BOOL SkipCellLine(PTERWND w, long l, int level, int PrevLevel, LPINT InPartBotRow, LPINT InPartTopRow, int PageNo, 
             LPINT SkipCell, LPINT pSkipLevel, LPINT CurCell, LPINT CurRowId, LPINT pPageColHeight,
             LPINT pCurPgHeight, LPINT TableX, LPINT PrevCellsWidth, LPINT pEndPage,LPINT CellHeight,LPINT pEndCell)
{
    int i,NewCell;
    BOOL SkipLine;

    // define arguments for easy reference
    #define SkipLevel     (*pSkipLevel)
    #define PageColHeight (*pPageColHeight)
    #define CurPgHeight   (*pCurPgHeight)
    #define EndPage       (*pEndPage)
    #define EndCell       (*pEndCell)
         
    EndPage=EndCell=FALSE;

    // check if page top  row ended
    if (InPartTopRow[PrevLevel] && (l>0 && TableLevel(w,l-1)==PrevLevel && LineInfo(w,l-1,INFO_ROW))) {
       int CurRowHt;
       if (InPartBotRow[PrevLevel]) {// this cells begins on a previous page, and overflows the current page as well
          if (PrevLevel==0) {                    // end of overflowing level 0 row
             EndPage=TRUE;
             return FALSE;
          }
          else SkipLevel=PrevLevel;              // skip the lines at this level
       }
       // end the row at this level
       CurRowHt=TableAux[CurRowId[PrevLevel]].TopRowHt;
       TableAux[CurRowId[PrevLevel]].LastPage=PageNo;        // record the last page number
       PageColHeight+=CurRowHt;
       CurPgHeight+=CurRowHt;
       if (PrevLevel>0) CellHeight[PrevLevel-1]+=CurRowHt;

       InPartTopRow[PrevLevel]=FALSE;
       CurRowId[PrevLevel]=0;
       CurCell[PrevLevel]=0;
    }
    if (SkipLevel>0 && level>=SkipLevel) {  // skip the lines at this level, they belong to the next page
       if (LinePage(l)<=PageNo) LinePage(l)=PageNo+1;
       return TRUE;
    }
    SkipLevel=0;                            // halt skipping  

    // assign the SkipCell to the next higher level
    NewCell=cid(l);
    SkipLine=FALSE;

    if (level>PrevLevel) {   // going to a higher level
       int cl=NewCell;
       if (l>0 && cid(l-1)==LevelCell(w,PrevLevel,l)) {  // if this cell is embedded by the previous cell
          for (i=level;i>PrevLevel;i--) {
              if (SkipCell[PrevLevel]>0) SkipCell[i]=cl;  // skip higher level too, do not assign CurCell yet for the higher level
              else                       SkipCell[i]=0;
              CellHeight[i]=0;
              PrevCellsWidth[i]=0;
              cl=cell[cl].ParentCell;
          }
          for (i=PrevLevel+1;i<=level;i++) TableX[i]=TableX[i-1]+TableRow[CurRowId[i-1]].CurIndent+PrevCellsWidth[i-1];
          if (SkipCell[level]) SkipLine=TRUE;
       }
    }  
    else if (level==PrevLevel && SkipCell[level]>0) { // skip remainder of the current cell at the bottom of the page
     
       if (NewCell==0 || cell[SkipCell[level]].row!=cell[NewCell].row) {  // out of the row 
          if (level==0) {                                         // getting out of the last row for the page
             EndPage=TRUE;
             return FALSE;
          }
          else SkipLevel=level;                                    // skip remaining rows for this table
       }   
       if (!SkipLevel) {
          if (NewCell==SkipCell[level]) SkipLine=TRUE;  // skip the remainder of this cell
          if (NewCell!=SkipCell[level]) {   // next cell for the row
             if (false && NewCell>0 && level>0 && SkipCell[level-1]) SkipLevel=level;  // the parent cell is being skipped, so skip child cell also
             else SkipCell[level]=0;       // do not skip this cell line 
          }
          else if (LineInfo(w,l,INFO_CELL) && !LineInfo(w,l+1,INFO_ROW)) SkipCell[level]=0;  // next column starts at this level
       }
    }
    else if (level<PrevLevel) {                                   // going down a level
       for (i=(level+1);i<=PrevLevel;i++) SkipCell[i]=0;
       if (SkipCell[level]>0) SkipLine=EndCell=TRUE;              // this cell is to be skipped so end it now as soon as touch it
    }  

    if (SkipLine || SkipLevel) {                         // skip this line
      if (LinePage(l)<=PageNo) LinePage(l)=PageNo+1;     // this line belongs to the next page
      return TRUE;
    }         

    if (InPartTopRow[0] && SkipCell[level]==0 && LinePage(l)<PageNo  && NewCell>0) return TRUE;    // skip the line on the earlier page

    // undefine the local arguments
    #undef SkipLevel
    #undef PageColHeight
    #undef CurPgHeight
    #undef EndPage
    #undef EndCell

    return FALSE;        // don't skip this line
}

    
/******************************************************************************
    EndCellOnPage:
    End the specified cell on the page.
*******************************************************************************/
EndCellOnPage(PTERWND w, long l, int PageNo, BOOL InPartTopRow, BOOL InPartBotRow, LPINT pCurCell, LPINT pMinCellHeight, LPINT pPageColHeight, LPINT pCurPgHeight, int CellHeight, LPINT pRowSpanned)
{
    int TblCellHeight;
    int CurRowId;

    // define arguments for easy reference
    #define CurCell (*pCurCell)
    #define MinCellHeight (*pMinCellHeight)
    #define PageColHeight (*pPageColHeight)
    #define CurPgHeight   (*pCurPgHeight)
    #define RowSpanned    (*pRowSpanned)

    if (CellAux[CurCell].flags&CAUX_PAGINATION_DONE) return TRUE;  // this cell already ended - came here because may be the level increased then dropped again to this level for a cell being skipped

    CurRowId=cell[CurCell].row;

    // check for the vertical text in the cell
    if (cell[CurCell].TextAngle!=0) {
       if (MinCellHeight<TwipsToPrtY(MIN_VTEXT_CELL_HEIGHT)) MinCellHeight=TwipsToPrtY(MIN_VTEXT_CELL_HEIGHT);
       if (CellHeight>MinCellHeight) CellHeight=MinCellHeight;   // do not let the height of this cell influence the row height
    }

    // store the cell height
    if (!InPartTopRow) {     // cell begin page
       CellAux[CurCell].height=CellHeight;       // store the cell height on the first page only
       CellAux[CurCell].FirstPageHt=CellHeight;  // store the cell height on the first page only
       CellAux[CurCell].LastPageHt=CellHeight;   
       CellAux[CurCell].FirstPage=PageNo;
       CellAux[CurCell].LastPage=PageNo;
    }
    else {                                       // subsequet cell page
       CellAux[CurCell].LastPageHt=CellHeight;   
       CellAux[CurCell].LastPage=PageNo;
    } 

    if (TableRow[CurRowId].MinHeight<0) MinCellHeight=0;
    TblCellHeight=(MinCellHeight>CellHeight)?MinCellHeight:CellHeight;

    // adjust the cell height for row spanning - the last row height gets adjusted for the spanning
    if (IsLastSpannedCell(w,CurCell)) TblCellHeight=GetLastSpannedCellHeight(w,CurCell,NULL,PageNo);  // height contrinution of this cell toward the row height
    else if (cell[CurCell].RowSpan>1 || cell[CurCell].flags&CFLAG_ROW_SPANNED) TblCellHeight=0; // prev spanns do not contribute to the height of the row
    if (cell[CurCell].RowSpan>1) RowSpanned=TRUE;

    if (PrtToTwipsY(MinCellHeight)>TableRow[CurRowId].MinPictHeight) TableRow[CurRowId].MinPictHeight=PrtToTwipsY(MinCellHeight);


    if (InPartTopRow && TableAux[CurRowId].TopRowHt<TblCellHeight) TableAux[CurRowId].TopRowHt=TblCellHeight;
    if (InPartBotRow && TableAux[CurRowId].BotRowHt<TblCellHeight) TableAux[CurRowId].BotRowHt=TblCellHeight;

    if (!InPartTopRow && !InPartBotRow) {
       if (TableRow[CurRowId].height<TblCellHeight
         && (TableRow[CurRowId].MinHeight>=0 || TableRow[CurRowId].MinPictHeight>0)) TableRow[CurRowId].height=TblCellHeight;
       cell[CurCell].LastLine=l;
    }

    CellAux[CurCell].flags|=CAUX_PAGINATION_DONE;    // to avoid from ending this cell again if the level rises and drops for a cell being skipped

    // position the Y pointer back for the next cell
    PageColHeight-=CellHeight;
    CurPgHeight-=CellHeight;
    CurCell=0;

    // undefine the local arguments
    #undef CurCell
    #undef MinCellHeight
    #undef PageColHeight
    #undef CurPgHeight
    #undef RowSpanned

    return TRUE;
}

/******************************************************************************
    UpdateBookmark:
    Update page number for the bookmarks.
*******************************************************************************/
UpdateBookmark(PTERWND w, long line, int PageNo)
{
    LPWORD ct;
    int i,tag,len;
      
    if (pCtid(line)==null) return true;

    ct=pCtid(line);
    len=LineLen(line);
    
    for (i=0;i<len;i++) {
       tag=ct[i];
       while (tag!=0) {
          CharTag[tag].line=line;
          tag=CharTag[tag].next;
       }
    }

    return true;
}
 
/******************************************************************************
    CalcTblHdrHt:
    Calculate the tbl hdr height for a page.
*******************************************************************************/
CalcTblHdrHt(PTERWND w, int PageNo)
{
   long l,FirstLine,LastLine;
   int PrevRow=0,cl,row;

   PageInfo[PageNo].TblHdrHt=0;
   PageInfo[PageNo].TblHdrFirstLine=PageInfo[PageNo].TblHdrLastLine=-1;
   FirstLine=LastLine=-1;

   l=PageInfo[PageNo].FirstLine-1;

   if (PageNo==0 || l<0 || HtmlMode || TerArg.FittedView) return TRUE;
   if (cid(PageInfo[PageNo].FirstLine)==0) return TRUE;   // page does not start with a table

   cl=cid(l);
   row=cell[cl].row;
   if (cl==0/* || TableRow[row].flags&ROWFLAG_HDR*/) return TRUE;

   while (l>=0) {
      cl=cid(l);
      if (cl==0) break;        // out of table
      if (cell[cl].level==0) {
         row=cell[cl].row;
         if (TableRow[row].flags&ROWFLAG_HDR) {
            if (LastLine==-1) LastLine=l;
            FirstLine=l;
            if (row!=PrevRow) {   // add height
               PageInfo[PageNo].TblHdrHt+=TableRow[row].height;
               PrevRow=row;
            }
         }
         else if (FirstLine!=-1) break;    // out of the header rows
      }
      l--;
   }

   PageInfo[PageNo].TblHdrFirstLine=FirstLine;
   PageInfo[PageNo].TblHdrLastLine=LastLine;

   return TRUE;
}

/******************************************************************************
    HdrFtrExists:
    Check if hdr/ftr text exists for a section
*******************************************************************************/
HdrFtrExists(PTERWND w, struct StrHdrFtr far *hdr)
{
   if ((hdr->LastLine-hdr->FirstLine)>2) return TRUE;
   if (hdr->FirstLine>=0 && (hdr->FirstLine+1)<TotalLines && LineLen(hdr->FirstLine+1)>1) return TRUE;
   return FALSE;
}

/******************************************************************************
    ResetHdrFtr:
    Reset hdr/ftr.
*******************************************************************************/
ResetHdrFtr(PTERWND w, struct StrHdrFtr far *hdr)
{
   hdr->FirstLine=-1;
   hdr->height=0;
   hdr->TextHeight=0;

   return TRUE;
}
 
/******************************************************************************
    PageHdrSect:
    Return the section that supplies the header for the specified page.
*******************************************************************************/
PageHdrSect(PTERWND w, int PageNo, struct StrHdrFtr far *hdr)
{
   BOOL FirstSectPage=(PageInfo[PageNo].flags&PAGE_FIRST_SECT_PAGE);
   int sect,TopSect=PageInfo[PageNo].TopSect;

   if (FirstSectPage && TerSect1[TopSect].fhdr.FirstLine>=0 
       /*&& (TerSect[TopSect].flags&SECT_NEW_PAGE || TerSect1[TopSect].PrevSect<0)*/)  {
      if (hdr) FarMove(&(TerSect1[TopSect].fhdr),hdr,sizeof(struct StrHdrFtr));
      return TopSect;
   }
   
   if (FirstSectPage && True(TerSect[TopSect].flags&SECT_HAS_TITLE_PAGE)) {       // check if fhdr can be inherited
      sect=TopSect;
      while (sect>=0) {
         if (sect>=0 && TerSect1[sect].fhdr.FirstLine>=0) {
            if (hdr) FarMove(&(TerSect1[sect].fhdr),hdr,sizeof(struct StrHdrFtr));
            return sect;
         }
         else if (True(TerSect[sect].flags&SECT_HAS_TITLE_PAGE)) {       // this page has title page set, but does not have fhdr so check the previous sect now
            sect=TerSect1[sect].PrevSect;
         }
         else break;
      }
      return -1;  // section needs to print a title page, but it is missing
   }

   sect=TopSect;
   while (sect>=0) {
      if (sect>=0 && TerSect1[sect].hdr.FirstLine>=0) {
         if (EditPageHdrFtr) break;   // in edit mode every section has header/footer
         //if (ViewPageHdrFtr && TerSect1[sect].PrevSect<0) break;  // first section hdr/ftr always exists when hdr/ftr active
         if (HdrFtrExists(w,&TerSect1[sect].hdr)) break;  // sectious hdr is empty
      }
      sect=TerSect1[sect].PrevSect;
   }
   
   if (sect>=0 && TerSect1[sect].hdr.FirstLine<0) sect=-1;
   if (sect>=0 && hdr) FarMove(&(TerSect1[sect].hdr),hdr,sizeof(struct StrHdrFtr));

   return sect;
}

/******************************************************************************
    PageFtrSect:
    Return the section that supplies the footer for the specified page.
*******************************************************************************/
PageFtrSect(PTERWND w, int PageNo, struct StrHdrFtr far *ftr)
{
   BOOL FirstSectPage=(PageInfo[PageNo].flags&PAGE_FIRST_SECT_PAGE);
   int sect,TopSect=PageInfo[PageNo].TopSect;

   if (FirstSectPage && TerSect1[TopSect].fftr.FirstLine>=0 
        /*&& (TerSect[TopSect].flags&SECT_NEW_PAGE || TerSect1[TopSect].PrevSect<0)*/)  {
      if (ftr) FarMove(&(TerSect1[TopSect].fftr),ftr,sizeof(struct StrHdrFtr));
      return TopSect;
   }

   if (FirstSectPage && True(TerSect[TopSect].flags&SECT_HAS_TITLE_PAGE)) {       // check if fftr can be inherited
      sect=TopSect;
      while (sect>=0) {
         if (sect>=0 && TerSect1[sect].fftr.FirstLine>=0) {
            if (ftr) FarMove(&(TerSect1[sect].fftr),ftr,sizeof(struct StrHdrFtr));
            return sect;
         }
         else if (True(TerSect[sect].flags&SECT_HAS_TITLE_PAGE)) {       // this page has title page set, but does not have fftr so check the previous sect now
            sect=TerSect1[sect].PrevSect;
         }
         else break;
      }
      return -1;  // section needs to print a title page, but it is missing
   }


   sect=TopSect;
   while (sect>=0) {
      if (TerSect1[sect].ftr.FirstLine>=0) {
        if (EditPageHdrFtr) break;   // in edit mode every section has header/footer
        //if (ViewPageHdrFtr && TerSect1[sect].PrevSect<0) break;  // first section hdr/ftr always exists when hdr/ftr active
        if (HdrFtrExists(w,&TerSect1[sect].ftr)) break;   // sectious footer is empty
      }
      sect=TerSect1[sect].PrevSect;
   }

   if (sect>=0 && TerSect1[sect].ftr.FirstLine<0) sect=-1;
   if (sect>=0 && ftr) FarMove(&(TerSect1[sect].ftr),ftr,sizeof(struct StrHdrFtr));

   return sect;
}

/******************************************************************************
    PageHdrHeight:
    This function returns the height of the page header in printer units.
*******************************************************************************/
PageHdrHeight(PTERWND w, int PageNo, BOOL IncludeOverflow)
{
    return PageHdrHeight2(w,PageNo,IncludeOverflow,false);
}

PageHdrHeight2(PTERWND w, int PageNo, BOOL IncludeOverflow, BOOL inherit)
{
   BOOL FirstSectPage=PageNo==0 || (PageInfo[PageNo].flags&PAGE_FIRST_SECT_PAGE);
   BOOL exists;
   int TopSect=PageInfo[PageNo].TopSect,height,sect;

   sect=-1;
   if (FirstSectPage && TerSect1[TopSect].fhdr.FirstLine>=0 /*&& (PageNo==0 || TerSect[TopSect].flags&SECT_NEW_PAGE)*/) {
      sect=TopSect;
      height=TerSect1[TopSect].fhdr.height;
   }
   else if (FirstSectPage && True(TerSect[TopSect].flags&SECT_HAS_TITLE_PAGE)) {       // check if fhdr can be inherited
      sect=PageHdrSect(w,PageNo,NULL);
      if (sect>=0) height=TerSect1[sect].fhdr.height;
   } 
   
   if (sect<0)  {   // use regular hdr footer
      sect=inherit?PageHdrSect(w,PageNo,NULL):GetSection(w,PageInfo[PageNo].FirstLine);
      if (sect<0) sect=TopSect;
      height=TerSect1[sect].hdr.height;
   }

   exists=(height>0)?TRUE:FALSE;  // header exists


   if (!IncludeOverflow && (TerSect[sect].flags&SECT_EXACT_MARGT)) 
        height=(int)((TerSect[sect].TopMargin-TerSect[sect].HdrMargin)*PrtResY);
   
   if (exists && height==0) height=1;  // to indicate the header exists in the document
   
   if (height<0) height=0;

   return height;
}

/******************************************************************************
    PageFtrHeight:
    This function returns the height of the page header in printer units.
*******************************************************************************/
PageFtrHeight(PTERWND w, int PageNo, BOOL IncludeOverflow)
{
   BOOL FirstSectPage=PageNo==0 || (PageInfo[PageNo].flags&PAGE_FIRST_SECT_PAGE);
   BOOL exists;
   int  TopSect=PageInfo[PageNo].TopSect,height,sect;

   sect=-1;
   if (FirstSectPage && TerSect1[TopSect].fftr.FirstLine>=0 /*&& (PageNo==0 || TerSect[TopSect].flags&SECT_NEW_PAGE)*/) {
       sect=TopSect;
       height=TerSect1[TopSect].fftr.height;
   }
   else if (FirstSectPage && True(TerSect[TopSect].flags&SECT_HAS_TITLE_PAGE)) {       // check if fhdr can be inherited
      sect=PageFtrSect(w,PageNo,NULL);
      if (sect>=0) height=TerSect1[sect].fftr.height;
   } 

   if (sect<0)  {
      sect=PageFtrSect(w,PageNo,NULL);
      if (sect<0) sect=0;
      height=TerSect1[sect].ftr.height;
   }

   exists=(height>0)?TRUE:FALSE;  // header exists

   if (!IncludeOverflow && (TerSect[sect].flags&SECT_EXACT_MARGB)) 
        height=(int)((TerSect[sect].BotMargin-TerSect[sect].FtrMargin)*PrtResY);

   if (exists && height==0) height=1;

   if (height<0) height=0;

   return height;
}

/******************************************************************************
    PageFtrTextHeight:
    This function returns the height of the page header in printer units.
*******************************************************************************/
PageFtrTextHeight(PTERWND w, int PageNo)
{
   BOOL FirstSectPage=PageNo==0 || (PageInfo[PageNo].flags&PAGE_FIRST_SECT_PAGE);
   int  TopSect=PageInfo[PageNo].TopSect;

   if (FirstSectPage && TerSect1[TopSect].fftr.FirstLine>=0 && (PageNo==0 || TerSect[TopSect].flags&SECT_NEW_PAGE))
         return TerSect1[TopSect].fftr.TextHeight;
   else  return TerSect1[TopSect].ftr.TextHeight;
}

/******************************************************************************
    WrapMoreLines:
    wrap more lines during paginations
*******************************************************************************/
WrapMoreLines(PTERWND w, int sect)
{
    long l,LinesWrapped,PrevLastWrappedLine;
    long WrapLines=30;                        // wrap 30 lines at a time
    long StartLine=LastWrappedLine+1;
    long WrapBlockSize;
    BOOL DelEmptyLines;

    if (TerOpFlags2&TOFLAG2_NO_WRAP) {          // wrapping disabled
       LastWrappedLine++;
       return TRUE;
    }    

    // set section values for optimization
    KnownSect=sect;
    KnownSectBegLine=LastWrappedLine;
    KnownSectEndLine=LastWrappedLine+30;

    // do word wrapping
    LinesWrapped=0;
    while (LinesWrapped<=WrapLines) {              // wrap requested number of lines
        if ((StartLine+LinesWrapped)>=TotalLines) break;

        PrevLastWrappedLine=LastWrappedLine;

        WrapBlockSize=WrapLines-LinesWrapped+1;    // number of ilnes  to wrap
        if (WrapBlockSize<20) WrapBlockSize=20;

        WrapMakeBuffer(w,StartLine+LinesWrapped,WrapBlockSize); // build buffer for pasing
        WrapParseBuffer(w,StartLine+LinesWrapped);   // parse the buffer

        DelEmptyLines=FALSE;
        if (LastBufferedLine>(LastWrappedLine+25)) DelEmptyLines=TRUE;
        if (BufferLength==0) DelEmptyLines=TRUE;
        if ((LastBufferedLine+1)>=TotalLines) DelEmptyLines=TRUE;
        if (LastWrappedLine==(PrevLastWrappedLine+1)) DelEmptyLines=TRUE;

        if (LastBufferedLine>LastWrappedLine) {
           if (DelEmptyLines) {
              DisplacePointers(w,LastBufferedLine+1,LastWrappedLine-LastBufferedLine); // scroll up the remaining pointers
              LastBufferedLine=LastWrappedLine;
           }
           else {
              for (l=LastWrappedLine+1;l<=LastBufferedLine;l++) {
                 FreeLine(w,l);
                 InitLine(w,l);
                 pfmt(l)=0;
              }
           }
        }

        if (LastWrappedLine<0) LastWrappedLine=0;

        // go back a line when an incomplete paragraph is encoutered
        if (!(LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK|LFLAG_LINE))) {
            if (LastWrappedLine>StartLine
                && (LastWrappedLine+1)<=TotalLines && LastWrappedLine<(StartLine+WrapLines-1)
                && LastWrappedLine>(PrevLastWrappedLine+1)) LastWrappedLine--;
            else DoExtraPass=TRUE;
        }
        if (LineFlags(LastWrappedLine)&LFLAG_SECT) KnownSect=-1;  // section changed

        LinesWrapped=LastWrappedLine-StartLine+1;
    }

    DisplacePointers(w,LastBufferedLine+1,LastWrappedLine-LastBufferedLine); // scroll up the remaining pointers

    // go back a line when an incomplete paragraph is encoutered
    if (!(LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK|LFLAG_LINE))
         && LastWrappedLine>StartLine
         && (LastWrappedLine+1)<=TotalLines
         && LastWrappedLine>(StartLine+1)) LastWrappedLine--;

    KnownSect=-1;   // reset known section

    return TRUE;
}

/******************************************************************************
    HasUnpositionedPict:
    Check if the line contains unpositioned picture frames. This function
    also sets the InUse flag and page number for the picture frames in the line.
*******************************************************************************/
BOOL HasUnpositionedPict(PTERWND w,long LineNo, int PageNo)
{
   LPWORD fmt;
   WORD  pict;
   BOOL result=FALSE;
   int i,ParaFID;

   if (!(LineFlags(LineNo)&LFLAG_PICT)) return FALSE;

   fmt=OpenCfmt(w,LineNo);

   for (i=0;i<LineLen(LineNo);i++) {
      pict=fmt[i];
      if (TerFont[pict].InUse && TerFont[pict].style&PICT && TerFont[pict].FrameType!=PFRAME_NONE && TerFont[pict].ParaFID>0) {
         ParaFID=TerFont[pict].ParaFID;
         ParaFrame[ParaFID].InUse=TRUE;  // this flag was reset in the beginning of the CreateOnePage function
         
         if (ParaFrame[ParaFID].PageNo>PageNo) {
            ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_POSITIONED);  // consider this page unpositioned for this page
         }
         if (!(ParaFrame[ParaFID].flags&PARA_FRAME_POSITIONED)) result=TRUE;
         
         ParaFrame[ParaFID].PageNo=PageNo;
      }
   }
   CloseCfmt(w,LineNo);

   return result;
}

/******************************************************************************
    IsPictPageBreak:
    Returns TRUE if this line contains a picture frame causing page break.
*******************************************************************************/
BOOL IsPictPageBreak(PTERWND w,long LineNo, int PageNo)
{
   LPWORD fmt;
   WORD  pict;
   BOOL result=FALSE;
   int i,ParaFID;

   if (!(LineFlags(LineNo)&LFLAG_PICT)) return FALSE;

   fmt=OpenCfmt(w,LineNo);

   for (i=0;i<LineLen(LineNo);i++) {
      pict=fmt[i];
      if (TerFont[pict].InUse && TerFont[pict].style&PICT && TerFont[pict].FrameType!=PFRAME_NONE && TerFont[pict].ParaFID>0) {
         ParaFID=TerFont[pict].ParaFID;
         if (ParaFrame[ParaFID].PageNo==(PageNo+1) && ParaFrame[ParaFID].flags&PARA_FRAME_PAGE_BREAK) return TRUE;
      }
   }
   CloseCfmt(w,LineNo);

   return FALSE;
}

/******************************************************************************
    SetPictPageBreak:
    Set the page berak for the picture frame on the given line.
*******************************************************************************/
BOOL SetPictPageBreak(PTERWND w,long LineNo, int PageNo)
{
   LPWORD fmt;
   WORD  pict;
   BOOL result=FALSE;
   int i,ParaFID;

   if (!(LineFlags(LineNo)&LFLAG_PICT)) return FALSE;

   fmt=OpenCfmt(w,LineNo);

   for (i=0;i<LineLen(LineNo);i++) {
      pict=fmt[i];
      if (TerFont[pict].InUse && TerFont[pict].style&PICT && TerFont[pict].FrameType!=PFRAME_NONE && TerFont[pict].ParaFID>0) {
         ParaFID=TerFont[pict].ParaFID;
         ParaFrame[ParaFID].PageNo=PageNo;
         ParaFrame[ParaFID].flags|=PARA_FRAME_PAGE_BREAK;
         ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_POSITIONED);  // will need to position this picture on the new page

      }
   }
   CloseCfmt(w,LineNo);

   return FALSE;
}

/******************************************************************************
    ResetLinePictPos:
    Reset the PARA_FRAME_POSITIONED flag for all the framed picture for the given line.
*******************************************************************************/
BOOL ResetLinePictPos(PTERWND w,long LineNo)
{
   LPWORD fmt;
   WORD  pict;
   BOOL result=FALSE;
   int i,ParaFID;

   if (!(LineFlags(LineNo)&LFLAG_PICT)) return FALSE;

   fmt=OpenCfmt(w,LineNo);

   for (i=0;i<LineLen(LineNo);i++) {
      pict=fmt[i];
      if (TerFont[pict].InUse && TerFont[pict].style&PICT && TerFont[pict].FrameType!=PFRAME_NONE && TerFont[pict].ParaFID>0) {
         ParaFID=TerFont[pict].ParaFID;
         ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_POSITIONED);

      }
   }
   CloseCfmt(w,LineNo);

   return result;
}

/******************************************************************************
    GetPictLastY:
    Get the last Y of the picture frames.
*******************************************************************************/
int GetPictLastY(PTERWND w,long LineNo)
{
   LPWORD fmt;
   WORD  pict;
   BOOL result=FALSE;
   int i,ParaFID,LastY=0,EndY;

   if (!(LineFlags(LineNo)&LFLAG_PICT)) return 0;

   fmt=OpenCfmt(w,LineNo);

   for (i=0;i<LineLen(LineNo);i++) {
      pict=fmt[i];
      if (TerFont[pict].InUse && TerFont[pict].style&PICT && TerFont[pict].FrameType!=PFRAME_NONE && TerFont[pict].ParaFID>0) {
         ParaFID=TerFont[pict].ParaFID;
         if (ParaFrame[ParaFID].ShapeType==SHPTYPE_PICT_FRAME) {
           if (ParaFrame[ParaFID].flags&PARA_FRAME_POSITIONED) {
              EndY=ParaFrame[ParaFID].y+ParaFrame[ParaFID].height;
              if (EndY>LastY) LastY=EndY;
           }
         }
      }
   }
   CloseCfmt(w,LineNo);

   return TwipsToPrtY(LastY);
}

/******************************************************************************
    GetSectColWidthSpace:
    Get the column width and space for the section.
*******************************************************************************/
GetSectColWidthSpace(PTERWND w,int TopSect,int CurSect,LPINT pColWidth, LPINT pColSpace,LPINT pTextX, LPINT pYBefHdr)
{
    int LeftMargin,RightMargin,TextWidth,MaxColumns,ColumnSpace,ColumnWidth;
    int TextX=0,YBefHdr=0;

    LeftMargin=(int)(PrtResX*TerSect[TopSect].LeftMargin);
    RightMargin=(int)(PrtResX*TerSect[TopSect].RightMargin);
    TextWidth=(int)(PrtResX*TerSect1[TopSect].PgWidth)-LeftMargin-RightMargin;
    if (TerArg.FittedView) TextWidth=ScrToPrtX(TerWinWidth);
    MaxColumns=TerSect[CurSect].columns;
    ColumnSpace=(int)(PrtResX*TerSect[CurSect].ColumnSpace);
    if (MaxColumns==1) ColumnSpace=0;
    ColumnWidth=(TextWidth-(MaxColumns-1)*ColumnSpace)/MaxColumns;

    if (BorderShowing) TextX=LeftBorderWidth+LeftMargin;

    if (BorderShowing) {  // calculate the distance before the header begins
       YBefHdr=(int)(PrtResY*TerSect[TopSect].HdrMargin)+TopBorderHeight;
    }
    else if (ViewPageHdrFtr) {
       YBefHdr=(int)(PrtResY*TerSect[TopSect].HdrMargin)-TerSect1[TopSect].HiddenY;
    }

    if (pColWidth) (*pColWidth)=ColumnWidth;
    if (pColSpace) (*pColSpace)=ColumnSpace;
    if (pTextX)    (*pTextX)=TextX;
    if (pYBefHdr)  (*pYBefHdr)=YBefHdr;

    return TRUE;
}

/******************************************************************************
    GetSpannedRowHeight:
    Accumulate the heights of all rows spanned by a cell.  If the page number
    is specified, then only the spanned row in that page are considered.
*******************************************************************************/
int GetSpannedRowHeight(PTERWND w,int CurCell,LPINT pScrHeight, int PageNo)
{
    int CurRowId,SpanCount,height=0,ScrHeight=0,FirstRow=0,LastRow=0;

    if (pScrHeight) (*pScrHeight)=0;

    CurRowId=cell[CurCell].row;

    if (PageNo>=0) {
       FirstRow=LevelRow(w,cell[CurCell].level,PageInfo[PageNo].FirstRow); // first row of the page at the current level
       LastRow=LevelRow(w,cell[CurCell].level,PageInfo[PageNo].LastRow); // last row of the page at the current level
    }

    if (cell[CurCell].flags&CFLAG_ROW_SPANNED) {
       if (PageNo<0) return 0;
       if (CurRowId!=FirstRow) return 0;   // height not applicable to subsequent spanned rows
       SpanCount=GetRemainingCellSpans(w,CurCell);
    }
    else SpanCount=cell[CurCell].RowSpan;

    if (PageNo<0 || LastRow!=CurRowId) {
       CurRowId=TableRow[CurRowId].NextRow;  // get the next row
       while (CurRowId>0 && SpanCount>1) {
          height+=TableRow[CurRowId].height;
          ScrHeight+=PrtToScrY(TableRow[CurRowId].height);
          if (PageNo>=0 && LastRow==CurRowId) break;
          CurRowId=TableRow[CurRowId].NextRow;
          SpanCount--;
       }
    }

    if (pScrHeight) (*pScrHeight)=ScrHeight;

    return height;
}

/******************************************************************************
    GetLastSpannedCellHeight:
    Get the height which is the last spanned cell contributes toward the row height.
    This height would be the height of the spanning cell minus the height of all
    previous spanning rows.  If PageNo is specified then if the first spanning row is
    on the previous page, then the function will returns 0.
*******************************************************************************/
int GetLastSpannedCellHeight(PTERWND w,int CurCell,LPINT pScrHeight, int PageNo)
{
    int CurRowId,SpanRowId,SpanCount,height=0,ScrHeight=0,SpanningCell,FirstRow=0;

    if (pScrHeight) (*pScrHeight)=0;

    if (!(cell[CurCell].flags&CFLAG_ROW_SPANNED)) return 0;  // not a spanned cell

    SpanningCell=CellAux[CurCell].SpanningCell;
    
    if (PageNo>=0) {
       int FirstPage=CellAux[SpanningCell].FirstPage;
       int LastPage=CellAux[SpanningCell].LastPage;
       if      (PageNo==FirstPage) height=CellAux[SpanningCell].FirstPageHt;
       else if (PageNo==LastPage) height=CellAux[SpanningCell].LastPageHt;
       else if (FirstPage!=LastPage) height=PageInfo[PageNo].BodyHt;          // text body height
       else height=CellAux[SpanningCell].height;              // height of the spanning cell
    } 
    else height=CellAux[SpanningCell].height;                 // height of the spanning cell

    CurRowId=cell[CurCell].row;
    SpanRowId=cell[SpanningCell].row;
    SpanCount=cell[SpanningCell].RowSpan;
    if (PageNo>=0) FirstRow=LevelRow(w,cell[CurCell].level,PageInfo[PageNo].FirstRow); // first row of the page at the current level

    while (SpanCount>1) {
        CurRowId=TableRow[CurRowId].PrevRow;
        if (CurRowId<=0) break;
        height-=TableRow[CurRowId].height;   // subtract height used up by the previous rows

        SpanCount--;
        if (CurRowId==SpanRowId || SpanCount==1) break;
        if (PageNo>=0 && CurRowId==FirstRow && CurRowId!=SpanRowId) return 0;  // no height contribution made by this last spanned cell
    }    

    return height;
}

/******************************************************************************
    AdjustForParaKeep:
    This function returns the first line of the next page taking into account
    the paragraph keep and keen next properties.
*******************************************************************************/
long AdjustForParaKeep(PTERWND w,long BreakLine, long FirstColLine, BOOL PageTop)
{
    long line=BreakLine,SaveLine,FirstRowLine;
    int  CurPara,PrevPara;
    UINT CurFlags,PrevFlags;
    BOOL ParaKeep;
    BOOL InLastPartialPara=TRUE;

    CurPara=pfmt(line);

    if (FirstColLine<0) FirstColLine=-FirstColLine;  // to handle hard page breaks

    // adjust FirstColLine if it belongs to header/footer
    while (PfmtId[pfmt(FirstColLine)].flags&PAGE_HDR_FTR) {
       if ((FirstColLine+1)>=line) break;
       FirstColLine++;
    } 

    // exclude any para relative frames
    while (line>FirstColLine) {
       int ParaFID=fid(line-1);    // check if the previous line is a frame
       if (ParaFID==0) break;
       if (ParaFrame[ParaFID].flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)) break;
       line--;
    }



    // handle keep and keen next
    while (TRUE) {
       SaveLine=line;

       // check if a table row line
       if (line>0 && cid(line-1)>0 && IsKeepNextRow(w,line-1,&FirstRowLine)) {  // this line belongs to the row that needs to be kept with the next row (or line after current row)
          line=FirstRowLine;
          CurPara=pfmt(line);
          CurFlags=PfmtId[CurPara].flags;
          goto KEEP_NEXT_END;
       } 
       
       // get current and previous para flags
       CurPara=pfmt(line);
       CurFlags=PfmtId[CurPara].flags;
       if (line>0) {
          PrevPara=pfmt(line-1);
          PrevFlags=PfmtId[PrevPara].flags;
       }
       if ((LineFlags(line)&LFLAG_PARA_FIRST)) InLastPartialPara=FALSE;  // out of the last partial paragraph

       // go back a line back when current line is a new para and the prev para has 'keep next'
       while (line>FirstColLine && PrevFlags&PARA_KEEP_NEXT && LineFlags(line)&LFLAG_PARA_FIRST) {
          if (tabw(line) && tabw(line)->type&(INFO_SECT|INFO_PAGE|INFO_COL)) return line;
          if (LineFlags(line)&LFLAG_HDRS_FTRS) return line;


          // if on a table, find a row without this attribute
          if (cid(line-1)>0) {
             int InitCell=cid(line-1);
             line=line-2;       // skip over immediately preceding row marker
             while (line>(FirstColLine+1)) {
                if (tabw(line) && tabw(line)->type&INFO_ROW) {
                   int CurCell=cid(line);
                   if (cell[InitCell].level==cell[CurCell].level) break;
                }
                if (cid(line)==0) break;
                line--;
             }
             if (line<=(FirstColLine+1)) {
                if (PageTop) return BreakLine;
                else         return FirstColLine;
             }

             PrevPara=pfmt(line);
             PrevFlags=PfmtId[PrevPara].flags;
             line++;   // position at the first line of the following row
             CurPara=pfmt(line);
             CurFlags=PfmtId[CurPara].flags;

             continue;
          }

          line--;
          CurPara=PrevPara;
          CurFlags=PrevFlags;
          if (line>0) {
             PrevPara=pfmt(line-1);
             PrevFlags=PfmtId[PrevPara].flags;
          }
       }

       KEEP_NEXT_END:
       if (line<=FirstColLine) {
          if (PageTop) return BreakLine;
          else         return FirstColLine;
       }

       

       // treat the entire para frame as keep together.
       while (line>FirstColLine && fid(line)>0 && fid(line)==fid(line-1)) {
          line--;
          CurPara=pfmt(line);
          CurFlags=PfmtId[CurPara].flags;
       }

       if (line<=FirstColLine) {
          if (PageTop) return BreakLine;
          else         return FirstColLine;
       }

       // go to the beginning of the paragraph on para keep
       //ParaKeep=((CurFlags&PARA_KEEP && InLastPartialPara) || (CurFlags&PARA_KEEP_NEXT));  // keep-next in the para before the last para is treated as keep-together
       ParaKeep=True(CurFlags&PARA_KEEP);
       while (cid(line)==0 && ParaKeep && line>FirstColLine && !(LineFlags(line)&LFLAG_PARA_FIRST)) {
          line--;
          CurPara=pfmt(line);
          CurFlags=PfmtId[CurPara].flags;
          //ParaKeep=((CurFlags&PARA_KEEP && InLastPartialPara) || (CurFlags&PARA_KEEP_NEXT));  // keep-next in the para before the last para is treated as keep-together
          ParaKeep=True(CurFlags&PARA_KEEP);
       }

       // adjust for window/orphan
       if (cid(line)==0 && PfmtId[CurPara].pflags&PFLAG_WIDOW && line>FirstColLine && !(LineFlags(line)&LFLAG_PARA_FIRST)) {
          long TempLine=line;
          if (LineFlags(line)&LFLAG_PARA) line--;  // fix for orphan line
          else if (LineFlags(line-1)&LFLAG_PARA_FIRST) line--;
          if (line!=TempLine) {
            CurPara=pfmt(line);
            CurFlags=PfmtId[CurPara].flags;
          }
       }

       if (line<=FirstColLine) {
          if (PageTop) return BreakLine;
          else         return FirstColLine;
       }

       if (InLastPartialPara) BreakLine=line;   // moves last para to the next page if the entire page is keep-next and last para is keep-together

       if (line==SaveLine) break;   // no more changes ocurring
    }

    return line;
}

/******************************************************************************
    IsKeepNextRow:
    Return true if the given line belongs to a 'keep-with-next' row. Also, returns
    the first line of the row.
*******************************************************************************/
BOOL IsKeepNextRow(PTERWND w,long line,LPLONG pFirstLine)
{
    long l;
    int cl,row,level;

    if (pFirstLine) (*pFirstLine)=line;

    if (line<0 || line>=TotalLines) return false;
    cl=cid(line);
    if (cl==0) return false;
    level=cell[cl].level;
    row=cell[cl].row;

    for (l=line-1;l>=0;l--) {
       cl=cid(l);
       if (cl==0) break;  // out of the table

       if (cell[cl].level>level) continue;  // skip the embedded cells
       if (row!=cell[cl].row) break;  // out of table row
    } 
    l++;  // first table row lne
    
    if (pFirstLine) (*pFirstLine)=l;

    return True(PfmtId[pfmt(l)].flags&PARA_KEEP_NEXT);
} 

/******************************************************************************
    AdjustSectColHeight:
    This routine adjust the column height of the last page of a section. It also
    returns the modified last line for the section.
*******************************************************************************/
AdjustSectColHeight(PTERWND w,int MaxColumns, int ColHeight,long FirstLine, LPLONG pLastLine, int MaxColHeight, BOOL PageTop)
{
    int columns=0;
    int MaxHeight=0,CurHeight=0,CellId,LineHeight;
    long line,LastLine=(*pLastLine),FirstColLine;
    BOOL AfterHardColBreak,NewColumn=FALSE;

    // check if the columns contained any frame displaced line, if so then we can't balance the section because it would affect the wrapping of lines
    for (line=FirstLine;line<=LastLine;line++) {
       if (LineInfo(w,line,INFO_FRAME)) return MaxColHeight;
    } 

    // remove any soft column break from this section on this page
    for (line=FirstLine;line<=LastLine;line++) if (LineFlags(line)&LFLAG_SOFT_COL) LineFlags(line)=ResetLongFlag(LineFlags(line),LFLAG_SOFT_COL);   // reset the soft column break

    FirstColLine=FirstLine;   // first line of the first column

    for (line=FirstLine;line<=LastLine;line++) {
       if (PfmtId[pfmt(line)].flags&(PAGE_HDR|PAGE_FTR)) continue;

       // Get the current line height
       LineHeight=0;
       if (fid(line)==0) {
          CellId=cid(line);
          if (CellId==0) LineHeight=LineHt(line)+GetFrmSpcBef(w,line,FALSE);   // regular line
          else {
             if (tabw(line) && tabw(line)->type&INFO_ROW && cell[cid(line)].level==0) {
                LineHeight=TableRow[cell[CellId].row].height;
             }
             else continue;
          }
       }
       else continue;


       // check for column break
       if (line>FirstLine && tabw(line-1) && tabw(line-1)->type&INFO_COL)
            AfterHardColBreak=TRUE;  // preceding line was a hard col break
       else AfterHardColBreak=FALSE;
       if (NewColumn) AfterHardColBreak=FALSE;   // new column alredy started
       NewColumn=FALSE;

       // if current line goes beyound the column height or previous line was a hard column break
       if ( line>FirstLine && (((CurHeight+LineHeight)>ColHeight) || AfterHardColBreak) ) {
          if ((columns+1)<MaxColumns) {      // more column can be created
             if (CurHeight>MaxHeight) MaxHeight=CurHeight;

             // adjust for para keep/keepnext
             if (!AfterHardColBreak) {
                line=AdjustForParaKeep(w,line,FirstColLine,PageTop);  // get first line for the next column
                if (LineInfo(w,line,INFO_ROW)) {  // get the first line of the row
                   line--;
                   while (line>=FirstColLine) {
                       if (cid(line)==0 || (LineInfo(w,line,INFO_ROW) && TableLevel(w,line)==0)) break;
                       line--;
                   }
                   line++;                        // position on the first line of the row
                }    
                LineFlags(line)|=LFLAG_SOFT_COL;
             }

             CurHeight=0;
             FirstColLine=line;     // first line of the last column
             columns++;
             NewColumn=TRUE;

             line--;  // line height for this line not known yet
             continue;
          }
          else if (AfterHardColBreak || (CurHeight+LineHeight)>MaxColHeight) { // no more columns can be created
             line=AdjustForParaKeep(w,line,FirstColLine,PageTop);  // get first line for the next column
             (*pLastLine)=(line-1); // last line on the page
             return MaxHeight;
          }
       }

       CurHeight+=LineHeight;
    }

    if (CurHeight>MaxHeight) MaxHeight=CurHeight;  // height of the last column

    return MaxHeight;
}

/******************************************************************************
    AdjustRowHeightForBaseAlign:
    This routine adjust the row height when the cells us the base alignement option.
*******************************************************************************/
AdjustRowHeightForBaseAlign(PTERWND w,int row)
{
    int FirstCell,cl,MaxBaseHeight,MaxRowHeight;

    FirstCell=TableRow[row].FirstCell;
 
    for (cl=FirstCell;cl>0;cl=cell[cl].NextCell) if (cell[cl].flags&CFLAG_VALIGN_BASE) break;
    if (cl<=0) return TRUE;   // none of the cells use the base-alignment option

    // get the max base height for the first lines in the cell
    MaxBaseHeight=0;
    for (cl=FirstCell;cl>0;cl=cell[cl].NextCell) {
       if (cell[cl].flags&CFLAG_VALIGN_BASE && CellAux[cl].BaseHeight>MaxBaseHeight) MaxBaseHeight=CellAux[cl].BaseHeight;
    } 

    // Calculate additional space before for aligned cells
    for (cl=FirstCell;cl>0;cl=cell[cl].NextCell) {
       CellAux[cl].SpaceBefore=MaxBaseHeight-CellAux[cl].BaseHeight;
       CellAux[cl].height+=CellAux[cl].SpaceBefore;
    }

    if (TableRow[row].MinHeight<0) return TRUE;  // row height should not be changed

    // check if the additional space causes a
    MaxRowHeight=0; 
    for (cl=FirstCell;cl>0;cl=cell[cl].NextCell) if (CellAux[cl].height>MaxRowHeight) MaxRowHeight=CellAux[cl].height;

    if (MaxRowHeight>TableRow[row].height) TableRow[row].height=MaxRowHeight;

    return TRUE;
}

/******************************************************************************
    PosWatermarkFrame:
    Position the watermark frame for a page.
*******************************************************************************/
PosWatermarkFrame(PTERWND w,int page)
{
    int sect=PageInfo[page].TopSect;
    int PgWidth=InchesToTwips(TerSect1[sect].PgWidth);  // page width in twips
    int PgHeight=InchesToTwips(TerSect1[sect].PgHeight);
    int frm=WmParaFID;
    int pict;

    if (WmParaFID<=0) return TRUE;   // no watermark exists

    pict=ParaFrame[frm].pict;
    // check if the picture needs scaling
    if (pict>0) {
       int CurScale,scale=100;
       if (ParaFrame[frm].height>PgHeight) {
          CurScale=MulDiv(PgHeight,100,ParaFrame[frm].height);
          if (CurScale<scale) scale=CurScale;
       }
       if (ParaFrame[frm].width>PgWidth) {
          CurScale=MulDiv(PgWidth,100,ParaFrame[frm].width);
          if (CurScale<scale) scale=CurScale;
       }
       if (scale<100) {
          ParaFrame[frm].height=MulDiv(ParaFrame[frm].height,scale,100);
          ParaFrame[frm].width=MulDiv(ParaFrame[frm].width,scale,100);
          TerFont[pict].PictHeight=ParaFrame[frm].height;
          TerFont[pict].PictWidth=ParaFrame[frm].width;
          SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),true);
       } 
    }    

    // center the frame
    ParaFrame[frm].y=ParaFrame[frm].ParaY=(PgHeight-ParaFrame[frm].height)/2;  // center the frame
    ParaFrame[frm].x=(PgWidth-ParaFrame[frm].width)/2;  // relative to the page
    ParaFrame[frm].x-=InchesToTwips(TerSect[sect].LeftMargin);   // relative to the left border

    ResetUintFlag(ParaFrame[frm].flags,PARA_FRAME_VMARG);
    ParaFrame[frm].flags|=PARA_FRAME_VPAGE;  // set y relative to the top of the page

    return TRUE;
} 
