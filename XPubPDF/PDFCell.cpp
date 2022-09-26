
#include "PDFCell.h"
#include "PDFDocument.h"
#include "pdffnc.h"

using namespace std;

CPDFCell::CPDFCell(CPDFTable *i_pTable)
{
  m_pTable = i_pTable;
  m_lOverRowCount = 0;
  m_Cell = cell(-1, -1);
  m_lBorderType = eNoBorder;//eRecBorder;
  SetBorderWidth(eRecBorder | eLeftDiagonalBorder | eRightDiagonalBorder, 0);
//  SetBorderColor(eRecBorder | eLeftDiagonalBorder | eRightDiagonalBorder, 0, 0, 0);
  m_lBorderIndents[LEFT_BORDER] = m_lBorderIndents[RIGHT_BORDER] =
      m_lBorderIndents[TOP_BORDER] = m_lBorderIndents[BOTTOM_BORDER] = 0;
  m_recPosSize.lBottom = 0;
  m_recPosSize.lLeft = 0;
  m_recPosSize.lRight = 0;
  m_recPosSize.lTop = 0;
}

CPDFCell::~CPDFCell()
{
}

void CPDFCell::CopyAttr(CPDFCell *i_pCell)
{
  if (!i_pCell)
    return;
  short i;
  i_pCell->m_pTable = this->m_pTable;
  i_pCell->m_Cell.m_lCol = this->m_Cell.m_lCol;
  i_pCell->m_Cell.m_lRow = this->m_Cell.m_lRow;
  i_pCell->m_lOverRowCount = this->m_lOverRowCount;
  i_pCell->m_lBorderType = this->m_lBorderType;
  for (i = 0; i < LEFT_DIAG_BORDER; i++)
    i_pCell->m_lBorderIndents[i] = this->m_lBorderIndents[i];
  for (i = 0; i < BORDER_COUNT; i++)
  {
    i_pCell->m_lBorderWidth[i] = this->m_lBorderWidth[i];
    i_pCell->m_BorderColor[i] = this->m_BorderColor[i];
  }
  COPY_REC(i_pCell->m_recPosSize, i_pCell->m_recPosSize);
}

void CPDFCell::SetOverRowCount(long i_lCount)
{
  m_lOverRowCount = i_lCount;
}

long CPDFCell::GetOverRowCount()
{
  return m_lOverRowCount;
}

void CPDFCell::IncOverRowCount()
{
  m_lOverRowCount++;
}

void CPDFCell::DecOverRowCount()
{
  if (m_lOverRowCount <= 0)
  {
    m_lOverRowCount = 0;
  }
  else
    m_lOverRowCount--;
}

bool_a CPDFCell::IsOverRowCell()
{
  if (m_lOverRowCount > 0)
    return true_a;
  return false_a;
}

void CPDFCell::SetBorderWidth(long i_lBorder, long i_lWidth)
{
  if (i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder)
  {
    m_lBorderWidth[LEFT_BORDER] = i_lWidth;
  }
  if (i_lBorder & eRightBorder || i_lBorder & eRightDoubleBorder)
  {
    m_lBorderWidth[RIGHT_BORDER] = i_lWidth;
  }
  if (i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder)
  {
    m_lBorderWidth[TOP_BORDER] = i_lWidth;
  }
  if (i_lBorder & eBottomBorder || i_lBorder & eBottomDoubleBorder)
  {
    m_lBorderWidth[BOTTOM_BORDER] = i_lWidth;
  }
  if (i_lBorder & eLeftDiagonalBorder)
  {
    m_lBorderWidth[LEFT_DIAG_BORDER] = i_lWidth;
  }
  if (i_lBorder & eRightDiagonalBorder)
  {
    m_lBorderWidth[RIGHT_DIAG_BORDER] = i_lWidth;
  }
}

bool_a CPDFCell::CopyAttributes(CPDFCell *i_pCell)
{
  if (!i_pCell)
    return false_a;
  return true_a;
}

long CPDFCell::GetBorderWidth(EBORDERTYPE i_eBorder)
{
  switch (i_eBorder)
  {
  case eLeftBorder:
  case eLeftDoubleBorder:
    return m_lBorderWidth[LEFT_BORDER];
  case eRightBorder:
  case eRightDoubleBorder:
    return m_lBorderWidth[RIGHT_BORDER];
  case eTopBorder:
  case eTopDoubleBorder:
    return m_lBorderWidth[TOP_BORDER];
  case eBottomBorder:
  case eBottomDoubleBorder:
    return m_lBorderWidth[BOTTOM_BORDER];
  case eLeftDiagonalBorder:
    return m_lBorderWidth[LEFT_DIAG_BORDER];
  case eRightDiagonalBorder:
    return m_lBorderWidth[RIGHT_DIAG_BORDER];
  case eNoBorder:
  default:
    break;
  }
  return 0;
}

