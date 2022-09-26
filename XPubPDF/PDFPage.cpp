
#include "pdffnc.h"
#include "PDFPage.h"
#include "PDFDocument.h"
#include "PDFBaseFont.h"

CPDFPage::CPDFPage(CPDFDocument *i_pDoc, long i_lWidth, long i_lHeight,
                    long i_lPageNum, EPageType i_ePageType)
  :m_pDoc(i_pDoc)
{
  m_bFlush = false_a;
  lObjIndex = 0;
  lParent = 0;
  lContIndex = 0;
  bCompressed = false_a;
  lFontCount = 0;
  lImageCount = 0;
  lAnnotCount = 0;
  lCompLength = 0;
  pucCompStream = NULL;
  
  m_lColCount = 1;
  m_lColSpace = 0;

  recCropBox.lTop = recMediaBox.lTop = i_lHeight;
  recCropBox.lBottom = recMediaBox.lBottom = 0;
  recCropBox.lLeft = recMediaBox.lLeft = 0;
  recCropBox.lRight = recMediaBox.lRight = i_lWidth;
  lPageNum = i_lPageNum - 1;
  lRotation = i_ePageType;
  pPageStream = NULL;
  pImageIdx = NULL;
  pAnnotIdx = NULL;
  pFontIdx = (PDFList *)PDFMalloc(sizeof(PDFList));
  if (!pFontIdx)
    return;
  pFontIdx->lValue = -1;
  pFontIdx->pNext = NULL;
  pImageIdx = (PDFList *)PDFMalloc(sizeof(PDFList));
  if (!pImageIdx)
    return;
  pImageIdx->lValue = -1;
  pImageIdx->pNext = NULL;

  pAnnotIdx = (PDFList *)PDFMalloc(sizeof(PDFList));
  if (!pAnnotIdx)
    return;
  pAnnotIdx->lValue = -1;
  pAnnotIdx->pNext = NULL;

  pPageStream = PDFCreateStream();

  m_eActCtrl = eTextArea;
  m_pHeader = NULL;
  m_pFooter = NULL;

  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
  {
    m_pTextArea = new CPDFTextBox(m_pDoc, recMediaBox, i_lPageNum, true_a);
    return;
  }
  rec_a recTextArea;
  recTextArea.lLeft = pSect->GetLeftMargin();
  recTextArea.lRight = i_lWidth - pSect->GetRightMargin();
  recTextArea.lBottom = pSect->GetBottomMargin();
  recTextArea.lTop = i_lHeight - pSect->GetTopMargin();
  CPDFHeaderFooter *pHdrExemplar = pSect->GetHdrExemplar();
  if (pHdrExemplar)
  {
    m_pHeader = pHdrExemplar->Clone();
    m_pHeader->SetPage(i_lPageNum);
    if (recTextArea.lTop > m_pHeader->GetPosSize().lBottom)
      recTextArea.lTop = m_pHeader->GetPosSize().lBottom;
  }
  CPDFHeaderFooter *pFtrExemplar = pSect->GetFtrExemplar();
  if (pFtrExemplar)
  {
    m_pFooter = pFtrExemplar->Clone();
    m_pFooter->SetPage(i_lPageNum);
    if (recTextArea.lBottom < m_pFooter->GetPosSize().lTop)
      recTextArea.lBottom = m_pFooter->GetPosSize().lTop;
  }
  m_pTextArea = new CPDFTextBox(m_pDoc, recTextArea, i_lPageNum, true_a);

  SetColumns(pSect->GetColumns(), pSect->GetColumnSpace());
}

CPDFPage::~CPDFPage()
{
  if (m_pHeader)
    delete m_pHeader;
  if (m_pFooter)
    delete m_pFooter;
  CPDFTextBox *pTB = m_pTextArea;
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

  if(this->pPageStream)
  {
    PDFDeleteStream(&(this->pPageStream));
    this->pPageStream = NULL;
  }
  if(this->pucCompStream)
  {
    free(this->pucCompStream);
    this->pucCompStream = NULL;
  }
  PDFList *iL, *iLprev;
  iL = this->pFontIdx;
  while(iL != NULL)
  {
    iLprev = iL;
    iL = iL->pNext;
    free(iLprev);
  }
  iL = this->pImageIdx;
  while(iL != NULL)
  {
    iLprev = iL;
    iL = iL->pNext;
    free(iLprev);
  }
  iL = this->pAnnotIdx;
  while(iL != NULL)
  {
    iLprev = iL;
    iL = iL->pNext;
    free(iLprev);
  }

}

