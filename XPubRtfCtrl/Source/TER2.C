/*==============================================================================
   TER Editor Developer's Kit
   TER2.C
   Text editing functions

   Sub Systems, Inc.
   Software License Agreement  (1991-2000)
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
    TerInsert:
    Toggle Insert/Overtype mode.
******************************************************************************/
TerInsert(PTERWND w)
{
    if (TrackChanges) InsertMode=true;
    else {
      if (InsertMode) InsertMode=FALSE;else InsertMode=TRUE;
    }

    DisplayStatus(w);    // show the status line

    return TRUE;
}

/******************************************************************************
   ImeChar:
   Multibyte character input.
*******************************************************************************/
ImeChar(PTERWND w,WPARAM wParam)
{
   BYTE byte1=(BYTE)(wParam&0xFF);
   BYTE byte2=(BYTE)(wParam>>8);
   HIMC hImc=0;          // ime context
   BOOL done=false;
   int  i;

   if (ImeDiscards>0) {   // number of WM_IME_CHARs to discard
      ImeDiscards--;
      return true;
   } 
   

   if (byte1=='?' && byte2==0) {
      hImc=ImmGetContext(hTerWnd);
      if (hImc) {
         LPWORD pTextU;
         int LenU=ImmGetCompositionStringW(hImc,GCS_RESULTSTR,NULL,0);

         if (LenU>0) {
            pTextU=MemAlloc((LenU+2));   // LenU species number of bytes (not words)

            ImmGetCompositionStringW(hImc,GCS_RESULTSTR,pTextU,LenU);
            LenU/=sizeof(WORD);          // convert to number of characters
            
            for (i=0;i<LenU;i++) {
               KeyDownUniChar=pTextU[i];
               TerAscii(w,byte1,0);      // byte1 is meaningless here, only KeyDownUniChar will be entered
               done=true;
            } 
            
            KeyDownUniChar=0;
            MemFree(pTextU);

            ImeDiscards=LenU-1;   // so many subsequent WM_IME_CHAR msgs should be discard, becase we did the whole string here
         }

         ImmReleaseContext(hTerWnd,hImc);
      }
   }

   if (!done) {
     if (byte2) TerAscii(w,byte1,byte2);  // dbcs character
     else       TerAscii(w,byte1,0);      // sbcs character
   }

   return true;
}

/******************************************************************************
   TerAscii:
   process an ASCII characters
*******************************************************************************/
TerAscii(PTERWND w,BYTE AscCode, BYTE LeadByte)
{
   int SaveLen,i,LineFontHeight=0,SaveHilightType,TextAngle;
   LPBYTE ptr,lead;
   LPWORD fmt;
   WORD CurFmt,NewFmt;
   BOOL CurInsertMode,BlockDeleted=FALSE;
   BOOL CellLineAfterSubTable;
   WORD UcBase=0;
   BOOL FirstRtlChar=FALSE,ProtectedDataField;

   if (eval && expired) return TRUE;

   if (mbcs && !Win32) {
      if (LeadWaiting==0 && IsDBCSLeadByte(AscCode)) {
         LeadWaiting=AscCode;
         return TRUE;
      }
      LeadByte=LeadWaiting;
      if (AscCode<32) LeadByte=0;
      LeadWaiting=0;
   }

   // convert mbcs to unicode
   if (TerFlags4&TFLAG4_IME_UNICODE && LeadByte) {
      WORD UChar;
      BYTE DbcsText[3];
      DbcsText[0]=LeadByte;
      DbcsText[1]=AscCode;
      DbcsText[2]=0;
      if (MultiByteToWideChar(GetACP(),MB_PRECOMPOSED,DbcsText,2,&UChar,1)==1) {
          CrackUnicode(w,UChar,&UcBase,&AscCode);
          LeadByte=0;
      } 
   } 
   else if (True(TerFlags6&TFLAG6_XLATE_UNICODE) && KeyDownUniChar>255) {  // use the unicode character derived on the keydown message
      CrackUnicode(w,KeyDownUniChar,&UcBase,&AscCode);
   }
    
   // discard control characters
   if (lstrchr(BreakChars,AscCode)!=NULL) return TRUE;

   // check for the escape character
   if (AscCode==ESCAPE) {
      MessageBeep(0);
      return FALSE;
   }

   AdjustInputPos(w,AscCode,LeadByte,UcBase);     // adjust the input position

   // restrict editing in page header and footer
   if ( (TerArg.PageMode && !EditPageHdrFtr && PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR)
      || LineFlags(CurLine)&LFLAG_HDRS_FTRS){
      MessageBeep(0);
      return FALSE;
   }

   CurInsertMode=InsertMode;               // insert mode applicable for this function
   if (!CurInsertMode && LeadByte==0 && (AscCode==ParaChar || AscCode==LINE_CHAR || AscCode==PAGE_CHAR || AscCode==TAB)) CurInsertMode=true;

   // check if overwriting a protected character
   if (!CurInsertMode && LineLen(CurLine)>0) {
      // check the character protection
      if (cfmt(CurLine).info.type==UNIFORM) CurFmt=cfmt(CurLine).info.fmt;
      else {
        fmt=OpenCfmt(w,CurLine);            // open the format data
        if (CurCol<LineLen(CurLine)) CurFmt=fmt[CurCol];
        else                         CurFmt=DEFAULT_CFMT;
        CloseCfmt(w,CurLine);               // close the format data
      }
      if (TerFont[CurFmt].style&PROTECT) {       // protect text
         MessageBeep(0);
         return FALSE;
      }
      // check if overwriting a paragraph marker character
      ptr=pText(CurLine);
      if (CurCol<LineLen(CurLine) && (ptr[CurCol]==ParaChar || ptr[CurCol]==CELL_CHAR)) CurInsertMode=TRUE;

   }


   // Find the font to apply to the new character
   if (InputFontId>=0 && TerFont[InputFontId].FieldId==0) {
      int CurFont=GetCurCfmt(w,CurLine,CurCol);
      int PrevFont=GetPrevCfmt(w,CurLine,CurCol);
      int CurFldId=TerFont[CurFont].FieldId;
      int PrevFldId=TerFont[PrevFont].FieldId;
      BOOL IsCurFld=(CurFldId==FIELD_NAME || CurFldId==FIELD_DATA);
      BOOL IsPrevFld=(PrevFldId==FIELD_NAME || PrevFldId==FIELD_DATA);
      if (IsCurFld && IsPrevFld) InputFontId=-1;  // can't break a field
   } 

   if (InputFontId>=0) NewFmt=InputFontId;
   else {
      SaveHilightType=HilightType;
      HilightType=HILIGHT_OFF;                 // disable hilight type checking in the following fuction
      NewFmt=GetEffectiveCfmt(w);
      HilightType=SaveHilightType;
   }

   // test for max input field length
   if (CurInsertMode && TerFont[NewFmt].FieldId==FIELD_TEXTBOX && !CanInsertInInputField(w,NewFmt,CurLine,CurCol)) {
       MessageBeep(0);
       return false;
   } 
   
   // test for protected and footnote text
   ProtectedDataField=True(TerFlags5&TFLAG5_PROTECT_DATA_FIELD) && (TerFont[NewFmt].FieldId==FIELD_NAME || TerFont[NewFmt].FieldId==FIELD_DATA);
   
   if (   TerFont[NewFmt].style&PROTECT                       // protected text
      || (InFootnote && !(TerFont[NewFmt].style&FNOTETEXT))   // footnote marker
      || ProtectedDataField ){
      NewFmt=GetCurCfmt(w,CurLine,CurCol);    // try the current character now
      if (ProtectedDataField || TerFont[NewFmt].style&(PROTECT|FNOTE|FNOTEREST)) {
         MessageBeep(0);
         return FALSE;
      }
   }

   // check if position on a line or rectangle drawing object
   if (fid(CurLine)>0) {
     int ParaFID=fid(CurLine);
     if (True(ParaFrame[ParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT))) {
        MessageBeep(0);
        return FALSE;
     }
   } 

   // test for typing on a break line
   if ( (LineFlags(CurLine)&LFLAG_BREAK && !(LineFlags(CurLine)&(LFLAG_PARA|LFLAG_SECT)) && AscCode!=ParaChar)
     || LineFlags(CurLine)&LFLAG_HTML_RULE) {
      MessageBeep(0);
      return FALSE;
   }

   if (TerFont[NewFmt].style&PICT) NewFmt=DEFAULT_CFMT;
   if (ImeEnabled && !(TerFont[NewFmt].TempStyle&TSTYLE_IME) && UcBase==0) NewFmt=GetNewTempStyle(w,NewFmt,TSTYLE_IME,(DWORD)TRUE,CurLine,CurCol);

   // check text angle
   TextAngle=fid(CurLine)?ParaFrame[fid(CurLine)].TextAngle:0;
   if (TextAngle==0) TextAngle=cid(CurLine)?cell[cid(CurLine)].TextAngle:0;
   if (TerFont[NewFmt].TextAngle!=TextAngle) NewFmt=SetFontTextAngle(w,NewFmt,TextAngle);
   
   // set font for the current language
   NewFmt=SetCurLangFont(w,NewFmt);   // set font for the currently selected language

   // set font for tracking
   if (TrackChanges) NewFmt=SetTrackingFont(w,NewFmt,TRACK_INS);
   else if (True(TerFont[NewFmt].InsRev) || True(TerFont[NewFmt].DelRev)) NewFmt=SetTrackingFont(w,NewFmt,TRACK_NONE);

   if (AscCode!=ParaChar && AscCode!=LINE_CHAR) TerOpFlags|=TOFLAG_INSERT_CHAR;             // inserted a character
   if (TerFlags3&TFLAG3_DATA_FIELD_INPUT) TerOpFlags|=TOFLAG_NO_FIELD_ADJ;

   // Erase any previously highlighted text
   if (HilightType==HILIGHT_CHAR) {
      DWORD SaveTerFlags=TerFlags;

      if (IsProtected(w,TRUE,TRUE)) return FALSE;   // can not delete this block

      TerFlags|=TFLAG_NO_AUTO_FULL_CELL_SEL;
      TerOpFlags|=TOFLAG_SEL_CELL_CONTENT;
      TerDeleteBlock(hTerWnd,FALSE);
      TerFlags=SaveTerFlags;
      ResetLongFlag(TerOpFlags,TOFLAG_SEL_CELL_CONTENT);

      CurLine=HilightBegRow;
      CurCol=HilightBegCol;
      HilightType=HILIGHT_OFF;
      PaintFlag=PAINT_WIN;
      BlockDeleted=TRUE;
   }

   // process the current character
   SaveLen=LineLen(CurLine);             // store line length
   if (CurInsertMode && CurCol<LineLen(CurLine)) {
      if (LineLen(CurLine)>=LineWidth) {
          if (AscCode!=ParaChar || LineLen(CurLine)>LineWidth) return FALSE;
      }

      MoveLineData(w,CurLine,CurCol,1,'B');  // create space before the current line

      ptr=pText(CurLine);
      fmt=OpenCfmt(w,CurLine);
      if (mbcs) lead=OpenLead(w,CurLine);
   }
   else {
      int SaveLen=LineLen(CurLine);
      if (IsProtectedChar(w,CurLine,CurCol)) {  // protect text
         MessageBeep(0);
         return FALSE;
      }
      if ((CurCol+1)>LineLen(CurLine)) LineAlloc(w,CurLine,LineLen(CurLine),CurCol+1);
      ptr=pText(CurLine);
      fmt=OpenCfmt(w,CurLine);
      if (mbcs) lead=OpenLead(w,CurLine);              
      if (LineLen(CurLine)>SaveLen) {
         WORD InitFmt;
         if (SaveLen==0) InitFmt=0;
         else            InitFmt=fmt[SaveLen-1];
         for (i=SaveLen;i<LineLen(CurLine);i++) fmt[i]=InitFmt;  // initialize any added length
      }
   }

   // get the height of the largest font
   for (i=0;i<LineLen(CurLine);i++) if (TerFont[fmt[i]].height>LineFontHeight) LineFontHeight=TerFont[fmt[i]].height;

   for (i=SaveLen;i<=CurCol;i++) ptr[i]=' ';    // move spaces in unused area

   CurFmt=DEFAULT_CFMT;
   if (SaveLen>0) CurFmt=fmt[SaveLen-1];
   if (TerFont[CurFmt].style&(PICT|PROTECT))   CurFmt=DEFAULT_CFMT;

   for (i=SaveLen;i<=CurCol;i++) fmt[i]=CurFmt;


   // save for undo
   if (AscCode==ParaChar || AscCode==LINE_CHAR) TerOpFlags|=TOFLAG_BREAK_CHAR;
   if (!ImeEnabled || !InsertMode) {
      SaveUndo(w,CurLine,CurCol,CurLine,CurCol,(BYTE)(CurInsertMode? 'I' : 'R'));
      fmt=OpenCfmt(w,CurLine);   // open again because SaveUndo can compress the font array
   }

   // assign new character
   ptr[CurCol]=AscCode;
   if (UcBase) fmt[CurCol]=NewFmt=SetUniFont(w,NewFmt,UcBase);
   else {
      if (TerFont[NewFmt].UcBase!=0) NewFmt=ResetUniFont(w,NewFmt);
      fmt[CurCol]=NewFmt;

   }
   if (!(LineFlags2(CurLine)&LFLAG2_RTL) && TerFont[NewFmt].rtl) FirstRtlChar=TRUE;
   
   if (mbcs) lead[CurCol]=LeadByte;

   // convert to unicode
   //LogPrintf("c: %c %x, cs: %d, ub: %d",AscCode,AscCode,TerFont[fmt[CurCol]].CharSet,TerFont[fmt[CurCol]].UcBase);
   if ((TerFlags5&TFLAG5_INPUT_TO_UNICODE) && TerFont[fmt[CurCol]].CharSet!=2 && TerFont[fmt[CurCol]].UcBase==0) {
      int CodePage,count;
      BYTE txt,mb[3];
      WORD UcBase,UText[3]; // unicode string
      BOOL convert=true;

      IsMbcsCharSet(w,TerFont[fmt[CurCol]].CharSet,&CodePage);  // get the code page
      if (mbcs && lead[CurCol]!=0) {
        mb[0]=lead[CurCol];
        mb[1]=ptr[CurCol];
        count=2;
      }
      else {
        if (!mbcs && TerFont[fmt[CurCol]].CharSet<2 && (ptr[CurCol]<0x80 || ptr[CurCol]>0xA0)) convert=false;   // These ansi characters mapped directly to identical unicode values
        mb[0]=ptr[CurCol];
        count=1;
      }  
      //LogPrintf("   cp: %d, count: %d",CodePage,count);
      if (convert && MultiByteToWideChar(CodePage,0,mb,count,UText,1)==1) {
         CrackUnicode(w,UText[0],&UcBase,&txt);
         fmt[CurCol]=SetUniFont(w,fmt[CurCol],UcBase);
         ptr[CurCol]=txt;
         if (mbcs) lead[CurCol]=0;
      } 

   } 

   CloseCfmt(w,CurLine);
   if (mbcs) CloseLead(w,CurLine);

   CellLineAfterSubTable=(CurLine>0 && LineLen(CurLine)==2 && cid(CurLine) && LineInfo(w,CurLine,INFO_CELL) && TableLevel(w,CurLine-1)>TableLevel(w,CurLine));  // text being added to the last blank line after a subtable
   
   if (   AscCode!=ParaChar && AscCode!=LINE_CHAR && AscCode!=TAB
          && AscCode!=NBSPACE_CHAR && AscCode!=NBDASH_CHAR && !BlockDeleted
       && InputFontId==-1 && TerFont[NewFmt].height<=LineFontHeight 
       && !CellLineAfterSubTable && !FirstRtlChar && !(LineFlags2(CurLine)&LFLAG2_RTL)
       && KeyDownUniChar==0
       && !(TerFont[NewFmt].style&(FNOTETEXT|ENOTETEXT|SCAPS))
       ) {
      WrapFlag=WRAP_MIN;
      PaintFlag=PAINT_LINE;                       // only paint after the current column onward
   }

   if (CurCol < LineWidth-1 || (CurCol+1)<LineLen(CurLine)) CurCol++;

   if (CurCol>=LineLen(CurLine) && TerArg.WordWrap && (CurLine+1)<TotalLines) {
      CurLine++;                               // advance to the next line
      CurRow++;
      CurCol=0;
      PaintFlag=PAINT_WIN;
      WrapFlag=WRAP_WIN;
   }

   FrameClicked=FALSE;                         // turnoff frame sizing tabs
   
   ResetLongFlag(LineFlags2(CurLine),LFLAG2_SPELL_CHECKED);  // need to respell check this line
   EditLine=CurLine;                           // positioned being edited
   EditCol=CurCol;

   if (LineFlags(CurLine)&LFLAG_PICT) TerOpFlags2|=TOFLAG2_REPOS_PICT_FRAMES;

   PaintTer(w);

   TerArg.modified++;
   InputFontId=-1;                             // reset the input font id
   EnterHit=false;                             // another key hit
   ResetUintFlag(TerOpFlags,TOFLAG_INSERT_CHAR|TOFLAG_NO_RESET_FONT|TOFLAG_NO_FIELD_ADJ|TOFLAG_BREAK_CHAR);
   if (EditEndnoteText && TerFont[NewFmt].style&ENOTETEXT) TerOpFlags|=TOFLAG_UPDATE_ENOTE;  // update endnote display
   
   if (TerOpFlags&TOFLAG_UPDATE_ENOTE && CurLine<RepageBeginLine) RepageBeginLine=CurLine; // RequestPagination(w,TRUE);  // update endnote display
   KeyDownUniChar=0;
  
   // do auto complete
   if (TotalAutoComps>0) DoAutoComp(w,AscCode,LeadByte,UcBase);

   // set next style
   if (AscCode==ParaChar) SetNextStyle(w);

   return TRUE;
}

