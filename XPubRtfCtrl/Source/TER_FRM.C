/*==============================================================================
   TER_FRM.C
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
    CreateFrames:
    Create text frame for the current page.  The second argument specifies
    frames for the printer.
*******************************************************************************/
void CreateFrames(PTERWND w, BOOL printer, int PageNo, int LastPage)
{
    int  FrameNo=0,FirstFrame,LeftMargin,RightMargin,TextWidth=0,
         ColumnHeight,MaxColumns,LastY,
         i;
    int  ColumnsCreated,TextHeight,SaveSect,BlankHdrFrameHt;
    int  FtrTextHeight,SpannedHeight;
    long l,line,BreakLine,FirstLine,LastLine;
    BOOL ColBreak,RowBreak,CellBreak,ColumnFrame,CellFrame;
    int  CurCell,CurRowId,PrevCell,PrevRowId;
    int  TableRowHeight,FirstCellFrame,PageFirstFrame;
    long PrevFrameLastLine,FrameFirstLine,FrameLastLine,FrameNextLine;
    UINT PassFlags;
    int  ScrTableRowHeight,ScrLastY,PageFrameHt,PageWdth,
         FirstPageScrHeight;
    int  InHdrFtr;
    int  pass,PassFirstY,PassFirstFrame;
    BOOL IsParaFrame,UseSoftColBreak,HasPictFrames;
    int  ParaFrameBreak,PrevParaFrame;
    int  FrameLastY=0,ScrFrameLastY=0;
    int  TopRightMargin,TopLeftMargPage1,TopLeftMargPage2;
    int  LeftFrame=0,RightFrame,CellWidth;
    int  ColumnNo,TableRowIndent=0,CellFramed,CurLineHt,SubTableHt;
    struct StrHdrFtr hdr,ftr;
    BOOL InPartTopRow,InPartBotRow;
    BOOL   NewParaFrame,WmFrameCreated;
    int    BoxFrame=0,PageTopX,PageTopY,InitialFirstLine;
    struct StrFrameSet prt,scr,SavePrt,SaveScr;     // frame set variables
    struct StrFrameSet far *pPrt=&prt;
    struct StrFrameSet far *pScr=&scr;
    int   FrameColX[20],FrameColWidth[20],FrameColFirst[20],AlignFrame;
    int   PrevPageFrameCount=0;
    BOOL debug=FALSE;

    dm("CreateFrames");

    TotalFrames=0;
    ContainsParaFrames=FALSE;
    PageHasControls=FALSE;      // TRUE when a page includes controsl
    HasOverlayingFrames=FALSE;  // TRUE when the page has overlyaing frames
    FirstPageHeight=FirstPageScrHeight=0;
    if (PageNo<0) PageNo=0;
    FirstFramePage=PageNo;          // record the first page number in the frame set
    BorderShowing=(ShowPageBorder && !TerArg.FittedView);
    FarMemSet(&prt,0,sizeof(prt));  // initialize printer frame set variables
    FarMemSet(&scr,0,sizeof(scr));  // initialize screen frame set variables

    // initialize the row frame ranges
    for (i=0;i<TotalTableRows;i++) TableRow[i].FirstFrame=TableRow[i].LastFrame=-1;

    NEXT_PAGE:
    FrameNo=PageFirstFrame=TotalFrames;
    InitFrame(w,FrameNo);
    CurPageWidth=0;
    PHdrHeight=PFtrHeight=FtrTextHeight=0;
    HasPictFrames=FALSE;
    LastFramePage=PageNo;        // last page in the frame set
    PHdrMargin=-1;                // top margin will be set only once for the page
    NewParaFrame=FALSE;
    WmFrameCreated=false;        // true when Watermark frame is creatd
    pass=0;                      // pass within this frame
    AlignFrame=-1;               // the frame used for vertical alignment
    if (PageNo==CurPage) HdrFirstPageLine=HdrLastPageLine=FtrFirstPageLine=FtrLastPageLine=0;
    
    PrevPageFrameCount=PageInfo[PageNo].FrameCount;
    PageInfo[PageNo].FrameCount=0;
    ResetUintFlag(PageInfo[PageNo].flags,PAGE_REPAGE);   // this flag tells if the page needs to be repaginated

    // adjust the page first line if the page border creeped
    FirstLine=PageInfo[PageNo].FirstLine; // first line of the page
    if (FirstLine>=TotalLines && PageNo>0) {
       if (PageNo>0) {
         TotalPages=PageNo;
         goto FRAMES_CREATED;
       }
       else FirstLine=TotalLines-1;
    }
    
    if (cid(FirstLine)) {
      while (FirstLine>=0 && LinePage(FirstLine)==PageNo && cid(FirstLine)) FirstLine--;
      FirstLine++;
      PageInfo[PageNo].FirstLine=FirstLine;
    }

    // get the top Psection number
    line=PageInfo[PageNo].FirstLine; // first line of the page
    PTopSect=GetSection(w,line);      // find the Psection to take the header lines form

    PTopLeftMargin=(int)(PrtResX*TerSect[PTopSect].LeftMargin);
    if (PageNo==FirstFramePage) TopLeftMargPage1=PTopLeftMargin;
    else                        TopLeftMargPage2=PTopLeftMargin;
    TopRightMargin=(int)(PrtResX*TerSect[PTopSect].RightMargin);

    // create the top page border frame
    PageTopX=0;
    PageTopY=Py;
    LeftBorderWidth=TopBorderHeight=BotBorderHeight=0;
    if (BorderShowing) CreatePageBorderFrames(w,PageNo,&(Py),&(Sy),&FrameNo,&LeftFrame,&RightFrame);
    PageTopX+=LeftBorderWidth;
    PageTopY+=TopBorderHeight;

    // calculate top and bottom margins for the Psection
    PHiddenY=TerSect1[PTopSect].HiddenY;

    if (PHdrMargin==-1) { // top/bottom margin and header height are calculated only once on the top of the page
       Psect=PageHdrSect(w,PageNo,&hdr);
       if (Psect<0) Psect=PTopSect;
       PHdrHeight=PageHdrHeight2(w,PageNo,FALSE,true);
       

       if (PHdrHeight>0) PHdrMargin=(int)(PrtResY*TerSect[Psect].HdrMargin);   // top margin including the hidden space
       else              PHdrMargin=(int)(PrtResY*TerSect[Psect].TopMargin);   // no header text, so use the top margin instead
       
       Psect=PageFtrSect(w,PageNo,&ftr);
       if (Psect<0) Psect=PTopSect;
       PFtrHeight=PageFtrHeight(w,PageNo,FALSE);
       
       if (PFtrHeight>0) PFtrMargin=(int)(PrtResY*TerSect[Psect].FtrMargin);   // bottom margin including the header space
       else              PFtrMargin=(int)(PrtResY*TerSect[Psect].BotMargin);   // no footer text, so use the bottom margin instead

       FtrTextHeight=PageFtrTextHeight(w,PageNo);

       // calculate the height/width of the current page frame structure
       Psect=PTopSect;
       if (BorderShowing) {
          PageFrameHt=(int)(TerSect1[Psect].PgHeight*PrtResY)+TopBorderHeight+BotBorderHeight;
          PageWdth=(int)(TerSect1[Psect].PgWidth*PrtResX)+2*LeftBorderWidth;
       }
       else {
          PageFrameHt=(int)(TerSect1[Psect].PgHeight*PrtResY);
          if (ViewPageHdrFtr)  PageFrameHt-=2*PHiddenY;              // do not show the hidden area
          else                 PageFrameHt-=(PHdrMargin+PFtrMargin+PHdrHeight+PFtrHeight);  // margin and header not to show
          PageWdth=(int)(TerSect1[Psect].PgWidth*PrtResX)-PTopLeftMargin-TopRightMargin;
       }
    }


    // create the page header, footer or the main body of the page
    NEXT_PASS:
    pass++;                            // increment the pass number
    PassFlags=0;
    PassFirstY=Py;                     // first y of the pass
    PassFirstFrame=FrameNo;            // first frame of the pass
    
    FirstLine=PageInfo[PageNo].FirstLine; // first line of the page
    if (FirstLine>=TotalLines) FirstLine=TotalLines-1;

    FrameNo=TotalFrames;
    InitFrame(w,FrameNo);
    BlankHdrFrameHt=0;                 // height of the blank header frame

    if (pass==PASS_HDR) {                     // check if the header needs to be created
       InHdrFtr=PAGE_HDR;

       if (ViewPageHdrFtr) {
          Psect=PageHdrSect(w,PageNo,&hdr);

          if (Psect<0) {
             Psect=PTopSect;
             goto LAST_EMPTY_FRAME; // NEXT_PASS;     // no header
          }

          FirstLine=hdr.FirstLine+1; // skip the break line
          LastLine=hdr.LastLine-1;    // skip the break line
          
          if (!EditPageHdrFtr) PassFlags=FRAME_DISABLED;        // header frames are disabled

          if (PageNo==CurPage) {         // record the header beginning/ending lines
             HdrFirstPageLine=FirstLine;
             HdrLastPageLine=LastLine;
          }
          
          if (LastLine<FirstLine) goto NEXT_PASS;

          // get the current Psection again
          line=PageInfo[PageNo].FirstLine; // first line of the page
          Psect=GetSection(w,line);      // find the Psection to take the header lines form
          
          // create a blank header frame
          if (!BorderShowing && (BlankHdrFrameHt=BlankHdrFrameHeight(w,FirstLine,LastLine,Psect))>0) {
              frame[FrameNo].empty=TRUE;
              frame[FrameNo].y=Py;
              frame[FrameNo].ScrY=Sy;
              frame[FrameNo].x=0;
              frame[FrameNo].width=(int)(PrtResX*(TerSect1[Psect].PgWidth-TerSect[Psect].LeftMargin-TerSect[Psect].RightMargin));
              frame[FrameNo].flags=FRAME_RIGHTMOST; // last frame

              frame[FrameNo].height=BlankHdrFrameHt;
              frame[FrameNo].ScrHeight=PrtToScrY(BlankHdrFrameHt);
              Py+=frame[FrameNo].height;
              Sy+=frame[FrameNo].ScrHeight;

              TotalFrames++;
              FrameNo++;
              InitFrame(w,FrameNo);
          }
       }
       else goto NEXT_PASS;
    }
    else if (pass==PASS_TBL_HDR) {                 // check if the table header needs to be printed
       long TempLine;
       if (PageInfo[PageNo].TblHdrHt==0) goto NEXT_PASS;

       FirstLine=PageInfo[PageNo].TblHdrFirstLine;
       LastLine=PageInfo[PageNo].TblHdrLastLine;
       if (FirstLine>=TotalLines || LastLine>=TotalLines) goto NEXT_PASS;  // these lines possibly deleted

       InHdrFtr=0;
       PassFlags=FRAME_DISABLED|FRAME_GRAY;        // header frames are disabled
       Psect=GetSection(w,FirstLine);   // find the Psection to take the header lines form


       for (TempLine=FirstLine;TempLine<=LastLine;TempLine++) {
          int row=cell[cid(TempLine)].row;
          if (cid(TempLine)==0) goto NEXT_PASS;
          if (!(TableRow[row].flags&ROWFLAG_HDR)) goto NEXT_PASS;
       }
       if (LineInfo(w,FirstLine,INFO_ROW)) goto NEXT_PASS;
       if (!LineInfo(w,LastLine,INFO_ROW)) goto NEXT_PASS;

    }
    else if (pass==PASS_FTR) {                 // check if the footer needs to be created
       InHdrFtr=PAGE_FTR;

       if (ViewPageHdrFtr) {
          SaveSect=Psect;                // save the last Psection of the page
          Psect=PageFtrSect(w,PageNo,&ftr);  // find the Psection of the first line
          if (Psect<0) {
             Psect=PTopSect;
             goto LAST_EMPTY_FRAME;  // FRAMES_CREATED;         // no header
          }
          
          FirstLine=ftr.FirstLine+1; // skip the break line
          LastLine=(ftr.LimitFtrLine?ftr.LimitFtrLine:(ftr.LastLine-1));   // skip the break line
          if (PageNo==CurPage) {         // record the footer beginning/ending lines
             FtrFirstPageLine=FirstLine;
             FtrLastPageLine=LastLine;
          }
          if (LastLine<FirstLine) goto FRAMES_CREATED;

          // draw the height above the footer text up to the bottom margin
          if (ViewPageHdrFtr && ftr.height>ftr.TextHeight) {
              frame[FrameNo].empty=TRUE;
              frame[FrameNo].y=Py;
              frame[FrameNo].ScrY=Sy;
              frame[FrameNo].x=LeftBorderWidth;
              frame[FrameNo].width=(int)(PrtResX*(TerSect1[Psect].PgWidth));
              if (!BorderShowing) {
                 frame[FrameNo].width-=(int)(PrtResX*(TerSect[Psect].LeftMargin+TerSect[Psect].RightMargin));
                 frame[FrameNo].flags=FRAME_RIGHTMOST; // last frame
              }

              frame[FrameNo].height=ftr.height-ftr.TextHeight;
              frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);
              Py+=frame[FrameNo].height;
              Sy+=frame[FrameNo].ScrHeight;

              TotalFrames++;
              FrameNo++;
              InitFrame(w,FrameNo);
          }

          // get the current Psection again
          Psect=SaveSect;                          // use the last Psection of the page

       }
       else goto FRAMES_CREATED;
    }
    else {                             // do the body of the page
       int LastRow,LastCell;
       BOOL IsPartRow;

       InHdrFtr=0;
       Psect=GetSection(w,FirstLine);   // find the Psection to take the header lines form

       if (PageNo==(TotalPages-1)) LastLine=TotalLines-1;
       else {
          LastLine=PageInfo[PageNo].LastLine;

          if (LastLine>=TotalLines) {  // pagination not correct, force repagination
             LastLine=TotalLines-1;
             if (RepageBeginLine>(LastLine-1)) RepageBeginLine=LastLine-1;
             if (RepageBeginLine<0) RepageBeginLine=0;
          }

          // fix if last line belongs to table but does not end in a row break
          LastCell=cid(LastLine);
          LastRow=(LastCell>0)?cell[LastCell].row:0;
          IsPartRow=(TableRow[LastRow].flags&ROWFLAG_SPLIT);

          if (!repaginating && LastRow>0 && !IsPartRow && !LineInfo(w,LastLine,INFO_ROW)) {
             long TempLine;
             for (TempLine=LastLine;TempLine>FirstLine;TempLine--) {  //
                if (cid(TempLine)==0 || (tabw(TempLine) && tabw(TempLine)->type&INFO_ROW)) break;
             }
             if (TempLine==FirstLine) {
                for (TempLine=LastLine;(TempLine+1)<TotalLines;TempLine++) {  //
                   if (cid(TempLine)==0 || (tabw(TempLine) && tabw(TempLine)->type&INFO_ROW)) break;
                }
             }
             LastLine=TempLine;
             PageInfo[PageNo].LastLine=LastLine;         // fix the page table
             LastCell=cid(LastLine);
             LastRow=LevelRow(w,0,cell[LastCell].row);
             if ((LastLine+1)>=TotalLines || !(TableRow[LastRow].flags&ROWFLAG_SPLIT)) PageInfo[PageNo+1].FirstLine=LastLine+1;    // fix the page table
             ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);
          }
       }
       if (LastLine<FirstLine)   LastLine=FirstLine;
       if (LastLine>=TotalLines) LastLine=TotalLines-1;
    }

    if (LastLine>=TotalLines) LastLine=TotalLines-1;
    if (FirstLine>LastLine) FirstLine=LastLine;


    // set the vertical alignment frame
    if ((pass==PASS_BODY || pass==PASS_TBL_HDR) && AlignFrame==-1 
           && TerSect[PTopSect].flags&(SECT_VALIGN_CTR|SECT_VALIGN_BOT)) {
       frame[FrameNo].empty=TRUE;
       frame[FrameNo].y=Py;
       frame[FrameNo].ScrY=Sy;
       frame[FrameNo].x=0;
       frame[FrameNo].width=(int)(PrtResX*(TerSect1[Psect].PgWidth-TerSect[Psect].LeftMargin-TerSect[Psect].RightMargin));
       frame[FrameNo].flags=FRAME_RIGHTMOST; // last frame

       frame[FrameNo].height=0;      // height will be set after the body of the page is processed
       frame[FrameNo].ScrHeight=0;
       Py+=frame[FrameNo].height;
       Sy+=frame[FrameNo].ScrHeight;

       AlignFrame=FrameNo;
       TotalFrames++;
       FrameNo++;
       InitFrame(w,FrameNo);
    } 
       


    
    InitialFirstLine=FirstLine;      // FirstLine is reset to BreakLine after process the lines upto first break line

    CREATE_TEXT_FRAME:

    // Create watermark frame
    if (WmParaFID>0 && !WmFrameCreated) {
       CreateWatermarkFrame(w,PageNo,Py,&FrameNo,PTopSect,PTopLeftMargin,PHiddenY,PHdrMargin,PHdrHeight);
       WmFrameCreated=true;
    }

    // Look for Psection break
    for (line=FirstLine;line<LastLine;line++) {
       if (tabw(line) && tabw(line)->type&INFO_SECT) break;
    }
    

    BreakLine=line;
    if (BreakLine>=TotalLines) BreakLine=TotalLines-1;
    if (BreakLine<0) BreakLine=0;

    // skip page header/footer lines from the beginning of the page
    if (!InHdrFtr) {
       if (!ViewPageHdrFtr && PageNo==CurPage) HdrFirstPageLine=FirstLine;  // keep approx track of header/footer lines
       while(PfmtId[pfmt(FirstLine)].flags&(PAGE_HDR|PAGE_FTR)) {
          FirstLine++;
          if (FirstLine>BreakLine) break;
       }
       if (!ViewPageHdrFtr && PageNo==CurPage) HdrLastPageLine=FirstLine-1;  // keep approx track of header/footer lines

       if (FirstLine>BreakLine) goto LAST_EMPTY_FRAME;

       Psect=GetSection(w,FirstLine);

    }
    

    // calculate current Psection margin and text width
    LeftMargin=(int)(PrtResX*TerSect[Psect].LeftMargin);
    RightMargin=(int)(PrtResX*TerSect[Psect].RightMargin);
    TextWidth=(int)(PrtResX*TerSect1[Psect].PgWidth)-LeftMargin-RightMargin;
    if (TextWidth>CurPageWidth) CurPageWidth=TextWidth;   // max page width
    if (TerArg.FittedView) TextWidth=ScrToPrtX(TerWinWidth);
    MaxColumns=TerSect[Psect].columns;
    if (InHdrFtr) MaxColumns=1;                              // allow one column on page header/footer
    PColumnSpace=(int)(PrtResX*TerSect[Psect].ColumnSpace);
    if (MaxColumns==1) PColumnSpace=0;
    PColumnWidth=(TextWidth-(MaxColumns-1)*PColumnSpace)/MaxColumns;


    // calculate the text height of all lines
    if      (InHdrFtr&PAGE_HDR) ColumnHeight=PageHdrHeight2(w,PageNo,FALSE,true)+TwipsToPrtY(40);  // 40 twips for the dotted lines
    else if (InHdrFtr&PAGE_FTR) ColumnHeight=PageFtrHeight(w,PageNo,FALSE)+TwipsToPrtY(40);  // extra allowance
    else {
       if (TerSect[Psect].LastPage==PageNo && TerSect1[Psect].LastPageHeight>0)
             ColumnHeight=TerSect1[Psect].LastPageHeight;
       else  ColumnHeight=PageFrameHt-(Py-FirstPageHeight);   // go to the end of the page
    }

    // divide text into columns
    FirstFrame=FrameNo;
    if (!InitFrame(w,FrameNo))  AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 1)",1);

    PFrameHt=0;              // adds up the line height
    SFrameHt=0;           // adds up the line height
    PCurColHeight=0;         // current column height
    SCurColHeight=0;      // current column height
    PMaxColHeight=0;
    SMaxColHeight=0;
    PColumnX=PCellX=0;        // x position of the previous column or cell
    if (BorderShowing) PColumnX=PCellX=(LeftBorderWidth+PTopLeftMargin);

    FrameColX[0]=PColumnX;
    FrameColFirst[0]=FrameNo;
    FrameColWidth[0]=PColumnWidth;

    ColumnsCreated=ColumnNo=0;
    PrevFrameLastLine=FirstLine-1;
    FirstCellFrame=-1;
    TableRowHeight=0;       // height of a table row
    ScrTableRowHeight=0;    // height of a table row
    UseSoftColBreak=TRUE;   // make of of soft column breaks
    CurCell=0;
    FrameFirstLine=-1;
    CellFramed=0;           // last cell framed
    InPartTopRow=FALSE;
    InPartBotRow=FALSE;
    SubTableHt=0;           // subtable height before the line

    PrevParaFrame=CurParaFrame=0;

    for (l=FirstLine;;l++) {
       if (l<TotalLines /*&& l<BreakLine */ && pass==PASS_BODY && LinePage(l)!=PageNo && cid(l)) continue;  // this line does not belong to this page

       CurLineHt=0;
       if (l<TotalLines) CurLineHt=LineHt(l);
       if (l<TotalLines) LineFrame(l)=-1;   // reset the line frame number

       // set the frame line variables
       if (FrameFirstLine<0) FrameFirstLine=FrameNextLine=l;
       FrameLastLine=FrameNextLine;
       FrameNextLine=l;

       // reset frame types and break types
       ColBreak=RowBreak=CellBreak=CellFrame=ColumnFrame=FALSE;
       IsParaFrame=ParaFrameBreak=FALSE;

       if (!InHdrFtr && l<TotalLines && PfmtId[pfmt(l)].flags&(PAGE_HDR|PAGE_FTR)) BreakLine=l-1;  // break now

       // get current and previous cell ids
       PrevCell=CurCell;
       if (l<TotalLines && cid(l)) CurCell=LevelCell(w,0,l);
       else                        CurCell=0;
       CurRowId=cell[CurCell].row;
       PrevRowId=cell[PrevCell].row;

       InPartTopRow=(PrevRowId>0 && IsPartRow(w,TRUE,PrevRowId,PageNo));
       InPartBotRow=(PrevRowId>0 && IsPartRow(w,FALSE,PrevRowId,PageNo));

       // get current and previous para frame ids
       PrevParaFrame=CurParaFrame;
       if (l<TotalLines) CurParaFrame=fid(l);
       else              CurParaFrame=0;
       //if (l>FirstLine)  PrevParaFrame=fid(l-1);
       //else              PrevParaFrame=0;


       // determine kind of frame
       if   (PrevCell>0)         CellFrame=TRUE;
       else if (PrevParaFrame>0) IsParaFrame=TRUE;
       else                      ColumnFrame=TRUE;

       if (CurCell!=PrevCell && l>FirstLine)                   CellBreak=TRUE;
       if (CurRowId!=PrevRowId && PrevRowId!=0 && l>FirstLine) RowBreak=TRUE;
       if (CurParaFrame!=PrevParaFrame && l>FirstLine)         ParaFrameBreak=TRUE;

       if (l>FirstLine && tabw(l-1) && tabw(l-1)->type&INFO_COL
          && (ColumnsCreated+1)<MaxColumns) ColBreak=TRUE;  // hard column break
       if (l>BreakLine) {
          if (InPartTopRow || InPartBotRow) RowBreak=TRUE;
          ColBreak=TRUE;
          UseSoftColBreak=FALSE;   // do not rely on soft column breaks any more
       }
       if (l<TotalLines && LineFlags(l)&LFLAG_SOFT_COL && UseSoftColBreak) ColBreak=TRUE;  // soft column break

       if (CellFrame && !RowBreak)  ColBreak=FALSE;
       if (CurParaFrame>0) ColBreak=FALSE;

       if (ColBreak) ColumnsCreated++;


       if (ColBreak || CellBreak || RowBreak || ParaFrameBreak) {// start next frame

          // create the new frame
          if (CellFrame) {
             // creaet the cell frames for the current cell
             CreateCellFrame(w,0,&FrameNo,Py,Sy, &(PFrameHt),&(SFrameHt), 
                             PCurColHeight,SCurColHeight, &FirstCellFrame, 
                             &TableRowHeight, &ScrTableRowHeight,&(PCellX),TableRowIndent,
                             PageNo,PTopLeftMargin,ColumnNo,Psect,PrevCell, &CellFramed,
                             FrameFirstLine, FrameLastLine, PassFlags, RowBreak,&CellWidth,TextWidth,
                             MaxColumns,PColumnSpace,PageWdth,PColumnX,PColumnWidth,BoxFrame,PrevParaFrame);
             
             if (PrevParaFrame!=CurParaFrame && PrevParaFrame>0)    // check the height of the container (box) frame
                 SetBoxFrameHt(w,BoxFrame,PrevParaFrame,PCurColHeight+PFrameHt,FrameLastLine);
            
          }
          else if (IsParaFrame) {            // generate a paragraph frame
             BOOL LastFrameOfPF=(CurParaFrame!=PrevParaFrame);  // last frame for the para frame
             int i,TextAngle=ParaFrame[PrevParaFrame].TextAngle;
             struct StrFrame far *pFrameBox=&(frame[BoxFrame]);

             if (!(pFrameBox->flags&FRAME_NON_TEXT_DO)) {  // non text frame already done as part of the box frame  
                FarMove(pFrameBox,&(frame[FrameNo]),sizeof(struct StrFrame));

                // create a paragraph text frame
                frame[FrameNo].empty=FALSE;
                frame[FrameNo].PageFirstLine=ParaFrame[PrevParaFrame].TextLine=FrameFirstLine;
                frame[FrameNo].PageLastLine=FrameLastLine;
                frame[FrameNo].y=Py+PCurColHeight;  // relative to top margin
                frame[FrameNo].ScrY=Sy+SCurColHeight;  // relative to top margin
                frame[FrameNo].x=PColumnX;
                frame[FrameNo].SpaceTop=0;    // an interrupted frame
                frame[FrameNo].SpaceBot=0;  // not the last frame of a para-frame
                frame[FrameNo].height=PFrameHt;

                if (TextAngle>0) frame[FrameNo].width=pFrameBox->height;   // vertical frames are rotate at display time - not creation time

                frame[FrameNo].width-=(frame[FrameNo].SpaceLeft+frame[FrameNo].SpaceRight);
                //                          +TwipsToPrtX(frame[FrameNo].BorderWidth[BORDER_INDEX_LEFT]+frame[FrameNo].BorderWidth[BORDER_INDEX_RIGHT]));
                frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);
                frame[FrameNo].ScrWidth=PrtToScrX(frame[FrameNo].width);
                for (i=0;i<4;i++) frame[FrameNo].BorderWidth[i]=0;
                frame[FrameNo].border=0;
                frame[FrameNo].SpaceLeft=frame[FrameNo].SpaceRight=0;
                ResetUintFlag(frame[FrameNo].flags1,FRAME1_PARA_FRAME_BOX);

                if (LastFrameOfPF) SetBoxFrameHt(w,BoxFrame,PrevParaFrame,PCurColHeight+PFrameHt,FrameLastLine);

                // record the last y used by any frame
                FrameLastY=frame[FrameNo].y+frame[FrameNo].height;
                ScrFrameLastY=frame[FrameNo].ScrY+frame[FrameNo].ScrHeight;

                FrameNo++;                      // advance to the next frame
                if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 4)",1);
             }
          }
          else {
               // create a regular text frame
               frame[FrameNo].empty=FALSE;
               frame[FrameNo].sect=Psect;     // Psection to which the frame belongs
               frame[FrameNo].PageFirstLine=FrameFirstLine;
               frame[FrameNo].PageLastLine=FrameLastLine;
               frame[FrameNo].y=Py+PCurColHeight;  // relative to top margin
               frame[FrameNo].ScrY=Sy+SCurColHeight;  // relative to top margin

               frame[FrameNo].x=PColumnX;

               frame[FrameNo].width=PColumnWidth;
               if (MaxColumns==1 || (ColumnNo+1)==MaxColumns) { // last column
                  if (BorderShowing) {
                     frame[FrameNo].width=PageWdth-LeftBorderWidth-frame[FrameNo].x;
                     frame[FrameNo].SpaceRight=TopRightMargin;
                  }
                  else frame[FrameNo].flags|=FRAME_RIGHTMOST;
                  if ((ColumnNo+1)==MaxColumns) frame[FrameNo].flags|=FRAME_LAST_COL;
               }
               else frame[FrameNo].width+=PColumnSpace;      // add the inter-column space

               frame[FrameNo].height=PFrameHt;
               frame[FrameNo].ScrHeight=SFrameHt;
               frame[FrameNo].TextHeight=SFrameHt;
               
               //frame[FrameNo].border=BORDER_ALL;         // uncomment for debugging visual aid

               if (ColBreak) frame[FrameNo].LastColumnFrame=TRUE;

               FrameNo++;                      // advance to the next frame
               if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 4)",1);
          }

          NewParaFrame=FALSE;
          FrameFirstLine=FrameNextLine;      // for the next frame

          // reset frame height for cells and para frames
          if (CellFrame && !RowBreak) PFrameHt=SFrameHt=0;
          //if (IsParaFrame) PFrameHt=SFrameHt=0;

          // check for premature page overflow
          if (!InHdrFtr && l<TotalLines
              && (PCurColHeight+PFrameHt+CurLineHt)>ColumnHeight
              && (PCurColHeight>0 || PFrameHt>0)// atleast some text in the page
              && BreakLine==LastLine && l<=BreakLine
              && ColumnsCreated>=MaxColumns  // end of the page
              && !repaginating               // do not disturb the repagination process
              && !(PageInfo[PageNo+1].flags&PAGE_HARD)) {   // do not disturb the hard pagebreaks

             PageInfo[PageNo].LastLine=l-1;          // adjust the beginning of the next page
             PageInfo[PageNo+1].FirstLine=l;          // adjust the beginning of the next page
             ResetUintFlag(PageInfo[PageNo+1].flags,PAGE_HARD);
             BreakLine=LastLine=l-1;         // end this page here
          }

          // adjust operational variables
          PCurColHeight+=PFrameHt;
          SCurColHeight+=SFrameHt;
          if (PCurColHeight>PMaxColHeight) PMaxColHeight=PCurColHeight;
          if (SCurColHeight>SMaxColHeight) SMaxColHeight=SCurColHeight;
          if (ColBreak) PCurColHeight=SCurColHeight=0;
          PFrameHt=SFrameHt=0;            // reset the height for the frame

          // set x position for the next frame
          if (ColBreak) {
             PColumnX=PCellX=PColumnX+PColumnWidth+PColumnSpace;
          }
          if (CellFrame)  PCellX=PCellX+CellWidth;
          if (CurCell==0) PCellX=PColumnX;
          if (RowBreak) PCellX=PColumnX; // x for the first cell of the next row

          // on column break, supply a column frame if necessary
          if (ColBreak && !ColumnFrame) {
             int PrevFrame;
             for (PrevFrame=FrameNo-1;PrevFrame>=FirstFrame;PrevFrame--) {
               if (frame[PrevFrame].ParaFrameId==0 && frame[PrevFrame].CellId==0) break;
             }
             if (PrevFrame>=FirstFrame) {
                frame[FrameNo].empty=TRUE;
                frame[FrameNo].LastColumnFrame=TRUE;
                frame[FrameNo].y=frame[PrevFrame].y+frame[PrevFrame].height;
                frame[FrameNo].ScrY=frame[PrevFrame].ScrY+frame[PrevFrame].ScrHeight;
                frame[FrameNo].x=frame[PrevFrame].x;
                frame[FrameNo].width=frame[PrevFrame].width;

                frame[FrameNo].height=0;
                frame[FrameNo].ScrHeight=0;
                frame[FrameNo].sect=frame[FrameNo-1].sect;
                if (!BorderShowing && (MaxColumns==1 || ColumnsCreated==MaxColumns)) frame[FrameNo].flags|=FRAME_RIGHTMOST;

                FrameNo++;                      // advance to the next frame
                if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 5)",1);
             }
          }
          ColumnNo=ColumnsCreated;     // number of columns created

          if (ColBreak) {
             FrameColFirst[ColumnNo]=FrameNo;   // first column of the next column
             FrameColX[ColumnNo]=PColumnX;   // first column of the next column
             FrameColWidth[ColumnNo]=PColumnWidth;   // first column of the next column
          }


          if (l>BreakLine) {                // going past the last line on the page
             if (!ColBreak) {
                ColumnsCreated++;
                if (!BorderShowing) frame[FrameNo-1].flags|=FRAME_RIGHTMOST;
             }
             if (PrevParaFrame>0) {    // switch out of any paragraph frame
                SwitchParaFrames(w,PrevParaFrame,&prt,&scr,&SavePrt,&SaveScr,&FrameNo,l,PassFlags,PageNo);
             }
             break;
          }

       }

       // check if new frame is beginning
       if (CurParaFrame!=PrevParaFrame) {
          SwitchParaFrames(w,PrevParaFrame,&prt,&scr,&SavePrt,&SaveScr,&FrameNo,l,PassFlags,PageNo);
          NewParaFrame=CurParaFrame>0?TRUE:FALSE;          // start a new paraframe
          BoxFrame=(NewParaFrame?(FrameNo-1):0);
       } 

       // check if any sub table frames need to be created.
       if ((l<TotalLines && TableLevel(w,l)>0)) {
          int BeginX;        // where then new subtable begins
          int cl,ParentCell,ParentRow;
          ParentCell=LevelCell(w,0,l);
          ParentRow=cell[ParentCell].row;
          BeginX=PColumnX+TwipsToPrtX(TableRow[ParentRow].indent);
          for (cl=TableRow[ParentRow].FirstCell;cl>0 && cl!=ParentCell;cl=cell[cl].NextCell) BeginX+=TwipsToPrtX(cell[cl].width);

          l=CreateSubTableFrames(w,l,0,&FrameNo,&SubTableHt,BeginX/*PCellX*/,PageNo,Psect,Py+PCurColHeight+PFrameHt,
                                Sy+SCurColHeight+SFrameHt,PHiddenY,PHdrMargin,PHdrHeight,PTopSect,
                                PTopLeftMargin,ColumnNo,&HasPictFrames,pass,BoxFrame,fid(l));
          PFrameHt+=SubTableHt;
          SFrameHt+=PrtToScrY(SubTableHt);
          continue;
       }

       // record line y and x position, set PageHasControl flag
       if (l<TotalLines) SetFrameLineInfo(w,l,Py,Sy,&PFrameHt,&SFrameHt,PCurColHeight,
                            PCellX,&TableRowIndent,PageNo,PHiddenY,PHdrMargin,PHdrHeight,
                            PTopSect,PTopLeftMargin,ColumnNo,&HasPictFrames,Psect,CurLineHt);

    }

    // equate the height of the last column frames
    TotalFrames=FrameNo;
    LastY=ScrLastY=0;
    for (i=FirstFrame;i<TotalFrames;i++) {   // get the last Y position
       if (frame[i].LastColumnFrame && (frame[i].y+frame[i].height)>LastY) LastY=frame[i].y+frame[i].height;
       if (frame[i].LastColumnFrame && (frame[i].ScrY+frame[i].ScrHeight)>ScrLastY) ScrLastY=frame[i].ScrY+frame[i].ScrHeight;
    }
    if (pass==PASS_BODY && ColumnsCreated>=2) {  // ensure that the last Y does go past the page
       if (BorderShowing) TextHeight=(int)(PrtResY*TerSect1[PTopSect].PgHeight)+TopBorderHeight-PFtrMargin-PFtrHeight;
       else {
           if (ViewPageHdrFtr) TextHeight=PageFrameHt-(PFtrMargin/*+PFtrHeight*/);
           else                TextHeight=PageFrameHt;
       }
       if (LastY>(FirstPageHeight+TextHeight)) LastY=FirstPageHeight+TextHeight;
    }
    for (i=FirstFrame;i<TotalFrames;i++) {
       if (frame[i].LastColumnFrame) frame[i].height=LastY-frame[i].y; // set all frames to equal height
       if (frame[i].LastColumnFrame) frame[i].ScrHeight=ScrLastY-frame[i].ScrY; // set all frames to equal height
    }

    // Supply the last empty column if necessary for multi column frame
    if (ColumnsCreated<MaxColumns) {
       frame[FrameNo].empty=TRUE;
       frame[FrameNo].y=Py;           // relative to top margin
       frame[FrameNo].ScrY=Sy;     // relative to top margin
       frame[FrameNo].x=PColumnX;
       frame[FrameNo].width=PageWdth-LeftBorderWidth-PColumnX;
       frame[FrameNo].height=PMaxColHeight;
       frame[FrameNo].ScrHeight=SMaxColHeight;
       frame[FrameNo].sect=Psect;               //Psection to which the frame belongs
       frame[FrameNo].LastColumnFrame=TRUE;
       if (!BorderShowing) frame[i].flags|=FRAME_RIGHTMOST;

       PColumnX+=frame[FrameNo].width;

       FrameColWidth[ColumnsCreated]=PColumnWidth*(MaxColumns-ColumnsCreated)+PColumnSpace*(MaxColumns-ColumnsCreated-1);  // update these variables for the MapRtlCol to work properly
       MaxColumns=ColumnsCreated+1;

       FrameNo++;
       if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 5)",1);
       TotalFrames=FrameNo;
    }

    // rtl adjustment for multicolumn section
    if (MaxColumns>1 && IsParaRtl(w,FLOW_DEF,FLOW_DEF,TerSect[Psect].flow,DocTextFlow)) 
       MapRtlCol(w,FrameColFirst,FrameColX,FrameColWidth,PColumnSpace,FrameNo,MaxColumns);


    Py+=PMaxColHeight;                   // increment the y position on the page
    Sy+=SMaxColHeight;             // increment the y position on the page
    
    // More lines to process?
    if (BreakLine<LastLine) {             // more lines to process
        FirstLine=BreakLine+1;
        goto CREATE_TEXT_FRAME;
    }

    LAST_EMPTY_FRAME:                    // Create the last empty frame
    Psect=PTopSect;

    if (HasPictFrames) CreatePictFrames(w,PageNo,PHiddenY,PHdrMargin,PTopLeftMargin,PHdrHeight,PTopSect,InitialFirstLine,LastLine,InHdrFtr); // position the picture frames

    // get the body text height
    if (pass==PASS_BODY) {
      int LastY=PassFirstY,CurY,frm;
      for (i=PassFirstFrame;i<TotalFrames;i++) {
         if (frame[i].empty || frame[i].height==0) continue;
         if (frame[i].ParaFrameId!=0) {
            int ParaFID=frame[i].ParaFrameId;
            if (ParaFrame[ParaFID].pict==0) frm=frame[i].BoxFrame;
            else                            frm=i;
         }
         else frm=i;

         CurY=frame[frm].y+frame[frm].height;
         if (CurY>LastY) LastY=CurY;
      } 
      PageInfo[PageNo].BodyTextHt=LastY-PassFirstY;
    } 

    if (pass==PASS_TBL_HDR) goto NEXT_PASS;


    if (!InitFrame(w,TotalFrames)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 7)",1);

    frame[TotalFrames].empty=TRUE;
    frame[TotalFrames].y=Py;
    frame[TotalFrames].ScrY=Sy;
    frame[TotalFrames].width=TextWidth;
    frame[TotalFrames].x=0;
    frame[TotalFrames].sect=(TotalFrames>0)?frame[TotalFrames-1].sect:0;


    if (BorderShowing) {
       frame[TotalFrames].x=(LeftBorderWidth+PTopLeftMargin);
       frame[TotalFrames].width+=TopRightMargin;

       // calulate the height that needs to be covered
       if      (InHdrFtr&PAGE_HDR) TextHeight=TopBorderHeight+PHdrMargin+PHdrHeight;
       else if (InHdrFtr&PAGE_FTR) TextHeight=(int)(PrtResY*TerSect1[Psect].PgHeight)+TopBorderHeight-PFtrMargin;
       else {
           TextHeight=(int)(PrtResY*TerSect1[Psect].PgHeight)+TopBorderHeight-PFtrMargin-PFtrHeight;
           if (!ViewPageHdrFtr) TextHeight+=PFtrHeight;  // show the footer space since the border is showing
       }
       
       if (InHdrFtr&PAGE_HDR) frame[TotalFrames].flags|=FRAME_DIVIDER_LINE;
       if (ViewPageHdrFtr && !InHdrFtr) frame[TotalFrames].flags|=FRAME_DIVIDER_LINE;  // draw line after the body frame to indicated the beginning of the footer frame
    }
    else {
       // calulate the height that needs to be covered
       if      (InHdrFtr&PAGE_HDR) TextHeight=PHdrMargin-PHiddenY+PHdrHeight;
       else if (InHdrFtr&PAGE_FTR) TextHeight=PageFrameHt;
       else {
           if (ViewPageHdrFtr) TextHeight=PageFrameHt+PHiddenY-PFtrMargin-PFtrHeight;
           else                TextHeight=PageFrameHt;
       }
       frame[TotalFrames].flags|=(FRAME_RIGHTMOST|FRAME_DIVIDER_LINE);
       if (PageInfo[PageNo].FnoteHt>0 && !InHdrFtr) ResetUintFlag(frame[TotalFrames].flags,FRAME_DIVIDER_LINE);
    }


    if ((TerFlags2&TFLAG2_HIDE_PAGE_BREAK && !InPrinting)
        || (TerArg.FittedView && (PageNo>=(TotalPages-1) || !(PageInfo[PageNo+1].flags&PAGE_HARD)))) {
       frame[TotalFrames].height=frame[TotalFrames].ScrHeight=0;  // no page separation in Fitted view
       if (Py<FrameLastY) frame[TotalFrames].height=FrameLastY-Py;                 // reserve space for any extending frames
       if (Sy<ScrFrameLastY) frame[TotalFrames].ScrHeight=ScrFrameLastY-Sy;
    }
    else {
       frame[TotalFrames].height=TextHeight-(Py-FirstPageHeight);
       frame[TotalFrames].ScrHeight=PrtToScrY(TextHeight)-(Sy-FirstPageScrHeight);
       if (!InHdrFtr && PageInfo[PageNo].FnoteHt>0) {
          frame[TotalFrames].height-=PageInfo[PageNo].FnoteHt;
          frame[TotalFrames].ScrHeight-=PrtToScrY(PageInfo[PageNo].FnoteHt);
       }
       if (frame[TotalFrames].height<TwipsToPrtY(40)) frame[TotalFrames].height=TwipsToPrtY(40);       // leave some space for dotted lines
       if (frame[TotalFrames].ScrHeight<TwipsToScrY(40)) frame[TotalFrames].ScrHeight=TwipsToScrY(40);
    }

    Py+=frame[TotalFrames].height;
    Sy+=frame[TotalFrames].ScrHeight;

    // Set vertical alignment
    if (pass==PASS_BODY && AlignFrame>=0 && frame[TotalFrames].height>0) {
       int height=frame[TotalFrames].height;  // height of the alignment frame
       int i;

       if (TerSect[PTopSect].flags&(SECT_VALIGN_CTR)) height=height/2;

       frame[AlignFrame].height=height;
       frame[AlignFrame].ScrHeight=PrtToScrY(height);

       // move other frames download
       for (i=(AlignFrame+1);i<=TotalFrames;i++) {
          frame[i].y+=height;
          frame[i].ScrY+=PrtToScrY(height);
       }
       
       // adjust the height of the bottom empty frame
       frame[TotalFrames].height-=height; 
       frame[TotalFrames].ScrHeight-=PrtToScrY(height); 
    } 
      

    TotalFrames++;
    InitFrame(w,TotalFrames);

    // draw the footnote frame
    if (!InHdrFtr && PageInfo[PageNo].FnoteHt>0)
         CreateFnoteFrame(w,PageNo,&(Py),&(Sy),TextWidth,PTopLeftMargin,TopRightMargin);


    // create the page box frame is spcified
    ResetUintFlag(PageInfo[PageNo].flags,PAGE_BOXED);
    if (BorderShowing || InPrinting) {
       if (!ViewPageHdrFtr || InHdrFtr&PAGE_FTR) CreatePageBox(w,PageNo,PageTopX,PageTopY,PTopSect,LeftFrame);
    }

    // check if y needs to be adjusted for fixed height margins
    if (pass==PASS_HDR && TerSect[PTopSect].flags&SECT_EXACT_MARGT) {
       int diff=PageHdrHeight2(w,PageNo,TRUE,true)-PageHdrHeight2(w,PageNo,FALSE,true);
       Py-=diff;
       Sy-=PrtToScrY(diff);
       if (diff) HasOverlayingFrames=TRUE;
    } 
    if (pass==PASS_BODY && TerSect[PTopSect].flags&SECT_EXACT_MARGB) {
       int diff=PageFtrHeight(w,PageNo,TRUE)-PageFtrHeight(w,PageNo,FALSE);
       Py-=diff;
       Sy-=PrtToScrY(diff);
       if (diff) HasOverlayingFrames=TRUE;
    }
    
     
    if (pass<MAX_PASSES) goto NEXT_PASS;           // do the next part


    FRAMES_CREATED:
    
    // draw the page border frame
    if (BorderShowing) {
      if (true || !ViewPageHdrFtr || InHdrFtr&PAGE_FTR) CreatePageBorderBot(w,PageNo,&(Py),&(Sy),LeftFrame,RightFrame);
    }

    if (PageNo<LastPage || FirstFramePage==LastPage) { // record the first frame height
       FirstPage2Frame=TotalFrames;       // first frame of the second page
       if (TotalFrames>0) {
         FirstPageHeight=frame[TotalFrames-1].y+frame[TotalFrames-1].height;  // height in printer units
         FirstPageScrHeight=frame[TotalFrames-1].ScrY+frame[TotalFrames-1].ScrHeight;  // height in printer units
       }
       else FirstPageHeight=FirstPageScrHeight=0;
    }

    // record screen page height for use in FittedView since page height varry
    if (PageNo==FirstFramePage || TotalFrames==0) PageInfo[PageNo].ScrHt=FirstPageScrHeight;
    else PageInfo[PageNo].ScrHt=frame[TotalFrames-1].ScrY+frame[TotalFrames-1].ScrHeight - FirstPageScrHeight;

    // record the text height for the page
    PageInfo[PageNo].TextHt=0;
    for (i=PageFirstFrame;i<TotalFrames;i++) {
       BOOL select=(!frame[i].empty && !(frame[i].flags&FRAME_DISABLED) && (frame[i].ScrY+frame[i].TextHeight)>PageInfo[PageNo].TextHt);
       if (frame[i].flags&FRAME_FNOTE) select=TRUE;
       if (frame[i].ParaFrameId!=0) frame[i].TextHeight=frame[i].ScrHeight;
       if (select) PageInfo[PageNo].TextHt=frame[i].ScrY+frame[i].TextHeight;
    }
    if (PageFirstFrame>=FirstPage2Frame) PageInfo[PageNo].TextHt-=(PrtToScrY(FirstPageHeight));

    if (PrevPageFrameCount!=PageInfo[PageNo].FrameCount) PageInfo[PageNo].flags|=PAGE_REPAGE;
    
    PageNo++;
    if (PageNo<=LastPage && PageNo<TotalPages) goto NEXT_PAGE;   // create the next page


    // Adjust height for row spanning
    for (i=0;i<TotalFrames;i++) {
        if (BorderShowing) {                    // extend the left-most frame over the left margin
            int PgWidth=(int)(PrtResX*TerSect1[PTopSect].PgWidth);
            int CurLeftMargin=(int)(PrtResX*TerSect[frame[i].sect].LeftMargin);
            PTopLeftMargin=(i<FirstPage2Frame)?TopLeftMargPage1:TopLeftMargPage2;
            if (frame[i].ParaFrameId==0 && frame[i].level==0 && frame[i].x==(LeftBorderWidth+PTopLeftMargin)
                && (frame[i].CellId==0 || frame[i].empty)) {
               frame[i].x=LeftBorderWidth;
               
               //frame[i].SpaceLeft+=CurLeftMargin;   // this causes misalignment between paragraph frames and regular text
               //frame[i].width+=CurLeftMargin;
               frame[i].SpaceLeft+=PTopLeftMargin;
               frame[i].width+=PTopLeftMargin;
               if (frame[i].width>PgWidth) frame[i].width=PgWidth;
            }    
        }    
        if (frame[i].CellId>0 && !frame[i].empty) {
           int cl=frame[i].CellId;
           int row=cell[cl].row;
           int pg=(i>=FirstPage2Frame?FirstFramePage+1:FirstFramePage);
           BOOL IsFixedHt;

           if (cell[cl].RowSpan>1 || cell[cl].flags&CFLAG_ROW_SPANNED) {
              SpannedHeight=GetSpannedRowHeight(w,cl,NULL,pg);
              frame[i].height+=SpannedHeight;
              frame[i].ScrHeight+=PrtToScrY(SpannedHeight);
           }
           if (cell[cl].flags&CFLAG_ROW_SPANNED && row==LevelRow(w,cell[cl].level,PageInfo[pg].FirstRow)) frame[i].flags|=FRAME_TOP_SPAN;

           // adjust space before frame to vertical alignment
           if (cell[cl].flags&(CFLAG_VALIGN_CTR|CFLAG_VALIGN_BOT|CFLAG_VALIGN_BASE)) {
              long l;
              int height=0,ExtraHeight;
              BOOL HasSubTables=FALSE;
              for (l=frame[i].PageFirstLine;l<=frame[i].PageLastLine;l++) {
                 height+=LineHt(l);
                 if (TableLevel(w,l)>frame[i].level) HasSubTables=TRUE;
              }
              if (!HasSubTables) {
                 if (cell[cl].flags&CFLAG_VALIGN_BASE) frame[i].SpaceTop+=CellAux[cl].SpaceBefore;
                 else {
                    if (cell[cl].TextAngle==0) ExtraHeight=frame[i].height-height-frame[i].SpaceTop-frame[i].SpaceBot;
                    else                       ExtraHeight=frame[i].width-height-frame[i].SpaceLeft-frame[i].SpaceRight;
                    if (ExtraHeight>0) {
                       if (cell[cl].TextAngle==0) {
                          if (cell[cl].flags&CFLAG_VALIGN_CTR) frame[i].SpaceTop+=(ExtraHeight/2);
                          else                                 frame[i].SpaceTop+=ExtraHeight;
                       }
                       else if (cell[cl].TextAngle==90) {
                          if (cell[cl].flags&CFLAG_VALIGN_CTR) frame[i].SpaceLeft+=(ExtraHeight/2);
                          else                                 frame[i].SpaceLeft+=ExtraHeight;
                       }
                       else if (cell[cl].TextAngle==270) {
                          if (cell[cl].flags&CFLAG_VALIGN_CTR) frame[i].SpaceRight+=(ExtraHeight/2);
                          else                                 frame[i].SpaceRight+=ExtraHeight;
                       }
                    }
                 }
              }
           }

           // Y limiation because of fixed row table
           IsFixedHt=(TableRow[row].MinHeight<0);
           frame[i].LimitY=frame[i].y+frame[i].height;
           if (cell[cl].level!=0) frame[i].LimitY=ApplyParentCellLimitY(w,i,cl,frame[i].LimitY,&IsFixedHt);
           if (!IsFixedHt || True(TerFlags6&TFLAG6_ALLOW_CELL_OVERFLOW)) frame[i].LimitY=-1;  // no limitation
           
           // cell/frame cross reference
           CellAux[cl].FrameId=i;
        }

        // increase the width of the fitted view regular frame to contain a big picture
        if (TerArg.FittedView && frame[i].CellId==0 && !frame[i].empty && !InPrinting) {
           int pict;
           for (l=frame[i].PageFirstLine;l<=frame[i].PageLastLine;l++) {
               pict=GetCurCfmt(w,l,0);
               if (TerFlags&TFLAG_NO_WRAP || (PfmtId[pfmt(l)].pflags&PFLAG_NO_WRAP)) {
                   int width=GetLineWidth(w,l,FALSE,FALSE);
                   if (frame[i].width<width) frame[i].width=width;
               }  
               else if (pict>=0 && pict<TotalFonts && TerFont[pict].style&PICT) {
                   int width=TwipsToPrtX(TerFont[pict].PictWidth);
                   if (frame[i].width<width) frame[i].width=width;
               }
           } 
        }    
        
        // set the line frame number
        if (!frame[i].empty && False(frame[i].flags&FRAME_PICT)) {  // skip picture frames, the frame of the anchor line is returned instead
           for (l=frame[i].PageFirstLine;l<=frame[i].PageLastLine;l++) {
              if (frame[i].CellId!=cid(l)) continue;     // could be a nested cell
              LineFrame(l)=i;
           }
        } 

        if (!frame[i].empty && ViewPageHdrFtr && !(EditPageHdrFtr) && PfmtId[pfmt(frame[i].PageFirstLine)].flags&PAGE_HDR_FTR) frame[i].flags|=FRAME_DISABLED;
        if (TerFlags3&TFLAG3_SHOW_FRAMES) frame[i].border=BORDER_ALL;
        
        frame[i].OrigX=frame[i].x;                 // original x value
    }


    // Adjust x position for rtl table row frames
    for (i=0;i<TotalFrames;i++) MapRtlCell(w,i);


    // initialize the screen fields
    for (i=0;i<TotalFrames;i++) {
       frame[i].ScrFirstLine=-1;          // not set yet
       frame[i].ScrLastLine=-1;
       frame[i].RowOffset=0;
       frame[i].ScrWidth=0;
       if (!frame[i].empty) LineFlags(frame[i].PageFirstLine)|=LFLAG_FRAME_TOP;  // top line of the frame
    }

    RowX[0]=RowY[0]=RowHeight[0]=0;       // initialize the position arrays
    
    CurPageHeight=0;
    for (i=0;i<TotalFrames;i++) {
      int height=frame[i].y+frame[i].height;
      if (height>CurPageHeight) CurPageHeight=height;
    }

    SortFrames(w);         // sort the frames in the Z order

    if (!printer) CreateFramesScr(w);     // do not translate to screen units

    // reset to single frame
    if (!TerArg.PageMode) {
       TotalFrames=1;
       InitFrame(w,0);                    // initialize the first frame
       frame[0].ScrFirstLine=frame[0].ScrLastLine=BeginLine;
       frame[0].RowOffset=0;
       frame[0].PageFirstLine=0;
       frame[0].PageLastLine=TotalLines-1;
       frame[0].ScrFirstLine=frame[0].ScrLastLine=BeginLine;
    }

    dm("CreateFrames - End");

    return;
}

