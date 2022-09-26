/*===============================================================================
   TER_TBL2.C
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
     TerGetTableId:
     Get the id for a table row.
******************************************************************************/
int WINAPI _export TerGetTableId(HWND hWnd, int row)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data
 
    if (row<0) {
      int cl=cid(CurLine);
      if (cl==0) return -1;
      else row=cell[cl].row;
    }
    
    if (!TableRow[row].InUse) return -1;

    if (TableRow[row].id<0) return TableRow[row].id;  // row specific id

    while (!IsFirstTableRow(w,row)) row=TableRow[row].PrevRow;   // id applicable to the whole table

    return TableRow[row].id;
}
 
/******************************************************************************
     TerSetTableId:
     Get the id for a table row.
******************************************************************************/
BOOL WINAPI _export TerSetTableId(HWND hWnd, int row, int id)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data
 
    if (row<0) {
      int cl=cid(CurLine);
      if (cl==0) return FALSE;
      else row=cell[cl].row;
    }
    
    if (!TableRow[row].InUse) return FALSE;

    if (id<0) {     // row specific id
       TableRow[row].id=id;
       return TRUE;
    } 

    while (!IsFirstTableRow(w,row)) row=TableRow[row].PrevRow;  // get the first table row

    while(TRUE) {               // assign the new id
      TableRow[row].id=id;
      if (IsLastTableRow(w,row)) break;
      row=TableRow[row].NextRow;
    }

    return TRUE;
}
 
