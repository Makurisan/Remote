

#include "PDFUtil.h"


using namespace std;

CPDFColor::CPDFColor()
  :m_strColor(""), m_strColorWithNonStrokingOperator(""), m_strColorWithStrokingOperator("")
{
  m_eColorType = eNoColor;
  m_strColor.assign("");
  m_strColorWithNonStrokingOperator.assign("");
  m_strColorWithStrokingOperator.assign("");
}

CPDFColor::CPDFColor(float i_fR, float i_fG, float i_fB)
{
  m_eColorType = eRgbColor;
  m_RGB.fR = i_fR;
  m_RGB.fG = i_fG;
  m_RGB.fB = i_fB;
  char czTemp[300];
  sprintf(czTemp, "%.5f %.5f %.5f", m_RGB.fR, m_RGB.fG, m_RGB.fB);
  m_strColor.assign(czTemp);
  m_strColorWithNonStrokingOperator.assign(czTemp);
  m_strColorWithNonStrokingOperator.append(" rg ");
  m_strColorWithStrokingOperator.assign(czTemp);
  m_strColorWithStrokingOperator.append(" RG ");
}

CPDFColor::CPDFColor(float i_fC, float i_fM, float i_fY, float i_fK)
{
  m_eColorType = eCmykColor;
  m_CMYK.fC = i_fC;
  m_CMYK.fM = i_fM;
  m_CMYK.fY = i_fY;
  m_CMYK.fK = i_fK;
  char czTemp[300];
  sprintf(czTemp, "%.5f %.5f %.5f %.5f", m_CMYK.fC, m_CMYK.fM, m_CMYK.fY, m_CMYK.fK);
  m_strColor.assign(czTemp);
  m_strColorWithNonStrokingOperator.assign(czTemp);
  m_strColorWithNonStrokingOperator.append(" k ");
  m_strColorWithStrokingOperator.assign(czTemp);
  m_strColorWithStrokingOperator.append(" K ");
}

CPDFColor::CPDFColor(float i_fGray)
{
  m_eColorType = eGrayColor;
  m_fGray = i_fGray;
  char czTemp[300];
  sprintf(czTemp, "%.5f", m_fGray);
  m_strColor.assign(czTemp);
  m_strColorWithNonStrokingOperator.assign(czTemp);
  m_strColorWithNonStrokingOperator.append(" g ");
  m_strColorWithStrokingOperator.assign(czTemp);
  m_strColorWithStrokingOperator.append(" G ");
}

CPDFColor::~CPDFColor()
{
}

void CPDFColor::operator=(const CPDFColor& r)
{
  m_eColorType = r.m_eColorType;
  m_CMYK = r.m_CMYK;
  m_strColor.assign(r.m_strColor);
  m_strColorWithNonStrokingOperator.assign(r.m_strColorWithNonStrokingOperator);
  m_strColorWithStrokingOperator.assign(r.m_strColorWithStrokingOperator);
}

void CPDFColor::SetColor(float i_fR, float i_fG, float i_fB)
{
  m_eColorType = eRgbColor;
  m_RGB.fR = i_fR;
  m_RGB.fG = i_fG;
  m_RGB.fB = i_fB;
  char czTemp[300];
  sprintf(czTemp, "%.5f %.5f %.5f", m_RGB.fR, m_RGB.fG, m_RGB.fB);
  m_strColor.assign(czTemp);
  m_strColorWithNonStrokingOperator.assign(czTemp);
  m_strColorWithNonStrokingOperator.append(" rg ");
  m_strColorWithStrokingOperator.assign(czTemp);
  m_strColorWithStrokingOperator.append(" RG ");
}

void CPDFColor::SetColor(float i_fC, float i_fM, float i_fY, float i_fK)
{
  m_eColorType = eCmykColor;
  m_CMYK.fC = i_fC;
  m_CMYK.fM = i_fM;
  m_CMYK.fY = i_fY;
  m_CMYK.fK = i_fK;
  char czTemp[300];
  sprintf(czTemp, "%.5f %.5f %.5f %.5f", m_CMYK.fC, m_CMYK.fM, m_CMYK.fY, m_CMYK.fK);
  m_strColor.assign(czTemp);
  m_strColorWithNonStrokingOperator.assign(czTemp);
  m_strColorWithNonStrokingOperator.append(" k ");
  m_strColorWithStrokingOperator.assign(czTemp);
  m_strColorWithStrokingOperator.append(" K ");
}

