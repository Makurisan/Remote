/*==============================================================================
   TER_SPL.C
   TER Spell Checker interface.

   SpellTime &
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

/*****************************************************************************
    Spell.h variables
******************************************************************************/
#define ST_DISPLAY_LEN   15

#define ST_GET_ALTERNATES 0x1

#define ST_INTERACTIVE   0x1
#define ST_BEEP          0x2

#define ST_IGNORE        0x1
#define ST_REPLACE       0x2
#define ST_REFRESH       0x4
#define ST_TOO_LONG      0x8
#define ST_ERROR         0x10
#define ST_EXIT          0x20
#define ST_ADD           0x40
#define ST_INPUT         0x80
#define ST_ALL           0x100

/*****************************************************************************
    Global variables specific to this module only
******************************************************************************/
static int  WordIndex,                 // position of the current word in the current line
            WordLen;                   // current word length

/******************************************************************************
    SearchSpellTime:
    Search for the SpellTime product and find the function pointers.
******************************************************************************/
SearchSpellTime(PTERWND w)
{

    StSearched=TRUE;         // spelltime search complete
    hSpell=TerLoadDll("SPELL32.DLL");

    if (!hSpell) return false;

    // find the function pointers
    #if defined (WIN32)
       StParseLine=(LPVOID)GetProcAddress(hSpell,"StParseLine");
       StResetUserDict=(LPVOID)GetProcAddress(hSpell,"StResetUserDict");
       StClearHist=(LPVOID)GetProcAddress(hSpell,"StClearHist");
       SpellWord=(LPVOID)GetProcAddress(hSpell,"SpellWord");
       StSetDictName=(LPVOID)GetProcAddress(hSpell,"StSetDictName");
       ToSpellHist=(LPVOID)GetProcAddress(hSpell,"ToSpellHist");
       ToUserDict=(LPVOID)GetProcAddress(hSpell,"ToUserDict");
    #else
       StParseLine=(LPVOID)GetProcAddress(hSpell,"STPARSELINE");
       StResetUserDict=(LPVOID)GetProcAddress(hSpell,"STRESETUSERDICT");
       StClearHist=(LPVOID)GetProcAddress(hSpell,"STCLEARHIST");
       SpellWord=(LPVOID)GetProcAddress(hSpell,"SPELLWORD");
       StSetDictName=(LPVOID)GetProcAddress(hSpell,"STSETDICTNAME");
       ToSpellHist=(LPVOID)GetProcAddress(hSpell,"TOSPELLHIST");
       ToUserDict=(LPVOID)GetProcAddress(hSpell,"TOUSERDICT");
    #endif

    if (!StParseLine || !StResetUserDict || !StClearHist
        || !SpellWord || !ToSpellHist || !ToUserDict) {
       FreeLibrary(hSpell);
       hSpell=NULL;
    }
     
    return TRUE;
}

/******************************************************************************
    TerSpellCheckerPopped:
    Returns TRUE if the spell-checker menu popped on previous right mouse click
******************************************************************************/
BOOL WINAPI _export TerSpellCheckerPopped(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return SpellCheckerPopped;
}

/******************************************************************************
    DoAutoSpellCheck:
    This function returns TRUE when auto spell checking is turned on
******************************************************************************/
BOOL DoAutoSpellCheck(PTERWND w)
{
   return (TerArg.WordWrap && !(TerArg.ReadOnly) && hSpell && (TerFlags4&TFLAG4_AUTO_SPELL));
}

/******************************************************************************
    TerSpellCheckCurWord:
    Show popup for spell checking the current word
******************************************************************************/
BOOL WINAPI _export TerSpellCheckCurWord(HWND hWnd, LPARAM lParam)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return SpellCheckCurWordPart1(w,lParam);
}
 
