
#if defined(WIN32)
#include <crtdbg.h>
#endif // WIN32
#include "PDFTable.h"
#include "PDFDocument.h"
#include "pdffnc.h"
#include "PDFBaseFont.h"

using namespace std;

CPDFTable::CPDFTable(CPDFDocument *i_pDoc, long i_lPage, long i_lRowCount, long i_lColCount,
                     long i_lXPos, long i_lYPos, long i_lXStep, long i_lYStep)
  :m_pDoc(i_pDoc), CPDFCell(this)
{
  m_bChangeHeight = false_a;
  m_pParentTable = NULL;
  m_bInTable = false_a;
  m_pInTextBox = NULL;
  m_recPosSize.lLeft = 0;
  m_recPosSize.lRight = 0;
  m_recPosSize.lTop = 0;
  m_recPosSize.lBottom = 0;
  m_lDefRowHeight = i_lYStep;//DEF_ROW_HEIGHT;
  m_lDefColWidth = i_lXStep;//DEF_COL_WIDTH;
  m_lDefColCount = i_lColCount;
  i_lRowCount/*m_lRowCount*/ = i_lRowCount < 0 ? 0 : i_lRowCount;
  m_lXPos = i_lXPos;
  m_lYPos = i_lYPos;
  m_lPage = i_lPage - 1;
  CPDFTableCell *pCell;
  rec_a recPosSize;
  if (i_lRowCount)
  {

    recPosSize.lTop = i_lYPos;
    recPosSize.lBottom = recPosSize.lTop - m_lDefRowHeight;
    CRowInfo rowInfo;
    rowInfo.SetColCount(0);
    rowInfo.SetRowHeight(m_lDefRowHeight);
    for (long lRow = 0; lRow < i_lRowCount; lRow++)
    {
      m_RowInfo.push_back(rowInfo);
      recPosSize.lLeft = i_lXPos;
      recPosSize.lRight = recPosSize.lLeft + m_lDefColWidth;
      for (long lCol = 0; lCol < i_lColCount; lCol++)
      {
        pCell = new CPDFTableCell(m_pDoc, this, recPosSize, lRow, lCol, m_lPage + 1);
        if (!pCell)
          return;
        InsertCell(lRow, lCol, (CPDFCell *)pCell);
        recPosSize.lLeft += m_lDefColWidth;
        recPosSize.lRight += m_lDefColWidth;
      }
      recPosSize.lTop -= m_lDefRowHeight;
      recPosSize.lBottom -= m_lDefRowHeight;
    }
  }
}

CPDFTable::~CPDFTable()
{
  long lSize = GetRowCount();
  for (long lRow = 0; lRow < lSize; lRow++)
  {
    m_RowInfo[lRow].DeleteAllColumn();
  }
}

PDFDocPTR CPDFTable::GetBaseDoc()
{
  return (PDFDocPTR)m_pDoc;
}

CPDFCell *CPDFTable::GetCell(long i_lRow, long i_lCol)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return NULL;
  return m_RowInfo[i_lRow].GetColumn(i_lCol);
}

CPDFCell *CPDFTable::GetCell(cell i_Cell)
{
  CRowInfo rowInfo;
  if (i_Cell.m_lRow < 0 || i_Cell.m_lRow >= GetRowCount())
    return NULL;
  return m_RowInfo[i_Cell.m_lRow].GetColumn(i_Cell.m_lCol);
}

long CPDFTable::GetRowCount()
{
  return m_RowInfo.size();
}

long CPDFTable::GetColCount(long i_lRow)
{
  if ((i_lRow < 0) || (i_lRow >= GetRowCount()))
    return 0;
  return m_RowInfo[i_lRow].GetColCount();
}

long CPDFTable::GetMaxColCount()
{
  long lRet = 0;
  long lSize = GetRowCount();
  CRowInfo rowInfo;
  for (long lRow = 0; lRow < lSize; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    if (lRet < rowInfo.GetColCount())
      lRet = rowInfo.GetColCount();
  }
  return lRet;
}

long CPDFTable::GetDefRowHeight()
{
  return m_lDefRowHeight;
}

void CPDFTable::SetDefRowHeight(long i_lHeight)
{
  m_lDefRowHeight = i_lHeight;
}

long CPDFTable::GetDefColWidth()
{
  return m_lDefColWidth;
}

void CPDFTable::SetDefColWidth(long i_lWidth)
{
  m_lDefColWidth = i_lWidth;
}

bool_a CPDFTable::HasOverRowCell(long i_lRow)
{
  CPDFCell *pCell = NULL;
  if ((i_lRow < 0) || (i_lRow >= GetRowCount()))
    return false;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  long lColCount = rowInfo.GetColCount();
  for (long lCol = 0; lCol < lColCount; lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell && pCell->IsOverRowCell())
      return true_a;
  }
  return false_a;
}

CPDFCell *CPDFTable::GetFirstOverRowCell(long i_lRow, long i_lStartCol)
{
  CPDFCell *pCell = NULL;
  if ((i_lRow < 0) || (i_lRow >= GetRowCount()))
    return pCell;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  long lColCount = rowInfo.GetColCount();
  if (i_lStartCol < 0)
    i_lStartCol = 0;
  for (long lCol = i_lStartCol; lCol < lColCount; lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell && pCell->IsOverRowCell())
      return pCell;
  }
  return NULL;
}

bool_a CPDFTable::SetTableBgImage(long i_lRow, long i_lCol, const char *i_pczImageName)
{
  CPDFImage *pImage = m_pDoc->CreateImage(i_pczImageName);
  long lImage = -1;
  if (i_lRow > 0 && i_lCol > 0)
  {
    CPDFCell *pCell = GetCell(i_lRow - 1, i_lCol - 1);
    if (!pCell)
      return false_a;
    ((CPDFTableCell *)pCell)->SetBgImage(i_pczImageName);
    return ((CPDFTableCell *)pCell)->SetTransparentBg(false_a);
  }
  m_TableBgMap[cell(i_lRow, i_lCol)] = pImage->GetImageID();
  lImage = m_TableBgMap[cell(i_lRow, i_lCol)];
  return true_a;
}

bool_a CPDFTable::MoveTo(long i_lX, long i_lY)
{
  CPDFCell *pCell = NULL;
  CRowInfo rowInfo;
  rec_a recPosSize;
  bool_a bRet = true_a;
  long lSize = GetRowCount();

  if (m_lXPos == i_lX && m_lYPos == i_lY)
    return true_a;
  CPDFCell::MoveTo(i_lX, i_lY);

  for (long lRow = 0; lRow < lSize; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    long lColCount = rowInfo.GetColCount();
    for (long lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (pCell)
      {
         recPosSize = pCell->GetPosSize();
         if (!(pCell->IsOverRowCell())
             || (cell(lRow, lCol) == pCell->GetCellPos()))
         {
           if (!(pCell->MoveTo(recPosSize.lLeft + (i_lX - m_lXPos), recPosSize.lTop + (i_lY - m_lYPos))))
             bRet = false_a;
         }
      }
    }
  }
  m_lXPos = i_lX;
  m_lYPos = i_lY;

  if (bRet && m_pInTextBox)
    bRet = m_pInTextBox->UpdateTable(this);

  return bRet;
}

bool_a CPDFTable::MoveRowTo(long i_lRow, long i_lX)
{
  CPDFCell *pCell = NULL;
  rec_a recPosSize;
  long lX;
  if ((i_lRow < 0) || (i_lRow >= GetRowCount()))
    return false_a;
  pCell = GetCell(i_lRow, 0);
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  recPosSize = pCell->GetPosSize();
  lX = recPosSize.lLeft;
  if (lX == i_lX)
    return true_a;
  lX = i_lX - lX;
  if (HasOverRowCell(i_lRow))
    return false_a;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  for (long lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell)
    {
      recPosSize = pCell->GetPosSize();
      if (!(((CPDFTableCell *)pCell)->MoveTo(recPosSize.lLeft + lX, recPosSize.lTop)))
        return false_a;
    }
  }
  return true_a;
}

long CPDFTable::GeCellColumnInRow(CPDFCell *i_pCell, long i_lRow)
{
  if (!i_pCell || (i_lRow < 0) || (i_lRow >= GetRowCount()))
    return -1;
  CPDFCell *pCell = NULL;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  for (long lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell && pCell == i_pCell)
      return lCol;
  }
  return -1;
}

bool_a CPDFTable::InsertTableToCell(cell i_Cell, CPDFTable *i_pTable)
{
  CPDFCell *pCell = GetCell(i_Cell);
  if (!pCell || !i_pTable)
    return false_a;
  rec_a recPosSize = pCell->GetPosSize();
  long lOver = pCell->GetOverRowCount();
  cell Cell = pCell->GetCellPos();
  long lOverRowsHeight = 0;

  for (long lRow = Cell.m_lRow + lOver; lRow > Cell.m_lRow; lRow--)
  {
    lOverRowsHeight += GetRowHeight(lRow);
    if (!RemoveCell(lRow, Cell.m_lCol))
      return false_a;
    if (!InsertCell(lRow, Cell.m_lCol, i_pTable))
      return false_a;
  }

  long lHeight = i_pTable->GetRowsHeights(i_pTable->GetRowCount());
  if ((recPosSize.lTop - recPosSize.lBottom) < lHeight)
  {
    lHeight -= lOverRowsHeight;
    if (!SetRowHeight(Cell.m_lRow, lHeight))
      return false_a;
    recPosSize.lBottom = recPosSize.lTop - lHeight;
  }

  if (!DeleteCell(Cell))
    return false_a;

  i_pTable->SetInTable();
  i_pTable->SetClippingRect(recPosSize);
  i_pTable->MoveTo(recPosSize.lLeft, recPosSize.lTop);
  i_pTable->SetPage(m_lPage + 1);
  i_pTable->SetCellPos(Cell);
  i_pTable->SetDocument(m_pDoc);
  i_pTable->SetOverRowCount(lOver);
  i_pTable->SetParentTable(this);
  return InsertCell(Cell, (CPDFCell *)i_pTable);
}

bool_a CPDFTable::InsertColumn(long i_lBeforeCol, long i_lRow, eINSDELTYPE i_eType)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  CPDFCell *pCell = NULL;
  long lWidth = 0, lColWidth = 0;
  rec_a recPosSize;

  if (i_lBeforeCol < 0)
    i_lBeforeCol = 0;
  if (i_lBeforeCol > GetColCount(i_lRow))
    i_lBeforeCol = GetColCount(i_lRow);

  if (GetCell(i_lRow, i_lBeforeCol))
    lColWidth = GetCellWidth(cell(i_lRow, i_lBeforeCol));

  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];

  switch (i_eType)
  {
  case eResizeLeftCell:
    if (0 == i_lBeforeCol)
      return false_a;
    lWidth = GetCellWidth(cell(i_lRow, i_lBeforeCol - 1));
    if (lWidth <= lColWidth)
      return false_a;

    pCell = GetCell(i_lRow, i_lBeforeCol - 1);
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();

    pCell = GetCell(i_lRow, i_lBeforeCol);
    if (pCell)
    {
      recPosSize.lRight = pCell->GetPosSize().lLeft;
      recPosSize.lLeft = recPosSize.lRight - lColWidth;
    }

    m_TempMap.clear();
    m_TempMap[pCell->GetCellPos()] = i_eType;//pCell;
    if (!SetCellWidth(cell(i_lRow, i_lBeforeCol - 1), lWidth - lColWidth, CPDFTable::eResizeRightCell))
      return false_a;

    break;
  case eResizeRightCell:
    if (rowInfo.GetColCount() <= i_lBeforeCol)
      return false_a;
    lWidth = GetCellWidth(cell(i_lRow, i_lBeforeCol));
    if (lWidth <= lColWidth)
      return false_a;

    pCell = GetCell(i_lRow, i_lBeforeCol);
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();

    if (0 == i_lBeforeCol)
    {
      recPosSize.lLeft = m_lXPos;
      recPosSize.lRight = recPosSize.lLeft + lColWidth;
    }
    else
    {
      pCell = GetCell(i_lRow, i_lBeforeCol - 1);
      _ASSERT(pCell);
      recPosSize.lLeft = pCell->GetPosSize().lRight;
      recPosSize.lRight = recPosSize.lLeft + lColWidth;
    }

    m_TempMap.clear();
    m_TempMap[pCell->GetCellPos()] = i_eType;//pCell;
    if (!SetCellWidth(cell(i_lRow, i_lBeforeCol), lWidth - lColWidth, CPDFTable::eResizeLeftCell))
      return false_a;

    break;
  case eMoveCellRight:
    if (rowInfo.GetColCount() <= i_lBeforeCol)
      return false_a;
//    lWidth = GetCellWidth(cell(i_lRow, i_lBeforeCol));
//    if (lWidth <= lColWidth)
//      return false_a;

    pCell = GetCell(i_lRow, i_lBeforeCol);
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();
//    pCell->MoveTo(recPosSize.lLeft + lColWidth, recPosSize.lTop);

    if (0 == i_lBeforeCol)
    {
      recPosSize.lLeft = m_lXPos;
      recPosSize.lRight = recPosSize.lLeft + lColWidth;
    }
    else
    {
      pCell = GetCell(i_lRow, i_lBeforeCol - 1);
      _ASSERT(pCell);
      recPosSize.lLeft = pCell->GetPosSize().lRight;
      recPosSize.lRight = recPosSize.lLeft + lColWidth;
    }

    m_TempMap.clear();
    m_TempMap[pCell->GetCellPos()] = i_eType;//pCell;
    if (!MoveCell(cell(i_lRow, i_lBeforeCol), lColWidth))
      return false_a;
//    if (!SetCellWidth(cell(i_lRow, i_lBeforeCol), lWidth - lColWidth, CPDFTable::eMoveCellRight))
//      return false_a;
    break;
  default:
    return false_a;
    break;
  }

  long lColCount = rowInfo.GetColCount();
  for (long j = lColCount - 1; j >= i_lBeforeCol; j--)
  {
    pCell = GetCell(i_lRow, j);//RemoveCell(i_lRow, j);
    _ASSERT(pCell);
    if (!pCell)
      return false_a;
    else
    {
      if (pCell->IsOverRowCell())
      {
        if (pCell->GetCellPos() == cell(i_lRow, j))
          pCell->SetCellPos(i_lRow, j + 1);
//        if (!InsertCell(i_lRow, j + 1, pCell))
//          return false_a;
      }
      else
      {
        pCell->SetCellPos(i_lRow, j + 1);
//        if (!InsertCell(i_lRow, j + 1, pCell))
//          return false_a;
      }
    }
  }

  pCell = (CPDFCell *) new CPDFTableCell(m_pDoc, this, recPosSize, i_lRow, i_lBeforeCol, m_lPage + 1);
  if (!pCell)
    return false_a;

  pCell->CopyAttributes(GetCell(i_lRow, i_lBeforeCol/* + 1*/));

  if (!InsertCell(i_lRow, i_lBeforeCol, pCell))
    return false_a;

  pCell->SetBorderType(pCell->GetBorderType());

  m_lDefColCount++;

  return true_a;
}

