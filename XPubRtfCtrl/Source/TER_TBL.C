/*===============================================================================
   TER_TBL.C
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
    TerCreateTable:
    This routine allows the user to create a table.  If row argument is -1,
    it shows a dialog box for user to enter row and columns
******************************************************************************/
BOOL WINAPI _export TerCreateTable(HWND hWnd, int row, int col, BOOL refresh)
{
    int AddLines,width,ParaFID;
    int i,j,PrevCell,CurRowId,PrevRowId=-1,CurCell,level,ParentCell;
    long SaveCurLine,LastUndoLine,GroupUndoRef;
    WORD CurFont;
    PTERWND w;
    BOOL RecoverCells=TRUE;
    HCURSOR hSaveCursor=NULL;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data


    // are we already positioned on a table?
    if (cid(CurLine)>0 && !(TerFlags3&TFLAG3_EMBEDDED_TABLES)) return FALSE;

    // get row and column to create the table
    if (row<0) {
       TableRows=3;               // default table parameters
       TableCols=2;
       if (!CallDialogBox(w,"TableParam",TableParam,0L)) return FALSE;
    }
    else {
       TableRows=row;            // use the specified values
       TableCols=col;
    }

    GroupUndoRef=UndoRef;         // to connect all undos in this function

    // check if on the protected area
    if (!PrepForObject(w)) return FALSE;
    
    SaveCurLine=CurLine;

    // calculate the cell width
    if (cid(CurLine)==0) {
       level=ParentCell=0;
       width=TerWrapWidth(w,CurLine,-1);
       width=ScrToTwipsX(width);       // total line width width in twips
    }
    else {                           // embedded table
       int CurCell=cid(CurLine);
       level=cell[CurCell].level+1;
       width=cell[CurCell].width-2*cell[CurCell].margin;
       ParentCell=CurCell;
    }
    width=width/TableCols;          // width of each cell


    // decide number of lines to add
    AddLines=TableRows*TableCols;   // number of cell  markers
    AddLines+=TableRows;            // number of row  markers

    if (!CheckLineLimit(w,TotalLines+AddLines)) return TRUE;
    CurFont=(WORD)GetEffectiveCfmt(w);
    if (TerFont[CurFont].FieldId>0) CurFont=0;
    
    ParaFID=fid(CurLine);            // para-frame id to use

    if (!(TerFlags&TFLAG_NO_HOUR_GLASS)) hSaveCursor=SetCursor(hWaitCursor);    // show hour glass

    // save the previous top level row for undo if this table is being embedded
    if (ParentCell>0) {
       UndoRef=GroupUndoRef;
       SaveUndo(w,CurLine,0,CurLine,0,UNDO_ROW_DEL);
    } 

    for (i=0;i<TableRows;i++) {             // add row breaks
       if ((CurRowId=GetTableRowSlot(w))==-1) continue;

       TableRow[CurRowId].InUse=TRUE;
       TableRow[CurRowId].PrevRow=PrevRowId;
       TableRow[CurRowId].NextRow=-1;
       TableRow[CurRowId].FirstCell=0;
       TableRow[CurRowId].LastCell=0;
       TableRow[CurRowId].CellMargin=DefCellMargin;
       if (HtmlMode && i==0) TableRow[CurRowId].flags|=ROWFLAG_NEW_TABLE;

       if (PrevRowId>0) TableRow[PrevRowId].NextRow=CurRowId;
       PrevRowId=CurRowId;

       PrevCell=-1;



       for (j=0;j<TableCols;j++) {       // add cell breaks
          if (!RecoverCells) TerFlags|=TFLAG_USE_NEXT_ID;   // use the next cell id
          if ((CurCell=GetCellSlot(w,RecoverCells))==-1) continue;

          if (CurCell==(TotalCells-1)) RecoverCells=FALSE;

          cell[CurCell].InUse=TRUE;
          cell[CurCell].row=CurRowId;
          cell[CurCell].width=width;
          cell[CurCell].margin=DefCellMargin;
          cell[CurCell].border=0;
          cell[CurCell].level=level;
          cell[CurCell].ParentCell=ParentCell;
          if (HtmlMode) {             // set the borders by default
             cell[CurCell].border=BORDER_LEFT|BORDER_RIGHT|BORDER_TOP|BORDER_BOT;
             cell[CurCell].BorderWidth[BORDER_INDEX_LEFT]=15;  // default border width
             cell[CurCell].BorderWidth[BORDER_INDEX_RIGHT]=15;
             cell[CurCell].BorderWidth[BORDER_INDEX_TOP]=15;
             cell[CurCell].BorderWidth[BORDER_INDEX_BOT]=15;
             cell[CurCell].flags|=CFLAG_FIX_WIDTH;
             cell[CurCell].FixWidth=cell[CurCell].width;
          }
          cell[CurCell].PrevCell=PrevCell;
          cell[CurCell].NextCell=-1;
          if (PrevCell>0) cell[CurCell].x=cell[PrevCell].x+cell[PrevCell].width;
          else            cell[CurCell].x=0;

          // connect to the previous cell
          if (PrevCell>0) cell[PrevCell].NextCell=CurCell;
          PrevCell=CurCell;

          // connect to the table
          if (TableRow[CurRowId].FirstCell==0) TableRow[CurRowId].FirstCell=CurCell;
          TableRow[CurRowId].LastCell=CurCell;

          // create the cell break line
          InsertMarkerLine(w,CurLine,CellChar,CurFont,pfmt(CurLine),INFO_CELL,CurCell);
          fid(CurLine)=ParaFID;
          CurLine++;
       }

       // create the row break line
       InsertMarkerLine(w,CurLine,ROW_CHAR,CurFont,pfmt(CurLine),INFO_ROW,cid(CurLine-1));
       fid(CurLine)=ParaFID;
       LastUndoLine=CurLine;
       CurLine++;
    }

    if (hSaveCursor) SetCursor(hSaveCursor);

    CurLine=SaveCurLine;    // position at the beginning of the table
    CurCol=0;
    RequestPagination(w,FALSE);

    //ReleaseUndo(w);     // release any existing undos
    UndoRef=GroupUndoRef;
    SaveUndo(w,CurLine,CurCol,LastUndoLine,0,UNDO_ROW_INS);

    // repaginate when inserting big tables
    if ((long)TableRows*TableCols>(long)300) TerRepaginate(hTerWnd,refresh);
    else if (refresh) PaintTer(w);


    return TRUE;
}

/******************************************************************************
     TerSetHtmlTblWidth:
     Set a fixed width for an html table. The CellId can indicate any cell
     in the table.  The 'width' is specified in twips.  Negative width specifies
     percentage.
******************************************************************************/
BOOL WINAPI _export TerSetHtmlTblWidth(HWND hWnd, int CellId, int width)
{
    PTERWND w;
    int row;
    BOOL EntireTable=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CellId<=0) {
       CellId=cid(CurLine);
       EntireTable=TRUE;        // set for the entire table
    }
    if (CellId<=0 || CellId>=TotalCells || !cell[CellId].InUse) return FALSE;

    row=cell[CellId].row;

    if (EntireTable) {
      while (!IsFirstTableRow(w,row)) row=TableRow[row].PrevRow;
      while (row>0) {
        TableRow[row].FixWidth=width;
        row=TableRow[row].NextRow;
      }
    }
    else TableRow[row].FixWidth=width;

    return TRUE;
}

/******************************************************************************
     TerCreateCellId:
     This function creates a new cell id to be used to create the table structure
     within the text.  If the NewRow is TRUE and PrevCell is zero, this function
     create a new table and returns the id of the first cell (and only cell)
     in the table.
     CellWidth of -1 indicates an auto width cell.
     This function does not actually insert a cell in the text.
     This function returns the new cell id if successful, otherwise it returns
     a 0.
******************************************************************************/
int WINAPI _export TerCreateCellId(HWND hWnd, BOOL NewRow, int PrevCell,int RowAlign, int RowPos, int RowMinHeight,
                                   int CellWidth, int shading, int LeftWidth,int RightWidth, int TopWidth, int BotWidth,
                                   int RowSpan, int ColSpan, UINT CellFlags)
{
    PTERWND w;
    int NewCell,row,PrevRow;
    DWORD SaveTerOpFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    // create a new row
    if (NewRow) {
       if (PrevCell>0) {                             // get previous row
          if (PrevCell>=TotalCells || !(cell[PrevCell].InUse)) return 0;
          PrevRow=cell[PrevCell].row;
          if (TableRow[PrevRow].NextRow!=-1) return FALSE;   // not the last row
       }
       else PrevRow=0;

       SaveTerOpFlags=TerOpFlags;
       TerOpFlags|=TOFLAG_NO_RECOVE_ID;              // do not recove ids - as ids may not have text yet
       
       if ((row=GetTableRowSlot(w))==-1) return 0;   // out of table row slot

       TerOpFlags=SaveTerOpFlags;

       TableRow[row].InUse=TRUE;
       if (PrevRow>0) {                             // connect to previous row
          TableRow[PrevRow].NextRow=row;
          TableRow[row].PrevRow=PrevRow;
       }
       else {
          TableRow[row].PrevRow=-1;
          TableRow[row].flags|=ROWFLAG_NEW_TABLE; // keeps two consequtive tables from getting joined
       }

       TableRow[row].NextRow=-1;
       TableRow[row].FirstCell=0;
       TableRow[row].LastCell=0;
       TableRow[row].CellMargin=DefCellMargin;
       if      (RowAlign==CENTER)        TableRow[row].flags|=CENTER;
       else if (RowAlign==RIGHT_JUSTIFY) TableRow[row].flags|=RIGHT_JUSTIFY;
       TableRow[row].MinHeight=RowMinHeight;

       // create a new cell
       if ((NewCell=GetCellSlot(w,FALSE))==-1) return 0;  // out of cell table
       cell[NewCell].InUse=TRUE;
       cell[NewCell].row=row;
       cell[NewCell].margin=DefCellMargin;
       cell[NewCell].PrevCell=-1;
       cell[NewCell].NextCell=-1;
       cell[NewCell].x=RowPos;

       TableRow[row].FirstCell=TableRow[row].LastCell=NewCell;
       TableRow[row].indent=RowPos;
    }
    else {                       // append a cell to the existing row
       if (PrevCell<=0  || PrevCell>=TotalCells || !(cell[PrevCell].InUse)) return 0;
       if (cell[PrevCell].NextCell!=-1) return 0;    // not the last cell

       // create new cell
       if ((NewCell=GetCellSlot(w,FALSE))==-1) return 0;  // out of cell table
       CopyCell(w,PrevCell,NewCell);

       cell[NewCell].x=cell[PrevCell].x+cell[PrevCell].width;

       // set cell and row chain
       cell[PrevCell].NextCell=NewCell;
       cell[NewCell].PrevCell=PrevCell;
       cell[NewCell].NextCell=-1;

       row=cell[PrevCell].row;
       TableRow[row].LastCell=NewCell;

       if (RowMinHeight) TableRow[row].MinHeight=RowMinHeight;  // set the row height
    }

    // set the cell properties
    if (CellWidth>0) cell[NewCell].width=cell[NewCell].FixWidth=CellWidth;
    else {
       cell[NewCell].width=1000;                // set some initial value
       cell[NewCell].FixWidth=CellWidth;        // indicates the percentage
    }

    cell[NewCell].shading=shading;

    // set the cell border
    cell[NewCell].border=0;
    if (LeftWidth>0) cell[NewCell].border|=BORDER_LEFT;
    if (RightWidth>0) cell[NewCell].border|=BORDER_RIGHT;
    if (TopWidth>0) cell[NewCell].border|=BORDER_TOP;
    if (BotWidth>0) cell[NewCell].border|=BORDER_BOT;

    cell[NewCell].BorderWidth[BORDER_INDEX_LEFT]=LeftWidth;
    cell[NewCell].BorderWidth[BORDER_INDEX_RIGHT]=RightWidth;
    cell[NewCell].BorderWidth[BORDER_INDEX_TOP]=TopWidth;
    cell[NewCell].BorderWidth[BORDER_INDEX_BOT]=BotWidth;


    // set the cell flags
    cell[NewCell].flags=0;
    if (RowSpan<1) cell[NewCell].flags|=CFLAG_ROW_SPANNED;
    if (ColSpan<1) cell[NewCell].flags|=CFLAG_COL_SPANNED;
    if (RowSpan<1) RowSpan=1;
    if (ColSpan<1) ColSpan=1;

    if (CellWidth==0) cell[NewCell].flags|=CFLAG_AUTO_WIDTH;
    if (CellWidth<0)  cell[NewCell].flags|=CFLAG_FIX_WIDTH_PCT;  // width percentage specified
    if (CellWidth>0)  cell[NewCell].flags|=CFLAG_FIX_WIDTH;      // fix width specified

    cell[NewCell].flags|=CellFlags;   // append cell bits

    // set the cell spans
    cell[NewCell].RowSpan=RowSpan;
    cell[NewCell].ColSpan=ColSpan;

    return NewCell;
}