/******************************************************************************
    SpellCheckCurWordPart1:
    Show popup for spell checking the current word
******************************************************************************/
BOOL SpellCheckCurWordPart1(PTERWND w, LPARAM lParam)
{
    BYTE CurWord[MAX_WIDTH+1];
    int  WordIdx,WordLen;
    int  x,y,i,id;
    long l;

    // delete any existing popup menu
    if (hPopup) {
      DestroyMenu(hPopup);
      hPopup=NULL;
    } 

    // extract the misspelled word
    TerMousePos(w,lParam,TRUE);          // get mouse position in MouseCol and MouseLine variables
    SpellLine=MouseLine;                 // save for the SpellCheckCurWordPartII function
    SpellCol=MouseCol;

    if (!GetMisspelledWord(w,MouseLine,MouseCol,CurWord,&WordIdx,&WordLen)) return FALSE; // no misspelled word at this location
    
    if (SpellWord(CurWord,0,hTerWnd,&SplResult)) {  // may be this misspelling was ingored in another TE window
       for (l=0;l<TotalLines;l++) ResetLongFlag(LineFlags2(l),LFLAG2_SPELL_CHECKED);  // check this doc again
       InvalidateRect(hTerWnd,NULL,TRUE);
       return FALSE;
    }

    if (SplResult.TotalAltWords>6) SplResult.TotalAltWords=6;   // show only 6 alternate words

    // find the x/y location to show the popup menu
    TerTextPosToPix(hTerWnd,REL_SCREEN,MouseLine,WordIdx+WordLen,&x,&y);


    hPopup=CreatePopupMenu();    // create a popup menu

    // append the alternate words
    id=IDP_FIRST_WORD;
    for (i=0;i<SplResult.TotalAltWords;i++,id++) AppendMenu(hPopup,MF_ENABLED|MF_STRING,id,SplResult.AltWord[i]);
    
    if (SplResult.TotalAltWords>0) AppendMenu(hPopup,MF_ENABLED|MF_SEPARATOR,0,"-"); // separation line

    // add ignore and add options
    AppendMenu(hPopup,MF_ENABLED|MF_STRING,IDP_IGNORE,MsgString[MSG_IGNORE_ALL]); // ignore this word for spell checking
    AppendMenu(hPopup,MF_ENABLED|MF_STRING,IDP_ADD,MsgString[MSG_ADD]);        // add to the user dictionary

    SpellCheckerPopped=TRUE;                                       // popmenu now showing
    TrackPopupMenu(hPopup,TPM_LEFTALIGN|TPM_RIGHTBUTTON,x,y+PrtToScrY(LineHt(MouseLine)),0,hTerWnd,NULL);
     
    return TRUE;
}
 
