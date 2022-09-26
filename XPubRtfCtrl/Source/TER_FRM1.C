/*==============================================================================
   TER_FRM1.C
   TER more Frame and drawing object functions

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
    FrameEmptyCells:
    Frame empty cells in the partial page top and bottom rows.
*******************************************************************************/
FrameEmptyCells(PTERWND w, int row,LPINT pCellFramed,int NextCell,LPINT pX,int y,int height,LPINT pFrameNo,int sect,int PageNo)
{
    int i,CellFramed,FrameNo,CurCell,LeftWidth,RightWidth,TopWidth,BotWidth,x;
    COLORREF BorderColor[4];

    if (!(TableRow[row].flags&ROWFLAG_SPLIT)) return TRUE;
    if (PageNo<TableAux[row].FirstPage || PageNo>TableAux[row].LastPage) return TRUE;   // not split on this page

    CellFramed=(*pCellFramed);
    FrameNo=(*pFrameNo);
    x=(*pX);


    if (CellFramed) CurCell=cell[CellFramed].NextCell;
    else            CurCell=TableRow[row].FirstCell;

    while (CurCell!=NextCell && CurCell>0) {

       frame[FrameNo].empty=TRUE;
       frame[FrameNo].y=y;
       frame[FrameNo].ScrY=PrtToScrY(y);
       frame[FrameNo].x=x;

       frame[FrameNo].width=TwipsToPrtX(cell[CurCell].width);
       frame[FrameNo].height=height;
       frame[FrameNo].ScrHeight=PrtToScrY(y+height)-PrtToScrY(y);
       frame[FrameNo].sect=sect;
       frame[FrameNo].RowId=row;
       frame[FrameNo].CellId=CurCell;  // set cell id so that frame height is adjusted properly when this frame is after a spanning cell frame
       frame[FrameNo].shading=cell[CurCell].shading; // shading percentage
       frame[FrameNo].BackColor=cell[CurCell].BackColor; // background color
       frame[FrameNo].level=cell[CurCell].level;
       
       if (cell[CurCell].flags&CFLAG_FORCE_BKND_CLR) frame[FrameNo].flags|=FRAME_FORCE_BKND_CLR;  // enforce background color even for default color
       frame[FrameNo].flags1|=FRAME1_PART_EMPTY_CELL;

       frame[FrameNo].border=GetCellFrameBorder(w,CurCell,&LeftWidth,&RightWidth,&TopWidth,&BotWidth,PageNo, BorderColor);
       frame[FrameNo].BorderWidth[BORDER_INDEX_LEFT]=LeftWidth;
       frame[FrameNo].BorderWidth[BORDER_INDEX_RIGHT]=RightWidth;
       frame[FrameNo].BorderWidth[BORDER_INDEX_TOP]=TopWidth;
       frame[FrameNo].BorderWidth[BORDER_INDEX_BOT]=BotWidth;

       for (i=0;i<4;i++) frame[FrameNo].BorderColor[i]=BorderColor[i];

       CellFramed=CurCell;
       x+=frame[FrameNo].width;
       CurCell=cell[CurCell].NextCell;

       FrameNo++;                      // advance to the next frame
       if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 4)",1);
    }

    (*pFrameNo)=FrameNo;
    (*pCellFramed)=CellFramed;
    (*pX)=x;

    return TRUE;
}

/******************************************************************************
    GetCellFrameBorder:
    Get the cell frame border widths.
*******************************************************************************/
UINT GetCellFrameBorder(PTERWND w, int CurCell, LPINT pLeftWidth,LPINT pRightWidth, LPINT pTopWidth, LPINT pBotWidth, int PageNo, COLORREF far *pColor)
{
    UINT border=cell[CurCell].border;
    int LeftWidth=0,RightWidth=0,TopWidth=0,BotWidth=0;

    // set the cell border width
    LeftWidth=GetCellFrameLeftWidth(w,CurCell,&border,pColor?(&(pColor[BORDER_INDEX_LEFT])):NULL);
    RightWidth=GetCellFrameRightWidth(w,CurCell,&border,pColor?(&(pColor[BORDER_INDEX_RIGHT])):NULL);
    TopWidth=GetCellFrameTopWidth(w,CurCell,&border,PageNo,pColor?(&(pColor[BORDER_INDEX_TOP])):NULL);
    BotWidth=GetCellFrameBotWidth(w,CurCell,&border,PageNo,pColor?(&(pColor[BORDER_INDEX_BOT])):NULL);

    if (ShowTableGridLines) {
       int row=cell[CurCell].row;
       border=border|BORDER_BOT|BORDER_RIGHT;

       if (  GetPrevCellInColumn(w,CurCell,FALSE,FALSE)==-1
          && UniformRowBorderCell(w,CurCell,FALSE)==0) border=border|BORDER_TOP;
       border|=BORDER_TOP;  // set to always draw, the drawing length will be restricted at the draw time
       if (cell[CurCell].PrevCell==-1) border=border|BORDER_LEFT;
    }

    // pass the values back
    (*pLeftWidth)=LeftWidth;
    (*pRightWidth)=RightWidth;
    (*pTopWidth)=TopWidth;
    (*pBotWidth)=BotWidth;

    return border;
}

/******************************************************************************
    CreatePageBorderBot:
    Create page border bottom frames
*******************************************************************************/
CreatePageBorderBot(PTERWND w, int PageNo, LPINT pY, LPINT pScrY, int LeftFrame, int RightFrame)
{
    int y,ScrY,FrameNo,sect;
    long line;

    y=(*pY);
    ScrY=(*pScrY);
    FrameNo=TotalFrames;

    line=PageInfo[PageNo].FirstLine; // first line of the page
    sect=GetSection(w,line);      // find the section to take the header lines form

    // create the bottom margin
    frame[FrameNo].empty=TRUE;
    frame[FrameNo].y=y;
    frame[FrameNo].ScrY=ScrY;
    frame[FrameNo].x=frame[LeftFrame].x+frame[LeftFrame].width;
    frame[FrameNo].width=frame[RightFrame].x-frame[FrameNo].x;

    frame[FrameNo].height=(int)(PrtResY*TerSect[sect].FtrMargin);  // this value will be updated later
    frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);

    y+=frame[FrameNo].height;
    ScrY+=frame[FrameNo].ScrHeight;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);

    // adjust the height of the left/right frames
    frame[LeftFrame].height=frame[RightFrame].height=y-frame[LeftFrame].y;
    frame[LeftFrame].ScrHeight=frame[RightFrame].ScrHeight=ScrY-frame[LeftFrame].ScrY;

    // create the bottom frame border
    frame[FrameNo].empty=TRUE;
    frame[FrameNo].y=y;
    frame[FrameNo].ScrY=ScrY;
    frame[FrameNo].x=0;
    frame[FrameNo].width=(int)(PrtResX*TerSect1[sect].PgWidth)+2*LeftBorderWidth;
    frame[FrameNo].LeftBorderWdth=LeftBorderWidth;

    frame[FrameNo].height=((PageNo+1)>=TotalPages)?TopBorderHeight:BotBorderHeight;
    frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);

    y+=frame[FrameNo].height;
    ScrY+=frame[FrameNo].ScrHeight;
    frame[FrameNo].flags=FRAME_RIGHTMOST|FRAME_BORDER_BOT|FRAME_DIVIDER_LINE;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);

    // pass the ending values
    (*pY)=y;
    (*pScrY)=ScrY;

    return TRUE;
}

/******************************************************************************
    CreatePageBox:
    Create page border box if required by the section
*******************************************************************************/
CreatePageBox(PTERWND w,int PageNo,int PageTopX,int PageTopY,int TopSect, int LeftFrame)
{
    int x,y,FrameNo,sect,height,width;

    y=PageTopY;
    x=PageTopX;
    FrameNo=TotalFrames;

    sect=TopSect;      // find the section to take the header lines form

    // check for the page border specification for the section
    if (!TerSect[sect].border) return TRUE;   // not page border for the section
    if (!InPrinting && !BorderShowing) return TRUE;  // the border is not shown on the screen if page-border not enabled

    PageInfo[PageNo].flags|=PAGE_BOXED;       // box around this page

    y+=TwipsToPrtY(TerSect[sect].BorderSpace[BORDER_INDEX_TOP]);
    x+=TwipsToPrtX(TerSect[sect].BorderSpace[BORDER_INDEX_LEFT]);

    // calculate height/width
    height=(int)(PrtResY*(TerSect1[sect].PgHeight))
                              -TwipsToPrtY(TerSect[sect].BorderSpace[BORDER_INDEX_TOP]+TerSect[sect].BorderSpace[BORDER_INDEX_BOT]);
    if (false && !InPrinting && !ViewPageHdrFtr) {
       //height-=(int)(PrtResY*(TerSect[sect].TopMargin-TerSect[sect].HdrMargin));  // top margin is displayed even when ViewPageHdrFtr is off, but BorderShowing is true
       height-=(int)(PrtResY*(TerSect[sect].BotMargin-TerSect[sect].FtrMargin));
    }

    width=(int)(PrtResX*TerSect1[sect].PgWidth);
    width-=TwipsToPrtX(TerSect[sect].BorderSpace[BORDER_INDEX_LEFT]+TerSect[sect].BorderSpace[BORDER_INDEX_RIGHT]);


    // create the bottom margin
    frame[FrameNo].empty=TRUE;
    frame[FrameNo].y=y;
    frame[FrameNo].ScrY=PrtToScrY(y);
    frame[FrameNo].x=x;
    frame[FrameNo].width=width;
    frame[FrameNo].height=height;
    frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);
    frame[FrameNo].sect=sect;

    frame[FrameNo].flags=FRAME_RIGHTMOST|FRAME_PAGE_BOX;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);

    return TRUE;
}

/******************************************************************************
    CreateFnoteFrame:
    Create page border bottom frames
*******************************************************************************/
CreateFnoteFrame(PTERWND w, int PageNo, LPINT pY, LPINT pScrY, int TextWidth, int TopLeftMargin, int TopRightMargin)
{
    int y,ScrY,FrameNo,sect;
    long line;

    y=(*pY);
    ScrY=(*pScrY);
    FrameNo=TotalFrames;

    line=PageInfo[PageNo].FirstLine; // first line of the page
    sect=GetSection(w,line);      // find the section to take the header lines form

    frame[FrameNo].empty=TRUE;    // treate it as an empty frame
    frame[FrameNo].y=y;           // relative to top margin
    frame[FrameNo].ScrY=ScrY;     // relative to top margin
    frame[FrameNo].PageFirstLine=PageInfo[PageNo].FirstLine;
    frame[FrameNo].PageLastLine=PageInfo[PageNo].LastLine;
    frame[FrameNo].width=TextWidth;
    frame[FrameNo].height=PageInfo[PageNo].FnoteHt;
    frame[FrameNo].ScrHeight=frame[FrameNo].TextHeight=PrtToScrY(frame[FrameNo].height);
    frame[FrameNo].sect=sect;               //section to which the frame belongs
    frame[FrameNo].flags|=FRAME_FNOTE;
    if (BorderShowing) {
       frame[FrameNo].x=LeftBorderWidth;
       frame[FrameNo].width+=(TopLeftMargin+TopRightMargin);
       frame[FrameNo].SpaceLeft=TopLeftMargin;

    }
    else {
       frame[FrameNo].x=0;
       frame[FrameNo].flags|=FRAME_RIGHTMOST;
    }

    y+=frame[FrameNo].height;
    ScrY+=frame[FrameNo].ScrHeight;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);

    // pass the ending values
    (*pY)=y;
    (*pScrY)=ScrY;

    return TRUE;
}

/******************************************************************************
    RefreshFrames:
    This function calls the CreateFrames to refresh frame sets.  This routine
    atteps to maintain the original text view.
*******************************************************************************/
RefreshFrames(PTERWND w, BOOL ForceRefresh)
{
    int FirstPage,LastPage;
    BOOL modified=(TotalLines != FrameRefreshLineCount);
    BOOL refresh;

    refresh=(modified || ForceRefresh || TotalFrames==1 || (TerOpFlags&TOFLAG_FULL_PAINT));

    if (!FrameRefreshEnabled) return TRUE;

    FirstPage=CurPage;
    LastPage=CurPage+1;

    if (CurPage>=FirstFramePage && CurPage<=LastFramePage) {
       if (FirstFramePage==LastFramePage) CreateFrames(w,FALSE,CurPage,CurPage+1);     // create the text frames
       else {                             // frame set contains 2 pages
          int WinHalfY=TerWinOrgY+(TerWinHeight/2);  // half point of the window

          if (CurPage==FirstFramePage) {
            if (WinHalfY<(FirstPageHeight/2) && CurPage>0) {
               FirstPage=CurPage-1;
               LastPage=CurPage;
               if (refresh || (FirstPage!=FirstFramePage || LastPage!=LastFramePage)) { 
                  CreateFrames(w,FALSE,FirstPage,LastPage);     // create the text frames
               }
               TerWinOrgY+=FirstPageHeight;
               SetTerWindowOrg(w);
            }
            else {
               if (refresh || (FirstPage!=FirstFramePage || LastPage!=LastFramePage)) { 
                  CreateFrames(w,FALSE,FirstPage,LastPage);     // retain the previous range
               }
            }
          }
          else {
            int SecondPageHalfY=((CurPageHeight-FirstPageHeight)/2)+FirstPageHeight;
            if (WinHalfY>SecondPageHalfY && (CurPage+1)<TotalPages) {
               TerWinOrgY-=FirstPageHeight;
               if (TerWinOrgY<0) TerWinOrgY=0;
               if (refresh || (FirstPage!=FirstFramePage || LastPage!=LastFramePage)) { 
                  CreateFrames(w,FALSE,FirstPage,LastPage);     // retain the previous range
               }
               SetTerWindowOrg(w);
            }
            else {
               FirstPage=CurPage-1;
               LastPage=CurPage; 
               if (refresh || (FirstPage!=FirstFramePage || LastPage!=LastFramePage)) { 
                   CreateFrames(w,FALSE,FirstPage,LastPage);   // retain the previous range
               }
            }
          }
       }
    }
    else CreateFrames(w,FALSE,CurPage,CurPage+1);     // create the text frames

    FrameRefreshLineCount=TotalLines;    // frames refreshed at this line count

    return TRUE;
}


