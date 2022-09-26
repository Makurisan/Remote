/*==============================================================================
   TER_BLK.C
   TER Block Copy/Move/Delete functions.

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
   TerNormalizeBlock:
   Normalize beginning and ending of the hilighted blocks.
*******************************************************************************/
BOOL WINAPI _export TerNormalizeBlock(HWND hWnd)
{
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   return NormalizeBlock(w);
}

/******************************************************************************
   NormalizeBlock:
   Normalize beginning and ending of the hilighted blocks.
*******************************************************************************/
NormalizeBlock(PTERWND w)
{
   int temp,col;
   long l,temp1,line;
   int  i,BegCol,EndCol,CurFont,BegFont,EndFont;
   BOOL AdjCurPos;

   if (HilightType==HILIGHT_OFF) return TRUE;

   if (HilightType==HILIGHT_CHAR) {
      if (HilightBegRow<0) HilightBegRow=0;    // validity check
      if (HilightEndRow<0) HilightEndRow=0;
      if (HilightBegRow>=TotalLines) HilightBegRow=TotalLines-1;
      if (HilightEndRow>=TotalLines) HilightEndRow=TotalLines-1;

      // exclude zero length block
      if (HilightBegRow==HilightEndRow && HilightBegCol==HilightEndCol) {
         HilightType=HILIGHT_OFF;
         return FALSE;
      }


      if (HilightBegRow>HilightEndRow) {
         temp1=HilightEndRow;HilightEndRow=HilightBegRow;HilightBegRow=temp1;
         temp=HilightEndCol;HilightEndCol=HilightBegCol;HilightBegCol=temp;
      }
      if (HilightBegRow==HilightEndRow && HilightBegCol>HilightEndCol) {
         temp=HilightEndCol;HilightEndCol=HilightBegCol;HilightBegCol=temp;
      }
      if (TerArg.WordWrap && HilightBegCol>=LineLen(HilightBegRow)) HilightBegCol=LineLen(HilightBegRow)-1;
      if (!TerArg.WordWrap && HilightBegCol>LineLen(HilightBegRow)) HilightBegCol=LineLen(HilightBegRow);
      if (HilightBegCol<0) HilightBegCol=0;
      if (HilightEndCol>LineLen(HilightEndRow)) HilightEndCol=LineLen(HilightEndRow);
      if (HilightEndCol<0) HilightEndCol=0;

      if (AllSelected(w)) return TRUE; // test if the entire text is highlighted

      if (!InUndo && False(TerFlags5&TFLAG5_NO_ADJ_FOR_TABLE)) AdjustBlockForTable(w,&HilightBegRow,&HilightBegCol,&HilightEndRow,&HilightEndCol,TRUE);
   }
   else {
      if (HilightBegRow>HilightEndRow) {temp1=HilightEndRow;HilightEndRow=HilightBegRow;HilightBegRow=temp1;}
      if (HilightBegCol>HilightEndCol) {temp=HilightEndCol;HilightEndCol=HilightBegCol;HilightBegCol=temp;}

      if (AllSelected(w)) return TRUE; // test if the entire text is highlighted
   }
   if (HilightBegRow>=TotalLines) HilightBegRow=TotalLines-1;
   if (HilightEndRow>=TotalLines) HilightEndRow=TotalLines-1;

   StretchHilight=FALSE;               // stop stretching

   // exclude hidden text from either end of the selection when the hidden text is not visible
   if (TerFlags&TFLAG_EXCLUDE_HIDDEN_SEL && !ShowHiddenText) {
      // modify the hilight beginning position
      line=HilightBegRow;
      col=HilightBegCol;
      AdjCurPos=(line==CurLine && col==CurCol)?TRUE:FALSE;  // adjust current position

      CurFont=GetCurCfmt(w,line,col);
      if (TerFont[CurFont].style&HIDDEN) {
         if (TerLocateStyleChar(hTerWnd,HIDDEN,FALSE,&line,&col,TRUE)) {
            if (line<HilightEndRow || (line==HilightEndRow && col<=(HilightEndCol-1))) {
               HilightBegRow=line;
               HilightBegCol=col;
               if (AdjCurPos) {
                  CurLine=line;
                  CurCol=col;
               }
            }
         }
      }
      // modify the hilight ending position
      line=HilightEndRow;
      col=HilightEndCol-1;          // position where visible hilighting ends
      AdjCurPos=(line==CurLine && (col+1)==CurCol)?TRUE:FALSE;  // adjust current position

      if (col<0 && line>0) {
         line--;
         col=LineLen(line)-1;
      }
      if (col<0) col=0;
      CurFont=GetCurCfmt(w,line,col);
      if (TerFont[CurFont].style&HIDDEN) {
         if (TerLocateStyleChar(hTerWnd,HIDDEN,FALSE,&line,&col,FALSE)) {
            if (line>HilightBegRow || (line==HilightBegRow && col>=HilightBegCol)) {
               HilightEndRow=line;
               HilightEndCol=col+1;
               if (AdjCurPos) {
                  CurLine=line;
                  CurCol=col+1;
               }
            }
         }
      }
      CursDirection=CURS_FORWARD;
   }

   // drag fully the protected text
   if ((DraggingText) && True(TerFlags4&TFLAG4_FULL_DRAG_PROT_TEXT)) {
      int BegField,EndField;
      BegFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
      EndFont=GetPrevCfmt(w,HilightEndRow,HilightEndCol);
      BegField=TerFont[BegFont].FieldId;
      EndField=TerFont[EndFont].FieldId;
   
      if (True(TerFont[BegFont].style&PROTECT) && BegField==0) {  // change the beginning position
          if (TerLocateStyleChar(hTerWnd,PROTECT,false,&HilightBegRow,&HilightBegCol,false)) { // scan backward for absense of prot
            NextTextPos(w,&HilightBegRow,&HilightBegCol);  // position at first protected text
          }
      } 
      if (True(TerFont[EndFont].style&PROTECT) && EndField==0) {  // change the beginning position
          TerLocateStyleChar(hTerWnd,PROTECT,false,&HilightEndRow,&HilightEndCol,true); // scan forward for absense of prot
      } 
   } 

   
   // makesure that any field are completely included
   if (DraggingText || (!(TerOpFlags&TOFLAG_NO_FIELD_ADJ) && !(TerFlags5&TFLAG5_NO_NORMALIZE_FIELD))) {
      BOOL adjust=FALSE,full,OnDifferentFields;
      int BegField,EndField;
      BegFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
      EndFont=GetPrevCfmt(w,HilightEndRow,HilightEndCol);
      BegField=TerFont[BegFont].FieldId;
      EndField=TerFont[EndFont].FieldId;

      if (BegField==FIELD_NAME || BegField==FIELD_DATA || BegField==FIELD_TOC) adjust=TRUE;
      if (EndField==FIELD_NAME || EndField==FIELD_DATA || EndField==FIELD_TOC) adjust=TRUE;

      if (adjust && TerFont[BegFont].FieldId==TerFont[EndFont].FieldId) {    // test if selection spans multiple fields
         int FieldId=TerFont[BegFont].FieldId;
         int BefFont=GetPrevCfmt(w,HilightBegRow,HilightBegCol);
         int AftFont=GetCurCfmt(w,HilightEndRow,HilightEndCol);
         full=(TerFont[BefFont].FieldId!=FieldId && TerFont[AftFont].FieldId!=FieldId);
         if (!full) {     // scan for non field chracter in the range
            OnDifferentFields=FALSE;
            for (l=HilightBegRow;l<=HilightEndRow;l++) {
               BegCol=(l==HilightBegRow)?HilightBegCol:0;
               EndCol=(l==HilightEndRow)?HilightEndCol:LineLen(l);
               for (i=BegCol;i<EndCol;i++) {
                  CurFont=GetCurCfmt(w,l,i);
                  if (TerFont[CurFont].FieldId!=FieldId) {
                    OnDifferentFields=TRUE;
                    break;
                  }
               }
               if (OnDifferentFields) break;
            }
            if (!OnDifferentFields) adjust=FALSE;    // on the same field but the selection does not include the entire field component
         }
      }
      if (BegField==FIELD_TOC || EndField==FIELD_TOC || DraggingText) adjust=TRUE;


      if (adjust) {
        if (BegField!=FIELD_NAME && BegField!=FIELD_DATA && EndField==FIELD_NAME && !ShowFieldNames) {
            // deselect the field name
            line=HilightEndRow;
            col=HilightEndCol-1;   // position of the last highlighted character
            FixPos(w,&line,&col);
            GetFieldScope(w,line,col,FIELD_NAME,&HilightEndRow,&HilightEndCol,NULL,NULL);  // position at the beginning of the field name
        }
        else {    // select the field completely
            CurFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
            if (TerFont[CurFont].FieldId==FIELD_NAME || TerFont[CurFont].FieldId==FIELD_DATA )  // position at the beginning of the field name
                GetFieldScope(w,HilightBegRow,HilightBegCol,FIELD_NAME,&HilightBegRow,&HilightBegCol,NULL,NULL);
            else if (TerFont[CurFont].FieldId==FIELD_TOC) // position at the beginning of the toc field
                GetFieldLoc(w,HilightBegRow,HilightBegCol,TRUE,&HilightBegRow,&HilightBegCol);

            line=HilightEndRow;
            col=HilightEndCol-1;   // position of the last highlighted character
            FixPos(w,&line,&col);
            CurFont=GetCurCfmt(w,line,col);
            if (TerFont[CurFont].FieldId==FIELD_NAME || TerFont[CurFont].FieldId==FIELD_DATA) {// position at the beginning of the field name
                BOOL result=GetFieldScope(w,line,col,FIELD_DATA,NULL,NULL,&HilightEndRow,&HilightEndCol);
                if (!result) GetFieldScope(w,line,col,FIELD_NAME,NULL,NULL,&HilightEndRow,&HilightEndCol);
            }
            else if (TerFont[CurFont].FieldId==FIELD_TOC)
                GetFieldLoc(w,line,col,FALSE,&HilightEndRow,&HilightEndCol);
        }
      }
   }


   return TRUE;
}

/******************************************************************************
   NormalizeForFootnote:
   Normalize a block to fully contain the footnotes
*******************************************************************************/
NormalizeForFootnote(PTERWND w)
{
   int CurFont,col;
   long line;
   BOOL adjusted,AtBegPos;

   if (HilightType!=HILIGHT_CHAR) return FALSE;

   if (TerOpFlags2&TOFLAG2_NO_NORMALIZE_FNOTE) return TRUE;
   if (TerFlags5&TFLAG5_NO_NORMALIZE_FNOTE) return TRUE;
   

   // adjust the first line
   CurFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
   if (TerFont[CurFont].style&(FNOTETEXT|FNOTEREST)) { // locate the end of the footnote marker
      if (!TerLocateStyleChar(hTerWnd,FNOTE,TRUE,&HilightBegRow,&HilightBegCol,FALSE)) return FALSE;  // search backward
      CurFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
   }

   AtBegPos=(HilightBegRow==0 && HilightBegCol==0);
   if (TerFont[CurFont].style&FNOTE && !AtBegPos) { // locate beginning of the footnote marker
      if (!TerLocateStyleChar(hTerWnd,FNOTE,FALSE,&HilightBegRow,&HilightBegCol,FALSE)) return FALSE; // search backward
      HilightBegCol++;                 // position at the first character of the footnote marker
      if ((HilightBegCol+1)>LineLen(HilightBegRow)) {
         HilightBegRow++;
         HilightBegCol=0;
      }
   }

   // adjust the last line
   line=HilightEndRow;         // position at last character of the block
   col=HilightEndCol-1;
   if (col<0) {
     line--;
     if (line<0) line=0;
     col=LineLen(line)-1;
     if (col<0) col=0;
   }
   adjusted=FALSE;

   CurFont=GetCurCfmt(w,line,col);
   if (TerFont[CurFont].style&(FNOTE)) { // go past footnore marker
      if (!TerLocateStyleChar(hTerWnd,FNOTE,FALSE,&line,&col,TRUE)) return FALSE;  // search forward
      CurFont=GetCurCfmt(w,line,col);
      adjusted=TRUE;
   }
   if (TerFont[CurFont].style&FNOTETEXT) { // go past footnote text
      if (!TerLocateStyleChar(hTerWnd,FNOTETEXT,FALSE,&line,&col,TRUE)) return FALSE; // search forward
      CurFont=GetCurCfmt(w,line,col);
      adjusted=TRUE;
   }
   if (TerFont[CurFont].style&FNOTEREST) { // go paste footnore trailer
      if (!TerLocateStyleChar(hTerWnd,FNOTEREST,FALSE,&line,&col,TRUE)) return FALSE; // search forward
      adjusted=TRUE;
   }
   if (adjusted) {
      HilightEndRow=line;
      HilightEndCol=col;
      TerOpFlags|=TOFLAG_UPDATE_ENOTE;
   }

   return TRUE;
}


/******************************************************************************
   TerDeleteBlock:
   Delete a highlighted block
*******************************************************************************/
BOOL WINAPI _export TerDeleteBlock(HWND hWnd, BOOL repaint)
{
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   if (IsProtected(w,TRUE,TRUE)) return FALSE;

   TerArg.modified++;

   if      (HilightType==HILIGHT_LINE)   return DeleteLineBlock(w,repaint); // delete and repaint screen
   else if (HilightType==HILIGHT_CHAR)   return DeleteCharBlock(w,TRUE,repaint);  // delete and repaint screen
   else return FALSE;
}

/******************************************************************************
   BlockMove(w)
   Move a hilighted block
*******************************************************************************/
BlockMove(PTERWND w)
{
   TerArg.modified++;

   if      (HilightType==HILIGHT_LINE)     CopyLineBlock(w,TRUE);  // copy with delete
   else if (HilightType==HILIGHT_CHAR)     CopyCharBlock(w,TRUE);  // copy with delete

   return TRUE;
}

/******************************************************************************
   BlockCopy(w)
   Copy a hilighted block
*******************************************************************************/
BlockCopy(PTERWND w)
{
   TerArg.modified++;

   if      (HilightType==HILIGHT_LINE)     CopyLineBlock(w,FALSE);  // copy without delete
   else if (HilightType==HILIGHT_CHAR)     CopyCharBlock(w,FALSE);  // copy without delete

   return TRUE;
}

/******************************************************************************
    IsProtected:
    This function returns TRUE if the text in the highlighted area is protected.
*******************************************************************************/
BOOL IsProtected(PTERWND w,BOOL msg, BOOL del)
{
    BOOL   all=FALSE,DelHdrFtr=FALSE;

    if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block
    if (TerFlags&TFLAG_UNPROTECTED_DEL) return FALSE;  // protection off

    // test if the entire text is highlighted
    all=AllSelected(w);

    // Ensure that a section keeps atleast one non header/footer line
    if (del && !all && HilightBegRow>0 && HilightBegCol==0 && HilightEndCol==LineLen(HilightEndRow)
        && PfmtId[pfmt(HilightBegRow-1)].flags&PAGE_HDR_FTR
        && !(PfmtId[pfmt(HilightEndRow)].flags&PAGE_HDR_FTR)
        && (HilightEndRow>=(TotalLines-1) || LineInfo(w,HilightEndRow+1,INFO_SECT)) ) {
        if (msg) PrintError(w,MSG_KEEP_HDRFTR_LINE,MsgString[MSG_PROTECTED]);
        return TRUE;
    }

    // ensure that the para char before html rule is not being deleted
    if (HtmlMode) {
       if (del && HilightBegCol>0 && PfmtId[pfmt(HilightBegRow)].AuxId!=PfmtId[pfmt(HilightEndRow)].AuxId) {
          if (msg) PrintError(w,MSG_NODEL_MIX_HTML,MsgString[MSG_PROTECTED]);
          return TRUE;
       }
    }

    // check if deleting header/footer lines
    if (!all && ((PfmtId[pfmt(HilightBegRow)].flags&PAGE_HDR_FTR) != (PfmtId[pfmt(HilightEndRow)].flags&PAGE_HDR_FTR))) {
       BOOL error=FALSE;
       if (PfmtId[pfmt(HilightBegRow)].flags&PAGE_HDR_FTR) error=TRUE;  // this block starts from header and ends into non-header
       else {
          long EndLine=HilightEndRow,l;
          if (del) {  // delete to  the end of the header footer area
             for (l=HilightEndRow;l<TotalLines;l++) if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) break;  // go past the header/footer area
             HilightEndRow=(l-1);
             HilightEndCol=LineLen(HilightEndRow);
             DelHdrFtr=TRUE;
          }
          else {
             if (HilightEndCol==0 && EndLine>HilightBegRow) EndLine--;
             if  (PfmtId[pfmt(EndLine)].flags&PAGE_HDR_FTR) {
                if (LineFlags(EndLine)&(LFLAG_HDR|LFLAG_FHDR)
                   && EndLine>HilightBegRow && !(PfmtId[pfmt(EndLine-1)].flags&PAGE_HDR_FTR)) {
                   HilightEndRow=EndLine-1;
                   HilightEndCol=LineLen(HilightEndRow);
                }
                else error=TRUE;
             }
          }
       }
       if (error) {
          if (msg) PrintError(w,MSG_SEL_SPAN_HDRFTR,MsgString[MSG_INVALID_SELECT]);
          return TRUE;
       }
    }
    
    // check if deleting the last para char in a header/footer section
    if (del && !all && !DelHdrFtr && HilightEndCol>=LineLen(HilightEndRow) && (HilightEndRow+1)<TotalLines
        && LineFlags(HilightEndRow+1)&LFLAG_HDRS_FTRS) {
        if (msg) PrintError(w,MSG_PROTECTED,MsgString[MSG_INVALID_SELECT]);
        return TRUE;
    }     

    return (del && BlockHasProtectOn(w,msg,TRUE));   // returns TRUE if the block has any protected character
}

/******************************************************************************
    BlockHasProtectOn:
    This function returns TRUE if any character in the block has the PROTECT
    attribute. When the 'msg' is TRUE, the function displays the message
    when the protected text is encountered.
*******************************************************************************/
BlockHasProtectOn(PTERWND w,BOOL msg,BOOL SkipFieldNames)
{
    long l;
    int i,CurFmt,BegCol,EndCol;
    LPWORD fmt;

    for (l=HilightBegRow;l<=HilightEndRow; l++) {
       if (PfmtId[pfmt(l)].pflags&PFLAG_ENDNOTE) continue;   // generated line

       if (!SkipFieldNames && cfmt(l).info.type==UNIFORM && l!=HilightEndRow) {
          CurFmt=cfmt(l).info.fmt;
          if (SkipFieldNames && TerFont[CurFmt].FieldId==FIELD_NAME) continue;

          if (TerFont[CurFmt].style&PROTECT) {
             if (True(TerFont[CurFmt].style&FNOTEALL) && HilightEndCol<LineLen(HilightEndRow)) {
                int LastFmt=GetCurCfmt(w,HilightEndRow,HilightEndCol);
                if (False(TerFont[LastFmt].style&FNOTEALL)) continue;  // endnote completely selected
             } 
             if (msg) PrintError(w,MSG_SEL_PROTECTED,MsgString[MSG_PROTECTED]);
             return TRUE;   // text protected
          }
          continue;
       }

       // define column range
       BegCol=0;
       EndCol=LineLen(l);                  //exclusive
       if (HilightType==HILIGHT_CHAR) {
          if (l==HilightBegRow && BegCol<LineLen(l))  BegCol=HilightBegCol;
          if (l==HilightEndRow && EndCol<=LineLen(l)) EndCol=HilightEndCol;   // exclusive
       }

       fmt=OpenCfmt(w,l);                  // open the format data
       for (i=BegCol;i<EndCol;i++) {
          CurFmt=fmt[i];
          if (SkipFieldNames && TerFont[CurFmt].FieldId==FIELD_NAME) {
             BYTE CurChar=pText(l)[i];
             if (CurChar=='{' || CurChar=='}') continue;    // the protection on the field name braces are insignificant
          }

          if (TerFont[CurFmt].style&PROTECT 
             || (True(TerFlags5&TFLAG5_PROTECT_DATA_FIELD) 
                  && (TerFont[CurFmt].FieldId==FIELD_NAME || TerFont[CurFmt].FieldId==FIELD_DATA)) ){
             CloseCfmt(w,l);
             
             if (True(TerFont[CurFmt].style&FNOTEALL) && HilightEndCol<LineLen(HilightEndRow)) {
                int LastFmt=GetCurCfmt(w,HilightEndRow,HilightEndCol);
                if (False(TerFont[LastFmt].style&FNOTEALL)) continue;  // endnote completely selected
             } 
             
             if (msg) PrintError(w,MSG_SEL_PROTECTED,MsgString[MSG_PROTECTED]);
             return TRUE;
          }
       }
       CloseCfmt(w,l);
    }

    return FALSE;                          // not protected
}

/******************************************************************************
    AllSelected:
    True if the entire text is selected.
*******************************************************************************/
BOOL AllSelected(PTERWND w)
{
  return AllSelected2(w,NULL);
}

BOOL AllSelected2(PTERWND w, BOOL *pLastCharSelected)
{
    if (pLastCharSelected) (*pLastCharSelected)=(HilightEndCol>=LineLen(HilightEndRow));

    if (HilightBegRow==0 && HilightEndRow==(TotalLines-1)) {
        if (HilightType==HILIGHT_LINE || (HilightBegCol==0 && HilightEndCol>=(LineLen(HilightEndRow))-1)) return TRUE;
    }

    if (HilightType==HILIGHT_CHAR && HilightBegRow==0 && HilightBegCol==0
        && TotalLines>0 && LineLen(TotalLines-1)==1
        && HilightEndRow==(TotalLines-2) && HilightEndCol>=LineLen(HilightEndRow)) return TRUE;

    return FALSE;
}

/******************************************************************************
    DeleteLineBlock:
    Delete a hilighted line block. A TRUE value of the argument will force
    the editor to repaint the screen after the deletion.
*******************************************************************************/
DeleteLineBlock(PTERWND w,BOOL disp)
{

    if (TotalLines<=1) {
        HilightType=HILIGHT_OFF;
        StretchHilight=FALSE;
        return TRUE;
    }

    if (!NormalizeBlock(w)) return TRUE;  // normalize beginning and ending of the block

    SaveUndo(w,HilightBegRow,0,HilightEndRow,LineLen(HilightEndRow),'D');  // save undo

    MoveLineArrays(w,HilightBegRow,HilightEndRow-HilightBegRow+1,'D'); // delete the lines

    if (TotalLines==0) {
        TotalLines=1;
        InitLine(w,0);
    }

    // Turn off page header/footer when the entire text is being deleted
    if (TotalLines==1) EditPageHdrFtr=ViewPageHdrFtr=FALSE;

    if (!disp) return TRUE;   // don't rePaintFlag the screen

    CurLine=HilightBegRow;
    if (CurLine>=TotalLines) CurLine--;

    if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
        BeginLine=CurLine-WinHeight/2;  // position the current line at center
        if (BeginLine<0) BeginLine=0;
    }
    CurRow=CurLine-BeginLine;
    HilightType=HILIGHT_OFF;
    StretchHilight=FALSE;

    PaintFlag=PAINT_WIN;      // repaint the whole window
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    CopyLineBlock:
    copy the highlighted line block.  When the 'delete' argument is true,
    the original block is deleted.