/******************************************************************************
    SpellCheckCurWordPart2:
    Apply popup selection for spell checking one word
******************************************************************************/
BOOL SpellCheckCurWordPart2(PTERWND w, int CmdId)
{
    BYTE CurWord[MAX_WIDTH+1];
    int  WordIdx,WordLen,SegLen=0,LineIndex=0;
    long l;
    int OrigUndoRef=UndoRef;

    if (!GetMisspelledWord(w,SpellLine,SpellCol,CurWord,&WordIdx,&WordLen)) return FALSE; // no misspelled word at this location
    
    // replace the word
    if (CmdId>=IDP_FIRST_WORD && CmdId<=IDP_LAST_WORD) {
       SplReplaceWord(w,CurWord,SpellLine,WordIdx,&LineIndex,&SegLen,SplResult.AltWord[CmdId-IDP_FIRST_WORD],OrigUndoRef);
       ResetLongFlag(LineFlags2(SpellLine),LFLAG2_SPELL_CHECKED);  // check this line again
       StTextToUnicode(w,SpellLine);                               // convert any regular text to unicode
    } 
    else if (CmdId==IDP_IGNORE || CmdId==IDP_ADD) {
       lstrlwr(w,CurWord);          // requirement for ToSpellHist and ToUserDict
       if (CmdId==IDP_IGNORE) ToSpellHist(CurWord,'I',"");
       else {
          ToUserDict(CurWord);
          StResetUserDict(NULL,NULL);      // save changes
       }  
       for (l=0;l<TotalLines;l++) ResetLongFlag(LineFlags2(l),LFLAG2_SPELL_CHECKED);  // check this line again
    }  
    
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    AutoSpellCheck:
    This function auto spell a given line.
******************************************************************************/
AutoSpellCheck(PTERWND w, long LineNo)
{
    int i,len,CurFont,PrevFont=-1,NewFont,IgnoreFont=-1,TextFont=-1;
    int WordLen,WordIndex,LineIndex;
    LPWORD fmt;
    LPBYTE ptr,lead,XlatePtr=null;
    BYTE CurWord[MAX_WIDTH+1];
    struct StrStResult StResult;        // result from 'SpellWord'
    BOOL EditWordSkipped=FALSE;
     
    // reset the autospell attribute
    len=LineLen(LineNo); 
    if (len==0) return TRUE;
    
    fmt=OpenCfmt(w,LineNo);
    PrevFont=-1;
    for (i=0;i<len;i++) {
       CurFont=fmt[i];
       if (TerFont[CurFont].flags&FFLAG_AUTO_SPELL) {
          if (CurFont!=PrevFont) {    // reset spell check attribute
             NewFont=SetFontFlags(w,CurFont,FFLAG_AUTO_SPELL,FALSE);  // turn-off the flag
             PrevFont=CurFont;
          }
          fmt[i]=(WORD)NewFont; 
       } 
    } 
    

    // ******** Spell check the given line *****************************
    OpenTextPtr(w,LineNo,&ptr,&lead);  // lock the current line
    
    // convert the unicode characters
    if (pStFromUniChar!=null) {    // unicode to byte conversion table available
       WORD uc;

       len=LineLen(LineNo);
       XlatePtr=MemAlloc(len+1);
       for (i=0;i<len;i++) {
          XlatePtr[i]=ptr[i];

          CurFont=fmt[i];
          if (TerFont[CurFont].UcBase==0) continue;
          if (TerFont[CurFont].CharSet!=StCharSet) continue;

          uc=MakeUnicode(w,TerFont[CurFont].UcBase,ptr[i]);
          if (uc>=StFirstUniChar && uc<=StLastUniChar) XlatePtr[i]=pStFromUniChar[uc-StFirstUniChar];  // converted
       }
       ptr=XlatePtr;   // now use this pointer 
    } 
    
    // find incorrect words
    WordIndex=LineIndex=0;
    
    while ((WordLen=StParseLine(ptr,CurWord,&WordIndex,&LineIndex,LineLen(LineNo)))>0) {  // process each word
       // check if this word has fonts to be ignored
       len=lstrlen(CurWord);
       for (i=0;i<len;i++) {
          CurFont=fmt[WordIndex+i];
          if (CurFont==0 || CurFont==TextFont) continue;      // this is never the hidden font
          if (CurFont==IgnoreFont) break;
          if (  TerFont[CurFont].FieldId==FIELD_NAME || HiddenText(w,CurFont) 
             || TerFont[CurFont].style&(PICT|PROTECT|SUPSCR|SUBSCR) 
             || (TerFont[CurFont].UcBase>1 && TerFont[CurFont].CharSet!=StCharSet)
             || TerFont[CurFont].rtl || (mbcs && lead[i])) {
             IgnoreFont=CurFont;         // to save calculation the next time
             break;
          }
          else TextFont=CurFont;         // acceptable font for spell checking
       }
       if (i>0) {                        // spell check part of the word
          CurWord[i]=0;
          LineIndex-=(WordLen-i);        // start parse position for the next loop
          WordLen=i;
       } 
       else continue;                   // ignore this word
       
       // check if this word is being edited
       if (EditLine>=0) {
          BOOL IsEditWord=(EditLine==LineNo && EditCol>=WordIndex && EditCol<=(WordIndex+WordLen));
          BOOL IsCurWord=(CurLine==LineNo && CurCol>=WordIndex && CurCol<=(WordIndex+WordLen));


          if (IsEditWord) {              // word being edited
             if (IsCurWord) {            // cursor on the word being edited
                EditWordSkipped=TRUE;
                EditWordIndex=WordIndex;       // column index of the word being edited
                EditWordLen=WordLen;
                continue;
             }
             else EditLine=-1;               // cursor moved off the word being edited
          }
       } 

       // spell check the word
       if (!SpellWord(CurWord,0,hTerWnd,&StResult)) {  // mis-spelled word
          if (StResult.code&ST_ERROR) break;
       
          // set the mis-spell font
          PrevFont=-1;
          for (i=0;i<WordLen;i++) {
             CurFont=fmt[WordIndex+i];
             if (CurFont!=PrevFont) {    // reset spell check attribute
                NewFont=SetFontFlags(w,CurFont,FFLAG_AUTO_SPELL,TRUE);  // turn-off the flag
                PrevFont=CurFont;
             }
             fmt[WordIndex+i]=(WORD)NewFont; 
          } 
       }
    }

    CloseTextPtr(w,LineNo);
    CloseCfmt(w,LineNo);

    if (XlatePtr) MemFree(XlatePtr);  // xlated pointer memory

    if (EditWordSkipped) SpellPending=TRUE;
    else  {
        LineFlags2(LineNo)|=LFLAG2_SPELL_CHECKED;  // this line spell checked
        if (!WordBeingEdited(w)) EditLine=-1;
    }
    
    return TRUE; 
}
  
/******************************************************************************
    WordBeingEdited:
    This function returns TRUE if the cursor is placed on a word being edited
******************************************************************************/
BOOL WordBeingEdited(PTERWND w)
{
     BOOL IsEditWord,IsCurWord;

     if (EditLine!=CurLine) return FALSE;

     IsEditWord=(EditCol>=EditWordIndex && EditCol<=(EditWordIndex+EditWordLen));
     IsCurWord=(CurCol>=EditWordIndex && CurCol<=(EditWordIndex+EditWordLen));

     return (IsEditWord && IsCurWord);
}
 
/******************************************************************************
    GetMisspelledWord
    This function returns TRUE the misspelled word at the specified location
******************************************************************************/
BOOL GetMisspelledWord(PTERWND w, long LineNo, int col, LPBYTE word, LPINT pWordIdx, LPINT pWordLen)
{
    LPWORD fmt;
    int i,len,CurFont,WordIdx,WordLen;

    if (LineNo<0 || LineNo>=TotalLines) return FALSE;
    len=LineLen(LineNo);
    if (col<0 || col>=len) return FALSE;

    fmt=OpenCfmt(w,LineNo);
    CurFont=fmt[col];
    if (!(TerFont[CurFont].flags&FFLAG_AUTO_SPELL)) return FALSE;  // not a misspelled word

    // find the beginning of the word
    for (i=col-1;i>=0;i--) {
       CurFont=fmt[i];
       if (!(TerFont[CurFont].flags&FFLAG_AUTO_SPELL)) break;
    }
    WordIdx=i+1;
 
    WordLen=0;
    for (i=WordIdx;i<len;i++,WordLen++) {
       CurFont=fmt[i];
       if (!(TerFont[CurFont].flags&FFLAG_AUTO_SPELL)) break;
    }

    // extract the word if requested
    if (word) {
       LPBYTE ptr=pText(LineNo);
       for (i=0;i<WordLen;i++) {
          word[i]=ptr[WordIdx+i];

          // convert unicode to regular text
          CurFont=fmt[WordIdx+i];
          if (pStFromUniChar!=null && TerFont[CurFont].CharSet==StCharSet && TerFont[CurFont].UcBase>1) {
             WORD uc=MakeUnicode(w,TerFont[CurFont].UcBase,word[i]);
             if (uc>=StFirstUniChar && uc<=StLastUniChar) word[i]=pStFromUniChar[uc-StFirstUniChar];
          } 
       }
       word[i]=0;
    } 

    CloseCfmt(w,LineNo);

    if (pWordIdx) (*pWordIdx)=WordIdx;
    if (pWordLen) (*pWordLen)=WordLen;

    return TRUE;
}
 
/******************************************************************************
    TerSpellCheck:
     1. Determine the range of lines to spell check.
     2. Spell check each line in the specified range.  The misspelled words
        are highlighted and the user is allowed to provide a correction.
******************************************************************************/
BOOL WINAPI _export TerSpellCheck(HWND hWnd, BOOL StopAfterFirst, BOOL msg)
{
   return TerSpellCheck2(hWnd,StopAfterFirst,msg,NULL);
}
 
BOOL WINAPI _export TerSpellCheck2(HWND hWnd, BOOL StopAfterFirst, BOOL msg, BOOL *pCancelled)
{
     PTERWND w;
     long BegRow,EndRow;
     int  BegCol,EndCol;
     int  i,LineIndex,SegLen,len;
     LPBYTE ptr,lead;
     char  string[80];
     long TotalWords=0,WrongWords=0;
     BYTE CurWord[ST_MAX_WORD_LEN+1];
     HCURSOR hSaveCursor=NULL;
     struct StrStResult StResult;        // result from 'SpellWord'
     int  IgnoreFont=-1,CurFont,TextFont=-1;
     LPWORD fmt;
     DWORD SaveAutoRepage;
     int OrigUndoRef;
     BOOL xlated;

     if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

     if (pCancelled) (*pCancelled)=false;

     if (!hSpell) return FALSE;          // spelltime product not found

     OrigUndoRef=UndoRef;

    //************ determine the range of the text to spell check **********
    if (HilightType==HILIGHT_LINE) {         // line block
        NormalizeBlock(w);                   // normalize beginning and ending of the block
        BegRow=HilightBegRow;
        BegCol=0;
        EndRow=HilightEndRow;
        EndCol=LineLen(EndRow);
        if (EndCol<0) EndCol=0;
        HilightType=HILIGHT_OFF;             // reset the line block
        StretchHilight=FALSE;
    }
    else if (HilightType==HILIGHT_CHAR) {  // character block
        NormalizeBlock(w);                   // normalize beginning and ending of the block
        BegRow=HilightBegRow;
        BegCol=HilightBegCol;
        if (BegCol>=LineLen(BegRow)) BegCol=LineLen(BegRow)-1;
        if (BegCol<0) BegCol=0;

        EndRow=HilightEndRow;
        EndCol=HilightEndCol-1;
        if (EndCol>=LineLen(EndRow)) EndCol=LineLen(EndRow)-1;
        if (EndCol<0) EndCol=0;
        if (BegRow==EndRow && BegCol>EndCol) BegCol=EndCol;
        HilightType=HILIGHT_OFF;             // reset the line block
        StretchHilight=FALSE;
    }
    else {                                  // if text not highlighted, check the entire file
        BegRow=0;
        BegCol=0;
        EndRow=TotalLines-1;
        if (EndRow<0) EndRow=0;
        EndCol=LineLen(EndRow);
        if (EndCol<0) EndCol=0;
    }

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);   // show hour glass while reading

    // set the no-auto-repage flag
    SaveAutoRepage=TerFlags2&TFLAG2_NO_AUTO_REPAGE;
    TerFlags2|=TFLAG2_NO_AUTO_REPAGE;
    TerOpFlags2|=TOFLAG2_NO_WRAP;

    if (!(TerFlags5&TFLAG5_NO_CLEAR_SPL_HIST)) StClearHist();                        // clear the SpellTime history buffer

    for (CurLine=BegRow;CurLine<=EndRow;CurLine++) {         // spell check each line
        if (LineLen(CurLine)==0) continue;

        xlated=StUnicodeToText(w,CurLine);  // convert any unicode character to regular text

        OpenTextPtr(w,CurLine,&ptr,&lead);  // lock the current line

        LineIndex=0;                      // number of characters checked in the current line

        while (TRUE) {
           if (mbcs) while (LineIndex<LineLen(CurLine) && lead[LineIndex]) LineIndex++;  // skip over dbc
           if (LineIndex>=LineLen(CurLine)) break;
           if (mbcs) {
              SegLen=LineIndex+1;
              while (SegLen<LineLen(CurLine) && lead[SegLen]==0) SegLen++;
           }
           else SegLen=LineLen(CurLine);

           while ((WordLen=StParseLine(ptr,CurWord,&WordIndex,&LineIndex,SegLen))>0) {  // process each word
              if (CurLine==BegRow && WordIndex<BegCol) continue;
              if (CurLine==EndRow && WordIndex>EndCol) continue;

              // check if this word has fonts to be ignored
              fmt=OpenCfmt(w,CurLine);
              len=lstrlen(CurWord);
              for (i=0;i<len;i++) {
                 CurFont=fmt[WordIndex+i];
                 if (CurFont==0 || CurFont==TextFont) continue;      // this is never the hidden font
                 if (CurFont==IgnoreFont) break;

                 if (  TerFont[CurFont].FieldId==FIELD_NAME || HiddenText(w,CurFont) 
                    || TerFont[CurFont].style&(PICT|PROTECT|SUPSCR|SUBSCR) || TerFont[CurFont].UcBase
                    /*|| TerFont[CurFont].rtl*/) {
                    IgnoreFont=CurFont;         // to save calculation the next time
                    break;
                 }
                 else TextFont=CurFont;         // acceptable font for spell checking

              }
              CloseCfmt(w,CurLine);
              if (i>0) {                        // spell check part of the word
                 CurWord[i]=0;
                 LineIndex-=(WordLen-i);        // start parse position for the next loop
                 len=WordLen=i;
              } 
              else continue;                   // ignore this word


              /* Check once without user interaction, and if word is found
                 misspelled, then call the 'SpellWord' function again with
                 ST_INTERACTIVE flag */

              if (!SpellWord(CurWord,0,hTerWnd,&StResult)) {  // mis-spelled word
                 if (StResult.code&ST_ERROR) goto END;

                 if (StopAfterFirst) return FALSE;

                 if (hSaveCursor) SetCursor(hSaveCursor);        // display regular cursor

                 SplHilightWord(w);                              // highlight the misspelled word

                 SpellWord(CurWord,ST_INTERACTIVE,hTerWnd,&StResult);

                 if (StResult.code&(ST_IGNORE|ST_REPLACE|ST_ADD|ST_INPUT)) {
                     WrongWords++;
                 }
                 else if (StResult.code&(ST_EXIT|ST_ERROR)) {
                    if (StResult.code&ST_EXIT && pCancelled) (*pCancelled)=true;
                    goto END;
                 }

                 if (StResult.replace[0]) {
                                // unlock the current line
                    SplReplaceWord(w,CurWord,CurLine,WordIndex,&LineIndex,&SegLen,StResult.replace,OrigUndoRef);
                    if (!mbcs) SegLen=LineLen(CurLine);
                    OpenTextPtr(w,CurLine,&ptr,&lead);   // lock again
                 }
                 SetCursor(hWaitCursor);   // resume
              }
              TotalWords++;
           }
           if (SegLen==LineLen(CurLine)) break;
           LineIndex=SegLen;
        }
        CloseTextPtr(w,CurLine);
        
        if (xlated) StTextToUnicode(w,CurLine);   // convert text to unicode

        ResetLongFlag(LineFlags2(CurLine),LFLAG2_SPELL_CHECKED);  // recheck this line for auto-spell
    
    }
    CurLine=EndRow;

    END:
    // update the user dictionary file
    StResetUserDict(NULL,NULL);

    if (hSaveCursor) SetCursor(hSaveCursor);
    HilightType=HILIGHT_OFF;

    // restore the auto-repage flag
    if (!SaveAutoRepage) ResetLongFlag(TerFlags2,TFLAG2_NO_AUTO_REPAGE);
    ResetLongFlag(TerOpFlags2,TOFLAG2_NO_WRAP);

    if ((CurLine-BeginLine)>=(WinHeight-1) || (CurLine-BeginLine)<0) {
       BeginLine=CurLine-(WinHeight)/2;    // position the current toward the center
       if (BeginLine<0) BeginLine=0;
    }
    CurRow=CurLine-BeginLine;
    CurCol=0;
    PaintFlag=PAINT_WIN;
    PaintTer(w);

    wsprintf(string,MsgString[MSG_SPL_DONE],TotalWords,WrongWords);
    if (msg && !StopAfterFirst) MessageBox(hTerWnd,string,MsgString[MSG_SPELL_OVER],MB_OK);

    return (WrongWords==0);
}

