/*==============================================================================
   TER1 Editor Developer's Kit
   Window Painting routines.
===============================================================================*/

#define  PREFIX extern
#include "ter_hdr.h"

/******************************************************************************
    RepaintTer:
    This routine is called when a paint message is received.
    This routine repaint the entire client area.
*******************************************************************************/
RepaintTer(PTERWND w)
{
    PAINTSTRUCT PaintData;

    dm("RepaintTer");

    BeginPaint(hTerWnd,&PaintData);
    ClipRect=PaintData.rcPaint;
    OnWmPaint=TRUE;

    EndPaint(hTerWnd,&PaintData);

    RepaintRuler=TRUE;     // repaint ruler unconditionally
    
    return TerRepaint(hTerWnd,FALSE);
}

/******************************************************************************
    TerRepaintArea:
    This routine refreshes a given area on the window.
*******************************************************************************/
BOOL WINAPI _export TerRepaintArea(HWND hWnd,LPRECT rect)
{
    PTERWND w;
    
    dm("TerRepaintArea");

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    ClipRect=(*rect);
    OnWmPaint=TRUE;
    RepaintRuler=TRUE;     // repaint ruler unconditionally
    RedrawBorder=TRUE;

    return TerRepaint(hTerWnd,FALSE);
}

/******************************************************************************
    TerInvalidateRect:
    This function wraps InvalidateRect, since OCX wrapper looses fails to
    clear the background even when InvalidateRect is called with Clearbackground
    set to TRUE;
*******************************************************************************/
TerInvalidateRect(PTERWND w, LPRECT rect, BOOL ClearBackground)
{
    dm("TerInvalidateRect");
    
    if (ClearBackground) RedrawBorder=TRUE;
    InvalidateRect(hTerWnd,rect,ClearBackground);
    TerOpFlags|=TOFLAG_PAINT_WAITING;
    return TRUE;
}

/******************************************************************************
    TerQuickRepaint:
    This routine repaints the ter window calling the PaintTer function
*******************************************************************************/
BOOL WINAPI _export TerQuickRepaint(HWND hWnd)
{
    PTERWND w;

    dm("TerQuickRepaint");

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    PaintTer(w);
    return TRUE;
}

