/*===============================================================================
   TER_TBL1.C
   TER table handling routines.

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
    IsPartRow:
    Returns true if the split row is on the page at the top or bottom.
******************************************************************************/
IsPartRow(PTERWND w,BOOL top, int row, int PageNo)
{
    if (!(TableRow[row].flags&ROWFLAG_SPLIT)) return FALSE;  // not a split row
    
    if (top) return (PageNo>TableAux[row].FirstPage && PageNo<=TableAux[row].LastPage);
    else     return (PageNo==TableAux[row].FirstPage);
}        

/******************************************************************************
    LevelRow:
    Get row id of the specified level from the row id at the deepest level.
******************************************************************************/
LevelRow(PTERWND w,int level,int row)
{
    int FirstCell=TableRow[row].FirstCell,CurCell;
    int CurLevel=cell[FirstCell].level;

    if (CurLevel==0) return row;
    
    CurCell=LevelCell(w,level,-FirstCell);
    return cell[CurCell].row;
}

/******************************************************************************
    TerIsTableSelect:
    Retrurns TRUE if a cell, row, or table is selected.
******************************************************************************/
BOOL WINAPI _export TerIsTableSelected(HWND hWnd)
{
    PTERWND w;
    long l;
    int StartCol,EndCol,i;
    LPBYTE ptr;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (HilightType==HILIGHT_OFF) return FALSE;
    if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block

    for (l=HilightBegRow;l<=HilightEndRow;l++) {
       if (cid(l)==0) continue;
       
       StartCol=(l==HilightBegRow)?HilightBegCol:0;
       EndCol=(l==HilightEndRow)?HilightEndCol:LineLen(l);

       ptr=pText(l);
       for (i=StartCol;i<EndCol;i++) if (ptr[i]==CellChar || ptr[i]==ROW_CHAR) return TRUE;

    } 
    
    return FALSE;
}

/******************************************************************************
    TerSelectTable:
    Select the entire text in the cell. Set level to -1 to select the current level.
******************************************************************************/
BOOL WINAPI _export TerSelectTable(HWND hWnd, int level, BOOL repaint)
{
    PTERWND w;
    int CurCell;
    long l;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    CurCell=cid(CurLine);
    if (CurCell==0) return FALSE;
    
    if (level<0) level=cell[CurCell].level;
    if (level>cell[CurCell].level) return FALSE;

    for (l=CurLine;l>0;l--) if (cid(l-1)==0 || cell[cid(l-1)].level<level) break;
    HilightBegRow=l;
    HilightBegCol=0;

    for (l=CurLine;l<(TotalLines-1);l++) if (cid(l+1)==0 || cell[cid(l+1)].level<level) break;
    if (LineInfo(w,l,INFO_ROW)) l--;
    HilightEndRow=l;
    HilightEndCol=LineLen(l);

    StretchHilight=FALSE;
    HilightType=HILIGHT_CHAR;

    if (repaint) PaintTer(w);

    return TRUE;
}
 

/******************************************************************************
    TerSelectCellText:
    Select the entire text in the cell.
******************************************************************************/
BOOL WINAPI _export TerSelectCellText(HWND hWnd, BOOL repaint)
{
    PTERWND w;
    int CurCell;
    long l;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    CurCell=cid(CurLine);
    if (CurCell==0) return FALSE;

    for (l=CurLine;l>0;l--) if (cid(l-1)!=CurCell) break;
    HilightBegRow=l;
    HilightBegCol=0;

    for (l=CurLine;l<(TotalLines-1);l++) if (cid(l+1)!=CurCell) break;
    if (LineInfo(w,l,INFO_ROW)) l--;
    HilightEndRow=l;
    HilightEndCol=LineLen(l)-1;
    if (HilightEndCol<0) HilightEndCol=0;

    StretchHilight=FALSE;
    HilightType=HILIGHT_CHAR;

    if (repaint) PaintTer(w);

    return TRUE;
}
 
/******************************************************************************
    TerGetLevelCell:
    Get the cell at the specified level for the given line. Set the LineNo to 
    a negative number to specify the cell id for line.
******************************************************************************/
int WINAPI _export TerGetLevelCell(HWND hWnd,int level, long LineNo)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    return LevelCell(w,level,LineNo);
}

/******************************************************************************
    InOuterLevel:
    Check if the given line is at a outer lovel.
******************************************************************************/
InOuterLevels(PTERWND w,int level, long LineNo)
{
   int CurCell;

   if (LineNo>=0) CurCell=cid(LineNo);
   else           CurCell=(int)(-LineNo);

   return (CurCell==0 || cell[CurCell].level<level);
}

        
/******************************************************************************
    LevelCell:
    Get cell id of the specified level cell at the specified line number.  Set LineNo
    to a negative value to specify a cell id at the deepest level.
******************************************************************************/
LevelCell(PTERWND w,int level,long LineNo)
{
   int CurCell;

   if (LineNo>=0) CurCell=cid(LineNo);
   else           CurCell=(int)(-LineNo);

   if (CurCell==0 || cell[CurCell].level==0 || cell[CurCell].level==level) return CurCell;

   while ((CurCell=cell[CurCell].ParentCell)>0) if (cell[CurCell].level==level) break;

   return CurCell;
}

/******************************************************************************
    TerGetTableLevel:
    Get table level of a line.  Set the LineNo to negative to specify a cell id.
******************************************************************************/
int WINAPI _export TerGetTableLevel(HWND hWnd,long LineNo)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (LineNo<0) {
       int cl=(int)(-LineNo);
       if (cl>=TotalCells || !cell[cl].InUse) return 0;
       return cell[cl].level;
    }    
        
    return TableLevel(w,LineNo);
}
    
/******************************************************************************
    TableLevel:
    Get table level of a line
******************************************************************************/
TableLevel(PTERWND w,long LineNo)
{
   int CurCell;

   if (LineNo<0 || LineNo>=TotalLines) return 0;

   CurCell=cid(LineNo);
   if (CurCell==0) return 0;

   return cell[CurCell].level;
}
    
/******************************************************************************
    MinTableLevel:
    Find the minimum table level used by the given range of lines
******************************************************************************/
MinTableLevel(PTERWND w,long FromLine,long ToLine)
{
    int level;
    long l;

    if (TableLevel(w,FromLine)==0 && TableLevel(w,ToLine)==0) return 0;  // can't get any lower
    
    if (FromLine>ToLine) SwapLongs(&FromLine,&ToLine);
    
    level=TableLevel(w,FromLine);   

    for (l=FromLine+1;l<=ToLine;l++) if (TableLevel(w,l)<level) level=TableLevel(w,l);

    return level;
}    

/******************************************************************************
    GetTblSpcBef:
    Get subtable space before
******************************************************************************/
GetTblSpcBef(PTERWND w,long LineNo, BOOL screen)
{
    int height=0,row,cl,PageNo=LinePage(LineNo),CurHeight;
    int FirstRow;
    long InitLine=LineNo,l;

    if (!(TerArg.PageMode)) return 0;
    
    while (TRUE) {
      if (LineNo==0 || TableLevel(w,LineNo-1)<=TableLevel(w,InitLine)) return height;

      cl=cid(LineNo-1);
      if (cl<=0) return height;

      row=cell[cl].row;
      while (row>0) {
         FirstRow=row;
         if (PageNo>TableAux[row].LastPage) return height;      // remaining rows on the previous page

         if (TableRow[row].flags&ROWFLAG_SPLIT) CurHeight=TableAux[row].TopRowHt;
         else                                   CurHeight=TableRow[row].height;

         if (screen) height+=PrtToScrY(CurHeight);
         else        height+=CurHeight;

         if (TableRow[row].flags&ROWFLAG_SPLIT) return height;  // remaining rows on the previous page

         row=TableRow[row].PrevRow;
      }
    
      // In html file, there can be two tables one after another
      // scan backward to the first line of this table
      if (HtmlMode) {
         int SubTableLevel=TableLevel(w,LineNo-1);
         l=LineNo-1;
         if (cell[cid(l)].row!=FirstRow) {   // scan back to the first row
            for (;l>=0;l--) if (cell[cid(l)].row==FirstRow) break; 
         }
         // position before the first row
         for (;l>=0;l--) {
            if (TableLevel(w,l)>SubTableLevel) continue;   // go past inner tables
            if (cell[cid(l)].row!=FirstRow) break;
         }
         LineNo=l+1;
      }
      else break;
    }
    
    return height;
}

/******************************************************************************
    TerCellColor:
    Select Cell color
******************************************************************************/
BOOL WINAPI _export TerCellColor(HWND hWnd, int select, COLORREF color, BOOL repaint)
{
    int cl,CurCell;
    COLORREF  SaveColor;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && HilightType!=HILIGHT_CHAR)) return TRUE;


    // save the width of the current cell color

    if (select<=0) {
       SaveColor=cell[CurCell].BackColor;

       select=CallDialogBox(w,"CellColorParam",CellColorParam,0L);

       color=cell[CurCell].BackColor;      // restore the cell color percentage
       cell[CurCell].BackColor=SaveColor;
    }
    else {
       if      (select==SEL_CELLS) select=IDC_TABLE_SEL_CELLS;
       else if (select==SEL_COLS)  select=IDC_TABLE_SEL_COLS;
       else if (select==SEL_ROWS)  select=IDC_TABLE_SEL_ROWS;
       else if (select==SEL_ALL)   select=IDC_TABLE_ALL;
       else select=0;
    }

    if (select==0) return TRUE;

    // mark the selected cells
    if (!MarkCells(w,select)) return FALSE;
    
    SaveUndo(w,0,0,0,0,UNDO_TABLE_ATTRIB);   // save for undo of marked cells

    HilightType=HILIGHT_OFF;     // remove highlighting so color can be seen

    //  Set the color for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       cell[cl].BackColor=color;
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerTabCell
    Handle a tab character input in a cell.