/******************************************************************************
     TerSetCellInfo:
     Set additional cell informaton.
******************************************************************************/
BOOL WINAPI _export TerSetCellInfo(HWND hWnd, int CellId, COLORREF BackColor,int margin)
{
     return TerSetCellInfo2(hWnd,CellId,BackColor,margin,0);
}
    
/******************************************************************************
     TerSetCellInfo2:
     Set additional cell informaton.
******************************************************************************/
BOOL WINAPI _export TerSetCellInfo2(HWND hWnd, int CellId, COLORREF BackColor,int margin, int ParentCell)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    if (CellId<=0 || CellId>=TotalCells || !(cell[CellId].InUse)) return 0;

    cell[CellId].BackColor=BackColor;
    if (margin>=0) {
       cell[CellId].margin=margin;
       TableRow[cell[CellId].row].CellMargin=margin;
    }

    if (ParentCell>0) {
       if (ParentCell<=0 || ParentCell>=TotalCells || !(cell[ParentCell].InUse)) return 0;
       cell[CellId].ParentCell=ParentCell;
       cell[CellId].level=cell[ParentCell].level+1;
    }    

    return TRUE;
}

/******************************************************************************
     TerSetCellSpan:
     Set row span and col-span for the cell (set to 0 to leave unchanged).
******************************************************************************/
BOOL WINAPI _export TerSetCellSpan(HWND hWnd, int CellId, int RowSpan, int ColSpan)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    if (CellId<=0) CellId=cid(CurLine);

    if (CellId<=0 || CellId>=TotalCells || !(cell[CellId].InUse)) return 0;

    if (RowSpan>0) cell[CellId].RowSpan=RowSpan;
    if (ColSpan>0) cell[CellId].ColSpan=ColSpan;

    return TRUE;
}
      
/******************************************************************************
     TerSetCellBorderColor:
     Set cell border color
******************************************************************************/
BOOL WINAPI _export TerSetCellBorderColor(HWND hWnd, int CellId, COLORREF top,COLORREF bot, COLORREF left, COLORREF right)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    if (CellId<=0 || CellId>=TotalCells || !(cell[CellId].InUse)) return 0;

    cell[CellId].BorderColor[BORDER_INDEX_TOP]=top;
    cell[CellId].BorderColor[BORDER_INDEX_BOT]=bot;
    cell[CellId].BorderColor[BORDER_INDEX_LEFT]=left;
    cell[CellId].BorderColor[BORDER_INDEX_RIGHT]=right;

    return TRUE;
}

/******************************************************************************
     TerGetCellInfo:
     Get the cell information.
******************************************************************************/
BOOL WINAPI _export TerGetCellInfo(HWND hWnd, int CellId, LPINT row,
                                   LPINT PrevCell,LPINT NextCell,
                                   LPINT width, LPINT border, LPINT shading,
                                   LPINT RowSpan, LPINT ColSpan, UINT far *CellFlags)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (CellId<0 || CellId>=TotalCells /*|| !cell[CellId].InUse*/) return FALSE;

    if (row)       (*row)=cell[CellId].row;
    if (PrevCell)  (*PrevCell)=cell[CellId].PrevCell;
    if (NextCell)  (*NextCell)=cell[CellId].NextCell;
    if (width)     (*width)=cell[CellId].width;
    if (border)    (*border)=cell[CellId].border;
    if (shading)   (*shading)=cell[CellId].shading;
    if (RowSpan)   (*RowSpan)=cell[CellId].RowSpan;
    if (ColSpan)   (*ColSpan)=cell[CellId].ColSpan;
    if (CellFlags) (*CellFlags)=cell[CellId].flags;

    return TRUE;
}

/******************************************************************************
     TerGetCellInfo2:
     Get the additional cell information.
******************************************************************************/
BOOL WINAPI _export TerGetCellInfo2(HWND hWnd, int CellId, COLORREF far *BackColor, LPINT margin)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (CellId<0) CellId=cid(CurLine);
    if (CellId<=0 || CellId>=TotalCells || !cell[CellId].InUse) return FALSE;

    if (BackColor) (*BackColor)=cell[CellId].BackColor;
    if (margin)    (*margin)=cell[CellId].margin;

    return TRUE;
}

/******************************************************************************
     TerGetRowCellCount:
     Get the number of rows in the table, or the number of cell in the current row
******************************************************************************/
int WINAPI _export TerGetRowCellCount(HWND hWnd, BOOL GetRowCount)
{
    PTERWND w;
    int cl,OrigCell,count;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PageMode) return 0;
    cl=OrigCell=cid(CurLine);
    if (cl<=0) return 0;

    if (GetRowCount) {
      int row=cell[cl].row;
      count=1;
      while (!IsFirstTableRow(w,row)) {
         row=TableRow[row].PrevRow;
         count++;
      }
      row=cell[cl].row;
      while (!IsLastTableRow(w,row)) {
         row=TableRow[row].NextRow;
         count++;
      }

      return count;
    }
    else {
      count=1;
      while (cell[cl].PrevCell>0) {
         cl=cell[cl].PrevCell;
         count++;
      } 
      cl=OrigCell;
      while (cell[cl].NextCell>0) {
         cl=cell[cl].NextCell;
         count++;
      }
      return count;
       
    }
}     

/******************************************************************************
     TerGetCellParam:
     Get the additional cell information.
******************************************************************************/
BOOL WINAPI _export TerGetCellParam(HWND hWnd, int type, int CellId, LPINT val)
{
    PTERWND w;
    int result=TRUE;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (!val) return FALSE;
    if (CellId<0) CellId=cid(CurLine);
    if (CellId<=0 || CellId>=TotalCells || !cell[CellId].InUse) return FALSE;

    if (type==CP_FIX_WIDTH)        (*val)=cell[CellId].FixWidth;
    else if (type==CP_PARENT_CELL) (*val)=cell[CellId].ParentCell;
    else if (type==CP_LEVEL)       (*val)=cell[CellId].level;
    else if (type==CP_TEXT_ROTATION) {
       int angle=cell[CellId].TextAngle;
       if  (angle==270)    (*val)= TEXT_TOP_TO_BOT;
       else if (angle==90) (*val)= TEXT_BOT_TO_TOP;
       else                (*val)= TEXT_HORZ;
    } 
    else result=FALSE;

    return result;
}

/******************************************************************************
     TerGetCellBorderWidth:
     Get the current cell border width
******************************************************************************/
BOOL WINAPI _export TerGetCellBorderWidth(HWND hWnd, int CellId, LPINT pLeft, LPINT pRight, LPINT pTop, LPINT pBot)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (CellId<=0) CellId=cid(CurLine);
    if (CellId<=0 || CellId>=TotalCells || !cell[CellId].InUse) return FALSE;

    (*pLeft)=(*pRight)=(*pTop)=(*pBot)=0;

    if (cell[CellId].border&BORDER_LEFT) (*pLeft)=cell[CellId].BorderWidth[BORDER_INDEX_LEFT];
    if (cell[CellId].border&BORDER_RIGHT)(*pRight)=cell[CellId].BorderWidth[BORDER_INDEX_RIGHT];
    if (cell[CellId].border&BORDER_TOP)  (*pTop)=cell[CellId].BorderWidth[BORDER_INDEX_TOP];
    if (cell[CellId].border&BORDER_BOT)  (*pBot)=cell[CellId].BorderWidth[BORDER_INDEX_BOT];

    return TRUE;
}

/******************************************************************************
     TerGetCellBorderColor:
     Get the current cell border color
******************************************************************************/
BOOL WINAPI _export TerGetCellBorderColor(HWND hWnd, int CellId, COLORREF far * pLeft, COLORREF far * pRight, COLORREF far * pTop, COLORREF far * pBot)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (CellId<=0) CellId=cid(CurLine);
    if (CellId<=0 || CellId>=TotalCells || !cell[CellId].InUse) return FALSE;

    (*pLeft)=(*pRight)=(*pTop)=(*pBot)=0;

    (*pLeft)=cell[CellId].BorderColor[BORDER_INDEX_LEFT];
    (*pRight)=cell[CellId].BorderColor[BORDER_INDEX_RIGHT];
    (*pTop)=cell[CellId].BorderColor[BORDER_INDEX_TOP];
    (*pBot)=cell[CellId].BorderColor[BORDER_INDEX_BOT];

    return TRUE;
}

/******************************************************************************
     TerEquateHtmlTable:
     Set the same number of columns for each row in an html table.
******************************************************************************/
BOOL WINAPI _export TerEquateHtmlTable(HWND hWnd, int CellId, int TotalTableCols)
{
    PTERWND w;
    int  i,row,LastCell,NewCell,TotalCols,CellCols;
    long LastLine;
    LPINT  RowSpan=0;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CellId<0 || CellId>=TotalCells || !(cell[CellId].InUse)) return FALSE;

    // allocate memory for the row span table
    if (NULL==(RowSpan=MemAlloc(sizeof(int)*(TotalTableCols+1)))) return FALSE;
    FarMemSet(RowSpan,0,sizeof(int)*(TotalTableCols+1));

    
    // find the first row of the table
    row=cell[CellId].row;
    while (TableRow[row].PrevRow>0) row=TableRow[row].PrevRow;

    // process each table row
    while (row>0) {
       TotalCols=0;
       LastCell=TableRow[row].FirstCell;

       while (TRUE) { // update row span table and get the last cell of the row
          CellCols=cell[LastCell].ColSpan;
          for (i=TotalCols;i<(TotalCols+CellCols);i++) {
             if (cell[LastCell].flags&CFLAG_ROW_SPANNED) RowSpan[i]--;
             else                                        RowSpan[i]=cell[LastCell].RowSpan-1;   // remaining spans left
          }
          TotalCols+=CellCols;

          if (cell[LastCell].NextCell<=0) break;
          LastCell=cell[LastCell].NextCell;
       }

       // create an additional column if short
       if (TotalCols<TotalTableCols) {
          while (TotalCols<TotalTableCols) {
             SetCellLines(w);    // set the first and last lines of the cells

             if ((NewCell=GetCellSlot(w,FALSE))==-1) return FALSE;  // out of cell table
             CopyCell(w,LastCell,NewCell);

             cell[NewCell].RowSpan=1;
             cell[NewCell].ColSpan=1;
             cell[NewCell].FixWidth=0;     // use the default width
             if (RowSpan[TotalCols]>0) {   // is this cell row-spanned
                cell[NewCell].flags=CFLAG_ROW_SPANNED;
                RowSpan[TotalCols]--;
             }
             else cell[NewCell].flags=ResetUintFlag(cell[NewCell].flags,CFLAG_ROW_SPANNED);

             cell[NewCell].width=cell[LastCell].width/cell[LastCell].ColSpan;

             // link the new cell in the chain
             cell[LastCell].NextCell=NewCell;
             cell[NewCell].PrevCell=LastCell;
             TableRow[row].LastCell=NewCell;

             // create the cell break line for the new cell
             LastLine=cell[LastCell].LastLine;
             InsertMarkerLine(w,LastLine+1,CellChar,0,pfmt(LastLine+1),INFO_CELL,NewCell);

             LastCell=NewCell;
             TotalCols++;
          }

          // fix the cell id for the row line
          if ((LastLine+2)<TotalLines) cid(LastLine+2)=cid(LastLine+1);

       }
       row=TableRow[row].NextRow;    // go to next row
    }

    if (RowSpan) MemFree(RowSpan);   // release the row span memory

    return TRUE;
}