/******************************************************************************
    ApplyParentCellLimitY:
    Apply the parent cell limit y for the fixed height table row cell.
*******************************************************************************/
int ApplyParentCellLimitY(PTERWND w,int frm, int cl,int LimitY, BOOL far* IsFixedHt)
{
    int i,row,limit=LimitY;
    
    cl=cell[cl].ParentCell;
    if (cl<=0) return LimitY;     // end of chain
    if (cell[cl].RowSpan>1) return LimitY;

    row=cell[cl].row;
    if (TableRow[row].MinHeight<0) {   // the parent cell is in the exact height row
       int FirstFrm=0;                // locate the frame for this cell
       int LastFrm=FirstPage2Frame-1; 
       if (frm>FirstPage2Frame) {
          FirstFrm=FirstPage2Frame;
          LastFrm=TotalFrames-1;
       } 
       for (i=FirstFrm;i<=LastFrm;i++) if (frame[i].CellId==cl) break;

       if (i<=LastFrm) {
          if (frame[i].y+frame[i].height<limit) limit=frame[i].y+frame[i].height;  // new limit
          frm=i;
       }
       (*IsFixedHt)=TRUE;                           // fixed height restriction exists 
    } 

    return ApplyParentCellLimitY(w,frm,cl,limit,IsFixedHt);   // apply the parent limitation
} 

