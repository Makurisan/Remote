/*==============================================================================
   TER Editor Developer's Kit
   TER2.C
   Text editing functions

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
    TerSetCursorShape:
    This function changes the cursor shape depending upon the hot spot type.
*******************************************************************************/
TerSetCursorShape(PTERWND w,DWORD lParam,BOOL normal)
{
    int x,y,obj=-1,HotSpot,MouseFrame,CurFmt,MouseRow=0;
    POINT pt;
    HCURSOR hCursor=NULL;

    if (TerFlags2&TFLAG2_NO_CURSOR_CHANGE) return TRUE;  // no cursor changes allowed

    if (!normal && TblSelCursShowing && StretchHilight) return TRUE;  // keep the table highlighting cursor on

    // get the hot spot
    x=(int)(short)LOWORD(lParam);
    y=(int)(short)HIWORD(lParam);

    pt.x=x-TerWinRect.left+TerWinOrgX;    // convert to logical units
    pt.y=y-TerWinRect.top+TerWinOrgY;

    if (!normal && !TerArg.ReadOnly) {
       GetHotSpotHit(w,&pt,&obj,&HotSpot);    // get object and hot shot hit
    }

    // Select cursor for the drag object
    if (obj>=0) {
       // get the cursor type
       if (DragObj[obj].type==DRAG_TYPE_CELL || DragObj[obj].type==DRAG_TYPE_ROW_INDENT) {
           hCursor=hTable1Cur;
       }
       else if (DragObj[obj].type==DRAG_TYPE_ROW) {
           hCursor=hTable3Cur;
       }

       if (DragObj[obj].type>=DRAG_TYPE_RULER_BEGIN && DragObj[obj].type<=DRAG_TYPE_RULER_END) {
           if (DragObj[obj].type==DRAG_TYPE_RULER_TAB) hCursor=hTab1Cur;
           else                                        hCursor=LoadCursor(0,IDC_SIZEWE);
       }

       if (DragObj[obj].type==DRAG_TYPE_PICT_SIZE || DragObj[obj].type==DRAG_TYPE_FRAME_SIZE) {
          if (DragObj[obj].type==DRAG_TYPE_FRAME_SIZE && ParaFrame[DragObj[obj].id1].flags&PARA_FRAME_LINE) hCursor=LoadCursor(0,IDC_CROSS);
          else if (HotSpot==DRAG_HOT_LEFT || HotSpot==DRAG_HOT_RIGHT) hCursor=LoadCursor(0,IDC_SIZEWE);
          else                                                        hCursor=LoadCursor(0,IDC_SIZENS);
       }
       if (DragObj[obj].type==DRAG_TYPE_FRAME_MOVE) hCursor=hPlusCur;
    }
    else {
       TerMousePos(w,lParam,FALSE);             // get the mouse line
       if (-1==(MouseFrame=GetFrame(w,MouseLine))) return TRUE;      // frame of the mouse line

       // find the table row on which the mouse is placed
       if (TerArg.PageMode && MouseLine>=0 && MouseLine<TotalLines) {  // find the cell
          int MouseCell=cid(MouseLine);
          MouseRow=cell[MouseCell].row;
       }

       // show other cursors
       if (WheelShowing && !normal && hWheelCur) {
          hCursor=hWheelCur;
          CurDragObj=-1;
          RulerClicked=FALSE;
       }
       else if (DraggingText && !InOleDrag && !normal) {
          if (MouseOverShoot!=' ' && MouseOverShootDist>(TerFont[0].height*3/2))
                hCursor=hDragOutCur;
          else  {
             BOOL CtrlPressed=GetKeyState(VK_CONTROL)&0x8000;
             hCursor=CtrlPressed?hDragInCopyCur:hDragInCur;
          }
          RulerClicked=FALSE;                 // reset the mouse position flags
          CurDragObj=-1;
       }
       else if (!normal && TerOpFlags&TOFLAG_CURS_ON_TABLE_TOP) hCursor=hTable2Cur;
       else if (ShowHyperlinkCursor && !normal && !(TerOpFlags&TOFLAG_X_BEFORE_LINE)) {
          if (!RulerClicked && CurDragObj<0) {
             // get the current font
             int CurCfmt=GetCurCfmt(w,MouseLine,MouseCol);
             if (IsHypertext3(w,CurCfmt,FALSE,TRUE)) hCursor=hHyperlinkCur;
          }
          RulerClicked=FALSE;                 // reset the mouse position flags
          CurDragObj=-1;
       }
       else if (True(GetKeyState(VK_CONTROL)&0x8000) && CurDragObj==-1 && !RulerClicked 
                 && !normal && !(TerOpFlags&TOFLAG_X_BEFORE_LINE)) {
          if (InvokeTextLink(w,false,MouseLine,MouseCol)) hCursor=hHyperlinkCur;
       } 

       // set text and margin cursor
       if (!hCursor) {
          // get the text area
          if   (TerArg.ReadOnly) hCursor=LoadCursor(0,IDC_ARROW);
          else if (pt.y<TerWinOrgY || pt.y>(TerWinOrgY+TerWinHeight)) hCursor=LoadCursor(0,IDC_ARROW);
          else if (pt.x<(frame[MouseFrame].x+PfmtId[pfmt(MouseLine)].LeftIndent)) hCursor=LoadCursor(0,IDC_ARROW);
          else if (pt.x<(frame[MouseFrame].x+frame[MouseFrame].width-PfmtId[pfmt(MouseLine)].RightIndent)) {
             if (CanDragText(w)) hCursor=LoadCursor(0,IDC_ARROW);
             else {
               CurFmt=GetCurCfmt(w,MouseLine,MouseCol);
               if      (TerFont[CurFmt].style&PROTECT) hCursor=LoadCursor(0,IDC_ARROW);
               else if (!(TerFont[CurFmt].style&PICT)) {
                  if (GetFrameTextAngle(w,MouseFrame)>0) hCursor=hHBeamCur;
                  else hCursor=LoadCursor(0,IDC_IBEAM);
               }
             }
          }
       }
    }

    if (obj<0 && !MouseOnTextLine && hCursor!=hWheelCur) hCursor=NULL;

    if (hCursor) SetCursor(hCursor);
    else         SetCursor(LoadCursor(0,IDC_ARROW));

    TblSelCursShowing=(hCursor==hTable2Cur);
    LinkCursShowing=(hCursor==hHyperlinkCur);

    return TRUE;
}

/******************************************************************************
    GetHotSpotHit:
    This function scans the drag object table to see if the given point
    lies in an object. If found, it returns the object number and hot spot
    number by reference.
*******************************************************************************/
GetHotSpotHit(PTERWND w,LPPOINT pt,LPINT obj,LPINT HotSpot)
{
    int i,j;

    (*obj)=-1;

    for (i=0;i<TotalDragObjs;i++) {
       if (!DragObj[i].InUse) continue;
       if (DragObj[i].IsHotPolygon) {
          POINT HotPt[4];
          HRGN hRgn;
          for (j=0;j<DragObj[i].HotRectCount;j++) {
             HotPt[j].x=DragObj[i].HotRect[j].left;
             HotPt[j].y=DragObj[i].HotRect[j].top;
          }
          if (NULL!=(hRgn=CreatePolygonRgn(HotPt,DragObj[i].HotRectCount,WINDING))) {
             BOOL InRegion=PtInRegion(hRgn,pt->x,pt->y);
             DeleteObject(hRgn);
             if (InRegion) {
                (*obj)=i;              // Current drag object
                (*HotSpot)=0;          // Current hot spot within the object
                return TRUE;
             }
          }
       }
       else {
          for (j=0;j<DragObj[i].HotRectCount;j++) {
             if (PtInRect(&(DragObj[i].HotRect[j]),(*pt))) {
                (*obj)=i;              // Current drag object
                (*HotSpot)=j;          // Current hot spot within the object
                return TRUE;
             }
          }
       }
    }
    return FALSE;
}

/******************************************************************************
    CanDragText:
    This function returns TRUE if the mouse is placed on a highlighted that
    can be dragged. The mouse position is given by the MouseLine and MouseCol
    global variables.
******************************************************************************/
BOOL CanDragText(PTERWND w)
{
    long HilightBeg,HilightEnd,MousePos;
    int  pict;

    if (HilightType!=HILIGHT_CHAR) return FALSE;

    HilightBeg=RowColToAbs(w,HilightBegRow,HilightBegCol);
    HilightEnd=RowColToAbs(w,HilightEndRow,HilightEndCol);
    MousePos=RowColToAbs(w,MouseLine,MouseCol);

    if (HilightBeg>HilightEnd) SwapLongs(&HilightBeg,&HilightEnd);
    if ((!TerArg.ReadOnly || True(TerFlags6&TFLAG6_OLE_DROP_SOURCE))
        && !(TerFlags&TFLAG_NO_DRAG_TEXT) && MousePos>=HilightBeg && MousePos<HilightEnd) {
        if (cid(HilightBegRow)==cid(HilightEndRow) && fid(HilightBegRow)==fid(HilightEndRow)) {
           if (PictureHilighted) {
              pict=GetCurCfmt(w,HilightBegRow,HilightBegCol);
              return (TerFont[pict].FrameType!=PFRAME_FLOAT);
           } 
           else return TRUE;
        }
    }

    return FALSE;
}

/******************************************************************************
    TerLineSelected:
    This function returns TRUE if a line is highlighted.  This function also
    returns TRUE if a text is not highlighted.
******************************************************************************/
BOOL WINAPI _export TerLineSelected(HWND hWnd, long LineNo)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return LineSelected(w,LineNo);
}

/******************************************************************************
    LineSelected:
    This function returns TRUE if the given line is highlighted line.
*******************************************************************************/
BOOL LineSelected(PTERWND w,long LineNo)
{
    int  BegCell,EndCell,CurCell,col,col1,col2,level,row;
    long BegCellLine=-1;
    int  min1,min2,max1,max2;

    if (HilightType!=HILIGHT_CHAR) return TRUE;
    if (LineNo<0 || LineNo>=TotalLines) return TRUE;


    level=MinTableLevel(w,HilightBegRow,HilightEndRow);
    CurCell=LevelCell(w,level,LineNo);
    if (CurCell==0) return TRUE;                  // non table line
    
    BegCell=LevelCell(w,level,HilightBegRow);
    EndCell=LevelCell(w,level,HilightEndRow);
    if (BegCell==0 || EndCell==0) return TRUE;    // out of table

    // check if hilight limited to one table
    if (!InSameTable(w,BegCell,EndCell)) return TRUE; // do not belong to the same table

    // check if the entire table selected
    if (IsFirstTableRow(w,cell[BegCell].row) && IsLastTableRow(w,cell[EndCell].row)
        && cell[BegCell].PrevCell<=0 && cell[EndCell].NextCell<=0) return TRUE;

    // check if entire table row is selected
    if (cell[BegCell].PrevCell<=0 && cell[EndCell].NextCell<=0) {  // full rows selected
       int FirstRow=cell[BegCell].row;
       int LastRow=cell[EndCell].row;
       int CurCellRow=cell[CurCell].row;

       if (CurCellRow==FirstRow || CurCellRow==LastRow) return TRUE;

       for(row=FirstRow;row>0;row=TableRow[row].NextRow) {
          if (row==CurCellRow) return TRUE;           // one of the selected row
          if (row==LastRow) break;
       }  
    }  
    
    // get the first/last highlighted column
    min1=GetCellColumn(w,BegCell,TRUE);
    max1=min1+cell[BegCell].ColSpan-1;
    min2=GetCellColumn(w,EndCell,TRUE);
    max2=min2+cell[EndCell].ColSpan-1;
    col1=(min1<min2)?min1:min2;
    col2=(max1>max2)?max1:max2;

    col=GetCellColumn(w,CurCell,TRUE);

    return (col>=col1 && col<=col2)?TRUE:FALSE;
}

/******************************************************************************
    TerMousePos:
    convert the x,y device co-ordinates of the current mouse position into
    text column and line positions-- MouseCol & MouseLine.
*******************************************************************************/
TerMousePos(PTERWND w,DWORD lParam, BOOL SetPage)
{
    int x,y,frm,FlatX;
    POINT pt;
    int CurFont;

    MouseOverShoot=' ';                  // reset mouse overshoot direction
    MouseOverShootDist=0;
    MouseLine=CurLine;
    MouseCol=CurCol;
    RulerClicked=FALSE;
    CurDragObj=-1;
    TerOpFlags=ResetLongFlag(TerOpFlags,TOFLAG_CURS_ON_TABLE_TOP);
    TerOpFlags=ResetLongFlag(TerOpFlags,TOFLAG_X_BEFORE_LINE);

    x=MouseX=(int)(short)LOWORD(lParam); // casting preserves sign
    y=MouseY=(int)(short)HIWORD(lParam);

    // check if mouse clicked on other hot areas
    if (!StretchHilight && !TerArg.ReadOnly) {
       // check if mouse clicked on the ruler
       if (TerArg.ruler && (x>=RulerRect.left && x<=RulerRect.right && y>=RulerRect.top && y<=RulerRect.bottom)) RulerClicked=TRUE;

       // check if a hot spot on a screen object clicked
       pt.x=x-TerWinRect.left+TerWinOrgX;    // convert to logical units
       pt.y=y-TerWinRect.top+TerWinOrgY;
       GetHotSpotHit(w,&pt,&CurDragObj,&CurHotSpot);
    }

    // check if mouse is out of the text area
    if (x<TerWinRect.left)   {
       MouseOverShoot='L'; // mouse in the left margin
       MouseOverShootDist=TerWinRect.left-x;
    }
    if (x>TerWinRect.right)  {
       MouseOverShoot='R'; // mouse in the right margin
       MouseOverShootDist=x-TerWinRect.right;
    }
    if (y<TerWinRect.top)    {
       MouseOverShoot='T'; // mouse in the top margin
       MouseOverShootDist=TerWinRect.top-y;
    }
    if (y>TerWinRect.bottom) {
       MouseOverShoot='B'; // mouse in the bottom margin
       MouseOverShootDist=y-TerWinRect.bottom;
    }

    // calculate mouse line and column position
    if (x<TerWinRect.left) x=TerWinRect.left;

    // convert to logical units
    x=x-TerWinRect.left+TerWinOrgX;
    y=y-TerWinRect.top+TerWinOrgY;

    TerOpFlags|=TOFLAG_SET_MOUSE_ON_TEXT_LINE;   // set mouse is clicked on the text line flag
    MouseLine=UnitsToLine(w,x,y);   // text line position
    ResetLongFlag(TerOpFlags,TOFLAG_SET_MOUSE_ON_TEXT_LINE);
    
    if (MouseLine>=TotalLines) MouseLine=TotalLines-1;

    // check if mouse is over a table top border
    if (MouseOverShoot==' ' && TerArg.PageMode && cid(MouseLine)>0
       && (MouseLine==0 || cid(MouseLine-1)!=cid(MouseLine)
       && IsFirstTableRow(w,cell[cid(MouseLine)].row))
       && ((frm=GetFrame(w,MouseLine))>0)
       && (x>=frame[frm].x && x<=(frame[frm].x+frame[frm].width)) ){
       if (abs(frame[frm].y-y)<TwipsToScrX(RULER_TOLERANCE)) {
          int cl=cid(MouseLine);
          if (!HtmlMode || cell[cl].border&BORDER_TOP) TerOpFlags|=TOFLAG_CURS_ON_TABLE_TOP;
       }
    }


    // fix the current page
    if (SetPage && TerArg.PageMode) {
       if ((CurPage==FirstFramePage) && y>=FirstPageHeight && (CurPage+1)<TotalPages) CurPage++;  // position at next page
       else if ((CurPage==(FirstFramePage+1)) && y<FirstPageHeight && CurPage>0) CurPage--; // position at the previous page
    }

    // check if this line went beyond the screen area
    if ((frm=GetFrame(w,MouseLine))>=0 && frame[frm].ScrLastLine>=0 && MouseLine>frame[frm].ScrLastLine && MouseOverShoot==' ') MouseOverShoot='B';

    FlatX=GetFlatX(w,x,y,MouseLine);

    if (  (MouseLine>=BeginLine)
       && ((MouseLine-BeginLine)<WinHeight)
       && (FlatX<GetRowX(w,MouseLine)) ) {
         MouseCol=-1;
         TerOpFlags|=TOFLAG_X_BEFORE_LINE;
    }
    else if (MousePictFrame>=0 && MousePictFrame<TotalFrames && frame[MousePictFrame].ParaFrameId>0) {
       int ParaFID=frame[MousePictFrame].ParaFrameId;
       int pict=ParaFrame[ParaFID].pict,i,len;
       LPWORD fmt;

       fmt=OpenCfmt(w,MouseLine);
       len=LineLen(MouseLine);
       for (i=0;i<len;i++) if (fmt[i]==pict) break;
       if (i<len) MouseCol=i;
       else       MouseCol=0;
    } 
    else MouseCol=UnitsToCol(w,FlatX,MouseLine); // mouse column position with respect to the beginning of the line

    // round-off the column position
    if (MouseCol>=0) {
       CurFont=GetCurCfmt(w,MouseLine,MouseCol);
       if (!(TerFont[CurFont].style&PICT) && !IsHypertext(w,CurFont)) {
          int MidX=ColToUnits(w,MouseCol,MouseLine,TER_CENTER);
          if (FlatX>MidX) MouseCol++;
       }
    }

    //************** adjust the column when going beyond the line
    if (MouseCol>=LineLen(MouseLine)) {
        MouseCol=LineLen(MouseLine);
        if (TerArg.WordWrap) MouseCol--;
    }

    if (MouseCol<0) MouseCol=0;

    return TRUE;
}

/******************************************************************************
    TerTextPosToPix:
    Return the pixel position at a given text position.
    The 'rel' argument specifies the relative-to value: REL_SCREEN,
    REL_WINDOW,REL_TEXT_BOX. The 'line' and 'col' specify the text location.
    To specifiy absolute value, set col to -1, and specify the absolute value in
    the line parameter.  To specify the current caret location, set line
    to -1.
    The 'x' and 'y' parameters return the position
    in pixel units.  The function return TRUE if successful.
******************************************************************************/
BOOL WINAPI _export TerTextPosToPix(HWND hWnd, int rel, long line, int col, LPINT pX, LPINT pY)
{
    PTERWND w;
    int x,y;
    POINT pt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (rel!=REL_SCREEN && rel!=REL_WINDOW && rel!=REL_TEXT_BOX) return FALSE;

    // get line and column number
    if (line<0) {         // use the current position
       line=CurLine;
       col=CurCol;
    }
    if (col<0) AbsToRowCol(w,line,&line,&col);  // calculate row/col from absolute location

    // *** get the pixel location
    x=ColToUnits(w,col,line,LEFT)-TerWinOrgX;
    y=LineToUnits(w,line)-TerWinOrgY;

    // adjust for relative offset to the window client area
    if (rel==REL_WINDOW || rel==REL_SCREEN) {
       x+=TerWinRect.left;
       y+=TerWinRect.top;
    }

    // translate to the screen units
    if (rel==REL_SCREEN) {
       pt.x=x;
       pt.y=y;
       ClientToScreen(hTerWnd,&pt);
       x=pt.x;
       y=pt.y;
    }

    if (pX) (*pX)=x;
    if (pY) (*pY)=y;

    return TRUE;
}

/******************************************************************************
    TerPixToTextPos:
    Return the text position (line/col) at a given pixel position.
    The 'rel' argument specifies the relative-to value: REL_SCREEN,
    REL_WINDOW,REL_TEXT_BOX.  The x and y position parameter and the 'rel'
    argument together speicifes the pixel position.
    The pLine and pCol receive the the text position.  If pCol has -1 in it or
    if pCol is NULL, then pLine receives the absolute text position.
    The function return TRUE if successful.
******************************************************************************/
BOOL WINAPI _export TerPixToTextPos(HWND hWnd, int rel, int x, int y, LPDWORD pLine, LPINT pCol)
{
    PTERWND w;
    POINT pt;
    long line;
    int col;
    DWORD PrevOpFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    if (rel!=REL_SCREEN && rel!=REL_WINDOW && rel!=REL_TEXT_BOX) return FALSE;

    // translate to the screen units
    if (rel==REL_SCREEN) {
       pt.x=x;
       pt.y=y;
       ScreenToClient(hTerWnd,&pt);
       x=pt.x;
       y=pt.y;
    }

    // adjust for relative offset to the window client area
    if (rel==REL_WINDOW || rel==REL_SCREEN) {
       x-=TerWinRect.left;
       y-=TerWinRect.top;
    }

    // *** add the current window origin
    x+=TerWinOrgX;
    y+=TerWinOrgY;

    // converty x/y to line number
    PrevOpFlags=TerOpFlags2;
    TerOpFlags2|=TOFLAG2_CHECK_DISABLED_FRAMES;  // check disabled frames also
    
    line=UnitsToLine(w,x,y);

    TerOpFlags2=PrevOpFlags;

    col=PosToCol(w,x,y,line);

    if (!pCol || (*pCol)==-1) {   // convert to absolute
       line=RowColToAbs(w,line,col);
       col=-1;
    }

    if (pLine) (*pLine)=line;
    if (pCol)  (*pCol)=col;

    return TRUE;
}