******************************************************************************/
CopyLineBlock(PTERWND w,BOOL delete)
{
    int  i;
    long blines;

    if (HilightType==HILIGHT_OFF || TerArg.WordWrap) return TRUE;
    if (!NormalizeBlock(w)) return TRUE;

    if (!CheckLineLimit(w,TotalLines+HilightEndRow-HilightBegRow+1)) {
       PrintError(w,MSG_MAX_LINES_EXCEEDED,MsgString[MSG_ERR_BLOCK_SIZE]);
       return TRUE;
    }

    blines=HilightEndRow-HilightBegRow+1;         // lines in the block
    if (CurLine>=HilightBegRow && CurLine<=HilightEndRow) CurLine=HilightEndRow+1;else CurLine++;

    SaveUndo(w,CurLine,0,CurLine+blines-1,LineLen(CurLine+blines-1)-1,'I');  // save undo

    MoveLineArrays(w,CurLine,blines,'B');           // make space for additional lines before the current line

    if (CurLine<=HilightBegRow) {HilightBegRow+=blines;HilightEndRow+=blines;} // lines got displaced

    for (i=0;i<blines;i++) {
       LineLen(CurLine+i)=0;
       LineAlloc(w,CurLine+i,0,LineLen(HilightBegRow+i)); // allocated line spaces

       MoveCharInfo(w,HilightBegRow+i,0,CurLine+i,0,LineLen(HilightBegRow+i));

       pfmt(CurLine+i)=pfmt(HilightBegRow+i);
    }

    if (delete) {                                 // delete the original block
       DeleteLineBlock(w,FALSE);                    // delete w/o repainting
       if (CurLine>=HilightEndRow) CurLine-=blines;
    }

    if (CurLine>=TotalLines) CurLine--;
    if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
      BeginLine=CurLine-WinHeight/2;              // position the current line at center
      if (BeginLine<0) BeginLine=0;
    }

    CurRow=CurLine-BeginLine;

    HilightType=HILIGHT_OFF;                      // disable the hilighting
    StretchHilight=FALSE;

    PaintFlag=PAINT_WIN;                          // refresh the whole window
    PaintTer(w);

    return TRUE;
}

/*******************************************************************************
    DeleteCharBlock:
    Delete a hilighted character block.  The 'disp' argument when false
    suppresses the repainting of the screen.
********************************************************************************/
DeleteCharBlock(PTERWND w,BOOL SetCurPos, BOOL repaint)
{
    int  NewLen,TotalChars,level;
    BOOL all=FALSE,LastCharSelected=false;

    if (HilightType==HILIGHT_OFF) return TRUE;

    if (TerOpFlags&TOFLAG_IN_MERGE_PRINT) goto DEL_TEXT;

    // check if a table of content is being deleted
    if (!(TerFlags&TFLAG_UNPROTECTED_DEL)) {
       int BegFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
       int EndFont=GetPrevCfmt(w,HilightEndRow,HilightEndCol);
       if (TerFont[BegFont].FieldId==FIELD_TOC && TerFont[EndFont].FieldId==FIELD_TOC) {
          if (IDNO==(MessageBox(hTerWnd,MsgString[MSG_DEL_TOC]," ",MB_YESNO))) return FALSE;
       }
    } 


    if (False(TerOpFlags2&TOFLAG2_NO_NORMALIZE)) {
       if (!NormalizeBlock(w)) return TRUE;
       if (!NormalizeForFootnote(w)) return TRUE;    // make sure that footnotes are fully contained
    }

      // restrict cell marker selction if requested
    level=MinTableLevel(w,HilightBegRow,HilightEndRow);
    if (TerFlags3&TFLAG3_NO_FULL_CELL_COPY && cid(HilightBegRow)>0
        && LevelCell(w,level,HilightBegRow)==LevelCell(w,level,HilightEndRow)
        && LineInfo(w,HilightEndRow,INFO_CELL) && HilightEndCol>=LineLen(HilightEndRow)
        && HilightEndCol>0) HilightEndCol--;    // exclude the cell marker character

    all=AllSelected2(w,&LastCharSelected);

    // is block limited to a table
    if (TableHilighted(w)) {
       if (TerOpFlags2&TOFLAG2_DEL_CELL_TEXT) TerDeleteCellText(hTerWnd,SEL_CELLS,FALSE);
       else                                   TerDeleteCells(hTerWnd,SEL_CELLS,FALSE);
       goto END;
    }
    
    if (HilightBegRow==HilightEndRow && HilightEndCol<=HilightBegCol) return true;

    if (TrackChanges && TrackDelBlock(w,HilightBegRow,HilightBegCol,HilightEndRow,HilightEndCol,true,repaint)) return true;
    
    SaveUndo(w,HilightBegRow,HilightBegCol,HilightEndRow,HilightEndCol-1,'D');  // save undo

    DEL_TEXT:

    if (HilightBegRow==HilightEndRow) {        // single hilighted line block
       if (LineLen(HilightBegRow)==0) goto END;
       if (HilightBegCol>=LineLen(HilightEndRow)) goto END;
       if (HilightEndCol>LineLen(HilightEndRow)) HilightEndCol=LineLen(HilightEndRow);
       TotalChars=HilightEndCol-HilightBegCol;

       if (HilightEndCol=LineLen(HilightEndRow) && HilightEndCol>HilightBegCol) 
          ResetLongFlag(LineFlags(HilightEndRow),(LFLAG_PARA|LFLAG_BREAK));    // para-marker deleted

       MoveLineData(w,HilightBegRow,HilightBegCol,TotalChars,'D');

       if (LineLen(HilightBegRow)==0 && (TerArg.WordWrap || !(TerOpFlags&TOFLAG_INSERT_CHAR))) {        // zero length line,  delete it
           MoveLineArrays(w,HilightBegRow,1,'D');
           if (CurLine>HilightBegRow) CurLine--;
       }

       if (TotalLines==1 && LineLen(0)==0) {
          CompressCfmt(w,0);
          cfmt(0).info.type=UNIFORM;
          cfmt(0).info.fmt=DEFAULT_CFMT;
       }

       goto END;
    }

    // add remaining part of the last hilighted line to the first line
    TotalChars=LineLen(HilightEndRow)-HilightEndCol;
    if (TotalChars<0) TotalChars=0;
    NewLen=HilightBegCol+TotalChars;          // new length of the first highlighted line

    LineAlloc(w,HilightBegRow,LineLen(HilightBegRow),NewLen);

    if (NewLen>0 && TotalChars>0) {
       MoveCharInfo(w,HilightEndRow,HilightEndCol,HilightBegRow,HilightBegCol,TotalChars);

       pfmt(HilightBegRow)=pfmt(HilightEndRow);   // transfer the paragraph id
       cid(HilightBegRow)=cid(HilightEndRow);
       fid(HilightBegRow)=fid(HilightEndRow);
       LineFlags(HilightBegRow)=LineFlags(HilightEndRow);
       FreeTabw(w,HilightBegRow);                   // transfer tabw to set the INFO_CELL etc
       tabw(HilightBegRow)=tabw(HilightEndRow);
       tabw(HilightEndRow)=NULL;
    }
    else if (NewLen==0) {
       cfmt(HilightBegRow).info.type=UNIFORM;
       cfmt(HilightBegRow).info.fmt=DEFAULT_CFMT;
    }


    // delete lines after the first line to the last hilighted line
    MoveLineArrays(w,HilightBegRow+1,HilightEndRow-HilightBegRow,'D');

    if (TotalLines==0) {
       TotalLines=1;
       InitLine(w,0);
    }

    END:
    // check highlight line range
    if (HilightBegRow>=TotalLines) {
        HilightBegRow=TotalLines-1;
        HilightBegCol=0;
    }
    if (HilightEndRow>=TotalLines) {
        HilightEndRow=TotalLines-1;
        HilightEndCol=LineLen(HilightEndRow);
    }
    if (CurLine>=TotalLines) CurLine = TotalLines-1;
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
    if (CurCol<0) CurCol=0;

    
    if (all) {
      int SavePfmt=pfmt(0);

      LPBYTE ptr;
      LPWORD fmt;
      EditPageHdrFtr=ViewPageHdrFtr=FALSE;
      InputFontId=-1;
      CurLine=0;
      CurCol=0;
      InitLine(w,0);                           // initialize the first line
      pfmt(0)=LastCharSelected?0:SavePfmt;
      LineAlloc(w,CurLine,0,1);
      ptr=pText(CurLine);
      fmt=OpenCfmt(w,CurLine);
      ptr[0]=ParaChar;
      fmt[0]=DEFAULT_CFMT;
      CloseCfmt(w,CurLine);
    }

    if (SetCurPos) {
       CurLine=HilightBegRow;
       if (CurLine>=TotalLines) CurLine--;
       CurCol=HilightBegCol;
       if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
          BeginLine=CurLine-WinHeight/2;       // position the current line at center
          if (BeginLine<0) BeginLine=0;
       }
       CurRow=CurLine-BeginLine;

       CurCol=HilightBegCol;
    }

    if (CurLine>=TotalLines) {
       CurLine=TotalLines-1;
       CurCol=LineLen(CurLine)-1;
       if (CurCol<0) CurCol=0;
       BeginLine=CurLine-WinHeight/2;       // position the current line at center
       if (BeginLine<0) BeginLine=0;
       CurRow=CurLine-BeginLine;
    } 

    HilightType=HILIGHT_OFF;                // disable the mark
    StretchHilight=FALSE;

    if (!repaint) return TRUE;                  // don't repaint the screen

    PaintFlag=PAINT_WIN;                    // refresh from the text
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    CopyCharBlock:
    Copy a hilighted character block.  The argument 'delete' when false,
    suppresses the deletion of the original block.
*******************************************************************************/
CopyCharBlock(PTERWND w,int delete)
{
    int  i,len,TotalChars,NewLen,OldLen,NewCol,UndoEndCol;
    long l,m,StartCord,EndCord,CurCord,UndoEndRow,blines;
    LPBYTE ptr1,lead1;
    LPWORD fmt1,ct1;
    BYTE line[MAX_WIDTH+1];
    WORD LineFmt[MAX_WIDTH+1],LineCt[MAX_WIDTH+1];
    WORD CurFmt;

    if (HilightType==HILIGHT_OFF || TerArg.WordWrap) return TRUE;

    if (!NormalizeBlock(w)) return TRUE;

    StartCord=RowColToAbs(w,HilightBegRow,HilightBegCol);
    EndCord=RowColToAbs(w,HilightEndRow,HilightEndCol);
    CurCord=RowColToAbs(w,CurLine,CurCol);

    if (CurCord>=StartCord && CurCord<EndCord) goto END;

    if (!CheckLineLimit(w,TotalLines+HilightEndRow-HilightBegRow+1)) {
       PrintError(w,MSG_MAX_LINES_EXCEEDED,MsgString[MSG_ERR_BLOCK_SIZE]);
       return TRUE;
    }

    len=LineLen(CurLine);
    if (CurCol>=len) {                // pad with spaces
       OldLen=LineLen(CurLine);
       LineAlloc(w,CurLine,LineLen(CurLine),CurCol+1);
       ptr1=pText(CurLine);
       fmt1=OpenCfmt(w,CurLine);
       if (len>0) CurFmt=fmt1[len-1];else CurFmt=DEFAULT_CFMT;
       if (TerFont[CurFmt].style&PICT)    CurFmt=DEFAULT_CFMT;

       for (i=OldLen;i<LineLen(CurLine);i++) {
          ptr1[i]=' ';
          fmt1[i]=CurFmt;
       }

       CloseCfmt(w,CurLine);
    }

    //*************** save for undo ***************************************
    CurCord=RowColToAbs(w,CurLine,CurCol);
    AbsToRowCol(w,CurCord+EndCord-StartCord,&UndoEndRow,&UndoEndCol);
    SaveUndo(w,CurLine,CurCol,UndoEndRow,UndoEndCol,'I');

    if (HilightBegRow==HilightEndRow) {            // single hilighted line block
       if (LineLen(HilightBegRow)==0) goto END;
       if (HilightEndCol>LineLen(HilightEndRow)) HilightEndCol=LineLen(HilightEndRow);
       if (HilightBegCol>HilightEndCol) HilightBegCol=HilightEndCol;
       TotalChars=HilightEndCol-HilightBegCol;

       OldLen=LineLen(CurLine);
       NewLen=OldLen+TotalChars;                   // adjusted new length of the line

       ptr1=pText(HilightBegRow);
       OpenCharInfo(w,HilightBegRow,&fmt1,&ct1);
       if (mbcs) lead1=OpenLead(w,HilightBegRow);

       FarMove(&ptr1[HilightBegCol],line,TotalChars);      // save the characters
       FarMove(&fmt1[HilightBegCol],LineFmt,sizeof(WORD)*TotalChars);  // save the formats
       FarMove(&ct1[HilightBegCol],LineCt,sizeof(WORD)*TotalChars);  // save the formats
       if (mbcs) FarMove(&lead1[HilightBegCol],LineLead,sizeof(BYTE)*TotalChars);  // save the formats

       CloseCharInfo(w,HilightBegRow);
       if (mbcs) CloseLead(w,HilightBegRow);

       LineAlloc(w,CurLine,OldLen,NewLen);

       MoveCharInfo(w,CurLine,CurCol,CurLine,CurCol+TotalChars,OldLen-CurCol);

       ptr1=pText(CurLine);                 // copy the characters
       FarMove(line,&ptr1[CurCol],TotalChars);

       OpenCharInfo(w,CurLine,&fmt1,&ct1);                     // copy the formats
       if (mbcs) lead1=OpenLead(w,CurLine);

       FarMove(LineFmt,&fmt1[CurCol],sizeof(WORD)*TotalChars); // copy the saved
       FarMove(LineCt,&ct1[CurCol],sizeof(WORD)*TotalChars); // copy the saved
       FarMove(LineLead,&lead1[CurCol],sizeof(BYTE)*TotalChars); // copy the saved

       CloseCharInfo(w,CurLine);
       if (mbcs) CloseLead(w,CurLine);

       if (CurLine==HilightBegRow && CurCol<HilightBegCol) {
          HilightBegCol+=TotalChars;
          HilightEndCol+=TotalChars;
       }
       if (delete) DeleteCharBlock(w,FALSE,FALSE);         // delete block without re-PaintFlaging the screen
       goto END;
    }

    //***** break current line at the current column ************
    if (CurCol<LineLen(CurLine)) {
       MoveLineArrays(w,CurLine,1,'A');             // insert a line after the current line

       LineAlloc(w,CurLine+1,0,LineLen(CurLine)-CurCol);

       MoveCharInfo(w,CurLine,CurCol,CurLine+1,0,LineLen(CurLine)-CurCol);

       LineAlloc(w,CurLine,LineLen(CurLine),CurCol);// shrink the current line

       if (CurLine<=HilightBegRow) {          // adjust the highlighted block coordinates
          if (CurLine==HilightBegRow) {       // adjust the beginning column of the highlighted block
             HilightBegCol=HilightBegCol-CurCol;
             if (HilightBegCol<0) HilightBegCol=0;
          }
          HilightBegRow++;                    // adjust the highlighted block coordinates
          HilightEndRow++;
       }
    }

    //* append hilighted portions of first line to the current line
    if (HilightBegCol<LineLen(HilightBegRow)) {
       TotalChars=LineLen(HilightBegRow)-HilightBegCol;
       OldLen=LineLen(CurLine);
       LineAlloc(w,CurLine,OldLen,OldLen+TotalChars);

       MoveCharInfo(w,HilightBegRow,HilightBegCol,CurLine,OldLen,TotalChars);
    }

    //* insert hilighted portions of last line to the line after current line
    TotalChars=HilightEndCol;
    if (TotalChars>LineLen(HilightEndRow)) TotalChars=LineLen(HilightEndRow);
    OldLen=LineLen(CurLine+1);
    LineAlloc(w,CurLine+1,OldLen,OldLen+TotalChars);

    NewCol=HilightEndCol;
    if (NewCol>LineLen(HilightEndRow)) NewCol=LineLen(HilightEndRow);

    MoveCharInfo(w,CurLine+1,0,CurLine+1,NewCol,OldLen);
    MoveCharInfo(w,HilightEndRow,0,CurLine+1,0,TotalChars);

    //*** copy the remainning hilighted lines **********************
    if (HilightEndRow-HilightBegRow>1) {
       blines=HilightEndRow-HilightBegRow-1;
       MoveLineArrays(w,CurLine,blines,'A');        // insert blines after the current line

       if (CurLine<HilightBegRow) {HilightBegRow+=blines;HilightEndRow+=blines;}

       m=HilightBegRow+1;
       for (l=CurLine+1;l<CurLine+1+blines;l++,m++) { // copy the lines
           LineAlloc(w,l,0,LineLen(m));

           MoveCharInfo(w,m,0,l,0,LineLen(m));

           pfmt(l)=pfmt(m);
       }
    }

    if (delete) {
       DeleteCharBlock(w,FALSE,FALSE);                    // delete the block without repainting the screen
       if (CurLine>=HilightEndRow) CurLine=CurLine-(HilightEndRow-HilightBegRow);
    }

    END:
    if (CurLine>=TotalLines) CurLine--;
    if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
        BeginLine=CurLine-WinHeight/2;            // position the current line at center
        if (BeginLine<0) BeginLine=0;
    }

    CurRow=CurLine-BeginLine;

    HilightType=HILIGHT_OFF;                     // disable the hilighting
    StretchHilight=FALSE;

    PaintFlag=PAINT_WIN;                         // refresh from the text
    PaintTer(w);
    return TRUE;
}

/****************************************************************************
    TerGetTextSel:
    This function extracts the selected text.  The function
    returns the global handle containing the text and also returns the
    length of the buffer via a parameter 'BufLen' parameter
****************************************************************************/
HGLOBAL WINAPI _export TerGetTextSel(HWND hWnd,LPLONG len)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL; // get the pointer to window data

    if (HilightType==HILIGHT_OFF) return NULL;  // no text selected

    if (!NormalizeBlock(w)) return NULL;  // normalize beginning and ending of the block

    if (CopyToClipboard(w,ID_COPY,FALSE)) {
       (*len)=DlgLong1;            // length returned in DlgLong1
       GlobalFree((HGLOBAL)DlgLong2);       // free the unicode block
       return (HGLOBAL)(DlgLong);  // handle returned in DlgLong
    }
    else return FALSE;
}

/****************************************************************************
    TerGetTextSelU:
    This function extracts the selected text.  The function
    returns the global handle containing the text and also returns the
    length of the buffer via a parameter 'BufLen' parameter
****************************************************************************/
HGLOBAL WINAPI _export TerGetTextSelU(HWND hWnd,LPLONG len)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL; // get the pointer to window data

    if (HilightType==HILIGHT_OFF) return NULL;  // no text selected

    if (!NormalizeBlock(w)) return NULL;  // normalize beginning and ending of the block

    if (CopyToClipboard(w,ID_COPY,FALSE)) {
       (*len)=DlgLong3;            // length returned in DlgLong3
       GlobalFree((HGLOBAL)DlgLong);        // free the ascii block
       return (HGLOBAL)(DlgLong2); // handle returned in DlgLong2
    }
    else return FALSE;
}

