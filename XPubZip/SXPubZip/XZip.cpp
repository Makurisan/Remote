#include "stdafx.h"
#include <assert.h>
#include "XZip.h"
#include "SZip/ZipArchive.h"

#include <string>
#include <vector>

#include <io.h>

#define COMPRESSION_LEVEL     -1

using namespace std;

typedef vector<CZipString>          CZipStringVector;
typedef CZipStringVector::iterator  CZipStringVectorIterator;









CXPubZip::CXPubZip(CXPubZipProgressCallBack* pCallBack)
{
  m_pCallBack = pCallBack;
  m_pZIPFile = 0;
  m_pPassword = 0;
}

CXPubZip::~CXPubZip()
{
  if (m_pZIPFile)
    delete m_pZIPFile;
  m_pZIPFile = 0;
  if (m_pPassword)
    delete m_pPassword;
  m_pPassword = 0;
}

void CXPubZip::SetZIPFile(LPCTSTR pZIPFile)
{
  if (!m_pZIPFile)
    m_pZIPFile = new CZipString;
  *m_pZIPFile = pZIPFile;
}

void CXPubZip::SetPassword(LPCTSTR pPassword)
{
  if (!m_pPassword)
    m_pPassword = new CZipString;
  *m_pPassword = pPassword;
}

bool CXPubZip::IsFileZIPFile()
{
  bool bRet = false;

  if (!m_pZIPFile)
    return bRet;

  CZipArchive cZip;
  try
  {
    cZip.Open(LPCTSTR(m_pZIPFile), CZipArchive::zipOpenReadOnly);
    bRet = true;
  }
  catch (CZipException& /*e*/)
  {
  }
  return bRet;
}

bool CXPubZip::FileExistInZIP(LPCTSTR pFileName)
{
  if (!m_pZIPFile)
    return false;

  CZipString sFileNameToFind(pFileName);
  sFileNameToFind.Replace(_T('\\'), _T('/'));

  CZipArchive cZip;
  try
  {
    cZip.Open(LPCTSTR(m_pZIPFile), CZipArchive::zipOpenReadOnly);
  }
  catch (CZipException& /*e*/)
  {
    return false;
  }
  bool bRet = false;
  for (int i = 0; i < cZip.GetCount(); i++)
  {
    CZipFileHeader fhInfo;
    if (!cZip.GetFileInfo(fhInfo, i))
      continue;
    CZipString sFile(fhInfo.GetFileName());
    sFile.Replace(_T('\\'), _T('/'));
    if (sFileNameToFind.CompareNoCase(sFile) == 0)
    {
      bRet = true;
      break;
    }
  }
  cZip.Close();
  return bRet;
}
long CXPubZip::GetFileLength(LPCTSTR pFileName)
{
  if (!m_pZIPFile)
    return 0;

  CZipString sFileNameToFind(pFileName);
  sFileNameToFind.Replace(_T('\\'), _T('/'));

  CZipArchive cZip;
  try
  {
    cZip.Open(LPCTSTR(m_pZIPFile), CZipArchive::zipOpenReadOnly);
  }
  catch (CZipException& /*e*/)
  {
    return 0;
  }
  long nRet = 0;
  for (int i = 0; i < cZip.GetCount(); i++)
  {
    CZipFileHeader fhInfo;
    if (!cZip.GetFileInfo(fhInfo, i))
      continue;
    CZipString sFile(fhInfo.GetFileName());
    sFile.Replace(_T('\\'), _T('/'));
    if (sFileNameToFind.CompareNoCase(sFile) == 0)
    {
      nRet = fhInfo.m_uUncomprSize;//GetSize();
      break;
    }
  }
  cZip.Close();
  return nRet;
}
bool CXPubZip::GetFileContent(LPCTSTR pFileName, char* pData, long nLen)
{
  if (!m_pZIPFile)
    return false;

  CZipString sFileNameToFind(pFileName);
  sFileNameToFind.Replace(_T('\\'), _T('/'));

  CZipArchive cZip;
  try
  {
    cZip.Open(LPCTSTR(m_pZIPFile), CZipArchive::zipOpenReadOnly);
  }
  catch (CZipException& /*e*/)
  {
    return false;
  }
  bool bRet = false;
  for (int i = 0; i < cZip.GetCount(); i++)
  {
    CZipFileHeader fhInfo;
    if (!cZip.GetFileInfo(fhInfo, i))
      continue;
    CZipString sFile(fhInfo.GetFileName());
    sFile.Replace(_T('\\'), _T('/'));
    if (sFileNameToFind.CompareNoCase(sFile) == 0)
    {
      CZipMemFile mf;
      if (cZip.ExtractFile(i, mf))
      {
        mf.Seek(0, CZipAbstractFile::begin);
        mf.Read(pData, nLen);
        bRet = true;
      }
      break;
    }
  }
  cZip.Close();
  return bRet;
}