/******************************************************************************
    AdjustInputPos:
    Do any adjustment to input position.
*******************************************************************************/
BOOL AdjustInputPos(PTERWND w, BYTE CurChar, BYTE LeadByte, WORD UcBase)
{
    int CurFont,PrevFont,i;
    LPWORD fmt;

    if (LeadByte!=0 || UcBase!=0) goto CHECK_HLINK;

    // when cursor is at the first field-data char, move it at the field-name position
    if (CurChar!=' ') goto CHECK_HLINK;   // do it only for the space input
    if (ShowFieldNames) goto CHECK_HLINK;  // do it only when the field data is showing
      
    if (CurCol==0) goto CHECK_HLINK;
      
    CurFont=GetCurCfmt(w,CurLine,CurCol);
    if (TerFont[CurFont].FieldId!=FIELD_DATA) goto CHECK_HLINK;

    PrevFont=GetCurCfmt(w,CurLine,CurCol-1);
    if (TerFont[PrevFont].FieldId!=FIELD_NAME) goto CHECK_HLINK;

    fmt=OpenCfmt(w,CurLine);
    for (i=CurCol-1;i>=0;i--) if (TerFont[fmt[i]].FieldId!=FIELD_NAME) break;
    CloseCfmt(w,CurLine);

    CurCol=i+1;    // new column position

    if (InputFontId>=0) InputFontId=SetFontFieldId(w,InputFontId,0,"");
    else                InputFontId=SetFontFieldId(w,CurFont,0,"");

    if (True(TerFont[InputFontId].style&PROTECT)) 
       InputFontId=SetFontStyle(w,InputFontId,PROTECT,false);

    CursDirection=CURS_FORWARD;
   
    CHECK_HLINK:      // if on the first character of a hyperlink

    if (CurCol==0 && InputFontId>0 && TerFont[InputFontId].FieldId==FIELD_HLINK) {
      if (CurLine==0) InputFontId=0;
      else {
         int PrevFont=GetPrevCfmt(w,CurLine,CurCol);
         if (TerFont[PrevFont].FieldId!=FIELD_HLINK) InputFontId=PrevFont;
      } 
    } 

    //NEXT_CHECK:

    return true;
} 


/******************************************************************************
    DoAutoComp:
    Do auto-complete word/phrase replacement.
*******************************************************************************/
BOOL DoAutoComp(PTERWND w, BYTE CurChar, BYTE LeadByte, WORD UcBase)
{
    int i,col,OrgCol,WordLen,GrpUndoRef;
    long line,OrgLine;
    LPBYTE ptr=null;
    LPBYTE AcWord;

    if (!InAutoComp) {
        AutoCompPos=RowColToAbs(w,CurLine,CurCol);
        if (CurChar>=32) AutoCompPos--;  // include the character just typed in the word being entered
        InAutoComp=true;
        return true;
    }
       
    if (CurCol==0) goto END;     // start new
    if (LeadByte || UcBase) goto END;

    if (CurChar!=' ' && CurChar!=',') return true;  // delimter not found


    AbsToRowCol(w,AutoCompPos,&line,&col);

    if (line!=CurLine) goto END;

    // go past the initial delimiters
    ptr=pText(line);

    for (i=col;i<CurCol;i++) {
       if (ptr[i]!=' ' && ptr[i]!=',' && ptr[i]!=';' && ptr[i]>=32) break;
    }
    if (i==CurCol) goto END;
    col=i; 

    // ensure that the preceeding character is a delimter
    if (col>0) {
      char chr=ptr[col-1];
      if (chr!=' ' && chr!=',' && chr!=';' && chr!=TAB) goto END;
    } 

    // find the end of the string
    for (i=col;i<CurCol;i++) {
       if (ptr[i]==' ' || ptr[i]==',' || ptr[i]==';' || ptr[i]<32) break;
    }
    if (col==i) goto END;  // zero length word

    WordLen=i-col;
    AcWord=&(ptr[col]);

    // check if this word included in the auto complete list
    for (i=0;i<TotalAutoComps;i++) {
       if (strncmp(AcWord,AutoCompWord[i],WordLen)==0) break;
    }
    if (i==TotalAutoComps) goto END;  // word not found in the list

    // do the replacement
    OrgLine=line;
    OrgCol=col;
    GrpUndoRef=UndoRef;
    SaveUndo(w,OrgLine,OrgCol,OrgLine,OrgCol+WordLen-1,'D');

    ReplaceTextInPlace(w,&line,&col,WordLen,AutoCompPhrase[i],NULL);
    
    UndoRef=GrpUndoRef;
    SaveUndo(w,OrgLine,OrgCol,OrgLine,OrgCol+strlen(AutoCompPhrase[i])-1,'I');

    CurCol+=(lstrlen(AutoCompPhrase[i])-WordLen);

    END:
    AutoCompPos=RowColToAbs(w,CurLine,CurCol);  // next word start

    return true;
} 


/******************************************************************************
    TerReturn:
    Process a carriage return
******************************************************************************/
TerReturn(PTERWND w)
{
    int i,CurPara,blt,NewCol,row,CurCell;
    LPBYTE ptr;
    BOOL ShiftPressed=GetKeyState(VK_SHIFT)&0x8000;
    BOOL HtmlTableBegins;

    // check for the line limit
    if (TerArg.LineLimit>0 && TotalLines>=TerArg.LineLimit) return PrintError(w,MSG_MAX_LINES_EXCEEDED,"Carriage Return");

    // word wrap mode
    if (TerArg.WordWrap) {
        // on a blank list line?
        if (LineLen(CurLine)==1 && True(LineFlags(CurLine)&LFLAG_PARA_FIRST)) {
           CurPara=pfmt(CurLine);
           blt=PfmtId[CurPara].BltId;
           if (False(TerFlags6&TFLAG6_DONT_PROCESS_BULLET_KEYS) && blt!=0 && TerBlt[blt].ls!=0) TerSetListBullet(hTerWnd, false,0,0,0,"","",false);  // turn-off list
        }
        else if (CurLine>0 && LineLen(CurLine-1)==1 && True(LineFlags(CurLine-1)&LFLAG_PARA_FIRST) 
                && False(LineFlags(CurLine-1)&LFLAG_BREAK)) {
           CurPara=pfmt(CurLine-1);
           blt=PfmtId[CurPara].BltId;
           if (False(TerFlags6&TFLAG6_DONT_PROCESS_BULLET_KEYS) && blt!=0 && TerBlt[blt].ls!=0) {
              CurLine-=1;  // position at the previous paragraph
              TerSetListBullet(hTerWnd, false,0,0,0,"","",false);  // turn-off list
           }
        } 
     
             
        // insert a line when the first table is on the top of the text section
        if (CurCol==0 && cid(CurLine)>0 && !ShiftPressed && !(TerFlags3&TFLAG3_CURSOR_IN_CELL)) {
           CurCell=cid(CurLine);
           row=cell[CurCell].row;
           HtmlTableBegins=(HtmlMode && cell[CurCell].PrevCell<=0 && TableRow[row].flags&ROWFLAG_NEW_TABLE);
           if (CurLine==0
              || (HtmlTableBegins && cid(CurLine-1)>0 && cid(CurLine-1)!=cid(CurLine))  // user is trying to separate two html tables
              || LineFlags(CurLine-1)&LFLAG_HDRS_FTRS
              || (tabw(CurLine-1) && tabw(CurLine-1)->type&(INFO_SECT|INFO_PAGE|INFO_COL) && cid(CurLine-1)==0)) {
              InsertMarkerLine(w,CurLine,ParaChar,0,pfmt(CurLine),0,0);
              PaintTer(w);
              return TRUE;
           }
        }
        if (ShiftPressed) TerAscii(w,LINE_CHAR,0);  // enter the line break
        else              TerAscii(w,ParaChar,0);   // now insert it as an asc character
        EnterHit=true;                              // Enter key hit

        return TRUE;
    }

    NewCol=0;
    if (LineLen(CurLine)>0) {  // determine the tab position for the next line
       ptr=pText(CurLine);
       for (i=0;i<LineLen(CurLine);i++) if (ptr[i]!=' ') break;
       if (ptr[i]!=' ') NewCol=i;
       if (CurCol<NewCol) NewCol=CurCol;

    }

    if (CrSplitLine) {
       TerSplitLine(w,NewCol,TRUE,TRUE);
       return TRUE;
    }
    if (CrNewLine && LineLen(CurLine)<=CurCol) {
       TerSplitLine(w,NewCol,TRUE,TRUE);
       return TRUE;
    }
    if ((CurLine+1)>=TotalLines) return TRUE;

    CurCol=0;CurRow++;CurLine++;

    // erase any highlighting
    if (HilightType!=HILIGHT_OFF) {
       HilightType=HILIGHT_OFF;
       PaintFlag=PAINT_WIN;
    }
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerPageBreak:
    Insert a hard page break before the current line.
******************************************************************************/
BOOL WINAPI _export TerPageBreak(HWND hWnd,BOOL repaint)
{
   PTERWND w;
   long FirstLine;
   int  CurFont;
   BOOL LineSplit=FALSE,TableSplit=FALSE;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
   ReleaseUndo(w);     // release any existing undos

   // not valid if positioned in a table or header/footer
   if (fid(CurLine)>0) return FALSE;
   if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR) return FALSE;
   
   if (cid(CurLine)>0) {  // insert before the current row
      int cl,row1,row2;
      long l;
      for (l=CurLine-1;l>=0;l--) {
         if (cid(l)==0) break;
         cl=cid(l);
         if (cell[cl].level>0) continue;
         if (LineInfo(w,l,INFO_ROW)) break;   // row-break at the top level
      }
      if (l>=0 && LinePage(l)!=LinePage(CurLine)) return FALSE;
      CurLine=l+1;    // line before which the page break will be created
      CurCol=0;

      // break this table row chain
      if (l>=0) {
        row1=cell[cid(l)].row;   // this will be the last table row on the current page
        if (row1>0) {
          row2=TableRow[row1].NextRow;  // this will be first table row on the next page
          TableRow[row1].NextRow=-1;
          if (row2>0) TableRow[row2].PrevRow=-1;
        }
      }
      TableSplit=true;
   }

   // split the current line if not at the beginning of the line
   if (CurCol>0 && SplitLine(w,CurLine,CurCol,0)) {
       CurLine++;
       CurCol=0;
       LineSplit=TRUE;
   }

   CurFont=GetEffectiveCfmt(w);

   // ensure that the line before is delimited with a parachar
   FirstLine=CurLine;
   if (CurLine==0 || !(LineFlags(CurLine-1)&LFLAG_PARA) || LineSplit) {
      if (!(TerOpFlags&TOFLAG_MERGE_PRINT_RECT) && !TableSplit) {    // TerMergeprint - To handle a temporary page break within a numbered list 
         InsertMarkerLine(w,CurLine,ParaChar,CurFont,pfmt(CurLine),0,0);
         CurLine++;
      }
   }

   InsertMarkerLine(w,CurLine,PAGE_CHAR,CurFont,-1,0,-1);
   cid(CurLine)=0;                       // cid can be non-zero when inserting a page break inside a table

   if (TableSplit) {    // Word needs \par needed after \page when spliting a table, otherwise \page gets sucked into the next row
      CurLine++;
      InsertMarkerLine(w,CurLine,ParaChar,CurFont,0,0,0);
   }

   // record for undo
   SaveUndo(w,FirstLine,0,CurLine,0,'I');

   CurLine++;                            // increment the current line number

   if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
      BeginLine=CurLine-WinHeight/2;  // position the current line at center
      if (BeginLine<0) BeginLine=0;
   }
   CurRow=CurLine-BeginLine;

   HilightType=HILIGHT_OFF;              // turnoff any hilighting
   TerArg.modified++;                   // indicate that file has been modified
   if (repaint){
      Repaginate(w,FALSE,TRUE,0,TRUE);
      PaintTer(w); // Force a screen paint
   }

   return TRUE;
}

/******************************************************************************
    TerColBreak:
    Insert a hard column break before the current line.
******************************************************************************/
BOOL WINAPI _export TerColBreak(HWND hWnd,BOOL repaint)
{
   PTERWND w;
   LPBYTE ptr;
   LPWORD fmt;

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
   ReleaseUndo(w);     // release any existing undos

   // not valid if positioned in a table or header/footer
   if (cid(CurLine)>0) return FALSE;
   if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR) return FALSE;

   MoveLineArrays(w,CurLine,1,'B');        // create a line before Current Line

   // insert the column break character
   LineAlloc(w,CurLine,0,1);             // allocate 1 byte
   ptr=pText(CurLine);
   fmt=OpenCfmt(w,CurLine);
   ptr[0]=COL_CHAR;                     // insert column break character
   fmt[0]=DEFAULT_CFMT;

   CloseCfmt(w,CurLine);

   // record for undo
   SaveUndo(w,CurLine,0,CurLine,0,'I');

   CurLine++;                            // increment the current line number

   if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
      BeginLine=CurLine-WinHeight/2;  // position the current line at center
      if (BeginLine<0) BeginLine=0;
   }
   CurRow=CurLine-BeginLine;

   HilightType=HILIGHT_OFF;              // turnoff any hilighting
   TerArg.modified++;                   // indicate that file has been modified
   if (repaint) PaintTer(w);

   return TRUE;
}

/******************************************************************************
    TerSectBreak:
    Insert a section break before the current line.
******************************************************************************/
BOOL WINAPI _export TerSectBreak(HWND hWnd,BOOL repaint)
{
   PTERWND w;
   LPBYTE ptr;
   LPWORD fmt;
   int NewSect,CurSect,PrevSect;
   BYTE string[20];

   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

   if (!CheckLineLimit(w,TotalLines+1)) return TRUE;
   ReleaseUndo(w);     // release any existing undos

   // not valid if positioned in a table or header/footer
   if (cid(CurLine)>0) return FALSE;
   if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR) return FALSE;
   if (EditPageHdrFtr) return FALSE;

   // can a break character inserted at this position
   if (!CanInsertBreakChar(w,CurLine,CurCol)) return FALSE;

   // split the line if the cursor is not in the first column
   if (CurCol>0) TerSplitLine(w,0,FALSE,FALSE);

   // insert the page break character
   MoveLineArrays(w,CurLine,1,'B');   // create a line before Current Line
   CurSect=GetSection(w,CurLine);        // find the current section

   // find an unused section slot and initialize
   for (NewSect=0;NewSect<TotalSects;NewSect++) if (!TerSect[NewSect].InUse) break;
   if (NewSect==TotalSects) {
      if (TotalSects>=MaxSects) {
         ExpandSectArray(w,-1);
         if (TotalSects>=MaxSects) return PrintError(w,MSG_SECTION_FULL,"TerSectBreak");
      }
      TotalSects++;
   }

   FarMove(&TerSect[CurSect],&TerSect[NewSect],sizeof(struct StrSect)); //Initialize from the current section
   FarMove(&TerSect1[CurSect],&TerSect1[NewSect],sizeof(struct StrSect1)); //Initialize from the current section


   TerSect[NewSect].LastLine=CurLine;    // End the new section here
   TerSect[CurSect].FirstLine=CurLine+1; // Begin the current section here
   TerSect1[CurSect].hdr.FirstLine=-1;    // page header line
   TerSect1[CurSect].ftr.FirstLine=-1;    // page footer line
   TerSect1[CurSect].PrevSect=NewSect;   // section before this section

   PrevSect=TerSect1[NewSect].PrevSect;  // section before the current section, order -> PrevSect,newSect,curSect
   if (PrevSect>=0) TerSect1[PrevSect].NextSect=NewSect;  

   LineAlloc(w,CurLine,0,1);             // allocate 1 byte
   ptr=pText(CurLine);
   fmt=OpenCfmt(w,CurLine);
   ptr[0]=SECT_CHAR;                     // insert section break character
   fmt[0]=DEFAULT_CFMT;

   wsprintf(string,"Section%d",NewSect);
   SetTag(w,CurLine,0,TERTAG_SECT,string,NULL,NewSect);   // insert the section tag

   // record for undo
   //SaveUndo(w,CurLine,0,CurLine,0,'I');

   // assign the para id to the break line
   if (CurLine>0 && !(LineFlags(CurLine-1)&LFLAG_PARA))
        pfmt(CurLine)=pfmt(CurLine-1);
   else pfmt(CurLine)=0;
   fid(CurLine)=0;

   // attach the section info structure to the current line
   if (AllocTabw(w,CurLine)) {
       tabw(CurLine)->type=INFO_SECT;
       tabw(CurLine)->section=NewSect;
   }


   CloseCfmt(w,CurLine);                // call this function only after initializing the tabw structure

   CurLine++;                           // position in the new section
   CurCol=0;
   if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
      BeginLine=CurLine-WinHeight/2;  // position the current line at center
      if (BeginLine<0) BeginLine=0;
   }
   CurRow=CurLine-BeginLine;

   HilightType=HILIGHT_OFF;              // turnoff any hilighting
   TerArg.modified++;                    // indicate that file has been modified
   SectModified=TRUE;                    // recalculate section boundaries
   if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;

   ReleaseUndo(w);
   if (repaint) PaintTer(w);

   return TRUE;
}