/******************************************************************************
    MapRtlCol:
    Map the column frames for an rtl section
*******************************************************************************/
MapRtlCol(PTERWND w,LPINT FrameColFirst, LPINT FrameColX, LPINT FrameColWidth, int SectColSpace, int NextFrame, int MaxColumns)
{
    int i,j,LastFrame,TotalWidth=0,RelX,RelColX,NewX,NewColX,CurColWidth;

    for (i=0;i<MaxColumns;i++) {
       if ((i+1)<MaxColumns) FrameColWidth[i]+=SectColSpace;
       TotalWidth+=FrameColWidth[i];
    }
    
    NewX=TotalWidth;   // relative x position as the columns are added from right

    for (i=0;i<MaxColumns;i++) {   // fix each colunm
       if ((i+1)<MaxColumns) LastFrame=FrameColFirst[i+1]-1;
       else                  LastFrame=NextFrame-1;
       CurColWidth=FrameColWidth[i];

       NewX=NewX-FrameColWidth[i];
       
       RelColX=FrameColX[i]-FrameColX[0];  // relative x for the colunm
       NewColX=NewX+FrameColX[0];          // new column x position

       for (j=FrameColFirst[i];j<=LastFrame;j++) {
          if (frame[j].ParaFrameId>0) continue;
          RelX=frame[j].x-FrameColX[i];   // relative X
          frame[j].x=NewColX+RelX;
          if (frame[j].CellId==0) {
             frame[j].width=CurColWidth;
             frame[j].SpaceLeft=frame[j].SpaceRight=0;
             if ((i+1)<MaxColumns) frame[j].SpaceLeft=SectColSpace;
          }
          else if (i!=0) {
             frame[j].x-=SectColSpace;
             if (BorderShowing) frame[j].x-=SectColSpace;
          }
       } 
    }  
    return TRUE;
}
  
