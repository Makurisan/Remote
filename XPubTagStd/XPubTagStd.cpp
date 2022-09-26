// XPubTagStd.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "XPubTagStdDef.h"
#include "XPubTagStd.h"

#include "..\XPubTagUtility\checker.h"
#include "SControl/IntelliSenseTag.h"

using namespace std;

CXPubTagStdMgr     g_TagStdMgr;

extern "C" BOOL WINAPI TerInit(HINSTANCE hInstance,DWORD reason);
extern "C" int WINAPI TerExit(int nParameter);
#include "XPubMagicInit.h"

#define NOLICENSE

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

#define MODULE_NAME "XPubTagBuilder.Dll"

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		XPubTagBuilderInitialize();
    // XPubMagicInit(GetModuleHandle(MODULE_NAME), MODULE_NAME);
		// TerInit(GetModuleHandleA(MODULE_NAME), DLL_PROCESS_ATTACH);
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
    //XPubMagicRelease();
    //TerExit(0);
		XPubTagBuilderTerminate();
	}
	return TRUE;
}



bool g_bInitCalled = false;


// Wir müssen die Lokale setzen, da sonst Probleme bei Dezimalzeichen auftauchen
static int SetLocale()
{
	setlocale(LC_NUMERIC, "English");
	return 0;
}

extern "C" int XPubTagBuilderInitialize()
{
  int retval = XPubTagBuilder_OK;
  try
  {
    char* pModuleName = "XPubTagBuilder.dll";
  
		//int nRetVal = g_TagStdMgr.CreateTagStd();
    //if (nRetVal != XPubTagBuilder_OK)
    //  return nRetVal;
    
		HMODULE hDll=::GetModuleHandle(pModuleName);
    XPubMagicInit(hDll, pModuleName);
    TerInit(hDll, DLL_PROCESS_ATTACH);
    g_bInitCalled = true;
  
	}
  catch(...)
  {
    retval = 1;
  }
  return retval;
}

extern "C" int XPubTagBuilderTerminate()
{
  int retval = 0;
  try
  {
    g_bInitCalled = false;
    g_TagStdMgr.RemoveAll();
    XPubMagicRelease();
    TerExit(0);
  }
  catch(...)
  {
    retval = 1;
  }
return retval;
}

extern "C" int XPubTagBuilderClose(int iID)
{
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.Close(iID);
}

extern "C" int XPubTagBuilderInit()
{
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.CreateTagStd();
}

extern "C" int XPubTagBuilderLicense(int iID, char* pLicense)
{
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.License(iID, pLicense);
}

extern "C" int XPubTagBuilderToPDF(int iID, char *pInput, char **ppOutput, long &lSize)
{
	SetLocale();
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.ConvertTagToPDF(iID, pInput, ppOutput, lSize);
}

extern "C" int XPubTagBuilderToPDFFile(int iID, char *pInput, char *pFile)
{
	SetLocale();
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.ConvertTagToPDF(iID, pInput, pFile);
}

extern "C" int XPubTagBuilderSetPassword(int iID, char *pOwner, char *pUser, long lPerm)
{
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.SetPassword(iID, pOwner, pUser, lPerm);
}

extern "C" int XPubTagBuilderToRTF(int iID, char *pInput, char **ppOutput, long &lSize)
{
	SetLocale();
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.ConvertTagToRTF(iID, pInput, ppOutput, lSize);
}

extern "C" int XPubTagBuilderToRTFFile(int iID, char *pInput, char *pFile)
{
	SetLocale();
  if (!g_bInitCalled)
    return XPubTagBuilder_InitNotCalled;
  return g_TagStdMgr.ConvertTagToRTF(iID, pInput, pFile);
}















CXPubTagStdMgr::CXPubTagStdMgr()
               :m_cAddOnManager(NULL)
{
  m_cAddOnManager.InitManager();
  m_iTagStd = 0;
}

CXPubTagStdMgr::~CXPubTagStdMgr()
{
  RemoveAll();
}
void CXPubTagStdMgr::RemoveAll()
{
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  for (iter = m_TagStdMap.begin(); iter != m_TagStdMap.end(); iter++)
  {
    pTagStd = (CXPubTagStd *)iter->second;
    delete pTagStd;
  }
  m_TagStdMap.clear();
}


int CXPubTagStdMgr::CreateTagStd()
{
  CXPubTagStd *pTagStd;
  pTagStd = new CXPubTagStd(&m_cAddOnManager);
  if (!pTagStd)
    return XPubTagBuilder_Alloc_Err;
  m_TagStdMap[m_iTagStd] = pTagStd;
  m_iTagStd++;
  return m_iTagStd - 1;
}

