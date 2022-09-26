/*==============================================================================
   TER Editor Developer's Kit
   Application Interface routines

   Sub Systems, Inc.
   Software License Agreement  (1990-2004)
   ----------------------------------
   This license agreement allows the purchaser the right to modify the
   source code to incorporate in an application larger than the editor itself.
   The target application must not be a programmer's utility 'like' a text editor.
   Sub Systems, Inc. reserves the right to prosecute anybody found to be
   making illegal copies of the executable software or compiling the source
   code for the purpose of selling there after.
===============================================================================*/

#define  PREFIX

#include "ter_hdr.h"
#include "imm.h"

/*******************************************************************************
    LineAlloc:
    Reallocate a text space for a line.
*******************************************************************************/
void LineAlloc(PTERWND w,long LineNo,UINT OldSize,UINT NewSize)
{

    TerArg.modified++;                     

    if (cfmt(LineNo).info.type!=UNIFORM) FmtAlloc(w,LineNo,OldSize,NewSize);
    if (pCtid(LineNo)) CtidAlloc(w,LineNo,OldSize,NewSize);
    if (pLead(LineNo)) LeadAlloc(w,LineNo,OldSize,NewSize);
    if (pCharWidth(LineNo)) CharWidthAlloc(w,LineNo,OldSize,NewSize);

    if (OldSize==0 && NewSize==0) {      // FALSE operation
        pText(LineNo)=NULL;
        LineLen(LineNo)=0;
        tabw(LineNo)=0;
        return;
    }

    if (OldSize==NewSize) {LineLen(LineNo)=NewSize;return;}        // FALSE operation

    if (NewSize==0){                     // free up memory
        sFree(pText(LineNo));            // free the text object
        pText(LineNo)=NULL;
        LineLen(LineNo)=0;
        if (tabw(LineNo)) FreeTabw(w,LineNo); // free the tab width object
        return;
    }

    if (OldSize!=0) {                    // reallocate memory
        pText(LineNo)= sReAlloc(pText(LineNo),NewSize+1);
        if (pText(LineNo)==0) {
            AbortTer(w,MsgString[MSG_OUT_OF_MEM],40);
        }
        LineLen(LineNo)=NewSize;
        return;
    }

    if (OldSize==0) {                    // allocate a new block of memory
        pText(LineNo)= sAlloc(NewSize+1);
        if (pText(LineNo)==0) {
            AbortTer(w,MsgString[MSG_OUT_OF_MEM],41);
        }
        LineLen(LineNo)=NewSize;
        return;
    }
}

/****************************************************************************
    TerGetCharDim:
    Get the width/height of a character in device units.
*****************************************************************************/
BOOL WINAPI _export TerGetCharDim(HWND hWnd, int FontId, UINT chr, LPINT pWidth, LPINT pHeight, BOOL screen)
{
    PTERWND w;
    int width,height;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (FontId<0 || FontId>TotalFonts || !TerFont[FontId].InUse) return FALSE;

    if (chr>255) chr='A';

    if (screen || !TerArg.PrintView || !PrinterAvailable) {
        width=TerFont[FontId].CharWidth[chr];
        height=TerFont[FontId].height;
    }
    else {
        width=PrtFont[FontId].CharWidth[chr];
        height=PrtFont[FontId].height;
    }

    if (pWidth) (*pWidth)=width;
    if (pHeight) (*pHeight)=height;

    return TRUE;
}

/****************************************************************************
    TerGetDeviceRes:
    Get the device resolution in pixels/inch
*****************************************************************************/
BOOL WINAPI _export TerGetDeviceRes(HWND hWnd, LPINT pResX, LPINT pResY, BOOL screen)
{
    PTERWND w;
    int ResX,ResY;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (screen) {
       ResX=ScrResX;
       ResY=ScrResY;
    }
    else {
       ResX=PrtResX;
       ResY=PrtResY;
    }

    if (pResX) (*pResX)=ResX;
    if (pResY) (*pResY)=ResY;

    return TRUE;
}

/****************************************************************************
    TerSetTextAreaSize:
    Set the dimensions of the text area (specified in screen pixels)
*****************************************************************************/
BOOL WINAPI _export TerSetTextAreaSize(HWND hWnd, int width, int height)
{
    PTERWND w;
    RECT rect;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    GetWindowRect(hWnd,&rect);

    rect.right+=(width-TerWinWidth);                // adjust the text area size
    rect.bottom+=(height-TerWinHeight);             // adjust the text area size

    SetWindowPos(hTerWnd,0,0,0,rect.right-rect.left,rect.bottom-rect.top,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);

    return TRUE;
}

/****************************************************************************
    TerDocName:
    Set/get the document name.
*****************************************************************************/
BOOL WINAPI _export TerDocName(HWND hWnd, BOOL get, LPBYTE name)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (get) lstrcpy(name,DocName);
    else     lstrcpy(DocName,name);

    return TRUE;
}

/****************************************************************************
    TerGetBufferDC:
    Get the handle of the buffer device context.
*****************************************************************************/
HDC WINAPI _export TerGetBufferDC(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return hBufDC;
}

/****************************************************************************
    TerIgnoreCommand:
    Ignore the current preprocessed command.
*****************************************************************************/
BOOL WINAPI _export TerIgnoreCommand(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    SkipCommand=TRUE;
    return TRUE;
}

/****************************************************************************
    TerGetLastMessage:
    Get the last message.
*****************************************************************************/
int WINAPI _export TerGetLastMessage(LPBYTE message, LPBYTE DebugMsg)
{
    if (message)  lstrcpy(message,MsgString[TerLastMsg]);
    if (DebugMsg) lstrcpy(DebugMsg,TerLastDebugMsg);

    return TerLastMsg;
}

/****************************************************************************
    TerResetLastMessage:
    Reset the last message.
*****************************************************************************/
BOOL WINAPI _export TerResetLastMessage()
{
    TerLastMsg=0;
    lstrcpy(TerLastDebugMsg,"");

    return TRUE;
}

/****************************************************************************
    TerSetCustomMessage:
    Get the last message.
*****************************************************************************/
BOOL WINAPI _export TerSetCustomMessage(int id, LPBYTE message)
{
    if (id<0 || id>=MAX_MSGS) return false;
    if (message==null) return false;

    if (CustomMsg[id]) {             // free any previous message
       MemFree(MsgString[id]);
       CustomMsg[id]=false;
    } 

    MsgString[id]=MemAlloc(strlen(message)+1);
    strcpy(MsgString[id],message);
    CustomMsg[id]=true;

    return true;
}


/****************************************************************************
    LoadTerControl:
    This function is called to ensures that the TER.LIB file is actually
    linked to your program. This function call is not needed if you are
    not using the TER editor as an edit control
*****************************************************************************/
BOOL WINAPI _export LoadTerControl()
{
    return TRUE;
}

/****************************************************************************
    TerAnd:
    This function returns result of and'ing two variables
*****************************************************************************/
UINT WINAPI _export TerAnd(UINT val1, UINT val2)
{
   return (val1&val2);
}

/****************************************************************************
    TerOr:
    This function returns result of or'ing two variables
*****************************************************************************/
UINT WINAPI _export TerOr(UINT val1, UINT val2)
{
   return (val1|val2);
}

/****************************************************************************
    TerDataVersion:
    This function returns the version of the TER internal data structures.
*****************************************************************************/
int WINAPI _export TerDataVersion()
{
    return TER_DATA_VER;
}

/****************************************************************************
    CreateTerWindow:
    This routine is called by your application to open a TER window.  This
    routine returns the control to your application after opening the window.
    The routine return the window handle if successful, otherwise it returns
    NULL.
*****************************************************************************/
HWND WINAPI _export CreateTerWindow(struct arg_list far *ArgPtr)
{
    BYTE string[256];
    HMENU hMenu=0;
    DWORD style;
    HWND  hWnd;
    PTERWND w;

    // create the window
    if (ArgPtr->file[0]==0) lstrcpy(string,"(UNNAMED)");else lstrcpy(string,ArgPtr->file);

    if (ArgPtr->ShowMenu && !(ArgPtr->style&WS_CHILD)) hMenu=hTerMenu;
    else                                               hMenu=NULL;

    style=ArgPtr->style;
    if (ArgPtr->style&WS_CHILD) style=style|WS_CLIPSIBLINGS;  // clip child siblings
    style|=TER_USE_PARAMS;                // instructs the InitTer routine to use parameter data

    hWnd=CreateWindow( TER_CLASS,
                       string,
                       style,
                       ArgPtr->x,
                       ArgPtr->y,
                       ArgPtr->width,
                       ArgPtr->height,
                       ArgPtr->hParentWnd,
                       hMenu,
                       hTerInst,
                       ArgPtr
                       );

    if (hWnd && !(style&TER_INVISIBLE) ) {
       w=GetWindowPointer(hWnd);       // get the window control data area
       if (ArgPtr->InitLine!=1) TerPosLine(w,ArgPtr->InitLine);  // position on the initial line number
       RulerPending=TRUE;

       ShowWindow(hWnd, SW_SHOW);
       if (ArgPtr->style&WS_CHILD) BringWindowToTop(hWnd);

       if (TerFlags3&TFLAG3_SELECT_FIRST_FIELD) SelectFirstFormField(w);
       else                                     SetFocus(hWnd);
    }

    return hWnd;
}

/****************************************************************************
    TerCreateWindowAlt:
    Quick method of creating a TER window.
*****************************************************************************/
HWND WINAPI _export TerCreateWindowAlt(int x, int y, int width, int height, BOOL IsChild, HWND hParentWindow, DWORD TerSty)
{
   DWORD style=(IsChild?WS_CHILDWINDOW:WS_OVERLAPPEDWINDOW);
   HWND hWnd;
   PTERWND w;
   //DWORD ParentStyle;

   if (TerSty==0) TerSty=TER_WORD_WRAP|TER_PRINT_VIEW|TER_PAGE_MODE|TER_VSCROLL|TER_SHOW_STATUS|TER_SHOW_RULER|TER_BORDER_MARGIN|TER_SHOW_TOOLBAR;

   // set clipchildren style on the parent, if not already set
   if (hParentWindow) {
     //ParentStyle=GetWindowLong(hParent,GWL_STYLE);
     //if (!(ParentStyle&WS_CLIPCHILDREN)) {
     //   ParentStyle|=WS_CLIPCHILDREN;
     //   SetWindowLong(hParent,GWL_STYLE,ParentStyle);
     //}
   }

   style=style|TerSty;
   if (!(TerSty&TER_INVISIBLE)) style|=WS_VISIBLE;

   hWnd=CreateWindow(TER_CLASS,"",style,x,y,width,height,hParentWindow,
                      (IsChild?NULL:hTerMenu),hTerInst,NULL);
   //hWnd=CreateWindowEx(WS_EX_MDICHILD,TER_CLASS,"",style,x,y,width,height,hParentWindow,
   //                   (IsChild?NULL:hTerMenu),hTerInst,NULL);

   if (hWnd && !IsChild) {
      if (NULL==(w=GetWindowPointer(hWnd))) return NULL;

      if (TerArg.InputType=='B') {
        if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
        TerArg.hBuffer=NULL;
        TerArg.InputType='F';
        DocName[0]=0;
      }
   }

   return hWnd;
}

/******************************************************************************
    CloseTer:
    Normal closing of TER routine.

    This routine can also be called from your application to close a TER
    Window. You will need to provide the handle of the window to be closed.

    The routine returns a TRUE value on successful closing, otherwise it
    returns a FALSE value.
*******************************************************************************/
BOOL WINAPI _export CloseTer(HWND hWnd,BOOL ForcedClose)
{
    int MessageResult;
    int i;
    long l;
    HCURSOR hSaveCursor=NULL;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (ImeEnabled) DisableIme(w,FALSE);    // disable ime processing

    //************* Save text if necessary *******************************
    if (TerArg.InputType=='F') {
       if (ForcedClose) {
          if (TerArg.modified && !TerArg.ReadOnly) { // let user save the modifications
              if (!TerSave(w,DocName,FALSE)) return FALSE;
          }
       }
       else {
          if (TerArg.modified && !TerArg.ReadOnly) { // let user save the modifications
              if (IDYES==(MessageResult=MessageBox(hTerWnd,MsgString[MSG_SAVE_MODS],"",MB_YESNOCANCEL))) {
                 if (!TerSave(w,DocName,FALSE)) return FALSE;
              }
              else if (MessageResult==IDCANCEL) return FALSE;
              else SendMessageToParent(w,TER_NOT_SAVED,(WPARAM)hWnd,0L,FALSE);
          }
       }
    }


    //*********** Send message to your application before closing *********
    if      (MsgCallback) MsgCallback(hTerWnd,TER_CLOSE,(WPARAM)hWnd,0L);
    else if (VbxCallback) VbxCallback(hTerWnd,TER_CLOSE,(WPARAM)hWnd,0L);
    else                  SendMessage(TerArg.hParentWnd,TER_CLOSE,(WPARAM)hWnd,0L);

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while waiting

    for (l=0;l<TotalLines;l++) FreeLine(w,l);
    if (hSaveCursor) SetCursor(hSaveCursor);

    TerArg.InitLine=CurLine+1;      // last line positioned


    //****** free the input buffer *****
    TerArg.open=FALSE;              // mark this window as closed
    if (TerArg.InputType=='B' && TerArg.hBuffer) GlobalFree(TerArg.hBuffer);

    // delete the tool bar
    if (hToolBarWnd) DestroyToolBar(w);  // destroy the tool bar
    TerArg.ToolBar=FALSE;
    FreeToolbar(w);                      // free the toolbar resources
    for (i=0;i<TotalTlbCustIds;i++) {
       if (!TlbCustId[i].InUse) continue;
       if (TlbCustId[i].pInfo)   OurFree(TlbCustId[i].pInfo);
       if (TlbCustId[i].pImage)  OurFree(TlbCustId[i].pImage);
       if (TlbCustId[i].pBalloon) MemFree(TlbCustId[i].pBalloon);
    } 
    TotalTlbCustIds=0;


    if (hWrap) {                    // free memory for the word wrap buffer
       GlobalUnlock(hWrap);
       GlobalFree(hWrap);
       GlobalUnlock(hWrapCfmt);
       GlobalFree(hWrapCfmt);
       OurFree(WrapCtid);
       OurFree(WrapLead);

       if (WrapCharWidth)      OurFree(WrapCharWidth);
       if (WrapCharWidthOrder) OurFree(WrapCharWidthOrder);
       if (WrapCharWidthDX)    OurFree(WrapCharWidthDX);
       if (WrapCharWidthText)  OurFree(WrapCharWidthText);
       if (WrapCharWidthClass) OurFree(WrapCharWidthClass);
    }


    if (hDeviceMode) {                   // release printer data memory
       GlobalUnlock(hDeviceMode);
       GlobalFree(hDeviceMode);
    }

    // freeup all undo buffers
    if (UndoCount>0) ReleaseUndo(w);    // release memory for the undo
    OurFree(undo);
    undo=NULL;

    // free the rtf informaion strings
    FreeRtfInfo(w);

    // freeup screen segment map
    DeleteTextMap(w,FALSE);

    // free the image map table
    FreeImageMapTable(w);

    // free auto-complete strings
    for (i=0;i<TotalAutoComps;i++) {
       MemFree(AutoCompWord[i]);
       MemFree(AutoCompPhrase[i]);
    } 
    TotalAutoComps=0;

    // release print preview edit window sub classing
    if (InPrintPreview && hPvToolBarWnd) {
       HWND hWnd1;                   // reset the combo edit sub classing
       if (NULL!=(hWnd1=GetComboEditWindow(hPvToolBarWnd,IDC_PV_ZOOM))) {
          SetWindowLong(hWnd1,GWL_WNDPROC,(DWORD)OrigZoomSubclassProc);
       }
    }

    // Disable any more message processing for this window
    if (VbxControl) {
       for (i=0;i<TotalWinPtrs;i++) if (WinPtr[i].InUse && WinPtr[i].hWnd==hWnd) break;
       if (i<TotalWinPtrs) {
          WinPtr[i].InUse=FALSE;
          WinPtr[i].hWnd=NULL;
       }
    }
    else SetWindowLong(hTerWnd,0,0L);      // Now send all messages to the default proceedure

    // freeup device contect resources
    if (TerArg.ShowMenu && !(TerArg.style&WS_CHILD)) SetMenu(hTerWnd,NULL); // detach the menu

    // free the resources for the buffered display
    TerFlags=ResetLongFlag(TerFlags,TFLAG_BUF_DISP);  // delete the buffer display resources

    SelectObject(hTerDC,GetStockObject(SYSTEM_FONT));
    for (i=0;i<TotalFonts;i++) {              // delete font objects
       if (TerFont[i].InUse) DeleteTerObject(w,i);// delete font/picture
    }
    if (DelRulerFont && hRulerFont) DeleteObject(hRulerFont);
    if (DelRulerFontBold && hRulerFontBold) DeleteObject(hRulerFontBold);
    if (DelStatusFont && hStatusFont) DeleteObject(hStatusFont);

    if (CharScrFontId) OurFree(CharScrFontId);    // release the character width cache
    if (CharPrtFontId) OurFree(CharPrtFontId);    // release the character width cache
    if (CharScrWidth)  OurFree(CharScrWidth);
    if (CharPrtWidth)  OurFree(CharPrtWidth);

    // release any text palette
    SelectPalette(hTerDC,GetStockObject(DEFAULT_PALETTE),TRUE);
    if (hBufDC) SelectPalette(hBufDC,GetStockObject(DEFAULT_PALETTE),TRUE);

    // destroy any popup menu
    if (hPopup) DestroyMenu(hPopup);

    // reset any message hooks
    if (hMsgHook) TerRedirectArrowKeys(hTerWnd,false);

    // free form subclass
   // delete any shared resources
   #if !defined(WIN32)
      if (FormFieldSubclassProc) FreeProcInstance((FARPROC)FormFieldSubclassProc);
   #endif

    // Free the OLE and timer resources
    ExitOle(w);

    // kill timers
    if (RepageTimerOn)   KillTimer(hTerWnd,TIMER_REPAGE);
    if (MouseStopTimerOn)KillTimer(hTerWnd,TIMER_MOUSE_STOP);
    if (eval)            KillTimer(hTerWnd,TIMER_EVAL);
    KillHilightTimer(w);

    if (hScrollBM) DeleteObject(hScrollBM);
    if (hBkPictBM) DeleteObject(hBkPictBM);
    if (hInitBkBM) DeleteObject(hInitBkBM);
    if (hTerPal)   DeleteObject(hTerPal);
    if (hToolbarBrush) DeleteObject(hToolbarBrush);

    SelectObject(hTerDC,GetStockObject(BLACK_PEN)); // delete pens

    // free spell-time variables
    if (pStFromUniChar!=null) MemFree(pStFromUniChar);

    // delete printer device context
    if (hPr && hPr!=hTerDC) {
       if (PrivatePrtDC) {
          SelectObject(hPr,GetStockObject(SYSTEM_FONT));
          DeleteDC(hPr);    // delete any old device context
       }
       else GlbPrtDC[GlbPrtDCId].UseCount--;
    }

    ResetBufBM(w);
    
    ReleaseDC(hTerWnd,hTerDC);

    if (hInternet) TerInternetClose(hTerWnd);

    if (hGetTextMem) GlobalFree(hGetTextMem); // free any WM_GETTEXT text data

    if (!WindowDestroyed) {
       WindowDestroyed=TRUE;
       if (TerArg.hParentWnd) SetFocus(TerArg.hParentWnd); //activate the previous window now
       DestroyWindow(hTerWnd);
    }

    // delete common resources when the last windows is closed
    TerOpenCount--;
    if (TerOpenCount<=0) {
       if (hMemDC) DeleteDC(hMemDC);
       hMemDC=NULL;
       if (hPagePen)  DeleteObject(hPagePen);
       if (hFocusPen) DeleteObject(hFocusPen);
       hPagePen=hFocusPen=NULL;

       // delete the tool bar bitmaps
       if (hToolbarBM)       DeleteObject(hToolbarBM);
       hToolbarBM=NULL;

       // freeup the cursor resources
       DestroyCursor(hTable1Cur);
       DestroyCursor(hTable2Cur);
       DestroyCursor(hTable3Cur);
       DestroyCursor(hTab1Cur);
       DestroyCursor(hHyperlinkCur);
       DestroyCursor(hPlusCur);
       DestroyCursor(hDragInCur);
       DestroyCursor(hDragInCopyCur);
       DestroyCursor(hDragOutCur);
       DestroyCursor(hWheelFullCur);
       DestroyCursor(hWheelUpCur);
       DestroyCursor(hWheelDownCur);
       DestroyCursor(hHBeamCur);

       hTable1Cur=hTable2Cur=hTable3Cur=hTab1Cur=hHyperlinkCur=hPlusCur=hDragInCur=hDragInCopyCur=hDragOutCur=NULL;
       hWheelFullCur=hWheelUpCur=hWheelDownCur=hHBeamCur=NULL;

       // release the global font table
       if (hGlbFont) {
          for (i=0;i<TotalGlbFonts;i++) {  // release the fonts
             if (GlbFont[i].hFont)  DeleteObject(GlbFont[i].hFont);
          }
          GlobalUnlock(hGlbFont);          // unlock an release the font
          GlobalFree(hGlbFont);
          hGlbFont=NULL;
          TotalGlbFonts=0;
       }

       // release the global printer DC
       if (!(TerFlags2&TFLAG2_KEEP_PRINTER_OPEN)) TerClosePrinter();
    }
    else TerShrinkFontTable();


    // relase memory used by line pointers
    for (i=0;i<LinePtrCount;i++) MemFree(LinePtrCache[i]);  // free the cache
    OurFree(LinePtr);

    // relase memory used by attribute ids
    GlobalUnlock(hFrame);            // freeup the text frame
    GlobalFree(hFrame);

    GlobalUnlock(hParaFrame);        // freeup the para frame
    GlobalFree(hParaFrame);

    OurFree(TerBlt);                // free bullet table

    for (i=0;i<MaxFonts;i++) if (TerFont[i].CharWidth) OurFree(TerFont[i].CharWidth);
    OurFree(TerFont);

    for (i=0;i<MaxFonts;i++) if (PrtFont[i].CharWidth) OurFree(PrtFont[i].CharWidth);
    OurFree(PrtFont);

    OurFree(TextSeg);               // freeup the text segment memory

    if (pTerPal) OurFree(pTerPal);  // free any palette table

    GlobalUnlock(hTerTab);          // freeup the tab table
    GlobalFree(hTerTab);

    OurFree(TerSect);
    OurFree(TerSect1);

    OurFree(PageInfo);

    for (i=1;i<TotalReviewers;i++) FreeReviewer(w,i);
    OurFree(reviewer);               // free the reviewer table

    GlobalUnlock(hPfmtId);           // freeup the paragraph id table
    GlobalFree(hPfmtId);

    GlobalUnlock(hStyleId);           // freeup the paragraph id table
    GlobalFree(hStyleId);

    GlobalUnlock(hDragObj);          // freeup the drag object table
    GlobalFree(hDragObj);

    GlobalUnlock(hTableRow);         // freeup the drag object table
    GlobalFree(hTableRow);

    OurFree(TableAux);               // free the auxiliary table row structure

    GlobalUnlock(hCell);             // freeup the table cell array
    GlobalFree(hCell);

    OurFree(CellAux);                // free the auxiliary cell structure

    for (i=0;i<TotalCharTags;i++) FreeTag(w,i);  // relese the strings
    OurFree(CharTag);                // free the character tag structure

    FreeListTable(w);                // free the list table
    OurFree(list);
    OurFree(ListOr);

    OurFree(RowX);                   // free the window line x array
    OurFree(RowY);                   // free the window line y array
    OurFree(RowHeight);              // free the window line height array

    if (hAppMemory) {              // free the user block
      GlobalUnlock(hAppMemory);
      GlobalFree(hAppMemory);
    }

    OurFree(w);                      // free the window data area
    
    return TRUE;
}

/****************************************************************************
   TerClosePrinter:
   Close the open printer device contexts.
*****************************************************************************/
BOOL WINAPI _export TerClosePrinter()
{
   int i;

   if (TerOpenCount>0) return FALSE;

   if (GlbPrtDC) {
      for (i=0;i<GlbPrtDCCount;i++) {
         if (GlbPrtDC[i].hDC) {
            SelectObject(GlbPrtDC[i].hDC,GetStockObject(SYSTEM_FONT));
            DeleteDC(GlbPrtDC[i].hDC);    // delete any old device context
         }
      }
      OurFree(GlbPrtDC);
      GlbPrtDC=NULL;
      GlbPrtDCCount=0;
   }

   return TRUE;
}

/****************************************************************************
    TerRegisterPrintCallback:
    This function register the pointer to a vbx callback process which
    receives messages intended to the parent of a TER control.
*****************************************************************************/
BOOL WINAPI _export TerRegisterPrintCallback(PRINT_CALLBACK proc)
{
    PrintCallback=proc;

    return TRUE;
}

/****************************************************************************
    TerRegisterVbxCallback:
    This function register the pointer to a vbx callback process which
    receives messages intended to the parent of a TER control.
*****************************************************************************/
BOOL WINAPI _export TerRegisterVbxCallback(HWND hWnd,VBX_CALLBACK proc)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    VbxCallback=proc;

    return TRUE;
}

/****************************************************************************
    TerGetVbxCallback:
    This function retrieves the message callback function set by a VBX.
*****************************************************************************/
VBX_CALLBACK WINAPI _export TerGetVbxCallback(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return VbxCallback;
}

/****************************************************************************
    TerSetParentPtr:
    This function saves a pointer to the parent window.
*****************************************************************************/
BOOL WINAPI _export TerSetParentPtr(HWND hWnd,void far *ptr)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    ParentPtr=ptr;

    return TRUE;
}

/****************************************************************************
    TerGetVbxCallback:
    This function retrieves the parent pointer
*****************************************************************************/
void far * WINAPI _export TerGetParentPtr(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return ParentPtr;
}

/****************************************************************************
    TerSetAccelHandle:
    Set custor accelrator table handle.  Set to 0 to use the original accelerator
    table handle.
*****************************************************************************/
BOOL WINAPI _export TerSetAccelHandle(HWND hWnd,HANDLE hAcc)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    hUserAccTable=hAcc;
    return TRUE;
}


/****************************************************************************
    TerRegisterMsgCallback:
    This function register the pointer to a application callback process which
    receives messages intended to the parent of a TER control.
*****************************************************************************/
BOOL WINAPI _export TerRegisterMsgCallback(HWND hWnd,MSG_CALLBACK proc)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    MsgCallback=proc;

    return TRUE;
}

/****************************************************************************
    TerGetCaretPos:
    Retrieve the position where the next text input is inserted.
*****************************************************************************/
long WINAPI _export TerGetCaretPos(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (CaretEngaged) return RowColToAbs(w,CurLine,CurCol);
    else              return CaretPos;
}

/****************************************************************************
    TerSetCaretPos:
    Retrieve the position where the next text input is inserted.
*****************************************************************************/
BOOL WINAPI _export TerSetCaretPos(HWND hWnd, long pos)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return TRUE;  // get the pointer to window data

    if (CaretEngaged) SetTerCursorPos(hWnd,pos,-1,FALSE);
    else              CaretPos=pos;

    return TRUE;
}

/****************************************************************************
    TerSetDefTabWidth:
    Set the new value for default tab width (specified in twips).
    This function returns the previous value of the tab width.
*****************************************************************************/
int WINAPI _export TerSetDefTabWidth(HWND hWnd,int width, BOOL repaint)
{
    PTERWND w;
    int OldWidth;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (width<RULER_TOLERANCE) width=RULER_TOLERANCE;

    OldWidth=DefTabWidth;
    DefTabWidth=width;

    if (repaint) PaintTer(w);

    return OldWidth;
}

/****************************************************************************
    TerSetCtlColor:
    Set the control background color.  This function also changes the default
    background colors of the existing font elements.
*****************************************************************************/
BOOL WINAPI _export TerSetCtlColor(HWND hWnd,COLORREF BkColor, BOOL repaint)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
 
    TextDefBkColor=TextBorderColor=BkColor=BkColor&0xFFFFFF;

    if (pTerPal) {                        // may need to create a new palette
       OurFree(pTerPal);
       pTerPal=NULL;
    }

    if (repaint) TerRepaint(hWnd,TRUE);
    else         InvalidateRect(hTerWnd,NULL,TRUE);    // set to refresh

    return TRUE;
}

