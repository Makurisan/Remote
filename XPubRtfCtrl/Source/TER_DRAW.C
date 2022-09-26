/*==============================================================================
   TER_DRAW Editor Developer's Kit
   Window misc Screen drawing routines.


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

#pragma optimize("g",off)  // turn-off global optimization

/******************************************************************************
   OurStretchDIBits:
   The StretchDIBits function that supports text rotation
*******************************************************************************/
int OurStretchDIBits(PTERWND w, HDC hDC, int DestX, int DestY, int DestWidth, int DestHeight, int SrcX, int SrcY, int SrcWidth, int SrcHeight, LPVOID pBits, LPBITMAPINFO pInfo, UINT usage, DWORD rop)
{
   RECT rect;
   int x,y,x1,x2;

   // rtl translation
   if (ScrRtl && pScrSeg && CurScrSeg<TotalScrSeg) {
      x1=RtlX(w,DestX,0,CurFrame,&(pScrSeg[CurScrSeg]));
      x2=RtlX(w,DestX+DestWidth,0,CurFrame,&(pScrSeg[CurScrSeg]));
      if (x2<x1) SwapInts(&x1,&x2);
      DestX=x1;
      DestWidth=x2-x1;
   }

   if (ScrFrameAngle==0) 
      return StretchDIBits(hDC,DestX,DestY,DestWidth,DestHeight,SrcX,SrcY,SrcWidth,SrcHeight,pBits,pInfo,usage,rop);
 
   SetRect(&rect,DestX,DestY,DestX+DestWidth,DestY+DestHeight);
   FrameRotateRect(w,&rect,CurFrame);
   x=(rect.left<rect.right?rect.left:rect.right);
   y=(rect.top<rect.bottom?rect.top:rect.bottom);

   return StretchDIBits(hDC,x,y,DestHeight,DestWidth,SrcX,SrcY,SrcWidth,SrcHeight,pBits,pInfo,usage,rop);
} 

/******************************************************************************
   OurBitBlt:
   The BitBlt function that supports text rotation
*******************************************************************************/
int OurBitBlt(PTERWND w, HDC hDC, int DestX, int DestY, int DestWidth, int DestHeight, HDC hSrcDC, int SrcX, int SrcY, DWORD rop)
{
   RECT rect;
   int x,y,x1,x2;

   // rtl translation
   if (ScrRtl && pScrSeg && CurScrSeg<TotalScrSeg) {
      x1=RtlX(w,DestX,0,CurFrame,&(pScrSeg[CurScrSeg]));
      x2=RtlX(w,DestX+DestWidth,0,CurFrame,&(pScrSeg[CurScrSeg]));
      if (x2<x1) SwapInts(&x1,&x2);
      DestX=x1;
      DestWidth=x2-x1;
   }

   if (ScrFrameAngle==0) 
      return BitBlt(hDC,DestX,DestY,DestWidth,DestHeight,hSrcDC,SrcX,SrcY,rop);
 
   SetRect(&rect,DestX,DestY,DestX+DestWidth,DestY+DestHeight);
   FrameRotateRect(w,&rect,CurFrame);
   x=(rect.left<rect.right?rect.left:rect.right);
   y=(rect.top<rect.bottom?rect.top:rect.bottom);

   return BitBlt(hDC,x,y,DestHeight,DestWidth,hSrcDC,SrcX,SrcY,rop);
} 

/******************************************************************************
   OurExtTextOut:
   The ExtTextOut function that supports text rotation
*******************************************************************************/
int  OurExtTextOut(PTERWND w,HDC hDC, int x, int y, UINT options, LPRECT rect, LPBYTE string, UINT count, LPINT pDX)
{
   int NewX,NewY,result;
   RECT NewRect;
   LPRECT pNewRect=&NewRect;
   BOOL CheckRtl=(ScrRtl && pScrSeg && CurScrSeg<TotalScrSeg);


   if (rect)  NewRect=(*rect);
   else       pNewRect=NULL;

   // rtl translation
   if (CheckRtl && rect) {
      x=RtlX(w,x,NewRect.right-NewRect.left,CurFrame,&(pScrSeg[CurScrSeg]));
      RtlRect(w,&NewRect,CurFrame,&(pScrSeg[CurScrSeg]));
   }

   // text drawing 
   if (OSCanRotate || ScrFrameAngle==0) {
      //BOOL rtl=CheckRtl?pScrSeg[CurScrSeg].rtl:TerFont[CurFmtId].rtl;   // :FALSE;
      BOOL rtl=CheckRtl?pScrSeg[CurScrSeg].rtl:FALSE;   // :FALSE;

      if (rtl /*&& !IsWinNT2000*/) {
         //GCP_RESULTS gcp;
         //WORD glyphs[MAX_WIDTH];
         int i,extra,adj,width=0;
         int dx[MAX_WIDTH+1];

         if (count>0) {
            for (i=0;i<(int)count;i++) {
               dx[i]=pDX[count-i-1];
               width+=dx[i];
            }

            if (FALSE && IsWinNT2000) {     // check for text overshoot
               SIZE size;
               GetTextExtentPoint(hDC,string,count,&size);
               if (size.cx>width) {
                 extra=(size.cx-width)+1;
                 adj=extra/count;
                 if (adj>0) for (i=0;i<(int)count;i++) dx[i]-=adj;  // take-off pixels uniformly
                 extra-=(adj*count);
                 for (i=0;i<extra;i++) dx[i]--;    // adjust for remaining pixels
               } 
               return ExtTextOut(hDC,x,y,options|ETO_RTLREADING,pNewRect,string,count,NULL);
            }
            else {
               SIZE size;
               int tol=2;
               GetTextExtentPoint(hDC,string,count,&size);
               
               if (InPrinting) tol=ScrToPrtX(tol);  // convert to the printer units

               adj=0;
               if (size.cx>(width+tol))      adj=-((((int)size.cx-width)/(int)count)+1);
               else if (size.cx<(width-tol)) adj=(width-size.cx)/count;
               
               if (adj==0) return ExtTextOut(hDC,x,y,options|ETO_RTLREADING,pNewRect,string,count,NULL);
               else if (TerFont[CurFmtId].expand || !pNewRect || InPrinting) {
                  SetTextCharacterExtra(hDC,adj);
                  result=ExtTextOut(hDC,x,y,options|ETO_RTLREADING,pNewRect,string,count,NULL);
                  SetTextCharacterExtra(hDC,0);
                  return result;
               }
               else {         // do text output in full size then stretch blast it - ExtTextOut does not adjust character widths properly
                  RECT TempRect;
                  HBITMAP hMemBM,hOldBM;
                  HFONT hOldFont;
                  int OldBkMode,adj=0;
                  COLORREF OldTextColor,OldBkColor;

                  SetRect(&TempRect,0,0,size.cx+1,pNewRect->bottom-pNewRect->top); // real text size
                  
                  if (pNewRect->right>(TerWinOrgX+TerWinWidth)) {   // to avoid clipping related issues
                     int adj=pNewRect->right-(TerWinOrgX+TerWinWidth);
                     pNewRect->right-=adj;
                     TempRect.right-=adj;
                  }

                  // make temporary bitmap
                  if (NULL==(hMemBM=CreateCompatibleBitmap(hTerDC,TempRect.right+1,TempRect.bottom+1))) return FALSE;
                  hOldBM=SelectObject(hMemDC,hMemBM);
                  hOldFont=SelectObject(hMemDC,TerFont[CurFmtId].hFont);
                  OldTextColor=SetTextColor(hMemDC,CurForeColor);
                  OldBkColor=SetBkColor(hMemDC,CurBackColor);
                  OldBkMode=SetBkMode(hMemDC,(transparent || InPrinting)?TRANSPARENT:OPAQUE);  // set transparency
                  
                  // copy the background to the memory device context
                  StretchBlt(hMemDC,0,0,TempRect.right,TempRect.bottom,
                      hDC,pNewRect->left,pNewRect->top,pNewRect->right-pNewRect->left,pNewRect->bottom-pNewRect->top,SRCCOPY);
                  result=ExtTextOut(hMemDC,0,y-pNewRect->top,options|ETO_RTLREADING,&TempRect,string,count,NULL);
               
                  // copy text to the original device context
                  StretchBlt(hDC,pNewRect->left,pNewRect->top,pNewRect->right-pNewRect->left,pNewRect->bottom-pNewRect->top,
                      hMemDC,0,0,TempRect.right,TempRect.bottom,SRCCOPY);
                   
                  // delete the temporary bitmap
                  SelectObject(hMemDC,hOldBM);
                  DeleteObject(hMemBM);

                  SelectObject(hMemDC,hOldFont);
                  SetTextColor(hMemDC,OldTextColor);
                  SetBkColor(hMemDC,OldBkColor);
                  SetBkMode(hMemDC,OldBkMode);

                  return result;
               }
            } 
         }
         
         return ExtTextOut(hDC,x,y,options|ETO_RTLREADING,pNewRect,string,count,NULL);

         //FarMemSet(&gcp,0,sizeof(gcp));
         //gcp.lStructSize=sizeof(gcp);
         //gcp.lpDx=pDX;
         //gcp.lpGlyphs=(LPWSTR)glyphs;
         //gcp.nGlyphs=MAX_WIDTH;

         //GetCharacterPlacement(hDC,string,count,0,&gcp,GCP_GLYPHSHAPE|GCP_REORDER);
         //if (count==0) gcp.nGlyphs=0;

         //width=0;
         //for (i=0;i<(int)count;i++) width+=pDX[i];

         //if (rect->right==103) OurPrintf("pf: %d, nrl: %d, nrr: %d, x: %d, c: %d, opt: %x, w: %d, hf: %x",
         //PaintFlag,pNewRect->left,pNewRect->right,x,count,options,width,TerFont[CurFmtId].hFont);

         //return ExtTextOut(hDC,x,y,options|ETO_GLYPH_INDEX,pNewRect,(LPBYTE)glyphs,gcp.nGlyphs,pDX);
      }
      else {
         if (TerFont[CurFmtId].rtl) options|=ETO_RTLREADING; // to print arabic numbers properly, arabic numbers are printed ltr
         return ExtTextOut(hDC,x,y,options,pNewRect,string,count,pDX);
      }
   }
    
   NewX=FrameRotateX(w,x,y,CurFrame);
   NewY=FrameRotateY(w,x,y,CurFrame);

   if (rect) FrameRotateRect(w,&NewRect,CurFrame);
   
   if (TerFont[CurFmtId].rtl) options|=ETO_RTLREADING; // to print arabic numbers properly, arabic numbers are printed ltr

   return ExtTextOut(hDC,NewX,NewY,options,pNewRect,string,count,pDX);
}

/******************************************************************************
   OurExtTextOutW:
   The ExtTextOutW function that supports text rotation
*******************************************************************************/
int  OurExtTextOutW(PTERWND w,HDC hDC, int x, int y, UINT options, LPRECT rect, LPWORD string, UINT count, LPINT pDX)
{
   int NewX,NewY;
   RECT NewRect;
   LPRECT pNewRect=&NewRect;

   if (rect)  NewRect=(*rect);
   else       pNewRect=NULL;

   // rtl translation
   if (ScrRtl && rect && pScrSeg && CurScrSeg<TotalScrSeg) {
      x=RtlX(w,x,NewRect.right-NewRect.left,CurFrame,&(pScrSeg[CurScrSeg]));
      RtlRect(w,&NewRect,CurFrame,&(pScrSeg[CurScrSeg]));
   }


   if (OSCanRotate || ScrFrameAngle==0) return ExtTextOutW(hDC,x,y,options,pNewRect,string,count,pDX);
    
   NewX=FrameRotateX(w,x,y,CurFrame);
   NewY=FrameRotateY(w,x,y,CurFrame);

   if (rect) {
      NewRect=(*rect);
      FrameRotateRect(w,&NewRect,CurFrame);
   }
   else pNewRect=0;
   
   return ExtTextOutW(hDC,NewX,NewY,options,pNewRect,string,count,pDX);
}

/******************************************************************************
   OurMoveToEx:
   The MoveToEx function that supports text rotation
*******************************************************************************/
BOOL  OurMoveToEx(PTERWND w,HDC hDC, int x, int y, LPPOINT pPt)
{
   int NewX,NewY;

   if ((OSCanRotate || ScrFrameAngle==0) && !ScrRtl) return MoveToEx(hDC,x,y,pPt);

   if (ScrRtl) x=RtlX(w,x,0,CurFrame,pScrSeg?(&(pScrSeg[CurScrSeg])):NULL);
    
   if (OSCanRotate) {
      NewX=x;
      NewY=y;
   }
   else {
      NewX=FrameRotateX(w,x,y,CurFrame);
      NewY=FrameRotateY(w,x,y,CurFrame);
   }

   return MoveToEx(hDC,NewX,NewY,pPt);
}

/******************************************************************************
   OurLineTo:
   The LineTo function that supports text rotation
*******************************************************************************/
BOOL  OurLineTo(PTERWND w,HDC hDC, int x, int y)
{
   int NewX,NewY;

   if ((OSCanRotate || ScrFrameAngle==0) && !ScrRtl) return LineTo(hDC,x,y);

   if (ScrRtl) x=RtlX(w,x,0,CurFrame,pScrSeg?(&(pScrSeg[CurScrSeg])):NULL);

   if (OSCanRotate) {
      NewX=x;
      NewY=y;
   }
   else {
      NewX=FrameRotateX(w,x,y,CurFrame);
      NewY=FrameRotateY(w,x,y,CurFrame);
   }


   return LineTo(hDC,NewX,NewY);
}

/******************************************************************************
   FrameRotateDC:
   Rotate DC for the frame. Works only with WinNT compatible OS
*******************************************************************************/
int  FrameRotateDC(PTERWND w,HDC hDC, int FrameNo)
{
     XFORM form;
     int BoxFrame=frame[FrameNo].BoxFrame;
     int CurFID=frame[FrameNo].ParaFrameId;
     int BoxX,BoxY,BoxHeight,BoxWidth;
     int CellId=frame[FrameNo].CellId;

     if (  ( CurFID==0 || ParaFrame[CurFID].TextAngle==0) 
        && (CellId==0 || cell[CellId].TextAngle==0) )  return TRUE;

     BoxX=frame[BoxFrame].x+frame[BoxFrame].SpaceLeft+(InPrinting?PrtLeftMarg:0);
     BoxY=frame[BoxFrame].y+frame[BoxFrame].SpaceTop+(InPrinting?PrtTopMarg:0);
     BoxHeight=frame[BoxFrame].height-frame[BoxFrame].SpaceTop-frame[BoxFrame].SpaceBot;
     BoxWidth=frame[BoxFrame].width-frame[BoxFrame].SpaceLeft-frame[BoxFrame].SpaceRight;
     
     form.eM11=0;
     form.eM22=0;

     if (ScrFrameAngle==90) {
        form.eM21=1;
        form.eDx=(float)(BoxX-BoxY);
        form.eM12=-1;
        form.eDy=(float)(BoxX+BoxY+BoxHeight);
     }
     else {
        form.eM21=-1;
        form.eDx=(float)(BoxX+BoxY+BoxWidth);
        form.eM12=1;
        form.eDy=(float)(BoxY-BoxX);
     } 

     SetGraphicsMode(hDC,GM_ADVANCED);
     SetWorldTransform(hDC,&form);

     return TRUE;
}

/******************************************************************************
   FrameNoRotateDC:
   Reset DC rotation. Works only on WinNT compatible OS
*******************************************************************************/
FrameNoRotateDC(PTERWND w,HDC hDC)
{
   ModifyWorldTransform(hDC,NULL,MWT_IDENTITY);
   SetGraphicsMode(hDC,GM_COMPATIBLE);
   
   return TRUE;
}
 
/******************************************************************************
   FrameRotateX:
   Rotate X for the frame
*******************************************************************************/
int  FrameRotateX(PTERWND w,int x,int y,int FrameNo)
{
   int NewX;
   int BoxFrame=frame[FrameNo].BoxFrame;
   int CurFID=frame[FrameNo].ParaFrameId;
   int BoxX,BoxY,BoxWidth;
   int TextAngle=0;

   if (CurFID==0) BoxFrame=FrameNo;
   TextAngle=GetFrameTextAngle(w,FrameNo);
   if (TextAngle==0) return x;  // no rotation

   if (CurFID==0) {
     BoxX=frame[BoxFrame].x+frame[BoxFrame].SpaceLeft+(InPrinting?PrtLeftMarg:0);
     BoxY=frame[BoxFrame].y+frame[BoxFrame].SpaceTop+(InPrinting?PrtTopMarg:0);
     BoxWidth=frame[BoxFrame].width-frame[BoxFrame].SpaceLeft-frame[BoxFrame].SpaceRight;
   }
   else {
     BoxX=frame[BoxFrame].x+frame[BoxFrame].SpaceLeft+(InPrinting?PrtLeftMarg:0);
     BoxY=frame[BoxFrame].y+frame[BoxFrame].SpaceTop+(InPrinting?PrtTopMarg:0);
     BoxWidth=frame[BoxFrame].width-frame[BoxFrame].SpaceLeft-frame[BoxFrame].SpaceRight;
   } 

   if (TextAngle==90) 
         NewX=BoxX+y-BoxY;
   else  NewX=BoxX+BoxY+BoxWidth-y;

   return NewX;
}

/******************************************************************************
   FrameRotateY:
   Rotate X for the frame
*******************************************************************************/
int  FrameRotateY(PTERWND w,int x,int y,int FrameNo)
{
   int NewY;
   int BoxFrame=frame[FrameNo].BoxFrame;
   int CurFID=frame[FrameNo].ParaFrameId;
   int BoxX,BoxY,BoxHeight;
   int TextAngle=0;

   if (CurFID==0) BoxFrame=FrameNo;
   TextAngle=GetFrameTextAngle(w,FrameNo);
   if (TextAngle==0) return y;  // no rotation

   if (CurFID==0) {
      BoxX=frame[BoxFrame].x+frame[BoxFrame].SpaceLeft+(InPrinting?PrtLeftMarg:0);
      BoxY=frame[BoxFrame].y+frame[BoxFrame].SpaceTop+(InPrinting?PrtTopMarg:0);
      BoxHeight=frame[BoxFrame].height-frame[BoxFrame].SpaceTop-frame[BoxFrame].SpaceBot;
   }
   else {
      BoxX=frame[BoxFrame].x+frame[BoxFrame].SpaceLeft+(InPrinting?PrtLeftMarg:0);
      BoxY=frame[BoxFrame].y+frame[BoxFrame].SpaceTop+(InPrinting?PrtTopMarg:0);
      BoxHeight=frame[BoxFrame].height-frame[BoxFrame].SpaceTop-frame[BoxFrame].SpaceBot;
   } 

   if (TextAngle==90) 
         NewY=BoxX+BoxY+BoxHeight-x;
   else  NewY=-BoxX+x+BoxY;

   return NewY;
}

/******************************************************************************
   FrameRotateRect:
   Rotate rectangle for the frame
*******************************************************************************/
FrameRotateRect(PTERWND w,LPRECT rect,int FrameNo)
{
    int x,y,width,height;
    int CurFID=frame[FrameNo].ParaFrameId;
    int TextAngle;

    // rotate the top-left, it comes bottom-left
    x=FrameRotateX(w,rect->left,rect->top,FrameNo);
    y=FrameRotateY(w,rect->left,rect->top,FrameNo);

    height=rect->right-rect->left;    // width becomes height of the rotated rectangle
    width=rect->bottom-rect->top;

    rect->left=x;
    rect->bottom=y;

    TextAngle=GetFrameTextAngle(w,FrameNo);

    if (TextAngle==90) {
       rect->right=rect->left+width;
       rect->top=rect->bottom-height;
    }
    else {
       rect->right=rect->left-width;
       rect->top=rect->bottom+height;
    }
    
    return TRUE;
} 

/******************************************************************************
   RtlX:
   Translate X for rtl text flow
*******************************************************************************/
int  RtlX(PTERWND w,int x,int TextRectWidth, int FrameNo, struct StrLineSeg far *pSeg)
{
   int NewX;
   int BoxX,BoxWidth,FrameWidth,FrameX,SegX,offset;
   int BoxFrame,CurFID;

   if (FrameNo<0 || FrameNo>=TotalFrames) return x;

   BoxFrame=frame[FrameNo].BoxFrame;
   CurFID=frame[FrameNo].ParaFrameId;

   FrameX=frame[FrameNo].x+(InPrinting?PrtLeftMarg:0);
   BoxX=FrameX+frame[FrameNo].SpaceLeft;
   
   FrameWidth=frame[FrameNo].width;
   if (FrameWidth==9999) FrameWidth=TerWinWidth;  // GetWinDim sets the FrameWidth to 9999 initially
   BoxWidth=FrameWidth-frame[FrameNo].SpaceLeft-frame[FrameNo].SpaceRight;

   if (TerArg.FittedView && BoxWidth>=TerWinWidth) BoxWidth=TerWinWidth-1;

   if (ParaFrame[CurFID].TextAngle!=0) BoxWidth=frame[BoxFrame].height-frame[BoxFrame].SpaceTop-frame[BoxFrame].SpaceBot;

   if (!pSeg || pSeg->rtl) NewX=BoxX+(BoxWidth-(x-BoxX))-TextRectWidth;
   else {         // ltr text in rtl line
     SegX=pSeg->x+FrameX;    // segment x relative to the left edge of the frame system
     
     offset=x-SegX;   // character position relative to tbe beginning of the segment
     
     NewX=BoxX+(BoxWidth-(SegX-BoxX))-pSeg->width;   // x for the whole ltr segment
     NewX+=offset;                              // x for this character
   } 


   return NewX;
}

/******************************************************************************
   RtlRect:
   Translate rectangle for rtl text flow
*******************************************************************************/
int RtlRect(PTERWND w,LPRECT pRect,int FrameNo, struct StrLineSeg far *pSeg)
{
   int BoxX,BoxWidth;
   int left,right;
   int TextAngle=0;

   TextAngle=GetFrameTextAngle(w,FrameNo);

   BoxX=frame[FrameNo].x+frame[FrameNo].SpaceLeft+(InPrinting?PrtLeftMarg:0);
   BoxWidth=frame[FrameNo].width-frame[FrameNo].SpaceLeft-frame[FrameNo].SpaceRight;
   if (TextAngle!=0) BoxWidth=frame[FrameNo].height-frame[FrameNo].SpaceTop-frame[FrameNo].SpaceBot;

   left=RtlX(w,pRect->left,0,FrameNo,pSeg);
   right=RtlX(w,pRect->right,0,FrameNo,pSeg);
   if (right<left) SwapInts(&left,&right);
   pRect->left=left;
   pRect->right=right;

   return TRUE;
}


/******************************************************************************
   NormalizeRect:
   Normalize rect so that rect.left is smaller than rect.right and rect.top is small than rect.bottom
*******************************************************************************/
NormalizeRect(PTERWND w,LPRECT rect)
{
   if (rect->right<rect->left) SwapInts(&rect->left,&rect->right);
   if (rect->bottom<rect->top) SwapInts(&rect->bottom,&rect->top);

   return TRUE;
}
 
/******************************************************************************
   InRotateFrame:
   This function returns TRUE if the given point lines in the rotate frame.
   The frames (except boxed frame) generated for paragraph frames are not 
   rotated at the creation time. So to find if a point exists within a frame, 
   we need to find the frames rotated co-ordinates.
*******************************************************************************/
BOOL InRotatedFrame(PTERWND w,int x, int y, int FrameNo)
{
    int FrameX,FrameY,FrameWidth,FrameHt,ParaFID,tol;
    RECT rect;
    BOOL ParaFrameRotated=false;

    FrameX=frame[FrameNo].x;
    FrameY=frame[FrameNo].y;
    FrameWidth=frame[FrameNo].width;
    FrameHt=frame[FrameNo].height;
    ParaFID=frame[FrameNo].ParaFrameId;

    if (ParaFID>0 && ParaFrame[ParaFID].TextAngle!=0) ParaFrameRotated=true;  // CreateFrames swaps the frame width/height for vertical ParaFrames, but it does not do this swapping for cell with vertical text. 

    if (ParaFrameRotated && GetFrameTextAngle(w,FrameNo)>0 && !(frame[FrameNo].flags1&FRAME1_PARA_FRAME_BOX)) {   // rotate the frame
       SetRect(&rect,FrameX,FrameY,FrameX+FrameWidth,FrameY+FrameHt);
       FrameRotateRect(w,&rect,FrameNo);
       NormalizeRect(w,&rect);  // ensure rect.left<=rect.right and rect.top<=rect.bottom
       FrameX=rect.left;
       FrameY=rect.top;
       FrameWidth=rect.right-rect.left;
       FrameHt=rect.bottom-rect.top;
    } 

    if (ParaFrame[ParaFID].flags&PARA_FRAME_LINE && !(TerOpFlags&TOFLAG_NO_TOL))
         tol=TwipsToScrY(RULER_TOLERANCE);   // tolerance for line objects
    else tol=0;


    return (x>=(FrameX-tol) && x<(FrameX+FrameWidth+tol)
           &&  y>=(FrameY-tol) && y<(FrameY+FrameHt+tol));
}

/******************************************************************************
   GetSectDisplayLine:
   Get the display line (0 based) for the first line of the frame.
*******************************************************************************/
long GetSectDisplayLine(PTERWND w,int FrameNo)
{
   long l,FrameFirstLine,PageFirstLine,DispLine=-1;
   int page,sect,NewSect,TopSect;

   if (frame[FrameNo].empty) return -1;
   if (True(frame[FrameNo].flags&FRAME_DISABLED)) return -1;
   if (frame[FrameNo].ParaFrameId>0) return -1;
   if (frame[FrameNo].CellId>0) return -1;

   FrameFirstLine=frame[FrameNo].PageFirstLine;
   if (True(PfmtId[pfmt(FrameFirstLine)].flags&PAGE_HDR_FTR)) return -1;  // execlude header/footer

   sect=frame[FrameNo].sect;
   if (False(TerSect[sect].flags&SECT_LINE)) return -1;   // line numbering not enabled

   page=LinePage(FrameFirstLine);
   PageFirstLine=PageInfo[page].FirstLine;
   TopSect=GetSection(w,PageFirstLine);

   for (l=PageFirstLine;l<=PageInfo[page].LastLine;l++) {
      if (LinePage(l)!=page) continue;
      if (cid(l)>0) continue;
      if (fid(l)>0) continue;
      if (True(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)) continue;

      if (l>FrameFirstLine) break;   // use > condition so that the DispLine gets incrented for the first line

      if (TopSect!=sect) {     // different sections encountered 
         NewSect=GetSection(w,l);
         if (False(TerSect[NewSect].flags&SECT_LINE)) {
            DispLine=-1;  // reset
            continue;
         }
      } 
      DispLine++;
   } 

   return DispLine;
}

/******************************************************************************
   GetFrameTextAngle:
   Get the text angle used by the current frame
*******************************************************************************/
int GetFrameTextAngle(PTERWND w,int FrameNo)
{
   int ParaFID,CellId,TextAngle=0;

   ParaFID=frame[FrameNo].ParaFrameId;
   CellId=frame[FrameNo].CellId;

   if (ParaFID>0)     TextAngle=ParaFrame[ParaFID].TextAngle;
   if (TextAngle==0 && CellId>0) TextAngle=cell[CellId].TextAngle;

   return TextAngle;
} 

/******************************************************************************
    HorScreenCheck:
    Check if horizontal scrolling is required.
*******************************************************************************/
void HorScrollCheck(PTERWND w)
{
    int NewScreenNo,x,MaxTextX,y=0,TempOrgX;
    int ApproxFirstCharWidth=15;
    int PrevWinOrgX=TerWinOrgX;

    if (TerArg.WordWrap && TerWinOrgX==0 && !HScrollAllowed(w)) {         // avoid scrolling if possible
       if (TerArg.PageMode) MaxTextX=PageTextWidth(w); // calculate maximum text x
       else                 MaxTextX=TerWrapWidth(w,CurLine,-1);
       if (MaxTextX<=TerWinWidth) return;
    }

    if (!UseCaret(w) || !CaretEngaged) return;  // caret hidden or in read only mode
    if (TerFlags3&TFLAG3_NO_SCROLL && TerWinOrgX==0) return;   // scrolling disabled
    //if (!CaretEnabled || !CaretEngaged) return;  // caret hidden or in read only mode


    if (LineFlags2(CurLine)&LFLAG2_RTL) GetCaretXY(w,CurLine,CurCol,y,&x,&y);
    else                                x=ColToUnits(w,CurCol,CurLine,LEFT);

    if (x>=TerWinOrgX && x<=(TerWinOrgX+TerWinWidth)) return;

    if (TerWinWidth<=0) NewScreenNo=1;
    else                NewScreenNo=((x*2)/TerWinWidth);
    if (NewScreenNo==0) NewScreenNo=1;
    TerWinOrgX=(NewScreenNo-1)*(TerWinWidth/2);
    
    if (PrevWinOrgX==0 && LineFlags2(CurLine)&LFLAG2_RTL 
       && (x>(TerWinWidth-ApproxFirstCharWidth) || CurCol<=1)
       && TerArg.WordWrap && TerArg.PageMode) {  // when entering first rtl character in a left aligned line
       MaxTextX=PageTextWidth(w); // calculate maximum text x
       TempOrgX=MaxTextX-TerWinWidth;
       if (TempOrgX<0) TempOrgX=0;
       if (x>=TempOrgX && x<=(TempOrgX+TerWinWidth)) TerWinOrgX=TempOrgX;
       TerWinOrgX++;
    } 

    SetTerWindowOrg(w);      // set the window origin
    DeleteTextMap(w,TRUE);   // redraw any background picture
}

/******************************************************************************
    HScrollAllowed:
    Returns TRUE if horizontal scrolling is allowed in the simple word-wrap mode.
*******************************************************************************/
BOOL HScrollAllowed(PTERWND w)
{
    int i;

    if (TerFlags&TFLAG_NO_WRAP) return TRUE;
 
    for (i=0;i<TotalPfmts;i++) {
      if (PfmtId[i].pflags&PFLAG_NO_WRAP) return TRUE;
    } 

    return FALSE;
} 

/******************************************************************************
    PageColor:
    Get the page text background coolor.
*******************************************************************************/
COLORREF PageColor(PTERWND w)
{
    if (PageBkColor!=0xFFFFFF) return PageBkColor;
    else                       return TextDefBkColor;
} 

