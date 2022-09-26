/*==============================================================================
   TER_FRM2.C
   TER Frame and drawing object functions

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
    TerSetFrameMarginDist:
    Set the minimum distance from margin for the frame at which the text starts wrapping
    around the frame.
******************************************************************************/
BOOL WINAPI _export TerSetFrameMarginDist(HWND hWnd, int dist)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    FrameDistFromMargin=dist;

    return TRUE;
}

/******************************************************************************
    TerSetFrameTextDist:
    Set the minimum distance to maintain between text and the frame border.
******************************************************************************/
BOOL WINAPI _export TerSetFrameTextDist(HWND hWnd, int ParaFID,int dist)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (ParaFID<=0) ParaFID=fid(CurLine);
    if (ParaFID<=0 || ParaFID>=TotalParaFrames || !ParaFrame[ParaFID].InUse) return false;

    ParaFrame[ParaFID].DistFromText=dist;

    RequestPagination(w,true);

    return TRUE;
}


/******************************************************************************
    BlankHdrFrameHeight:
    Get the height of the blank header frame.
*******************************************************************************/
int BlankHdrFrameHeight(PTERWND w, long FirstLine, long LastLine, int sect)
{
    int HdrMargin,HiddenY=0;

    // get the top margin and hidden top area height
    HdrMargin=(int)(TerSect[sect].HdrMargin*PrtResY);   // top margin in printer units

    HiddenY=TerSect1[sect].HiddenY;

    return (HdrMargin-HiddenY/*-TwipsToPrtY(40)*/);  // leave 40 twips for the dotted frame line
}

