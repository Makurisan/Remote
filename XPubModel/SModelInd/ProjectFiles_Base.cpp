#include "ProjectFiles_Base.h"

#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "STools/Utils.h"

#include "SXPubZip/XZip.h"




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CProjectFileBase
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CProjectFileBase::CProjectFileBase(TProjectFileType nTypeOfFile)
{
  m_nTypeOfFile = nTypeOfFile;

  m_pDirChecker = NULL;
  m_pOutMsgContainer = NULL;


  m_sXMLFileName.erase();
  m_sModuleName.erase();
  m_sBaseFolder.erase();
}

CProjectFileBase::~CProjectFileBase()
{
}

void CProjectFileBase::SetCallBacks(CDirectoryCheckerCallBack* pDirChecker,
                                    COutMessageContainerCallBack* pOutMsgContainer)
{
  m_pDirChecker = pDirChecker;
  m_pOutMsgContainer = pOutMsgContainer;
}

void CProjectFileBase::operator = (CProjectFileBase* pFile)
{
  if (!pFile)
    return;

  m_sXMLFileName      = pFile->m_sXMLFileName;
  m_sBaseFolder       = pFile->m_sBaseFolder;
  m_nTypeOfFile       = pFile->m_nTypeOfFile;
  m_pDirChecker       = pFile->m_pDirChecker;
  m_pOutMsgContainer  = pFile->m_pOutMsgContainer;
  // da muessen wir SetModuleName benutzen, weil in der Funktion
  // ist die Name in mehreren Objekten gesetzt.
  SetModuleName(pFile->m_sModuleName);
}

void CProjectFileBase::GetCreatedAndChangedTime(const xtstring& sFullFileName,
                                                xtstring& sCreated,
                                                xtstring& sChanged,
                                                bool bWithSeconds)
{
  sCreated.clear();
  sChanged.clear();
  struct stat filestat;
  if (stat(sFullFileName.c_str(), &filestat))
    return;

  time_t timeCreated = filestat.st_ctime;
  time_t timeChanged = filestat.st_mtime;

  sCreated = CModelBaseInd::GetFormatedTime(timeCreated, bWithSeconds);
  sChanged = CModelBaseInd::GetFormatedTime(timeChanged, bWithSeconds);
}

bool CProjectFileBase::ProjectFileIsCompressed(const xtstring& sFileName)
{
  CXPubZip cZip(0/*no progress callback*/);
  cZip.SetZIPFile(sFileName.c_str());
  return cZip.IsFileZIPFile();
}

bool CProjectFileBase::ReadFile_Compressed(const xtstring& sCompressedFileName,
                                           const xtstring& sFileName,
                                           xtstring& sFileContent)
{
  CXPubZip cZip(0/*no progress callback*/);
  cZip.SetZIPFile(sCompressedFileName.c_str());

  if (!cZip.FileExistInZIP(sFileName.c_str()))
    return false;

  long nLen = cZip.GetFileLength(sFileName.c_str());
  if (nLen)
  {
    char* p = new char[nLen + 1];
    cZip.GetFileContent(sFileName.c_str(), p, nLen);
    p[nLen] = _XT('\0');
    sFileContent = p;
    delete p;
  }
  else
    sFileContent.clear();
  return true;
}

bool CProjectFileBase::ReadFile_Uncompressed(const xtstring& sFileName,
                                             xtstring& sFileContent)
{
  bool bResult = false;
	FILE* fp = fopen(sFileName.c_str(), _XT("rb"));
	if (fp)
	{
		// Determine file length
		fseek( fp, 0L, SEEK_END );
		int nFileLen = ftell(fp);
		fseek( fp, 0L, SEEK_SET );

    if (!nFileLen)
    {
      sFileContent.erase();
      bResult = true;
    }
    else
    {
		  // Load string
		  allocator<char> mem;
		  allocator<char>::pointer pBuffer = mem.allocate(nFileLen + 1, NULL);
		  if (fread(pBuffer, nFileLen, 1, fp ) == 1)
		  {
			  pBuffer[nFileLen] = _XT('\0');
        bResult = true;
			  sFileContent = pBuffer;
		  }
      mem.deallocate(pBuffer, 1);
    }
		fclose(fp);
	}
	return bResult;
}

bool CProjectFileBase::WriteFile(const xtstring& sFileName,
                                 const xtstring& sFileContent)
{
  // Save document to file
	bool bResult = false;
	FILE* fp = fopen(sFileName.c_str(), _XT("wb"));
	if (fp)
	{
		// Save string
		size_t nFileLen = sFileContent.size();
		if (!nFileLen)
			bResult = true;
		else if (fwrite(sFileContent.c_str(), nFileLen, 1, fp ) == 1)
			bResult = true;
		fclose(fp);
	}
	return bResult;
}

xtstring CProjectFileBase::GetNameForShow()
{
  return m_sModuleName;
}

void CProjectFileBase::SetBaseFileName(const xtstring& sFileName)
{
  if (!sFileName.length())
    // nichts uebergeben, nichts setzen
    return;

  m_sXMLFileName = sFileName;
  m_sXMLFileName += GetFileExtension();
}

xtstring CProjectFileBase::GetBaseFileName()
{
  size_t nPos = m_sXMLFileName.find(GetFileExtension());
  if (nPos == xtstring::npos)
    return m_sXMLFileName;
  xtstring sFileName(m_sXMLFileName, 0, nPos);
  return sFileName;
}

xtstring CProjectFileBase::GetMD5Sum(const xtstring& sText)
{
  xtstring sMD5;
  CMD5 md5;
  md5.NextBufferForMD5((unsigned char*)sText.c_str(), (unsigned int)sText.size());
  sMD5 = md5.GetMD5Sum();
  return sMD5;
}

void CProjectFileBase::GetFileTimeChanged(const xtstring& sFullFileName,
                                          time_t& timeChanged)
{
  CUtils::GetFileChangedTime(sFullFileName, timeChanged);
}

