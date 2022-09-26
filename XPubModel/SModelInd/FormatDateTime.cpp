// mydate.cpp
// defines a date type
// Date: 28 Aug 2003 12:04 AM
// Last Modified: 28 Aug 2003 9:43 AM
// Author: Vijay Mathew Pandyalakal

/* Copyright 2000 - 2005 Vijay Mathew Pandyalakal.  All rights reserved.
 *
 * This software may be used or modified for any purpose, personal or
 * commercial.  Open Source redistributions are permitted.  
 *
 * Redistributions qualify as "Open Source" under  one of the following terms:
 *   
 *    Redistributions are made at no charge beyond the reasonable cost of
 *    materials and delivery.
 *
 *    Redistributions are accompanied by a copy of the Source Code or by an
 *    irrevocable offer to provide a copy of the Source Code for up to three
 *    years at the cost of materials and delivery.  Such redistributions
 *    must allow further use, modification, and redistribution of the Source
 *    Code under substantially the same terms as this license.
 *
 * Redistributions of source code must retain the copyright notices as they
 * appear in each source code file, these license terms, and the
 * disclaimer/limitation of liability set forth as paragraph 6 below.
 *
 * Redistributions in binary form must reproduce this Copyright Notice,
 * these license terms, and the disclaimer/limitation of liability set
 * forth as paragraph 6 below, in the documentation and/or other materials
 * provided with the distribution.
 *
 * The Software is provided on an "AS IS" basis.  No warranty is
 * provided that the Software is free of defects, or fit for a
 * particular purpose.  
 *
 * Limitation of Liability. The Author shall not be liable
 * for any damages suffered by the Licensee or any third party resulting
 * from use of the Software.
 */


#include <time.h>

#include "SOpSys/charwchar.h"

#include "FormatDateTime.h"
#include "XPubVariant.h"

//using namespace openutils;

CFormatDateTime::CFormatDateTime()
{
  time_t now;
  time(&now); 
  struct tm* l_time;       
  l_time = localtime(&now); 

  m_nYear = l_time->tm_year + 1900;
  m_nMonth = l_time->tm_mon + 1;
  m_nDay = l_time->tm_mday;
  m_nHour = l_time->tm_hour;
  m_nMinute = l_time->tm_min;
  m_nSecond = l_time->tm_sec;

  m_nOldYear = m_nYear;
  m_nOldMonth = m_nMonth;
  m_nOldDay = m_nDay;
  m_nOldHour = m_nHour;
  m_nOldMinute = m_nMinute;
  m_nOldSecond = m_nSecond;
}

CFormatDateTime::CFormatDateTime(INT64 time)
{
  QLHTM l_time;
  CXPubDateTime::_LOCALTIME(&l_time, time);

  m_nYear = l_time.nYear/* + 1900*/;
  m_nMonth = l_time.nMonth/* + 1*/;
  m_nDay = l_time.nDay;
  m_nHour = l_time.nHour;
  m_nMinute = l_time.nMinute;
  m_nSecond = l_time.nSecond;

  m_nOldYear = m_nYear;
  m_nOldMonth = m_nMonth;
  m_nOldDay = m_nDay;
  m_nOldHour = m_nHour;
  m_nOldMinute = m_nMinute;
  m_nOldSecond = m_nSecond;
}

CFormatDateTime::CFormatDateTime(LPCXTCHAR strd)
{
  CFormatDateTime dt;
  char* date = new char[_tcslen(strd) + 1];
  _tcscpy(date, strd);
  m_nOldYear = dt.m_nYear;
  m_nOldMonth = dt.m_nMonth;
  m_nOldDay = dt.m_nDay;  
  size_t len = _tcslen(date);
  if (len < 7 || len > 10)
  {
    delete[] date;
    throw new CFormatDateTimeException("Invalid date format. Expected DD/MM/YYYY or DD/MM/YY");
  }
  bool day_found = false;
  XTCHAR buff[11];
  int j = 0;
  int d = 0, m = 0, y = 0;
  for (size_t i = 0; i < len; i++)
  {
    XTCHAR c = date[i];   
    if (c == _XT('/'))
    {
      if (!day_found)
      {
        buff[j] = _XT('\0');
        d = _ttoi(buff);
        _tcscpy(buff, _XT(""));
        day_found = true;
        j = 0;
      }
      else
      {
        buff[j] = _XT('\0');
        m = _ttoi(buff);
        _tcscpy(buff, _XT(""));
        j = 0;
      }
    }
    else
    {
      buff[j] = c;
      j++;
    }
  }
  buff[j] = _XT('\0');
  size_t yl = _tcslen(buff);
  if (yl < 1 && yl > 4)
  {
    delete[] date;
    throw new CFormatDateTimeException("Invalid year");
  }
  if (yl == 2)
  {
    if (buff[0] == _XT('0'))
    {
      XTCHAR tmp[5];
      sprintf(tmp, "20%s", buff);
      _tcscpy(buff, tmp);
    }
  }
  y = atoi(buff);
  m_nYear = y;
  m_nMonth = m;
  m_nDay = d; 
  delete[] date;
  validate();
}