/******************************************************************************
    MapRtlCell:
    Map the frame if it contains an RTL cell.
*******************************************************************************/
MapRtlCell(PTERWND w,int frm)
{
    BOOL IsRtl;
    int FirstFrame,LastFrame,row,RowBeginX,RowWidth,cl,RelX;

    if (frame[frm].CellId==0 || frame[frm].flags&FRAME_RTL_CHECKED) return TRUE;
    
    frame[frm].flags|=FRAME_RTL_CHECKED;
        
    cl=frame[frm].CellId;
    row=cell[cl].row;
    IsRtl=IsParaRtl(w,FLOW_DEF,TableRow[row].flow,TerSect[frame[frm].sect].flow,DocTextFlow);
    
    if (IsRtl) {
       FirstFrame=TableRow[row].FirstFrame;    // row indentation frame
       RowBeginX=frame[FirstFrame].OrigX;                   
       LastFrame=TableRow[row].LastFrame;
       RowWidth=frame[LastFrame].OrigX+frame[LastFrame].width-RowBeginX;  // empty frame after the last cell frame
       if (cell[cl].level==0 && BorderShowing) {                // add right frame margin
          int sect=frame[frm].sect;
          RowWidth+=(int)(TerSect[sect].RightMargin*PrtResX);
       } 
       
       RelX=frame[frm].OrigX-RowBeginX;           // cell frame x relative to the beginnig of the row
       RelX=RowWidth-RelX-frame[frm].width;  // apply rtl mapping

       if (cell[cl].level>0) {
          int ParentCell=cell[cl].ParentCell;
          int ParentFrame=CellAux[ParentCell].FrameId;
          if (!(frame[ParentFrame].flags&FRAME_RTL_CHECKED)) MapRtlCell(w,ParentFrame);

          frame[frm].x=frame[ParentFrame].x+frame[ParentFrame].SpaceLeft+RelX;
       } 
       else frame[frm].x=frame[FirstFrame].OrigX+RelX;        // new x position
       
       // swap left/right border
       //SwapInts(&(frame[frm].BorderWidth[BORDER_INDEX_LEFT]),&(frame[frm].BorderWidth[BORDER_INDEX_RIGHT]));
       //border=frame[frm].border;
       //ResetUintFlag(frame[frm].border,(BORDER_LEFT|BORDER_RIGHT));
       //if (border&BORDER_LEFT) frame[frm].border|=BORDER_RIGHT;
       //if (border&BORDER_RIGHT) frame[frm].border|=BORDER_LEFT;
       
       frame[frm].flags|=FRAME_RTL;
       TableRow[row].flags|=ROWFLAG_RTL;
    }
    else ResetUintFlag(TableRow[row].flags,ROWFLAG_RTL); 

    return TRUE;
} 