/******************************************************************************
    TerPosLineAtTop:
    Position the speicified line at the top or middle of the window.
******************************************************************************/
BOOL WINAPI _export TerPosLineAtTop(HWND hWnd, long line,BOOL top)
{
    PTERWND w;
    int y,PrevPageHt;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
    // set the line variables
    CurLine=BeginLine=line;
    CurRow=0;
    CurCol=0;

    if (!TerArg.PageMode) {
       if (!top) {
          CurRow=WinHeight/2;
          BeginLine=CurLine-CurRow;
          if (BeginLine<0) BeginLine=0;
       }
       PaintTer(w);
       return TRUE;
    }

    // handle the page mode
    CurPage=GetCurPage(w,CurLine);    // get the page when the line resides

    RefreshFrames(w,TRUE);

    y=LineToUnits(w,CurLine);
    if (!top) {                       // position at the middle of the page
       y-=(TerWinHeight/2);
       if (y<0) y=0;
    }
    
    if (CurPage==(FirstFramePage+1)) PrevPageHt=FirstPageHeight;
    else                             PrevPageHt=0;

    // on the last page, show full bottom of the page
    if (HtmlMode && top && CurPage==(TotalPages-1) && (y-PrevPageHt+TerWinHeight)>GetScrPageHt(w,CurPage)) {
       y=GetScrPageHt(w,CurPage)-TerWinHeight+PrevPageHt;
       if (y<0) y=0;
    }    
    
    
    if (y!=TerWinOrgY) {
       TerWinOrgY=y;
       SetTerWindowOrg(w);
    }


    PaintTer(w);
    return TRUE;
}

/******************************************************************************
    TerSetBkPictId:
    Set the background picture id.  Set the id to 0 to remove any background
    picture.  If the picture id is -1, this function prompts the user to
    enter a picture file (.BMP or .WMF).
******************************************************************************/
BOOL WINAPI _export TerSetBkPictId(HWND hWnd, int id, UINT flag, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (id<0) {
       id=TerInsertPictureFile(hWnd,NULL,TRUE,0,FALSE);
       if (id<0) return FALSE;
    }

    if (id<0 || id>=TotalFonts || !(TerFont[id].InUse)) return FALSE;
    if (!(TerFont[id].style&PICT) && id!=0) return FALSE;

    if (BkPictId) DeleteTerObject(w,BkPictId);   // delete the existing picture
    if (hBkPictBM) DeleteObject(hBkPictBM);  // delete the background picture bitmap
    hBkPictBM=NULL;

    BkPictId=id;
    BkPictFlag=flag;      // BKPICT_STRETCH, BKPICT_TILE or 0

    // resize the picture
    if (BkPictId && BkPictFlag==BKPICT_STRETCH) SetPictSize(w,BkPictId,TerWinHeight,TerWinWidth,TRUE);

    if (repaint) TerRepaint(hWnd,TRUE);

    return TRUE;
}

/******************************************************************************
    TerSetWatermarkPict:
    Set the watermark picture id.  Set the id to 0 to remove any waterground
    picture.  If the picture id is -1, this function prompts the user to
    enter a picture file.
******************************************************************************/
BOOL WINAPI _export TerSetWatermarkPict(HWND hWnd, int id, BOOL wash, BOOL repaint)
{
    PTERWND w;
    int i,pict;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (id<0) {
       id=TerInsertPictureFile(hWnd,NULL,TRUE,0,FALSE);
       if (id<0) return FALSE;
    }

    if (id<0 || id>=TotalFonts || !(TerFont[id].InUse)) return FALSE;
    if (!(TerFont[id].style&PICT) && id!=0) return FALSE;

    WmWashed=wash;

    if (WmParaFID>0) {        // remove the previous frame
       // remove the watermark frame from the frame set
       for (i=0;i<TotalFrames;i++) {
          if (frame[i].ParaFrameId!=WmParaFID) continue;
          frame[i].ParaFrameId=0;
          ResetUintFlag(frame[i].flags,FRAME_WATERMARK);
          break;
       }
       if (i<TotalFrames) {
          pict=ParaFrame[i].pict;
          if (pict>0) DeleteTerObject(w,pict);   // delete the existing picture
          ParaFrame[i].InUse=false;
       }
       WmParaFID=0;
    }

    // create a new watermark frame
    if (id>0) {
       WmParaFID=TerSetPictFrame2(hTerWnd,id,PFRAME_FLOAT,0,0,false);
       if (WmParaFID<=0) return false;
       ParaFrame[WmParaFID].flags|=(PARA_FRAME_WATERMARK|PARA_FRAME_WRAP_THRU);  // flag this frame as watermark frame
       
       if (wash) ApplyPictureBrightnessContrast(w,id,22938,19661);  // use standard brightness=22938 and contast = 19661
       SetPictSize(w,id,TwipsToScrY(TerFont[id].PictHeight),TwipsToScrX(TerFont[id].PictWidth),true);
       XlateSizeForPrt(w,id);              // convert size to printer resolution

       PosWatermarkFrame(w,CurPage);
    }     

    TerArg.modified++;

    RefreshFrames(w,true);     // Create the new frame set

    if (repaint) Repaginate(w,FALSE,TRUE,0,TRUE);

    return TRUE;
}

/******************************************************************************
    TerGetBkPictId:
    Get the background picture id.  This function returns a positive picture id 
    when a background picture exists.
******************************************************************************/
BOOL WINAPI _export TerGetBkPictId(HWND hWnd)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return BkPictId;
}

/******************************************************************************
    TerInsertFootnote:
    This function inserts a footnote character and footnote text at the
    current cursor position.  If the ftext (footnote text) parameter is NULL,
    this function displays a dialog box for user to enter the footnote.
    After the insertion, the cursor is placed after the footnote text.
    The footnote style can be any compbination of BOLD, ITALIC, ULINE, and
    SUPSCR.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerInsertFootnote(HWND hWnd, LPBYTE FnMarker, LPBYTE FnText,UINT style, BOOL repaint)
{
    return TerInsertFootnote2(hWnd,FnMarker,FnText,style,TRUE,repaint);
}
 
/******************************************************************************
    TerInsertFootnote2:
    Insert footenote for endnote.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerInsertFootnote2(HWND hWnd, LPBYTE FnMarker, LPBYTE FnText,UINT style, BOOL IsFootnote, BOOL repaint)
{
    PTERWND w;
    int  i,len1,len2,MarkerFont,TextFont,TrailerFont=0,TextSuperFont,
         inserted,PrevFontId,SuperLen;
    BYTE FootnoteText[MAX_WIDTH+12],FootnoteMarker[12];
    LPBYTE ptr,lead;
    LPWORD fmt;
    BYTE LeadFnText[MAX_WIDTH+1],LeadFnMarker[MAX_WIDTH+1];
    UINT NoteStyle,TextStyle;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (InFootnote) return FALSE;  // already in a footnote

    // if footnote text is NULL accept from the user
    if (!FnMarker || !FnText) {
       if (IsFootnote && !CallDialogBox(w,"FootnoteParam",FootnoteParam,0L)) return FALSE;
       if (!IsFootnote && !CallDialogBox(w,"EndnoteParam",FootnoteParam,0L)) return FALSE;  // use the same dialog proceedure as the footnote

       lstrcpy(FootnoteMarker,TempString);
       lstrcpy(FootnoteText,FootnoteMarker);  // prefix marker to the text
       lstrcat(FootnoteText," ");             // space delimiter
       lstrcat(FootnoteText,TempString1);     // append the text
       FnMarker=FootnoteMarker;
       FnText=FootnoteText;
       if (DlgInt1) style=PROTECT|SUPSCR;
       else         style=PROTECT;
    }
    else {
       lstrcpy(FootnoteText,FnMarker);        // prefix marker to the text
       lstrcat(FootnoteText,FnText);
       FnText=FootnoteText;                   // now points to the local variable
       style=ResetUintFlag(style,FNOTEALL);  // footnote styles not allowed for the text
    }

    if (mbcs) {
       len1=(int)TerSplitMbcs(hTerWnd,FnMarker,FnMarker,LeadFnMarker);
       len2=(int)TerSplitMbcs(hTerWnd,FnText,FnText,LeadFnText);
    }
    else {
       len1=lstrlen(FnMarker);                   // lengths of the strings
       len2=lstrlen(FnText);
    }

    // Create the footnote marker and footnote text font ids
    if (HilightType!=HILIGHT_OFF) HilightType=HILIGHT_OFF;  // turnoff any highlighting
    InputFontId=GetEffectiveCfmt(w);
    
    NoteStyle=(FNOTE|FNOTETEXT|style);
    if (!IsFootnote) NoteStyle|=ENOTETEXT;

    if (TerFont[InputFontId].style&(NoteStyle|style)) {     // create a font without the footer style
       SetTerCharStyle(hTerWnd,NoteStyle|style,FALSE,FALSE);
    }
    if (DefLang!=ENGLISH) InputFontId=SetCurLangFont2(w,InputFontId,(HKL)DefLang);   // use the default language

    PrevFontId=InputFontId;     // save this id

    // create the footnote marker font
    SetTerCharStyle(hTerWnd,FNOTE|style,TRUE,FALSE);    // create the marker font
    MarkerFont=InputFontId;

    // create the footnote text font
    InputFontId=PrevFontId;
    TextStyle=(IsFootnote?(FNOTETEXT):(FNOTETEXT|ENOTETEXT));
    SetTerCharStyle(hTerWnd,TextStyle,TRUE,FALSE);// create font with footnote text style
    TextFont=InputFontId;

    // create the footnote text font with superscript
    if (len2>len1) {
       InputFontId=PrevFontId;
       SetTerCharStyle(hTerWnd,TextStyle|style,TRUE,FALSE);// create font with footnote text style
       TextSuperFont=InputFontId;
    }

    // create the footnote additional text when marker is bigger than one character
    if (false && len1>1) {
       InputFontId=PrevFontId;
       SetTerCharStyle(hTerWnd,FNOTEREST|style,TRUE,FALSE);// create font with footnote text style
       TrailerFont=InputFontId;
    }

    InputFontId=-1;                                     // reset InputFontId

    // insert the footnote marker and text
    MoveLineData(w,CurLine,CurCol,len1+len2,'B');

    ptr=pText(CurLine);
    fmt=OpenCfmt(w,CurLine);
    if (mbcs) lead=OpenLead(w,CurLine);

    // insert the first character of the footnote marker
    inserted=0;                   // number of characters inserted
    ptr[CurCol]=FnMarker[0];
    if (mbcs) lead[CurCol]=LeadFnMarker[0];
    fmt[CurCol]=MarkerFont;
    inserted++;

    // insert the remaining characters of the marker
    for (i=1;i<len1;i++) {
       ptr[CurCol+inserted]=FnMarker[i];
       if (mbcs) lead[CurCol+inserted]=LeadFnMarker[i];
       fmt[CurCol+inserted]=MarkerFont;
       inserted++;
    }

    // insert the superscripted part of the footnote text
    SuperLen=len1;
    for (i=0;i<len1;i++) {
       ptr[CurCol+inserted]=FnText[i];
       if (mbcs) lead[CurCol+inserted]=LeadFnText[i];
       fmt[CurCol+inserted]=TextSuperFont;
       inserted++;
    }

    // insert the regular part of the footnote text
    for (i=0;i<(len2-SuperLen);i++) {
       ptr[CurCol+inserted]=FnText[SuperLen+i];
       if (mbcs) lead[CurCol+inserted]=LeadFnText[SuperLen+i];
       fmt[CurCol+inserted]=TextFont;
       if (DefLang!=ENGLISH) {   // reset font to english for english characters
          BOOL HasLead=(mbcs && LeadFnText[SuperLen+i]!=0);
          if (!HasLead && IsEnglishChar(w,FnText,SuperLen+i,len2)) fmt[CurCol+inserted]=SetCurLangFont2(w,fmt[CurCol+inserted],(HKL)ENGLISH);   // use the default language
       } 
       inserted++;
    }

    CloseCfmt(w,CurLine);
    if (mbcs) CloseLead(w,CurLine);

    CurCol+=(len1+len2);     // place the cursor after the footnote

    ReleaseUndo(w);

    // repaint
    if (!IsFootnote) {         // endnotes 
       LineFlags2(CurLine)|=LFLAG2_ENOTETEXT;
       CreateEndnote(w);
       RequestPagination(w,TRUE);
    }

    if (repaint) PaintTer(w);
    else WordWrap(w,CurLine,30);  // simply rewrap

    return TRUE;
}


/******************************************************************************
    CreateCellDragObj:
    Create table drag object
******************************************************************************/
CreateCellDragObj(PTERWND w,int type,int id,int BorderX, int BorderY1, int BorderY2)
{
    int obj;

    if ((obj=GetDragObjectSlot(w))<0) return FALSE;

    DragObj[obj].InUse=TRUE;
    DragObj[obj].type=type;              // object type
    DragObj[obj].id1=id;                 // object id
    DragObj[obj].id2=-1;
    DragObj[obj].id3=-1;

    DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
    DragObj[obj].HotRect[0].left=BorderX-TwipsToScrX(RULER_TOLERANCE);
    DragObj[obj].HotRect[0].top=BorderY1;
    DragObj[obj].HotRect[0].right=BorderX+TwipsToScrX(RULER_TOLERANCE);
    DragObj[obj].HotRect[0].bottom=BorderY2;

    DragObj[obj].ObjPointCount=2;          // two points in the polygon
    DragObj[obj].ObjPoint[0].x=BorderX;
    DragObj[obj].ObjPoint[0].y=BorderY1;
    DragObj[obj].ObjPoint[1].x=BorderX;
    DragObj[obj].ObjPoint[1].y=BorderY2;

    return TRUE;
}


/******************************************************************************
    CreateRowDragObj:
    Create table drag object
******************************************************************************/
CreateRowDragObj(PTERWND w,int type,int id,int BorderX1, int BorderX2, int BorderY)
{
    int obj;

    if ((obj=GetDragObjectSlot(w))<0) return FALSE;

    DragObj[obj].InUse=TRUE;
    DragObj[obj].type=type;              // object type
    DragObj[obj].id1=id;                 // object id
    DragObj[obj].id2=-1;
    DragObj[obj].id3=-1;

    DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
    DragObj[obj].HotRect[0].left=BorderX1;
    DragObj[obj].HotRect[0].top=BorderY-TwipsToScrY(RULER_TOLERANCE);
    DragObj[obj].HotRect[0].right=BorderX2;
    DragObj[obj].HotRect[0].bottom=BorderY+TwipsToScrY(RULER_TOLERANCE);

    DragObj[obj].ObjPointCount=2;          // two points in the polygon
    DragObj[obj].ObjPoint[0].x=BorderX1;
    DragObj[obj].ObjPoint[0].y=BorderY;
    DragObj[obj].ObjPoint[1].x=BorderX2;
    DragObj[obj].ObjPoint[1].y=BorderY;

    return TRUE;
}


/******************************************************************************
     TerHdrFtrExists:
     This function returns LFLAG_ flags to indicate whether the section or the
     document contains header or footers.  The SectId (sequential section id) can
     be set to SECT_ALL or SECT_CUR to indicate the whole document, or the current
     section
******************************************************************************/
int WINAPI _export TerHdrFtrExists(HWND hWnd, int SectId)
{
    int line,sec=0,result=0,CurPara;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

  
    if (SectId<0) line=CurLine;       // line to start search
    else {
       for (line=0;line<TotalLines;line++) {
          if (sec==SectId) break;              // sequential section found
          if (True(LineFlags(line)&LFLAG_SECT)) sec++;
       }
       if (line==TotalLines) return result;    // section not found 
    } 

    // position  after header/footers
    for (;line<TotalLines;line++) {
       CurPara=pfmt(line);
       if (False(PfmtId[CurPara].flags&PAGE_HDR_FTR)) break;   // out of header/footer
    } 
    if (line==TotalLines) return result;

    // find header/footer
    for (;line>=0;line--) {
       result|=(LineFlags(line)&LFLAG_HDRS_FTRS);
       if (SectId!=SECT_ALL && True(LineFlags(line)&LFLAG_SECT)) break;   // section break found
    } 

    return result;
}
   

/******************************************************************************
     TerGetHdrFtrPos:
     Return if the current cursor position is in a header or footer, or 
     the body text. This function returns the LFLAG_HDR series values.  It
     returns 0 when the cursor is placed in the body text.
******************************************************************************/
int WINAPI _export TerGetHdrFtrPos(HWND hWnd, int line)
{
    PTERWND w;
    int CurPara;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (line<0) line=CurLine;
    if (line<0 || line>=TotalLines) return 0;

    CurPara=pfmt(line);
    if (False(PfmtId[CurPara].flags&PAGE_HDR_FTR)) return 0;  // in the body text

    for (;line<TotalLines;line++) if (True(LineFlags(line)&LFLAG_HDRS_FTRS)) break;
    if (line==TotalLines) return 0;

    return (LineFlags(line)&LFLAG_HDRS_FTRS);
}
   