/*******************************************************************************
    CopyToClipboard:
    Cut or copy a line or character block to the clipboard.  A CR/LF pair is copied
    for everyline. The data to the clipboard is given in the CF_TEXT and
    TerClipFormat formats.  The TerClipFormat contains the text as well as the
    formatting information. When the ToBuf is TRUE, the data is returned
    DlgLong (handle) and DlgLong1 (length) variables.
*******************************************************************************/
CopyToClipboard(PTERWND w,WPARAM wParam, BOOL ToCB)
{
    long l,ClipSize=0,index=0,UIndex=0,PrevLine;
    HGLOBAL hTextClip,hUTextClip;
    BYTE huge *pClpText;
    WORD huge *pClpUText;

    // check if protected text is being cut top clipboard
    if (IsProtected(w,TRUE,(wParam==ID_CUT))) return FALSE;

    if (HilightType==HILIGHT_OFF) return FALSE;
    if (!NormalizeBlock(w)) return FALSE;
    if (!NormalizeForFootnote(w)) return FALSE;    // make sure that footnotes are fully contained

    if (ToCB && !OpenClipboard(hTerWnd)) {    // open clipboard
        return PrintError(w,MSG_CLIPBOARD_BUSY,"");
    }

    if (HilightType==HILIGHT_LINE) {      // set beginning and ending columns
        HilightBegCol=0;
        HilightEndCol=LineLen(HilightEndRow);
        if (HilightEndCol<0) HilightEndCol=0;
    }

    // allocate clipboard buffer for the text format
    for (l=HilightBegRow;l<=HilightEndRow;l++) {
       ClipSize=ClipSize+2*LineLen(l)+2; // roughly size the clipboard, double to be able to convert widechar to multibyte
       if (mbcs && pLead(l)) ClipSize+=LineLen(l);    // size for the lead bytes
    }

    ClipSize++;                           // increment to store the NULL terminater

    // buffer for ansi text
    if (NULL==(hTextClip=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,ClipSize))  // allocate memory for the format data in CF_TEXT format
      ||NULL==(pClpText=GlobalLock(hTextClip)) ) {
        PrintError(w,MSG_OUT_OF_MEM,"CopyToClipboard");
        if (ToCB) CloseClipboard();
        return FALSE;
    }

    // buffer for unicode text
    if (NULL==(hUTextClip=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,2*ClipSize))  // allocate memory for the format data in CF_TEXT format
      ||NULL==(pClpUText=GlobalLock(hUTextClip)) ) {
        PrintError(w,MSG_OUT_OF_MEM,"CopyToClipboard");
        if (ToCB) CloseClipboard();
        return FALSE;
    }

    // Proces a single line buffer
    if (HilightBegRow==HilightEndRow) {   // single line block
        CopyLineToClipBuf(w,HilightBegRow,HilightBegCol,HilightEndCol-1,pClpText,NULL,(LPDWORD)&index);
        CopyLineToClipBuf(w,HilightBegRow,HilightBegCol,HilightEndCol-1,NULL,pClpUText,(LPDWORD)&UIndex);
        goto CLOSE;                      // close the clipboard
    }

    //*************** handle the first line of the multiline blocks **********
    CopyLineToClipBuf(w,HilightBegRow,HilightBegCol,LineLen(HilightBegRow)-1,pClpText,NULL,(LPDWORD)&index);
    CopyLineToClipBuf(w,HilightBegRow,HilightBegCol,LineLen(HilightBegRow)-1,NULL,pClpUText,(LPDWORD)&UIndex);

    //*************** handle middle lines of the multiline blocks ************
    PrevLine=HilightBegRow;
    for (l=HilightBegRow+1;l<HilightEndRow;l++) {
       if (!LineSelected(w,l)) continue;

       // check for row break
       if (cid(l) && cid(PrevLine)) {
          int PrevCell=cid(PrevLine);
          int CurCell=cid(l);
          if (cell[CurCell].row!=cell[PrevCell].row && !LineInfo(w,PrevLine,INFO_ROW)) {  // write row break if not alread written
             index=AddCrLf(w,index,pClpText,NULL,NULL,NULL);  // add cr/lf
             UIndex=AddCrLf(w,UIndex,NULL,NULL,NULL,pClpUText);  // add cr/lf
          }
       }
        
       CopyLineToClipBuf(w,l,0,LineLen(l)-1,pClpText,NULL,(LPDWORD)&index);
       CopyLineToClipBuf(w,l,0,LineLen(l)-1,NULL,pClpUText,(LPDWORD)&UIndex);
       PrevLine=l;
    }

    //*************** handle the last line of the multiline blocks ************
    CopyLineToClipBuf(w,HilightEndRow,0,HilightEndCol-1,pClpText,NULL,(LPDWORD)&index);
    CopyLineToClipBuf(w,HilightEndRow,0,HilightEndCol-1,NULL,pClpUText,(LPDWORD)&UIndex);


    CLOSE:
    pClpText[index]=0;                      // NULL terminate the clipboard data
    pClpUText[UIndex]=0;                    // NULL terminate the clipboard data
    
    GlobalUnlock(hTextClip);
    GlobalUnlock(hUTextClip);

    if (ToCB) {
       EmptyClipboard();
       SetClipboardData(CF_TEXT,hTextClip);
       if (True(TerFlags5&TFLAG5_INPUT_TO_UNICODE)) SetClipboardData(CF_UNICODETEXT,hUTextClip);
       else                                         GlobalFree(hUTextClip);
       
       RtfWrite(w,RTF_CB,NULL);             // write in rich  text format

       CloseClipboard();

       // delete the block if 'cut' option selected
       if (wParam==ID_CUT) TerDeleteBlock(hTerWnd,TRUE);

       PaintFlag=PAINT_WIN;                 // erase the highlighting
       PaintTer(w);
    }
    else {
       DlgLong=(DWORD)(UINT)hTextClip;
       DlgLong1=index+1;
       DlgLong2=(DWORD)(UINT)hUTextClip;
       DlgLong3=(UIndex+1)*sizeof(WORD);
    }

    return TRUE;
}

/*******************************************************************************
    CopyLineToClipBuf:
    Copy a line to clipboard buffer in text format. 'type' can be 'T' for regular text output
    and 'U' for unicode output
*******************************************************************************/
CopyLineToClipBuf(PTERWND w,long line,int BegCol,int EndCol,BYTE huge *pClpText,WORD huge *pClpUText,LPDWORD StartIndex)
{
    LPBYTE ptr,lead;
    LPWORD fmt;
    int i,len,CurFont;
    DWORD index=(*StartIndex),SaveIndex;
    BOOL CopyEntireLine=FALSE;
    LPBYTE string=NULL;
    BYTE CurChar;
    WORD LastCharW;
    BOOL ansi=(pClpText!=NULL);

    if (mbcs && NULL==(string=OurAlloc(MAX_WIDTH*2+2))) return FALSE;

    if (LineInfo(w,line,INFO_ROW)) return TRUE;   // row break line ignored

    // can we copy the entire line
    if (cfmt(line).info.type==UNIFORM) {
       CurFont=cfmt(line).info.fmt;
       if (!HiddenText(w,CurFont)) CopyEntireLine=TRUE;
    }

    ptr=pText(line);
    lead=pLead(line);

    // copy to the buffer
    if (CopyEntireLine) {
      len=EndCol-BegCol+1;
      if (len>0) {
         if (mbcs && lead) {
            len=TerMakeMbcs(hTerWnd,string,&(ptr[BegCol]),&(lead[BegCol]),len);
            if (ansi) FarMove(string,&(pClpText[index]),len);
            else      for (i=0;i<len;i++) pClpUText[index+i]=string[i];
         }
         else {
            if (ansi) FarMove(&(ptr[BegCol]),&(pClpText[index]),len);
            else {
               for (i=0;i<len;i++) {
                  if (TerFont[CurFont].UcBase==0) 
                       pClpUText[index+i]=ptr[BegCol+i];
                  else pClpUText[index+i]=MakeUnicode(w,TerFont[CurFont].UcBase,ptr[BegCol+i]);
               }
            }
         }
      }
    }
    else {
      fmt=OpenCfmt(w,line);
      len=0;
      CurFont=0;
      for (i=BegCol;i<=EndCol && i<LineLen(line);i++) {
         if ((int)fmt[i]==CurFont || (!HiddenText(w,fmt[i]) && !(TerFont[fmt[i]].style&PICT))) {  // use CurFont to save call to HiddenText
            if (mbcs && lead && lead[i]) {
               if (ansi) pClpText[index+len]=lead[i];
               else      pClpUText[index+len]=lead[i];
               len++;
            }

            CurChar=ptr[i];
            if (TerFont[fmt[i]].UcBase) {
              WORD uc=MakeUnicode(w,TerFont[fmt[i]].UcBase,CurChar);
              if (!ansi) pClpUText[index+len]=uc;

              if (ansi) {
                if (uc<=255) CurChar=(BYTE)uc;
                else {                            // convert to mulitbyte
                   BYTE lead,chr;
                   if (UnicodeToMultiByte(w,uc,&lead,&chr,fmt[i])) {
                      if (lead!=0) {
                        pClpText[index+len]=lead;
                        len++;
                      }
                      CurChar=chr;
                   }
                   else CurChar='?'; 
                }
              }   
            }
            else if (CurChar==NBSPACE_CHAR) CurChar=' ';
            
            if (ansi) pClpText[index+len]=CurChar;
            else if (TerFont[fmt[i]].UcBase==0) pClpUText[index+len]=CurChar;

            len++;
            CurFont=fmt[i];
         }
      }
      CloseCfmt(w,line);
    }


    index=index+len;

    if (TerArg.WordWrap) {
       SaveIndex=index;
       
       LastCharW=0;
       if (index>0) {
         if (ansi) LastCharW=pClpText[index-1];
         else      LastCharW=pClpUText[index-1];
       } 
       if (index>0 && (LastCharW==SECT_CHAR || LastCharW==HDR_CHAR || LastCharW==FTR_CHAR
                       || LastCharW==FHDR_CHAR || LastCharW==FFTR_CHAR)) index--;       // discard the section break characte

       if (index>0) {
         if (ansi) LastCharW=pClpText[index-1];
         else      LastCharW=pClpUText[index-1];
       } 
       if (index>0 && (LastCharW==ParaChar || LastCharW==CellChar || LastCharW==LINE_CHAR))  {
          index--;       // discard the para break characte
          if (LastCharW==CellChar && (line+1)<TotalLines && !LineInfo(w,line+1,INFO_ROW)) {
             if (ansi) pClpText[index]=TAB;        // replace cell char by tab
             else      pClpUText[index]=TAB;

             index++;
          }
       }

       if (index<SaveIndex) index=AddCrLf(w,index,pClpText,NULL,NULL,pClpUText);  // add cr/lf
    }
    else index=AddCrLf(w,index,pClpText,NULL,NULL,pClpUText);  // add cr/lf

    if (ansi) pClpText[index]=0;                  // NULL terminate the clipboard data
    else      pClpUText[index]=0;

    (*StartIndex)=index;

    if (string) OurFree(string);

    return TRUE;
}


/*******************************************************************************
    DragText:
    Drag text to a new location.
*******************************************************************************/
DragText(PTERWND w,long lParam)
{
    long HilightBeg,HilightEnd,MousePos,TextLen;
    HGLOBAL hText=NULL;
    LPBYTE pText;
    BOOL CtrlPressed=GetKeyState(VK_CONTROL)&0x8000;

    if (HilightType==HILIGHT_OFF) goto END;

    TerMousePos(w,lParam,TRUE);          // get mouse position in MouseCol and MouseLine variables
    if (MouseOverShoot!=' ') goto END;   // outside the text box

    NormalizeBlock(w);

    HilightBeg=RowColToAbs(w,HilightBegRow,HilightBegCol);
    HilightEnd=RowColToAbs(w,HilightEndRow,HilightEndCol);
    MousePos=RowColToAbs(w,MouseLine,MouseCol);
    if (HilightBeg>HilightEnd) SwapLongs(&HilightBeg,&HilightEnd);

    if (MousePos>=HilightBeg && MousePos<HilightEnd) goto END;  // nowhere to drag


    // check for protected text
    if (TerFlags4&TFLAG4_NO_DRAG_PROT_TEXT && IsProtected(w,TRUE,TRUE)) goto END;


    // copy the text to a buffer
    if (NULL==(hText=TerGetRtfSel(hTerWnd,&TextLen))) goto END;


    // position the cursor at the new location and insert the buffer
    CurLine=MouseLine;
    CurCol=MouseCol;

    // check if the cursor is on the protected text
    if (!CanInsert(w,CurLine,CurCol)) {
       MessageBeep(0);
       goto END;
    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && !CtrlPressed) {
       DWORD SaveFlags=TerFlags;
       if (TerFlags4&TFLAG4_FULL_DRAG_PROT_TEXT) TerFlags|=TFLAG_UNPROTECTED_DEL;
       
       if (TerDeleteBlock(hTerWnd,TRUE)) {
           UndoRef--;                   // restore undo reference
           // adjust the cursor position
           if (MousePos>=HilightEnd) MousePos-=(HilightEnd-HilightBeg);
           AbsToRowCol(w,MousePos,&CurLine,&CurCol);
       }
       
       TerFlags=SaveFlags;
    }

    SendMessageToParent(w,TER_BEFORE_DROP,(WPARAM)hTerWnd,0L,FALSE);

    // insert the text
    pText=GlobalLock(hText);
    InsertRtfBuf(hTerWnd,pText,TextLen,CurLine,CurCol,FALSE);
    GlobalUnlock(hText);

    // cursor positioning
    if (CurLine>=TotalLines) CurLine=TotalLines-1;
    if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) BeginLine=CurLine-WinHeight/2;            // position the current line at center

    if (BeginLine<0) BeginLine=0;
    CurRow=CurLine-BeginLine;
    if (CurRow<0) CurRow=0;

    SendMessageToParent(w,TER_DROP,(WPARAM)hTerWnd,0L,FALSE);

    END:
    if (hText) GlobalFree(hText);

    HilightType=HILIGHT_OFF;
    DraggingText=FALSE;

    PaintFlag=PAINT_WIN;
    PaintTer(w);

    return TRUE;
}

/*******************************************************************************
    OleDragText:
    Do drag using OLE DropSource/DataObject interfaces
*******************************************************************************/
OleDragText(PTERWND w)
{
    LPDROPSOURCE pDropSource;
    LPDATAOBJECT pDataObject;
    HRESULT hr;
    DWORD effect=0,allow;
    int SaveModified=TerArg.modified;

    pDropSource=CreateDropSource(w);
    pDataObject=CreateDataObject(w);
 
    InOleDrag=true;

    allow=DROPEFFECT_COPY;
    if (!TerArg.ReadOnly) allow|=DROPEFFECT_MOVE;
    hr=DoDragDrop(pDataObject,pDropSource,allow,&effect);
    InOleDrag=false;

    if (hr==DRAGDROP_S_DROP && True(effect&DROPEFFECT_MOVE) && HilightType!=HILIGHT_OFF) {    // drop successful - delete the original text
       DWORD SaveFlags=TerFlags;
       if (TerFlags4&TFLAG4_FULL_DRAG_PROT_TEXT) TerFlags|=TFLAG_UNPROTECTED_DEL;
       
       if (TerArg.modified>SaveModified) UndoRef--;     // drag drop in the same window, connect undo with the inserted text
       TerDeleteBlock(hTerWnd,TRUE);
       TerFlags=SaveFlags;
    } 

    DraggingText=false;

    // release the interfaces
    pDropSource->lpVtbl->Release(pDropSource);
    pDataObject->lpVtbl->Release(pDataObject);

    return TRUE;
}


/*******************************************************************************
    SetDragCaret:
    Set drag caret.
*******************************************************************************/
SetDragCaret(PTERWND w,long lParam)
{
   TerMousePos(w,lParam,FALSE);

   // check if the cursor is on the protected text
   if (!CanInsert(w,MouseLine,MouseCol)) {
      MessageBeep(0);
      return TRUE;
   }

   // position the cursor at the new location and insert the buffer
   CurLine=MouseLine;
   CurCol=MouseCol;

   if (!CaretEnabled) InitCaret(w);
   OurSetCaretPos(w);

   return TRUE;
}
 
/*******************************************************************************
    CopyFromClipboard:
    If data exists in the buffer in the text,native or rtf formats,  this data
    is pasted at the current cursor position.
*******************************************************************************/
CopyFromClipboard(PTERWND w,UINT format)
{
    HANDLE hMem,hInfo;
    BYTE huge *pClip,huge *pClpText;
    struct StrClipInfo far *pInfo;
    BOOL RtfFormat=FALSE;
    UINT ClipFormat=0;


    // Paste picture if RTF format not available
    if (format==0 && !IsClipboardFormatAvailable(RtfClipFormat)) {
       if      (IsClipboardFormatAvailable(CF_METAFILEPICT)) ClipFormat=CF_METAFILEPICT;
       else if (Win32 && IsClipboardFormatAvailable(CfEnhMetafile)) ClipFormat=CfEnhMetafile;
       else if (IsClipboardFormatAvailable(CF_DIB))          ClipFormat=CF_DIB;
       else if (IsClipboardFormatAvailable(CF_BITMAP))       ClipFormat=CF_BITMAP;
       if (ClipFormat) return TerPastePicture(hTerWnd,ClipFormat,NULL,0,ALIGN_BOT,TRUE);
    }

    // check if the cursor is on the protected text
    if (!CanInsert(w,CurLine,CurCol)) {
       MessageBeep(0);
       return TRUE;
    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                   // restore undo reference

    // open the clipboard
    if (!OpenClipboard(hTerWnd)) {        // open buffer
        PrintError(w,MSG_CLIPBOARD_BUSY,"");
        return TRUE;
    }

    // select the clipboard format
    hMem=NULL;
    if  (format==CF_TEXT)  {
      if (True(TerFlags6&TFLAG6_PASTE_UNICODE)) hMem=GetClipboardData(CF_UNICODETEXT);  // Try for unicode format over plain text
      if (hMem)  format=CF_UNICODETEXT;
      else       hMem=GetClipboardData(CF_TEXT);
    } 
    else if (format==CF_UNICODETEXT) hMem=GetClipboardData(CF_UNICODETEXT);
    else if (format==RtfClipFormat) {
       hMem=GetClipboardData(RtfClipFormat);
       RtfFormat=TRUE;
    }
    else {
       if   (NULL!=(hMem=GetClipboardData(RtfClipFormat))) RtfFormat=TRUE;
       else {
          if      (NULL!=(hMem=GetClipboardData(CF_UNICODETEXT))) format=CF_UNICODETEXT;
          else if (NULL!=(hMem=GetClipboardData(CF_TEXT))) format=CF_TEXT;
          else    hMem=NULL;
       }
    }

    if (!hMem) {
        PrintError(w,MSG_CLIPBOARD_NO_FMT,"");
        CloseClipboard();
        return TRUE;
    }

    if (NULL==(pClip=GlobalLock(hMem))) {
        PrintError(w,MSG_OUT_OF_MEM,"CopyFromClipboard");
        CloseClipboard();
        return TRUE;
    }

    // retrieve the clipboard data information
    ClipTblLevel=-1;      // table level not found yet
    ClipEmbTable=FALSE;
    if (cid(CurLine)) ClipTblLevel=cell[cid(CurLine)].level;  // default level

    if (NULL!=(hInfo=GetClipboardData(SSClipInfo))
      && NULL!=(pInfo=GlobalLock(hInfo)) ) {
       if (pInfo->size==sizeof(struct StrClipInfo)) {  // version matching
          ClipTblLevel=pInfo->TblLevel;
          ClipEmbTable=pInfo->EmbTable;
       }
       GlobalUnlock(hInfo);
    }


    // Use rtf format if available or selected
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine);
    if (CurCol<0) CurCol=0;

    if (RtfFormat && !(TerFlags3&TFLAG3_PASTE_RTF_CODE)) {
       TerOpFlags2|=TOFLAG2_FROM_CB;
       InsertRtfBuf(hTerWnd,pClip,GlobalSize(hMem),CurLine,CurCol,FALSE);
       ResetLongFlag(TerOpFlags2,TOFLAG2_FROM_CB);
    }
    else {
       BOOL UnicodeDone,IsUnicode;
       int i,len;
       LPWORD pUniText,pUcBase;

       // extract text pointer
       InputFontId=GetEffectiveCfmt(w);

       IsUnicode=false;
       UnicodeDone=false;
       pUniText=(LPVOID)pClip;
       if (format==CF_UNICODETEXT) {
          for (i=0;pUniText[i]!=0;i++) if (pUniText[i]>255) IsUnicode=true;
          
          if (!IsUnicode) {    // check if plain text available
             HGLOBAL hTextMem;
             if (NULL!=(hTextMem=GetClipboardData(CF_TEXT))
                 && (NULL!=(pClip=GlobalLock(hTextMem))) ){
                GlobalUnlock(hMem);   // unlock the unicode hMem
                hMem=hTextMem;        // start using the regular text now
                format=CF_TEXT;
             }
             else IsUnicode=true;    // regular text not available, so use the unicode instead
          }
           
          if (IsUnicode) {
             len=i;   // length of unicode characters

             pUcBase=MemAlloc(sizeof(WORD)*(len+1));
             pClpText=MemAlloc(sizeof(BYTE)*(len+1));
             for (i=0;i<=len;i++) CrackUnicode(w,pUniText[i],&(pUcBase[i]),&(pClpText[i])); // crach unicode
             InsertBuffer(w,pClpText,pUcBase,NULL,NULL,NULL,FALSE);      // insert the clipboard data at current position
             UnicodeDone=true;

             MemFree(pUcBase);
             MemFree(pClpText);
             pClpText=NULL;
          } 
       }
        
       pClpText=pClip;
       if (!UnicodeDone) InsertBuffer(w,pClpText,NULL,NULL,NULL,NULL,FALSE);      // insert regular text.
    }

    GlobalUnlock(hMem);
    CloseClipboard();

    ClipTblLevel=1;             // reset the table level to unknown
    ClipEmbTable=TRUE;

    if (CurLine>=TotalLines) CurLine--;
    if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
        BeginLine=CurLine-WinHeight/2;            // position the current line at center
        if (BeginLine<0) BeginLine=0;
    }

    CurRow=CurLine-BeginLine;

    HilightType=HILIGHT_OFF;                     // disable the hilighting
    StretchHilight=FALSE;

    PaintFlag=PAINT_WIN;                         // refresh from the text
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    HideControl:
    Hide a spcified control.
*******************************************************************************/
HideControl(PTERWND w, int ctl)
{
    int FirstCtl,LastCtl;
    BOOL hidden=FALSE;

    FirstCtl=LastCtl=ctl;
    if (ctl<0) {
       FirstCtl=0;
       LastCtl=TotalFonts-1;
    }

    for (ctl=FirstCtl;ctl<=LastCtl;ctl++) {
       if (!TerFont[ctl].InUse || !(TerFont[ctl].style&PICT)) continue;

       if (TerFont[ctl].ObjectType==OBJ_OCX && !(TerFont[ctl].flags&FFLAG_OCX_SRV_ERROR)) {
          HideOcx(w,ctl);
          hidden=TRUE;                  // atleast one ocx was hidden
       }
       else if (hBufDC && IsControl(w,ctl)) {
          ShowWindow(TerFont[ctl].hWnd,SW_HIDE);
          TerFont[ctl].flags=ResetUintFlag(TerFont[ctl].flags,FFLAG_CTL_VISIBLE);
          hidden=TRUE;
       }
    }

    return TRUE;
}

/******************************************************************************
    TerGetControlId:
    This function retries the control id for a picture id (if the picture
    id represents a control).  It returns -1 if unsuccessful.
*******************************************************************************/
int WINAPI _export TerGetControlId(HWND hWnd,int pict)
{
    PTERWND w;
    struct StrControl far *pInfo;
    int id=-1;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (pict<0 || pict>=TotalFonts || !IsControl(w,pict)) return -1;

    pInfo=(LPVOID)TerFont[pict].pInfo;
    id=(int)(pInfo->id);

    return id;
}

/******************************************************************************
    TerXlateControlId:
    This function returns the picture id for a control id of an embedded control.
    It returns -1 if unsuccessful.
*******************************************************************************/
int WINAPI _export TerXlateControlId(HWND hWnd,int id)
{
    PTERWND w;
    struct StrControl far *pInfo;
    int pict,CurId;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    for (pict=0;pict<TotalFonts;pict++) {
       if (!IsControl(w,pict)) continue;
       pInfo=(LPVOID)TerFont[pict].pInfo;
       CurId=(int)(pInfo->id);
       if (CurId==id) return pict;
    }
    return -1;
}

/******************************************************************************
    TerPictLinkName:
    Retrieve or set the picture link file name.
*******************************************************************************/
BOOL WINAPI _export TerPictLinkName(HWND hWnd,int pict,BOOL get,LPBYTE name)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (pict<0 || pict>=TotalFonts || !TerFont[pict].InUse || !(TerFont[pict].style&PICT)) return FALSE;

    if (get) {     // retrieve the file name
       if (TerFont[pict].LinkFile) lstrcpy(name,TerFont[pict].LinkFile);
       else name[0]=0;
    }
    else {         // set the new file name
       if (TerFont[pict].LinkFile) {
          OurFree(TerFont[pict].LinkFile);
          TerFont[pict].LinkFile=NULL;
       }
       if (name && lstrlen(name)>0) {
          if (NULL!=(TerFont[pict].LinkFile=OurAlloc(lstrlen(name)+1))) {
             lstrcpy(TerFont[pict].LinkFile,name);
          }
       }
    }

    return TRUE;
}

