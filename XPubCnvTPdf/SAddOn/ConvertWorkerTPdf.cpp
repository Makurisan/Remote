#include <assert.h>
#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"

#include "SAddOnManager/XPubAddOnManager.h"

#include "SAddOnManager/ConvertWorker.h"
#include "SAddOn/ConvertWorkerFTag.h"
#include "PDFDocument.h"
#include "PDFLayer.h"

#include "ConvertWorkerTPdf.h"

#include <string.h>

#define GETPARAM1(tag, val1)     \
  CConvertWorkerFromTag::GetParameterValues(cInTag, tag, val1, cParValue[1], cParValue[2], cParValue[3], 1)


#if !defined(WIN32)
#define GetRValue(rgb)  ((unsigned char)(rgb))
#define GetGValue(rgb)  ((unsigned char)((/*(WORD)*/(rgb)) >> 8))
#define GetBValue(rgb)  ((unsigned char)((rgb)>>16))
#endif // WIN32



static bool bT = false;

// Unter LINUX waren die Namen den folgenden Funktionen
// in SO mit verrueckten Namen. Ich habe nur diese Moeglichkeit gefunden,
// dass die Namen in SO unveraendert sind.

#if defined(LINUX_OS) || defined(MAC_OS_MWE)
extern "C"
{
#endif // LINUX_OS

#if defined(MAC_OS_MWE)
#pragma export on
#endif

#if !defined(_LIB)
  CConvertToAddOn* RegisterAddOnManager(CXPubAddOnManager* pManager);
  bool UnregisterAddOnManager(CConvertToAddOn* pCnvAddOn);
  int GetCnvToAddOnVersion();
  CConvertWorkerTo* CreateConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, HWND hWndParent);
  CConvertWorkerTo* CreateConvertWorkerToClone_(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
  bool ReleaseConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
#endif // !defined(_LIB)

#if defined(MAC_OS_MWE)
#pragma export off
#endif

#if !defined(_LIB)
CConvertToAddOn* RegisterAddOnManager(CXPubAddOnManager* pManager)
{
  CConvertToAddOn* pNew = new CConvertToPdfAddOn(pManager);
  if (!pNew)
    throw(ERR_NEW_PROBLEM);
  return pNew;
}

bool UnregisterAddOnManager(CConvertToAddOn* pCnvAddOn)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_UNREGISTER_ADDON_WITH_NULL_POINTER);
    return false;
  }

  delete pCnvAddOn;
  pCnvAddOn = NULL;
  return true;
}

int GetCnvToAddOnVersion()
{
  return ConvertWorkerTPdf_Version;
}

CConvertWorkerTo* CreateConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, HWND hWndParent)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerTo(hWndParent);
}

CConvertWorkerTo* CreateConvertWorkerToClone_(CConvertToAddOn* pCnvAddOn,
                                              CConvertWorkerTo* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->CreateConvertWorkerToClone(pConvertWorker);
}

bool ReleaseConvertWorkerTo_(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker)
{
  if (!pCnvAddOn)
  {
    throw(ERR_ADDON_RELEASEWORKER_WITH_NULL_POINTER);
    return false;
  }

  return pCnvAddOn->ReleaseConvertWorkerTo(pConvertWorker);
}
#endif // !defined(_LIB)

#if defined(LINUX_OS)  || defined(MAC_OS_MWE)
}
#endif // LINUX_OS









CConvertToPdfAddOn::CConvertToPdfAddOn(CXPubAddOnManager* pAddOnManager)
                   :CConvertToAddOn(pAddOnManager)
{
}

CConvertToPdfAddOn::~CConvertToPdfAddOn()
{
}

CConvertWorkerTo* CConvertToPdfAddOn::CreateConvertWorkerTo(HWND hWndParent)
{
  CConvertWorkerTo* pWorker = new CConvertWorkerToPdf(this, hWndParent);
  if (!pWorker)
  {
    throw(ERR_ADDON_NEWWORKER_NOT_CREATED);
    return pWorker;
  }

  if (AddConvertWorkerTo(pWorker))
    return pWorker;

  delete pWorker;
  pWorker = NULL;
  throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);
  return pWorker;
}

CConvertWorkerTo* CConvertToPdfAddOn::CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker)
{
  if (!pConvertWorker)
  {
    throw(ERR_ADDON_CREATEWORKERCLONE_FROM_NULL_POINTER);
    return NULL;
  }

  CConvertWorkerToPdf* pWorker = (CConvertWorkerToPdf*)((CConvertWorkerToPdf*)pConvertWorker)->Clone();
  if (AddConvertWorkerTo(pWorker))
    return pWorker;

  delete pWorker;
  pWorker = NULL;
  throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);
  return pWorker;
}

bool CConvertToPdfAddOn::ReleaseConvertWorkerTo(CConvertWorkerTo* pWorker)
{
  if (!pWorker)
  {
    throw(ERR_ADDON_RELEASEWORKER_FROM_NULL_POINTER);
    return false;
  }

  if (!RemoveConvertWorkerTo(pWorker))
    return false;

  return true;
}


















































bool ConvertColorToPDFColor(_ColorDef& color, CPDFColor& rPDFColor)
{
  switch (color.GetType())
  {
    case tColorCMYK:
      rPDFColor.SetColor(color.cmyk.c / 255.f, color.cmyk.m / 255.f,
                      color.cmyk.y / 255.f, color.cmyk.k / 255.f);
      break;
    case tColorRGB:
      rPDFColor.SetColor(color.rgb.r / 255.f, color.rgb.g / 255.f, color.rgb.b / 255.f);
      break;
    case tColorEmpty:
      rPDFColor.SetColorTypeToEmpty();
      break;
    default:
      return false;
  }
  return true;
}

CConvertWorkerToPdf::CConvertWorkerToPdf(CConvertToPdfAddOn* pAddOn, HWND hWndParent)
                    :CConvertWorkerTo(tDataFormatST_Pdf, hWndParent)
{

  bT = false;

  m_pAddOn = pAddOn;

  m_sFontName = m_sDefaults.DefFontName;
  m_sCodePage = m_sDefaults.DefCodePage;
  m_nFontSize = m_sDefaults.DefFontSize;

  m_nTableID = -1;
  m_nTableRow = -1;
  m_nTableCol = -1;
  m_nTableNestRow = -1;
  m_nTableNestCol = -1;
  m_nActiveTableID = -1;
  m_nActiveRow = -1;
  m_nActiveCol = -1;

  m_nActiveNestTableID = -1;
  m_nActiveNestTableRowPos = -1;
  m_nActiveNestTableColPos = -1;

  m_nActiveNestRow = -1;
  m_nActiveNestCol = -1;
  //m_bInTable = false;
  m_nFrameID = -1;

  m_nLastStyleID = 1;


  m_eActiveCtrlType[0] = ePageTextAreaCtrl;
  m_eActiveCtrlType[1] = eNoCtrl;
  m_eActiveCtrlType[2] = eNoCtrl;


  m_pCurDoc = m_cPDFMan.CreateDocument("$$yswq1pbba02");
  if (m_pCurDoc)
  {
    m_pCurDoc->EnableCompression();
    m_pCurDoc->FlushToFile(false_a);

    m_pCurDoc->SetActualFont(m_sFontName.c_str(), m_nFontSize, atol(m_sCodePage.c_str()));

    //tab width
    rec_a recPosSize;
    recPosSize.lBottom = 0;
    recPosSize.lLeft = 0;

    CPDFSection *pSect = m_pCurDoc->GetSection(1);
    if (pSect)
    {
      pSect->SetLeftMargin(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.LeftMargin));
      pSect->SetRightMargin(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.RightMargin));
      pSect->SetTopMargin(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.TopMargin));
      pSect->SetBottomMargin(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.BotMargin));
      pSect->SetPaperSize(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.PageWidth), m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.PageHeight));
      pSect->SetOrientation(m_sDefaults.PaperOrient);
      pSect->SetHdrMargin(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.HdrMargin));
      pSect->SetFtrMargin(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.FtrMargin));

      recPosSize.lRight = pSect->GetPaperWidth();
      recPosSize.lTop = pSect->GetPaperHeight();
    }
    else
    {
      recPosSize.lRight = m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.PageWidth);
      recPosSize.lTop = m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.PageHeight);
    }

    m_pCurDoc->SetCellMargins(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.DefCellMargin),
                              m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.DefCellMargin),
                              m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.DefCellMargin),
                              m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.DefCellMargin));

    m_pCurDoc->SetTabDefault(m_pCurDoc->ConvertTwipsToPoints(m_sDefaults.DefTabWidth), m_sDefaults.DefDecimal[0]);

    m_pCurDoc->CreatePage(1, recPosSize.lRight, recPosSize.lTop);

    //m_pCurDoc->SetPassword("Master", "User", 0);
  }
}

CConvertWorkerToPdf::~CConvertWorkerToPdf()
{
}

CConvertWorkerTo* CConvertWorkerToPdf::Clone()
{
  CConvertWorkerToPdf* pNew = new CConvertWorkerToPdf(m_pAddOn, m_hWndParent);
  if (!pNew)
  {
    throw(ERR_ADDON_NEWWORKER_FOR_CLONE_NOT_CREATED);
    return NULL;
  }

  return pNew;
}

bool CConvertWorkerToPdf::IsDataFormatAvailable(TDataFormatForStoneAndTemplate nOutputDataFormat)
{
  return (nOutputDataFormat == tDataFormatST_Pdf);
}

bool CConvertWorkerToPdf::SetPassword(const xtstring& sOwner, const xtstring& sUser, int nPermission)
{
  if (!m_pCurDoc)
    return false;
  //password set only to main document(not layers)
  m_pCurDoc->SetPassword(sOwner.c_str(), sUser.c_str(), nPermission);
  return true;
}

bool CConvertWorkerToPdf::SetFontEmbedded(bool i_bEmbed)
{
  if (!m_pCurDoc)
    return false;
  //set it only to the main document, layers use fonts from main document
  m_pCurDoc->SetFontEmbedded(i_bEmbed);
  return true;
}

bool CConvertWorkerToPdf::GetDocumentStrings(CxtstringVector& vStringArray)
{
  if (!m_pCurDoc)
    return false;
  if(m_pCurDoc->GetTemplateCount())
  {
    for (long iCount = 0; iCount < m_pCurDoc->GetTemplateCount(); ++iCount)
    {
      for(long lPage = 0; lPage < m_pCurDoc->GetTemplatePageCount(iCount); ++lPage)
      {
        for(long lBlkCount = 0; lBlkCount < m_pCurDoc->GetTemplateStrBlockCount(iCount, lPage); ++lBlkCount)
        {
          xtstring sWord;
        //  sWord.ConvertAndAssign(m_pCurDoc->GetTemplateStrBlock(iCount, lPage, lBlkCount), CP_ACP);
          vStringArray.push_back(sWord);
        }
      }
    }
    return true;
  }
  return true;
}

