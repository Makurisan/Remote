

#if defined(WIN32)
#include <crtdbg.h>
#endif
#include "pdfdef.h"
#include "pdffnc.h"
#include "PDFDocument.h"
#include "PDFTemplate.h"
#include "PDFBaseFont.h"
#include "PDFType0Font.h"
#include "PDFTTFFont.h"
#include "PDFPage.h" // MIK
#include "pdfimg.h"
#include "zlib.h"
#include <sstream>

using namespace std;

CPDFDocument::CPDFDocument()
{
  m_bFlush = false_a;
  m_bCanCopyOverflow = false_a;

  m_bFontEmbed = false_a;

  pDocStream = NULL;
  bFlushToFile = false_a;

  pObjOffset = NULL;
  pObjIndex = NULL;

  SetActualCodePage(lDefCodePage);

  PDFDoc* ptr = GetStruct();
  PDFInit(ptr);

  m_strLayer = "";
}

CPDFDocument::~CPDFDocument()
{
  DeleteAll();
  long i;

  for (i = 0; i < this->m_vctAnnots.size(); i++)
  {
    if (this->m_vctAnnots[i])
    {
      delete (CPDFAnnotation *)m_vctAnnots[i];
    }
  }
  if(this->streamFilterList)
    free(this->streamFilterList);
  this->streamFilterList = NULL;

  free(this->pObjOffset);
  this->pObjOffset = NULL;
  free(this->pObjIndex);
  this->pObjIndex = NULL;
}

long CPDFDocument::SetPos(void *i_pPtr, eSPACETYPE i_eType, long i_lPage, long i_lPosY, long i_lPosX,
                          bool_a i_bDivide, void *i_pParent)
{
  long lRet = -1;
  if (!i_pPtr)
    return lRet;
  switch (i_eType)
  {
  case eFrame:
    {
      CPDFDocument *pDoc = ((CPDFFrame *)i_pPtr)->GetDocument();
      if (pDoc)
        lRet = pDoc->ProcessFrame((CPDFFrame *)i_pPtr, i_lPage, i_lPosX, i_lPosY);
      break;
    }
  case eFixTableWidth:
    {
      CPDFDocument *pDoc = ((CPDFTable *)i_pPtr)->GetDocument();
      if (pDoc)
        lRet = pDoc->ProcessTable((CPDFTable *)i_pPtr, i_lPage, i_lPosX, i_lPosY, i_bDivide, (CPDFTextBox *)i_pParent);
      break;
    }
  default:
    return 0;
  }
  return lRet;
}

long CPDFDocument::ProcessFrame(CPDFFrame *i_pFrame, long i_lPage, long i_lPosX, long i_lPosY)
{
  i_lPosX;//because of warning
  long lRet = -1;
  if (!i_pFrame)
    return lRet;
  i_pFrame->SetPage(i_lPage);
  rec_a recPosSize = i_pFrame->GetPosSize();
  i_pFrame->MoveTo(recPosSize.lLeft/*i_lPosX*/, i_lPosY);
  lRet = 0;
  return lRet;
}

long CPDFDocument::ProcessTable(CPDFTable *i_pTable, long i_lPage, long i_lPosX, long i_lPosY,
                                bool_a i_bDivide, CPDFTextBox *i_pParent)
{
  long lRet = -1;
  if (!i_pTable)
    return lRet;
  CPDFDocument *pDoc = i_pTable->GetDocument();
  if (!pDoc)
    return lRet;

  i_lPage = i_pTable->CalculateTBPos(i_pParent);

  if (!i_pTable->IsDivided()/* || (i_pTable->GetPageNumber() == i_lPage)*/)
  {
    i_pTable->SetPage(i_lPage);
    i_pTable->MoveTo(/*i_pTable->GetXPos()*/i_lPosX, i_lPosY);
    if (i_pTable->GetInTextBox() != i_pParent)
      i_pTable->SetInTextBox(i_pParent);
  }
  if (i_bDivide)
  {
    long lHeight, lWidth;
    CPDFTextBox *pTextArea = i_pParent;//i_pTable->GetInTextBox();
    if (!pTextArea)
      return lRet;
    if (!pTextArea->GetSize(lWidth, lHeight))
      return lRet;
    if (!i_pTable->IsDivided())
    {
      lHeight = pTextArea->GetBottomDrawPos() -
                (i_pTable->GetYPos() - i_pTable->GetRowsHeights(i_pTable->GetRowCount()));
      lRet = i_pTable->DivideTable(i_pTable->GetRowsHeights(i_pTable->GetRowCount()) - lHeight, i_pTable->GetPageNumber());
    }
    else
    {
//      lHeight = lHeight - pTextArea->GetDrawHeight();
      lHeight = pTextArea->GetBottomDrawPos() -
              (i_lPosY - i_pTable->GetDividedHeight(i_lPage));
      lRet = i_pTable->DivideTable(i_pTable->GetDividedHeight(i_lPage) - lHeight, i_lPage);
    }
  }
  else
    lRet = 0;

  return lRet;
}



void CPDFDocument::FlushToFile(bool_a i_bFlush)
{
  this->bFlushToFile = i_bFlush;
}

void CPDFDocument::SetDocInfo(eDOCINFO i_eType, const char *i_pText)
{
  size_t lSize = 0;
  if (!i_pText)
    return;

  switch (i_eType)
  {
    case eTitle:
      memset(this->strInfo.czTitle, 0, NAME_SIZE);
      lSize = strlen(i_pText);
      if (lSize < NAME_SIZE)
        strcpy(this->strInfo.czTitle, i_pText);
      else
        strncpy(this->strInfo.czTitle, i_pText, NAME_SIZE - 1);
    break;
    case eAuthor:
      memset(this->strInfo.czAuthor, 0, NAME_SIZE);
      lSize = strlen(i_pText);
      if (lSize < NAME_SIZE)
        strcpy(this->strInfo.czAuthor, i_pText);
      else
        strncpy(this->strInfo.czAuthor, i_pText, NAME_SIZE - 1);
    break;
    case eSubject:
      memset(this->strInfo.czSubject, 0, NAME_SIZE);
      lSize = strlen(i_pText);
      if (lSize < NAME_SIZE)
        strcpy(this->strInfo.czSubject, i_pText);
      else
        strncpy(this->strInfo.czSubject, i_pText, NAME_SIZE);
    break;
    case eKeywords:
      memset(this->strInfo.czKeywords, 0, NAME_SIZE);
      lSize = strlen(i_pText);
      if (lSize < NAME_SIZE)
        strcpy(this->strInfo.czKeywords, i_pText);
      else
        strncpy(this->strInfo.czKeywords, i_pText, NAME_SIZE);
    break;
    case eCreator:
      memset(this->strInfo.czCreator, 0, NAME_SIZE);
      lSize = strlen(i_pText);
      if (lSize < NAME_SIZE)
        strcpy(this->strInfo.czCreator, i_pText);
      else
        strncpy(this->strInfo.czCreator, i_pText, NAME_SIZE - 1);
    break;
    case eProducer:
      memset(this->strInfo.czProducer, 0, NAME_SIZE);
      lSize = strlen(i_pText);
      if (lSize < NAME_SIZE)
        strcpy(this->strInfo.czProducer, i_pText);
      else
        strcpy(this->strInfo.czProducer, i_pText);
    break;
    case eCreationDate:
      memset(this->strInfo.czCreationDate, 0, DATE_SIZE);
      lSize = strlen(i_pText);
      if (lSize < DATE_SIZE)
        strcpy(this->strInfo.czCreationDate, i_pText);
      else
        strncpy(this->strInfo.czCreationDate, i_pText, DATE_SIZE - 1);
    break;
    case eModDate:
      memset(this->strInfo.czModDate, 0, DATE_SIZE);
      lSize = strlen(i_pText);
      if (lSize < DATE_SIZE)
        strcpy(this->strInfo.czModDate, i_pText);
      else
        strncpy(this->strInfo.czModDate, i_pText, DATE_SIZE - 1);
    break;
    default:
    break;
  }
}

const char *CPDFDocument::GetDocInfo(eDOCINFO i_eType)
{
  switch (i_eType)
  {
    case eTitle:
      return this->strInfo.czTitle;
    case eAuthor:
      return this->strInfo.czAuthor;
    case eSubject:
      return this->strInfo.czSubject;
    case eKeywords:
      return this->strInfo.czKeywords;
    case eCreator:
      return this->strInfo.czCreator;
    case eProducer:
      return this->strInfo.czProducer;
    case eCreationDate:
      return this->strInfo.czCreationDate;
    case eModDate:
      return this->strInfo.czModDate;
    default:
    break;
  }
  return NULL;
}

bool_a CPDFDocument::InsertPageBreak()
{
  long lPrevPage = GetCurrentPage();
  CPDFPage *pPage = GetPage(lPrevPage);
  if (!pPage)
    return false_a;
  CPDFTextBox *pTextAreaPrev = pPage->GetTextArea();
  if (!pTextAreaPrev)
    return false_a;

  if (!CanCopyOverflowLines())
    return pTextAreaPrev->InsertBreak();

  CPDFSection *pSect = GetSection(lPrevPage + 1);
  if (!pSect)
    return false_a;
  if (!CreatePage(lPrevPage + 1, pSect))
    return false_a;

//  if (pTextAreaPrev->HasCanPageBreak())
  {
    CPDFPage *pPage = GetPage(lPrevPage + 1);
    if (!pPage)
      return false_a;
    CPDFTextBox *pTextArea = pPage->GetTextArea();
    if (!pTextArea)
      return false_a;

    if (!pTextArea->CopyAttributtes(pTextAreaPrev))
      return false_a;

    bool_a bRet = pTextAreaPrev->InsertBreak(pTextArea);
    pTextAreaPrev->SetCanPageBreak(true_a);
    return bRet;
  }
//  return true_a;
}

bool_a CPDFDocument::CanPageBreak(bool_a i_bDelete)
{
  CPDFPage *pPage = GetPage(GetCurrentPage());
  if (!pPage)
    return false_a;
  CPDFTextBox *pTextArea = pPage->GetTextArea();
  if (!pTextArea)
    return false_a;
  if (!CanCopyOverflowLines())
    pTextArea->SetCanPageBreak();
  else
    pTextArea->SetCanPageBreak(i_bDelete);
  return true_a;
}

bool_a CPDFDocument::CheckCanPageBreak()
{
  if (!CanCopyOverflowLines())
    return true_a;
  CPDFPage *pPage = GetPage(GetCurrentPage());
  if (!pPage)
    return false_a;
  CPDFTextBox *pTextArea = pPage->GetTextArea();
  if (!pTextArea)
    return false_a;
  long lHeight = 0, lWidth = 0;
  if (!pTextArea->GetSize(lWidth, lHeight))
    return false_a;
  if (pTextArea->HasCanPageBreak())
  {
    if (pTextArea->GetHeight() < lHeight)
    {
      return InsertPageBreak();
    }
  }
  return true_a;
}

bool_a CPDFDocument::InsertSectionBreak()
{
  long lCurrentPage = GetCurrentPage();

  CPDFSection *pSect = GetSection(lCurrentPage);
  if (!pSect)
    return false_a;
  pSect->SetLastPage(lCurrentPage);
  pSect = new CPDFSection(pSect);//(this, lCurrentPage + 1, -1);
  if (!pSect)
    return false_a;
  m_SectionMap[(long)(m_SectionMap.size())] = pSect;

  if (!CanCopyOverflowLines())
  {
    if (!CreatePage(lCurrentPage + 1, pSect))
      return false_a;
    return true_a;
  }

  CPDFPage *pPage = GetPage(lCurrentPage);
  if (!pPage)
    return false_a;
  CPDFTextBox *pTextArea = pPage->GetTextArea();
  if (!pTextArea)
    return false_a;
  if (pTextArea->HasCanPageBreak())
  {
    InsertPageBreak();
    if (lCurrentPage == (GetCurrentPage() - 1))
    {
      lCurrentPage++;
      pPage = GetPage(lCurrentPage);
      if (!pPage)
        return false_a;
      pTextArea = pPage->GetTextArea();
      if (!pTextArea)
        return false_a;
      if (!pTextArea->IsEmpty())
        InsertPageBreak();
    }
  }
  else
    InsertPageBreak();
  return true_a;
}

bool_a CPDFDocument::InsertColumnBreak()
{
  long lPrevPage = GetCurrentPage();
  CPDFPage *pPage = GetPage(lPrevPage);
  if (!pPage)
    return false_a;
  CPDFTextBox *pTextArea = pPage->GetTextArea();
  if (!pTextArea)
    return false_a;

  if (!CanCopyOverflowLines())
    return pTextArea->InsertColumnBreak();

  return false_a;
}

bool_a CPDFDocument::Initialize(const char *i_pczDocName)
{
  if (!i_pczDocName)
    return false_a;
  m_strFileName = i_pczDocName;
  CPDFSection *pSect = new CPDFSection(this, 1, -1);
  if (!pSect)
    return false_a;
  m_SectionMap[0] = pSect;

  this->iCurrAllocObjects = OBJECTS_ALLOC_STEP;

  this->pObjOffset = (long *)PDFMalloc((this->iCurrAllocObjects * sizeof(long)));
  if (!(this->pObjOffset))
    return false_a;
  this->pObjIndex = (long *)PDFMalloc((this->iCurrAllocObjects * sizeof(long)));
  if (!(this->pObjIndex))
    return false_a;

  this->pDocStream = PDFCreateStream();

  // Decode filter list as specified in PDF /Filter [] list 
  //TODO:fix it
  this->streamFilterList = PDFStrDup("/FlateDecode");

  pDocFunc = (&CPDFDocument::SetPos);
  return true_a;
}

void CPDFDocument::SetPassword(const char *i_pOwner, const char *i_pUser, int i_iPermission)
{
  PDFSetOwnerPassword(this, (char *)i_pOwner);
  PDFSetUserPassword(this, (char *)i_pUser);
  PDFSetPermissionPassword(this, i_iPermission);
}

const char *CPDFDocument::GetOwnerPassword()
{
  return (const char *)this->ucOwnerPassw;
}

const char *CPDFDocument::GetUserPassword()
{
  return (const char *)this->ucOwnerPassw;
}

void CPDFDocument::SetFontEmbedded(bool_a i_bEmbed)
{
  m_bFontEmbed = i_bEmbed;
}

bool_a CPDFDocument::GetFontEmbedded()
{
  return m_bFontEmbed;
}

void CPDFDocument::SetCanCopyOverflowLines(bool_a i_bEnable)
{
  m_bCanCopyOverflow = i_bEnable;
}

bool_a CPDFDocument::CanCopyOverflowLines()
{
  return m_bCanCopyOverflow;
}

