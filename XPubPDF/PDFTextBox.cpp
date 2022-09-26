

#include "pdffnc.h"
#include "PDFBaseFont.h"
#include "PDFTextBox.h"
#include "PDFDocument.h"
#include "PDFUtil.h"

using namespace std;

//deprecated
CPDFTextBox::CPDFTextBox()
{
  m_bDelete = false_a;//TODO: remove it
  //m_Color.SetColor(PDF_DEFAULT_RED, PDF_DEFAULT_GREEN, PDF_DEFAULT_BLUE);
  //m_BgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  this->bTransparent = true_a;
  //m_TBBgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  this->lTBBgImage = NOT_USED;
  this->recIndents.lBottom = 0;
  this->recIndents.lTop = 0;
  this->recIndents.lLeft = 0;
  this->recIndents.lRight = 0;
  this->lIndent = NOT_USED;
  this->sAlignment = NOT_USED;
  this->sVAlignment = NOT_USED;
  this->lPage = NOT_USED;
  this->m_eTextDirection = eHorizontal;
  //COPY_REC(this->recTB, i_recPosSize);
  this->recBorder.fBottom = 0;
  this->recBorder.fLeft = 0;
  this->recBorder.fRight = 0;
  this->recBorder.fTop = 0;
  this->recMargins.lBottom = 0;
  this->recMargins.lLeft = 0;
  this->recMargins.lRight = 0;
  this->recMargins.lTop = 0;
  this->lTBHeight = 0;
  this->lTBWidth = 0;
  this->lLineSpace = NOT_USED;
  this->lLetterSpace = NOT_USED;
  this->bTBCanPageBreak = false_a;
  this->bTBBreak = false_a;
  this->bColumnBreak = false_a;
  this->bPageMark = false_a;
  this->bTBCopy = false_a;

  this->lTabSize = NOT_USED;
  this->cTabDecimalType = NOT_USED;

  this->pTBTabStops = NULL;
  this->sTabsCount = 0;
  this->pTBImages = NULL;
  this->pTBSpaces = NULL;
  this->pParagraph = NULL;
  this->pActiveParagraph = NULL;
  this->pTBNext = NULL;
  this->pTBPrev = NULL;
  this->pParagraph = NULL;
  this->pActiveParagraph = this->pParagraph;
}

CPDFTextBox::CPDFTextBox(CPDFDocument *i_pDoc, rec_a i_recPosSize,
                         long i_lPage, bool_a i_bPage, CPDFTextBox *i_pParent)
  :m_pDoc(i_pDoc),m_bFlush(false_a),m_lLastAnnot(NOT_USED),m_lLastID(1),m_bPage(i_bPage),m_eTBType(eTBTextBox)
{
  m_bDelete = true_a;//TODO: remove it
  m_Color.SetColor(PDF_DEFAULT_RED, PDF_DEFAULT_GREEN, PDF_DEFAULT_BLUE);
  m_BgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  this->bTransparent = true_a;
  m_TBBgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  this->lTBBgImage = NOT_USED;
  this->recIndents.lBottom = 0;
  this->recIndents.lTop = 0;
  this->recIndents.lLeft = 0;
  this->recIndents.lRight = 0;
  this->lIndent = PDF_DEFAULT_FIRST_INDENT;
  this->sAlignment = TF_LEFT;
  this->sVAlignment = TF_UP;
  this->lPage = i_lPage - 1;
  this->m_eTextDirection = eHorizontal;
  COPY_REC(this->recTB, i_recPosSize);
  this->recBorder.fBottom = 0;
  this->recBorder.fLeft = 0;
  this->recBorder.fRight = 0;
  this->recBorder.fTop = 0;
  this->recMargins.lBottom = 0;
  this->recMargins.lLeft = 0;
  this->recMargins.lRight = 0;
  this->recMargins.lTop = 0;
  this->lTBHeight = 0;
  this->lTBWidth = 0;
  this->lLineSpace = PDF_DEFAULT_LINE_SPACE;
  this->lLetterSpace = m_pDoc->GetLetterSpace();
  this->bTBCanPageBreak = false_a;
  this->bTBBreak = false_a;
  this->bColumnBreak = false_a;
  this->bPageMark = false_a;
  this->bTBCopy = false_a;

  this->lTabSize = m_pDoc->GetTabDefaultSize();
  this->cTabDecimalType = m_pDoc->GetTabDefaultDecimal();

  this->pTBTabStops = NULL;
  this->sTabsCount = 0;
  this->pTBImages = NULL;
  this->pTBSpaces = NULL;
  this->pParagraph = NULL;
  this->pActiveParagraph = NULL;
  this->pTBNext = NULL;
  this->pTBPrev = NULL;
  if (i_pParent)
  {
    this->pTBPrev = i_pParent;
    i_pParent->pTBNext = this;
  }
  this->pParagraph = PDFCreateParagraph(GetBaseDoc(), this);
  this->pActiveParagraph = this->pParagraph;
}

CPDFTextBox::CPDFTextBox(CPDFDocument *i_pDoc, long i_lWidth, long i_lHeight,
                         long i_lPage, bool_a i_bPage, CPDFTextBox *i_pParent)
  :m_pDoc(i_pDoc),m_bFlush(false_a),m_lLastAnnot(NOT_USED),m_lLastID(1),m_bPage(i_bPage),m_eTBType(eTBTextBox)
{
  m_bDelete = true_a;//TODO: remove it
  rec_a recPosSize;
  recPosSize.lBottom = 0;
  recPosSize.lLeft = 0;
  recPosSize.lRight = i_lWidth;
  recPosSize.lTop = i_lHeight;

  m_Color.SetColor(PDF_DEFAULT_RED, PDF_DEFAULT_GREEN, PDF_DEFAULT_BLUE);
  m_BgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  this->bTransparent = true_a;
  m_TBBgColor.SetColor(PDF_DEFAULT_BG_RED, PDF_DEFAULT_BG_GREEN, PDF_DEFAULT_BG_BLUE);
  this->lTBBgImage = NOT_USED;
  this->recIndents.lBottom = 0;
  this->recIndents.lTop = 0;
  this->recIndents.lLeft = 0;
  this->recIndents.lRight = 0;
  this->lIndent = PDF_DEFAULT_FIRST_INDENT;
  this->sAlignment = TF_LEFT;
  this->sVAlignment = TF_UP;
  this->lPage = i_lPage - 1;
  this->m_eTextDirection = eHorizontal;
  COPY_REC(this->recTB, recPosSize);
  this->recBorder.fBottom = 0;
  this->recBorder.fLeft = 0;
  this->recBorder.fRight = 0;
  this->recBorder.fTop = 0;
  this->recMargins.lBottom = 0;
  this->recMargins.lLeft = 0;
  this->recMargins.lRight = 0;
  this->recMargins.lTop = 0;
  this->lTBHeight = 0;
  this->lTBWidth = 0;
  this->lLineSpace = PDF_DEFAULT_LINE_SPACE;
  this->lLetterSpace = m_pDoc->GetLetterSpace();
  this->bTBCanPageBreak = false_a;
  this->bTBBreak = false_a;
  this->bColumnBreak = false_a;
  this->bPageMark = false_a;
  this->bTBCopy = false_a;

  this->lTabSize = m_pDoc->GetTabDefaultSize();
  this->cTabDecimalType = m_pDoc->GetTabDefaultDecimal();

  this->pTBTabStops = NULL;
  this->sTabsCount = 0;
  this->pTBImages = NULL;
  this->pTBSpaces = NULL;
  this->pParagraph = NULL;
  this->pActiveParagraph = NULL;
  this->pTBNext = NULL;
  this->pTBPrev = NULL;
  if (i_pParent)
  {
    this->pTBPrev = i_pParent;
    i_pParent->pTBNext = this;
  }
  this->pParagraph = PDFCreateParagraph(GetBaseDoc(), this);
  this->pActiveParagraph = this->pParagraph;
}

CPDFTextBox::~CPDFTextBox()
{
  if (m_bDelete)//TODO: remove it
  {
    PDFParagraph *pParaOld, *pPara = this->pParagraph;

    while (pPara)
    {
      pParaOld = pPara;
      pPara = pPara->pParaNext;
      PDFFreeParagraph(&pParaOld);
    }
    if (this->pTBTabStops)
      free(this->pTBTabStops);
    this->sTabsCount = 0;
    PDFFreeTBImageList(this);
    PDFFreeTBSpaceList(this);
  }
}

PDFDocPTR CPDFTextBox::GetBaseDoc()
{
  return (PDFDocPTR)m_pDoc;
}

void CPDFTextBox::SetDocument(CPDFDocument *i_pDoc)
{
  m_pDoc = i_pDoc;
}

void CPDFTextBox::CopyAttr(CPDFTextBox *i_pTextBoxTo)
{
  if (!i_pTextBoxTo)
    return;
  PDFCopyTextBox(i_pTextBoxTo, this);
  i_pTextBoxTo->m_lLastID = this->m_lLastID;
  i_pTextBoxTo->m_lLastAnnot = this->m_lLastAnnot;
  i_pTextBoxTo->m_bFlush = this->m_bFlush;
  i_pTextBoxTo->m_bPage = this->m_bPage;
  i_pTextBoxTo->m_pDoc = this->m_pDoc;
}

CPDFTextBox *CPDFTextBox::CreateIdentical()
{
  CPDFTextBox *pRetTB = NULL;
  PDFParagraph *pPara = NULL;
  pRetTB = new CPDFTextBox(m_pDoc, GetPosSize(), GetPageNumber(), IsPage());
  if (NULL == pRetTB)
     return pRetTB;

  pPara = pRetTB->pParagraph;
//  memcpy(pRetTB, this, sizeof(CPDFTextBox));
  CopyAttr(pRetTB);
  pRetTB->pParagraph = pRetTB->pActiveParagraph = pPara;

  pRetTB->pTBImages = NULL;
  pRetTB->pTBSpaces = NULL;
  pRetTB->pTBTabStops = NULL;
  pRetTB->sTabsCount = 0;
  pRetTB->SetTabs(this->pTBTabStops, this->sTabsCount);
  pRetTB->SetPrevTextBox(NULL);
  pRetTB->SetNextTextBox(NULL);
  return pRetTB;
}

CPDFTextBox *CPDFTextBox::Clone()
{
  CPDFTextBox *pRetTB = NULL;
  pRetTB = new CPDFTextBox(m_pDoc, GetPosSize(), GetPageNumber(), IsPage());
  if (NULL == pRetTB)
     return pRetTB;

  PDFFreeParagraph(&pRetTB->pParagraph);
  //memcpy(pRetTB, this, sizeof(CPDFTextBox));
  CopyAttr(pRetTB);

  pRetTB->pTBImages = NULL;
  pRetTB->pTBSpaces = NULL;
  pRetTB->pParagraph = NULL;
  pRetTB->pActiveParagraph = NULL;
  pRetTB->pTBTabStops = NULL;
  pRetTB->sTabsCount = 0;
  pRetTB->SetTabs(this->pTBTabStops, this->sTabsCount);
  pRetTB->SetPrevTextBox(NULL);
  pRetTB->SetNextTextBox(NULL);
  pRetTB->SetImages(this->pTBImages);
  pRetTB->SetSpaces(this->pTBSpaces);
  pRetTB->SetParagraph(this->pParagraph);
  return pRetTB;
}