/******************************************************************************
     TerPosHdrFtr:
     Position at the header/footer text.  This function is available in the
     page mode only.  The section number is 0 for the first section, 1 for the
     next and so on.  The 'header' is TRUE for header and FALSE for the footer.
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosHdrFtr(HWND hWnd, int sect, BOOL header, int pos, BOOL repaint)
{
     return TerPosHdrFtrEx(hWnd,sect,(BYTE)(header?HDR_CHAR:FTR_CHAR),pos,repaint);
}

/******************************************************************************
     TerPosHdrFtrEx:
     Position at the header/footer text.  This function is available in the
     page mode only.  The section number is 0 for the first section, 1 for the
     next and so on.
     The HdrFtr should be set to one of the hdr/ftr delimiter character
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosHdrFtrEx(HWND hWnd, int sect, int HdrFtr, int pos, BOOL repaint)
{
    PTERWND w;
    BOOL SavePaintEnabled,HdrFtrFound=FALSE;
    long l;
    int CurSectNo,col;
    int SectId;
    BOOL FirstHdrFtr=(HdrFtr==FHDR_CHAR || HdrFtr==FFTR_CHAR);
    BOOL NewHdrFtr;
    BYTE CurHdrFtr;
    BOOL created=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (pos!=POS_BEG && pos!=POS_END) return FALSE;  // bad argument

    // toggle the edit page hdr/ftr mode
    SavePaintEnabled=PaintEnabled;
    PaintEnabled=repaint;                            // enable/disable painting
    if (!EditPageHdrFtr) ToggleEditHdrFtr(w);        // toggle the edit mode
    PaintEnabled=SavePaintEnabled;

    LOCATE_SECT:
    CurSectNo=0;
    l=0;
    while (CurSectNo<sect) {
       while (l<TotalLines) {
          if (tabw(l) && tabw(l)->type&INFO_SECT) {
             CurSectNo++;
             l++;                           // go past the section marker
             break;
          }
          l++;
       }
       if (l==TotalLines) break;
    }
    if (l==TotalLines) return FALSE;        // section not found

    // check if first page header/footer is available
    if (FirstHdrFtr) {
       RecreateSections(w);
       SectId=GetSection(w,l);
       NewHdrFtr=(HdrFtr==FHDR_CHAR && TerSect1[SectId].fhdr.FirstLine<0)
              || (HdrFtr==FFTR_CHAR && TerSect1[SectId].fftr.FirstLine<0);
       if (NewHdrFtr) {                     // create new first page header/footer
          if (created) return FALSE;        // creation already attempted
          SavePaintEnabled=PaintEnabled;
          PaintEnabled=repaint;                            // enable/disable painting
          CurLine=l;                        // position inside the section
          CurCol=0;
          TerCreateFirstHdrFtr(hTerWnd,(HdrFtr==FHDR_CHAR));
          PaintEnabled=SavePaintEnabled;
          created=TRUE;
          goto LOCATE_SECT;                 // locate section again
       }
    }


    // locate header or footer starting from the first line of the section
    while (l<TotalLines) {
       if (tabw(l) && tabw(l)->type&INFO_SECT) return FALSE;  // header/footer not found

       // test if header/footer begins
       if (!HdrFtrFound) {
          if      (LineFlags(l)&LFLAG_HDR)  CurHdrFtr=HDR_CHAR;
          else if (LineFlags(l)&LFLAG_FTR)  CurHdrFtr=FTR_CHAR;
          else if (LineFlags(l)&LFLAG_FHDR) CurHdrFtr=FHDR_CHAR;
          else if (LineFlags(l)&LFLAG_FFTR) CurHdrFtr=FFTR_CHAR;
          else                              CurHdrFtr=0;

          if (CurHdrFtr==HdrFtr) {      // header/footer delimiter found
            HdrFtrFound=TRUE;
            if (pos==POS_BEG) {      // position at the line after the delimiter line
               CursDirection=CURS_FORWARD;     // don't let cursor move backward
               SetTerCursorPos(hWnd,l+1,0,repaint);
               return TRUE;
            }
            l++;                     // go past the delimiter line
            continue;
          }
       }

       // locate the end of the header/footer
       if (HdrFtrFound) {
          if (LineFlags(l)&LFLAG_HDRS_FTRS) { // ending delimiter found
              // position before the last character of the previous line
              col=LineLen(l-1)-1;
              if (col<0) col=0;
              CursDirection=CURS_BACKWARD;       // don't let cursor go any farther
              SetTerCursorPos(hWnd,l-1,col,repaint);
              return TRUE;
          }
       }
       l++;
    }

    return FALSE;
}

/******************************************************************************
     TerPosBodyText:
     Position in the body text outside of header/footer.
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosBodyText(HWND hWnd, int sect, int pos, BOOL repaint)
{
    PTERWND w;
    long l;
    int CurSectNo,col;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (pos!=POS_BEG && pos!=POS_END) return FALSE;  // bad argument

    // locate the section
    CurSectNo=0;
    l=0;
    while (CurSectNo<sect) {
       while (l<TotalLines) {
          if (tabw(l) && tabw(l)->type&INFO_SECT) {
             CurSectNo++;
             l++;                                    // go past the section marker
             break;
          }
          l++;
       }
       if (l==TotalLines) break;
    }
    if (l==TotalLines) return FALSE;                 // section not found

    // go past any header footer
    for (;l<TotalLines;l++) if (!(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)) break;
    if (l==TotalLines) return FALSE;
    if (pos==POS_BEG) {
       SetTerCursorPos(hWnd,l,0,repaint);
       return TRUE;
    }

    // position at the end of the section
    for (;l<TotalLines;l++) if (tabw(l) && tabw(l)->type&INFO_SECT) break;
    if (l==TotalLines) l--;
    if (l>=0) {
       col=LineLen(l)-1;
       if (col<0) col=0;
       SetTerCursorPos(hWnd,l,col,repaint);
       return TRUE;
    }

    return FALSE;
}

/******************************************************************************
     TerPosFrame:
     Position in a given frame.
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosFrame(HWND hWnd, int FrameNo, int pos, BOOL repaint)
{
    PTERWND w;
    long l;
    int col;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (pos!=POS_BEG && pos!=POS_END) return FALSE;  // bad argument
    if (FrameNo<1 || FrameNo>=TotalParaFrames) return FALSE;

    // locate the first line of the frame
    for (l=0;l<TotalLines;l++) if (fid(l)==FrameNo) break;
    if (l==TotalLines) return FALSE;
    if (pos==POS_BEG) {      // position at the line after the delimiter line
       SetTerCursorPos(hWnd,l,0,repaint);
       return TRUE;
    }

    // locate the last line of the frame
    for (;l<TotalLines;l++) if (fid(l)!=FrameNo) break;
    l--;
    if (l>=0) {
       col=LineLen(l)-1;
       if (col<0) col=0;
       SetTerCursorPos(hWnd,l,col,repaint);
       return TRUE;
    }

    return FALSE;
}

/******************************************************************************
     ColToUnits:
     Convert the column units to logical units.  The function returns the
     logical unit where a specified column begins. The routine can return
     the logical unit for the beginning of the column (TER_LEFT), or at the
     center of the column (TER_CENTER), or TER_RIGHT
******************************************************************************/
int ColToUnits(PTERWND w,int col,long LineNo,int pos)
{
    int x,i,len,TabNo=0,CharWidth,FrameNo,SpaceCount=0,AdjBef,AdjAft;
    int CurFmt=0;
    LPWORD fmt,pWidth;

    if (LineNo>=TotalLines) return 0;

    pWidth=GetLineCharWidth(w,LineNo);
    if (!pWidth) return 0;

    if (cfmt(LineNo).info.type!=UNIFORM) fmt=cfmt(LineNo).pFmt;

    len=LineLen(LineNo);

    if (cfmt(LineNo).info.type!=UNIFORM) fmt=cfmt(LineNo).pFmt;

    x=GetRowX(w,LineNo);              // get indentation

    for (i=0;i<=col;i++) {
       AdjBef=AdjAft=0;          // space before and after the character

       // add any frame space before the column
       if (tabw(LineNo) && tabw(LineNo)->type&INFO_FRAME && i==tabw(LineNo)->FrameCharPos) x+=tabw(LineNo)->FrameScrWidth;

       if (cfmt(LineNo).info.type==UNIFORM) CurFmt=cfmt(LineNo).info.fmt;
       else if (i<len) CurFmt=fmt[i];

       // find the current character CharWidth
       if (i<len) {
            CharWidth=pWidth[i];
            if (tabw(LineNo) && tabw(LineNo)->CharFlags  // add addtional character space
            && i<tabw(LineNo)->CharFlagsLen) {

                if (tabw(LineNo)->CharFlags[i]&BFLAG_BOX_FIRST) AdjBef=ExtraSpaceScrX;
                if (tabw(LineNo)->CharFlags[i]&BFLAG_BOX_LAST)  AdjAft=ExtraSpaceScrX;
                //CharWidth+=(AdjBef+AdjAft);
            }
       }
       else  CharWidth=DblCharWidth(w,CurFmt,TRUE,' ',0);

       if (i==col) {
          if (pos==TER_CENTER) x+=(AdjBef+(CharWidth-AdjBef-AdjAft)/2);
          if (pos==TER_RIGHT)  x+=(AdjBef+(CharWidth-AdjBef-AdjAft));
          else                 x+=AdjBef;
       }
       else x+=CharWidth;
    }

    // add frame displacement
    if (TerArg.PageMode) {
       FrameNo=GetFrame(w,LineNo);
       if (FrameNo>=0) x+=frame[FrameNo].ScrX;
    }

    MemFree(pWidth);

    return x;
}

/******************************************************************************
     LineToUnits:
     Convert the row units to the logical units.
******************************************************************************/
int LineToUnits(PTERWND w,long line)
{
    if (line==STATUS_ROW) {                // status line
       if (TerArg.BorderMargins) return TerWinOrgY+TerWinRect.bottom-TerWinRect.top+TwipsToOrigScrY(BORDER_MARGIN);
       else                      return TerWinOrgY+TerWinRect.bottom-TerWinRect.top;
    }
    else return GetRowY(w,line); // Text rows
}

/******************************************************************************
     PosToCol:
     Convert the pixel x/y position to the text column position.
******************************************************************************/
int PosToCol(PTERWND w,int x, int y,long line)
{
    x=GetFlatX(w,x,y,line);
    return UnitsToCol(w,x,line);
}

/******************************************************************************
     GetFlatX:
     Convert screen pixel x, y position to the flat x position.
******************************************************************************/
int GetFlatX(PTERWND w,int x, int y,long line)
{
    int i,pict,ParaFID,j,FrameNo,width,height;
    LPWORD fmt;
    BOOL  VCellText=false;

    if (line<0) {
       line=UnitsToLine(w,x,y);   // text line position
       if (line>=TotalLines) line=TotalLines-1;
    }
    
    if (cell[cid(line)].TextAngle!=0) VCellText=true;  

    // check if this location falls in a picture frame
    if (TerArg.PageMode && (ContainsParaFrames || VCellText)) {
       for (i=0;i<TotalFrames;i++) {
          if (frame[i].empty || frame[i].ParaFrameId==0 || !(frame[i].flags&FRAME_PICT)) continue; // not a picture frame
          if (frame[i].flags&FRAME_DISABLED && !(TerOpFlags2&TOFLAG2_CHECK_DISABLED_FRAMES)) continue;

          if   (x>=frame[i].x && x<(frame[i].x+frame[i].width)
            &&  y>=frame[i].y && y<(frame[i].y+frame[i].height)) break;
       }
       if (i<TotalFrames) {
          ParaFID=frame[i].ParaFrameId;
          pict=ParaFrame[ParaFID].pict;
          if (pict<TotalFonts && TerFont[pict].style&PICT && TerFont[pict].FrameType!=PFRAME_NONE) {
             fmt=OpenCfmt(w,line);
             for (j=0;j<LineLen(line);j++) if ((int)fmt[j]==pict) break;
             CloseCfmt(w,line);

             if (j<LineLen(line)) return ColToUnits(w, j, line, TER_LEFT);
          }
       }

       // adjust for vertical frames
       for (i=0;i<TotalFrames;i++) {
          if (frame[i].empty || (frame[i].ParaFrameId==0 && frame[i].CellId==0)) continue; // not a picture frame
          if (frame[i].flags&FRAME_DISABLED && !(TerOpFlags2&TOFLAG2_CHECK_DISABLED_FRAMES)) continue;
          
          if (GetFrameTextAngle(w,i)==0) continue;   // not a rotated frame
          if   (InRotatedFrame(w,x,y,i)) break;
       }

       FrameNo=i;
       if (FrameNo<TotalFrames) {
          int TextAngle,BoxFrame,BeginX,BeginY,width,height;
          
          ParaFID=frame[FrameNo].ParaFrameId;

          BoxFrame=(ParaFID==0)?FrameNo:frame[FrameNo].BoxFrame;
          
          BeginX=frame[BoxFrame].x+frame[BoxFrame].SpaceLeft;
          BeginY=frame[BoxFrame].y+frame[BoxFrame].SpaceTop;
          width=frame[BoxFrame].width-frame[BoxFrame].SpaceLeft-frame[BoxFrame].SpaceRight;
          height=frame[BoxFrame].height-frame[BoxFrame].SpaceTop-frame[BoxFrame].SpaceBot;

          TextAngle=GetFrameTextAngle(w,FrameNo);
          if (TextAngle>0) {
             if (TextAngle==90) 
                   x=BeginX+height-(y-BeginY);          // unrotated x value
             else  x=BeginX+(y-BeginY);
          } 
       }

    }

    // adjust for rtl text
    if (LineFlags2(line)&LFLAG2_RTL) {
       int frm=-1,x1,x2,NewX;

       if (TerArg.PageMode) {
          for (i=0;i<TotalFrames;i++) {
             if (frame[i].empty) continue;
             if (frame[i].flags&FRAME_DISABLED && !(TerOpFlags2&TOFLAG2_CHECK_DISABLED_FRAMES)) continue;
             
             ParaFID=frame[i].ParaFrameId;
             width=frame[i].width;
             height=frame[i].height;
             if (GetFrameTextAngle(w,i)!=0) SwapInts(&width,&height);

             if   (x>=frame[i].x && x<(frame[i].x+width)
                 &&  y>=frame[i].y && y<(frame[i].y+height)) break;
          }
          if (i<TotalFrames) frm=i;
       }
       else {
          frm=0;
          width=frame[frm].width;
       }
       
       if (frm<TotalFrames) {
          int FrameX=frame[frm].x/*+frame[frm].SpaceLeft*/;
          pScrSeg=GetLineSeg(w,line,NULL,&TotalScrSeg);

          NewX=-1;

          for (i=0;i<TotalScrSeg;i++) {      // check if x falls in any othe segments after mapping
             x1=pScrSeg[i].x+FrameX;  // segment start relative to the frame set
             x1=RtlX(w,x1,0,frm,&(pScrSeg[i]));   // shifted start relative to be beginning of the frame set
             
             x2=pScrSeg[i].x+pScrSeg[i].width+FrameX;  // segment end relative to the frame set
             x2=RtlX(w,x2,0,frm,&(pScrSeg[i]));
             
             if (x1>x2) SwapInts(&x1,&x2);

             if (i==0 && x>=x2) {               // end the beginning of the line
                NewX=FrameX;
                break;
             } 
             if ((x>=x1 && x<x2)) {   // found in a segment
                NewX=pScrSeg[i].x+FrameX;
                if (pScrSeg[i].rtl) NewX+=(x2-x);
                else                NewX+=(x-x1);
                break;
             }

             if (i==(TotalScrSeg-1) && x<x1) {   // after the end of the line
                NewX=FrameX+(width-1);
                break;
             } 
          }
          
          if (NewX!=-1) x=NewX;

          MemFree(pScrSeg);
          pScrSeg=NULL;
          TotalScrSeg=CurScrSeg=0; 
       }
    } 

    return x;
}


/******************************************************************************
     UnitsToCol:
     Convert the device units to horrizontal columns.  The routine returns
     the column number pointed by a specified logical unit
******************************************************************************/
int UnitsToCol(PTERWND w,int HorUnits,long line)
{
    int col=0,len,CharWidth,TabNo=0,FrameNo,OrigHorPos,
        SpaceCount=0,CurFmt=0;
    LPWORD fmt,pWidth;

    ResetLongFlag(TerOpFlags,TOFLAG_X_BEYOND_LINE);  // will be set in this function if x is beyond the line width
    ResetLongFlag(TerOpFlags,TOFLAG_X_BEFORE_LINE);  // will be set in this function if x is before the line text

    if (line>=TotalLines || line<0) return 0;

    pWidth=GetLineCharWidth(w,line);
    if (!pWidth) return 0;

    len=LineLen(line);

    if (cfmt(line).info.type!=UNIFORM) fmt=cfmt(line).pFmt;

    HorUnits=HorUnits-GetRowX(w,line);// adjust for indentation
    if (TerArg.PageMode) {            // apply frame displacement
       FrameNo=GetFrame(w,line);
       if (FrameNo>=0) HorUnits=HorUnits-frame[FrameNo].ScrX;
    }
    
    if (HorUnits<0) TerOpFlags|=TOFLAG_X_BEFORE_LINE;

    OrigHorPos=HorUnits;

    while (TRUE) {
       // add any frame space before the column
       if (tabw(line) && tabw(line)->type&INFO_FRAME && col==tabw(line)->FrameCharPos)
            CharWidth=tabw(line)->FrameScrWidth;
       else CharWidth=0;

       if (cfmt(line).info.type==UNIFORM) CurFmt=cfmt(line).info.fmt;
       else if (col<len) CurFmt=fmt[col];

       // find the current character CharWidth
       if (col<len) {
            CharWidth+=pWidth[col];
            if (FALSE && tabw(line) && tabw(line)->CharFlags  // add addtional character space
               && col<tabw(line)->CharFlagsLen) {

               if (tabw(line)->CharFlags[col]&BFLAG_BOX_FIRST) CharWidth+=ExtraSpaceScrX;
               if (tabw(line)->CharFlags[col]&BFLAG_BOX_LAST)  CharWidth+=ExtraSpaceScrX;
            }
       }
       else  {
          if (TerArg.WordWrap) {
             TerOpFlags|=TOFLAG_X_BEYOND_LINE;
             break;
          }
          CharWidth+=DblCharWidth(w,CurFmt,TRUE,' ',0);
       }

       HorUnits-=CharWidth;
       if (HorUnits<0) break;
       col++;
       if (col>=MAX_WIDTH) break;
    }

    if (TerArg.WordWrap) {
       if (col>=LineLen(line) && LineLen(line)>0) col=LineLen(line)-1;
    }
    else {
       if (col>LineLen(line) && LineLen(line)>0) col=LineLen(line);
    }

    MemFree(pWidth);           // free the allcoated pointer

    return col;
}

/******************************************************************************
     GetLineCharWidth:
     Return an array containing the screen width of each character in the given
     line.  The calling function must free the returned pWidth using MemFree.
******************************************************************************/
LPWORD GetLineCharWidth(PTERWND w,long LineNo)
{
    int x,NewX,i,j,len,TabNo=0,CharWidth,FrameNo,SpaceCount=0,AdjBef,AdjAft;
    int CurFmt=0,FrameX,NewScrX,ScrX,ScrCharWidth,SaveWidth;
    LPBYTE ptr,lead;
    BYTE CurChar;
    LPWORD fmt, pWidth,pScrWidth;
    BOOL ScrUnit=(!TerArg.WordWrap || !TerArg.PrintView || TerArg.FittedView);
    int width=0,NextSpacePos;

    if (LineNo>=TotalLines || LineNo<0) return NULL;
    pWidth=MemAlloc(sizeof(WORD)*(LineLen(LineNo)+1));
    pScrWidth=MemAlloc(sizeof(WORD)*(LineLen(LineNo)+1));     // need to provide comparison to the printer units
    if (pWidth==NULL || pScrWidth==NULL) {
       if (pWidth) MemFree(pWidth);
       if (pScrWidth) MemFree(pScrWidth);
       return NULL;
    }

    ptr=pText(LineNo);
    lead=pLead(LineNo);
    len=LineLen(LineNo);

    if (cfmt(LineNo).info.type!=UNIFORM) fmt=cfmt(LineNo).pFmt;

    if (InPrinting) x=0;                 // while printing the x position does not affect the character width
    else {
       x=GetRowX(w,LineNo);              // get indentation
       if (!ScrUnit) {
          int CurPara=pfmt(LineNo);
          if (PfmtId[CurPara].LeftIndent==x) x=TwipsToPrtX(PfmtId[CurPara].LeftIndentTwips); // to reduce rounding error
          else                               x=ScrToPrtX(x);  // convert to printer units
       }
    }

    // check for any assumed tab 
    if (LineFlags(LineNo)&LFLAG_ASSUMED_TAB) TabNo++;

    for (i=0;i<len;i++) {
       AdjBef=AdjAft=0;          // space before and after the character
       CurChar=ptr[i];

       if (cfmt(LineNo).info.type==UNIFORM) CurFmt=cfmt(LineNo).info.fmt;
       else CurFmt=fmt[i];

       // adjust CurChar for caps and scaps
       if ((TerFont[CurFmt].style&(SCAPS|CAPS)) && TerFont[CurFmt].UcBase==0 
           && LeadByte(lead,i)==0 && IsLcChar(CurChar)) CurChar+=(BYTE)('A'-'a');    // convert to upper case

       // find the current character CharWidth
       ScrCharWidth=0;
       if (CurChar==TAB && !HiddenText(w,CurFmt)) {
           CharWidth=GetTabWidth(w,LineNo,TabNo,x);
           TabNo++;
       }
       else if (CurChar==' ' && tabw(LineNo) && tabw(LineNo)->type&INFO_JUST && JustifySpace(w,CurFmt)) {
           CharWidth=DblCharWidth(w,CurFmt,ScrUnit,CurChar,0)+GetSpaceAdj(w,LineNo,SpaceCount);
           SpaceCount++;
       }
       else if (CurChar==HYPH_CHAR && i==(len-1) && !ShowParaMark) {
            CharWidth=DblCharWidth(w,CurFmt,ScrUnit,'-',0);
       }
       else if (CurChar==ZWNJ_CHAR) {
            CharWidth=0;  // zero-width non-joiner
       }
       else if (ShowParaMark && !ScrUnit && (TerFont[CurFmt].style&PICT) && TerFont[CurFmt].FrameType!=PFRAME_NONE) {
            ScrCharWidth=TerFont[CurFmt].CharWidth[PICT_CHAR];
            CharWidth=ScrToPrtX(ScrCharWidth);
       }
       else if (TerFont[CurFmt].style&PICT && HiddenText(w,CurFmt)) {
            ScrCharWidth=CharWidth=0;
       } 
       else {
           if (pCharWidth(LineNo) && CWidth(LineNo,i)&CWIDTH_USED) {  // context sensitive width used
              if (ScrUnit) CharWidth=ScrCharWidth=(CWidth(LineNo,i)&CWIDTH_WIDTH);
              else {
                CharWidth=(CWidth(LineNo,i)&CWIDTH_WIDTH);
                ScrCharWidth=PrtToScrX(CharWidth);
              }
              
              if (!(TerFont[CurFmt].flags&FFLAG_HIDDEN_INFO)) {
                 if (!EditFootnoteText && IsFootnoteStyle(TerFont[CurFmt].style)) CharWidth=ScrCharWidth=0;
                 if (!EditEndnoteText && TerFont[CurFmt].style&ENOTETEXT) CharWidth=ScrCharWidth=0;
              }
           }
           else { 
              ScrCharWidth=DblCharWidth(w,CurFmt,TRUE,CurChar,LeadByte(lead,i));
              if (ScrUnit) CharWidth=ScrCharWidth;
              else         CharWidth=DblCharWidth(w,CurFmt,ScrUnit,CurChar,LeadByte(lead,i));
           }
           

           if ( tabw(LineNo) && tabw(LineNo)->CharFlags  // add addtional character space
              && i<tabw(LineNo)->CharFlagsLen) {
              if (tabw(LineNo)->CharFlags[i]&(BFLAG_BOX_FIRST|BFLAG_BOX_LAST)) {
                  ScrCharWidth+=ExtraSpaceScrX;
                  CharWidth+=(ScrUnit?ExtraSpaceScrX:ExtraSpacePrtX);
              }
           }
       }

       pWidth[i]=CharWidth;
       pScrWidth[i]=ScrCharWidth;
    }

    if (ScrUnit || InPrinting) {
       MemFree(pScrWidth);   // not needed any more
       return pWidth;
    }

    // add frame displacement
    FrameX=0;
    if (TerArg.PageMode) {
       FrameNo=GetFrame(w,LineNo);
       if (FrameNo>=0) FrameX=frame[FrameNo].x;
    }


    // convert to screen units
    x+=FrameX;
    ScrX=PrtToScrX(x);
    width=0;
    NextSpacePos=9999; // next space position (not set yet)

    for (i=0;i<len;i++) {
       if (NextSpacePos==9999 || ptr[i]==' ') {  // find the position of the next space character
          for (j=i+1;j<len;j++) if (ptr[j]==' ') break;
          if (j<len) NextSpacePos=j;
          else       NextSpacePos=-1;            // no more space left in the line
       } 
       
       
       NewX=x+pWidth[i];
       NewScrX=PrtToScrX(NewX);

       SaveWidth=pWidth[i];

       if (NewScrX>=ScrX) pWidth[i]=NewScrX-ScrX;
       else {                          // handle negative width by taking width from the previous character
          int adj=-(NewScrX-ScrX);     // adjust prev char for this much width
          if (i>0) {
            if ((int)pWidth[i-1]>=adj) pWidth[i-1]-=adj; 
            else                       pWidth[i-1]=0;
          }
          pWidth[i]=0;
       }


       if (pWidth[i]<pScrWidth[i]) {     // compensate up to one pixel
          pWidth[i]++;
          NewScrX++;
       }
       
       if (pWidth[i]>pScrWidth[i] && pScrWidth[i]!=0 && pWidth[i]>0) {     // compensate up to one pixel
          if (ptr[i]!=' ') {                 // let space absorb the pending enlargment
             int adj=pWidth[i]-pScrWidth[i]-1;
             pWidth[i]=pScrWidth[i]+1;        // allow only one extra pixel - hoping for the next space to abosrb the remaining enlargement
             NewScrX-=adj;
             if (i>NextSpacePos && adj>2)  { // no more spaces left, adjumemt is mounting up, so start applying one pixel at a time now
                pWidth[i]++;
                NewScrX++;
             }
          }
       }


       if (SaveWidth==0 && pWidth[i]>0) {
          NewScrX-=pWidth[i];
          pWidth[i]=0;         // hidden characters
       }

       x=NewX;
       ScrX=NewScrX;

       width+=pWidth[i];
    }

    MemFree(pScrWidth);


    return pWidth;
}