bool_a CPDFDocument::CheckPageOverflow(long i_lPage)
{
  if (!CanCopyOverflowLines())
    return true_a;
  long lPage = i_lPage == -1 ? GetCurrentPage() : i_lPage;
  CPDFPage *pPage = GetPage(lPage);
  if (!pPage)
    return false_a;

  CPDFTextBox *pTextArea = pPage->GetTextArea();
  if (!pTextArea)
    return false_a;

  CPDFSection *pSect = GetSection(lPage);
  if (!pSect)
    return false_a;

  if (pSect->GetColumns() > 1)
    pTextArea = pPage->CheckColumnOverflow();

  long lHeight, lWidth;
  if (!pTextArea->GetSize(lWidth, lHeight))
    return false_a;

  if (lHeight <= pTextArea->GetDrawHeight())
  {
    if (pTextArea->HasOnlyTable() || pTextArea->IsTableLast())
      if (lPage < GetPageCount())
        return CheckPageOverflow(lPage + 1);
    if (!pTextArea->HasBreak())
    {
      if (!pSect->PageBelong(lPage + 1))
        return CheckPageOverflow(lPage + 1);
      else
        return true_a;
    }
  }
/*
  if (pTextArea->HasCanPageBreak() && !pTextArea->IsTableOverflow())
  {
    if (!InsertPageBreak())
      return false_a;
    return CheckPageOverflow(lPage + 1);
  }
*/
  if (!pTextArea->CanDivide())
  {
    if (!pSect->PageBelong(lPage + 1))
      return CheckPageOverflow(lPage + 1);
    else
      return true_a;
  }

  SetCanCopyOverflowLines(false_a);
  if (!pSect->PageBelong(lPage + 1))
  {
    pSect->SetLastPage(lPage + 1);
    PDFPtrIterator iterator;
    bool_a bFind = false_a;
    for (iterator = m_SectionMap.begin(); iterator != m_SectionMap.end(); iterator++)
    {
      CPDFSection *pSectTemp = (CPDFSection *)(iterator->second);
      if (bFind)
      {
        pSectTemp->SetFirstPage(pSectTemp->GetFirstPage() + 1);
        if (pSectTemp->GetLastPage() > 0)
          pSectTemp->SetLastPage(pSectTemp->GetLastPage() + 1);
      }
      if (pSectTemp && pSectTemp == pSect)
        bFind = true_a;
    }
    pPage = InsertPage(lPage + 1, pSect);
    if (!pPage)
      return false_a;
  }
  else
  {
    pPage = GetPage(lPage + 1);
    if (!pPage)
    {
      pPage = CreatePage(lPage + 1, pSect);
      if (!pPage)
        return false_a;
    }
  }
  SetCanCopyOverflowLines(true_a);
  if (pPage->GetTextArea())
    if (!pPage->GetTextArea()->CopyAttributtes(pTextArea))
      return false_a;

  if (!pTextArea->CopyOverflowLines(pPage->GetTextArea()))
    return false_a;
  return CheckPageOverflow(lPage + 1);
}

const char *CPDFDocument::GetName()
{
  return m_strFileName.c_str();
}

void CPDFDocument::EnableCompression(bool_a i_bEnable)
{
  PDFEnableCompress(this, i_bEnable);
}

long CPDFDocument::GetDocumentStream(const char **o_pczStream)
{
  if (GetTemplateCount() > 0)
  {
    long lSize = 0;
    *o_pczStream = m_TemplatesPDF[0]->GetPDFStream(lSize);
    return lSize;
  }
  else
  {
    (*o_pczStream) = PDFGetDocStream((PDFDocPTR)this);
    return (long)pDocStream->curSize;
  }
}

long CPDFDocument::ConvertMMToPoints(float i_fMM)
{
  return PDFConvertMMToPoints(i_fMM);
}

long CPDFDocument::ConvertPointsToTwips(long i_lPoints)
{
  return PDFConvertPointsToTwips(i_lPoints);
}

long CPDFDocument::ConvertTwipsToPoints(long i_lTwips)
{
  return PDFConvertTwipsToPoints(i_lTwips);
}

long CPDFDocument::ConvertTwipsToPoints(int i_iTwips)
{
  return PDFConvertTwipsToPoints((long)i_iTwips);
}

long CPDFDocument::ConvertTwipsToPoints(float i_fTwips)
{
  long lTwips = PDFRound(i_fTwips);
  return PDFConvertTwipsToPoints(lTwips);
}

CPDFPage *CPDFDocument::CreatePage(long i_lNum, long i_lWidth, long i_lHeight,
                                    CPDFPage::EPageType i_eType)
{
  CPDFPage *pPage = NULL;

  if (i_lWidth <= 0 || i_lHeight <= 0 || i_lNum <= 0)
    return pPage;

  if (i_lNum <= GetPageCount())
  {
    pPage = ((CPDFPage *)(m_vctPages[i_lNum - 1]));
    return pPage;
  }
  if (i_lNum > GetPageCount())
    i_lNum = GetPageCount() + 1;

  pPage = new CPDFPage(this, i_lWidth, i_lHeight, i_lNum, i_eType);

  m_vctPages.push_back(pPage);

  PDFSetCurrentPage(this, i_lNum);
  PDFresetTextCTM(this);

  return pPage;
}

CPDFPage *CPDFDocument::CreatePage(long i_lNum, CPDFSection *i_pSection,
                                    CPDFPage::EPageType i_eType)
{
  CPDFPage *pPage = NULL;

  if (i_lNum <= 0 || !i_pSection)
    return pPage;

  if (i_lNum <= GetPageCount())
  {
    pPage = ((CPDFPage *)(m_vctPages[i_lNum - 1]));
    return pPage;
  }
  if (i_lNum > GetPageCount())
    i_lNum = GetPageCount() + 1;

  long lHeight, lWidth;

  lWidth = i_pSection->GetPaperWidth();
  lHeight = i_pSection->GetPaperHeight();
  if (i_pSection->GetOrientation() == DMORIENT_PORTRAIT)
  {
  }
  else
  {
    lWidth = i_pSection->GetPaperHeight();
    lHeight = i_pSection->GetPaperWidth();
  }

  pPage = new CPDFPage(this, lWidth, lHeight, i_lNum, i_eType);

  m_vctPages.push_back(pPage);

  PDFSetCurrentPage(this, i_lNum);
  PDFresetTextCTM(this);

  return pPage;
}

CPDFPage *CPDFDocument::InsertPage(long i_lNum, CPDFSection *i_pSection,
                                    CPDFPage::EPageType i_eType)
{
  CPDFPage *pPage = NULL;
  if (i_lNum <= 0 || !i_pSection)
    return pPage;
  long lHeight, lWidth;
  lWidth = i_pSection->GetPaperWidth();
  lHeight = i_pSection->GetPaperHeight();
  if (i_pSection->GetOrientation() == DMORIENT_PORTRAIT)
  {
  }
  else
  {
    lWidth = i_pSection->GetPaperHeight();
    lHeight = i_pSection->GetPaperWidth();
  }

  pPage = new CPDFPage(this, lWidth, lHeight, i_lNum, i_eType);

  m_vctPages.insert(m_vctPages.begin() + (i_lNum - 1), (PDFPage*)pPage);

  PDFSetCurrentPage(this, i_lNum);
  PDFresetTextCTM(this);

  AdjustSections(i_lNum);

  return pPage;
}

CPDFPage *CPDFDocument::GetPage(long i_lNum)
{
  if (i_lNum <= 0 || i_lNum > GetPageCount())
    return NULL;

  return ((CPDFPage *)(m_vctPages[i_lNum - 1]));
}

long CPDFDocument::GetPageCount()
{
  return m_vctPages.size();
}

void CPDFDocument::DeletePage(long i_lNum)
{
  if (i_lNum <= 0 || i_lNum > GetPageCount())
    return;

  CPDFPage *pPage = ((CPDFPage *)(m_vctPages[i_lNum - 1]));

  PDFDeletePage(this, i_lNum - 1);
  m_vctPages.erase(m_vctPages.begin() + (i_lNum - 1));

  AdjustSections(i_lNum);

  delete pPage;

}

long CPDFDocument::GetLastPage()
{
  return GetPageCount();
}

long CPDFDocument::GetCurrentPage()
{
  return m_lActualPage + 1;
}

bool_a CPDFDocument::SetCurrentPage(long i_lPage)
{
  CPDFPage *pPage = GetPage(i_lPage);
  if (!pPage)
    return false_a;
  return PDFSetCurrentPage(this, i_lPage);
}

void CPDFDocument::AdjustSections(long i_lPage)
{
  long i;
  CPDFPage *pPage = NULL;

  for(i = i_lPage - 1; i < GetPageCount(); i++)
  {
    pPage = ((CPDFPage *)(m_vctPages[i]));
    _ASSERT(pPage);
    if (pPage)
    {
      pPage->SetPageNumber(i);
    }
  }
  //TO DO: adjust sections?
}









long CPDFDocument::GetFontsCount()
{
  return m_vctFonts.size();
}

long CPDFDocument::GetImagesCount()
{
  return m_vctImages.size();
}






CPDFTextBox *CPDFDocument::CreateTextBox(long i_lPage, rec_a i_recPosSize, CPDFTextBox *i_pParent)
{
  CPDFTextBox *pRetTB = NULL;
  pRetTB = new CPDFTextBox(this, i_recPosSize, i_lPage, false_a, i_pParent);
  if (pRetTB)
    m_TextBoxMap[(long)m_TextBoxMap.size()] = pRetTB;
  return pRetTB;
}

void CPDFDocument::DeleteTextBox(long i_lID)
{
  CPDFTextBox     *pTextBox = NULL;
  PDFPtrIterator  iterator = m_TextBoxMap.find(i_lID);
  if (iterator != m_TextBoxMap.end())
  {
    pTextBox = ((CPDFTextBox *)(iterator->second));
    _ASSERT(pTextBox);
    if (pTextBox)
    {
      if (pTextBox->GetPrevTextBox())
        pTextBox->GetPrevTextBox()->SetNextTextBox(pTextBox->GetNextTextBox());
      if (pTextBox->GetNextTextBox())
        pTextBox->GetNextTextBox()->SetPrevTextBox(pTextBox->GetPrevTextBox());
      delete pTextBox;
    }
    m_TextBoxMap.erase(iterator);
  }
}

void CPDFDocument::DeleteTextBox(CPDFTextBox *i_pTextBox)
{
  CPDFTextBox     *pTextBox = NULL;
  PDFPtrIterator  iterator;
  for (iterator = m_TextBoxMap.begin(); iterator != m_TextBoxMap.end(); iterator++)
  {
    pTextBox = ((CPDFTextBox *)(iterator->second));
    _ASSERT(pTextBox);
    if (pTextBox == i_pTextBox)
    {
      if (pTextBox->GetPrevTextBox())
        pTextBox->GetPrevTextBox()->SetNextTextBox(pTextBox->GetNextTextBox());
      if (pTextBox->GetNextTextBox())
        pTextBox->GetNextTextBox()->SetPrevTextBox(pTextBox->GetPrevTextBox());
      delete pTextBox;
      m_TextBoxMap.erase(iterator);
      break;
    }
  }
}

CPDFTextBox *CPDFDocument::RemoveTextBox(long i_lID)
{
  CPDFTextBox     *pTextBox = NULL;
  PDFPtrIterator  iterator = m_TextBoxMap.find(i_lID);
  if (iterator != m_TextBoxMap.end())
  {
    pTextBox = ((CPDFTextBox *)(iterator->second));
    _ASSERT(pTextBox);
    if (pTextBox)
    {
      if (pTextBox->GetPrevTextBox())
        pTextBox->GetPrevTextBox()->SetNextTextBox(pTextBox->GetNextTextBox());
      if (pTextBox->GetNextTextBox())
        pTextBox->GetNextTextBox()->SetPrevTextBox(pTextBox->GetPrevTextBox());
    }
    m_TextBoxMap.erase(iterator);
    return pTextBox;
  }
  return NULL;
}

CPDFTextBox *CPDFDocument::RemoveTextBox(CPDFTextBox *i_pTextBox)
{
  CPDFTextBox     *pTextBox = NULL;
  PDFPtrIterator  iterator;
  for (iterator = m_TextBoxMap.begin(); iterator != m_TextBoxMap.end(); iterator++)
  {
    pTextBox = ((CPDFTextBox *)(iterator->second));
    _ASSERT(pTextBox);
    if (pTextBox == i_pTextBox)
    {
      if (pTextBox->GetPrevTextBox())
        pTextBox->GetPrevTextBox()->SetNextTextBox(pTextBox->GetNextTextBox());
      if (pTextBox->GetNextTextBox())
        pTextBox->GetNextTextBox()->SetPrevTextBox(pTextBox->GetPrevTextBox());
      m_TextBoxMap.erase(iterator);
      return pTextBox;
    }
  }
  return NULL;
}

long CPDFDocument::GetTextBoxCount()
{
  return (long)m_TextBoxMap.size();
}

CPDFTextBox *CPDFDocument::GetTextBox(long i_lID)
{
  PDFPtrIterator  iterator = m_TextBoxMap.find(i_lID);
  if (iterator != m_TextBoxMap.end())
    return (CPDFTextBox *)(iterator->second);
  return NULL;
}

long CPDFDocument::GetFrameCount()
{
  return (long)m_FrameMap.size();
}

CPDFFrame *CPDFDocument::GetFrame(long i_lOrder)
{
  PDFPtrIterator iterator = m_FrameMap.find(i_lOrder);
  if (iterator != m_FrameMap.end())
    return (CPDFFrame *)(iterator->second);
  return NULL;
}

CPDFFrame *CPDFDocument::GetFrame(const char *i_pID, long &o_lOrder)
{
  PDFPtrIterator  iterator;
  CPDFFrame *pFrame;
  for (iterator = m_FrameMap.begin(); iterator != m_FrameMap.end(); iterator++)
  {
    pFrame = (CPDFFrame *)(iterator->second);
    _ASSERT(pFrame);
    if (pFrame && pFrame->GetFrameID())
    {
      if (!strcmp(pFrame->GetFrameID(), i_pID))
      {
        o_lOrder = iterator->first;
        return pFrame;
      }
    }
  }
  o_lOrder = -1;
  return NULL;
}

CPDFFrame *CPDFDocument::CreateFrame(long i_lPage, rec_a i_recPosSize, CPDFFrame::eYPosType i_eType,
                                     long &o_lID, const char *i_pID)
{
  CPDFFrame *pRet = GetFrame(i_pID, o_lID);
  if (pRet)
    return pRet;
  o_lID = (long)m_FrameMap.size();
  pRet = new CPDFFrame(this, i_recPosSize, i_pID, i_lPage, i_eType);
  if (pRet)
    m_FrameMap[o_lID] = pRet;
  return pRet;
}

bool_a CPDFDocument::JoinFrames(CPDFFrame *i_pFrame, const char *i_pID)
{
  if (!i_pFrame || !i_pID || !strlen(i_pID))
    return false_a;
  PDFPtrIterator  iterator;
  CPDFFrame *pFrame;
  for (iterator = m_FrameMap.begin(); iterator != m_FrameMap.end(); iterator++)
  {
    pFrame = (CPDFFrame *)(iterator->second);
    _ASSERT(pFrame);
    if (pFrame && pFrame->GetFrameID())
    {
      if (!strcmp(pFrame->GetFrameID(), i_pID))
      {
        if (pFrame->GetNextTextBox())
        {
          pFrame->GetNextTextBox()->SetPrevTextBox(i_pFrame);
          i_pFrame->SetNextTextBox(pFrame->GetNextTextBox());
        }
        pFrame->SetNextTextBox(i_pFrame);
        i_pFrame->SetPrevTextBox(pFrame);
        return true_a;
      }
    }
  }
  return false_a;
}

bool_a CPDFDocument::CheckFrameOverlap()
{
  PDFPtrIteratorR iter1, iter2;
  PDFPtrIterator iter;
  CPDFFrame *pFrame1, *pFrame2;

  for (iter = m_FrameMap.begin(); iter != m_FrameMap.end(); iter++)
  {
    pFrame1 = (CPDFFrame *)(iter->second);
    _ASSERT(pFrame1);
    if (!pFrame1->MoveToYPos())
      return false_a;
  }

  for (iter1 = m_FrameMap.rbegin(); iter1 != m_FrameMap.rend();)
  {
    pFrame1 = (CPDFFrame *)(iter1->second);
    _ASSERT(pFrame1);
    iter1++;
    for (iter2 = iter1; iter2 != m_FrameMap.rend(); iter2++)
    {
      pFrame2 = (CPDFFrame *)(iter2->second);
      _ASSERT(pFrame2);
      if (pFrame2->GetOverlapType() == CPDFFrame::eNoOverlap)
        continue;
      if (!pFrame1 || !pFrame2)
        return false_a;
      if (pFrame2->Overlap(pFrame1) && (pFrame1->GetPageNumber() == pFrame2->GetPageNumber()))
      {
        if (!pFrame2->AddFrame(pFrame1, true_a))
          return false_a;
      }
    }
  }
  return true_a;
}