/******************************************************************************
     TerAdjustHtmlTable:
     Adjust html table cell widths and table position. This function does
     not refresh the screen.
******************************************************************************/
BOOL WINAPI _export TerAdjustHtmlTable(HWND hWnd)
{
    PTERWND w;
    LPLONG ColMinWidth=NULL,ColMaxWidth=NULL;
    int  i,FirstRow,cl;
    long TblMinWidth,TblMaxWidth,TblWidth;
    BOOL ExactWidth;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // check if the document has nested tables
    HasNestedTables=FALSE;
    for (i=0;i<TotalCells;i++) if (cell[i].InUse && cell[i].ParentCell!=0) HasNestedTables=TRUE;

    //SetRowBreakFlag(w);      // set the row break flags

    SetCellLines(w);    // set the first and last of the cells

    // process each table
    for (FirstRow=1;FirstRow<TotalTableRows;FirstRow++) {
       // process one table
       if (TableRow[FirstRow].InUse && (TableRow[FirstRow].PrevRow<=0 || IsFirstTableRow(w,FirstRow))) {  // first row of a table found
          cl=TableRow[FirstRow].FirstCell;
          if (cell[cl].level>0) continue;            // do only top level rows here

          if (!GetTableMinMaxWidths(w,FirstRow,&TblMinWidth,&TblMaxWidth,&TblWidth,&ExactWidth,
                                    &ColMinWidth, &ColMaxWidth,0L)) continue;

          SetTableCellWidths(w,FirstRow,TblMinWidth,TblMaxWidth,TblWidth,ExactWidth,ColMinWidth,ColMaxWidth);

          // free the pointers returned by the GetTableMinMaxWidth function
          if (ColMinWidth) OurFree(ColMinWidth);
          if (ColMaxWidth) OurFree(ColMaxWidth);
          ColMinWidth=ColMaxWidth=NULL;
       }
    }

    RepageBeginLine=0;

    return TRUE;
}

/******************************************************************************
     SetTableCellWidths:
     Set the width of the table cells for html adjustment.
******************************************************************************/
BOOL SetTableCellWidths(PTERWND w, int FirstRow, long TblMinWidth, long TblMaxWidth, long TblWidth,
                          BOOL ExactWidth, LPLONG ColMinWidth, LPLONG ColMaxWidth)
{         
    int i,row,cl,col,CellX,TotalExactWidth;
    long RowWidth,RowMaxWidth,RowMinWidth;


    // set the cell widths
    row=FirstRow;
    while (row>0) {
       // find the total exact cell width
       TotalExactWidth=0;
       col=0;
       for (cl=TableRow[row].FirstCell;cl>0;cl=cell[cl].NextCell) {
          long CellMinWidth=ColMinWidth[col]*cell[cl].ColSpan;
          if (cell[cl].FixWidth>0 && cell[cl].FixWidth>CellMinWidth) TotalExactWidth+=cell[cl].FixWidth;
          col+=cell[cl].ColSpan;
       }

       RowWidth=TblWidth-TotalExactWidth;       // exact width cells are not considered in the width calculations of other cells 
       RowMaxWidth=TblMaxWidth-TotalExactWidth;
       RowMinWidth=TblMinWidth-TotalExactWidth;
       if (RowMaxWidth==0) RowMaxWidth=1;   // to avoid divide by zero
     
       col=0;
       // process each cell in the current row
       cl=TableRow[row].FirstCell;
       CellX=0;
       while (cl>0) {
          cell[cl].x=CellX;

          // set cell width by adding width of all the columns spanned by this cell
          cell[cl].width=0;


          for (i=0;i<cell[cl].ColSpan;i++) {
             if (cell[cl].FixWidth>0 /*&& cell[cl].FixWidth>=ColMinWidth[col]*/) {  // exact width specified
                cell[cl].width=cell[cl].FixWidth;
                col++;
                continue;
             } 
             
             if (RowMinWidth>=RowWidth)  cell[cl].width+=(int)(ColMinWidth[col]);
             else if (RowMaxWidth<=RowWidth) {
                if (ExactWidth) cell[cl].width+=(int)(ColMaxWidth[col]*RowWidth/RowMaxWidth);
                else            cell[cl].width+=(int)(ColMaxWidth[col]);
             }
             else {        // prorate the width
                long AvailWidth=RowWidth-RowMinWidth;
                long TblMaxMinusMin=RowMaxWidth-RowMinWidth;
                long ColMaxMinusMin=ColMaxWidth[col]-ColMinWidth[col];

                if (TblMaxMinusMin==0) TblMaxMinusMin=1;   // to avoid divide by zero

                if (!ExactWidth) AvailWidth=AvailWidth*9/10;  // leave some are on the left and right
                if (AvailWidth<0) AvailWidth=0;
                cell[cl].width+=(int)(ColMinWidth[col]+(ColMaxMinusMin*AvailWidth/TblMaxMinusMin));
             }
             col++;
          }

          CellX=cell[cl].x+cell[cl].width;   // advance cell x

          // check for embedded tables
          SetSubtableCellWidths(w,cl,cell[cl].width-2*cell[cl].margin);
          

          cl=cell[cl].NextCell;
       }

       // check for row overflow
       if (!HasNestedTables) {
         RowWidth=0;
         for (cl=TableRow[row].FirstCell;cl>0;cl=cell[cl].NextCell) RowWidth+=cell[cl].width;
         if (RowWidth>TblWidth && TblWidth>0) {
            for (cl=TableRow[row].FirstCell;cl>0;cl=cell[cl].NextCell) {
               cell[cl].width=MulDiv(cell[cl].width,TblWidth,RowWidth);
            }
         }
       }    

       row=TableRow[row].NextRow;    // advance to the next row
       if (row>0 && IsFirstTableRow(w,row)) row=0;  // new table starts
    }
 
    return TRUE;
}

/******************************************************************************
     SetSubtableCellWidths:
     Set the width of the embedded tables within the given cell
******************************************************************************/
BOOL SetSubtableCellWidths(PTERWND w, int cl, int width)
{
    long l;
    int CurCell,level,FirstRow;
    long TblMinWidth,TblMaxWidth;
    LPLONG ColMinWidth,ColMaxWidth;
    long   TblWidth;
    int    ExactWidth;

    level=cell[cl].level;

    for (l=cell[cl].FirstLine;l<=cell[cl].LastLine;l++) {
       // check if a subtable encountered
       CurCell=cid(l);
       if (cell[CurCell].level>level) {     // get the first row of the next immediate child
          FirstRow=0;
          for (;l<cell[cl].LastLine;l++) {
             CurCell=cid(l);
             if (CurCell==cl) break;        // back into the parent cell
             if (FirstRow && cell[CurCell].level==(level+1) && FirstRow!=cell[CurCell].row) {  // check if next table begins at the same level
                int row=cell[CurCell].row;
                if (row==0 || IsFirstTableRow(w,row)) break;  // out of subtable, or next subtable
             } 
             if (FirstRow==0 && cell[CurCell].level==(level+1)) FirstRow=cell[CurCell].row;
          }
          
          if (FirstRow>0) {                 // subtable found
             if (GetTableMinMaxWidths(w,FirstRow,&TblMinWidth,&TblMaxWidth,&TblWidth,&ExactWidth,&ColMinWidth,&ColMaxWidth,width)) {
                 SetTableCellWidths(w,FirstRow,TblMinWidth,TblMaxWidth,width,ExactWidth,ColMinWidth,ColMaxWidth);

                 // free the pointers returned by the GetTableMinMaxWidth function
                 OurFree(ColMinWidth);
                 OurFree(ColMaxWidth);
             } 
             l--;
             continue;
          }  
       } 
    }

    return TRUE;
}
  