CFormatDateTime::CFormatDateTime(int d, int m, int y, int hh /*= 0*/, int mm /*= 0*/, int ss /*= 0*/)
{
  CFormatDateTime dt;

  m_nOldYear = dt.m_nYear;
  m_nOldMonth = dt.m_nMonth;
  m_nOldDay = dt.m_nDay;
  m_nOldHour = dt.m_nHour;
  m_nOldMinute = dt.m_nMinute;
  m_nOldSecond = dt.m_nSecond;

  m_nYear = y;
  m_nMonth = m;
  m_nDay = d;
  m_nHour = hh;
  m_nMinute = mm;
  m_nSecond = ss;

  validate();
}

void CFormatDateTime::setDate(LPCXTCHAR date)
{
  m_nOldYear = m_nYear;
  m_nOldMonth = m_nMonth;
  m_nOldDay = m_nDay;
  m_nOldHour = m_nHour;
  m_nOldMinute = m_nMinute;
  m_nOldSecond = m_nSecond;

  CFormatDateTime sd(date);
  *this = sd;
  validate();
}

void CFormatDateTime::setYear(int y)
{
  m_nOldYear = m_nYear;
  m_nYear = y;
  validate();
}

void CFormatDateTime::setDay(int d)
{
  m_nOldDay = m_nDay;
  m_nDay = d;
  validate();
}

void CFormatDateTime::setMonth(int m)
{
  m_nOldMonth = m_nMonth;
  m_nMonth = m;
  validate();
}

void CFormatDateTime::setHour(int hh)
{
  m_nOldHour = m_nHour;
  m_nHour = hh;
  validate();
}

void CFormatDateTime::setMinute(int mm)
{
  m_nOldMinute = m_nMinute;
  m_nMinute = mm;
  validate();
}

void CFormatDateTime::setSecond(int ss)
{
  m_nOldSecond = m_nSecond;
  m_nSecond = ss;
  validate();
}

int CFormatDateTime::getYear()
{
  return m_nYear;
}

int CFormatDateTime::getMonth()
{
  return m_nMonth;
}

int CFormatDateTime::getDay()
{
  return m_nDay;
}

int CFormatDateTime::getHour()
{
  return m_nHour;
}

int CFormatDateTime::getMinute()
{
  return m_nMinute;
}

int CFormatDateTime::getSecond()
{
  return m_nSecond;
}

xtstring CFormatDateTime::getShortMonth()
{
  xtstring ret;
  if (m_nMonth == 1)
    ret = _XT("JAN");
  else if (m_nMonth == 2)
    ret = _XT("FEB");
  else if (m_nMonth == 3)
    ret = _XT("MÄR");
  else if (m_nMonth == 4)
    ret = _XT("APR");
  else if (m_nMonth == 5)
    ret = _XT("MAY");
  else if (m_nMonth == 6)
    ret = _XT("JUN");
  else if (m_nMonth == 7)
    ret = _XT("JUL");
  else if (m_nMonth == 8)
    ret = _XT("AUG");
  else if (m_nMonth == 9)
    ret = _XT("SEP");
  else if (m_nMonth == 10)
    ret = _XT("OKT");
  else if (m_nMonth == 11)
    ret = _XT("NOV");
  else if (m_nMonth == 12)
    ret = _XT("DEZ");

  return ret;
}