/******************************************************************************
    SwitchParaFrames:
    Switch para frame during frame set creation
*******************************************************************************/
SwitchParaFrames(PTERWND w,int PrevParaFrame, 
                 PFRAMESET pPrt, PFRAMESET pScr, PFRAMESET pSavePrt, PFRAMESET pSaveScr, 
                 LPINT pFrameNo, long l, UINT PassFlags, int PageNo)
{
    int FrameNo=(*pFrameNo);
    int ParaId,i;
    UINT flags;

    if (CurParaFrame>0) {           // start a new paraframe
       if (PrevParaFrame==0) {     // save the operating variables for regular text (no frame)
          FarMove(pPrt,pSavePrt,sizeof(FRAMESET));  
          FarMove(pScr,pSaveScr,sizeof(FRAMESET));  
       }
       else {                     // reset for new frame
          FarMove(pSavePrt,pPrt,sizeof(FRAMESET));  
          FarMove(pSaveScr,pScr,sizeof(FRAMESET));  
       } 

       // create a paragraph text frame
       ContainsParaFrames=TRUE;     // TRUE when a page contains paraframes

       frame[FrameNo].empty=TRUE;
       frame[FrameNo].sect=Psect;     // section to which the frame belongs
       frame[FrameNo].PageFirstLine=ParaFrame[PrevParaFrame].TextLine=l;   // this value will be modified
       frame[FrameNo].PageLastLine=l;                                      // this value will be modified
       frame[FrameNo].ParaFrameId=CurParaFrame;
       frame[FrameNo].ZOrder=ParaFrame[CurParaFrame].ZOrder;
       frame[FrameNo].flags|=PassFlags;
       frame[FrameNo].flags1|=FRAME1_PARA_FRAME_BOX;
       frame[FrameNo].BoxFrame=FrameNo;                                    // container frame

       if (ParaFrame[CurParaFrame].flags&(PARA_FRAME_LINE|PARA_FRAME_RECT)) {
          frame[FrameNo].flags|=FRAME_NON_TEXT_DO;  // the text in these frames are not drawn
          frame[FrameNo].empty=FALSE;               // this will serve as content frame also
       }

       // set the transparent flag
       if (ParaFrame[CurParaFrame].flags&PARA_FRAME_OBJECT && ParaFrame[CurParaFrame].FillPattern==0)
          frame[FrameNo].flags|=FRAME_TRANSPARENT;

       // set para border
       if (ParaFrame[CurParaFrame].flags&(PARA_FRAME_OBJECT|PARA_FRAME_CONV)) {
          if (ParaFrame[CurParaFrame].flags&PARA_FRAME_BOXED) flags=PARA_BOX;
          else                                                flags=0;
       }
       else {
           ParaId=pfmt(frame[FrameNo].PageLastLine);
           flags=PfmtId[ParaId].flags;
           frame[FrameNo].shading=(PfmtId[ParaId].shading/100);  // specified in percentage
           frame[FrameNo].BackColor=PfmtId[ParaId].BkColor;
       }

       // set the border width
       for (i=0;i<4;i++) frame[FrameNo].BorderWidth[i]=0;
       if (flags&PARA_BOX) {
           int thick=1;
           if (flags&PARA_BOX_LEFT)  frame[FrameNo].border|=BORDER_LEFT;
           if (flags&PARA_BOX_RIGHT) frame[FrameNo].border|=BORDER_RIGHT;
           if (flags&PARA_BOX_TOP)   frame[FrameNo].border|=BORDER_TOP;
           if (flags&PARA_BOX_BOT)   frame[FrameNo].border|=BORDER_BOT;

           if (ParaFrame[CurParaFrame].flags&(PARA_FRAME_TEXT_BOX|PARA_FRAME_RECT)) {
              thick=ParaFrame[CurParaFrame].LineWdth;
              if (thick<1) thick=1;
              frame[FrameNo].BorderWidth[BORDER_INDEX_TOP]=thick;
              frame[FrameNo].BorderWidth[BORDER_INDEX_BOT]=thick;
              frame[FrameNo].BorderWidth[BORDER_INDEX_LEFT]=thick;
              frame[FrameNo].BorderWidth[BORDER_INDEX_RIGHT]=thick;
           }
           else {
              if (flags&(PARA_BOX_THICK|PARA_BOX_DOUBLE)) thick=2;
              if (flags&PARA_BOX_TOP) frame[FrameNo].BorderWidth[BORDER_INDEX_TOP]=FRAME_BORDER_WIDTH*thick;
              if (flags&PARA_BOX_BOT) frame[FrameNo].BorderWidth[BORDER_INDEX_BOT]=FRAME_BORDER_WIDTH*thick;
              if (flags&PARA_BOX_LEFT) frame[FrameNo].BorderWidth[BORDER_INDEX_LEFT]=FRAME_BORDER_WIDTH*thick;
              if (flags&PARA_BOX_RIGHT) frame[FrameNo].BorderWidth[BORDER_INDEX_RIGHT]=FRAME_BORDER_WIDTH*thick;
           }
       }

       frame[FrameNo].SpaceTop=TwipsToPrtY(ParaFrame[CurParaFrame].margin
                               +frame[FrameNo].BorderWidth[BORDER_INDEX_TOP]);
       frame[FrameNo].SpaceBot=TwipsToPrtY(ParaFrame[CurParaFrame].margin
                               +frame[FrameNo].BorderWidth[BORDER_INDEX_BOT]);

       frame[FrameNo].height=TwipsToPrtY(ParaFrame[CurParaFrame].MinHeight);
       frame[FrameNo].ScrHeight=TwipsToScrY(ParaFrame[CurParaFrame].MinHeight);

       // allow negative values for y and ScrY here
       SetParaFrameY(w,CurParaFrame,FrameNo,Py+PCurColHeight,PHiddenY,PHdrMargin,PHdrHeight,PTopSect,l);
       
       frame[FrameNo].y=frame[FrameNo].y;
       frame[FrameNo].ScrY=frame[FrameNo].ScrY;

       if (repaginating) {
          ParaFrame[CurParaFrame].flags|=PARA_FRAME_POSITIONED;
          ParaFrame[CurParaFrame].InUse=TRUE;
       }

       // update the ParaFrame y (relative to the top of the page);
       ParaFrame[CurParaFrame].y=PrtToTwipsY(frame[FrameNo].y-FirstPageHeight);

       frame[FrameNo].x=TwipsToPrtX(ParaFrame[CurParaFrame].x);
       
       if (BorderShowing) frame[FrameNo].x+=(LeftBorderWidth+PTopLeftMargin);
       
       frame[FrameNo].width=TwipsToPrtX(ParaFrame[CurParaFrame].width);
       //       +(frame[FrameNo].BorderWidth[BORDER_INDEX_LEFT]+frame[FrameNo].BorderWidth[BORDER_INDEX_RIGHT])/2);
       frame[FrameNo].SpaceLeft=TwipsToPrtX(ParaFrame[CurParaFrame].margin
                                 +frame[FrameNo].BorderWidth[BORDER_INDEX_LEFT]);
       frame[FrameNo].SpaceRight=TwipsToPrtX(ParaFrame[CurParaFrame].margin
                                 +frame[FrameNo].BorderWidth[BORDER_INDEX_RIGHT]);
             
       // adjust the width if needed
       if (ParaFrame[CurParaFrame].flags&PARA_FRAME_SET_WIDTH && (TRUE || (l+1)==TotalLines || fid(l)==fid(l+1))) {
           int width=0;
           long k;
           if (cid(l)>0) {
              int row=cell[cid(l)].row;
              width=GetRowWidth(w,row)+2*ParaFrame[CurParaFrame].margin;
              if (width>ParaFrame[CurParaFrame].width) ParaFrame[CurParaFrame].width=width; 
           }
           else { 
             for (k=l;(k<TotalLines && fid(k)==fid(l));k++) width+=GetLineWidth(w,k,TRUE,FALSE);
             ParaId=pfmt(l);
             width=PrtToTwipsX(width)+2*ParaFrame[CurParaFrame].margin+MIN_FRAME_MARGIN
                +PfmtId[ParaId].LeftIndentTwips+PfmtId[ParaId].FirstIndentTwips+PfmtId[ParaId].RightIndentTwips;
             if (width<ParaFrame[CurParaFrame].width) ParaFrame[CurParaFrame].width=width; 
           }
           ParaFrame[CurParaFrame].flags=ResetUintFlag(ParaFrame[CurParaFrame].flags,PARA_FRAME_SET_WIDTH);
           if (ParaFrame[CurParaFrame].flags&(PARA_FRAME_RIGHT_JUST|PARA_FRAME_CENTER)) {
              ParaFrame[CurParaFrame].x=PrtToTwipsX(pPrt->ColumnX+pPrt->ColumnWidth)-ParaFrame[CurParaFrame].width;
              if (True(ParaFrame[CurParaFrame].flags&PARA_FRAME_CENTER)) ParaFrame[CurParaFrame].x/=2;
              frame[FrameNo].x=TwipsToPrtX(ParaFrame[CurParaFrame].x);
              if (BorderShowing) frame[FrameNo].x+=(LeftBorderWidth+PTopLeftMargin);
           } 
       }

       // set current variables
       Py=frame[FrameNo].y+frame[FrameNo].SpaceTop;
       pScr->y=PrtToScrY(Py);
       PCellX=PColumnX=frame[FrameNo].x+frame[FrameNo].SpaceLeft;
       PColumnWidth=frame[FrameNo].width-frame[FrameNo].SpaceLeft-frame[FrameNo].SpaceRight;
       PColumnSpace=0;
       PFrameHt=pScr->FrameHt=0;
       PCurColHeight=pScr->CurColHeight=0;
       PMaxColHeight=pScr->MaxColHeight=0;

       FrameNo++;                      // advance to the next frame
       if (!InitFrame(w,FrameNo)) AbortTer(w,"Ran Out of Text Frame Table (CreateFrames 5)",1);

    }
    else {                          // switching to regular text
       FarMove(pSavePrt,pPrt,sizeof(FRAMESET));  
       FarMove(pSaveScr,pScr,sizeof(FRAMESET));  
    }  


    // update the reference variables
    (*pFrameNo)=FrameNo;

    return TRUE;
 
}
 
