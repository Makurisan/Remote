// Utils.cpp: implementation of the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if defined(LINUX_OS)
 
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <utime.h>

#elif defined(WIN32)

#include <windows.h>
#include <direct.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/utime.h>

#elif defined(MAC_OS_MWE)

#define __pure2
#define __dead2

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <utime.h>

#include <stdio.h>

#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <cerrno>

#else

#error "not implemented"

#endif

#include <time.h>
#include <ctype.h>
#include <stdio.h>

#include "SSTLdef/STLdef.h"
#include "SMD5/MD5.h"
#include "Utils.h"




xtstring CUtils::GetTimeStamp(bool bMiliToo)
{
  srand((unsigned int)time(0));
  // nActualTime in UTC
  time_t nActualTime = time(NULL);
  //
  struct tm* pLocalTime = localtime(&nActualTime);
  //
  char sFormatedTime[100];
  strftime(sFormatedTime, sizeof(sFormatedTime), "%Y-%m-%d-%H-%M-%S", pLocalTime);
  //
  xtstring sActualTime = sFormatedTime;
  if (bMiliToo)
  {
#if defined(LINUX_OS)

    struct timespec ts;
    clock_gettime(0, &ts);
    sprintf(sFormatedTime, "%s-%06ld", sActualTime.c_str(), ts.tv_nsec);
    sActualTime = sFormatedTime;

#elif defined(WIN32)

    SYSTEMTIME st;
    GetLocalTime(&st);
    srand(rand());
    sprintf(sFormatedTime, "%s-%02ld%04ld", sActualTime.c_str(), st.wMilliseconds%100, rand()%10000);
    sActualTime = sFormatedTime;

#elif defined(MAC_OS_MWE)

    clock_t start = clock();
    srand(rand());
    sprintf(sFormatedTime, "%s-%02ld%04ld", sActualTime.c_str(), (long)start%100, rand()%10000);
    sActualTime = sFormatedTime;

#else

#error "not implemented"

#endif
  }
  return sActualTime;
}

xtstring CUtils::GetTimeStamp_OnlyNumbers(bool bMiliToo)
{
  srand((unsigned int)time(0));
  // nActualTime in UTC
  time_t nActualTime = time(NULL);
  //
  struct tm* pLocalTime = localtime(&nActualTime);
  //
  char sFormatedTime[100];
  strftime(sFormatedTime, sizeof(sFormatedTime), "%Y%m%d%H%M%S", pLocalTime);
  //
  xtstring sActualTime = sFormatedTime;
  if (bMiliToo)
  {
#if defined(LINUX_OS)
    struct timespec ts;
    clock_gettime(0, &ts);
    sprintf(sFormatedTime, "%s%06ld", sActualTime.c_str(), ts.tv_nsec);
    sActualTime = sFormatedTime;
#elif defined(WIN32)
    SYSTEMTIME st;
    GetLocalTime(&st);
    srand(rand());
    sprintf(sFormatedTime, "%s%02d%04d", sActualTime.c_str(), st.wMilliseconds%100, rand()%10000);
    sActualTime = sFormatedTime;
#elif defined(MAC_OS_MWE)
    clock_t start = clock();
    srand(rand());
    sprintf(sFormatedTime, "%s%02d%04d", sActualTime.c_str(), (long)start%100, rand()%10000);
    sActualTime = sFormatedTime;
#else
#error "not implemented"
#endif
  }
  return sActualTime;
}

void CUtils::SleepInMILISEC(int nSleep)
{
#if defined(LINUX_OS)
  usleep(nSleep * 1000);
#elif defined(WIN32)
  Sleep(nSleep);
#elif defined(MAC_OS_MWE)
  usleep(nSleep * 1000);
#else
#error "not implemented"
#endif
}

