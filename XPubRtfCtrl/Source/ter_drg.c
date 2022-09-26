/*==============================================================================
   TER_DRG.C
   Support for Dragon speech libaray in the regular Edit mode

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

#include "richedit.h"

/****************************************************************************
    DoDragonMsg:
    Handle message used by dragon speech library.
****************************************************************************/
BOOL DoDragonMsg(PTERWND w, HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam, LRESULT *pResult)
{
    DWORD lresult=0;
    BOOL result=true;
    int  SaveModified=TerArg.modified;
    bool debug=false;
    typedef struct _charrange
    {
     LONG cpMin;
     LONG cpMax;
    } CHARRANGE;

    switch (message) {
        case WM_CLEAR:        // delete selected text
        if (debug) OurPrintf("WM_CLEAR");
        {
           if (HilightType!=HILIGHT_OFF) TerDeleteBlock(hWnd,TRUE); 
           break;
        }  

        case WM_GETTEXTLENGTH:        // get text length in characters
        if (debug) OurPrintf("WM_GETTEXTLENGTH");
        {
           long len;
           HGLOBAL hMem;
           int SaveFormat=TerArg.SaveFormat;
           int SaveModified;

           if (True(GetWindowLong(hTerWnd,GWL_STYLE)&WS_CAPTION) && False(TerFlags6&TFLAG6_SUPPORT_DRAGON_SPEECH)) return false;

           if (hGetTextMem && GetTextLen>0 && GetTextMod==TerArg.modified) len=GetTextLen;  // data not changed use previous
           else {
              if (hGetTextMem) GlobalFree(hGetTextMem);  // release any previous get text mem data
               
              SaveModified=TerArg.modified;
              TerArg.SaveFormat=SAVE_TEXT;
              
              hMem=GetTerBuffer(hWnd,&len);
              
              TerArg.SaveFormat=SaveFormat;
              TerArg.modified=SaveModified;              // restore modification status

              if (hMem==0) len=0;

              hGetTextMem=hMem;    // save for quick access next time
              GetTextLen=len;
              GetTextMod=TerArg.modified;
           }

           lresult=len;
           break;
        }  
         
        case WM_GETTEXT:        // get all text
        if (debug) OurPrintf("WM_GETTEXT");
        {
           long len=0;
           LPBYTE pMem;
           HGLOBAL hMem=null;
           int SaveFormat=TerArg.SaveFormat;
           int SaveModified;

           if (True(GetWindowLong(hTerWnd,GWL_STYLE)&WS_CAPTION) && False(TerFlags6&TFLAG6_SUPPORT_DRAGON_SPEECH)) return false;

           if (hGetTextMem && GetTextLen>0 && GetTextMod==TerArg.modified) {  // data not changed use previous
              hMem=hGetTextMem;   // use previous data
              len=GetTextLen;
           } 
           else {
              if (hGetTextMem) GlobalFree(hGetTextMem);  // release any previous get text mem data
              
              SaveModified=TerArg.modified;
              TerArg.SaveFormat=SAVE_TEXT;

              hMem=GetTerBuffer(hWnd,&len);
              
              TerArg.SaveFormat=SaveFormat;
              TerArg.modified=SaveModified;              // restore modification status

              if (hMem==NULL) len=0;
              
              hGetTextMem=hMem;    // save for quick access next time
              GetTextLen=len;
              GetTextMod=TerArg.modified;
           }
           
           if (len>0 && hMem!=null) {            // copy text to output
              pMem=GlobalLock(hMem);
              if (len>(long)wParam) len=(long)wParam; // max to be copied
              FarMove(pMem,(LPBYTE)lParam,len);
              GlobalUnlock(hMem);
           }


           lresult=len;
           break;
        }  
         
        case EM_GETSELTEXT:     // get selected text
        if (debug) OurPrintf("EM_GETSELTEXT");
        {
           long len=0;
           LPBYTE pMem;
           HGLOBAL hMem=TerGetTextSel(hWnd,&len);

           if (hMem!=NULL) {
              pMem=GlobalLock(hMem);
           
              FarMove(pMem,(LPBYTE)lParam,len);
           
              GlobalUnlock(hMem);
              GlobalFree(hMem);
           }
           else len=0;
           
           lresult=len;
           break;
        }  
         
        case EM_GETTEXTRANGE:     // get the range of characters
        if (debug) OurPrintf("EM_GETTEXTRANGE");
        {
           TEXTRANGE *pRange=(LPVOID)lParam;
           long len=0;
           LPBYTE pMem;
           HGLOBAL hMem=null;
           long start=pRange->chrg.cpMin;
           long end=pRange->chrg.cpMax;
           long EndLine,BegLine;
           int  BegCol,EndCol;

           DrgAbsToRowCol(w,start,&BegLine,&BegCol);
           DrgAbsToRowCol(w,end,&EndLine,&EndCol);
           SelectTerText(hWnd,BegLine,BegCol,EndLine,EndCol,true);  // can't use absolute because we need to convert assuming cr/lf
           
           hMem=TerGetTextSel(hWnd,&len);
           DeselectTerText(hWnd,false);

           if (hMem!=NULL) {
              pMem=GlobalLock(hMem);
           
              FarMove(pMem,pRange->lpstrText,len);
           
              GlobalUnlock(hMem);
              GlobalFree(hMem);
           }
           else len=0;
           
           lresult=len;
           break;
        }  
         

        case EM_GETSEL:        // get selection begin and end (exclusive) positions
        if (debug) OurPrintf("EM_GETSEL");
        {
           long start=-1,end=-1;

           if (HilightType!=HILIGHT_OFF) {
              start=DrgRowColToAbs(w,HilightBegRow,HilightBegCol);
              end=DrgRowColToAbs(w,HilightEndRow,HilightEndCol);     // position after the selection
           } 
           else {
              start=end=DrgRowColToAbs(w,CurLine,CurCol);
           } 

           if (wParam)   *((LPLONG)(wParam))=start;
           if (lParam)   *((LPLONG)(lParam))=end;

           if (start>65535 || end>65535) lresult=(DWORD)(long)-1;
           else lresult=((end)<16)  | start;

           break;
        }  

        case EM_EXGETSEL:        // get selection begin and end (exclusive) positions
        if (debug) OurPrintf("EM_EXGETSEL");
        {
           long start=-1,end=-1;
           CHARRANGE *pRange=(LPVOID)lParam;

           if (HilightType!=HILIGHT_OFF) {
              start=DrgRowColToAbs(w,HilightBegRow,HilightBegCol);
              end=DrgRowColToAbs(w,HilightEndRow,HilightEndCol);     // position after the selection
           } 
           pRange->cpMin=start;
           pRange->cpMax=end;

           break;
        }  

        case EM_SETSEL:        // select text
        if (debug) OurPrintf("EM_SETSEL");
        {
           long start=(long)wParam;
           long end=(long)lParam;
           long EndLine,BegLine;
           int  EndCol,BegCol;

           if (start==-1) DeselectTerText(hWnd,TRUE);      // deselect any text
           else if (start==0 && end==-1) {
              SetTerCursorPos(hWnd,-1,0,false);
              TerCommand(hWnd,ID_SELECT_ALL);
           }
           else {
              // set cursor position
              DrgAbsToRowCol(w,end,&EndLine,&EndCol);
              SetTerCursorPos(hWnd,EndLine,EndCol,false);

              // select text
              if (start>end) SwapInts(&((int)start),&((int)end));
              
              DrgAbsToRowCol(w,start,&BegLine,&BegCol);
              DrgAbsToRowCol(w,end,&EndLine,&EndCol);
              SelectTerText(hWnd,BegLine,BegCol,EndLine,EndCol,true);  // can't use absolute because we need to convert assuming cr/lf
           }     

           break;
        }  

        case EM_EXSETSEL:        // select text
        if (debug) OurPrintf("EM_EXSETSEL");
        {
           CHARRANGE *pRange=(LPVOID)lParam;
           long start=pRange->cpMin;
           long end=pRange->cpMax;
           long EndLine,BegLine;
           int  EndCol,BegCol;

           if (start==-1) DeselectTerText(hWnd,TRUE);      // deselect any text
           else if (start==0 && end==-1) {
              SetTerCursorPos(hWnd,-1,0,false);
              TerCommand(hWnd,ID_SELECT_ALL);
           }
           else {
              // set cursor position
              DrgAbsToRowCol(w,end,&EndLine,&EndCol);
              SetTerCursorPos(hWnd,EndLine,EndCol,false);

              // select text
              if (start>end) SwapInts(&((int)start),&((int)end));
              DrgAbsToRowCol(w,start,&BegLine,&BegCol);
              DrgAbsToRowCol(w,end,&EndLine,&EndCol);
              SelectTerText(hWnd,BegLine,BegCol,EndLine,EndCol,true);  // can't use absolute because we need to convert assuming cr/lf
           }     

           break;
        }  


        case EM_REPLACESEL:        // delete selected text
        if (debug) OurPrintf("EM_REPLACESEL");
        {
           int GrpUndoRef=UndoRef;
           long BegPos=-1;
           
           if (HilightType!=HILIGHT_OFF) {
              BegPos=DrgRowColToAbs(w,HilightBegRow,HilightBegCol);  
              TerDeleteBlock(hWnd,false);   // deleted any selected text before inserting new text
           }
           
           UndoRef=GrpUndoRef;   // connect undo
           if (BegPos!=-1) DrgAbsToRowCol(w,BegPos,&CurLine,&CurCol);
           InsertTerText(hWnd,(LPBYTE)lParam,TRUE);

           if (False(wParam)) TerFlushUndo(hWnd);  // repl
           
           break;
        }  

        case EM_GETLINECOUNT:        // get number of lines in the control
        if (debug) OurPrintf("EM_GETLINECOUNT");
        {
           if (PageModifyCount!=TerArg.modified) TerRepaginate(hWnd,TRUE); // repaginate 
           lresult=TotalLines;
           break;
        }  

        case EM_GETFIRSTVISIBLELINE:        // get first visible line
        if (debug) OurPrintf("EM_GETFIRSTVISIBLELINE");
        {
           long line;
           int col;
           TerPixToTextPos(hWnd,REL_TEXT_BOX,0,0,&line,&col);
           lresult=line;
           break;
        }  

        case EM_LINEINDEX:              // get the character position of the first character of the specified line
        if (debug) OurPrintf("EM_LINEINDEX");
        {
           long pos;
           long line=(long)wParam;
           
           if (line<0) line=CurLine;

           if (line<TotalLines) pos=DrgRowColToAbs(w,line,0);
           else                 pos=-1;

           lresult=(DWORD)pos;
           break;
        }  

        case EM_LINEFROMCHAR:        // get line number for a character position
        if (debug) OurPrintf("EM_LINEFROMCHAR");
        {
           long pos=(int)wParam;
           long line;
           int  col;

           if (pos<0) {
              if (HilightType==HILIGHT_OFF) line=CurLine;
              else                          line=HilightBegRow;
           }
           else {
              DrgAbsToRowCol(w,pos,&line,&col);
           }  
           lresult=(DWORD)line;
           break;
        }  

        case EM_POSFROMCHAR:             // implement like Edit control
        if (debug) OurPrintf("EM_POSFROMCHAR");
        {
           long pos=(long)wParam,line;
           int x,y,col,SpcBef,SpcAft;

           if (pos<0) {
              line=CurLine;
              col=CurCol;
           }
           else DrgAbsToRowCol(w,pos,&line,&col);

           TerTextPosToPix(hWnd,REL_WINDOW,line,col,&x,&y);

           // add any paragraph space before
           GetLineSpacing(w,line,0,&SpcBef,&SpcAft,TRUE);
           y+=SpcBef;

           lresult=(((DWORD)y)<<16) | (DWORD)x;
           break;
        }  

        case EM_SETREADONLY:        // set/reset read-only mode.
        if (debug) OurPrintf("EM_SETREADONLY");
        {
           BOOL ReadOnly=(BOOL)wParam;
           TerSetReadOnly(hWnd,TRUE);
           lresult=1;               // successful result
           break;
        }  

        case EM_SETEVENTMASK:      // set event mask
        if (debug) OurPrintf("EM_SETEVENTMASK");
        {
           lresult=EventMask;      // return previous event mask

           EventMask=(DWORD)lParam; // set new
           break;
        }  

        case EM_GETEVENTMASK:      // get event mask
        if (debug) OurPrintf("EM_GETEVENTMASK");
        {
           lresult=EventMask;      // return event mask
           break;
        }  

        case EM_GETCHARFORMAT:
        if (debug) OurPrintf("EM_GETCHARFORMAT");
        {
           CHARFORMAT *pFmt=(LPVOID)lParam;
           int font=0;
           if (wParam==SCF_SELECTION) {
              if (HilightType==HILIGHT_OFF) font=GetCurCfmt(w,CurLine,CurCol);
              else                          font=GetCurCfmt(w,HilightBegRow,HilightBegCol);
           }
            
           if (pFmt->cbSize==sizeof(CHARFORMAT)) {
               pFmt->dwMask=0xFFFFFFFF;    // output all values
               pFmt->dwEffects=0;
               if (TerFont[font].style&BOLD)    pFmt->dwEffects|=CFE_BOLD;
               if (TerFont[font].style&ITALIC)  pFmt->dwEffects|=CFE_ITALIC;
               if (TerFont[font].style&PROTECT) pFmt->dwEffects|=CFE_PROTECTED;
               if (TerFont[font].style&STRIKE)  pFmt->dwEffects|=CFE_STRIKEOUT;
               if (TerFont[font].style&ULINE)   pFmt->dwEffects|=CFE_UNDERLINE;

               pFmt->yHeight=TerFont[font].TwipsSize;
               pFmt->yOffset=TerFont[font].offset;
               pFmt->crTextColor=TerFont[font].TextColor;
               pFmt->bCharSet=TerFont[font].CharSet;
               pFmt->bPitchAndFamily=TerFont[font].FontFamily;
               lstrcpy(pFmt->szFaceName,TerFont[font].TypeFace);
           } 
           else if (pFmt->cbSize==sizeof(CHARFORMAT2)) {
               CHARFORMAT2 *pFmt=(LPVOID)lParam;
            
               pFmt->dwMask=0xFFFFFFFF;    // output all values
               pFmt->dwEffects=0;
               if (TerFont[font].style&BOLD)    pFmt->dwEffects|=CFE_BOLD;
               if (TerFont[font].style&ITALIC)  pFmt->dwEffects|=CFE_ITALIC;
               if (TerFont[font].style&PROTECT) pFmt->dwEffects|=CFE_PROTECTED;
               if (TerFont[font].style&STRIKE)  pFmt->dwEffects|=CFE_STRIKEOUT;
               if (TerFont[font].style&ULINE)   pFmt->dwEffects|=CFE_UNDERLINE;
               if (TerFont[font].style&CAPS)    pFmt->dwEffects|=CFE_ALLCAPS;
               if (TerFont[font].style&HIDDEN)  pFmt->dwEffects|=CFE_HIDDEN;
               if (TerFont[font].style&SCAPS)   pFmt->dwEffects|=CFE_SMALLCAPS;
               if (TerFont[font].style&SUBSCR)  pFmt->dwEffects|=CFE_SUBSCRIPT;
               if (TerFont[font].style&SUPSCR)  pFmt->dwEffects|=CFE_SUPERSCRIPT;
               if (TerFont[font].FieldId==FIELD_HLINK) pFmt->dwEffects|=CFE_LINK;

               pFmt->yHeight=TerFont[font].TwipsSize;
               pFmt->yOffset=TerFont[font].offset;
               pFmt->crTextColor=TerFont[font].TextColor;
               pFmt->bCharSet=TerFont[font].CharSet;
               pFmt->bPitchAndFamily=TerFont[font].FontFamily;
               lstrcpy(pFmt->szFaceName,TerFont[font].TypeFace);

               pFmt->wWeight=(True(TerFont[font].style&BOLD))?700:400;
               pFmt->sSpacing=TerFont[font].expand;
               pFmt->lcid=TerFont[font].lang;
               pFmt->bUnderlineType=CFU_UNDERLINE;
               pFmt->bRevAuthor=(TerFont[font].DelRev)?TerFont[font].DelRev:TerFont[font].InsRev;

               pFmt->dwReserved=0;
               pFmt->sStyle=0;
               pFmt->wKerning=0;
               pFmt->bAnimation=0;
           } 
           
           break;
        }

        case EM_EMPTYUNDOBUFFER:   // flush undo buffer
        if (debug) OurPrintf("EM_EMPTYUNDOBUFFER");
        {
           TerFlushUndo(hWnd);
           break;
        }  
         
        default:
            lresult=(LRESULT)null;
            result=false;                    // not handled
    }
    
         
    (*pResult)=(LRESULT)lresult;

    if (TerArg.modified!=SaveModified) PostMessage(hTerWnd,TER_IDLE,0,0L);

    return result;
}