void CPDFColor::SetColor(float i_fGray)
{
  m_eColorType = eGrayColor;
  m_fGray = i_fGray;
  char czTemp[300];
  sprintf(czTemp, "%.5f", m_fGray);
  m_strColor.assign(czTemp);
  m_strColorWithNonStrokingOperator.assign(czTemp);
  m_strColorWithNonStrokingOperator.append(" g ");
  m_strColorWithStrokingOperator.assign(czTemp);
  m_strColorWithStrokingOperator.append(" G ");
}

const char *CPDFColor::GetColorString()
{
  return m_strColor.c_str();
}

const char *CPDFColor::GetColorStringWithNonStrokingOperator()
{
  return m_strColorWithNonStrokingOperator.c_str();
}

const char *CPDFColor::GetColorStringWithStrokingOperator()
{
  return m_strColorWithStrokingOperator.c_str();
}

//bool_a CPDFColor::IsValid()
//{
//  return m_eColorType != eNoColor;
//}

CPDFColor::eColorType CPDFColor::GetColorType()
{
  return m_eColorType;
}

void CPDFColor::Invalidate()
{
  m_eColorType = eNoColor;
  m_strColor.assign("");
  m_strColorWithNonStrokingOperator.assign("");
  m_strColorWithStrokingOperator.assign("");
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//CPDFString
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//CPDFString::CPDFString()
//{
//  m_pWstr = NULL;
//  m_eStringType = CPDFChar::eUndefinedString;
//}
//
CPDFString::CPDFString(std::wstring &i_wstr)
{
  m_eStringType = CPDFChar::eWideString;
  m_pWstr = new wstring(i_wstr);
}

CPDFString::CPDFString(const wchar_t *i_pWstr)
{
  m_pWstr = NULL;
  if (i_pWstr)
  {
    m_eStringType = CPDFChar::eWideString;
    m_pWstr = new wstring(i_pWstr);
  } else
    m_eStringType = CPDFChar::eUndefinedString;
}

CPDFString::CPDFString(const wchar_t *i_pWstr, size_t size)
{
  m_pWstr = NULL;
  if (i_pWstr)
  {
    m_eStringType = CPDFChar::eWideString;
    m_pWstr = new wstring(i_pWstr, size);
  } else
    m_eStringType = CPDFChar::eUndefinedString;
}

CPDFString::CPDFString(std::string &i_str)
{
  m_eStringType = CPDFChar::eMultibyteString;
  m_pStr = new string(i_str);
}

CPDFString::CPDFString(const char *i_pStr)
{
  m_pStr = NULL;
  if (i_pStr)
  {
    m_eStringType = CPDFChar::eMultibyteString;
    m_pStr = new string(i_pStr);
  } else
    m_eStringType = CPDFChar::eUndefinedString;
}

CPDFString::CPDFString(const char *i_pStr, size_t size)
{
  m_pStr = NULL;
  if (i_pStr)
  {
    m_eStringType = CPDFChar::eMultibyteString;
    m_pStr = new string(i_pStr, size);
  } else
    m_eStringType = CPDFChar::eUndefinedString;
}

CPDFString::CPDFString(CPDFString& rPDFString)
{
  m_pStr = NULL;
  SetString(rPDFString);
}

CPDFString::~CPDFString()
{
  Clean();
}

void CPDFString::Clean()
{
  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    delete m_pStr;
    m_pStr = NULL;
    break;
  case CPDFChar::eWideString:
    delete m_pWstr;
    m_pWstr = NULL;
    break;
  default:
    break;
  }
  m_eStringType = CPDFChar::eUndefinedString;
}

void CPDFString::operator=(const CPDFString& r)
{
  Clean();
  m_eStringType = r.m_eStringType;
  switch (r.m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    if (r.m_pStr)
      m_pStr = new string(*(r.m_pStr));
    break;
  case CPDFChar::eWideString:
    if (r.m_pWstr)
      m_pWstr = new wstring(*(r.m_pWstr));
    break;
  default:
    break;
  }
}

void CPDFString::SetString(CPDFString& rPDFString)
{
  *this = rPDFString;
}

bool_a CPDFString::SubString(CPDFString& o_Str, size_t Off, size_t Count)
{
  o_Str.Clean();
  if (Off < 0 || Off >= Length() || (Off + Count) > Length())
    return false_a;

  o_Str.m_eStringType = m_eStringType;
  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
      o_Str.m_pStr = new string(*(m_pStr), Off, Count);
    break;
  case CPDFChar::eWideString:
      o_Str.m_pWstr = new wstring(*(m_pWstr), Off, Count);
    break;
  default:
    return false_a;
    break;
  }
  return true_a;
}

