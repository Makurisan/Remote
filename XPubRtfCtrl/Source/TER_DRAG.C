/*===============================================================================
   TER_DRAG.C
   TER drag object routines.

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1994)
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
    TerDragObject:
    Drag the current object
*******************************************************************************/
BOOL TerDragObject(PTERWND w,DWORD lParam)
{
    int x,y,DeltaX,DeltaY;
    MSG msg;
    int TotalDeltaX=0,TotalDeltaY=0,delta=0;

    if (CurDragObj<0 || !DragObj[CurDragObj].InUse) return FALSE;
    SetCapture(hTerWnd);                        // capture the mouse


    DO_DRAG:
    // calculate mouse movement
    x=(int)(short)LOWORD(lParam);
    y=(int)(short)HIWORD(lParam);
    
    if (DragObj[CurDragObj].type==DRAG_TYPE_FRAME_MOVE) delta=ScrResX/10;
    if (x<delta) x=delta;
    if (x>(TerRect.right-delta)) x=TerRect.right-delta;
    
    if (y<0) y=0;
    if (y>TerRect.bottom) y=TerRect.bottom;

    DeltaX=x-MouseX;
    DeltaY=y-MouseY;
    TotalDeltaX+=DeltaX;
    TotalDeltaY+=DeltaY;

    // process each object type
    if      (DragObj[CurDragObj].type==DRAG_TYPE_PICT_SIZE)   DragPictFrameSize(w,DRAG_TYPE_PICT_SIZE,DeltaX,DeltaY);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_CELL)        DragCellSize(w,DeltaX);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_ROW)         DragRowSize(w,DeltaY);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_FRAME_SIZE)  DragPictFrameSize(w,DRAG_TYPE_FRAME_SIZE,DeltaX,DeltaY);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_FRAME_MOVE)  DragFrameMove(w,DeltaX,DeltaY);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_ROW_INDENT)  DragRowIndent(w,DeltaX);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_RULER_LEFT)  DragRulerIndent(w,DeltaX);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_RULER_RIGHT) DragRulerIndent(w,DeltaX);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_RULER_FIRST) DragRulerIndent(w,DeltaX);
    else if (DragObj[CurDragObj].type==DRAG_TYPE_RULER_TAB)   DragRulerTab(w,DeltaX,y);

    // record current position for the next movement
    MouseX+=DeltaX;
    MouseY+=DeltaY;


    // wait for the next mosue move message
    while (!PeekMessage(&msg,NULL,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_NOREMOVE|PM_NOYIELD)) {
       // stop if mouse button released
       if (PeekMessage(&msg,NULL,WM_LBUTTONUP,WM_LBUTTONUP,PM_NOREMOVE|PM_NOYIELD)) goto END;
       if (PeekMessage(&msg,NULL,WM_RBUTTONUP,WM_RBUTTONUP,PM_NOREMOVE|PM_NOYIELD)) goto END;
       if (PeekMessage(&msg,NULL,WM_MBUTTONUP,WM_MBUTTONUP,PM_NOREMOVE|PM_NOYIELD)) goto END;
    }

    // retrieve next mouse move message
    if (PeekMessage(&msg,NULL,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_REMOVE|PM_NOYIELD)) {
         lParam=msg.lParam;
         goto DO_DRAG;
    }

    END:

    DragApply(w,TotalDeltaX,TotalDeltaY,x,y);      // apply the cummulative drag movement now

    return TRUE;
}

/*****************************************************************************
    ShowPictureDragObjects:
    Show the drag objects for the current picture
******************************************************************************/
BOOL ShowPictureDragObjects(PTERWND w,int pict)
{
    int  x,y,obj,width,height;
    RECT rect;
    int  DragTabWidth,DragTabHeight;

    if (PictureHilighted) {
       if (CurDragObj>0 && DragObj[CurDragObj].InUse 
          && DragObj[CurDragObj].type==DRAG_TYPE_PICT_SIZE && DragObj[CurDragObj].id1==pict) return true;  // picture already hilighted
       DrawDragPictRect(w);
       DeleteDragObjects(w,DRAG_TYPE_SCR_FIRST,DRAG_TYPE_SCR_LAST);
    }

    // Get an empty slot for the new object
    if ((obj=GetDragObjectSlot(w))<0) return FALSE;

    // fill initial  values
    DragObj[obj].InUse=TRUE;
    DragObj[obj].type=DRAG_TYPE_PICT_SIZE;
    DragObj[obj].id1=pict;         // picture font number
    DragObj[obj].HotRectCount=4;   // 4 hot spot rectangles

    // save for undo
    UndoInt1=TerFont[pict].PictWidth;
    UndoInt2=TerFont[pict].PictHeight;

    // Set the picture dimensions
    width=TwipsToScrX(TerFont[pict].PictWidth);
    height=TwipsToScrY(TerFont[pict].PictHeight);

    DragObj[obj].AspectRatio=((double)width)/height;

    y=TerFont[pict].PictY+TerWinOrgY;

    DragObj[obj].ObjRect.top=y;
    DragObj[obj].ObjRect.bottom=DragObj[obj].ObjRect.top+height;

    DragObj[obj].ObjRect.left=x=TerFont[pict].PictX+TerWinOrgX;   //ColToUnits(w,CurCol,CurLine,LEFT);
    DragObj[obj].ObjRect.right=DragObj[obj].ObjRect.left+width;

    // get the drag tab width/height
    DragTabWidth=TwipsToScrX(DRAG_TAB_WIDTH);
    DragTabHeight=TwipsToScrY(DRAG_TAB_WIDTH);

    // create left tab rectangle
    rect.left=x;
    rect.right=rect.left+DragTabWidth;
    rect.top=y+(height-DragTabHeight)/2;
    rect.bottom=rect.top+DragTabHeight;
    DragObj[obj].HotRect[DRAG_HOT_LEFT]=rect;

    // create right tab
    rect.left=x+width-DragTabWidth;
    rect.right=rect.left+DragTabWidth;
    rect.top=y+(height-DragTabHeight)/2;
    rect.bottom=rect.top+DragTabHeight;
    DragObj[obj].HotRect[DRAG_HOT_RIGHT]=rect;


    // create top tab
    rect.left=x+(width-DragTabWidth)/2;
    rect.right=rect.left+DragTabWidth;
    rect.top=y;
    rect.bottom=rect.top+DragTabHeight;
    DragObj[obj].HotRect[DRAG_HOT_TOP]=rect;

    // create bottom tab
    rect.left=x+(width-DragTabWidth)/2;
    rect.right=rect.left+DragTabWidth;
    rect.top=y+height-DragTabHeight;
    rect.bottom=rect.top+DragTabHeight;
    DragObj[obj].HotRect[DRAG_HOT_BOT]=rect;

    // draw the hot spot rectangles and outside rectangle
    DrawDragPictRect(w);

    if (TerFont[pict].ParaFID>0) FarMove(&(ParaFrame[TerFont[pict].ParaFID]),&UndoParaFrame,sizeof(UndoParaFrame));

    if (TerFont[pict].FrameType==PFRAME_FLOAT) ShowFrameMoveObjects(w,-1,pict);

    return TRUE;
}