PDFDocPTR CPDFPage::GetBaseDoc()
{
  return (PDFDocPTR)m_pDoc;
}

rec_a CPDFPage::GetMediaBox()
{
  return recMediaBox;
}

rec_a CPDFPage::GetCropBox()
{
  return recCropBox;
}

long CPDFPage::GetWidth()
{
  return (recMediaBox.lRight - recMediaBox.lLeft);
}

long CPDFPage::GetHeight()
{
  return (recMediaBox.lTop - recMediaBox.lBottom);
}

CPDFPage::EPageType CPDFPage::GetRotation()
{
  return (EPageType)lRotation;
}

long CPDFPage::GetImagesCount()
{
  return lImageCount;
}

long CPDFPage::GetFontsCount()
{
  return lFontCount;
}

long CPDFPage::GetAnnotsCount()
{
  return lAnnotCount;
}

long CPDFPage::GetFontID(long i_lIndex)
{
  long lRet = -1;
  PDFList *pList;
  pList = pFontIdx;
  while(pList != NULL)
  {
    if(pList->lValue >= 0)
    {
      i_lIndex--;
      if (i_lIndex < 0)
      {
        lRet = pList->lValue;
        break;
      }
    }
    pList = pList->pNext;
  }
  return lRet;
}

long CPDFPage::GetImageID(long i_lIndex)
{
  long lRet = -1;
  PDFList *pList;
  pList = pImageIdx;
  while(pList != NULL)
  {
    if(pList->lValue >= 0)
    {
      i_lIndex--;
      if (i_lIndex < 0)
      {
        lRet = pList->lValue;
        break;
      }
    }
    pList = pList->pNext;
  }
  return lRet;
}

long CPDFPage::GetAnnotID(long i_lIndex)
{
  long lRet = -1;
  PDFList *pList;
  pList = pAnnotIdx;
  while(pList != NULL)
  {
    if(pList->lValue >= 0)
    {
      i_lIndex--;
      if (i_lIndex < 0)
      {
        lRet = pList->lValue;
        break;
      }
    }
    pList = pList->pNext;
  }
  return lRet;
}

PDFStream *CPDFPage::GetPageStream()
{
  return pPageStream;
}

bool_a CPDFPage::CopyPageStream(PDFStream *i_pBuffer)
{
  if (!i_pBuffer)
    return false_a;
  return PDFWriteToPageStream(GetBaseDoc(), i_pBuffer->pStream, i_pBuffer->curSize, lPageNum);
}

void CPDFPage::SetPageNumber(long i_lPage)
{
  lPageNum = i_lPage - 1;
  if (m_pHeader)
    m_pHeader->SetPage(i_lPage);
  if (m_pFooter)
    m_pFooter->SetPage(i_lPage);
  if (m_pTextArea)
  {
    CPDFTextBox *pTB = m_pTextArea;
    while (pTB)
    {
      pTB->SetPage(i_lPage);
      pTB = pTB->GetNextTextBox();
    }
  }
}

long CPDFPage::GetPageNumber()
{
  return lPageNum + 1;
}

bool_a CPDFPage::SetSize(long i_ldmPaperType, long i_lWidth, long i_lHeight)
{
  short i;
  if(i_ldmPaperType > 0) // wenn 0, dann kommt Wert über i_lWidth,i_lHeight
  {
    for (i = 0; i < DEF_PAPER_SIZE_COUNT; i++)
      if (DefPaperSize[i][0] == i_ldmPaperType)
        break;
    if (i < DEF_PAPER_SIZE_COUNT)
    {
      i_lWidth = m_pDoc->ConvertTwipsToPoints(DefPaperSize[i][1]);
      i_lHeight = m_pDoc->ConvertTwipsToPoints(DefPaperSize[i][2]);
    }
  }
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  pSect->SetPaperSize(i_lWidth, i_lHeight);
  long lOrient = pSect->GetOrientation();

  if (lOrient == DMORIENT_PORTRAIT)
  {
    recCropBox.lTop = recMediaBox.lTop = i_lHeight;
    recCropBox.lRight = recMediaBox.lRight = i_lWidth;
  }
  else
  {
    recCropBox.lTop = recMediaBox.lTop = i_lWidth;
    recCropBox.lRight = recMediaBox.lRight = i_lHeight;
  }
  recCropBox.lBottom = recMediaBox.lBottom = 0;
  recCropBox.lLeft = recMediaBox.lLeft = 0;

  rec_a recPosSize;
  if (m_pHeader)
  {
    if (!CalcHeaderSize(recPosSize))
      return false_a;
    if (!m_pHeader->SetPosSize(recPosSize))
      return false_a;
  }
  if (m_pFooter)
  {
    if (!CalcFooterSize(recPosSize))
      return false_a;
    if (!m_pFooter->SetPosSize(recPosSize))
      return false_a;
  }
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  return SetTextAreaSize(recPosSize);
  //return m_pTextArea->SetPosSize(recPosSize);
}