/*
xtstring CFormatDateTime::getShortMonth()
{
  xtstring ret;
  if (m_nMonth == 1)
    ret = _XT("JAN");
  else if (m_nMonth == 2)
    ret = _XT("FEB");
  else if (m_nMonth == 3)
    ret = _XT("MAR");
  else if (m_nMonth == 4)
    ret = _XT("APR");
  else if (m_nMonth == 5)
    ret = _XT("MAY");
  else if (m_nMonth == 6)
    ret = _XT("JUN");
  else if (m_nMonth == 7)
    ret = _XT("JUL");
  else if (m_nMonth == 8)
    ret = _XT("AUG");
  else if (m_nMonth == 9)
    ret = _XT("SEP");
  else if (m_nMonth == 10)
    ret = _XT("OCT");
  else if (m_nMonth == 11)
    ret = _XT("NOV");
  else if (m_nMonth == 12)
    ret = _XT("DEC");

  return ret;
}
*/

xtstring CFormatDateTime::getFullMonth()
{
  xtstring ret;
  if (m_nMonth == 1)
    ret = _XT("Januar");
  else if (m_nMonth == 2)
    ret = _XT("Februar");
  else if (m_nMonth == 3)
    ret = _XT("März");
  else if (m_nMonth == 4)
    ret = _XT("April");
  else if (m_nMonth == 5)
    ret = _XT("Mai");
  else if (m_nMonth == 6)
    ret = _XT("Juni");
  else if (m_nMonth == 7)
    ret = _XT("Juli");
  else if (m_nMonth == 8)
    ret = _XT("August");
  else if (m_nMonth == 9)
    ret = _XT("September");
  else if (m_nMonth == 10)
    ret = _XT("Oktober");
  else if (m_nMonth == 11)
    ret = _XT("November");
  else if (m_nMonth == 12)
    ret = _XT("Dezember");

  return ret;
}
/*
xtstring CFormatDateTime::getFullMonth()
{
  xtstring ret;
  if (m_nMonth == 1)
    ret = _XT("January");
  else if (m_nMonth == 2)
    ret = _XT("February");
  else if (m_nMonth == 3)
    ret = _XT("March");
  else if (m_nMonth == 4)
    ret = _XT("April");
  else if (m_nMonth == 5)
    ret = _XT("May");
  else if (m_nMonth == 6)
    ret = _XT("June");
  else if (m_nMonth == 7)
    ret = _XT("July");
  else if (m_nMonth == 8)
    ret = _XT("August");
  else if (m_nMonth == 9)
    ret = _XT("September");
  else if (m_nMonth == 10)
    ret = _XT("October");
  else if (m_nMonth == 11)
    ret = _XT("November");
  else if (m_nMonth == 12)
    ret = _XT("December");

  return ret;
}
*/

xtstring CFormatDateTime::getDayOfWeek()
{
  xtstring ret;
  QLHTM qlhtm;

  qlhtm.Reset();
  qlhtm.nYear = m_nYear;
  qlhtm.nMonth = m_nMonth;
  qlhtm.nDay = m_nDay;
  qlhtm.nHour = m_nHour;
  qlhtm.nMinute = m_nMinute;
  qlhtm.nSecond = m_nSecond;

  INT64 time = CXPubDateTime::_MKTIME(&qlhtm);
  CXPubDateTime::_LOCALTIME(&qlhtm, time);

  // QLHTM berechnte Sonntag als erstem Tag in Woche.
  switch (qlhtm.nDayOfWeek)
  {
  case 2:
    ret = _XT("Montag");      break;
  case 3:
    ret = _XT("Dienstag");    break;
  case 4:
    ret = _XT("Mittwoch");    break;
  case 5:
    ret = _XT("Donnerstag");  break;
  case 6:
    ret = _XT("Freitag");     break;
  case 7:
    ret = _XT("Samstag");     break;
  case 1:
  default:
    ret = _XT("Sonntag");     break;
  }

  return ret;
}

xtstring CFormatDateTime::getDate()
{
  xtstring s;
  s.Format(_XT("%02ld/%02ld/%02ld"), m_nDay, m_nMonth, m_nYear);
  return s;
}

xtstring CFormatDateTime::getTime()
{
  xtstring s;
  s.Format(_XT("%02ld:%02ld:%02ld"), m_nHour, m_nMinute, m_nSecond);
  return s;
}