/******************************************************************************
    CreateSubTableFrames:
    Create the frames for the sub table.
*******************************************************************************/
long CreateSubTableFrames(PTERWND w,long FirstLine,int ParentLevel,LPINT pFrameNo,LPINT pTableHt,
                          int ColumnX, int PageNo,int sect,int y, int ScrY,int HiddenY, int HdrMargin, 
                          int HdrHeight,int TopSect,int TopLeftMargin, int ColumnNo, LPINT pHasPictFrames, int pass,
                          int BoxFrame, int ParaFrameId)
{
    long l;
    int FrameNo=(*pFrameNo),ParentCell,CurLevel=ParentLevel+1,CurCell,ParentRow;
    int FirstFrame=0,FrameHt,CurColHeight,MaxColumnHeight,CellX,ColumnWidth;
    int ScrFrameHt,ScrCurColHeight,ScrMaxColumnHeight;
    long PrevFrameLastLine=0;
    int  TableRowIndent,CellWidth,SubTableHt;
    int ParentCellMargin,FirstCellFrame,TableRowHeight,ScrTableRowHeight;
    BOOL EndOfSubTable=FALSE,RowBreak,CellBreak;
    long FrameFirstLine,FrameNextLine,FrameLastLine;
    int  CurLineHt,PrevCell,CurRowId,PrevRowId;
    int  PassFlags=0,LeftWidth,RightWidth,TopWidth,BotWidth;
    BOOL CellFramed;
    COLORREF BorderColor[4];

    dm("CreateSubTableFrames");

    (*pTableHt)=0;
   
    if (cid(FirstLine)==0) return FirstLine;


    // find the cell at at the current level
    CurCell=0;
    for (l=FirstLine;l<TotalLines;l++) {
       if (TableLevel(w,l)==ParentLevel) return (l-1);  // current level not found
       if (TableLevel(w,l)==(ParentLevel+1)) {
          CurCell=cid(l);
          break;
       }
    }
    if (CurCell==0) return (TotalLines-1);     // current level not found

    ParentCell=cell[CurCell].ParentCell;
    ParentRow=cell[ParentCell].row;
    ParentCellMargin=TwipsToPrtX(cell[ParentCell].margin);
    ColumnWidth=TwipsToPrtX(cell[ParentCell].width)-2*ParentCellMargin;
    ColumnX+=ParentCellMargin;

    // check if parent row indentation needs to be added to ColumnX
    if (cell[ParentCell].PrevCell<=0 && cell[ParentCell].level>0) {   // get the row indentation - level 0 ColumnX is already adjusted in CreateFrames function
       int row=cell[ParentCell].row;
       ColumnX+=TableRow[row].CurIndent;
    }    


    // find the space needed at the top of the parent cell
    CurColHeight=0;         // current column height
    GetCellFrameBorder(w,ParentCell,&LeftWidth,&RightWidth,&TopWidth,&BotWidth,PageNo,BorderColor);
    if (CellAux[ParentCell].flags&CAUX_SET_TOP_SPACE) TopWidth=cell[ParentCell].margin;  // use extra space
    CurColHeight=TwipsToPrtY(TopWidth);
    ScrCurColHeight=PrtToScrY(CurColHeight);


    // divide text into columns
    FirstFrame=FrameNo;
    if (!InitFrame(w,FrameNo))  AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 1)",1);

    FrameHt=0;              // adds up the line height
    ScrFrameHt=0;
    MaxColumnHeight=0;
    ScrMaxColumnHeight=0;
    CellX=ColumnX;          // x position of the previous column or cell

    PrevFrameLastLine=FirstLine-1;
    FirstCellFrame=-1;
    TableRowHeight=0;       // height of a table row
    ScrTableRowHeight=0;    // height of a table row
    CurCell=0;
    FrameFirstLine=-1;
    SubTableHt=0;           // subtable height before the line
    TableRowIndent=0;
    CellFramed=FALSE;

    for (l=FirstLine;;l++) {
       if (TableLevel(w,l)<CurLevel) EndOfSubTable=TRUE;   // out of the sub table

       if (l<TotalLines && pass==PASS_BODY && LinePage(l)!=PageNo && cid(l) && !EndOfSubTable) continue;  // this line does not belong to this page
        
        
       // set the frame line variables
       if (FrameFirstLine<0) FrameFirstLine=FrameNextLine=l;
       FrameLastLine=FrameNextLine;
       FrameNextLine=l;

       CurLineHt=0;
       if (l<TotalLines) CurLineHt=LineHt(l);

       // reset frame types and break types
       RowBreak=CellBreak=FALSE;

       // get current and previous cell ids
       PrevCell=CurCell;
       if (l<TotalLines && !EndOfSubTable) CurCell=LevelCell(w,CurLevel,l);
       else                                CurCell=0;
       
       if (EndOfSubTable && PrevCell==0 && CurCell==0) break;  // This would happen if the whole subtable falls in the following page


       CurRowId=cell[CurCell].row;
       PrevRowId=cell[PrevCell].row;

       if (CurCell!=PrevCell && PrevCell!=0)                   CellBreak=TRUE;
       if (CurRowId!=PrevRowId && PrevRowId!=0 && PrevCell!=0) RowBreak=TRUE;
       if (EndOfSubTable) CellBreak=RowBreak=TRUE;

       if (CellBreak || RowBreak) {// start next frame
          CreateCellFrame(w,CurLevel,&FrameNo,y,ScrY, &FrameHt,&ScrFrameHt, 
                 CurColHeight,ScrCurColHeight, &FirstCellFrame, 
                 &TableRowHeight, &ScrTableRowHeight,&CellX,TableRowIndent,
                 PageNo,TopLeftMargin,ColumnNo,sect,PrevCell, &CellFramed,
                 FrameFirstLine, FrameLastLine, PassFlags, RowBreak,
                 &CellWidth, ColumnWidth,1,0,ColumnWidth,ColumnX,ColumnWidth,BoxFrame,ParaFrameId);


          // logic after cell frame creation
          FrameFirstLine=FrameNextLine;      // for the next frame

          // reset frame height for cells and para frames
          if (!RowBreak) FrameHt=ScrFrameHt=0;

          // adjust operational variables
          CurColHeight+=FrameHt;
          ScrCurColHeight+=ScrFrameHt;
          FrameHt=ScrFrameHt=0;            // reset the height for the frame

          // set x position for the next frame
          CellX=CellX+CellWidth;
          if (RowBreak)   CellX=ColumnX; // x for the first cell of the next row

       }
       if (EndOfSubTable) break;  // end of sub table

       // check if a subtable needs to be created
       if (l<TotalLines && TableLevel(w,l)>CurLevel) {
          int BeginX=CellX;
          int cl,FirstCell=0;
          if (FirstLine<TotalLines) FirstCell=LevelCell(w,CurLevel,FirstLine);
          if (FirstCell>0 && cell[CurCell].PrevCell>0) {    // subtable not starting at column 0, so add the column width of the empty columns
             ParentRow=cell[FirstCell].row;
             for (cl=TableRow[ParentRow].FirstCell;cl>0 && cl!=FirstCell;cl=cell[cl].NextCell) BeginX+=TwipsToPrtX(cell[cl].width);
          } 

          l=CreateSubTableFrames(w,l,CurLevel,&FrameNo,&SubTableHt,BeginX,
                                 PageNo,sect,y+CurColHeight+FrameHt,
                                 ScrY+ScrCurColHeight+ScrFrameHt,HiddenY,HdrMargin,HdrHeight,TopSect,
                                 TopLeftMargin,ColumnNo,pHasPictFrames,pass,BoxFrame,ParaFrameId);
          FrameHt+=SubTableHt;
          ScrFrameHt+=PrtToScrY(SubTableHt);
          continue;
       }

       // record line y and x position, set PageHasControl flag
       if (l<TotalLines) SetFrameLineInfo(w,l,y,ScrY,&FrameHt,&ScrFrameHt,CurColHeight,
                 CellX,&TableRowIndent,PageNo,HiddenY,HdrMargin,HdrHeight,
                 TopSect,TopLeftMargin,ColumnNo,pHasPictFrames,sect,CurLineHt);

    }

    // pass values by reference
    (*pFrameNo)=FrameNo;
    (*pTableHt)=CurColHeight;  


    return (l-1);    // return the last line of the subtable
}

