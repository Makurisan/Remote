/*==============================================================================
   TER Editor Developer's Kit
   TER_WRAP.C
   Text wrapping functions

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
    WordWrap:
    wrap lines starting from the StartLine until total lines equal to
    WrapLines are produced.
*******************************************************************************/
WordWrap(PTERWND w,long StartLine,long WrapLines)
{
    long  l,LinesWrapped,SaveTotalLines;
    LPBYTE ptr;
    LPWORD fmt;
    BYTE LastChar;
    BOOL SaveModified;
    BOOL BreakFound=FALSE;
    int  LastPfmt,pass=0;
    long HilightBeg,HilightEnd,CursorLoc,PrevLastWrappedLine;
    int  len,MaxLineWidth,PixLineWidth;
    BOOL SaveJust=FALSE;
    BOOL BegHilightAtLineEnd=FALSE,EndHilightAtLineEnd=FALSE,SelectAll=FALSE;
    BOOL SyncEditLine;
    BOOL FileWrap=(StartLine==0 && WrapLines==TotalLines); //entire file needs to be wrapped

    dm("WordWrap");

    if (!TerArg.WordWrap) return TRUE;

    // wrap enough lines to cover the current line
    if (StartLine>CurLine) StartLine=CurLine;
    if (StartLine<0) StartLine=0;
    if (CurLine>(StartLine+WrapLines)) WrapLines=CurLine-StartLine+WinHeight;


    // if wrapping was postponed previously, increase the scope of wrapping now
    if (WrapPending && WrapFlag==WRAP_PARA) WrapFlag=WRAP_WIN;
    if (WrapPending && WrapFlag==WRAP_MIN)  WrapFlag=WRAP_PARA;
    if (WrapFlag==WRAP_PAGE) WrapFlag=WRAP_WIN;

    // ******** check if minimum wrapping is permissible
    if (WrapFlag==WRAP_MIN) {                       // entire screen word wrapping may not be required
       // check if the line contains page break, or section break character
       if (tabw(CurLine) && (tabw(CurLine)->type&(INFO_TAB|INFO_SECT|INFO_PAGE|INFO_ROW|INFO_COL|INFO_FRAME))) goto FULL_WRAP;
       if (tabw(CurLine) && (tabw(CurLine)->CharFlags!=NULL)) goto FULL_WRAP;
       if (LineFlags(CurLine)&LFLAG_HDRS_FTRS) goto FULL_WRAP;
       if ((fid(CurLine) && ParaFrame[fid(CurLine)].TextAngle>0)) {
           PaintFlag=PAINT_WIN;
           goto FULL_WRAP;
       }
       if (cid(CurLine) && cell[cid(CurLine)].TextAngle>0) {
           PaintFlag=PAINT_WIN;
           goto FULL_WRAP;
       }

       if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR && LineLen(CurLine)>1 && LineHt(CurLine)==0) goto FULL_WRAP;  // some text entered into header footer

       if (WrapWidthChars>0 && LineLen(CurLine)>WrapWidthChars) goto FULL_WRAP;

       if (LineHt(CurLine)==0 && LineLen(CurLine)>0) goto FULL_WRAP;   // may be line ht becoming non-zero now

       MaxLineWidth=TerWrapWidth(w,CurLine,-1)-PfmtId[pfmt(CurLine)].LeftIndent-PfmtId[pfmt(CurLine)].RightIndent;
       if (LineFlags(CurLine)&LFLAG_PARA_FIRST) MaxLineWidth-=PfmtId[pfmt(CurLine)].FirstIndent;
       if (CurFrame>=0 && CurFrame<TotalFrames) MaxLineWidth+=frame[CurFrame].x;             // add the frame displacement
       else                                     MaxLineWidth+=frame[0].x;                    // add the frame displacement
       if (CurLine==0 || LineFlags(CurLine-1)&(LFLAG_PARA|LFLAG_BREAK)) MaxLineWidth+=PfmtId[pfmt(CurLine)].FirstIndent;

       // check if line is running out of pixel width
       if (LineLen(CurLine)>0) {
          if (tabw(CurLine) && tabw(CurLine)->type&INFO_JUST && TerArg.PrintView && MessagePending(w)) SaveJust=TRUE;
          if (SaveJust) tabw(CurLine)->type=ResetUintFlag(tabw(CurLine)->type,INFO_JUST); // turn off the justification flag temporariy
          PixLineWidth=ColToUnits(w,LineLen(CurLine),CurLine,LEFT);
          if (SaveJust) tabw(CurLine)->type|=INFO_JUST;   // restore the justification flag
          if (PixLineWidth>=MaxLineWidth) {
            if ((TerOpFlags&TOFLAG_INSERT_CHAR) && RepageBeginLine>CurLine) RepageBeginLine=CurLine;
            goto FULL_WRAP;
          }
       }

       // No short wrapping when Page Break/column break encountered
       ptr=pText(CurLine);
       len=LineLen(CurLine);
       if (len>0 && lstrchr(BreakChars,ptr[len-1])!=NULL) BreakFound=TRUE;

       if (BreakFound) goto FULL_WRAP;

       if (HilightType==HILIGHT_OFF && (PaintFlag==PAINT_WIN || PaintFlag==PAINT_PARTIAL_WIN)) PaintFlag=PAINT_LINE;         // re-PaintFlag just this line
       if (!RepagePending) RepageBeginLine=TotalLines;     // don't if repagination was suspended because of pending messages
       return TRUE;
    }

    FULL_WRAP:

    if (WrapFlag==WRAP_MIN) WrapFlag=WRAP_PARA;             // promote to para

    SaveModified=TerArg.modified;
    SaveTotalLines=TotalLines;
    SyncEditLine=(EditLine==CurLine && EditCol==CurCol);     // TRUE to syn edit line with current line

    // save the highlight position
    SaveWrapHilight(w,&HilightBeg,&HilightEnd,&BegHilightAtLineEnd,&EndHilightAtLineEnd,&SelectAll);


    if (WrapFlag==WRAP_PARA) {  // wrap from the current line to the end of current paragraph
       StartLine=CurLine;
       if (CurLine>0 && LineLen(CurLine-1)>0) {
          ptr=pText(CurLine-1);
          if (ptr[LineLen(CurLine-1)-1]!=ParaChar) StartLine--;

       }
    }

    // find absolute location of the cursor
    CursorLoc=0;
    for (l=StartLine;l<CurLine;l++) CursorLoc+=LineLen(l);

    ptr=pText(CurLine);
    if (LineLen(CurLine)>0) LastChar=ptr[LineLen(CurLine)-1];


    if (LineLen(CurLine)>0 && CurCol>=LineLen(CurLine) && LastChar==ParaChar) {
        CursorLoc+=(LineLen(CurLine)+1);
    }
    else CursorLoc+=(CurCol+1);

    if (TotalLines>0) LastPfmt=pfmt(TotalLines-1);
    else              LastPfmt=DEFAULT_PFMT;

    // do the wrapping
    LinesWrapped=0;
    while (LinesWrapped<=WrapLines || FileWrap) {              // wrap requested number of lines
        if ((StartLine+LinesWrapped)>=TotalLines) break;

        PrevLastWrappedLine=LastWrappedLine;

        WrapMakeBuffer(w,StartLine+LinesWrapped,WrapLines-LinesWrapped+1); // build buffer for pasing
        WrapParseBuffer(w,StartLine+LinesWrapped);   // parse the buffer

        if (LastBufferedLine>(LastWrappedLine+25) || BufferLength==0 || (LastBufferedLine+1)>=TotalLines) {
           DisplacePointers(w,LastBufferedLine+1,LastWrappedLine-LastBufferedLine); // scroll up the remaining pointers
           LastBufferedLine=LastWrappedLine;
        }
        else if (LastBufferedLine>LastWrappedLine){
           for (l=LastWrappedLine+1;l<=LastBufferedLine;l++) {
              FreeLine(w,l);
              InitLine(w,l);
              pfmt(l)=0;
           }
        }

        if (LastWrappedLine<0) LastWrappedLine=0;

        // go back a line when an incomplete paragraph is encoutered
        if (!(LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK|LFLAG_LINE)) && LastWrappedLine>StartLine
             && (LastWrappedLine+1)<=TotalLines && LastWrappedLine<(StartLine+WrapLines-1)
             && LastWrappedLine>(PrevLastWrappedLine+1)) LastWrappedLine--;

        LinesWrapped=LastWrappedLine-StartLine+1;
        if (WrapFlag==WRAP_PARA && LastWrappedLine>=CurLine) break;
    }

    DisplacePointers(w,LastBufferedLine+1,LastWrappedLine-LastBufferedLine); // scroll up the remaining pointers



    //*** find the cursor position
    CurLine=StartLine;
    while(CurLine<TotalLines && CursorLoc>LineLen(CurLine)) {
       CursorLoc-=LineLen(CurLine);
       CurLine++;
    }
    CurCol=(int)(CursorLoc-1);

    if (CurLine>=TotalLines) { // don't let the cursor go past the last line
        CurLine=TotalLines-1;
        CurCol+=LineLen(CurLine);
    }

    ptr=pText(CurLine);
    if (LineLen(CurLine)>0) LastChar=ptr[LineLen(CurLine)-1];

    if (LineLen(CurLine)>0 && CurCol>=LineLen(CurLine) && LastChar==ParaChar) {
       if ((CurLine+1)>=TotalLines) {
         CurCol=LineLen(CurLine)-1;
         if (CurCol<0) CurCol=0;
       }
       else { 
         CurCol-=LineLen(CurLine);   // here paragraph character is forcing a new line
         CurLine++;
       }
    }

    if (CurLine>=TotalLines) {
       TotalLines++;
       CurLine=TotalLines-1;
       InitLine(w,CurLine);
    }

    // last line should always be a regular text line
    if (CurLine==(TotalLines-1)) {
       int ParaId=pfmt(CurLine);
       if (PfmtId[ParaId].flags&PAGE_HDR_FTR || fid(CurLine)>0 || cid(CurLine)>0) {
          TotalLines++;
          CurLine=TotalLines-1;
          InitLine(w,CurLine);
          fid(CurLine)=0;
          cid(CurLine)=0;
       }
    }

    //***** initialize a blank line ****
    if (LineLen(CurLine)==0) {
       LineAlloc(w,CurLine,0,1);   // insert a paramarker character in the new line
       ptr=pText(CurLine);
       ptr[0]=ParaChar;
       fmt=OpenCfmt(w,CurLine);
       fmt[0]=DEFAULT_CFMT;
       CloseCfmt(w,CurLine);
       pfmt(CurLine)=LastPfmt;
       if (CurLine==(TotalLines-1) && (PfmtId[LastPfmt].flags&PAGE_HDR_FTR || fid(CurLine)>0)) pfmt(CurLine)=0;
    }

    if (CurCol>LineLen(CurLine)) CurCol=LineLen(CurLine);
    if (CurCol<0) CurCol=0;

    CurRow = CurLine-BeginLine;
    if (CurRow>=WinHeight && !TerArg.PageMode) {CurRow=WinHeight-1;BeginLine=CurLine-CurRow;}

    TerArg.modified=SaveModified;

    // restore the hilight position
    RestoreWrapHilight(w,HilightBeg,HilightEnd,BegHilightAtLineEnd,EndHilightAtLineEnd,SelectAll);
    if (HilightType!=HILIGHT_OFF && EndHilightAtLineEnd && CurLine==(HilightEndRow+1) && CurCol==0 
       && ((LineFlags(CurLine)&LFLAG_HDRS_FTRS) || LineInfo(w,CurLine,INFO_ROW))) {  // needed to keep the cursor from moving to the footer area when the last header line is completely selected and then the cursor is clicked in the header
       PrevTextPos(w,&CurLine,&CurCol);
    }

        pass++;
    // check if another word-wrap pass needed in simple word-wrap mode
    if (!TerArg.PageMode && CurLine>0 && IsListLine(w,CurLine) && IsListLine(w,CurLine-1) && pass==1) {
       LPBYTE CurListText=NULL,PrevListText=NULL;
       if (True(tabw(CurLine))) CurListText=tabw(CurLine)->ListText;
       if (True(tabw(CurLine-1))) PrevListText=tabw(CurLine-1)->ListText;
       if (CurListText==null || PrevListText==null || lstrcmpi(CurListText,PrevListText)==0) goto FULL_WRAP;   // this can happen when <Enter> is pressed in a list to generate a new line
    } 

    if (SyncEditLine) {                // sync edit line with current line
       EditLine=CurLine;
       EditCol=CurCol;
    } 

    WrapPending=FALSE;
    if (RepageBeginLine<StartLine) RepageBeginLine=StartLine;

    if (TotalLines!=SaveTotalLines && PaintFlag!=PAINT_WIN && PaintFlag!=PAINT_WIN_RESET) PaintFlag=PAINT_WIN;
    if (LineInfo(w,CurLine,INFO_FRAME) && PaintFlag!=PAINT_WIN && PaintFlag!=PAINT_WIN_RESET) PaintFlag=PAINT_WIN;

    if (TotalLines!=SaveTotalLines && (TerOpFlags&TOFLAG_INSERT_CHAR) && RepageBeginLine>(CurLine-1)) RepageBeginLine=(CurLine>0?CurLine-1:CurLine);

    return TRUE;
}