/******************************************************************************
    CreatePictFrames:
    Create the picture frames.
*******************************************************************************/
CreatePictFrames(PTERWND w,int PageNo,int HiddenY,int HdrMargin,int TopLeftMargin,int HdrHeight,int TopSect, long FirstLine, long LastLine, UINT InHdrFtr)
{
    long l;
    int  FrameNo,i,pict,ParaFID,CurFID;
    LPWORD fmt;

    if (FirstLine>=TotalLines) FirstLine=TotalLines-1;    // this can happen when the window width is enlarged and pagination has not taken place
    if (LastLine>=TotalLines) LastLine=TotalLines-1;    // this can happen when the window width is enlarged and pagination has not taken place

    for (l=FirstLine;l<=LastLine;l++) {  // scan each line for the picture frames
       if (!(LineFlags(l)&LFLAG_PICT)) continue;
       if (LinePage(l)!=PageNo && InHdrFtr==0) continue;

       if ((PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)!=InHdrFtr) continue;

       // check if any parent frame is already positioned
       CurFID=fid(l);
       if (CurFID && !(ParaFrame[CurFID].flags&PARA_FRAME_POSITIONED)) continue;  // parent frame not yet positioned

       fmt=OpenCfmt(w,l);

       for (i=0;i<LineLen(l);i++) {
          pict=fmt[i];
          if (!(TerFont[pict].InUse) || !(TerFont[pict].style&PICT) || TerFont[pict].FrameType==PFRAME_NONE || TerFont[pict].ParaFID==0) continue;

          ParaFID=TerFont[pict].ParaFID;

          // initialize the new frame
          FrameNo=TotalFrames;
          frame[FrameNo].empty=FALSE;
          frame[FrameNo].sect=GetSection(w,l);     // section to which the frame belongs
          frame[FrameNo].PageFirstLine=l;
          frame[FrameNo].PageLastLine=l;
          frame[FrameNo].ParaFrameId=ParaFID;
          frame[FrameNo].flags|=FRAME_PICT;
          if (TerFont[pict].PictType==PICT_SHAPE) frame[FrameNo].flags|=FRAME_NON_TEXT_DO;
          frame[FrameNo].ZOrder=ParaFrame[ParaFID].ZOrder;

          // set the width and height
          frame[FrameNo].width=TwipsToPrtX(ParaFrame[ParaFID].width);
          frame[FrameNo].SpaceLeft=TwipsToPrtX(ParaFrame[ParaFID].margin);
          frame[FrameNo].SpaceRight=TwipsToPrtX(ParaFrame[ParaFID].margin);

          frame[FrameNo].SpaceTop=TwipsToPrtY(ParaFrame[ParaFID].margin);
          frame[FrameNo].SpaceBot=TwipsToPrtY(ParaFrame[ParaFID].margin);
          frame[FrameNo].height=TwipsToPrtY(ParaFrame[ParaFID].height);
          frame[FrameNo].ScrHeight=TwipsToScrY(ParaFrame[ParaFID].height);
          frame[FrameNo].TextHeight=frame[FrameNo].ScrHeight;
          
          // set the location
          frame[FrameNo].x=TwipsToPrtX(ParaFrame[ParaFID].x);
          if (BorderShowing) frame[FrameNo].x+=(LeftBorderWidth+TopLeftMargin);

          SetParaFrameY(w,ParaFID,FrameNo,LineY(l),HiddenY,HdrMargin,HdrHeight,TopSect,l);

          // initialize the text frame
          ContainsParaFrames=TRUE;
          TotalFrames++;
          InitFrame(w,TotalFrames);
       }
       CloseCfmt(w,l);
    }

    return TRUE;
}

/******************************************************************************
    PosPictFrames:
    Position the picture frames contained in a given line.
    CurX is relative to the left margin.  The CurY is relative to the top
    of the frame page.
*******************************************************************************/
PosPictFrames(PTERWND w,long LineNo, int CurX, int CurY, int HiddenY, int HdrMargin, int HdrHeight,int TopSect, int PageNo)
{
   LPWORD fmt;
   int pict;
   BOOL result=FALSE,align;
   int  x,y,i,ParaFID,WrapWidth,CurCell;
   UINT border;
   int CurFID,PrevLeftPictWidth=0,PrevRightPictWidth=0;
   BOOL PictAftText=FALSE;
   LPWORD  LineCharWidth=null;        // Line character width

   if (!(LineFlags(LineNo)&LFLAG_PICT)) return FALSE;

   fmt=OpenCfmt(w,LineNo);

   // check if any enclosing paraframe is already positioned
   CurFID=fid(LineNo);
   if (CurFID && !(ParaFrame[CurFID].flags&PARA_FRAME_POSITIONED)) return FALSE;  // parent frame not yet positioned

   // reset the PARA_FRAME_POSITIONED flags
   for (i=0;i<LineLen(LineNo);i++) {
      pict=fmt[i];
      if (!(TerFont[pict].InUse) || !(TerFont[pict].style&PICT) || TerFont[pict].FrameType==PFRAME_NONE || TerFont[pict].ParaFID==0) {
         continue;
      }
      ParaFID=TerFont[pict].ParaFID;
      ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_POSITIONED); 
   }

   // position each frame
   for (i=0;i<LineLen(LineNo);i++) {
      pict=fmt[i];
      if (!(TerFont[pict].InUse) || !(TerFont[pict].style&PICT) || TerFont[pict].FrameType==PFRAME_NONE || TerFont[pict].ParaFID==0) {
         continue;
         PictAftText=TRUE;  // some text found
      }

      ParaFID=TerFont[pict].ParaFID;
      
      
      align=(TerFont[pict].FrameType!=PFRAME_FLOAT);

      // set the frame width and height
      if (TerFont[pict].PictType!=PICT_SHAPE) {
         ParaFrame[ParaFID].width=TerFont[pict].PictWidth+2*ParaFrame[ParaFID].margin;
         ParaFrame[ParaFID].height=TerFont[pict].PictHeight+2*ParaFrame[ParaFID].margin;
         ParaFrame[ParaFID].MinHeight=ParaFrame[ParaFID].height;
      }
      ParaFrame[ParaFID].TextLine=LineNo;
      ParaFrame[ParaFID].CellId=cid(LineNo);


      // adjust CurX/Y when frame is enclosed in a table
      y=CurY;                         // relative to the top of the frame page

      if (HtmlMode) {                 // check if the picture is in the first position
        //int CurFont=GetCurCfmt(w,LineNo,0);
        //if (CurFont!=pict) y+=(LineHt(LineNo)+TwipsToPrtY(50));
        if (PictAftText) y+=(LineHt(LineNo)+TwipsToPrtY(50));
      }
      if (align) {                    // left/right aligned frame
         x=PrtToTwipsX(CurX);         // relative to the left margin
         if (cid(LineNo)>0) {
            CurCell=cid(LineNo);
            border=cell[CurCell].border;
            x+=cell[CurCell].margin;

            y+=TwipsToPrtY(GetCellFrameTopWidth(w,CurCell,&border,PageNo,NULL));

         }

         // adjust CurX when the frame is aligned right
         if (TerFont[pict].FrameType==PFRAME_RIGHT) {
            WrapWidth=ScrToTwipsX(TerWrapWidth(w,LineNo,-1));
            x=x+WrapWidth-ParaFrame[ParaFID].width;
            x-=PrevRightPictWidth;                             // offset for other pictures on the line
         }
         else x+=PrevLeftPictWidth;

         // set the frame location
         ParaFrame[ParaFID].x=x;  // x relative to the left margin
         ParaFrame[ParaFID].ParaY=0;  // treat it as a para frame
         if (!(LineFlags(LineNo)&LFLAG_PARA_FIRST)) ParaFrame[ParaFID].ParaY=PrtToTwipsY(LineHt(LineNo));
         
         // keep track of width used up
         if (TerFont[pict].FrameType==PFRAME_RIGHT) PrevRightPictWidth+=ParaFrame[ParaFID].width;
         else                                       PrevLeftPictWidth+=ParaFrame[ParaFID].width;
      }
      else {
         x=TwipsToPrtX(ParaFrame[ParaFID].OrgX);
         
         if      (True(ParaFrame[ParaFID].flags&PARA_FRAME_IN_CELL)) x+=CurX;   // relative to the cell
         else if (True(ParaFrame[ParaFID].flags&PARA_FRAME_IGNORE_X)) x+=CurX;  // default for ingore-x is column relative positioning (when posrelh is missing)

         if (True(ParaFrame[ParaFID].flags&PARA_FRAME_HPAGE)) {
            x-=(int)(PrtResX*TerSect[TopSect].LeftMargin);  // convert to margin relative value
         } 

         if (false && True(ParaFrame[ParaFID].flags&PARA_FRAME_IGNORE_X)) {
            int j;

            if (LineCharWidth==null) LineCharWidth=GetLineCharWidth(w,LineNo);
            for (j=0;j<i;j++) {                  // 5/5/05
               if (InPrinting) x+=(int)LineCharWidth[j];
               else            x+=ScrToPrtX((int)LineCharWidth[j]);  // when not in printing, GetLineChawWidth returns in screen units
            }
         }   
         
         // check if this line is moved to right because of frames in the other lines
         if (True(ParaFrame[ParaFID].flags&PARA_FRAME_IGNORE_X) 
              && tabw(LineNo) && tabw(LineNo)->FrameCharPos==0 && tabw(LineNo)->FrameSpaceWidth>0) {
           int FrameX,FrameWidth,FrameHt;
           GetFrameSpace(w,LineNo,NULL,&FrameX,&FrameWidth,&FrameHt); // recalculate because tabw(l)->FrameSpaceWidth takes into account the current line frames as well
               
           if (FrameHt==0) x+=FrameWidth;  // a first frame would affect the shape object x position
         }

         if (false && True(ParaFrame[ParaFID].flags&PARA_FRAME_SHPGRP)) x+=TwipsToPrtX(ParaFrame[ParaFID].GroupX);

         ParaFrame[ParaFID].x=PrtToTwipsX(x);

         // set x relative to the current character position of the anchor character
         if (false && True(ParaFrame[ParaFID].flags&PARA_FRAME_IGNORE_Y)) {
             ParaFrame[ParaFID].ParaY=PrtToTwipsY(CurY)+ParaFrame[ParaFID].OrgY;
             if (True(ParaFrame[ParaFID].flags&PARA_FRAME_SHPGRP)) ParaFrame[ParaFID].ParaY+=ParaFrame[ParaFID].GroupY;

             if (BorderShowing) ParaFrame[ParaFID].ParaY-=PrtToTwipsY(TopBorderHeight);
             else if (ViewPageHdrFtr) ParaFrame[ParaFID].ParaY+=PrtToTwipsY(HiddenY);   // make it relative to the edge of the page
         }
      } 

      
      ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_POSITIONED);  // force repostioning in the SetParaFrameY function
      SetParaFrameY(w,ParaFID,-1,y,HiddenY,HdrMargin,HdrHeight,TopSect,LineNo);

      PageInfo[PageNo].FrameCount++;
   }
   
   CloseCfmt(w,LineNo);
   if (LineCharWidth) MemFree(LineCharWidth);


   return result;
}

/******************************************************************************
    SetParaFrameY:
    Set the Y location of a paragraph relative frame.
*******************************************************************************/
SetParaFrameY(PTERWND w,int ParaFID, int FrameNo, int CurY, int HiddenY, int HdrMargin, int HdrHeight,int TopSect, long line)
{
    int i,FrameY;

    // adjust hidden y when the top margin is less than hidden y
    if ((int)(TerSect[TopSect].TopMargin*PrtResY)<HiddenY) HiddenY=(int)(TerSect[TopSect].TopMargin*PrtResY);

    // vertical centereing
    if (True(ParaFrame[ParaFID].flags&PARA_FRAME_VCENTER)) {
       int HeightTwips;
       float height=((TerSect[TopSect].orient==DMORIENT_PORTRAIT)?TerSect[TopSect].PprHeight:TerSect[TopSect].PprWidth);
       if (True(ParaFrame[ParaFID].flags&PARA_FRAME_VMARG)) height=height-TerSect[TopSect].TopMargin-TerSect[TopSect].BotMargin;   // text area height
       else ParaFrame[ParaFID].flags|=PARA_FRAME_VPAGE;  // 20050926 - ParaY is calculate relative to the page or top-margin

       HeightTwips=(int)InchesToTwips(height);  // height of the line
       ParaFrame[ParaFID].ParaY=(HeightTwips-ParaFrame[ParaFID].height)/2;
    }

    // set the non-aligned page/margin relative frames
    if (ParaFrame[ParaFID].flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)) {
       BOOL set=false;
       if (True(ParaFrame[ParaFID].flags&PARA_FRAME_IN_CELL) && (line>=0 && line<TotalLines) && cid(line)>0) {
          int cl=cid(line);
          for (i=0;i<TotalFrames;i++) {   // look for a cell frame
             if (i==FrameNo) continue;
             if (frame[i].CellId==cl) {
                FrameY=frame[i].y+TwipsToPrtY(ParaFrame[ParaFID].ParaY);
                set=true;
                break;
             } 
          }
       }
       if (!set) { 
          int PageY=TwipsToPrtY(ParaFrame[ParaFID].ParaY);   // y relative to the top of the page
          if (ParaFrame[ParaFID].flags&PARA_FRAME_VMARG) PageY+=(int)(PrtResY*TerSect[TopSect].TopMargin);

          if (BorderShowing) FrameY=PageY+TopBorderHeight;
          else {
             if (ViewPageHdrFtr) FrameY=PageY-HiddenY;
             else                FrameY=PageY-HdrMargin-HdrHeight;
          }
          if (FrameNo>=0 && FrameNo>=FirstPage2Frame) FrameY+=FirstPageHeight;
       }

       if (FrameNo>=0) {
          frame[FrameNo].y=FrameY;
          frame[FrameNo].ScrY=PrtToScrY(FrameY);
       }

       ParaFrame[ParaFID].y=PrtToTwipsY(FrameY-FirstPageHeight);

       if (repaginating) {
          ParaFrame[ParaFID].flags|=PARA_FRAME_POSITIONED;
       }

       return TRUE;
    }

    // set the PageY if not already set
    if (!(ParaFrame[ParaFID].flags&PARA_FRAME_POSITIONED)) {
       ParaFrame[ParaFID].PageY=PrtToTwipsY(CurY)+ParaFrame[ParaFID].ParaY-PrtToTwipsY(FirstPageHeight);  // y relative to the top of the Frame page
       
       if (BorderShowing)      // calculate the y relative to the top of the physical page
          ParaFrame[ParaFID].PageY-=PrtToTwipsY(TopBorderHeight);  // value relative to the top of the page
       else {
          if (ViewPageHdrFtr) ParaFrame[ParaFID].PageY+=PrtToTwipsY(HiddenY);
          else                ParaFrame[ParaFID].PageY+=PrtToTwipsY(HdrMargin+HdrHeight);
       }
       if (repaginating) {
          ParaFrame[ParaFID].flags|=PARA_FRAME_POSITIONED;
       }

    }

    // use the previously set PageY value
    if (BorderShowing)
       FrameY=TwipsToPrtY(ParaFrame[ParaFID].PageY)+TopBorderHeight;
    else {
       if (ViewPageHdrFtr) FrameY=TwipsToPrtY(ParaFrame[ParaFID].PageY)-HiddenY;
       else                FrameY=TwipsToPrtY(ParaFrame[ParaFID].PageY)-HdrMargin-HdrHeight;
    }

    ParaFrame[ParaFID].y=PrtToTwipsY(FrameY);

    if (FrameNo>=0) {
      frame[FrameNo].y=FrameY;
      if (FrameNo>=FirstPage2Frame) frame[FrameNo].y+=FirstPageHeight;
      frame[FrameNo].ScrY=PrtToScrY(frame[FrameNo].y);
    }

    return TRUE;
}

/******************************************************************************
    DeleteFrame:
    Delete Currently highlighted frame.
******************************************************************************/
DeleteFrame(PTERWND w)
{
    long l;
    int ParaFID;
    LPBYTE msg;

    if (!FrameClicked) return TRUE;

    // is it protected in the header/footer area
    if (PfmtId[pfmt(CurLine)].flags&PAGE_HDR_FTR && !EditPageHdrFtr) {
       MessageBeep(0);
       return TRUE;
    }

    ParaFID=fid(CurLine);
    if (!ParaFID) return TRUE;        // not on a paragraph frame

    if (ParaFrame[ParaFID].flags&PARA_FRAME_OBJECT)
         msg=(LPBYTE)MsgString[MSG_DEL_DRAW_OBJ];
    else msg=(LPBYTE)MsgString[MSG_DEL_FRAME];

    if (!(TerFlags&TFLAG_RETURN_MSG_ID)) {  // when this flag is set, the host application should ask this question in PreProcess event
       if (IDNO==(MessageBox(hTerWnd,msg,MsgString[MSG_IRREVERSIBLE_DEL],MB_YESNO))) return TRUE;
    }


    // find the beginning and end of the current frame
    for (l=CurLine-1;l>=0;l--) if (fid(l)!=ParaFID) break;
    HilightBegRow=l+1;
    HilightBegCol=0;
    for (l=CurLine+1;l<TotalLines;l++) if (fid(l)!=ParaFID) break;
    HilightEndRow=l-1;
    HilightEndCol=LineLen(HilightEndRow);
    HilightType=HILIGHT_LINE;

    DeleteLineBlock(w,FALSE);     // delete line block without display the screen
    HilightType=HILIGHT_OFF;

    ParaFrame[ParaFID].InUse=FALSE;

    Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint

    return TRUE;
}