bool_a CPDFPage::SetOrientation(long i_lOrient)
{
  long lHeight, lWidth;

  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  pSect->SetOrientation(i_lOrient);

  lHeight = GetHeight();
  lWidth = GetWidth();
  if (i_lOrient == DMORIENT_PORTRAIT)
  {
    recCropBox.lTop = recMediaBox.lTop = lHeight;
    recCropBox.lRight = recMediaBox.lRight = lWidth;
  }
  else
  {
    recCropBox.lTop = recMediaBox.lTop = lWidth;
    recCropBox.lRight = recMediaBox.lRight = lHeight;
  }
  recCropBox.lBottom = recMediaBox.lBottom = 0;
  recCropBox.lLeft = recMediaBox.lLeft = 0;

  rec_a recPosSize;
  if (m_pHeader)
  {
    if (!CalcHeaderSize(recPosSize))
      return false_a;
    if (!m_pHeader->SetPosSize(recPosSize))
      return false_a;
  }
  if (m_pFooter)
  {
    if (!CalcFooterSize(recPosSize))
      return false_a;
    if (!m_pFooter->SetPosSize(recPosSize))
      return false_a;
  }
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  return SetTextAreaSize(recPosSize);
  //return m_pTextArea->SetPosSize(recPosSize);
}

bool_a CPDFPage::SetMargins(long i_lLeft, long i_lRight, long i_lTop, long i_lBottom)
{
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  if (i_lLeft >= 0)
    pSect->SetLeftMargin(i_lLeft);
  if (i_lRight >= 0)
    pSect->SetRightMargin(i_lRight);
  if (i_lTop >= 0)
    pSect->SetTopMargin(i_lTop);
  if (i_lBottom >= 0)
    pSect->SetBottomMargin(i_lBottom);

  rec_a recPosSize;
  if (m_pHeader)
  {
    if (!CalcHeaderSize(recPosSize))
      return false_a;
    if (!m_pHeader->SetPosSize(recPosSize))
      return false_a;
  }
  if (m_pFooter)
  {
    if (!CalcFooterSize(recPosSize))
      return false_a;
    if (!m_pFooter->SetPosSize(recPosSize))
      return false_a;
  }
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  return SetTextAreaSize(recPosSize);
  //return m_pTextArea->SetPosSize(recPosSize);
}

bool_a CPDFPage::SetHeaderPos(long i_lPos)
{
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  pSect->SetHdrMargin(i_lPos);

  rec_a recPosSize;
  if (m_pHeader)
  {
    if (!CalcHeaderSize(recPosSize))
      return false_a;
    if (!m_pHeader->SetPosSize(recPosSize))
      return false_a;
  }
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  return SetTextAreaSize(recPosSize);
  //return m_pTextArea->SetPosSize(recPosSize);
}

bool_a CPDFPage::SetFooterPos(long i_lPos)
{
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  pSect->SetFtrMargin(i_lPos);
  rec_a recPosSize;
  if (m_pFooter)
  {
    if (!CalcFooterSize(recPosSize))
      return false_a;
    if (!m_pFooter->SetPosSize(recPosSize))
      return false_a;
  }
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  return SetTextAreaSize(recPosSize);
  //return m_pTextArea->SetPosSize(recPosSize);
}

long CPDFPage::GetMinHeaderBottom()
{
  long lRet = 2 * GetHeight() / 5;
  return lRet;
}

long CPDFPage::GetMaxFooterTop()
{
  long lRet = 2 * GetHeight() / 5;
  return lRet;
}



CPDFPage::EActiveControl CPDFPage::GetActiveControlType()
{
  return m_eActCtrl;
}

CPDFTextBox *CPDFPage::GetActiveControl()
{
  CPDFTextBox *pRet = NULL;
  switch (m_eActCtrl)
  {
  case eTextArea:
    pRet = GetTextArea();
    break;
  case eHeader:
    pRet = (CPDFTextBox *)GetHeader();
    break;
  case eFooter:
    pRet = (CPDFTextBox *)GetFooter();
    break;
  }
  return pRet;
}

bool_a CPDFPage::ActivateControl(EActiveControl i_eControl)
{
  m_eActCtrl = i_eControl;
  return true_a;
}

CPDFTextBox *CPDFPage::GetTextArea()
{
  return m_pTextArea;
}