bool_a CPDFTable::DeleteColumn(long i_lCol, long i_lRow, eINSDELTYPE i_eType)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  CPDFCell *pCell = NULL;
//  long lWidth;
//  rec_a recPosSize;

  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];

  switch (i_eType)
  {
  case eResizeLeftCell:
    if (0 == i_lCol)
      return false_a;
//    lWidth = GetCellWidth(cell(i_lRow, i_lCol - 1));
//    if (lWidth <= m_lDefColWidth)
//      return false_a;
/*
    pCell = GetCell(i_lRow, i_lCol - 1);
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();
*/
/*
    pCell = GetCell(i_lRow, i_lCol);
    _ASSERT(pCell);
    recPosSize.lRight = pCell->GetPosSize().lLeft;
    recPosSize.lLeft = recPosSize.lRight - m_lDefColWidth;
*/
    m_TempMap.clear();
//    m_TempMap[pCell->GetCellPos()] = i_eType;//pCell;
    if (!SetCellWidth(cell(i_lRow, i_lCol), 0/*lWidth - m_lDefColWidth*/, CPDFTable::eResizeLeftCell))
      return false_a;

    break;
  case eResizeRightCell:
    if (rowInfo.GetColCount() <= i_lCol)
      return false_a;
//    lWidth = GetCellWidth(cell(i_lRow, i_lCol));
//    if (lWidth <= m_lDefColWidth)
//      return false_a;
/*
    pCell = GetCell(i_lRow, i_lCol);
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();
*/
/*
    pCell = GetCell(i_lRow, i_lCol - 1);
    _ASSERT(pCell);
    recPosSize.lLeft = pCell->GetPosSize().lRight;
    recPosSize.lRight = recPosSize.lLeft + m_lDefColWidth;
*/
    m_TempMap.clear();
//    m_TempMap[pCell->GetCellPos()] = i_eType;//pCell;
    if (!SetCellWidth(cell(i_lRow, i_lCol), 0/*lWidth - m_lDefColWidth*/, CPDFTable::eResizeRightCell))
      return false_a;

    break;
  case eMoveCellRight:
    if (rowInfo.GetColCount() <= i_lCol)
      return false_a;
//    lWidth = GetCellWidth(cell(i_lRow, i_lCol));
//    if (lWidth <= m_lDefColWidth)
//      return false_a;
/*
    pCell = GetCell(i_lRow, i_lCol);
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();
//    pCell->MoveTo(recPosSize.lLeft + m_lDefColWidth, recPosSize.lTop);
*/
/*
    pCell = GetCell(i_lRow, i_lCol - 1);
    _ASSERT(pCell);
    recPosSize.lLeft = pCell->GetPosSize().lRight;
    recPosSize.lRight = recPosSize.lLeft + m_lDefColWidth;
*/
//    m_TempMap.clear();
//    m_TempMap[pCell->GetCellPos()] = i_eType;//pCell;
//    if (!MoveCell(cell(i_lRow, i_lCol), m_lDefColWidth))
//      return false_a;
    if (!SetCellWidth(cell(i_lRow, i_lCol), 0, CPDFTable::eMoveCellRight))
      return false_a;

    break;
  default:
    return false_a;
    break;
  }

  if (!DeleteCell(i_lRow, i_lCol))
    return false_a;
  rowInfo = m_RowInfo[i_lRow];
  long lColCount = rowInfo.GetColCount();
  for (long j = i_lCol; j < lColCount; j++)
  {
    pCell = GetCell(i_lRow, j);//RemoveCell(i_lRow, j);
    _ASSERT(pCell);
    if (!pCell)
      return false_a;
    else
    {
      if (pCell->IsOverRowCell())
      {
        if (pCell->GetCellPos() == cell(i_lRow, j))
          pCell->SetCellPos(i_lRow, j - 1);
//        if (!InsertCell(i_lRow, j - 1, pCell))
//          return false_a;
      }
      else
      {
        pCell->SetCellPos(i_lRow, j - 1);
//        if (!InsertCell(i_lRow, j - 1, pCell))
//          return false_a;
      }
    }
  }

  m_lDefColCount--;
  if (m_lDefColCount < 1)
    m_lDefColCount = 1;

  return true_a;
}

bool_a CPDFTable::InsertRow(long i_lColCount, long i_lBeforeRow)
{
  CPDFCell *pTempCell;
  CPDFTableCell *pCell = NULL;
  rec_a recPosSize;
  long lColCount, lOver;
  long lBefore = i_lBeforeRow;
  long lRowHeight = m_lDefRowHeight;
  long lColWidth = m_lDefColWidth;
  long lSourceRow = i_lBeforeRow;

  if (!m_pDoc)
    return false_a;
  lRowHeight = m_pDoc->GetActualFontHeight();

  if (lBefore < 0 || lBefore >= GetRowCount())
    lSourceRow = lBefore = GetRowCount();
  if (lBefore > 0)
    lSourceRow = lBefore - 1;
  if (i_lColCount == -1)
    i_lColCount = GetColCount(lSourceRow);
  if (i_lColCount <= 0)
    return false_a;

  for (long i = GetRowCount() - 1; i >= lBefore; i--)
  {
    CRowInfo rowInfo = m_RowInfo[i];
    lColCount = rowInfo.GetColCount();
    for (long j = 0; j < lColCount; j++)
    {
      pTempCell = GetCell(i, j);
      _ASSERT(pTempCell);
      if (pTempCell)
      {
        recPosSize = pTempCell->GetPosSize();
//        RemoveCell(i, j);
        lOver = pTempCell->GetOverRowCount();
        if (!lOver || (lOver && ((pTempCell->GetCellPos().m_lRow + lOver) == i)))
        {
          pTempCell->MoveTo(recPosSize.lLeft, recPosSize.lTop - lRowHeight);
        }
        pTempCell->SetCellPos(cell(i + 1, j));
//        InsertCell(i + 1, j, pTempCell);
      }
    }
  }
  CRowInfo rowInfo;
  if (lBefore >= GetRowCount())
  {
    rowInfo.SetColCount(0);
    rowInfo.SetRowHeight(lRowHeight);
    m_RowInfo.push_back(rowInfo);
  }
  else
  {
    rowInfo.SetRowHeight(lRowHeight);
    m_RowInfo.insert(m_RowInfo.begin() + lBefore, rowInfo);
  }
//  m_CellHeights.insert(m_CellHeights.begin() + lBefore, lRowHeight);
  recPosSize.lLeft = m_lXPos;
  recPosSize.lTop = m_lYPos - GetRowsHeights(lBefore);
  if (GetCell(lSourceRow, 0))
    lColWidth = GetCellWidth(cell(lSourceRow, 0));
  recPosSize.lRight = recPosSize.lLeft + lColWidth;
  recPosSize.lBottom = recPosSize.lTop - lRowHeight;
  for (long lCol = 0; lCol < i_lColCount; lCol++)
  {
    pCell = new CPDFTableCell(m_pDoc, this, recPosSize, lBefore, lCol, m_lPage + 1);
    if (!pCell)
      return false_a;
    InsertCell(lBefore, lCol, (CPDFCell *)pCell);

//    pCell->SetBorderType(pCell->GetBorderType());

    if (GetCell(lSourceRow, lCol + 1))
      lColWidth = GetCellWidth(cell(lSourceRow, lCol + 1));
    else
      lColWidth = m_lDefColWidth;
    recPosSize.lLeft = recPosSize.lRight;//lColWidth;
    recPosSize.lRight = recPosSize.lLeft + lColWidth;
  }

  for (long lCol = 0; lCol < i_lColCount; lCol++)
  {
    pTempCell = GetCell(lBefore, lCol);
    _ASSERT(pTempCell);
    if (!pTempCell || !pTempCell->CopyAttributes(GetCell(lSourceRow, lCol)))
      return false_a;
  }

  HeightChanged();
  return true_a;
}

long CPDFTable::GetHeaderRowHeight()
{
  long lRet = 0, i;
  long lSize = (long)m_RowInfo.size();
  CRowInfo rowInfo;
  for (i = 0; i < lSize; i++)
  {
    rowInfo = m_RowInfo[i];
    if (!rowInfo.Header())
      break;
    lRet += rowInfo.GetRowHeight();
  }
  if (i > 0)
  {
    long lH = GetTopBorder();
    lRet += lH;
  }
  return lRet;
}

long CPDFTable::GetHeaderRowCount()
{
  long i;
  long lSize = (long)m_RowInfo.size();
  CRowInfo rowInfo;
  for (i = 0; i < lSize; i++)
  {
    rowInfo = m_RowInfo[i];
    if (!rowInfo.Header())
      break;
  }
  return i;
}

bool_a CPDFTable::MakeSpaceForRow(long i_lRow, long i_lSize, long i_lHeight)
{
  if (!m_pDoc || i_lRow < 0 || i_lSize <= 0 || i_lHeight <= 0)
    return false_a;
  if (i_lRow >= GetRowCount())
    return true_a;
  CPDFCell *pCell = NULL;
  rec_a recPS;
  long lRow, lCol;

  for (lRow = i_lRow; lRow < GetRowCount(); lRow++)
  {
    CRowInfo rowInfo = m_RowInfo[lRow];
    long lColCount = rowInfo.GetColCount();
//    m_RowInfo[lRow + i_lSize].SetRowHeight(rowInfo.GetRowHeight());
    for (lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = GetCell(lRow, lCol);
      _ASSERT(pCell);
      if (pCell)
      {
        recPS = pCell->GetPosSize();
//        RemoveCell(lRow, lCol);
        pCell->MoveTo(recPS.lLeft, recPS.lTop - i_lHeight);
        pCell->SetCellPos(cell(lRow + i_lSize, lCol));
//        InsertCell(lRow + i_lSize, lCol, pCell);
      }
    }
  }

  for (lRow = 0; lRow < i_lSize; lRow++)
  {
    CRowInfo rowInfo;
    m_RowInfo.insert(m_RowInfo.begin() + i_lRow, rowInfo);
  }
  return true_a;
}

bool_a CPDFTable::InsertHeaderRows(long i_lPage)
{
  long lPage = i_lPage - GetPageNumber();
  if (lPage <= 0 || lPage > (long)m_DivHeights.size())
    return false_a;
  long lBRow, lERow, lHRow, lHeight, lRow, lCol;
  CRowInfo rowInfo;
  CPDFCell *pCell, *pNewCell;
  if (!GetRowsRangeAtPage(i_lPage, lBRow, lERow))
    return false_a;
  if (lBRow == NOT_USED || lERow == NOT_USED)
    return true_a;
  lHRow = GetHeaderRowCount();
  if (!lHRow)
    return true_a;
  lHeight = GetHeaderRowHeight();
  if (!MakeSpaceForRow(lBRow, lHRow, lHeight))
    return false_a;
  for (lRow = 0; lRow < lHRow; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    if (!rowInfo.Header())
      break;
    m_RowInfo[lBRow + lRow].SetHeader(true_a);
    m_RowInfo[lBRow + lRow].SetFixed(true_a);
    m_RowInfo[lBRow + lRow].SetKeepTogether(rowInfo.KeepTogether());
    m_RowInfo[lBRow + lRow].SetRowHeight(rowInfo.GetRowHeight());
    for (lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
    {
      pCell = GetCell(lRow, lCol);
      _ASSERT(pCell);
      if (pCell)
      {
        if (pCell->IsTable())
          pNewCell = ((CPDFTable *)pCell)->Clone();
        else
          pNewCell = ((CPDFTableCell *)pCell)->Clone();
        if (!pNewCell)
          return false_a;
        pNewCell->SetCellPos(lBRow + lRow, lCol);
        pNewCell->SetPage(i_lPage);
        if (!InsertCell(lBRow + lRow, lCol, pNewCell))
          return false_a;
      }
    }
  }
  return true_a;
}

bool_a CPDFTable::DivideRow(long i_lPage)
{
  i_lPage -= GetPageNumber();
  if (i_lPage < 0 || i_lPage > (long)m_DivHeights.size())
    return false_a;
  if (i_lPage < (long)m_DivHeights.size())
    return false_a;
  long lPage = 0, lRet = 0, lNextRow;
  long lHeight = -1, lTopBorderH = 0, lBottomBorderH = 0;
  long lSize = GetRowCount();
  bool_a bSame = true_a;

  //if (KeepTogether)
    //return true_a;

  if (lPage < (long)m_DivHeights.size())
    lHeight = m_DivHeights[lPage].m_lHeight;
  lTopBorderH = GetTopBorder(0);
  for (long lRow = 0, lTemp = 0; lRow < lSize; lRow++)
  {
    lNextRow = lRow;
    lRet += GetTogetherRowHeight(lNextRow);

    lBottomBorderH = GetBottomBorder(lNextRow - 1);

    if (lHeight > 0 && (lRet/* + lTopBorderH*/ + lBottomBorderH) > lHeight)
    {
      //try to divide row
      if (!m_RowInfo[lNextRow - 1].KeepTogether() &&
          (lNextRow - 1 == lRow // one row
          || (lNextRow - 1 > 0 && m_RowInfo[lNextRow - 2].Header())) //header rows with only one '' row
          && lPage == (long)m_DivHeights.size() - 1)// only last overflow
      {
        long lH = GetDividedRowHeight(lNextRow - 1, (lRet/* + lTopBorderH*/ + lBottomBorderH) - lHeight);

        if (lH > 0)
        {
          if (!SplitRow(lNextRow -1, (lRet/* + lTopBorderH*/ + lBottomBorderH) - lHeight))
            return false_a;
          return true_a;
        }
        else
        {
          lTopBorderH = GetTopBorder(lNextRow - 1);
          lRet = 0;
        }

        lTopBorderH = GetBottomBorder(lNextRow - 1);
      }
      else
      {
        lTopBorderH = GetTopBorder(lNextRow - 1);
        lRet = 0;
      }

      lPage++;
      if (lHeight > 0 && bSame
          && (lNextRow >= lSize || !CanBeMoved(GetPageNumber() + (lPage - 1)))
          && !lRet)
      {
        return false_a;
      }

      if ((lNextRow >= lSize || !bSame) && !lRet)
      {
        lNextRow = lRow;
      }

      bSame = true_a;
      lTemp = lRow;
      if (lPage < (long)m_DivHeights.size())
        lHeight = m_DivHeights[lPage].m_lHeight;
      else
        lHeight = -1;
    }
    else
      bSame = false_a;
    lRow = lNextRow - 1;
  }

  return true_a;
}

bool_a CPDFTable::SplitRow(long i_lRow, long i_lHeight)
{

  if (!m_pDoc || i_lRow < 0 || i_lHeight <= 0)
    return false_a;
  if (i_lRow >= GetRowCount())
    return false_a;
  CPDFCell *pCell = NULL, *pNewCell = NULL;
  rec_a recPS;
  long lRow, lCol, lColCount, lH = 0, lTemp, lDrawHeight;

  for (lRow = i_lRow + 1; lRow < GetRowCount(); lRow++)
  {
    lColCount = GetColCount(lRow);
    for (lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = GetCell(lRow, lCol);
      _ASSERT(pCell);
      if (pCell)
      {
        if (pCell->GetCellPos().m_lRow == lRow)
        {
          recPS = pCell->GetPosSize();
          pCell->MoveTo(recPS.lLeft, recPS.lTop - i_lHeight);
          pCell->SetCellPos(cell(lRow, lCol));
        }
      }
    }
  }

  CRowInfo rowInfo;

  //recPS.lTop = ;
  //recPS.lBottom = ;
  //recPS.lRight = ;
  //recPS.lLeft = ;
  lColCount = GetColCount(i_lRow);
  for (lCol = 0; lCol < lColCount; lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell && !pCell->IsTable())
    {
      //pCell->SetTable(NULL);
      //((CPDFTableCell *)pCell)->SetDrawHeight(((CPDFTableCell *)pCell)->GetHeight() - i_lHeight);

      pNewCell = new CPDFTableCell(m_pDoc, NULL, ((CPDFTableCell *)pCell)->GetPosSize(),
                                    i_lRow + 1, lCol, m_lPage + 1);
      if (!pNewCell)
        return false_a;
      if (!pNewCell->CopyAttributes(pCell))
        return false_a;


      lTemp = ((CPDFTableCell *)pCell)->GetHeight();
      lDrawHeight = ((CPDFTableCell *)pCell)->GetDrawHeight();
      ((CPDFTableCell *)pCell)->m_bChangeSize = true_a;
      ((CPDFTableCell *)pCell)->SetHeight(lTemp - i_lHeight);

      if (!((CPDFTableCell *)pCell)->CopyOverflowLines((CPDFTableCell *)pNewCell, false_a,
                                                          lDrawHeight - i_lHeight))
                                                          //lTemp - i_lHeight))
        return false_a;

      ((CPDFTableCell *)pCell)->SetHeight(lTemp);
      ((CPDFTableCell *)pCell)->m_bChangeSize = false_a;


      pNewCell->SetTable(this);
      //pCell->SetTable(this);

      if (lH < ((CPDFTableCell *)pNewCell)->GetHeight())
        lH = ((CPDFTableCell *)pNewCell)->GetHeight();
      rowInfo.InsertColumn(pNewCell);
    }
  }
  //m_RowInfo[i_lRow].SetRowHeight(i_lHeight);

  rowInfo.SetRowHeight(lH);
  m_RowInfo.insert(m_RowInfo.begin() + i_lRow + 1, rowInfo);

  ((CPDFTableCell *)GetCell(i_lRow, 0))->CheckResize();
  for (lCol = 0; lCol < GetColCount(i_lRow + 1); lCol++)
  {
   ((CPDFTableCell *)GetCell(i_lRow + 1, lCol))->SetHeight(lH);
  }

  return true_a;
}