/******************************************************************************
    SplHilightWord:
    This routine highlight the misspelled word.  The word location is given
    by these variables: CurLine -> Current line number, WordIndex -> word position
    within the current line, WordLen -> length of the misspelled word.
******************************************************************************/
void SplHilightWord(PTERWND w)
{
    HilightType=HILIGHT_CHAR;             // highlight the misspelled word
    HilightBegRow=HilightEndRow=CurLine;  // highlight the misspelled word
    CurCol=HilightBegCol=WordIndex;
    HilightEndCol=WordIndex+WordLen;

    if ((CurLine-BeginLine)>WinHeight/2 || (CurLine-BeginLine)<0) {
       BeginLine=CurLine-(WinHeight)/4;   // position toward top
       if (BeginLine<0) BeginLine=0;
    }

    CurRow=CurLine-BeginLine;
    WrapFlag=WRAP_OFF;                    // refresh from the text

    TerOpFlags|=TOFLAG_NO_ADJUST_POS;     // do no adjust the cursor position
    if (!CaretEngaged) TerEngageCaret(hTerWnd,true);    // needed to show the current line on the screen
    
    PaintTer(w);

    ResetLongFlag(TerOpFlags,TOFLAG_NO_ADJUST_POS);
}

/******************************************************************************
    SplReplaceWord:
    This routine replaces an misspelled word from the current line by a replacement
    word.  The line length and current line pointer variables
    are adjusted accordingly.
******************************************************************************/
SplReplaceWord(PTERWND w,LPBYTE CurWord,long LineNo, int WordIndex,LPINT LineIndex,LPINT SegLen, LPBYTE NewWord, int OrigUndoRef)
{
   int OldLen,NewLen,AdjLen,i;
   LPBYTE ptr;


   OldLen=lstrlen(CurWord);
   NewLen=lstrlen(NewWord);
   AdjLen=NewLen-OldLen;

   if (AdjLen!=0) {                    // make necessary adjustments
      if ((LineLen(LineNo)+AdjLen)>(LineWidth-1)) return FALSE;
      *LineIndex=(*LineIndex)+AdjLen;  // adjust the current word pointer within the current line
      *SegLen=(*SegLen)+AdjLen;        // adjust the length of the segment
   }

   // Save undo before deletion
   UndoRef=OrigUndoRef;
   SaveUndo(w,LineNo,WordIndex,LineNo,WordIndex+OldLen,'D');

   if (AdjLen<0) MoveLineData(w,LineNo,WordIndex,abs(AdjLen),'D'); // delete character
   
   if (AdjLen>0) MoveLineData(w,LineNo,WordIndex,abs(AdjLen),'B'); // insert spaces

   ptr=pText(LineNo);
   for(i=0;i<NewLen;i++) ptr[WordIndex+i]=NewWord[i];    // substitute the new word

   // Save undo after insertion
   UndoRef=OrigUndoRef;
   SaveUndo(w,LineNo,WordIndex,LineNo,WordIndex+NewLen,'I');

   ResetLongFlag(LineFlags2(LineNo),LFLAG2_SPELL_CHECKED);  // recheck this line for auto-spell
   TerArg.modified++;

   return TRUE;
}