CPDFTable *CPDFDocument::CreateTable(long i_lPage, long i_lRowCount, long i_lColCount,
                                     long i_lXPos, long i_lYPos, long i_lXStep, long i_lYStep)
{
  CPDFTable *pRetTable = NULL;
  pRetTable = new CPDFTable(this, i_lPage, i_lRowCount, i_lColCount, i_lXPos, i_lYPos,
                            i_lXStep, i_lYStep);
  if (pRetTable)
    m_TableMap[(long)m_TableMap.size()] = pRetTable;
  return pRetTable;
}

CPDFTable *CPDFDocument::CreateTable(long i_lPage, long i_lRowCount, long i_lColCount, long i_lXPos,
                                      long i_lYPos, long i_lXStep, long i_lYStep, long &o_lID)
{
  CPDFTable *pRetTable = NULL;
  long lID = (long)m_TableMap.size();
  pRetTable = new CPDFTable(this, i_lPage, i_lRowCount, i_lColCount, i_lXPos, i_lYPos,
                            i_lXStep, i_lYStep);
  if (pRetTable)
    m_TableMap[lID] = pRetTable;
  o_lID = lID;
  return pRetTable;
}

void CPDFDocument::DeleteTable(long i_lID)
{
  CPDFTable *pTable = NULL;
  PDFPtrIterator  iterator = m_TableMap.find(i_lID);
  if (iterator != m_TableMap.end())
  {
    pTable = ((CPDFTable *)(iterator->second));
    _ASSERT(pTable);
    delete pTable;
    m_TableMap.erase(iterator);
  }
}

void CPDFDocument::DeleteTable(CPDFTable *i_pTable)
{
  CPDFTable *pTable = NULL;
  PDFPtrIterator  iterator;
  for (iterator = m_TableMap.begin(); iterator != m_TableMap.end(); iterator++)
  {
    pTable = ((CPDFTable *)(iterator->second));
    _ASSERT(pTable);
    if (pTable == i_pTable)
    {
      delete pTable;
      m_TableMap.erase(iterator);
      break;
    }
  }
}

CPDFTable *CPDFDocument::RemoveTable(long i_lID)
{
  CPDFTable *pTable = NULL;
  PDFPtrIterator  iterator = m_TableMap.find(i_lID);
  if (iterator != m_TableMap.end())
  {
    pTable = ((CPDFTable *)(iterator->second));
    _ASSERT(pTable);
    m_TableMap.erase(iterator);
    return pTable;
  }
  return NULL;
}

CPDFTable *CPDFDocument::RemoveTable(CPDFTable *i_pTable)
{
  CPDFTable *pTable = NULL;
  PDFPtrIterator  iterator;
  for (iterator = m_TableMap.begin(); iterator != m_TableMap.end(); iterator++)
  {
    pTable = ((CPDFTable *)(iterator->second));
    _ASSERT(pTable);
    if (pTable == i_pTable)
    {
      m_TableMap.erase(iterator);
      return pTable;
    }
  }
  return NULL;
}

long CPDFDocument::GetTableCount()
{
  return (long)m_TableMap.size();
}

CPDFTable *CPDFDocument::GetTable(long i_lID)
{
  PDFPtrIterator  iterator;
  iterator = m_TableMap.find(i_lID);
  if (iterator != m_TableMap.end())
    return (CPDFTable *)(iterator->second);
  return NULL;
}

long CPDFDocument::GetTableID(CPDFTable *i_pTable)
{
  CPDFTable *pTable = NULL;
  PDFPtrIterator  iterator;
  for (iterator = m_TableMap.begin(); iterator != m_TableMap.end(); iterator++)
  {
    pTable = ((CPDFTable *)(iterator->second));
    _ASSERT(pTable);
    if (pTable == i_pTable)
    {
      return (iterator->first);
    }
  }
  return -1;
}

CPDFBaseFont *CPDFDocument::MakeFont(const char *i_pczFontName)
{
  if (!i_pczFontName)
  {
    CPDFBaseFont *pFont = GetActualFont();
    if (!pFont)
      return NULL;
    i_pczFontName = pFont->GetFontName();
    if (!i_pczFontName)
      return NULL;
  }

  char czTemp[100];

  //TO DO:
  //check whether already exist
  long lID = CheckExistFont(i_pczFontName);
  if (lID >= 0)
  {
    m_lActualFontID = lID;
    return GetFont(lID);
  }

  //TODO: which font should be created???

  m_lActualFontID = GetFontsCount();
  CPDFBaseFont *pNewFont = CPDFBaseFont::MakeFont(i_pczFontName, m_lActualFontID,
                                                  GetActualFontStyle(), GetActualCodePage(), this);
  if (!pNewFont)
  {
    //if (GetActualCodePage() != lDefCodePage)
    {
      pNewFont = CPDFType0Font::MakeFont(i_pczFontName, m_lActualFontID, GetActualFontStyle(),
                                                    GetActualCodePage(), this);
    }/* else
    {
      pNewFont = CPDFTrueTypeFont::MakeFont(i_pczFontName, m_lActualFontID, GetActualFontStyle(),
                                                    GetActualCodePage(), this);
    }*/
    if (!pNewFont)
    {
      pNewFont = CPDFBaseFont::MakeFont(pczSTANDART_FONT_NAME[0][0], m_lActualFontID,
                                                  GetActualFontStyle(), GetActualCodePage(), this);
      if (!pNewFont)
        return NULL;
    }
  }

  //SetActualFontSize(PDFConvertDimensionFromDefault(i_lSize));
  m_vctFonts.push_back(pNewFont);

  PDFPage *pPage = GetPage(GetCurrentPage());
  if (pPage)
    PDFAddFontToPage(GetStruct(), m_lActualFontID, GetCurrentPage() - 1);

  this->bTextObj = true_a; //TO DO: check it, is it OK???

  sprintf(czTemp, "/%s %.5f %s\n", pNewFont->GetFontResName(), PDFConvertDimensionToDefault(GetActualFontSize()), czFONTNAMESIZE);
  PDFWriteToPageStream(GetStruct(), czTemp, strlen(czTemp), GetCurrentPage() - 1);

  return pNewFont;
}

long CPDFDocument::CheckExistFont(const char *i_pczFontName)
{
  long i;
  CPDFBaseFont *pFont;

  for (i = 0; i < GetFontsCount(); i++)
  {
    pFont = GetFont(i);
    if (!pFont)
      return NOT_USED;
    if (pFont->Compare(i_pczFontName, GetActualFontID(), GetActualFontStyle(), GetActualCodePage()))
      return i;
  }
  return NOT_USED;
}




CPDFBaseFont *CPDFDocument::SetActualFont(const char *i_pFontName, float i_fSize, long i_lCodePage)
{
  if (i_lCodePage > 0)
    SetActualCodePage(i_lCodePage);
  SetActualFontSize(PDFConvertDimensionFromDefault(i_fSize));
  return MakeFont(i_pFontName);
}

CPDFBaseFont *CPDFDocument::SetActualFont(const char *i_pFontName)
{
  return MakeFont(i_pFontName);
}

CPDFBaseFont *CPDFDocument::SetDefaultFont(const char *i_pFontName, float i_fSize)//TO DO: check it
{
  long lCFI = GetActualFontID();
  float fFS = GetActualFontSize();
  long lFF = GetActualFontStyle();
  CPDFBaseFont *pFont = NULL;

  SetActualFontStyle(CPDFBaseFont::ePDFStandard);
  pFont = MakeFont(i_pFontName);

  SetActualFontID(lCFI);
  SetActualFontSize(fFS);
  SetActualFontStyle(lFF);

  return pFont;
}

CPDFBaseFont *CPDFDocument::SetDefaultFont(const char *i_pFontName) //TO DO: check it
{
  long lCFI = GetActualFontID();
  long lFF = GetActualFontStyle();
  CPDFBaseFont *pFont = NULL;

  SetActualFontStyle(CPDFBaseFont::ePDFStandard);

  pFont = MakeFont(i_pFontName);

  SetActualFontID(lCFI);
  SetActualFontStyle(lFF);

  return pFont;
}

bool_a CPDFDocument::SetDefaultFontSize(long i_fSize)
{
  //fDefFontSize = i_fSize;
  return true_a;
}

bool_a CPDFDocument::SetFontSize(float i_fSize)
{
  SetActualFontSize(PDFConvertDimensionFromDefault(i_fSize));
  return true_a;
}

float CPDFDocument::GetFontSize()
{
  return PDFConvertDimensionToDefault(GetActualFontSize());
}

CPDFFontDescriptor *CPDFDocument::AddFontDescriptor(CPDFFontDescriptor *i_pFontDesc, const char *i_pczFontName)
{
  if (!i_pFontDesc || !i_pczFontName)
    return NULL;

  CPDFFontDescriptor *pDesc = GetFontDescriptor(i_pczFontName);
  if (pDesc)
    return pDesc;

  m_mapFontDesc.insert(pair <string, CPDFFontDescriptor *>(i_pczFontName, i_pFontDesc));

  return i_pFontDesc;
}

CPDFFontDescriptor *CPDFDocument::GetFontDescriptor(const char *i_pczFontName)
{
  if (!i_pczFontName)
    return NULL;

  CPDFFontDescriptor *pDesc;

  map<string, CPDFFontDescriptor *>::iterator iter = m_mapFontDesc.find(i_pczFontName);
  if (iter == m_mapFontDesc.end())
    return NULL;

  pDesc = iter->second;
  if (pDesc && 0 == strcmp(pDesc->GetFontName(), i_pczFontName))
    return pDesc;

  return NULL;
}

float CPDFDocument::GetConvertFontSize()
{
  return GetActualFontSize();
}

long CPDFDocument::GetActualFontHeight()
{
  CPDFBaseFont *pFont = GetActualFont();
  if (pFont)
    return pFont->GetFontHeight(GetActualFontSize());
  return NOT_USED;
}

long CPDFDocument::GetFontHeight(long i_lID)
{
  CPDFBaseFont *pFont = GetFont(i_lID);
  if (pFont)
    return pFont->GetFontHeight(GetActualFontSize());
  return NOT_USED;
}

long CPDFDocument::GetActualFontAscentHeight()
{
  CPDFBaseFont *pFont = GetActualFont();
  if (pFont)
    return pFont->GetFontAscentHeight(GetActualFontSize());
  return NOT_USED;
}

long CPDFDocument::GetActualFontDescentHeight()
{
  CPDFBaseFont *pFont = GetActualFont();
  if (pFont)
    return pFont->GetFontDescentHeight(GetActualFontSize());
  return NOT_USED;
}

CPDFBaseFont *CPDFDocument::GetActualFont()
{
  if (GetActualFontID() >= 0 && GetActualFontID() < GetFontsCount())
    return m_vctFonts.at(GetActualFontID());
  return NULL;
}

long CPDFDocument::GetActualFontID()
{
  return m_lActualFontID;
}

void CPDFDocument::SetActualFontID(long i_lID)
{
  if (i_lID < 0 || i_lID >= GetFontsCount())
    return;
  m_lActualFontID = i_lID;
}

float CPDFDocument::GetActualFontSize()
{
  return m_fActualFontSize;
}

void CPDFDocument::SetActualFontSize(float i_fSize)
{
  m_fActualFontSize = i_fSize;
}

long CPDFDocument::GetActualFontStyle()
{
  return m_lActualFontStyle;
}

void CPDFDocument::SetActualFontStyle(long i_lStyle)
{
  m_lActualFontStyle = i_lStyle;
}

long CPDFDocument::GetActualCodePage()
{
  return m_lActualCodePage;
}

bool_a CPDFDocument::SetActualCodePage(long i_lCodePage)
{
  //TODO: check codepage ????
  m_lActualCodePage = i_lCodePage;
  if (!MakeFont())
    return false_a;
  return true_a;
}




bool_a CPDFDocument::SetFontStyle(short i_sFlag)
{
  CPDFBaseFont *pFont = GetActualFont();
  if (!pFont)
    return false_a;

  long lStyle = GetActualFontStyle();

  switch (i_sFlag)
  {
  case FA_STANDARD:
    SetActualFontStyle(CPDFBaseFont::ePDFStandard);
    break;
  case FA_BOLD:
    SetActualFontStyle(lStyle |= CPDFBaseFont::ePDFBold);
    break;
  case FA_NOBOLD:
    if (lStyle & CPDFBaseFont::ePDFBold)
      SetActualFontStyle(lStyle &= ~CPDFBaseFont::ePDFBold);
    break;
  case FA_ITALIC:
    SetActualFontStyle(lStyle |= CPDFBaseFont::ePDFItalic);
    break;
  case FA_NOITALIC:
    if (lStyle & CPDFBaseFont::ePDFItalic)
      SetActualFontStyle(lStyle &= ~CPDFBaseFont::ePDFItalic);
    break;
  case FA_UNDERLINE:
    SetActualFontStyle(lStyle |= CPDFBaseFont::ePDFUnderline);
    break;
  case FA_NOUNDERLINE:
    if (lStyle & CPDFBaseFont::ePDFUnderline)
      SetActualFontStyle(lStyle &= ~CPDFBaseFont::ePDFUnderline);
    break;
  case FA_DOUBLE_UNDERLINE:
    SetActualFontStyle(lStyle |= CPDFBaseFont::ePDFDoubleUnderline);
    break;
  case FA_NODOUBLE_UNDERLINE:
    if (lStyle & CPDFBaseFont::ePDFDoubleUnderline)
      SetActualFontStyle(lStyle &= ~CPDFBaseFont::ePDFDoubleUnderline);
    break;
  case FA_STRIKEOUT:
    SetActualFontStyle(lStyle |= CPDFBaseFont::ePDFStrikeout);
    break;
  case FA_NOSTRIKEOUT:
    if (lStyle & CPDFBaseFont::ePDFStrikeout)
      SetActualFontStyle(lStyle &= ~CPDFBaseFont::ePDFStrikeout);
    break;
  }

  pFont = MakeFont(pFont->GetFontName());
  if (!pFont)
    return false_a;
  return true_a;
}

CPDFBaseFont *CPDFDocument::GetFont(long i_lID)
{
  if (i_lID >= 0 && i_lID < GetFontsCount())
    return m_vctFonts.at(i_lID);
  return NULL;
}

CPDFImage *CPDFDocument::CreateImage(const char *i_pImage)
{
  long lIndex;
  if (!PDFReadImage(this, i_pImage, &lIndex))
    return NULL;
  if (lIndex < 0)
    return NULL;
  //m_vctImages.push_back(pImage);
  return GetImage(lIndex);
}

bool_a CPDFDocument::AddImageToPage(long i_lPage, CPDFImage *i_pImage, long i_lX, long i_lY,
                                    long i_lWidth, long i_lHeight, long i_lRotate)
{
  long lPage = GetCurrentPage();
  bool_a bRet = false_a;
  if (lPage != i_lPage)
    PDFSetCurrentPage(this, i_lPage);
  bRet = PDFAddImageWH(this, i_pImage->filepath, i_lX, i_lY, i_lRotate, i_lWidth, i_lHeight);
  if (lPage != i_lPage)
    PDFSetCurrentPage(this, lPage);
  return bRet;
}