/******************************************************************************
    SaveWrapHilight:
    Save the hilight position for before wrapping.
*******************************************************************************/
SaveWrapHilight(PTERWND w,LPLONG pHilightBeg,LPLONG pHilightEnd,LPINT pBegHilightAtLineEnd, LPINT pEndHilightAtLineEnd, LPINT pSelectAll)
{
    long HilightBeg=0,HilightEnd=0;
    BOOL BegHilightAtLineEnd=FALSE,EndHilightAtLineEnd=FALSE,SelectAll=FALSE;

    if (HilightBegRow>=TotalLines || HilightEndRow>=TotalLines) HilightType=HILIGHT_OFF;

    if (HilightType==HILIGHT_CHAR) {  // find absolute co-ordinates for the block
       SelectAll=(HilightBegRow==0 && HilightBegCol==0 && HilightEndRow==(TotalLines-1) && HilightEndCol==LineLen(HilightEndRow));
       HilightBeg=RowColToAbs(w,HilightBegRow,HilightBegCol);
       HilightEnd=RowColToAbs(w,HilightEndRow,HilightEndCol);
       BegHilightAtLineEnd=(TerArg.WordWrap && HilightBeg>HilightEnd && HilightBegCol==LineLen(HilightBegRow) && LineEndsInBreak(w,HilightBegRow));
       EndHilightAtLineEnd=(TerArg.WordWrap && HilightEnd>HilightBeg && HilightEndCol==LineLen(HilightEndRow) && LineEndsInBreak(w,HilightEndRow));
    }

    (*pHilightBeg)=HilightBeg;
    (*pHilightEnd)=HilightEnd;
    (*pBegHilightAtLineEnd)=BegHilightAtLineEnd;
    (*pEndHilightAtLineEnd)=EndHilightAtLineEnd;
    (*pSelectAll)=SelectAll;

    return TRUE;
}

/******************************************************************************
    RestoreWrapHilight:
    Restore the hilight position for before wrapping.
*******************************************************************************/
RestoreWrapHilight(PTERWND w,long HilightBeg,long HilightEnd,BOOL BegHilightAtLineEnd, BOOL EndHilightAtLineEnd, BOOL SelectAll)
{

    if (HilightType==HILIGHT_CHAR) {  // calclulate co-ordinates for the block
      AbsToRowCol(w,HilightBeg,(long far *)&HilightBegRow,(int far *)&HilightBegCol);
      if (BegHilightAtLineEnd && HilightBegRow>0 && HilightBegCol==0) {
         HilightBegRow--;
         HilightBegCol=LineLen(HilightBegRow);
      }

      AbsToRowCol(w,HilightEnd,(long far *)&HilightEndRow,(int far *)&HilightEndCol);

      if (EndHilightAtLineEnd) {
         long line=(HilightEndCol==0)?(HilightEndRow-1):HilightEndRow;
         int cl=(line<TotalLines && line>=0)?cid(line):0;
         if (HilightEndRow==(TotalLines-1) && HilightEndCol==(LineLen(HilightEndRow)-1) && cl==0) HilightEndCol++;
         else if (HilightEndRow>0 && HilightEndCol==0) {
            HilightEndRow--;
            HilightEndCol=LineLen(HilightEndRow);
         }
      }
      if (SelectAll) {
         HilightBegRow=0;
         HilightBegCol=0;
         HilightEndRow=TotalLines-1;
         HilightEndCol=LineLen(HilightEndRow);
      }
    }

    return TRUE;
}

/******************************************************************************
    DisplacePointers:
    Scroll down the line handle and line length arrays
    The routine returns FALSE if ran out of the maximum allowed lines.
*******************************************************************************/
BOOL DisplacePointers(PTERWND w,long StartLine,long count) // move down PTR arrays
{
    long l;
    BOOL result;

    if (count>0) {                  // allocate more lines
       TerOpFlags|=TOFLAG_WRAPPING;
       result=CheckLineLimit(w,TotalLines+count);
       ResetLongFlag(TerOpFlags,TOFLAG_WRAPPING);

       if (!result) return FALSE;

       if (TerArg.LineLimit>0 && (TotalLines+count)>TerArg.LineLimit)
            PrintError(w,MSG_MAX_LINES_EXCEEDED,"WrapParseBuffer");
    }

    if (count==0) return FALSE;
    LastBufferedLine=LastBufferedLine+count;

    if (count<0) { // free the lines to be scrolled over
       l=-count;
       while (l>0) {
          // preserve some line flags
          if (StartLine<TotalLines && LineFlags(StartLine-l)&(LFLAG_SOFT_COL)) LineFlags(StartLine)|=LFLAG_SOFT_COL;
          if (StartLine<TotalLines && LineFlags(StartLine-l)&(LFLAG_FRAME_TOP)) LineFlags(StartLine)|=LFLAG_FRAME_TOP;

          FreeLine(w,StartLine-l);
          l--;
       }
    }

    HugeMove(&LinePtr[StartLine],&LinePtr[StartLine+count],(TotalLines-StartLine)*(sizeof (struct StrLinePtr far *))); // make space for new lines
    TotalLines=TotalLines+count;

    if (count>0) { // intialized the newly generated lines
        for (l=0;l<count;l++) InitLine(w,StartLine+l);
        if (count==1 && (StartLine+1)<TotalLines) LineY(StartLine)=LineY(StartLine+1);
    }

    if (TotalLines<=0) {
     TotalLines=1;
     InitLine(w,0);
    }

    // adjust section boundaries
    if (count>0) AdjustSections(w,StartLine-1,count);
    else         AdjustSections(w,StartLine+count,count);

    return TRUE;
}

/******************************************************************************
    WrapMakeBuffer:
    consolidate all lines for a paragraph into one buffer for parsing.
******************************************************************************/
WrapMakeBuffer(PTERWND w,long StartLine,long WrapLines)
{
    LPBYTE ptr;
    LPWORD fmt;
    int len;
    long l;

    WrapSect=0;                     // section being wrapped
    TagsWrapped=LeadWrapped=CharWidthWrapped=FALSE;  // TRUE when the tags and lead bytes are found
    WrapTextFlow=FLOW_DEF;          // text flow direction
    
    WrapSpellChecked=DoAutoSpellCheck(w);   // true for default when auto spelling enabled

    MaxBufferLength=WrapBufferSize-LineWidth;
    BufferLength=0;
    LastBufferedLine=StartLine;
    CurWrapPfmt=CurWrapCell=CurWrapParaFID=WrapParaFont=0;

    if (LineLen(LastBufferedLine)>=MaxBufferLength) AllocWrapBuf(w,LineLen(LastBufferedLine)+LineWidth);

    while (BufferLength<=MaxBufferLength && LastBufferedLine<TotalLines) {

        if (LineLen(LastBufferedLine)>0) {
            
            ptr=pText(LastBufferedLine);


            // End para before special characters
            if (BufferLength>0 && IsHdrFtrChar(ptr[0])) break;

            fmt=OpenCfmt(w,LastBufferedLine);

            len=LineLen(LastBufferedLine);

            // hide paragraph markers if needed
            if (ptr[len-1]==ParaChar) {
               int font=fmt[len-1];
               BOOL HiddenPara=FALSE;
               if (TerFont[font].FieldId==FIELD_DATA && TerFont[font].CharWidth[ParaChar]==0) HiddenPara=TRUE;
               if (!EditFootnoteText && !EditEndnoteText && True(TerFont[font].style&FNOTETEXT) && TerFont[font].CharWidth[ParaChar]==0) HiddenPara=TRUE;
               if (HiddenPara) {
                  ptr[len-1]=HPARA_CHAR;
                  SetTag(w,LastBufferedLine,len-1,TERTAG_HPARA2,"HPARA2",NULL,pfmt(LastBufferedLine));   // save the next para id;

                  ptr=pText(LastBufferedLine);
                  fmt=OpenCfmt(w,LastBufferedLine);
                  len=LineLen(LastBufferedLine);
               } 
            }  


            FarMove(ptr,&wrap[BufferLength],len);
            FarMove(fmt,&WrapCfmt[BufferLength],len*sizeof(WORD));
            if (!(LineFlags2(LastBufferedLine)&LFLAG2_SPELL_CHECKED)) WrapSpellChecked=FALSE;     // need to re-spell check this para

            if (pCtid(LastBufferedLine) || TagsWrapped) {      // copy the tags
               if (!TagsWrapped && BufferLength>0) FarMemSet(WrapCtid,0,BufferLength*sizeof(WORD));  // initialize the previous ids
               TagsWrapped=TRUE;
               if (pCtid(LastBufferedLine)) FarMove(pCtid(LastBufferedLine),&WrapCtid[BufferLength],len*sizeof(WORD));
               else                         FarMemSet(&WrapCtid[BufferLength],0,len*sizeof(WORD));
            }
            if (pLead(LastBufferedLine) || LeadWrapped) {      // copy the tags
               if (!LeadWrapped && BufferLength>0) FarMemSet(WrapLead,0,BufferLength*sizeof(BYTE));  // initialize the previous ids
               LeadWrapped=TRUE;
               if (pLead(LastBufferedLine)) FarMove(pLead(LastBufferedLine),&WrapLead[BufferLength],len*sizeof(BYTE));
               else                         FarMemSet(&WrapLead[BufferLength],0,len*sizeof(BYTE));
            }


            BufferLength+=len;

            if (len>1 || ptr[0]!=SECT_CHAR) CurWrapPfmt=pfmt(LastBufferedLine);     // remember para id

            CurWrapCell=cid(LastBufferedLine);      // remember cell id
            CurWrapParaFID=fid(LastBufferedLine);   // remember para frame id
        }

        LastBufferedLine++;

        // set the paragraph font
        if (BufferLength>0 && (wrap[BufferLength-1]==ParaChar || wrap[BufferLength-1]==CellChar || wrap[BufferLength-1]==PAGE_CHAR)) WrapParaFont=WrapCfmt[BufferLength-1];
        if (BufferLength>1 && wrap[BufferLength-1]==SECT_CHAR && wrap[BufferLength-2]==ParaChar) WrapParaFont=WrapCfmt[BufferLength-2];   // incase para char is follwed by sect char

        // check for para/cell breaks
        if (BufferLength>0) {
          if (wrap[BufferLength-1]==ParaChar 
           || wrap[BufferLength-1]==CellChar) {
              
            // Look for the first character of the next line
            if (LastBufferedLine<TotalLines && LineLen(LastBufferedLine)>0) {
               if ((WrapLines-1)<=0) break;   // ran out lines to do forward lookup
               ptr=pText(LastBufferedLine);
               if (ptr[0]!=SECT_CHAR) break;
               if (CurWrapParaFID>0 || CurWrapCell>0) break;  // don't let the SECT char get inside a frame
            }
            else break;
          }
        }

        // check for other break characters
        if (BufferLength>0) {                      // other breaks
          BYTE LastChar=wrap[BufferLength-1];
          if (LastChar==ROW_CHAR || LastChar==PAGE_CHAR || LastChar==COL_CHAR || IsHdrFtrChar(LastChar)) break;
        }

        if (BufferLength>0 && wrap[BufferLength-1]==SECT_CHAR) {
           if (tabw(LastBufferedLine-1) && tabw(LastBufferedLine-1)->type&INFO_SECT) {
              WrapSect=tabw(LastBufferedLine-1)->section;
           }
           break;
        }

        WrapLines--;
        if (WrapLines<=0) {       // look forward to find the line with a break character
           long l;
           for (l=LastBufferedLine;l<TotalLines;l++) {
              LPBYTE ptr=pText(l);
              int i,len=LineLen(l);
              if (LineFlags(l)&LFLAG_SECT) break;   // can't go beyond the section boundary

              for (i=0;i<len;i++) {
                 BYTE CurChar=ptr[i];
                 if (  CurChar==ParaChar || CurChar==CellChar || CurChar==ROW_CHAR 
                       || CurChar==PAGE_CHAR || CurChar==COL_CHAR
                    || IsHdrFtrChar(CurChar)) {
                    CurWrapPfmt=pfmt(l);
                    WrapParaFont=GetCurCfmt(w,l,i);
                    break;
                 }
              }
              if (i<len) break;
           } 

           break;
        }
    }

    //************* add the last paragraph marker character if necessary
    if (LastBufferedLine==TotalLines) {

        if (BufferLength==0 || wrap[BufferLength-1]!=ParaChar) {
           if (BufferLength>0 && wrap[BufferLength-1]==LINE_CHAR) wrap[BufferLength-1]=ParaChar;     // convert line char to para char
           else {
              int ParaFont;
              wrap[BufferLength]=ParaChar;
              if (TagsWrapped) WrapCtid[BufferLength]=0;
              if (LeadWrapped) WrapLead[BufferLength]=0;

              ParaFont=(BufferLength>0)?WrapCfmt[BufferLength-1]:0;
              if (BufferLength==0 || TerFont[ParaFont].style&PICT || TerFont[ParaFont].FieldId>0)
                    WrapCfmt[BufferLength]=DEFAULT_CFMT;
              else  WrapCfmt[BufferLength]=ResetUniFont(w,WrapCfmt[BufferLength-1]);
              BufferLength++;
           }
        }
        if (TerFlags4&TFLAG4_MOD_END_MARK_FONT && BufferLength>1 && wrap[BufferLength-1]==ParaChar) {  // modify end marker font
           int NewFont=WrapCfmt[BufferLength-2];  // use the previous font
           if (TerFont[NewFont].style&PICT || TerFont[NewFont].FieldId>0) NewFont=WrapCfmt[BufferLength-1];
           WrapCfmt[BufferLength-1]=NewFont;
        } 
    }


    LastBufferedLine--;                   // last PTR line consumed to make this buffer

    GetWrapCharWidth(w);  // get character width for special characters

    // set the same pfmt id for the entire paragraph
    for (l=StartLine;l<=LastBufferedLine;l++) pfmt(l)=CurWrapPfmt;

    return TRUE;
}