/******************************************************************************
    GetLineSeg:
    Return an array of rtl/ltr segments for the line.
******************************************************************************/
struct StrLineSeg far *GetLineSeg(PTERWND w,long LineNo, LPWORD pWidthParam, LPINT pTotalLineSeg)
{
   return GetLineSeg2(w,LineNo,pWidthParam,pTotalLineSeg,0,0,NULL,NULL);
}

/******************************************************************************
    GetLineSeg2:
    Return an array of rtl/ltr segments for the line.
******************************************************************************/
struct StrLineSeg far *GetLineSeg2(PTERWND w,long LineNo, LPWORD pWidthParam, LPINT pTotalLineSeg, int len, int CurX, LPBYTE ptr, LPWORD fmt)
{
    LPWORD pWidth,pOrigWidth;
    int i,TotalLineSeg=0,CurFont,StartCol,width,StartX,FrameCharPos;
    struct StrLineSeg far *pSeg=NULL;
    BOOL    CurTextRtl,PrevTextRtl,CurFontRtl,PrevFontRtl,CurFontFNote,PrevFontFNote;
    BOOL   DoBreak;

    if (LineNo>=0) {
       pWidth=(pWidthParam?pWidthParam:GetLineCharWidth(w,LineNo));  // width calculated by the GetLineCharWidth function

       pOrigWidth=(LineFlags2(LineNo)&LFLAG2_RTL)?pCharWidth(LineNo):NULL;

       if (LineInfo(w,LineNo,INFO_FRAME)) FrameCharPos=tabw(LineNo)->FrameCharPos;
       else                               FrameCharPos=-1;

       if (InPrinting) CurX=0;                   // in printing the segment positions are returned relative to the RowBeginX
       else            CurX=GetRowX(w,LineNo);   // indentation

       len=LineLen(LineNo);
       if (len==0) goto END;

       fmt=OpenCfmt(w,LineNo);
       ptr=pText(LineNo);
    }
    else {
       pWidth=pWidthParam;  // width provided by the calling function
       pOrigWidth=NULL;
       FrameCharPos=-1;
    }

    for (i=0;i<=len;i++) {
       if (i<len) {
          CurFont=fmt[i];
          CurTextRtl=pOrigWidth?(pOrigWidth[i]&CWIDTH_RTL):FALSE;
          CurFontRtl=TerFont[CurFont].rtl;
          CurFontFNote=TerFont[CurFont].style&FNOTEALL;
          if (CurFontRtl && !CurTextRtl) {
            int FieldId=TerFont[CurFont].FieldId;
            CurTextRtl=(FieldId==FIELD_DATE || FieldId==FIELD_PRINTDATE);
          }
       }

       if (i==0) {
          PrevTextRtl=CurTextRtl;
          PrevFontRtl=CurFontRtl;
          PrevFontFNote=CurFontFNote;
          StartCol=width=0;
          StartX=CurX;
       } 
       else {
          DoBreak=false;
          if (i==len || (pOrigWidth && CurTextRtl!=PrevTextRtl)
                        || i==FrameCharPos
                        || PrevFontFNote!=CurFontFNote
                        || (PrevFontRtl && (ptr[i-1]=='(' || ptr[i-1]==')'))    // let '(' be in its own segment
                        || (CurFontRtl && (ptr[i]=='(' || ptr[i]==')'))
                        || CurFontRtl!=PrevFontRtl 
                              || (ptr[i]<32 && ptr[i]!=PICT_CHAR)
                              || (ptr[i-1]<32 && ptr[i-1]!=PICT_CHAR) )   DoBreak=true;// characters less than 32 to be placed in their own segment
          if (!DoBreak) {   // examin further break criteria
             BOOL CurNumeric=(ptr[i]>='0' && ptr[i]<='9');
             BOOL PrevNumeric=(ptr[i-1]>='0' && ptr[i-1]<='9');
             if (PrevFontRtl && CurNumeric!=PrevNumeric) DoBreak=true;
          } 
          
          if (DoBreak) {
             // create a segment
             if (TotalLineSeg==0) pSeg=MemAlloc(sizeof(struct StrLineSeg));
             else                 pSeg=MemReAlloc(pSeg,sizeof(struct StrLineSeg)*(TotalLineSeg+1));

             // check if a paragraph frame falls within this frame
             if (StartCol==FrameCharPos) {
                if (InPrinting) StartX+=tabw(LineNo)->FrameSpaceWidth;  // skip over this space
                else            StartX+=PrtToScrX(tabw(LineNo)->FrameSpaceWidth);  // skip over this space
             }

             pSeg[TotalLineSeg].col=StartCol;
             pSeg[TotalLineSeg].count=i-StartCol;
             pSeg[TotalLineSeg].x=StartX;
             pSeg[TotalLineSeg].width=width;
             pSeg[TotalLineSeg].rtl=PrevFontRtl; // (pOrigWidth?PrevTextRtl:PrevFontRtl);
          
             //if (ptr[StartCol]<32 && PrevFontRtl) pSeg[TotalLineSeg].rtl=TRUE;  // for proper caret positioning
             if (ptr[StartCol]<32 && TotalLineSeg>0 /*&& pSeg[TotalLineSeg-1].rtl*/) pSeg[TotalLineSeg].rtl=TRUE;  // for proper caret positioning
          
             if (i==(StartCol+1) && PrevFontRtl) pSeg[TotalLineSeg].rtl=TRUE;  // for proper caret positioning
             if (LineNo>=0 && LineFlags2(LineNo)&LFLAG2_RTL) {
                BOOL IsNumeric=(ptr[StartCol]>='0' && ptr[StartCol]<='9');
                if (!IsNumeric) {
                   if (len==1 || PrevFontRtl) pSeg[TotalLineSeg].rtl=TRUE;
                   if (PrevTextRtl) pSeg[TotalLineSeg].rtl=true;            // 20050928 - honor rtl set by GetWrapCharWidth function while wrppaing
                }
             }

             TotalLineSeg++;

             if (i==len) break;
             PrevTextRtl=CurTextRtl;
             PrevFontRtl=CurFontRtl;
             PrevFontFNote=CurFontFNote;
             StartCol=i;
             StartX=StartX+width;
             width=0;
          } 
       }
       width+=pWidth[i];
    } 
    
    if (LineNo>=0) CloseCfmt(w,LineNo);

    END:
    if (!pWidthParam) MemFree(pWidth);    // temporary varaible

    if (pTotalLineSeg) (*pTotalLineSeg)=TotalLineSeg;

    return pSeg;
}
  
/******************************************************************************
    GetCharSeg:
    Return the segment which contains a given character position.
******************************************************************************/
int GetCharSeg(PTERWND w,long LineNo, int col, int SegCount, struct StrLineSeg far *pSegIn)
{
    struct StrLineSeg far *pSeg;
    int i,result=0;
    
    pSeg=(pSegIn?pSegIn:GetLineSeg(w,LineNo,NULL,&SegCount));

    if (!pSeg || SegCount==0) return 0;

    for (i=SegCount-1;i>=0;i--) if (col>=pSeg[i].col) break;

    result=i;

    if (!pSegIn) MemFree(pSeg);  // release temporary buffer

    return result;
}
 
/******************************************************************************
     UnitsToLine:
     Convert the logical units to the text row number.
******************************************************************************/
long UnitsToLine(PTERWND w,int x, int y)
{
    return UnitsToLine2(w,x,y,-1);
}

/******************************************************************************
     UnitsToLine2:
     Convert the logical units to the text row number.  When 'frm' is non-zero,
     then x,y are assumed to be in unrotated form, so they can be compared
     linearly within the frame.  Note that all non-boxed para-frame frames are 
     generated unrotated (they are rotated display time only).
******************************************************************************/
long UnitsToLine2(PTERWND w,int x, int y, int frm)
{
     int CurX,CurY,i,FrameNo,offset,y1,y2,height,FirstTextFrm=-1,LastTextFrm=-1,
         tol,NearestFrm=-1,NearestDeltaY,DeltaY,x1,x2,PrevX,PrevY,PrevFrame;
     int ParaFID,CurHeight,FrameX;
     long l,LastPageLine,LastLine;
     BOOL FrameFound,BoxFrame=-1,RegFrame=-1;
     int  TextAngle,LimitCell=0;
     BOOL CheckMouseOnTextLine=(TerOpFlags&TOFLAG_SET_MOUSE_ON_TEXT_LINE)?TRUE:FALSE;

     MouseOnTextLine=TRUE;
     MousePictFrame=-1;

     // locate the line
     if (TerArg.PageMode) {  // works for whole page
        if (TerFlags3&TFLAG3_CURSOR_IN_CELL && cid(CurLine)>0) LimitCell=cid(CurLine);

        if (frm>=0) {     // frame already given
           FrameNo=frm;
           goto FRAME_FOUND;
        } 

        // First check the overlay frames
        if (ContainsParaFrames && !LimitCell) {
           FrameFound=FALSE;
           for (i=0;i<TotalFrames;i++) {
              if (frame[i].empty && !(frame[i].flags1&FRAME1_PARA_FRAME_BOX)) continue; // not a text frame
              if (frame[i].ParaFrameId==0) continue; // not a text frame
              if (frame[i].flags&FRAME_DISABLED && !(TerOpFlags2&TOFLAG2_CHECK_DISABLED_FRAMES)) continue;

              ParaFID=frame[i].ParaFrameId;
              if (ParaFrame[ParaFID].flags&PARA_FRAME_RECT 
                  && (ParaFrame[ParaFID].ShapeType==0 || ParaFrame[ParaFID].ShapeType==SHPTYPE_RECT) ) {

                 tol=TwipsToScrY(RULER_TOLERANCE)/2;   // tolerance for rectangle borders
                 if (TerOpFlags&TOFLAG_NO_TOL) tol=0;

                 x1=frame[i].x;
                 y1=frame[i].y;
                 x2=x1+frame[i].width;
                 y2=y1+frame[i].height;
                 if (y>=(y1-tol) && y<=(y2+tol)) { // left and right border check
                    if (x>=(x1-tol) && x<=(x1+tol)) goto ONE_FRAME_FOUND;
                    if (x>=(x2-tol) && x<=(x2+tol)) goto ONE_FRAME_FOUND;
                 }
                 if (x>=(x1-tol) && x<=(x2+tol)) { // top and bottom border checks
                    if (y>=(y1-tol) && y<=(y1+tol)) goto ONE_FRAME_FOUND;
                    if (y>=(y2-tol) && y<=(y2+tol)) goto ONE_FRAME_FOUND;
                 }
              }
              else {
                 if   (InRotatedFrame(w,x,y,i)) goto ONE_FRAME_FOUND;
              }
              continue;

              ONE_FRAME_FOUND:
              if (!FrameFound) {
                 PrevFrame=i;
                 FrameFound=TRUE;
              }
              else if (frame[i].x>PrevX || frame[i].y>PrevY) PrevFrame=i;
              
              if (frame[i].flags1&FRAME1_PARA_FRAME_BOX) BoxFrame=i;
              else                                       RegFrame=i;  // regular frame

              PrevX=frame[i].x;
              PrevY=frame[i].y;
           }

           if (FrameFound) i=PrevFrame;
           if (i<TotalFrames && frame[i].flags&FRAME_PICT) {
              MousePictFrame=i;
              return frame[i].PageFirstLine;
           }
        }
        else i=TotalFrames;

        // check if positioned in a box frame
        if (i==BoxFrame && !(frame[i].flags&FRAME_DISABLED)) {
           if (RegFrame!=-1) i=RegFrame;  // regular frame also found, so use the regular frame instead of the box frame
           else {
              l=frame[BoxFrame].PageLastLine;    // pick the last line of a frame set
              if (LineInfo(w,l,INFO_ROW) && l>0) l--;
              return l;
           }
        }

        // Then check the other frames
        if (i==TotalFrames) {
           for (i=0;i<TotalFrames;i++) {
              if (frame[i].empty || frame[i].ParaFrameId>0) continue; // not a text frame
              if (frame[i].flags&FRAME_DISABLED && !(TerOpFlags2&TOFLAG2_CHECK_DISABLED_FRAMES)) continue;
              if (LimitCell && frame[i].CellId!=LimitCell) continue;

              if   (x>=frame[i].x && x<frame[i].x+frame[i].width
                &&  y>=frame[i].y && y<frame[i].y+frame[i].height) break;
           }
        }

        if (i==TotalFrames) {
           for (i=0;i<TotalFrames;i++) { // try match for Y location only
              if (frame[i].empty || frame[i].ParaFrameId>0) continue;  // empty or para frame
              if (frame[i].flags&FRAME_DISABLED && !(TerOpFlags2&TOFLAG2_CHECK_DISABLED_FRAMES)) continue;
              if (LimitCell && frame[i].CellId!=LimitCell) continue;

              if (y>=frame[i].y && y<frame[i].y+frame[i].height) break;

              // get the first and last text frame on the page
              if (y<FirstPageHeight && i<FirstPage2Frame) {// consider first page frames only
                 if (FirstTextFrm==-1) FirstTextFrm=i;
                 LastTextFrm=i;
              }
              else if (y>=FirstPageHeight && i>=FirstPage2Frame) {// consider second page frames only
                 if (FirstTextFrm==-1) FirstTextFrm=i;
                 LastTextFrm=i;
              }
              // find the nearest frame in the y direction
              DeltaY=abs(y-frame[i].y);
              if (NearestFrm<0) {
                 NearestFrm=i;
                 NearestDeltaY=DeltaY;
              }
              if (DeltaY<NearestDeltaY) {
                 NearestFrm=i;
                 NearestDeltaY=DeltaY;
              }
              DeltaY=abs(frame[i].y+frame[i].height-y);
              if (DeltaY<NearestDeltaY) {
                 NearestFrm=i;
                 NearestDeltaY=DeltaY;
              }
           }
        }
        
        
        if (i==TotalFrames) {
           if (CheckMouseOnTextLine) MouseOnTextLine=FALSE;
           if (LimitCell /*&& NearestFrm<0*/) {   // find the frame with the cell
              for (i=0;i<TotalFrames;i++) if (frame[i].CellId==LimitCell) break;
              if (i<TotalFrames) NearestFrm=i;
           }

           if (NearestFrm>=0) {
              int DeltaTop=abs(frame[NearestFrm].y-y);
              int DeltaBot=abs(frame[NearestFrm].y+frame[NearestFrm].height-y);
              if (DeltaTop<DeltaBot) l=frame[NearestFrm].PageFirstLine;
              else                   l=frame[NearestFrm].PageLastLine;
              if (LineInfo(w,l,INFO_ROW)) l--;
              return l;
           }
           if (FirstTextFrm>=0 && y<frame[FirstTextFrm].y) return frame[FirstTextFrm].PageFirstLine;
           if (LastTextFrm>=0 && y>=(frame[LastTextFrm].y+frame[LastTextFrm].height)) return frame[LastTextFrm].PageLastLine;
           // line not found, return the current line if within the current page
           if (CurPage>=(TotalPages-1)) LastPageLine=TotalLines-1;
           else                         LastPageLine=PageInfo[CurPage+1].FirstLine;
           if (CurLine>=PageInfo[CurPage].FirstLine && CurLine<=LastPageLine) return CurLine;
           // now return the first line of the last frame
           if (LastTextFrm>=0) return frame[LastTextFrm].PageFirstLine;
           return CurLine;
        }

        
        FrameNo=i;
        
        FRAME_FOUND:

        // get the relative Y position within this frame
        ParaFID=frame[FrameNo].ParaFrameId;
        CurY=frame[FrameNo].y+frame[FrameNo].SpaceTop; // frame beginning
        CurX=frame[FrameNo].x+frame[FrameNo].SpaceLeft; // frame beginning
        TextAngle=GetFrameTextAngle(w,FrameNo);

        if (TextAngle==0 || frm>0) y-=CurY;     // relative Y position
        else if (TextAngle==90) {
           FrameX=FrameRotateX(w,frame[FrameNo].x,frame[FrameNo].y,FrameNo); // rotated frame x beginning
           y=x-FrameX;     // distance in the text height direction
        }
        else {
           int width=frame[FrameNo].height;
           FrameX=FrameRotateX(w,frame[FrameNo].x,frame[FrameNo].y,FrameNo)-width; // rotated frame x beginning
           y=width-(x-FrameX);        // distance in the text height direction
           y-=frame[FrameNo].SpaceTop;
        } 

        LastLine=frame[FrameNo].PageLastLine;
        if (LastLine>0 && LastLine<TotalLines && tabw(LastLine) && tabw(LastLine)->type&INFO_ROW) LastLine--;
        
        height=0;
        for (l=frame[FrameNo].PageFirstLine;l<LastLine;l++) {
           if (TableLevel(w,l)!=frame[FrameNo].level) continue;
           CurHeight=ScrLineHeight(w,l,TRUE,FALSE);
           if ((height+CurHeight)>y) break;
           height=height+CurHeight;
        }
     }
     else {                       // works in current screen only
        FrameNo=0;                // default frame
        offset=frame[FrameNo].RowOffset;
        for (l=frame[FrameNo].ScrFirstLine;l<=frame[FrameNo].ScrLastLine;l++) {
           y1=RowY[(int)(offset+l-frame[FrameNo].ScrFirstLine)];
           y2=RowY[(int)(offset+l+1-frame[FrameNo].ScrFirstLine)];
           if (y>=y1 && y<y2) break;
        }
        if (y<RowY[offset]) l=frame[FrameNo].ScrFirstLine;
        if (l>frame[FrameNo].ScrLastLine) {
           l=frame[FrameNo].ScrLastLine;
           if (CheckMouseOnTextLine) MouseOnTextLine=FALSE;
        }
     }

     return l;
}

/******************************************************************************
     TerScrLineHeight:
     Get the pixel height of the specified line on the screen.  This routine takes into
     account the space before and after the paragraph and line spacing.
******************************************************************************/
int WINAPI _export TerScrLineHeight(HWND hWnd,long line)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data
    if (line<0 || line>=TotalLines) return 0;

    return ScrLineHeight(w,line,TRUE,FALSE);
}

/******************************************************************************
     ScrLineHeight:
     Get the pixel height of the line on the screen.  This routine takes into
     account the space before and after the paragraph and line spacing.
******************************************************************************/
int ScrLineHeight(PTERWND w,long lin, BOOL AddSpcBef, BOOL recalc)
{
     int temp,height,SpcBef,SpcAft;

     if (lin>=TotalLines) lin=TotalLines-1;

     if (TerArg.PageMode && !recalc) {
        height=ScrLineHt(lin);
        if (AddSpcBef) height+=GetObjSpcBef(w,lin,TRUE);
     }
     else {
        height=GetLineHeight(w,lin,&temp,NULL);
        GetLineSpacing(w,lin,height,&SpcBef,&SpcAft,TRUE);
        height+=(SpcBef+SpcAft);
     }

     return height;
}