/*****************************************************************************
    ShowFrameDragObjects:
    Show the drag objects for the current frame
******************************************************************************/
BOOL ShowFrameDragObjects(PTERWND w,int FrameNo, int ParaFID)
{
    int  x,y,obj,width,height,x1,y1,x2,y2;
    RECT rect;
    int  DragTabWidth,DragTabHeight,SmallTabWidth,SmallTabHeight;
    BOOL IsLine=FALSE;

    if (FrameTabsHilighted) return TRUE;    // object already hilighted

    // Get an empty slot for the size object
    if ((obj=GetDragObjectSlot(w))<0) return FALSE;

    if (ParaFID<0) ParaFID=frame[FrameNo].ParaFrameId;  // picture font number
    
    FarMove(&(ParaFrame[ParaFID]),&UndoParaFrame,sizeof(UndoParaFrame));  // save for undo

    // fill initial  values

    DragObj[obj].InUse=TRUE;
    DragObj[obj].type=DRAG_TYPE_FRAME_SIZE;
    DragObj[obj].id1=ParaFID;  // picture font number
    if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) IsLine=TRUE;

    if (IsLine) DragObj[obj].HotRectCount=2;   // 2 hot spot rectangles
    else        DragObj[obj].HotRectCount=4;   // 4 hot spot rectangles

    // Set the picture dimensions
    width=frame[FrameNo].width;
    height=frame[FrameNo].height;

    DragObj[obj].AspectRatio=((double)width)/height;
    
    // build object space
    if (IsLine) {
       GetLinePoints(w,FrameNo,&x1,&y1,&x2,&y2);  // get the line points
       DragObj[obj].ObjPointCount=2;
       DragObj[obj].ObjPoint[0].x=x1;
       DragObj[obj].ObjPoint[0].y=y1;
       DragObj[obj].ObjPoint[1].x=x2;
       DragObj[obj].ObjPoint[1].y=y2;
       // save the copy of point[0] and point[1] into point[2] and point[3]
       DragObj[obj].ObjPoint[2].x=x1;
       DragObj[obj].ObjPoint[2].y=y1;
       DragObj[obj].ObjPoint[3].x=x2;
       DragObj[obj].ObjPoint[3].y=y2;
    }
    else {
       DragObj[obj].ObjRect.left=x=frame[FrameNo].x;
       DragObj[obj].ObjRect.top=y=frame[FrameNo].y;
       DragObj[obj].ObjRect.right=DragObj[obj].ObjRect.left+width;
       DragObj[obj].ObjRect.bottom=DragObj[obj].ObjRect.top+height;
    }

    // get the drag tab width/height
    DragTabWidth=TwipsToScrX(DRAG_TAB_WIDTH);
    DragTabHeight=TwipsToScrY(DRAG_TAB_WIDTH);
    SmallTabWidth=(DragTabWidth*3)/5;
    SmallTabHeight=(DragTabHeight*3)/5;

    // create left tab rectangle
    if (IsLine) {
       rect.left=x1;                          // add 1 to avoid overlap with the boundary line
       rect.top=y1-DragTabHeight/2;
    }
    else {
       rect.left=x+1;                         // add 1 to avoid overlap with the boundary line
       rect.top=y+(height-DragTabHeight)/2;
    }
    rect.right=rect.left+SmallTabWidth;
    rect.bottom=rect.top+DragTabHeight;
    DragObj[obj].HotRect[DRAG_HOT_LEFT]=rect;

    // create right tab
    if (IsLine) {
       rect.left=x2-SmallTabWidth;
       rect.top=y2-DragTabHeight/2;
    }
    else {
       rect.left=x+width-SmallTabWidth-1;
       rect.top=y+(height-DragTabHeight)/2;
    }
    rect.right=rect.left+SmallTabWidth;
    rect.bottom=rect.top+DragTabHeight;
    DragObj[obj].HotRect[DRAG_HOT_RIGHT]=rect;


    // create top/bottom hot rect tabs for non-line frames
    if (!IsLine) {                            // create top/bottom hot rect tabs for non-line frames
       // create top tab
       rect.left=x+(width-DragTabWidth)/2;
       rect.right=rect.left+DragTabWidth;
       rect.top=y+1;                         // add 1 to avoid overlap with the boundary line
       rect.bottom=rect.top+SmallTabHeight;
       DragObj[obj].HotRect[DRAG_HOT_TOP]=rect;

       // create bottom tab
       rect.left=x+(width-DragTabWidth)/2;
       rect.right=rect.left+DragTabWidth;
       rect.top=y+height-SmallTabHeight-1;
       rect.bottom=y+height-1;
       DragObj[obj].HotRect[DRAG_HOT_BOT]=rect;
    }

    if (ParaFrame[ParaFID].pict>0) 
         ShowFrameMoveObjects(w,FrameNo,ParaFrame[ParaFID].pict);
    else ShowFrameMoveObjects(w,FrameNo,-1);
    return TRUE;
}

/*****************************************************************************
    ShowFrameMoveObjects:
    Show the drag objects for the current frame to move
******************************************************************************/
BOOL ShowFrameMoveObjects(PTERWND w,int FrameNo,int pict)
{
    int obj,ParaFID,x1,x2,y1,y2,width,height,DragTabWidth,DragTabHeight;
    int x,y;
    BOOL IsLine=FALSE;
    RECT rect;

    // Get an empty slot for the move object
    if ((obj=GetDragObjectSlot(w))<0) return FALSE;

    if (pict>0) FrameNo=TerFont[pict].DispFrame;
    ParaFID=frame[FrameNo].ParaFrameId;
    if (pict>0 && TerFont[pict].ParaFID>0) ParaFID=TerFont[pict].ParaFID;

    // get the dimensions
    x=frame[FrameNo].x;
    y=frame[FrameNo].y;
    width=frame[FrameNo].width;
    height=frame[FrameNo].height;

    // get the drag tab width/height
    DragTabWidth=TwipsToScrX(DRAG_TAB_WIDTH);
    DragTabHeight=TwipsToScrY(DRAG_TAB_WIDTH);

    if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) IsLine=TRUE;

    // fill initial  values
    DragObj[obj].InUse=TRUE;
    DragObj[obj].type=DRAG_TYPE_FRAME_MOVE;
    DragObj[obj].id1=frame[FrameNo].ParaFrameId;  // picture font number
    DragObj[obj].HotRectCount=4;   // 4 hot spot rectangles

    // create the object polygon
    if (IsLine) {
       GetLinePoints(w,FrameNo,&x1,&y1,&x2,&y2);  // get the line points
       DragObj[obj].ObjPointCount=2;
       DragObj[obj].ObjPoint[0].x=x1;
       DragObj[obj].ObjPoint[0].y=y1;
       DragObj[obj].ObjPoint[1].x=x2;
       DragObj[obj].ObjPoint[1].y=y2;
    }
    else {
       DragObj[obj].ObjRect.left=x;
       DragObj[obj].ObjRect.top=y;
       DragObj[obj].ObjRect.right=DragObj[obj].ObjRect.left+width;
       DragObj[obj].ObjRect.bottom=DragObj[obj].ObjRect.top+height;
    }

    // create the object hot spots
    if (IsLine) {
       DragObj[obj].IsHotPolygon=TRUE;  // left/top points in HotRect are actually the points of a polygon
       DragObj[obj].HotRect[0].left=x1+DragTabWidth;
       DragObj[obj].HotRect[0].top=y1-DragTabWidth;
       DragObj[obj].HotRect[1].left=x1+DragTabWidth;
       DragObj[obj].HotRect[1].top=y1+DragTabWidth;
       DragObj[obj].HotRect[2].left=x2-DragTabWidth;
       DragObj[obj].HotRect[2].top=y2+DragTabWidth;
       DragObj[obj].HotRect[3].left=x2-DragTabWidth;
       DragObj[obj].HotRect[3].top=y2-DragTabWidth;
    }
    else {
       if (pict>0
           && ( ParaFrame[ParaFID].ShapeType==0 
             || ParaFrame[ParaFID].ShapeType==SHPTYPE_PICT_FRAME)) {   // use the entire picture as hot rectangle
         DragObj[obj].HotRect[DRAG_HOT_LEFT]=DragObj[obj].ObjRect;
         DragObj[obj].HotRect[DRAG_HOT_RIGHT]=DragObj[obj].ObjRect;
         DragObj[obj].HotRect[DRAG_HOT_TOP]=DragObj[obj].ObjRect;
         DragObj[obj].HotRect[DRAG_HOT_BOT]=DragObj[obj].ObjRect;
       }
       else { 
         // create left bar rectangle
         rect.left=x-DragTabWidth;
         rect.right=x;
         rect.top=y;
         rect.bottom=rect.top+height;
         DragObj[obj].HotRect[DRAG_HOT_LEFT]=rect;

         // create right bar
         rect.left=x+width;
         rect.right=rect.left+DragTabWidth;
         rect.top=y;
         rect.bottom=rect.top+height;
         DragObj[obj].HotRect[DRAG_HOT_RIGHT]=rect;

         // create top bar
         rect.left=x-DragTabWidth;
         rect.right=x+width+DragTabWidth;
         rect.top=y-DragTabHeight;
         rect.bottom=y;
         DragObj[obj].HotRect[DRAG_HOT_TOP]=rect;

         // create bottom tab
         rect.left=x-DragTabWidth;
         rect.right=x+width+DragTabWidth;
         rect.top=y+height;
         rect.bottom=rect.top+DragTabHeight;
         DragObj[obj].HotRect[DRAG_HOT_BOT]=rect;
       }
    }

    // draw the hot spot rectangles and outside rectangle
    FrameRectHilighted=FALSE;       // frame move rectangle not be highlighted

    DrawDragFrameTabs(w);           // hilight the frame size rectangle

    return TRUE;
}