/******************************************************************************
     GetTableMinMaxWidth:
     Given the first row of the table, get the minimum and maximum widths.
******************************************************************************/
BOOL GetTableMinMaxWidths(PTERWND w, int FirstRow, LPLONG pTblMinWidth, LPLONG pTblMaxWidth, LPLONG pTblWidth, 
                          LPINT pExactWidth, long far * (far* pColMinWidth), long far * (far * pColMaxWidth), 
                          long EmbTblWidth)
{
    LPLONG ColMinWidth,ColMaxWidth;
    LPBYTE ColExact,HasText;
    int  row,cl,col,level;
    long MinWidth,MaxWidth,TblMinWidth,TblMaxWidth,TblCols;
    long SpanMinWidth,SpanMaxWidth,TblWidth;
    int  i,ColSpan;
    BOOL ExactWidth,ExactCellWidth;
 
    // get the table level
    cl=TableRow[FirstRow].FirstCell;
    if (!(cell[cl].InUse)) return FALSE; 
    
    // allocate the arrays
    ColMinWidth=OurAlloc(sizeof(long)*MAX_TBL_COLS);
    ColMaxWidth=OurAlloc(sizeof(long)*MAX_TBL_COLS);
    ColExact=OurAlloc(sizeof(BYTE)*MAX_TBL_COLS);
    HasText=OurAlloc(sizeof(BYTE)*MAX_TBL_COLS);

    level=cell[cl].level;

    // get the table width
    if (InPrinting || !(TerArg.FittedView)) TblWidth=(int)((TerSect1[0].PgWidth-TerSect[0].LeftMargin-TerSect[0].RightMargin)*1440);
    else TblWidth=ScrToTwipsX(TerWinWidth);

    if (TblWidth<0) TblWidth=7*1440;   // happens with zero size window created by mfc
    if (EmbTblWidth>0 && TblWidth>EmbTblWidth) TblWidth=EmbTblWidth;  // embedded table width specified
    
    ExactWidth=FALSE;
    if (TableRow[FirstRow].FixWidth) {
       if (TableRow[FirstRow].FixWidth<0) {      // specified in percentage
           TblWidth=lMulDiv(TblWidth,-TableRow[FirstRow].FixWidth,100);
       }
       else TblWidth=TableRow[FirstRow].FixWidth;
       ExactWidth=TRUE;
    }

    for (col=0;col<MAX_TBL_COLS;col++) {  // initialize minimum and max column width
       ColMinWidth[col]=ColMaxWidth[col]=0;
       ColExact[col]=FALSE;
       HasText[col]=FALSE;                 // mark all columns as spanned in the beginning
    }

    // get minimum/max col width for this table
    TblCols=0;
    row=FirstRow;
    while (row>0) {
       col=0;
       // process each cell in the current row
       cl=TableRow[row].FirstCell;
       while (cl>0) {
          if (cell[cl].FixWidth==0) cell[cl].flags|=CFLAG_AUTO_WIDTH;

          if (cell[cl].flags&CFLAG_AUTO_WIDTH || (!ExactWidth && cell[cl].FixWidth<0)) {  // auto, or if cell width specified in percentage when table does not have a fix width
             GetCellMinMaxWidth(w,cl,&MinWidth,&MaxWidth,TblWidth);
             MinWidth+=(2*cell[cl].margin); // add the cell margin
             MaxWidth+=(2*cell[cl].margin); // add the cell margin
             ExactCellWidth=FALSE;
          }
          else {
             long CellMinWidth,CellMaxWidth;
             if (cell[cl].FixWidth<0) {   // specified in percentage
                 MinWidth=lMulDiv(TblWidth,-cell[cl].FixWidth,100);  // indicates the percentage
                 if (cell[cl].level>0 && EmbTblWidth==0) MinWidth=2*cell[cl].margin;  // if embedded table width not yet available, then percentage width should be set to a small number)
             }
             else  MinWidth=cell[cl].FixWidth;
             MaxWidth=MinWidth;
             ExactCellWidth=TRUE;

             GetCellMinMaxWidth(w,cl,&CellMinWidth,&CellMaxWidth,TblWidth);
             if (cell[cl].FixWidth>0 && cell[cl].FixWidth>CellMinWidth) CellMinWidth=CellMaxWidth=MinWidth;
             else if (CellMinWidth>MinWidth && !(TerFlags3&TFLAG3_EXACT_CELL_WIDTH))
                   MinWidth=MaxWidth=CellMinWidth;
             else  ExactCellWidth=TRUE;
          }

          if ((cell[cl].FirstLine!=cell[cl].LastLine) || LineLen(cell[cl].FirstLine)>1) HasText[col]=TRUE;

          // apply the width to all the columns covered by this cell
          if (cell[cl].ColSpan<1) cell[cl].ColSpan=1;
          if (cell[cl].ColSpan==1) {
             if (!ColExact[col] || ExactCellWidth) {
                if (MinWidth>ColMinWidth[col] || ExactCellWidth) ColMinWidth[col]=MinWidth;
                if (MaxWidth>ColMaxWidth[col] || ExactCellWidth) ColMaxWidth[col]=MaxWidth;
             }
             if (ExactCellWidth) ColExact[col]=TRUE;
             col++;
          }
          else {
             SpanMinWidth=SpanMaxWidth=0;  // cal the current min/max for the span
             ColSpan=cell[cl].ColSpan;
             for (i=0;i<ColSpan;i++) {
                SpanMinWidth+=ColMinWidth[col+i];
                SpanMaxWidth+=ColMaxWidth[col+i];
             }
             if (!ColExact[col] || ExactCellWidth) {
                if (MinWidth>SpanMinWidth) { // distribut the difference
                   long adj=(MinWidth-SpanMinWidth)/ColSpan;
                   for (i=0;i<ColSpan;i++) ColMinWidth[col+i]+=adj;
                }
                if (MaxWidth>SpanMinWidth) { // distribut the difference
                   long adj=(MaxWidth-SpanMinWidth)/ColSpan;
                   for (i=0;i<ColSpan;i++) ColMaxWidth[col+i]+=adj;
                }
             }
             if (ExactCellWidth) ColExact[col]=TRUE;
             if (HasText[col]) for (i=1;i<ColSpan;i++) HasText[col+i]=TRUE;
             col+=ColSpan;
          }
          cl=cell[cl].NextCell;
       }
       if (col>TblCols) TblCols=col; // max number of columns in the table

       row=TableRow[row].NextRow;    // advance to the next row
       if (row>0 && IsFirstTableRow(w,row)) row=0;  // next table starts
    }

    // get mininum/max width for the table
    TblMinWidth=0;
    TblMaxWidth=0;
    for (col=0;col<TblCols;col++) {
       // limit the max width to the width of the window
       //if (ColMinWidth[col]>TblWidth) ColMinWidth[col]=TblWidth;   // limit to some large value value
       if (ColMinWidth[col]>ColMaxWidth[col]) ColMaxWidth[col]=ColMinWidth[col];

       if (!HasText[col] && !ColExact[col]) ColMinWidth[col]=ColMaxWidth[col]=100;   // non-data column

       TblMinWidth+=ColMinWidth[col];
       TblMaxWidth+=ColMaxWidth[col];
    }

    
    // release memory
    OurFree(ColExact);
    OurFree(HasText);

    if (pColMinWidth) (*pColMinWidth)=ColMinWidth;    // the calling function releases this memory
    else              OurFree(ColMinWidth);
    
    if (pColMaxWidth) (*pColMaxWidth)=ColMaxWidth;    // the calling function releases this memory
    else              OurFree(ColMaxWidth);
    
    if (pTblMinWidth) (*pTblMinWidth)=TblMinWidth;
    if (pTblMaxWidth) (*pTblMaxWidth)=TblMaxWidth;
    if (pExactWidth)  (*pExactWidth)=ExactWidth;
    if (pTblWidth)    (*pTblWidth)=TblWidth;
    
    return TRUE;
}

/******************************************************************************
     SetRowBreakFlag:
     Set the ROWFLAG_BREAK for the table rows.
******************************************************************************/
BOOL SetRowBreakFlag(PTERWND w)
{
     int i,j,FirstRow,k,row1,row2,SpanCols,cell1,cell2,TotalCols1,TotalCols2,CheckCols,cl,width1,width2;
     LPINT pWidth1,pWidth2;

     FirstRow=-1;       // first row of a table

     for (i=0;i<TotalTableRows;i++) ResetUintFlag(TableRow[i].flags,ROWFLAG_BREAK);

     for (i=0;i<TotalTableRows;i++) {
         if (!TableRow[i].InUse || TableRow[i].PrevRow<=0 || TableRow[i].flags&ROWFLAG_NEW_TABLE) continue;

         row1=i;
         row2=TableRow[i].PrevRow;
         if (FirstRow<0) FirstRow=row2;    // first row of a table

         TotalCols1=TotalCols2=0;
         for (cl=TableRow[row1].FirstCell;cl>0;cl=cell[cl].NextCell) {
            if (cell[cl].flags&CFLAG_ROW_SPANNED) break;   // this cell connnect with following row
            if (cell[cl].ColSpan<1) cell[cl].ColSpan=1;  // to avoid divide-by-zero
            TotalCols1+=cell[cl].ColSpan;
         }
         if (cl>0) continue;   // this row is spanned by the previous row - so can't have a row break

         for (cl=TableRow[row2].FirstCell;cl>0;cl=cell[cl].NextCell) {
            if (cell[cl].RowSpan>1) break;   // this cell connnect with following row
            if (cell[cl].ColSpan<1) cell[cl].ColSpan=1;  // to avoid divide-by-zero
            TotalCols2+=cell[cl].ColSpan;
         }
         if (cl>0) continue;   // this row spans the next row - so can't have a row break

         // compare width adding up colspan values
         cell1=TableRow[row1].FirstCell;
         cell2=TableRow[row2].FirstCell;
         while (cell1>0 && cell2>0) {
            if (cell[cell1].ColSpan==1 && cell[cell2].ColSpan==1) {
               if (cell[cell1].FixWidth==cell[cell2].FixWidth) {
                  cell1=cell[cell1].NextCell;
                  cell2=cell[cell2].NextCell;
                  continue;
               }
               else goto COMPARE_AVERAGE_WIDTH;  // no-match, check if average width matches
            }
            // colspan used
            SpanCols=max(cell[cell1].ColSpan,cell[cell2].ColSpan); 
            width1=width2=0;
            for (j=0;j<SpanCols && cell1>0;j++) {
               width1+=cell[cell1].FixWidth;
               j+=(cell[cell1].ColSpan-1);    // the loop automatically increments by 1
               cell1=cell[cell1].NextCell;
            } 
            for (j=0;j<SpanCols && cell2>0;j++) {
               width2+=cell[cell2].FixWidth;
               j+=(cell[cell2].ColSpan-1);    // the loop automatically increments by 1
               cell2=cell[cell2].NextCell;
            }
            if (width1==width2) continue;
            else goto COMPARE_AVERAGE_WIDTH;    // no-match, check if average width matches
         } 
         continue;     // all columns matched

         COMPARE_AVERAGE_WIDTH:
         pWidth1=MemAlloc(TotalCols1*sizeof(int));
         pWidth2=MemAlloc(TotalCols2*sizeof(int));

         // get the cell width for the first row
         j=0;
         for (cl=TableRow[row1].FirstCell;cl>0;cl=cell[cl].NextCell) {
            for (k=0;k<cell[cl].ColSpan;k++,j++) pWidth1[j]=cell[cl].FixWidth/cell[cl].ColSpan;
         } 
         j=0;
         for (cl=TableRow[row2].FirstCell;cl>0;cl=cell[cl].NextCell) {
            for (k=0;k<cell[cl].ColSpan;k++,j++) pWidth2[j]=cell[cl].FixWidth/cell[cl].ColSpan;
         } 

         // compare the cell widths
         CheckCols=(TotalCols1<TotalCols2?TotalCols1:TotalCols2);
         for (j=0;j<CheckCols;j++) {
            if (pWidth1[j]==0 || pWidth2[j]==0) continue;  // auto width
            if (pWidth1[j]>0 && pWidth2[j]>0) continue;    // fix width
            if (pWidth1[j]==pWidth2[j]) continue;          // same width
            break;                                         // percentage width, or mix of percentage and fix width
         } 
         if (j<CheckCols) goto SET_ROW_BREAK; // treat this row as a beginning of separate table for calculating the cell widths

         MemFree(pWidth1);  // free 
         MemFree(pWidth2);
         continue;

         SET_ROW_BREAK:
         if (FirstRow>0) TableRow[i].flags=TableRow[FirstRow].flags;
         if (FirstRow>0) TableRow[i].FixWidth=TableRow[FirstRow].FixWidth;
         TableRow[i].flags|=ROWFLAG_BREAK; // treat this row as a beginning of separate table for calculating the cell widths
         FirstRow=-1;
     } 
 
     return TRUE;
}

/******************************************************************************
     TerGetTablePos:
     Get the current table position in terms of table number, row number, column number.
******************************************************************************/
BOOL WINAPI _export TerGetTablePos(HWND hWnd, LPINT pTableNo, LPINT pRowNo, LPINT pColNo)
{
   return TerGetTablePos2(hWnd,pTableNo,pRowNo,pColNo,0);
}
        
/******************************************************************************
     TerGetTablePos:
     Get the current table position in terms of table number, row number, column number.
******************************************************************************/
BOOL WINAPI _export TerGetTablePos2(HWND hWnd, LPINT pTableNo, LPINT pRowNo, LPINT pColNo, int ParentCell)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    int  CellId,CurTableNo,CurRowNo,CurColNo;
    int  ParentLevel,CurLevel;
    BOOL InTable=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    
    // find the first parent line
    if (ParentCell>0) {
       if (ParentCell>TotalCells) return FALSE;
       if (!(cell[ParentCell].InUse)) return FALSE;
       ParentLevel=cell[ParentCell].level;
       FirstLine=-1;
       for (l=0;l<TotalLines;l++) {
          if (FirstLine<0) {
             if (InOuterLevels(w,ParentLevel,l) || LevelCell(w,ParentLevel,l)!=ParentCell) continue;
             FirstLine=LastLine=l;
          }
          else if (InOuterLevels(w,ParentLevel,l) || LevelCell(w,ParentLevel,l)!=ParentCell) break;
          LastLine=l; 
       }
       if (FirstLine<0) return FALSE;
       CurLevel=ParentLevel+1; 
    }
    else {
       FirstLine=0;
       LastLine=TotalLines-1; 
       ParentLevel=CurLevel=0;
    }
    
    if (InOuterLevels(w,CurLevel,CurLine)) return FALSE;     // not postioned on a table or correct table level
    if (CurLine<FirstLine || CurLine>LastLine) return FALSE; // not in the parent cell

    // get the table position
    if (pTableNo) {
       l=FirstLine;
       CurTableNo=-1;
       while (l<=CurLine) {
          if (!InTable && !InOuterLevels(w,CurLevel,l)) {               // new table begins?
             CurTableNo++;
             InTable=TRUE;
          }
          if (InTable && InOuterLevels(w,CurLevel,l)) InTable=FALSE;  // table ended
          l++;
       }
       (*pTableNo)=CurTableNo;
    }

    // get the table row position
    if (pRowNo) {
       CurRowNo=0;
       l=CurLine;
       while (l>=FirstLine) {
          if (InOuterLevels(w,CurLevel,l)) break;   // out of the table
          if (tabw(l) && tabw(l)->type&INFO_ROW && TableLevel(w,l)==CurLevel) CurRowNo++;
          l--;
       }
       (*pRowNo)=CurRowNo;
    }

    // get the column position
    if (pColNo>0) {
       CurColNo=0;
       l=CurLine-1;
       while (l>=0) {
          CellId=cid(l);
          if (InOuterLevels(w,CurLevel,l)) break;   // out of the table row
          if (LineInfo(w,l,INFO_ROW) && TableLevel(w,l)==CurLevel) break;   // out of the table row
          if (LineInfo(w,l,INFO_CELL) && TableLevel(w,l)==CurLevel) CurColNo++;
          l--;
       }
       (*pColNo)=CurColNo;
    }

    return TRUE;
}