bool_a CPDFTextBox::SetImages(PDFTBImage *i_pTBImages)
{
  PDFTBImage *pImg = NULL, *pLastImg = NULL, *pTBImg = i_pTBImages;
  if (pTBImg)
  {
    pTBImages = pImg = (PDFTBImage *)PDFMalloc(sizeof(PDFTBImage));
    if (!pTBImages)
      return false_a;
  }
  while (pTBImg)
  {
    memcpy(pImg, pTBImg, sizeof(PDFTBImage));
    pImg->pTBImageNext = NULL;
    pImg->pTBImagePrev = pLastImg;
    if (pLastImg)
      pLastImg->pTBImageNext = pImg;
    pLastImg = pImg;
    if (pTBImg->pTBImageNext)
    {
      pImg = (PDFTBImage *)PDFMalloc(sizeof(PDFTBImage));
      if (!pImg)
         return false_a;
    }
    pTBImg = pTBImg->pTBImageNext;
  }
  return true_a;
}

bool_a CPDFTextBox::SetSpaces(PDFTBSpace *i_pTBSpaces)
{
  PDFTBSpace *pSpc = NULL, *pLastSpc = NULL, *pTBSpc = i_pTBSpaces;
  if (pTBSpc)
  {
    pTBSpaces = pSpc = (PDFTBSpace *)PDFMalloc(sizeof(PDFTBSpace));
    if (!pTBSpaces)
      return false_a;
  }
  while (pTBSpc)
  {
    memcpy(pSpc, pTBSpc, sizeof(PDFTBSpace));
    pSpc->pTBSpaceNext = NULL;
    pSpc->pTBSpacePrev = pLastSpc;
    if (pLastSpc)
      pLastSpc->pTBSpaceNext = pSpc;
    pLastSpc = pSpc;
    if (pTBSpc->pTBSpaceNext)
    {
      pSpc = (PDFTBSpace *)PDFMalloc(sizeof(PDFTBSpace));
      if (!pSpc)
         return false_a;
    }
    pTBSpc = pTBSpc->pTBSpaceNext;
  }
  return true_a;
}

bool_a CPDFTextBox::SetParagraph(PDFParagraph *i_pParagraph)
{
  PDFParagraph *pPrg = NULL, *pLastPrg = NULL, *pTBPrg = i_pParagraph;
  if (pTBPrg)
  {
    this->pParagraph = pPrg = (PDFParagraph *)PDFMalloc(sizeof(PDFParagraph));
    if (!this->pParagraph)
      return false_a;
  }
  while (pTBPrg)
  {
    memcpy(pPrg, pTBPrg, sizeof(PDFParagraph));
    pPrg->pParaTabStops = (PDFTabStop *)PDFMalloc(this->sTabsCount * sizeof(PDFTabStop));
    memcpy(pPrg->pParaTabStops, this->pTBTabStops, this->sTabsCount * sizeof(PDFTabStop));
    pPrg->sParaTabsCount = this->sTabsCount;
    pPrg->pPDFLine = NULL;
    pPrg->pPDFLastLine = NULL;
    SetParagraphLines(pPrg, pTBPrg->pPDFLine);
    pPrg->pParaNext = NULL;
    pPrg->pParaPrev = pLastPrg;
    if (pLastPrg)
      pLastPrg->pParaNext = pPrg;
    pLastPrg = pPrg;
    if (pTBPrg->pParaNext)
    {
      pPrg = (PDFParagraph *)PDFMalloc(sizeof(PDFParagraph));
      if (!pPrg)
        return false_a;
    }
    pTBPrg = pTBPrg->pParaNext;
  }
  if (pLastPrg)
    pActiveParagraph = pLastPrg;

  return true_a;
}

bool_a CPDFTextBox::SetParagraphLines(PDFParagraph *i_pParagraph, PDFLine *i_pLine)
{
  PDFLine *pLine = NULL, *pLastLine = NULL, *pTBLine = i_pLine;
  if (pTBLine)
  {
    i_pParagraph->pPDFLine = pLine = (PDFLine *)PDFMalloc(sizeof(PDFLine));
    if (!pLine)
      return false_a;
  }
  while (pTBLine)
  {
    memcpy(pLine, pTBLine, sizeof(PDFLine));
    pLine->pElemList = NULL;
    SetLineElements(pLine, pTBLine->pElemList);
    pLine->pLineNext = NULL;
    pLine->pLinePrev = pLastLine;
    if (pLastLine)
      pLastLine->pLineNext = pLine;
    pLastLine = pLine;
    if (pTBLine->pLineNext)
    {
      pLine = (PDFLine *)PDFMalloc(sizeof(PDFLine));
      if (!pLine)
        return false_a;
    }
    pTBLine = pTBLine->pLineNext;
  }
  if (pLastLine)
    i_pParagraph->pPDFLastLine = pLastLine;
  return true_a;
}

bool_a CPDFTextBox::SetLineElements(PDFLine *i_pLine, PDFLineElemList *i_pElement)
{
  PDFLineElemList *pElem = NULL, *pLastElem = NULL, *pTBElem = i_pElement;
  if (pTBElem)
  {
    i_pLine->pElemList = pElem = new PDFLineElemList();
    if (!pElem)
      return false_a;
  }
  while (pTBElem)
  {
    *pElem = *pTBElem;
    //memcpy(pElem, pTBElem, sizeof(PDFLineElemList));
    if (pLastElem)
    {
      pLastElem->pLCNext = pElem;
      pElem->pLCPrev = pLastElem;
    }
    pLastElem = pElem;
    if (pTBElem->pLCNext)
    {
      pElem = new PDFLineElemList();
      if (!pElem)
        return false_a;
    }
    pTBElem = pTBElem->pLCNext;
  }
  return true_a;
}

bool_a CPDFTextBox::MoveTo(long i_lX, long i_lY)
{
  return PDFMoveTextBoxTo(GetBaseDoc(), i_lX, i_lY, this);
}

bool_a CPDFTextBox::CheckResize()
{
  return true_a;
}

bool_a CPDFTextBox::FontSizeChanged()
{
  if (HasLastLineOnlyWhiteSigns())
  {
    if (!this->pActiveParagraph || !this->pActiveParagraph->pPDFLastLine || !m_pDoc)
      return false_a;

    if (m_pDoc->GetActualFontHeight() != this->pActiveParagraph->pPDFLastLine->lCurrLineHeight)
    {
      if (!m_pDoc)
        return false_a;
      if (this->pActiveParagraph->pPDFLastLine->pElemList)
      {
        this->pActiveParagraph->pPDFLastLine->pElemList->fFontSize = m_pDoc->GetConvertFontSize();
      }
      if (!PDFAdjustParagraph(GetBaseDoc(), this, GetTopDrawPos() - this->lTBHeight
                              + this->pActiveParagraph->lParaHeight - 1, false_a))
         return false_a;
      return CheckResize();
    }
  }
  return false_a;
}

bool_a CPDFTextBox::IsTableOverflow()
{
  PDFParagraph *pPara = NULL;
  PDFLine *pLine = NULL;
  long lHeight = 0;
  long lMaxHeight = GetDrawHeight();
  pPara = this->pParagraph;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      lHeight += pLine->lCurrLineHeight;
      if (lMaxHeight < lHeight)
      {
        PDFTBSpace *pTBSpace;
        PDFLineElemList *pElemList = pLine->pElemList;
        while(pElemList)
        {
          pTBSpace = PDFGetTBSpace(this, pElemList->lSpace);
          if (pTBSpace && eFixTableWidth == pTBSpace->eSpaceType)
            return true_a;
          pElemList = pElemList->pLCNext;
        }
        return false_a;
      }
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return false_a;
}

//TODO: same counting is in PDFCopyOverflowLines - make it better
bool_a CPDFTextBox::CanDivide()
{
  PDFParagraph *pPara = NULL;
  PDFLine *pLine = NULL;
  long lHeight = 0;
  long lMaxHeight = GetDrawHeight();
  pPara = this->pParagraph;
  while (pPara)
  {
    lHeight += pPara->recIndents.lTop;
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      if (lMaxHeight < (lHeight + pLine->lCurrLineHeight))
      {
        if (pPara == this->pParagraph && pLine == pPara->pPDFLine)
        {
          PDFTBSpace *pTBSpace;
          if (pPara->pParaNext || (pLine->pLineNext && pLine->pLineNext->pElemList))
            return true_a;
          if (!pLine->pElemList)
            return false_a;
          if (pLine->pElemList->lSpace <= 0)
            return false_a;
          pTBSpace = PDFGetTBSpace(this, pLine->pElemList->lSpace);
          if (!pTBSpace || eFixTableWidth != pTBSpace->eSpaceType)
            return false_a;
          CPDFTable *pTable = (CPDFTable *)pLine->pElemList->lSpace;
          return pTable->CanBeDivided(lMaxHeight - lHeight);
        }
        else
          return true_a;
      }
      lHeight += pLine->lCurrLineHeight;
      pLine = pLine->pLineNext;
    }
    lHeight += pPara->recIndents.lBottom;
    pPara = pPara->pParaNext;
  }
  if (HasBreak())
    return true_a;
  return false_a;
}