/******************************************************************************
    DrawWheel:
    Draw the mouse wheel.
*******************************************************************************/
DrawWheel(PTERWND w, HDC hDC)
{
    int x,y,radius=WHEEL_RADIUS;
    COLORREF PenColor,BrushColor;
    HBRUSH hBr,hOldBr;
    HPEN hPen,hOldPen;
    POINT pt[3];

    // calculate the center x,y
    x=WheelX-TerWinRect.left+TerWinOrgX;
    y=WheelY-TerWinRect.top+TerWinOrgY;
    
    // create pen/brush
    PenColor=0x7f7f7f;   // gray color for the circle
    if (PenColor==PageColor(w)) PenColor=0xFFFFFF;  // use white when the background is gray
    BrushColor=PageColor(w);
    
    hPen=CreatePen(PS_SOLID,0,PenColor);
    hBr=CreateSolidBrush(BrushColor);
    

    // draw the circle
    hOldPen=SelectObject(hDC,hPen);
    hOldBr=SelectObject(hDC,hBr);

    Ellipse(hDC,x-radius,y-radius,x+radius,y+radius);

    // select null pen and gray brush
    SelectObject(hDC,GetStockObject(NULL_PEN));  // select null pen and delete the new pen
    DeleteObject(hPen);

    SelectObject(hDC,hOldBr);        // delete the white brush
    DeleteObject(hBr);

    hBr=CreateSolidBrush(PenColor);  // create and select gray brush
    hOldBr=SelectObject(hDC,hBr);

    Ellipse(hDC,x-2,y-2,x+3,y+3);    // draw a dot at the center

    // draw top triangle
    pt[0].x=x;            // top point
    pt[0].y=y-(radius-2);
    pt[1].x=x-(radius-6);  // left point
    pt[1].y=y-4;
    pt[2].x=x+(radius-6);  // left point
    pt[2].y=y-4;

    Polygon(hDC,pt,3);

    // draw bottom triangle
    pt[0].x=x;            // bottom point
    pt[0].y=y+(radius-3);
    pt[1].x=x-(radius-7);  // left point
    pt[1].y=y+4;
    pt[2].x=x+(radius-7);  // left point
    pt[2].y=y+4;

    Polygon(hDC,pt,3);

    SelectObject(hDC,hOldBr);
    DeleteObject(hBr);
 
    return TRUE;
}

/******************************************************************************
    AdjustHilight:
    Adjust the hilighted area for table columns.
*******************************************************************************/
AdjustHilight(PTERWND w)
{
    int CurPara;
    long l;

    if (HilightType!=HILIGHT_CHAR || PictureClicked) return TRUE;

    if (AdjustBlockForTable(w,&HilightBegRow,&HilightBegCol,&HilightEndRow,&HilightEndCol,TRUE)) 
       PaintTer(w);

    // check for crossing of hilight from header/footer
    if (HilightBegRow>=TotalLines) return TRUE;

    CurPara=pfmt(HilightBegRow);
    if (PfmtId[CurPara].flags&PAGE_HDR_FTR && !AllSelected(w)) {
       for (l=HilightBegRow+1;l<TotalLines && l<=HilightEndRow;l++) if (LineFlags(l)&LFLAG_HDRS_FTRS) break;
       if ((l-1)<HilightEndRow) {
          HilightEndRow=l-1;
          HilightEndCol=LineLen(HilightEndRow);
          PaintTer(w);
       }
    }

    
    return TRUE;
}

/******************************************************************************
   SetFont:
   Select a suitable font for a character format into the display context.
   The last argument is S=Status area, T=Text area, H=Highlighted area,
   N=Not Known.
*******************************************************************************/
SetFont(PTERWND w,HDC hDC,WORD fmt,BYTE area)
{
    HFONT NewFont;

    //****** set color *****
    if (area!='N') SetColor(w,hDC,area,fmt);

    CurFmtId=fmt;           // set the current format id

    //************* skip if picture bitmap *****************
    if (TerFont[fmt].style&PICT) return TRUE;   // not a true font

    NewFont=TerFont[fmt].hFont;

    if (NewFont!=hTerCurFont || hBufDC || InPrinting) SelectObject(hDC,NewFont);

    hTerCurFont=NewFont;                        // current font selected in the context

    return TRUE;
}

/******************************************************************************
   SetColor:
   This function sets the foreground and background colors.
   The second argument is S=Status area, T=Text area, H=Highlighted area,
   P=Paragraph space
*******************************************************************************/
SetColor(PTERWND w,HDC hDC,BYTE area,WORD font)
{
    COLORREF ForeColor,BackColor,FrameBackColor;
    DWORD     shade;
    BYTE red,green,blue;
    BOOL ModifyTextColor=TRUE;
    BOOL FillSolid=false;


    if (CurFrame<0) CurFrame=0;
    if (CurFrame>0) {
       int ParaFID=frame[CurFrame].ParaFrameId;
       if (ParaFID>0 && ParaFID<TotalParaFrames && True(ParaFrame[ParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_CONV))) 
           FillSolid=(ParaFrame[ParaFID].FillPattern!=0);
    } 

    //****************** calculate the shading color if shading specified ****
    BackColor=TerFont[font].TextBkColor;
    if (BackColor==CLR_WHITE) BackColor=ParaBackColor;
    if (BackColor==CLR_WHITE) BackColor=PageColor(w);  // use the default background color instead
    if (area=='P') {                          // paragraph space
        if (ParaBackColor!=CLR_WHITE) BackColor=ParaBackColor;
        else                          BackColor=PageColor(w);;
        area='T';
    }

    FrameBackColor=FrameBkColor;
    if (FrameBackColor==CLR_WHITE) FrameBackColor=PageColor(w); // use the default background color


    if ((FrameShading!=0 || FrameBackColor!=PageColor(w) || PaintFrameFlags&FRAME_FORCE_BKND_CLR)
        && BackColor==PageColor(w) && (area=='T' || area=='H') && ParaShading==0 && ParaBackColor==CLR_WHITE) {
       if (FrameShading==-1) BackColor=FrameBackColor;   // frame background color specified
       else {
          if (FrameBackColor!=PageColor(w) || PaintFrameFlags&FRAME_FORCE_BKND_CLR) {
             int shading=FrameShading>0?FrameShading:0;
             red=(BYTE)((GetRValue(FrameBackColor)*(100-shading))/100);
             green=(BYTE)((GetGValue(FrameBackColor)*(100-shading))/100);
             blue=(BYTE)((GetBValue(FrameBackColor)*(100-shading))/100);
             BackColor=RGB(red,green,blue);
          }
          else {
             shade=((DWORD)FrameShading*0xFF)/100;
             if (shade>0xFF) shade=0xFF;
             shade=(~shade)&0xFF;             // convert to blackness
             BackColor=(shade<<16)|(shade<<8)|shade;    // all comonents the same
             if (FrameShading<80) ModifyTextColor=FALSE;
          }
       }
    }


    //******************** set color *************************************
    if (FullCellHilight) area='H';      // entire area hilighted

    ForeColor=TerFont[font].TextColor;   // apply the tracking color
    if (True(TerFont[font].InsRev)) ForeColor=reviewer[TerFont[font].InsRev].InsColor;
    if (True(TerFont[font].DelRev)) ForeColor=reviewer[TerFont[font].DelRev].DelColor;

    if (PageBkColor!=CLR_WHITE) ModifyTextColor=false;
    if (ForeColor==CLR_AUTO && !(TerFlags3&TFLAG3_NO_TEXT_COLOR_ADJ) && ModifyTextColor)  
        ForeColor=(~BackColor)&0xFFFFFF;  // set it to reverse of the background color
    if (ForeColor==CLR_BLACK) ForeColor=TextDefColor;
    
    
    if (area=='T') {
       if (TerArg.ReadOnly && TerFlags3&TFLAG3_GRAY_READ_ONLY) {
           if (ForeColor==0) ForeColor=0x808080;
           else              ForeColor=ForeColor^PROTECT_COLOR;
       }
       else if ((TerFont[font].style&PROTECT && ModifyProtectColor) || (TerArg.ReadOnly && TerFlags3&TFLAG3_GRAY_READ_ONLY)
           || TerOpFlags&TOFLAG_FNOTE_FRAME
           || (frame[CurFrame].flags&FRAME_GRAY && ModifyProtectColor)
           || (ViewPageHdrFtr && !EditPageHdrFtr && PfmtId[CurPfmt].flags&PAGE_HDR_FTR)
           || (CurSID>=0)) {
           //ForeColor=ForeColor^PROTECT_COLOR;  // in stylesheet session gray the text
           ForeColor=ForeColor^(0x3F3F3F);
       }
    }
    else if (area=='H') {
       if (TerFont[font].style&PROTECT && ModifyProtectColor)
             ForeColor=(~(ForeColor^PROTECT_COLOR))&0xFFFFFF;  // inverse
       else  ForeColor=(~(ForeColor))&0xFFFFFF;  // inverse
       BackColor=(~BackColor)&0xFFFFFF;
    }
    else if (area=='S') {
       ForeColor=StatusColor;
       BackColor=StatusBkColor;
    }


    // apply the paragraph shading
    if (ParaShading && ParaBackColor==CLR_WHITE) {
       shade=((DWORD)ParaShading*0xFF)/10000L;
       if (shade>0xFF) shade=0xFF;
       if (shade>=0x68 && shade<=0x98) shade=0x68;  // provide some contrast
       shade=(shade<<16)|(shade<<8)|shade;    // all comonents the same
       if ((!HtmlMode || !(TerFont[font].style&LinkStyle)) && ForeColor==TerFont[0].TextColor && ParaShading>=80)
          ForeColor=ForeColor^shade;
       if (BackColor==PageColor(w))  BackColor=0xFFFFFF^shade;
       else                          BackColor=BackColor^shade;
    }
    else if (/*ParaShading!=10000 && */ ParaShading!=0/* && ParaBackColor==0*/) {     // reduce the background color
       BYTE shade=(BYTE) (255*(long)ParaShading/10000);

       red=GetRValue(BackColor);
       green=GetGValue(BackColor);
       blue=GetBValue(BackColor);
       //red=MulDiv(ParaShading,255-red,10000);
       //green=MulDiv(ParaShading,255-green,10000);
       //blue=MulDiv(ParaShading,255-blue,10000);
       //BackColor=RGB(255-red,255-green,255-blue);

       red=(red<shade)?0:red-shade;
       green=(green<shade)?0:green-shade;
       blue=(blue<shade)?0:blue-shade;
       BackColor=RGB(red,green,blue);
    }

    // provide contract for the foreground color
    if (ParaShading || ParaBackColor!=CLR_WHITE) {
       COLORREF clr=(ForeColor==CLR_AUTO)?CLR_BLACK:ForeColor;
       if (clr==BackColor) ForeColor=(~BackColor)&0xFFFFFF;
    } 


    // shade the FIELD text
    if ( (TerFont[font].FieldId>0 && IsDynField(w,TerFont[font].FieldId))
      || TerFont[font].FieldId==FIELD_NAME
      || (TerFont[font].FieldId==FIELD_DATA && !(TerFlags2&TFLAG2_NO_SHADE_FIELD_TEXT)) 
      || (TerFont[font].FieldId==FIELD_TEXTBOX  && !(TerFlags2&TFLAG2_NO_SHADE_FIELD_TEXT)) ) {
       ForeColor=ForeColor^(0x3F3F3F);
       BackColor=BackColor^(0x3F3F3F);
    }

    // set the palette
    TerSetPalette(w,hDC,NULL,1,&ForeColor,TRUE);
    TerSetPalette(w,hDC,NULL,1,&BackColor,TRUE);

    // select the color
    SetTextColor(hDC,ForeColor);
    SetBkColor(hDC,BackColor);
   
    CurForeColor=ForeColor;
    CurBackColor=BackColor;

    transparent=EnableTransparent;
    if ((BackColor&0xFFFFFF)!=PageColor(w)) transparent=FALSE;  // background color to be printed
    if (FillSolid) transparent=false;

    SetBkMode(hDC,transparent?TRANSPARENT:OPAQUE);  // set transparency

    return TRUE;
}

/******************************************************************************
   AdjustColor:
   Adjust the color value
*******************************************************************************/
COLORREF AdjustColor(PTERWND w,COLORREF clr,BOOL palettergb)
{
   BYTE red,green,blue;

   red=GetRValue(clr);         // supply adjustment for low color values
   green=GetGValue(clr);
   blue=GetBValue(clr);

   if (Clr256Display) {
      if (red>0 && red<=0x40) red=0x60;
      if (green>0 && green<=0x40) green=0x60;
      if (blue>0 && blue<=0x40) blue=0x60;
   }

   if (palettergb) return PALETTERGB(red,green,blue);
   else            return RGB(red,green,blue);
}

/****************************************************************************
    TerDrawImage:
    Draw the control image to the given device context at the given location
*****************************************************************************/
BOOL WINAPI _export TerDrawImage(HWND hWnd, HDC hDC, HDC hAttribDC, LPRECT pRect, BOOL ReserveNonTextSpace)
{
    PTERWND w;
    int x,y,width,height;
    int DestWidth,DestHeight,pict,BarHeight,BarWidth;
    HBITMAP hOldBM,hMemBM;
    RECT rect=(*pRect);
    RECT SpaceRect;

    //LogPrintf("1");
    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (TerFlags5&TFLAG5_NO_DRAW_IMAGE) return TRUE;  // this function bypassed

    x=TerWinOrgX-TerWinRect.left;
    y=TerWinOrgY-TerWinRect.top;
    width=TerRect.right-TerRect.left;
    height=TerRect.bottom-TerRect.top;

    //LogPrintf("2");

    // reserve space for the non text elements
    if (false && ReserveNonTextSpace) {
       // reserve space for the toolbar and ruler
       BarHeight=TerWinRect.top;
       if (BarHeight>0) {
         SetRect(&SpaceRect,rect.left,rect.top,rect.right,rect.top+BarHeight);
         FillRect(hDC,&SpaceRect,GetStockObject(WHITE_BRUSH));
         rect.top=SpaceRect.bottom;
       }

       // reserve space for the status bar and horzontal scroll bar
       BarHeight=(TerRect.bottom-TerWinRect.bottom);
       if (TerArg.ShowHorBar) BarHeight+=GetSystemMetrics(SM_CYHSCROLL);
       if (BarHeight>0) {
          SetRect(&SpaceRect,rect.left,rect.bottom-BarHeight,rect.right,rect.bottom);
          FillRect(hDC,&SpaceRect,GetStockObject(WHITE_BRUSH));
          rect.bottom-=BarHeight;
       }

       // left border space
       BarWidth=TerWinRect.left-TerRect.left;
       if (BarWidth>0) {
          SetRect(&SpaceRect,rect.left,rect.top,rect.left+BarWidth,rect.bottom);
          FillRect(hDC,&SpaceRect,GetStockObject(WHITE_BRUSH));
          rect.left+=BarWidth;
       }

       // right border space
       BarWidth=TerRect.right-TerWinRect.right;
       if (TerArg.ShowVerBar) BarWidth+=GetSystemMetrics(SM_CXVSCROLL);
       if (BarWidth>0) {
          SetRect(&SpaceRect,rect.right-BarWidth,rect.top,rect.right,rect.bottom);
          FillRect(hDC,&SpaceRect,GetStockObject(WHITE_BRUSH));
          rect.right-=BarWidth;
       }
    }
    //LogPrintf("3");


    DestWidth=rect.right-rect.left;
    DestHeight=rect.bottom-rect.top;

    if (GetDeviceCaps(hDC,TECHNOLOGY)==DT_METAFILE) {
        // retrieve the image in a bitmap
        if (NULL==(hMemBM=CreateCompatibleBitmap(hTerDC,width,height))) return FALSE;
        hOldBM=SelectObject(hMemDC,hMemBM);
        StretchBlt(hMemDC,0,0,width,height,hBufDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight,SRCCOPY);

        SelectObject(hMemDC,hOldBM);

        // retrieve the bits from the bitmap
        if ((pict=Bitmap2DIB(w,hMemBM))<0) return FALSE;
        DeleteObject(hMemBM);

        StretchDIBits(hDC,rect.left,rect.top,DestWidth,DestHeight,0,0,width,height,(LPVOID)TerFont[pict].pImage,(LPVOID)TerFont[pict].pInfo,DIB_RGB_COLORS,SRCCOPY);

        DeleteTerObject(w,pict);
    }
    else {
       int ResX=GetDeviceCaps(hDC,LOGPIXELSX);
       int ResY=GetDeviceCaps(hDC,LOGPIXELSY);
       RECT PrtRect;
       HGLOBAL hBuf;
       DWORD size;
       int NextY;

       PrtRect.left=-1; //-MulDiv(rect.left,1440,ResX);       // pass in twips
       PrtRect.right=-MulDiv(rect.right-rect.left,1440,ResX);
       PrtRect.top=-1;//-MulDiv(rect.top,1440,ResY);    // really 0, but we have -1 to tell TerMergePrint that the units are in twips
       PrtRect.bottom=-MulDiv(rect.bottom-rect.top,1440,ResY);

       hBuf=GetTerBuffer(hTerWnd,&size);
       TerMergePrintRep4(hDC,NULL,hBuf,size,&PrtRect,null,null,0,&NextY,NULL,TRUE,MERGEPRINT_NO_EVAL_MSG);
       GlobalFree(hBuf);

       //StretchBlt(hDC,rect.left,rect.top,DestWidth,DestHeight,hBufDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight,SRCCOPY);
    }

    //LogPrintf("4");

    return TRUE;
}

/******************************************************************************
   WriteBreakLine;
   Write a break line with a break string.
   as 'FrameSpace' segment.
******************************************************************************/
WriteBreakLine(PTERWND w,HDC hDC, LPBYTE BreakString, int x, int y, int width,HPEN hPen)
{
    int TextDist;
    SIZE size;
    HFONT hOldFont;
    COLORREF OldColor;
    BOOL SaveScrRtl=ScrRtl;

    // print page break line centered
    if (ZoomPercent<=50) hOldFont=SelectObject(hDC,TerFont[0].hFont);
    else                 hOldFont=SelectObject(hDC,hRulerFont);
    GetTextExtentPoint(hDC,BreakString,lstrlen(BreakString),&size);
    if (size.cx>=(width*3)/4) size.cx=0;    // not enough space for the text

    if (ScrRtl) x=RtlX(w,x,width,CurFrame,NULL);

    TextDist=(width-size.cx)/2;
    if (TextDist<0) TextDist=0;
    OldColor=SetTextColor(hDC,0);
    if (size.cx>0) TextOut(hDC,x+TextDist,y-(size.cy/2),BreakString,lstrlen(BreakString));
    SelectObject(hDC,hOldFont);
    SetTextColor(hDC,OldColor);
    
    ScrRtl=FALSE;
    if (hPen) {     // draw line using the specified pen
       DrawShadowLine(w,hDC,x,y,x+TextDist,y,hPen,NULL);
       DrawShadowLine(w,hDC,x+TextDist+size.cx,y,x+width,y,hPen,NULL);
    }
    else {          // draw dotted line
       DrawDottedLine(w,hDC,x,y,x+TextDist,y,0);
       DrawDottedLine(w,hDC,x+TextDist+size.cx,y,x+width,y,0);
    }
    ScrRtl=SaveScrRtl;

    return TRUE;
}

/******************************************************************************
   WriteSpaceRect;
   Write a space rectangle such that any intervening frame space is drawn
   as 'FrameSpace' segment.
******************************************************************************/
WriteSpaceRect(PTERWND w,HDC hDC,RECT far *rect)
{
    int FrameX,FrameWidth;
    RECT rect1;

    // non-page mode don't have intervening frames
    if (!TerArg.PageMode || !ContainsParaFrames || frame[CurFrame].ParaFrameId>0) {
       TerTextOut(w,hDC,rect->left,rect->top,rect,NULL,NULL,0,NULL,0,0);
       return TRUE;
    }

    // find intervening frame space
    GetFrameSpace(w,-1L,rect,&FrameX,&FrameWidth,NULL);
    if (FrameX==0 && FrameWidth==0) {           // no intervening frames
       TerTextOut(w,hDC,rect->left,rect->top,rect,NULL,NULL,0,NULL,0,0);
       return TRUE;
    }

    // draw the rectangle in 3 parts
    FrameX=rect->left+PrtToScrX(FrameX);
    FrameWidth=PrtToScrX(FrameWidth);
    rect1.top=rect->top;
    rect1.bottom=rect->bottom;

    // draw the space before the frame
    if (FrameX>rect->left) {
       rect1.left=rect->left;
       rect1.right=FrameX;
       if (rect1.right>rect->right) rect1.right=rect->right;
       TerTextOut(w,hDC,rect1.left,rect1.top,&rect1,NULL,NULL,0,NULL,0,0);
    }

    // draw the frame space
    if (FrameWidth>0 && FrameX<rect->right) {
       rect1.left=FrameX;
       rect1.right=FrameX+FrameWidth;
       if (rect1.right>rect->right) rect1.right=rect->right;
       ParaFrameSpace=TRUE;         // this space reserved for paragraph frames
       if (BkPictId==0) TerTextOut(w,hDC,rect1.left,rect1.top,&rect1,NULL,NULL,0,NULL,0,0);
       ParaFrameSpace=FALSE;        // reset for normal use
    }

    // draw the space after the frame
    if ((FrameX+FrameWidth)<rect->right) {
       rect1.left=FrameX+FrameWidth;
       rect1.right=rect->right;
       TerTextOut(w,hDC,rect1.left,rect1.top,&rect1,NULL,NULL,0,NULL,0,0);
    }

    return TRUE;
}

/******************************************************************************
   WriteTab;
   Write the spaces for the tab character.
******************************************************************************/
WriteTab(PTERWND w,HDC hDC,long line, int y, int RectY,int height,int BaseHeight, int width)
{
   int TabId,x1,x2,y1,TabX;
   RECT rect;
   HBRUSH hOldBrush;

   // get the tab flags
   TabId=PfmtId[pfmt(line)].TabId;
   //if (TabId>0 && TabId<TotalTabs && NextTab>=0 && NextTab<TerTab[TabId].count)
   //     TabFlags=TerTab[TabId].flags[NextTab];
   //else TabFlags=0;

   TabX=NextX;         // tab position
   if (TerArg.PageMode /*&& ShowPageBorder*/) TabX-=(frame[CurFrame].x+frame[CurFrame].SpaceLeft);
   GetTabPos(w,pfmt(line),&TerTab[TabId],TabX,NULL,NULL,&TabFlags,TRUE);

   NextTab++;

   // build the tab rectangle
   rect.left=NextX;
   NextX=rect.right=rect.left+width;
   rect.top=RectY;
   rect.bottom=RectY+height;

   TerTextOut(w,hDC,NextX,y,&rect,NULL,NULL,0,NULL,0,0);

   // show the tab markers
   if (ShowParaMark) {
      int i,ArrowWidth=10,ArrowHeight=4;    // arrow width
      POINT pt[3];
      x1=rect.left+(rect.right-rect.left)/2-ArrowWidth/2;
      if (x1<rect.left) {
        x1=rect.left;
        ArrowWidth=rect.right-rect.left;
      }
      x2=x1+ArrowWidth;
      if (x2>rect.right) x2=rect.right;
      if ((x2-x1)>ArrowWidth/2) {
         y1=rect.top+BaseHeight-5;
         if (y1<=(rect.top)) y1=rect.top+1;     // very thin line or zero-height line

         DrawShadowLine(w,hDC,x1,y1,x2,y1,GetStockObject(BLACK_PEN),NULL);

         // draw the triangle for the arrow
         pt[0].x=x2;      // middle point
         pt[0].y=y1;
         pt[1].x=x2-ArrowHeight;
         if (pt[1].x<rect.left) pt[1].x=rect.left;
         pt[1].y=y1-ArrowHeight/2;
         pt[2].x=x2-ArrowHeight;
         if (pt[2].x<rect.left) pt[2].x=rect.left;
         pt[2].y=y1+ArrowHeight/2;
         if (ScrRtl) for (i=0;i<3;i++) pt[i].x=RtlX(w,pt[i].x,0,CurFrame,NULL);

         hOldBrush=SelectObject(hDC,GetStockObject(BLACK_BRUSH));
         Polygon(hDC,pt,3);
         SelectObject(hDC,hOldBrush);
      }
   }

   // draw dot leaders and underlines
   if (TabFlags==TAB_DOT) {
      int LeadY=RectY+BaseHeight;
      if (LeadY<=RectY) LeadY=RectY+1;
      DrawDottedLine(w,hDC,rect.left,LeadY, rect.right, LeadY,CurForeColor);
   }
   else if (TabFlags==TAB_HYPH) {
      HPEN hPen=CreatePen(PS_DOT,0,CurForeColor);
      int LeadY=RectY+BaseHeight-2;
      if (LeadY<=RectY) LeadY=RectY+1;
      DrawShadowLine(w,hDC,rect.left,LeadY,rect.right,LeadY,hPen,NULL);
      DeleteObject(hPen);
   }
   else if (TabFlags==TAB_ULINE) {
      HPEN hPen=CreatePen(PS_SOLID,TerFont[CurFmtId].style&BOLD?1:0,CurForeColor);
      int LeadY=RectY+BaseHeight;
      if (LeadY<=RectY) LeadY=RectY+1;
      DrawShadowLine(w,hDC,rect.left,LeadY,rect.right,LeadY,hPen,NULL);
      DeleteObject(hPen);
   }

   if (!HiddenText(w,CurFmtId)) {
     if (TerFont[CurFmtId].style&ULINE)
        DrawShadowLine(w,hDC,rect.left,y+TerFont[CurFmtId].BaseHeight+1,rect.right,y+TerFont[CurFmtId].BaseHeight+1,GetStockObject(BLACK_PEN),NULL);
     else if (TerFont[CurFmtId].style&ULINED) {
        DrawShadowLine(w,hDC,rect.left,y+TerFont[CurFmtId].BaseHeight,rect.right,y+TerFont[CurFmtId].BaseHeight,GetStockObject(BLACK_PEN),NULL);
        DrawShadowLine(w,hDC,rect.left,y+TerFont[CurFmtId].BaseHeight+2,rect.right,y+TerFont[CurFmtId].BaseHeight+2,GetStockObject(BLACK_PEN),NULL);
     
     } 
   }

   TabFlags=0;                             // reset the tab flags

   return TRUE;
}

/******************************************************************************
   WriteSpaceSym;
   Write the dot' symbol to show the space
******************************************************************************/
bool WriteSpaceSym(PTERWND w, HDC hDC,int line, int y, int RectY,int height,int BaseHeight, int width)
{
   RECT rect;
   int size;
   HBRUSH hBr;

   // build the tab rectangle
   rect.left=NextX;
   NextX=rect.right=rect.left+width;
   rect.top=RectY;
   rect.bottom=RectY+height;

   TerTextOut(w,hDC,NextX,y,&rect,null,null,0,null,0,0);



   // show the space symbol
   size=TerFont[CurFmtId].BaseHeight/4;
   if (size>=width) size=width-1; 
   if (size<1) size=1;
      
   rect.left=rect.left+width/2-size/2;
   rect.right=rect.left+size;
   rect.top=rect.top+BaseHeight-TerFont[CurFmtId].BaseHeight/2;
   rect.bottom=rect.top+size;
     

   hBr=CreateSolidBrush(CurForeColor);
   if (size<=4) FillRectangle(hDC,&rect,hBr);
   else {
      HBRUSH hOldBr=SelectObject(hDC,hBr);
      HPEN hOldPen=SelectObject(hDC,GetStockObject(NULL_PEN));
      Ellipse(hDC,rect.left,rect.top,rect.right,rect.bottom);
      SelectObject(hDC,hOldPen);
      SelectObject(hDC,hOldBr);
   }

   DeleteObject(hBr);

   return true;
}


/******************************************************************************
   WriteFrameSpace;
   Write the spaces for the intevening frame
******************************************************************************/
WriteFrameSpace(PTERWND w,HDC hDC,long line, int y,int height)
{
   int width,dist,FrameX;
   RECT rect;
   BOOL IsRtl=(LineFlags2(line)&LFLAG2_RTL);

   dm("WriteFrameSpace");

   // calculate the frame begin position
   FrameX=tabw(line)->FrameX;          // relative to the current column
   FrameX+=LineX(line);                // relative to the left edge of the screen

   // calculate distance from text and total frame space width
   dist=PrtToScrX(FrameX)-NextX;
   if (dist<0) dist=0;
   width=PrtToScrX(tabw(line)->FrameWidth);

   tabw(line)->FrameScrWidth=dist+width;     // update the frame screen width

   // find the space to cover the distance from text
   if (dist>0) {
      // build the tab rectangle
      rect.left=NextX;
      NextX=rect.right=rect.left+dist;
      rect.top=y;
      rect.bottom=y+height;

      SetColor(w,hTerDC,'T',DEFAULT_CFMT);
      if (!IsRtl) TerTextOut(w,hDC,NextX,y,&rect,NULL,NULL,0,NULL,0,0);
   }

   if (width>0) {
      // build the tab rectangle
      rect.left=NextX;
      NextX=rect.right=rect.left+width;
      rect.top=y;
      rect.bottom=y+height;

      SetColor(w,hTerDC,'T',DEFAULT_CFMT);

      ParaFrameSpace=TRUE;                     // this space reserved for paragraph frame
      if (!IsRtl) TerTextOut(w,hDC,NextX,y,&rect,NULL,NULL,0,NULL,0,0);
      ParaFrameSpace=FALSE;                    // reset for normal use
   }

   return TRUE;
}

/******************************************************************************
   WriteSpace;
   Write the spaces for the space character in a justified text.
******************************************************************************/
WriteSpace(PTERWND w,HDC hDC,long line, int width,int y, int RectY,int height)
{
   RECT rect;
   int  SaveCurStyle;
   BYTE spaces[]="    ";

   // find the tab width
   if (tabw(line) && tabw(line)->type&INFO_JUST) {
      width+=GetSpaceAdj(w,line,NextSpace);
      NextSpace++;
   }

   // build the tab rectangle
   rect.left=NextX;
   NextX=rect.right=rect.left+width;
   if (OverhangFont>=0) {           // protect italic overhang of the previous text
      rect.left+=DblCharWidth(w,OverhangFont,TRUE,'W',0);
      if (rect.left>=rect.right) return TRUE;
   }

   rect.top=RectY;
   rect.bottom=RectY+height;

   SaveCurStyle=TerFont[CurFmtId].style;
   TerFont[CurFmtId].style=SaveCurStyle;  // to print even the adjustment to the hidden spaces
   ResetUintFlag(TerFont[CurFmtId].style,HIDDEN|FNOTETEXT|FNOTEREST);  // to print even the adjustment to the hidden spaces
   TerTextOut(w,hDC,rect.left,y,&rect,"",NULL,0,NULL,0,0);
   TerFont[CurFmtId].style=SaveCurStyle;

   return TRUE;
}