bool_a CPDFDocument::AddImageToPage(long i_lPage, long i_lIndex, long i_lX, long i_lY,
                                    long i_lWidth, long i_lHeight, long i_lRotate)
{
  long lPage = GetCurrentPage();
  bool_a bRet = false_a;
  if (lPage != i_lPage)
    PDFSetCurrentPage(this, i_lPage);
  bRet = PDFAddImageWHIndex(this, i_lIndex, i_lX, i_lY, i_lRotate, i_lWidth, i_lHeight);
  if (lPage != i_lPage)
    PDFSetCurrentPage(this, lPage);
  return bRet;
}

CPDFImage *CPDFDocument::GetImage(long i_lID)
{
  if (i_lID >= 0 && i_lID < GetImagesCount())
    return m_vctImages.at(i_lID);
  return NULL;
}


bool_a CPDFDocument::CreateLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                                long i_lTop, long i_lRight, long i_lBottom, float i_fZoom,
                                const char *i_pLocation)
{
  i_fZoom /= 100.0;
  long lAnnot = PDFCreateLinkAnnotationFile(this, i_pczName, i_lPage, i_eDest, i_lLeft,
                                            i_lTop, i_lRight, i_lBottom, i_fZoom, i_pLocation);
  if (lAnnot < 0)
    return false_a;
  return true_a;
}

bool_a CPDFDocument::CreateLink(const char *i_pczName, const char *i_pLocation)
{
  long lAnnot = PDFCreateLinkAnnotationURI(this, i_pczName, i_pLocation);
  if (lAnnot < 0)
    return false_a;
  return true_a;
}

bool_a CPDFDocument::CreateLink(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                                long i_lTop, long i_lRight, long i_lBottom, float i_fZoom)
{
  i_fZoom /= 100.0;
  long lAnnot = PDFCreateLinkAnnotationDest(this, i_pczName, i_lPage, i_eDest, i_lLeft,
                                            i_lTop, i_lRight, i_lBottom, i_fZoom);
  if (lAnnot < 0)
    return false_a;
  return true_a;
}

bool_a CPDFDocument::ChangeLinkDest(const char *i_pczName, long i_lPage, eDESTSPEC i_eDest, long i_lLeft,
                                    long i_lTop, long i_lRight, long i_lBottom, float i_fZoom)
{
  return PDFChangeAnnotationDest(this, i_pczName, i_lPage, i_eDest, i_lLeft,
                                  i_lTop, i_lRight, i_lBottom, i_fZoom);

}

CPDFAnnotation *CPDFDocument::CreateForm(const char *i_pczName, eDICTFIELDTYPE i_eType, rec_a i_recPosSize, long i_lPage)
{
  CPDFAnnotation *pAnnot;

  pAnnot = new CPDFAnnotation(this, i_eType, i_pczName, i_recPosSize, i_lPage);
  if (!pAnnot)
    return pAnnot;

  m_vctAnnots.push_back((PDFAnnotation *)pAnnot);

  return pAnnot;
}

CPDFAnnotation *CPDFDocument::GetForm(long i_lID)
{
  if (i_lID >= 0 && i_lID < GetFormCount())
    return (CPDFAnnotation *)m_vctAnnots.at(i_lID);
  return NULL;
}

bool_a CPDFDocument::MoveForm(long i_lDeltaX, long i_lDeltaY)
{
  long i;
  bool_a bRet = false_a;
  PDFAnnotation *pAnnot;
  for (i = 0; i < m_vctAnnots.size(); i++)
  {
    pAnnot = m_vctAnnots[i];
    if (!pAnnot)
      return bRet;
    if (eAnnotWidget == pAnnot->eAnnotType)
    {
      pAnnot->recAnnot.lBottom += i_lDeltaY;
      pAnnot->recAnnot.lLeft += i_lDeltaX;
      pAnnot->recAnnot.lRight += i_lDeltaX;
      pAnnot->recAnnot.lTop += i_lDeltaY;
    }
  }

  for (i = 0; i < GetTemplateCount(); i++)
  {
    if (m_TemplatesPDF[i]->MoveFormField(i_lDeltaX, i_lDeltaY))
      bRet = true_a;
  }

  return bRet;
}

bool_a CPDFDocument::MoveForm(long i_lDeltaX, long i_lDeltaY, const char *i_pName)
{
  long i;
  bool_a bRet = false_a;
  PDFAnnotation *pAnnot;
  if (!i_pName)
    return false_a;
  for (i = 0; i < m_vctAnnots.size(); i++)
  {
    pAnnot = m_vctAnnots[i];
    if (!pAnnot)
      return false_a;
    if (!pAnnot->pczAnnotName)
      return false_a;
    if (eAnnotWidget == pAnnot->eAnnotType && strcmp(pAnnot->pczAnnotName, i_pName) == 0)
    {
      pAnnot->recAnnot.lBottom += i_lDeltaY;
      pAnnot->recAnnot.lLeft += i_lDeltaX;
      pAnnot->recAnnot.lRight += i_lDeltaX;
      pAnnot->recAnnot.lTop += i_lDeltaY;
    }
  }

  for (i = 0; i < GetTemplateCount(); i++)
  {
    if (m_TemplatesPDF[i]->MoveFormField(i_pName, i_lDeltaX, i_lDeltaY))
      bRet = true_a;
  }

  return bRet;
}

bool_a CPDFDocument::HasWidget()
{
  for(long i = 0; i < m_vctAnnots.size(); i++)
  {
    if (eAnnotWidget == m_vctAnnots[i]->eAnnotType)
      return true_a;
  }
  return false_a;
}

bool_a CPDFDocument::HasLink(const char *i_pczName)
{
  if (!i_pczName)
    return false_a;
  long lRet = PDFGetAnnotationIndex(this, i_pczName);
  if (lRet >= 0)
    return true_a;
  return false_a;
}

long CPDFDocument::GetOutlineEntryCount()
{
  return (long)m_OEMap.size();
}

CPDFOutlineEntry *CPDFDocument::GetOutlineEntry(long i_lIndex)
{
  PDFPtrIterator iterator = m_OEMap.find(i_lIndex);
  if (iterator != m_OEMap.end())
    return (CPDFOutlineEntry *)(iterator->second);
  return NULL;
}

CPDFOutlineEntry *CPDFDocument::CreateOutlineEntry(CPDFOutlineEntry *i_pFollowThis, bool_a i_bSubEntry,
                                  bool_a i_bOpen, long i_lPage, char *i_pTitle,
                                  eDESTSPEC i_eDest, long i_lLeft, long i_lTop,
                                  long i_lRight, long i_lBottom, float i_fZoom,
                                  float i_fR, float i_fG, float i_fB, short i_sFkey, long i_lFont)
{
  CPDFOutlineEntry *pOutlineEntry = NULL;
  pOutlineEntry = new CPDFOutlineEntry(this);

  if (i_lFont < 0)
    i_lFont = GetActualFontID();

  i_fZoom /= 100.0;
  pOutlineEntry->AddOutlineEntry(i_pFollowThis, i_bSubEntry, i_bOpen, i_lPage, i_pTitle, i_eDest,
                                  PDFConvertDimensionToDefaultL(i_lLeft), PDFConvertDimensionToDefaultL(i_lTop),
                                  PDFConvertDimensionToDefaultL(i_lRight), PDFConvertDimensionToDefaultL(i_lBottom),
                                  i_fZoom, i_fR, i_fG, i_fB, i_sFkey, i_lFont);

  m_OEMap[(long)m_OEMap.size()] = pOutlineEntry;
  return pOutlineEntry;
}

CPDFOutlineEntry *CPDFDocument::CreateOutlineEntry(long i_lPage, char *i_pLevel, char *i_pTitle, long i_lLeft, long i_lTop,
                                                   float i_fZoom, float i_fR, float i_fG, float i_fB, short i_sTitleStyle, long i_lFont)
{
  CPDFOutlineEntry *pTempOE = NULL;;

  long lLevelCount = 1, lTemp = 0;
  long *pLevels = NULL;
  char *pTemp = i_pLevel;
  bool_a bSub = false_a;
  if (strlen(pTemp) > 0)
  {
    while (pTemp && *pTemp != '\0')
    {
      pTemp = strchr(pTemp, '.');
      if (pTemp && *(pTemp++) != '\0')
        lLevelCount++;
    }
  }
  else
    lLevelCount = 0;
  if (lLevelCount > 0)
  {
    pLevels = new long[lLevelCount];
    short i = 0;
    if (!pLevels)
      return false_a;
    pTemp = i_pLevel;
    while (pTemp && *pTemp != '\0')
    {
      pLevels[i] = atol(pTemp);
      pTemp = strchr(pTemp, '.');
      if (pTemp)
        pTemp++;
      i++;
    }
    pTempOE = (CPDFOutlineEntry *)pFirstOutlineEntry;
    if (pTempOE)
    {
      for (i = 0; i < lLevelCount; i++)
      {
        lTemp = pLevels[i];
        lTemp--;
        while (pTempOE && lTemp > 0)
        {
          if (pTempOE->GetNext())
            pTempOE = pTempOE->GetNext();
          else
            break;
          lTemp--;
        }
        if (i == lLevelCount - 1)
          break;

        if (pTempOE->GetFirstSub())
        {
          pTempOE = pTempOE->GetFirstSub();
        }
        else
        {
          bSub = true_a;
        }
      }
    }
  }
  else
    pTempOE = (CPDFOutlineEntry *)pLastOutlineEntry;

  if (i_fZoom < 0)
    i_fZoom = 0;
  if (lLevelCount > 0)
    delete pLevels;
  return CreateOutlineEntry(pTempOE, bSub, false_a, i_lPage, i_pTitle, eDestLeftTopZoom,
                            i_lLeft, i_lTop, 0, 0, i_fZoom, i_fR, i_fG, i_fB, i_sTitleStyle, i_lFont);
}

bool_a CPDFDocument::DrawDashedLine(float i_fXFrom, float i_fYFrom, float i_fXTo, float i_fYTo)
{
  bool_a bRet = false_a;
  bRet = PDFSetDashedLine(this);
  if (bRet)
    bRet = PDFMoveTo(this, i_fXFrom, i_fYFrom);
  if (bRet)
    bRet = PDFLineTo(this, i_fXTo, i_fYTo);
  if (bRet)
    bRet = PDFSetSolidLine(this);
  return bRet;
}

bool_a CPDFDocument::DrawDashedLine(long i_lXFrom, long i_lYFrom, long i_lXTo, long i_lYTo)
{
  bool_a bRet = false_a;
  bRet = PDFSetDashedLine(this);
  if (bRet)
    bRet = PDFMoveToL(this, i_lXFrom, i_lYFrom);
  if (bRet)
    bRet = PDFLineToL(this, i_lXTo, i_lYTo);
  if (bRet)
    bRet = PDFSetSolidLine(this);
  return bRet;
}

bool_a CPDFDocument::DrawLine(float i_fXFrom, float i_fYFrom, float i_fXTo, float i_fYTo)
{
  bool_a bRet = false_a;
  bRet = PDFMoveTo(this, i_fXFrom, i_fYFrom);
  if (bRet)
    bRet = PDFLineTo(this, i_fXTo, i_fYTo);
  return bRet;
}

bool_a CPDFDocument::DrawLine(long i_lXFrom, long i_lYFrom, long i_lXTo, long i_lYTo)
{
  bool_a bRet = false_a;
  bRet = PDFMoveToL(this, i_lXFrom, i_lYFrom);
  if (bRet)
    bRet = PDFLineToL(this, i_lXTo, i_lYTo);
  return bRet;
}

bool_a CPDFDocument::DrawRectangle(rec_a i_Rec, bool_a i_bFill)
{
  return PDFRectangleL(this, i_Rec, i_bFill);
}

bool_a CPDFDocument::DrawRectangle(frec_a i_Rec, bool_a i_bFill)
{
  return PDFRectangle(this, i_Rec, i_bFill);
}

bool_a CPDFDocument::SetLineWidth(long i_lWidth)
{
  return PDFSetLineWidthL(this, i_lWidth);
}

bool_a CPDFDocument::SetLineWidth(float i_fWidth)
{
  return PDFSetLineWidth(this, i_fWidth);
}

bool_a CPDFDocument::SetLineColor(float i_fRed, float i_fGreen, float i_fBlue)
{
  char czTemp[101] = {0};
  sprintf(czTemp, "%.4f %.4f %.4f RG\n", i_fRed, i_fGreen, i_fBlue);
  if (!PDFWriteToPageStream(this, czTemp, strlen(czTemp), GetCurrentPage() - 1))
    return false_a;
  return true_a;
}

bool_a CPDFDocument::SetLineColor(CPDFColor& i_rColor)
{
  const char *pTemp = i_rColor.GetColorStringWithStrokingOperator();
  if (!PDFWriteToPageStream(this, (char *)pTemp, strlen(pTemp), GetCurrentPage() - 1))
    return false_a;
  return true_a;
}

bool_a CPDFDocument::AddTextToPage(long i_lPage, long i_lX, long i_lY,
                                    long i_lRotate, char *i_pString)
{
  long lPage = GetCurrentPage();
  bool_a bRet = false_a;

  if (lPage != i_lPage)
    PDFSetCurrentPage(this, i_lPage);
  PDFBgnText(this, 0);


  bRet = PDFAddText(this, i_lX, i_lY, i_lRotate, i_pString);


  PDFEndText(this);
  if (lPage != i_lPage)
   PDFSetCurrentPage(this, lPage);

  return bRet;
}

bool_a CPDFDocument::ContinueTextToPage(long i_lPage, char *i_pczString)
{
  long lPage = GetCurrentPage();
  bool_a bRet = false_a;

  if (lPage != i_lPage)
   PDFSetCurrentPage(this, i_lPage);
  PDFBgnText(this, 0);

  bRet = PDFContinueText(this, i_pczString);

  PDFEndText(this);
  if (lPage != i_lPage)
   PDFSetCurrentPage(this, lPage);

  return bRet;
}

bool_a CPDFDocument::SetTextColor(float i_fRed, float i_fGreen, float i_fBlue)
{
  char czTemp[101] = {0};
  sprintf(czTemp, "%.4f %.4f %.4f rg\n", i_fRed, i_fGreen, i_fBlue);
  if (!PDFWriteToPageStream(this, czTemp, strlen(czTemp), GetCurrentPage() - 1))
    return false_a;
  return true_a;
}

bool_a CPDFDocument::SetTextColor(CPDFColor& i_rColor)
{
  const char *pTemp = i_rColor.GetColorStringWithNonStrokingOperator();
  if (!PDFWriteToPageStream(this, (char *)pTemp, strlen(pTemp), GetCurrentPage() - 1))
    return false_a;
  return true_a;
}

bool_a CPDFDocument::SetLeading(float i_fLeading)
{
  return PDFSetLeading(this, i_fLeading);
}

bool_a CPDFDocument::NextLine()
{
  return PDFNextLine(this);
}

bool_a CPDFDocument::IsFlush()
{
  return m_bFlush;
}