/******************************************************************************
    CreateCellFrame:
    Create a Cell frame.
*******************************************************************************/
CreateCellFrame(PTERWND w,int level,LPINT pFrameNo,int y,int ScrY, LPINT pFrameHt,LPINT pScrFrameHt, 
                 int CurColHeight,int ScrCurColHeight, LPINT pFirstCellFrame, 
                 LPINT pTableRowHeight, LPINT pScrTableRowHeight,LPINT pCellX,int TableRowIndent,
                 int PageNo,int TopLeftMargin,int ColumnNo,int sect,int PrevCell, LPINT pCellFramed,
                 long FrameFirstLine, long FrameLastLine, UINT PassFlags, BOOL RowBreak,
                 LPINT pCellWidth, int TextWidth,int MaxColumns, int ColumnSpace, int PageWdth,
                 int ColumnX,int ColumnWidth, int BoxFrame, int ParaFrameId)
{
    int TblCellHeight,LeftWidth,RightWidth,TopWidth,BotWidth,SpaceTop,SpaceBot;
    int row,CellMargin,FrameSpaceHt;
    BOOL IsFirstCellFrame=FALSE;
    UINT border,ParentCellMargin;
    int  i,x,PrevRowId=cell[PrevCell].row;
    COLORREF BorderColor[4];

    //dm("CreateCellFrame");

    // define the argument for easy reference
    #define FrameNo (*pFrameNo)
    #define FrameHt (*pFrameHt)
    #define ScrFrameHt (*pScrFrameHt)
    #define FirstCellFrame (*pFirstCellFrame)
    #define TableRowHeight (*pTableRowHeight)
    #define ScrTableRowHeight (*pScrTableRowHeight)
    #define CellX (*pCellX)
    #define CellFramed (*pCellFramed)
    #define CellWidth (*pCellWidth)

    
    // calcualte parent cell margin
    if (level>0) {
       int ParentCell=cell[PrevCell].ParentCell;
       ParentCellMargin=TwipsToPrtX(cell[ParentCell].margin);
    }
    else ParentCellMargin=0;

    // adjust y from any frame space before the row
    row=cell[PrevCell].row;
    FrameSpaceHt=TableRow[row].FrmSpcBef;
    if (FrameSpaceHt>0) {
       y+=FrameSpaceHt;
       ScrY+=PrtToScrY(FrameSpaceHt);
    } 

    // update table row height
    if (FirstCellFrame==-1) {
        int MinHeight;
        FirstCellFrame=FrameNo; // first frame for the row
        MinHeight=TableRow[PrevRowId].MinHeight;
        if (MinHeight<0) MinHeight=-MinHeight;   // exact match
        if (TableRow[PrevRowId].MinPictHeight>MinHeight) MinHeight=TableRow[PrevRowId].MinPictHeight;  // height of the picture frames
        TableRowHeight=TwipsToPrtY(MinHeight);
        ScrTableRowHeight=TwipsToScrY(MinHeight);
        IsFirstCellFrame=TRUE;
    }

    // set the border and space top/bottom
    border=GetCellFrameBorder(w,PrevCell,&LeftWidth,&RightWidth,&TopWidth,&BotWidth,PageNo,BorderColor);
    if (CellAux[PrevCell].flags&CAUX_SET_TOP_SPACE) SpaceTop=TwipsToPrtY(cell[PrevCell].margin);  // use extra space
    else                                            SpaceTop=TwipsToPrtY(TopWidth);
    SpaceBot=TwipsToPrtY(BotWidth);

    // adjust the frame height
    FrameHt+=(SpaceTop+SpaceBot);
    ScrFrameHt+=PrtToScrY(SpaceTop+SpaceBot);
    
    // keep track of max row height
    TblCellHeight=0;        // height contribute by this cell toward the row
    if (IsLastSpannedCell(w,PrevCell)) TblCellHeight=GetLastSpannedCellHeight(w,PrevCell,NULL,PageNo);  // height contrinution of this cell toward the row height
    else if ((cell[PrevCell].RowSpan==1 || IsPageLastRow(w,cell[PrevCell].row,PageNo)) 
             && !(cell[PrevCell].flags&CFLAG_ROW_SPANNED)) {
         TblCellHeight=FrameHt;
         if (cell[PrevCell].TextAngle!=0) TblCellHeight=TwipsToPrtY(MIN_VTEXT_CELL_HEIGHT);  // vertical text does not affect the row height
         
         if (cell[PrevCell].flags&CFLAG_VALIGN_BASE) TblCellHeight+=CellAux[PrevCell].SpaceBefore;  // add base alignment adjustment
    }
    if (TableRow[PrevRowId].MinHeight>=0) {
        if (TblCellHeight>TableRowHeight) TableRowHeight=TblCellHeight;
    }
    ScrTableRowHeight=PrtToScrY(TableRowHeight);

    if (RowBreak) {            // last cell of the table
        int row=cell[PrevCell].row;

        TableRow[row].height=TableRowHeight;  // record the updated table row height

        for (i=FirstCellFrame;i<=FrameNo;i++) {
            if (frame[i].level!=level) continue;
            frame[i].height=TableRowHeight;
            frame[i].ScrHeight=ScrTableRowHeight;
        }
        FirstCellFrame=-1;      // reset for the next row

    }

    // create the empty row indentation frame
    if (IsFirstCellFrame) {
        int row=cell[PrevCell].row;

        // create the empty frame to reserve any frame space
        if (TableRow[row].FrmSpcBef>0) {
            int SpcBef=TableRow[row].FrmSpcBef;
            frame[FrameNo].empty=TRUE;
            frame[FrameNo].level=level;
            frame[FrameNo].sect=sect;     // section to which the frame belongs
            frame[FrameNo].x=CellX;
            frame[FrameNo].y=y+CurColHeight-SpcBef;  // relative to top margin
            frame[FrameNo].ScrY=ScrY+ScrCurColHeight-PrtToScrY(SpcBef);  // relative to top margin
            frame[FrameNo].width=TextWidth;
            frame[FrameNo].BoxFrame=BoxFrame;
            frame[FrameNo].ParaFrameId=ParaFrameId;
            if (ParaFrameId>0) frame[FrameNo].ZOrder=ParaFrame[ParaFrameId].ZOrder;
            if (!BorderShowing && level==0) frame[FrameNo].flags|=FRAME_RIGHTMOST;

            frame[FrameNo].height=SpcBef;
            frame[FrameNo].ScrHeight=PrtToScrY(SpcBef);

            FrameNo++;                      // advance to the next frame
            if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 1)",1);

            FirstCellFrame=FrameNo; // first frame for the row
        }

        TableRow[PrevRowId].FirstFrame=FrameNo;  // first frame of the row

        frame[FrameNo].empty=TRUE;
        frame[FrameNo].level=level;
        frame[FrameNo].sect=sect;     // section to which the frame belongs
        frame[FrameNo].y=y+CurColHeight;  // relative to top margin
        frame[FrameNo].ScrY=ScrY+ScrCurColHeight;  // relative to top margin
        frame[FrameNo].BoxFrame=BoxFrame;
        frame[FrameNo].ParaFrameId=ParaFrameId;
        if (ParaFrameId>0) frame[FrameNo].ZOrder=ParaFrame[ParaFrameId].ZOrder;
        frame[FrameNo].CellId=PrevCell;

        frame[FrameNo].width=TableRow[PrevRowId].CurIndent;   // TableRowIndent;
        frame[FrameNo].x=CellX;

        frame[FrameNo].border=0;
        frame[FrameNo].RowId=PrevRowId;
        frame[FrameNo].height=FrameHt=TableRowHeight;
        frame[FrameNo].ScrHeight=ScrFrameHt=ScrTableRowHeight;
        frame[FrameNo].flags|=FRAME_FIRST_ROW_FRAME;

        CellX=CellX+frame[FrameNo].width;

        FrameNo++;                      // advance to the next frame
        if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 1)",1);
    }

    // create any intervening empty cell frames
    FrameEmptyCells(w,PrevRowId,&CellFramed,PrevCell,&CellX,y+CurColHeight,TableRowHeight,&FrameNo,sect,PageNo);

    // create the text frame
    CellMargin=TwipsToPrtX(cell[PrevCell].margin);
    CellWidth=TwipsToPrtX(cell[PrevCell].width);

    frame[FrameNo].empty=FALSE;
    frame[FrameNo].level=level;
    frame[FrameNo].sect=sect;     // section to which the frame belongs
    frame[FrameNo].PageFirstLine=FrameFirstLine;
    frame[FrameNo].PageLastLine=FrameLastLine;
    frame[FrameNo].y=y+CurColHeight;  // relative to top margin
    frame[FrameNo].ScrY=ScrY+ScrCurColHeight;  // relative to top margin
    frame[FrameNo].BoxFrame=BoxFrame;
    frame[FrameNo].ParaFrameId=ParaFrameId;
    if (ParaFrameId>0) frame[FrameNo].ZOrder=ParaFrame[ParaFrameId].ZOrder;
    frame[FrameNo].shading=cell[PrevCell].shading; // shading percentage
    
    frame[FrameNo].BackColor=cell[PrevCell].BackColor; // background color
    if (cell[PrevCell].BackColor==CLR_WHITE && cell[PrevCell].ParentCell>0) {          // check if parent cell is non-white
       int ParentCell=cell[PrevCell].ParentCell;
       while (ParentCell>0) {
          if (cell[ParentCell].BackColor!=CLR_WHITE) break;
          ParentCell=cell[ParentCell].ParentCell;
       }
       if (ParentCell>0) frame[FrameNo].BackColor=0xFDFDFD;  // to force drawing of white color 
    } 

    frame[FrameNo].flags|=PassFlags;  // flags for the pass
    if (cell[PrevCell].flags&CFLAG_FORCE_BKND_CLR) frame[FrameNo].flags|=FRAME_FORCE_BKND_CLR;  // enforce background color even for default color

    frame[FrameNo].x=CellX;
    frame[FrameNo].width=CellWidth;
    frame[FrameNo].SpaceLeft=CellMargin;
    if (HtmlMode && IsFirstCellFrame && border&BORDER_LEFT) frame[FrameNo].SpaceLeft+=ScrToPrtX(3);  // add width of the html table border
    frame[FrameNo].SpaceRight=CellMargin;

    // record the frame x space for the row
    if (IsFirstCellFrame) TableAux[PrevRowId].FrmBegX=frame[FrameNo].x;
    TableAux[PrevRowId].FrmEndX=frame[FrameNo].x+frame[FrameNo].width;;

    frame[FrameNo].border=border;
    frame[FrameNo].BorderWidth[BORDER_INDEX_LEFT]=LeftWidth;
    frame[FrameNo].BorderWidth[BORDER_INDEX_RIGHT]=RightWidth;
    frame[FrameNo].BorderWidth[BORDER_INDEX_TOP]=TopWidth;
    frame[FrameNo].BorderWidth[BORDER_INDEX_BOT]=BotWidth;

    // set border color
    for (i=0;i<4;i++) frame[FrameNo].BorderColor[i]=BorderColor[i];
    
    frame[FrameNo].RowId=PrevRowId;

    if (cell[PrevCell].flags&CFLAG_ROW_SPANNED)    // set height
        frame[FrameNo].TextHeight=frame[FrameNo].height=frame[FrameNo].ScrHeight=0;
    else {
        frame[FrameNo].TextHeight=ScrFrameHt-PrtToScrY(SpaceBot);
        frame[FrameNo].height=TableRowHeight;
        frame[FrameNo].ScrHeight=ScrTableRowHeight;
    }
    FrameHt=TableRowHeight;
    ScrFrameHt=ScrTableRowHeight;

    frame[FrameNo].SpaceTop=SpaceTop;
    frame[FrameNo].SpaceBot=SpaceBot;
    frame[FrameNo].CellId=PrevCell;

    CellFramed=PrevCell;
        
    FrameNo++;                      // advance to the next frame
    if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 4)",1);

    // on row break create an empty frame on the right to cover any blank area
    x=frame[FrameNo-1].x+frame[FrameNo-1].width;  // right edge of the right most frame
    if (RowBreak) {
        if (true || level==0) FrameEmptyCells(w,PrevRowId,&CellFramed,0,&x,frame[FrameNo-1].y,frame[FrameNo-1].height,&FrameNo,sect,PageNo);

        TableRow[PrevRowId].LastFrame=FrameNo;

        frame[FrameNo].empty=TRUE;
        frame[FrameNo].level=level;
        frame[FrameNo].y=frame[FrameNo-1].y;
        frame[FrameNo].ScrY=frame[FrameNo-1].ScrY;
        frame[FrameNo].x=x;
        frame[FrameNo].BoxFrame=BoxFrame;
        frame[FrameNo].ParaFrameId=ParaFrameId;
        if (ParaFrameId>0) frame[FrameNo].ZOrder=ParaFrame[ParaFrameId].ZOrder;

        frame[FrameNo].width=ColumnX+ColumnWidth+ColumnSpace-x;
        if (level==0) {
            if (ParaFrameId>0) { 
               int right=frame[BoxFrame].x+frame[BoxFrame].width-frame[BoxFrame].BorderWidth[BORDER_INDEX_RIGHT];
               frame[FrameNo].width=right-frame[FrameNo].x;
            } 
            else if (BorderShowing) {
              if (MaxColumns==1 || ((ColumnNo+1)==MaxColumns)) {
                  int sect=frame[FrameNo-1].sect;
                  frame[FrameNo].width=PageWdth-LeftBorderWidth-(int)(TerSect[sect].RightMargin*PrtResX)-x;
               }
            }
            else frame[FrameNo].flags|=FRAME_RIGHTMOST;
        }

        if (frame[FrameNo].width<0) frame[FrameNo].width=0;

        frame[FrameNo].height=frame[FrameNo-1].height;
        frame[FrameNo].ScrHeight=frame[FrameNo-1].ScrHeight;
        frame[FrameNo].sect=frame[FrameNo-1].sect;
        frame[FrameNo].RowId=frame[FrameNo-1].RowId;
        frame[FrameNo].CellId=frame[FrameNo-1].CellId;  // set cell id so that frame height is adjusted properly when this frame is after a spanning cell frame
        frame[FrameNo].flags|=(PassFlags|FRAME_LAST_ROW_FRAME);     // last frame for the row

        CellFramed=0;                   // initialize for the next row
        
        FrameNo++;                      // advance to the next frame
        if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 4)",1);
    }

    // adjust FrameHt to FrameSpace before
    FrameHt+=FrameSpaceHt;
    ScrFrameHt+=PrtToScrY(FrameSpaceHt);

    // undefine the arguments;
    #undef FrameNo
    #undef FrameHt
    #undef ScrFrameHt
    #undef FirstCellFrame
    #undef TableRowHeight
    #undef ScrTableRowHeight
    #undef CellX
    #undef CellFramed
    #undef CellWidth

    return TRUE;
}