******************************************************************************/
TerTabCell(PTERWND w)
{
    long l,FirstLine,LastLine;
    int  CurCell,CurLevel,level;

    if (TerFlags3&TFLAG3_CURSOR_IN_CELL) return TRUE;   // can't escape this table cell

    // find the first line of the next cell
    CurCell=cid(CurLine);
    CurLevel=cell[CurCell].level;

    for (l=CurLine+1;l<TotalLines;l++) {
       //if (TableLevel(w,l)!=TableLevel(w,CurLine)) continue;
       if (cid(l)==0 || LevelCell(w,CurLevel,l)!=CurCell && !(cell[cid(l)].flags&CFLAG_ROW_SPANNED)) break;
    }
    if (l>=TotalLines) return TRUE;         // last cell in the file
    CurCell=LevelCell(w,CurLevel,l);        // cell at the current level
    if (CurCell==0) return TRUE;            // last cell of the table
    FirstLine=l;

    // find the last line of the cell
    for (l=FirstLine+1;l<TotalLines;l++) {
       //if (TableLevel(w,l)!=TableLevel(w,CurLine)) continue;
       level=cell[cid(l)].level;

       if (  LevelCell(w,CurLevel,l)!=CurCell
         || (level==CurLevel && tabw(l) && tabw(l)->type&INFO_ROW) ) {
         LastLine=l-1;                      // last line of the cell
         break;
       }
    }
    if (LastLine>=TotalLines) LastLine=TotalLines-1;

    // set the current line and column position
    CurLine=LastLine;
    CurCol=LineLen(LastLine)-1;
    if (CurCol<0) CurCol=0;

    // Highlight the next cell
    HilightType=HILIGHT_OFF;
    HilightBegRow=FirstLine;
    HilightBegCol=0;
    HilightEndRow=CurLine;
    HilightEndCol=LineLen(CurLine); // CurCol;
    if (HilightBegRow!=HilightEndRow || HilightBegCol!=HilightEndCol) HilightType=HILIGHT_CHAR;

    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerBackTabCell
    Handle a shift tab character input in a cell.
******************************************************************************/
TerBackTabCell(PTERWND w)
{
    long l,FirstLine,LastLine;
    int  CurCell,CurLevel;

    if (TerFlags3&TFLAG3_CURSOR_IN_CELL) return TRUE;   // can't escape this table cell

    // find the last line of the prev cell
    CurCell=cid(CurLine);
    CurLevel=cell[CurCell].level;

    for(l=CurLine-1;l>=0;l--) {
       //if (TableLevel(w,l)!=TableLevel(w,CurLine)) continue;
       if (cid(l)==0 || LevelCell(w,CurLevel,l)!=CurCell && tabw(l) && tabw(l)->type&INFO_CELL && !(cell[cid(l)].flags&CFLAG_ROW_SPANNED)) break;
    }
    if (l<0) return TRUE;         // last cell in the file

    CurCell=LevelCell(w,CurLevel,l);
    if (CurCell==0) return TRUE;            // first cell of the table
    LastLine=l;

    // find the first line of the cell
    for(l=LastLine-1;l>=0;l--) {
       //if (TableLevel(w,l)!=TableLevel(w,CurLine)) continue;
       if (LevelCell(w,CurLevel,l)!=CurCell) break;
    }
    FirstLine=l+1;

    // set the current line and column position
    CurLine=LastLine;
    CurCol=LineLen(LastLine)-1;
    if (CurCol<0) CurCol=0;

    // Highlight the next cell
    HilightType=HILIGHT_OFF;
    HilightBegRow=FirstLine;
    HilightBegCol=0;
    HilightEndRow=CurLine;
    HilightEndCol=LineLen(CurLine);  //CurCol;
    if (HilightBegRow!=HilightEndRow || HilightBegCol!=HilightEndCol) HilightType=HILIGHT_CHAR;

    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerToggleTableGrid:
    Toggle the display of table grid lines.
******************************************************************************/
TerToggleTableGrid(PTERWND w)
{
    if (!(TerArg.PageMode)) return TRUE;

    ShowTableGridLines=!ShowTableGridLines;

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerSetDefCellMargin:
    Set default cell margin (in twips)
******************************************************************************/
BOOL WINAPI _export TerSetDefCellMargin(HWND hWnd, int margin)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    DefCellMargin=margin;

    return TRUE;
}

/******************************************************************************
    TerIsTableWider:
    This function returns TRUE if any table in the document is wider than the given
    pixels (screen pixels).  Set MaxWidth to a negative value to specify the twips
******************************************************************************/
BOOL WINAPI _export TerIsTableWider(HWND hWnd, int MaxWidth)
{
    int i,cl,width;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    TerRepaginate(hWnd,TRUE);

    SetCellLines(w);    // set the cell lines

    if (MaxWidth==0) MaxWidth=ScrToTwipsX(TerWinWidth);
    else {
        if (MaxWidth<0) MaxWidth=-MaxWidth;               // already specified in twips
        else            MaxWidth=ScrToTwipsX(MaxWidth);
    }

    for (i=0;i<TotalTableRows;i++) {
       cl=TableRow[i].FirstCell;
       if (!cell[cl].InUse || cell[cl].FirstLine<0) continue;  // row not in use

       // get the row width
       width=TableRow[i].indent;
       while (cl>0) {
         width+=cell[cl].width;
         cl=cell[cl].NextCell;
       }
       if (width>MaxWidth) return TRUE;
    }

    return FALSE;
}

/******************************************************************************
    TerShrinkTable:
    This function shrinks any table wider than the given width (specified in
    screen pixels).
******************************************************************************/
BOOL WINAPI _export TerShrinkTable(HWND hWnd, int MaxWidth, int MinCellWidth)
{
    int i,cl,RowWidth,ExtraShrink,x;
    PTERWND w;
    BOOL ShrankAll=TRUE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    SetCellLines(w);    // set the cell lines

    if (MaxWidth==0) MaxWidth=ScrToTwipsX(TerWinWidth);
    else {
        if (MaxWidth<0) MaxWidth=-MaxWidth;               // already specified in twips
        else            MaxWidth=ScrToTwipsX(MaxWidth);
    }
    MaxWidth-=90;                                         // make it a little smaller

    if (MinCellWidth<0) MinCellWidth=-MinCellWidth;
    else                MinCellWidth=ScrToTwipsX(MinCellWidth);

    for (i=0;i<TotalTableRows;i++) {
       cl=TableRow[i].FirstCell;
       if (!cell[cl].InUse || cell[cl].FirstLine<0) continue;  // row not in use

       // get the row width
       RowWidth=TableRow[i].indent;
       while (cl>0) {
         RowWidth+=cell[cl].width;
         cl=cell[cl].NextCell;
       }

       // shrink the table row if ncessary
       if (RowWidth>MaxWidth) {
          ExtraShrink=0;                   // extra shrink when a cell can not shrink any more
          TableRow[i].indent=x=MulDiv(TableRow[i].indent,MaxWidth,RowWidth);

          cl=TableRow[i].FirstCell;
          while (cl>0) {
             cell[cl].x=x;
             cell[cl].width=MulDiv(cell[cl].width,MaxWidth,RowWidth);

             if (cell[cl].width>MinCellWidth && ExtraShrink>0) {
                 cell[cl].width-=ExtraShrink;
                 ExtraShrink=0;
             }
             if (cell[cl].width<MinCellWidth) {
                ExtraShrink+=(MinCellWidth-cell[cl].width);  // extra shrinkage to be compensated from other cells
                cell[cl].width=MinCellWidth;
             }

             x+=cell[cl].width;
             cl=cell[cl].NextCell;
          }
          if (ExtraShrink>0) ShrankAll=FALSE;   // could not shrink this row completely.
       }
    }

    Repaginate(w,FALSE,FALSE,0,TRUE);
    return TRUE;
}


/******************************************************************************
    TerDiffTableRows:
    This function returns TRUE if two table rows have different cell configuration.
******************************************************************************/
BOOL WINAPI _export TerDifTableRows(HWND hWnd, int row1, int row2)
{
    int cell1,cell2,span1,span2,width1,width2;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (TerFlags3&TFLAG3_HTML_CONT_TABLE) return FALSE;  // non-uniform continuous table allowed

    cell1=TableRow[row1].FirstCell;
    cell2=TableRow[row2].FirstCell;

    span1=cell[cell1].ColSpan;
    span2=cell[cell2].ColSpan;
    width1=cell[cell1].width;
    width2=cell[cell2].width;

    while (TRUE) {
       if (cell1<=0 && cell2<=0) break;       // end of table
       if (cell1<=0 && cell2>0) return TRUE; // rows don't match
       if (cell2<=0 && cell1>0) return TRUE;

       if (span1<=0) {
          span1=cell[cell1].ColSpan;
          width1+=cell[cell1].width;
       }
       if (span2<=0) {
          span2=cell[cell2].ColSpan;
          width2+=cell[cell2].width;
       }

       if ((span1==span2) && abs(width1-width2)>RULER_TOLERANCE) return TRUE;

       span1--;
       span2--;
       if (span1==0 && span2==0) width1=width2=0;

       if (span1<=0) cell1=cell[cell1].NextCell;
       if (span2<=0) cell2=cell[cell2].NextCell;
    }

    return FALSE;
}
/******************************************************************************
    TerSetRowKeep:
    Set or reset the 'row keep'.  Set the cell id to 0 to set the
    current row.
******************************************************************************/
BOOL WINAPI _export TerSetRowKeep(HWND hWnd, int CurCell, BOOL set, BOOL refresh)
{
    int row;
    PTERWND w;
    long l;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return TRUE;


    if (HilightType==HILIGHT_OFF) {  // set the current row
      if (CurCell<=0) CurCell=cid(CurLine);
      if (CurCell==0) return TRUE;

      row=cell[CurCell].row;

      if (set) TableRow[row].flags|=ROWFLAG_KEEP;
      else     ResetUintFlag(TableRow[row].flags,ROWFLAG_KEEP);
    }
    else {             // 20050922 - set the selected rows   
       if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block

       for (l=HilightBegRow;l<=HilightEndRow;l++) {
          if (cid(l)==0) continue;
          CurCell=cid(l);

          row=cell[CurCell].row;

          if (set) TableRow[row].flags|=ROWFLAG_KEEP;
          else     ResetUintFlag(TableRow[row].flags,ROWFLAG_KEEP);
       }
    } 


    TerArg.modified++;
    if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;


    if (refresh) Repaginate(w,FALSE,TRUE,0,TRUE);

    return TRUE;
}

/******************************************************************************
MAK:
TerSetHdrRow2:
Set or reset the hdr row flag.  Set the cell id to 0 to set the
current row.
******************************************************************************/
BOOL WINAPI _export TerSetHdrRow2(HWND hWnd, int select, BOOL set, BOOL refresh)
{
  int row;
  int cl, CurCell=0, CurRowId;
  PTERWND w;

  if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

  if      (select==SEL_CELLS) select=IDC_TABLE_SEL_CELLS;
  else if (select==SEL_COLS)  select=IDC_TABLE_SEL_COLS;
  else if (select==SEL_ROWS)  select=IDC_TABLE_SEL_ROWS;
  else if (select==SEL_ALL)   select=IDC_TABLE_ALL;
  else select=0;

  // mark the selected cells
  if (!MarkCells(w,select)) return FALSE;

  //  Set the border for the marked cells
  for (cl=0;cl<TotalCells;cl++)
  {
    if (!(cell[cl].InUse) || !(cell[cl].flags&CFLAG_SEL1)) continue;

    // scan cell in the same row
    CurRowId=cell[cl].row;
    if(TableRow[CurRowId].FirstCell)
    {
      if (set) TableRow[CurRowId].flags|=ROWFLAG_HDR;
      else     ResetUintFlag(TableRow[CurRowId].flags,ROWFLAG_HDR);
    }
  }

/*

  if (CurCell<=0) CurCell=cid(CurLine);
  if (!TerArg.PageMode || CurCell==0) return TRUE;

  row=cell[CurCell].row;

  if (set) TableRow[row].flags|=ROWFLAG_HDR;
  else     ResetUintFlag(TableRow[row].flags,ROWFLAG_HDR);

  TerArg.modified++;
  if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;

*/

	TerArg.modified++;

  if (refresh) Repaginate(w,FALSE,TRUE,0,TRUE);

  return TRUE;
}

/******************************************************************************
    TerSetHdrRow:
    Set or reset the hdr row flag.  Set the cell id to 0 to set the
    current row.
******************************************************************************/
BOOL WINAPI _export TerSetHdrRow(HWND hWnd, int CurCell, BOOL set, BOOL refresh)
{
    int row;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurCell<=0) CurCell=cid(CurLine);
    if (!TerArg.PageMode || CurCell==0) return TRUE;

    row=cell[CurCell].row;

    if (set) TableRow[row].flags|=ROWFLAG_HDR;
    else     ResetUintFlag(TableRow[row].flags,ROWFLAG_HDR);

    TerArg.modified++;
    if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;


    if (refresh) Repaginate(w,FALSE,TRUE,0,TRUE);

    return TRUE;
}


/******************************************************************************
    SetRowIndent:
    Get the indentation (printer units) for a table row
******************************************************************************/
SetRowIndent(PTERWND w,long LineNo, int row, int sect, int ColumnWidth)
{
    int NextCell,TableRowWidth,indent,FrameSpaceWidth;

    if (TableRow[row].flags&(RIGHT_JUSTIFY|CENTER)) {
        TableRowWidth=0;
        NextCell=TableRow[row].FirstCell;
        while (NextCell>0) {
           TableRowWidth+=cell[NextCell].width;
           NextCell=cell[NextCell].NextCell;
        }
        TableRow[row].indent=PrtToTwipsX(ColumnWidth)-TableRowWidth;
        if (TableRow[row].flags&CENTER) TableRow[row].indent=TableRow[row].indent/2;
        //if (TableRow[row].indent<0) TableRow[row].indent=0;  // 20050928 commented to handle table wider than column width
    }

    indent=TwipsToPrtX(TableRow[row].indent);

    // move for any frame in the beginning of the table
    FrameSpaceWidth=CalcFrmIndentBefRow(w,LineNo,sect);

    if (FrameSpaceWidth>indent && FrameSpaceWidth>0) indent=FrameSpaceWidth;

    TableRow[row].CurIndent=indent;

    return indent;
}

/******************************************************************************
    MarkCells:
    Select cells for an operation.
******************************************************************************/
MarkCells(PTERWND w,int select)
{
    int i;
    int  cl,CurCell,PrevCell,CurRowId,level;
    long l;


    // reset the cell selection flag
    for (i=0;i<TotalCells;i++) cell[i].flags=ResetUintFlag(cell[i].flags,(CFLAG_SEL1|CFLAG_SEL2));

    // Select all cells in the table
    if (select==IDC_TABLE_ALL) {          // select the entire table
       CurCell=cid(CurLine);
       if (CurCell==0) return TRUE;
       CurRowId=cell[CurCell].row;
       // position at the first row
       while (TableRow[CurRowId].PrevRow>0) CurRowId=TableRow[CurRowId].PrevRow;  // find the first row
       // scan each row until the end of table
       while (CurRowId>0) {
          CurCell=TableRow[CurRowId].FirstCell;
          while (CurCell>0) {
             cell[CurCell].flags|=CFLAG_SEL1;
             CurCell=cell[CurCell].NextCell;
          }
          CurRowId=TableRow[CurRowId].NextRow;
       }
       return TRUE;
    }

    // collect the highlighted cells
    if (HilightType==HILIGHT_OFF) {
       CurCell=cid(CurLine);
       if (CurCell>0) cell[CurCell].flags=cell[CurCell].flags|CFLAG_SEL1;
    }
    else {
       if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block
       level=MinTableLevel(w,HilightBegRow,HilightEndRow);
       
       PrevCell=0;
       for (l=HilightBegRow;l<=HilightEndRow;l++) {
          if (!LineSelected(w,l)) continue;
          CurCell=cid(l);
          if (cell[CurCell].level>level) continue;  // do not mark subcells

          if (CurCell!=PrevCell) {
             PrevCell=CurCell;
             if (PrevCell>0) cell[PrevCell].flags=cell[PrevCell].flags|CFLAG_SEL1;
          }
       }
    }

    // Add other cells in the same column
    if (select==IDC_TABLE_SEL_COLS) {
       for (cl=0;cl<TotalCells;cl++) {
          if (!(cell[cl].InUse) || !(cell[cl].flags&CFLAG_SEL1)) continue;

          // collect column cells in forward direction
          CurCell=cl;
          while ((CurCell=GetNextCellInColumnPos(w,CurCell))>0) {
             cell[CurCell].flags=cell[CurCell].flags|CFLAG_SEL2;
          }
          // collect column cells in backward direction
          CurCell=cl;
          while ((CurCell=GetPrevCellInColumnPos(w,CurCell,TRUE))>0) {
             cell[CurCell].flags=cell[CurCell].flags|CFLAG_SEL2;
          }
       }
    }

    // Add other cells in the same row
    if (select==IDC_TABLE_SEL_ROWS) {
       for (cl=0;cl<TotalCells;cl++) {
          if (!(cell[cl].InUse) || !(cell[cl].flags&CFLAG_SEL1)) continue;

          // scan cell in the same row
          CurRowId=cell[cl].row;
          CurCell=TableRow[CurRowId].FirstCell;

          while (CurCell>0) {
             cell[CurCell].flags=cell[CurCell].flags|CFLAG_SEL2;
             CurCell=cell[CurCell].NextCell;
          }
       }
    }

    // check if any of the selected cells span other cells
    for (i=0;i<TotalCells;i++) {
       if (!(cell[i].InUse) || (cell[i].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;  // not in use or already selected
       cl=CellAux[i].SpanningCell; // cell that spans over this cell
       if (cl==0 || cl<0 || cl>=TotalCells) continue;
       if (cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2)) cell[i].flags|=CFLAG_SEL2;
    }

    return TRUE;
}

/******************************************************************************
   TerSelectRow:
   Select the current table row
*******************************************************************************/
BOOL WINAPI _export TerSelectRow(HWND hWnd, BOOL repaint)
{
    PTERWND w;
    long line;
    int level,CurCell,cl,row;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    CurCell=cid(CurLine);
    if (CurCell==0) return FALSE;
    
    row=cell[CurCell].row;
    level=cell[CurCell].level;   // cell level

    // find the first line of the table row
    for (line=CurLine;line>0;line--) {
        cl=LevelCell(w,level,line-1);  // test the previous line
        if (cl==0) break;
        if (row!=cell[cl].row) break;
    }
    HilightBegRow=line;
    HilightBegCol=0;
     
    // find the last line of the table row
    for (line=CurLine;(line+1)<TotalLines;line++) {
        cl=LevelCell(w,level,line+1);  // test the next line
        if (cl==0) break;
        if (row!=cell[cl].row) break;
    }
    HilightEndRow=line;
    HilightEndCol=LineLen(HilightEndRow);
     
    HilightType=HILIGHT_CHAR;
    StretchHilight=FALSE;                     // halt any stretching
    HilightWithColCursor=FALSE;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
   TerSelectCol:
   process the left arrow when stretching hilighting inside a table.
*******************************************************************************/
BOOL WINAPI _export TerSelectCol(HWND hWnd, BOOL repaint)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (cid(CurLine)==0) return FALSE;

    HilightTableCol(w,CurLine,TRUE,FALSE);
    StretchHilight=FALSE;                     // halt any stretching
    HilightWithColCursor=FALSE;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
   TblHilightLeft:
   process the left arrow when stretching hilighting inside a table.
*******************************************************************************/
TblHilightLeft(PTERWND w)
{
   int cell1,cell2,row,FirstCell;
   BOOL SameTable;

   if (CurLine==0 && CurCol==0) return TRUE;


   cell1=cid(HilightBegRow);
   cell2=cid(HilightEndRow);
   if (CurLine!=HilightEndRow) {
      CurLine=HilightEndRow;
      CurCol=LineLen(CurLine)-1;
   }
   WrapFlag=WRAP_OFF;
   PaintFlag=PAINT_MIN;        // minimum painting required
   HilightAtCurPos=FALSE;        // hilight end pos can be on the previous line

   if (cell1==cell2) {         // in the same cell
      if (CurCol>0) {
        CurCol--;
        PaintTer(w);
        return TRUE;
      }
      if (CurLine>0 && cid(CurLine)==cid(CurLine-1)) {
        CurLine--;
        CurCol=LineLen(CurLine)-1;
        if (CurCol<0) CurCol=0;
        PaintTer(w);
        return TRUE;
      }
   }


   // go back to previous cells
   if (CurLine>0) {
     SameTable=InSameTable(w,cell1,cell2);
     if (SameTable) {
        if (cell[cell2].PrevCell>0) {     // goto previous column
           CurLine--;
           while (CurLine>0) {
              if (tabw(CurLine) && tabw(CurLine)->type&INFO_CELL) break;
              CurLine--;
           }
        }
        else {                               // go to the first cell of the previous row
           row=cell[cell2].row;
           row=TableRow[row].PrevRow;
           if (row>0) FirstCell=TableRow[row].FirstCell;   // position at this cell
           else       FirstCell=0;                         // out of table
           CurLine--;

           while (CurLine>0) {
              if (cid(CurLine)==0) break;
              if (cid(CurLine)==FirstCell &&
                    tabw(CurLine) && tabw(CurLine)->type&INFO_CELL) break;
              CurLine--;
           }
        }
     }
     else {                     // not in the same table, go to the previous row
        CurLine--;
        while (CurLine>0) {
           if (cid(CurLine)==0) break;
           if (tabw(CurLine) && tabw(CurLine)->type&INFO_ROW &&
               tabw(CurLine-1) && tabw(CurLine-1)->type&INFO_CELL) {
              CurLine--;
              break;
           }
           CurLine--;
        }
     }
   }

   CurCol=LineLen(CurLine)-1;
   if (CurCol<0) CurCol=0;

   PaintTer(w);

   return TRUE;
}

/******************************************************************************
   TblHilightRight:
   process the right arrow when stretching hilighting inside a table.
*******************************************************************************/
TblHilightRight(PTERWND w,BOOL HilightBegins)
{
   int cell1,cell2,TempCell;
   BOOL SameTable,AtRowEnd;

   cell1=cid(HilightBegRow);
   cell2=cid(HilightEndRow);
   WrapFlag=WRAP_OFF;
   PaintFlag=PAINT_MIN;        // minimum painting required
   HilightAtCurPos=FALSE;        // hilight end pos can be on the previous line

   if (cell1==cell2) {// in the same cell
      if ((CurCol+1)<LineLen(CurLine)) CurCol++;
      else if ((CurLine+1)<TotalLines) {
        CurLine++;
        CurCol=0;
      }
      TempCell=cid(CurLine);
      if (cell[TempCell].row==cell[cell1].row) {
         PaintTer(w);
         return TRUE;
      }
   }


   // syn CurLine and HilightEndRow
   CurLine=HilightEndRow;
   if (HilightEndCol==LineLen(HilightEndRow)) CurLine++;
   CurCol=LineLen(CurLine)-1;
   if (CurCol<0) CurCol=0;

   // advance to next cell
   if ((CurLine+1)<TotalLines) { //>>> RZW 3/1/02 - Prevent CurLine from going beyond TotalLines
     SameTable=InSameTable(w,cell1,cell2);
     if (SameTable) {
       if (cell[cell1].row==cell[cell2].row
           && cell[cell2].NextCell>0) {     // move within the same row
          CurLine++;
          while ((CurLine+1)<TotalLines) {
             if (tabw(CurLine) && tabw(CurLine)->type&INFO_CELL) break;
             CurLine++;
          }
       }
       else {                             // go to the next row
          CurLine++;                      // goto the next cell
          if (tabw(CurLine) && tabw(CurLine)->type&INFO_ROW) CurLine++;  // skip the row row marker
          AtRowEnd=(tabw(CurLine-1) && tabw(CurLine-1)->type&INFO_ROW);

          if (cid(CurLine)==0) {          // out of the table
             CurCol=0;
             PaintTer(w);
             return TRUE;
          }
          else {
             while ((CurLine+1)<TotalLines) {
                if (tabw(CurLine) && tabw(CurLine)->type&INFO_CELL) {  // at cell end
                    if (!AtRowEnd) break; // break at the end of the next cell
                    if (tabw(CurLine+1) && tabw(CurLine+1)->type&INFO_ROW) {
                       CurLine++;
                       break; // at row end
                    }
                }
                CurLine++;
             }
          }
       }
     }
     else {   // not in the same table, go to the next row
        CurLine++;                      // goto the next cell
        if (tabw(CurLine) && tabw(CurLine)->type&INFO_ROW) CurLine++;  // go past the row marker
        if (cid(CurLine)==0) {          // out of the table
           CurCol=0;
           PaintTer(w);
           return TRUE;
        }
        else {   // go to the end of the new row
           while ((CurLine+1)<TotalLines) {
              if (tabw(CurLine) && tabw(CurLine)->type&INFO_CELL &&
                  tabw(CurLine+1) && tabw(CurLine+1)->type&INFO_ROW) break;
              CurLine++;
           }
           CurLine++;
        }
     }
   }

   CurCol=LineLen(CurLine)-1;
   if (CurCol<0) CurCol=0;

   PaintTer(w);
   return TRUE;
}


/******************************************************************************
    InsertCell:
    Insert a cell in the cell chain.  The new cell is inserted before or
    after the CurCell.The can tbe 'B' to insert before or 'A' to insert after the CurCell.
******************************************************************************/
InsertCell(PTERWND w,int NewCell, int CurCell, int CurRowId, BYTE type)
{
    int LastCell,PrevFirst,PrevCell,NextCell;

    if (TableRow[CurRowId].FirstCell<=0 || TableRow[CurRowId].LastCell<=0) {
       TableRow[CurRowId].FirstCell=TableRow[CurRowId].LastCell=NewCell;
    }
    else {
       if (type=='B') {  // insert before the current cell
          if (CurCell==TableRow[CurRowId].FirstCell) {  // insert at the beginning of the row
             TableRow[CurRowId].FirstCell=NewCell;
             cell[NewCell].PrevCell=-1;
             cell[NewCell].NextCell=CurCell;
             cell[CurCell].PrevCell=NewCell;
          }
          else if (CurCell<=0) {  // insert at the end
             LastCell=TableRow[CurRowId].LastCell;
             TableRow[CurRowId].LastCell=NewCell;
             cell[LastCell].NextCell=NewCell;
             cell[NewCell].PrevCell=LastCell;
             cell[NewCell].NextCell=-1;
          }
          else {                  // insert before the CurCell
             PrevCell=cell[CurCell].PrevCell;

             cell[NewCell].PrevCell=PrevCell;
             if (PrevCell>0) cell[PrevCell].NextCell=NewCell;

             cell[NewCell].NextCell=CurCell;
             cell[CurCell].PrevCell=NewCell;
          }
       }
       else if (type=='A') {          // insert after the current cell
          if (CurCell==TableRow[CurRowId].LastCell) { // insert at the end of the chain
             LastCell=TableRow[CurRowId].LastCell;
             TableRow[CurRowId].LastCell=NewCell;
             cell[LastCell].NextCell=NewCell;
             cell[NewCell].PrevCell=LastCell;
             cell[NewCell].NextCell=-1;
          }
          else if (CurCell<=0) {           // insert at the beginning
             PrevFirst=TableRow[CurRowId].FirstCell;
             TableRow[CurRowId].FirstCell=NewCell;
             cell[NewCell].PrevCell=-1;
             cell[NewCell].NextCell=PrevFirst;
             cell[PrevFirst].PrevCell=NewCell;
          }
          else {                      // insert after the CurCell
             NextCell=cell[CurCell].NextCell;

             cell[NewCell].NextCell=NextCell;
             if (NextCell>0) cell[NextCell].PrevCell=NewCell;

             cell[NewCell].PrevCell=CurCell;
             cell[CurCell].NextCell=NewCell;
          }
       }
    }

    cell[NewCell].InUse=TRUE;
    cell[NewCell].row=CurRowId;

    return TRUE;
}

/******************************************************************************
    CopyCell:
    Copy the content of one cell to another
******************************************************************************/
CopyCell(PTERWND w,int src, int dest)
{
    HugeMove(&(cell[src]),&(cell[dest]),sizeof(struct StrCell));
    HugeMove(&(CellAux[src]),&(CellAux[dest]),sizeof(struct StrCellAux));
    return TRUE;
}

/******************************************************************************
    RemoveCell:
    Remove a cell from the cell chain and reset its InUse flag
******************************************************************************/
RemoveCell(PTERWND w,int CurCell)
{
    int CurRowId=cell[CurCell].row;
    int NewFirst,NewLast,PrevCell,NextCell;

    if (CurCell==TableRow[CurRowId].FirstCell && CurCell==TableRow[CurRowId].LastCell)
       TableRow[CurRowId].FirstCell=TableRow[CurRowId].LastCell=-1;  // remove the only cell in the row
    else if (CurCell==TableRow[CurRowId].FirstCell) {
       NewFirst=cell[CurCell].NextCell;
       TableRow[CurRowId].FirstCell=NewFirst;
       if (NewFirst>0) cell[NewFirst].PrevCell=-1;// chain ends here
    }
    else if (CurCell==TableRow[CurRowId].LastCell) {
       NewLast=cell[CurCell].PrevCell;
       TableRow[CurRowId].LastCell=NewLast;
       if (NewLast>0) cell[NewLast].NextCell=-1;  // chain ends here
    }
    else {                                        // remvoe from the middle
       PrevCell=cell[CurCell].PrevCell;
       NextCell=cell[CurCell].NextCell;
       if (PrevCell>0) cell[PrevCell].NextCell=NextCell;
       if (NextCell>0) cell[NextCell].PrevCell=PrevCell;
    }

    cell[CurCell].InUse=FALSE;

    if (FirstFreeCellId==0) FirstFreeCellId=CurCell;
    else if (CurCell<FirstFreeCellId) FirstFreeCellId=CurCell;

    return TRUE;
}

/******************************************************************************
    DelCell:
    Reset the InUse flag.
******************************************************************************/
DelCell(PTERWND w,int CurCell)
{
    cell[CurCell].InUse=FALSE;    // current cell not in use any more

    if (FirstFreeCellId==0) FirstFreeCellId=CurCell;
    else if (CurCell<FirstFreeCellId) FirstFreeCellId=CurCell;

    return TRUE;
}

/******************************************************************************
    RepairTable:
    This routine performs any required table structure repair before the page
    frames are created.
******************************************************************************/
RepairTable(PTERWND w)
{
    long l;
    int  i,PrevCell;
    int  CurCell;
    BOOL InRow;

    if (TotalTableRows==1 && TotalCells==1) return TRUE;  // tables not used

    // reset subtable flag
    for (i=0;i<TotalTableRows;i++) ResetUintFlag(TableRow[i].flags,ROWFLAG_HAS_SUBTABLE);

    // update para ids
    InRow=FALSE;
    for (l=TotalLines-1;l>=0;l--) {
       if (TableLevel(w,l)>0) {   // don't touch the embedded tables
          int ParentCell=cell[cid(l)].ParentCell;
          TableRow[cell[ParentCell].row].flags|=ROWFLAG_HAS_SUBTABLE;
       }

       if (InRow) {
          if (cid(l)==0) InRow=FALSE;
          else if (tabw(l) && tabw(l)->type&(INFO_ROW|INFO_CELL)) CurCell=cid(l);
          else if (cell[cid(l)].level<cell[CurCell].level) CurCell=cid(l);
          else if (cid(l)!=CurCell && InRtfRead) { // the cell should have the same id
              cid(l)=CurCell;
              if (l<RepageBeginLine) RepageBeginLine=l;  // repaginate from this line onward
          }
       }
       else {                        // this line not in a table row
          if (tabw(l) && tabw(l)->type&(INFO_ROW|INFO_CELL)) {
             InRow=TRUE;
             CurCell=cid(l);
          }
          else {                     // reset cell id
             if (cid(l)>0) {
                 cid(l)=0;
                 if (l<RepageBeginLine) RepageBeginLine=l;  // repaginate from this line onward
             }
          }
       }
    }
    

    // recreate row and cells
    for (l=0;l<TotalLines;l++) {
       if (cid(l)>0) l=RepairOneTable(w,l,0);  // returns the last line of the table
    }

    // set RowSpan counter
    for (i=0;i<TotalCells;i++) if (cell[i].InUse) cell[i].RowSpan=1;  // initialize

    for (i=0;i<TotalTableRows;i++) {
       if (!TableRow[i].InUse || TableRow[i].FirstCell<=0) continue;  // does not start a new table
       CurCell=TableRow[i].FirstCell;

       while (CurCell>0) {
          CellAux[CurCell].SpanningCell=0;

          if (cell[CurCell].flags&CFLAG_ROW_SPANNED) {
             int RowSpan=2;
             PrevCell=CurCell;
             while ((PrevCell=GetPrevCellInColumn(w,PrevCell,FALSE,TRUE))>0 && cell[PrevCell].flags&CFLAG_ROW_SPANNED) RowSpan++;  // GetPrevCellInColumnPos function does not work with html input when the row indentation (because of centering) might be different
             if (PrevCell>0) {
                if (RowSpan>cell[PrevCell].RowSpan) cell[PrevCell].RowSpan=RowSpan;
                CellAux[CurCell].SpanningCell=PrevCell;
             }
          }

          CurCell=cell[CurCell].NextCell;
       }
    }

    return TRUE;
}


/******************************************************************************
    RepairOneTable:
    Repair one table and return the last line of the table
******************************************************************************/
long RepairOneTable(PTERWND w, long FirstLine, int level)
{
    long l,LastCellLine;
    int  FirstCell,TempCell,CurRowId,PrevRow,CurCell;
    int  LastCell,PrevCell,TableId;
    
    // recreate row and cells
    FirstCell=CurRowId=PrevRow=-1;

    for (l=FirstLine;l<TotalLines;l++) {
       CurCell=cid(l);
       if (cell[CurCell].level<0) cell[CurCell].level=0;

       if (TableLevel(w,l)>level) {        // do the sub table
           l=RepairOneTable(w,l,level+1);
           continue;
       }

       CurCell=cid(l);
       if (CurCell==0 || cell[CurCell].level<level) {   // table ended, return the last line of the table
          return (l-1);
       }

       // delete the cell with zero width
       if (cell[CurCell].width<=2*cell[CurCell].margin && !HtmlMode) {
          MoveLineArrays(w,l,1,'D');
          cell[CurCell].InUse=FALSE;
          continue;
       }

       // process this row
       FirstCell=CurCell;
       cell[FirstCell].PrevCell=-1;
       cell[FirstCell].NextCell=-1;
       cell[FirstCell].x=0;
       PrevCell=FirstCell;
       TableId=TableRow[cell[FirstCell].row].id;   // table id


       // extract cells for the current row
       LastCellLine=-1;
       for (;l<TotalLines;l++) {
          if (TableLevel(w,l)>level) {        // repair the embedded table
             l=RepairOneTable(w,l,level+1);
             continue;
          }
          TempCell=cid(l);


          // row break
          if (tabw(l) && tabw(l)->type&INFO_ROW) {      // row break;
             //if (TempCell>0 && cell[TempCell].width<=2*cell[TempCell].margin) cell[TempCell].InUse=FALSE;
             if ((TempCell>0 && !cell[TempCell].InUse)) cid(l)=CurCell;
             if (InRtfRead && TempCell!=CurCell) cid(l)=CurCell;
             break;
          }

          // discard cells of zero width
          if (TempCell>0 && cell[TempCell].width<=2*cell[TempCell].margin && !HtmlMode) {
             int margin=cell[TempCell].width/4;  // leave some space for cell text
             if (margin<10) {                    // near zero width
                MoveLineArrays(w,l,1,'D');
                cell[TempCell].InUse=FALSE;
                l--;
                continue;
             }
             else cell[TempCell].margin=margin;
          }


          if (tabw(l) && tabw(l)->type&INFO_CELL) LastCellLine=l;

          CurCell=cid(l);
          if (CurCell==0) break;

          // reset the save column cell
          CellAux[CurCell].PrevColCell=CellAux[CurCell].NextColCell=0;  // need to recalculate

          // cell changed, add to the chain
          if (CurCell!=PrevCell) {
             cell[PrevCell].NextCell=CurCell;
             cell[CurCell].PrevCell=PrevCell;
             cell[CurCell].NextCell=-1;
             cell[CurCell].x=cell[PrevCell].x+cell[PrevCell].width;
             PrevCell=CurCell;
          }
       }


       // Supply the row marker if missing
       if (l==TotalLines || CurCell==0 || !tabw(l) || !(tabw(l)->type&INFO_ROW)) {
          if (CheckLineLimit(w,TotalLines+1)) {
             // create the row break line
             InsertMarkerLine(w,l,ROW_CHAR,DEFAULT_CFMT,0,INFO_ROW,cid(l-1));
             if (l<RepageBeginLine) RepageBeginLine=l;  // repaginate from this line onward
          }
       }

       // delete the lines between the last cell and row break line
       if (LastCellLine>=0 && (LastCellLine+1)<l) {
           MoveLineArrays(w,LastCellLine+1,l-LastCellLine-1,'D');
           l=LastCellLine+1;
       }

       // supply a cell marker if missing before the row break line
       if (tabw(l)->type&INFO_ROW && l>0 && LevelCell(w,level,l)!=LevelCell(w,level,l-1)) {
          if (CheckLineLimit(w,TotalLines+1)) {
             // create a cell break line
             InsertMarkerLine(w,l,CellChar,DEFAULT_CFMT,-1,INFO_CELL,-1);
             pfmt(l)=pfmt(l+1);              // same as the row line cell number
             cid(l)=cid(l+1);              // same as the row line cell number

             if (l<RepageBeginLine) RepageBeginLine=l;  // repaginate from this line onward
             l++;                            // new row line number
          }
       }



       // Associate the row id with each cell in the row
       LastCell=PrevCell;
       CurRowId=cell[LastCell].row;
       if (TableRow[CurRowId].flags&ROWFLAG_NEW_TABLE && HtmlMode) PrevRow=-1;  // force a new table

       TableRow[CurRowId].FirstCell=FirstCell;
       TableRow[CurRowId].LastCell=0;
       TableRow[CurRowId].PrevRow=PrevRow;    // is it the first row of the table
       TableRow[CurRowId].NextRow=-1;         // will be set when the next row is created
       TableRow[CurRowId].id=TableId;

       if (PrevRow>=0) TableRow[PrevRow].NextRow=CurRowId;
       PrevRow=CurRowId;


       CurCell=FirstCell;
       while (CurCell>0) {
          cell[CurCell].row=CurRowId;
          TableRow[CurRowId].LastCell=CurCell;

          CurCell=cell[CurCell].NextCell;
       }
    }

    return (l-1);
}
    
/******************************************************************************
    AdjustBlockForTable:
    Adjust the block beginning/ending to enclose table columns. This function
    also adjusts the block for hyperlink text.
******************************************************************************/
AdjustBlockForTable(PTERWND w,LPLONG pBegRow,LPINT pBegCol,LPLONG pEndRow,LPINT pEndCol, BOOL AdjustCurPos)
{
    long line1=(*pBegRow);
    long line2=(*pEndRow);
    int  col1=(*pBegCol),min1,min2,max1,max2;
    int  col2=(*pEndCol);
    int  cell1,cell2,row,level;
    BOOL SameTable=TRUE,HlForward=TRUE,AdjustBegin,AdjustEnd,BlockModified=FALSE;
    int  TblCol1,TblCol2,DestCol,col,DestCell;
    int  CurCfmt1,CurCfmt2;


    if (HilightType!=HILIGHT_CHAR) return FALSE;

    if (tabw(line1) && tabw(line1)->type&INFO_ROW && line1>0) {
       line1--;
       col1=LineLen(line1);
    }
    if (tabw(line2) && tabw(line2)->type&INFO_ROW && line2>0) {
       line2--;
       col2=LineLen(line2);
    }

    // adjust the hyperlink selection
    if (True(TerFlags&TFLAG_SELECT_FULL_HLINK)) {
      CurCfmt1=GetCurCfmt(w,line1,col1);
      CurCfmt2=GetCurCfmt(w,line2,col2);
      AdjustBegin=(IsHypertext(w,CurCfmt1)/* || TerFont[CurCfmt1].style&HIDDEN*/);   // 20050923
      AdjustEnd=(IsHypertext(w,CurCfmt2)/* || TerFont[CurCfmt2].style&HIDDEN*/);
      
      if (CursDirection==CURS_FORWARD) {
         if (AdjustBegin)  GetHypertextStart(w,&line1,&col1);
         if (AdjustEnd) {
            GetHypertextEnd(w,&line2,&col2);
            CurLine=line2;
            CurCol=col2;
         }
      }
      else if (CursDirection==CURS_BACKWARD) {
         if (AdjustBegin)    GetHypertextEnd(w,&line1,&col1);
         if (AdjustEnd) {
            GetHypertextStart(w,&line2,&col2);
            CurLine=line2;
            CurCol=col2;
         }
      }
    }

    // make the block forword
    if (line1>line2) {
       SwapLongs(&line1,&line2);
       SwapInts(&col1,&col2);
       HlForward=FALSE;
    }
    if (line1==line2 && col1>col2) {
       SwapInts(&col1,&col2);
       HlForward=FALSE;
    }

    // adjust the hyperlink selection
    if (false && True(TerFlags&TFLAG_SELECT_FULL_HLINK)) {
       BOOL CursForward=HlForward;  // GetCursDirection(w);   // get the cursor direction
       CurCfmt1=GetCurCfmt(w,line1,col1);
       CurCfmt2=GetCurCfmt(w,line2,col2);
       AdjustBegin=(IsHypertext(w,CurCfmt1) || TerFont[CurCfmt1].style&HIDDEN);
       AdjustEnd=(IsHypertext(w,CurCfmt2) || TerFont[CurCfmt2].style&HIDDEN);

       // check if select_full_hlink already applied on the right side - this is needed because the previous application would happen move the cursor past the current hlink to cover the entire hlink
       if (col2>0 && AdjustEnd) {
          int TempFont=GetCurCfmt(w,line2,col2-1); // check the previous field
          if (!IsSameField(w,CurCfmt2,TempFont)) AdjustEnd=false;  // the hyperlink already covered entirely on the right side
       } 

       if (AdjustBegin || AdjustEnd) {
          if (HlForward) {              // forward highlighting
            if (AdjustBegin) GetHypertextStart(w,&line1,&col1);
            if (CursForward && AdjustEnd)   GetHypertextEnd(w,&line2,&col2);
            if (!CursForward && AdjustEnd)   GetHypertextStart(w,&line2,&col2);
            CurLine=line2;
            CurCol=col2;
          }
          else {                        // backward highlighting
            if (AdjustEnd) GetHypertextEnd(w,&line2,&col2);
            if (CursForward && AdjustBegin)   GetHypertextEnd(w,&line1,&col1);
            if (!CursForward && AdjustBegin)   GetHypertextStart(w,&line1,&col1);
            CurLine=line1;
            CurCol=col1;
          }
          if (CurCol>=LineLen(CurLine) && (CurLine+1)<TotalLines) {
             CurLine++;
             CurCol=0;
          }
       }
    }

    if (!TerArg.PageMode) goto END;   // don't do table related adjustment

    // check if block limited to one cell
    level=MinTableLevel(w,line1,line2);   // get the minimum table level used by this line range
    cell1=LevelCell(w,level,line1);
    cell2=LevelCell(w,level,line2);
    if (cell1==cell2 && cell1!=0) {       // within the same cell, check if the last character of the cell is selected
       if (TableLevel(w,line2)==level && LineInfo(w,line2,INFO_CELL) && col2==LineLen(line2)) {  // cell character selected
           while(LevelCell(w,level,line1)==cell1 && line1>=0) line1--;
           line1++;            // start the selection from the beginning of the cell
           col1=0;
       }
       goto END; 
    }
        
    SameTable=InSameTable(w,cell1,cell2);

    if (SameTable) {                         // get the column
       min1=GetCellColumn(w,cell1,TRUE);
       max1=min1+cell[cell1].ColSpan-1;
       min2=GetCellColumn(w,cell2,TRUE);
       max2=min2+cell[cell2].ColSpan-1;
       TblCol1=(min1<min2)?min1:min2;
       TblCol2=(max1>max2)?max1:max2;
    }


    // adjust the beginnig point
    if (cell1>0) {
       if (SameTable) DestCol=TblCol1; // start the block from the first colum of the block
       else           DestCol=0;       // start the block from the first column of the table
       col=GetCellColumn(w,cell1,TRUE);
       // go back a number of columns
       while (col>DestCol && line1>0) {
           if (cid(line1-1)==0) break;  // out of table - shouldn't happen
           if (LineInfo(w,line1-1,INFO_ROW) && level==TableLevel(w,line1-1)) break;  // shouldn't happen
           line1--;
           if (LineInfo(w,line1,INFO_CELL) && level==TableLevel(w,line1)) col-=cell[cid(line1)].ColSpan;;
       }
       // position at the beginning of the cell
       DestCell=LevelCell(w,level,line1);
       while (line1>0) {
          if (LevelCell(w,level,line1-1)!=DestCell) break;
          line1--;
       }
       col1=0;
    }

    // adjust the ending point
    if (cell2>0) {
       col=GetCellColumn(w,cell2,TRUE)+cell[cell2].ColSpan-1;

       if (SameTable) DestCol=TblCol2; // end the block at the last colum of the block
       else  {
          DestCol=9999;    // end the block at the last column of the table
          if (HilightWithColCursor) { // position at the last row
             while ((line2+1)<TotalLines) {
               cell2=LevelCell(w,level,line2);
               row=cell[cell2].row; // position at the last row
               if (TableRow[row].NextRow<=0) break;
               line2++;
             }
             col=0;                  // now positioned at the first column
          }
       }

       // go forward number of columns
       while (col<DestCol && (line2+1)<TotalLines) {
           if (cid(line2+1)==0) break;  // out of table - shouldn't happen
           if (LineInfo(w,line2+1,INFO_ROW) && TableLevel(w,line2+1)==level) break; // last column of the table
           if (LineInfo(w,line2,INFO_CELL) && TableLevel(w,line2)==level) col+=cell[cid(line2)].ColSpan;
           line2++;
       }
       // position at the end of the cell
       DestCell=LevelCell(w,level,line2);
       while ((line2+1)<TotalLines) {
          if (LineInfo(w,line2,INFO_CELL) && TableLevel(w,line2)==level) break;  // last line of the cell
          if (LevelCell(w,level,line2+1)!=DestCell) break;
          line2++;
       }
       col2=LineLen(line2)/*-1*/;   // include the cell marker
       if (col2<0) col2=0;
    }

    END:
    // include table row line when more than one cell selected
    if (!SameTable && cid(line2)>0 && (line2+1)<TotalLines && LineInfo(w,line2+1,INFO_ROW)) {
        line2++;
        col2=1;     // include the row marker
    }


    // restore the block direction
    if (!HlForward) {
       SwapLongs(&line1,&line2);
       SwapInts(&col1,&col2);
    }

    BlockModified=((*pBegRow)!=line1 || (*pBegCol)!=col1 || (*pEndRow)!=line2 || (*pEndCol)!=col2);

    (*pBegRow)=line1;
    (*pBegCol)=col1;
    (*pEndRow)=line2;
    (*pEndCol)=col2;

    if (StretchHilight && AdjustCurPos) {
       CurLine=(*pEndRow);
       CurCol=(*pEndCol);
       if (CurCol>=LineLen(CurLine) && (CurLine+1)<TotalLines) {
          CurLine++;
          CurCol=0;
       }
       CurRow=CurLine-BeginLine;
       if (CurRow<0) CurRow=0;
       BeginLine=CurLine-CurRow;
    }

    return BlockModified;
}

/******************************************************************************
    InSameTable:
    This function returns TRUE if the two cells belong to the same table.
******************************************************************************/
BOOL InSameTable(PTERWND w, int cell1, int cell2)
{
    int row1,row2;

    if (cell1==0 || cell2==0) return FALSE;

    row1=cell[cell1].row;
    while (!IsFirstTableRow(w,row1)) row1=TableRow[row1].PrevRow;

    row2=cell[cell2].row;
    while (!IsFirstTableRow(w,row2)) row2=TableRow[row2].PrevRow;

    return (row1==row2);
}

/******************************************************************************
    IsFirstTableRow:
    Returns TRUE if the given row is the first table row.
******************************************************************************/
BOOL IsFirstTableRow(PTERWND w, int row)
{
    if (TableRow[row].PrevRow<=0) return TRUE;
    if (HtmlMode && TableRow[row].flags&(ROWFLAG_NEW_TABLE|ROWFLAG_BREAK)) return TRUE;
    return FALSE;
}

/******************************************************************************
    IsLastTableRow:
    Returns TRUE if the given row is the last table row.
******************************************************************************/
BOOL IsLastTableRow(PTERWND w, int row)
{
    int NextRow;

    if (TableRow[row].NextRow<=0) return TRUE;
    if (HtmlMode) {
        NextRow=TableRow[row].NextRow;
        if (NextRow>0 && TableRow[NextRow].flags&(ROWFLAG_NEW_TABLE|ROWFLAG_BREAK)) return TRUE;
    }

    return FALSE;
}

/******************************************************************************
    TableHilighted:
    This function returns TRUE if the entire hilighted block lies within
    a table.
******************************************************************************/
BOOL TableHilighted(PTERWND w)
{
    int cell1,cell2,LastCol,level,LastCell;

    if (!TerArg.PageMode) return FALSE;

    // check if block limited to one cell
    level=MinTableLevel(w,HilightBegRow,HilightEndRow);
    cell1=LevelCell(w,level,HilightBegRow);
    cell2=LevelCell(w,level,HilightEndRow);
    if (cell1==0 && cell2==0) return FALSE;

    if (cell1==cell2) {         // check if the entire cell highlighted
       if (TerOpFlags&TOFLAG_SEL_CELL_CONTENT && HilightEndCol==LineLen(HilightEndRow) && HilightEndCol>0) HilightEndCol--;  // do not delete the cell marker
       LastCol=LineLen(HilightEndRow)-1;
       if (TerFlags&TFLAG_NO_AUTO_FULL_CELL_SEL) LastCol++;

       LastCell=cid(HilightEndRow);

       return ((HilightBegRow==0 || LevelCell(w,level,HilightBegRow-1)!=cell1)
               && HilightBegCol==0
               && LineInfo(w,HilightEndRow,(INFO_CELL|INFO_ROW))
               && cell[LastCell].level==level
               && HilightEndCol>=LastCol);
    }

    return InSameTable(w,cell1,cell2);
}

/******************************************************************************
    GetPrevCellInColumn:
    Get the prev cell in the specified column of the table.  The function
    returns -1 when no more cells are found.
******************************************************************************/
int GetPrevCellInColumn(PTERWND w, int CurCell,BOOL exact,BOOL UseIndex)
{
    int i,col,row,PrevRow,NextCell,delta1,delta2;

    if (CurCell==-1) return -1;

    // current column number and row id
    col=GetCellColumn(w,CurCell,TRUE);
    row=cell[CurCell].row;

    PrevRow=TableRow[row].PrevRow;

    if (PrevRow==-1) return -1;       // first row of the table reached

    // find the cell at the required column postion
    NextCell=TableRow[PrevRow].FirstCell;
    for (i=(cell[NextCell].ColSpan-1);i<col;i++) {
       if (cell[NextCell].NextCell<=0) break;  // last cell of the table reached
       NextCell=cell[NextCell].NextCell;
       i+=(cell[NextCell].ColSpan-1);
    }

    if (UseIndex) return NextCell;  // just go by column index

    if (!exact) {     // just make sure that the previous column cell atleast covers the top border of the current cell
       if (cell[NextCell].x<=cell[CurCell].x &&
         ((cell[NextCell].x+cell[NextCell].width)>=(cell[CurCell].x+cell[CurCell].width)))
         return NextCell;
       else return -1;
    }

    delta1=abs(cell[NextCell].x-cell[CurCell].x);
    delta2=abs(cell[NextCell].width-cell[CurCell].width);

    if (delta1<RULER_TOLERANCE && delta2<RULER_TOLERANCE) return NextCell;  // cell found
    else                                                  return -1;
}

/******************************************************************************
    GetNextCellInColumn:
    Get the next cell in the specified column of the table.  The function
    returns -1 when no more cells are found.
******************************************************************************/
int GetNextCellInColumn(PTERWND w, int CurCell, BOOL exact)
{
    int i,col,row,NextRow,NextCell,delta1,delta2;

    if (CurCell==-1) return -1;

    // current column number and row id
    col=GetCellColumn(w,CurCell,FALSE);
    row=cell[CurCell].row;

    NextRow=TableRow[row].NextRow;

    if (NextRow==-1) return -1;       // first row of the table reached

    // find the cell at the required column postion
    NextCell=TableRow[NextRow].FirstCell;
    for (i=0;i<col;i++) {
       if (cell[NextCell].NextCell==-1) break;  // last cell of the table reached
       i+=(cell[NextCell].ColSpan-1);
       NextCell=cell[NextCell].NextCell;
    }

    if (!exact) return NextCell;

    delta1=abs(cell[NextCell].x-cell[CurCell].x);
    delta2=abs(cell[NextCell].width-cell[CurCell].width);

    if (delta1<RULER_TOLERANCE && delta2<RULER_TOLERANCE) return NextCell;  // cell found
    else                                                  return -1;

}

/******************************************************************************
    GetPrevCellInColumnPos:
    Get the prev cell in the specified column of the table.  The function
    returns -1 when no more cells are found.
    This function is similar to GetPrevCellInColumn except that this funcion
    uses the cell right border position for match instead of the cell number.
******************************************************************************/
int GetPrevCellInColumnPos(PTERWND w, int CurCell,BOOL exact)
{
    int BorderX,PrevRow,NextCell,delta;

    if (CurCell==-1) return -1;

    // current column number and row id
    BorderX=GetCellRightX(w,CurCell);
    PrevRow=cell[CurCell].row;

    while (TRUE) {
       PrevRow=TableRow[PrevRow].PrevRow;
       if (PrevRow==-1) return -1;       // first row of the table reached

       // find the cell at the required column postion
       NextCell=TableRow[PrevRow].FirstCell;
       while (TRUE) {
          delta=GetCellRightX(w,NextCell)-BorderX;
          if (abs(delta)<RULER_TOLERANCE) return NextCell;
          if (cell[NextCell].NextCell==-1) break;  // last cell of the table reached
          NextCell=cell[NextCell].NextCell;
       }
    }
}

/******************************************************************************
    GetNextCellInColumnPos:
    Get the next cell in the specified column of the table.  The function
    returns -1 when no more cells are found.
    This function is similar to GetNextCellInColumn except that this funcion
    uses the cell right border position for match instead of the cell number.
******************************************************************************/
int GetNextCellInColumnPos(PTERWND w, int CurCell)
{
    int BorderX,NextRow,NextCell,delta;

    if (CurCell==-1) return -1;

    // current column number and row id
    BorderX=GetCellRightX(w,CurCell);
    NextRow=cell[CurCell].row;

    while (TRUE) {
       NextRow=TableRow[NextRow].NextRow;
       if (NextRow==-1) return -1;       // first row of the table reached

       // find the cell at the required column postion
       NextCell=TableRow[NextRow].FirstCell;
       while (TRUE) {
          delta=GetCellRightX(w,NextCell)-BorderX;
          if (abs(delta)<RULER_TOLERANCE) return NextCell;
          if (cell[NextCell].NextCell==-1) break;  // last cell of the table reached
          NextCell=cell[NextCell].NextCell;
       }
    }
}

/******************************************************************************
    GetColumnCell:
    Find the cell id at the specified column number (0 based) for a given row.
    When 'UseColSpan' is TRUE the ColSpan is taken into consideration
******************************************************************************/
int GetColumnCell(PTERWND w, int row,int col,BOOL UseColSpan)
{
    int CurCell;

    if (row<0 || row>=TotalTableRows || !TableRow[row].InUse) return 0;
    CurCell=TableRow[row].FirstCell;

    while (col>0 && cell[CurCell].NextCell>0) {
      if (UseColSpan) col-=cell[CurCell].ColSpan;
      else            col--;
      if (col<0) break;
      CurCell=cell[CurCell].NextCell;
    }

    return CurCell;
}

/******************************************************************************
    GetCellColumn:
    Find the column number (0 based) for a given cell id. When 'UseColSpan' is
    TRUE the ColSpan is taken into consideration
******************************************************************************/
int GetCellColumn(PTERWND w, int CurCell,BOOL UseColSpan)
{
    int row;                           // table row id
    int col=0;
    int NextCell;

    if (CurCell<0 || CurCell>=TotalCells || !(cell[CurCell].InUse)) return 0;

    row=cell[CurCell].row;             // current table row

    // find our cell in the row
    NextCell=TableRow[row].FirstCell; // first cell the row

    while (TRUE) {
       if (NextCell==-1 || NextCell==0) break;  // our cell not found
       if (NextCell==CurCell) return col; // column number found
       if (UseColSpan) col+=cell[NextCell].ColSpan;
       else            col++;
       NextCell=cell[NextCell].NextCell;
    }


    return 0;
}

/******************************************************************************
    GetCellRightX:
    Find the cell right border x position in twips.
******************************************************************************/
int GetCellRightX(PTERWND w, int CurCell)
{
    int row;                           // table row id
    int x;
    int NextCell;

    if (CurCell<0 || CurCell>=TotalCells || !(cell[CurCell].InUse)) return 0;

    row=cell[CurCell].row;             // current table row

    // find our cell in the row
    x=TableRow[row].indent;
    NextCell=TableRow[row].FirstCell; // first cell the row

    while (TRUE) {
       if (NextCell==-1 || NextCell==0) break;  // our cell not found
       x+=cell[NextCell].width;
       if (NextCell==CurCell) break;            // column number found
       NextCell=cell[NextCell].NextCell;
    }

    return x;
}

/******************************************************************************
    GetRowCell:
    Find the cell id at a specified column location in the specified row.
    This function returns -1 when unsuccessful.
******************************************************************************/
int GetRowCell(PTERWND w, int row, int col)
{
    int NextCell;

    if (row<0 || row>=TotalTableRows || !(TableRow[row].InUse)) return -1;

    NextCell=TableRow[row].FirstCell;

    while (col>0) {
       NextCell=cell[NextCell].NextCell;
       if (NextCell==-1) break;
       col--;
    }

    return NextCell;
}


/******************************************************************************
    TerHtmlCellWidthFlag:
    Set the html table cell width flag. This function is used to set the
    flag to 0, or one of the following: CFLAG_FIX_WIDTH, CFLAG_FIX_WIDTH_PCT
******************************************************************************/
BOOL WINAPI _export TerHtmlCellWidthFlag(HWND hWnd, int select, UINT flag, BOOL repaint)
{
    int cl,CurCell;
    PTERWND w;
    BOOL TableHilighted=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data


    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TableHilighted=TRUE;
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && !TableHilighted)) return TRUE;

    if (select<=0) {
       DlgInt1=cell[CurCell].flags;
       select=CallDialogBox(w,"CellWidthFlagParam",CellWidthFlagParam,0L);
       flag=DlgInt1;
    }
    else {
       if      (select==SEL_CELLS) select=IDC_TABLE_SEL_CELLS;
       else if (select==SEL_COLS)  select=IDC_TABLE_SEL_COLS;
       else if (select==SEL_ROWS)  select=IDC_TABLE_SEL_ROWS;
       else if (select==SEL_ALL)   select=IDC_TABLE_ALL;
       else select=0;
    }

    if (select==0) return TRUE;
    if (flag!=0 && flag!=CFLAG_FIX_WIDTH && flag!=CFLAG_FIX_WIDTH_PCT) return FALSE;

    // mark the selected cells
    if (!MarkCells(w,select)) return FALSE;
    HilightType=HILIGHT_OFF;     // remove highlighting so shading can be seen

    //  Set the shading for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       cell[cl].flags=ResetUintFlag(cell[cl].flags,(CFLAG_AUTO_WIDTH|CFLAG_FIX_WIDTH|CFLAG_FIX_WIDTH_PCT));
       cell[cl].flags|=flag;
       if (flag&CFLAG_FIX_WIDTH_PCT) cell[cl].flags|=CFLAG_AUTO_WIDTH;

       if (cell[cl].flags&CFLAG_FIX_WIDTH) cell[cl].FixWidth=cell[cl].width;
       else                                cell[cl].FixWidth=0;
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    GetSameColumnCell:
    Get the cell from the next or previous row in the same column. If the columns
    for the table rows are not identical or if the next/prev row does not
    exist, then this function return 0.
******************************************************************************/
GetSameColumnCell(PTERWND w, int CellId, BOOL next)
{
    int TempCell,SaveInitCell=CellId;
    int row,OtherRow,cell1,x1,x2,NextX1,NextX2;
    // int cell2;


    // check if this calculation is already available
    if (RepageBeginLine>=TotalLines) {
       TempCell=next?CellAux[CellId].NextColCell:CellAux[CellId].PrevColCell;
       if (TempCell>0 && TempCell<TotalCells && cell[TempCell].InUse) return TempCell;
    }


    // check if this row and the next row has the same column widths
    NEXT_ROW:
    row=cell[CellId].row;
    if (next) OtherRow=TableRow[row].NextRow;
    else      OtherRow=TableRow[row].PrevRow;

    if (OtherRow<=0) return 0;

    x1=cell[CellId].x;
    x2=x1+cell[CellId].width;

    cell1=TableRow[OtherRow].FirstCell;
    while (cell1>0) {
       NextX1=cell[cell1].x;
       NextX2=NextX1+cell[cell1].width;

       if (abs(x1-NextX1)<=RULER_TOLERANCE && abs(x2-NextX2)<=RULER_TOLERANCE) {
          if (next && cell[cell1].flags&CFLAG_ROW_SPANNED) {
             CellId=cell1;
             goto NEXT_ROW;
          }
          if (next) CellAux[SaveInitCell].NextColCell=cell1;
          else      CellAux[SaveInitCell].PrevColCell=cell1;
          return cell1;
       }

       cell1=cell[cell1].NextCell;
    }


    //if (OtherRow>0 && TableRow[row].indent==TableRow[OtherRow].indent
    //   && (TableRow[row].flags&(RIGHT_JUSTIFY|CENTER))==(TableRow[OtherRow].flags&(RIGHT_JUSTIFY|CENTER))) {
    //
    //   cell1=TableRow[row].FirstCell;
    //   cell2=TableRow[OtherRow].FirstCell;
    //   TempCell=0;
    //   x1=x2=0;
    //   while (cell1>0 && cell2>0) {
    //      x1+=cell[cell1].width;
    //      x2+=cell[cell2].width;
    //      if (abs(x1-x2)>RULER_TOLERANCE) break;
    //      if (cell1==CellId) TempCell=cell2;
    //
    //      cell1=cell[cell1].NextCell;
    //      cell2=cell[cell2].NextCell;
    //   }
    //   if (cell1<=0 && cell2<=0 && TempCell>0) {
    //      if (next && cell[TempCell].flags&CFLAG_ROW_SPANNED) {
    //         CellId=TempCell;
    //         goto NEXT_ROW;
    //      }
    //
    //      // record this cell for the next call
    //      if (next) CellAux[SaveInitCell].NextColCell=TempCell;
    //      else      CellAux[SaveInitCell].PrevColCell=TempCell;
    //
    //      return TempCell;
    //   }
    //}

    return 0;
}

/******************************************************************************
    UniformRowBorderCell:
    Returns the first cell of the row which has uniform top or bottom border.
    Otherwise it returns 0.
******************************************************************************/
UniformRowBorderCell(PTERWND w, int CellId, BOOL next)
{
    int TempCell,OtherRow;
    int row,width=-1,CurWidth;

    row=cell[CellId].row;

    if (next) OtherRow=TableRow[row].NextRow;
    else  {
       if ((OtherRow=TableRow[row].PrevRow)<=0) return 0;
       TempCell=TableRow[OtherRow].FirstCell;
       while (OtherRow>0 && cell[TempCell].flags&CFLAG_ROW_SPANNED) {  // position at first row that has first cell which is non-spanned
          if ((OtherRow=TableRow[OtherRow].PrevRow)<=0) return 0;
          TempCell=TableRow[OtherRow].FirstCell;
       }
    }
    if (OtherRow<=0) return 0;

    if (OtherRow>0 && TableRow[row].indent!=TableRow[OtherRow].indent) return 0;
    if ((TableRow[row].flags&(RIGHT_JUSTIFY|CENTER))!=(TableRow[OtherRow].flags&(RIGHT_JUSTIFY|CENTER))) return 0;
    if (abs(GetRowWidth(w,row)-GetRowWidth(w,OtherRow))>RULER_TOLERANCE) return 0;


    TempCell=TableRow[OtherRow].FirstCell;
    while (TempCell>0) {
       CurWidth=0;
       if (next && cell[TempCell].border&BORDER_TOP)  CurWidth=cell[TempCell].BorderWidth[BORDER_INDEX_TOP];
       if (!next && cell[TempCell].border&BORDER_BOT) CurWidth=cell[TempCell].BorderWidth[BORDER_INDEX_BOT];

       if (width==-1) width=CurWidth;
       if (width!=CurWidth) return 0;

       TempCell=cell[TempCell].NextCell;
    }

    return TableRow[OtherRow].FirstCell;
}

/******************************************************************************
    GetRowWidth:
    The width of the given table row in twip.s
******************************************************************************/
GetRowWidth(PTERWND w, int row)
{
    int width=0,cl;

    if (row<=0 || row>=TotalTableRows) return 0;

    cl=TableRow[row].FirstCell;

    while (cl>0) {
       width+=cell[cl].width;
       cl=cell[cl].NextCell;
    }

    return width;
}

/******************************************************************************
    GetTableRowSlot:
    get the table row slot from the slot table.  This function returns -1 if
    a table row slot not found.
******************************************************************************/
GetTableRowSlot(PTERWND w)
{
    int i;
    DWORD OldSize,NewSize;
    int   NewMaxTableRows;
    HGLOBAL hMem;
    BYTE huge * pMem;

    if (InRtfRead && TotalTableRows>500 && (TotalTableRows+1)<MaxTableRows) {
       i=TotalTableRows;
       TotalTableRows++;
       FarMemSet(&TableRow[i],0,sizeof(struct StrTableRow));
       FarMemSet(&TableAux[i],0,sizeof(struct StrTableAux));
       return i;
    } 
    
    // find an open slot
    for (i=1;i<TotalTableRows;i++) {
       if (!TableRow[i].InUse) {
          FarMemSet(&TableRow[i],0,sizeof(struct StrTableRow));
          FarMemSet(&TableAux[i],0,sizeof(struct StrTableAux));
          return i;
       }
    }
    if (!InRtfRead && !(TerFlags3&TFLAG3_HTML_READ) && !(TerOpFlags&TOFLAG_NO_RECOVE_ID)) {
       RecoverCellSlots(w);               // recover unused entries from the cell table
       RecoverTableRowSlots(w);           // recover unused entries from the table row array
    }

    // again find an open slot
    for (i=1;i<TotalTableRows;i++) {
       if (!TableRow[i].InUse) {
          FarMemSet(&TableRow[i],0,sizeof(struct StrTableRow));
          FarMemSet(&TableAux[i],0,sizeof(struct StrTableAux));
          return i;
       }
    }

    // use the next slot
    if (TotalTableRows>=MaxTableRows) {        // allocate more memory for the TableRow table
       NewMaxTableRows=MaxTableRows+MaxTableRows/2;

       OldSize=((DWORD)MaxTableRows+1)*sizeof(struct StrTableRow);
       NewSize=((DWORD)NewMaxTableRows+1)*sizeof(struct StrTableRow);

       if ( (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,NewSize)))
          && (NULL!=(pMem=GlobalLock(hMem))) ){

          // move data to new location
          HugeMove(TableRow,pMem,OldSize);

          // free old handle
          GlobalUnlock(hTableRow);
          GlobalFree(hTableRow);

          // assign new handle
          hTableRow=hMem;              // new array
          TableRow=(struct StrTableRow huge *)pMem;

          // allocate the TableRowAux array
          OldSize=((DWORD)MaxTableRows+1)*sizeof(struct StrTableAux);
          NewSize=((DWORD)NewMaxTableRows+1)*sizeof(struct StrTableAux);
          pMem=OurAlloc(NewSize);
          HugeMove(TableAux,pMem,OldSize);
          OurFree(TableAux);
          TableAux=(struct StrTableAux huge *)pMem;

          MaxTableRows=NewMaxTableRows; // new max table rows
       }
       else return -1;
    }

    TotalTableRows++;
    FarMemSet(&TableRow[TotalTableRows-1],0,sizeof(struct StrTableRow));
    FarMemSet(&TableAux[TotalTableRows-1],0,sizeof(struct StrTableAux));

    return (TotalTableRows-1);

}

