#include <assert.h>
#include "GlobalSettings.h"



/*
  {
    // test
    DWORD dwTLS = TlsAlloc();
    ASSERT(dwTLS != TLS_OUT_OF_INDEXES);
    LPVOID lpVoid = TlsGetValue(dwTLS);
    BOOL bRet = TlsSetValue(dwTLS, (LPVOID)0x1234);
    ASSERT(bRet);
    lpVoid = TlsGetValue(dwTLS);
    bRet = TlsSetValue(dwTLS, (LPVOID)0x12345678);
    ASSERT(bRet);
    lpVoid = TlsGetValue(dwTLS);
//    TlsFree(dwTLS);
    lpVoid = TlsGetValue(dwTLS);
    lpVoid = TlsGetValue(dwTLS);
  }

int pthread_key_create(pthread_key_t *key, void (*destr_function)(void *));
int pthread_key_delete(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void* pointer);
void* pthread_getspecific(pthread_key_t key);
*/


CGlobalSettings::CGlobalSettings(TGlobalSettingsUser nUser /*= tGSU_Get*/)
{
  m_nUser = nUser;
}
CGlobalSettings::~CGlobalSettings()
{
}

bool CGlobalSettings::CreateGlobalSettings()
{
  GS_ASSERT(m_nUser == tGSU_Creator);
  bool bRet = false;

  GS_ASSERT(GetGlobalSettingsHandle() == GS_HANDLE_NULL);
  if (GetGlobalSettingsHandle() != GS_HANDLE_NULL)
    return true;

#if defined(LINUX_OS)
  pthread_key_create(GetGlobalSettingsHandlePtr(), 0);
#elif defined(WIN32)
  GS_HANDLE handle = TlsAlloc();
  if (handle != TLS_OUT_OF_INDEXES)
  {
    SetGlobalSettingsHandle(handle);
    bRet = true;
  }
#elif defined(MAC_OS_MWE)  
#else
#error "not implemented"
#endif
  return bRet;
}

bool CGlobalSettings::RemoveGlobalSettings()
{
  GS_ASSERT(m_nUser == tGSU_Creator);
  bool bRet = false;

  GS_ASSERT(GetGlobalSettingsHandle() != GS_HANDLE_NULL);
  if (GetGlobalSettingsHandle() == GS_HANDLE_NULL)
    return true;

#if defined(LINUX_OS)
  pthread_key_delete(GetGlobalSettingsHandle());
  bRet = true;
#elif defined(WIN32)
  BOOL b = TlsFree(GetGlobalSettingsHandle());
  GS_ASSERT(b);
  SetGlobalSettingsHandle(GS_HANDLE_NULL);
  bRet = true;
#elif defined(MAC_OS_MWE)  
#else
#error "not implemented"
#endif
  return bRet;
}

bool CGlobalSettings::WriteGlobalSettings()
{
  GS_ASSERT(m_nUser == tGSU_Set);
  bool bRet = false;

  GS_ASSERT(GetGlobalSettingsHandle() != GS_HANDLE_NULL);
  if (GetGlobalSettingsHandle() == GS_HANDLE_NULL)
    return false;

#if defined(LINUX_OS)
  pthread_setspecific(GetGlobalSettingsHandle(), GetGlobalSettings());
  bRet = true;
#elif defined(WIN32)
  if (TlsSetValue(GetGlobalSettingsHandle(), GetGlobalSettings()))
    bRet = true;
#elif defined(MAC_OS_MWE)  
#else
#error "not implemented"
#endif
  return bRet;
}

bool CGlobalSettings::ClearGlobalSettings()
{
  GS_ASSERT(m_nUser == tGSU_Set);
  bool bRet = false;

  GS_ASSERT(GetGlobalSettingsHandle() != GS_HANDLE_NULL);
  if (GetGlobalSettingsHandle() == GS_HANDLE_NULL)
    return false;

#if defined(LINUX_OS)
  pthread_setspecific(GetGlobalSettingsHandle(), 0);
  bRet = true;
#elif defined(WIN32)
  if (TlsSetValue(GetGlobalSettingsHandle(), 0))
    bRet = true;
#elif defined(MAC_OS_MWE)  
#else
#error "not implemented"
#endif
  return bRet;
}