CPDFTextBox *CPDFPage::GetLastColumn()
{
  CPDFTextBox *pTB = m_pTextArea;
  while (pTB)
  {
    if (pTB->GetNextTextBox())
    {
      pTB = pTB->GetNextTextBox();
    }
    else
      return pTB;
  }
  return NULL;
}

bool_a CPDFPage::SetColumns(long i_lColCount, long i_lColSpace)
{
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;

  if (m_lColCount == i_lColCount && m_lColSpace == i_lColSpace)
    return true_a;

  m_lColCount = i_lColCount;
  m_lColSpace = i_lColSpace;

  if (m_lColCount <= 0)
    m_lColCount = 1;
  pSect->SetColumns(m_lColCount);
  pSect->SetColumnSpace(m_lColSpace);

  rec_a recPS;
  CPDFTextBox *pTB = m_pTextArea;
  while (pTB)
  {
    if (pTB != m_pTextArea)
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
    else
      pTB = pTB->GetNextTextBox();
  }
  m_pTextArea->SetNextTextBox(NULL);
  if (!CalcTextAreaSize(recPS))
    return false_a;
  long i, lW = (recPS.lRight - recPS.lLeft) - ((m_lColCount - 1) * m_lColSpace);
  lW /= m_lColCount;
  if (lW < m_pDoc->ConvertMMToPoints(MIN_PAGE_COLUMN_WIDTH))
  {
    long lTemp = 0;
    lW = m_pDoc->ConvertMMToPoints(MIN_PAGE_COLUMN_WIDTH);
    i = 0;
    lTemp += lW;
    while (lTemp < (recPS.lRight - recPS.lLeft))
    {
      lTemp += m_lColSpace;
      lTemp += lW;
      i++;
    }
    if (0 == i)
    {
      lW = (recPS.lRight - recPS.lLeft);
      m_lColCount = 1;
      m_lColSpace = 0;
    } else if (lTemp == (recPS.lRight - recPS.lLeft))
    {
      m_lColCount = i + 1;
    } else
    {
      m_lColCount = i + 1;
      m_lColSpace = ((recPS.lRight - recPS.lLeft) - (lW * m_lColCount)) / (m_lColCount - 1);
      //m_lColSpace -= (lTemp - (recPS.lRight - recPS.lLeft)) / i;
      if (m_lColSpace < m_pDoc->ConvertMMToPoints(MIN_PAGE_COLUMN_SPACE))
      {
        m_lColCount = i;
        m_lColSpace = ((recPS.lRight - recPS.lLeft) - (lW * m_lColCount)) / (m_lColCount - 1);
      }
    }
  }

  m_pTextArea->SetWidth(lW);

  CPDFTextBox *pTBLast = m_pTextArea;
  for (i = 0; i < m_lColCount - 1; i++)
  {
    pTB = pTBLast->CreateIdentical();
//    pTB = pTBLast->Clone();
    pTBLast->SetNextTextBox(pTB);
    pTB->SetPrevTextBox(pTBLast);
    pTB->MoveTo(pTB->GetPosSize().lLeft + pTBLast->GetWidth() + m_lColSpace,
                pTB->GetPosSize().lTop);

    pTBLast = pTB;
  }

  return true_a;
}

CPDFTextBox *CPDFPage::CheckColumnOverflow()
{
  if (!m_pDoc->CanCopyOverflowLines())
    return NULL;

  long lHeight, lWidth;
  CPDFTextBox *pTB = m_pTextArea;
  while (pTB)
  {
    if (pTB->GetNextTextBox())
    {
      if (!pTB->GetSize(lWidth, lHeight))
        return NULL;

      if (!pTB->CopyOverflowLines(pTB->GetNextTextBox(), true_a))
        return pTB;
    }
    else
      return pTB;
    pTB = pTB->GetNextTextBox();
  }
  return pTB;
}

bool_a CPDFPage::HasHeader()
{
  return m_pHeader ? true_a : false_a;
}

CPDFHeaderFooter *CPDFPage::GetHeader()
{
  if (m_pHeader)
    return m_pHeader;
  if (!m_pDoc)
    return NULL;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect || !m_pTextArea)
    return NULL;
  rec_a recPosSize;
  rec_a recTextArea = m_pTextArea->GetPosSize();
  recPosSize.lLeft = pSect->GetLeftMargin();
  recPosSize.lRight = GetWidth() - pSect->GetRightMargin();
  recPosSize.lTop = GetHeight() - pSect->GetHdrMargin();
  recPosSize.lBottom = recPosSize.lTop - m_pDoc->GetActualFontHeight();
  if (recPosSize.lBottom > recTextArea.lTop)
    recPosSize.lBottom = recTextArea.lTop;
  m_pHeader = new CPDFHeaderFooter(CPDFHeaderFooter::eHeader, m_pDoc, recPosSize, GetPageNumber());
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  if (!SetTextAreaSize(recPosSize))
    return false_a;
  //if (!m_pTextArea->SetPosSize(recPosSize))
  //  return false_a;
  m_pDoc->CheckPageOverflow(pSect->GetFirstPage());
  return m_pHeader;
}