long CPDFCell::GetBorderIndents(EBORDERTYPE i_eBorder)
{
  long lWidthHi = 0, lWidthLo = 0;
  long lBorder = GetBorderType();
  switch (i_eBorder)
  {
  case eLeftBorder:
    if (lBorder & eLeftBorder || lBorder & eLeftDoubleBorder)
      lWidthHi = GetBorderWidth(eLeftBorder);
    GetHalfBorder((lBorder & eLeftBorder | lBorder & eLeftDoubleBorder), lWidthHi, lWidthHi, lWidthLo);
    return m_lBorderIndents[LEFT_BORDER] > lWidthHi ? m_lBorderIndents[LEFT_BORDER] : lWidthHi;
  case eRightBorder:
    if (lBorder & eRightBorder || lBorder & eRightDoubleBorder)
      lWidthHi = GetBorderWidth(eRightBorder);
    GetHalfBorder((lBorder & eRightBorder | lBorder & eRightDoubleBorder), lWidthHi, lWidthHi, lWidthLo);
    return m_lBorderIndents[RIGHT_BORDER] > lWidthHi ? m_lBorderIndents[RIGHT_BORDER] : lWidthHi;
  case eTopBorder:
    if (lBorder & eTopBorder || lBorder & eTopDoubleBorder)
      lWidthHi = GetBorderWidth(eTopBorder);
    GetHalfBorder((lBorder & eTopBorder | lBorder & eTopDoubleBorder), lWidthHi, lWidthHi, lWidthLo);
    return m_lBorderIndents[TOP_BORDER] > lWidthHi ? m_lBorderIndents[TOP_BORDER] : lWidthHi;
  case eBottomBorder:
    if (lBorder & eBottomBorder || lBorder & eBottomDoubleBorder)
      lWidthHi = GetBorderWidth(eBottomBorder);
    GetHalfBorder((lBorder & eBottomBorder | lBorder & eBottomDoubleBorder), lWidthHi, lWidthHi, lWidthLo);
    return m_lBorderIndents[BOTTOM_BORDER] > lWidthHi ? m_lBorderIndents[BOTTOM_BORDER] : lWidthHi;
  case eLeftDoubleBorder:
  case eRightDoubleBorder:
  case eTopDoubleBorder:
  case eBottomDoubleBorder:
  case eLeftDiagonalBorder:
  case eRightDiagonalBorder:
  case eNoBorder:
  default:
    break;
  }
  return 0;
}

void CPDFCell::SetBorderType(long i_lBorder)
{
  m_lBorderType = i_lBorder;
}

long CPDFCell::GetBorderType()
{
  return m_lBorderType;
}

void CPDFCell::SetBorderColor(long i_lBorder, CPDFColor& i_rColor)
{
  if (i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder)
  {
    m_BorderColor[LEFT_BORDER] = i_rColor;
  }
  if (i_lBorder & eRightBorder || i_lBorder & eRightDoubleBorder)
  {
    m_BorderColor[RIGHT_BORDER] = i_rColor;
  }
  if (i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder)
  {
    m_BorderColor[TOP_BORDER] = i_rColor;
  }
  if (i_lBorder & eBottomBorder || i_lBorder & eBottomDoubleBorder)
  {
    m_BorderColor[BOTTOM_BORDER] = i_rColor;
  }
  if (i_lBorder & eLeftDiagonalBorder)
  {
    m_BorderColor[LEFT_DIAG_BORDER] = i_rColor;
  }
  if (i_lBorder & eRightDiagonalBorder)
  {
    m_BorderColor[RIGHT_DIAG_BORDER] = i_rColor;
  }
}

void CPDFCell::SetBorderColor(long i_lBorder, float i_fR, float i_fG, float i_fB)
{
  if (i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder)
  {
    m_BorderColor[LEFT_BORDER].SetColor(i_fR, i_fG, i_fB);
  }
  if (i_lBorder & eRightBorder || i_lBorder & eRightDoubleBorder)
  {
    m_BorderColor[RIGHT_BORDER].SetColor(i_fR, i_fG, i_fB);
  }
  if (i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder)
  {
    m_BorderColor[TOP_BORDER].SetColor(i_fR, i_fG, i_fB);
  }
  if (i_lBorder & eBottomBorder || i_lBorder & eBottomDoubleBorder)
  {
    m_BorderColor[BOTTOM_BORDER].SetColor(i_fR, i_fG, i_fB);
  }
  if (i_lBorder & eLeftDiagonalBorder)
  {
    m_BorderColor[LEFT_DIAG_BORDER].SetColor(i_fR, i_fG, i_fB);
  }
  if (i_lBorder & eRightDiagonalBorder)
  {
    m_BorderColor[RIGHT_DIAG_BORDER].SetColor(i_fR, i_fG, i_fB);
  }
}