bool CConvertWorkerToPdf::SetDocumentInfo(const TDocumentProperty nTypeOfProperty, const xtstring& sValue)
{
  xtstring sDoc(_XT(""));
  if (nTypeOfProperty == eDoc_Title) // tPDFP_Title
    return PDFSetDocInfo(sDoc, eTitle, sValue);
  else if (nTypeOfProperty == eDoc_Subject) // tPDFP_Subject
    return PDFSetDocInfo(sDoc, eSubject, sValue);
  else if (nTypeOfProperty == eDoc_Author) // tPDFP_Author
    return PDFSetDocInfo(sDoc, eAuthor, sValue);
  else if (nTypeOfProperty == eDoc_Keywords) // tPDFP_Keywords
    return PDFSetDocInfo(sDoc, eKeywords, sValue);
  else if (nTypeOfProperty == eDoc_Creator)
    return PDFSetDocInfo(sDoc, eCreator, sValue);
  return true;
}

bool CConvertWorkerToPdf::PDFSetDocInfo(const xtstring& sDoc, eDOCINFO eType, const xtstring& sText)
{
  CPDFDocument *pDoc = NULL;
  if (sDoc.length() > 0)
    pDoc = m_cPDFMan.GetDocument(sDoc.c_str());
  else
    pDoc = m_pCurDoc;
  if (!pDoc)
    return false;
  pDoc->SetDocInfo(eType, sText.c_str());
  return true;
}

bool CConvertWorkerToPdf::PositionCursor(const xtstring& sXPubConst, TWorkPaperCopyType eWorkPaperCopyType)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
//  if (pPage->GetActiveControlType() != CPDFPage::eTextArea)
//    pPage->ActivateControl(CPDFPage::eTextArea);
//  else
//    m_bInTable = false;
  return true;
}

xtstring CConvertWorkerToPdf::GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat)
{
  return _XT("");
/*
  if (!m_pCurDoc)
    return _XT("");
  m_cPDFMan.FlushAll();
  char *pBuf = NULL;
  long lSize = m_pCurDoc->GetDocumentStream(&pBuf);
  CString sPDF(pBuf, lSize);
  return xtstring(sPDF);
*/
}

bool CConvertWorkerToPdf::GetDataStream(TDataFormatForStoneAndTemplate nOutputDataFormat, char *pOutput, int &nSize)
{
  if (!m_pCurDoc)
    return false;
  if (!m_cPDFMan.FlushAll())
    return false;
  const char *pBuf = NULL;
  long lSize = m_pCurDoc->GetDocumentStream(&pBuf);
  if (nSize < lSize)
  {
    nSize = lSize;
    return false;
  }
  memcpy(pOutput, pBuf, (size_t)lSize);
  return true;
}

void CConvertWorkerToPdf::GetCellRange(CPDFTable *i_pTable, long i_lRow, long i_lCol,
                                        long &o_lBgnRow, long &o_lEndRow, long &o_lBgnCol, long &o_lEndCol)
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
    o_lEndRow = i_pTable->GetRowCount();
    o_lBgnCol = i_lCol;
    o_lEndCol = i_lCol;
    return;
  }
  if (i_lRow == 0 && i_lCol == 0)
  {
    o_lBgnRow = 1;
    o_lEndRow = i_pTable->GetRowCount();
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

long CConvertWorkerToPdf::GetTable()
{
  //if (m_nActiveTableID != -1)
  //{
  //    return m_nActiveTableID;
  //}
  return m_nTableID;
}

long CConvertWorkerToPdf::GetRow()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return 0;
  CPDFTable *pTable = pDoc->GetTable(GetTable());
  if (!pTable)
    return 0;
  if (GetTableRow() <= 0)
    return 0;
  if (GetTableRow() >= pTable->GetRowCount())
    return pTable->GetRowCount() - 1;
  return GetTableRow() - 1;
}

long CConvertWorkerToPdf::GetCol()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return 0;
  CPDFTable *pTable = pDoc->GetTable(GetTable());
  if (!pTable)
    return 0;
  if (GetTableCol() <= 0)
    return 0;
  if (GetTableCol() >= pTable->GetColCount(GetRow()))
    return pTable->GetColCount(GetRow()) - 1;
  return GetTableCol() - 1;
}

CPDFTable *CConvertWorkerToPdf::GetNestTable()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return NULL;
  CPDFTable *pTable = pDoc->GetTable(GetTable());
  if (!pTable)
    return NULL;
  CPDFCell *pCell = pTable->GetCell(m_nActiveNestTableRowPos - 1, m_nActiveNestTableColPos - 1);
  if (!pCell)
    return NULL;
  //if (!pCell->IsTable())
  //  return NULL;
  pTable = pDoc->GetTable(m_nActiveNestTableID);//ToDo: add check - pTable must be in pCell;
  if (!pTable)
    return NULL;
  return pTable;//(CPDFTable *)pCell;
}

long CConvertWorkerToPdf::GetNestRow()
{
  if (m_nActiveNestRow > 0)
    return m_nActiveNestRow - 1;
  if (m_nTableNestRow <= 0)
    return 0;
  return m_nTableNestRow - 1;
}

long CConvertWorkerToPdf::GetNestCol()
{
  if (m_nActiveNestCol > 0)
    return m_nActiveNestCol - 1;
  if (m_nTableNestCol <= 0)
    return 0;
  return m_nTableNestCol - 1;
}

void CConvertWorkerToPdf::SetActiveCtrlType(eActiveCtrlType i_eActiveCtrlType)
{
  switch (i_eActiveCtrlType)
  {
  case ePageTextAreaCtrl:
      m_eActiveCtrlType[0] = ePageTextAreaCtrl;
      m_eActiveCtrlType[1] = m_eActiveCtrlType[2] = eNoCtrl;
    break;
  case ePageHeaderCtrl:
      m_eActiveCtrlType[0] = ePageHeaderCtrl;
      m_eActiveCtrlType[1] = m_eActiveCtrlType[2] = eNoCtrl;
    break;
  case ePageFooterCtrl:
      m_eActiveCtrlType[0] = ePageFooterCtrl;
      m_eActiveCtrlType[1] = m_eActiveCtrlType[2] = eNoCtrl;
    break;
  case eFrameCtrl:
    if (eNoCtrl == m_eActiveCtrlType[1])
      m_eActiveCtrlType[1] = eFrameCtrl;
    else if (eNoCtrl == m_eActiveCtrlType[2])
        m_eActiveCtrlType[2] = eFrameCtrl;
    else
    {
      m_eActiveCtrlType[1] = m_eActiveCtrlType[2];
      m_eActiveCtrlType[2] = eFrameCtrl;
    }
    break;
  case eTableCtrl:
    if (eNoCtrl == m_eActiveCtrlType[1] || eTableCtrl == m_eActiveCtrlType[1])
      m_eActiveCtrlType[1] = eTableCtrl;
    else if (eNoCtrl == m_eActiveCtrlType[2] || eTableCtrl == m_eActiveCtrlType[2])
      m_eActiveCtrlType[2] = eTableCtrl;
    else
    {
      m_eActiveCtrlType[1] = m_eActiveCtrlType[2];
      m_eActiveCtrlType[2] = eTableCtrl;
    }
    break;
  //case eNestTableCtrl:
  //  if (eTableCtrl == m_eActiveCtrlType[2])
  //    m_eActiveCtrlType[2] = eNestTableCtrl;
  //  else if (eNoCtrl == m_eActiveCtrlType[2] && eTableCtrl == m_eActiveCtrlType[1])
  //    m_eActiveCtrlType[1] = eNestTableCtrl;
  //  break;
  case eNoCtrl:
/*    if (eNestTableCtrl == m_eActiveCtrlType[2])
      m_eActiveCtrlType[2] = eTableCtrl;
    else if (eNestTableCtrl == m_eActiveCtrlType[1])
      m_eActiveCtrlType[1] = eTableCtrl;
    else */if (eNoCtrl != m_eActiveCtrlType[2])
    {
      if (eTableCtrl == m_eActiveCtrlType[2])
      {
        if(!m_TableStack.empty())
        {
          CTableStackItem cTableStack;
          m_TableStack.pop_back();
          if(m_TableStack.empty())
              m_eActiveCtrlType[2] = eNoCtrl;
          else
          {
            cTableStack = m_TableStack.back();
            //m_nActiveTableID = cTableStack.m_nTableID;
            //m_nActiveRow = cTableStack.m_nTableRow;
            //m_nActiveCol = cTableStack.m_nTableCol;
            SetTableID(cTableStack.m_nTableID);
            SetTableRow(cTableStack.m_nTableRow);
            SetTableCol(cTableStack.m_nTableCol);
          }
        }
        else
          m_eActiveCtrlType[2] = eNoCtrl;
      }
    }
    else
    {
      if (eTableCtrl == m_eActiveCtrlType[1])
      {
        if(!m_TableStack.empty())
        {
          CTableStackItem cTableStack;
          m_TableStack.pop_back();
          if(m_TableStack.empty())
            m_eActiveCtrlType[1] = eNoCtrl;
          else
          {
            cTableStack = m_TableStack.back();
            //m_nActiveTableID = cTableStack.m_nTableID;
            //m_nActiveRow = cTableStack.m_nTableRow;
            //m_nActiveCol = cTableStack.m_nTableCol;
            SetTableID(cTableStack.m_nTableID);
            SetTableRow(cTableStack.m_nTableRow);
            SetTableCol(cTableStack.m_nTableCol);
          }
        }
        else
          m_eActiveCtrlType[1] = eNoCtrl;
      }
    }


    break;
  default:
    break;
  };
}

CConvertWorkerToPdf::eActiveCtrlType CConvertWorkerToPdf::GetActiveCtrlType()
{
  if (eNoCtrl != m_eActiveCtrlType[2])
    return m_eActiveCtrlType[2];
  if (eNoCtrl != m_eActiveCtrlType[1])
    return m_eActiveCtrlType[1];
  return m_eActiveCtrlType[0];
}

CPDFDocument *CConvertWorkerToPdf::GetActiveDocumet()
{
  CPDFDocument *pDoc = m_pCurDoc->GetActiveLayer();
  return pDoc;
}

CPDFTextBox *CConvertWorkerToPdf::GetActiveCtrl()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return NULL;

  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return NULL;
  eActiveCtrlType eCtrlType = GetActiveCtrlType();
  CPDFTextBox *pRet = NULL;

  switch (eCtrlType)
  {
  case ePageTextAreaCtrl:
  case ePageHeaderCtrl:
  case ePageFooterCtrl:
    pRet = pPage->GetActiveControl();
    break;
  case eFrameCtrl:
    if (m_nFrameID >= 0)
    {
      CPDFFrame *pFrame = pDoc->GetFrame(m_nFrameID);
      if (!pFrame)
        break;
      pRet = (CPDFTextBox *)pFrame;
    }
    break;
  case eTableCtrl:
    {
      CPDFTable *pTable = pDoc->GetTable(GetTable());
      if (!pTable)
        break;
      CPDFCell *pCell = pTable->GetCell(GetRow(), GetCol());
      if (!pCell)
        break;
      if (pCell->IsTable())
        break;
      pRet = (CPDFTextBox *)(CPDFTableCell *)pCell;
    }
    break;
  //case eNestTableCtrl:
  //  {
  //    CPDFTable *pTable = GetNestTable();
  //    if (!pTable)
  //      break;
  //    CPDFCell *pCell = pTable->GetCell(GetNestRow(), GetNestCol());
  //    if (!pCell)
  //      break;
  //    if (pCell->IsTable())
  //      break;
  //    pRet = (CPDFTextBox *)(CPDFTableCell *)pCell;
  //  }
  //  break;
  case eNoCtrl:
  default:
    break;
  };

  return pRet;
}