bool CGlobalSettings::ReadGlobalSettings()
{
  GS_ASSERT(m_nUser == tGSU_Get);
  bool bRet = false;

  GS_ASSERT(GetGlobalSettingsHandle() != GS_HANDLE_NULL);
  if (GetGlobalSettingsHandle() == GS_HANDLE_NULL)
    return false;

#if defined(LINUX_OS)
  SetGlobalSettings(pthread_getspecific(GetGlobalSettingsHandle()));
#elif defined(WIN32)
  SetGlobalSettings(TlsGetValue(GetGlobalSettingsHandle()));
#elif defined(MAC_OS_MWE)  
#else
#error "not implemented"
#endif
  return bRet;
}










GS_HANDLE CGS_DateTimeNumber::m_handle = GS_HANDLE_NULL;

#define DEFAULT_YEARWITHCENTURY                 true
#define DEFAULT_DATESEPARATOR                   _GST(".")
#define DEFAULT_TIMESEPARATOR                   _GST(":")
#define DEFAULT_DECIMALSYMBOL                   _GST(",")
#define DEFAULT_DIGITGROUPINGSYMBOL             _GST(".")
#define DEFAULT_USECOUNTOFDIGITSAFTERDECIMAL    false
#define DEFAULT_COUNTOFDIGITSAFTERDECIMAL       2
#define DEFAULT_NEGATIVESIGN                    _GST("-")


CGS_DateTimeNumber::CGS_DateTimeNumber(TGlobalSettingsUser nUser /*= tGSU_Get*/)
                   :CGlobalSettings(nUser)
{
  m_pSettings = 0;
  m_cSettings.m_nLongDateFormat = tLDF_Default;
  m_cSettings.m_bYearWithCentury = DEFAULT_YEARWITHCENTURY;
  m_cSettings.m_sDateSeparator = DEFAULT_DATESEPARATOR;
  m_cSettings.m_sTimeSeparator = DEFAULT_TIMESEPARATOR;
  m_cSettings.m_sDecimalSymbol = DEFAULT_DECIMALSYMBOL;
  m_cSettings.m_sDigitGroupingSymbol = DEFAULT_DIGITGROUPINGSYMBOL;
  m_cSettings.m_bUseCountOfDigitsAfterDecimal = DEFAULT_USECOUNTOFDIGITSAFTERDECIMAL;
  m_cSettings.m_nCountOfDigitsAfterDecimal = DEFAULT_COUNTOFDIGITSAFTERDECIMAL;

  if (m_nUser == tGSU_Creator)
  {
    CreateGlobalSettings();
  }
  else if (m_nUser == tGSU_Set)
  {
    int n = 10;
  }
  else if (m_nUser == tGSU_Get)
  {
    ReadGlobalSettings();
  }
  else
    GS_ASSERT(false);
}
CGS_DateTimeNumber::~CGS_DateTimeNumber()
{
  if (m_nUser == tGSU_Creator)
  {
    RemoveGlobalSettings();
  }
  else if (m_nUser == tGSU_Set)
  {
    ClearGlobalSettings();
  }
}