/******************************************************************************
     GetLineHeight:
     Get the maximum pixel height of the text on specified line number.
     The second argument returns the distance of the baseline to the
     top of the character for the biggest font in the line.

     This routine does not take into account space before and after the line.
******************************************************************************/
int GetLineHeight(PTERWND w,long lin,LPINT pBaseHeight, LPINT pExtLead)
{
     return GetLineHeight2(w,lin,pBaseHeight,pExtLead,FALSE);
}

 
/******************************************************************************
     GetLineHeight2:
     Get the maximum pixel height of the text on specified line number.
     The second argument returns the distance of the baseline to the
     top of the character for the biggest font in the line.

     This routine does not take into account space before and after the line.
******************************************************************************/
int GetLineHeight2(PTERWND w,long lin,LPINT pBaseHeight, LPINT pExtLead, BOOL TextOnly)
{
     int len,i,height=0,descent=0,ExtLead=0,CurDescent,LineFont,BaseHeight=0;
     int BigVal=9999,PosOffsetY=BigVal,NegOffsetY=BigVal,OffsetY,CurOffset;
     LPWORD fmt;
     LPBYTE ptr;
     WORD   CurFont;
     BOOL   TextFound,SpaceFound,BreakIgnored;

     // calculate the height of a text line now
     if (pBaseHeight) *pBaseHeight=0;
     if (pExtLead)   *pExtLead=0;


     if (lin>=TotalLines) height=TerFont[0].height;
     else if (cfmt(lin).info.type==UNIFORM) {
        CurFont=cfmt(lin).info.fmt;

        CurDescent=TerFont[CurFont].height-TerFont[CurFont].BaseHeight;
        if (TerFont[CurFont].OffsetVal>0) CurDescent-=TerFont[CurFont].OffsetVal;
        
        OffsetY=TerFont[CurFont].OffsetVal;
        PosOffsetY=NegOffsetY=0;
        if (OffsetY>0) PosOffsetY=OffsetY;
        else           NegOffsetY=-OffsetY;

        BaseHeight=TerFont[CurFont].BaseHeight;
        height=BaseHeight+CurDescent-NegOffsetY;
        
        if (NegOffsetY) BaseHeight-=NegOffsetY;
        if (PosOffsetY) BaseHeight+=PosOffsetY;

        ExtLead=TerFont[CurFont].ExtLead;
     }
     else if (LineLen(lin)>0) {
        fmt=cfmt(lin).pFmt;
        ptr=pText(lin);
        CurFont=fmt[0];
        height=descent=ExtLead=0;
        BaseHeight=0;
        len=LineLen(lin);
        TextFound=SpaceFound=BreakIgnored=FALSE;
        LineFont=0;
        if (len>1 && LineFlags(lin)&(LFLAG_PARA|LFLAG_LINE)) {
           LineFont=fmt[len-1];
           len--; // don't count paramarker for height
           BreakIgnored=TRUE;
        }
        for (i=0;i<len;i++) {
            if (!SpaceFound && (ptr[i]==' ' || ptr[i]==TAB) && TerFont[fmt[i]].height>0) SpaceFound=TRUE;
            if ((ptr[i]==' ' || ptr[i]==TAB) && (i<(len-1) || TextFound)) continue;
            if (TextOnly && TerFont[fmt[i]].style&PICT) continue;
            if (!TextFound || fmt[i]!=CurFont) {
               CurFont=fmt[i];
               if (TerFont[CurFont].BaseHeight>BaseHeight) BaseHeight=TerFont[CurFont].BaseHeight;
               CurDescent=TerFont[CurFont].height-TerFont[CurFont].BaseHeight;
               if (TerFont[CurFont].OffsetVal>0) CurDescent-=TerFont[CurFont].OffsetVal;
               if (CurDescent>descent) descent=CurDescent;
               if (TerFont[CurFont].ExtLead>ExtLead) ExtLead=TerFont[CurFont].ExtLead;

               CurOffset=TerFont[CurFont].OffsetVal;
               if (CurOffset==0) NegOffsetY=PosOffsetY=0;
               else if (CurOffset>0 && CurOffset<PosOffsetY) PosOffsetY=CurOffset;  // minimum positive offset 
               else if (CurOffset<0 && (-CurOffset)<NegOffsetY) NegOffsetY=-CurOffset;  // minimum negative offset 


               if (TerFont[CurFont].height>0) TextFound=TRUE;
            }
        }

        if (PosOffsetY==BigVal) PosOffsetY=0;  // not used
        if (NegOffsetY==BigVal) NegOffsetY=0;  // not used
        if (PosOffsetY && NegOffsetY) PosOffsetY=NegOffsetY=0;  // height adjustment not needed

        height=BaseHeight+descent-NegOffsetY-PosOffsetY;
        if (NegOffsetY) BaseHeight-=NegOffsetY;
        
        if (height==0 && (SpaceFound || BreakIgnored)) {
           //if (SpaceFound) LineFont=0;
           height=TerFont[LineFont].height;
           BaseHeight=TerFont[LineFont].BaseHeight;
           ExtLead=TerFont[LineFont].ExtLead;
        }
     }
     else {
        height=TerFont[0].height;
        BaseHeight=TerFont[0].BaseHeight;
        ExtLead=TerFont[0].ExtLead;
     }

     if (pExtLead) (*pExtLead)=ExtLead;           // pass the external
     if (pBaseHeight) (*pBaseHeight)=BaseHeight;  // pass the base height
     
     return height;
}

/******************************************************************************
     GetTextHeight:
     Get the height of the give text.  This text does not belong to any
     particular line.
******************************************************************************/
int GetTextHeight(PTERWND w,LPBYTE ptr, LPWORD fmt, int len, BOOL screen, LPINT pBaseHeight, LPINT pExtLead)
{
     int    i,height=0,descent=0,ExtLead=0,BaseHeight=0;
     WORD   CurFont=0;

     // calculate the height of a text line now
     if (pBaseHeight) *pBaseHeight=0;
     if (pExtLead)   *pExtLead=0;

     for (i=0;i<len;i++) {
         if (i>0 && (ptr[i]==ParaChar || ptr[i]==LINE_CHAR)) continue;

         if (fmt[i]!=CurFont || i==0) {
            CurFont=fmt[i];
            if (screen) {
               if (TerFont[CurFont].BaseHeight>BaseHeight) BaseHeight=TerFont[CurFont].BaseHeight;
               if (TerFont[CurFont].height-TerFont[CurFont].BaseHeight>descent)
                  descent=TerFont[CurFont].height-TerFont[CurFont].BaseHeight;
               if (TerFont[CurFont].ExtLead>ExtLead) ExtLead=TerFont[CurFont].ExtLead;
            }
            else {
               if (PrtFont[CurFont].BaseHeight>BaseHeight) BaseHeight=PrtFont[CurFont].BaseHeight;
               if (PrtFont[CurFont].height-PrtFont[CurFont].BaseHeight>descent)
                  descent=PrtFont[CurFont].height-PrtFont[CurFont].BaseHeight;
               if (PrtFont[CurFont].ExtLead>ExtLead) ExtLead=PrtFont[CurFont].ExtLead;
            }
         }
     }

     height=BaseHeight+descent;

     if (pBaseHeight) (*pBaseHeight)=BaseHeight;
     if (pExtLead) (*pExtLead)=ExtLead;

     return height;
}

/******************************************************************************
    GetRowX:
    Get the indentation of a given row in the window
******************************************************************************/
int GetRowX(PTERWND w,long lin)
{
   int FrameNo,offset;


   FrameNo=GetFrame(w,lin);
   if (FrameNo<0) return PrtToScrX(JustAdjX(lin));          // line not on the screen

   if (lin<frame[FrameNo].ScrFirstLine || lin>frame[FrameNo].ScrLastLine) {
      int x;
      int CurPara=pfmt(lin);
      x=PfmtId[CurPara].LeftIndent;
      if (TerArg.PageMode && BorderShowing) x+=frame[FrameNo].SpaceLeft;
      return (x+PrtToScrX(JustAdjX(lin)));
   }

   offset=frame[FrameNo].RowOffset;  // offset in the row table used

   return RowX[(int)(offset+lin-frame[FrameNo].ScrFirstLine)];
}

/******************************************************************************
    GetRowY:
    Get the Y position of a given row in the window
******************************************************************************/
int GetRowY(PTERWND w,long lin)
{
   int FrameNo,offset,y;
   long l,SaveLine=lin;

   if (lin<0 || lin>=TotalLines) return 0;

   if (TerArg.PageMode) {
      // adjust for header/footer control lines
      if (LineFlags(lin)&(LFLAG_HDR|LFLAG_FHDR)) {
        if ((lin+1)<TotalLines && PfmtId[pfmt(lin+1)].flags&PAGE_HDR) lin++;
        else if (lin>0)                                               lin--;
      }
      if (LineFlags(lin)&(LFLAG_FTR|LFLAG_FFTR)) {
        if ((lin+1)<TotalLines && PfmtId[pfmt(lin+1)].flags&PAGE_FTR) lin++;
        else if (lin>0)                                               lin--;
      }

      FrameNo=GetFrame(w,lin);
      if (FrameNo<0) return 0;          // line not on the screen

      y=frame[FrameNo].y+frame[FrameNo].SpaceTop;
      for (l=frame[FrameNo].PageFirstLine;l<SaveLine;l++) {
         if (TableLevel(w,l)!=frame[FrameNo].level) continue;
         y+=ScrLineHeight(w,l,TRUE,FALSE);
      }
      return (y+GetObjSpcBef(w,SaveLine,TRUE));
   }
   else {
      FrameNo=0;
      if (lin<frame[FrameNo].ScrFirstLine) lin=frame[FrameNo].ScrFirstLine;
      if (lin>frame[FrameNo].ScrLastLine)  lin=frame[FrameNo].ScrLastLine;

      offset=frame[FrameNo].RowOffset;  // offset in the row table used

      return RowY[(int)(offset+lin-frame[FrameNo].ScrFirstLine)];
   }
}

/******************************************************************************
    GetRowHeight:
    Get the height of a given row in the window
******************************************************************************/
int GetRowHeight(PTERWND w,long lin)
{
   int FrameNo,offset;

   FrameNo=GetFrame(w,lin);
   if (FrameNo<0) return 0;          // line not on the screen

   if (lin<frame[FrameNo].ScrFirstLine || lin>frame[FrameNo].ScrLastLine) return 0;

   offset=frame[FrameNo].RowOffset;  // offset in the row table used

   return RowHeight[(int)(offset+lin-frame[FrameNo].ScrFirstLine)];

}

/******************************************************************************
    GetLineSpacing:
    Get the space before and after a line.  This function calculates the
    TextHeight if a zero value is specified.
******************************************************************************/
GetLineSpacing(PTERWND w,long lin, int TextHeight, int far *SpcBef, int far *SpcAft, BOOL screen)
{
    int SkipInt;
    return GetLineSpacing2(w,lin,TextHeight,SpcBef,SpcAft,&SkipInt,&SkipInt,screen);
} 

GetLineSpacing2(PTERWND w,long lin, int TextHeight, int far *SpcBef, int far *SpcAft, LPINT pParaSpcBef, LPINT pParaSpcAft, BOOL screen)
{
   int height;

   if (SpcBef) (*SpcBef)=0;     // initialize the return values
   if (SpcAft) (*SpcAft)=0;     // initialize the return values

   if (TextHeight==0) {
      TextHeight=GetLineHeight(w,lin,&height,NULL);
      if (!screen) TextHeight=ScrToPrtY(TextHeight);
   }
   if (TextHeight==0) return TRUE;  // hidden line

   return GetLineSpacingAlt(w,lin,TextHeight,SpcBef,SpcAft,pParaSpcBef,pParaSpcAft,screen);
}

/******************************************************************************
    GetLineSpacingAlt:
    Get the space before and after a line.  This function expects a valid
    value in the TextHeight parameter.
******************************************************************************/
GetLineSpacingAlt(PTERWND w,long lin, int TextHeight, int far *SpcBef, int far *SpcAft, LPINT pParaSpcBef, LPINT pParaSpcAft, BOOL screen)
{
   int i,bef=0,aft=0,CurPara,height,BorderMargin,space,adj,adj1,adj2;
   LPWORD fmt;
   BOOL LineHasPict;

   if (SpcBef) (*SpcBef)=0;     // initialize the return values
   if (SpcAft) (*SpcAft)=0;     // initialize the return values
   
   (*pParaSpcBef)=0;
   (*pParaSpcAft)=0;

   if (TextHeight==0) return TRUE;  // hidden line

   CurPara=pfmt(lin);

   // calculate space between the lines
   if (PfmtId[CurPara].SpaceBetween!=0) {   // space between the lines
      if (screen) height=TwipsToScrY(PfmtId[CurPara].SpaceBetween);
      else        height=TwipsToPrtY(PfmtId[CurPara].SpaceBetween);
      if (height>0) {    // minimum height specified
         if (height>TextHeight) bef+=(height-TextHeight);
      }
      else {             // exact height specification
         height=-height;
         adj=(height-TextHeight);
         adj1=adj/2;    // spread it between 'before' and 'after'
         adj2=adj-adj1;
         bef+=adj1;
         aft+=adj2;
      }
   }
   else if (PfmtId[CurPara].LineSpacing!=0) {        // multiple line spacing
      int JustTextHeight;
      LineHasPict=FALSE;
      fmt=OpenCfmt(w,lin);
      for (i=0;i<LineLen(lin);i++) if (TerFont[fmt[i]].style&PICT) LineHasPict=TRUE;

      if (LineHasPict) {
         JustTextHeight=GetLineHeight2(w,lin,NULL,NULL,TRUE);
         if (!screen) JustTextHeight=ScrToPrtY(JustTextHeight);
      }
      else JustTextHeight=TextHeight;

      aft+=MulDiv(JustTextHeight,PfmtId[CurPara].LineSpacing,100);
   } 
   else if (PfmtId[CurPara].flags&DOUBLE_SPACE) aft+=TextHeight;

   // add space before and after
   if (TRUE || !(LineFlags(lin)&LFLAG_HTML_RULE)) {
      int SpaceAfter=0;

      // add space before the paragraph
      if (LineFlags(lin)&LFLAG_BOX_TOP) BorderMargin=PfmtId[CurPara].BorderSpace+PARA_BORDER_WIDTH;
      else                              BorderMargin=0;
      if (PfmtId[CurPara].flags&PARA_BOX_DOUBLE && BorderMargin>0) BorderMargin+=(PARA_BORDER_WIDTH/2);  // increase the border margin
      space=PfmtId[CurPara].SpaceBefore+BorderMargin;
      if (space>0 && LineFlags(lin)&LFLAG_PARA_FIRST) {
         if (screen) height=TwipsToScrY(space);
         else        height=TwipsToPrtY(space);
         bef+=height;
         if (True(LineFlags(lin)&(LFLAG_SHADE_BEGIN|LFLAG_BOX_TOP))) (*pParaSpcBef)=height;
      }

      // add space after the paragraph
      if ((LineFlags(lin)&LFLAG_BOX_BOT) || (LineFlags2(lin)&LFLAG2_BOX_BETWEEN)) 
            BorderMargin=PfmtId[CurPara].BorderSpace+PARA_BORDER_WIDTH;
      else  BorderMargin=0;
      if (PfmtId[CurPara].flags&PARA_BOX_DOUBLE && BorderMargin>0) BorderMargin+=(PARA_BORDER_WIDTH/2);  // increase the border margin
      
      SpaceAfter=PfmtId[CurPara].SpaceAfter;
      if ((lin+1)<TotalLines && True(LineFlags(lin)&LFLAG_PARA)) {  // space-after applys only when it is more than next para's space-before
         int NextPara=pfmt(lin+1);
         int NextSpaceBefore=PfmtId[NextPara].SpaceBefore;
         if (SpaceAfter>NextSpaceBefore) SpaceAfter=(SpaceAfter-NextSpaceBefore);
         else                            SpaceAfter=0;
      } 

      space=SpaceAfter+BorderMargin;
      if (space>0 && LineFlags(lin)&LFLAG_PARA) { // space after the line
         if (screen) height=TwipsToScrY(space);
         else        height=TwipsToPrtY(space);
         aft+=height;
         if (True(LineFlags(lin)&(LFLAG_SHADE_END|LFLAG_BOX_BOT))) (*pParaSpcAft)=height;
      }
   }

   // set line spacing for the parent cell marker line
   if (lin>0 && LineLen(lin)==1 && LineInfo(w,lin,INFO_CELL) && TableLevel(w,lin-1)>TableLevel(w,lin)) {  // only cell marker on the last line, and subtable before this line
       int cl=cid(lin);
       if (screen) height=TwipsToScrY(cell[cl].margin);  // leave the margin space at the end
       else        height=TwipsToPrtY(cell[cl].margin);
       adj=(height-TextHeight);
       adj1=adj/2;    // spread it between 'before' and 'after'
       adj2=adj-adj1;
       bef=adj1;
       aft=adj2;
   } 

   if (SpcBef) *SpcBef=bef;
   if (SpcAft) *SpcAft=aft;

   return TRUE;
}

/******************************************************************************
    JustifySpace:
    This routine returns TRUE if the space with the given font can be a justification space.
******************************************************************************/
BOOL JustifySpace(PTERWND w,int CurFont)
{
    return !HiddenText(w,CurFont);
}

/******************************************************************************
    GetFrame:
    This routine returns the text frame that includes the current line
******************************************************************************/
int GetFrame(PTERWND w,long lin)
{
    int i,FrameNo=-1,frm;
    BOOL InHdrFtr=FALSE;

    if (!TerArg.PageMode) return 0;    // default frame

    // header/footer line is attempted to be located in the current page
    if (lin>=0 && lin<TotalLines && PfmtId[pfmt(lin)].flags&PAGE_HDR_FTR) InHdrFtr=TRUE;

    // try quick search first
    if (!InHdrFtr && lin>=0 && lin<TotalLines) {
       frm=LineFrame(lin);
       if (frm==-1) return FrameNo;
       if (frm<0 || frm>=TotalFrames) goto DETAIL_SEARCH;  
       if (frame[frm].empty || True(frame[frm].flags&FRAME_DISABLED)) goto DETAIL_SEARCH;   // not a text frame
       if (True(frame[frm].flags&FRAME_PICT)) goto DETAIL_SEARCH;   // skip picture frames, the frame of the anchor line is returned instead
       if (lin>=frame[frm].PageFirstLine && lin<=frame[frm].PageLastLine) return frm;
    } 

    // detail search
    DETAIL_SEARCH:
    for (i=0;i<TotalFrames;i++) {
       if (frame[i].empty) continue;   // not a text frame
       if (True(frame[i].flags&FRAME_PICT)) continue;   // skip picture frames, the frame of the anchor line is returned instead

       if (lin>=frame[i].PageFirstLine && lin<=frame[i].PageLastLine) {
          if (InHdrFtr) {
             FrameNo=i;                // save this frame number
             if (CurPage==FirstFramePage && frame[i].y<FirstPageHeight) return i;
          }
          else return i;
       }
    }

    return FrameNo;
}

/******************************************************************************
    TerWrapWidth:
    Calculate the max pixel length of a wrapped line. set the sect to -1 to
    use calculate the current section.
******************************************************************************/
int TerWrapWidth(PTERWND w,long lin, int sect)
{
    return TerWrapWidth2(w,lin,sect,TRUE);
}

/******************************************************************************
    TerWrapWidth:
    Calculate the max pixel length of a wrapped line. set the sect to -1 to
    use calculate the current section.
******************************************************************************/
int TerWrapWidth2(PTERWND w,long lin, int sect, BOOL screen)
{
    int width;
    int CellId,ParaFID;
    BOOL WrapToCtlWidth=True(TerFlags6&TFLAG6_WRAP_AT_WINDOW_WIDTH);

    if (sect<0 && lin<0) return (screen?TerWinWidth:ScrToPrtX(TerWinWidth));

    if (TerArg.WordWrap && TerArg.PrintView) {
        // apply table cell width
        if (lin>=0) {
           if ((CellId=cid(lin))>0) {// use the cell width
              int TextAngle=cell[CellId].TextAngle;
              if (TextAngle>0 && fid(lin)>0 && ParaFrame[fid(lin)].TextAngle>0) TextAngle=0;
              
              if (TextAngle==0) width=cell[CellId].width-(2*cell[CellId].margin);
              else {
                 int row=cell[CellId].row;
                 width=PrtToTwipsX(TableRow[row].height);
                 if (TableRow[row].MinHeight<0) width=-TableRow[row].MinHeight;
                 else if (TableRow[row].MinHeight>width) width=TableRow[row].MinHeight;
              }
              if (screen) width=TwipsToScrX(width);              // convert to screen units
              else        width=TwipsToPrtX(width);              // convert to screen units
           }
           else if (fid(lin)>0) {
              int margin;
              ParaFID=fid(lin);
              margin=ParaFrame[ParaFID].margin;
              if ((ParaFrame[ParaFID].flags&PARA_FRAME_TEXT_BOX)) margin+=ParaFrame[ParaFID].LineWdth;
              if (ParaFrame[ParaFID].TextAngle>0) 
                   width=ParaFrame[ParaFID].height-2*margin;
              else width=ParaFrame[ParaFID].width-2*margin;
              
              if (screen) width=TwipsToScrX(width);
              else        width=TwipsToPrtX(width);
           }
           else if (PfmtId[pfmt(lin)].flags&PAGE_HDR_FTR) {   // use the section column width
              if (sect<0) sect=GetSection(w,lin);                // get the current section number
              if (screen) width=(int)(ScrResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));
              else        width=(int)(PrtResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));
           }
           else {                                    // use the section column width
              if (sect<0) sect=GetSection(w,lin);                // get the current section number
              if ((TerArg.FittedView || WrapToCtlWidth) && TerWinWidth>0) width=(screen?TerWinWidth:ScrToPrtX(TerWinWidth));
              else {
                if (screen) width=(int)(ScrResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));
                else        width=(int)(PrtResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));
              }
              if (screen) {
                 width=width-(int)(ScrResX*(TerSect[sect].ColumnSpace*(TerSect[sect].columns-1)));
                 //if (width<ScrResX) width=ScrResX;
              }
              else {
                 width=width-(int)(PrtResX*(TerSect[sect].ColumnSpace*(TerSect[sect].columns-1)));
                 //if (width<ScrToPrtX(ScrResX)) width=ScrToPrtX(ScrResX);
              } 
              
              width=width/TerSect[sect].columns;
           }
        }
        else {
           if (sect<0) sect=0;
           if (TerArg.FittedView && TerWinWidth>0) width=(screen?TerWinWidth:ScrToPrtX(TerWinWidth));
           else {
              if (screen) width=(int)(ScrResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));
              else        width=(int)(PrtResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin));
           }
        }
        return width;
    }
    else {                                         // regular word wrap mode
       if (sect<0 && lin<0) return (screen?TerWinWidth:ScrToPrtX(TerWinWidth));

       if (WrapWidthTwips>0) {
          if (sect<0) sect=GetSection(w,lin);                // get the current section number
          
          if (screen) {
             width=TwipsToScrX(WrapWidthTwips)-(int)(ScrResX*(TerSect[sect].LeftMargin+TerSect[sect].RightMargin));
             if (width<ScrResX) width=ScrResX;
          }
          else {
             width=TwipsToPrtX(WrapWidthTwips)-(int)(PrtResX*(TerSect[sect].LeftMargin+TerSect[sect].RightMargin));
             if (width<ScrToPrtX(ScrResX)) width=ScrToPrtX(ScrResX);
          } 
          return width;
       }
       if (WrapWidthChars>0) {                     // specified in terms of number of characters
          return (DblCharWidth(w,0,screen,'M',0)*WrapWidthChars);
       }
       else {
          if (TerWinWidth<ScrResX) width=ScrResX;    // minimum wrap width 1 inch
          else                     width=TerWinWidth;// width of word wrapped lines
          if (!screen) width=ScrToPrtX(width);
          return width;
       }
    }
}

