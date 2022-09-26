#if !defined(_STNSUPPORT_H_)
#define _STNSUPPORT_H_


#include "ModelExpImp.h"
#include "ModelDef.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32



class XPUBMODEL_EXPORTIMPORT CDTNSupport
{
public:
  CDTNSupport();
  ~CDTNSupport();

  void operator =(const CDTNSupport& cSupp);

  TLongDateFormat GetLongDateFormat() const {return m_nLongDateFormat;};
  void SetLongDateFormat(TLongDateFormat nLongDateFormat){m_nLongDateFormat = nLongDateFormat;};
  bool GetYearWithCentury() const {return m_bYearWithCentury;};
  void SetYearWithCentury(bool bYearWithCentury){m_bYearWithCentury = bYearWithCentury;};
  xtstring GetDateSeparator() const {return m_sDateSeparator;};
  void SetDateSeparator(const xtstring& sDateSeparator){m_sDateSeparator = sDateSeparator;};
  xtstring GetTimeSeparator() const {return m_sTimeSeparator;};
  void SetTimeSeparator(const xtstring& sTimeSeparator){m_sTimeSeparator = sTimeSeparator;};
  bool GetUseSymbolsFromSystem() const {return m_bUseSymbolsFromSystem;};
  void SetUseSymbolsFromSystem(bool bUseSymbolsFromSystem){m_bUseSymbolsFromSystem = bUseSymbolsFromSystem;};
  xtstring GetDecimalSymbol() const {return m_sDecimalSymbol;};
  void SetDecimalSymbol(const xtstring& sDecimalSymbol){m_sDecimalSymbol = sDecimalSymbol;};
  xtstring GetDigitGroupingSymbol() const {return m_sDigitGroupingSymbol;};
  void SetDigitGroupingSymbol(const xtstring& sDigitGroupingSymbol){m_sDigitGroupingSymbol = sDigitGroupingSymbol;};
  bool GetUseCountOfDigitsAfterDecimal() const {return m_bUseCountOfDigitsAfterDecimal;};
  void SetUseCountOfDigitsAfterDecimal(bool bUseCountOfDigitsAfterDecimal){m_bUseCountOfDigitsAfterDecimal = bUseCountOfDigitsAfterDecimal;};
  int GetCountOfDigitsAfterDecimal() const {return m_nCountOfDigitsAfterDecimal;};
  void SetCountOfDigitsAfterDecimal(int nCountOfDigitsAfterDecimal){m_nCountOfDigitsAfterDecimal = nCountOfDigitsAfterDecimal;};

private:
  TLongDateFormat m_nLongDateFormat;
  bool            m_bYearWithCentury; // false-04 / true-2004
  xtstring        m_sDateSeparator;
  xtstring        m_sTimeSeparator;
  bool            m_bUseSymbolsFromSystem;
  xtstring        m_sDecimalSymbol;
  xtstring        m_sDigitGroupingSymbol;
  bool            m_bUseCountOfDigitsAfterDecimal;
  int             m_nCountOfDigitsAfterDecimal;
};



#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_STNSUPPORT_H_)