/******************************************************************************
    DragPictFrameSize:
    Drag the frame or picture size tabs
*******************************************************************************/
BOOL DragPictFrameSize(PTERWND w,int type, int DeltaX, int DeltaY)
{
    int  obj,width,height,loop,SaveCurHotSpot;
    BOOL IsLine=FALSE,KeepAspect=FALSE;
    int  ParaFID=0;
    BOOL ShiftPressed=false;

    obj=DragObj[CurDragObj].id1;    // picture font or para frame object id
    if (type==DRAG_TYPE_FRAME_SIZE && ParaFrame[obj].flags&PARA_FRAME_LINE) IsLine=TRUE;
    if (type==DRAG_TYPE_PICT_SIZE) {
        ParaFID=TerFont[obj].ParaFID;
        if (ParaFID>0 && True(ParaFrame[ParaFID].flags&PARA_FRAME_LINE)) IsLine=true;
    } 

    // reverse the previous rectangle
    if (type==DRAG_TYPE_PICT_SIZE && PictureHilighted) DrawDragPictRect(w);
    if (type==DRAG_TYPE_FRAME_SIZE) {
       if (FrameTabsHilighted) DrawDragFrameTabs(w);
       if (FrameRectHilighted) DrawDragFrameRect(w);
    }

    // maintain the aspect ratio
    ShiftPressed=GetKeyState(VK_SHIFT)&0x8000;
    if (type==DRAG_TYPE_PICT_SIZE && (TerFlags&TFLAG_KEEP_PICT_ASPECT || ShiftPressed)) KeepAspect=TRUE;
    if (type==DRAG_TYPE_FRAME_SIZE && TerFlags&TFLAG_KEEP_FRAME_ASPECT && !IsLine) KeepAspect=TRUE;
    if (KeepAspect) {
       int width=abs(DragObj[CurDragObj].ObjRect.right-DragObj[CurDragObj].ObjRect.left);
       int height=abs(DragObj[CurDragObj].ObjRect.bottom-DragObj[CurDragObj].ObjRect.top);
       double ar=DragObj[CurDragObj].AspectRatio;
       if (CurHotSpot==DRAG_HOT_LEFT || CurHotSpot==DRAG_HOT_RIGHT) {
           int NewHeight=(int)((width+DeltaX)/ar);
           DeltaY=NewHeight-height;          // MulDiv(DeltaX,height,width);
       }
       else {
          int NewWidth=(int)((height+DeltaY)*ar);
          DeltaX=NewWidth-width;             //MulDiv(DeltaY,width,height);
       }
    }

    // adjust the picture rectangle
    SaveCurHotSpot=CurHotSpot;

    for (loop=0;loop<2;loop++) {
       if (loop==1) {            // maintain aspect ration
          if (!KeepAspect) break;
          if      (CurHotSpot==DRAG_HOT_LEFT)  CurHotSpot=DRAG_HOT_TOP;
          else if (CurHotSpot==DRAG_HOT_TOP)   CurHotSpot=DRAG_HOT_LEFT;
          else if (CurHotSpot==DRAG_HOT_RIGHT) CurHotSpot=DRAG_HOT_BOT;
          else if (CurHotSpot==DRAG_HOT_BOT)   CurHotSpot=DRAG_HOT_RIGHT;
       }

       if (CurHotSpot==DRAG_HOT_LEFT) {
          DragObj[CurDragObj].ObjRect.left+=DeltaX;          // stretch picture rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].left+=DeltaX; // move hot spot rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].right+=DeltaX;
          if (IsLine) {
             DragObj[CurDragObj].ObjPoint[0].x+=DeltaX;           // stretch the line
             DragObj[CurDragObj].ObjPoint[0].y+=DeltaY;           // stretch the line
             DragObj[CurDragObj].HotRect[CurHotSpot].top+=DeltaY; // move hot spot rectangle
             DragObj[CurDragObj].HotRect[CurHotSpot].bottom+=DeltaY;
          }
       }
       if (CurHotSpot==DRAG_HOT_RIGHT) {
          DragObj[CurDragObj].ObjRect.right+=DeltaX;          // stretch picture rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].left+=DeltaX;  // move hot spot rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].right+=DeltaX;
          if (IsLine) {
             DragObj[CurDragObj].ObjPoint[1].x+=DeltaX;            // stretch the line
             DragObj[CurDragObj].ObjPoint[1].y+=DeltaY;            // stretch the line
             DragObj[CurDragObj].HotRect[CurHotSpot].top+=DeltaY;  // move hot spot rectangle
             DragObj[CurDragObj].HotRect[CurHotSpot].bottom+=DeltaY;
          }
       }
       if (CurHotSpot==DRAG_HOT_TOP) {
          DragObj[CurDragObj].ObjRect.top+=DeltaY;            // stretch picture rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].top+=DeltaY;   // move hot spot rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].bottom+=DeltaY;
       }
       if (CurHotSpot==DRAG_HOT_BOT) {
          DragObj[CurDragObj].ObjRect.bottom+=DeltaY;         // stretch picture rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].top+=DeltaY;   // move hot spot rectangle
          DragObj[CurDragObj].HotRect[CurHotSpot].bottom+=DeltaY;
       }
    }
    CurHotSpot=SaveCurHotSpot;

    // calculate the new object size
    if (!IsLine) {
       // adjust the size
       width=DragObj[CurDragObj].ObjRect.right-DragObj[CurDragObj].ObjRect.left;
       height=DragObj[CurDragObj].ObjRect.bottom-DragObj[CurDragObj].ObjRect.top;

       if (width<TwipsToScrX(2*RULER_TOLERANCE)) width=TwipsToScrX(2*RULER_TOLERANCE);
       if (height<TwipsToScrY(2*RULER_TOLERANCE)) height=TwipsToScrY(2*RULER_TOLERANCE);
    }


    if (type==DRAG_TYPE_PICT_SIZE) {
       TerFont[obj].PictWidth=ScrToTwipsX(width);     // convert to point sizes
       TerFont[obj].PictHeight=ScrToTwipsY(height);
       if (ParaFID>0) {                                 // if the picture is a shape
         ParaFrame[ParaFID].width=ScrToTwipsX(width);
         ParaFrame[ParaFID].height=ScrToTwipsY(height);
       }
       
       SetPictSize(w,obj,height,width,TRUE);
       XlateSizeForPrt(w,obj);

       // draw new picture rectangle
       DrawDragPictRect(w);
    }
    else {
       if (!IsLine) {                                  // line gets updated at the end of the operation
          ParaFrame[obj].width=ScrToTwipsX(width);     // convert to twips
          ParaFrame[obj].MinHeight=ParaFrame[obj].height=ScrToTwipsY(height);
          if (ParaFrame[obj].TextAngle>0) ParaFrame[obj].height=ParaFrame[obj].MinHeight;
       }

       // draw new frame rectangle
       DrawDragFrameTabs(w);
       DrawDragFrameRect(w);
    }

    TerArg.modified++;

    return TRUE;
}

/******************************************************************************
    DragFrameMove:
    Drag the frame
*******************************************************************************/
BOOL DragFrameMove(PTERWND w,int DeltaX, int DeltaY)
{
    int  i,ParaFID;
    BOOL IsLine=FALSE;


    // reverse the previous rectangle
    if (FrameRectHilighted) DrawDragFrameRect(w);

    ParaFID=DragObj[CurDragObj].id1;

    if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) IsLine=TRUE;

    // move the frame rectangle of 'move' type
    if (IsLine) {
       DragObj[CurDragObj].ObjPoint[0].x+=DeltaX;
       DragObj[CurDragObj].ObjPoint[1].x+=DeltaX;
       DragObj[CurDragObj].ObjPoint[0].y+=DeltaY;
       DragObj[CurDragObj].ObjPoint[1].y+=DeltaY;
    }
    else {
       DragObj[CurDragObj].ObjRect.left+=DeltaX;
       DragObj[CurDragObj].ObjRect.right+=DeltaX;
       DragObj[CurDragObj].ObjRect.top+=DeltaY;
       DragObj[CurDragObj].ObjRect.bottom+=DeltaY;
    }

    // update the rectangle in the 'size' object also
    for (i=0;i<TotalDragObjs;i++) {
       if (DragObj[i].InUse
         && (DragObj[i].type==DRAG_TYPE_FRAME_SIZE || DragObj[i].type==DRAG_TYPE_PICT_SIZE)) {
          if (IsLine) {
             DragObj[i].ObjPoint[0]=DragObj[CurDragObj].ObjPoint[0];
             DragObj[i].ObjPoint[1]=DragObj[CurDragObj].ObjPoint[1];
          }
          else DragObj[i].ObjRect=DragObj[CurDragObj].ObjRect;
          break;
       }
    }

    // move the hot spots for the 'move' type object
    for (i=0;i<4;i++) {
       DragObj[CurDragObj].HotRect[i].left+=DeltaX;
       DragObj[CurDragObj].HotRect[i].right+=DeltaX;
       DragObj[CurDragObj].HotRect[i].top+=DeltaY;
       DragObj[CurDragObj].HotRect[i].bottom+=DeltaY;
    }

    TerArg.modified++;

    DrawDragFrameRect(w);       // draw the new drag rectangle

    return TRUE;
}