bool_a CPDFTable::DeleteRow(long i_lRow)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  CPDFCell *pCell = NULL;
  cell Cell;
  rec_a recPosSize;
  CRowInfo rowInfo = m_RowInfo[i_lRow];
  bool_a bRet = true_a, bOver = false_a;
  long lHeight = rowInfo.GetRowHeight();
  long lColCount;

  for (long i = i_lRow + 1; i < GetRowCount(); i++)
  {
    rowInfo = m_RowInfo[i];
    lColCount = rowInfo.GetColCount();
    for (long j = 0; j < lColCount; j++)
    {
      bOver = false_a;
      pCell = GetCell(i, j);
      _ASSERT(pCell);
      recPosSize = pCell->GetPosSize();
      if (pCell->IsOverRowCell())
      {
        if ((i == (i_lRow + 1)) && (pCell->GetCellPos().m_lRow <= i_lRow))
        {
          if (!(pCell->SetHeight((recPosSize.lTop - recPosSize.lBottom) - lHeight)))
            bRet = false_a;
          if (pCell->GetCellPos().m_lRow <= (i_lRow - 1))
            bOver = true_a;
        }
        else
        {
          if (!(pCell->MoveTo(recPosSize.lLeft, recPosSize.lTop + lHeight)))
            bRet = false_a;
        }

        if ((i == (i_lRow + 1)))
        {
          pCell = GetCell(i - 1, j);
          _ASSERT(pCell);
          if (!pCell)
            bRet = false_a;
          else
          {
            if (pCell->IsOverRowCell())
            {
              if (!bOver)
              {
                recPosSize = pCell->GetPosSize();
                pCell->DecOverRowCount();
                if (!(pCell->SetHeight((recPosSize.lTop - recPosSize.lBottom) - lHeight)))
                  bRet = false_a;
              }
              if (!RemoveCell(i - 1, j))
                bRet = false_a;
            }
            else
            {
              if (!DeleteCell(i - 1, j))
                bRet = false_a;
            }
          }
        }

        pCell = GetCell(i, j);//RemoveCell(i, j);
        _ASSERT(pCell);
        if (!pCell)
          bRet = false_a;
        else
        {
          if ((i == (i_lRow + 1)) && (pCell->GetCellPos().m_lRow <= i_lRow) && bOver)
            pCell->DecOverRowCount();
          if (!bOver)
            pCell->SetCellPos(i - 1, j);
//          if (!InsertCell(i - 1, j, pCell))
//            bRet = false_a;
        }
      }
      else
      {
        if (!(pCell->MoveTo(recPosSize.lLeft, recPosSize.lTop + lHeight)))
          bRet = false_a;

        if ((i == (i_lRow + 1)))
        {
          pCell = GetCell(i - 1, j);
          _ASSERT(pCell);
          if (!pCell)
            bRet = false_a;
          else
          {
            if (pCell->IsOverRowCell())
            {
              recPosSize = pCell->GetPosSize();
              pCell->DecOverRowCount();
              if (!(pCell->SetHeight((recPosSize.lTop - recPosSize.lBottom) - lHeight)))
                bRet = false_a;
              if (!RemoveCell(i - 1, j))
                bRet = false_a;
            }
            else
            {
              if (!DeleteCell(i - 1, j))
                bRet = false_a;
            }
          }
        }

        pCell = GetCell(i, j);//RemoveCell(i, j);
        _ASSERT(pCell);
        if (!pCell)
          bRet = false_a;
        else
        {
          pCell->SetCellPos(i - 1, j);
//          if (!InsertCell(i - 1, j, pCell))
//            bRet = false_a;
        }
      }
    }
  }
  m_RowInfo.pop_back();
  m_RowInfo.erase(m_RowInfo.begin() + i_lRow);
  HeightChanged();
  return bRet;
}

CPDFTable *CPDFTable::Clone()
{
  CPDFTable *pRetTable = NULL;
  CPDFCell *pCellOld = NULL, *pCell = NULL;
  pRetTable = m_pDoc->CreateTable(m_lPage + 1, GetRowCount(), 0,
                                    m_lXPos, m_lYPos, m_lDefColWidth, m_lDefRowHeight);
  if (!pRetTable)
    return pRetTable;

  if (IsInTable())
  {
    pRetTable->SetInTable();
    pRetTable->SetClippingRect(GetPosSize());
    pRetTable->SetCellPos(GetCellPos());
    pRetTable->SetOverRowCount(GetOverRowCount());
  }
  CRowInfo rowInfo;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    for (long lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
    {
      pCellOld = GetCell(lRow, lCol);
      _ASSERT(pCellOld);
      if (pCellOld->IsOverRowCell() && (pCellOld->GetCellPos() != cell(lRow, lCol)))
      {
        pCell = pRetTable->GetCell(pCellOld->GetCellPos());//pRetTable->GetCell(lRow - 1, lCol);
      }
      else
      {
        if (pCellOld->IsTable())
          pCell = ((CPDFTable *)pCellOld)->Clone();
        else
          pCell = ((CPDFTableCell *)pCellOld)->Clone();
      }
      pRetTable->InsertCell(lRow, lCol, pCell);
    }
  }
  return pRetTable;
}

bool_a CPDFTable::InsertCell(cell i_Cell, CPDFCell *i_pCell)
{
//  CPDFCell *pCell = GetCell(i_Cell);
//  _ASSERT(!pCell);
//  if (pCell)
//    return false_a;
  i_pCell->SetTable(this);
  CRowInfo rowInfo;
  if (i_Cell.m_lRow >= 0 && i_Cell.m_lRow < GetRowCount())
  {
    if (!m_RowInfo[i_Cell.m_lRow].InsertColumn(i_pCell, i_Cell.m_lCol))
      return false_a;
  }
  else
  {
    if (i_Cell.m_lRow < 0 || i_Cell.m_lRow > GetRowCount())
      return false_a;
    rowInfo.SetRowHeight(m_lDefRowHeight);
    m_RowInfo.push_back(rowInfo);
    if (!m_RowInfo[i_Cell.m_lRow].InsertColumn(i_pCell, i_Cell.m_lCol))
      return false_a;
  }
  return true_a;
}

bool_a CPDFTable::InsertCell(long i_lRow, long i_lCol, CPDFCell *i_pCell)
{
//  CPDFCell *pCell = GetCell(i_lRow, i_lCol);
//  _ASSERT(!pCell);
//  if (pCell)
//    return false_a;
  i_pCell->SetTable(this);
  CRowInfo rowInfo;
  if (i_lRow >= 0 && i_lRow < GetRowCount())
  {
    if (!m_RowInfo[i_lRow].InsertColumn(i_pCell, i_lCol))
      return false_a;
  }
  else
  {
    if (i_lRow < 0 || i_lRow > GetRowCount())
      return false_a;
    rowInfo.SetRowHeight(m_lDefRowHeight);
    m_RowInfo.push_back(rowInfo);
    if (!m_RowInfo[i_lRow].InsertColumn(i_pCell, i_lCol))
      return false_a;
  }
  return true_a;
}

CPDFCell *CPDFTable::RemoveCell(cell i_Cell)
{
  CPDFCell *pCell = GetCell(i_Cell);
  _ASSERT(pCell);
  if (!pCell)
    return NULL;
  if (i_Cell.m_lRow >= 0 && i_Cell.m_lRow < GetRowCount())
  {
    pCell = m_RowInfo[i_Cell.m_lRow].RemoveColumn(i_Cell.m_lCol);
  }
  return pCell;
}

CPDFCell *CPDFTable::RemoveCell(long i_lRow, long i_lCol)
{
  CPDFCell *pCell = GetCell(i_lRow, i_lCol);
  _ASSERT(pCell);
  if (!pCell)
    return NULL;
  if (i_lRow >= 0 && i_lRow < GetRowCount())
  {
    pCell = m_RowInfo[i_lRow].RemoveColumn(i_lCol);
  }
  return pCell;
}

bool_a CPDFTable::DeleteCell(cell i_Cell)
{
  CPDFCell *pCell = GetCell(i_Cell);
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  if (i_Cell.m_lRow >= 0 && i_Cell.m_lRow < GetRowCount())
  {
    m_RowInfo[i_Cell.m_lRow].DeleteColumn(i_Cell.m_lCol);
  }
  return true_a;
}

bool_a CPDFTable::DeleteCell(long i_lRow, long i_lCol)
{
  CPDFCell *pCell = GetCell(i_lRow, i_lCol);
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  if (i_lRow >= 0 && i_lRow < GetRowCount())
  {
    m_RowInfo[i_lRow].DeleteColumn(i_lCol);
  }
  return true_a;
}

bool_a CPDFTable::ProcessCell(long i_lRow, long i_lCol, long i_lDif, eINSDELTYPE i_eType)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  if (i_lCol < 0 || i_lCol >= rowInfo.GetColCount())
    return false_a;
  CPDFCell *pCell = GetCell(i_lRow, i_lCol);
  CPDFCell *pTempCell = NULL;
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  cell Cell = pCell->GetCellPos();
  long lWidth = GetCellWidth(Cell);
  rec_a recPosSize = pCell->GetPosSize();
  long lStartRow, lEndRow;
  lStartRow = Cell.m_lRow;
  lEndRow = lStartRow + pCell->GetOverRowCount();

  map <cell, CPDFTable::eINSDELTYPE, cell>::iterator iter;
  iter = m_TempMap.find(Cell);
  if (iter != m_TempMap.end())
    return true_a;

  switch (i_eType)
  {
  case eResizeLeftCell:
    pCell->SetWidth(lWidth - i_lDif);
    pCell->SetBorderType(pCell->GetBorderType());
    break;
  case eResizeRightCell:
    pCell->SetWidth(lWidth - i_lDif);
    pCell->MoveTo(recPosSize.lLeft + i_lDif, recPosSize.lTop);
    pCell->SetBorderType(pCell->GetBorderType());
    break;
  case eMoveCellRight:
    pCell->MoveTo(recPosSize.lLeft + i_lDif, recPosSize.lTop);
    pCell->SetBorderType(pCell->GetBorderType());
    break;
  }

  m_TempMap[Cell] = i_eType;//pCell;
  for (long lRow = lStartRow; lRow <= lEndRow; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    switch (i_eType)
    {
    case eResizeLeftCell:
      if ((Cell.m_lCol + 1) >= 0 && (Cell.m_lCol + 1) < rowInfo.GetColCount())
      {
        i_eType = eResizeRightCell;
        i_lDif *= -1;
        if (!ProcessCell(lRow, Cell.m_lCol + 1, i_lDif, i_eType))
          return false_a;
        i_lDif *= -1;
        i_eType = eResizeLeftCell;
      }
      break;
    case eResizeRightCell:
      if ((Cell.m_lCol - 1) >= 0 && (Cell.m_lCol - 1) < rowInfo.GetColCount())
      {
        i_eType = eResizeLeftCell;
        i_lDif *= -1;
        if (!ProcessCell(lRow, Cell.m_lCol - 1, i_lDif, i_eType))
          return false_a;
        i_lDif *= -1;
        i_eType = eResizeRightCell;
      }
      break;
    case eMoveCellRight:
      if ((Cell.m_lCol - 1) >= 0 && (Cell.m_lCol - 1) < rowInfo.GetColCount())
      {
        i_eType = eResizeLeftCell;
        i_lDif *= -1;
        pTempCell = GetCell(lRow, Cell.m_lCol - 1);
        _ASSERT(pTempCell);
        if (pTempCell)
        {
          iter = m_TempMap.find(cell(lRow, Cell.m_lCol - 1));
          if (iter != m_TempMap.end() && (*iter).second != CPDFTable::eMoveCellRight)
          {
            pTempCell->SetWidth(GetCellWidth(pTempCell->GetCellPos()) - i_lDif);
            pTempCell->SetBorderType(pTempCell->GetBorderType());
          }
        }
        if (!ProcessCell(lRow, Cell.m_lCol - 1, i_lDif, i_eType))
          return false_a;
        i_lDif *= -1;
        i_eType = eMoveCellRight;
      }
      if ((Cell.m_lCol + 1) >= 0 && (Cell.m_lCol + 1) < rowInfo.GetColCount())
      {
        pTempCell = GetCell(lRow, Cell.m_lCol + 1);
        _ASSERT(pTempCell);
        if (pTempCell)
        {
          iter = m_TempMap.find(cell(lRow, Cell.m_lCol + 1));
          if (iter != m_TempMap.end() && (*iter).second != CPDFTable::eMoveCellRight)
          {
            recPosSize = pTempCell->GetPosSize();
            pTempCell->SetWidth(GetCellWidth(pTempCell->GetCellPos()) + i_lDif);
            pTempCell->MoveTo(recPosSize.lLeft - i_lDif, recPosSize.lTop);
            pTempCell->SetBorderType(pTempCell->GetBorderType());
            m_TempMap.erase(iter);
          }
        }
        if (!ProcessCell(lRow, Cell.m_lCol + 1, i_lDif, i_eType))
          return false_a;
      }
      break;
    }
  }
  return true_a;
}