/******************************************************************************
   ClearEOL;
   Clear out the space until the end of the line.  The arguments give the
   starting co-ordinates.
******************************************************************************/
ClearEOL(PTERWND w,HDC hDC,int x,int y,int LastX,int height,BOOL ClearBknd)
{
   RECT rect;
   int  SaveCurFmtId;
   int  LastY=TerWinOrgY+TerWinHeight;

   dm("ClearEOL");

   if (x==LastX || height==0) return TRUE;

   rect.left=x;
   if (rect.left<TerWinOrgX && y<LastY) rect.left=TerWinOrgX;
   rect.top=y;
   rect.right=LastX;
   if (rect.right==(TerWinOrgX+TerWinWidth)) rect.right++;
   if (rect.right>(TerWinOrgX+TerWinWidth+1) && y<LastY) rect.right=TerWinOrgX+TerWinWidth+1;
   rect.bottom=y+height;

   SaveCurFmtId=CurFmtId;
   CurFmtId=0;                             // to print even the adjustment to the hidden spaces
   SetColor(w,hDC,'T',(WORD)CurFmtId);

   if (ClearBknd) TerOpFlags|=TOFLAG_WRITING_BKND;
   TerOpFlags|=TOFLAG_CLEAR_EOL;

   if (y>(TerWinOrgY+TerWinHeight)) TerTextOut(w,hDC,x,y,&rect,NULL,NULL,0,NULL,0,0);
   else WriteSpaceRect(w,hDC,&rect);

   TerOpFlags=ResetLongFlag(TerOpFlags,TOFLAG_CLEAR_EOL);
   if (ClearBknd) TerOpFlags=ResetLongFlag(TerOpFlags,TOFLAG_WRITING_BKND);

   CurFmtId=SaveCurFmtId;

   return TRUE;
}


/******************************************************************************
    SetScrollBars:
    Set horizontal and vertical scroll bar positions
*******************************************************************************/
SetScrollBars(PTERWND w)
{
    int  ScrollPos,MaxX,width,ThumbSize,ScrollRange;
    long l,LastY,PageStartY,StartY,TextLastY,DocSize;

    dm("SetScrollBars");

    if (TerArg.ShowVerBar) {   // check for any change in vertical bar status
       ThumbSize=0;
       if (TerArg.PageMode) {
          // find the last Y position
          PageStartY=SumPageScrHeight(w,0,FirstFramePage);
          StartY=PageStartY+TerWinOrgY;

          //if (CurPage==(TotalPages-1)) {
          //   TextLastY=PageStartY+CurPageHeight;
          //   LastY=TextLastY-TerFont[0].height-TerWinHeight;
          //   if (LastY<StartY) LastY=StartY;
          //
          //   DocSize=SumPageScrHeight(w,0,TotalPages);
          //}
          //else LastY=TextLastY=DocSize=SumPageScrHeight(w,0,TotalPages);

          TextLastY=DocSize=SumPageScrHeight(w,0,TotalPages);
          LastY=TextLastY/*-TerFont[0].height*/-TerWinHeight;

          if (DocSize<TerWinHeight && TerWinOrgY>0) PostMessage(hTerWnd,WM_VSCROLL,SB_PAGEUP,0L);  // post a page-up message to show the top of the document

          if (Win32) {
             if (DocSize==0) ThumbSize=SCROLL_RANGE;
             else {
                ThumbSize=MulDiv(TerWinHeight,SCROLL_RANGE,DocSize);
                if (ThumbSize<MinThumbHt) ThumbSize=MinThumbHt;
                if (ThumbSize>SCROLL_RANGE) ThumbSize=SCROLL_RANGE;
             }
             if (ThumbSize==SCROLL_RANGE && TerWinOrgY!=0) ThumbSize--;
          }

          ScrollRange=SCROLL_RANGE-ThumbSize;
          
          if (LastY<=0) ScrollPos=0;
          else  if (StartY>0 && (StartY+TerWinHeight)>=TextLastY) ScrollPos=ScrollRange;
          else ScrollPos=MulDiv(StartY,ScrollRange,LastY);
       }
       else {
          int CurSize=0;
          ScrollRange=SCROLL_RANGE;
          DocSize=0;

          if (Win32) {
             if (TotalLines>500) ThumbSize=MinThumbHt;
             else {

                // find the total line height and height upto the current line
                for (l=0;l<TotalLines;l++) {
                   int height=ScrLineHeight(w,l,FALSE,FALSE);   // line height
                   DocSize+=height;
                   if (l<CurLine) CurSize+=height;
                }
                if (WinYOffsetLine!=-1) CurSize+=WinYOffset;

                if (DocFitsInWindow(w) && BeginLine>0) {
                   BeginLine=0;
                   CurRow=CurLine;
                   WrapFlag=WRAP_OFF;
                   PaintTer(w);
                }

                if (DocSize==0) ThumbSize=SCROLL_RANGE;
                else {
                   ThumbSize=(int)((long)TerWinHeight*SCROLL_RANGE/DocSize);
                   if (ThumbSize<MinThumbHt) ThumbSize=MinThumbHt;
                   if (ThumbSize>SCROLL_RANGE) ThumbSize=SCROLL_RANGE;
                }
             }
             ScrollRange=SCROLL_RANGE-ThumbSize;
          }

          //ScrollPos=(int)(BeginLine*ScrollRange/TotalLines);
          //if (BeginLine>0 && BeginLine==(TotalLines-1)) ScrollPos=SCROLL_RANGE;
          
          ScrollPos=MulDiv(CurSize,ScrollRange,DocSize);
          if (ScrollPos>ScrollRange) ScrollPos=ScrollRange;

       }

       if (ScrollPos!=VerScrollPos || ThumbSize!=VerThumbSize) { // redraw
          #if defined(WIN32)
             SCROLLINFO ScrollInfo;

             if (ScrollPos>(SCROLL_RANGE-ThumbSize)) ScrollPos=SCROLL_RANGE-ThumbSize;

             FarMemSet(&ScrollInfo,0,sizeof(ScrollInfo));
             ScrollInfo.cbSize=sizeof(ScrollInfo);
             ScrollInfo.fMask=SIF_POS|SIF_DISABLENOSCROLL;
             if (ThumbSize>0) ScrollInfo.fMask|=SIF_PAGE;
             ScrollInfo.nPage=ThumbSize+1;    // add 1 so that nPage can go beyond SCROLL_RANGE to disable the scroll bar
             ScrollInfo.nPos=VerScrollPos=ScrollPos;
             SetScrollInfo(hTerWnd,SB_VERT,&ScrollInfo,TRUE);
             VerThumbSize=ThumbSize;
          #else
             VerScrollPos=ScrollPos;
             SetScrollPos(hTerWnd,SB_VERT,VerScrollPos,TRUE);
          #endif
       }
    }

    if (TerArg.ShowHorBar) {   // check for any change in horizontal bar status
       ThumbSize=0;
       if (TerArg.WordWrap && (TerArg.PageMode || !HScrollAllowed(w))) {
           if (TerArg.PageMode) MaxX=PageTextWidth(w);
           else                 MaxX=TerWrapWidth(w,CurLine,-1);


           if (Win32) {
              if (MaxX==0) ThumbSize=SCROLL_RANGE;
              else {
                if (MaxX<=TerWinWidth && TerWinOrgX>0) {
                   int CurX=ColToUnits(w,CurCol,CurLine,LEFT);
                   if (CurX<TerWinWidth && LineTextAngle(w,CurLine)==0) {     // extra check to co-operate with HorScrollCheck function
                      TerWinOrgX=0;
                      SetTerWindowOrg(w);      // set the window origin
                      WrapFlag=WRAP_OFF;
                      PaintTer(w);
                   }
                }

                ThumbSize=(int)((long)TerWinWidth*SCROLL_RANGE/MaxX);
                if (ThumbSize<MinThumbHt) ThumbSize=MinThumbHt;
                if (ThumbSize>SCROLL_RANGE) ThumbSize=SCROLL_RANGE;
              }
           }

       }
       else {
           // calc the max width of the text lines on the screen
           MaxX=0;
           for (l=BeginLine;l<=frame[0].ScrLastLine;l++) {
              width=GetLineWidth(w,l,TRUE,TRUE);
              if (width>MaxX) MaxX=width;
           }

           if (Win32) {
              if (MaxX<=TerWinWidth && TerWinOrgX>0) {
                 TerWinOrgX=0;
                 SetTerWindowOrg(w);      // set the window origin
                 WrapFlag=WRAP_OFF;
                 PaintTer(w);
              }
              
              if (MaxX==0) ThumbSize=SCROLL_RANGE;
              else {
                 ThumbSize=(int)((long)TerWinWidth*SCROLL_RANGE/MaxX);
                 if (ThumbSize<MinThumbHt) ThumbSize=MinThumbHt;
                 if (ThumbSize>SCROLL_RANGE) ThumbSize=SCROLL_RANGE;
              }
           }
       }

       // Get scroll pos
       ScrollRange=SCROLL_RANGE-ThumbSize;

       width=MaxX-TerWinWidth;
       if (width<=0) width=1;

       ScrollPos=(int)((long)TerWinOrgX*ScrollRange/width);
       if (TerWinOrgX>=MaxX-TerWinWidth) ScrollPos=ScrollRange;
       if (TerWinOrgX==0) ScrollPos=0;

       if (ScrollPos!=HorScrollPos || ThumbSize!=HorThumbSize) { // redraw
          #if defined(WIN32)
             SCROLLINFO ScrollInfo;
             FarMemSet(&ScrollInfo,0,sizeof(ScrollInfo));
             ScrollInfo.cbSize=sizeof(ScrollInfo);
             ScrollInfo.fMask=SIF_POS|SIF_DISABLENOSCROLL;
             if (ThumbSize>0) ScrollInfo.fMask|=SIF_PAGE;
             ScrollInfo.nPage=ThumbSize+1;    // add 1 so that nPage can go beyond SCROLL_RANGE to disable the scroll bar
             ScrollInfo.nPos=HorScrollPos=ScrollPos;
             SetScrollInfo(hTerWnd,SB_HORZ,&ScrollInfo,TRUE);
             HorThumbSize=ThumbSize;
          #else
             HorScrollPos=ScrollPos;
             SetScrollPos(hTerWnd,SB_HORZ,HorScrollPos,TRUE);
          #endif
       }
    }
    return TRUE;
}

/******************************************************************************
    VerThumbPos:
    Postion vertical thumb
*******************************************************************************/
VerThumbPos(PTERWND w,WPARAM wParam,LPARAM lParam)
{
    if (InPrintPreview) PreviewPageVert(w,THUMB_POS(wParam,lParam));
    else if ((WORD)VerScrollPos!=THUMB_POS(wParam,lParam)) {
        if (TerArg.PageMode) PgmPageVert(w,THUMB_POS(wParam,lParam));
        else {
           int ScrollRange=(Win32?(SCROLL_RANGE-VerThumbSize):SCROLL_RANGE);
           TerPosLine(w,(1+((long)TotalLines*THUMB_POS(wParam,lParam))/ScrollRange));
           //if (CurLine==(TotalLines-1) && CurLine!=BeginLine) {
           BeginLine=CurLine;
           CurRow=0;
           PaintTer(w);
           //}
        }
    }

    return TRUE;
}


/******************************************************************************
   DrawFootnote:
   Draw the footnote frame.
*******************************************************************************/
DrawFootnote(PTERWND w,HDC hDC, int FrameNo, BOOL screen)
{
   long l;
   RECT rect;
   int x,y,y1,margin,width;
   COLORREF SaveBkColor;
   HPEN hPen;
   int page;

   dm("DrawFootnote");

   // clear the frame area
   CurFrame=FrameNo;
   
   frame[FrameNo].SpaceRight=frame[FrameNo].SpaceLeft;

   x=frame[FrameNo].x+frame[FrameNo].SpaceLeft;

   width=frame[FrameNo].width-frame[FrameNo].SpaceLeft-frame[FrameNo].SpaceRight;
   if (!screen) x+=PrtLeftMarg;
   y=frame[FrameNo].y;

   if (screen) {
      rect.left=x;  // frame[FrameNo].x;
      if (BorderShowing) rect.right=rect.left+width; // frame[FrameNo].width;
      else               rect.right=TerWinOrgX+TerWinWidth;
      rect.top=frame[FrameNo].y;
      rect.bottom=rect.top+frame[FrameNo].height;


      SaveBkColor=CurBackColor;
      CurBackColor=PageColor(w);
      SetBkColor(hDC,CurBackColor);

      TerTextOut(w,hDC,rect.left,rect.top,&rect,NULL,NULL,0,NULL,0,0);

      CurBackColor=SaveBkColor;
      SetBkColor(hDC,CurBackColor);

      margin=TwipsToScrY(FNOTE_MARGIN);   // top footnote margin
   }
   else margin=TwipsToPrtY(FNOTE_MARGIN);   // top footnote margin


   y+=margin;   // top footnote margin

   // extract the footnote text to display
   page=LinePage(frame[FrameNo].PageFirstLine);
   for (l=frame[FrameNo].PageFirstLine;l<=frame[FrameNo].PageLastLine;l++) {
      if (l>=TotalLines) break;
      if (LinePage(l)!=page) continue;
      if (LineFlags(l)&LFLAG_FNOTETEXT) y+=ExtractFootnote(w,hDC,x,y,l,GetSection(w,l),screen,TRUE);
   }


   // draw the top partial line
   if (NULL==(hPen=CreatePen(PS_SOLID,0,0xc0c0c0))) return TRUE;  // choose one of the standard color
   y1=frame[CurFrame].y+margin/2;
   if (IsDefLangRtl(w)) DrawShadowLine(w,hDC,x+width,y1,x+width*2/3,y1,hPen,NULL);
   else                 DrawShadowLine(w,hDC,x,y1,x+width/3,y1,hPen,NULL);
   DeleteObject(hPen);


   // draw the bottom divider line
   y1=frame[CurFrame].y+frame[CurFrame].height-margin/2;
   if (screen && !BorderShowing) DrawDottedLine(w,hDC,x,y1,frame[CurFrame].x+frame[CurFrame].width,y1,0);

   return TRUE;
}

/******************************************************************************
   DrawOneFootnote:
   Draw one footnote from the wrap arrays.  This function returns the
   height of the footnote lines.
*******************************************************************************/
DrawOneFootnote(PTERWND w,HDC hDC, int x, int y, long line, int BufLen, int WrapWidth, BOOL screen, BOOL draw)
{
   int height=0;
   int i,BegPos=0,EndPos,SpacePos,CurWidth,CurPrtWidth,width,len;
   int PrtHeight,BaseHeight,ExtLead,count=0,indent=0;
   BOOL SpaceFound=FALSE;
   WORD LineCharWidth[MAX_WIDTH+1];

   TerOpFlags2|=TOFLAG2_DRAW_FNOTE;

   while (BegPos<BufLen) {   // Extrat each line
      EndPos=BegPos;
      SpacePos=-1;
      width=0;
      if (count>0) width=indent;  // indent the second line onward

      while (TRUE) {
         if (TRUE || WrapCharWidth) {
            CurWidth=WrapCharWidth[EndPos];
            CurPrtWidth=screen?ScrToPrtX(CurWidth):CurWidth;
         }
         else  CurWidth=DblCharWidth(w,WrapCfmt[EndPos],FALSE,wrap[EndPos],WrapLead[EndPos]);  // character width in printer unit
         LineCharWidth[EndPos-BegPos]=CurWidth;

         if (count==0 && !SpaceFound) indent+=CurWidth;
         if (wrap[EndPos]==' ') SpaceFound=TRUE;

         if ( wrap[EndPos]!=ParaChar && wrap[EndPos]!=LINE_CHAR
           && (width+CurPrtWidth)<WrapWidth && (EndPos-BegPos)<MAX_WIDTH && (EndPos+1)<BufLen) {

            if (wrap[EndPos]==' ') SpacePos=EndPos;
            width+=CurPrtWidth;
            EndPos++;
            continue;
         }

         count++;

         if (SpacePos>=0 && (width+CurPrtWidth)>WrapWidth) EndPos=SpacePos;

         len=EndPos-BegPos+1;

         // find the height
         PrtHeight=GetTextHeight(w,&(wrap[BegPos]),&(WrapCfmt[BegPos]),len,FALSE,&BaseHeight,&ExtLead);
         if (screen) height+=PrtToScrY(PrtHeight);
         else        height+=PrtHeight;
         if (!draw) break;


         // draw the footnote line here
         TerOpFlags|=TOFLAG_FNOTE_FRAME;
         TempCfmt=&(WrapCfmt[BegPos]);
         TempLead=mbcs?(&(WrapLead[BegPos])):NULL;
         NextX=x;

         // build line segment for rtl footnotes
         if (FALSE && LineFlags2(line)&LFLAG2_RTL) {
            ScrRtl=TRUE;
            pScrSeg=MemAlloc(sizeof(struct StrLineSeg));
            CurScrSeg=0;
            TotalScrSeg=1;
          
            pScrSeg[0].col=0;
            pScrSeg[0].count=len;
            pScrSeg[0].x=NextX;
            pScrSeg[0].width=0;
            for (i=0;i<len;i++) pScrSeg[0].width+=LineCharWidth[i];
            pScrSeg[0].rtl=TRUE;
         } 
         if (LineFlags2(line)&LFLAG2_RTL) {
            ScrRtl=TRUE;
            pScrSeg=GetLineSeg2(w,-1,LineCharWidth,&TotalScrSeg,len,frame[CurFrame].SpaceLeft,&(wrap[BegPos]),&(WrapCfmt[BegPos]));
            CurScrSeg=0;
         }

         
         if (screen) {
            // get the character width array
            if (pLineCharWidth) MemFree(pLineCharWidth);
            pLineCharWidth=MemAlloc(sizeof(WORD)*(len+1));
            if (pLineCharWidth) {
               LPBYTE ptr=&(wrap[BegPos]);
               for (i=0;i<len;i++) {
                  //pLineCharWidth[i]=TerFont[TempCfmt[i]].CharWidth[ptr[i]];
                  pLineCharWidth[i]=LineCharWidth[i];
               }
            }

            GetTextHeight(w,&(wrap[BegPos]),&(WrapCfmt[BegPos]),len,TRUE,&BaseHeight,&ExtLead);
            if (count>1) NextX+=PrtToScrX(indent);
            DisplayText(w,hDC,0,y,PrtToScrY(PrtHeight),BaseHeight,&wrap[BegPos],len,line,FALSE,'T');

            y+=PrtToScrY(PrtHeight);
            if (pLineCharWidth) MemFree(pLineCharWidth);
            pLineCharWidth=NULL;
         }
         else {
            if (count>1) NextX+=indent;

            PrintOneLine(w,line,hPrtDataDC,hDC,NextX,y,&wrap[BegPos],&WrapCfmt[BegPos],
                &WrapLead[BegPos],len,PrtHeight,BaseHeight,NULL,pfmt(line),0,0,ExtLead,CurFrame,
                PrtLeftMarg,PageX,LineCharWidth,false,0,0); 

            //PrintNonWrapLine(w,line,hPrtDataDC,hDC,NextX,y,&wrap[BegPos],&WrapCfmt[BegPos],
            //    &WrapLead[BegPos],len,PrtHeight,BaseHeight,NULL,pfmt(line),0,0,ExtLead,0,0,LineCharWidth);
            y+=PrtHeight;
         }

         if (LineFlags2(line)&LFLAG2_RTL) {
            ScrRtl=FALSE;
            MemFree(pScrSeg);
            pScrSeg=NULL;
            TotalScrSeg=CurScrSeg=0;
         } 

         ResetUintFlag(TerOpFlags,TOFLAG_FNOTE_FRAME);

         break;
      }

      BegPos=EndPos+1;
   }

   ResetUintFlag(TerOpFlags2,TOFLAG2_DRAW_FNOTE);

   return height;
}

/******************************************************************************
   DrawAnimPict:
   Draw the animation picture
*******************************************************************************/
DrawAnimPict(PTERWND w, int TimerId)
{
   int pict=TimerId-TIMER_ANIM;    // initial picture id associated with the timer
   int CurPict;

   if (InDialogBox || InPrinting) return TRUE;  // suspend animation

   KillTimer(hTerWnd,TimerId);     // kill the current timer

   if (pict<0 || pict>=TotalFonts) return FALSE;
   if (!TerFont[pict].InUse || !(TerFont[pict].style&PICT)) return FALSE;

   if (!TerFont[pict].anim) return FALSE;
   TerFont[pict].anim->TimerId=0;  // timer off

   if (!(TerFont[pict].flags&FFLAG_PICT_DRAWN)) return TRUE;  // The picture no longer on the screen

   // get the current picture to draw
   CurPict=TerFont[pict].anim->NextAnim;   // current picture to draw
   if (!TerFont[CurPict].InUse || !(TerFont[CurPict].style&PICT)) return FALSE;

   // draw the current picture
   TerSetClipRgn(w);                    // create clipping region

   TerFont[CurPict].CharWidth[PICT_CHAR]=TerFont[pict].CharWidth[PICT_CHAR];
   TerFont[CurPict].height=TerFont[pict].height;

   if (TerFont[pict].FrameType!=PFRAME_NONE) SetPictSize(w,CurPict,TwipsToScrY(TerFont[CurPict].PictHeight),TwipsToScrX(TerFont[CurPict].PictWidth),FALSE);

   PictOut(w,hTerDC,TerFont[pict].PictX+TerWinOrgX,TerFont[pict].PictY+TerWinOrgY,CurPict,1);

   if (TerFont[pict].FrameType!=PFRAME_NONE) SetPictSize(w,CurPict,0,0,TRUE);           // set it to the icon size

   TerResetClipRgn(w);                  // reset clip before show caret

   // advance the animation sequence
   TerFont[pict].anim->CurAnim=CurPict;

   SetAnimTimer(w,pict);

   return TRUE;
}

/******************************************************************************
   DrawLineObject:
   Draw the line object
*******************************************************************************/
DrawLineObject(PTERWND w, HDC hDC, int FrameNo)
{
   int ParaFID=frame[FrameNo].ParaFrameId;
   int pict,x1,y1,x2,y2,thick,PenStyle=PS_SOLID;
   HPEN hPen;

   if (frame[FrameNo].empty || !(ParaFrame[ParaFID].flags&PARA_FRAME_LINE)) return TRUE;
   if (ParaFrame[ParaFID].flags&PARA_FRAME_NO_LINE) return TRUE;   // do not draw this line

   // set the current position for the shape object
   pict=ParaFrame[ParaFID].pict;
   if (pict>0 && pict<TotalFonts && TerFont[pict].PictType==PICT_SHAPE) { // record the current x/y position
      TerFont[pict].PictX=frame[FrameNo].x-TerWinOrgX;
      TerFont[pict].PictY=frame[FrameNo].y-TerWinOrgY;
      TerFont[pict].DispFrame=FrameNo;
   } 

   // get the line coordinates
   GetLinePoints(w,FrameNo,&x1,&y1,&x2,&y2);

   // create a pen
   if (InPrinting) thick=TwipsToPrtX(ParaFrame[ParaFID].LineWdth);
   else            thick=TwipsToScrX(ParaFrame[ParaFID].LineWdth);


   if (ParaFrame[ParaFID].flags&PARA_FRAME_DOTTED) {   // uses a dotted pen
      PenStyle=PS_DOT;
      thick=1;
   }

   if (NULL==(hPen=CreatePen(PenStyle,thick,ParaFrame[ParaFID].LineColor))) return TRUE;

   DrawShadowLine(w,hDC,x1,y1,x2,y2,hPen,NULL);

   DeleteObject(hPen);

   return TRUE;
}

/******************************************************************************
   DrawPictFrame:
   Draw the picture frame
*******************************************************************************/
DrawPictFrame(PTERWND w, HDC hDC, int FrameNo)
{
   RECT rect;
   int pict,ParaFID,x,y,width,height;

   dm("DrawPictFrame");

   x=frame[FrameNo].x;
   y=frame[FrameNo].y;
   width=frame[FrameNo].width;
   height=frame[FrameNo].height;

   // draw the frame rectangle
   SetRect(&rect,x,y-frame[FrameNo].SpaceTop,x+width,y+height);
   SetColor(w,hDC,'T',DEFAULT_CFMT);
   TerTextOut(w,hDC,rect.left,rect.top,&rect,NULL,NULL,0,NULL,0,0);

   // draw the picture
   ParaFID=frame[FrameNo].ParaFrameId;
   pict=ParaFrame[ParaFID].pict;

   if (!(TerFont[pict].InUse) || !(TerFont[pict].style&PICT)) return TRUE;

   rect.top=y;
   rect.left=x+frame[FrameNo].SpaceLeft;
   rect.right=x+width-frame[FrameNo].SpaceRight;
   rect.bottom=y+height-frame[FrameNo].SpaceBot;

   // set the picture to full size
   SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),FALSE);

   TerOpFlags|=TOFLAG_DRAWING_FRAME_PICT;

   TerPictOut(w,hDC,rect.left,rect.top,&rect,pict,1);

   ResetLongFlag(TerOpFlags,TOFLAG_DRAWING_FRAME_PICT);

   // reset the picture to the icon size
   SetPictSize(w,pict,0,0,TRUE);           // set it to the icon size

   TerFont[pict].DispFrame=FrameNo;        // record the display frame number

   return TRUE;
}

/******************************************************************************
   DrawRectObject:
   Draw the rectangle object
*******************************************************************************/
DrawRectObject(PTERWND w, HDC hDC, int FrameNo)
{
   int pict,ParaFID=frame[FrameNo].ParaFrameId,PenWidth;
   HBRUSH hBrush=NULL,hOldBrush;
   RECT rect;
   COLORREF SaveForeColor,SaveBackColor;
   BOOL exists=FALSE;
   HPEN hPen,hOldPen;
   COLORREF BackColor;

   if (frame[FrameNo].empty || !(ParaFrame[ParaFID].flags&PARA_FRAME_RECT)) return TRUE;

   // set the current position for the shape object
   pict=ParaFrame[ParaFID].pict;
   if (pict>0 && pict<TotalFonts && TerFont[pict].PictType==PICT_SHAPE) { // record the current x/y position
      TerFont[pict].PictX=frame[FrameNo].x-TerWinOrgX;
      TerFont[pict].PictY=frame[FrameNo].y-TerWinOrgY;
      TerFont[pict].DispFrame=FrameNo;
   } 

   // build the rectangle
   rect.left=frame[FrameNo].x;
   rect.right=rect.left+frame[FrameNo].width;
   if (rect.right==rect.left) rect.right++;

   rect.top=frame[FrameNo].y;
   rect.bottom=rect.top+frame[FrameNo].height;
   if (rect.bottom==rect.top) rect.bottom++;

   // check if segment already exists
   CurFmtId=0;
   SaveForeColor=CurForeColor;            // save the color variables
   SaveBackColor=CurBackColor;
   CurForeColor=CLR_WHITE;                 // set the color variables
   CurBackColor=ParaFrame[ParaFID].BackColor;
   if (!InPrinting && !InPrintPreview) exists=TextSegmentExists(w,rect.left,rect.top,&rect,0,"");
   CurForeColor=SaveForeColor;            // restore the color variables
   CurBackColor=SaveBackColor;
   if (exists) return TRUE;


   // get the background color
   BackColor=ParaFrame[ParaFID].BackColor;
   if (ParaFrame[ParaFID].FillPattern>=2 && ParaFrame[ParaFID].FillPattern<=13) {
      int pct[]={5,10,20,25,30,40,50,60,70,75,80,90};
      int idx=ParaFrame[ParaFID].FillPattern-2;    // FillPattern 2 is 5 percent shading
      int shading=pct[idx];
             
      BYTE red=(BYTE)((GetRValue(BackColor)*(100-shading))/100);
      BYTE green=(BYTE)((GetGValue(BackColor)*(100-shading))/100);
      BYTE blue=(BYTE)((GetBValue(BackColor)*(100-shading))/100);
      BackColor=RGB(red,green,blue);
   } 


   if (ParaFrame[ParaFID].ShapeType==SHPTYPE_ELLIPSE) {
      BOOL fill=(ParaFrame[ParaFID].FillPattern>0 && BackColor!=PageColor(w));
      BOOL PenCreated=false;

      if (fill) {
         if (NULL==(hBrush=CreateSolidBrush(BackColor))) return TRUE;
      }
      else hBrush=GetStockObject(NULL_BRUSH);
         
      hOldBrush=SelectObject(hDC,hBrush);
      
      if (ParaFrame[ParaFID].LineWdth>0 && ParaFrame[ParaFID].flags&PARA_FRAME_BOXED) {

         // draw left and right lines
         if (InPrinting) PenWidth=TwipsToPrtX(ParaFrame[ParaFID].LineWdth);
         else            PenWidth=TwipsToScrX(ParaFrame[ParaFID].LineWdth);
         if (InPrintPreview && PenWidth<ScrToPrtX(1)) PenWidth=ScrToPrtX(1);  // don't let the line disappear

         if (PenWidth>0) {
            int PenStyle=PS_SOLID;

            if (ParaFrame[ParaFID].flags&PARA_FRAME_DOTTED) {
               PenStyle=PS_DOT;
               PenWidth=1;
            }

            hPen=CreatePen(PenStyle,PenWidth,ParaFrame[ParaFID].LineColor);
            PenCreated=true;   // draw outline
         }
      }
      else hPen=GetStockObject(NULL_PEN);
      
      hOldPen=SelectObject(hDC,hPen);

      Ellipse(hDC,rect.left,rect.top,rect.right,rect.bottom);
      
      SelectObject(hDC,hOldPen);
      if (PenCreated) DeleteObject(hPen);
      
      SelectObject(hDC,hOldBrush);
      if (fill) DeleteObject(hBrush);

   }
   else {
      if (ParaFrame[ParaFID].FillPattern>0 && BackColor!=PageColor(w)) {
         if (NULL==(hBrush=CreateSolidBrush(BackColor))) return TRUE;
         FillRect(hDC,&rect,hBrush);
         DeleteObject(hBrush);
      }

      // draw any fill pictrue
      if (!InPrinting && True(ParaFrame[ParaFID].flags&PARA_FRAME_FILL_PICT) && ParaFrame[ParaFID].FillPict>0) {
         pict=ParaFrame[ParaFID].FillPict;
         TerFont[pict].PictWidth=rect.right-rect.left;
         TerFont[pict].PictHeight=rect.bottom-rect.top;
         SetPictSize(w,pict,TerFont[pict].PictHeight,TerFont[pict].PictWidth,false);
         PictOut(w,hDC,rect.left,rect.top,ParaFrame[ParaFID].FillPict,1);
      } 
      
      // draw the borders
      if (ParaFrame[ParaFID].LineWdth>0 && ParaFrame[ParaFID].flags&PARA_FRAME_BOXED) {

         // draw left and right lines
         if (InPrinting) PenWidth=TwipsToPrtX(ParaFrame[ParaFID].LineWdth);
         else            PenWidth=TwipsToScrX(ParaFrame[ParaFID].LineWdth);
         if (InPrintPreview && PenWidth<ScrToPrtX(1)) PenWidth=ScrToPrtX(1);  // don't let the line disappear

         if (PenWidth>0) {
            int PenStyle=PS_SOLID;

            if (ParaFrame[ParaFID].flags&PARA_FRAME_DOTTED) {
               PenStyle=PS_DOT;
               PenWidth=1;
            }

            hPen=CreatePen(PenStyle,PenWidth,ParaFrame[ParaFID].LineColor);
            hOldPen=SelectObject(hDC,hPen);
            DrawShadowLine(w,hDC,rect.left,rect.top,rect.left,rect.bottom,hPen,NULL);
            DrawShadowLine(w,hDC,rect.right,rect.top,rect.right,rect.bottom,hPen,NULL);
            SelectObject(hDC,hOldPen);
            DeleteObject(hPen);
         }


         // draw top and bottom lines
         if (InPrinting) PenWidth=TwipsToPrtY(ParaFrame[ParaFID].LineWdth);
         else            PenWidth=TwipsToScrY(ParaFrame[ParaFID].LineWdth);
         if (InPrintPreview && PenWidth<ScrToPrtY(1)) PenWidth=ScrToPrtY(1);  // don't let the line disappear

         if (PenWidth>0) {
            int PenStyle=PS_SOLID;
            if (ParaFrame[ParaFID].flags&PARA_FRAME_DOTTED) {
               PenStyle=PS_DOT;
               PenWidth=1;
            }

            hPen=CreatePen(PenStyle,PenWidth,ParaFrame[ParaFID].LineColor);

            hOldPen=SelectObject(hDC,hPen);
            DrawShadowLine(w,hDC,rect.left,rect.top,rect.right,rect.top,hPen,NULL);
            DrawShadowLine(w,hDC,rect.left,rect.bottom,rect.right,rect.bottom,hPen,NULL);
            SelectObject(hDC,hOldPen);
            DeleteObject(hPen);
         }
      }
   }
   return TRUE;
}

