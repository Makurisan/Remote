
#include "PDFFrame.h"
#include "PDFDocument.h"
#include "pdffnc.h"

#include <vector>

using namespace std;

CPDFFrame::CPDFFrame(CPDFDocument *i_pDoc, rec_a i_recPosSize, const char *i_pID,
                     long i_lPage, eYPosType i_eType)
  :CPDFTextBox(i_pDoc, i_recPosSize, i_lPage), m_ePosType(i_eType), m_lYPos(i_recPosSize.lTop)
{
  m_eTBType = eTBFrame;
  m_eResizeType = eNoResize;
  m_eOverlapType = eNoOverlap;
  m_bCanResize = false_a;
  m_pTextCol = NULL;
  m_lColCount = NOT_USED;
  m_lColWidth = NOT_USED;
  m_lColGutter = NOT_USED;
  if (i_pID && strlen(i_pID) > 0)
    m_pFrameID = PDFStrDup(i_pID);
  else
    m_pFrameID = NULL;
  SetBorder(0, 0, 0, 0);
  SetMargins(ADDITION_BORDER, ADDITION_BORDER, ADDITION_BORDER, ADDITION_BORDER);
  m_eResizeType = eFullResize;
  m_lRotate = 0;
}

CPDFFrame::~CPDFFrame()
{
  if (m_pFrameID)
    free(m_pFrameID);
  CPDFTextBox *pTB = m_pTextCol;
  while (pTB)
  {
    if (pTB->GetNextTextBox())
    {
      pTB = pTB->GetNextTextBox();
      delete pTB->GetPrevTextBox();
    }
    else
    {
      delete pTB;
      pTB = NULL;
    }
  }
}

char *CPDFFrame::GetFrameID()
{
  return m_pFrameID;
}

void CPDFFrame::SetFrameID(char *i_pID)
{
  if (m_pFrameID)
    free(m_pFrameID);
  if (i_pID)
    m_pFrameID = PDFStrDup(i_pID);
}

bool_a CPDFFrame::SetBorder(long i_lLeft, long i_lTop, long i_lRight, long i_lBottom)
{
  long lL, lT, lB, lR;
  if (!GetBorder(lL, lT, lB, lR))
    return false_a;
  lL = i_lLeft < 0 ? lL : i_lLeft;
  lT = i_lTop < 0 ? lT : i_lTop;
  lR = i_lRight < 0 ? lR : i_lRight;
  lB = i_lBottom < 0 ? lB : i_lBottom;
  return CPDFTextBox::SetBorder(lL, lT, lR, lB);
}

bool_a CPDFFrame::GetBorder(float &o_fLeft, float &o_fTop, float &o_fRight, float &o_fBottom)
{
  bool_a bRet = false_a;
  bRet = CPDFTextBox::GetBorder(o_fLeft, o_fTop, o_fRight, o_fBottom);
  return bRet;
}

bool_a CPDFFrame::GetBorder(long &o_lLeft, long &o_lTop, long &o_lRight, long &o_lBottom)
{
  bool_a bRet = false_a;
  bRet = CPDFTextBox::GetBorder(o_lLeft, o_lTop, o_lRight, o_lBottom);
  return bRet;
}

bool_a CPDFFrame::SetBorderColor(float i_fRed, float i_fGreen, float i_fBlue)
{
  m_BorderColor.SetColor(i_fRed < 0 ? m_BorderColor.GetR() : i_fRed,
                      i_fGreen < 0 ? m_BorderColor.GetG() : i_fGreen,
                      i_fBlue < 0 ? m_BorderColor.GetB() : i_fBlue);
  return true_a;
}

CPDFColor& CPDFFrame::GetBorderColor()
{
  return m_BorderColor;
}

bool_a CPDFFrame::MoveToYPos()
{
  m_bCanResize = true_a;

  if (!MakeColumns())
    return false_a;

  if (!CheckResize())
    return false_a;

  if (!m_pDoc)
    return false_a;
  CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber());
  if (!pPage)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  long lY;
  rec_a recPosSize = GetPosSize();
  switch (m_ePosType)
  {
  case ePosPage:
    {
      lY = pPage->GetHeight() - m_lYPos;
      break;
    }
  case ePosMarg:
    {
      lY = pPage->GetHeight() - pSect->GetTopMargin() - m_lYPos;
      break;
    }
  case ePosPara:
      lY = recPosSize.lTop - m_lYPos;
    break;
  default:
    return false_a;
  }


  CPDFTextBox *pTB = m_pTextCol;
  while (pTB)
  {
    recPosSize = pTB->GetPosSize();
    if (!pTB->MoveTo(recPosSize.lLeft, lY))
      return false_a;
    pTB = pTB->GetNextTextBox();
  }
  recPosSize = GetPosSize();
  if (!CPDFTextBox::MoveTo(recPosSize.lLeft, lY))
    return false_a;
  return true_a;
}

