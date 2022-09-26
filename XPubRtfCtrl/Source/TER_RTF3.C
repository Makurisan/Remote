/*==============================================================================
   TER_RTF3.C
   RTF write for TER files/buffers

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

/****************************************************************************
    TerGetRtfSel:
    This function extracts the selected text in the RTF format.  The function
    returns the global handle containing the text and also returns the
    length of the buffer via a parameter 'BufLen' parameter
****************************************************************************/
HGLOBAL WINAPI _export TerGetRtfSel(HWND hWnd,LPLONG len)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL; // get the pointer to window data

    if (HilightType==HILIGHT_OFF) return NULL;  // no text selected

    if (!NormalizeBlock(w)) return NULL;  // normalize beginning and ending of the block

    if (RtfWrite(w,RTF_CB_BUF,NULL)) {
       (*len)=DlgLong1;            // length returned in DlgLong1
       return (HGLOBAL)(DlgLong);  // handle returned in DlgLong
    }
    else return FALSE;
}


/*****************************************************************************
    RtfWrite:
    Write the RTF file
******************************************************************************/
BOOL RtfWrite(PTERWND w,int output,LPBYTE OutFile)
{
    struct StrRtfOut far *rtf=NULL;
    struct StrRtfColor far *color=NULL;
    HANDLE  hFile=INVALID_HANDLE_VALUE;
    long l,WriteBegRow,WriteEndRow;
    LPBYTE ptr;
    LPWORD fmt;
    int  i,j,CurFID,len,CurFont,PrevPfmt,WriteBegCol,WriteEndCol,FirstCol,LastCol,
         sect,PrevCell,PrevFID,NewTag;
    BOOL result=FALSE,WriteEntireFile=FALSE,FunctResult=FALSE;
    BOOL NextDbcs,WritingTableRows=FALSE;
    UINT AllocType;
    HCURSOR hSaveCursor=NULL;
    BOOL JoinedFieldNames,NoFullCellSelection=FALSE;
    BOOL CellMarkerWasSelected=FALSE;
    BOOL NoFrames=True(TerFlags5&TFLAG5_FRAME_TEXT_ONLY);
    int  SectBreakLine=-1;
    COLORREF BkColor;

    // initialize global variables
    RtfInHdrFtr=0;
    RtfInTable=FALSE;

    // open the input file
    if (output==RTF_FILE) {                // data in the file
         rTrim(OutFile);                   // trim any spaces on right
         if (lstrlen(OutFile)==0) return FALSE;
         lstrcpy(TempString,OutFile);      // make a copy in the data segment
         unlink(TempString);
         if (INVALID_HANDLE_VALUE==(hFile=CreateFile(TempString,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) return FALSE;
    }

    // calculate range of lines to write
    if (output>=RTF_CB) {
       WriteBegRow=HilightBegRow;
       WriteEndRow=HilightEndRow;
       WriteBegCol=0;
       WriteEndCol=LineLen(WriteEndRow);     // last BYTE not included
       if (HilightType==HILIGHT_CHAR) {
          WriteBegCol=HilightBegCol;
          WriteEndCol=HilightEndCol;         // last BYTE not included
       }


      // restrict cell marker selection if requested
      if (TerFlags3&TFLAG3_NO_FULL_CELL_COPY && cid(WriteBegRow)>0
          && cid(WriteBegRow)==cid(WriteEndRow)
          && LineInfo(w,WriteEndRow,INFO_CELL) && WriteEndCol>=LineLen(WriteEndRow)
          && WriteEndCol>0) {
          CellMarkerWasSelected=TRUE;  // not that the cell marker was initially selected
          WriteEndCol--;    // exclude the cell marker character
          NoFullCellSelection=TRUE;
      }

    }
    else {                                   // write the entire file
       // fix the last line if needed
       // last line should always be a regular text line
       if (TotalLines>0 && (fid(TotalLines-1)>0 || cid(TotalLines-1)>0)) {
          TotalLines++;
          l=TotalLines-1;
          InitLine(w,l);
          LineAlloc(w,l,0,1);   // insert a paramarker character in the new line
          ptr=pText(l);
          ptr[0]=ParaChar;
          fmt=OpenCfmt(w,l);
          fmt[0]=DEFAULT_CFMT;
          CloseCfmt(w,l);
          pfmt(l)=pfmt(l-1);
          fid(l)=0;
          cid(l)=0;
       }
       
       WriteBegRow=0;
       WriteBegCol=0;
       WriteEndRow=TotalLines-1;
       WriteEndCol=LineLen(WriteEndRow);     // last BYTE not included
    }

    if (WriteBegRow==0 && WriteBegCol==0 && WriteEndRow==(TotalLines-1) && WriteEndCol==LineLen(WriteEndRow)) WriteEntireFile=TRUE;

    // allocate space for the Strrtf info structure
    if (NULL==(rtf=(struct StrRtfOut far *)OurAlloc(sizeof(struct StrRtfOut)))) {
       PrintError(w,MSG_OUT_OF_MEM,"RtfWrite");
       return FALSE;
    }


    FarMemSet(rtf,0,sizeof(struct StrRtfOut));  // initialize with zeros
    rtf->output=output;                         // output type
    if (rtf->output==RTF_FILE) rtf->hFile=hFile;
    else {
       rtf->BufLen=0;                // calculate estimated text size
       for (l=WriteBegRow;l<=WriteEndRow;l++) rtf->BufLen=rtf->BufLen+LineLen(l)+2;
       rtf->BufLen=(rtf->BufLen*3/2);// add estimated space for the formatting info
       if (rtf->BufLen<=0) rtf->BufLen=1;

       if (output==RTF_BUF) AllocType=GMEM_MOVEABLE;
       else                 AllocType=GMEM_MOVEABLE|GMEM_DDESHARE;   // clipboard data needs to be shareable
       if ( NULL==(rtf->hBuf=GlobalAlloc(AllocType,rtf->BufLen))
         || NULL==(rtf->buf=(BYTE huge *)GlobalLock(rtf->hBuf)) ){
          PrintError(w,MSG_OUT_OF_MEM,"RtfWrite(buf)");
          return FALSE;
       }
       rtf->BufIndex=0;              // next character position in the buffer
       rtf->hFile=INVALID_HANDLE_VALUE;

       if (rtf->output>=RTF_CB && HilightType==HILIGHT_CHAR && !NoFullCellSelection) rtf->TblHilight=TableHilighted(w);
    }


    // set the table level
    ClipTblLevel=1;                       // will be set to table level of the data being written out
    ClipEmbTable=FALSE;                   // has embedded table
    if (output>=RTF_CB) {
       if (!rtf->TblHilight || HilightType!=HILIGHT_CHAR) {
          ClipTblLevel=0;  // data contains regular text and may be tables as well
          ClipEmbTable=TRUE; // embed any mix of table/regular text into the target cell
       }
       else {
          int BegCell=cid(HilightBegRow);
          int EndCell=cid(HilightEndRow);
          int MinLevel=(cell[BegCell].level<cell[EndCell].level)?cell[BegCell].level:cell[EndCell].level;
          int MaxLevel,cl;
          long l;
          BOOL WholeCell=FALSE;
          BegCell=LevelCell(w,MinLevel,-BegCell);
          EndCell=LevelCell(w,MinLevel,-EndCell);
          if (BegCell!=EndCell) ClipTblLevel=MinLevel+1;
          else if (CellCharIncluded(w,WriteBegRow,WriteBegCol,WriteEndRow,WriteEndCol,MinLevel)) ClipTblLevel=MinLevel+1;

          // Is whole cell being written out
          if (cid(HilightBegRow)>0 && cid(HilightBegRow)==cid(HilightEndRow)) {
             if (CellMarkerWasSelected || LineInfo(w,HilightEndRow,INFO_CELL)) {
                if (HilightBegRow==0 || cid(HilightBegRow-1)!=cid(HilightBegRow)) WholeCell=TRUE;
             }
          } 
          
          // check if an embedded table exist in the selection
          if (!WholeCell && BegCell==EndCell) {    // within the same cell, look for embedded tables
             BOOL HasPlainText=FALSE;
             MinLevel=99;
             MaxLevel=0;
             for (l=WriteBegRow;l<=WriteEndRow;l++) {
                if ((cl=cid(l))==0) {
                   HasPlainText=TRUE;
                   continue;
                }
                if (cell[cl].level<MinLevel) MinLevel=cell[cl].level;
                if (cell[cl].level>MaxLevel) MaxLevel=cell[cl].level;
                if (MinLevel<MaxLevel || HasPlainText) {      // has multiple levels or a mix or table/regular text
                   ClipEmbTable=TRUE;
                   break;
                } 
             }
          }
       }
    }


    // set the LFLAG_SELECTED flag
    if (HilightType!=HILIGHT_OFF) {
       for (l=WriteBegRow;l<=WriteEndRow;l++) {
          if (LineSelected(w,l)) LineFlags(l)|=LFLAG_SELECTED;
          else                   ResetLongFlag(LineFlags(l),LFLAG_SELECTED);
       } 
    } 


    // allocate color table
    if (NULL==(color=rtf->color=(struct StrRtfColor far *)OurAlloc(sizeof(struct StrRtfFont)*MaxRtfColors))) {
       return PrintError(w,MSG_OUT_OF_MEM,"RtfRead(color)");
    }
    FarMemSet(color,0,sizeof(struct StrRtfFont)*MaxRtfColors);  // initialize with zeros

    // set wait cursor
    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while reading

    // mark the cells to write when a table is highlighted
    if (rtf->TblHilight) {
       int cl;
       SetCellLines(w);
       for (i=0;i<TotalCells;i++) cell[i].flags=ResetUintFlag(cell[i].flags,CFLAG_RTF_SEL);
       for (l=WriteBegRow;l<=WriteEndRow;l++) if (LineFlags(l)&LFLAG_SELECTED) cell[cid(l)].flags|=CFLAG_RTF_SEL;
       // check if writing whole table rows
       cl=cid(WriteBegRow);
       if (cl>0 && cell[cl].PrevCell<=0) {
          cl=cid(WriteEndRow);
          if (cl>0 && cell[cl].NextCell<=0 && LineInfo(w,WriteEndRow,(INFO_ROW|INFO_CELL))
              && WriteEndCol>=(LineLen(WriteEndRow)-1)) WritingTableRows=TRUE;
       }
    }


    // write the rtf header
    if (!BeginRtfGroup(w,rtf)) goto WRITE_END;
    if (!WriteRtfControl(w,rtf,"rtf",PARAM_INT,1)) goto WRITE_END;
    if (!WriteRtfControl(w,rtf,"ansi",PARAM_NONE,0)) goto WRITE_END;
    if (mbcs && !WriteRtfControl(w,rtf,"ansicpg",PARAM_INT,1252)) goto WRITE_END;
    if (!WriteRtfControl(w,rtf,"deflang",PARAM_INT,DefLang)) goto WRITE_END;
    if (!WriteRtfControl(w,rtf,"ftnbj",PARAM_NONE,0)) goto WRITE_END;  // footnote at the bottom of the page
    if (!WriteRtfControl(w,rtf,"uc",PARAM_INT,1)) goto WRITE_END;      // default number of ascii charqacters for a unicode character
    if (!WriteRtfFont(w,rtf)) goto WRITE_END; //write the font table
    if (!WriteRtfColor(w,rtf)) goto WRITE_END; //write the color table
    if (!WriteRtfStylesheet(w,rtf)) goto WRITE_END; //write the color table
    if (!WriteRtfRev(w,rtf)) goto WRITE_END; //write the reviewer table
    if (!WriteRtfList(w,rtf)) goto WRITE_END; //write the list table and list override table
    
    BkColor=True(TerFlags6&TFLAG6_SAVE_TEXT_BK_COLOR)?TextDefBkColor:PageBkColor;
    if (BkColor!=CLR_WHITE && (!HtmlMode || BkColor!=0xc0c0c0) && !WriteRtfBackground(w,rtf,BkColor)) goto WRITE_END;
    
    if (!WriteRtfMargin(w,rtf)) goto WRITE_END; //write default margin and paper information

    // write a markerif saving undo for cell deletion
    if (TerOpFlags&TOFLAG_DEL_CELL && !WriteRtfControl(w,rtf,"ssdelcell",PARAM_NONE,0)) goto WRITE_END; // end the previous section
    if (WritingTableRows && !WriteRtfControl(w,rtf,"sswholetablerows",PARAM_NONE,0)) goto WRITE_END; // end the previous section


    sect=GetSection(w,WriteBegRow);
    if (TRUE || output<RTF_CB || WriteEntireFile) {
      if (!WriteRtfSection(w,rtf,sect)) goto WRITE_END; //write section properties
    }

    // initialize character formatting information
    CurFont=DEFAULT_CFMT;
    rtf->group[rtf->GroupLevel].FontId=-1;   // to force writing of default properties
    if (!WriteRtfControl(w,rtf,"plain",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfCharFmt(w,rtf,CurFont,FALSE)) goto WRITE_END;

    // initialize paragraph formatting information
    PrevPfmt=pfmt(WriteBegRow);
    PrevCell=cid(WriteBegRow);
    PrevFID=fid(WriteBegRow);
    if (cid(WriteBegRow)>0) {
       PrevPfmt=DEFAULT_PFMT;    // force writing of cell data
       PrevCell=0;
       if (rtf->TblHilight || CellCharIncluded(w,WriteBegRow,WriteBegCol,WriteEndRow,WriteEndCol,-1))
             rtf->flags|=ROFLAG_WRITE_TROWD;    // write be table begin code
    }
    else if (ParaCharIncluded(w,WriteBegRow,WriteBegCol,WriteEndRow,WriteEndCol)) {
       PrevPfmt=DEFAULT_PFMT;    // force writing of frame data
    }
    if (PrevFID>0) {
       long EndLine=WriteEndRow;
       if (WriteEndCol==LineLen(WriteEndRow) && (EndLine+1)<TotalLines) EndLine++;
       if (NoFrames || fid(WriteBegRow)!=fid(EndLine)) PrevFID=0;  // write frame info
    }
    
    if (HilightType==HILIGHT_OFF) RtfInitLevel=0;           // keep track of subtable levels
    else {
       RtfInitLevel=MinTableLevel(w,WriteBegRow,WriteEndRow);
       if (!CellCharIncluded(w,WriteBegRow,WriteBegCol,WriteEndRow,WriteEndCol,RtfInitLevel)) RtfInitLevel++;  // the parent table will be written out as straight text
    }
    RtfCurLevel=RtfInitLevel;

    // write the text lines
    for (l=WriteBegRow;l<=WriteEndRow;l++) {
       rtf->line=l;               // record the current line number

       // skip over the generated endnote lines
       if (PfmtId[pfmt(l)].pflags&PFLAG_ENDNOTE) continue;

       // check for table highlighting
       if (rtf->TblHilight) {
         if (!(LineFlags(l)&LFLAG_SELECTED)) {                    // table line not selected
            if (!LineInfo(w,l,INFO_ROW)) continue;
            else if (cid(l) && TableLevel(w,l)>RtfInitLevel) continue;  // ROW_CHAR for nested tables not used for writing \row
         }
       }

       RtfPrevLevel=RtfCurLevel;
       RtfCurLevel=TableLevel(w,l);

       // write header control if any
       if (LineLen(l)==1 && LineFlags(l)&LFLAG_HDRS_FTRS) {
          if (!RtfInHdrFtr) {
             BOOL BlankHdrFtr=false;

             // check if blank header footer
             BlankHdrFtr=((l+2)<TotalLines && True(LineFlags(l+2)&LFLAG_HDRS_FTRS) && LineLen(l+1)==1);
             if (BlankHdrFtr) {       // skip-over blank hdr/ftr as they are auto-generated during header/footer display
                l=l+2;
                continue;
             }
             FlushRtfLine(w,rtf);                       // Create line berak for visual aid
             if (!BeginRtfGroup(w,rtf)) goto WRITE_END;
             RtfInHdrFtr=PAGE_HDR_FTR;

             if (LineFlags(l)&LFLAG_HDR) {
                 if (!WriteRtfControl(w,rtf,"header",PARAM_NONE,0)) goto WRITE_END; // begin the header group
                 if (WmParaFID>0 && !rtf->WatermarkWritten && output<RTF_CB && !WriteRtfWatermark(w,rtf,false)) goto WRITE_END;
             }
             else if (LineFlags(l)&LFLAG_FTR && !WriteRtfControl(w,rtf,"footer",PARAM_NONE,0)) goto WRITE_END;
             else if (LineFlags(l)&LFLAG_FHDR && !WriteRtfControl(w,rtf,"headerf",PARAM_NONE,0)) goto WRITE_END;
             else if (LineFlags(l)&LFLAG_FFTR && !WriteRtfControl(w,rtf,"footerf",PARAM_NONE,0)) goto WRITE_END;
             rtf->flags|=ROFLAG_WRITE_FONT;   // write full font information
          }
          else {
             CurFont=0;                      // force to the default font
             if (!WriteRtfCharFmt(w,rtf,CurFont,FALSE)) goto WRITE_END; // write the font change
             
             if (!EndRtfGroup(w,rtf)) goto WRITE_END;
             FlushRtfLine(w,rtf);                       // Create line berak for visual aid

             RtfInHdrFtr=0;
             PrevPfmt=-1;    // force write of attribute
          }
          continue;
       }

       // write watermark if not already written in a header group
       if (!RtfInHdrFtr && WmParaFID>0 && !rtf->WatermarkWritten && output<RTF_CB && !WriteRtfWatermark(w,rtf,true)) goto WRITE_END;

       // determine the column range to write
       FirstCol=0;
       LastCol=LineLen(l);                // last BYTE not included
       if (l==WriteBegRow) FirstCol=WriteBegCol;
       if (l==WriteEndRow) LastCol=WriteEndCol;

       // write paragraph formats
       CurFID=NoFrames?0:fid(l);
       if ((int)pfmt(l)!=PrevPfmt || cid(l)!=PrevCell || CurFID!=PrevFID || l==(SectBreakLine+1)) {
         // terminate character attributes which are limited to a paragraph such as subscr/subscr/field names
         EndInterParaGroups(w,rtf,GetCurCfmt(w,l,0));

         if (!WriteRtfParaFmt(w,rtf,pfmt(l),PrevPfmt,cid(l),PrevCell,CurFID,PrevFID,l)) goto WRITE_END;
         PrevPfmt=pfmt(l);
         PrevCell=cid(l);
         PrevFID=CurFID;

         // reset font as fonts could have changed in a higher group
         if (rtf->group[rtf->GroupLevel].FieldId==0 /*!=FIELD_TOC*/) {
            if (!WriteRtfCharFmt(w,rtf,0,FALSE)) goto WRITE_END;
         }
       }

       
       // Write character formats and para break
       if (FirstCol>=LastCol) goto LINE_END;  // nothing to write
       ptr=pText(l);                   // lock the text
       fmt=OpenCfmt(w,l);
       CurFont=fmt[FirstCol];
       rtf->IsDbcs=(pLead(l) && pLead(l)[0]);
       len=0;      // length of the segment
       for (j=FirstCol;j<=LastCol;j++) {
          NextDbcs=(pLead(l) && pLead(l)[j]);
          NewTag=(pCtid(l) && j<LastCol)?pCtid(l)[j]:0;
          if (NewTag && j==FirstCol) WriteRtfTag(w,rtf,NewTag);  // write the tag

          JoinedFieldNames=(j<LastCol && TerFont[CurFont].FieldId==FIELD_NAME
                            && TerFont[fmt[j]].FieldId==FIELD_NAME
                            && TerFont[fmt[j]].style&PROTECT && ptr[j]=='{'); // next field name follows

          if ( j==LastCol || fmt[j]!=(WORD)CurFont  || NextDbcs!=rtf->IsDbcs
            || ptr[j]==ParaChar || ptr[j]==CellChar || ptr[j]==LINE_CHAR
            || ptr[j]==ROW_CHAR || ptr[j]==SECT_CHAR || TerFont[fmt[j]].style&PICT
            || (NewTag && j>FirstCol)
            || JoinedFieldNames ) {

            int WriteBeginCol=j-len;


             // skip the opening and closing braces
             if (TerFont[CurFont].FieldId==FIELD_NAME && TerFont[CurFont].style&PROTECT) {
                if (ptr[WriteBeginCol]=='{') {
                   len--;  // don't write the opening braces for the field name
                   WriteBeginCol++;
                }
                if (ptr[j-1]=='}') len--;  // don't write the closing braces for the field name
             }

             // end previous field name for a joined field
             if (JoinedFieldNames && rtf->flags&ROFLAG_IN_FIELD_NAME) 
                 BeginRtfFieldName(w,rtf,FIELD_NAME,CurFont);   // terminate the previous field name and start new field name group

             if (len>0) {
                if (!WriteRtfCharFmt(w,rtf,CurFont,FALSE)) goto WRITE_END;
                if (!(rtf->flags&ROFLAG_HIDDEN_TEXT) && rtf->DelRevCount<=0 && !WriteRtfMbcsText(w,rtf,&(ptr[WriteBeginCol]),(pLead(l)?&(pLead(l)[WriteBeginCol]):NULL),TerFont[CurFont].UcBase,len,CurFont)) goto WRITE_END; // write text
                ResetUintFlag(rtf->flags,ROFLAG_CELL_CLOSED);  // will be set to TRUE after writing \cell tag
             }
                     
             if (j==LastCol) break;           // end of line
             len=0;                           // reset line segment length
          }

          // write the tag
          if (NewTag && j>FirstCol) WriteRtfTag(w,rtf,NewTag);

          CurFont=fmt[j];
          rtf->IsDbcs=NextDbcs;

          // write picture and parabreak
          if (TerFont[CurFont].style&PICT) {
             if (TerFont[CurFont].FieldId==FIELD_TEXTBOX) {   // get the text box font
                struct StrForm far *pForm=(LPVOID)TerFont[CurFont].pInfo;
                int NewFont=pForm->FontId;
                COLORREF SaveColor=TerFont[NewFont].TextBkColor;
                TerFont[NewFont].TextBkColor=CLR_WHITE;  // do not save shading
                if (!WriteRtfCharFmt(w,rtf,NewFont,FALSE)) goto WRITE_END; // write the font change
                TerFont[NewFont].TextBkColor=SaveColor;
             }
             else {        // write the font change
                if (!WriteRtfCharFmt(w,rtf,CurFont,FALSE)) goto WRITE_END; // write the font change
             }

             result=TRUE;
             if (output==RTF_FRAME_BUF) {     // write objects as ids
                result=WriteRtfControl(w,rtf,"subpictid",PARAM_INT,CurFont);
             }
             else if (TerFont[CurFont].ParaFID>0) result=WriteRtfShape(w,rtf,CurFont);
             else if (TerFont[CurFont].ObjectType==OBJ_NONE || TerFont[CurFont].ObjectType==OBJ_EMBED_TEMP) {
                result=WriteRtfPicture(w,rtf,CurFont);
             }
             else {
                result=WriteRtfObject(w,rtf,CurFont);
             }
             if (!result) goto WRITE_END;
          }
          else if (ptr[j]==ParaChar) {       // write paragraph break
             int ParaFID=PrevFID;
             int NewFID=NoFrames?0:fid(l);
             if (!WriteRtfCharFmt(w,rtf,CurFont,FALSE)) goto WRITE_END; // write the font change
             if (ParaFID==0 || !(ParaFrame[ParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT))) {
                if (!(rtf->flags&ROFLAG_HIDDEN_TEXT)) {
                   if (rtf->ParaFmtOnParaEnd && !WriteRtfParaFmt(w,rtf,pfmt(l),0,cid(l),PrevCell,CurFID,PrevFID,l)) goto WRITE_END;
                   rtf->ParaFmtOnParaEnd=FALSE;   // reset

                   if (!WriteRtfControl(w,rtf,"par",PARAM_NONE,0)) goto WRITE_END; // write para break
                }
             }
             rtf->FieldHasPara=TRUE;         // to keep track if field-result has multiple paragraphs
          }
          else if (ptr[j]==CellChar || ptr[j]==LINE_CHAR) { // write cell or line break
             if (!WriteRtfCharFmt(w,rtf,CurFont,FALSE)) goto WRITE_END; // write the font change
             if (ptr[j]==CellChar) {
                int ColSpan=cell[cid(l)].ColSpan;
                int level=cell[cid(l)].level;
                LPBYTE pCellTag;

                if (rtf->ParaFmtOnParaEnd && !WriteRtfParaFmt(w,rtf,pfmt(l),0,cid(l),PrevCell,CurFID,PrevFID,l)) goto WRITE_END;
                rtf->ParaFmtOnParaEnd=FALSE;   // reset
                
                pCellTag=(level==RtfInitLevel)?"cell":"nestcell";
                if (!WriteRtfControl(w,rtf,pCellTag,PARAM_NONE,0)) goto WRITE_END;
                if (level==RtfInitLevel) {
                   while (ColSpan>1) {      // write spanning for root level only - nested levels do not use clmrg tag
                      if (!WriteRtfControl(w,rtf,pCellTag,PARAM_NONE,0)) goto WRITE_END;
                      if (level>RtfInitLevel)  WriteRtfNoNestGroup(w,rtf);
                      ColSpan--;
                   }
                }
                else WriteRtfNoNestGroup(w,rtf);

                rtf->flags|=ROFLAG_CELL_CLOSED;           // cell closed
                FlushRtfLine(w,rtf);             // Create line berak for visual aid
             }
             if (ptr[j]==LINE_CHAR && TerFont[CurFont].CharId>0 && !WriteRtfControl(w,rtf,"lbr",PARAM_INT,TerFont[CurFont].CharId)) goto WRITE_END;
             if (ptr[j]==LINE_CHAR && !WriteRtfControl(w,rtf,"line",PARAM_NONE,0)) goto WRITE_END;
          }
          else if (ptr[j]==ROW_CHAR) {       // write row break
             int CurLevel=TableLevel(w,l);
             if (!WriteRtfCharFmt(w,rtf,CurFont,FALSE)) goto WRITE_END; // write the font change
             if (!WriteRtfControl(w,rtf,"intbl",PARAM_NONE,0)) goto WRITE_END; // write para break
             if (CurLevel==RtfInitLevel) {
                if (!WriteRtfControl(w,rtf,"row",PARAM_NONE,0)) goto WRITE_END; // write para break
                FlushRtfLine(w,rtf);             // Create line berak for visual aid
                RtfInTable=FALSE;
             }
             else {
                if (!WriteRtfControl(w,rtf,"itap",PARAM_INT,CurLevel-RtfInitLevel+1)) goto WRITE_END; // write subtable level
             }
             ResetUintFlag(rtf->flags,ROFLAG_CELL_CLOSED);                   // need to write the /cell for the next higher level
          }
          else if (ptr[j]==SECT_CHAR) {       // new section
             if (tabw(l) && tabw(l)->type&INFO_SECT) {
                if (!WriteRtfCharFmt(w,rtf,CurFont,TRUE)) goto WRITE_END; // write the font change
                if (l>WriteBegRow && cid(l-1)>0) if (!WriteRtfControl(w,rtf,"pard",PARAM_NONE,0)) goto WRITE_END;  // end any open table
                if (!WriteRtfControl(w,rtf,"sect",PARAM_NONE,0)) goto WRITE_END; // end the previous section

                sect=GetSection(w,l+1);          // get the next section
                if (!WriteRtfSection(w,rtf,sect)) goto WRITE_END; //write section properties
                SectBreakLine=l;
             }
          }
          else len++;                         // do not write para char
       }
       CloseCfmt(w,l);
       
       LINE_END:
       // Write EOL in non-wordwrap mode
       if (!TerArg.WordWrap && l<WriteEndRow && !WriteRtfControl(w,rtf,"par",PARAM_NONE,0)) goto WRITE_END; // write para break
    }


    // terminate table in not terminated properly
    if (RtfInTable) {
       if (TerOpFlags&TOFLAG_DEL_CELL && !WriteRtfControl(w,rtf,"sstblend",PARAM_NONE,0)) goto WRITE_END; // instructs the reader to ignore the following cell/row symbols
       EndInterParaGroups(w,rtf,GetCurCfmt(w,l,0));    // end groups such as subscript/field, etc
       for (i=RtfCurLevel;i>RtfInitLevel;i--) {  // terminate higher levels
          if (!WriteRtfControl(w,rtf,"intbl",PARAM_NONE,0)) goto WRITE_END;
          if (!WriteRtfControl(w,rtf,"itap",PARAM_INT,i-RtfInitLevel+1)) goto WRITE_END; // write subtable level
          if (!rtf->flags&ROFLAG_CELL_CLOSED && !WriteRtfControl(w,rtf,"nestcell",PARAM_NONE,0)) goto WRITE_END; // write para break
          if (!WriteRtfRow(w,rtf,0,LevelCell(w,i,l),i)) goto WRITE_END; // write row definition and nest row tag
          ResetUintFlag(rtf->flags,ROFLAG_CELL_CLOSED);                   // need to write the /cell for the next higher level
       } 
       
       if (!WriteRtfControl(w,rtf,"intbl",PARAM_NONE,0)) goto WRITE_END; // to specify the root level
       if (!(rtf->flags&ROFLAG_CELL_CLOSED) && !WriteRtfControl(w,rtf,"cell",PARAM_NONE,0)) goto WRITE_END; // write para break
       if (!WriteRtfControl(w,rtf,"row",PARAM_NONE,0)) goto WRITE_END; // write para break
    }

    // end all open groups
    for (j=rtf->GroupLevel;j>0;j--) if (!EndRtfGroup(w,rtf)) goto WRITE_END;

    FunctResult=TRUE;

    WRITE_END:

    // flush text buffer
    FlushRtfLine(w,rtf);             // flush the rtf line to the output

    if (output==RTF_FILE)  {
       CloseHandle(hFile);             // close file
       SetWindowText(hTerWnd,OutFile);
    }
    else if (output==RTF_BUF){      // assign new buffer to the output
       if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
       GlobalUnlock(rtf->hBuf);
       TerArg.hBuffer=rtf->hBuf;
       TerArg.BufferLen=rtf->BufIndex;// length of text in the buffer
    }
    else {                            // assign to clipboard
       rtf->buf[rtf->BufIndex]=0;
       GlobalUnlock(rtf->hBuf);
       if (output==RTF_CB) {
          HGLOBAL hInfo;
          struct StrClipInfo far *pInfo;

          SetClipboardData(RtfClipFormat,rtf->hBuf);
          
          // write the clipboard information
          if (NULL!=(hInfo=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,sizeof(struct StrClipInfo)))
            && NULL!=(pInfo=GlobalLock(hInfo)) ) {
            FarMemSet(pInfo,0,sizeof(struct StrClipInfo));
            pInfo->size=sizeof(struct StrClipInfo);
            pInfo->TblLevel=ClipTblLevel;
            pInfo->EmbTable=ClipEmbTable;
            GlobalUnlock(hInfo);
            SetClipboardData(SSClipInfo,hInfo);
          }
          ClipTblLevel=1;      // reset
          ClipEmbTable=TRUE;
       }
       else {
          DlgLong=(DWORD)(UINT)(rtf->hBuf);   // pass using a temporary varible
          DlgLong1=rtf->BufIndex;     // length of text in the buffer
       }
    }

    if (rtf)   OurFree(rtf);          // free rtf control area
    if (color) OurFree(color);        // free the rtf color table

    if (hSaveCursor) SetCursor(hSaveCursor);

    if (output==RTF_FILE || output==RTF_BUF) TerArg.modified=PageModifyCount=0;       // reset modification flag


    return FunctResult;
}

/*****************************************************************************
    WriteRtfCharFmt:
    Write the character formatting info when the attributes change from
    the previsous font selection
******************************************************************************/
BOOL WriteRtfCharFmt(PTERWND w,struct StrRtfOut far *rtf,int CurFont, BOOL BeforeBreak)
{
    int  CurFieldId,PrevFieldId;
    LPBYTE CurFieldCode,PrevFieldCode;
    int  GroupFont;
    UINT CurStyle,PrevStyle;
    BOOL ResetFont=FALSE;
    struct StrRtfOutGroup far *pGroup;
    UINT  DestStyleMask=(FNOTEALL);   // styles not affected by the 'plain' keyword

    if (TerFlags3&TFLAG3_OLD_WORD_FORMAT) DestStyleMask|=CHAR_BOX;  // in old format, character borders are written in a group

    if (CurFont>=TotalFonts) return TRUE;
    if (CurFont<0) return TRUE;

    // extract value for comparison
    pGroup=&(rtf->group[rtf->GroupLevel]);
    GroupFont=pGroup->FontId;
    if (GroupFont<0) GroupFont=0;
    PrevStyle=TerFont[GroupFont].style;          // SUPSCR and SUBSCR needed in the first part
    ResetUintFlag(PrevStyle,DestStyleMask);

    PrevFieldCode=pGroup->FieldCode;
    PrevFieldId=pGroup->FieldId;
    PrevStyle|=(pGroup->style&DestStyleMask);
    

    CurStyle=TerFont[CurFont].style;
    
    if (BeforeBreak) {
       CurFieldCode=0;   // do not enclose a break character in a field group
       CurFieldId=0;     // do not enclose a break character in a field group
    }
    else {
       CurFieldCode=TerFont[CurFont].FieldCode;
       CurFieldId=TerFont[CurFont].FieldId;
    }
    if (!(rtf->flags&ROFLAG_IN_MERGE_FIELD) && CurFieldId==FIELD_DATA) CurFieldId=0;

    // end superscript/subscript groups
    if ((CurStyle&SUPSCR)!=(PrevStyle&SUPSCR) && !(CurStyle&SUPSCR)) {
       if (!EndRtfGroup(w,rtf)) return FALSE;      // end the group
       rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_IN_SUPSCR);  // out of superscript group
    }
    if ((CurStyle&SUBSCR)!=(PrevStyle&SUBSCR) && !(CurStyle&SUBSCR)) {
       if (!EndRtfGroup(w,rtf)) return FALSE;      // end the group
       rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_IN_SUBSCR);  // out of subscript group
    }

    // temporarily halt superscript/subscript groups
    if (rtf->flags&ROFLAG_IN_SUPSCR) if (!EndRtfGroup(w,rtf)) return FALSE;      // end the group
    if (rtf->flags&ROFLAG_IN_SUBSCR) if (!EndRtfGroup(w,rtf)) return FALSE;      // end the group

    // terminate a field name/data group
    if ((CurFieldId!=FIELD_NAME && CurFieldId!=FIELD_DATA) && (PrevFieldId==FIELD_NAME || PrevFieldId==FIELD_DATA)) {
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end inner group
       if (PrevFieldId==FIELD_NAME) {                // no field data, so write an empty fldrslt group
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group for fldrslt
          WriteRtfControl(w,rtf,"fldrslt",PARAM_NONE,0);  // turnoff formatting
          if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field group
       } 
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field group
       ResetUintFlag(rtf->flags,(ROFLAG_IN_FIELD_NAME|ROFLAG_IN_MERGE_FIELD));
       //ResetFont=TRUE;
       if (rtf->FieldHasPara) rtf->ParaFmtOnParaEnd=TRUE;      // this fldrslt had multiple para, write the last para properties again
    }
    if (CurFieldId==FIELD_DATA && PrevFieldId==FIELD_NAME) {
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end the field name group
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group for fldrslt
       WriteRtfControl(w,rtf,"fldrslt",PARAM_NONE,0);  // turnoff formatting
       ResetUintFlag(rtf->flags,ROFLAG_IN_FIELD_NAME);
       rtf->FieldHasPara=FALSE;                      // TRUE will indicate multiple paragraph in the field
       //ResetFont=TRUE;
    }
    if (CurFieldId==FIELD_NAME && (PrevFieldId==0 || PrevFieldId==FIELD_DATA)) {
       BeginRtfFieldName(w,rtf,PrevFieldId, -1);
    }

    // Check for the end of a previous dynamic field (page number) group
    if ((IsDynField(w,PrevFieldId) || PrevFieldId==FIELD_TOC || PrevFieldId==FIELD_HLINK || PrevFieldId==FIELD_TEXTBOX) 
        && (PrevFieldId!=CurFieldId || !IsSameFieldCode(w,PrevFieldCode,CurFieldCode)) ) {  // close the previous field result
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field result
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field group

       ResetUintFlag(rtf->flags,ROFLAG_HIDDEN_TEXT);
       if (rtf->FieldHasPara) rtf->ParaFmtOnParaEnd=TRUE;  // this field had multple paragraphs - write para fmt again
       //ResetFont=TRUE;
    }

    // Check for the end of a tc field group
    if (PrevFieldId==FIELD_TC && PrevFieldId!=CurFieldId) {  // close the previous field result
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field group

       ResetUintFlag(rtf->flags,ROFLAG_IN_FIELD_TC);
       ResetUintFlag(rtf->flags,ROFLAG_HIDDEN_TEXT);
    }

    // Check for the end of a xe field group
    if (PrevFieldId==FIELD_XE && PrevFieldId!=CurFieldId) {  // close the previous field result
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field group

       ResetUintFlag(rtf->flags,ROFLAG_IN_FIELD_XE);
    }

    // Check for the end of a previous equation (boxed string)
    if (TerFlags3&TFLAG3_OLD_WORD_FORMAT && (PrevStyle&CHAR_BOX)!=(CurStyle&CHAR_BOX)) {
       if (PrevStyle&CHAR_BOX) {  // close the previous field result
          WriteRtfText(w,rtf,")",1);                    // close the equation
          if (!EndRtfGroup(w,rtf)) return FALSE;        // close the fldinst group
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group for fldrslt
          if (!WriteRtfControl(w,rtf,"fldrslt",PARAM_NONE,0)) return FALSE;   // field display
          if (!EndRtfGroup(w,rtf)) return FALSE;        // close fldrslt group
          if (!EndRtfGroup(w,rtf)) return FALSE;        // close field group
       }
       else {
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin field group
          if (!WriteRtfControl(w,rtf,"field",PARAM_NONE,0)) return FALSE;
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin instruction group
          if (!WriteRtfControl(w,rtf,"fldinst",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfText(w,rtf,"EQ \\X(",6)) return FALSE;
       }

    }

    // check for the end of previous footnote
    if (PrevStyle&FNOTETEXT && CurStyle&FNOTEREST) if (!EndRtfGroup(w,rtf)) return FALSE;
    if (PrevStyle&FNOTEREST && !(CurStyle&(FNOTETEXT|FNOTEREST))) if (!EndRtfGroup(w,rtf)) return FALSE;
    if (PrevStyle&FNOTE && !(CurStyle&(FNOTE|FNOTETEXT|FNOTEREST))) if (!EndRtfGroup(w,rtf)) return FALSE;
    if (PrevStyle&FNOTETEXT && !(CurStyle&(FNOTETEXT|FNOTEREST))) {
       if (!EndRtfGroup(w,rtf)) return FALSE;
       if (!EndRtfGroup(w,rtf)) return FALSE;
    }

    // check for the beginning a footnote style
    if (!(PrevStyle&FNOTE) && CurStyle&FNOTE) if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin footnote reference group
    if (PrevStyle&FNOTE && CurStyle&FNOTETEXT) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin footnote reference group
       if (!WriteRtfControl(w,rtf,"footnote",PARAM_NONE,0)) return FALSE;
       if (CurStyle&ENOTETEXT && !WriteRtfControl(w,rtf,"ftnalt",PARAM_NONE,0)) return FALSE;  // endnote text
       rtf->flags|=ROFLAG_WRITE_FONT;  // write the font tag unconditionally, msword needs it
       rtf->group[rtf->GroupLevel].flags|=ROGFLAG_WRITE_FONT;  // need so that even when level changes because of immediate 'super', the following text is still written with full font
    }

    // Check for the beginning of new field group
    if ((IsDynField(w,CurFieldId) || CurFieldId==FIELD_TOC || CurFieldId==FIELD_HLINK || CurFieldId==FIELD_TEXTBOX)
        && (CurFieldId!=PrevFieldId || !IsSameFieldCode(w,CurFieldCode,PrevFieldCode))) {
       
       if (!WriteRtfFontAttrib(w,rtf,CurFont)) return FALSE;

       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin field group
       if (!WriteRtfControl(w,rtf,"field",PARAM_NONE,0)) return FALSE;
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin instruction group
       if (!WriteRtfControl(w,rtf,"fldinst",PARAM_NONE,0)) return FALSE;

       // write any font change for dynamic field
       if (CurFieldId==FIELD_PAGE_NUMBER || CurFieldId==FIELD_PAGE_COUNT || CurFieldId==FIELD_DATE
           || CurFieldId==FIELD_PRINTDATE) {
           if (!WriteRtfFontAttrib(w,rtf,CurFont)) return FALSE;
       }

       if (CurFieldId==FIELD_PAGE_NUMBER)     if (!WriteRtfText(w,rtf,"PAGE",4)) return FALSE;
       if (CurFieldId==FIELD_PAGE_COUNT) if (!WriteRtfText(w,rtf,"NUMPAGES",8)) return FALSE;
       
       if (CurFieldId==FIELD_DATE || CurFieldId==FIELD_PRINTDATE) {
           BYTE string[50];
           lstrcpy(string,(CurFieldId==FIELD_DATE)?"TIME":"PRINTDATE");;
           if (CurFieldCode) {
              lstrcat(string," \\@ ");
              AddChar(string,'"');
              lstrcat(string,CurFieldCode);
              AddChar(string,'"');
           }
           if (!WriteRtfText(w,rtf,string,lstrlen(string))) return FALSE;
       }
       if (CurFieldId==FIELD_TOC) {
           BYTE string[50]="TOC ";
           if (CurFieldCode) {
              if (!WriteRtfText(w,rtf,string,lstrlen(string))) return FALSE;
              //rtf->flags|=ROFLAG_IGNORE_SLASH;
              if (!WriteRtfText(w,rtf,CurFieldCode,lstrlen(CurFieldCode))) return FALSE;
              ResetLongFlag(rtf->flags,ROFLAG_IGNORE_SLASH);
           }
           else {
              lstrcat(string,"\\o ");
              AddChar(string,'"');
              lstrcat(string,"1-9");
              AddChar(string,'"');
              if (!WriteRtfText(w,rtf,string,lstrlen(string))) return FALSE;
           }
       } 
       if (CurFieldId==FIELD_LISTNUM || CurFieldId==FIELD_AUTONUMLGL 
           || CurFieldId==FIELD_HLINK || CurFieldId==FIELD_PAGE_REF) {
           BYTE string[MAX_WIDTH+20];
           if (CurFieldId==FIELD_LISTNUM)    lstrcpy(string,"LISTNUM");
           if (CurFieldId==FIELD_AUTONUMLGL) lstrcpy(string,"AUTONUMLGL");
           if (CurFieldId==FIELD_HLINK)      lstrcpy(string,"HYPERLINK");
           if (CurFieldId==FIELD_PAGE_REF)   lstrcpy(string,"PAGEREF");
           if (CurFieldCode) {
              lstrcat(string," ");
              if (CurFieldId==FIELD_HLINK) AppendRtfHlink(w,rtf,string,CurFieldCode);
              else  {
                 lstrcat(string,CurFieldCode);
                 if (CurFieldId==FIELD_PAGE_REF) lstrcat(string," ");  // MSWord likes an additional space at the end when \\h switch is used
              }
           } 
           if (!WriteRtfText(w,rtf,string,lstrlen(string))) return FALSE;

           rtf->FieldHasPara=FALSE;      // to keep track if the field has multiple paragrahs
       } 
       
       if (CurFieldId==FIELD_TEXTBOX && !WriteRtfFormTextField(w,rtf,CurFieldCode)) return false; 

       if (!EndRtfGroup(w,rtf)) return FALSE;        // end the instruction group
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin the result group
       if (!WriteRtfControl(w,rtf,"fldrslt",PARAM_NONE,0)) return FALSE;
    }

    // begin tc field groups
    if (CurFieldId==FIELD_TC && PrevFieldId!=FIELD_TC && False(rtf->flags&ROFLAG_IN_FIELD_TC)) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin the group
       if (!WriteRtfControl(w,rtf,"tc",PARAM_NONE,0)) return TRUE; // turn on
       if (CurFieldCode!=null && lstrlen(CurFieldCode)>0) {
          rtf->flags|=ROFLAG_IN_FIELD_TC;
          WriteRtfText(w,rtf,CurFieldCode,lstrlen(CurFieldCode));    // write the field code
          ResetUintFlag(rtf->flags,ROFLAG_IN_FIELD_TC);
       }
    }

    // begin xe field groups
    if (CurFieldId==FIELD_XE && PrevFieldId!=FIELD_XE && False(rtf->flags&ROFLAG_IN_FIELD_XE)) {
       rtf->flags|=ROFLAG_IN_FIELD_XE;
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin the group
       if (!WriteRtfControl(w,rtf,"xe",PARAM_NONE,0)) return TRUE; // turn on
    }

    // begin superscript/subscript groups
    if ((CurStyle&SUPSCR)!=(PrevStyle&SUPSCR) && CurStyle&SUPSCR) rtf->flags|=ROFLAG_IN_SUPSCR;
    if (rtf->flags&ROFLAG_IN_SUPSCR) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin the group
       if (!WriteRtfControl(w,rtf,"super",PARAM_NONE,0)) return TRUE; // turn on
    }
    if ((CurStyle&SUBSCR)!=(PrevStyle&SUBSCR) && CurStyle&SUBSCR) rtf->flags|=ROFLAG_IN_SUBSCR;
    if (rtf->flags&ROFLAG_IN_SUBSCR) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin the group
       if (!WriteRtfControl(w,rtf,"sub",PARAM_NONE,0)) return TRUE; // turn on
    }

    // update the field information for the current level
    pGroup=&(rtf->group[rtf->GroupLevel]);
    if (BeforeBreak) {       // field group not used when writing a break character
       pGroup->FieldId=0;
       pGroup->FieldCode=0;
    }
    else {
       pGroup->FieldId=CurFieldId;
       pGroup->FieldCode=TerFont[CurFont].FieldCode;
    }
    
    
    // PART_II:    // apply pure character styles (groupless styles)
    if (!WriteRtfFontAttrib(w,rtf,CurFont)) return FALSE;

    return TRUE;
}