/******************************************************************************
    GetCellSlot:
    get the table cell slot from the slot table.  This function returns -1 if
    a table cell slot not found.
******************************************************************************/
GetCellSlot(PTERWND w,BOOL recover)
{
    int i,first;
    DWORD OldSize, NewSize;
    int   NewMaxCells;
    HGLOBAL hMem;
    BYTE huge *pMem;

    if (InRtfRead && TotalCells>1000 && (TotalCells+1)<MaxCells) {  // simply use the last cell id for efficiency
       TotalCells++;
       InitCell(w,TotalCells-1);
       return (TotalCells-1);
    } 
    
    // find an open slot
    if (TerFlags&TFLAG_USE_NEXT_ID) {
       TerFlags=ResetLongFlag(TerFlags,TFLAG_USE_NEXT_ID);
       recover=FALSE;                  // do not recover
    }
    else {                             // look for an open slot
       if (!recover && FirstFreeCellId>0) first=FirstFreeCellId;
       else                               first=1;
       for (i=first;i<TotalCells;i++) {
          if (!cell[i].InUse) {
             InitCell(w,i);
             FirstFreeCellId=i;
             return i;
          }
       }
    }

    if (recover) {
       RecoverCellSlots(w);               // recover unused entries from the cell table

       // again find an open slot
       for (i=1;i<TotalCells;i++) {
          if (!cell[i].InUse) {
             InitCell(w,i);
             FirstFreeCellId=i;
             return i;
          }
       }
    }

    // use the next slot
    if (TotalCells>=MaxCells) {        // allocate more memory for the cell table
       NewMaxCells=MaxCells+MaxCells/2;

       OldSize=((DWORD)MaxCells+1)*sizeof(struct StrCell);
       NewSize=((DWORD)NewMaxCells+1)*sizeof(struct StrCell);

       if ( (NULL!=(hMem=GlobalAlloc(GMEM_MOVEABLE,NewSize)))
          && (NULL!=(pMem=GlobalLock(hMem))) ){

          // move data to new location
          HugeMove(cell,pMem,OldSize);

          // free old handle
          GlobalUnlock(hCell);
          GlobalFree(hCell);

          // assign new handle
          hCell=hMem;              // new array
          cell=(struct StrCell huge *)pMem;

          // allocate the CellAux array
          OldSize=((DWORD)MaxCells+1)*sizeof(struct StrCellAux);
          NewSize=((DWORD)NewMaxCells+1)*sizeof(struct StrCellAux);
          pMem=OurAlloc(NewSize);
          HugeMove(CellAux,pMem,OldSize);
          OurFree(CellAux);
          CellAux=(struct StrCellAux huge *)pMem;

          MaxCells=NewMaxCells;    // new max cells
       }
       else return -1;
    }

    TotalCells++;
    InitCell(w,TotalCells-1);
    FirstFreeCellId=TotalCells-1;

    return (TotalCells-1);

}