bool_a CPDFFrame::Flush()
{
  if (!MoveToYPos())
    return false_a;
  if (IsFlush())
    return false_a;

  if ((m_lRotate % 360) != 0)
  {
    PDFSetCurrentPage(GetBaseDoc(), this->lPage + 1);
    PDFSaveGraphicsState(GetBaseDoc());
    PDFTranslateL(GetBaseDoc(), this->recTB.lLeft, this->recTB.lBottom);
    PDFrotate(GetBaseDoc(), m_lRotate);
    PDFTranslateL(GetBaseDoc(), -this->recTB.lLeft, -this->recTB.lBottom);
  }

  if (!CPDFTextBox::Flush())
    return false_a;
  CPDFTextBox *pTB = m_pTextCol;
  while (pTB)
  {
    if (!pTB->Flush())
      return false_a;
    pTB = pTB->GetNextTextBox();
  }
  if (!FlushBorder())
    return false_a;

  if ((m_lRotate % 360) != 0)
  {
    PDFSetCurrentPage(GetBaseDoc(), this->lPage + 1);
    PDFRestoreGraphicsState(GetBaseDoc());
  }

  return true_a;
}

bool_a CPDFFrame::FlushBorder()
{
  if (!m_pDoc)
    return false_a;
  float fL, fT, fB, fR, fL1, fT1, fB1, fR1;
  rec_a rec = GetPosSize();
  if (!GetBorder(fL, fT, fB, fR))
    return false_a;
  fL1 = (float)rec.lLeft + (fL / 2.f);
  fT1 = (float)rec.lTop - (fT / 2.f);
  fB1 = (float)rec.lBottom + (fB / 2.f);
  fR1 = (float)rec.lRight - (fR / 2.f);
  m_pDoc->SetLineColor(m_BorderColor);
  if (fT > 0)
  {
    m_pDoc->SetLineWidth(fT);
    m_pDoc->DrawLine((float)rec.lLeft, fT1, (float)rec.lRight, fT1);
  }
  if (fL > 0)
  {
    m_pDoc->SetLineWidth(fL);
    m_pDoc->DrawLine(fL1, (float)rec.lTop, fL1, (float)rec.lBottom);
  }
  if (fR > 0)
  {
    m_pDoc->SetLineWidth(fR);
    m_pDoc->DrawLine(fR1, (float)rec.lTop, fR1, (float)rec.lBottom);
  }
  if (fB > 0)
  {
    m_pDoc->SetLineWidth(fB);
    m_pDoc->DrawLine((float)rec.lLeft, fB1, (float)rec.lRight, fB1);
  }
  return true_a;
}

bool_a CPDFFrame::MoveTo(long i_lX, long i_lY)
{
  return CPDFTextBox::MoveTo(i_lX, i_lY);
}

bool_a CPDFFrame::InsertBreak()
{
  return InsertColumnBreak();
}

bool_a CPDFFrame::CheckResize()
{
  if (!m_bCanResize)
    return true_a;
  if (CPDFTextBox::GetTextDirection() != eHorizontal)
    return true_a;

  if (eNoResize == m_eResizeType ||
      (eGrowResize == m_eResizeType && this->GetNextTextBox()))
    return CPDFTextBox::CheckResize();

  long lWidth, lHeight;
  bool_a bRet = true_a;

  bRet = GetSize(lWidth, lHeight);
  if (!bRet)
    return bRet;
  if (lWidth > GetDrawWidth() && eFullResize == m_eResizeType)
    SetDrawWidth(lWidth);
  bRet = GetSize(lWidth, lHeight);
  if (!bRet)
    return bRet;
  if (m_lColCount != NOT_USED && m_pTextCol)
    lHeight = m_pTextCol->GetDrawHeight();
  if (eGrowResize == m_eResizeType)
  {
    if (lHeight > GetDrawHeight())
      SetDrawHeight(lHeight);
  }
  else
  {
    if (lHeight != GetDrawHeight())
      SetDrawHeight(lHeight);
  }
  return bRet;
}

void CPDFFrame::SetResizeType(eResizeType i_eResize)
{
  m_eResizeType = i_eResize;
  CheckResize();
}

CPDFFrame::eResizeType CPDFFrame::GetResizeType()
{
  return m_eResizeType;
}

void CPDFFrame::SetOverlapType(eOverlapType i_eOverlapType)
{
  m_eOverlapType = i_eOverlapType;
}