#pragma optimize("g",off)  // turn-off global optimization
/******************************************************************************
    WrapParseBuffer:
    parse the buffer to product word wrapped lines
******************************************************************************/
WrapParseBuffer(PTERWND w,long StartLine)
{
    int  LineBegin,LineEnd,WordEnd,WrapWidth;
    int  CurParaId,LeftIndent,RightIndent,len;
    int  TabPos,TabType,CurTabWidth,JustSpaceCount,AdjLen;
    int  PrevTabType=TAB_LEFT,PrevTabPos=0,PrevTabWidth,PrevTextLen=0,
         PrevScrTextLen=0;
    int  j,BaseHeight,DescentHeight,SaveCurLen,JustSpaceIgnore,JustCX;
    register int CurLen;
    LPWORD fmt;
    WORD CurChar,EndChar,PrevChar,DispChar;
    struct StrTab far *tab;
    struct StrTabw CurTabw;   // current tab width array
    WORD BreakChar;
    BYTE decimal='.';
    WORD CurFmt,WrapFmt=0;
    BOOL DecimalFound,BoxOn,BoxFmtUsed,NoWrap,WrapChars=0;
    int  FrameX,FrameWidth,PrtFrameX,PrtFrameWidth,FrameHt;
    int  WordBeginX,WordBeginIdx,DynFieldPos,ControlPos;
    DWORD lflags,lflags2;
    BOOL UsePrtUnits,SectBreak,SectBreakFollows;
    long PrevTextLine;
    int  FrmSpcBef,FieldId,TextAngle;
    int  LeftIndentTwips,FirstIndentTwips,EndSpaceLen;
    BOOL IsFramePict,TabPastRuler;
    BOOL WrapSpaces=((TerFlags3&TFLAG3_WRAP_SPACES)?TRUE:FALSE);     // wrap additinal spaces to the next line
    BOOL CurHidden,PrevHidden,IsToc,IsListnum,IsAutoNumLgl,FarTabStop,IsHyphen,IsAsianChar;
    BYTE ListText[50];
    int  ListTextWidth,TabId,ListFontId,CharWidth,row;
    long ListNbr;
    BOOL HasAssumedTab,CharWidthUsed,IsRtl,NewLine,CurWrapSpaces;
    UINT CurStyle;
    BOOL XlateHiddenPara=(TerFlags4&TFLAG4_HIDE_HIDDEN_PARA_MARK)?TRUE:FALSE;
    int  WrapWidthAdj=0,NextParaId=-1;

    if (TerFlags&TFLAG_SWAP_DECIMAL) decimal=',';

    // get thet text angle for the frame and rtl text flow
    TextAngle=CurWrapParaFID>0?ParaFrame[CurWrapParaFID].TextAngle:0;
    if (TextAngle==0 && CurWrapCell>0 && cell[CurWrapCell].TextAngle!=0) TextAngle=cell[CurWrapCell].TextAngle;

    row=cell[CurWrapCell].row;
    
    // Set the text flow direction
    IsRtl=IsParaRtl(w,PfmtId[CurWrapPfmt].flow,TableRow[row].flow,TerSect[WrapSect].flow,DocTextFlow);
    if (!IsRtl && WrapTextFlow==FLOW_RTL /*&& PfmtId[CurWrapPfmt].flow==FLOW_DEF*/) {
       if (True(TerOpFlags&TOFLAG_INSERT_CHAR) || PfmtId[CurWrapPfmt].flow==FLOW_DEF)  // 20050916
           CurWrapPfmt=SetParaTextFlow(w,CurWrapPfmt,FLOW_RTL);
       IsRtl=TRUE;
       PaintFlag=PAINT_WIN;
    }

    // set the units to use
    if (TerArg.PrintView && !TerArg.FittedView) UsePrtUnits=TRUE;
    else                                        UsePrtUnits=FALSE;

    if (!TerArg.PrintView && WrapWidthChars>0) WrapChars=WrapWidthChars;

    LineEnd=0;
    LastWrappedLine=StartLine-1;
    //WrapFmt=WrapCfmt[0];
    //if (TerFont[WrapFmt].ParaStyId==PfmtId[CurWrapPfmt].StyId) CurFmt=WrapFmt;
    //else  CurFmt=WrapCfmt[0]=ApplyParaStyleOnFont(w,WrapFmt,PfmtId[CurWrapPfmt].StyId);
    //if (TerFont[CurFmt].style&SCAPS) {        // do font modification for scaps
    //   if (IsLcChar(wrap[0]) && !(TerFont[CurFmt].flags&FFLAG_SCAPS)) CurFmt=SetScapFont(w,CurFmt,TRUE);
    //   if (!IsLcChar(wrap[0]) && (TerFont[CurFmt].flags&FFLAG_SCAPS)) CurFmt=SetScapFont(w,CurFmt,FALSE);
    //} 
    //if (TerFont[CurFmt].TextAngle!=TextAngle) CurFmt=SetFontTextAngle(w,CurFmt,TextAngle);

    if (UsePrtUnits) WrapWidth=TerWrapWidth2(w,LastBufferedLine,-1,FALSE);  // wrap in printer units
    else             WrapWidth=TerWrapWidth(w,LastBufferedLine,-1);         // wrap in screen units

    // Access the first paragraph information
    CurParaId=CurWrapPfmt;
    if (CurParaId>=TotalPfmts) CurParaId=TotalPfmts-1;
    if (CurParaId<0) CurParaId=0;
    NoWrap=PfmtId[CurParaId].pflags&PFLAG_NO_WRAP;
    if (TerFlags&TFLAG_NO_WRAP && !(TerArg.PageMode)) NoWrap=TRUE;


    EXTRACT_LINE:
    if (LineEnd>=BufferLength) return TRUE; // end of buffer

    if (NextParaId>=0) CurParaId=NextParaId;     // HPARA converted to regular para

    // find the previous text line to test for new para condition
    PrevTextLine=LastWrappedLine;
    if (PrevTextLine<0) PrevTextLine=0;

    // check if a list number line
    ListTextWidth=0;
    if (IsListLine(w,LastWrappedLine+1)) GetListText(w,CurParaId,LastWrappedLine+1,ListText,&ListTextWidth,&ListNbr,&ListFontId,WrapParaFont,-1,UsePrtUnits);


    RightIndent=PfmtId[CurParaId].RightIndent;
    if (RightIndent<0 && (!TerArg.PageMode || TerArg.FittedView)) RightIndent=0;

    LeftIndentTwips=PfmtId[CurParaId].LeftIndentTwips;
    FirstIndentTwips=PfmtId[CurParaId].FirstIndentTwips;
    
    if (ListTextWidth>0) FirstIndentTwips+=(UsePrtUnits?PrtToTwipsX(ListTextWidth):ScrToTwipsX(ListTextWidth));         // reserve space taken by the list number
    else if (PfmtId[CurParaId].flags&BULLET && FirstIndentTwips<0) {
       int BltId=PfmtId[CurParaId].BltId;
       if (!(TerBlt[BltId].flags&BLTFLAG_HIDDEN)) FirstIndentTwips=0;  // this space taken for showing the bullet
    }
    
    if ((LastWrappedLine+1)==0) LeftIndentTwips+=FirstIndentTwips;
    else {                   // check if new paragraph
       if (LineFlags(PrevTextLine)&(LFLAG_PARA|LFLAG_BREAK)) LeftIndentTwips+=FirstIndentTwips;
    }
    LeftIndent=TwipsToScrX(LeftIndentTwips);


    if (UsePrtUnits) {  // convert to printer resolution
       LeftIndent=TwipsToPrtX(LeftIndentTwips);
       RightIndent=ScrToPrtX(RightIndent);
    }
    tab=&(TerTab[PfmtId[CurParaId].TabId]); // current tab table

    FarMemSet(&CurTabw,0,sizeof(CurTabw));
    CurTabw.type=INFO_TAB;                  // use this for tab information
    CurTabw.FrameCharPos=-1;                // frame position

    // check if any tab stop goes beyond the line
    FarTabStop=FALSE;
    TabId=PfmtId[CurParaId].TabId;
    if (TabId && TerArg.PageMode && (!(TerArg.FittedView) || CurWrapCell>0) && CurWrapParaFID==0) {
       int i;
       for (i=0;i<tab->count;i++) {
          if (TwipsToPrtX(tab->pos[i])>=(WrapWidth-RightIndent+TwipsToPrtX(RULER_TOLERANCE))) {
             FarTabStop=TRUE;    // tab stop beyond the right wrap point
             break;
          } 
       }    
    } 

    //EXTRACT_LINE:
    LineBegin=LineEnd;                      // next line begins here
    WordEnd=0;


    JustSpaceCount=0;                       // number of spaces in the line
    JustSpaceIgnore=0;                      // position of the last tab in the line
    SaveCurLen=0;
    BreakChar=0;                            // reset breaks
    lflags=0;                               // line flags
    lflags2=0;                              // line flags2
    DecimalFound=FALSE;                     // word beginning pixel position
    DynFieldPos=-1;                       // position where page number found
    ControlPos=-1;                          // position where embedded control found
    WordBeginIdx=LineBegin;                 // index of the first word
    BoxFmtUsed=FALSE;                       // true if a character in line has box attribute
    BoxOn=FALSE;                            // true when the box attribute is on
    CurChar=PrevChar=0;                     // previous character not yet set
    SectBreak=FALSE;                        // TRUE if sect break followed para/cell/line break
    TabPastRuler=FALSE;                     // tab past ruler
    JustCX=0;
    EndSpaceLen=0;                          // pixel length of the space at the end of the line
    CurHidden=PrevHidden=FALSE;             // current and previous character hidden
    IsToc=IsListnum=FALSE;                  // field type encountered
    IsAutoNumLgl=FALSE;                     // autonumlgl type field encountered
    HasAssumedTab=FALSE;                    // TRUE when a cell line has a tab stop 
    CharWidthUsed=FALSE;

    if ((LastWrappedLine+1)<TotalLines) JustCX=JustAdjX(LastWrappedLine+1);

    FrmSpcBef=GetFrmSpcBef(w,LastWrappedLine+1,FALSE);

    CurLen=WordBeginX=LeftIndent;            // apply left indentation
    if ((LastWrappedLine+1)<TotalLines) {
       if (IsRtl) LineFlags2(LastWrappedLine+1)|=LFLAG2_RTL;     // this flag is used by the GetFrameSpace function
       else       ResetLongFlag(LineFlags2(LastWrappedLine+1),LFLAG2_RTL);
    }
    
    if (CurWrapParaFID==0) GetFrameSpace(w,LastWrappedLine+1,NULL,&PrtFrameX,&PrtFrameWidth,&FrameHt);
    else                   PrtFrameX=PrtFrameWidth=FrameHt=0;


    if (UsePrtUnits) {
       FrameX=PrtFrameX;
       FrameWidth=PrtFrameWidth;
       CurTabw.FrameX=FrameX;                // frame space position
       CurTabw.FrameWidth=FrameWidth;        // frame space width
       CurTabw.FrameScrWidth=PrtToScrX(FrameX+FrameWidth);  // width of the frame space including the space after the last word
    }
    else {                  // convert to screen units
       FrameX=PrtToScrX(PrtFrameX);
       FrameWidth=PrtToScrX(PrtFrameWidth);
       CurTabw.FrameX=PrtFrameX;          // frame space position
       CurTabw.FrameWidth=PrtFrameWidth;  // frame space width
       CurTabw.FrameScrWidth=FrameX+FrameWidth;  // width of the frame space including the space after the last word
    }

    // check for an assumed tab character for a cell line
    if (TabId && TerArg.PageMode && CurWrapCell>0 && tab->count==1 && tab->type[0]==TAB_DECIMAL) {
       HasAssumedTab=TRUE;
       GetTabPos(w,CurParaId,tab,CurLen,&TabPos,&TabType,NULL,!UsePrtUnits);   // base it on screen position
       CurTabWidth=TabPos-CurLen;
       CurTabw.width[CurTabw.count]=CurTabWidth;
       CurTabw.count++;

       PrevTabPos=TabPos;
       PrevTabType=TabType;                          // save for right tab calculation
       PrevTextLen=0;                                // measure length of the text after this tab position
       PrevScrTextLen=0;                             // measure length of the text after this tab position
       DecimalFound=FALSE;
       if (TabPos>WrapWidth) TabPastRuler=TRUE;
       if (FarTabStop && TabPos>=(WrapWidth-RightIndent+TwipsToPrtX(RULER_TOLERANCE))) NoWrap=TRUE;
    } 

    // extract a text line now
    NewLine=TRUE;
    while (TRUE) {                          // extract the line
       if (WrapCfmt[LineEnd]!=WrapFmt || NewLine) {
          WrapFmt=WrapCfmt[LineEnd];
          if (TerFont[WrapFmt].ParaStyId==PfmtId[CurParaId].StyId) CurFmt=WrapFmt;
          else  CurFmt=ApplyParaStyleOnFont(w,WrapFmt,PfmtId[CurParaId].StyId);
          if (TerFont[CurFmt].TextAngle!=TextAngle) CurFmt=SetFontTextAngle(w,CurFmt,TextAngle);
          NewLine=FALSE;
       }
       CurStyle=TerFont[CurFmt].style;
       if (CurStyle&SCAPS) {        // do font modification for scaps
          if (IsLcChar(wrap[LineEnd]) && !(TerFont[CurFmt].flags&FFLAG_SCAPS)) CurFmt=SetScapFont(w,CurFmt,TRUE);
          if (!IsLcChar(wrap[LineEnd]) && (TerFont[CurFmt].flags&FFLAG_SCAPS)) CurFmt=SetScapFont(w,CurFmt,FALSE);
          CurStyle=TerFont[CurFmt].style;
       } 
       WrapCfmt[LineEnd]=CurFmt;            // update the format buffer

       // set the track-changes flags
       if (True(TerFont[CurFmt].InsRev)) lflags2|=LFLAG2_INS_REV;
       if (True(TerFont[CurFmt].DelRev)) lflags2|=LFLAG2_DEL_REV;

       // get context sensitive character width
       CharWidth=(CharWidthWrapped && WrapCharWidth[LineEnd]&CWIDTH_USED)?(WrapCharWidth[LineEnd]&CWIDTH_WIDTH):-1;
       if (CharWidth!=-1) {
          CharWidthUsed=TRUE;    // context-sensitive char width used
          if (!(TerFont[CurFmt].flags&FFLAG_HIDDEN_INFO)) {
             if (!EditFootnoteText && TerFont[CurFmt].style&FNOTETEXT) CharWidth=0;
             if (!EditEndnoteText && TerFont[CurFmt].style&ENOTETEXT) CharWidth=0;
          }
       }

       PrevHidden=CurHidden;
       CurHidden=FALSE;
       PrevChar=CurChar;
       CurChar=DispChar=WrapChar(LineEnd);
       if (CurStyle&(SCAPS|CAPS) && IsLcChar(DispChar)) DispChar-=(BYTE)('a'-'A');  // convert to uppercase

       // translate hidden para-mark
       if (CurChar==HPARA_CHAR) {
          BOOL ShowHiddenPara=(XlateHiddenPara && ShowHiddenText && TerFont[CurFmt].style&HIDDEN);
          BOOL ShowFieldPara=(TerFont[CurFmt].FieldId==FIELD_DATA && TerFont[CurFmt].CharWidth[ParaChar]!=0);
          BOOL ShowFootnotePara=(True(TerFont[CurFmt].style&FNOTETEXT) && TerFont[CurFmt].CharWidth[ParaChar]!=0);
          int ParaId,HiddenPfmt=-1;

          if (TagsWrapped && WrapCtid[LineEnd]>0) {   // Get next para id
            int TagId=WrapCtid[LineEnd];
            if (ShowHiddenPara && GetTag(w,TagId,-1,TERTAG_HPARA,NULL,NULL,&ParaId)>0) HiddenPfmt=ParaId;
            if (ShowFieldPara && GetTag(w,TagId,-1,TERTAG_HPARA2,NULL,NULL,&ParaId)>0) HiddenPfmt=ParaId;
            if (ShowFootnotePara && GetTag(w,TagId,-1,TERTAG_HPARA2,NULL,NULL,&ParaId)>0) HiddenPfmt=ParaId;
            if (HiddenPfmt>=0) {   
               CurChar=ParaChar;             // converter into regular paragraph marker
               wrap[LineEnd]=(BYTE)CurChar;  // converter into regular paragraph marker
               NextParaId=CurParaId;     // save for the next paragraph
               CurParaId=HiddenPfmt;     // use the hidden paragraph id for this paragraph
            }
          }
       } 

       if (PrevChar!=' ' && !PrevHidden && CurChar==' ') EndSpaceLen=0;    // reset the end space string length


       if ((LineEnd+1)<BufferLength && WrapChar(LineEnd+1)==SECT_CHAR)
            SectBreakFollows=TRUE;
       else SectBreakFollows=FALSE;


       // record the page number field and embedded control position
       FieldId=TerFont[CurFmt].FieldId;
       if (DynFieldPos<0 && FieldId>0 && IsDynField(w,FieldId)) DynFieldPos=LineEnd;
       if      (FieldId==FIELD_TOC) IsToc=TRUE;
       else if (FieldId==FIELD_LISTNUM) IsListnum=TRUE;
       else if (FieldId==FIELD_AUTONUMLGL) IsAutoNumLgl=TRUE;
       else if (FieldId==FIELD_TC)      lflags2|=LFLAG2_TC;

       if (TerFont[CurFmt].style&PICT
          && (   TerFont[CurFmt].PictType==PICT_CONTROL
              || TerFont[CurFmt].PictType==PICT_FORM
              || TerFont[CurFmt].ObjectType==OBJ_OCX)) ControlPos=LineEnd;
       IsFramePict=(TerFont[CurFmt].style&PICT && TerFont[CurFmt].ParaFID>0);
       if (IsFramePict) lflags|=LFLAG_PICT;  // contains an aligned picture
       if (TerFont[CurFmt].style&(FNOTE|FNOTETEXT|FNOTEREST)) lflags|=LFLAG_FNOTE;
       
       if      (TerFont[CurFmt].style&ENOTETEXT) lflags2|=LFLAG2_ENOTETEXT;
       else if (TerFont[CurFmt].style&FNOTETEXT) lflags|=LFLAG_FNOTETEXT;

       // check for para break character when it occurs in the middle of a paragraph (HPARA conversion can do this)
       if (CurChar==ParaChar && (LineEnd+1)<BufferLength && !SectBreakFollows) {
          LineEnd++;
          break;
       } 

       // check for cell break character
       if (CurChar==CellChar || CurChar==LINE_CHAR) {
          SectBreak=SectBreakFollows;
          if (SectBreak) LineEnd++;  // include the text break character also
          LineEnd++;
          BreakChar=CurChar;
          break;
       }

       // Check for section break
       if (CurChar==SECT_CHAR) {
          LineEnd++;           // include the break BYTE if on the first position
          BreakChar=CurChar;   // acknowledge break character only if it is the only character on the page
          break;
       }

       // Check for other break characters which takes up a line
       if (  CurChar<=LAST_CTRL_CHAR
          && (  CurChar==ROW_CHAR || IsHdrFtrChar(CurChar)
             || CurChar==PAGE_CHAR || CurChar==COL_CHAR)) {
          if (LineBegin==LineEnd) {
             LineEnd++;           // include the break BYTE if on the first position
             BreakChar=CurChar;   // acknowledge break character only if it is the only character on the page
          }
          break;
       }


       // find character width
       if (CurChar==TAB && !HiddenText(w,CurFmt)) {                        // calculate tab width
          // set the previous right tab if any
          if (CurTabw.count>0 && PrevTabType!=TAB_LEFT) {// set the width of previous right/center tab
              PrevTabWidth=PrevTabPos-CurLen;  // width of the previous tab
              if      (PrevTabType==TAB_CENTER)  PrevTabWidth+=(PrevTextLen/2); // center
              else if (PrevTabType==TAB_DECIMAL) PrevTabWidth+=PrevTextLen; // center
              if (PrevTabWidth<0) PrevTabWidth=0;
              CurLen+=PrevTabWidth;
              CurTabw.width[CurTabw.count-1]=PrevTabWidth;
          }

          // get next tab position
          GetTabPos(w,CurParaId,tab,CurLen,&TabPos,&TabType,NULL,!UsePrtUnits);   // base it on screen position
          CurTabWidth=TabPos-CurLen;

          CurTabw.width[CurTabw.count]=CurTabWidth;


          if (CurTabw.count<TE_MAX_TAB_STOPS) CurTabw.count++;
          if (TabType==TAB_LEFT) CurLen=TabPos;    // current length of line

          PrevTabPos=TabPos;
          PrevTabType=TabType;                          // save for right tab calculation
          PrevTextLen=0;                                // measure length of the text after this tab position
          PrevScrTextLen=0;                             // measure length of the text after this tab position
          DecimalFound=FALSE;
          if (TabPos>WrapWidth) TabPastRuler=TRUE;
          if (FarTabStop && TabPos>=(WrapWidth-RightIndent+TwipsToPrtX(RULER_TOLERANCE))) NoWrap=TRUE;
       }
       else {                                            // add the character length
          if (CurChar!=ParaChar && !IsFramePict) {       // para char width in assumed zero within a frame
             int width=CharWidth;
             if (width==-1) width=WideCharWidth(w,CurFmt,!UsePrtUnits,DispChar); // current line length for wrapping
             if (DispChar==HYPH_CHAR) width=0;   // optional hyphen character does not count for word-wrapping
             CurLen=CurLen+width; // current line length for wrapping
             if (CurChar==' ') EndSpaceLen+=width;
             if (width==0) CurHidden=TRUE;
          }

          // addition space when the character box begins or ends
          if (  (!BoxOn && TerFont[CurFmt].style&CHAR_BOX)
             || (BoxOn && !(TerFont[CurFmt].style&CHAR_BOX)) ) {
             BoxOn=!BoxOn;
             if (BoxOn) BoxFmtUsed=TRUE;
             if (UsePrtUnits) CurLen+=ExtraSpacePrtX;
             else             CurLen+=ExtraSpaceScrX;
          }

          // displace for intervening frames
          if (  (WordBeginX+JustCX)<(FrameX+FrameWidth)
             && (CurLen+JustCX)>=FrameX && FrameWidth>0
             && CurWrapParaFID==0) {
             int WordLen=CurLen-WordBeginX;
             int adj=0;                                   // used by left indentation

             CurTabw.FrameCharPos=WordBeginIdx-LineBegin;      // character position before which the frame is inserted
             if ((WordBeginX+JustCX)>FrameX) adj=WordBeginX+JustCX-FrameX;
             if (UsePrtUnits) {                           // page mode
                CurTabw.FrameX=FrameX+adj;                // frame space position
                CurTabw.FrameWidth=FrameWidth-adj;        // frame space width
                CurTabw.FrameScrWidth=PrtToScrX(FrameX+FrameWidth-WordBeginX);  // width of the frame space including the space after the last word
                CurTabw.FrameSpaceWidth=FrameX+FrameWidth-WordBeginX-adj;  // width of the frame space including the space after the last word
             }
             else {                                       // fitted view mode
                CurTabw.FrameX=PrtFrameX+ScrToPrtX(adj);          // frame space position
                CurTabw.FrameWidth=PrtFrameWidth-ScrToPrtX(adj);  // frame space width
                CurTabw.FrameScrWidth=FrameX+FrameWidth-WordBeginX;  // width of the frame space including the space after the last word
                CurTabw.FrameSpaceWidth=FrameX+FrameWidth-WordBeginX-adj;  // width of the frame space including the space after the last word
             }

             WordBeginX=FrameX+FrameWidth;            // start current word after the frame
             CurLen=WordBeginX+WordLen;
             if (CurLen>(WrapWidth-RightIndent)) {    // restart the word
                 LineEnd=WordBeginIdx;
                 CurLen=WordBeginX;
                 continue;
             } 
          }

          // measure text length of center and decimal tabs
          if (CurChar==decimal && !CurHidden) DecimalFound=TRUE;
          if (PrevTabType==TAB_CENTER || (PrevTabType==TAB_DECIMAL && DecimalFound)) {
             if (CurChar!=ParaChar) {
                int width=CharWidth;
                if (width==-1) width=WideCharWidth(w,CurFmt,!UsePrtUnits,DispChar); // current line length for wrapping
                PrevTextLen=PrevTextLen+width;                                // length of text after the previous tab position
                PrevScrTextLen=PrevScrTextLen+WideCharWidth(w,CurFmt,TRUE,CurChar); // length of text after the previous tab position
             }
          }
       }


       // note the non-breaking space/dash character
       if (CurChar==NBSPACE_CHAR) lflags|=LFLAG_NBSPACE;
       if (CurChar==NBDASH_CHAR)  lflags|=LFLAG_NBDASH;
       if (CurChar==HYPH_CHAR)  lflags|=LFLAG_HYPH;
       if (CurChar==HPARA_CHAR)  lflags|=LFLAG_HPARA;
       if (CurChar==HIDDEN_CHAR)  lflags2|=LFLAG2_HIDDEN_CHAR;

       // calcualte wrap space flag
       CurWrapSpaces=WrapSpaces;
       if (IsRtl && !(TerFont[CurFmt].rtl)) CurWrapSpaces=TRUE;
       
       // calculate any adjustment to wrap width
       if (CurWrapParaFID>0 && UsePrtUnits && (LineEnd+2)==BufferLength && wrap[LineEnd+1]==ParaChar) WrapWidthAdj=PrtResX/30; // allow tolerance so a line does not wrap in a frame line
         
       // Check for line wrap condition
       if (  (   NoWrap || CurHidden
              || (CurLen<=(WrapWidth-RightIndent+WrapWidthAdj) && !WrapChars)
              || (TerArg.PrintView && CurChar==' ' && !CurWrapSpaces)
              || (TerArg.PrintView && CurChar==ParaChar 
                      && (PrevChar==' ' || PrevChar==TAB || PrevHidden))
              || (WrapChars && (LineEnd-LineBegin)<WrapChars) )
           && ((LineEnd-LineBegin)<(MAX_WIDTH-2)) ){

          LineEnd++;
          if (CurChar==ParaChar) {
             SectBreak=SectBreakFollows;
             if (SectBreak) LineEnd++;    // go past the following text break
             break;// process this paragraph line
          }
          if (LineEnd>=BufferLength) break;  // process the last line of the buffer
          
          IsHyphen=(CurChar=='-' && TerFont[CurFmt].CharSet!=2);
          IsAsianChar=FALSE;
          if (TerFont[CurFmt].UcBase>0) {                  // individual characters are whole words
             WORD UChar=TerFont[CurFmt].UcBase+CurChar;
             IsAsianChar=((UChar>=0x2e00 && UChar<=0x9faf)|| 
                          (UChar>=0xf900 && UChar<0xfaff) ||
                          (UChar>=0xfe30 && UChar<0xfe4f) ||
                          (UChar>=0xff00 && UChar<0xffef) );
             if (!IsAsianChar) {                           // japanese char
               IsAsianChar=(UChar>=0x4e00 && UChar<=0xa7ff); 
             } 
          }
          if (!IsAsianChar && LeadWrapped && WrapLead[LineEnd]>0 && DefLang==1041) IsAsianChar=true;  // Japanese lang  

          if (CurChar==' ' || CurChar==TAB || IsHyphen || CurChar==HYPH_CHAR || CurChar==HIDDEN_CHAR || IsAsianChar) {
             BOOL CharShowing=(ShowHiddenText || !(TerFont[CurFmt].style&HIDDEN))
                      && (ShowFieldNames || TerFont[CurFmt].FieldId!=FIELD_NAME)
                      && (!ShowFieldNames || TerFont[CurFmt].FieldId!=FIELD_DATA);
             if (CharShowing) {
                WordEnd=LineEnd; // word wrap delimiter found
                SaveCurLen=CurLen;
                if (CurChar==TAB && PfmtId[CurParaId].flags&JUSTIFY && !HiddenText(w,CurFmt)) JustSpaceIgnore=JustSpaceCount;  // spaces until the last tab are to be ignored
                if (CurChar==' ' && PfmtId[CurParaId].flags&JUSTIFY && !HiddenText(w,CurFmt)) JustSpaceCount++;
                EndChar=CurChar;
                WordBeginX=CurLen;      // next word position
                WordBeginIdx=LineEnd;   // next word index
             }
          }
       }
       else {                        // process this line
          // don't break if no word yet found and cur char is para char
          if ( (WordEnd==0 && LineEnd==LineBegin)
            || (WordEnd==0 && CurChar==ParaChar && LineEnd>LineBegin && wrap[LineEnd-1]!=ParaChar)) {
             LineEnd++;              // wait till the next character
             if (LineEnd>=BufferLength) break;  // process the last line of the buffer
          }
          else {                     // break now
             if (WordEnd!=0) {
                LineEnd=WordEnd;     // end the line at the word break

                CurLen=SaveCurLen;
                if (EndChar==' ') JustSpaceCount--; // do not consider the last space
                CurChar=EndChar;     // last character
             }
             if (LineEnd==LineBegin) LineEnd=LineBegin+1;  // atleast one character per line
             break;
          }
       }
    }


    // Set previous right tab if any
    if (  (TRUE || CurChar==ParaChar || CurChar==CellChar || BreakChar!=0) 
       && CurTabw.count>0 && PrevTabType!=TAB_LEFT) {// set the width of previous right/center tab

       PrevTabWidth=PrevTabPos-CurLen;          // width of the previous tab
       if (PrevTextLen<0) PrevTextLen=0;
       if      (PrevTabType==TAB_CENTER)  PrevTabWidth+=(PrevTextLen/2); // center
       else if (PrevTabType==TAB_DECIMAL) PrevTabWidth+=PrevTextLen; // center
       if (PrevTabWidth<0) PrevTabWidth=0;

       CurLen+=PrevTabWidth;
       if (CurLen>WrapWidth) TabPastRuler=TRUE;

       CurTabw.width[CurTabw.count-1]=PrevTabWidth;
    }


    // add the width of the break characters
    if (CurChar==ParaChar || BreakChar!=0) {
       CurLen+=WideCharWidth(w,CurFmt,!UsePrtUnits,CurChar);
       if (SectBreak) CurLen+=WideCharWidth(w,CurFmt,!UsePrtUnits,SECT_CHAR);   // exact width is not important since this character is never printed
    }

    // add the width of the last optional hyphen character
    if (CurChar==HYPH_CHAR && (!ShowParaMark || InPrinting)) {
       CurLen+=WideCharWidth(w,CurFmt,!UsePrtUnits,'-');
    }

    //LINE_EXTRACTED:

    LastWrappedLine++;                       // line number of this line

    if (LastWrappedLine>LastBufferedLine) {  // scroll down 1 line
       if (!DisplacePointers(w,LastBufferedLine+1,WrapAddLines)) {   // allocate in batches of 10 lines
            LastWrappedLine--;
            return TRUE;
       }
    }

    // set the data into the line being wrapped
    CopyWrapLineData(w,LineBegin,LineEnd,CurChar);
    pfmt(LastWrappedLine)=CurParaId;          // assign the paragraph id
    cid(LastWrappedLine)=CurWrapCell;         // assign the cell id
    fid(LastWrappedLine)=CurWrapParaFID;      // assign the para frame id

    if (LastWrappedLine==(TotalLines-1) && LineEnd==BufferLength) cid(LastWrappedLine)=0;
    
    if (CurWrapCell>0 && BreakChar==CellChar) {
       long l;
       BOOL CellHidden=HiddenText(w,CurFmt);
       for (l=LastWrappedLine;l>=0;l--) {
         if (cid(l)==0 || (cell[cid(l)].level==cell[CurWrapCell].level && cid(l)!=CurWrapCell)) break;
         if (LineHt(l)>0) {
            CellHidden=FALSE;
            break;
         } 
       } 
       if (CellHidden) CellAux[CurWrapCell].flags|=CAUX_HIDDEN;
       else            ResetUintFlag(CellAux[CurWrapCell].flags,CAUX_HIDDEN);
    }    


    // Set the line flags
    PrevTextLine=LastWrappedLine-1;
    if (PrevTextLine<0) PrevTextLine=0;

    LineFlags(LastWrappedLine)=(lflags|(LineFlags(LastWrappedLine)&(LFLAG_SOFT_COL|LFLAG_FRAME_TOP)));    // retain the soft col break flag
    LineFlags2(LastWrappedLine)=lflags2;

    if (CurChar==ParaChar || CurChar==CellChar) LineFlags(LastWrappedLine)|=LFLAG_PARA;
    if (BreakChar==LINE_CHAR) LineFlags(LastWrappedLine)|=LFLAG_LINE;
    else if (BreakChar!=0) LineFlags(LastWrappedLine)|=LFLAG_BREAK;

    if (BreakChar) SetHdrFtrLineFlags(w,LastWrappedLine,(BYTE)BreakChar);

    if (SectBreak || BreakChar==SECT_CHAR) LineFlags(LastWrappedLine)|=LFLAG_SECT;
    if (IsToc)     LineFlags(LastWrappedLine)|=LFLAG_TOC;
    if (IsListnum) LineFlags(LastWrappedLine)|=LFLAG_LISTNUM;
    if (IsAutoNumLgl) LineFlags(LastWrappedLine)|=LFLAG_AUTONUMLGL;

    if (IsFirstParaLine(w,LastWrappedLine)) LineFlags(LastWrappedLine)|=LFLAG_PARA_FIRST;

    if (ListTextWidth>0) LineFlags(LastWrappedLine)|=LFLAG_LIST;         // contains a list number

    if (ControlPos>=0 && ControlPos<LineEnd) LineFlags(LastWrappedLine)|=LFLAG_CONTROL;
    if (HtmlMode && PfmtId[CurParaId].AuxId>0 && IsHtmlRule(w,CurParaId)) LineFlags(LastWrappedLine)|=LFLAG_HTML_RULE;
    
    if (HasAssumedTab) LineFlags(LastWrappedLine)|=LFLAG_ASSUMED_TAB;

    if (PfmtId[CurParaId].flags&PARA_BOX_TOP && LineFlags(LastWrappedLine)&LFLAG_PARA_FIRST && fid(LastWrappedLine)==0) {                               // check for top line
        if (LastWrappedLine==0 || !HasSameParaBorder(w,LastWrappedLine-1,LastWrappedLine)) LineFlags(LastWrappedLine)|=LFLAG_BOX_TOP;
    }
    if (PfmtId[CurParaId].flags&PARA_BOX_BOT && LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK) && fid(LastWrappedLine)==0)  { // check for the bottom line
       if (LastWrappedLine>=(TotalLines-1) || !HasSameParaBorder(w,LastWrappedLine+1,LastWrappedLine)) LineFlags(LastWrappedLine)|=LFLAG_BOX_BOT;
    }
    if (PfmtId[CurParaId].flags&PARA_BOX_BETWEEN && LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK) && fid(LastWrappedLine)==0)  { // check for the bottom line
       if (LastWrappedLine<(TotalLines-1) && HasSameParaBorder(w,LastWrappedLine+1,LastWrappedLine)) {
          if (!(LineFlags(LastWrappedLine)&LFLAG_BOX_BOT)) LineFlags2(LastWrappedLine)|=LFLAG2_BOX_BETWEEN;
       }
    }
    if (PfmtId[CurParaId].shading>0 && LineFlags(LastWrappedLine)&LFLAG_PARA_FIRST) {                               // check for top line
        if (LastWrappedLine==0 || !HasSameParaShading(w,LastWrappedLine-1,LastWrappedLine)) LineFlags(LastWrappedLine)|=LFLAG_SHADE_BEGIN;
    }
    if (PfmtId[CurParaId].shading>0 && LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK))  { // check for the bottom line
       if (LastWrappedLine>=(TotalLines-1) || !HasSameParaShading(w,LastWrappedLine+1,LastWrappedLine)) LineFlags(LastWrappedLine)|=LFLAG_SHADE_END;
    }

    if (WrapSpellChecked) LineFlags2(LastWrappedLine)|=LFLAG2_SPELL_CHECKED;  // this line already spell checked
    if (IsRtl)            LineFlags2(LastWrappedLine)|=LFLAG2_RTL;   // right-to-left flow for this line

    // pixels to be adjusted for justifiation or when the line exceeds the width
    AdjLen=WrapWidth-RightIndent-CurLen;
    if (!IsRtl) AdjLen+=EndSpaceLen;

    // store width for the tabs used in this line, also mark the breaks
    if (CurTabw.count>0 || FrameWidth>0 || BoxFmtUsed
      || BreakChar!=0   || SectBreak
      || PfmtId[CurParaId].flags&JUSTIFY || AdjLen>0
      || ListTextWidth>0
      || (DynFieldPos>=0 && DynFieldPos<LineEnd) || FrmSpcBef>0) {

       UINT type;

       // allocate the tabw structure for this line
       if (!tabw(LastWrappedLine)) {
          if (!AllocTabw(w,LastWrappedLine)) AbortTer(w,"RAN OUT OF MEMORY (WrapParseBuffer)",40);
       }
       else FreeTabwMembers(w,tabw(LastWrappedLine));

       FarMove(&CurTabw,tabw(LastWrappedLine),sizeof(CurTabw));

       tabw(LastWrappedLine)->type=tabw(LastWrappedLine)->type&
          (~((UINT)(INFO_SECT|INFO_PAGE|INFO_CELL|INFO_ROW|INFO_COL|INFO_JUST|INFO_FRAME|INFO_DYN_FIELD|INFO_FRM_SPC_BEF)));
       if (tabw(LastWrappedLine)->count==0) tabw(LastWrappedLine)->type=ResetUintFlag(tabw(LastWrappedLine)->type,INFO_TAB);
       tabw(LastWrappedLine)->JustCount=tabw(LastWrappedLine)->JustAdj=0;
       type=tabw(LastWrappedLine)->type;

       if (BreakChar!=0 || SectBreak) {  // set the section info
          if (BreakChar==SECT_CHAR || SectBreak) tabw(LastWrappedLine)->type=type|INFO_SECT;
          
          if (BreakChar==PAGE_CHAR) tabw(LastWrappedLine)->type=type|INFO_PAGE;
          if (BreakChar==COL_CHAR)  tabw(LastWrappedLine)->type=type|INFO_COL;
          
          if (BreakChar==CellChar)   tabw(LastWrappedLine)->type=type|INFO_CELL;
          if (BreakChar==ROW_CHAR)   tabw(LastWrappedLine)->type=type|INFO_ROW;

          if (BreakChar!=CellChar && BreakChar!=LINE_CHAR && !SectBreak)
             tabw(LastWrappedLine)->count=0;
          tabw(LastWrappedLine)->section=WrapSect;
       }

       if (FrameWidth>0 && TerArg.PrintView) tabw(LastWrappedLine)->type|=INFO_FRAME;
       if (DynFieldPos>=0 && DynFieldPos<LineEnd) tabw(LastWrappedLine)->type|=INFO_DYN_FIELD;
       if (FrmSpcBef>0) {
          tabw(LastWrappedLine)->type|=INFO_FRM_SPC_BEF;
          tabw(LastWrappedLine)->height=FrmSpcBef;
       }

       // set list text
       if (ListTextWidth>0) {
          tabw(LastWrappedLine)->ListText=MemAlloc(lstrlen(ListText)+1);
          lstrcpy(tabw(LastWrappedLine)->ListText,ListText);
          tabw(LastWrappedLine)->ListTextWidth=ListTextWidth;
          tabw(LastWrappedLine)->ListFontId=ListFontId;
          tabw(LastWrappedLine)->ListNbr=ListNbr;
       } 

       // calculation adjustment for justification
       if ((PfmtId[CurParaId].flags&JUSTIFY || AdjLen<0) && (!(tabw(LastWrappedLine)->type&(INFO_FRAME)) || tabw(LastWrappedLine)->FrameX==0) ) {
          int SpaceCount=JustSpaceCount-JustSpaceIgnore;
          if (AdjLen>0 && SpaceCount>0 && CurChar!=ParaChar && (BreakChar==0 || BreakChar==LINE_CHAR)) {  // distribute adjustment among spaces
             (tabw(LastWrappedLine)->type)|=INFO_JUST;
             tabw(LastWrappedLine)->JustAdj=AdjLen/SpaceCount;
             tabw(LastWrappedLine)->JustCount=AdjLen-(tabw(LastWrappedLine)->JustAdj*SpaceCount);  // extra
             tabw(LastWrappedLine)->JustSpaceCount=JustSpaceCount;
             tabw(LastWrappedLine)->JustSpaceIgnore=JustSpaceIgnore;
          }
       }

       // set char flags if box format used
       if (BoxFmtUsed) {
          int i,len;
          LPBYTE ptr;
          AllocTabwCharFlags(w,LastWrappedLine);

          // set the BFLAG_BOX_? flags
          BoxOn=FALSE;
          fmt=cfmt(LastWrappedLine).pFmt;
          ptr=pText(LastWrappedLine);
          len=LineLen(LastWrappedLine);
          if (LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK)) len--;
          for (i=len-1;i>=0;i--) if (ptr[i]!=' ') break;      // strip spaces from length caculation
          len=i+1; 
          for (i=0;i<len;i++) {
             if (!BoxOn && TerFont[fmt[i]].style&CHAR_BOX) {
                tabw(LastWrappedLine)->CharFlags[i]|=BFLAG_BOX_FIRST;
                BoxOn=TRUE;
             }
             else if (BoxOn && !(TerFont[fmt[i]].style&CHAR_BOX)) {
                tabw(LastWrappedLine)->CharFlags[i-1]|=BFLAG_BOX_LAST;
                BoxOn=FALSE;
             }
          }
          if (BoxOn) tabw(LastWrappedLine)->CharFlags[i-1]|=BFLAG_BOX_LAST;  // set the last byte
       }
    }
    else if (tabw(LastWrappedLine)) FreeTabw(w,LastWrappedLine);        // free the tab width object for the line

    // set the LISTNUM field values
    if (IsListnum || LineFlags(LastWrappedLine)&LFLAG_HPARA)    SetListnum(w,LastWrappedLine,UsePrtUnits);
    if (IsAutoNumLgl) SetAutoNumLgl(w,LastWrappedLine,UsePrtUnits);

    // reset para border for break lines
    if (tabw(LastWrappedLine) && tabw(LastWrappedLine)->type&(INFO_ROW|INFO_PAGE|INFO_COL)) {
       ResetLongFlag(LineFlags(LastWrappedLine),(LFLAG_BOX_TOP|LFLAG_BOX_BOT));
       ResetLongFlag(LineFlags2(LastWrappedLine),(LFLAG2_BOX_BETWEEN));
    }

    // adjust frame width when text goes past the frame width
    if (CurWrapParaFID>0 && !InPrinting && !(ParaFrame[CurWrapParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT))) {
       int ParaFID=CurWrapParaFID;
       int lLen=LineLen(LastWrappedLine);
       int pict=GetCurCfmt(w,LastWrappedLine,0);
       len=ScrToTwipsX(TerFont[pict].CharWidth[PICT_CHAR]);
       if (lLen==1 || (lLen==2 && LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_BREAK))) {
          if (TerFont[pict].style&PICT && len>(ParaFrame[ParaFID].width-2*ParaFrame[ParaFID].margin))
             ParaFrame[ParaFID].width=len+2*ParaFrame[ParaFID].margin;
       }
    }


    CompressCfmt(w,LastWrappedLine);         // lines with tab will not be compressed

    // Calculate line height for Print View mode
    if (TerArg.PrintView) {
       int CurHeight,CurFont,CurDescentHeight,ExtLead;
       int BigVal=9999;
       int SpcBef,SpcAft,LineFont=0,OffsetY,NegOffsetY=BigVal,PosOffsetY=BigVal,CurOffset;
       BOOL SpaceFound=FALSE,BreakIgnored=FALSE;
       BOOL ZeroLineHeight=FALSE;

       BaseHeight=DescentHeight=ExtLead=0;

       if (LineLen(LastWrappedLine)==1 && BreakChar==CellChar) {  // check for cell marker line
          if ((TerFlags3&TFLAG3_ZERO_CELL_HEIGHT)) ZeroLineHeight=TRUE;
          else if (LastWrappedLine>0) {
             int CurCell=cid(LastWrappedLine);
             int CurLevel=cell[CurCell].level;
             int PrevCell=cid(LastWrappedLine-1);
             if (CurCell!=PrevCell && CurCell==LevelCell(w,CurLevel,-PrevCell)) ZeroLineHeight=TRUE;;
          }
       }
       if (LineLen(LastWrappedLine)==1 && BreakChar==LINE_CHAR) {  // check for lbr ids
          if (cfmt(LastWrappedLine).info.type==UNIFORM) CurFont=cfmt(LastWrappedLine).info.fmt;
          else {
             fmt=cfmt(LastWrappedLine).pFmt;
             CurFont=fmt[0];
          }
          if (TerFont[CurFont].CharId>0) ZeroLineHeight=true;   // clear options 
       } 


       if (cfmt(LastWrappedLine).info.type==UNIFORM) {
          CurFont=cfmt(LastWrappedLine).info.fmt;

          BaseHeight=PrtFont[CurFont].BaseHeight;
          DescentHeight=PrtFont[CurFont].height-PrtFont[CurFont].BaseHeight-PrtFont[CurFont].ExtLead;
          if (PrtFont[CurFont].OffsetVal>0) DescentHeight-=PrtFont[CurFont].OffsetVal;
          OffsetY=abs(PrtFont[CurFont].OffsetVal);
          ExtLead=PrtFont[CurFont].ExtLead;
       }
       else {
          BOOL TextFound=FALSE;
          LPBYTE ptr;

          fmt=cfmt(LastWrappedLine).pFmt;
          ptr=pText(LastWrappedLine);

          len=LineLen(LastWrappedLine);
          if (ZeroLineHeight) {
             len=0;
             BreakIgnored=TRUE; 
          }

          if (len>1 && LineFlags(LastWrappedLine)&(LFLAG_PARA|LFLAG_LINE)) {
             LineFont=fmt[len-1];
             len--;
             BreakIgnored=TRUE;
             if (len>0 && LineFlags(LastWrappedLine)&LFLAG_PICT) {  // check if line contains just the thumbnails
               for (j=0;j<len;j++) if (!(TerFont[fmt[j]].style&PICT) || TerFont[fmt[j]].ParaFID==0) break;
               if (j==len) len++;
             }
          }


          for (j=0;j<len;j++) {
             CurFont=fmt[j];

             if (!SpaceFound && (ptr[j]==' ' || ptr[j]==TAB) && PrtFont[CurFont].height>0) SpaceFound=TRUE;
             if ((ptr[j]==' ' || ptr[j]==TAB) && (j<(len-1) || TextFound)) continue;

             if (TerFont[CurFont].style&PICT && TerFont[CurFont].ParaFID>0) continue;  // ignore the thumbnail picture height

             if (PrtFont[CurFont].BaseHeight>BaseHeight) BaseHeight=PrtFont[CurFont].BaseHeight;
             CurDescentHeight=PrtFont[CurFont].height-PrtFont[CurFont].BaseHeight-PrtFont[CurFont].ExtLead;
             if (PrtFont[CurFont].OffsetVal>0) CurDescentHeight-=PrtFont[CurFont].OffsetVal;
             if (CurDescentHeight>DescentHeight) DescentHeight=CurDescentHeight;
             if (PrtFont[CurFont].ExtLead>ExtLead) ExtLead=PrtFont[CurFont].ExtLead;

             CurOffset=PrtFont[CurFont].OffsetVal;
             if (CurOffset==0) NegOffsetY=PosOffsetY=0;
             else if (CurOffset>0 && CurOffset<PosOffsetY) PosOffsetY=CurOffset;  // minimum positive offset 
             else if (CurOffset<0 && (-CurOffset)<NegOffsetY) NegOffsetY=-CurOffset;  // minimum negative offset 

             TextFound=TRUE;
          }
          if (PosOffsetY==BigVal) PosOffsetY=0;  // not used
          if (NegOffsetY==BigVal) NegOffsetY=0;  // not used
          if (PosOffsetY && NegOffsetY) PosOffsetY=NegOffsetY=0;  // height adjustment not needed
          OffsetY=PosOffsetY+NegOffsetY;
       }
       if (DescentHeight<0) DescentHeight=0;
       
       CurHeight=BaseHeight+DescentHeight+ExtLead-OffsetY;

       if (CurHeight==0 && (SpaceFound || BreakIgnored)) CurHeight=PrtFont[LineFont].height+PrtFont[LineFont].ExtLead;
       
       if (ZeroLineHeight) CurHeight=TwipsToPrtY(20);
       else {
          // apply paragraph before and after spacing
          GetLineSpacing(w,LastWrappedLine,CurHeight,&SpcBef,&SpcAft,FALSE);
          CurHeight+=(SpcBef+SpcAft);
          BaseHeight+=SpcBef;
       }
       
       // set the height of the only line in header/footer to 0
       if (LineLen(LastWrappedLine)==1 && PfmtId[CurParaId].flags&PAGE_HDR_FTR && !EditPageHdrFtr
          && ((LastWrappedLine+1)<TotalLines && LineFlags(LastWrappedLine+1)&LFLAG_HDRS_FTRS)
          && ((LastWrappedLine-1)>=0 && LineFlags(LastWrappedLine-1)&LFLAG_HDRS_FTRS) ){
          CurHeight=0;
       }

       // set repagination line when some text entered into an empty header footer
       if (PfmtId[CurParaId].flags&PAGE_HDR_FTR && CurHeight!=LineHt(LastWrappedLine)
          && (CurHeight==0 && LineHt(LastWrappedLine)==0)
          && RepageBeginLine>LastWrappedLine && !repaginating) RepageBeginLine=LastWrappedLine;


       // zero height for non-printable lines
       if ( BreakChar!=0 && BreakChar!=COL_CHAR && BreakChar!=LINE_CHAR
         && BreakChar!=CellChar && BreakChar!=PAGE_CHAR
         && BreakChar!=SECT_CHAR) CurHeight=0;
       if (   TerArg.PageMode && (BreakChar==ROW_CHAR
           || IsHdrFtrChar(BreakChar))) CurHeight=0;
       if (TerArg.PageMode && (BreakChar==COL_CHAR || BreakChar==PAGE_CHAR)) CurHeight=ScrToPrtY(RulerFontHeight);
       if (TerArg.PageMode && BreakChar==SECT_CHAR && LineLen(LastWrappedLine)==1) {
          if ((LastWrappedLine+1)<TotalLines) {
             //int NewSect=GetSection(w,LastWrappedLine+1);  during wordwrapping the next line may not be formatted yet
             int sect=GetSection(w,LastWrappedLine);
             int NewSect=TerSect1[sect].NextSect;
             if (NewSect<0 || NewSect>=TotalSects) NewSect=0;
           
             if (!(TerSect[NewSect].flags&SECT_NEW_PAGE)) CurHeight=0;
          }
       }
       
       if (CurHeight==0) ResetLongFlag(LineFlags(LastWrappedLine),(LFLAG_BOX_TOP|LFLAG_BOX_BOT));

       if (LineHt(LastWrappedLine)!=CurHeight && PaintFlag<PAINT_WIN) PaintFlag=PAINT_WIN;
       
       LineHt(LastWrappedLine)=CurHeight;
       if (TerOpFlags&TOFLAG_INSERT_CHAR && LastWrappedLine==CurLine 
          && PrtToScrY(CurHeight)>ScrLineHt(LastWrappedLine)) ScrLineHt(LastWrappedLine)=PrtToScrY(CurHeight);  // rough estimate - will be revised during frame creation
       BaseLineHt(LastWrappedLine)=BaseHeight;
       if (BaseLineHt(LastWrappedLine)>LineHt(LastWrappedLine)) BaseLineHt(LastWrappedLine)=LineHt(LastWrappedLine);

    }

    goto EXTRACT_LINE;  // process next line in this para
}
#pragma optimize("",off)  // restore optimization