/****************************************************************************
    TerSetDefBkColor:
    Set the default background color.
*****************************************************************************/
BOOL WINAPI _export TerSetDefBkColor(HWND hWnd,COLORREF BackColor, BOOL repaint)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (BackColor==TextDefBkColor) return TRUE;

    TextDefBkColor=TextBorderColor=BackColor;
    if (repaint) TerRepaint(hWnd,TRUE);

    return TRUE;
}

/****************************************************************************
    TerSetDefTextColor:
    Set the default text foreground color.
*****************************************************************************/
BOOL WINAPI _export TerSetDefTextColor(HWND hWnd,COLORREF ForeColor, BOOL repaint)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (ForeColor==TextDefColor) return TRUE;

    TextDefColor=ForeColor;
    if (repaint) {
       DeleteTextMap(w,TRUE);
       PaintTer(w);
    }

    return TRUE;
}

/****************************************************************************
    TerSetBorderColor:
    Set the window border area color.
*****************************************************************************/
BOOL WINAPI _export TerSetBorderColor(HWND hWnd,COLORREF BkColor)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    TextBorderColor=BkColor&0xFFFFFF;

    InvalidateRect(hTerWnd,NULL,TRUE);    // set to refresh

    return TRUE;
}

/****************************************************************************
    TerSetStatusColor:
    Set the toolbar, ruler, status area background color.
*****************************************************************************/
BOOL WINAPI _export TerSetStatusColor(HWND hWnd,COLORREF color, COLORREF BkColor)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    StatusColor=color&0xFFFFFF;
    StatusBkColor=BkColor&0xFFFFFF;

    if (hToolbarBrush) DeleteObject(hToolbarBrush);
    hToolbarBrush=CreateSolidBrush(StatusBkColor);
    if (hToolBarWnd) InvalidateRect(hToolBarWnd,NULL,TRUE);   // show the tool bar
    if (hPvToolBarWnd) InvalidateRect(hPvToolBarWnd,NULL,TRUE); // show the tool bar
    RulerPending=TRUE;

    InvalidateRect(hTerWnd,NULL,TRUE);    // set to refresh

    return TRUE;
}

/****************************************************************************
    TerSetPageBkColor:
    Set the background color for the page (the area within the page borders).
*****************************************************************************/
BOOL WINAPI _export TerSetPageBkColor(HWND hWnd,COLORREF BkColor)
{
    PTERWND w;
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    PageBkColor=BkColor&0xFFFFFF;

    return TRUE;
}

/****************************************************************************
    TerSetAppMemory:
    Record the handle to the application memory block
*****************************************************************************/
BOOL WINAPI _export TerSetAppMemory(HWND hWnd,long size)
{
    PTERWND w;
    long l;
    LPBYTE ptr;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // check if app memory to be released
    if (size==0) {
       if (hAppMemory) {
          GlobalUnlock(hAppMemory);
          GlobalFree(hAppMemory);
          hAppMemory=NULL;
          pAppMemory=NULL;
       }
       return TRUE; 
    }    
    
    // allocate new memory
    hAppMemory=GlobalAlloc(GMEM_MOVEABLE,size);
    pAppMemory=GlobalLock(hAppMemory);

    // initialize to 0
    ptr=(LPBYTE)pAppMemory;
    for (l=0;l<size;l++) ptr[l]=0;

    return (pAppMemory!=NULL ? TRUE : FALSE);
}

/****************************************************************************
    TerGetAppMemory:
    Retrieve the pointer to the application memory block
*****************************************************************************/
LPVOID WINAPI _export TerGetAppMemory(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL;  // get the pointer to window data'

    return pAppMemory;
}

/****************************************************************************
    GetTerBuffer:
    You can use this function to get the handle of the global buffer
    containing the text data.  Your application can release this buffer
    when it is no longer needed.

    This function returns the global handle, and the size of the buffer
    (second argument).
****************************************************************************/
HGLOBAL WINAPI _export GetTerBuffer(HWND hWnd, long far *size)
{
    HGLOBAL result=NULL;
    PTERWND w;
    BYTE SaveInputType;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL;  // get the pointer to window data

    // check if output requested in the HTML format
    if (TerArg.SaveFormat==SAVE_HTML) {
       HGLOBAL hMem;
       BOOL result;
       LoadHtmlAddOn(w);
       if (!hHts) return NULL;

       result=HtsSaveFromTer(hTerWnd,FALSE,NULL,&hMem,size);
       FreeHtmlAddOn(w);
       if (!result) hMem=NULL;
       return hMem;                 
    }    
    
    
    (*size)=0;

    // set the input type to 'buffer' temporariy
    SaveInputType=TerArg.InputType;
    if (TerArg.InputType!='B') {
       TerArg.InputType='B';
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;
       TerArg.delim=0xd;
    }

    //****** save the text to the new buffer if necessary *******
    if (TerArg.modified || TerArg.hBuffer==0) {
        if (!TerSave(w,DocName,FALSE)) goto END;
    }

    //**** return the initial buffer if not modified ***
    if (!TerArg.modified && TerArg.hBuffer) {
        result=TerArg.hBuffer;
        (*size)=TerArg.BufferLen;
        TerArg.hBuffer=NULL;
        TerArg.BufferLen=0;
    }

    END:
    TerArg.InputType=SaveInputType;        // set the old inputtype back

    return result;
}

/****************************************************************************
    SetTerBuffer:
    Your application can use this function to assign new text data to an
    already open text window.  The old content of the text window is discarded.

    The first argument is the handle of the text window.  The second argument
    is the GLOBAL handle of buffer containing new data.  The third argument
    is the size of the buffer. The fourth argument specifies the new name
    to appear in the window caption.

    If the 'release' argument is TRUE, the buffer becomes the property of the
    editor. Your application should never try to open or lock this buffer.

    The text lines in the buffer must be delimted by the delimiter that
    you specified in the arg_list parameter structure.

    To simply clear the existing contents, pass a handle to the buffer
    containing just the delimiter character and set the BufSize to 1.
    The function returns a TRUE value when successful.
****************************************************************************/
BOOL WINAPI _export SetTerBuffer(HWND hWnd, HGLOBAL hBuffer, long BufSize,LPBYTE name, BOOL release)
{
    long l;
    HCURSOR hSaveCursor=NULL;
    PTERWND w;
    BYTE SaveInputType;
    BOOL result=FALSE;
    struct StrFont SavePictFont;
    struct StrPrtFont SavePictPrtFont;
    int SaveBkPictId=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // check if input in the HTML format
    if (TerFlags4&TFLAG4_HTML_INPUT) {
       BOOL result;
       LoadHtmlAddOn(w);
       if (!hHts) return FALSE;

       for (l=0;l<TotalLines;l++) FreeLine(w,l);
       ResetInitVariables(w);
       if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;

       result=HtsReadFromTer(hTerWnd,FALSE,NULL,hBuffer,BufSize);
       FreeHtmlAddOn(w);

       if (release) GlobalFree(hBuffer);  // free it now

       return result;                 
    }    

    // set the input type to 'buffer' temporariy
    SaveInputType=TerArg.InputType;
    if (TerArg.InputType!='B') {
       TerArg.InputType='B';
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;
       TerArg.delim=0xd;
    }

    //**************** free the text area ************************
    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while waiting

    for (l=0;l<TotalLines;l++) FreeLine(w,l);
    if (hSaveCursor) SetCursor(hSaveCursor);

    // save any background picture
    if (BkPictId) {
       TransferFontId(w,FALSE,BkPictId,&SavePictFont,&SavePictPrtFont);
       SaveBkPictId=BkPictId;
       BkPictId=0;
    }

    //****************** reset buffer variables *****************
    ResetInitVariables(w);

    //****** free the existing buffer, assign new buffer *****
    if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);

    TerArg.hBuffer=hBuffer;
    TerArg.BufferLen=BufSize;

    //****************** read the new buffer *********************
    if (!TerRead(w,DocName)) goto END;

    if (TerFlags3&TFLAG3_SELECT_FIRST_FIELD) SelectFirstFormField(w);

    if (!release) {                // do not own this buffer
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;
    }
    if (SaveInputType=='F' && release) GlobalFree(hBuffer);  // this buffer can not be used any more

    if (name!=NULL) SetWindowText(hWnd,name);

    if (TerArg.ShowHorBar || TerArg.ShowVerBar) SetScrollBars(w);
    result=TRUE;

    END:
    TerArg.InputType=SaveInputType;   // restore the old input type

    // restore any background picture
    if (SaveBkPictId) {
       if (TotalFonts>=MaxFonts) {  // expand the font table
          int NewMaxFonts=MaxFonts+(MaxFonts/3)+1;
          if (NewMaxFonts>MAX_FONTS && !Win32) NewMaxFonts=MAX_FONTS;
          ExpandFontTable(w,NewMaxFonts);  // expand the font table
       }

       TerFont[TotalFonts].CharWidth = NULL;
       TerFont[TotalFonts].hidden    = NULL;
       PrtFont[TotalFonts].CharWidth = NULL;
       PrtFont[TotalFonts].hidden    = NULL;
              
       TransferFontId(w,TRUE,TotalFonts,&SavePictFont,&SavePictPrtFont);
       TotalFonts++;
       TerSetBkPictId(hWnd,TotalFonts-1,BkPictFlag,TRUE);
    }
    else PaintTer(w);

    return result;
}

/****************************************************************************
    SaveTerFile:
    This application saves the editor data to the specified file.
    This function returns TRUE if successful.
****************************************************************************/
BOOL WINAPI _export SaveTerFile(HWND hWnd, LPBYTE file)
{
    int result=FALSE;
    PTERWND w;
    BYTE SaveInputType;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // check if output requested in the HTML format
    if (TerArg.SaveFormat==SAVE_HTML) {
       LoadHtmlAddOn(w);
       if (!hHts) return FALSE;

       result=HtsSaveFromTer(hTerWnd,TRUE,file,NULL,NULL);
       FreeHtmlAddOn(w);
       return result;
    }    

    // set the input type to 'buffer' temporariy
    SaveInputType=TerArg.InputType;
    if (TerArg.InputType!='F') {
       TerArg.InputType='F';
       if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;
    }

    //****** save the text to the new buffer if necessary *******
    lstrcpy(DocName,file);
    if (!TerSave(w,DocName,FALSE)) goto END;
    result=TRUE;

    END:
    TerArg.InputType=SaveInputType;        // set the old inputtype back

    return result;
}

/****************************************************************************
    ReadTerFile:
    This function reads the specified file into the specified editor window.
    The previous content of the window is lost.
    The function returns a TRUE value when successful.
****************************************************************************/
BOOL WINAPI _export ReadTerFile(HWND hWnd, LPBYTE file)
{
    long l;
    HCURSOR hSaveCursor=NULL;
    PTERWND w;
    BYTE SaveInputType;
    BOOL result=FALSE;
    struct StrFont SavePictFont;
    struct StrPrtFont SavePictPrtFont;
    int SaveBkPictId=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // check if input in the HTML format
    if (TerFlags4&TFLAG4_HTML_INPUT) {
       LoadHtmlAddOn(w);
       if (!hHts) return FALSE;

       for (l=0;l<TotalLines;l++) FreeLine(w,l);
       ResetInitVariables(w);
       if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;

       result=HtsReadFromTer(hTerWnd,TRUE,file,NULL,0);
       FreeHtmlAddOn(w);
       return result;
    }    

    // ensure that the file exists
    lstrcpy(TempString,file);   // make a copy in the data segment
    if (lstrlen(file)>0 && access(TempString,4)==-1) return FALSE; // check for read permission

    // set the input type to 'buffer' temporariy
    SaveInputType=TerArg.InputType;
    if (TerArg.InputType!='F') {
       TerArg.InputType='F';
       if (TerArg.hBuffer) GlobalFree(TerArg.hBuffer);
       TerArg.hBuffer=NULL;
       TerArg.BufferLen=0;
    }

    //**************** free the text area ************************
    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass while waiting

    for (l=0;l<TotalLines;l++) FreeLine(w,l);

    if (hSaveCursor) SetCursor(hSaveCursor);

    // save any background picture
    if (BkPictId) {
       TransferFontId(w,FALSE,BkPictId,&SavePictFont,&SavePictPrtFont);
       SaveBkPictId=BkPictId;
       BkPictId=0;
    }

    //****************** reset buffer variables *****************
    ResetInitVariables(w);

    //****************** read the new buffer *********************
    lstrcpy(DocName,file);
    
    TerLastMsg=0;
    if (!TerRead(w,DocName) || TerLastMsg!=0) goto END;

    if (TerFlags3&TFLAG3_SELECT_FIRST_FIELD) SelectFirstFormField(w);

    if (TerArg.ShowHorBar || TerArg.ShowVerBar) SetScrollBars(w);
    result=TRUE;

    END:
    TerArg.InputType=SaveInputType;   // restore the old input type

    SetWindowText(hTerWnd,file);

    if (SaveBkPictId) {
       if (TotalFonts>=MaxFonts) {  // expand the font table
          int NewMaxFonts=MaxFonts+(MaxFonts/3)+1;
          if (NewMaxFonts>MAX_FONTS && !Win32) NewMaxFonts=MAX_FONTS;
          ExpandFontTable(w,NewMaxFonts);  // expand the font table
       }
       
       TransferFontId(w,TRUE,TotalFonts,&SavePictFont,&SavePictPrtFont);
       TotalFonts++;
       TerSetBkPictId(hWnd,TotalFonts-1,BkPictFlag,TRUE);
    }
    else PaintTer(w);

    return result;
}

/******************************************************************************
    ResetInitVariables:
    initialize new buffer variables.
******************************************************************************/
ResetInitVariables(PTERWND w)
{
    int i;

    IgnoreMouseMove=TRUE;
    MouseLine=0;
    MouseCol=0;

    TotalLines=1;
    TotalPfmts=1;
    TotalTableRows=1;
    TotalCells=1;
    TotalParaFrames=1;
    TotalTabs=1;
    TotalPages=1;
    TotalSID=2;
    TotalBlts=1;
    CurLine=CurRow=BeginLine=0;
    CurCol=0;
    CaretEngaged=TRUE;
    ViewPageHdrFtr=EditPageHdrFtr=PosPageHdrFtr=FALSE;
    HilightType=HILIGHT_OFF;
    DraggingText=FALSE; // TRUE when dragging text
    TerArg.modified=PageModifyCount=0;
    PaperOrient=DMORIENT_PORTRAIT; // protrait orientation
    Notified=FALSE;
    LeadWaiting=0;      // lead by waiting to be processed
    ToolBarCfmt=-1;              // reset
    ToolBarPfmt=-1;
    ToolBarSID=-1;
    DocHasToc=FALSE;      // TRUE if doc has toc
    DocHasHeadings=FALSE; // TRUE when the document has headings
    MultipleToc=FALSE;    // TRUE if doc has multiple toc
    ViewKind=0;           // rtf view mind
    FootnoteNumFmt=NBR_DEC; // default footnote number format
    DocTextFlow=FLOW_DEF;  // TRUE if right-to-left doc
    hCurCtlWnd=NULL;       // current control window handle
    CurCtlId=0;            // current control id
    OverrideTotalPages=0;  // Override for the page count field
    InPreprocess=false;    // TRUE when in preprocess
    PageBkColor=CLR_WHITE; // page background color
    ActiveOleObj=-1;       // active ole object

    InAutoComp=false;      // true when in auto-comp

    OverrideBinCopy=0;     // do not override bin for printing
    OverrideBin=0;         // override bin id
    WmParaFID=0;           // watermark para fid

    CurMapPict=0;         // map picture
    CurMapId=0;
    CurMapRect=0;

    TotalSects=1;
    InitSect(w,0);      // initialize the first section

    InitLine(w,0);
    if (UndoCount>0) ReleaseUndo(w);  // release the undos

    for (i=1;i<TotalFonts;i++) {    // delete font objects
       if (TerFont[i].InUse) DeleteTerObject(w,i);// delete font/picture
    }
    TotalFonts=1;
    InputFontId=-1;                 // initialize

    // create the default text frame
    TotalFrames=1;
    InitFrame(w,0);                          // initialize the first frame

    // release the tags
    for (i=1;i<TotalCharTags;i++) FreeTag(w,i);
    TotalCharTags=1;

    // free the list table
    FreeListTable(w);
    TotalLists=1;
    TotalListOr=1;

    // free the rtf informaion strings
    FreeRtfInfo(w);

    // free the image map table
    FreeImageMapTable(w);

    if (mbcs) InitCharWidthCache(w);        // initialize the character width cache

    // initialize the reviewer table
    for (i=1;i<TotalReviewers;i++) FreeReviewer(w,i);
    FarMemSet(&(reviewer[0]),0,sizeof(struct StrReviewer));
    TotalReviewers=1;
    
    TrackChanges=false;

    return TRUE;
}

/******************************************************************************
    TerShrinkFontTable:
    Remove the unused fonts from the global font table.
******************************************************************************/
BOOL WINAPI _export TerShrinkFontTable()
{
    int i;

    // shrink the global font table
    if (hGlbFont) {
       for (i=0;i<TotalGlbFonts;i++) {  // release the fonts
          if (GlbFont[i].UseCount==0) {
            if (GlbFont[i].hFont)  DeleteObject(GlbFont[i].hFont);
            GlbFont[i].hFont=NULL;
          }
       }
    }

    return TRUE;
}

/******************************************************************************
    TerRewrap:
    Rewrap the entire document.
******************************************************************************/
BOOL WINAPI _export TerRewrap(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.WordWrap) return FALSE;                  // valid in the word wrap mode only

    WordWrap(w,0,TotalLines);                           // wrap the entire file

    return TRUE;
}

/******************************************************************************
    TerEnableSpeedKey:
    This function is used to enable or disable a speed key.  The speed key
    id is defined in the TER_CMD.H file.  This function returns the previous
    status of the key. Values for the enable argument:
    
    0 = disable
    1 = enable always
    2 = enable for pre-process only  
******************************************************************************/
BOOL WINAPI _export TerEnableSpeedKey(HWND hWnd, int cmd, int enable)
{
    PTERWND w;
    BOOL PrevStat=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (cmd>=ID_FIRST_COMMAND && cmd<(ID_FIRST_COMMAND+MAX_SPEED_KEYS)) {
       PrevStat=SpeedKeyEnabled[cmd-ID_FIRST_COMMAND];
       SpeedKeyEnabled[cmd-ID_FIRST_COMMAND]=(char)enable;
    }

    return PrevStat;
}

/******************************************************************************
    TerEnableRefresh:
    This function enables or disables screen painting.
******************************************************************************/
BOOL WINAPI _export TerEnableRefresh(HWND hWnd, BOOL enable)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    PaintEnabled=enable;
    return TRUE;
}

/******************************************************************************
    TerGetReadOnly:
    This function retrieves the ReadOnly status.
******************************************************************************/
BOOL WINAPI _export TerGetReadOnly(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    return TerArg.ReadOnly;
}

/******************************************************************************
    TerSetReadOnly:
    This function sets or resets the ReadOnly status.  The function returns
    the previous status of read only flag.
******************************************************************************/
BOOL WINAPI _export TerSetReadOnly(HWND hWnd, BOOL ReadOnly)
{
    PTERWND w;
    BOOL PrevReadOnly;
    int i;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    PrevReadOnly=TerArg.ReadOnly;
    TerArg.ReadOnly=ReadOnly;
    
    InitCaret(w);                     // create or destroy caret
    if (hToolBarWnd) {
       EnableToolbarIcons(w,!ReadOnly);
       UpdateToolBar(w,TRUE);
    }
    DragAcceptFiles(hTerWnd,!TerArg.ReadOnly);

    if (!PrevReadOnly && !(TerFlags&TFLAG_NO_OLE))    RevokeDragDrop(hTerWnd);
    if (!TerArg.ReadOnly && !(TerFlags&TFLAG_NO_OLE) && !(TerFlags5&TFLAG5_NO_EXT_DROP)) RegisterDragDrop(hTerWnd,(LPDROPTARGET)CreateDropTarget(w));

    if (TerFlags4&TFLAG4_READONLY_CONTROLS) {  // enable/dissable input fields
       for (i=0;i<TotalFonts;i++) {
          if (IsControl(w,i) && TerFont[i].hWnd) EnableWindow(TerFont[i].hWnd,!ReadOnly);
       } 
    } 

    return PrevReadOnly;
}

/******************************************************************************
    TerSetModify:
    This function sets or resets the modification flag.
******************************************************************************/
BOOL WINAPI _export TerSetModify(HWND hWnd, BOOL modified)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (modified) TerArg.modified++;
    else {
       TerArg.modified=0;
       Notified=FALSE;       // notify again if modified
    }

    return TRUE;
}

/******************************************************************************
    TerIsModified:
    This function returns TRUE if editor data needs to be saved.
******************************************************************************/
BOOL WINAPI _export TerIsModified(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return TerArg.modified;
}

/******************************************************************************
    TerSetOutputFormat:
    This function sets or resets the modification flag. The 'format'
    argument can be SAVE_DEFAULT, SAVE_TER, SAVE_TEXT, SAVE_TEXT_LIENS or SAVE_RTF
******************************************************************************/
BOOL WINAPI _export TerSetOutputFormat(HWND hWnd, int format)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (format!=SAVE_DEFAULT && format!=SAVE_TER && format!=SAVE_TEXT && format!=SAVE_TEXT_LINES 
        && format!=SAVE_RTF && format!=SAVE_HTML && format!=SAVE_UTEXT) return FALSE;

    if (format==SAVE_HTML && !HtmlAddOnFound(w)) return FALSE;

    TerArg.SaveFormat=format;
    TerArg.modified++;         // ensure returning new buffer
    Notified=TRUE;             // do not notify for this change

    return TRUE;
}

/******************************************************************************
    InsertTerText:
    Insert a text buffer at the current cursor location.  The cursor location
    is advanced to the first character after the buffer text.
******************************************************************************/
BOOL WINAPI _export InsertTerText(HWND hWnd, BYTE huge *text,BOOL repaint)
{
    PTERWND w;
    long SaveCurLine;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    SaveCurLine=CurLine;
    InsertBuffer(w,text,NULL,NULL,NULL,NULL,FALSE);         // insert the text buffer

    if (repaint) {
       WinHeight=TerWinHeight/TerFont[0].height;  // set to default
       PaintTer(w);                               // refresh the screen
    }
    else if (TerArg.WordWrap) WordWrap(w,SaveCurLine,CurLine-SaveCurLine+1);

    return TRUE;
}

/******************************************************************************
    TerAppendText:
    This function appends the specified text to the current buffer.  The FontId
    and ParaId are assigned to the new text.  Use -1 for FontId and ParaId to
    select a default value.
    This function does not change the current cursor position.
******************************************************************************/
BOOL WINAPI _export TerAppendText(HWND hWnd, BYTE huge *text, int FontId, int ParaId, BOOL repaint)
{
    return TerAppendTextEx(hWnd,text,FontId,ParaId,-1,-1,repaint);
}

/******************************************************************************
    TerAppendTextEx:
    this function is enhanced version of TerAppendText.  It also takes
    the cell id and another reserved field.  The cell id can be set to -1
    to use the default value. The reserved field should always the set to -1.
    The function does not change the cursor position
******************************************************************************/
BOOL WINAPI _export TerAppendTextEx(HWND hWnd, BYTE huge *string, int FontId, int ParaId, int CellId, int ParaFID, BOOL repaint)
{
    return TerAppendText2(hWnd,string,NULL,FontId,ParaId,CellId,ParaFID,repaint);
}

/******************************************************************************
    TerAppendTextU:
    this function is unicode version of TerAppendTextEx.
******************************************************************************/
BOOL WINAPI _export TerAppendTextU(HWND hWnd, WORD huge *string, int FontId, int ParaId, int CellId, int ParaFID, BOOL repaint)
{

    return TerAppendText2(hWnd,NULL,string,FontId,ParaId,CellId,ParaFID,repaint);
}

/******************************************************************************
    TerAppendText2:
    this function is enhanced version of TerAppendText.  It also takes
    the cell id and another reserved field.  The cell id can be set to -1
    to use the default value. The reserved field should always the set to -1.
    The function does not change the cursor position
******************************************************************************/
BOOL TerAppendText2(HWND hWnd, BYTE huge *string, LPWORD UText, int FontId, int ParaId, int CellId, int ParaFID, BOOL repaint)
{
    PTERWND w;
    int far *pPfmt=NULL,ParaTable[2],BytesDone,CurLen;
    long SaveCurLine,SaveCurRow,line,SaveLastLine;
    int  i,SaveCurCol,SaveInputFontId,MaxLen,CrCount=0,ParaCount=0,len;
    int  SaveCellId,SaveParaFID,FirstCrLf=-1;
    LPBYTE ptr,lead,text=NULL,LeadText=NULL;
    LPWORD UcBase=NULL,fmt;
    UINT  InfoType=0;
    BOOL  EmbeddedCrLf;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (UText) {               // unicode text specifid, 
      for (i=0;UText[i]!=0;i++);  // get the length of the string
      text=OurAlloc(i+1);
      UcBase=OurAlloc((i+1)*sizeof(WORD));
      CrackUnicodeString(w,UText,UcBase,text);
    } 
    else if (mbcs) AllocDB(w,string,&text,&LeadText);
    else      text=(LPBYTE)string;

    // check if quick method can be used
    if (TerArg.WordWrap) MaxLen=LineWidth/3;
    else                 MaxLen=LineWidth-2;
    len=lstrlen(text);
    for (i=0;i<len;i++) {   // scan text for null and cr/lf or para id
       if (text[i]==0) break;
       if (text[i]==0xd) CrCount++;
       if (text[i]==ParaChar || text[i]==CellChar || text[i]==LINE_CHAR) ParaCount++;
       if (text[i]==CellChar) InfoType|=INFO_CELL;
       if (text[i]==ROW_CHAR) InfoType|=INFO_ROW;
       if (CrCount==1 && FirstCrLf<0) FirstCrLf=i;
    }
    if (len==0) goto REPAINT;                           // zero length buffer

    // use fast method for short line with one or zero cr/lf
    EmbeddedCrLf=(FirstCrLf>=0 && FirstCrLf<(len-2));
    if (CellId>0 || ParaFID>0) goto QUICK_METHOD;

    if ((len>=LineWidth && !TerArg.WordWrap) || (CrCount+ParaCount)>1) goto ALTERNATE_METHOD;
    if (CrCount==1 && len>=2 && text[len-2]!=0xd)        goto ALTERNATE_METHOD;
    if (ParaCount==1 && len>=1 && text[len-1]!=ParaChar && text[len-1]!=LINE_CHAR
                               && text[len-1]!=CellChar) goto ALTERNATE_METHOD;

    // quick method - append the line (MaxLen at a time) to the text array
    QUICK_METHOD:
    BytesDone=0;
    len=lstrlen(text);
    while (BytesDone<len) {
       CurLen=len-BytesDone;
       if (CurLen>MaxLen) CurLen=MaxLen;
       if (text[BytesDone+CurLen-1]==0xd && CurLen==MaxLen) CurLen-=1;  // don't break the 0xd/0xa pair

       // check for embedded cr/lf
       if (EmbeddedCrLf) {
          for (i=0;i<(CurLen-2);i++) {     // 'CurLen-2' excludes the last cr/lf if any
             if (text[BytesDone+i]==0xd && text[BytesDone+i+1]==0xa) {
                CurLen=i+2;                // includes this cr/lf
                break;
             }
          }
       }

       if (!CheckLineLimit(w,TotalLines+1)) goto REPAINT;
       line=TotalLines;
       TotalLines++;

       InitLine(w,line);
       LineAlloc(w,line,0,CurLen);
       OpenTextPtr(w,line,&ptr,&lead);
       for (i=0;i<CurLen;i++) {
          ptr[i]=text[BytesDone+i];  // copy text
          if (lead) lead[i]=0;
          if (mbcs && LeadText) lead[i]=LeadText[BytesDone+i];
       }
       if (CurLen>=2 && ptr[CurLen-2]==0xd) {      // convert cr/lf to ParaChar
          if (TerArg.WordWrap) {
             ptr[CurLen-2]=ParaChar;
             LineAlloc(w,line,CurLen,CurLen-1);   // discard the 0xa character
          }
          else LineAlloc(w,line,CurLen,CurLen-2); // discard cr/lf
       }
       CloseTextPtr(w,line);

       // assign the font id
       cfmt(line).info.type=UNIFORM;
       if (FontId>=0 && FontId<TotalFonts && TerFont[FontId].InUse) cfmt(line).info.fmt=FontId;
       else if (InputFontId>=0) cfmt(line).info.fmt=InputFontId;
       else                     cfmt(line).info.fmt=DEFAULT_CFMT;

       // apply unicode fonts
       if (UcBase) {
          fmt=OpenCfmt(w,line);
          len=LineLen(line);
          for (i=0;i<len;i++) fmt[i]=SetUniFont(w,fmt[i],UcBase[i]);
       } 

       // assign the para and cell id
       if (ParaId>=0 && ParaId<TotalPfmts)        pfmt(line)=ParaId;
       if (CellId>=0 && CellId<TotalCells)        cid(line)=CellId;
       if (ParaFID>=0 && ParaFID<TotalParaFrames) fid(line)=ParaFID;

       // set tabw structure
       if (InfoType) {
          if (AllocTabw(w,line)) tabw(line)->type=InfoType;
       }
       if (ParaCount>0 && ((BytesDone+CurLen)==len)) LineFlags(line)=LFLAG_PARA;


       BytesDone+=CurLen;
    }

    goto REPAINT;


    ALTERNATE_METHOD:
    // set the assigned font and para id
    SaveInputFontId=InputFontId;
    if (FontId>=0 && FontId<TotalFonts && TerFont[FontId].InUse) InputFontId=FontId;

    if (ParaId>=0 && ParaId<TotalPfmts) {
       ParaTable[0]=ParaId;
       ParaTable[1]=-1;            // table terminator
       pPfmt=ParaTable;
    }

    // save current parameters and set the cursor to the end of the document
    SaveCurLine=CurLine;
    SaveCurRow=CurRow;
    SaveCurCol=CurCol;

    CurLine=TotalLines-1;
    CurCol=LineLen(CurLine);

    SaveLastLine=CurLine;
    SaveCellId=cid(SaveLastLine);
    if (CellId>=0) cid(CurLine)=CellId;
    SaveParaFID=fid(SaveLastLine);
    if (ParaFID>=0) fid(CurLine)=ParaFID;

    InsertBuffer(w,text,UcBase,LeadText,NULL,pPfmt,FALSE);         // insert the text buffer

    // restore the cell id of the previous row line
    if (CellId>=0)  cid(SaveLastLine)=SaveCellId;
    if (ParaFID>=0) fid(SaveLastLine)=SaveParaFID;

    // reset the current position
    CurLine=SaveCurLine;
    CurRow=SaveCurRow;
    CurCol=SaveCurCol;
    InputFontId=SaveInputFontId;


    REPAINT:
    if (UText) {            // free unicode related resources
       OurFree(text);
       OurFree(UcBase);
    }
    else { 
       if (mbcs && text) OurFree(text);
       if (LeadText) OurFree(LeadText);
    }

    if (repaint) {
       WinHeight=TerWinHeight/TerFont[0].height;  // set to default
       PaintTer(w);                               // refresh the screen
    }

    return TRUE;
}