/******************************************************************************
    GetFrameSpace:
    Get the horizontal space used by paragraph frames at a particular line number.
    The values are returned in the printer units. This function returns a
    non-zero value int the 'height' variable, if the frame spans the entire
    width of the column.  The 'height' value can be used to create blank line.
    If the 'line' argument is -1, the 'rect' is used to get the area of
    interest.  'rect' is specified in screen units.
******************************************************************************/
GetFrameSpace(PTERWND w,long line, RECT far *rect, int far *FrameX, int far *FrameWidth, int far *FrameHt)
{
    if (!CalcFrameSpace(w,line,rect,FrameX,FrameWidth,FrameHt,-1,FALSE,FALSE,FALSE,-1)) return TRUE;

    if (FrameHt && (*FrameHt)>0) (*FrameHt)=(*FrameX)=(*FrameWidth)=0;  // space line disabled

    return TRUE;
}

/******************************************************************************
    CalcFrameSpace:
    Get the horizontal space used by paragraph frames at a particular line number.
    The values are returned in the printer units. This function returns a
    non-zero value int the 'height' variable, if the frame spans the entire
    width of the column.  The 'height' value can be used to create blank line.
    If the 'line' argument is -1, the 'rect' is used to get the area of
    interest.  'rect' is specified in screen units.
    Set GetRowSpace to TRUE to get the frame space before a table row.
******************************************************************************/
CalcFrameSpace(PTERWND w,long line, RECT far *rect, int far *FrameX, int far *FrameWidth, int far *FrameHt, int sect, BOOL GetRowSpace, BOOL GetLineSpace, BOOL GetRowIndent, int PageNo)
{
    int i,y,x1,x2,y1,y2,BeginX,EndX,MinMarginDist,WrapWidth;
    int tol,ColX,LineHeight,BeginY,EndY,LineBegY,LineEndY,LineCID=0,FrameCID;
    BOOL first=TRUE,NoWrapFrameFound=FALSE;
    int RowBeginX,RowEndX,RowWidth,InterFrameDist,WideY,columns=1,pass;
    BOOL FirstRowLine=FALSE,ScopeEnlarged,ScanCellFrames;
    DWORD InHdrFtr=0;
    long FirstLine,OrigLine=line;
    BOOL GetSpaceBef=(GetRowSpace || GetLineSpace);
    BOOL IsRtl=FALSE;
    DWORD FrameInHdrFtr;

    (*FrameX)=(*FrameWidth)=0;
    if (FrameHt) (*FrameHt)=0;

    if (!TerArg.PageMode) return FALSE;
    if (GetRowIndent && GetRowSpace) return FALSE;

    if (line>=TotalLines) line=TotalLines-1;
    if (line>=0 && fid(line)>0) return FALSE;

    ScanCellFrames=(line>=0 && cid(line)>0)?TRUE:FALSE;
    if (ScanCellFrames) LineCID=cid(line);    // cell id of the line

    if ((GetRowSpace || GetRowIndent) && !repaginating) return FALSE;
    if (GetRowSpace || GetRowIndent) ScanCellFrames=FALSE;    // do not scan the frames inside cells

    // get the line y position and PageNo number
    if (line>=0) {
       if (LineFlags(line)&LFLAG_HDRS_FTRS) return FALSE;

       y=LineY(line);
       if (y==0 && ViewPageHdrFtr) return FALSE;   // y not set yet

       if (PageNo<0) {
          int LineInHdrFtr=0;
          long l;
          PageNo=LinePage(line);     // if page number not specified than take it from the line
          if (True(PfmtId[pfmt(line)].flags&PAGE_HDR_FTR)) {  // adjust page number when the lines belongs to header/footer, but first page hdr/ftr also exist
             LineInHdrFtr=GetHdrFtrFlag(w,line);  // check if the line is a regular hdr/ftr line
             if (True(LineInHdrFtr&LFLAG_HDR_FTR)) {
                 for (l=PageInfo[PageNo].FirstLine;l<=PageInfo[PageNo].LastLine;l++) {  // check if the page also has first page hdr/ftr
                     if (l>=TotalLines || False(PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)) break;  // out of hdr/ftr
                     if (  (True(LineInHdrFtr&LFLAG_HDR) && (LineFlags(l)&LFLAG_FHDR))
                        || (True(LineInHdrFtr&LFLAG_FTR) && (LineFlags(l)&LFLAG_FFTR)) ){
                        PageNo++;        // because the current hdr/ftr line actually never display on the first page
                        break;
                     }
                 } 
             } 
          } 
       }

       LineHeight=LineHt(line);
       if (OrigLine>line) y+=LineHeight;  // tihs can happen when word-wrapping the last line

       InHdrFtr=GetHdrFtrFlag(w,line);
       IsRtl=LineFlags2(line)&LFLAG2_RTL;
    }
    else {
       y=ScrToPrtY(rect->top);
       if (PageNo<0) PageNo=CurPage;
       LineHeight=ScrToPrtY(rect->bottom-rect->top);
       IsRtl=FALSE;
    }

    if (LineHeight==0) return FALSE;

    // if in a cell, find the width of the table
    if (GetRowSpace || GetRowIndent) {
       int row=cell[cid(line)].row;
       int cl=TableRow[row].FirstCell;
       if (GetRowSpace) {
          RowBeginX=LineX(line)+TwipsToPrtX(TableRow[row].indent); // assume that 'line' corresponds to the first cell
          if (BorderShowing) RowBeginX-=GetBorderLeftSpace(w,PageNo);
       }
       else RowBeginX=LineX(line);

       RowEndX=RowBeginX;
       while (cl>0) {
          RowEndX+=TwipsToPrtX(cell[cl].width);
          cl=cell[cl].NextCell;
       }
       RowWidth=RowEndX-RowBeginX;
    }

    // set the begin x and width for the current column
    if (line>=0) {
       if (sect<0) sect=GetSection(w,line);                // get the current section number
       if (GetRowSpace || GetRowIndent) {  // Get the width of the section area
          WrapWidth=ScrToPrtX(TerWrapWidth(w,-1,sect));     // get the section width
          if (TerSect[sect].columns>1 && !(PfmtId[pfmt(line)].flags&PAGE_HDR_FTR)) {
             WrapWidth=WrapWidth-(int)(PrtResX*(TerSect[sect].ColumnSpace*(TerSect[sect].columns-1)));
             WrapWidth=WrapWidth/TerSect[sect].columns;
          }
       }
       else WrapWidth=ScrToPrtX(TerWrapWidth(w,line,sect));

       ColX=LineX(line);
       if (cid(line)) MinMarginDist=TwipsToPrtX(FrameDistFromMargin/4);  // minimum distance from the margin
       else           MinMarginDist=TwipsToPrtX(FrameDistFromMargin);  // minimum distance from the margin

       columns=TerSect[sect].columns;
    }
    else {
       WrapWidth=ScrToPrtX(rect->right-rect->left);
       ColX=ScrToPrtX(rect->left);
       MinMarginDist=0;
    }
    if (BorderShowing) ColX-=GetBorderLeftSpace(w,PageNo);  // ColX is now relative to the left margin


    // scan each paragraph frame
    pass=0;                 // do not take into account GetSpaceBef in the first pass
    
    NEXT_PASS:

    first=true;
    NoWrapFrameFound=false;
    LineBegY=WideY=y;                  // define the Y area used by the line
    LineEndY=y+LineHeight;
    InterFrameDist=TwipsToPrtY(FRAME_DIST_FROM_TEXT)*3/4;
    tol=5;                         // tolerance for y checking
    if (TerFlags&TFLAG_NO_PRINTER || !PrinterAvailable) tol=1;

    for (i=1;i<TotalParaFrames;i++) {     // default frame 0 not used
       ParaFrame[i].flags=ResetUintFlag(ParaFrame[i].flags,PARA_FRAME_SELECT);  // reset the select flag
       
       if (!ParaFrame[i].InUse) continue;
       if (True(ParaFrame[i].flags&PARA_FRAME_WATERMARK)) continue;
       if (ParaFrame[i].ShapeType==SHPTYPE_LINE || True(ParaFrame[i].flags&PARA_FRAME_LINE)) continue;

       if (repaginating && !(ParaFrame[i].flags&PARA_FRAME_POSITIONED)) continue;   // not yet positioned
       
       FirstLine=ParaFrame[i].TextLine;
       if (FirstLine>=0 && FirstLine<TotalLines) FrameInHdrFtr=GetHdrFtrFlag(w,FirstLine);
       else                                      FrameInHdrFtr=0;
       
       // skip if frame lies in the regular header/footer and the first page header/ftr exists and current page is the first page for the section
       if (ParaFrame[i].PageNo==PageNo && (FrameInHdrFtr&LFLAG_HDR_FTR)) { 
          int sect=PageInfo[PageNo].TopSect;
          if ((FrameInHdrFtr&LFLAG_HDR) && TerSect1[sect].fhdr.FirstLine>=0) continue;  // this frame start on the second page
          if ((FrameInHdrFtr&LFLAG_FTR) && TerSect1[sect].fftr.FirstLine>=0) continue;  // this frame start on the second page
       } 
       

       if (ParaFrame[i].PageNo!=PageNo) {      // check if the frame belongs to the current header/footer
          int FramePage=ParaFrame[i].PageNo;
          if (FrameInHdrFtr && FramePage>=0) {
             if (PageInfo[FramePage].TopSect!=PageInfo[PageNo].TopSect) continue;
             if ((FrameInHdrFtr&LFLAG_FHDR) || (FrameInHdrFtr&LFLAG_FFTR)) continue;  // frame in first hdr/ftr, but current PageNo is regular hdr/ftr
          }
          else continue;
       }

       if (ParaFrame[i].flags&PARA_FRAME_WRAP_THRU) continue;  // object type of frame

       // check if it is a picture frame in side a cell
       FrameCID=0;
       if (ParaFrame[i].pict>0) FrameCID=ParaFrame[i].CellId;
       if (ScanCellFrames && LineCID!=FrameCID) continue;  // the cell does not contain this frame
       if (!ScanCellFrames && FrameCID>0) continue;


       // frame in hdr/ftr?
       if (FirstLine>=0 && FirstLine<TotalLines) {
          if (FrameInHdrFtr!=InHdrFtr && GetRowSpace) continue;
          if (FrameInHdrFtr!=InHdrFtr) {    // 041108 - don't let header/footer text be affected by outside frames
            if (True(FrameInHdrFtr&LFLAG_HDRS) && True(InHdrFtr&LFLAG_FTRS)) continue;
            if (True(FrameInHdrFtr&LFLAG_FTRS) && True(InHdrFtr&LFLAG_HDRS)) continue;
          } 
          if (FrameInHdrFtr && !InHdrFtr /*&& !first*/) continue;   // let only one frame from header/footer to body
          //if (InHdrFtr && !(FrameInHdrFtr&PAGE_HDR_FTR)) continue;
          //if ((InHdrFtr&PAGE_FTR) && (FrameInHdrFtr&PAGE_HDR)) continue;
          if (InHdrFtr && !(FrameInHdrFtr&LFLAG_HDRS_FTRS)) continue;
          if ((InHdrFtr&LFLAG_FTR) && (FrameInHdrFtr&LFLAG_HDR)) continue;
          
          // text-box and rectangle shape objects in header/footer do displace hdr/footer text
          if (True(FrameInHdrFtr) && (FrameInHdrFtr==InHdrFtr)) { 
             if (ParaFrame[i].ShapeType!=0) continue;
             if (True(ParaFrame[i].flags&PARA_FRAME_OBJECT)) continue;
          }
       }


       // check the y range
       y1=TwipsToPrtY(ParaFrame[i].y);
       y2=TwipsToPrtY((ParaFrame[i].y+ParaFrame[i].height));

       if (LineEndY<=(y1+tol) || LineBegY>=(y2-tol)) continue;

       // increase the scope of LineBegY and LineEndY
       ScopeEnlarged=FALSE;
       if (GetRowSpace || (GetLineSpace && pass>0)) {
          if ((y1-InterFrameDist)<LineBegY) {
             LineBegY=y1-2*InterFrameDist;
             ScopeEnlarged=TRUE;
          }
          if ((y2+InterFrameDist)>LineEndY) {
             LineEndY=y2+2*InterFrameDist;
             ScopeEnlarged=TRUE;
          }
       }
       if (ScopeEnlarged) {
          i=0;              // restart to pickup up any previous frame that might come into scope now
          continue;
       }

       // check the x range
       x1=TwipsToPrtX(ParaFrame[i].x-ParaFrame[i].DistFromText);
       x2=TwipsToPrtX(ParaFrame[i].x+ParaFrame[i].width+ParaFrame[i].DistFromText);

       // check for no wrap
       if (ParaFrame[i].flags&PARA_FRAME_NO_WRAP) {   // expand horizontally
          if (x1>ColX) x1=ColX;
          if (x2<(ColX+WrapWidth)) x2=ColX+WrapWidth;
       }

       // when calculating for the first line of the row
       if (GetRowSpace) {
          if (x2<RowBeginX) continue;   // before the row
          if (x1>RowEndX) continue;     // after the row
       }

       // check if the frame falls in another column of a multicolumn section
       if (columns>1 &&  (x1>(ColX+WrapWidth) || (x2<ColX))) continue;
       
       // make rtl adjustment
       if (IsRtl) {           // if the line is rtl
          //int RelX=x1-ColX;   // x relative to the beginning of the column
          //int FrmWidth=x2-x1;
          //RelX=(WrapWidth-(RelX+FrmWidth));  // mapped relative X
          //x1=ColX+RelX;
          //x2=x1+FrmWidth;
          int RelX=ColX+WrapWidth-x2;   // x relative to the beginning of the column
          int FrmWidth=x2-x1;
          //RelX=(WrapWidth-RelX);       // mapped relative X
          x1=ColX+RelX;
          x2=x1+FrmWidth;
       } 

       if ((x1-MinMarginDist)<=ColX && (x2+MinMarginDist)>=(ColX+WrapWidth)) {
           NoWrapFrameFound=TRUE;
           if (y2>WideY) WideY=y2;
       }

       if (first) {                       // record frame space
          BeginX=x1;
          EndX=x2;
          BeginY=y1;
          EndY=y2;
       }
       else {
          if (x1<BeginX) BeginX=x1;
          if (x2>EndX)   EndX=x2;
          if (y1<BeginY) BeginY=y1;      // add to frame space
          if (y2>EndY)   EndY=y2;
       }

       first=FALSE;
       ParaFrame[i].flags|=PARA_FRAME_SELECT;  // selected
       if (ParaFrame[i].flags&PARA_FRAME_NO_WRAP) NoWrapFrameFound=TRUE;
    }


    if (first) return FALSE;                  // no intervening frames

    if (pass==0 && GetLineSpace) {             // now take into account GetSpaceBef
       BOOL DoNextPass=((EndX+MinMarginDist)>=WrapWidth);    // entire width used up
       if (DoNextPass) {
         pass++;
         goto NEXT_PASS;
       }
    } 

    // check if the frames can be fit by moving a table toward right
    if (GetRowSpace) {
       if ((EndX+RowWidth+MinMarginDist/4)<WrapWidth) return FALSE;
    }


    // calculate x relative to column
    BeginX-=ColX;
    if (BeginX<0) BeginX=0;
    EndX-=ColX;
    if (EndX<BeginX) EndX=BeginX;

    if (EndX>WrapWidth) EndX=WrapWidth;
    if (BeginX<MinMarginDist) BeginX=0;
    if ((EndX+MinMarginDist)>WrapWidth) EndX=WrapWidth;

    // adjust EndX
    if (line>=0 && BeginX==0) {   // ensure that atleast one character fits in the line
       if ((EndX+TerFont[0].CharWidth['W'])>=WrapWidth) EndX=WrapWidth;
    } 

    *FrameX=BeginX;
    *FrameWidth=EndX-BeginX;

    if ((BeginX==0 && EndX>=WrapWidth) || NoWrapFrameFound || GetRowSpace) {
       if (NoWrapFrameFound) EndY=WideY;
       if (FrameHt && EndY>y) {
          int HtTol=LineHeight/3;    // this tolerance value must be within the tolerance used in the PageResized function for page height calculations to work properly
          if (HtTol>100) HtTol=100;
          if (!GetRowSpace) HtTol=HtTol/2;  
          (*FrameHt)=EndY-y+TwipsToPrtY(HtTol);
       }
    }
    
    return TRUE;
}