/******************************************************************************
   DrawParaBorder:
   This routine is called to draw a fragment of paragraph border.  Only the
   fragment that lies within the given rectangle is drawn.
*******************************************************************************/
DrawParaBorder(PTERWND w,HDC hDC,RECT far * TextRect,BOOL outer)
{
    int x1,y1,x2,y2,left,right,thick,LineThickness,DeltaY1,DeltaY2,DeltaX,BoxSpace;
    BOOL DrawLeft=FALSE,DrawRight=FALSE,DrawTop=FALSE,DrawBot=FALSE;
    HPEN hPen;
    RECT rect;

    rect=(*TextRect);

    rect.top-=SpaceBef;
    rect.bottom+=SpaceAft;

    if (InPrinting) BoxSpace=TwipsToPrtY(ParaBoxSpace);
    else            BoxSpace=TwipsToScrY(ParaBoxSpace);
    //rect.top-=BoxSpace;          // space between the text and the border
    //rect.bottom+=BoxSpace;

    rect.top+=ParaSpcBef;          // 20050919 do not include paragraph bef/aft space in the box
    rect.bottom-=ParaSpcAft;  

    // calculate border thickness and adjust left and right
    if (InPrinting) thick=PointsToPrtX(1);  // single line thick ness
    else            thick=PointsToScrX(1);

    if (thick<1) thick=1;        // one pixel minimum
    if (TextBorder&BOX_THICK)  LineThickness=2*thick;
    else                       LineThickness=thick;

    DeltaY1=0;
    if (TextBorder&BOX_DOUBLE) {
        if (outer) {                      // draw outer box
           DeltaX=2*LineThickness+thick;
           //DeltaY1=0;
           DeltaY1=-(thick+LineThickness);
        }
        else {                            // draw inner box
           DeltaX=LineThickness;
           //DeltaY1=thick+LineThickness;
        }
    }
    else {
       //DeltaY1=thick;                     // one point clearance
       DeltaX=LineThickness;
    }

    if (TerFlags3&TFLAG3_LARGE_PARA_BORDER) DeltaY2=DeltaY1+1;                    // additional clearance at the bottom
    else {                // para space bef/aft to be outside the border
       DeltaY2=DeltaY1;
       //if (SpaceAft==0) DeltaY2++;          // additional clearance at the bottom

       DeltaY1+=SpaceBef;
       //if (SpaceBef>0) DeltaY1--;           // allow little bit more space on the top
       DeltaY2+=SpaceAft;
    }



    left=BoxLeft-(DeltaX-LineThickness)-BoxSpace;     // adjustment to left and right
    right=BoxRight+(DeltaX-LineThickness)+BoxSpace;

    // get the sides to draw
    if (TextBorder&BOX_TOP) DrawTop=TRUE;
    if (TextBorder&(BOX_BOT|BOX_BETWEEN)) DrawBot=TRUE;
    if (TextBorder&BOX_LEFT && left>=rect.left && left<rect.right) DrawLeft=TRUE;
    if (TextBorder&BOX_RIGHT && right>=rect.left && right<rect.right) DrawRight=TRUE;
    if (!(DrawTop|DrawBot|DrawLeft|DrawRight)) return TRUE;  // nothing to draw

    // create the border pen
    if (NULL==(hPen=CreatePen(PS_SOLID,LineThickness,ParaBoxColor==CLR_AUTO?0:ParaBoxColor))) return TRUE;

    // draw top side
    if (DrawTop) {
       if (left<rect.left)   x1=rect.left;
       else                   x1=left;
       if (right>rect.right) x2=rect.right;
       else                   x2=right;

       y1=y2=rect.top+DeltaY1;
       DrawShadowLine(w,hDC,x1,y1,x2,y2,hPen,NULL);
       if (HtmlMode && !DrawBot) DrawShadowLine(w,hDC,x1,y1,x2,y2,GetStockObject(WHITE_PEN),NULL);
    }

    // draw bottom side
    if (DrawBot) {
       if (left<rect.left)   x1=rect.left;
       else                   x1=left;
       
       if (right>rect.right) x2=rect.right;
       else                   x2=right;

       y1=y2=rect.bottom-DeltaY2;
       if (True(TextBorder&BOX_BETWEEN)) y1=y2=y2-2*thick;  // 9/14/2005 bring it within the text area
       if (x2>x1) DrawShadowLine(w,hDC,x1,y1,x2,y2,hPen,NULL);
    }

    // draw left side
    if (DrawLeft) {
       x1=x2=left;
       y1=rect.top;
       if (DrawTop) y1+=DeltaY1;
       y2=rect.bottom;
       if (DrawBot) y2-=DeltaY2;
       DrawShadowLine(w,hDC,x1,y1,x2,y2,hPen,NULL);
    }

    // draw rightt side
    if (DrawRight) {
       x1=x2=right;
       y1=rect.top;
       if (DrawTop) y1+=DeltaY1;
       y2=rect.bottom;
       if (DrawBot) y2-=DeltaY2;
       DrawShadowLine(w,hDC,x1,y1,x2,y2,hPen,NULL);
    }

    // delete the pen
    DeleteObject(hPen);

    // draw inside box for a double border
    if (TextBorder&BOX_DOUBLE && outer) DrawParaBorder(w,hDC,TextRect,FALSE);

    return TRUE;
}


/******************************************************************************
     PaintBkPict:
     Paint the background picture
******************************************************************************/
PaintBkPict(PTERWND w, HDC hDC)
{
    RECT PaintRect;
    HDC  hMetaDC=0,hDestDC=hDC;
    HBITMAP hMetaBM,hOldMetaBM;

    dm("PaintBkPict");

    if (BkPictId==0) return TRUE;

    if (TerFont[BkPictId].PictType==PICT_METAFILE) {

       // create a metafile device context
       if (hDC==hTerDC || hDC==hBufDC) { // use intermediate memory device context
          if (NULL==(hMetaDC=CreateCompatibleDC(hTerDC))) return FALSE;
          if (NULL==(hMetaBM=CreateCompatibleBitmap(hTerDC,TerWinWidth,TerWinHeight))) return FALSE;
          hOldMetaBM=SelectObject(hMetaDC,hMetaBM);
          hDestDC=hMetaDC;         // destination is this temporary DC
       }

       PaintRect.left=PaintRect.top=0;
       PaintRect.right=PaintRect.left+TerWinWidth;
       PaintRect.bottom=PaintRect.top+TerWinHeight;

       SetBkColor(hDestDC,PageColor(w));
       ExtTextOut(hDestDC,0,0,ETO_OPAQUE,&PaintRect,NULL,0,NULL);
    }

    // display the picture background
    if (hDestDC==hTerDC || hDestDC==hBufDC) {    // write to window device context
       if (CaretEnabled && !CaretHidden) HideCaret(hTerWnd);// hide caret before taking a snapshot
       BkPictOut(w,hDestDC,TerWinOrgX,TerWinOrgY);
       if (CaretEnabled && !CaretHidden) ShowCaret(hTerWnd);
    }
    else {                     // write to memory
       RECT SaveTerWinRect=TerWinRect;  // save the original window variables
       int  SaveTerWinOrgX=TerWinOrgX;
       int  SaveTerWinOrgY=TerWinOrgY;
       TerWinRect=PaintRect;
       TerWinOrgX=TerWinOrgY=0;

       BkPictOut(w,hDestDC,0,0);  // display the picture

       TerWinRect=SaveTerWinRect;      // restore the original variables
       TerWinOrgX=SaveTerWinOrgX;
       TerWinOrgY=SaveTerWinOrgY;

       // bitblt meta dc to hDC
       if (hMetaDC && (hDC==hTerDC || hDC==hBufDC)) {
          if (CaretEnabled && !CaretHidden) HideCaret(hTerWnd);// hide caret before taking a snapshot
          BitBlt(hDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight,hMetaDC,0,0,SRCCOPY);
          if (CaretEnabled && !CaretHidden) ShowCaret(hTerWnd);
       }
    }

    // relase temporary DC resources
    if (hMetaDC) {
       // free metafile related resources
       SelectObject(hMetaDC,hOldMetaBM);
       DeleteObject(hMetaBM);
       DeleteDC(hMetaDC);
    }

    dm("PaintBkPict - End");

    return TRUE;
}

