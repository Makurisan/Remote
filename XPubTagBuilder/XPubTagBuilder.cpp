// XPubTagBuilder.cpp : Defines the entry point for the DLL application.
//

#if defined(WIN32)
#include "stdafx.h"
#endif

#include "XPubTagBuilderErr.h"
#include "XPubTagBuilder.h"

using namespace std;

CXPubTagBuilderMgr     g_TagBuilderMgr;

#if defined(WIN32)


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  dwReason, 
                       LPVOID lpReserved
                      )
{
  // Remove this if you use lpReserved
  UNREFERENCED_PARAMETER(lpReserved);

  switch (dwReason)
  {
    case DLL_PROCESS_ATTACH:
    {
      break;
    }
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
    {
      break;
    }
  }
  return TRUE;
}

#endif

#if defined(MAC_OS_MWE) || defined(LINUX)
extern "C"
{
#endif

#if defined(MAC_OS_MWE)
#pragma export on
#endif

int XPubTagBuilderInit(void)
{
  return g_TagBuilderMgr.CreateTagBuilder();
}


int XPubTagBuilderToPDF(int iID, char *pInput, char **ppOutput, long &lSize)
{
  return g_TagBuilderMgr.ConvertTagToPDF(iID, pInput, ppOutput, lSize);
}

int XPubTagBuilderToPDFFile(int iID, char *pInput, char *pFile)
{
  return g_TagBuilderMgr.ConvertTagToPDF(iID, pInput, pFile);
}

int XPubTagBuilderSetPassword(int iID, char *pOwner, char *pUser, long lPerm)
{
  return g_TagBuilderMgr.SetPassword(iID, pOwner, pUser, lPerm);
}

int XPubTagBuilderToRTF(int iID, char *pInput, char **ppOutput, long &lSize)
{
  return g_TagBuilderMgr.ConvertTagToRTF(iID, pInput, ppOutput, lSize);
}

int XPubTagBuilderToRTFFile(int iID, char *pInput, char *pFile)
{
  return g_TagBuilderMgr.ConvertTagToRTF(iID, pInput, pFile);
}


int XPubTagBuilderClose(int iID)
{
  return g_TagBuilderMgr.Close(iID);
}

#if defined(MAC_OS_MWE)
#pragma export off
#endif

#if defined(MAC_OS_MWE) || defined(LINUX)
}
#endif

CXPubTagBuilderMgr::CXPubTagBuilderMgr()
                    :m_cAddOnManager(NULL)
{
  m_cAddOnManager.InitManager();
  m_iTagBuilder = 0;
}

CXPubTagBuilderMgr::~CXPubTagBuilderMgr()
{
  XPubTagBuilderMap::iterator iter;
  CXPubTagBuilder *pTagBuilder;
  for (iter = m_TagBuilderMap.begin(); iter != m_TagBuilderMap.end(); iter++)
  {
    pTagBuilder = (CXPubTagBuilder *)iter->second;
    delete pTagBuilder;
  }
  m_TagBuilderMap.clear();
}

int CXPubTagBuilderMgr::CreateTagBuilder()
{

  CXPubTagBuilder *pTagBuilder;
  pTagBuilder = new CXPubTagBuilder(&m_cAddOnManager);
  if (!pTagBuilder)
    return XPubTagBuilder_Alloc_Err;
  m_TagBuilderMap[m_iTagBuilder] = pTagBuilder;
  m_iTagBuilder++;
  return m_iTagBuilder -1;

}