/******************************************************************************
    CalcFrmIndentBefRow:
    If a frame space exists before a table row before the first cell, the function
    calculates and returns the space before the first column.
******************************************************************************/
CalcFrmIndentBefRow(PTERWND w,long line, int sect)
{
    int FrameX,FrameWidth,FrameHt;
    int height=0;

    if (line<0) return 0;
    if (CalcFrameSpace(w,line,NULL,&FrameX,&FrameWidth,&FrameHt,sect,FALSE,FALSE,TRUE,-1)) {
       if (FrameHt==0 && FrameX==0) return FrameWidth;
    }

    return 0;
}

/******************************************************************************
    CalcFrmSpcBefRow:
    If a frame space exists before a table row, the function calculates and
    returns the frame space before the line.
******************************************************************************/
CalcFrmSpcBefRow(PTERWND w,long line, int sect)
{
    int FrameX,FrameWidth,FrameHt;
    int height=0;

    if (line<0) return 0;
    if (CalcFrameSpace(w,line,NULL,&FrameX,&FrameWidth,&FrameHt,sect,TRUE,FALSE,FALSE,-1)) height=FrameHt;

    return height;
}

/******************************************************************************
    CalcFrmSpcBef:
    If a frame space exists before a line, the function calculates and
    returns the frame space before the line.
    This function also sets the frame space if requested.  The space
    is returned in the printer units units. if 'sect' is -1, it is calculated.
******************************************************************************/
CalcFrmSpcBef(PTERWND w,long line, int sect, BOOL set, int PageNo)
{
    int FrameX,FrameWidth,FrameHt;
    int height=0;

    if (line<0) return 0;
    if (CalcFrameSpace(w,line,NULL,&FrameX,&FrameWidth,&FrameHt,sect,FALSE,TRUE,FALSE,PageNo)) height=FrameHt;

    if (set) {
      if (height>0) {                   // set the result in the tabw structure
         if (!tabw(line) && AllocTabw(w,line)) tabw(line)->section=sect;
         if (tabw(line)) {
            tabw(line)->type|=INFO_FRM_SPC_BEF;
            tabw(line)->height=height;
         }
      }
      else if (tabw(line)) {
        tabw(line)->type=ResetUintFlag(tabw(line)->type,INFO_FRM_SPC_BEF);
        tabw(line)->height=0;
      }
    }

    return height;
}

/******************************************************************************
    GetFrmSpcBef:
    Get the previously calculated frame space before this line.
******************************************************************************/
GetFrmSpcBef(PTERWND w,long line, BOOL InScrUnits)
{
    int height=0;

    if (!TerArg.PageMode) return 0;
    if (line<0 || line>=TotalLines || fid(line)) return 0;
    if (tabw(line) && tabw(line)->type&INFO_FRM_SPC_BEF) height=tabw(line)->height;
    if (InScrUnits && height) height=PrtToScrY(height);
    return height;
}

/******************************************************************************
    TerGetPageBorderDim:
    Get the dimensions of the page border in twips
******************************************************************************/
BOOL WINAPI _export TerGetPageBorderDim(HWND hWnd, LPINT pWidth, LPINT pHeight)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (pWidth)  (*pWidth)=PrtToTwipsX(LeftBorderWidth);
    if (pHeight) (*pHeight)=PrtToTwipsX(TopBorderHeight);

    return TRUE;
}

/******************************************************************************
    TerGetDrawObjectInfo:
    Retrieve drawing object information.
******************************************************************************/
BOOL WINAPI _export TerGetDrawObjectInfo(HWND hWnd, int FrameId, LPINT width, LPINT height, LPINT LineWdth, COLORREF far *LineColor, COLORREF far *BackColor,UINT far *flags)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (FrameId<0 || FrameId>=TotalParaFrames || !ParaFrame[FrameId].InUse) return FALSE;

    if (width) (*width)=ParaFrame[FrameId].width;
    if (height) (*height)=ParaFrame[FrameId].height;
    if (LineWdth) (*LineWdth)=ParaFrame[FrameId].LineWdth;
    if (LineColor) (*LineColor)=ParaFrame[FrameId].LineColor;
    if (BackColor) (*BackColor)=ParaFrame[FrameId].BackColor;
    if (flags) (*flags)=ParaFrame[FrameId].flags;

    return TRUE;

}

/******************************************************************************
    TerGetFrameParam:
    Retrieve frame object parameters.
******************************************************************************/
int WINAPI _export TerGetFrameParam(HWND hWnd, int FrameId, int type)
{

    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FP_ERROR;  // get the pointer to window data

    if (FrameId<0 || FrameId>=TotalParaFrames || !ParaFrame[FrameId].InUse) return FP_ERROR;
 
    if (type==FP_TEXT_ROTATION) {
       int angle=ParaFrame[FrameId].TextAngle;
       if  (angle==270)    return TEXT_TOP_TO_BOT;
       else if (angle==90) return TEXT_BOT_TO_TOP;
       else                return TEXT_HORZ;
    } 
    else if (type==FP_WRAP_STYLE) {
       int flags=ParaFrame[FrameId].flags;
       if       (flags&PARA_FRAME_NO_WRAP)   return SWRAP_NO_WRAP;
       else if (flags&PARA_FRAME_WRAP_THRU) return SWRAP_THRU;
       else return SWRAP_AROUND;
    } 
    else if (type==FP_YBASE) {
       int flags=ParaFrame[FrameId].flags;
       if      (flags&PARA_FRAME_VPAGE)     return BASE_PAGE;
       else if (flags&PARA_FRAME_VMARG)     return BASE_MARG;
       else return BASE_PARA;
    }
    else if (type==FP_FILL_PATTERN) return ParaFrame[FrameId].FillPattern;
    else if (type==FP_TEXT_DIST)    return ParaFrame[FrameId].DistFromText;

    return FP_ERROR;
}
 
/******************************************************************************
    TerInsertLineObject:
    This function inserts a line object of any angle.  The line points are
    specified in the twips units.
    This function is valid only in the PrintView and PageModes.
    This function return frame id of the frame if successful, otherwise it
    return 0.
******************************************************************************/
int WINAPI _export TerInsertLineObject(HWND hWnd, int x1, int y1, int x2, int y2)
{
    PTERWND w;
    int FrameId,x,y,width,height;
    UINT flags=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PrintView) return FALSE;

    // apply specific object properties
    flags=PARA_FRAME_LINE|PARA_FRAME_WRAP_THRU;

    // get the rectangle dimensions
    width=abs(x1-x2);
    height=abs(y1-y2);
    x=(x1<x2)?x1:x2;
    y=(y1<y2)?y1:y2;

    if (0==(FrameId=InsertParaFrame(w,x,y,width,height,FALSE,flags,TRUE))) return 0;

    // apply common object properties
    ParaFrame[FrameId].BackColor=0xFFFFFF;   // background color is white
    ParaFrame[FrameId].LineColor=0;          // line color is zero
    ParaFrame[FrameId].FillPattern=1;        // default
    ParaFrame[FrameId].margin=MIN_FRAME_MARGIN;
    ParaFrame[FrameId].margin=0;
    ParaFrame[FrameId].LineWdth=20;          // 20 twips by default
    ParaFrame[FrameId].flags|=PARA_FRAME_BOXED;

    // apply specific object properties
    LinePointsToRect(w,FrameId,x1,y1,x2,y2);

    PaintTer(w);

    return FrameId;
}

/******************************************************************************
    TerInsertDrawObject:
    This function inserts a drawing object.  The drawing object type is
    given by the DOB_ constants. When the 'type' is set to 0, this function
    prompts the user to select a type.
    The position and dimension variable
    area specified in twips unit. The 'x' is relative to the left margin and the
    'y' is relative to the beginning of the current paragraph.
    Any of these unit can be set to -1 to use
    the respective default value.  The default value for x and y position
    is the current line position.
    This function is valid only in the PrintView and PageModes.
    This function return frame id of the frame if successful, otherwise it
    return 0.
******************************************************************************/
int WINAPI _export TerInsertDrawObject(HWND hWnd, int type, int x, int y, int width, int height, int base)
{
    PTERWND w;
    int FrameId;
    UINT flags=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PrintView) return FALSE;

    // get the object type
    if (type<=0) {
       if (0==(type=CallDialogBox(w,"DrawObjectParam",DrawObjectParam,0L))) return 0;
    }

    // apply specific object properties
    flags|=PARA_FRAME_WRAP_THRU;

	//  // MAK: PARA_FRAME_VMARG, geht sonst nicht!
    if(base == BASE_PAGE)
      flags |= PARA_FRAME_VPAGE;
    if(base == BASE_MARG)
      flags |= PARA_FRAME_VMARG;

    if      (type==DOB_TEXT_BOX) flags|=PARA_FRAME_TEXT_BOX;
    else if (type==DOB_RECT)     flags|=PARA_FRAME_RECT;
    else if (type==DOB_LINE)     flags|=PARA_FRAME_LINE;


    // MAK: changed	
    // Es gibt Probleme mit WordXP, dort wird bei DOB_TEXT_BOX in RTF nichts angezeigt
    if(type==-3)
      flags=PARA_FRAME_NO_WRAP; 
    if(type==-4)
      flags=PARA_FRAME_WRAP_THRU;

    if (0==(FrameId=InsertParaFrame(w,x,y,width,height,FALSE,flags,TRUE))) return 0;

    // apply common object properties
    ParaFrame[FrameId].BackColor=0xFFFFFF;   // background color is white
    ParaFrame[FrameId].LineColor=0;          // line color is zero
    ParaFrame[FrameId].FillPattern=1;        // default
    ParaFrame[FrameId].margin=MIN_FRAME_MARGIN;
    if (type==DOB_RECT || type==DOB_LINE) ParaFrame[FrameId].margin=0;
    if (type==DOB_LINE && y==-1) {
       ParaFrame[FrameId].ParaY=PrtToTwipsX(LineHt(CurLine));  // place it after the line
       TerRepaginate(hWnd,TRUE);
    }
    ParaFrame[FrameId].LineWdth=20;          // 20 twips by default
    ParaFrame[FrameId].flags|=PARA_FRAME_BOXED;

    // apply specific object properties
    if (type==DOB_RECT) ParaFrame[FrameId].MinHeight=ParaFrame[FrameId].height;
    else if (type==DOB_LINE) ParaFrame[FrameId].MinHeight=ParaFrame[FrameId].height=0; // horizontal line

    PaintTer(w);

    return FrameId;
}

/******************************************************************************
    TerInsertParaFrame:
    This function return frame id of the frame if successful, otherwise it
    return 0.
******************************************************************************/
int WINAPI _export TerInsertParaFrame(HWND hWnd, int x, int y, int width, int height,BOOL boxed)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return InsertParaFrame(w,x,y,width,height,boxed,0,TRUE);
}

/******************************************************************************
    TerCreateParaFrameId:
    This function return frame id of the frame if successful, otherwise it
    return 0.
******************************************************************************/
int WINAPI _export TerCreateParaFrameId(HWND hWnd, int x, int y, int width, int height)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return InsertParaFrame(w,x,y,width,height,FALSE,0,FALSE);
}