void CUtils::SleepInSEC(int nSleep)
{
#if defined(LINUX_OS)
  sleep(nSleep);
#elif defined(WIN32)
  Sleep(nSleep * 1000);
#elif defined(MAC_OS_MWE)
  sleep(nSleep);
#else
#error "not implemented"
#endif
}

void CUtils::ResetRandom()
{
#if defined(LINUX_OS)
  struct timespec ts;
  clock_gettime(0, &ts);
  srand(ts.tv_nsec % 1000);
#elif defined(WIN32)
  SYSTEMTIME st;
  GetLocalTime(&st);
  srand(st.wMilliseconds % 1000);
#elif defined(MAC_OS_MWE)
  srand(1 % 1000);
#else
#error "not implemented"
#endif
}


bool CUtils::CreatePath(const xtstring& sPath1,
                        bool bLastElementToo)
{
  if (!sPath1.size())
    return false;

#if defined(LINUX_OS) || defined(MAC_OS_MWE)
  xtstring sTemp;
  xtstring sResult;

  xtstring sPath;
  {
    size_t nLast = sPath1.find_last_of(_XT("/\\"));
    if (nLast == sPath1.size() - 1)
      sPath.assign(sPath1.begin(), sPath1.begin() + nLast);
    else
      sPath = sPath1;
  }
  sTemp = sPath;

  size_t nPos = xtstring::npos;
  nPos = sTemp.find_first_of(_XT("/\\"));
  if (nPos == xtstring::npos || nPos + 1 == sTemp.size())
    // es ist entweder direkt die Festplatte, oder Schrot
    return true;
  nPos = sTemp.find_first_of(_XT("/\\"), nPos + 1);

  while (nPos != xtstring::npos)
  {
    sResult.append(sTemp.begin(), sTemp.begin() + nPos + 1);
    sTemp.erase(sTemp.begin(), sTemp.begin() + nPos + 1);

    if (mkdir(sResult.c_str(), 0777) != 0)
    {
      if (errno != EEXIST)
        return false;
    }
    nPos = sTemp.find_first_of(_XT("/\\"));
  }

  if (bLastElementToo)
  {
    if (mkdir(sPath.c_str(), 0777) != 0)
    {
      if (errno != EEXIST)
        return false;
    }
  }
#elif defined(WIN32)
  xtstring sTemp;
  xtstring sResult;

  xtstring sPath;
  {
    size_t nLast = sPath1.find_last_of(_XT("/\\"));
    if (nLast == sPath1.size() - 1)
      sPath.assign(sPath1.begin(), sPath1.begin() + nLast);
    else
      sPath = sPath1;
  }
  sTemp = sPath;

  size_t nPos = xtstring::npos;
  nPos = sTemp.find_first_of(_XT("/\\"));
  if (nPos == xtstring::npos || nPos + 1 == sTemp.size())
    // es ist entweder direkt die Festplatte, oder Schrot
    return true;
  nPos = sTemp.find_first_of(_XT("/\\"), nPos + 1);

//  DWORD dw;
  while (nPos != xtstring::npos)
  {
    sResult.append(sTemp.begin(), sTemp.begin() + nPos + 1);
    sTemp.erase(sTemp.begin(), sTemp.begin() + nPos + 1);

    if (_mkdir(sResult.c_str()) != 0)
    {
      if (errno != EEXIST)
        return false;
    }
/*
    dw = CreateDirectory(sResult.c_str(), 0);
    if (dw == 0)
    {
      dw = GetLastError();
      if (dw != 183)
        // Cannot create a file when that file already exists.
        return false;
    }
*/
    nPos = sTemp.find_first_of(_XT("/\\"));
  }

  if (bLastElementToo)
  {
    if (_mkdir(sPath.c_str()) != 0)
    {
      if (errno != EEXIST)
        return false;
    }
/*
    dw = CreateDirectory(sPath.c_str(), 0);
    if (dw == 0)
    {
      dw = GetLastError();
      if (dw != 183)
        // Cannot create a file when that file already exists.
        return false;
    }
*/
  }
#else
#error "not implemented"
#endif

  return true;
}

