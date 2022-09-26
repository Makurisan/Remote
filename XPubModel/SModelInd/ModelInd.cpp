// ModelInd.cpp: implementation of the CModelBaseInd class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "ModelInd.h"
#include "ERModelInd.h"
#include "XPubVariant.h"














//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CModelBaseInd
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CModelBaseInd::CModelBaseInd(TTypeOfModel nType)
{
  m_nTypeOfModel = nType;
  m_bPropChanged = false;
}

CModelBaseInd::~CModelBaseInd()
{
}

static LPCXTCHAR DEFAULT_REPLACE_TOKENS[] = 
{
  _XT("ä"),      _XT("ae"),
  _XT("Ä"),      _XT("Ae"),
  _XT("ö"),      _XT("oe"),
  _XT("Ö"),      _XT("Oe"),
  _XT("ü"),      _XT("ue"),
  _XT("Ü"),      _XT("Ue"),
  _XT("ß"),      _XT("ss"),
  0,             0,
};
static XTCHAR* DEFAULT_RESTRICT_CHARS       = _XT(" /\\\a\n\r\t\"°'’`´~!?@#$µ€%^&*-+=()|{}[]<>:;,.");
static XTCHAR* DEFAULT_RESTRICT_TABLE_CHARS = _XT(" /\\\a\n\r\t\"°'’`´~!?@#$µ€%^&*-+=()|{}[]<>:;,");

bool CModelBaseInd::CheckDefaultName(const xtstring& sOldName,
                                     xtstring& sNewName,
                                     const xtstring& sRestrictedChars,
                                     LPCXTCHAR* pReplaceTokens)
{
  xtstring sName;
  sName = sOldName;
  if (pReplaceTokens)
  {
    size_t nI = 0;
    while (pReplaceTokens[nI] && pReplaceTokens[nI + 1])
    {
      xtstring sOld, sNew;
      sOld = pReplaceTokens[nI];
      sNew = pReplaceTokens[nI + 1];
      size_t nPos = sName.find(sOld);
      while (nPos != xtstring::npos)
      {
        sName.replace(sName.begin() + nPos,
                      sName.begin() + nPos + sOld.size(),
                      sNew);
        nPos = sName.find(sOld);
      }
      nI++;
      nI++;
    }
  }
  sNewName.clear();
  for (size_t nI = 0; nI < sName.size(); nI++)
  {
    if (sRestrictedChars.find(sName[nI]) == xtstring::npos)
      sNewName.push_back(sName[nI]);
  }
  return (sNewName.size() != 0);
}

bool CModelBaseInd::CheckModuleName(const xtstring& sName,
                                    xtstring& sNewName)
{
  return CheckDefaultName(sName, sNewName, DEFAULT_RESTRICT_CHARS, DEFAULT_REPLACE_TOKENS);
}

bool CModelBaseInd::CheckIFSEntityName(const xtstring& sName,
                                       xtstring& sNewName)
{
  return CheckDefaultName(sName, sNewName, DEFAULT_RESTRICT_CHARS, DEFAULT_REPLACE_TOKENS);
}

bool CModelBaseInd::CheckERTableName(const xtstring& sName,
                                     xtstring& sNewName)
{
  return CheckDefaultName(sName, sNewName, DEFAULT_RESTRICT_TABLE_CHARS, DEFAULT_REPLACE_TOKENS);
}

bool CModelBaseInd::CheckERFieldName(const xtstring& sName,
                                     xtstring& sNewName)
{
  return CheckDefaultName(sName, sNewName, DEFAULT_RESTRICT_CHARS, DEFAULT_REPLACE_TOKENS);
}
bool CModelBaseInd::CheckERParamName(const xtstring& sName,
                                     xtstring& sNewName)
{
  return CheckDefaultName(sName, sNewName, DEFAULT_RESTRICT_CHARS, DEFAULT_REPLACE_TOKENS);
}
bool CModelBaseInd::CheckERRelationName(const xtstring& sName,
                                        xtstring& sNewName)
{
  return CheckDefaultName(sName, sNewName, DEFAULT_RESTRICT_CHARS, DEFAULT_REPLACE_TOKENS);
}
bool CModelBaseInd::CheckVariableName(const xtstring& sName,
                                      xtstring& sNewName)
{
  return CheckDefaultName(sName, sNewName, DEFAULT_RESTRICT_CHARS, DEFAULT_REPLACE_TOKENS);
}

xtstring CModelBaseInd::GetFormatedTime(time_t time, bool bWithSeconds)
{
  XTCHAR buff[100];
  buff[0] = 0;

  struct tm* ptmTemp = localtime(&time);
  if (ptmTemp)
  {
    if (bWithSeconds)
      strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
    else
      strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M"), ptmTemp);
  }
  return buff;
}

bool CModelBaseInd::SetFormatedTime(time_t* pTime,
                                    const xtstring& sTime)
{
  int nD, nM, nY, nHou, nMin, nSec;
  size_t nPos;
  size_t nActPos = 0;

  if ((nPos = sTime.find(_XT("."), nActPos)) == xtstring::npos)
    return false;
  nD = atoi(sTime.c_str() + nActPos);
  nActPos = nPos + 1;
  if ((nPos = sTime.find(_XT("."), nActPos)) == xtstring::npos)
    return false;
  nM = atoi(sTime.c_str() + nActPos);
  nActPos = nPos + 1;
  if ((nPos = sTime.find(_XT(" "), nActPos)) == xtstring::npos)
    return false;
  nY = atoi(sTime.c_str() + nActPos);
  nActPos = nPos + 1;
  if ((nPos = sTime.find(_XT(":"), nActPos)) == xtstring::npos)
    return false;
  nHou = atoi(sTime.c_str() + nActPos);
  nActPos = nPos + 1;
  nPos = sTime.find(_XT(":"), nActPos);
  nMin = atoi(sTime.c_str() + nActPos);
  nActPos = nPos + 1;
  if (nPos == xtstring::npos)
    nSec = 0;
  else
    nSec = atoi(sTime.c_str() + nActPos);


  struct tm tmTime;
  tmTime.tm_sec   = nSec;     /* seconds after the minute - [0,59] */
  tmTime.tm_min   = nMin;     /* minutes after the hour - [0,59] */
  tmTime.tm_hour  = nHou;    /* hours since midnight - [0,23] */
  tmTime.tm_mday  = nD;    /* day of the month - [1,31] */
  tmTime.tm_mon   = nM - 1;     /* months since January - [0,11] */
  tmTime.tm_year  = nY - 1900;    /* years since 1900 */
  tmTime.tm_wday  = -1;    /* days since Sunday - [0,6] */
  tmTime.tm_yday  = -1;    /* days since January 1 - [0,365] */
  tmTime.tm_isdst = -1;   /* daylight savings time flag */
  time_t time = mktime(&tmTime);
  *pTime = time;
  return true;
}




