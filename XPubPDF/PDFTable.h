#ifndef __PDFTABLE_H__
#define __PDFTABLE_H__

#include "XPubPDFExpImp.h"
#include <map>
#include <vector>
#include "PDFCell.h"

#define DEF_ROW_HEIGHT          25     // in mm
#define DEF_COL_WIDTH           100    // in mm

#if defined(WIN32)
#pragma warning(disable: 4251)
#endif // WIN32

class CPDFDocument;
class CRowInfo;

struct heightrow
{
  long m_lRow;
  long m_lHeight;
};


typedef std::map <cell, CPDFCell *, cell>              PDFCellMap;
typedef std::map <cell, CPDFCell *, cell>::iterator    PDFCellMapIter;
typedef std::vector<CRowInfo>      PDFRowInfo;
typedef std::vector<heightrow>     PDFDivHeight;
typedef std::vector<CPDFCell *>    PDFColumns;

class CRowInfo
{
public:
  ~CRowInfo();

protected:
  CRowInfo();

  long GetRowHeight(){return m_lRowHeight;};
  long GetRowMinHeight(){return m_lRowMinHeight;};
  void SetRowHeight(long i_lRowHeight, bool_a i_bMinHeight = false_a);

  long GetColCount(){return m_lColCount;};
  void SetColCount(long i_lColCount){m_lColCount = i_lColCount;};

  bool_a Fixed(){return m_bFixed;};
  void SetFixed(bool_a i_bFixed){m_bFixed = i_bFixed;};

  bool_a KeepTogether(){return m_bKeepTogether;};
  void SetKeepTogether(bool_a i_bKeepTogether){m_bKeepTogether = i_bKeepTogether;};

  bool_a Header(){return m_bHeader;};
  void SetHeader(bool_a i_bHeader){m_bHeader = i_bHeader;};

  CPDFCell *GetColumn(long i_lCol);
  bool_a InsertColumn(CPDFCell *i_pCell, long i_lCol = -1);
  CPDFCell *RemoveColumn(long i_lCol = -1);
  bool_a DeleteColumn(long i_lCol = -1);
  bool_a DeleteAllColumn();

  friend class CPDFTable;

private:
  long         m_lRowHeight;
  long         m_lRowMinHeight;
  long         m_lColCount;
  bool_a       m_bFixed;
  bool_a       m_bHeader;
  bool_a       m_bKeepTogether;
  PDFColumns   m_Columns;
};


class XPUBPDF_EXPORTIMPORT CPDFTable :public CPDFCell
{
public:
  CPDFTable(CPDFDocument *i_pDoc, long i_lPage, long i_lRowCount, long i_lColCount,
            long i_lXPos, long i_lYPos, long i_lXStep, long i_lYStep);
  ~CPDFTable();

  CPDFCell *GetCell(long i_lRow, long i_lCol);
  CPDFCell *GetCell(cell i_Cell);
  long GetColsWidth(long i_lEndCol, long i_lRow);
  long GetRowsHeights(long i_lEndRow);
  long GetMaxTableColWidth(long i_lCol);
  long GetMaxTableCellsWidth();
  long GetMinTableCellsWidth();
  long GetTopBorder(long i_lRow = NOT_USED);
  long GetBottomBorder(long i_lRow = NOT_USED);

  enum eINSDELTYPE
  {
     eResizeLeftCell = 0,
     eResizeRightCell,
     eMoveCellRight,
  };

  bool_a InsertColumn(long i_lBeforeCol, long i_lRow, eINSDELTYPE i_eType = eMoveCellRight);
  bool_a DeleteColumn(long i_lCol, long i_lRow, eINSDELTYPE i_eType = eMoveCellRight);

  bool_a InsertRow(long i_lColCount = -1, long i_lBeforeRow = -1);
  bool_a DeleteRow(long i_lRow);

  bool_a InsertTableToCell(cell i_Cell, CPDFTable *i_pTable);
  void SetParentTable(CPDFTable *i_pTable){m_pParentTable = i_pTable;};

  bool_a SetCellWidth(cell i_Cell, long i_lWidth, eINSDELTYPE i_eType = eMoveCellRight);
  long GetCellWidth(cell i_Cell);
  //sets height of row i_lRow
  //all overrow cells will be stretched
  bool_a SetRowHeight(long i_lRow, long i_lHeight, bool_a i_bMinHeight = false_a);
  long GetRowHeight(long i_lRow);

  //cells from (i_lRow, i_lBeginCol + 1) to (i_lRow, i_lEndCol) will be deleted
  //and cell (i_lRow, i_lBeginCol) will be stretched
  //if function succeeds return value is true_a
  //if fails returns false_a
  bool_a JoinRowCells(long i_lRow, long i_lBeginCol, long i_lEndCol);
  //cells from (i_lBeginRow + 1, i_lCol) to (i_lEndRow, i_lCol) will be deleted
  //and cell (i_lBeginRow, i_lCol) will be stretched
  //cells must have same left and right position
  //if function succeeds return value is true_a
  //if fails returns false_a
  bool_a JoinColCells(long i_lCol, long i_lBeginRow, long i_lEndRow);

  bool_a SplitCellHorizontal(long i_lRow, long i_lCol);
  bool_a SplitCellVertical(long i_lRow, long i_lCol);

  bool_a HasOverRowCell(long i_lRow);
  CPDFCell *GetFirstOverRowCell(long i_lRow, long i_lStartCol = -1);

  bool_a SetTableBgImage(long i_lRow, long i_lCol, const char *i_pczImageName);