CPDFColor *CPDFCell::GetBorderColor(EBORDERTYPE i_eBorder)
{
  switch (i_eBorder)
  {
  case eLeftBorder:
  case eLeftDoubleBorder:
    return &m_BorderColor[LEFT_BORDER];
  case eRightBorder:
  case eRightDoubleBorder:
    return &m_BorderColor[RIGHT_BORDER];
  case eTopBorder:
  case eTopDoubleBorder:
    return &m_BorderColor[TOP_BORDER];
  case eBottomBorder:
  case eBottomDoubleBorder:
    return &m_BorderColor[BOTTOM_BORDER];
  case eLeftDiagonalBorder:
    return &m_BorderColor[LEFT_DIAG_BORDER];
  case eRightDiagonalBorder:
    return &m_BorderColor[RIGHT_DIAG_BORDER];
  case eNoBorder:
  default:
    break;
  }
  return NULL;
}

cell CPDFCell::GetCellPos()
{
  return m_Cell;
}

void CPDFCell::SetCellPos(cell i_Cell)
{
  m_Cell = i_Cell;
}

void CPDFCell::SetCellPos(long i_lRow, long i_lCol)
{
  m_Cell.m_lCol = i_lCol;
  m_Cell.m_lRow = i_lRow;
}

rec_a CPDFCell::GetPosSize()
{
  return m_recPosSize;
}

long CPDFCell::GetWidth()
{
  return (m_recPosSize.lRight - m_recPosSize.lLeft);
}

bool_a CPDFCell::SetWidth(long i_lWidth)
{
  m_recPosSize.lRight += i_lWidth - (m_recPosSize.lRight - m_recPosSize.lLeft);
  return true_a;
}

bool_a CPDFCell::SetHeight(long i_lHeight)
{
  m_recPosSize.lBottom += (m_recPosSize.lTop - m_recPosSize.lBottom) - i_lHeight;
  return true_a;
}

bool_a CPDFCell::SetDrawHeight(long i_lHeight)
{
  m_recPosSize.lBottom += (m_recPosSize.lTop - m_recPosSize.lBottom) - i_lHeight;
  return true_a;
}

bool_a CPDFCell::MoveTo(long i_lX, long i_lY)
{
  long lX, lY;

  lX = m_recPosSize.lLeft - i_lX;
  lY = m_recPosSize.lTop - i_lY;
  m_recPosSize.lLeft -= lX;
  m_recPosSize.lRight -= lX;
  m_recPosSize.lTop -= lY;
  m_recPosSize.lBottom -= lY;
  return true_a;
}

void CPDFCell::GetHalfBorder(long i_lBorder, long i_lWidth, long &o_lHi, long &o_lLow)
{
  if (i_lWidth > 0)
  {
    o_lHi = o_lLow = (i_lWidth / 2);
//    o_lHi++;
    if (i_lWidth % 2)
      o_lLow++;

    if (i_lBorder & eLeftDoubleBorder || i_lBorder & eRightDoubleBorder
        || i_lBorder & eTopDoubleBorder || i_lBorder & eBottomDoubleBorder)
      o_lHi = o_lLow = i_lWidth + 1;
  }
  else
  {
    o_lHi = 0;
    o_lLow = 0;
  }
}

float CPDFCell::GetHalfBorderForDraw(long i_lBorder, long i_lWidth)
{
  float fRet = 1.f;
//  if (i_lWidth > 1)
  {
    fRet = (float)i_lWidth / 2;
    fRet -= .5f;
  }
  if (!(i_lWidth % 2))
    fRet -= .5f;
  return fRet;
}

void CPDFCell::ChangeLeftIndents(long i_lWidth)
{
  m_lBorderIndents[LEFT_BORDER] = i_lWidth;
}

void CPDFCell::ChangeRightIndents(long i_lWidth)
{
  m_lBorderIndents[RIGHT_BORDER] = i_lWidth;
}

void CPDFCell::ChangeTopIndents(long i_lWidth)
{
  m_lBorderIndents[TOP_BORDER] = i_lWidth;
}