bool_a CPDFTable::MoveCell(cell i_Cell, long i_lDif)
{
  if (i_Cell.m_lRow < 0 || i_Cell.m_lRow >= GetRowCount())
    return false_a;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_Cell.m_lRow];
  if (i_Cell.m_lCol < 0 || i_Cell.m_lCol >= rowInfo.GetColCount())
    return false_a;
  CPDFCell *pCell = GetCell(i_Cell);
  CPDFCell *pTempCell;
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  rec_a recPosSize = pCell->GetPosSize();
  cell Cell = pCell->GetCellPos();
  long lDif = i_lDif;
  long lStartRow, lEndRow;
  if (0 == lDif)
    return true_a;

  pCell->MoveTo(recPosSize.lLeft + lDif, recPosSize.lTop);
  lStartRow = i_Cell.m_lRow;
  lEndRow = lStartRow + pCell->GetOverRowCount();

//  m_TempMap.clear();
  m_TempMap[Cell] = CPDFTable::eMoveCellRight;//pCell;
  for (long lRow = lStartRow; lRow <= lEndRow; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    if ((Cell.m_lCol + 1) >= 0 && (Cell.m_lCol + 1) < rowInfo.GetColCount())
    {
      pTempCell = GetCell(lRow, Cell.m_lCol + 1);
      _ASSERT(pTempCell);
      if (pTempCell)
      {
        map <cell, CPDFTable::eINSDELTYPE, cell>::iterator iter;
        iter = m_TempMap.find(cell(lRow, Cell.m_lCol + 1));
        if (iter != m_TempMap.end() && (*iter).second != CPDFTable::eMoveCellRight)
        {
          recPosSize = pTempCell->GetPosSize();
          pTempCell->SetWidth(GetCellWidth(pTempCell->GetCellPos()) - lDif);
//            pTempCell->MoveTo(recPosSize.lLeft + lDif, recPosSize.lTop);
          m_TempMap.erase(iter);
        }
      }
      if (!ProcessCell(lRow, Cell.m_lCol + 1, lDif, CPDFTable::eMoveCellRight))
        return false_a;
    }
  }
  m_TempMap.clear();
  return true_a;
}


bool_a CPDFTable::SetCellWidth(cell i_Cell, long i_lWidth, eINSDELTYPE i_eType)
{
  if (i_Cell.m_lRow < 0 || i_Cell.m_lRow >= GetRowCount())
    return false_a;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_Cell.m_lRow];
  if (i_Cell.m_lCol < 0 || i_Cell.m_lCol >= rowInfo.GetColCount())
    return false_a;
  CPDFCell *pCell = GetCell(i_Cell);
  CPDFCell *pTempCell;
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  rec_a recPosSize = pCell->GetPosSize();
  cell Cell = pCell->GetCellPos();
  long lWidth = GetCellWidth(Cell);
  long lDif = i_lWidth - lWidth;
  long lStartRow, lEndRow;
  if (0 == lDif)
    return true_a;

  switch (i_eType)
  {
  case eResizeLeftCell:
    pCell->SetWidth(i_lWidth);
    pCell->MoveTo(recPosSize.lLeft - lDif, recPosSize.lTop);
    pCell->SetBorderType(pCell->GetBorderType());
    break;
  case eResizeRightCell:
    pCell->SetWidth(i_lWidth);
    pCell->SetBorderType(pCell->GetBorderType());
    break;
  case eMoveCellRight:
    pCell->SetWidth(i_lWidth);
    pCell->SetBorderType(pCell->GetBorderType());
    break;
  }

  lStartRow = Cell.m_lRow;
  lEndRow = lStartRow + pCell->GetOverRowCount();

//  m_TempMap.clear();
  m_TempMap[Cell] = i_eType;//pCell;
  for (long lRow = lStartRow; lRow <= lEndRow; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    switch (i_eType)
    {
    case eResizeLeftCell:
      if ((Cell.m_lCol - 1) >= 0 && (Cell.m_lCol - 1) < rowInfo.GetColCount())
        if (!ProcessCell(lRow, Cell.m_lCol - 1, lDif, i_eType))
          return false_a;
      break;
    case eResizeRightCell:
      if ((Cell.m_lCol + 1) >= 0 && (Cell.m_lCol + 1) < rowInfo.GetColCount())
        if (!ProcessCell(lRow, Cell.m_lCol + 1, lDif, i_eType))
          return false_a;
      break;
    case eMoveCellRight:
      if ((Cell.m_lCol + 1) >= 0 && (Cell.m_lCol + 1) < rowInfo.GetColCount())
      {
        pTempCell = GetCell(lRow, Cell.m_lCol + 1);
        _ASSERT(pTempCell);
        if (pTempCell)
        {
          map <cell, CPDFTable::eINSDELTYPE, cell>::iterator iter;
          iter = m_TempMap.find(cell(lRow, Cell.m_lCol + 1));
          if (iter != m_TempMap.end() && (*iter).second != CPDFTable::eMoveCellRight)
          {
            recPosSize = pTempCell->GetPosSize();
            pTempCell->SetWidth(GetCellWidth(pTempCell->GetCellPos()) - lDif);
            pTempCell->SetBorderType(pTempCell->GetBorderType());
//            pTempCell->MoveTo(recPosSize.lLeft + lDif, recPosSize.lTop);
            m_TempMap.erase(iter);
          }
        }
        if (!ProcessCell(lRow, Cell.m_lCol + 1, lDif, i_eType))
          return false_a;
      }
      break;
    }
  }
  m_TempMap.clear();

//  pCell = GetCell(i_Cell);
//  pCell->SetBorderType(pCell->GetBorderType());

  return true_a;
}

long CPDFTable::GetCellWidth(cell i_Cell)
{
  long lRet = 0;
  CPDFCell *pCell = GetCell(i_Cell);
  if (pCell)
  {
    rec_a recPosSize = pCell->GetPosSize();
    lRet = recPosSize.lRight - recPosSize.lLeft;
  }
  return lRet;
}

bool_a CPDFTable::SetRowHeight(long i_lRow, long i_lHeight, bool_a i_bMinHeight)
{
  if (m_bChangeHeight)
    return true_a;
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  long lDif = i_lHeight - rowInfo.GetRowHeight();
  long /*lHeight, lWidth, */lTemp;
  long lCol;
  CPDFCell *pCell = NULL;
  rec_a recPosSize;
  if (0 == lDif)
    return true_a;

  for (lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell)
    {
      if (!pCell->IsTable())
      {
        lTemp = lDif;
        if (!((CPDFTableCell *)pCell)->CanBeHeightChanged(lDif))
        {
          if (0 == lDif)
            return true_a;
        }
      }
    }
  }

  if (0 == lDif)
    return true_a;
  i_lHeight += lDif - (i_lHeight - rowInfo.GetRowHeight());
  m_bChangeHeight = true_a;
  if (i_lHeight < m_RowInfo[i_lRow].GetRowMinHeight())
  {
    i_lHeight = m_RowInfo[i_lRow].GetRowMinHeight();
    lDif = i_lHeight - rowInfo.GetRowHeight();
  }
  m_RowInfo[i_lRow].SetRowHeight(i_lHeight, i_bMinHeight);
  for (lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell)
    {
      if (pCell->IsOverRowCell())
      {
        recPosSize = pCell->GetPosSize();
        pCell->SetHeight((recPosSize.lTop - recPosSize.lBottom) + lDif);
      }
      else
        pCell->SetHeight(i_lHeight);
    }
  }
//  m_RowInfo[i_lRow].SetRowHeight(i_lHeight);
  for (long i = GetRowCount() - 1; i > i_lRow; i--)
  {
    rowInfo = m_RowInfo[i];
    for (long j = 0; j < rowInfo.GetColCount(); j++)
    {
      pCell = GetCell(i, j);
      _ASSERT(pCell);
      if (pCell)
      {
        if (pCell->IsOverRowCell())
        {
          if (pCell->GetCellPos().m_lRow > i_lRow && (pCell->GetCellPos().m_lRow + pCell->GetOverRowCount() == i))
          {
            recPosSize = pCell->GetPosSize();
            pCell->MoveTo(recPosSize.lLeft, recPosSize.lTop - lDif);
          }
        }
        else
        {
          recPosSize = pCell->GetPosSize();
          pCell->MoveTo(recPosSize.lLeft, recPosSize.lTop - lDif);
        }
      }
    }
  }
  m_bChangeHeight = false_a;
  HeightChanged(i_lRow);
  return true_a;
}

long CPDFTable::GetRowHeight(long i_lRow)
{
  long lRet = 0;
  CRowInfo rowInfo;
  if (0 <= i_lRow && i_lRow < GetRowCount())
  {
    rowInfo = m_RowInfo[i_lRow];
    lRet = rowInfo.GetRowHeight();
  }
  return lRet;
}

bool_a CPDFTable::JoinRowCells(long i_lRow, long i_lBeginCol, long i_lEndCol)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  if (i_lBeginCol < 0 || i_lBeginCol >= rowInfo.GetColCount())
    return false_a;
  if (i_lEndCol < 0 || i_lEndCol >= rowInfo.GetColCount())
    return false_a;
  if (i_lBeginCol >= i_lEndCol)
    return false_a;
  long lWidth = 0, lDif = i_lEndCol - i_lBeginCol;
  long j;
  rec_a recPosSize;
  CPDFCell *pCell = NULL;
  bool_a bRet = true_a;
  for (j = i_lBeginCol; j <= i_lEndCol; j++)
  {
    pCell = GetCell(i_lRow, j);
    if (pCell->IsOverRowCell())
      return false_a;
  }
  long lColCount = rowInfo.GetColCount();
  for (j = i_lBeginCol; j <= i_lEndCol; j++)
  {
    pCell = GetCell(cell(i_lRow, j));
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();
    lWidth += (long)(recPosSize.lRight - recPosSize.lLeft);
    if (j != i_lBeginCol)
    {
      if (!DeleteCell(i_lRow, j))
        bRet = false_a;
    }
  }
  for (j = i_lEndCol + 1; j < lColCount; j++)
  {
    pCell = RemoveCell(i_lRow, j);
    _ASSERT(pCell);
    if (!pCell)
      bRet = false_a;
    else
    {
      if (pCell->IsOverRowCell())
      {
        if (pCell->GetCellPos() == cell(i_lRow, j))
          pCell->SetCellPos(i_lRow, j - lDif);
        if (!InsertCell(i_lRow, j - lDif, pCell))
          bRet = false_a;
      }
      else
      {
        pCell->SetCellPos(i_lRow, j - lDif);
        if (!InsertCell(i_lRow, j - lDif, pCell))
          bRet = false_a;
      }
    }
  }
  pCell = GetCell(cell(i_lRow, i_lBeginCol));
  pCell->SetWidth(lWidth);

  pCell->SetBorderType(pCell->GetBorderType());

  return bRet;
}

bool_a CPDFTable::JoinColCells(long i_lCol, long i_lBeginRow, long i_lEndRow)
{
  if (i_lBeginRow >= i_lEndRow)
    return false_a;
  if (i_lBeginRow < 0 || i_lBeginRow >= GetRowCount())
    return false_a;
  if (i_lEndRow < 0 || i_lEndRow >= GetRowCount())
    return false_a;
  rec_a recPosSize = {0}, recPSTemp;
  CPDFCell *pCell = NULL, *pTempCell = NULL;
  bool_a bRet = true_a;
  long lOver = 0;
  long i;
  cell Cell;
  CRowInfo rowInfo;
  for (i = i_lBeginRow; i <= i_lEndRow; i++)
  {
    rowInfo = m_RowInfo[i];
    if (i_lCol < 0 || i_lCol >= rowInfo.GetColCount())
      return false_a;
    pCell = GetCell(i, i_lCol);
    _ASSERT(pCell);
    if (i == i_lBeginRow)
    {
      recPosSize = pCell->GetPosSize();
    }
    else
    {
      recPSTemp = pCell->GetPosSize();
      if ((recPosSize.lRight != recPSTemp.lRight) || (recPosSize.lLeft != recPSTemp.lLeft))
        return false_a;
    }
  }
  long lHeight = 0;
  for (i = i_lBeginRow; i <= i_lEndRow; i++)
  {
    pCell = GetCell(i, i_lCol);
    _ASSERT(pCell);
    recPosSize = pCell->GetPosSize();
    lHeight += (long)(recPosSize.lTop - recPosSize.lBottom);
    if (i != i_lBeginRow)
    {
/*
      Cell = pCell->GetCellPos();
      lOver = pCell->GetOverRowCount();
        for (long lRow = Cell.m_lRow + lOver; lRow > Cell.m_lRow; lRow--)
        {
          if (!RemoveCell(lRow, Cell.m_lCol))
            return false_a;
          if (!InsertCell(lRow, Cell.m_lCol, i_pTable))
            return false_a;
        }
*/
      if (!DeleteCell(i, i_lCol))
        bRet = false_a;
      InsertCell(i, i_lCol, GetCell(i_lBeginRow, i_lCol));
    }
    else
      pTempCell = pCell;
  }
  pCell = GetCell(i_lBeginRow, i_lCol);
  lOver = pCell->GetOverRowCount();
  if (lOver > 0)
  {
    lOver = (i_lBeginRow - pCell->GetCellPos().m_lRow) + (i_lEndRow - i_lBeginRow);
    pCell->SetOverRowCount(lOver);
  }
  else
    pCell->SetOverRowCount(i_lEndRow - i_lBeginRow);
  pCell->SetHeight(lHeight);

  pCell->SetBorderType(pCell->GetBorderType());

  return bRet;
}