/******************************************************************************
    SetFrameLineInfo:
    Set the information for the line begin built in the frame.
*******************************************************************************/
SetFrameLineInfo(PTERWND w,long l,int y,int ScrY, LPINT pFrameHt,LPINT pScrFrameHt, int CurColHeight,
                 int CellX,LPINT pTableRowIndent,int PageNo,int HiddenY,int HdrMargin,int HdrHeight,
                 int TopSect,int TopLeftMargin,int ColumnNo,LPINT pHasPictFrames, int sect,
                 int CurLineHt)
{
    int FrameHt=(*pFrameHt);
    int ScrFrameHt=(*pScrFrameHt);
    int ParaFID=fid(l);
    
    if (ParaFID && ParaFrame[ParaFID].flags&PARA_FRAME_POSITIONED) {
       LineY(l)=TwipsToPrtY(ParaFrame[ParaFID].y)+FrameHt;
    } 
    else LineY(l)=y+CurColHeight+FrameHt-FirstPageHeight; // y position of the line relative to page top

    LineX(l)=CellX;                       // x position of the line

    if (LineFlags(l)&LFLAG_CONTROL) PageHasControls=TRUE;
    LineFlags(l)=ResetLongFlag(LineFlags(l),LFLAG_FRAME_TOP);

    // account for any space before the current table row
    if (cid(l) && cell[cid(l)].PrevCell<=0 && (l==0 || cid(l)!=cid(l-1))) {   // first line of a table row
        int row=cell[cid(l)].row;
        int SpcBefRow=TableRow[row].FrmSpcBef;
        LineY(l)+=SpcBefRow;
        y+=SpcBefRow;                 // this space is not treated as part of the current frame
        ScrY+=PrtToScrY(SpcBefRow);

        // calculate the left indentation for this table row
        (*pTableRowIndent)=TableRow[row].CurIndent;
    }
    if (cid(l) && cell[cid(l)].PrevCell<=0) LineX(l)+=(*pTableRowIndent); // increment x before the first column

    // position any picture frames contained in the line
    if (LineFlags(l)&LFLAG_PICT) {
        int CurFID=fid(l);
        if (repaginating && l<=PageInfo[PageNo].LastLine) {
            int x=LineX(l);                // calculate x relative to the beginning of the frame page
            if (BorderShowing) x-=(LeftBorderWidth+TopLeftMargin); // relative to left margin
            PosPictFrames(w,l,x,LineY(l),HiddenY,HdrMargin,HdrHeight,TopSect,PageNo); // position the picture frames
        }
        (*pHasPictFrames)=TRUE;
    }

    // calculate the space before the line and adjust frame height
    if (!tabw(l) || !(tabw(l)->type&INFO_ROW)) {
        int FrmSpcBef=0;
        if (fid(l)==0) FrmSpcBef=CalcFrmSpcBef(w,l,sect,TRUE,PageNo);
        FrameHt=FrameHt+CurLineHt+FrmSpcBef;
        ScrFrameHt=PrtToScrY(FrameHt);
        LineY(l)+=FrmSpcBef;
    }

    // update page number
    if (!repaginating && tabw(l) && tabw(l)->type&INFO_DYN_FIELD) {   // don't do during repagination as it through off the final cursor position
        long SaveRepageBeginLine=RepageBeginLine;
        UpdateDynField(w,l,PageNo);
        RepageBeginLine=SaveRepageBeginLine;
    }

    // set the x adjustment for centering/right justification
    JustAdjX(l)=0;
    if (LineFlags(l)&LFLAG_ASSUMED_TAB) {
       if (tabw(l) && tabw(l)->count>0) JustAdjX(l)=tabw(l)->width[0];
    }
    else if (PfmtId[pfmt(l)].flags&(CENTER|RIGHT_JUSTIFY) && !LineInfo(w,l,INFO_FRAME)) {
        int CurPara=pfmt(l);
        int CurSect=(TerSect[TopSect].columns==TerSect[sect].columns)?TopSect:sect;
        int AdjWidth=TerWrapWidth(w,l,CurSect)
                -PfmtId[CurPara].LeftIndent
                -((LineFlags(l)&LFLAG_PARA_FIRST)?PfmtId[CurPara].FirstIndent:0)
                -PfmtId[CurPara].RightIndent
                -GetLineWidth(w,l,FALSE,TRUE);
        if (AdjWidth>0) {
            JustAdjX(l)=ScrToPrtX(AdjWidth);   // save the adjustment value for centering/right-justification
            if ((PfmtId[CurPara].flags)&CENTER) JustAdjX(l)=JustAdjX(l)/2;
        }
    }

    // pass the values by reference
    (*pFrameHt)=FrameHt;
    (*pScrFrameHt)=ScrFrameHt;

    return TRUE;
}