bool_a CPDFString::Erase(size_t Off, size_t Count)
{
  if (Off < 0 || Off >= Length() || (Off + Count) > Length())
    return false_a;

  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
      m_pStr->erase(Off, Count);
    break;
  case CPDFChar::eWideString:
      m_pWstr->erase(Off, Count);
    break;
  default:
    return false_a;
    break;
  }
  return true_a;
}

bool_a CPDFString::Append(CPDFChar pdfChar)
{
  if (pdfChar.GetCharType() != GetStringType())
    return false_a;

  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    {
      const char cTemp = pdfChar.GetChar();
      m_pStr->append(&cTemp, 1);
      break;
    }
  case CPDFChar::eWideString:
    {
      const wchar_t wcTemp = pdfChar.GetWChar();
      m_pWstr->append(&wcTemp, 1);
      break;
    }
  default:
    return false_a;
    break;
  }
  return true_a;
}

void CPDFString::SetString(std::wstring &i_wstr)
{
  Clean();
  m_eStringType = CPDFChar::eWideString;
  m_pWstr = new wstring(i_wstr);
}

void CPDFString::SetString(std::string &i_str)
{
  Clean();
  m_eStringType = CPDFChar::eMultibyteString;
  m_pStr = new string(i_str);
}

void CPDFString::SetString(const char *i_pStr)
{
  Clean();
  m_eStringType = CPDFChar::eMultibyteString;
  m_pStr = new string(i_pStr);
}

bool_a CPDFString::HasOnlyWhiteSigns()
{
  if (Length() == 0)
    return true_a;
  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    if (m_pStr)
    {
      basic_string <char>::iterator strIter;
      for (strIter = m_pStr->begin(); strIter != m_pStr->end(); strIter++)
      {
        if (*strIter != ' ')
          return false_a;
      }
    }
    break;
  case CPDFChar::eWideString:
    if (m_pWstr)
    {
      basic_string <wchar_t>::iterator wstrIter;
      for (wstrIter = m_pWstr->begin(); wstrIter != m_pWstr->end(); wstrIter++)
      {
        if (*wstrIter != L' ')
          return false_a;
      }
    }
    break;
  default:
    break;
  }
  return true_a;
}

bool_a CPDFString::IsLastSpace()
{
  if (Length() == 0)
    return false_a;
  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    if (m_pStr)
    {
      basic_string <char>::reverse_iterator rstrIter = m_pStr->rbegin();
      if (*rstrIter == ' ')
        return true_a;
    }
    break;
  case CPDFChar::eWideString:
    if (m_pWstr)
    {
      basic_string <wchar_t>::reverse_iterator rwstrIter = m_pWstr->rbegin();
      if (*rwstrIter == L' ')
        return true_a;
    }
    break;
  default:
    break;
  }
  return false_a;
}

size_t CPDFString::SpaceCount()
{
  size_t retSize(0);
  if (Length() == 0)
    return retSize;
  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    if (m_pStr)
    {
      basic_string <char>::size_type index(0);
      index = m_pStr->find(' ');
      while (index != -1)
      {
        retSize++;
        index++;
        index = m_pStr->find(' ', index);
      }
    }
    break;
  case CPDFChar::eWideString:
    if (m_pWstr)
    {
      basic_string <wchar_t>::size_type index(0);
      index = m_pWstr->find(' ');
      while (index != -1)
      {
        retSize++;
        index++;
        index = m_pWstr->find(' ', index);
      }
    }
    break;
  default:
    break;
  }
  return retSize;
}

const string *CPDFString::GetString()
{
  if (m_pStr)
    return m_pStr;
  return NULL;
}

const wstring *CPDFString::GetWString()
{
  if (m_pWstr)
    return m_pWstr;
  return NULL;
}

CPDFChar CPDFString::GetPDFChar(size_t i_index)
{
  if (i_index < 0 || i_index >= Length())
    return CPDFChar();
  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    return CPDFChar(m_pStr->at(i_index));
  case CPDFChar::eWideString:
    return CPDFChar(m_pWstr->at(i_index));
  }
  return CPDFChar();
}

CPDFChar::eStringType CPDFString::GetStringType()
{
  return m_eStringType;
}

size_t CPDFString::Length()
{
  switch (m_eStringType)
  {
  case CPDFChar::eMultibyteString:
    if (m_pStr)
      return m_pStr->size();
    break;
  case CPDFChar::eWideString:
    if (m_pWstr)
      return m_pWstr->size();
    break;
  default:
    break;
  }
  return 0;
}