int CXPubTagStdMgr::Close(int iID)
{
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  iter = m_TagStdMap.find(iID);
  if (iter != m_TagStdMap.end())
  {
    pTagStd = (CXPubTagStd *)iter->second;
    m_TagStdMap.erase(iter);
    delete pTagStd;
    return XPubTagBuilder_OK;
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagStdMgr::GetTagBuiderCount()
{
  return (int)m_TagStdMap.size();
}

int CXPubTagStdMgr::License(int iID, char* pLicense)
{
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  iter = m_TagStdMap.find(iID);
  if (iter != m_TagStdMap.end())
  {
    pTagStd = (CXPubTagStd *)iter->second;
    pTagStd->License(pLicense);
    return XPubTagBuilder_OK;
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagStdMgr::ConvertTagToPDF(int iID, char *pInput, char **ppOutput, long &lSize)
{
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  iter = m_TagStdMap.find(iID);
  if (iter != m_TagStdMap.end())
  {
    pTagStd = (CXPubTagStd *)iter->second;
    return pTagStd->ToPDF(pInput, ppOutput, lSize);
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagStdMgr::ConvertTagToPDF(int iID, char *pInput, char *pFile)
{
  if (!pFile)
    return XPubTagBuilder_Null_Err;
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  char *pOutput = NULL;
  long lSize = 0;
  iter = m_TagStdMap.find(iID);
  if (iter != m_TagStdMap.end())
  {
    pTagStd = (CXPubTagStd *)iter->second;
    int iRet = pTagStd->ToPDF(pInput, &pOutput, lSize);

    if (iRet != XPubTagBuilder_OK)
      return iRet;
    FILE *pStream = fopen(pFile, "wb");
    if (!pStream)
      return XPubTagBuilder_File_Open_Err;
    fwrite(pOutput, (size_t)lSize, 1, pStream);
    fclose(pStream);
    delete []pOutput;
    pOutput = NULL;
    return XPubTagBuilder_OK;
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagStdMgr::SetPassword(int iID, char *pOwner, char *pUser, long lPerm)
{
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  iter = m_TagStdMap.find(iID);
  if (iter != m_TagStdMap.end())
  {
    pTagStd = (CXPubTagStd *)iter->second;
    pTagStd->SetPassword(pOwner, pUser, lPerm);
    return XPubTagBuilder_OK;
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagStdMgr::ConvertTagToRTF(int iID, char *pInput, char **ppOutput, long &lSize)
{
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  iter = m_TagStdMap.find(iID);
  if (iter != m_TagStdMap.end())
  {
    pTagStd = (CXPubTagStd *)iter->second;
    return pTagStd->ToRTF(pInput, ppOutput, lSize);
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagStdMgr::ConvertTagToRTF(int iID, char *pInput, char *pFile)
{
  if (!pFile)
    return XPubTagBuilder_Null_Err;
  XPubTagStdMap::iterator iter;
  CXPubTagStd *pTagStd;
  char *pOutput = NULL;
  long lSize = 0;
  iter = m_TagStdMap.find(iID);
  if (iter != m_TagStdMap.end())
  {
    pTagStd = (CXPubTagStd *)iter->second;
    int iRet = pTagStd->ToRTF(pInput, &pOutput, lSize);
    if (iRet != XPubTagBuilder_OK)
      return iRet;
    FILE *pStream = fopen(pFile, "wb");
    if (!pStream)
      return XPubTagBuilder_File_Open_Err;
    fwrite(pOutput, (size_t)lSize, 1, pStream);
    fclose(pStream);
    delete []pOutput;
    pOutput = NULL;
    return XPubTagBuilder_OK;
  }
  return XPubTagBuilder_Not_Found;
}







CXPubTagStd::CXPubTagStd(CXPubAddOnManager *pAddOnMgr)
            :m_cBuilder(/*CConvertWorkerProgress**/NULL)
{
  m_cBuilder.Attach(pAddOnMgr);
  m_lPerm = 0;
  m_pOwner = NULL;
  m_pUser = NULL;
  m_bLicense = false;
  m_bDemoLicense = false;
  m_sLicense.clear();
}

CXPubTagStd::~CXPubTagStd()
{
  if (m_pOwner)
    free(m_pOwner);
  if (m_pUser)
    free(m_pUser);
}

void CXPubTagStd::CheckOpenFile()
{
  CChecker checker;
  checker.CheckKey(string(m_sLicense),
                   m_bDemoLicense,
                   string(LICENSE_PDFV10_DEMO),
                   m_bLicense,
                   string(LICENSE_PDFV10));
}

void CXPubTagStd::License(char* pLicense)
{
  if (pLicense)
    m_sLicense = pLicense;
}

int CXPubTagStd::ToPDF(char *pInput, char **ppOutput, long &lSize)
{
  CheckOpenFile();

  // Falls License da ist, dann werden wir die Standard/Freeware
  // Kewords/Parameters nicht kontrollieren.
  if (m_bLicense)
    m_cBuilder.SetKeywords(0);
  else
    m_cBuilder.SetKeywords(g_aKeywords_TAG);

  m_cBuilder.SetRestrictFeatures(false);
  bool bOnlyFreewareKeywordsUsed = true;

  // Wenn License definiert war, aber keine License da,
  // dann Fehler.
#ifndef NOLICENSE
	if (m_sLicense.size() && !m_bLicense && !m_bDemoLicense)
    return XPubTagBuilder_LicenseNotValid;
#else
// vorrübergehende Einstellungen für W&W AG
	xtstring sInput(pInput);
	sInput.ReplaceAll("<style", "<!style");
	if( sInput.FindNoCase("SYKA031P") != xtstring::npos)
		sInput.ReplaceAll("<formfield", "<formfield fontname=\"Arial\"");
	m_bLicense =  true;
#endif

  if (!m_cBuilder.Open(CTagBuilder::PDF))
    return XPubTagBuilder_Open_Err;

  m_cBuilder.Set(sInput);

  if (m_pUser || m_pOwner)
    m_cBuilder.SetPassword(xtstring(m_pOwner), xtstring(m_pUser), m_lPerm);

  char *pDataPDF = NULL;
  int lSizePDF = 0;
  if (!m_cBuilder.Get(pDataPDF, lSizePDF))
  {
    m_cBuilder.Close();
    return XPubTagBuilder_Get_Err;
  }
  bOnlyFreewareKeywordsUsed = m_cBuilder.GetOnlyFreewareKeywordsUsed();
  m_cBuilder.Close();
  if (!m_bLicense && !m_bDemoLicense && !bOnlyFreewareKeywordsUsed)
  {
    if (lSizePDF && pDataPDF)
      delete pDataPDF;
    lSizePDF = 0;
    pDataPDF = 0;
    m_cBuilder.SetRestrictFeatures(true);
    if (!m_cBuilder.Open(CTagBuilder::PDF))
      return XPubTagBuilder_Open_Err;
    m_cBuilder.Set(pInput);
    if (!m_cBuilder.Get(pDataPDF, lSizePDF))
    {
      m_cBuilder.Close();
      return XPubTagBuilder_Get_Err;
    }
    m_cBuilder.Close();
  }

  *ppOutput = pDataPDF;
  lSize = lSizePDF;

 return XPubTagBuilder_OK;
}

int CXPubTagStd::ToRTF(char *pInput, char **ppOutput, long &lSize)
{
  CheckOpenFile();

  // Falls License da ist, dann werden wir die Standard/Freeware
  // Kewords/Parameters nicht kontrollieren.
  if (m_bLicense)
    m_cBuilder.SetKeywords(0);
  else
    m_cBuilder.SetKeywords(g_aKeywords_TAG);

  m_cBuilder.SetRestrictFeatures(false);
  bool bOnlyFreewareKeywordsUsed = true;

  // Wenn License definiert war, aber keine License da,
  // dann Fehler.
#ifndef NOLICENSE
  if (m_sLicense.size() && !m_bLicense && !m_bDemoLicense)
    return XPubTagBuilder_LicenseNotValid;
#else
	xtstring sInput(pInput);
	sInput.ReplaceAll("<style", "<!style");
	m_bLicense =  true;
#endif

  if (!m_cBuilder.Open(CTagBuilder::RTF))
    return XPubTagBuilder_Open_Err;

  m_cBuilder.Set(sInput);

  xtstring sDataRTF;
  if (!m_cBuilder.Get(sDataRTF))
  {
    m_cBuilder.Close();
    return XPubTagBuilder_Get_Err;
  }
  bOnlyFreewareKeywordsUsed = m_cBuilder.GetOnlyFreewareKeywordsUsed();
  m_cBuilder.Close();
  if (!m_bLicense && !m_bDemoLicense && !bOnlyFreewareKeywordsUsed)
  {
    m_cBuilder.SetRestrictFeatures(true);
    if (!m_cBuilder.Open(CTagBuilder::RTF))
      return XPubTagBuilder_Open_Err;
    m_cBuilder.Set(pInput);
    if (!m_cBuilder.Get(sDataRTF))
    {
      m_cBuilder.Close();
      return XPubTagBuilder_Get_Err;
    }
    m_cBuilder.Close();
  }

  if (sDataRTF.length() > 0)
  {
    *ppOutput = new char[sDataRTF.length() + 1];
    if (!(*ppOutput))
      return XPubTagBuilder_Alloc_Err;
    memcpy(*ppOutput, sDataRTF.c_str(), sDataRTF.length());
    lSize = (long)sDataRTF.length();
    (*ppOutput)[lSize] = 0;
  }
  return XPubTagBuilder_OK;
}

void CXPubTagStd::SetPassword(char *pOwner, char *pUser, long lPerm)
{
  if (m_pOwner)
    free(m_pOwner);
  if (m_pUser)
    free(m_pUser);
  if (pOwner && strlen(pOwner))
    m_pOwner = strdup(pOwner);
  if (pUser && strlen(pUser))
    m_pUser = strdup(pUser);
  m_lPerm = lPerm;
}