bool_a CPDFDocument::Flush()
{
  if (m_bFlush)
    return true_a;
  CPDFDocument *pDoc = this;
  PDFPtrIterator  iterator;
  CPDFTextBox *pTextBox = NULL;
  CPDFFrame *pFrame = NULL;
  CPDFTable *pTable = NULL;
  CPDFPage *pPage = NULL;
  long i;

  if (!CheckTable())
    return false_a;

  SetCanCopyOverflowLines(true_a);
  if (!CheckPageOverflow(1))
    return false_a;

  if (!CheckFrameOverlap())
    return false_a;

  if (!CheckHeaderFooter())
    return false_a;

  for (iterator = m_FrameMap.begin(); iterator != m_FrameMap.end(); iterator++)
  {
    pFrame = (CPDFFrame *)(iterator->second);
    _ASSERT(pFrame);
    if (pFrame/* && !pFrame->GetPrevTextBox()*/)
      pFrame->CopyOverflowLines();
  }

  for(i = 0; i < GetPageCount(); i++)
  {
    pPage = ((CPDFPage *)(m_vctPages[i]));
    _ASSERT(pPage);
    if (pPage)
      pPage->Flush();
  }
  for (iterator = m_TextBoxMap.begin(); iterator != m_TextBoxMap.end(); iterator++)
  {
    pTextBox = (CPDFTextBox *)(iterator->second);
    _ASSERT(pTextBox);
    if (pTextBox)
      pTextBox->Flush();
  }
  for (iterator = m_FrameMap.begin(); iterator != m_FrameMap.end(); iterator++)
  {
    pFrame = (CPDFFrame *)(iterator->second);
    _ASSERT(pFrame);
    if (pFrame)
      pFrame->Flush();
  }
  for (iterator = m_TableMap.begin(); iterator != m_TableMap.end(); iterator++)
  {
    pTable = (CPDFTable *)(iterator->second);
    _ASSERT(pTable);
    if (pTable)
      pTable->Flush();
  }

  if (!FlushLayers())
    return false_a;

  if (!FlushDoc())
    return false_a;

  if (GetTemplateCount() > 0)
  {
    if (!CopyObjectToTemplate())
      return false_a;
  }

  PDFEndDoc((PDFDocPTR *)&pDoc);
  m_bFlush = true_a;
  return true_a;
}

bool_a CPDFDocument::FlushDoc()
{
  long i, lObjCount;
  CPDFBaseFont *pFont;
  CPDFImage *pImg;

  this->lActualSize = 0;

  this->lActualSize += PDFWriteFormatedBufToDocStream(this, "%%PDF-%ld.%ld\n", this->lMajorLevel, this->lMinorLever);

  this->pObjOffset[0] = 0;
  this->pObjOffset[1] = this->lActualSize;
  lObjCount = 1;
  this->pObjIndex[PDF_Catalog] = lObjCount++;
  this->pObjIndex[PDF_Outlines] = lObjCount++;
  this->pObjIndex[PDF_Pages] = lObjCount++;
  this->pObjIndex[PDF_ProcSet] = lObjCount++;
  if (HasWidget())
    this->pObjIndex[PDF_Annots] = lObjCount++;

  for (i = 0; i < GetPageCount(); i++)
  {
    //JuKo TODO: make it like font below
    if (this->m_vctPages[i])
    {
      this->m_vctPages[i]->lObjIndex = lObjCount++;  // Page object for this page 
      this->m_vctPages[i]->lParent = this->pObjIndex[PDF_Pages];
      this->m_vctPages[i]->lContIndex = lObjCount++;
      PDFEndPage(this, i + 1);        // close page if not already 
    }
  }

  for(i = 0; i < GetFontsCount(); i++)
  {
    pFont = GetFont(i);
    if (pFont)
    {
      pFont->SetEmbedded(GetFontEmbedded());
      pFont->AssignObjNum(lObjCount);
    }
  }
  for(i = 0; i < GetImagesCount(); i++)
  {
    pImg = GetImage(i);
    if (pImg)
      pImg->lObjIndex = lObjCount++;
  }
  for(i=0; i< GetFormCount(); i++)
  {
    CPDFAnnotation *pAnnot = GetForm(i);
    if (pAnnot)
      pAnnot->AssignObjNum(lObjCount);
  }

  for (i = 0; i < GetLayerCount();i++)
  {
    CPDFLayer *pLayer = GetLayer(i);
    if (pLayer)
      pLayer->SetObjNum(lObjCount++);
  }

  // now do outline entries 
  if(this->lOutlineEntriesCount)
    PDFserializeOutlineEntries(&lObjCount, this->pFirstOutlineEntry, this->pLastOutlineEntry);
  this->pObjIndex[PDF_Info] = lObjCount++;
///////////////////////////
  PDFWriteCatalogObject(this, this->pObjIndex[PDF_Catalog]);
  this->pObjOffset[this->pObjIndex[PDF_Catalog] + 1] = this->lActualSize;

  PDFWriteOutlinesObject(this, this->pObjIndex[PDF_Outlines]);
  this->pObjOffset[this->pObjIndex[PDF_Outlines] + 1] = this->lActualSize;
  
  PDFWritePagesObject(this, this->pObjIndex[PDF_Pages]);
  this->pObjOffset[this->pObjIndex[PDF_Pages] + 1] = this->lActualSize;

  //TODO: fix ProcSet, now it is not properly used
  PDFWriteProcSetArray(this, this->pObjIndex[PDF_ProcSet]);
  this->pObjOffset[this->pObjIndex[PDF_ProcSet] + 1] = this->lActualSize;

  if (HasWidget())
  {
    string strStream;
    if (!GetAcroFormObject(strStream))
      return false_a;
    this->lActualSize += PDFWriteBufToDocStream(this, strStream.c_str(), strStream.size());
    this->pObjOffset[this->pObjIndex[PDF_Annots] + 1] = this->lActualSize;
  }

  for(i = 0; i < GetPageCount(); i++)
  {
    if(this->m_vctPages[i])
    {
      PDFWritePageObject(this, this->m_vctPages[i]);
      this->pObjOffset[this->m_vctPages[i]->lObjIndex + 1] = this->lActualSize;
      PDFWriteContentsFromMemory(this, this->m_vctPages[i]);
      this->pObjOffset[this->m_vctPages[i]->lContIndex + 1] = this->lActualSize;
    }
  }

  for(i = 0; i < GetFontsCount(); i++)
  {
    pFont = GetFont(i);
    if (pFont)
    {
      vector<string> vctStreams;
      if (!pFont->GetEntireFontStream(vctStreams))
        continue;
      for(size_t ii = 0; ii < vctStreams.size(); ii++)
      {
        this->lActualSize += PDFWriteBufToDocStream(this, vctStreams[ii].c_str(), vctStreams[ii].size());
        long lTemp = atol(vctStreams[ii].c_str());//TODO: make it better
        this->pObjOffset[lTemp + 1] = this->lActualSize;
      }
    }
  }

  for(i = 0; i < GetImagesCount(); i++)
  {
    pImg = GetImage(i);
    PDFWriteImage(this, pImg, &lObjCount);
  }

  for(i = 0; i < GetFormCount(); i++)
  {
    CPDFAnnotation *pAnnot = GetForm(i);
    if (pAnnot)
    {
      vector<string> vctStreams;
      if (!pAnnot->GetEntireFormStream(vctStreams))
        continue;
      for(size_t ii = 0; ii < vctStreams.size(); ii++)
      {
        this->lActualSize += PDFWriteBufToDocStream(this, vctStreams[ii].c_str(), vctStreams[ii].size());
        long lTemp = atol(vctStreams[ii].c_str());//TODO: make it better
        this->pObjOffset[lTemp + 1] = this->lActualSize;
      }
    }
//    PDFWriteAnnotations(this, this->m_vctAnnots[i]);
  }

  for (i = 0; i < GetLayerCount();i++)
  {
    string strStream;
    CPDFLayer *pLayer = GetLayer(i);
    if (pLayer->GetLayerStream(strStream))
    {
      this->lActualSize += PDFWriteBufToDocStream(this, strStream.c_str(), strStream.size());
      this->pObjOffset[pLayer->GetObjNum() + 1] = this->lActualSize;
    }
  }

  if(this->lOutlineEntriesCount)
    PDFWriteOutlineEntries(this, this->pFirstOutlineEntry, this->pLastOutlineEntry);

  PDFWriteInfo(this, this->pObjIndex[PDF_Info]);
  this->pObjOffset[this->pObjIndex[PDF_Info] + 1] = this->lActualSize;

  if (Encrypt())
  {
    this->pObjIndex[PDF_Encrypt] = lObjCount++;
    PDFWriteEncryption(this, this->pObjIndex[PDF_Encrypt]);
  }

  PDFWriteXrefTrailer(this, lObjCount);

  return true_a;
}

bool_a CPDFDocument::GetAcroFormObject(string &io_Stream)
{
  ostringstream strBuffer("");
  io_Stream.clear();

  strBuffer << this->pObjIndex[PDF_Annots] << " 0 obj << ";

  if (HasWidget())
  {
    long i;
    CPDFBaseFont *pFont;

    strBuffer << "/Fields [";
    for(i = 0; i < m_vctAnnots.size(); i++)
    {
      if (eAnnotWidget == m_vctAnnots[i]->eAnnotType)
      {
        strBuffer << m_vctAnnots[i]->lObjIndex[0] << " 0 R ";
      }
    }
    strBuffer << "] ";

    strBuffer << "/DA(/Helv 0 Tf 0 g ) ";

    strBuffer << "/DR <</Font << ";
    for(i = 0; i < m_vctAnnots.size(); i++)
    {
      if (eAnnotWidget == m_vctAnnots[i]->eAnnotType && m_vctAnnots[i]->lFont >= 0 && m_vctAnnots[i]->lFont < GetFontsCount())
      {
        pFont = GetFont(m_vctAnnots[i]->lFont);
        strBuffer << "/" << pFont->GetFontResName() << " " << pFont->GetFontObjNum() << " 0 R ";
      }
    }
    strBuffer << ">> >> ";
  }

  strBuffer << ">> endobj\n";

  io_Stream.assign(strBuffer.str());

  return true_a;
}

bool_a CPDFDocument::FlushLayers()
{
  long i, lLayer, j;
  CPDFPage *pPage;
  string strTemp;
  if (m_vctLayers.size() <= 0)
    return true_a;

  std::vector<CPDFLayer *>::iterator iterLayer;
  //flush it first
  for (iterLayer = m_vctLayers.begin(), lLayer = 0; iterLayer != m_vctLayers.end(); iterLayer++, lLayer++)
  {
    CPDFLayer *pLayer = (CPDFLayer *)(*iterLayer);
    if (!pLayer)
      continue;
    if (!pLayer->Flush())
      return false_a;
  }
  for (iterLayer = m_vctLayers.begin(), lLayer = 0; iterLayer != m_vctLayers.end(); iterLayer++, lLayer++)
  {
    CPDFLayer *pLayer = (CPDFLayer *)(*iterLayer);
    if (!pLayer)
      continue;

    for (i = 0; i < pLayer->GetPageCount(); i++)
    {
      if (i >= GetPageCount())
      {
        if (!CreatePage(i + 1, pLayer->GetSection(i+1)))
          return false_a;
      }
      pPage = pLayer->GetPage(i + 1);
      if (!pPage)
        return false_a;

      for (j = 0 ; j < pPage->GetFontsCount(); j++)
      {
        PDFAddFontToPage(GetStruct(), pPage->GetFontID(j), i);
      }
      for (j = 0 ; j < pPage->GetImagesCount(); j++)
      {
        PDFAddFontToPage(GetStruct(), pPage->GetImageID(j), i);
      }
      for (j = 0 ; j < pPage->GetAnnotsCount(); j++)
      {
        PDFAddFontToPage(GetStruct(), pPage->GetAnnotID(j), i);
      }

      if (pLayer->GetParent().size() == 0)//take one root layers
      {
        strTemp.clear();
        if (pLayer->GetPageContent(i + 1, strTemp))
        {
          PDFWriteToPageStream(this, (char *)strTemp.c_str(), strTemp.size(), i);
        }
      }
    }

  }

  return true_a;
}

CPDFLayer *CPDFDocument::CreateLayer(const char *i_pName)
{
  if (!i_pName)
    return NULL;
  CPDFLayer *pLayer = GetLayer(i_pName);
  char czTemp[100];
  if (pLayer)
    return pLayer;

  m_strLayer = i_pName;
  pLayer = new CPDFLayer(this);
  sprintf(czTemp, "%s%ld", czLayerName, (long)m_vctLayers.size());
  m_vctLayers.push_back(pLayer);

  if (!pLayer->Initialize(i_pName))
  {
    delete pLayer;
    return NULL;
  }

  pLayer->SetLayerName(i_pName);
  pLayer->SetLayerResName(czTemp);

  //pLayer->EnableCompression();
  pLayer->FlushToFile(false_a);

  CPDFBaseFont *pFont = this->GetActualFont();
  pLayer->SetActualFont(pFont->GetFontBaseName(), this->GetFontSize(), pFont->GetFontCodePage());

  //tab width
  rec_a recPosSize;
  recPosSize.lBottom = 0;
  recPosSize.lLeft = 0;

  CPDFSection *pSect = this->GetSection(1);

  recPosSize.lRight = pSect->GetPaperWidth();
  recPosSize.lTop = pSect->GetPaperHeight();

  pLayer->SetCellMargins(pLayer->ConvertTwipsToPoints(22),
                            pLayer->ConvertTwipsToPoints(22),
                            pLayer->ConvertTwipsToPoints(22),
                            pLayer->ConvertTwipsToPoints(22));

  pLayer->SetTabDefault(this->GetTabDefaultSize(), this->GetTabDefaultDecimal());

  pLayer->CreatePage(1, pSect);

  //pLayer->SetPassword("Master", "User", 0);


  return pLayer;
}

void CPDFDocument::EndLayer()
{
  m_strLayer.assign("");
}

CPDFLayer *CPDFDocument::GetActiveLayer(bool_a i_bReturnThis)
{
  CPDFLayer *pLayer = NULL;
  if (m_strLayer.empty())
  {
    if (i_bReturnThis)
      pLayer = (CPDFLayer *)this;
  } else {
    pLayer = GetLayer(m_strLayer.c_str());
    if (!pLayer && i_bReturnThis)
      pLayer = (CPDFLayer *)this;
  }
  return pLayer;
}

CPDFLayer *CPDFDocument::GetLayer(const char *i_pName)
{
  if (!i_pName)
    return NULL;
  std::vector<CPDFLayer *>::iterator iterLayer;
  for (iterLayer = m_vctLayers.begin(); iterLayer != m_vctLayers.end(); iterLayer++)
  {
    CPDFLayer *pLayer = (CPDFLayer *)(*iterLayer);
    pLayer = pLayer->GetLayer(i_pName);
    if (pLayer)
      return pLayer;
  }
  return NULL;
}

CPDFLayer *CPDFDocument::GetLayer(long i_lIndex)
{
  if (i_lIndex < 0 || i_lIndex >= m_vctLayers.size())
    return NULL;
  return (CPDFLayer *)m_vctLayers.at(i_lIndex);
}

bool_a CPDFDocument::GetLayerOrderStream(std::string &o_stream)
{
  string strTemp;
  o_stream.clear();
  for (long i = 0;i < GetLayerCount(); i++)
  {
    CPDFLayer *pLayer = GetLayer(i);
    if (pLayer && pLayer->GetParent().size() == 0)//take one root layers
    {
      pLayer->GetLayerOrderStream(strTemp);
      o_stream.append(strTemp);
      strTemp.clear();
    }
  }
  return true_a;
}