int CXPubTagBuilderMgr::Close(int iID)
{
  XPubTagBuilderMap::iterator iter;
  CXPubTagBuilder *pTagBuilder;
  iter = m_TagBuilderMap.find(iID);
  if (iter != m_TagBuilderMap.end())
  {
    pTagBuilder = (CXPubTagBuilder *)iter->second;
    m_TagBuilderMap.erase(iter);
    delete pTagBuilder;
    return XPubTagBuilder_OK;
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagBuilderMgr::GetTagBuiderCount()
{
  return (int)m_TagBuilderMap.size();
}

int CXPubTagBuilderMgr::ConvertTagToPDF(int iID, char *pInput, char **ppOutput, long &lSize)
{
  XPubTagBuilderMap::iterator iter;
  CXPubTagBuilder *pTagBuilder;
  iter = m_TagBuilderMap.find(iID);
  if (iter != m_TagBuilderMap.end())
  {
    pTagBuilder = (CXPubTagBuilder *)iter->second;
    return pTagBuilder->ToPDF(pInput, ppOutput, lSize);
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagBuilderMgr::ConvertTagToPDF(int iID, char *pInput, char *pFile)
{
  if (!pFile)
    return XPubTagBuilder_Null_Err;
  XPubTagBuilderMap::iterator iter;
  CXPubTagBuilder *pTagBuilder;
  char *pOutput = NULL;
  long lSize = 0;
  iter = m_TagBuilderMap.find(iID);
  if (iter != m_TagBuilderMap.end())
  {
    pTagBuilder = (CXPubTagBuilder *)iter->second;
    int iRet = pTagBuilder->ToPDF(pInput, &pOutput, lSize);

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

int CXPubTagBuilderMgr::SetPassword(int iID, char *pOwner, char *pUser, long lPerm)
{
  XPubTagBuilderMap::iterator iter;
  CXPubTagBuilder *pTagBuilder;
  iter = m_TagBuilderMap.find(iID);
  if (iter != m_TagBuilderMap.end())
  {
    pTagBuilder = (CXPubTagBuilder *)iter->second;
    pTagBuilder->SetPassword(pOwner, pUser, lPerm);
    return XPubTagBuilder_OK;
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagBuilderMgr::ConvertTagToRTF(int iID, char *pInput, char **ppOutput, long &lSize)
{
  XPubTagBuilderMap::iterator iter;
  CXPubTagBuilder *pTagBuilder;
  iter = m_TagBuilderMap.find(iID);
  if (iter != m_TagBuilderMap.end())
  {
    pTagBuilder = (CXPubTagBuilder *)iter->second;
    return pTagBuilder->ToRTF(pInput, ppOutput, lSize);
  }
  return XPubTagBuilder_Not_Found;
}

int CXPubTagBuilderMgr::ConvertTagToRTF(int iID, char *pInput, char *pFile)
{
  if (!pFile)
    return XPubTagBuilder_Null_Err;
  XPubTagBuilderMap::iterator iter;
  CXPubTagBuilder *pTagBuilder;
  char *pOutput = NULL;
  long lSize = 0;
  iter = m_TagBuilderMap.find(iID);
  if (iter != m_TagBuilderMap.end())
  {
    pTagBuilder = (CXPubTagBuilder *)iter->second;
    int iRet = pTagBuilder->ToRTF(pInput, &pOutput, lSize);
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







CXPubTagBuilder::CXPubTagBuilder(CXPubAddOnManager *pAddOnMgr)
                :m_cBuilder(/*CConvertWorkerProgress**/NULL)
{
  m_cBuilder.Attach(pAddOnMgr);
  m_lPerm = 0;
  m_pOwner = NULL;
  m_pUser = NULL;
}

CXPubTagBuilder::~CXPubTagBuilder()
{
  if (m_pOwner)
    free(m_pOwner);
  if (m_pUser)
    free(m_pUser);
}

int CXPubTagBuilder::ToPDF(char *pInput, char **ppOutput, long &lSize)
{
  if (!m_cBuilder.Open(CTagBuilder::PDF))
    return XPubTagBuilder_Open_Err;

  xtstring sData(pInput);
  m_cBuilder.Set(sData);

  if (m_pUser || m_pOwner)
    m_cBuilder.SetPassword(xtstring(m_pOwner), xtstring(m_pUser), m_lPerm);

  char *pDataPDF = NULL;
  int lSizePDF = 0;


  if (!m_cBuilder.Get(pDataPDF, lSizePDF))
  {
    m_cBuilder.Close();
    return XPubTagBuilder_Get_Err;
  }


  m_cBuilder.Close();
  *ppOutput = pDataPDF;
  lSize = lSizePDF;

  return XPubTagBuilder_OK;
}

int CXPubTagBuilder::ToRTF(char *pInput, char **ppOutput, long &lSize)
{
  if(!m_cBuilder.Open(CTagBuilder::RTF))
    return XPubTagBuilder_Open_Err;

  m_cBuilder.Set(pInput);

  xtstring sDataRTF;
  if(!m_cBuilder.Get(sDataRTF))
  {
    m_cBuilder.Close();
    return XPubTagBuilder_Get_Err;
  }
  m_cBuilder.Close();

  if (sDataRTF.length() > 0)
  {
    *ppOutput = new char[sDataRTF.length() + 1];
    if (!(*ppOutput))
      return XPubTagBuilder_Alloc_Err;
    memcpy(*ppOutput, sDataRTF.c_str(), sDataRTF.length());
    *ppOutput[sDataRTF.length()] = 0;
    lSize = (long)sDataRTF.length();
  }
  return XPubTagBuilder_OK;
}

void CXPubTagBuilder::SetPassword(char *pOwner, char *pUser, long lPerm)
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