bool_a CPDFTextBox::CopyAttributtes(CPDFTextBox *i_pTextBox)
{
  bool_a bRet = PDFCopyTextBoxAttributes(GetBaseDoc(), this, i_pTextBox);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

long CPDFTextBox::GetWidth()
{
  rec_a recPosSize = GetPosSize();
  return (recPosSize.lRight - recPosSize.lLeft);
}

bool_a CPDFTextBox::SetWidth(long i_lWidth)
{
  bool_a bRet = false_a;
  if ((i_lWidth - GetWidth()) == 0)
    return true_a;
  recTB.lRight += i_lWidth - GetWidth();
  bRet = PDFAdjustParagraph(GetBaseDoc(), this, GetTopDrawPos(), false_a);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

long CPDFTextBox::GetDrawWidth(bool_a i_bDirection)
{
  long lRet = NOT_USED;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lRight - PDFRound(this->recBorder.fRight) - this->recMargins.lRight;
  lRet -= (recPosSize.lLeft + PDFRound(this->recBorder.fLeft) + this->recMargins.lLeft);
  return lRet;
}

bool_a CPDFTextBox::SetDrawWidth(long i_lWidth)
{
  bool_a bRet = false_a;
  if ((i_lWidth - GetDrawWidth()) == 0)
    return true_a;
  recTB.lRight += i_lWidth - GetDrawWidth();
  bRet = PDFAdjustParagraph(GetBaseDoc(), this, GetTopDrawPos(), false_a);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

long CPDFTextBox::GetHeight()
{
  rec_a recPosSize = GetPosSize();
  return (recPosSize.lTop - recPosSize.lBottom);
}

bool_a CPDFTextBox::SetHeight(long i_lHeight)
{
  bool_a bRet = false_a;
  if ((GetHeight() - i_lHeight) == 0)
    return true_a;
  recTB.lBottom += GetHeight() - i_lHeight;
  bRet = CheckResize();
  return bRet;
}

long CPDFTextBox::GetDrawHeight(bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lTop - PDFRound(this->recBorder.fTop) - this->recMargins.lTop;
  lRet -= (recPosSize.lBottom + PDFRound(this->recBorder.fBottom) + this->recMargins.lBottom);
  return lRet;
}

bool_a CPDFTextBox::SetDrawHeight(long i_lHeight)
{
  bool_a bRet = false_a;
  if ((GetDrawHeight() - i_lHeight) == 0)
    return true_a;
  recTB.lBottom += GetDrawHeight() - i_lHeight;
  bRet = CheckResize();
  return bRet;
}

long CPDFTextBox::GetDividedHeight(long i_lHeight)
{
  PDFParagraph *pPara = NULL;
  PDFLine *pLine = NULL;
  long lHeight = 0;

  //long lTBH = GetHeight();

  //if (i_lHeight >= lTBH)
  //  return NOT_USED;
  //if (i_lHeight >= this->lTBHeight)
  //  return NOT_USED;

  lHeight += this->recMargins.lTop + PDFRound(this->recBorder.fBottom);
  pPara = this->pParagraph;
  while (pPara)
  {
    lHeight += pPara->recIndents.lTop;

    pLine = pPara->pPDFLine;
    while (pLine)
    {
      lHeight += pLine->lCurrLineHeight;
      if (lHeight > i_lHeight)
      {
        PDFLineElemList *pElem = PDFGetFixTableElem(this, pLine);
        if (this->pParagraph->pPDFLine == pLine && !pElem)
          return NOT_USED;

        if (pElem)
        {
          //lHeight -= 
          if (!((CPDFTable *)pElem->lSpace)->CanBeDivided(i_lHeight -
                                                              (lHeight - pLine->lCurrLineHeight)))
            return NOT_USED;
          lHeight = i_lHeight;//TODO: is it correct???
        } else 
        {
          lHeight -= pLine->lCurrLineHeight;
          if (pPara->pPDFLine == pLine)
            lHeight -= pPara->recIndents.lTop;
        }
        if (lHeight <= 0)
          return NOT_USED;
        return GetHeight() - lHeight;
      }
      pLine = pLine->pLineNext;
    }

    lHeight += pPara->recIndents.lBottom;
    pPara = pPara->pParaNext;
  }
  return lHeight;
}

bool_a CPDFTextBox::Join(CPDFTextBox *i_pParent)
{
  return PDFJoinTextBox(i_pParent, this);
}

CPDFTextBox *CPDFTextBox::UnJoin()
{
  return (CPDFTextBox *)PDFUnJoinTextBox(this);
}

bool_a CPDFTextBox::AddFillBreak(long i_lBreakCount)
{
  return PDFAddFillBreak(GetBaseDoc(), this, i_lBreakCount);
}

bool_a CPDFTextBox::AddAnchor(string i_strAnchor)
{
  return PDFAddAnchor(GetBaseDoc(), this, i_strAnchor);
}

bool_a CPDFTextBox::ReplaceBookmarks()
{
  if (!m_pDoc)
    return false_a;
  PDFParagraph  *pPara = NULL;
  PDFLine       *pLine      = NULL;
  PDFLineElemList *pElemList = NULL;
  long lHeight = 0, lWidth = 0;
  long lTop, lLeft;
  pPara = this->pParagraph;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      lWidth = 0;
      pElemList = pLine->pElemList;
      while(pElemList)
      {
        lWidth += pElemList->lElemWidth;
        if (pElemList->m_strAnchor.size() > 0)
        {
          CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber());
          if (!pPage)
            return false_a;
          CPDFOutlineEntry *pOE = NULL;
          long i;
          for (i = 0; i < m_pDoc->GetOutlineEntryCount(); i++)
          {
            pOE = m_pDoc->GetOutlineEntry(i);
            if (!pOE)
              continue;
            if (pOE->GetAnchor().compare(pElemList->m_strAnchor) == 0)
            {
              if (pOE->GetIgnorePos())
              {
                lTop = pPage->GetHeight();
                lLeft = 0;
              }
              else
              {
                lTop = GetTopDrawPos() - lHeight;
                lLeft = lWidth;
              }

              pOE->SetDestination(GetPageNumber(), eDestLeftTopZoom,
                            lLeft, lTop, 0, 0, pOE->GetZoom());
            }
          }

          lTop = GetTopDrawPos() - lHeight;
          lLeft = lWidth;
          m_pDoc->ChangeLinkDest(pElemList->m_strAnchor.c_str(), GetPageNumber(), eDestLeftTopZoom,
                                  lLeft, lTop, 0, 0, NOT_USED);
        }
        pElemList = pElemList->pLCNext;
      }
      lHeight += pLine->lCurrLineHeight;
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return true_a;
//  return PDFReplaceBookmarks(GetBaseDoc(), this);
}

bool_a CPDFTextBox::AddPageMark(ePAGEMARKTYPE i_eType)
{
  return PDFAddPageMark(GetBaseDoc(), this, i_eType);
}

bool_a CPDFTextBox::ReplacePageMarks()
{
  long lTop = 0;
  if (!this->bPageMark)
    return true_a;
  lTop = GetTopDrawPos();
  if (!PDFAdjustParagraph(GetBaseDoc(), this, lTop, true_a))
    return false_a;
  return true_a;
}

bool_a CPDFTextBox::BeginLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                              long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                              const char *i_pLocation)
{
  i_fZoom /= 100.0;
  long lAnnot = PDFCreateLinkAnnotationFile(GetBaseDoc(), i_pczName, i_lPage, i_eDest, i_lLeft,
                                            i_lTop, i_lRight, i_lBottom, i_fZoom, i_pLocation);
  if (lAnnot < 0)
    return false_a;
  m_lLastAnnot = lAnnot;
  return PDFAddLink(GetBaseDoc(), this, m_lLastAnnot);
}

bool_a CPDFTextBox::BeginLink(const char *i_pczName, const char *i_pLocation)
{
  long lAnnot = PDFCreateLinkAnnotationURI(GetBaseDoc(), i_pczName, i_pLocation);
  if (lAnnot < 0)
    return false_a;
  m_lLastAnnot = lAnnot;
  return PDFAddLink(GetBaseDoc(), this, m_lLastAnnot);
}

bool_a CPDFTextBox::BeginLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                              long i_lTop, long i_lRight, long i_lBottom, float i_fZoom)
{
  i_fZoom /= 100.0;
  long lAnnot = PDFCreateLinkAnnotationDest(GetBaseDoc(), i_pczName, i_lPage, i_eDest, i_lLeft,
                                            i_lTop, i_lRight, i_lBottom, i_fZoom);
  if (lAnnot < 0)
    return false_a;
  m_lLastAnnot = lAnnot;
  return PDFAddLink(GetBaseDoc(), this, m_lLastAnnot);
}

bool_a CPDFTextBox::BeginLink(const char *i_pczName)
{
  long lAnnot = PDFCreateLinkAnnotationEmptyDest(GetBaseDoc(), i_pczName);
  if (lAnnot < 0)
    return false_a;
  m_lLastAnnot = lAnnot;
  return PDFAddLink(GetBaseDoc(), this, m_lLastAnnot);
}

bool_a CPDFTextBox::ChangeLinkDest(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                                    long i_lTop, long i_lRight, long i_lBottom, float i_fZoom)
{
  return PDFChangeAnnotationDest(GetBaseDoc(), i_pczName, i_lPage, i_eDest, i_lLeft,
                                  i_lTop, i_lRight, i_lBottom, i_fZoom);

}

bool_a CPDFTextBox::EndLink()
{
  bool_a bRet = PDFAddLink(GetBaseDoc(), this, m_lLastAnnot);
  m_lLastAnnot = NOT_USED;
  return bRet;
}

long CPDFTextBox::AddText(char *i_pText)
{
  long lRet = 0;
  lRet = PDFAddTextToTextBox(GetBaseDoc(), i_pText, this);
  //wchar_t czTemp[100];
  //czTemp[0] = 0x0118;
  //czTemp[1] = 0x0159;
  //czTemp[2] = 0x015E;
  //czTemp[3] = 0x05DE;
  //czTemp[4] = 0x05E7;
  //czTemp[5] = 0x01A1;
  //czTemp[6] = 0x06D2;
  //czTemp[7] = 0x0637;
  //czTemp[8] = 0x064B;
  //czTemp[9] = 0x0679;
  //czTemp[10] = 0x0065;
  //czTemp[11] = 0;
  //lRet = PDFAddTextToTextBox(GetBaseDoc(), czTemp, this);
  CheckResize();
  return lRet;
}

long CPDFTextBox::AddText(wchar_t *i_pText)
{
  long lRet = 0;
  lRet = PDFAddTextToTextBox(GetBaseDoc(), i_pText, this);
  //wchar_t czTemp[100];
  //czTemp[0] = 0x0118;
  //czTemp[1] = 0x0159;
  //czTemp[2] = 0x015E;
  //czTemp[3] = 0x05DE;
  //czTemp[4] = 0x05E7;
  //czTemp[5] = 0x01A1;
  //czTemp[6] = 0x06D2;
  //czTemp[7] = 0x0637;
  //czTemp[8] = 0x064B;
  //czTemp[9] = 0x0679;
  //czTemp[10] = 0x0065;
  //czTemp[11] = 0;
  //lRet = PDFAddTextToTextBox(GetBaseDoc(), czTemp, this);
  CheckResize();
  return lRet;
}