/******************************************************************************
     TerPosTable:
     Position at the specified table number, table row, and table column.
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosTable(HWND hWnd, int TableNo, int RowNo, int ColNo, int pos, BOOL repaint)
{
    return TerPosTable2(hWnd,TableNo,RowNo,ColNo,pos,0,repaint);
}

/******************************************************************************
     TerPosTable2:
     Position at the specified table number, table row, and table column.
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosTable2(HWND hWnd, int TableNo, int RowNo, int ColNo, int pos, int ParentCell, BOOL repaint)
{
    return TerPosTable4(hWnd,0,TableNo,RowNo,ColNo,pos,ParentCell,repaint);
}

/******************************************************************************
     TerPosTable2:
     Position at the specified table number, table row, and table column.
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosTable3(HWND hWnd, int TableId, int RowNo, int ColNo, int pos,BOOL repaint)
{
   return TerPosTable4(hWnd,TableId,0,RowNo,ColNo,pos,0,repaint);
}
 
/******************************************************************************
     TerPosTable4:
     Position at the specified table number, table row, and table column.
     The 'pos' should be either POS_BEG or POS_END
******************************************************************************/
BOOL WINAPI _export TerPosTable4(HWND hWnd, int TableId, int TableNo, int RowNo, int ColNo, int pos, int ParentCell, BOOL repaint)
{
    PTERWND w;
    long l,FirstLine,LastLine;
    int row,col,CellId,CurTableNo,CurRowNo,CurColNo,ParentLevel=0,CurLevel;
    BOOL InTable=FALSE;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;
    if (pos!=POS_BEG && pos!=POS_END) return FALSE;  // bad argument

    // find the first parent line
    if (TableId!=0) {                        // find the table with the given id
       for (l=0;l<TotalLines;l++) {
         CellId=cid(l);
         if (CellId==0) continue;
         row=cell[CellId].row;
         if (TableRow[row].id==TableId) break;
       }
       if (l==TotalLines) return FALSE;   // table id not found
       FirstLine=l; 
       LastLine=TotalLines-1; 
       CurLevel=cell[CellId].level;
    } 
    else if (ParentCell>0) {
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
       CurLevel=0;
    }

    // position at the table number
    if (TableId!=0) l=FirstLine;
    else if (TableNo>=0) {
       l=FirstLine; 
       CurTableNo=-1;
       while (l<=LastLine) {
          if (!InTable && !InOuterLevels(w,CurLevel,l)) {          // new table begins?
             CurTableNo++;
             if (CurTableNo==TableNo) break;
             else InTable=TRUE;
          }
          if (InTable && InOuterLevels(w,CurLevel,l)) InTable=FALSE;  // table ended
          l++;
       }
       if (l>=LastLine) return FALSE;   // table not found
    }
    else {                                // position at the first line of the current table
       if (cid(CurLine)==0) return FALSE; // not positioned in the table
       if (ParentCell>0 && InOuterLevels(w,CurLevel,CurLine)) return FALSE;  // situated outside the requested level cell
       CellId=LevelCell(w,CurLevel,CurLine);
       row=cell[CellId].row;
       while (!IsFirstTableRow(w,row)) row=TableRow[row].PrevRow;
       CellId=TableRow[row].FirstCell;    // first cell of the table
       l=CurLine;                         // position at the first line of the cell
       while (l>=0 && !InOuterLevels(w,CurLevel,l) && LevelCell(w,CurLevel,l)!=CellId) l--;  // go into the first cell
       if (l<0 || InOuterLevels(w,CurLevel,l)) return FALSE;// out of the table
       while (l>=0 && !InOuterLevels(w,CurLevel,l) && LevelCell(w,CurLevel,l)==CellId) l--;  // go paste the cell
       l++;                               // first line of the table
    }

    // position at the table row
    if (RowNo>0) {
       CurRowNo=0;
       while (l<=LastLine) {
          if (InOuterLevels(w,CurLevel,l)) return FALSE; // row not found

          if (tabw(l) && tabw(l)->type&INFO_ROW && TableLevel(w,l)==CurLevel) {
             CurRowNo++;
             if (CurRowNo==RowNo) {    // row located
                l++;                   // go past the row delimiter
                break;
             }
          }
          l++;
       }
    }
    if (InOuterLevels(w,CurLevel,l)) return FALSE;    // out of the table rows

    // position at the table cell
    if (pos==POS_END) ColNo++;     // position at the next col
    if (ColNo>0) {
       CurColNo=0;
       while (l<=LastLine) {
          if (tabw(l) && tabw(l)->type&INFO_ROW && TableLevel(w,l)==CurLevel) return FALSE;  // cell not found

          if (tabw(l) && tabw(l)->type&INFO_CELL && TableLevel(w,l)==CurLevel) {
             CurColNo++;
             if (CurColNo==ColNo) {
                l++;              // go past the cell marker line
                if (pos==POS_BEG && l<TotalLines && LineInfo(w,l,INFO_ROW) && TableLevel(w,l)==CurLevel) return FALSE;  // column beyond the row
                break;            // row located
             }
          }
          l++;
       }
    }

    // position the cursor
    if (pos==POS_BEG) SetTerCursorPos(hWnd,l,0,repaint);
    else {
       col=LineLen(l-1)-1;        // column to position at
       if (col<0) col=0;
       SetTerCursorPos(hWnd,l-1,col,repaint);
    }

    return TRUE;
}

/******************************************************************************
     TerPosAfterTable:
     Position after the current table.
******************************************************************************/
BOOL WINAPI _export TerPosAfterTable(HWND hWnd, BOOL OuterMost, BOOL repaint)
{
    PTERWND w;
    long l;
    int CellId,CurLevel;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!TerArg.PageMode) return FALSE;

    CellId=cid(CurLine);
    if (CellId==0) return FALSE;         
    CurLevel=cell[CellId].level;

    for (l=CurLine+1;l<TotalLines;l++) {
       CellId=cid(l);
       if (CellId==0) break;   // output of the table

       if (!OuterMost && cell[CellId].level<CurLevel) break;
    } 

    SetTerCursorPos(hWnd,l,0,repaint);

    return TRUE;
}