/******************************************************************************
    TerPictAltInfo:
    Retrieve or set the picture link file name.
*******************************************************************************/
BOOL WINAPI _export TerPictAltInfo(HWND hWnd,int pict,BOOL get,LPBYTE info)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (pict<0 || pict>=TotalFonts || !TerFont[pict].InUse || !(TerFont[pict].style&PICT)) return FALSE;

    if (get) {     // retrieve the file name
       if (TerFont[pict].PictAlt) lstrcpy(info,TerFont[pict].PictAlt);
       else info[0]=0;
    }
    else {         // set the new file info
       if (TerFont[pict].PictAlt) {
          OurFree(TerFont[pict].PictAlt);
          TerFont[pict].PictAlt=NULL;
       }
       if (info && lstrlen(info)>0) {
          if (NULL!=(TerFont[pict].PictAlt=OurAlloc(lstrlen(info)+1))) {
             lstrcpy(TerFont[pict].PictAlt,info);
          }
       }
    }

    return TRUE;
}

/******************************************************************************
    TerSetFontId:
    Set the next font id to use by the font/picture creation functions.
*******************************************************************************/
BOOL WINAPI _export TerSetFontId(HWND hWnd,int FontId)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (FontId<0 || FontId>=TotalFonts || !TerFont[FontId].InUse) return FALSE;  // id not found

    NextFontId=FontId;      // set the next font id to use

    return TRUE;
}

/******************************************************************************
    TerPastePicture:
    Routine to copy a specified picture format from the specified data handle
    or clipboard (if hData equal to 0) at the current cursor location.  The
    hData handle is not freed by this function.
    The 'format' can be one of CF_BITMAP, CF_DIB, CF_METAFILE.  The data in
    the hData must correspond to the specified 'format'.
    If the 'insert' argument is TRUE, the picture is inserted at the
    current cursor position, otherwise the function simply returns the picture
    id.
    If a para frame id is specified (value greater than 0) than the picture is
    inserted within the specified frame.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful, otherwise it returns 0.
*******************************************************************************/
int WINAPI _export TerPastePicture(HWND hWnd,UINT format, HGLOBAL hData, int ParaFID, int align, BOOL insert)
{
    PTERWND w;
    HGLOBAL hClip;
    HGLOBAL hMeta;
    long   PictLine;
    int    pict,CurCfmt,PictCol;
    BOOL   ReplacePicture;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    // position cursor if not on the given frame
    PictLine=CurLine;
    PictCol=CurCol;
    if (ParaFID>0 && fid(PictLine)!=ParaFID) {
       long l;                    // locate the line with the given para frame id
       for (l=0;l<TotalLines;l++) if (fid(l)==ParaFID) break;
       if (l<TotalLines && LineLen(l)>0) {
          PictLine=l;
          PictCol=0;
       }
    }

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,PictLine,PictCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // Determine the clipboard format to process
    if (!format && hData==NULL) {
       if      (IsClipboardFormatAvailable(CF_METAFILEPICT)) format=CF_METAFILEPICT;
       else if (IsClipboardFormatAvailable(CF_DIB))          format=CF_DIB;
       else if (IsClipboardFormatAvailable(CF_BITMAP))       format=CF_BITMAP;
       else if (Win32 && IsClipboardFormatAvailable(CfEnhMetafile)) format=CfEnhMetafile;
    }
    if (format==0) return 0;

    //**** open and access the clipboard ***********************************
    if (!hData && !OpenClipboard(hTerWnd)) {  // open clipboard
        PrintError(w,MSG_CLIPBOARD_BUSY,"");
        return 0;
    }

    ImgDenX=OrigScrResX;     // pixel densities - these variables are set by the image import functions //>>> RZW 3/15/02 - Initialize global variables
    ImgDenY=OrigScrResY;                                                                                //>>> RZW 3/15/02 - Initialize global variables

    // get bitmap data first
    pict=-1;
    if (format==CF_METAFILEPICT) {
       if (hData) hMeta=hData;
       if (hData || NULL!=(hMeta=GetClipboardData(CF_METAFILEPICT))) { // picture in metafile format
         pict=TerCreateMetafile(w,hMeta,-1);          // extract metafile
       }
    }
    #if defined(WIN32)
    else if (format==CfEnhMetafile) {
       if (hData) hMeta=hData;
       if (hData || NULL!=(hMeta=GetClipboardData(CfEnhMetafile))) { // picture in enhanced metafile format
         pict=TerCreateEnhMetafile(w,hMeta,-1);     // extract metafile
       }
    }
    #endif
    else if (format==CF_DIB) {
       if (hData) hClip=hData;
       if (hData || NULL!=(hClip=GetClipboardData(CF_DIB))) {    // picture in DIB format
          ImgDenX=ImgDenY=-1;          // let ExtractDIB function retrieve it from the pInfo structure
          if ((pict=ExtractDIB(w,(HGLOBAL)hClip))==-1) {// create a device independent bitmap
              if (!hData) CloseClipboard();
              return 0;
          }
          if (!DIB2Bitmap(w,hTerDC,pict)) {        // translate DIB to device bitmap
              if (!hData) CloseClipboard();
              return 0;
          }
       }

    }
    else if (format==CF_BITMAP) {
       if (hData) hClip=hData;
       if (hData || NULL!=(hClip=GetClipboardData(CF_BITMAP))) { // picture in bitmap format
          if ((pict=Bitmap2DIB(w,hClip))==-1) {  // create a device independent bitmap
              if (!hData) CloseClipboard();
              return 0;
          }
          if (!DIB2Bitmap(w,hTerDC,pict)) {      // translate DIB to device bitmap
              if (!hData) CloseClipboard();
              return 0;
          }
       }
    }

    if (pict==-1) {
       PrintError(w,MSG_CLIPBOARD_NO_FMT,"TerPastePicture");
       if (!hData) CloseClipboard();
       return 0;
    }

    // set alignment
    TerFont[pict].PictAlign=align;
    if (TerFont[pict].PictAlign!=ALIGN_BOT) {
      if (TerFont[pict].PictAlign==ALIGN_MIDDLE) TerFont[pict].BaseHeight/=2;
      if (TerFont[pict].PictAlign==ALIGN_TOP)    TerFont[pict].BaseHeight=0;
      TerFont[pict].BaseHeight+=(TerFont[0].BaseHeight/2);
      if (TerFont[pict].BaseHeight>TerFont[pict].height) TerFont[pict].BaseHeight=TerFont[pict].height;
    }

    //** End of Copy Operation
    if (!hData) CloseClipboard();

    if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       PaintTer(w);
    }
    else if (insert) {
       CurLine=PictLine;                  // insert object into text
       CurCol=PictCol;
       TerInsertObjectId(hTerWnd,pict,TRUE);      // insert object into current cursor position
       FitPictureInFrame(w,CurLine,FALSE);
    }

    return pict;
}

/*****************************************************************************
    TerGetPictInfo:
    Get assorted picture information.  The 'rect' argument returns the current
    location of the picture on the screen.  This location is relative to the
    top of the screen.  This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerGetPictInfo(HWND hWnd, int pict, UINT far *style, LPRECT rect, LPINT align, LPINT AuxId)
{
    PTERWND w;
    int width,height;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return FALSE;

    // pass values
    if (style) (*style)=(int)TerFont[pict].style;
    if (align) (*align)=TerFont[pict].PictAlign;
    if (AuxId) (*AuxId)=TerFont[pict].AuxId;
    if (rect) {
       rect->left=TerFont[pict].PictX;   // relative to the to of the text box box
       rect->top=TerFont[pict].PictY;
       
       rect->left+=TerWinRect.left;      // relative to the client area 
       rect->top+=TerWinRect.top;      // relative to the client area 
        
       if (TerFont[pict].FrameType==PFRAME_NONE) {
          width=TerFont[pict].CharWidth[PICT_CHAR];
          height=TerFont[pict].height;
       }
       else {
          width=TwipsToScrX(TerFont[pict].PictWidth);
          height=TwipsToScrY(TerFont[pict].PictHeight);
       } 
       rect->right=rect->left+width;
       rect->bottom=rect->top+height;
    }

    return TRUE;
}

/*****************************************************************************
    IsFramePict:
    Returns TRUE if this picture id is framed picture
******************************************************************************/
BOOL IsFramePict(PTERWND w, int pict)
{
    if (pict>=0 && pict<TotalFonts && TerFont[pict].InUse && TerFont[pict].style&PICT && TerFont[pict].FrameType!=PFRAME_NONE) {
       int ParaFID=TerFont[pict].ParaFID;
       if (ParaFID>=0 && ParaFID<TotalParaFrames) return TRUE;
    }

    return FALSE;
}

/*****************************************************************************
    TerGetPictFrame:
    Get the picture frame. The frame type can be one
    of the PFRAME_NONE, PFRAME_LEFT or PFRAME_RIGHT constants
******************************************************************************/
int WINAPI _export TerGetPictFrame(HWND hWnd, int pict)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse) || !(TerFont[pict].style&PICT)) return FALSE;

    return TerFont[pict].FrameType;
}

/*****************************************************************************
    TerSetPictFrame:
    Set the picture frame. 
    This function supportsPFRAME_NONE, PFRAME_LEFT, PFRAME_RIGHT PFRAME_FLOAT types.
******************************************************************************/
int WINAPI _export TerSetPictFrame(HWND hWnd, int pict, int FrameType, BOOL repaint)
{
   return TerSetPictFrame2(hWnd,pict,FrameType,2000,2000,repaint);
}

/*****************************************************************************
    TerSetPictFrame:
    Set the picture frame. The frame type can be one
    of the PFRAME_NONE, PFRAME_LEFT, PFRAME_RIGHT PFRAME_FLOAT constants. The x,y values
    in twips must be specified for the PFRAME_FLOAT type. 
    This function returns the frame id for the picture
******************************************************************************/
int WINAPI _export TerSetPictFrame2(HWND hWnd, int pict, int FrameType, int x, int y, BOOL repaint)
{
    PTERWND w;
    int ParaFID=0,sect,col,FrameY;
    long line,l;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse) || !(TerFont[pict].style&PICT)) return -1;
    if (FrameType!=PFRAME_NONE && FrameType!=PFRAME_LEFT && FrameType!=PFRAME_RIGHT && FrameType!=PFRAME_FLOAT) return -1;

    if (FrameType==PFRAME_NONE) {      // turnoff the FrameTypement
       if (TerFont[pict].FrameType==PFRAME_NONE) return 0;  // aleady off
       ParaFID=TerFont[pict].ParaFID;
       if (ParaFID && ParaFID<TotalParaFrames && ParaFrame[ParaFID].InUse) ParaFrame[ParaFID].InUse=FALSE;
       TerFont[pict].ParaFID=0;
       TerFont[pict].FrameType=FrameType;
    }
    else {                        // set para frame for a picture
       if (TerFont[pict].FrameType==PFRAME_NONE) {  // create an a paragraph frame for this picture
          if ((ParaFID=GetParaFrameSlot(w))<0) return -1;

          FarMemSet(&(ParaFrame[ParaFID]),0,sizeof(struct StrParaFrame));
          ParaFrame[ParaFID].InUse=TRUE;
          ParaFrame[ParaFID].DistFromText=FRAME_DIST_FROM_TEXT;
          ParaFrame[ParaFID].margin=0;    //MIN_FRAME_MARGIN*2;
          ParaFrame[ParaFID].PageNo=CurPage;

          TerFont[pict].ParaFID=ParaFID;
       }

       // set the fields
       TerFont[pict].FrameType=FrameType;
       ParaFID=TerFont[pict].ParaFID;
       ParaFrame[ParaFID].pict=pict;
       ParaFrame[ParaFID].ShapeType=SHPTYPE_PICT_FRAME;

       ParaFrame[ParaFID].width=TerFont[pict].PictWidth+2*ParaFrame[ParaFID].margin;
       ParaFrame[ParaFID].height=TerFont[pict].PictHeight+2*ParaFrame[ParaFID].margin;
       ParaFrame[ParaFID].MinHeight=ParaFrame[ParaFID].height;

       // set the location for float type frame
       if (FrameType==PFRAME_FLOAT) {
          ParaFrame[ParaFID].flags|=PARA_FRAME_VPAGE;
          ParaFrame[ParaFID].ParaY=ParaFrame[ParaFID].y=y;
          ParaFrame[ParaFID].x=ParaFrame[ParaFID].OrgX=x;

          // check the picture section
          sect=0;
          if (TerLocateFontId(hTerWnd,pict,&line,&col)) sect=GetSection(w,line);
          
          ParaFrame[ParaFID].x-=(InchesToTwips(TerSect[sect].LeftMargin)); 
          ParaFrame[ParaFID].OrgX-=(InchesToTwips(TerSect[sect].LeftMargin)); 

          // find the location for the picture frame anchor
          FrameY=PageToFrameY(w,y);
          FrameY=TwipsToScrY(FrameY); 
          
          line=UnitsToLine(w,0,FrameY);

          for (l=line;l>=0;l--) {
             if (fid(l)>0 || cid(l)>0) continue;
             if (LineFlags(l)&LFLAG_HDRS_FTRS) continue;
             if (LinePage(l)<CurPage) break;
             if (LineFlags(l)&LFLAG_PARA_FIRST) break;
          }
          if (l<0 || LinePage(l)<CurPage) {
             if (l<0) l=0;
             for (;l<TotalLines;l++) {
                if (fid(l)>0 || cid(l)>0) continue;
                if (LineFlags(l)&LFLAG_HDRS_FTRS) continue;
                break;
             }
             if (l==TotalLines) l=TotalLines-1;
          }
          AnchorPictFrame(w,pict,line,0);
       }
       else {      // left and right floating frames are relative to paragraph
          ResetUintFlag(ParaFrame[ParaFID].flags,(PARA_FRAME_VPAGE|PARA_FRAME_VMARG));
       }  
    }

    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),(TerFont[pict].FrameType==PFRAME_NONE?FALSE:TRUE));
    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    if (repaint) Repaginate(w,FALSE,TRUE,0,TRUE);

    return ParaFID;
}

/*****************************************************************************
    TerSetPictInfo:
    Set the assorted picture information.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetPictInfo(HWND hWnd, int pict, UINT style, int align, int AuxId)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return TRUE;

    TerFont[pict].style=style;
    TerFont[pict].AuxId=AuxId;
    if (align!=TerFont[pict].PictAlign) {
       TerFont[pict].PictAlign=align;
       SetPictSize(w,pict,TerFont[pict].height,TerFont[pict].CharWidth[PICT_CHAR],TRUE);
    }

    return TRUE;
}

/*****************************************************************************
    TerGetPictOffset:
    Set the offset value (twips) for picture placement.
    This function returns TRUE when successful.
******************************************************************************/
int WINAPI _export TerGetPictOffset(HWND hWnd, int pict)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    
    if (pict<0) pict=GetCurCfmt(w,CurLine,CurCol);
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return -1;
    //if (!(TerFont[pict].style&PICT)) return -1;

    return TerFont[pict].offset;
}

/*****************************************************************************
    TerSetPictOffset:
    Set the offset value (twips) for picture placement.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetPictOffset(HWND hWnd, int pict, int offset, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    if (pict<0) pict=GetCurCfmt(w,CurLine,CurCol);
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return FALSE;
    if (!(TerFont[pict].style&PICT)) return FALSE;
    
    TerFont[pict].offset=offset;
    TerFont[pict].PictAlign=ALIGN_BOT;  // default alignement
 
    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
    XlateSizeForPrt(w,pict);

    TerArg.modified++;
        
    if (repaint) PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    TerSetPictCropping:
    Set picture cropping (twips).
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetPictCropping(HWND hWnd, int pict, int CropLeft, int CropTop, int CropRight, int CropBot, BOOL repaint)
{
    PTERWND w;
    int AdjWidth,AdjHeight,width,height;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return TRUE;

    // calculate the width and height adjustment
    AdjWidth=CropLeft+CropRight-TerFont[pict].CropLeft-TerFont[pict].CropRight;
    AdjHeight=CropTop+CropBot-TerFont[pict].CropTop-TerFont[pict].CropBot;

    // save new cropping values
    TerFont[pict].CropLeft=CropLeft;
    TerFont[pict].CropRight=CropRight;
    TerFont[pict].CropTop=CropTop;
    TerFont[pict].CropBot=CropBot;

    // adjust the picture width
    width=TerFont[pict].PictWidth=TerFont[pict].PictWidth-AdjWidth;
    height=TerFont[pict].PictHeight=TerFont[pict].PictHeight-AdjHeight;
     
    TerFont[pict].PctWidth=0;         // disable percentage width
    SetPictSize(w,pict,TwipsToScrY(height),TwipsToScrX(width),TRUE);
    TerFont[pict].flags|=FFLAG_RESIZED;

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    TerArg.modified++;
        
    if (repaint) PaintTer(w);

    return TRUE;
}

/*****************************************************************************
    TerGetPictCropping:
    Get picture cropping (twips).
    This function returns TRUE when successful.
******************************************************************************/
int WINAPI _export TerGetPictCropping(HWND hWnd, int pict, int type)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return -1;

    if (type==CROP_LEFT)  return TerFont[pict].CropLeft;
    if (type==CROP_RIGHT) return TerFont[pict].CropRight;
    if (type==CROP_TOP)   return TerFont[pict].CropTop;
    if (type==CROP_BOT)   return TerFont[pict].CropBot;

    return -1;
}

/*****************************************************************************
    TerSetPictSize:
    Set the picture size. The width or height parameters can be
    set to -1 to leave these values unchanged.  The width and height
    are specified in screen units. To specify the values in twips, use the negative values.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetPictSize(HWND hWnd, int pict, int width, int height)
{
    PTERWND w;
    int OrigWidth,OrigHeight;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse) || False(TerFont[pict].style&PICT)) return FALSE;

    OrigWidth=TerFont[pict].PictWidth;
    OrigHeight=TerFont[pict].PictHeight;
    
    // set new width
    if (width==0 && height!=-1) {  // adjust the height propertionately
       if (height>=0) width=(int)((height*OrigWidth)/OrigHeight);
       else           width=height;    // use the same percentage 
    } 
    if (width!=-1) {
       TerFont[pict].PictWidth=(width>=0)?ScrToTwipsX(width):(-width);
       TerFont[pict].PctWidth=0;         // disable percentage width
    }

    // set new height
    if (height==0 && width!=-1) {  // adjust the height propertionately
       if (width>=0) height=(int)((width*OrigHeight)/OrigWidth);
       else          height=width;    // use the same percentage 
    } 
    if (height!=-1) {
       TerFont[pict].PictHeight=(height>=0)?ScrToTwipsY(height):(-height);
    }
    
    if (height!=-1 || width!=-1) {
       SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
       TerFont[pict].flags|=FFLAG_RESIZED;
       XlateSizeForPrt(w,pict);              // convert size to printer resolution

       if (TerFont[pict].ObjectType!=OBJ_NONE) UpdateOleObj(w,pict);   // update ole object

       // set the picture frame if any
       if (TerFont[pict].FrameType!=PFRAME_NONE) {
          int ParaFID=TerFont[pict].ParaFID;
          ParaFrame[ParaFID].width=TerFont[pict].PictWidth; 
          ParaFrame[ParaFID].MinHeight=ParaFrame[ParaFID].height=TerFont[pict].PictHeight; 
          if (!InRtfRead) TerRepaginate(hWnd,true);
       } 

    }
     
    return TRUE;
}

/*****************************************************************************
MAK:
******************************************************************************/
BOOL WINAPI _export TerSetPictSizeTwips(HWND hWnd, int pict, int width, int height)
{
	PTERWND w;

	if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
	if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return TRUE;

	return TerSetPictSize(hTerWnd,pict,TwipsToScrX(width),TwipsToScrY(height));

}

/*****************************************************************************
    TerGetOrigPictSize:
    Get the original picture size (in twips).
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerGetOrigPictSize(HWND hWnd, int pict, LPINT pWidth, LPINT pHeight)
{
    PTERWND w;
    int width,height;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return TRUE;

    width=TerFont[pict].bmWidth;
    height=TerFont[pict].bmHeight;

    if (   TerFont[pict].PictType==PICT_DIBITMAP 
        || TerFont[pict].PictType==PICT_JPEG
        || TerFont[pict].PictType==PICT_PNG
        || TerFont[pict].PictType==PICT_TIFF
        || TerFont[pict].PictType==PICT_GIF) {  // convert to twips
       width=ScrToTwipsX(width);
       height=ScrToTwipsY(height);
    } 

    if (pWidth) (*pWidth)=width;
    if (pHeight) (*pHeight)=height;

    return TRUE;
}

/*****************************************************************************
    TerSetAnimInfo:
    Set the picture animation information.
******************************************************************************/
BOOL WINAPI _export TerSetAnimInfo(HWND hWnd, int pict, long LoopCount, int delay)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return FALSE;

    if (!TerFont[pict].anim && !CreateAnimInfo(w,pict)) return FALSE;

    if (LoopCount>=0 || LoopCount==ANIM_CONTINUOUS)
       TerFont[pict].anim->LoopCount=TerFont[pict].anim->OrigLoopCount=LoopCount;
    if (delay>=0) TerFont[pict].anim->delay=delay;

    return TRUE;
}

/*****************************************************************************
    TerAddAnimPict:
    Add a picture to the animation sequence
******************************************************************************/
BOOL WINAPI _export TerAddAnimPict(HWND hWnd, int pict, int NewPict, int delay)
{
    PTERWND w;
    int LastPict;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (pict<0 || pict>=TotalFonts || !(TerFont[pict].InUse)) return FALSE;
    if (!(TerFont[pict].style&PICT)) return FALSE;
    if (NewPict<0 || NewPict>=TotalFonts || !(TerFont[NewPict].InUse)) return FALSE;
    if (!(TerFont[NewPict].style&PICT)) return FALSE;
    if (delay<0) return TRUE;

    if (!TerFont[pict].anim && !CreateAnimInfo(w,pict)) return FALSE;

    if (TerFont[pict].anim->FirstPict>0) return FALSE;     // not the initial picture

    LastPict=pict;     // find the
    while (TerFont[LastPict].anim && TerFont[LastPict].anim->NextPict>0) {
      LastPict=TerFont[LastPict].anim->NextPict;
      if (LastPict<TotalFonts && TerFont[LastPict].InUse && TerFont[LastPict].style&PICT) continue;
      else return FALSE;
    }

    if (!TerFont[NewPict].anim && !CreateAnimInfo(w,NewPict)) return FALSE;

    TerFont[LastPict].anim->NextPict=NewPict;
    TerFont[NewPict].anim->FirstPict=pict;
    TerFont[NewPict].anim->delay=delay;

    return TRUE;
}