/******************************************************************************
TerSplitCell: MAK:
This is an new function because TerSplitCell is not public and goes via PTERWND
******************************************************************************/
BOOL WINAPI _export TerTableSplitCell(HWND hWnd)
{
  PTERWND w;

  if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

  TerSplitCell(w);

	return TRUE;
}

/******************************************************************************
    TerSplitCell:
    Split the current cell. The new cell is created after the current
    cell.
******************************************************************************/
TerSplitCell(PTERWND w)
{
    int row,CurCell,OrigRow,OrigWidth,OrigCell,NextCell,ColCell,OrigCol,PrevCell,CurRowId,RowSpan;
    long l,OrigLine;
    int GrpUndoRef;
    WORD CurFont;

    if (HilightType!=HILIGHT_OFF) return TRUE;  // cell selection not allowed
    if (!TerArg.PrintView) return TRUE;

    // save for undo
    GrpUndoRef=UndoRef;
    OrigLine=CurLine;   // undo scope
    SaveUndo(w,OrigLine,0,OrigLine,0,UNDO_ROW_DEL);

    CurCell=ColCell=OrigCell=cid(CurLine);
    RowSpan=cell[CurCell].RowSpan;
    OrigCol=GetCellColumn(w,CurCell,TRUE);
    OrigWidth=cell[CurCell].width;
    OrigRow=cell[OrigCell].row;

    while (TRUE) {
       if (CurCell==0) break;
       if (tabw(CurLine) && tabw(CurLine)->type&INFO_ROW) break;
       if (cell[CurCell].width<=MIN_CELL_WIDTH) {
          MessageBeep(0);
          break;
       }

       // check the line limit
       if (!CheckLineLimit(w,TotalLines+1)) break;
       CurFont=(WORD)GetEffectiveCfmt(w);

       // prepare for cell chain
       PrevCell=CurCell;
       NextCell=cell[CurCell].NextCell;
       CurRowId=cell[CurCell].row;

       // find the first line of the next cell
       l=CurLine;
       while (l<TotalLines) {
          if (tabw(l) && tabw(l)->type&INFO_ROW) break;
          if (NextCell>0 && cid(l)==NextCell) break;
          if (cid(l)==0 || l==(TotalLines-1)) {
             return PrintError(w,MSG_MISSING_ROW_MARKER,"TerSplitCell");
          }
          l++;
       }
       CurLine=l;
       CurCol=0;

       // insert a cell
       if ((CurCell=GetCellSlot(w,TRUE))==-1) return FALSE;

       // copy the cell properties from the next row cell
       CopyCell(w,PrevCell,CurCell);

       cell[CurCell].InUse=TRUE;
       cell[CurCell].PrevCell=PrevCell;
       if (PrevCell>0) cell[PrevCell].NextCell=CurCell;
       cell[CurCell].NextCell=NextCell;
       if (NextCell>0) cell[NextCell].PrevCell=CurCell;

       // adjust the cell width
       cell[CurCell].width=cell[PrevCell].width=cell[PrevCell].width/2;
       cell[CurCell].x=cell[PrevCell].x+cell[PrevCell].width;
       if (cell[PrevCell].ColSpan>1) cell[PrevCell].ColSpan--;
       cell[CurCell].ColSpan=1;

       // connect to the table
       if (TableRow[CurRowId].LastCell==PrevCell) TableRow[CurRowId].LastCell=CurCell;

       // create the cell break line
       InsertMarkerLine(w,CurLine,CellChar,CurFont,pfmt(CurLine-1),INFO_CELL,CurCell);

       // set the cell id for the table row line
       if (CurLine<(TotalLines-1) && tabw(CurLine+1) && tabw(CurLine+1)->type&INFO_ROW) cid(CurLine+1)=cid(CurLine);

       // decrement the row span
       RowSpan--;
       if (RowSpan<1) break;
       row=cell[ColCell].row;
       row=TableRow[row].NextRow;  // position at the next row
       if (row<=0) break;
       ColCell=GetColumnCell(w,row,OrigCol,TRUE);
       while ((CurLine+1)<TotalLines) {   // find the line at the next column cell
          if (cid(CurLine)==ColCell) break;
          CurLine++;
       }
       CurCol=0;
       CurCell=cid(CurLine);
    }

    // increase the colspan of the other cells in the same column
    row=cell[OrigCell].row;
    while (!IsFirstTableRow(w,row)) row=TableRow[row].PrevRow;  // got the beginning of the table
    for (;row>0;row=TableRow[row].NextRow) {
       if (row==OrigRow) continue;
       ColCell=GetColumnCell(w,row,OrigCol,TRUE);
       if (abs(cell[ColCell].width-OrigWidth)<RULER_TOLERANCE) cell[ColCell].ColSpan++;
    } 

    // save for undo
    UndoRef=GrpUndoRef;
    SaveUndo(w,OrigLine,0,OrigLine,0,UNDO_ROW_INS);

    PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerMergeCells:
    Merge the cells in the highlighted block of text.
******************************************************************************/
TerMergeCells(PTERWND w)
{
    int CurRowId,CurCell,PrevCell,PrevToPrevCell,len,RowSpan,col1,TempCell;
    long l,EndRow,TopCellLine,FirstLine,LastLine;
    int    TopCell,GrpUndoRef;
    LPBYTE ptr,ptr1;
    LPWORD fmt1;
    BOOL SetCurCell,AdjustColSpan;

    if (HilightType==HILIGHT_OFF) return TRUE;
    if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block


    // validate that the block contains cells only
    EndRow=HilightEndRow;
    for (l=HilightBegRow;l<=EndRow;l++) if (cid(l)==0) break;
    if (l<=EndRow) return FALSE;     // non cell lines found


    MarkCells(w,0);                  // mark highlighted cells

    // save the lines for undo
    GetTableSelRange(w,&FirstLine,&LastLine);
    GrpUndoRef=UndoRef;
    if (FirstLine!=-1) SaveUndo(w,FirstLine,0,LastLine,0,UNDO_ROW_DEL);


    CurCell=cid(EndRow);
    CurRowId=cell[CurCell].row;

    // check if ColSpan need to be adjusted
    AdjustColSpan=TRUE;
    if (TableRow[CurRowId].NextRow>0) {
      col1=GetCellColumn(w,CurCell,TRUE);
      TempCell=GetColumnCell(w,TableRow[CurRowId].NextRow,col1,TRUE);
      if (cell[TempCell].ColSpan==1) {  // don't look for uniform table if the next row cell already merged
         if (abs(cell[CurCell].x-cell[TempCell].x)>RULER_TOLERANCE) AdjustColSpan=FALSE;
         if (abs(cell[CurCell].width-cell[TempCell].width)>RULER_TOLERANCE) AdjustColSpan=FALSE;
      }
    }
    if (TableRow[CurRowId].PrevRow>0) {
      col1=GetCellColumn(w,CurCell,TRUE);
      TempCell=GetColumnCell(w,TableRow[CurRowId].PrevRow,col1,TRUE);
      if (cell[TempCell].ColSpan==1) {  // don't look for uniform table if the next row cell already merged
         if (abs(cell[CurCell].x-cell[TempCell].x)>RULER_TOLERANCE) AdjustColSpan=FALSE;
         if (abs(cell[CurCell].width-cell[TempCell].width)>RULER_TOLERANCE) AdjustColSpan=FALSE;
      }
    }


    // Horizontal merge
    SetCurCell=FALSE;
    for (l=EndRow;l>=HilightBegRow;l--) {
       if (tabw(l) && tabw(l)->type&INFO_ROW) {  // new row begins
          CurCell=cid(l);
          CurRowId=cell[CurCell].row;
          continue;
       }
       if (!(cell[cid(l)].flags&CFLAG_SEL1)) {   // reset the last cell in the column
          SetCurCell=TRUE;
          continue;
       }
       if (SetCurCell) {                         // record the last cell in the column
          CurCell=cid(l);
          SetCurCell=FALSE;
          continue;
       }

       PrevCell=cid(l);
       if (PrevCell==CurCell) continue;  // same cell contiuing

       // merge this cell
       if (tabw(l) && tabw(l)->type&INFO_CELL) { // convert cell mark to paramark
           if (LineLen(l)==1) {
              MoveLineArrays(w,l,1,'D');  // This line had only the cell character
              HilightEndRow--;            // adjust highlight end row
           }
           else {  // convert the cell marker to the para marker
              ptr=pText(l);
              len=LineLen(l);
              if (len>0 && ptr[len-1]==CellChar) ptr[len-1]=ParaChar;

              tabw(l)->type=ResetUintFlag(tabw(l)->type,INFO_CELL);  // does not contain a cell id any more
           }

           // update the current cell width and position
           cell[CurCell].x=cell[PrevCell].x;
           cell[CurCell].width+=cell[PrevCell].width;
           if (AdjustColSpan) cell[CurCell].ColSpan++;

           // remove the previous cell from the cell chain
           PrevToPrevCell=cell[PrevCell].PrevCell;
           if (PrevToPrevCell>0) {
              cell[PrevToPrevCell].NextCell=CurCell;
              cell[CurCell].PrevCell=PrevToPrevCell;
           }
           else {
              cell[CurCell].PrevCell=-1;
              TableRow[CurRowId].FirstCell=CurCell;
           }

       }

       // assign the cell id
       cid(l)=CurCell;
       CurLine=HilightBegRow;
    }

    // update para id for the remaining partial cell
    if (PrevCell!=CurCell) {
       l=HilightBegRow-1;
       while (l>=0) {
         if (cid(l)!=PrevCell) break;
         cid(l)=CurCell;
         l--;
       }
    }

    // Vertical Merge
    EndRow=HilightEndRow;
    TopCell=cid(HilightBegRow);
    RowSpan=cell[TopCell].RowSpan;              // initial row span of the top cell
    for (l=HilightBegRow;l<=EndRow;l++) if (tabw(l) && tabw(l)->type&INFO_CELL) break;
    TopCellLine=l;       // last line of the top cell

    for (l=TopCellLine+1;l<=EndRow;l++) {
        if (tabw(l) && tabw(l)->type&INFO_ROW) continue;

        CurCell=cid(l);
        if (!(cell[CurCell].flags&CFLAG_SEL1)) continue;

        // check if it is a last line of the cell
        if (tabw(l) && tabw(l)->type&INFO_CELL) {
           if (!(cell[CurCell].flags&CFLAG_ROW_SPANNED)) RowSpan+=cell[CurCell].RowSpan;
           //RowSpan++;
           
           cell[CurCell].flags|=CFLAG_ROW_SPANNED;
           cell[CurCell].RowSpan=1;
           CellAux[CurCell].SpanningCell=TopCell;
        }

        // convert any cell marker to paramarker in the top cell last line
        len=LineLen(TopCellLine);
        ptr=pText(TopCellLine);
        if (len>0 && ptr[len-1]==CellChar) {  // if current line is not blank line
           if (len==1) LineAlloc(w,TopCellLine,LineLen(TopCellLine),0);
           else {
              ptr[len-1]=ParaChar;
              if (tabw(TopCellLine)) tabw(TopCellLine)->type=ResetUintFlag(tabw(TopCellLine)->type,INFO_CELL);
           }
        }

        // append this line to the top cell and mark it as
        MoveLineArrays(w,TopCellLine,1,'A');  // create a line after the last line

        TopCellLine++;                       // update the top cell last line
        l++;
        EndRow++;

        len=LineLen(l);
        LineAlloc(w,TopCellLine,LineLen(TopCellLine),len);
        MoveCharInfo(w,l,0,TopCellLine,0,len);

        pfmt(TopCellLine)=pfmt(l);
        cid(TopCellLine)=TopCell;
        fid(TopCellLine)=fid(TopCellLine-1);

        ptr1=pText(l);
        fmt1=OpenCfmt(w,l);

        if (ptr1[len-1]==CellChar) {
           ptr1[0]=ptr1[len-1];             // leave only the cell marker on this line
           fmt1[0]=fmt1[len-1];
           LineAlloc(w,l,LineLen(l),1);

           if (!tabw(TopCellLine)) AllocTabw(w,TopCellLine);
           if (tabw(TopCellLine)) tabw(TopCellLine)->type|=INFO_CELL;
        }
        else LineAlloc(w,l,LineLen(l),0);   // empty the current line

        CloseCfmt(w,l);
        CloseCfmt(w,TopCellLine);
    }

    cell[TopCell].RowSpan=RowSpan;          // update the row span for the top cell


    TerArg.modified++;

    CurLine=HilightBegRow;
    CurCol=0;

    // save the lines for undo
    UndoRef=GrpUndoRef;
    if (FirstLine!=-1) SaveUndo(w,FirstLine,0,EndRow,0,UNDO_ROW_INS);

    HilightType=HILIGHT_OFF;

    PaintTer(w);

    TerRepaginate(hTerWnd,TRUE);

    return TRUE;
}

/******************************************************************************
    TerDeleteCells:
    Delete the table cells.  The select parameter can be SEL_CELLS,
    SEL_ROWS, SEL_COLS, or 0 to invoke the dialog box.
******************************************************************************/
BOOL WINAPI _export TerDeleteCells(HWND hWnd, int select, BOOL repaint)
{
    int j;
    int cl,CurCell,PrevCell,NextCell,CurRowId,PrevRowId,NextRowId,FirstUndoRow,LastUndoRow,GrpUndoRef;
    long LineCount,RowLine,DelLastLine,FirstLine,LastLine;
    BOOL TblHilighted=FALSE;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TblHilighted=TRUE;
    
    if (TblHilighted) {
       int level=MinTableLevel(w,HilightBegRow,HilightEndRow);
       CurCell=LevelCell(w,level,CurLine);
    }    
    else CurCell=cid(CurLine);
    
    if (!TerArg.PrintView || (CurCell==0 && !TblHilighted)) return TRUE;

    if (select) {
       if      (select==SEL_CELLS) select=IDC_TABLE_SEL_CELLS;
       else if (select==SEL_ROWS)  select=IDC_TABLE_SEL_ROWS;
       else if (select==SEL_COLS)  select=IDC_TABLE_SEL_COLS;
       else return FALSE;
    }
    else {
       select=CallDialogBox(w,"DeleteCellsParam",DeleteCellsParam,0L);
       if (select==0) return TRUE;
    }

    // mark the selected cells
    if (!MarkCells(w,select)) return FALSE;
    HilightType=HILIGHT_OFF;     // remove highlighting before delete

    // Get first and last line of each cell
    SetCellLines(w);
    //TerOpFlags|=TOFLAG_DEL_CELL; // deleting table cells

    // save the lines for undo
    if (!InUndo) {
       GetTableSelRange(w,&FirstLine,&LastLine);
       GrpUndoRef=UndoRef;
       if (FirstLine!=-1) {
          int cl;

          SaveUndo(w,FirstLine,0,LastLine,0,UNDO_ROW_DEL);
          
          // UNDO_ROW_DEL saves the level0 row
          // so, find the level 0 cell at the first and last line, and save the corresponding row number
          cl=LevelCell(w,0,FirstLine);
          FirstUndoRow=cell[cl].row;
          cl=LevelCell(w,0,LastLine);
          LastUndoRow=cell[cl].row;

          //FirstUndoRow=cell[cid(FirstLine)].row;
          //LastUndoRow=cell[cid(LastLine)].row;
       }
    }

    // Delete the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;
       if (cell[cl].FirstLine==-1 || cell[cl].LastLine==-1) continue;

       // get the row and col chain variables
       CurRowId=cell[cl].row;
       PrevRowId=TableRow[CurRowId].PrevRow;
       NextRowId=TableRow[CurRowId].NextRow;

       PrevCell=cell[cl].PrevCell;
       NextCell=cell[cl].NextCell;

       // Save for undo
       DelLastLine=cell[cl].LastLine;
        
       if (cl==TableRow[CurRowId].FirstCell && cl==TableRow[CurRowId].LastCell) DelLastLine++; // include the row break line
       //SaveUndo(w,cell[cl].FirstLine,0,DelLastLine,LineLen(DelLastLine)-1,'D');

       // fix the backward cell chain
       if (PrevCell>0) {      // fix the backward chain
          cell[PrevCell].NextCell=NextCell;
       }
       else {                 // first cell of the row deleted
          if (NextCell<=0) {  // last cell of the row also deleted
             // delete this empty row
             if (PrevRowId>0) TableRow[PrevRowId].NextRow=NextRowId;
             if (NextRowId>0) TableRow[NextRowId].PrevRow=PrevRowId;

             // include the row line for deletion
             cell[cl].LastLine++;

             // delete the rowe
             TableRow[CurRowId].InUse=FALSE;                  // delete the current row
          }
          else {              // fix the first cell of the row
             TableRow[CurRowId].FirstCell=NextCell;
          }
       }

       // fix the forward cell chain
       if (NextCell>0) cell[NextCell].PrevCell=PrevCell;      // fix the forward chain
       else {
          TableRow[CurRowId].LastCell=PrevCell;  // last cell being deleted

          // fix the para id for the row line
          RowLine=cell[cl].LastLine+1;
          if (tabw(RowLine) && tabw(RowLine)->type&INFO_ROW) {
             cid(RowLine)=PrevCell;
          }
       }

       // delete the text line for the cell
       LineCount=cell[cl].LastLine-cell[cl].FirstLine+1;
       
       MoveLineArrays(w,cell[cl].FirstLine,LineCount,'D'); // delete the cell lines
       for (j=0;j<TotalCells;j++) {     // scroll the line numbers of other cells
          if (cell[j].InUse && cell[j].FirstLine>cell[cl].LastLine) {
             cell[j].FirstLine-=LineCount;
             cell[j].LastLine-=LineCount;
          }
       }

       cell[cl].InUse=FALSE;                             // delete the current cell
    }

    TerOpFlags=ResetLongFlag(TerOpFlags,TOFLAG_DEL_CELL);

    // adjust the current line
    if (CurLine>=TotalLines) CurLine=TotalLines-1;
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
    if (CurCol<0) CurCol=0;

    // check if the modified rows needs to be written out for undo
    if (!InUndo) {
       int FirstCell=TableRow[FirstUndoRow].FirstCell;
       int LastCell=TableRow[LastUndoRow].LastCell;
       if (cell[FirstCell].InUse && cell[LastCell].InUse) {
         GrpUndoRef=UndoRef;
         SaveUndo(w,cell[FirstCell].FirstLine,0,cell[LastCell].LastLine,0,UNDO_ROW_INS);
       }
    }

    TerArg.modified++;
    RequestPagination(w,false);

    PaintTer(w);


    return TRUE;
}