bool_a CPDFPage::HasFooter()
{
  return m_pFooter ? true_a : false_a;
}

CPDFHeaderFooter *CPDFPage::GetFooter()
{
  if (m_pFooter)
    return m_pFooter;
  if (!m_pDoc)
    return NULL;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect || !m_pTextArea)
    return NULL;
  rec_a recPosSize;
//  rec_a recTextArea = m_pTextArea->GetPosSize();
  recPosSize.lLeft = pSect->GetLeftMargin();
  recPosSize.lRight = GetWidth() - pSect->GetRightMargin();
  recPosSize.lBottom = pSect->GetFtrMargin();
  recPosSize.lTop = recPosSize.lBottom + m_pDoc->GetActualFontHeight();
//  if (recPosSize.lTop < recTextArea.lBottom)
//    recPosSize.lTop = recTextArea.lBottom;
  m_pFooter = new CPDFHeaderFooter(CPDFHeaderFooter::eFooter, m_pDoc, recPosSize, GetPageNumber());
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  if (!SetTextAreaSize(recPosSize))
    return false_a;
  //if (!m_pTextArea->SetPosSize(recPosSize))
  //  return false_a;
  m_pDoc->CheckPageOverflow(pSect->GetFirstPage());
  return m_pFooter;
}

bool_a CPDFPage::Flush()
{
  if (m_bFlush)
    return true_a;

//  if (!MakeColumns())
//    return false_a;

  if (m_pHeader)
  {
    if (!m_pHeader->ReplacePageMarks())
      return false_a;
    if (!m_pHeader->Flush())
      return false_a;
  }
  if (m_pFooter)
  {
    if (!m_pFooter->ReplacePageMarks())
      return false_a;
    if (!m_pFooter->Flush())
      return false_a;
  }
  CPDFTextBox *pTB = m_pTextArea;
  while (pTB)
  {
    if (!pTB->ReplacePageMarks())
      return false_a;
    if (!pTB->Flush())
      return false_a;
    pTB = pTB->GetNextTextBox();
  }

  m_bFlush = true_a;
  return true_a;
}

bool_a CPDFPage::IsFlush()
{
  return m_bFlush;
}

bool_a CPDFPage::CalcHeaderSize(rec_a &i_Rec)
{
  if (!m_pHeader || !m_pTextArea || !m_pDoc)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  COPY_REC(i_Rec, recMediaBox);
  long lHeight = 0/*, lWidth = 0*/;
  rec_a recPosSize = m_pHeader->GetPosSize();
  lHeight = recPosSize.lTop - recPosSize.lBottom;
//  if (!m_pHeader->GetSize(lWidth, lHeight))
//    return false_a;
  i_Rec.lRight -= pSect->GetRightMargin();
  i_Rec.lLeft += pSect->GetLeftMargin();
  i_Rec.lTop -= pSect->GetHdrMargin();
  i_Rec.lBottom = i_Rec.lTop - lHeight;
  recPosSize = m_pTextArea->GetPosSize();
  if (i_Rec.lBottom > recPosSize.lTop)
    i_Rec.lBottom = recPosSize.lTop;
  return true_a;
}

bool_a CPDFPage::CalcFooterSize(rec_a &i_Rec)
{
  if (!m_pFooter || !m_pTextArea || !m_pDoc)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  COPY_REC(i_Rec, recMediaBox);
  long lHeight = 0, lWidth = 0;
  if (!m_pFooter->GetSize(lWidth, lHeight))
    return false_a;
  i_Rec.lRight -= pSect->GetRightMargin();
  i_Rec.lLeft += pSect->GetLeftMargin();
  i_Rec.lBottom += pSect->GetFtrMargin();
  i_Rec.lTop = i_Rec.lBottom + lHeight;
  rec_a recPosSize = m_pTextArea->GetPosSize();
  if (i_Rec.lTop < recPosSize.lBottom)
    i_Rec.lTop = recPosSize.lBottom;
  return true_a;
}