/*****************************************************************************
    CreateAnimInfo:
    Create the animation information structure for a picture id
    Add a picture to the animation sequence
******************************************************************************/
CreateAnimInfo(PTERWND w,int pict)
{
    if (TerFont[pict].anim) return TRUE;
    if (NULL==(TerFont[pict].anim=MemAlloc(sizeof(struct StrAnim)))) return FALSE;
    FarMemSet(TerFont[pict].anim,0,sizeof(struct StrAnim));

    return TRUE;
}

/*****************************************************************************
    DeleteAnimInfo:
    Delete the animation information structure for a picture id
    Add a picture to the animation sequence
******************************************************************************/
DeleteAnimInfo(PTERWND w,int pict)
{
    if (!TerFont[pict].anim) return TRUE;

    if (TerFont[pict].anim->TimerId) KillTimer(hTerWnd,TerFont[pict].anim->TimerId);   // kill the timer

    MemFree(TerFont[pict].anim);
    TerFont[pict].anim=NULL;

    return TRUE;
}

/*****************************************************************************
    IsFirstAniPict:
    This function returns TRUE if the given picture is a the first animation
    picture.
******************************************************************************/
BOOL IsFirstAnimPict(PTERWND w,int pict)
{
    return (TerFont[pict].InUse && (TerFont[pict].style&PICT) && TerFont[pict].anim
            && (TerFont[pict].anim->FirstPict==0 || TerFont[pict].anim->FirstPict==pict));
}

/*****************************************************************************
    SetAnimTimer:
    Set an animation timer.
******************************************************************************/
SetAnimTimer(PTERWND w,int pict)
{
    int CurAnim;

    if (InDialogBox || InPrinting) return TRUE;  // suspend animation

    if (!TerFont[pict].anim) return TRUE;

    CurAnim=TerFont[pict].anim->CurAnim;      // currently drawn picture
    if (CurAnim==0) CurAnim=pict;

    // kill the current timer
    if (TerFont[pict].anim->TimerId) KillTimer(hTerWnd,TerFont[pict].anim->TimerId);
    TerFont[pict].anim->TimerId=0;


    if (TerFont[CurAnim].anim && TerFont[pict].anim->LoopCount!=-1) { // set the timer for the next picture in the sequence
       int NextPict=TerFont[CurAnim].anim->NextPict;
       if  (TerOpFlags&TOFLAG_FULL_PAINT) NextPict=CurAnim;  // do not advance now

       if (NextPict==0) {                  // time to loop
          if (TerFont[pict].anim->LoopCount==ANIM_CONTINUOUS) NextPict=pict;
          else {
             TerFont[pict].anim->LoopCount--;
             if (TerFont[pict].anim->LoopCount>=0) NextPict=pict;  // start all over
          }

       }
       if (NextPict>0 && TerFont[NextPict].anim) {
          int TimerId=TIMER_ANIM+pict;
          TerFont[pict].anim->NextAnim=NextPict;
          SetTimer(hTerWnd,TimerId,TerFont[NextPict].anim->delay*10,NULL);  // set the time on
          TerFont[pict].anim->TimerId=TimerId;
       }
    }

    return TRUE;
}

/*****************************************************************************
    AdjustHtmlPictWidth:
    Adjust the percentage width of the pictures in the html mode
******************************************************************************/
BOOL AdjustHtmlPictWidth(PTERWND w)
{
    int i;

    for (i=0;i<TotalFonts;i++) {
       if (!TerFont[i].InUse || !(TerFont[i].style&PICT) || TerFont[i].PctWidth==0) continue;
       TerSetPictPctWidth(hTerWnd,i,TerFont[i].PctWidth);
    }

    return TRUE;
}

/*****************************************************************************
    AdjustHtmlRulerWidth:
    Adjust the width of the html ruler
******************************************************************************/
BOOL AdjustHtmlRulerWidth(PTERWND w)
{
    int CurPara,RulerType,LeftIndent,RightIndent,RulerWidth,align;
    int RulerPixWidth;

    for (CurPara=0;CurPara<TotalPfmts;CurPara++) {
       RulerType=(PfmtId[CurPara].AuxId&HTML_MISC_FLAG_MASK)>>9;
       if (RulerType!=PARA_HR) continue;

       RulerWidth=(short)LOWORD(PfmtId[CurPara].Aux1Id);
       if (RulerWidth==0) continue;        // no width specification for the ruler
       align=HIWORD(PfmtId[CurPara].Aux1Id);

       if (RulerWidth>=0) RulerPixWidth=MulDiv(TerWinWidth,RulerWidth,100);
       else               RulerPixWidth=-RulerWidth;

       LeftIndent=RightIndent=0;
       if      (align==CENTER)        LeftIndent=RightIndent=(TerWinWidth-RulerPixWidth)/2;
       else if (align==RIGHT_JUSTIFY) LeftIndent=TerWinWidth-RulerPixWidth;
       else                           RightIndent=TerWinWidth-RulerPixWidth;

       if (LeftIndent<0) LeftIndent=0;
       if (RightIndent<0) RightIndent=0;

       // modify the paragraph id - these paragraph ids can be shared only among other rulers
       PfmtId[CurPara].LeftIndentTwips=ScrToTwipsX(LeftIndent);
       PfmtId[CurPara].LeftIndent=LeftIndent;
       PfmtId[CurPara].RightIndentTwips=ScrToTwipsX(RightIndent);
       PfmtId[CurPara].RightIndent=RightIndent;
    }


    return TRUE;
}

/*****************************************************************************
    TerSetPictPctWidth:
    Set the picture width in percentage of screen width.
******************************************************************************/
BOOL WINAPI _export TerSetPictPctWidth(HWND hWnd, int pict, int width)
{
    PTERWND w;
    int  PixWidth;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>TotalFonts || !(TerFont[pict].InUse)) return TRUE;

    // set new width
    if (width>0) {
       if (width>100) width=100;
       TerFont[pict].PctWidth=width;  // specified in percentage of the screen width
       PixWidth=MulDiv(TerWinWidth,width,100);
       TerFont[pict].PictWidth=ScrToPointsX(PixWidth);
       SetPictSize(w,pict,TerFont[pict].height,PixWidth,TRUE);
    }

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    return TRUE;
}


/*****************************************************************************
    TerCreateMetafile:
    Extract the metafile from clipboard.  The metafile information is returned
    in TerFont array element.
******************************************************************************/
int TerCreateMetafile(PTERWND w,HGLOBAL hClip,int pict)
{
    int  ResX,ResY,width,height;
    HMETAFILE hMeta;
    METAHEADER far *hdr;         // metafile header
    long MetaSize;
    METAFILEPICT far *pInfo;

    // Get device resolution
    ResX=GetDeviceCaps(hTerDC,LOGPIXELSX);
    ResY=GetDeviceCaps(hTerDC,LOGPIXELSY);     // resolution (pixel/inch) of the current device

    // access clipboard data
    if (NULL==(pInfo=(METAFILEPICT far *)GlobalLock(hClip))) {
        PrintError(w,MSG_ERR_MEM_LOCK,"Clipboard");
        return -1;
    }

    // calculate width/height in device units
    if (pInfo->mm==MM_TEXT) {
       width=pInfo->xExt;
       height=pInfo->yExt;
    }
    else if (pInfo->mm==MM_ANISOTROPIC) { // only anisotropic mode supported now
       width=MulDiv(pInfo->xExt,ResX,2540);// 1 inch = 2540 HIMETRICs (.01 mm)
       height=MulDiv(pInfo->yExt,ResY,2540);
       if (width==0) width=ResX*3;        // supply a default
       if (height==0) height=ResY*3;
    }
    else {                                        // unsupported mode
       GlobalUnlock(hClip);
       return -1;
    }


    // Extract metafile and release clipboard data
    if (NULL==(hMeta=CopyMetaFile(pInfo->hMF,NULL))) {
        PrintError(w,MSG_ERR_META_COPY,NULL);
        return -1;
    }
    GlobalUnlock(hClip);

    // Get the metafile size
    if ( NULL==(hdr=(LPVOID)TerGetMetaFileBits(hMeta,null))) {    // convert to handler
        PrintError(w,MSG_ERR_META_ACCESS,NULL);
        return -1;
    }
    MetaSize=hdr->mtSize*2;                     // size specified in words
    if (NULL==(hMeta=TerSetMetaFileBits((LPVOID)hdr))) {  // recreate metafile handle
       PrintError(w,MSG_ERR_META_RECREATE,NULL);
       return -1;
    }

    // set the font array
    if (pict==-1) {                              // if slot not specified, get one
       if ((pict=FindOpenSlot(w))==-1) return -1;  // find an empty slot
    }
    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=PICT_METAFILE;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].hMeta=hMeta;            // meta file handle
    TerFont[pict].bmWidth=ScrToTwipsX(width);   // width in twips
    TerFont[pict].bmHeight=ScrToTwipsY(height); // height in twips
    TerFont[pict].PictHeight=(int)(((long)height*1440)/ResY);// height in twips
    TerFont[pict].PictWidth=(int)(((long)width*1440)/ResX);  // width in twips
    TerFont[pict].OrigPictHeight=TerFont[pict].PictHeight;
    TerFont[pict].OrigPictWidth=TerFont[pict].PictWidth;
    TerFont[pict].ImageSize=MetaSize;     // size of the meta file
    TerFont[pict].InfoSize=0;             // info size not used by metafiles
    TerFont[pict].pImage=NULL;
    TerFont[pict].pInfo=NULL;
    TerFont[pict].style=PICT;
    TerFont[pict].PictAlign=ALIGN_BOT;

    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    return pict;
}

#if defined(WIN32)
/*****************************************************************************
    TerCreateEnhMetafile:
    Extract the metafile from clipboard.  The metafile information is returned
    in TerFont array element.
******************************************************************************/
int TerCreateEnhMetafile(PTERWND w,HENHMETAFILE hEnhMeta,int pict)
{
    int  ResX,ResY,width,height;
    HENHMETAFILE hMeta;
    long MetaSize;
    ENHMETAHEADER  EnhHdr;

    if (NULL==(hMeta=CopyEnhMetaFile(hEnhMeta,NULL))) {
        PrintError(w,MSG_ERR_META_COPY,"TerCreateEnhMetaFile");
        return -1;
    }

    // get the enh meta header
    if (0==GetEnhMetaFileHeader(hMeta,sizeof(ENHMETAHEADER),&EnhHdr)) {
        PrintError(w,MSG_ERR_ENH_META_HDR,"TerCreateEnhMetafile");
        return -1;
    }

    MetaSize=EnhHdr.nBytes;          // size of the metafile

    //******** get width height **********
    ResY=GetDeviceCaps(hTerDC,LOGPIXELSY);         // resolution (pixel/inch) of the current device
    ResX=GetDeviceCaps(hTerDC,LOGPIXELSX);
    CalcEnhMetafileDim(w,-1,&EnhHdr,&width,&height);   // extract width/height in pixel units


    // set the font array
    if (pict==-1) {                              // if slot not specified, get one
       if ((pict=FindOpenSlot(w))==-1) return -1;  // find an empty slot
    }
    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=PICT_ENHMETAFILE;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].hEnhMeta=hMeta;         // meta file handle
    TerFont[pict].PictHeight=MulDiv(height,1440,ResY); // dimension in twips
    TerFont[pict].PictWidth=MulDiv(width,1440,ResX);
    TerFont[pict].bmHeight=TerFont[pict].PictHeight;   // original picture dimensions
    TerFont[pict].bmWidth=TerFont[pict].PictWidth;
    TerFont[pict].ImageSize=MetaSize;     // size of the meta file
    TerFont[pict].InfoSize=0;             // info size not used by metafiles
    TerFont[pict].pImage=NULL;
    TerFont[pict].pInfo=NULL;
    TerFont[pict].style=PICT;
    TerFont[pict].PictAlign=ALIGN_BOT;

    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    return pict;
}

#endif

/******************************************************************************
   TerInsertPictureFileXY:
   Drop a picture file at a certain x/y pixel location.
*******************************************************************************/
int WINAPI _export TerInsertPictureFileXY(HWND hWnd,LPBYTE FileName,BOOL embed, int align, BOOL insert, int x, int y)
{
   PTERWND w;
   DWORD lParam;
   long ParaLine,l;
   int  CurY,PictId,AnchorY;
   DWORD SaveTerFlags;

   if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

   SaveTerFlags=TerFlags;
   TerFlags|=TFLAG_PICT_IN_FRAME;

   // get cursor location at the drag point

   lParam=y;
   lParam=lParam<<16;             // high word
   lParam=lParam+x;
   TerMousePos(w,lParam,FALSE);
   CurLine=MouseLine;
   CurCol=MouseCol;

   // translate the x/y for an bounding frame
   if (TerArg.PageMode && TerFlags&TFLAG_PICT_IN_FRAME && fid(CurLine)==0) {
      NewFrameX=ScrToTwipsX(x-TerWinRect.left+TerWinOrgX);
      if (BorderShowing) NewFrameX-=PrtToTwipsX(GetBorderLeftSpace(w,CurPage));

      CurY=y-TerWinRect.top+TerWinOrgY;

      if (NewFrameVPage) NewFrameY=FrameToPageY(w,ScrToTwipsY(CurY));  // relative to the physical top of the page 
      else {       // find NewFrameY relative to the anchor paragraph
         ParaLine=GetParaFrameLine(w,CurLine);   // get the anchor line

         // find the next stationary line.
         l=ParaLine;
         while (l<TotalLines) {
            if (fid(l)==0) break;
            l++;
         }
         if (l==TotalLines) l--;
         AnchorY=LineToUnits(w,l);              // anchor pargraph y
         NewFrameY=CurY-AnchorY;                // y relative to the anchor paragraph

         NewFrameY=ScrToTwipsY(NewFrameY);
      }  
   }

   PictId=TerInsertPictureFile(hWnd,FileName,embed,align,insert);

   TerFlags=SaveTerFlags;

   return PictId;
}

/******************************************************************************
    TerInsertPictureFile:
    This routine embeds or links a picture file.
    If the FileName is NULL if prompts the user for a file name.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerInsertPictureFile(HWND hWnd,LPBYTE FileName,BOOL embed, int align, BOOL insert)
{
    PTERWND w;
    BYTE name[MAX_WIDTH+1]="";
    int FileType,pict;
    BYTE PictFiles[200];

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (FileName!=NULL && FileName[0]==0) FileName=NULL;

    while (TRUE) {
        if   (FileName) {
           lstrcpy(name,FileName);
           ResolveLinkFileName(w,name);
        }
        else {
           PictFiles[0]=0;
           lstrcat(PictFiles,MsgString[MSG_BMP_FILES]);
           lstrcat(PictFiles,MsgString[MSG_METAFILES]);
           lstrcat(PictFiles,MsgString[MSG_JPG_FILES]);
           lstrcat(PictFiles,MsgString[MSG_PNG_FILES]);
           lstrcat(PictFiles,MsgString[MSG_GIF_FILES]);
           lstrcat(PictFiles,MsgString[MSG_TIF_FILES]);
           if (Win32) lstrcat(PictFiles,MsgString[MSG_ENH_METAFILES]);
           if (!GetFileName(w,TRUE,name,1,PictFiles)) return -1;
        }

        FileType=GetPictFileType(w,name,FileName?FALSE:TRUE);

        if (FileType==FTYPE_NONE || FileType==FTYPE_ERR) {  // file not found
           if (FileName) {
              if (FileType==FTYPE_ERR) return 0;
           }
           else {
              PrintError(w,MSG_ERR_PICT_FILE_READ,"TerInsertPictureFile");
              continue;
           }

        }
        break;
    }

    if (FileType==FTYPE_DIB)           pict=TerPictureFromFile(hTerWnd,name,align,insert);
    else if (FileType==FTYPE_METAFILE) pict=TerPictureFromWmf(hTerWnd,name,align,insert);
    else if (FileType==FTYPE_ENHMETAFILE) pict=TerPictureFromEmf(hTerWnd,name,align,insert);
    else if (FileType==FTYPE_JPEG)     pict=TerPictureFromJpg(hTerWnd,name,align,insert);
    else if (FileType==FTYPE_GIF )     pict=TerPictureFromGif(hTerWnd,name,align,insert);
    else if (FileType==FTYPE_PNG )     pict=TerPictureFromPng(hTerWnd,name,align,insert);
    else if (FileType==FTYPE_TIF )     pict=TerPictureFromTif(hTerWnd,name,align,insert);
    else                               pict=TerPictureFromOther(hTerWnd,name,align,insert);  // read an unsupported picture with the help of the host application

    if (pict<=0) return 0;

    // create link to the picture name if specified
    if (!embed) {
       if (NULL!=(TerFont[pict].LinkFile=OurAlloc(lstrlen(name)+1))) {
          lstrcpy(TerFont[pict].LinkFile,name);
       }
       if (TerFont[pict].OrigImageSize>0) {    // do not save original picture information for linked images
          OurFree(TerFont[pict].pOrigImage);
          TerFont[pict].pOrigImage=NULL;
          TerFont[pict].OrigImageSize=0;
          TerFont[pict].OrigPictType=0;
       } 
    }

    return pict;
}

/******************************************************************************
    TerPictureFromFile:
    This routine asks for a bitmap file name if the BitmapFileName is NULL.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromFile(HWND hWnd,LPBYTE BitmapFileName,int align, BOOL insert)
{
    LPBYTE pImage;
    LPBITMAPINFOHEADER pInfo;
    int    i,pict,ResX,ResY,height,width,CurCfmt,BytesRead;
    BYTE   name[MAX_WIDTH+1]="";
    WORD   BmSign;
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    BITMAPFILEHEADER hdr;
    DWORD ImageSize,InfoSize;
    PTERWND w;
    long FileLen;
    BOOL   ReplacePicture;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    if (BitmapFileName!=NULL && BitmapFileName[0]==0) BitmapFileName=NULL;

    while (InFile==INVALID_HANDLE_VALUE) {
        if   (BitmapFileName) lstrcpy(name,BitmapFileName);
        else if (!GetFileName(w,TRUE,name,1,"Bitmap Files(*.BMP)|*.BMP|")) return 0;

        if ((InFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
           if (!BitmapFileName) PrintError(w,MSG_ERR_PICT_FILE_OPEN,NULL);
           if (BitmapFileName) return 0;
           else                continue;       // get another name
        }

        FileLen=SetFilePointer(InFile,0,NULL,FILE_END);   // get the file length
        SetFilePointer(InFile,0,NULL,FILE_BEGIN);           // reposition the file

        ReadFile(InFile,TempString,sizeof(BITMAPFILEHEADER),&((DWORD)BytesRead),NULL);
        if (BytesRead!=sizeof(BITMAPFILEHEADER)) {
           PrintError(w,MSG_ERR_PICT_FILE_READ,NULL);
           CloseHandle(InFile);
           InFile=INVALID_HANDLE_VALUE;
           if (BitmapFileName) return 0;
           else                continue;       // get another name
        }
        FarMove(TempString,&hdr,sizeof(BITMAPFILEHEADER));

        BmSign=(((WORD)'M')<<8)|'B';    // signature of the bitmap file
        if (hdr.bfType!=BmSign) {
           PrintError(w,MSG_BAD_BMP_FILE,NULL);
           CloseHandle(InFile);
           InFile=INVALID_HANDLE_VALUE;
           if (BitmapFileName) return 0;
           else                continue;       // get another name
        }
    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    //************ calculate image and info structure sizes **
    InfoSize=hdr.bfOffBits-sizeof(BITMAPFILEHEADER);
    ImageSize=FileLen-sizeof(BITMAPFILEHEADER)-InfoSize;

    //************ Allocate Space for image and info *********
    if  (NULL==(TerFont[pict].pImage=OurAlloc(ImageSize))
      || NULL==(TerFont[pict].pInfo=OurAlloc(InfoSize)) ){
          PrintError(w,MSG_OUT_OF_MEM,"TerPictureFromFile");
          InitTerObject(w,pict);
          goto EOR;
    }

    pInfo=(LPBITMAPINFOHEADER)TerFont[pict].pInfo;
    pImage=(LPVOID)TerFont[pict].pImage;

    //********** Read image and info data *******************
    if (!TerFarRead(w,'F',InFile,NULL,NULL,0L,InfoSize,(LPBYTE)pInfo)
     || !TerFarRead(w,'F',InFile,NULL,NULL,0L,ImageSize,pImage) ) {
        PrintError(w,MSG_OUT_OF_MEM,NULL);
        InitTerObject(w,pict);
        goto EOR;
    }

    // convert old style bitmap to new style
    if (pInfo->biSize==sizeof(BITMAPCOREHEADER)) {  // old style bitmap
       LPBITMAPCOREHEADER pCoreHdr=(LPBITMAPCOREHEADER)pInfo;
       LPBITMAPCOREINFO pCore=(LPBITMAPCOREINFO)pInfo;
       int NumColors=0;
       RGBTRIPLE far *pOldColor;
       RGBQUAD far *pColor;

       // determine the size of the color table
       if      (pCoreHdr->bcBitCount==1) NumColors=2;
       else if (pCoreHdr->bcBitCount==4) NumColors=16;
       else if (pCoreHdr->bcBitCount==8) NumColors=256;

       // allocate new bitmap info structure
       InfoSize=sizeof(BITMAPINFOHEADER)+NumColors*sizeof(RGBQUAD);
       if  (NULL==(TerFont[pict].pInfo=OurAlloc(InfoSize))) {
             PrintError(w,MSG_OUT_OF_MEM,"hInfo");
             InitTerObject(w,pict);
             goto EOR;
       }

       pInfo=(LPBITMAPINFOHEADER)TerFont[pict].pInfo;

       // transfer values from the old structure to the new structure
       FarMemSet((LPBYTE)pInfo,0,(int)InfoSize);
       pInfo->biSize=sizeof(BITMAPINFOHEADER);
       pInfo->biWidth=pCoreHdr->bcWidth;
       pInfo->biHeight=pCoreHdr->bcHeight;
       pInfo->biPlanes=pCoreHdr->bcPlanes;
       pInfo->biBitCount=pCoreHdr->bcBitCount;
       pInfo->biCompression=BI_RGB;           // no compression

       // transfer the colors
       if (NumColors>0) {
          pOldColor=pCore->bmciColors;
          pColor=((LPBITMAPINFO)pInfo)->bmiColors;

          for (i=0;i<NumColors;i++) {
             pColor[i].rgbBlue=pOldColor[i].rgbtBlue;
             pColor[i].rgbGreen=pOldColor[i].rgbtGreen;
             pColor[i].rgbRed=pOldColor[i].rgbtRed;
             pColor[i].rgbReserved=0;
          }
       }

       OurFree(pCore);
    }

    //******** initialize other TerFont variables **********
    ResY=GetDeviceCaps(hTerDC,LOGPIXELSY);         // resolution (pixel/inch) of the current device
    ResX=GetDeviceCaps(hTerDC,LOGPIXELSX);

    if (!HtmlMode && False(TerFlags6&TFLAG6_SHOW_PICT_IN_PIXEL_SIZE) && pInfo->biXPelsPerMeter!=0 && pInfo->biYPelsPerMeter!=0) {                              // use the resolution embedded in the file to be compatible with MSWord
       ResY=MulDiv(pInfo->biYPelsPerMeter,254,10000);
       ResX=MulDiv(pInfo->biXPelsPerMeter,254,10000);
    } 

    height=(int)(pInfo->biHeight);
    width=(int)(pInfo->biWidth);

    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=PICT_DIBITMAP;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].PictHeight=MulDiv(height,1440,ResY); // dimension in twips
    TerFont[pict].PictWidth=MulDiv(width,1440,ResX);
    TerFont[pict].ImageSize=ImageSize;
    TerFont[pict].InfoSize=InfoSize;
    TerFont[pict].style=PICT;
    TerFont[pict].PictAlign=align;

    
    ImgDenX=OrigScrResX;     // pixel densities - these variables are set by the image import functions //>>> RZW 3/15/02 - Initialize global variables
    ImgDenY=OrigScrResY;                                                                                //>>> RZW 3/15/02 - Initialize global variables

    //***************** translate into device bitmap *********************
    if (!DIB2Bitmap(w,hTerDC,pict)) {     // translate DIB to device bitmap
        InitTerObject(w,pict);
        goto EOR;
    }

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }


    EOR:                                  // end of routine
    CloseHandle(InFile);

    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here

    return pict;
}

/******************************************************************************
    ApplyPrevPictProp:
    Apply the propert of the picture id replaced with current picture insertion
    operation
*******************************************************************************/
BOOL ApplyPrevPictProp(PTERWND w,int pict, struct StrFont *pPrevPict)
{
    if (False(pPrevPict->style&PICT)) return true;   // previous id was not a picture
 
    TerFont[pict].style=pPrevPict->style;
    TerFont[pict].FrameType=pPrevPict->FrameType;
    TerFont[pict].ParaFID=pPrevPict->ParaFID;
    TerFont[pict].FieldId=pPrevPict->FieldId;
    TerFont[pict].AuxId=pPrevPict->AuxId;
    TerFont[pict].Aux1Id=pPrevPict->Aux1Id;
    TerFont[pict].MapId=pPrevPict->MapId;
    TerFont[pict].DispFrame=pPrevPict->DispFrame;

    return true;
}
 