/******************************************************************************
    InsertParaFrame:
    This function inserts a paragraph frame.  The position and dimension variable
    area specified in twips unit. The 'x' is relative to the left margin and the
    'y' is relative to the beginning of the current paragraph. If the NewFrameVPage
    flag is set then the y is relative to the top of the page.
    Any of these unit can be set to -1 to use
    the respective default value.  The default value for x and y position
    is the current line position.
    This function is valid only in the PrintView and PageModes.
    This function return frame id of the frame if successful, otherwise it
    return 0.
******************************************************************************/
int InsertParaFrame(PTERWND w, int x, int y, int width, int height,BOOL boxed, UINT InitFlags, BOOL insert)
{
    int FrameId,ParaBegY,CurY;
    long ParaLine,l;
    LPBYTE ptr;
    LPWORD fmt;
    UINT flags;

    if (!TerArg.PrintView) return FALSE;

    if (!CheckLineLimit(w,TotalLines+1)) {
       PrintError(w,MSG_MAX_LINES_EXCEEDED,MsgString[MSG_ERR_BLOCK_SIZE]);
       return 0;
    }

    ReleaseUndo(w);     // release any existing undos

    if ((FrameId=GetParaFrameSlot(w))<0) return 0;

    if (x==-1) x=NewFrameX;
    if (y==-1) y=NewFrameY;

    // supply default values for loation and dimemsion if needed
    if (x==-1) {
       x=ColToUnits(w,CurCol,CurLine,LEFT);
       x=ScrToTwipsX(x); // relative to the left edge of the screen
       if (BorderShowing) x-=PrtToTwipsX(GetBorderLeftSpace(w,CurPage)); // relative to left margin
    }
    if (width<0) width=NewFrameWidth;
    if (height<0) height=NewFrameHeight;

    // populate the paraframe structure
    FarMemSet(&(ParaFrame[FrameId]),0,sizeof(struct StrParaFrame));
    ParaFrame[FrameId].InUse=TRUE;
    ParaFrame[FrameId].x=x;
    ParaFrame[FrameId].width=width;
    ParaFrame[FrameId].height=height;
    ParaFrame[FrameId].margin=MIN_FRAME_MARGIN*2;
    ParaFrame[FrameId].MinHeight=height;
    ParaFrame[FrameId].DistFromText=FRAME_DIST_FROM_TEXT;
    ParaFrame[FrameId].PageNo=CurPage;
    ParaFrame[FrameId].flags=InitFlags;  // initial flags
    ParaFrame[FrameId].ZOrder=1;    // do not use 0 as it would be changed to -1 during rtf re-read
    if (NewFrameVPage) ParaFrame[FrameId].flags|=PARA_FRAME_VPAGE;

    // find the beginning of the current paragraph
    ParaLine=GetParaFrameLine(w,CurLine);

    // calculate the y location of the frame relative to the top margin
    // find the next stationary paragraph
    l=ParaLine;
    while (l<TotalLines) {
       if (fid(l)==0) break;
       l++;
    }
    if (l==TotalLines) l--;
    ParaBegY=LineToUnits(w,l);
    ParaBegY=ScrToTwipsY(ParaBegY);

    CurY=LineToUnits(w,CurLine);
    CurY=ScrToTwipsY(CurY);

    // set the y value
    if (ParaFrame[FrameId].flags&PARA_FRAME_VPAGE) {
       if (y==-1) y=FrameToPageY(w,CurY);
       ParaFrame[FrameId].ParaY=ParaFrame[FrameId].y=y;   // value relative to the top of the page
    }
    else {
       if (y==-1) y=CurY-ParaBegY;
       ParaFrame[FrameId].ParaY=y;   // value relative to the next paragraph
       ParaFrame[FrameId].y=ParaFrame[FrameId].ParaY+ParaBegY;   // value relative to the top of text
    }

    if (!insert) return FrameId;        // simply create the frame without inserting it

    // turn on para border
    flags=PfmtId[pfmt(ParaLine)].flags;
    ResetUintFlag(flags,(PARA_BOX_ATTRIB|BULLET));
    if (boxed) flags|=PARA_BOX;

    // create the paragraph line
    MoveLineArrays(w,ParaLine,1,'B');   // insert a line before this line
    fid(ParaLine)=FrameId;              // associate the frame id
    cid(ParaLine)=0;                    // not in a table

    pfmt(ParaLine)=NewParaId(w,pfmt(ParaLine),    // set borders
                         PfmtId[pfmt(ParaLine)].LeftIndentTwips,
                         PfmtId[pfmt(ParaLine)].RightIndentTwips,
                         PfmtId[pfmt(ParaLine)].FirstIndentTwips,
                         PfmtId[pfmt(ParaLine)].TabId,
                         0,
                         PfmtId[pfmt(ParaLine)].AuxId,
                         PfmtId[pfmt(ParaLine)].Aux1Id,
                         PfmtId[pfmt(ParaLine)].StyId,
                         PfmtId[pfmt(ParaLine)].shading,
                         PfmtId[pfmt(ParaLine)].pflags|PFLAG_FRAME,
                         PfmtId[pfmt(ParaLine)].SpaceBefore,
                         PfmtId[pfmt(ParaLine)].SpaceAfter,
                         PfmtId[pfmt(ParaLine)].SpaceBetween,
                         PfmtId[pfmt(ParaLine)].LineSpacing,
                         PfmtId[pfmt(ParaLine)].BkColor,
                         PfmtId[pfmt(ParaLine)].BorderSpace,
                         PfmtId[pfmt(ParaLine)].flow,
                         flags);

    // insert the paragraph marker.
    LineAlloc(w,ParaLine,0,1);
    ptr=pText(ParaLine);
    fmt=OpenCfmt(w,ParaLine);
    ptr[0]=ParaChar;
    fmt[0]=DEFAULT_CFMT;

    CloseCfmt(w,ParaLine);

    CurLine=ParaLine;
    CurCol=0;

    Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint

    return FrameId;
}

/******************************************************************************
    TerGetFrameSize:
    This function retrieves the location and size of the given frame.
    The location is returned in twips relative to the edge of the page
    If the frame id is 0, it retrieves the size of the current frame.
    The function returns TRUE when successul.
******************************************************************************/
BOOL WINAPI _export TerGetFrameSize(HWND hWnd, int ParaFID, LPINT pX, LPINT pY, LPINT pWidth, LPINT pHeight)
{
    PTERWND w;
    int sect,x,y,LeftMarg;


    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (ParaFID>0 && !ParaFrame[ParaFID].InUse) return FALSE;

    if (ParaFID<=0) {
       ParaFID=fid(CurLine);
       if (ParaFID<=0) return FALSE;
    }

    x=ParaFrame[ParaFID].x;  // x relative to margin
    y=ParaFrame[ParaFID].y;  // y relative to frame set

    // convert x relative to frame set to x relative to the page border
    sect=TerGetPageSect(hTerWnd,CurPage);
    LeftMarg=(int)(TerSect[sect].LeftMargin*1440);
    x+=LeftMarg;        // x relative to page edge in twips

    // convert y relative to frame set to y relative to the page border
    y=TwipsToScrY(y);   // in screen units
    y=y-TerWinOrgY+TerWinRect.top;  // x relative to the client area
    TerScrToTwipsY(hWnd,y,&y);       // x relative to the page


    if (pX)      (*pX)=x;
    if (pY)      (*pY)=y;
    if (pWidth)  (*pWidth)=ParaFrame[ParaFID].width;
    if (pHeight) (*pHeight)=ParaFrame[ParaFID].height;

    return TRUE;
}

/******************************************************************************
    TerSetNewFrameDim:
    Set the dimensions to be used for creating new frames.
    The x,y,width, and height are specified in twips.
    Set the x/y to -1 to use the current cursor position for the frame.
    Set width/height to 0 if you wish to retain the old values.
    The function returns TRUE when successul.
******************************************************************************/
BOOL WINAPI _export TerSetNewFrameDim(HWND hWnd, int x, int y, int width, int height, BOOL PageTop)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    NewFrameX=x;
    NewFrameY=y;
    if (width>0)  NewFrameWidth=width;
    if (height>0) NewFrameHeight=height;
    NewFrameVPage = PageTop;

    return TRUE;
}

/******************************************************************************
    TerMovePictFrame:
    The FrameX is relative to the left margin.  The FrameY is relative to
    the frame's vertical base position (page top, margin, or para)
******************************************************************************/
BOOL WINAPI _export TerMovePictFrame(HWND hWnd, int pict, int FrameX, int FrameY)
{
    PTERWND w;
    int ParaFID;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    if (pict<0 || pict>=TotalFonts) return FALSE;
    if (!TerFont[pict].InUse || !(TerFont[pict].style&PICT) || TerFont[pict].FrameType==PFRAME_NONE) return FALSE;

    ParaFID=TerFont[pict].ParaFID;
    if (!ParaFrame[ParaFID].InUse) return FALSE;

    SaveUndo(w,ParaFID,0,0,0,UNDO_FRAME_LIMITED);  // do limited undo because anchor does not move

    if (FrameX!=PARAM_IGNORE) {
       int adj=FrameX-ParaFrame[ParaFID].x;
       ParaFrame[ParaFID].x+=adj;
       ParaFrame[ParaFID].OrgX+=adj;
    }
    if (FrameY!=PARAM_IGNORE) ParaFrame[ParaFID].ParaY=FrameY;

    TerArg.modified++;

    Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint

    return TRUE;
}

/******************************************************************************
    TerMoveParaFrame:
    This function moves a paragraph frame to the given x and y location.  The
    location is specified in the Twips unit.  The 'x' value is relative to
    the page left margin. The 'y' value is relative to the top margin when
    the page header/footer view option is not set, otherwise it is relative
    to the the top of the page. Specify -1 for width or height, if you do not
    wish to change the dimensions of the frame.

    The function returns TRUE when successul.
******************************************************************************/
BOOL WINAPI _export TerMoveParaFrame(HWND hWnd, int ParaFID, int FrameX, int FrameY, int FrmWidth, int FrmHeight)
{
    return TerMoveParaFrame2(hWnd,ParaFID,FrameX,FrameY,FrmWidth,FrmHeight,-1);
}

BOOL WINAPI _export TerMoveParaFrame3(HWND hWnd, int ParaFID, int nFrameX, int nFrameY, int FrmWidth, int FrmHeight, int base)
{
  PTERWND w;
  int PageNo, sect, i, UndoReleased=false;

  if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

  if (FrmWidth>0) ParaFrame[ParaFID].width=FrmWidth;
  if (FrmHeight>=0) ParaFrame[ParaFID].MinHeight=FrmHeight;   // 0 means adjustable height
  if (ParaFrame[ParaFID].flags&PARA_FRAME_OBJECT)
    ParaFrame[ParaFID].height=ParaFrame[ParaFID].MinHeight;

  if (ParaFrame[ParaFID].ShapeType==SHPTYPE_PICT_FRAME && (FrmWidth>0 || FrmHeight>0)) {    // set the picture size
    int pict=ParaFrame[ParaFID].pict;
    if (pict>0) {
      if (FrmWidth>0) TerFont[pict].PictWidth=FrmWidth;
      if (FrmHeight>0) TerFont[pict].PictHeight=FrmHeight;
      SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),FALSE);
      XlateSizeForPrt(w,pict);     // convert size to printer resolution
    } 
  } 

  // invalidate para space text segments
  if (BkPictId) DeleteTextMap(w,TRUE);
  else {
    for (i=0;i<TotalSegments;i++) {
      if (TextSeg[i].ParFrameSpace || ParaFrame[ParaFID].flags&PARA_FRAME_WRAP_THRU)
        DeleteTextSeg(w,i,BkPictId?TRUE:FALSE);
    }
  }

  if (UndoReleased) ReleaseUndo(w);    // release it again for any intervening undos

  Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint

	return TRUE;
}

BOOL WINAPI _export TerMoveParaFrame2(HWND hWnd, int ParaFID, int FrameX, int FrameY, int FrmWidth, int FrmHeight, int page)
{
 
    PTERWND w;
    int  i,x,y,AnchorLineY,ParaY,CurFrameNo;
    long l,ParaLine,AnchorLine,LineCount,FrameBegLine=0,FrameEndLine;
    POINT HiddenMarg;
    UINT HdrFtr;
    BOOL MoveAnchor=TRUE,SaveReclaimResources,UndoReleased=false;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (!ParaFrame[ParaFID].InUse) return FALSE;

    // Get the closest text line. Allow negative value
    if (BorderShowing) FrameY+=PrtToTwipsY(TopBorderHeight);  // relative to the top of the page frame
    x=TwipsToScrX(FrameX);
    y=TwipsToScrY(FrameY);


    // disable the current frame temporariy
    for (CurFrameNo=0;CurFrameNo<TotalFrames;CurFrameNo++)
       if (!frame[CurFrameNo].empty && frame[CurFrameNo].ParaFrameId==ParaFID) break;
    if (CurFrameNo<TotalFrames) frame[CurFrameNo].empty=TRUE;

    // adjust the screen y position relative to the beginning of the frame set
    if (CurFrameNo<TotalFrames) {
       l=frame[CurFrameNo].PageFirstLine;
       if (page<0) page=LinePage(l);   // page on which the line is located
       if (page==(FirstFramePage+1)) y+=FirstPageHeight;
    } 


    ParaLine=UnitsToLine(w,x,y);

    SetPageFromY(w,y);                      // set the page number from y

    if (CurFrameNo<TotalFrames) frame[CurFrameNo].empty=FALSE; // re-enable

    // check if the frame anchor needs to be changed
    if ((ParaFrame[ParaFID].flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG))) {
       for (l=0;l<TotalLines;l++) if (fid(l)==ParaFID) break;
       if (l<TotalLines && (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR || PageFromLine(w,l,-1)==CurPage)) MoveAnchor=FALSE;
       FrameBegLine=l;
    }

    // get the anchor line before which the frame will be inserted
    AnchorLine=GetParaFrameLine(w,ParaLine);    // get the para begin line closest to this line
    AnchorLineY=GetAnchorY(w,AnchorLine);

    RecreateSections(w);

    if (ParaFrame[ParaFID].flags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG)) {   // y relative to page top or top margin
       int sect=GetSection(w,MoveAnchor?AnchorLine:FrameBegLine);
       int TopMargin=(int)InchesToTwips(TerSect[sect].TopMargin);
       HiddenMarg.y=PrtToTwipsY(TerSect1[sect].HiddenY);


       if (CurPage>FirstFramePage) ParaY=FrameY-ScrToTwipsY(FirstPageHeight);
       else                        ParaY=FrameY;

       if (ParaFrame[ParaFID].flags&PARA_FRAME_VPAGE) {   // y relative to page top
          if (BorderShowing)       ParaY-=PrtToTwipsY(TopBorderHeight);
          else if (ViewPageHdrFtr) ParaY+=HiddenMarg.y;
          else                     ParaY+=TopMargin;
       }
       else {                                             // y relative to the top margin
          if (BorderShowing)       ParaY-=(PrtToTwipsY(TopBorderHeight)+TopMargin);
          else if (ViewPageHdrFtr) ParaY-=(TopMargin-HiddenMarg.y);
       }
    }
    else ParaY=ScrToTwipsY(y-AnchorLineY);           // displacement from the paragraph

    // find the beginning and end of the current frame
    for (l=CurLine-1;l>=0;l--) if (fid(l)!=ParaFID) break;
    FrameBegLine=l+1;
    for (l=CurLine+1;l<TotalLines;l++) if (fid(l)!=ParaFID) break;
    FrameEndLine=l-1;

    // do we need to cut/paste?
    if (!MoveAnchor || (AnchorLine>=FrameBegLine && AnchorLine<=(FrameEndLine+1))) {
       SaveUndo(w,ParaFID,0,0,0,UNDO_FRAME_LIMITED);   // anchor does not move, so do limited undo
       goto SET_FRAME;  // no cut/paste needed
    }
    
    if (TotalLines<=UNDO_LINE_LIMIT) SaveUndo(w,ParaFID,0,0,0,UNDO_FRAME);
    else                             UndoReleased=ReleaseUndo(w);


    HdrFtr=PfmtId[pfmt(AnchorLine)].flags&PAGE_HDR_FTR;  // get the hdr/ftr flags in effect

    // move the text pointer to the new location
    LineCount=FrameEndLine-FrameBegLine+1;
    if (!CheckLineLimit(w,TotalLines+LineCount)) return TRUE;
    MoveLineArrays(w,AnchorLine,LineCount,'B');  // insert before the anchor line
    if (FrameBegLine>AnchorLine) {              // adjust the frame begin line
       FrameBegLine+=LineCount;
       FrameEndLine+=LineCount;
    }

    SaveReclaimResources=ReclaimResources;      // do not reclaim becuase LinePtr can be NULL
    ReclaimResources=FALSE;
    for (l=0;l<LineCount;l++) {                 // move the line pointers
       FreeLine(w,AnchorLine+l);                // free before overwriting
       LinePtr[AnchorLine+l]=LinePtr[FrameBegLine+l];
       LinePtr[FrameBegLine+l]=NULL;            // this pointer transferred

       pfmt(AnchorLine+l)=ParaIdForFrame(w,pfmt(AnchorLine+l),HdrFtr);
    }
    ReclaimResources=SaveReclaimResources;


    // delete the previous lines
    MoveLineArrays(w,FrameBegLine,LineCount,'D');
    if (FrameBegLine<AnchorLine) AnchorLine-=LineCount;
    CurLine=AnchorLine;
    CurCol=0;

    SET_FRAME:
    ParaFrame[ParaFID].x=FrameX;    // relative to left margin
    ParaFrame[ParaFID].y=FrameY;    // relative to page
    ParaFrame[ParaFID].ParaY=ParaY; // relative to the anchor paragraph
    if (FrmWidth>0) ParaFrame[ParaFID].width=FrmWidth;
    if (FrmHeight>=0) ParaFrame[ParaFID].MinHeight=FrmHeight;   // 0 means adjustable height
    if (ParaFrame[ParaFID].flags&PARA_FRAME_OBJECT)
       ParaFrame[ParaFID].height=ParaFrame[ParaFID].MinHeight;

    if (ParaFrame[ParaFID].ShapeType==SHPTYPE_PICT_FRAME && (FrmWidth>0 || FrmHeight>0)) {    // set the picture size
       int pict=ParaFrame[ParaFID].pict;
       if (pict>0) {
          if (FrmWidth>0) TerFont[pict].PictWidth=FrmWidth;
          if (FrmHeight>0) TerFont[pict].PictHeight=FrmHeight;
          SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),FALSE);
          XlateSizeForPrt(w,pict);     // convert size to printer resolution
       } 
    } 
    
    // invalidate para space text segments
    if (BkPictId) DeleteTextMap(w,TRUE);
    else {
       for (i=0;i<TotalSegments;i++) {
          if (TextSeg[i].ParFrameSpace || ParaFrame[ParaFID].flags&PARA_FRAME_WRAP_THRU)
             DeleteTextSeg(w,i,BkPictId?TRUE:FALSE);
       }
    }
    
    if (UndoReleased) ReleaseUndo(w);    // release it again for any intervening undos

    Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint

    return TRUE;
}


