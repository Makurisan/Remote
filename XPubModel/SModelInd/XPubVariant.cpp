#include <math.h>
#include "SOpSys/charwchar.h"
#include "XPubVariant.h"
#include "FormatNumber.h"
#include "FormatDateTime.h"

#include "STools/GlobalSettings.h"

// Constant array with months # of days of year
int anXPubMonthDayInYear[13] = {0, 31, 59, 90, 120, 151, 181,212, 243, 273, 304, 334, 365};

#define DEF_GROUPINGSYMBOL    DEFAULT_DIGITGROUPINGSYMBOL
#define DEF_DECIMALSYMBOL     DEFAULT_DECIMALSYMBOL
#define DEF_DATEFORMAT        tLDF_Default
#define DEF_DATESEPARATOR     DEFAULT_DATESEPARATOR
#define DEF_YEARWITHCENTURY   DEFAULT_YEARWITHCENTURY
#define DEF_TIMESEPARATOR     DEFAULT_TIMESEPARATOR

#define DB_DATE_DELIMITER   _XT("/")
#define DB_TIME_DELIMITER   _XT(":")

#define SERDESER_DATE_DELIMITER   _XT(".")
#define SERDESER_TIME_DELIMITER   _XT(":")


#ifdef WIN32
#pragma warning( disable : 4172)
#endif // WIN32




INT64 CXPubDateTime::_MKTIME(const QLHTM *SrcTime)
{
  INT64 nDate;
  int iDays = SrcTime->nDay;
  unsigned int nHour = SrcTime->nHour;
  unsigned int nMinute = SrcTime->nMinute;
  unsigned int nSecond = SrcTime->nSecond;
  unsigned int nMilliSecond = SrcTime->nMilli;
  unsigned int nMicroSecond = SrcTime->nMicro;
  unsigned int nHundredsNano = (SrcTime->nNano +50) / 100;

  // Validate year and month
  if (SrcTime->nYear > 29000 || SrcTime->nYear < -29000 ||
      SrcTime->nMonth < 1 || SrcTime->nMonth > 12)
  {
#if defined(LINUX_OS)  || defined(MAC_OS_MWE)
    INT64 nVal;
    nVal = EMPTY_DATETIME_HIGH;
    nVal <<= 32;
    nVal += EMPTY_DATETIME_LOW;
    return nVal;
#elif defined(WIN32)
    return EMPTY_DATETIME;
#else

#error "not implemented"
#endif
  }

  //  Check for leap year
  bool bIsLeapYear = 
    ((SrcTime->nYear & 3) == 0) && ((SrcTime->nYear % 100) != 0 || (SrcTime->nYear % 400) == 0);

  // Adjust time and frac time
  nMicroSecond += nHundredsNano / 10;
  nHundredsNano %= 10;
  nMilliSecond += nMicroSecond / 1000;
  nMicroSecond %= 1000;
  nSecond +=nMilliSecond / 1000;
  nMilliSecond %= 1000;
  nMinute += nSecond / 60;
  nSecond %= 60;
  nHour += nMinute / 60;
  nMinute %= 60;
  iDays += nHour / 24;
  nHour %= 24;

  //It is a valid date; make Jan 1, 1AD be 1
  nDate = SrcTime->nYear*365L + SrcTime->nYear/4 - SrcTime->nYear/100 + SrcTime->nYear/400 +
    anXPubMonthDayInYear[SrcTime->nMonth-1] + iDays;

  //  If leap year and it's before March, subtract 1:
  if (SrcTime->nMonth <= 2 && bIsLeapYear)
    --nDate;

  //  Offset so that 01/01/1601 is 0
  nDate -= 584754L;

  // Change nDate to seconds
  nDate *= 86400L;
  nDate += (nHour * 3600L) + (nMinute * 60L) + nSecond;

  // Change nDate to hundreds of nanoseconds
  nDate *= 10000000L;
  nDate += (nMilliSecond * 10000L) + (nMicroSecond * 10L) + nHundredsNano;

  return nDate;
}


//-------------------------------------------------------------------
void CXPubDateTime::_LOCALTIME(QLHTM* time,
                               INT64 nSrcTime)
{
  if (!time)
    return;

  long nDaysAbsolute;     // Number of days since 1/1/0
  long nSecsInDay;        // Time in seconds since midnight
  long nMinutesInDay;     // Minutes in day

  long n400Years;         // Number of 400 year increments since 1/1/0
  long n400Century;       // Century within 400 year block (0,1,2 or 3)
  long n4Years;           // Number of 4 year increments since 1/1/0
  long n4Day;             // Day within 4 year block
  //  (0 is 1/1/yr1, 1460 is 12/31/yr4)
  long n4Yr;              // Year within 4 year block (0,1,2 or 3)
  bool bLeap4 = true;     // TRUE if 4 year block includes leap year
  long nHNanosThisDay;
  long nMillisThisDay;
//  QLHTM QLDestTime;

  nHNanosThisDay      = (long)(nSrcTime % 10000000L);
  nSrcTime /= 10000000L;
  nSecsInDay          = (long)(nSrcTime % 86400L);
  nSrcTime /= 86400L;
  nDaysAbsolute       = (long)(nSrcTime);
  nDaysAbsolute      += 584754L;	

  // Calculate the day of week (sun=1, mon=2...)
  //   -1 because 1/1/0 is Sat.  +1 because we want 1-based
  time->nDayOfWeek = (int)((nDaysAbsolute - 1) % 7L) + 1;

  // Leap years every 4 yrs except centuries not multiples of 400.
  n400Years = (long)(nDaysAbsolute / 146097L);

  // Set nDaysAbsolute to day within 400-year block
  nDaysAbsolute %= 146097L;

  // -1 because first century has extra day
  n400Century = (long)((nDaysAbsolute - 1) / 36524L);

  // Non-leap century
  if (n400Century != 0)
  {
    // Set nDaysAbsolute to day within century
    nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

    // +1 because 1st 4 year increment has 1460 days
    n4Years = (long)((nDaysAbsolute + 1) / 1461L);

    if (n4Years != 0)
      n4Day = (long)((nDaysAbsolute + 1) % 1461L);
    else
    {
      bLeap4 = false;
      n4Day = (long)nDaysAbsolute;
    }
  }
  else
  {
    // Leap century - not special case!
    n4Years = (long)(nDaysAbsolute / 1461L);
    n4Day = (long)(nDaysAbsolute % 1461L);
  }

  if (bLeap4)
  {
    // -1 because first year has 366 days
    n4Yr = (n4Day - 1) / 365;

    if (n4Yr != 0)
      n4Day = (n4Day - 1) % 365;
  }
  else
  {
    n4Yr = n4Day / 365;
    n4Day %= 365;
  }

  // n4Day is now 0-based day of year. Save 1-based day of year, year number
  time->nDayOfYear = (int)n4Day + 1;
  time->nYear = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

  // Handle leap year: before, on, and after Feb. 29.
  if (n4Yr == 0 && bLeap4)
  {
    // Leap Year
    if (n4Day == 59)
    {
      /* Feb. 29 */
      time->nMonth  = 2;
      time->nDay = 29;
      goto DoTime;
    }

    // Pretend it's not a leap year for month/day comp.
    if (n4Day >= 60)
      --n4Day;
  }

  // Make n4DaY a 1-based day of non-leap year and compute
  //  month/day for everything but Feb. 29.
  ++n4Day;

  // Month number always >= n/32, so save some loop time */
  for (time->nMonth = (n4Day >> 5) + 1;
    n4Day > anXPubMonthDayInYear[time->nMonth]; time->nMonth++);

    time->nDay = (int)(n4Day - anXPubMonthDayInYear[time->nMonth-1]);

DoTime:
  if (nSecsInDay == 0)
    time->nHour = time->nMinute = time->nSecond = 0;
  else
  {
    time->nSecond = (unsigned int)nSecsInDay % 60L;
    nMinutesInDay = nSecsInDay / 60L;
    time->nMinute = (unsigned int)nMinutesInDay % 60;
    time->nHour   = (unsigned int)nMinutesInDay / 60;
  }

  if (nHNanosThisDay == 0)
    time->nMilli = time->nMicro = time->nNano = 0;
  else
  {
    time->nNano = (unsigned int)((nHNanosThisDay % 10L) * 100L);
    nMillisThisDay = nHNanosThisDay / 10L;
    time->nMicro = (unsigned int)nMillisThisDay % 1000;
    time->nMilli   = (unsigned int)nMillisThisDay / 1000;
  }

//  return &QLDestTime;
}