/******************************************************************************
    InitCell:
    Initialize a new cell.
******************************************************************************/
InitCell(PTERWND w, int CurCell)
{
    int i;

    FarMemSet(&cell[CurCell],0,sizeof(struct StrCell));
    FarMemSet(&CellAux[CurCell],0,sizeof(struct StrCellAux));
    
    cell[CurCell].BackColor=CLR_WHITE;
    cell[CurCell].RowSpan=cell[CurCell].ColSpan=1;
    
    for (i=0;i<4;i++) cell[CurCell].BorderColor[i]=CLR_AUTO;

    return TRUE;
}

/******************************************************************************
    RecoverCellSlots:
    Mark unused slots in the cell table.
******************************************************************************/
RecoverCellSlots(PTERWND w)
{
    long l;
    int i;

    // mark each slots as not in use
    for (i=1;i<TotalCells;i++)   cell[i].InUse=FALSE;     // don't touch the default slot

    // mark the slots in use
    for (l=0;l<TotalLines;l++) {
       cell[cid(l)].InUse=TRUE;
    }

    cell[0].InUse=TRUE;  // default cell always in use

    return TRUE;
}

/******************************************************************************
    RecoverTableRowSlots:
    Mark unused slots in the table row array.
******************************************************************************/
RecoverTableRowSlots(PTERWND w)
{
    int i;

    // mark each slots as not in use
    for (i=1;i<TotalTableRows;i++)   TableRow[i].InUse=FALSE;     // don't touch the default slot

    // mark the slots in use
    for (i=0;i<TotalCells;i++) {
       if (cell[i].InUse) TableRow[cell[i].row].InUse=TRUE;
    }

    TableRow[0].InUse=TRUE;  // default row always in use

    return TRUE;
}