CPDFFrame::eOverlapType CPDFFrame::GetOverlapType()
{
  return m_eOverlapType;
}

bool_a CPDFFrame::SetFrameColumns(long i_lColCount, long i_lColSpace, long i_lColGutter)
{
  m_lColCount = i_lColCount;
  m_lColWidth = i_lColSpace;
  m_lColGutter = i_lColGutter;
  return true_a;
}

bool_a CPDFFrame::MakeColumns()
{
  if (m_pTextCol || m_lColCount <= 1)
    return true_a;
  rec_a recPS;
  CPDFTextBox *pTB = m_pTextCol;
  while (pTB)
  {
    //if (pTB == m_pTextCol)
    //{
    //  pTB = m_pTextCol->GetNextTextBox();
    //  continue;
    //}
    if (pTB->GetNextTextBox())
    {
      pTB = pTB->GetNextTextBox();
      delete pTB->GetPrevTextBox();
    }
    else
    {
      delete pTB;
      pTB = NULL;
    }
  }
  recPS = GetDrawPosSize();
  long lW = m_lColWidth, lG = m_lColGutter, lAW = (recPS.lRight - recPS.lLeft) - 2 * FRAME_MARGIN;
  if (m_lColGutter >= 0)
  {
    lW = lAW - ((m_lColCount - 1) * lG);
    lW /= m_lColCount;
    if (lW < MIN_FRAME_COL_WIDTH)
    {
      if (m_lColWidth <= 0)
        lW = MIN_FRAME_COL_WIDTH;
      else
        lW = m_lColWidth;
      lG = lAW - m_lColCount * lW;
      lG /= (m_lColCount - 1);
      if (lG < 0)
      {
        if (m_lColWidth <= 0)
          return false_a;
        lW = MIN_FRAME_COL_WIDTH;
        lG = lAW - m_lColCount * lW;
        lG /= (m_lColCount - 1);
        if (lG < 0)
          return false_a;
      }
    }
  }
  else
  {
    if (m_lColWidth <= 0)
      lW = MIN_FRAME_COL_WIDTH;
    lG = lAW - m_lColCount * lW;
    lG /= (m_lColCount - 1);
    if (lG < 0)
    {
      lG = MIN_FRAME_GUTTER;
      lW = lAW - ((m_lColCount - 1) * lG);
      lW /= m_lColCount;
      if (lW < MIN_FRAME_COL_WIDTH)
      {
        if (m_lColWidth <= 0)
          return false_a;
        lW = MIN_FRAME_COL_WIDTH;
        lG = lAW - m_lColCount * lW;
        lG /= (m_lColCount - 1);
        if (lG < 0)
          return false_a;
      }
    }
  }

  //pTB = m_pTextCol;
  m_pTextCol = ((CPDFTextBox *)this)->CreateIdentical();
  m_pTextCol->SetWidth(lW);
  m_pTextCol->MoveTo(recPS.lLeft + FRAME_MARGIN, m_pTextCol->GetPosSize().lTop);
  //if (pTB)
  //{
  //  if (!PDFMoveTextBoxContents(GetBaseDoc(), pTB, m_pTextCol))
  //    return false_a;
  //  delete pTB;
  //}

  CPDFTextBox *pTBLast = m_pTextCol;
  for (long i = 0; i < m_lColCount - 1; i++)
  {
    pTB = (CPDFTextBox *)pTBLast->CreateIdentical();
//    pTB = pTBLast->Clone();
    pTBLast->SetNextTextBox(pTB);
    pTB->SetPrevTextBox(pTBLast);
    pTB->MoveTo(pTB->GetPosSize().lLeft + pTBLast->GetWidth() + lG,
                pTB->GetPosSize().lTop);

    pTBLast = pTB;
  }

  if (!PDFMoveTextBoxContents(GetBaseDoc(), this, m_pTextCol))
    return false_a;

  long lH = CalcColumnHeight();

  if (eNoResize == m_eResizeType ||
      (eGrowResize == m_eResizeType && this->GetNextTextBox()))
      ;
  else
  {
    if (eGrowResize == m_eResizeType)
    {
      if (lH > GetDrawHeight())
        SetDrawHeight(lH);
    }
    else
    {
      if (lH != GetDrawHeight())
        SetDrawHeight(lH);
    }
  }

  //pTB = m_pTextCol;
  //while (pTB)
  //{
  //  if (pTB->GetNextTextBox())
  //  {
  //    if (!pTB->CopyOverflowLines(pTB->GetNextTextBox(), true_a))
  //      return false_a;
  //  }
  //  pTB = pTB->GetNextTextBox();
  //}

  return true_a;
}