/****************************************************************************
    DrgAbsToRowCol:
    Dragon wrapper for AbsToRowCol
****************************************************************************/
void DrgAbsToRowCol(PTERWND w,long abs,long far *row,int far *col)
{
    bool CountPCharAsCrLf=True(TerFlags4&TFLAG4_COUNT_PCHAR_AS_CRLF);

    TerFlags4|=TFLAG4_COUNT_PCHAR_AS_CRLF;    // count pchar as cr/lf

    AbsToRowCol(w,abs,row,col);

    if (!CountPCharAsCrLf) ResetLongFlag(TerFlags4,TFLAG4_COUNT_PCHAR_AS_CRLF);   // restore status
}       

/******************************************************************************
    DrgRowColToAbs:
    Dragon wrapper for RowColToAbs
******************************************************************************/
long DrgRowColToAbs(PTERWND w,long row,int col)
{
    long result;
    bool CountPCharAsCrLf=True(TerFlags4&TFLAG4_COUNT_PCHAR_AS_CRLF);

    TerFlags4|=TFLAG4_COUNT_PCHAR_AS_CRLF;    // count pchar as cr/lf

    result=RowColToAbs(w,row,col);
    if (!CountPCharAsCrLf) ResetLongFlag(TerFlags4,TFLAG4_COUNT_PCHAR_AS_CRLF);   // restore status

    return result;
} 