CGS_DateTimeNumber::TLongDateFormat CGS_DateTimeNumber::GetLongDateFormat()
{
  if (m_pSettings)
    return m_pSettings->m_nLongDateFormat;
  return m_cSettings.m_nLongDateFormat;
}
bool CGS_DateTimeNumber::GetYearWithCentury()
{
  if (m_pSettings)
    return m_pSettings->m_bYearWithCentury;
  return m_cSettings.m_bYearWithCentury;
}
gststring CGS_DateTimeNumber::GetDateSeparator()
{
  if (m_pSettings)
    return m_pSettings->m_sDateSeparator;
  return m_cSettings.m_sDateSeparator;
}
gststring CGS_DateTimeNumber::GetTimeSeparator()
{
  if (m_pSettings)
    return m_pSettings->m_sTimeSeparator;
  return m_cSettings.m_sTimeSeparator;
}
gststring CGS_DateTimeNumber::GetDecimalSymbol()
{
  if (m_pSettings)
    return m_pSettings->m_sDecimalSymbol;
  return m_cSettings.m_sDecimalSymbol;
}
gststring CGS_DateTimeNumber::GetDigitGroupingSymbol()
{
  if (m_pSettings)
    return m_pSettings->m_sDigitGroupingSymbol;
  return m_cSettings.m_sDigitGroupingSymbol;
}
bool CGS_DateTimeNumber::GetUseCountOfDigitsAfterDecimal()
{
  if (m_pSettings)
    return m_pSettings->m_bUseCountOfDigitsAfterDecimal;
  return m_cSettings.m_bUseCountOfDigitsAfterDecimal;
}
int CGS_DateTimeNumber::GetCountOfDigitsAfterDecimal()
{
  if (m_pSettings)
    return m_pSettings->m_nCountOfDigitsAfterDecimal;
  return m_cSettings.m_nCountOfDigitsAfterDecimal;
}


void CGS_DateTimeNumber::SetLongDateFormat(TLongDateFormat nLongDateFormat)
{
  if (m_pSettings)
    m_pSettings->m_nLongDateFormat = nLongDateFormat;
  m_cSettings.m_nLongDateFormat = nLongDateFormat;
}
void CGS_DateTimeNumber::SetYearWithCentury(bool bYearWithCentury)
{
  if (m_pSettings)
    m_pSettings->m_bYearWithCentury = bYearWithCentury;
  m_cSettings.m_bYearWithCentury = bYearWithCentury;
}
void CGS_DateTimeNumber::SetDateSeparator(const gststring& sDateSeparator)
{
  if (m_pSettings)
    m_pSettings->m_sDateSeparator = sDateSeparator;
  m_cSettings.m_sDateSeparator = sDateSeparator;
}
void CGS_DateTimeNumber::SetTimeSeparator(const gststring& sTimeSeparator)
{
  if (m_pSettings)
    m_pSettings->m_sTimeSeparator = sTimeSeparator;
  m_cSettings.m_sTimeSeparator = sTimeSeparator;
}
void CGS_DateTimeNumber::SetDecimalSymbol(const gststring& sDecimalSymbol)
{
  if (m_pSettings)
    m_pSettings->m_sDecimalSymbol = sDecimalSymbol;
  m_cSettings.m_sDecimalSymbol = sDecimalSymbol;
}
void CGS_DateTimeNumber::SetDigitGroupingSymbol(const gststring& sDigitGroupingSymbol)
{
  if (m_pSettings)
    m_pSettings->m_sDigitGroupingSymbol = sDigitGroupingSymbol;
  m_cSettings.m_sDigitGroupingSymbol = sDigitGroupingSymbol;
}
void CGS_DateTimeNumber::SetUseCountOfDigitsAfterDecimal(bool bUseCountOfDigitsAfterDecimal)
{
  if (m_pSettings)
    m_pSettings->m_bUseCountOfDigitsAfterDecimal = bUseCountOfDigitsAfterDecimal;
  m_cSettings.m_bUseCountOfDigitsAfterDecimal = bUseCountOfDigitsAfterDecimal;
}
void CGS_DateTimeNumber::SetCountOfDigitsAfterDecimal(int nCountOfDigitsAfterDecimal)
{
  if (m_pSettings)
    m_pSettings->m_nCountOfDigitsAfterDecimal = nCountOfDigitsAfterDecimal;
  m_cSettings.m_nCountOfDigitsAfterDecimal = nCountOfDigitsAfterDecimal;
}