/*****************************************************************************
    WriteRtfFontAttrib:
    Write the font attribute
******************************************************************************/
BOOL WriteRtfFontAttrib(PTERWND w,struct StrRtfOut far *rtf,int CurFont)
{
    BYTE CurTypeFace[32],PrevTypeFace[32];
    BYTE CurFamily,PrevFamily;
    UINT CurStyle,PrevStyle;
    int  CurCharSID,PrevCharSID,CurAuxId,PrevAuxId;
    int  i,CurTwipsSize,PrevTwipsSize;
    int  CurExpand,PrevExpand;
    int  CurLang,PrevLang,CurOffset,PrevOffset,GroupFont,PrevCharSet,CurCharSet;
    BOOL result,ResetFont=FALSE;
    COLORREF PrevTextColor,CurTextColor;
    COLORREF PrevTextBkColor,CurTextBkColor;
    COLORREF PrevUlineColor,CurUlineColor;
    UINT  DestStyleMask=(FNOTEALL);   // styles not affected by the 'plain' keyword
    UINT style;
    struct StrRtfOutGroup far *pGroup;
    BOOL WriteAll=FALSE;
    int  CurInsRev,PrevInsRev,CurDelRev,PrevDelRev;
    DWORD  CurInsTime,PrevInsTime,CurDelTime,PrevDelTime;


    if (TerFlags3&TFLAG3_OLD_WORD_FORMAT) DestStyleMask|=CHAR_BOX;  // in old format, character borders are written in a group

    if (CurFont>=TotalFonts) return TRUE;
    if (CurFont<0) return TRUE;

    // get the current attributes
    lstrcpy(CurTypeFace,TerFont[CurFont].TypeFace);
    CurFamily=TerFont[CurFont].FontFamily;
    CurStyle=TerFont[CurFont].style;
    CurTextColor=TerFont[CurFont].TextColor;
    CurTextBkColor=TerFont[CurFont].TextBkColor;
    CurUlineColor=TerFont[CurFont].UlineColor;
    CurCharSID=TerFont[CurFont].CharStyId;
    CurTwipsSize=TerFont[CurFont].TwipsSize;
    if (CurTwipsSize==0 && True(TerFont[CurFont].style&PICT)) CurTwipsSize=(TerArg.PointSize*20);  // the font.TwipsSize can be zero for picture objects
    CurAuxId=TerFont[CurFont].AuxId;
    CurExpand=TerFont[CurFont].expand;
    CurLang=TerFont[CurFont].lang;
    CurCharSet=TerFont[CurFont].CharSet;
    CurOffset=TerFont[CurFont].offset;
    CurInsRev=TerFont[CurFont].InsRev;
    CurDelRev=TerFont[CurFont].DelRev;
    CurInsTime=TerFont[CurFont].InsTime;
    CurDelTime=TerFont[CurFont].DelTime;

    if (TerFlags2&TFLAG2_NO_HIDDEN_RTF_TEXT) {
       if (CurStyle&HIDDEN) rtf->flags|=ROFLAG_HIDDEN_TEXT;
       else                 ResetUintFlag(rtf->flags,ROFLAG_HIDDEN_TEXT);
    }
 
    // get the previouse group font information
    pGroup=&(rtf->group[rtf->GroupLevel]);

    if (pGroup->FontId<0 || True(rtf->flags&ROFLAG_WRITE_FONT) || True(rtf->group[rtf->GroupLevel].flags&ROGFLAG_WRITE_FONT)) {
       pGroup->FontId=0;
       WriteAll=TRUE;
       ResetUintFlag(rtf->flags,ROFLAG_WRITE_FONT);
       ResetUintFlag(rtf->group[rtf->GroupLevel].flags,ROGFLAG_WRITE_FONT);
    } 
    

    PrevStyle=(pGroup->style&DestStyleMask);
    

    GroupFont=pGroup->FontId;
    if (WriteAll) {     // WriteAll forces this function to write all non-default properties
       PrevTypeFace[0]=0;
       PrevFamily=0;
       PrevTextColor=CLR_AUTO;
       PrevTextBkColor=CLR_WHITE;
       PrevUlineColor=CLR_AUTO;
       PrevCharSID=1;
       PrevTwipsSize=0;
       PrevAuxId=0;
       PrevExpand=0;
       PrevLang=0;
       PrevOffset=0;
       PrevCharSet=DEFAULT_CHARSET;
       PrevInsRev=0;
       PrevDelRev=0;
       PrevInsTime=0;
       PrevDelTime=0;
    }
    else {
       lstrcpy(PrevTypeFace,TerFont[GroupFont].TypeFace);
       PrevFamily=TerFont[GroupFont].FontFamily;
       
       style=TerFont[GroupFont].style;
       ResetUintFlag(style,DestStyleMask);  // desti style comes from the text steam
       PrevStyle|=style;

       PrevTextColor=TerFont[GroupFont].TextColor;
       PrevTextBkColor=TerFont[GroupFont].TextBkColor;
       PrevUlineColor=TerFont[GroupFont].UlineColor;
       PrevCharSID=TerFont[GroupFont].CharStyId;
       PrevTwipsSize=TerFont[GroupFont].TwipsSize;
       PrevAuxId=TerFont[GroupFont].AuxId;
       PrevExpand=TerFont[GroupFont].expand;
       PrevLang=TerFont[GroupFont].lang;
       PrevCharSet=TerFont[GroupFont].CharSet;
       PrevOffset=TerFont[GroupFont].offset;
       PrevInsRev=TerFont[GroupFont].InsRev;
       PrevDelRev=TerFont[GroupFont].DelRev;
       PrevInsTime=TerFont[GroupFont].InsTime;
       PrevDelTime=TerFont[GroupFont].DelTime;
    }

    if (!WriteAll && pGroup->FontId==CurFont 
                  && pGroup->style==(TerFont[CurFont].style&DestStyleMask)) return TRUE;   // font properties not changed
    
    pGroup->FontId=CurFont;
    pGroup->style=TerFont[CurFont].style&DestStyleMask;

    
    // check for default properties
    if ( lstrcmpi(CurTypeFace,TerArg.FontTypeFace)==0
      && CurTwipsSize==(TerArg.PointSize*20)
      && (CurTextColor==0 || CurTextColor==CLR_AUTO)
      && CurTextBkColor==CLR_WHITE
      && CurUlineColor==CLR_AUTO
      && (CurStyle&(~((UINT)(DestStyleMask))))==0
      && CurCharSID==1
      && CurLang==0 
      && CurOffset==0
      && CurAuxId==0 
      && CurExpand==0
      && CurInsRev==0
      && CurDelRev==0      ) {
         if (!WriteRtfControl(w,rtf,"plain",PARAM_NONE,0)) return FALSE;
         if (CurFont>=0 && !WriteRtfControl(w,rtf,"f",PARAM_INT,TerFont[CurFont].RtfIndex)) return FALSE;  // write font number
         if (!WriteRtfControl(w,rtf,"fs",PARAM_INT,CurTwipsSize/10)) return FALSE;  // convert twips to pointsize2
         ResetUintFlag(rtf->flags,ROFLAG_HIDDEN_TEXT);
         return TRUE;
    }


    //write the character style id
    if (CurCharSID!=PrevCharSID) {
      if (!WriteRtfControl(w,rtf,"cs",PARAM_INT,CurCharSID)) return FALSE;
    }

    //write language
    if (CurLang!=PrevLang && !WriteRtfControl(w,rtf,"lang",PARAM_INT,TerFont[CurFont].lang?TerFont[CurFont].lang:DefLang)) return FALSE;  // write font language

    //write typeface/family
    if (lstrcmpi(CurTypeFace,PrevTypeFace)!=0 || CurFamily!=PrevFamily || CurLang!=PrevLang 
          || CurCharSet!=PrevCharSet) {
      if (!WriteRtfControl(w,rtf,"f",PARAM_INT,TerFont[CurFont].RtfIndex)) return FALSE;  // write font number
    }

    //write foreground color color
    if (CurTextColor!=PrevTextColor) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==CurTextColor) break;
       if (i==rtf->TotalColors) i=0;
       if (!WriteRtfControl(w,rtf,"cf",PARAM_INT,i)) return FALSE;        // write the color index
    }

    //write background color color
    if (CurTextBkColor!=PrevTextBkColor) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==CurTextBkColor) break;
       if (i==rtf->TotalColors) i=0;
       //if (!WriteRtfControl(w,rtf,"cb",PARAM_INT,i)) return FALSE;        // write the color index - old method - Words ignores this now
       if (!WriteRtfControl(w,rtf,"highlight",PARAM_INT,i)) return FALSE;   // this background color gets modified over another background like cell background color
    }

    //write underline color
    if (CurUlineColor!=PrevUlineColor) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==CurUlineColor) break;
       if (i==rtf->TotalColors) i=0;
       if (!WriteRtfControl(w,rtf,"ulc",PARAM_INT,i)) return FALSE;        // write the color index
    }

    //write point size
    if (CurTwipsSize!=PrevTwipsSize) if (!WriteRtfControl(w,rtf,"fs",PARAM_INT,CurTwipsSize/10)) return FALSE;   // write point size

    //write aux id
    if (CurAuxId!=PrevAuxId && !HtmlMode && !WriteRtfControl(w,rtf,"sscharaux",PARAM_INT,CurAuxId)) return FALSE;   // write aux id

    // write font expansion
    if (CurExpand!=PrevExpand) {
        if (!WriteRtfControl(w,rtf,"expnd",PARAM_INT,(CurExpand*4/20))) return FALSE;   // write expansion in 1/4 of points
        if (!WriteRtfControl(w,rtf,"expndtw",PARAM_INT,CurExpand)) return FALSE;   // write expansion in twips
    }
    
    if (CurOffset!=PrevOffset) {
       if (CurOffset==0) result=WriteRtfControl(w,rtf,PrevOffset>0?"up":"dn",PARAM_INT,0); // turn on
       else              result=WriteRtfControl(w,rtf,CurOffset>0?"up":"dn",PARAM_INT,TwipsToPoints(abs(CurOffset)*2)); // turn on
    } 

    // write tracking changes
    if (CurInsRev!=PrevInsRev) { // process bold
       if (rtf->output<RTF_CB || !TrackChanges) {
          if (CurInsRev!=0) {
             if (!WriteRtfControl(w,rtf,"revised",PARAM_NONE,0)) return false;         // turn on
             if (!WriteRtfControl(w,rtf,"revauth",PARAM_INT,CurInsRev)) return false;  // turn on
             if (!WriteRtfControl(w,rtf,"revdttm",PARAM_INT,(long)GetRtfTrackingTime(w,CurInsTime))) return false;  // turn on
          } 
          else  if (!WriteRtfControl(w,rtf,"revised",PARAM_INT,0)) return false;  // turn off
       }
    }
    if (CurDelRev!=PrevDelRev) { // process bold
       if (rtf->output<RTF_CB || !TrackChanges) {
         if (CurDelRev!=0) {
            if (!WriteRtfControl(w,rtf,"deleted",PARAM_NONE,0)) return false;         // turn on
            if (!WriteRtfControl(w,rtf,"revauthdel",PARAM_INT,CurDelRev)) return false;  // turn on
            if (!WriteRtfControl(w,rtf,"revdttmdel",PARAM_INT,(long)GetRtfTrackingTime(w,CurDelTime))) return false;  // turn on
         } 
         else  if (!WriteRtfControl(w,rtf,"deleted",PARAM_INT,0)) return false;  // turn off
       }
       else {
         if (CurDelRev!=0) rtf->DelRevCount++;
         else              rtf->DelRevCount--;
       } 
    }

    if (!WriteRtfFontStyle(w,rtf,CurStyle,PrevStyle)) return FALSE;

    return TRUE;
}