bool CConvertWorkerToPdf::InsertWideTextFromMemory(const xwstring& sInput, int nFormat/* = CNV_TEXTFORMAT_TEXT*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  long lPage = -1;
  xwstring str = sInput.c_str();
//  str.TrimLeft(_XT(" "));

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
  {
    if (str.empty())
      return true;
    return false;
  }
  pTextBox->AddText((wchar_t *)str.c_str());
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::InsertTextFromMemory(const xtstring& sInput, int nFormat/* = CNV_TEXTFORMAT_TEXT*/)
{
// ergänzt um UTF8 Unterstützung
  if(isLegalUTF8String((UTF8*)sInput.c_str(), sInput.size()))
  {
  //AfxMessageBox("InsertTextFromMemory ");
    xwstring sUni;
    sUni.ConvertAndAssign(sInput, UTF8_CODEPAGE);

    return InsertWideTextFromMemory(sUni, nFormat);
  }

  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  long lPage = -1;
  xtstring str = sInput;
//  str.TrimLeft(_XT(" "));

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
  {
    if (str.empty())
      return true;
    return false;
  }
  pTextBox->AddText((char *)str.c_str());
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::FillPageBreak()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  if (!pDoc->FillPageBreak())
    return false_a;
  return true;
}

bool CConvertWorkerToPdf::BeginPageBody(const int nPos)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  CPDFTextBox *pTextBox = pPage->GetTextArea();
  if (!pTextBox)
    return false;
  if (!pTextBox->AddEmptySpace(pDoc->ConvertTwipsToPoints(nPos)))
    return false;

  return true;
}

bool CConvertWorkerToPdf::InsertParagraph()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  long lPage = -1;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  pTextBox->AddText((char *)czNEWPARAGRAPH_2);
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetDefaults()
{
  return SetFont(m_sFontName, m_nFontSize, atol(m_sCodePage.c_str()));
}

bool CConvertWorkerToPdf::InsertLine()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  // not used. bool_a bRet = true_a;
  long lPage = -1;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if (!pTextBox->InsertLine())
    return false;
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetParagraphControl(const int nKeep, const int nKeepNext, const int nWidowOrphon)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if(nKeep != -1)
    if (!pTextBox->SetKeepLinesTogether(0 == nKeep ? false_a : true_a))
      return false;

  if (nWidowOrphon != -1)
    if (!pTextBox->SetWidowOrphan(0 == nWidowOrphon ? false_a : true_a))
      return false;


  if(nKeepNext != -1)
  {
    eActiveCtrlType eCtrlType = GetActiveCtrlType();
    if (eTableCtrl == eCtrlType)
    {
      CPDFTable *pTable = pDoc->GetTable(GetTable());
      if (!pTable)
        return false;
      pTable->SetFixedRow(GetRow(), 0 == nKeepNext ? false_a : true_a);
      pTable->SetFixedRow(GetRow() + 1, 0 == nKeepNext ? false_a : true_a);
    }
    else if (!pTextBox->SetKeepWithNext(0 == nKeepNext ? false_a : true_a))
      return false;
  }

  return true;
}

bool CConvertWorkerToPdf::ParagraphPageBreak(const int nInsert)
{
  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if(nInsert != -1)
    if (!pTextBox->SetParagraphPageBreak(0 == nInsert ? false_a : true_a))
      return false;

  return true;
}

bool CConvertWorkerToPdf::SetParagraphSpacing(int nSpaceBefore, int nSpaceAfter, int nSpaceBetween, int nLineSpacing)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if (nSpaceBetween != -1)
  {
    if (!pTextBox->SetLineSpace(pDoc->ConvertTwipsToPoints(nSpaceBetween)))
      return false;
  }
  long lLeft, lRight, lTop, lBottom, lFirst;
  pTextBox->GetIndents(lLeft, lTop, lRight, lBottom, lFirst);
  if (nSpaceBefore != -1)
    lTop = pDoc->ConvertTwipsToPoints(nSpaceBefore);
  if (nSpaceAfter != -1)
    lBottom = pDoc->ConvertTwipsToPoints(nSpaceAfter);
  if (!pTextBox->SetIndents(lLeft, lTop, lRight, lBottom, lFirst))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetParagraphIndents(int nLinks, int nRechts, int nFirstIndent)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  long lPage = -1;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  long lLeft, lRight, lTop, lBottom, lFirst;
  pTextBox->GetIndents(lLeft, lTop, lRight, lBottom, lFirst);
  if (nLinks)
    lLeft = pDoc->ConvertTwipsToPoints(nLinks);
  if (nRechts)
    lRight = pDoc->ConvertTwipsToPoints(nRechts);
  if (nFirstIndent)
    lFirst = pDoc->ConvertTwipsToPoints(nFirstIndent);

  if (!pTextBox->SetLeftRightIndents(lLeft, lRight, lFirst))
    return false;
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::InsertTabulator(int nTabFlag, int nTabPos, const xtstring& sTabDecChar, const xtstring& sTabLeader)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  bool_a bRet = true_a;
  PDFTabStop strTab;
  strTab.sTabFlag = TAB_LEFT;
  strTab.lTabPos = pDoc->ConvertTwipsToPoints(nTabPos);

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;

  switch(nTabFlag)
  {
  case TAG_TABULATOR_LEFT: // The tab position is at the left side of text.
    strTab.sTabFlag = TAB_LEFT;
    break;
  case TAG_TABULATOR_RIGHT: // The tab position is at the right side of text.
    strTab.sTabFlag = TAB_RIGHT;
    break;
  case TAG_TABULATOR_CENTER: // The text is centered on the tab position.
    strTab.sTabFlag = TAB_CENTER;
    break;
  case TAG_TABULATOR_DECIMAL: // The text is centered on the tab position.
    strTab.sTabFlag = TAB_DECIMAL;
    break;
  case TAG_TABULATOR_CLEARALL: // Clear all Positions
    bRet = pTextBox->SetTabs(NULL, 0);
    return (bRet != false_a);
  case TAG_TABULATOR_DEFAULT: // the tab has no position
    break;
  }

  bRet = pTextBox->InsertTab(strTab);
  if (!bRet)
    return false;
  //bRet = InsertTab();
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::ClearTabulators()
{
  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;

  if (!pTextBox->SetTabs(NULL, 0))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetDefTabulator(int nNewWidth, int nDecDel)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  char cTabDec;
  bool_a bRet = true_a;
  switch (nDecDel)
  {
  case TAG_TABULATOR_DECIMAL_COMMA:
    cTabDec = PDF_DEFAULT_TAB_DECIMAL_COMMA;
  break;
  case TAG_TABULATOR_DECIMAL_POINT:
    cTabDec = PDF_DEFAULT_TAB_DECIMAL_POINT;
  break;
  default:
    return false;
  }
  bRet = pDoc->SetTabDefault(pDoc->ConvertTwipsToPoints(nNewWidth), cTabDec);
  if (!bRet)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  long lPage = -1;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if (!pTextBox->SetTabDefault(pDoc->ConvertTwipsToPoints(nNewWidth), cTabDec))
    return false;
  lPage = pTextBox->GetPageNumber();

  if (bRet)
    bRet = pDoc->CheckPageOverflow(lPage);
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::SetFont(const xtstring& sFontName, int nFontSize, int nCodePage)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  xtstring str = sFontName;
  str.Trim(_XT(" "));
  if (!pDoc->SetActualFont(str.c_str(), nFontSize, nCodePage))
    return false;

  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  long lPage = -1;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  pTextBox->FontSizeChanged();
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetFontName(const xtstring& sFontName)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  xtstring str = sFontName;
  str.Trim(_XT(" "));
  if (!pDoc->SetActualFont(str.c_str()))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetCodePage(const xtstring& sCodePage)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  if (pDoc->SetActualCodePage(atol(sCodePage.c_str())))
    return true;
  return false;
}

bool CConvertWorkerToPdf::SetFontSize(int nFontSize)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  pDoc->SetFontSize(nFontSize);

  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  long lPage = -1;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  pTextBox->FontSizeChanged();
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetFontNameDefault(const xtstring& sFontName)
{
  xtstring str = sFontName;
  str.Trim(_XT(" "));
  m_sFontName = str;
  //CPDFDocument *pDoc = GetActiveDocumet();
  //if (!pDoc)
  //  return false;
  //pDoc->SetDefaultFont(str.c_str());
  return true;
}

bool CConvertWorkerToPdf::SetFontSizeDefault(int nFontSize)
{
  m_nFontSize = nFontSize;
  //CPDFDocument *pDoc = GetActiveDocumet();
  //if (!pDoc)
  //  return false;
  //pDoc->SetDefaultFontSize(nFontSize);
  return true;
}

bool CConvertWorkerToPdf::SetCodePageDefault(const xtstring& sCodePage)
{
  m_sCodePage = sCodePage;
  //CPDFDocument *pDoc = GetActiveDocumet();
  //if (!pDoc)
  //  return false;
  //pDoc->SetDefaultCodePage(nFontSize);
  return true;
}

bool CConvertWorkerToPdf::InsertTab()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  long lPage = -1;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  pTextBox->AddText((char *)czTABSTOP);
  lPage = pTextBox->GetPageNumber();

  if (!pDoc->CheckPageOverflow(lPage))
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetTableBackgroundPicture(int nTableID, int wRow, int wCol, const xtstring& cFile)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;

  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  //wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  if ( !pTable->SetTableBgImage(wRow, wCol, cFile.c_str()) )
    return false;

  return true;
}