void CPDFCell::ChangeBottomIndents(long i_lWidth)
{
  m_lBorderIndents[BOTTOM_BORDER] = i_lWidth;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
CPDFTableCell::CPDFTableCell(CPDFDocument *i_pDoc, CPDFTable *i_pTable, rec_a i_recPosSize, long i_lRow,
                   long i_lCol, long i_lPage)
  :CPDFTextBox(i_pDoc, i_recPosSize, i_lPage), CPDFCell(i_pTable)
{
  m_eTBType = eTBTableCell;
  SetBorderWidth(eRecBorder | eLeftDiagonalBorder | eRightDiagonalBorder, 1);
  SetBorderColor(eRecBorder | eLeftDiagonalBorder | eRightDiagonalBorder, 0, 0, 0);
  SetMargins(m_pDoc->GetLeftMargin(), m_pDoc->GetTopMargin(), m_pDoc->GetRightMargin(), m_pDoc->GetBottomMargin());
  SetCellPos(i_lRow, i_lCol);

  m_bChangeSize = false_a;
}

CPDFTableCell::~CPDFTableCell()
{
}

void CPDFTableCell::SetBorderType(long i_lBorder)
{
  long lLeftW = 0, lRightW = 0, lTopW = 0, lBottomW = 0;
  long lLeftW1 = 0, lRightW1 = 0, lTopW1 = 0, lBottomW1 = 0;

  if (i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder)
  {
    lLeftW = GetBorderWidth(eLeftBorder);
    GetHalfBorder((i_lBorder & eLeftBorder | i_lBorder & eLeftDoubleBorder), lLeftW, lLeftW, lLeftW1);
  }

  if (i_lBorder & eRightBorder || i_lBorder & eRightDoubleBorder)
  {
    lRightW = GetBorderWidth(eRightBorder);
    GetHalfBorder((i_lBorder & eRightBorder | i_lBorder & eRightDoubleBorder), lRightW, lRightW, lRightW1);
  }

  if (i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder)
  {
    lTopW = GetBorderWidth(eTopBorder);
    GetHalfBorder((i_lBorder & eTopBorder | i_lBorder & eTopDoubleBorder), lTopW, lTopW, lTopW1);
  }

  if (i_lBorder & eBottomBorder || i_lBorder & eBottomDoubleBorder)
  {
    lBottomW = GetBorderWidth(eBottomBorder);
    GetHalfBorder((i_lBorder & eBottomBorder | i_lBorder & eBottomDoubleBorder), lBottomW, lBottomW, lBottomW1);
  }

  CPDFCell::SetBorderType(i_lBorder);
  SetBorder(lLeftW, lTopW, lRightW, lBottomW);

  if (m_pTable)
    m_pTable->ChangeCellsIndents(GetCellPos(), i_lBorder, lLeftW1, lTopW1, lRightW1, lBottomW1);
}

void CPDFTableCell::SetBorderWidth(long i_lBorder, long i_lWidth)
{
  if (i_lWidth < 1)
    i_lWidth = 1;
  long lLeftW = 0, lRightW = 0, lTopW = 0, lBottomW = 0;
  long lLeftW1 = 0, lRightW1 = 0, lTopW1 = 0, lBottomW1 = 0;
  bool_a bChanged = false_a;
  long lBorder = GetBorderType();

  if (lBorder & eLeftBorder || lBorder & eLeftDoubleBorder)
  {
    lLeftW = GetBorderWidth(eLeftBorder);
    if (lLeftW != i_lWidth && (i_lBorder & eLeftBorder || i_lBorder & eLeftDoubleBorder))
    {
      bChanged = true_a;
      lLeftW = i_lWidth;
    }
    GetHalfBorder((lBorder & eLeftBorder | lBorder & eLeftDoubleBorder), lLeftW, lLeftW, lLeftW1);
  }

  if (lBorder & eRightBorder || lBorder & eRightDoubleBorder)
  {
    lRightW = GetBorderWidth(eRightBorder);
    if (lRightW != i_lWidth && (i_lBorder & eRightBorder || i_lBorder & eRightDoubleBorder))
    {
      bChanged = true_a;
      lRightW = i_lWidth;
    }  
    GetHalfBorder((lBorder & eRightBorder | lBorder & eRightDoubleBorder), lRightW, lRightW, lRightW1);
  }

  if (lBorder & eTopBorder || lBorder & eTopDoubleBorder)
  {
    lTopW = GetBorderWidth(eTopBorder);
    if (lTopW != i_lWidth && (i_lBorder & eTopBorder || i_lBorder & eTopDoubleBorder))
    {
      bChanged = true_a;
      lTopW = i_lWidth;
    }
    GetHalfBorder((lBorder & eTopBorder | lBorder & eTopDoubleBorder), lTopW, lTopW, lTopW1);
  }

  if (lBorder & eBottomBorder || lBorder & eBottomDoubleBorder)
  {
    lBottomW = GetBorderWidth(eBottomBorder);
    if (lBottomW != i_lWidth && (i_lBorder & eBottomBorder || i_lBorder & eBottomDoubleBorder))
    {
      bChanged = true_a;
      lBottomW = i_lWidth;
    }
    GetHalfBorder((lBorder & eBottomBorder | lBorder & eBottomDoubleBorder), lBottomW, lBottomW, lBottomW1);
  }

  CPDFCell::SetBorderWidth(i_lBorder, i_lWidth);
  SetBorder(lLeftW, lTopW, lRightW, lBottomW);

  if (m_pTable)
    m_pTable->ChangeCellsIndents(GetCellPos(), lBorder, lLeftW1, lTopW1, lRightW1, lBottomW1);
}

bool_a CPDFTableCell::CopyAttributes(CPDFCell *i_pCell)
{
  if (!i_pCell)
    return false_a;
  //float fR, fG, fB;
  CPDFColor *pColor;
  SetBorderWidth(CPDFCell::eLeftBorder, i_pCell->GetBorderWidth(CPDFCell::eLeftBorder));
  SetBorderWidth(CPDFCell::eRightBorder, i_pCell->GetBorderWidth(CPDFCell::eRightBorder));
  SetBorderWidth(CPDFCell::eTopBorder, i_pCell->GetBorderWidth(CPDFCell::eTopBorder));
  SetBorderWidth(CPDFCell::eBottomBorder, i_pCell->GetBorderWidth(CPDFCell::eBottomBorder));
  SetBorderWidth(CPDFCell::eLeftDiagonalBorder, i_pCell->GetBorderWidth(CPDFCell::eLeftDiagonalBorder));
  SetBorderWidth(CPDFCell::eRightDiagonalBorder, i_pCell->GetBorderWidth(CPDFCell::eRightDiagonalBorder));
  SetBorderType(i_pCell->GetBorderType());

  pColor = i_pCell->GetBorderColor(CPDFCell::eLeftBorder);
  if (!pColor)
    return false_a;
  SetBorderColor(CPDFCell::eLeftBorder, *pColor);

  pColor = i_pCell->GetBorderColor(CPDFCell::eRightBorder);
  if (!pColor)
    return false_a;
  SetBorderColor(CPDFCell::eRightBorder, *pColor);

  pColor = i_pCell->GetBorderColor(CPDFCell::eTopBorder);
  if (!pColor)
    return false_a;
  SetBorderColor(CPDFCell::eTopBorder, *pColor);

  pColor = i_pCell->GetBorderColor(CPDFCell::eBottomBorder);
  if (!pColor)
    return false_a;
  SetBorderColor(CPDFCell::eBottomBorder, *pColor);

  pColor = i_pCell->GetBorderColor(CPDFCell::eLeftDiagonalBorder);
  if (!pColor)
    return false_a;
  SetBorderColor(CPDFCell::eLeftDiagonalBorder, *pColor);

  pColor = i_pCell->GetBorderColor(CPDFCell::eRightDiagonalBorder);
  if (!pColor)
    return false_a;
  SetBorderColor(CPDFCell::eRightDiagonalBorder, *pColor);

  if (!i_pCell->IsTable() && !IsTable())
  {
    CPDFTextBox::CopyAttributtes((CPDFTableCell *)i_pCell);
  }

  return true_a;
}

bool_a CPDFTableCell::FlushBorder()
{
  bool_a bRet = true_a;
  long lType, lWidth;
  //float fR, fG, fB;
  rec_a recPosSize;
  float fDif = 0, fLeft = 0, fRight = 0, fTop = 0, fBottom = 0;
  cell Cell = GetCellPos();
  CPDFColor *pColor;

  lType = GetBorderType();
  recPosSize = GetPosSize();
  
  m_pDoc->SetCurrentPage(GetPageNumber());

  if (lType & eLeftBorder)
    fLeft = GetHalfBorderForDraw(eLeftBorder, GetBorderWidth(eLeftBorder));
  if (lType & eRightBorder)
    fRight = GetHalfBorderForDraw(eRightBorder, GetBorderWidth(eRightBorder));
  if (lType & eTopBorder)
    fTop = GetHalfBorderForDraw(eTopBorder, GetBorderWidth(eTopBorder));
  if (lType & eBottomBorder)
    fBottom = GetHalfBorderForDraw(eBottomBorder, GetBorderWidth(eBottomBorder));
  if (lType & eLeftDoubleBorder)
    fLeft = (float)GetBorderWidth(eLeftBorder);
  if (lType & eRightDoubleBorder)
    fRight = (float)GetBorderWidth(eRightBorder);
  if (lType & eTopDoubleBorder)
    fTop = (float)GetBorderWidth(eTopBorder);
  if (lType & eBottomDoubleBorder)
    fBottom = (float)GetBorderWidth(eBottomBorder);

  if (lType & eLeftBorder && !(lType & eLeftDoubleBorder))
  {
    lWidth = GetBorderWidth(eLeftBorder);
    pColor = GetBorderColor(eLeftBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    if (lWidth > 1)
      fDif = 0.5f;
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft + fDif, (float)recPosSize.lTop + fTop,
                              (float)recPosSize.lLeft + fDif, (float)recPosSize.lBottom - fBottom);
    if (!bRet)
      return bRet;
  }
#ifdef __DRAW_BORDER__
  else
  {
    m_pDoc->SetLineColor(0.f, 0.f, 0.f);
    m_pDoc->SetLineWidth(1);
    m_pDoc->DrawDashedLine((float)recPosSize.lLeft + fDif, (float)recPosSize.lTop + fTop,
                     (float)recPosSize.lLeft + fDif, (float)recPosSize.lBottom - fBottom);
  }
#endif

  if (lType & eRightBorder && !(lType & eRightDoubleBorder))
  {
    lWidth = GetBorderWidth(eRightBorder);
    pColor = GetBorderColor(eRightBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine((float)recPosSize.lRight, (float)recPosSize.lTop + fTop,
                              (float)recPosSize.lRight, (float)recPosSize.lBottom - fBottom);
    if (!bRet)
      return bRet;
  }
#ifdef __DRAW_BORDER__
  else
  {
    m_pDoc->SetLineColor(0.f, 0.f, 0.f);
    m_pDoc->SetLineWidth(1);
    m_pDoc->DrawDashedLine((float)recPosSize.lRight, (float)recPosSize.lTop + fTop,
                     (float)recPosSize.lRight, (float)recPosSize.lBottom - fBottom);
  }
#endif

  if (lType & eTopBorder && !(lType & eTopDoubleBorder))
  {
    lWidth = GetBorderWidth(eTopBorder);
    pColor = GetBorderColor(eTopBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    if (lWidth > 1)
      fDif = 0.5f;
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lTop - fDif,
                              (float)recPosSize.lRight + fRight, (float)recPosSize.lTop - fDif);
    if (!bRet)
      return bRet;
  }
#ifdef __DRAW_BORDER__
  else
  {
    m_pDoc->SetLineColor(0.f, 0.f, 0.f);
    m_pDoc->SetLineWidth(1);
    m_pDoc->DrawDashedLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lTop - fDif,
                     (float)recPosSize.lRight + fRight, (float)recPosSize.lTop - fDif);
  }
#endif

  if (lType & eBottomBorder && !(lType & eBottomDoubleBorder))
  {
    lWidth = GetBorderWidth(eBottomBorder);
    pColor = GetBorderColor(eBottomBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lBottom,
                            (float)recPosSize.lRight + fRight, (float)recPosSize.lBottom);
    if (!bRet)
      return bRet;
  }
#ifdef __DRAW_BORDER__
  else
  {
    m_pDoc->SetLineColor(0.f, 0.f, 0.f);
    m_pDoc->SetLineWidth(1);
    m_pDoc->DrawDashedLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lBottom,
                     (float)recPosSize.lRight + fRight, (float)recPosSize.lBottom);
  }
#endif

  if (lType & eLeftDiagonalBorder)
  {
    lWidth = GetBorderWidth(eLeftDiagonalBorder);
    pColor = GetBorderColor(eLeftDiagonalBorder);
    if (!pColor)
      return false_a;
    bRet = m_pDoc->SetLineColor(*pColor);
    bRet = m_pDoc->SetLineWidth(lWidth);
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft, (float)recPosSize.lBottom,
                            (float)recPosSize.lRight, (float)recPosSize.lTop);
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
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft, (float)recPosSize.lTop,
                              (float)recPosSize.lRight, (float)recPosSize.lBottom);
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
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft + ((float)lWidth / 2) + 1, (float)recPosSize.lTop + fTop,
                              (float)recPosSize.lLeft + ((float)lWidth / 2) + 1, (float)recPosSize.lBottom - fBottom);
    if (!bRet)
      return bRet;
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft - ((float)lWidth / 2), (float)recPosSize.lTop + fTop,
                              (float)recPosSize.lLeft - ((float)lWidth / 2), (float)recPosSize.lBottom - fBottom);
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
    bRet = m_pDoc->DrawLine((float)recPosSize.lRight - ((float)lWidth / 2), (float)recPosSize.lTop + fTop,
                              (float)recPosSize.lRight - ((float)lWidth / 2), (float)recPosSize.lBottom - fBottom);
    if (!bRet)
      return bRet;
    bRet = m_pDoc->DrawLine((float)recPosSize.lRight + ((float)lWidth / 2) + 1, (float)recPosSize.lTop + fTop,
                              (float)recPosSize.lRight + ((float)lWidth / 2) + 1, (float)recPosSize.lBottom - fBottom);
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
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lTop - ((float)lWidth / 2) - 1,
                              (float)recPosSize.lRight + fRight, (float)recPosSize.lTop - ((float)lWidth / 2) - 1);
    if (!bRet)
      return bRet;
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lTop + ((float)lWidth / 2),
                              (float)recPosSize.lRight + fRight, (float)recPosSize.lTop + ((float)lWidth / 2));
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

    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lBottom + ((float)lWidth / 2),
                            (float)recPosSize.lRight + fRight, (float)recPosSize.lBottom + ((float)lWidth / 2));
    if (!bRet)
      return bRet;
    bRet = m_pDoc->DrawLine((float)recPosSize.lLeft - fLeft, (float)recPosSize.lBottom - ((float)lWidth / 2) - 1,
                            (float)recPosSize.lRight + fRight, (float)recPosSize.lBottom - ((float)lWidth / 2) - 1);
    if (!bRet)
      return bRet;
  }
  return bRet;
}