/******************************************************************************
    TerDeleteCellText:
    Delete the table cell contents.  The select parameter can be SEL_CELLS,
    SEL_ROWS, SEL_COLS, or 0 to invoke the dialog box.
******************************************************************************/
BOOL WINAPI _export TerDeleteCellText(HWND hWnd, int select, BOOL repaint)
{
    int j,CurLevel;
    int cl,CurCell;
    int LineCount,GrpUndoRef,FirstLine,LastLine,LastCol;
    BOOL DelLastLine;

    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // Check if the cell text can be deleted
    if (HilightType==HILIGHT_CHAR) {
       if (!TableHilighted(w)) return false;   // the entire selection should be within the same table
    }
    else if (cid(CurLine)==0) return false;
      
    CurLevel=MinTableLevel(w,HilightBegRow,HilightEndRow);
    CurCell=LevelCell(w,CurLevel,CurLine);


    if (True(select)) {
       if      (select==SEL_CELLS) select=IDC_TABLE_SEL_CELLS;
       else if (select==SEL_ROWS)  select=IDC_TABLE_SEL_ROWS;
       else if (select==SEL_COLS)  select=IDC_TABLE_SEL_COLS;
       else return false;
    }
    else {
       select=CallDialogBox(w,"DeleteCellsParam",DeleteCellsParam,0L);
       if (select==0) return TRUE;
    }

    GrpUndoRef=UndoRef;

    // mark the selected cells
    if (!MarkCells(w,select)) return false;
    HilightType=HILIGHT_OFF;     // remove highlighting before delete

    // Get first and last line of each cell
    SetCellLines(w);

    // Delete the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (False(cell[cl].InUse) || False(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;
       if (cell[cl].level!=CurLevel) continue;
         
       FirstLine=cell[cl].FirstLine;
       LastLine=cell[cl].LastLine;

       if (FirstLine==-1 || LastLine==-1) continue;
       if (FirstLine==LastLine && LineLen(FirstLine)==1) continue;  // nothing to delete

       LastCol=LineLen(LastLine)-2;   // inclusive - do not delete the cell mark
       if (LastCol<0) {
          LastLine--;
          LastCol=LineLen(LastLine)-1;    // LastCol is inclusive
          if (LastLine<FirstLine) continue;
          DelLastLine=true;              // delete the last line completely
       }
       else DelLastLine=false;           // do not delete the last line completely because it include the CELL_CHAR
          
       // delete the cell text
       UndoRef=GrpUndoRef;
       SaveUndo(w,FirstLine,0,LastLine,LastCol,'D');

       // delete the text line for the cell
       LineCount=LastLine-FirstLine+1;
       if (DelLastLine) {               // entire last line being deleted as well
          MoveLineArrays(w,FirstLine,LineCount,'D'); // delete the cell lines
       }
       else {                           // last line contains the CELL_CHAR, do not delete cell char
          LineCount--;
          if (LineCount>0) MoveLineArrays(w,FirstLine,LineCount,'D'); // delete the cell lines
            
          // delete the text in the last line
          LastLine-=LineCount;
          MoveLineData(w,LastLine,0,LastCol+1,'D');
       }
         
       for (j=0;j<TotalCells;j++) {     // scroll the line numbers of other cells
          if (cell[j].InUse && cell[j].FirstLine>cell[cl].LastLine) {
             cell[j].FirstLine-=LineCount;
             cell[j].LastLine-=LineCount;
          }
       }
    }

    // adjust the current line
    CurLine=HilightBegRow;      // position at the first cell
    CurCol=0;
    if (CurLine>=TotalLines) CurLine=TotalLines-1;
    if (CurCol>=LineLen(CurLine)) CurCol=LineLen(CurLine)-1;
    if (CurCol<0) CurCol=0;

    TerArg.modified++;

    PaintTer(w);

    return true;
}