bool_a CPDFTable::SplitCellHorizontal(long i_lRow, long i_lCol)
{
  CPDFCell *pCell, *pNewCell;
  rec_a recPosSize;
  bool_a bRet = true_a;
  pCell = GetCell(i_lRow, i_lCol);
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  long lWidth, lColCount = rowInfo.GetColCount() - 1;
  for (long j = lColCount; j > i_lCol; j--)
  {
    pCell = RemoveCell(i_lRow, j);
    _ASSERT(pCell);
    if (pCell)
    {
      pCell->SetCellPos(cell(i_lRow, j + 1));
      if (!InsertCell(i_lRow, j + 1, pCell))
        bRet = false_a;
    }
    else
      bRet = false_a;
  }
  pCell = GetCell(i_lRow, i_lCol);
  recPosSize = pCell->GetPosSize();
  lWidth = (recPosSize.lRight - recPosSize.lLeft);
  pCell->SetWidth(lWidth / 2);
  if (pCell->IsTable())
    pNewCell = (CPDFCell *)(((CPDFTable *)pCell)->Clone());
  else
    pNewCell = (CPDFCell *)(((CPDFTableCell *)pCell)->Clone());
  if (lWidth % 2)
    pNewCell->SetWidth((lWidth / 2) + 1);
  if (!pNewCell->MoveTo(recPosSize.lLeft + (lWidth / 2), recPosSize.lTop))
    bRet = false_a;
  pNewCell->SetCellPos(i_lRow, i_lCol + 1);
  if (!InsertCell(i_lRow, i_lCol + 1, pNewCell))
    bRet = false_a;
  return bRet;
}

bool_a CPDFTable::SplitCellVertical(long i_lRow, long i_lCol)
{
  CPDFCell *pCell = NULL, *pNewCell = NULL;
  rec_a recPosSize;
  long lOverRowCount, lHeight;
  cell Cell;
  bool_a bRet = true_a;
  pCell = GetCell(i_lRow, i_lCol);
  _ASSERT(pCell);
  if (!pCell)
    return false_a;
  if (!pCell->IsOverRowCell())
    return false_a;
  lOverRowCount = pCell->GetOverRowCount();
  Cell = pCell->GetCellPos();
  recPosSize = pCell->GetPosSize();
  CRowInfo rowInfo = m_RowInfo[Cell.m_lRow];

  if (!pCell->SetHeight(rowInfo.GetRowHeight()))
    bRet = false_a;
  pCell->SetOverRowCount(0);

  lHeight = 0;
  for (long i = Cell.m_lRow + 1; i <= Cell.m_lRow + lOverRowCount; i++)
  {
    rowInfo = m_RowInfo[i - 1];
    if (!RemoveCell(i, Cell.m_lCol))
      bRet = false_a;
    lHeight += rowInfo.GetRowHeight();
    if (pCell->IsTable())
      pNewCell = (CPDFCell *)(((CPDFTable *)pCell)->Clone());
    else
      pNewCell = (CPDFCell *)(((CPDFTableCell *)pCell)->Clone());
    rowInfo = m_RowInfo[i];
    if (!pNewCell->SetHeight(rowInfo.GetRowHeight()))
      bRet = false_a;
    pNewCell->SetCellPos(i, Cell.m_lCol);
    if (!pNewCell->MoveTo(recPosSize.lLeft, recPosSize.lTop - lHeight))
      bRet = false_a;
    if (!InsertCell(i, Cell.m_lCol, pNewCell))
      bRet = false_a;
  }

  return bRet;
}

void CPDFTable::SetDocument(CPDFDocument *i_pDoc)
{
  m_pDoc = i_pDoc;
  CPDFCell *pCell;
  CRowInfo rowInfo;

  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    long lColCount = rowInfo.GetColCount();
    for (long lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (pCell)
      {
        pCell->SetDocument(i_pDoc);
      }
    }
  }
}

void CPDFTable::SetPage(long i_lPage)
{
  if (m_lPage == i_lPage - 1)
    return;
  m_lPage = i_lPage - 1;
  CPDFCell *pCell;
  CRowInfo rowInfo;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    long lColCount = rowInfo.GetColCount();
    for (long lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (pCell)
      {
        pCell->SetPage(i_lPage);
      }
    }
  }
}

long CPDFTable::GetPageNumber()
{
  return m_lPage + 1;
}

bool_a CPDFTable::ChangeFontID(map <long, long> *i_pFontMap)
{
  CPDFCell *pCell;
  CRowInfo rowInfo;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    long lColCount = rowInfo.GetColCount();
    for (long lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (pCell)
      {
        pCell->ChangeFontID(i_pFontMap);
      }
    }
  }
  return true_a;
}

bool_a CPDFTable::ChangeImageID(map <long, long> *i_pImageMap)
{
  CPDFCell *pCell;
  CRowInfo rowInfo;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    long lColCount = rowInfo.GetColCount();
    for (long lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (pCell)
      {
        pCell->ChangeImageID(i_pImageMap);
      }
    }
  }
  return true_a;
}


bool_a CPDFTable::FlushBackground()
{
  if (m_TableBgMap.size() == 0)
    return true_a;
  map <cell, long, cell>::iterator iterBg;
  CPDFCell *pCell = NULL;
  cell Cell;
  long lImage = NOT_USED;
  long lBCol,lECol, lBRow, lERow, lWidth, lHeight;
  CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber());
  if (!pPage)
    return false_a;
  CPDFTextBox *pTextBox = pPage->GetTextArea();
  if (!pTextBox)
    return false_a;

  PDFSetCurrentPage(GetBaseDoc(), GetPageNumber());
  if (IsDivided())
  {
    PDFSaveGraphicsState(GetBaseDoc());
    rec_a recClip = pTextBox->GetPosSize();
    recClip.lBottom = GetYPos() - GetRowsHeights(GetDividedRowCount(-1));
    PDFSetClippingRect(GetBaseDoc(), recClip);
  }

  for (iterBg = m_TableBgMap.begin(); iterBg != m_TableBgMap.end(); iterBg++)
  {
    Cell = (cell)(iterBg->first);
    lImage = iterBg->second;
    GetRange(Cell.m_lRow, Cell.m_lCol, lBRow, lERow, lBCol, lECol);
    lBRow--;lERow--;
    lBCol--;lECol = lECol < 0 ? lECol : lECol-1;
    if (lBRow == lERow && lBCol == lECol)
      continue;
    if (lBRow == 0 && lERow == (GetRowCount()-1) && lBCol == 0 && lECol == -1)
    {
      lWidth = GetMaxTableCellsWidth();
      lHeight = GetRowsHeights(GetRowCount());
      m_pDoc->AddImageToPage(GetPageNumber(), lImage, GetXPos(), GetYPos() - lHeight, lWidth, lHeight);
      if (IsDivided())
      {
        PDFSetCurrentPage(GetBaseDoc(), GetPageNumber() + 1);
        PDFSaveGraphicsState(GetBaseDoc());

        pPage = m_pDoc->GetPage(GetPageNumber() + 1);
        if (!pPage)
          return false_a;
        pTextBox = pPage->GetTextArea();
        if (!pTextBox)
          return false_a;
        PDFSetClippingRect(GetBaseDoc(), pTextBox->GetPosSize());

        m_pDoc->AddImageToPage(GetPageNumber() + 1, lImage, GetXPos(),
                               pTextBox->GetPosSize().lTop - (GetRowsHeights(GetRowCount()) - GetRowsHeights(GetDividedRowCount(-1))), lWidth, lHeight);

        PDFRestoreGraphicsState(GetBaseDoc());
        PDFSetCurrentPage(GetBaseDoc(), GetPageNumber());
      }
    }
    else if (lBRow != lERow && lBCol == lECol)
    {
      pCell = GetCell(lBRow, lBCol);
      if (!pCell)
        return false_a;
      lWidth = GetMaxTableColWidth(lBCol);
      lHeight = GetRowsHeights(GetRowCount());
      m_pDoc->AddImageToPage(GetPageNumber(), lImage, pCell->GetPosSize().lLeft, GetYPos() - lHeight, lWidth, lHeight);
      if (IsDivided())
      {
        PDFSetCurrentPage(GetBaseDoc(), GetPageNumber() + 1);
        PDFSaveGraphicsState(GetBaseDoc());

        pPage = m_pDoc->GetPage(GetPageNumber() + 1);
        if (!pPage)
          return false_a;
        pTextBox = pPage->GetTextArea();
        if (!pTextBox)
          return false_a;
        PDFSetClippingRect(GetBaseDoc(), pTextBox->GetPosSize());

        m_pDoc->AddImageToPage(GetPageNumber() + 1, lImage, pCell->GetPosSize().lLeft,
                               pTextBox->GetPosSize().lTop - (GetRowsHeights(GetRowCount()) - GetRowsHeights(GetDividedRowCount(-1))), lWidth, lHeight);

        PDFRestoreGraphicsState(GetBaseDoc());
        PDFSetCurrentPage(GetBaseDoc(), GetPageNumber());
      }
    }
    else if (lBRow == lERow && lBCol != lECol)
    {
      pCell = GetCell(lBRow, lBCol);
      if (!pCell)
        return false_a;
      lWidth = GetColsWidth(GetColCount(lBRow), lBRow);
      lHeight = GetRowHeight(lBRow);
      if (!IsDivided() || (lBRow < GetDividedRowCount(-1)))
      {
        m_pDoc->AddImageToPage(GetPageNumber(), lImage, GetXPos(), GetYPos() - GetRowsHeights(lBRow + 1), lWidth, lHeight);
      }
      else
      {
        PDFSetCurrentPage(GetBaseDoc(), GetPageNumber() + 1);
        PDFSaveGraphicsState(GetBaseDoc());

        pPage = m_pDoc->GetPage(GetPageNumber() + 1);
        if (!pPage)
          return false_a;
        pTextBox = pPage->GetTextArea();
        if (!pTextBox)
          return false_a;
        PDFSetClippingRect(GetBaseDoc(), pTextBox->GetPosSize());

        m_pDoc->AddImageToPage(GetPageNumber() + 1, lImage, GetXPos(),
                               pTextBox->GetPosSize().lTop - (GetRowsHeights(lBRow + 1) - GetRowsHeights(GetDividedRowCount(-1))), lWidth, lHeight);

        PDFRestoreGraphicsState(GetBaseDoc());
        PDFSetCurrentPage(GetBaseDoc(), GetPageNumber());
      }
    }
    else
      return false_a;
  }

  if (IsDivided())
    PDFRestoreGraphicsState(GetBaseDoc());

  return true_a;
}

bool_a CPDFTable::FlushBorder()
{
  bool_a bRet = true_a;
  long lType, lWidth;
  //float fR, fG, fB;
  rec_a recPosSize;
  float fDif;
  cell Cell = GetCellPos();
  CPDFColor *pColor;

  lType = GetBorderType();
  recPosSize = GetPosSize();
  if (lType & eLeftBorder && !(lType & eLeftDoubleBorder))
  {
    lWidth = GetBorderWidth(eLeftBorder);
    fDif = 0;
    if (0 == Cell.m_lCol)
    {
      if (lWidth % 2)
      {
        lWidth = lWidth / 2;
        lWidth++;
      }
      else
      {
        lWidth = lWidth / 2;
      }
      fDif = (float)lWidth / 2;
    }
    pColor = GetBorderColor(eLeftBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft + fDif, (float)recPosSize.lTop,
                              (float)recPosSize.lLeft + fDif, (float)recPosSize.lBottom);
    if (!bRet)
      return bRet;
  }
  if (lType & eRightBorder && !(lType & eRightDoubleBorder))
  {
    lWidth = GetBorderWidth(eRightBorder);
    pColor = GetBorderColor(eRightBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lRight, recPosSize.lTop, recPosSize.lRight, recPosSize.lBottom);
    if (!bRet)
      return bRet;
  }
  if (lType & eTopBorder && !(lType & eTopDoubleBorder))
  {
    lWidth = GetBorderWidth(eTopBorder);
    fDif = 0;
    if (0 == Cell.m_lRow)
    {
      if (lWidth % 2)
      {
        lWidth = lWidth / 2;
        lWidth++;
      }
      else
      {
        lWidth = lWidth / 2;
      }
      fDif = (float)lWidth / 2;
    }
    pColor = GetBorderColor(eTopBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft, (float)recPosSize.lTop - fDif,
                              (float)recPosSize.lRight, (float)recPosSize.lTop - fDif);
    if (!bRet)
      return bRet;
  }
  if (lType & eBottomBorder && !(lType & eBottomDoubleBorder))
  {
    lWidth = GetBorderWidth(eBottomBorder);
    pColor = GetBorderColor(eBottomBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft, recPosSize.lBottom, recPosSize.lRight, recPosSize.lBottom);
    if (!bRet)
      return bRet;
  }
  if (lType & eLeftDiagonalBorder)
  {
    lWidth = GetBorderWidth(eLeftDiagonalBorder);
    pColor = GetBorderColor(eLeftDiagonalBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft, recPosSize.lBottom, recPosSize.lRight, recPosSize.lTop);
    if (!bRet)
      return bRet;
  }
  if (lType & eRightDiagonalBorder)
  {
    lWidth = GetBorderWidth(eRightDiagonalBorder);
    pColor = GetBorderColor(eRightDiagonalBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft, recPosSize.lTop, recPosSize.lRight, recPosSize.lBottom);
    if (!bRet)
      return bRet;
  }

  if (lType & eLeftDoubleBorder)
  {
    lWidth = GetBorderWidth(eLeftBorder);
    pColor = GetBorderColor(eLeftBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft + lWidth, recPosSize.lTop,
                            recPosSize.lLeft + lWidth, recPosSize.lBottom);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft - lWidth, recPosSize.lTop,
                            recPosSize.lLeft - lWidth, recPosSize.lBottom);
    if (!bRet)
      return bRet;
  }
  if (lType & eRightDoubleBorder)
  {
    lWidth = GetBorderWidth(eRightBorder);
    pColor = GetBorderColor(eRightBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lRight - lWidth, recPosSize.lTop,
                            recPosSize.lRight - lWidth, recPosSize.lBottom);
    bRet = m_pDoc->DrawLine(recPosSize.lRight + lWidth, recPosSize.lTop,
                            recPosSize.lRight + lWidth, recPosSize.lBottom);
    if (!bRet)
      return bRet;
  }
  if (lType & eTopDoubleBorder)
  {
    lWidth = GetBorderWidth(eTopBorder);
    pColor = GetBorderColor(eTopBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft, recPosSize.lTop - lWidth,
                            recPosSize.lRight, recPosSize.lTop - lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft, recPosSize.lTop + lWidth,
                            recPosSize.lRight, recPosSize.lTop + lWidth);
    if (!bRet)
      return bRet;
  }
  if (lType & eBottomDoubleBorder)
  {
    lWidth = GetBorderWidth(eBottomBorder);
    pColor = GetBorderColor(eBottomBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft, recPosSize.lBottom + lWidth,
                            recPosSize.lRight, recPosSize.lBottom + lWidth);
    bRet = m_pDoc->DrawLine(recPosSize.lLeft, recPosSize.lBottom - lWidth,
                            recPosSize.lRight, recPosSize.lBottom - lWidth);
    if (!bRet)
      return bRet;
  }
  return bRet;
}