bool_a CPDFDocument::GetOCPropertiesStream(std::string &o_stream)
{
  string strTemp;
  string strON(""), strOFF(""), strLocked("");
  char czTemp[100];
  map<string, string> mapGroup;
  map<string, string>::iterator iterGroup;
  o_stream.clear();

  if (GetLayerCount() > 0)
  {
    //o_stream.append("/OCProperties << ");

    o_stream.append("/OCGs [ ");
    for (long i = 0;i < GetLayerCount(); i++)
    {
      CPDFLayer *pLayer = GetLayer(i);
      if (pLayer)
      {
        sprintf(czTemp, "%ld 0 R ", pLayer->GetObjNum());
        o_stream.append(czTemp);
        //build state
        if (pLayer->GetDefaultState())
          strON.append(czTemp);
        else
          strOFF.append(czTemp);
        if (pLayer->IsLocked())
          strLocked.append(czTemp);
        if (pLayer->GetGroup().size())
        {
          iterGroup = mapGroup.find(pLayer->GetGroup());
          if (iterGroup != mapGroup.end())
          {
            iterGroup->second.append(czTemp);
          } else {
            mapGroup.insert(pair<string, string>(pLayer->GetGroup(), czTemp));
          }
        }
      }

    }
    o_stream.append("] ");

    o_stream.append("/D << ");

    o_stream.append("/Order [ ");

    GetLayerOrderStream(strTemp);
    o_stream.append(strTemp);

    o_stream.append("] ");

    if (strON.size())
    {
      o_stream.append("/ON [ ");
      o_stream.append(strON);
      o_stream.append("] ");
    }
    if (strOFF.size())
    {
      o_stream.append("/OFF [ ");
      o_stream.append(strOFF);
      o_stream.append("] ");
    }
    if (strLocked.size())
    {
      o_stream.append("/Locked [ ");
      o_stream.append(strLocked);
      o_stream.append("] ");
    }
    if (mapGroup.size())
    {
      o_stream.append("/RBGroups [ ");
      for (iterGroup = mapGroup.begin(); iterGroup != mapGroup.end(); iterGroup++)
      {
        o_stream.append("[ ");
        o_stream.append(iterGroup->second);
        o_stream.append("] ");
      }
      o_stream.append("] ");
    }

    o_stream.append(">> ");

    //o_stream.append(">> ");
  }

  return true_a;
}

long CPDFDocument::GetLayerCount()
{
  return m_vctLayers.size();
}

bool_a CPDFDocument::CloseAllObject()
{
  long i;
  for (i = 0; i < GetPageCount(); i++)
  {
    if (!PDFEndPage(this, i + 1))
      return false_a;
  }
  return true_a;
}

long CPDFDocument::CalculatePageCount()
{
  long i, lPages = 0, lPos;
  for (i = 0; i < GetTemplateCount(); i++)
  {
    lPos = m_TemplatesPDF[i]->GetPDFPosition();
    if (!lPos)
    {
      lPages += m_TemplatesPDF[i]->GetPageCount();
    }
    else if (lPos > 0)
    {
      if (lPages >= lPos)
      {
        if (lPages <= (lPos + m_TemplatesPDF[i]->GetPageCount() - 1))
          lPages = lPos + m_TemplatesPDF[i]->GetPageCount() - 1;
      }
      else if (lPages < lPos)
      {
        lPages += m_TemplatesPDF[i]->GetPageCount();
      }
    }
  }
  lPos = GetPageCount();
  lPages = lPages > lPos ? lPages : lPos;
  return lPages;
}

bool_a CPDFDocument::CopyObjectToTemplate()
{
  if (!GetTemplateCount())
    return false_a;
  long i, j;
  bool_a bF = false_a;

  CPDFPage *pPage = NULL;
  CPDFBaseFont *pFont = NULL;
  CPDFImage *pImg = NULL;
  CPDFLayer *pLayer = NULL;

  long lPages = CalculatePageCount();

  i = GetTemplateCount();
  if (m_TemplatesPDF[0]->GetPDFPosition() < 0)
  {
    bF = true_a;
    CPDFTemplate *pPDFTempl = new CPDFTemplate(this);
    if (!pPDFTempl)
      return false_a;
    if (!pPDFTempl->Read(m_TemplatesPDF[0]->GetFileName()))
    {
      delete pPDFTempl;
      return false_a;
    }
    pPDFTempl->SetPDFPosition(0);
    m_TemplatesPDF.insert(m_TemplatesPDF.begin(), pPDFTempl);
  }
  i = GetTemplateCount();

  if (!m_TemplatesPDF[0]->ApplyLayer())
    return false_a;

  long lLastPage = -1;
  for (i = 1; i < GetTemplateCount(); i++)
  {
    if (!m_TemplatesPDF[i]->ApplyLayer())
      return false_a;

      if (m_TemplatesPDF[i]->GetPDFPosition() < 0)
      {
        if (lLastPage < 0)
          lLastPage = m_TemplatesPDF[0]->GetPageCount();
        for (j = 1; j <= lPages; j++)
        {
          if (1 == i && 1 == j && 0 == strcmp(m_TemplatesPDF[i]->GetFileName(), m_TemplatesPDF[0]->GetFileName()))
          {
            if (bF)
              lLastPage = 0;
            continue;
          }
          m_TemplatesPDF[i]->SetPDFPosition(j);
          if (!m_TemplatesPDF[0]->AddPDFTemplate(m_TemplatesPDF[i], lPages))
            return false_a;
        }
        m_TemplatesPDF[i]->SetPDFPosition(-1);
      }
      else
      {
    //if (m_TemplatesPDF[i]->GetPDFPosition() < 0)
        if (lLastPage >= 0 && (lLastPage < m_TemplatesPDF[i]->GetPDFPosition() || !m_TemplatesPDF[i]->GetPDFPosition()))
        {
          m_TemplatesPDF[i]->SetPDFPosition(lLastPage + 1);
          lLastPage = m_TemplatesPDF[i]->GetPDFPosition() + m_TemplatesPDF[i]->GetPageCount() - 1;
        }
    {
      if (!m_TemplatesPDF[0]->AddPDFTemplate(m_TemplatesPDF[i], lPages))
        return false_a;
    }
      }
  }

  for (i = 0; i < GetFontsCount(); i++)
  {
    pFont = GetFont(i);
    if (!m_TemplatesPDF[0]->AssignTemplateObjNum(pFont))
      return false_a;
  }
  for (i = 0; i < GetImagesCount(); i++)
  {
    pImg = GetImage(i);
    if (!m_TemplatesPDF[0]->AddImageToTemplate(pImg))
      return false_a;
  }
  for (i = 0; i < GetLayerCount(); i++)
  {
    pLayer = GetLayer(i);
    if (!m_TemplatesPDF[0]->AddLayerToTemplate(pLayer))
      return false_a;
  }

  for (i = 0; i < GetFormCount(); i++)
  {
    CPDFAnnotation *pAnnot = GetForm(i);
    if (!m_TemplatesPDF[0]->AddFormToTemplate(pAnnot))
      return false_a;
  }

  for (i = 0; i < GetPageCount(); i++)
  { 
    pPage = GetPage(i + 1);
    if (!pPage)
      return false_a;
    if (!m_TemplatesPDF[0]->AddPageToTemplate(pPage, this->streamFilterList))
      return false_a;
    //for (j = 0; j < pPage->GetFontsCount(); j++)
    //{
    //  pFont = GetFont(pPage->GetFontID(j));
    //  if (!m_TemplatesPDF[0]->AddFontToPage(i + 1, pFont))
    //    return false_a;
    //}
  }

  if (GetLayerCount())
  {
    string strOC;
    if (!GetOCPropertiesStream(strOC))
      return false_a;
    if (!m_TemplatesPDF[0]->AddLayersToCatalog(strOC))
      return false_a;
  }

  return true_a;
}

bool_a CPDFDocument::CopyResourcesToTemplate()
{
  if (!GetTemplateCount())
    return false_a;
  long i;
  //CPDFPage *pPage = NULL;
  CPDFBaseFont *pFont = NULL;
  //CPDFImage *pImg = NULL;
  //CPDFLayer *pLayer = NULL;

  for (i = 0; i < GetFontsCount(); i++)
  {
    pFont = GetFont(i);
    if (!m_TemplatesPDF[0]->AddFontToTemplate(pFont))
      return false_a;
  }
  //for (i = 0; i < GetImagesCount(); i++)
  //{
  //  pImg = GetImage(i);
  //  if (!m_TemplatesPDF[0]->AddImageToTemplate(pImg))
  //    return false_a;
  //}
  //for (i = 0; i < GetLayerCount(); i++)
  //{
  //  pLayer = GetLayer(i);
  //  if (!m_TemplatesPDF[0]->AddLayerToTemplate(pLayer))
  //    return false_a;
  //}

  //for (i = 0; i < GetPageCount(); i++)
  //{
  //  pPage = GetPage(i + 1);
  //  if (!pPage)
  //    return false_a;
  //  if (!m_TemplatesPDF[0]->AddPageToTemplate(pPage, this->streamFilterList))
  //    return false_a;
  //  //for (j = 0; j < pPage->GetFontsCount(); j++)
  //  //{
  //  //  pFont = GetFont(pPage->GetFontID(j));
  //  //  if (!m_TemplatesPDF[0]->AddFontToPage(i + 1, pFont))
  //  //    return false_a;
  //  //}
  //}

  //if (GetLayerCount())
  //{
  //  string strOC;
  //  if (!GetOCPropertiesStream(strOC))
  //    return false_a;
  //  if (!m_TemplatesPDF[0]->AddLayersToCatalog(strOC))
  //    return false_a;
  //}

  return true_a;
}

void CPDFDocument::DeleteAll()
{
  PDFPtrIterator iterator;
  CPDFPage *pPage;
  CPDFBaseFont *pFont;
  CPDFOutlineEntry *pOE;
  CPDFImage *pImage;
  CPDFTable *pTable;
  CPDFSection *pSect;
  CPDFFrame *pFrame;
  long i;
//  for (iterator = m_TextBoxMap.begin(); iterator != m_TextBoxMap.end(); iterator++)
//    delete ((CPDFTextBox *)(iterator->second));
//  m_TextBoxMap.clear();

  for(i = 0; i < GetPageCount(); i++)
  {
    pPage = ((CPDFPage *)(m_vctPages[i]));
    _ASSERT(pPage);
    if (pPage)
      delete pPage;
  }
  m_vctPages.clear();
  for(i = 0; i < GetFontsCount(); i++)
  {
    pFont = m_vctFonts.at(i);
    _ASSERT(pFont);
    if (pFont)
      delete pFont;
  }
  m_vctFonts.clear();
  for (iterator = m_OEMap.begin(); iterator != m_OEMap.end(); iterator++)
  {
    pOE = (CPDFOutlineEntry *)(iterator->second);
    _ASSERT(pOE);
    if (pOE)
      delete pOE;
  }
  m_OEMap.clear();
  for (i = 0; i < GetImagesCount(); i++)
  {
    pImage = m_vctImages.at(i);
    _ASSERT(pImage);
    if (pImage)
      delete pImage;
  }
  m_vctImages.clear();
  for (iterator = m_TableMap.begin(); iterator != m_TableMap.end(); iterator++)
  {
    pTable = (CPDFTable *)(iterator->second);
    _ASSERT(pTable);
    if (pTable)
      delete pTable;
  }
  m_TableMap.clear();
  for (iterator = m_SectionMap.begin(); iterator != m_SectionMap.end(); iterator++)
  {
    pSect = (CPDFSection *)(iterator->second);
    _ASSERT(pSect);
    if (pSect)
      delete pSect;
  }
  m_SectionMap.clear();
  for (iterator = m_FrameMap.begin(); iterator != m_FrameMap.end(); iterator++)
  {
    pFrame = (CPDFFrame *)(iterator->second);
    _ASSERT(pFrame);
    if (pFrame)
      delete pFrame;
  }
  m_FrameMap.clear();
  PDFDeleteStream(&(this->pDocStream));

  for (i = 0; i < GetTemplateCount(); i++)
  {
    if (m_TemplatesPDF[i])
      delete m_TemplatesPDF[i];
  }
  m_TemplatesPDF.clear();

  //map<string, CPDFFontDescriptor *>::iterator iter;
  //for (iter = m_mapFontDesc.begin(); iter != m_mapFontDesc.end(); iter++)
  //{
  //  CPDFFontDescriptor *pDesc;
  //  pDesc = iter->second;
  //  delete pDesc;
  //}
  //m_mapFontDesc.clear();

  std::vector<CPDFLayer *>::iterator iterLayer;
  for (iterLayer = m_vctLayers.begin(); iterLayer != m_vctLayers.end(); iterLayer++)
  {
    CPDFLayer *pLayer = (CPDFLayer *)(*iterLayer);
    delete pLayer;
  }
  m_vctLayers.clear();
}

bool_a CPDFDocument::AppendDocument(CPDFDocument *i_pDoc)
{
  if (!i_pDoc)
    return false_a;
  map <long, long> FontMap;
  map <long, long> ImageMap;
  PDFPtrIterator  iterator;
  CPDFTextBox *pTextBox = NULL, *pTextBoxOld = NULL;
  CPDFBaseFont *pFont = NULL, *pNewFont = NULL;
  CPDFImage *pImage = NULL, *pNewImage = NULL;
  CPDFPage *pPage = NULL, *pNewPage = NULL;
  CPDFTable *pTableOld = NULL, *pTable = NULL;
  long lLast = GetLastPage();
  long lTB = -1, lTable = -1;
  long j, i;

  for (i = 1; i <= (long)i_pDoc->GetPageCount(); i++)
  {
    pPage = i_pDoc->GetPage(i);
    pNewPage = CreatePage(lLast + i, pPage->GetWidth(), pPage->GetHeight(), pPage->GetRotation());
    if (!pNewPage)
      return false_a;
    for (j = 0; j < (long)i_pDoc->GetFontsCount(); j++)
    {
      pFont = i_pDoc->GetFont(j);
      if (pFont)
      {
        pNewFont = SetActualFont(pFont->GetFontName());
        if (!pNewFont)
          return false_a;
      }
    }
    for (j = 0; j < (long)i_pDoc->GetImagesCount(); j++)
    {
      pImage = i_pDoc->GetImage(j);
      if (pImage)
      {
        pNewImage = CreateImage(pImage->filepath);
        if (!pImage)
          return false_a;
        ImageMap[pImage->GetImageID()] = pNewImage->GetImageID();
      }
    }
    if (!(pNewPage->CopyPageStream(pPage->GetPageStream())))
      return false_a;
  }
  for (i = 0; i < (long)i_pDoc->GetTextBoxCount(); i++)
  {
    pTextBoxOld = i_pDoc->GetTextBox(i);
    pTextBox = pTextBoxOld->Clone();
    if (pTextBox)
    {
      if (lTB < 0)
        lTB = (long)m_TextBoxMap.size();
      m_TextBoxMap[(long)m_TextBoxMap.size()] = pTextBox;
      pTextBox->SetPage(lLast + pTextBoxOld->GetPageNumber());
      pTextBox->SetDocument(this);
      pTextBox->SetPrevTextBox(NULL);
      pTextBox->SetNextTextBox(NULL);
    }
  }
  for (i = 0; i < (long)i_pDoc->GetTableCount(); i++)
  {
    pTableOld = i_pDoc->GetTable(i);
    pTable = pTableOld->Clone();
    if (pTable)
    {
      if (lTable < 0)
        lTable = (long)m_TableMap.size();
      m_TableMap[(long)m_TableMap.size()] = pTable;
      pTable->SetPage(lLast + pTableOld->GetPageNumber());
      pTable->SetDocument(this);
    }
  }
  if (lTable >= 0)
    for (i = lTable; i < (long)GetTableCount(); i++)
    {
      pTable = GetTable(i);
      pTable->ChangeFontID(&FontMap);
      pTable->ChangeImageID(&ImageMap);
    }
  if (lTB >= 0)
    for (i = lTB; i < (long)GetTextBoxCount(); i++)
    {
      pTextBox = GetTextBox(i);
      pTextBox->ChangeFontID(&FontMap);
      pTextBox->ChangeImageID(&ImageMap);
    }
  return true_a;
}