bool_a CPDFPage::CalcTextAreaSize(rec_a &i_Rec)
{
  if (!m_pTextArea || !m_pDoc)
    return false_a;
  rec_a recTemp;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  COPY_REC(i_Rec, recMediaBox);
  i_Rec.lRight -= pSect->GetRightMargin();
  i_Rec.lLeft += pSect->GetLeftMargin();
  i_Rec.lBottom += pSect->GetBottomMargin();
  i_Rec.lTop -= pSect->GetTopMargin();
  if (m_pHeader)
  {
    if (!CalcHeaderSize(recTemp))
      return false_a;
    i_Rec.lTop = recTemp.lBottom;
  }
  if (m_pFooter)
  {
    if (!CalcFooterSize(recTemp))
      return false_a;
    i_Rec.lBottom = recTemp.lTop;
  }
  return true_a;
}

bool_a CPDFPage::SetTextAreaSize(rec_a &i_Rec)
{
  if (!m_pTextArea)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  long lSaveCol = m_lColCount;


  long i, lW = (i_Rec.lRight - i_Rec.lLeft) - ((pSect->GetColumns() - 1) * pSect->GetColumnSpace());
  lW /= pSect->GetColumns();

  if (lW < m_pDoc->ConvertMMToPoints(MIN_PAGE_COLUMN_WIDTH))
  {
    long lTemp = 0;
    lW = m_pDoc->ConvertMMToPoints(MIN_PAGE_COLUMN_WIDTH);
    i = 0;
    lTemp += lW;
    while (lTemp < (i_Rec.lRight - i_Rec.lLeft))
    {
      lTemp += pSect->GetColumnSpace();
      lTemp += lW;
      i++;
    }
    if (0 == i)
    {
      lW = (i_Rec.lRight - i_Rec.lLeft);
      m_lColCount = 1;
      m_lColSpace = 0;
    } else if (lTemp == (i_Rec.lRight - i_Rec.lLeft))
    {
      m_lColCount = i + 1;
    } else
    {
      m_lColCount = i + 1;
      m_lColSpace = ((i_Rec.lRight - i_Rec.lLeft) - (lW * m_lColCount)) / (m_lColCount - 1);
      //m_lColSpace -= (lTemp - (i_Rec.lRight - i_Rec.lLeft)) / i;
      if (m_lColSpace < m_pDoc->ConvertMMToPoints(MIN_PAGE_COLUMN_SPACE))
      {
        m_lColCount = i;
        m_lColSpace = ((i_Rec.lRight - i_Rec.lLeft) - (lW * m_lColCount)) / (m_lColCount - 1);
      }
    }
  }
  long lH = i_Rec.lTop - i_Rec.lBottom;

  m_pTextArea->SetPosSize(i_Rec);
  m_pTextArea->SetWidth(lW);

  CPDFTextBox *pTB = NULL;
  if (lSaveCol != m_lColCount)
  {
    pTB = m_pTextArea;
    while (pTB)
    {
      if (pTB != m_pTextArea)
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
      else
        pTB = pTB->GetNextTextBox();
    }
    m_pTextArea->SetNextTextBox(NULL);

    CPDFTextBox *pTBLast = m_pTextArea;
    for (i = 0; i < m_lColCount - 1; i++)
    {
      pTB = pTBLast->CreateIdentical();
  //    pTB = pTBLast->Clone();
      pTBLast->SetNextTextBox(pTB);
      pTB->SetPrevTextBox(pTBLast);
      pTB->MoveTo(pTB->GetPosSize().lLeft + pTBLast->GetWidth() + m_lColSpace,
                  pTB->GetPosSize().lTop);

      pTBLast = pTB;
    }
    return true_a;
  }

  CPDFTextBox *pTBPrev = m_pTextArea;
  pTB = m_pTextArea->GetNextTextBox();
  while (pTB)
  {
    pTB->SetWidth(lW);
    pTB->SetHeight(lH);
    pTB->MoveTo(pTBPrev->GetPosSize().lRight + m_lColSpace, pTBPrev->GetPosSize().lTop);

    pTBPrev = pTB;
    pTB = pTB->GetNextTextBox();
  }

  return true_a;
}

bool_a CPDFPage::CheckResize()
{
  if (!m_pDoc)
    return false_a;
  rec_a recPosSize;
  bool_a bRet = false_a;
  if (!CalcTextAreaSize(recPosSize))
    return false_a;
  bRet = SetTextAreaSize(recPosSize);
  //bRet = m_pTextArea->SetPosSize(recPosSize);
  if (!bRet)
    return bRet;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (!pSect)
    return false_a;
  bRet = m_pDoc->CheckPageOverflow(pSect->GetFirstPage());
  return bRet;
}