bool_a CPDFTableCell::Flush()
{
  bool_a bRet = false_a;

  //rec_a rec;
  //rec.lLeft = this->GetLeftDrawPos() - this->GetLeftMargin();
  //rec.lRight = this->GetRightDrawPos() + this->GetRightMargin();
  //rec.lTop = this->GetTopDrawPos() + this->GetTopMargin();
  //rec.lBottom = this->GetBottomDrawPos() - this->GetBottomMargin();

  //PDFSaveGraphicsState(GetBaseDoc());
  //PDFSetClippingRect(GetBaseDoc(), rec);

  bRet = CPDFTextBox::Flush();

  //PDFRestoreGraphicsState(GetBaseDoc());

  if (!bRet)
    return bRet;
  return bRet;
}

void CPDFTableCell::CopyAttr(CPDFTableCell *i_pTCell)
{
  CPDFCell::CopyAttr((CPDFCell *)i_pTCell);
  CPDFTextBox::CopyAttr((CPDFTextBox *)i_pTCell);
}

CPDFTableCell *CPDFTableCell::Clone()
{
  CPDFTableCell *pRetCell = NULL;
  pRetCell = new CPDFTableCell(m_pDoc, m_pTable, GetPosSize());
  if (NULL == pRetCell)
     return pRetCell;

  PDFFreeParagraph(&pRetCell->pParagraph);
  //memcpy(pRetCell, this, sizeof(CPDFTableCell));
  CopyAttr(pRetCell);

  pRetCell->pTBImages = NULL;
  pRetCell->pTBSpaces = NULL;
  pRetCell->pParagraph = NULL;
  pRetCell->pActiveParagraph = NULL;
  pRetCell->pTBTabStops = NULL;
  pRetCell->sTabsCount = 0;
  pRetCell->SetTabs(this->pTBTabStops, this->sTabsCount);
  pRetCell->SetPrevTextBox(NULL);
  pRetCell->SetNextTextBox(NULL);
  pRetCell->SetImages(this->pTBImages);
  pRetCell->SetSpaces(this->pTBSpaces);
  pRetCell->SetParagraph(this->pParagraph);
  return pRetCell;
}