/******************************************************************************
    StUnicodeToText:
    convert unicode to text.
******************************************************************************/
BOOL StUnicodeToText(PTERWND w, long line)
{
    BOOL result=false;
    LPBYTE ptr,lead;
    LPWORD fmt;
    int i,len,CurFont;
    WORD uc;

    if (StCharSet==ANSI_CHARSET || StCharSet==0 || pStFromUniChar==null) return false;

    OpenTextPtr(w,line,&ptr,&lead);  // lock the current line
    fmt=OpenCfmt(w,line);

    len=LineLen(line);

    for (i=0;i<len;i++) {      // convert each character
       CurFont=fmt[i];
       if (TerFont[CurFont].UcBase<=1) continue;
       if (TerFont[CurFont].CharSet!=StCharSet) continue;

       uc=MakeUnicode(w,TerFont[CurFont].UcBase,ptr[i]);

       if (uc>=StFirstUniChar && uc<=StLastUniChar) {
          ptr[i]=pStFromUniChar[uc-StFirstUniChar];
          fmt[i]=ResetUniFont(w,fmt[i]);
          result=true;
       } 
    } 

    CloseTextPtr(w,line);
    CloseCfmt(w,line);

    return result;
}

/******************************************************************************
    StTextToUnicode:
    convert regulsr text to unicode
******************************************************************************/
BOOL StTextToUnicode(PTERWND w, long line)
{
    BOOL result=false;
    LPBYTE ptr,lead;
    LPWORD fmt;
    int i,len,CurFont;
    WORD uc,UcBase;
    BYTE chr;

    if (False(TerFlags5&TFLAG5_INPUT_TO_UNICODE)) return false;

    if (StCharSet==ANSI_CHARSET || StCharSet==0 || pStFromUniChar==null) return false;

    OpenTextPtr(w,line,&ptr,&lead);  // lock the current line
    fmt=OpenCfmt(w,line);

    len=LineLen(line);

    for (i=0;i<len;i++) {      // convert each character
       CurFont=fmt[i];
       if (TerFont[CurFont].UcBase>1) continue;
       if (TerFont[CurFont].CharSet!=StCharSet) continue;

       chr=ptr[i];
       uc=StToUniChar[chr];
       if (uc==0) continue;      // converion not available
       if (uc<128) continue;

       CrackUnicode(w,uc,&UcBase,&(ptr[i]));
       fmt[i]=SetUniFont(w,fmt[i],UcBase);

       result=true;
    } 

    CloseTextPtr(w,line);
    CloseCfmt(w,line);

    return result;
}