/******************************************************************************
    IsTableSplit:
    Returns TRUE if a page break splits table between two rows specified by
    two cells.
******************************************************************************/
BOOL IsTableSplit(PTERWND w,int CurCell,int PrevCell)
{
    long CurCellLine,PrevCellLine;
    int  i,CurCellPage,PrevCellPage;

    if (CurCell<=0 || PrevCell<=0) return FALSE;

    // get the first cell lines
    CurCellLine=cell[CurCell].FirstLine;
    PrevCellLine=cell[PrevCell].FirstLine;
    if (PfmtId[pfmt(CurCellLine)].flags&PAGE_HDR_FTR) return FALSE;  // tables in hdr/footer don't break
    if (PfmtId[pfmt(PrevCellLine)].flags&PAGE_HDR_FTR) return FALSE;  // tables in hdr/footer don't break

    // get the page number for the current and previous cells
    for (i=0;i<TotalPages;i++) if (PageInfo[i].FirstLine>CurCellLine) break;
    if (i>0) i--;
    CurCellPage=i;

    for (i=0;i<TotalPages;i++) if (PageInfo[i].FirstLine>PrevCellLine) break;
    if (i>0) i--;
    PrevCellPage=i;

    if (CurCellPage==PrevCellPage) return FALSE;
    else                           return TRUE;
}