/******************************************************************************
    DragCellSize:
    Drag the cell right border
*******************************************************************************/
BOOL DragCellSize(PTERWND w,int DeltaX)
{
    int i;
    int CurCell,PrevCell,NextCell,TempCell;
    int PrevFrame,NextFrame;
    BOOL DoSelectedCells=FALSE;

    // First the first and the last row and cells affected
    if (DragObj[CurDragObj].id2==-1) {  // when first cell in the column not calculated
       CurCell=DragObj[CurDragObj].id1;

       // select cells if hilight of
       DoSelectedCells=DragObj[CurDragObj].TextHilighted && cell[CurCell].flags&(CFLAG_SEL1|CFLAG_SEL2);

       // get the first cell in the column
       PrevCell=CurCell;
       PrevFrame=-1;
       while(TRUE) {
          if ((TempCell=GetPrevCellInColumnPos(w,PrevCell,TRUE))==-1) break;
          if (DoSelectedCells && !cell[TempCell].flags&(CFLAG_SEL1|CFLAG_SEL2)) break;
          for (i=0;i<TotalFrames;i++) if (frame[i].CellId==TempCell) break;
          if (i<TotalFrames) PrevFrame=i;    // first cell in the frames

          PrevCell=TempCell;                // first cell affected
       }

       // get the last cell in the column
       NextCell=CurCell;
       NextFrame=-1;
       while(TRUE) {
          if ((TempCell=GetNextCellInColumnPos(w,NextCell))==-1) break;
          if (DoSelectedCells && !cell[TempCell].flags&(CFLAG_SEL1|CFLAG_SEL2)) break;
          for (i=0;i<TotalFrames;i++) if (frame[i].CellId==TempCell) break;
          if (i<TotalFrames) NextFrame=i;        // last cell in the frames

          NextCell=TempCell;                // last cell affected
       }

       // update the focus line point
       if (PrevFrame>=0) DragObj[CurDragObj].ObjPoint[0].y=frame[PrevFrame].ScrY;
       if (NextFrame>=0) DragObj[CurDragObj].ObjPoint[1].y=frame[NextFrame].ScrY+frame[NextFrame].ScrHeight;
       DragObj[CurDragObj].id2=PrevCell;
       DragObj[CurDragObj].id3=NextCell;
    }

    // reverse the previous cell line
    if (DragObj[CurDragObj].drawn) DrawDragCellLine(w);

    // adjust the hot spot position
    DragObj[CurDragObj].HotRect[CurHotSpot].left+=DeltaX; // move hot spot rectangle
    DragObj[CurDragObj].HotRect[CurHotSpot].right+=DeltaX;

    // adjust the polygon position
    for (i=0;i<DragObj[CurDragObj].ObjPointCount;i++) DragObj[CurDragObj].ObjPoint[i].x+=DeltaX;

    // draw new cell line
    DrawDragCellLine(w);

    TerArg.modified++;

    return TRUE;
}

/******************************************************************************
    DragRowIndent:
    Drag the row indentation line for the table
*******************************************************************************/
BOOL DragRowIndent(PTERWND w,int DeltaX)
{
    int i;
    int CurRowId,PrevRowId,NextRowId,TempRowId;
    int PrevFrame,NextFrame;

    // First the first and the last row and cells affected
    if (DragObj[CurDragObj].id2==-1) {  // when first cell in the column not calculated
       CurRowId=DragObj[CurDragObj].id1;

       // get the first row in the table
       PrevRowId=CurRowId;
       PrevFrame=-1;
       while(TRUE) {
          if ((TempRowId=TableRow[PrevRowId].PrevRow)==-1) break;

          for (i=0;i<TotalFrames;i++) if (frame[i].RowId==TempRowId) break;
          if (i<TotalFrames) PrevFrame=i;        // cell in the frames

          PrevRowId=TempRowId;                // first cell affected
       }

       // get the last cell in the column
       NextRowId=CurRowId;
       NextFrame=-1;
       while(TRUE) {
          if ((TempRowId=TableRow[NextRowId].NextRow)==-1) break;
          for (i=0;i<TotalFrames;i++) if (frame[i].RowId==TempRowId) break;
          if (i<TotalFrames) NextFrame=i;        // cell in the frames

          NextRowId=TempRowId;                // last cell affected
       }

       // update the focus line point
       if (PrevFrame>=0) DragObj[CurDragObj].ObjPoint[0].y=frame[PrevFrame].ScrY;
       if (NextFrame>=0) DragObj[CurDragObj].ObjPoint[1].y=frame[NextFrame].ScrY+frame[NextFrame].ScrHeight;
       DragObj[CurDragObj].id2=PrevRowId;
       DragObj[CurDragObj].id3=NextRowId;
    }

    // reverse the previous cell line
    if (DragObj[CurDragObj].drawn) DrawDragCellLine(w);

    // adjust the hot spot position
    DragObj[CurDragObj].HotRect[CurHotSpot].left+=DeltaX; // move hot spot rectangle
    DragObj[CurDragObj].HotRect[CurHotSpot].right+=DeltaX;

    // adjust the polygon position
    for (i=0;i<DragObj[CurDragObj].ObjPointCount;i++) DragObj[CurDragObj].ObjPoint[i].x+=DeltaX;

    // draw new cell line
    DrawDragCellLine(w);

    TerArg.modified++;

    return TRUE;
}

/******************************************************************************
    DragRowSize:
    Drag the cell right border
*******************************************************************************/
BOOL DragRowSize(PTERWND w,int DeltaY)
{
    int i;
    int CurCell,PrevCell,NextCell,TempCell;
    int PrevFrame,NextFrame;

    // First the first and the last row and cells affected
    if (DragObj[CurDragObj].id2==-1) {  // when first cell in the row not calculated
       CurCell=DragObj[CurDragObj].id1;

       // get the first cell in the row
       PrevCell=CurCell;
       PrevFrame=-1;
       while(TRUE) {
          if ((TempCell=cell[PrevCell].PrevCell)<=0) break;
          if (cell[TempCell].flags&CFLAG_ROW_SPANNED) TempCell=CellAux[TempCell].SpanningCell;  // merged cell
          if (TempCell<=0) break;
          for (i=0;i<TotalFrames;i++) if (frame[i].CellId==TempCell) break;
          if (i<TotalFrames) PrevFrame=i;    // first cell in the frames

          PrevCell=TempCell;                // first cell affected
       }

       // get the last cell in the column
       NextCell=CurCell;
       NextFrame=-1;
       while(TRUE) {
          if ((TempCell=cell[NextCell].NextCell)<=0) break;
          if (cell[TempCell].flags&CFLAG_ROW_SPANNED) TempCell=CellAux[TempCell].SpanningCell;  // merged cell
          if (TempCell<=0) break;
          for (i=0;i<TotalFrames;i++) if (frame[i].CellId==TempCell) break;
          if (i<TotalFrames) NextFrame=i;        // last cell in the frames

          NextCell=TempCell;                // last cell affected
       }

       // update the focus line point
       if (PrevFrame>=0) DragObj[CurDragObj].ObjPoint[0].x=frame[PrevFrame].ScrX;
       if (NextFrame>=0) DragObj[CurDragObj].ObjPoint[1].x=frame[NextFrame].ScrX+frame[NextFrame].ScrWidth;
       DragObj[CurDragObj].id2=PrevCell;
       DragObj[CurDragObj].id3=NextCell;
    }

    // reverse the previous cell line
    if (DragObj[CurDragObj].drawn) DrawDragRowLine(w);

    // adjust the hot spot position
    DragObj[CurDragObj].HotRect[CurHotSpot].top+=DeltaY; // move hot spot rectangle
    DragObj[CurDragObj].HotRect[CurHotSpot].bottom+=DeltaY;

    // adjust the polygon position
    for (i=0;i<DragObj[CurDragObj].ObjPointCount;i++) DragObj[CurDragObj].ObjPoint[i].y+=DeltaY;

    // draw new cell line
    DrawDragRowLine(w);

    TerArg.modified++;

    return TRUE;
}


/******************************************************************************
    DragRulerIndent:
    Drag left/right indentaion mark on the ruler.  The actual indentation
    is applied only after the mouse button is released.
*******************************************************************************/
BOOL DragRulerIndent(PTERWND w,int DeltaX)
{
    int i;

    // reverse the previous picture rectangle
    if (DragObj[CurDragObj].drawn) DrawDragRulerIndent(w);

    // adjust the hot spot position
    DragObj[CurDragObj].HotRect[CurHotSpot].left+=DeltaX; // move hot spot rectangle
    DragObj[CurDragObj].HotRect[CurHotSpot].right+=DeltaX;

    // adjust the polygon position
    for (i=0;i<DragObj[CurDragObj].ObjPointCount;i++) DragObj[CurDragObj].ObjPoint[i].x+=DeltaX;


    TerArg.modified++;
    RulerPending=TRUE;

    // draw new picture rectangle
    DrawDragRulerIndent(w);

    return TRUE;
}