bool_a CPDFString::Empty()
{
  if (Length() <= 0)
    return true_a;
  return false_a;
}

bool_a CPDFString::Valid()
{
  return m_eStringType != CPDFChar::eUndefinedString;
}

void CPDFString::Invalidate()
{
  Clean();
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//CPDFChar
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


CPDFChar::CPDFChar()
{
   m_eCharType = eUndefinedString;
}

CPDFChar::CPDFChar(char i_cChar)
{
  SetChar(i_cChar);
}

CPDFChar::CPDFChar(unsigned char i_ucChar)
{
  SetChar(i_ucChar);
}

CPDFChar::CPDFChar(wchar_t i_wChar)
{
  SetChar(i_wChar);
}

CPDFChar::~CPDFChar()
{
}

void CPDFChar::operator=(const CPDFChar& c)
{
  m_eCharType = c.m_eCharType;
  m_wChar = c.m_wChar;
}

void CPDFChar::operator=(const char& c)
{
  SetChar(c);
}

void CPDFChar::operator=(const wchar_t& w)
{
  SetChar(w);
}

void CPDFChar::SetChar(char i_cChar)
{
  m_eCharType = eMultibyteString;
  m_cChar = i_cChar;
}

void CPDFChar::SetChar(unsigned char i_ucChar)
{
  m_eCharType = eMultibyteString;
  m_cChar = (char)i_ucChar;
}

void CPDFChar::SetChar(wchar_t i_wChar)
{
  m_eCharType = eWideString;
  m_wChar = i_wChar;
}

bool_a CPDFChar::IsSpace()
{
  switch (m_eCharType)
  {
  case eMultibyteString:
    if (' ' == m_cChar)
      return true_a;
    break;
  case eWideString:
    if (L' ' == m_wChar)
      return true_a;
    break;
  default:
    break;
  }
  return false_a;
}

bool_a CPDFChar::IsWhite()
{
  switch (m_eCharType)
  {
  case eMultibyteString:
    if(' ' == m_cChar || '\n' == m_cChar || '\r' == m_cChar || '\0' == m_cChar || '\t' == m_cChar)
      return true_a;
    break;
  case eWideString:
    if(L' ' == m_wChar || L'\n' == m_wChar || L'\r' == m_wChar || L'\0' == m_wChar || L'\t' == m_wChar)
      return true_a;
    break;
  default:
    break;
  }
  return false_a;
}

bool_a CPDFChar::IsTab()
{
  switch (m_eCharType)
  {
  case eMultibyteString:
    if ('\t' == m_cChar)
      return true_a;
    break;
  case eWideString:
    if (L'\t' == m_wChar)
      return true_a;
    break;
  default:
    break;
  }
  return false_a;
}

bool_a CPDFChar::IsNewLine()
{
  switch (m_eCharType)
  {
  case eMultibyteString:
    if ('\n' == m_cChar || '\r' == m_cChar)
      return true_a;
    break;
  case eWideString:
    if (L'\n' == m_wChar || L'\r' == m_wChar)
      return true_a;
    break;
  default:
    break;
  }
  return false_a;
}

bool_a CPDFChar::IsBrek()
{
  switch (m_eCharType)
  {
  case eMultibyteString:
    if('\n' == m_cChar || '\r' == m_cChar || '\0' == m_cChar || '\t' == m_cChar)
      return true_a;
    break;
  case eWideString:
    if(L'\n' == m_wChar || L'\r' == m_wChar || L'\0' == m_wChar || L'\t' == m_wChar)
      return true_a;
    break;
  default:
    break;
  }
  return false_a;
}

bool_a CPDFChar::Equal(CPDFChar pdfChar)
{
  //TODO: convert if type is different
  if (pdfChar.GetCharType() != GetCharType())
    return false_a;
  switch (m_eCharType)
  {
  case eMultibyteString:
    if(pdfChar.GetChar() == m_cChar)
      return true_a;
    break;
  case eWideString:
    if(pdfChar.GetWChar() == m_wChar)
      return true_a;
    break;
  default:
    break;
  }
  return false_a;
}

const char CPDFChar::GetChar()
{
  return m_cChar;
}

const wchar_t CPDFChar::GetWChar()
{
  return m_wChar;
}

CPDFChar::eStringType CPDFChar::GetCharType()
{
  return m_eCharType;
}

bool_a CPDFChar::Valid()
{
  return m_eCharType != eUndefinedString;
}

void CPDFChar::Invalidate()
{
  m_eCharType = eUndefinedString;
}