/******************************************************************************
   BkPictOut:
   Display the background picture to the windows DC or memory DC.
*******************************************************************************/
BkPictOut(PTERWND w,HDC hDC,int x,int y)
{
   int width=TerFont[BkPictId].CharWidth[PICT_CHAR];
   int height=TerFont[BkPictId].height;

   dm("BkPictOut");

   if (BkPictFlag==BKPICT_STRETCH) PictOut(w,hDC,x,y,BkPictId,1);
   else if (BkPictFlag==BKPICT_TILE) {
      int EndX=x+TerWinWidth;
      int EndY=y+TerWinHeight;
      int x1,y1;
      DWORD NoPal=TerFlags&TFLAG_NO_PALETTE;

      TerSetClipRgn(w);

      // draw horizontally
      y1=y;
      while(y1<EndY) {
         x1=x;
         while (x1<EndX) {
            PictOut(w,hDC,x1,y1,BkPictId,1);
            x1+=width;
            TerFlags|=TFLAG_NO_PALETTE;  // do not set palette again
         }
         y1+=height;
      }

      if (!NoPal) TerSetFlags(hTerWnd,FALSE,TFLAG_NO_PALETTE);

      TerResetClipRgn(w);
   }
   else {
      PictOut(w,hDC,x,y,BkPictId,1);

      if (hDC==hTerDC || hDC==hBufDC) {  // clear any remaining spaces
         RECT rect;

         if (width<TerWinWidth) {     // clear the space from the right of the picture
            rect.left=x+width;
            rect.right=x+TerWinWidth;
            rect.top=y;
            rect.bottom=y+height;
            SetBkColor(hDC,PageColor(w));
            ExtTextOut(hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
         }
         if (height<TerWinHeight) {   // clear the space from the bottom of the picture
            rect.left=x;
            rect.right=x+TerWinWidth;
            rect.top=y+height;
            rect.bottom=y+TerWinHeight;
            SetBkColor(hDC,PageColor(w));
            ExtTextOut(hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
         }
      }
   }

   return TRUE;
}


/******************************************************************************
    SyncHilight:
    Synchronize the higliht end position with the current position.
*******************************************************************************/
SyncHilight(PTERWND w)
{
    HilightEndRow=CurLine;
    HilightEndCol=CurCol;
    if (HilightEndRow>HilightBegRow || (HilightEndRow==HilightBegRow && HilightEndCol>HilightBegCol)) {
       if (TerArg.WordWrap && !HilightAtCurPos && HilightEndCol==0 && HilightEndRow>0 && LineEndsInBreak(w,HilightEndRow-1)) {
          HilightEndRow--;
          HilightEndCol=LineLen(HilightEndRow);
          if (LineInfo(w,HilightEndRow,INFO_ROW) && HilightEndRow>0) {
             HilightEndRow--;
             HilightEndCol=LineLen(HilightEndRow);
          }
       }
    }

return TRUE;
}

/******************************************************************************
    TerSetHilight:
    Set highlighted areas using mouse.  The arguments wParam and lParam are
    provided byWindows through mouse messages.
*******************************************************************************/
TerSetHilight(PTERWND w,WPARAM wParam,DWORD lParam, BOOL scrolling)
{
    int DeltaX,DeltaY,SaveMouseCol,NewEndCol,SaveEndCell=0;
    long FirstLine,LastLine,SaveMouseLine;
    MSG msg;
    BOOL ScreenScrolled=FALSE,SaveStretch,SaveFrameRefreshEnabled;
    BOOL SaveXBeyondLine;
    BOOL moved=FALSE;
    BOOL AllowMouseSel=!(TerFlags3&TFLAG3_NO_MOUSE_SEL);
    BOOL NewHilighting=(HilightType==HILIGHT_OFF);

    if (!AllowMouseSel) return TRUE;   // mouse hightlighting disabled

    //******* save the position where the mouse was clicked originally ***
    SaveMouseLine=MouseLine;
    SaveMouseCol=MouseCol;
    SaveXBeyondLine=(TerOpFlags&TOFLAG_X_BEYOND_LINE)?TRUE:FALSE;
    
    // stop any previous hilight timer
    KillHilightTimer(w);

    if (scrolling) ScreenScrolled=TRUE;
    else {                             // beginning the hilight operation
       // check if the movement is large enough to warrant hilighting
       DeltaX=abs(((int)(short)LOWORD(lParam))-MouseX);               // calculate the recent mouse movement
       DeltaY=abs(((int)(short)HIWORD(lParam))-MouseY);

       moved=(DeltaX>TerTextMet.tmAveCharWidth/4
           || DeltaY>(TerTextMet.tmHeight+TerTextMet.tmExternalLeading)/8);
       if (VerySmallMovement && moved) VerySmallMovement=FALSE;                        // very small movement?

       TerMousePos(w,lParam,TRUE);                       // get mouse position in text co-ordinates
       
       // check if in table highligting
       if (TblSelCursShowing && MouseOverShoot==' ' && HilightType==HILIGHT_CHAR
           && InSameTable(w,cid(HilightBegRow),cid(MouseLine))) return HilightTableCol(w,MouseLine,FALSE,TRUE);
    }


    if (moved) FrameClicked=FALSE;         // remove any frame sizing bars

    // PROCESS MOUSE MESSAGE
    if (wParam&(MK_LBUTTON|MK_RBUTTON|MK_MBUTTON) || scrolling) { // check if the screen needs to scroll
        if (MouseOverShoot!=' ')  {                     // update the text line/col where the mouse is placed now
            PaintEnabled=FALSE;                         // suspend painting for a while
            ScrollText(w);                              // scroll the text
            PaintEnabled=TRUE;
            MouseLine=CurLine;
            if (!(LineFlags2(CurLine)&LFLAG2_RTL)) MouseCol=CurCol;
            ScreenScrolled=TRUE;
        }
    }

    if (HilightType!=HILIGHT_OFF && !StretchHilight// turn-off previous hilight if mouse pressed again
        && wParam&(MK_LBUTTON|MK_RBUTTON|MK_MBUTTON)) {
        HilightType=HILIGHT_OFF;
        WrapFlag=WRAP_OFF;
        PaintTer(w);
    }

    if (HilightType==HILIGHT_OFF) {                    // begin a new block
       if (wParam&MK_LBUTTON)      HilightType=HILIGHT_CHAR;
       else if (wParam&MK_RBUTTON) HilightType=HILIGHT_LINE;
       if (HilightType!=HILIGHT_OFF) {
           HilightBegRow=SaveMouseLine;
           HilightBegCol=SaveMouseCol;
           HilightEndRow=HilightEndCol=-1;
           StretchHilight=TRUE;                        // highlight may be stretched now
           HilightWithColCursor=FALSE;
           SetCapture(hTerWnd);                        // capture the mouse
       }
       VerySmallMovement=TRUE;                         // records extent of mouse movement
    }

    // calculate the highlight end column
    NewEndCol=MouseCol;                                // new hilight end column
    if (!ScreenScrolled && MouseLine<TotalLines-1 && MouseCol<LineLen(MouseLine)) {
       if (HilightEndRow>HilightBegRow || (HilightEndRow==HilightBegRow && HilightEndCol>=HilightBegCol)) {
          NewEndCol++;
          if (TerArg.WordWrap && NewEndCol>=LineLen(MouseLine)) NewEndCol--;
       }
    }
    if (!ScreenScrolled && NewEndCol==(LineLen(MouseLine)-1) && TerOpFlags&TOFLAG_X_BEYOND_LINE  
          && (!NewHilighting || !SaveXBeyondLine)) NewEndCol++;

    if (DblClickHilight && MouseLine!=DblClickEndRow) DblClickHilight=FALSE;
    if (DblClickHilight && NewEndCol>=HilightBegCol && NewEndCol<DblClickEndCol) NewEndCol=DblClickEndCol;

    if (StretchHilight && (HilightEndRow!=MouseLine || HilightEndCol!=NewEndCol)) { // stretch the highlighted area
       if (HilightEndRow>=0 && HilightEndRow<TotalLines) SaveEndCell=cid(HilightEndRow);

       HilightEndRow=MouseLine;
       HilightEndCol=NewEndCol;
       
       if (HilightBegRow>=TotalLines) {
          HilightBegRow=TotalLines-1;
          HilightBegCol=0;
       } 
       if (HilightEndRow>=TotalLines) {
          HilightEndRow=TotalLines-1;
          HilightEndCol=LineLen(HilightEndRow);
       } 
       
       if (ScreenScrolled) {
           CurLine=MouseLine;
           CurCol=NewEndCol;
           CurRow=CurLine-BeginLine;
           PaintTer(w);
       }
       else {
           FirstLine=CurLine;     // range of lines to repaint
           LastLine=MouseLine;
           if (CurLine>MouseLine) {
              FirstLine=MouseLine;
              LastLine=CurLine;
           }
           SaveStretch=StretchHilight;
           StretchHilight=FALSE;       // to suppress automatic hilight calc in PaintTer
           SaveFrameRefreshEnabled=FrameRefreshEnabled;
           FrameRefreshEnabled=FALSE;
           if (TerArg.PageMode) {
              CurLine=MouseLine;
              if (CurCol>=LineLen(MouseLine) && LineLen(MouseLine)>0) CurCol=LineLen(MouseLine)-1;

              PaintBegLine=FirstLine-1;
              PaintEndLine=LastLine+1;
              PaintFlag=PAINT_RANGE;
              if (TerFlags2&TFLAG2_XPARENT_WRITE) PaintFlag=PAINT_WIN;

              if (FirstLine==LastLine) PaintFlag=PAINT_LINE;

              if (cid(HilightBegRow)!=cid(HilightEndRow)) PaintFlag=PAINT_WIN;
              else if (cid(HilightEndRow)!=SaveEndCell)   PaintFlag=PAINT_WIN;
              else if (LineFlags(HilightBegRow)&LFLAG_PICT) PaintFlag=PAINT_WIN;
              else if (HasOverlayingFrames)               PaintFlag=PAINT_WIN;
              else if (LineInfo(w,HilightBegRow,INFO_FRAME)) PaintFlag=PAINT_WIN;
              else if (LineTextAngle(w,HilightBegRow)>0)  PaintFlag=PAINT_WIN;

              WrapFlag=WRAP_OFF;       // no need for wrap operation
              PaintTer(w);
           }
           else {
              CurLine=MouseLine;
              CurRow=CurLine-BeginLine;
              PaintFlag=PAINT_WIN;
              WrapFlag=WRAP_OFF;              // no need for wrap operation
              PaintTer(w);
           }
           StretchHilight=SaveStretch;
           FrameRefreshEnabled=SaveFrameRefreshEnabled;

           CurLine=MouseLine;
           CurCol=NewEndCol;
           if ((CurLine+1)==TotalLines && CurCol>=LineLen(CurLine) && CurCol>0) CurCol--;  // don't position after the last line otherwise wordrapping will create another line
           CurRow=CurLine-BeginLine;
           OurSetCaretPos(w);                 // no need to repaint again
       }
    }

    // Check if the screen should keep scrolling
    if (ScreenScrolled) {                              // check if the screen should keep scrolling
       if (!PeekMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST,PM_NOREMOVE|PM_NOYIELD)) {
          if (!HilightTimerOn) HilightTimerOn=SetTimer(hTerWnd,TIMER_HILIGHT,40,NULL);
       }
    }



    return TRUE;
}

/******************************************************************************
    KillHilightTimer:
    Kill the hilight timer.
*******************************************************************************/
KillHilightTimer(PTERWND w)
{
   if (HilightTimerOn) {
      KillTimer(hTerWnd,TIMER_HILIGHT);
      HilightTimerOn=FALSE;
   }

   return TRUE;
}

/******************************************************************************
    HilightTableCol:
    Hilight the current table column and allow the highlighting to stretch
*******************************************************************************/
HilightTableCol(PTERWND w,long LineNo,BOOL new, BOOL repaint)
{
    int cell1,cell2,col,row1,row2,SaveBegCol,SaveEndCol,SaveType;
    long line,SaveBegRow,SaveEndRow;
    int tol=20;

    if (LineNo<0 || LineNo>=TotalLines || cid(LineNo)==0) return TRUE;

    // save the current highlight info
    SaveType=HilightType;
    SaveBegRow=HilightBegRow;
    SaveEndRow=HilightEndRow;
    SaveBegCol=HilightBegCol;
    SaveEndCol=HilightEndCol;

    cell1=cid(LineNo);

    // get the beginning row of the table
    row1=row2=cell[cell1].row;
    while (!IsFirstTableRow(w,row1)) row1=TableRow[row1].PrevRow;

    col=GetCellColumn(w,cell1,TRUE);
    if (cell[cell1].row!=row1) cell1=GetColumnCell(w,row1,col,TRUE);
    
    // get the ending row for the selection
    while (!IsLastTableRow(w,row2)) row2=TableRow[row2].NextRow;
    cell2=GetColumnCell(w,row2,col,TRUE);
    
    if (abs(cell[cell2].x-cell[cell1].x)>tol) {   // scan backward to a row where the cell begins at the same x
       while ((row2=TableRow[row2].PrevRow)>0) {
          cell2=GetColumnCell(w,row2,col,TRUE);
          if (abs(cell[cell2].x-cell[cell1].x)<tol) break;
       }
       if (row2<=0) {
          row2=row1;
          cell2=cell1;
       }  
    } 

    if (cell[cell1].ColSpan>1) {    // span more cells on the end
       int span=cell[cell1].ColSpan;
       while (span>1 && cell[cell2].NextCell>0) {
          span-=cell[cell2].ColSpan;
          if (span<=0) break;
          if (abs((cell[cell1].x+cell[cell1].width)-(cell[cell2].x+cell[cell2].width))<tol) {
             cell[cell1].ColSpan-=span;  // fix the colspan for the first cell
             if (cell[cell1].ColSpan<1) cell[cell1].ColSpan=1;
             break;
          }
          cell2=cell[cell2].NextCell;
       }
    }

    // set the highlighting beginning position if not already set
    if (HilightType!=HILIGHT_CHAR || new) {
       line=LineNo;
       while (line>=0 && cid(line)!=cell1) line--;  // position at the first cell of the column
       while (line>=0 && cid(line)==cell1) line--;  // position at the first line of the column
       HilightBegRow=line+1;
       HilightBegCol=0;
       HilightType=HILIGHT_CHAR;
    }

    // set the hilight end position
    line=LineNo;
    while (line<TotalLines && cid(line)>0 && cid(line)!=cell2) line++;  // find the last cell
    if (line<TotalLines) while(line<TotalLines && cid(line)==cell2) line++;  // go past the current cell
    line--;              // go back to the current cell
    if (line>=TotalLines) line--;
    if (line<0) line=0;
    if (tabw(line) && tabw(line)->type&INFO_ROW) line--;
    HilightEndRow=line;
    HilightEndCol=LineLen(line)-1;
    if (HilightBegRow==HilightEndRow && HilightBegCol==HilightEndCol) HilightEndCol++;
    if (HilightEndCol<0) HilightEndCol=0;

    StretchHilight=TRUE;                // stretch the highlight
    HilightWithColCursor=TblSelCursShowing;  // record that this hilighting is done using the column cursor

    if (repaint && (HilightType!=SaveType || HilightBegRow!=SaveBegRow || HilightBegCol!=SaveBegCol
      || HilightEndRow!=SaveEndRow || HilightEndCol!=SaveEndCol)) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerSetLineHilight:
    Set highlighted areas for a line block.
    The command key stroke toggles the highlight on or off.
*******************************************************************************/
TerSetLineHilight(PTERWND w)
{

    if (HilightType!=HILIGHT_OFF) {
       if (StretchHilight) StretchHilight=FALSE;
       else {
          HilightType=HILIGHT_OFF;
          PaintTer(w);
          return TRUE;
       }
    }
    if (HilightType==HILIGHT_OFF) {       // begin a new block
       HilightType=HILIGHT_LINE;
       HilightBegRow=HilightEndRow=CurLine;
       HilightBegCol=HilightEndCol=CurCol;
       StretchHilight=TRUE;               // highlight may be stretched now
       PaintTer(w);
    }

    return TRUE;
}

/******************************************************************************
    TerSetCharHilight:
    Set highlighted areas for a character block.  This routine examines the
    status of the SHIFT key to determine beginning or end of a block.
*******************************************************************************/
TerSetCharHilight(PTERWND w)
{
    BOOL ShiftPressed;

    KillHilightTimer(w);    // kill any hilight timer

    if (PictureClicked) HilightType=HILIGHT_OFF;
    
    if (TerFlags5&TFLAG5_NO_KB_SEL) return TRUE;  // selection using keyboard not allowed

    ShiftPressed=GetKeyState(VK_SHIFT)&0x8000;

    if (ShiftPressed) {
       if (HilightType!=HILIGHT_CHAR) { // begin highlighting
          HilightType=HILIGHT_CHAR;
          HilightBegRow=HilightEndRow=CurLine;
          HilightBegCol=HilightEndCol=CurCol;
       }
       StretchHilight=TRUE;             // stretch highlight
    }
    else if (HilightType==HILIGHT_CHAR) {
       HilightType=HILIGHT_OFF;         // turn off highlighting
       PaintTer(w);
    }

    // set/reset other flags
    InputFontId=-1;                     // reset the input font
    PictureClicked=FrameClicked=FALSE;
    HilightWithColCursor=FALSE;

    return TRUE;
}

/******************************************************************************
     TextSegmentExists:
     This routine checks if a text segment is already drawn in the window.
     If not it creates a new segments, and invalidates any intersecting
     segments.
******************************************************************************/
BOOL TextSegmentExists(PTERWND w,int x,int y,RECT far *rect,int len,LPBYTE string)
{
    //int  i,j,NewSeg=-1,left,right,top,bottom,fmt;
    //COLORREF ForeColor,BackColor;
    //LPBYTE ptr;
    //BOOL ControlsEmbedded=FALSE;
    //HBITMAP hOldBmp;

    if (hScrollBM) return FALSE;

    if ((TerFlags2&TFLAG2_XPARENT_WRITE) && (TerOpFlags&TOFLAG_WRITING_BKND) && transparent) {  // write the background for the entire line
       if (hBkPictBM) DrawBkPictRect(w,rect,hBufDC);  // refresh the screen rectangle
       else {
         COLORREF OldBkColor=SetBkColor(hBufDC,PageColor(w));
         ExtTextOut(hBufDC,0,0,ETO_OPAQUE,rect,NULL,0,NULL);
         SetBkColor(hBufDC,OldBkColor);
       }
    }
    else if (hBkPictBM) DrawBkPictRect(w,rect,hBufDC);  // refresh the screen rectangle

    if (DirtyRect.left==-1) DirtyRect=(*rect);     // record the screen rectangle to be updated
    else {                                         // expand the dirty rectangle
       if (rect->left<DirtyRect.left) DirtyRect.left=rect->left;
       if (rect->top<DirtyRect.top) DirtyRect.top=rect->top;
       if (rect->right>DirtyRect.right) DirtyRect.right=rect->right;
       if (rect->bottom>DirtyRect.bottom) DirtyRect.bottom=rect->bottom;
    }
    return FALSE;
}

/******************************************************************************
     DrawBkPictRect:
     Draw the background picture in a rectangle
******************************************************************************/
BOOL DrawBkPictRect(PTERWND w, LPRECT rect, HDC hDC)
{
    if (CaretEnabled && !CaretHidden) HideCaret(hTerWnd);// hide caret before taking a snapshot
    if (hBkPictBM) {
       BOOL CheckRtl=(ScrRtl && pScrSeg && CurScrSeg<TotalScrSeg);
       RECT NewRect=(*rect);
       HBITMAP hOldBmp;

       // rtl translation
       if (CheckRtl) RtlRect(w,&NewRect,CurFrame,&(pScrSeg[CurScrSeg]));
       
       hOldBmp=SelectObject(hMemDC,hBkPictBM);
       BitBlt(hDC,NewRect.left,NewRect.top,NewRect.right-NewRect.left,NewRect.bottom-NewRect.top,hMemDC,NewRect.left-TerWinOrgX,NewRect.top-TerWinOrgY,SRCCOPY);
       SelectObject(hMemDC,hOldBmp);         // deselect the bitmap
    }
    if (CaretEnabled && !CaretHidden) ShowCaret(hTerWnd);

    return TRUE;
}

/******************************************************************************
     DeleteTextMap:
     Delete the entire text segment map.
******************************************************************************/
DeleteTextMap(PTERWND w, BOOL RedrawBackground)
{
    int i;

    dm("DeleteTextMap");

    for (i=0;i<TotalSegments;i++) DeleteTextSeg(w,i,FALSE);
    TotalSegments=0;

    // set the background picture
    if (RedrawBackground && BkPictId>0) {
       if (hBkPictBM) {            // quick draw
          if (hBufDC==NULL) {      // when hBufDC is active, the background is painted in segments
             HBITMAP hOldBmp=SelectObject(hMemDC,hBkPictBM);
             if (CaretEnabled && !CaretHidden) HideCaret(hTerWnd);// hide caret before taking a snapshot
             BitBlt(hTerDC,TerWinOrgX,TerWinOrgY,TerWinWidth,TerWinHeight,hMemDC,0,0,SRCCOPY);
             if (CaretEnabled && !CaretHidden) ShowCaret(hTerWnd);
             SelectObject(hMemDC,hOldBmp);         // deselect the bitmap
          }
       }
       else {
          HDC hDC=hBufDC?hBufDC:hTerDC;

          PaintBkPict(w,hDC);   // draw from the picture buffer
          if (NULL!=(hBkPictBM=CreateCompatibleBitmap(hTerDC,TerWinWidth,TerWinHeight))) {
             HBITMAP hOldBmp=SelectObject(hMemDC,hBkPictBM);
             if (CaretEnabled && !CaretHidden) HideCaret(hTerWnd);// hide caret before taking a snapshot
             BitBlt(hMemDC,0,0,TerWinWidth,TerWinHeight,hDC,TerWinOrgX,TerWinOrgY,SRCCOPY);
             if (CaretEnabled && !CaretHidden) ShowCaret(hTerWnd);
             SelectObject(hMemDC,hOldBmp);         // deselect the bitmap
          }
       }
    }

    PaintFlag=PAINT_WIN;
    
    dm("DeleteTextMap- End");

    return TRUE;
}

/******************************************************************************
     DeleteTextSeg:
     Delete a text segment.
******************************************************************************/
DeleteTextSeg(PTERWND w,int idx,BOOL RestoreBitmap)
{
    int fmt;
    HBITMAP hOldBmp=NULL;

    if (idx<0 || idx>=TotalSegments) return TRUE;
    if (!(TextSeg[idx].drawn)) return TRUE;

    if (RestoreBitmap) {
       int left=TextSeg[idx].rect.left;
       int top=TextSeg[idx].rect.top;
       int width=TextSeg[idx].rect.right-left;
       int height=TextSeg[idx].rect.bottom-top;
       if (BkPictId && hBkPictBM) {
          hOldBmp=SelectObject(hMemDC,hBkPictBM);
          BitBlt(hTerDC,left,top,width,height,hMemDC,left-TerWinOrgX,top-TerWinOrgY,SRCCOPY);
       }
       else if (TextSeg[idx].hBmp) {
          hOldBmp=SelectObject(hMemDC,TextSeg[idx].hBmp);
          BitBlt(hTerDC,left,top,width,height,hMemDC,0,0,SRCCOPY);
       }
       if (hOldBmp) SelectObject(hMemDC,hOldBmp);         // deselect the bitmap
    }

    if (TextSeg[idx].hString) {         // free up string memory
       GlobalUnlock(TextSeg[idx].hString);
       GlobalFree(TextSeg[idx].hString);
    }

    // hide the control window
    fmt=TextSeg[idx].fmt;
    if ( IsControl(w,fmt)
      && TerFont[fmt].hWnd) {
      ShowWindow(TerFont[fmt].hWnd,SW_HIDE);
      ValidateRect(hTerWnd,NULL);      // force no repainting
      TerFont[fmt].flags=ResetUintFlag(TerFont[fmt].flags,(FFLAG_CTL_DRAWN|FFLAG_CTL_VISIBLE));
      // record the border spills
      if (TextSeg[idx].rect.top<TerWinOrgY) BorderSpill|=SPILL_TOP;
      if (TextSeg[idx].rect.bottom>(TerWinOrgY+TerWinHeight)) BorderSpill|=SPILL_BOT;
      if (TextSeg[idx].rect.left<TerWinOrgX) BorderSpill|=SPILL_LEFT;
      if (TextSeg[idx].rect.right>(TerWinOrgX+TerWinWidth)) BorderSpill|=SPILL_LEFT;
    }

    if (TextSeg[idx].hBmp) DeleteObject(TextSeg[idx].hBmp);  // delete the background bitmap

    TextSeg[idx].drawn=FALSE;

    return TRUE;
}

/******************************************************************************
    DrawBullet:
    Draw the bullet character.
*******************************************************************************/
int DrawBullet(PTERWND w,HDC hPropDC,HDC hDrawDC,long LineNo,int CurPara, int x, int y, int BaseHeight, BOOL draw)
{
    BYTE string[100];
    int  i,BltId,PixLen=0,len,CurBltId,TwipsSize,PrevBkMode;
    long l,nbr,FirstBltLine;
    WORD font=0;       // font to draw the bullet character
    BYTE typeface[32];
    int sty=PfmtId[CurPara].StyId;
    BYTE StyName[100],heading[]="HEADING";
    BOOL IsHeading,LangRtl=0,IsBullet;
    UINT option;
    BOOL IsRoman=FALSE;
    int  ParaFont=-1;

    dm("DrawBullet");


    DlgText1[0]=0;  // this variable is used to pass the bullet string when the 'draw' parameter is FALSE

    // check if the entire line is hidden
    if (IsHiddenLine(w,LineNo) || LineInfo(w,LineNo,INFO_ROW)) return TRUE;

    // check if any header style used
    lstrcpy(StyName,StyleId[sty].name);
    _strupr(StyName);
    IsHeading=(BOOL)(strstr(StyName,heading)==StyName);   // belong to the heading style


    BltId=PfmtId[CurPara].BltId;

    // set lang inf
    if ((LangRtl=IsDefLangRtl(w))) option=ETO_RTLREADING;
    else option=0;


    // get the font used by the para character
    if (FALSE && HtmlMode) font=GetCurCfmt(w,LineNo,0);
    else {
       for (l=LineNo;l<(TotalLines-1);l++) if (LineFlags(l)&LFLAG_PARA) break;  // find the line with the paramarker
       if (LineLen(l)==0) font=0;
       else {
          font=ParaFont=GetCurCfmt(w,l,LineLen(l)-1);
          if (TerFont[font].style&(ULINE|ULINED)) {// turn-off underline
             UINT style=TerFont[font].style;
             style=ResetUintFlag(style,(ULINE|ULINED));
             font=GetNewFont(w,hPropDC,font,TerFont[font].TypeFace,TerFont[font].TwipsSize,style,TerFont[font].TextColor,CLR_WHITE,TerFont[font].UlineColor,0,0,0,1,PfmtId[CurPara].StyId,0,0,0,NULL,0,0,DEFAULT_CHARSET,0,TerFont[font].TextAngle);
          }
       }    
    }

    // check if a list is actually a bullet
    IsBullet=TerBlt[BltId].IsBullet;
    if (!IsBullet && (LineFlags(LineNo)&LFLAG_LIST)) {
       struct StrListLevel far *pLevel;
       if ((pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,TerBlt[BltId].lvl))!=NULL) {  // get the level pointer
          if (pLevel->NumType==LIST_BLT) IsBullet=TRUE;   // treat it as bullet because we force it to 0xb7
          IsRoman=(pLevel->NumType==LIST_UPR_ROMAN || pLevel->NumType==LIST_LWR_ROMAN);
          if (IsRoman && pLevel->CharAft!=LISTAFT_TAB) IsRoman=FALSE;  // no space available to right-justify
          if (IsRoman && pLevel->text!=null && strlen(pLevel->text)>0) {
             for (i=0;i<(int)pLevel->text[0];i++) {
                int lvl=(int)pLevel->text[i+1];
                if (lvl>=9)  continue;  // not a level, but some text
                if (lvl!=TerBlt[BltId].lvl) {       // do not right-justify complex numbering
                   IsRoman=false;
                   break;
                } 
             } 
          }
       }
    } 

    // retrieve the index of the symbol font, create one if not already exists
    if (IsBullet) {
       struct StrListLevel far *pLevel;
       BOOL UseSymbolFont=FALSE;
       BOOL CreateFont=TRUE;
       if ((pLevel=GetListLevelPtr(w,TerBlt[BltId].ls,TerBlt[BltId].lvl))!=NULL) {  // get the level pointer
          if (TRUE || pLevel->NumType!=LIST_BLT) {
             font=pLevel->FontId;
             if (TerFont[font].TextColor==CLR_AUTO && ParaFont>=0 && TerFont[ParaFont].TextColor!=CLR_AUTO) {  // get the color from the paragraph marker
                font=(int)GetNewColor(w,(WORD)font,TerFont[ParaFont].TextColor,0L,LineNo,0);
             } 
             CreateFont=FALSE;            // font found
          }
          else if (TerBlt[BltId].font!=BFONT_SYMBOL && TerBlt[BltId].font!=BFONT_WINGDINGS) UseSymbolFont=TRUE;       // we tream LIST_BLT as regular bullet
       }
       if (CreateFont) {
          if      (TerBlt[BltId].font==BFONT_SYMBOL || UseSymbolFont) lstrcpy(typeface,"Symbol");
          else if (TerBlt[BltId].font==BFONT_WINGDINGS) lstrcpy(typeface,"Wingdings");
          else                                          lstrcpy(typeface,TerFont[0].TypeFace);
          TwipsSize=TerFont[font].TwipsSize;
          if (TerBlt[BltId].BulletChar==0xa8) {         // BLT_DIAMOND
             if (InPrinting) TwipsSize=TwipsSize*4/5;
             else            TwipsSize=TwipsSize*3/4;
          }
          font=GetNewFont(w,hPropDC,font,typeface,TwipsSize,0,TerFont[font].TextColor,CLR_WHITE,TerFont[font].UlineColor,0,0,0,1,PfmtId[CurPara].StyId,0,0,0,NULL,0,0,DEFAULT_CHARSET,0,TerFont[font].TextAngle);
       }
    }
    else if (LineFlags(LineNo)&LFLAG_LIST) {
       if (tabw(LineNo)) font=tabw(LineNo)->ListFontId;
    } 
    else {
       if ((HtmlMode && TerFont[font].style) || TerFont[font].FieldId>0)   // create a font without styles
          font=GetNewFont(w,hPropDC,font,TerFont[font].TypeFace,TerFont[font].TwipsSize,0,TerFont[font].TextColor,CLR_WHITE,TerFont[font].UlineColor,0,0,0,1,PfmtId[CurPara].StyId,0,0,0,NULL,0,0,DEFAULT_CHARSET,0,TerFont[font].TextAngle);
       if (LangRtl && !TerFont[font].rtl) font=SetCurLangFont2(w,font,(HKL)DefLang);
    }

    SetFont(w,hDrawDC,font,'T');  // set font

    if (TerFont[font].BaseHeight>0)   // zero value might correspond to the hidden paramarker at the end of the line.
       y=y+BaseHeight-TerFont[font].BaseHeight+TerFont[font].BaseHeightAdj;

    if (TerBlt[BltId].IsBullet) {
       string[0]=TerBlt[BltId].BulletChar;
       string[1]=0;
    }
    else if (TerBlt[BltId].ls>0) {
      if (LineFlags(LineNo)&LFLAG_LIST) {
        string[0]=0;
        if (tabw(LineNo) && tabw(LineNo)->ListText) lstrcpy(string,tabw(LineNo)->ListText);
      }
      else return true;   // perhaps a blank listtext field
    }
    else {
       nbr=0;
       FirstBltLine=LineNo;
       for (l=LineNo-1;l>=0;l--) {
          CurBltId=PfmtId[pfmt(l)].BltId;
          if (LineInfo(w,l,INFO_ROW)) continue;

          if (HtmlMode) {
            if (TRUE || IsHtmlList(w,pfmt(l))) {  // the pasted data does not this flag turned on
               int ListLevel=TerBlt[CurBltId].level;  // HtmlListLevel(w,pfmt(l));   // HtmlListLeve not set for pasted data
               int ParaId=pfmt(l+1);                       // check the beginning of a list
               int PARA_BEGIN_LI=0x2000; 

               //if (PfmtId[ParaId].SpaceBefore>0 && LineFlags(l+1)&LFLAG_PARA_FIRST) break;    // the first items has paragraph space before
               

               if (ListLevel<TerBlt[BltId].level) break;  // reached a parent list
               if (!TerBlt[CurBltId].IsBullet && TerBlt[CurBltId].start!=TerBlt[BltId].start) break;  // skip over child lists
               if (CurBltId==0) continue;                 // subsequent para of a list item - multiple paragraph list item
               if (TerBlt[CurBltId].level>TerBlt[BltId].level) continue;  // skip over child lists
               if (TerBlt[BltId].IsBullet) break;
               if (LineInfo(w,l,INFO_CELL)) break;   // cell contains the list - not other way
               if (LineFlags(l)&LFLAG_PARA_FIRST) nbr++;
            }
            else break;
          }
          else {
            if (CurBltId) {
               if (TerBlt[CurBltId].flags&BLTFLAG_HIDDEN) continue;
               if (TerBlt[CurBltId].IsBullet) continue;  //break;
               if (TerBlt[CurBltId].start!=TerBlt[BltId].start) continue;
               if (LineFlags(l)&LFLAG_BREAK) continue;
               if (TerBlt[CurBltId].level<TerBlt[BltId].level) break;
               if (TerBlt[CurBltId].level==TerBlt[BltId].level) {
                  if (TerBlt[CurBltId].AftChar!=TerBlt[BltId].AftChar) break;
                  if (lstrcmpi(TerBlt[CurBltId].BefText,TerBlt[BltId].BefText)!=0) break;
                  if (TerBlt[CurBltId].NumberType!=TerBlt[BltId].NumberType) break;
                  if (LineFlags(l)&LFLAG_PARA_FIRST) nbr++;
                  FirstBltLine=l;
               }
            }
            else break;
          }
       }

       // add the starting number
       l++;
       CurBltId=PfmtId[pfmt(FirstBltLine)].BltId;
       nbr+=(TerBlt[CurBltId].start);
       string[0]=0;
       if      (TerBlt[BltId].NumberType==NBR_UPR_ALPHA) AlphaFormat(string,(int)nbr,TRUE);
       else if (TerBlt[BltId].NumberType==NBR_LWR_ALPHA) AlphaFormat(string,(int)nbr,FALSE);
       else if (TerBlt[BltId].NumberType==NBR_UPR_ROMAN) romanize(string,(int)nbr,TRUE);
       else if (TerBlt[BltId].NumberType==NBR_LWR_ROMAN) romanize(string,(int)nbr,FALSE);
       else if (TerBlt[BltId].NumberType==NBR_DEC)       wsprintf(string,"%ld",nbr);
       else if (TerBlt[BltId].NumberType==NBR_DEFAULT && !IsHeading) wsprintf(string,"%c",'A'+nbr-1);

       IsRoman=(TerBlt[BltId].NumberType==NBR_UPR_ROMAN || TerBlt[BltId].NumberType==NBR_LWR_ROMAN);

       if (!HtmlMode) {       // add before and after symbols
          if (lstrlen(TerBlt[CurBltId].BefText)>0) {
             BYTE string1[20];
             lstrcpy(string1,string);
             lstrcpy(string,TerBlt[CurBltId].BefText);
             lstrcat(string,string1);
          }
          if (TerBlt[CurBltId].AftChar) {
             int len=lstrlen(string);
             string[len]=TerBlt[CurBltId].AftChar;
             string[len+1]=0;
          }
       }
       if (HtmlMode || (lstrlen(TerBlt[CurBltId].BefText)==0 && TerBlt[CurBltId].AftChar==0)) {
          int len=lstrlen(string);   // append a period
          string[len]='.';
          string[len+1]=0;
       }
    }
    if (LineInfo(w,LineNo,(INFO_PAGE|INFO_COL)) && LineLen(LineNo)==1) string[0]=0;  // no bullets on page/column break lines


    // check if the bullet will fit
    len=lstrlen(string);
    for (i=0;i<len;i++) PixLen+=DblCharWidth(w,font,TRUE,string[i],0);
    if (!(TerBlt[BltId].flags&BLTFLAG_HIDDEN)) {
        
        if (!draw) {       // simply pass the bullet string
           strcpy(DlgText1,string);
           DlgInt1=font;
           return TRUE;
        } 
        
        y+=ScrYOffset;
        
        if (InPrinting) PrevBkMode=SetBkMode(hDrawDC,TRANSPARENT);  // set transparency for printing, screen displays sets its own transparency
       
        if (IsRoman) {               // right-justify
           SIZE size;
           int  PixLenTwips;
           GetTextExtentPoint(hDrawDC,string,lstrlen(string),&size);
           if (InPrinting) PixLenTwips=PrtToTwipsX(size.cx);
           else            PixLenTwips=ScrToTwipsX(size.cx);
           if ((PfmtId[CurPara].LeftIndentTwips+PfmtId[CurPara].FirstIndentTwips)<PixLenTwips) {   // right justify if enough space available
              PixLenTwips=PfmtId[CurPara].LeftIndentTwips+PfmtId[CurPara].FirstIndentTwips;
              if (InPrinting) size.cx=TwipsToPrtX(PixLenTwips);
              else            size.cx=TwipsToScrX(PixLenTwips);
           } 
           x-=size.cx;
           if (x<0) x=0;
        }  
        else if (!(LineFlags(LineNo)&LFLAG_LIST) && PixLen>abs(PfmtId[CurPara].FirstIndent)) {
           x-=(PixLen-abs(PfmtId[CurPara].FirstIndent));
           if (x<0) x=0;
        } 

        if (ScrRtl && pScrSeg) {     // adjust x
           SIZE size;
           GetTextExtentPoint(hDrawDC,string,lstrlen(string),&size);
           x=RtlX(w,x,size.cx,CurFrame,NULL);
        } 
        
        //TextOut(hDrawDC,x,y,string,len);

        ExtTextOut(hDrawDC,x,y,option,NULL,string,len,NULL);

        if (InPrinting) SetBkMode(hDrawDC,PrevBkMode);  // set transparency
        
        if (TerFont[font].style&(ULINE|ULINED)) {   // draw underlines
           SIZE size;
           HPEN hPen;

           GetTextExtentPoint(hDrawDC,string,lstrlen(string),&size);
           y+=(TerFont[font].BaseHeight-TerFont[font].BaseHeightAdj);

           hPen=CreatePen(PS_SOLID,0,TerFont[font].TextColor);
           DrawShadowLine(w,hDrawDC,x,y,x+size.cx,y,hPen,NULL);
           
           if (TerFont[font].style&(ULINED)) {
              if (InPrinting) y+=TwipsToPrtY(30);
              else            y+=TwipsToScrY(30);
              DrawShadowLine(w,hDrawDC,x,y,x+size.cx,y,hPen,NULL);
           } 
           
           DeleteObject(hPen);

        } 
    }

    return TRUE;
}

/******************************************************************************
    DrawSectLineNbr:
    Draw the section line number.
*******************************************************************************/
DrawSectLineNbr(PTERWND w,HDC hPropDC,HDC hDrawDC,int row,int x, int y, int BaseHeight)
{
    BYTE str[100];
    WORD font=0;       // font to draw the bullet character
    int i,PixLen=0,len,PrevBkMode;
    int LangRtl=0;
    UINT option;

    dm("DrawSectLineNbr");

    wsprintf(str,"%d",SectLine+row+1);

    // set lang inf
    if ((LangRtl=IsDefLangRtl(w))) option=ETO_RTLREADING;
    else option=0;

    // check if the bullet will fit
    len=lstrlen(str);
    for (i=0;i<len;i++) PixLen+=DblCharWidth(w,font,TRUE,str[i],0);
    
    x-=(InPrinting?TwipsToPrtX(SECT_LINE_DIST):TwipsToScrX(SECT_LINE_DIST));    // distance from text
    x-=PixLen;

    y=y+BaseHeight-TerFont[font].BaseHeight+TerFont[font].BaseHeightAdj;
    y+=ScrYOffset;
        
    if (InPrinting) PrevBkMode=SetBkMode(hDrawDC,TRANSPARENT);  // set transparency for printing, screen displays sets its own transparency
       
    if (ScrRtl && pScrSeg) {     // adjust x
       SIZE size;
       GetTextExtentPoint(hDrawDC,str,lstrlen(str),&size);
       x=RtlX(w,x,size.cx,CurFrame,NULL);
    } 
        
    SetFont(w,hDrawDC,font,'T');  // set font

    ExtTextOut(hDrawDC,x,y,option,NULL,str,len,NULL);

    if (InPrinting) SetBkMode(hDrawDC,PrevBkMode);  // set transparency
        
    return TRUE;
}

/******************************************************************************
    DrawTrackingLine:
    Draw a vertical line to indicate the reviewed text line.
*******************************************************************************/
DrawTrackingLine(PTERWND w,HDC hPropDC,HDC hDrawDC,int x, int y, int height)
{
    int margin=45;

    dm("DrawTrackingLine");

    if (!InPrinting && !BorderShowing) margin=0;
    x-=(InPrinting?TwipsToPrtX(margin):TwipsToScrX(margin));    // distance from text

    DrawShadowLine(w,hDrawDC,x,y,x,y+height,GetStockObject(BLACK_PEN),NULL);
          
    return TRUE;
}

/******************************************************************************
    DrawRuler:
    Draw the ruler.
*******************************************************************************/
void DrawRuler(PTERWND w,BOOL CanPostpone)
{
    int   FrameNo,sect,i,pos,x1,y1,y2,count,ResX,RulerWidth,TabId,ColBeginX,
          RulerY,RoundX;
    float x,interval;
    BYTE string[10];
    RECT rect,rect1;
    HRGN rgn;
    HPEN hSolidPen,hShadowPen,hOldPen;
    HBRUSH hOldBrush,hBkBrush=null;
    HFONT hOldFont;
    MSG msg;
    struct StrTab far *tab;
    POINT  pt[5];
    int  obj,ScrX,BandHeight;
    HBITMAP hBM,hOldBM;

    if (True(TerFlags5&TFLAG5_OLD_RULER)) {      // draw old ruler
       OldDrawRuler(w,CanPostpone);
       return;
    } 

    if (!TerArg.ruler) return;

    if (!CaretEngaged && !RepaintRuler && MessageId!=WM_HSCROLL && HilightType==HILIGHT_OFF) { // caret not moving
       RulerPending=TRUE;
       return;
    }

    if (CanPostpone && RulerLineX==LineX(CurLine)) {
       if ( PeekMessage(&msg,hTerWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE|PM_NOYIELD)
         || PeekMessage(&msg,hTerWnd,WM_KEYDOWN,WM_KEYDOWN,PM_NOREMOVE|PM_NOYIELD) ) {
          RulerPending=TRUE;                // postpone the ruler painting
          return;
       }
    }

    RulerPending=RepaintRuler=FALSE;
    RulerSection=GetSection(w,CurLine);  // section for which the ruler is displayed
    RulerLineX=LineX(CurLine);
    if (HilightType!=HILIGHT_OFF) CurPfmt=pfmt(HilightBegRow);
    else                          CurPfmt=pfmt(CurLine); // update current paragraph id
    DeleteDragObjects(w,DRAG_TYPE_RULER_BEGIN,DRAG_TYPE_RULER_END);

    // ******************* select pen, font, color etc **************
    SetTextColor(hMemDC,0);             // write text in black color
    SetBkMode(hMemDC,TRANSPARENT);
    hSolidPen=GetStockObject(BLACK_PEN);
    hShadowPen=GetStockObject(WHITE_PEN);
    hOldFont=SelectObject(hMemDC,hRulerFont);

    ResX=ScrResX;

    // calculate the ruler width to draw
    sect=GetSection(w,CurLine);
    if (BorderShowing) RulerWidth=(int)(ResX*TerSect1[sect].PgWidth)+2*PrtToScrX(LeftBorderWidth);
    else               RulerWidth=(int)(ResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin)); // ruler width to draw
    if (RulerWidth<(TerWinOrgX+TerWinWidth)) RulerWidth=TerWinOrgX+TerWinWidth;
    FrameNo=GetFrame(w,CurLine);
    if (FrameNo<0) FrameNo=0;
    CurFrame=FrameNo;
    ColBeginX=frame[FrameNo].x+frame[FrameNo].SpaceLeft; // column displacement

    // set clip region
    rgn=CreateRectRgn(RulerRect.left,RulerRect.top,RulerRect.right,RulerRect.bottom);
    SelectObject(hTerDC,rgn);
    DeleteObject(rgn);

    // calculate ruler box coordinates in logical units
    rect.left=TerWinOrgX+(RulerRect.left-TerWinRect.left);
    rect.right=TerWinOrgX+TerWinWidth+(RulerRect.right-TerWinRect.right);
    rect.top=0;
    rect.bottom=RulerRect.bottom-RulerRect.top;

    // calculate the relative position of the ruler from the top of text window
    RulerY=TerWinOrgY+RulerRect.top-TerWinRect.top;

    // Create compatible bitmap and select it into the memory device context
    hBM=CreateCompatibleBitmap(hTerDC,rect.right-rect.left+1,rect.bottom-rect.top+1);
    hOldBM=SelectObject(hMemDC,hBM);
    SetViewportOrgEx(hMemDC,0,TerWinRect.top,NULL); // set view port origins
    SetWindowOrgEx(hMemDC,rect.left,TerWinRect.top,NULL); // set the logical window origin


    // *** draw ruler rectangle on the memory device context
    if (TextDefBkColor==CLR_WHITE) hBkBrush=GetStockObject(WHITE_BRUSH);
    else                           hBkBrush=CreateSolidBrush(TextDefBkColor);
      
    hOldBrush=SelectObject(hMemDC,hBkBrush);
    hOldPen=SelectObject(hMemDC,GetStockObject(NULL_PEN));
    
    Rectangle(hMemDC,rect.left,rect.top,rect.right+2,rect.bottom+1);   // clear to white
    
    SelectObject(hMemDC,(hToolbarBrush?hToolbarBrush:GetStockObject(LTGRAY_BRUSH)));  // brush for light-gray top and bottom bands
    if (TextDefBkColor!=CLR_WHITE) DeleteObject(hBkBrush);
    
    BandHeight=RulerFontHeight/2;
    
    // gray the top and bottom bands
    rect1=rect;
    rect1.bottom=rect1.top+BandHeight;
    Rectangle(hMemDC,rect1.left,rect1.top,rect1.right+2,rect1.bottom+1);   // clear to white
    
    rect1=rect;
    rect1.top=rect1.bottom-BandHeight;
    Rectangle(hMemDC,rect1.left,rect1.top,rect1.right+2,rect1.bottom+1);   // clear to white
    
    SelectObject(hMemDC,GetStockObject(GRAY_BRUSH));  // brush for dark-gray left and right areas

    // gray the left and right bands
    rect1=rect;
    rect1.top+=BandHeight;
    rect1.bottom-=BandHeight;
    rect1.right=ColBeginX;
    Rectangle(hMemDC,rect1.left,rect1.top,rect1.right+2,rect1.bottom+1);   // clear to white

    rect1.right=rect.right;  // restore
    rect1.left=ColBeginX+TerWrapWidth(w,CurLine,-1);
    Rectangle(hMemDC,rect1.left,rect1.top,rect1.right+2,rect1.bottom+1);   // clear to white
    
    SelectObject(hMemDC,hOldBrush);
    SelectObject(hMemDC,hOldPen);

    //************ Print the top ruler line *************************
    DrawShadowLine(w,hMemDC,rect.left,rect.top,rect.right,rect.top,hSolidPen,NULL);

    // set rtl variable
    ScrRtl=LineFlags2(CurLine)&LFLAG2_RTL;
    pScrSeg=NULL;

    y1=rect.top+RulerFontHeight;
    
    // draw the inch/cm marks
    x=(float)ColBeginX-3;
    interval=(float)ResX;
    if (TerFlags&TFLAG_METRIC) interval=interval*10/InchesToMm(1);
    y2=rect.top+BandHeight;
    count=0;                          // start with 0 inch
    while (x<=RulerWidth) {
       RoundX=RoundInt((int)(x*10),10)/10;

       // print inch label
       wsprintf(string,"%d",count);
       ScrX=RoundX;
       if (ScrRtl) ScrX=RtlX(w,ScrX-5,0,FrameNo,NULL);
       if (x>ColBeginX) TextOut(hMemDC,ScrX,y2,string,lstrlen(string));

       // increment to next inch
       x+=interval;
       count++;
    }

    // draw the half marks
    interval=((float)ResX)/2;
    if (TerFlags&TFLAG_METRIC) interval=interval*10/InchesToMm(1);
    x=ColBeginX+interval;
    y2=y1-RulerFontHeight/3;
    count=0;
    while (x<RulerWidth) {
       count++;
       if ((count%2)!=0) {  // do not write over the 'inch' locations
          RoundX=RoundInt((int)(x*10),10)/10;
          DrawShadowLine(w,hMemDC,RoundX,y2+2,RoundX,y2+6,hSolidPen,hShadowPen);
       }
       x+=interval;
    }

    // draw the 1/8 marks
    interval=((float)ResX)/8;
    if (TerFlags&TFLAG_METRIC) interval=interval*20/InchesToMm(1);
    x=ColBeginX+interval;
    y2=y1-RulerFontHeight/3;
    count=0;
    while (x<RulerWidth) {
       count++;
       if ((count%4)!=0) {                   // do not write over the 1/2 inch locations
          RoundX=RoundInt((int)(x*10),10)/10;
          DrawShadowLine(w,hMemDC,RoundX,y2+3,RoundX,y2+5,hSolidPen,hShadowPen);
       }
       x+=interval;
    }

    // ************ draw left and right tab marks *********************
    if (EditingParaStyle) TabId=StyleId[CurSID].TabId;
    else                  TabId=PfmtId[CurPfmt].TabId;   // current tab id

    tab=&TerTab[TabId];
    
    for (i=0;i<tab->count;i++) {
       int SideWidth=BandHeight-2;

       pos=tab->pos[i];                  // tab position in twips
       pos=TwipsToScrX(pos);             // tab position in screen units
       pos+=ColBeginX;                   // add column displacement

       // draw vertical line of the arrow
       y2=rect.bottom-BandHeight;
       DrawShadowLine(w,hMemDC,pos,y1+1,pos,y2,hSolidPen,hSolidPen);

       // Insert this object into the drag object table
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          int ObjPos=ScrRtl?RtlX(w,pos,0,FrameNo,NULL):pos;

          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_TAB;
          DragObj[obj].id1=i;                    // tab number

          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=ObjPos-TwipsToScrX(RULER_TOLERANCE);
          DragObj[obj].HotRect[0].top=RulerY+y1;
          DragObj[obj].HotRect[0].right=ObjPos+TwipsToScrX(RULER_TOLERANCE);
          DragObj[obj].HotRect[0].bottom=RulerY+y1+RulerFontHeight+1;

          DragObj[obj].ObjPointCount=2;          // three points in the polygon
          DragObj[obj].ObjPoint[0].x=ObjPos;
          DragObj[obj].ObjPoint[0].y=RulerY+y1;
          DragObj[obj].ObjPoint[1].x=ObjPos;
          DragObj[obj].ObjPoint[1].y=RulerY+y1+RulerFontHeight;
       }

       pos++;   // to compensate for double width horizontal line being drawn
       if      (tab->type[i]==TAB_LEFT)    DrawShadowLine(w,hMemDC,pos-1,y2,pos+SideWidth+1,y2,hSolidPen,hSolidPen); // left tab marker
       else if (tab->type[i]==TAB_RIGHT)   DrawShadowLine(w,hMemDC,pos,y2,pos-SideWidth-1,y2,hSolidPen,hSolidPen); // right tab marker
       else                                DrawShadowLine(w,hMemDC,pos-SideWidth+1,y2,pos+SideWidth,y2,hSolidPen,hSolidPen); // right tab marker

       if (tab->type[i]==TAB_DECIMAL)  {
          y2=(y1+y2)/2 + 1;
          DrawShadowLine(w,hMemDC,pos+SideWidth/2,y2,pos+SideWidth/2+2,y2,hSolidPen,hSolidPen); // decimal point
       }
        
    }

    // ************ draw default tab position markers *******************
    if (tab->count==0) pos=0;                         // last custom tab position
    else               pos=tab->pos[tab->count-1];

    pos = (((pos+RULER_TOLERANCE)/DefTabWidth)+1)*DefTabWidth; // use 50 twips for tolerance
    while (TRUE) {
       x1=TwipsToScrX(pos);
       x1+=ColBeginX;                                // add column displacement
       if (x1>=RulerWidth/*TerWinOrgX+TerWinWidth*/) break;        // out of screen width

       // draw the default tab marker
       y2=rect.bottom-BandHeight;
       DrawShadowLine(w,hMemDC,x1,y2+2,x1,y2+5,GetStockObject(WHITE_PEN),NULL);

       pos+=DefTabWidth;                           // position at next tab
       if (pos<0) break;                             // beyond 16 bit value
    }

    // ************ paragraph indentation *******************
    if (!TerArg.BorderMargins && (!ShowPageBorder || TerArg.FittedView)) {  // when the border space on the left is missing
       hOldBrush=SelectObject(hMemDC,GetStockObject(LTGRAY_BRUSH));

       if (EditingParaStyle) pt[0].x=TwipsToScrX(StyleId[CurSID].LeftIndentTwips); // draw left edge marker
       else                  pt[0].x=PfmtId[CurPfmt].LeftIndent;  // draw left edge marker

       pt[0].x+=ColBeginX;                      // add column displacement
       pt[0].y=y1;
       pt[1].x=pt[0].x;
       pt[1].y=y1+RulerFontHeight;
       pt[2].x=pt[0].x+BandHeight;
       pt[2].y=y1+BandHeight;
       if (ScrRtl) for (i=0;i<3;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
       Polygon(hMemDC,pt,3);
       // insert it to drag objects
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_LEFT;
          DragObj[obj].id1=CurPfmt;
          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=pt[0].x;
          DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
          DragObj[obj].HotRect[0].right=pt[2].x;
          if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
          DragObj[obj].HotRect[0].bottom=RulerY+pt[1].y;
          DragObj[obj].ObjPointCount=3;          // three points in the polygon
          for (i=0;i<3;i++) DragObj[obj].ObjPoint[i]=pt[i];
       }

       // draw the first line indent symbol
       if (EditingParaStyle)
            pt[0].x=TwipsToScrX(StyleId[CurSID].LeftIndentTwips+StyleId[CurSID].FirstIndentTwips);// draw first line edge marker
       else pt[0].x=PfmtId[CurPfmt].LeftIndent+PfmtId[CurPfmt].FirstIndent;// draw first line edge marker

       pt[0].x+=ColBeginX;                      // add column displacement
       pt[0].y=y1-RulerFontHeight;
       pt[1].x=pt[0].x;
       pt[1].y=y1;
       pt[2].x=pt[0].x+BandHeight;
       pt[2].y=y1-BandHeight;
       if (ScrRtl) for (i=0;i<3;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
       Polygon(hMemDC,pt,3);
       // insert it to drag objects
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_FIRST;
          DragObj[obj].id1=CurPfmt;
          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=pt[0].x;
          DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
          DragObj[obj].HotRect[0].right=pt[2].x;
          if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
          DragObj[obj].HotRect[0].bottom=RulerY+pt[1].y;
          DragObj[obj].ObjPointCount=3;          // three points in the polygon
          for (i=0;i<3;i++) DragObj[obj].ObjPoint[i]=pt[i];
       }

       // draw the right indent symbol
       if (EditingParaStyle)
            pt[0].x=TerWrapWidth(w,CurLine,-1)-TwipsToScrX(StyleId[CurSID].RightIndentTwips); // draw right edge marker
       else pt[0].x=TerWrapWidth(w,CurLine,-1)-PfmtId[CurPfmt].RightIndent; // draw right edge marker

       pt[0].x+=ColBeginX;                      // add column displacement
       pt[0].y=y1;
       pt[1].x=pt[0].x;
       pt[1].y=y1+RulerFontHeight;
       pt[2].x=pt[0].x-BandHeight;
       pt[2].y=y1+BandHeight;
       if (ScrRtl) for (i=0;i<3;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
       Polygon(hMemDC,pt,3);
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_RIGHT;
          DragObj[obj].id1=CurPfmt;
          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=pt[2].x;
          DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
          DragObj[obj].HotRect[0].right=pt[0].x;
          if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
          DragObj[obj].HotRect[0].bottom=RulerY+pt[1].y;
          DragObj[obj].ObjPointCount=3;          // three points in the polygon
          for (i=0;i<3;i++) DragObj[obj].ObjPoint[i]=pt[i];
       }
       
       SelectObject(hMemDC,hOldBrush);
    }
    else {
       int HalfWidth=BandHeight/2+1;  // 1/2 width of the indentation symbol
       hOldBrush=SelectObject(hMemDC,GetStockObject(LTGRAY_BRUSH));

       if (EditingParaStyle) pt[0].x=TwipsToScrX(StyleId[CurSID].LeftIndentTwips); // draw left edge marker
       else                  pt[0].x=PfmtId[CurPfmt].LeftIndent;  // draw left edge marker

       pt[0].x+=ColBeginX;                      // add point counter clock wise from the x pointer point (top point)
       pt[0].y=y1;
       pt[1].x=pt[0].x-HalfWidth;
       pt[1].y=pt[0].y+HalfWidth;
       pt[2].x=pt[1].x;
       pt[2].y=rect.bottom-BandHeight;
       pt[3].x=pt[0].x+HalfWidth;
       pt[3].y=pt[2].y;
       pt[4].x=pt[3].x;
       pt[4].y=pt[1].y;
       if (ScrRtl) for (i=0;i<5;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
       Polygon(hMemDC,pt,5);
       // insert it to drag objects
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_LEFT;
          DragObj[obj].id1=CurPfmt;
          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=pt[0].x-HalfWidth;
          DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
          DragObj[obj].HotRect[0].right=pt[0].x+HalfWidth;
          //if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
          DragObj[obj].HotRect[0].bottom=RulerY+pt[2].y;
          DragObj[obj].ObjPointCount=5;          // three points in the polygon
          for (i=0;i<5;i++) DragObj[obj].ObjPoint[i]=pt[i];
       }

       // draw the first line indent symbol
       if (EditingParaStyle)
            pt[0].x=TwipsToScrX(StyleId[CurSID].LeftIndentTwips+StyleId[CurSID].FirstIndentTwips);// draw first line edge marker
       else pt[0].x=PfmtId[CurPfmt].LeftIndent+PfmtId[CurPfmt].FirstIndent;// draw first line edge marker

       pt[0].x+=ColBeginX;                      // add point clock wise from the x pointer point (bottom point)
       pt[0].y=y1;
       pt[1].x=pt[0].x-HalfWidth;
       pt[1].y=pt[0].y-HalfWidth;
       pt[2].x=pt[1].x;
       pt[2].y=rect.top+BandHeight;
       pt[3].x=pt[0].x+HalfWidth;
       pt[3].y=pt[2].y;
       pt[4].x=pt[3].x;
       pt[4].y=pt[1].y;
       if (ScrRtl) for (i=0;i<5;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
       Polygon(hMemDC,pt,5);

       // insert it to drag objects
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_FIRST;
          DragObj[obj].id1=CurPfmt;
          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=pt[0].x-HalfWidth;
          DragObj[obj].HotRect[0].top=RulerY+pt[2].y;
          DragObj[obj].HotRect[0].right=pt[0].x+HalfWidth;
          //if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
          DragObj[obj].HotRect[0].bottom=RulerY+pt[0].y;
          DragObj[obj].ObjPointCount=5;          // three points in the polygon
          for (i=0;i<5;i++) DragObj[obj].ObjPoint[i]=pt[i];
       }

       // draw the right indent symbol
       if (EditingParaStyle)
            pt[0].x=TerWrapWidth(w,CurLine,-1)-TwipsToScrX(StyleId[CurSID].RightIndentTwips); // draw right edge marker
       else pt[0].x=TerWrapWidth(w,CurLine,-1)-PfmtId[CurPfmt].RightIndent; // draw right edge marker

       pt[0].x+=ColBeginX;                      // add point counter clock wise from the x pointer point (top point)
       pt[0].y=y1;
       pt[1].x=pt[0].x-HalfWidth;
       pt[1].y=pt[0].y+HalfWidth;
       pt[2].x=pt[1].x;
       pt[2].y=rect.bottom-BandHeight;
       pt[3].x=pt[0].x+HalfWidth;
       pt[3].y=pt[2].y;
       pt[4].x=pt[3].x;
       pt[4].y=pt[1].y;
       if (ScrRtl) for (i=0;i<5;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
       Polygon(hMemDC,pt,5);
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_RIGHT;
          DragObj[obj].id1=CurPfmt;
          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=pt[0].x-HalfWidth;
          DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
          DragObj[obj].HotRect[0].right=pt[0].x+HalfWidth;
          //if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
          DragObj[obj].HotRect[0].bottom=RulerY+pt[2].y;
          DragObj[obj].ObjPointCount=5;          // three points in the polygon
          for (i=0;i<5;i++) DragObj[obj].ObjPoint[i]=pt[i];
       }
       
       SelectObject(hMemDC,hOldBrush);
    } 
    
    
    // reset resources
    SetBkMode(hMemDC,OPAQUE);         // reset to opaque
    SelectObject(hMemDC,hOldFont);
    SelectObject(hMemDC,hOldBrush);

    // bit blast from mem dc to window dc
    BitBlt(hTerDC,rect.left,
           TerWinOrgY+rect.top-(TerWinRect.top-RulerRect.top),
           rect.right-rect.left+1,rect.bottom-rect.top+1,
           hMemDC,rect.left,
           rect.top,SRCCOPY);

    // delete the compatible bitmap
    SelectObject(hMemDC,hOldBM);
    DeleteObject(hBM);
    SetViewportOrgEx(hMemDC,0,0,NULL); // reset view port origins
    SetWindowOrgEx(hMemDC,0,0,NULL);   // reset the logical window origin

    // reset the clip region
    TerResetClipRgn(w);               // reset clip region

    ScrRtl=FALSE;

    return;
}