bool CUtils::CreateFileWithContent(const xtstring& sFullPathFileName,
                                   unsigned long nDataSize,
                                   const unsigned char *data)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)

  if (!CreatePath(sFullPathFileName, false))
    return false;

  FILE* fp = fopen(sFullPathFileName.c_str(), _XT("wb"));
  if (!fp)
    return false;
  if (fwrite(data, nDataSize, 1, fp ) != 1)
  {
    fclose(fp);
    RemoveFile(sFullPathFileName);
    return false;
  }
  fclose(fp);

#elif defined(WIN32)

  if (!CreatePath(sFullPathFileName, false))
    return false;

  FILE* fp = fopen(sFullPathFileName.c_str(), _XT("wb"));
  if (!fp)
    return false;
  if (fwrite(data, nDataSize, 1, fp ) != 1)
  {
    fclose(fp);
    RemoveFile(sFullPathFileName);
    return false;
  }
  fclose(fp);


#else
#error "not implemented"
#endif

  return true;
}

bool CUtils::SetFileChangedTime(const xtstring& sFullPathFileName,
                                time_t timeChanged)
{
#if defined(LINUX_OS)

#if defined(UNICODE) || defined(_UNICODE)
  struct utimbuf times;
  times.actime = times.modtime = timeChanged;
  if (wutime(sFullPathFileName.c_str(), &times))
    return false;
#else // defined(UNICODE) || defined(_UNICODE)
  struct utimbuf times;
  times.actime = times.modtime = timeChanged;
  if (utime(sFullPathFileName.c_str(), &times))
    return false;
#endif // defined(UNICODE) || defined(_UNICODE)

#elif defined(WIN32)

#if defined(UNICODE) || defined(_UNICODE)
  struct _utimbuf times;
  times.actime = times.modtime = timeChanged;
  if (_wutime(sFullPathFileName.c_str(), &times))
    return false;
#else // defined(UNICODE) || defined(_UNICODE)
  struct _utimbuf times;
  times.actime = times.modtime = timeChanged;
  if (_utime(sFullPathFileName.c_str(), &times))
    return false;
#endif // defined(UNICODE) || defined(_UNICODE)

#elif defined(MAC_OS_MWE)

#if defined(UNICODE) || defined(_UNICODE)
  struct utimbuf times;
  times.actime = times.modtime = timeChanged;
  if (wutime(sFullPathFileName.c_str(), &times))
    return false;
#else // defined(UNICODE) || defined(_UNICODE)
  struct utimbuf times;
  times.actime = times.modtime = timeChanged;
  if (utime(sFullPathFileName.c_str(), &times))
    return false;
#endif // defined(UNICODE) || defined(_UNICODE)

#else
#error "not implemented"
#endif

  return true;
}

bool CUtils::ReadFileContent(const xtstring& sFullPathFileName,
                             unsigned long& nDataSize,
                             unsigned char* data)
{
  bool bRet = false;

  unsigned long nFileSize = 0;

  FILE* fp = fopen(sFullPathFileName.c_str(), _XT("rb"));
	if (fp)
	{
		// Determine file length
		fseek( fp, 0L, SEEK_END );
		nFileSize = ftell(fp);
		fseek( fp, 0L, SEEK_SET );
    if (!nFileSize)
    {
      nDataSize = 0;
      bRet = true;
    }
    else
    {
      if (data)
      {
        // lesen
        if (nFileSize > nDataSize)
        {
          nDataSize = nFileSize;
        }
        else
        {
          nDataSize = nFileSize;
          if (fread(data, nDataSize, 1, fp ) == 1)
          {
            bRet = true;
          }
        }
      }
      else
      {
        // bestimmen
        nDataSize = nFileSize;
        bRet = true;
      }
    }
    fclose(fp);
  }
  return bRet;
}