/******************************************************************************
    CreateFramesScr:
    Translate the frames from printer units to screen units.
*******************************************************************************/
void CreateFramesScr(PTERWND w)
{
    int i,y,TempY,PageTextHt=0,PageNo=FirstFramePage,FirstPageTextHt=0;
    long l;
    BOOL select;
    BOOL OnFirstPage=true;

    CurTextHeight=CurPageHeight=0;

    // Convert cooridnates to screen pixel values
    for (i=0;i<TotalFrames;i++) {
       int TempX=frame[i].x;

       frame[i].ScrY=PrtToScrY(frame[i].y);           // equate to printer units
       frame[i].ScrHeight=PrtToScrY(frame[i].y+frame[i].height)-frame[i].ScrY;
       
       if (frame[i].level>0 && frame[i].CellId>0 && !frame[i].empty) {   // update the row height of the last subtabel row to eliminate rounding
          int CellId=frame[i].CellId;
          int row=cell[CellId].row;
          if (cell[CellId].RowSpan==1 && !(cell[CellId].flags&CFLAG_ROW_SPANNED && TableRow[row].NextRow<=0)) {
             TableRow[row].height=ScrToPrtY(frame[i].ScrHeight);
          }  
       } 


       // calculate the screen line height of the lines
       if (!frame[i].empty && !(frame[i].flags&FRAME_PICT) && frame[i].PageFirstLine>=0) {
          y=frame[i].y+frame[i].SpaceTop;
          for (l=frame[i].PageFirstLine;l<=frame[i].PageLastLine;l++) {
             if (TableLevel(w,l)!=frame[i].level) continue;      // not a displayable line

             if (PrtResY==ScrResY) ScrLineHt(l)=LineHt(l);
             else {
                TempY=y+LineHt(l);
                ScrLineHt(l)=PrtToScrY(TempY)-PrtToScrY(y);
                y=TempY;
             }
          }
       }


       // calculate x for the table cell from the previous cell
       if (frame[i].RowId>0 && frame[i].flags&FRAME_RTL) {
          frame[i].width=PrtToScrX(frame[i].width);
          if (frame[i].flags&FRAME_FIRST_ROW_FRAME) 
                frame[i].x=PrtToScrX(frame[i].x);
          else  frame[i].x=frame[i-1].x-frame[i].width;
       }
       else { 
          if (i>0 && frame[i].RowId>0 && frame[i-1].RowId==frame[i].RowId)      // when a cell follows another cell in the row
              frame[i].x=frame[i-1].x+frame[i-1].width;
          else frame[i].x=PrtToScrX(frame[i].x);
          frame[i].width=PrtToScrX(TempX+frame[i].width)-frame[i].x;
       }

       frame[i].ScrX=frame[i].x;
       frame[i].ScrWidth=frame[i].width;

       frame[i].y=frame[i].ScrY;
       
       frame[i].height=frame[i].ScrHeight;
       
       if (frame[i].LimitY!=-1) frame[i].LimitY=PrtToScrY(frame[i].LimitY);

       frame[i].SpaceTop=PrtToScrY(frame[i].SpaceTop);          // additional space at the top of the frame
       frame[i].SpaceBot=PrtToScrY(frame[i].SpaceBot);          // additional space at the bottom of the frame
       frame[i].SpaceLeft=PrtToScrX(frame[i].SpaceLeft);        // left margin
       frame[i].SpaceRight=PrtToScrX(frame[i].SpaceRight);      // right margin

       if ((!frame[i].empty || frame[i].flags&FRAME_FNOTE) &&(frame[i].y+frame[i].TextHeight)>CurTextHeight)
           CurTextHeight=frame[i].y+frame[i].TextHeight;
       
       if ((frame[i].y+frame[i].height)>CurPageHeight)
           CurPageHeight=frame[i].y+frame[i].height;

       if (i<FirstPage2Frame) FirstPageHeight=frame[i].y+frame[i].height;

       // calculate page text height
       if (FirstPageTextHt==0 && i>=FirstPage2Frame && OnFirstPage) {
          PageInfo[PageNo].TextHt=FirstPageTextHt=PageTextHt;
          PageInfo[PageNo].ScrHt=PageInfo[PageNo].TextHt;
          PageNo++;
          PageTextHt=0;
          OnFirstPage=false;   // on the second page of the frame-set
       } 
       
       select=(!frame[i].empty && !(frame[i].flags&FRAME_DISABLED) && (frame[i].ScrY+frame[i].TextHeight)>PageTextHt);
       if (frame[i].flags&FRAME_FNOTE) select=TRUE;
       if (select) PageTextHt=frame[i].ScrY+frame[i].TextHeight;
    }

    PageInfo[PageNo].TextHt=PageTextHt;
    if (PageNo>FirstFramePage) PageInfo[PageNo].TextHt-=FirstPageTextHt;
    PageInfo[PageNo].ScrHt=PageInfo[PageNo].TextHt;
    
    return;
}