rec_a CPDFTableCell::GetPosSize()
{
  return CPDFTextBox::GetPosSize();
}

long CPDFTableCell::GetWidth()
{
  return CPDFTextBox::GetWidth();
}

bool_a CPDFTableCell::SetWidth(long i_lWidth)
{
  CPDFCell::SetWidth(i_lWidth);
  return CPDFTextBox::SetWidth(i_lWidth);
}

bool_a CPDFTableCell::SetHeight(long i_lHeight)
{
  CPDFCell::SetHeight(i_lHeight);
  return CPDFTextBox::SetHeight(i_lHeight);
}

bool_a CPDFTableCell::SetDrawHeight(long i_lHeight)
{
  CPDFCell::SetDrawHeight(i_lHeight);
  return CPDFTextBox::SetDrawHeight(i_lHeight);
}

bool_a CPDFTableCell::MoveTo(long i_lX, long i_lY)
{
  CPDFCell::MoveTo(i_lX, i_lY);
  return CPDFTextBox::MoveTo(i_lX, i_lY);
}

void CPDFTableCell::SetDocument(CPDFDocument *i_pDoc)
{
  return CPDFTextBox::SetDocument(i_pDoc);
}

void CPDFTableCell::SetPage(long i_lPage)
{
  return CPDFTextBox::SetPage(i_lPage);
}