time_t CXPubDateTime::_MKTIME_1970(const QLHTM *SrcTime)
{
  tm timestruct;

  timestruct.tm_sec   = SrcTime->nSecond;
  timestruct.tm_min   = SrcTime->nMinute;
  timestruct.tm_hour  = SrcTime->nHour;
  timestruct.tm_mday  = SrcTime->nDay;
  timestruct.tm_mon   = SrcTime->nMonth - 1;
  timestruct.tm_year  = SrcTime->nYear - 1900;

  return mktime(&timestruct);
}

void CXPubDateTime::_LOCALTIME_1970(QLHTM* time, time_t nSrcTime)
{
  if (!time)
    return;

  tm* timestruct = localtime(&nSrcTime);
  if (!timestruct)
    return;

  time->Reset();

  time->nNano       = 0;
  time->nMicro      = 0;
  time->nMilli      = 0;
  time->nSecond     = timestruct->tm_sec;     /* seconds after the minute - [0,59] */
  time->nMinute     = timestruct->tm_min;     /* minutes after the hour - [0,59] */
  time->nHour       = timestruct->tm_hour;    /* hours since midnight - [0,23] */
  time->nDay        = timestruct->tm_mday;    /* day of the month - [1,31] */
  time->nMonth      = 1 + timestruct->tm_mon;     /* months since January - [0,11] */
  time->nYear       = 1900 + timestruct->tm_year;    /* years since 1900 */
  // Calculate the day of week (sun=1, mon=2...)
  // +1 because we want 1-based
  time->nDayOfWeek  = 1 + timestruct->tm_wday;    /* days since Sunday - [0,6] */
  // n4Day is now 0-based day of year. Save 1-based day of year, year number
  time->nDayOfYear  = timestruct->tm_yday;    /* days since January 1 - [0,365] */

}

xtstring CXPubDateTime::TimeAsString(CGS_DateTimeNumber* pGSettings) const
{
  xtstring sRet;

  TLongDateFormat nDateFormat;
  xtstring sDateSeparator;
  bool bYearWithCentury;
  xtstring sTimeSeparator;

  if (pGSettings)
  {
    nDateFormat = (TLongDateFormat)pGSettings->GetLongDateFormat();
    sDateSeparator = pGSettings->GetDateSeparator();
    bYearWithCentury = pGSettings->GetYearWithCentury();
    sTimeSeparator = pGSettings->GetTimeSeparator();
  }
  else
  {
    nDateFormat = DEF_DATEFORMAT;
    sDateSeparator = DEF_DATESEPARATOR;
    bYearWithCentury = DEF_YEARWITHCENTURY;
    sTimeSeparator = DEF_TIMESEPARATOR;
  }

  QLHTM ttime;
  _LOCALTIME(&ttime, m_nTime);

  xtstring sDay, sMonth, sYear;
  sYear.Format(_XT("%ld"), ttime.nYear);
  if (!bYearWithCentury)
    sYear.erase(0, 2);

  if (nDateFormat == tLDF_DMY || nDateFormat == tLDF_MDY || nDateFormat == tLDF_YMD)
  {
    sDay.Format(_XT("%ld"), ttime.nDay);
    sMonth.Format(_XT("%ld"), ttime.nMonth);
  }
  else
  {
    sDay.Format(_XT("%02ld"), ttime.nDay);
    sMonth.Format(_XT("%02ld"), ttime.nMonth);
  }

  switch(nDateFormat)
  {
  case tLDF_DMY:// 1.2.2004     / 1.2.04        1.feb.2004
  case tLDF_DDMMY:// 01.02.2004   / 01.02.04      1.feb.2004
    sRet = sDay;      sRet += sDateSeparator;
    sRet += sMonth;   sRet += sDateSeparator;
    sRet += sYear;    sRet += _XT(" ");
    break;
  case tLDF_MDY:// 2.1.2004     / 2.1.04        1.feb.2004
  case tLDF_MMDDY:// 02.01.2004   / 02.01.04      1.feb.2004
    sRet = sMonth;    sRet += sDateSeparator;
    sRet += sDay;     sRet += sDateSeparator;
    sRet += sYear;    sRet += _XT(" ");
    break;
  case tLDF_YMD:// 2004.2.1     / 04.2.1        1.feb.2004
  case tLDF_YMMDD:// 2004.02.01   / 04.02.01      1.feb.2004
    sRet = sYear;     sRet += sDateSeparator;
    sRet += sMonth;   sRet += sDateSeparator;
    sRet += sDay;     sRet += _XT(" ");
    break;
  }

  xtstring sTemp;
  sTemp.Format(_XT("%02ld%s%02ld%s%02ld"),
      ttime.nHour, sTimeSeparator.c_str(), ttime.nMinute, sTimeSeparator.c_str(), ttime.nSecond);
  sRet += sTemp;
  return sRet;
}

xtstring CXPubDateTime::TimeAsSQLString() const
{
  xtstring sRet;

  QLHTM ttime;
  _LOCALTIME(&ttime, m_nTime);
  sRet.Format(_XT("%02ld%s%02ld%s%ld %02ld%s%02ld%s%02ld"),
    ttime.nMonth, DB_DATE_DELIMITER,
    ttime.nDay, DB_DATE_DELIMITER,
    ttime.nYear,
    ttime.nHour, DB_TIME_DELIMITER,
    ttime.nMinute, DB_TIME_DELIMITER,
    ttime.nSecond);
  return sRet;
}