/******************************************************************************
    FrameParaForId:
    This routine gets a new para id from the previous one to be used with
    a paragraph frame. The HdrFtr arguments tells which of (PAGE_HDR or PAGE_FTR)
    , if any, flag should be set
******************************************************************************/
ParaIdForFrame(PTERWND w,int CurPara,UINT HdrFtr)
{
   UINT flags;

   flags=PfmtId[CurPara].flags;
   ResetUintFlag(flags,PAGE_HDR_FTR);
   flags|=HdrFtr;

   return NewParaId(w,CurPara,
                   PfmtId[CurPara].LeftIndentTwips,
                   PfmtId[CurPara].RightIndentTwips,
                   PfmtId[CurPara].FirstIndentTwips,
                   PfmtId[CurPara].TabId,
                   0,
                   PfmtId[CurPara].AuxId,
                   PfmtId[CurPara].Aux1Id,
                   PfmtId[CurPara].StyId,
                   PfmtId[CurPara].shading,
                   PfmtId[CurPara].pflags,
                   PfmtId[CurPara].SpaceBefore,
                   PfmtId[CurPara].SpaceAfter,
                   PfmtId[CurPara].SpaceBetween,
                   PfmtId[CurPara].LineSpacing,
                   PfmtId[CurPara].BkColor,
                   PfmtId[CurPara].BorderSpace,
                   PfmtId[CurPara].flow,
                   flags);
}

