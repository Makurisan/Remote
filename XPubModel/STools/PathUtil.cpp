// PathUtil.cpp: implementation of the CPathUtil class.
//
//////////////////////////////////////////////////////////////////////

#if defined(LINUX_OS)

#include <io.h>

#elif defined(WIN32)

#include <io.h>

#elif defined(MAC_OS_MWE)

#include <io.h>

#else

#error "not implemented"

#endif

#include <stdlib.h>
#include <stdlib.h>
#include "SSTLdef/STLdef.h"
#include "PathUtil.h"



xtstring CPathUtil::GetPathWithSlashFromFullFileName(const xtstring& sFile)
{
  xtstring sRet;
  sRet = GetPathWithoutSlashFromFullFileName(sFile);
  if (sRet.size())
    sRet += _XT("/");
  return sRet;
}

xtstring CPathUtil::GetPathWithoutSlashFromFullFileName(const xtstring& sFile)
{
  xtstring sRet;
  sRet = sFile;
  size_t nPos = sRet.find_last_of(_XT("\\/"));
  if (nPos != xtstring::npos)
    sRet.erase(nPos, sRet.size());
  return sRet;
}

xtstring CPathUtil::MakeFullPath(const xtstring& sFirstPart,
                                 const xtstring& sSecondPart)
{
  xtstring sPath;

#if defined(LINUX_OS)

  xtstring sFirst(sFirstPart);
  xtstring sSecond(sSecondPart);
  if (sFirst.size() && sFirst[sFirst.size() - 1] == _XT('/'))
    sFirst.erase(sFirst.begin() + sFirst.size() - 1);
  if (sSecond.size() && sSecond[0] == _XT('/'))
    sSecond.erase(sSecond.begin());
  sPath = sFirst;
  if (sFirst.size() && sSecond.size())
    sPath += _XT('/');
  sPath += sSecond;

#elif defined(WIN32)

  xtstring sFirst(sFirstPart);
  xtstring sSecond(sSecondPart);
  if (sFirst.size() && (sFirst[sFirst.size() - 1] == _XT('/') || sFirst[sFirst.size() - 1] == _XT('\\')))
    sFirst.erase(sFirst.begin() + sFirst.size() - 1);
  if (sSecond.size() && (sSecond[0] == _XT('/') || sSecond[0] == _XT('\\')))
    sSecond.erase(sSecond.begin());
  sPath = sFirst;
  if (sFirst.size() && sSecond.size())
    sPath += _XT('\\');
  sPath += sSecond;

#elif defined(MAC_OS_MWE)

#error "not implemented"

#else

#error "not implemented"

#endif

  return sPath;
}

xtstring CPathUtil::MakeFullPathSpecial(const xtstring& sPartialPath, const xtstring& sFileMayBeWithPath)
{
  xtstring sPath;

#if defined(LINUX_OS)

#error "not implemented"

#elif defined(WIN32)

  xtstring sFirst(sPartialPath);
  xtstring sSecond(sFileMayBeWithPath);
  if (sFirst.size() && (sFirst[sFirst.size() - 1] == _XT('/') || sFirst[sFirst.size() - 1] == _XT('\\')))
    sFirst.erase(sFirst.begin() + sFirst.size() - 1);

  char sDrive[_MAX_PATH], sDir[_MAX_PATH], sFile[_MAX_PATH], sExt[_MAX_PATH];
  _splitpath(sSecond.c_str(), sDrive, sDir, sFile, sExt);
  bool bNetwork = false;
  if (sSecond.size() >= 2 && sSecond[0] == _XT('/') && sSecond[1] == _XT('/'))
      bNetwork = true;
  if (_tcslen(sDrive) == 0 && !bNetwork)
  {
    if (sSecond.size() && (sSecond[0] == _XT('/') || sSecond[0] == _XT('\\')))
      sSecond.erase(sSecond.begin());
    sPath = sFirst;
    if (sFirst.size() && sSecond.size())
      sPath += _XT('\\');
    sPath += sSecond;
  }
  else
  {
    sPath = sSecond;
  }

#elif defined(MAC_OS_MWE)

#error "not implemented"

#else

#error "not implemented"

#endif

  return sPath;
}




bool CPathUtil::PathExist(const xtstring& sPath)
{
  bool bRet = false;
#if defined(LINUX_OS)

#elif defined(WIN32)

  xtstring sTemp(sPath);

  if (sTemp.size() && (sTemp[sTemp.size() - 1] == _XT('/') || sTemp[sTemp.size() - 1] == _XT('\\')))
    sTemp.erase(sTemp.begin() + sTemp.size() - 1);

  // wir muessen testen, ob verzeichnis existiert
  struct _finddata_t fdata;
  intptr_t nRetHandle = _findfirst(sTemp.c_str(), &fdata);
  if (nRetHandle != -1L)
    bRet = true;

#elif defined(MAC_OS_MWE)

#error "not implemented"

#else

#error "not implemented"

#endif
  return bRet;
}

bool CPathUtil::FileExist(const xtstring& sFile)
{
  bool bRet = false;
#if defined(LINUX_OS)

#elif defined(WIN32)

  // wir muessen testen, ob verzeichnis existiert
  struct _finddata_t fdata;
  intptr_t nRetHandle = _findfirst(sFile.c_str(), &fdata);
  if (nRetHandle != -1L)
    bRet = true;

#elif defined(MAC_OS_MWE)

#error "not implemented"

#else

#error "not implemented"

#endif
  return bRet;
}