/******************************************************************************
    TerSetStCharSet:
    Set the current character set for spell-checking operation.
******************************************************************************/
BOOL WINAPI _export TerSetStCharSet(HWND hWnd, int CharSet)
{
    PTERWND w;
    int i,CodePage,count;
    BYTE str[2];
    
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CharSet==ANSI_CHARSET || CharSet==SYMBOL_CHARSET) {  // clear the conversion table
       StCharSet=ANSI_CHARSET;
       if (pStFromUniChar!=NULL) MemFree(pStFromUniChar);
       pStFromUniChar=NULL;
       return true;
    } 

    // get code page
    StCharSet=(BYTE)CharSet;
    
    if (CharSet==DEFAULT_CHARSET) CodePage=GetACP();
    else IsMbcsCharSet(w,CharSet,&CodePage);

    // set the byte to unicode table
    FarMemSet(StToUniChar,0,sizeof(StToUniChar));

    StFirstUniChar=0;
    StLastUniChar=0;

    for (i=128;i<255;i++) {      // convert upper character - character 255 not used
       str[0]=i;
       str[1]=0;
       if (MultiByteToWideChar(CodePage,0,str,1,&(StToUniChar[i]),1)!=1) {
          StToUniChar[i]=0;
       }
       if (StToUniChar[i]!=0) {
          if (StFirstUniChar==0) {          // set the first unicode character
             StFirstUniChar=StToUniChar[i];
             StLastUniChar=StToUniChar[i];
          }
          if (StToUniChar[i]<StFirstUniChar) StFirstUniChar=StToUniChar[i];
          if (StToUniChar[i]>StLastUniChar) StLastUniChar=StToUniChar[i];
       } 
    } 

    // set the unicode to byte conversion table
    count=StLastUniChar-StFirstUniChar;
    pStFromUniChar=MemAlloc((count+1)*sizeof(BYTE));
    FarMemSet(pStFromUniChar,0,(count+1)*sizeof(BYTE));
    for (i=0;i<256;i++) {
       if (StToUniChar[i]!=0) {
          pStFromUniChar[StToUniChar[i]-StFirstUniChar]=i;
       } 
    } 

    return TRUE;
}