/******************************************************************************
    GetCellMinMaxWidth:
    Get the minimum and maximum width of a cell.
******************************************************************************/
GetCellMinMaxWidth(PTERWND w, int cl, LPLONG MinWidth, LPLONG MaxWidth, int TblWidth)
{
    int i;
    long min,max,WordWidth=0,ParaWidth=0,width;
    int  PrevFont=0,len,CurFmt,CurCell,level,FirstRow;
    long l,CellTextBytes=0;
    LPBYTE ptr,lead=NULL;
    LPWORD fmt;
    BOOL HasPicture=FALSE,NoWrap;
    BYTE PrevChar=0;

    // initialize the output
    min=max=0;
    level=cell[cl].level;


    for (l=cell[cl].FirstLine;l<=cell[cl].LastLine;l++) {
       // check if a subtable encountered
       CurCell=cid(l);
       if (cell[CurCell].level>level) {     // get the first row of the next immediate child
          FirstRow=0;
          for (;l<cell[cl].LastLine;l++) {
             CurCell=cid(l);
             if (CurCell==cl) break;        // back into the parent cell
             if (FirstRow && cell[CurCell].level==(level+1) && FirstRow!=cell[CurCell].row) {  // check if next table begins at the same level
                int row=cell[CurCell].row;
                if (row==0 || IsFirstTableRow(w,row)) break;  // out of subtable, or next subtable
             } 
             if (FirstRow==0 && cell[CurCell].level==(level+1)) FirstRow=cell[CurCell].row;
          }

          if (FirstRow>0) {
             long TblMinWidth,TblMaxWidth;
             if (GetTableMinMaxWidths(w,FirstRow,&TblMinWidth,&TblMaxWidth,NULL,NULL,NULL,NULL,0L)) { // last param 0 because embedded table width not yet available
                TblMinWidth=lMulDiv(TblMinWidth,PrtResX,1440);
                TblMaxWidth=lMulDiv(TblMaxWidth,PrtResX,1440);
                if (TblMinWidth>min) min=TblMinWidth;
                if (TblMaxWidth>max) max=TblMaxWidth;
             } 
             l--;
             continue;
          }  
       } 

       len=LineLen(l);
       CellTextBytes+=len;
       ptr=pText(l);
       if (mbcs) lead=OpenLead(w,l);
       fmt=OpenCfmt(w,l);
       NoWrap=PfmtId[pfmt(l)].pflags&PFLAG_NO_WRAP;

        
       for (i=0;i<=len;i++) {
          // check for the end of the word
          if ((!NoWrap) && i<len 
               && ((PrevChar==' ' && ptr[i]!=' ') || (TerFont[PrevFont].style&PICT)) ) {
             if (WordWidth>min) min=WordWidth;
             WordWidth=0;
          }
          // check for the end of the line
          if (i==len && NoWrap) {
             if (WordWidth>min) min=WordWidth;
             WordWidth=0;
          }
          // check for the end of the para
          if (i==len && LineFlags(l)&(LFLAG_PARA|LFLAG_LINE)) {
             if (ParaWidth>max) max=ParaWidth;
             if (WordWidth>min) min=WordWidth;
             ParaWidth=WordWidth=0;
          }
          if (i==len) break;

          // add to word and para width
          CurFmt=fmt[i];
          if (TerFont[CurFmt].style&PICT) HasPicture=TRUE;

          if (HasPicture && TerFont[CurFmt].FrameType!=PFRAME_NONE) {
              width=TwipsToPrtX(TerFont[CurFmt].PictWidth)+TwipsToPrtX(720);   // add tolerance
          }
          else {
              if (ptr[i]==CellChar && TerArg.ReadOnly && HasPicture && CellTextBytes==2)
                 width=0;
              else width=DblCharWidth(w,CurFmt,FALSE,ptr[i],LeadByte(lead,i));
          }

          WordWidth+=width;
          ParaWidth+=width;

          PrevChar=ptr[i];
          PrevFont=fmt[i];
       }
       CloseCfmt(w,l);
       if (mbcs) CloseLead(w,l);
    }

    // pass the results
    TblWidth=TblWidth*PrtResX/1440;  // in printer units
    
    max+=TwipsToPrtX(RULER_TOLERANCE);
    min+=TwipsToPrtX(RULER_TOLERANCE);

    if (max>TblWidth) max=TblWidth;
    if (max<min) max=min;
    
    (*MinWidth)=min*1440/PrtResX;    // convert to twips, can't use standard macros for long values
    (*MaxWidth)=max*1440/PrtResX;

    return TRUE;
}

/******************************************************************************
    SetCellLines:
    Set the first and last lines of the cells in the document.
******************************************************************************/
SetCellLines(PTERWND w)
{
    int cl,row,CurCell,ParentCell;
    long l;

    // reset InUse flag for all cells and rows
    for (row=0;row<TotalTableRows;row++) TableRow[row].InUse=FALSE;
    for (cl=0;cl<TotalCells;cl++) {
       cell[cl].FirstLine=cell[cl].LastLine=-1;
       cell[cl].InUse=FALSE;    // will be set to TRUE if text lines found for the cell
    }

    for (l=0;l<TotalLines;l++) {
       CurCell=cid(l);
       if (!tabw(l) || !(tabw(l)->type&INFO_ROW)) {
         if (cell[CurCell].FirstLine==-1) {          // set the first line
            cell[CurCell].FirstLine=l;
            // set the first lines for all parent cells also
            for (ParentCell=cell[CurCell].ParentCell;ParentCell>0;ParentCell=cell[ParentCell].ParentCell) {
               if (cell[ParentCell].FirstLine==-1) cell[ParentCell].FirstLine=cell[ParentCell].LastLine=l;
            }
         }
         cell[CurCell].LastLine=l;
         cell[CurCell].InUse=TRUE;
         row=cell[CurCell].row;
         TableRow[row].InUse=TRUE;
         
         if (cell[CurCell].level>0) {    // set in-use flag for the parent cells also
            for (cl=CurCell;cl>0;cl=cell[cl].ParentCell) {
               cell[cl].InUse=TRUE;
               row=cell[cl].row;
               TableRow[row].InUse=TRUE;
            }    
         }   
       }
    }


    return TRUE;
}

/******************************************************************************
    TerSetTableColWidth:
    Set the column width of the current column of the table.  Width is specified
    in twip units. The function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerSetTableColWidth(HWND hWnd,int width, BOOL repaint)
{
    int cl,CurCell;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    CurCell=cid(CurLine);
    if (CurCell==0) return FALSE;         // not positioned on a table
    if (width<0) return FALSE;

    // mark the selected cells
    HilightType=HILIGHT_OFF;              // remove highlighting to pickup only the current cell
    if (!MarkCells(w,IDC_TABLE_SEL_COLS)) return FALSE;

    // Change the cell width
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       cell[cl].width=width;
    }

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerInsertTableCol:
    Insert or append a column into the table.
    If the insert argument is TRUE, the new column is inserted before the
    current column, otherwise a new column is appended to the table.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerInsertTableCol(HWND hWnd,BOOL insert, BOOL AllRows, BOOL repaint)
{
    int cl,i,NewPfmt,CurCell,NewCell,LastCell,CurFont;
    long FirstLine,LastLine,InsertBefore,FontLine;
    int  GrpUndoRef;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    CurCell=cid(CurLine);
    if (CurCell==0) return FALSE;         // not positioned on a table

    // Get first and last line of each cell
    SetCellLines(w);

    // mark the selected cells
    HilightType=HILIGHT_OFF;              // remove highlighting to pickup only the current cell
    if (!insert) {                        // highlight the last cell of the row
       int CurRowId=cell[CurCell].row;
       LastCell=TableRow[CurRowId].LastCell;
       HilightType=HILIGHT_CHAR;
       HilightBegRow=cell[LastCell].FirstLine;
       HilightEndRow=cell[LastCell].LastLine;
       HilightBegCol=0;
       HilightEndCol=LineLen(HilightEndRow);
    }
    if (!MarkCells(w,(AllRows?IDC_TABLE_SEL_COLS:0))) return FALSE;
    HilightType=HILIGHT_OFF;

    // save the lines for undo
    GetTableSelRange(w,&FirstLine,&LastLine);
    GrpUndoRef=UndoRef;
    if (FirstLine!=-1) SaveUndo(w,FirstLine,0,LastLine,0,UNDO_ROW_DEL);


    // Insert or append a new column
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       if (!CheckLineLimit(w,TotalLines+1)) return TRUE;

       if ((NewCell=GetCellSlot(w,TRUE))==-1) continue;

       // create a new cell
       CopyCell(w,cl,NewCell);

       cell[NewCell].flags=ResetUintFlag(cell[NewCell].flags,(CFLAG_SEL1|CFLAG_SEL2));

       // connect the new cell to the chain
       if (insert) {
          if (cell[cl].PrevCell<0) {   // first cell of the chain
             int CurRowId=cell[cl].row;
             TableRow[CurRowId].FirstCell=NewCell;

             // connect to the chain
             cell[NewCell].PrevCell=-1;
             cell[NewCell].NextCell=cl;
             cell[cl].PrevCell=NewCell;
          }
          else {
             int PrevCell=cell[cl].PrevCell;

             // connect with the previous cell
             cell[PrevCell].NextCell=NewCell;
             cell[NewCell].PrevCell=PrevCell;

             // connect with the next cell
             cell[cl].PrevCell=NewCell;
             cell[NewCell].NextCell=cl;
          }
          InsertBefore=cell[cl].FirstLine;      // insert new cell line before this line
          NewPfmt=pfmt(InsertBefore);
          FontLine=InsertBefore;                // pickup font from thisline
       }
       else { // last cell of the chain
          int CurRowId=cell[cl].row;
          TableRow[CurRowId].LastCell=NewCell;

          // connect to the chain
          cell[NewCell].NextCell=-1;
          cell[NewCell].PrevCell=cl;
          cell[cl].NextCell=NewCell;

          InsertBefore=cell[cl].LastLine+1;  // insert new cell line before the row line
          NewPfmt=pfmt(cell[cl].LastLine);
          FontLine=cell[cl].LastLine;        // pickup font from this line

          // fix the pfmt id of the row line
          cid(InsertBefore)=NewCell;
       }

       // insert the cell marker line
       if (InputFontId>=0) CurFont=InputFontId;
       else                CurFont=TerGetCurFont(hTerWnd,FontLine,0);

       CurFont=GetTextFont(w,CurFont);     // turn-off things such picture, field, etc

       InsertMarkerLine(w,InsertBefore,CellChar,CurFont,NewPfmt,INFO_CELL,NewCell);
       if (CurLine>=InsertBefore) CurLine++;

       // fix the first/last line of the remaining cells
       for (i=cl;i<TotalCells;i++) {
          if (!(cell[i].InUse) || !(cell[i].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;
          if (cell[i].FirstLine>=InsertBefore) cell[i].FirstLine++;
          if (cell[i].LastLine>=InsertBefore)  cell[i].LastLine++;
       }

    }

    // fix the cell first/last lines
    SetCellLines(w);

    // save the lines for undo
    GetTableSelRange(w,&FirstLine,&LastLine);
    UndoRef=GrpUndoRef;
    if (FirstLine!=-1) SaveUndo(w,FirstLine,0,LastLine,0,UNDO_ROW_INS);

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
   GetTabelSelRange:
   Getthe first and last line of the selected range of lines for the current operation
*******************************************************************************/
BOOL GetTableSelRange(PTERWND w, LPLONG pFirstLine, LPLONG pLastLine)
{
   long FirstLine=(*pFirstLine);
   long LastLine=(*pLastLine);
   int cl;

   FirstLine=LastLine=-1;  // initialize the out variabels
  
   for (cl=0;cl<TotalCells;cl++) {
      if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;
      if (FirstLine==-1) FirstLine=cell[cl].FirstLine;
      if (cell[cl].FirstLine<FirstLine) FirstLine=cell[cl].FirstLine;
      if (cell[cl].LastLine>LastLine)   LastLine=cell[cl].LastLine;
   }

   (*pFirstLine)=FirstLine;
   (*pLastLine)=LastLine;

   return true;
}