/******************************************************************************
    TerInsertTableRow:
    Insert a row before the current table row.  The new row is created before
    the current row.
******************************************************************************/
BOOL WINAPI _export TerInsertTableRow(HWND hWnd, BOOL insert, BOOL repaint)
{
    int PrevCell,CurCell,FirstCell,LastCell,CellId,CellCount,cl;
    int CurPara,CurRowId,PrevRowId,OrigRowId;
    int NextRowCell;
    int  AddLines;
    int  j,GrpUndoRef;
    WORD CurFont;
    PTERWND w;
    long SaveLine,OrigLine;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    CurCell=cid(CurLine);
    if (!TerArg.PrintView || CurCell==0) return TRUE;

    // save the parent row for undo
    GrpUndoRef=UndoRef;
    if (cell[CurCell].level>0) {
       UndoRef=GrpUndoRef;
       SaveUndo(w,CurLine,0,CurLine,0,UNDO_ROW_DEL);
    } 

    // get the insertion row
    OrigRowId=cell[CurCell].row;
    if (!insert) {      // find the last row for the table
       while (TRUE) {
          if (TableRow[OrigRowId].NextRow<0) break;
          OrigRowId=TableRow[OrigRowId].NextRow;
       }

    }

    // find the number of cells in the insertion row
    CellCount=0;
    CellId=TableRow[OrigRowId].FirstCell;
    while (CellId>0) {
       CellCount++;
       CellId=cell[CellId].NextCell;
    }

    // Set first and last line of each cell
    SetCellLines(w);

    // decide number of lines to add
    AddLines=CellCount+1;       // number of cell  markers plus one row marker line

    if (!CheckLineLimit(w,TotalLines+AddLines)) return TRUE;
    
    CurFont=(WORD)GetEffectiveCfmt(w);
    if (TerFont[CurFont].FieldId>0) CurFont=SetFontFieldId(w,CurFont,0,NULL);


    // add a row id
    if ((CurRowId=GetTableRowSlot(w))==-1) return TRUE;

    // copy the row property from the next cell
    HugeMove(&(TableRow[OrigRowId]),&(TableRow[CurRowId]),sizeof(struct StrTableRow));
    HugeMove(&(TableAux[OrigRowId]),&(TableAux[CurRowId]),sizeof(struct StrTableAux));

    TableRow[CurRowId].FirstCell=0;
    TableRow[CurRowId].LastCell=0;

    // prepare for row chain
    if (insert) {
       PrevRowId=TableRow[OrigRowId].PrevRow;

       TableRow[CurRowId].PrevRow=PrevRowId;   // backward chain
       if (PrevRowId>0) TableRow[PrevRowId].NextRow=CurRowId;

       TableRow[CurRowId].NextRow=OrigRowId;   // forward chain
       TableRow[OrigRowId].PrevRow=CurRowId;

       ResetUintFlag(TableRow[OrigRowId].flags,ROWFLAG_NEW_TABLE);  // because the new row is inserted before this row
    }
    else {
       TableRow[CurRowId].PrevRow=OrigRowId;   // back chain
       TableRow[OrigRowId].NextRow=CurRowId;

       TableRow[CurRowId].NextRow=-1;          // last row of the table
    }


    // find line before which new text lines are inserted
    if (insert) {
       FirstCell=TableRow[OrigRowId].FirstCell;
       CurLine=cell[FirstCell].FirstLine;
    }
    else {
       FirstCell=LastCell=TableRow[OrigRowId].LastCell;
       CurLine=cell[LastCell].LastLine;
       CurLine=CurLine+2;   // go past the last cell line and the row marker line
       SaveLine=CurLine;
    }
    
    // save the paragraph id for each cell
    for (cl=FirstCell;cl>0;cl=cell[cl].NextCell) CellAux[cl].TempPfmt=pfmt(cell[cl].LastLine);
    
    CurCol=0;

    NextRowCell=TableRow[OrigRowId].FirstCell;

    PrevCell=-1;

    for (j=0;j<CellCount;j++) {       // add cell breaks
       if ((CurCell=GetCellSlot(w,TRUE))==-1) continue;

       // copy the cell properties from the next row cell
       CopyCell(w,NextRowCell,CurCell);
       OrigLine=cell[NextRowCell].LastLine;
       CurPara=CellAux[NextRowCell].TempPfmt;

       NextRowCell=cell[NextRowCell].NextCell;

       cell[CurCell].InUse=TRUE;
       cell[CurCell].row=CurRowId;
       cell[CurCell].PrevCell=PrevCell;
       cell[CurCell].NextCell=-1;

       // connect to the previous cell
       if (PrevCell>0) cell[PrevCell].NextCell=CurCell;
       PrevCell=CurCell;

       // connect to the table
       if (TableRow[CurRowId].FirstCell==0) TableRow[CurRowId].FirstCell=CurCell;
       TableRow[CurRowId].LastCell=CurCell;

       // create the cell break line
       InsertMarkerLine(w,CurLine,CellChar,CurFont,CurPara,INFO_CELL,CurCell);
       CurLine++;
    }

    // create the row break line
    InsertMarkerLine(w,CurLine,ROW_CHAR,CurFont,pfmt(CurLine),INFO_ROW,cid(CurLine-1));
    CurLine++;

    // save for undo
    UndoRef=GrpUndoRef;
    SaveUndo(w,CurLine-1,0,CurLine-1,0,UNDO_ROW_INS);

    if (!insert) {
       CurLine=SaveLine;
       CurCol=0;
    }

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerCellBorder:
    Select Cell Border.  The 'select' can be set to SEL_CELLS, SEL_COLS,
    SEL_ROWS, SEL_ALL.  Set 'select' to 0 to invoke the dialog box.  Set any of
    the width variables to -1 to leave the original values unchanged.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerCellBorder(HWND hWnd,int select, int TopWidth, int BotWidth, int LeftWidth, int RightWidth,BOOL repaint)
{
    return TerCellBorder2(hWnd,select,TopWidth,BotWidth,LeftWidth,RightWidth,false,repaint);
}

/******************************************************************************
    TerCellBorder:
    Select Cell Border.  The 'select' can be set to SEL_CELLS, SEL_COLS,
    SEL_ROWS, SEL_ALL.  Set 'select' to 0 to invoke the dialog box.  Set any of
    the width variables to -1 to leave the original values unchanged.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerCellBorder2(HWND hWnd,int select, int TopWidth, int BotWidth, int LeftWidth, int RightWidth,BOOL outline, BOOL repaint)
{
    int i;
    int cl,CurCell,margin,SaveMargin;
    int  SaveWidth[4],width[4];
    BOOL TableHilighted=FALSE;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TableHilighted=TRUE;
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && !TableHilighted)) return TRUE;
    if (CurCell==0 && select==SEL_ALL) return TRUE;  // not positioned on a table


    // save the width of the current cell border
    for (i=0;i<4;i++) SaveWidth[i]=cell[CurCell].BorderWidth[i];
    SaveMargin=cell[CurCell].margin;

    if (select<=0) {
       select=CallDialogBox(w,"CellBorderParam",CellBorderParam,0L);
       outline=DlgInt1;
    }
    else {
       if      (select==SEL_CELLS) select=IDC_TABLE_SEL_CELLS;
       else if (select==SEL_COLS)  select=IDC_TABLE_SEL_COLS;
       else if (select==SEL_ROWS)  select=IDC_TABLE_SEL_ROWS;
       else if (select==SEL_ALL)   select=IDC_TABLE_ALL;
       else select=0;

       if (select>0) {       // set the width variables
          cell[CurCell].BorderWidth[BORDER_INDEX_TOP]=TopWidth;
          cell[CurCell].BorderWidth[BORDER_INDEX_BOT]=BotWidth;
          cell[CurCell].BorderWidth[BORDER_INDEX_LEFT]=LeftWidth;
          cell[CurCell].BorderWidth[BORDER_INDEX_RIGHT]=RightWidth;
       }
    }

    // restore the current cell width and extract the new width
    for (i=0;i<4;i++) {
       width[i]=cell[CurCell].BorderWidth[i];
       cell[CurCell].BorderWidth[i]=SaveWidth[i];
    }
    margin=cell[CurCell].margin;
    cell[CurCell].margin=SaveMargin;
    TableRow[cell[CurCell].row].CellMargin=SaveMargin;


    if (select==0) return TRUE;

    // mark the selected cells
    if (!MarkCells(w,select)) return FALSE;
    
    SaveUndo(w,0,0,0,0,UNDO_TABLE_ATTRIB);   // save for undo of marked cells

    //  Set the border for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       // set the margin
       cell[cl].margin=margin;
       TableRow[cell[cl].row].CellMargin=margin;

       // set borders. if the width is -1, leave the border as it is
       if (width[BORDER_INDEX_TOP]>=0) {
          cell[cl].BorderWidth[BORDER_INDEX_TOP]=min(margin,width[BORDER_INDEX_TOP]);
          if (width[BORDER_INDEX_TOP]>0) {
             BOOL apply=true;
             if (outline) {
                int PrevCell=GetPrevCellInColumnPos(w,cl,false);
                if (PrevCell>0 && True(cell[PrevCell].flags&(CFLAG_SEL1|CFLAG_SEL2))) apply=false;
             } 
             if (apply) cell[cl].border|=BORDER_TOP;
          }
          else cell[cl].border=ResetUintFlag(cell[cl].border,BORDER_TOP);
       }
       if (width[BORDER_INDEX_BOT]>=0) {
          cell[cl].BorderWidth[BORDER_INDEX_BOT]=min(margin,width[BORDER_INDEX_BOT]);
          if (width[BORDER_INDEX_BOT]>0) {
             BOOL apply=true;
             if (outline) {
                int NextCell=GetNextCellInColumnPos(w,cl);
                if (NextCell>0 && True(cell[NextCell].flags&(CFLAG_SEL1|CFLAG_SEL2))) apply=false;
             } 
             if (apply) cell[cl].border|=BORDER_BOT;
          }
          else cell[cl].border=ResetUintFlag(cell[cl].border,BORDER_BOT);
       }
       if (width[BORDER_INDEX_LEFT]>=0) {
          cell[cl].BorderWidth[BORDER_INDEX_LEFT]=min(margin,width[BORDER_INDEX_LEFT]);
          if (width[BORDER_INDEX_LEFT]>0) {
             BOOL apply=true;
             if (outline) {
                int PrevCell=cell[cl].PrevCell;
                if (PrevCell>0 && True(cell[PrevCell].flags&(CFLAG_SEL1|CFLAG_SEL2))) apply=false;
             } 
             if (apply) cell[cl].border|=BORDER_LEFT;
          }
          else  cell[cl].border=ResetUintFlag(cell[cl].border,BORDER_LEFT);
       }
       if (width[BORDER_INDEX_RIGHT]>=0) {
          cell[cl].BorderWidth[BORDER_INDEX_RIGHT]=min(margin,width[BORDER_INDEX_RIGHT]);
          if (width[BORDER_INDEX_RIGHT]>0) {
             BOOL apply=true;
             if (outline) {
                int NextCell=cell[cl].NextCell;
                if (NextCell>0 && True(cell[NextCell].flags&(CFLAG_SEL1|CFLAG_SEL2))) apply=false;
             } 
             if (apply) cell[cl].border|=BORDER_RIGHT;
          }
          else  cell[cl].border=ResetUintFlag(cell[cl].border,BORDER_RIGHT);
       }
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerTableOutlineBorder:
    Set table outline border and color
******************************************************************************/
BOOL WINAPI _export TerTableOutlineBorder(HWND hWnd,int CurCell, int width, COLORREF color, BOOL repaint)
{
    int cl,row,FirstRow,LastRow;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (CurCell<=0 || CurCell>=TotalCells) return false;

    row=cell[CurCell].row;
    
    while (row>0 && !IsFirstTableRow(w,row)) {
       if (TableRow[row].PrevRow<=0) break;
       row=TableRow[row].PrevRow;
    }
    FirstRow=row;
    
    while (row>0 && !IsLastTableRow(w,row)) {
       if (TableRow[row].NextRow<=0) break;
       row=TableRow[row].NextRow;
    }
    LastRow=row;

    // set the top border
    cl=TableRow[FirstRow].FirstCell;
    while (cl>0) {
       cell[cl].BorderWidth[BORDER_INDEX_TOP]=min(cell[cl].margin,width);
       cell[cl].BorderColor[BORDER_INDEX_TOP]=color;
       cell[cl].border|=BORDER_TOP;

       cl=cell[cl].NextCell;
    } 

    // set the bottom border
    cl=TableRow[LastRow].FirstCell;
    while (cl>0) {
       cell[cl].BorderWidth[BORDER_INDEX_BOT]=min(cell[cl].margin,width);
       cell[cl].BorderColor[BORDER_INDEX_BOT]=color;
       cell[cl].border|=BORDER_BOT;
       cl=cell[cl].NextCell;
    } 

    // set the left/right border
    row=FirstRow;
    while (row>0) {
       cl=TableRow[row].FirstCell;
       cell[cl].BorderWidth[BORDER_INDEX_LEFT]=min(cell[cl].margin,width);
       cell[cl].BorderColor[BORDER_INDEX_LEFT]=color;
       cell[cl].border|=BORDER_LEFT;

       while (cl>0) {
          if (cell[cl].NextCell<=0) {  // last cell
             cell[cl].BorderWidth[BORDER_INDEX_RIGHT]=min(cell[cl].margin,width);
             cell[cl].BorderColor[BORDER_INDEX_RIGHT]=color;
             cell[cl].border|=BORDER_RIGHT;
             break;
          }
          cl=cell[cl].NextCell; 
       }
       row=TableRow[row].NextRow; 
    } 

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerCellBorderColor:
    Select Cell Border color.  The 'select' can be set to SEL_CELLS, SEL_COLS,
    SEL_ROWS, SEL_ALL.  Set 'select' to 0 to invoke the dialog box.
    This function returns TRUE when successful.
******************************************************************************/
BOOL WINAPI _export TerCellBorderColor(HWND hWnd,int select, COLORREF TopColor, COLORREF BotColor, COLORREF LeftColor, COLORREF RightColor,BOOL repaint)
{
    int i;
    int cl,CurCell;
    COLORREF SaveColor[4],color[4];
    BOOL TableHilighted=FALSE;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TableHilighted=TRUE;
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && !TableHilighted)) return TRUE;
    if (CurCell==0 && select==SEL_ALL) return TRUE;  // not positioned on a table


    // save the width of the current cell border
    for (i=0;i<4;i++) SaveColor[i]=cell[CurCell].BorderColor[i];

    if (select<=0) {
       select=CallDialogBox(w,"CellBorderColorParam",CellBorderColorParam,0L);
    }
    else {
       if      (select==SEL_CELLS) select=IDC_TABLE_SEL_CELLS;
       else if (select==SEL_COLS)  select=IDC_TABLE_SEL_COLS;
       else if (select==SEL_ROWS)  select=IDC_TABLE_SEL_ROWS;
       else if (select==SEL_ALL)   select=IDC_TABLE_ALL;
       else select=0;

       if (select>0) {       // set the width variables
          cell[CurCell].BorderColor[BORDER_INDEX_TOP]=TopColor;
          cell[CurCell].BorderColor[BORDER_INDEX_BOT]=BotColor;
          cell[CurCell].BorderColor[BORDER_INDEX_LEFT]=LeftColor;
          cell[CurCell].BorderColor[BORDER_INDEX_RIGHT]=RightColor;
       }
    }

    // restore the current cell width and extract the new width
    for (i=0;i<4;i++) {
       color[i]=cell[CurCell].BorderColor[i];
       cell[CurCell].BorderColor[i]=SaveColor[i];
    }

    if (select==0) return TRUE;

    // mark the selected cells
    if (!MarkCells(w,select)) return FALSE;
    
    SaveUndo(w,0,0,0,0,UNDO_TABLE_ATTRIB);   // save for undo of marked cells

    //  Set the border for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       // set borders. if the width is -1, leave the border as it is
       cell[cl].BorderColor[BORDER_INDEX_TOP]=color[BORDER_INDEX_TOP];
       cell[cl].BorderColor[BORDER_INDEX_BOT]=color[BORDER_INDEX_BOT];
       cell[cl].BorderColor[BORDER_INDEX_LEFT]=color[BORDER_INDEX_LEFT];
       cell[cl].BorderColor[BORDER_INDEX_RIGHT]=color[BORDER_INDEX_RIGHT];
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}