/******************************************************************************
    DragRulerTab:
    Move a tab position.  The actual tab position is modified only after the
    mouse button is released.
*******************************************************************************/
BOOL DragRulerTab(PTERWND w,int DeltaX, int LastY)
{
    int i;

    // reverse the previous picture rectangle
    if (DragObj[CurDragObj].drawn) DrawDragRulerTab(w,LastY);

    // adjust the hot spot position
    DragObj[CurDragObj].HotRect[CurHotSpot].left+=DeltaX; // move hot spot rectangle
    DragObj[CurDragObj].HotRect[CurHotSpot].right+=DeltaX;

    // adjust the polygon position
    for (i=0;i<DragObj[CurDragObj].ObjPointCount;i++) DragObj[CurDragObj].ObjPoint[i].x+=DeltaX;

    TerArg.modified++;
    RulerPending=TRUE;

    // draw new picture rectangle
    DrawDragRulerTab(w,LastY);

    return TRUE;
}

/******************************************************************************
    DragApply:
    Some objects are modified only after the mousebutton is released.  This
    function updates those objects.
*******************************************************************************/
BOOL DragApply(PTERWND w,int DeltaX,int DeltaY, int LastX, int LastY)
{
    int i,DeltaXTwips,DeltaYTwips;
    int TabNo,TabId,TabPos,TabType;

    DeltaXTwips=ScrToTwipsX(DeltaX);
    DeltaYTwips=ScrToTwipsY(DeltaY);

    // update objects
    if      (DragObj[CurDragObj].type==DRAG_TYPE_RULER_LEFT)  {// left indent
       if (DragObj[CurDragObj].drawn) DrawDragRulerIndent(w);  // switch off the focus object
       if (LineFlags2(CurLine)&LFLAG2_RTL) DeltaXTwips=-DeltaXTwips;
       if (HilightType!=HILIGHT_OFF && True(TerFlags5&TFLAG5_RULER_INDENT_FIXED)) {  // apply fixed ruler indent to all lines in the selected block
          int CurPara=DragObj[CurDragObj].id1;
          int indent=PfmtId[CurPara].LeftIndentTwips+DeltaXTwips;
          if (indent<0) indent=0;
          TerSetParaIndent(hTerWnd,indent,-1,PfmtId[CurPara].FirstIndentTwips-DeltaXTwips,FALSE);
       } 
       else ParaIndentTwips(hTerWnd,DeltaXTwips,0,-DeltaXTwips,FALSE);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_RULER_RIGHT) {// right indent
       if (DragObj[CurDragObj].drawn) DrawDragRulerIndent(w);  // switch off the focus object
       if (LineFlags2(CurLine)&LFLAG2_RTL) DeltaXTwips=-DeltaXTwips;
       if (HilightType!=HILIGHT_OFF && True(TerFlags5&TFLAG5_RULER_INDENT_FIXED)) {  // apply fixed ruler indent to all lines in the selected block
          int CurPara=DragObj[CurDragObj].id1;
          int indent=PfmtId[CurPara].RightIndentTwips-DeltaXTwips;
          if (indent<0) indent=0;
          TerSetParaIndent(hTerWnd,-1,indent,-1,FALSE);
       } 
       else ParaIndentTwips(hTerWnd,0,-DeltaXTwips,0,FALSE);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_RULER_FIRST) {// first line indent
       if (DragObj[CurDragObj].drawn) DrawDragRulerIndent(w);  // switch off the focus object
       if (LineFlags2(CurLine)&LFLAG2_RTL) DeltaXTwips=-DeltaXTwips;
       if (HilightType!=HILIGHT_OFF && True(TerFlags5&TFLAG5_RULER_INDENT_FIXED)) {  // apply fixed ruler indent to all lines in the selected block
          int CurPara=DragObj[CurDragObj].id1;
          int indent=PfmtId[CurPara].FirstIndentTwips+DeltaXTwips;
          if (indent<0) indent=0;
          TerSetParaIndent(hTerWnd,PfmtId[CurPara].LeftIndentTwips,-1,indent,FALSE);
       } 
       else ParaIndentTwips(hTerWnd,0,0,DeltaXTwips,FALSE);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_RULER_TAB) {  // tab position
       if (DragObj[CurDragObj].drawn) DrawDragRulerTab(w,LastY);     // switch off the focus object

       //Clear the old tab position and create a new onen
       TabNo=DragObj[CurDragObj].id1;
       if (EditingParaStyle) TabId=StyleId[CurSID].TabId;
       else                  TabId=PfmtId[pfmt(CurLine)].TabId;

       if (LineFlags2(CurLine)&LFLAG2_RTL) DeltaXTwips=-DeltaXTwips;

       if (TabNo<TerTab[TabId].count) {
          BYTE flags=TerTab[TabId].flags[TabNo];
          TabType=TerTab[TabId].type[TabNo];
          TabPos=TerTab[TabId].pos[TabNo]+DeltaXTwips;
          if (SnapToGrid) {
             if (TerFlags&TFLAG_METRIC) TabPos=RoundInt(TabPos,SNAP_DIST_METRIC);
             else                       TabPos=RoundInt(TabPos,SNAP_DIST);
          }

          ClearTab(hTerWnd,TerTab[TabId].pos[TabNo],FALSE); // clear this tab position
          if (TabPos>0 && LastY>=RulerRect.top && LastY<RulerRect.bottom)
             TerSetTab(hTerWnd,TabType,TabPos,flags,FALSE);             // set new tab, do not refresh screen now
       }
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_ROW) {    // table cell width
       int row,CurCell,MinHeight;

       if (DragObj[CurDragObj].drawn) DrawDragRowLine(w);  // switch off the focus object

       // calculate first table row, last table row and current column
       CurCell=DragObj[CurDragObj].id1;
       
       SaveUndo(w,-1,CurCell,-1,CurCell,UNDO_TABLE_ATTRIB);   // specify cell instead of lines
       
       row=cell[CurCell].row;

       MinHeight=TableRow[row].MinHeight;
       if      (MinHeight>=0) TableRow[row].MinHeight=PrtToTwipsY(TableRow[row].height)+DeltaYTwips;
       else                   TableRow[row].MinHeight-=DeltaYTwips;

       DeleteTextMap(w,TRUE);
       RequestPagination(w,TRUE);
       //ReleaseUndo(w);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_CELL) {    // table cell width
       int FirstRow,LastRow,BorderX,NextRow,cl,delta;
       int FirstCell,LastCell,NextCell,CurCell;
       BOOL DoLeftCellOnly=(BOOL)(TerFlags4&TFLAG4_ADJ_LEFT_TABLE_COL);

       if (DragObj[CurDragObj].drawn) DrawDragCellLine(w);  // switch off the focus object

       // calculate first table row, last table row and current column
       CurCell=DragObj[CurDragObj].id1;
       FirstCell=DragObj[CurDragObj].id2;
       LastCell=DragObj[CurDragObj].id3;
       if (FirstCell==-1) FirstCell=CurCell;
       if (LastCell==-1)  LastCell=CurCell;
       
       SaveUndo(w,-1,FirstCell,-1,LastCell,UNDO_TABLE_ATTRIB);   // specify cell instead of lines

       FirstRow=cell[FirstCell].row;
       LastRow=cell[LastCell].row;
       BorderX=GetCellRightX(w,CurCell);

       // update each table row
       NextRow=FirstRow;
       while (TRUE) {                    // scan each row
          cl=TableRow[NextRow].FirstCell;
          while (TRUE) {                 // scan each cell for cells with the same right boundary
             delta=GetCellRightX(w,cl)-BorderX;

             if (abs(delta)<RULER_TOLERANCE) {   // cell found
                int RowDeltaX;
                // update the next cell in the row
                RowDeltaX=DeltaXTwips;
                if (TableRow[NextRow].flags&ROWFLAG_RTL) RowDeltaX=-RowDeltaX;
                
                NextCell=cell[cl].NextCell;
                if ((cell[cl].width+RowDeltaX)<MIN_CELL_WIDTH) break;
                if (NextCell>0 && !DoLeftCellOnly && (cell[NextCell].width-RowDeltaX)<MIN_CELL_WIDTH) break;

                if (SnapToGrid) {
                    int x=cell[cl].x+cell[cl].width+RowDeltaX;
                    if (TerFlags&TFLAG_METRIC) x=RoundInt(x,SNAP_DIST_METRIC);
                    else                       x=RoundInt(x,SNAP_DIST);
                    RowDeltaX=x-cell[cl].x-cell[cl].width;
                }

                cell[cl].width+=RowDeltaX;
                if (HtmlMode) {
                   cell[cl].FixWidth=cell[cl].width;
                   ResetUintFlag(cell[cl].flags,CFLAG_FIX_WIDTH_PCT);
                   cell[cl].flags|=CFLAG_FIX_WIDTH;
                } 
                
                if (NextCell>0 && (!DoLeftCellOnly)) {
                   cell[NextCell].width-=RowDeltaX;
                   cell[NextCell].x+=RowDeltaX;

                   if (HtmlMode) {
                      cell[NextCell].FixWidth=cell[NextCell].width;
                      ResetUintFlag(cell[NextCell].flags,CFLAG_FIX_WIDTH_PCT);
                      cell[NextCell].flags|=CFLAG_FIX_WIDTH;
                   } 
                }

                break;
             }
             if (cell[cl].NextCell==-1) break;  // last cell of the table reached
             cl=cell[cl].NextCell;
          }

          // advance to the next row
          if (NextRow==LastRow) break;
          NextRow=TableRow[NextRow].NextRow;
          if (NextRow==-1) break;
       }
       //***************** reset the text map *************
       DeleteTextMap(w,TRUE);
       RequestPagination(w,TRUE);
       //ReleaseUndo(w);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_ROW_INDENT) { // table cell width
       int FirstRow,CurRowId,LastRow,NextRow;
       int NextCell,CurX,RowWidth,CellCount,MinCellWidth=180;

       if (DragObj[CurDragObj].drawn) DrawDragCellLine(w);  // switch off the focus object

       // calculate first table row, last table row and current column
       CurRowId=DragObj[CurDragObj].id1;
       FirstRow=DragObj[CurDragObj].id2;
       LastRow=DragObj[CurDragObj].id3;
       if (FirstRow==-1) FirstRow=CurRowId;
       if (LastRow==-1)  LastRow=CurRowId;

       SaveUndo(w,-1,TableRow[FirstRow].FirstCell,-1,TableRow[LastRow].FirstCell,UNDO_TABLE_ATTRIB);   // specify cell instead of lines

       // update each table row
       NextRow=FirstRow;
       while (NextRow>0) {
          int RowDeltaX=DeltaXTwips;
          if (SnapToGrid) {
              int x=TableRow[NextRow].indent+RowDeltaX;
              if (TerFlags&TFLAG_METRIC) x=RoundInt(x,SNAP_DIST_METRIC);
              else                       x=RoundInt(x,SNAP_DIST);
              RowDeltaX=x-TableRow[NextRow].indent;
              if (TableRow[NextRow].flags&ROWFLAG_RTL) RowDeltaX=-RowDeltaX;
          }

          if ((TableRow[NextRow].indent+RowDeltaX)>=0) {
             // find the row width
             RowWidth=0;
             NextCell=TableRow[NextRow].FirstCell;
             CellCount=0;
             while (NextCell>0) {
                RowWidth+=cell[NextCell].width;
                NextCell=cell[NextCell].NextCell;
                CellCount++;
             }

             // set the table indentation
             if (RowDeltaX>=(RowWidth-MinCellWidth*CellCount)) {
                RowDeltaX=RowWidth-MinCellWidth*CellCount;
                if (RowDeltaX<0) RowDeltaX=0;
             }
             TableRow[NextRow].indent+=RowDeltaX;
             CurX=TableRow[NextRow].indent;

             // update cell position and widths
             NextCell=TableRow[NextRow].FirstCell;
             while (NextCell>0) {
                cell[NextCell].x=CurX;
                cell[NextCell].width-=(int)(((long)cell[NextCell].width*RowDeltaX)/RowWidth);
                if (cell[NextCell].width<MinCellWidth) cell[NextCell].width=MinCellWidth;

                if (HtmlMode) {
                   cell[NextCell].FixWidth=cell[NextCell].width;
                   ResetUintFlag(cell[NextCell].flags,CFLAG_FIX_WIDTH_PCT);
                   cell[NextCell].flags|=CFLAG_FIX_WIDTH;
                } 

                CurX=CurX+cell[NextCell].width;

                NextCell=cell[NextCell].NextCell;
             }
          }

          NextRow=TableRow[NextRow].NextRow;
       }
       //***************** reset the text map *************
       DeleteTextMap(w,TRUE);
       RequestPagination(w,TRUE);
       //ReleaseUndo(w);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_FRAME_SIZE) { // frame being sized
       int ParaFID=DragObj[CurDragObj].id1;
       bool UndoReleased=false;

       if (FrameRectHilighted) DrawDragFrameRect(w);
       FitPictureInFrame(w,CurLine,TRUE);   // resize picture in frame

       TerOpFlags2|=TOFLAG2_USE_UNDO_FRAME;  // used the saved ParaFrame information for undo

       if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) {
          DragApplyLineSize(w,DeltaX,DeltaY);
       }
       else if (CurHotSpot==DRAG_HOT_LEFT || CurHotSpot==DRAG_HOT_TOP) {  // move to new location
          int x,y;              // location of the top left of the frame in twips
          x=ScrToTwipsX(DragObj[CurDragObj].ObjRect.left);
          y=DragObj[CurDragObj].ObjRect.top;
          if (y>FirstPageHeight) y-=FirstPageHeight;  // relative to the top of the page
          y=ScrToTwipsY(y);
          if (BorderShowing) y-=PrtToTwipsY(TopBorderHeight);
          if (BorderShowing) x-=PrtToTwipsX(GetBorderLeftSpace(w,CurPage));
          TerMoveParaFrame(hTerWnd,DragObj[CurDragObj].id1,x,y,-1,-1);     // move the frame
       }
       else {   // invalidate para space text segments
          SaveUndo(w,ParaFID,0,0,0,UNDO_FRAME_LIMITED);   // anchor does not change, so do limited save for undo

          for (i=0;i<TotalSegments;i++) if (TextSeg[i].ParFrameSpace) DeleteTextSeg(w,i,BkPictId?TRUE:FALSE);
          Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint
       }
       ResetUintFlag(TerOpFlags2,TOFLAG2_USE_UNDO_FRAME);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_PICT_SIZE) { // picture being sized
       int pict=DragObj[CurDragObj].id1;
       int ParaFID=TerFont[pict].ParaFID;

       // save for undo
       TerOpFlags2|=TOFLAG2_USE_UNDO_FRAME;  // used the saved ParaFrame information for undo
       if (ParaFID>0) SaveUndo(w,ParaFID,0,0,0,UNDO_FRAME_LIMITED);
       else           SaveUndo(w,pict,0,0,0,UNDO_PICT_SIZE);
       ResetUintFlag(TerOpFlags2,TOFLAG2_USE_UNDO_FRAME);

       TerFont[pict].flags|=FFLAG_RESIZED;
       if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;
       if (TerFont[pict].FrameType!=PFRAME_NONE && ParaFID>0) {
          Repaginate(w,FALSE,TRUE,0,TRUE);
       }
       if (TerFont[pict].ObjectType!=OBJ_NONE) UpdateOleObj(w,pict);
    }
    else if (DragObj[CurDragObj].type==DRAG_TYPE_FRAME_MOVE) { // frame being moved
       int x,y,ParaFID;    // location of the top left of the frame in twips
       BOOL UndoReleased=false;

       if (FrameRectHilighted) DrawDragFrameRect(w);

       ParaFID=DragObj[CurDragObj].id1;
       if (ParaFrame[ParaFID].pict>0)  {
          TerMovePictFrame2(hTerWnd,ParaFrame[ParaFID].pict,DeltaXTwips,DeltaYTwips);
       }
       else {
          int page;
          if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE) {
             x=ScrToTwipsX(DragObj[CurDragObj].ObjPoint[0].x);
             if (ParaFrame[ParaFID].LineType==DOL_DIAG)
                   y=DragObj[CurDragObj].ObjPoint[1].y;
             else  y=DragObj[CurDragObj].ObjPoint[0].y;
          }
          else {
             x=ScrToTwipsX(DragObj[CurDragObj].ObjRect.left);
             y=DragObj[CurDragObj].ObjRect.top;
          }
          page=FirstFramePage;
          if (y>FirstPageHeight) {
             y-=FirstPageHeight;  // relative to the top of the page
             page++;
          }
          y=ScrToTwipsY(y);
          
          if (BorderShowing) y-=PrtToTwipsY(TopBorderHeight);
          if (BorderShowing) x-=PrtToTwipsX(GetBorderLeftSpace(w,CurPage));

          TerMoveParaFrame2(hTerWnd,DragObj[CurDragObj].id1,x,y,-1,-1,page);     // move the frame
       }
    }

    return TRUE;
}