/******************************************************************************
    TerRowHeight:
    Position the table row
******************************************************************************/
BOOL WINAPI _export TerRowHeight(HWND hWnd, int MinHeight, BOOL AllRows, BOOL refresh)
{
    int i,select;
    int cl,CurCell,PrevCell;
    int CurRowId;
    long l;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    CurCell=cid(CurLine);
    if (!TerArg.PrintView || CurCell==0) return TRUE;

    if (MinHeight==-1) {       // accept user parameters
       select=CallDialogBox(w,"RowHeightParam",RowHeightParam,0L);
       if (select==0) return TRUE;

       if (select==IDC_TABLE_ALL_ROWS) AllRows=TRUE;
       else                            AllRows=FALSE;

       // Get the minimum height from the temporary variable
       MinHeight=DlgInt1;
    }

    TerArg.modified++;

    // set the entire table
    if (AllRows) {
       CurCell=cid(CurLine);
       CurRowId=cell[CurCell].row;
       // find the first row of the table
       while (TRUE) {
          if (TableRow[CurRowId].PrevRow<=0) break;  // first row found
          CurRowId=TableRow[CurRowId].PrevRow;
       }

       SaveUndo(w,-1,TableRow[CurRowId].FirstCell,-1,0,UNDO_TABLE_ATTRIB);   // specify first cell only to save the entire table

       // set each row
       while (CurRowId>0) {
          TableRow[CurRowId].MinHeight=MinHeight;
          CurRowId=TableRow[CurRowId].NextRow;
       }

       if (refresh) PaintTer(w);
       return TRUE;
    }

    // reset the selection flag from the cell table
    for (i=0;i<TotalCells;i++) cell[i].flags=ResetUintFlag(cell[i].flags,(CFLAG_SEL1|CFLAG_SEL2));

    // collect the highlighted cells
    if (HilightType==HILIGHT_OFF) {
       CurCell=cid(CurLine);
       cell[CurCell].flags=cell[CurCell].flags|CFLAG_SEL1;
       
       SaveUndo(w,-1,CurCell,-1,CurCell,UNDO_TABLE_ATTRIB);   // specify current cell as first and last cell
    }
    else {
       if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block
       
       SaveUndo(w,HilightBegRow,0,HilightEndRow,0,UNDO_TABLE_ATTRIB);   // specify first and last line
       
       PrevCell=cid(HilightBegRow);
       cell[PrevCell].flags=cell[PrevCell].flags|CFLAG_SEL1;
       for (l=HilightBegRow+1;l<=HilightEndRow;l++) {
          if (!LineSelected(w,l)) continue;
          CurCell=cid(l);
          if (CurCell!=PrevCell) {
             PrevCell=CurCell;
             cell[PrevCell].flags=cell[PrevCell].flags|CFLAG_SEL1;
          }
       }
    }

    // Scan each cell and corresponding row to set the flag
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&CFLAG_SEL1)) continue;

       // set this row
       CurRowId=cell[cl].row;
       TableRow[CurRowId].MinHeight=MinHeight;  // Set the minimum height
    }


    if (refresh) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerSetRowTextFlow:
    Set rtl flow option for table rows
******************************************************************************/
BOOL WINAPI _export TerSetRowTextFlow(HWND hWnd, BOOL dialog, BOOL AllRows, int flow, BOOL refresh)
{
    int i,select;
    int cl,CurCell,PrevCell;
    int CurRowId;
    long l;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (flow!=FLOW_DEF && flow!=FLOW_RTL && flow!=FLOW_LTR) return FALSE;

    CurCell=cid(CurLine);
    if (!TerArg.PrintView || CurCell==0) return TRUE;

    CurRowId=cell[CurCell].row;

    if (dialog) {       // accept user parameters
       DlgInt1=TableRow[CurRowId].flow;

       select=CallDialogBox(w,"RowTextFlowParam",RowTextFlowParam,0L);
       if (select==0) return TRUE;

       if (select==IDC_TABLE_ALL_ROWS) AllRows=TRUE;
       else                            AllRows=FALSE;

       flow=DlgInt1;
    }

    TerArg.modified++;

    // set the entire table
    if (AllRows) {
       CurCell=cid(CurLine);
       CurRowId=cell[CurCell].row;
       // find the first row of the table
       while (TRUE) {
          if (TableRow[CurRowId].PrevRow<=0) break;  // first row found
          CurRowId=TableRow[CurRowId].PrevRow;
       }

       SaveUndo(w,-1,TableRow[CurRowId].FirstCell,-1,0,UNDO_TABLE_ATTRIB);   // specify first cell only to save the entire table

       // set each row
       while (CurRowId>0) {
          TableRow[CurRowId].flow=flow;
          CurRowId=TableRow[CurRowId].NextRow;
       }

       if (refresh) TerRepaginate(hTerWnd,TRUE);
       else         RequestPagination(w,TRUE);
       return TRUE;
    }

    // reset the selection flag from the cell table
    for (i=0;i<TotalCells;i++) cell[i].flags=ResetUintFlag(cell[i].flags,(CFLAG_SEL1|CFLAG_SEL2));

    // collect the highlighted cells
    if (HilightType==HILIGHT_OFF) {
       CurCell=cid(CurLine);
       cell[CurCell].flags=cell[CurCell].flags|CFLAG_SEL1;
       
       SaveUndo(w,-1,CurCell,-1,CurCell,UNDO_TABLE_ATTRIB);   // specify current cell as first and last cell
    }
    else {
       if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block
       
       SaveUndo(w,HilightBegRow,0,HilightEndRow,0,UNDO_TABLE_ATTRIB);   // specify first and last line
       
       PrevCell=cid(HilightBegRow);
       cell[PrevCell].flags=cell[PrevCell].flags|CFLAG_SEL1;
       for (l=HilightBegRow+1;l<=HilightEndRow;l++) {
          if (!LineSelected(w,l)) continue;
          CurCell=cid(l);
          if (CurCell!=PrevCell) {
             PrevCell=CurCell;
             cell[PrevCell].flags=cell[PrevCell].flags|CFLAG_SEL1;
          }
       }
    }

    // Scan each cell and corresponding row to set the flag
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&CFLAG_SEL1)) continue;

       // set this row
       CurRowId=cell[cl].row;
       TableRow[CurRowId].flow=flow;  // Set the minimum height
    }


    if (refresh) TerRepaginate(hTerWnd,TRUE);
    else         RequestPagination(w,TRUE);

    return TRUE;
}

/******************************************************************************
    TerGetRowInfo:
    Get table row information.  To specify a cell id, use the negative value
    for the RowId parameter.
******************************************************************************/
BOOL WINAPI _export TerGetRowInfo(HWND hWnd, int RowId, LPINT height, LPINT MinHeight,
                                  LPINT FixWidth, LPINT PrevRow, LPINT NextRow,
                                  LPINT indent, LPINT flags, LPINT border, LPINT CurWidth)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (RowId<0) {
       int CellId=-RowId;
       if (CellId>=TotalCells || !cell[CellId].InUse) return FALSE;
       RowId=cell[CellId].row;
    }

    if (RowId>=TotalTableRows || !TableRow[RowId].InUse) return FALSE;

    if (height) (*height)=TableRow[RowId].height;
    if (MinHeight) (*MinHeight)=TableRow[RowId].MinHeight;
    if (FixWidth) (*FixWidth)=TableRow[RowId].FixWidth;
    if (PrevRow) (*PrevRow)=TableRow[RowId].PrevRow;
    if (NextRow) (*NextRow)=TableRow[RowId].NextRow;
    if (indent) (*indent)=TableRow[RowId].indent;
    if (flags) (*flags)=TableRow[RowId].flags;
    if (border) (*border)=TableRow[RowId].border;

    // get the table row width
    if (CurWidth) {
       int width=0;
       int cl=TableRow[RowId].FirstCell;
       while (cl>0) {
          width+=cell[cl].width;
          cl=cell[cl].NextCell;
       }
       (*CurWidth)=width;
    }

    return  TRUE;
}

/******************************************************************************
    TerRowPosition:
    Position the table row
******************************************************************************/
BOOL WINAPI _export TerRowPosition(HWND hWnd, UINT JustFlag, BOOL AllRows, BOOL refresh)
{
   return TerRowPositionEx(hWnd,JustFlag,0,AllRows,refresh);
}

/******************************************************************************
    TerRowPositionEx:
    Position the table row.  The JustFlag is ignored when indent is non-zero
******************************************************************************/
BOOL WINAPI _export TerRowPositionEx(HWND hWnd, UINT JustFlag, int indent, BOOL AllRows, BOOL refresh)
{
    int i,select;
    int cl,CurCell,PrevCell;
    int CurRowId;
    long l;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    CurCell=cid(CurLine);
    if (!TerArg.PrintView || CurCell==0) return TRUE;

    if (JustFlag==0 && indent==0) { // accept user parameters
       select=CallDialogBox(w,"RowPositionParam",RowPositionParam,0L);
       if (select==0) return TRUE;

       if (select==IDC_TABLE_ALL_ROWS) AllRows=TRUE;
       else                            AllRows=FALSE;

       // set the justification flag
       JustFlag=TempString[0];         // justification code sent via this string
       if (JustFlag==0) JustFlag=LEFT;
    }
    if (indent) JustFlag=LEFT;

    TerArg.modified++;
    if (CurLine<RepageBeginLine) RepageBeginLine=CurLine;

    // set the entire table
    if (AllRows) {
       CurCell=cid(CurLine);
       CurRowId=cell[CurCell].row;
       // find the first row of the table
       while (TRUE) {
          if (TableRow[CurRowId].PrevRow<=0) break;  // first row found
          CurRowId=TableRow[CurRowId].PrevRow;
       }

       SaveUndo(w,-1,TableRow[CurRowId].FirstCell,-1,0,UNDO_TABLE_ATTRIB);   // specify first cell only to save the entire table

       // set each row
       while (CurRowId>0) {
          TableRow[CurRowId].flags=ResetUintFlag(TableRow[CurRowId].flags,(CENTER|RIGHT_JUSTIFY));
          TableRow[CurRowId].flags=TableRow[CurRowId].flags|JustFlag;
          TableRow[CurRowId].indent=indent;

          CurRowId=TableRow[CurRowId].NextRow;
       }

       if (refresh) Repaginate(w,FALSE,TRUE,0,TRUE);
       return TRUE;
    }

    // reset the selection flag from the cell table
    for (i=0;i<TotalCells;i++) cell[i].flags=ResetUintFlag(cell[i].flags,(CFLAG_SEL1|CFLAG_SEL2));

    // collect the highlighted cells
    if (HilightType==HILIGHT_OFF) {
       CurCell=cid(CurLine);
       cell[CurCell].flags=cell[CurCell].flags|CFLAG_SEL1;
       SaveUndo(w,-1,CurCell,-1,CurCell,UNDO_TABLE_ATTRIB);   // specify current cell as first and last cell
    }
    else {
       if (!NormalizeBlock(w)) return FALSE;  // normalize beginning and ending of the block
       
       SaveUndo(w,HilightBegRow,0,HilightEndRow,0,UNDO_TABLE_ATTRIB);   // specify first and last line

       PrevCell=cid(HilightBegRow);
       cell[PrevCell].flags=cell[PrevCell].flags|CFLAG_SEL1;
       for (l=HilightBegRow+1;l<=HilightEndRow;l++) {
          if (!LineSelected(w,l)) continue;
          CurCell=cid(l);
          if (CurCell!=PrevCell) {
             PrevCell=CurCell;
             cell[PrevCell].flags=cell[PrevCell].flags|CFLAG_SEL1;
          }
       }
    }

    // Scan each cell and corresponding row to set the flag
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&CFLAG_SEL1)) continue;

       // set this row
       CurRowId=cell[cl].row;
       TableRow[CurRowId].flags=ResetUintFlag(TableRow[CurRowId].flags,(CENTER|RIGHT_JUSTIFY));
       TableRow[CurRowId].flags=TableRow[CurRowId].flags|JustFlag;
       TableRow[CurRowId].indent=indent;   // indenation will be recalculated
    }

    if (refresh) Repaginate(w,FALSE,TRUE,0,TRUE);

    return TRUE;
}