xtstring CFormatDateTime::getFormatedDateTime(LPCXTCHAR format)
{
  xtstring sFormat(format);

#define COUNT_OF_TOKENS   15
  xtstring sReplace;
  xtstring sTokens[COUNT_OF_TOKENS][2];
  size_t nPos[COUNT_OF_TOKENS];

  //
  sTokens[0][0] = _XT("YYYY");
  sReplace.Format(_XT("%04ld"), getYear());
  sTokens[0][1] = sReplace;
  //
  sTokens[1][0] = _XT("YY");
  sReplace.erase(sReplace.begin(), sReplace.begin() + 2);
  sTokens[1][1] = sReplace;
  //
  sTokens[2][0] = _XT("MONTH");
  sReplace = getFullMonth();
  sTokens[2][1] = sReplace;
  //
  sTokens[3][0] = _XT("MON");
  sReplace = getShortMonth();
  sTokens[3][1] = sReplace;
  //
  sTokens[4][0] = _XT("MM");
  sReplace.Format(_XT("%02ld"), getMonth());
  sTokens[4][1] = sReplace;
  //
  sTokens[5][0] = _XT("M");
  sReplace.Format(_XT("%ld"), getMonth());
  sTokens[5][1] = sReplace;
  //
  sTokens[6][0] = _XT("DAY");
  sReplace = getDayOfWeek();
  sTokens[6][1] = sReplace;
  //
  sTokens[7][0] = _XT("DD");
  sReplace.Format(_XT("%02ld"), getDay());
  sTokens[7][1] = sReplace;
  //
  sTokens[8][0] = _XT("D");
  sReplace.Format(_XT("%ld"), getDay());
  sTokens[8][1] = sReplace;
  //
  sTokens[9][0] = _XT("hh");
  sReplace.Format(_XT("%02ld"), getHour());
  sTokens[9][1] = sReplace;
  //
  sTokens[10][0] = _XT("h");
  sReplace.Format(_XT("%ld"), getHour());
  sTokens[10][1] = sReplace;
  //
  sTokens[11][0] = _XT("mm");
  sReplace.Format(_XT("%02ld"), getMinute());
  sTokens[11][1] = sReplace;
  //
  sTokens[12][0] = _XT("m");
  sReplace.Format(_XT("%ld"), getMinute());
  sTokens[12][1] = sReplace;
  //
  sTokens[13][0] = _XT("ss");
  sReplace.Format(_XT("%02ld"), getSecond());
  sTokens[13][1] = sReplace;
  //
  sTokens[14][0] = _XT("s");
  sReplace.Format(_XT("%ld"), getSecond());
  sTokens[14][1] = sReplace;

  size_t nStartPos = 0;
  for (;;)
  {
    for (size_t nI = 0; nI < COUNT_OF_TOKENS; nI++)
      nPos[nI] = sFormat.find(sTokens[nI][0], nStartPos);

    size_t nMinimumVal, nMinimumIndex;
    nMinimumVal = nPos[0];
    nMinimumIndex = 0;
    for (size_t nI = 1; nI < COUNT_OF_TOKENS; nI++)
    {
      if (nPos[nI] < nMinimumVal)
      {
        nMinimumVal = nPos[nI];
        nMinimumIndex = nI;
      }
    }
    if (nMinimumVal == xtstring::npos)
      break;
    sFormat.replace(nPos[nMinimumIndex],
                    sTokens[nMinimumIndex][0].size(),
                    sTokens[nMinimumIndex][1].c_str());
    nStartPos = nMinimumVal + sTokens[nMinimumIndex][1].size();
  }
  return sFormat;
/*
  vector<size_t> vctDDPos;
  vector<size_t> vctMMPos;
  vector<size_t> vctMONPos;
  vector<size_t> vctMonthPos;
  vector<size_t> vctYYPos;
  vector<size_t> vctYYYYPos;

  xtstring arg = format;
  size_t pos = 0;
  size_t curr_pos = 0;

  while(true)
  {
    pos = arg.find(_XT("DD"), curr_pos);
    if (pos == -1)
      break;
    vctDDPos.push_back(pos);
    curr_pos = pos + 2;
  }
  pos = 0;
  curr_pos = 0;
  while(true)
  {
    pos = arg.find(_XT("MM"), curr_pos);
    if (pos == -1)
      break;
    vctMMPos.push_back(pos);
    curr_pos = pos + 2;
  }
  pos = 0;
  curr_pos = 0;
  while (true)
  {
    pos = arg.find(_XT("MON"), curr_pos);
    if (pos == -1)
      break;
    vctMONPos.push_back(pos);
    curr_pos = pos + 3;
  }
  pos = 0;
  curr_pos = 0;
  while (true)
  {
    pos = arg.find(_XT("MONTH"), curr_pos);
    if (pos == -1)
      break;
    vctMonthPos.push_back(pos);
    curr_pos = pos + 5;
  }
  pos = 0;
  curr_pos = 0;
  while (true)
  {
    pos = arg.find(_XT("YY"), curr_pos);
    if (pos == -1)
      break;
    vctYYPos.push_back(pos);
    curr_pos = pos + 2;
  }
  pos = 0;
  curr_pos = 0;
  while (true)
  {
    pos = arg.find(_XT("YYYY"), curr_pos);
    if (pos == -1)
      break;
    vctYYYYPos.push_back(pos);
    curr_pos = pos + 4;
  }
  LPXTCHAR buff = new XTCHAR[arg.size() + vctMonthPos.size() + 51];
  _tcscpy(buff, arg.c_str());
  size_t i;
  XTCHAR strDD[3];
  sprintf(strDD, "%02d", getDay());
  for (i = 0; i < vctDDPos.size(); i++)
  {
    size_t j;
    int cpos = vctDDPos[i];
    int count = 0;
    for (j = 0; j < arg.size(); j++)
    {
      if (j == cpos)
      {
        buff[j] = strDD[0];
        j++;
        buff[j] = strDD[1];       
        continue;
      }
      buff[j] = buff[j];
    }
    buff[j] = '\0';
  }
  XTCHAR strMM[3];
  sprintf(strMM, "%02d", getMonth());
  for (i = 0; i < vctMMPos.size(); i++)
  {
    size_t j;
    int cpos = vctMMPos[i];
    int count = 0;
    for (j=0;j<arg.size();j++)
    {
      if (j == cpos)
      {
        buff[j] = strMM[0];
        j++;
        buff[j] = strMM[1];       
        continue;
      }
      buff[j] = buff[j];
    }
    buff[j] = '\0';
  }
  XTCHAR strMon[4];
  xtstring sm = getShortMonth();
  strcpy(strMon, sm.c_str());
  for (i = 0; i < vctMONPos.size(); i++)
  {
    size_t j;
    int cpos = vctMONPos[i];
    int count = 0;
    for (j = 0; j < arg.size(); j++)
    {
      if (j == cpos)
      {
        buff[j] = strMon[0];
        j++;
        buff[j] = strMon[1];        
        j++;
        buff[j] = strMon[2];
        continue;
      }
      buff[j] = buff[j];
    }
    buff[j] = _XT('\0');
  }
  XTCHAR strYear[5];
  sprintf(strYear, "%02d", getYear());
  for (i = 0; i < vctYYPos.size(); i++)
  {
    size_t j;
    int cpos = vctYYPos[i];
    int count = 0;
    for (j = 0; j < arg.size(); j++)
    {
      if (j == cpos)
      {
        buff[j] = strYear[2];
        j++;
        buff[j] = strYear[3];       
        continue;
      }
      buff[j] = buff[j];
    }
    buff[j] = _XT('\0');
  }
  sprintf(strYear, "%04d", getYear());
  for (i = 0; i < vctYYYYPos.size(); i++)
  {
    size_t j;
    int cpos = vctYYYYPos[i];
    int count = 0;
    for (j = 0; j < arg.size(); j++)
    {
      if (j == cpos)
      {
        buff[j] = strYear[0];
        j++;
        buff[j] = strYear[1];
        j++;
        buff[j] = strYear[2];
        j++;
        buff[j] = strYear[3];       
        continue;
      }
      buff[j] = buff[j];
    }
    buff[j] = _XT('\0');
  }
  XTCHAR strFullMon[16];
  xtstring sFM = getFullMonth();
  _tcsncpy(strFullMon, sFM.c_str(), 16);
  strFullMon[15] = 0;
  for (i = 0; i < vctMonthPos.size(); i++)
  {
    size_t j;
    int cpos = vctMonthPos[i];
    int count = 0;
    LPXTCHAR tmp = new char[arg.size() + vctMonthPos.size() + 51];
    _tcscpy(tmp, buff);
    size_t sz = arg.size();
    int c = 0;
    int idx = 0;
    for (j = 0; j < sz; j++)
    {
      if (j == cpos)
      {
        for (size_t k = 0; k < _tcslen(strFullMon); k++)
        {
          buff[idx] = strFullMon[k];
          idx++;
        } 
//        int diff = _tcslen(strFullMon) - 5;
//        if (diff > 0)
//          sz += diff;
        if (_tcslen(strFullMon) > 5)
          sz += (_tcslen(strFullMon) - 5);
        c += 5;
        //j--;
        continue;
      }
      buff[idx] = tmp[c];
      c++;
      idx++;
    }
    buff[idx] = _XT('\0');
    delete[] tmp;
  }
  xtstring ret = buff;
  delete[] buff;
  return ret;
*/
}