/*****************************************************************************
    ApplyLineSize:
    Apply the line size adjustment
******************************************************************************/
BOOL DragApplyLineSize(PTERWND w,int DeltaX, int DeltaY)
{
    int ParaFID,x1,x2,y1,y2,SaveX,SaveParaY,SaveY,page;

    ParaFID=DragObj[CurDragObj].id1;    // picture font or para frame object id

    // recreate the line rect
    SaveX=ParaFrame[ParaFID].x;   // save the original x/y for the TerMoveParaFrame function to use
    SaveY=ParaFrame[ParaFID].y;
    SaveParaY=ParaFrame[ParaFID].ParaY;

    // update the y from the screen variables
    if (ParaFrame[ParaFID].LineType==DOL_DIAG)
          ParaFrame[ParaFID].y=ScrToTwipsY(DragObj[CurDragObj].ObjPoint[3].y);   // use the unmodified values
    else  ParaFrame[ParaFID].y=ScrToTwipsY(DragObj[CurDragObj].ObjPoint[2].y);

    // recreate original points relative to page
    LineRectToPoints(w,ParaFID,&x1,&y1,&x2,&y2);

    // apply the adjustment
    if (CurHotSpot==DRAG_HOT_LEFT) {
       x1+=ScrToTwipsX(DeltaX);
       y1+=ScrToTwipsY(DeltaY);
       if (abs(x1-x2)<RULER_TOLERANCE) x1=x2;   // to ensure clear horizontal and vertical lines
       if (abs(y1-y2)<RULER_TOLERANCE) y1=y2;
    }
    else if (CurHotSpot==DRAG_HOT_RIGHT) {
       x2+=ScrToTwipsX(DeltaX);
       y2+=ScrToTwipsY(DeltaY);
       if (abs(x1-x2)<RULER_TOLERANCE) x2=x1;   // to ensure clear horizontal and vertical lines
       if (abs(y1-y2)<RULER_TOLERANCE) y2=y1;
    }

    LinePointsToRect(w,ParaFID,x1,y1,x2,y2);

    //x1=ParaFrame[ParaFID].x;      // new rectangle location relative to page
    //y1=ParaFrame[ParaFID].y;
    //if (BorderShowing) y1-=PrtToTwipsY(TopBorderHeight);

    x1=ParaFrame[ParaFID].x;      // new rectangle location relative to page
    y1=TwipsToScrY(ParaFrame[ParaFID].y);
    page=FirstFramePage;
    if (y1>=FirstPageHeight) {
       y1-=FirstPageHeight;  // relative to the top of the page
       page++;
    }
    y1=ScrToTwipsY(y1);
    if (BorderShowing) y1-=PrtToTwipsY(TopBorderHeight);
    //if (BorderShowing) x1-=PrtToTwipsX(GetBorderLeftSpace(w,CurPage));

    
    ParaFrame[ParaFID].x=SaveX;  // restore original value as TerMoveParaFrame needs the original value
    ParaFrame[ParaFID].y=SaveY;
    ParaFrame[ParaFID].ParaY=SaveParaY;

    TerMoveParaFrame2(hTerWnd,ParaFID,x1,y1,-1,-1,page);     // move the frame

    LineRectToPoints(w,ParaFID,&x1,&y1,&x2,&y2);

    return TRUE;
}