/******************************************************************************
    TerPictureFromWmf:
    This routine asks for a Windows meta file name if the file is NULL.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromWmf(HWND hWnd,LPBYTE WmfName,int align, BOOL insert)
{
    LPBYTE pImage;
    int    pict,ResX,ResY,height,width,CurCfmt,OrgRecSize=10,ExtRecSize=10;
    BYTE   name[MAX_WIDTH+1]="";
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    DWORD ImageSize,InitImageSize;
    PTERWND w;
    long   FileLen,idx,BytesRead;
    BOOL   ReplacePicture;
    struct StrMetaFileHdr FileHdr;
    METARECORD far *pOrg;          // SetWindowOrg record
    METARECORD far *pExt;          // SetWindowExt record
    METAHEADER far *pMetaHdr;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    if (WmfName!=NULL && WmfName[0]==0) WmfName=NULL;

    while (InFile==INVALID_HANDLE_VALUE) {
        if   (WmfName) lstrcpy(name,WmfName);
        else if (!GetFileName(w,TRUE,name,1,"Windows Metafiles(*.WMF)|*.WMF|")) return 0;

        if ((InFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
           if (!WmfName) PrintError(w,MSG_ERR_PICT_FILE_OPEN,NULL);
           if (WmfName)  return 0;
           else          continue;       // get another name
        }

        FileLen=SetFilePointer(InFile,0,NULL,FILE_END);   // get the file length
        SetFilePointer(InFile,0,NULL,FILE_BEGIN);           // reposition the file

        if (!TerFarRead(w,'F',InFile,NULL,NULL,0L,sizeof(FileHdr),(LPBYTE)&FileHdr)) {
           PrintError(w,MSG_ERR_META_READ,NULL);
           CloseHandle(InFile);
           InFile=INVALID_HANDLE_VALUE;
           if (WmfName) return 0;
           else         continue;       // get another name
        }

        // test if metafile
        if (FileHdr.sign!=0x9ac6cdd7) {
           PrintError(w,MSG_ERR_META_BAD_FILE,NULL);
           CloseHandle(InFile);
           InFile=INVALID_HANDLE_VALUE;
           if (WmfName) return 0;
           else         continue;       // get another name
        }

        ImageSize=InitImageSize=FileLen-sizeof(FileHdr);
        ImageSize+=(OrgRecSize+ExtRecSize);    // add the size for two additional records

        if  (NULL==(TerFont[pict].pImage=OurAlloc(ImageSize))) {
              PrintError(w,MSG_OUT_OF_MEM,"pImage");
              InitTerObject(w,pict);
              goto EOR;
        }
        pImage=(LPVOID)TerFont[pict].pImage;

        //********** Read the metahdr record *******************
        if (!TerFarRead(w,'F',InFile,NULL,NULL,0L,sizeof(METAHEADER),pImage) ) {
            PrintError(w,MSG_ERR_META_READ,NULL);
            OurFree(TerFont[pict].pImage);
            InitTerObject(w,pict);
            goto EOR;
        }
        BytesRead=sizeof(METAHEADER);
        idx=BytesRead;                  // index into pImage

        pMetaHdr=(METAHEADER far *)pImage;

        // place the org and ext record in the beginning of the meta data
        pOrg=(METARECORD far *)&(pImage[idx]);
        idx+=OrgRecSize;
        pExt=(METARECORD far *)&(pImage[idx]);
        idx+=ExtRecSize;

        pMetaHdr->mtSize+=((OrgRecSize+ExtRecSize)/sizeof(WORD));

        //********** Read image and info data *******************
        if (!TerFarRead(w,'F',InFile,NULL,NULL,0L,InitImageSize-BytesRead,&(pImage[idx])) ) {
            PrintError(w,MSG_ERR_META_READ,NULL);
            OurFree(TerFont[pict].pImage);
            InitTerObject(w,pict);
            goto EOR;
        }

    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    //******** initialize other TerFont variables **********
    ResY=GetDeviceCaps(hTerDC,LOGPIXELSY);         // resolution (pixel/inch) of the current device
    ResX=GetDeviceCaps(hTerDC,LOGPIXELSX);

    height=FileHdr.bottom-FileHdr.top;               // in units
    width=FileHdr.right-FileHdr.left;                // in units

    // set the SetWindowOrt and SetWindowExt records
    pOrg->rdSize=pExt->rdSize=5;        // size in words
    pOrg->rdFunction=META_SETWINDOWORG;
    pExt->rdFunction=META_SETWINDOWEXT;
    pOrg->rdParm[0]=FileHdr.top;
    pOrg->rdParm[1]=FileHdr.left;
    pExt->rdParm[0]=height;
    pExt->rdParm[1]=width;

    height=MulDiv(height,ResY,FileHdr.UnitsPerInch); // in pixels
    width=MulDiv(width,ResX,FileHdr.UnitsPerInch);   // in pixels


    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=PICT_METAFILE;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].PictHeight=MulDiv(height,1440,ResY); // dimension in twips
    TerFont[pict].PictWidth=MulDiv(width,1440,ResX);
    TerFont[pict].ImageSize=ImageSize;
    TerFont[pict].InfoSize=0;
    TerFont[pict].style=PICT;
    TerFont[pict].PictAlign=align;

    //***************** create metafile  *********************
    TerFont[pict].hMeta=TerSetMetaFileBits(TerFont[pict].pImage); // convert to metafile
    TerFont[pict].pImage=0;
    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
    if (TerFont[pict].bmHeight==0) TerFont[pict].bmHeight=TerFont[pict].PictHeight; // actual height of the picture
    if (TerFont[pict].bmWidth==0) TerFont[pict].bmWidth=TerFont[pict].PictWidth;
    TerFont[pict].OrigPictHeight=TerFont[pict].PictHeight; // original height of the picture in twips
    TerFont[pict].OrigPictWidth=TerFont[pict].PictWidth;

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }


    EOR:                                  // end of routine
    CloseHandle(InFile);

    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here

    return pict;
}


/******************************************************************************
    TerPictureFromEmf:
    This routine asks for a enhanced meta file name if the file is NULL.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromEmf(HWND hWnd,LPBYTE EmfName,int align, BOOL insert)
{
    int pict=0;

    #if defined(WIN32)                // Enhanced metafile supported in WIN32 only

    LPBYTE pImage;
    int    ResX,ResY,height,width,CurCfmt;
    BYTE   name[MAX_WIDTH+1]="";
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    DWORD ImageSize;
    PTERWND w;
    long   FileLen;
    BOOL   ReplacePicture;
    ENHMETAHEADER FileHdr;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    if (EmfName!=NULL && EmfName[0]==0) EmfName=NULL;

    while (InFile==INVALID_HANDLE_VALUE) {
        if   (EmfName) lstrcpy(name,EmfName);
        else if (!GetFileName(w,TRUE,name,1,"Enhanced Metafiles(*.EMF)|*.EMF|")) return 0;

        if ((InFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
           if (!EmfName) PrintError(w,MSG_ERR_PICT_FILE_OPEN,"TerPictureFromEmf");
           if (EmfName)  return 0;
           else          continue;       // get another name
        }

        FileLen=SetFilePointer(InFile,0,NULL,FILE_END);   // get the file length
        SetFilePointer(InFile,0,NULL,FILE_BEGIN);           // reposition the file

        if (!TerFarRead(w,'F',InFile,NULL,NULL,0L,sizeof(FileHdr),(LPBYTE)&FileHdr)) {
           PrintError(w,MSG_ERR_META_READ,"TerPictureFromEmf");
           CloseHandle(InFile);
           InFile=INVALID_HANDLE_VALUE;
           if (EmfName) return 0;
           else         continue;       // get another name
        }

        // test if metafile
        if (FileHdr.dSignature!=ENHMETA_SIGNATURE) {
           PrintError(w,MSG_ERR_META_BAD_FILE,NULL);
           CloseHandle(InFile);
           InFile=INVALID_HANDLE_VALUE;
           if (EmfName) return 0;
           else         continue;       // get another name
        }

        ImageSize=FileLen;

        if  (NULL==(TerFont[pict].pImage=OurAlloc(ImageSize))) {
              PrintError(w,MSG_OUT_OF_MEM,"pImage");
              InitTerObject(w,pict);
              goto EOR;
        }
        pImage=(LPVOID)TerFont[pict].pImage;

        // copy the header
        FarMove(&FileHdr,pImage,sizeof(FileHdr));

        //********** Read image and info data *******************
        if (!TerFarRead(w,'F',InFile,NULL,NULL,0L,ImageSize-sizeof(FileHdr),&(pImage[sizeof(FileHdr)])) ) {
            PrintError(w,MSG_ERR_META_READ,"TerPictureFromEmf");
            OurFree(TerFont[pict].pImage);
            InitTerObject(w,pict);
            goto EOR;
        }

    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    //******** initialize other TerFont variables **********
    ResY=GetDeviceCaps(hTerDC,LOGPIXELSY);         // resolution (pixel/inch) of the current device
    ResX=GetDeviceCaps(hTerDC,LOGPIXELSX);
    CalcEnhMetafileDim(w,-1,&FileHdr,&width,&height);  // extract width/heigh in pixel units

    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=PICT_ENHMETAFILE;
    TerFont[pict].ObjectType=OBJ_NONE;
    TerFont[pict].PictHeight=MulDiv(height,1440,ResY); // dimension in twips
    TerFont[pict].PictWidth=MulDiv(width,1440,ResX);
    TerFont[pict].bmHeight=TerFont[pict].PictHeight;   // original picture dimensions
    TerFont[pict].bmWidth=TerFont[pict].PictWidth;
    TerFont[pict].ImageSize=ImageSize;
    TerFont[pict].InfoSize=0;
    TerFont[pict].style=PICT;
    TerFont[pict].PictAlign=align;

    //***************** create metafile  *********************
    TerFont[pict].hEnhMeta=TerSetEnhMetaFileBits(TerFont[pict].pImage); // convert to metafile
    TerFont[pict].pImage=0;
    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }


    EOR:                                  // end of routine
    CloseHandle(InFile);

    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here

    #endif                               // end of WIN32 code

    return pict;
}

#if defined(WIN32)                // Enhanced metafile supported in WIN32 only
/******************************************************************************
    CalcEnhMetafileDim:
    Calcualte the enhanced metafile dimension from the metafile header.  The width/height
    are returned in screen pixel units.
*******************************************************************************/
BOOL CalcEnhMetafileDim(PTERWND w, int pict, LPENHMETAHEADER pFileHdr, LPINT pWidth, LPINT pHeight)
{
    int width, height, ResX, ResY;
    ENHMETAHEADER FileHdr;
    
    //int MetaResX,MetaResY;

    if (pWidth) (*pWidth)=0;
    if (pHeight) (*pHeight)=0;

    // get the enh meta header from the picture if specified
    if (pict>=0 && TerFont[pict].style&PICT && TerFont[pict].hEnhMeta) {
        if (0==GetEnhMetaFileHeader(TerFont[pict].hEnhMeta,sizeof(ENHMETAHEADER),&FileHdr)) {
            PrintError(w,MSG_ERR_ENH_META_HDR,"CAlcEnhMetafileDim");
            return FALSE;
        }
        pFileHdr=&FileHdr;
    }

    ResY=GetDeviceCaps(hTerDC,LOGPIXELSY);         // resolution (pixel/inch) of the current device
    ResX=GetDeviceCaps(hTerDC,LOGPIXELSX);
    
    //MetaResY=(int)(pFileHdr->szlDevice.cy*25.4/pFileHdr->szlMillimeters.cy);
    //MetaResX=(int)(pFileHdr->szlDevice.cx*25.4/pFileHdr->szlMillimeters.cx);

    //height=pFileHdr->rclBounds.bottom-pFileHdr->rclBounds.top;  // in reference device units
    //width=pFileHdr->rclBounds.right-pFileHdr->rclBounds.left;   // in reference device units
    //height=MulDiv(height,ResY,MetaResY); // in pixels
    //width=MulDiv(width,ResX,MetaResX);   // in pixels

    height=MulDiv((pFileHdr->rclFrame.bottom-pFileHdr->rclFrame.top),ResY,2540);  // convert from .01 mm to screen pixels
    width=MulDiv((pFileHdr->rclFrame.right-pFileHdr->rclFrame.left),ResX,2540);

    if (pWidth) (*pWidth)=width;
    if (pHeight) (*pHeight)=height;

    return TRUE;
}
#endif                               // end of WIN32 code

/******************************************************************************
    TerPictureFromJpg:
    This routine asks for a tiff file name if the TifFileName is NULL.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromJpg(HWND hWnd,LPBYTE FileName,int align, BOOL insert)
{
    int    pict,CurCfmt;
    BYTE   name[MAX_WIDTH+1]="";
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    PTERWND w;
    BOOL   ReplacePicture;
    HGLOBAL hDIB;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    if (FileName!=NULL && FileName[0]==0) FileName=NULL;

    while (InFile==INVALID_HANDLE_VALUE) {
        if   (FileName) lstrcpy(name,FileName);
        else if (!GetFileName(w,TRUE,name,1,"Jpeg Files(*.JPG)|*.JPG|")) return 0;

        // check if the file exists
        if ((InFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
           if (!FileName) PrintError(w,MSG_ERR_PICT_FILE_OPEN,NULL);
           if (FileName) return 0;
           else          continue;       // get another name
        }
        CloseHandle(InFile);

        if (!Jpg2Bmp(w,TER_FILE,FileName,NULL,0,NULL,FALSE)) {
           PrintError(w,MSG_ERR_PICT_FILE_FMT,NULL);
           InFile=INVALID_HANDLE_VALUE;
           if (FileName) return 0;
           else          continue;       // get another name
        }
    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    ImgDenX=OrigScrResX;     // pixel densities - these variables are set by the image import functions //>>> RZW 3/15/02 - Initialize global variables
    ImgDenY=OrigScrResY;                                                                                //>>> RZW 3/15/02 - Initialize global variables

    // convert TIFF to bmp
    if (!Jpg2Bmp(w,TER_FILE,FileName,NULL,0,&hDIB,TRUE)) return 0;

    if ((pict=ExtractDIB(w,hDIB))==-1) {// create a device independent bitmap
        return 0;
    }
    GlobalFree(hDIB);                   // not needed any more

    if (!DIB2Bitmap(w,hTerDC,pict)) {        // translate DIB to device bitmap
        return 0;
    }

    // save the original picture information
    TerFont[pict].pOrigImage=FileToMemPtr(FileName,&TerFont[pict].OrigImageSize);
    if (TerFont[pict].OrigImageSize>0) TerFont[pict].OrigPictType=PICT_JPEG;

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }


    //EOR:                                  // end of routine
    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here

    return pict;
}

/******************************************************************************
    TerPictureFromPng:
    This routine asks for a png file name if the TifFileName is NULL.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromPng(HWND hWnd,LPBYTE FileName,int align, BOOL insert)
{
    int    pict,CurCfmt;
    BYTE   name[MAX_WIDTH+1]="";
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    PTERWND w;
    BOOL   ReplacePicture;
    HGLOBAL hDIB;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    if (FileName!=NULL && FileName[0]==0) FileName=NULL;

    while (InFile==INVALID_HANDLE_VALUE) {
        if   (FileName) lstrcpy(name,FileName);
        else if (!GetFileName(w,TRUE,name,1,"Jpeg Files(*.JPG)|*.JPG|")) return 0;

        // check if the file exists
        if ((InFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
           if (!FileName) PrintError(w,MSG_ERR_PICT_FILE_OPEN,NULL);
           if (FileName) return 0;
           else          continue;       // get another name
        }
        CloseHandle(InFile);

        if (!Png2Bmp(w,TER_FILE,FileName,NULL,0,NULL)) {
           PrintError(w,MSG_ERR_PICT_FILE_FMT,NULL);
           InFile=INVALID_HANDLE_VALUE;
           if (FileName) return 0;
           else          continue;       // get another name
        }
    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    ImgDenX=OrigScrResX;     // pixel densities - these variables are set by the image import functions //>>> RZW 3/15/02 - Initialize global variables
    ImgDenY=OrigScrResY;                                                                                //>>> RZW 3/15/02 - Initialize global variables

    // convert TIFF to bmp
    if (!Png2Bmp(w,TER_FILE,FileName,NULL,0,&hDIB)) return 0;

    if ((pict=ExtractDIB(w,hDIB))==-1) {// create a device independent bitmap
        return 0;
    }
    GlobalFree(hDIB);                   // not needed any more

    if (!DIB2Bitmap(w,hTerDC,pict)) {        // translate DIB to device bitmap
        return 0;
    }

    // save the original picture information
    TerFont[pict].pOrigImage=FileToMemPtr(FileName,&TerFont[pict].OrigImageSize);
    if (TerFont[pict].OrigImageSize>0) TerFont[pict].OrigPictType=PICT_PNG;

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }


    //EOR:                                  // end of routine
    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here

    return pict;
}

/******************************************************************************
    TerPictureFromGif:
    This routine asks for a tiff file name if the TifFileName is NULL.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromGif(HWND hWnd,LPBYTE FileName,int align, BOOL insert)
{
    int    i,pict,CurCfmt,NewPict;
    BYTE   name[MAX_WIDTH+1]="";
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    PTERWND w;
    BOOL   ReplacePicture;
    HGLOBAL hDIB;
    struct StrGifAnim far *anim=NULL;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    if (FileName!=NULL && FileName[0]==0) FileName=NULL;

    while (InFile==INVALID_HANDLE_VALUE) {
        if   (FileName) lstrcpy(name,FileName);
        else if (!GetFileName(w,TRUE,name,1,"Jpeg Files(*.JPG)|*.JPG|")) return 0;

        // check if the file exists
        if ((InFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
           if (!FileName) PrintError(w,MSG_ERR_PICT_FILE_OPEN,NULL);
           if (FileName) return 0;
           else          continue;       // get another name
        }
        CloseHandle(InFile);

        if (!Gif2Bmp(w,TER_FILE,FileName,NULL,0,NULL,NULL)) {
           PrintError(w,MSG_ERR_PICT_FILE_FMT,NULL);
           InFile=INVALID_HANDLE_VALUE;
           if (FileName) return 0;
           else          continue;       // get another name
        }
    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    ImgDenX=OrigScrResX;     // pixel densities - these variables are set by the image import functions //>>> RZW 3/15/02 - Initialize global variables
    ImgDenY=OrigScrResY;                                                                                //>>> RZW 3/15/02 - Initialize global variables

    // convert TIFF to bmp
    anim=MemAlloc(sizeof (struct StrGifAnim));
    if (!Gif2Bmp(w,TER_FILE,FileName,NULL,0,&hDIB,anim)) return 0;

    if ((pict=ExtractDIB(w,hDIB))==-1) {// create a device independent bitmap
        if (anim) MemFree(anim);
        return 0;
    }
    GlobalFree(hDIB);                   // not needed any more

    if (!DIB2Bitmap(w,hTerDC,pict)) {        // translate DIB to device bitmap
        if (anim) MemFree(anim);
        return 0;
    }

    // save the original picture information
    TerFont[pict].pOrigImage=FileToMemPtr(FileName,&TerFont[pict].OrigImageSize);
    if (TerFont[pict].OrigImageSize>0) TerFont[pict].OrigPictType=PICT_GIF;

    // do gif animation
    if (anim && anim->AnimPicts>0) {
       int LastAnim,delay;

       LastAnim=anim->AnimPicts-1;
       if (LastAnim<0) LastAnim=0;
       TerSetAnimInfo(hTerWnd,pict,anim->AnimLoops,anim->delay[LastAnim]);

       for (i=0;i<anim->AnimPicts;i++) {
          if ((NewPict=TerPastePicture(hTerWnd,CF_DIB,anim->hDIB[i],0,align,FALSE))==0) return FALSE;
          GlobalFree(anim->hDIB[i]);
          anim->hDIB[i]=NULL;

          if (i==0) delay=anim->InitDelay;
          else      delay=anim->delay[i-1];

          TerAddAnimPict(hTerWnd,pict,NewPict,delay);
       }
    }
    if (anim) MemFree(anim);

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }


    //EOR:                                  // end of routine
    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here

    return pict;
}

/******************************************************************************
    TerPictureFromTif:
    This routine asks for a Tiff file name if the TifFileName is NULL.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromTif(HWND hWnd,LPBYTE FileName,int align, BOOL insert)
{
    int    pict,CurCfmt;
    BYTE   name[MAX_WIDTH+1]="";
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    PTERWND w;
    BOOL   ReplacePicture;
    HGLOBAL hDIB;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    if (FileName!=NULL && FileName[0]==0) FileName=NULL;

    while (InFile==INVALID_HANDLE_VALUE) {
        if   (FileName) lstrcpy(name,FileName);
        else if (!GetFileName(w,TRUE,name,1,MsgString[MSG_TIF_FILES])) return 0;

        // check if the file exists
        if ((InFile=CreateFile(name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
           if (!FileName) PrintError(w,MSG_ERR_PICT_FILE_OPEN,NULL);
           if (FileName) return 0;
           else          continue;       // get another name
        }
        CloseHandle(InFile);

        if (!Tif2Bmp(w,TER_FILE,FileName,NULL,0,NULL,false)) {
           PrintError(w,MSG_ERR_PICT_FILE_FMT,NULL);
           InFile=INVALID_HANDLE_VALUE;
           if (FileName) return 0;
           else          continue;       // get another name
        }
    }

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    ImgDenX=OrigScrResX;     // pixel densities - these variables are set by the image import functions //>>> RZW 3/15/02 - Initialize global variables
    ImgDenY=OrigScrResY;                                                                                //>>> RZW 3/15/02 - Initialize global variables

    // convert TIFF to bmp
    if (!Tif2Bmp(w,TER_FILE,FileName,NULL,0,&hDIB,true)) return 0;

    if ((pict=ExtractDIB(w,hDIB))==-1) {// create a device independent bitmap
        return 0;
    }
    GlobalFree(hDIB);                   // not needed any more

    if (!DIB2Bitmap(w,hTerDC,pict)) {        // translate DIB to device bitmap
        return 0;
    }

    // save the original picture information
    //TerFont[pict].pOrigImage=FileToMemPtr(FileName,&TerFont[pict].OrigImageSize);
    //if (TerFont[pict].OrigImageSize>0) TerFont[pict].OrigPictType=PICT_TIF;

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }


    //EOR:                                  // end of routine
    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here

    return pict;
}

/******************************************************************************
    TerPictureFromOther:
    Read a picture of an unsupported format with the help of the host
    application.
    if 'insert' argument is TRUE, It load the bitmap at the current cursor
    location, otherwise it simply returns the picture id.
    The 'align' argument specifies the picture alignment relative to the the
    text base line.  The 'align' can be set to ALIGN_BOT (default),
    ALIGN_TOP, or ALIGN_MIDDLE.
    This function returns the picture id if successful. Otherwise it returns
    0.
*******************************************************************************/
int WINAPI _export TerPictureFromOther(HWND hWnd,LPBYTE FileName,int align, BOOL insert)
{
    int    pict,CurCfmt;
    PTERWND w;
    BOOL   ReplacePicture;
    HGLOBAL hDIB;
    struct StrFont SaveFont;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    // check if the cursor is on the protected text
    if (NextFontId>=0) FarMove(&(TerFont[NextFontId]),&SaveFont,sizeof(SaveFont));
    ReplacePicture=(NextFontId>=0?TRUE:FALSE);
    CurCfmt=GetCurCfmt(w,CurLine,CurCol);
    if (!CanInsert(w,CurLine,CurCol) && insert && !ReplacePicture) {
       MessageBeep(0);
       return 0;
    }

    // find an open slot in the font table
    if ((pict=FindOpenSlot(w))==-1) return 0;      // ran out of picture table

    ImgDenX=OrigScrResX;     // pixel densities - these variables are set by the image import functions //>>> RZW 3/15/02 - Initialize global variables
    ImgDenY=OrigScrResY;                                                                                //>>> RZW 3/15/02 - Initialize global variables

    // convert external format to bmp
    hDIB=(HGLOBAL)SendMessageToParent(w,TER_XLATE_PICT,(WPARAM)hTerWnd,(LPARAM)(LPBYTE)FileName,FALSE);
    if (!hDIB) return 0;

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR && insert) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference


    if ((pict=ExtractDIB(w,hDIB))==-1) {// create a device independent bitmap
        return 0;
    }
    if (!DIB2Bitmap(w,hTerDC,pict)) {        // translate DIB to device bitmap
        return 0;
    }

    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    //*************** insert the picture at the current cursor location ***
    if (insert && !ReplacePicture) {
       TerInsertObjectId(hTerWnd,pict,FALSE);
    }
    else if (ReplacePicture) {
       ApplyPrevPictProp(w,pict,&SaveFont);
       RequestPagination(w,true);
       FitPictureInFrame(w,CurLine,FALSE);  // if within a frame, resize the frame to fit the picture completely
    }

    // let the host application do any animation to this picture if necessary
    SendMessageToParent(w,TER_ANIM_PICT,(WPARAM)hTerWnd,(LPARAM)pict,FALSE);

    //EOR:                                  // end of routine
    PaintFlag=PAINT_WIN;                 // refresh from the text
    if (insert || ReplacePicture) PaintTer(w);  // suppress word-wrap when not inserting into text here


    return pict;
}

