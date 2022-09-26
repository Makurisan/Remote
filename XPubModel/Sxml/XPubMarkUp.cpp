#include "XPubMarkUp.h"

unsigned int CXPubMarkUp::m_nCodePage = DEFAULT_XPUB_CODEPAGE;


CXPubMarkUp::CXPubMarkUp()
{
  m_bFileInUTF8 = false;
}

CXPubMarkUp::~CXPubMarkUp()
{
}




// encoding="UTF-8"
bool CXPubMarkUp::Load(const char* szFileName)
{
  m_bFileInUTF8 = false;
  bool bRet = CMarkupSTL::Load(szFileName);
  if (bRet)
  {
    xtstring sEncoding;
    sEncoding = CMarkupSTL::GetDeclaredEncoding(GetDoc().c_str());
    if (sEncoding == _XT("UTF-8"))
      m_bFileInUTF8 = true;
  }
  return bRet;
}

bool CXPubMarkUp::SetDoc(const char* szDoc)
{
  m_bFileInUTF8 = false;
  bool bRet = CMarkupSTL::SetDoc(szDoc);
  if (bRet)
  {
    xtstring sEncoding;
    sEncoding = CMarkupSTL::GetDeclaredEncoding(szDoc);
    if (sEncoding == _XT("UTF-8"))
      m_bFileInUTF8 = true;
  }
  return bRet;
}




bool CXPubMarkUp::AddChildElem(const char* szName,
                               const char* szData /*= NULL*/,
                               int nFlags /*= 0*/)
{
  if (!szData || !m_bFileInUTF8)
    return CMarkupSTL::AddChildElem(szName, szData, nFlags);

  if (isLegalUTF8String((UTF8*)szData, strlen(szData)))
    return CMarkupSTL::AddChildElem(szName, szData, nFlags);

  xastring sData;
  xwstring s;
  s.ConvertAndAssign(xastring(szData), m_nCodePage);
  sData.ConvertAndAssign(s, UTF8_CODEPAGE);
  return CMarkupSTL::AddChildElem(szName, sData.c_str(), nFlags);
}

bool CXPubMarkUp::AddChildElem(const char* szName,
                               int nValue,
                               int nFlags /*= 0*/)
{
  return CMarkupSTL::AddChildElem(szName, nValue, nFlags);
}

bool CXPubMarkUp::SetAttrib(const char* szAttrib,
                            const char* szValue)
{
  if (!szValue || szValue[0] == 0 || !m_bFileInUTF8)
    return CMarkupSTL::SetAttrib(szAttrib, szValue);

  if (isLegalUTF8String((UTF8*)szValue, strlen(szValue)))
    return CMarkupSTL::SetAttrib(szAttrib, szValue);

  xastring sValue;
  xwstring s;
  s.ConvertAndAssign(xastring(szValue), m_nCodePage);
  sValue.ConvertAndAssign(s, UTF8_CODEPAGE);
  bool bRet = CMarkupSTL::SetAttrib(szAttrib, sValue.c_str());
  GetDoc();
  return bRet;
}

bool CXPubMarkUp::SetAttrib(const char* szAttrib,
                            int nValue)
{
  return CMarkupSTL::SetAttrib(szAttrib, nValue);
}

bool CXPubMarkUp::AddAttrib(const char* szAttrib,
                            const char* szValue)
{
  if (!szValue || szValue[0] == 0 || !m_bFileInUTF8)
    return CMarkupSTL::AddAttrib(szAttrib, szValue);

  if (isLegalUTF8String((UTF8*)szValue, strlen(szValue)))
    return CMarkupSTL::AddAttrib(szAttrib, szValue);

  xastring sValue;
  xwstring s;
  s.ConvertAndAssign(xastring(szValue), m_nCodePage);
  sValue.ConvertAndAssign(s, UTF8_CODEPAGE);
  return CMarkupSTL::AddAttrib(szAttrib, sValue.c_str());
}

bool CXPubMarkUp::AddAttrib(const char* szAttrib,
                            int nValue)
{
  return CMarkupSTL::AddAttrib(szAttrib, nValue);
}

std::string CXPubMarkUp::GetAttrib(const char* szAttrib) const
{
  xastring sResult = CMarkupSTL::GetAttrib(szAttrib);
  if (sResult.size() == 0 || !m_bFileInUTF8)
    return  (std::string&)sResult;

  xwstring s;
  s.ConvertAndAssign(sResult, UTF8_CODEPAGE);
  sResult.ConvertAndAssign(s, m_nCodePage);
  return  (std::string&)sResult;
}

std::string CXPubMarkUp::GetAttrib_UTF8(const char* szAttrib) const
{
  xastring sResult = CMarkupSTL::GetAttrib(szAttrib);
  if (m_bFileInUTF8)
    return  (std::string&)sResult;

  xwstring s;
  s.ConvertAndAssign(sResult, m_nCodePage);
  sResult.ConvertAndAssign(s, UTF8_CODEPAGE);
  return (std::string&)sResult;
}

long CXPubMarkUp::GetAttribLong(const char* szAttrib) const
{
  return CMarkupSTL::GetAttribLong(szAttrib);
}

std::string CXPubMarkUp::GetData() const
{
  xastring sResult = CMarkupSTL::GetData();
  if (sResult.size() == 0 || !m_bFileInUTF8)
    return  (std::string&)sResult;

  xwstring s;
  s.ConvertAndAssign(sResult, UTF8_CODEPAGE);
  sResult.ConvertAndAssign(s, m_nCodePage);
  return  (std::string&)sResult;
}

std::string CXPubMarkUp::GetData_UTF8() const
{
  xastring sResult = CMarkupSTL::GetData();
  if (m_bFileInUTF8)
    return  (std::string&)sResult;

  xwstring s;
  s.ConvertAndAssign(sResult, m_nCodePage);
  sResult.ConvertAndAssign(s, UTF8_CODEPAGE);
  return  (std::string&)sResult;
}

bool CXPubMarkUp::SetData(const char* szData, int nFlags /*= 0*/)
{
  if (!szData || !m_bFileInUTF8)
    return CMarkupSTL::SetData(szData, nFlags);

  if (isLegalUTF8String((UTF8*)szData, strlen(szData)))
    return CMarkupSTL::SetData(szData, nFlags);

  xastring sValue;
  xwstring s;
  s.ConvertAndAssign(xastring(szData), m_nCodePage);
  sValue.ConvertAndAssign(s, UTF8_CODEPAGE);
  return CMarkupSTL::SetData(sValue.c_str(), nFlags);
}

bool CXPubMarkUp::SetData_UTF8(const char* szData, int nFlags /*= 0*/)
{
  return CMarkupSTL::SetData(szData, nFlags);
}