  CPDFTable *Clone();
  long GetRowCount();
  long GetColCount(long i_lRow);
  long GetMaxColCount();
  long GetMaxWidth();
  long GetDefRowHeight();
  void SetDefRowHeight(long i_lHeight);
  long GetDefColWidth();
  void SetDefColWidth(long i_lWidth);
  long GetPageNumber();
  void SetPage(long i_lPage);
  bool_a EndEdit();
  void SetInTextBox(CPDFTextBox *i_pTextBox);
  CPDFTextBox *GetInTextBox(){return m_pInTextBox;};
  CPDFTextBox *GetInTextBoxNext(long i_lNext);
  long CalculateTBPos(CPDFTextBox *i_pTB);
  bool_a IsInTable();
  bool_a Flush();
  virtual bool_a IsFlush();
  virtual bool_a IsTable(){return true_a;};
  virtual bool_a MoveTo(long i_lX, long i_lY);
  bool_a MoveRowTo(long i_lRow, long i_lX);
  long GetXPos(){return m_lXPos;};
  long GetYPos(){return m_lYPos;};
  long GeCellColumnInRow(CPDFCell *i_pCell, long i_lRow);
  virtual void ChangeLeftIndents(long i_lWidth);
  virtual void ChangeRightIndents(long i_lWidth);
  virtual void ChangeTopIndents(long i_lWidth);
  virtual void ChangeBottomIndents(long i_lWidth);
  virtual void SetBorderType(long i_lBorder);
  virtual void SetBorderWidth(long i_lBorder, long i_lWidth);
  virtual void SetDocument(CPDFDocument *i_pDoc);
  CPDFDocument *GetDocument(){return m_pDoc;};
  bool_a ChangeImageID(std::map <long, long> *i_pImageMap);
  bool_a ChangeFontID(std::map <long, long> *i_pFontMap);
  std::vector<cell> GetCellsBetweenRange(long i_lRow, long i_lLeft, long i_lRight);
  std::vector<cell> GetRightNeighbours(cell i_Cell);
  std::vector<cell> GetLeftNeighbours(cell i_Cell);
  bool_a ChangeCellsIndents(cell i_Cell, long i_lBorder, long i_lLeft, long i_lTop, long i_lRight, long i_lBottom);


  long DivideTable(long i_lHeight, long i_lPage);//divide table or update divided height
  bool_a IsDivided();
  long GetDivideCount();
  long GetDividedHeight(long i_lPage);
  long GetDividedRowHeight(long i_lRow, long i_lHeight);
  bool_a GetRowsRangeAtPage(long i_lPage, long &i_lBRow, long &i_lERow);
  bool_a CanBeMoved(long i_lPage);
  bool_a CanBeDivided(long i_lHeight);

  long GetDividedRowCount(long i_lPage);
  void SetHeaderRow(long i_lRow, bool_a i_bSet);
  void SetFixedRow(long i_lRow, bool_a i_bSet);
  bool_a HeaderRow(long i_lRow);
  bool_a KeepRowTogether(long i_lRow);
  void SetKeepRowTogether(long i_lRow, bool_a i_bKeepTogether);
  long GetTogetherRowHeight(long &i_lRow);

  bool_a ReplacePageMarks();

  void GetRange(long i_lRow, long i_lCol, long &o_lBgnRow, long &o_lEndRow, long &o_lBgnCol, long &o_lEndCol);

protected:
  virtual bool_a FlushBackground();
  virtual bool_a FlushBorder();

  PDFDocPTR GetBaseDoc();

private:
  bool_a InsertCell(cell i_Cell, CPDFCell *i_pCell);
  bool_a InsertCell(long i_lRow, long i_lCol, CPDFCell *i_pCell);
  bool_a DeleteCell(cell i_Cell);
  bool_a DeleteCell(long i_lRow, long i_lCol);
  CPDFCell *RemoveCell(cell i_Cell);
  CPDFCell *RemoveCell(long i_lRow, long i_lCol);
  void SetInTable(bool_a i_bInTable = true_a);
  void SetClippingRect(rec_a i_Rec);
  bool_a ProcessCell(long i_lRow, long i_lCol, long i_lDif, eINSDELTYPE i_eType);
  bool_a MoveCell(cell i_Cell, long i_lDif);
  bool_a MoveCellToNextPage();
  long GetHeaderRowHeight();
  long GetHeaderRowCount();
  bool_a InsertHeaderRows(long i_lPage);
  bool_a MakeSpaceForRow(long i_lRow, long i_lSize, long i_lHeight);
  bool_a DivideRow(long i_lPage);
  bool_a SplitRow(long i_lRow, long i_lHeight);

  void HeightChanged(long i_lRow = -1);
  void WidthChanged(long i_lRow = -1);
  void ChildTableHeightChanged(cell i_Cell, long i_lHeight);
private:

//  PDFCellMap    m_CellMap;

  PDFRowInfo    m_RowInfo;

  PDFDivHeight  m_DivHeights;

  bool_a        m_bChangeHeight;
  CPDFTextBox   *m_pInTextBox;
  bool_a        m_bInTable;
  long          m_lDefRowHeight;
  long          m_lDefColWidth;
  long          m_lDefColCount;
//  long          m_lRowCount;
  long          m_lXPos;
  long          m_lYPos;
  long          m_lPage;
  CPDFDocument  *m_pDoc;
  CPDFTable     *m_pParentTable;

  std::map <cell, long, cell> m_TableBgMap;
  std::map <cell, CPDFTable::eINSDELTYPE, cell>  m_TempMap;
};

#if defined(WIN32)
#pragma warning(default: 4251)
#endif // WIN32

#endif  /*  __PDFTABLE_H__  */