bool CUtils::GetFileChangedTime(const xtstring& sFullPathFileName,
                                time_t& timeChanged)
{
#if defined(LINUX_OS)

#if defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  if (wstat(sFullPathFileName.c_str(), &st))
    return false;
  timeChanged = st.st_mtime;
#else // defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  if (stat(sFullPathFileName.c_str(), &st))
    return false;
  timeChanged = st.st_mtime;
#endif // defined(UNICODE) || defined(_UNICODE)

#elif defined(WIN32)

#if defined(UNICODE) || defined(_UNICODE)
  struct _stat st;
  if (_wstat(sFullPathFileName.c_str(), &st))
    return false;
  timeChanged = st.st_mtime;
#else // defined(UNICODE) || defined(_UNICODE)
  struct _stat st;
  if (_stat(sFullPathFileName.c_str(), &st))
    return false;
  timeChanged = st.st_mtime;
#endif // defined(UNICODE) || defined(_UNICODE)

#elif defined(MAC_OS_MWE)

#if defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  if (wstat(sFullPathFileName.c_str(), &st))
    return false;
  timeChanged = st.st_mtime;
#else // defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  if (stat(sFullPathFileName.c_str(), &st))
    return false;
  timeChanged = st.st_mtime;
#endif // defined(UNICODE) || defined(_UNICODE)

#else
#error "not implemented"
#endif

  return true;
}

bool CUtils::GetFileLength(const xtstring& sFullPathFileName,
                           long& nFileLength)
{
#if defined(LINUX_OS)

#if defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  wstat(sFullPathFileName.c_str(), &st);
  nFileLength = st.st_size;
#else // defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  stat(sFullPathFileName.c_str(), &st);
  nFileLength = st.st_size;
#endif // defined(UNICODE) || defined(_UNICODE)

#elif defined(WIN32)

#if defined(UNICODE) || defined(_UNICODE)
  struct _stat st;
  _wstat(sFullPathFileName.c_str(), &st);
  nFileLength = st.st_size;
#else // defined(UNICODE) || defined(_UNICODE)
  struct _stat st;
  _stat(sFullPathFileName.c_str(), &st);
  nFileLength = st.st_size;
#endif // defined(UNICODE) || defined(_UNICODE)

#elif defined(MAC_OS_MWE)

#if defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  wstat(sFullPathFileName.c_str(), &st);
  nFileLength = st.st_size;
#else // defined(UNICODE) || defined(_UNICODE)
  struct stat st;
  stat(sFullPathFileName.c_str(), &st);
  nFileLength = st.st_size;
#endif // defined(UNICODE) || defined(_UNICODE)

#else
#error "not implemented"
#endif

  return true;
}

bool CUtils::GetFileMD5(const xtstring& sFullPathFileName,
                        xtstring& sMD5)
{
  CMD5 md5;
  md5.Reset();
  if (!md5.NextFileForMD5(sFullPathFileName))
    return false;
  sMD5 = md5.GetMD5Sum();
  return true;
}

bool CUtils::RemoveFile(const xtstring& sFileName)
{
#if defined(LINUX_OS)
  if (unlink(sFileName.c_str()))
    return false;
#elif defined(WIN32)
  if (_unlink(sFileName.c_str()))
    return false;
#elif  defined(MAC_OS_MWE)
  if (remove(sFileName.c_str()))
    return false;
#else
#error "not implemented"
#endif

  return true;
}

xtstring CUtils::GetLoggedUser()
{
  xtstring sUser;

#if defined(LINUX_OS)
  char* pUser = ::getenv("USER");
  if (pUser)
    sUser = pUser;
#elif defined(WIN32)
  char* pUser = ::getenv("USERNAME");
  if (pUser)
    sUser = pUser;
#elif defined(MAC_OS_MWE)
  char* pUser = ::getenv("USER");
  if (pUser)
    sUser = pUser;
#else
#error "not implemented"
#endif
  return sUser;
}