/*****************************************************************************
    GetRtfTrackingTime:
    Convert DOS date/time to RTF date time for change tracking
******************************************************************************/
DWORD GetRtfTrackingTime(PTERWND w,DWORD DateTime)
{
    FILETIME FileTime;
    SYSTEMTIME SysTime;

    DosDateTimeToFileTime((WORD)(DateTime>>16),(WORD)(DateTime&0xFFFF),&FileTime);
    FileTimeToSystemTime(&FileTime,&SysTime);

    // convert to rtf format
    DateTime=(SysTime.wMinute) | (SysTime.wHour<<6) | (SysTime.wDay<<11) | (SysTime.wMonth<<16) 
              | ((SysTime.wYear-1900)<<20) | (SysTime.wDayOfWeek<<29);

    return DateTime;
}
 
/*****************************************************************************
    WriteRtfFontStyle:
    Write character styles
******************************************************************************/
BOOL WriteRtfFontStyle(PTERWND w,struct StrRtfOut far *rtf,UINT CurStyle, UINT PrevStyle)
{
    BOOL result;

    if ((CurStyle&BOLD)!=(PrevStyle&BOLD)) { // process bold
       if (CurStyle&BOLD) result=WriteRtfControl(w,rtf,"b",PARAM_NONE,0); // turn on
       else               result=WriteRtfControl(w,rtf,"b",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    if ((CurStyle&HLINK)!=(PrevStyle&HLINK)) { // hyperlink style
       if (CurStyle&HLINK) result=WriteRtfControl(w,rtf,"sshlink",PARAM_NONE,0); // turn on
       else                result=WriteRtfControl(w,rtf,"sshlink",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    if ((CurStyle&ULINE)!=(PrevStyle&ULINE)) { // process underline
       if (CurStyle&ULINE) result=WriteRtfControl(w,rtf,"ul",PARAM_NONE,0); // turn on
       else                result=WriteRtfControl(w,rtf,"ul",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    if ((CurStyle&ULINED)!=(PrevStyle&ULINED)) { // process double underline
       if (CurStyle&ULINED) result=WriteRtfControl(w,rtf,"uldb",PARAM_NONE,0); // turn on
       else                 result=WriteRtfControl(w,rtf,"uldb",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    if ((CurStyle&ITALIC)!=(PrevStyle&ITALIC)) { // process italic
       if (CurStyle&ITALIC) result=WriteRtfControl(w,rtf,"i",PARAM_NONE,0); // turn on
       else                 result=WriteRtfControl(w,rtf,"i",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    
    if (TerFlags2&TFLAG2_NO_HIDDEN_RTF_TEXT) {
       if (CurStyle&HIDDEN) rtf->flags|=ROFLAG_HIDDEN_TEXT;
       else                 ResetUintFlag(rtf->flags,ROFLAG_HIDDEN_TEXT);
    }
    else if ((CurStyle&HIDDEN)!=(PrevStyle&HIDDEN)) { // process hidden text
       if (CurStyle&HIDDEN) result=WriteRtfControl(w,rtf,"v",PARAM_NONE,0); // turn on
       else                 result=WriteRtfControl(w,rtf,"v",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    
    if ((CurStyle&STRIKE)!=(PrevStyle&STRIKE)) { // process strike
       if (CurStyle&STRIKE) result=WriteRtfControl(w,rtf,"strike",PARAM_NONE,0); // turn on
       else                 result=WriteRtfControl(w,rtf,"strike",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    if ((CurStyle&CAPS)!=(PrevStyle&CAPS)) { // process strike
       if (CurStyle&CAPS) result=WriteRtfControl(w,rtf,"caps",PARAM_NONE,0); // turn on
       else                 result=WriteRtfControl(w,rtf,"caps",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    if ((CurStyle&SCAPS)!=(PrevStyle&SCAPS)) { // process strike
       if (CurStyle&SCAPS) result=WriteRtfControl(w,rtf,"scaps",PARAM_NONE,0); // turn on
       else                 result=WriteRtfControl(w,rtf,"scaps",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }
    if ((CurStyle&PROTECT)!=(PrevStyle&PROTECT)) { // process protect
       if (CurStyle&PROTECT) result=WriteRtfControl(w,rtf,"protect",PARAM_NONE,0); // turn on
       else                  result=WriteRtfControl(w,rtf,"protect",PARAM_INT,0);  // turn off
       if (!result) return FALSE;
    }

    if ((CurStyle&CHAR_BOX)!=(PrevStyle&CHAR_BOX)) {
       if (CurStyle&CHAR_BOX) {
          if (!WriteRtfControl(w,rtf,"chbrdr",PARAM_NONE,0)) return FALSE;   // Word97 keyword
          if (!WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;    // single line border
          if (!WriteRtfControl(w,rtf,"brdrw",PARAM_INT,10)) return FALSE;    // default border width
       }
       else {
          if (!WriteRtfControl(w,rtf,"chbrdr",PARAM_INT,0)) return FALSE;   // Word97 keyword
       } 
    }

    return TRUE;
} 

/*****************************************************************************
    AppendRtfHlink:
    Append href information to the given string
******************************************************************************/
BOOL AppendRtfHlink(PTERWND w,struct StrRtfOut far *rtf, LPBYTE out, LPBYTE code)
{
    LPBYTE string,pos;
    BYTE FirstChar;
    int i,len,begin,end;

    string=MemAlloc(lstrlen(code)+1);
    lstrcpy(string,code);

    lstrupr(w,string);

    pos=strstr(string,"HREF=");
    if ((HtmlMode || True(TerFlags4&TFLAG4_HTML_INPUT)) && pos) {
       i=(int)((DWORD)pos-(DWORD)string);  // index of the href string
       i+=5;  // go past href=

       len=lstrlen(code);

       while(code[i]==' ' && i<len) i++;   // find the first non-space character

       if (i<len) {
          FirstChar=code[i];
          begin=i;                    // begin href string
          for (i=begin+1;i<len;i++) {
             if (FirstChar=='\'' || FirstChar=='"') {
                if (code[i]==FirstChar) {
                   i++;
                   break;  // end 
                }
             }
             else if (code[i]==' ') break;
          }
          end=i;
          lstrcpy(string,&(code[begin]));
          string[end-begin]=0;
          lstrcat(out,string); 
       } 
       else lstrcat(out,code);
    }
    else lstrcat(out,code);
    
    MemFree(string);

    return TRUE;
}
 
/*****************************************************************************
    ResetRtfFont:
    Reset the group font
******************************************************************************/
BOOL ResetRtfFont(PTERWND w,struct StrRtfOut far *rtf)
{
    struct StrRtfOutGroup far *pGroup=&(rtf->group[rtf->GroupLevel]);

    if (!WriteRtfControl(w,rtf,"plain",PARAM_NONE,0)) return FALSE;

    pGroup->FontId=-1;   // to force writing all character formatting 

    return TRUE;
}
 
/*****************************************************************************
    BeginRtfFieldName:
    Begin a mail-merge field name
******************************************************************************/
BOOL BeginRtfFieldName(PTERWND w,struct StrRtfOut far *rtf,int PrevFieldId, int CurFont)
{

    LPBYTE pString;

    if (PrevFieldId==FIELD_DATA || PrevFieldId==FIELD_NAME) { // terminate the previous field
        if (rtf->flags&ROFLAG_IN_MERGE_FIELD) {
           if (!EndRtfGroup(w,rtf)) return FALSE;        // end inner group
           if (!EndRtfGroup(w,rtf)) return FALSE;        // end current field group
           ResetUintFlag(rtf->flags,ROFLAG_IN_MERGE_FIELD);
        }
    }

    if (!BeginRtfGroup(w,rtf)) return FALSE;         // begin field group
    if (!WriteRtfControl(w,rtf,"field",PARAM_NONE,0)) return FALSE;   // field display
    rtf->flags|=ROFLAG_IN_MERGE_FIELD;               // in mail merge field group
 
 //MAK nicht fixieren   
		//if (!WriteRtfControl(w,rtf,"fldlock",PARAM_NONE,0)) return FALSE;    // Word needs to display the result values for the fields in header/footer section

    if (!BeginRtfGroup(w,rtf)) return FALSE;         // begin fldinst group
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"fldinst",PARAM_NONE,0)) return FALSE;   // field display
 
		//pString="MERGEFIELD ";
 //MAK wir übergeben das und es ist flexibler	
		pString="";
    WriteRtfText(w,rtf,pString,lstrlen(pString));                    // close the equation
    
    rtf->flags|=ROFLAG_IN_FIELD_NAME;

    if (CurFont>=0) {     // update the rtf group
       struct StrRtfOutGroup far *pGroup=&(rtf->group[rtf->GroupLevel]);
       pGroup->style=TerFont[CurFont].style;        // don't update the font id because the font attributes are not written yet
       pGroup->FieldId=TerFont[CurFont].FieldId;
       pGroup->FieldCode=TerFont[CurFont].FieldCode;
    } 

    return TRUE;
}

/*****************************************************************************
    WriteRtfParaFmt:
    Write the paragraph formatting info when the attributes change from
    the previsous paragraph selection
******************************************************************************/
BOOL WriteRtfParaFmt(PTERWND w,struct StrRtfOut far *rtf,int NewPfmt,int PrevPfmt, int NewCell, int PrevCell, int NewFID, int PrevFID, long line)
{
    int i,CurLeftIndent,PrevLeftIndent=0;
    int CurRightIndent,PrevRightIndent=0;
    int CurFirstIndent,PrevFirstIndent=0;
    int CurTabId,PrevTabId=0;
    int CurBltId,PrevBltId=0;
    UINT CurFlags,PrevFlags=0,NoValueFlags,NoValuePflags;
    int CurCellId,PrevCellId=0;
    int CurRowId,PrevRowId=0;
    int CurShading,PrevShading=0;
    int CurParaFID,PrevParaFID=0;
    int CurSpaceBefore,PrevSpaceBefore=0;
    int CurSpaceAfter,PrevSpaceAfter=0;
    int CurSpaceBetween,PrevSpaceBetween=0;
    int CurLineSpacing,PrevLineSpacing=0;
    int CurParaSID,PrevParaSID=0;
    int CurPflags,PrevPflags=0;
    int CurFlow,PrevFlow=FLOW_DEF;
    int CurLevel,PrevLevel=0;
    COLORREF CurBkColor,PrevBkColor=CLR_WHITE;
    COLORREF CurBorderColor,PrevBorderColor=CLR_AUTO;
    BOOL result,reset=FALSE,ResetFont=FALSE;
    BOOL PrevSectBreak;

    if (PrevPfmt>=TotalPfmts || NewPfmt>=TotalPfmts) return TRUE;
    if (NewPfmt<0) return TRUE;

    // extract value for comparison
    if (PrevPfmt>0) {   // defautl pfmt (value 0) can have non-default values when the 'normal' style is changed and then applied
       PrevLeftIndent=PfmtId[PrevPfmt].LeftIndentTwips;
       PrevRightIndent=PfmtId[PrevPfmt].RightIndentTwips;
       PrevFirstIndent=PfmtId[PrevPfmt].FirstIndentTwips;
       PrevFlags=PfmtId[PrevPfmt].flags;
       if (PrevFID && PrevCell) ResetUintFlag(PrevFlags,PARA_BOX);
       ResetUintFlag(PrevFlags,PAGE_HDR_FTR);
       PrevTabId=PfmtId[PrevPfmt].TabId;
       PrevBltId=PfmtId[PrevPfmt].BltId;
       PrevShading=PfmtId[PrevPfmt].shading;
       PrevParaSID=PfmtId[PrevPfmt].StyId;
       PrevSpaceBefore=PfmtId[PrevPfmt].SpaceBefore;
       PrevSpaceAfter=PfmtId[PrevPfmt].SpaceAfter;
       PrevSpaceBetween=PfmtId[PrevPfmt].SpaceBetween;
       PrevLineSpacing=PfmtId[PrevPfmt].LineSpacing;
       PrevBkColor=PfmtId[PrevPfmt].BkColor;
       PrevBorderColor=PfmtId[PrevPfmt].BorderColor;
       PrevFlow=PfmtId[PrevPfmt].flow;
       PrevPflags=PfmtId[PrevPfmt].pflags&PFLAG_STYLES; // consider only the user styles
    }
    PrevCellId=PrevCell;
    PrevRowId=cell[PrevCellId].row;
    PrevLevel=cell[PrevCell].level;
    PrevParaFID=PrevFID;

    CurLeftIndent=PfmtId[NewPfmt].LeftIndentTwips;
    CurRightIndent=PfmtId[NewPfmt].RightIndentTwips;
    CurFirstIndent=PfmtId[NewPfmt].FirstIndentTwips;
    CurFlags=PfmtId[NewPfmt].flags;
    if (NewFID && NewCell) ResetUintFlag(CurFlags,PARA_BOX);

    ResetUintFlag(CurFlags,PAGE_HDR_FTR);
    CurTabId=PfmtId[NewPfmt].TabId;
    CurBltId=PfmtId[NewPfmt].BltId;
    CurCellId=NewCell;
    CurRowId=cell[CurCellId].row;
    CurShading=PfmtId[NewPfmt].shading;
    CurParaFID=NewFID;
    CurParaSID=PfmtId[NewPfmt].StyId;
    CurSpaceBefore=PfmtId[NewPfmt].SpaceBefore;
    CurSpaceAfter=PfmtId[NewPfmt].SpaceAfter;
    CurSpaceBetween=PfmtId[NewPfmt].SpaceBetween;
    CurLineSpacing=PfmtId[NewPfmt].LineSpacing;
    CurBkColor=PfmtId[NewPfmt].BkColor;
    CurBorderColor=PfmtId[NewPfmt].BorderColor;
    CurFlow=PfmtId[NewPfmt].flow;
    CurPflags=PfmtId[NewPfmt].pflags&PFLAG_STYLES;

    CurLevel=cell[NewCell].level;


    if (CurLevel!=PrevLevel) FlushRtfLine(w,rtf);             // Create line berak for visual aid

    // terminate the previous para frame text box
    if (PrevParaFID>0 && PrevParaFID!=CurParaFID && ParaFrame[PrevParaFID].flags&PARA_FRAME_TEXT_BOX) {
       if (!WritePfObjectTail(w,rtf,PrevFID)) return FALSE;
    }

    // check if para frame specification need to be written
    if ((CurParaFID!=PrevParaFID/* || reset*/) && CurParaFID>0) {    // commented because new frame should not begin simply because the paragraph properties changed
       if (ParaFrame[CurParaFID].flags&PARA_FRAME_OBJECT) 
            WriteRtfDoInfo(w,rtf,CurParaFID,CurFlags,NewFID);
       else WriteRtfFrameInfo(w,rtf,CurParaFID);
       
       // reset previous variable to force write any changes
       PrevLeftIndent=PrevRightIndent=PrevFirstIndent=PrevTabId=PrevBltId=0;
       PrevSpaceBefore=PrevSpaceAfter=PrevSpaceBetween=PrevLineSpacing=0;
       PrevFlags=PrevPflags=0;
       PrevParaSID=0;
       PrevBkColor=CLR_WHITE;
       PrevBorderColor=CLR_AUTO;
       PrevFlow=FLOW_DEF;
       reset=TRUE;                                   // paragraph reset
    }

    // check for table row or cell definition
    if (CurRowId!=PrevRowId) {
       int CurParentCell=cell[NewCell].ParentCell;
       int CurParentRow=cell[CurParentCell].row;
       if (PrevLevel>RtfInitLevel && PrevRowId!=CurParentRow) {
          if (line>0 && LineInfo(w,line-1,INFO_ROW)) WriteRtfRow(w,rtf,0,PrevCell,PrevLevel);  // close the previous nested row
       }
       
       if (CurLevel==RtfInitLevel && CurRowId>0) {
          WriteRtfRow(w,rtf,NewCell,0,PrevLevel);  // start next root level row
          RtfInTable=TRUE;
       }
    }

    // check for default para
    if ( CurLeftIndent==0 && CurRightIndent==0 && CurFirstIndent==0
      && CurFlags==0 && CurTabId==0 && CurCellId==0
      && CurShading==0 && CurParaFID==0 && PrevParaFID==0
      && CurSpaceBefore==0 && CurSpaceAfter==0 && CurSpaceBetween==0
      && CurLineSpacing==0 && CurParaSID==0 && CurFlow==FLOW_DEF && CurPflags==0) {
       return WriteRtfControl(w,rtf,"pard",PARAM_NONE,0);
    }

    // do we need to reset the paragraph?
    NoValueFlags=PARA_KEEP|PARA_KEEP_NEXT|PARA_BOX_ATTRIB|BULLET;  // para flags which do not have turn on/off values
    NoValuePflags=(PFLAG_WIDOW|PFLAG_NO_WRAP|PFLAG_PAGE_BREAK);

    PrevSectBreak=(line>0 && line<TotalLines && tabw(line-1) && True(tabw(line-1)->type&INFO_SECT));

    if (  CurCellId!=PrevCellId
       || CurTabId!=PrevTabId
       || CurBltId!=PrevBltId
       || CurParaFID!=PrevParaFID
       || CurParaSID!=PrevParaSID
       || (PrevFlow!=FLOW_DEF && CurFlow==FLOW_DEF)
       || (CurFlags&NoValueFlags)!=(PrevFlags&NoValueFlags)
       || (CurPflags&NoValuePflags)!=(PrevPflags&NoValuePflags) 
       || PrevSectBreak) {  // reset para id

       WriteRtfControl(w,rtf,"pard",PARAM_NONE,0);   // reset para information as tab stops are not incremental
       if (!(ParaFrame[CurParaFID].flags&PARA_FRAME_OBJECT)) WriteRtfFrameInfo(w,rtf,CurParaFID);          // write any frame information

       if (CurCellId>0 && cell[CurCellId].level>=RtfInitLevel) {
          WriteRtfControl(w,rtf,"intbl",PARAM_NONE,0);   // indicate that we are in a table now
          if (CurLevel>RtfInitLevel) WriteRtfControl(w,rtf,"itap",PARAM_INT,CurLevel-RtfInitLevel+1);   // indicate that we are in a table now
       }

       // reset previous variable to force write any changes
       PrevLeftIndent=PrevRightIndent=PrevFirstIndent=PrevTabId=PrevBltId=0;
       PrevSpaceBefore=PrevSpaceAfter=PrevSpaceBetween=PrevLineSpacing=0;
       PrevFlags=PrevPflags=0;
       PrevParaSID=PrevShading=0;
       PrevFlow=FLOW_DEF;
       PrevBkColor=CLR_WHITE;
       PrevBorderColor=CLR_AUTO;
       reset=TRUE;                                   // paragraph reset
    }

    // check if style id needs to be written
    if (CurParaSID!=PrevParaSID || reset) {
       BOOL WriteSS=TRUE;
       if ((TerFlags2&TFLAG2_COMP_WPF) && CurCellId>0) WriteSS=FALSE;
       if (WriteSS && !WriteRtfControl(w,rtf,"s",PARAM_INT,CurParaSID)) return FALSE;
    }


    // check if tab id needs to be written
    if (CurTabId!=PrevTabId || reset) {
       // write tab position
       if (!WriteRtfTab(w,rtf,CurTabId)) return FALSE;

       // force writing of other parameters
       PrevFirstIndent=PrevLeftIndent=PrevRightIndent=PrevBltId=0;
       PrevSpaceBefore=PrevSpaceAfter=PrevSpaceBetween=PrevLineSpacing=0;
       PrevFlags=PrevFlags=PrevFlow=0;
       PrevBkColor=CLR_WHITE;
       PrevBorderColor=CLR_AUTO;
    }

    // write any paragraph bullets
    if (CurFlags&BULLET && (reset || !(PrevFlags&BULLET))) {
       if (!WriteRtfBullet(w,rtf,CurBltId)) return FALSE;
    }

    // check if para border specification need to be written
    if (False(CurFlags&(PARA_BOX|PARA_BOX_THICK|PARA_BOX_DOUBLE))) CurBorderColor=CLR_AUTO;
    if (False(PrevFlags&(PARA_BOX|PARA_BOX_THICK|PARA_BOX_DOUBLE))) PrevBorderColor=CLR_AUTO;
    if (reset || (CurFlags&(PARA_BOX|PARA_BOX_THICK|PARA_BOX_DOUBLE))!=(PrevFlags&(PARA_BOX|PARA_BOX_THICK|PARA_BOX_DOUBLE)) 
              || CurBorderColor!=PrevBorderColor) {

       if (False(TerFlags5&TFLAG5_FRAME_TEXT_ONLY) && !WriteRtfParaBorder(w,rtf,CurFlags,CurBorderColor,CurParaFID)) return FALSE;   // if frame is suppressed, suppress the box also

       // force writing of other parameters
       PrevFirstIndent=PrevLeftIndent=PrevRightIndent=0;
       PrevFlags=PrevPflags=PrevFlow=0;
    }

    // compare and write indents
    if (CurFirstIndent!=PrevFirstIndent) {
       if (!WriteRtfControl(w,rtf,"fi",PARAM_INT,CurFirstIndent)) return FALSE;
    }
    if (CurLeftIndent!=PrevLeftIndent) {
       if (!WriteRtfControl(w,rtf,"li",PARAM_INT,CurLeftIndent)) return FALSE;
    }
    if (CurRightIndent!=PrevRightIndent) {
       if (!WriteRtfControl(w,rtf,"ri",PARAM_INT,CurRightIndent)) return FALSE;
    }
    if (CurSpaceBefore!=PrevSpaceBefore) {
       if (!WriteRtfControl(w,rtf,"sb",PARAM_INT,CurSpaceBefore)) return FALSE;
    }
    if (CurSpaceAfter!=PrevSpaceAfter) {
       if (!WriteRtfControl(w,rtf,"sa",PARAM_INT,CurSpaceAfter)) return FALSE;
    }
    
    if (CurLineSpacing!=PrevLineSpacing) {
       if (!WriteRtfControl(w,rtf,"sl",PARAM_INT,MulDiv(CurLineSpacing,240,100)+240)) return FALSE;
       if (!WriteRtfControl(w,rtf,"slmult",PARAM_INT,1)) return FALSE;
    }
    else if (CurSpaceBetween!=PrevSpaceBetween) {
       if (!WriteRtfControl(w,rtf,"sl",PARAM_INT,CurSpaceBetween)) return FALSE;
    }

    // compare and write justification
    if (CurFlags&RIGHT_JUSTIFY){
       if (!(PrevFlags&RIGHT_JUSTIFY)) {
          result=WriteRtfControl(w,rtf,"qr",PARAM_NONE,0);
          if (!result) return FALSE;
       }
    }
    else if (CurFlags&CENTER){
       if (!(PrevFlags&CENTER)) {
          result=WriteRtfControl(w,rtf,"qc",PARAM_NONE,0);
          if (!result) return FALSE;
       }
    }
    else if (CurFlags&JUSTIFY){
       if (!(PrevFlags&JUSTIFY)) {
          result=WriteRtfControl(w,rtf,"qj",PARAM_NONE,0);
          if (!result) return FALSE;
       }
    }
    else if (CurFlags&LEFT || !(CurFlags&(RIGHT_JUSTIFY|CENTER|JUSTIFY)) ){
       if (!(PrevFlags&LEFT) || PrevFlags&(RIGHT_JUSTIFY|CENTER|JUSTIFY) ) {
          result=WriteRtfControl(w,rtf,"ql",PARAM_NONE,0);
          if (!result) return FALSE;
       }
    }

    // compare and write double spacing
    if ((CurFlags&DOUBLE_SPACE)!=(PrevFlags&DOUBLE_SPACE) 
        && CurSpaceBetween==PrevSpaceBetween && CurLineSpacing==PrevLineSpacing) {
       if (CurFlags&DOUBLE_SPACE)   {
           result=WriteRtfControl(w,rtf,"sl",PARAM_INT,480);
           result=WriteRtfControl(w,rtf,"slmult",PARAM_INT,1);
       }
       else                         result=WriteRtfControl(w,rtf,"sl",PARAM_INT,0);
       if (!result) return FALSE;
    }

    // compare and write para keep together/next controls
    if ((CurFlags&PARA_KEEP) && !(PrevFlags&PARA_KEEP)) {
        result=WriteRtfControl(w,rtf,"keep",PARAM_NONE,0);
    }
    if ((CurFlags&PARA_KEEP_NEXT) && !(PrevFlags&PARA_KEEP_NEXT)) {
        result=WriteRtfControl(w,rtf,"keepn",PARAM_NONE,0);
    }
    if ((CurPflags&PFLAG_WIDOW) && !(PrevPflags&PFLAG_WIDOW)) {
        result=WriteRtfControl(w,rtf,"widctlpar",PARAM_NONE,0);
    }
    if ((CurPflags&PFLAG_NO_WRAP) && !(PrevPflags&PFLAG_NO_WRAP)) {
        result=WriteRtfControl(w,rtf,"ssparnw",PARAM_NONE,0);         // paragraph no wrap
    }
    if ((CurPflags&PFLAG_PAGE_BREAK) && !(PrevPflags&PFLAG_PAGE_BREAK)) {
        result=WriteRtfControl(w,rtf,"pagebb",PARAM_NONE,0);         // paragraph no wrap
    }

    // compare and write shading and background color
    if (CurShading!=PrevShading) {
       if (!WriteRtfControl(w,rtf,"shading",PARAM_INT,CurShading)) return FALSE;
       ResetFont=TRUE;       // MSWord requires 'plain' keyword for this feature to work
    }
    if (CurBkColor!=PrevBkColor) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==CurBkColor) break;
       if (i==rtf->TotalColors) i=0;
       if (!WriteRtfControl(w,rtf,"cbpat",PARAM_INT,i)) return FALSE;        // write the color index
    }

    if (CurFlow!=PrevFlow) {
       if (CurFlow==FLOW_RTL && !WriteRtfControl(w,rtf,"rtlpar",PARAM_NONE,0L)) return FALSE;        // write the color index
       if (CurFlow==FLOW_LTR && !WriteRtfControl(w,rtf,"ltrpar",PARAM_NONE,0L)) return FALSE;        // write the color index
    }

    if (ResetFont) ResetRtfFont(w,rtf);

    return TRUE;
}

/*****************************************************************************
    WriteRtfDoInfo:
    Write the current drawing object information
******************************************************************************/
BOOL WriteRtfDoInfo(PTERWND w,struct StrRtfOut far * rtf,int CurParaFID, UINT ParaFlags, int NewFID)
{
    int ZOrder;

    // write applicable para info line and rectangle object here because they don't actually have any text
    if (ParaFrame[CurParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT)) {
       if (ParaFlags&PARA_KEEP_NEXT) WriteRtfControl(w,rtf,"keepn",PARAM_NONE,0);
    } 
          
    FlushRtfLine(w,rtf);             // Create line berak for visual aid
    
    if (TerFlags5&TFLAG5_WRITE_DOB) {      
      // write object frame header
      if (!BeginRtfGroup(w,rtf)) return FALSE;   // begin 'do' group
      if (!WriteRtfControl(w,rtf,"do",PARAM_NONE,0)) return FALSE;
      if (!(ParaFrame[CurParaFID].flags&PARA_FRAME_HPAGE) && !WriteRtfControl(w,rtf,"dobxmargin",PARAM_NONE,0)) return FALSE;
      if ((ParaFrame[CurParaFID].flags&PARA_FRAME_HPAGE) && !WriteRtfControl(w,rtf,"dobxpage",PARAM_NONE,0)) return FALSE;
      if (!(ParaFrame[CurParaFID].flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)) && !WriteRtfControl(w,rtf,"dobypara",PARAM_NONE,0)) return FALSE;
      if ((ParaFrame[CurParaFID].flags&PARA_FRAME_VMARG) && !WriteRtfControl(w,rtf,"dobymargin",PARAM_NONE,0)) return FALSE;
      if ((ParaFrame[CurParaFID].flags&PARA_FRAME_VPAGE) && !WriteRtfControl(w,rtf,"dobypage",PARAM_NONE,0)) return FALSE;
    
      ZOrder=ParaFrame[CurParaFID].ZOrder;
      if (ZOrder<0) ZOrder=0;   // MSWord does not like negative Zorder 
      if (!WriteRtfControl(w,rtf,"dodhgt",PARAM_INT,ZOrder)) return FALSE;     // z order

      // write the frame object type
      if (ParaFrame[CurParaFID].flags&PARA_FRAME_TEXT_BOX) {
         if (!WriteRtfControl(w,rtf,"dptxbx",PARAM_NONE,0)) return FALSE;  // text box
         if (!WriteRtfControl(w,rtf,"dptxbxmar",PARAM_INT,ParaFrame[CurParaFID].margin)) return FALSE;  // text box
         if (ParaFrame[CurParaFID].TextAngle==90 && !WriteRtfControl(w,rtf,"dptxbtlr",PARAM_NONE,0)) return FALSE;
         if (ParaFrame[CurParaFID].TextAngle==270 && !WriteRtfControl(w,rtf,"dptxtbrl",PARAM_NONE,0)) return FALSE;
         if (!BeginRtfGroup(w,rtf)) return FALSE;   // begin the text group
         if (!WriteRtfControl(w,rtf,"dptxbxtext",PARAM_NONE,0)) return FALSE;  // text box
      }
      else if (ParaFrame[CurParaFID].flags&PARA_FRAME_LINE) {
         if (!WriteRtfControl(w,rtf,"dpline",PARAM_NONE,0)) return FALSE;  // text box
         WritePfObjectTail(w,rtf,NewFID);
      }
      else if (ParaFrame[CurParaFID].flags&PARA_FRAME_RECT) {
         if (!WriteRtfControl(w,rtf,"dprect",PARAM_NONE,0)) return FALSE;  // text box
         WritePfObjectTail(w,rtf,NewFID);
      }
    }
    else {                   // write the drawing object as 
      struct StrParaFrame far *pFrame=&(ParaFrame[CurParaFID]);
      int ShapeType;
      BYTE string[50];

      WriteRtfControl(w,rtf,"pard",PARAM_NONE,0);     // terminate any previous frames
      FlushRtfLine(w,rtf);                            // Create line berak for visual aid

      // write the shape group
      if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shp group
      if (!WriteRtfControl(w,rtf,"shp",PARAM_NONE,0)) return FALSE;

      if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shpinst destination group
      if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
      if (!WriteRtfControl(w,rtf,"shpinst",PARAM_NONE,0)) return FALSE;

      if (!WriteRtfControl(w,rtf,"shpleft",PARAM_INT,pFrame->x)) return FALSE;
      if (!WriteRtfControl(w,rtf,"shpright",PARAM_INT,pFrame->x+pFrame->width)) return FALSE;
      if (!WriteRtfControl(w,rtf,"shptop",PARAM_INT,pFrame->ParaY)) return FALSE;
      if (!WriteRtfControl(w,rtf,"shpbottom",PARAM_INT,pFrame->ParaY+pFrame->height)) return FALSE;

      if (!WriteRtfControl(w,rtf,"shpbxmarg",PARAM_NONE,0)) return FALSE;
      if (pFrame->flags&PARA_FRAME_VMARG && !WriteRtfControl(w,rtf,"shpbymargin",PARAM_NONE,0)) return FALSE;
      if (pFrame->flags&PARA_FRAME_VPAGE && !WriteRtfControl(w,rtf,"shpbypage",PARAM_NONE,0)) return FALSE;
      if (!(pFrame->flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)) && !WriteRtfControl(w,rtf,"shpbypara",PARAM_NONE,0)) return FALSE;

      if (pFrame->flags&PARA_FRAME_NO_WRAP && !WriteRtfControl(w,rtf,"shpwr",PARAM_INT,SWRAP_NO_WRAP)) return FALSE;
      if (pFrame->flags&PARA_FRAME_WRAP_THRU && !WriteRtfControl(w,rtf,"shpwr",PARAM_INT,SWRAP_IGNORE)) return FALSE;
      if (!(pFrame->flags&(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU))) {
         if (!WriteRtfControl(w,rtf,"shpwr",PARAM_INT,SWRAP_AROUND)) return FALSE;
         if (!WriteRtfControl(w,rtf,"shpwrk",PARAM_INT,WRAPSIDE_BOTH)) return FALSE;
      }
      
      if (pFrame->ZOrder<0) {
         if (!WriteRtfShapeProp(w,rtf,"fBehindDocument","1")) return FALSE;   // 14 means linked image
      }
      else if (!WriteRtfControl(w,rtf,"shpz",PARAM_INT,(pFrame->ZOrder<0?0:pFrame->ZOrder))) return FALSE;   // Word does not support negative Zorder

      if      (pFrame->flags&PARA_FRAME_LINE) ShapeType=SHPTYPE_LINE;
      else if (pFrame->flags&PARA_FRAME_RECT) ShapeType=SHPTYPE_RECT;
      else                                    ShapeType=SHPTYPE_TEXT_BOX;
      wsprintf(string,"%d",ShapeType);
      if (!WriteRtfShapeProp(w,rtf,"shapeType",string)) return FALSE;

      // write the attributes
      if (ShapeType==SHPTYPE_LINE) {
         if (pFrame->flags&PARA_FRAME_NO_LINE) {
           if (!WriteRtfShapeProp(w,rtf,"fLine","0")) return FALSE;
         }
			// MAK:
				 if (pFrame->LineType==DOL_DIAG)
				 {
					 if (!WriteRtfShapeProp(w,rtf,"fFlipH","0")) return FALSE;
					 if (!WriteRtfShapeProp(w,rtf,"fFlipV","1")) return FALSE;
					 
				 }
      } 
      else {
         wsprintf(string,"%d",((pFrame->flags&PARA_FRAME_BOXED) && pFrame->LineWdth>0)?1:0);
         if (!WriteRtfShapeProp(w,rtf,"fLine",string)) return FALSE;
      }
       
      if (ShapeType==SHPTYPE_LINE || pFrame->flags&PARA_FRAME_BOXED) {
          wsprintf(string,"%d",TwipsToEmu(pFrame->LineWdth));
          if (!WriteRtfShapeProp(w,rtf,"lineWidth",string)) return FALSE;

          wsprintf(string,"%d",pFrame->LineColor);
          if (!WriteRtfShapeProp(w,rtf,"lineColor",string)) return FALSE;

          if (pFrame->flags&PARA_FRAME_DOTTED) {
             wsprintf(string,"%d",6);
             if (!WriteRtfShapeProp(w,rtf,"lineDashing",string)) return FALSE;
          } 
      }
      
      if (ShapeType==SHPTYPE_RECT || ShapeType==SHPTYPE_TEXT_BOX) {
          wsprintf(string,"%d",ParaFrame[CurParaFID].FillPattern==0?0:1);
          if (!WriteRtfShapeProp(w,rtf,"fFilled",string)) return FALSE;
          
          if (ParaFrame[CurParaFID].FillPattern>0) {
             wsprintf(string,"%d",pFrame->BackColor);
             if (!WriteRtfShapeProp(w,rtf,"fillColor",string)) return FALSE;
          }
      }  

      // write the ole object data if any
      if (ShapeType==SHPTYPE_TEXT_BOX) {
         if (ParaFrame[CurParaFID].TextAngle!=0) {
            wsprintf(string,"%d",ParaFrame[CurParaFID].TextAngle==90?TEXTFLOW_B2T:TEXTFLOW_T2B);
            if (!WriteRtfShapeProp(w,rtf,"txflTextFlow",string)) return FALSE;
         }   
         
         wsprintf(string,"%d",TwipsToEmu(pFrame->margin));
         if (!WriteRtfShapeProp(w,rtf,"dxTextLeft",string)) return FALSE;
         if (!WriteRtfShapeProp(w,rtf,"dxTextRight",string)) return FALSE;
         if (!WriteRtfShapeProp(w,rtf,"dyTextTop",string)) return FALSE;
         if (!WriteRtfShapeProp(w,rtf,"dyTextBottom",string)) return FALSE;

         if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin shptxt group
         if (!WriteRtfControl(w,rtf,"shptxt",PARAM_NONE,0)) return FALSE;
         rtf->flags|=ROFLAG_WRITE_FONT;                // write full font information

      }
      else {             // terminate the line and rectangle objects
         WritePfObjectTail(w,rtf,NewFID);
      } 
    
    }
     
    return TRUE;
}

/*****************************************************************************
    WriteRtfFrameInfo:
    Write the current frame information
******************************************************************************/
BOOL WriteRtfFrameInfo(PTERWND w,struct StrRtfOut far * rtf,int CurParaFID)
{
    int height;

    if (CurParaFID==0) return TRUE;

    // write the frame information
    if      ((ParaFrame[CurParaFID].flags&PARA_FRAME_VPAGE) && !WriteRtfControl(w,rtf,"pvpg",PARAM_NONE,0)) return FALSE;
    else if ((ParaFrame[CurParaFID].flags&PARA_FRAME_VMARG) && !WriteRtfControl(w,rtf,"pvmrg",PARAM_NONE,0)) return FALSE;
    else if (!(ParaFrame[CurParaFID].flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)) && !WriteRtfControl(w,rtf,"pvpara",PARAM_NONE,0)) return FALSE;

    if (!(ParaFrame[CurParaFID].flags&PARA_FRAME_HPAGE) && !WriteRtfControl(w,rtf,"phmrg",PARAM_NONE,0)) return FALSE;
    //if ((ParaFrame[CurParaFID].flags&PARA_FRAME_HPAGE) && !WriteRtfControl(w,rtf,"phpg",PARAM_NONE,0)) return FALSE;  // x internally always translated relative to the margin
    if (!WriteRtfControl(w,rtf,"posx",PARAM_INT,ParaFrame[CurParaFID].x)) return FALSE;
    if (!WriteRtfControl(w,rtf,"posy",PARAM_INT,ParaFrame[CurParaFID].ParaY)) return FALSE;
    
    height=ParaFrame[CurParaFID].MinHeight;
    if (True(ParaFrame[CurParaFID].flags&PARA_FRAME_FIXED_HEIGHT)) height=-height;
    if (!WriteRtfControl(w,rtf,"absh",PARAM_INT,height)) return FALSE;
    
    if (!WriteRtfControl(w,rtf,"absw",PARAM_INT,ParaFrame[CurParaFID].width)) return FALSE;
    if (!WriteRtfControl(w,rtf,"dxfrtext",PARAM_INT,ParaFrame[CurParaFID].DistFromText)) return FALSE;
    if (!WriteRtfControl(w,rtf,"dfrmtxtx",PARAM_INT,ParaFrame[CurParaFID].DistFromText)) return FALSE;
    if (!WriteRtfControl(w,rtf,"dfrmtxty",PARAM_INT,0)) return FALSE;
    if ((ParaFrame[CurParaFID].flags&PARA_FRAME_NO_WRAP) && !WriteRtfControl(w,rtf,"nowrap",PARAM_NONE,0)) return FALSE;
    if (ParaFrame[CurParaFID].TextAngle==90 && !WriteRtfControl(w,rtf,"frmtxbtlr",PARAM_NONE,0)) return FALSE;
    if (ParaFrame[CurParaFID].TextAngle==270 && !WriteRtfControl(w,rtf,"frmtxtblr",PARAM_NONE,0)) return FALSE;
 
    return TRUE;
}
 
/*****************************************************************************
    WriteRtfBullet:
    Write the para graph bullet group
******************************************************************************/
BOOL WriteRtfBullet(PTERWND w,struct StrRtfOut far * rtf,int BltId)
{
    int i,FontId;
    BYTE typeface[32];
    BOOL hidden=FALSE;
    BOOL WriteListInfo=FALSE;

    BOOL IsBullet=TerBlt[BltId].IsBullet;

    // check if it is a bullet
    if (!IsBullet && TerBlt[BltId].ls>0) {
       struct StrListLevel far *pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,TerBlt[BltId].lvl);
       IsBullet=(pLevel->NumType==LIST_BLT);
    } 

    // write the pntext group
    if (IsBullet && False(rtf->flags&ROFLAG_IN_STYLESHEET)) {
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group

       if (!WriteRtfControl(w,rtf,"pntext",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"pard",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"plain",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"f",PARAM_INT,1)) return FALSE;
       if (!WriteRtfControl(w,rtf,"fs",PARAM_INT,20)) return FALSE;

       if (!PutRtfChar(w,rtf,'\\')) return FALSE;  // write \'b7
       if (!WriteRtfText(w,rtf,"'b7",3))  return FALSE;

       if (!WriteRtfControl(w,rtf,"tab",PARAM_NONE,20)) return FALSE;

       if (!EndRtfGroup(w,rtf)) return FALSE;        // end group
    }

    // write the pn group
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group

    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"pn",PARAM_NONE,0)) return FALSE;

    if (TerBlt[BltId].flags&BLTFLAG_HIDDEN) {
       if (!WriteRtfControl(w,rtf,"pnlvlcont",PARAM_NONE,0)) return FALSE;
       hidden=TRUE;
    }

    if (IsBullet) {                // bullet
       if (!hidden && !WriteRtfControl(w,rtf,"pnlvlblt",PARAM_NONE,0)) return FALSE;
    }
    else {                                        // numbering
       if (TerBlt[BltId].level==0 || TerBlt[BltId].level==10) {
          if (!hidden && !WriteRtfControl(w,rtf,"pnlvlbody",PARAM_NONE,0)) return FALSE;
       }
       else {
          if (!hidden && !WriteRtfControl(w,rtf,"pnlvl",PARAM_INT,TerBlt[BltId].level)) return FALSE;
       }
       if (!WriteRtfControl(w,rtf,"pnstart",PARAM_INT,(TerBlt[BltId].start==0)?1:TerBlt[BltId].start)) return FALSE;

       // write the number type
       if (TerBlt[BltId].NumberType==NBR_UPR_ALPHA && !WriteRtfControl(w,rtf,"pnucltr",PARAM_NONE,0L)) return FALSE;
       else if (TerBlt[BltId].NumberType==NBR_LWR_ALPHA && !WriteRtfControl(w,rtf,"pnlcltr",PARAM_NONE,0L)) return FALSE;
       else if (TerBlt[BltId].NumberType==NBR_UPR_ROMAN && !WriteRtfControl(w,rtf,"pnucrm",PARAM_NONE,0L)) return FALSE;
       else if (TerBlt[BltId].NumberType==NBR_LWR_ROMAN && !WriteRtfControl(w,rtf,"pnlcrm",PARAM_NONE,0L)) return FALSE;
       else if (TerBlt[BltId].NumberType==NBR_DEC && !WriteRtfControl(w,rtf,"pndec",PARAM_NONE,0L)) return FALSE;

    }

    // write the list number and list level
    if (TotalListOr>1) {
       int ls=TerBlt[BltId].ls;
       if (!WriteRtfControl(w,rtf,"ls",PARAM_INT,rtf->XlateLs[ls])) return FALSE;
       if (!WriteRtfControl(w,rtf,"ilvl",PARAM_INT,TerBlt[BltId].lvl)) return FALSE;
       WriteListInfo=TRUE;
    } 

    if (!WriteRtfControl(w,rtf,"pnhang",PARAM_NONE,0)) return FALSE;

    if (IsBullet) {              // bullet
       // write the font number
       BYTE BulletChar;
       FontId=0;
       if      (TerBlt[BltId].font==BFONT_SYMBOL)    lstrcpy(typeface,"Symbol");
       else if (TerBlt[BltId].font==BFONT_WINGDINGS) lstrcpy(typeface,"Wingdings");
       for (i=0;i<TotalFonts;i++) if (TerFont[i].InUse && !(TerFont[i].style&PICT) && lstrcmpi(TerFont[i].TypeFace,typeface)==0) break;
       if (i<TotalFonts) FontId=i;
       if (!WriteRtfControl(w,rtf,"pnf",PARAM_INT,TerFont[FontId].RtfIndex)) return FALSE;

       // write the pntxtb group
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group
       if (!WriteRtfControl(w,rtf,"pntxtb",PARAM_NONE,0)) return FALSE;
       BulletChar=(TerBlt[BltId].BulletChar==0)?0xb7:TerBlt[BltId].BulletChar;
       if (!PutRtfSpecChar(w,rtf,BulletChar))  return FALSE;
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end group
    }
    else {
       BYTE AftChar;

       // write the pntxta group
       if (lstrlen(TerBlt[BltId].BefText)>0) {
          if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group
          if (!WriteRtfControl(w,rtf,"pntxtb",PARAM_NONE,0)) return FALSE;

          rtf->WritingControl=TRUE;                     // disable line break
          WriteRtfText(w,rtf,TerBlt[BltId].BefText,lstrlen(TerBlt[BltId].BefText));                    // close the equation
          rtf->WritingControl=FALSE;

          if (!EndRtfGroup(w,rtf)) return FALSE;        // end group
       }
       // write the pntxtb group
       AftChar=TerBlt[BltId].AftChar;
       if (AftChar==0) AftChar='.';

       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin group
       if (!WriteRtfControl(w,rtf,"pntxta",PARAM_NONE,0)) return FALSE;
       if (!PutRtfSpecChar(w,rtf,AftChar))  return FALSE;
       if (!EndRtfGroup(w,rtf)) return FALSE;        // end group
    }

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end group

    // write the list information
    if (WriteListInfo) {
       int ls=TerBlt[BltId].ls;
       if (!WriteRtfControl(w,rtf,"ls",PARAM_INT,rtf->XlateLs[ls])) return FALSE;
       if (!WriteRtfControl(w,rtf,"ilvl",PARAM_INT,TerBlt[BltId].lvl)) return FALSE;
    } 

    return TRUE;
}

/*****************************************************************************
    WritePfObjectTail:
    Write the para frame object tail information.
******************************************************************************/
BOOL WritePfObjectTail(PTERWND w,struct StrRtfOut far * rtf, int ParaFID)
{
    char string[20];
    int height;

    if (ParaFID<=0) return TRUE;

    if (!(TerFlags5&TFLAG5_WRITE_DOB)) {  // written in shape object form
      if (ParaFrame[ParaFID].flags&PARA_FRAME_TEXT_BOX) {   // end the shptxt group
         if (!EndRtfGroup(w,rtf)) return FALSE;
      }
      
      if (!EndRtfGroup(w,rtf)) return FALSE;         // end the 'shpinst' group
      if (!EndRtfGroup(w,rtf)) return FALSE;         // end the 'shp' group
    
      FlushRtfLine(w,rtf);             // Create line berak for visual aid
      return TRUE;
    } 

    if (ParaFrame[ParaFID].flags&PARA_FRAME_TEXT_BOX) {   // end the dptxbxtext group
       if (!EndRtfGroup(w,rtf)) return FALSE;
    }

    // write the line vertex
    if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) {   // points are relative to dpx,dpy
       int x1=0,x2,y1=0,y2;
       x2=ParaFrame[ParaFID].width;
       y2=ParaFrame[ParaFID].height;
       if      (ParaFrame[ParaFID].LineType==DOL_HORZ) y2=y1;
       else if (ParaFrame[ParaFID].LineType==DOL_VERT) x2=x1;
       else if (ParaFrame[ParaFID].LineType==DOL_DIAG) {
          y1=ParaFrame[ParaFID].height;
          y2=0;
       }
       if (!WriteRtfControl(w,rtf,"dpptx",PARAM_INT,x1)) return FALSE;  // relative x1 position
       if (!WriteRtfControl(w,rtf,"dppty",PARAM_INT,y1)) return FALSE;  // relative y1 position
       if (!WriteRtfControl(w,rtf,"dpptx",PARAM_INT,x2)) return FALSE;  // relative x2 position
       if (!WriteRtfControl(w,rtf,"dppty",PARAM_INT,y2)) return FALSE;  // relative y2 position
    }


    if (!WriteRtfControl(w,rtf,"dpx",PARAM_INT,ParaFrame[ParaFID].x)) return FALSE;  // x position
    if (!WriteRtfControl(w,rtf,"dpy",PARAM_INT,ParaFrame[ParaFID].ParaY)) return FALSE;  // y position

    if (!WriteRtfControl(w,rtf,"dpxsize",PARAM_INT,ParaFrame[ParaFID].width)) return FALSE;  // width
    height=ParaFrame[ParaFID].MinHeight;
    if (height==0) height=ParaFrame[ParaFID].height;
    if (!WriteRtfControl(w,rtf,"dpysize",PARAM_INT,height)) return FALSE;  // height

    if (ParaFrame[ParaFID].flags&PARA_FRAME_BOXED) {  // write the border type
       if (ParaFrame[ParaFID].flags&PARA_FRAME_DOTTED)
             lstrcpy(string,"dplinedot");
       else  lstrcpy(string,"dplinesolid");
       if (!WriteRtfControl(w,rtf,string,PARAM_NONE,0)) return FALSE;
    }
    else if (!WriteRtfControl(w,rtf,"dplinehollow",PARAM_NONE,0)) return FALSE;

    if (!WriteRtfControl(w,rtf,"dplinecor",PARAM_INT,GetRValue(ParaFrame[ParaFID].LineColor))) return FALSE;  // line red
    if (!WriteRtfControl(w,rtf,"dplinecog",PARAM_INT,GetGValue(ParaFrame[ParaFID].LineColor))) return FALSE;  // line red
    if (!WriteRtfControl(w,rtf,"dplinecob",PARAM_INT,GetBValue(ParaFrame[ParaFID].LineColor))) return FALSE;  // line red
    if (!WriteRtfControl(w,rtf,"dplinew",PARAM_INT,ParaFrame[ParaFID].LineWdth)) return FALSE;  // solid line

    if (!WriteRtfControl(w,rtf,"dpfillbgcr",PARAM_INT,GetRValue(ParaFrame[ParaFID].BackColor))) return FALSE;  // background red
    if (!WriteRtfControl(w,rtf,"dpfillbgcg",PARAM_INT,GetGValue(ParaFrame[ParaFID].BackColor))) return FALSE;  // background red
    if (!WriteRtfControl(w,rtf,"dpfillbgcb",PARAM_INT,GetBValue(ParaFrame[ParaFID].BackColor))) return FALSE;  // background red
    if (!WriteRtfControl(w,rtf,"dpfillpat",PARAM_INT,ParaFrame[ParaFID].FillPattern)) return FALSE;  // fill pattern

    if (!EndRtfGroup(w,rtf)) return FALSE;         // end the 'do' group
    
    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    return TRUE;
}

/*****************************************************************************
    WriteRtfRow:
    Write the table row information to the file.  For InitLevel the function writes
    the row definition for the current row, otherwise it writes the function definition
    for the prevous row.
******************************************************************************/
BOOL WriteRtfRow(PTERWND w,struct StrRtfOut far *rtf, int NewCell, int PrevCell, int PrevLevel)
{
    BOOL result,CellFound=FALSE;
    int  i,CellId,PrevCellId,cl,level;
    int  CellX,SaveX,LastCell;
    int  row,CurRowId,CurCellId,PrevRowId;
    UINT CheckFlags;
    long l;
    BOOL WritePrevRow=FALSE;


    CurCellId=NewCell;
    CurRowId=cell[CurCellId].row;

    if (NewCell==0) WritePrevRow=TRUE;

    // start a new destination group for the nested PrevLevel
    if (WritePrevRow) {
       FlushRtfLine(w,rtf);             // flush the rtf line to the output
       
       if (!BeginRtfGroup(w,rtf)) return FALSE;    // start the group that enclosed nesttableprops and nonesttables groups
       if (!BeginRtfGroup(w,rtf)) return FALSE;    // start the nesttableprops group
       if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;         // destination group
       if (!WriteRtfControl(w,rtf,"nesttableprops",PARAM_NONE,0)) return FALSE;
       goto WRITE_ROW;
    } 

    // check if row formatting is identical to the previous row
    PrevRowId=TableRow[CurRowId].PrevRow;
    if ( PrevRowId<0                               // new table beginning
      || cell[PrevCell].level>RtfInitLevel         // descending to root level
      || rtf->flags&ROFLAG_WRITE_TROWD || rtf->TblHilight
      || TableRow[PrevRowId].flags!=TableRow[CurRowId].flags
      || TableRow[PrevRowId].CellMargin!=TableRow[CurRowId].CellMargin
      || TableRow[PrevRowId].indent!=TableRow[CurRowId].indent
      || TableRow[PrevRowId].AddedIndent!=TableRow[CurRowId].AddedIndent
      || TableRow[PrevRowId].MinHeight!=TableRow[CurRowId].MinHeight
      || TableRow[PrevRowId].FixWidth!=TableRow[CurRowId].FixWidth
      || TableRow[PrevRowId].border!=TableRow[CurRowId].border
      || TableRow[PrevRowId].flow!=TableRow[CurRowId].flow
      || TableRow[PrevRowId].id!=TableRow[CurRowId].id) goto WRITE_ROW;


    CellId=TableRow[CurRowId].FirstCell;        // now check for cell formatting
    PrevCellId=TableRow[PrevRowId].FirstCell;

    CheckFlags=CFLAG_ROW_SPANNED|CFLAG_VALIGN_CTR|CFLAG_VALIGN_BOT;

    while (CellId>0) {
       if (PrevCellId<0) goto WRITE_ROW;  // prev row had less number of cells

       if ( cell[PrevCellId].x!=cell[CellId].x
         || cell[PrevCellId].width!=cell[CellId].width
         || cell[PrevCellId].border!=cell[CellId].border
         || cell[PrevCellId].shading!=cell[CellId].shading
         || cell[PrevCellId].BackColor!=cell[CellId].BackColor
         || cell[PrevCellId].RowSpan!=cell[CellId].RowSpan
         || cell[PrevCellId].ColSpan!=cell[CellId].ColSpan
         || cell[PrevCellId].TextAngle!=cell[CellId].TextAngle
         || (cell[PrevCellId].flags&CheckFlags)!=(cell[CellId].flags&CheckFlags)
         ) goto WRITE_ROW;

         if (cell[CellId].border) {   // check if the border width are the same
           for (i=0;i<4;i++) {
              if (cell[PrevCellId].BorderWidth[i]!=cell[CellId].BorderWidth[i]) goto WRITE_ROW;
              if (cell[PrevCellId].BorderColor[i]!=cell[CellId].BorderColor[i]) goto WRITE_ROW;
           }
         }

       CellId=cell[CellId].NextCell;
       PrevCellId=cell[PrevCellId].NextCell;
    }
    if (PrevCellId>0) goto WRITE_ROW;   // prev row had additional cells
    goto END;


    WRITE_ROW:
    
    if (WritePrevRow) row=cell[PrevCell].row;   // write the ending row
    else              row=CurRowId;             // write the new row
      
    if (!WritePrevRow) FlushRtfLine(w,rtf);     // create a line break for visual aid

    if (!WriteRtfControl(w,rtf,"trowd",PARAM_NONE,0)) return FALSE;

    // write table data
    result=TRUE;
    if      (TableRow[row].flags&CENTER)        result=WriteRtfControl(w,rtf,"trqc",PARAM_NONE,0);
    else if (TableRow[row].flags&RIGHT_JUSTIFY) result=WriteRtfControl(w,rtf,"trqr",PARAM_NONE,0);
    if (!result) return result;

    if (!WriteRtfControl(w,rtf,"trgaph",PARAM_INT,TableRow[row].CellMargin)) return FALSE;
    
    // write subtable padding controls
    cl=TableRow[row].FirstCell;
    level=cell[cl].level;
    if (level>RtfInitLevel) {
       if (!WriteRtfControl(w,rtf,"trpaddl",PARAM_INT,TableRow[row].CellMargin)) return FALSE;
       if (!WriteRtfControl(w,rtf,"trpaddr",PARAM_INT,TableRow[row].CellMargin)) return FALSE;
       if (!WriteRtfControl(w,rtf,"trpaddfl",PARAM_INT,3)) return FALSE;
       if (!WriteRtfControl(w,rtf,"trpaddfr",PARAM_INT,3)) return FALSE;
    } 
    
    if (!WriteRtfControl(w,rtf,"trleft",PARAM_INT,TableRow[row].indent-TableRow[row].AddedIndent)) return FALSE;

    if (TableRow[row].MinHeight!=0 && !WriteRtfControl(w,rtf,"trrh",PARAM_INT,TableRow[row].MinHeight)) return FALSE;

    if (TableRow[row].flags&ROWFLAG_HDR && !WriteRtfControl(w,rtf,"trhdr",PARAM_NONE,0)) return FALSE;
    if (TableRow[row].flags&ROWFLAG_KEEP && !WriteRtfControl(w,rtf,"trkeep",PARAM_NONE,0)) return FALSE;

    if (TableRow[row].id!=0 && !WriteRtfControl(w,rtf,"sstrid",PARAM_INT,TableRow[row].id)) return FALSE;

    // write border
    if (TableRow[row].border&BORDER_TOP) {
       if (!WriteRtfControl(w,rtf,"trbrdrt",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrw",PARAM_INT,TableRow[row].BorderWidth[BORDER_INDEX_TOP])) return FALSE;
    }
    if (TableRow[row].border&BORDER_BOT) {
       if (!WriteRtfControl(w,rtf,"trbrdrb",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrw",PARAM_INT,TableRow[row].BorderWidth[BORDER_INDEX_BOT])) return FALSE;
    }
    if (TableRow[row].border&BORDER_LEFT) {
       if (!WriteRtfControl(w,rtf,"trbrdrl",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrw",PARAM_INT,TableRow[row].BorderWidth[BORDER_INDEX_LEFT])) return FALSE;
    }
    if (TableRow[row].border&BORDER_RIGHT) {
       if (!WriteRtfControl(w,rtf,"trbrdrr",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
       if (!WriteRtfControl(w,rtf,"brdrw",PARAM_INT,TableRow[row].BorderWidth[BORDER_INDEX_RIGHT])) return FALSE;
    }

    // write the text flow
    if (TableRow[row].flow==FLOW_RTL && !WriteRtfControl(w,rtf,"rtlrow",PARAM_NONE,0)) return FALSE;
    if (TableRow[row].flow==FLOW_LTR && !WriteRtfControl(w,rtf,"ltrrow",PARAM_NONE,0)) return FALSE;


    // determine the last cell to write
    LastCell=0;
    if (rtf->output>=RTF_CB && HilightType!=HILIGHT_OFF) {
        for (l=HilightEndRow;l>HilightBegRow;l--) {
           if (!(LineFlags(l)&LFLAG_SELECTED)) continue;
           if (cell[cid(l)].row==row) break;
        }
        LastCell=cid(l);
    }

    // write the cell information
    CellId=TableRow[row].FirstCell;
    CellX=TableRow[row].indent;
    while (CellId>0) {
       //if (!CellFound && CellId==CurCellId) CellFound=TRUE;
     
       if (!CellFound) {               // find first cell info to write
          if (rtf->TblHilight) CellFound=cell[CellId].flags&CFLAG_RTF_SEL;
          else CellFound=TRUE;         // write from the first cell;
       }    

       // update the cell x position
       if (CellFound) {
          if (rtf->TblHilight && !(cell[CellId].flags&CFLAG_RTF_SEL)) break;  // after the last column

          SaveX=cell[CellId].x;        // save the x value

          cell[CellId].x=CellX;
          CellX+=cell[CellId].width;
          WriteRtfCell(w,rtf,CellId);

          cell[CellId].x=SaveX;        // restore the x value
       }

       if (CellId==LastCell) break;

       CellId=cell[CellId].NextCell;
    }


    // end nesttableprops group
    if (WritePrevRow) {
       if (!WriteRtfControl(w,rtf,"nestrow",PARAM_NONE,0)) return FALSE;
       if (!EndRtfGroup(w,rtf)) return FALSE;       // end the nesttableprops group
    
       WriteRtfNoNestGroup(w,rtf);      // write nonesttables group
       
       if (!EndRtfGroup(w,rtf)) return FALSE;       // end the outer group
       FlushRtfLine(w,rtf);             // Create line berak for visual aid
    }
     
    rtf->flags=ResetUintFlag(rtf->flags,ROFLAG_WRITE_TROWD);

    END:
    if (PrevLevel==RtfInitLevel) RtfInTable=TRUE;   // to ensure that table is terminated properly.

    return TRUE;
}

/*****************************************************************************
    WriteRtfNoNestGroup:
    Write no nest tables group
******************************************************************************/
BOOL WriteRtfNoNestGroup(PTERWND w,struct StrRtfOut far *rtf)
{
    if (!BeginRtfGroup(w,rtf)) return FALSE;    // start the nonesttables group
    if (!WriteRtfControl(w,rtf,"nonesttables",PARAM_NONE,0)) return FALSE;
    if (!WriteRtfControl(w,rtf,"par",PARAM_NONE,0)) return FALSE;
    if (!EndRtfGroup(w,rtf)) return FALSE;       // end the nesttableprops group

    return TRUE;
}
 
/*****************************************************************************
    WriteRtfCell:
    Write the table cell information to the file
******************************************************************************/
BOOL WriteRtfCell(PTERWND w,struct StrRtfOut far *rtf, int CurCellId)
{
    int row=cell[CurCellId].row,ColSpan,i,x;
    UINT flags=cell[CurCellId].flags;
          
    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    // write the vertical alignment
    if (!(flags&(CFLAG_VALIGN_CTR|CFLAG_VALIGN_BOT|CFLAG_VALIGN_BASE)) && !WriteRtfControl(w,rtf,"clvertalt",PARAM_NONE,0)) return FALSE;  // top aligned
    if (flags&CFLAG_VALIGN_CTR && !WriteRtfControl(w,rtf,"clvertalc",PARAM_NONE,0)) return FALSE;  // center aligned
    if (flags&CFLAG_VALIGN_BOT && !WriteRtfControl(w,rtf,"clvertalb",PARAM_NONE,0)) return FALSE;  // bottom aligned
    if (flags&CFLAG_VALIGN_BASE && !WriteRtfControl(w,rtf,"ssclvertalbs",PARAM_NONE,0)) return FALSE;  // bottom aligned


    // write any vertical/horizontal merging
    if (cell[CurCellId].RowSpan>1 && !(cell[CurCellId].flags&CFLAG_ROW_SPANNED) && !WriteRtfControl(w,rtf,"clvmgf",PARAM_NONE,0)) return FALSE;
    if (cell[CurCellId].flags&CFLAG_ROW_SPANNED) {
        int SpanningCell=CellAux[CurCellId].SpanningCell;
        if (SpanningCell>0 && cell[SpanningCell].RowSpan>1 && !WriteRtfControl(w,rtf,"clvmrg",PARAM_NONE,0)) return FALSE;
    }

    if (cell[CurCellId].ColSpan>1) {
       if (cell[CurCellId].level==RtfInitLevel && !WriteRtfControl(w,rtf,"clmgf",PARAM_NONE,0)) return FALSE;
       if (cell[CurCellId].level!=RtfInitLevel && !WriteRtfControl(w,rtf,"sscolspan",PARAM_INT,cell[CurCellId].ColSpan)) return FALSE;
    }

    // write border
    if (!WriteRtfCellBorder(w,rtf,CurCellId)) return FALSE;

    // write shading percentage
    if (cell[CurCellId].shading>0 && !WriteRtfControl(w,rtf,"clshdng",PARAM_INT,cell[CurCellId].shading*(long)100)) return FALSE;

    // write the foreground color of the background pattern
    if (cell[CurCellId].BackColor != CLR_WHITE || cell[CurCellId].ParentCell>0 || cell[CurCellId].flags&CFLAG_FORCE_BKND_CLR) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==cell[CurCellId].BackColor) break;
       if (i==rtf->TotalColors) i=0;
       if (!WriteRtfControl(w,rtf,"clcbpat",PARAM_INT,i)) return FALSE;        // write the color index
    }

    // write cell margin override
    if (cell[CurCellId].flags&CFLAG_MARGIN_OVERRIDE) {
       if (!WriteRtfControl(w,rtf,"clpadt",PARAM_INT,cell[CurCellId].margin)) return FALSE;     
       if (!WriteRtfControl(w,rtf,"clpadr",PARAM_INT,cell[CurCellId].margin)) return FALSE;     
       if (!WriteRtfControl(w,rtf,"clpadft",PARAM_INT,3)) return FALSE;       // flag used by word 2000
       if (!WriteRtfControl(w,rtf,"clpadfr",PARAM_INT,3)) return FALSE;     
    } 

    // write text angle
    if (cell[CurCellId].TextAngle==90 && !WriteRtfControl(w,rtf,"cltxbtlr",PARAM_NONE,0)) return FALSE;
    if (cell[CurCellId].TextAngle==270 && !WriteRtfControl(w,rtf,"cltxtbrl",PARAM_NONE,0)) return FALSE;

    // write ending x position
    x=cell[CurCellId].x+cell[CurCellId].width-TableRow[row].AddedIndent;
    ColSpan=cell[CurCellId].ColSpan;

    if (ColSpan>1 && cell[CurCellId].level==RtfInitLevel) 
       x-=(2*TableRow[row].CellMargin*(ColSpan-1));
    if (!WriteRtfControl(w,rtf,"cellx",PARAM_INT,x)) return FALSE;
    
    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    // write ColSpan cellx for the root level, nested level do not support clmrg tag
    if (cell[CurCellId].level==RtfInitLevel) {
       while (ColSpan>1) {
          if (!WriteRtfControl(w,rtf,"clmrg",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfCellBorder(w,rtf,CurCellId)) return FALSE;

          x+=(2*TableRow[row].CellMargin);
          if (!WriteRtfControl(w,rtf,"cellx",PARAM_INT,x)) return FALSE;
          ColSpan--;
       }
    }
    FlushRtfLine(w,rtf);             // Create line berak for visual aid


    return TRUE;
}

/*****************************************************************************
    WriteRtfCellBorder:
    Write the cell border tags
******************************************************************************/
BOOL WriteRtfCellBorder(PTERWND w,struct StrRtfOut far *rtf, int CellId)
{
    COLORREF PrevColor=CLR_AUTO,CurColor;
    int width,PrevIndex=-1,CurIndex,i,j;

    for (i=0;i<4;i++)  {
       width=cell[CellId].BorderWidth[i];
       if (i==BORDER_INDEX_TOP   && !(cell[CellId].border&BORDER_TOP))   width=0;
       if (i==BORDER_INDEX_BOT   && !(cell[CellId].border&BORDER_BOT))   width=0;
       if (i==BORDER_INDEX_LEFT  && !(cell[CellId].border&BORDER_LEFT))  width=0;
       if (i==BORDER_INDEX_RIGHT && !(cell[CellId].border&BORDER_RIGHT)) width=0;

       if (width==0 && cell[CellId].BorderColor[i]==CLR_AUTO) continue;

       if (i==BORDER_INDEX_TOP   && !WriteRtfControl(w,rtf,"clbrdrt",PARAM_NONE,0)) return FALSE;
       if (i==BORDER_INDEX_BOT   && !WriteRtfControl(w,rtf,"clbrdrb",PARAM_NONE,0)) return FALSE;
       if (i==BORDER_INDEX_LEFT  && !WriteRtfControl(w,rtf,"clbrdrl",PARAM_NONE,0)) return FALSE;
       if (i==BORDER_INDEX_RIGHT && !WriteRtfControl(w,rtf,"clbrdrr",PARAM_NONE,0)) return FALSE;

       if (cell[CellId].BorderWidth[i]>0) {
          if (!WriteRtfControl(w,rtf,"brdrs",PARAM_NONE,0)) return FALSE;
          if (!WriteRtfControl(w,rtf,"brdrw",PARAM_INT,width)) return FALSE;
       }
    
       if ((CurColor=cell[CellId].BorderColor[i])!=CLR_AUTO) {
          if (CurColor==PrevColor) CurIndex=PrevIndex;
          else { 
             for (j=0;j<rtf->TotalColors;j++) if (rtf->color[j].color==CurColor) break;
             if (j==rtf->TotalColors) j=0;
             PrevColor=CurColor;
             PrevIndex=CurIndex=j;
          }
          if (!WriteRtfControl(w,rtf,"brdrcf",PARAM_INT,CurIndex)) return FALSE;
       }
    }

    return TRUE;
}

/*****************************************************************************
    WriteRtfFont:
    Write the font table to the rtf output device.
******************************************************************************/
BOOL WriteRtfFont(PTERWND w,struct StrRtfOut far *rtf)
{
    int i,j,RtfIndex;
    BYTE name[32],family[32];
    BYTE FontFamily,DefFamily;
    LPBYTE FirstTypeFace="Times New Roman";
    int FirstFamily=FF_ROMAN;
    int FirstCharSet=0;

    if (!WriteRtfControl(w,rtf,"deff",PARAM_INT,(strcmpi(TerFont[0].TypeFace,FirstTypeFace)==0)?0:1)) return false;    // The default font will be written in the first or second slot

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    // create a symbol font for bullets if necessary
    for (i=0;i<TotalPfmts;i++) if (PfmtId[i].flags&BULLET || PfmtId[i].BltId>0) break;
    if (i<TotalPfmts) {   // bullets used
       for (i=0;i<TotalFonts;i++) if (TerFont[i].InUse && lstrcmpi(TerFont[i].TypeFace,"Symbol")==0) break;
       if (i==TotalFonts) GetNewFont(w,hTerDC,0,"Symbol",12,0,0,CLR_WHITE,CLR_AUTO,0,0,0,1,0,0,0,0,NULL,0,0,DEFAULT_CHARSET,0,0);
       for (i=0;i<TotalFonts;i++) if (TerFont[i].InUse && lstrcmpi(TerFont[i].TypeFace,"Wingdings")==0) break;
       if (i==TotalFonts) GetNewFont(w,hTerDC,0,"Wingdings",12,0,0,CLR_WHITE,CLR_AUTO,0,0,0,1,0,0,0,0,NULL,0,0,DEFAULT_CHARSET,0,0);
    }

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin font group
    if (!WriteRtfControl(w,rtf,"fonttbl",PARAM_NONE,0)) return FALSE;

    // find default font family
    GetCharSet(w,hTerDC,TerArg.FontTypeFace,&DefFamily,NULL,NULL);
    DefFamily=(BYTE)(DefFamily&0xF0);             // discard pitch info

    RtfIndex=0;

    //write Times New Roman in the first entry for MS Word copy/paste compatibility ('normal' styleitem need to be in Times New Roman)
    WriteRtfOneFont(w,rtf,RtfIndex,FirstTypeFace,"roman",FirstCharSet);
    RtfIndex++;

    for (i=0;i<TotalFonts;i++) {
      // get typeface and font family
      if (!TerFont[i].InUse || TerFont[i].style&PICT) {
         TerFont[i].RtfIndex=0;
         continue;
      }

      // check if this font alread written
      for (j=0;j<i;j++) {
         if (lstrcmpi(TerFont[j].TypeFace,TerFont[i].TypeFace)==0 &&
           TerFont[j].FontFamily==TerFont[i].FontFamily && 
           TerFont[j].CharSet==TerFont[i].CharSet) break;
      }
      if (j<i) {                  // font already written
         TerFont[i].RtfIndex=TerFont[j].RtfIndex;
         continue;
      }

      if (lstrcmpi(TerFont[i].TypeFace,FirstTypeFace)==0 &&
          TerFont[i].FontFamily==FirstFamily &&
          TerFont[i].CharSet==FirstCharSet) {
         TerFont[i].RtfIndex=0;                        // written in the first slot
         continue;
      }

      lstrcpy(name,TerFont[i].TypeFace);
      FontFamily=TerFont[i].FontFamily;

      if      (FontFamily==FF_ROMAN)      lstrcpy(family,"roman");
      else if (FontFamily==FF_SWISS)      lstrcpy(family,"swiss");
      else if (FontFamily==FF_MODERN)     lstrcpy(family,"modern");
      else if (FontFamily==FF_SCRIPT)     lstrcpy(family,"script");
      else if (FontFamily==FF_DECORATIVE) lstrcpy(family,"decor");
      else                                lstrcpy(family,"nil");

      //write the font entry
      WriteRtfOneFont(w,rtf,RtfIndex,name,family,TerFont[i].CharSet);
      TerFont[i].RtfIndex=RtfIndex;
      RtfIndex++;

      rtf->WritingControl=FALSE;                  // enable line breaks
    }

    // write the stylesheet fonts
    for (i=0;i<TotalSID;i++) {
       StyleId[i].RtfIndex=0;                     // initialize 
       if (!StyleId[i].InUse || lstrlen(StyleId[i].TypeFace)==0) continue;

       // check if this font alread written
       for (j=0;j<TotalFonts;j++) {
          if (TerFont[j].InUse  && lstrcmpi(TerFont[j].TypeFace,StyleId[i].TypeFace)==0 &&
            (TerFont[j].FontFamily==StyleId[i].FontFamily || StyleId[i].FontFamily==0)) break;
       }
       if (j<TotalFonts) {                     // font already written
          StyleId[i].RtfIndex=TerFont[j].RtfIndex;
          continue;
       }

       for (j=0;j<i;j++) {                     // check the stylesheet table
          if (StyleId[j].InUse  && lstrcmpi(StyleId[j].TypeFace,StyleId[i].TypeFace)==0 &&
            StyleId[j].FontFamily==StyleId[i].FontFamily) break;
       }
       if (j<i) {                     // font already written
          StyleId[i].RtfIndex=StyleId[j].RtfIndex;
          continue;
       }

       lstrcpy(name,StyleId[i].TypeFace);
       FontFamily=StyleId[i].FontFamily;

       if      (FontFamily==FF_ROMAN)      lstrcpy(family,"roman");
       else if (FontFamily==FF_SWISS)      lstrcpy(family,"swiss");
       else if (FontFamily==FF_MODERN)     lstrcpy(family,"modern");
       else if (FontFamily==FF_SCRIPT)     lstrcpy(family,"script");
       else if (FontFamily==FF_DECORATIVE) lstrcpy(family,"decor");
       else                                lstrcpy(family,"nil");

       //write the font entry
       WriteRtfOneFont(w,rtf,RtfIndex,name,family,DEFAULT_CHARSET);
       StyleId[i].RtfIndex=RtfIndex;
       RtfIndex++;

       rtf->WritingControl=FALSE;                  // enable line breaks
    }    

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end font group

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    return TRUE;
}

/*****************************************************************************
    WriteRtfOneFont:
    Write one font table entry to the rtf output device.
******************************************************************************/
BOOL WriteRtfOneFont(PTERWND w,struct StrRtfOut far *rtf, int RtfIndex, LPBYTE name, LPBYTE family, int CharSet)
{
    if (!BeginRtfGroup(w,rtf)) return FALSE;// begin current font

    if (!WriteRtfControl(w,rtf,"f",PARAM_INT,RtfIndex)) return FALSE;  // write font number


    rtf->WritingControl=TRUE;                   // disable line break
    if (!PutRtfChar(w,rtf,'\\')) return FALSE;  // write family control prefix
    if (!PutRtfChar(w,rtf,'f'))  return FALSE;  // write family control
    if (!WriteRtfText(w,rtf,family,lstrlen(family))) return FALSE; // write font family
    if (!WriteRtfText(w,rtf," ",1)) return FALSE;// write family delimiter

    if ((mbcs || CharSet!=DEFAULT_CHARSET) && !WriteRtfControl(w,rtf,"fcharset",PARAM_INT,CharSet)) return FALSE;  // write font number

    if (!WriteRtfText(w,rtf,name,lstrlen(name))) return FALSE; // write font typeface
    if (!WriteRtfText(w,rtf,";",1)) return FALSE;// write typeface delimiter

    if (!EndRtfGroup(w,rtf)) return FALSE;      // end current font

    return TRUE;
}

/*****************************************************************************
    WriteRtfColor:
    Fill the RTF color table and write the color table to the rtf output device.
******************************************************************************/
BOOL WriteRtfColor(PTERWND w,struct StrRtfOut far *rtf)
{
    int i,j,k,TotalColors;
    BYTE red,green,blue;
    struct StrRtfColor far *color;

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    // Fill the rtf color table
    color=rtf->color;
    color[0].color=TerFont[0].TextColor;    // default color
    TotalColors=1;

    // add page background color
    if (PageBkColor!=CLR_WHITE && color[0].color!=PageBkColor) {
       color[TotalColors].color=PageBkColor;
       TotalColors++;
    } 
    
    // scan the font table
    for (i=0;i<TotalFonts;i++) {
       // fill the text foreground color
       for (j=0;j<TotalColors;j++) {                        // scan the color table
          if (TerFont[i].TextColor==color[j].color) break;  // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=TerFont[i].TextColor;
          TotalColors++;
       }
       // fill the text foreground color
       for (j=0;j<TotalColors;j++) {                         // scan the color table
          if (TerFont[i].TextBkColor==color[j].color) break; // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=TerFont[i].TextBkColor;
          TotalColors++;
       }
       // fill underline color
       for (j=0;j<TotalColors;j++) {                        // scan the color table
          if (TerFont[i].UlineColor==color[j].color) break;  // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=TerFont[i].UlineColor;
          TotalColors++;
       }
    }

    // scan the stylesheet table
    for (i=0;i<TotalSID;i++) {
       // fill the text foreground color
       for (j=0;j<TotalColors;j++) {                        // scan the color table
          if (StyleId[i].TextColor==color[j].color) break;  // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=StyleId[i].TextColor;
          TotalColors++;
       }
       // fill the text foreground color
       for (j=0;j<TotalColors;j++) {                         // scan the color table
          if (StyleId[i].TextBkColor==color[j].color) break; // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=StyleId[i].TextBkColor;
          TotalColors++;
       }
       // fill the underline color
       for (j=0;j<TotalColors;j++) {                        // scan the color table
          if (StyleId[i].UlineColor==color[j].color) break;  // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=StyleId[i].UlineColor;
          TotalColors++;
       }

       // fill the paragraph background color
       if (StyleId[i].ParaBkColor!=CLR_WHITE) {  // if not the default color
          for (j=0;j<TotalColors;j++) {                         // scan the color table
             if (StyleId[i].ParaBkColor==color[j].color) break; // color found
          }
          if (j==TotalColors && TotalColors<MaxRtfColors) {
             color[TotalColors].color=StyleId[i].ParaBkColor;
             TotalColors++;
          }
       }

       // fill the paragraph border color
       if (StyleId[i].ParaBkColor!=CLR_AUTO) {  // if not the default color
          for (j=0;j<TotalColors;j++) {                         // scan the color table
             if (StyleId[i].ParaBorderColor==color[j].color) break; // color found
          }
          if (j==TotalColors && TotalColors<MaxRtfColors) {
             color[TotalColors].color=StyleId[i].ParaBorderColor;
             TotalColors++;
          }
       }

    }

    // scan the section table
    for (i=0;i<TotalSects;i++) {
       if (!TerSect[i].border) continue;

       // fill the text foreground color
       for (j=0;j<TotalColors;j++) {                        // scan the color table
          if (TerSect[i].BorderColor==color[j].color) break;  // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=TerSect[i].BorderColor;
          TotalColors++;
       }
    }

    // scan the cell background colors
    for (i=0;i<TotalCells;i++) {
       if (!cell[i].InUse) continue;

       // fill the cell background color
       for (j=0;j<TotalColors;j++) {                     // scan the color table
          if (cell[i].BackColor==color[j].color) break;  // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=cell[i].BackColor;
          TotalColors++;
       }

       // fill the cell border color
       for (k=0;k<4;k++) {        // do each border
          if (cell[i].BorderWidth[k]<=0) continue;
          if (cell[i].BorderColor[k]==CLR_AUTO) continue;

          for (j=0;j<TotalColors;j++) {                     // scan the color table
             if (cell[i].BorderColor[k]==color[j].color) break;  // color found
          }
          if (j==TotalColors && TotalColors<MaxRtfColors) {
             color[TotalColors].color=cell[i].BorderColor[k];
             TotalColors++;
          }
       } 
    }

    // scan the paragraph background colors
    for (i=0;i<TotalPfmts;i++) {
       // fill the para background color
       for (j=0;j<TotalColors;j++) {                     // scan the color table
          if (PfmtId[i].BkColor==color[j].color) break;  // color found
       }
       if (j==TotalColors && TotalColors<MaxRtfColors) {
          color[TotalColors].color=PfmtId[i].BkColor;
          TotalColors++;
       }

       // fill the para background color
       if (PfmtId[i].BorderColor!=CLR_AUTO) {
          for (j=0;j<TotalColors;j++) {                     // scan the color table
             if (PfmtId[i].BorderColor==color[j].color) break;  // color found
          }
          if (j==TotalColors && TotalColors<MaxRtfColors) {
             color[TotalColors].color=PfmtId[i].BorderColor;
             TotalColors++;
          }
       }
    }

    rtf->TotalColors=TotalColors;                           // total number of colors in the table

    // write the color table
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin color group
    if (!WriteRtfControl(w,rtf,"colortbl",PARAM_NONE,0)) return FALSE;

    for (i=0;i<TotalColors;i++) {                  // write colors from the font table
      if (i>0 || (rtf->output>=RTF_CB && False(TerFlags6&TFLAG6_WRITE_DEFAULT_COLOR)) 
              || TerFlags2&TFLAG2_WRITE_FIRST_RTF_COLOR) {  // write the current color
         red=GetRValue(color[i].color);
         green=GetGValue(color[i].color);
         blue=GetBValue(color[i].color);
         if (color[i].color==CLR_AUTO) red=green=blue=0;
         if (!WriteRtfControl(w,rtf,"red",PARAM_INT,red)) return FALSE;
         if (!WriteRtfControl(w,rtf,"green",PARAM_INT,green)) return FALSE;
         if (!WriteRtfControl(w,rtf,"blue",PARAM_INT,blue)) return FALSE;
      }
      if (!WriteRtfText(w,rtf,";",1)) return FALSE;  // write the delimiter
    }

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end color group

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    return TRUE;
}

/*****************************************************************************
    WriteRtfRev:
    Write the reviewer table.
******************************************************************************/
BOOL WriteRtfRev(PTERWND w,struct StrRtfOut far *rtf)
{
    int i;
    LPBYTE pName;

    if (rtf->output>=RTF_CB && TrackChanges) return true;

    FlushRtfLine(w,rtf);             // Create line berak for visual aid


    // write the color table
    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin outer group
    if (!WriteRtfControl(w,rtf,"*",PARAM_NONE,0)) return FALSE;        // make it a destination group
    if (!WriteRtfControl(w,rtf,"revtbl",PARAM_NONE,0)) return FALSE;

    for (i=0;i<TotalReviewers;i++) {                  // write reviewers
      if (!BeginRtfGroup(w,rtf)) return FALSE;        // begin the inner group
      
      pName=(strlen(reviewer[i].name)==0)?"Unknown":reviewer[i].name;
      if (!WriteRtfText(w,rtf,pName,strlen(pName))) return FALSE;   // author name
      if (!WriteRtfText(w,rtf,";",1)) return FALSE;   // write the delimiter
      
      if (!EndRtfGroup(w,rtf)) return FALSE;          // end inner group
    }

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end outer group

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    return TRUE;
}


/*****************************************************************************
    WriteRtfStylesheet:
    Write the stylesheet table
******************************************************************************/
BOOL WriteRtfStylesheet(PTERWND w,struct StrRtfOut far *rtf)
{
    int i;

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin stylesheet group
    if (!WriteRtfControl(w,rtf,"stylesheet",PARAM_NONE,0)) return FALSE;

    rtf->flags|=ROFLAG_IN_STYLESHEET;

    // write each stylesheet item
    for (i=0;i<TotalSID;i++) {
       if (!StyleId[i].InUse) continue;

       // start the style item
       if (!BeginRtfGroup(w,rtf)) return FALSE;      // begin stylesheet item group

       // write the body of the style item
       if (StyleId[i].type==SSTYPE_CHAR) {
           if (!WriteRtfControl(w,rtf,"cs",PARAM_INT,i)) return FALSE;  // write the id
           if (!WriteRtfCharStyle(w,rtf,i)) return FALSE;
       }
       else  {
           if (i>0 && !WriteRtfControl(w,rtf,"s",PARAM_INT,i)) return FALSE;  // write the id
           if (i>0 && !WriteRtfControl(w,rtf,"snext",PARAM_INT,StyleId[i].next)) return FALSE;  // write the next id
           if (!WriteRtfCharStyle(w,rtf,i)) return FALSE;   // write the character formatting first
           if (!WriteRtfParaStyle(w,rtf,i)) return FALSE;
       }

       // write the name and end the item
       if (!WriteRtfText(w,rtf,StyleId[i].name,lstrlen(StyleId[i].name))) return FALSE; // write font typeface
       if (!WriteRtfText(w,rtf,";",1)) return FALSE;// write delimiter

       if (!EndRtfGroup(w,rtf)) return FALSE;        // end group
    }

    if (!EndRtfGroup(w,rtf)) return FALSE;        // end group

    ResetUintFlag(rtf->flags,ROFLAG_IN_STYLESHEET);

    FlushRtfLine(w,rtf);             // Create line berak for visual aid

    return TRUE;
}

/*****************************************************************************
    WriteRtfCharStyle:
    Write the character type stylesheet item
******************************************************************************/
BOOL WriteRtfCharStyle(PTERWND w,struct StrRtfOut far *rtf, int id)
{
    int i;
    UINT style=StyleId[id].style;

    // write if additive
    //if (StyleId[id].flags&SSFLAG_ADD && !WriteRtfControl(w,rtf,"additive",PARAM_NONE,0L)) return FALSE;   // write additive - seems to cause problem with Windows 2000

    //write typeface/family
    if (id>0 && lstrlen(StyleId[id].TypeFace)>0) {
      if (!WriteRtfControl(w,rtf,"f",PARAM_INT,StyleId[id].RtfIndex)) return FALSE;  // write font number
    }

    //write point size
    if (StyleId[id].TwipsSize>0 && !WriteRtfControl(w,rtf,"fs",PARAM_INT,StyleId[id].TwipsSize/10)) return FALSE;   // write point size

    // write styles
    if (style&BOLD && !WriteRtfControl(w,rtf,"b",PARAM_NONE,0)) return FALSE;
    if (style&ULINE && !WriteRtfControl(w,rtf,"ul",PARAM_NONE,0)) return FALSE;
    if (style&ULINED && !WriteRtfControl(w,rtf,"uldb",PARAM_NONE,0)) return FALSE;
    if (style&ITALIC && !WriteRtfControl(w,rtf,"i",PARAM_NONE,0)) return FALSE;
    if (style&HIDDEN && !WriteRtfControl(w,rtf,"v",PARAM_NONE,0)) return FALSE;
    if (style&STRIKE && !WriteRtfControl(w,rtf,"strike",PARAM_NONE,0)) return FALSE;
    if (style&PROTECT && !WriteRtfControl(w,rtf,"protect",PARAM_NONE,0)) return FALSE;
    if (style&SUPSCR && !WriteRtfControl(w,rtf,"super",PARAM_NONE,0)) return FALSE;
    if (style&SUBSCR && !WriteRtfControl(w,rtf,"sub",PARAM_NONE,0)) return FALSE;
    if (style&HLINK && !WriteRtfControl(w,rtf,"sshlink",PARAM_NONE,0)) return FALSE;
    if (style&CAPS && !WriteRtfControl(w,rtf,"caps",PARAM_NONE,0)) return FALSE;
    if (style&SCAPS && !WriteRtfControl(w,rtf,"scaps",PARAM_NONE,0)) return FALSE;

    //write foreground color color
    //if (StyleId[id].TextColor!=0) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==StyleId[id].TextColor) break;
       if (i==rtf->TotalColors) i=0;
       if (!WriteRtfControl(w,rtf,"cf",PARAM_INT,i)) return FALSE;        // write the color index
    //}

    //write background color color
    //if (StyleId[id].TextBkColor!=CLR_WHITE) {
       for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==StyleId[id].TextBkColor) break;
       if (i==rtf->TotalColors) i=0;
       if (!WriteRtfControl(w,rtf,"cb",PARAM_INT,i)) return FALSE;        // write the color index
    //}

    for (i=0;i<rtf->TotalColors;i++) if (rtf->color[i].color==StyleId[id].UlineColor) break;
    if (i==rtf->TotalColors) i=0;
    if (!WriteRtfControl(w,rtf,"ulc",PARAM_INT,i)) return FALSE;        // write the color index

    // write character space
    if (StyleId[id].expand) {
        if (!WriteRtfControl(w,rtf,"expnd",PARAM_INT,(StyleId[id].expand*4/20))) return FALSE;   // write expansion in 1/4 of points
        if (!WriteRtfControl(w,rtf,"expndtw",PARAM_INT,StyleId[id].expand)) return FALSE;   // write expansion in twips
    }

    // write character space
    if (StyleId[id].offset) {
       int CurOffset=StyleId[id].offset;

       WriteRtfControl(w,rtf,CurOffset>0?"up":"dn",PARAM_INT,TwipsToPoints(abs(CurOffset)*2)); // turn on
    }

    return TRUE;
}