bool_a CPDFTable::ReplacePageMarks()
{
  long lRow, lColCount, lCol;
  CRowInfo rowInfo;
  CPDFCell *pCell;
  for (lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    lColCount = rowInfo.GetColCount();
    for (lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (!pCell)
        continue;
      if (pCell->IsTable())
        if (!((CPDFTable *)pCell)->ReplacePageMarks())
          return false_a;
      else
        if (!((CPDFTableCell *)pCell)->ReplacePageMarks())
          return false_a;
    }
  }
  return true_a;
}

bool_a CPDFTable::Flush()
{
  if (IsFlush())
    return true_a;
  CPDFCell *pCell;
  bool_a bRet = true_a;
  CRowInfo rowInfo;
  long lPage = 0, lTemp, lCol, lRow, lColCount;

  if (!ReplacePageMarks())
    return false_a;

  MoveTo(m_lXPos, m_lYPos - GetTopBorder());

  if (!m_bInTable && m_DivHeights.size() > 0)
  {
    if (!MoveCellToNextPage())
      return false_a;
  }

  PDFSetCurrentPage(GetBaseDoc(), GetPageNumber());

  if (m_bInTable)
  {
    PDFSaveGraphicsState(GetBaseDoc());
    PDFSetClippingRect(GetBaseDoc(), m_recPosSize);
  }
  else if (m_pInTextBox) // ToDo JuKo: needs? 
  {
    //CPDFTextBox *pTextBox;
    lPage = 0;
    for (lRow = 0; lRow < GetRowCount(); lRow++)
    {
      rowInfo = m_RowInfo[lRow];
      lColCount = rowInfo.GetColCount();
      for (lCol = 0; lCol < lColCount; lCol++)
      {
        pCell = rowInfo.GetColumn(lCol);
        _ASSERT(pCell);
        if (!pCell)
          continue;
        if (pCell->IsTable())
          lTemp = ((CPDFTable *)pCell)->GetPageNumber();
        else
          lTemp = ((CPDFTableCell *)pCell)->GetPageNumber();
        if (lTemp != lPage)
        {
          lPage = lTemp;
          //pTextBox = m_pInTextBox;
          //for (long ii = GetPageNumber(); ii < lPage; ii++)
          //{
          //  if (!pTextBox)
          //  {
          //    ...
          //    return false_a;
          //  }
          //  pTextBox = pTextBox->GetNextTextBox();
          //}
            //if (pTextBox)
            {
               PDFSetCurrentPage(GetBaseDoc(), lPage);
               PDFSaveGraphicsState(GetBaseDoc());
               //rec_a recTmp = pTextBox->GetPosSize();
//               recTmp.lLeft = 0;
//               recTmp.lRight = pPage->GetWidth();
//               PDFSetClippingRect(GetBaseDoc(), recTmp);
            }
        }
      }
    }
  }
  if (!FlushBackground())
    bRet = false_a;
  for (lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    lColCount = rowInfo.GetColCount();
    for (lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (!pCell)
        continue;
      if (!(pCell->IsFlush()))
      {
        if (!pCell->Flush())
        bRet = false_a;
      }
    }
  }

  for (lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    lColCount = rowInfo.GetColCount();
    for (lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (!pCell || !pCell->FlushBorder())
        bRet = false_a;
    }
  }

  PDFSetCurrentPage(GetBaseDoc(), GetPageNumber());
  if (m_bInTable)
    PDFRestoreGraphicsState(GetBaseDoc());
  else if (m_pInTextBox) // ToDo JuKo: needs? 
  {
    //CPDFTextBox *pTextBox;
    lPage = 0;
    for (lRow = 0; lRow < GetRowCount(); lRow++)
    {
      rowInfo = m_RowInfo[lRow];
      lColCount = rowInfo.GetColCount();
      for (lCol = 0; lCol < lColCount; lCol++)
      {
        pCell = rowInfo.GetColumn(lCol);
        _ASSERT(pCell);
        if (!pCell)
          continue;
        if (pCell->IsTable())
          lTemp = ((CPDFTable *)pCell)->GetPageNumber();
        else
          lTemp = ((CPDFTableCell *)pCell)->GetPageNumber();
        if (lTemp != lPage)
        {
          lPage = lTemp;
          //pTextBox = m_pInTextBox;
          //for (long ii = GetPageNumber(); ii < lPage; ii++)
          //{
          //  if (!pTextBox)
          //  {
          //    ...
          //    return false_a;
          //  }
          //  pTextBox = pTextBox->GetNextTextBox();
          //}
//          CPDFPage *pPage = m_pDoc->GetPage(lPage);
//          if (pPage)
//          {
//            CPDFTextBox *pTextBox = pPage->GetTextArea();
            //if (pTextBox)
            {
               PDFSetCurrentPage(GetBaseDoc(), lPage);
               PDFRestoreGraphicsState(GetBaseDoc());
            }
//          }
        }
      }
    }
  }
  return bRet;
}

bool_a CPDFTable::IsFlush()
{
  CPDFCell *pCell;
  CRowInfo rowInfo;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    long lColCount = rowInfo.GetColCount();
    for (long lCol = 0; lCol < lColCount; lCol++)
    {
      pCell = rowInfo.GetColumn(lCol);
      _ASSERT(pCell);
      if (!pCell || !pCell->IsFlush())
        return false_a;
    }
  }
  return true_a;
}

bool_a CPDFTable::IsInTable()
{
  return m_bInTable;
}

void CPDFTable::ChangeLeftIndents(long i_lWidth)
{
  i_lWidth;
}

void CPDFTable::ChangeRightIndents(long i_lWidth)
{
  i_lWidth;
}

void CPDFTable::ChangeTopIndents(long i_lWidth)
{
  i_lWidth;
}

void CPDFTable::ChangeBottomIndents(long i_lWidth)
{
  i_lWidth;
}

void CPDFTable::SetBorderType(long i_lBorder)
{
  i_lBorder;
}

void CPDFTable::SetBorderWidth(long i_lBorder, long i_lWidth)
{
  if (i_lWidth < 1)
    i_lWidth = 1;
  if (m_bInTable)
  {
    long lLeft, lRight, lTop, lBottom;
    lLeft = GetBorderWidth(eLeftBorder);
    lRight = GetBorderWidth(eRightBorder);
    lTop = GetBorderWidth(eTopBorder);
    lBottom = GetBorderWidth(eBottomBorder);
    if ((i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder)
        && (i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder))
      MoveTo(m_lXPos + (i_lWidth - lLeft), m_lYPos - (i_lWidth - lTop));
    else
      if ((i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder))
        MoveTo(m_lXPos + (i_lWidth - lLeft), m_lYPos);
      else
        if ((i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder))
          MoveTo(m_lXPos, m_lYPos - (i_lWidth - lTop));
    rec_a recPosSize = GetPosSize();
    if (i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder)
      recPosSize.lLeft -= (i_lWidth - lLeft);
    if (i_lBorder & eRightBorder || i_lBorder & eRightDoubleBorder)
      recPosSize.lRight -= (i_lWidth - lRight);
    if (i_lBorder & eBottomBorder || i_lBorder & eBottomDoubleBorder)
      recPosSize.lBottom += (i_lWidth - lBottom);
    if (i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder)
      recPosSize.lTop += (i_lWidth - lTop);
    SetClippingRect(recPosSize);
  }
  CPDFCell::SetBorderWidth(i_lBorder, i_lWidth);
}

bool_a CPDFTable::ChangeCellsIndents(cell i_Cell, long i_lBorder, long i_lLeft,
                                    long i_lTop, long i_lRight, long i_lBottom)
{
  i_lBorder;
  CPDFCell *pCell = GetCell(i_Cell);
  if (!pCell)
    return false_a;
  vector<cell> Cells;
  rec_a recPosSize = pCell->GetPosSize();
  cell Cell = pCell->GetCellPos();
  long lOver = pCell->GetOverRowCount();
  long lCount, i;

  Cells = GetCellsBetweenRange(Cell.m_lRow - 1, recPosSize.lLeft - i_lLeft, recPosSize.lRight + i_lRight);
  lCount = (long)Cells.size();
  for (i = 0; i < lCount; i++)
  {
    pCell = GetCell(Cells[i]);
    if (pCell)
    {
      ((CPDFTableCell *)pCell)->ChangeBottomIndents(i_lBottom);
    }
  }
  Cells = GetCellsBetweenRange(Cell.m_lRow + lOver + 1, recPosSize.lLeft - i_lLeft, recPosSize.lRight + i_lRight);
  lCount = (long)Cells.size();
  for (i = 0; i < lCount; i++)
  {
    pCell = GetCell(Cells[i]);
    if (pCell)
    {
      ((CPDFTableCell *)pCell)->ChangeTopIndents(i_lTop);
    }
  }
  Cells = GetLeftNeighbours(Cell);
  lCount = (long)Cells.size();
  for (i = 0; i < lCount; i++)
  {
    pCell = GetCell(Cells[i]);
    if (pCell)
    {
      ((CPDFTableCell *)pCell)->ChangeRightIndents(i_lLeft);
    }
  }
  Cells = GetRightNeighbours(Cell);
  lCount = (long)Cells.size();
  for (i = 0; i < lCount; i++)
  {
    pCell = GetCell(Cells[i]);
    if (pCell)
    {
      ((CPDFTableCell *)pCell)->ChangeLeftIndents(i_lRight);
    }
  }
  return true_a;
}

vector<cell> CPDFTable::GetLeftNeighbours(cell i_Cell)
{
  vector<cell> retVector;
  CPDFCell *pCell = GetCell(i_Cell), *pTempCell;
  if (!pCell)
    return retVector;
  long lOver = pCell->GetOverRowCount();
  CRowInfo rowInfo;
  for (long lRow = i_Cell.m_lRow; lRow <= i_Cell.m_lRow + lOver; lRow++)
  {
    if (lRow == i_Cell.m_lRow)
    {
      if (0 != i_Cell.m_lCol)
      {
        pCell = GetCell(lRow, i_Cell.m_lCol - 1);
        retVector.push_back(pCell->GetCellPos());
      }
    }
    else
    {
      pCell = NULL;
      rowInfo = m_RowInfo[lRow];
      for (long lCol = 0; lCol < rowInfo.GetColCount() - 1; lCol++)
      {
        pTempCell = GetCell(lRow, lCol);
        if (pTempCell->GetCellPos() == i_Cell)
        {
          if (pCell)
            retVector.push_back(pCell->GetCellPos());
        }
        else
          pCell = pTempCell;
      }
    }
  }
  return retVector;
}

vector<cell> CPDFTable::GetRightNeighbours(cell i_Cell)
{
  vector<cell> retVector;
  CPDFCell *pCell = GetCell(i_Cell), *pTempCell;
  if (!pCell)
    return retVector;
  long lOver = pCell->GetOverRowCount();
  CRowInfo rowInfo;
  for (long lRow = i_Cell.m_lRow; lRow <= i_Cell.m_lRow + lOver; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    if (lRow == i_Cell.m_lRow)
    {
      if (i_Cell.m_lCol < rowInfo.GetColCount() - 1)
      {
        pCell = GetCell(lRow, i_Cell.m_lCol + 1);
        retVector.push_back(pCell->GetCellPos());
      }
    }
    else
    {
      pCell = NULL;
      for (long lCol = rowInfo.GetColCount() - 1; lCol > 0; lCol--)
      {
        pTempCell = GetCell(lRow, lCol);
        if (pTempCell->GetCellPos() == i_Cell)
        {
          if (pCell)
            retVector.push_back(pCell->GetCellPos());
        }
        else
          pCell = pTempCell;
      }
    }
  }
  return retVector;
}

vector<cell> CPDFTable::GetCellsBetweenRange(long i_lRow, long i_lLeft, long i_lRight)
{
  vector<cell> retVector;
  CPDFCell *pCell = NULL;
  rec_a recPosSize;
  if (i_lRow >= GetRowCount() || i_lRow < 0)
    return retVector;
  CRowInfo rowInfo;
  rowInfo = m_RowInfo[i_lRow];
  for (long lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    if (pCell)
    {
      recPosSize = pCell->GetPosSize();
      if ((recPosSize.lRight > i_lLeft && recPosSize.lLeft < i_lRight))
        retVector.push_back(pCell->GetCellPos());
    }
  }
  return retVector;
}

void CPDFTable::SetClippingRect(rec_a i_Rec)
{
  COPY_REC(m_recPosSize, i_Rec);
}

void CPDFTable::SetInTable(bool_a i_bInTable)
{
  m_bInTable = i_bInTable;
  m_pDoc->RemoveTable(this);
}

void CPDFTable::SetInTextBox(CPDFTextBox *i_pTextBox)
{
  m_pInTextBox = i_pTextBox;
}

long CPDFTable::GetColsWidth(long i_lEndCol, long i_lRow)
{
  long lRet = 0;
  CPDFCell *pCell;
  rec_a rec;
  if (i_lEndCol < 0)
    return lRet;

  for (long i = 0; i < i_lEndCol; i++)
  {
    pCell = GetCell(i_lRow, i);
    if (pCell)
    {
      rec = pCell->GetPosSize();
      lRet += (long)(rec.lRight - rec.lLeft);
    }
    else
      return 0;
  }
  return lRet;
}