/******************************************************************************
    TerInsertLine:
    This function is a very fast method of inserting individual text lines.
    The line is inserted before the current line and the cursor is moved
    to the first column of the next line. In a batch of TerInsertLine functions,
    the repaint argument should be set to TRUE only for the last TerInsertLine
    function.
    The length of the line must not execeed MAX_WIDTH-1 characters.
******************************************************************************/
BOOL WINAPI _export TerInsertLine(HWND hWnd, BYTE huge *string, int FontId, int ParaId, int CellId, int ParaFID, BOOL repaint)
{
    PTERWND w;
    LPBYTE ptr,lead,text=NULL,LeadText=NULL;
    int i,len;
    long line;
    BOOL HasParaChar=FALSE;
    UINT InfoType=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    len=lstrlen((LPBYTE)string);
    if (len>(MAX_WIDTH-1)) return FALSE;

    // quick method - append the line to the text array
    if (!CheckLineLimit(w,TotalLines+1)) return TRUE;

    // split mbcs
    if (mbcs) len=AllocDB(w,string,&text,&LeadText);
    else      text=string;

    // insert a blank line
    MoveLineArrays(w,CurLine,1,'B');               // create a line before Current Line
    line=CurLine;
    CurLine++;
    CurCol=0;

    LineAlloc(w,line,0,len);
    OpenTextPtr(w,line,&ptr,&lead);
    for (i=0;i<len;i++) {
       ptr[i]=text[i];  // copy text
       if (mbcs) lead[i]=LeadText[i];
    }
    if (len>=2 && ptr[len-2]==0xd) {      // convert cr/lf to ParaChar
       if (TerArg.WordWrap) {
          ptr[len-2]=ParaChar;
          LineAlloc(w,line,len,len-1);   // discard the 0xa character
       }
       else LineAlloc(w,line,len,len-2); // discard cr/lf
       HasParaChar=TRUE;
    }
    else if (len>0 && text[len-1]==PARA_CHAR) HasParaChar=TRUE;
    else if (len>0 && text[len-1]==CellChar)  InfoType|=INFO_CELL;
    else if (len>0 && text[len-1]==ROW_CHAR)  InfoType|=INFO_ROW;

    CloseTextPtr(w,line);

    // assign the font id
    cfmt(line).info.type=UNIFORM;
    if (FontId>=0 && FontId<TotalFonts && TerFont[FontId].InUse) cfmt(line).info.fmt=FontId;
    else if (InputFontId>=0) cfmt(line).info.fmt=InputFontId;
    else                     cfmt(line).info.fmt=DEFAULT_CFMT;

    // assign the para and cell id
    if (ParaId>=0 && ParaId<TotalPfmts)        pfmt(line)=ParaId;
    if (CellId>=0 && CellId<TotalCells)        cid(line)=CellId;
    if (ParaFID>=0 && ParaFID<TotalParaFrames) fid(line)=ParaFID;

    // set tabw structure
    if (InfoType) {
       if (AllocTabw(w,line)) tabw(line)->type=InfoType;
    }
    if (HasParaChar || InfoType&INFO_CELL) LineFlags(line)=LFLAG_PARA;

    // free the text pointers
    if (mbcs && text) OurFree(text);
    if (LeadText)  OurFree(LeadText);

    if (repaint) {
       WinHeight=TerWinHeight/TerFont[0].height;  // set to default
       PaintTer(w);                               // refresh the screen
    }

    return TRUE;
}

/******************************************************************************
    TerInsertText:
    This function inserts the specified text at the current cursor position. The
    FontId and ParaId are assigned to the new text.  Use -1 for FontId and
    ParaId to select a default value.
    This function positions the cursor position at the end of the inserted text.
******************************************************************************/
BOOL WINAPI _export TerInsertText(HWND hWnd, BYTE huge *text, int FontId, int ParaId, BOOL repaint)
{
    PTERWND w;
    int far *pPfmt=NULL,ParaTable[2];
    int  SaveInputFontId;
    long SaveCurLine,SaveTotalLines;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // set the assigned font and para id
    SaveInputFontId=InputFontId;
    if (FontId>=0 && FontId<TotalFonts && TerFont[FontId].InUse) {
       if (ParaId>=0 && ParaId<TotalPfmts && PfmtId[ParaId].StyId>0) {  // associate this style id with the font so it holds after word wrapping
          FontId=SetFontStyleId(w,FontId,TerFont[FontId].CharStyId,PfmtId[ParaId].StyId);
       } 
       InputFontId=FontId;
    }

    if (ParaId>=0 && ParaId<TotalPfmts) {
       ParaTable[0]=ParaId;
       ParaTable[1]=-1;            // table terminator
       pPfmt=ParaTable;
    }

    SaveCurLine=CurLine;
    SaveTotalLines=TotalLines;
    InsertBuffer(w,text,NULL,NULL,NULL,pPfmt,FALSE);         // insert the text buffer

    InputFontId=SaveInputFontId;                   // reset input font id

    if (repaint) {
       WinHeight=TerWinHeight/TerFont[0].height;  // set to default
       PaintTer(w);                               // refresh the screen
    }
    else if (TerArg.WordWrap) WordWrap(w,SaveCurLine,CurLine-SaveCurLine+1);

    return TRUE;
}

/******************************************************************************
    TerInsertTextU:
    This function inserts the specified text at the current cursor position. The
    FontId and ParaId are assigned to the new text.  Use -1 for FontId and
    ParaId to select a default value.
    This function positions the cursor position at the end of the inserted text.
******************************************************************************/
BOOL WINAPI _export TerInsertTextU(HWND hWnd, WORD huge *UText, int FontId, int ParaId, BOOL repaint)
{
    PTERWND w;
    int far *pPfmt=NULL,ParaTable[2];
    int  i,SaveInputFontId;
    long SaveCurLine;
    LPBYTE text;
    LPWORD UcBase;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    for (i=0;UText[i]!=0;i++);  // get the length of the string
    text=OurAlloc(i+1);
    UcBase=OurAlloc((i+1)*sizeof(WORD));
    CrackUnicodeString(w,UText,UcBase,text);

    // set the assigned font and para id
    SaveInputFontId=InputFontId;
    if (FontId>=0 && FontId<TotalFonts && TerFont[FontId].InUse) InputFontId=FontId;

    if (ParaId>=0 && ParaId<TotalPfmts) {
       ParaTable[0]=ParaId;
       ParaTable[1]=-1;            // table terminator
       pPfmt=ParaTable;
    }

    SaveCurLine=CurLine;
    InsertBuffer(w,text,UcBase,NULL,NULL,pPfmt,FALSE);         // insert the text buffer

    InputFontId=SaveInputFontId;                   // reset input font id

    OurFree(text);
    OurFree(UcBase);

    if (repaint) {
       WinHeight=TerWinHeight/TerFont[0].height;  // set to default
       PaintTer(w);                               // refresh the screen
    }
    else if (TerArg.WordWrap) WordWrap(w,SaveCurLine,CurLine-SaveCurLine+1);

    return TRUE;
}

/******************************************************************************
    SelectTerText:
    This function can be used to highlight the selected area programatically.
    The beginning highlight position is specified by FirstLine and FirstCol
    variables. The ending highlight position is specified by LastLine and LastCol
    variable.  To specify absolute positions, set the 'FirstCol' or 'LastCol'
    to -1, and specify the absolute position using the 'FirstLine' or 'LastLine'
    variables.  All position are specified with base 0.
******************************************************************************/
BOOL WINAPI _export SelectTerText(HWND hWnd, long FirstLine, int FirstCol, long LastLine, int LastCol,BOOL repaint)
{
    PTERWND w;
    long line;
    BOOL EmptySelection;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data


    // convert absolute position to line/col
    if (FirstCol<0) {
       line=FirstLine;
       AbsToRowCol(w,line,(long far *)&FirstLine,(int far *)&FirstCol);
    }
    if (LastCol<0) {
       line=LastLine;
       AbsToRowCol(w,line,(long far *)&LastLine,(int far *)&LastCol);
       EmptySelection=(FirstLine==LastLine && FirstCol==LastCol);
       if (!EmptySelection && TerArg.WordWrap && LastCol==0 && LastLine>0 && LineEndsInBreak(w,LastLine-1)) {
          LastLine--;
          LastCol=LineLen(LastLine);
       }
    }

    // set the highlight
    HilightType=HILIGHT_CHAR;
    HilightBegRow=FirstLine;
    HilightBegCol=FirstCol;
    HilightEndRow=LastLine;
    HilightEndCol=LastCol;
    StretchHilight=FALSE;

    // check if picture clicked
    if (repaint) {
       int CurFont=GetCurCfmt(w,HilightBegRow,HilightBegCol);
       PictureClicked=FALSE;
       if (TerFont[CurFont].style&PICT) {
          long BegPos=RowColToAbs(w,HilightBegRow,HilightBegCol);
          long EndPos=RowColToAbs(w,HilightEndRow,HilightEndCol);
          PictureClicked=(EndPos==(BegPos+1));        // only picture selected
       } 
       if (PictureClicked && !(TerFlags&TFLAG_NO_EDIT_PICT)) {
          PictureHilighted=FALSE;
          ShowPictureDragObjects(w,CurFont);
       }
       else PaintTer(w);
    } 


    return TRUE;
}

/******************************************************************************
    DeselectTerText:
    This function can be used to turn off the text highlight programatically.
******************************************************************************/
BOOL WINAPI _export DeselectTerText(HWND hWnd,BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    HilightType=HILIGHT_OFF;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    SetTerCursorPos:
    Set the cursor position at the specified line/col position.  To specify
    the absolute position, set the NewCol to -1, and specify the absolute
    position using the NewLine variable.
******************************************************************************/
BOOL WINAPI _export SetTerCursorPos(HWND hWnd,long NewLine, int NewCol, BOOL repaint)
{
    PTERWND w;
    long line;
    int  WinLines;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // convert absolute position to line/col
    if (NewCol<0) {
       line=NewLine;
       AbsToRowCol(w,line,(long far *)&NewLine,(int far *)&NewCol);
    }

    // validate position
    if (NewLine>=TotalLines || NewLine<0) {
       NewLine=TotalLines-1;
       NewCol=LineLen(NewLine);
       if (TerArg.WordWrap) NewCol--;
       if (NewCol<0) NewCol=0;
    }

    if (NewCol>LineLen(NewLine)) NewCol=LineLen(NewLine);
    if (NewCol<0) NewCol=0;

    if (!CaretEngaged) EngageCaret(w,-1);     // engate caret at the current cursor location

    // set position
    CurLine=NewLine;
    CurCol=NewCol;
    WinLines=TerWinHeight/TerFont[0].height;  // calculte rough window height
    if ((CurLine-BeginLine)>=WinLines || (CurLine-BeginLine)<0) {
       BeginLine=CurLine-WinLines/2;  // position the current line at center
       if (BeginLine<0) BeginLine=0;
    }
    CurRow=CurLine-BeginLine;

    if (repaint) {
       PaintTer(w);
       PostMessage(hTerWnd,TER_IDLE,0,0L); // do idle time activity to show the cursor
    }

    return TRUE;
}

/******************************************************************************
    GetTerCursorPos:
    This function is used to get the current position of the cursor.  The
    cursor position is returned as CursLine/CursCol using the argument pointers.
    To get the absolute cursor position, set the CursCol value to -1 before
    calling this function.  The absolute position is returned in the
    CursLine variable.
******************************************************************************/
BOOL WINAPI _export GetTerCursorPos(HWND hWnd,long far *CursLine, int far * CursCol)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (*CursCol==-1) {
       *CursLine=RowColToAbs(w,CurLine,CurCol);
    }
    else {
       *CursLine=CurLine;
       *CursCol=CurCol;
    }

    return TRUE;
}


/****************************************************************************
    TerGetLine:
    This function returns the contents of the specified line.
****************************************************************************/
int WINAPI _export TerGetLine(HWND hWnd, long LineNo, LPBYTE text, LPWORD font)
{
    return TerGetLineEx(hWnd,LineNo,text,NULL,font);
}

/****************************************************************************
    TerGetLineEx:
    This function returns the contents of the specified line.
    Either 'text' or 'font' argument can be NULL, if you do not wish to
    receive that information.
    The 'text' pointer receives the NULL terminated line text.
    The 'font' pointer receives the font id of each character of the line.
    The function returns the length of the line, if successful. Otherwise it return -1.
****************************************************************************/
int WINAPI _export TerGetLineEx(HWND hWnd, long LineNo, LPBYTE text, LPBYTE LeadText, LPWORD font)
{
    LPBYTE ptr,lead;
    LPWORD fmt;
    int   i;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (LineNo<0 || LineNo>=TotalLines) return -1;  // invalid line no

    //************* get the text data *************
    if (text) {
       ptr=pText(LineNo);
       for (i=0;i<LineLen(LineNo);i++) text[i]=ptr[i];
       text[i]=0;
    }

    //************* get the text lead data *************
    if (mbcs && LeadText) {
       lead=OpenLead(w,LineNo);
       for (i=0;i<LineLen(LineNo);i++) LeadText[i]=lead[i];
       CloseLead(w,LineNo);
    }

    //************* fill in font data *************
    if (font) {
       fmt=OpenCfmt(w,LineNo);
       for (i=0;i<LineLen(LineNo);i++) font[i]=fmt[i];
       CloseCfmt(w,LineNo);
    }

    return LineLen(LineNo);
}

/****************************************************************************
    TerGetLineU:
    This function returns the contents of the specified line.
    Either 'text' or 'font' argument can be NULL, if you do not wish to
    receive that information.
    The 'text' pointer receives the NULL terminated line text.
    The 'font' pointer receives the font id of each character of the line.
    The function returns the length of the line, if successful. Otherwise it return -1.
****************************************************************************/
int WINAPI _export TerGetLineU(HWND hWnd, long LineNo, LPWORD text, LPWORD font)
{
    LPBYTE ptr;
    LPWORD fmt;
    int   i;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (LineNo<0 || LineNo>=TotalLines) return -1;  // invalid line no

    //************* get the text data *************
    if (text) {
       ptr=pText(LineNo);
       fmt=OpenCfmt(w,LineNo);
       
       for (i=0;i<LineLen(LineNo);i++) {
         if (TerFont[fmt[i]].UcBase>0) text[i]=MakeUnicode(w,TerFont[fmt[i]].UcBase,ptr[i]);
         else                          text[i]=ptr[i];
       }
       text[i]=0;
       
       CloseCfmt(w,LineNo);
    }

    //************* fill in font data *************
    if (font) {
       fmt=OpenCfmt(w,LineNo);
       for (i=0;i<LineLen(LineNo);i++) font[i]=fmt[i];
       CloseCfmt(w,LineNo);
    }

    return LineLen(LineNo);
}

/****************************************************************************
    GetTerLine:
    This function is being phased out, use TerGetLine function instead.
****************************************************************************/
int WINAPI _export GetTerLine(HWND hWnd, long LineNo, LPBYTE text, LPBYTE font)
{
    if (font) {
       WORD wFont[MAX_WIDTH+1];
       int i;
       int len=TerGetLine(hWnd,LineNo,text,wFont);
       for (i=0;i<len;i++) font[i]=(BYTE)wFont[i];
       return len;
    }
    else return TerGetLine(hWnd,LineNo,text, NULL); // simply call the new funtion
}

/****************************************************************************
    TerSetLine:
    You can use this function replace text and font information for the
    current line.
****************************************************************************/
BOOL WINAPI _export TerSetLine(HWND hWnd, long LineNo, LPBYTE text, LPWORD font)
{
    return TerSetLineEx(hWnd,LineNo,text,NULL,font);
}

/****************************************************************************
    TerSetLineEx:
    You can use this function replace text and font information for the
    current line.
    The 'text' argument must point to a buffer containing new text for the.
    The length of the buffer may not exceed 300 bytes.
    The 'font' argument specifies the font ids for each character of the.
    If the 'font' field is NULL, the previous font ids are used.
    This function return TRUE when successful.
****************************************************************************/
BOOL WINAPI _export TerSetLineEx(HWND hWnd, long LineNo, LPBYTE text, LPBYTE LeadText, LPWORD font)
{
    LPBYTE ptr,lead;
    LPWORD fmt;
    int   i,len,OldLen;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // increment the total lines if needed
    if (LineNo==TotalLines) {
       if (!CheckLineLimit(w,TotalLines+1)) {
          return PrintError(w,MSG_MAX_LINES_EXCEEDED,"SetTerLine");
       }
       TotalLines++;
       InitLine(w,LineNo);
       TerArg.modified++;                      // indicate that file has been modified
    }

    //************** apply the line data *************
    len=lstrlen(text);
    if (LineNo>=0 && LineNo<TotalLines) {
       if (len>MAX_WIDTH-1) len=MAX_WIDTH-1;
       if (len<0) len=0;

       OldLen=LineLen(LineNo);
       LineAlloc(w,LineNo,LineLen(LineNo),len); // allocate space

       if (len>0) {
          //************* store in text data *************
          OpenTextPtr(w,LineNo,&ptr,&lead);
          for (i=0;i<LineLen(LineNo);i++) {
             ptr[i]=text[i];
             if (mbcs && LeadText) lead[i]=LeadText[i];
          }
          CloseTextPtr(w,LineNo);


          //************* store in font data *************
          fmt=OpenCfmt(w,LineNo);
          if (len>OldLen) {                       // set the default font for the new characters
             for (i=OldLen;i<len;i++) {
               if (OldLen>0) fmt[i]=fmt[OldLen-1];
               else          fmt[i]=DEFAULT_CFMT;
               if (TerFont[fmt[i]].style&PICT) fmt[i]=DEFAULT_CFMT;
             }
          }
          if (font) {                            // apply the new font ids
             for (i=0;i<LineLen(LineNo);i++) {
                if (font[i]<=(WORD)TotalFonts && TerFont[font[i]].InUse) fmt[i]=font[i];
             }
          }
          CloseCfmt(w,LineNo);
          TerArg.modified++;
       }
    }

    return TRUE;
}

/****************************************************************************
    SetTerLine:
    This function is being phased out, use the TerSetLine function instead
****************************************************************************/
BOOL WINAPI _export SetTerLine(HWND hWnd, long LineNo, LPBYTE text, LPBYTE font)
{
    if (font) {        // move the font id to a WORD array and call TerSetLine
       WORD wFont[MAX_WIDTH+1];
       int i;
       PTERWND w;

       if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

       for (i=0;i<LineLen(LineNo);i++) wFont[i]=font[i];
       return TerSetLine(hWnd,LineNo,text,wFont);
    }
    else return TerSetLine(hWnd,LineNo,text,NULL);
}

/****************************************************************************
    TerGetLineInfo:
    Get the information about the speicified line
****************************************************************************/
BOOL WINAPI _export TerGetLineInfo(HWND hWnd, long LineNo, LPINT ParaId, LPINT CellId, LPINT ParaFID, LPINT x, LPINT y, LPINT height, LPDWORD lflags, UINT far *InfoFlags)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (LineNo==-1) LineNo=CurLine;

    if (LineNo<0 || LineNo>=TotalLines) return FALSE;

    if (ParaId) (*ParaId)=pfmt(LineNo);
    if (CellId) (*CellId)=cid(LineNo);
    if (ParaFID) (*ParaFID)=fid(LineNo);
    if (x) (*x)=LineX(LineNo);
    if (y) {
       (*y)=LineY(LineNo);
       if (TerArg.PageMode) {
          if (BorderShowing) (*y)-=TopBorderHeight;
          else if (!ViewPageHdrFtr) {
             int sect=GetSection(w,LineNo);
             int TopMargin=(int)(PrtResY*TerSect[sect].TopMargin);
             (*y)+=TopMargin;  // top margin not displaying
          }
          else {        // hdr/ftr showing, but not the page border - add the hidden top space
             int sect=GetSection(w,LineNo);
             (*y)+=TerSect1[sect].HiddenY;
          } 
       }      
    }
    if (height) (*height)=LineHt(LineNo);
    if (lflags) (*lflags)=LineFlags(LineNo);

    if (InfoFlags) {
       if (tabw(LineNo)) (*InfoFlags)=tabw(LineNo)->type;
       else              (*InfoFlags)=0;
    }

    return TRUE;
}

/****************************************************************************
    TerGetLineParam:
    Get the line parameters
****************************************************************************/
long WINAPI _export TerGetLineParam(HWND hWnd, long LineNo, int type)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return LP_ERROR;  // get the pointer to window data

    if (LineNo==-1) LineNo=CurLine;

    if (type==LP_LINE_FLAGS)       return LineFlags(LineNo);
    else if (type==LP_LINE_FLAGS2) return LineFlags2(LineNo);
    
    return LP_ERROR;
}

/****************************************************************************
    GetFontInfo:
    This function returns information about the specified font id.  The font
    id must be between 0 and TotalFonts -1.  The TotalFonts can be retrieved
    by calling the GetTerFields function.
    This function returns TRUE if succesful.  It returns false if the
    specified font id does not exist or is not in use.
****************************************************************************/
BOOL WINAPI _export GetFontInfo(HWND hWnd, int FontId, LPBYTE TypeFace, LPINT PointSize, UINT far *style)
{
    int TwipsSize;
    BOOL result;

    result=GetFontInfo2(hWnd,FontId,TypeFace,&TwipsSize,style);

    if (PointSize) (*PointSize)=TwipsToPoints(TwipsSize);

    return result;
}

/****************************************************************************
    GetFontInfo2:
    This function returns information about the specified font id.  The font
    id must be between 0 and TotalFonts -1.  The TotalFonts can be retrieved
    by calling the GetTerFields function.
    This function returns TRUE if succesful.  It returns false if the
    specified font id does not exist or is not in use.
****************************************************************************/
BOOL WINAPI _export GetFontInfo2(HWND hWnd, int FontId, LPBYTE TypeFace, LPINT TwipsSize, UINT far *style)
{
    PTERWND w;
    int sid;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data


    if (FontId>=0) {
       if (FontId>=TotalFonts || !TerFont[FontId].InUse) return FALSE;
       
       if (TypeFace)  lstrcpy(TypeFace,TerFont[FontId].TypeFace);
       if (TwipsSize) (*TwipsSize)=TerFont[FontId].TwipsSize;
       if (style)     (*style)=TerFont[FontId].style;
    }
    else {
       if ((sid=ParamIdToSID(w,FontId))<0) return FALSE;

       if (TypeFace)  lstrcpy(TypeFace,StyleId[sid].TypeFace);
       if (TwipsSize) (*TwipsSize)=StyleId[sid].TwipsSize;
       if (style)     (*style)=StyleId[sid].style;

    } 

    return TRUE;
}

/****************************************************************************
    TerGetFontParam:
    Get individual font info
****************************************************************************/
long WINAPI _export TerGetFontParam(HWND hWnd, int FontId, int type)
{
    PTERWND w;
    int sid;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
 
    if (FontId>=0) {
       if (FontId>=TotalFonts || !TerFont[FontId].InUse) return -1;
       
       if (type==FONTINFO_UCBASE)   return TerFont[FontId].UcBase;
       if (type==FONTINFO_CHARSET)  return TerFont[FontId].CharSet;
       
       if (type==FONTINFO_PICT_WIDTH)   return TerFont[FontId].PictWidth;   // in twips
       if (type==FONTINFO_PICT_HEIGHT)  return TerFont[FontId].PictHeight;  // in twips
       if (type==FONTINFO_ULINE_COLOR)  return (long)(DWORD)TerFont[FontId].UlineColor;  // in twips
       if (type==FONTINFO_AUX_ID)       return TerFont[FontId].AuxId;      // Aux id
       if (type==FONTINFO_FLAGS)        return TerFont[FontId].flags;      // FFLAG_ constants
       if (type==FONTINFO_FRAME_TYPE)   return TerFont[FontId].FrameType;    // PFRAME_ constants
       if (type==FONTINFO_FRAME_ID)     return TerFont[FontId].ParaFID;     // paragraph frame for the picture id
       if (type==FONTINFO_OFFSET)       return TerFont[FontId].offset;     // character offset
       if (type==FONTINFO_IS_PICT)      return (True(TerFont[FontId].style&PICT))?1:0; 
       if (type==FONTINFO_IS_OCX)       return (True(TerFont[FontId].style&PICT) && TerFont[FontId].ObjectType==OBJ_OCX)?1:0; 
       if (type==FONTINFO_IS_OLE)       return (True(TerFont[FontId].style&PICT) && TerFont[FontId].ObjectType!=OBJ_NONE)?1:0; 
    }
    else {
       if ((sid=ParamIdToSID(w,FontId))<0) return FALSE;
       if (type==FONTINFO_ULINE_COLOR)  return (long)(DWORD)StyleId[sid].UlineColor;  // in twips
       if (type==FONTINFO_OFFSET)       return StyleId[sid].offset;     // character offset
    } 
 
    return -1;
}
 