/******************************************************************************
    TerBackSpace:
    Process the backspace character
*******************************************************************************/
TerBackSpace(PTERWND w)
{
    int CurCfmt,SaveCurCol;
    long SaveCurLine,l;
    BYTE DelChar;

    if (HilightType!=HILIGHT_OFF) {
       DWORD SaveTerFlags=TerFlags;
       TerFlags|=TFLAG_NO_AUTO_FULL_CELL_SEL;
       TerOpFlags2|=TOFLAG2_DEL_CELL_TEXT;
       
       TerDeleteBlock(hTerWnd,TRUE);     // delete the block
       
       ResetLongFlag(TerOpFlags2,TOFLAG2_DEL_CELL_TEXT);
       TerFlags=SaveTerFlags;
       return TRUE;
    }

    // save the line and column
    SaveCurLine=CurLine;
    SaveCurCol=CurCol;

    BACKSPACE:

    if (CurCol==0) {
        if (CurLine==0) return TRUE;
        if (cid(CurLine) && !cid(CurLine-1)) return TRUE;  // don't let the line before to get inside the table

        if (TerArg.WordWrap) {CurLine--;CurCol=LineLen(CurLine);}
        else if (JoinLines) {       // join with previous line
           CurLine--;CurRow--;CurCol=LineLen(CurLine);
           TerJoinLine(w);
           return TRUE;
        }
        else return TRUE;
    }


    if (CurCol>LineLen(CurLine)) {
       CurCol--;
       PaintFlag=PAINT_MIN;        // minimum painting required
       PaintTer(w);
       return TRUE;
    }


    // delete the previous character
    if (LineLen(CurLine)>0) {
       // check if character protected
       if (IsProtectedChar(w,CurLine,CurCol-1)) {
          CurCfmt=GetCurCfmt(w,CurLine,CurCol-1);
          
          // check if a picture frame being deleted
          if (True(TerFont[CurCfmt].style&PICT) && TerFont[CurCfmt].ParaFID!=0 && !ShowParaMark) {
             CurCol--;
             goto BACKSPACE;
          } 

          if (TerFont[CurCfmt].FieldId==FIELD_NAME && HiddenText(w,CurCfmt)) {
             CurCol--;
             CursDirection=CURS_BACKWARD;
             AdjustHiddenPos(w);
             PrevCursLine=CurLine;  // record current line number
             PrevCursCol=CurCol;    // record previous column number
             CursDirection=CURS_BACKWARD;
             return TerDel(w);
          } 
          
          CurLine=SaveCurLine;
          CurCol=SaveCurCol;
          MessageBeep(0);
          return FALSE;
       }

       // handle the last link character
       if (IsLoneHypertextChar(w,CurLine,CurCol-1))
          return TerDeleteHypertext(hTerWnd,CurLine,CurCol-1,TRUE);

       // special treatment for hidden character
       CurCfmt=GetCurCfmt(w,CurLine,CurCol-1);
       if (HiddenText(w,CurCfmt)) {
           CurCol--;
           CursDirection=CURS_BACKWARD;
           AdjustHiddenPos(w);
           PrevCursLine=CurLine;  // record current line number
           PrevCursCol=CurCol;    // record previous column number
           CursDirection=CURS_FORWARD;
           return TerDel(w);
       }
       
       if (TrackChanges && TrackDel(w,CurLine,CurCol-1,false)) goto END;

       TransferTags(w,CurLine,CurCol-1);   // transfer the tags to the following character before deletion

       // treatment for regular characters
       InputFontId=CurCfmt;   // so it can be applied to a next character at this position
       DelChar=GetCurChar(w,CurLine,CurCol-1);

       SaveUndo(w,CurLine,CurCol-1,CurLine,CurCol-1,'D');

       MoveLineData(w,CurLine,CurCol-1,1,'D'); // delete the previous character

       // get paragraph properties from the previous para
       if (DelChar==ParaChar && LineLen(CurLine)>0 && CurCol>LineLen(CurLine)) { // when the para char was the last char and is being deleted
          for (l=CurLine+1;l<TotalLines;l++) {
             pfmt(l)=pfmt(CurLine);
             if (LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK)) break;
          } 
       } 
    }

    END:

    CurCol--;


    // determine the scope of painting
    if (LineFlags(CurLine)&LFLAG_PARA && CurCol<(LineLen(CurLine)-1) && !CursorOnFirstWord(w))
         WrapFlag=WRAP_MIN;              // minimum wrapping
    else WrapFlag=WRAP_PARA;             // need to re-wrap just the current para
        
    if (LineFlags(CurLine)&LFLAG_FNOTETEXT || LineFlags2(CurLine)&LFLAG2_ENOTETEXT) {
       WrapFlag=WRAP_PARA;
       TerOpFlags|=TOFLAG_UPDATE_ENOTE;  // update endnote display
    }

    if (WrapFlag==WRAP_MIN) PaintFlag=PAINT_LINE; // refresh the current line
    else                    PaintFlag=PAINT_WIN;  // refresh the screen

    FrameClicked=FALSE;             // disable highlighting of a frame
     

    PaintTer(w);


    return TRUE;
}


/******************************************************************************
    TerDel:
    delete a character
******************************************************************************/
TerDel(PTERWND w)
{
    long l;
    BOOL FNoteTextDel=FALSE;

    if (HilightType!=HILIGHT_OFF) {    // delete the highlight block if any
       DWORD SaveTerFlags=TerFlags;
       TerFlags|=TFLAG_NO_AUTO_FULL_CELL_SEL;
       TerOpFlags2|=TOFLAG2_DEL_CELL_TEXT;
       
       TerDeleteBlock(hTerWnd,TRUE);     // delete the block
       
       ResetLongFlag(TerOpFlags2,TOFLAG2_DEL_CELL_TEXT);
       TerFlags=SaveTerFlags;
       return TRUE;
    }

    if (FrameClicked) return DeleteFrame(w);

    if (CurCol>=LineLen(CurLine) && !TerArg.WordWrap) {
       if (JoinLines) {TerJoinLine(w);return TRUE;}
       else return TRUE;
    }

    if (TerArg.WordWrap && CurCol+1>LineLen(CurLine)) return TRUE;

    // check for protected text
    if (HtmlMode && !ShowHiddenText) PosAfterHiddenText(w);  // may be positioned on the first hidden character
    if (IsProtectedChar(w,CurLine,CurCol)) {
       MessageBeep(0);
       return TRUE;
    }

    if (TerArg.WordWrap && CurLine==(TotalLines-1) && CurCol==(LineLen(CurLine)-1)) return TRUE;  // can't delete the last character of the file

    if (TrackChanges && TrackDel(w,CurLine,CurCol,true)) goto END;

    // handle the last link character
    if (IsLoneHypertextChar(w,CurLine,CurCol))
       return TerDeleteHypertext(hTerWnd,CurLine,CurCol,TRUE);

    if (LineLen(CurLine)>0) {
        BYTE DelChar=GetCurChar(w,CurLine,CurCol);

        if (LineFlags(CurLine)&LFLAG_FNOTETEXT || LineFlags2(CurLine)&LFLAG2_ENOTETEXT) {  // check if footnote text being deleted - requires full repaint
           int  DelFont=GetCurCfmt(w,CurLine,CurCol);
           if (TerFont[DelFont].style&(FNOTETEXT|ENOTETEXT)) FNoteTextDel=TRUE;
           TerOpFlags|=TOFLAG_UPDATE_ENOTE;  // update endnote display
        }
         
        TransferTags(w,CurLine,CurCol);   // transfer the tags to the following character before deletion

        SaveUndo(w,CurLine,CurCol,CurLine,CurCol,'D'); //save the deleted text before deleting
        MoveLineData(w,CurLine,CurCol,1,'D');          // delete the character

        // get paragraph properties from the previous para
        if (DelChar==ParaChar && LineLen(CurLine)>0 && CurCol==LineLen(CurLine)) {  // when the para char was the last character and is being deleted
           for (l=CurLine+1;l<TotalLines;l++) {
              pfmt(l)=pfmt(CurLine);
              if (LineFlags(l)&(LFLAG_PARA|LFLAG_BREAK)) break;
           } 
        } 
    }

    END:

    // determine the scope of painting
    if (LineFlags(CurLine)&LFLAG_PARA && CurCol<(LineLen(CurLine)-1) 
         && !CursorOnFirstWord(w) && !FNoteTextDel)
         WrapFlag=WRAP_MIN;              // minimum wrapping
    else WrapFlag=WRAP_PARA;             // need to re-wrap just the current para

    if (WrapFlag==WRAP_MIN) PaintFlag=PAINT_LINE; // refresh the current line
    else                    PaintFlag=PAINT_WIN;  // refresh the screen

    FrameClicked=FALSE;             // disable highlighting of a frame
    PaintTer(w);
    return TRUE;
}


/******************************************************************************
    CursorOnFirstWord:
    This function returns TRUE when the cursor is located on the first word of a line
*******************************************************************************/
BOOL CursorOnFirstWord(PTERWND w)
{
    int col;
    LPBYTE ptr=pText(CurLine);

    col=CurCol;
    if (col>=LineLen(CurLine)) col=LineLen(CurLine)-1;
    if (col<0) col=0;
    if (ptr[col]==' ') col--;

    while (col>=0) {
       if (ptr[col]==' ') return FALSE;
       col--;
    }

    return TRUE;
}

/******************************************************************************
    TerInsertTab:
    process the tab character
*******************************************************************************/
TerInsertTab(PTERWND w)
{
    int CurPara,blt,CurFont,SaveInputFontId;

    SaveInputFontId=InputFontId;          // save the input font id
    TerSetCharHilight(w);                 // turnoff any highlighting
    InputFontId=SaveInputFontId;          // restore the input font id

    if (TerArg.WordWrap) {
        // on the first character of a list?
        if (CurCol==0 && True(LineFlags(CurLine)&LFLAG_PARA_FIRST)) {
           CurPara=pfmt(CurLine);
           blt=PfmtId[CurPara].BltId;
           if (False(TerFlags6&TFLAG6_DONT_PROCESS_BULLET_KEYS) && blt!=0 && TerBlt[blt].ls!=0 && TerBlt[blt].lvl<8) {
              TerSetListLevel(hTerWnd,-1,1,true);  // increase by one level
              return true;
           }
        }

       // check if tab pressed on a text-input field
       CurFont=GetCurCfmt(w,CurLine,CurCol);
       if (TerArg.ReadOnly && TerFont[CurFont].FieldId!=FIELD_TEXTBOX) {
           int PrevFont=GetPrevCfmt(w,CurLine,CurCol);
           if (TerFont[PrevFont].FieldId==FIELD_TEXTBOX) {
              PrevTextPos(w,&CurLine,&CurCol);  // when the field is highlighted, the cursor is past the field, so bring it back 
              CurFont=GetCurCfmt(w,CurLine,CurCol);
           }
       }
       if (TerArg.ReadOnly && TerFont[CurFont].FieldId==FIELD_TEXTBOX) return TabOnControl(w,-1,false);

       // check if positioned on a cell
       if (TerArg.PrintView && cid(CurLine)>0 && CommandId!=ID_CTRL_TAB) return TerTabCell(w);

       if (TerArg.ReadOnly) return TRUE;
       
       if (HtmlMode) {      //simulate it as 4 spaces
          return InsertTerText(hTerWnd,"    ",TRUE);
       } 

       if (!tabw(CurLine)) {              // mark this line to contain tabs
          if (!AllocTabw(w,CurLine)) AbortTer(w,MsgString[MSG_OUT_OF_MEM],40);
       }
       if (!(tabw(CurLine)->type&INFO_TAB)) {
          tabw(CurLine)->type|=INFO_TAB;// specify tab also
          tabw(CurLine)->count=0;         // initialize number of tabs
       }
    }

    if (!TerArg.ReadOnly) return TerAscii(w,TAB,0);              // treat tab like an ordinary character
    else                  return TRUE;
}