/******************************************************************************
    SetBoxFrameHt:
    Set the height of the box frame.
*******************************************************************************/
SetBoxFrameHt(PTERWND w,int BoxFrame, int PrevParaFrame, int BoxHeight, int FrameLastLine)
{
     int TextAngle=ParaFrame[PrevParaFrame].TextAngle;
     struct StrFrame far *pFrameBox=&(frame[BoxFrame]);
     int ParaFrameHt,MinHeight;
     int SpaceTop=pFrameBox->SpaceTop;
     int SpaceBot=pFrameBox->SpaceBot;

     pFrameBox->PageLastLine=FrameLastLine;

     if (TextAngle==0 && False(ParaFrame[PrevParaFrame].flags&PARA_FRAME_FIXED_HEIGHT)) {
        MinHeight=TwipsToPrtY(ParaFrame[PrevParaFrame].MinHeight);
                  
        if (MinHeight==0 || False(ParaFrame[PrevParaFrame].flags&PARA_FRAME_TEXT_BOX)) {
           ParaFrameHt=BoxHeight+SpaceTop+SpaceBot;

           if (ParaFrameHt>MinHeight) pFrameBox->height=ParaFrameHt;
                  
           if (pFrameBox->height!=TwipsToPrtY(ParaFrame[PrevParaFrame].height)) {
              PaintFlag=PAINT_WIN_RESET;
              ParaFrame[PrevParaFrame].height=PrtToTwipsY(pFrameBox->height);
           }
        }
     }

     pFrameBox->ScrHeight=PrtToScrY(pFrameBox->height);
     pFrameBox->TextHeight=0;    // box frame does not have text

     return TRUE;
}

/******************************************************************************
    CreatePageBorderFrames:
    Create the border frames when the page border is displayed
*******************************************************************************/
#pragma optimize("g",off)  // turn-off global optimization

CreatePageBorderFrames(PTERWND w, int PageNo, LPINT pY, LPINT pScrY, LPINT pFrameNo, LPINT pLeftFrame, LPINT pRightFrame)
{
    int y,ScrY,FrameNo,sect,SaveY,SaveScrY,LeftFrame,RightFrame,PgWidth;
    long line;

    y=(*pY);
    ScrY=(*pScrY);
    FrameNo=(*pFrameNo);

    line=PageInfo[PageNo].FirstLine; // first line of the page
    sect=GetSection(w,line);      // find the section to take the header lines form

    // caculate the border width
    LeftBorderWidth=TwipsToPrtX(PAGE_BORDER_MARGIN);
    PgWidth=(int)(PrtResX*TerSect1[sect].PgWidth);
    if ((PgWidth+2*LeftBorderWidth)<ScrToPrtX(TerWinWidth))  // center
       LeftBorderWidth=(ScrToPrtX(TerWinWidth)-PgWidth)/2;

    // create the top frame border
    frame[FrameNo].empty=TRUE;
    frame[FrameNo].y=y;
    frame[FrameNo].ScrY=ScrY;
    frame[FrameNo].x=0;
    frame[FrameNo].width=PgWidth+2*LeftBorderWidth;

    frame[FrameNo].height=TopBorderHeight=TwipsToPrtY(PAGE_BORDER_MARGIN);
    BotBorderHeight=TopBorderHeight/4;
    frame[FrameNo].ScrHeight=PrtToScrY(TopBorderHeight);

    frame[FrameNo].LeftBorderWdth=LeftBorderWidth;

    y+=frame[FrameNo].height;
    ScrY+=frame[FrameNo].ScrHeight;
    frame[FrameNo].flags=FRAME_RIGHTMOST|FRAME_BORDER_TOP;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);

    // create the left border
    SaveY=y;
    SaveScrY=ScrY;
    LeftFrame=FrameNo;
    frame[FrameNo].empty=TRUE;
    frame[FrameNo].y=SaveY;
    frame[FrameNo].ScrY=SaveScrY;
    frame[FrameNo].x=0;
    frame[FrameNo].width=LeftBorderWidth;

    frame[FrameNo].height=(int)(PrtResY*TerSect1[sect].PgHeight);  // this value will be updated later
    frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);
    frame[FrameNo].flags=FRAME_BORDER_LEFT;


    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);


    // create the right border
    RightFrame=FrameNo;
    frame[FrameNo].empty=TRUE;
    frame[FrameNo].y=SaveY;
    frame[FrameNo].ScrY=SaveScrY;
    frame[FrameNo].x=LeftBorderWidth+PgWidth;  // this value will be updated later
    frame[FrameNo].width=LeftBorderWidth;

    frame[FrameNo].height=(int)(PrtResY*TerSect1[sect].PgHeight);  // this value will be updated later
    frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);
    frame[FrameNo].flags=FRAME_BORDER_RIGHT|FRAME_RIGHTMOST;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);

    // create the top margin frame when header/footer not displayed
    frame[FrameNo].empty=TRUE;
    frame[FrameNo].y=SaveY;
    frame[FrameNo].ScrY=SaveScrY;
    frame[FrameNo].x=frame[LeftFrame].x+frame[LeftFrame].width;
    frame[FrameNo].width=frame[RightFrame].x-frame[FrameNo].x;


    if (ViewPageHdrFtr) {
       int HdrHeight=PageHdrHeight2(w,PageNo,FALSE,true);
       int HdrMargin;

       if (HdrHeight>0) HdrMargin=(int)(PrtResY*TerSect[sect].HdrMargin);   // top margin including the hidden space
       else             HdrMargin=(int)(PrtResY*TerSect[sect].TopMargin);   // no header text, so use the top margin instead
     
       frame[FrameNo].height=HdrMargin;  // this value will be updated later
    }
    else frame[FrameNo].height=(int)(PrtResY*TerSect[sect].TopMargin);  // this value will be updated later

    frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);

    SaveY+=frame[FrameNo].height;
    SaveScrY+=frame[FrameNo].ScrHeight;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);


    // pass the ending values
    (*pY)=SaveY;
    (*pScrY)=SaveScrY;
    (*pFrameNo)=FrameNo;
    (*pLeftFrame)=LeftFrame;
    (*pRightFrame)=RightFrame;

    return TRUE;
}
#pragma optimize("",off)  // restore optimization

/******************************************************************************
    CreateWatermarkFrame:
    Create the watermark frame.
*******************************************************************************/
CreateWatermarkFrame(PTERWND w, int PageNo, int y, LPINT pFrameNo, int TopSect, int TopLeftMargin, int HiddenY, int HdrMargin, int HdrHeight)
{
    int FrameNo,ParaFID;

    FrameNo=(*pFrameNo);

    if (WmParaFID<=0) return TRUE;
    
    ParaFID=WmParaFID;

    // position the watermark
    if (!repaginating) PosWatermarkFrame(w,PageNo);

    // create the top frame border
    frame[FrameNo].empty=TRUE;
    
    SetParaFrameY(w,ParaFID,FrameNo,y,HiddenY,HdrMargin,HdrHeight,TopSect,-1);
    frame[FrameNo].x=TwipsToPrtX(ParaFrame[ParaFID].x);
    if (BorderShowing) frame[FrameNo].x+=(LeftBorderWidth+TopLeftMargin);
    frame[FrameNo].width=TwipsToPrtY(ParaFrame[ParaFID].width);
    frame[FrameNo].height=TwipsToPrtY(ParaFrame[ParaFID].height);

    frame[FrameNo].ScrHeight=PrtToScrY(frame[FrameNo].height);

    frame[FrameNo].BoxFrame=FrameNo;
    frame[FrameNo].flags=FRAME_WATERMARK;
    frame[FrameNo].ParaFrameId=ParaFID;

    TotalFrames++;FrameNo++;
    InitFrame(w,FrameNo);

    // pass the ending values
    (*pFrameNo)=FrameNo;

    return TRUE;
}