bool CConvertWorkerToPdf::InsertImage(const xtstring& id, const xtstring& sName, const xtstring& cPath,  float nWidht, float nHeight,
                                      int nZoom, bool bLinked/*=true*/,  bool bShrink/*=false*/, int nFitting/*=0*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  if (cPath.size() <= 0)
    return false;
  long lWidth, lHeight;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  long lPage = -1;
  bool_a bRet = true_a;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  lPage = pTextBox->GetPageNumber();

  lWidth = pDoc->ConvertTwipsToPoints(nWidht);
  lHeight = pDoc->ConvertTwipsToPoints(nHeight);

  if (nZoom != -1)
    bRet = pTextBox->AddScaledImage((char *)cPath.c_str(), 0, 0, nZoom, nZoom, DEF_LEFT_DIST, DEF_RIGHT_DIST,
                                    DEF_TOP_DIST, DEF_BOTTOM_DIST, true_a);
  else
  {
    bRet = pTextBox->AddImage((char *)cPath.c_str(), 0, 0, lWidth, lHeight, DEF_LEFT_DIST, DEF_RIGHT_DIST,
                              DEF_TOP_DIST, DEF_BOTTOM_DIST, true_a);
  }

  if (bRet)
    bRet = pDoc->CheckPageOverflow(lPage);

  if(bShrink)
    unlink(cPath.c_str());
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::SetFontAttribute(int nAttrib)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  bool_a bRet = false_a;
  switch (nAttrib)
  {
  case TAG_TEXT_BOLD:
    bRet = pDoc->SetFontStyle(FA_BOLD);
    break;
  case TAG_TEXT_BOLD_END:
    bRet = pDoc->SetFontStyle(FA_NOBOLD);
    break;
  case TAG_TEXT_ITALIC:
    bRet = pDoc->SetFontStyle(FA_ITALIC);
    break;
  case TAG_TEXT_ITALIC_END:
    bRet = pDoc->SetFontStyle(FA_NOITALIC);
    break;
  case TAG_TEXT_UNDERLINE:
    bRet = pDoc->SetFontStyle(FA_UNDERLINE);
    break;
  case TAG_TEXT_UNDERLINE_END:
    bRet = pDoc->SetFontStyle(FA_NOUNDERLINE);
    break;
  case TAG_TEXT_UL_DOUBLE:
    bRet = pDoc->SetFontStyle(FA_DOUBLE_UNDERLINE);
    break;
  case TAG_TEXT_UL_DOUBLE_END:
    bRet = pDoc->SetFontStyle(FA_NODOUBLE_UNDERLINE);
    break;
  case TAG_TEXT_STRIKE:
    bRet = pDoc->SetFontStyle(FA_STRIKEOUT);
    break;
  case TAG_TEXT_STRIKE_END:
    bRet = pDoc->SetFontStyle(FA_NOSTRIKEOUT);
    break;
  default:
    bRet = pDoc->SetFontStyle(FA_STANDARD);
    break;
  }
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::SetTextColor(_ColorDef color)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;

  if (!ConvertColorToPDFColor(color, pTextBox->GetTextColor()))
    return false;

  return true;
}

bool CConvertWorkerToPdf::SetHeaderFooterPos(int nCase, int nPosition)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  bool_a bRet = false_a;
  switch(nCase)
  {
  case TAG_TEXT_HEADPOS: // The distance of the header text from the top of the page
    bRet = pPage->SetHeaderPos(pDoc->ConvertTwipsToPoints(nPosition));
    break;
  case TAG_TEXT_FOOTERPOS: // The distance of the footer text from the bottom of the page
    bRet = pPage->SetFooterPos(pDoc->ConvertTwipsToPoints(nPosition));
    break;
  default:
    assert(0);
    break;
  }
  if (bRet)
    bRet = pDoc->CheckPageOverflow();
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::SwitchHeaderFooter(int nCase, int nPos/*=CURSOR_END*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  bool_a bRet = false_a;
  switch(nCase)
  {
  case TAG_TEXT_HEADPOS:
    bRet = pPage->ActivateControl(CPDFPage::eHeader);
    break;
  case TAG_TEXT_FOOTERPOS:
    bRet = pPage->ActivateControl(CPDFPage::eFooter);
    break;
  case TAG_TEXT_BODYPOS:
    bRet = pPage->ActivateControl(CPDFPage::eTextArea);
    break;
  default:
    assert(0);
    break;
  }
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::ToggleEditHdrFtr()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  CPDFPage::EActiveControl eCtrlType = pPage->GetActiveControlType();
  if (eCtrlType == CPDFPage::eTextArea)
    return false;
  bool_a bRet = false_a;
  if (eCtrlType == CPDFPage::eFooter)
    bRet = pPage->ActivateControl(CPDFPage::eHeader);
  else
    bRet = pPage->ActivateControl(CPDFPage::eFooter);
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::TextAlignment(int nAlign)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  bool_a bRet = false_a;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;

  switch(nAlign)
  {
  case TAG_TEXT_TEXTZ: // Set centered paragraphs.
    bRet = pTextBox->SetAlignment(TF_CENTER);
    break;
  case TAG_TEXT_TEXTL: // Set left-aligned paragraphs.
    bRet = pTextBox->SetAlignment(TF_LEFT);
    break;
  case TAG_TEXT_TEXTR: // Set right-aligned paragraphs.
    bRet = pTextBox->SetAlignment(TF_RIGHT);
    break;
  case TAG_TEXT_TEXTB: // Set to block formatted paragraphs
    bRet = pTextBox->SetAlignment(TF_JUSTIFY);
    break;
  }
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::SetCursorPosition(int nCase, int nSection /*= 0*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  if (pPage->GetActiveControlType() != CPDFPage::eTextArea)
    pPage->ActivateControl(CPDFPage::eTextArea);
  else
  {
    m_nFrameID = -1;
    //m_bInTable = false;
    CPDFTable *pTable = pDoc->GetTable(GetTable());
    if (eTableCtrl == GetActiveCtrlType())
    {
      if (!pTable || !pTable->EndEdit())
        return false;
    }
    else
    {
      m_nActiveNestTableID = -1;
      m_nActiveNestTableRowPos = -1;
      m_nActiveNestTableColPos = -1;
    }
  }

  SetActiveCtrlType(ePageTextAreaCtrl);

  return true;
}

bool CConvertWorkerToPdf::LayerEnd()
{
  if (!m_pCurDoc)
    return false;
  m_pCurDoc->EndLayer();
  //TODO: reset activ controls?
  m_eActiveCtrlType[0] = ePageTextAreaCtrl;
  m_eActiveCtrlType[1] = eNoCtrl;
  m_eActiveCtrlType[2] = eNoCtrl;
  return true;
}

bool CConvertWorkerToPdf::Layer(const xtstring& sName, const xtstring& sGroup, const xtstring& sParent, const int nIntent,
                   const int nDefState, const int nLocked, const int nVisible, const int nPrint, const int nExport)
{
  if (!m_pCurDoc)
    return false;
  CPDFLayer *pParent;
  CPDFLayer *pLayer = m_pCurDoc->CreateLayer(sName.c_str());
  if (!pLayer)
    return false;

  pLayer->SetGroup(sGroup.c_str());
  pLayer->SetDefaultState(nDefState == 0 ? false_a : true_a);
  if (sParent.size() > 0)
  {
    pParent = m_pCurDoc->GetLayer(sParent.c_str());
    if (pParent)
    {
      pLayer->SetParent(sParent.c_str());
      pParent->InsertChildLayer(pLayer);
    }
  }
  if (nIntent)
    pLayer->SetLayerIntent(CPDFLayer::eDesignIntent);
  if (nLocked)
    pLayer->SetLocked();

//const int nVisible
//const int nPrint
//const int nExport

  return true;
}

bool CConvertWorkerToPdf::InsertTextFrameColumns(const xtstring& sName, const int nColumns, const double nWidth, const double nGutter)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  long lID;
  CPDFFrame *pFrame = pDoc->GetFrame(sName.c_str(), lID);
  if (!pFrame)
    return false;
  bool_a bRet = pFrame->SetFrameColumns(nColumns, pDoc->ConvertTwipsToPoints((int)nWidth),
                                        pDoc->ConvertTwipsToPoints((int)nGutter));
  return (bRet != false_a);
}

int CConvertWorkerToPdf::CreateTextFrame(const xtstring& id, const xtstring& sName, const xtstring& sJoin, const xtstring& sLibrary, int nLeft, int nTop, int nWidth, int nHeight, int nVertAlign, int nHorzAlign,
                int nType/*=POSFRAME_PAGE*/, int LineThickness/*=-1*/, _ColorDef LineColor/*=-1*/, _ColorDef FillColor/*=-1*/,
                int nWrap/*=0*/, int nTextDirection/*=0*/)
{
  long lID = -1;
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return lID;
  CPDFFrame *pFrame;
  pFrame = pDoc->GetFrame(sName.c_str(), lID);
  if (pFrame)
  {
    SetActiveCtrlType(eFrameCtrl);
    m_nFrameID = lID;
    return lID;
  }
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return lID;
  CPDFSection *pSect = pDoc->GetSection(pDoc->GetCurrentPage());
  if (!pSect)
    return lID;
  nHeight = pDoc->ConvertTwipsToPoints(nHeight);
  nWidth = pDoc->ConvertTwipsToPoints(nWidth);
//  nTop = pPage->GetHeight() - pSect->GetTopMargin() - nTop;
  nLeft += pSect->GetLeftMargin();

  rec_a recPosSize;
  recPosSize.lBottom = nTop - nHeight;
  recPosSize.lLeft = nLeft;
  recPosSize.lRight = nLeft + nWidth;
  recPosSize.lTop = nTop;

  pFrame = pDoc->CreateFrame(pDoc->GetCurrentPage(), recPosSize,
                                  (CPDFFrame::eYPosType)nType, lID, sName.c_str());
  if (!pFrame)
    return lID;
  if (!pFrame->SetBorder((long)LineThickness, (long)LineThickness, (long)LineThickness, (long)LineThickness))
    return lID;

  if (!ConvertColorToPDFColor(FillColor, pFrame->GetBgColor()))
    return false;
  if (!ConvertColorToPDFColor(LineColor, pFrame->GetBorderColor()))
    return false;

  if (CPDFColor::eNoColor != pFrame->GetBgColor().GetColorType())
    pFrame->SetTransparentBg(false_a);
  CPDFTextBox *pTextBox = pPage->GetTextArea();
  if (!pTextBox)
    return lID;
  if (!pTextBox->AddFrame(pFrame))
    return lID;
  m_nFrameID = lID;
  SetActiveCtrlType(eFrameCtrl);
  if (!sJoin.empty())
    pDoc->JoinFrames(pFrame, sJoin.c_str());

  if (nHeight < 0)
    pFrame->SetResizeType(CPDFFrame::eFullResize);
  else
    pFrame->SetResizeType(CPDFFrame::eGrowResize);

  short sVertAlign = TF_UP;
  switch (nVertAlign)
  {
  case TAG_TABLE_VERTALIGN_CENTER: // Center alignment
    sVertAlign = TF_MIDDLE; break;
  case TAG_TABLE_VERTALIGN_BOTTOM: // Bottom alignment
    sVertAlign = TF_DOWN; break;
  case TAG_TABLE_VERTALIGN_TOP:  // Align base line of the text
    sVertAlign = TF_UP; break;
  default:
    assert(0); return lID;
  }
  pFrame->SetVerticalAlignment(sVertAlign);

  switch (nWrap)
  {
  case TAG_TEXTFRAME_WRAP_C_NOTEXT:
    pFrame->SetOverlapType(CPDFFrame::eNoOverlap); break;
  case TAG_TEXTFRAME_WRAP_C_AROUNDBOX:
    pFrame->SetOverlapType(CPDFFrame::eAroundOverlap); break;
  default:
    return lID;
  }

  pFrame->SetRotate(nTextDirection);

  return lID;
}

bool CConvertWorkerToPdf::CheckTextFrameOverset(const xtstring& sName)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  long lID = -1;
  CPDFFrame *pFrame = pDoc->GetFrame(sName.c_str(), lID);
  if (!pFrame)
    return false;
  bool_a bRet = pFrame->Overflow();
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::Line(int nX1, int nY1, int nX2, int nY2, int nWidth, _ColorDef LineColor)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  CPDFSection *pSect = pDoc->GetSection(pDoc->GetCurrentPage());
  if (!pSect)
    return false;
  long lTop;
  long lLeft = pSect->GetLeftMargin();

  if (!pPage->HasHeader())
    lTop = pPage->GetHeight() - pSect->GetTopMargin();
  else
    lTop = pPage->GetHeader()->GetPosSize().lTop;

  pDoc->ConvertTwipsToPoints(nX1);
  pDoc->ConvertTwipsToPoints(nY1);
  pDoc->ConvertTwipsToPoints(nX2);
  pDoc->ConvertTwipsToPoints(nY2);
  nX1 += lLeft;
  nY1 = lTop - nY1;
  nX2 += lLeft;
  nY2 = lTop - nY2;

  CPDFColor tempColor;
  if (!ConvertColorToPDFColor(LineColor, tempColor))
    return false;

  if (!pDoc->SetLineColor(tempColor))
    return false;
  if (!pDoc->SetLineWidth((long)nWidth))
    return false;
  if (!pDoc->DrawLine((long)nX1, (long)nY1, (long)nX2, (long)nY2))
    return false;
  return true;
}