/*****************************************************************************
    DrawDragPictRect:
    This function draws picture selection rectangle and the hot spot tabs.
******************************************************************************/
BOOL DrawDragPictRect(PTERWND w)
{
    int i;

    // draw the rectangle
    for (i=0;i<TotalDragObjs;i++) {
       if (DragObj[i].InUse && DragObj[i].type==DRAG_TYPE_PICT_SIZE) {
          TerDrawRect(w,&(DragObj[i].ObjRect),hFocusPen,TRUE,TRUE);
          break;
       }
    }

    DrawDragHotSpots(w,DRAG_TYPE_PICT_SIZE);

    PictureHilighted=!PictureHilighted;

    return TRUE;
}

/*****************************************************************************
    DrawDragFrameTabs:
    This function draws frame hot spot tabs.
******************************************************************************/
BOOL DrawDragFrameTabs(PTERWND w)
{
    // draw the hotspots
    DrawDragHotSpots(w,DRAG_TYPE_FRAME_SIZE);

    FrameTabsHilighted=!FrameTabsHilighted;

    return TRUE;
}

/*****************************************************************************
    DrawDragFrameRect:
    This function draws frame rectangle hilighting
******************************************************************************/
BOOL DrawDragFrameRect(PTERWND w)
{
    int i;
    int ParaFID;

    // draw the rectangle
    for (i=0;i<TotalDragObjs;i++) {
       if ( DragObj[i].InUse
          && (DragObj[i].type==DRAG_TYPE_FRAME_SIZE || DragObj[i].type==DRAG_TYPE_PICT_SIZE)) {

          if (DragObj[i].type==DRAG_TYPE_FRAME_SIZE) ParaFID=DragObj[i].id1;
          else {
             int pict=DragObj[i].id1;
             ParaFID=TerFont[pict].ParaFID;
          }

          if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE)
               TerDrawLine(w,DragObj[i].ObjPoint[0].x,
                             DragObj[i].ObjPoint[0].y,
                             DragObj[i].ObjPoint[1].x,
                             DragObj[i].ObjPoint[1].y,
                             hFocusPen,NULL,TRUE);
          else TerDrawRect(w,&(DragObj[i].ObjRect),hFocusPen,TRUE,TRUE);
          break;
       }
    }

    FrameRectHilighted=!FrameRectHilighted;

    return TRUE;
}

/*****************************************************************************
    DrawDragRulerIndent:
    This function draws ruler indentation selection polygon.  The object
    is given by the CurDragObj global variable.
******************************************************************************/
BOOL DrawDragRulerIndent(PTERWND w)
{
    POINT pt[6];
    int i,idx;

    // draw the polygon
    for (i=0;i<DragObj[CurDragObj].ObjPointCount;i++) {    // apply the y offset
       pt[i]=DragObj[CurDragObj].ObjPoint[i];
       pt[i].y=pt[i].y+TerWinOrgY+RulerRect.top-TerWinRect.top;
    }

    TerDrawPolygon(w,pt,
                     DragObj[CurDragObj].ObjPointCount,
                     GetStockObject(BLACK_PEN),GetStockObject(NULL_BRUSH),&RulerRect,TRUE);

    if (DragObj[CurDragObj].ObjPointCount==5) idx=0;  // 5 point indentation symbol
    else                                      idx=1;

    TerDrawLine(w,DragObj[CurDragObj].ObjPoint[idx].x,
                  TerWinOrgY,
                  DragObj[CurDragObj].ObjPoint[idx].x,
                  TerWinOrgY+TerWinRect.bottom-TerWinRect.top,
                  hFocusPen,NULL,TRUE);

    DragObj[CurDragObj].drawn=!DragObj[CurDragObj].drawn;

    return TRUE;
}

/*****************************************************************************
    DrawDragRulerTab:
    This function draws ruler tab selection line.  The object
    is given by the CurDragObj global variable.
******************************************************************************/
BOOL DrawDragRulerTab(PTERWND w, int LastY)
{
    BOOL draw=(LastY>=RulerRect.top && LastY<=RulerRect.bottom)?TRUE:FALSE;

    // draw the line
    if (draw || DragObj[CurDragObj].drawn) {
       TerDrawLine(w,DragObj[CurDragObj].ObjPoint[0].x,
                     DragObj[CurDragObj].ObjPoint[0].y,
                     DragObj[CurDragObj].ObjPoint[1].x,
                     DragObj[CurDragObj].ObjPoint[1].y,
                     hFocusPen,&RulerRect,TRUE);

       TerDrawLine(w,DragObj[CurDragObj].ObjPoint[1].x,
                     DragObj[CurDragObj].ObjPoint[1].y,
                     DragObj[CurDragObj].ObjPoint[1].x,
                     TerWinOrgY+TerWinRect.bottom-TerWinRect.top,
                     hFocusPen,&TerWinRect,TRUE);

       DragObj[CurDragObj].drawn=!DragObj[CurDragObj].drawn;
    }

    return TRUE;
}