/****************************************************************************
    ParamIdToSID:
    convert a font or param id parameter to stylesheet id
****************************************************************************/
int ParamIdToSID(PTERWND w,int id)
{
    int sid;

    if (id==SID_NORMAL) sid=0;
    else if (id==SID_CUR) sid=CurSID;
    else sid=-id;

    if (sid>=TotalSID) sid=-1;

    return sid;
}
 
/****************************************************************************
    TerGetFontStyleId:
    This function returns the character style id associated with a font. The font
    id must be between 0 and TotalFonts -1.  The TotalFonts can be retrieved
    by calling the GetTerFields function.
    This function returns -1 if an error is encountered.
****************************************************************************/
int WINAPI _export TerGetFontStyleId(HWND hWnd, int FontId)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (FontId<0 || FontId>=TotalFonts || !TerFont[FontId].InUse) return -1;

    return TerFont[FontId].CharStyId;
}

/****************************************************************************
    TerSetFontStyleId:
    This function sets the character and paragraph style id for a font id.
    The font id must be between 0 and TotalFonts -1.  The TotalFonts can be retrieved
    by calling the GetTerFields function. The style id value can be -1 to leave
    them unchanged.
****************************************************************************/
BOOL WINAPI _export TerSetFontStyleId(HWND hWnd, int FontId, int CharStyId, int ParaStyId)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (FontId<0 || FontId>=TotalFonts || !TerFont[FontId].InUse) return FALSE;

    if (CharStyId>=0 && CharStyId<TotalSID && StyleId[CharStyId].InUse
       && StyleId[CharStyId].type==SSTYPE_CHAR) TerFont[FontId].CharStyId=CharStyId;

    if (ParaStyId>=0 && ParaStyId<TotalSID && StyleId[ParaStyId].InUse
       && StyleId[ParaStyId].type==SSTYPE_PARA) TerFont[FontId].ParaStyId=ParaStyId;

    return TRUE;
}

/****************************************************************************
    TerGetTextColor:
    This function returns text color and text background color.
****************************************************************************/
BOOL WINAPI _export TerGetTextColor(HWND hWnd, int FontId, LPDWORD TextColor, LPDWORD TextBackColor)
{
    PTERWND w;
    int sid;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (FontId>=0) {
       if (FontId>=TotalFonts || !TerFont[FontId].InUse) return FALSE;

       if (TextColor)   *TextColor=TerFont[FontId].TextColor;
       if (TextBackColor) *TextBackColor=TerFont[FontId].TextBkColor;
    }
    else {
       if ((sid=ParamIdToSID(w,FontId))<0) return FALSE;
    
       if (TextColor)   *TextColor=StyleId[sid].TextColor;
       if (TextBackColor) *TextBackColor=StyleId[sid].TextBkColor;
    }

    return TRUE;
}


/****************************************************************************
    StrToHandle:
    This function can be used by Visual Basic programs to convert a string
    data to global memory handle.

    This function allocates global memory block for the specified
    buffer length and copies the content of the buffer to the global
    memory block. The function returns the handle of the global memory
    block.  The return value is 0 if required amount of memory is not
    available.
****************************************************************************/
HANDLE WINAPI _export StrToHandle(BYTE huge *buf,long BufLen)
{
    BYTE huge * ptr;
    HANDLE hMem;
    long i;

    if ( (NULL==(hMem=GlobalAlloc(GMEM_MOVEABLE,BufLen)))
       || (NULL==(ptr=(BYTE huge *)GlobalLock(hMem))) ) {
       MessageBox(NULL,MsgString[MSG_OUT_OF_MEM],NULL,MB_OK);
       return 0;
    }

    for (i=0;i<BufLen;i++) ptr[i]=buf[i];   // copy the buffer

    GlobalUnlock(hMem);

    return hMem;
}

/****************************************************************************
    StrToHandleInChunks:
    This function can be used by Visual Basic programs to convert a string
    data to global memory handle in chunks.

    This function allocates global memory block for the specified
    buffer length when the first chunk is created.
    The function returns the handle of the global memory
    block.  The return value is 0 if required amount of memory is not
    available.
****************************************************************************/
HANDLE WINAPI _export StrToHandleInChunks(BYTE huge *buf,long BufLen,HANDLE hMem, long ChunkPos, long ChunkLen)
{
    BYTE huge * ptr;
    long i;

    if ((ChunkPos+ChunkLen)>BufLen) ChunkLen=BufLen-ChunkPos;
    if (ChunkLen<=0) return FALSE;

    if (ChunkPos==0) {
      if (NULL==(hMem=GlobalAlloc(GMEM_MOVEABLE,BufLen))) {
         MessageBox(NULL,MsgString[MSG_OUT_OF_MEM],NULL,MB_OK);
         return 0;
      }
    }

    if (NULL==(ptr=(BYTE huge *)GlobalLock(hMem))) {
         MessageBox(NULL,MsgString[MSG_OUT_OF_MEM],NULL,MB_OK);
         return 0;
    }

    for (i=0;i<ChunkLen;i++) ptr[ChunkPos+i]=buf[i];   // copy the buffer

    GlobalUnlock(hMem);

    return hMem;
}

/****************************************************************************
    HandleToStr:
    This function can be used by Visual Basic programs to copy a
    Global Memory block to a string.  The calling routine must expand
    the string to the required buffer length before calling this function.
    Example:
       buf = space(BufLen)

    This function frees the global handle after the copy operation.
    The return value a FALSE value if not successful.
****************************************************************************/
BOOL WINAPI _export HandleToStr(BYTE huge * buf,long BufLen,HANDLE hMem)
{
    BYTE huge * ptr;
    long i;

    if ( NULL==(ptr=(BYTE huge *)GlobalLock(hMem)) ) {
       MessageBox(NULL,MsgString[MSG_OUT_OF_MEM],NULL,MB_OK);
       return FALSE;
    }

    for (i=0;i<BufLen;i++) buf[i]=ptr[i];   // copy the buffer

    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return TRUE;
}

/****************************************************************************
    HandleToIntArray:
    This function can be used by Visual Basic programs to copy a
    Global Memory block to an integer array.
    This function frees the global handle after the copy operation.
    The function returns the number of unicode characters transferred.
****************************************************************************/
int WINAPI _export HandleToIntArray(LPWORD buf,long BufLen,HANDLE hMem)
{
    LPWORD ptr;
    long i;

    if ( NULL==(ptr=(LPWORD)GlobalLock(hMem)) ) {
       MessageBox(NULL,MsgString[MSG_OUT_OF_MEM],NULL,MB_OK);
       return 0;
    }

    BufLen=BufLen/2;   // in number of words
    for (i=0;i<BufLen;i++) buf[i]=ptr[i];   // copy the buffer

    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return BufLen;
}

/****************************************************************************
    HandleToStrInChunks:
    This function can be used by Visual Basic programs to copy a chunk of
    Global Memory block to a string.  The calling routine must expand
    the string to the required buffer length before calling this function.
    Example:
       buf = space(BufLen)

    The global memory handle is freed when last chunk is retrieved.
****************************************************************************/
BOOL WINAPI _export HandleToStrInChunks(BYTE huge * buf,long BufLen,HANDLE hMem, long ChunkPos, long ChunkLen)
{
    BYTE huge * ptr;
    long i;

    if ( NULL==(ptr=(BYTE huge *)GlobalLock(hMem)) ) {
       MessageBox(NULL,MsgString[MSG_OUT_OF_MEM],NULL,MB_OK);
       return FALSE;
    }

    if ((ChunkPos+ChunkLen)>BufLen) ChunkLen=BufLen-ChunkPos;
    if (ChunkLen<=0) return FALSE;

    for (i=0;i<ChunkLen;i++) buf[i]=ptr[i+ChunkPos];   // copy the buffer

    GlobalUnlock(hMem);
    if ((ChunkPos+ChunkLen)>=BufLen) GlobalFree(hMem);

    return TRUE;
}

/****************************************************************************
    TerCommand:
    This function executes a ter menu command.
****************************************************************************/
BOOL WINAPI _export TerCommand(HWND hWnd, int CmdId)
{
    return TerCommand2(hWnd,CmdId,TRUE);
}

/****************************************************************************
    TerCommand2:
    This function executes a ter menu command.
****************************************************************************/
BOOL WINAPI _export TerCommand2(HWND hWnd, int CmdId, BOOL send)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
   
    if (InDialogBox) return FALSE;
    
    if (TerMenuEnable(hWnd,CmdId)==MF_ENABLED) {
        if (send) SendMessage(hWnd,WM_COMMAND,CmdId,0L);
        else      PostMessage(hWnd,WM_COMMAND,CmdId,0L);
    }
    return TRUE;
}

/****************************************************************************
    VbxProcessMessage:
    This routine is called by the VBX control process to process a TER
    window message.
****************************************************************************/
LRESULT CALLBACK _export VbxProcessMessage(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,LPINT processed)
{
    PTERWND w;
    LRESULT result;
    int WinNo;

    (*processed) = FALSE;                     // reset

    // process the create message
    if (message==WM_CREATE) {
        (*processed)=TRUE;                    // vbx message processed
        if (InitTer(hWnd,(CREATESTRUCT far *)lParam)) return 0;
        else                                          return -1;
    }

    // get the window number in non-standard window table
    for (WinNo=0;WinNo<TotalWinPtrs;WinNo++) if (WinPtr[WinNo].InUse && WinPtr[WinNo].hWnd==hWnd) break;
    if (WinNo>=TotalWinPtrs) return (LRESULT)NULL;
    w=WinPtr[WinNo].data;

    VbxMessageProcessed=TRUE;                 // will be set to FALSE if not processed

    result=TerWndProc(hWnd,message,wParam,lParam);  // process the message

    (*processed)=TRUE;
    if (WinPtr[WinNo].InUse && WinPtr[WinNo].hWnd==hWnd) {
         (*processed)=VbxMessageProcessed; // indicate if the message was processed
         VbxMessageProcessed=TRUE;         // to handle recursive message calls
    }

    return result;
}

/****************************************************************************
    TerDefWindowProc:
    This routine calls the default window proceedure for unprocessed messages.
****************************************************************************/
LRESULT TerDefWindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    BYTE class[20];

    if (IsWin95_98_ME && message==WM_PRINT) return 0; // These kernel crash on these messages

    GetClassName(hWnd,class,sizeof(class)-1);
    if (lstrcmpi(class,TER_CLASS)==0) return (DefWindowProc(hWnd, message, wParam, lParam));

    if (NULL==(w=GetWindowPointer(hWnd))) return (LRESULT)NULL;
    if (VbxControl) VbxMessageProcessed=FALSE;

    return (LRESULT)NULL;
}