/******************************************************************************
    TerCellWidth:
    Select Cell width and cell maing values.  Set the width and margin to leave those values unchanged
******************************************************************************/
BOOL WINAPI _export TerCellWidth(HWND hWnd,int select, int width, int margin, BOOL repaint)
{
    int cl,CurCell;
    BOOL TableHilighted=FALSE;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TableHilighted=TRUE;
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && !TableHilighted)) return TRUE;
    if (CurCell==0 && select==SEL_ALL) return TRUE;  // not positioned on a table


    if (select<=0) {
       select=CallDialogBox(w,"CellWidthParam",CellWidthParam,0L);
       width=DlgInt1;
       margin=DlgInt2;
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

    //  Set the border for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       // set borders. if the width is -1, leave the border as it is
       if (width>0) cell[cl].width=cell[cl].FixWidth=width;
       if (margin>0) {
          cell[cl].margin=margin;
          TableRow[cell[cl].row].CellMargin=margin;
       }

       if (cell[cl].width<3*cell[cl].margin) cell[cl].FixWidth=cell[cl].width=3*cell[cl].margin;  // make cell width enough to provide the margin area

       

       // check the border width
       if (margin>0) {
          if (cell[cl].border&BORDER_LEFT && cell[cl].BorderWidth[BORDER_INDEX_LEFT]>margin) cell[cl].BorderWidth[BORDER_INDEX_LEFT]=margin;
          if (cell[cl].border&BORDER_RIGHT && cell[cl].BorderWidth[BORDER_INDEX_RIGHT]>margin) cell[cl].BorderWidth[BORDER_INDEX_RIGHT]=margin;
          if (cell[cl].border&BORDER_TOP && cell[cl].BorderWidth[BORDER_INDEX_TOP]>margin) cell[cl].BorderWidth[BORDER_INDEX_TOP]=margin;
          if (cell[cl].border&BORDER_BOT && cell[cl].BorderWidth[BORDER_INDEX_BOT]>margin) cell[cl].BorderWidth[BORDER_INDEX_BOT]=margin;
       }    
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}