bool_a CPDFDocument::AppendPageFromDocument(CPDFDocument *i_pDoc, long i_lPage)
{
  i_pDoc;
  i_lPage;
  return false_a;
/*
  if (!i_pDoc)
    return false_a;
  CPDFPage *pPage = i_pDoc->GetPage(i_lPage);
  if (!pPage)
    return false_a;
  CPDFPage *pNewPage = NULL;
  long lLast = GetLastPage() + 1;
  long lFont = GetActualFontID();
  float fSize = GetActualFontSize();
  long lFlags = GetActualFontStyle();
  CPDFFont *pFont = NULL, *pNewFont = NULL;
  CPDFImage *pImage = NULL;

  pNewPage = CreatePage(lLast, pPage->GetWidth(), pPage->GetHeight(), pPage->GetRotation());
  if (!pNewPage)
    return false_a;

  for (long i = 0; i < pPage->GetFontsCount(); i++)
  {
    pFont = i_pDoc->GetFont( pPage->GetFontID(i) );
    if (pFont)
    {
      SetActualFontStyle(pFont->GetStyle());
      pNewFont = SetActualFont(pFont->GetName());
      if (!pNewFont)
        return false_a;
    }
  }
  for (i = 0; i < pPage->GetImagesCount(); i++)
  {
    pImage = i_pDoc->GetImage(pPage->GetImageID(i));
    if (pImage)
    {
      if (!CreateImage(pImage->GetFileName()))
        return false_a;
    }
  }
//  if (!(pNewPage->CopyBuffer(pPage->GetBuffer())))
//    return false_a;

//  PDFOutlineEntry *pFirstOutlineEntry;
//  PDFOutlineEntry *pLastOutlineEntry;

  SetActualFontID(lFont);
  SetActualFontSize(fSize);
  SetActualFontStyle(lFlags);

  return true_a;
*/
}

void CPDFDocument::SetCellMargins(long i_lLeft, long i_lRight, long i_lTop, long i_lBottom)
{
  m_lLeftCellMargin = i_lLeft;
  m_lRightCellMargin = i_lRight;
  m_lTopCellMargin = i_lTop;
  m_lBottomCellMargin = i_lBottom;
}

long CPDFDocument::GetLetterSpace()
{
  return this->lLetterSpace;
}

long CPDFDocument::GetTabDefaultSize()
{
  return this->lDefTabSpace;
}

char CPDFDocument::GetTabDefaultDecimal()
{
  return this->cTabDecimalType;
}

bool_a CPDFDocument::SetTabDefault(long i_lTabSize, char i_cTabDec)
{
  this->lDefTabSpace = i_lTabSize;
  this->cTabDecimalType = i_cTabDec;
  return true_a;
}

CPDFSection *CPDFDocument::GetSection(long i_lPageNum)
{
  PDFPtrIterator iterator;
  CPDFSection *pSect = NULL;
  for (iterator = m_SectionMap.begin(); iterator != m_SectionMap.end(); iterator++)
  {
    pSect = (CPDFSection *)(iterator->second);
    _ASSERT(pSect);
    if (pSect)
    {
      if (pSect->GetFirstPage() <= i_lPageNum && (i_lPageNum <= pSect->GetLastPage() || pSect->GetLastPage() == -1))
        return pSect;
    }
  }
  return NULL;
}

bool_a CPDFDocument::FillPageBreak()
{
  long i, lPages = 0, lPos;
  for (i = 0; i < GetTemplateCount(); i++)
  {
    lPos = m_TemplatesPDF[i]->GetPDFPosition();
    if (!lPos)
    {
      lPages += m_TemplatesPDF[i]->GetPageCount();
    }
    else if (lPos > 0)
    {
      if (lPages >= lPos)
      {
        if (lPages <= (lPos + m_TemplatesPDF[i]->GetPageCount() - 1))
          lPages = lPos + m_TemplatesPDF[i]->GetPageCount() - 1;
      }
      else if (lPages < lPos)
      {
        lPages += m_TemplatesPDF[i]->GetPageCount();
      }
    }
  }

  CPDFPage *pPage = GetPage(GetCurrentPage());
  if (!pPage)
    return false_a;
  CPDFTextBox *pTextBox = pPage->GetTextArea();
  if (!pTextBox)
    return false_a;
  if (!pTextBox->AddFillBreak(lPages))
    return false_a;

  return true_a;
}

bool_a CPDFDocument::CheckTable()
{
  PDFPtrIterator iterator;
  CPDFTable *pTable;
  CPDFTextBox *pTextBox;
  for (iterator = m_TableMap.begin(); iterator != m_TableMap.end(); iterator++)
  {
    pTable = (CPDFTable *)(iterator->second);
    _ASSERT(pTable);
    if (pTable)
    {
      pTextBox = pTable->GetInTextBox();
      if (pTextBox)
      {
        if (!pTextBox->RemoveEmptyLineAfterTable())
          return false_a;
      }
    }
  }

  CPDFPage *pPage;
  vector<PDFPage *>::iterator vctIter;
  for(vctIter = m_vctPages.begin();vctIter != m_vctPages.end(); vctIter++)
  {
    pPage = (CPDFPage *)*vctIter;
    _ASSERT(pPage);
    if (pPage)
    {
      pTextBox = pPage->GetTextArea();
      if (pTextBox)
        if (!pTextBox->UpdateYPosSpaces())
          return false_a;
      if (pPage->HasHeader())
      {
        pTextBox = pPage->GetHeader();
        if (pTextBox)
          if (!pTextBox->UpdateYPosSpaces())
            return false_a;
      }
      if (pPage->HasFooter())
      {
        pTextBox = pPage->GetFooter();
        if (pTextBox)
          if (!pTextBox->UpdateYPosSpaces())
            return false_a;
      }
    }
  }

  return true_a;
}

bool_a CPDFDocument::CheckHeaderFooter()
{
  long i, lPages = 0;

  lPages = CalculatePageCount();

  for (i = GetPageCount(); i < lPages; i++)
    if (!InsertPageBreak())
      return false_a;
  return true_a;
}

bool_a CPDFDocument::LoadTemplatePDF(const char *i_pFileName, long i_lPos, const char *pPassword)
{
  CPDFTemplate *pPDFTempl = new CPDFTemplate(this);
  if (!pPDFTempl)
    return false_a;
  if (pPassword)
    pPDFTempl->SetTeplatePassword(pPassword);
  if (!pPDFTempl->Read(i_pFileName))
  {
    delete pPDFTempl;
    return false_a;
  }
  pPDFTempl->SetPDFPosition(i_lPos);
  CPDFLayer *pActualLayer = GetActiveLayer(false_a);
  if (pActualLayer)
    pPDFTempl->SetLayer(pActualLayer);
  m_TemplatesPDF.push_back(pPDFTempl);
  return true_a;
}

void CPDFDocument::SetTemplatePassword(long i_lIndex, const char *i_pczPassword)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return;
  return m_TemplatesPDF[i_lIndex]->SetTeplatePassword(i_pczPassword);
}

const char *CPDFDocument::GetTemplatePassword(long i_lIndex)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return NULL;
  return m_TemplatesPDF[i_lIndex]->GetTeplatePassword();
}

bool_a CPDFDocument::IsTemplate()
{
  return GetTemplateCount() > 0;;
}

long CPDFDocument::GetTemplateCount()
{
  return m_TemplatesPDF.size();
}

long CPDFDocument::GetTemplatePageCount(long i_lIndex)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return 0;
  return m_TemplatesPDF[i_lIndex]->GetPageCount();
}

long CPDFDocument::GetTemplateStrBlockCount(long i_lIndex, long i_lPage)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return 0;
  return m_TemplatesPDF[i_lIndex]->GetStrBlockCount(i_lPage);
}

const wchar_t *CPDFDocument::GetTemplateStrBlock(long i_lIndex, long i_lPage, long i_lBlock)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return NULL;
  return m_TemplatesPDF[i_lIndex]->GetStrBlock(i_lPage, i_lBlock);
}

const char *CPDFDocument::GetTemplateStrBlockFontName(long i_lIndex, long i_lPage, long i_lBlock)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return NULL;
  return m_TemplatesPDF[i_lIndex]->GetStrBlockFontName(i_lPage, i_lBlock);
}

float CPDFDocument::GetTemplateStrBlockFontSize(long i_lIndex, long i_lPage, long i_lBlock)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return 0;
  return m_TemplatesPDF[i_lIndex]->GetStrBlockFontSize(i_lPage, i_lBlock);
}

void CPDFDocument::GetTemplateStrBlockPosition(long i_lIndex, long i_lPage, long i_lBlock, float &o_fX, float &o_fY)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return;
  return m_TemplatesPDF[i_lIndex]->GetStrBlockPosition(i_lPage, i_lBlock, o_fX, o_fY);
}

long CPDFDocument::GetTemplateImageCount(long i_lIndex, long i_lPage)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return 0;
  return m_TemplatesPDF[i_lIndex]->GetImageCount(i_lPage);
}

const char *CPDFDocument::GetTemplateImageName(long i_lIndex, long i_lPage, long i_lImageIndex)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return NULL;
  return m_TemplatesPDF[i_lIndex]->GetImageName(i_lPage, i_lImageIndex);
}

bool_a CPDFDocument::CreateFormOnImage(long i_lIndex, const char *i_pImageName)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return false_a;
  long lPageCount = GetTemplatePageCount(i_lIndex);
  for (long i = 0; i < lPageCount; i++)
  {
    long lImageCount = GetTemplateImageCount(i_lIndex, i);
    const char *pImageName = NULL;
    for (long j = 0; j < lImageCount; j++)
    {
      pImageName = GetTemplateImageName(i_lIndex, i, j);
      if (!pImageName)
        return false_a;
      if (!strcmp(pImageName, i_pImageName))
      {
        PDFCTM M = m_TemplatesPDF[i_lIndex]->GetImageCTM(i, j);

        rec_a anotRec;
        anotRec.lLeft = PDFConvertDimensionFromDefault(floor(M.m_fE));
        anotRec.lBottom = PDFConvertDimensionFromDefault(floor(M.m_fF));
        anotRec.lRight = PDFConvertDimensionFromDefault(ceil(M.m_fE + M.m_fA));
        anotRec.lTop = PDFConvertDimensionFromDefault(ceil(M.m_fF + M.m_fD));
        CPDFAnnotation *pTestAnnot = CreateForm(pImageName, eDICTFIELDTYPE::eText, anotRec, i+1);
        pTestAnnot->SetValue(pImageName);
        pTestAnnot->SetBorderWidth(2);
        pTestAnnot->SetShortDesc(pImageName);
        pTestAnnot->GetBGColor().SetColor(0.75f, 0.666656f, 1.0f);
        pTestAnnot->GetBorderColor().SetColor(0.282349f, 0.84314f, 0.509811f);
      }
    }
  }
  return true_a;
}

bool_a CPDFDocument::BuildStrBlock(long i_lIndex, CPDFTemplate::eStrBlockType i_eType,
                                   CPDFStrBlockControl *i_pStrBlockCtrl)
{
  if (i_lIndex < 0 || i_lIndex >= GetTemplateCount())
    return false_a;
  return m_TemplatesPDF[i_lIndex]->BuildStrBlock(i_eType, i_pStrBlockCtrl);
}

long CPDFDocument::GetFormFieldCount()
{
  if (!GetTemplateCount())
    return NOT_USED;

  long i, lRet = 0;
  for (i = 0; i < GetTemplateCount(); i++)
  {
    lRet += m_TemplatesPDF[i]->GetFormFieldCount();
  }

  return lRet;
}

const char *CPDFDocument::GetFormFieldName(long i_lIndex, bool_a i_bSorted)
{
  if (!GetTemplateCount())
    return NULL;

  long i, lTemp = 0;
  for (i = 0; i < GetTemplateCount(); i++)
  {
    lTemp = m_TemplatesPDF[i]->GetFormFieldCount();
    if (lTemp > i_lIndex)
      return GetFormFieldName(i_lIndex, i_bSorted);
    i_lIndex -= lTemp;
  }

  return NULL;
}

bool_a CPDFDocument::SetFormField(const char *i_pName, const char *i_pValue)
{
  if (!GetTemplateCount())
    return false_a;

  long i;
  bool_a bRet = false_a;
  for (i = 0; i < GetTemplateCount(); i++)
  {
    if (m_TemplatesPDF[i]->SetFormField(i_pName, i_pValue))
      bRet = true_a;
  }

  return bRet;
}

bool_a CPDFDocument::SetFormFieldFont(const char *i_pName, const char *i_pFontName,
                                      float i_fFontSize, long i_lCodePage)
{
  if (!GetTemplateCount())
    return false_a;

  short sIndex;
  long lCodePage = GetActualCodePage();
  long lFieldCodePage = lDefCodePage;
  const char *pFontName = NULL;
  long lCFI = GetActualFontID();
  float fFS = GetActualFontSize();
  long lFF = GetActualFontStyle();
  CPDFBaseFont *pFont = NULL, *pFieldFont = NULL;

  SetActualFontStyle(CPDFBaseFont::ePDFStandard);

  long i;
  bool_a bRet = false_a;
  for (i = 0; i < GetTemplateCount(); i++)
  {
    SetActualFontStyle(CPDFBaseFont::ePDFStandard);

    pFieldFont = m_TemplatesPDF[i]->GetFormFieldFont(i_pName);
    if (pFieldFont)
    {
      lFieldCodePage = pFieldFont->GetFontCodePage();
      pFontName = pFieldFont->GetFontBaseName();
    }
    if (i_lCodePage > 0)
      lFieldCodePage = i_lCodePage;
    if (i_pFontName && PDFStrLenC(i_pFontName) > 0)
      pFontName = i_pFontName;

    sIndex = CPDFBaseFont::GetBaseFontIndex(pFontName);
    if (sIndex >= 0)
    {
      if (lFieldCodePage != lDefCodePage)
        pFontName = pczSTANDART_FONT_NAME[sIndex][1];
      switch (sIndex % 4)
      {
      case 1: SetActualFontStyle(CPDFBaseFont::ePDFBold); break;
      case 2: SetActualFontStyle(CPDFBaseFont::ePDFItalic); break;
      case 3: SetActualFontStyle(CPDFBaseFont::ePDFBold & CPDFBaseFont::ePDFItalic); break;
      }
    }


    pFont = SetActualFont(pFontName, i_fFontSize, lFieldCodePage);
    if (!pFont)
      return false_a;

    if (m_TemplatesPDF[i]->SetFormFieldFont(i_pName, pFont, i_fFontSize))
      bRet = true_a;
  }

  SetActualFontID(lCFI);
  SetActualFontSize(fFS);
  SetActualFontStyle(lFF);
  SetActualCodePage(lCodePage);

  return bRet;
}

bool_a CPDFDocument::CheckFormFieldOverflow(const char *i_pName, const char *i_pValue)
{
  if (!GetTemplateCount())
    return false_a;

  long i;
  for (i = 0; i < GetTemplateCount(); i++)
  {
    if (m_TemplatesPDF[i]->CheckFormFieldOverflow(i_pName, i_pValue))
      return true_a;
  }

  return false_a;
  //return m_TemplatesPDF[0]->CheckFormFieldOverflow(i_pName, i_pValue);
}

const char *CPDFDocument::GetFormFieldValue(const char *i_pName)
{
  if (!GetTemplateCount())
    return NULL;
  const char *pRet;
  long i;
  for (i = GetTemplateCount() - 1; i >= 0 ; i--)
  {
    pRet = m_TemplatesPDF[i]->GetFormFieldValue(i_pName);
    if (pRet)
      return pRet;
  }

  return NULL;
}

void CPDFDocument::SetFileID(char *i_pID)
{
  if (!i_pID)
    return;
  strncpy((char *)this->ucFileID, i_pID, 32);
}

unsigned char *CPDFDocument::GetEncryptionOwnerKey() const
{
  return (unsigned char *)this->ucO;
}

unsigned char *CPDFDocument::GetEncryptionUserKey() const
{
  return (unsigned char *)this->ucU;
}

int CPDFDocument::GetEncryptionPermissionKey() const
{
  return this->iPermission;
}

bool_a CPDFDocument::Encrypt()
{
  if (GetTemplateCount() > 0)
    return false_a;
  if (PDFStrLenUC(this->ucOwnerPassw) > 0)
    return true_a;

  return false_a;
}