/****************************************************************************
    TerWndProc:
    This routine process the messages coming to the main TER window.
****************************************************************************/
LRESULT CALLBACK _export TerWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    PTERWND w;
    BOOL edit,WordWrap;
    WORD flags;
    int i,CmdId;
    MSG msg;
    HBRUSH hBr;
    LRESULT lresult;

    //DispMsg(message);


    // create a new window
    if (message==WM_CREATE) {
        if (InitTer(hWnd,(CREATESTRUCT far *)lParam)) return 0;
        else                                          return -1;
    }

    w=GetWindowPointer(hWnd);  // get the window control data area pointer

    if (w==NULL || WindowBeingCreated || HoldMessages || WaitForOle || !TerArg.open)
            return (TerDefWindowProc(hWnd, message, wParam, lParam));

    MessageId=message;                        // store the current message id
    if (message==WM_COMMAND) CmdId=GET_WM_COMMAND_ID(wParam,lParam);
    else                     CmdId=0;
    CommandId=CmdId;                          // store in the global variable also
   
     
    if (InPrintPreview) {          // disable certain commands in print preview mode
       if (message==WM_CHAR || message==WM_DROPFILES) return (TerDefWindowProc(hWnd, message, wParam, lParam));
       if (message>=WM_MOUSEFIRST && message<=WM_MOUSELAST) return (TerDefWindowProc(hWnd, message, wParam, lParam));
       if (message==WM_COMMAND && CmdId!=ID_PGUP && CmdId!=ID_PGDN && CmdId!=ID_PRINT_PREVIEW) return (LRESULT)NULL;
    }

    if (!TerArg.ReadOnly && IsImeMessage(w,message,wParam,lParam)) return (LRESULT)NULL;   // Ime message processed

    //*********** translate accelerator ***********
    if (TerTranslateAccelerator(w,message,wParam,lParam)) return TRUE; // accelerator translated

    if ((CurLine>=0 && CurLine<TotalLines) && (LineTextAngle(w,CurLine)!=0 || LineFlags2(CurLine)&LFLAG2_RTL)) CmdId=CommandId=XlateCommandId(w,CmdId);

    if (SendPreprocessMessage(w,message,wParam,lParam)) return TRUE;   // return TRUE if the command is to be ignored
    
    if (DoDragonMsg(w,hWnd,message,wParam,lParam,&lresult)) return lresult;   // interface with dragon speech library
    

    edit=!(TerArg.ReadOnly);                  // copy to local variables
    WordWrap=TerArg.WordWrap;
    CaretPositioned=FALSE;                    // TRUE when the caret is positiond
    if (TerFlags4&TFLAG4_UNDO_WINDOW_OVERFLOW && (message==WM_COMMAND || message==WM_CHAR)) DocHeight=GetDocHeight(w);

    switch (message) {
        case WM_INITMENU:                     // initialize menu
            if (WheelShowing) ResetWheel(w);
            if ((GetMenu(hWnd)!=(HMENU)wParam)) break;
            if (TerArg.ShowMenu) TerInitMenu(w,(HMENU)wParam);
            break;

        case WM_CHAR:                         // process an ASCII character
            if (!edit && !EditingInputField(w,false,false)) break;

            // is highlighting in progress?
            if (HilightType!=HILIGHT_OFF && (StretchHilight || TblSelCursShowing) && !IgnoreMouseMove) {
               MessageBeep(0);
               break;
            }

            // show the cursor if hidden
            if (!CaretEnabled && UseCaret(w)) InitCaret(w);
            if (!CaretEngaged && !InPrintPreview) EngageCaret(w,0);
            if (WheelShowing) ResetWheel(w);

            switch (wParam) {
               case VK_RETURN:                // process the <CR> key
                  TerReturn(w);break;
               case VK_BACK:                  // process the backspace key
                  if (!InAccelerator && (edit || EditingInputField(w,false,true))) TerBackSpace(w);break;     // to allow the use of ctrl+h for a speed key involving dialog box, such as search/replace
               case VK_TAB:                   // process the tab key. Because of OCX message routing we also need to process it as an accelerator
                    TerInsertTab(w);break;
               case VK_ESCAPE:                // escape not processed
                  break;
               default:                       // character input
                  // keys below 26 might have been sent from disabled accelerators
                  if ((BYTE)wParam>26)         TerAscii(w,(char)wParam,0);  // filter the control characters
            }
            break;

        case WM_IME_CHAR:                 // mbcs character input - used only when inline ime is diabled
           if (!TerArg.ReadOnly) ImeChar(w,wParam);break;
        
        case WM_DEADCHAR:                 // mbcs character input - used only when inline ime is diabled
           DeadChar=wParam;
           return (TerDefWindowProc(hWnd, message, wParam, lParam));

        case WM_KEYDOWN:                      // process keydown
           KeyDownUniChar=0;                  // translate unicode character
           if (false && True(TerFlags6&TFLAG6_XLATE_UNICODE) && DeadChar!=0) {    // ToUnicodeEx interfres of Windows processing of dead-charcter
              BYTE KeyState[256];             // peek at the unicode character
              HKL  hkl=GetKeyboardLayout(0);
              GetKeyboardState(KeyState);
              if (ToUnicodeEx(wParam,HIWORD(lParam),KeyState,&KeyDownUniChar,1,0,hkl)!=1) KeyDownUniChar=0;
           }  
           DeadChar=0;                       // dead-char applicable for immediate keyboard input only
           return (TerDefWindowProc(hWnd, message, wParam, lParam));

         
        case WM_KEYUP:                         // process an ASCII character
            switch (wParam) {
               case VK_SHIFT:                  // process the shift key
                  StretchHilight=FALSE;
                  if (HilightType==HILIGHT_CHAR) AdjustHilight(w);  // adjust the highlighting area
                  if (!StretchHilight && HilightBegRow==HilightEndRow && HilightBegCol==HilightEndCol) HilightType=HILIGHT_OFF;
                  break;

               default:                       // character input
                  return (TerDefWindowProc(hWnd, message, wParam, lParam));
            }
            break;

        case WM_VSCROLL:                      // vertical scroll bar commands
            if (CaretEngaged) DisengageCaret(w); // disengage the caret
            if (WheelShowing) ResetWheel(w);
            if (PictureClicked) {            // disable any picture hilighting
               HilightType=HILIGHT_OFF;
               PictureClicked=FALSE;
            }

            switch (SCROLL_MSG(wParam,lParam)) {
                case SB_LINEDOWN:             // scroll window one line down
                   TerWinDown(w);break;
                case SB_LINEUP:               // scroll window one line up
                   TerWinUp(w);break;
                case SB_PAGEDOWN:             // next page
                   TerPageDn(w,FALSE);break;
                case SB_PAGEUP:               // previous page
                   TerPageUp(w,FALSE);break;
                case SB_THUMBPOSITION:        // absolute line position when the thump is positioned
                   VerThumbPos(w,wParam,lParam);
                   break;

                case SB_THUMBTRACK:            // thumb tracking
                   if (TerFlags2&TFLAG2_VERT_THUMB_TRACK) {
                      TerOpFlags|=TOFLAG_CONTINUOUS_SCROLL;
                      VerThumbPos(w,wParam,lParam);
                      ResetLongFlag(TerOpFlags,TOFLAG_CONTINUOUS_SCROLL);
                   }
                   else if (PagesShowing && !InPrintPreview) DrawPageBox(w,hTerDC,THUMB_POS(wParam,lParam));
                   break;

                case SB_ENDSCROLL:            // end of scrolling
                   if ((PageHasControls && hScrollBM) || PageBoxShowing) PaintTer(w);  // do painting to draw the controls
                   ContinuousScroll=FALSE;
                   break;
                default:
                   break;
            }
            break;
        case WM_HSCROLL:                      // horizontal scroll bar commands
            if (CaretEngaged) DisengageCaret(w); // disengage the caret
            if (WheelShowing) ResetWheel(w);
            if (PictureClicked) {            // disable any picture hilighting
               HilightType=HILIGHT_OFF;
               PictureClicked=FALSE;
            }

            switch (SCROLL_MSG(wParam,lParam)) {
                case SB_LINERIGHT:            // scroll window one column toward right
                   TerPageRight(w,FALSE);break;
                case SB_LINELEFT:             // scroll window one column toward left
                   TerPageLeft(w,FALSE);break;
                case SB_PAGERIGHT:            // next horizontal page
                   TerPageRight(w,TRUE);break;
                case SB_PAGELEFT:             // previous horizontal page
                   TerPageLeft(w,TRUE);break;
                case SB_THUMBPOSITION:        // absolute column position as the thump moves
                   if (!InPrintPreview) TerPageHorz(w,'T',THUMB_POS(wParam,lParam));break;
                default:
                   break;
            }
            break;

        case WM_COMMAND:
            // test if it is a embedded control message
            for (i=0;i<TotalFonts;i++) {
               if (IsControl(w,i)) {
                  if (TerFont[i].AuxId==CmdId || (TerFont[i].hWnd && GetDlgCtrlID(TerFont[i].hWnd)==CmdId)) {
                     CurCtlId=CmdId;     // currently selected control
                     hCurCtlWnd=TerFont[i].hWnd;
                     return (LRESULT) SendMessageToParent(w,WM_COMMAND,wParam,lParam,TRUE);
                  }
               }
            }
            if (CmdId==ID_TAB && GetFocus()==hCurCtlWnd) return SendMessage(hCurCtlWnd,WM_KEYDOWN,VK_TAB,0L);  // Delphi 6 translates tab in the input-field control to tab in TE control, so redirect it to the input-field

            // check if it is a popmenu command
            if (CmdId>=IDP_FIRST_POPUP && CmdId<=IDP_LAST_POPUP) {
               DoPopupSelection(w,CmdId);
               break;
            } 

            // check for help
            if (TerHelpWanted) {              // show contect sensitive help if requrested
                WinHelp(hWnd,TerHelpFile,HELP_CONTEXT,wParam);
                TerHelpWanted=FALSE;
                break;
            }

            // check if speed key enabled
            if (InAccelerator && CmdId>=ID_FIRST_COMMAND && CmdId<(ID_FIRST_COMMAND+MAX_SPEED_KEYS) && !SpeedKeyEnabled[CmdId-ID_FIRST_COMMAND]) {
               CommandSkipped=TRUE;
               return (LRESULT)NULL;
            }

            // is highlighting in progress?
            if (HilightType!=HILIGHT_OFF && (StretchHilight || TblSelCursShowing) && !IgnoreMouseMove) {
               MessageBeep(0);
               break;
            }

            // show the cursor if hidden
            if (!CaretEnabled && UseCaret(w)) InitCaret(w);
            if (!CaretEngaged && !InPrintPreview) EngageCaret(w,CmdId);
            if (WheelShowing) ResetWheel(w);
            InAutoComp=false;                 // reset auto comp

            switch (CmdId) {
                //*********************************************************
                // Navigation commands
                //*********************************************************
                case ID_PGUP:                 // process page-up key
                    TerPageUp(w,TRUE);break;
                case ID_PGDN:                 // process page-down key
                    TerPageDn(w,TRUE);break;
                case ID_UP:                   // process the up arrow key
                    TerUp(w);break;
                case ID_CTRL_UP:              // process the up arrow + ctrl
                    TerCtrlUp(w);break;
                case ID_DOWN:                 // process the down arrow key
                    TerDown(w);break;
                case ID_CTRL_DOWN:            // process the down arrow + ctrl
                    TerCtrlDown(w);break;
                case ID_LEFT:                 // process the left arrow
                    TerLeft(w);break;
                case ID_RIGHT:                // process the right arrow
                    TerRight(w);break;
                case ID_RETURN:               // process the return key if translated
                    if (edit) TerReturn(w);break;
                case ID_TAB:                  // process the tab key. Because of OCX message routing, we need to process it as an accelerator
                case ID_CTRL_TAB:             // process the control tab key.
                    TerInsertTab(w);break;
                case ID_BACK_TAB:             // process the backtab (shift tab) key
                    TerBackTab(w);break;
                case ID_LINE_BEGIN:           // position at the beginning of the line
                    TerBeginLine(w);break;
                case ID_LINE_END:             // position at the end of the line
                    TerEndLine(w);break;
                case ID_FILE_BEGIN:           // position at the beginning of the file
                    TerBeginFile(w);break;
                case ID_FILE_END:             // position at the end of the file
                    TerEndFile(w);break;
                case ID_NEXT_WORD:            // position at the next word
                    TerNextWord(w);break;
                case ID_PREV_WORD:            // position at the previous word
                    TerPrevWord(w,TRUE);break;
                case ID_DEL_PREV_WORD:        // delete the previous word
                    if (edit) TerDelPrevWord(w);break;

                //*********************************************************
                //Character and line deletion/insertion/split/join commands
                //*********************************************************
                case ID_DEL:                  // delete a character
                    if (edit || EditingInputField(w,true,false)) TerDel(w);break;
                case  ID_INS_AFT:             // insert a line after the current line
                    if (edit && !WordWrap) TerInsertLineAft(w);break;
                case  ID_INS_BEF:             // insert a line before the current line
                    if (edit && !WordWrap) TerInsertLineBef(w);break;
                case  ID_DEL_LINE:            // delete the current line
                    if (edit) TerDeleteLine(w);break;
                case  ID_SPLIT_LINE:          // split line
                    if (edit && !WordWrap) TerSplitLine(w,0,TabAlign,TRUE);break;
                case  ID_JOIN_LINE:           // join lines
                    if (edit && !WordWrap) TerJoinLine(w);break;

                //*********************************************************
                // Block highlighting and cut/paste and other edit functions
                //*********************************************************
                case  ID_HILIGHT_LINE:        // begin/end line block
                    TerSetLineHilight(w);
                    break;
                case  ID_COPY:                // copy/cut a text block to clipboard
                    CopyToClipboard(w,CmdId,TRUE);break;
                case  ID_CUT:
                    if (edit) CopyToClipboard(w,CmdId,TRUE);break;
                case  ID_PASTE:               // paste from clipboard
                    if (edit) CopyFromClipboard(w,0);break;
                case  ID_PASTE_TEXT:           // paste from clipboard in text format
                    if (edit) CopyFromClipboard(w,CF_TEXT);break;
                case  ID_PASTE_SPEC:          // paste picture from clipboard
                    if (edit) TerPasteSpecial(w);break;
                case  ID_EDIT_PICT:           // edit picture size
                    if (edit) EditPicture(w);break;

                case  ID_BLOCK_COPY:          // copy a hilighted block
                    if (edit) BlockCopy(w);break;
                case  ID_BLOCK_MOVE:          // move a hilighted block
                    if (edit) BlockMove(w);break;

                case  ID_EDIT_OLE:            // edit an ole object
                    if (edit) TerEditOle(w,TRUE);break;

                case  ID_SELECT_ALL:          // select the entire document
                    HilightType=HILIGHT_CHAR;
                    HilightBegRow=HilightBegCol=0;
                    HilightEndRow=TotalLines-1;
                    HilightEndCol=LineLen(HilightEndRow);
                    StretchHilight=FALSE;
                    PaintTer(w);break;

                //*********************************************************
                // Insert functions
                //*********************************************************
                case  ID_PAGE_BREAK:         // insert a hard page break
                    if (edit) TerPageBreak(hWnd,TRUE);break;
                case  ID_COL_BREAK:          // insert a hard column break
                    if (edit) TerColBreak(hWnd,TRUE);break;
                case  ID_SECT_BREAK:         // insert a section break
                    if (edit) TerSectBreak(hWnd,TRUE);break;
                case  ID_EMBED_PICT:         // insert a picture from a bitmap file
                    if (edit) TerInsertPictureFile(hWnd,NULL,TRUE,ALIGN_BOT,TRUE);break;
                case  ID_LINK_PICT:           // link a picture from a bitmap file
                    if (edit) TerInsertPictureFile(hWnd,NULL,FALSE,ALIGN_BOT,TRUE);break;
                case  ID_INSERT_OBJECT:       // insert an ole object
                    if (edit) TerInsertObject(w,-1);break;
                case  ID_INSERT_PARA_FRAME:   // insert a paragraph frame
                    if (edit) TerInsertParaFrame(hWnd,-1,-1,-1,-1,TRUE);break;
                case  ID_INSERT_DRAW_OBJECT:  // insert a drawing object
                    if (edit) TerInsertDrawObject(hWnd,-1,-1,-1,-1,-1,1);break;
                case  ID_INSERT_PAGE_NUMBER:  // insert a page number
                    if (edit) InsertDynField(w,FIELD_PAGE_NUMBER,NULL);break;
                case  ID_INSERT_PAGE_COUNT:   // insert page count
                    if (edit) InsertDynField(w,FIELD_PAGE_COUNT,NULL);break;
                case  ID_INSERT_DATE_TIME:    // insert page count
                    if (edit) TerInsertDateTime(hTerWnd,NULL,TRUE);break;
                case  ID_INSERT_TOC:         // insert table of content
                    if (edit) TerInsertToc(hTerWnd,TRUE);break;
                case  ID_INSERT_FNOTE:        // insert a footnote page number
                    if (edit) TerInsertFootnote(hWnd,0,NULL,0,TRUE);break;
                case  ID_INSERT_ENOTE:        // insert a endnote page number
                    if (edit) TerInsertFootnote2(hWnd,0,NULL,0,FALSE,TRUE);break;
                case  ID_INSERT_BOOKMARK:     // insert a bookmark
                    if (edit) InsertBookmark(w);break;
                case  ID_INSERT_DATA_FIELD:   // insert a datafield
                    if (edit) TerInsertField(hWnd,NULL,NULL,TRUE);break;
                case  ID_INSERT_INPUT_FIELD:   // insert a text input formfield
                    //MAK
                    if (edit) TerInsertTextInputField(hWnd,NULL,NULL,0,TRUE,NULL,0,0,0,TRUE,0, NULL, (LPBYTE)"", (LPBYTE)"EDIT",TRUE);break;
                    //if (edit) TerInsertTextInputField(hWnd,NULL,NULL,0,TRUE,NULL,0,0,0,TRUE,TRUE);break;
                case  ID_INSERT_CHECKBOX:      // insert a checkbox
                    if (edit) TerInsertCheckBoxField(hWnd,NULL,0,FALSE,TRUE,TRUE);break;
                case  ID_INSERT_HLINK:         // insert a hyperlink field
                    if (edit) InsertHyperlink(w);break;
                case  ID_INSERT_NBSPACE:      // insert a non-breaking space
                    if (edit) TerAscii(w,NBSPACE_CHAR,0);break;
                case  ID_INSERT_NBDASH:      // insert a non-breaking dash
                    if (edit) TerAscii(w,NBDASH_CHAR,0);break;
                case  ID_INSERT_HYPH:        // insert a optional hyphen character
                    if (edit) TerAscii(w,HYPH_CHAR,0);break;

                //*********************************************************
                // Search/Replace functions
                //*********************************************************
                case  ID_SEARCH:              // start string search
                    TerSearchString(w);break;
                case  ID_SEARCH_FOR:          // search a previous string in forward direction
                    TerSearchForward(w);break;
                case  ID_SEARCH_BACK:        // search a previous string in backward direction
                    TerSearchBackward(w);break;
                case  ID_REPLACE:            // replace a string
                    if (edit) TerReplaceString(w);break;

                //*********************************************************
                // I/O functions
                //*********************************************************
                case  ID_NEW:                // clear the existing document
                    if (edit) TerNew(w,"");break;
                case  ID_OPEN:               // open the new document
                    TerOpen(w);break;
                case  ID_SAVE:               // save the modified text
                    if (edit || ProtectForm) {
                       if (lstrlen(DocName)>0) TerSave(w,DocName,TRUE);
                       else                    TerSaveAs(w,DocName);
                    }
                    break;
                case  ID_SAVEAS:             // save the modified text, as user for the file name
                    TerSaveAs(w,DocName);break;
                case  ID_QUIT:               // quit TER session
                    if (TerArg.UserCanClose) CloseTer(hTerWnd,FALSE);
                    return (LRESULT)NULL;
                case  ID_PRINT:              // print text
                    //TerPrint(hTerWnd,TRUE);break;
                    TerPrintOptions(w,true);break;
                case  ID_PRINT_PREVIEW:      // print preview
                    TerPrintPreviewMode(hTerWnd,ShowPvToolbar);
                    SendActionMessage(w,message,wParam,lParam);        // send TER_ACTION message
                    return (LRESULT)NULL;

                case  ID_PAGE_OPTIONS:       // accepts page layout options from the users
                    TerPageOptions(w);break;
                case  ID_PRINT_OPTIONS:      // accepts printer options from the users
                    TerPrintOptions(w,false);break;

                //*********************************************************
                // Charracter Formatting Functions
                //********************************************************
                case ID_CHAR_NORMAL:          // turn off all character formats
                    if (edit) {
                       flags=BOLD|ULINE|ULINED|ITALIC|STRIKE|SUPSCR|SUBSCR;  // styles to turnoff
                       if (ShowHiddenText) flags|=HIDDEN;
                       if (!ProtectionLock) flags|=PROTECT;
                       SetTerCharStyle(hWnd,flags,FALSE,TRUE);
                    }
                    break;

                case ID_BOLD_ON:              // turn on the bold formatting
                    if (edit) SetTerCharStyle(hWnd,BOLD,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_ULINE_ON:             // turn on the underline formatting
                    if (edit) SetTerCharStyle(hWnd,ULINE,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_ULINED_ON:            // turn on the double underline formatting
                    if (edit) SetTerCharStyle(hWnd,ULINED,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_ITALIC_ON:            // turn on the italic formatting
                    if (edit) SetTerCharStyle(hWnd,ITALIC,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_STRIKE_ON:            // turn on the strike formatting
                    if (edit) SetTerCharStyle(hWnd,STRIKE,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_SUPSCR_ON:            // turn on the superscript formatting
                    if (edit) SetTerCharStyle(hWnd,SUPSCR,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_SUBSCR_ON:            // turn on the superscript formatting
                    if (edit) SetTerCharStyle(hWnd,SUBSCR,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_CHAR_SPACE:            // turn on the superscript formatting
                    if (edit) TerSetCharSpace(hWnd,TRUE,0,TRUE);break;
                case ID_HIDDEN_ON:            // turn on the superscript formatting
                    if (edit) SetTerCharStyle(hWnd,HIDDEN,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_BOX_ON:               // turn on the box formatting
                    if (edit) SetTerCharStyle(hWnd,CHAR_BOX,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_PROTECT_ON:            // turn on the text protection
                    if (edit) SetTerCharStyle(hWnd,PROTECT,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_HLINK_ON:              // turn on the bold formatting
                    if (edit) SetTerCharStyle(hWnd,HLINK,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_CAPS_ON:              // turn on the caps formatting
                    if (edit) SetTerCharStyle(hWnd,CAPS,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;
                case ID_SCAPS_ON:              // turn on the scaps formatting
                    if (edit) SetTerCharStyle(hWnd,SCAPS,MF_UNCHECKED==TerMenuSelect(hWnd,CmdId),TRUE);break;

                case ID_FONTS:                // display/accept fonts
                    if (edit) TerFonts(w);break;

                case ID_COLOR:                // display/accept text color
                    if (edit) TerColors(w,TRUE);break;
                case ID_BK_COLOR:             // display/accept text background color
                    if (edit) TerColors(w,FALSE);break;
                case ID_ULINE_COLOR:         // display/accept text color
                    if (edit) TerSetUlineColor(hWnd,true,CLR_AUTO,true);break;

                //*********************************************************
                // Paragraph Formatting Functions
                //********************************************************
                case ID_PARA_NORMAL:          // turn off all paragraph formats
                    if (edit) ParaNormal(hWnd,TRUE);break;
                case ID_CENTER:               // paragraph centering
                    if (edit) SetTerParaFmt(hWnd,CENTER,!TerMenuSelect(hWnd,ID_CENTER),TRUE);break;
                case ID_LEFT_JUSTIFY:        // right justify the paragraph
                    if (edit) SetTerParaFmt(hWnd,LEFT,!TerMenuSelect(hWnd,ID_LEFT_JUSTIFY),TRUE);break;
                case ID_RIGHT_JUSTIFY:        // right justify the paragraph
                    if (edit) SetTerParaFmt(hWnd,RIGHT_JUSTIFY,!TerMenuSelect(hWnd,ID_RIGHT_JUSTIFY),TRUE);break;
                case ID_JUSTIFY:              // justify left/right
                    if (edit) SetTerParaFmt(hWnd,JUSTIFY,!TerMenuSelect(hWnd,ID_JUSTIFY),TRUE);break;
                case ID_DOUBLE_SPACE:         // double spacing between the lines
                    if (edit) SetTerParaFmt(hWnd,DOUBLE_SPACE,!TerMenuSelect(hWnd,ID_DOUBLE_SPACE),TRUE);break;
                case ID_PARA_KEEP:            // keep the paragraph in one page
                    if (edit) SetTerParaFmt(hWnd,PARA_KEEP,!TerMenuSelect(hWnd,ID_PARA_KEEP),TRUE);break;
                case ID_PARA_KEEP_NEXT:            // keep it with the next paragraph
                    if (edit) SetTerParaFmt(hWnd,PARA_KEEP_NEXT,!TerMenuSelect(hWnd,ID_PARA_KEEP_NEXT),TRUE);break;
                case ID_WIDOW_ORPHAN:          // widow orphan
                    if (edit) TerSetPflags(hWnd,PFLAG_WIDOW,!TerMenuSelect(hWnd,ID_WIDOW_ORPHAN),TRUE);break;
                case ID_PAGE_BREAK_BEFORE:     // page break before
                    if (edit) TerSetPflags(hWnd,PFLAG_PAGE_BREAK,!TerMenuSelect(hWnd,ID_PAGE_BREAK_BEFORE),TRUE);break;
                case ID_LEFT_INDENT:          // create left indentation
                    if (edit) ParaLeftIndent(hWnd,TRUE,TRUE);break;
                case ID_LEFT_INDENT_DEC:          // create left indentation
                    if (edit) ParaLeftIndent(hWnd,FALSE,TRUE);
                    break;

                case ID_RIGHT_INDENT:         // create right indentation
                    if (edit) ParaRightIndent(hWnd,TRUE,TRUE); break;
                case ID_HANGING_INDENT:       // create hanging indent
                    if (edit) ParaHangingIndent(hWnd,TRUE,TRUE);break;
                case ID_PARA_BORDER:          // Edit paragraph borders
                    if (edit) TerParaBorder(w);break;
                case ID_PARA_SPACING:         // Edit paragraph spacing
                    if (edit) TerParaSpacing(w);break;
                case ID_BULLET:               // Toggle paragraph bullet
                    if (edit) TerSetListBullet(hWnd,MF_UNCHECKED==TerMenuSelect(hWnd,ID_BULLET),LIST_BLT,-1,1,"","",true);break;
                case ID_PARA_NBR:             // Toggle paragraph numbering
                    if (edit) TerSetListBullet(hWnd,MF_UNCHECKED==TerMenuSelect(hWnd,ID_PARA_NBR),LIST_DEC,-1,1,"",".",true);break;
                case ID_LIST_LEVEL_INC:       // increase list level
                    if (edit) TerSetListLevel(hWnd,-1,1,true);break;  // increase by one level
                case ID_LIST_LEVEL_DEC:       // decrease list level
                    if (edit) TerSetListLevel(hWnd,-1,-1,true);break;  // decrease by one level
                case ID_PARA_LIST:             // Toggle paragraph numbering
                    if (edit) TerSetParaList(hTerWnd,TRUE,-1,0,0,TRUE);break;
                case ID_PARA_BK_COLOR:        // set paragraph background color
                    if (edit) TerSetParaBkColor(hWnd,TRUE,0,TRUE);break;
                case ID_PARA_RTL:             // set paragraph text flow
                    if (edit) TerSetParaTextFlow(hWnd,TRUE,0,TRUE);break;

                //*********************************************************
                // Stylesheet functions
                //********************************************************
                case  ID_EDIT_STYLE:         // edit stylesheet
                    if (edit) TerEditStyle(hWnd,TRUE,NULL,TRUE,0,TRUE);break;
                case ID_CHAR_STYLE:           // accept new character style
                    if (edit) TerSelectCharStyle(hTerWnd,-1,TRUE);break;
                case ID_PARA_STYLE:           // accept new paragraph style
                    if (edit) TerSelectParaStyle(hTerWnd,-1,TRUE);break;

                //*********************************************************
                // list and listoverride functions
                //********************************************************
                case  ID_CREATE_LIST:         // create a list item
                    if (edit) TerEditList(hTerWnd, TRUE, -1,TRUE,NULL,TRUE,0L);break;
                case  ID_EDIT_LIST:          // edit a list item
                    if (edit) TerEditList(hTerWnd, FALSE, -1, TRUE,NULL,FALSE,0L);break;
                case  ID_CREATE_LIST_OR:     // create a list override
                    if (edit) TerEditListOr(hTerWnd, TRUE,-1,TRUE,1,TRUE,0);break;
                case  ID_EDIT_LIST_OR:       // edit a list override
                    if (edit) TerEditListOr(hTerWnd, FALSE,-1,TRUE,1,TRUE,0);break;
                case  ID_EDIT_LIST_LEVEL:    // edit a list level
                    if (edit) TerEditListLevel(hTerWnd, TRUE,-1,0,1,0,0,"",0,0); break;


                //*********************************************************
                // Table functions
                //********************************************************
                case ID_TABLE_INSERT:         // create a new table
                    if (edit) TerCreateTable(hWnd,-1,-1,TRUE);break;
                case ID_TABLE_INSERT_ROW:     // insert a row in the table
                    if (edit) TerInsertTableRow(hWnd,TRUE,TRUE);break;
                case ID_TABLE_SPLIT_CELL:     // split the current cell
                    if (edit) TerSplitCell(w);break;
                case ID_TABLE_MERGE_CELLS:    // merge cells
                    if (edit) TerMergeCells(w);break;
                case ID_TABLE_DEL_CELLS:      // delete cells
                    if (edit) TerDeleteCells(hWnd,0,TRUE);break;
                case ID_TABLE_ROW_POS:        // position row
                    if (edit) TerRowPosition(hWnd,0,TRUE,TRUE);break;
                case ID_TABLE_HDR_ROW:        // toggle header row
                    if (edit) TerSetHdrRow(hWnd,0,!TerMenuSelect(hWnd,ID_TABLE_HDR_ROW),TRUE);break;
                case ID_TABLE_ROW_KEEP:        // toggle header row
                    if (edit) TerSetRowKeep(hWnd,0,!TerMenuSelect(hWnd,ID_TABLE_ROW_KEEP),TRUE);break;
                case ID_TABLE_ROW_HEIGHT:        // position row
                    if (edit) TerRowHeight(hWnd,-1,TRUE,TRUE);break;
                case ID_TABLE_INSERT_COL:        // position row
                    if (edit) TerInsertTableCol(hWnd,TRUE,TRUE,TRUE);break;
                case ID_TABLE_CELL_BORDER:    // cell border
                    if (edit) TerCellBorder(hWnd,0,0,0,0,0,TRUE);break;
                case ID_TABLE_CELL_WIDTH:     // cell width
                    if (edit) TerCellWidth(hWnd,0,0,0,TRUE);break;
                case ID_TABLE_CELL_BORDER_COLOR:    // cell border color
                    if (edit) TerCellBorderColor(hWnd,0,0,0,0,0,TRUE);break;
                case ID_TABLE_CELL_SHADE:     // cell shading
                    if (edit) TerCellShading(hWnd,0,0,TRUE);break;
                case ID_TABLE_CELL_COLOR:     // cell background color
                    if (edit) TerCellColor(hWnd,0,0,TRUE);break;
                case ID_TABLE_CELL_VALIGN:    // cell vertical alignment
                    if (edit) TerCellVertAlign(hWnd,0,0,TRUE);break;
                case ID_TABLE_CELL_VTEXT:    // cell vertical alignment
                    if (edit) TerCellRotateText(hWnd,0,0,TRUE);break;
                case ID_TABLE_SEL_COL:        // cell column selection
                    if (edit) TerSelectCol(hWnd,TRUE);break;   // select the current column
                case ID_TABLE_SHOW_GRID:      // toggle show table grid lines
                    TerToggleTableGrid(w);break;
                case ID_TABLE_ROW_RTL:        // row text floe
                    if (edit) TerSetRowTextFlow(hWnd,TRUE,TRUE,0,TRUE);break;

                //*********************************************************
                // Tab functions
                //********************************************************
                case ID_TAB_SET:             // set a tab position
                    if (edit) SetTab(hWnd,0,-1,TRUE);break;
                case ID_TAB_CLEAR:           // clear a tab position
                    if (edit) ClearTabDlg(w);break;
                case ID_TAB_CLEAR_ALL:       // clear all tabs
                    if (edit) ClearAllTabs(hWnd,TRUE);break;

                //*********************************************************
                // View functions
                //*********************************************************
                case  ID_PAGE_MODE:          // toggle page mode
                    TogglePageMode(w);break;
                case  ID_FITTED_VIEW:        // toggle fitted view
                    ToggleFittedView(w);break;
                case  ID_VIEW_HDR_FTR:          // toggle page mode
                    ToggleViewHdrFtr(w);break;
                case  ID_TOOL_BAR:           // toggle tool bar
                    ToggleToolBar(w);break;
                case  ID_RULER:              // toggle ruler
                    ToggleRuler(w);break;
                case  ID_STATUS_RIBBON:      // toggle status ribbon
                    ToggleStatusRibbon(w);break;
                case  ID_SHOW_FIELD_NAMES:    // toggle field name/data display
                    ToggleFieldNames(w);break;
                case  ID_SHOW_HIDDEN:        // toggle hidden text display
                    ToggleHiddenText(w);break;
                case  ID_EDIT_FNOTE:         // toggle footnote text display
                    ToggleFootnoteEdit(w,TRUE);break;
                case  ID_EDIT_ENOTE:         // toggle endnote text display
                    ToggleFootnoteEdit(w,FALSE);break;
                case  ID_SHOW_PARA_MARK:     // toggle display of param marker
                    ToggleParaMark(w);break;
                case  ID_SHOW_PAGE_BORDER:   // toggle display of page borders
                    TogglePageBorder(w);break;
                case  ID_SHOW_HYPERLINK_CURSOR: // hyperlink cursor
                    ShowHyperlinkCursor=!ShowHyperlinkCursor;
                    break;

                //*********************************************************
                // Frame and drawing object edit functions
                //*********************************************************
                case  ID_FRAME_YBASE:          // set the y base for a frame/drawing object
                    if (edit) TerSetFrameYBase(hWnd,-1,0);break;
                case  ID_FRAME_ROTATE_TEXT:     // set the frame text direction
                    if (edit) TerRotateFrameText(hWnd,TRUE,CurLine,0,TRUE);break;
                case  ID_EDIT_DOB:             // set attributes a drawing object
                    if (edit) TerSetObjectAttrib(hWnd,-1,0,0,0,TRUE,0);break;

                //*********************************************************
                // Miscellaneous functions
                //*********************************************************
                case  ID_TER_HELP:           // display help on functions
                    WinHelp(hWnd,TerHelpFile,HELP_INDEX,0L);
                    break;
                case  ID_UNDO:               // undo previous operation
                    if (edit) TerUndo(w,TRUE);break;
                case  ID_REDO:               // redo previous operation
                    if (edit) TerUndo(w,FALSE);break;
                case  ID_INSERT:             // toggle insert/overtype status
                    if (edit) TerInsert(w);break;
                case  ID_JUMP:               // jump to a line number
                    TerJump(w);break;
                case  ID_BKND_PICT:          // background picture
                    if (BkPictId==0) TerSetBkPictId(hWnd,-1,BKPICT_STRETCH,TRUE);  // set background picture
                    else             TerSetBkPictId(hWnd,0,0,TRUE);                // turnoff background picture
                    break;
                case  ID_WATERMARK:         // set watermark
                    if (WmParaFID<=0)TerSetWatermarkPict(hWnd,-1,true,true);  // set watermark
                    else             TerSetWatermarkPict(hWnd,0,true,true);   // remove watermark
                    break;
                case  ID_ZOOM:               // set zoom percentage
                    TerSetZoom(hWnd,-1);break;
                case  ID_REPAGINATE:         // forced repagination
                    Repaginate(w,FALSE,FALSE,0,TRUE);// repaginate without yielding
                    PaintTer(w);
                    break;

                case  ID_INLINE_IME:         // toggle inline ime
                    if (edit) {
                       if (ImeEnabled) DisableIme(w,TRUE);
                       InlineIme=!InlineIme;
                    }
                    break;
                
                case  ID_DOC_RTL:            // set text flow
                    if (edit) TerSetDocTextFlow(hTerWnd,TRUE,0,TRUE);
                    break;

                case  ID_SECT_OPTIONS:       // accepts section parameters
                    TerSetSect(hWnd,0,0,TRUE);break;
                case  ID_OVERRIDE_BIN:       // accept bin override during printing
                    TerSetOverrideBin(hWnd,TRUE,0,0);break;
                case  ID_PROTECTION_LOCK:    // toggle protection lock
                    if (edit) ProtectionLock=!ProtectionLock;break;
                case  ID_TRACK_CHANGES:      // toggle track-changes
                    if (edit) TerEnableTracking(hTerWnd,!TrackChanges,NULL,TRUE,0,CLR_AUTO,0,CLR_AUTO);break;
                case  ID_NEXT_CHANGE:        // locate next changed text
                    TerFindNextChange(hTerWnd,true,true);break;
                case  ID_PREV_CHANGE:        // locate previous changed text
                    TerFindNextChange(hTerWnd,false,true);break;
                case  ID_ACCEPT_CHANGE:      // accept one change
                    if (edit) TerAcceptChanges(hTerWnd,false,false,true);break;
                case  ID_ACCEPT_ALL_CHANGES: // accept all changes
                    if (edit) TerAcceptChanges(hTerWnd,true,true,true);break;
                case  ID_SNAP_TO_GRID:    // toggle protection lock
                    if (edit) SnapToGrid=!SnapToGrid;break;
                case  ID_BORDER_MARGIN:    // toggle border margin
                    TerArg.BorderMargins=!TerArg.BorderMargins;
                    TerRepaint(hWnd,TRUE);
                    break;
                case  ID_PROTECT_FORM:      // form protection mode
                    ProtectForm=!ProtectForm;
                    TerSetReadOnly(hTerWnd,ProtectForm);
                    break;
                case  ID_EDIT_HDR_FTR:       // toggle edit page header/footer
                    ToggleEditHdrFtr(w);break;
                case  ID_CREATE_FIRST_HDR:   // create first page header
                    TerCreateFirstHdrFtr(hTerWnd,TRUE);break;
                case  ID_CREATE_FIRST_FTR:   // create first page footer
                    TerCreateFirstHdrFtr(hTerWnd,FALSE);break;
                case  ID_DELETE_FIRST_HDR:   // create first page header
                    TerDeleteFirstHdrFtr(hTerWnd,TRUE,TRUE);break;
                case  ID_DELETE_FIRST_FTR:   // create first page footer
                    TerDeleteFirstHdrFtr(hTerWnd,FALSE,TRUE);break;

                case  ID_EDIT_INPUT_FIELD:   // edit input field name border
                    EditInputField(w);break;

                case  ID_SPELL:             // spell checker hook
                    TerSpellCheck(hWnd,FALSE,TRUE);break;
                
                case  ID_AUTO_SPELL:        // spell checker hook
                    if (hSpell) {
                       if (TerFlags4&TFLAG4_AUTO_SPELL) ResetLongFlag(TerFlags4,TFLAG4_AUTO_SPELL);
                       else                             TerFlags4|=TFLAG4_AUTO_SPELL;
                       PaintTer(w);
                    }
                    break;

                default:
                    return (TerDefWindowProc(hWnd, message, wParam, lParam));
            }
            PrevCmdId=CmdId;              // previous command id
            CmdModified=TerArg.modified;  // keep track of modified count for dragon processing
            break;

            //***************************************************************
            // Mouse functions
            //***************************************************************
        case WM_LBUTTONDOWN:                   // position cursor at the current mouse position
            if (WheelShowing) ResetWheel(w);
            if (TerOpFlags&TOFLAG_SET_FOCUS) break;
            if (!TerLButtonDown(w,edit,message,wParam,lParam)) return TRUE;  // FALSE return if this window destroyed by the parent on a hyperlink message
            CurInputField=0;                  // input field lost focus
            if (CurDragObj>=0) return TRUE;   // no painting until drag object is processed
            break;

        case WM_RBUTTONDOWN:                 // process the right click on the ruler
            SpellCheckerPopped=FALSE;        // will the set when the spell-checker popmenu is displayed
            if (WheelShowing) ResetWheel(w);
            if (edit) {
                TerMousePos(w,lParam,TRUE);          // get mouse position in MouseCol and MouseLine variables
                if (RulerClicked && CurDragObj<0) DoRulerClick(w,message,lParam);
                CurDragObj=-1;
            }
            break;

        case WM_MBUTTONDOWN:                 // mouse wheel pressed
            if (WheelShowing) ResetWheel(w);
            else if (!InPrinting && GetSystemMetrics(SMTER_MOUSEWHEELPRESENT)) ActivateWheel(w,lParam);
            break;


        case WM_MOUSEMOVE:                   // set highlighted areas
            if (WheelShowing) {
               SetWheelTimer(w,lParam,FALSE);
               if (hWnd==GetFocus()) TerSetCursorShape(w,lParam,FALSE);
               return FALSE;
            } 
            if (IgnoreMouseMove || DraggingText || (PictureClicked && CurDragObj<0)) {
               if (hWnd==GetFocus() || ShowHyperlinkCursor)
                   TerSetCursorShape(w,lParam,FALSE);
               if (DraggingText) {
                  SetCapture(hTerWnd);   // capture the mouse movements
                  SetDragCaret(w,lParam);
                  if (ScrollText(w)) break;  // text scrolled
               }
               if (IgnoreMouseMove && !DraggingText && CurDragObj<0) SetMouseStopTimer(w,lParam);  
               return FALSE;
            }
            if (wParam&(MK_LBUTTON|MK_RBUTTON|MK_MBUTTON)) {
               if (CurDragObj>=0) TerDragObject(w,lParam);
               else TerSetHilight(w,wParam,lParam,FALSE);
            }
            break;


        case WM_LBUTTONUP:                   // stop stretching highlighted area now
            if (TerOpFlags&TOFLAG_SET_FOCUS) {
               ResetLongFlag(TerOpFlags,TOFLAG_SET_FOCUS);
               break;
            }
            StretchHilight=TblSelCursShowing=DblClickHilight=FALSE; // stop stretching highlighted area now
            KillHilightTimer(w);             // kill any hilight timer
            if (!StretchHilight && HilightBegRow==HilightEndRow && HilightBegCol==HilightEndCol && !DraggingText) {
                  HilightType=HILIGHT_OFF;
                  if (MoveCursor(w,CurLine,CurCol)) PaintTer(w);   // moved the cursor off the hidden area
            }
            if (HilightType==HILIGHT_CHAR && !DraggingText) AdjustHilight(w);  // adjust the highlighting area
            IgnoreMouseMove=TRUE;            // ignore mouse move until mouse pressed again
            ReleaseCapture();                // release the mouse
            if (CurDragObj>=0) {
                PaintTer(w);
                CurDragObj=-1;
            }
            else if (DraggingText && !InOleDrag) DragText(w,lParam);
            else if (VerySmallMovement && HilightType!=HILIGHT_OFF && HilightBegRow==HilightEndRow && HilightBegCol==HilightEndCol) {
                HilightType=HILIGHT_OFF;
                PaintTer(w);
            }
            else if (HilightType!=HILIGHT_OFF && !PictureHilighted) {
                InputFontId=-1;               // disable any font selection since the mouse moved
                PaintTer(w);
            }

            TerSetCursorShape(w,lParam,FALSE);    // show regular cursor

            if (TerOpFlags&TOFLAG_NO_RESET_FONT) ResetLongFlag(TerOpFlags,TOFLAG_NO_RESET_FONT);
            else                                 InputFontId=-1;  // reset input font id
            break;

        case WM_RBUTTONUP:                   // stop stretching highlighted area now
            if (DoAutoSpellCheck(w)) {
               SpellCheckCurWordPart1(w,lParam);  // spell check on word
               break;
            }
            else return (TerDefWindowProc(hWnd, message, wParam, lParam));

        case WM_LBUTTONDBLCLK:               // erase highlighting
            if (MouseOverShoot==' ' && MouseOnTextLine) TerDoubleClick(w);    // highlight the current word
            break;

        case WM_NCLBUTTONDOWN:               // repaint the client area
        case WM_NCRBUTTONDOWN:
        case WM_NCMBUTTONDOWN:
            if (TerArg.style&WS_CHILD && hWnd!=GetFocus()) SetFocus(hWnd);
            return (TerDefWindowProc(hWnd, message, wParam, lParam));

        case WMTER_MOUSEWHEEL:
            if (CaretEngaged) DisengageCaret(w); // disengage the caret
            if (WheelShowing) ResetWheel(w);
            if (PictureClicked) {            // disable any picture hilighting
               HilightType=HILIGHT_OFF;
               PictureClicked=FALSE;
            }
            if ((short)HIWORD(wParam)>0) TerWinUp(w);
            else                         TerWinDown(w);
            break;

        case TER_CREATED:          // control created 
           SendMessageToParent(w,TER_CREATED,(WPARAM)hWnd,0L,FALSE);
           return (LPARAM)NULL;
         
        case WM_SETCURSOR:                   // allow automatic setting of the cursor?
            if (LOWORD(lParam)==HTCLIENT && (hWnd==GetFocus() || ShowHyperlinkCursor) && !InPrintPreview) return TRUE;   // keep the cursor as it is
            else return (TerDefWindowProc(hWnd, message, wParam, lParam));

        case WM_DROPFILES:                   // insert a dropped object
            TerInsertDragObject(w,(HDROP)(wParam),false);
            break;

        case WM_SETFOCUS:
            if (FontsReleased) RecreateFonts(w,hTerDC);  // recreate fonts
            InitCaret(w);                                // recreate caret
            if (TerArg.style&WS_CHILD) {
               //BringWindowToTop(GetParent(hWnd));
               if (!(TerFlags&TFLAG_NO_CHILD_TOP)) BringWindowToTop(hWnd);  // Comment out this line to get correct dialog tab order
               if (TerFlags&TFLAG_ACTIVATE_MDI_CHILD && hMdiClient && hMdiChild) SendMessage(hMdiClient,WM_MDIACTIVATE,(WPARAM)hMdiChild,0L);
            }
            
            if (TRUE || GetTopParent(w,hTerWnd)!=GetTopParent(w,(HWND)wParam))  TerOpFlags|=TOFLAG_NO_RESET_FONT;      // don't reset effecitive font on mouse up
            IgnoreMouseMove=TRUE;                  // reset any highligting started by preceeding WM_LBUTTONDOWN
            if (PictureClicked && !PictureHilighted) {
               DrawDragPictRect(w);    // erase to protect from clipping region
            }
            if (FrameClicked && !FrameTabsHilighted) {
               DrawDragFrameTabs(w);
            }
            return (TerDefWindowProc(hWnd, message, wParam, lParam));

        case WM_KILLFOCUS:
            if (CaretEngaged) DisengageCaret(w); // disengage the caret
            if (WheelShowing) ResetWheel(w);
            TerDestroyCaret(w);
            if (PictureHilighted) {
               DrawDragPictRect(w);    // erase to protect from clipping region
               //PictureClicked=FALSE;
               //HilightType=HILIGHT_OFF;
            }
            if (FrameTabsHilighted) {
               DrawDragFrameTabs(w);
               //FrameClicked=FALSE;
            }
            if (True(TerFlags6&TFLAG6_SUPPORT_DRAGON_SPEECH) && PrevCmdId==ID_UNDO && TerArg.modified==CmdModified) {  // if undo was the last modification that happened to the document
                SetTimer(hTerWnd,TIMER_DRAGON_FOCUS,1000,NULL);  // timer to set the focus back
            }
               
            //DeleteDragObjects(w,DRAG_TYPE_SCR_FIRST,DRAG_TYPE_SCR_LAST);
            return (TerDefWindowProc(hWnd, message, wParam, lParam));

        case WM_PAINT:
            RepaintTer(w);
            if (!CaretEnabled && UseCaret(w)) InitCaret(w);
            break;

        case TER_REPAINT:                    // this message is posted to do a repaint
            if (!(TerOpFlags&TOFLAG_REPAINT_MSG)) {
               TerOpFlags|=TOFLAG_REPAINT_MSG;
               TerRepaint(hWnd,TRUE);
               ResetLongFlag(TerOpFlags,TOFLAG_REPAINT_MSG);
            }
            break;

        case WM_INPUTLANGCHANGEREQUEST:
            SetCurLang(w,(HKL)lParam);
            return (TerDefWindowProc(hWnd, message, wParam, lParam));

        case WM_ENTERIDLE:                   // check if context sensitive help is requested
            if ((wParam == MSGF_MENU) && (GetKeyState(VK_F1) & 0x8000)) {
                TerHelpWanted=TRUE;
                PostMessage(hWnd,WM_KEYDOWN,VK_RETURN,0L); // generate a dummy <CR> key, cause selection of the menu item
            }
            break;

        case WM_CLOSE:
            if (TerArg.UserCanClose) CloseTer(hTerWnd,FALSE);
            return (LRESULT)NULL;

        case WM_WININICHANGE:
        case WM_DEVMODECHANGE:
            TerSetPrinter(hWnd,NULL,NULL,NULL,NULL);  // update printer selection
            return (LRESULT)TRUE;

        case WM_QUERYENDSESSION:
            if (!TerQueryExit(hTerWnd)) return FALSE;
            return (CloseTer(hTerWnd,FALSE));

        case WM_DESTROY:                 // control comes here only if WM_CLOSE did not preceed WM_DESTROY
            if (!WindowDestroyed) {
               WindowDestroyed=TRUE;     // window already destroyed
               CloseTer(hTerWnd,TRUE);   // forced closed
            }
            return (LRESULT)NULL;

        #if defined (WIN32)              // send color messages to the parent
        case WM_CTLCOLORBTN: 
        case WM_CTLCOLORDLG:   
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            if (NULL!=(hBr=SetControlColor(w,(HDC)wParam,(HWND)lParam))) return (LRESULT) (UINT)hBr;
            else return (LRESULT) SendMessageToParent(w,message,wParam,lParam,TRUE);
        #else
        case WM_CTLCOLOR:                // let parent of TER handle the child colors
            if (NULL!=(hBr=SetControlColor(w,(HDC)wParam,(HWND)LOWORD(lParam)))) return (LRESULT) (UINT)hBr;
            else return (LRESULT) SendMessageToParent(w,message,wParam,lParam,TRUE);
        #endif

        case WM_GETDLGCODE:
            {
               LPMSG pMsg=(LPMSG)lParam;
               UINT want=(DLGC_WANTTAB|DLGC_WANTARROWS|DLGC_WANTCHARS);  // process all messages when TER used as a control
               BOOL WantAllKeys=TRUE;
               if (pMsg && (pMsg->message==WM_CHAR || pMsg->message==WM_KEYDOWN || pMsg->message==WM_KEYUP) && pMsg->wParam==VK_ESCAPE) WantAllKeys=FALSE;
               return (long)(WantAllKeys?(want|DLGC_WANTALLKEYS):want);
            }

        case WM_ERASEBKGND:
            if (!MovingControl) RedrawBorder=TRUE;
            return (LRESULT)NULL;

        case WM_TIMER:                 // resume repagination
            if (wParam==TIMER_REPAGE && !(TerFlags2&TFLAG2_IGNORE_TIMER)) {
               while (PeekMessage(&msg,hTerWnd,WM_TIMER,WM_TIMER,PM_REMOVE|PM_NOYIELD));  // remove any additional timer messages
               if (!PeekMessage(&msg,hTerWnd,0,0,PM_NOREMOVE|PM_NOYIELD)
                   && !(TerFlags2&TFLAG2_NO_AUTO_REPAGE))  // do pagination only when nothing else to do
                   Repaginate(w,TRUE,FALSE,0,TRUE);        // repaginate with yield
               while (PeekMessage(&msg,hTerWnd,WM_TIMER,WM_TIMER,PM_REMOVE));  // remove any timer messages
            }
            else if (wParam==TIMER_HILIGHT) TerSetHilight(w,0,0L,TRUE);
            else if (wParam==TIMER_EVAL) DrawEval(w);
            else if (wParam==TIMER_WHEEL_UP || wParam==TIMER_WHEEL_DOWN) {
               if (WheelShowing) {
                  if (wParam==TIMER_WHEEL_UP) TerWinUp(w);
                  else                        TerWinDown(w);
               } 
            } 
            else if (wParam==TIMER_MOUSE_STOP) MouseStopAction(w);   // do mouse stop activities
            else if (wParam>=TIMER_ANIM && !(TerFlags2&TFLAG2_IGNORE_TIMER)) DrawAnimPict(w,wParam);
            else if (wParam==TIMER_DRAGON_FOCUS) {
               KillTimer(hTerWnd,TIMER_DRAGON_FOCUS);
               SetFocus(hTerWnd);
            } 
            return (LRESULT) NULL;

        case TER_IDLE:                // do idle time activity
            if (TerOpFlags&TOFLAG_PAINT_WAITING) return (LRESULT)NULL;  // do idle processing in the following paint message
            OlePostProcessing(w);
            break;

        case WM_PRINT:
            if (InPrintPreview || (HDC)wParam==hTerDC) return (LRESULT)NULL;
            else return (TerDefWindowProc(hWnd, message, wParam, lParam));

        default:
            return (TerDefWindowProc(hWnd, message, wParam, lParam));
    }

    if (!TerArg.open) return (LRESULT)NULL;

    TerPostProcessing(w,message,wParam,lParam);    // set cursor position, scroll bars, rulers, repagination etc

    SendActionMessage(w,message,wParam,lParam);            // send TER_ACTION message

    return (LRESULT)NULL;
}

/****************************************************************************
    TerRedirectArrowKeys:
    Redirect the Arrow keys to our TerWndProc process.
****************************************************************************/
BOOL WINAPI _export TerRedirectArrowKeys(HWND hWnd, BOOL redirect)
{
    LRESULT CALLBACK HookGetMsgProc(int,WPARAM,LPARAM);
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (redirect) {
       if (hMsgHook==null) hMsgHook=SetWindowsHookEx(WH_GETMESSAGE,(HOOKPROC)HookGetMsgProc,hTerInst,0);
       return (hMsgHook!=null);
    }
    else {
       if (hMsgHook) UnhookWindowsHookEx(hMsgHook);
       hMsgHook=null;
       return true;
    }
}


/****************************************************************************
    RedirectArrowKeys:
    Redirect the Arrow keys to our TerWndProc process.
****************************************************************************/
LRESULT CALLBACK HookGetMsgProc(int code,WPARAM wParam,LPARAM lParam)
{
    LPMSG pMsg=(LPMSG)lParam;
    PTERWND w;
    static DWORD PrevTime=0;

    if (NULL==(w=GetWindowPointer(pMsg->hwnd))) return (LRESULT) null;  // get the pointer to window data

    if (code>=0 && wParam==PM_REMOVE && GetFocus()==hTerWnd  && PrevTime!=pMsg->time) {  // code>=0 Do processing,  wParam==PM_REMOVE In GetMessage or in PeekMessage with message removal
       if (pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_LEFT || pMsg->wParam==VK_RIGHT || pMsg->wParam==VK_UP || pMsg->wParam==VK_DOWN)) {
           if (!(pMsg->lParam&0x10000000)) {          // not in the menu invoked mode
              if (hMsgHook!=null) {
                 bool translate=true;
                 if      (GetKeyState(VK_CONTROL)&0x80) translate=false;
                 else if (GetKeyState(VK_SHIFT)&0x80) translate=false;
                 else if (GetKeyState(VK_MENU)&0x80) translate=false;
                 
                 if (translate) {
                    int cmd=0;
                    if (pMsg->wParam==VK_UP)        cmd=ID_UP;
                    else if (pMsg->wParam==VK_DOWN) cmd=ID_DOWN;
                    else if (pMsg->wParam==VK_LEFT) cmd=ID_LEFT;
                    else if (pMsg->wParam==VK_RIGHT)cmd=ID_RIGHT;
                    //LogPrintf("cmd: %d, code: %x, mlp: %x, tm: %x",cmd,code,pMsg->lParam,pMsg->time);
                    if (cmd!=0) {
                       TerCommand2(hTerWnd,cmd,false);
                       pMsg->message=TER_IDLE;
                    }

                    PrevTime=pMsg->time;
                 }
              }
           }
       }
    }    

    return CallNextHookEx(hMsgHook,code,wParam,lParam);
}
 
/******************************************************************************
   TerPostProcessing:
   This routine is called after processing each message.  This routine
   sets cursor position, scroll bars, ruler, repagination etc.
******************************************************************************/
BOOL TerPostProcessing(PTERWND w,UINT message,WPARAM wParam,LPARAM lParam)
{
    dm("TerPostProcessing");

    if (InPrintPreview || !PaintEnabled) return TRUE;        // no processing when in print preview
    if (TotalLines==0) return TRUE;                          // nothing in the window
    if (TotalLines==1 && LineLen(0)==0) return TRUE;         // nothing in the window
    
    if (  TerFlags4&TFLAG4_UNDO_WINDOW_OVERFLOW 
       && (MessageId==WM_COMMAND || MessageId==WM_CHAR)) CheckWindowOverflow(w);  // check if the previous edit needs to be undone


    OlePostProcessing(w);

    // reset horizontal cursor position for up/down keys
    if (message!=WM_COMMAND || (
           GET_WM_COMMAND_ID(wParam,lParam)!=ID_UP
        && GET_WM_COMMAND_ID(wParam,lParam)!=ID_DOWN
        && GET_WM_COMMAND_ID(wParam,lParam)!=ID_PGUP
        && GET_WM_COMMAND_ID(wParam,lParam)!=ID_PGDN) ) CursHorzPos=-1;

    //********** set caret position *****************
    if (!CaretPositioned) OurSetCaretPos(w);

    //********** send modificatio message ************************
    if (!Notified && TerArg.modified && !TerArg.ReadOnly) {
       Notified=TRUE;
       if      (MsgCallback) MsgCallback(hTerWnd,TER_MODIFIED,(WPARAM)hTerWnd,0L);
       else if (VbxCallback) VbxCallback(hTerWnd,TER_MODIFIED,(WPARAM)hTerWnd,0L);
       else if (TerArg.hParentWnd) SendMessage(TerArg.hParentWnd,TER_MODIFIED,(WPARAM)hTerWnd,0L);  //Send in TER format
       #if defined(WIN32)
         if (TerArg.hParentWnd) SendMessage(TerArg.hParentWnd,WM_COMMAND,MAKELONG(GetDlgCtrlID(hTerWnd),EN_CHANGE),(LPARAM)hTerWnd);        //Send in EN_ format
       #else
         if (TerArg.hParentWnd) SendMessage(TerArg.hParentWnd,WM_COMMAND,GetDlgCtrlID(hTerWnd),MAKELONG((WORD)hTerWnd,EN_CHANGE));  //Send in EN_ format
       #endif
    }


    // check if page header/footer need to be moved
    if (PosPageHdrFtr)  ReposPageHdrFtr(w,TRUE);  // reposition page header/footer

    if (CurPfmt!=pfmt(CurLine) || RulerSection!=GetSection(w,CurLine)) DrawRuler(w,TRUE);     // redraw the ruler

    // update tool bar
    if (!CharMessagePending(w) || CommandId!=0) UpdateToolBar(w,FALSE);  // excluding for WM_CHAR increases the performance during typing enourmously

    // repaginate if necessary
    if (TerArg.PrintView && (RepageBeginLine<TotalLines || PageModifyCount!=TerArg.modified) ) {
      if (TotalLines>=SMALL_FILE && !RepageTimerOn  && !(TerFlags2&TFLAG2_IGNORE_TIMER)) 
          RepageTimerOn=SetTimer(hTerWnd,TIMER_REPAGE,5000,NULL);

      if (TotalLines<SMALL_FILE || !RepageTimerOn) {
         if (!(TerFlags2&TFLAG2_NO_AUTO_REPAGE) && Repaginate(w,TRUE,FALSE,0,TRUE)) {
            TerPostProcessing(w,message,wParam,lParam);   // repagination happened, process again
            return TRUE;
         }
       }
    }

    // Recreate section breaks
    if (SectModified && !MessagePending(w) && RecreateSections(w)) {
       PaintTer(w);           // redisplay the screen
       TerPostProcessing(w,message,wParam,lParam);  // sections modified, process again
       return TRUE;
    }

    dm("TerPostProcessing - End");

    return TRUE;
}

/******************************************************************************
   SendActionMessage:
   Send certain action messages.
******************************************************************************/
SendActionMessage(PTERWND w,UINT message, WPARAM wParam,LPARAM lParam)
{
   if      (message==WM_COMMAND) SendMessageToParent(w,TER_ACTION,ACTION_COMMAND,GET_WM_COMMAND_ID(wParam,lParam),FALSE);
   else if (message==WM_VSCROLL) SendMessageToParent(w,TER_ACTION,ACTION_VSCROLL,SCROLL_MSG(wParam,lParam),FALSE);
   else if (message==WM_HSCROLL) SendMessageToParent(w,TER_ACTION,ACTION_HSCROLL,SCROLL_MSG(wParam,lParam),FALSE);
   else if (message==WM_CHAR)    SendMessageToParent(w,TER_ACTION,ACTION_CHAR,wParam,FALSE);
   else if (message==WM_LBUTTONDOWN) SendMessageToParent(w,TER_ACTION,ACTION_LBUTTONDOWN,lParam,FALSE);
   else if (message==WM_RBUTTONDOWN) SendMessageToParent(w,TER_ACTION,ACTION_RBUTTONDOWN,lParam,FALSE);
   else if (message==WM_LBUTTONUP) SendMessageToParent(w,TER_ACTION,ACTION_LBUTTONUP,lParam,FALSE);
   else if (message==WM_RBUTTONUP) SendMessageToParent(w,TER_ACTION,ACTION_RBUTTONUP,lParam,FALSE);
   else if (message==WM_LBUTTONDBLCLK) SendMessageToParent(w,TER_ACTION,ACTION_LBUTTONDBLCLK,lParam,FALSE);
   else if (message==WM_RBUTTONDBLCLK) SendMessageToParent(w,TER_ACTION,ACTION_RBUTTONDBLCLK,lParam,FALSE);
   else if (message==WM_SIZE)      SendMessageToParent(w,TER_ACTION,ACTION_SIZE,lParam,FALSE);
   else if (message==WM_SETFOCUS)  SendMessageToParent(w,TER_ACTION,ACTION_SETFOCUS,wParam,FALSE);
   else if (message==WM_KILLFOCUS)  SendMessageToParent(w,TER_ACTION,ACTION_KILLFOCUS,wParam,FALSE);
   else if (message==WM_MOUSEMOVE)  SendMessageToParent(w,TER_ACTION,ACTION_MOUSEMOVE,lParam,FALSE);

   return TRUE;
}

/******************************************************************************
   SendPreprocessMessage:
   Send certain action messages.
******************************************************************************/
SendPreprocessMessage(PTERWND w,UINT message, WPARAM wParam,LPARAM lParam)
{
   if (InPreprocess) return false;    // do not send nested preprocess events because the SkipCommand can be altereed 

   InPreprocess=true;
   SkipCommand=FALSE;

   if      (message==WM_COMMAND) SendMessageToParent(w,TER_PREPROCESS,ACTION_COMMAND,GET_WM_COMMAND_ID(wParam,lParam),FALSE);
   else if (message==WM_VSCROLL) SendMessageToParent(w,TER_PREPROCESS,ACTION_VSCROLL,SCROLL_MSG(wParam,lParam),FALSE);
   else if (message==WM_HSCROLL) SendMessageToParent(w,TER_PREPROCESS,ACTION_HSCROLL,SCROLL_MSG(wParam,lParam),FALSE);
   else if (message==WM_CHAR)    SendMessageToParent(w,TER_PREPROCESS,ACTION_CHAR,wParam,FALSE);
   else if (message==WM_LBUTTONDOWN) SendMessageToParent(w,TER_PREPROCESS,ACTION_LBUTTONDOWN,lParam,FALSE);
   else if (message==WM_RBUTTONDOWN) SendMessageToParent(w,TER_PREPROCESS,ACTION_RBUTTONDOWN,lParam,FALSE);
   else if (message==WM_LBUTTONUP) SendMessageToParent(w,TER_PREPROCESS,ACTION_LBUTTONUP,lParam,FALSE);
   else if (message==WM_RBUTTONUP) SendMessageToParent(w,TER_PREPROCESS,ACTION_RBUTTONUP,lParam,FALSE);
   else if (message==WM_LBUTTONDBLCLK) SendMessageToParent(w,TER_PREPROCESS,ACTION_LBUTTONDBLCLK,lParam,FALSE);
   else if (message==WM_RBUTTONDBLCLK) SendMessageToParent(w,TER_PREPROCESS,ACTION_RBUTTONDBLCLK,lParam,FALSE);
   else if (message==WM_SIZE)      SendMessageToParent(w,TER_PREPROCESS,ACTION_SIZE,lParam,FALSE);
   else if (message==WM_SETFOCUS)  SendMessageToParent(w,TER_PREPROCESS,ACTION_SETFOCUS,wParam,FALSE);
   else if (message==WM_KILLFOCUS)  SendMessageToParent(w,TER_PREPROCESS,ACTION_KILLFOCUS,wParam,FALSE);
   else if (message==WM_MOUSEMOVE)  SendMessageToParent(w,TER_PREPROCESS,ACTION_MOUSEMOVE,lParam,FALSE);
   else if (message==WM_QUERYENDSESSION) SendMessageToParent(w,TER_PREPROCESS,ACTION_QUERYENDSESSION,lParam,FALSE);

   InPreprocess=false;

   // check if command enabled for proprocess only
   if (message==WM_COMMAND) {
     int CmdId=GET_WM_COMMAND_ID(wParam,lParam);
     if (CmdId>=ID_FIRST_COMMAND && CmdId<(ID_FIRST_COMMAND+MAX_SPEED_KEYS) && SpeedKeyEnabled[CmdId-ID_FIRST_COMMAND]==2) 
         SkipCommand=true;   // this command always skipped after preprocess
   } 

   return SkipCommand;
}

/******************************************************************************
   XlateCommandId:
   translate command id when in a vertical frame
******************************************************************************/
int XlateCommandId(PTERWND w,int CmdId)
{
    int angle=LineTextAngle(w,CurLine);
    int NewId=CmdId;

    if (LineFlags2(CurLine)&LFLAG2_RTL) {
       if (TerFlags5&TFLAG5_RTL_CURSOR) {
          if (CmdId==ID_LEFT)          NewId=ID_RIGHT;
          if (CmdId==ID_RIGHT)         NewId=ID_LEFT;
       }
       if (CmdId==ID_LEFT_INDENT)   NewId=ID_RIGHT_INDENT;
       if (CmdId==ID_RIGHT_INDENT)  NewId=ID_LEFT_INDENT;
       if (CmdId==ID_LEFT_JUSTIFY)  NewId=ID_RIGHT_JUSTIFY;
       if (CmdId==ID_RIGHT_JUSTIFY) NewId=ID_LEFT_JUSTIFY;
    }
    CmdId=NewId;

    if (angle==0) return CmdId;

    if (CmdId==ID_UP)   NewId=(angle==90)?ID_RIGHT:ID_LEFT; 
    if (CmdId==ID_DOWN) NewId=(angle==90)?ID_LEFT:ID_RIGHT;
    if (CmdId==ID_LEFT) NewId=(angle==90)?ID_UP:ID_DOWN;    
    if (CmdId==ID_RIGHT)NewId=(angle==90)?ID_DOWN:ID_UP;    
 
    return NewId;
}

/******************************************************************************
   TerTranslateAccelerator:
   Translates the accelerator keys for the ter window
******************************************************************************/
BOOL TerTranslateAccelerator(PTERWND w,UINT message,WPARAM wParam,LPARAM lParam)
{
    MSG msg;
    BOOL translated;
    HWND hWnd;
    HANDLE hAccTable=(hUserAccTable)?hUserAccTable:hTerAccTable;

    if (TerFlags&TFLAG_DISABLE_ACCEL) return FALSE;   // accelarators disabled

    if ( message==WM_NCHITTEST || message==WM_MOUSEMOVE
      || message==WM_SETCURSOR) return FALSE;

    if (!hAccTable) return FALSE;          // translation table not defined yet

    hWnd=hTerWnd;

    if (InAccelerator) return FALSE;       // already processing an accelerator key

    // fill the message structure
    msg.hwnd=hWnd;
    msg.message=message;
    msg.wParam=wParam;
    msg.lParam=lParam;
    msg.time=0;
    msg.pt.x=msg.pt.y=0;

    InAccelerator=TRUE;
    CommandSkipped=FALSE;

    translated=TranslateAccelerator(hWnd,hAccTable,&msg);  // TRUE if accelerator tranlated
    if (IsWindow(hWnd)) {
       InAccelerator=FALSE;                   // window still valid?
       if (CommandSkipped) translated=FALSE;  // speedkey disabled
       if (translated) {
          // discard any messages generated by the TranslateMessage function in WinMain
          while (PeekMessage(&msg,hWnd,WM_CHAR,WM_CHAR,PM_REMOVE|PM_NOYIELD));
          //while (PeekMessage(&msg,hWnd,WM_KEYUP,WM_KEYUP,PM_REMOVE|PM_NOYIELD));
          while (PeekMessage(&msg,hWnd,WM_INITMENU,WM_INITMENUPOPUP,PM_REMOVE|PM_NOYIELD));
          while (PeekMessage(&msg,hWnd,WM_MENUSELECT,WM_MENUCHAR,PM_REMOVE|PM_NOYIELD));
          while (PeekMessage(&msg,hWnd,WM_SYSKEYDOWN,WM_SYSKEYDOWN,PM_REMOVE|PM_NOYIELD));
          while (PeekMessage(&msg,hWnd,WM_SYSKEYUP,WM_SYSKEYUP,PM_REMOVE|PM_NOYIELD));
          while (PeekMessage(&msg,hWnd,WM_SYSCOMMAND,WM_SYSCOMMAND,PM_REMOVE|PM_NOYIELD));
          while (PeekMessage(&msg,hWnd,WM_SYSCHAR,WM_SYSCHAR,PM_REMOVE|PM_NOYIELD));
       }
    }

    return translated;
}


/******************************************************************************
MAK:
   LibMain: Function to initialize DLL
******************************************************************************/

#if defined (WIN32) && !defined(_LIB)
		BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
		{
			return TerInit(hInstance,reason);
		}    
#else
		int CALLBACK LibMain(HINSTANCE hInstance, WORD wDataSeg, WORD cbHeapSize,  LPSTR lpszCmdLine)
		{
				return TerInit(hInstance,0L);
		}    
#endif

/******************************************************************************
   TerInit: Function to initialize DLL
******************************************************************************/
BOOL WINAPI _export TerInit(HINSTANCE hInstance,DWORD reason)
{
    WNDCLASS  wc;
    int i,j,len;
    BYTE ShortDateFmt,LongDateFmt,TimeDelim,DateDelim;
    DWORD version;
    CPINFO cp;

    // process the 'reason' flag for WIN32
    Win32=TRUE;
    if (reason == DLL_PROCESS_DETACH) return TerExit(0);
    if (reason != DLL_PROCESS_ATTACH) return TRUE;

    GetCPInfo(CP_ACP,&cp);
    InitMbcs=(cp.MaxCharSize>1);


    // initialize the message table
    for (i=0;i<MAX_MSGS;i++) {
       MsgString[i]=NULL;
       CustomMsg[i]=0;
    }
    TerLastMsg=0;              // initialize the last message
    TerLastDebugMsg[0]=0;
    #include "ter_msg.h"       // initialize the message table

    // Initialize the common global variables
    hTerInst=hInstance;
    hCurAbortWnd=NULL;         // current printing abort window
    hMemDC=NULL;               // shared by windows
    hTerAccTable=NULL;         // common accelerator table
    hPagePen=NULL;             // page pen
    hFocusPen=NULL;            // focus pen
    TerOpenCount=0;            // number of open TER windows
    TotalWinPtrs=0;            // number of non-standard open windows
    TotalWinTasks=0;           // total tasks using the dll
    hGlbFont=NULL;             // global font table pointer
    GlbPrtDC=NULL;             // global printer device context table pointer
    GlbPrtDCCount=0;           // number of device context cached
    StorageId=0;               // used to make unique OLE storage names
    InitPageWidth=0;           // initial page width
    InitPageHeight=0;          // initial page height
    InitFontFace[0]=0;         // initial typeface
    TerInitFlags=TFLAG_BUF_DISP|TFLAG_NO_AUTO_FULL_CELL_SEL; // reset initialization flags
    #if defined(NO_OLE)
      TerInitFlags|=TFLAG_NO_OLE;
    #endif
    TerInitFlags2=TFLAG2_XPARENT_WRITE;      // reset initialization flags
    TerInitFlags3=TFLAG3_EMBEDDED_TABLES|TFLAG3_STYLES_ON_TOOLBAR|TFLAG3_READ_PNG;    // reset initialization flags
    TerInitFlags4=(TFLAG4_NO_BUFFERED_SCROLL|TFLAG4_DONT_FIX_NEG_INDENT); // reset initialization flags
    TerInitFlags5=TFLAG5_INPUT_TO_UNICODE;     // initialization flags
    TerInitFlags6=(TFLAG6_USE_PEN_FOR_BORDER|TFLAG6_XLATE_UNICODE|TFLAG6_OLE_DROP_SOURCE
                   |TFLAG6_PASTE_UNICODE|TFLAG6_ALT_UNI_CHARWIDTH);   // initialization flags
    eval=expired=FALSE;
    DebugMode=FALSE;          // turn-off debug mode
    lstrcpy(PdfDllName,"wPDFControl01.dll");   // pdf support dll name
    HuffInitialized=false;                     // set to true after initializing the huff table

    MergeDelim='|';            // delimiter character for mail-merge name and data arrays

    DefPrinterName[0]=0;       // TER default printer name
    TerStyles=0;               // Overiding styles
    PrintCallback=NULL;        // TerMergePrint callback

    // build the table of basic colors
    i=-1;
    i++;PalColor[i]=0x0;           // black
    i++;PalColor[i]=0xFFFFFF;      // white
    i++;PalColor[i]=0xFF;          // red
    i++;PalColor[i]=0xFF00;        // green
    i++;PalColor[i]=0xFF0000;      // blue
    i++;PalColor[i]=0xFFFF;        // yelllow
    i++;PalColor[i]=0xFFFF00;      // cyan
    i++;PalColor[i]=0xFF00FF;      // magenta
    i++;PalColor[i]=0x80;          // dark red
    i++;PalColor[i]=0x8000;        // dark green
    i++;PalColor[i]=0x800000;      // dark blue
    i++;PalColor[i]=0x8080;        // dark yellow
    i++;PalColor[i]=0x800080;      // dark magenta
    i++;PalColor[i]=0x808000;      // dark cyan
    i++;PalColor[i]=0x808080;      // medium gray
    i++;PalColor[i]=0xc0c0c0;      // light gray
    i++;PalColor[i]=0xc0dcc0;      // money green
    i++;PalColor[i]=0xf0caa6;      // sky blue
    i++;PalColor[i]=0xf0fbff;      // cream
    i++;PalColor[i]=0xa4a0a0;      // light gray
    TotalBaseColors=TotalPalColors=i+1; // color in the color table

    // check if runing on WinNT or 2000
    version=GetVersion();
    IsWinNT2000=(version<0x80000000);
    IsWin95_98_ME=(version&0x80000000) && (version&0xF)==4;
    OSCanRotate=false;   // IsWinNT2000;  OS can rotate GDI drawing - we will do our own rotation to fix hilighting on vertical text

    // build the date field text used by rtf io
    ShortDateFmt='0';   // mmddyy
    LongDateFmt='0';    // mmddyy
    DateDelim='/';      // date separator
    TimeDelim=':';      // time delimiter character
    
    strcpy(LicenseKey,"8YP41-NM5Q2-34BL3");

    #if defined(WIN32)
    {
      BYTE string[20];

      if (GetLocaleInfoA(GetUserDefaultLCID(),LOCALE_IDATE,string,sizeof(string)))
         ShortDateFmt=string[0];
      if (GetLocaleInfoA(GetUserDefaultLCID(),LOCALE_ILDATE,string,sizeof(string)))
         LongDateFmt=string[0];
      if (GetLocaleInfoA(GetUserDefaultLCID(),LOCALE_SDATE,string,sizeof(string)))
         DateDelim=string[0];
      if (GetLocaleInfoA(GetUserDefaultLCID(),LOCALE_STIME,string,sizeof(string)))
         TimeDelim=string[0];
    }
    #endif
    
    
    i=-1;
    i++;lstrcpy(DateFieldText[i],(ShortDateFmt=='0'?"M/d/yy":"d/M/yy"));
    i++;lstrcpy(DateFieldText[i],(LongDateFmt=='0'?"dddd, MMMM dd, yyyy":"dddd, dd MMMM, yyyy"));
    i++;lstrcpy(DateFieldText[i],(LongDateFmt=='0'?"MMMM d, yyyy":"d MMMM, yyyy"));
    i++;lstrcpy(DateFieldText[i],(ShortDateFmt?"M/d/yyyy":"d/M/yyyy"));
    i++;lstrcpy(DateFieldText[i],"yyyy-MM-dd");
    i++;lstrcpy(DateFieldText[i],"d-MMM-yy");
    i++;lstrcpy(DateFieldText[i],(ShortDateFmt=='0'?"M.d.yy":"d.M.yy"));
    i++;lstrcpy(DateFieldText[i],"MMM. d, yy"); 
    i++;lstrcpy(DateFieldText[i],"d MMMM, yyyy");
    i++;lstrcpy(DateFieldText[i],"MMMM, yy");
    i++;lstrcpy(DateFieldText[i],"MMM-yy");
    i++;lstrcpy(DateFieldText[i],(ShortDateFmt=='0'?"M/d/yy h:mm am/pm":"d/M/yy h:mm am/pm"));
    i++;lstrcpy(DateFieldText[i],(ShortDateFmt=='0'?"M/d/yy h:mm:ss am/pm":"d/M/yy h:mm:ss am/pm"));
    i++;lstrcpy(DateFieldText[i],"h:mm am/pm");
    i++;lstrcpy(DateFieldText[i],"h:mm:ss am/pm");
    i++;lstrcpy(DateFieldText[i],"HH:mm");
    i++;lstrcpy(DateFieldText[i],"HH:mm:ss");
    TotalDateFmts=i+1;

    if (DateDelim!='/' || TimeDelim!=':') {   // customize date and time delimiters
       for (i=0;i<TotalDateFmts;i++) {
          LPBYTE pFmt=DateFieldText[i];
          len=lstrlen(pFmt);
          for (j=0;j<len;j++) {
             if (DateDelim!='/' && pFmt[j]=='/' && pFmt[j+1]!='p') pFmt[j]=DateDelim;
             if (TimeDelim!=':' && pFmt[j]==':') pFmt[j]=TimeDelim;
          }  
       } 
    }    

    // initialize weekdays
    WeekDays[0]="Sunday";
    WeekDays[1]="Monday";
    WeekDays[2]="Tuesday";
    WeekDays[3]="Wednesday";
    WeekDays[4]="Thursday";
    WeekDays[5]="Friday";
    WeekDays[6]="Saturday";

    // initialize month names
    MonthNames[0]="January";
    MonthNames[1]="February";
    MonthNames[2]="March";
    MonthNames[3]="April";
    MonthNames[4]="May";
    MonthNames[5]="June";
    MonthNames[6]="July";
    MonthNames[7]="August";
    MonthNames[8]="September";
    MonthNames[9]="October";
    MonthNames[10]="November";
    MonthNames[11]="December";

    // define default paper sizes
    i=0;
    DefPaperSize[i]=DMPAPER_LETTER;
    DefPaperName[i]="Letter";
    DefPaperWidth[i]=(float)8.5;
    DefPaperHeight[i]=(float)11;
     
    i=1;
    DefPaperSize[i]=DMPAPER_LEGAL;
    DefPaperName[i]="Legal";
    DefPaperWidth[i]=(float)8.5;
    DefPaperHeight[i]=(float)14;
     
    i=2;
    DefPaperSize[i]=DMPAPER_LEDGER;
    DefPaperName[i]="Ledger";
    DefPaperWidth[i]=(float)17;
    DefPaperHeight[i]=(float)11;

    i=3;
    DefPaperSize[i]=DMPAPER_TABLOID;
    DefPaperName[i]="Tabloid";
    DefPaperWidth[i]=(float)11;
    DefPaperHeight[i]=(float)17;
    
    i=4;
    DefPaperSize[i]=DMPAPER_STATEMENT;
    DefPaperName[i]="Statement";
    DefPaperWidth[i]=(float)5.5;
    DefPaperHeight[i]=(float)8.5;
    
    i=5;
    DefPaperSize[i]=DMPAPER_EXECUTIVE;
    DefPaperName[i]="Executive";
    DefPaperWidth[i]=(float)7.25;
    DefPaperHeight[i]=(float)10.5;

    i=6;
    DefPaperSize[i]=DMPAPER_A3;
    DefPaperName[i]="A3";
    DefPaperWidth[i]=MmToInches(297);
    DefPaperHeight[i]=MmToInches(420);

    i=7;
    DefPaperSize[i]=DMPAPER_A4;
    DefPaperName[i]="A4";
    DefPaperWidth[i]=MmToInches(210);
    DefPaperHeight[i]=MmToInches(297);

    i=8;
    DefPaperSize[i]=DMPAPER_A5;
    DefPaperName[i]="A5";
    DefPaperWidth[i]=MmToInches(148);
    DefPaperHeight[i]=MmToInches(210);

    i=9;
    DefPaperSize[i]=DMPAPER_B4;
    DefPaperName[i]="B4";
    DefPaperWidth[i]=MmToInches(250);
    DefPaperHeight[i]=MmToInches(354);

    i=10;
    DefPaperSize[i]=DMPAPER_B5;
    DefPaperName[i]="B5";
    DefPaperWidth[i]=MmToInches(182);
    DefPaperHeight[i]=MmToInches(257);
    
    i=11;
    DefPaperSize[i]=DMPAPER_ENV_10;
    DefPaperName[i]="Envelope 10";
    DefPaperWidth[i]=(float)4.5;
    DefPaperHeight[i]=(float)9.5;
    
    i=12;
    DefPaperSize[i]=DMPAPER_ENV_DL;
    DefPaperName[i]="Envelope DL";
    DefPaperWidth[i]=MmToInches(110);
    DefPaperHeight[i]=MmToInches(220);
    
    i=13;
    DefPaperSize[i]=DMPAPER_ENV_C5;
    DefPaperName[i]="Envelope C5";
    DefPaperWidth[i]=MmToInches(162);
    DefPaperHeight[i]=MmToInches(229);
    
    i=14;
    DefPaperSize[i]=DMPAPER_ENV_MONARCH;
    DefPaperName[i]="Envelope Monarch";
    DefPaperWidth[i]=(float)3.875;
    DefPaperHeight[i]=(float)7.5;
    
    i=15;
    DefPaperSize[i]=OURDMPAPER_A6;
    DefPaperName[i]="A6";
    DefPaperWidth[i]=MmToInches(105);
    DefPaperHeight[i]=MmToInches(148);

    DefPaperCount=i+1;

    // Ensure that (i+1) is equal to MAX_PAPERS constant
    

    // initialize tool bar bitmaps
    hToolbarBM=NULL;

    // initialize the cursor resource variables
    hTable1Cur=hTable2Cur=hTable3Cur=hTab1Cur=hHyperlinkCur=hPlusCur=hDragInCur=hDragInCopyCur=hDragOutCur=NULL;
    hWheelFullCur=hWheelUpCur=hWheelDownCur=hHBeamCur=NULL;

    hTerMenu=LoadMenu(hTerInst,"TerMenu");  // load menu only once to reduce less of user/gdi memory

    InitJpgZTable();                 // initialize the Jpg Z table

    // Create the window class
    if (!GetClassInfo(hTerInst,TER_CLASS,&wc)) {
       wc.style = CS_DBLCLKS|CS_OWNDC|CS_HREDRAW|CS_VREDRAW|CS_GLOBALCLASS;  // allocate a DC for this class
       wc.lpfnWndProc = (void far *) TerWndProc;
       wc.cbClsExtra = 0;
       wc.cbWndExtra = sizeof(PTERWND);                         // extra 4 bytes to store the data pointer
       wc.hInstance = hTerInst;
       wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
       wc.hCursor = LoadCursor(NULL,IDC_ARROW);
       wc.hbrBackground = GetStockObject(WHITE_BRUSH);
       wc.lpszMenuName =  NULL;
       wc.lpszClassName = TER_CLASS;
       RegisterClass(&wc);
    }

    // Create the toolbar window class
    lstrcpy(TlbClass,TER_CLASS);
    lstrcat(TlbClass,"Toolbar");
    if (!GetClassInfo(hTerInst,TlbClass,&wc)) {
       wc.style = CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_GLOBALCLASS;  // allocate a DC for this class
       wc.lpfnWndProc = (void far *) TlbWndProc;
       wc.cbClsExtra = 0;
       wc.cbWndExtra = sizeof(PTERWND);                         // extra 4 bytes to store the data pointer
       wc.hInstance = hTerInst;
       wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
       wc.hCursor = LoadCursor(NULL,IDC_ARROW);
       wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
       wc.lpszMenuName =  NULL;
       wc.lpszClassName = TlbClass;
       RegisterClass(&wc);
    }

    // define default toolbar text
    BalloonText[TLB_LINE]             ="";
    BalloonText[TLB_TYPEFACE]         ="Font";
    BalloonText[TLB_POINTSIZE]        ="Font Size";
    BalloonText[TLB_BOLD]             ="Bold";
    BalloonText[TLB_ITALIC]           ="Italic";
    BalloonText[TLB_ULINE]            ="Underline";
    BalloonText[TLB_ALIGN_LEFT]       ="Align Left";
    BalloonText[TLB_ALIGN_RIGHT]      ="Align Right";
    BalloonText[TLB_ALIGN_CENTER]     ="Center";
    BalloonText[TLB_ALIGN_JUSTIFY]    ="Justify";
    BalloonText[TLB_INC_INDENT]       ="Increase Indent";
    BalloonText[TLB_DEC_INDENT]       ="Decrease Indent";
    BalloonText[TLB_STYLE]            ="Style";
    BalloonText[TLB_ZOOM]             ="Zoom";
    BalloonText[TLB_CUT]              ="Cut";
    BalloonText[TLB_COPY]             ="Copy";
    BalloonText[TLB_PASTE]            ="Paste";
    BalloonText[TLB_SPACER]           ="";
    BalloonText[TLB_NEW]              ="New";
    BalloonText[TLB_OPEN]             ="Open";
    BalloonText[TLB_SAVE]             ="Save";
    BalloonText[TLB_PRINT]            ="Print";
    BalloonText[TLB_HELP]             ="Help";
    BalloonText[TLB_PAR]              ="Show Markers";
    BalloonText[TLB_PREVIEW]          ="Print Preview";
    BalloonText[TLB_NUMBER]           ="Numbering";
    BalloonText[TLB_BULLET]           ="Bullets";
    BalloonText[TLB_UNDO]             ="Undo";
    BalloonText[TLB_REDO]             ="Redo";
    BalloonText[TLB_FIND]             ="Find";
    BalloonText[TLB_DATE]             ="Insert Date/Time";
    BalloonText[TLB_PAGE_NUM]         ="Insert Page Number";
    BalloonText[TLB_PAGE_COUNT]       ="Insert Page Count";

    // Define toolbar icons
    for (i=0;i<MAX_TLB_LINES;i++) {       // initialize
       for (j=0;j<MAX_TLB_PER_LINE;j++) {
          TlbItem[i][j]=0;
          TlbItemHide[i][j]=FALSE;
       }
    } 

    // toolbar first row
    i=-1;
    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_LINE;
    i++;TlbItem[0][i]=TLB_LINE;

    i++;TlbItem[0][i]=TLB_NEW;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_OPEN;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_SAVE;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_PRINT;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_PREVIEW;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_LINE;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_CUT;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_COPY;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_PASTE;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_LINE;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_UNDO;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_REDO;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_LINE;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_FIND;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_LINE;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_DATE;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_PAGE_NUM;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_PAGE_COUNT;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_PAR;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_LINE;
    i++;TlbItem[0][i]=TLB_SPACER;

    i++;TlbItem[0][i]=TLB_HELP;
    i++;TlbItem[0][i]=TLB_SPACER;


    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_ZOOM;

    i++;TlbItem[0][i]=TLB_SPACER;
    i++;TlbItem[0][i]=TLB_LINE;
    i++;TlbItem[0][i]=TLB_SPACER;
    
    // toolbar second row
    i=-1;
    i++;TlbItem[1][i]=TLB_SPACER;
    i++;TlbItem[1][i]=TLB_LINE;
    i++;TlbItem[1][i]=TLB_LINE;
    i++;TlbItem[1][i]=TLB_SPACER;

    i++;TlbItem[1][i]=TLB_STYLE;
    i++;TlbItem[1][i]=TLB_SPACER;

    i++;TlbItem[1][i]=TLB_TYPEFACE;
    i++;TlbItem[1][i]=TLB_SPACER;

    i++;TlbItem[1][i]=TLB_POINTSIZE;
    i++;TlbItem[1][i]=TLB_SPACER;
    i++;TlbItem[1][i]=TLB_SPACER;

    i++;TlbItem[1][i]=TLB_BOLD;
    i++;TlbItem[1][i]=TLB_ITALIC;
    i++;TlbItem[1][i]=TLB_ULINE;

    i++;TlbItem[1][i]=TLB_SPACER;
    i++;TlbItem[1][i]=TLB_LINE;
    i++;TlbItem[1][i]=TLB_SPACER;
    
    i++;TlbItem[1][i]=TLB_ALIGN_LEFT;
    i++;TlbItem[1][i]=TLB_ALIGN_CENTER;
    i++;TlbItem[1][i]=TLB_ALIGN_RIGHT;
    i++;TlbItem[1][i]=TLB_ALIGN_JUSTIFY;

    i++;TlbItem[1][i]=TLB_SPACER;
    i++;TlbItem[1][i]=TLB_LINE;
    i++;TlbItem[1][i]=TLB_SPACER;
    
    i++;TlbItem[1][i]=TLB_NUMBER;
    i++;TlbItem[1][i]=TLB_BULLET;
    
    i++;TlbItem[1][i]=TLB_SPACER;

    i++;TlbItem[1][i]=TLB_INC_INDENT;
    i++;TlbItem[1][i]=TLB_DEC_INDENT;


    // initialize the spelltime function pointers
    StSearched=FALSE;
    hSpell=NULL;                // spell checker handle, if spelltime installed
    StParseLine=(LPVOID)NULL;
    StResetUserDict=(LPVOID)NULL;
    StClearHist=(LPVOID)NULL;
    SpellWord=(LPVOID)NULL;

    hHts=NULL;                    // HTML library handle

    SspSearched=FALSE;           // TRUE after ssp is searched
    hSsp=NULL;                   // ssp handle

    // initialize Wininet function pointers
    WininetSearched=FALSE;        // TRUE after searching wininet
    hWininet=NULL;               // WININET dll handle
    #if defined(WIN32)
       pInternetOpen=NULL;          // InternetOpen function
    #endif

    eval=true;              
    disable=false;
    extended=false;
    CheckEval(false,"");

		TerSetLicenseKey("8YP41-NM5Q2-34BL3");

    return TRUE;
}

/*****************************************************************************
    WEP:
    Library exit routine.
******************************************************************************/
#if defined(WIN32)
 int WINAPI _export TerExit(int nParameter)
#else
  CALLBACK _export WEP(int nParameter)
#endif   
{
    int i;

    // delete shared resources
    if (hTerMenu) DestroyMenu(hTerMenu);
    hTerMenu=NULL;

    // free the hTerAccTable table
    if (hTerAccTable) DestroyAcceleratorTable(hTerAccTable);
    hTerAccTable = NULL;

    for (i=0;i<MAX_MSGS;i++) {    // free custom messages
       if (CustomMsg[i]) MemFree(MsgString[i]);
       CustomMsg[i]=0;
    }

    if (hSpell) FreeLibrary(hSpell);  // unload the spell checker
    hSpell=NULL;
    StSearched=FALSE;
    
    if (hSsp) FreeLibrary(hSsp);      // unload the ssp library
    hSsp=NULL;
    SspSearched=FALSE;

    if (hWininet) FreeLibrary(hWininet);      // unload the ssp library
    hWininet=NULL;
    WininetSearched=FALSE;            

    UnregisterClass(TER_CLASS,hTerInst);
    UnregisterClass(TlbClass,hTerInst);

    return 1;
}

/******************************************************************************
    TerInitMenu:
    Initialze the menu before displaying
******************************************************************************/
TerInitMenu(PTERWND w,HMENU hMenu)
{
    //****** File Menu *****
    EnableMenuItem(hMenu,ID_NEW,TerMenuEnable(hTerWnd,ID_NEW));
    EnableMenuItem(hMenu,ID_OPEN,TerMenuEnable(hTerWnd,ID_OPEN));
    EnableMenuItem(hMenu,ID_SAVE,TerMenuEnable(hTerWnd,ID_SAVE));
    EnableMenuItem(hMenu,ID_SAVEAS,TerMenuEnable(hTerWnd,ID_SAVEAS));
    EnableMenuItem(hMenu,ID_PRINT,TerMenuEnable(hTerWnd,ID_PRINT));
    EnableMenuItem(hMenu,ID_PRINT_PREVIEW,TerMenuEnable(hTerWnd,ID_PRINT_PREVIEW));
    EnableMenuItem(hMenu,ID_PAGE_OPTIONS,TerMenuEnable(hTerWnd,ID_PAGE_OPTIONS));
    EnableMenuItem(hMenu,ID_PRINT_OPTIONS,TerMenuEnable(hTerWnd,ID_PRINT_OPTIONS));
    EnableMenuItem(hMenu,ID_QUIT,TerMenuEnable(hTerWnd,ID_QUIT));

    CheckMenuItem(hMenu,ID_PRINT_PREVIEW,TerMenuSelect(hTerWnd,ID_PRINT_PREVIEW));

    //**** Line Edit Menu ***
    EnableMenuItem(hMenu,ID_INS_AFT,TerMenuEnable(hTerWnd,ID_INS_AFT));
    EnableMenuItem(hMenu,ID_INS_BEF,TerMenuEnable(hTerWnd,ID_INS_BEF));
    EnableMenuItem(hMenu,ID_JOIN_LINE,TerMenuEnable(hTerWnd,ID_JOIN_LINE));
    EnableMenuItem(hMenu,ID_SPLIT_LINE,TerMenuEnable(hTerWnd,ID_SPLIT_LINE));
    EnableMenuItem(hMenu,ID_DEL_LINE,TerMenuEnable(hTerWnd,ID_DEL_LINE));

    //**** Block Edit Menu and other edit commands***
    EnableMenuItem(hMenu,ID_SELECT_ALL,TerMenuEnable(hTerWnd,ID_SELECT_ALL));
    EnableMenuItem(hMenu,ID_UNDO,TerMenuEnable(hTerWnd,ID_UNDO));
    EnableMenuItem(hMenu,ID_REDO,TerMenuEnable(hTerWnd,ID_REDO));

    EnableMenuItem(hMenu,ID_INLINE_IME,TerMenuEnable(hTerWnd,ID_INLINE_IME));
    CheckMenuItem(hMenu,ID_INLINE_IME,TerMenuSelect(hTerWnd,ID_INLINE_IME));

    EnableMenuItem(hMenu,ID_DOC_RTL,TerMenuEnable(hTerWnd,ID_DOC_RTL));
    CheckMenuItem(hMenu,ID_DOC_RTL,TerMenuSelect(hTerWnd,ID_DOC_RTL));

    EnableMenuItem(hMenu,ID_CUT,TerMenuEnable(hTerWnd,ID_CUT));
    EnableMenuItem(hMenu,ID_COPY,TerMenuEnable(hTerWnd,ID_COPY));
    EnableMenuItem(hMenu,ID_PASTE,TerMenuEnable(hTerWnd,ID_PASTE));
    
    EnableMenuItem(hMenu,ID_PASTE_TEXT,TerMenuEnable(hTerWnd,ID_PASTE_TEXT));
    EnableMenuItem(hMenu,ID_PASTE_SPEC,TerMenuEnable(hTerWnd,ID_PASTE_SPEC));
    EnableMenuItem(hMenu,ID_EMBED_PICT,TerMenuEnable(hTerWnd,ID_EMBED_PICT));
    EnableMenuItem(hMenu,ID_LINK_PICT,TerMenuEnable(hTerWnd,ID_LINK_PICT));
    EnableMenuItem(hMenu,ID_EDIT_PICT,TerMenuEnable(hTerWnd,ID_EDIT_PICT));

    EnableMenuItem(hMenu,ID_SECT_OPTIONS,TerMenuEnable(hTerWnd,ID_SECT_OPTIONS));
    EnableMenuItem(hMenu,ID_OVERRIDE_BIN,TerMenuEnable(hTerWnd,ID_OVERRIDE_BIN));

    EnableMenuItem(hMenu,ID_BLOCK_COPY,TerMenuEnable(hTerWnd,ID_BLOCK_COPY));
    EnableMenuItem(hMenu,ID_BLOCK_MOVE,TerMenuEnable(hTerWnd,ID_BLOCK_MOVE));

    EnableMenuItem(hMenu,ID_EDIT_OLE,TerMenuEnable(hTerWnd,ID_EDIT_OLE));

    EnableMenuItem(hMenu,ID_EDIT_STYLE,TerMenuEnable(hTerWnd,ID_EDIT_STYLE));
    CheckMenuItem(hMenu,ID_EDIT_STYLE,TerMenuSelect(hTerWnd,ID_EDIT_STYLE));

    EnableMenuItem(hMenu,ID_CREATE_LIST,TerMenuEnable(hTerWnd,ID_CREATE_LIST));
    CheckMenuItem(hMenu,ID_CREATE_LIST,TerMenuSelect(hTerWnd,ID_CREATE_LIST));

    EnableMenuItem(hMenu,ID_EDIT_LIST,TerMenuEnable(hTerWnd,ID_EDIT_LIST));
    CheckMenuItem(hMenu,ID_EDIT_LIST,TerMenuSelect(hTerWnd,ID_EDIT_LIST));

    EnableMenuItem(hMenu,ID_CREATE_LIST_OR,TerMenuEnable(hTerWnd,ID_CREATE_LIST_OR));
    CheckMenuItem(hMenu,ID_CREATE_LIST_OR,TerMenuSelect(hTerWnd,ID_CREATE_LIST_OR));

    EnableMenuItem(hMenu,ID_EDIT_LIST_OR,TerMenuEnable(hTerWnd,ID_EDIT_LIST_OR));
    CheckMenuItem(hMenu,ID_EDIT_LIST_OR,TerMenuSelect(hTerWnd,ID_EDIT_LIST_OR));

    EnableMenuItem(hMenu,ID_EDIT_LIST_LEVEL,TerMenuEnable(hTerWnd,ID_EDIT_LIST_LEVEL));
    CheckMenuItem(hMenu,ID_EDIT_LIST_LEVEL,TerMenuSelect(hTerWnd,ID_EDIT_LIST_LEVEL));

    EnableMenuItem(hMenu,ID_FRAME_YBASE,TerMenuEnable(hTerWnd,ID_FRAME_YBASE));
    EnableMenuItem(hMenu,ID_FRAME_ROTATE_TEXT,TerMenuEnable(hTerWnd,ID_FRAME_ROTATE_TEXT));
    EnableMenuItem(hMenu,ID_EDIT_DOB,TerMenuEnable(hTerWnd,ID_EDIT_DOB));

    //**** Character Formatting Features ***
    EnableMenuItem(hMenu,ID_CHAR_NORMAL,TerMenuEnable(hTerWnd,ID_CHAR_NORMAL));
    EnableMenuItem(hMenu,ID_BOLD_ON,TerMenuEnable(hTerWnd,ID_BOLD_ON));
    EnableMenuItem(hMenu,ID_ULINE_ON,TerMenuEnable(hTerWnd,ID_ULINE_ON));
    EnableMenuItem(hMenu,ID_ULINED_ON,TerMenuEnable(hTerWnd,ID_ULINED_ON));
    EnableMenuItem(hMenu,ID_ITALIC_ON,TerMenuEnable(hTerWnd,ID_ITALIC_ON));
    EnableMenuItem(hMenu,ID_STRIKE_ON,TerMenuEnable(hTerWnd,ID_STRIKE_ON));
    EnableMenuItem(hMenu,ID_SUPSCR_ON,TerMenuEnable(hTerWnd,ID_SUPSCR_ON));
    EnableMenuItem(hMenu,ID_SUBSCR_ON,TerMenuEnable(hTerWnd,ID_SUBSCR_ON));
    EnableMenuItem(hMenu,ID_PROTECT_ON,TerMenuEnable(hTerWnd,ID_PROTECT_ON));
    EnableMenuItem(hMenu,ID_COLOR,TerMenuEnable(hTerWnd,ID_COLOR));
    EnableMenuItem(hMenu,ID_ULINE_COLOR,TerMenuEnable(hTerWnd,ID_ULINE_COLOR));
    EnableMenuItem(hMenu,ID_BK_COLOR,TerMenuEnable(hTerWnd,ID_BK_COLOR));
    EnableMenuItem(hMenu,ID_FONTS,TerMenuEnable(hTerWnd,ID_FONTS));
    EnableMenuItem(hMenu,ID_CHAR_STYLE,TerMenuEnable(hTerWnd,ID_CHAR_STYLE));
    EnableMenuItem(hMenu,ID_CHAR_SPACE,TerMenuEnable(hTerWnd,ID_CHAR_SPACE));
    EnableMenuItem(hMenu,ID_HIDDEN_ON,TerMenuEnable(hTerWnd,ID_HIDDEN_ON));
    EnableMenuItem(hMenu,ID_BOX_ON,TerMenuEnable(hTerWnd,ID_BOX_ON));
    EnableMenuItem(hMenu,ID_HLINK_ON,TerMenuEnable(hTerWnd,ID_HLINK_ON));
    EnableMenuItem(hMenu,ID_CAPS_ON,TerMenuEnable(hTerWnd,ID_CAPS_ON));
    EnableMenuItem(hMenu,ID_SCAPS_ON,TerMenuEnable(hTerWnd,ID_SCAPS_ON));

    CheckMenuItem(hMenu,ID_BOLD_ON,TerMenuSelect(hTerWnd,ID_BOLD_ON));
    CheckMenuItem(hMenu,ID_ULINE_ON,TerMenuSelect(hTerWnd,ID_ULINE_ON));
    CheckMenuItem(hMenu,ID_ULINED_ON,TerMenuSelect(hTerWnd,ID_ULINED_ON));
    CheckMenuItem(hMenu,ID_ITALIC_ON,TerMenuSelect(hTerWnd,ID_ITALIC_ON));
    CheckMenuItem(hMenu,ID_STRIKE_ON,TerMenuSelect(hTerWnd,ID_STRIKE_ON));
    CheckMenuItem(hMenu,ID_SUPSCR_ON,TerMenuSelect(hTerWnd,ID_SUPSCR_ON));
    CheckMenuItem(hMenu,ID_SUBSCR_ON,TerMenuSelect(hTerWnd,ID_SUBSCR_ON));
    CheckMenuItem(hMenu,ID_PROTECT_ON,TerMenuSelect(hTerWnd,ID_PROTECT_ON));
    CheckMenuItem(hMenu,ID_CHAR_SPACE,TerMenuSelect(hTerWnd,ID_CHAR_SPACE));
    CheckMenuItem(hMenu,ID_HIDDEN_ON,TerMenuSelect(hTerWnd,ID_HIDDEN_ON));
    CheckMenuItem(hMenu,ID_BOX_ON,TerMenuSelect(hTerWnd,ID_BOX_ON));
    CheckMenuItem(hMenu,ID_CHAR_STYLE,TerMenuSelect(hTerWnd,ID_CHAR_STYLE));
    CheckMenuItem(hMenu,ID_HLINK_ON,TerMenuSelect(hTerWnd,ID_HLINK_ON));
    CheckMenuItem(hMenu,ID_CAPS_ON,TerMenuSelect(hTerWnd,ID_CAPS_ON));
    CheckMenuItem(hMenu,ID_SCAPS_ON,TerMenuSelect(hTerWnd,ID_SCAPS_ON));

    //**** Paragraph Formatting Features ***
    EnableMenuItem(hMenu,ID_PARA_NORMAL,TerMenuEnable(hTerWnd,ID_PARA_NORMAL));
    EnableMenuItem(hMenu,ID_DOUBLE_SPACE,TerMenuEnable(hTerWnd,ID_DOUBLE_SPACE));
    EnableMenuItem(hMenu,ID_PARA_KEEP,TerMenuEnable(hTerWnd,ID_PARA_KEEP));
    EnableMenuItem(hMenu,ID_PARA_KEEP_NEXT,TerMenuEnable(hTerWnd,ID_PARA_KEEP_NEXT));
    EnableMenuItem(hMenu,ID_WIDOW_ORPHAN,TerMenuEnable(hTerWnd,ID_WIDOW_ORPHAN));
    EnableMenuItem(hMenu,ID_PAGE_BREAK_BEFORE,TerMenuEnable(hTerWnd,ID_PAGE_BREAK_BEFORE));
    EnableMenuItem(hMenu,ID_CENTER,TerMenuEnable(hTerWnd,ID_CENTER));
    EnableMenuItem(hMenu,ID_LEFT_JUSTIFY,TerMenuEnable(hTerWnd,ID_LEFT_JUSTIFY));
    EnableMenuItem(hMenu,ID_RIGHT_JUSTIFY,TerMenuEnable(hTerWnd,ID_RIGHT_JUSTIFY));
    EnableMenuItem(hMenu,ID_JUSTIFY,TerMenuEnable(hTerWnd,ID_JUSTIFY));
    EnableMenuItem(hMenu,ID_LEFT_INDENT,TerMenuEnable(hTerWnd,ID_LEFT_INDENT));
    EnableMenuItem(hMenu,ID_LEFT_INDENT_DEC,TerMenuEnable(hTerWnd,ID_LEFT_INDENT_DEC));
    EnableMenuItem(hMenu,ID_RIGHT_INDENT,TerMenuEnable(hTerWnd,ID_RIGHT_INDENT));
    EnableMenuItem(hMenu,ID_HANGING_INDENT,TerMenuEnable(hTerWnd,ID_HANGING_INDENT));
    EnableMenuItem(hMenu,ID_PARA_BORDER,TerMenuEnable(hTerWnd,ID_PARA_BORDER));
    EnableMenuItem(hMenu,ID_PARA_SPACING,TerMenuEnable(hTerWnd,ID_PARA_SPACING));
    EnableMenuItem(hMenu,ID_BULLET,TerMenuEnable(hTerWnd,ID_BULLET));
    EnableMenuItem(hMenu,ID_PARA_NBR,TerMenuEnable(hTerWnd,ID_PARA_NBR));
    EnableMenuItem(hMenu,ID_LIST_LEVEL_INC,TerMenuEnable(hTerWnd,ID_LIST_LEVEL_INC));
    EnableMenuItem(hMenu,ID_LIST_LEVEL_DEC,TerMenuEnable(hTerWnd,ID_LIST_LEVEL_DEC));
    EnableMenuItem(hMenu,ID_PARA_LIST,TerMenuEnable(hTerWnd,ID_PARA_LIST));
    EnableMenuItem(hMenu,ID_PARA_BK_COLOR,TerMenuEnable(hTerWnd,ID_PARA_BK_COLOR));
    EnableMenuItem(hMenu,ID_PARA_RTL,TerMenuEnable(hTerWnd,ID_PARA_RTL));
    EnableMenuItem(hMenu,ID_PARA_STYLE,TerMenuEnable(hTerWnd,ID_PARA_STYLE));

    CheckMenuItem(hMenu,ID_DOUBLE_SPACE,TerMenuSelect(hTerWnd,ID_DOUBLE_SPACE));
    CheckMenuItem(hMenu,ID_PARA_KEEP,TerMenuSelect(hTerWnd,ID_PARA_KEEP));
    CheckMenuItem(hMenu,ID_PARA_KEEP_NEXT,TerMenuSelect(hTerWnd,ID_PARA_KEEP_NEXT));
    CheckMenuItem(hMenu,ID_WIDOW_ORPHAN,TerMenuSelect(hTerWnd,ID_WIDOW_ORPHAN));
    CheckMenuItem(hMenu,ID_PAGE_BREAK_BEFORE,TerMenuSelect(hTerWnd,ID_PAGE_BREAK_BEFORE));
    CheckMenuItem(hMenu,ID_CENTER,TerMenuSelect(hTerWnd,ID_CENTER));
    CheckMenuItem(hMenu,ID_LEFT_JUSTIFY,TerMenuSelect(hTerWnd,ID_LEFT_JUSTIFY));
    CheckMenuItem(hMenu,ID_RIGHT_JUSTIFY,TerMenuSelect(hTerWnd,ID_RIGHT_JUSTIFY));
    CheckMenuItem(hMenu,ID_JUSTIFY,TerMenuSelect(hTerWnd,ID_JUSTIFY));
    CheckMenuItem(hMenu,ID_LEFT_INDENT,TerMenuSelect(hTerWnd,ID_LEFT_INDENT));
    CheckMenuItem(hMenu,ID_RIGHT_INDENT,TerMenuSelect(hTerWnd,ID_RIGHT_INDENT));
    CheckMenuItem(hMenu,ID_HANGING_INDENT,TerMenuSelect(hTerWnd,ID_HANGING_INDENT));
    CheckMenuItem(hMenu,ID_PARA_BORDER,TerMenuSelect(hTerWnd,ID_PARA_BORDER));
    CheckMenuItem(hMenu,ID_PARA_SPACING,TerMenuSelect(hTerWnd,ID_PARA_SPACING));
    CheckMenuItem(hMenu,ID_BULLET,TerMenuSelect(hTerWnd,ID_BULLET));
    CheckMenuItem(hMenu,ID_PARA_NBR,TerMenuSelect(hTerWnd,ID_PARA_NBR));
    CheckMenuItem(hMenu,ID_LIST_LEVEL_INC,TerMenuSelect(hTerWnd,ID_LIST_LEVEL_INC));
    CheckMenuItem(hMenu,ID_LIST_LEVEL_DEC,TerMenuSelect(hTerWnd,ID_LIST_LEVEL_DEC));
    CheckMenuItem(hMenu,ID_PARA_LIST,TerMenuSelect(hTerWnd,ID_PARA_LIST));
    CheckMenuItem(hMenu,ID_PARA_BK_COLOR,TerMenuSelect(hTerWnd,ID_PARA_BK_COLOR));
    CheckMenuItem(hMenu,ID_PARA_RTL,TerMenuSelect(hTerWnd,ID_PARA_RTL));
    CheckMenuItem(hMenu,ID_PARA_STYLE,TerMenuSelect(hTerWnd,ID_PARA_STYLE));

    //**** Table functions ***
    EnableMenuItem(hMenu,ID_TABLE_INSERT,TerMenuEnable(hTerWnd,ID_TABLE_INSERT));
    EnableMenuItem(hMenu,ID_TABLE_INSERT_ROW,TerMenuEnable(hTerWnd,ID_TABLE_INSERT_ROW));
    EnableMenuItem(hMenu,ID_TABLE_MERGE_CELLS,TerMenuEnable(hTerWnd,ID_TABLE_MERGE_CELLS));
    EnableMenuItem(hMenu,ID_TABLE_DEL_CELLS,TerMenuEnable(hTerWnd,ID_TABLE_DEL_CELLS));
    EnableMenuItem(hMenu,ID_TABLE_SPLIT_CELL,TerMenuEnable(hTerWnd,ID_TABLE_SPLIT_CELL));
    EnableMenuItem(hMenu,ID_TABLE_ROW_POS,TerMenuEnable(hTerWnd,ID_TABLE_ROW_POS));
    EnableMenuItem(hMenu,ID_TABLE_ROW_KEEP,TerMenuEnable(hTerWnd,ID_TABLE_ROW_KEEP));
    EnableMenuItem(hMenu,ID_TABLE_ROW_HEIGHT,TerMenuEnable(hTerWnd,ID_TABLE_ROW_HEIGHT));

    EnableMenuItem(hMenu,ID_TABLE_ROW_RTL,TerMenuEnable(hTerWnd,ID_TABLE_ROW_RTL));
    CheckMenuItem(hMenu,ID_TABLE_ROW_RTL,TerMenuSelect(hTerWnd,ID_TABLE_ROW_RTL));
    
    EnableMenuItem(hMenu,ID_TABLE_INSERT_COL,TerMenuEnable(hTerWnd,ID_TABLE_INSERT_COL));
    EnableMenuItem(hMenu,ID_TABLE_CELL_BORDER,TerMenuEnable(hTerWnd,ID_TABLE_CELL_BORDER));
    EnableMenuItem(hMenu,ID_TABLE_CELL_BORDER_COLOR,TerMenuEnable(hTerWnd,ID_TABLE_CELL_BORDER_COLOR));
    EnableMenuItem(hMenu,ID_TABLE_CELL_WIDTH,TerMenuEnable(hTerWnd,ID_TABLE_CELL_WIDTH));
    EnableMenuItem(hMenu,ID_TABLE_CELL_SHADE,TerMenuEnable(hTerWnd,ID_TABLE_CELL_SHADE));
    EnableMenuItem(hMenu,ID_TABLE_CELL_COLOR,TerMenuEnable(hTerWnd,ID_TABLE_CELL_COLOR));
    EnableMenuItem(hMenu,ID_TABLE_CELL_VALIGN,TerMenuEnable(hTerWnd,ID_TABLE_CELL_VALIGN));
    EnableMenuItem(hMenu,ID_TABLE_CELL_VTEXT,TerMenuEnable(hTerWnd,ID_TABLE_CELL_VTEXT));
    EnableMenuItem(hMenu,ID_TABLE_SEL_COL,TerMenuEnable(hTerWnd,ID_TABLE_SEL_COL));
    EnableMenuItem(hMenu,ID_TABLE_SHOW_GRID,TerMenuEnable(hTerWnd,ID_TABLE_SHOW_GRID));
    CheckMenuItem(hMenu,ID_TABLE_SHOW_GRID,TerMenuSelect(hTerWnd,ID_TABLE_SHOW_GRID));

    EnableMenuItem(hMenu,ID_TABLE_HDR_ROW,TerMenuEnable(hTerWnd,ID_TABLE_HDR_ROW));
    CheckMenuItem(hMenu,ID_TABLE_HDR_ROW,TerMenuSelect(hTerWnd,ID_TABLE_HDR_ROW));
    EnableMenuItem(hMenu,ID_TABLE_ROW_KEEP,TerMenuEnable(hTerWnd,ID_TABLE_ROW_KEEP));
    CheckMenuItem(hMenu,ID_TABLE_ROW_KEEP,TerMenuSelect(hTerWnd,ID_TABLE_ROW_KEEP));

    //**** Tab functions ***
    EnableMenuItem(hMenu,ID_TAB_SET,TerMenuEnable(hTerWnd,ID_TAB_SET));
    EnableMenuItem(hMenu,ID_TAB_CLEAR,TerMenuEnable(hTerWnd,ID_TAB_CLEAR));
    EnableMenuItem(hMenu,ID_TAB_CLEAR_ALL,TerMenuEnable(hTerWnd,ID_TAB_CLEAR_ALL));

    //**** View functions ***
    CheckMenuItem(hMenu,ID_PAGE_MODE,TerMenuSelect(hTerWnd,ID_PAGE_MODE));
    EnableMenuItem(hMenu,ID_PAGE_MODE,TerMenuEnable(hTerWnd,ID_PAGE_MODE));

    CheckMenuItem(hMenu,ID_FITTED_VIEW,TerMenuSelect(hTerWnd,ID_FITTED_VIEW));
    EnableMenuItem(hMenu,ID_FITTED_VIEW,TerMenuEnable(hTerWnd,ID_FITTED_VIEW));

    CheckMenuItem(hMenu,ID_VIEW_HDR_FTR,TerMenuSelect(hTerWnd,ID_VIEW_HDR_FTR));
    EnableMenuItem(hMenu,ID_VIEW_HDR_FTR,TerMenuEnable(hTerWnd,ID_VIEW_HDR_FTR));

    CheckMenuItem(hMenu,ID_SHOW_HIDDEN,TerMenuSelect(hTerWnd,ID_SHOW_HIDDEN));
    EnableMenuItem(hMenu,ID_SHOW_HIDDEN,TerMenuEnable(hTerWnd,ID_SHOW_HIDDEN));

    CheckMenuItem(hMenu,ID_SHOW_FIELD_NAMES,TerMenuSelect(hTerWnd,ID_SHOW_FIELD_NAMES));
    EnableMenuItem(hMenu,ID_SHOW_FIELD_NAMES,TerMenuEnable(hTerWnd,ID_SHOW_FIELD_NAMES));

    CheckMenuItem(hMenu,ID_EDIT_FNOTE,TerMenuSelect(hTerWnd,ID_EDIT_FNOTE));
    EnableMenuItem(hMenu,ID_EDIT_FNOTE,TerMenuEnable(hTerWnd,ID_EDIT_FNOTE));

    CheckMenuItem(hMenu,ID_EDIT_ENOTE,TerMenuSelect(hTerWnd,ID_EDIT_ENOTE));
    EnableMenuItem(hMenu,ID_EDIT_ENOTE,TerMenuEnable(hTerWnd,ID_EDIT_ENOTE));

    CheckMenuItem(hMenu,ID_TOOL_BAR,TerMenuSelect(hTerWnd,ID_TOOL_BAR));
    EnableMenuItem(hMenu,ID_TOOL_BAR,TerMenuEnable(hTerWnd,ID_TOOL_BAR));
    CheckMenuItem(hMenu,ID_RULER,TerMenuSelect(hTerWnd,ID_RULER));
    EnableMenuItem(hMenu,ID_RULER,TerMenuEnable(hTerWnd,ID_RULER));
    CheckMenuItem(hMenu,ID_STATUS_RIBBON,TerMenuSelect(hTerWnd,ID_STATUS_RIBBON));
    EnableMenuItem(hMenu,ID_STATUS_RIBBON,TerMenuEnable(hTerWnd,ID_STATUS_RIBBON));

    CheckMenuItem(hMenu,ID_SHOW_PARA_MARK,TerMenuSelect(hTerWnd,ID_SHOW_PARA_MARK));
    EnableMenuItem(hMenu,ID_SHOW_PARA_MARK,TerMenuEnable(hTerWnd,ID_SHOW_PARA_MARK));
    CheckMenuItem(hMenu,ID_SHOW_PAGE_BORDER,TerMenuSelect(hTerWnd,ID_SHOW_PAGE_BORDER));
    EnableMenuItem(hMenu,ID_SHOW_PAGE_BORDER,TerMenuEnable(hTerWnd,ID_SHOW_PAGE_BORDER));

    //**** Miscellaenous functions ***
    EnableMenuItem(hMenu,ID_REPAGINATE,TerMenuEnable(hTerWnd,ID_REPAGINATE));
    EnableMenuItem(hMenu,ID_PAGE_BREAK,TerMenuEnable(hTerWnd,ID_PAGE_BREAK));
    EnableMenuItem(hMenu,ID_INSERT_PARA_FRAME,TerMenuEnable(hTerWnd,ID_INSERT_PARA_FRAME));
    EnableMenuItem(hMenu,ID_INSERT_DRAW_OBJECT,TerMenuEnable(hTerWnd,ID_INSERT_DRAW_OBJECT));
    EnableMenuItem(hMenu,ID_INSERT_DATA_FIELD,TerMenuEnable(hTerWnd,ID_INSERT_DATA_FIELD));
    EnableMenuItem(hMenu,ID_INSERT_INPUT_FIELD,TerMenuEnable(hTerWnd,ID_INSERT_INPUT_FIELD));
    EnableMenuItem(hMenu,ID_INSERT_CHECKBOX,TerMenuEnable(hTerWnd,ID_INSERT_CHECKBOX));
    EnableMenuItem(hMenu,ID_INSERT_HLINK,TerMenuEnable(hTerWnd,ID_INSERT_HLINK));
    EnableMenuItem(hMenu,ID_SECT_BREAK,TerMenuEnable(hTerWnd,ID_SECT_BREAK));
    EnableMenuItem(hMenu,ID_COL_BREAK,TerMenuEnable(hTerWnd,ID_COL_BREAK));
    EnableMenuItem(hMenu,ID_INSERT_OBJECT,TerMenuEnable(hTerWnd,ID_INSERT_OBJECT));
    EnableMenuItem(hMenu,ID_REPLACE,TerMenuEnable(hTerWnd,ID_REPLACE));
    EnableMenuItem(hMenu,ID_SEARCH,TerMenuEnable(hTerWnd,ID_SEARCH));
    EnableMenuItem(hMenu,ID_SEARCH_FOR,TerMenuEnable(hTerWnd,ID_SEARCH_FOR));
    EnableMenuItem(hMenu,ID_SEARCH_BACK,TerMenuEnable(hTerWnd,ID_SEARCH_BACK));
    EnableMenuItem(hMenu,ID_JUMP,TerMenuEnable(hTerWnd,ID_JUMP));
    EnableMenuItem(hMenu,ID_SPELL,TerMenuEnable(hTerWnd,ID_SPELL));
    EnableMenuItem(hMenu,ID_DEL_PREV_WORD,TerMenuEnable(hTerWnd,ID_DEL_PREV_WORD));

    EnableMenuItem(hMenu,ID_BKND_PICT,TerMenuEnable(hTerWnd,ID_BKND_PICT));
    CheckMenuItem(hMenu,ID_BKND_PICT,TerMenuSelect(hTerWnd,ID_BKND_PICT));

    EnableMenuItem(hMenu,ID_WATERMARK,TerMenuEnable(hTerWnd,ID_WATERMARK));
    CheckMenuItem(hMenu,ID_WATERMARK,TerMenuSelect(hTerWnd,ID_WATERMARK));

    EnableMenuItem(hMenu,ID_ZOOM,TerMenuEnable(hTerWnd,ID_ZOOM));
    CheckMenuItem(hMenu,ID_ZOOM,TerMenuSelect(hTerWnd,ID_ZOOM));

    EnableMenuItem(hMenu,ID_AUTO_SPELL,TerMenuEnable(hTerWnd,ID_AUTO_SPELL));
    CheckMenuItem(hMenu,ID_AUTO_SPELL,TerMenuSelect(hTerWnd,ID_AUTO_SPELL));

    CheckMenuItem(hMenu,ID_INSERT_PAGE_NUMBER,TerMenuSelect(hTerWnd,ID_INSERT_PAGE_NUMBER));
    EnableMenuItem(hMenu,ID_INSERT_PAGE_NUMBER,TerMenuEnable(hTerWnd,ID_INSERT_PAGE_NUMBER));

    CheckMenuItem(hMenu,ID_INSERT_PAGE_COUNT,TerMenuSelect(hTerWnd,ID_INSERT_PAGE_COUNT));
    EnableMenuItem(hMenu,ID_INSERT_PAGE_COUNT,TerMenuEnable(hTerWnd,ID_INSERT_PAGE_COUNT));
    
    CheckMenuItem(hMenu,ID_INSERT_DATE_TIME,TerMenuSelect(hTerWnd,ID_INSERT_DATE_TIME));
    EnableMenuItem(hMenu,ID_INSERT_DATE_TIME,TerMenuEnable(hTerWnd,ID_INSERT_DATE_TIME));

    CheckMenuItem(hMenu,ID_INSERT_TOC,TerMenuSelect(hTerWnd,ID_INSERT_TOC));
    EnableMenuItem(hMenu,ID_INSERT_TOC,TerMenuEnable(hTerWnd,ID_INSERT_TOC));

    CheckMenuItem(hMenu,ID_INSERT_NBSPACE,TerMenuSelect(hTerWnd,ID_INSERT_NBSPACE));
    EnableMenuItem(hMenu,ID_INSERT_NBSPACE,TerMenuEnable(hTerWnd,ID_INSERT_NBSPACE));

    CheckMenuItem(hMenu,ID_INSERT_NBDASH,TerMenuSelect(hTerWnd,ID_INSERT_NBDASH));
    EnableMenuItem(hMenu,ID_INSERT_NBDASH,TerMenuEnable(hTerWnd,ID_INSERT_NBDASH));

    CheckMenuItem(hMenu,ID_INSERT_HYPH,TerMenuSelect(hTerWnd,ID_INSERT_HYPH));
    EnableMenuItem(hMenu,ID_INSERT_HYPH,TerMenuEnable(hTerWnd,ID_INSERT_HYPH));

    EnableMenuItem(hMenu,ID_INSERT_FNOTE,TerMenuEnable(hTerWnd,ID_INSERT_FNOTE));
    EnableMenuItem(hMenu,ID_INSERT_ENOTE,TerMenuEnable(hTerWnd,ID_INSERT_ENOTE));
    EnableMenuItem(hMenu,ID_INSERT_BOOKMARK,TerMenuEnable(hTerWnd,ID_INSERT_BOOKMARK));

    CheckMenuItem(hMenu,ID_PROTECTION_LOCK,TerMenuSelect(hTerWnd,ID_PROTECTION_LOCK));
    EnableMenuItem(hMenu,ID_PROTECTION_LOCK,TerMenuEnable(hTerWnd,ID_PROTECTION_LOCK));

    CheckMenuItem(hMenu,ID_TRACK_CHANGES,TerMenuSelect(hTerWnd,ID_TRACK_CHANGES));
    EnableMenuItem(hMenu,ID_TRACK_CHANGES,TerMenuEnable(hTerWnd,ID_TRACK_CHANGES));

    CheckMenuItem(hMenu,ID_NEXT_CHANGE,TerMenuSelect(hTerWnd,ID_NEXT_CHANGE));
    EnableMenuItem(hMenu,ID_NEXT_CHANGE,TerMenuEnable(hTerWnd,ID_NEXT_CHANGE));

    CheckMenuItem(hMenu,ID_PREV_CHANGE,TerMenuSelect(hTerWnd,ID_PREV_CHANGE));
    EnableMenuItem(hMenu,ID_PREV_CHANGE,TerMenuEnable(hTerWnd,ID_PREV_CHANGE));

    CheckMenuItem(hMenu,ID_ACCEPT_CHANGE,TerMenuSelect(hTerWnd,ID_ACCEPT_CHANGE));
    EnableMenuItem(hMenu,ID_ACCEPT_CHANGE,TerMenuEnable(hTerWnd,ID_ACCEPT_CHANGE));

    CheckMenuItem(hMenu,ID_ACCEPT_ALL_CHANGES,TerMenuSelect(hTerWnd,ID_ACCEPT_ALL_CHANGES));
    EnableMenuItem(hMenu,ID_ACCEPT_ALL_CHANGES,TerMenuEnable(hTerWnd,ID_ACCEPT_ALL_CHANGES));

    CheckMenuItem(hMenu,ID_SNAP_TO_GRID,TerMenuSelect(hTerWnd,ID_SNAP_TO_GRID));
    EnableMenuItem(hMenu,ID_SNAP_TO_GRID,TerMenuEnable(hTerWnd,ID_SNAP_TO_GRID));

    CheckMenuItem(hMenu,ID_SHOW_HYPERLINK_CURSOR,TerMenuSelect(hTerWnd,ID_SHOW_HYPERLINK_CURSOR));
    EnableMenuItem(hMenu,ID_SHOW_HYPERLINK_CURSOR,TerMenuEnable(hTerWnd,ID_SHOW_HYPERLINK_CURSOR));

    CheckMenuItem(hMenu,ID_EDIT_HDR_FTR,TerMenuSelect(hTerWnd,ID_EDIT_HDR_FTR));
    EnableMenuItem(hMenu,ID_EDIT_HDR_FTR,TerMenuEnable(hTerWnd,ID_EDIT_HDR_FTR));

    CheckMenuItem(hMenu,ID_CREATE_FIRST_HDR,TerMenuSelect(hTerWnd,ID_CREATE_FIRST_HDR));
    EnableMenuItem(hMenu,ID_CREATE_FIRST_HDR,TerMenuEnable(hTerWnd,ID_CREATE_FIRST_HDR));

    CheckMenuItem(hMenu,ID_CREATE_FIRST_FTR,TerMenuSelect(hTerWnd,ID_CREATE_FIRST_FTR));
    EnableMenuItem(hMenu,ID_CREATE_FIRST_FTR,TerMenuEnable(hTerWnd,ID_CREATE_FIRST_FTR));

    CheckMenuItem(hMenu,ID_DELETE_FIRST_HDR,TerMenuSelect(hTerWnd,ID_DELETE_FIRST_HDR));
    EnableMenuItem(hMenu,ID_DELETE_FIRST_HDR,TerMenuEnable(hTerWnd,ID_DELETE_FIRST_HDR));

    CheckMenuItem(hMenu,ID_DELETE_FIRST_FTR,TerMenuSelect(hTerWnd,ID_DELETE_FIRST_FTR));
    EnableMenuItem(hMenu,ID_DELETE_FIRST_FTR,TerMenuEnable(hTerWnd,ID_DELETE_FIRST_FTR));

    CheckMenuItem(hMenu,ID_EDIT_INPUT_FIELD,TerMenuSelect(hTerWnd,ID_EDIT_INPUT_FIELD));
    EnableMenuItem(hMenu,ID_EDIT_INPUT_FIELD,TerMenuEnable(hTerWnd,ID_EDIT_INPUT_FIELD));

    return TRUE;
}