/******************************************************************************
    CopyWrapLineData:
    Copy data into the line begin wrapped
    parse the buffer to product word wrapped lines
******************************************************************************/
CopyWrapLineData(PTERWND w,int LineBegin,int LineEnd, WORD CurChar)
{
    LPBYTE ptr;
    LPWORD fmt;

    if (LineLen(LastWrappedLine)!=LineEnd-LineBegin) {   // ensure proper allocation for this line
        if (LineLen(LastWrappedLine)>0) sFree(pText(LastWrappedLine));
        if (cfmt(LastWrappedLine).info.type!=UNIFORM) sFree(cfmt(LastWrappedLine).pFmt);
        if (pCtid(LastWrappedLine))     {
           sFree(pCtid(LastWrappedLine));
           pCtid(LastWrappedLine)=NULL;
        }
        if (pCharWidth(LastWrappedLine))  {
           sFree(pCharWidth(LastWrappedLine));
           pCharWidth(LastWrappedLine)=NULL;
        }
        if (pLead(LastWrappedLine)) {
           sFree(pLead(LastWrappedLine));
           pLead(LastWrappedLine)=NULL;
        }

        if (LineEnd-LineBegin>0) {
           pText(LastWrappedLine)=sAlloc(LineEnd-LineBegin+1);
           cfmt(LastWrappedLine).pFmt=sAlloc(sizeof(WORD)*(LineEnd-LineBegin+1));
           if (TagsWrapped) pCtid(LastWrappedLine)=sAlloc(sizeof(WORD)*(LineEnd-LineBegin+1));
           if (CharWidthWrapped) pCharWidth(LastWrappedLine)=sAlloc(sizeof(WORD)*(LineEnd-LineBegin+1));
           if (LeadWrapped) pLead(LastWrappedLine)=sAlloc(sizeof(BYTE)*(LineEnd-LineBegin+1));
        }
        else {
           cfmt(LastWrappedLine).info.type=UNIFORM;
           cfmt(LastWrappedLine).info.fmt=WrapCfmt[LineBegin];
           pCtid(LastWrappedLine)=NULL;
           pCharWidth(LastWrappedLine)=NULL;
           pLead(LastWrappedLine)=NULL;
        }
        LineLen(LastWrappedLine)=LineEnd-LineBegin;

        if (PaintFlag!=PAINT_WIN && CurChar!=ParaChar && CurChar!=CellChar)
           PaintFlag=PAINT_WIN;  // possibly more than one line modified
    }

    if (LineLen(LastWrappedLine)>0) {
        ptr=pText(LastWrappedLine);
        fmt=cfmt(LastWrappedLine).pFmt;
        FarMove(&wrap[LineBegin],ptr,LineLen(LastWrappedLine));
        FarMove(&WrapCfmt[LineBegin],fmt,sizeof(WORD)*LineLen(LastWrappedLine));
        if (TagsWrapped) {
           if (!pCtid(LastWrappedLine)) pCtid(LastWrappedLine)=sAlloc(sizeof(WORD)*LineLen(LastWrappedLine));
           FarMove(&WrapCtid[LineBegin],pCtid(LastWrappedLine),sizeof(WORD)*LineLen(LastWrappedLine));
           CompressCtid(w,LastWrappedLine);
        }
        else {
           if (pCtid(LastWrappedLine))     sFree(pCtid(LastWrappedLine));
           pCtid(LastWrappedLine)=NULL;
        }

        if (CharWidthWrapped) {
           if (!pCharWidth(LastWrappedLine)) pCharWidth(LastWrappedLine)=sAlloc(sizeof(WORD)*(LineLen(LastWrappedLine)+1));
           FarMove(&WrapCharWidth[LineBegin],pCharWidth(LastWrappedLine),sizeof(WORD)*LineLen(LastWrappedLine));
        }
        else {
           if (pCharWidth(LastWrappedLine))  sFree(pCharWidth(LastWrappedLine));
           pCharWidth(LastWrappedLine)=NULL;
        }
        
        if (LeadWrapped) {
           if (!pLead(LastWrappedLine)) pLead(LastWrappedLine)=sAlloc(sizeof(WORD)*LineLen(LastWrappedLine));
           FarMove(&WrapLead[LineBegin],pLead(LastWrappedLine),sizeof(BYTE)*LineLen(LastWrappedLine));
           CloseLead(w,LastWrappedLine);
        }
        else {
           if (pLead(LastWrappedLine))     sFree(pLead(LastWrappedLine));
           pLead(LastWrappedLine)=NULL;
        }
    }

    return TRUE;
}

