#if defined(WIN32)
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

//#define MMToTwips(nMM) ((int)((((float)nMM)*1440*10)/254))
long MMToTwips( float nMM )
{
	long nRet = 0;
	if ( nMM <= ( LONG_MAX / 567 ) && nMM >= ( LONG_MIN / 567 ) )
		nRet = nMM * 567 / 10;
	return nRet;
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
	
  m_sDefaults.DefFontSize = 12;
	
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

bool CConvertWorker::SplitPath(const xtstring& sFileName,
                               xtstring& sDrive, xtstring& sDir, xtstring& sName, xtstring& sExt)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( sFileName.c_str(), drive, dir, fname, ext );

	sDrive = drive;
	sDir = dir;
	sName = fname;
	sExt = ext;

	return true;
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

bool CConvertWorker::SetPathCombine(xtstring& sDestFilePathName, const xtstring& sDirectory, const xtstring& sFileName)
{
  bool bRet = false;
#if defined(WIN32)
	sDestFilePathName.empty();
  char sMaxPath[_MAX_PATH];
	bRet = ::PathCombine(sMaxPath, (LPCSTR)sDirectory.c_str(), (LPCSTR)sFileName.c_str()) != FALSE;
	if(bRet) 
		sDestFilePathName = sMaxPath;
#else
	sDestFilePathName = sDirectory + sFileName;
#endif
  return bRet;
}

bool CConvertWorker::IsPathFile(const xtstring& sFilePathName)
{
  bool bRet = false;
#if defined(WIN32)
  bRet = ::PathFileExists((LPCSTR)sFilePathName.c_str()) != FALSE;
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
  m_bRestrictFeatures = false;
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
    m_pConvertTo->SetBridgeToStyleManager(&m_cStyleManager);
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
  pRet->SetBridgeToStyleManager(0);
  
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

bool CConvertWorkerFrom::ParameterIsFreeware(const xtstring& sKeyword,
                                             const xtstring& sParameter)
{
  if (!m_pKeywords)
    return true;
  TagParameters* pParams = 0;
  for (int nI = 0; ;nI++)
  {
    if (!m_pKeywords[nI].lpszKeyword)
      break;
    if (sKeyword == m_pKeywords[nI].lpszKeyword)
    {
      pParams = m_pKeywords[nI].par;
      break;
    }
  }
  if (!pParams)
    return true;
  for (int nI = 0; ;nI++)
  {
    if (sParameter == pParams[nI].lpszParameter)
    {
      if (pParams[nI].bFreeWare)
        return true;
      return false;
    }
  }
  return true;
}














CConvertWorkerTo::CConvertWorkerTo(TDataFormatForStoneAndTemplate nDataFormat, HWND hWndParen)
{
  m_nDataFormat = nDataFormat;
  m_pConvertFrom = 0;
  // Style members
  m_pBridgeToStyleManager = 0;
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