void CFormatDateTime::addDays(int days)
{
  m_nOldDay = m_nDay;    
  for (int i = 0; i < days; i++)
  {
    m_nDay++;
    int d = getDaysInMonth();
    if (m_nDay > d)
    {
      m_nDay = 1;
      m_nMonth++;
      if (m_nMonth > 12)
      {
        m_nMonth = 1;
        m_nYear++;      
      }
    } 
  }
  correctDays();
}

void CFormatDateTime::addMonths(int mons)
{
  m_nOldMonth = m_nMonth;
  for (int i = 0; i < mons; i++)
  {
    if (m_nMonth > 12)
    {
      m_nMonth = 1;
      m_nYear++;        
    }
    m_nMonth++;
  }
  correctDays();
}

void CFormatDateTime::addYears(int yrs)
{
  m_nOldYear = m_nYear;
  m_nYear += yrs; 
  correctDays();
}

void CFormatDateTime::lessDays(int days)
{
  m_nOldDay = m_nDay;    
  for (int i = 0; i < days; i++)
  {
    m_nDay--;   
    if (m_nDay <= 0)
    {
      m_nMonth--;
      int d = getDaysInMonth();
      m_nDay = d;
      if (m_nMonth <= 0)
      {
        m_nMonth = 12;
        m_nYear--;      
      }
    } 
  }
  correctDays();
}