/******************************************************************************
    IsFirstParaLine:
    Check if this is the first line of a paragraph
******************************************************************************/
BOOL IsFirstParaLine(PTERWND w,long LineNo)
{
    long l;

    if (LineNo==0) return TRUE;
    else {                              // scan previous lines for para/cell break
        for (l=LineNo-1;l>=0;l--) if (!(LineInfo(w,l,(INFO_PAGE|INFO_COL)))) break;  // go past any hard page or column breaks
        if (l<0 || LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK)) return TRUE;
    }

    return FALSE;
}

/******************************************************************************
    GetWrapCharWidth:
    Set character width array for the wrap buffer.
******************************************************************************/
BOOL GetWrapCharWidth(PTERWND w)
{
    int PrevFont=0,i,j,PrevIdx=-1,count,width;
    int CurOrder,PrevOrder;
    WORD UcBase;
    BOOL UsePrtUnits;
    HDC hDC;
    HFONT hPrevFont;

    CharWidthWrapped=FALSE;

    //if (!HasVarWidthFont) return FALSE;   // document does not context-sensitive character width fonts
    if (BufferLength==0) return FALSE;

    if (TerArg.PrintView && !TerArg.FittedView) UsePrtUnits=TRUE;
    else                                        UsePrtUnits=FALSE;

    for (i=0;i<=BufferLength;i++) {
       if (PrevIdx==-1) {          // first
          if (wrap[i]<32) continue;  // delimiter characters are always shown in ansi font
          PrevFont=WrapCfmt[i];
          PrevIdx=i;
          continue;
       }
       if (PrevFont!=WrapCfmt[i] || wrap[i]<32 || i==BufferLength) {  // font changed of special character
          if (i>PrevIdx) {  // get the width for these characters
             BOOL result,skip=true;
             LPINT pOrder,pDx;
             UINT FirstUniChar,LastUniChar;
             
             // check if GetCharacterPlacement needs to be used
             if (TerFont[PrevFont].VarWidth || TerFont[PrevFont].rtl) skip=false;
             if (True(TerFlags6&TFLAG6_ALT_UNI_CHARWIDTH) && TerFont[PrevFont].UcBase>0) skip=false;

             FirstUniChar=TerFont[PrevFont].UcBase+128;  // first possible unicode character using this UcBase
             LastUniChar=TerFont[PrevFont].UcBase+255;
             if (skip && FirstUniChar>=2304 && FirstUniChar<=2431) skip=false;   // hindi code point - add 128 because in UcBase+ptr[i] , ptr[i] is between 128 and 255
             if (skip && LastUniChar>=2304 && LastUniChar<=2431) skip=false;   // hindi code point

             if (skip && FirstUniChar>=2944 && FirstUniChar<=3071) skip=false;             // tamil code point
             if (skip && LastUniChar>=2944 && LastUniChar<=3071) skip=false; // tamil code point

             if (skip && wrap[PrevIdx]==' ' && WrapTextFlow==FLOW_RTL && WrapCharWidth!=null && !(TerFont[PrevFont].rtl)) skip=false;  // 20050928

             if (skip) goto END_SEGMENT;
            
             if (TerFont[PrevFont].rtl) WrapTextFlow=FLOW_RTL;    // text flow direction for the paragraph

             // allocate global arrays if not already allocated
             if (!WrapCharWidth && (NULL==(WrapCharWidth=OurAlloc((long)WrapBufferSize*sizeof(WORD))))) return FALSE;
             if (!WrapCharWidthOrder && (NULL==(WrapCharWidthOrder=OurAlloc((long)WrapBufferSize*sizeof(int))))) return FALSE;
             if (!WrapCharWidthDX && (NULL==(WrapCharWidthDX=OurAlloc((long)WrapBufferSize*sizeof(int))))) return FALSE;
             if (!WrapCharWidthText && (NULL==(WrapCharWidthText=OurAlloc((long)WrapBufferSize*sizeof(WORD))))) return FALSE;
             if (!WrapCharWidthClass && (NULL==(WrapCharWidthClass=OurAlloc((long)WrapBufferSize*sizeof(BYTE))))) return FALSE;

             // initialize the WrapCharWidth array
             if (!CharWidthWrapped) for (j=0;j<BufferLength;j++) WrapCharWidth[j]=0;

             // Get width of each character
             count=i-PrevIdx;

             hDC=(UsePrtUnits?hPr:hTerDC);
             hPrevFont=SelectObject(hDC,(UsePrtUnits?PrtFont[PrevFont].hFont:TerFont[PrevFont].hFont));

             // Get character placement information
             UcBase=TerFont[PrevFont].UcBase;
             if (UcBase>0) {
                GCP_RESULTSW gcpW;

                for (j=0;j<count;j++) WrapCharWidthText[j]=MakeUnicode(w,UcBase,wrap[PrevIdx+j]);  // text in unicode form
                WrapCharWidthText[j]=0;

                FarMemSet(&gcpW,0,sizeof(gcpW));
                gcpW.lStructSize=sizeof(gcpW);
                gcpW.lpDx=WrapCharWidthDX;
                gcpW.lpOrder=WrapCharWidthOrder;
                gcpW.lpClass=WrapCharWidthClass;
                gcpW.nGlyphs=count;

                result=GetCharacterPlacementW(hDC,WrapCharWidthText,count,0,&gcpW,GCP_GLYPHSHAPE|GCP_REORDER);
                pOrder=gcpW.lpOrder;
                pDx=gcpW.lpDx;

                if (LOWORD(result)==0) {          // x extent is zero, use the GetTextExtentPoint method
                   SIZE size;
                   int  TotalWidth=0,ExtWidth=0,dif,SecondWidth;
                   for (j=0;j<count;j++) {     // get individual character widths
                      GetTextExtentPointW(hDC,&(WrapCharWidthText[j]),1,&size);
                      pDx[j]=size.cx;
                      TotalWidth+=pDx[j];
                      pOrder[j]=j;
                   }
                   // check if the combined width matches the sum of individual widths
                   GetTextExtentPointW(hDC,WrapCharWidthText,count,&size);
                   ExtWidth=size.cx;             // total extent width
                   if (ExtWidth!=TotalWidth) {   // get width in pairs
                      for (j=0;j<(count-1);j++) {
                         GetTextExtentPointW(hDC,&(WrapCharWidthText[j]),2,&size);
                         SecondWidth=size.cx-pDx[j];   // calculate width of the second character
                         if (SecondWidth<pDx[j+1]) {   // possibly a diactric character
                            pDx[j+1]=SecondWidth;
                            j++;                       // now skip the diactric
                         } 
                      } 
                      TotalWidth=0;
                      for (j=0;j<count;j++) TotalWidth+=pDx[j];
                      
                      // spread any difference
                      dif=(ExtWidth-TotalWidth);
                      if (dif!=0) {
                         for (j=0;j<count;j++) pDx[j]=(dif/count);
                         dif = dif % count;   // find the remainder
                         if (dif>0) for (j=0;j<dif;j++) pDx[j]++;
                      } 
                   } 
                   result=ExtWidth;   // force result to true 
                } 
             }
             else {
                GCP_RESULTS gcp;
                
                FarMemSet(&gcp,0,sizeof(gcp));
                gcp.lStructSize=sizeof(gcp);
                gcp.lpDx=WrapCharWidthDX;
                gcp.lpOrder=WrapCharWidthOrder;
                gcp.lpClass=WrapCharWidthClass;
                gcp.nGlyphs=count;
             
                result=GetCharacterPlacement(hDC,&(wrap[PrevIdx]),count,0,&gcp,GCP_GLYPHSHAPE|GCP_REORDER);

                pOrder=gcp.lpOrder;
                pDx=gcp.lpDx;
             } 

             // Store character placement information
             if (result!=0 && pDx) {
                PrevOrder=-1;
                for (j=0;j<count;j++) {
                   if (pOrder) {   // order may be different
                      CurOrder=pOrder[j];
                      if (PrevOrder==CurOrder) width=0;  // zero character width
                      else                     width=pDx[CurOrder];
                      PrevOrder=CurOrder;
                   }
                   else width=pDx[j];     // order not changed
                   
                   // apply character spacing
                   if (TerFont[PrevFont].expand) {
                      if (UsePrtUnits) width+=TwipsToPrtY(TerFont[PrevFont].expand);
                      else             width+=TwipsToScrY(TerFont[PrevFont].expand);
                      if (width<0) width=0;
                   }  
                   
                   WrapCharWidth[PrevIdx+j]=width|CWIDTH_USED;
                   if (WrapCharWidthClass[j]&(GCPCLASS_ARABIC|GCPCLASS_HEBREW)) {
                      WrapCharWidth[PrevIdx+j]|=CWIDTH_RTL;
                      WrapTextFlow=FLOW_RTL;    // 20050916
                   }
                   else if (wrap[PrevIdx+j]==' ' && WrapTextFlow==FLOW_RTL) {
                      WrapCharWidth[PrevIdx+j]|=CWIDTH_RTL;  // 20050928
                   }

                   //else if (TerFont[PrevFont].rtl && wrap[PrevIdx+j]>='0' && wrap[PrevIdx+j]<='9') WrapCharWidth[PrevIdx+j]|=CWIDTH_RTL;    // to print arabic numbers
                }
                CharWidthWrapped=TRUE; 
             }
             
             SelectObject(hDC,hPrevFont); 
          }
          
          END_SEGMENT:
          if (i==BufferLength) break;   // done

          PrevFont=WrapCfmt[i];
          PrevIdx=i;

          if (wrap[i]<32) PrevIdx++;  // don't include the current delimiter characte
       }  
    }
    
    return CharWidthWrapped;
} 