bool_a CPDFTableCell::ChangeFontID(map <long, long> *i_pFontMap)
{
  return CPDFTextBox::ChangeFontID(i_pFontMap);
}

bool_a CPDFTableCell::ChangeImageID(map <long, long> *i_pImageMap)
{
  return CPDFTextBox::ChangeImageID(i_pImageMap);
}

bool_a CPDFTableCell::IsFlush()
{
  return CPDFTextBox::IsFlush();
}

bool_a CPDFTableCell::CheckResize()
{
  long lWidth, lHeight, lDif;
  bool_a bRet = true_a;
  if (CPDFTextBox::GetTextDirection() != eHorizontal || m_bChangeSize)
    return true_a;
  cell Cell = GetCellPos();
  if (!bRet)
    return bRet;
  bRet = GetSize(lWidth, lHeight);
  if (!bRet)
    return bRet;
  lDif = GetHeight() - GetDrawHeight();
  if (lHeight != GetDrawHeight())
  {
    if (m_pTable)
      bRet = m_pTable->SetRowHeight(Cell.m_lRow, lHeight + lDif);
    else
      SetDrawHeight(lHeight);
  }
  return bRet;
}

bool_a CPDFTableCell::FontSizeChanged()
{
  return CPDFTextBox::FontSizeChanged();
}