/******************************************************************************
    TerSetFrameYBase:
    This function the y position base of a paragraph frame or drawing object.
    The y position can be relative to BASE_PAGE, BASE_PARA, BASE_MARG.
    if the specified frame id is -1, this function shows the user dialog box
    and applies the new setting to the current object, if any.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetFrameYBase(HWND hWnd, int FrameId, int base)
{
    PTERWND w;
    int PageY,sect,ScrFrameId,HiddenY,PageNo;
    long l,FrameLine;
    UINT PrevFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    if (FrameId<0) {
       FrameId=fid(CurLine);
       if (FrameId<=0) {        // check for the picture frame at the current location
          int pict=GetCurCfmt(w,CurLine,CurCol);
          if (!(TerFont[pict].style&PICT) || TerFont[pict].FrameType==PFRAME_NONE) return FALSE;
          FrameId=TerFont[pict].ParaFID;
       }

       DlgInt1=FrameId;                      // pass to the dialog box
       if (!CallDialogBox(w,"ObjectPosParam",ObjectPosParam,0L)) return FALSE;

       base=DlgInt2;                        // base
       FrameLine=CurLine;                   // one of lines contained in the frame

    }
    else {
       if (FrameId<=0 || FrameId>=TotalParaFrames || !(ParaFrame[FrameId].InUse)) return FALSE;

       // find the first line of the frame
       for (l=0;l<TotalLines;l++) {
          if (fid(l)==FrameId) break;
       }
       if (l==TotalLines) return FALSE;   // frame not found

       FrameLine=l;
    }

    if (base!=BASE_PAGE && base!=BASE_MARG && base!=BASE_PARA) return FALSE;

    // check if modification needed
    PrevFlags=ParaFrame[FrameId].flags;
    if (PrevFlags&PARA_FRAME_VPAGE && base==BASE_PAGE) return TRUE;
    if (PrevFlags&PARA_FRAME_VMARG && base==BASE_MARG) return TRUE;
    if (!(PrevFlags&(PARA_FRAME_VMARG|PARA_FRAME_VPAGE)) && base==BASE_PARA) return TRUE;

    TerArg.modified++;                     // apply the modification now

    // get the current section
    sect=GetSection(w,FrameLine);
    PageNo=PageFromLine(w,FrameLine,-1);
    HiddenY=TerSect1[sect].HiddenY;

    // Modify ParaY for each base type
    if (base==BASE_PAGE || base==BASE_MARG) {
       // get PageY in twips
       if (BorderShowing)       PageY=ParaFrame[FrameId].y-PrtToTwipsY(TopBorderHeight);
       else if (ViewPageHdrFtr) PageY=ParaFrame[FrameId].y+PrtToTwipsY(HiddenY);
       else                     PageY=ParaFrame[FrameId].y+(int)(InchesToTwips(TerSect[sect].HdrMargin))+PrtToTwipsY(PageHdrHeight2(w,PageNo,FALSE,true));

       if (base==BASE_PAGE) ParaFrame[FrameId].ParaY=PageY;
       else                 ParaFrame[FrameId].ParaY=PageY-(int)InchesToTwips(TerSect[sect].TopMargin);
    }
    else {
       int AnchorY=GetAnchorY(w,-FrameId);

       // get PageY in screen units
       if ((ScrFrameId=GetFrame(w,FrameLine))<0) return FALSE;  // line not found in the current frame

       ParaFrame[FrameId].ParaY=ScrToTwipsY(frame[ScrFrameId].y-AnchorY);

    }

    // apply the base
    ResetUintFlag(ParaFrame[FrameId].flags,(PARA_FRAME_VPAGE|PARA_FRAME_VMARG));
    if (base==BASE_PAGE) ParaFrame[FrameId].flags|=PARA_FRAME_VPAGE;
    else if (base==BASE_MARG) ParaFrame[FrameId].flags|=PARA_FRAME_VMARG;

    Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint

    return TRUE;
}

/******************************************************************************
    TerSetObjectAttrib:
    This function edits the line and fill attributes for a drawing object.
    if the specified object id is -1, this function shows the user dialog box
    and applies the new setting to the current object, if any.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetObjectAttrib(HWND hWnd, int FrameId, int LineType, int LineThickness, COLORREF LineColor, BOOL FillSolid, COLORREF FillColor)
{
    return TerSetObjectAttribEx(hWnd,FrameId,LineType,LineThickness,LineColor,FillSolid,FillColor,-9999);
}

/******************************************************************************
    TerSetObjectAttribEx:
    This function edits the line and fill attributes for a drawing object.
    if the specified object id is -1, this function shows the user dialog box
    and applies the new setting to the current object, if any.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetObjectAttribEx(HWND hWnd, int FrameId, int LineType, int LineThickness, COLORREF LineColor, BOOL FillSolid, COLORREF FillColor, int ZOrder)
{
    PTERWND w;
    UINT FrameWrapFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    if (FrameId<0) {
       FrameId=fid(CurLine);
       if (FrameId<=0) {        // check for the picture frame at the current location
          int pict=GetCurCfmt(w,CurLine,CurCol);
          if (!(TerFont[pict].style&PICT) || TerFont[pict].FrameType==PFRAME_NONE) return FALSE;
          FrameId=TerFont[pict].ParaFID;
       }

       if (!(ParaFrame[FrameId].flags&PARA_FRAME_OBJECT)) return FALSE;         // not positioned on a paragraph frame

       DlgInt1=FrameId;                      // pass to the dialog box
       if (!CallDialogBox(w,"ObjectAttribParam",ObjectAttribParam,0L)) return FALSE;

       // extract the dialog box values
       LineThickness=DlgInt1;   // line thickness passed using the DlgInt1 variable
       LineType=DlgInt2;
       LineColor=DlgLong;
       FillSolid=DlgInt3;
       FillColor=DlgLong1;
       ZOrder=DlgInt4;
       FrameWrapFlags=(UINT)DlgInt5;
    }
    else {
       if (FrameId<=0 || FrameId>=TotalParaFrames || !(ParaFrame[FrameId].InUse)) return FALSE;
       if (!(ParaFrame[FrameId].flags&PARA_FRAME_OBJECT)) return FALSE;
       FrameWrapFlags=ParaFrame[FrameId].flags&(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU);  // use the existing wrap type
    }

    // validity check on the arguments
    if (LineThickness<0) return FALSE;
    if (LineType!=DOB_LINE_NONE && LineType!=DOB_LINE_SOLID && LineType!=DOB_LINE_DOTTED) return FALSE;

    // apply the values
    ParaFrame[FrameId].LineWdth=LineThickness;
    //if (LineThickness<ParaFrame[FrameId].margin && ParaFrame[FrameId].margin>BORDER_MARGIN) ParaFrame[FrameId].margin=BORDER_MARGIN;  // reset margin
    //if (LineThickness>ParaFrame[FrameId].margin) ParaFrame[FrameId].margin=LineThickness;
    
    ParaFrame[FrameId].flags=ResetUintFlag(ParaFrame[FrameId].flags,(PARA_FRAME_BOXED|PARA_FRAME_DOTTED));
    if      (LineType==DOB_LINE_SOLID)  ParaFrame[FrameId].flags|=(PARA_FRAME_BOXED);
    else if (LineType==DOB_LINE_DOTTED) ParaFrame[FrameId].flags|=(PARA_FRAME_BOXED|PARA_FRAME_DOTTED);
    ParaFrame[FrameId].LineColor=LineColor;
    if (FillSolid) ParaFrame[FrameId].FillPattern=1;
    else           ParaFrame[FrameId].FillPattern=0;
    ParaFrame[FrameId].BackColor=FillColor;
    if (ZOrder!=-9999) ParaFrame[FrameId].ZOrder=ZOrder;
    
    if (!(ParaFrame[FrameId].flags&PARA_FRAME_LINE)) {
       if ((ParaFrame[FrameId].flags&(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU))!=FrameWrapFlags) RequestPagination(w,TRUE);
       ResetUintFlag(ParaFrame[FrameId].flags,(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU));
       ParaFrame[FrameId].flags|=FrameWrapFlags;
    } 
     


    PaintFlag=PAINT_WIN_RESET;
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerSetObjectWrapStyle:
    This function wrapping style for an object.  The WrapStyles can be 
    one of SWRAP_NO_WRAP, SWRAP_AROUND, or SWRAP_THRU.
*****************************************************************************/
BOOL WINAPI _export TerSetObjectWrapStyle(HWND hWnd, int FrameId, int WrapStyle)
{
    PTERWND w;
    UINT FrameWrapFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (WrapStyle!=SWRAP_NO_WRAP && WrapStyle!=SWRAP_AROUND && WrapStyle!=SWRAP_THRU) return false;

    if (FrameId<0) FrameId=fid(CurLine);
    if (FrameId==0 || FrameId>=TotalParaFrames || !(ParaFrame[FrameId].InUse)) return FALSE;
    if (False(ParaFrame[FrameId].flags&PARA_FRAME_OBJECT) && ParaFrame[FrameId].pict==0) return FALSE;
    if (True(ParaFrame[FrameId].flags&PARA_FRAME_LINE)) return false;
    
    FrameWrapFlags=0;
    if (WrapStyle==SWRAP_NO_WRAP)   FrameWrapFlags=PARA_FRAME_NO_WRAP;
    if (WrapStyle==SWRAP_THRU)      FrameWrapFlags=PARA_FRAME_WRAP_THRU;
    
    
    if ((ParaFrame[FrameId].flags&(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU))!=FrameWrapFlags) RequestPagination(w,true);
    ResetUintFlag(ParaFrame[FrameId].flags,(PARA_FRAME_NO_WRAP|PARA_FRAME_WRAP_THRU));
    ParaFrame[FrameId].flags|=FrameWrapFlags;
     
    PaintFlag=PAINT_WIN_RESET;
    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    GetCellFrameLeftWidth:
    Get the width for the left border for the cell
******************************************************************************/
GetCellFrameLeftWidth(PTERWND w, int CellId, UINT far *borders, COLORREF far *pColor)
{
    int LeftWidth=0,TempWidth,TempCell;
    COLORREF color=cell[CellId].BorderColor[BORDER_INDEX_LEFT];
    BOOL ShareBorder;
      
    if (HtmlMode && !(TerFlags3&TFLAG3_PLAIN_TABLE_BORDER) && (cell[CellId].flags&CFLAG_3D)) {              // 3d border
       LeftWidth=cell[CellId].BorderWidth[BORDER_INDEX_TOP];
       if (LeftWidth>0) {        // border specified
          TempCell=cell[CellId].PrevCell;
          if (TempCell<=0) LeftWidth=LeftWidth*3; // additional width to create 3d effect
       }
    }
    else {
       if ((*borders)&BORDER_LEFT) LeftWidth=cell[CellId].BorderWidth[BORDER_INDEX_LEFT];
       TempCell=cell[CellId].PrevCell;
       if (TempCell>0) {
          LeftWidth=(LeftWidth+1)/2;  // round up, other half will be printed in the previous cell

          if (cell[TempCell].border&BORDER_RIGHT) TempWidth=cell[TempCell].BorderWidth[BORDER_INDEX_RIGHT]/2;
          else                                    TempWidth=0;
          
          ShareBorder=!(TerFlags4&TFLAG4_NO_SHARE_BORDER);
          if (ShareBorder) {
            if (LeftWidth==0 && TempWidth>0) color=cell[TempCell].BorderColor[BORDER_INDEX_RIGHT];
            if (TempWidth>LeftWidth) LeftWidth=TempWidth;
          }
          else LeftWidth=LeftWidth*2;  // The left border is printed fully when the border is not shared
          
          if (LeftWidth>0) (*borders)|=BORDER_LEFT;
       }
    }

    if (LeftWidth>cell[CellId].margin) LeftWidth=cell[CellId].margin;

    if (pColor) (*pColor)=color;

    return LeftWidth;
}

/******************************************************************************
    GetCellFrameRightWidth:
    Get the width for the right border for the cell
******************************************************************************/
GetCellFrameRightWidth(PTERWND w, int CellId, UINT far *borders, COLORREF far *pColor)
{
    int RightWidth=0,TempWidth,TempCell;
    COLORREF color=cell[CellId].BorderColor[BORDER_INDEX_RIGHT];
    BOOL ShareBorder;

    if (HtmlMode && !(TerFlags3&TFLAG3_PLAIN_TABLE_BORDER) && (cell[CellId].flags&CFLAG_3D)) {              // 3d border
       RightWidth=cell[CellId].BorderWidth[BORDER_INDEX_TOP];
       if (RightWidth>0) {        // border specified
          TempCell=cell[CellId].NextCell;
          if (TempCell<=0) RightWidth=RightWidth*3; // additional width to create 3d effect
       }
    }
    else {
       if ((*borders)&BORDER_RIGHT) RightWidth=cell[CellId].BorderWidth[BORDER_INDEX_RIGHT];
       TempCell=cell[CellId].NextCell;
       if (TempCell>0) {
          RightWidth=RightWidth/2;   // other half will be printed in the next cell
          if (cell[TempCell].border&BORDER_LEFT) TempWidth=cell[TempCell].BorderWidth[BORDER_INDEX_LEFT]/2;
          else                                   TempWidth=0;
          
          ShareBorder=!(TerFlags4&TFLAG4_NO_SHARE_BORDER);
          
          if (ShareBorder) {
             if (RightWidth==0 && TempWidth>0) color=cell[TempCell].BorderColor[BORDER_INDEX_LEFT];
             if (TempWidth>RightWidth) RightWidth=TempWidth;
          }
          else {
             if (TempWidth>0) RightWidth=0;  // the right border is not printed for intemediate cells if the following row has left border set
             else             RightWidth=RightWidth*2;     // restore original right width
          }

          if (RightWidth>0) (*borders)|=BORDER_RIGHT;
       }
       if (RightWidth==0) ResetUintFlag((*borders),BORDER_RIGHT);
    }

    if (RightWidth>cell[CellId].margin) RightWidth=cell[CellId].margin;

    if (pColor) (*pColor)=color;

    return RightWidth;
}

/******************************************************************************
    GetCellFrameTopWidth:
    Get the width for the top border for the cell
******************************************************************************/
GetCellFrameTopWidth(PTERWND w, int CellId, UINT far *borders, int PageNo, COLORREF far *pColor)
{
    int TempCell,TempWidth,row;
    int TopWidth=0;
    COLORREF color=cell[CellId].BorderColor[BORDER_INDEX_TOP];
    BOOL ShareBorder;

    if (HtmlMode && !(TerFlags3&TFLAG3_PLAIN_TABLE_BORDER) && (cell[CellId].flags&CFLAG_3D)) {              // 3d border
       TopWidth=cell[CellId].BorderWidth[BORDER_INDEX_TOP];
       if (TopWidth>0) {        // border specified
          row=cell[CellId].row;
          if (TableRow[row].PrevRow<=0) TopWidth=TopWidth*3; // additional width to create 3d effect
       }
       if (cell[CellId].margin!=DefCellMargin) TopWidth+=cell[CellId].margin;
    }
    else {
       if ((*borders)&BORDER_TOP) TopWidth=cell[CellId].BorderWidth[BORDER_INDEX_TOP];
       TempCell=GetBorderCell(w,CellId,PageNo,FALSE);
       if (TempCell>0) {
          TopWidth=(TopWidth+1)/2;  // round up. other half will be printed in the previous cell
          if (cell[TempCell].border&BORDER_BOT) TempWidth=cell[TempCell].BorderWidth[BORDER_INDEX_BOT]/2;
          else                                  TempWidth=0;
          ShareBorder=!(TerFlags4&TFLAG4_NO_SHARE_BORDER);
          
          if (ShareBorder) {
             if (TopWidth==0 && TempWidth>0) color=cell[TempCell].BorderColor[BORDER_INDEX_BOT];
             if (TempWidth>TopWidth) TopWidth=TempWidth;
          }
          else TopWidth=TopWidth*2;

          if (TopWidth>0) (*borders)|=BORDER_TOP;
       }
       //(*borders)|=BORDER_TOP;  // set to always draw, the drawing length will be restricted at the draw time
    }


    if (pColor) (*pColor)=color;

    return TopWidth;
}

/******************************************************************************
    GetCellFrameBotWidth:
    Get the width for the bottom border for the cell
******************************************************************************/
GetCellFrameBotWidth(PTERWND w, int CellId, UINT far *borders, int PageNo, COLORREF far *pColor)
{
    int TempCell,TempWidth,row,span;
    int BotWidth=0;
    COLORREF color=cell[CellId].BorderColor[BORDER_INDEX_BOT];
    BOOL ShareBorder;

    if (HtmlMode && !(TerFlags3&TFLAG3_PLAIN_TABLE_BORDER) && (cell[CellId].flags&CFLAG_3D)) {              // 3d border
       BotWidth=cell[CellId].BorderWidth[BORDER_INDEX_BOT];
       if (BotWidth>0) {        // border specified
          BotWidth+=BotWidth;   // double up to get 3d effect
          //check if this cell is on the last row of the table
          row=cell[CellId].row;
          span=cell[CellId].RowSpan;  // row span for the cell
          while (span>1) {
            if (row>0) row=TableRow[row].NextRow;
            span--;
          }
          if (row>0) row=TableRow[row].NextRow;    // get the next row

          if (row<=0) BotWidth+=BotWidth;       // additional width to create 3d effect
       }
       if (cell[CellId].margin!=DefCellMargin) BotWidth+=cell[CellId].margin;
    }
    else {
       if ((*borders)&BORDER_BOT) BotWidth=cell[CellId].BorderWidth[BORDER_INDEX_BOT];
       TempCell=GetBorderCell(w,CellId,PageNo,TRUE);
       if (TempCell>0) {
          BotWidth=BotWidth/2;   // other half will be printed in the previous cell
          if (cell[TempCell].border&BORDER_TOP) TempWidth=cell[TempCell].BorderWidth[BORDER_INDEX_TOP]/2;
          else                                  TempWidth=0;
          
          ShareBorder=!(TerFlags4&TFLAG4_NO_SHARE_BORDER);
          
          if (ShareBorder) {
             if (BotWidth==0 && TempWidth>0) color=cell[TempCell].BorderColor[BORDER_INDEX_TOP];
             if (TempWidth>BotWidth) BotWidth=TempWidth;
          }
          else {
             if (TempWidth>0) BotWidth=0;  // the bottom border is not printed for intemediate rows if the following row has top border set
             else             BotWidth=BotWidth*2;  // restore original bottom width
          }

          if (BotWidth>0) (*borders)|=BORDER_BOT;
       }
       if (BotWidth==0) ResetUintFlag((*borders),BORDER_BOT);
    }

    if (pColor) (*pColor)=color;

    return BotWidth;
}

/******************************************************************************
    GetBorderCell:
    Get next or previous cell if it shares border with the given cell, otherwise
    return 0.
******************************************************************************/
int GetBorderCell(PTERWND w,int CellId, int PageNo, BOOL next)
{
    int i,TempCell,row,InitCell=CellId;

    TempCell=GetSameColumnCell(w,CellId,next);  // get cell in next row
    
    if (TempCell<=0) {
        // check if another row exist after this row
        if (next && cell[CellId].RowSpan>1) {
           row=cell[CellId].row;
           for (i=1;i<cell[CellId].RowSpan;i++) {
              row=TableRow[row].NextRow;
              if (row<=0) return 0;
           }
           CellId=TableRow[row].FirstCell;
        }
        TempCell=UniformRowBorderCell(w,CellId,next);  // get the first cell of the next row if it has uniform top border, otherwise the furnction returns 0
        if (TempCell>0 && UniformRowBorderCell(w,TempCell,!next)<=0) TempCell=0;  // The current row does not have uniform bottom border
        
        if (TempCell<=0) {   // check if cells directly below or above have the uniform width
           int BegX=cell[CellId].x;
           int EndX=BegX+cell[CellId].width;
           int row=cell[CellId].row;
           row=next?TableRow[row].NextRow:TableRow[row].PrevRow;
           if (row>0) {
              int cl,x1,x2,width=-1,NewWidth,FirstCell=0,LastX=0,tol=RULER_TOLERANCE;
              for (cl=TableRow[row].FirstCell;cl>0;cl=cell[cl].NextCell) {
                 x1=cell[cl].x;
                 x2=x1+cell[cl].width;
                 if (x2<=(BegX+tol)) continue;
                 if (x1>=(EndX-tol)) break; 
                 if (x1<(BegX-tol) && x2>(BegX+tol) && x2<(EndX-tol)) break;   // this cell overlaps at BegX
                 if (x1>(BegX+tol) && x1<(EndX-tol) && x2>(EndX+tol)) break;   // this cell overlaps at EndX
                 NewWidth=0;
                 if (next && cell[cl].border&BORDER_TOP) NewWidth=cell[cl].BorderWidth[BORDER_INDEX_TOP];
                 if (!next && cell[cl].border&BORDER_BOT) NewWidth=cell[cl].BorderWidth[BORDER_INDEX_BOT];
                 if (width==-1) NewWidth=width;
                 else if (NewWidth!=width) break;
                 if (FirstCell<=0) FirstCell=cl;
                 LastX=cell[cl].x+cell[cl].width;      // last x being covered
              }
              if (LastX>=(EndX-tol) && width>0) TempCell=FirstCell;     // uniform cells found below or above
           } 
        } 
    }
    if (TempCell>0) {
       int row=cell[CellId].row;
       int PrevRow=TableRow[row].PrevRow;
       int NextRow=TableRow[row].NextRow;

       if (next) {
          if (IsPageLastRow(w,row,PageNo))   TempCell=0;
          if (NextRow>0 && !(TableRow[NextRow].flags&ROWFLAG_HDR) && (TableRow[row].flags&ROWFLAG_HDR)) TempCell=0;
       }
       if (!next) {
          if (IsPartRow(w,TRUE,cell[CellId].row,PageNo)) TempCell=0;
          if (cell[CellId].row==PageInfo[PageNo].FirstRow) TempCell=0;
          if (PrevRow>0 && (TableRow[PrevRow].flags&ROWFLAG_HDR) && !(TableRow[row].flags&ROWFLAG_HDR)) TempCell=0;
       }
    }

    return TempCell;
}

/******************************************************************************
    FitPictureInFrame:
    If a para frame contains just the picture, adjust the picture size to the
    size of the frame.  This function return TRUE if resizing did take place.
******************************************************************************/
FitPictureInFrame(PTERWND w,long line,BOOL ResizePicture)
{
    int i,ParaFID,FrameNo,TextLen=0,pict,ParaCharWidth=0,ParaCharDescent=0;
    long l;
    LPWORD fmt;

    if (!TerArg.PageMode) return FALSE;

    ParaFID=fid(line);
    if (ParaFID==0) return FALSE;      // not within a para frame
    if ((FrameNo=GetFrame(w,line))==-1) return FALSE;
    if (frame[FrameNo].ParaFrameId!=ParaFID) return FALSE;

    // get the length of the text in the frame
    for (l=frame[FrameNo].PageFirstLine;l<=frame[FrameNo].PageLastLine;l++) {
       TextLen+=LineLen(l);
       if (TextLen>2) return FALSE;           // not a picture frame
    }

    // get the picture id
    l=frame[FrameNo].PageFirstLine;
    if (LineLen(l)==0) return FALSE;
    fmt=OpenCfmt(w,l);
    pict=fmt[0];
    CloseCfmt(w,l);
    if (!(TerFont[pict].style&PICT)) return FALSE;

    // resize picture or the frame
    if (ResizePicture) {
       int width,height=0;
       width=ParaFrame[ParaFID].width-2*ParaFrame[ParaFID].margin-ScrToTwipsX(ParaCharWidth);
       if (ParaFrame[ParaFID].MinHeight>0) height=ParaFrame[ParaFID].MinHeight-2*ParaFrame[ParaFID].margin-ScrToTwipsY(ParaCharDescent);
       if (TerFlags&TFLAG_KEEP_PICT_ASPECT && width>0 && height>0) {
          int PictWidth=TerFont[pict].PictWidth;
          int PictHeight=TerFont[pict].PictHeight;
          if (PictWidth>=MulDiv(width,PictHeight,height)) {  // fit width
             TerFont[pict].PictWidth=width;
             TerFont[pict].PictHeight=MulDiv(PictHeight,TerFont[pict].PictWidth,PictWidth);
          }
          else {                                             // fit height
             TerFont[pict].PictHeight=height;
             TerFont[pict].PictWidth=MulDiv(PictWidth,TerFont[pict].PictHeight,PictHeight);
          }
       }
       else {
          if (width>0)  TerFont[pict].PictWidth=width;
          if (height>0) TerFont[pict].PictHeight=height;
       }

       SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
       XlateSizeForPrt(w,pict);
    }
    else {   // resize the frame
       ParaFrame[ParaFID].width=TerFont[pict].PictWidth+2*ParaFrame[ParaFID].margin;
       ParaFrame[ParaFID].MinHeight=0; // let the height adjust as needed

       // invalidate para space text segments
       if (BkPictId) DeleteTextMap(w,TRUE);
       else {
          for (i=0;i<TotalSegments;i++) if (TextSeg[i].ParFrameSpace) DeleteTextSeg(w,i,BkPictId?TRUE:FALSE);
       }
       if (line<RepageBeginLine) RepageBeginLine=line;
    }

    return TRUE;
}

/******************************************************************************
    GetAnchorY:
    Get the Y (screen frame Y) to which the given frame is anchored.
******************************************************************************/
int GetAnchorY(PTERWND w,long AnchorLine)
{
    long  l;
    int   ParaFID,y;

    if (AnchorLine<0) {   // ParaFID specified instead
       ParaFID=(int)(-AnchorLine);
       for (l=0;l<TotalLines;l++) if (fid(l)==ParaFID) break;
       if (l==TotalLines) return 0;
       AnchorLine=l;
    }

    // get the first line after the frames
    for (;AnchorLine<TotalLines;AnchorLine++) if (fid(AnchorLine)==0) break;
    if (AnchorLine==TotalLines) AnchorLine=TotalLines-1;

    // calculate the y position in screen units
    y=LineToUnits(w,AnchorLine);

    // subtract any frame space before
    if (LineInfo(w,AnchorLine,INFO_FRM_SPC_BEF)) y-=PrtToScrY(tabw(AnchorLine)->height);

    if (cid(AnchorLine)) {
      int row=cell[cid(AnchorLine)].row;
      y-=PrtToScrY(TableRow[row].FrmSpcBef);
    }

    return y;
}


/******************************************************************************
    GetParaFrameLine:
    get the line number to which the paraframe should be anchored.  The frame
    should be inserted before this line.
******************************************************************************/
long GetParaFrameLine(PTERWND w,long StartLine)
{
    long l,ParaLine,PageFirstLine,PageLastLine;
    int  len,CurFID=fid(StartLine),LoopCount=0;
    UINT HdrFtr;
    LPBYTE ptr;
    LPWORD fmt;
    BOOL   forward;
    BYTE  LastByte;

    HdrFtr=PfmtId[pfmt(StartLine)].flags&PAGE_HDR_FTR;

    //GetCurPage(w,StartLine);

    PageFirstLine=PageInfo[CurPage].FirstLine;
    if ((CurPage+1)==TotalPages) PageLastLine=TotalLines-1;
    else                         PageLastLine=PageInfo[CurPage].LastLine;
    if (PageFirstLine<RepageBeginLine) RepageBeginLine=PageFirstLine;

    // Determine the initial scan direction
    if (fid(StartLine)==0) forward=FALSE;
    else                   forward=TRUE;

    // scan the lines
    while (LoopCount<2) {
       if (forward) {  // forward scan
          if (HdrFtr) {
             for (l=StartLine;l<TotalLines && (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)==HdrFtr;l++) {
                if (LinePage(l)!=CurPage) continue;
                if (!AnchorParaFound(w,l,CurFID)) continue;
                if ((l+1)<TotalLines && (PfmtId[pfmt(l+1)].flags&PAGE_HDR_FTR)==HdrFtr) return (l+1);
             }
          }
          else {
             for (l=StartLine;l<=PageLastLine;l++) {
                if (LinePage(l)!=CurPage) continue;
                if (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR) continue;
                if (!AnchorParaFound(w,l,CurFID)) continue;
                if ((l+1)<=PageLastLine) return (l+1);
             }
          }
       }
       else {          // backward scan
          if (HdrFtr) {
             for (l=StartLine-1;l>=0 && (PfmtId[pfmt(l)].flags&PAGE_HDR_FTR)==HdrFtr;l--) {
                if (LinePage(l)!=CurPage) continue;
                if (!AnchorParaFound(w,l,CurFID)) continue;
                if ((l+1)<TotalLines && (PfmtId[pfmt(l+1)].flags&PAGE_HDR_FTR)==HdrFtr) return (l+1);
             }
          }
          else {
             if (StartLine==0 && fid(StartLine)==0) return StartLine;     // first line is always valid

             for (l=StartLine-1;l>=(PageFirstLine-1) && l>=0;l--) {
                if (LinePage(l)!=CurPage) continue;
                if (!AnchorParaFound(w,l,CurFID)) continue;
                if ((l+1)<=PageLastLine) return (l+1);
             }
          }
       }
       forward=!forward;
       LoopCount++;
    }

    if (StartLine==0 && fid(StartLine)==0) return StartLine;

    // check if the beginning of the paragraph is acceptable
    for (l=StartLine-1;l>=(PageFirstLine-1) && l>=0;l--) {
       if (LinePage(l)!=CurPage) continue;
       if (False(LineFlags(l)&LFLAG_PARA_FIRST)) continue;  // can only insert frame before a new paragraph
       
       // if in table, can be inserted before the row break line
       if (cid(l) && (!tabw(l) || !(tabw(l)->type&INFO_ROW))) continue;
       
       // can't insert within another frame
       if (fid(l)) if (l>0 && fid(l-1)==fid(l)) continue;

       return l;
    }


    // no lines found, create a para break
    if (StartLine>0 && !(LineFlags(StartLine-1)&LFLAG_HDRS_FTRS)) {
       ParaLine=StartLine-1;
       len=LineLen(ParaLine);
       ptr=pText(ParaLine);
       if (len>0) LastByte=ptr[len-1];
       else       LastByte=0;
       if (LastByte!=ParaChar && LastByte!=CellChar && lstrchr(BreakChars,LastByte)==0) {
          LineAlloc(w,ParaLine,len,len+1);
          ptr=pText(ParaLine);
          fmt=OpenCfmt(w,ParaLine);
          ptr[len]=ParaChar;
          fmt[len]=DEFAULT_CFMT;
          CloseCfmt(w,ParaLine);
       }
    }

    return StartLine;
}

/******************************************************************************
    AnchorParaFound:
    This routine examines the given line to check if a para frame can be
    anchored to the next line.
******************************************************************************/
BOOL AnchorParaFound(PTERWND w, long line, int CurFID)
{
    LPBYTE ptr;
    BYTE LastChar;
    int len;

    if (LineLen(line)==0) return FALSE;

    len=LineLen(line);
    ptr=pText(line);
    LastChar=ptr[len-1];


    if (LastChar!=ParaChar && lstrchr(BreakChars,LastChar)==NULL) return FALSE;

    // if in table, must be at the row break line
    if (cid(line) && (!tabw(line) || !(tabw(line)->type&INFO_ROW))) return FALSE;

    // can not anchor the hdr/ftr delimiter lines
    if ((line+1)<TotalLines && LineFlags(line+1)&LFLAG_HDRS_FTRS) return FALSE;

    if (line<(TotalLines-1) && (fid(line+1)==0 || fid(line+1)!=CurFID))
         return TRUE;
    else return FALSE;
}

/******************************************************************************
    GetBorderLeftSpace:
    This function calculates the left margin and the left border space when
    the border is showing.
    To specify the line number, use the negative parameter for the PageNo.
******************************************************************************/
int GetBorderLeftSpace(PTERWND w, long PageNo)
{
    int sect;

    if (!BorderShowing) return 0;   // border not showing

    if (PageNo<0) PageNo=GetCurPage(w,-PageNo);  // convert line to page number
    sect=TerGetPageSect(hTerWnd,(int)PageNo);
    return (LeftBorderWidth+(int)(PrtResX*TerSect[sect].LeftMargin));
}

/******************************************************************************
    InitFrame:
    Initialize a screen frame.
*******************************************************************************/
BOOL InitFrame(PTERWND w,int FrameNo)
{
    long OldSize,NewSize;
    HGLOBAL hMem;
    LPBYTE pMem;
    int i,delta=10;

    // allocate spaces for more frames
    if ((FrameNo+delta)>=MaxFrames) {
       OldSize=(long)(MaxFrames+1)*sizeof(struct StrFrame);
       while ((FrameNo+delta)>=MaxFrames) {
          if (MaxFrames<100) MaxFrames+=20;    // increase the number of frames
          else               MaxFrames+=50;
       }
       NewSize=(long)(MaxFrames+1)*sizeof(struct StrFrame);

       if ( (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,NewSize)))
          && (NULL!=(pMem=GlobalLock(hMem))) ){

          // move data to new location
          HugeMove(frame,pMem,OldSize);

          // free old handle
          GlobalUnlock(hFrame);
          GlobalFree(hFrame);

          // assign new handle
          hFrame=hMem;              // new array
          frame=(struct StrFrame huge *)pMem;
       }
       else return FALSE;
    }

    FarMemSet(&(frame[FrameNo]),0,sizeof(struct StrFrame));
    frame[FrameNo].width=frame[FrameNo].ScrWidth=9999;         // a high value
    frame[FrameNo].height=frame[FrameNo].ScrHeight=TerWinHeight;
    frame[FrameNo].BackColor=CLR_WHITE;                        // background color of the frame
    frame[FrameNo].LimitY=-1;                                  // any text limiation on the y direction

    for (i=0;i<4;i++) frame[FrameNo].BorderColor[i]=CLR_AUTO;

    return TRUE;
}