/******************************************************************************
    OldDrawRuler:
    Draw the ruler.
*******************************************************************************/
void OldDrawRuler(PTERWND w,BOOL CanPostpone)
{
    int   FrameNo,sect,i,pos,x1,y1,y2,count,ResX,RulerWidth,TabId,ColBeginX,
          RulerY,RoundX;
    float x,interval;
    BYTE string[10];
    RECT rect;
    HRGN rgn;
    HPEN hSolidPen,hShadowPen,hOldPen;
    HBRUSH hOldBrush;
    HFONT hOldFont;
    MSG msg;
    struct StrTab far *tab;
    POINT  pt[3];
    int  obj,ScrX;
    HBITMAP hBM,hOldBM;

    if (!TerArg.ruler) return;

    if (!CaretEngaged && !RepaintRuler && MessageId!=WM_HSCROLL && HilightType==HILIGHT_OFF) { // caret not moving
       RulerPending=TRUE;
       return;
    }

    if (CanPostpone && RulerLineX==LineX(CurLine)) {
       if ( PeekMessage(&msg,hTerWnd,WM_CHAR,WM_CHAR,PM_NOREMOVE|PM_NOYIELD)
         || PeekMessage(&msg,hTerWnd,WM_KEYDOWN,WM_KEYDOWN,PM_NOREMOVE|PM_NOYIELD) ) {
          RulerPending=TRUE;                // postpone the ruler painting
          return;
       }
    }

    RulerPending=RepaintRuler=FALSE;
    RulerSection=GetSection(w,CurLine);  // section for which the ruler is displayed
    RulerLineX=LineX(CurLine);
    if (HilightType!=HILIGHT_OFF) CurPfmt=pfmt(HilightBegRow);
    else                          CurPfmt=pfmt(CurLine); // update current paragraph id
    DeleteDragObjects(w,DRAG_TYPE_RULER_BEGIN,DRAG_TYPE_RULER_END);

    // ******************* select pen, font, color etc **************
    SetTextColor(hMemDC,0);             // write text in black color
    SetBkMode(hMemDC,TRANSPARENT);
    hSolidPen=GetStockObject(BLACK_PEN);
    hShadowPen=GetStockObject(WHITE_PEN);
    hOldFont=SelectObject(hMemDC,hRulerFont);

    ResX=ScrResX;

    // calculate the ruler width to draw
    sect=GetSection(w,CurLine);
    if (BorderShowing) RulerWidth=(int)(ResX*TerSect1[sect].PgWidth)+2*PrtToScrX(LeftBorderWidth);
    else               RulerWidth=(int)(ResX*(TerSect1[sect].PgWidth-TerSect[sect].LeftMargin-TerSect[sect].RightMargin)); // ruler width to draw
    if (RulerWidth<(TerWinOrgX+TerWinWidth)) RulerWidth=TerWinOrgX+TerWinWidth;
    FrameNo=GetFrame(w,CurLine);
    if (FrameNo<0) FrameNo=0;
    CurFrame=FrameNo;
    ColBeginX=frame[FrameNo].x+frame[FrameNo].SpaceLeft; // column displacement

    // set clip region
    rgn=CreateRectRgn(RulerRect.left,RulerRect.top,RulerRect.right,RulerRect.bottom);
    SelectObject(hTerDC,rgn);
    DeleteObject(rgn);

    // calculate ruler box coordinates in logical units
    rect.left=TerWinOrgX+(RulerRect.left-TerWinRect.left);
    rect.right=TerWinOrgX+TerWinWidth+(RulerRect.right-TerWinRect.right);
    rect.top=0;
    rect.bottom=RulerRect.bottom-RulerRect.top;

    // calculate the relative position of the ruler from the top of text window
    RulerY=TerWinOrgY+RulerRect.top-TerWinRect.top;

    // Create compatible bitmap and select it into the memory device context
    hBM=CreateCompatibleBitmap(hTerDC,rect.right-rect.left+1,rect.bottom-rect.top+1);
    hOldBM=SelectObject(hMemDC,hBM);
    SetViewportOrgEx(hMemDC,0,TerWinRect.top,NULL); // set view port origins
    SetWindowOrgEx(hMemDC,rect.left,TerWinRect.top,NULL); // set the logical window origin


    // draw ruler rectangle on the memory device context
    hOldBrush=SelectObject(hMemDC,(hToolbarBrush?hToolbarBrush:GetStockObject(LTGRAY_BRUSH)));
    hOldPen=SelectObject(hMemDC,GetStockObject(NULL_PEN));
    Rectangle(hMemDC,rect.left,rect.top,rect.right+2,rect.bottom+1);
    SelectObject(hMemDC,hOldBrush);
    SelectObject(hMemDC,hOldPen);

    //************ Print the ruler line *************************
    DrawShadowLine(w,hMemDC,rect.left,rect.top,rect.right,rect.top,hSolidPen,hShadowPen);
    y1=rect.top+RulerFontHeight;
    DrawShadowLine(w,hMemDC,rect.left,y1,rect.right,y1,hSolidPen,hShadowPen);

    // set rtl variable
    ScrRtl=LineFlags2(CurLine)&LFLAG2_RTL;
    pScrSeg=NULL;

    // draw the inch/cm marks
    x=(float)ColBeginX;
    interval=(float)ResX;
    if (TerFlags&TFLAG_METRIC) interval=interval*10/InchesToMm(1);
    y2=rect.top;
    count=0;                          // start with 0 inch
    while (x<=RulerWidth) {
       RoundX=RoundInt((int)(x*10),10)/10;
       DrawShadowLine(w,hMemDC,RoundX,y1,RoundX,y2,hSolidPen,hShadowPen);

       // print inch label
       wsprintf(string,"%d",count);
       ScrX=(int)(RoundX+2);
       if (ScrRtl) ScrX=RtlX(w,ScrX-5,0,FrameNo,NULL);
       if (x>ColBeginX) TextOut(hMemDC,ScrX,y2,string,lstrlen(string));

       // increment to next inch
       x+=interval;
       count++;
    }

    // draw the half marks
    interval=((float)ResX)/2;
    if (TerFlags&TFLAG_METRIC) interval=interval*10/InchesToMm(1);
    x=ColBeginX+interval;
    y2=y1-RulerFontHeight/2;
    while (x<RulerWidth) {
       RoundX=RoundInt((int)(x*10),10)/10;
       DrawShadowLine(w,hMemDC,RoundX,y1,RoundX,y2,hSolidPen,hShadowPen);
       x+=interval;
    }

    // draw the 1/8 marks
    interval=((float)ResX)/8;
    if (TerFlags&TFLAG_METRIC) interval=interval*20/InchesToMm(1);
    x=ColBeginX+interval;
    y2=y1-RulerFontHeight/3;
    while (x<RulerWidth) {
       RoundX=RoundInt((int)(x*10),10)/10;
       DrawShadowLine(w,hMemDC,RoundX,y1,RoundX,y2,hSolidPen,hShadowPen);
       x+=interval;
    }

    // ************ draw left and right tab marks *********************
    if (EditingParaStyle) TabId=StyleId[CurSID].TabId;
    else                  TabId=PfmtId[CurPfmt].TabId;   // current tab id

    tab=&TerTab[TabId];
    for (i=0;i<tab->count;i++) {
       pos=tab->pos[i];                  // tab position in twips
       pos=TwipsToScrX(pos);             // tab position in screen units
       pos+=ColBeginX;                   // add column displacement

       // draw vertical line of the arrow
       if (tab->type[i]==TAB_CENTER)       y2=y1+RulerFontHeight*3/4;
       else if (tab->type[i]==TAB_DECIMAL) y2=y1+RulerFontHeight*2/3 -1;
       else                                y2=y1+RulerFontHeight*2/3;
       DrawShadowLine(w,hMemDC,pos,y1,pos,y2,hSolidPen,NULL);

       // Insert this object into the drag object table
       if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
          int ObjPos=ScrRtl?RtlX(w,pos,0,FrameNo,NULL):pos;

          DragObj[obj].InUse=TRUE;
          DragObj[obj].type=DRAG_TYPE_RULER_TAB;
          DragObj[obj].id1=i;                    // tab number

          DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
          DragObj[obj].HotRect[0].left=ObjPos-TwipsToScrX(RULER_TOLERANCE);
          DragObj[obj].HotRect[0].top=RulerY+y1;
          DragObj[obj].HotRect[0].right=ObjPos+TwipsToScrX(RULER_TOLERANCE);
          DragObj[obj].HotRect[0].bottom=RulerY+y1+RulerFontHeight+1;

          DragObj[obj].ObjPointCount=2;          // three points in the polygon
          DragObj[obj].ObjPoint[0].x=ObjPos;
          DragObj[obj].ObjPoint[0].y=RulerY+y1;
          DragObj[obj].ObjPoint[1].x=ObjPos;
          DragObj[obj].ObjPoint[1].y=RulerY+y1+RulerFontHeight;
       }

       // draw left/right side of the arrow
       y2=y1+RulerFontHeight/4;
       DrawShadowLine(w,hMemDC,pos,y1,pos-RulerFontHeight/4,y2,hSolidPen,NULL); // left side
       DrawShadowLine(w,hMemDC,pos,y1,pos+RulerFontHeight/4,y2,hSolidPen,NULL); // right side

       // draw the bottom segment for the arrow
       y2=y1+RulerFontHeight*2/3;
       if      (tab->type[i]==TAB_LEFT)    DrawShadowLine(w,hMemDC,pos,y2,pos+RulerFontHeight/3,y2,hSolidPen,NULL); // left tab marker
       else if (tab->type[i]==TAB_RIGHT)   DrawShadowLine(w,hMemDC,pos,y2,pos-RulerFontHeight/3,y2,hSolidPen,NULL); // right tab marker
       else if (tab->type[i]==TAB_DECIMAL) DrawShadowLine(w,hMemDC,pos,y2,pos,y2+2,hSolidPen,NULL); // right tab marker
    }

    // ************ draw default tab position markers *******************
    if (tab->count==0) pos=0;                         // last custom tab position
    else               pos=tab->pos[tab->count-1];

    pos = (((pos+RULER_TOLERANCE)/DefTabWidth)+1)*DefTabWidth; // use 50 twips for tolerance
    while (TRUE) {
       x1=TwipsToScrX(pos);
       x1+=ColBeginX;                                // add column displacement
       if (x1>=RulerWidth/*TerWinOrgX+TerWinWidth*/) break;        // out of screen width

       // draw the default tab marker
       if (TerFlags&TFLAG_METRIC) {
          y2=y1+RulerFontHeight;
          DrawShadowLine(w,hMemDC,x1,y1+RulerFontHeight/2,x1,y2,hSolidPen,hShadowPen);
       }
       else {
          y2=y1+RulerFontHeight/2;
          DrawShadowLine(w,hMemDC,x1,y1,x1,y2,hSolidPen,NULL);
          DrawShadowLine(w,hMemDC,x1-RulerFontHeight/4,y2,x1+RulerFontHeight/4,y2,hSolidPen,NULL);
       }

       pos+=DefTabWidth;                           // position at next tab
       if (pos<0) break;                             // beyond 16 bit value
    }

    // ************ paragraph indentation *******************
    hOldBrush=SelectObject(hMemDC,GetStockObject(BLACK_BRUSH));

    if (EditingParaStyle) pt[0].x=TwipsToScrX(StyleId[CurSID].LeftIndentTwips); // draw left edge marker
    else                  pt[0].x=PfmtId[CurPfmt].LeftIndent;  // draw left edge marker

    pt[0].x+=ColBeginX;                      // add column displacement
    pt[0].y=y1;
    pt[1].x=pt[0].x;
    pt[1].y=y1+RulerFontHeight;
    pt[2].x=pt[0].x+RulerFontHeight/2;
    pt[2].y=y1+RulerFontHeight/2;
    if (ScrRtl) for (i=0;i<3;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
    Polygon(hMemDC,pt,3);
    // insert it to drag objects
    if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
       DragObj[obj].InUse=TRUE;
       DragObj[obj].type=DRAG_TYPE_RULER_LEFT;
       DragObj[obj].id1=CurPfmt;
       DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
       DragObj[obj].HotRect[0].left=pt[0].x;
       DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
       DragObj[obj].HotRect[0].right=pt[2].x;
       if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
       DragObj[obj].HotRect[0].bottom=RulerY+pt[1].y;
       DragObj[obj].ObjPointCount=3;          // three points in the polygon
       for (i=0;i<3;i++) DragObj[obj].ObjPoint[i]=pt[i];
    }

    // draw the first line indent symbol
    if (EditingParaStyle)
         pt[0].x=TwipsToScrX(StyleId[CurSID].LeftIndentTwips+StyleId[CurSID].FirstIndentTwips);// draw first line edge marker
    else pt[0].x=PfmtId[CurPfmt].LeftIndent+PfmtId[CurPfmt].FirstIndent;// draw first line edge marker

    pt[0].x+=ColBeginX;                      // add column displacement
    pt[0].y=y1-RulerFontHeight;
    pt[1].x=pt[0].x;
    pt[1].y=y1;
    pt[2].x=pt[0].x+RulerFontHeight/2;
    pt[2].y=y1-RulerFontHeight/2;
    if (ScrRtl) for (i=0;i<3;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
    Polygon(hMemDC,pt,3);
    // insert it to drag objects
    if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
       DragObj[obj].InUse=TRUE;
       DragObj[obj].type=DRAG_TYPE_RULER_FIRST;
       DragObj[obj].id1=CurPfmt;
       DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
       DragObj[obj].HotRect[0].left=pt[0].x;
       DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
       DragObj[obj].HotRect[0].right=pt[2].x;
       if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
       DragObj[obj].HotRect[0].bottom=RulerY+pt[1].y;
       DragObj[obj].ObjPointCount=3;          // three points in the polygon
       for (i=0;i<3;i++) DragObj[obj].ObjPoint[i]=pt[i];
    }

    // draw the right indent symbol
    if (EditingParaStyle)
         pt[0].x=TerWrapWidth(w,CurLine,-1)-TwipsToScrX(StyleId[CurSID].RightIndentTwips); // draw right edge marker
    else pt[0].x=TerWrapWidth(w,CurLine,-1)-PfmtId[CurPfmt].RightIndent; // draw right edge marker

    pt[0].x+=ColBeginX;                      // add column displacement
    pt[0].y=y1;
    pt[1].x=pt[0].x;
    pt[1].y=y1+RulerFontHeight;
    pt[2].x=pt[0].x-RulerFontHeight/2;
    pt[2].y=y1+RulerFontHeight/2;
    if (ScrRtl) for (i=0;i<3;i++) pt[i].x=RtlX(w,pt[i].x,0,FrameNo,NULL);
    Polygon(hMemDC,pt,3);
    if ((CurSID<0 || EditingParaStyle) && (obj=GetDragObjectSlot(w))>=0) {
       DragObj[obj].InUse=TRUE;
       DragObj[obj].type=DRAG_TYPE_RULER_RIGHT;
       DragObj[obj].id1=CurPfmt;
       DragObj[obj].HotRectCount=1;           // 1 hot spot rectangle
       DragObj[obj].HotRect[0].left=pt[2].x;
       DragObj[obj].HotRect[0].top=RulerY+pt[0].y;
       DragObj[obj].HotRect[0].right=pt[0].x;
       if (ScrRtl) SwapInts(&(DragObj[obj].HotRect[0].left),&(DragObj[obj].HotRect[0].right));
       DragObj[obj].HotRect[0].bottom=RulerY+pt[1].y;
       DragObj[obj].ObjPointCount=3;          // three points in the polygon
       for (i=0;i<3;i++) DragObj[obj].ObjPoint[i]=pt[i];
    }

    // reset resources
    SetBkMode(hMemDC,OPAQUE);         // reset to opaque
    SelectObject(hMemDC,hOldFont);
    SelectObject(hMemDC,hOldBrush);

    // bit blast from mem dc to window dc
    BitBlt(hTerDC,rect.left,
           TerWinOrgY+rect.top-(TerWinRect.top-RulerRect.top),
           rect.right-rect.left+1,rect.bottom-rect.top+1,
           hMemDC,rect.left,
           rect.top,SRCCOPY);

    // delete the compatible bitmap
    SelectObject(hMemDC,hOldBM);
    DeleteObject(hBM);
    SetViewportOrgEx(hMemDC,0,0,NULL); // reset view port origins
    SetWindowOrgEx(hMemDC,0,0,NULL);   // reset the logical window origin

    // reset the clip region
    TerResetClipRgn(w);               // reset clip region

    ScrRtl=FALSE;

    return;
}