bool CConvertWorkerToPdf::Margins(int nLeft, int nRight, int Top, int Button)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  bool_a bRet = pPage->SetMargins(pDoc->ConvertTwipsToPoints(nLeft), pDoc->ConvertTwipsToPoints(nRight),
                            pDoc->ConvertTwipsToPoints(Top), pDoc->ConvertTwipsToPoints(Button));
  if (bRet)
    bRet = pDoc->CheckPageOverflow();
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::SetSectionOrientation(int nOrient/*=SECTION_PORTRAIT*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  if (!pPage->SetOrientation(nOrient == TAG_SECTION_PORTRAIT ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE))
    return false;
  if (!pDoc->CheckPageOverflow())
    return false;
  return true;
}

bool CConvertWorkerToPdf::SetPaperSize(int nDmConstant, int nWidth/*=0*/, int nLength/*=0*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  if (!pPage->SetSize(nDmConstant, pDoc->ConvertTwipsToPoints(nWidth), pDoc->ConvertTwipsToPoints(nLength)))
    return false;
  if (!pDoc->CheckPageOverflow())
    return false;
  return true;
}

bool CConvertWorkerToPdf::DocumentInfo(const xtstring& cTag)
{
  xtstring cParValue[4];
  xtstring cInTag(cTag);

  struct PDFDocumentInfo {
    const char  *constante;
    eDOCINFO   methodval;
  };

  PDFDocumentInfo docinfo[] = {
    {TAG_PDF_DOC_INFO_TITLE, eTitle },
    {TAG_PDF_DOC_INFO_AUTHOR, eAuthor },
    {TAG_PDF_DOC_INFO_SUBJECT, eSubject},
    {TAG_PDF_DOC_INFO_KEYWORDS, eKeywords},
    {TAG_PDF_DOC_INFO_CREATOR, eCreator},
  //  {TAG_PDF_DOC_INFO_PRODUCER, eProducer}, // nicht erlaubt zu setzen
    {TAG_PDF_DOC_INFO_CREATEDATE, eCreationDate},
    {TAG_PDF_DOC_INFO_MODDATE, eModDate},
  };

  bool bReturn = true;
  for (size_t nI = 0; nI < sizeof (docinfo) / sizeof(PDFDocumentInfo); nI++)
  {
    if (cInTag.FindNoCase(docinfo[nI].constante) != xtstring::npos)
    {
      GETPARAM1(xtstring(docinfo[nI].constante), cParValue[0]);
      m_pCurDoc->SetDocInfo(docinfo[nI].methodval, cParValue[0].c_str());
      //break;
    }
  }
  return bReturn;

}

bool CConvertWorkerToPdf::MergeField(const xtstring& cTag)
{
  return true;
}

bool CConvertWorkerToPdf::Field(const xtstring& cTag)
{
  return true;
}

bool CConvertWorkerToPdf::MoveFormField(const xtstring& sName, long i_lDeltaX, long i_lDeltaY)
{
  if (!m_pCurDoc)
    return false;
  i_lDeltaX = m_pCurDoc->ConvertTwipsToPoints(i_lDeltaX);
  i_lDeltaY = m_pCurDoc->ConvertTwipsToPoints(-i_lDeltaY);
  if (m_pCurDoc->IsTemplate())
  {
    if (!sName.empty())
    {
      if (!m_pCurDoc->MoveForm(i_lDeltaX, i_lDeltaY, sName.c_str()))
        return false;
    }
    else
    {
      if (!m_pCurDoc->MoveForm(i_lDeltaX, i_lDeltaY))
        return false;
    }
    return true;
  }
  return false;
}

bool CConvertWorkerToPdf::FormField(const xtstring& sName, const xtstring& sValue, const xtstring& sType,
                const xtstring& sMask, const int nLength, const xtstring& sFontName, const int nFontSize, const int nCodePage)
{
  if (!m_pCurDoc)
    return false;
  if (m_pCurDoc->IsTemplate())
  {
    if (!m_pCurDoc->SetFormField(sName.c_str(), sValue.c_str()))
      return false;
    //if (!sFontName.empty() || nFontSize > 0 || nCodePage > 0)
    {
      if (!m_pCurDoc->SetFormFieldFont(sName.c_str(), sFontName.c_str(), nFontSize,
                                        nCodePage <= 0 ? atol(m_sCodePage.c_str()) : nCodePage))
        return false;
    }
    return true;
  }
  return false;
}

bool CConvertWorkerToPdf::CheckFormFieldOverset(const xtstring& cName, const xtstring& cValue)
{
  if (!m_pCurDoc)
    return false;
  if (m_pCurDoc->IsTemplate())
  {
    if (!m_pCurDoc->CheckFormFieldOverflow(cName.c_str(), cValue.c_str()))
      return false;
    return true;
  }
  return false;
}

bool CConvertWorkerToPdf::GetFormFieldValue(const xtstring& cName, xtstring& cValue)
{
  if (!m_pCurDoc)
    return false;
  if (m_pCurDoc->IsTemplate())
  {
    const char *pRet = NULL;
    pRet = m_pCurDoc->GetFormFieldValue(cName.c_str());
    if (!pRet)
      return false;
    cValue = pRet;
    return true;
  }
  return false;
}

bool CConvertWorkerToPdf::SetPageCount(int nCount)
{
  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if (!pTextBox->AddPageMark(ePageCount))
    return false;

  return true;
}

bool CConvertWorkerToPdf::SetCurrentPage(int nCount)
{
  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if (!pTextBox->AddPageMark(ePageNum))
    return false;

  return true;
}

bool CConvertWorkerToPdf::InsertPageBreak()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
// Kannumbruch löschen
  pDoc->CanPageBreak(true);

  if (!pDoc->InsertPageBreak())
    return false;

  return true;
}

bool CConvertWorkerToPdf::InsertSectionBreak()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;

  // Kannumbruch löschen
//  pDoc->CanPageBreak(true);

  bool_a bRet = pDoc->InsertSectionBreak();

  return (bRet != false_a);
}

bool CConvertWorkerToPdf::InsertColumnBreak()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  bool_a bRet = pDoc->InsertColumnBreak();
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::InsertPosFrameBreak()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  if (m_nFrameID < 0)
    return false;

  CPDFFrame *pFrame = pDoc->GetFrame(m_nFrameID);
  if (!pFrame)
    return false;
  bool_a bRet = pFrame->InsertBreak();

  return (bRet != false_a);
}

bool CConvertWorkerToPdf::SetRowPosition(int nTableID, int wRow, int indent)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1)
  //  wRow = m_nActiveRow;
  //else
    wRow = _GET_ROW(wRow);

  //if (m_TableStack.size() > 1)
  //{
  //  TableStackVector::iterator iter;
  //  for (iter = m_TableStack.begin() + 1; iter < m_TableStack.end(); iter++)
  //  {
  //    if ((*iter).m_nTableID == nTableID)
  //    {
  //      return true;
  //    }
  //  }
  //}

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;

  CPDFTextBox *pTextBox = pTable->GetInTextBox();
  if (!pTextBox)
    return false;

  long lPos = pDoc->ConvertTwipsToPoints(indent);
  long lWidth;
  //lWidth -= lPos;
  //if (lWidth <= 0)
  //  return false;
  //pTable->SetWidth
  lPos += pTextBox->GetLeftDrawPos();

  if (0 == wRow)
  {
    if (!pTable->MoveTo(lPos, pTable->GetYPos()))
      return false;

    for (wRow = 1; wRow <= pTable->GetRowCount(); wRow++)
    {
      lWidth = pTable->GetColsWidth(pTable->GetColCount(wRow - 1), wRow - 1);
      CPDFCell *pCell = pTable->GetCell(cell(wRow - 1, 0));
      if (!pCell)
        return false;
      long lS1 = (((CPDFTableCell *)pCell)->GetPosSize().lLeft);
      lS1 += lWidth;
      long lS2 = (pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin());
      if (lS1 > lS2)
      {
        lWidth = (((CPDFTableCell *)pCell)->GetPosSize().lLeft + lWidth) - (pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin());
        lWidth /= pTable->GetColCount(wRow - 1);
        if (lWidth == 0)
          lWidth = (((CPDFTableCell *)pCell)->GetPosSize().lLeft + lWidth) - (pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin());
        for (long i = 0; i < pTable->GetColCount(wRow - 1); i++)
        {
          if ( !pTable->SetCellWidth( cell(wRow - 1, i), pTable->GetCellWidth(cell(wRow - 1, i))  - lWidth) )
            return false;
        }
      }
    }
  }
  else
  {
    if (!pTable->MoveRowTo(wRow-1, lPos))
      return false;

    lWidth = pTable->GetColsWidth(pTable->GetColCount(wRow - 1), wRow - 1);
    CPDFCell *pCell = pTable->GetCell(cell(wRow - 1, 0));
    if (!pCell)
      return false;
    long lS1 = (((CPDFTableCell *)pCell)->GetPosSize().lLeft);
    lS1 += lWidth;
    long lS2 = (pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin());
    if (lS1 > lS2)
    {
      lWidth = (((CPDFTableCell *)pCell)->GetPosSize().lLeft + lWidth) - (pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin());
      lWidth /= pTable->GetColCount(wRow - 1);
      if (lWidth == 0)
        lWidth = (((CPDFTableCell *)pCell)->GetPosSize().lLeft + lWidth) - (pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin());
      for (long i = 0; i < pTable->GetColCount(wRow - 1); i++)
      {
        if ( !pTable->SetCellWidth( cell(wRow - 1, i), pTable->GetCellWidth(cell(wRow - 1, i))  - lWidth) )
          return false;
      }
    }
  }
  return true;
}

bool CConvertWorkerToPdf::SetRowHeight(int nTableID, int wRow, int wCol, double fRowHeight, long nPos/*=-1*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;

  long lHeight = pDoc->ConvertMMToPoints(fRowHeight);
  wRow = _GET_ROW(wRow);

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;


  if (0 == wRow)
  {
    for (wRow = 1; wRow <= pTable->GetRowCount(); wRow++)
    {
      if (!pTable->SetRowHeight(wRow - 1, lHeight, true_a))
        return false;
    }
  }
  else
  {
    if (!pTable->SetRowHeight(wRow - 1, lHeight, true_a))
      return false;
  }
  return true;
}

bool CConvertWorkerToPdf::CanPageBreak(bool bDelete)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  bool_a bRet = pDoc->CanPageBreak(bDelete);
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::CheckCanPageBreak()
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  bool_a bRet = pDoc->CheckCanPageBreak();
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::EditSection(int NumCols, int ColSpace, int FirstPageNo)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  bool_a bRet = pPage->SetColumns(NumCols, pDoc->ConvertTwipsToPoints(ColSpace));
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::InsertBookmark(xtstring& sLevel, xtstring& sText, xtstring& sSymbol, int nZoom, bool bIgnorePos, int nColor, int nStyle)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  float fR = ((unsigned char)nColor) / 255.f;
  float fG = ((unsigned char) (nColor >>  8)) / 255.f;
  float fB = ((unsigned char) (nColor >> 16)) /255.f;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;

  CPDFOutlineEntry *pOE = pDoc->CreateOutlineEntry(pDoc->GetCurrentPage(), (char *)sLevel.c_str(),
                            (char *)sText.c_str(),
                            0, 0, nZoom, // will be set later
                            fR, fG, fB, nStyle);
  if (!pOE)
    return false;

  pOE->SetIgnorePos(bIgnorePos);
  pOE->SetAnchor(sSymbol.c_str());

  return true;
}

bool CConvertWorkerToPdf::InsertAnchor(const xtstring& sAnchorName)
{
  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;
  if (!pTextBox->AddAnchor(sAnchorName.c_str()))
    return false;
  return true;
}

bool CConvertWorkerToPdf::InsertLink(const xtstring& sReferenzName, const xtstring& sSymbolName, int nPage, int nZoom)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return false;
  const char *pSymbolName = NULL;
  if (!sSymbolName.empty())
    pSymbolName = sSymbolName.c_str();
  bool_a bF = false_a;
  bool_a bEnd = false_a;
  if (sSymbolName.empty() && sReferenzName.empty() && !nPage && !nZoom)
    bEnd = true_a;
  if (!strncmp(sReferenzName.c_str(), "http://", 7))
    bF = true_a;

  CPDFTextBox *pTextBox = GetActiveCtrl();
  if (!pTextBox)
    return false;

  if (bEnd)
  {
    pTextBox->EndLink();
  }
  else
  {
    if (sReferenzName.empty())
      pTextBox->BeginLink(pSymbolName, nPage, eDestLeftTopZoom, 0, pPage->GetHeight(), 0, 0, nZoom);
    else
    {
      if (bF)
        pTextBox->BeginLink(pSymbolName, sReferenzName.c_str());
      else
        pTextBox->BeginLink(pSymbolName, nPage, eDestLeftTopZoom, 0, pPage->GetHeight(), 0, 0,
                            nZoom, sReferenzName.c_str());
    }
  }

  return true;
}


int CConvertWorkerToPdf::CreateTable(const xtstring& id, const xtstring& sName, int wRows, int wColumns,
																				int numHeaderRows, int numFooterRows, double spacebefore, double spaceafter)
{
  long lID = -1;
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return lID;
  long lHeight, lWidth;
  CPDFPage *pPage = pDoc->GetPage(pDoc->GetCurrentPage());
  if (!pPage)
    return lID;
  CPDFTextBox *pTextBox = NULL;

  pTextBox = GetActiveCtrl();

  if (!pTextBox)
    return lID;
  if (!pTextBox->GetSize(lWidth, lHeight))
    return lID;

  lWidth = pTextBox->GetDrawWidth(true_a);// + pTextBox->GetLeftMargin() + pTextBox->GetRightMargin();
  lWidth = (long)(lWidth / wColumns);

  lHeight = pDoc->GetActualFontHeight();

  if(!m_TableStack.empty())
  {
    TableStackVector::iterator iter = m_TableStack.end() - 1;
    (*iter).m_nTableRow = GetTableRow();
    (*iter).m_nTableCol = GetTableCol();
  }

  CPDFTable *pTable = NULL;
  if (!(pTable = pDoc->CreateTable(pDoc->GetCurrentPage(), wRows, wColumns,
                                        pTextBox->GetLeftDrawPos()/* - pTextBox->GetLeftMargin()*/,
                                        pTextBox->GetTopDrawPos(), lWidth, lHeight, lID)))
    lID = -1;
  else
  {
    if (eTableCtrl == GetActiveCtrlType())
    {
      CPDFTable *pParent = pDoc->GetTable(GetTable());
      if (!pParent)
        return -1;


      CPDFCell *pCell = pParent->GetCell(cell(GetRow(), GetCol()));
      if (!pCell || pCell->IsTable())
        return -1;
      ((CPDFTableCell *)pCell)->AddTable(pTable);
      //if (!pParent->InsertTableToCell(cell(GetRow(), GetCol()), pTable))
      //  return -1;


      //SetActiveCtrlType(eNestTableCtrl);
      m_nActiveNestTableID = lID;
      m_nActiveNestTableRowPos = GetRow() + 1;
      m_nActiveNestTableColPos = GetCol() + 1;
      //lID = GetTable();
    }
    else
    {
      pTextBox->AddTable(pTable);
      //m_bInTable = true;
      SetActiveCtrlType(eTableCtrl);
    }
  }

  CTableStackItem cTableStack;
  cTableStack.m_nTableCol = GetTableCol();
  cTableStack.m_nTableRow = GetTableRow();
  cTableStack.m_nTableID = lID;

  m_TableStack.push_back(cTableStack);


  if (!pDoc->CheckPageOverflow())
    return -1;
  return lID;
}