void CXPubDateTime::Serialize(xtstring& sVal)
{
  QLHTM ttime;
  _LOCALTIME(&ttime, m_nTime);
  sVal.Format(_XT("%02ld%s%02ld%s%ld %02ld%s%02ld%s%02ld"),
    ttime.nMonth, SERDESER_DATE_DELIMITER,
    ttime.nDay, SERDESER_DATE_DELIMITER,
    ttime.nYear,
    ttime.nHour, SERDESER_TIME_DELIMITER,
    ttime.nMinute, SERDESER_TIME_DELIMITER,
    ttime.nSecond);
}
void CXPubDateTime::Deserialize(const xtstring& sVal)
{
  xtstring sTemp(sVal);

  xtstring sDatePart;
  xtstring sTimePart;
  // Datum ist vorne, Zeit hinten, dazwischen ist " "
  size_t nPos = sTemp.find(_XT(" "));
  if (nPos == xtstring::npos)
  {
    sDatePart.assign(sTemp);
    sTimePart.assign(sTemp);
  }
  else
  {
    sDatePart.assign(sTemp.begin(), sTemp.begin() + nPos);
    sTimePart.assign(sTemp.begin() + nPos + 1, sTemp.end());
  }
  QLHTM nNewDateTime;
  nNewDateTime.Reset();

  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  // date
  xtstring sD1, sD2, sD3;
  nPos = sDatePart.find(SERDESER_DATE_DELIMITER);
  if (nPos != xtstring::npos)
  {
    sD1.assign(sDatePart.begin(), sDatePart.begin() + nPos);
    sDatePart.erase(sDatePart.begin(), sDatePart.begin() + nPos + 1);
    nPos = sDatePart.find(SERDESER_DATE_DELIMITER);
    if (nPos != xtstring::npos)
    {
      sD2.assign(sDatePart.begin(), sDatePart.begin() + nPos);
      sDatePart.erase(sDatePart.begin(), sDatePart.begin() + nPos + 1);
      sD3 = sDatePart;
    }
  }
  if (sD1.size() != 0 && sD2.size() != 0 && sD3.size() != 0)
  {
    int n1, n2, n3;
    n1 = _ttol(sD1.c_str());
    n2 = _ttol(sD2.c_str());
    n3 = _ttol(sD3.c_str());
    // in DB ist immer M/D/Y
    nNewDateTime.nDay = n2;
    nNewDateTime.nMonth = n1/* - 1*/;
    nNewDateTime.nYear = n3/* - 1900*/;
  }

  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  // time
  xtstring sT1, sT2, sT3;
  nPos = sTimePart.find(SERDESER_TIME_DELIMITER);
  if (nPos != xtstring::npos)
  {
    sT1.assign(sTimePart.begin(), sTimePart.begin() + nPos);
    sTimePart.erase(sTimePart.begin(), sTimePart.begin() + nPos + 1);
    nPos = sTimePart.find(SERDESER_TIME_DELIMITER);
    if (nPos != xtstring::npos)
    {
      sT2.assign(sTimePart.begin(), sTimePart.begin() + nPos);
      sTimePart.erase(sTimePart.begin(), sTimePart.begin() + nPos + 1);
      sT3 = sTimePart;
    }
  }
  if (sT1.size() != 0 && sT2.size() != 0 && sT3.size() != 0)
  {
    nNewDateTime.nHour = _ttol(sT1.c_str());
    nNewDateTime.nMinute = _ttol(sT2.c_str());
    nNewDateTime.nSecond = _ttol(sT3.c_str());
  }

  m_nTime = CXPubDateTime::_MKTIME(&nNewDateTime);
}










// wandelt das interne Datumsformat in eine Floatzahl
/*
time_t CXPubDateTime::MakeTime(const xtstring& Time)
{
  if (Time.length() < 10)
    return 0;

  struct tm p;
  memset((void *) &p, _XT('\0'), sizeof(struct tm));

  p.tm_mon = _ttoi(Time.substr(0, 2).c_str()) - 1;
  p.tm_mday = _ttoi(Time.substr(3, 2).c_str()) - 1;
  p.tm_year = _ttoi(Time.substr(6, 4).c_str()) - 1900;
  p.tm_isdst = -1;

  if (Time.length() == 19)
  {
    p.tm_hour = _ttoi(Time.substr(11, 2).c_str());
    p.tm_min = _ttoi(Time.substr(14, 2).c_str());
    p.tm_sec = _ttoi(Time.substr(17, 2).c_str());
  }
  time_t m = mktime(&p); // time_t
  return m;
}
*/


bool CXPubVariant::CreateGlobalSettings() const
{
  if (m_pGSettings)
    return true;
  ((CXPubVariant*)this)->m_pGSettings = new CGS_DateTimeNumber;
  if (m_pGSettings)
    return true;
  return false;
}