/******************************************************************************
   DrawFrameBorder:
   Draw the specified line for a frame.
*******************************************************************************/
DrawFrameBorder(PTERWND w,HDC hDC, int x1,int y1,int x2,int y2,int thickness,int border)
{
   RECT rect;
   int  ParaFID;
   COLORREF BorderColor=0;
   HPEN hPen;
   HBRUSH hBrush;
   int CellId=frame[CurFrame].CellId;
   int PageNo=(CurFrame>=FirstPage2Frame?FirstFramePage+1:FirstFramePage);
   BOOL SaveScrRtl=ScrRtl;

   if (CellId>0 && CellAux[CellId].flags&CAUX_HIDDEN) {
      BOOL hidden=TRUE;
      int row=cell[CellId].row;
      int cl=TableRow[row].FirstCell;
      while (cl>0) {
        if (!(CellAux[cl].flags&CAUX_HIDDEN)) hidden=FALSE;
        cl=cell[cl].NextCell;
      } 
      if (hidden) return TRUE;  // this cell is hidden
   }

   // convert thickness to pixel units
   if (thickness>0) {
      int SaveThickness=thickness;
      if (CellId==0) {
         if (y1==y2) thickness=TwipsToScrY(thickness);
         else        thickness=TwipsToScrX(thickness);
      }
      else if (border==BORDER_TOP || border==BORDER_LEFT) {  // top/left border of the cell
         BOOL EndingCell=true;
         //if (border==BORDER_LEFT && ScrRtl) EndingCell=(cell[CellId].NextCell<=0);

           
         if (border==BORDER_TOP || EndingCell) {
            if (y1==y2) thickness=TwipsToScrY(thickness);
            else        thickness=TwipsToScrX(thickness);
            if (thickness<=0) thickness=1;
         }
         else {
            int RightThick=TwipsToScrX(thickness);  // contributing thickness of right border
            if (RightThick<=0) RightThick=1;
            thickness=TwipsToScrX(thickness)-RightThick;
         } 

      }
      else {      // bottom/right border of a cell
         BOOL EndingCell;
         if (border==BORDER_RIGHT) {
            int NextCell=cell[CellId].NextCell;
            EndingCell=(NextCell<=0 /*|| cell[NextCell].flags&CFLAG_ROW_SPANNED*/);
            if (ScrRtl) {
               EndingCell=true;   // do not apply special adjustment below
               if (cell[CellId].NextCell<=0) thickness=thickness/2;  // now this border is being shared
            }
         }
         else {
            //int row=cell[CellId].row;
            // EndingCell=(TableRow[row].NextRow<=0 || IsPageLastRow(w,-CellId,PageNo));
            EndingCell=(GetBorderCell(w,CellId,PageNo,TRUE)<=0);
         }
         if (EndingCell) {
            if (y1==y2) thickness=TwipsToScrY(thickness);
            else        thickness=TwipsToScrX(thickness);
            if (thickness<=0) thickness=1;
         }
         else {
            int LeftTopThick;  // contributing thickness of left/top border
            if (y1==y2) LeftTopThick=TwipsToScrY(thickness);
            else        LeftTopThick=TwipsToScrX(thickness);
            if (LeftTopThick<=0) LeftTopThick=1;
            if (y1==y2) thickness=TwipsToScrY(2*thickness)-LeftTopThick;
            else        thickness=TwipsToScrX(2*thickness)-LeftTopThick;
         }
      }
      if (thickness<=0) return TRUE;
   }



   if (thickness==0) {
      if (!(frame[CurFrame].border&border)) return TRUE;

      if (CellId>0 && cell[CellId].border&border) return TRUE;  // zero width border
      
      // does the previous row covers the current row top-border
      if (CellId>0 && border==BORDER_TOP) {
         int row=cell[CellId].row;
         int PrevRow=TableRow[row].PrevRow;
         BOOL IsTopRow=false;
         if (PrevRow>=0) {
            if (TableAux[PrevRow].LastPage!=TableAux[row].FirstPage) IsTopRow=true;  // top row of the page
            if (!IsTopRow && TableAux[row].LastPage==PageNo && TableAux[row].LastPage!=TableAux[row].FirstPage) IsTopRow=true;
            if (!IsTopRow) {
               int RowBegX=PrtToScrX(TableAux[PrevRow].FrmBegX);
               int RowEndX=PrtToScrX(TableAux[PrevRow].FrmEndX);
               BOOL DrawFull;
               if (x1>=RowBegX && x2<=RowEndX) return TRUE;   // this row top border is completely covered by the previous row's bottom border
            
               DrawFull=(x1<RowBegX && x2>RowEndX); // this is True when the current cell is wider than the previous row on both sides - so draw fully
               if (!DrawFull && (x1<RowBegX) && (x2<=RowBegX)) DrawFull=true;  // the current cell is not covered at all by the previous row
               if (!DrawFull && (x2>RowEndX) && (x1>=RowEndX)) DrawFull=true;  // the current cell is not covered at all by the previous row
               if (!DrawFull) {   
                  if (x1<RowBegX) x2=RowBegX;   // draw only the left uncovered side
                  if (x2>RowEndX) x1=RowEndX;   // draw only the right uncovered side
               }
            }     
         } 
      } 

      if (True(frame[CurFrame].flags&FRAME_RTL) && border==BORDER_RIGHT && x1==-1 && x2==-1) x1=x2=0;  // try to show the visual left dotted line

      if (FullCellHilight) return DrawDottedLine(w,hDC,x1,y1,x2,y2,0xFFFFFF);
      else                 return DrawDottedLine(w,hDC,x1,y1,x2,y2,0);
   }

   // Get the border color
   if (border==BORDER_TOP)   BorderColor=frame[CurFrame].BorderColor[BORDER_INDEX_TOP];
   if (border==BORDER_BOT)   BorderColor=frame[CurFrame].BorderColor[BORDER_INDEX_BOT];
   if (border==BORDER_LEFT)  BorderColor=frame[CurFrame].BorderColor[BORDER_INDEX_LEFT];
   if (border==BORDER_RIGHT) BorderColor=frame[CurFrame].BorderColor[BORDER_INDEX_RIGHT];


   ParaFID=frame[CurFrame].ParaFrameId;
   if (ParaFrame[ParaFID].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_CONV)) BorderColor=ParaFrame[ParaFID].LineColor;
   if (ParaFID && ParaFrame[ParaFID].flags&PARA_FRAME_DOTTED) {
      return DrawDottedLine(w,hDC,x1,y1,x2,y2,BorderColor);
   } 

    
   // draw border lines
   ScrRtl=false;
   if (thickness==1) {
      if (FullCellHilight) DrawShadowLine(w,hDC,x1,y1,x2,y2,GetStockObject(WHITE_PEN),NULL);
      else {
         if (BorderColor==0L) hPen=GetStockObject(BLACK_PEN);
         else                 hPen=CreatePen(PS_SOLID,1,BorderColor);
         DrawShadowLine(w,hDC,x1,y1,x2,y2,hPen,NULL);
         if (BorderColor!=0L) DeleteObject(hPen);
      }
   }
   else {                 // draw a thick border
      if (thickness>1) {
        if (border==BORDER_TOP || border==BORDER_BOT) {
           if (border==BORDER_TOP) rect.top=y1;
           else                    rect.top=y1-thickness+1;
           rect.bottom=rect.top+thickness;

           rect.left=x1;
           rect.right=x2;
        }
        else {
           if (border==BORDER_LEFT)  rect.left=x1;
           else                      rect.left=x1-thickness+1;
           rect.right=rect.left+thickness;

           rect.top=y1;
           rect.bottom=y2;
        }
        if (FullCellHilight) FillRect(hDC,&rect,GetStockObject(WHITE_BRUSH));
        else {
           if (BorderColor==0L) hBrush=GetStockObject(BLACK_BRUSH);
           else                 hBrush=CreateSolidBrush(BorderColor);
           FillRect(hDC,&rect,hBrush);
           if (BorderColor!=0L) DeleteObject(hBrush);
        }
      }
   }
   ScrRtl=SaveScrRtl;

   return TRUE;
}

/******************************************************************************
     DisplayStatus:
     Display the row/col/line status line
*******************************************************************************/
DisplayStatus(PTERWND w)
{
    BYTE string[50];
    int x,y,top,bottom,left,right,width,i,base,RowUnits,LeftX,RightX;
    HRGN rgn;
    HFONT hOldFont;
    COLORREF OldTextColor,OldTextBkColor;
    HPEN hDarkPen,hLightPen;
    RECT rect;
    //int  SaveCurFmtId;

    if (InPrintPreview) return TRUE;

    if (!TerArg.ShowStatus) return (BOOL)SendMessageToParent(w,TER_UPDATE_STATUSBAR,(WPARAM)hTerWnd,0L,FALSE);  // update any external status bar

    // include the status line into the clipping region
    rgn=CreateRectRgn(TerRect.left,TerWinRect.top,TerRect.right,TerRect.bottom);
    SelectObject(hTerDC,rgn);
    DeleteObject(rgn);

    if (FontsReleased) RecreateFonts(w,hTerDC);  // recreate fonts

    // create the pens
    hDarkPen=CreatePen(PS_SOLID,StatusLineWidth,0x7f7f7f);  // dark pen
    hLightPen=CreatePen(PS_SOLID,StatusLineWidth,0xefefef);  // light pen

    // select the font and the colors
    hOldFont=SelectObject(hTerDC,hStatusFont);
    OldTextColor=SetTextColor(hTerDC,StatusColor);
    OldTextBkColor=SetBkColor(hTerDC,StatusBkColor);

    // draw the background
    RowUnits=y=LineToUnits(w,STATUS_ROW);
    LeftX=TerWinOrgX-(TerWinRect.left-TerRect.left);
    RightX=LeftX+TerRect.right-TerRect.left;
    //ClearEOL(w,hTerDC,LeftX,RowUnits,RightX,StatusFontHeight+6*StatusLineWidth);
    rect.left=LeftX;
    rect.right=RightX;
    rect.top=RowUnits;
    rect.bottom=rect.top+StatusFontHeight+6*StatusLineWidth;
    //SaveCurFmtId=CurFmtId;
    //CurFmtId=0;
    //TerTextOut(w,hTerDC,LeftX,RightX,&rect,NULL,NULL,0,NULL,0,0);
    ExtTextOut(hTerDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);

    //CurFmtId=SaveCurFmtId;

    bottom=RowUnits+StatusFontHeight+6*StatusLineWidth-1;
    DrawShadowLine(w,hTerDC,LeftX,bottom,RightX,bottom,hLightPen,NULL);  // right line

    RowUnits+=(3*StatusLineWidth);

    // print insert/overtype status
    if (InsertMode) lstrcpy(string,MsgString[MSG_SB_INS]);else lstrcpy(string,MsgString[MSG_SB_OT]);
    x=TerWinOrgX+2;
    TextOut(hTerDC,x+StatusLineWidth,RowUnits,string,3);

    // draw a box around insert/overtype status
    width=TerTextExtentX(hTerDC,MsgString[MSG_SB_INS],3);    // pixel length of the string
    top=y+2*StatusLineWidth;
    bottom=top+StatusFontHeight+StatusLineWidth;
    left=x;
    right=x+width+2*StatusLineWidth;
    DrawShadowBox(w,hTerDC,left,top,right,bottom,hDarkPen,hLightPen);


    // position the row/col/line status info
    if (TerArg.FittedView) sprintf(string,"%s00000 %s0000",MsgString[MSG_SB_LINE],MsgString[MSG_SB_COL]);       // sample status line
    else                   sprintf(string,"%s0000 %s00000 %s0000",MsgString[MSG_SB_ROW],MsgString[MSG_SB_LINE],MsgString[MSG_SB_COL]);       // sample status line
    i=TerTextExtentX(hTerDC,string,lstrlen(string));
    base=max((TerWinRect.right-TerWinRect.left-i)/2,right+10);  // status line being column
    base=base+TerWinOrgX;
    left=base-4;                             // left of the box

    // draw row or page number
    if (!TerArg.FittedView) {
       if (TerArg.WordWrap && TerArg.PrintView) lstrcpy(string,MsgString[MSG_SB_PAGE]);
       else                                     lstrcpy(string,MsgString[MSG_SB_ROW]);
       TextOut(hTerDC,base,RowUnits,string,lstrlen(string));
       base+=TerTextExtentX(hTerDC,string,lstrlen(string));
       StatusRowPos=base-TerWinOrgX;

       if (TerArg.WordWrap && TerArg.PrintView) {
          if (TerFlags2&TFLAG2_SHOW_SECT_PAGE_NO)  _ltoa(PageInfo[CurPage].DispNbr,TempString,10);
          else                                     _ltoa(CurPage+1,TempString,10);
       }
       else _ltoa(CurRow+1,TempString,10);

       for (i=strlen(TempString);i<4;i++) TempString[i]=' ';TempString[4]=0;
       TextOut(hTerDC,base,RowUnits,TempString,strlen(TempString));
       base+=TerTextExtentX(hTerDC,"0000 ",5);
    }

    // draw the line number
    lstrcpy(string,MsgString[MSG_SB_LINE]);
    TextOut(hTerDC,base,RowUnits,string,lstrlen(string));
    base+=TerTextExtentX(hTerDC,string,lstrlen(string));
    StatusLinePos=base-TerWinOrgX;

    _ltoa(GetStatusLineNo(w),TempString,10);
    for (i=strlen(TempString);i<5;i++) TempString[i]=' ';TempString[5]=0;
    TextOut(hTerDC,base,RowUnits,TempString,strlen(TempString));
    base+=TerTextExtentX(hTerDC,"00000 ",6);

    // draw the column number
    lstrcpy(string,MsgString[MSG_SB_COL]);
    TextOut(hTerDC,base,RowUnits,string,lstrlen(string));
    base+=TerTextExtentX(hTerDC,string,lstrlen(string));
    StatusColPos=base-TerWinOrgX;

    _itoa(CurCol+1,TempString,10);
    for (i=strlen(TempString);i<4;i++) TempString[i]=' ';TempString[4]=0;
    TextOut(hTerDC,base,RowUnits,TempString,strlen(TempString));

    // draw the box around row/col/line
    right=base+TerTextExtentX(hTerDC,"00000",5)+1;
    DrawShadowBox(w,hTerDC,left,top,right,bottom,hDarkPen,hLightPen);

    // reset font and color in the DC
    SelectObject(hTerDC,hOldFont);
    SetTextColor(hTerDC,OldTextColor);
    SetBkColor(hTerDC,OldTextBkColor);

    TerResetClipRgn(w);                // reset the clip region

    // delete the resources
    DeleteObject(hDarkPen);
    DeleteObject(hLightPen);

    return TRUE;
}

/******************************************************************************
     DisplayStatusInfo:
     Display the status line information
*******************************************************************************/
DisplayStatusInfo(PTERWND w)
{
    int i,RowUnits,OldBkMode,WidthTable[5];
    HRGN rgn;
    HFONT hOldFont;
    COLORREF OldTextColor,OldTextBkColor;
    SIZE size;
    RECT rect;

    if (!TerArg.ShowStatus) return (BOOL)SendMessageToParent(w,TER_UPDATE_STATUSBAR,(WPARAM)hTerWnd,0L,FALSE);  // update any external status bar

    // include the status line into the clipping region
    rgn=CreateRectRgn(TerWinRect.left,TerWinRect.top,TerWinRect.right-1,TerRect.bottom);
    SelectObject(hTerDC,rgn);
    DeleteObject(rgn);

    // select the font and the colors
    hOldFont=SelectObject(hTerDC,hStatusFont);
    OldTextColor=SetTextColor(hTerDC,StatusColor);
    OldTextBkColor=SetBkColor(hTerDC,StatusBkColor);
    OldBkMode=SetBkMode(hTerDC,OPAQUE);

    // get the ave width of digits
    lstrcpy(TempString,"0123456789");
    GetTextExtentPoint(hTerDC,TempString,lstrlen(TempString),&size);

    for (i=0;i<5;i++) WidthTable[i]=size.cx/10;    // set the width to the width of a digit

    RowUnits=LineToUnits(w,STATUS_ROW)+(3*StatusLineWidth);

    rect.top=RowUnits;
    rect.bottom=RowUnits+size.cy;

    // write row number or the page number
    if (!TerArg.FittedView) {
       if (TerArg.WordWrap && TerArg.PrintView) {
          if (TerFlags2&TFLAG2_SHOW_SECT_PAGE_NO)  _ltoa(PageInfo[CurPage].DispNbr,TempString,10);
          else                                     _ltoa(CurPage+1,TempString,10);
       }
       else  _ltoa(CurRow+1,TempString,10);
       for (i=strlen(TempString);i<4;i++) TempString[i]=' ';TempString[4]=0;
       rect.left=TerWinOrgX+StatusRowPos;  // print using rect to compensate for bug with MatroxMil winnt video driver
       rect.right=rect.left+4*WidthTable[0];
       ExtTextOut(hTerDC,TerWinOrgX+StatusRowPos,RowUnits,ETO_OPAQUE,&rect,TempString,4,WidthTable);
    }

    // calculate the display line number
    _ltoa(GetStatusLineNo(w),TempString,10);
    for (i=strlen(TempString);i<5;i++) TempString[i]=' ';TempString[5]=0;
    rect.left=TerWinOrgX+StatusLinePos;
    rect.right=rect.left+5*WidthTable[0];
    ExtTextOut(hTerDC,TerWinOrgX+StatusLinePos,RowUnits,ETO_OPAQUE,&rect,TempString,5,WidthTable);

    // write the column number
    _itoa(CurCol+1,TempString,10);
    for (i=strlen(TempString);i<4;i++) TempString[i]=' ';TempString[4]=0;
    rect.left=TerWinOrgX+StatusColPos;
    rect.right=rect.left+4*WidthTable[0];
    ExtTextOut(hTerDC,TerWinOrgX+StatusColPos,RowUnits,ETO_OPAQUE,&rect,TempString,4,WidthTable);

    // select the font and the colors
    SelectObject(hTerDC,hOldFont);
    SetTextColor(hTerDC,OldTextColor);
    SetBkColor(hTerDC,OldTextBkColor);
    SetBkMode(hTerDC,OldBkMode);

    TerResetClipRgn(w);

    return TRUE;
}

/******************************************************************************
     RedrawNonText:
     Redraw the non text area when the painting spill over the non-text area.
******************************************************************************/
RedrawNonText(PTERWND w)
{
    // draw the ruler and status line
    if (BorderSpill&SPILL_TOP && TerArg.ruler) {
       RepaintRuler=TRUE;      // force repaint ruler
       DrawRuler(w,FALSE);
    }
    if (BorderSpill&SPILL_BOT && TerArg.ShowStatus) DisplayStatus(w);

    // draw the border margins
    if (TerArg.BorderMargins) {
       RECT rect;
       COLORREF OldBkColor=SetBkColor(hTerDC,TextDefBkColor);

       if (BorderSpill&SPILL_LEFT) {   // draw the left border margin
          rect.left=TerWinOrgX-TwipsToScrX(BORDER_MARGIN);
          rect.right=TerWinOrgX;
          rect.top=TerWinOrgY-TerWinRect.top;
          rect.bottom=rect.top+TerRect.bottom;
          ExtTextOut(hTerDC,rect.left,rect.top,ETO_OPAQUE,&rect,NULL,0,NULL);
       }
       if (BorderSpill&SPILL_RIGHT) {   // draw the left border margin
          rect.left=TerWinOrgX+TerWinWidth;;
          rect.right=rect.left+TwipsToScrX(BORDER_MARGIN)+1;
          rect.top=TerWinOrgY-TerWinRect.top;
          rect.bottom=rect.top+TerRect.bottom;
          ExtTextOut(hTerDC,rect.left,rect.top,ETO_OPAQUE,&rect,NULL,0,NULL);
       }
       if (BorderSpill&SPILL_TOP) {   // draw the top border margin
          rect.left=TerWinOrgX-TwipsToScrX(BORDER_MARGIN);
          rect.right=rect.left+TerRect.right;
          rect.top=TerWinOrgY-TwipsToScrY(BORDER_MARGIN);
          rect.bottom=TerWinOrgY;
          ExtTextOut(hTerDC,rect.left,rect.top,ETO_OPAQUE,&rect,NULL,0,NULL);
       }
       if (BorderSpill&SPILL_BOT) {   // draw the bottom border margin
          rect.left=TerWinOrgX-TwipsToScrX(BORDER_MARGIN);
          rect.right=rect.left+TerRect.right;
          rect.top=TerWinOrgY+TerWinHeight;
          rect.bottom=rect.top+TwipsToScrY(BORDER_MARGIN);
          ExtTextOut(hTerDC,rect.left,rect.top,ETO_OPAQUE,&rect,NULL,0,NULL);
       }
       SetBkColor(hTerDC,OldBkColor);
    }

    return TRUE;
}

/******************************************************************************
   DrawPageBorder:
   Draw a border for an html cell.
*******************************************************************************/
BOOL DrawPageBorder(PTERWND w,HDC hDC, int FrameNo)
{
   HPEN hThinPen=GetStockObject(BLACK_PEN);
   HPEN hThickPen=NULL;
   UINT border=frame[FrameNo].flags&FRAME_BORDER;
   int x,y,width,height,BorderWidth;

   // create a thick pen
   if (border&(FRAME_BORDER_RIGHT|FRAME_BORDER_BOT)) hThickPen=CreatePen(PS_SOLID,3,0);

   x=frame[FrameNo].x;
   y=frame[FrameNo].y;
   width=frame[FrameNo].width;
   height=frame[FrameNo].height;
   BorderWidth=PrtToScrX(frame[FrameNo].LeftBorderWdth);

   // draw borders
   if (border&FRAME_BORDER_TOP) {
      DrawShadowLine(w,hDC,x+BorderWidth,y+height-1,x+width-BorderWidth,y+height-1,hThinPen,NULL);
   }
   if (border&FRAME_BORDER_BOT) {
      DrawShadowLine(w,hDC,x+BorderWidth,y,x+width-BorderWidth,y,hThinPen,hThickPen);
   }
   if (border&FRAME_BORDER_LEFT) {
      DrawShadowLine(w,hDC,x+width-1,y,x+width-1,y+height,hThinPen,NULL);
   }
   if (border&FRAME_BORDER_RIGHT) {
      DrawShadowLine(w,hDC,x,y,x,y+height,hThinPen,hThickPen);
   }

   if (hThickPen) DeleteObject(hThickPen);

   return TRUE;
}

/******************************************************************************
   DrawPageBorderBox:
   Draw a box around the text area on the page
*******************************************************************************/
BOOL DrawPageBorderBox(PTERWND w,HDC hDC, int FrameNo)
{
   HPEN hPen,hShadowPen;
   UINT border;
   int i,x,y,width,height,sect,thick,gap=30,delta;
   BOOL shadow;
   BOOL IsThin[5];
   int count,NumHalfWidths,TwipsThick[5],TotalLineThickness;
   float UnitThickness;

   x=frame[FrameNo].x;
   y=frame[FrameNo].y;
   width=frame[FrameNo].width;
   height=frame[FrameNo].height;

   sect=frame[FrameNo].sect;
   border=TerSect[sect].border;
   
   // decide the number of lines for the border
   count=1;
   NumHalfWidths=2;         // number of thin-line widths used for all lines in the composit line
   IsThin[0]=IsThin[1]=IsThin[2]=FALSE;
   shadow=FALSE;
   if (TerSect[sect].BorderType==BRDRTYPE_DBL) {
      count=2;
      NumHalfWidths=4;
   }
   else if (TerSect[sect].BorderType==BRDRTYPE_TRIPLE) {
      count=3;
      NumHalfWidths=6;
   }
   else if (TerSect[sect].BorderType==BRDRTYPE_SHADOW) shadow=TRUE;
   else if (TerSect[sect].BorderType==BRDRTYPE_THICK_THIN) {
      count=2;
      IsThin[1]=TRUE;
      NumHalfWidths=3;
   } 
   else if (TerSect[sect].BorderType==BRDRTYPE_THIN_THICK) {
      count=2;
      IsThin[0]=TRUE;
      NumHalfWidths=3;
   } 
   else if (TerSect[sect].BorderType==BRDRTYPE_THICK_THIN_THICK) {
      count=3;
      IsThin[1]=TRUE;
      NumHalfWidths=5;
   } 
   else if (TerSect[sect].BorderType==BRDRTYPE_THIN_THICK_THIN) {
      count=3;
      IsThin[0]=IsThin[2]=TRUE;
      NumHalfWidths=4;
   } 

   // calculate the thickness of a thin line
   while (true) {
      TotalLineThickness=TerSect[sect].BorderWidth[BORDER_INDEX_TOP]-gap*(count-1);  // decrease by number of gaps in the compsite line
      UnitThickness=(float)TotalLineThickness/NumHalfWidths; // thickness of a thin line - thin line is half the width of the thick line
      if (UnitThickness>gap || gap<=15 || count==1) break;
      gap=gap/2;                                             // try to reduce gap as the linethikness reduces
   }

   
   // calcualte the thickness of each line
   for (i=0;i<count;i++) TwipsThick[i]=(int)(UnitThickness*(IsThin[i]?1:2));

   // clear the area around the page
   if (PageBkColor!=CLR_WHITE && !InPrinting) {     // printing code does its own page background drawing
      RECT rect;
      int  SpaceTop,SpaceBot,SpaceLeft,SpaceRight;

      SpaceTop=TerSect[sect].BorderSpace[BORDER_INDEX_TOP];
      SpaceTop=(InPrinting?TwipsToPrtY(SpaceTop):TwipsToScrY(SpaceTop));
      if (InPrintPreview) SpaceTop-=ScrToPrtY(2);      // save the top print-preview box line

      SpaceBot=TerSect[sect].BorderSpace[BORDER_INDEX_BOT];
      SpaceBot=(InPrinting?TwipsToPrtY(SpaceBot):TwipsToScrY(SpaceBot));
      
      SpaceLeft=TerSect[sect].BorderSpace[BORDER_INDEX_LEFT];
      SpaceLeft=(InPrinting?TwipsToPrtX(SpaceLeft):TwipsToScrX(SpaceLeft));
      if (InPrintPreview) SpaceLeft-=ScrToPrtY(2);      // save the left print-preview box line
      
      SpaceRight=TerSect[sect].BorderSpace[BORDER_INDEX_RIGHT];
      SpaceRight=(InPrinting?TwipsToPrtX(SpaceRight):TwipsToScrX(SpaceRight));

      SetRect(&rect,x-SpaceLeft,y-SpaceTop,x+width+SpaceRight,y);  // top area above the border
      FillRect(hDC,&rect,GetStockObject(WHITE_BRUSH));             // clear this area

      SetRect(&rect,x-SpaceLeft,y+height,x+width+SpaceRight,y+height+SpaceBot);  // area below the bottom border
      FillRect(hDC,&rect,GetStockObject(WHITE_BRUSH));             // clear this area

      SetRect(&rect,x-SpaceLeft,y-SpaceTop,x,y+height+SpaceBot);  // area before the left border
      FillRect(hDC,&rect,GetStockObject(WHITE_BRUSH));             // clear this area

      SetRect(&rect,x+width,y-SpaceTop,x+width+SpaceRight,y+height+SpaceBot);  // area before the left border
      FillRect(hDC,&rect,GetStockObject(WHITE_BRUSH));             // clear this area
   } 

   // draw borders;
   hShadowPen=NULL;

   for (i=0;i<count;i++) {
      
      hPen=OurCreatePen(w,TwipsThick[i],TerSect[sect].BorderColor,&thick);  // assume all sides to be of the same thickness
      if (shadow) hShadowPen=OurCreatePen(w,TerSect[sect].BorderWidth[BORDER_INDEX_TOP]*(IsThin[i]?2:1),TerSect[sect].BorderColor|0x808080,&thick);  // assume all sides to be of the same thickness

      if (border&BORDER_TOP) {
         DrawShadowLine(w,hDC,x,y,x+width,y,hPen,hShadowPen);
      }
      if (border&BORDER_BOT) {
         DrawShadowLine(w,hDC,x,y+height,x+width,y+height,hPen,hShadowPen);
      }
      if (border&BORDER_LEFT) {
         DrawShadowLine(w,hDC,x,y,x,y+height,hPen,hShadowPen);
      }
      if (border&BORDER_RIGHT) {
         DrawShadowLine(w,hDC,x+width,y,x+width,y+height,hPen,hShadowPen);
      }

      // check if dbl line border needs to be displayed
      if ((i+1)<count) {
         delta=gap+(TwipsThick[i]+TwipsThick[i+1])/2;
         delta=(InPrinting?TwipsToPrtX(delta):TwipsToScrX(delta));
         x+=delta;
         y+=delta;
         width-=2*delta;
         height-=2*delta;
      }

      DeleteObject(hPen);
      if (hShadowPen) DeleteObject(hShadowPen);
      hShadowPen=NULL;
   }

   return TRUE;
}

/******************************************************************************
   OurCreatePen:
   Create a solid pen of the specified width in twips and color. The last argument
   receives the pen thickness in device units. 
*******************************************************************************/
HPEN OurCreatePen(PTERWND w,int TwipsThick, COLORREF color, LPINT pDeviceThick)
{
   int thick;


   if (InPrinting) thick=TwipsToPrtX(TwipsThick);
   else            thick=TwipsToScrX(TwipsThick);

   if (thick<1) thick=1;

   if (pDeviceThick) (*pDeviceThick)=thick;

   return CreatePen(PS_SOLID,thick,color);
}
 