bool_a CPDFDocument::CompressStream(string &io_Stream)
{
  if (io_Stream.empty())
    return true_a;
  long lRet;
  size_t lSize = io_Stream.size() + (io_Stream.size() * 0.001) + 16.0;
  unsigned char *pBuff = (unsigned char *)PDFMalloc(lSize * sizeof(unsigned char));
  if (!pBuff)
    return false_a;
  lRet = compress(pBuff, (uLong *)&lSize, (unsigned char *)io_Stream.c_str(), (uLong)io_Stream.size());
  if (Z_OK != lRet)
  {
    free(pBuff);
    return false_a;
  }
  io_Stream.assign((char *)pBuff, lSize);
  free(pBuff);
  return true_a;
}

bool_a CPDFDocument::UncompressStream(string &io_Stream)
{
  if (io_Stream.empty())
    return true_a;
  long lRet;
  size_t lSize = io_Stream.size() * 10;
  unsigned char *pBuff = (unsigned char *)PDFMalloc(lSize * sizeof(unsigned char));
  if (!pBuff)
    return false_a;

  lRet = uncompress(pBuff, (uLong *)&lSize, (unsigned char *)io_Stream.c_str(), (uLong)io_Stream.size());
  if (Z_OK != lRet)
  {
    lSize = io_Stream.size() * 100;
    PDFRealloc((void **)&pBuff, lSize, lSize - io_Stream.size() * 10);
    if (!pBuff)
      return false_a;
    if (Z_OK != uncompress(pBuff, (uLong *)&lSize, (unsigned char *)io_Stream.c_str(), (uLong)io_Stream.size()))
    {
      free(pBuff);
      return false_a;
    }
  }
  io_Stream.assign((char *)pBuff, lSize);
  free(pBuff);
  return true_a;
}

size_t CPDFDocument::PDFStrStrKey(string& i_strObj, size_t i_index, const char *i_pczKey)
{
  if (!i_pczKey || i_strObj.empty() || i_index >= i_strObj.size() || i_index < 0)
    return NOT_USED;

  basic_string <char>::size_type index, index1;
  long lDic, lObj;
  size_t ActPos;

  ActPos = i_index;
  index = i_strObj.find(i_pczKey, ActPos);
  while (index != -1)
  {
    lObj = lDic = 0;
    index1 = i_strObj.find(czBGNDIC, ActPos);
    while (index1 != -1 && index1 < index)
    {
      lDic++;
      index1++;
      index1 = i_strObj.find(czBGNDIC, index1);
    }

    index1 = i_strObj.find(czENDDIC, ActPos);
    while (index1 != -1 && index1 < index)
    {
      lDic--;
      index1++;
      index1 = i_strObj.find(czENDDIC, index1);
    }

    index1 = index + PDFStrLenC(i_pczKey);
    if (IsDelimiter(*(i_strObj.c_str() + index1)) || IsWhiteSpace(*(i_strObj.c_str() + index1)))
    {
      if (1 == lDic || 0 == lDic)
        return index;
    }

    ActPos = index + 1;
    index = i_strObj.find(i_pczKey, ActPos);
  }

  return NOT_USED;
}

size_t CPDFDocument::PDFStrStrKey(string& i_strObj, const char *i_pczKey)
{
  if (!i_pczKey || i_strObj.empty())
    return NOT_USED;

  basic_string <char>::size_type index, index1;
  long lDic, lObj;
  size_t ActPos;

  ActPos = 0;
  index = i_strObj.find(i_pczKey, ActPos);
  while (index != -1)
  {
    lObj = lDic = 0;
    index1 = i_strObj.find(czBGNDIC, ActPos);
    while (index1 != -1 && index1 < index)
    {
      lDic++;
      index1++;
      index1 = i_strObj.find(czBGNDIC, index1);
    }

    index1 = i_strObj.find(czENDDIC, ActPos);
    while (index1 != -1 && index1 < index)
    {
      lDic--;
      index1++;
      index1 = i_strObj.find(czENDDIC, index1);
    }

    index1 = index + PDFStrLenC(i_pczKey);
    if (IsDelimiter(*(i_strObj.c_str() + index1)) || IsWhiteSpace(*(i_strObj.c_str() + index1)))
    {
      if (1 == lDic || 0 == lDic)
        return index;
    }

    ActPos = index + 1;
    index = i_strObj.find(i_pczKey, ActPos);
  }

  return NOT_USED;
}

size_t CPDFDocument::PDFStrStrArrayKey(string& i_strObj, const char *i_pczKey)
{
  if (!i_pczKey || i_strObj.empty())
    return NOT_USED;

  basic_string <char>::size_type index, index1;
  string strArray;

  index = PDFStrStrKey(i_strObj, i_pczKey);
  if (index == -1)
    return NOT_USED;

  index += PDFStrLenC(i_pczKey);
  index1 = index;
  strArray = PDFGetStringUpToWhiteSpace(i_strObj, index1);

  if ('[' == strArray[0])
    return index;

  return NOT_USED;
}

size_t CPDFDocument::PDFStrStrNumKey(string& i_strObj, const char *i_pczKey)
{
  if (!i_pczKey || i_strObj.empty())
    return NOT_USED;

  basic_string <char>::size_type index, index1;
  string strNum;

  index = PDFStrStrKey(i_strObj, i_pczKey);
  if (index == -1)
    return NOT_USED;

  index += PDFStrLenC(i_pczKey);
  index1 = index;
  strNum = PDFGetStringUpToWhiteSpace(i_strObj, index1);

  if (PDFIsNumber(strNum))
    return index;

  return NOT_USED;
}

//bool_a CPDFDocument::PDFGetColorValue(std::string& i_strObj, const char *i_pczKey, CPDFColor& io_Color)
//{
//  if (!i_pczKey || i_strObj.empty())
//    return false_a;
//
//  basic_string <char>::size_type index, index1;
//  float fRet[4];
//
//  index = CPDFDocument::PDFStrStrArrayKey(i_strObj, i_pczKey);
//  if (!index == -1)
//    return false_a;
//  if ('[' != *pPos)
//    return rgb;
//  pPos++;
//  while (*pPos != ']')
//  {
//    if (*pPos == ' ' || *pPos == '\t' || *pPos == '\n' || *pPos == '\r')
//    {
//      pPos++;
//      continue;
//    }
//    if (sIndex > 2)
//      return rgb;
//    fRet[sIndex] = (float)atof(pPos);
//    if (fRet[sIndex] < 0)
//      return rgb;
//    sIndex++;
//    while (*pPos != ' ' && *pPos != ']')
//    {
//      if (*pPos != '.' && *pPos != ',' && (*pPos < '0' || *pPos > '9'))
//        return rgb;
//      pPos++;
//    }
//  }
//
//  rgb.fR = fRet[0];
//  rgb.fG = fRet[1];
//  rgb.fB = fRet[2];
//  return rgb;
//
//  return
//}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

CPDFSection::CPDFSection(CPDFDocument *i_pDoc, long i_lFirst, long i_lLast)
  :m_pDoc(i_pDoc)
{
  m_lLeftMargin = DEF_LEFT_MARGIN;
  m_lRightMargin = DEF_RIGHT_MARGIN;
  m_lTopMargin = DEF_TOP_MARGIN;
  m_lBottomMargin = DEF_BOTTOM_MARGIN;
  m_lColumns = DEF_COLUMNS;
  m_lColumnSpace = DEF_COLUMN_SPACE;
  m_lOrientation = DEF_ORIENTATION;
  m_lFirstPage = i_lFirst;
  m_lLastPage = i_lLast;
  m_lHdrMargin = DEF_HDR_MARGIN;
  m_lFtrMargin = DEF_FTR_MARGIN;
  m_lPaperWidth = DEF_PAPER_WIDTH;
  m_lPaperHeight = DEF_PAPER_HEIGHT;
  m_bForward = true_a;
}

CPDFSection::CPDFSection(CPDFSection *i_pSection)
{
  m_pDoc = i_pSection->GetDocument();
  m_lLeftMargin = i_pSection->GetLeftMargin();
  m_lRightMargin = i_pSection->GetRightMargin();
  m_lTopMargin = i_pSection->GetTopMargin();
  m_lBottomMargin = i_pSection->GetBottomMargin();
  m_lColumns = i_pSection->GetColumns();
  m_lColumnSpace = i_pSection->GetColumnSpace();
  m_lOrientation = i_pSection->GetOrientation();
  m_lFirstPage = i_pSection->GetLastPage() + 1;
  m_lLastPage = -1;
  m_lHdrMargin = i_pSection->GetHdrMargin();
  m_lFtrMargin = i_pSection->GetFtrMargin();
  m_lPaperWidth = i_pSection->GetPaperWidth();
  m_lPaperHeight = i_pSection->GetPaperHeight();
  m_bForward = true_a;
}

CPDFSection::~CPDFSection()
{
}

void CPDFSection::SetPaperSize(long i_lPaperWidth, long i_lPaperHeight)
{
  m_lPaperWidth = i_lPaperWidth;
  m_lPaperHeight = i_lPaperHeight;
}

void CPDFSection::SetMargins(long i_lLeft, long i_lRight, long i_lTop, long i_lBottom)
{
  m_lLeftMargin = i_lLeft;
  m_lRightMargin = i_lRight;
  m_lTopMargin = i_lTop;
  m_lBottomMargin = i_lBottom;
}
/*
void CPDFSection::SetLeftMargin(CPDFTextBox *i_pTextArea, long i_lLeftMargin)
{
  m_lLeftMargin = i_lLeftMargin;
}

void CPDFSection::SetRightMargin(CPDFTextBox *i_pTextArea, long i_lRightMargin)
{
  m_lRightMargin = i_lRightMargin;
}

void CPDFSection::SetTopMargin(CPDFTextBox *i_pTextArea, long i_lTopMargin)
{
  m_lTopMargin = i_lTopMargin;
}

void CPDFSection::SetBottomMargin(CPDFTextBox *i_pTextArea, long i_lBottomMargin)
{
  m_lBottomMargin = i_lBottomMargin;
}

void CPDFSection::SetColumns(CPDFTextBox *i_pTextArea, long i_lColumns)
{
  m_lColumns = i_lColumns;
}

void CPDFSection::SetColumnSpace(CPDFTextBox *i_pTextArea, long i_lColumnSpace)
{
  m_lColumnSpace = i_lColumnSpace;
}

void CPDFSection::SetOrientation(CPDFTextBox *i_pTextArea, long i_lOrientation)
{
  m_lOrientation = i_lOrientation;
  if (!m_pDoc)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pTextArea = NULL;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
        pTextArea = pPage->GetHeader();
      if (pTextArea && (pTextArea != i_pTextArea))
        pPage->SetOrientation(i_lOrientation);
    }
  }
}

void CPDFSection::SetFirstPage(CPDFTextBox *i_pTextArea, long i_lFirstPage)
{
  m_lFirstPage = i_lFirstPage;
}

void CPDFSection::SetLastPage(CPDFTextBox *i_pTextArea, long i_lLastPage)
{
  m_lLastPage = i_lLastPage;
}

void CPDFSection::SetHdrMargin(CPDFTextBox *i_pTextArea, long i_lHdrMargin)
{
  m_lHdrMargin = i_lHdrMargin;
}

void CPDFSection::SetFtrMargin(CPDFTextBox *i_pTextArea, long i_lFtrMargin)
{
  m_lFtrMargin = i_lFtrMargin;
}

void CPDFSection::SetPaperWidth(CPDFTextBox *i_pTextArea, long i_lPaperWidth)
{
  m_lPaperWidth = i_lPaperWidth;
}

void CPDFSection::SetPaperHeight(CPDFTextBox *i_pTextArea, long i_lPaperHeight)
{
  m_lPaperHeight = i_lPaperHeight;
}
*/
CPDFHeaderFooter *CPDFSection::GetHdrExemplar()
{
  if (!m_pDoc)
    return NULL;
  CPDFPage *pPage = m_pDoc->GetPage(m_lFirstPage);
  if (!pPage || !pPage->HasHeader())
    return NULL;
  return pPage->GetHeader();
}

CPDFHeaderFooter *CPDFSection::GetFtrExemplar()
{
  if (!m_pDoc)
    return NULL;
  CPDFPage *pPage = m_pDoc->GetPage(m_lFirstPage);
  if (!pPage || !pPage->HasFooter())
    return NULL;
  return pPage->GetFooter();
}

void CPDFSection::AddTextToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, wchar_t *i_pText)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->AddText(i_pText);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::AddTextToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, char *i_pText)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->AddText(i_pText);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::AddImageToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, char *i_pImageName, long i_lX, long i_lY, long i_lWidth, long i_lHeight,
                                      long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist, bool_a i_bAddToLine)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (!m_pDoc)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->AddImage(i_pImageName, i_lX, i_lY, i_lWidth, i_lHeight, i_lLeftDist,
                          i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::AddScaledImageToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, char *i_pImageName, long i_lX, long i_lY, long i_lScaledX, long i_lScaledY,
                                          long i_lLeftDist, long i_lRightDist, long i_lTopDist, long i_lBottomDist, bool_a i_bAddToLine)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->AddScaledImage(i_pImageName, i_lX, i_lY, i_lScaledX, i_lScaledY, i_lLeftDist,
                                i_lRightDist, i_lTopDist, i_lBottomDist, i_bAddToLine);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::InsertLineToHdrFtr(CPDFHeaderFooter *i_pHdrFtr)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->InsertLine();
    }
  }
  m_bForward = true_a;
}

void CPDFSection::InsertTabToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, PDFTabStop i_Tab)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->InsertTab(i_Tab);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::SetTextColorToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, float i_fRed, float i_fGreen, float i_fBlue)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->SetTextColor(i_fRed, i_fGreen, i_fBlue);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::SetAlignmentToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, short i_sAlignment)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->SetAlignment(i_sAlignment);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::SetLeftRightIndentsToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, long i_lLeft, long i_lRight, long i_lFirstIndent)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->SetLeftRightIndents(i_lLeft, i_lRight, i_lFirstIndent);
    }
  }
  m_bForward = true_a;
}

void CPDFSection::AddPageMarkToHdrFtr(CPDFHeaderFooter *i_pHdrFtr, ePAGEMARKTYPE i_eType)
{
  if (!m_pDoc || !m_bForward)
    return;
  if (m_lFirstPage < 1)
    return;
  long lLast = m_lLastPage;
  if (lLast < 0)
    lLast = m_pDoc->GetLastPage();
  CPDFPage *pPage = NULL;
  CPDFHeaderFooter *pHdrFtr = NULL;
  m_bForward = false_a;
  for (long i = m_lFirstPage; i <= lLast; i++)
  {
    pPage = m_pDoc->GetPage(i);
    if (pPage)
    {
      if (i_pHdrFtr->GetHdrFtrType() == CPDFHeaderFooter::eHeader)
        pHdrFtr = pPage->GetHeader();
      else
        pHdrFtr = pPage->GetFooter();
      if (pHdrFtr && (pHdrFtr != i_pHdrFtr))
        pHdrFtr->AddPageMark(i_eType);
    }
  }
  m_bForward = true_a;
}

bool_a CPDFSection::PageBelong(long i_lPage)
{
  if (!m_pDoc)
    return false_a;
  if (m_lFirstPage < 1)
    return false_a;
  if (m_lFirstPage > i_lPage)
    return false_a;
  if ((i_lPage <= m_lLastPage) || (m_lLastPage == -1))
    return true_a;
  return false_a;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

/*
//CPDFInfo implementation
CPDFInfo::CPDFInfo()
{
}

CPDFInfo::~CPDFInfo()
{
}
*/