bool CConvertWorkerToPdf::CellBackgroundColor(int nTableID, int wRow, int wCol, _ColorDef crBkColor)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
      lColCount = pTable->GetColCount(lRow - 1);
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {

      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      if (!pCell)
        continue;
      if (pCell->IsTable())
        continue;

      if (!ConvertColorToPDFColor(crBkColor, ((CPDFTableCell *)pCell)->GetBgColor()))
        return false;
      if (!((CPDFTableCell *)pCell)->SetTransparentBg(false_a))
        return false;
    }
  }
  return true;
}

bool CConvertWorkerToPdf::SetCellBorderWidth(int nTableID, int wRow, int wCol, int nLeft, int nTop, int nRight, int nButton)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
      lColCount = pTable->GetColCount(lRow - 1);
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {
      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      if (!pCell)
        continue;

      long lBorderType = pCell->GetBorderType();
      if (nLeft <= 0)
        lBorderType &= ~CPDFCell::eLeftBorder;
      else
        lBorderType = (lBorderType | CPDFCell::eLeftBorder);
      if (nTop <= 0)
        lBorderType &= ~CPDFCell::eTopBorder;
      else
        lBorderType = (lBorderType | CPDFCell::eTopBorder);
      if (nRight <= 0)
        lBorderType &= ~CPDFCell::eRightBorder;
      else
        lBorderType = (lBorderType | CPDFCell::eRightBorder);
      if (nButton <= 0)
        lBorderType &= ~CPDFCell::eBottomBorder;
      else
        lBorderType = (lBorderType | CPDFCell::eBottomBorder);

      pCell->SetBorderWidth(CPDFCell::eLeftBorder, pDoc->ConvertTwipsToPoints(nLeft));
      pCell->SetBorderWidth(CPDFCell::eTopBorder, pDoc->ConvertTwipsToPoints(nTop));
      pCell->SetBorderWidth(CPDFCell::eRightBorder, pDoc->ConvertTwipsToPoints(nRight));
      pCell->SetBorderWidth(CPDFCell::eBottomBorder, pDoc->ConvertTwipsToPoints(nButton));

      pCell->SetBorderType(lBorderType);
    }
  }

  return true;
}

bool CConvertWorkerToPdf::SetCursorAfterTable(int nTableID)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if(m_TableStack.empty())
  //  return false;

  //CTableStackItem cTableStack;
  //m_TableStack.pop_back();
  //cTableStack = m_TableStack.back();


  //m_bInTable = false;
  CPDFTable *pTable = pDoc->GetTable(GetTable());
  if (!pTable)
    return false;
  //if (eTableCtrl == GetActiveCtrlType())
  //{
    if (!pTable->EndEdit())
      return false;
  //}
  //else
  //{
  //  m_nActiveNestTableID = -1;
  //  m_nActiveNestTableRowPos = -1;
  //  m_nActiveNestTableColPos = -1;
  //}
  SetActiveCtrlType(eNoCtrl);
  return true;
}

bool CConvertWorkerToPdf::SetTableHeaderRow(int nTableID, int wRow, int wCol, int bSet)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  long lBgnRow, lEndRow, lBgnCol, lEndCol;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    pTable->SetHeaderRow(lRow - 1, bSet);
  }
  return true;
}

bool CConvertWorkerToPdf::SetTableRowKeep(int nTableID, int wRow, int wCol, bool bSet)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  long lBgnRow, lEndRow, lBgnCol, lEndCol;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    pTable->SetKeepRowTogether(lRow - 1, bSet);
  }
  return true;
}

bool CConvertWorkerToPdf::SetCellRotate(int nTableID,
                                        int wRow,
                                        int wCol,
                                        int nDirection)
{
  // MIK -->
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  CPDFTextBox *pTextBox = pTable->GetInTextBox();

  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
      lColCount = pTable->GetColCount(lRow - 1);
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {
      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      if (!pCell)
        continue;
      CPDFTextBox *pTextBox = GetActiveCtrl();

      eTEXTDIRECTION nDir;
      if (nDirection == TAG_TABLE_ROTATE_HORZ) // normale Darstellung in Zelle
        nDir = eHorizontal;
      else if (nDirection == TAG_TABLE_ROTATE_TOPTOBOT) // von oben nach unten
        nDir = eVerticalDown;
      else if (nDirection == TAG_TABLE_ROTATE_BOTTOTOP) // von unten nach oben
        nDir = eVerticalUp;
      else
        continue;

      if (!pCell->IsTable())
        ((CPDFTableCell *)pCell)->SetTextDirection(nDir);
    }
  }
  return true;
  // MIK <--
}

int CConvertWorkerToPdf::GetTableID()
{
  return m_nTableID;
}

bool CConvertWorkerToPdf::SetTableID(int nTableID)
{
  m_nTableID = nTableID;
  return true;
}

bool CConvertWorkerToPdf::SetTableRow(int nTableRow)
{
  m_nTableRow = _GET_ROW(nTableRow);
  return true;
}

bool CConvertWorkerToPdf::SetTableCol(int nTableCol)
{
  m_nTableCol = _GET_COL(nTableCol);
  return true;
}

int CConvertWorkerToPdf::GetTableRow()
{
  return m_nTableRow;
}

int CConvertWorkerToPdf::GetTableCol()
{
  return m_nTableCol;
}

bool CConvertWorkerToPdf::GetRowAndCols(int nTableID, int& wRows, int& wCols)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRows = pTable->GetRowCount();
  wCols = pTable->GetMaxColCount();
  if (wRows < 0 || wCols < 0)
    return false;
  return true;
}