bool_a CPDFTextBox::AddImage(char *i_pImageName, long i_lX, long i_lY, long i_lWidth, long i_lHeight,
                long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                bool_a i_bAddToLine)
{
  bool_a bRet = false_a;
//  if (!m_pDoc->CreateImage(i_pImageName))
//    return bRet;
  bRet = PDFAddImageToTextBox(GetBaseDoc(), i_pImageName, this, i_lX, i_lY, i_lWidth, i_lHeight, i_lLeftDist,
                              i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::AddScaledImage(char *i_pImageName, long i_lX, long i_lY, long i_lScaleX, long i_lScaleY,
                                long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                                bool_a i_bAddToLine)
{
  bool_a bRet = false_a;
//  if (!m_pDoc->CreateImage(i_pImageName))
//    return bRet;
  bRet = PDFAddScaledImageToTextBox(GetBaseDoc(), i_pImageName, this, i_lX, i_lY, i_lScaleX, i_lScaleY, i_lLeftDist,
                              i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::IsTableLast(CPDFTable *i_pTable)
{
  PDFLine *pLine = NULL;
  bool_a bRet = false_a;
  PDFLineElemList *pElem = NULL;
  if (!this->pActiveParagraph)
    return false_a;
  pLine = this->pActiveParagraph->pPDFLine;
  while (pLine)
  {
    pElem = pLine->pElemList;
    while (pElem)
    {
      bRet = false_a;
      if (i_pTable != NULL)
      {
        if (pElem->lSpace == (long)i_pTable)
          bRet = true_a;
      }
      else
      {
        if (pElem->lSpace > 0)
          bRet = true_a;
      }
      pElem = pElem->pLCNext;
    }
    pLine = pLine->pLineNext;
  }
  return bRet;
}

bool_a CPDFTextBox::IsTableFirst(CPDFTable *i_pTable)
{
  PDFLineElemList *pElem = NULL;
  PDFLine *pLine = NULL;
  PDFParagraph *pPara = NULL;
  pPara = this->pParagraph;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      if (pLine->lCurrLineHeight > 0 || pLine->lCurrLineAsc > 0)
      {
        pElem = pLine->pElemList;
        if (!pElem)
        {
           if (pLine->pLinePrev || pPara->pParaPrev)
              return false_a;
        }
        while (pElem)
        {
          if (pElem->lSpace == (long)i_pTable)
            return true_a;
          else
            return false_a;
//          pElem = pElem->pLCNext;
        }
      }
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return true_a;
}

bool_a CPDFTextBox::AddEmptySpace(long i_lHeight)
{
  rec_a recPosSize;
  rec_a recTemp = GetPosSize();

  recPosSize.lTop = recTemp.lTop;
  recPosSize.lBottom = recTemp.lTop - i_lHeight;
  recPosSize.lLeft = recTemp.lLeft - 1;
  recPosSize.lRight = recTemp.lRight + 1;

  if (!PDFAddToTBSpaceList(GetBaseDoc(), this, recPosSize, m_lLastID, eFixEmptySpace))
    return false;

  if (!CheckResize())
    return false_a;
  return true_a;
}

bool_a CPDFTextBox::AddFrame(CPDFFrame *i_pFrame, bool_a i_bUseSize)
{
  bool_a bRet = false_a;
  rec_a recPosSize;
  if (!i_bUseSize)
  {
    recPosSize.lTop = 0;
    recPosSize.lBottom = 0;
    recPosSize.lLeft = 0;
    recPosSize.lRight = 0;

    bRet = PDFAddToTBSpaceList(GetBaseDoc(), this, recPosSize, (long)i_pFrame, eFrame);
  }
  else
  {
    float fL, fT, fB, fR;
    recPosSize = i_pFrame->GetPosSize();

    if (!i_pFrame->GetBorder(fL, fT, fB, fR))
      return false_a;
    recPosSize.lLeft -= PDFRound(fL / 2.f);
    recPosSize.lTop -= PDFRound(fT / 2.f);
    recPosSize.lBottom += PDFRound(fB / 2.f);
    recPosSize.lRight += PDFRound(fR / 2.f);

    bRet = PDFAddToTBSpaceList(GetBaseDoc(), this, recPosSize, (long)i_pFrame, eFixFrame);
  }
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::UpdateTable(CPDFTable *i_pTable)
{
  if (!i_pTable)
    return false_a;
  PDFTBSpace  *pTBSpace = PDFGetTBSpace(this, (long)i_pTable);
  if (!pTBSpace)
    return false_a;
  pTBSpace->recSpacePos.lLeft = i_pTable->GetXPos();
  pTBSpace->recSpacePos.lRight = pTBSpace->recSpacePos.lLeft + i_pTable->GetMaxWidth();
  return true_a;
}

bool_a CPDFTextBox::UpdateYPosSpaces()
{
  PDFParagraph *pPara = this->pParagraph;
  PDFLine *pLine = NULL;
  PDFLineElemList *pElem;
  long lH = 0;

  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      lH += pLine->lCurrLineHeight;
      pElem = pLine->pElemList;
      while (pElem)
      {
        if (pElem->lSpace > 0)
        {
          PDFTBSpace  *pTBSpace = PDFGetTBSpace(this, pElem->lSpace);
          if (pTBSpace)
          {
            long lHeight = pTBSpace->recSpacePos.lTop - pTBSpace->recSpacePos.lBottom;

            if (pTBSpace->eSpaceType != eFrame)
              pTBSpace->recSpacePos.lTop = GetTopDrawPos() - (lH - pLine->lCurrLineHeight);
            else
              pTBSpace->recSpacePos.lTop = GetTopDrawPos() - (lH - pLine->lCurrLineHeight);
            pTBSpace->recSpacePos.lBottom = pTBSpace->recSpacePos.lTop - lHeight;


            switch (pTBSpace->eSpaceType)
            {
            case eFrame:
              ((CPDFFrame *)pElem->lSpace)->MoveTo(((CPDFFrame *)pElem->lSpace)->GetPosSize().lLeft,
                                                                              pTBSpace->recSpacePos.lTop);
              break;
            case eFixTableWidth:
              ((CPDFTable *)pElem->lSpace)->MoveTo(pTBSpace->recSpacePos.lLeft, pTBSpace->recSpacePos.lTop);
              break;
            }
          }
        }
        pElem = pElem->pLCNext;
      }

      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }

  return true_a;
}

bool_a CPDFTextBox::AddTable(CPDFTable *i_pTable)
{
  if (!i_pTable)
    return false_a;
  bool_a bRet = false_a;
  rec_a recPosSize;
  recPosSize.lTop = i_pTable->GetYPos();
  recPosSize.lBottom = recPosSize.lTop - i_pTable->GetRowsHeights(i_pTable->GetRowCount());
  recPosSize.lLeft = i_pTable->GetXPos();
  recPosSize.lRight = recPosSize.lLeft + i_pTable->GetMaxWidth();
  i_pTable->SetInTextBox(this);
  bRet = PDFAddToTBSpaceList(GetBaseDoc(), this, recPosSize, (long)i_pTable, eFixTableWidth);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::AddTable(CPDFTable *i_pTable, long i_lYPos, long i_lHeight)
{
  bool_a bRet = false_a;
  rec_a recPosSize;
  long lHeight, lWidth;
  if (!GetSize(lWidth, lHeight))
    return false_a;
  if (i_lYPos < 0)
  {
    PDFTBSpace *pSpace = PDFGetTBSpace(this, (long)i_pTable);
    if (!pSpace)
      return false_a;
    recPosSize.lTop = pSpace->recSpacePos.lTop;
  }
  else
    recPosSize.lTop = i_lYPos;
  recPosSize.lBottom = recPosSize.lTop - i_lHeight;
  recPosSize.lLeft = i_pTable->GetXPos();
  recPosSize.lRight = recPosSize.lLeft + i_pTable->GetMaxWidth();
  i_pTable->SetInTextBox(this);
  bRet = PDFAddToTBSpaceList(GetBaseDoc(), this, recPosSize, (long)i_pTable, eFixTableWidth);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::RemoveEmptyLineAfterTable()
{
  if (!this->pActiveParagraph || !this->pActiveParagraph->pPDFLastLine)
    return false_a;

  long lTmpTestHeight = CalcParagraphsHeights(this->pParagraph);
  if (lTmpTestHeight != this->lTBHeight)
    lTmpTestHeight = lTmpTestHeight;

  if (!PDFIsLastLineEmpty(this))
    return true_a;
  PDFLine *pLine;
  if (this->pActiveParagraph->pPDFLastLine == this->pActiveParagraph->pPDFLine
      || (this->pActiveParagraph->pPDFLastLine->pLinePrev == this->pActiveParagraph->pPDFLine
          && !this->pActiveParagraph->pPDFLastLine->lCurrLineHeight))
  {
    if (!this->pActiveParagraph->pParaPrev)
      return true_a;
    pLine = this->pActiveParagraph->pParaPrev->pPDFLastLine;
    if (!pLine)
      return true_a;
    if (!pLine->lCurrLineHeight)
      pLine = pLine->pLinePrev;
    PDFLineElemList *pT = PDFGetFixTableElem(this, pLine);
    if (!pT)
      return true_a;
    PDFParagraph *pPara = this->pActiveParagraph;
    this->pActiveParagraph = this->pActiveParagraph->pParaPrev;
    this->pActiveParagraph->pParaNext = NULL;
    this->lTBHeight -= pPara->lParaHeight;
    PDFFreeParagraph(&pPara);
  }
  else
  {
    PDFLineElemList *pT = PDFGetFixTableElem(this, this->pActiveParagraph->pPDFLastLine->pLinePrev);
    if (!pT)
      return true_a;
    pLine = this->pActiveParagraph->pPDFLastLine;
    this->pActiveParagraph->pPDFLastLine = this->pActiveParagraph->pPDFLastLine->pLinePrev;
    this->pActiveParagraph->pPDFLastLine->pLineNext = NULL;
    this->pActiveParagraph->lParaHeight -= pLine->lCurrLineHeight;
    this->lTBHeight -= pLine->lCurrLineHeight;
    PDFDeleteLineElemList(&(pLine->pElemList));
    free(pLine);
  }

  lTmpTestHeight = CalcParagraphsHeights(this->pParagraph);
  if (lTmpTestHeight != this->lTBHeight)
    lTmpTestHeight = lTmpTestHeight;

  if (!CheckResize())
    return false_a;

  lTmpTestHeight = CalcParagraphsHeights(this->pParagraph);
  if (lTmpTestHeight != this->lTBHeight)
    lTmpTestHeight = lTmpTestHeight;

  return true_a;
}

bool_a CPDFTextBox::HasLastLineOnlyWhiteSigns()
{
  if (!this->pActiveParagraph || !this->pActiveParagraph->pPDFLastLine)
    return true_a;
  PDFLineElemList *pElem = NULL;
  long lLen;
  pElem = this->pActiveParagraph->pPDFLastLine->pElemList;
  while (pElem)
  {
    if (pElem->lSpace > 0)
      return false_a;
    if (pElem->lImage >= 0)
      return false_a;
    if (NULL != pElem->m_pStr && (lLen = (long)pElem->m_pStr->Length()) > 0)
    {
      if (!pElem->m_pStr->HasOnlyWhiteSigns())
        return false_a;
    }
    pElem = pElem->pLCNext;
  }
  return true_a;
}

bool_a CPDFTextBox::HasOnlyTable(CPDFTable *i_pTable)
{
  PDFLineElemList *pElem = NULL;
  PDFLine *pLine = NULL;
  PDFParagraph *pPara = NULL;
  pPara = this->pParagraph;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      if (pLine->lCurrLineHeight > 0 || pLine->lCurrLineAsc > 0)
      {
        pElem = pLine->pElemList;
        while (pElem)
        {
          if (pElem->lSpace != (long)i_pTable)
            return false_a;
          pElem = pElem->pLCNext;
        }
      }
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return true_a;
}

bool_a CPDFTextBox::HasOnlyTable()
{
  PDFLineElemList *pElem = NULL;
  PDFLine *pLine = NULL;
  PDFParagraph *pPara = NULL;
  pPara = this->pParagraph;
  long lTable = -1;
  bool_a bRet = false_a;
  PDFTBSpace *pTBSpace;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      if (pLine->lCurrLineHeight > 0 || pLine->lCurrLineAsc > 0)
      {
        pElem = pLine->pElemList;
        while (pElem)
        {
          bRet = true_a;
          if (lTable == -1)
          {
            pTBSpace = PDFGetTBSpace(this, pElem->lSpace);
            if (pTBSpace && eFixTableWidth == pTBSpace->eSpaceType)
              lTable = pElem->lSpace;
            else
              return false_a;
          }
          else
          {
            if (pElem->lSpace != lTable)
              return false_a;
          }
          pElem = pElem->pLCNext;
        }
      }
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return bRet;
}

void CPDFTextBox::SetCanPageBreak()
{
  PDFAddCanPageBreak(GetBaseDoc(), this);
}

void CPDFTextBox::SetCanPageBreak(bool_a i_bDelete)
{
  PDFCanPageBreak(GetBaseDoc(), this, i_bDelete);
}

bool_a CPDFTextBox::HasBreak()
{
  if (HasPageBreak())
    return true_a;

  PDFParagraph      *pPara;

  pPara = this->pParagraph;
  while (pPara)
  {
    if (pPara->m_bParagraphPageBreak
        && pPara != this->pParagraph)//first paragraph -> no break
      return true_a;
    pPara = pPara->pParaNext;
  }

  return false_a;
}

bool_a CPDFTextBox::HasPageBreak()
{
  if (this->bTBBreak || this->bColumnBreak)
    return true_a;
  return false_a;
}

bool_a CPDFTextBox::HasCanPageBreak()
{
  return this->bTBCanPageBreak;
}

bool_a CPDFTextBox::InsertColumnBreak()
{
  return PDFAddColumnBreak(GetBaseDoc(), this);
}

bool_a CPDFTextBox::InsertBreak()
{
  return PDFAddBreak(GetBaseDoc(), this);
}

bool_a CPDFTextBox::InsertBreak(CPDFTextBox *i_pTextBox)
{
  return PDFInsertBreak(GetBaseDoc(), this, i_pTextBox, NULL);
}

bool_a CPDFTextBox::CopyOverflowLines()
{
  //long lHeight, lWidth;
  CPDFTextBox *pTB = this;
  while (pTB)
  {
    if (pTB->GetNextTextBox())
    {
      //if (!pTB->GetSize(lWidth, lHeight))
      //  return false_a;

      if (!pTB->CopyOverflowLines(pTB->GetNextTextBox()))
        return false_a;
      pTB->GetNextTextBox()->CheckResize();
    }
    pTB = pTB->GetNextTextBox();
  }
  return true_a;
}

bool_a CPDFTextBox::CopyOverflowLines(CPDFTextBox *i_pTextBox, bool_a i_bCheckBreak, long i_lHeight)
{
  bool_a bRet = PDFCopyOverflowLines(GetBaseDoc(), this, i_pTextBox, i_bCheckBreak, i_lHeight);
  if (!bRet)
    return bRet;
  i_pTextBox->CheckResize();
  return bRet;
}

bool_a CPDFTextBox::Overlap(CPDFTextBox *i_pTextBox)
{
  float fL, fT, fB, fR;
  rec_a recPS = this->GetPosSize();
  rec_a recPS1 = i_pTextBox->GetPosSize();

  if (!this->GetBorder(fL, fT, fB, fR))
    return false_a;
  recPS.lLeft -= PDFRound(fL / 2.f);
  recPS.lTop -= PDFRound(fT / 2.f);
  recPS.lBottom += PDFRound(fB / 2.f);
  recPS.lRight += PDFRound(fR / 2.f);

  if (!i_pTextBox->GetBorder(fL, fT, fB, fR))
    return false_a;
  recPS1.lLeft -= PDFRound(fL / 2.f);
  recPS1.lTop -= PDFRound(fT / 2.f);
  recPS1.lBottom += PDFRound(fB / 2.f);
  recPS1.lRight += PDFRound(fR / 2.f);

  if (recPS1.lLeft >= recPS.lLeft && recPS1.lRight <= recPS.lRight
      && recPS1.lBottom >= recPS.lBottom && recPS1.lTop <= recPS.lTop)
    return true_a;

  if (recPS.lLeft >= recPS1.lLeft && recPS.lRight <= recPS1.lRight
      && recPS.lBottom >= recPS1.lBottom && recPS.lTop <= recPS1.lTop)
    return true_a;


  if (recPS1.lLeft >= recPS.lRight)
    return false_a;
  if (recPS1.lRight <= recPS.lLeft)
    return false_a;
  if (recPS1.lTop <= recPS.lBottom)
    return false_a;
  if (recPS1.lBottom >= recPS.lTop)
    return false_a;

/*
  if (recPS1.lLeft > recPS.lLeft && recPS1.lLeft < recPS.lRight &&
      recPS1.lTop > recPS.lBottom && recPS1.lTop < recPS.lTop)
  {
    return true_a;
  }
  if (recPS1.lLeft > recPS.lLeft && recPS1.lLeft < recPS.lRight &&
      recPS1.lBottom > recPS.lBottom && recPS1.lBottom < recPS.lTop)
  {
    return true_a;
  }
  if (recPS1.lRight > recPS.lLeft && recPS1.lRight < recPS.lRight &&
      recPS1.lTop > recPS.lBottom && recPS1.lTop < recPS.lTop)
  {
    return true_a;
  }
  if (recPS1.lRight > recPS.lLeft && recPS1.lRight < recPS.lRight &&
      recPS1.lBottom > recPS.lBottom && recPS1.lBottom < recPS.lTop)
  {
    return true_a;
  }
*/
  return true_a;
}

bool_a CPDFTextBox::Overflow()
{
  long lHeight, lWidth;
  if (!this->GetSize(lWidth, lHeight))
    return false_a;
  if (lHeight > GetDrawHeight())
    return true_a;
  return false_a;
}

bool_a CPDFTextBox::SetAlignment(short i_sAlignment)
{
  return PDFSetParaAlignment(this, i_sAlignment);
}

bool_a CPDFTextBox::SetKeepLinesTogether(bool_a i_bKeepLinesTogether)
{
  if (!(this->pActiveParagraph))
    return false_a;
  this->pActiveParagraph->m_bKeepLinesTogether = i_bKeepLinesTogether;
  return true_a;
}

bool_a CPDFTextBox::SetKeepWithNext(bool_a i_bKeepWithNext)
{
  if (!(this->pActiveParagraph))
    return false_a;
  this->pActiveParagraph->m_bKeepWithNext = i_bKeepWithNext;
  return true_a;
}

bool_a CPDFTextBox::SetWidowOrphan(bool_a i_bWidowOrphon)
{
  if (!(this->pActiveParagraph))
    return false_a;
  this->pActiveParagraph->m_bWidowOrphan = i_bWidowOrphon;
  return true_a;
}

bool_a CPDFTextBox::SetParagraphPageBreak(bool_a i_bParagraphPageBreak)
{
  if (!(this->pActiveParagraph))
    return false_a;
  this->pActiveParagraph->m_bParagraphPageBreak = i_bParagraphPageBreak;
  return true_a;
}

bool_a CPDFTextBox::SetVerticalAlignment(short i_sVAlignment)
{
  return PDFSetVerticalAlignment(this, i_sVAlignment);
}

bool_a CPDFTextBox::GetSize(long &o_lWidth, long &o_lHeight)
{
  bool_a bRet = PDFGetTextBoxActualSize(this, &o_lWidth, &o_lHeight);
  return bRet;
}

bool_a CPDFTextBox::SetLineSpace(long i_lLineSpace)
{
  bool_a bRet = false_a;
  bRet = PDFSetParaLineSpace(GetBaseDoc(), this, i_lLineSpace);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::SetTextDirection(eTEXTDIRECTION i_eDirection)
{
  bool_a bRet = false_a;
  bRet = PDFSetTextDirection(GetBaseDoc(), this, i_eDirection);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

eTEXTDIRECTION CPDFTextBox::GetTextDirection()
{
  return m_eTextDirection;
}


bool_a CPDFTextBox::SetBorder(long i_lLeft, long i_lTop, long i_lRight, long i_lBottom)
{
  bool_a bRet = false_a;
  bRet = SetBorder((float)i_lLeft, (float)i_lTop, (float)i_lRight, (float)i_lBottom);
  return bRet;
}

bool_a CPDFTextBox::SetBorder(float i_fLeft, float i_fTop, float i_fRight, float i_fBottom)
{
  bool_a bRet = false_a;
  bRet = PDFSetTextBoxBorder(GetBaseDoc(), this, i_fLeft, i_fTop, i_fRight, i_fBottom);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::GetBorder(float &o_fLeft, float &o_fTop, float &o_fRight, float &o_fBottom)
{
  return PDFGetTextBoxBorder(this, &o_fLeft, &o_fTop, &o_fRight, &o_fBottom);
}

bool_a CPDFTextBox::GetBorder(long &o_lLeft, long &o_lTop, long &o_lRight, long &o_lBottom)
{
  bool_a bRet;
  float fLeft, fTop, fRight, fBottom;
  bRet = PDFGetTextBoxBorder(this, &fLeft, &fTop, &fRight, &fBottom);
  o_lLeft = PDFRound(fLeft);
  o_lTop = PDFRound(fTop);
  o_lRight = PDFRound(fRight);
  o_lBottom = PDFRound(fBottom);
  return bRet;
}

bool_a CPDFTextBox::SetMargins(long i_lLeft, long i_lTop, long i_lRight, long i_lBottom)
{
  bool_a bRet = false_a;
  bRet = PDFSetTextBoxMargins(GetBaseDoc(), this, i_lLeft, i_lTop, i_lRight, i_lBottom);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::GetMargins(long &o_lLeft, long &o_lTop, long &o_lRight, long &o_lBottom)
{
  return PDFGetTextBoxMargins(this, &o_lLeft, &o_lTop, &o_lRight, &o_lBottom);
}

bool_a CPDFTextBox::SetIndents(long i_lLeft, long i_lTop, long i_lRight,
                                long i_lBottom, long i_lFirstIndent)
{
  bool_a bRet = false_a;
  bRet = PDFSetParaIndents(GetBaseDoc(), this, i_lLeft, i_lTop, i_lRight, i_lBottom, i_lFirstIndent);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::SetLeftRightIndents(long i_lLeft, long i_lRight)
{
  bool_a bRet = false_a;
  long lLeft, lRight, lTop, lBottom, lFirst;
  bRet = GetIndents(lLeft, lTop, lRight, lBottom, lFirst);
  if (!bRet)
    return bRet;
  bRet = SetIndents(i_lLeft, lTop, i_lRight, lBottom, lFirst);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::SetLeftRightIndents(long i_lLeft, long i_lRight, long i_lFirstIndent)
{
  bool_a bRet = false_a;
  long lLeft, lRight, lTop, lBottom, lFirst;
  bRet = GetIndents(lLeft, lTop, lRight, lBottom, lFirst);
  if (!bRet)
    return bRet;
  bRet = SetIndents(i_lLeft, lTop, i_lRight, lBottom, i_lFirstIndent);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::SetTopBottomIndents(long i_lTop, long i_lBottom)
{
  bool_a bRet = false_a;
  long lLeft, lRight, lTop, lBottom, lFirst;
  bRet = GetIndents(lLeft, lTop, lRight, lBottom, lFirst);
  if (!bRet)
    return bRet;
  bRet = SetIndents(lLeft, i_lTop, lRight, i_lBottom, lFirst);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::GetIndents(long &o_lLeft, long &o_lTop, long &o_lRight,
                                long &o_lBottom, long &o_lFirstIndent)
{
  return PDFGetParaIndents(this, &o_lLeft, &o_lTop, &o_lRight, &o_lBottom, &o_lFirstIndent);
}

bool_a CPDFTextBox::SetTabs(PDFTabStop *i_pTabs, short i_sTabsCount)
{
  bool_a bRet = false_a;
  bRet = PDFSetTextBoxTabs(GetBaseDoc(), this, i_pTabs, i_sTabsCount);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::InsertLine()
{
  bool_a bRet = false_a;
  bRet = PDFInsertLine(GetBaseDoc(), this);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::InsertTab(PDFTabStop i_Tab)
{
  bool_a bRet = false_a;
  bRet = PDFInsertTextBoxTab(GetBaseDoc(), this, i_Tab);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::SetTabDefault(long i_lTabSize, char i_cTabDec)
{
  return PDFSetTabDefault(GetBaseDoc(), this, i_lTabSize, i_cTabDec);
}

bool_a CPDFTextBox::HasTransparentBg()
{
  return PDFHasTextBoxTransparentBg(this);
}

bool_a CPDFTextBox::SetTransparentBg(bool_a i_bTransparent)
{
  return PDFSetTextBoxTransparentBg(this, i_bTransparent);
}

CPDFColor& CPDFTextBox::GetBgColor()
{
  return m_TBBgColor;
}

bool_a CPDFTextBox::SetBgColor(float i_fRed, float i_fGreen, float i_fBlue)
{
  GetBgColor().SetColor(i_fRed, i_fGreen, i_fBlue);
  return true_a;
}

bool_a CPDFTextBox::SetBgImage(const char *i_pczImageName)
{
  return PDFSetTextBoxBgImage(GetBaseDoc(), this, i_pczImageName);
}

bool_a CPDFTextBox::ClearBgImage()
{
  return PDFClearTextBoxBgImage(this);
}

CPDFColor& CPDFTextBox::GetTextColor()
{
  return m_Color;
}

bool_a CPDFTextBox::SetTextColor(float i_fRed, float i_fGreen, float i_fBlue)
{
  GetTextColor().SetColor(i_fRed, i_fGreen, i_fBlue);
  return true_a;
}

CPDFColor& CPDFTextBox::GetTextBgColor()
{
  return m_BgColor;
}

bool_a CPDFTextBox::SetTextBgColor(float i_fRed, float i_fGreen, float i_fBlue)
{
  GetTextBgColor().SetColor(i_fRed, i_fGreen, i_fBlue);
  return true_a;
}

bool_a CPDFTextBox::DeleteLastParagraph()
{
  bool_a bRet =false_a;
  bRet = PDFDeleteLastParagraph(GetBaseDoc(), this);
  if (bRet)
    bRet = CheckResize();
  return bRet;
}

bool_a CPDFTextBox::Flush()
{
  vector<long> vectorSpc;
  //CPDFTextBox *pTextBox = this;
  //PDFTextBox *pTB = (PDFTextBox *)pTextBox;
  long lTemp = this->lPage + 1;
  if (m_bFlush)
    return false_a;

  long lTmpTestHeight = CalcParagraphsHeights(this->pParagraph);
  if (lTmpTestHeight != this->lTBHeight)
    lTmpTestHeight = lTmpTestHeight;

  if (!PDFAdjustParagraph(GetBaseDoc(), this, GetTopDrawPos(), false_a))
    return false_a;

  if (!ReplaceBookmarks())
    return false_a;

  PDFTBSpace  *pTBSpace = NULL;
  pTBSpace = this->pTBSpaces;
  while (pTBSpace)
  {
    if (eFixTableWidth == pTBSpace->eSpaceType)
      vectorSpc.push_back(pTBSpace->lSpaceID);
    pTBSpace = pTBSpace->pTBSpaceNext;
  }

  PDFSetCurrentPage(GetBaseDoc(), this->lPage + 1);
  PDFSaveGraphicsState(GetBaseDoc());
//  PDFSetClippingRect(GetBaseDoc(), this->recTB);

  if (!FlushBackground())
    return false_a;

#ifdef __SHOW_TB_BORDER__
  if (!ShowBorder())
    return false_a;
#endif

  MakeDirection();

  if (!FlushImages())
    return false_a;

  if (!FlushElements())
    return false_a;

  if (vectorSpc.size())
  {
    CPDFTable *pTable;
    for (size_t i = 0; i < vectorSpc.size(); i++)
    {
      pTable = (CPDFTable *)(long)vectorSpc[i];
      if (pTable)
      {
        if (!pTable->IsDivided()
              || (pTable->IsDivided() && (pTable->GetPageNumber() + pTable->GetDivideCount()) == lTemp))
        {
          if (!pTable->Flush())
            return false_a;
        }
      }
    }
  }

  PDFSetCurrentPage(GetBaseDoc(), this->lPage + 1);
  PDFRestoreGraphicsState(GetBaseDoc());


  m_bFlush = true_a;
  return true_a;
}

bool_a CPDFTextBox::IsFlush()
{
  return m_bFlush;
}

bool_a CPDFTextBox::FlushElements()
{
  PDFParagraph    *pParagraph   = NULL;
  PDFLine         *pLine        = NULL;
  long            lX            = 0;
  long            lY            = 0;
  long            lWidth        = 0;
  long            lAddWidth     = 0;
  PDFLineElemList *pLineElem    = NULL;
  short           sCurrBlock    = 0;
  short           sBlockCount   = 0;
  rec_a           **ppRec       = NULL;
  long            lAddY         = 0;
  long            lAnnot        = NOT_USED;

  lAddY = CalcAlignment();

  pParagraph = this->pParagraph;
  if (!pParagraph)
    return false_a;

  ppRec = NULL;
  lY = GetTopDrawPos();
  PDFBgnText(GetBaseDoc(), 0);

  while (pParagraph)
  {
    pLine = pParagraph->pPDFLine;
    lY -= pParagraph->recIndents.lTop;
    lWidth = GetDrawWidthInd(pParagraph, true_a);
#ifdef __SHOW_PARA__
    if (!ShowParagraph(pParagraph, lY))
      return false_a;
#endif
    pLineElem = NULL;

    if (ppRec)
      PDFFreeTBRanges(&ppRec);
    ppRec = PDFGetTBRanges(lY, lY - pLine->lCurrLineHeight, this, &sBlockCount);

    while (pLine)
    {
      if (ppRec)
      {
        if (sCurrBlock >= sBlockCount)
        {
          lAddWidth = ppRec[sBlockCount - 1]->lRight - GetLeftDrawPosInd(pParagraph, true_a);
        }
        else
        {
          if (sCurrBlock == 0)
          {
            lAddWidth = GetRightDrawPosInd(pParagraph, true_a) - ppRec[sCurrBlock]->lLeft;
          }
          else
          {
            lAddWidth = GetRightDrawPosInd(pParagraph, true_a) - ppRec[sCurrBlock]->lLeft;
            lAddWidth += ppRec[sCurrBlock - 1]->lRight - GetLeftDrawPosInd(pParagraph, true_a);
          }
        }
      }
      else
        lAddWidth = 0;

      if (!pLineElem)
        lY -= (pLine->lCurrLineHeight + pLine->lCurrLineDesc/* - pParagraph->lLineSpace*/);

#ifdef __SHOW_TAB__
      if (!ShowTab(pParagraph, lY))
        return false_a;
#endif

      switch(pParagraph->sAlignment)
      {
        default:
        case TF_JUSTIFY:
          {
            if (pLine->pLineNext && 0 < pLine->pLineNext->lCurrLineWidth)
            {
              long lTBWidth = GetDrawWidthInd(pParagraph, true_a);
              long lSpaceCount;
              if (pLineElem)
                lSpaceCount = PDFGetCountSpaces(pLineElem);
              else
                lSpaceCount = PDFGetCountSpaces(pLine->pElemList);
              if (pLine == pParagraph->pPDFLine)
                lTBWidth -= pParagraph->lIndent;
              if (lSpaceCount > 0)
              {
                if (pLineElem)
                  lTBWidth -= GetElemListWidth(pParagraph, pLineElem);
                else
                  lTBWidth -= GetElemListWidth(pParagraph, pLine->pElemList);
                lTBWidth -= lAddWidth;
                if (lTBWidth < 0)
                  PDFWordSpacing(GetBaseDoc(), 0);
                else
                  PDFWordSpacing(GetBaseDoc(), (float)(lTBWidth) / lSpaceCount);
              }
            }
            else
              PDFWordSpacing(GetBaseDoc(), 0);
          }
        case TF_LEFT:
          lX = PDFGetStartPos(this, pParagraph, pLine, pLineElem);
          if (ppRec && sCurrBlock > 0)
            if (sCurrBlock > sBlockCount)
              lX += ppRec[sBlockCount - 1]->lRight - pParagraph->recIndents.lLeft - GetLeftDrawPos(true_a);
            else
              lX += ppRec[sCurrBlock - 1]->lRight - pParagraph->recIndents.lLeft - GetLeftDrawPos(true_a);
          break;
        case TF_CENTER:
          if (pLineElem)
            lX = GetLeftDrawPos(true_a) + ((lWidth - lAddWidth - (GetElemListWidth(pParagraph, pLineElem))) / 2);
          else
            lX = GetLeftDrawPos(true_a) + ((lWidth - lAddWidth - (GetElemListWidth(pParagraph, pLine->pElemList))) / 2);

          if (pLine == pParagraph->pPDFLine)
            lX += (pParagraph->lIndent / 2);

          if (ppRec && sCurrBlock > 0)
            if (sCurrBlock > sBlockCount)
              lX += ppRec[sBlockCount - 1]->lRight - pParagraph->recIndents.lLeft - GetLeftDrawPos(true_a);
            else
              lX += ppRec[sCurrBlock - 1]->lRight - pParagraph->recIndents.lLeft - GetLeftDrawPos(true_a);

          lX += pParagraph->recIndents.lLeft;
          break;
        case TF_RIGHT:
          if (pLineElem)
            lX = GetLeftDrawPos(true_a) + (lWidth - lAddWidth - GetElemListWidth(pParagraph, pLineElem));
          else
            lX = GetLeftDrawPos(true_a) + (lWidth - lAddWidth - GetElemListWidth(pParagraph, pLine->pElemList));

          if (ppRec && sCurrBlock > 0)
            if (sCurrBlock > sBlockCount)
              lX += ppRec[sBlockCount - 1]->lRight - pParagraph->recIndents.lLeft - GetLeftDrawPos(true_a);
            else
              lX += ppRec[sCurrBlock - 1]->lRight - pParagraph->recIndents.lLeft - GetLeftDrawPos(true_a);

          lX += pParagraph->recIndents.lLeft;
          break;
      }
      if (pLine->pElemList && (lY >= GetBottomDrawPos(true_a) || pLine->pElemList->lImage >= 0))
      {
        if (pLineElem)
          pLineElem = PDFFlushStringToFile(GetBaseDoc(), pLineElem, lX, lY - lAddY,
                                            (pParagraph->pPDFLine == pLine) ? pParagraph->lIndent : 0,
                                            this, pParagraph, pLine, &lAnnot);
        else
          pLineElem = PDFFlushStringToFile(GetBaseDoc(), pLine->pElemList, lX, lY - lAddY,
                                            (pParagraph->pPDFLine == pLine) ? pParagraph->lIndent : 0,
                                            this, pParagraph, pLine, &lAnnot);

        if (pLineElem)
        {
          sCurrBlock++;
          continue;
        }
      }
      lY += pLine->lCurrLineDesc;
      //lY -= pParagraph->lLineSpace;

      if (ppRec)
        PDFFreeTBRanges(&ppRec);
      ppRec = PDFGetTBRanges(lY, lY - pLine->lCurrLineHeight, this, &sBlockCount);
      sCurrBlock = 0;

      pLine = pLine->pLineNext;
    }

    if (pParagraph->sAlignment == TF_JUSTIFY)
      PDFWordSpacing(GetBaseDoc(), 0);

//    lY += pParagraph->lLineSpace;
    lY -= pParagraph->recIndents.lBottom;
    pParagraph = pParagraph->pParaNext;
  }
  PDFEndText(GetBaseDoc());

  if (ppRec)
    PDFFreeTBRanges(&ppRec);

  return true_a;
}

long CPDFTextBox::CalcAlignment()
{
  long lRet;

  switch (this->sVAlignment)
  {
  case TF_MIDDLE:
    lRet = GetDrawHeight(true_a) - this->lTBHeight;
    lRet /= 2;
    break;
  case TF_DOWN:
    lRet = GetDrawHeight(true_a) - this->lTBHeight;
    break;
  default:
  case TF_UP:
    lRet = 0;
    break;
  }
  if (lRet < 0)
    lRet = 0;

  return lRet;
}

void CPDFTextBox::MakeDirection()
{
  switch (this->m_eTextDirection)
  {
  case eVerticalUp:
    PDFrotate(GetBaseDoc(), 90);
    PDFTranslateL(GetBaseDoc(), (this->recTB.lBottom - this->recTB.lLeft),
                    -this->recTB.lLeft - this->recTB.lTop);
    break;
  case eVerticalDown:
    PDFrotate(GetBaseDoc(), -90);
    PDFTranslateL(GetBaseDoc(), -this->recTB.lLeft - this->recTB.lTop,
                    this->recTB.lRight - this->recTB.lTop);
    break;
  }
}

void CPDFTextBox::MakeDirection(rec_a &rec)
{
  rec_a recTemp;
  switch (this->m_eTextDirection)
  {
  case eVerticalUp:
    //rotate
    recTemp.lLeft = rec.lBottom;
    recTemp.lBottom = -rec.lRight;
    recTemp.lRight = rec.lTop;
    recTemp.lTop = -rec.lLeft;
    //translate
    recTemp.lLeft += recTB.lLeft - recTB.lBottom;
    recTemp.lBottom += recTB.lLeft + recTB.lTop;
    recTemp.lRight += recTB.lLeft - recTB.lBottom;
    recTemp.lTop += recTB.lLeft + recTB.lTop;

    rec = recTemp;
    break;
  case eVerticalDown:
    //rotate
    recTemp.lLeft = -rec.lTop;
    recTemp.lTop = rec.lRight;
    recTemp.lRight = -rec.lBottom;
    recTemp.lBottom = rec.lLeft;
    //translate
    recTemp.lLeft += recTB.lTop + recTB.lLeft;
    recTemp.lBottom += recTB.lTop - recTB.lRight;
    recTemp.lRight += recTB.lTop + recTB.lLeft;
    recTemp.lTop += recTB.lTop - recTB.lRight;

    rec = recTemp;
    break;
  }
}

bool_a CPDFTextBox::FlushBackground()
{
  CPDFImage *pImage = NULL;
  char czTemp[201] = {0};

  if (!this->bTransparent)
  {
    if (NOT_USED == this->lTBBgImage)
    {
      PDFSetLineWidth(GetBaseDoc(), 0);
      m_pDoc->SetTextColor(m_TBBgColor);
      m_pDoc->SetLineColor(m_TBBgColor);
      PDFRectangleL(GetBaseDoc(), this->recTB, true_a);
    }
    else
    {
      pImage = m_pDoc->GetImage(this->lTBBgImage);
      PDFSaveGraphicsState(GetBaseDoc());
      PDFTranslateL(GetBaseDoc(), this->recTB.lLeft, this->recTB.lBottom);
      PDFrotate(GetBaseDoc(), 0);
      PDFConcatL(GetBaseDoc(), PDFConvertDimensionToDefaultL((this->recTB.lRight - this->recTB.lLeft)),
                  0, 0, PDFConvertDimensionToDefaultL((this->recTB.lTop - this->recTB.lBottom)), 0, 0);
      sprintf(czTemp, "/%s %s\n", pImage->name, czOPERATORDO);
      PDFWriteToPageStream(GetBaseDoc(), czTemp, strlen(czTemp), GetBaseDoc()->m_lActualPage);
      PDFRestoreGraphicsState(GetBaseDoc());
    }
  }

  return true_a;
}


bool_a CPDFTextBox::ShowBorder()
{
  PDFSaveGraphicsState(GetBaseDoc());

  m_pDoc->SetLineColor(1.f, 0.f, 0.f);
  PDFSetLineWidth(GetBaseDoc(), 20);
  PDFSetDashedLine(GetBaseDoc());
  PDFMoveToL(GetBaseDoc(), this->recTB.lLeft, this->recTB.lTop);
  PDFLineToL(GetBaseDoc(), this->recTB.lRight, this->recTB.lTop);

  PDFMoveToL(GetBaseDoc(), this->recTB.lRight, this->recTB.lTop);
  PDFLineToL(GetBaseDoc(), this->recTB.lRight, this->recTB.lBottom);

  PDFMoveToL(GetBaseDoc(), this->recTB.lRight, this->recTB.lBottom);
  PDFLineToL(GetBaseDoc(), this->recTB.lLeft, this->recTB.lBottom);

  PDFMoveToL(GetBaseDoc(), this->recTB.lLeft, this->recTB.lBottom);
  PDFLineToL(GetBaseDoc(), this->recTB.lLeft, this->recTB.lTop);

  PDFRestoreGraphicsState(GetBaseDoc());

  return true_a;
}

bool_a CPDFTextBox::FlushImages()
{
  PDFTBImage *pTBImage = NULL;
  CPDFImage *pImage = NULL;
  char czTemp[201] = {0};


  pTBImage = this->pTBImages;
  while (pTBImage)
  {
    PDFAddImageToPage(GetBaseDoc(), pTBImage->lImage, GetBaseDoc()->m_lActualPage);

    pImage = m_pDoc->GetImage(pTBImage->lImage);

    PDFSaveGraphicsState(GetBaseDoc());

    PDFTranslateL(GetBaseDoc(), pTBImage->recPos.lLeft, pTBImage->recPos.lBottom/* - lAddY*/);

    PDFrotate(GetBaseDoc(), 0);

    PDFConcatL(GetBaseDoc(), PDFConvertDimensionToDefaultL((pTBImage->recPos.lRight - pTBImage->recPos.lLeft)),
              0, 0, PDFConvertDimensionToDefaultL((pTBImage->recPos.lTop - pTBImage->recPos.lBottom)), 0, 0);

    sprintf(czTemp, "/%s %s\n", pImage->name, czOPERATORDO);
    PDFWriteToPageStream(GetBaseDoc(), czTemp, strlen(czTemp), GetBaseDoc()->m_lActualPage);
    PDFRestoreGraphicsState(GetBaseDoc());

    //m_pDoc->SetTextColor(0.f, 1.f, 0.f); //TODO: ???
    //m_pDoc->SetLineColor(0.f, 1.f, 0.f); //TODO: ???
    //m_pDoc->DrawRectangle(pTBImage->recPos, false_a); //TODO: ???

    pTBImage = pTBImage->pTBImageNext;
  }

  return true_a;
}

bool_a CPDFTextBox::ShowParagraph(PDFParagraph *i_pParagraph, long i_lY)
{
  if (!i_pParagraph)
    return false_a;

  //if (i_pParagraph->pPDFLine == i_pParagraph->pPDFLastLine && i_pParagraph->pPDFLine->lCurrLineWidth == 0)
  {
    PDFEndText(GetBaseDoc());
    m_pDoc->SetLineColor(0, 0, 0);
    PDFSetLineWidth(GetBaseDoc(), 0.5);
    PDFMoveToL(GetBaseDoc(), GetLeftDrawPos(true_a), i_lY);
    PDFLineToL(GetBaseDoc(), GetLeftDrawPos(true_a) + 50, i_lY - i_pParagraph->pPDFLine->lCurrLineHeight);
    PDFBgnText(GetBaseDoc(), 0);
  }
  return true_a;
}

bool_a CPDFTextBox::ShowTab(PDFParagraph *i_pParagraph, long i_lY)
{
  if (!i_pParagraph)
    return false_a;

  if (i_pParagraph->pParaTabStops)
  {
    long iii, lStartPos = GetLeftDrawPos(true_a);
    PDFEndText(GetBaseDoc());
    PDFSetLineWidth(GetBaseDoc(), 0.5);
    m_pDoc->SetLineColor(0, 0.5, 0);
    for (iii = 0; iii < i_pParagraph->sParaTabsCount; iii++)
    {
      PDFMoveToL(GetBaseDoc(), lStartPos + i_pParagraph->pParaTabStops[iii].lTabPos, i_lY);
      PDFLineToL(GetBaseDoc(), lStartPos + i_pParagraph->pParaTabStops[iii].lTabPos,
                  i_lY - i_pParagraph->pPDFLine->lCurrLineHeight);
    }
    PDFBgnText(GetBaseDoc(), 0);
  }
  if (i_pParagraph->lParaTabSize > 0)
  {
    long iii, lStartPos = GetLeftDrawPos(true_a);
    PDFEndText(GetBaseDoc());
    PDFSetLineWidth(GetBaseDoc(), 0.5);
    m_pDoc->SetLineColor(0.5, 0.5, 0);
    for (iii = 0; iii < 100; iii++)
    {
      PDFMoveToL(GetBaseDoc(), lStartPos + iii * i_pParagraph->lParaTabSize, i_lY);
      PDFLineToL(GetBaseDoc(), lStartPos + iii * i_pParagraph->lParaTabSize,
                  i_lY - i_pParagraph->pPDFLine->lCurrLineHeight);
    }
    PDFBgnText(GetBaseDoc(), 0);
  }

  return true_a;
}

bool_a CPDFTextBox::IsEmpty()
{
  if (this->pParagraph == this->pActiveParagraph
    && this->pParagraph->pPDFLine == this->pParagraph->pPDFLastLine
    && this->pParagraph->pPDFLine->lCurrLineWidth == 0)
    return true_a;

  return false_a;
}

long CPDFTextBox::GetLineCount()
{
  PDFParagraph      *pPara;
  PDFLine           *pLine;
  long lRet = 0;

  pPara = this->pParagraph;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      lRet++;
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return lRet;
}

CPDFTextBox *CPDFTextBox::GetNextTextBox()
{
  return this->pTBNext;
}

void CPDFTextBox::SetNextTextBox(CPDFTextBox *i_pTextBox)
{
  this->pTBNext = i_pTextBox;
}

CPDFTextBox *CPDFTextBox::GetPrevTextBox()
{
  return this->pTBPrev;
}

void CPDFTextBox::SetPrevTextBox(CPDFTextBox *i_pTextBox)
{
  this->pTBPrev = i_pTextBox;
}

long CPDFTextBox::GetPageNumber()
{
  return lPage + 1;
}

rec_a CPDFTextBox::GetPosSize()
{
  return this->recTB;
}

bool_a CPDFTextBox::SetPosSize(rec_a i_recPosSize)
{
  bool_a bRet = false_a;

  long lTop, lWidthOrig, lWidthNew;
  lWidthOrig = GetDrawWidth();
  lWidthNew = i_recPosSize.lRight - i_recPosSize.lLeft;
  COPY_REC(this->recTB, i_recPosSize);
  lTop = GetTopDrawPos();
//  if (lWidthOrig != lWidthNew)
  bRet = PDFAdjustParagraph(GetBaseDoc(), this, lTop, false_a);

  if (bRet)
    bRet = CheckResize();
  return bRet;
}

long CPDFTextBox::GetBottomDrawPos(bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lBottom + PDFRound(this->recBorder.fBottom )+ this->recMargins.lBottom;
  return lRet;
}

long CPDFTextBox::GetTopDrawPos(bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lTop - PDFRound(this->recBorder.fTop) - this->recMargins.lTop;
  return lRet;
}

long CPDFTextBox::GetLeftDrawPos(bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lLeft + PDFRound(this->recBorder.fLeft) + this->recMargins.lLeft;
  return lRet;
}

long CPDFTextBox::GetRightDrawPos(bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lRight - PDFRound(this->recBorder.fRight) - this->recMargins.lRight;
  return lRet;
}

rec_a CPDFTextBox::GetDrawPosSize()
{
  rec_a rec;
  rec.lLeft = GetLeftDrawPos();
  rec.lRight = GetRightDrawPos();
  rec.lTop = GetTopDrawPos();
  rec.lBottom = GetBottomDrawPos();
  return rec;
}

void CPDFTextBox::GetActualPos(long &i_lLeft, long &i_lTop)
{
  rec_a recPos = GetPosSize();
  GetSize(i_lLeft, i_lTop);
  if (pActiveParagraph && pActiveParagraph->pPDFLastLine)
  {
    i_lLeft = recPos.lLeft + pActiveParagraph->pPDFLastLine->lCurrLineWidth;
    i_lTop = (recPos.lTop - i_lTop) + pActiveParagraph->pPDFLastLine->lCurrLineHeight;
  }
}

void CPDFTextBox::SetPage(long i_lPage)
{
  lPage = i_lPage - 1;
}

bool_a CPDFTextBox::ChangeFontID(map <long, long> *i_pFontMap)
{
  PDFParagraph      *pPara;
  PDFLine           *pLine;
  PDFLineElemList   *pElem;
  map <long, long>::iterator  FontIter;

  pPara = this->pParagraph;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      pElem = pLine->pElemList;
      while (pElem)
      {
        FontIter = i_pFontMap->find(pElem->lFont);
        if (FontIter != i_pFontMap->end())
          pElem->lFont = (*FontIter).second;
        else
          return false_a;
        pElem = pElem->pLCNext;
      }
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return true_a;
}

bool_a CPDFTextBox::ChangeImageID(map <long, long> *i_pImageMap)
{
  map <long, long>::iterator  ImageIter;
  PDFTBImage        *pImages;
  PDFParagraph      *pPara;
  PDFLine           *pLine;
  PDFLineElemList   *pElem;
  map <long, long>::iterator  FontIter;

  pImages = pTBImages;
  while (pImages)
  {
    ImageIter = i_pImageMap->find(pImages->lImage);
    if (ImageIter != i_pImageMap->end())
      pImages->lImage = (*ImageIter).second;
    else
      return false_a;
    pImages = pImages->pTBImageNext;
  }

  pPara = this->pParagraph;
  while (pPara)
  {
    pLine = pPara->pPDFLine;
    while (pLine)
    {
      pElem = pLine->pElemList;
      while (pElem)
      {
        if (pElem->lImage >= 0)
        {
          ImageIter = i_pImageMap->find(pElem->lImage);
          if (FontIter != i_pImageMap->end())
            pElem->lImage = (*FontIter).second;
          else
            return false_a;
        }
        pElem = pElem->pLCNext;
      }
      pLine = pLine->pLineNext;
    }
    pPara = pPara->pParaNext;
  }
  return true_a;
}

long CPDFTextBox::GetDrawWidthInd(PDFParagraph *i_pParagraph, bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lRight - PDFRound(this->recBorder.fRight) - this->recMargins.lRight;
  lRet -= i_pParagraph->recIndents.lRight;
  lRet -= (recPosSize.lLeft + PDFRound(this->recBorder.fLeft) + this->recMargins.lLeft);
  lRet -= i_pParagraph->recIndents.lLeft;
  return lRet;
}

long CPDFTextBox::GetRightDrawPosInd(PDFParagraph *i_pParagraph, bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lRight - PDFRound(this->recBorder.fRight) - this->recMargins.lRight
            - this->recIndents.lRight;
  return lRet;
}

long CPDFTextBox::GetLeftDrawPosInd(PDFParagraph *i_pParagraph, bool_a i_bDirection)
{
  long lRet = 0;
  rec_a recPosSize = GetPosSize();
  if (i_bDirection)
    MakeDirection(recPosSize);
  lRet = recPosSize.lLeft + PDFRound(this->recBorder.fLeft) + this->recMargins.lLeft
            + i_pParagraph->recIndents.lLeft;
  return lRet;
}


//TODO: merge GetElemListWidth with PDFGetLineWidth
long CPDFTextBox::GetElemListWidth(PDFParagraph *i_pParagraph, PDFLineElemList *i_pElem)
{
  if (!i_pParagraph || !i_pElem)
    return 0;
  long            lWidth      = 0;
  long            lLastWidth  = 0;
  PDFLineElemList *pElemList  = i_pElem;
  long            lX          = 0;
  long            lRightPos;
  PDFTabStop      strTabStop;
  long lSpaceWidth = 0;
  CPDFBaseFont *pFont;

  lRightPos = GetRightDrawPosInd(i_pParagraph, true_a);
  while (pElemList)
  {
    pFont = m_pDoc->GetFont(pElemList->lFont);
    lWidth += pElemList->lElemWidth;
    if (pElemList->m_pStr)
    {
      if (pElemList->m_pStr->HasOnlyWhiteSigns())
        lSpaceWidth += pElemList->lElemWidth;
      else
        lSpaceWidth = pFont->GetFontLastSpaceWidth(*pElemList->m_pStr, pElemList->fFontSize);
    } else if (pElemList->lElemWidth > 0)
      lSpaceWidth = 0;
    if (pElemList->sTabStop >= 0)
    {
      lSpaceWidth = 0;
      lLastWidth = lX + lWidth;
      strTabStop = PDFGetTabStop(i_pParagraph->pParaTabStops, i_pParagraph->sParaTabsCount,
                                 pElemList->sTabStop, i_pParagraph->lParaTabSize);
      lX = strTabStop.lTabPos - i_pParagraph->recIndents.lLeft;
      if (i_pParagraph->pPDFLine == i_pParagraph->pPDFLastLine)
        lX -= i_pParagraph->lIndent;
      switch (strTabStop.sTabFlag)
      {
        case TAB_LEFT:
          break;
        case TAB_RIGHT:
            lWidth = PDFGetWidthAfterTab(pElemList, pElemList->sTabStop);
            lX -= lWidth;//PDFGetWidthAfterTab(pElemList, pElemList->sTabStop);
            if (lX < lLastWidth)
              lX = lLastWidth;
          break;
        case TAB_CENTER:
            lWidth = PDFGetWidthAfterTab(pElemList, pElemList->sTabStop) / 2;
            lX -= lWidth;//PDFGetWidthAfterTab(pElemList, pElemList->sTabStop) / 2;
            if ((lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) > lRightPos)
            {
              lX -= (lX + PDFGetWidthAfterTab(pElemList, pElemList->sTabStop)) - lRightPos;
            }
            else
              if (lLastWidth > lX)
                lX = lLastWidth;
          break;
        case TAB_DECIMAL:
            lWidth = PDFGetDecimalStopWidthAfterTab(GetBaseDoc(), pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType);
            lX -= lWidth;//PDFGetDecimalStopWidthAfterTab(GetBaseDoc(), pElemList, pElemList->sTabStop);
            if ((lX + PDFGetDecimalStopWidthAfterTab(GetBaseDoc(), pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                > lRightPos)
            {
              lX -= (lX + PDFGetDecimalStopWidthAfterTab(GetBaseDoc(), pElemList, pElemList->sTabStop, i_pParagraph->cParaTabDecimalType))
                      - lRightPos;
            }
            else
              if (lLastWidth > lX)
                lX = lLastWidth;
          break;
      }
      lWidth = 0;
    }
    if (pElemList->bBlockEnd)
      break;
    pElemList = pElemList->pLCNext;
  }
  lLastWidth = lX + lWidth;
  lLastWidth -= lSpaceWidth;
  return lLastWidth;
}



void CPDFTextBox::TBCpy(CPDFTextBox& tb)
{
  m_Color = tb.m_Color;
  m_BgColor = tb.m_BgColor;
  m_TBBgColor = tb.m_TBBgColor;
  bTransparent = tb.bTransparent;
  lTBBgImage = tb.lTBBgImage;
  recMargins = tb.recMargins;
  recIndents = tb.recIndents;
  lIndent = tb.lIndent;
  recBorder = tb.recBorder;
  sAlignment = tb.sAlignment;
  sVAlignment = tb.sVAlignment;
  m_eTextDirection = tb.m_eTextDirection;
  lPage = tb.lPage;
  recTB = tb.recTB;
  lLineSpace = tb.lLineSpace;
  lLetterSpace = tb.lLetterSpace;
  bTBCanPageBreak = tb.bTBCanPageBreak;
  bTBBreak = tb.bTBBreak;
  bColumnBreak = tb.bColumnBreak;
  bPageMark = tb.bPageMark;
  bTBCopy = tb.bTBCopy;
  lTBWidth = tb.lTBWidth;
  lTBHeight = tb.lTBHeight;
  lTabSize = tb.lTabSize;
  cTabDecimalType = tb.cTabDecimalType;
  pTBTabStops = tb.pTBTabStops;
  sTabsCount = tb.sTabsCount;
  pTBImages = tb.pTBImages;
  pTBSpaces = tb.pTBSpaces;
  pParagraph = tb.pParagraph;
  pActiveParagraph = tb.pActiveParagraph;
  pTBNext = tb.pTBNext;
  pTBPrev = tb.pTBPrev;
}
