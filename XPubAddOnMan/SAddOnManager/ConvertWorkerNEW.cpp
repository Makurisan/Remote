#if defined(WIN32)
#include "stdafx.h"
#include "Shlwapi.h"
#endif // WIN32

#include <assert.h>

#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"

#include "XPubAddOnDeclarations.h"
#include "XPubAddOnManager.h"

#include "ConvertWorker.h"



CConvertWorkerProgress::CConvertWorkerProgress()
{
}
CConvertWorkerProgress::~CConvertWorkerProgress()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CConvertFromAddOn
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CConvertFromAddOn::CConvertFromAddOn(CXPubAddOnManager* pAddOnManager)
{
  m_pAddOnManager = pAddOnManager;
}
CConvertFromAddOn::~CConvertFromAddOn()
{
  DestroyAndRemoveAllConvertWorkers();
}
bool CConvertFromAddOn::AddConvertWorkerFrom(CConvertWorkerFrom* pWorker)
{
  if (!pWorker)
  {
    throw(ERR_ADDON_ADDWORKER_WITH_NULL_POINTER);
    return false;
  }

  size_t nCount = m_cWorkers.size();
  m_cWorkers.push_back(pWorker);
  if (nCount == m_cWorkers.size())
    throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);

  return (nCount != m_cWorkers.size());
}

bool CConvertFromAddOn::RemoveConvertWorkerFrom(CConvertWorkerFrom* pWorker)
{
  CConvertWorkersFromIterator it = m_cWorkers.begin();
  while (it != m_cWorkers.end())
  {
    if ((*it) == pWorker)
    {

      // Wir muessen es zuerst aus Liste entfernen
      // Es kann sein, dass mehrere Worker da eingefuegt sind
      // und nach delete waere 'it' ungueltig.
      m_cWorkers.erase(it);
      delete pWorker;
      return true;
    }
    it++;
  }
  throw(ERR_REMOVE_ELEMENT_BUT_NOT_PRESENT_IN_ARRAY);
  return false;
}