void CFormatDateTime::lessMonths(int mons)
{
  m_nOldMonth = m_nMonth;
  for (int i = 0; i < mons; i++)
  {
    if (m_nMonth <= 0)
    {
      m_nMonth = 12;
      m_nYear--;        
    }
    m_nMonth--;
  }
  correctDays();
}

void CFormatDateTime::lessYears(int yrs)
{
  m_nOldYear = m_nYear; 
  m_nYear -= yrs;
  if (m_nYear < 0)
    m_nYear *= -1;
  correctDays();
  validate();
}

int CFormatDateTime::differenceInDays(CFormatDateTime date)
{
  int diff = 0;  
  CFormatDateTime tmp_this = *this;
  CFormatDateTime tmp_date = date;
  if (*this > date)
  {
    while (true)
    {
      if (*this == tmp_date)
        break;
      tmp_date.addDays(1);
      diff++;
    }
  }
  else if(*this < date)
  {
    while (true)
    {
      if (tmp_this == tmp_date)
        break;
      tmp_this.addDays(1);
      diff++;
    }
  }
  else
  {
    diff = 0;
  }
  return diff;
}

int CFormatDateTime::differenceInMonths(CFormatDateTime date)
{
  int diff = 0;    
  CFormatDateTime tmp_this = *this;
  CFormatDateTime tmp_date = date;
  if (*this > date)
  {
    while (true)
    {
      if (tmp_this.m_nYear == tmp_date.m_nYear)
      {
        if (tmp_this.m_nMonth == tmp_date.m_nMonth)
          break;
      }
      tmp_date.addMonths(1);
      diff++;
    }
  }
  else if (*this < date)
  {
    while (true)
    {
      if (tmp_this.m_nYear == tmp_date.m_nYear)
      {
        if (tmp_this.m_nMonth == tmp_date.m_nMonth)
          break;
      }
      tmp_this.addMonths(1);
      diff++;
    }
  }
  else
  {
    diff = 0;
  }
  return diff;
}