/******************************************************************************
   DrawHtmlCellBorder:
   Draw a border for an html cell.
*******************************************************************************/
BOOL DrawHtmlCellBorder(PTERWND w,HDC hDC,int CellId,int x1,int y1,int x2,int y2,LPINT BorderWidth, BOOL screen)
{
   HPEN hBlackPen=GetStockObject(BLACK_PEN);
   HPEN hWhitePen=GetStockObject(WHITE_PEN);
   COLORREF OldBkColor;
   BOOL TempWhitePen=FALSE,TempBlackPen=FALSE,DrawCellInside=TRUE;
   RECT rect;
   int  i,row,LeftWidth,RightWidth,TopWidth,BotWidth;
   BOOL DrawTopLine=FALSE,DrawBotLine=FALSE,DrawLeftLine=FALSE,DrawRightLine=FALSE;
   int PageNo=(CurFrame>=FirstPage2Frame?FirstFramePage+1:FirstFramePage);
   BOOL PartTopRow,PartBotRow;
   int  one,two,three;

   if (cell[CellId].flags&CFLAG_ROW_SPANNED) return TRUE;  // no borders for the spanning cell

   if (screen) one=1;
   else {
      one=TwipsToPrtX(15);  // unit thick ness
      hBlackPen=CreatePen(PS_SOLID,one,0x0);
      hWhitePen=CreatePen(PS_SOLID,one,0x3F3F3F);
      TempWhitePen=TempBlackPen=TRUE;
   }
   two=2*one;               // two times pixel width
   three=3*one;

   PartTopRow=IsPartRow(w,TRUE,cell[CellId].row,PageNo);
   PartBotRow=IsPartRow(w,FALSE,cell[CellId].row,PageNo);

   // is this an empty cell
   if (TerArg.ReadOnly) {
      long FirstLine=frame[CurFrame].PageFirstLine;
      if (tabw(FirstLine) && tabw(FirstLine)->type&INFO_CELL && LineLen(FirstLine)==1 && !PartTopRow) DrawCellInside=FALSE;
   }

   if (screen) {
      LeftWidth=TwipsToScrX(BorderWidth[BORDER_INDEX_LEFT]);
      RightWidth=TwipsToScrX(BorderWidth[BORDER_INDEX_RIGHT]);
      TopWidth=TwipsToScrY(BorderWidth[BORDER_INDEX_TOP]);
      BotWidth=TwipsToScrY(BorderWidth[BORDER_INDEX_BOT]);
   }
   else {
      LeftWidth=TwipsToPrtX(BorderWidth[BORDER_INDEX_LEFT]);
      RightWidth=TwipsToPrtX(BorderWidth[BORDER_INDEX_RIGHT]);
      TopWidth=TwipsToPrtY(BorderWidth[BORDER_INDEX_TOP]);
      BotWidth=TwipsToPrtY(BorderWidth[BORDER_INDEX_BOT]);
   } 

   // check if we need to draw the outer table enclosed
   row=cell[CellId].row;
   if (IsFirstTableRow(w,row)) DrawTopLine=TRUE;
   for (i=cell[CellId].RowSpan;i>1;i--) if (row>0) row=TableRow[row].NextRow;
   if (row<=0 || IsLastTableRow(w,row))  DrawBotLine=TRUE;
   if (cell[CellId].PrevCell<=0) DrawLeftLine=TRUE;
   if (cell[CellId].NextCell<=0) DrawRightLine=TRUE;


   // clear the border area
   if (screen) OldBkColor=SetBkColor(hDC,PageColor(w));

   SetRect(&rect,x1,y1,x2,y1+TopWidth); // clear top area
   if (!PartTopRow) ExtTextOut(hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);

   SetRect(&rect,x1,y2-BotWidth,x2,y2); // clear bottom area
   if (!PartBotRow) ExtTextOut(hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);

   SetRect(&rect,x1,y1,x1+LeftWidth,y2); // clear left area
   ExtTextOut(hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);

   SetRect(&rect,x2-RightWidth,y1,x2,y2); // clear right area
   ExtTextOut(hDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);

   if (screen) SetBkColor(hDC,OldBkColor);

   // create white pen if necessary
   if (cell[CellId].BorderColor[BORDER_INDEX_TOP]!=CLR_AUTO) {
      if (TempWhitePen) DeleteObject(hWhitePen);
      if (TempBlackPen) DeleteObject(hBlackPen);
      TempWhitePen=TempBlackPen=FALSE;

      hWhitePen=CreatePen(PS_SOLID,0,cell[CellId].BorderColor[BORDER_INDEX_TOP]);
      hBlackPen=CreatePen(PS_SOLID,0,cell[CellId].BorderColor[BORDER_INDEX_TOP]);
      TempWhitePen=TempBlackPen=TRUE;
   } 
   else if (screen && PageColor(w)==0xFFFFFF) {   // create a shaded color pen
      if (TempWhitePen) DeleteObject(hWhitePen);
      if (TempBlackPen) DeleteObject(hBlackPen);
      TempWhitePen=TempBlackPen=FALSE;
      hWhitePen=CreatePen(PS_SOLID,0,0xAFAFAF);
      TempWhitePen=TRUE;
   }

   // first draw the table enclosure lines
   if (DrawTopLine) {
      DrawShadowLine(w,hDC,x1,y1,x2-one,y1,hWhitePen,NULL);
      y1=y1+three;
   }
   if (DrawBotLine) {
      DrawShadowLine(w,hDC,x1,y2-one,x2-one,y2-one,hBlackPen,NULL);
      y2=y2-one;
   }
   if (DrawLeftLine) {
      DrawShadowLine(w,hDC,x1,y1-two,x1,y2,hWhitePen,NULL);
      x1=x1+three;
   }
   if (DrawRightLine) {
      DrawShadowLine(w,hDC,x2-one,y1-two,x2-one,y2,hBlackPen,NULL);
      x2=x2-one;
   }

   // now draw the cell borders
   if (DrawCellInside) {
      // draw the top dark line
      if (!PartTopRow) DrawShadowLine(w,hDC,x1,y1,x2-three,y1,hBlackPen,NULL);

      // draw the left dark line
      DrawShadowLine(w,hDC,x1,y1,x1,y2-three,hBlackPen,NULL);

      // draw the white bottom line
      if (!PartBotRow) DrawShadowLine(w,hDC,x1+one,y2-three,x2-three,y2-three,hWhitePen,NULL);  // leave a blank line below

      // draw the right white line
      DrawShadowLine(w,hDC,x2-three,y1+one,x2-three,y2-three,hWhitePen,NULL);
   }

   if (TempWhitePen) DeleteObject(hWhitePen);
   if (TempBlackPen) DeleteObject(hBlackPen);
   
   if (!screen) {
      DeleteObject(hBlackPen);
      DeleteObject(hWhitePen);
   } 

   return TRUE;
}

/******************************************************************************
   DrawMouseStopMsg:
   Draw the message at mouse stop message
*******************************************************************************/
BOOL DrawMouseStopMsg(PTERWND w,HDC hDC, LPBYTE text)
{
   int BoxHeight,BoxWidth,OldBkMode;
   HPEN hWhitePen=GetStockObject(WHITE_PEN);
   HPEN hBlackPen=GetStockObject(BLACK_PEN);
   BOOL TempPen=FALSE,BoxOnText=false;
   HFONT hOldFont=NULL;
   HBRUSH hBr;
   SIZE size;
   RECT rect;
   int margin=5,y;
   LPBYTE pText[10];   // show up to 10 strings
   int i,len,StrCount=0;

   if (InPrintPreview) return true;

   hOldFont=SelectObject(hDC,hRulerFontBold);
   OldBkMode=SetBkMode(hDC,TRANSPARENT);

   // find the number of strings
   len=lstrlen(text);
   pText[0]=text;
   for (i=0;i<len;i++) {
      if (text[i]=='\n') {
         text[i]=0;   // null terminate the previous string
         StrCount++;
         if (StrCount>=10) break;  // max reached
         pText[StrCount]=&(text[i+1]);
      } 
   }
   if (i==len) StrCount++; 

   // get the box dimensions
   BoxHeight=RulerFontHeight*StrCount + RulerFontHeight/2;  // add some margin space for top and bottom
   if (BoxHeight>=TerWinHeight) goto END;   // window too small
   
   BoxWidth=0;
   for (i=0;i<StrCount;i++) {
      SelectObject(hDC,True(i%2)?hRulerFont:hRulerFontBold);  // alternal between bold and regular font
      GetTextExtentPoint(hDC,pText[i],strlen(pText[i]),&size);          
      if (size.cx>BoxWidth) BoxWidth=size.cx;  // not subject to zooming
   }
   
   BoxWidth+=margin*2;          // add left/right margin space
   if (BoxWidth>=TerWinWidth) goto END;

   // show the box
   rect.top=TerWinOrgY+(MouseStopY-TerWinRect.top-BoxHeight-10);  // display 10 pixels above the MouseStopY position
   if (rect.top<=TerWinOrgY) {
      rect.top=TerWinOrgY+1;
      BoxOnText=true;
   }
   rect.bottom=rect.top+BoxHeight;
   if (rect.bottom>=(TerWinOrgY+TerWinHeight)) {
      rect.bottom=TerWinOrgY+TerWinHeight-1;
      rect.top=rect.bottom-BoxHeight;
      BoxOnText=true;     // box is overlapping the text in the vertical direction
   }
   
   rect.left=TerWinOrgX+(MouseStopX-TerWinRect.left-BoxWidth/2);  // position centered
   if (BoxOnText) {      // try to place the box on the right or left of the text
      if ((MouseStopX-TerWinRect.left)<TerWinWidth/2) 
            rect.left+=(BoxWidth/2 + 2*margin);
      else  rect.left-=(BoxWidth/2 + 2*margin);
   }

   if (rect.left<TerWinOrgX) rect.left=TerWinOrgX;
   rect.right=rect.left+BoxWidth;
   if (rect.right>=(TerWinOrgX+TerWinWidth)) {
      rect.right=TerWinOrgX+TerWinWidth-1;
      rect.left=rect.right-BoxWidth;
   } 

   // display the page number
   if (PageColor(w)==0xFFFFFF) {       // create a shaded color pen
      hWhitePen=CreatePen(PS_SOLID,0,0xAFAFAF);
      TempPen=TRUE;
   }
   // draw the rectangle
   hBr=CreateSolidBrush(RGB(255,255,225));  // light yellow
   FillRect(hDC,&rect,hBr);
   DeleteObject(hBr);

   DrawShadowBox(w,hDC,rect.left,rect.top,rect.right,rect.bottom,hWhitePen,hBlackPen);

   // write the page number
   y=rect.top+RulerFontHeight/4;
   for (i=0;i<StrCount;i++) {
     SelectObject(hDC,True(i%2)?hRulerFont:hRulerFontBold);  // alternal between bold and regular font
     TextOut(hDC,rect.left+margin,y,pText[i],lstrlen(pText[i]));
     y+=RulerFontHeight;
   }
   
   END:
   hOldFont=SelectObject(hDC,hOldFont);
   OldBkMode=SetBkMode(hDC,OldBkMode);

   if (TempPen) DeleteObject(hWhitePen);
   
   MouseStopMsgOn=true;         // mouse stop message display

   return true;
}
 
/******************************************************************************
   DrawPageBox:
   Draw a box containg the page number that corresponds to the current
   vertical scrollbar thumb position.
*******************************************************************************/
DrawPageBox(PTERWND w,HDC hDC,int pos)
{
   int NewPage,BoxHeight,BoxWidth,OldBkMode;
   HPEN hWhitePen=GetStockObject(WHITE_PEN);
   HPEN hBlackPen=GetStockObject(BLACK_PEN);
   BOOL TempPen=FALSE;
   BYTE string[20];
   HFONT hOldFont=NULL;
   int  ScrollRange=SCROLL_RANGE-VerThumbSize;
   long TotalY,PageAbsY,height;
   HBRUSH hBr;

   if (!PagesShowing || TotalPages<1) return FALSE;
   if (!PageBoxShowing && pos==VerScrollPos) return FALSE;  // position not changed

   // find the current page at the thumb position
   TotalY=SumPageScrHeight(w,0,TotalPages)-TerWinHeight;
   PageAbsY=MulDiv(TotalY,pos,ScrollRange);
   height=0;
   for (NewPage=0;NewPage<(TotalPages-1);NewPage++) {
      height+=GetScrPageHt(w,NewPage);
      if (height>PageAbsY) break;
   }


   BoxHeight=RulerFontHeight*3/2;
   BoxWidth=TwipsToOrigScrX(PAGE_BOX_WIDTH);  // not subject to zooming

   // show the box
   if (!PageBoxShowing) {    // calculate the page box rectangle
      PageBoxRect.top=TerWinOrgY+(int)(pos*(long)TerWinHeight/SCROLL_RANGE);
      if (PageBoxRect.top<=TerWinOrgY) PageBoxRect.top=TerWinOrgY+1;
      if (TerWinHeight<=BoxHeight) return FALSE;   // window too small
      PageBoxRect.bottom=PageBoxRect.top+BoxHeight;
      if (PageBoxRect.bottom>=(TerWinOrgY+TerWinHeight)) {
         PageBoxRect.bottom=TerWinOrgY+TerWinHeight-1;
         PageBoxRect.top=PageBoxRect.bottom-BoxHeight;
      }
      PageBoxRect.right=TerWinOrgX+TerWinWidth-BoxWidth/8;
      PageBoxRect.left=PageBoxRect.right-BoxWidth;
      if (PageBoxRect.left<TerWinOrgX) return FALSE;      // window too small
      BoxPage=-1;                                  // reset the box page number
      PageBoxShowing=TRUE;
   }

   // display the page number
   if (NewPage!=BoxPage) {
      if (PageColor(w)==0xFFFFFF) {       // create a shaded color pen
         hWhitePen=CreatePen(PS_SOLID,0,0xAFAFAF);
         TempPen=TRUE;
      }
      // draw the rectangle
      hBr=CreateSolidBrush(RGB(255,255,225));  // light yellow
      FillRect(hDC,&PageBoxRect,hBr);
      DeleteObject(hBr);

      DrawShadowBox(w,hDC,PageBoxRect.left,PageBoxRect.top,PageBoxRect.right,PageBoxRect.bottom,hWhitePen,hBlackPen);

      // write the page number
      if (TerFlags2&TFLAG2_SHOW_SECT_PAGE_NO) wsprintf(string,"%s%d",MsgString[MSG_PAGE_BOX],PageInfo[NewPage].DispNbr);
      else                                    wsprintf(string,"%s%d",MsgString[MSG_PAGE_BOX],NewPage+1);
      hOldFont=SelectObject(hDC,hRulerFont);
      OldBkMode=SetBkMode(hDC,TRANSPARENT);
      TextOut(hDC,PageBoxRect.left+BoxWidth/8,PageBoxRect.top+RulerFontHeight/4,string,lstrlen(string));
      hOldFont=SelectObject(hDC,hOldFont);
      OldBkMode=SetBkMode(hDC,OldBkMode);

      BoxPage=NewPage;
   }

   if (TempPen) DeleteObject(hWhitePen);

   return TRUE;
}

/******************************************************************************
   DrawShadowBox:
   Draw a box where the top and left sides are drawn with the solid pen, and
   the right and the bottom sides are drawn with the shadow pen
*******************************************************************************/
DrawShadowBox(PTERWND w,HDC hDC,int x1,int y1,int x2,int y2,HPEN hSolidPen,HPEN hShadowPen)
{
    DrawShadowLine(w,hDC,x1,y1,x2,y1,hSolidPen,NULL);   // top line
    DrawShadowLine(w,hDC,x1,y2,x2,y2,hShadowPen,NULL);  // bottom line
    DrawShadowLine(w,hDC,x1,y1,x1,y2,hSolidPen,NULL);   // left line
    DrawShadowLine(w,hDC,x2,y1,x2,y2,hShadowPen,NULL);  // right line

    return TRUE;
}

/******************************************************************************
   DrawShadowLine:
   Draw a line between two point with a shadow. The shadow line is drawn
   shifted right/bottom.
*******************************************************************************/
DrawShadowLine(PTERWND w,HDC hDC,int x1,int y1,int x2,int y2,HPEN hSolidPen,HPEN hShadowPen)
{
   HPEN hOldPen;

     
   hOldPen=SelectObject(hDC,hSolidPen); // draw solid line
   OurMoveToEx(w,hDC,x1,y1,NULL);
   OurLineTo(w,hDC,x2,y2);

   if (hShadowPen) {
      SelectObject(hDC,hShadowPen);         // draw shadow line
      if (hSolidPen==hShadowPen) { // simply draw a thick line
        if (x1==x2) x1=x2=x1+1;    // horizontal line
        if (y1==y2) y1=y2=y1+1;    // vertical line
      }
      else {
        x1++;                                    // create shadow by shifting
        x2++;
        y1++;
        y2++;
      }
      OurMoveToEx(w,hDC,x1,y1,NULL);
      OurLineTo(w,hDC,x2,y2);
   }

   SelectObject(hDC,hOldPen);           // select previous pen

   return TRUE;
}

/******************************************************************************
   DrawDottedLine:
   Draw a dotted line.
*******************************************************************************/
BOOL DrawDottedLine(PTERWND w,HDC hDC,int x1,int y1, int x2, int y2,COLORREF color)
{
   int x,y,delta=3,DotsPerInch=40;
   BOOL HorzLine=TRUE;
   HPEN hPen=GetStockObject(BLACK_PEN),hOldPen;

   if (abs(y1-y2)>abs(x1-x2)) HorzLine=FALSE;

   // adjust spacing while printing
   if (InPrinting) {
      hOldPen=SelectObject(hDC,hPen);

      if (HorzLine) {
         delta=PrtResX/DotsPerInch;
         if (delta<2) delta=2;
         for (x=x1;x<=x2;x=x+delta) {
            OurMoveToEx(w,hDC,x,y1,NULL);
            OurLineTo(w,hDC,x+delta/2,y1);
         }
      }
      else {
         delta=PrtResY/DotsPerInch;
         if (delta<2) delta=2;
         for (y=y1;y<=y2;y=y+delta) {
            OurMoveToEx(w,hDC,x1,y,NULL);
            OurLineTo(w,hDC,x1,y+delta/2);
         }
      }

      SelectObject(hDC,hOldPen);      // restore the pen
   }
   else {
      if (HorzLine) {
         if (ScrRtl) {
            x1=RtlX(w,x1,0,CurFrame,NULL);
            x2=RtlX(w,x2,0,CurFrame,NULL);
         }
         
         if (x1>x2) SwapInts(&x1,&x2);
         for (x=x1;x<=x2;x=x+delta) SetPixel(hDC,x,y1,color);
      }
      else {
         if (y1>y2) SwapInts(&y1,&y2);
         for (y=y1;y<=y2;y=y+delta) SetPixel(hDC,x1,y,color);
      }
   }

   return TRUE;
}

/******************************************************************************
   DrawWigglyLine:
   Draw wiggly line.
*******************************************************************************/
BOOL DrawWigglyLine(PTERWND w,HDC hDC,int x1,int y1, int x2, int y2,COLORREF color)
{
   int x,y,count=0,adj;
   BOOL HorzLine=TRUE;

   if ((OSCanRotate || ScrFrameAngle==0) && !ScrRtl) goto DRAW;

   if (ScrRtl) {
      x1=RtlX(w,x1,0,CurFrame,pScrSeg?(&(pScrSeg[CurScrSeg])):NULL);
      x2=RtlX(w,x2,0,CurFrame,pScrSeg?(&(pScrSeg[CurScrSeg])):NULL);
   }
    
   if (!OSCanRotate) {
      int NewX,NewY;

      NewX=FrameRotateX(w,x1,y1,CurFrame);
      NewY=FrameRotateY(w,x1,y1,CurFrame);
      x1=NewX;
      y1=NewY;
      
      NewX=FrameRotateX(w,x2,y2,CurFrame);
      NewY=FrameRotateY(w,x2,y2,CurFrame);
      x2=NewX;
      y2=NewY;
   }

   DRAW:
   if (y1!=y2) HorzLine=FALSE;

   // adjust spacing while printing
   if (HorzLine) {
      if (x1>x2) SwapInts(&x1,&x2);
      adj=0;
      for (x=x1;x<=x2;x++,count++) {
         SetPixel(hDC,x,y1+adj,color);

         adj=count%4;
         if (adj==2) adj=0;
         else if (adj==3) adj=-1;
      }
   }
   else {
      if (y1>y2) SwapInts(&y1,&y2);
      adj=0;
      for (y=y1;y<=y2;y++) {
         SetPixel(hDC,x1+adj,y,color);
         adj=count%4;
         if (adj==2) adj=0;
         else if (adj==3) adj=-1;
      }
   }

   return TRUE;
}

/******************************************************************************
   DrawLightDottedLine:
   Draw a light dotted horizontal line.
*******************************************************************************/
BOOL DrawLightDottedLine(PTERWND w,HDC hDC,int x1,int y1, int x2, int y2)
{
   int x,y;

   if (y1==y2) for (x=x1;x<=x2;x=x+4) SetPixel(hDC,x,y1,0);
   else        for (y=y1;y<=y2;y=y+4) SetPixel(hDC,x1,y,0);

   return TRUE;
}

/******************************************************************************
   DrawDarkDottedLine:
   Draw a dark dotted horizontal line.
*******************************************************************************/
BOOL DrawDarkDottedLine(PTERWND w,HDC hDC,int x1,int y1, int x2, int y2)
{
   int x,y;

   if (y1==y2) for (x=x1;x<=x2;x=x+2) SetPixel(hDC,x,y1,0);
   else        for (y=y1;y<=y2;y=y+2) SetPixel(hDC,x1,y,0);

   return TRUE;
}

/******************************************************************************
    TerDrawBorder:
    Draw the border area.
******************************************************************************/
TerDrawBorder(PTERWND w)
{
    RECT rect,BorderRect;
    int SaveId;

    if (!TerArg.BorderMargins && !InPrintPreview) return TRUE;  // nothing to clear

    SaveId=SaveDC(hTerDC);                  // save the current status of the printer device context

    SetViewportOrgEx(hTerDC,0,0,NULL);     // reset view port origins
    SetWindowOrgEx(hTerDC,0,0,NULL);       // reset window port origins
    if (hTerPal)  {
       SelectPalette(hTerDC,hTerPal,FALSE);   // select a foreground palette
       RealizePalette(hTerDC);
    }
    SetBkMode(hTerDC,OPAQUE); 
    SetBkColor(hTerDC,AdjustColor(w,TextBorderColor,TRUE));
    TerResetClipRgn(w);

    BorderRect=TerRect;

    if (InPrintPreview) {
       HideControl(w,-1);             // hide the controls
       BorderRect.top+=ToolBarHeight;
       ExtTextOut(hTerDC,0,0,ETO_OPAQUE,&BorderRect,NULL,0,NULL);
    }
    else {
       // determine the rectangle that contains the border area
       if (StatusBarHeight || ToolBarHeight || TerArg.ruler) {
          BorderRect.top=BorderRect.top+ToolBarHeight;
          if (TerArg.ruler) BorderRect.top=RulerRect.bottom;
          BorderRect.bottom-=StatusBarHeight;
       }

       // draw the top area
       if (BorderRect.top<TerWinRect.top) {
          rect=BorderRect;
          rect.bottom=TerWinRect.top;
          ExtTextOut(hTerDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
       }

       // draw the bottom area
       if (BorderRect.bottom>TerWinRect.bottom) {
          rect=BorderRect;
          rect.top=TerWinRect.bottom;
          ExtTextOut(hTerDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
       }

       // draw the left area
       if (BorderRect.left<TerWinRect.left) {
          rect=BorderRect;
          rect.right=TerWinRect.left;
          ExtTextOut(hTerDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
       }

       // draw the right area
       if (BorderRect.right>TerWinRect.right) {
          rect=BorderRect;
          rect.left=TerWinRect.right;
          ExtTextOut(hTerDC,0,0,ETO_OPAQUE,&rect,NULL,0,NULL);
       }
    }

    RestoreDC(hTerDC,SaveId);

    RedrawBorder=FALSE;

    return (LRESULT)(TRUE);
}

/******************************************************************************
    DrawEval:
    Draw the border area.
******************************************************************************/
DrawEval(PTERWND w)
{
    LPBYTE pString;
    HWND hWnd;
    BOOL html=FALSE;
    BYTE str2[100];

    if (!eval) return TRUE;

    if (w) {
        if (EvalTimerCount<0) return TRUE;
        EvalTimerCount++;
        if (EvalTimerCount<5 && (!expired)) return TRUE;
        if (True(TerArg.style&TER_DESIGN_MODE)) return TRUE;
        if (True(TerFlags6&TFLAG6_IN_TEST_MODE) && (!expired)) return TRUE;
        EvalTimerCount=-1;                // halts the timer processing
        hWnd=hTerWnd;
        html=HtmlMode;
    }
    else hWnd=NULL;

    pString=MemAlloc(2000);

    pString[0]=0;

    if (html) lstrcat(pString,"This is an evaluation version of TE v13 and HTML Add-on v8.\n");
    else      lstrcat(pString,"This is an evaluation version of TE Edit Control, v13.\n");

    if (!expired) lstrcat(pString,"License key not set! This product is now running in the demo mode.\n\n");

    lstrcat(pString,"Please visit our web site: www.subsystems.com to purchase the product.\n");
    lstrcat(pString,"\n");
    lstrcat(pString,"If you have already purchased a license for this product, please set the license \n");
    lstrcat(pString,"using the TerSetLicenseKey method.  Your license key is available in a \n");
    lstrcat(pString,"distribution file called key.txt."); 
    lstrcat(pString,"\n");
    lstrcat(pString,"\n");

    lstrcat(pString,"\n");
    lstrcat(pString,"Thank you.");

    if (EvalDaysRemaining==1)  lstrcpy(str2,"EVALUATION EXPIRING TODAY!");
    else                       wsprintf(str2,"EVALUATION DAYS REMAINING: %d",EvalDaysRemaining);
    MessageBox(hWnd,pString,expired?"EVALUATION EXPIRED!":str2,MB_OK);

    MemFree(pString);

    if (w) EvalTimerCount=0;                 // restart the timer processing

    return (!expired);
}

/******************************************************************************
   TerSetPalette:
   Build the current palette.  Set the CurColorCount to 0 if the number of
   colors in the palette is not known. The 'color' parameter is used only
   if the hCurPal is set to NULL and CurColorCount is equal to 1.  The
   background color is added if a new palette is being created.
*******************************************************************************/
HPALETTE TerSetPalette(PTERWND w,HDC hDC,HPALETTE hCurPal, int CurColorCount,COLORREF far *pColor, BOOL realize)
{
    int i,PrevColorCount,NewColorCount,BkColor=0,ColorCount;
    HPALETTE hResult=NULL;
    BOOL BkndPal=TRUE;

    // initialize the pTerPal
    if (PaintFlag==PAINT_WIN && FirstPalette && pTerPal) {
       OurFree(pTerPal);
       pTerPal=NULL;
       FirstPalette=FALSE;
       BkndPal=FALSE;         // make it a foreground palette
    }

    // working on true video and palette not disabled
    if (!Clr256Display && !(TerFlags&TFLAG_NO_PALETTE)) {  // simply call the SelectPalette function
       if (hCurPal) {
          hResult = SelectPalette(hDC,hCurPal,BkndPal);
          if (realize) RealizePalette(hDC);
       }
       return hResult;
    }

    if (TerFlags&TFLAG_NO_PALETTE) return NULL;  // palette disabled

    if (CurColorCount<=0 && hCurPal) CurColorCount=256;   // extact upto 256 colors
    if (hCurPal && pTerPal && pTerPal->palNumEntries>1000) return NULL;

    if (pTerPal) {                 //expand existing palette
       PrevColorCount=pTerPal->palNumEntries;
       pTerPal=OurRealloc(pTerPal,sizeof(LOGPALETTE)+(CurColorCount+PrevColorCount)*sizeof(PALETTEENTRY));
    }
    else {                         // create new palette
       if (PageColor(w)!=CLR_WHITE) BkColor=2;  // reserve a color the background color
       if (pTerPal=OurAlloc(sizeof(LOGPALETTE)+(CurColorCount+BkColor+TotalBaseColors)*sizeof(PALETTEENTRY))) {
          pTerPal->palVersion=0x300;
          pTerPal->palNumEntries=0;
          PrevColorCount=0;
       }
    }
    ColorCount=PrevColorCount;

    if (pTerPal) {               // update this palette table
       if (BkColor) {            // set the background color as well
          COLORREF InvColor=(~PageColor(w))&0xFFFFFF;

          // set the background color
          pTerPal->palPalEntry[ColorCount].peRed=GetRValue(PageColor(w));
          pTerPal->palPalEntry[ColorCount].peBlue=GetBValue(PageColor(w));
          pTerPal->palPalEntry[ColorCount].peGreen=GetGValue(PageColor(w));
          pTerPal->palPalEntry[ColorCount].peFlags=0;
          ColorCount++;

          // set the inverse color for the cursor
          pTerPal->palPalEntry[ColorCount].peRed=GetRValue(InvColor);
          pTerPal->palPalEntry[ColorCount].peBlue=GetBValue(InvColor);
          pTerPal->palPalEntry[ColorCount].peGreen=GetGValue(InvColor);
          pTerPal->palPalEntry[ColorCount].peFlags=0;
          ColorCount++;
       }
       if (PrevColorCount==0) {  // new palette
          for (i=0;i<TotalBaseColors;i++) {
             pTerPal->palPalEntry[ColorCount].peRed=GetRValue(PalColor[i]);
             pTerPal->palPalEntry[ColorCount].peBlue=GetBValue(PalColor[i]);
             pTerPal->palPalEntry[ColorCount].peGreen=GetGValue(PalColor[i]);
             pTerPal->palPalEntry[ColorCount].peFlags=0;
             ColorCount++;
          }
       }
       // add the colors from the specified palette
       if (hCurPal) {
          NewColorCount=GetPaletteEntries(hCurPal,0,CurColorCount,&(pTerPal->palPalEntry[ColorCount]));
          ColorCount+=NewColorCount;
       }
       // add the specified color
       else  if (CurColorCount==1 && pColor) {  // check if the given color needs to be added
          COLORREF color=(*pColor);
          for (i=0;i<ColorCount;i++) {
             if (color==RGB(pTerPal->palPalEntry[i].peRed,pTerPal->palPalEntry[i].peGreen,pTerPal->palPalEntry[i].peBlue)) break;
          }
          if (i==ColorCount) {
             pTerPal->palPalEntry[ColorCount].peRed=GetRValue(color);
             pTerPal->palPalEntry[ColorCount].peBlue=GetBValue(color);
             pTerPal->palPalEntry[ColorCount].peGreen=GetGValue(color);
             pTerPal->palPalEntry[ColorCount].peFlags=0;
             ColorCount++;
          }
          (*pColor)=PALETTERGB(GetRValue(color),GetGValue(color),GetBValue(color));  // use the palette for this color
       }

       if (ColorCount>PrevColorCount) {
          pTerPal->palNumEntries=ColorCount;
          if (hTerPal) {         // deselect and delete
             SelectPalette(hDC,GetStockObject(DEFAULT_PALETTE),TRUE);
             if (hTerDC!=hDC) SelectPalette(hTerDC,GetStockObject(DEFAULT_PALETTE),TRUE);
             DeleteObject(hTerPal);
          }
          hTerPal=CreatePalette(pTerPal);
          if (hTerPal)  {
             hResult = SelectPalette(hDC,hTerPal,BkndPal);
             if (realize) RealizePalette(hDC);
             return hResult;
          }
       }
    }

    return NULL;
}
#pragma optimize("",off)  // restore optimization

/****************************************************************************
    TerGetBaseLineY:
    Get base line y position for any line on the screen.
*****************************************************************************/
int WINAPI _export TerGetBaseLineY(HWND hWnd, int rel, long line)
{
    PTERWND w;
    int y,adj,height,BaseHeight,ExtLead,frm;
    POINT pt;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
    if (rel!=REL_SCREEN && rel!=REL_WINDOW && rel!=REL_TEXT_BOX && rel!=REL_TEXT_ORIG) return FALSE;

    // get line and column number
    if (line<0) {         // use the current position
       line=CurLine;
    }

    // *** get the pixel location
    y=LineToUnits(w,line);

    height=GetLineHeight(w,line,&BaseHeight,&ExtLead);
    adj=ScrLineHeight(w,line,FALSE,FALSE)-height;
    if (adj<-1) adj=-1;  // same adjust as for ScrYOffset in ter1.c

    y+=BaseHeight+adj;

    frm = GetFrame(w, line);
    if (frm>=0) y+=frame[frm].SpaceTop;

    //DrawShadowLine(w,hTerDC,0,y,400,y,GetStockObject(BLACK_PEN),NULL);   // test code

    if (rel==REL_TEXT_ORIG) return y;   // relative to the frame set


    y-=TerWinOrgY;

    // adjust for relative offset to the window client area
    if (rel==REL_WINDOW || rel==REL_SCREEN) {
       y+=TerWinRect.top;
    }

    // translate to the screen units
    if (rel==REL_SCREEN) {
       pt.x=0;
       pt.y=y;
       ClientToScreen(hTerWnd,&pt);
       y=pt.y;
    }
 
    return y;
} 