/******************************************************************************
    GetParaFrameSlot:
    get the para frame slot from the para frame table.  This function returns -1 if
    a table slot not found.
******************************************************************************/
GetParaFrameSlot(PTERWND w)
{
    int i,ParaFID;
    DWORD OldSize, NewSize;
    int   NewMaxParaFrames;
    HGLOBAL hMem;
    LPBYTE pMem;
    long  l;

    // recapture any unused slots
    if (!InRtfRead) {
       for (i=1;i<TotalParaFrames;i++) ParaFrame[i].InUse=FALSE;
       for (l=0;l<TotalLines;l++) {
          ParaFID=fid(l);
          ParaFrame[ParaFID].InUse=TRUE;
       }
       for (i=0;i<TotalFonts;i++) {
          if (TerFont[i].InUse && TerFont[i].style&PICT && TerFont[i].FrameType!=PFRAME_NONE) {
             ParaFID=TerFont[i].ParaFID;
             ParaFrame[ParaFID].InUse=TRUE;
          }
       }

       // find an open slot
       for (i=1;i<TotalParaFrames;i++) {
          if (!ParaFrame[i].InUse) {
             FarMemSet(&ParaFrame[i],0,sizeof(struct StrParaFrame));
             return i;
          }
       }
    }


    // enlarge the table if needed
    if (TotalParaFrames>=MaxParaFrames) {        // allocate more memory for the ParaFrame table
       NewMaxParaFrames=MaxParaFrames+MaxParaFrames/2;

       if (!Win32 && NewMaxParaFrames>MAX_PARA_FRAMES) NewMaxParaFrames=MAX_PARA_FRAMES;

       OldSize=((DWORD)MaxParaFrames+1)*sizeof(struct StrParaFrame);
       NewSize=((DWORD)NewMaxParaFrames+1)*sizeof(struct StrParaFrame);

       if ( (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,NewSize)))
          && (NULL!=(pMem=GlobalLock(hMem))) ){

          // move data to new location
          HugeMove(ParaFrame,pMem,OldSize);

          // free old handle
          GlobalUnlock(hParaFrame);
          GlobalFree(hParaFrame);

          // assign new handle
          hParaFrame=hMem;              // new array
          ParaFrame=(struct StrParaFrame far *)pMem;

          MaxParaFrames=NewMaxParaFrames;    // new max ParaFrames
       }
       else {
          PrintError(w,MSG_OUT_OF_MEM,"GetParaFrameSlot");
          return -1;
       }
    }

    // use the next slot
    if (TotalParaFrames<MaxParaFrames) {
       TotalParaFrames++;
       FarMemSet(&ParaFrame[TotalParaFrames-1],0,sizeof(struct StrParaFrame));
       return (TotalParaFrames-1);
    }

    PrintError(w,MSG_OUT_OF_PARAFID_SLOT,"GetParaFrameSlot");

    return -1;
}

/******************************************************************************
    AnchorPictFrame:
    Anchor the given picture frame to a different line/col position.
******************************************************************************/
BOOL AnchorPictFrame(PTERWND w, int pict, long LineNo, int col)
{
    long PictLine=0;
    int PictCol=0;
    LPBYTE ptr;
    LPWORD fmt;

    // move the picture character
    if (!TerLocateFontId(hTerWnd,pict,&PictLine,&PictCol)) return FALSE;

    MoveLineData(w,PictLine,PictCol,1,'D');  // delete the picture character
    MoveLineData(w,LineNo,col,1,'B');      // make space for one character

    ptr=pText(LineNo);
    fmt=OpenCfmt(w,LineNo);
    ptr[col]=PICT_CHAR;
    fmt[col]=pict;
    CloseCfmt(w,LineNo);

    ReleaseUndo(w);

    return TRUE;
}
 
/******************************************************************************
    ReposPictFrames:
    Reposition the picture frames such that they are at the beginning of a paragraph
******************************************************************************/
ReposPictFrames(PTERWND w)
{                 
    int i,ParaFID;
    long line,PictLine;
    int col,PictPage;
    int font1,font2;

    for (i=0;i<TotalFonts;i++) {
       if (!TerFont[i].InUse || !(TerFont[i].style&PICT) || TerFont[i].FrameType!=PFRAME_FLOAT) continue;
       
       // check if the frame moves with the character
       ParaFID=TerFont[i].ParaFID;
       //if (ParaFID>0 && ParaFID<TotalParaFrames && True(ParaFrame[ParaFID].flags&(PARA_FRAME_IGNORE_X|PARA_FRAME_IGNORE_Y))) continue;

       PictLine=0;
       col=0;
       if (!TerLocateFontId(hTerWnd,i,&PictLine,&col)) continue;
    
       font1=GetCurCfmt(w,PictLine,0);
       font2=GetCurCfmt(w,PictLine,col);
       if (TerFont[font1].FieldId!=0 || TerFont[font2].FieldId!=0) continue;  // can't move picture in or out of a field

       PictPage=LinePage(PictLine);

       for (line=PictLine;line>=0 && !(LineFlags(line)&LFLAG_PARA_FIRST) && (line!=PageInfo[PictPage].FirstLine);line--);  // find the beginning of the paragraph
       if (line==PageInfo[PictPage].FirstLine) continue;
       if (line<0) line=0;
       
       if (PictLine!=line || col!=0) {
          long OldPictPos=RowColToAbs(w,PictLine,col);
          long NewPictPos=RowColToAbs(w,line,0);
          long CursPos=RowColToAbs(w,CurLine,CurCol);
          AnchorPictFrame(w,i,line,0);
          if (CursPos>=NewPictPos && CursPos<=OldPictPos) {  // adjust cursor position
             CurCol++;  // the picture character is now taking up an extract space before the current column
             if (CurCol>=LineLen(CurLine) && LineLen(CurLine)>0) CurCol=LineLen(CurLine)-1;
          }
       }
    } 
 
    ResetUintFlag(TerOpFlags2,TOFLAG2_REPOS_PICT_FRAMES);

    return TRUE;
} 

#pragma optimize("g",off)  // turn-off global optimization
/******************************************************************************
    TerMovePictFrame2:
    The FrameX is relative to the left margin.  The DeltaY is relative to
    the frame's vertical base position (page top, margin, or para)
    This function works within multiple pages. DeltaX and DeltaY are specified in
    the twips units
******************************************************************************/
BOOL WINAPI _export TerMovePictFrame2(HWND hWnd, int pict, int DeltaX, int DeltaY)
{
    PTERWND w;
    int PageY,ParaFID,x,y,PageBegY,PageEndY,EndY,CurFrameNo,AnchorLineY,FramePage;
    long AnchorLine,ParaLine;
    BOOL WithinCurPage=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    if (pict<0 || pict>=TotalFonts) return FALSE;
    if (!TerFont[pict].InUse || !(TerFont[pict].style&PICT) || TerFont[pict].FrameType==PFRAME_NONE) return FALSE;

    ParaFID=TerFont[pict].ParaFID;
    if (!ParaFrame[ParaFID].InUse) return FALSE;

    // check if the frame crosses the page boundaries
    y=ParaFrame[ParaFID].y+DeltaY;
    y=TwipsToScrY(y);
    if (ParaFrame[ParaFID].PageNo>FirstFramePage) y+=FirstPageHeight;

    EndY=y+TwipsToScrY(ParaFrame[ParaFID].height);

    PageBegY=0;
    PageEndY=FirstPageHeight;
    FramePage=ParaFrame[ParaFID].PageNo;
    if (FramePage>FirstFramePage) {
       PageBegY=PageEndY;
       PageEndY=CurPageHeight;
    }
    
    WithinCurPage=(y>=PageBegY && EndY<=PageEndY);

    if (WithinCurPage) return TerMovePictFrame(hTerWnd,pict,ParaFrame[ParaFID].x+DeltaX,ParaFrame[ParaFID].ParaY+DeltaY);
 
    // get the anchor line
    if (TotalLines<=UNDO_LINE_LIMIT) SaveUndo(w,ParaFID,0,0,0,UNDO_FRAME);
    else                             ReleaseUndo(w);

    // Get the closest text line. Allow negative value
    ParaFrame[ParaFID].x+=DeltaX;
    ParaFrame[ParaFID].OrgX+=DeltaX;
    x=TwipsToScrX(ParaFrame[ParaFID].x);


    // disable the current frame temporariy
    for (CurFrameNo=0;CurFrameNo<TotalFrames;CurFrameNo++)
       if (!frame[CurFrameNo].empty && frame[CurFrameNo].ParaFrameId==ParaFID) break;
    if (CurFrameNo<TotalFrames) frame[CurFrameNo].empty=TRUE;

    ParaLine=UnitsToLine(w,x,y);

    SetPageFromY(w,y);                      // set the page number from y

    if (CurFrameNo<TotalFrames) frame[CurFrameNo].empty=FALSE; // re-enable

    // get the anchor line before which the frame will be inserted
    AnchorLine=GetParaFrameLine(w,ParaLine);    // get the para begin line closest to this line
    AnchorLineY=GetAnchorY(w,AnchorLine);

    // move the picture character
    AnchorPictFrame(w,pict,AnchorLine,0);

    // make the para frame paragraph relative
    //ResetUintFlag(ParaFrame[ParaFID].flags,(PARA_FRAME_VMARG|PARA_FRAME_VPAGE));
    
    if (CurPage==0 && y<0) y=0;
    PageY=FrameToPageY(w,ScrToTwipsY(y));
    if ((PageY-ParaFrame[ParaFID].height)<0) PageY=ParaFrame[ParaFID].height;

    if (ParaFrame[ParaFID].flags&(PARA_FRAME_VMARG|PARA_FRAME_VPAGE)) {
       ParaFrame[ParaFID].ParaY=PageY;
       if (ParaFrame[ParaFID].flags&PARA_FRAME_VMARG) {       // y relative to the margin
          int sect=GetSection(w,AnchorLine);
          ParaFrame[ParaFID].ParaY-=InchesToTwips(TerSect[sect].TopMargin);
       } 
    } 
    else ParaFrame[ParaFID].ParaY=PageY-ScrToTwipsY(AnchorLineY);
    
    
    ParaFrame[ParaFID].PageNo=LinePage(AnchorLine);

    TerArg.modified++;
    
    CurLine=AnchorLine;
    CurCol=0;

    TerRepaginate(hTerWnd,TRUE);
    //Repaginate(w,FALSE,TRUE,0,TRUE);    // selective repaginate and paint

    return TRUE;
}
#pragma optimize("",off)  // restore optimization
 