int CFormatDateTime::differenceInYears(CFormatDateTime date)
{
  int ret = m_nYear - date.getYear();
  if (ret < 0)
    ret *= -1;
  return ret;
}

bool CFormatDateTime::operator > (CFormatDateTime date)
{
  if (date.getYear() < m_nYear)
  {
    return true;
  }
  else if (date.getYear() == m_nYear)
  {
    if (date.getMonth() < m_nMonth)
    {
      return true;
    }
    else if (date.getMonth() == getMonth())
    {
      if (date.getDay() < m_nDay)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
  return false;
}

bool CFormatDateTime::operator < (CFormatDateTime date)
{
  if (date.getYear() > m_nYear)
  {
    return true;
  }
  else if (date.getYear() == m_nYear)
  {
    if (date.getMonth() > m_nMonth)
    {
      return true;
    }
    else if (date.getMonth() == getMonth())
    {
      if (date.getDay() > m_nDay)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
  return false;
}

bool CFormatDateTime::operator == (CFormatDateTime date)
{
  if (date.getYear() == getYear()
      && date.getMonth() == getMonth()
      && date.getDay() == getDay())
  {
    return true;
  }
  return false;
}

bool CFormatDateTime::operator != (CFormatDateTime date)
{
  if (date.getYear() != getYear()
    || date.getMonth() != getMonth()
    || date.getDay() != getDay())
  {
    return true;
  }
  return false;
}

void CFormatDateTime::operator = (CFormatDateTime date)
{
  m_nOldYear = m_nYear;
  m_nOldMonth = m_nMonth;
  m_nOldDay = m_nDay;
  m_nYear = date.getYear();
  m_nMonth = date.getMonth();
  m_nDay = date.getDay();
  validate();
}

void CFormatDateTime::operator = (LPCXTCHAR date)
{
  setDate(date);
}

void CFormatDateTime::validate()
{
  if (m_nYear <= 0)
  {
    char buff[51];
    sprintf(buff, "Not a valid year: %d", m_nYear);
    restore();
    throw new CFormatDateTimeException(buff);
  }
  if (m_nMonth < 1 || m_nMonth > 12)
  {
    char buff[51];
    sprintf(buff,"Not a valid month: %d",m_nMonth);
    restore();
    throw new CFormatDateTimeException(buff);
  }
  if (m_nDay < 1 || m_nDay > 31)
  {
    char buff[51];
    sprintf(buff,"Not a valid day: %d",m_nDay);
    restore();
    throw new CFormatDateTimeException(buff);
  }
  if (m_nDay > getDaysInMonth())
  {
    char buff[51];
    sprintf(buff,"Not a valid day: %d",m_nDay);
    restore();
    throw new CFormatDateTimeException(buff);
  }    
}

int CFormatDateTime::getDaysInMonth()
{
  if (m_nMonth == 1)
  {
    return 31;
  }
  else if (m_nMonth == 2)
  {
    if (isLeapYear())
      return 29;
    else
      return 28;   
  }
  else if (m_nMonth == 3)
  {
    return 31;
  }
  else if (m_nMonth == 4)
  {
    return 30;
  }
  else if (m_nMonth == 5)
  {
    return 31;
  }
  else if (m_nMonth == 6)
  {
    return 30;
  }
  else if (m_nMonth == 7)
  {
    return 31;
  }
  else if (m_nMonth == 8)
  {
    return 31;
  }
  else if (m_nMonth == 9)
  {
    return 30;
  }
  else if (m_nMonth == 10)
  {
    return 31;
  }
  else if (m_nMonth == 11)
  {
    return 30;
  }
  else if (m_nMonth == 12)
  {
    return 31;
  }
  return -1;
}

bool CFormatDateTime::isLeapYear()
{
  if (m_nYear <= 1752)
  {
    if (m_nYear % 4 == 0)
      return true;
  }
  else
  {
    if ((m_nYear % 4 == 0 && m_nYear % 100 != 0)
        || m_nYear % 400 == 0)
    {
      return true;
    }
  }
  return false;
}

void CFormatDateTime::correctDays()
{
  int days = getDaysInMonth();
  if (m_nDay > days)
  {
    m_nDay = days;
  }
}

void CFormatDateTime::restore()
{
  m_nYear = m_nOldYear;
  m_nMonth = m_nOldMonth;
  m_nDay = m_nOldDay;
}