bool_a CPDFTableCell::CanBeHeightChanged(long &i_lDif)
{
  if (i_lDif >= 0)
    return true_a;
  long lHeight, lWidth, lDif;
  if (!GetSize(lWidth, lHeight))
  {
    i_lDif = 0;
    return false_a;
  }
  lDif = GetDrawHeight() - lHeight;
  if ((i_lDif + lDif) >= 0)
    return true_a;
  i_lDif = -1 * lDif;
  return false_a;
}

void CPDFTableCell::ChangeLeftIndents(long i_lWidth)
{
  long lLeft, lTop, lRight, lBottom;
  GetBorder(lLeft, lTop, lRight, lBottom);
  CPDFCell::ChangeLeftIndents(i_lWidth);
  SetBorder(i_lWidth, lTop, lRight, lBottom);
}

void CPDFTableCell::ChangeRightIndents(long i_lWidth)
{
  long lLeft, lTop, lRight, lBottom;
  GetBorder(lLeft, lTop, lRight, lBottom);
  CPDFCell::ChangeRightIndents(i_lWidth);
  SetBorder(lLeft, lTop, i_lWidth, lBottom);
}

void CPDFTableCell::ChangeTopIndents(long i_lWidth)
{
  long lLeft, lTop, lRight, lBottom;
  GetBorder(lLeft, lTop, lRight, lBottom);
  CPDFCell::ChangeTopIndents(i_lWidth);
  SetBorder(lLeft, i_lWidth, lRight, lBottom);
}

void CPDFTableCell::ChangeBottomIndents(long i_lWidth)
{
  long lLeft, lTop, lRight, lBottom;
  GetBorder(lLeft, lTop, lRight, lBottom);
  CPDFCell::ChangeBottomIndents(i_lWidth);
  SetBorder(lLeft, lTop, lRight, i_lWidth);
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


void cell::operator=(const cell& i_Cell)
{
  m_lCol = i_Cell.m_lCol;
  m_lRow = i_Cell.m_lRow;
}

bool cell::operator<(const cell& i_Cell) const
{
  if (m_lRow != i_Cell.m_lRow)
    return m_lRow < i_Cell.m_lRow;
  if (m_lCol != i_Cell.m_lCol)
    return m_lCol < i_Cell.m_lCol;
  return false;
}

bool cell::operator==(const cell& i_Cell) const
{
  if ((m_lCol != i_Cell.m_lCol) || (m_lRow != i_Cell.m_lRow))
    return false;
  return true;
}

bool cell::operator!=(const cell& i_Cell) const
{
  if ((m_lCol == i_Cell.m_lCol) && (m_lRow == i_Cell.m_lRow))
    return false;
  return true;
}

bool cell::operator()(const cell& i_Cell1, const cell& i_Cell2) const
{
  return i_Cell1 < i_Cell2;
}
