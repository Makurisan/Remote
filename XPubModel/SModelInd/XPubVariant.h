#if !defined(AFX_XPUBVARIANT_H__D68B21F6_1D11_44D3_A5AB_1D16F4C895E8__INCLUDED_)
#define AFX_XPUBVARIANT_H__D68B21F6_1D11_44D3_A5AB_1D16F4C895E8__INCLUDED_



#include "ModelExpImp.h"
#include "SOpSys/types.h"
#include "SSTLdef/STLdef.h"
#include "ModelInd.h"
#include "ModelDef.h"
#include <time.h>
#include <assert.h>

typedef struct tagXPUBVARIANT XPUBVARIANT;



class CGS_DateTimeNumber;



#define EMPTY_DATETIME  0x8000000000000000
#define EMPTY_DATETIME_HIGH  0x80000000
#define EMPTY_DATETIME_LOW   0x00000000

typedef struct 
{
  void Reset(){nYear = 0;nMonth = 0;nDay = 0;nHour = 0;nMinute = 0;nSecond = 0;nMilli = 0;nMicro = 0;nNano = 0;nDayOfYear = 0;nDayOfWeek = 0;};

  int  nYear;
  unsigned int nMonth;
  unsigned int nDay;
  unsigned int nHour;
  unsigned int nMinute;
  unsigned int nSecond;
  unsigned int nMilli;
  unsigned int nMicro;
  unsigned int nNano;
  unsigned int nDayOfYear;
  unsigned int nDayOfWeek;
} QLHTM;


class XPUBMODEL_EXPORTIMPORT CXPubDateTime
{
public:
  INT64 m_nTime;
  
  static INT64 _MKTIME(const QLHTM *SrcTime);
  static void _LOCALTIME(QLHTM* time, INT64 nSrcTime);
  static time_t _MKTIME_1970(const QLHTM *SrcTime);
  static void _LOCALTIME_1970(QLHTM* time, time_t nSrcTime);

  xtstring TimeAsString(CGS_DateTimeNumber* pGSettings) const;
  xtstring TimeAsSQLString() const;

  void Serialize(xtstring& sVal);
  void Deserialize(const xtstring& sVal);

  //MakeTime(xtstring("07/16/2003 10:35:59"));
//  static time_t MakeTime(const xtstring& Time);
};





struct XPUBMODEL_EXPORTIMPORT tagXPUBVARIANT
{
public:
  TFieldType GetType() const {return vt;};
protected:
  xtstring sValue;
  TFieldType vt;
  union 
  {
    long          lVal;   // tFieldTypeInteger
    bool          bVal;   // tFieldTypeBoolean
    XTCHAR        cVal;   // tFieldTypeChar
    short         iVal;   // tFieldTypeSmallInt
    CXPubDateTime tmVal;  // tFieldTypeDate
    double        dblVal; // tFieldTypeDouble
  };
};



// COleVariant
class XPUBMODEL_EXPORTIMPORT CXPubVariant : public tagXPUBVARIANT
{
private:
  bool CreateGlobalSettings() const;
  void RemoveGlobalSettings();
  CGS_DateTimeNumber* m_pGSettings;
public:

  // construct / destruct
  CXPubVariant();
  //CXPubVariant(const XPUBVARIANT& varSrc);
  CXPubVariant(const CXPubVariant& varSrc);
  CXPubVariant(const xtstring& sSrc);
  CXPubVariant(const xtstring& varSrc, const TFieldType vt);
  CXPubVariant(long lSrc);
  CXPubVariant(bool bSrc);
  CXPubVariant(char cSrc);
  CXPubVariant(short iSrc);
  CXPubVariant(CXPubDateTime& dSrc);
  CXPubVariant(double dblSrc);
  virtual ~CXPubVariant();

  // xxx
  void Clear();
  bool IsEmpty() const;
  xtstring GetValue() const;

  // operators
  bool operator == (const CXPubVariant& cVar) const;
  bool operator ==(const bool& bSrc) const;
  bool operator ==(const xtstring& sSrc) const;

  const CXPubVariant& operator =(LPXTCHAR pSrc);
  const CXPubVariant& operator =(xtstring sSrc);
  const CXPubVariant& operator =(long lSrc);
  const CXPubVariant& operator =(bool bSrc);
  const CXPubVariant& operator =(XTCHAR cSrc);
  const CXPubVariant& operator =(short iSrc);
  const CXPubVariant& operator =(CXPubDateTime& dSrc);
  const CXPubVariant& operator =(double dblSrc);
  void operator =(const CXPubVariant& varSrc);

  operator xtstring() const;
  operator double() const;
  operator short() const;
  operator long() const;
  operator unsigned int() const;
  operator bool() const;
  operator CXPubDateTime() const;

  // serialize / deserialize
  void Serialize(TFieldType& type, xtstring& sVal);
  void Deserialize(TFieldType type, const xtstring& sVal);

  // SetValue
  void SetValue(const xtstring& sSrc);
  void SetValue(long lSrc);
  void SetValue(bool bSrc);
  void SetValue(XTCHAR cSrc);
  void SetValue(short iSrc);
  void SetValue(CXPubDateTime& dSrc);
  void SetValue(double dblSrc);

  // TypedValue / MaskedValue
  void SetTypedValueFromSQL(const xtstring& varSrc, const TFieldType nType/*=tFieldTypeVarChar*/);
  void SetTypedValue(const xtstring& varSrc, const TFieldType nType/*=tFieldTypeVarChar*/);
  xtstring GetTypedValueForSQL(const TFieldType nType) const;
  xtstring GetTypedValue(const TFieldType nType) const;
  xtstring GetMaskedValue(const xtstring& sMask);

  // static functions
  static TGroupOfFieldTypes GetGroupOfFieldType(TFieldType nType);
  static TFieldType GetDefaultFieldTypeOfGroup(TGroupOfFieldTypes nGroup);
  static bool IsDefaultFieldTypeOfGroup(TFieldType nType);
  static TFieldType GetDefaultFieldTypeOfDefaultGroup(){return GetDefaultFieldTypeOfGroup(tFieldTypeGroupNumber);};
  static TFieldType TranslateFieldTypeFromWIDEToANSI(TFieldType nType);

protected:

};

typedef vector<CXPubVariant*>                   CXPubVariantPtrVector;
typedef CXPubVariantPtrVector::iterator         CXPubVariantPtrIterator;
typedef CXPubVariantPtrVector::reverse_iterator CXPubVariantPtrRIterator;
typedef CXPubVariantPtrVector::const_iterator   CXPubVariantPtrCIterator;

typedef map<xtstring, CXPubVariant>     XPubVariantFieldList;
typedef XPubVariantFieldList::iterator  XPubVariantFieldListIterator;



#endif // !defined(AFX_XPUBVARIANT_H__D68B21F6_1D11_44D3_A5AB_1D16F4C895E8__INCLUDED_)

////////////////////////////////////////////////////////////////////////////////////////////////

