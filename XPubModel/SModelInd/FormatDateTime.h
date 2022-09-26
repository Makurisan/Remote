#ifndef _MYDATE_H_
#define _MYDATE_H_


#ifdef WIN32
#include <basetsd.h>
#endif // WIN32


#define _MYDATE_VERSION 2


/*
Format        Output
DD            Day as two digits (e.g.: 01)
D             Day as two or one digits (e.g.: 1)
MM            Month as two digits (e.g.: 12)
M             Month as two or one digits (e.g.: 2)
MON           Month as three characters (e.g.: DEC)
MONTH         Month as full string (e.g.: December)
YY            Year as two digits (e.g.: 03)
YYYY          Year as four digits (e.g.: 2003)
hh            Hours as two digits
h             Hours as two or one digits
mm            Minutes as two digits
m             Minutes as two or one digits
ss            Seconds as two digits
s             Seconds as two or one digits
*/


#include "ModelExpImp.h"
#include "SOpSys/types.h"
#include "SSTLdef/STLdef.h"
#include "ModelDef.h"



//namespace openutils {

class XPUBMODEL_EXPORTIMPORT CFormatDateTime
{
private:
  int m_nYear;
  int m_nMonth;
  int m_nDay;
  int m_nHour;
  int m_nMinute;
  int m_nSecond;

  int m_nOldYear;
  int m_nOldMonth;
  int m_nOldDay;
  int m_nOldHour;
  int m_nOldMinute;
  int m_nOldSecond;

public:
  // constrcutors
  CFormatDateTime();
  CFormatDateTime(INT64 time);
  CFormatDateTime(LPCXTCHAR date); // Expected DD/MM/YYYY or DD/MM/YY
  CFormatDateTime(int d, int m, int y, int hh = 0, int mm = 0, int ss = 0);

  // set value functions
  void setDate(LPCXTCHAR date); // Expected DD/MM/YYYY or DD/MM/YY
  void setYear(int y);
  void setMonth(int m);
  void setDay(int d);
  void setHour(int hh);
  void setMinute(int mm);
  void setSecond(int ss);

  // get functions
  int getYear();
  int getMonth();
  int getDay();
  int getHour();
  int getMinute();
  int getSecond();

  xtstring getShortMonth();
  xtstring getFullMonth();   
  xtstring getDayOfWeek();

  xtstring getDate(); // DD/MM/YYYY
  xtstring getTime(); // hh:mm:ss
  xtstring getFormatedDateTime(LPCXTCHAR format);

  // arithmetic functions
  void addDays(int d);
  void addMonths(int m);
  void addYears(int y);   
  void lessDays(int d);
  void lessMonths(int m);
  void lessYears(int y);

  int differenceInDays(CFormatDateTime d);
  int differenceInMonths(CFormatDateTime d);
  int differenceInYears(CFormatDateTime d);
    
  // operators
  bool operator > (CFormatDateTime d);
  bool operator < (CFormatDateTime d);
  bool operator == (CFormatDateTime d);
  bool operator != (CFormatDateTime d);
  void operator = (CFormatDateTime d);
  void operator = (LPCXTCHAR c);
    
  // utilities    
  bool isLeapYear();

private:
  void validate();
  int getDaysInMonth();
  void correctDays();
  void restore(); 
};

class CFormatDateTimeException
{
private:
  xtstring m_strErr;
public:
  CFormatDateTimeException() { m_strErr = ""; }
  CFormatDateTimeException(xtstring s) { m_strErr = s; }   
  xtstring getMessage() { return m_strErr; }
protected:
  void setMessage(xtstring s) { m_strErr = s; }
};

//} // namespace myutils

#endif // _MYDATE_H_