/******************************************************************************
    TerRepaint:
    This routine redisplays every aspect of the control
*******************************************************************************/
BOOL WINAPI _export TerRepaint(HWND hWnd,BOOL ClearBackground)
{
    PTERWND w;
    int PrevWinWidth,PrevWinHeight;
    BOOL BackgroundCleared;
    long SaveRepageBeginLine;

    dm("TerRepaint");

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    TerOpFlags|=TOFLAG_FULL_PAINT;

    //if (ClearBackground) TerEraseBackground(w,hTerDC);
    BackgroundCleared=OnWmPaint;

    if (hBkPictBM) DeleteObject(hBkPictBM);  // delete the background picture bitmap
    hBkPictBM=NULL;

    if (FontsReleased) RecreateFonts(w,hTerDC);  // recreate fonts if necessary

    if (OnWmPaint) {
       int AdjX=-TerWinOrgX+TerWinRect.left;  // to convert to phisical unit
       int AdjY=-TerWinOrgY+TerWinRect.top;  // to convert to phisical unit
       ClipRect.left+=AdjX;
       ClipRect.right+=AdjX;
       ClipRect.top+=AdjY;
       ClipRect.bottom+=AdjY;
    }

    PrevWinWidth=TerWinWidth;
    PrevWinHeight=TerWinHeight;

    GetWinDimension(w);           // get window dimensions

    if (ClearBackground || RedrawBorder) TerDrawBorder(w);

    if (HtmlMode && TerWinWidth!=PrevWinWidth) {
       if (TotalCells>1) TerAdjustHtmlTable(hTerWnd);
       AdjustHtmlRulerWidth(w);
       if (TotalFonts>1) AdjustHtmlPictWidth(w);
       if (TotalCells>1) RequestPagination(w,TRUE);
    }

    if (CurRow>=WinHeight) {     // forrmalize the row and begin line
       CurRow=WinHeight-1;
       BeginLine=CurLine-CurRow;
    }

    // set the clipping region when acting on WM_PAINT message
    if (OnWmPaint) {
       if (ClipRect.left<TerWinRect.left) ClipRect.left=TerWinRect.left;
       if (ClipRect.right>TerWinRect.right) ClipRect.right=TerWinRect.right;
       if (ClipRect.left>ClipRect.right) ClipRect.left=ClipRect.right;
       if (ClipRect.top<TerWinRect.top) ClipRect.top=TerWinRect.top;
       if (ClipRect.bottom>TerWinRect.bottom) ClipRect.bottom=TerWinRect.bottom;
       if (ClipRect.top>ClipRect.bottom) ClipRect.top=ClipRect.bottom;

       if (!InPrintPreview) CurClipRgn=CreateRectRgn(ClipRect.left,ClipRect.top,ClipRect.right,ClipRect.bottom);
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    SaveRepageBeginLine=RepageBeginLine;

    if (ClearBackground) {
       RepaintRuler=TRUE;     // repaint ruler unconditionally
       RulerPending=true;
    }
      
    PaintTer(w);
    
    if (HtmlMode && TotalCells>1) RepageBeginLine=SaveRepageBeginLine;

    if (TerArg.ShowStatus) DisplayStatus(w);          // PaintFlag the status line

    if (ClearBackground || TerWinWidth!=PrevWinWidth || BackgroundCleared) {
       if (hToolBarWnd) InvalidateRect(hToolBarWnd,NULL,TRUE);   // show the tool bar
       if (hPvToolBarWnd) InvalidateRect(hPvToolBarWnd,NULL,TRUE); // show the tool bar
    }

    if (HtmlMode && (TerWinWidth!=PrevWinWidth || TerWinHeight!=PrevWinHeight))
       SendMessageToParent(w,TER_HTML_SIZE,(WPARAM)hTerWnd,0L,FALSE);

    ResetLongFlag(TerOpFlags,(TOFLAG_FULL_PAINT|TOFLAG_PAINT_WAITING));

    return TRUE;
}

/******************************************************************************
    GetWinDimension:
    The the following dimensions of the current window:
       WinWidth in number of character columns.
       WinHeight in number of character rows.
*******************************************************************************/
void GetWinDimension(PTERWND w)
{
    int RulerHeight,DefFontHeight,DefFontWidth;
    int PrevTerWinWidth=TerWinWidth;
    int ThumbHt,BarHt;

    dm("GetWinDimension");

    GetClientRect(hTerWnd,(LPRECT) &TerRect);

    DefFontHeight=TerTextMet.tmHeight+TerTextMet.tmExternalLeading;   // default font height
    if (DefFontHeight<=0) DefFontHeight=1;     // to avoid division by zero
    DefFontWidth=TerTextMet.tmAveCharWidth;
    if (DefFontWidth<=0) DefFontWidth=1;

    // set the ruler area
    if (TerArg.ruler) {
       RulerRect.top=TerRect.top+ToolBarHeight;
       RulerRect.bottom=RulerRect.top+2*RulerFontHeight;
       RulerHeight=RulerRect.bottom-RulerRect.top;
    }
    else RulerHeight=0;

    // find the window dimensions
    if (TerTextMet.tmAveCharWidth>0) WinWidth=(TerRect.right-TerRect.left)/DefFontWidth-1;
    WinHeight=((TerRect.bottom-TerRect.top-RulerHeight)/DefFontHeight);

    //if (TerArg.ShowStatus) WinHeight--; // last line may be used for showing the status line
    if (WinHeight==0) WinHeight=1;      // minimum window height
    if (WinWidth<=0)  WinWidth=1;       // minimum window width

    if (TerArg.BorderMargins && ScrResX>0) { // display margin around text area
       TerWinRect.left=TerRect.left+TwipsToOrigScrX(BORDER_MARGIN);
       TerWinRect.right=TerRect.right-TwipsToOrigScrX(BORDER_MARGIN);
       TerWinRect.top=TerRect.top+RulerHeight+ToolBarHeight+TwipsToOrigScrY(BORDER_MARGIN);
       TerWinRect.bottom=TerRect.bottom-TwipsToOrigScrY(BORDER_MARGIN);
    }
    else {                              // leave no room for the margin area
       TerWinRect=TerRect;
       TerWinRect.top+=(RulerHeight+ToolBarHeight); // leave room for the ruler and tool bar
    }

    StatusBarHeight=(TerArg.ShowStatus)?(StatusFontHeight+6*StatusLineWidth):0;  // leave room for the status line
    TerWinRect.bottom-=StatusBarHeight;  // leave room for the status line

    // set other ruler rectangle dimension
    //RulerRect.left=TerWinRect.left;
    //RulerRect.right=TerWinRect.right;
    RulerRect.left=TerRect.left;
    RulerRect.right=TerRect.right;

    TerWinWidth=TerWinRect.right-TerWinRect.left/*-1*/;     //window width in units
    TerWinHeight=TerWinRect.bottom-TerWinRect.top;

    // set the minimum scrollbar thumb size
    ThumbHt=GetSystemMetrics(SM_CYVTHUMB);
    BarHt=TerRect.bottom-TerRect.top-2*GetSystemMetrics(SM_CYVSCROLL);
    if (ThumbHt>(BarHt/2)) ThumbHt=BarHt/2;
    MinThumbHt=MulDiv(ThumbHt,SCROLL_RANGE,BarHt);

    // size the tool bars
    if (hToolBarWnd) MoveWindow(hToolBarWnd,TerRect.left,0,TerRect.right-TerRect.left,ToolBarHeight,FALSE);
    if (hPvToolBarWnd) MoveWindow(hPvToolBarWnd,TerRect.left,0,TerRect.right-TerRect.left,ToolBarHeight,TerWinWidth!=PrevTerWinWidth);

    // set the width/height of the default frame
    frame[0].width=frame[0].ScrWidth=9999;                         // a high value
    frame[0].height=frame[0].ScrHeight=TerWinHeight;

    ResetBufBM(w);           // create/destory the buffered DC

    SetViewportOrgEx(hTerDC,TerWinRect.left,TerWinRect.top,NULL); // set view port origins
    SetWindowOrgEx(hTerDC,TerWinOrgX,TerWinOrgY,NULL); // set the logical window origin
    if (hBufDC) {
       SetViewportOrgEx(hBufDC,TerWinRect.left,TerWinRect.top,NULL); // set view port origins
       SetWindowOrgEx(hBufDC,TerWinOrgX,TerWinOrgY,NULL); // set the logical window origin
    }

    // set the dimension of the background bitmap
    if (BkPictId && BkPictFlag==BKPICT_STRETCH) SetPictSize(w,BkPictId,TerWinHeight,TerWinWidth,TRUE);

    if (TerArg.FittedView && TerWinWidth!=PrevTerWinWidth && PrevTerWinWidth>0) RequestPagination(w,TRUE);  // force repagination

}


/******************************************************************************
    ResetBufBM:
    Create/destroy the buffer DC/bitmap
*******************************************************************************/
ResetBufBM(PTERWND w)
{
    COLORREF SaveBkColor;

    dm("ResetBufBM");

    if (!(TerFlags&TFLAG_BUF_DISP) && hBufDC) {   // destroy the device context
       SelectObject(hBufDC,GetStockObject(SYSTEM_FONT));
       SelectPalette(hBufDC,GetStockObject(DEFAULT_PALETTE),TRUE);
       if (hOrigBufBM) SelectObject(hBufDC,hOrigBufBM);
       DeleteDC(hBufDC);
       DeleteObject(hBufBM);
       hBufDC=NULL;
       hBufBM=hOrigBufBM=NULL;
    }
    if (!(TerFlags&TFLAG_BUF_DISP)) return TRUE;

    // create the buffer device context if it does not exist
    if (!hBufDC ) {
      if (NULL==(hBufDC=CreateCompatibleDC(hTerDC))) return FALSE;
      hOrigBufBM=NULL;
    }

    // delete the existing bitmap
    if (hBufBM) {
       if (hOrigBufBM) SelectObject(hBufDC,hOrigBufBM);
       DeleteObject(hBufBM);
       hBufBM=NULL;
    }

    // create the bitmap and select it into the device context
    if (NULL==(hBufBM=CreateCompatibleBitmap(hTerDC,TerRect.right-TerRect.left+1,TerRect.bottom-TerRect.top+1))) return FALSE;
    hOrigBufBM=SelectObject(hBufDC,hBufBM);

    // clear the bitmap area
    SaveBkColor=SetBkColor(hBufDC,PageColor(w));
    OurExtTextOut(w,hBufDC,0,0,ETO_OPAQUE,&TerRect,NULL,0,NULL);
    SetBkColor(hBufDC,SaveBkColor);
    
    return TRUE;
}

/******************************************************************************
    PaintTer:
    Main TER window paint routine
*******************************************************************************/
PaintTer(PTERWND w)
{

    int temp,WrapLines,SavePage;
    MSG msg;
    int i,SavePaintFlag;
    HDC  hDC;

    dm("PaintTer");

    hDC=(hBufDC)?hBufDC:hTerDC; // decide the device context to write to

    if (!PaintEnabled || TerFlags4&TFLAG4_NO_REPAGINATE) return TRUE;           // painting disabled

    if (InPrintPreview) {                // in print preview mode
       PaintPrintPreview(w);
       return TRUE;
    }

    // turn-off highlighting if it goes beyond
    if (HilightBegRow>=TotalLines || HilightEndRow>=TotalLines) HilightType=HILIGHT_OFF;

    if (FontsReleased) RecreateFonts(w,hTerDC);  // recreate fonts

    // set background image if pending
    if (hInitBkBM) {
       int pict=TerPastePicture(hTerWnd,CF_BITMAP,(HGLOBAL)hInitBkBM,0,ALIGN_BOT,FALSE);
       DeleteObject(hInitBkBM);
       hInitBkBM=NULL;
       if (pict) TerSetBkPictId(hTerWnd,pict,BKPICT_STRETCH,FALSE);
       DeleteTextMap(w, TRUE);           // build the background bitmap
    }

    // increase the scope of painting for page break line
    if (PaintFlag==PAINT_MIN) {
       if (!PageBreakShowing && LineInfo(w,CurLine,INFO_PAGE)) PaintFlag=PAINT_WIN;
       if (PageBreakShowing && !LineInfo(w,CurLine,INFO_PAGE)) PaintFlag=PAINT_WIN;
    }

    // redraw any picture background
    if (PaintFlag==PAINT_WIN_RESET) DeleteTextMap(w,TRUE);  // reset the text segments
    else if (BkPictId && PaintFlag==PAINT_WIN && !UseTextMap) DeleteTextMap(w,FALSE);  // delete text map and redraw the background
    if (PaintFlag==PAINT_WIN_RESET) PaintFlag=PAINT_WIN;

    // adjust header/footer if necessary
    if (PosPageHdrFtr) {
       PaintEnabled=FALSE;
       ReposPageHdrFtr(w,FALSE);
       PaintEnabled=TRUE;
    }

    // adjust column number for the hidden text
    AdjustHiddenPos(w);

    PAINT_BEGIN:
    SavePaintFlag=PaintFlag;

    WrapLines=WinHeight+10;
    if (TerArg.PageMode && (CurFrame=GetFrame(w,CurLine))>=0) {
       WrapLines=WinHeight+20;  // wrap additinal lines to take care of smaller lines
       BeginLine=UnitsToLine(w,frame[CurFrame].x,TerWinOrgY);
       if (CurLine<BeginLine) {
          BeginLine=PageInfo[CurPage].FirstLine;
          if (CurLine<BeginLine) BeginLine=CurLine;
       }
       if (frame[CurFrame].flags&FRAME_ON_SCREEN && frame[CurFrame].ScrFirstLine<BeginLine && frame[CurFrame].ScrFirstLine>=0) 
          BeginLine=frame[CurFrame].ScrFirstLine;
       if ((PfmtId[pfmt(BeginLine)].flags&PAGE_HDR_FTR)!=(PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR)) {
          BeginLine=CurLine;
          if (!(PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR) && BeginLine>=PageInfo[CurPage].FirstLine && BeginLine<=PageInfo[CurPage].FirstLine) BeginLine=PageInfo[CurPage].FirstLine;
       }
       if (WrapFlag==WRAP_PAGE) {
          BeginLine=PageInfo[CurPage].FirstLine;
          if (CurPage==(TotalPages-1)) WrapLines=(int)(TotalLines-BeginLine);
          else                         WrapLines=(int)(PageInfo[CurPage+1].FirstLine-BeginLine);
       }
    }

    // wrap the current screen
    if (TerArg.WordWrap && WrapFlag!=WRAP_OFF && !(TerOpFlags2&TOFLAG2_NO_WRAP)) {
       // do word wrapping only on typing pause
       if (CommandId>0 || !PeekMessage(&msg,hTerWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE|PM_NOYIELD)) WordWrap(w,BeginLine,WrapLines);
       else WrapPending=TRUE;                // postpone wrapping operation
    }

    // delete any scroll bitmap
    if (hScrollBM) PaintFlag=PAINT_WIN;      // repaint the whole window to do the real painting
    if (hScrollBM) DeleteObject(hScrollBM);  // delete the temporary bitmap used for scrolling
    hScrollBM=NULL;

    //****************** redraw the screen or a part of it *****************
    if (CurLine>=TotalLines) CurLine=TotalLines-1;   // fix any out of bound values before calling the HorScrollCheck function
    if (TerArg.WordWrap && CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
    if (CurCol<0) CurCol=0;
    HorScrollCheck(w);      // check if horizontal scrolling is required

    // calculate current page number
    SavePage=CurPage;
    CurPage=GetCurPage(w,CurLine);
    if (CurPage!=SavePage) {
       WrapFlag=WRAP_WIN;
       PaintFlag=PAINT_WIN;
    }

    // adjust current text column
    if (CurCol<0) CurCol=0;              // provide any corrections to current col
    if (CurCol>=LineWidth) CurCol=LineWidth-1;

    // adjust current row
    if (!TerArg.PageMode) {
       if (CurRow>=WinHeight && !DocFitsInWindow(w)) {  // adjust the current row - don't adjust when partial window may being painted
          BeginLine=CurLine-(WinHeight-1);
          if (BeginLine<0) BeginLine=0;
          PaintFlag=PAINT_WIN;             // repaint the window
       }
       if (BeginLine<0) BeginLine=0;
       CurRow=CurLine-BeginLine;

       if (CurRow<0)  {                    // adjust the current row
          BeginLine=CurLine;
          CurRow=0;
          PaintFlag=PAINT_WIN;             // repaint the window
       }
    }

    // check if first line offset being disabled
    if (BeginLine!=WinYOffsetLine) WinYOffsetLine=-1;
    if (WinYOffsetLine==-1) WinYOffset=0;

    // adjust highlighting
    if (HilightType!=HILIGHT_OFF && StretchHilight) {
       if (!TblSelCursShowing || !InSameTable(w,cid(HilightBegRow),cid(HilightEndRow))) {
          SyncHilight(w);
       }
       if (PaintFlag!=PAINT_WIN) PaintFlag=PAINT_WIN;
    }
    if (HilightType!=HILIGHT_OFF && !StretchHilight && HilightBegRow==HilightEndRow && HilightBegCol==HilightEndCol) HilightType=HILIGHT_OFF;

    // set the default frame
    if (!TerArg.PageMode) {
       CurFrame=0;
       frame[CurFrame].ScrFirstLine=BeginLine;
       frame[CurFrame].PageFirstLine=0;
       frame[CurFrame].PageLastLine=TotalLines-1;
       frame[CurFrame].level=0;
    }

    // erase and delete the drag objects
    if (PictureHilighted) {
        PaintFlag=PAINT_WIN;
        DrawDragPictRect(w);       // erase the picture rectangle
    }
    if (FrameTabsHilighted) {
        PaintFlag=PAINT_WIN;
        DrawDragFrameTabs(w);     // erase the frame size tabs
    }
    DeleteDragObjects(w,DRAG_TYPE_SCR_FIRST,DRAG_TYPE_SCR_LAST);

    // set the AutoSpell variables
    if (PaintFlag<PAINT_WIN && SpellPending && !WordBeingEdited(w)) PaintFlag=PAINT_WIN;   // need to spell-check the entire window
    if (PaintFlag>=PAINT_WIN) SpellPending=FALSE;  // this flag will be set by the AutoSpellCheck function is spelling could not be completed because a word is being edited

    // reset the 'drawn' flag on the ocx objects
    if (PaintFlag>=PAINT_WIN) {
       for (i=0;i<TotalFonts;i++)
         if (TerFont[i].InUse && TerFont[i].style&PICT) ResetUintFlag(TerFont[i].flags,(FFLAG_PICT_DRAWN|FFLAG_CTL_DRAWN|FFLAG_DRAW_PENDING));
    }


    // Paint the screen
    RepaintNo++;                  // incremental number of this repaint
    DirtyRect.left=-1;
    if (PaintFlag>=PAINT_WIN) PageBreakShowing=false;
                 
    ClearScrForXparentWrite(w,hDC);  // clear screen for xparent write

    
    // clear the text area
    DrawBknd=TRUE;
    if (PaintFlag==PAINT_WIN && BkPictId==0 && hDC==hBufDC) {
       RECT rect;
       COLORREF SaveBkColor;
       SetRect(&rect,TerWinOrgX,TerWinOrgY,TerWinOrgX+TerWinWidth,TerWinOrgY+TerWinHeight);
       SaveBkColor=SetBkColor(hDC,PageColor(w));
       OurExtTextOut(w,hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
       SetBkColor(hDC,SaveBkColor);
       DrawBknd=FALSE;                  // background already drawn
    }

    // draw text
    if (TerArg.PageMode) {
        if (PaintFlag==PAINT_LINE || PaintFlag==PAINT_RANGE) {
            BOOL PaintFull=(/*fid(CurLine) ||*/ (BOOL)(TerFlags2&TFLAG2_FULL_REPAINT));
            if (!PaintFull) {           // check if the line height has changed
                int SpcBef,SpcAft;
                int DispHeight=GetRowHeight(w,CurLine);
                GetLineSpacing(w,CurLine,DispHeight,&SpcBef, &SpcAft, TRUE);
                DispHeight+=(SpcBef+SpcAft);
                if (ScrLineHeight(w,CurLine,FALSE,FALSE)!=DispHeight) PaintFull=TRUE;
            }
            if (PaintFull) {
                 PaintFlag=PAINT_WIN;
                 WrapFlag=WRAP_WIN;      // to allow recreation of frames in PaintFrames
                 ClearScrForXparentWrite(w,hDC);  // clear screen because we turned on PAINT_WIN 
                 
                 PaintFrames(w,hDC,-1);
            }
            else PaintFrames(w,hDC,CurLine);
        }
        else if (PaintFlag==PAINT_PARTIAL_WIN || PaintFlag==PAINT_WIN)  PaintFrames(w,hDC,-1);
    }
    else {
        if (CaretEnabled && !CaretHidden) HideCaret(hTerWnd);// hide caret before clipping
        TerSetClipRgn(w);                    // create clipping region

        if (PaintFlag==PAINT_LINE) {
            if (GetLineHeight(w,CurLine,&temp,NULL)!=GetRowHeight(w,CurLine)) PaintFlag=PAINT_PARTIAL_WIN;
            else PaintRows(w,hDC,CurRow,CurRow);    // paint only the current row
        }
        if (PaintFlag==PAINT_PARTIAL_WIN)  PaintRows(w,hDC,CurRow-1,9999);    // paint enough rows to fill the screen
        else if (PaintFlag==PAINT_WIN)     PaintRows(w,hDC,0,9999);           // paint enough rows to fill the screen
    }
    DrawBknd=TRUE;                          // reset this variable

    // delete the visible ocx not used (drawn) on  the current screen
    if (PaintFlag>=PAINT_WIN) {
       BOOL hidden=FALSE;
       for (i=0;i<TotalFonts;i++) {
         if (!TerFont[i].InUse || !(TerFont[i].style&PICT) || TerFont[i].flags&(FFLAG_CTL_DRAWN|FFLAG_DRAW_PENDING)) continue;
         if (!(TerFont[i].flags&FFLAG_CTL_VISIBLE)) continue;
         if (HideControl(w,i)) hidden=TRUE;
       }
       if (hidden) {
         ValidateRect(hTerWnd,NULL);
         TerSetClipRgn(w);               // restore clip region reset by ValidateRect
       }
    }

    if (WheelShowing) DrawWheel(w,hDC);

    // do bitblast when using buffered display
    if (hBufDC && !disable) {
       if (PageColor(w)!=CLR_WHITE && !pTerPal) TerSetPalette(w,hDC,NULL,0,NULL,FALSE);    // add bk color to the palette
       if (hTerPal)  {
          SelectPalette(hTerDC,hTerPal,FALSE);   // select a foreground palette
          RealizePalette(hTerDC);
       }
       if (PaintFlag==PAINT_LINE || PaintFlag==PAINT_RANGE)
                                      BitBlt(hTerDC,DirtyRect.left,DirtyRect.top,DirtyRect.right-DirtyRect.left,DirtyRect.bottom-DirtyRect.top,hBufDC,DirtyRect.left,DirtyRect.top,SRCCOPY);
       else if (PaintFlag!=PAINT_MIN) BitBlt(hTerDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight,hBufDC,TerWinOrgX,TerWinOrgY,SRCCOPY);
       
       // show the controls
       PosControls(w);
    }
    else if (disable && False(TerArg.style&TER_DESIGN_MODE)) {
       RECT rect;
       HFONT hPrevFont;
       COLORREF SaveBkColor;
       LPBYTE msg="EVALUATION EXPIRED!  TO PURCHASE, PLEASE VISIT:  WWW.SUBSYSTEMS.COM";
       hPrevFont=SelectObject(hTerDC,TerFont[0].hFont);
       SetRect(&rect,TerWinOrgX,TerWinOrgY,TerWinOrgX+TerWinWidth,TerWinOrgY+TerWinHeight);
       SaveBkColor=SetBkColor(hTerDC,PageColor(w));
       OurExtTextOut(w,hTerDC,TerWinOrgX,TerWinOrgY+TerWinHeight/2,ETO_OPAQUE,&rect,msg,strlen(msg),NULL);
       SetBkColor(hTerDC,SaveBkColor);
       SelectObject(hTerDC,hPrevFont);
    } 

    //************* update the status lines *********************************
    DisplayStatusInfo(w);

    // reset clip region and redisplay the caret
    TerResetClipRgn(w);                   // reset clip before show caret
    if (CaretEnabled && !CaretHidden) ShowCaret(hTerWnd); // show caret after clip reset

    //********** set ruler, caret, scroll bar positions *****************
    if (RulerPending || RulerSection!=GetSection(w,CurLine) || RulerLineX!=LineX(CurLine) || SavePaintFlag==PAINT_WIN) DrawRuler(w,TRUE);  // redraw the ruler
    if (TerArg.ShowHorBar || TerArg.ShowVerBar) SetScrollBars(w);
    OurSetCaretPos(w);

    // recreate non-text area if overwritten
    if (BorderSpill) RedrawNonText(w);

    if (hTerPal && hBufDC) {         // restore the default palette
       SelectPalette(hTerDC,GetStockObject(DEFAULT_PALETTE),FALSE);
       RealizePalette(hTerDC);
    }

    PaintFlag=PAINT_WIN;                     // reset the paint flag
    WrapFlag=WRAP_WIN;                       // reset to full window wrapping
    UseTextMap=TRUE;                         // set the use of text map
    PrevCursLine=CurLine;                    // record current line number
    PrevCursCol=CurCol;                      // record previous column number
    PrevCursLineY=CurLineY+TerWinOrgY;       // record previous Y postion
    PrevCursPage=CurPage;                    // record previous page number
    CursDirection=CURS_RESET;                // reset cursor direction
    if (CurClipRgn) DeleteObject(CurClipRgn);// delete any clipping region
    CurClipRgn=NULL;
    BorderSpill=0;                           // border spill
    if (!(TerFlags5&TFLAG5_GROUP_UNDO)) UndoRef++; // undo reference number
    FirstPalette=TRUE;                       // first palette to be realized
    PageBoxShowing=FALSE;                    // page box erased if any
    OnWmPaint=FALSE;                         // painting for the paint message done
    MouseStopMsgOn=false;                    // mouse-stop message erased
    if (HilightType!=HILIGHT_CHAR || !StretchHilight) HilightAtCurPos=FALSE;

    if (!TerArg.PageMode && CurLine>frame[0].ScrLastLine && BeginLine<CurLine) {
        BeginLine++;
        goto PAINT_BEGIN; // repaint until current line is within the window
    }

    // Auto vertical scroll bar?
    if (True(TerFlags&TFLAG_AUTO_VSCROLL_BAR)) {
       bool enable=false;
       bool PrevEnabled=TerArg.ShowVerBar;

       if (!TerArg.WordWrap || !TerArg.PageMode) {
          enable=(BeginLine!=0 || (frame[0].ScrLastLine+1)!=TotalLines);
          EnableVScrollBar(w,enable);
       }
       if (TerArg.PageMode) {
          enable=(TerWinOrgY>0 || TotalPages>1 || SumPageScrHeight(w,0,TotalPages)>TerWinHeight);
          EnableVScrollBar(w,enable);
       } 
       else if (TerArg.FittedView) {
          enable=(TerWinOrgY>0 || TotalPages>1 || CurTextHeight>TerWinHeight);
          EnableVScrollBar(w,enable);
       }
      if (enable && !PrevEnabled && TerArg.ShowVerBar) SetScrollBars(w);
    }

    return TRUE;
}

/******************************************************************************
    PaintFrames:
    Create and paint each text frame on the window.  When PaintLine is -1,
    all frames are painted, otherwise only one line (PaintLine) is painted.
*******************************************************************************/
void PaintFrames(PTERWND w,HDC hDC, long PaintLine)
{
    int  pict,frm,pass,i,idx,RowOffset=0,CurHeight,SaveCurFrame,BorderWidthY,
         BorderWidthX,PassCount,LastFrameY;
    long line,LastVisibleLine;
    int  y,ParaFID;
    BOOL SnapToNextLine=FALSE,CurLineAtTop=FALSE;
    int  BorderX1,BorderX2,BorderY1,BorderY2;
    BOOL OnNonTextFrame,TopMost,rotated;
    UINT SaveBorder;
    int  SavePaintFlag=PaintFlag;

    dm("PaintFrames");

    // Create frames
    if (PaintFlag!=PAINT_LINE && PaintFlag!=PAINT_RANGE) {
       DeleteDragObjects(w,DRAG_TYPE_CELL,DRAG_TYPE_CELL);            // delete old cell drag objects
       DeleteDragObjects(w,DRAG_TYPE_ROW,DRAG_TYPE_ROW);              // delete old cell drag objects
       DeleteDragObjects(w,DRAG_TYPE_ROW_INDENT,DRAG_TYPE_ROW_INDENT);// delete old cell drag objects
       if (WrapFlag!=WRAP_MIN) {
          BOOL ForceRefresh=!(CommandId==ID_PGUP || CommandId==ID_PGDN || CommandId==ID_UP 
                              || CommandId==ID_DOWN || MessageId==WM_HSCROLL || MessageId==WM_VSCROLL 
                              || MessageId==WM_CHAR);
          RefreshFrames(w, ForceRefresh);
       }
    }

    // adjust x orig when the window size might have changed
    if (OnWmPaint) {
       int FrameWidth=PageTextWidth(w);
       if ((TerWinOrgX+TerWinWidth)>FrameWidth) {
           TerWinOrgX=FrameWidth-TerWinWidth;
           if (TerWinOrgX<0) TerWinOrgX=0;
           SetWindowOrgEx(hTerDC,TerWinOrgX,TerWinOrgY,NULL);
           SetWindowOrgEx(hDC,TerWinOrgX,TerWinOrgY,NULL);
       }
    }

    // Get the frame where the current line lies
    line=CurLine;    // line for the frame
    if (line>0 && LineFlags(line)&LFLAG_SECT && CurPage>FirstFramePage && line==PageInfo[CurPage].FirstLine) line--;  // compensate for page creep when typing the section break line
    if ((CurFrame=GetFrame(w,line))<0) {
       for (;line<=PageInfo[CurPage].LastLine;line++) if ((CurFrame=GetFrame(w,line))>=0) break;
       if (line>PageInfo[CurPage].LastLine) return;
       CurLine=line;
    }

    // Check if the current line lies in the visible area
    ParaFID=fid(CurLine);
    OnNonTextFrame=(ParaFID>0 && ParaFrame[ParaFID].flags&FRAME_NON_TEXT_DO);
    if (!OnNonTextFrame && True(LineFlags(CurLine)&LFLAG_PICT)) {
       int font=GetCurCfmt(w,CurLine,CurCol);
       if (TerFont[font].ParaFID>0) OnNonTextFrame=true;
    }
     
    y=frame[CurFrame].y+frame[CurFrame].SpaceTop;
    LastFrameY=frame[CurFrame].y+frame[CurFrame].height-frame[CurFrame].SpaceTop;


    LastVisibleLine=-1;
    for (line=frame[CurFrame].PageFirstLine;line<=CurLine;line++) {
       if (TableLevel(w,line)!=frame[CurFrame].level) continue;

       CurHeight=ScrLineHeight(w,line,TRUE,FALSE);
       if ((y+CurHeight/4)>=LastFrameY && LastVisibleLine<0 && line>frame[CurFrame].PageFirstLine) LastVisibleLine=line-1;

       if (line>=CurLine) break;
       y+=CurHeight;
    }

    if (y>(TerWinOrgY+TerWinHeight-ScrLineHeight(w,CurLine,TRUE,FALSE))) SnapToNextLine=TRUE;
    if (y<TerWinOrgY) CurLineAtTop=TRUE;   // current line should be at the top of the window

    y+=ScrLineHeight(w,line,TRUE,FALSE);   // position after the current line


    if (TerFlags3&TFLAG3_CURSOR_IN_CELL && CurLine>LastVisibleLine && LastVisibleLine>=0) {
       CurLine=LastVisibleLine;
       CurCol=LineLen(CurLine)-1;
       if (CurCol<0) CurCol=0;
    }
    else if (CaretEngaged && !OnNonTextFrame && !hScrollBM 
            && !InDragDrop && (y<=TerWinOrgY || y>(TerWinOrgY+TerWinHeight)) ) 
          SetYOrigin(w,CurLine);
    else  CurLineY=y-TerWinOrgY;


    // lock the scroll if specified
    if (TerFlags3&TFLAG3_NO_SCROLL && TerWinOrgY!=0) {
       TerWinOrgY=0;
       SetTerWindowOrg(w);
    }

    // Align the current frame at the top of the window
    if ((CurFrame=GetFrame(w,CurLine))<0) return;
    if (WinYOffsetLine<0 && CurLineAtTop && CaretEngaged && !OnNonTextFrame && !hScrollBM 
        && !(TerFlags3&TFLAG3_NO_SCROLL) && !InDragDrop) {  // if alignment not disabled
       if ((frame[CurFrame].y+frame[CurFrame].SpaceTop)<TerWinOrgY && (frame[CurFrame].y+frame[CurFrame].height)>TerWinOrgY) {
          y=frame[CurFrame].y+frame[CurFrame].SpaceTop;
          for (line=frame[CurFrame].PageFirstLine;line<CurLine;line++) { // find the line not aligned on the top
             if (TableLevel(w,line)!=frame[CurFrame].level) continue;
             
             CurHeight=ScrLineHeight(w,line,TRUE,FALSE);

             // snap to the nearest line
             if (!SnapToNextLine && (y+CurHeight/2)>TerWinOrgY ) break; // snap to the nearest line
             if (y>TerWinOrgY) break;    // snap to this line

             y+=CurHeight;
          }

          if (y!=TerWinOrgY) {            // This line not aligned
             CurLineY=CurLineY-(y-TerWinOrgY);
             TerWinOrgY=y;
             SetTerWindowOrg(w);
             if (BkPictId>0) DeleteTextMap(w,TRUE); // delete text map and redraw the background
          }
       }
    }

    // clear screen if necessary
    if (PaintFlag==PAINT_WIN && PaintFlag!=SavePaintFlag) {
          ClearScrForXparentWrite(w,hDC);  // clear screen because we turned on PAINT_WIN 
    } 

    SaveCurFrame=CurFrame;             // save current frame no

    // hide caret and set clipping region
    if (CaretEnabled && !CaretHidden) HideCaret(hTerWnd);// hide caret before clipping
    TerSetClipRgn(w);                    // create clipping region

    // Display each frame
    BeginLine=-1;           // reset begining/ending lines
    WinHeight=0;

    // draw any watermark frames
    if (WmParaFID>0 && PaintFlag>=PAINT_WIN) {
      for (i=0;i<TotalFrames;i++) {
         if (False(frame[i].flags&FRAME_WATERMARK)) continue;

         if (frame[i].ScrX>=(TerWinOrgX+TerWinWidth)) continue;
         if ((frame[i].ScrX+frame[i].ScrWidth)<=TerWinOrgX) continue;
         if (frame[i].y>=(TerWinOrgY+TerWinHeight)) continue;
         if ((frame[i].ScrY+frame[i].ScrHeight)<=TerWinOrgY) continue;

         ParaFID=frame[i].ParaFrameId;
         pict=ParaFrame[ParaFID].pict;
         if (pict>0) PictOut(w,hDC,frame[i].ScrX,frame[i].ScrY,pict,1);
      }
    }

    // paint in passes, pass 0 = stationary frames, pass 1 = moveable frames
    // pass 2 = current moveable frame, pass3 = any line/rect object frame
    PassCount=(FramesSorted?1:5);

    for (pass=0;pass<PassCount;pass++) {
       for (frm=0;frm<TotalFrames;frm++) {
          i=frame[frm].DispFrame;        // actual index in Zorder

          if (True(frame[i].flags&FRAME_WATERMARK)) continue;

          ParaFID=frame[i].ParaFrameId;


          // pass filter
          if (!FramesSorted) {
             if (pass==0 && ParaFID!=0) continue;
             if (pass>0 && ParaFID==0 && !(frame[i].flags&(FRAME_BORDER|FRAME_PAGE_BOX))) continue;
             if (  pass==1 && !frame[i].empty
                && CurLine>=frame[i].PageFirstLine && CurLine<=frame[i].PageLastLine) continue;
             if ( pass>1 && frame[i].empty && frame[i].BoxFrame==i && ParaFID>0) continue;  // box frame displayed in the first pass only
             if (  pass==2 && !frame[i].empty
                && (CurLine<frame[i].PageFirstLine || CurLine>frame[i].PageLastLine)) continue;
             TopMost=(BOOL)(ParaFrame[ParaFID].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT));
             if (TopMost && ParaFrame[ParaFID].flags&PARA_FRAME_RECT && (ParaFrame[ParaFID].FillPattern>0 || ParaFrame[ParaFID].FillPict>0)) TopMost=FALSE;
             if (pass!=3 && TopMost) continue;
             if (pass==3 && !TopMost) continue;
             if (pass!=4 && (frame[i].flags&(FRAME_BORDER|FRAME_PAGE_BOX))) continue;
             if (pass==4 && !(frame[i].flags&(FRAME_BORDER|FRAME_PAGE_BOX))) continue;
          }

          // set the current frame
          CurFrame=i;                                  // current frame number
          EnableTransparent=(frame[CurFrame].flags&FRAME_TRANSPARENT || TerFlags2&TFLAG2_XPARENT_WRITE);

          // Initialize the screen variables
          frame[i].ScrX=frame[i].x;
          frame[i].ScrY=frame[i].y+frame[i].SpaceTop;  // add the space for the borders
          frame[i].ScrWidth=frame[i].width;
          frame[i].ScrHeight=frame[i].height-frame[i].SpaceTop;
          if (frame[i].LimitY!=-1 && frame[i].height>(frame[i].LimitY-frame[i].y)) frame[i].ScrHeight=frame[i].LimitY-frame[i].ScrY;

          frame[i].ScrFirstLine=-1L;
          if (PaintFlag!=PAINT_LINE && PaintFlag!=PAINT_RANGE) frame[i].ScrLastLine=-1L;

          frame[i].flags=ResetUintFlag(frame[i].flags,FRAME_ON_SCREEN);// turnoff the on screen flag
          SaveBorder=frame[i].border;                  // save the border value

          // Is the frame rotated
          rotated=false;
          if (frame[i].ParaFrameId>0) rotated=(ParaFrame[frame[i].ParaFrameId].TextAngle>0);  // rotated frame
          else if (frame[i].CellId>0) rotated=(cell[frame[i].CellId].TextAngle>0);  // rotated frame


          // skip hidden frames
          if (frame[i].ScrHeight<=0 && !(frame[i].flags&(FRAME_NON_TEXT_DO|FRAME_BORDER_BOT)) && !rotated) continue;
          if (frame[i].ScrX>=(TerWinOrgX+TerWinWidth)) continue;
          if ((frame[i].ScrX+frame[i].ScrWidth)<=TerWinOrgX && !rotated) continue;
          if (frame[i].y>=(TerWinOrgY+TerWinHeight)) continue;
          if (frame[i].LimitY!=-1 && frame[i].y>frame[i].LimitY) continue;
          if ((frame[i].ScrY+frame[i].ScrHeight)<=TerWinOrgY && !rotated) continue;
          if (frame[i].CellId>0 && cell[frame[i].CellId].flags&CFLAG_ROW_SPANNED && !(frame[i].flags&FRAME_TOP_SPAN)) continue;

          frame[i].flags|=FRAME_ON_SCREEN;     // this frame displayed

          // display the non-text drawing objects
          if (!frame[i].empty && frame[i].flags&FRAME_NON_TEXT_DO) {
              frame[i].ScrFirstLine=frame[i].ScrLastLine=frame[i].PageFirstLine;

              if (PaintFlag!=PAINT_LINE && PaintFlag!=PAINT_RANGE) {
                 frame[i].ScrFirstLine=frame[i].ScrLastLine=frame[i].PageFirstLine;
                 frame[i].RowOffset=RowOffset;
                 RowX[RowOffset]=0;
                 RowY[RowOffset]=frame[i].ScrY;
                 RowHeight[RowOffset]=0;
                 RowOffset=RowOffset+2;     // beginning for the next frame
                 if (RowOffset>=MAX_LINES_PER_WIN) AbortTer(w,MsgString[MSG_MAX_WIN_LINES],1);
              }
              if (PaintFlag!=PAINT_LINE) {
                if      (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) DrawLineObject(w,hDC,i);
                else if (ParaFrame[ParaFID].flags&PARA_FRAME_RECT) DrawRectObject(w,hDC,i);
              }
              continue;
          }


          // display the picture frame
          if (!frame[i].empty && frame[i].flags&FRAME_PICT) {
             DrawPictFrame(w,hDC,i);
             continue;
          }

          // display the footnote frame
          if (frame[i].flags&FRAME_FNOTE) {
             DrawFootnote(w,hDC,i,TRUE);
             continue;
          }


          // set the frame shading and background color
          FrameShading=frame[i].shading;
          FrameBkColor=frame[i].BackColor;
          PaintFrameFlags=frame[i].flags;
          if (True(ParaFrame[ParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_CONV)) && (FrameBkColor==CLR_WHITE || frame[i].CellId==0)) {
             if (ParaFrame[ParaFID].FillPattern>0) FrameBkColor=ParaFrame[ParaFID].BackColor;
             FrameShading=-1;
          }

          // display the top space
          BorderWidthY=BorderWidthX=0;
          if (frame[i].border&BORDER_TOP)  BorderWidthY=TwipsToScrY(frame[i].BorderWidth[BORDER_INDEX_TOP]);
          if (frame[i].border&BORDER_LEFT) BorderWidthX=TwipsToScrY(frame[i].BorderWidth[BORDER_INDEX_LEFT]);
          if (FrameToDraw(w,i,PaintLine)
              && (frame[i].SpaceTop-BorderWidthY)>0
              && frame[i].ScrY>TerWinOrgY
              && frame[i].y<(TerWinOrgY+TerWinHeight)) {
             RECT rect;
             SetRect(&rect,frame[i].ScrX+BorderWidthX,frame[i].y+BorderWidthY,
                     frame[i].ScrX+frame[i].ScrWidth-BorderWidthX+1,frame[i].ScrY);
             SetColor(w,hDC,'T',DEFAULT_CFMT);
             
             if (BkPictId) transparent=TRUE;
             TerTextOut(w,hDC,rect.left,rect.top,&rect,NULL,NULL,0,NULL,0,0);
          }

          // initialize the screen lines
          frame[i].ScrFirstLine=frame[i].PageFirstLine;
          if (PaintFlag!=PAINT_LINE && PaintFlag!=PAINT_RANGE)
             frame[i].ScrLastLine=frame[i].PageLastLine;

          // save the border coordinates
          BorderX1=frame[i].ScrX;
          BorderX2=frame[i].ScrX+frame[i].ScrWidth;
          BorderY1=frame[i].y;
          BorderY2=BorderY1+frame[i].height;
          if (frame[i].empty && frame[i].CellId>0 && cell[frame[i].CellId].NextCell==-1 && frame[i].flags&FRAME_LAST_ROW_FRAME) {
             if (frame[i].flags&FRAME_RTL) BorderX1++;
             else                          BorderX2--;
          }

          // adjust the frame beginning position (text frames)
          if (!frame[i].empty && frame[i].ScrY<TerWinOrgY && !rotated) {
              y=frame[i].ScrY;
              line=frame[i].ScrFirstLine;
              while (TRUE) {
                 if (TableLevel(w,line)!=frame[i].level) CurHeight=0;  // these lines not printed in this frame
                 else  CurHeight=ScrLineHeight(w,line,TRUE,FALSE);
                 if ((y+CurHeight)>TerWinOrgY) break;
                 if (line==frame[i].ScrLastLine) break;
                 y+=CurHeight;
                 line++;
              }

              frame[i].ScrHeight=frame[i].height-(y-frame[i].ScrY);
              frame[i].ScrY=y;              // start painting from this line onward
              frame[i].ScrFirstLine=line;
              if (BeginLine<0) BeginLine=line;
              // do not draw the top border line
              frame[i].border=frame[i].border&(~((UINT)BORDER_TOP));   // bottom border can not be drawn
          }


          // Adjust frame ending position
          if ((frame[i].ScrY+frame[i].ScrHeight)>(TerWinOrgY+TerWinHeight) && !rotated) {
             frame[i].ScrHeight=TerWinOrgY+TerWinHeight-frame[i].ScrY;
             frame[i].border=frame[i].border&(~((UINT)BORDER_BOT));   // bottom border can not be drawn
          }

          // expand the rightmost frame toward right until the end of screen
          if (i<(TotalFrames-1) && frame[i].flags&FRAME_RIGHTMOST && !rotated && frame[i].ParaFrameId==0) {
             frame[i].ScrWidth=TerWinOrgX+TerWinWidth-frame[i].ScrX;
          }

          // expand the last frame to cover the ending spaces
          if (i==(TotalFrames-1) && !rotated) {
             frame[i].ScrWidth=TerWinOrgX+TerWinWidth-frame[i].ScrX;
             frame[i].ScrHeight=TerWinOrgY+TerWinHeight-frame[i].ScrY;
          }

          // get the section line number
          SectLine=GetSectDisplayLine(w,CurFrame);

          // Paint the frame
          if (PaintFlag==PAINT_LINE/* && pass==0*/) {// paint only the given line
             if (!frame[i].empty && TableLevel(w,PaintLine)==frame[i].level 
                 && PaintLine>=frame[i].ScrFirstLine && PaintLine<=frame[i].ScrLastLine) { // draw one line only
                 PaintRows(w,hDC,PaintLine-frame[i].ScrFirstLine,PaintLine-frame[i].ScrFirstLine); // draw one line
                 break;
             }
          }
          else if (PaintFlag==PAINT_RANGE) {// paint the range of lines
             for (PaintLine=PaintBegLine;PaintLine<=PaintEndLine;PaintLine++) {
                if (!frame[i].empty && TableLevel(w,PaintLine)==frame[i].level 
                    && PaintLine>=frame[i].ScrFirstLine && PaintLine<=frame[i].ScrLastLine) { // draw one line only
                    PaintRows(w,hDC,PaintLine-frame[i].ScrFirstLine,PaintLine-frame[i].ScrFirstLine); // draw one line
                }
             }
             if (!frame[i].empty && PaintEndLine>=frame[i].ScrFirstLine && PaintEndLine<=frame[i].ScrLastLine) break;
          }
          else if (PaintFlag!=PAINT_LINE && PaintFlag!=PAINT_RANGE) {// paint the whole screen
             frame[CurFrame].RowOffset=RowOffset;
               
             if (!(frame[i].flags&FRAME_PAGE_BOX)) PaintRows(w,hDC,0,9999);      // draw the whole frame

             // insert the frame borders into the drag object table
             if (frame[CurFrame].CellId>0 && (HilightType==HILIGHT_OFF || !StretchHilight) 
                 && !(frame[CurFrame].flags&(FRAME_FIRST_ROW_FRAME|FRAME_DISABLED)) && !(TerFlags3&TFLAG3_NO_EDIT_TABLE_COL)) {
                int RowId=frame[CurFrame].RowId;
                BOOL IsRtl=frame[CurFrame].flags&FRAME_RTL;
                if (False(TerFlags5&TFLAG5_NO_DRAG_CELL_LINE)) {
                   if (TableRow[RowId].FirstCell==frame[CurFrame].CellId) {  // first cell of the row
                      CreateCellDragObj(w,DRAG_TYPE_ROW_INDENT,frame[i].RowId,IsRtl?BorderX2:BorderX1,BorderY1,BorderY2);  // create the left edge drag object
                   }
                   // create the right edge drag object
                   CreateCellDragObj(w,DRAG_TYPE_CELL,frame[i].CellId,IsRtl?BorderX1:BorderX2,BorderY1,BorderY2);
                }

                // create the bottom edge drag object
                if (False(TerFlags5&TFLAG5_NO_DRAG_ROW_LINE)) CreateRowDragObj(w,DRAG_TYPE_ROW,frame[i].CellId,IsRtl?BorderX2:BorderX1,IsRtl?BorderX1:BorderX2,BorderY2);
             }

             // Adjust frame values
             if (!frame[i].empty) {
                //update offset into row position table
                idx=(int)(frame[i].ScrLastLine-frame[i].ScrFirstLine+1);
                RowOffset=RowOffset+idx+1; // beginning for the next frame
                if (RowOffset>=MAX_LINES_PER_WIN) AbortTer(w,MsgString[MSG_MAX_WIN_LINES],1);
             }

             // draw the text area delimiting lines
             if (frame[i].flags&FRAME_DIVIDER_LINE && !TerArg.FittedView && !(TerFlags2&TFLAG2_HIDE_PAGE_BREAK)) {  // draw a divider line
                int LineYPos=frame[i].y+frame[i].height-2;
                if (LineYPos>=TerWinOrgY && LineYPos<(TerWinOrgY+TerWinHeight))
                   if (i==(TotalFrames-1) && (LastFramePage+1)<TotalPages) {
                       if (!BorderShowing) WriteBreakLine(w,hDC,MsgString[MSG_SOFT_PAGE_BREAK],TerWinOrgX,LineYPos,TerWinWidth,0);
                   }
                   else {
                       if (i==(FirstPage2Frame-1)) {
                          if (frame[i].height>0 && !BorderShowing) DrawDottedLine(w,hDC,frame[i].x,LineYPos,frame[i].x+frame[i].width,LineYPos,0); // page break line
                       }
                       else if (EditPageHdrFtr)    DrawLightDottedLine(w,hDC,frame[i].x,LineYPos,frame[i].x+frame[i].width,LineYPos);     // header/body/footer separation lines
                   }
             }

             // draw the page border lines
             if (frame[i].flags&FRAME_BORDER)   DrawPageBorder(w,hDC,i);
          }
          
          if (frame[i].flags&FRAME_PAGE_BOX) DrawPageBorderBox(w,hDC,i);

          frame[i].border=SaveBorder;   // restore the original border value

          // calculate number of lines in the window
          if (!frame[i].empty) WinHeight+=(int)(frame[i].ScrLastLine-frame[i].ScrFirstLine+1);
       }
    }

    CurFrame=SaveCurFrame;               // restore current frame number
    if (BeginLine<0) BeginLine=CurLine;  // first line of the screen
    EnableTransparent=FALSE;             // reset the transparent flag
    
}

/******************************************************************************
    FrameToDraw:
    Returns TRUE if this frame is being redrawn.
*******************************************************************************/
BOOL FrameToDraw(PTERWND w,int FrameNo, long PaintLine)
{
    dm("FrameToDraw");

    if (PaintFlag==PAINT_LINE) {// paint only the given line
        if (!frame[FrameNo].empty && TableLevel(w,PaintLine)==frame[FrameNo].level 
            && PaintLine>=frame[FrameNo].ScrFirstLine && PaintLine<=frame[FrameNo].ScrLastLine) { // draw one line only
            return TRUE;
        }
    }
    else if (PaintFlag==PAINT_RANGE) {// paint the range of lines
        for (PaintLine=PaintBegLine;PaintLine<=PaintEndLine;PaintLine++) {
            if (!frame[FrameNo].empty && TableLevel(w,PaintLine)==frame[FrameNo].level 
                && PaintLine>=frame[FrameNo].ScrFirstLine && PaintLine<=frame[FrameNo].ScrLastLine) { // draw one line only
                return TRUE;
            }
        }
    }
    else return TRUE;

    return FALSE;
}

/******************************************************************************
    ClearScrForXparentWrite:
    Returns TRUE if this frame is being redrawn.
*******************************************************************************/
ClearScrForXparentWrite(PTERWND w, HDC hDC)
{    
    if (TerFlags2&TFLAG2_XPARENT_WRITE && (PaintFlag==PAINT_WIN || PaintFlag==PAINT_WIN_RESET)) {
        CurFrame=0;
        ClearEOL(w,hDC,TerWinOrgX,TerWinOrgY,TerWinOrgX+TerWinWidth,TerWinOrgY+TerWinHeight,TRUE); // clear to the end of line
    }

    return TRUE;
}

/******************************************************************************
    PaintRows:
    Paint rows for text between FirstRow and LastRow.
*******************************************************************************/
void PaintRows(PTERWND w,HDC hDC, long FirstRow,long LastRow)
{
    long temp1;
    int i,j,k,lLen,RowUnits,temp,RowOffset;
    int BaseHeight,AdjWidth,CurPara;
    int HiBegCol,HiEndCol,BlockBegCol,BlockEndCol;
    int FrameBeginX,FrameBeginY,FrameLastX,FrameLastY;
    int ParaBeginX,SubTableLevel,BulletWidth;
    long FrameLastLine;
    long BlockBegRow,BlockEndRow;
    long  LineNo;
    LPBYTE ptr;
    RECT rect;
    UINT ParaFlags;
    UINT border;
    int  BorderX1,BorderX2,BorderY1,BorderY2,LineSpcBef,level;
    int  SaveHilightType,SaveWinHeight,HilightFirstCell=-1,HilightLastCell=-1;
    BOOL ShowingLastPartialLine=FALSE;
    UINT SaveParaShading;
    COLORREF SaveParaBackColor;
    BYTE area,CurBreakChar;
    BOOL TblHilighted=FALSE,ParaBkSet=FALSE;
    BOOL WithinOneCell=FALSE,AltBreakSym;
    BOOL DrawFullLine;

    dm("PaintRows");

    SaveWinHeight=WinHeight;       // save the original value of winheight

    // reset the highlighting when only a picture is clicked
    SaveHilightType=HilightType;
    if (HilightType==HILIGHT_CHAR && PictureClicked
       && HilightBegRow==HilightEndRow && HilightBegCol==(HilightEndCol-1))
       HilightType=HILIGHT_OFF;

    if (FirstRow<0) FirstRow=0;

    // extract the frame parameters
    FrameBeginX=frame[CurFrame].ScrX;
    FrameBeginY=TsFrameY=frame[CurFrame].ScrY;
    FrameLastX=FrameBeginX+frame[CurFrame].ScrWidth;
    FrameLastY=FrameBeginY+frame[CurFrame].ScrHeight;
    FrameLastLine=frame[CurFrame].PageLastLine;
    if (FrameLastLine<TotalLines && LineInfo(w,FrameLastLine,INFO_ROW) && FrameLastLine>frame[CurFrame].PageFirstLine) FrameLastLine--;
    FrameCellId=frame[CurFrame].CellId;
    FrameRowId=frame[CurFrame].RowId;
    FrameHeight=frame[CurFrame].height;
    ScrFrameAngle=0;
    SubTableLevel=frame[CurFrame].level;
    ParaBeginX=FrameBeginX;
    EnableTransparent=frame[CurFrame].flags&FRAME_TRANSPARENT;
    if (BkPictId) EnableTransparent=TRUE;      // write transparently over the background picture
    if (TerFlags2&TFLAG2_XPARENT_WRITE) EnableTransparent=TRUE;      // write transparently over the background picture

    FullCellHilight=FALSE;                     // set to TRUE when the full cell needs to be hilighted

    RowOffset=frame[CurFrame].RowOffset;       // offset into row position table
    if (PaintFlag!=PAINT_LINE) {
       RowY[RowOffset]=FrameBeginY;               // Start the first row at the top of the frame
       if (!TerArg.PageMode) RowY[RowOffset]-=WinYOffset;  // apply win offset adjustment
    }

    // skip zero width/height frames
    if (frame[CurFrame].ScrWidth==0 || frame[CurFrame].ScrHeight==0) {
       BOOL skip=TRUE;
       if (frame[CurFrame].PageFirstLine==FrameLastLine && FrameLastLine>=0 && FrameLastLine<TotalLines && PfmtId[pfmt(FrameLastLine)].flags&PAGE_HDR_FTR) skip=FALSE;  // draw lone hdr/ftr line any way for proper cursor positioning
       if (skip) goto DRAW_BORDERS;
    }

    // normalize highlighted block
    if (HilightType!=HILIGHT_OFF) {            // normalize the highlihted block

       BlockBegRow=HilightBegRow;              // move block co-ordinates to temp variables
       BlockEndRow=HilightEndRow;
       BlockBegCol=HilightBegCol;
       BlockEndCol=HilightEndCol;
       AdjustBlockForTable(w,&BlockBegRow,&BlockBegCol,&BlockEndRow,&BlockEndCol,FALSE);


       if (HilightType==HILIGHT_CHAR) {

          if (BlockBegRow>BlockEndRow) {
             temp1=BlockEndRow;BlockEndRow=BlockBegRow;BlockBegRow=temp1;
             temp=BlockEndCol;BlockEndCol=BlockBegCol;BlockBegCol=temp;
          }
          if (BlockBegRow==BlockEndRow && BlockBegCol>BlockEndCol) {
             temp=BlockEndCol;BlockEndCol=BlockBegCol;BlockBegCol=temp;
          }
       }
       else {
          if (BlockBegRow>BlockEndRow) {temp1=BlockEndRow;BlockEndRow=BlockBegRow;BlockBegRow=temp1;}
          if (BlockBegCol>BlockEndCol) {temp=BlockEndCol;BlockEndCol=BlockBegCol;BlockBegCol=temp;}
       }

       // test if a range of cells selected
       level=MinTableLevel(w,BlockBegRow,BlockEndRow);
       HilightFirstCell=HilightLastCell=LevelCell(w,level,BlockBegRow);
       HilightLastCell=LevelCell(w,level,BlockEndRow);

       TblHilighted=TableHilighted(w);
       if (HilightFirstCell==HilightLastCell && HilightFirstCell>0) WithinOneCell=TRUE;
    }

    // Draw an empty frame
    if (frame[CurFrame].empty) {
       COLORREF SaveBkColor=FrameBkColor;
       BOOL  SetPageBorderColor=FALSE;
       int CellId=frame[CurFrame].CellId;
       long FirstLine=cell[CellId].FirstLine;

       if (frame[CurFrame].flags&FRAME_BORDER && (!BkPictId || !(TerFlags2&TFLAG2_BKPICT_OVER_PAGE_BORDER))) SetPageBorderColor=TRUE;
       if (CellId>0 && frame[CurFrame].flags1&FRAME1_PART_EMPTY_CELL && HilightType!=HILIGHT_OFF && !WithinOneCell
           && FirstLine>=HilightBegRow && FirstLine<=HilightEndRow && LineSelected(w,FirstLine)) FullCellHilight=TRUE;

       if    (SetPageBorderColor) FrameBkColor=PageBorderColor;
       else  SetColor(w,hDC,'T',DEFAULT_CFMT);

       //if (!(TerFlags2&TFLAG2_XPARENT_WRITE) || (frame[CurFrame].flags&FRAME_BORDER))
          ClearEOL(w,hDC,FrameBeginX,RowY[RowOffset],FrameLastX,FrameLastY-RowY[RowOffset],!EnableTransparent); // clear to the end of line

       if (SetPageBorderColor) FrameBkColor=SaveBkColor;
       FullCellHilight=FALSE;

       goto DRAW_BORDERS;
    }

    // Enable text rotation and rotate DC if supported
    if (frame[CurFrame].ParaFrameId>0) ScrFrameAngle=ParaFrame[frame[CurFrame].ParaFrameId].TextAngle;
    else if (frame[CurFrame].CellId>0) ScrFrameAngle=cell[frame[CurFrame].CellId].TextAngle;
    if (ScrFrameAngle>0) {
       if (OSCanRotate) FrameRotateDC(w,hDC,CurFrame);
       SwapInts(&FrameLastX,&FrameLastY);
    }


    // draw the lines
    for (i=(int)FirstRow;i<=(int)LastRow;i++) {
        LineNo=frame[CurFrame].ScrFirstLine+i;

        if (TerArg.PageMode && LineNo<TotalLines && TableLevel(w,LineNo)!=SubTableLevel) {
            RowY[RowOffset+i+1]=RowY[RowOffset+i];
            RowHeight[RowOffset+i]=0;
            if (FALSE && LineNo==frame[CurFrame].PageLastLine) break;  // don't fill the remaining frame space with blanks, because it is occupied by subtables
            continue;  // does not belong to this frame
        }

        TextBorder=BoxLeft=BoxRight=ParaShading=SaveParaShading=0;  // reset any text borders
        ParaBoxColor=CLR_AUTO;
        SpaceBef=SpaceAft=ScrYOffset=0;
        ParaSpcBef=ParaSpcAft=0;
        CharBoxLeft=CharBoxRight=bullet=ShadeBegin=ShadeEnd=FALSE;
        ExpandLineFmt=FALSE;
        ScrRtl=((LineNo>=0 && LineNo<TotalLines && LineFlags2(LineNo)&LFLAG2_RTL));
        TotalScrSeg=CurScrSeg=0;

        DrawFullLine=(ScrRtl || ScrFrameAngle>0);

        if (LineNo<TotalLines) CurPfmt=pfmt(LineNo);
        else                   CurPfmt=0;
        TabFlags=0;
        CurBreakChar=0;
        BulletWidth=0;
        ParaBackColor=SaveParaBackColor=CLR_WHITE;
        ParaBoxSpace=PARA_BOX_SPACE;

        //**************** check if the line is elligible to display *******
        if (LineNo>FrameLastLine || LineNo>=TotalLines) {     // past the end of the file
           if (!TerArg.PageMode) WinHeight=i;
           if (RowY[RowOffset+i]<FrameLastY && ScrFrameAngle==0) { // clean up the partially displayed row
              int AdjY=0;
              if (FrameLastY==(TerWinOrgY+TerWinHeight)) AdjY=1;
              //if (!(TerFlags2&TFLAG2_XPARENT_WRITE)) 
              ClearEOL(w,hDC,FrameBeginX,RowY[RowOffset+i],FrameLastX,FrameLastY-RowY[RowOffset+i]+AdjY,!EnableTransparent); // clear to the end of line
              if (!TerArg.PageMode && WinHeight<SaveWinHeight) WinHeight=SaveWinHeight;  // additional space available
           }
           break;
        }

        if (LineNo<TotalLines) CurPara=pfmt(LineNo);
        else                   CurPara=DEFAULT_PFMT;

        // update date/time
        if (!TerArg.PrintView && LineInfo(w,LineNo,INFO_DYN_FIELD)) UpdateDynField(w,LineNo,0);   // in PrintView mode, the fields gets refreshed during frame creation

        if (LineNo<TotalLines) CompressCfmt(w,LineNo);    // compress the character formatting information

        RowHeight[RowOffset+i]=GetLineHeight(w,LineNo,&BaseHeight,NULL); // height of this row
        if (TerArg.PageMode && LineHt(LineNo)==0) RowHeight[RowOffset+i]=0;
        DispTextHt=RowHeight[RowOffset+i];                // text height of the line being displayed


        //************ get the starting X position of the this row
        if (TerArg.WordWrap) {
           BOOL IsRightJust=false;

           RowX[RowOffset+i]=ParaBeginX=frame[CurFrame].SpaceLeft+PfmtId[CurPara].LeftIndent; // apply left indentation
           if (LineFlags(LineNo)&LFLAG_PARA_FIRST) 
              RowX[RowOffset+i]+=GetFirstIndent( w,LineNo,&BulletWidth,&bullet,TRUE);

           if (PfmtId[CurPara].FirstIndent<0) ParaBeginX+=PfmtId[CurPara].FirstIndent;
           ParaBeginX+=FrameBeginX;

           // Set paragraph border flags
           ParaFlags=PfmtId[CurPara].flags;
           if (ParaFlags&PARA_BOX && fid(LineNo)==0) SetParaBorder(w,LineNo);  // frame do their own borders
           BoxLeft=FrameBeginX+frame[CurFrame].SpaceLeft+PfmtId[CurPara].LeftIndent-TwipsToScrX(PARA_BORDER_MARGIN);
           if (PfmtId[CurPara].FirstIndent<0) BoxLeft+=PfmtId[CurPara].FirstIndent;
           BoxRight=FrameBeginX+frame[CurFrame].SpaceLeft+TerWrapWidth(w,LineNo,-1)-PfmtId[CurPara].RightIndent+TwipsToScrX(PARA_BORDER_MARGIN);
           if (LineFlags(LineNo)&LFLAG_SHADE_BEGIN) ShadeBegin=TRUE;
           if (LineFlags(LineNo)&LFLAG_SHADE_END) ShadeEnd=TRUE;

           ParaBoxSpace=PfmtId[CurPara].BorderSpace;
           ParaBoxColor=PfmtId[CurPara].BorderColor;

           // Set the cell border flag
           if (!TerArg.PageMode && tabw(LineNo) && tabw(LineNo)->type&INFO_CELL) TextBorder|=BORDER_BOT;

           // set the paragraph shading
           SaveParaShading=PfmtId[CurPara].shading;
           SaveParaBackColor=PfmtId[CurPara].BkColor;
           ParaBkSet=(SaveParaShading>0 || SaveParaBackColor!=CLR_WHITE)?TRUE:FALSE;

           // apply line centering and right justification
           IsRightJust=false;
           if (ScrRtl) {
              if (PfmtId[CurPara].flow==FLOW_LTR) {  // ScrRtl lines are usually in rtl para, if not reverse the alignemnt
                if (False(ParaFlags&(CENTER|RIGHT_JUSTIFY|JUSTIFY))) IsRightJust=true; 
              }
              else if (True(ParaFlags&RIGHT_JUSTIFY)) IsRightJust=true; 
           }
           else if (True(ParaFlags&RIGHT_JUSTIFY)) IsRightJust=true; 

           if (True(ParaFlags&CENTER) || IsRightJust) {
              int adjust=(!LineInfo(w,LineNo,INFO_FRAME)) && !(LineFlags(LineNo)&LFLAG_ASSUMED_TAB);
              int FrameWidth=0;
              if (!adjust && tabw(LineNo) && tabw(LineNo)->FrameCharPos==0) {
                  adjust=TRUE;
                  FrameWidth=tabw(LineNo)->FrameScrWidth;
              }
              if (adjust) {
                 AdjWidth=TerWrapWidth(w,LineNo,-1)
                         -(RowX[RowOffset+i]-frame[CurFrame].SpaceLeft)
                         -PfmtId[CurPara].RightIndent
                         -GetLineWidth(w,LineNo,FALSE,TRUE)
                         -FrameWidth;
                 
                 if (AdjWidth>0) {
                    if ((PfmtId[CurPara].flags)&CENTER)
                        RowX[RowOffset+i]=RowX[RowOffset+i]+AdjWidth/2;
                    else RowX[RowOffset+i]=RowX[RowOffset+i]+AdjWidth;
                 }
              }
           }

           // apply assumed tab spacing
           if (TerArg.PageMode && LineFlags(LineNo)&LFLAG_ASSUMED_TAB 
               && tabw(LineNo) && tabw(LineNo)->count>0) {
               RowX[RowOffset+i]+=PrtToScrX(tabw(LineNo)->width[0]);
           } 
        
        }
        else RowX[RowOffset+i]=0;


        // check if any frame space needs to be written
        LineSpcBef=GetObjSpcBef(w,LineNo,TRUE);
        if (LineSpcBef>0 /*&& PaintFlag!=PAINT_LINE*/) {
           int SaveTextBorder=TextBorder;
           TextBorder=0;
           SetColor(w,hDC,'T',DEFAULT_CFMT);
           ClearEOL(w,hDC,FrameBeginX,RowY[RowOffset+i],FrameLastX,LineSpcBef,!EnableTransparent); // clear to the end of line
           if (PaintFlag!=PAINT_LINE) RowY[RowOffset+i]+=LineSpcBef;
           TextBorder=SaveTextBorder;
        }

        //***********  get the Y position of the next row ********
        GetLineSpacing2(w,LineNo,RowHeight[RowOffset+i],&SpaceBef, &SpaceAft, &ParaSpcBef, &ParaSpcAft, TRUE);
        
        RowY[RowOffset+i+1]=RowY[RowOffset+i]+RowHeight[RowOffset+i];  // Y coordinate of the next row
        RowY[RowOffset+i+1]+=(SpaceBef+SpaceAft);

        if (TerArg.PageMode) {   // derive from printer height in page mode
           RowY[RowOffset+i+1]=RowY[RowOffset+i]+ScrLineHeight(w,LineNo,FALSE,FALSE);
           ScrYOffset=RowY[RowOffset+i+1]-RowHeight[RowOffset+i]-SpaceBef-SpaceAft-RowY[RowOffset+i];
           if (ScrYOffset<-1) ScrYOffset=-1; // to keep the accent on the characters from getting chopped off
           RowHeight[RowOffset+i]=RowY[RowOffset+i+1]-RowY[RowOffset+i]-SpaceBef-SpaceAft;
        }

        // Stop if writing beyond the frame
        if (RowY[RowOffset+i+1]>/*FrameLastY*/(TerWinOrgY+TerWinHeight) && i!=0 && ScrFrameAngle==0) {// no more lines can be displayed
           if ((FrameLastY-RowY[RowOffset+i])>WIN_BOT_MARGIN && !ShowingLastPartialLine) ShowingLastPartialLine=TRUE;         // show a partial line if more than 1/4 inch left
           else {
              if (!TerArg.PageMode) WinHeight=i+1;
              if (RowY[RowOffset+i]<FrameLastY) {  // clean up the partially displayed row
                 TextBorder=SpaceBef=SpaceAft=0;        // reset any text border
                 ClearEOL(w,hDC,FrameBeginX,RowY[RowOffset+i],FrameLastX,FrameLastY-RowY[RowOffset+i],!EnableTransparent); // clear to the end of line
              }
              break;
           }
        }

        // Stop if writing beyond frame limitation
        if (TerArg.PageMode && frame[CurFrame].LimitY!=-1 && (RowY[RowOffset+i+1]-SpaceAft)>FrameLastY && ScrFrameAngle==0 && i>0) {// no more lines can be displayed
           if (RowY[RowOffset+i]<FrameLastY) {  // clean up the partially displayed row
              TextBorder=SpaceBef=SpaceAft=0;        // reset any text border
              ClearEOL(w,hDC,FrameBeginX,RowY[RowOffset+i],FrameLastX,FrameLastY-RowY[RowOffset+i],!EnableTransparent); // clear to the end of line
           }
           break;
        }

        // clear the entire row when writing transparently
        if (TerFlags2&TFLAG2_XPARENT_WRITE && PaintFlag!=PAINT_WIN) {
           ClearEOL(w,hDC,FrameBeginX,RowY[RowOffset+i],FrameLastX,RowY[RowOffset+i+1]-RowY[RowOffset+i],TRUE); // clear to the end of line
        }


        RowUnits=RowY[RowOffset+i];     // convert rows to logical units

        if (PaintFlag!=PAINT_LINE && PaintFlag!=PAINT_RANGE)
           frame[CurFrame].ScrLastLine=LineNo;
        if (CurLine==LineNo) CurLineY=RowUnits-TerWinOrgY;

        // Check soft page break at this line
        if (TerArg.WordWrap && TerArg.PrintView && !TerArg.PageMode) {
           for (j=0;j<TotalPages;j++) if ((PageInfo[j].FirstLine-1)==LineNo && !(PageInfo[j].flags&PAGE_HARD)) break;
           if (j<TotalPages)  {
             if (!tabw(LineNo) || !(tabw(LineNo)->type&(INFO_PAGE|INFO_SECT))) TextBorder=BORDER_BOT;       // draw a line to show soft page break
           }
        }

        // *************** update the page number ***********
        if (tabw(LineNo) && tabw(LineNo)->type&INFO_DYN_FIELD) UpdateDynField(w,LineNo,(CurFrame>=FirstPage2Frame?FirstFramePage+1:FirstFramePage));


        // ************** auto spell checking *****************************
        if (DoAutoSpellCheck(w) && !(LineFlags2(LineNo)&LFLAG2_SPELL_CHECKED)) AutoSpellCheck(w,LineNo);

        //***************** lock data to display **************************
        ptr=pText(LineNo);
        lLen=LineLen(LineNo);
        if (TerArg.WordWrap && lLen>0 && ShowParaMark && LineFlags(LineNo)&LFLAG_SECT) {
           lLen--;
           CurBreakChar=ptr[lLen];    // show this break marker
        }

        while (lLen>0 && (!ShowParaMark || !LineFlags(LineNo)&LFLAG_SECT) && !ScrRtl) { // don't show ending spaces/tabs
           if (ptr[lLen-1]!=' ') break;
           lLen--;
        }


        // Non word-wrap mode, decompress formatting for lines containing tabs
        if (!TerArg.WordWrap && cfmt(LineNo).info.type==UNIFORM) {
           for (k=0;k<lLen;k++) if (ptr[k]==TAB) break;
           if (k!=lLen) ExpandCfmt(w,LineNo);    // tab found
        }

        if (cfmt(LineNo).info.type!=UNIFORM) TempCfmt=cfmt(LineNo).pFmt;
        if (TerArg.WordWrap && lLen>0 && !ShowParaMark && LineFlags(LineNo)&(LFLAG_PARA|LFLAG_LINE|LFLAG_SECT|LFLAG_ASSUMED_TAB)) ExpandLineFmt=TRUE;

        // exapnd fonts to display alternate break symbols
        AltBreakSym=TerFlags2&(TFLAG2_ALT_PARA_SYM|TFLAG2_ALT_LINE_SYM) || mbcs;
        if (AltBreakSym && TerArg.WordWrap && lLen>0 && ShowParaMark && LineFlags(LineNo)&(LFLAG_PARA|LFLAG_LINE|LFLAG_SECT)) {
           if (cfmt(LineNo).info.type!=UNIFORM) {  // make a copy
              for (k=0;k<lLen;k++) LineCfmt[k]=cfmt(LineNo).pFmt[k];
              TempCfmt=LineCfmt;
           }
           ExpandLineFmt=TRUE;     // use the copy or make a copy from the uniform font
        }
        if (ScrRtl) ExpandLineFmt=TRUE;

        if (ExpandLineFmt && cfmt(LineNo).info.type==UNIFORM) {  // expand the line formatting temporarily
           for (k=0;k<lLen;k++) LineCfmt[k]=cfmt(LineNo).info.fmt;
           TempCfmt=LineCfmt;
        }

        if (ExpandLineFmt || cfmt(LineNo).info.type!=UNIFORM) {  // copy the line text
           for (k=0;k<lLen;k++) {
              if      (ptr[k]==NBDASH_CHAR) LineText[k]='-';  // convert to regular dash for display only
              else if (ptr[k]==NBSPACE_CHAR) LineText[k]=' ';  // convert to regular space for display only
              else if (ptr[k]==HYPH_CHAR) {
                 if (ShowParaMark)     LineText[k]=DISP_HYPH_CHAR; // show special character for optional hyphen
                 else if (k==(lLen-1)) LineText[k]='-';           // last optional hyphen gets converted to regular dash
                 else                  LineText[k]=ptr[k];
              }
              else if (ptr[k]==HIDDEN_CHAR) {
                 if (ShowParaMark)     LineText[k]=DISP_HIDDEN_CHAR; // show special character for hidden character
                 else                  LineText[k]=ptr[k];
              }
              else LineText[k]=ptr[k];
           }
           ptr=LineText;      // point to the temporary buffer

           // get the standard font to display the break characters
           if (AltBreakSym && ShowParaMark && lLen>0 && (ptr[lLen-1]==ParaChar || ptr[lLen-1]==CellChar || ptr[lLen-1]==LINE_CHAR)) {
              int font=TempCfmt[lLen-1];
              LPBYTE pTypeface="Arial";
              int PointSize=TerFont[font].TwipsSize/20;

              if (ptr[lLen-1]==ParaChar && True(TerFlags2&TFLAG2_ALT_PARA_SYM)) pTypeface="Symbol";
              if (ptr[lLen-1]==LINE_CHAR && True(TerFlags2&TFLAG2_ALT_LINE_SYM)) {
                 pTypeface="Symbol";
                 if (PointSize>3) PointSize-=3;
              }

              TempCfmt[lLen-1]=GetNewFont(w,hDC,font,pTypeface,PointSize*20,0,TerFont[font].TextColor,TerFont[font].TextBkColor,TerFont[font].UlineColor,0,0,0,1,0,0,0,0,NULL,0,0,DEFAULT_CHARSET,0,TerFont[font].TextAngle);
           }
        }                                 

        // assign the lead pointer
        if (mbcs && pLead(LineNo) && LineLen(LineNo)>0) {
           TempLead=MemAlloc(LineLen(LineNo)*sizeof(BYTE));
           FarMove(pLead(LineNo),TempLead,LineLen(LineNo)*sizeof(BYTE));
           //TempLead=(mbcs?pLead(LineNo):NULL);
        }
        else TempLead=NULL;

        // get the character width array
        if (pLineCharWidth) MemFree(pLineCharWidth);
        pLineCharWidth=GetLineCharWidth(w,LineNo);
        
        if (pScrSeg) MemFree(pScrSeg);
        pScrSeg=NULL;
        if (ScrRtl) pScrSeg=GetLineSeg(w,LineNo,pLineCharWidth,&TotalScrSeg);

        //************* establish the highlighted and non-highlighted parts **
        HiBegCol=HiEndCol=-1;           // determine highlighted part of the line
        FullCellHilight=FALSE;
        if (HilightType!=HILIGHT_OFF && LineNo>=BlockBegRow
            && LineNo<=BlockEndRow && LineSelected(w,LineNo)) {
            if (HilightType==HILIGHT_LINE) {
               HiBegCol=0;HiEndCol=9999;    // highlighted the whole line
            }
            else if (HilightType==HILIGHT_CHAR) {
               if (LineNo>BlockBegRow && LineNo<BlockEndRow) {
                   HiBegCol=0;
                   HiEndCol=lLen;           // highlighted the entire text
               }
               else if (LineNo==BlockBegRow && LineNo==BlockEndRow) {    // first and last line of a character block
                   HiBegCol=BlockBegCol;
                   HiEndCol=BlockEndCol;
               }
               else if (LineNo==BlockBegRow) {    // first line of a character block
                   HiBegCol=BlockBegCol;
                   HiEndCol=lLen;                 // highlighted the entire text
               }
               else {                             // Last line of a character block
                   HiBegCol=0;
                   HiEndCol=BlockEndCol;
               }

               if (cid(LineNo)>0 && TblHilighted) FullCellHilight=TRUE;
               if (LineNo<=BlockEndRow && cid(LineNo)>0 && !WithinOneCell) FullCellHilight=TRUE;
            }
            if (HiEndCol<HiBegCol) HiEndCol=HiBegCol;

        }

        //******************* paint left indentation if any ******************
        NextX=FrameBeginX+RowX[RowOffset+i];      // X coordinate where the next character will be written
        if (RowX[RowOffset+i]>0) {                // display indentation
            rect.left=FrameBeginX;
            rect.top=RowY[RowOffset+i];
            if (ParaBkSet) rect.right=ParaBeginX-TwipsToScrX(PARA_BORDER_MARGIN);
            else           rect.right=NextX;
            rect.bottom=RowY[RowOffset+i+1];
            if (HilightType==HILIGHT_LINE && HiBegCol>=0) area='H';  // use highlight color
            else                                          area='T';
            SetColor(w,hDC,area,DEFAULT_CFMT);
            CurFmtId=0;
            WriteSpaceRect(w,hDC,&rect);
            //ClearEOL(w,hDC,rect.left,rect.top,rect.right,rect.bottom,!EnableTransparent); // clear to the end of line


            // draw beginning part of paragraph shading
            if (ParaBkSet && NextX>rect.right) {
               rect.left=rect.right;
               rect.right=NextX;
               ParaShading=SaveParaShading;
               ParaBackColor=SaveParaBackColor;
               SetColor(w,hDC,area,DEFAULT_CFMT);
               WriteSpaceRect(w,hDC,&rect);
            }
        }

        //******************* draw a bullet if specified for the para *********
        TerOpFlags2|=TOFLAG2_NO_COMPRESS_CFMT;
        if (bullet) DrawBullet(w,hDC,hDC,LineNo,CurPara,NextX-BulletWidth,RowUnits+SpaceBef,BaseHeight,TRUE);
        ResetUintFlag(TerOpFlags2,TOFLAG2_NO_COMPRESS_CFMT);

        NextTab=0;                                           // number of tabs processed
        NextSpace=0;                                         // number of spaces processed
        ParaShading=SaveParaShading;
        ParaBackColor=SaveParaBackColor;

        // draw the section line numbering
        if (SectLine>=0) DrawSectLineNbr(w,hDC,hDC,LineNo-frame[CurFrame].PageFirstLine,FrameBeginX+frame[CurFrame].SpaceLeft,RowUnits+SpaceBef,BaseHeight);

        // display the text line
        DisplayText(w,hDC,0,RowUnits,RowHeight[RowOffset+i],BaseHeight,ptr,lLen,LineNo,CurBreakChar==0 && !ParaBkSet,'T');

        // draw reviewed text indicator line
        if (True(LineFlags2(LineNo)&(LFLAG2_DEL_REV|LFLAG2_INS_REV))) DrawTrackingLine(w,hDC,hDC,FrameBeginX+frame[CurFrame].SpaceLeft,RowUnits,RowHeight[RowOffset+i]);

        // highlighting
        if (HiBegCol>=0 && HiEndCol>0 && !FullCellHilight) {
           int seg,SegCount;
           int SaveScrSeg=CurScrSeg;
           if (ScrRtl && pScrSeg) SegCount=TotalScrSeg;
           else                   SegCount=1;

           for (seg=0;seg<SegCount;seg++) {
              int col1,col2,x1,x2;
              if (ScrRtl && pScrSeg && SegCount>1) {
                 if (HiBegCol>=(pScrSeg[seg].col+pScrSeg[seg].count)) continue;
                 if (HiEndCol<=pScrSeg[seg].col) continue;

                 if (HiBegCol<=pScrSeg[seg].col) col1=pScrSeg[seg].col;
                 else                            col1=HiBegCol;

                 if (HiEndCol>=(pScrSeg[seg].col+pScrSeg[seg].count)) 
                       col2=pScrSeg[seg].col+pScrSeg[seg].count;
                 else  col2=HiEndCol;
                 col2--;
              }
              else {
                 col1=HiBegCol;
                 col2=HiEndCol-1;
              }  
              
              x1=ColToUnits(w,col1,LineNo,TER_LEFT);
              x2=ColToUnits(w,col2,LineNo,TER_RIGHT);
              
              if (ScrRtl && pScrSeg) CurScrSeg=seg;
              OurBitBlt(w,hDC,x1,RowUnits,x2-x1,RowHeight[RowOffset+i]+SpaceBef+SpaceAft,hDC,0,0,DSTINVERT);
           }
           CurScrSeg=SaveScrSeg;   // to make the following drawing work properly
        } 

        // **** paint any right indentation spaces
        if ((NextX<FrameLastX || DrawFullLine) && ParaBkSet) {  // right indentation already drawn for non-shaded lines
            if (NextX<BoxRight) {              // draw last part of shading
               SetColor(w,hDC,'T',DEFAULT_CFMT);
               ClearEOL(w,hDC,NextX,RowUnits,BoxRight,RowHeight[RowOffset+i]+SpaceBef+SpaceAft,FALSE); // clear to the end of line
               NextX=BoxRight;
            }
            ParaShading=0;                          // turnoff para shading
            ParaBackColor=CLR_WHITE;                // turnoff para bk color
            SetColor(w,hDC,'T',DEFAULT_CFMT);
            ClearEOL(w,hDC,NextX,RowUnits,FrameLastX,RowHeight[RowOffset+i]+SpaceBef+SpaceAft,FALSE); // clear to the end of line
        }
        else if ((NextX<FrameLastX || DrawFullLine) && CurBreakChar>0) {  // draw the break line
            int y=RowUnits+(RowHeight[RowOffset+i]+SpaceBef+SpaceAft)/2;
            ClearEOL(w,hDC,NextX,RowUnits,FrameLastX,RowHeight[RowOffset+i]+SpaceBef+SpaceAft,FALSE); // clear to the end of line
            if (CurBreakChar==SECT_CHAR) WriteBreakLine(w,hDC,MsgString[MSG_SECT_BREAK], NextX, y, TerArg.PageMode?(FrameLastX-NextX):(TerWinWidth-NextX),NULL);
            NextX=FrameLastX;
        }
        
        // free line pointers
        if (TempLead!=NULL) {
           MemFree(TempLead);
           TempLead=NULL;
        } 
    }


    // reset any DC rotation
    if (ScrFrameAngle>0 && OSCanRotate) FrameNoRotateDC(w,hDC);
    ScrFrameAngle=0;                  // end rotation at this point


    DRAW_BORDERS:                     // draw frame borders
    border=frame[CurFrame].border;
    if (border&BORDER_ALL && !(frame[CurFrame].flags&FRAME_NON_TEXT_DO)) {
       // calculate the border coordinates
       BorderX1=frame[CurFrame].ScrX;
       BorderY1=frame[CurFrame].y;
       BorderX2=frame[CurFrame].ScrX+frame[CurFrame].width;   // stay within the frame
       BorderY2=frame[CurFrame].y+frame[CurFrame].height; // stay within the frame

       // draw border adjusting co-ordinates such that border line remains within the frame
       if (HtmlMode && frame[CurFrame].CellId>0 && !(TerFlags3&TFLAG3_PLAIN_TABLE_BORDER) && cell[frame[CurFrame].CellId].flags&CFLAG_3D)
          DrawHtmlCellBorder(w,hDC,frame[CurFrame].CellId,BorderX1,BorderY1,BorderX2,BorderY2,frame[CurFrame].BorderWidth,TRUE);
       else {
          BOOL RtlCell=(frame[CurFrame].CellId>0 && True(frame[CurFrame].flags&FRAME_RTL));
          int x;

          if (RtlCell) SwapInts(&BorderX1,&BorderX2);

          DrawFrameBorder(w,hDC,BorderX1,BorderY1,BorderX2,BorderY1,frame[CurFrame].BorderWidth[BORDER_INDEX_TOP],BORDER_TOP);
          DrawFrameBorder(w,hDC,BorderX1,BorderY2-1,BorderX2,BorderY2-1,frame[CurFrame].BorderWidth[BORDER_INDEX_BOT],BORDER_BOT);
          DrawFrameBorder(w,hDC,BorderX1,BorderY1,BorderX1,BorderY2,frame[CurFrame].BorderWidth[BORDER_INDEX_LEFT],BORDER_LEFT);
          
          x=BorderX2-(RtlCell?0:1);
          DrawFrameBorder(w,hDC,x,BorderY1,x,BorderY2,frame[CurFrame].BorderWidth[BORDER_INDEX_RIGHT],BORDER_RIGHT);
       }
    }

    // reset global variables
    TextBorder=BoxLeft=BoxRight=FrameCellId=FrameRowId=FrameHeight=FrameShading=ParaShading=0;     // reset any text borders set
    ParaBoxColor=CLR_AUTO;
    SpaceBef=SpaceAft=TsFrameY=ScrYOffset=0;
    CharBoxLeft=CharBoxRight=bullet=FullCellHilight=EnableTransparent=transparent=ShadeBegin=ShadeEnd=FALSE;
    HilightType=SaveHilightType;
    CurPfmt=0;
    TabFlags=0;
    FrameBkColor=ParaBackColor=CLR_WHITE;
    ParaBoxSpace=PARA_BOX_SPACE;
    PaintFrameFlags=0;
    ExpandLineFmt=FALSE;
    ScrRtl=FALSE;
    if (pLineCharWidth) MemFree(pLineCharWidth);
    pLineCharWidth=NULL;
    if (pScrSeg) MemFree(pScrSeg);
    pScrSeg=NULL;
    TotalScrSeg=CurScrSeg=0;
    SectLine=-1;

}

/******************************************************************************
   DisplayText:
   Display the text in proper font.  Before calling this routine,  the
   text color should already be set in the display context.
*******************************************************************************/
#pragma optimize("g",off)  // turn-off global optimization
//#pragma optimize( "", off )   // turn-off all optimization temporariy
DisplayText(PTERWND w,HDC hDC,int col,int RowUnits,int height,int BaseHeight,LPBYTE ptr,int count,long line,BOOL ToEOL,BYTE area)
{
    int i,LastX,j,PrintedCol,CurX,CurY,FrameCharPos,RectHeight,AdjY,len;
    LPWORD fmt;
    LPBYTE lead;
    WORD PrevFmt=DEFAULT_CFMT;
    WORD CurFmt;
    UINT CurStyle;
    RECT rect;
    BOOL justify=FALSE;
    BOOL PrevItalicOverhang,CurItalicOverhang;
    BOOL InFnoteFrame=(BOOL)(TerOpFlags&TOFLAG_FNOTE_FRAME);
    LPWORD pWidth;
    BOOL DrawFullLine,CurSegRtl,HasCharBoxes;
    int  NextSegStart;
    WORD UcBase[MAX_WIDTH+1];
    BOOL HasUcBase;


    if (!InFnoteFrame && WordWrap && tabw(line) && tabw(line)->type&INFO_JUST) justify=TRUE;

    OverhangFont=-1;                        // non-zero when overhand need to be coverd
    
    // determine last x
    if      (ScrFrameAngle>0) LastX=frame[CurFrame].ScrX+frame[CurFrame].height;
    else if (ScrRtl) LastX=frame[CurFrame].x+frame[CurFrame].width;
    else {
       LastX=frame[CurFrame].ScrX+frame[CurFrame].ScrWidth;
       if (LastX>(TerWinOrgX+TerWinWidth)) LastX=TerWinOrgX+TerWinWidth;
    }
    DrawFullLine=(ScrFrameAngle>0 /*|| ScrRtl*/);

    // initiliaze the rtf segment
    if (ScrRtl && pScrSeg) {
       CurScrSeg=GetCharSeg(w,line,col,TotalScrSeg,pScrSeg);
       NextSegStart=pScrSeg[CurScrSeg].col+pScrSeg[CurScrSeg].count;
       CurSegRtl=pScrSeg[CurScrSeg].rtl;
    }
    else {
       CurScrSeg=0;
       CurSegRtl=FALSE;
       NextSegStart=9999;
    } 

    rect.top=RowUnits;                      // top of bounding rectangle
    rect.bottom=rect.top+height+SpaceBef+SpaceAft;   // bottom of bounding rectangle
    RectHeight=rect.bottom-rect.top;        // height of the rectangle
    RowUnits+=SpaceBef;                     // modify the text y position
    len=LineLen(line);

    if (count<=0) goto BLANK_EOL;
    if (NextX>LastX && !DrawFullLine) return TRUE;

    if (!InFnoteFrame && cfmt(line).info.type==UNIFORM && !ExpandLineFmt) { // one format line with no tabs
       CurFmt=cfmt(line).info.fmt;
       CurStyle=TerFont[CurFmt].style;
       SetFont(w,hDC,CurFmt,area);
       AdjY=TerFont[CurFmt].BaseHeightAdj;
       HasUcBase=(TerFont[CurFmt].UcBase>0);
       if (NextX<LastX || DrawFullLine) {
         rect.left=CurX=NextX;        // build the bounding rectangle
         for (j=0;j<count;j++) {
            NextX+=pLineCharWidth[col+j];
            UcBase[j]=TerFont[CurFmt].UcBase;
         }

         if (ToEOL && CurStyle&ITALIC && !(TerFont[CurFmt].flags&FFLAG_ITALIC_OH)) NextX+=DblCharWidth(w,CurFmt,TRUE,'W',0); // allow some extra space at the end of the non-overhang italic fonts
         rect.right=(NextX<=LastX?NextX:LastX);

         if (CurStyle&PICT) TerPictOut(w,hDC,CurX,RowUnits,&rect,CurFmt,count);
         else               TerTextOut(w,hDC,CurX,RowUnits+AdjY,&rect,ptr,TempLead?&TempLead[col]:NULL,count,&(pLineCharWidth[col]),HasUcBase?UcBase:NULL,BaseHeight);
       }
       PrevFmt=CurFmt;
    }
    else{
       BOOL FontBreak=FALSE;
       if (col>0) PrevFmt=TempCfmt[col-1];
       fmt=&TempCfmt[col];                     // pointer to the formatting information
       pWidth=&pLineCharWidth[col];
       CurFmt=fmt[0];                          // current character formatting info
       CurStyle=TerFont[CurFmt].style;         // current style
       CurItalicOverhang=TerFont[CurFmt].flags&FFLAG_ITALIC_OH;
       PrevItalicOverhang=TerFont[PrevFmt].flags&FFLAG_ITALIC_OH;
       PrintedCol=0;                           // number of columns printed so far
       UcBase[0]=TerFont[CurFmt].UcBase;
       HasUcBase=(UcBase[0]>0);

       lead=TempLead?(&TempLead[col]):NULL;   // pointer to the formatting information

       // write any initial frame space for the intervening frame
       if (!InFnoteFrame && tabw(line) && tabw(line)->type&INFO_FRAME && tabw(line)->FrameCharPos==col && col==0) WriteFrameSpace(w,hDC,line,rect.top,RectHeight);  // Write the frame space

       i=1;                                    // scan from the second column onward
       if (PrevItalicOverhang && !CurItalicOverhang && i<count && ptr[i]==' ') {  // don't print the next space character
           NextX+=DblCharWidth(w,CurFmt,TRUE,' ',0);
           i++;PrintedCol++;
           CurFmt=fmt[i];
           CurStyle=TerFont[CurFmt].style;
       }
       while(i<=count) {
          // get the character position before any intervening frame begins
          if (!InFnoteFrame && tabw(line) && tabw(line)->type&INFO_FRAME)
               FrameCharPos=tabw(line)->FrameCharPos;
          else FrameCharPos=-1;

          HasCharBoxes=(!InFnoteFrame && tabw(line) && tabw(line)->CharFlagsLen>0);

          // make uc base array
          if (i<count) {     // set after processing
             UcBase[i]=TerFont[fmt[i]].UcBase;
             //if (UcBase[i]>0) HasUcBase=TRUE;
          } 

          if (i<count) {
            FontBreak=(fmt[i]!=CurFmt);
            if (TerFont[CurFmt].UcBase && TerFont[fmt[i]].UcBase) FontBreak=IsFontBreak(w,CurFmt,fmt[i]);
          } 

          if ( i==count || FontBreak || ptr[i]==TAB || ptr[i]==HYPH_CHAR || ptr[i]==HIDDEN_CHAR || ptr[i]==ZWNJ_CHAR
            || (ptr[i]==' ' && ShowParaMark && False(TerFlags5&TFLAG5_NO_SHOW_SPACE_SYM))
            || (!TerArg.PageMode && ptr[i]==SECT_CHAR)
            || (FrameCharPos==(col+i)) || ((i+col)>=NextSegStart) 
            || (ScrRtl && CurSegRtl && (ptr[i-1]==' ' || ptr[i]==' ')) 
            || (HasCharBoxes && (col+i-1)>0 && tabw(line)->CharFlags[col+i-1]&BFLAG_BOX_LAST)) {

             SetFont(w,hDC,CurFmt,area);

             CurY=RowUnits+BaseHeight-TerFont[CurFmt].BaseHeight+TerFont[CurFmt].BaseHeightAdj;

             // do temporary tranlation of the characters
             if (ptr[PrintedCol]==TAB && !HiddenText(w,fmt[PrintedCol])) {    // write first tab in the segment
                WriteTab(w,hDC,line,CurY,rect.top,RectHeight,BaseHeight+SpaceBef,pWidth[PrintedCol]);  // generate tab spaces
                PrintedCol++;
             }
             else if (ShowParaMark && ptr[PrintedCol]==' ' && !HiddenText(w,fmt[PrintedCol]) && False(TerFlags5&TFLAG5_NO_SHOW_SPACE_SYM)) {    // show space character
                WriteSpaceSym(w,hDC,line,CurY,rect.top,RectHeight,BaseHeight+SpaceBef,pWidth[PrintedCol]);  // generate tab spaces
                PrintedCol++;
             }
             else if ((ptr[PrintedCol]==HYPH_CHAR || ptr[PrintedCol]==HIDDEN_CHAR) && !ShowParaMark) {
                PrintedCol++;              // this optional hyphen hidden
             }
             else if (ptr[PrintedCol]==ZWNJ_CHAR) {
                PrintedCol++;
             }
             ShowPageBreak=(line==CurLine && ptr[PrintedCol]==PAGE_CHAR);

             if ((NextX<LastX || DrawFullLine) && i>PrintedCol) {
                // get the print rectangle
                rect.left=CurX=NextX;

                for (j=0;j<(i-PrintedCol);j++) 
                   NextX+=pWidth[PrintedCol+j];

                if (!InFnoteFrame && tabw(line) && (col+PrintedCol)<tabw(line)->CharFlagsLen && tabw(line)->CharFlags[col+PrintedCol]&BFLAG_BOX_FIRST) {
                   CurX+=ExtraSpaceScrX;
                   CharBoxLeft=TRUE;
                }
                if (!InFnoteFrame && tabw(line) && (col+i-1)<tabw(line)->CharFlagsLen && tabw(line)->CharFlags[col+i-1]&BFLAG_BOX_LAST) {
                   CharBoxRight=TRUE;
                }

                if (ToEOL && i==count && CurStyle&ITALIC && !CurItalicOverhang) NextX+=DblCharWidth(w,CurFmt,TRUE,'W',0); // allow some extra space at the end of the non-overhang italic fonts
                rect.right=((NextX<=LastX || DrawFullLine)?NextX:LastX);

                CurY=RowUnits+BaseHeight-TerFont[CurFmt].BaseHeight+TerFont[CurFmt].BaseHeightAdj;

                // print the text
                if (CurStyle&PICT) TerPictOut(w,hDC,CurX,CurY+ScrYOffset,&rect,CurFmt,i-PrintedCol);
                else               TerTextOut(w,hDC,CurX,CurY,&rect,&ptr[PrintedCol],(lead?&(lead[PrintedCol]):NULL),i-PrintedCol,&pWidth[PrintedCol],HasUcBase?&(UcBase[PrintedCol]):NULL,BaseHeight);

                CharBoxLeft=CharBoxRight=FALSE;  // reset character box border

                if (CurItalicOverhang) {      // print a space to cover the overhang
                   if (i<count && ptr[i]==' ' && !(TerFont[fmt[i]].flags&FFLAG_ITALIC_OH)) {   // protect italic overhang part
                      NextX+=DblCharWidth(w,fmt[i],TRUE,' ',0);
                      i++;
                   }
                }
             }

             // now write the frame space for the intervening frame
             if (FrameCharPos==(col+i)) WriteFrameSpace(w,hDC,line,rect.top,RectHeight);  // Write the frame space

             //NEXT_SEG:
             if (i==count) break;

             PrintedCol=i;

             PrevFmt=CurFmt;                         // cur & prev BYTE formatting info
             if (PrintedCol<count) CurFmt=fmt[i];
             CurStyle=TerFont[CurFmt].style;         // current style
             CurItalicOverhang=TerFont[CurFmt].flags&FFLAG_ITALIC_OH;
             PrevItalicOverhang=TerFont[PrevFmt].flags&FFLAG_ITALIC_OH;
             HasUcBase=(UcBase[i]>0);

             if (ScrRtl && pScrSeg && (i+col)>=NextSegStart) {     // check next rtf/ltr seg start
                CurScrSeg++;
                NextSegStart=pScrSeg[CurScrSeg].col+pScrSeg[CurScrSeg].count;
                CurSegRtl=pScrSeg[CurScrSeg].rtl;
             } 
          }
          i++;
       }
    }


    BLANK_EOL:
    if (ToEOL && NextX<=LastX && ScrFrameAngle==0) {         // clear to the end of line
       BOOL IsRtlLine=(ScrRtl && pScrSeg && CurScrSeg<TotalScrSeg),SaveRtl;
       
       if (OverhangFont>=0)  NextX+=DblCharWidth(w,OverhangFont,TRUE,'W',0);  // reserve a space to cover the overhang

       SetFont(w,hDC,DEFAULT_CFMT,area);
       rect.left=NextX;
       rect.right=LastX;
       if (rect.right==(TerWinOrgX+TerWinWidth)) rect.right++;  // cover the last pixel also
       
       if (IsRtlLine) {
          SaveRtl=pScrSeg[CurScrSeg].rtl;
          pScrSeg[CurScrSeg].rtl=TRUE;
       }
       if (rect.right>rect.left && (CurBackColor!=PageColor(w) || TextBorder || BkPictId>0 || CurFrame>0)) 
           WriteSpaceRect(w,hDC,&rect);

       if (IsRtlLine) pScrSeg[CurScrSeg].rtl=SaveRtl;

       NextX=LastX;
    }

   return TRUE;
}
//#pragma optimize( "", on ) // restore optimization to previous mode
#pragma optimize("",off)  // restore optimization

/******************************************************************************
   TerTextOut:
   This routine displays a string of characters using the current font.
*******************************************************************************/
#pragma optimize( "", off )   // turn-off all optimization temporariy
TerTextOut(PTERWND w,HDC hDC,int x,int y,RECT far * rect, LPBYTE string,LPBYTE lead, int count, LPWORD pWidth,LPWORD pUcBase, int BaseHeight)
{
   UINT CurStyle,CurTempStyle,opaque=0,clip=ETO_CLIPPED;
   int  SaveRight,CurBaseHeight,PrevOverhangFont,width;
   HPEN hPen;
   RECT OrigRect,DispRect,TempRect;
   BOOL IsBreakLine=FALSE,IsMisspelled;
   COLORREF SaveForeColor,SaveBackColor;
   int  OrigCount=count,DelimChars=0;
   BYTE text[2*MAX_WIDTH+2];
   WORD UText[MAX_WIDTH+2];   // unicode text

   OrigRect=*rect;
   
   if (!transparent) opaque=ETO_OPAQUE;
   if ((TerFlags2&TFLAG2_XPARENT_WRITE) && (transparent || ScrFrameAngle>0 || ScrRtl)) clip=0;
   IsMisspelled=(TerFlags4&TFLAG4_AUTO_SPELL || TerOpFlags&TOFLAG_EXT_SPELLER) && TerFont[CurFmtId].flags&FFLAG_AUTO_SPELL;

   // Adjust the bounding rectangle
   if (count==1) {
      BYTE FirstChar=string[0];
      if (  FirstChar==PAGE_CHAR || FirstChar==SECT_CHAR || FirstChar==COL_CHAR
         || FirstChar==ROW_CHAR
         || IsHdrFtrChar(FirstChar)) {
         if (TerArg.PageMode) NextX=rect->right=frame[CurFrame].x+frame[CurFrame].width-10; // don't go all the way to right
         else                 NextX=rect->right=rect->left+TerWinWidth;
         IsBreakLine=TRUE;
      }
   }
   if (rect->right<TerWinOrgX && ScrFrameAngle==0 && !ScrRtl) return TRUE;

   //***** if the graphic segment exists, simply set the current position
   CurStyle=TerFont[CurFmtId].style;
   CurTempStyle=TerFont[CurFmtId].TempStyle;
   if (TerFont[CurFmtId].flags&FFLAG_ITALIC_OH && !ScrRtl) {
      rect->right=rect->right+DblCharWidth(w,CurFmtId,TRUE,'W',0);
   }

   // check if the text segment already drawn
   if (!HiddenText(w,CurFmtId) || count==0) {
      PrevOverhangFont=OverhangFont;                  // save the previous status of overhang
      if (TerFont[CurFmtId].flags&FFLAG_ITALIC_OH && count>0)
           OverhangFont=CurFmtId;  // protect this overhang from next string
      else OverhangFont=-1;        // no overhang to protect

      if (TextSegmentExists(w,x,y,rect,count,string)) return TRUE;  // already drawn
      if (True(TerOpFlags&TOFLAG_WRITING_BKND) && BkPictId!=0) opaque=0; 

      // When current character has italic overhang
      if (TerFont[CurFmtId].flags&FFLAG_ITALIC_OH && !ScrRtl) {
         if (PrevOverhangFont>=0) {   // previous string was italic
            // reduce the left extent of the rectangle
            rect->left+=DblCharWidth(w,PrevOverhangFont,TRUE,'W',0);
            if (rect->left>rect->right) rect->left=rect->right;
         }
         else {                       // previous string was not italic
            // fill the initial tilt space of the italic string
            SaveRight=rect->right;
            rect->right=rect->left+DblCharWidth(w,CurFmtId,TRUE,' ',0);
            OurExtTextOut(w,hDC,x,y,ETO_OPAQUE,rect,NULL,0,NULL);
            rect->right=SaveRight;
         }
      }
   }

   //***************** write text **********************************

   // draw character
   if (IsBreakLine) {                  // print a break line
      OurExtTextOut(w,hDC,x,y,opaque,rect,NULL,0,NULL); // draw rectangle

      // draw line
      y=rect->top+(rect->bottom-rect->top)/2;          // draw the line in the middle
      width=rect->right-rect->left;

      if (!TerArg.ReadOnly || TerFlags&TFLAG_SHOW_BREAKS) {
         if (!(TerArg.PageMode) || ShowParaMark || ((ShowPageBreak || True(TerFlags5&TFLAG5_SHOW_PAGE_BREAK)) && string[0]==PAGE_CHAR)) {
            if (string[0]==PAGE_CHAR) {
               WriteBreakLine(w,hDC,MsgString[MSG_PAGE_BREAK],rect->left,y,width,GetStockObject(BLACK_PEN));
               PageBreakShowing=TRUE;
            }
            if (string[0]==COL_CHAR)  WriteBreakLine(w,hDC,MsgString[MSG_COL_BREAK],rect->left,y,width-TwipsToScrX(360),GetStockObject(BLACK_PEN));
            if (string[0]==SECT_CHAR) WriteBreakLine(w,hDC,MsgString[MSG_SECT_BREAK],rect->left,y,width,GetStockObject(BLACK_PEN));
         }
      }

      if (string[0]==HDR_CHAR)  WriteBreakLine(w,hDC,MsgString[MSG_PAGE_HDR],rect->left,y,width,0);
      if (string[0]==FTR_CHAR)  WriteBreakLine(w,hDC,MsgString[MSG_PAGE_FTR],rect->left,y,width,0);
      if (string[0]==FHDR_CHAR)  WriteBreakLine(w,hDC,MsgString[MSG_FIRST_HDR],rect->left,y,width,0);
      if (string[0]==FFTR_CHAR)  WriteBreakLine(w,hDC,MsgString[MSG_FIRST_FTR],rect->left,y,width,0);
      if (string[0]==ROW_CHAR && !TerArg.PageMode)
                                WriteBreakLine(w,hDC,MsgString[MSG_ROW_BREAK],rect->left,y,width-TwipsToScrX(360),GetStockObject(BLACK_PEN));

      if (TextBorder&PARA_BOX) DrawParaBorder(w,hDC,rect,TRUE);  //draw paragraph border
   }
   else {    // draw regular text
      if (count==0 || !HiddenText(w,CurFmtId)) {
         int CharWidth[MAX_WIDTH],i,LeadLen=count;
         BYTE SaveMarker=0;

         if (pUcBase) {
             for (i=0;i<count;i++) {
                UText[i]=MakeUnicode(w,pUcBase[i],string[i]);  // build unicode characters
                text[i]=string[i];
                if (lead) lead[i]=0;
             }
             UText[i]=0;
             text[i]=0;
         }
         else  count=TerMakeMbcs(hTerWnd,text,string,lead,count);



         // discount the ending para/cell/sect char
         if (TerArg.WordWrap && !ShowParaMark && !pUcBase) {
            while (count>0 && (text[count-1]==ParaChar || text[count-1]==CellChar ||
                  text[count-1]==LINE_CHAR || text[count-1]==SECT_CHAR)) {
                  count--;
                  DelimChars++;
            }
         }
         
         // apply scap and caps adjustment
         if (lead==NULL && pUcBase==NULL && TerFont[CurFmtId].style&(SCAPS|CAPS)) {
            for (i=0;i<count;i++) if (IsLcChar(text[i])) text[i]+=(BYTE)('A'-'a');  // convert to upper case
         } 

         BuildMbcsCharWidth(w,CharWidth,CurFmtId,text,count,string,lead,LeadLen,pWidth);  // set the CharWidth array
         if (CharBoxLeft) CharWidth[0]-=ExtraSpaceScrX;
         if (CharBoxRight && count>0) CharWidth[count-1]-=ExtraSpaceScrX;

         // translate para and cell markers for display
         if (ShowParaMark && count>0 && pUcBase==NULL) {
            if (text[count-1]==ParaChar || text[count-1]==CellChar || text[count-1]==LINE_CHAR) {
               SaveMarker=text[count-1];   // save the original marker
               if (text[count-1]==ParaChar)  text[count-1]=(BYTE)((TerFlags2&TFLAG2_ALT_PARA_SYM)?DISP_PARA_CHAR_ALT:DISP_PARA_CHAR);  // show paramaker
               if (text[count-1]==LINE_CHAR) text[count-1]=(BYTE)((TerFlags2&TFLAG2_ALT_LINE_SYM)?DISP_LINE_CHAR_ALT:DISP_LINE_CHAR);  // show line break
               if (text[count-1]==CellChar)  text[count-1]=(BYTE)DISP_CELL_CHAR;  // show cell maker
            }
         }


         // translate '(' and ')' for rtl display
         //if (ScrRtl && OrigCount==1 && TerFont[CurFmtId].rtl) {
         //   if      (text[0]=='(') text[0]=')';
         //   else if (text[0]==')') text[0]='(';
         //}

         // draw the text
         if (SpaceBef>0 || SpaceAft>0) {   // draw the space rects separately
            TempRect=*rect;                 // draw the text
            if (CharBoxLeft) TempRect.left+=ExtraSpaceScrX;
            if (CharBoxRight) TempRect.right-=ExtraSpaceScrX;
            
            DispRect=TempRect;
            DispRect.top=DispRect.top+SpaceBef;
            DispRect.bottom=DispRect.bottom-SpaceAft;

            // show the text
            if (pUcBase) OurExtTextOutW(w,hDC,x,y+ScrYOffset,opaque|clip,&DispRect,UText,count,CharWidth); // ordinary character
            else         OurExtTextOut(w,hDC,x,y+ScrYOffset,opaque|clip,&DispRect,text,count,CharWidth); // ordinary character

            if (BkPictId==0) {             // clear the paragraph space before and after
               int SaveParaShading=ParaShading;
               COLORREF SaveParaBackColor=ParaBackColor;
               int SaveBkMode=GetBkMode(hDC);

               SaveForeColor=CurForeColor;
               SaveBackColor=CurBackColor;

               // shade space above text and and below any paragraph before spacing
               DispRect=TempRect;
               DispRect.top+=ParaSpcBef;  // this area not shaded
               DispRect.bottom=TempRect.top+SpaceBef;   // before the text starts
               SetColor(w,hDC,'T',DEFAULT_CFMT);
               SetBkMode(hDC,OPAQUE);
               OurExtTextOut(w,hDC,x,y+ScrYOffset,opaque|clip,&DispRect,"",0,NULL);

               // shade space below text and before any paragraph after spacing
               DispRect=TempRect;
               DispRect.top=TempRect.bottom-SpaceAft;
               DispRect.bottom=TempRect.bottom-ParaSpcAft;
               SetColor(w,hDC,'T',DEFAULT_CFMT);
               SetBkMode(hDC,OPAQUE);
               OurExtTextOut(w,hDC,x,y+ScrYOffset,opaque|clip,&DispRect,"",0,NULL);


               // reset the color
               if (SaveForeColor!=CurForeColor) {       // change foreground color
                   SetTextColor(hDC,SaveForeColor);
                   CurForeColor=SaveForeColor;
               }
               if (SaveBackColor!=CurBackColor) {       // change background color
                   SetBkColor(hDC,SaveBackColor);
                   CurBackColor=SaveBackColor;
               }

               SetBkMode(hDC,SaveBkMode);
            }
         }
         else {
            DispRect=(*rect);
            if (CharBoxLeft) DispRect.left+=ExtraSpaceScrX;
            if (CharBoxRight) DispRect.right-=ExtraSpaceScrX;
            if (pUcBase) OurExtTextOutW(w,hDC,x,y+ScrYOffset,opaque|clip,&DispRect/*rect*/,UText,count,CharWidth); // ordinary character
            else         OurExtTextOut(w,hDC,x,y+ScrYOffset,opaque|clip,&DispRect/*rect*/,text,count,CharWidth); // ordinary character
         }


         if (SaveMarker) text[count-1]=SaveMarker;  // restore para/cell delimiter

         if (TextBorder&BORDER_TOP) DrawShadowLine(w,hDC,rect->left,y+1+SpaceBef,rect->right,y+1+SpaceBef,hPagePen,NULL);
         if (TextBorder&BORDER_BOT) DrawShadowLine(w,hDC,rect->left,rect->bottom-1-SpaceAft,rect->right,rect->bottom-1-SpaceAft,hPagePen,NULL);

         // mark hidden text
         if (count>0 && (CurStyle&HIDDEN || (CurStyle&FNOTETEXT && !(TerOpFlags&TOFLAG_FNOTE_FRAME)))) {
            if (NULL!=(hPen=CreatePen(PS_DOT,0,GetTextColor(hDC)))) {
               CurBaseHeight=TerFont[CurFmtId].BaseHeight;
               DrawShadowLine(w,hDC,OrigRect.left,y+BaseHeight,OrigRect.right,y+BaseHeight,hPen,NULL);
               DeleteObject(hPen);
            }
         }
         // create under lines
         if ((CurStyle&(ULINED|ULINE) /*&& !(CurStyle&(SUPSCR|SUBSCR))*/)
              || CurTempStyle&TSTYLE_IME || IsMisspelled) {
           if (count>0) {    // don't write space for non-text drawing, so ClearEOL and WriteSpaceRect do not draw underline when the default font has ULINE
             BOOL PenStyle=(CurTempStyle&TSTYLE_IME && CurStyle&(ULINED|ULINE))?PS_DOT:PS_SOLID;
             COLORREF PenColor=GetTextColor(hDC);
             if (TerFont[CurFmtId].UlineColor!=CLR_AUTO) PenColor=TerFont[CurFmtId].UlineColor;
             if (IsMisspelled && CurBackColor!=0xFF) PenColor=0xFF;  // spell-check color

             if (NULL!=(hPen=CreatePen(PenStyle,0,PenColor))) {
                int DeltaY=(CurStyle&ULINE || IsMisspelled)?1:0;
                int width=0,BaseY;
               
                if (OrigCount==0) width=OrigRect.right-OrigRect.left;
                else  for (i=0;i<(OrigCount-DelimChars);i++) width+=(pWidth?pWidth[i]:DblCharWidth(w,CurFmtId,TRUE,text[i],0));
                if (BaseHeight==0) BaseY=y+TerFont[CurFmtId].BaseHeight+ScrYOffset;
                else               BaseY=BaseHeight+OrigRect.top+SpaceBef+ScrYOffset;

                if (IsMisspelled) DrawWigglyLine(w,hDC,OrigRect.left,BaseY+1,OrigRect.left+width,BaseY+1,PenColor);
                else if (CurStyle&ULINED) {
                   DrawShadowLine(w,hDC,OrigRect.left,BaseY,OrigRect.left+width,BaseY,hPen,NULL);
                   DrawShadowLine(w,hDC,OrigRect.left,BaseY+2,OrigRect.left+width,BaseY+2,hPen,NULL);
                }
                else  DrawShadowLine(w,hDC,OrigRect.left,BaseY+1,OrigRect.left+width,BaseY+1,hPen,NULL);
                DeleteObject(hPen);
             }
           }
         }
         
         // draw paragraph box
         if (TextBorder&PARA_BOX) DrawParaBorder(w,hDC,&OrigRect,TRUE);  //draw paragraph border

         // draw the character box
         if (TerFont[CurFmtId].style&CHAR_BOX && count>0) {
            if (NULL!=(hPen=CreatePen(PS_SOLID,0,GetTextColor(hDC)))) {
               int x1=OrigRect.left;
               int x2,width=0;
               int y1=y;  // rect->top+1;
               int y2=y1+DispTextHt-2;

               if (OrigCount==0) width=OrigRect.right-OrigRect.left;
               else              for (i=0;i<count;i++) width+=(pWidth?pWidth[i]:DblCharWidth(w,CurFmtId,TRUE,text[i],0));
               x2=x1+width;

               if (CharBoxLeft) x1++;
               if (CharBoxRight) x2--;
               if (y2>(rect->bottom-2)) y2=rect->bottom-2;
               CurBaseHeight=TerFont[CurFmtId].BaseHeight;

               DrawShadowLine(w,hDC,x1,y1,x2,y1,hPen,NULL);         // top line
               DrawShadowLine(w,hDC,x1,y2,x2,y2,hPen,NULL);   // bottom line
               if (CharBoxLeft)  DrawShadowLine(w,hDC,x1,y1,x1,y2,hPen,NULL); // left border
               if (CharBoxRight) DrawShadowLine(w,hDC,x2,y1,x2,y2,hPen,NULL); // right border
               DeleteObject(hPen);
            }
         }
      }
   }

   return TRUE;
}
#pragma optimize( "", on ) // restore optimization to previous mode

/******************************************************************************
   IsFontBreak:
   Check if the font characteristics changes enough to warrant a new text segment 
*******************************************************************************/
BOOL IsFontBreak(PTERWND w,int fmt1, int fmt2)
{
   if (TerFont[fmt1].hFont!=TerFont[fmt2].hFont) return TRUE;
   if (TerFont[fmt1].offset!=TerFont[fmt2].offset) return TRUE;
   if (TerFont[fmt1].style!=TerFont[fmt2].style) return TRUE;
   if (TerFont[fmt1].TextColor!=TerFont[fmt2].TextColor) return TRUE;
   if (TerFont[fmt1].TextBkColor!=TerFont[fmt2].TextBkColor) return TRUE;
   if ((TerFont[fmt1].TempStyle&TSTYLE_IME)!=(TerFont[fmt2].TempStyle&TSTYLE_IME)) return TRUE;

   return FALSE;
}

/******************************************************************************
   TerPictOut:
   Displays a bitmap specified by the 'pict' argument, at the specified
   location for a specified number of times.
   The area denoted by the 'rect' rectangle is painted with the background
   color and then the picture is drawn on the painted surface.
*******************************************************************************/
TerPictOut(PTERWND w,HDC hDC,int CurX,int CurY,RECT far *rect,int pict,int count)
{
    int i;
    BYTE string[80];
    BOOL ClearFull=TRUE;
    BOOL DeferPainting=FALSE;
    BOOL invisible;

    if (HiddenText(w,pict)) return TRUE;

    // store the picture co-ordinates for API return
    if (TerFont[pict].FrameType==PFRAME_NONE || (TerOpFlags&TOFLAG_DRAWING_FRAME_PICT)) {
       TerFont[pict].PictX=CurX;
       if (ScrRtl) TerFont[pict].PictX=RtlX(w,TerFont[pict].PictX,TerFont[pict].CharWidth[PICT_CHAR],CurFrame,&pScrSeg[CurScrSeg]);
       TerFont[pict].PictX-=TerWinOrgX;

       TerFont[pict].PictY=CurY-TerWinOrgY;
    }

    if (rect->right<TerWinOrgX && !ScrRtl) return TRUE;

    // check if it is an invisible aligned picture
    invisible=(TerArg.PageMode && TerFont[pict].FrameType!=PFRAME_NONE && !(TerOpFlags&TOFLAG_DRAWING_FRAME_PICT) && !ShowParaMark);

    //******** if graphic segment exists, simply set the current position
    if (count>=sizeof(string)) count=sizeof(string)-1;
    for (i=0;i<count;i++) string[i]='_';    // dummy character for picture
    string[count]=0;
    if (TextSegmentExists(w,CurX,CurY,rect,count,string)) {
       FirstPalette=FALSE;                  // protect this picture color any subsequent picture colors
       if (TerFont[pict].ObjectType==OBJ_OCX) TerFont[pict].flags|=FFLAG_CTL_DRAWN;
       return TRUE;
    }

    // clear the picture space
    if (BkPictId==0) {
      if (( TerFont[pict].PictType==PICT_DIBITMAP
         || TerFont[pict].PictType==PICT_CONTROL
         || TerFont[pict].PictType==PICT_FORM)
         && (!invisible) ) ClearFull=FALSE; //clear only the space above and below the picture

      if (TerFont[pict].ObjectType==OBJ_OCX  && !(TerFont[pict].flags&FFLAG_OCX_SRV_ERROR) && !hScrollBM) ClearFull=FALSE;

      if (!ClearFull) {        // clear only the space above or below the picture
         RECT rect1=*rect;
         if (CurY>rect->top) {
            rect1.bottom=CurY;
            OurExtTextOut(w,hDC,CurX,CurY,ETO_OPAQUE|ETO_CLIPPED,&rect1,NULL,0,NULL);
         }
         if ((CurY+TerFont[pict].height)<rect->bottom) {
            rect1=*rect;
            rect1.top=CurY+TerFont[pict].height;
            OurExtTextOut(w,hDC,CurX,CurY,ETO_OPAQUE|ETO_CLIPPED,&rect1,NULL,0,NULL);
         }
      }
      else if (invisible || TerFont[pict].PictType==PICT_ENHMETAFILE || TerFont[pict].PictType==PICT_METAFILE || TerFont[pict].PictType==PICT_META7) {
         OurExtTextOut(w,hDC,CurX,CurY,ETO_OPAQUE|ETO_CLIPPED,rect,NULL /*" "*/ ,0 /*1*/ ,NULL); //clear the entire area
      }
    }

    if (invisible) return TRUE;

    // should we defer painting
    if (hBufDC && hDC!=hTerDC && !hScrollBM && !invisible) {        // defer drawing of the control until the screen the drawn
       if (TerFont[pict].ObjectType==OBJ_OCX && !(TerFont[pict].flags&FFLAG_OCX_SRV_ERROR)) DeferPainting=TRUE;
       else if (IsControl(w,pict)) DeferPainting=TRUE;
    }

    if (DeferPainting) TerFont[pict].flags|=FFLAG_DRAW_PENDING;
    else {
       int CurPict=pict;
       if (TerFont[pict].anim && TerFont[pict].anim->CurAnim) {
          CurPict=TerFont[pict].anim->CurAnim;
          if (TerFont[CurPict].InUse && TerFont[CurPict].style&PICT) {
             TerFont[CurPict].CharWidth[PICT_CHAR]=TerFont[pict].CharWidth[PICT_CHAR];
             TerFont[CurPict].height=TerFont[pict].height;
          }
          else CurPict=pict;
       }

       PictOut(w,hDC,CurX,CurY,CurPict,count);
    }

    TerFont[pict].flags|=FFLAG_PICT_DRAWN;           // picture on the screen

    // draw paragraph box
    if (TextBorder&PARA_BOX) DrawParaBorder(w,hDC,rect,TRUE);  //draw paragraph border

    // set the timer for the next animation in sequence
    if (TerFont[pict].anim) SetAnimTimer(w,pict);

    return TRUE;
}

/******************************************************************************
   PictOut:
   Displays a bitmap specified by the 'pict' argument, at the specified
   location for a specified number of times.
*******************************************************************************/
PictOut(PTERWND w,HDC hDC,int CurX,int CurY,int pict,int count)
{
   HBITMAP hOldBM,hCurBM;
   int width,height,SaveId,CurColorCount,DrawWidth,DrawHeight;
   int DrawX=CurX;
   int DrawY=CurY;
   LPBITMAPINFO pInfo;
   void far *pImage;
   MFENUMPROC lpProc;
   HPEN hOldPen=NULL;
   BOOL crop;
   HRGN  rgn;

   DrawWidth=width=TerFont[pict].CharWidth[PICT_CHAR];   // width and height of the picture
   DrawHeight=height=TerFont[pict].height;

   // display the ocx control
   if (TerFont[pict].ObjectType==OBJ_OCX && !(TerFont[pict].flags&FFLAG_OCX_SRV_ERROR) && !hScrollBM) {
     if (TerFont[pict].flags&FFLAG_CTL_VISIBLE) {
        MovingControl=TRUE;
        PosOcx(w,pict,hDC);
        MovingControl=FALSE;

        ValidateRect(hTerWnd,&TerWinRect);
        TerSetClipRgn(w);               // restore clip region reset by ValidateRect
     }
     else ShowOcx(w,pict);


     if (TerFont[pict].flags&FFLAG_CTL_VISIBLE) {
        TerFont[pict].flags|=FFLAG_CTL_DRAWN;
        return TRUE;
     }
   }

   // apply picture cropping to the start pos
   crop=(TerFont[pict].CropLeft || TerFont[pict].CropRight || TerFont[pict].CropTop || TerFont[pict].CropBot);  // Is cropping used
   if (crop && TerFont[pict].PictWidth!=0) {  // convert twips to Pixel
      float scale=(float)TerFont[pict].PictWidth/(TerFont[pict].PictWidth-TerFont[pict].CropLeft-TerFont[pict].CropRight);  // non-zero scale implies that the picture was resized
      int CropLeft=(int)(TerFont[pict].CropLeft*scale);
      DrawX-=TwipsToScrY(CropLeft);
      DrawWidth=(int)(TerFont[pict].PictWidth*scale);
      DrawWidth=TwipsToScrX(DrawWidth);
   } 
   if (crop && TerFont[pict].PictHeight!=0) {  // convert twips to Pixel
      float scale=(float)TerFont[pict].PictHeight/(TerFont[pict].PictHeight-TerFont[pict].CropTop-TerFont[pict].CropBot);  // non-zero scale implies that the picture was resized
      int CropTop=(int)(TerFont[pict].CropTop*scale);
      DrawY-=TwipsToScrY(CropTop);
      DrawHeight=(int)(TerFont[pict].PictHeight*scale);
      DrawHeight=TwipsToScrY(DrawHeight);
   } 

   // turn-off rotation
   if (ScrFrameAngle>0 && OSCanRotate) FrameNoRotateDC(w,hDC);

   // Draw picture
   if (TerFont[pict].PictType==PICT_DIBITMAP) { // draw device independent bitmap

      while(count>0) {                          // draw bitmaps
         if (FALSE && TerFont[pict].hPal==NULL) {    // use default palette
            hCurBM=TerFont[pict].hBM;
            if (NULL==(hOldBM=SelectObject(hMemDC,hCurBM))) return TRUE;

            StretchBlt(hDC,CurX,CurY,width,height,hMemDC,0,0,TerFont[pict].bmWidth,TerFont[pict].bmHeight,SRCCOPY);

            SelectObject(hMemDC,hOldBM);         // deselect the bitmap
         }
         else {                                 // use logical palette
            int PrevStretchMode;
            BOOL SetStretchMode;

            pInfo=(LPBITMAPINFO)TerFont[pict].pInfo;
            pImage=(LPVOID)TerFont[pict].pImage;

            if (TerFont[pict].hPal) {
               if (TerFont[pict].TotalRgbs>0) CurColorCount=TerFont[pict].TotalRgbs;
               else                           CurColorCount=(WORD)TerFont[pict].ColorsUsed;
               TerSetPalette(w,hDC,TerFont[pict].hPal,CurColorCount,NULL,TRUE);
            }

            SetStretchMode=(TerFont[pict].bmHeight!=DrawWidth || TerFont[pict].bmHeight!=DrawHeight);
            if (SetStretchMode) {
               if (false && IsWinNT2000) PrevStretchMode=SetStretchBltMode(hDC,STRETCH_DELETESCANS);  // Newer STRETCH_HALFTONE does not work on WinNT
               else                      PrevStretchMode=SetStretchBltMode(hDC,HALFTONE);
            }

            if (crop) {      // set the clipping region
               SaveDC(hDC);
               rgn=CreateRectRgn(TerWinRect.left+CurX-TerWinOrgX,TerWinRect.top+CurY-TerWinOrgY,TerWinRect.left+CurX-TerWinOrgX+width,TerWinRect.top+CurY-TerWinOrgY+height);
               SelectClipRgn(hDC,rgn);
               DeleteObject(rgn);
            }
            
            OurStretchDIBits(w,hDC,DrawX,DrawY,DrawWidth,DrawHeight,0,0,TerFont[pict].bmWidth,TerFont[pict].bmHeight,pImage,pInfo,DIB_RGB_COLORS,SRCCOPY);

            if (crop) RestoreDC(hDC,-1);

            if (SetStretchMode) SetStretchBltMode(hDC,PrevStretchMode);
         }
         DrawX=DrawX+TerFont[pict].CharWidth[PICT_CHAR];
         count--;
      }
   }
   else if (TerFont[pict].PictType==PICT_METAFILE
         || TerFont[pict].PictType==PICT_META7) { // draw device independent bitmap
      HRGN hRgn;
      int  x,y,EndY;
      BOOL ExtDC=(hDC!=hTerDC && hDC!=hBufDC);
      int  OffsetX=(ExtDC?0:(TerWinRect.left-TerWinOrgX));
      int  OffsetY=(ExtDC?0:(TerWinRect.top-TerWinOrgY));

      SaveId=SaveDC(hDC);                  // save device context before playing the metafile

      if (ScrFrameAngle>0) {
         RECT rect;
         SetRect(&rect,CurX,CurY,CurX+width,CurY+height);
         FrameRotateRect(w,&rect,CurFrame);
         CurX=(rect.left<rect.right?rect.left:rect.right);
         CurY=(rect.top<rect.bottom?rect.top:rect.bottom);
         SwapInts(&height,&width);

         SetRect(&rect,DrawX,DrawY,DrawX+DrawWidth,DrawY+DrawHeight);
         FrameRotateRect(w,&rect,CurFrame);
         DrawX=(rect.left<rect.right?rect.left:rect.right);
         DrawY=(rect.top<rect.bottom?rect.top:rect.bottom);
         SwapInts(&DrawHeight,&DrawWidth);
      }    
      
      if (ScrRtl && pScrSeg && CurScrSeg<TotalScrSeg) {
         CurX=RtlX(w,CurX,width,CurFrame,&(pScrSeg[CurScrSeg]));
         DrawX=RtlX(w,DrawX,DrawWidth,CurFrame,&(pScrSeg[CurScrSeg]));
      }

      // set the clipping region for the metafile drawing
      x=CurX+OffsetX;
      y=CurY+OffsetY;
      EndY=y+height;
      if (!ExtDC) {
         if (y<TerWinRect.top) y=TerWinRect.top; // protect ruler/toolbar etc
         if (EndY>TerWinRect.bottom) EndY=TerWinRect.bottom;
      } 
      
      
      hRgn=CreateRectRgn(x,y,x+width,EndY);
      if (TerFont[pict].TextAngle==0) SelectClipRgn(hDC,hRgn);
      DeleteObject(hRgn);

      if (TerFont[pict].PictType==PICT_META7) SetMapMode(hDC,MM_ISOTROPIC);          // use customized units
      else                                    SetMapMode(hDC,MM_ANISOTROPIC);

      if (TerFont[pict].PictType==PICT_METAFILE) {
          // most metafile would override the SetWindowExtEx to set it to the actual drawing height
          // Here we are setting the values when a metafile does not have an override. 
          //SetWindowExtEx(hDC,MulDiv(TerFont[pict].bmWidth,OrigScrResX,1440),MulDiv(TerFont[pict].bmHeight,OrigScrResY,1440),NULL); // logical dimension
         SetWindowExtEx(hDC,TerFont[pict].bmWidth,TerFont[pict].bmHeight,NULL); // logical dimension
      }
      else SetWindowExtEx(hDC,TerFont[pict].bmWidth,TerFont[pict].bmHeight,NULL); // logical dimension

      SetViewportExtEx(hDC,DrawWidth,DrawHeight,NULL); // equivalent device dimension

      SelectObject(hDC,GetStockObject(SYSTEM_FONT));

      while(count>0) {                      // draw bitmaps
         SetViewportOrgEx(hDC,DrawX+OffsetX,DrawY+OffsetY,NULL); // set view port origins
         SetWindowOrgEx(hDC,0,0,NULL);

         MetaPalFound=FALSE;
         if (NULL!=(lpProc=(MFENUMPROC)MakeProcInstance((FARPROC)ScrEnumMetafile, hTerInst))) {
             EnumMetaFile(hDC,TerFont[pict].hMeta,lpProc,(LPARAM)w);
             FreeProcInstance((FARPROC)lpProc);        // free the process instances
         }

         CurX=CurX+TerFont[pict].CharWidth[PICT_CHAR];
         count--;
      }

      RestoreDC(hDC,SaveId);

      if (hTerPal && MetaPalFound && !(TerFlags&TFLAG_NO_PALETTE)) {   // re-apply because the dc got reset
         SelectPalette(hDC,hTerPal,FALSE);
         RealizePalette(hDC);
      }
   }
   #if defined(WIN32)
   else if (TerFont[pict].PictType==PICT_ENHMETAFILE ) {
       RECT rect;
       SaveId=SaveDC(hDC);                  // save device context before playing the metafile
       
       if (ScrRtl && pScrSeg && CurScrSeg<TotalScrSeg) {
         CurX=RtlX(w,CurX,width,CurFrame,&(pScrSeg[CurScrSeg]));
         DrawX=RtlX(w,DrawX,DrawWidth,CurFrame,&(pScrSeg[CurScrSeg]));
       }
       while(count>0) {                      // draw bitmaps
           //ENHMETAHEADER emh;
           if (ScrFrameAngle>0) {
              RECT rect;
              SetRect(&rect,CurX,CurY,CurX+width,CurY+height);
              FrameRotateRect(w,&rect,CurFrame);
              CurX=(rect.left<rect.right?rect.left:rect.right);
              CurY=(rect.top<rect.bottom?rect.top:rect.bottom);
              SwapInts(&height,&width);

              SetRect(&rect,DrawX,DrawY,DrawX+DrawX+DrawWidth,DrawY+DrawHeight);
              FrameRotateRect(w,&rect,CurFrame);
              DrawX=(rect.left<rect.right?rect.left:rect.right);
              DrawY=(rect.top<rect.bottom?rect.top:rect.bottom);
              SwapInts(&DrawHeight,&DrawWidth);
           }    

           
           if (crop) {
              SaveDC(hDC);
              rgn=CreateRectRgn(TerWinRect.left+CurX-TerWinOrgX,TerWinRect.top+CurY-TerWinOrgY,TerWinRect.left+CurX-TerWinOrgX+width,TerWinRect.top+CurY-TerWinOrgY+height);
              SelectClipRgn(hDC,rgn);
              DeleteObject(rgn);
           }
           
           rect.left = DrawX;
           rect.top = DrawY;
           rect.right = rect.left + width;   
           rect.bottom = rect.top + height;  

           //GetEnhMetaFileHeader(TerFont[pict].hEnhMeta, sizeof(emh), &emh);
           
           //PlayEnhMetaFile(hDC, TerFont[pict].hEnhMeta, &rect);
           EnumEnhMetaFile(hDC, TerFont[pict].hEnhMeta, &ScrEnhMetaFileProc, (LPVOID) w, &rect); 

           if (crop) RestoreDC(hDC,-1);

           DrawX=DrawX+DrawWidth;
           count--;
       }
       RestoreDC(hDC, SaveId);
   }
   #endif
   else if (IsControl(w,pict) && TerFont[pict].hWnd) { // show the control window
      if (hScrollBM) {        // simply draw an empty rectangle
         HPEN hOldPen=SelectObject(hTerDC,GetStockObject(BLACK_PEN));
         Rectangle(hTerDC,CurX,CurY,CurX+width,CurY+height);
         SelectObject(hTerDC,hOldPen);
      }
      else {
         int x,y,i;
         HWND hSaveWnd;
         RECT rect;
         struct StrControl far *pInfo;

         x=TerWinRect.left+CurX-TerWinOrgX;  // x in client cooridnate
         y=TerWinRect.top+CurY-TerWinOrgY;   // y in client cooridnate

         MovingControl=TRUE;
         SetWindowPos(TerFont[pict].hWnd,NULL,x,y,0,0,SWP_DRAWFRAME|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
         MovingControl=FALSE;

         InvalidateRect(TerFont[pict].hWnd,NULL,TRUE);
         SendMessage(TerFont[pict].hWnd,WM_PAINT,0,0L);

         rect=TerRect;
         if (hToolBarWnd) rect.top=ToolBarHeight;
         ValidateRect(hTerWnd,&rect);
         TerSetClipRgn(w);              // restore clip region reset by ValidateRect

         // draw rectangle around a radio control when selected
         pInfo=(LPVOID)TerFont[pict].pInfo;
         if (lstrcmpi(pInfo->class,"Button")==0 && (pInfo->style&0xFF)==BS_RADIOBUTTON) {
            if (GetFocus()==TerFont[pict].hWnd) {
               int y2=CurY+height-1;
               int x2=CurX+width-1;
               DrawDottedLine(w,hDC,CurX,CurY,CurX,y2,0);
               DrawDottedLine(w,hDC,CurX,y2,x2,y2,0);
               DrawDottedLine(w,hDC,x2,y2,x2,CurY,0);
               DrawDottedLine(w,hDC,x2,CurY,CurX,CurY,0);
            }
         } 

         // remove the extra control elements from the text segments
         hSaveWnd=TerFont[pict].hWnd;
         TerFont[pict].hWnd=0;           // to protect the window from begin hidden
         for (i=0;i<TotalSegments;i++) {
           if (!(TextSeg[i].drawn) || TextSeg[i].fmt!=pict || RepaintNo==TextSeg[i].RepaintNoW) continue;
           DeleteTextSeg(w,i,FALSE);
         }
         TerFont[pict].hWnd=hSaveWnd;       //  restore the window handle

         // record the directions in which the control spilled over the border
         CalcBorderSpill(w,x,y,x+width,y+width);

         TerFont[pict].flags|=(FFLAG_CTL_VISIBLE|FFLAG_CTL_DRAWN);
      }
   }

   TerFont[pict].flags=ResetUintFlag(TerFont[pict].flags,FFLAG_DRAW_PENDING);  // picture drawn

   if (ScrFrameAngle>0 && OSCanRotate) FrameRotateDC(w,hDC,CurFrame);

   return TRUE;
}

/****************************************************************************
    ScrEnhMetaFileProc:
    process the page metafile
*****************************************************************************/
int CALLBACK ScrEnhMetaFileProc(HDC hDC, HANDLETABLE *pTable, CONST ENHMETARECORD *pRec,  int nObj, LPARAM lParam)
{
    PTERWND w=(PTERWND)lParam;
    int type=pRec->iType;
    
    //DispEnhMetaRecType(w,type);

    if (type==EMR_STRETCHDIBITS) {    // PlayEnhMEtaFileRecord does not work for this type sometimes, so do our own
       EMRSTRETCHDIBITS far * rec=(EMRSTRETCHDIBITS far *) pRec;
       LPBYTE pByte=(LPBYTE) pRec;
       LPBYTE pImage=&(pByte[rec->offBitsSrc]);
       BITMAPINFO *pInfo=(BITMAPINFO *) &(pByte[rec->offBmiSrc]);

       StretchDIBits(hDC,rec->xDest,rec->yDest,(rec->rclBounds.right-rec->rclBounds.left),labs(rec->rclBounds.bottom-rec->rclBounds.top),
                        rec->xSrc,rec->ySrc,rec->cxSrc,rec->cySrc,pImage,pInfo,rec->iUsageSrc,rec->dwRop);
    }
    else PlayEnhMetaFileRecord(hDC,pTable,pRec,nObj);

    return 1;
}    

/******************************************************************************
   CalcBorderSpill:
   Calculate the border spill by a control.
*******************************************************************************/
CalcBorderSpill(PTERWND w,int x1, int y1, int x2, int y2)
{
    if (x1<=TerWinRect.left && x2>=0)               BorderSpill|=SPILL_LEFT;
    if (x1<=TerRect.right && x2>=TerWinRect.right)  BorderSpill|=SPILL_RIGHT;
    if (y1<=TerWinRect.top && y2>=0)                BorderSpill|=SPILL_TOP;
    if (y1<=TerRect.bottom && y2>=TerWinRect.bottom)BorderSpill|=SPILL_BOT;

    return TRUE;
}
/******************************************************************************
   PosControls:
   Position the controls.
*******************************************************************************/
PosControls(PTERWND w)
{
   int i;

   for (i=0;i<TotalFonts;i++) {
      if (TerFont[i].style&PICT && TerFont[i].flags&FFLAG_DRAW_PENDING) {
         PictOut(w,hTerDC,TerFont[i].PictX+TerWinOrgX,TerFont[i].PictY+TerWinOrgY,i,1);
         CopyCtlImage(w,i);      // copy the image to buf DC
      }
   }
   return TRUE;
}

/******************************************************************************
   CopyCtlImage:
   Copy the images of the curretnly visible control to the buffer dc.
*******************************************************************************/
CopyCtlImage(PTERWND w,int pict)
{
   int i,x,y,width,height,first=0,last=TotalFonts-1;

   if (!hBufDC) return TRUE;

   if (pict>=0) first=last=pict;

   for (i=first;i<=last;i++) {
     if (!TerFont[i].InUse || !(TerFont[i].style&PICT)) continue;
     if (  TerFont[i].PictType!=PICT_CONTROL
        && TerFont[i].PictType!=PICT_FORM
        && TerFont[i].ObjectType!=OBJ_OCX) continue;
     if (!(TerFont[i].flags&FFLAG_CTL_VISIBLE)) continue;

     x=TerFont[i].PictX+TerWinOrgX;
     y=TerFont[i].PictY+TerWinOrgY;
     width=TerFont[i].CharWidth[PICT_CHAR];
     height=TerFont[i].height;

     BitBlt(hBufDC,x,y,width,height,hTerDC,x,y,SRCCOPY);
   }

   return TRUE;
}

/******************************************************************************
    ScrEnumMetafile:
    Enumeration of metafile records to use background palette.  All metafile
    'SelectPalette' function by default use the foreground palette.
*******************************************************************************/
BOOL CALLBACK _export ScrEnumMetafile(HDC hDC, HANDLETABLE FAR *pTable, METARECORD FAR *pMetaRec, int ObjCount, LPARAM lParam)
{
    PTERWND w=(PTERWND)lParam;
    LPBITMAPINFOHEADER pInfoHdr;
    LPBITMAPINFO pBmInfo;
    HPALETTE hPal=NULL,hOldPal=NULL;
    LOGPALETTE far * pPal;
    int i,ColorsUsed;

    //DispMetaRecName(pMetaRec->rdFunction);

    if (pMetaRec->rdFunction==META_SELECTPALETTE) {
       TerSetPalette(w,hDC,pTable->objectHandle[(pMetaRec->rdParm[0])],0,NULL,FALSE);
       MetaPalFound=TRUE;                     // this metafile uses palette
    }
    else if (pMetaRec->rdFunction==META_STRETCHDIB) {
       // create palette if necessary
       if (!MetaPalFound && Clr256Display && !(TerFlags&TFLAG_NO_PALETTE)) { // check if palette needs to be created
         pInfoHdr=(LPBITMAPINFOHEADER)&(pMetaRec->rdParm[11]);  // bitmap info starts at 11 word offset
         if (pInfoHdr->biBitCount==8) {   // 256 colors used
            ColorsUsed=(pInfoHdr->biClrUsed==0)?256:(int)pInfoHdr->biClrUsed;
            if (NULL==(pPal=OurAlloc(sizeof(LOGPALETTE)+ColorsUsed*sizeof(PALETTEENTRY)))) return TRUE;
            pPal->palVersion=0x300;
            pPal->palNumEntries=ColorsUsed;

            pBmInfo=(LPBITMAPINFO)pInfoHdr;
            for (i=0;i<(int)(pPal->palNumEntries);i++) {
               pPal->palPalEntry[i].peRed=pBmInfo->bmiColors[i].rgbRed;
               pPal->palPalEntry[i].peGreen=pBmInfo->bmiColors[i].rgbGreen;
               pPal->palPalEntry[i].peBlue=pBmInfo->bmiColors[i].rgbBlue;
               pPal->palPalEntry[i].peFlags=0;
            }

            hPal=CreatePalette(pPal);
            OurFree(pPal);
         }
       }
       if (hPal) hOldPal=TerSetPalette(w,hDC,hPal,0,NULL,TRUE);

       PlayMetaFileRecord(hDC,pTable,pMetaRec,ObjCount);
       if (hPal) {
         if (hOldPal) SelectPalette(hDC,hOldPal,TRUE);
         DeleteObject(hPal);
       }
    }
    //else if (pMetaRec->rdFunction==META_SETWINDOWORG) return TRUE;  // filter it out
    else if (pMetaRec->rdFunction==META_SETVIEWPORTORG) return TRUE;  // filter it out
    
    else if (pMetaRec->rdFunction==0x41) return FALSE;  // unsupported mac metafile command, stop playing

    else PlayMetaFileRecord(hDC,pTable,pMetaRec,ObjCount);

    return TRUE;
}