void CXPubZip::CreateZIP_FolderContent(LPCTSTR pFolder,
                                       bool bRecursive,
                                       XPubZip_PathBehaviour nPathBehaviour)
{
  if (!m_pZIPFile)
    return;
  if (!pFolder)
    return;

  CZipString sWildCard("*");
  CZipString sFolder;
  sFolder = pFolder;

  CreateZIP(sFolder, sWildCard, bRecursive, nPathBehaviour);
}

void CXPubZip::CreateZIP_WildCard(LPCTSTR pFolder,
                                  bool bRecursive,
                                  XPubZip_PathBehaviour nPathBehaviour)
{
  if (!m_pZIPFile)
    return;
  if (!pFolder)
    return;

  CZipString sStartFolder(pFolder);
  size_t nRight1 = stdbs(sStartFolder).rfind("/");
  size_t nRight2 = stdbs(sStartFolder).rfind("\\");
  size_t nRight;

  if (nRight1 != stdbs::npos && nRight2 != stdbs::npos)
  {
    if (nRight1 > nRight2)
      nRight = nRight1;
    else
      nRight = nRight2;
  }
  else if (nRight1 != stdbs::npos)
    nRight = nRight1;
  else
    nRight = nRight2;

  CZipString sFolder;
  CZipString sWildCard;
  stdbs(sFolder).assign(stdbs(sStartFolder).begin(), stdbs(sStartFolder).begin() + nRight);
  stdbs(sWildCard).assign(stdbs(sStartFolder).begin() + nRight + 1, stdbs(sStartFolder).end());

  CreateZIP(sFolder, sWildCard, bRecursive, nPathBehaviour);
}

void CXPubZip::CreateZIP(const CZipString& sFolder,
                         const CZipString& sWildcardToAdd,
                         bool bRecursive,
                         XPubZip_PathBehaviour nPathBehaviour)
{
  m_bRecursive = bRecursive;
  m_nPathBehaviour = nPathBehaviour;

  CZipArchive cZip;
  cZip.Open(LPCTSTR(m_pZIPFile), CZipArchive::zipCreate);
  if (m_pPassword && stdbs(*m_pPassword).size())
    cZip.SetPassword(stdbs(*m_pPassword).c_str());
  if (m_nPathBehaviour == tXPZ_RelativePath)
    cZip.SetRootPath(sFolder);

  int nCountOfFiles = AddFolderToZip_EstimateCountOfFiles(sFolder, sWildcardToAdd);
  int nActualCount = 0;

  if (m_pCallBack)
    m_pCallBack->XPubZip_AddFolderContent_ProgressStart(nCountOfFiles);
  AddFolderToZip(sFolder, sWildcardToAdd, &cZip, nCountOfFiles, nActualCount);
  if (m_pCallBack)
    m_pCallBack->XPubZip_AddFolderContent_ProgressEnd(nCountOfFiles);
  cZip.Close();
}