long CPDFTable::GetTopBorder(long i_lRow)
{
  if (GetRowCount() <= 0)
    return 0;
  long lH = 0, lType = 0, i;
  CPDFCell *pCell;
  if (i_lRow == NOT_USED)
    i_lRow = 0;
  CRowInfo rowInfo = m_RowInfo[i_lRow];
  for (i = 0; i < rowInfo.GetColCount(); i++)
  {
    pCell = GetCell(i_lRow, i);
    _ASSERT(pCell);
    if (pCell)
    {
      lType = pCell->GetBorderType();
      if (lType & eTopDoubleBorder)
      {
        if (lH < pCell->GetBorderWidth(eTopDoubleBorder) / 2)
          lH = pCell->GetBorderWidth(eTopDoubleBorder) / 2;
      }
      else if (lType & eTopBorder)
        if (lH < pCell->GetBorderWidth(eTopBorder) / 2)
          lH = pCell->GetBorderWidth(eTopBorder) / 2;
    }
  }
  return lH;
}

long CPDFTable::GetBottomBorder(long i_lRow)
{
  if (GetRowCount() <= 0)
    return 0;
  long lH = 0, lType = 0, i;
  CPDFCell *pCell;
  if (i_lRow == NOT_USED)
    i_lRow = GetRowCount() - 1;
  CRowInfo rowInfo = m_RowInfo[i_lRow];
  for (i = 0; i < rowInfo.GetColCount(); i++)
  {
    pCell = GetCell(i_lRow, i);
    _ASSERT(pCell);
    if (pCell)
    {
      lType = pCell->GetBorderType();
      if (lType & eBottomDoubleBorder)
      {
        if (lH < pCell->GetBorderWidth(eBottomDoubleBorder) / 2)
          lH = pCell->GetBorderWidth(eBottomDoubleBorder) / 2;
      }
      else if (lType & eBottomBorder)
        if (lH < pCell->GetBorderWidth(eBottomBorder) / 2)
          lH = pCell->GetBorderWidth(eBottomBorder) / 2;
    }
  }
  return lH;
}

long CPDFTable::GetRowsHeights(long i_lEndRow)
{
  long lRet = 0, i;
  long lSize = (long)m_RowInfo.size();
  if (i_lEndRow < 0 || i_lEndRow > lSize)
    return lRet;
  CRowInfo rowInfo;
  for (i = 0; i < i_lEndRow; i++)
  {
    rowInfo = m_RowInfo[i];
    lRet += rowInfo.GetRowHeight();
  }
  long lH = GetTopBorder();
  lRet += lH;
  if (i_lEndRow == GetRowCount())
  {
   lH = GetBottomBorder();
   lRet += lH;
  }
  return lRet;
}

long CPDFTable::GetMaxWidth()
{
  long lRet = 0, lWidth;
  CRowInfo rowInfo;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    lWidth = GetColsWidth(rowInfo.GetColCount(), lRow);
    if (lRet < lWidth)
      lRet = lWidth;
  }
  return lRet;
}

void CPDFTable::HeightChanged(long i_lRow)
{
  i_lRow;
  long lWidth, lHeight, lTemp = 1;
  lHeight = GetRowsHeights(GetRowCount());
  lWidth = GetMaxWidth();
  if (m_pParentTable)
    m_pParentTable->ChildTableHeightChanged(GetCellPos(), lHeight);
  if (m_pInTextBox)
  {
    CPDFTextBox *pTextBox = m_pInTextBox;
    if (m_DivHeights.size() > 0)
    {
      for (size_t ii = 0; ii < m_DivHeights.size(); ii++)//TODO: What is this?!!!!!!!!!!!!!!!!!!! m_DivHeights is not used!!
      {
        pTextBox = GetInTextBoxNext(lTemp);

        //if (!pTextBox->GetNextTextBox())
        //{
        //  CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber() + lTemp);
        //  if (!pPage)
        //    return;
        //  pTextBox = pPage->GetTextArea();
        //  if (!pTextBox)
        //    return;
        //}
        //else
        //  pTextBox = pTextBox->GetNextTextBox();

        lTemp++;
      }
      if (pTextBox)
        pTextBox->AddTable(this, -1, GetDividedHeight(GetPageNumber() + (long)m_DivHeights.size()));
    }
    else
      pTextBox->AddTable(this);
  }
}

bool_a CPDFTable::EndEdit()
{
  if (!m_pInTextBox)
    return true_a;
  CPDFTextBox *pTextBox = m_pInTextBox;
  long lTemp = 1;
  if (m_DivHeights.size() > 0)
  {
    for (size_t ii = 0; ii < m_DivHeights.size(); ii++)//TODO: What is this?!!!!!!!!!!!!!!!!!!! m_DivHeights is not used!!
    {
      pTextBox = GetInTextBoxNext(lTemp);

      //if (!pTextBox->GetNextTextBox())
      //{
      //  CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber() + lTemp);
      //  if (!pPage)
      //    return false_a;
      //  pTextBox = pPage->GetTextArea();
      //  if (!pTextBox)
      //    return false_a;
      //  lTemp++;
      //}
      //else
      //  pTextBox = pTextBox->GetNextTextBox();

      lTemp++;
    }
    if (pTextBox && pTextBox->IsTableLast())
      pTextBox->AddText((char *)czNEWPARAGRAPH_2);
  }
  else
  {
    if (pTextBox->IsTableLast())
      pTextBox->AddText((char *)czNEWPARAGRAPH_2);
  }
  return true_a;
}

void CPDFTable::SetHeaderRow(long i_lRow, bool_a i_bSet)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return;
  bool_a bH = m_RowInfo[i_lRow].Header();
  if (bH == i_bSet)
    return;
  m_RowInfo[i_lRow].SetHeader(i_bSet);
  m_RowInfo[i_lRow].SetFixed(i_bSet);
}

void CPDFTable::SetFixedRow(long i_lRow, bool_a i_bSet)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return;
  m_RowInfo[i_lRow].SetFixed(i_bSet);
}

bool_a CPDFTable::HeaderRow(long i_lRow)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  return m_RowInfo[i_lRow].Header();
}

bool_a CPDFTable::KeepRowTogether(long i_lRow)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return false_a;
  return m_RowInfo[i_lRow].KeepTogether();
}

void CPDFTable::SetKeepRowTogether(long i_lRow, bool_a i_bKeepTogether)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return;
  bool_a bKeep = m_RowInfo[i_lRow].KeepTogether();
  if (bKeep == i_bKeepTogether)
    return;
  m_RowInfo[i_lRow].SetKeepTogether(i_bKeepTogether);
}

long CPDFTable::GetTogetherRowHeight(long &i_lRow)
{
  long lRet = 0;
  long lRow;
  CRowInfo rowInfo;
  bool_a bT = true_a, bF = true_a;
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return lRet;

  for (lRow = i_lRow; lRow < GetRowCount(); lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    if (!rowInfo.Fixed())
      bT = false_a;
    if (!rowInfo.Header())
      bF = false_a;

    if ( (bT || (lRow != i_lRow && m_RowInfo[lRow - 1].Header()))
        && (bF || (!bF && !m_RowInfo[lRow].Header())) )
    {
      lRet += rowInfo.GetRowHeight();
    }
    else
      break;
  }
  if (lRow == i_lRow)
  {
    lRet = rowInfo.GetRowHeight();
    lRow++;
  }
  i_lRow = lRow;
  return lRet;
}

bool_a CPDFTable::MoveCellToNextPage()
{
  long lPage = 0, lRet = 0;
  long lHeight = -1, lTopBorderH = 0, lBottomBorderH = 0;
  long lSize = GetRowCount();
  long lTempHeight = 0, lRowTem = 0, lNextRow = 0;
  bool_a bSame = true_a;
  CPDFCell *pCell = NULL;
  if (lPage < (long)m_DivHeights.size())
    lHeight = m_DivHeights[lPage].m_lHeight;
  CRowInfo rowInfo;
  lTopBorderH = GetTopBorder(0);
  for (long lRow = 0; lRow < lSize; lRow++)
  {
    lNextRow = lRow;
    lRet += GetTogetherRowHeight(lNextRow);

    lBottomBorderH = GetBottomBorder(lNextRow - 1);

    if (lHeight > 0 && (lRet + lTopBorderH + lBottomBorderH) > lHeight)
    {
      lRet = 0;

      lTempHeight = 0;
      if (lHeight > 0 && bSame)
        lRowTem = lNextRow;
      else
        lRowTem = lRow;

      lPage++;
      if (lPage < (long)m_DivHeights.size())
        lHeight = m_DivHeights[lPage].m_lHeight;
      else
        lHeight = -1;
      {
                          long lTempPage = lPage, lTemp = 1;
                          //CPDFPage *pPage = NULL;
                          CPDFTextBox *pTextArea = m_pInTextBox;
                          if (!pTextArea)
                            return false_a;
                          long lX = pTextArea->GetLeftDrawPos();
                          long lDif = 0;

                          while (pTextArea)
                          {
                              pTextArea = GetInTextBoxNext(lTemp);

                              //pTextArea = pTextArea->GetNextTextBox();
                              //if (!pTextArea)
                              //{
                              //  pPage = m_pDoc->GetPage(GetPageNumber() + lTemp);
                              //  if (!pPage)
                              //    return false_a;
                              //  pTextArea = pPage->GetTextArea();
                              //  if (!pTextArea)
                              //    return false_a;
                              //}
                              lTemp++;
                              if (pTextArea)
                              {
                                lDif = pTextArea->GetLeftDrawPos() - lX;
                                lX = pTextArea->GetLeftDrawPos();
                              }
                              lTempPage--;
                              if (lTempPage <= 0)
                              {
                                break;
                              }
                          }

                          if (!pTextArea)
                            return false_a;

                          long lY = pTextArea->GetTopDrawPos() - lTopBorderH;
/////////////////////////////////////////////////////////
                          //long lBorderType = 0, lCol;
                          //if (lRowTem > 0)
                          //{
                          //  for (lCol = 0; lCol < GetColCount(lRowTem); lCol++)
                          //  {
                          //    pCell = GetCell(lRowTem, lCol);
                          //    if (pCell)
                          //    {
                          //      lBorderType = pCell->GetBorderType();
                          //      if (!(lBorderType & CPDFCell::eTopBorder) & !(lBorderType & CPDFCell::eTopDoubleBorder))
                          //        ((CPDFCell*)pCell)->CPDFCell::SetBorderType(pCell->GetBorderType() | CPDFCell::eTopBorder);
                          //    }
                          //  }
                          //}
/////////////////////////////////////////////////////////
                          //for (lCol = 0; lCol < GetColCount(lRowTem - 1); lCol++)
                          //{
                          //  pCell = GetCell(lRowTem - 1, lCol);
                          //  if (pCell)
                          //  {
                          //    lBorderType = pCell->GetBorderType();
                          //    if (!(lBorderType & CPDFCell::eBottomBorder) & !(lBorderType & CPDFCell::eBottomDoubleBorder))
                          //      ((CPDFCell*)pCell)->CPDFCell::SetBorderType(pCell->GetBorderType() | CPDFCell::eBottomBorder);
                          //  }
                          //}
/////////////////////////////////////////////////////////
                          for (long lMoveRow = lRowTem;lMoveRow < GetRowCount(); lMoveRow++)
                          {
                            rec_a recPosSize;
                            //long lDif = 0;
                            rowInfo = m_RowInfo[lMoveRow];
                            for (long lCol = 0; lCol < rowInfo.GetColCount(); lCol++)
                            {
                              pCell = GetCell(lMoveRow, lCol);
                              recPosSize = pCell->GetPosSize();
                              //if (!lCol)
                              //  lDif = lX - recPosSize.lLeft;
                              if (pCell)
                              {
                                pCell->SetPage(pTextArea->GetPageNumber());
                                pCell->MoveTo(recPosSize.lLeft + lDif, lY - lTempHeight);
                              }
                            }
                            lTempHeight += rowInfo.GetRowHeight();
                          }
      }
//      lRowTem = lRow + 1;
      if (!bSame)
        lNextRow = lRow;

      bSame = true_a;

      lTopBorderH = GetTopBorder(lNextRow - 1);

    }
    else
      bSame = false_a;
    lRow = lNextRow - 1;
  }

  return true_a;
}

long CPDFTable::DivideTable(long i_lHeight, long i_lPage)
{
  long lPage = GetPageNumber();
  i_lPage -= lPage;
  if (i_lHeight < 0 || i_lPage < 0 || i_lPage > (long)m_DivHeights.size())
    return NOT_USED;
  heightrow strHeight;

  if (i_lPage < (long)m_DivHeights.size())
  {
    m_DivHeights[i_lPage].m_lHeight = i_lHeight;
  }
  else
  {
    strHeight.m_lHeight = i_lHeight;
    strHeight.m_lRow = -1;
    m_DivHeights.push_back(strHeight);
  }
  long lRet = GetDividedHeight(lPage + i_lPage + 1);//get height of rows in next page
  if (lRet > 0)
  {
    long lBRow, lERow;
    //if (!KeepTogether)
    {
      if (!DivideRow(lPage + i_lPage + 1))
        return NOT_USED;
    }
    if (!GetRowsRangeAtPage(lPage + i_lPage + 1, lBRow, lERow))
      return NOT_USED;

    lRet = 0;
    for (long i = lBRow; i <= lERow; i++)
    {
      lRet += GetRowHeight(i);
    }
    lRet += GetTopBorder(lBRow);
    lRet += GetBottomBorder(lERow);

    if (!InsertHeaderRows(lPage + i_lPage + 1))
      return NOT_USED;
    if (lBRow != NOT_USED && lERow != NOT_USED)
      lRet += GetHeaderRowHeight();
  }
  else
  {
    m_DivHeights.pop_back();
  }
  return lRet;
}

bool_a CPDFTable::IsDivided()
{
  if (m_DivHeights.size() > 0)
    return true_a;
  return false_a;
}

long CPDFTable::GetDivideCount()
{
  return m_DivHeights.size();
}

