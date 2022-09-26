#if !defined(_GLOBALSETTINGS_H_)
#define _GLOBALSETTINGS_H_


#include "XPubToolsExpImp.h"

#include <string>

using namespace std;


#if defined(UNICODE) || defined(_UNICODE)

typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>  > gststring;
#define _GST(x)         L##x
typedef wchar_t         GSTCHAR;
typedef wchar_t*        LPGSTCHAR;
typedef const wchar_t*  LPCGSTCHAR;

#else // #if defined(UNICODE) || defined(_UNICODE)

typedef basic_string<char,    char_traits<char>,    allocator<char>     > gststring;
#define _GST(x)          x
typedef char            GSTCHAR;
typedef char*           LPGSTCHAR;
typedef const char*     LPCGSTCHAR;

#endif // #if defined(UNICODE) || defined(_UNICODE)






#ifdef _DEBUG
#define GS_ASSERT(f)  assert(f);
#else // _DEBUG
#define GS_ASSERT(f)
#endif // _DEBUG




#if defined(LINUX_OS)

#include <pthread.h>
#define GS_HANDLE         pthread_key_t
#define GS_HANDLE_NULL    (GS_HANDLE)-1

#elif defined(WIN32)

#include "windows.h"
#define GS_HANDLE         DWORD
#define GS_HANDLE_NULL    (GS_HANDLE)-1

#elif defined(MAC_OS_MWE)

//#include <msl_thread>
#define GS_HANDLE         unsigned int
#define GS_HANDLE_NULL    (GS_HANDLE)-1

#else
#error "not implemented"
#endif



class XPUBTOOLS_EXPORTIMPORT CGlobalSettings
{
public:
  typedef enum TGlobalSettingsUser
  {
    tGSU_Creator = 0,
    tGSU_Set,
    tGSU_Get,
  };

protected:
  CGlobalSettings(TGlobalSettingsUser nUser = tGSU_Get);
  virtual ~CGlobalSettings();

  //
  bool CreateGlobalSettings();
  bool RemoveGlobalSettings();
  bool WriteGlobalSettings();
  bool ClearGlobalSettings();

  //
  bool ReadGlobalSettings();

protected:
  TGlobalSettingsUser m_nUser;

  virtual GS_HANDLE GetGlobalSettingsHandle() = 0;
  virtual GS_HANDLE* GetGlobalSettingsHandlePtr() = 0;
  virtual void SetGlobalSettingsHandle(GS_HANDLE handle) = 0;

  virtual void SetGlobalSettings(void* pData) = 0;
  virtual void* GetGlobalSettings() = 0;
};









class XPUBTOOLS_EXPORTIMPORT CGS_DateTimeNumber : public CGlobalSettings
{
public:
  CGS_DateTimeNumber(TGlobalSettingsUser nUser = tGSU_Get);
  virtual ~CGS_DateTimeNumber();

  typedef enum TLongDateFormat
  {
    tLDFFirst_Dummy = 0,
    tLDF_DMY,                 // 1.2.2004     / 1.2.04        1.feb.2004
    tLDF_DDMMY,               // 01.02.2004   / 01.02.04      1.feb.2004
    tLDF_MDY,                 // 2.1.2004     / 2.1.04        1.feb.2004
    tLDF_MMDDY,               // 02.01.2004   / 02.01.04      1.feb.2004
    tLDF_YMD,                 // 2004.2.1     / 04.2.1        1.feb.2004
    tLDF_YMMDD,               // 2004.02.01   / 04.02.01      1.feb.2004
    tLDFLast_Dummy,
    tLDF_Default = tLDF_DDMMY,
  };

  bool WriteGlobalSettings(){return CGlobalSettings::WriteGlobalSettings();};

  TLongDateFormat GetLongDateFormat();
  bool            GetYearWithCentury(); // false-04 / true-2004
  gststring       GetDateSeparator();
  gststring       GetTimeSeparator();
  gststring       GetDecimalSymbol();
  gststring       GetDigitGroupingSymbol();
  bool            GetUseCountOfDigitsAfterDecimal();
  int             GetCountOfDigitsAfterDecimal();
  void SetLongDateFormat(TLongDateFormat nLongDateFormat);
  void SetYearWithCentury(bool bYearWithCentury); // false-04 / true-2004
  void SetDateSeparator(const gststring& sDateSeparator);
  void SetTimeSeparator(const gststring& sTimeSeparator);
  void SetDecimalSymbol(const gststring& sDecimalSymbol);
  void SetDigitGroupingSymbol(const gststring& sDigitGroupingSymbol);
  void SetUseCountOfDigitsAfterDecimal(bool bUseCountOfDigitsAfterDecimal);
  void SetCountOfDigitsAfterDecimal(int nCountOfDigitsAfterDecimal);

protected:
  virtual GS_HANDLE GetGlobalSettingsHandle(){return m_handle;};
  virtual GS_HANDLE* GetGlobalSettingsHandlePtr(){return &m_handle;};
  virtual void SetGlobalSettingsHandle(GS_HANDLE handle){m_handle = handle;};

  virtual void SetGlobalSettings(void* pData){m_pSettings = (DateTimeNumberSettings*)pData;};
  virtual void* GetGlobalSettings(){return (void*)&m_cSettings;};

  typedef struct
  {
    TLongDateFormat m_nLongDateFormat;
    bool            m_bYearWithCentury; // false-04 / true-2004
    gststring       m_sDateSeparator;
    gststring       m_sTimeSeparator;
    gststring       m_sDecimalSymbol;
    gststring       m_sDigitGroupingSymbol;
    bool            m_bUseCountOfDigitsAfterDecimal;
    int             m_nCountOfDigitsAfterDecimal;
  } DateTimeNumberSettings;

  static GS_HANDLE m_handle;
  DateTimeNumberSettings  m_cSettings;
  DateTimeNumberSettings* m_pSettings;
};




#endif // _GLOBALSETTINGS_H_