/******************************************************************************
    TerGetLineWidth:
    This function returns the line width in twips.  The function returns
    -1 to indicate an error condition.
******************************************************************************/
int WINAPI _export TerGetLineWidth(HWND hWnd, long LineNo)
{
    PTERWND w;
    int width;
      
    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    width=GetLineWidth(w,LineNo,false,false);
    return PrtToTwipsX(width);
}

/******************************************************************************
     GetLineWidth:
     Get the pixel width of the specified line number.
     When IncludeDelim is FALSE, the ending para/cell marker or any ending
     space/tabs are exclude from the length.
******************************************************************************/
int GetLineWidth(PTERWND w,long lin,BOOL IncludeDelim, BOOL screen)
{
     int i,width=0,TabNo=0,SpaceCount=0,len;
     LPWORD fmt;
     LPBYTE ptr,lead;
     WORD  CurFont;
     LPWORD pWidth=NULL;

     if (lin>=TotalLines) return 0;
     if (LineLen(lin)==0) return 0;
     if (!TerArg.PrintView) screen=TRUE;

     ptr=pText(lin);
     lead=pLead(lin);

     // adjust the line length for the delimiter
     len=LineLen(lin);
     if (len>0 && !IncludeDelim && ptr[len-1]==SECT_CHAR) len--;
     if (len>0 && !IncludeDelim && (ptr[len-1]==ParaChar || ptr[len-1]==CellChar || ptr[len-1]==LINE_CHAR)) len--;
     if (!IncludeDelim) {    // exclude ending spaces or tabs
        while (len>0) {
           if (ptr[len-1]!=' ' /*&& ptr[len-1]!=TAB*/) break;
           if (cfmt(lin).info.type==UNIFORM) CurFont=cfmt(lin).info.fmt;
           else                              CurFont=cfmt(lin).pFmt[len-1];
           if (TerFont[CurFont].FieldId!=0) break;
           len--;
        }
     }
     if (len==0) return 0;

     if (screen && (NULL==(pWidth=GetLineCharWidth(w,lin)))) return 0;

     // check for any assumed tab 
     if (LineFlags(lin)&LFLAG_ASSUMED_TAB) TabNo++;

     if (cfmt(lin).info.type==UNIFORM) {
        CurFont=cfmt(lin).info.fmt;
        for (i=0;i<len;i++) {
           if (screen) width+=pWidth[i];
           else        width+=DblCharWidth(w,CurFont,screen,ptr[i],LeadByte(lead,i));
        }
     }
     else {
        fmt=cfmt(lin).pFmt;
        CurFont=fmt[0];

        for (i=0;i<len;i++) {
            int PrevWidth=width;
            if (screen) {
               width+=pWidth[i];
               continue;
            }

            if (fmt[i]!=CurFont) CurFont=fmt[i];

            if (ptr[i]==TAB) {
               int delta=GetTabWidth(w,lin,TabNo,PrtToScrX(width));
               width+=delta;
               TabNo++;
            }
            else if (ptr[i]==' ' && tabw(lin) && tabw(lin)->type&INFO_JUST && JustifySpace(w,CurFont)) {
               width=width+DblCharWidth(w,CurFont,screen,ptr[i],0)+GetSpaceAdj(w,lin,SpaceCount);
               SpaceCount++;
            }
            else width+=DblCharWidth(w,CurFont,screen,ptr[i],LeadByte(lead,i));
        }
     }

     if (pWidth) MemFree(pWidth);

     return width;
}

/******************************************************************************
     SetYOrigin:
     Set Y Origin of the windoe to include the given line at the given distance
     from the top of the window.
******************************************************************************/
SetYOrigin(PTERWND w,long line)
{
    int FrameNo,y,NewOrgY;
    long l;
    BOOL ScrollToBottom=FALSE;

    FrameNo=GetFrame(w,line);
    if (FrameNo<0) return FALSE;

    // get Y position of the current line
    y=frame[FrameNo].y+frame[FrameNo].SpaceTop;
    for (l=frame[FrameNo].PageFirstLine;l<line;l++) {
       if (TableLevel(w,l)!=frame[FrameNo].level) continue;
       y=y+ScrLineHeight(w,l,TRUE,FALSE);
    }

    // adjust CurLineY
    if (CurLineY>(TerWinHeight-ScrLineHeight(w,line,TRUE,FALSE)))     ScrollToBottom=TRUE;  // CurLineY way past the bottom
    if (y>(TerWinOrgY+TerWinHeight-ScrLineHeight(w,line,TRUE,FALSE))) ScrollToBottom=TRUE;  // CurLine way past the bottom
    
    if (ScrollToBottom) CurLineY=TerWinHeight-ScrLineHeight(w,line,TRUE,FALSE);  // scroll the cursor to the bottom of the screen
    
    if (CurLineY<0) CurLineY=0;
    if (y<CurLineY) CurLineY=y;

    NewOrgY=y-CurLineY;

    if (TerWinOrgY!=NewOrgY) {
       TerWinOrgY=NewOrgY;
       SetTerWindowOrg(w);
    }

    return TRUE;
}

/******************************************************************************
     GetCurPage:
     This routine returns the page number for the given line
******************************************************************************/
int GetCurPage(PTERWND w,long LineNo)
{
    int i;

    if (LineNo>=TotalLines) return (CurPage=TotalPages-1);

    // check if current line belongs to header/footer and is in the frame set
    if ( TerArg.PageMode && ViewPageHdrFtr
       && PfmtId[pfmt(LineNo)].flags&PAGE_HDR_FTR ) {
       for (i=0;i<TotalFrames;i++) if (!frame[i].empty && LineNo>=frame[i].PageFirstLine && LineNo<=(frame[i].PageLastLine+1)) break;  // allow one additional line for word wrapping
       if (i<TotalFrames) return CurPage;
    }

    CurPage=PageFromLine(w,LineNo,-1);

    return CurPage;
}

/******************************************************************************
    TerSetMaxUndo:
    Set max undo limit.
******************************************************************************/
BOOL WINAPI _export TerSetMaxUndo(HWND hWnd, int NewMaxUndoLimit)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (NewMaxUndoLimit>=MaxUndos) MaxUndoLimit=NewMaxUndoLimit;    // don't go below the already allocated size

    return TRUE;
}


/******************************************************************************
    SaveUndo:
    Save a specified text before it is deleted.
*******************************************************************************/
void SaveUndo(PTERWND w,long BegLine,int BegCol,long EndLine,int EndCol,BYTE type)
{
    int  idx,LastUndo;
    BOOL result=TRUE;

    if (InUndo || UndoRef==UndoSkipRef || (TerFlags3&TFLAG3_NO_SAVE_UNDO) || (TerOpFlags2&TOFLAG2_NO_SAVE_UNDO)) return;         // doing an undo operation


    ReleaseRedo(w);            // release any redos



    // fix end position
    if (EndCol<0 && EndLine>BegLine && LineLen(EndLine)>0) {
       EndLine--;
       EndCol=LineLen(EndLine)-1;
    } 
    
    // add to previous undo if in the typing mode
    LastUndo=UndoCount-1;

    if (LastUndo>=0 && undo[LastUndo].type=='I' && type=='I'
       && BegLine==EndLine && BegCol==EndCol
       && !(TerFlags4&TFLAG4_UNDO_WINDOW_OVERFLOW)
       && !(TerOpFlags&TOFLAG_BREAK_CHAR)) {   // add the current character to the undo
       long CurUndoEnd=RowColToAbs(w,EndLine,EndCol);
       if (CurUndoEnd==(undo[LastUndo].end+1)) {
          int PrevCfmt=GetPrevCfmt(w,BegLine,BegCol);
          if (False(TerFont[PrevCfmt].style&PICT)) {
             undo[LastUndo].end++;                 // add another character to the undo
             FreeOneUndo(w,LastUndo);
             return;
          }
       }
    }

    SendMessageToParent(w,TER_SAVE_UNDO,(WPARAM)hTerWnd,UndoRef,FALSE);

    if (type=='O') type='I';   // now treat object insertion same as character insertion


    // Make room for the new undo
    if (UndoCount==MaxUndos) {
       ScrollUndo(w);               // make room for new undo
       if (UndoSkipRef==UndoRef) {  // this undo was partially scrolled of - so skip the whole series
          UndoTblSize=UndoCount;  // discard any remaining redos
          return;
       } 
    }
    idx=UndoCount;                             // index of the latest undo

    FarMemSet(&(undo[idx]),0,sizeof(struct StrUndo));
    undo[idx].type=type;
    undo[idx].id=UndoRef;
    undo[idx].hText=undo[idx].hCfmt=undo[idx].hPfmt=undo[idx].hLead=0;
    undo[idx].pFrame=null;
    undo[idx].TblLevel=-1;
    undo[idx].EmbTable=FALSE;


    // save for undo
    if (undo[idx].type=='R')      result=SaveUndoRep(w,idx,BegLine,BegCol,EndLine,EndCol);
    else if (undo[idx].type=='F') result=SaveUndoFont(w,idx,BegLine,BegCol,EndLine,EndCol);
    else if (undo[idx].type=='D') result=SaveUndoDel(w,idx,BegLine,BegCol,EndLine,EndCol);
    else if (undo[idx].type=='P') result=SaveUndoPara(w,idx,BegLine,EndLine);
    else if (undo[idx].type==UNDO_ROW_DEL) result=SaveUndoRowDel(w,idx,&BegLine,&EndLine);
    else if (undo[idx].type==UNDO_ROW_INS) result=SaveUndoRowIns(w,idx,&BegLine,&EndLine);
    else if (undo[idx].type==UNDO_FRAME || undo[idx].type==UNDO_FRAME_LIMITED)   
                                          result=SaveUndoFrame(w,idx,&BegLine,&EndLine,undo[idx].type);
    else if (undo[idx].type==UNDO_PICT_SIZE) result=SaveUndoPict(w,idx,&BegLine,&EndLine);
    else if (undo[idx].type==UNDO_TABLE_ATTRIB) result=SaveUndoTableAttrib(w,&(undo[idx]),BegLine,BegCol,EndLine,EndCol);
    if (!result) return;


    // CLOSE
    if (undo[idx].hText)     GlobalUnlock(undo[idx].hText);
    if (undo[idx].hCfmt) GlobalUnlock(undo[idx].hCfmt);
    if (undo[idx].hPfmt) GlobalUnlock(undo[idx].hPfmt);
    if (undo[idx].hLead) GlobalUnlock(undo[idx].hLead);

    if (type==UNDO_ROW_DEL || type==UNDO_ROW_INS || type==UNDO_CELL_DEL || type==UNDO_CELL_INS) {
       BegCol=0;
       EndCol=LineLen(EndLine)-1;
       if (EndCol<0) EndCol=0;
    } 
    
    if (undo[idx].type!=UNDO_FRAME && undo[idx].type!=UNDO_FRAME_LIMITED && undo[idx].type!=UNDO_TABLE_ATTRIB) {
      undo[idx].beg=RowColToAbs(w,BegLine,BegCol);
      undo[idx].end=RowColToAbs(w,EndLine,EndCol);
    }

    UndoCount++;
    UndoTblSize=UndoCount;

}

/******************************************************************************
    SaveUndoDel:
    Save for deleted text for undo.  The EndCol is inclusive.
*******************************************************************************/
BOOL SaveUndoDel(PTERWND w, int idx, long BegLine, int BegCol, long EndLine, int EndCol)
{
    int  len,SaveHilightType,SaveHilightBegCol,SaveHilightEndCol;
    long index=0,SaveHilightBegRow,SaveHilightEndRow;
    BOOL CreateRtfBuf=FALSE,AppendCrLf=FALSE;
    LPBYTE ptr,lead;
    LPWORD fmt;
    BYTE huge * pUndo;
    BYTE huge * pUndoLead;
    WORD huge * pUndoCfmt;
    bool SaveTrackChanges;

    if (BegLine!=EndLine || (LineFlags(EndLine)&LFLAG_PARA && (EndCol+1)>=LineLen(EndLine))) CreateRtfBuf=TRUE;

    if (CreateRtfBuf) {
        // save any hilighting
        SaveHilightType=HilightType;
        SaveHilightBegRow=HilightBegRow;
        SaveHilightBegCol=HilightBegCol;
        SaveHilightEndRow=HilightEndRow;
        SaveHilightEndCol=HilightEndCol;

        // contruct a block to write and call rtr write
        HilightType=HILIGHT_CHAR;
        HilightBegRow=BegLine;
        HilightBegCol=BegCol;
        HilightEndRow=EndLine;
        HilightEndCol=EndCol+1;
        if (HilightEndCol>LineLen(HilightEndRow)) HilightEndCol=LineLen(HilightEndRow);

        SaveTrackChanges=TrackChanges;
        TrackChanges=false;                   // turn-off track-changes so that RtfWrite returns unaltered text

        RtfWrite(w,RTF_FRAME_BUF,NULL);       // put the text in a buffer
        
        TrackChanges=SaveTrackChanges;

        undo[idx].hText=(HGLOBAL)DlgLong;     // buffer containing text
        undo[idx].hCfmt=NULL;
        undo[idx].hLead=NULL;
        
        undo[idx].TblLevel=ClipTblLevel;  // table level of the undo data
        undo[idx].EmbTable=ClipEmbTable;
        ClipTblLevel=1;
        ClipEmbTable=TRUE;

        // restore the hilight block
        HilightType=SaveHilightType;
        HilightBegRow=SaveHilightBegRow;
        HilightBegCol=SaveHilightBegCol;
        HilightEndRow=SaveHilightEndRow;
        HilightEndCol=SaveHilightEndCol;
    }
    else {
       if (EndCol>=LineLen(EndLine)) AppendCrLf=TRUE;     // append cr/lf in the undo buffer
       if (!SaveUndoAlloc(w,'D',idx,BegLine,BegCol,EndLine,EndCol,(LPVOID far *)&pUndo,(LPVOID far *)&pUndoCfmt, (LPVOID far *)&pUndoLead,NULL)) return FALSE;

       len=EndCol-BegCol+1;             // length to copy
       if (len>LineLen(BegLine)) len=LineLen(BegLine);
       ptr=pText(BegLine);
       fmt=OpenCfmt(w,BegLine);
       if (mbcs) lead=OpenLead(w,BegLine);

       FarMove(&ptr[BegCol],&pUndo[index],len);
       FarMove(&fmt[BegCol],&pUndoCfmt[index],len*sizeof(WORD));
       if (mbcs) FarMove(&lead[BegCol],&pUndoLead[index],len*sizeof(BYTE));
       index=index+len;

       CloseCfmt(w,BegLine);
       if (mbcs) CloseLead(w,BegLine);

       if (AppendCrLf) index=AddCrLf(w,index,pUndo,pUndoCfmt,pUndoLead,NULL);

       pUndo[index]=0;                      // NULL terminate the undo data
       pUndoCfmt[index]=0;
       if (pUndoLead) pUndoLead[index]=0;
    }

    return TRUE;
}

/******************************************************************************
    SaveUndoFont:
    Save for font change for undo.  The EndCol is inclusive.
*******************************************************************************/
BOOL SaveUndoFont(PTERWND w, int idx, long BegLine, int BegCol, long EndLine, int EndCol)
{
    long l,index=0;
    BYTE huge * pUndo;
    BYTE huge * pUndoLead;
    WORD huge * pUndoCfmt;
    int  i,BegCl,EndCl;
    LPWORD fmt;

    if (!SaveUndoAlloc(w,'F',idx,BegLine,BegCol,EndLine,EndCol,(LPVOID far *)&pUndo,(LPVOID far *)&pUndoCfmt,(LPVOID far *)&pUndoLead,NULL)) return FALSE;

    for (l=BegLine;l<=EndLine;l++) {
      if (LineLen(l)==0) continue;
      if (l==BegLine) BegCl=BegCol;
      else            BegCl=0;
      if (l==EndLine) EndCl=EndCol;
      else            EndCl=LineLen(l)-1;

      fmt=OpenCfmt(w,l);
      for (i=BegCl;i<=EndCl;i++,index++) pUndoCfmt[index]=fmt[i];
      CloseCfmt(w,l);
    }
    pUndoCfmt[index]=0;

    return TRUE;
}

/******************************************************************************
    SaveUndoPara:
    Save for paragraph change for undo.  The EndCol is inclusive.
*******************************************************************************/
BOOL SaveUndoPara(PTERWND w, int idx, long BegLine, long EndLine)
{
    long l,index=0;
    WORD huge * pUndoPfmt;

    if (!SaveUndoAlloc(w,'P',idx,BegLine,0,EndLine,0,NULL,NULL,NULL,(LPVOID far *)&pUndoPfmt)) return FALSE;

    index++;    // first slot stores the count

    for (l=BegLine;l<=EndLine;l++) {
       if (LineFlags(l)&LFLAG_PARA_FIRST || l==BegLine) {
          pUndoPfmt[index]=pfmt(l);
          index++;
       }
    }

    pUndoPfmt[0]=(WORD)(index-1);       // number of ids stored

    return TRUE;
}

/******************************************************************************
    SaveUndoRep:
    Save the undo for character replacement.  The EndCol is inclusive.
*******************************************************************************/
BOOL SaveUndoRep(PTERWND w, int idx, long BegLine, int BegCol, long EndLine, int EndCol)
{
    BYTE huge * pUndo;
    BYTE huge * pUndoLead;
    WORD huge * pUndoCfmt;
    LPBYTE ptr,lead;
    LPWORD fmt;

    if (!SaveUndoAlloc(w,'R',idx,BegLine,BegCol,EndLine,EndCol,(LPVOID far *)&pUndo,(LPVOID far *)&pUndoCfmt,(LPVOID far*)&pUndoLead,NULL)) return FALSE;

    ptr=pText(BegLine);
    fmt=OpenCfmt(w,BegLine);
    if (mbcs) lead=pLead(BegLine);

    if (BegCol<(LineLen(BegLine))) {
       pUndo[0]=ptr[BegCol];
       pUndoCfmt[0]=fmt[BegCol];
       if (mbcs) pUndoLead[0]=lead[BegCol];
    }
    else {
      pUndo[0]=' ';
      pUndoCfmt[0]=DEFAULT_CFMT;
      if (mbcs) pUndoLead[0]=0;
    }
    pUndo[1]=0;         // NULL terminate the undo data
    pUndoCfmt[1]=0;
    if (mbcs) pUndoLead[1]=0;

    return TRUE;
}

/******************************************************************************
    SaveUndoRowDel:
    save undo info for Deletion of table rows.
*******************************************************************************/
BOOL SaveUndoRowDel(PTERWND w, int idx, LPLONG pBegLine, LPLONG pEndLine)
{

    if (!(GetUndoRowRange(w,pBegLine,pEndLine))) return FALSE;

    if (!SaveUndoDel(w,idx,*pBegLine,0,*pEndLine,0)) return FALSE;
    undo[idx].TblLevel=0;     // always at top level
    undo[idx].EmbTable=FALSE;

    return TRUE;
}

/******************************************************************************
    SaveUndoRowIns:
    save undo info for Insertion of table rows.
*******************************************************************************/
BOOL SaveUndoRowIns(PTERWND w, int idx, LPLONG pBegLine, LPLONG pEndLine)
{
    if (!(GetUndoRowRange(w,pBegLine,pEndLine))) return FALSE;
    
    return TRUE;
}