bool CConvertWorkerToPdf::InsertTableRow(int nTableID, int nInsert, int wLines/*=1*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  long lPage = -1;
  long lRow = GetRow();

  CPDFTable *pTable;
  pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;
  lPage = pTable->GetPageNumber();

  bool_a bRet = true;
  for (int i = 0; i < wLines && bRet; i++)
  {
    if (!pTable->InsertRow(-1, nInsert == TAG_TABLE_ROW_APPEND_CURRENT ? lRow : -1))
      bRet = false;
  }
  if (bRet)
    bRet = pDoc->CheckPageOverflow(lPage);
  return (bRet != false);
}

bool CConvertWorkerToPdf::SetTableCellActivate(int nTableID, int wRow, int wCol, int nPos/*=TAG_POS_END*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;

  wRow = _GET_ROW(wRow);
  wCol = _GET_COL(wCol);

  if( (wRow - 1) < 0 || (wCol - 1) < 0)
  {
    m_nActiveTableID = -1;
    m_nActiveRow = -1;
    m_nActiveCol = -1;
    return false;
  }
  m_nActiveTableID = nTableID;
  m_nActiveRow = wRow;
  m_nActiveCol = wCol;

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;
  if (m_nActiveRow < 1)
    m_nActiveRow = 1;
  if (m_nActiveRow > pTable->GetRowCount())
  {
    m_nActiveRow = pTable->GetRowCount();
    m_nActiveCol = 1;
  }
  else
  {
    if (m_nActiveCol < 1)
      m_nActiveCol = 1;
    if (m_nActiveCol > pTable->GetColCount(m_nActiveRow - 1))
      m_nActiveCol = pTable->GetColCount(m_nActiveRow - 1);
  }

//MAK
  int wNestRow = -2, wNestCol = -2;

  if (wNestRow == -2 && wNestCol == -2)
  {
    m_nActiveNestRow = -1;
    m_nActiveNestCol = -1;
  }
  else
  {
    // not used. CPDFCell *pCell = pTable->GetCell(cell(m_nActiveNestTableRowPos - 1, m_nActiveNestTableColPos - 1));
    //if (!pCell->IsTable())
    //  return false;

    //ToDo: add check - pTable must be in pCell;
    pTable = pDoc->GetTable(m_nActiveNestTableID);//(CPDFTable *)pCell;
    if (!pTable)
      return false;
    m_nActiveNestRow = wNestRow;
    m_nActiveNestCol = wNestCol;

    if (m_nActiveNestRow < 1)
      m_nActiveNestRow = 1;
    if (m_nActiveNestRow > pTable->GetRowCount())
      m_nActiveNestRow = pTable->GetRowCount();
    if (m_nActiveNestCol < 1)
      m_nActiveNestCol = 1;
    if (m_nActiveNestCol > pTable->GetColCount(m_nActiveRow - 1))
      m_nActiveNestCol = pTable->GetColCount(m_nActiveRow - 1);
  }

  //m_bInTable = true;
  return true;
}

bool CConvertWorkerToPdf::SetCellWidth(int nTableID, int wRow, int wCol, double fWidth, long nPos/*=-1*/)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  CPDFTextBox *pTextBox = pTable->GetInTextBox();

  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount, lWidth;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
      lColCount = pTable->GetColCount(lRow - 1);
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {
      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      if (!pCell)
        continue;

      lWidth = pTable->GetCellWidth(cell(lRow - 1, lCol - 1));

      if (!pTable->SetCellWidth(cell(lRow - 1, lCol - 1), pDoc->ConvertMMToPoints(fWidth)))
        return false;
      if (pTextBox->GetTBType() == CPDFTextBox::eTBTableCell)
      {
        if ((pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin()) < (pTable->GetXPos() + pTable->GetMaxWidth()))
        {
          lWidth = pTextBox->GetRightDrawPos() + pTextBox->GetRightMargin() - pTable->GetXPos();
          lWidth -= pTable->GetMaxWidth() - pDoc->ConvertMMToPoints(fWidth);
          pTable->SetCellWidth(cell(lRow - 1, lCol - 1), lWidth);
          return false;
        }
      }
    }
  }
  return true;
}

bool CConvertWorkerToPdf::SetCellBorderColor(int nTableID, int wRow, int wCol, _ColorDef cClrfLeft,
                                             _ColorDef cClrfTop, _ColorDef cClrfRight, _ColorDef cClrfButtom)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
      lColCount = pTable->GetColCount(lRow - 1);
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {
      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      CPDFColor tempColor;
      if (!pCell)
        continue;
      if(cClrfLeft != -1)
      {
        if (!ConvertColorToPDFColor(cClrfLeft, tempColor))
          return false;
        pCell->SetBorderColor(CPDFCell::eLeftBorder | CPDFCell::eLeftDoubleBorder, tempColor);
      }
      if(cClrfRight != -1)
      {
        if (!ConvertColorToPDFColor(cClrfRight, tempColor))
          return false;
        pCell->SetBorderColor(CPDFCell::eRightBorder | CPDFCell::eRightDoubleBorder, tempColor);
      }
      if(cClrfTop != -1)
      {
        if (!ConvertColorToPDFColor(cClrfTop, tempColor))
          return false;
        pCell->SetBorderColor(CPDFCell::eTopBorder | CPDFCell::eTopDoubleBorder, tempColor);
      }
      if(cClrfButtom != -1)
      {
        if (!ConvertColorToPDFColor(cClrfButtom, tempColor))
          return false;
        pCell->SetBorderColor(CPDFCell::eBottomBorder | CPDFCell::eBottomDoubleBorder, tempColor);
      }
    }
  }
  return true;
}

bool CConvertWorkerToPdf::CellVerticalAlign(int nTableID, int wRow, int wCol, unsigned int nAlign)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  short sVertAlign = TF_UP;
  switch (nAlign)
  {
  case TAG_TABLE_VERTALIGN_CENTER: // Center alignment
    sVertAlign = TF_MIDDLE;
    break;
  case TAG_TABLE_VERTALIGN_BOTTOM: // Bottom alignment
    sVertAlign = TF_DOWN;
    break;
  case TAG_TABLE_VERTALIGN_TOP:  // Align base line of the text
    sVertAlign = TF_UP;
    break;
  default:
    assert(0);
    return false;
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);
  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
      lColCount = pTable->GetColCount(lRow - 1);
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {
      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      if (!pCell)
        continue;
      if (!pCell->IsTable())
        ((CPDFTableCell *)pCell)->SetVerticalAlignment(sVertAlign);
    }
  }
  return true;
}

bool CConvertWorkerToPdf::CellMargin(int nTableID, int wRow, int wCol,
                                    int nLeft, int nTop, int nRight, int nButton)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  bool_a bRet = false_a;
  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);
  for (long lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
      lColCount = pTable->GetColCount(lRow - 1);
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {
      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      if (!pCell)
        continue;
      if (pCell->IsTable())
        return false;
      bRet = ((CPDFTableCell *)pCell)->SetMargins(pDoc->ConvertTwipsToPoints(nLeft), pDoc->ConvertTwipsToPoints(nTop),
                                            pDoc->ConvertTwipsToPoints(nRight), pDoc->ConvertTwipsToPoints(nButton));
    }
  }
  return (bRet != false_a);
}

bool CConvertWorkerToPdf::DeleteColumn(int nTableID, int wRow, int wCol)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  long lBgnRow, lEndRow, lBgnCol, lEndCol, lColCount;
  GetCellRange(pTable, wRow, wCol, lBgnRow, lEndRow, lBgnCol, lEndCol);

  long lRow;
  for (lRow = lBgnRow; lRow <= lEndRow; lRow++)
  {
    if (lEndCol == -1)
    {
      pTable->DeleteRow(lRow - 1);
      continue;
//      lColCount = pTable->GetColCount(lRow - 1);
    }
    else
    {
//      lColCount = lEndCol;
      lBgnCol = lColCount = lEndCol > pTable->GetColCount(lRow - 1) ? pTable->GetColCount(lRow - 1) : lEndCol;
    }
    for (long lCol = lBgnCol; lCol <= lColCount; lCol++)
    {
      CPDFCell *pCell = pTable->GetCell(lRow - 1, lCol - 1);
      if (!pCell)
        continue;
      if (!pTable->DeleteColumn(lCol - 1, lRow - 1))
        return false;

    }
  }

  pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;
  lRow = m_nActiveRow;
  for (; lRow > 0 ; lRow--)
  {
    long lCol;
    if (lRow == m_nActiveRow)
    {
      lCol = m_nActiveCol;
      m_nActiveCol = -1;
      m_nActiveRow = -1;
    }
    else
      lCol = pTable->GetColCount(lRow - 1);
    while (lCol > 0)
    {
      if (pTable->GetCell(lRow - 1, lCol - 1))
      {
        m_nActiveRow = lRow;
        m_nActiveCol = lCol;
        return true;
      }
      lCol--;
    }
  }

  return true;
}

bool CConvertWorkerToPdf::InsertColumn(int nTableID, int wRow, int wCol)
{
  CPDFDocument *pDoc = GetActiveDocumet();
  if (!pDoc)
    return false;
  //if (m_nActiveTableID != -1)
  //  nTableID = m_nActiveTableID;
  //if (m_nActiveRow != -1 && m_nActiveCol != -1)
  //{
  //  wRow = m_nActiveRow;
  //  wCol = m_nActiveCol;
  //}
  //else
  {
    wRow = _GET_ROW(wRow);
    wCol = _GET_COL(wCol);
  }

  CPDFTable *pTable = pDoc->GetTable(nTableID);
  if (!pTable)
    return false;

  wRow = wRow > pTable->GetRowCount() ? pTable->GetRowCount() : wRow;
  //wCol = wCol > pTable->GetColCount(wRow - 1) ? pTable->GetColCount(wRow - 1) : wCol;

  if (!pTable->InsertColumn(wCol - 1, wRow - 1))
    return false;

  return true;
}

bool CConvertWorkerToPdf::CreateDefinedStyle(CXPubParagraphStyle* pStyle)
{
  bool bRet = CConvertWorkerTo::CreateDefinedStyle(pStyle);

  bool bRegisterNow = false;
  if (pStyle->GetStyleIsPermanent()
      && (pStyle->GetStyleID() == -1))
    bRegisterNow = true;

  if (pStyle->GetStyleName() == DEFAULT_STYLE_NAME)
  {
    pStyle->SetStyleID(0);
    bRegisterNow = true;
  }

  if (bRegisterNow)
  {
    // die gesetzten Styles ans Ter übertragen
    if (pStyle->GetStyleID() == -1)
      pStyle->SetStyleID(m_nLastStyleID++);

// hier kommen die Setzungen 
    SetFont(pStyle->GetFontName(), pStyle->GetFontSize(), pStyle->GetCodePage());
    SetTextColor(pStyle->GetFontColor());
    SetFontAttribute(pStyle->GetBold() ? TAG_TEXT_BOLD : TAG_TEXT_BOLD_END);
    SetFontAttribute(pStyle->GetUnderline() ? TAG_TEXT_UNDERLINE : TAG_TEXT_UNDERLINE_END);
    SetFontAttribute(pStyle->GetItalic() ? TAG_TEXT_ITALIC : TAG_TEXT_ITALIC_END);
    SetFontAttribute(pStyle->GetDoubleUnderline() ? TAG_TEXT_UL_DOUBLE : TAG_TEXT_UL_DOUBLE_END);
    SetFontAttribute(pStyle->GetStrikeOut() ? TAG_TEXT_STRIKE : TAG_TEXT_STRIKE_END);

  // Paragraph
    int nLeft, nRight, nIdent;
    pStyle->GetParagraphIndents(nLeft, nRight, nIdent);
    SetParagraphIndents(nLeft, nRight, nIdent);

  // Paragraph control
    int nKeep, nKeepNext, nWidowOrphon;
    pStyle->GetParagraphControl(nKeep, nKeepNext, nWidowOrphon);
    SetParagraphControl(nKeep, nKeepNext, nWidowOrphon);

    int nPageBreakBefore;
    pStyle->GetPageBreakBefore(nPageBreakBefore);
    ParagraphPageBreak(nPageBreakBefore);

    int nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing;
    pStyle->GetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
    SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);

    int nTextAlign;
    pStyle->GetTextAlignment(nTextAlign);
    TextAlignment(nTextAlign);

//    if (pStyle->CountOfTabStops() == 0)
      ClearTabulators();
//    else
    {
      for(int i=0; i < pStyle->CountOfTabStops(); i++)
      {
        CXPubTabStop* pTabStop = pStyle->GetTabStop(i);
        if(pTabStop)
          InsertTabulator(pTabStop->GetTabFlag(),pTabStop->GetTabPos(), 
                        pTabStop->GetTabDecChar(), pTabStop->GetTabLeader());
      }
    }
    int nTabDefaultPos, nTabDefaultDecimalChar;
    pStyle->GetTabDefaults(nTabDefaultPos, nTabDefaultDecimalChar);
    SetDefTabulator(nTabDefaultPos, nTabDefaultDecimalChar);
/// Ende 
  }

  return bRet;
}