/******************************************************************************
    TerInsertObjectId:
    This function is called to insert an object into the current text postion.
******************************************************************************/
BOOL WINAPI _export TerInsertObjectId(HWND hWnd, int pict, BOOL repaint)
{
    LPBYTE ptr;
    LPWORD fmt;
    PTERWND w;
    int ParaFID;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // check for valid id
    if (pict<0 || pict>=TotalFonts || !TerFont[pict].InUse || !(TerFont[pict].style&PICT)) return FALSE;

    // create a frame if needed
    ParaFID=fid(CurLine);
    if (TerArg.PageMode && TerFlags&TFLAG_PICT_IN_FRAME
       && (ParaFID==0 || ParaFrame[ParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT))) {
       TerInsertParaFrame(hWnd,-1,-1,-1,-1,TRUE);
    }

    // create a non-frame line if necessary
    ParaFID=fid(CurLine);
    if (ParaFID && ParaFrame[ParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT)) {
        InsertMarkerLine(w,CurLine,ParaChar,0,0,0,0);
        fid(CurLine)=0;
        CurCol=0;
    }

    //*************** insert the object at the current cursor location ***
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine);
    if (CurCol<0) CurCol=0;

    MoveLineData(w,CurLine,CurCol,1,'B');   // make extra space

    ptr=pText(CurLine);
    ptr[CurCol]=PICT_CHAR;                  // any character


    fmt=OpenCfmt(w,CurLine);
    fmt[CurCol]=(WORD)pict;                 // picture index serves as the formatting attribute
    CloseCfmt(w,CurLine);

    // record for undo
    SaveUndo(w,CurLine,CurCol,CurLine,CurCol,'O');

    FitPictureInFrame(w,CurLine,(BOOL)(TerFlags&TFLAG_RESIZE_BITMAP));  // if within a frame, resize the frame to fit the picture completely

    TerArg.modified++;
    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerSavePict:
    This function saves the specified picture id to a disk file.
    This function returns TRUE when successful.
*******************************************************************************/
BOOL WINAPI _export TerSavePict(HWND hWnd, int pict, LPBYTE WmfName)
{
    PTERWND w;
    HANDLE  hFile;
    struct StrMetaFileHdr MetaHdr;
    struct StrFont SaveTerObject;
    BOOL converted=FALSE;
    LPBYTE pImage;
    int i;
    LPWORD pWord;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>=TotalFonts || !(TerFont[pict].InUse)
      || !(TerFont[pict].style&PICT) || IsControl(w,pict)) return FALSE;

    if (TerFont[pict].PictType==PICT_ENHMETAFILE) return FALSE;  // enhanced metafile not supported

    // open the output file
    if (INVALID_HANDLE_VALUE==(hFile=CreateFile(WmfName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"TerSavePict");

    // write the metafile header
    FarMemSet(&MetaHdr,0,sizeof(MetaHdr));
    MetaHdr.sign=0x9ac6cdd7;
    MetaHdr.UnitsPerInch=1000;
    MetaHdr.bottom=MulDiv(TerFont[pict].PictHeight,MetaHdr.UnitsPerInch,1440);
    MetaHdr.right=MulDiv(TerFont[pict].PictWidth,MetaHdr.UnitsPerInch,1440);

    pWord=(LPWORD)&MetaHdr;   // calculate the checksum of the first 10 words
    for (i=0;i<10;i++) MetaHdr.CheckSum^=pWord[i];

    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0L,sizeof(MetaHdr),(LPBYTE)&MetaHdr) )
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePict");

    // convert any DIB to metafile
    if (TerFont[pict].PictType==PICT_DIBITMAP) {
       SaveTerObject=TerFont[pict];
       TerFont[pict].hMeta=DIB2Metafile(w,pict);
       converted=TRUE;
    }

    // write the image data
    TerFont[pict].pImage=TerGetMetaFileBits(TerFont[pict].hMeta,null);  // convert to global handle
    pImage=(LPVOID)TerFont[pict].pImage;

    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0,TerFont[pict].ImageSize,pImage))
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePict(a)");

    if (TerFont[pict].PictType==PICT_METAFILE) {
       TerFont[pict].hMeta=TerSetMetaFileBits(TerFont[pict].pImage);  // convert to metafile handle
       TerFont[pict].pImage=0;
    }

    // reconvert to DIB from metafile
    if (converted) {
       if (TerFont[pict].hMeta)  DeleteMetaFile(TerFont[pict].hMeta); // delete metafile
       TerFont[pict]=SaveTerObject;              // restore the object
    }

    CloseHandle(hFile);

    return TRUE;
}

/******************************************************************************
    TerSaveEmf:
    This function saves the specified picture id to a disk file.
    The picture must be in the enhanced metafile format.
    This function returns TRUE when successful.
*******************************************************************************/
BOOL WINAPI _export TerSavePictEmf(HWND hWnd, int pict, LPBYTE EmfName)
{
    PTERWND w;
    HANDLE  hFile;
    LPBYTE pImage;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>=TotalFonts || !(TerFont[pict].InUse)
      || !(TerFont[pict].style&PICT) || IsControl(w,pict)) return FALSE;

    if (TerFont[pict].PictType!=PICT_ENHMETAFILE) return FALSE;  // enhanced metafile not supported

    // open the output file
    if (INVALID_HANDLE_VALUE==(hFile=CreateFile(EmfName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"TerSavePictEmf");

    // write the image data
    pImage=(LPVOID)TerGetEnhMetaFileBits(TerFont[pict].hEnhMeta);  // convert to global handle

    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0,TerFont[pict].ImageSize,pImage))
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePictEmf(a)");

    TerFont[pict].hEnhMeta=TerSetEnhMetaFileBits(pImage);  // convert to metafile handle

    CloseHandle(hFile);

    return TRUE;
}

/******************************************************************************
    TerSavePictDIB:
    This function saves the specified picture id in the DIB format to a disk
    file. This function returns TRUE when successful. The picture must
    point to a valid DIB picture inside the font table.
*******************************************************************************/
BOOL WINAPI _export TerSavePictDIB(HWND hWnd, int pict, LPBYTE FileName)
{
    PTERWND w;
    HANDLE  hFile;
    BITMAPFILEHEADER FileHdr;
    BOOL converted=FALSE;
    LPBYTE pImage,pInfo;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>=TotalFonts || !(TerFont[pict].InUse)
      || !(TerFont[pict].style&PICT) || IsControl(w,pict)) return FALSE;
    if (TerFont[pict].PictType!=PICT_DIBITMAP) return FALSE;

    // open the output file
    if (INVALID_HANDLE_VALUE==(hFile=CreateFile(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"TerSavePictDIB");

    // write the metafile header
    FarMemSet(&FileHdr,0,sizeof(FileHdr));
    FileHdr.bfType=(((WORD)'M')<<8)|'B';    // signature of the bitmap file
    FileHdr.bfSize=sizeof(FileHdr)+TerFont[pict].InfoSize+TerFont[pict].ImageSize;
    FileHdr.bfOffBits=sizeof(FileHdr)+TerFont[pict].InfoSize;

    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0L,sizeof(FileHdr),(LPBYTE)&FileHdr) )
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePict");


    // write the info data
    pInfo=(LPVOID)TerFont[pict].pInfo;

    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0,TerFont[pict].InfoSize,pInfo))
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePictDIB(a)");


    // write the image data
    pImage=(LPVOID)TerFont[pict].pImage;

    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0,TerFont[pict].ImageSize,pImage))
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePictDIB(a)");

    CloseHandle(hFile);

    return TRUE;
}

/******************************************************************************
    TerSavePictJpg:
    This function saves the specified picture id in the jpeg format.  The
    picture must originally be read in the jpeg format. 
*******************************************************************************/
BOOL WINAPI _export TerSavePictJpg(HWND hWnd, int pict, LPBYTE FileName)
{
    PTERWND w;
    HANDLE  hFile;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (pict<0 || pict>=TotalFonts || !(TerFont[pict].InUse)
      || !(TerFont[pict].style&PICT) || IsControl(w,pict)) return FALSE;
    if (TerFont[pict].OrigPictType!=PICT_JPEG || !TerFont[pict].pOrigImage || TerFont[pict].OrigImageSize==0) return FALSE;

    // open the output file
    if (INVALID_HANDLE_VALUE==(hFile=CreateFile(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"TerSavePictJpg");


    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0,TerFont[pict].OrigImageSize,TerFont[pict].pOrigImage))
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePictJpg(a)");

    CloseHandle(hFile);

    return TRUE;
}

/******************************************************************************
    TerSavePictGif:
    This function saves the specified picture id in the jpeg format.  The
    picture must originally be read in the jpeg format. 
*******************************************************************************/
BOOL WINAPI _export TerSavePictGif(HWND hWnd, int pict, LPBYTE FileName)
{
    PTERWND w;
    HANDLE  hFile;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    
    if (pict<0 || pict>=TotalFonts || !(TerFont[pict].InUse)
      || !(TerFont[pict].style&PICT) || IsControl(w,pict)) return FALSE;
    if (TerFont[pict].OrigPictType!=PICT_GIF || !TerFont[pict].pOrigImage || TerFont[pict].OrigImageSize==0) return FALSE;

    // open the output file
    if (INVALID_HANDLE_VALUE==(hFile=CreateFile(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))) 
          return PrintError(w,MSG_ERR_FILE_OPEN,"TerSavePictGif");


    if (!TerFarWrite(w,'F',hFile,NULL,NULL,0,TerFont[pict].OrigImageSize,TerFont[pict].pOrigImage))
       return PrintError(w,MSG_ERR_FILE_WRITE,"TerSavePictGif(a)");

    CloseHandle(hFile);

    return TRUE;
}

/******************************************************************************
    TerFileToMem:
    This utility function loads a file into a global memory buffer and returns
    its handle and size.
*******************************************************************************/
HGLOBAL WINAPI _export TerFileToMem(LPBYTE file, DWORD far *pSize)
{
    HANDLE hFile=INVALID_HANDLE_VALUE;
    long size=0;
    DWORD BytesRead;
    HGLOBAL hMem=NULL;
    LPBYTE pMem=NULL;

    hFile=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile!=INVALID_HANDLE_VALUE) {
       size=SetFilePointer(hFile,0,NULL,FILE_END); 
       SetFilePointer(hFile,0,NULL,FILE_BEGIN);
       if (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,size+1)) &&
           NULL!=(pMem=GlobalLock(hMem))) {
           ReadFile(hFile,pMem,size,&BytesRead,NULL);
           if (size!=(long)BytesRead) return FALSE;
           GlobalUnlock(hMem);
       }
       CloseHandle(hFile);
    }

    if (pSize) (*pSize)=size;

    return hMem;
}

/******************************************************************************
    FileToMemPtr:
    This utility function loads a file into a memory buffer and returns
    pointer (OurAlloc) to this buffer
*******************************************************************************/
LPBYTE FileToMemPtr(LPBYTE file, DWORD far *pSize)
{
    HANDLE hFile=INVALID_HANDLE_VALUE;
    long size=0;
    DWORD BytesRead;

    LPBYTE pMem=NULL;

    if (pSize) (*pSize)=0;

    hFile=CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile!=INVALID_HANDLE_VALUE) {
       size=SetFilePointer(hFile,0,NULL,FILE_END);
       SetFilePointer(hFile,0,NULL,FILE_BEGIN);  // reposition
       if (NULL!=(pMem=OurAlloc(size+1))) {
           ReadFile(hFile,pMem,size,&BytesRead,NULL);
           if (size!=(long)BytesRead) return NULL;
       }
       CloseHandle(hFile);
    }

    if (pSize) (*pSize)=size;

    return pMem;
}

/******************************************************************************
    IsControl:
    Returns TRUE is the current picture id is a control (PICT_CONTROL or PICT_FORM)
*******************************************************************************/
BOOL IsControl(PTERWND w, int pict)
{
    return (  pict>=0 && pict<TotalFonts && TerFont[pict].InUse && TerFont[pict].style&PICT
           && (TerFont[pict].PictType==PICT_CONTROL || TerFont[pict].PictType==PICT_FORM));
}