/******************************************************************************
    GetUndoRowRange:
    Get the lines which range the complete rows at level 0.
*******************************************************************************/
BOOL GetUndoRowRange(PTERWND w, LPLONG pBegLine, LPLONG pEndLine)
{
    long BegLine=(*pBegLine);
    long EndLine=(*pEndLine);
    int cl,row,PrevRow,LastCell;
    BOOL IsSpanned;

    // ensure that the beginning and endling lines belong to a table
    if (cid(BegLine)==0) {
       for (;(BegLine+1)<TotalLines;BegLine++) if (cid(BegLine)>0) break;
    }
    if (cid(EndLine)==0) {
       for (;EndLine>0;EndLine--) if (cid(EndLine)>0) break;
    }

    // ensure that beginning line is the first line of a top level row
    cl=cid(BegLine);
    if (cl==0) return FALSE;
    if (BegLine==0 || (cid(BegLine-1)==0)) goto SET_LAST_LINE;  // first line set
    for (BegLine--;BegLine>=0;BegLine--) {   // look for previous row break line at level 0
       cl=cid(BegLine);
       if (cl==0) break;
       if (cell[cl].level>0) continue;
       if (LineInfo(w,BegLine,INFO_ROW)) break;   // row break line
    } 
    BegLine++;                   // first row line

    // check if the first row contains any spanned cells, traverse backward
    cl=cid(BegLine);
    row=cell[cl].row;
    IsSpanned=false;
    while (row>0) {
      cl=TableRow[row].FirstCell;
      for (;cl>0;cl=cell[cl].NextCell) if (True(cell[cl].flags&CFLAG_ROW_SPANNED)) break;
      if (cl<=0) break;
      
      // spanning detected
      IsSpanned=true;
      if (TableRow[row].PrevRow<=0) break;    // first row
      row=TableRow[row].PrevRow;
    }   
    if (IsSpanned) {      // the orginal row was spanned, find the line corresponding to this row which is not spanned
       PrevRow=TableRow[row].PrevRow;
       if (PrevRow<0) PrevRow=0;
       if (PrevRow>0) LastCell=TableRow[PrevRow].LastCell;
       else           LastCell=0;
       for (;BegLine>=0;BegLine--) if (cid(BegLine)==LastCell) break;  
       BegLine++;        // first line of the row which is not spanned
    } 


    SET_LAST_LINE:
    cl=cid(EndLine);
    if (cl==0) return FALSE;
    if (cell[cl].level==0 && LineInfo(w,EndLine,INFO_ROW)) goto CHECK_SPANNING;   // row break line
    for (;(EndLine+1)<TotalLines;EndLine++) {
       cl=cid(EndLine);
       if (cl==0) return FALSE;
       if (cell[cl].level>0) continue;
       if (LineInfo(w,EndLine,INFO_ROW)) break;   // row break line
    } 

    CHECK_SPANNING:  // check if this row has spanned or spanning cells
    cl=cid(EndLine);
    row=cell[cl].row;
    IsSpanned=false;
    PrevRow=0;
    while (row>0) {
      cl=TableRow[row].FirstCell;
      for (;cl>0;cl=cell[cl].NextCell) if (True(cell[cl].flags&CFLAG_ROW_SPANNED) || cell[cl].RowSpan>1) break;
      if (cl<=0) break;

      // spanning detected
      IsSpanned=true;
      PrevRow=row;
      if (TableRow[row].NextRow<=0) break;    // first row
      row=TableRow[row].NextRow;
    }
    if (IsSpanned && PrevRow>0) {
       for (;(EndLine+1)<TotalLines;EndLine++) {
          cl=cid(EndLine);
          row=cell[cl].row;
          if (row==PrevRow && LineInfo(w,EndLine,INFO_ROW)) break;  
       }
    }    


    //END:
    (*pBegLine)=BegLine;
    (*pEndLine)=EndLine;

    return TRUE;
}

/******************************************************************************
    SaveUndoFrame:
    save undo info for a paragraph frame.
    The frame id is passed as pBegLine.  pEndLine not used.
*******************************************************************************/
BOOL SaveUndoFrame(PTERWND w, int idx, LPLONG pBegLine, LPLONG pEndLine,int type)
{
    int frm=(int)(*pBegLine);

    (*pBegLine)=(*pEndLine)=0;

    if (frm<0 || frm>=TotalParaFrames) return FALSE;

    undo[idx].pFrame=MemAlloc(sizeof(struct StrParaFrame));
    if (True(TerOpFlags2&TOFLAG2_USE_UNDO_FRAME)) 
         FarMove(&UndoParaFrame,undo[idx].pFrame,sizeof(struct StrParaFrame));
    else FarMove(&(ParaFrame[frm]),undo[idx].pFrame,sizeof(struct StrParaFrame));

    undo[idx].ObjId=frm;    // save the object id
 
    // save the line pointers
    if (type==UNDO_FRAME) {             // undo line array also
       undo[idx].LinePtrU=CloneLinePtr(w);
       undo[idx].TotalLinesU=TotalLines;
       undo[idx].MaxLinesU=MaxLines;
       undo[idx].CursPos=RowColToAbs(w,CurLine,CurCol);
    }

    return TRUE;
}

/******************************************************************************
    SaveUndoPict:
    save undo info for picture size change.
    The frame id is passed as pBegLine.  pEndLine not used.
*******************************************************************************/
BOOL SaveUndoPict(PTERWND w, int idx, LPLONG pBegLine, LPLONG pEndLine)
{
    int pict=(int)(*pBegLine);

    (*pBegLine)=(*pEndLine)=0;

    if (pict<0 || pict>=TotalFonts) return FALSE;

    undo[idx].ObjId=pict;    // save the object id
    undo[idx].width=UndoInt1;  // width passed as UndoInt1
    undo[idx].height=UndoInt2; 
 
    return TRUE;
}

/******************************************************************************
    SaveUndoTableAttrib:
    save undo info for table attributes.
*******************************************************************************/
BOOL SaveUndoTableAttrib(PTERWND w, struct StrUndo *pUndo, long BegLine, int BegCell, long EndLine, int EndCell)
{
    int RowCount=0;
    int CellCount=0;
    int row,PrevRow=0;
    int cl=0;
    long l;

    pUndo->RowCount=0;
    pUndo->RowId=null;
    pUndo->CellId=null;
    pUndo->pRow=null;
    pUndo->pCell=null;

    if (BegLine==0 && EndLine==0 && BegCell==0 && EndCell==0) return SaveUndoMarkedCells(w,pUndo);   // save all marked cells

    // get beginning and ending lines if not specified
    if (BegLine<0) {   // calculate beg line from BegCell
       for (l=0;l<TotalLines;l++) if (cid(l)==BegCell) break;
       if (l==TotalLines) return false;    // cell not found
       BegLine=l;
    } 
    if (EndLine<0) {   // calculate beg line from EndCell
       if (EndCell<=0) {  // save the entire table
          for (EndLine=BegLine;EndLine<TotalLines && cid(EndLine)>0;EndLine++) ;  
          EndLine--;
       }
       else { 
         for (l=0;l<TotalLines;l++) if (cid(l)==EndCell) break;
         if (l==TotalLines) return false;    // cell not found
         EndLine=l;
       }
    } 
    if (EndLine<BegLine) {  // swap
       l=BegLine;
       BegLine=EndLine;
       EndLine=l;
    }

    // get the row and cell count
    RowCount=CellCount=0;
    PrevRow=0;
    for (l=BegLine;l<=EndLine;l++) {
       cl=cid(l);
       if (cl==0) continue;

       row=cell[cl].row;
       if (row!=PrevRow) {         // new row
          RowCount++;
          for (cl=TableRow[row].FirstCell;cl>=0;cl=cell[cl].NextCell) CellCount++;

          PrevRow=row;
       } 
    } 

    if (RowCount==0 || CellCount==0) return false;

    // allocate memory
    pUndo->RowId=MemAlloc(sizeof(int)*(RowCount+1));
    pUndo->pRow=MemAlloc(sizeof(struct StrTableRow)*(RowCount+1));

    pUndo->CellId=MemAlloc(sizeof(int)*(CellCount+1));
    pUndo->pCell=MemAlloc(sizeof(struct StrCell)*(CellCount+1));

    // save the rows and cells
    RowCount=CellCount=0;
    PrevRow=0;
    for (l=BegLine;l<=EndLine;l++) {
       cl=cid(l);
       if (cl==0) continue;

       row=cell[cl].row;
       if (row!=PrevRow) {         // new row
          pUndo->RowId[RowCount]=row;      // save row id
          FarMove(&(TableRow[row]),&(pUndo->pRow[RowCount]),sizeof(struct StrTableRow));
          RowCount++;

          // save cell data
          for (cl=TableRow[row].FirstCell;cl>=0;cl=cell[cl].NextCell) {
             pUndo->CellId[CellCount]=cl;      // save cell id
             FarMove(&(cell[cl]),&(pUndo->pCell[CellCount]),sizeof(struct StrCell));

             CellCount++;
          } 

          PrevRow=row;
       } 
    } 
 
    pUndo->RowCount=RowCount;
    pUndo->CellCount=CellCount;

    return true;
} 

/******************************************************************************
    SaveUndoMarkedCells:
    save undo of marked cells by MarkCell function.
*******************************************************************************/
BOOL SaveUndoMarkedCells(PTERWND w, struct StrUndo *pUndo)
{
    int i,RowCount=0;
    int CellCount=0;
    int row,PrevRow=0;
    int cl=0;

    // get the count of marked cells
    CellCount=0;
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;
       CellCount++;
    } 

    if (CellCount==0) return false;

    // allocate memory 
    pUndo->RowId=MemAlloc(sizeof(int));          // initial allocation
    pUndo->pRow=MemAlloc(sizeof(struct StrTableRow));
    pUndo->CellId=MemAlloc(sizeof(int)*(CellCount+1));
    pUndo->pCell=MemAlloc(sizeof(struct StrCell)*(CellCount+1));

    // save current information
    CellCount=0;
    RowCount=0;
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;
       pUndo->CellId[CellCount]=cl;      // save cell id
       FarMove(&(cell[cl]),&(pUndo->pCell[CellCount]),sizeof(struct StrCell));
       CellCount++;

       // check if row needs to be saved
       row=cell[cl].row;
       for (i=0;i<RowCount;i++) if (pUndo->RowId[i]==row) break;
       if (i==RowCount) {          // this row not saved yet, save now
          pUndo->RowId=MemReAlloc(pUndo->RowId,sizeof(int)*(RowCount+2));          // initial allocation
          pUndo->pRow=MemReAlloc(pUndo->pRow,sizeof(struct StrTableRow)*(RowCount+2));
       
          pUndo->RowId[RowCount]=row;      // save row id
          FarMove(&(TableRow[row]),&(pUndo->pRow[RowCount]),sizeof(struct StrTableRow));
          RowCount++;
       } 
    } 

    pUndo->RowCount=RowCount;
    pUndo->CellCount=CellCount;

    return true;
} 

/******************************************************************************
    SaveUndoAlloc:
    Allocate buffers for undo
*******************************************************************************/
BOOL SaveUndoAlloc(PTERWND w, BYTE type, int idx, long BegLine, int BegCol, long EndLine, int EndCol, LPVOID far *ppUndo, LPVOID far *ppUndoCfmt, LPVOID far *ppUndoLead, LPVOID far *ppUndoPfmt)
{
    long l;
    long UndoSize=0;
    BYTE huge * pUndo;
    BYTE huge * pUndoLead;
    WORD huge * pUndoCfmt;
    WORD huge * pUndoPfmt;

    // release any old buffers
    FreeOneUndo(w,idx);

    pUndo=pUndoLead=NULL;
    pUndoCfmt=pUndoPfmt=NULL;
    

    // calculate the size of the undo buffer
    if (type=='D' || type=='F') {
       if (EndCol>=LineLen(EndLine)) {
          UndoSize=UndoSize+2; // space for cr/lf
          EndCol=LineLen(EndLine)-1;
          if (EndCol<0) EndCol=0;
       }
       for (l=BegLine;l<=EndLine;l++){   // caluculate the undo buffer size
          if  (BegLine==EndLine) UndoSize=UndoSize+EndCol-BegCol+1;
          else                   UndoSize=UndoSize+LineLen(l);
          UndoSize=UndoSize+2;        // add space cr/lf
       }
       UndoSize++;                    // space for the NULL delimiter
    }
    else if (type=='P') {             // paragraph information
       UndoSize=2;                    // to store count and the first para id
       for (l=BegLine+1;l<=EndLine;l++) if (LineFlags(l)&LFLAG_PARA_FIRST) UndoSize++;
    }
    else if (type=='R') UndoSize=2;

    // allocate memory
    if (type=='F') {     // just allocate the font space
       if (NULL==(undo[idx].hCfmt=GlobalAlloc(GMEM_MOVEABLE,UndoSize*sizeof(WORD)))){  // allocate memory for the clipboard data
           return PrintError(w,MSG_OUT_OF_MEM,"SaveUndo");
       }
       if (NULL==(pUndoCfmt=(WORD huge *)GlobalLock(undo[idx].hCfmt))) {
           return PrintError(w,MSG_OUT_OF_MEM,"SaveUndo(a)");
       }
    }
    else if (type=='D' || type=='R') {
       if ( NULL==(undo[idx].hText=GlobalAlloc(GMEM_MOVEABLE,UndoSize))
         || (mbcs && NULL==(undo[idx].hLead=GlobalAlloc(GMEM_MOVEABLE,UndoSize)))
         || NULL==(undo[idx].hCfmt=GlobalAlloc(GMEM_MOVEABLE,UndoSize*sizeof(WORD)))){  // allocate memory for the clipboard data
           return PrintError(w,MSG_OUT_OF_MEM,"SaveUndoAlloc");
       }
       if ( NULL==(pUndo=(BYTE huge *)GlobalLock(undo[idx].hText))
         || (mbcs && NULL==(pUndoLead=(BYTE huge *)GlobalLock(undo[idx].hLead)))
         || NULL==(pUndoCfmt=(WORD huge *)GlobalLock(undo[idx].hCfmt))) {
           return PrintError(w,MSG_OUT_OF_MEM,"SaveUndoAlloc(a)");
       }
    }
    else if (type=='P') {     // space for the para ids
       if (NULL==(undo[idx].hPfmt=GlobalAlloc(GMEM_MOVEABLE,UndoSize*sizeof(WORD)))){  // allocate memory for the clipboard data
           return PrintError(w,MSG_OUT_OF_MEM,"SaveUndo");
       }
       if (NULL==(pUndoPfmt=(WORD huge *)GlobalLock(undo[idx].hPfmt))) {
           return PrintError(w,MSG_OUT_OF_MEM,"SaveUndo(a)");
       }
    }

    if (!mbcs) pUndoLead=NULL;

    if (ppUndo)     (*ppUndo)=pUndo;
    if (ppUndoCfmt) (*ppUndoCfmt)=pUndoCfmt;
    if (ppUndoPfmt) (*ppUndoPfmt)=pUndoPfmt;
    if (ppUndoLead) (*ppUndoLead)=pUndoLead;

    return TRUE;
}

/******************************************************************************
    TerSetUndoRef:
    Flush the undo/redo buffer.
******************************************************************************/
int WINAPI _export TerSetUndoRef(HWND hWnd, int NewRef)
{
   PTERWND w;
   int SaveRef;

   if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

   SaveRef=UndoRef;
   if (NewRef>=0) UndoRef=NewRef;

   return SaveRef;
}


/******************************************************************************
    TerFlushUndo:
    Flush the undo/redo buffer.
******************************************************************************/
BOOL WINAPI _export TerFlushUndo(HWND hWnd)
{
   PTERWND w;
   if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
   return ReleaseUndo(w);
}

/******************************************************************************
    ReleaseUndo:
    Release all undo/redo buffers.
*******************************************************************************/
ReleaseUndo(PTERWND w)
{
    int i;

    for (i=0;i<UndoTblSize;i++) FreeOneUndo(w,i);
    UndoCount=UndoTblSize=0;

    return TRUE;
}

/******************************************************************************
    ReleaseRedo:
    Release all redo buffers.
*******************************************************************************/
ReleaseRedo(PTERWND w)
{
    int i;

    for (i=UndoCount;i<UndoTblSize;i++) FreeOneUndo(w,i);
    UndoTblSize=UndoCount;

    return TRUE;
}

/******************************************************************************
    FreeOneUndo:
    Free undo item memory.
*******************************************************************************/
FreeOneUndo(PTERWND w, int idx)
{
    if (undo[idx].hText) GlobalFree(undo[idx].hText);
    if (undo[idx].hCfmt) GlobalFree(undo[idx].hCfmt);
    if (undo[idx].hPfmt) GlobalFree(undo[idx].hPfmt);
    if (undo[idx].hLead) GlobalFree(undo[idx].hLead);
    undo[idx].hText=undo[idx].hCfmt=undo[idx].hPfmt=undo[idx].hLead=NULL;
    
       
    if (undo[idx].pFrame) MemFree(undo[idx].pFrame);
    undo[idx].pFrame=null;

    if (undo[idx].LinePtrU) FreeClonedLinePtr(w,undo[idx].LinePtrU,undo[idx].TotalLinesU);
    undo[idx].LinePtrU=null;

    // free row attribute undos
    if (undo[idx].RowId) MemFree(undo[idx].RowId);
    undo[idx].RowId=null;
    if (undo[idx].pRow) MemFree(undo[idx].pRow);
    undo[idx].pRow=null;
    if (undo[idx].CellId) MemFree(undo[idx].CellId);
    undo[idx].CellId=null;
    if (undo[idx].pCell) MemFree(undo[idx].pCell);
    undo[idx].pCell=null;

    undo[idx].RowCount=0;
    undo[idx].CellCount=0;

    return true;
}
 
/******************************************************************************
    ScrollUndo:
    Scroll off the first undo
*******************************************************************************/
ScrollUndo(PTERWND w)
{
    int i,ScrollCount;


    if (UndoCount<=0) return TRUE;

    if (MaxUndos<MaxUndoLimit) {     // limit not reached yet
       MaxUndos+=50;
       if (MaxUndos>MaxUndoLimit) MaxUndos=MaxUndoLimit;
       undo=MemReAlloc(undo,(MaxUndos+1)*sizeof(struct StrUndo));
       return TRUE;
    } 

    // determine the undos to scroll off
    ScrollCount=MaxUndoLimit/4;
    if (ScrollCount<1) ScrollCount=1;
    if (ScrollCount>1000) ScrollCount=1000;
    if (undo[ScrollCount-1].id==undo[ScrollCount].id) {    // make sure that whole series is scrolled off
       for (i=ScrollCount;i<UndoCount;i++) if (undo[i].id!=undo[ScrollCount-1].id) break;
       ScrollCount=i;
    } 
    UndoSkipRef=undo[ScrollCount-1].id;    // if this undo is encountered again, skip it

    // release the undos
    for (i=0;i<ScrollCount;i++) FreeOneUndo(w,i);

    // scroll the remaining undos
    for (i=ScrollCount;i<UndoCount;i++) undo[i-ScrollCount]=undo[i];

    UndoCount-=ScrollCount;
    if (UndoCount<0) UndoCount=0;

    return TRUE;
}

/******************************************************************************
    AddCrLf:
    Append CR/LF characters to the undo buffers.
******************************************************************************/
long AddCrLf(PTERWND w,long index,BYTE huge * pUndo,WORD huge * pUndoCfmt, BYTE huge *pUndoLead, WORD huge *pUText)
{
    if (pUndo)  pUndo[index]=0xD;               // add CR
    if (pUText) pUText[index]=0xD;
    if (pUndoLead) pUndoLead[index]=0;

    if (pUndoCfmt) {
       if (index>0) pUndoCfmt[index]=pUndoCfmt[index-1];
       else         pUndoCfmt[index]=DEFAULT_CFMT;
       if (TerFont[pUndoCfmt[index]].style&PICT) pUndoCfmt[index]=DEFAULT_CFMT;
    }

    index++;
    if (pUndo)  pUndo[index]=0xA;               // add LF
    if (pUText) pUText[index]=0xA;
    if (pUndoCfmt) pUndoCfmt[index]=pUndoCfmt[index-1];
    if (pUndoLead) pUndoLead[index]=0;

    index++;
    return index;
}


/******************************************************************************
    TerRowColToAbs:
    Exported function to convert row/col coordinates to absolute coordinate
******************************************************************************/
long WINAPI _export TerRowColToAbs(HWND hWnd,long row,int col)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    return RowColToAbs(w,row,col);
}

/******************************************************************************
    TerAbsToRowCol:
    Exported function to convert absolute coordinates to row/col coordinates
******************************************************************************/
void WINAPI _export TerAbsToRowCol(HWND hWnd,long abs,long far *row,int far *col)
{
    PTERWND w;

    (*row)=0;
    (*col)=0;
    if (NULL==(w=GetWindowPointer(hWnd))) return;  // get the pointer to window data

    AbsToRowCol(w,abs,row,col);
}

/******************************************************************************
    RowColToAbs:
    Convert row/col coordinates to absolute coordinate
******************************************************************************/
long RowColToAbs(PTERWND w,long row,int col)
{
    int  ReturnLen=0;
    long l,result=0;
    BOOL PCharAsCrLf=(TerFlags4&TFLAG4_COUNT_PCHAR_AS_CRLF)?TRUE:FALSE;


    if (!(TerArg.WordWrap)) ReturnLen=2;  // recognize the hard return in the text mode

    if (row<0) row=0;
    if (row>=TotalLines) row=TotalLines-1;
    for (l=0;l<row;l++) {
       result=result+LineLen(l)+ReturnLen;
       if (PCharAsCrLf && ((LineFlags(l)&(LFLAG_PARA|LFLAG_LINE|LFLAG_HDRS_FTRS)) 
           || (LineLen(l)==1  && LineInfo(w,l,INFO_ROW))) ) result++;
    
    }

    if (col>LineLen(row) && col>0) col=LineLen(row)-1+ReturnLen;
    if (col<0) col=0;
    result=result+col;

    return result;
}