long CPDFTable::GetDividedHeight(long i_lPage)
{
  i_lPage -= GetPageNumber();
  if (i_lPage < 0 || i_lPage > (long)m_DivHeights.size())
    return -1;
  if (i_lPage < (long)m_DivHeights.size())
    return m_DivHeights[i_lPage].m_lHeight;
  long lPage = 0, lRet = 0, lNextRow;
  long lHeight = -1, lTopBorderH = 0, lBottomBorderH = 0;
  long lSize = GetRowCount();
  long lRow, lTemp;
  bool_a bSame = true_a;

  if (lPage < (long)m_DivHeights.size())
    lHeight = m_DivHeights[lPage].m_lHeight;
  lTopBorderH = GetTopBorder(0);
  for (lRow = 0, lTemp = 0; lRow < lSize; lRow++)
  {
    lNextRow = lRow;
    lRet += GetTogetherRowHeight(lNextRow);

    lBottomBorderH = GetBottomBorder(lNextRow - 1);

    if (lHeight > 0 && (lRet/* + lTopBorderH*/ + lBottomBorderH) > lHeight)
    {
      //try to divide row
      if (!m_RowInfo[lNextRow - 1].KeepTogether() &&
          (lNextRow - 1 == lRow // one row
          || (lNextRow - 1 > 0 && m_RowInfo[lNextRow - 2].Header()))) //header rows with only one '' row
      {
        lRet = GetDividedRowHeight(lNextRow - 1, (lRet/* + lTopBorderH*/ + lBottomBorderH) - lHeight);
        if (lRet == NOT_USED)
        {
          lTopBorderH = GetTopBorder(lNextRow - 1);
          lRet = 0;
        }
        else
          lTopBorderH = GetBottomBorder(lNextRow - 1);
      }
      else
      {
        lTopBorderH = GetTopBorder(lNextRow - 1);
        lRet = 0;
      }

      lPage++;
      if (lHeight > 0 && bSame
          && (lNextRow >= lSize || !CanBeMoved(GetPageNumber() + (lPage - 1)))
          && !lRet)
      {
        lRet = 0;
        if (lPage == i_lPage)
          return lRet;
      }

      if ((lNextRow >= lSize || !bSame) && !lRet)
      {
        lNextRow = lRow;
      }

      bSame = true_a;
      lTemp = lRow;
      if (lPage < (long)m_DivHeights.size())
        lHeight = m_DivHeights[lPage].m_lHeight;
      else
        lHeight = -1;
    }
    else
      bSame = false_a;
    lRow = lNextRow - 1;
  }

  lRet += /*lTopBorderH + */lBottomBorderH;

  if (lHeight > 0 && lTemp == lRow - 1)
    lRet = 0;

  return lRet;
}

long CPDFTable::GetDividedRowHeight(long i_lRow, long i_lHeight)
{
  if (i_lRow < 0 || i_lRow >= GetRowCount())
    return NOT_USED;

  long lCol, lH, lRow, lRet = NOT_USED;
  CPDFCell *pCell;
  for (lCol = 0; lCol < GetColCount(i_lRow); lCol++)
  {
    pCell = GetCell(i_lRow, lCol);
    _ASSERT(pCell);
    if (pCell)
    {
      lH = 0;
      if (pCell->IsTable())
        return NOT_USED;
      else
      {
        cell Cell = pCell->GetCellPos();
        for (lRow = Cell.m_lRow; lRow <= i_lRow; lRow++)
        {
          lH += m_RowInfo[lRow].GetRowHeight();
        }
        lH = ((CPDFTableCell *)pCell)->GetDividedHeight(lH - i_lHeight);
        if (lH == NOT_USED)
           return lH;
        for (lRow = i_lRow + 1; lRow < Cell.m_lRow + pCell->GetOverRowCount(); lRow++)
        {
          lH -= m_RowInfo[lRow].GetRowHeight();
        }

        if (lRet == NOT_USED || lRet < lH)
          lRet = lH;
      }
    }
  }
  return lRet;
}

bool_a CPDFTable::GetRowsRangeAtPage(long i_lPage, long &i_lBRow, long &i_lERow)
{
  i_lPage -= GetPageNumber();
  if (i_lPage < 0 || i_lPage > (long)m_DivHeights.size())
    return false_a;

  long lPage = 0, lRet = 0, lNextRow, lRow;
  long lHeight = -1, lTopBorderH = 0, lBottomBorderH = 0;
  long lSize = GetRowCount();
  bool_a bSame = true_a;
  if (lPage < (long)m_DivHeights.size())
    lHeight = m_DivHeights[lPage].m_lHeight;
  i_lBRow = 0;
  lTopBorderH = GetTopBorder(0);
  for (lRow = 0; lRow < lSize; lRow++)
  {
    lNextRow = lRow;
    lRet += GetTogetherRowHeight(lNextRow);

    lBottomBorderH = GetBottomBorder(lNextRow - 1);

    if (lHeight > 0 && (lRet + lTopBorderH + lBottomBorderH) > lHeight)
    {
      if (lPage == i_lPage)
      {
        i_lERow = lNextRow - 1;
        return true_a;
      }
      lRet = 0;
      lPage++;
      if (lPage < (long)m_DivHeights.size())
        lHeight = m_DivHeights[lPage].m_lHeight;
      else
        lHeight = -1;
      if (bSame)
        i_lBRow = lNextRow;
      else
        i_lBRow = lRow;
      if (!bSame)
        lNextRow = lRow;

      lTopBorderH = GetTopBorder(lNextRow - 1);

      bSame = true_a;
    }
    else
      bSame = false_a;
    lRow = lNextRow - 1;
  }
  i_lERow = lRow - 1;
  if (lPage != i_lPage || i_lBRow >= lSize)
  {
    i_lBRow = NOT_USED;
    i_lERow = NOT_USED;
  }
  return true_a;
}

bool_a CPDFTable::CanBeMoved(long i_lPage)
{
  long lTemp = 1;
  i_lPage -= GetPageNumber();
  CPDFTextBox *pTextArea = m_pInTextBox;
  while (i_lPage > 0 && pTextArea)
  {
    pTextArea = GetInTextBoxNext(lTemp);
    //pTextArea = pTextArea->GetNextTextBox();
    //if (!pTextArea)
    //{
    //  CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber() + lTemp);
    //  if (!pPage)
    //    return false_a;
    //  pTextArea = pPage->GetTextArea();
    //  if (!pTextArea)
    //    return false_a;
    //}
    lTemp++;
    i_lPage--;
  }
  if (!pTextArea)
    return false_a;
  long lBRow, lERow;
  if (!pTextArea->IsTableFirst(this))
  {
    i_lPage += GetPageNumber();
    GetRowsRangeAtPage(i_lPage, lBRow, lERow);
    if (lBRow != NOT_USED && lERow != NOT_USED)
      return false_a;
  }
  //if (!pTextArea->HasOnlyTable(this))
  //{
  //  return true_a;
  //}
  return true_a;
}

bool_a CPDFTable::CanBeDivided(long i_lHeight)
{
  i_lHeight;
  return true_a;
}

long CPDFTable::GetDividedRowCount(long i_lPage)
{
  long lSize = (long)m_RowInfo.size();
  long lHeight = 0;
  long lRow;
  CRowInfo rowInfo;
  for (lRow = 0; lRow < lSize; lRow++)
  {
    rowInfo = m_RowInfo[lRow];
    lHeight += rowInfo.GetRowHeight();
    if (lHeight > m_DivHeights[0].m_lHeight)
      break;
  }
  return lRow;
}

void CPDFTable::WidthChanged(long i_lRow)
{
  i_lRow;
}

void CPDFTable::ChildTableHeightChanged(cell i_Cell, long i_lHeight)
{
  if (i_Cell.m_lRow < 0 || i_Cell.m_lRow >= GetRowCount())
    return ;
  CRowInfo rowInfo = m_RowInfo[i_Cell.m_lRow];
  if (rowInfo.GetRowHeight() != i_lHeight)
    SetRowHeight(i_Cell.m_lRow, i_lHeight);
}

void CPDFTable::GetRange(long i_lRow, long i_lCol, long &o_lBgnRow, long &o_lEndRow, long &o_lBgnCol, long &o_lEndCol)
{
  if (i_lRow > 0 && i_lCol > 0)
  {
    o_lBgnRow = i_lRow;
    o_lEndRow = i_lRow;
    o_lBgnCol = i_lCol;
    o_lEndCol = i_lCol;
    return;
  }
  if (i_lRow > 0 && i_lCol == 0)
  {
    o_lBgnRow = i_lRow;
    o_lEndRow = i_lRow;
    o_lBgnCol = 1;
    o_lEndCol = -1;
    return;
  }
  if (i_lRow == 0 && i_lCol > 0)
  {
    o_lBgnRow = 1;
    o_lEndRow = GetRowCount();
    o_lBgnCol = i_lCol;
    o_lEndCol = i_lCol;
    return;
  }
  if (i_lRow == 0 && i_lCol == 0)
  {
    o_lBgnRow = 1;
    o_lEndRow = GetRowCount();
    o_lBgnCol = 1;
    o_lEndCol = -1;
    return;
  }

  o_lBgnRow = i_lRow;
  o_lEndRow = i_lRow;
  o_lBgnCol = i_lCol;
  o_lEndCol = i_lCol;
  return;
}

long CPDFTable::GetMaxTableCellsWidth()
{
  long lWidth = 0, lTemp;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    lTemp = GetColsWidth(GetColCount(lRow), lRow);
    lWidth = lWidth > lTemp ? lWidth : lTemp;
  }
  return lWidth;
}

long CPDFTable::GetMinTableCellsWidth()
{
  long lWidth = 0, lTemp;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    lTemp = GetColsWidth(GetColCount(lRow), lRow);
    lWidth = lWidth < lTemp ? lWidth : lTemp;
  }
  return lWidth;
}

long CPDFTable::GetMaxTableColWidth(long i_lCol)
{
  long lWidth = 0, lTemp;
  CPDFCell *pCell = NULL;
  for (long lRow = 0; lRow < GetRowCount(); lRow++)
  {
    pCell = GetCell(lRow, i_lCol);
    if (!pCell)
      return false_a;
    lTemp = ((CPDFTableCell *)pCell)->GetWidth();
    lWidth = lWidth > lTemp ? lWidth : lTemp;
  }
  return lWidth;
}

CPDFTextBox *CPDFTable::GetInTextBoxNext(long i_lNext)
{
  if (i_lNext < 0 || !m_pDoc || !m_pInTextBox)
    return NULL;
  if (!i_lNext)
    return m_pInTextBox;

  CPDFTextBox *pRetTB = m_pInTextBox;
  long lTmpCell = 1;
  long lTmpPage = 1;
  CPDFPage *pPage;

  while (i_lNext > 0)
  {
    pRetTB = pRetTB->GetNextTextBox();

    if (!pRetTB)
    {
      switch (m_pInTextBox->GetTBType())
      {
        default:
        case CPDFTextBox::eTBFrame:
        case CPDFTextBox::eTBHeaderFooter:
        case CPDFTextBox::eTBTextBox:
        {
          pPage = m_pDoc->GetPage(m_pInTextBox->GetPageNumber() + lTmpPage);
          if (!pPage)
            return NULL;
          pRetTB = pPage->GetTextArea();
          if (!pRetTB)
            return NULL;
          lTmpPage++;
          break;
        }
        case CPDFTextBox::eTBTableCell:
        {
          cell cellPos = ((CPDFTableCell *)m_pInTextBox)->GetCellPos();
          cellPos.m_lRow += lTmpCell;
          if (NULL == ((CPDFTableCell *)m_pInTextBox)->GetTable())
            return NULL;
          pRetTB = (CPDFTextBox *)(CPDFTableCell *)(((CPDFTableCell *)m_pInTextBox)->GetTable()->GetCell(cellPos));
          lTmpCell++;
          break;
        }
      }
    }
    i_lNext--;

    if (!pRetTB)
      return NULL;
  }

  return pRetTB;
}

long CPDFTable::CalculateTBPos(CPDFTextBox *i_pTB)
{
  if (!m_pInTextBox)
    return 0;

  long lDivCount = (long)m_DivHeights.size();
  CPDFTextBox *pTmpTB;

  for (long i = 0; i < lDivCount + 1; i++)
  {
    pTmpTB = GetInTextBoxNext(i);
    if (pTmpTB == i_pTB)
      return m_pInTextBox->GetPageNumber() + i;
  }

  return m_pInTextBox->GetPageNumber();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
CRowInfo::CRowInfo()
{
  m_lRowHeight = 0;
  m_lRowMinHeight = NOT_USED;
  m_lColCount = 0;
  m_bFixed = false_a;
  m_bKeepTogether = false_a;
  m_bHeader = false_a;
}

CRowInfo::~CRowInfo()
{
/*
  long i, lSize = (long)m_Columns.size();
  for (i = 0; i < lSize; i++)
  {
    CPDFCell *pCell = m_Columns[i];
    _ASSERT(pCell);
    if (pCell)
    {
      if (pCell->IsTable())
        delete ((CPDFTable *)pCell);
      else
        delete ((CPDFTableCell *)pCell);
    }
  }
*/
}

void CRowInfo::SetRowHeight(long i_lRowHeight, bool_a i_bMinHeight)
{
  m_lRowHeight = i_lRowHeight;
  if (i_bMinHeight)
    m_lRowMinHeight = i_lRowHeight;
}

CPDFCell *CRowInfo::GetColumn(long i_lCol)
{
  if (i_lCol < 0 || i_lCol >= m_lColCount)
    return NULL;
  CPDFCell *pCell = m_Columns[i_lCol];
  _ASSERT(pCell);
  return pCell;
}

bool_a CRowInfo::InsertColumn(CPDFCell *i_pCell, long i_lCol)
{
  if (!i_pCell)
    return false_a;
  long lSize = (long)m_Columns.size();
  if (i_lCol < 0 || i_lCol > lSize)
    i_lCol = lSize;
  m_Columns.insert(m_Columns.begin() + i_lCol, i_pCell);
  m_lColCount++;
  return true_a;
}

CPDFCell *CRowInfo::RemoveColumn(long i_lCol)
{
  long lSize = (long)m_Columns.size();
  if (i_lCol < 0 || i_lCol >= lSize)
    return NULL;
  CPDFCell *pCell = m_Columns[i_lCol];
  _ASSERT(pCell);
  m_Columns.erase(m_Columns.begin() + i_lCol);
  m_lColCount--;
  return pCell;
}

bool_a CRowInfo::DeleteColumn(long i_lCol)
{
  long lSize = (long)m_Columns.size();
  if (i_lCol < 0 || i_lCol >= lSize)
    return false_a;
  CPDFCell *pCell = m_Columns[i_lCol];
  _ASSERT(pCell);
  m_Columns.erase(m_Columns.begin() + i_lCol);
  if (pCell)
  {
    if (pCell->IsTable())
      delete ((CPDFTable *)pCell);
    else
      delete ((CPDFTableCell *)pCell);
  }
  m_lColCount--;
  return true_a;
}

bool_a CRowInfo::DeleteAllColumn()
{
  long lSize = (long)m_Columns.size();
  for (long lCol = 0; lCol < lSize; lCol++)
  {
    CPDFCell *pCell = m_Columns[lCol];
    _ASSERT(pCell);
    if (pCell)
    {
      if (pCell->IsOverRowCell())
      {
        pCell->DecOverRowCount();
      }
      else
      {
        if (pCell->IsTable())
          delete ((CPDFTable *)pCell);
        else
          delete ((CPDFTableCell *)pCell);
      }
    }
  }
  m_Columns.clear();
  m_lColCount = 0;
  return true_a;
}