void CXPubVariant::RemoveGlobalSettings()
{
  if (m_pGSettings)
  {
    delete m_pGSettings;
    m_pGSettings = 0;
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// CXPubVariant constructors
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
CXPubVariant::CXPubVariant()
{
  m_pGSettings = 0;
  Clear();
}
//CXPubVariant::CXPubVariant(const XPUBVARIANT& varSrc)
//{
//  m_pGSettings = 0;
//  sValue = varSrc.sValue;
//  vt = varSrc.vt;
//
//  lVal = varSrc.lVal;
//  bVal = varSrc.bVal;
//  cVal = varSrc.cVal;
//  iVal = varSrc.iVal;
//  tmVal = varSrc.tmVal;
//  dblVal = varSrc.dblVal;
//}
CXPubVariant::CXPubVariant(const CXPubVariant& varSrc)
{
  m_pGSettings = 0;
  sValue = varSrc.sValue;
  vt = varSrc.vt;

  lVal = varSrc.lVal;
  bVal = varSrc.bVal;
  cVal = varSrc.cVal;
  iVal = varSrc.iVal;
  tmVal = varSrc.tmVal;
  dblVal = varSrc.dblVal;
}
CXPubVariant::CXPubVariant(const xtstring& sSrc)
{
  m_pGSettings = 0;
  SetValue(sSrc);
}
CXPubVariant::CXPubVariant(const xtstring& varSrc, const TFieldType vt)
{
  m_pGSettings = 0;
  SetTypedValue(varSrc, vt);
}
CXPubVariant::CXPubVariant(long lSrc)
{
  m_pGSettings = 0;
  SetValue(lSrc);
}
CXPubVariant::CXPubVariant(bool bSrc)
{
  m_pGSettings = 0;
  SetValue(bSrc);
}
CXPubVariant::CXPubVariant(char cSrc)
{
  m_pGSettings = 0;
  SetValue(cSrc);
}
CXPubVariant::CXPubVariant(short iSrc)
{
  m_pGSettings = 0;
  SetValue(iSrc);
}
CXPubVariant::CXPubVariant(double dblSrc)
{
  m_pGSettings = 0;
  SetValue(dblSrc);
}
CXPubVariant::CXPubVariant(CXPubDateTime& dSrc)
{
  m_pGSettings = 0;
  SetValue(dSrc);
}
CXPubVariant::~CXPubVariant()
{
  RemoveGlobalSettings();
}











////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// CXPubVariant xxxx
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void CXPubVariant::Clear()
{
  vt = tFieldTypeEmpty;
  sValue.clear();
}
bool CXPubVariant::IsEmpty() const
{
  return (vt == tFieldTypeEmpty);
}
xtstring CXPubVariant::GetValue() const
{
  return GetTypedValue(GetDefaultFieldTypeOfGroup(tFieldTypeGroupText));
}






////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// CXPubVariant operators
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool CXPubVariant::operator ==(const CXPubVariant& cVar) const
{
  if (vt != cVar.GetType())
    return false;
  if (vt == tFieldTypeInteger)
  {
    if (lVal != cVar.lVal)
      return false;
  }
  else if (vt == tFieldTypeBoolean)
  {
    if (bVal != cVar.bVal)
      return false;
  }
  else if (vt == tFieldTypeChar)
  {
    if (cVal != cVar.cVal)
      return false;
  }
  else if (vt == tFieldTypeSmallInt)
  {
    if (iVal != cVar.iVal)
      return false;
  }
  else if (vt == tFieldTypeDate)
  {
    if (tmVal.m_nTime != cVar.tmVal.m_nTime)
      return false;
  }
  else if (vt == tFieldTypeDouble)
  {
    if (dblVal != cVar.dblVal)
      return false;
  }
  else
  {
    if (sValue != cVar.sValue)
      return false;
  }

  return true;
}
bool CXPubVariant::operator ==(const bool& bSrc) const
{
  //////////////////////////////////////////////////
  if (vt == tFieldTypeInteger)
  {
    if (lVal && bSrc || !lVal && !bSrc)
      return true;
    return false;
  }
  //////////////////////////////////////////////////
  if (vt == tFieldTypeBoolean)
    return (bVal == bSrc);
  //////////////////////////////////////////////////
  if (vt == tFieldTypeChar)
  {
    if (cVal && bSrc || !cVal && !bSrc)
      return true;
    return false;
  }
  //////////////////////////////////////////////////
  if (vt == tFieldTypeSmallInt)
  {
    if (iVal && bSrc || !iVal && !bSrc)
      return true;
    return false;
  }
  //////////////////////////////////////////////////
  if (vt == tFieldTypeDate)
  {
    return false;
  }
  //////////////////////////////////////////////////
  if (vt == tFieldTypeDouble)
  {
    if (dblVal && bSrc || !dblVal && !bSrc)
      return true;
    return false;
  }

  //////////////////////////////////////////////////
  // es ist was nur in sValue
  // hm
  // wir probieren bekannte Texte
  bool bLocalSrc = false;
  int nI = _ttol(sValue.c_str());
  if (!sValue.CompareNoCase(_XT("true")) || nI)
    bLocalSrc = true;

  return (bSrc == bLocalSrc);
}
bool CXPubVariant::operator ==(const xtstring& sSrc) const
{
  return (sValue == sSrc);
}
const CXPubVariant& CXPubVariant::operator =(LPXTCHAR pSrc)
{
  if (pSrc)
  {
    xtstring s(pSrc);
    SetValue(s);
  }
  return *this;
}
const CXPubVariant& CXPubVariant::operator =(xtstring sSrc)
{
  SetValue(sSrc);
  return *this;
}
const CXPubVariant& CXPubVariant::operator =(long lSrc)
{
  SetValue(lSrc);
  return *this;
}
const CXPubVariant& CXPubVariant::operator =(bool bSrc)
{
  SetValue(bSrc);
  return *this;
}
const CXPubVariant& CXPubVariant::operator =(XTCHAR cSrc)
{
  SetValue(cSrc);
  return *this;
}
const CXPubVariant& CXPubVariant::operator =(short iSrc)
{
  SetValue(iSrc);
  return *this;
}
const CXPubVariant& CXPubVariant::operator =(CXPubDateTime& dSrc)
{
  SetValue(dSrc);
  return *this;
}
const CXPubVariant& CXPubVariant::operator =(double dblSrc)
{
  SetValue(dblSrc);
  return *this;
}
void CXPubVariant::operator =(const CXPubVariant& varSrc)
{
  // Das ist doch Unfug!!!! m_pGSettings = 0;
  sValue = varSrc.sValue;
  vt = varSrc.vt;

  lVal = varSrc.lVal;
  bVal = varSrc.bVal;
  cVal = varSrc.cVal;
  iVal = varSrc.iVal;
  tmVal = varSrc.tmVal;
  dblVal = varSrc.dblVal;
}
CXPubVariant::operator xtstring() const
{
  return GetTypedValue(GetDefaultFieldTypeOfGroup(tFieldTypeGroupText));
}
CXPubVariant::operator double() const
{
  if (vt == tFieldTypeDouble)
    return dblVal;

  // wir konvertieren von sValue
  xtstring sGroupingSymbol;
  xtstring sDecimalSymbol;

  if (m_pGSettings || CreateGlobalSettings())
  {
    sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();
    sDecimalSymbol = m_pGSettings->GetDecimalSymbol();
  }
  else
  {
    sGroupingSymbol = DEF_GROUPINGSYMBOL;
    sDecimalSymbol = DEF_DECIMALSYMBOL;
  }

  xtstring sTemp = sValue;
  sTemp.ReplaceAll(sGroupingSymbol, _XT(""));
  sTemp.ReplaceAll(sDecimalSymbol, _XT("."));
  double dRet = _tstof(sTemp.c_str());
  return dRet;
}
CXPubVariant::operator long() const
{
  if (vt == tFieldTypeInteger)
    return lVal;

  if (vt == tFieldTypeDate)
    return tmVal.m_nTime;

//  // wir konvertieren von sValue
//  xtstring sGroupingSymbol;
//  xtstring sDecimalSymbol;
//
//  if (m_pGSettings || CreateGlobalSettings())
//  {
//    sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();
//    sDecimalSymbol = m_pGSettings->GetDecimalSymbol();
//  }
//  else
//  {
//    sGroupingSymbol = DEF_GROUPINGSYMBOL;
//    sDecimalSymbol = DEF_DECIMALSYMBOL;
//  }

  xtstring sTemp = sValue;
//  sTemp.ReplaceAll(sGroupingSymbol, _XT(""));
//  sTemp.ReplaceAll(sDecimalSymbol, _XT("."));
  long nRet = _ttol(sTemp.c_str());
  return nRet;
}
CXPubVariant::operator unsigned int() const
{
//  // wir konvertieren von sValue
//  xtstring sGroupingSymbol;
//  xtstring sDecimalSymbol;
//
//  if (m_pGSettings || CreateGlobalSettings())
//  {
//    sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();
//    sDecimalSymbol = m_pGSettings->GetDecimalSymbol();
//  }
//  else
//  {
//    sGroupingSymbol = DEF_GROUPINGSYMBOL;
//    sDecimalSymbol = DEF_DECIMALSYMBOL;
//  }

  xtstring sTemp = sValue;
//  sTemp.ReplaceAll(sGroupingSymbol, _XT(""));
//  sTemp.ReplaceAll(sDecimalSymbol, _XT("."));
  unsigned int nRet;
  LPXTCHAR p;
  nRet = _tcstoul(sValue.c_str(), &p, 10);
  return nRet;
}

CXPubVariant::operator short() const
{
  short nI = _ttoi(sValue.c_str());
  return nI;
}

CXPubVariant::operator bool() const
{
  bool n = false;
  int nI = _ttol(sValue.c_str());
  if (sValue.FindNoCase(_XT("true")) != xtstring::npos || nI)
    n = true;
  return n;
}
CXPubVariant::operator CXPubDateTime() const
{
  if (vt == tFieldTypeDate)
    return tmVal;

  CXPubDateTime ret;
  ret.m_nTime = 0;
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// CXPubVariant serialize / deserialize
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void CXPubVariant::Serialize(TFieldType& type,
                             xtstring& sVal)
{
  // 1.
//  type = vt;
//  sVal = (xtstring)(*this);
  // 2.
//  type = vt;
//  sVal = GetTypedValueForSQL(vt);
  // 3.
  type = vt;
  if (vt == tFieldTypeEmpty)
  {
    sVal.clear();
  }
  else if (vt == tFieldTypeVarChar)
  {
    sVal = sValue;
  }
  else if (vt == tFieldTypeInteger)
  {
    // long          lVal;   // tFieldTypeInteger
    sVal.Format(_XT("%ld"), lVal);
  }
  else if (vt == tFieldTypeBoolean)
  {
    // bool          bVal;   // tFieldTypeBoolean
    if (bVal)
      sVal = _XT("1");
    else
      sVal = _XT("0");
  }
  else if (vt == tFieldTypeChar)
  {
    // XTCHAR        cVal;   // tFieldTypeChar
    sVal = cVal;
  }
  else if (vt == tFieldTypeSmallInt)
  {
    // short         iVal;   // tFieldTypeSmallInt
    sVal.Format(_XT("%i"), iVal);
  }
  else if (vt == tFieldTypeDate)
  {
    // CXPubDateTime tmVal;  // tFieldTypeDate
    tmVal.Serialize(sVal);
  }
  else if (vt == tFieldTypeDouble)
  {
    // double        dblVal; // tFieldTypeDouble
    sVal.Format(_XT("%f"), dblVal);
  }
  else if (vt == tFieldTypeCurrency)
  {
    // MAK: Was sollen wir tun?
    sVal.clear();
  }
  else
    sVal.clear();
}

void CXPubVariant::Deserialize(TFieldType type,
                               const xtstring& sVal)
{
  // 1.
//  SetTypedValue(sVal, type);
  // 2.
//  SetTypedValueFromSQL(sVal, type);
  // 3.
  if (type == tFieldTypeEmpty)
  {
    Clear();
  }
  else if (type == tFieldTypeVarChar
      || type == tFieldTypeVarWChar
      || type == tFieldTypeLongVarWChar
      || type == tFieldTypeLongVarChar)
  {
    vt = tFieldTypeVarChar;
    sValue = sVal;
  }
  else if (type == tFieldTypeInteger)
  {
    // long          lVal;   // tFieldTypeInteger
    long n = _ttol(sVal.c_str());
    SetValue(n);
  }
  else if (type == tFieldTypeBoolean)
  {
    // bool          bVal;   // tFieldTypeBoolean
    bool n = false;
    int nI = _ttol(sVal.c_str());
    if (nI)
      n = true;
    SetValue(n);
  }
  else if (type == tFieldTypeChar)
  {
    // XTCHAR        cVal;   // tFieldTypeChar
    XTCHAR n = 0;
    if (sVal.size())
      n = sVal[0];
    SetValue(n);
  }
  else if (type == tFieldTypeSmallInt)
  {
    // short         iVal;   // tFieldTypeSmallInt
    short n = (short)_ttol(sVal.c_str());
    SetValue(n);
  }
  else if (type == tFieldTypeDate)
  {
    // CXPubDateTime tmVal;  // tFieldTypeDate
    CXPubDateTime t;
    t.Deserialize(sVal);
    SetValue(t);
  }
  else if (type == tFieldTypeDouble)
  {
    // double        dblVal; // tFieldTypeDouble
    double n = _tstof(sVal.c_str());
    SetValue(n);
  }
  else
    // dazu sollte nie kommen. Serialize kontrollieren.
    Clear();
}





////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// CXPubVariant SetValue
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void CXPubVariant::SetValue(const xtstring& sSrc)
{
  vt = GetDefaultFieldTypeOfGroup(tFieldTypeGroupText);
  sValue = sSrc;
}
void CXPubVariant::SetValue(long lSrc)
{ 
  vt =  tFieldTypeInteger;
  lVal = lSrc; 

  sValue.Format(_XT("%ld"), lSrc);

//  xtstring sGroupingSymbol;
//
//  if (m_pGSettings || CreateGlobalSettings())
//  {
//    sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();
//  }
//  else
//  {
//    sGroupingSymbol = DEF_GROUPINGSYMBOL;
//  }
//
//  if (!sGroupingSymbol.size())
//    return;
//  size_t nPos = sValue.size();
//  for (nPos -= 3; nPos < sValue.size(); nPos -= 3)
//    sValue.insert(nPos, sGroupingSymbol);
}
void CXPubVariant::SetValue(bool bSrc)
{ 
  vt = tFieldTypeBoolean;
  bVal = bSrc;

  if (bSrc)
    sValue = _XT("1");
  else
    sValue = _XT("0");
}
void CXPubVariant::SetValue(XTCHAR cSrc)
{ 
  vt = tFieldTypeChar;
  cVal = cSrc; 
  sValue = cSrc;
} 
void CXPubVariant::SetValue(short iSrc)
{ 
  vt = tFieldTypeSmallInt;
  iVal = iSrc;

  sValue.Format(_XT("%i"), iSrc);

//  xtstring sGroupingSymbol;
//
//  if (m_pGSettings || CreateGlobalSettings())
//  {
//    sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();
//  }
//  else
//  {
//    sGroupingSymbol = DEF_GROUPINGSYMBOL;
//  }
//
//  if (!sGroupingSymbol.size())
//    return;
//  size_t nPos = sValue.size();
//  for (nPos -= 3; nPos < sValue.size(); nPos -= 3)
//    sValue.insert(nPos, sGroupingSymbol);
} 
void CXPubVariant::SetValue(CXPubDateTime& dSrc)
{ 
  vt = tFieldTypeDate;
  tmVal = dSrc; 
  sValue.erase();

  CreateGlobalSettings();
  sValue = tmVal.TimeAsString(m_pGSettings);
/*
  TLongDateFormat nDateFormat;
  xtstring sDateSeparator;
  bool bYearWithCentury;
  xtstring sTimeSeparator;

  if (m_pGSettings || CreateGlobalSettings())
  {
    nDateFormat = (TLongDateFormat)m_pGSettings->GetLongDateFormat();
    sDateSeparator = m_pGSettings->GetDateSeparator();
    bYearWithCentury = m_pGSettings->GetYearWithCentury();
    sTimeSeparator = m_pGSettings->GetTimeSeparator();
  }
  else
  {
    nDateFormat = DEF_DATEFORMAT;
    sDateSeparator = DEF_DATESEPARATOR;
    bYearWithCentury = DEF_YEARWITHCENTURY;
    sTimeSeparator = DEF_TIMESEPARATOR;
  }

  struct tm* ttime;
  ttime = localtime(&dSrc.tTime);
  if (!ttime)
    return;

  xtstring sDay, sMonth, sYear;
  sYear.Format(_XT("%ld"), ttime->tm_year + 1900);
  if (!bYearWithCentury)
    sYear.erase(0, 2);

  if (nDateFormat == tLDF_DMY || nDateFormat == tLDF_MDY || nDateFormat == tLDF_YMD)
  {
    sDay.Format(_XT("%ld"), ttime->tm_mday);
    sMonth.Format(_XT("%ld"), ttime->tm_mon + 1);
  }
  else
  {
    sDay.Format(_XT("%02ld"), ttime->tm_mday);
    sMonth.Format(_XT("%02ld"), ttime->tm_mon + 1);
  }

  switch(nDateFormat)
  {
  case tLDF_DMY:// 1.2.2004     / 1.2.04        1.feb.2004
  case tLDF_DDMMY:// 01.02.2004   / 01.02.04      1.feb.2004
    sValue = sDay; sValue += sDateSeparator; sValue += sMonth; sValue += sDateSeparator; sValue += sYear; sValue += _XT(" ");
    break;
  case tLDF_MDY:// 2.1.2004     / 2.1.04        1.feb.2004
  case tLDF_MMDDY:// 02.01.2004   / 02.01.04      1.feb.2004
    sValue = sMonth; sValue += sDateSeparator; sValue += sDay; sValue += sDateSeparator; sValue += sYear; sValue += _XT(" ");
    break;
  case tLDF_YMD:// 2004.2.1     / 04.2.1        1.feb.2004
  case tLDF_YMMDD:// 2004.02.01   / 04.02.01      1.feb.2004
    sValue = sYear; sValue += sDateSeparator; sValue += sMonth; sValue += sDateSeparator; sValue += sDay; sValue += _XT(" ");
    break;
  }

  xtstring sTemp;
  sTemp.Format(_XT("%02ld%s%02ld%s%02ld"), ttime->tm_hour, sTimeSeparator.c_str(), ttime->tm_min, sTimeSeparator.c_str(), ttime->tm_sec);
  sValue += sTemp;
*/
}
void CXPubVariant::SetValue(double dblSrc)
{ 
  vt = tFieldTypeDouble;
  dblVal = dblSrc;

// no fractional and less short
	if(dblSrc < 0x7FFF && fmod(dblSrc, 1) == 0)
	  sValue.Format(_XT("%ld"), (long)dblSrc);
	else
	  sValue.Format(_XT("%f"), dblSrc);

  xtstring sGroupingSymbol;
  xtstring sDecimalSymbol;

  if (m_pGSettings || CreateGlobalSettings())
  {
    sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();
    sDecimalSymbol = m_pGSettings->GetDecimalSymbol();
  }
  else
  {
    sGroupingSymbol = DEF_GROUPINGSYMBOL;
    sDecimalSymbol = DEF_DECIMALSYMBOL;
  }

  sValue.ReplaceAll(_XT("."), sDecimalSymbol);
  size_t nPos = sValue.find(sDecimalSymbol);
  if (nPos == xtstring::npos || !sGroupingSymbol.size())
    return;
  for (nPos -= 3; nPos < sValue.size(); nPos -= 3)
    sValue.insert(nPos, sGroupingSymbol);
}







////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// CXPubVariant TypedValue / MaskedValue
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void CXPubVariant::SetTypedValueFromSQL(const xtstring& varSrc,
                                        const TFieldType nType/*=tFieldTypeVarChar*/)
{
  if (nType != tFieldTypeDate)
    SetTypedValue(varSrc, nType);
  else
  {
    xtstring sTemp(varSrc);

    xtstring sDatePart;
    xtstring sTimePart;
    // Datum ist vorne, Zeit hinten, dazwischen ist " "
    size_t nPos = sTemp.find(_XT(" "));
    if (nPos == xtstring::npos)
    {
      sDatePart.assign(sTemp);
      sTimePart.assign(sTemp);
    }
    else
    {
      sDatePart.assign(sTemp.begin(), sTemp.begin() + nPos);
      sTimePart.assign(sTemp.begin() + nPos + 1, sTemp.end());
    }
    QLHTM nNewDateTime;
    nNewDateTime.Reset();

    /////////////////////////////////////////////////
    /////////////////////////////////////////////////
    // date
    xtstring sD1, sD2, sD3;
    nPos = sDatePart.find(DB_DATE_DELIMITER);
    if (nPos != xtstring::npos)
    {
      sD1.assign(sDatePart.begin(), sDatePart.begin() + nPos);
      sDatePart.erase(sDatePart.begin(), sDatePart.begin() + nPos + 1);
      nPos = sDatePart.find(DB_DATE_DELIMITER);
      if (nPos != xtstring::npos)
      {
        sD2.assign(sDatePart.begin(), sDatePart.begin() + nPos);
        sDatePart.erase(sDatePart.begin(), sDatePart.begin() + nPos + 1);
        sD3 = sDatePart;
      }
    }
    if (sD1.size() != 0 && sD2.size() != 0 && sD3.size() != 0)
    {
      int n1, n2, n3;
      n1 = _ttol(sD1.c_str());
      n2 = _ttol(sD2.c_str());
      n3 = _ttol(sD3.c_str());
      // in DB ist immer M/D/Y
      nNewDateTime.nDay = n2;
      nNewDateTime.nMonth = n1/* - 1*/;
      nNewDateTime.nYear = n3/* - 1900*/;
    }

    /////////////////////////////////////////////////
    /////////////////////////////////////////////////
    // time
    xtstring sT1, sT2, sT3;
    nPos = sTimePart.find(DB_TIME_DELIMITER);
    if (nPos != xtstring::npos)
    {
      sT1.assign(sTimePart.begin(), sTimePart.begin() + nPos);
      sTimePart.erase(sTimePart.begin(), sTimePart.begin() + nPos + 1);
      nPos = sTimePart.find(DB_TIME_DELIMITER);
      if (nPos != xtstring::npos)
      {
        sT2.assign(sTimePart.begin(), sTimePart.begin() + nPos);
        sTimePart.erase(sTimePart.begin(), sTimePart.begin() + nPos + 1);
        sT3 = sTimePart;
      }
    }
    if (sT1.size() != 0 && sT2.size() != 0 && sT3.size() != 0)
    {
      nNewDateTime.nHour = _ttol(sT1.c_str());
      nNewDateTime.nMinute = _ttol(sT2.c_str());
      nNewDateTime.nSecond = _ttol(sT3.c_str());
    }

    CXPubDateTime t;
    t.m_nTime = t._MKTIME(&nNewDateTime);
    INT64 nVal;
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
    nVal = EMPTY_DATETIME_HIGH;
    nVal <<= 32;
    nVal += EMPTY_DATETIME_LOW;
#elif defined(WIN32)
    nVal = EMPTY_DATETIME;
#else
#error "not implemented"
#endif
    if (t.m_nTime != nVal)
      SetValue(t);
    else
      Clear();
  }
}
void CXPubVariant::SetTypedValue(const xtstring& varSrc,
                                 const TFieldType nType/*=tFieldTypeVarChar*/)
{
  xtstring sTemp(varSrc);

  if (nType == tFieldTypeInteger
      || nType == tFieldTypeTinyInt
      || nType == tFieldTypeBigInt)
  {
//    xtstring sGroupingSymbol;
//    xtstring sDecimalSymbol;
//    if (m_pGSettings || CreateGlobalSettings())
//      {sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();sDecimalSymbol = m_pGSettings->GetDecimalSymbol();}
//    else
//      {sGroupingSymbol = DEF_GROUPINGSYMBOL;sDecimalSymbol = DEF_DECIMALSYMBOL;}
//
//    sTemp.ReplaceAll(sGroupingSymbol, _XT(""));
//    sTemp.ReplaceAll(sDecimalSymbol, _XT("."));
    long n = _ttol(sTemp.c_str());
    SetValue(n);
  }
  else if (nType == tFieldTypeBoolean)
  {
    bool n = false;
    int nI = _ttol(sTemp.c_str());
    if (sTemp.FindNoCase(_XT("true")) != xtstring::npos || nI)
      n = true;
    SetValue(n);
  }
  else if (nType == tFieldTypeChar)
  {
    XTCHAR n = 0;
    if (sTemp.size())
      n = sTemp[0];
    SetValue(n);
  }
  else if (nType == tFieldTypeSmallInt)
  {
//    xtstring sGroupingSymbol;
//    xtstring sDecimalSymbol;
//    if (m_pGSettings || CreateGlobalSettings())
//      {sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();sDecimalSymbol = m_pGSettings->GetDecimalSymbol();}
//    else
//      {sGroupingSymbol = DEF_GROUPINGSYMBOL;sDecimalSymbol = DEF_DECIMALSYMBOL;}
//
//    sTemp.ReplaceAll(sGroupingSymbol, _XT(""));
//    sTemp.ReplaceAll(sDecimalSymbol, _XT("."));
    short n = (short)_ttol(sTemp.c_str());
    SetValue(n);
  }
  else if (nType == tFieldTypeDate)
  {
    TLongDateFormat nDateFormat;
    xtstring sDateSeparator;
    bool bYearWithCentury;
    xtstring sTimeSeparator;
    if (m_pGSettings || CreateGlobalSettings())
      {nDateFormat = (TLongDateFormat)m_pGSettings->GetLongDateFormat();sDateSeparator = m_pGSettings->GetDateSeparator();bYearWithCentury = m_pGSettings->GetYearWithCentury();sTimeSeparator = m_pGSettings->GetTimeSeparator();}
    else
      {nDateFormat = DEF_DATEFORMAT;sDateSeparator = DEF_DATESEPARATOR;bYearWithCentury = DEF_YEARWITHCENTURY;sTimeSeparator = DEF_TIMESEPARATOR;}

    xtstring sDatePart;
    xtstring sTimePart;
    // Datum ist vorne, Zeit hinten, dazwischen ist " "
    size_t nPos = sTemp.find(_XT(" "));
    if (nPos == xtstring::npos)
    {
      sDatePart.assign(sTemp);
      sTimePart.assign(sTemp);
    }
    else
    {
      sDatePart.assign(sTemp.begin(), sTemp.begin() + nPos);
      sTimePart.assign(sTemp.begin() + nPos + 1, sTemp.end());
    }
    QLHTM nNewDateTime;
    nNewDateTime.Reset();

    /////////////////////////////////////////////////
    /////////////////////////////////////////////////
    // date
    xtstring sD1, sD2, sD3;
    nPos = sDatePart.find(sDateSeparator);
    if (nPos != xtstring::npos)
    {
      sD1.assign(sDatePart.begin(), sDatePart.begin() + nPos);
      sDatePart.erase(sDatePart.begin(), sDatePart.begin() + nPos + 1);
      nPos = sDatePart.find(sDateSeparator);
      if (nPos != xtstring::npos)
      {
        sD2.assign(sDatePart.begin(), sDatePart.begin() + nPos);
        sDatePart.erase(sDatePart.begin(), sDatePart.begin() + nPos + 1);
        sD3 = sDatePart;
      }
    }
    if (sD1.size() != 0 && sD2.size() != 0 && sD3.size() != 0)
    {
      int n1, n2, n3;
      n1 = _ttol(sD1.c_str());
      n2 = _ttol(sD2.c_str());
      n3 = _ttol(sD3.c_str());
      switch(nDateFormat)
      {
      case tLDF_DMY:// 1.2.2004     / 1.2.04        1.feb.2004
      case tLDF_DDMMY:// 01.02.2004   / 01.02.04      1.feb.2004
        if (n3 < 100) n3 += 2000;
        nNewDateTime.nDay = n1;
        nNewDateTime.nMonth = n2/* - 1*/;
        nNewDateTime.nYear = n3/* - 1900*/;
        break;
      case tLDF_MDY:// 2.1.2004     / 2.1.04        1.feb.2004
      case tLDF_MMDDY:// 02.01.2004   / 02.01.04      1.feb.2004
        if (n3 < 100) n3 += 2000;
        nNewDateTime.nDay = n2;
        nNewDateTime.nMonth = n1/* - 1*/;
        nNewDateTime.nYear = n3/* - 1900*/;
        break;
      case tLDF_YMD:// 2004.2.1     / 04.2.1        1.feb.2004
      case tLDF_YMMDD:// 2004.02.01   / 04.02.01      1.feb.2004
        if (n1 < 100) n1 += 2000;
        nNewDateTime.nDay = n3;
        nNewDateTime.nMonth = n2/* - 1*/;
        nNewDateTime.nYear = n1/* - 1900*/;
        break;
      }
    }

    /////////////////////////////////////////////////
    /////////////////////////////////////////////////
    // time
    xtstring sT1, sT2, sT3;
    nPos = sTimePart.find(sTimeSeparator);
    if (nPos != xtstring::npos)
    {
      sT1.assign(sTimePart.begin(), sTimePart.begin() + nPos);
      sTimePart.erase(sTimePart.begin(), sTimePart.begin() + nPos + 1);
      nPos = sTimePart.find(sTimeSeparator);
      if (nPos != xtstring::npos)
      {
        sT2.assign(sTimePart.begin(), sTimePart.begin() + nPos);
        sTimePart.erase(sTimePart.begin(), sTimePart.begin() + nPos + 1);
        sD3 = sTimePart;
      }
    }
    if (sT1.size() != 0 && sT2.size() != 0 && sT3.size() != 0)
    {
      nNewDateTime.nHour = _ttol(sT1.c_str());
      nNewDateTime.nMinute = _ttol(sT2.c_str());
      nNewDateTime.nSecond = _ttol(sT3.c_str());
    }

    CXPubDateTime t;
    t.m_nTime = t._MKTIME(&nNewDateTime);
    INT64 nVal;
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
    nVal = EMPTY_DATETIME_HIGH;
    nVal <<= 32;
    nVal += EMPTY_DATETIME_LOW;
#elif defined(WIN32)
    nVal = EMPTY_DATETIME;
#else
#error "not implemented"
#endif
    if (t.m_nTime != nVal)
      SetValue(t);
    else
      Clear();
  }
  else if (nType == tFieldTypeDouble || 
		nType == tFieldTypeDecimal)
  {
    xtstring sGroupingSymbol;
    xtstring sDecimalSymbol;
    if (m_pGSettings || CreateGlobalSettings())
      {sGroupingSymbol = m_pGSettings->GetDigitGroupingSymbol();sDecimalSymbol = m_pGSettings->GetDecimalSymbol();}
    else
      {sGroupingSymbol = DEF_GROUPINGSYMBOL;sDecimalSymbol = DEF_DECIMALSYMBOL;}

    sTemp.ReplaceAll(sGroupingSymbol, _XT(""));
    sTemp.ReplaceAll(sDecimalSymbol, _XT("."));
    double n = _tstof(sTemp.c_str());

    SetValue(n);
  }
  else if (nType == tFieldTypeVarChar
           || nType == tFieldTypeVarWChar
           || nType == tFieldTypeLongVarWChar
           || nType == tFieldTypeLongVarChar)
  {
    vt = tFieldTypeVarChar;
    sValue = sTemp;
  }
  else if (nType == tFieldTypeLongVarBinary)
  {    
    vt = tFieldTypeVarChar;
    sValue = sTemp;
  }
  else if (nType == tFieldTypeEmpty)
  {
    Clear();
  }
  else
  {
    Clear();   
   // assert(0); // Datentyp muss bearbeitet werden
  }
}
xtstring CXPubVariant::GetTypedValueForSQL(const TFieldType nType) const
{
  xtstring sRet;
  if (nType == tFieldTypeInteger)
  {
    if (GetType() == tFieldTypeInteger)
      sRet.Format(_XT("%ld"), lVal);
    else
      sRet = sValue;
  }
  else if (nType == tFieldTypeBoolean)
  {
    if (GetType() == tFieldTypeBoolean)
    {
      if (bVal)
        sRet = _XT("1");
      else
        sRet = _XT("0");
    }
    else
      sRet = sValue;
  }
  else if (nType == tFieldTypeChar)
  {
    if (GetType() == tFieldTypeChar)
      sRet = cVal;
    else
      sRet = sValue;
  }
  else if (nType == tFieldTypeSmallInt)
  {
    if (GetType() == tFieldTypeSmallInt)
      sRet.Format(_XT("%i"), iVal);
    else
      sRet = sValue;
  }
  else if (nType == tFieldTypeDate)
  {
    if (GetType() == tFieldTypeDate)
      sRet = tmVal.TimeAsSQLString();
    else
      sRet = sValue;
  }
  else if (nType == tFieldTypeDouble)
  {
    if (GetType() == tFieldTypeDouble)
      sRet.Format(_XT("%f"), dblVal);
    else
      sRet = sValue;
  }
  else
    sRet = sValue;
  return sRet;
}
xtstring CXPubVariant::GetTypedValue(const TFieldType nType) const
{
  // in sValue muss immer was stehen, wenn auch double in CXPubVariant eingestellt ist.
  // Wenn man in CXPubVariant was einstellt, es muss auch sValue gesetzt werden
  return sValue;
}

/*
  In der Datei MPModelStone.cpp oben findet man
  #define COUNT_OF_TEXTFIELDCONTENTMASKS    7
  LPCXTCHAR gpTextFieldContentMasks[COUNT_OF_TEXTFIELDCONTENTMASKS] = 
  Dort kann man die Masken ändern
*/
xtstring CXPubVariant::GetMaskedValue(const xtstring& sMask)
{
  if (GetGroupOfFieldType(GetType()) == tFieldTypeGroupCurrency
      || GetGroupOfFieldType(GetType()) == tFieldTypeGroupNumber)
  {
    if (!m_pGSettings)
      CreateGlobalSettings();
    CFormatNumber cFormat(m_pGSettings);
    xtstring sFormated;
    if (sMask.size() && cFormat.SetMask(sMask))
    { 
      if (GetGroupOfFieldType(GetType()) == tFieldTypeGroupCurrency)
        cFormat.FormatValue(dblVal, sFormated);
      else
        cFormat.FormatValue(lVal, sFormated);
      return sFormated;
    }
  }
  if (GetGroupOfFieldType(GetType()) == tFieldTypeGroupDateTime)
  {
    try
    {
      if (sMask.size())
      {
        CFormatDateTime cFormat(tmVal.m_nTime);
        xtstring sFormated = cFormat.getFormatedDateTime(sMask.c_str());
        return sFormated;
      }
    }
    catch (...)
    {
      return sValue;
    }
  }
  // Es war verlangt, dass ein Text als Nummer ausgegeben ist,
  // falls Maske fuer Zahl definiert ist.
  if (GetGroupOfFieldType(GetType()) == tFieldTypeGroupText
      && sMask.size() && sMask[0] == _XT('#'))
  {
    if (!m_pGSettings)
      CreateGlobalSettings();
    CFormatNumber cFormat(m_pGSettings);
    xtstring sFormated;
    if (cFormat.SetMask(sMask))
    { 
      cFormat.FormatValue((double)*this, sFormated);
      return sFormated;
    }
  }
  return sValue;
}












////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// CXPubVariant static functions
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
TGroupOfFieldTypes CXPubVariant::GetGroupOfFieldType(TFieldType nType)
{
  switch (nType)
  {
  case tFieldTypeEmpty:
    return tFieldTypeGroupStatus;
  case tFieldTypeError:
  case tFieldTypeUserDefined:
  case tFieldTypeVariant:
  case tFieldTypeIDispatch:
  case tFieldTypeIUnknown:
  case tFieldTypeGUID:
  case tFieldTypeChapter:
  case tFieldTypePropVariant:
    return tFieldTypeGroupErrorType;
  case tFieldTypeBinary:
  case tFieldTypeVarBinary:
  case tFieldTypeLongVarBinary:
  case tFieldTypeBSTR:
    return tFieldTypeGroupBinary;
  case tFieldTypeTinyInt:
  case tFieldTypeSmallInt:
  case tFieldTypeInteger:
  case tFieldTypeBigInt:
  case tFieldTypeUnsignedTinyInt:
  case tFieldTypeUnsignedSmallInt:
  case tFieldTypeUnsignedInt:
  case tFieldTypeUnsignedBigInt:
  case tFieldTypeSingle:
  case tFieldTypeDecimal:
  case tFieldTypeNumeric:
  case tFieldTypeBoolean:
  case tFieldTypeVarNumeric:
    return tFieldTypeGroupNumber;
  case tFieldTypeDouble:
  case tFieldTypeCurrency:
    return tFieldTypeGroupCurrency;
  case tFieldTypeDate:
  case tFieldTypeDBDate:
  case tFieldTypeDBTime:
  case tFieldTypeDBTimeStamp:
  case tFieldTypeFileTime:
    return tFieldTypeGroupDateTime;
  case tFieldTypeChar:
  case tFieldTypeVarChar:
  case tFieldTypeLongVarChar:
  case tFieldTypeWChar:
  case tFieldTypeVarWChar:
  case tFieldTypeLongVarWChar:
    return tFieldTypeGroupText;
  }
  assert(false);
  return tFieldTypeGroupErrorType;
}

TFieldType CXPubVariant::GetDefaultFieldTypeOfGroup(TGroupOfFieldTypes nGroup)
{
  TFieldType nFieldType;
  switch (nGroup)
  {
  case tFieldTypeGroupStatus:
    nFieldType = tFieldTypeEmpty;
    break;
  case tFieldTypeGroupErrorType:
    nFieldType = tFieldTypeError;
    break;
  case tFieldTypeGroupBinary:
    nFieldType = tFieldTypeLongVarBinary;
    break;
  case tFieldTypeGroupNumber:
    nFieldType = tFieldTypeBigInt;
    break;
  case tFieldTypeGroupCurrency:
    nFieldType = tFieldTypeDouble;
    break;
  case tFieldTypeGroupDateTime:
    nFieldType = tFieldTypeDate;
    break;
  case tFieldTypeGroupText:
    nFieldType = tFieldTypeVarChar;
    break;
  default:
    assert(false);
    nFieldType = tFieldTypeEmpty; 
  }
  return nFieldType;
}

bool CXPubVariant::IsDefaultFieldTypeOfGroup(TFieldType nType)
{
  if (GetDefaultFieldTypeOfGroup(tFieldTypeGroupStatus) == nType)
    return true;
  else if (GetDefaultFieldTypeOfGroup(tFieldTypeGroupErrorType) == nType)
    return true;
  else if (GetDefaultFieldTypeOfGroup(tFieldTypeGroupBinary) == nType)
    return true;
  else if (GetDefaultFieldTypeOfGroup(tFieldTypeGroupNumber) == nType)
    return true;
  else if (GetDefaultFieldTypeOfGroup(tFieldTypeGroupCurrency) == nType)
    return true;
  else if (GetDefaultFieldTypeOfGroup(tFieldTypeGroupDateTime) == nType)
    return true;
  else if (GetDefaultFieldTypeOfGroup(tFieldTypeGroupText) == nType)
    return true;
  return false;
}

TFieldType CXPubVariant::TranslateFieldTypeFromWIDEToANSI(TFieldType nType)
{
  TFieldType nNewType;
  switch (nType)
  {
  case tFieldTypeWChar:
    nNewType = tFieldTypeChar;
    break;
  case tFieldTypeVarWChar:
    nNewType = tFieldTypeVarChar;
    break;
  case tFieldTypeLongVarWChar:
    nNewType = tFieldTypeLongVarChar;
    break;
  default:
    nNewType = nType;
    break;
  }
  return nNewType;
}

#ifdef WIN32
#pragma warning( default : 4172)
#endif // WIN32