void CXPubZip::AddFolderToZip(const CZipString& sFolder,
                              const CZipString& sWildcardToAdd,
                              CZipArchive* pZip,
                              int nTotalCount,
                              int& nActualCount)
{
  if (!pZip)
    return;

#if defined(LINUX_OS)
#error "not implemented"
#elif defined(WIN32)

  CZipString sFolderForFiles;
  CZipString sFolderForFolders;
  sFolderForFiles = sFolder;
  sFolderForFiles += "\\";
  sFolderForFiles += sWildcardToAdd;
  sFolderForFolders = sFolder;
  sFolderForFolders += "\\*";

  {
    struct _finddata_t fdata;
    intptr_t nRetHandle = _findfirst(stdbs(sFolderForFiles).c_str(), &fdata);
    while (nRetHandle != -1L)
    {
      for (;;)
      {
        CZipString sName = fdata.name;
        if (sName == "." || sName == "..")
          break;
        CZipString sFullFile;
        sFullFile = sFolder;
        sFullFile += "\\";
        sFullFile += sName;
        if ((fdata.attrib & _A_SUBDIR) == 0)
        {
          pZip->AddNewFile(sFullFile, COMPRESSION_LEVEL, (m_nPathBehaviour == tXPZ_FullPath));//!= tXPZ_NoPath));
          nActualCount++;
          if (m_pCallBack)
            m_pCallBack->XPubZip_AddFolderContent_Progress(nActualCount, nTotalCount);
        }
        break;
      }
      if (_findnext(nRetHandle, &fdata))
        break;
    }
    _findclose(nRetHandle);
  }

  CZipStringVector folders;
  {
    struct _finddata_t fdata;
    intptr_t nRetHandle = _findfirst(stdbs(sFolderForFolders).c_str(), &fdata);
    while (nRetHandle != -1L)
    {
      for (;;)
      {
        CZipString sName = fdata.name;
        if (sName == "." || sName == "..")
          break;
        CZipString sFullFile;
        sFullFile = sFolder;
        sFullFile += "\\";
        sFullFile += sName;
        if (fdata.attrib & _A_SUBDIR)
        {
          if  (!m_bRecursive)
            break;
          folders.push_back(sFullFile);
        }
        break;
      }
      if (_findnext(nRetHandle, &fdata))
        break;
    }
    _findclose(nRetHandle);
  }

  for (size_t i = 0; i < folders.size(); i++)
  {
    // add the folder before adding its files
    // it is not needed to add the root folder
    pZip->AddNewFile(stdbs(folders[i]).c_str(), COMPRESSION_LEVEL, (m_nPathBehaviour == tXPZ_FullPath));
    AddFolderToZip(folders[i], sWildcardToAdd, pZip, nTotalCount, nActualCount);
  }

#elif defined(MAC_OS_MWE)
#error "not implemented"
#else
#error "not implemented"
#endif
}

int CXPubZip::AddFolderToZip_EstimateCountOfFiles(const CZipString& sFolder,
                                                  const CZipString& sWildcardToAdd)
{
  int nRetVal = 0;

#if defined(LINUX_OS)
#error "not implemented"
#elif defined(WIN32)

  CZipString sFolderForFiles;
  CZipString sFolderForFolders;
  sFolderForFiles = sFolder;
  sFolderForFiles += "\\";
  sFolderForFiles += sWildcardToAdd;
  sFolderForFolders = sFolder;
  sFolderForFolders += "\\*";

  {
    struct _finddata_t fdata;
    intptr_t nRetHandle = _findfirst(stdbs(sFolderForFiles).c_str(), &fdata);
    while (nRetHandle != -1L)
    {
      for (;;)
      {
        CZipString sName = fdata.name;
        if (sName == "." || sName == "..")
          break;
        CZipString sFullFile;
        sFullFile = sFolder;
        sFullFile += "\\";
        sFullFile += sName;
        if ((fdata.attrib & _A_SUBDIR) == 0)
          nRetVal++;
        break;
      }
      if (_findnext(nRetHandle, &fdata))
        break;
    }
    _findclose(nRetHandle);
  }

  CZipStringVector folders;
  {
    struct _finddata_t fdata;
    intptr_t nRetHandle = _findfirst(stdbs(sFolderForFolders).c_str(), &fdata);
    while (nRetHandle != -1L)
    {
      for (;;)
      {
        CZipString sName = fdata.name;
        if (sName == "." || sName == "..")
          break;
        CZipString sFullFile;
        sFullFile = sFolder;
        sFullFile += "\\";
        sFullFile += sName;
        if (fdata.attrib & _A_SUBDIR)
        {
          if  (!m_bRecursive)
            break;
          folders.push_back(sFullFile);
        }
        break;
      }
      if (_findnext(nRetHandle, &fdata))
        break;
    }
    _findclose(nRetHandle);
  }

  for (size_t i = 0; i < folders.size(); i++)
  {
    // add the folder before adding its files
    // it is not needed to add the root folder
    nRetVal += AddFolderToZip_EstimateCountOfFiles(folders[i], sWildcardToAdd);
  }

#elif defined(MAC_OS_MWE)
#error "not implemented"
#else
#error "not implemented"
#endif

  return nRetVal;
}