/******************************************************************************
    GetPictFileType:
    Get picture file type: PICT_DIB, PICT_METAFILE, or PICT_NONE
*******************************************************************************/
int GetPictFileType(PTERWND w, LPBYTE FileName, BOOL msg)
{
    HANDLE  InFile=INVALID_HANDLE_VALUE;
    BITMAPFILEHEADER hdr;
    int BytesRead;
    WORD   BmSign;
    struct StrMetaFileHdr MetaHdr;

    // open the bitmap file
    if ((InFile=CreateFile(FileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) {
       if (msg) PrintError(w,MSG_ERR_PICT_FILE_OPEN,FileName);
       return FTYPE_ERR;
    }

    // check for the bitmap file name
    SetFilePointer(InFile,0,NULL,FILE_BEGIN);           // reposition the file

    ReadFile(InFile,&hdr,sizeof(BITMAPFILEHEADER),&((DWORD)BytesRead),NULL);
    if (BytesRead!=sizeof(BITMAPFILEHEADER)) {
       CloseHandle(InFile);
       goto CHECK_META;
    }


    BmSign=(((WORD)'M')<<8)|'B';    // signature of the bitmap file
    if (hdr.bfType==BmSign) {
       CloseHandle(InFile);
       return FTYPE_DIB;
    }


    CHECK_META:
    // check for the metafile file
    SetFilePointer(InFile,0,NULL,FILE_BEGIN);           // reposition the file

    ReadFile(InFile,&MetaHdr,sizeof(struct StrMetaFileHdr),&((DWORD)BytesRead),NULL);
    if (BytesRead!=sizeof(struct StrMetaFileHdr)) {
       CloseHandle(InFile);
       goto CHECK_ENH_META;
    }

    // test if metafile
    if (MetaHdr.sign==0x9ac6cdd7) {
       CloseHandle(InFile);
       return FTYPE_METAFILE;
    }

    CHECK_ENH_META:

    // check for the enhanced metafile file
    #if defined(WIN32)
    {
       ENHMETAHEADER EnhMetaHdr;

       SetFilePointer(InFile,0,NULL,FILE_BEGIN);           // reposition the file

       ReadFile(InFile,&EnhMetaHdr,sizeof(ENHMETAHEADER),&((DWORD)BytesRead),NULL);
       if (BytesRead!=sizeof(ENHMETAHEADER)) {
          //if (msg) PrintError(w,MSG_ERR_META_READ,NULL);
          CloseHandle(InFile);
          //return FTYPE_ERR;
          goto CHECK_JPG;
       }

       // test if metafile
       if (EnhMetaHdr.dSignature==ENHMETA_SIGNATURE) {
          CloseHandle(InFile);
          return FTYPE_ENHMETAFILE;
       }
    }
    #endif


    CloseHandle(InFile);

    CHECK_JPG:

    // check other picture types
    if (Jpg2Bmp(w,TER_FILE,FileName,NULL,0,NULL,FALSE)) return FTYPE_JPEG;

    if (Png2Bmp(w,TER_FILE,FileName,NULL,0,NULL)) return FTYPE_PNG;

    if (Gif2Bmp(w,TER_FILE,FileName,NULL,0,NULL,NULL)) return FTYPE_GIF;
    
    if (Tif2Bmp(w,TER_FILE,FileName,NULL,0,NULL,FALSE)) return FTYPE_TIF;

    return FTYPE_NONE;
}

/******************************************************************************
    RealizeControl:
    Create a control window
*******************************************************************************/
HWND RealizeControl(PTERWND w, int pict)
{
    HWND hCtl;
    struct StrControl far *pCtl;
    struct StrForm far *pForm;
    int  width,height,NewWidth,i;


    // calculate the width/height in pixels
    width=NewWidth=TwipsToScrX(TerFont[pict].PictWidth);
    height=TwipsToScrX(TerFont[pict].PictHeight);

    if (TerFont[pict].PictType==PICT_CONTROL || TerFont[pict].PictType==PICT_FORM) {
       if (TerFont[pict].PictType==PICT_CONTROL) {
          pCtl=(LPVOID)TerFont[pict].pInfo;
          hCtl=CreateWindow(pCtl->class,"",pCtl->style,TerRect.right,TerRect.bottom,width,height,hTerWnd,(HMENU)pCtl->id,hTerInst,NULL);
       }
       else {
          pForm=(LPVOID)TerFont[pict].pInfo;

          // assign a control id if not already assigned
          if (pForm->id==0) {
             int id=0;
             for (i=0;i<TotalFonts;i++) {
                if (   TerFont[i].InUse && TerFont[i].style&PICT
                    && (TerFont[i].PictType==PICT_CONTROL || TerFont[i].PictType==PICT_FORM)
                    && TerFont[i].AuxId>id) id=TerFont[i].AuxId;
             }
             id++;   // get the next id
             if (id<23000) id=23000;     // let the form ids begin from 23000 to reduce conflict with the parent window menu ids
             pForm->id=TerFont[pict].AuxId=id;
          }
          hCtl=CreateWindow(pForm->class,"",pForm->style,TerRect.right,TerRect.bottom,width,height,hTerWnd,(HMENU)pForm->id,hTerInst,NULL);
///////////////////////////////////////////////////////////////////////////////////////
			// MAK
					if(pForm->InitText && strlen(pForm->InitText))
						SetWindowText(hCtl,pForm->InitText);

			 }
       if (!hCtl) {
           PrintError(w,MSG_ERR_CTL_CREATE,NULL);
           return NULL;
       }

       TerFont[pict].hWnd=hCtl;                         // store the window handle
    }

    // set font
    if (TerFont[pict].PictType==PICT_FORM && TerFont[pict].FieldId==FIELD_TEXTBOX) {
       SetTextInputFieldWnd(w,pict,TerFont[pict].PictWidth,TRUE);
    }
    else if (TerFont[pict].PictType==PICT_FORM && TerFont[pict].FieldId==FIELD_CHECKBOX) {
        if (pForm->InitNum) SendMessage(hCtl,BM_SETCHECK, TRUE, 0L);   // set the check box
    }



    //***************** set control size  *********************
    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
    TerFont[pict].bmHeight=TerFont[pict].height;       // actual height of the picture
    TerFont[pict].bmWidth=TerFont[pict].CharWidth[PICT_CHAR];
    TerFont[pict].TwipsSize=TerFont[pict].PictHeight;

    XlateSizeForPrt(w,pict);                       // convert to printer dimensions

    // subclass the form fields
    if (TerFont[pict].PictType==PICT_FORM) {
       if (!FormFieldSubclassProc) FormFieldSubclassProc=(WNDPROC)MakeProcInstance((FARPROC)FormFieldSubclass, hTerInst);
       pForm->OrigWndProc=(WNDPROC)(DWORD)GetWindowLong(hCtl,GWL_WNDPROC); // original control proceedure
       SetWindowLong(hCtl,GWL_WNDPROC,(DWORD)FormFieldSubclassProc);
    }

    if (TerFont[pict].PictType==PICT_CONTROL) return hCtl;
    else                                      return (HWND)TRUE;
}

/******************************************************************************
    SetTextInputFieldWnd:
    Set text input field window.
*******************************************************************************/
SetTextInputFieldWnd(PTERWND w, int pict,int CurCtlWidth, BOOL SetHeight)
{
    struct StrForm far *pForm=(LPVOID)TerFont[pict].pInfo;
    int  width,height,NewWidth;
    HWND hCtl=TerFont[pict].hWnd;
    UINT style,TempStyle=0;
    DWORD margins;
    int CtlLineCount;
    
    width=CurCtlWidth;                               // current width of the control
    NewWidth=TwipsToScrX(TerFont[pict].PictWidth);   // requested new width of the control

    height=TwipsToScrX(TerFont[pict].PictHeight);

    style=pForm->FontStyle;
    if (style&ULINE) TempStyle|=TSTYLE_ULINEF;
    if (pForm->FontId<0) 
        pForm->FontId=GetNewFont(w,hTerDC,0,pForm->typeface,pForm->TwipsSize,style&(BOLD|ITALIC|ULINE),pForm->TextColor,pForm->TextBkColor,CLR_AUTO,0,0,0,0,0,0,TempStyle,0,NULL,0,0,(BYTE)(DWORD)pForm->CharSet,0,0);
    
    if (pForm->FontId>=0) {  // apply the font
        int CurFont=pForm->FontId;
        HFONT hFont=TerFont[CurFont].hFont;

        // change the font
        margins=SendMessage(hCtl,EM_GETMARGINS,0,0);    // restore the editor control margin after setting the font
        SendMessage(hCtl,WM_SETFONT,(WPARAM)hFont,0L);
        SendMessage(hCtl,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,MAKELPARAM(LOWORD(margins), HIWORD(margins)));

        // calculate the height for the font
        TerFont[pict].PictAlign=ALIGN_MIDDLE;
        if (pForm->style&WS_BORDER) TerFont[pict].height=TerFont[CurFont].height*5/4;
        else                        TerFont[pict].height=TerFont[CurFont].height;
        if (SetHeight) {
           TerFont[pict].PictHeight=ScrToTwipsY(TerFont[pict].height);
        }

        // recalculate the width if max len specified
        if (TRUE /*!(pForm->flags&FMFLAG_SSFLDW_FOUND)*/) {  // don't change width if explicitly specified in the RTF file
            int CurTextLen=GetWindowTextLength(hCtl);
            LPBYTE pText;
            if (pForm->MaxLen==0 && CurTextLen>0) {
               pText=OurAlloc(CurTextLen+1);
               GetWindowText(hCtl,pText,CurTextLen+1);
            }
            else /*if (pForm->MaxLen>0)*/ {
               int i;
               CurTextLen=pForm->MaxLen;
               if (CurTextLen==0) CurTextLen=20;
               pText=OurAlloc(CurTextLen+1);
               for (i=0;i<CurTextLen;i++) pText[i]='a';
            } 
            pText[CurTextLen]=0;
            if (CurTextLen>0 /*&& (CurTextLen==pForm->MaxLen || pForm->MaxLen==0)*/) {
               SIZE size;
               BOOL big=FALSE;
               int  i,PictCell=0,col,len,LineCount,MaxSize=(TerWinWidth*5/6),ParaCount;
               long line=-1;
               HWND hCtl;


               HFONT hOldFont=SelectObject(hTerDC,hFont);
               GetTextExtentPoint(hTerDC,pText,CurTextLen,&size);
               SelectObject(hTerDC,hOldFont);

               if (pForm->flags&FMFLAG_SSFLDW_FOUND) MaxSize=TwipsToScrX(TerFont[pict].PictWidth);

               // find allowable width for the input field
               line=0;
               col=0;
               if (TerLocateFontId(hTerWnd,pict,&line,&col)) {
                  MaxSize=TerWrapWidth(w,line,-1)*5/6;
                  PictCell=cid(line);
               }  


               // check if the text has any para breaks
               len=lstrlen(pForm->InitText);
               ParaCount=0;
               for (i=0;i<len;i++) if (pForm->InitText[i]==ParaChar) ParaCount++;

               if (size.cx>MaxSize || ParaCount>0) {
                  LineCount=(size.cx/MaxSize)+1+ParaCount;
                  
                  RESIZE:
                  size.cx=MaxSize;
                  TerFont[pict].height=TerFont[pict].height*LineCount;
                  if (PictCell>0) {    // make sure that the field is contained in the table row
                    int row=cell[PictCell].row;
                    if (TableRow[row].MinHeight<0) {    // exact row height
                      int height=TwipsToScrY(abs(TableRow[row].MinHeight))*5/6;
                      if (TerFont[pict].height>height) {
                         TerFont[pict].height=height;
                         SetHeight=TRUE;
                      }
                    } 
                  } 
                  
                  if (SetHeight) TerFont[pict].PictHeight=ScrToTwipsY(TerFont[pict].height);
                  if (TRUE || !(pForm->style&ES_MULTILINE)) {
                     DestroyWindow(TerFont[pict].hWnd);

                     pForm->style|=ES_MULTILINE;
                     ResetLongFlag(pForm->style,ES_AUTOHSCROLL);
                     TerFont[pict].PictAlign=ALIGN_TOP;

                     height=TerFont[pict].height;
                     
                     if (pForm->flags&FMFLAG_SSFLDW_FOUND) size.cx=TwipsToScrX(TerFont[pict].PictWidth);
                     width=size.cx;

                     hCtl=CreateWindow(pForm->class,"",pForm->style,TerRect.right,TerRect.bottom,width,height,hTerWnd,(HMENU)pForm->id,hTerInst,NULL);
                     TerFont[pict].hWnd=hCtl;

                     // re-apply font to the recreated control
                     margins=SendMessage(hCtl,EM_GETMARGINS,0,0);    // restore the editor control margin after setting the font
                     SendMessage(hCtl,WM_SETFONT,(WPARAM)hFont,0L);
                     SendMessage(hCtl,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,MAKELPARAM(LOWORD(margins), HIWORD(margins)));

                     if (ParaCount==0) SetWindowText(hCtl,pForm->InitText);
                     else {
                        int len=lstrlen(pForm->InitText);
                        LPBYTE pText=MemAlloc(len+1+ParaCount);
                        int i,j=0;
                        for (i=0;i<(len+1);i++,j++) {
                           if (pForm->InitText[i]==ParaChar) {
                              pText[j]=0xd;
                              j++;
                              pText[j]=0xa;
                           }
                           else pText[j]=pForm->InitText[i]; 
                        } 
                        SetWindowText(hCtl,pText);
                        MemFree(pText);
                     } 
                     
                     // check if the control is big enough to contain all lines
                     CtlLineCount=(int)SendMessage(hCtl,EM_GETLINECOUNT,0,0L);
                     if (CtlLineCount!=LineCount) {
                        LineCount=CtlLineCount;
                        goto RESIZE;
                     } 
                     
                  }
                  

                   
               } 
               if (pForm->flags&FMFLAG_SSFLDW_FOUND) size.cx=TwipsToScrX(TerFont[pict].PictWidth);
               TerFont[pict].PictWidth=ScrToTwipsX(size.cx);
               NewWidth=size.cx;
            }
            OurFree(pText);
        }

        // Apply the height to the control
        if (TerFont[pict].height!=height || NewWidth!=width) {
            height=TerFont[pict].height;
            width=NewWidth;
            SetWindowPos(hCtl,NULL,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
        }

        // set max charaction input
        //MAK SendMessage(hCtl,EM_LIMITTEXT,pForm->MaxLen,0L);
    }

    return TRUE;    
}
    
/******************************************************************************
    EditPicture:
    Edit picture size
*******************************************************************************/
EditPicture(PTERWND w)
{
    int  pict;

    pict=GetCurCfmt(w,CurLine,CurCol);   // get the format id for the current character

    if (!(TerFont[pict].style&PICT)) return FALSE;

    if (CallDialogBox(w,"EditPictureParam",EditPictureParam,0L)) {
        ReleaseUndo(w);

        // calculate size in screen units
        TerFont[pict].flags|=FFLAG_RESIZED;
        SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
        XlateSizeForPrt(w,pict);
        TerArg.modified++;
        PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
    InsertBuffer:
    Insert the contents of a text buffer in the current position.  The first
    argument points to the text buffer.  The text lines in the buffer are
    delimited by the <CR><LF> pair.  This routine expects a <CR><LF> pair
    after an average length of line (less than 80 to 100 characters).
    The text buffer may contain the 'ParaChar' character as a delimiter to
    end a paragraph.  If the paragraph character is not used,  this routine
    will determine if the paragraphs should be constructed using a smart
    guess technique.

    The second argument is the a buffer containing the formatting data.
    If used, this buffer contains one byte data for each character in the
    the text buffer.  Thus the length of the two buffers must be the same.
    Each format byte in the format buffer is an index into the TerFont
    table.  If the format buffer is not used (fmt=NULL),  the default
    format will be used.
    The 'lead' argument is used only in the mbcs environment.  If 'lead' specified
    then the 'ptr' is assumed to have single byte text.  Otherwise 'ptr' can
    contain mbcs text.
*******************************************************************************/
void InsertBuffer(PTERWND w,BYTE huge *ptr,LPWORD pUcBase, BYTE huge *lead,WORD huge *fmt,int huge *pPfmt, BOOL TerFormat)
{
    int i,NewLen,OldLen,len,DefPfmtId=-1;
    long StartLine,EndLine;
    struct StructTextBuf info;
    LPBYTE ptr1;
    LPWORD fmt1,ct1;
    WORD CurFmt;
    long UndoBegRow,UndoBegPos,UndoEndRow,UndoEndPos;
    int  UndoBegCol,UndoEndCol,SavePfmt;
    BOOL allocated=FALSE;
    BOOL CurMbcs=(mbcs /*&& pUcBase==NULL*/);  // is mbcs processing allowed here

    // save undo begin position
    UndoBegPos=RowColToAbs(w,CurLine,CurCol);

    if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;

    // extract lead bytes from ptr
    if (CurMbcs) {
       if (lead==NULL) {
         BYTE huge *TempPtr=ptr;
         long BufLen=hstrlen(ptr);

         ptr=OurAlloc(BufLen+2);
         lead=OurAlloc(BufLen+2);
         if (!ptr || !lead) return;

         if (pUcBase==NULL) TerSplitMbcs(hTerWnd,TempPtr,ptr,lead);
         else {
            WORD huge *pOutUcBase;
            pOutUcBase=OurAlloc(sizeof(WORD)*(BufLen+2));
            if (!pOutUcBase) return;
            TerSplitMbcs3(w,TempPtr,pUcBase,ptr,lead,pOutUcBase,0);
            pUcBase=pOutUcBase;
         } 
         
         allocated=TRUE;
       }
    }
    else lead=NULL;

    // extract the default pfmt id
    if (pPfmt && pPfmt[1]==-1) {
       DefPfmtId=pPfmt[0];                // default para id
       pPfmt=NULL;                        // array on line basis not usable
    }

    // Split current line if the cursor is after the paragraph marker to protect current line paragraph attributes
    ptr1=pText(CurLine);
    for (i=LineLen(CurLine)-1;i>=0;i--) if (ptr1[i]==ParaChar) break;
    if (i>=0 && CurCol>i) {
       SplitLine(w,CurLine,CurCol,0);  // cursor beyond the paragraph marker
       CurLine++;
       CurCol=0;
    }

    // initialize the structure
    info.pBuf=ptr;                        // pointer to the buffer
    info.lead=lead;                       // pointer to the lead buffer
    info.pFmt=fmt;                        // pointer to the format data
    info.index=0;                         // initialize the index into buffer data
    info.len=0;                           // length of the previous line
    info.eol=FALSE;                       // reset end of line flag
    info.eof=FALSE;                       // reset end of file (buffer)
    info.ParaCharFound=FALSE;             // reset para BYTE flag
    StartLine=EndLine=CurLine;            // lines affected by the copy

    while (!info.eof) {
       if (TerArg.WordWrap) info.MaxLineLen=(3*LineWidth/4)-LineLen(CurLine)-1;
       else                 info.MaxLineLen=LineWidth-LineLen(CurLine)-1;
       if (info.MaxLineLen<=0) info.MaxLineLen=10; // This will happen only with LineLen has reached the max, possibly because of large number of
                                                  // hidden characters.  Allows 10 characters here to avoid a loop.
       NextBufferLine(w,&info);
       if (info.eof && info.len==0) break;

       // insert the buffer line to the current line at the current column position
       OldLen=LineLen(CurLine);
       NewLen=LineLen(CurLine)+info.len;  // new length of the current line
       //if (NewLen>LineWidth) NewLen=LineWidth;

       LineAlloc(w,CurLine,LineLen(CurLine),NewLen);

       if (CurCol<OldLen)                // make space for the new line
          MoveCharInfo(w,CurLine,CurCol,CurLine,CurCol+info.len,OldLen-CurCol);

       ptr1=pText(CurLine);
       OpenCharInfo(w,CurLine,&fmt1,&ct1);
       if (CurMbcs) OpenLead(w,CurLine);

       FarMove(&(info.pBuf[info.index]),&ptr1[CurCol],info.len);
       if (CurMbcs) {
          if (pUcBase==NULL) FarMove(&(info.lead[info.index]),&(pLead(CurLine)[CurCol]),info.len);
          else {
            for (i=0;i<info.len;i++) pLead(CurLine)[CurCol+i]=0;
          } 
       }

       if (info.pFmt==NULL) {            // use the default formatting
          if (InputFontId>=0)      CurFmt=(WORD)InputFontId;
          else if (CurCol>0)       CurFmt=fmt1[CurCol-1];
          else                     CurFmt=GetPrevCfmt(w,CurLine,CurCol);
          if (TerFont[CurFmt].style&PICT && InputFontId<0) CurFmt=DEFAULT_CFMT;
          if (InputFontId==-1 && TerFont[CurFmt].FieldId>0 && CurFmt!=fmt1[CurCol]) CurFmt=fmt1[CurCol];// DEFAULT_CFMT;  // to be able to insert text after a page number text
          
          if (!(TerOpFlags2&TOFLAG2_NO_SET_LANG)) CurFmt=SetCurLangFont(w,CurFmt);  // apply the current language

          for (i=0;i<info.len;i++) { // copy the font id (apply UcBase if needed) to all characters in the line
             fmt1[CurCol+i]=SetUniFont(w,(int)CurFmt,(WORD)(pUcBase?pUcBase[info.index+i]:0));
          }
       }
       else FarMove(&(info.pFmt[info.index]),&fmt1[CurCol],sizeof(WORD)*(info.len));
       for (i=0;i<info.len;i++) ct1[CurCol+i]=0;

       CloseCharInfo(w,CurLine);
       if (CurMbcs) CloseLead(w,CurLine);

       // assign the paragraph formatting information
       SavePfmt=pfmt(CurLine);
       if (pPfmt)             pfmt(CurLine)=pPfmt[CurLine-StartLine];
       else if (DefPfmtId>=0) pfmt(CurLine)=DefPfmtId;
       if (pfmt(CurLine)>=TotalPfmts) pfmt(CurLine)=0;  // default format

       CurCol=CurCol+info.len;            // advance the column position

       if (info.eol || info.MaxLineLen==0 || info.ParaCharFound) {
          if (!CheckLineLimit(w,TotalLines+1)) {
             PrintError(w,MSG_MAX_LINES_EXCEEDED,MsgString[MSG_ERR_BLOCK_SIZE]);
             break;
          }

          MoveLineArrays(w,CurLine,1,'A');  // make new line after the current line
          pfmt(CurLine+1)=SavePfmt;         // restore pfmt for the existing line
          tabw(CurLine+1)=tabw(CurLine);    // transfer the break information
          tabw(CurLine)=NULL;
          EndLine++;                        // lines affected by the copy

          NewLen=LineLen(CurLine)-CurCol;
          if (NewLen<0) NewLen=0;         // length of the next line
          LineAlloc(w,CurLine+1,0,NewLen);
          if (NewLen>0) {                 // split text to the next line
             MoveCharInfo(w,CurLine,CurCol,CurLine+1,0,NewLen);

             NewLen=LineLen(CurLine)-NewLen;// New length of the current line
             if (NewLen<0) NewLen=0;
             LineAlloc(w,CurLine,LineLen(CurLine),NewLen);
          }

          // append cr/lf if it is a clipboard buffer
          if (!TerFormat && TerArg.WordWrap && info.eol) {
             len=LineLen(CurLine);
             ptr1=pText(CurLine);  // split the text
             if (len==0 || (ptr1[len-1]!=ParaChar || ptr1[len-1]!=CellChar)) {

                LineAlloc(w,CurLine,len,len+1);  // allocate space for para char
                ptr1=pText(CurLine);
                ptr1[len]=ParaChar;

                OpenCharInfo(w,CurLine,&fmt1,&ct1);      // split the format info
                if (InputFontId>=0) fmt1[len]=(WORD)InputFontId;
                else if (len>0)     fmt1[len]=fmt1[len-1];
                else                fmt1[len]=GetPrevCfmt(w,CurLine,len);
                if (InputFontId==-1 && TerFont[fmt1[len]].FieldId>0) {  // to be able to insert text after a page number text
                   int NextFont=GetNextCfmt(w,CurLine,len);
                   if (NextFont!=fmt1[len]) fmt1[len]=0;
                } 
                
                if (TerFont[fmt1[len]].style&PICT) fmt1[len]=0;
                
                fmt1[len]=SetUniFont(w,fmt1[len],0);    // reset any unicode font

                ct1[len]=0;
                CloseCharInfo(w,CurLine);
             }

          }
          if (TerFlags5&TFLAG5_INPUT_TO_UNICODE) {
            if (CurMbcs) MbcsToUnicode(w);
            else TextToUnicode(w);
          }

          if (info.ParaCharFound) LineFlags(CurLine)|=LFLAG_PARA;

          CurLine++;
          if (CurLine>=TotalLines) CurLine=TotalLines-1;
          CurCol=0;
       }
    }

    if (TerFlags5&TFLAG5_INPUT_TO_UNICODE) {
      if (CurMbcs) MbcsToUnicode(w);
      else TextToUnicode(w);
    }

    // record for undo
    AbsToRowCol(w,UndoBegPos,&UndoBegRow,&UndoBegCol);
    UndoEndPos=RowColToAbs(w,CurLine,CurCol);      // absolute ending position
    AbsToRowCol(w,UndoEndPos-1,&UndoEndRow,&UndoEndCol);
    SaveUndo(w,UndoBegRow,UndoBegCol,UndoEndRow,UndoEndCol,'I');

    if (allocated) OurFree(lead);
    if (allocated) OurFree(ptr);
    if (allocated) OurFree(pUcBase);

    TerArg.modified++;
}


/******************************************************************************
    NextBufferLine:
    Parse the buffer data to get the next line.  If <CR,LF> pair is detected
    at the end of the line,  the pInfo->eol flag is set.  If a NULL is detected,
    the pInfo->.eof flag is set.  'index' gives the index to the next line and 'len'
    provide the length of the line excluding <CR,LF>.
******************************************************************************/
NextBufferLine(PTERWND w,struct StructTextBuf far *pInfo)
{

     BYTE CurChar;
     WORD CurFmt;


     pInfo->index=pInfo->index+pInfo->len;// index to the next line
     if (pInfo->eol) pInfo->index++;      // add LF
     if (pInfo->eol && pInfo->CrFound) pInfo->index++;     // add CR
     pInfo->len=0;                        // reset length of the next line
     pInfo->eol=pInfo->eof=FALSE;         // reset flags
     pInfo->ParaCharFound=FALSE;
     pInfo->CrFound=FALSE;

     do {                                 // find <CR,LF> or a NULL character
        CurChar=(pInfo->pBuf)[pInfo->index+pInfo->len];// current character

        // Ignore section break character
        if (CurChar==SECT_CHAR) {
           if (pInfo->len==0) {
              pInfo->index++;
              continue;                   // ignore section BYTE in first place
           }
           break;                         // break line
        }

        // validate the format character
        if (pInfo->pFmt) {                               // validate font
           CurFmt=(pInfo->pFmt)[pInfo->index+pInfo->len];// current font
           if (CurFmt>(WORD)TotalFonts || !(TerFont[CurFmt].InUse))
              (pInfo->pFmt)[pInfo->index+pInfo->len]=DEFAULT_CFMT;  // use default font
        }

        if (CurChar==0) {                // end of clipboard data
           pInfo->eof=TRUE;
           return TRUE;
        }

        if (CurChar==0xA){// end of line
           if (pInfo->CrFound) pInfo->len--;                 // decrement for CR
           pInfo->eol=TRUE;
           return TRUE;
        }
        if (CurChar==0xD) pInfo->CrFound=TRUE;else pInfo->CrFound=FALSE;

        if ((CurChar==ParaChar || CurChar==CellChar) && TerArg.WordWrap) {
           if (((pInfo->pBuf)[pInfo->index+pInfo->len+1])!=0xD) pInfo->ParaCharFound=TRUE;
           pInfo->len++;                 // include the para character
           return TRUE;
        }

        if (pInfo->len>=pInfo->MaxLineLen) {    // force line break
           pInfo->MaxLineLen=0;                 // indicates a line break
           return TRUE;
        }

        pInfo->len++;
     } while (TRUE);

     return TRUE;
}