/******************************************************************************
    AbsToRowCol:
    Convert absolute coordinates to row/col coordinates
******************************************************************************/
void AbsToRowCol(PTERWND w,long abs,long far *row,int far *col)
{
    int ReturnLen=0,len;
    BOOL PCharAsCrLf=(TerFlags4&TFLAG4_COUNT_PCHAR_AS_CRLF)?TRUE:FALSE;

    if (!(TerArg.WordWrap)) ReturnLen=2;  // in text mode, <CR> is a valid character

    (*row)=(*col)=0;
    while (abs>=0) {
       len=LineLen(*row)+ReturnLen;       // calculate effective line length
       if (PCharAsCrLf && ((LineFlags(*row)&(LFLAG_PARA|LFLAG_LINE|LFLAG_HDRS_FTRS)) 
           || (LineLen(*row)==1  && LineInfo(w,*row,INFO_ROW))) ) len++;

       if (abs>=len) abs=abs-len;
       else {
          *col=(int)abs;
          break;
       }
       if ((*row)>=(TotalLines-1)) {
          *col=LineLen(TotalLines-1)-1+ReturnLen;
          break;
       }
       (*row) ++;
    }

    if ((*col)<0) *col=0;
}

/******************************************************************************
     SendLinkMessage:
     Send hyperlink message to the parent window.  This routine returns TRUE
     if the application accepted the TER_LINK message;
******************************************************************************/
BOOL SendLinkMessage(PTERWND w,BOOL DoubleClick)
{
    struct StrHyperLink link;

    //if (!TerArg.hParentWnd) return FALSE;     // parent window not specified

    // initialize the hyperlink structure
    link.hWnd=hTerWnd;
    link.DoubleClick=DoubleClick;
    link.code[0]=link.text[0]=0;


    if (!TerGetHypertext(hTerWnd,link.text,link.code)) return FALSE;

    if (IsAnchorName(w,link.code)) return FALSE;   // this is just a link name

    // SEND_MESSAGE
    return (BOOL)SendMessageToParent(w,TER_LINK,(WPARAM)hTerWnd,(DWORD)(struct StrHyperLink far *)&link,TRUE);
}

/******************************************************************************
     SendMessageToParent:
     Send a message to the parent window or the callback function.
******************************************************************************/
LRESULT SendMessageToParent(PTERWND w,UINT msg, WPARAM wParam, LPARAM lParam, BOOL DisableTer)
{
    BOOL PreviouslyDisabled;
    LRESULT result;
    HWND hFocusWnd;
    int  i;

    if (MsgCallback) return MsgCallback(hTerWnd,msg,wParam,lParam);

    if (VbxCallback) return VbxCallback(hTerWnd,msg,wParam,lParam);
    if (VbxControl) return (LRESULT) FALSE;

    hFocusWnd=GetFocus();                             // get the window that has the focus

    if (DisableTer) {   // Ter can't be disabled if it has child controls
       for (i=0;i<TotalFonts;i++) if (IsControl(w,i)) break;
       if (i<TotalFonts) DisableTer=FALSE;
    }
    if (DisableTer) PreviouslyDisabled=EnableWindow(hTerWnd,FALSE);   // disable our window

    result=SendMessage(TerArg.hParentWnd,msg,wParam,lParam);
    if (DisableTer && !PreviouslyDisabled) EnableWindow(hTerWnd,TRUE); // enable our window

    if (IsWindow(hFocusWnd) && hFocusWnd!=GetFocus()) SetFocus(hFocusWnd);

    return result;
}

/******************************************************************************
    GetHypertextStart:
    returns the line/col of the first hidden character of the hypertext.
******************************************************************************/
BOOL GetHypertextStart(PTERWND w, LPLONG pLineNo, LPINT pColNo)
{
    long LineNo=(*pLineNo);
    int  ColNo=(*pColNo);
    int  CurCfmt,PrevCfmt;
    long line;
    int  col,StartCol;
    BOOL InHiddenText,InHlinkField;

    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    InHiddenText=TerFont[CurCfmt].style&HIDDEN;  // check if located on the hidden text
    InHlinkField=TerFont[CurCfmt].FieldId==FIELD_HLINK;

    for (line=LineNo;line>=0;line--) {
      if (line==LineNo) StartCol=ColNo;
      else              StartCol=LineLen(line)-1;

      for (col=StartCol;col>=0;col--) {
         PrevCfmt=GetPrevCfmt(w,line,col);   // get the previous font id
         if (InHlinkField && !IsSameField(w,PrevCfmt,CurCfmt)) break;

         if (InHiddenText && !(TerFont[PrevCfmt].style&HIDDEN)) break;
         else if (!IsHypertext(w,PrevCfmt)) InHiddenText=TRUE;
      }
      if (col>=0) break;
    }
    if (line<0) {
      line=0;
      col=0;
    }
    (*pLineNo)=line;
    (*pColNo)=col;

    return TRUE;
}

/******************************************************************************
    GetHypertextEnd:
    returns the line/col of the character following the last character of the
    hypertext. The pColNo points to the column number after the current column
    number.
******************************************************************************/
BOOL GetHypertextEnd(PTERWND w, LPLONG pLineNo, LPINT pColNo)
{
    long LineNo=(*pLineNo);
    int  ColNo=(*pColNo);
    int CurCfmt,InitCfmt,len;
    long line;
    int  col,StartCol;
    BOOL InHiddenText=TRUE,InHlinkField;

    InitCfmt=GetCurCfmt(w,LineNo,ColNo);
    InHlinkField=TerFont[InitCfmt].FieldId==FIELD_HLINK;

    for (line=LineNo;line<TotalLines;line++) {
      if (line==LineNo) StartCol=ColNo;
      else              StartCol=0;

      len=LineLen(line);
      for (col=StartCol;col<len;col++) {
         CurCfmt=GetCurCfmt(w,line,col);
         
         if (InHlinkField && !IsSameField(w,CurCfmt,InitCfmt)) break;

         if (InHiddenText && !(TerFont[CurCfmt].style&HIDDEN)) InHiddenText=FALSE;
         if (!InHiddenText && !IsHypertext(w,GetCurCfmt(w,line,col))) break;
      }
      //if (col>=len && LineFlags(line)&LFLAG_PARA) col=len-1;    // comment to allow for paramarker in the hypertext field
      if (col<len) break;
    }
    if (line>=TotalLines) {
      line=TotalLines-1;
      col=LineLen(line);
    }

    (*pLineNo)=line;
    (*pColNo)=col;

    return TRUE;
}


/******************************************************************************
    PosAfterHiddenText:
    In the html mode the cursor may be position on the first hidden character.
    This function repositions the cursor on the first link character.
******************************************************************************/
BOOL PosAfterHiddenText(PTERWND w)
{
    while (TerFont[GetCurCfmt(w,CurLine,CurCol)].style&HIDDEN) {
       CursDirection=CURS_BACKWARD;      // eventually the cursor needs to be moved back

       if ((CurCol+1)<LineLen(CurLine)) CurCol++;
       else {
          if ((CurLine+1)<TotalLines) {
             CurLine++;
             CurCol=0;
          }
          else break;
       }
    }
    return TRUE;
}

/******************************************************************************
    IsLoneHypertextChar:
    This function returns true if the chararacer is a lone hypertext character
******************************************************************************/
BOOL IsLoneHypertextChar(PTERWND w, long LineNo, int ColNo)
{
    int CurCfmt,PrevCfmt;

    CurCfmt=GetCurCfmt(w,LineNo,ColNo);
    if (IsHypertext(w,CurCfmt)) {
       PrevCfmt=GetPrevCfmt(w,LineNo,ColNo);
       if (TerFont[PrevCfmt].style&HIDDEN) {   // beginning of the hyperlink phrase
          int NextCfmt=GetNextCfmt(w,LineNo,ColNo);
          if (!IsHypertext(w,NextCfmt)) return TRUE;    // a lone hypertext character being deleted
       }
    }
    return FALSE;
}

/******************************************************************************
     IsHypertext:
     This function returns true if the given font id is a hypertext phrase.
******************************************************************************/
BOOL IsHypertext(PTERWND w, int CurCfmt)
{

    return IsHypertext2(w,CurCfmt,TRUE);
}
 
/******************************************************************************
     IsHypertext2:
     This function returns true if the given font id is a hypertext phrase.
******************************************************************************/
BOOL IsHypertext2(PTERWND w, int CurCfmt, BOOL IncludeAnchorName)
{
     return IsHypertext3(w,CurCfmt,IncludeAnchorName,false);
}

BOOL IsHypertext3(PTERWND w, int CurCfmt, BOOL IncludeAnchorName, BOOL IncludePageRef)
{
    COLORREF TextColor;

    // match the page ref field

    // match any character style style
    if (TerFont[CurCfmt].FieldId==FIELD_HLINK) {
       if (IncludeAnchorName) return TRUE; 
       else if (!IsAnchorName(w,TerFont[CurCfmt].FieldCode)) return TRUE;
    }
    
    // match the page ref field
    if (IncludePageRef && TerFont[CurCfmt].FieldId==FIELD_PAGE_REF && TerFont[CurCfmt].FieldCode!=null) {
       if (strstr(TerFont[CurCfmt].FieldCode,"\\h")!=null) return true;
    } 

    if (TerFlags5&TFLAG5_OLD_HLINK) { // support for old style hyperlink
       if (LinkStyle&HLINK && TerFont[CurCfmt].style&HLINK) return TRUE;
       if ( !(TerFont[CurCfmt].style&LinkStyle) && !(LinkStyle&PICT) && LinkStyle!=0) return FALSE;

       // For pictures all character styles should match
       if (TerFont[CurCfmt].style&PICT) {
         if ((TerFont[CurCfmt].style&LinkStyle)==LinkStyle && LinkStyle!=0) return TRUE; // check if LinkStyle set
         return CheckImageMapHit(w,CurCfmt);  // check if positioned over a image map
       }

       // apply the color check for the text type link
       TextColor=TerFont[CurCfmt].TextColor;
       if (TextColor==CLR_AUTO) TextColor=CLR_BLACK;
       if (TextColor==LinkColor && !(LinkStyle&HLINK)) return TRUE;
    }

    return FALSE;
}

/******************************************************************************
     IsAnchorName:
     This function returns if the hypertext code is just anchor name.
******************************************************************************/
BOOL IsAnchorName(PTERWND w, LPBYTE code)
{
    BYTE string[MAX_WIDTH+1];
    LPBYTE ptr;

    if (!code) return FALSE;

    lstrcpy(string,code);
    lstrupr(w,string);

    
    if ((ptr=strstr(string,"?"))!=NULL) (*ptr)=0;  // don't consider after the '?'
    if (strstr(string,"NAME =")==NULL && strstr(string,"NAME=")==NULL) return FALSE;  // does not have name tags

    // name found, check for href
    if (strstr(string,"HREF")) return FALSE;  // href also found, the code is not name only

    return TRUE;  // is name only
}

/******************************************************************************
   GetTabPos:
   Get tab position and type at the given location
*******************************************************************************/
GetTabPos(PTERWND w,int ParaId, struct StrTab far *tab, int CurPos,int far *pTabPos, int far *pTabType,LPBYTE pFlags, BOOL screen)
{
   int i,NewPos,indent,count,TabType;
   BOOL StopAtIndent=FALSE,ConsiderTabIndent=!NoTabIndent;
   BYTE flags=0;

   // convert input position to twips
   if (screen) CurPos=ScrToTwipsX(CurPos+1);  // add 1 to avoid error due to rounding
   else        CurPos=PrtToTwipsX(CurPos+1);

   // limit tab past screen in fitted view mode
   count=tab->count;
   //if (TerArg.PageMode && TerArg.FittedView && screen) {
   //   for (i=0;i<tab->count;i++) {
   //      if (tab->pos[i]>ScrToTwipsX(TerWinWidth)) break;
   //   }
   //   count=i; 
   //}

   // get the para indentation
   indent=PfmtId[ParaId].LeftIndentTwips;

   // find the next stop
   for (i=0;i<count;i++) {
      if (ConsiderTabIndent && indent<tab->pos[i] && indent>CurPos) {
         StopAtIndent=TRUE;
         break;
      }
      if (tab->pos[i]>CurPos) break;  // add 1 to avoid rounding error
   }

   if (StopAtIndent) {           // stop at the indent
      NewPos=indent;
      TabType=TAB_LEFT;
   }
   else if (i<count) {      // tab stop found
      NewPos=tab->pos[i];
      TabType=tab->type[i];
      flags=tab->flags[i];
   }
   else {                        // use the default stops
      NewPos=0;
      if (NoTabIndent) indent=-99999;  // do not consider left indent as a tab stop
      while (TRUE) {
         if (NewPos>=indent) {
            if (indent>CurPos) {
               NewPos=indent;
               break;
            }
            else if (NewPos>CurPos) break;
         }
         NewPos+=DefTabWidth;
      }
      TabType=TAB_LEFT;       // left tab by default
   }

   // convert position to current unit
   if (screen) NewPos=TwipsToScrX(NewPos);
   else        NewPos=TwipsToPrtX(NewPos);

   if (pTabPos) (*pTabPos)=NewPos;
   if (pTabType)(*pTabType)=TabType;
   if (pFlags) (*pFlags)=flags;

   return TRUE;
}

/******************************************************************************
   GetSpaceAdj;
   Get the adjustment for the space width
******************************************************************************/
GetSpaceAdj(PTERWND w,long line, int SpaceNo)
{
   int adj=0;

   if ( tabw(line) && tabw(line)->type&INFO_JUST
     && SpaceNo>=tabw(line)->JustSpaceIgnore
     && SpaceNo<tabw(line)->JustSpaceCount) {
      adj=tabw(line)->JustAdj;
      if ((SpaceNo-tabw(line)->JustSpaceIgnore)<tabw(line)->JustCount) adj++;
   }
   return adj;
}

/******************************************************************************
   GetTabWidth;
   Get the width of the specified tab number for a line
******************************************************************************/
GetTabWidth(PTERWND w,long line, int TabNo, int CurX)
{
   int x1,x2,width;

   if (tabw(line) && tabw(line)->type&INFO_TAB && TabNo<tabw(line)->count) width=tabw(line)->width[TabNo];
   else {                              // find the default tab postion
      x1=ScrToTwipsX(CurX+1);          // add one to take care of the rounding problem in ScrToTwips function
      x2=((x1/DefTabWidth)+1)*DefTabWidth;
      x2=TwipsToScrX(x2);
      width=x2-CurX;
   }

   return width;
}

/******************************************************************************
   GetLinePoints:
   Get the points for the line object
*******************************************************************************/
GetLinePoints(PTERWND w, int FrameNo, LPINT x1, LPINT y1, LPINT x2, LPINT y2)
{
   int ParaFID=frame[FrameNo].ParaFrameId;

   if (frame[FrameNo].empty || !(ParaFrame[ParaFID].flags&PARA_FRAME_LINE)) return TRUE;

   // get the line coordinates
   (*x1)=frame[FrameNo].x;
   (*x2)=frame[FrameNo].x+frame[FrameNo].width;
   (*y1)=frame[FrameNo].y;
   (*y2)=frame[FrameNo].y+frame[FrameNo].height;
   if (ParaFrame[ParaFID].LineType==DOL_DIAG) SwapInts(y1,y2);  // swap the points
   if (ParaFrame[ParaFID].LineType==DOL_HORZ) (*y2)=(*y1);
   if (ParaFrame[ParaFID].LineType==DOL_VERT) (*x2)=(*x1);

   return TRUE;
}

/******************************************************************************
   SetParaBorders:
   This routines sets the TextBorder variable for a paragraph border
*******************************************************************************/
SetParaBorder(PTERWND w,long LineNo)
{
    int CurPara;
    UINT ParaFlags;

    CurPara=(int)pfmt(LineNo);
    ParaFlags=PfmtId[CurPara].flags;


    if (LineFlags(LineNo)&LFLAG_BOX_TOP) TextBorder|=BOX_TOP;
    if (LineFlags(LineNo)&LFLAG_BOX_BOT) TextBorder|=BOX_BOT;
    if (LineFlags2(LineNo)&LFLAG2_BOX_BETWEEN) TextBorder|=BOX_BETWEEN;
    if (ParaFlags&PARA_BOX_LEFT)   TextBorder|=BOX_LEFT;
    if (ParaFlags&PARA_BOX_RIGHT)  TextBorder|=BOX_RIGHT;

    if (ParaFlags&PARA_BOX_DOUBLE) TextBorder|=BOX_DOUBLE;
    if (ParaFlags&PARA_BOX_THICK)  TextBorder|=BOX_THICK;

    if (tabw(LineNo) && tabw(LineNo)->type&INFO_PAGE) TextBorder=0;
    if (IsHtmlRule(w,pfmt(LineNo))) ResetUintFlag(TextBorder,BOX_BOT);

    return TRUE;
}

/******************************************************************************
   HasSameParaBorder:
   This function compares the paraborders for two para ids and returns TRUE
   if they use the same paraborders.
*******************************************************************************/
BOOL HasSameParaBorder(PTERWND w,long line1, long line2)
{
   UINT ParaFlags1,ParaFlags2;
   int  para1=pfmt(line1),para2=pfmt(line2),LeftBorder1,LeftBorder2;

   if (HtmlMode) return FALSE;   // para borders used for horizontal rules only

   if (tabw(line1) && tabw(line1)->type&(INFO_PAGE|INFO_ROW|INFO_COL)) para1=0;  // delimiter lines don't get borders
   if (LineFlags(line1)&LFLAG_HDRS_FTRS) para1=0;
   if (tabw(line2) && tabw(line2)->type&(INFO_PAGE|INFO_ROW|INFO_COL)) para2=0;
   if (LineFlags(line2)&LFLAG_HDRS_FTRS) para2=0;

   ParaFlags1=PfmtId[para1].flags;
   ParaFlags2=PfmtId[para2].flags;
   if ((ParaFlags1&PARA_BOX_ATTRIB)!=(ParaFlags2&PARA_BOX_ATTRIB)) return FALSE;
   if ((ParaFlags1&PAGE_HDR_FTR)!=(ParaFlags2&PAGE_HDR_FTR)) return FALSE;

   if (PfmtId[para1].BorderSpace!=PfmtId[para2].BorderSpace) return FALSE;
   if (PfmtId[para1].BorderColor!=PfmtId[para2].BorderColor) return FALSE;

   LeftBorder1=PfmtId[para1].LeftIndent;
   if (PfmtId[para1].FirstIndent<0) LeftBorder1+=PfmtId[para1].FirstIndent;
   LeftBorder2=PfmtId[para2].LeftIndent;
   if (PfmtId[para2].FirstIndent<0) LeftBorder2+=PfmtId[para2].FirstIndent;

   if (LeftBorder1!=LeftBorder2) return FALSE;
   if (PfmtId[para1].RightIndent!=PfmtId[para2].RightIndent) return FALSE;
   if (cid(line1)!=cid(line2)) return FALSE;

   return TRUE;
}

/******************************************************************************
   HasSameParaShading:
   This function compares the para shading/colors for two para ids and returns TRUE
   if they use the same.
*******************************************************************************/
BOOL HasSameParaShading(PTERWND w,long line1, long line2)
{
   UINT ParaFlags1,ParaFlags2;
   int  para1=pfmt(line1),para2=pfmt(line2),LeftBorder1,LeftBorder2;

   if (HtmlMode) return FALSE;   // para color/shade not used

   if (tabw(line1) && tabw(line1)->type&INFO_PAGE) para1=0;  // delimiter lines don't get borders
   if (LineFlags(line1)&LFLAG_HDRS_FTRS) para1=0;
   if (tabw(line2) && tabw(line2)->type&INFO_PAGE) para2=0;
   if (LineFlags(line2)&LFLAG_HDRS_FTRS) para2=0;

   if (PfmtId[para1].shading!=PfmtId[para2].shading) return FALSE;
   if (PfmtId[para1].BkColor!=PfmtId[para2].BkColor) return FALSE;

   ParaFlags1=PfmtId[para1].flags;
   ParaFlags2=PfmtId[para2].flags;
   if ((ParaFlags1&PAGE_HDR_FTR)!=(ParaFlags2&PAGE_HDR_FTR)) return FALSE;

   LeftBorder1=PfmtId[para1].LeftIndent;
   if (PfmtId[para1].FirstIndent<0) LeftBorder1+=PfmtId[para1].FirstIndent;
   LeftBorder2=PfmtId[para2].LeftIndent;
   if (PfmtId[para2].FirstIndent<0) LeftBorder2+=PfmtId[para2].FirstIndent;

   if (LeftBorder1!=LeftBorder2) return FALSE;
   if (PfmtId[para1].RightIndent!=PfmtId[para2].RightIndent) return FALSE;
   if (cid(line1)!=cid(line2)) return FALSE;

   return TRUE;
}

/******************************************************************************
    SetTerWindowOrg:
    Set the window origin and clear the text map
*******************************************************************************/
void SetTerWindowOrg(PTERWND w)
{
    // erase any drag objects
    if (PictureHilighted) {
        PaintFlag=PAINT_WIN;
        DrawDragPictRect(w);       // erase the picture rectangle
    }
    if (FrameTabsHilighted) {
        PaintFlag=PAINT_WIN;
        DrawDragFrameTabs(w);     // erase the frame size tabs
    }
    DeleteDragObjects(w,DRAG_TYPE_SCR_FIRST,DRAG_TYPE_SCR_LAST);

    TerDestroyCaret(w);          // hide caret

    SetWindowOrgEx(hTerDC,TerWinOrgX,TerWinOrgY,NULL); // set the logical window origin
    if (hBufDC) SetWindowOrgEx(hBufDC,TerWinOrgX,TerWinOrgY,NULL); // set the logical window origin

    InitCaret(w);                // redisplay the caret

    //***************** reset the text map *************
    DeleteTextMap(w,FALSE);

    RulerPending=TRUE;
}