void CConvertWorkerToPdf::StyleEvent(TStyleEvent nStyleEvent,
                                     CXPubParagraphStyle* pStyle)
{
  CConvertWorkerTo::StyleEvent(nStyleEvent, pStyle);

  // tSE_Start Reaktion nicht notwendig
  if (nStyleEvent == tSE_FormatStackStart)
  {
    assert(pStyle);
    CXPubParagraphStyle* pActualStyle = pStyle;
    if (pActualStyle->GetStyleID() == -1)
    {
      // es war noch nicht registriert
      bool bTemp = pActualStyle->GetStyleIsPermanent();
      pActualStyle->SetStyleIsPermanent(true);
      CreateDefinedStyle(pActualStyle);
      pActualStyle->SetStyleIsPermanent(bTemp);
    }

    if (pActualStyle->GetStyleID() != -1)
    {
      SetFont(pActualStyle->GetFontName(), pActualStyle->GetFontSize(), pActualStyle->GetCodePage());
      SetTextColor(pActualStyle->GetFontColor());
      SetFontAttribute(pActualStyle->GetBold() ? TAG_TEXT_BOLD : TAG_TEXT_BOLD_END);
      SetFontAttribute(pActualStyle->GetUnderline() ? TAG_TEXT_UNDERLINE : TAG_TEXT_UNDERLINE_END);
      SetFontAttribute(pActualStyle->GetItalic() ? TAG_TEXT_ITALIC : TAG_TEXT_ITALIC_END);
      SetFontAttribute(pActualStyle->GetDoubleUnderline() ? TAG_TEXT_UL_DOUBLE : TAG_TEXT_UL_DOUBLE_END);
      SetFontAttribute(pActualStyle->GetStrikeOut() ? TAG_TEXT_STRIKE : TAG_TEXT_STRIKE_END);

    // Paragraph
      int nLeft, nRight, nIdent;
      pActualStyle->GetParagraphIndents(nLeft, nRight, nIdent);
      SetParagraphIndents(nLeft, nRight, nIdent);

    // Paragraph control
      int nKeep, nKeepNext, nWidowOrphon;
      pActualStyle->GetParagraphControl(nKeep, nKeepNext, nWidowOrphon);
      SetParagraphControl(nKeep, nKeepNext, nWidowOrphon);

      int nPageBreakBefore;
      pActualStyle->GetPageBreakBefore(nPageBreakBefore);
      ParagraphPageBreak(nPageBreakBefore);

      int nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing;
      pActualStyle->GetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
      SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);

      int nTextAlign;
      pActualStyle->GetTextAlignment(nTextAlign);
      TextAlignment(nTextAlign);

//        if (m_pActualStyle->CountOfTabStops() == 0)
      ClearTabulators();
//        else
      {
        for(int i = 0; i < pActualStyle->CountOfTabStops(); i++)
        {
          CXPubTabStop* pTabStop = pActualStyle->GetTabStop(i);
          if (pTabStop)
            InsertTabulator(pTabStop->GetTabFlag(),pTabStop->GetTabPos(), 
                            pTabStop->GetTabDecChar(), pTabStop->GetTabLeader());
        }
      }
      int nTabDefaultPos, nTabDefaultDecimalChar;
      pActualStyle->GetTabDefaults(nTabDefaultPos, nTabDefaultDecimalChar);
      SetDefTabulator(nTabDefaultPos, nTabDefaultDecimalChar);
    }
  }
  else if (nStyleEvent == tSE_FormatStackAdd
          || nStyleEvent == tSE_FormatStackRemove
          || nStyleEvent == tSE_FormatStackSegmentRemoved)
  {
    assert(pStyle);
    SetFont(pStyle->GetFontName(), pStyle->GetFontSize(), pStyle->GetCodePage());
    SetTextColor(pStyle->GetFontColor());
    SetFontAttribute(pStyle->GetBold() ? TAG_TEXT_BOLD : TAG_TEXT_BOLD_END);
    SetFontAttribute(pStyle->GetUnderline() ? TAG_TEXT_UNDERLINE : TAG_TEXT_UNDERLINE_END);
    SetFontAttribute(pStyle->GetItalic() ? TAG_TEXT_ITALIC : TAG_TEXT_ITALIC_END);
    SetFontAttribute(pStyle->GetDoubleUnderline() ? TAG_TEXT_UL_DOUBLE : TAG_TEXT_UL_DOUBLE_END);
    SetFontAttribute(pStyle->GetStrikeOut() ? TAG_TEXT_STRIKE : TAG_TEXT_STRIKE_END);
  }
  else if (nStyleEvent == tSE_FormatStackParaChanged)
  {
    assert(pStyle);
    // Paragraph control
    int nKeep, nKeepNext, nWidowOrphon;
    pStyle->GetParagraphControl(nKeep, nKeepNext, nWidowOrphon);
    SetParagraphControl(nKeep, nKeepNext, nWidowOrphon);

    int nPageBreakBefore;
    pStyle->GetPageBreakBefore(nPageBreakBefore);
    ParagraphPageBreak(nPageBreakBefore);

    int nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing;
    pStyle->GetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);
    SetParagraphSpacing(nSpaceBefore, nSpaceAfter, nSpaceBetween, nLineSpacing);

    int nTextAlign;
    pStyle->GetTextAlignment(nTextAlign);
    TextAlignment(nTextAlign);

    // if (pStyle->CountOfTabStops() == 0)
    ClearTabulators();
    // else
    {
      for(int i=0; i < pStyle->CountOfTabStops(); i++)
      {
        CXPubTabStop* pTabStop = pStyle->GetTabStop(i);
        if(pTabStop)
          InsertTabulator(pTabStop->GetTabFlag(),pTabStop->GetTabPos(), 
                        pTabStop->GetTabDecChar(), pTabStop->GetTabLeader());
      }
    }
    int nTabDefaultPos, nTabDefaultDecimalChar;
    pStyle->GetTabDefaults(nTabDefaultPos, nTabDefaultDecimalChar);
    SetDefTabulator(nTabDefaultPos, nTabDefaultDecimalChar);
  }
//  else if (nStyleEvent == tSE_FormatStackFormatToRemove)
//  {
//    CXPubParagraphStyle::TStyleType nType;
//    nType = pStyle->GetStyleType();
//  }
}


//class CPDFStrBlockControlTMP :public CPDFStrBlockControl
//{
//public:
//  CPDFStrBlockControlTMP(){};
//  virtual ~CPDFStrBlockControlTMP(){};
//
//virtual void SetLanguageID(CPDFObjects *io_pPDFObj);
//virtual bool CanJoin(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, std::wstring &o_wstrJoin);
//virtual bool CanDivide(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, std::wstring &o_wstrJoin);
//
//};
//
//void CPDFStrBlockControlTMP::SetLanguageID(CPDFObjects *io_pPDFObj)
//{
//  if (!io_pPDFObj)
//    return;
//}
//
//bool CPDFStrBlockControlTMP::CanJoin(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, std::wstring &o_wstrJoin)
//{
//  if (!io_pPDFObjFirst || !io_pPDFObjSecond)
//    return false;
//
//  bool bRet = CPDFStrBlockControl::CanJoin(io_pPDFObjFirst, io_pPDFObjSecond, o_wstrJoin);
//  if (!bRet)
//  {
//    ::MessageBox(0, "asdfasdf", "CanJoin", MB_OK);
//    bRet = CPDFStrBlockControl::CanJoin(io_pPDFObjFirst, io_pPDFObjSecond, o_wstrJoin);
//  }
//  return bRet;
//}
//
//bool CPDFStrBlockControlTMP::CanDivide(CPDFObjects *io_pPDFObjFirst, CPDFObjects *io_pPDFObjSecond, std::wstring &o_wstrJoin)
//{
//  if(!io_pPDFObjFirst || !io_pPDFObjSecond)
//    return true;
//
//  bool bRet = CPDFStrBlockControl::CanDivide(io_pPDFObjFirst, io_pPDFObjSecond, o_wstrJoin);
//  if (!bRet)
//  {
//    ::MessageBox(0, "asdfasdf", "CanDivide", MB_OK);
//    bRet = CPDFStrBlockControl::CanDivide(io_pPDFObjFirst, io_pPDFObjSecond, o_wstrJoin);
//  }
//  return bRet;
//}


bool CConvertWorkerToPdf::LoadTemplate(const xtstring& cFileName, const xtstring& sLayer, const xtstring& sOwnerPwd, int nModus)
{
  if (!m_pCurDoc)
    return false;
  if (!m_pCurDoc->LoadTemplatePDF(cFileName.c_str(), nModus, sOwnerPwd.c_str()))
    return false;

  //CPDFStrBlockControlTMP *pCtrl = new CPDFStrBlockControlTMP();
  //if (!pCtrl)
  //  return false;


  ////for (long tt = 0; tt < m_pCurDoc->GetTemplateCount(); tt++)
  //{
  //  string strFile(cFileName.c_str());
  //  strFile.append(".txt");
  //  FILE *pPdfFile = fopen(strFile.c_str(), "wb+");

  //  if (!m_pCurDoc->BuildStrBlock(m_pCurDoc->GetTemplateCount() - 1, CPDFTemplate::eJoined, pCtrl))
  //    return false;

  //  long lPageCount = m_pCurDoc->GetTemplatePageCount(m_pCurDoc->GetTemplateCount() - 1);
  //  for (long i = 0; i < lPageCount; i++)
  //  {
  //    long lStrBlockCount = m_pCurDoc->GetTemplateStrBlockCount(m_pCurDoc->GetTemplateCount() - 1, i);
  //    for (long j = 0; j < lStrBlockCount; j++)
  //    {
  //      const wchar_t *pWChar = m_pCurDoc->GetTemplateStrBlock(m_pCurDoc->GetTemplateCount() - 1, i, j);
  //      if (pWChar)
  //        pWChar = pWChar;

  //      fwrite(pWChar, 2, wcslen(pWChar), pPdfFile);
  //      fwrite("\r\n", 1, sizeof("\r\n"), pPdfFile);
  //    }
  //  }
  //  const char *pTmp = m_pCurDoc->GetTemplateStrBlockFontName(m_pCurDoc->GetTemplateCount() - 1, 0, 0);

  //  fclose(pPdfFile);
  //}

  //delete pCtrl;

  return true;
}