/******************************************************************************
    SortFrames:
    Sort the frames in the Z Order
*******************************************************************************/
SortFrames(PTERWND w)
{
    int i,j,sort=FALSE;
    double diff;
    BOOL swap;

    struct StrZOrder {
      double ZOrder;
      int level;
      int FrameId;
      int line;
      int ParaFID;
      int CellId;
    } far *ZFrame;

    // assing the default painting order
    FramesSorted=FALSE;
    if (repaginating) return TRUE;

    for (i=0;i<TotalFrames;i++) {
       frame[i].DispFrame=i;   // default paint order
       if (frame[i].ZOrder!=0 || frame[i].level>0) sort=TRUE;
    }
    if (!sort) return TRUE;         // no need to sort

    // sort the frames
    if (NULL==(ZFrame=OurAlloc(sizeof(struct StrZOrder)*TotalFrames))) return TRUE;

    for (i=0;i<TotalFrames;i++) {
       if (frame[i].ParaFrameId>0 && frame[i].ZOrder==0) {
          int ParaFID=frame[i].ParaFrameId;
          if (ParaFrame[ParaFID].pict>0) frame[i].ZOrder=1;  // display above the text
          if (ParaFrame[ParaFID].FillPict>0) frame[i].ZOrder=-1;  // display background picture below the text
       }
       
       ZFrame[i].ZOrder=frame[i].ZOrder;
       ZFrame[i].level=frame[i].level;
       ZFrame[i].FrameId=i;
       ZFrame[i].line=(int)frame[i].PageFirstLine;
       ZFrame[i].ParaFID=(int)frame[i].ParaFrameId;

       // display header frames before the body text
       if (!frame[i].empty) {
          int ParaId=pfmt(frame[i].PageFirstLine);
          if (PfmtId[ParaId].flags&PAGE_HDR) {
             ZFrame[i].ZOrder-=16384;  // display first
             if (ZFrame[i].ZOrder>=0) ZFrame[i].ZOrder=-1;
          } 
       } 

       // Display the page border frames the last
       if (frame[i].flags&FRAME_BORDER) ZFrame[i].ZOrder=30000;

       if (frame[i].ParaFrameId>0 && frame[i].BoxFrame==i && !(frame[i].flags&FRAME_NON_TEXT_DO) 
           &&  frame[i].empty) ZFrame[i].ZOrder=ZFrame[i].ZOrder-.1;  // so the box frame display before the 

    }

    // Convert cooridnates to screen pixel values
    for (i=0;i<TotalFrames;i++) {
       int iFrame=ZFrame[i].FrameId;

       for (j=i+1;j<TotalFrames;j++) {
          int jFrame=ZFrame[j].FrameId;

          // ensure that box frame is painted before any embeeded table frames
          if (frame[iFrame].ParaFrameId>0 && frame[iFrame].ParaFrameId==frame[jFrame].ParaFrameId 
             && frame[iFrame].BoxFrame==jFrame) swap=true;  // paint box frames first
          else {
             swap=false;
             diff=(ZFrame[j].ZOrder-ZFrame[i].ZOrder);
          
             if (diff<0) swap=true;
             else if (diff==0) {
                diff=(ZFrame[j].level-ZFrame[i].level);
                if (diff<0) swap=true;
                else if (diff==0) {
                   diff=(ZFrame[j].line-ZFrame[i].line);
                   if (diff<0 && (ZFrame[j].ParaFID>0 || ZFrame[i].ParaFID>0)) swap=true;
                } 
             }
          }    

          if (swap) {
             SwapDbls(&ZFrame[i].ZOrder,&ZFrame[j].ZOrder);
             SwapInts(&ZFrame[i].level,&ZFrame[j].level);
             SwapInts(&ZFrame[i].FrameId,&ZFrame[j].FrameId);
             SwapInts(&ZFrame[i].line,&ZFrame[j].line);
             SwapInts(&ZFrame[i].ParaFID,&ZFrame[j].ParaFID);
          }
       }
    }

    for (i=0;i<TotalFrames;i++) frame[i].DispFrame=ZFrame[i].FrameId;

    OurFree(ZFrame);

    FramesSorted=TRUE;

    return TRUE;
}