/*****************************************************************************
    DrawDragCellLine:
    This function draws right cell border.  The object
    is given by the CurDragObj global variable.
******************************************************************************/
BOOL DrawDragCellLine(PTERWND w)
{
    // draw the line
    TerDrawLine(w,DragObj[CurDragObj].ObjPoint[0].x,
                  DragObj[CurDragObj].ObjPoint[0].y,
                  DragObj[CurDragObj].ObjPoint[1].x,
                  DragObj[CurDragObj].ObjPoint[1].y,
                  hFocusPen,&TerWinRect,TRUE);

    // draw the line on the ruler
    if (TerArg.ruler) {
        int y1=TerWinOrgY-(TerWinRect.top-RulerRect.top);
        int y2=y1+(RulerRect.bottom-RulerRect.top);
        TerDrawLine(w,DragObj[CurDragObj].ObjPoint[0].x,y1,
                    DragObj[CurDragObj].ObjPoint[1].x,y2,
                    GetStockObject(BLACK_PEN),&RulerRect,TRUE);
    }

    DragObj[CurDragObj].drawn=!DragObj[CurDragObj].drawn;

    return TRUE;
}

/*****************************************************************************
    DrawDragRowLine:
    This function draws right cell border.  The object
    is given by the CurDragObj global variable.
******************************************************************************/
BOOL DrawDragRowLine(PTERWND w)
{
    // draw the line
    TerDrawLine(w,DragObj[CurDragObj].ObjPoint[0].x,
                  DragObj[CurDragObj].ObjPoint[0].y,
                  DragObj[CurDragObj].ObjPoint[1].x,
                  DragObj[CurDragObj].ObjPoint[1].y,
                  hFocusPen,&TerWinRect,TRUE);

    DragObj[CurDragObj].drawn=!DragObj[CurDragObj].drawn;

    return TRUE;
}

/*****************************************************************************
    DrawDragHotSpots:
    This function draws the hot spots for the given type of object
******************************************************************************/
BOOL DrawDragHotSpots(PTERWND w,int type)
{
   int i,j;

   TerSetClipRgn(w);            // draw only within the text area

   for (i=0;i<TotalDragObjs;i++) {
      if (!DragObj[i].InUse) continue;
      if (DragObj[i].type!=type) continue;

      for (j=0;j<DragObj[i].HotRectCount;j++) {

         BitBlt(hTerDC,DragObj[i].HotRect[j].left,DragObj[i].HotRect[j].top,
                        DragObj[i].HotRect[j].right-DragObj[i].HotRect[j].left,
                        DragObj[i].HotRect[j].bottom-DragObj[i].HotRect[j].top,
                        NULL,0,0,DSTINVERT);
      }
   }

   TerResetClipRgn(w);          // reset clip regin

   return TRUE;
}

/*****************************************************************************
    GetDragObjectSlot:
    Get an empty slot to create a drag object.  This function returns
    the index of the empty slot.  On failure it returns -1.
******************************************************************************/
int GetDragObjectSlot(PTERWND w)
{
    int i;
    long OldSize,NewSize;
    LPBYTE pMem;
    HGLOBAL hMem;

    for (i=0;i<TotalDragObjs;i++) if (!DragObj[i].InUse) goto SLOT_FOUND;

    // allocate spaces for more drag objects if needed
    if (TotalDragObjs>=MaxDragObjs) {
       OldSize=(MaxDragObjs+1)*sizeof(struct StrDragObj);
       if (MaxDragObjs<100) MaxDragObjs+=20;    // increase the number of drag object slots
       else                 MaxDragObjs+=50;
       if (!Win32 && MaxDragObjs>MAX_DRAG_OBJS) MaxDragObjs=MAX_DRAG_OBJS;
       NewSize=(MaxDragObjs+1)*sizeof(struct StrDragObj);

       if (   NewSize>OldSize &&
             (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,NewSize)))
          && (NULL!=(pMem=GlobalLock(hMem))) ){

          // move data to new location
          HugeMove(DragObj,pMem,OldSize);

          // free old handle
          GlobalUnlock(hDragObj);
          GlobalFree(hDragObj);

          // assign new handle
          hDragObj=hMem;              // new array
          DragObj=(struct StrDragObj far *)pMem;
       }
       else return FALSE;
    }


    if (TotalDragObjs<MaxDragObjs) {
       i=TotalDragObjs;
       TotalDragObjs++;
    }
    else {
       if (!(MessageDisplayed&MSG_MAX_DRAG)) {
          PrintError(w,MSG_OUT_OF_DRAG_OBJ,NULL);
          MessageDisplayed|=MSG_MAX_DRAG;
       }
       return -1;
    }

    SLOT_FOUND:
    FarMemSet(&DragObj[i],0,sizeof(struct StrDragObj));
    return i;
}

/*****************************************************************************
    DeleteDragObjects:
    Delete the drag objects in the specified types
******************************************************************************/
BOOL DeleteDragObjects(PTERWND w,int FirstType, int LastType)
{
    int i;

    for (i=0;i<=TotalDragObjs;i++) {
       if (DragObj[i].type>=FirstType && DragObj[i].type<=LastType) DragObj[i].InUse=FALSE;
    }

    return TRUE;
}

/******************************************************************************
    TerDrawLine:
    Draw a polygon of the given dimension using the selected pen and brush
******************************************************************************/
int TerDrawLine(PTERWND w,int x1, int y1, int x2, int y2,HPEN hPen, RECT far *ClpRect,BOOL drag)
{
    HPEN hOldPen;
    int  SaveRop;
   
    dm("TerDrawLine");

    // set DC
    hOldPen=SelectObject(hTerDC,hPen);
    if (ClpRect) {                   // set the clipping rectangle
       HRGN rgn=CreateRectRgn(ClpRect->left,ClpRect->top,ClpRect->right,ClpRect->bottom);
       SelectObject(hTerDC,rgn);
       DeleteObject(rgn);
    }

    if (drag) {
       SaveRop=SetROP2(hTerDC,R2_NOTXORPEN);        // Use XOR drawing mode
       SetColor(w,hTerDC,'T',DEFAULT_CFMT);      // set the default background color
    }

    MoveToEx(hTerDC,x1,y1,NULL);
    LineTo(hTerDC,x2,y2);

    // reset DC
    SelectObject(hTerDC,hOldPen);
    if (ClpRect) TerSetClipRgn(w);
    if (drag) SetROP2(hTerDC,SaveRop);           // reset the drawing mode

    return TRUE;
}

/******************************************************************************
    TerDrawPolygon:
    Draw a polygon of the given dimension using the selected pen and brush
******************************************************************************/
int TerDrawPolygon(PTERWND w,POINT far *pt,int PointCount,HPEN hPen,HBRUSH hBrush, RECT far *ClpRect, BOOL drag)
{
    HPEN hOldPen;
    HBRUSH hOldBrush;
    int  SaveRop;

    dm("TerDrawPolygon");

    // set DC
    hOldPen=SelectObject(hTerDC,hPen);
    hOldBrush=SelectObject(hTerDC,hBrush);
    if (ClpRect) {                   // set the clipping rectangle
       HRGN rgn=CreateRectRgn(ClpRect->left,ClpRect->top,ClpRect->right,ClpRect->bottom);
       SelectObject(hTerDC,rgn);
       DeleteObject(rgn);
    }

    if (drag) {
       SaveRop=SetROP2(hTerDC,R2_NOTXORPEN);        // Use XOR drawing mode
       SetColor(w,hTerDC,'T',DEFAULT_CFMT);      // set the default background color
    }

    Polygon(hTerDC,pt,PointCount);

    // reset DC
    SelectObject(hTerDC,hOldPen);
    SelectObject(hTerDC,hOldBrush);
    if (ClpRect) TerSetClipRgn(w);
    if (drag) SetROP2(hTerDC,SaveRop);           // reset the drawing mode

    return TRUE;
}

/******************************************************************************
    TerDrawRect:
    Draw a rectangle of the given dimension using the selected pen.
******************************************************************************/
int TerDrawRect(PTERWND w,RECT far *rect,HPEN hPen,BOOL clip,BOOL drag)
{
    POINT pt[5];
    HPEN hOldPen;
    int  SaveRop;

    dm("TerDrawRect");

    // build points for the rectangle
    pt[0].x=rect->left;
    pt[0].y=rect->top;
    pt[1].x=rect->right;
    pt[1].y=rect->top;
    pt[2].x=rect->right;
    pt[2].y=rect->bottom;
    pt[3].x=rect->left;
    pt[3].y=rect->bottom;
    pt[4].x=rect->left;
    pt[4].y=rect->top;

    if (clip) TerSetClipRgn(w);                   // draw only in the text area
    hOldPen=SelectObject(hTerDC,hPen);

    if (drag) {
       SaveRop=SetROP2(hTerDC,R2_NOTXORPEN);        // Use XOR drawing mode
       SetColor(w,hTerDC,'T',DEFAULT_CFMT);      // set the default background color
    }

    Polyline(hTerDC,pt,5);

    SelectObject(hTerDC,hOldPen);
    if (clip) TerSetClipRgn(w);
    if (drag) SetROP2(hTerDC,SaveRop);           // reset the drawing mode

    return TRUE;
}