bool_a CPDFFrame::CopyOverflowLines()
{
  CPDFTextBox *pTB = m_pTextCol;
  while (pTB)
  {
    if (pTB->GetNextTextBox())
    {
      if (!pTB->CopyOverflowLines(pTB->GetNextTextBox(), true_a))
        return false_a;
      if (!pTB->ReplacePageMarks())
        return false_a;
    }
    else
    {
      if (!pTB->ReplacePageMarks())
        return false_a;
      break;
    }
    pTB = pTB->GetNextTextBox();
  }
  if (!m_pTextCol)
    pTB = this;
  if (pTB && this->GetNextTextBox())
  {
    if (((CPDFFrame *)this->GetNextTextBox())->GetTextCol())
    {
      if (!pTB->CopyOverflowLines(((CPDFFrame *)this->GetNextTextBox())->GetTextCol()))
        return false_a;
      if (!pTB->ReplacePageMarks())
        return false_a;
    }
    else
    {
      if (!pTB->CopyOverflowLines(this->GetNextTextBox()))
        return false_a;
      if (!pTB->ReplacePageMarks())
        return false_a;
    }
  }
  else
    if (pTB)
      if (!pTB->ReplacePageMarks())
        return false_a;

  return true_a;
}

//bool_a CPDFFrame::CopyOverflowLines(CPDFTextBox *i_pTextBox, bool_a i_bCheckBreak)
//{
//  if (!m_pTextCol)
//    return CPDFTextBox::CopyOverflowLines(i_pTextBox, bool_a i_bCheckBreak);
//  bool_a bRet = PDFCopyOverflowLines(GetBaseDoc(), this, i_pTextBox, i_bCheckBreak);
//  if (!bRet)
//    return bRet;
//  i_pTextBox->CheckResize();
//  return bRet;
//}

bool_a CPDFFrame::SetDrawHeight(long i_lHeight)
{
  if (!CPDFTextBox::SetDrawHeight(i_lHeight))
    return false_a;
  CPDFTextBox *pTB = m_pTextCol;
  while (pTB)
  {
    if (!pTB->SetDrawHeight(i_lHeight - 2 * FRAME_MARGIN))
      return false_a;
    pTB->MoveTo(pTB->GetPosSize().lLeft, pTB->GetPosSize().lTop - FRAME_MARGIN);
    pTB = pTB->GetNextTextBox();
  }
  return true_a;
}

bool_a CPDFFrame::AddFrame(CPDFFrame *i_pFrame, bool_a i_bUseSize)
{
  if (!m_pTextCol)
  {
    if (!CPDFTextBox::AddFrame(i_pFrame, i_bUseSize))
      return false_a;
  }
  else
  {
    CPDFTextBox *pTB = m_pTextCol;
    while (pTB)
    {
      if (!pTB->AddFrame(i_pFrame, i_bUseSize))
        return false_a;
      pTB = pTB->GetNextTextBox();
    }
  }
  return true_a;
}

long CPDFFrame::CalcColumnHeight()
{
  if (!m_pTextCol || m_lColCount <= 1)
    return NOT_USED;
  PDFParagraph *pPara;
  PDFLine *pLine;
  long lH = 0;
  long lMaxHeight = GetDrawHeight();
  long lWidth, lHeight, lHeight1, lCol, lMin, lCount;

  if (!m_pTextCol->GetSize(lWidth, lHeight))
    return NOT_USED;

//  lMaxHeight = lHeight / m_lColCount;

  do
  {
    lCol = 0;
    lH = 0;
    lHeight1 = 0;
    lMin = lHeight;
    lCount = 0;
    pPara = m_pTextCol->pParagraph;
    while (pPara)
    {
      pLine = pPara->pPDFLine;
      while (pLine)
      {
        lH += pLine->lCurrLineHeight;
        if (lMaxHeight < lH)
        {
          lH -= lMaxHeight;
          if (!lCol || lH < lMin || 0 == lCount)
            lMin = lH;
          if (0 == lCount)
            pLine = pLine->pLineNext;

          lH = 0;
          lCount = 0;
          lCol++;
          continue;
        }
        lHeight1 += pLine->lCurrLineHeight;
        lCount++;
        //pElem = pLine->pElemList;
        //while (pElem)
        //{
        //  pElem = pElem->pLCNext;
        //}
        pLine = pLine->pLineNext;
      }
      pPara = pPara->pParaNext;
    }

    if (lCol >= m_lColCount)
      lMaxHeight += lMin + 1;
  } while (lCol >= m_lColCount);

  return lMaxHeight;
}