bool CConvertFromAddOn::DestroyAndRemoveAllConvertWorkers()
{
  while (m_cWorkers.size())
  {
    CConvertWorkersFromIterator it = m_cWorkers.begin();
    if (it != m_cWorkers.end())
    {

      m_pAddOnManager->ReleaseConvertWorkerFrom(*it);
      //delete (*it);
      //m_cWorkers.erase(m_cWorkers.begin());
    }
  }
  return true;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CConvertToAddOn
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CConvertToAddOn::CConvertToAddOn(CXPubAddOnManager* pAddOnManager)
{
  m_pAddOnManager = pAddOnManager;
}
CConvertToAddOn::~CConvertToAddOn()
{
  DestroyAndRemoveAllConvertWorkers();
}
bool CConvertToAddOn::AddConvertWorkerTo(CConvertWorkerTo* pWorker)
{
  if (!pWorker)
  {
    throw(ERR_ADDON_ADDWORKER_WITH_NULL_POINTER);
    return false;
  }

  size_t nCount = m_cWorkers.size();
  m_cWorkers.push_back(pWorker);
  if (nCount == m_cWorkers.size())
    throw(ERR_ELEMENT_NOT_ADDED_IN_ARRAY);

  return (nCount != m_cWorkers.size());
}

bool CConvertToAddOn::RemoveConvertWorkerTo(CConvertWorkerTo* pWorker)
{
  CConvertWorkersToIterator it = m_cWorkers.begin();
  while (it != m_cWorkers.end())
  {
    if ((*it) == pWorker)
    {

      // Wir muessen es zuerst aus Liste entfernen
      // Es kann sein, dass mehrere Worker da eingefuegt sind
      // und nach delete waere 'it' ungueltig.
      m_cWorkers.erase(it);
      delete pWorker;
      return true;
    }
    it++;
  }
  throw(ERR_REMOVE_ELEMENT_BUT_NOT_PRESENT_IN_ARRAY);
  return false;
}

bool CConvertToAddOn::DestroyAndRemoveAllConvertWorkers()
{
  while (m_cWorkers.size())
  {
    CConvertWorkersToIterator it = m_cWorkers.begin();
    if (it != m_cWorkers.end())
    {

      m_pAddOnManager->ReleaseConvertWorkerTo(*it);
      //delete (*it);
      //m_cWorkers.erase(m_cWorkers.begin());
    }
  }
  return true;
}

#define MMToTwips(nMM) ((int)((((float)nMM)*1440*10)/254))





COneStyle::COneStyle()
{
}
COneStyle::~COneStyle()
{
}
bool COneStyle::IsDefaultStyle()
{
  if (m_sStyleName == DEFAULT_STYLE_NAME)
    return true;
  return false;
}
void COneStyle::SetDefaultValues(bool bWithDefaultName)
{
  if (bWithDefaultName)
    m_sStyleName = DEFAULT_STYLE_NAME;
  m_nStyleID = -1;
  m_bDirtyFlag = false;
  m_bStyleIsPermanent = true;
  m_sFontName = _XT("Arial");
  m_nFontSize = 12;
  m_nFontColor = 0;
  m_nFontBackColor = 0xFFFFFF;
  m_nCodePage = 1252;
  m_bBold = false;
  m_bItalic = false;
  m_bUnderline = false;
  m_bDoubleUnderline = false;
  m_bStrikeOut = false;
  m_nLeft = -1;
  m_nRight = -1;
  m_nIdent = -1;
	m_nKeep = 0;
	m_nKeepNext = 0;
	m_nWidowOrphon = 0;

}
void COneStyle::CopyFrom(const COneStyle* pStyle)
{
  if (!pStyle)
    return;

  m_sFontName = pStyle->m_sFontName;
  m_nStyleID = pStyle->m_nStyleID;
  m_bDirtyFlag = pStyle->m_bDirtyFlag;
  m_bStyleIsPermanent = pStyle->m_bStyleIsPermanent;
  m_nFontSize = pStyle->m_nFontSize;
  m_nFontColor = pStyle->m_nFontColor;
  m_nFontBackColor = pStyle->m_nFontBackColor;
  m_nCodePage = pStyle->m_nCodePage;
  m_bBold = pStyle->m_bBold;
  m_bItalic = pStyle->m_bItalic;
  m_bUnderline = pStyle->m_bUnderline;
  m_bDoubleUnderline = pStyle->m_bDoubleUnderline;
  m_bStrikeOut = pStyle->m_bStrikeOut;
  m_nLeft = pStyle->m_nLeft;
  m_nRight = pStyle->m_nRight;
  m_nIdent = pStyle->m_nIdent;
	m_nKeep = pStyle->m_nKeep;
	m_nKeepNext = pStyle->m_nKeepNext;
	m_nWidowOrphon = pStyle->m_nWidowOrphon;

}

CStyleManager::CStyleManager()
{
  m_pActualStyle = 0;
  m_pActualStyleToSet = 0;
  COneStyle* pNew = new COneStyle();
  if (pNew)
  {
    pNew->SetDefaultValues(true);
    CStylesPair pair;
    pair.first = pNew->GetStyleName();
    pair.second = pNew;
    m_cStyles.insert(pair);
    m_pActualStyle = pNew;
  }
}
CStyleManager::~CStyleManager()
{
}
void CStyleManager::SetDefaultStyleAsActualStyle()
{
  m_pActualStyle = GetDefaultStyle();
}
void CStyleManager::SetActualStyle(const xtstring& sStyleName)
{
  m_pActualStyle = GetStyle(sStyleName);
  if (!m_pActualStyle)
    SetDefaultStyleAsActualStyle();
}
COneStyle* CStyleManager::GetActualStyle()
{
  if (!m_pActualStyle)
    SetDefaultStyleAsActualStyle();
  return m_pActualStyle;
}
void CStyleManager::SetDefaultStyleAsActualStyleToSet()
{
  m_pActualStyleToSet = GetDefaultStyle();
}
void CStyleManager::SetActualStyleToSet(const xtstring& sStyleName)
{
  m_pActualStyleToSet = GetStyle(sStyleName);
  if (!m_pActualStyleToSet)
    SetDefaultStyleAsActualStyleToSet();
}
void CStyleManager::SetNoActualStyleToSet()
{
  m_pActualStyleToSet = 0;
}
COneStyle* CStyleManager::GetActualStyleToSet()
{
  if (!m_pActualStyleToSet)
    SetDefaultStyleAsActualStyleToSet();
  return m_pActualStyleToSet;
}
COneStyle* CStyleManager::CreateNewStyle(const xtstring& sStyleName)
{
  COneStyle* pNewStyle = GetStyle(sStyleName);
  if (pNewStyle)
    return pNewStyle;
  pNewStyle = new COneStyle;
  pNewStyle->m_sStyleName = sStyleName;
  pNewStyle->CopyFrom(GetDefaultStyle());
  m_cStyles[sStyleName] = pNewStyle;
  return pNewStyle;
}
COneStyle* CStyleManager::GetDefaultStyle()
{
  return GetStyle(DEFAULT_STYLE_NAME);
}
COneStyle* CStyleManager::GetStyle(const xtstring& sStyleName)
{
  COneStyle* pStyle = 0;
  CStylesIterator it = m_cStyles.find(sStyleName);
  if (it == m_cStyles.end())
  {
    // vielleich Klein/Gross schreibung
    for (it = m_cStyles.begin(); it != m_cStyles.end(); it++)
    {
      if (it->second->m_sStyleName.CompareNoCase(sStyleName) == 0)
        break;
    }
  }
  if (it != m_cStyles.end())
    pStyle = it->second;
  return pStyle;
}
void CStyleManager::RemoveStyle(const xtstring& sStyleName)
{
  CStylesIterator it = m_cStyles.find(sStyleName);
  if (it == m_cStyles.end())
  {
    // vielleich Klein/Gross schreibung
    for (it = m_cStyles.begin(); it != m_cStyles.end(); it++)
    {
      if (it->second->m_sStyleName.CompareNoCase(sStyleName) == 0)
        break;
    }
  }
  if (it == m_cStyles.end())
    return;
  if (it->second->IsDefaultStyle())
    return;
  delete it->second;
  m_cStyles.erase(it);
}
void CStyleManager::RemoveAllStyles()
{
  for (CStylesIterator it = m_cStyles.begin(); it != m_cStyles.end(); it++)
    delete (it->second);
  m_cStyles.erase(m_cStyles.begin(), m_cStyles.end());
}



CConvertWorker::CConvertWorker()
{
  m_pChannelCallBack = 0;

// Defaults bei den Dokumenten
#if defined(WIN32)
  m_sDefaults.DefLang = (int)GetUserDefaultLangID();
#else
  m_sDefaults.DefLang = 0;
#endif // WIN32
  strcpy(m_sDefaults.DefFontName, "Arial");
  strcpy(m_sDefaults.DefCodePage, "1252");
	
//  HKEY_CURRENT_USER\Control Panel\International
//  sDecimal
  strcpy(m_sDefaults.DefDecimal, ",");
	
  m_sDefaults.DefFontSize = 10;
	
  m_sDefaults.DefCellMargin = MMToTwips(.4); // Cellmargin
  m_sDefaults.DefTabWidth = MMToTwips(12.5); // Default Tabposition

#if defined(WIN32)
  m_sDefaults.PaperOrient = DMORIENT_PORTRAIT; // protrait orientation Werte: DMORIENT_PORTRAIT oder DMORIENT_LANDSCAPE
#endif // WIN32
  m_sDefaults.PageWidth = MMToTwips(210); // Default page width
  m_sDefaults.PageHeight = MMToTwips(297); // Default page height

  m_sDefaults.TopMargin = m_sDefaults.LeftMargin = m_sDefaults.RightMargin = MMToTwips(20.0);
  m_sDefaults.BotMargin = MMToTwips(20.0);

  m_sDefaults.HdrMargin = MMToTwips(12.5);
  m_sDefaults.FtrMargin = MMToTwips(12.5);
}

CConvertWorker::~CConvertWorker()
{
}

void CConvertWorker::SetChannelToExecutionModule(CChannelToExecutedEntity* pChannelCallBack,
                                                 CChanelToExecutionModule* pExecModuleCallBack)
{
  m_pChannelCallBack = pChannelCallBack;
  m_pExecModuleCallBack = pExecModuleCallBack;
}

xtstring CConvertWorker::GetKeyStrFromIni(const xtstring& sSection,
                                          const xtstring& sKey,
                                          const xtstring& sFileName)
{
#if defined(WIN32)
  xtstring csReturnBuffer;
  XTCHAR lpszReturnBuffer[_MAX_PATH];

  xtstring sPath;
  GetModulePath(sPath);
  sPath += sFileName;

  if (::GetPrivateProfileString(sSection.c_str(), sKey.c_str(), _XT(""), lpszReturnBuffer, _MAX_PATH, sPath.c_str()))
  {
    csReturnBuffer = lpszReturnBuffer;
    return csReturnBuffer;
  }
#endif // WIN32

  return _XT("");
}

void CConvertWorker::GetModulePath(xtstring& sPath)
{
#if defined(WIN32)
  xtstring sFileName;
  char s[_MAX_PATH];
 // If this parameter is NULL, GetModuleFileName retrieves the path of the executable file of the current process.  
 //HINSTANCE hInst = AfxGetInstanceHandle();
  ::GetModuleFileName(NULL, s, _MAX_PATH);
  sFileName = s;
  size_t nPos = sFileName.find_last_of(_XT("\\/"));
  if (nPos != xtstring::npos)
    sPath.assign(sFileName.begin(), sFileName.begin() + nPos + 1);
#endif // WIN32
}

bool CConvertWorker::GetTemporaryFileName(const xtstring& sInPicture,
                                          xtstring& sOutPicture, xtstring& sExt)
{
  bool bRet = false;
#if defined(WIN32)
	char *TempName = NULL;
	xtstring cModulePath;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( sInPicture.c_str(), drive, dir, fname, ext );

	GetModulePath(cModulePath);
	if( ( TempName = _tempnam(cModulePath.c_str(), "$xpubpic" ) ) != NULL )
	{
		sOutPicture = TempName;
		if(sExt.size() > 0)
			sOutPicture += sExt;
		else
			sOutPicture += ext;
		
		free(TempName);
		
		return true;
	}
#endif // WIN32
  return bRet;
}

bool CConvertWorker::IsPathFile(const xtstring& sFilePathName)
{
  bool bRet = false;
#if defined(WIN32)
  bRet = PathFileExists((LPCSTR)sFilePathName.c_str()) != FALSE;
#else
  FILE *fp = fopen(sFilePathName.c_str(), "rb");
  if(fp)
  { 
    bRet = true;
    fclose(fp);
  }
#endif // WIN32
  return bRet;
}














CConvertWorkerFrom::CConvertWorkerFrom(TDataFormatForStoneAndTemplate nDataFormat)
{
  m_nDataFormat = nDataFormat;
  m_pConvertTo = 0;
  m_pConvertProgress = 0;
  m_pKeywords = 0;
}

CConvertWorkerFrom::~CConvertWorkerFrom()
{
}

bool CConvertWorkerFrom::AttachConvertTo(CConvertWorkerTo* pConvertTo)
{
  assert(pConvertTo);
  assert(!m_pConvertTo);
  if (!pConvertTo)
    return false;

  if (m_pConvertTo)
    return false;

  m_pConvertTo = pConvertTo;
  bool bRet = m_pConvertTo->AttachConvertFrom(this);
  if (bRet)
  {
    m_pConvertTo->SetDefaultStyle(m_cStyleManager.GetDefaultStyle());
    return true;
  }
  m_pConvertTo = 0;
  return false;
}

CConvertWorkerTo* CConvertWorkerFrom::DetachConvertTo()
{
  CConvertWorkerTo* pRet = m_pConvertTo;
  m_pConvertTo = 0;
  
  pRet->DetachConvertFrom();
  
  return pRet;
}

bool CConvertWorkerFrom::KeywordIsFreeware(const xtstring& sKeyword)
{
  if (!m_pKeywords)
    return true;
  for (int nI = 0; ;nI++)
  {
    if (!m_pKeywords[nI].lpszKeyword)
      break;
    if (sKeyword == m_pKeywords[nI].lpszKeyword)
    {
      if (m_pKeywords[nI].bFreeWare)
        return true;
      return false;
    }
  }
  return true;
}














CConvertWorkerTo::CConvertWorkerTo(TDataFormatForStoneAndTemplate nDataFormat)
{
  m_nDataFormat = nDataFormat;
  m_pConvertFrom = 0;
  // Style members
  m_pDefaultStyle = 0;
  m_pActualStyle = 0;
}

CConvertWorkerTo::~CConvertWorkerTo()
{
}

bool CConvertWorkerTo::AttachConvertFrom(CConvertWorkerFrom* pConvertFrom)
{
// keine Ahnung wie das richtig sein muss!

  assert(pConvertFrom);
  assert(!m_pConvertFrom);
  if (!pConvertFrom)
    return false;
  if (m_pConvertFrom)
    return false;

  m_pConvertFrom = pConvertFrom;
  return true;
}

CConvertWorkerFrom* CConvertWorkerTo::DetachConvertFrom()
{
  CConvertWorkerFrom* pRet = m_pConvertFrom;
  m_pConvertFrom = 0;
  return pRet;
}