/******************************************************************************
   TerInsertLineAft(w):
   Insert a line after the current line
*******************************************************************************/
TerInsertLineAft(PTERWND w)
{

   TerArg.modified++;                      // indicate that file has been modified

   if (!CheckLineLimit(w,TotalLines+1)) return TRUE;

   MoveLineArrays(w,CurLine,1,'A');        // create a line after Current Line

   if (CurLine<HilightBegRow) HilightBegRow++;
   if (CurLine<HilightEndRow) HilightEndRow++;

   PaintFlag=PAINT_WIN;                  // refresh from the text
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TerInsertLineBef:
   insert a line before the current line
*******************************************************************************/
TerInsertLineBef(PTERWND w)
{
   TerArg.modified++;                       // indicate that file has been modified
   if (!CheckLineLimit(w,TotalLines+1)) return TRUE;

   MoveLineArrays(w,CurLine,1,'B');        // create a line before Current Line

   CurLine++;                            // increment the current line number

   if ((CurLine-1)<HilightBegRow) HilightBegRow++;
   if ((CurLine-1)<HilightEndRow) HilightEndRow++;

   if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
      BeginLine=CurLine-WinHeight/2;  // position the current line at center
      if (BeginLine<0) BeginLine=0;
   }
   CurRow=CurLine-BeginLine;

   PaintFlag=PAINT_WIN;                   // refresh the entire screen
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
    TerDeleteLine:
    delete a line
*******************************************************************************/
TerDeleteLine(PTERWND w)
{
   if (TerArg.WordWrap && TotalLines==1) {  // delete the first line except the para marker
      if (LineLen(CurLine)<=1) return TRUE;
      HilightBegRow=HilightEndRow=CurLine;
      HilightBegCol=0;
      HilightEndCol=LineLen(CurLine)-1;    
      HilightType=HILIGHT_CHAR;
      return DeleteCharBlock(w,TRUE,TRUE);
   } 
   
   TerArg.modified++;                    // indicate that file has been modified
                                         //  save undo before deleting the line
   SaveUndo(w,CurLine,0,CurLine,LineLen(CurLine),'D');

   MoveLineArrays(w,CurLine,1,'D');      // delete the current line

   if (CurLine==TotalLines && CurLine>0) {CurLine--;CurRow--;CurCol=0;}

   if (CurLine>=HilightBegRow && CurLine<=HilightEndRow) HilightType=HILIGHT_OFF;
   if (CurLine>=HilightEndRow && CurLine<=HilightBegRow) HilightType=HILIGHT_OFF;
   if (CurLine<=HilightBegRow) HilightBegRow--;
   if (CurLine<=HilightEndRow) HilightEndRow--;
   if (HilightBegRow<0 || HilightEndRow<0) HilightType=HILIGHT_OFF;

   if (CurRow<0) {                                // stop auto scrolling
     CurRow=0;
     BeginLine=CurLine;
   }
   if (TerArg.WordWrap && CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
   if (CurCol<0) CurCol=0;

   PaintFlag=PAINT_PARTIAL_WIN;      // refresh from the current line onward
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
    TerSplitLine:
    Split the current line at the current column position and place it
    at the next line starting with 'StartCol' position.
*******************************************************************************/
TerSplitLine(PTERWND w,int StartCol, BOOL AlignTab, BOOL repaint)
{
    long l;
    LPBYTE ptr1;
    int j;

    if (!CheckLineLimit(w,TotalLines+1)) {
       return PrintError(w,MSG_MAX_LINES_EXCEEDED,"TerSplitLine");
    }

    SplitLine(w,CurLine,CurCol,StartCol);

    pfmt(CurLine+1)=pfmt(CurLine);          // set to the same paragraph

    if (CurLine<HilightBegRow) HilightBegRow++;
    if (CurLine<HilightEndRow) HilightEndRow++;


    // PaintScreen

    CurCol=0;
    if (AlignTab && LineLen(CurLine+1)==0) {       // find the first column for tab alignment
       l=CurLine;
       while(LineLen(l)==0 && l>0) l--;
       j=0;
       ptr1=pText(l);
       while(j<LineLen(l) && ptr1[j]==' ') j++;

       CurCol=j;
    }

    PaintFlag=PAINT_PARTIAL_WIN;        // refresh from current line onward
    CurLine++;CurRow++;
    if (CurRow>=WinHeight) {CurRow=WinHeight-1;PaintFlag=PAINT_WIN;}
    BeginLine=CurLine-CurRow;

    TerArg.modified++;                      // indicate that file has been modified

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    SplitLine:
    Split the current line at the current column position and place it
    at the next line with specified extrac space in the beginning of the line.
*******************************************************************************/
SplitLine(PTERWND w,long line, int col, int extra)
{
    int i;
    WORD CurFmt,ct=0;
    BYTE space=' ',lead=0;

    MoveLineArrays(w,line,1,'A');        // create a line after Current Line

    if (col>=LineLen(line)) return FALSE;

    if (extra+LineLen(line)-col>=LineWidth) extra=0; // new line may be started at a col other than 0
    LineAlloc(w,line+1,LineLen(line+1),extra+LineLen(line)-col); // reserve space in next line

    if (col<LineLen(line)) CurFmt=GetCurCfmt(w,line,col);
    else                   CurFmt=DEFAULT_CFMT;
    if (TerFont[CurFmt].style&PICT) CurFmt=DEFAULT_CFMT;

    // insert space in the beginning of the line
    for (i=0;i<extra;i++) SetLineData(w,line+1,i,1,&space,&lead,&CurFmt,&ct);

    // move data from the curent line to the next line
    MoveCharInfo(w,line,col,line+1,extra,LineLen(line)-col);

    LineAlloc(w,line,LineLen(line),col);  // free space from the current line

    // transfer tabw properties other than tab and justification properties
    if (tabw(line)) {
       CopyTabw(w,line,line+1);   // copy tabw data
       tabw(line)->type=tabw(line)->type&(INFO_TAB|INFO_JUST);
    }

    if (LineInfo(w,line,INFO_SECT)) {     // transfer the section mark
       tabw(line+1)->type|=INFO_SECT;
       tabw(line+1)->section=tabw(line)->section;
       ResetUintFlag(tabw(line)->type,INFO_SECT);
    }

    return TRUE;
}

/******************************************************************************
    TerJoinLine:
    Join next line at the end of the current line
*******************************************************************************/
TerJoinLine(PTERWND w)
{
    int i,SaveLen,TabLen=0,SaveCol;
    long SaveLine,SaveRow;
    LPBYTE ptr1,ptr2;
    LPWORD fmt1,fmt2,ct1,ct2;

    if (TotalLines==(CurLine+1)) return TRUE;
    if (LineLen(CurLine)+LineLen(CurLine+1)>LineWidth) {
       PrintError(w,MSG_LINE_TOO_LONG,"TerJoinLine");
       return TRUE;
    }
    TerArg.modified++;          // indicate that file has been modified

    ptr1=pText(CurLine+1);
    while(TabLen<LineLen(CurLine+1) && ptr1[TabLen]==' ') TabLen++;


    SaveLen=LineLen(CurLine);
    LineAlloc(w,CurLine,LineLen(CurLine),LineLen(CurLine)+LineLen(CurLine+1)-TabLen); // reserve more space in the current line

    ptr1=pText(CurLine);ptr2=pText(CurLine+1);
    OpenCharInfo(w,CurLine,&fmt1,&ct1);
    OpenCharInfo(w,CurLine+1,&fmt2,&ct2);
    for (i=0;i<LineLen(CurLine+1)-TabLen;i++) {
       ptr1[SaveLen+i]=ptr2[TabLen+i]; // move text
       fmt1[SaveLen+i]=fmt2[TabLen+i]; // move formats
       ct1[SaveLen+i]=ct2[TabLen+i]; // move formats
    }

    CloseCharInfo(w,CurLine);CloseCharInfo(w,CurLine+1);

    SaveLine=CurLine;SaveRow=CurRow;SaveCol=CurCol;
    CurLine++;CurRow++;      // position at the next line
    if (CurRow>=WinHeight) CurRow=WinHeight-1;
    BeginLine=CurLine-CurRow;

    TerDeleteLine(w);                                // delete that line

    CurLine=SaveLine;CurRow=SaveRow;CurCol=SaveCol;
    BeginLine=CurLine-CurRow;
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
   TerUp(w)
   process up arrow
*******************************************************************************/
TerUp(PTERWND w)
{
   TerSetCharHilight(w);                       // turnoff any highlighting

   if (TerArg.PageMode) return PgmUp(w);       // page mode

   // reverse the last line hilighting
   if ((CurLine+1)==TotalLines && HilightType==HILIGHT_CHAR && StretchHilight
       && (CurCol+1)==LineLen(CurLine) && CurCol>0) {
      CurCol=0;                                // goto the beginning of the line
      WrapFlag=WRAP_OFF;
      PaintFlag=PAINT_MIN;     // minimum repainting
      PaintTer(w);
      return TRUE;
   }


   if (CurLine<=0) return TRUE;

   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,TER_CENTER);  // try to position on the same device column

   if (CurRow>0) {
       CurRow--;CurLine--;
       CurCol=UnitsToCol(w,CursHorzPos,CurLine);

       if (TerArg.WordWrap) {
          if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
          if (CurCol<0) CurCol=0;
       }

       WrapFlag=WRAP_OFF;
       PaintFlag=PAINT_MIN;     // minimum repainting
       PaintTer(w);
       return TRUE;
   }
   else   {

      CurLine--;BeginLine--;CurRow--;
      CurCol=UnitsToCol(w,CursHorzPos,CurLine);

      if (TerArg.WordWrap) {
         CurRow=0;           // disable windows automatic scrolling
         if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
         if (CurCol<0) CurCol=0;
         WrapFlag=WRAP_PARA;
      }

      PaintFlag=PAINT_WIN;
      UseTextMap=FALSE;      // no point building map, probably won't be used
      PaintTer(w);

      return TRUE;
   }

}

/******************************************************************************
   TerCtrlUp(w)
   process up arrow with control key
*******************************************************************************/
TerCtrlUp(PTERWND w)
{
   if (CurCol==0) return TerUp(w);             // treat it as an up arrow

   TerSetCharHilight(w);                       // turnoff any highlighting

   CurCol=0;                                   // position at the first line

   PaintFlag=PAINT_WIN;
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
    TerDown:
    process the down arrow
*******************************************************************************/
TerDown(PTERWND w)
{
   if (TerArg.PageMode) return PgmDown(w);     // process in page mode
   
   TerSetCharHilight(w);                       // turnoff any highlighting

   if (CurLine+1>=TotalLines) {                // special treatment to hilight the entire file
      if (HilightType==HILIGHT_CHAR && StretchHilight && (CurCol+1)<LineLen(CurLine)) {
         CurCol=LineLen(CurLine)-1;
         if (CurCol<0) CurCol=0;
         WrapFlag=WRAP_OFF;
         PaintFlag=PAINT_MIN;       // minmum painting required
         PaintTer(w);
      }
      return TRUE;
   }

   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,TER_CENTER);  // try to position on the same device column

   if (CurLine<frame[0].ScrLastLine) {
       CurRow++;CurLine++;
       CurCol=UnitsToCol(w,CursHorzPos,CurLine);

       WrapFlag=WRAP_OFF;
       if (TerArg.WordWrap) {
          if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
          if (CurCol<0) CurCol=0;
       }
       PaintFlag=PAINT_MIN;       // minmum painting required
       PaintTer(w);
       return TRUE;
   }
   else   {
      CurLine++;
      CurCol=UnitsToCol(w,CursHorzPos,CurLine);

      if (TerArg.WordWrap) {  // do not allow automatic scrolling in doc mode
         CurRow=WinHeight-1;
         BeginLine++;
      }
      else {
         CurRow=WinHeight;
         BeginLine=CurLine-CurRow;
      }

      if (BeginLine<0) {BeginLine=0;CurRow=CurLine-BeginLine;}
      if (TerArg.WordWrap) {
         if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
         if (CurCol<0) CurCol=0;
      }
      UseTextMap=FALSE;      // no point building map, probably won't be used
      PaintFlag=PAINT_WIN;
      PaintTer(w);
   }
   return TRUE;
}

/******************************************************************************
    TerCtrlDown:
    process the down arrow with ctrl key
*******************************************************************************/
TerCtrlDown(PTERWND w)
{
   if (CurCol>0 && CurLine<(TotalLines-1)) CurCol=0;
   if (CurCol==0 && CurLine<(TotalLines-1)) return TerDown(w);   // treat it as a down arrow
   else                                     return TerEndLine(w);
}

/******************************************************************************
   TerRight:
   process the right arrow
*******************************************************************************/
TerRight(PTERWND w)
{

   BYTE LastChar;
   LPBYTE ptr;
   int SaveHilightType=HilightType;
   BOOL HilightBegins;

   TerSetCharHilight(w);                       // turnoff any highlighting
   if (SaveHilightType!=HILIGHT_OFF && HilightType==HILIGHT_OFF) return TRUE;  // no movement


   HilightBegins=(SaveHilightType==HILIGHT_OFF && HilightType==HILIGHT_CHAR);
   if (TerArg.PageMode) return PgmRight(w,HilightBegins);

   if (CurCol==LineWidth-1) return TRUE;
   if (TerArg.WordWrap && (CurLine==(TotalLines-1)) && (CurCol+1)==LineLen(CurLine)) return TRUE;
   
   CurCol++;

   if (LineLen(CurLine)>0) {
       ptr=pText(CurLine);
       LastChar=ptr[LineLen(CurLine)-1];

   }

   if (TerArg.WordWrap && CurCol>=LineLen(CurLine) &&
      ((CurLine+1)<TotalLines || LastChar==ParaChar)) {
       CurCol=0;
       if ((CurLine+1)<TotalLines) CurLine++;
   }
   else {
       WrapFlag=WRAP_OFF;
       PaintFlag=PAINT_MIN;        // minimum painting required
   }
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TerLeft:
   process the left arrow
*******************************************************************************/
TerLeft(PTERWND w)
{
   int SaveHilightType=HilightType;

   TerSetCharHilight(w);                       // turnoff any highlighting
   if (SaveHilightType!=HILIGHT_OFF && HilightType==HILIGHT_OFF) return TRUE;  // no movement

   if (TerArg.PageMode) return PgmLeft(w);

   if (CurCol>0) {
       CurCol--;
       WrapFlag=WRAP_OFF;
       PaintFlag=PAINT_MIN;
       PaintTer(w);
       return TRUE;
   }
   else {
       if (CurLine!=0) {
           if (CurRow>0) {
               CurRow--;CurLine--;
               CurCol=LineLen(CurLine)-1;
               PaintFlag=PAINT_MIN;         // minimum painting required
               WrapFlag=WRAP_MIN;
               PaintTer(w);
               return TRUE;
           }
           else{
               CurLine--;CurRow=0;
               if (CurLine<CurRow) CurRow=CurLine;
               BeginLine=CurLine-CurRow;
               CurCol=LineLen(CurLine)-1;
               PaintFlag=PAINT_WIN;
               PaintTer(w);
               return TRUE;
           }
       }
       else return TRUE;
   }

}

/******************************************************************************
   TerWinUp:
   Scroll the window up by one line maintaining the same cursor position if
   possible.
*******************************************************************************/
TerWinUp(PTERWND w)
{
   BOOL scrolled=FALSE;
   int  height,temp;

   CursDirection=CURS_UP;            // cursor begin pushed down

   if (InPrintPreview)  return PreviewUp(w,FALSE);
   if (TerArg.PageMode) return PgmWinUp(w);

   // check if the current partial line needs to be displayed
   CHECK_LINE_HEIGHT:
   height=GetLineHeight(w,BeginLine,&temp,NULL);
   if (height>TerWinHeight && (WinYOffsetLine>=0 || scrolled)) {
      if (WinYOffsetLine==-1 && scrolled)
            WinYOffset=height-TerWinHeight;
      else  WinYOffset-=TerWinHeight/6; // scroll up by 1/6 of window height
      if (WinYOffset>0) WinYOffsetLine=BeginLine;
      else              WinYOffsetLine=-1;
      goto END_WIN_UP;
   }

   // reset first line offset
   WinYOffsetLine=-1;
   if (scrolled) goto END_WIN_UP;

   // scrolling of normal height text lines
   if (CurLine<=0 && BeginLine<=0) return TRUE;

   if (CurLine>0)   CurLine--;
   if (BeginLine>0) BeginLine--;
   if (BeginLine>CurLine) BeginLine=CurLine;
   CurRow=CurLine-BeginLine;
   if (TerArg.WordWrap) {
      if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
      if (CurCol<0) CurCol=0;
   }
   scrolled=TRUE;
   goto CHECK_LINE_HEIGHT;

   END_WIN_UP:
   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
    TerWinDown:
   Scroll the window down by one line maintaining the same cursor position if
   possible.
*******************************************************************************/
TerWinDown(PTERWND w)
{
   int height,temp;

   CursDirection=CURS_DOWN;            // cursor begin pushed down

   if (InPrintPreview)  return PreviewDown(w,FALSE);
   if (TerArg.PageMode) return PgmWinDown(w);

   // check if document too small to scroll
   if (TotalLines<=WinHeight && DocFitsInWindow(w)) return true;
   
   // check if partial line needs to be displayed
   height=GetLineHeight(w,BeginLine,&temp,NULL);
   if (height>TerWinHeight) {
      if (WinYOffsetLine==-1) WinYOffset=0;
      WinYOffset+=TerWinHeight/6;           // scroll up by 1/6 of window height
      if (WinYOffset<height) {              // picture still in view
         WinYOffsetLine=BeginLine;
         goto END_WIN_DOWN;
      }
      if (WinYOffset>height) WinYOffset=height;
   }
   else WinYOffsetLine=-1;

   // scrolling of normal height text lines
   if (CurLine+1>=TotalLines && (BeginLine+1)>=TotalLines) return TRUE;
   if (HilightType!=HILIGHT_OFF && StretchHilight && (TotalLines-BeginLine)<(WinHeight-1))
      return TRUE;

   if ((CurLine+1)<TotalLines)   CurLine++;
   if ((BeginLine+1)<TotalLines) BeginLine++;
   if (BeginLine>CurLine) BeginLine=CurLine;
   CurRow=CurLine-BeginLine;
   if (TerArg.WordWrap) {
      if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
      if (CurCol<0) CurCol=0;
   }

   END_WIN_DOWN:
   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TerWinRight:
   Scroll the window right by one column maintaining the same cursor position if
   possible.
*******************************************************************************/
TerWinRight(PTERWND w)
{
   LPBYTE ptr;
   BYTE CurLead;
   WORD font;

   if (TerArg.PageMode) return PgmWinRight(w);

   if ((CurCol+1)>=LineWidth) return TRUE;
   CurCol++;

   if (CurCol<LineLen(CurLine)) {
      ptr=pText(CurLine);
      font=GetCurCfmt(w,CurLine,CurCol-1);
      CurLead=GetCurLead(w,CurLine,CurCol-1);
      TerWinOrgX=TerWinOrgX+DblCharWidth(w,font,TRUE,ptr[CurCol-1],CurLead);
   }
   else {
      if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
      if (CurCol<0) CurCol=0;
      if (TerArg.WordWrap) {
         WrapFlag=WRAP_OFF;
      }
      else TerWinOrgX=TerWinOrgX+DblCharWidth(w,0,TRUE,' ',0);
   }

   SetTerWindowOrg(w);    // set the logical window origin

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintTer(w);
   return TRUE;
}

/******************************************************************************
   TerWinLeft:
   Scroll the window left by one column maintaining the same cursor position if
   possible.
*******************************************************************************/
TerWinLeft(PTERWND w)
{
   LPBYTE ptr;
   BYTE CurLead;
   WORD CurFont;

   if (TerArg.PageMode) return PgmWinLeft(w);

   if (CurCol==0) return TRUE;
   CurCol--;

   if ((CurCol+1)<LineLen(CurLine)) {
      ptr=pText(CurLine);
      CurFont=GetCurCfmt(w,CurLine,CurCol+1);
      CurLead=GetCurLead(w,CurLine,CurCol+1);
      TerWinOrgX=TerWinOrgX-DblCharWidth(w,CurFont,TRUE,ptr[CurCol+1],CurLead);
   }
   else TerWinOrgX=TerWinOrgX-DblCharWidth(w,0,TRUE,' ',0);

   if (TerWinOrgX<0) TerWinOrgX=0;
   SetTerWindowOrg(w);     // set the logical window origin

   if (TerArg.WordWrap) {
      if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
      if (CurCol<0) CurCol=0;
      WrapFlag=WRAP_OFF;
   }

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);
   return TRUE;
}

/******************************************************************************
   TerPageLeft:
   Scroll the window left by a page or a column
*******************************************************************************/
TerPageLeft(PTERWND w,BOOL page)
{
   if (InPrintPreview) return PreviewLeft(w,page);
   if (TerArg.PageMode) return PgmPageLeft(w,page);

   if (TerArg.WordWrap && !HScrollAllowed(w)) {
     if (page) TerWinOrgX-=TerWinWidth/2;
     else      TerWinOrgX-=TerWinWidth/6;
     if (TerWinOrgX<0) TerWinOrgX=0;
     SetTerWindowOrg(w); // set the window origin at TerWinOrgX value
   }
   else {
     if (page) return TerPageHorz(w,'L',0);  // bar left
     else      return TerPageHorz(w,'l',0);  // arrow left
   }

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);
   return TRUE;
}

/******************************************************************************
   TerPageRight:
   Scroll the window right by page or a column
*******************************************************************************/
TerPageRight(PTERWND w,BOOL page)
{
   if (InPrintPreview) return PreviewRight(w,page);
   if (TerArg.PageMode) return PgmPageRight(w,page);

   if (TerArg.WordWrap && !HScrollAllowed(w)) {
     if (TerWrapWidth(w,CurLine,-1)<TerWinWidth) return TRUE;  // no reason to scroll

     if (page) TerWinOrgX+=TerWinWidth/2;
     else      TerWinOrgX+=TerWinWidth/6;

     if (TerWinOrgX>(TerWrapWidth(w,CurLine,-1)-TerWinWidth)) TerWinOrgX=TerWrapWidth(w,CurLine,-1)-TerWinWidth;
     SetTerWindowOrg(w); // set the window origin at TerWinOrgX value
   }
   else {
     if (page) return TerPageHorz(w,'R',0);  // bar right
     else      return TerPageHorz(w,'r',0);  // arrow right
   }

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);
   return TRUE;
}

/******************************************************************************
   TerPageHorz:
   Scroll the window horizontally to position at the specified thumbtrack
   position.
*******************************************************************************/
TerPageHorz(PTERWND w,BYTE type, int pos)
{
   int MaxWidth,width;
   long l;

   if (TerArg.PageMode) return PgmPageHorz(w,pos);

   if (TerArg.WordWrap && !HScrollAllowed(w)) {
     if (TerWrapWidth(w,CurLine,-1)<TerWinWidth) return TRUE;  // no reason to scroll

     TerWinOrgX=(int)(((long)(TerWrapWidth(w,CurLine,-1)-TerWinWidth)*pos)/SCROLL_RANGE);
     if (TerWinOrgX>(TerWrapWidth(w,CurLine,-1)-TerWinWidth)) TerWinOrgX=TerWrapWidth(w,CurLine,-1)-TerWinWidth;

     SetTerWindowOrg(w); // set the window origin at TerWinOrgX value
   }
   else {
     // find the maximum width of lines on the screen
     MaxWidth=0;
     for (l=BeginLine;l<=frame[0].ScrLastLine;l++) {
        width=GetLineWidth(w,l,TRUE,TRUE);
        if (width>MaxWidth) MaxWidth=width;
     }

     // adjust TerWinOrgX
     if      (type=='L') TerWinOrgX-=(TerWinWidth/2);   // bar left
     else if (type=='l') TerWinOrgX-=(DblCharWidth(w,0,TRUE,'M',0));   // arrow left
     else if (type=='R') TerWinOrgX+=(TerWinWidth/2);  // bar right
     else if (type=='r') TerWinOrgX+=(DblCharWidth(w,0,TRUE,'M',0));   // arrow right
     else                TerWinOrgX=(int)(((long)(MaxWidth-TerWinWidth)*pos)/SCROLL_RANGE);  // thumb position

     if (TerWinOrgX>(MaxWidth-TerWinWidth)) TerWinOrgX=MaxWidth-TerWinWidth;
     if (TerWinOrgX<0) TerWinOrgX = 0;

     SetTerWindowOrg(w); // set the window origin at TerWinOrgX value
   }

   UseTextMap=FALSE;      // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);
   return TRUE;
}

/*****************************************************************************
     TerPageUp:
     Display the previous page
******************************************************************************/
TerPageUp(PTERWND w, BOOL keyboard)
{
   int PageLength;

   if (InPrintPreview) return PreviewUp(w,TRUE);

   if (keyboard) TerSetCharHilight(w);  // turnoff any highlighting on a keyboard action

   if (TerArg.PageMode) return PgmPageUp(w);

   if (CurLine==0) TerWinUp(w);   // simulate WinUp to handle lines larger than window height

   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,TER_CENTER);  // try to position on the same device column

   if (BeginLine==0) {                  // only one page text
      CurLine=CurRow=0;
      CurCol=UnitsToCol(w,CursHorzPos,CurLine);
      PaintFlag=PAINT_MIN;
      WrapFlag=WRAP_OFF;
      PaintTer(w);
      return true;
   }

   PageLength=WinHeight-PagingMargin;
   if (PageLength<=0) PageLength=1;
   CurLine=CurLine-PageLength;

   BeginLine=BeginLine-PageLength;
   if (BeginLine<0) {   // first page
      CurLine=CurRow;
      BeginLine=0;
   }
   CurCol=UnitsToCol(w,CursHorzPos,CurLine);

   UseTextMap=FALSE;    // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);

   return TRUE;
}

/*****************************************************************************
     TerPageDn:
     Display the next page
******************************************************************************/
TerPageDn(PTERWND w, BOOL keyboard)
{
   int PageLength;
   long LastLine;

   if (InPrintPreview) return PreviewDown(w,TRUE);

   if (keyboard) TerSetCharHilight(w);   // turnoff any highlighting on a keyboard action

   if (TerArg.PageMode) return PgmPageDn(w);

   if (CursHorzPos<0) CursHorzPos=ColToUnits(w,CurCol,CurLine,TER_CENTER);  // try to position on the same device column

   if (frame[0].ScrLastLine>=(TotalLines-1)) {    // last page displayed
      if (!keyboard && !StretchHilight && BeginLine!=(TotalLines-1)) { // for scroll down only
         CurLine=BeginLine=TotalLines-1;
         CurRow=0;
         PaintTer(w);
      }
      else TerWinDown(w);    // simulate win-down to handle pictures greater than screen height
      return TRUE;
   }
   else {
      PageLength=(int)(frame[0].ScrLastLine-BeginLine+1-PagingMargin);
      if (PageLength<=0) PageLength=1;

      LastLine=TotalLines-1;
      if (StretchHilight) {      // let half the screen be always full
         int height=TerWinHeight/2;
         for (;LastLine>0 && height>0;LastLine--) height-=ScrLineHeight(w,LastLine,TRUE,FALSE);
      }

      BeginLine=BeginLine+PageLength;
      if (BeginLine>=LastLine) BeginLine=LastLine;
      CurLine=CurLine+PageLength;
      if (CurLine>=(TotalLines-1)) CurLine=TotalLines-1;
      if (CurLine<BeginLine) CurLine=BeginLine;
      CurRow=CurLine-BeginLine;
   }

   CurCol=UnitsToCol(w,CursHorzPos,CurLine);

   UseTextMap=FALSE;                            // no point building map, probably won't be used
   PaintFlag=PAINT_WIN;
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
    TerBeginLine:
    process at the beginning of the line
*******************************************************************************/
TerBeginLine(PTERWND w)
{
   TerSetCharHilight(w);   // turnoff any highlighting

   CurCol=0;
   PrevCursCol=0;          // force forward lookup to avoid positioning on the hidden character

   PaintFlag=PAINT_MIN;
   PaintTer(w);
   return TRUE;
}

/******************************************************************************
    TerEndLine:
    process at the end of the line
*******************************************************************************/
TerEndLine(PTERWND w)
{
    TerSetCharHilight(w);                       // turnoff any highlighting

    if (LineLen(CurLine)==0) return TRUE;
    CurCol=LineLen(CurLine);
    if (CurCol>(LineWidth-1)) CurCol=LineWidth-1;
    if (TerArg.WordWrap && CurCol>0) CurCol--;

    PaintFlag=PAINT_MIN;
    PaintTer(w);
    return TRUE;
}

/******************************************************************************
   TerBeginFile:
   position at the beginning of the file
*******************************************************************************/
TerBeginFile(PTERWND w)
{
   long l,HdrLine,FHdrLine;

   TerSetCharHilight(w);                       // turnoff any highlighting

   CurCol=0;

   if (TerArg.PageMode) {
     if (EditPageHdrFtr) { // position at the header
        CurPage=0;
        RefreshFrames(w,TRUE);
        // find the first header line
        HdrLine=FHdrLine=-1;
        for (l=0;l<TotalLines;l++) {
           if (!(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)) break;
           if (LineFlags(l)&LFLAG_HDR && HdrLine==-1) HdrLine=l;
           if (LineFlags(l)&LFLAG_FHDR && FHdrLine==-1) FHdrLine=l;
        }
        if      (FHdrLine>=0) TerPosLine(w,FHdrLine+2);  // position beyond the delimiter line 
        else if (HdrLine>=0) TerPosLine(w,HdrLine+2);    // position beyond the delimiter line 
        else    TerPosLine(w,l+1);                       // position on the first body line
     }
     else {                 // position past any header/footer lines
        for (l=0;l<TotalLines;l++) if (!(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)) break;  // position past the header/footer
        if (l==TotalLines) l++;
        TerPosLine(w,l+1);
     }
   }
   else TerPosLine(w,1L);

   return TRUE;
}

/******************************************************************************
   TerEndFile:
   position at the end of the file
*******************************************************************************/
TerEndFile(PTERWND w)
{
   TerSetCharHilight(w);                       // turnoff any highlighting

   CurCol=LineLen(TotalLines-1);
   if (TerArg.WordWrap) CurCol--;
   if (CurCol<0) CurCol=0;

   if (TerArg.PageMode && ViewPageHdrFtr) {
      CurPage=TotalPages-1;
      if (CurPage<0) CurPage=0;
      RefreshFrames(w,TRUE);
   }
   HilightAtCurPos=TRUE;                      // lock hilight at current position

   TerPosLine(w,TotalLines);
   return TRUE;
}

/******************************************************************************
   TerBackTab:
   process the backtab (shift) key
*******************************************************************************/
TerBackTab(PTERWND w)
{
   LPBYTE ptr;
   int CurPara,blt,CurFont;

   if (TerArg.PrintView && cid(CurLine)>0) return TerBackTabCell(w);

   if (LineLen(CurLine)==0) return TRUE;

   // on the first character of a list?
   if (CurCol==0 && True(LineFlags(CurLine)&LFLAG_PARA_FIRST)) {
      CurPara=pfmt(CurLine);
      blt=PfmtId[CurPara].BltId;
      if (False(TerFlags6&TFLAG6_DONT_PROCESS_BULLET_KEYS) && blt!=0 && TerBlt[blt].ls!=0 && TerBlt[blt].lvl>0) TerSetListLevel(hTerWnd,-1,-1,true);  // decrease by one level
      return true;
   }

   // check if tab pressed on a text-input field
   CurFont=GetCurCfmt(w,CurLine,CurCol);
   if (TerFont[CurFont].FieldId==FIELD_TEXTBOX) return TabOnControl(w,-1,true);


   // position at the previous tab position
   if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
   ptr=pText(CurLine);
   if (CurCol > 0 && ptr[CurCol]==TAB) CurCol--;  // skip the current tab
   while (CurCol>0) {
      if (ptr[CurCol]==TAB) break;
      CurCol--;
   }


   PaintFlag=PAINT_MIN;
   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TerNextWord:
   position at the next word
*******************************************************************************/
TerNextWord(PTERWND w)
{
   int i,CurFont,PrevFont;
   BOOL FoundSpace=FALSE;
   LPBYTE ptr;
   LPWORD fmt;

   TerSetCharHilight(w);                       // turnoff any highlighting

   CursDirection=CURS_FORWARD;

   CurFont=PrevFont=0;

   LINE_LOOP:
   ptr=pText(CurLine);
   fmt=OpenCfmt(w,CurLine);

   for (i=CurCol;i<LineLen(CurLine);i++) {
      PrevFont=CurFont;
      CurFont=fmt[i];

      if ((FoundSpace && ptr[i]!=' ' && ptr[i]!=TAB)
          || (i>CurCol && IsBreakChar(w,ptr[i]))
          || (i>CurCol && TerFont[CurFont].FieldId!=TerFont[PrevFont].FieldId)) {

          CurCol=i;
          goto DisplayScreen;
      }
      if (ptr[i]==' ' || ptr[i]==TAB) FoundSpace=TRUE;
   }

   CloseCfmt(w,CurLine);


   if ((CurLine+1)<TotalLines) {
       CurCol=0;CurLine++;CurRow++;
       FoundSpace=TRUE;
       goto LINE_LOOP;
   }

   DisplayScreen:
   TerPosLine(w,CurLine+1);

   return TRUE;
}

/******************************************************************************
   TerPrevWord:
   position at the previous word
*******************************************************************************/
TerPrevWord(PTERWND w, BOOL pos)
{
   int i,CurFont,PrevFont,StartCol;
   BOOL FoundChars=FALSE;
   LPBYTE ptr;
   LPWORD fmt;

   TerSetCharHilight(w);                       // turnoff any highlighting

   CursDirection=CURS_BACKWARD;

   if (CurCol >= LineLen(CurLine)) StartCol = LineLen(CurLine) - 2;
   else StartCol = CurCol-1;

   CurFont=PrevFont=0;

   LINE_LOOP:
   ptr=pText(CurLine);
   fmt=OpenCfmt(w,CurLine);

   for (i=StartCol;i>=0;i--) {
     PrevFont=CurFont;
     CurFont=fmt[i];

     if ((FoundChars && (ptr[i]==' ' || ptr[i]==TAB))
        || (i<StartCol && TerFont[CurFont].FieldId!=TerFont[PrevFont].FieldId) ){

        CurCol=i+1;
        goto DisplayScreen;
     }
     if (ptr[i]!=' ' && ptr[i]!=TAB) FoundChars=TRUE;
   }

   CloseCfmt(w,CurLine);

   if (CurLine>0 && !FoundChars) {
      CurLine--;CurRow--;
      if (LineFlags(CurLine)&(LFLAG_PARA|LFLAG_BREAK) && LineLen(CurLine)>0) {
         CurCol=LineLen(CurLine)-1;
         goto DisplayScreen;
      }
      StartCol=LineLen(CurLine)-2;
      goto LINE_LOOP;
   }
   CurCol=0;

   DisplayScreen:
   if (pos) TerPosLine(w,CurLine+1);

   return TRUE;
}

/******************************************************************************
   TerDelPrevWord:
   delete the previous the previous word
*******************************************************************************/
TerDelPrevWord(PTERWND w)
{
   long SaveCurLine=CurLine;
   int  SaveCurCol=CurCol;

   if (CurCol==0 && CurLine>0 && LineInfo(w,CurLine-1,(INFO_ROW|INFO_CELL))) return FALSE;

   if (!TerPrevWord(w,FALSE)) return FALSE;

   HilightBegRow=CurLine;
   HilightBegCol=CurCol;
   HilightEndRow=SaveCurLine;
   HilightEndCol=SaveCurCol;
   HilightType=HILIGHT_CHAR;

   TerDeleteBlock(hTerWnd,TRUE);

   return TRUE;
}

/******************************************************************************
    TerPosLine:
    position at the given line number
*******************************************************************************/
TerPosLine(PTERWND w,long GotoLine)
{

    if (GotoLine>TotalLines) GotoLine=TotalLines;
    if (GotoLine<1) GotoLine=1;

    CurLine=GotoLine-1;

    if ((CurLine-BeginLine)>=WinHeight || (CurLine-BeginLine)<0) {
       BeginLine=CurLine-WinHeight/2;  // position the current line at center
       if (BeginLine<0) BeginLine=0;
    }

    CurRow=CurLine-BeginLine;

    // Page mode processing
    if (TerArg.PageMode) {
       int SavePage=CurPage,SaveWinOrgY;
       int y;
       CurPage=GetCurPage(w,CurLine);
       if (SavePage!=CurPage) RefreshFrames(w,TRUE);
       if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
       if (CurCol<0) CurCol=0;
       y=LineToUnits(w,CurLine);
       SaveWinOrgY=TerWinOrgY;
       if (y<TerWinOrgY || y>=(TerWinOrgY+TerWinHeight)) TerWinOrgY=y;
       if ((TerWinOrgY+TerWinHeight)>CurPageHeight) TerWinOrgY=CurPageHeight-TerWinHeight;
       if (TerWinOrgY<0) TerWinOrgY=0;
       if (TerWinOrgY!=SaveWinOrgY) SetTerWindowOrg(w);
    }

    PaintTer(w);

    if (TerArg.ShowHorBar || TerArg.ShowVerBar) SetScrollBars(w);

    InitCaret(w);                    // redisplay the caret if hidden

    return TRUE;
}

/******************************************************************************
    TerDoubleClick:
    This function processes the mouse double click.  If the mouse is clicked
    on an ole object, the object activated.  Else, if the mouse is clicked on
    a double underline text, a hyperlink message is sent to the calling
    application.  Otherwise the current word is highlighted.
*******************************************************************************/
TerDoubleClick(PTERWND w)
{
    int i,CurFont=0,PrevFont;
    LPBYTE ptr;
    BOOL OnWord,OffWord,IsPunctuation;
    LPWORD fmt;

    if (RulerClicked || LineLen(CurLine)==0) return TRUE;

    if (TerEditOle(w,FALSE)) return TRUE;    // edit ole item if applicable

    if (LinkDblClick && SendLinkMessage(w,TRUE)) return TRUE;  // send hyperlink message

    if (TerFlags3&TFLAG3_NO_MOUSE_SEL) return TRUE;

    if (PictureClicked) {
       CurFont=GetCurCfmt(w,CurLine,CurCol);
       if (True(TerFont[CurFont].style&PICT)) return TRUE;
    } 

    ptr=pText(CurLine);
    fmt=OpenCfmt(w,CurLine);

    // find the highlight beginning point
    OnWord=FALSE;
    for (i=CurCol;i>=0;i--) {
       PrevFont=CurFont;
       CurFont=fmt[i];
       if (i<CurCol && TerFont[CurFont].FieldId!=TerFont[PrevFont].FieldId) {  // break at field change too
         OnWord=TRUE;
         break;
       }

       IsPunctuation=TerFont[CurFont].UcBase==0 && (ptr[i]>=33 && ptr[i]<=46) || (ptr[i]>=58 && ptr[i]<=64);  // punctuation chars 33-46, 58 to 64

       if (OnWord && (ptr[i]==' ' || IsPunctuation || ptr[i]==TAB)) break;  // previous word begins
       if (ptr[i]!=' ' && ptr[i]!=TAB) OnWord=TRUE;       //now positioned on a word
    }
    if (OnWord) HilightBegCol=i+1;
    else {                                // scan the forward direction
       for (i=CurCol;i<LineLen(CurLine);i++) {
          if (ptr[i]!=' ' && ptr[i]!=TAB) break; // first word found
       }
       if (i==LineLen(CurLine)) return TRUE; // blank line
       HilightBegCol=i;
    }

    // find the highlight end point
    OffWord=FALSE;
    for (i=HilightBegCol;i<LineLen(CurLine);i++) {
       PrevFont=CurFont;
       CurFont=fmt[i];
       if (i>HilightBegCol && TerFont[CurFont].FieldId!=TerFont[PrevFont].FieldId) {  // break at field change too
         OffWord=TRUE;
         break;
       }

       IsPunctuation=TerFont[CurFont].UcBase==0 && (ptr[i]>=33 && ptr[i]<=46) || (ptr[i]>=58 && ptr[i]<=64);  // punctuation chars 33-46, 58 to 64

       if ( ptr[i]==TAB || ptr[i]==ParaChar || ptr[i]==LINE_CHAR || ptr[i]==CellChar
         || ptr[i]==PAGE_CHAR || ptr[i]==COL_CHAR || ptr[i]==SECT_CHAR
         || IsPunctuation) {  // break characters end the word
          OffWord=TRUE;
          break;
       }

       if (OffWord && ptr[i]!=' ') break;     // next word begins
       if (ptr[i]==' ') OffWord=TRUE;         //now positioned after the word
    }
    if (OffWord) HilightEndCol=i;
    else         HilightEndCol=LineLen(CurLine);

    if (HilightEndCol>=LineLen(CurLine) && (CurLine+1)==TotalLines && TerArg.WordWrap) HilightEndCol--;

                     // now unlock the line

    CloseCfmt(w,CurLine);

    if (HilightEndCol<=HilightBegCol || HilightBegCol==-1) return TRUE;

    HilightBegRow=HilightEndRow=CurLine;
    CurCol=HilightEndCol;
    HilightType=HILIGHT_CHAR;
    
    StretchHilight=TRUE;       // let it be stretched
    IgnoreMouseMove=FALSE;
    DblClickHilight=TRUE;      // hilight begus by double click
    DblClickEndRow=HilightEndRow;
    DblClickEndCol=HilightEndCol;

    PaintTer(w);
    return TRUE;
}

/******************************************************************************
    CanInsert:
    Returns TRUE if a text character for text block is allowed to be inserted
    at the specified position.
*******************************************************************************/
BOOL CanInsert(PTERWND w,long line, int col)
{
    int PrevCfmt,CurCfmt;
    LPBYTE ptr;

    if (line>=TotalLines) return FALSE;
    if (col>=LineLen(line) && !TerArg.WordWrap) return TRUE;
    if (col>=LineLen(line)) return FALSE;

    // check the header/footer protection
    if (TerArg.PageMode && !EditPageHdrFtr && PfmtId[pfmt(line)].flags&PAGE_HDR_FTR) return FALSE;
    if (LineFlags(line)&LFLAG_HDRS_FTRS) return FALSE;

    // check for protected text
    CurCfmt=GetCurCfmt(w,line,col);          // current font id
    if (TerFont[CurCfmt].style&PROTECT) {    // now check for the previous character
       PrevCfmt=GetPrevCfmt(w,line,col);
       if (TerFont[PrevCfmt].style&PROTECT) {
          BOOL InsertAllowed=FALSE;
          // check  if located on the first character of a field name
          if (TerFont[CurCfmt].FieldId==FIELD_NAME && TerFont[PrevCfmt].FieldId==FIELD_NAME) {
             ptr=pText(line);
             if (ptr[col]=='{') {           // located on the first character of a field name
                int NextCfmt=GetNextCfmt(w,line,col);
                if (!(TerFont[NextCfmt].style&PROTECT)) InsertAllowed=TRUE; // the entire field name is not protected
             }
          }
          if (FALSE && !InsertAllowed) {
             int EffectFont=GetEffectiveCfmt(w);
             if (!(TerFont[EffectFont].style&PROTECT)) InsertAllowed=TRUE;
          }  
          if (!InsertAllowed) return FALSE;    // can not insert between 2 protected characters.
       }
    }

    // can insert at the table row line.
    if (tabw(line) && tabw(line)->type&INFO_ROW) return FALSE;

    // on a dynamic field
    if (IsDynField(w,TerFont[CurCfmt].FieldId) && CurCfmt==GetPrevCfmt(w,line,col)) return FALSE;

    return TRUE;
}

/******************************************************************************
    IsProtectedChar:
    Returns TRUE is a character is a protected character.
*******************************************************************************/
BOOL IsProtectedChar(PTERWND w,long line, int col)
{
    int CurCfmt,PrevCfmt,NextCfmt;
    LPBYTE ptr;
    BYTE CurChar;
    long TempLine;

    if (line>=TotalLines || col>=LineLen(line)) return FALSE;

    // check the header/footer protection
    if (TerArg.PageMode && !EditPageHdrFtr && PfmtId[pfmt(line)].flags&PAGE_HDR_FTR) return TRUE;
    if (LineFlags(line)&LFLAG_HDRS_FTRS) return TRUE;

    // Ensure that a section with header/footer keeps atleast one regular line
    if ( line>0 && LineLen(line)==1
      && PfmtId[pfmt(line-1)].flags&PAGE_HDR_FTR
      && !(PfmtId[pfmt(line)].flags&PAGE_HDR_FTR)
      && (line>=(TotalLines-1) || (tabw(line+1) && tabw(line+1)->type&INFO_SECT)) ) return TRUE;

    // check for protected text
    CurCfmt=GetCurCfmt(w,line,col);          // current font id
    if (TerFont[CurCfmt].style&PROTECT) return TRUE;


    // check if this is a only character between 2 protected characters
    PrevCfmt=NextCfmt=0;
    if (col>0) PrevCfmt=GetCurCfmt(w,line,col-1);
    else {
       for (TempLine=line-1;TempLine>=0;TempLine--) if (LineLen(TempLine)>0) break;
       if (TempLine>=0) PrevCfmt=GetCurCfmt(w,TempLine,LineLen(TempLine)-1);
    }
    if ((col+1)<LineLen(line)) NextCfmt=GetCurCfmt(w,line,col+1);
    else {
       for (TempLine=line+1;TempLine<TotalLines;TempLine++) if (LineLen(TempLine)>0) break;
       if (TempLine<TotalLines) NextCfmt=GetCurCfmt(w,TempLine,0);
    }
    if (!(TerFlags2&TFLAG2_CAN_MERGE_PROT_TEXT) && TerFont[PrevCfmt].style&PROTECT && TerFont[NextCfmt].style&PROTECT) return TRUE;

    // check for footnote
    if (TerFont[CurCfmt].style&(FNOTE|FNOTEREST)) return TRUE;
    if (TerFont[CurCfmt].style&FNOTETEXT && TerFont[PrevCfmt].style&FNOTE && !(TerFont[NextCfmt].style&FNOTETEXT)) return TRUE;  // can't delete the last character of a footnote text

    // check for invisible framed picture
    if (!ShowParaMark && !PictureClicked && TerFont[CurCfmt].style&PICT && TerFont[CurCfmt].FrameType!=PFRAME_NONE) return TRUE;

    // check for data field protection
    if (True(TerFlags5&TFLAG5_PROTECT_DATA_FIELD) 
                  && (TerFont[CurCfmt].FieldId==FIELD_NAME || TerFont[CurCfmt].FieldId==FIELD_DATA)) return TRUE;

    // check for tablw/cell special characters
    ptr=pText(line);
    CurChar=ptr[col];


    if (CurChar==CellChar || CurChar==ROW_CHAR) return TRUE;

    // check for frame ending para character
    if (  fid(line)>0
       && (line==(TotalLines-1) || fid(line+1)==0)
       && CurChar==ParaChar) return TRUE;

    // check for para char before a frame
    if ((col+1)==LineLen(line) && LineFlags(line)&LFLAG_PARA && (line+1)<TotalLines
       && fid(line)!=fid(line+1)) return TRUE;

    // check for para char in html mode
    if (HtmlMode && col>0 && (col+1)==LineLen(line) && LineFlags(line)&LFLAG_PARA) {
       if ((line+1)>=TotalLines || PfmtId[pfmt(line)].AuxId!=PfmtId[pfmt(line+1)].AuxId) return TRUE;
    }

    // check for para char before header/footer delimiter line
    if ((col+1)==LineLen(line) && LineFlags(line)&LFLAG_PARA && !LineInfo(w,line,INFO_SECT) && (line+1)<TotalLines
       && LineFlags(line+1)&LFLAG_HDRS_FTRS) return TRUE;

    // check for para char before the table
    if (TerFlags4&TFLAG4_NO_MERGE_TABLE) {
      if (cid(line)==0 && (col+1)==LineLen(line) && LineFlags(line)&LFLAG_PARA && !LineInfo(w,line,INFO_SECT) && (line+1)<TotalLines
         && cid(line+1)>0) return TRUE;
    } 
    
    return FALSE;
}

/******************************************************************************
    CanInsertBreakChar:
    Returns TRUE if a break character can be inserted at the specified location.
*******************************************************************************/
BOOL CanInsertBreakChar(PTERWND w,long line, int col)
{
    if (PfmtId[pfmt(line)].flags&PAGE_HDR_FTR) return FALSE;  // no break characters allowed in header/footer

    return CanInsertObject(w,line,col);
}

/******************************************************************************
    CanInsertPageBreak:
    Returns TRUE if a break character can be inserted at the specified location.
*******************************************************************************/
BOOL CanInsertPageBreak(PTERWND w,long line, int col)
{
    if (PfmtId[pfmt(line)].flags&PAGE_HDR_FTR) return FALSE;  // no break characters allowed in header/footer

    if (fid(line)) return FALSE;       // can not insert a break character in a frame

    return CanInsertTextObject(w,line,col);
}

/******************************************************************************
    CanInsertTable:
    Returns TRUE if a table can be inserted at the specified location.
*******************************************************************************/
BOOL CanInsertTable(PTERWND w,long line, int col)
{
    int ParaFID;
 
    if (fid(line)) {
       ParaFID=fid(line);
       if (ParaFrame[ParaFID].flags&(PARA_FRAME_RECT|PARA_FRAME_LINE)) return FALSE;
    } 
    if (cid(line) && !(TerFlags3&TFLAG3_EMBEDDED_TABLES)) return FALSE;

    return CanInsertTextObject(w,line,col);
}

/******************************************************************************
    CanInsertObject:
    Returns TRUE if an object such as table, or frame can be inserted at the specified location.
*******************************************************************************/
BOOL CanInsertObject(PTERWND w,long line, int col)
{
    if (fid(line) || cid(line)) return FALSE;       // can not insert a break character in a frame

    return CanInsertTextObject(w,line,col);
}

/******************************************************************************
    CanInsertTextObject:
    Returns TRUE if an text object such as field or footnote can be inserted at the specified location.
*******************************************************************************/
BOOL CanInsertTextObject(PTERWND w,long line, int col)
{
    int CurCfmt,PrevCfmt,CurFieldId,PrevFieldId;

    if (LineFlags(line)&(LFLAG_HDR|LFLAG_FHDR|LFLAG_FTR|LFLAG_FFTR)) return FALSE;
    if (LineInfo(w,line,INFO_ROW)) return FALSE;

    // check the fonts
    CurCfmt=GetCurCfmt(w,line,col);
    PrevCfmt=GetPrevCfmt(w,line,col);
    CurFieldId=TerFont[CurCfmt].FieldId;
    PrevFieldId=TerFont[PrevCfmt].FieldId;
    
    if (   (CurFieldId == FIELD_DATA && PrevFieldId == FIELD_NAME)
        || (CurFieldId == FIELD_DATA && PrevFieldId == FIELD_DATA)
        || (CurFieldId == FIELD_NAME && PrevFieldId == FIELD_NAME))  return FALSE;

    if (TerFont[CurCfmt].style&(FNOTETEXT|FNOTEREST)) return FALSE;

    return TRUE;
}

/******************************************************************************
    CanInsertInInputField:
    Returns TRUE if an text can be inserted into an text input field.
*******************************************************************************/
BOOL CanInsertInInputField(PTERWND w,int pict, long line, int col)
{
    long StartPos,EndPos;
    long StartLine;
    int  MaxLen,StartCol,len;
    BYTE MaxLenStr[50];

    if (TerFont[pict].FieldCode==null) return true;
    
    GetStringField(TerFont[pict].FieldCode,0,'|',MaxLenStr);  // first field - max len
    MaxLen=atoi(MaxLenStr);
    if (MaxLen==0) return true;    // no max-length limitation

    // find the current length of the field
    StartLine=line;
    StartCol=col;
    if (!TerLocateFieldChar(hTerWnd,FIELD_TEXTBOX,NULL,false, &StartLine, &StartCol, false)) {
       StartLine=0;
       StartCol=0;
    }      
    StartPos=RowColToAbs(w,StartLine,StartCol);

    StartLine=line;
    StartCol=col;
    if (!TerLocateFieldChar(hTerWnd,FIELD_TEXTBOX,NULL,false, &StartLine, &StartCol, true)) return true;
    EndPos=RowColToAbs(w,StartLine,StartCol);

    len=(int)(EndPos-StartPos-1);

    return (MaxLen>len);
}

/******************************************************************************
    ScrollText:
    This function scrolls the text when the mouse pointer goes out of the text
    box edge. This fuction returns TRUE if the screen is scrolled.

    This function relies on the previous call to TerMousePos function.
*******************************************************************************/
ScrollText(PTERWND w)
{
    MSG  msg;

    if (MouseOverShoot==' ') return FALSE;
    if (DraggingText && MouseOverShootDist>TerFont[0].height*3/2) return FALSE;


    PROCESS_SCROLL:

    if       (MouseOverShoot=='L') TerWinLeft(w);   // left scroll
    else if  (MouseOverShoot=='R') TerWinRight(w);  // right scroll
    else if  (MouseOverShoot=='T') {
       BOOL fast=(TerWinRect.top-MouseY)>=(2*TerFont[0].height);
       if (TerArg.PageMode) {
          if (TerFlags4&TFLAG4_SMOOTH_SCROLL && !fast) TerWinUp(w);
          else                                         PgmUp(w);
       }
       else {
          if (fast) TerPageUp(w,FALSE);                     // bigger jump
          else      TerWinUp(w);
       }
    }
    else if  (MouseOverShoot=='B') {
       BOOL fast=(MouseY-TerWinRect.bottom)>=(2*TerFont[0].height);
       if (TerArg.PageMode) {
          CommandId=ID_DOWN;   // simulate arrow down
          if (TerFlags4&TFLAG4_SMOOTH_SCROLL && !fast) {
             int x;
             long line;
             TerWinDown(w);
             // position the current line at the bottom of the window
             x=ColToUnits(w,CurCol,CurLine,TER_CENTER);
             line=UnitsToLine(w,x,TerWinOrgY+TerWinHeight);
             if (line!=CurLine) {
                CurLine=line;
                CurCol=UnitsToCol(w,x,CurLine);
             } 
          }
          else PgmDown(w);
       }
       else {
          if (fast) TerPageDn(w,FALSE);                    // bigger jump
          else      TerWinDown(w);
       }
    }

    if (DraggingText) {         // check if the screen should keep scrolling
       if (!PeekMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST,PM_NOREMOVE|PM_NOYIELD)) {
          goto PROCESS_SCROLL;
       }
    }

    return TRUE;
}

/******************************************************************************
    AdjustHiddenPos:
    Adjust the cursor if positioned on the hidden text
*******************************************************************************/
void AdjustHiddenPos(PTERWND w)
{
    int i,len,StartCol,loop=0,NewCol;
    BOOL LineForward,HideHdrFtr=FALSE;
    long l,SaveCurLine;
    LPWORD fmt;

    if ((TerFlags2&TFLAG2_NO_ADJUST_CURSOR) || (TerOpFlags&TOFLAG_NO_ADJUST_POS || !CaretEngaged)) return;     // adjustment disabled

    // is cursor limited to a cell
    if (TerArg.PageMode && TerFlags3&TFLAG3_CURSOR_IN_CELL && CursorCell>0
        && (cid(CurLine)!=CursorCell || LineInfo(w,CurLine,INFO_ROW))) {
       for (l=0;l<TotalLines;l++) if (cid(l)==CursorCell) break;
       if (l==TotalLines) {
          CursorCell=0;
          return;   // cursor cell not found
       }
       if (CurLine<l) {      // is cursor before the cell
          CurLine=l;
          CurCol=0;
          CursDirection=CURS_FORWARD;
       }
       else {
          for (;l<TotalLines;l++) if (cid(l)!=CursorCell || LineInfo(w,l,INFO_ROW)) break;
          l--;
          if (CurLine>l) {
             CurLine=l;
             CurCol=LineLen(CurLine)-1;
             CursDirection=CURS_BACKWARD;
          }
       }
    }


    if (TerArg.PageMode && !ViewPageHdrFtr) HideHdrFtr=TRUE;

    if (TerArg.PageMode && tabw(CurLine) && tabw(CurLine)->type&(INFO_ROW)) goto ADJUST_POS;
    if (LineFlags(CurLine)&LFLAG_HDRS_FTRS) goto ADJUST_POS;
    if (HideHdrFtr && PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR) goto ADJUST_POS;

    //if (ShowHiddenText && EditFootnoteText) return;

    if (MoveCursor(w,CurLine,CurCol)) goto ADJUST_POS;  // move the cursor off this location

    return;

    ADJUST_POS:
    // determine the direction of the movement
    LineForward=GetCursDirection(w);

    MOVE_CURSOR:
    SaveCurLine=CurLine;

    // move cursor LineForward
    if (LineForward) {
       bool PosAftHdrFtr=false;
       if (TerArg.PageMode && ViewPageHdrFtr && !EditPageHdrFtr) {
         // move the line off the header area to the body of the current page
         if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR) {
            CurLine=PageInfo[CurPage].FirstLine;
            for (;(CurLine+1)<TotalLines;CurLine++) if (!(PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR)) break;
            CurCol=0;
         } 
         // move the line off the footer area to the body of the next page
         if (PfmtId[pfmt(CurLine)].flags&PAGE_FTR && (CurPage+1)<TotalPages) {
            CurLine=PageInfo[CurPage+1].FirstLine;
            PosAftHdrFtr=true; 
         }
       }

       for (l=CurLine;l<TotalLines;l++) {
          len=LineLen(l);
          if (len==0) continue;
          if (LineFlags(l)&LFLAG_HDRS_FTRS) continue;
          if (tabw(l) && tabw(l)->type&INFO_ROW) continue;
          if (HideHdrFtr && PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;

          fmt=OpenCfmt(w,l);
          if (l==CurLine) StartCol=CurCol+1;
          else            StartCol=0;
          if (PosAftHdrFtr) StartCol=0;

          for (i=StartCol;i<len;i++) {
             if (!MoveCursor(w,l,i)) {     // this location is ok
                if ((CommandId==ID_DOWN || CommandId==ID_PGDN || CursDirection==CURS_DOWN)
                     && l>PrevCursLine && TerArg.PageMode && LineX(l)!=LineX(CurLine)) {
                   NewCol=UnitsToCol(w,CursHorzPos,l);
                   if (NewCol>=i) {
                      i=NewCol;
                      if (MoveCursor(w,l,i)) continue;  // location not ok
                   }
                   else continue;
                }
                CurLine=l;
                CurCol=i;
                if (CommandId==ID_LINE_END && LineLen(CurLine)>0) CurCol=LineLen(CurLine)-1;
                CloseCfmt(w,l);
                goto END_ADJUST;
             }
          }
          CloseCfmt(w,l);
       }
       // position at the last character
       CurLine=TotalLines-1;
       CurCol=LineLen(CurLine)-1;
       if (CurCol<0) CurCol=0;
       
       goto END_ADJUST;
    }
    else {           // move cursor backward
       if (TerArg.PageMode && ViewPageHdrFtr && !EditPageHdrFtr) {
         // move the line off the footer area to the body of the current page
         if (PfmtId[pfmt(CurLine)].flags&PAGE_FTR) {
            CurLine=PageInfo[CurPage].LastLine;
         } 
         // move the line off the hdr area to the body of the previous page
         if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR && CurPage>0) {
            CurLine=PageInfo[CurPage-1].LastLine;
         } 
       }
       
       for (l=CurLine;l>=0;l--) {
          len=LineLen(l);
          if (len==0) continue;
          if (LineFlags(l)&LFLAG_HDRS_FTRS) continue;
          if (tabw(l) && tabw(l)->type&INFO_ROW) continue;
          if (HideHdrFtr && PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;

          fmt=OpenCfmt(w,l);

          // on up arrow try to maintain the column position
          if ( (CommandId==ID_UP || CommandId==ID_PGUP || CursDirection==CURS_UP) && l<PrevCursLine) {
             // try to position at the same horizontal position
             NewCol=UnitsToCol(w,CursHorzPos,l);
             for (i=NewCol;i<len;i++) if (!MoveCursor(w,l,i)) break;
             if (i>=len) {        // now try backward on the line
                for (i=NewCol;i>=0;i--) if (!MoveCursor(w,l,i)) break;
             }

             if (i>=0) {
                CurLine=l;
                CurCol=i;
                CloseCfmt(w,l);
                goto END_ADJUST;
             }
             else continue;   // go to the previous line
          }
          else {
             if (l==CurLine) StartCol=CurCol-1;
             else            StartCol=len-1;

             for (i=StartCol;i>=0;i--) {
                if (!MoveCursor(w,l,i)) {
                   CurLine=l;
                   CurCol=i;
                   CloseCfmt(w,l);
                   goto END_ADJUST;
                }
             }
             CloseCfmt(w,l);
          }
       }
    }

    // Handle if positioning could not be performed
    loop++;
    if (l<0 || l==TotalLines) {
       if (loop>=2) {
         if (HideHdrFtr) {  // display the header footer if no where to go
           ViewPageHdrFtr=TRUE;
           HideHdrFtr=FALSE;
         }
         else return;                  // accept the current position
       }

       LineForward=!LineForward;             // move other side
       goto MOVE_CURSOR;
    }

    END_ADJUST:
    // Maintain begin line at current line
    if (SaveCurLine==BeginLine) {
       BeginLine=CurLine;
       CurRow=0;
    }

}

/******************************************************************************
    GetCursDirection:
    Get the current cursor direction.  This function returns TRUE for the
    forward direction and FALSE for the backward direction.
*******************************************************************************/
BOOL GetCursDirection(PTERWND w)
{
    BOOL forward;

    if      (CursDirection==CURS_FORWARD || CursDirection==CURS_DOWN)    forward=TRUE;
    else if (CursDirection==CURS_BACKWARD || CursDirection==CURS_UP) forward=FALSE;
    else {
       forward=FALSE;
       if (TerArg.PageMode) {
          if      (CurPage>PrevCursPage) forward=TRUE;
          else if (CurPage==PrevCursPage && (CurLineY+TerWinOrgY)>PrevCursLineY) forward=TRUE;
          else if (CurLine==PrevCursLine && CurCol>=PrevCursCol) forward=TRUE;
       }
       else {
          if      (CurLine>PrevCursLine) forward=TRUE;
          else if (CurLine==PrevCursLine && CurCol>=PrevCursCol) forward=TRUE;
       }
    }

    return forward;
}

/*****************************************************************************
    MoveCursor:
    This function returns TRUE if the cursor should be moved off this location.
******************************************************************************/
BOOL MoveCursor(PTERWND w,long LineNo, int col)
{
    int CurCfmt=GetCurCfmt(w,LineNo,col);
    int FieldId,CellId;
    BOOL BefHiddenText=(TerFlags2&TFLAG2_CURSOR_BEF_HIDDEN)?TRUE:FALSE;
    BOOL BefAndAftHidden=(TerFlags5&TFLAG5_BEF_AND_AFT_HIDDEN)?TRUE:FALSE;
    BOOL NotInsideProtText=(TerFlags2&TFLAG2_NO_CURSOR_ON_PROTECT)?TRUE:FALSE;
    BOOL SkipProtText=(TerFlags4&TFLAG4_SKIP_PROT_TEXT)?TRUE:FALSE;

    if (HtmlMode) {
       if (!TerArg.ReadOnly) {  // in edit mode do not allow the cursor on the first hypertext character because new input at this location will take the hidden attribute
          int PrevCfmt=GetPrevCfmt(w,LineNo,col);
          int CurHidden=HiddenText(w,CurCfmt);
          int PrevHidden=HiddenText(w,PrevCfmt);
          if (CurHidden && (!PrevHidden)) return FALSE;  // ok to be on the first hidden character
          if ((!ShowHiddenText) && PrevHidden && (!CurHidden) && IsHypertext(w,CurCfmt)) return TRUE;  // can't be on the first link charcter
       }
       if (HiddenText(w,CurCfmt)) return TRUE;  // cursor not on a hidden character
    }
    else {
       if (BefHiddenText) {
          int PrevCfmt=GetPrevCfmt(w,LineNo,col);
          if (HiddenText(w,PrevCfmt)) {
             if (NotInsideProtText) {    // check if moving backward might endup between the protected text
                long TempLine=LineNo;
                int  TempCol=col-1,font1,font2;
                FixPos(w,&TempLine,&TempCol);
                if (!TerLocateStyleChar(hTerWnd,HIDDEN,FALSE,&TempLine,&TempCol,FALSE)) return TRUE;  // find the end of the hidden style in the backwrad direction
                font1=GetCurCfmt(w,TempLine,TempCol);
                font2=GetNextCfmt(w,TempLine,TempCol);
                if (!(TerFont[font1].style&PROTECT) || !(TerFont[font2].style&PROTECT)) return TRUE;
             }
             else return TRUE;
          }
       }
       else {
          if (HiddenText(w,CurCfmt)) {
            if (BefAndAftHidden) {   // cursor can also stop at first hidden chracter also
               int PrevCfmt=GetPrevCfmt(w,LineNo,col);
               if (HiddenText(w,PrevCfmt)) return TRUE;
            }
            else return TRUE;     // cursor not on a hidden character
          }
       }

       if (NotInsideProtText) {
          int PrevCfmt=GetPrevCfmt(w,LineNo,col);
          if (TerFont[PrevCfmt].style&PROTECT && TerFont[CurCfmt].style&PROTECT) return TRUE;
       }
       if (SkipProtText) {
          if (TerFont[CurCfmt].style&PROTECT) return TRUE;
       }

       FieldId=TerFont[CurCfmt].FieldId;
       if (FieldId>0 && FieldId!=FIELD_NAME && FieldId!=FIELD_DATA 
                     && FieldId!=FIELD_TOC && FieldId!=FIELD_HLINK && FieldId!=FIELD_TEXTBOX) {
          int PrevCfmt=GetPrevCfmt(w,LineNo,col);
          if (TerFont[PrevCfmt].FieldId>0) return TRUE;  // positioned in the middle of a field
       }
       if (ViewPageHdrFtr && !(EditPageHdrFtr) && PfmtId[pfmt(LineNo)].flags&PAGE_HDR_FTR) return TRUE;

       CellId=cid(LineNo);
       if (cell[CellId].flags&CFLAG_ROW_SPANNED) return TRUE;
    }         

    return FALSE;
}

/*****************************************************************************
    IsHiddenLine:
    Returns TRUE if all characters in the line are hidden
******************************************************************************/
BOOL IsHiddenLine(PTERWND w,long LineNo)
{
    int i,len,CurFont,PrevFont=-1;
    LPWORD fmt;
 
    if (cfmt(LineNo).info.type==UNIFORM) {
       CurFont=cfmt(LineNo).info.fmt;
       return HiddenText(w,CurFont);
    }
    else {
      fmt=cfmt(LineNo).pFmt;
      len=LineLen(LineNo);
      for (i=0;i<len;i++) {
        CurFont=fmt[i];
        if (CurFont==PrevFont) continue;
        if (!HiddenText(w,CurFont)) break;
        PrevFont=CurFont;
      }
      if (i<len) return FALSE; 
    }
 
    return TRUE;
}
 
/*****************************************************************************
    HiddenText:
    This function returns true when a hidden or footnote text need to be hidden
******************************************************************************/
BOOL HiddenText(PTERWND w,int CurFont)
{
    BOOL hide=FALSE;
    UINT style=TerFont[CurFont].style;

    if ( (!ShowHiddenText && style&HIDDEN)) hide=TRUE;
    if ((!EditFootnoteText && (style&FNOTETEXT && !(style&ENOTETEXT)) && !(TerOpFlags&TOFLAG_FNOTE_FRAME))) hide=TRUE;
    if (!EditEndnoteText && style&ENOTETEXT) hide=TRUE;

    if (!ShowFieldNames && TerFont[CurFont].FieldId==FIELD_NAME) hide=TRUE;
    if (ShowFieldNames && TerFont[CurFont].FieldId==FIELD_DATA) hide=TRUE;

    //if (!ShowFieldNames && TerFont[CurFont].FieldId==FIELD_TC) hide=TRUE;

    return hide;
}

/******************************************************************************
     MessagePending:
     This routine returns TRUE when a keystroke or mouse message is pending
******************************************************************************/
BOOL MessagePending(PTERWND w)
{
    MSG msg;
    
    if ( (PeekMessage(&msg,hTerWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE|PM_NOYIELD) && msg.wParam!=VK_RETURN && CommandId!=ID_RETURN)  // when the enter key is pressed, the TranslateMessage might generate WM_CHAR at the same time ID_RETURN accelerator is generated
      || PeekMessage(&msg,hTerWnd,WM_KEYDOWN,WM_KEYDOWN,PM_NOREMOVE|PM_NOYIELD)
      || PeekMessage(&msg,hTerWnd,WM_LBUTTONDOWN,WM_MOUSELAST,PM_NOREMOVE|PM_NOYIELD) )
         if (msg.hwnd==hTerWnd) return TRUE;

    if (HilightType!=HILIGHT_OFF && StretchHilight && PeekMessage(&msg,hTerWnd,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_NOREMOVE|PM_NOYIELD) )
         if (msg.hwnd==hTerWnd) return TRUE;

    return FALSE;
}

/******************************************************************************
     CharMessagePending:
     This routine returns TRUE when a keystroke message is pending
******************************************************************************/
BOOL CharMessagePending(PTERWND w)
{
    MSG msg;

    if ( PeekMessage(&msg,hTerWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE|PM_NOYIELD)
      || PeekMessage(&msg,hTerWnd,WM_KEYDOWN,WM_KEYDOWN,PM_NOREMOVE|PM_NOYIELD) )
         if (msg.hwnd==hTerWnd) return TRUE;

    if (HilightType!=HILIGHT_OFF && StretchHilight && PeekMessage(&msg,hTerWnd,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_NOREMOVE|PM_NOYIELD) )
         if (msg.hwnd==hTerWnd) return TRUE;

    return FALSE;
}

/******************************************************************************
    SetLineText:
    Apply the given text string at the specified location.  The string can
    be an mbcs strings.
******************************************************************************/
SetLineText(PTERWND w, LPBYTE string, long line, int col)
{
    LPBYTE ptr,lead,text=NULL,LeadText=NULL;
    int len,i;

    len=AllocDB(w,string,&text,&LeadText);  // allocate pointers and split

    if ((col+len)>LineLen(line)) LineAlloc(w,line,LineLen(line),col+len);

    ptr=pText(line);
    if (mbcs) lead=OpenLead(w,line);

    for (i=0;i<len;i++) {
       ptr[col+i]=text[i];
       if (mbcs) lead[col+i]=LeadText[i];
    }

    if (mbcs)   CloseLead(w,line);

    if (text)     OurFree(text);
    if (LeadText) OurFree(LeadText);

    return TRUE;
}

/******************************************************************************
    AllocDB:
    Split mbcs into two strings and allocate memory for the sub strings.  This
    function returns the length of the split strings.
******************************************************************************/
AllocDB(PTERWND w,BYTE huge *string,LPBYTE far *text,LPBYTE far *LeadText)
{
    int len;

    len=strlen(string);
    (*text)=OurAlloc(len+1);
    (*LeadText)=NULL;

    if (mbcs) {
       (*LeadText)=OurAlloc(len+1);
       len=(int)TerSplitMbcs(hTerWnd,string,(*text),(*LeadText));
    }
    else lstrcpy((*text),string);

    return len;
}

/******************************************************************************
    TerMbcsChar:
    Retrieve an mbcs character at the given character position.  This function
    return 0 if the index position is greater or equal to  the mbcs string length.
******************************************************************************/
WORD WINAPI _export TerMbcsChar(HWND hWnd,BYTE huge *string, long pos)
{
    long ByteIdx,CharIdx,len;
    WORD chr;
    BYTE CurByte;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!mbcs) {
       len=hstrlen(string);
       if (pos>=len) return 0;
       else          return string[pos];
    }

    // position at the 'pos' location
    len=hstrlen(string);
    if (len==0) return 0;

    ByteIdx=CharIdx=0;
    while (CharIdx<pos && ByteIdx<len) {
       CurByte=string[ByteIdx];
       if (IsDBCSLeadByte(CurByte)) ByteIdx++;
       ByteIdx++;
       CharIdx++;
    }
    if (ByteIdx==len) return 0;

    // extract the character at the current location
    CurByte=string[ByteIdx];
    chr=0;
    if (IsDBCSLeadByte(CurByte)) {
       chr=((WORD)CurByte)<<8;
       ByteIdx++;
       CurByte=string[ByteIdx];
    }
    chr|=CurByte;              // low byte

    return chr;
}

/******************************************************************************
    TerMbcsLen:
    This function returns the number of characters in an mbcs string.  The
    string must be NULL terminated.
******************************************************************************/
long WINAPI _export TerMbcsLen(HWND hWnd,BYTE huge *string)
{
    return TerSplitMbcs(hWnd,string,NULL,NULL);
}

/******************************************************************************
    TerSplitMbcs:
    This function splits an mbcs string into the lead and tail byte strings. This
    function returns the length of the split string.  The 'string' and 'tail'
    is allowed to point to the same location. If the string does contain the
    ending tail character, then the tail character is set to NULL, but the
    length (return value) variables includes this partial character.  Both
    'lead' and 'tail' can be NULL, in which case the function simply returns
    the length of the split string.
******************************************************************************/
long WINAPI _export TerSplitMbcs(HWND hWnd,BYTE huge *string, BYTE huge *tail, BYTE huge *lead)
{
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data
   
   return TerSplitMbcs2(w,string,tail,lead,0);
}

/******************************************************************************
    TerSplitMbcs2:
******************************************************************************/
long TerSplitMbcs2(PTERWND w,BYTE huge *string, BYTE huge *tail, BYTE huge *lead, int CodePage)
{
   return TerSplitMbcs3(w,string,NULL,tail,lead,NULL,CodePage);
}

/******************************************************************************
    TerSplitMbcs2:
******************************************************************************/
long TerSplitMbcs3(PTERWND w,BYTE huge *string, WORD huge *pUcBase, BYTE huge *tail, BYTE huge *lead, WORD huge * pOutUcBase, int CodePage)
{
   long len,l,m;
   BYTE CurByte;
   WORD CurUcBase,NextUcBase;

   len=hstrlen(string);

   if (!mbcs) {       // not in an mbcs os
      if (tail && (tail!=string)) strcpy(tail,string);
      if (lead) FarMemSet((LPBYTE)lead,0,(UINT)len);
      return len;
   }

   // decode mbcs
   m=0;
   for (l=0;l<len;l++) {
      CurByte=string[l];
      CurUcBase=pUcBase?pUcBase[l]:0;
      NextUcBase=(pUcBase && (l+1)<len)?pUcBase[l+1]:0;

      if (CurUcBase!=0 || NextUcBase!=0 || (l+1)==len || CurByte<32 || !IsDBCSLeadByteEx(CodePage,CurByte)) {     // last byte
         if (tail) tail[m]=CurByte;
         if (lead) lead[m]=0;
         if (pOutUcBase && pUcBase) pOutUcBase[m]=CurUcBase;
      }
      else {
         if (lead) lead[m]=CurByte;
         if (tail) tail[m]=string[l+1];
         if (pOutUcBase && pUcBase) pOutUcBase[m]=0;   // mbcs character, not an unicode character
         l++;
      }
      m++;
   }
   if (lead) lead[m]=0;
   if (tail) tail[m]=0;

   return m;
}

/******************************************************************************
    TerMakeMbcs:
    This function combines the lead and the tail bytes to make an mbcs
    string.  The 'count' parameter is the lenght of the tail string.
    The 'lead' parameter can be NULL.
    The function returns the length of the mbcs string.
******************************************************************************/
int WINAPI _export TerMakeMbcs(HWND hWnd,LPBYTE string, LPBYTE tail, LPBYTE lead, int count)
{
   int i,j;
   PTERWND w;

   if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

   if (count==0) return 0;

   if (!mbcs || lead==NULL) {   // lead bytes not available
      for (i=0;i<count;i++) string[i]=tail[i];  // simply copy
      string[i]=0;
      return count;
   }

   // combine lead and tail bytes
   j=0;
   for (i=0;i<count;i++,j++) {
      if (lead[i]) {
         string[j]=lead[i];
         j++;
      }
      string[j]=tail[i];
   }
   string[j]=0;

   return j;
}

/******************************************************************************
    MoveLineArrays:
    Use this routine to insert or delete elements from the line array.
    Parameters #1 and #2 specify the range of elements to insert or delete.
    Parameter #3 specifies the request type:
         'D'  : Delete the array elements
         'A'  : Insert after the start line element.
         'B'  : Insert before the start line element.
    The calling routine must ensure that final line count does not exceed
    MaxLines.
    This routine WILL update the TotalLines variable.
******************************************************************************/
void MoveLineArrays(PTERWND w,long StartLine,long count,BYTE InsertDel)
{
    long l;


    if (InsertDel=='A') {                     // insert after the start line
        if (!CheckLineLimit(w,TotalLines+count)) return;
        
        if (TotalLines-StartLine-1>0) {
           HugeMove(&LinePtr[StartLine+1],&LinePtr[StartLine+count+1],(TotalLines-StartLine-1)*(sizeof (struct StrLinePtr far *))); // make space for new lines
        }

        for (l=(StartLine+1);l<=(StartLine+count);l++) InitLine(w,l);      // initialize the new pointer space
        TotalLines=TotalLines+count;
        AdjustSections(w,StartLine,count);    // adjust the section boundaries
        if (RepageBeginLine>(StartLine+1)) RepageBeginLine=StartLine+1;
    }
    else if (InsertDel=='B') {                // insert before the start line
        if (!CheckLineLimit(w,TotalLines+count)) return;
        
        HugeMove(&LinePtr[StartLine],&LinePtr[StartLine+count],(TotalLines-StartLine)*(sizeof (struct StrLinePtr far *))); // make space for new lines
        for (l=StartLine;l<(StartLine+count);l++) InitLine(w,l);        // initialize the new pointer space
        TotalLines=TotalLines+count;
        AdjustSections(w,StartLine-1,count);    // adjust the section boundaries
        if (RepageBeginLine>StartLine) RepageBeginLine=StartLine;
    }
    else {                                      // delete the elements
        for (l=StartLine;l<(StartLine+count) && l<TotalLines;l++) {
           if (LinePtr[l] && LineFlags2(l)&LFLAG2_ENOTETEXT) TerOpFlags|=TOFLAG_UPDATE_ENOTE;
           FreeLine(w,l);
        }
        if (TotalLines-StartLine-count>0) {
           HugeMove(&LinePtr[StartLine+count],&LinePtr[StartLine],(TotalLines-StartLine-count)*(sizeof (struct StrLinePtr far *))); // make space for new lines
        }
        TotalLines=TotalLines-count;

        if (TotalLines==0) {
           TotalLines=1;
           InitLine(w,0);
        }
        AdjustSections(w,StartLine,-count);     // adjust the section boundaries
        if (RepageBeginLine>(StartLine-1)) RepageBeginLine=StartLine-1;
    }
}

/******************************************************************************
    MoveLineData:
    Use this routine to insert or delete data with a line.  Both text and
    format data is processed by this routine.
    Parameter #1 specifies the line number to process.
    Parameters #2 and #3 specify the range of characters to insert or delete.
    Parameter #4 specifies the request type:
         'D'  : Delete the specified characters
         'A'  : Insert after the starting character position.
         'B'  : Insert before the start character position line element.

    This routine WILL update the allocation space for the data by calling
    LineAlloc routine.  This routine will expand the formatting data if needed.
    If the formatting data is expaned, it will be compressed back at the end.
******************************************************************************/
void MoveLineData(PTERWND w,long line,int StartPos,int count,BYTE InsertDel)
{
    int i,OldLen;
    WORD CurFmt;
    LPBYTE ptr,lead;
    LPWORD fmt,cmi;
    LPWORD cw;
    BOOL UniformFmt;
    long SaveRepageBeginLine=RepageBeginLine;

    OldLen=LineLen(line);

    UniformFmt=(cfmt(line).info.type==UNIFORM);

    if (InsertDel=='A') {                     // insert after the starting position
        LineAlloc(w,line,OldLen,OldLen+count);
        if ((StartPos+1)<OldLen) {
           ptr=pText(line);
           FarMoveOl(&(ptr[StartPos+1]),&(ptr[StartPos+1+count]),OldLen-StartPos-1); // make space for new characters

           if (!UniformFmt) {
              fmt=cfmt(line).pFmt;
              FarMoveOl(&(fmt[StartPos+1]),&(fmt[StartPos+1+count]),sizeof(WORD)*(OldLen-StartPos-1)); // make space for new format
              CurFmt=fmt[StartPos+1+count];
              for(i=0;i<count;i++) fmt[StartPos+1+i]=CurFmt;
           }

           if (pCtid(line)) {
              cmi=pCtid(line);
              FarMoveOl(&(cmi[StartPos+1]),&(cmi[StartPos+1+count]),sizeof(WORD)*(OldLen-StartPos-1)); // make space for new format
              for(i=0;i<count;i++) cmi[StartPos+1+i]=0;
           }
           if (pLead(line)) {
              lead=pLead(line);
              FarMoveOl(&(lead[StartPos+1]),&(lead[StartPos+1+count]),sizeof(BYTE)*(OldLen-StartPos-1)); // make space for new format
              for(i=0;i<count;i++) lead[StartPos+1+i]=0;
           }
           if (pCharWidth(line)) {
              cw=pCharWidth(line);
              FarMoveOl(&(cw[StartPos+1]),&(cw[StartPos+1+count]),sizeof(WORD)*(OldLen-StartPos-1)); // make space for new format
              for(i=0;i<count;i++) cw[StartPos+1+i]=-1;
           }
        }
    }
    else if (InsertDel=='B') {                // insert before the start line
        LineAlloc(w,line,OldLen,OldLen+count);
        if (StartPos<OldLen) {
           ptr=pText(line);
           FarMoveOl(&(ptr[StartPos]),&(ptr[StartPos+count]),OldLen-StartPos); // make space for new characters

           if (!UniformFmt) {
              fmt=cfmt(line).pFmt;
              FarMoveOl(&(fmt[StartPos]),&(fmt[StartPos+count]),sizeof(WORD)*(OldLen-StartPos)); // make space for new format
              CurFmt=fmt[StartPos+count];
              for(i=0;i<count;i++) fmt[StartPos+i]=CurFmt;
           }

           if (pCtid(line)) {
              cmi=pCtid(line);
              FarMoveOl(&(cmi[StartPos]),&(cmi[StartPos+count]),sizeof(WORD)*(OldLen-StartPos)); // make space for new format
              for(i=0;i<count;i++) cmi[StartPos+i]=0;
           }
           if (pLead(line)) {
              lead=pLead(line);
              FarMoveOl(&(lead[StartPos]),&(lead[StartPos+count]),sizeof(BYTE)*(OldLen-StartPos)); // make space for new format
              for(i=0;i<count;i++) lead[StartPos+i]=0;
           }
           if (pCharWidth(line)) {
              cw=pCharWidth(line);
              FarMoveOl(&(cw[StartPos]),&(cw[StartPos+count]),sizeof(WORD)*(OldLen-StartPos)); // make space for new format
              for(i=0;i<count;i++) cw[StartPos+i]=-1;
           }
        }
    }
    else {                                    // delete the elements
        if (count<=OldLen) {
           // Check if section character being deleted
           ptr=pText(line);
           for (i=0;i<count;i++) if (ptr[StartPos+i]==SECT_CHAR) SectModified=PosPageHdrFtr=TRUE;

           if ((StartPos+count)<OldLen) {
              ptr=pText(line);
              FarMoveOl(&(ptr[StartPos+count]),&(ptr[StartPos]),OldLen-StartPos-count); // make space for new characters

              if (!UniformFmt) {
                 fmt=cfmt(line).pFmt;
                 FarMoveOl(&(fmt[StartPos+count]),&(fmt[StartPos]),sizeof(WORD)*(OldLen-StartPos-count)); // make space for new formats
              }

              if (pCtid(line)) {
                 cmi=pCtid(line);
                 FarMoveOl(&(cmi[StartPos+count]),&(cmi[StartPos]),sizeof(WORD)*(OldLen-StartPos-count)); // make space for new formats
              }
              if (pLead(line)) {
                 lead=pLead(line);
                 FarMoveOl(&(lead[StartPos+count]),&(lead[StartPos]),sizeof(BYTE)*(OldLen-StartPos-count)); // make space for new formats
              }
              if (pCharWidth(line)) {
                 cw=pCharWidth(line);
                 FarMoveOl(&(cw[StartPos+count]),&(cw[StartPos]),sizeof(WORD)*(OldLen-StartPos-count)); // make space for new formats
              }
           }
           LineAlloc(w,line,OldLen,OldLen-count);
        }
    }

    if (TerOpFlags&TOFLAG_INSERT_CHAR) RepageBeginLine=SaveRepageBeginLine;
    else     if (RepageBeginLine>line) RepageBeginLine=line;

    ResetLongFlag(LineFlags2(line),LFLAG2_SPELL_CHECKED);   // may need to respell check this line
}

/******************************************************************************
    MoveCharInfo:
    Move the char info from one line to another.
******************************************************************************/
void MoveCharInfo(PTERWND w,long SrcLine, int SrcCol, long DestLine, int DestCol, int count)
{
    LPBYTE ptr1,ptr2,lead1,lead2;
    LPWORD fmt1,fmt2,ct1,ct2;


    ptr1=pText(SrcLine);
    OpenCharInfo(w,SrcLine,&fmt1,&ct1);      // split the format info
    if (mbcs) lead1=OpenLead(w,SrcLine);

    if (SrcLine==DestLine) {             // overlapping move
       FarMoveOl(&ptr1[SrcCol],&ptr1[DestCol],sizeof(BYTE)*count);
       FarMoveOl(&fmt1[SrcCol],&fmt1[DestCol],sizeof(WORD)*count);
       FarMoveOl(&ct1[SrcCol],&ct1[DestCol],sizeof(WORD)*count);
       if (mbcs) FarMoveOl(&lead1[SrcCol],&lead1[DestCol],sizeof(BYTE)*count);
    }
    else {
       ptr2=pText(DestLine);
       OpenCharInfo(w,DestLine,&fmt2,&ct2);
       if (mbcs) lead2=OpenLead(w,DestLine);

       FarMove(&ptr1[SrcCol],&ptr2[DestCol],sizeof(BYTE)*count);
       FarMove(&fmt1[SrcCol],&fmt2[DestCol],sizeof(WORD)*count);
       FarMove(&ct1[SrcCol],&ct2[DestCol],sizeof(WORD)*count);
       if (mbcs) FarMove(&lead1[SrcCol],&lead2[DestCol],sizeof(BYTE)*count);

       CloseCharInfo(w,DestLine);
       if (mbcs) CloseLead(w,DestLine);
    }

    CloseCharInfo(w,SrcLine);
    if (mbcs) CloseLead(w,SrcLine);

    return;
}

/******************************************************************************
    GetLineData:
    Copy the char info to specified pointers.
******************************************************************************/
void GetLineData(PTERWND w,long SrcLine, int SrcCol, int count, LPBYTE ptr, LPBYTE lead, LPWORD fmt, LPWORD ct)
{
    LPBYTE ptr1,lead1;
    LPWORD fmt1,ct1;


    ptr1=pText(SrcLine);
    OpenCharInfo(w,SrcLine,&fmt1,&ct1);      // split the format info
    if (mbcs) lead1=OpenLead(w,SrcLine);

    if (ptr)  {
       FarMoveOl(&ptr1[SrcCol],ptr,sizeof(BYTE)*count);
       ptr[count]=0;                         // NULL terminate
    }
    if (fmt)  FarMoveOl(&fmt1[SrcCol],fmt,sizeof(WORD)*count);
    if (ct)   FarMoveOl(&ct1[SrcCol],ct,sizeof(WORD)*count);
    if (lead && mbcs) FarMoveOl(&lead1[SrcCol],lead,sizeof(BYTE)*count);

    CloseCharInfo(w,SrcLine);
    if (mbcs) CloseLead(w,SrcLine);

    return;
}

/******************************************************************************
    SetLineData:
    Set the line data form the specified pointers.
******************************************************************************/
void SetLineData(PTERWND w,long DestLine, int DestCol, int count, LPBYTE ptr, LPBYTE lead, LPWORD fmt, LPWORD ct)
{
    LPBYTE ptr1,lead1;
    LPWORD fmt1,ct1;


    ptr1=pText(DestLine);
    OpenCharInfo(w,DestLine,&fmt1,&ct1);      // split the format info
    if (mbcs) lead1=OpenLead(w,DestLine);

    if (ptr)  FarMoveOl(ptr,&ptr1[DestCol],sizeof(BYTE)*count);
    if (fmt)  FarMoveOl(fmt,&fmt1[DestCol],sizeof(WORD)*count);
    if (ct)   FarMoveOl(ct,&ct1[DestCol],sizeof(WORD)*count);
    if (lead && mbcs) FarMoveOl(lead,&lead1[DestCol],sizeof(BYTE)*count);

    CloseCharInfo(w,DestLine);
    if (mbcs) CloseLead(w,DestLine);

    return;
}

/******************************************************************************
    CopyLineData:
    Copy data from one line to another.
******************************************************************************/
void CopyLineData(PTERWND w,long SrcLine,long DestLine)
{
    int  len;
    LPBYTE ptr,lead;
    LPWORD fmt,ct;

    len=LineLen(SrcLine);
    LineAlloc(w,DestLine,0,len);
    
    if (len==0) return;

    // copy  line data
    ptr=MemAlloc(len+1);    // an extra byte for NULL terminator
    lead=MemAlloc(len);
    fmt=MemAlloc(len*sizeof(WORD));
    ct=MemAlloc(len*sizeof(WORD));

    GetLineData(w,SrcLine,0,len,ptr,lead,fmt,ct);
    SetLineData(w,DestLine,0,len,ptr,lead,fmt,ct);

    MemFree(ptr);
    MemFree(lead);
    MemFree(fmt);
    MemFree(ct);

    // set line ids
    pfmt(DestLine)=pfmt(SrcLine);
    cid(DestLine)=cid(SrcLine);
    fid(DestLine)=fid(SrcLine);
    LineFlags(DestLine)=LineFlags(SrcLine);

    return;
}
 
/*****************************************************************************
    InitLine:
    Initialize a new line.
******************************************************************************/
InitLine(PTERWND w,long line)
{
    int CurPfmtId,CurCellId,ParaFID;

    AllocLinePtr(w,line);   // allocate the line pointer

    pText(line)=NULL;
    cfmt(line).pFmt=NULL;
    cfmt(line).info.type=UNIFORM;
    cfmt(line).info.fmt=DEFAULT_CFMT;
    pCtid(line)=NULL;
    pLead(line)=NULL;
    pCharWidth(line)=NULL;

    LineLen(line)=0;
    LineHt(line)=(PrtResY*12)/72;     // initialize line height to 12 points
    ScrLineHt(line)=(ScrResY*12)/72;  // initialize line height to 12 points
    LineFlags(line)=0;        // initialize line flags 
    LineFlags2(line)=0;       // initialize line flags 2
    tabw(line)=NULL;

    LineY(line)=0;            // initialize line y position array
    LineX(line)=0;            // initialize line x position array

    if (line>0) CurPfmtId=pfmt(line-1);
    else        CurPfmtId=0;
    if (CurPfmtId<TotalPfmts) pfmt(line)=CurPfmtId;
    else                      pfmt(line)=DEFAULT_PFMT;

    if (line>0) CurCellId=cid(line-1);
    else        CurCellId=0;
    if (CurCellId<TotalCells) cid(line)=CurCellId;
    else                      cid(line)=0;

    if (line>0) ParaFID=fid(line-1);
    else        ParaFID=0;
    if (ParaFID<TotalParaFrames) fid(line)=ParaFID;
    else                         fid(line)=0;

    if (line>0 && fid(line)==fid(line-1)) {
       LineY(line)=LineY(line-1)+LineHt(line-1);
       LineX(line)=LineX(line-1);
    }
    JustAdjX(line)=0;          // x adjustment for center/rightjust

    if (line>0) LinePage(line)=LinePage(line-1);
    else        LinePage(line)=0;

    return TRUE;
}

/*****************************************************************************
    CloneLinePtr:
    Create a line pointer array.
******************************************************************************/
struct StrLinePtr far **CloneLinePtr(PTERWND w)
{
    long l;
    struct StrLinePtr far **ptr;

    ptr=OurAlloc((MaxLines+1)*sizeof(LPVOID));

    for (l=0;l<TotalLines;l++) {
       ptr[l]=CloneLine(w,l);
       if (ptr[l]==null) return (LPVOID)null;
    } 

    return ptr;
}

/*****************************************************************************
    FreeClonedLinePtr:
    Freeup the cloned line pointer object
******************************************************************************/
FreeClonedLinePtr(PTERWND w,struct StrLinePtr far **ptr, long LineCount)
{
   long l;

    for (l=0;l<LineCount;l++) FreeClonedLine(w,ptr[l]);

    OurFree(ptr);

    return true;
}

/*****************************************************************************
    CloneLine:
    Create a line object.
******************************************************************************/
struct StrLinePtr far *CloneLine(PTERWND w,long line)
{
    struct StrLinePtr *ptr;
    int len;

    ptr=MemAlloc(sizeof(struct StrLinePtr));
    if (NULL==ptr) return (LPVOID)PrintError(w,MSG_OUT_OF_MEM,"AllocLinePtr");

    FarMove((LinePtr[line]),ptr,sizeof(struct StrLinePtr));

    // allocate pointers
    len=LineLen(line);

    ptr->pText=MemAllocCopy(pText(line),len);
    if (cfmt(line).info.type!=UNIFORM)  ptr->cfmt.pFmt=MemAllocCopy((LPBYTE)cfmt(line).pFmt,len*sizeof(WORD));
    if (pCtid(line)!=null)  ptr->pCtid=MemAllocCopy((LPBYTE)pCtid(line),len*sizeof(WORD));
    if (pLead(line)!=null)  ptr->pLead=MemAllocCopy((LPBYTE)pLead(line),len);
    if (pCharWidth(line)!=null)  ptr->pCharWidth=MemAllocCopy((LPBYTE)pCharWidth(line),len*sizeof(WORD));
    
    if (tabw(line)!=null) {
       ptr->tabw=MemAllocCopy((LPBYTE)tabw(line),sizeof(struct StrTabw));
       FarMove(tabw(line),ptr->tabw,sizeof(struct StrTabw));
       if (tabw(line)->CharFlags) ptr->tabw->CharFlags=MemAllocCopy((LPBYTE)tabw(line)->CharFlags,len);
       if (tabw(line)->ListText) ptr->tabw->ListText=MemAllocCopy((LPBYTE)tabw(line)->ListText,lstrlen(tabw(line)->ListText)+1);
       if (tabw(line)->pListnum) ptr->tabw->pListnum=MemAllocCopy((LPBYTE)tabw(line)->pListnum,sizeof(struct StrListnum));
       if (tabw(line)->pAutoNumLgl) ptr->tabw->pAutoNumLgl=MemAllocCopy((LPBYTE)tabw(line)->pAutoNumLgl,lstrlen(tabw(line)->pAutoNumLgl)+1);
    }

    return ptr;
}


/*****************************************************************************
    FreeLine:
    Freeup memory used by a test line.
******************************************************************************/
FreeLine(PTERWND w,long line)
{
    if (LinePtr[line]==NULL) return TRUE;   // line alread freed up

    if (LineLen(line)!=0) {
       sFree(pText(line));  // release memory
       if (cfmt(line).info.type!=UNIFORM) sFree(cfmt(line).pFmt);
       if (pCtid(line)) sFree(pCtid(line));
       if (pLead(line)) sFree(pLead(line));
       if (pCharWidth(line)) sFree(pCharWidth(line));
    }
    if (tabw(line)) {
       if (tabw(line) && tabw(line)->type&INFO_SECT) SectModified=PosPageHdrFtr=TRUE;
       FreeTabwMembers(w,tabw(line));
       MemFree(tabw(line));
       tabw(line)=0;
    }
    FreeLinePtr(w,line);    // free the line pointer

    return TRUE;
}

/*****************************************************************************
    FreeClonedLine:
    Freeup the cloned line.
******************************************************************************/
FreeClonedLine(PTERWND w,struct StrLinePtr far *ptr)
{
    if (ptr==NULL) return TRUE;   // line alread freed up

    if (ptr->LineLen!=0) {
       if (ptr->pText) sFree(ptr->pText);  // release memory
       if (ptr->cfmt.info.type!=UNIFORM) sFree(ptr->cfmt.pFmt);
       if (ptr->pCtid) sFree(ptr->pCtid);
       if (ptr->pLead) sFree(ptr->pLead);
       if (ptr->pCharWidth) sFree(ptr->pCharWidth);
    }
    if (ptr->tabw) {
       FreeTabwMembers(w,ptr->tabw);
       MemFree(ptr->tabw);
       ptr->tabw=0;
    }
    OurFree(ptr);

    return TRUE;
}