const char *CPDFPage::GetContent(long &o_lSize)
{
  if (this->bCompressed)
  {
    o_lSize = this->lCompLength;
    return (char *)this->pucCompStream;
  }
  o_lSize = (long)this->pPageStream->curSize;
  return this->pPageStream->pStream;
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

CPDFHeaderFooter::CPDFHeaderFooter(EHeaderFooterType i_eHdrFtrType, CPDFDocument *i_pDoc, rec_a i_recPosSize, long i_lPage)
  :CPDFTextBox(i_pDoc,i_recPosSize, i_lPage), m_eHdrFtrType(i_eHdrFtrType)
{
  m_eTBType = eTBHeaderFooter;
  SetDrawHeight(i_recPosSize.lTop - i_recPosSize.lBottom);
}

CPDFHeaderFooter::~CPDFHeaderFooter()
{
}

void CPDFHeaderFooter::CopyAttr(CPDFHeaderFooter *i_pHF)
{
  if (!i_pHF)
    return;
  i_pHF->m_eHdrFtrType = this->m_eHdrFtrType;
  CPDFTextBox::CopyAttr(i_pHF);
}

CPDFHeaderFooter *CPDFHeaderFooter::Clone()
{
  CPDFHeaderFooter *pRetHdrFtr = NULL;
  pRetHdrFtr = new CPDFHeaderFooter(m_eHdrFtrType, m_pDoc, GetPosSize());
  if (NULL == pRetHdrFtr)
     return pRetHdrFtr;

  PDFFreeParagraph(&pRetHdrFtr->pParagraph);
  //memcpy(pRetHdrFtr, this, sizeof(CPDFHeaderFooter));
  CopyAttr(pRetHdrFtr);

  pRetHdrFtr->pTBImages = NULL;
  pRetHdrFtr->pTBSpaces = NULL;
  pRetHdrFtr->pParagraph = NULL;
  pRetHdrFtr->pActiveParagraph = NULL;
  pRetHdrFtr->pTBTabStops = NULL;
  pRetHdrFtr->sTabsCount = 0;
  pRetHdrFtr->SetTabs(this->pTBTabStops, this->sTabsCount);
  pRetHdrFtr->SetPrevTextBox(NULL);
  pRetHdrFtr->SetNextTextBox(NULL);
  pRetHdrFtr->SetImages(this->pTBImages);
  pRetHdrFtr->SetSpaces(this->pTBSpaces);
  pRetHdrFtr->SetParagraph(this->pParagraph);
  return pRetHdrFtr;
}

long CPDFHeaderFooter::AddText(wchar_t *i_pText)
{
  if (!m_pDoc)
    return -1;
  long lRet = CPDFTextBox::AddText(i_pText);
  bool_a bRet = false_a;
  bRet = CheckResize();
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->AddTextToHdrFtr(this, i_pText);
  return lRet;
}

long CPDFHeaderFooter::AddText(char *i_pText)
{
  if (!m_pDoc)
    return -1;
  long lRet = CPDFTextBox::AddText(i_pText);
  bool_a bRet = false_a;
  bRet = CheckResize();
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->AddTextToHdrFtr(this, i_pText);
  return lRet;
}

bool_a CPDFHeaderFooter::AddImage(char *i_pImageName, long i_lX, long i_lY, long i_lWidth, long i_lHeight,
                                  long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                                  bool_a i_bAddToLine)
{
  bool_a bRet = CPDFTextBox::AddImage(i_pImageName, i_lX, i_lY, i_lWidth, i_lHeight, i_lLeftDist,
                                      i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
  if (!bRet || !m_pDoc)
    return false_a;
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->AddImageToHdrFtr(this, i_pImageName, i_lX, i_lY, i_lWidth, i_lHeight, i_lLeftDist,
                            i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
  return bRet;
}

bool_a CPDFHeaderFooter::AddScaledImage(char *i_pImageName, long i_lX, long i_lY, long i_lScaledX, long i_lScaledY,
                                        long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist,
                                        bool_a i_bAddToLine)
{
  bool_a bRet = CPDFTextBox::AddScaledImage(i_pImageName, i_lX, i_lY, i_lScaledX, i_lScaledY, i_lLeftDist,
                                            i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
  if (!bRet || !m_pDoc)
    return false_a;
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->AddScaledImageToHdrFtr(this, i_pImageName, i_lX, i_lY, i_lScaledX, i_lScaledY, i_lLeftDist,
                                  i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
  return bRet;
}

bool_a CPDFHeaderFooter::InsertLine()
{
  if (!m_pDoc)
    return false_a;
  bool_a bRet = CPDFTextBox::InsertLine();
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->InsertLineToHdrFtr(this);
  return bRet;
}

bool_a CPDFHeaderFooter::InsertTab(PDFTabStop i_Tab)
{
  if (!m_pDoc)
    return false_a;
  bool_a bRet = CPDFTextBox::InsertTab(i_Tab);
  if (!bRet)
    return false_a;
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->InsertTabToHdrFtr(this, i_Tab);
  return bRet;
}

bool_a CPDFHeaderFooter::SetTextColor(float i_fRed, float i_fGreen, float i_fBlue)
{
  if (!m_pDoc)
    return false_a;
  bool_a bRet = CPDFTextBox::SetTextColor(i_fRed, i_fGreen, i_fBlue);
  if (!bRet)
    return false_a;
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->SetTextColorToHdrFtr(this, i_fRed, i_fGreen, i_fBlue);
  return bRet;
}

bool_a CPDFHeaderFooter::SetAlignment(short i_sAlignment)
{
  if (!m_pDoc)
    return false_a;
  bool_a bRet = CPDFTextBox::SetAlignment(i_sAlignment);
  if (!bRet)
    return false_a;
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->SetAlignmentToHdrFtr(this, i_sAlignment);
  return bRet;
}

bool_a CPDFHeaderFooter::SetLeftRightIndents(long i_lLeft, long i_lRight, long i_lFirstIndent)
{
  if (!m_pDoc)
    return false_a;
  bool_a bRet = CPDFTextBox::SetLeftRightIndents(i_lLeft, i_lRight, i_lFirstIndent);
  if (!bRet)
    return false_a;
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->SetLeftRightIndentsToHdrFtr(this, i_lLeft, i_lRight, i_lFirstIndent);
  return bRet;
}

bool_a CPDFHeaderFooter::AddPageMark(ePAGEMARKTYPE i_eType)
{
  if (!m_pDoc)
    return false_a;
  bool_a bRet = CPDFTextBox::AddPageMark(i_eType);
  if (!bRet)
    return false_a;
  bRet = CheckResize();
  if (!bRet)
    return false_a;
  CPDFSection *pSect = m_pDoc->GetSection(GetPageNumber());
  if (pSect)
    pSect->AddPageMarkToHdrFtr(this, i_eType);
  return bRet;
}

bool_a CPDFHeaderFooter::CheckResize()
{
  long lWidth, lHeight;
  bool_a bRet = true_a;
  if (CPDFTextBox::GetTextDirection() != eHorizontal)
    return true_a;
  bRet = GetSize(lWidth, lHeight);
  if (!bRet)
    return bRet;
  if (lHeight > GetDrawHeight())
    bRet = SetDrawHeight(lHeight);
  return bRet;
}

bool_a CPDFHeaderFooter::SetHeight(long i_lHeight)
{
  if (!m_pDoc)
    return false_a;
  CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber());
  if (!pPage)
    return false_a;
  long lMax;
  if (m_eHdrFtrType == CPDFHeaderFooter::eHeader)
  {
    lMax = pPage->GetMinHeaderBottom();
    recTB.lBottom += GetHeight() - i_lHeight;
    recTB.lBottom = recTB.lBottom > lMax ? recTB.lBottom : lMax;
//    bRet = CPDFTextBox::SetHeight(i_lHeight);
  }
  else
  {
    lMax = pPage->GetMaxFooterTop();
    recTB.lTop += i_lHeight - GetHeight();
    recTB.lTop = recTB.lTop < lMax ? recTB.lTop : lMax;
//    bRet = CheckResize();
  }
  return pPage->CheckResize();
}

bool_a CPDFHeaderFooter::SetDrawHeight(long i_lHeight)
{
  if (!m_pDoc)
    return false_a;
  CPDFPage *pPage = m_pDoc->GetPage(GetPageNumber());
  if (!pPage)
    return false_a;
  long lMax;
  if (m_eHdrFtrType == CPDFHeaderFooter::eHeader)
  {
    lMax = pPage->GetMinHeaderBottom();
    recTB.lBottom += GetDrawHeight() - i_lHeight;
    recTB.lBottom = recTB.lBottom > lMax ? recTB.lBottom : lMax;
//    bRet = CPDFTextBox::SetDrawHeight(i_lHeight);
  }
  else
  {
    lMax = pPage->GetMaxFooterTop();
    recTB.lTop += i_lHeight - GetDrawHeight();
    recTB.lTop = recTB.lTop < lMax ? recTB.lTop : lMax;
//    bRet = CheckResize();
  }
  return pPage->CheckResize();
}