/******************************************************************************
    TerCellShading:
    Select Cell Shading
******************************************************************************/
BOOL WINAPI _export TerCellShading(HWND hWnd, int select, int shading, BOOL repaint)
{
    int cl,CurCell;
    int  SaveShading;
    BOOL TableHilighted=FALSE;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TableHilighted=TRUE;
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && !TableHilighted)) return TRUE;


    // save the width of the current cell shading

    if (select<=0) {
       SaveShading=cell[CurCell].shading;

       select=CallDialogBox(w,"CellShadingParam",CellShadingParam,0L);

       shading=cell[CurCell].shading;      // restore the cell shading percentage
       cell[CurCell].shading=SaveShading;
    }
    else {
       if (shading<0) shading=0;
       if (shading>100) shading=100;

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

    HilightType=HILIGHT_OFF;     // remove highlighting so shading can be seen

    //  Set the shading for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       cell[cl].shading=shading;
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerCellVertAlign:
    Select Cell vertical Alignment.  Set 'align' to 0 for top alignment or
    set it to CFLAG_VALIGN_CTR or CFLAG_VALIGN_BOT.
******************************************************************************/
BOOL WINAPI _export TerCellVertAlign(HWND hWnd, int select, int align, BOOL repaint)
{
    int cl,CurCell;
    int  SaveAlign;
    BOOL TableHilighted=FALSE;
    UINT AlignFlags=(CFLAG_VALIGN_CTR|CFLAG_VALIGN_BOT|CFLAG_VALIGN_BASE);
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TableHilighted=TRUE;
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && !TableHilighted)) return TRUE;


    // save the width of the current cell shading

    if (select<=0) {
       SaveAlign=(cell[CurCell].flags&AlignFlags);

       select=CallDialogBox(w,"CellVertAlignParam",CellVertAlignParam,0L);

       align=(cell[CurCell].flags&AlignFlags);   // user entered value

       cell[CurCell].flags=ResetUintFlag(cell[CurCell].flags,AlignFlags);
       cell[CurCell].flags|=SaveAlign;
    }
    else {
       if (align!=CFLAG_VALIGN_CTR && align!=CFLAG_VALIGN_BOT && align!=CFLAG_VALIGN_BASE) align=0;

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
    
    HilightType=HILIGHT_OFF;     // remove highlighting so shading can be seen

    //  Set the shading for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       cell[cl].flags=ResetUintFlag(cell[cl].flags,AlignFlags);
       cell[cl].flags|=align;
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;
    if (align==CFLAG_VALIGN_BASE) {
       if (RepageBeginLine>CurLine) RepageBeginLine=CurLine;
       RequestPagination(w,FALSE);      // row height might change
    }

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    TerCellRotateText:
    Select Cell vertical Alignment.  Set 'direction' to the TEXT_ constants.
******************************************************************************/
BOOL WINAPI _export TerCellRotateText(HWND hWnd, int select, int direction, BOOL repaint)
{
    int cl,CurCell;
    int  angle;
    BOOL TableHilighted=FALSE;
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    // positioned on table or table hilighted
    if (  HilightType==HILIGHT_CHAR
      && (cid(HilightBegRow)>0 || cid(HilightEndRow)>0) ) TableHilighted=TRUE;
    CurCell=cid(CurLine);
    if (!TerArg.PrintView || (CurCell==0 && !TableHilighted)) return TRUE;


    // save the width of the current cell shading

    if (select<=0) {
       select=CallDialogBox(w,"CellRotationParam",CellRotationParam,0L);

       angle=cell[CurCell].TextAngle;   // user entered value
    }
    else {
       angle=0;
       if (direction==TEXT_TOP_TO_BOT) angle=270;
       if (direction==TEXT_BOT_TO_TOP) angle=90;

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

    HilightType=HILIGHT_OFF;     // remove highlighting so shading can be seen

    //  Set the shading for the marked cells
    for (cl=0;cl<TotalCells;cl++) {
       if (!(cell[cl].InUse) || !(cell[cl].flags&(CFLAG_SEL1|CFLAG_SEL2))) continue;

       cell[cl].TextAngle=angle;
    }

    //***************** reset the text map *************
    DeleteTextMap(w,TRUE);

    TerArg.modified++;
    if (RepageBeginLine>CurLine) RepageBeginLine=CurLine;
    RequestPagination(w,FALSE);      // row height might change

    if (repaint) PaintTer(w);

    return TRUE;
}

/******************************************************************************
    IsBaselineAlignedCellLine:
    Returns TRUE if a given line belongs to a baseline aligned cell line.
******************************************************************************/
BOOL IsBaselineAlignedCellLine(PTERWND w, long line)
{
    int cl=cid(line);

    return (cl>0 && cell[cl].flags&CFLAG_VALIGN_BASE);
}
 
