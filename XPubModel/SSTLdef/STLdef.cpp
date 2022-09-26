
#if defined(LINUX_OS)

#include <stdarg.h>
#include <wchar.h>

#elif defined(WIN32)

#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <winnls.h>

#elif defined(MAC_OS_MWE)

#include <stdarg.h>
#include <wchar.h>

#else
#error "not implemented"
#endif

#include "STLdef.h"

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

static const char validSingleByte[256] = {
/* 00 */  0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* \t, \n, \r allowed */
/* 20 */  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
/* 40 */  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
/* 60 */  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

          /* if (*source >= 0x80 && *source < 0xC2) return false */
/* 80 */  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* a0 */  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* c0 */  0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

          /* if (*source > 0xF4) return false; */
/* e0 */  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0
};

static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

UTF8 isLegalUTF8(UTF8 *source, int length) {
        UTF8 a;
        UTF8 *srcptr = source+length;
        switch (length) {
        default: return false;
                /* Everything else falls through when "true"... */
        case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
        case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
        case 2: if ((a = (*--srcptr)) > 0xBF) return false;
                switch (*source) {
                    /* no fall-through in this inner switch */
                    case 0xE0: if (a < 0xA0) return false; break;
                    case 0xF0: if (a < 0x90) return false; break;
                    case 0xF4: if (a > 0x8F) return false; break;
                    default:  if (a < 0x80) return false;
                }

        case 1: return validSingleByte[*source];
        }
        return true;
}

bool isLegalUTF8String(UTF8 *str, size_t len)
{
  UTF8 *cp = str;
  int i;
  bool bFound = false;
  while (*cp)
  {
    // how many bytes follow this character?
    int length = trailingBytesForUTF8[*cp]+1;
    if(length > 1) bFound = true;

    // check for early termination of string:
    for (i=1; i<length; i++) {
      if (cp[i] == 0) return 0;
    }

    // is this a valid group of characters?
    if (!isLegalUTF8(cp, length))
      return 0;

    cp += length;
  }
  // MAK: I dont know if this is correct 'bFound'
  return (cp == str + len) ? bFound : false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// xwstring
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const size_t xwstring::npos = -1;
const size_t xastring::npos = -1;

void xwstring::ConvertAndAssign(const xastring& s, unsigned int nCodePage)
{
  _imp.clear();

#if defined(WIN32)

  int wstringlen;

  wstringlen = MultiByteToWideChar(nCodePage, 0, s.c_str(), (int)s.size(), 0, 0);  if (wstringlen)
  {
    wstringlen++;
    wchar_t* wstring = new wchar_t[wstringlen];
    if (wstring)
    {
      if (MultiByteToWideChar(nCodePage, 0, s.c_str(), (int)s.size(), wstring, wstringlen))
        _imp.append(wstring, wstringlen - 1);
      delete wstring;
    }
  }

#elif defined(LINUX_OS)
#error "not implemented"
#elif defined(MAC_OS_MWE)
#error "not implemented"
#else
#error "not implemented"
#endif
}

xwstring::xwstring(xastring& s)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)

  // size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
  int wstringlen;
  mbstate_t ps;
  wstringlen = mbrtowc(0, s.c_str(), s.size(), &ps);
  if (wstringlen)
  {
    wstringlen++;
    wchar_t* wstring = new wchar_t[wstringlen];
    if (wstring)
    {
      if (mbrtowc(wstring, s.c_str(), s.size(), &ps))
        append(wstring, wstringlen - 1);
      delete wstring;
    }
  }

#elif defined(WIN32)
  
  int wstringlen;

  if (s.isUTF8String())
    wstringlen = MultiByteToWideChar(UTF8_CODEPAGE, 0, s.c_str(), (int)s.size(), 0, 0);
  else
    wstringlen = MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), 0, 0);
  if (wstringlen)
  {
    wstringlen++;
    wchar_t* wstring = new wchar_t[wstringlen];
    if (wstring)
    {
      if (s.isUTF8String())
      {
        if (MultiByteToWideChar(UTF8_CODEPAGE, 0, s.c_str(), (int)s.size(), wstring, wstringlen))
          append(wstring, wstringlen - 1);
      }
      else
      {
        if (MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), wstring, wstringlen))
          append(wstring, wstringlen - 1);
      }
      delete wstring;
    }
  }

#else
#error "not implemented"
#endif
}

void xwstring::operator = (const xastring& s)
{
  xwstring sWideString;
  sWideString.ConvertAndAssign(s, 0);
  _imp.erase(_imp.begin(), _imp.end());
  append(sWideString);
}

void xwstring::Format(LPCXWCHAR pszFormat, ...)
{
  va_list argList;
  va_start(argList, pszFormat);

  int nLength;
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
  nLength = vswprintf(0, 0, pszFormat, argList) * 10; //_vscwprintf
#elif defined(WIN32)
  nLength = _vscwprintf(pszFormat, argList) * 10; //_vscwprintf
#else
#error "not implemented"
#endif

  LPXWCHAR szBuffer = (LPXWCHAR)malloc((nLength + 1) * sizeof(XWCHAR));

  if (szBuffer)
  {
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
    vswprintf(szBuffer, nLength, pszFormat, argList); // vswprintf
#elif defined(WIN32)
    vswprintf(szBuffer, pszFormat, argList); // vswprintf
#else
#error "not implemented"
#endif
    *this = szBuffer;
    free(szBuffer);
  }

  va_end( argList );
}

void xwstring::TrimLeft()
{
  while (size() && (at(0) == _XW_(' ')
                    || at(0) == _XW_('\t')
                    || at(0) == _XW_('\r')
                    || at(0) == _XW_('\n')))
    erase(begin(), begin() + 1);
}

void xwstring::TrimRight()
{
  while (size() && (at(size() - 1) == _XW_(' ')
                    || at(size() - 1) == _XW_('\t')
                    || at(size() - 1) == _XW_('\r')
                    || at(size() - 1) == _XW_('\n')))
    erase(end() - 1, end());
}

void xwstring::Trim()
{
  TrimRight();
  TrimLeft();
}

void xwstring::TrimLeft(LPCXWCHAR pText)
{
  if (!pText)
    return;

  xwstring sText(pText);
  size_t nPos;

  for (;;)
  {
    nPos = find(pText);
    if (nPos != 0)
      break;
    erase(begin(), begin() + sText.size());
  }
}

void xwstring::TrimRight(LPCXWCHAR pText)
{
  if (!pText)
    return;

  xwstring sText(pText);
  size_t nPos;

  for(;;)
  {
    nPos = _imp.rfind(pText);
    if (nPos == xwstring::npos || size() != nPos + sText.size())
      break;
    erase(begin() + nPos, end());
  }
}

void xwstring::Trim(LPCXWCHAR pText)
{
  TrimRight(pText);
  TrimLeft(pText);
}

void xwstring::TrimLeft_AllChars(const xwstring& sChars)
{
  size_t nPos;
  nPos = _imp.find_first_of(sChars.c_str());
  while (nPos == 0)
  {
    erase(0, 1);
    nPos = _imp.find_first_of(sChars.c_str());
  }
}

void xwstring::TrimRight_AllChars(const xwstring& sChars)
{
  size_t nPos;
  nPos = _imp.find_last_of(sChars.c_str());
  while (_imp.size() && nPos == (_imp.size() - 1))
  {
    erase(_imp.size() - 1, 1);
    nPos = _imp.find_last_of(sChars.c_str());
  }
}

void xwstring::Trim_AllChars(const xwstring& sChars)
{
  TrimLeft_AllChars(sChars);
  TrimRight_AllChars(sChars);
}

xwstring xwstring::Left(size_t nLen) const
{
  if (nLen >= _imp.size())
    return _imp.c_str();
  wstring s;
  s.assign(_imp.begin(), _imp.begin() + nLen);
  return s;
}

xwstring xwstring::Right(size_t nLen) const
{
  if (nLen >= _imp.size())
    return _imp.c_str();
  wstring s;
  s.assign(_imp.end() - nLen - 1, _imp.end());
  return s;
}

xwstring xwstring::Mid(size_t nFirst) const
{
  return (Mid(nFirst, _imp.size() - nFirst));
}

xwstring xwstring::Mid(size_t nFirst, size_t nLen) const
{
  if (nLen >= _imp.size())
    return _imp.c_str();
  wstring s;
  s.assign(_imp.begin() + nFirst, _imp.begin() + nFirst + nLen);
  return s;
}

size_t xwstring::Find(XWCHAR ch, size_t startpos/* = 0*/) const
{
  return xwstring::find(ch, startpos);
}

size_t xwstring::Find(LPCXWCHAR lpszSub, size_t startpos/* = 0*/) const
{
  return xwstring::find(lpszSub, startpos);
}

size_t xwstring::FindNoCase(XWCHAR ch, size_t startpos /*= 0*/) const
{
	size_t locase = Find(tolower(ch), startpos);
	size_t upcase = Find(toupper(ch), startpos);

	return ((locase < upcase) ? locase : upcase);
}

size_t xwstring::FindNoCase(const xwstring& szSub, size_t startpos /*= 0*/) const
{
	xwstring sLowerThis = c_str();
	sLowerThis.MakeLower();

	xwstring sLowerSub = szSub.c_str();
	sLowerSub.MakeLower();

	return sLowerThis.Find(sLowerSub.c_str(), startpos);
}

size_t xwstring::FindNoCase(LPCXWCHAR lpszSub, size_t startpos /*= 0*/) const
{
	xwstring sLowerThis = c_str();
	sLowerThis.MakeLower();

	xwstring sLowerSub = lpszSub;
	sLowerSub.MakeLower();

	return sLowerThis.Find(sLowerSub.c_str(), startpos);
}

int xwstring::CompareNoCase(const xwstring& sText) const
{
  xwstring s1 = _imp.c_str();
  xwstring s2 = sText.c_str();

// unter LINUX existiert transform nicht
//  transform(s1.begin(), s1.end(), s1.begin(), tolower);
//  transform(s2.begin(), s2.end(), s2.begin(), tolower);

  xwstring::iterator is1;
  xwstring::iterator is2;
  for (is1 = s1.begin(); is1 != s1.end(); is1++)
    *is1 = tolower(*is1);
  for (is2 = s2.begin(); is2 != s2.end(); is2++)
    *is2 = tolower(*is2);

  return wcscmp(s1.c_str(), s2.c_str());
}

void xwstring::ReplaceNoCaseAll(const xwstring& sT1, const xwstring& sT2)
{
  size_t nPos = 0;
  size_t nLen = sT1.size();
  if (!nLen)
    return;
  do
  {
    nPos = FindNoCase(sT1.c_str(), nPos);
    if (nPos != xwstring::npos)
    {
      _imp.replace(_imp.begin() + nPos, _imp.begin() + nPos + nLen, sT2.c_str());
      nPos += sT2.size();
    }
  }
  while (nPos != xwstring::npos);
}

void xwstring::ReplaceAll(const xwstring& sT1, const xwstring& sT2)
{
  size_t nPos = 0;
  size_t nLen = sT1.size();
  if (!nLen)
    return;
  do
  {
    nPos = _imp.find(sT1.c_str(), nPos);
    if (nPos != xwstring::npos)
    {
      _imp.replace(_imp.begin() + nPos, _imp.begin() + nPos + nLen, sT2.c_str());
      nPos += sT2.size();
    }
  }
  while (nPos != xwstring::npos);
}

int xwstring::GetFieldCount(LPCXWCHAR delim)
{
	LPCXWCHAR lpsz;
  LPCXWCHAR lpszRemainder = c_str();
	size_t lenDelim = wcslen( delim );

  if (empty())
    return 0;

	int iCount = 1;
	while ((lpsz = wcsstr(lpszRemainder, delim)) != 0)
	{
		lpszRemainder = lpsz + lenDelim;
		iCount++;
	}

	return iCount;
}

int xwstring::GetFieldCount(XWCHAR delim)
{
	XWCHAR a[2];
	a[0] = delim;
	a[1] = 0;
	return GetFieldCount(a);
}

xwstring xwstring::GetField(LPCXWCHAR delim, int fieldnum)
{
  LPCXWCHAR lpsz;
  LPCXWCHAR lpszStart = c_str();
  LPCXWCHAR lpszRemainder = lpszStart;
  LPCXWCHAR lpszret = lpszRemainder;
  size_t retlen = 0;
  size_t lenDelim = wcslen(delim);

	while (fieldnum-- >= 0)
	{
		lpszret = lpszRemainder;
		lpsz = wcsstr(lpszRemainder, delim);
		if (lpsz)
		{
			// We did find the delim
			retlen = lpsz - lpszRemainder;
			lpszRemainder = lpsz + lenDelim;
		}
		else
		{
			retlen = wcslen(lpszRemainder);
			lpszRemainder += retlen;	// change to empty string
		}
	}
	return Mid(lpszret - lpszStart, retlen);
}

xwstring xwstring::GetField(XWCHAR delim, int fieldnum)
{
  LPCXWCHAR lpsz;
  LPCXWCHAR lpszStart = c_str();
  LPCXWCHAR lpszRemainder = lpszStart;
  LPCXWCHAR lpszret = lpszRemainder;
  size_t retlen = 0;

	while (fieldnum-- >= 0)
	{
		lpszret = lpszRemainder;
		lpsz = wcschr(lpszRemainder, delim);
		if (lpsz)
		{
			// We did find the delim
			retlen = lpsz - lpszRemainder;
			lpszRemainder = lpsz + 1;
		}
		else
		{
			retlen = wcslen(lpszRemainder);
			lpszRemainder += retlen;	// change to empty string
		}
	}
	return Mid(lpszret - lpszStart, retlen);
}

void xwstring::MakeLower()
{
  // unter LINUX existiert transform nicht
  // transform(begin(), end(), begin(), tolower);
  xwstring::iterator is;
  for (is = begin(); is != end(); is++)
    *is = tolower(*is);
}

void xwstring::MakeUpper()
{
  // unter LINUX existiert transform nicht
  // transform(begin(), end(), begin(), toupper);
  xwstring::iterator is;
  for (is = begin(); is != end(); is++)
    *is = toupper(*is);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// xastring
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

static std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

void xastring::encodeBase64(LPCXACHAR encodingBytes, unsigned int len )
{
  _imp =  base64_encode((unsigned char const*)encodingBytes, len); 
}
             
void xastring::encodeBase64(xastring const& s)
{
  _imp =  base64_encode((unsigned char const*)s.c_str(), s.size()); 
}

xastring xastring::decodeBase64()
{
  xastring s;
  s = base64_decode(_imp.c_str());
  return s;
}

bool xastring::isUTF8String() const
{
	return isLegalUTF8String((UTF8*)c_str(), size());
}

void xastring::ConvertAndAssign(const xwstring& s, unsigned int nCodePage)
{
  _imp.clear();

#if defined(WIN32)

  int astringlen;

  astringlen = WideCharToMultiByte(nCodePage, 0, s.c_str(), (int)s.size(), 0, 0, 0, 0);
  if (astringlen)
  {
    astringlen++;
    char* astring = new char[astringlen];
    if (astring)
    {
      if (WideCharToMultiByte(nCodePage, 0, s.c_str(), (int)s.size(), astring, astringlen, 0, 0))
        append(astring, astringlen - 1);
      delete astring;
    }
  }

#elif defined(LINUX_OS)
#error "not implemented"
#elif defined(MAC_OS_MWE)
#error "not implemented"
#else
#error "not implemented"
#endif
}

xastring::xastring(const xwstring& s)
     //    :basic_string<char, char_traits<char>, allocator<char> >()
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)

  // size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps);
  // size_t wcstombs(char *dest, const wchar_t *src, size_t n);
  int astringlen;

  astringlen = wcstombs(0, s.c_str(), s.size());
  if (astringlen)
  {
    astringlen++;
    char* astring = new char[astringlen];
    if (astring)
    {
      if (wcstombs(astring, s.c_str(), s.size()))
        append(astring, astringlen - 1);
      delete astring;
    }
  }

#elif defined(WIN32)
  
  int astringlen;

  astringlen = WideCharToMultiByte(/*CP_ACP*/UTF8_CODEPAGE, 0, s.c_str(), (int)s.size(), 0, 0, 0, 0);
  if (astringlen)
  {
    astringlen++;
    char* astring = new char[astringlen];
    if (astring)
    {
      if (WideCharToMultiByte(/*CP_ACP*/UTF8_CODEPAGE, 0, s.c_str(), (int)s.size(), astring, astringlen, 0, 0))
        append(astring, astringlen - 1);
      delete astring;
    }
  }

#else
#error "not implemented"
#endif
}

void xastring::operator = (const xwstring& s)
{
  xastring sAnsiString(s);
  erase(begin(), end());
  append(sAnsiString);
}

std::ostream&  operator << (ostream& rhs, const xastring& st) 
{ 
  size_t n = st.size();
  rhs << (string&)st;
  //rhs.write("\0", 1);
  return rhs; 
} 

void xastring::Format(LPCXTCHAR pszFormat, ...)
{
  va_list argList;
  va_start(argList, pszFormat);

  int nLength;
  
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
  nLength = vsnprintf(0, 0, pszFormat, argList) * 10; //_vscwprintf
#elif defined(WIN32)
  nLength = _vscprintf(pszFormat, argList) * 10; //_vscwprintf
#else
#error "not implemented"
#endif
  
  LPXACHAR szBuffer = (LPXACHAR)malloc((nLength + 1) * sizeof(XACHAR));

  if (szBuffer)
  {
#if defined(LINUX_OS) || defined(MAC_OS_MWE)
    vsprintf(szBuffer, pszFormat, argList); // vswprintf
#elif defined(WIN32)
    vsprintf(szBuffer, pszFormat, argList); // vswprintf
#else
#error "not implemented"
#endif
    *this = szBuffer;
    free(szBuffer);
  }

  va_end( argList );
}

void xastring::TrimLeft()
{
  while (size() && (at(0) == _XA_(' ')
                    || at(0) == _XA_('\t')
                    || at(0) == _XA_('\r')
                    || at(0) == _XA_('\n')))
    erase(begin(), begin() + 1);
}

void xastring::TrimRight()
{
  while (size() && (at(size() - 1) == _XA_(' ')
                    || at(size() - 1) == _XA_('\t')
                    || at(size() - 1) == _XA_('\r')
                    || at(size() - 1) == _XA_('\n')))
    erase(end() - 1, end());
}

void xastring::Trim()
{
  TrimRight();
  TrimLeft();
}

void xastring::TrimLeft(LPCXACHAR pText)
{
  if (!pText)
    return;

  xastring sText(pText);
  size_t nPos;

  for (;;)
  {
    nPos = _imp.find(pText);
    if (nPos != 0)
      break;
    _imp.erase(_imp.begin(), _imp.begin() + sText.size());
  }
}

void xastring::TrimRight(LPCXACHAR pText)
{
  if (!pText)
    return;

  xastring sText(pText);
  size_t nPos;

  for(;;)
  {
    nPos = _imp.rfind(pText);
    if (nPos == xastring::npos || size() != nPos + sText.size())
      break;
    _imp.erase(_imp.begin() + nPos, _imp.end());
  }
}

void xastring::Trim(LPCXACHAR pText)
{
  TrimRight(pText);
  TrimLeft(pText);
}

void xastring::TrimLeft_AllChars(const xastring& sChars)
{
  size_t nPos;
  nPos = _imp.find_first_of(sChars.c_str());
  while (nPos == 0)
  {
    erase(0, 1);
    nPos = _imp.find_first_of(sChars.c_str());
  }
}

void xastring::TrimRight_AllChars(const xastring& sChars)
{
  size_t nPos;
  nPos = _imp.find_last_of(sChars.c_str());
  while (size() && nPos == (size() - 1))
  {
    erase(size() - 1, 1);
    nPos = _imp.find_last_of(sChars.c_str());
  }
}

void xastring::Trim_AllChars(const xastring& sChars)
{
  TrimLeft_AllChars(sChars);
  TrimRight_AllChars(sChars);
}

xastring xastring::Left(size_t nLen) const
{
  if (nLen >= _imp.size())
    return _imp.c_str();
  string s;
  s.assign(_imp.begin(), _imp.begin() + nLen);
  return s;
}

xastring xastring::Right(size_t nLen) const
{
  if (nLen >= _imp.size())
    return _imp.c_str();
  string s;
  s.assign(_imp.end() - nLen - 1, _imp.end());
  return s;
}

xastring xastring::Mid(size_t nFirst) const
{
  return (Mid(nFirst, size() - nFirst));
}

xastring xastring::Mid(size_t nFirst, size_t nLen) const
{
  if (nLen >= _imp.size())
    return _imp.c_str();
  string s;
  s.assign(_imp.begin() + nFirst, _imp.begin() + nFirst + nLen);
  return s;
}

size_t xastring::Find(XACHAR ch, size_t startpos/* = 0*/) const
{
  return xastring::find(ch, startpos);
}

size_t xastring::Find(LPCXACHAR lpszSub, size_t startpos/* = 0*/) const
{
  return xastring::find(lpszSub, startpos);
}

size_t xastring::FindNoCase(XACHAR ch, size_t startpos /*= 0*/) const
{
	size_t locase = Find(tolower(ch), startpos);
	size_t upcase = Find(toupper(ch), startpos);

	return ((locase < upcase) ? locase : upcase);
}

size_t xastring::FindNoCase(LPCXACHAR lpszSub, size_t startpos /*= 0*/) const
{
	xastring sLowerThis = c_str();
	sLowerThis.MakeLower();

	xastring sLowerSub = lpszSub;
	sLowerSub.MakeLower();

	return sLowerThis.Find(sLowerSub.c_str(), startpos);
}

int xastring::CompareNoCase(const xastring& sText) const
{
  xastring s1 = c_str();
  xastring s2 = sText;

  // unter LINUX existiert transform nicht
//  transform(s1.begin(), s1.end(), s1.begin(), tolower);
//  transform(s2.begin(), s2.end(), s2.begin(), tolower);

  xastring::iterator is1;
  xastring::iterator is2;
  for (is1 = s1.begin(); is1 != s1.end(); is1++)
    *is1 = tolower(*is1);
  for (is2 = s2.begin(); is2 != s2.end(); is2++)
    *is2 = tolower(*is2);

  return strcmp(s1.c_str(), s2.c_str());
}

void xastring::ReplaceNoCaseAll(const xastring& sT1, const xastring& sT2)
{
  size_t nPos = 0;
  size_t nLen = sT1.size();
  if (!nLen)
    return;
  do
  {
    nPos = FindNoCase(sT1.c_str(), nPos);
    if (nPos != xastring::npos)
    {
      _imp.replace(_imp.begin() + nPos, _imp.begin() + nPos + nLen, sT2.c_str());
      nPos += sT2.size();
    }
  }
  while (nPos != xastring::npos);
}

void xastring::ReplaceAll(const xastring& sT1, const xastring& sT2)
{
  size_t nPos = 0;
  size_t nLen = sT1.size();
  if (!nLen)
    return;
  do
  {
    nPos = _imp.find(sT1.c_str(), nPos);
    if (nPos != xastring::npos)
    {
      _imp.replace(begin() + nPos, begin() + nPos + nLen, sT2.c_str());
      nPos += sT2.size();
    }
  }
  while (nPos != xastring::npos);
}

int xastring::GetFieldCount(LPCXACHAR delim)
{
	LPCXACHAR lpsz;
  LPCXACHAR lpszRemainder = c_str();
	size_t lenDelim = strlen( delim );

  if (empty())
    return 0;

	int iCount = 1;
	while ((lpsz = strstr(lpszRemainder, delim)) != 0)
	{
		lpszRemainder = lpsz + lenDelim;
		iCount++;
	}

	return iCount;
}

int xastring::GetFieldCount(XACHAR delim)
{
	XACHAR a[2];
	a[0] = delim;
	a[1] = 0;
	return GetFieldCount(a);
}

xastring xastring::GetField(LPCXACHAR delim, int fieldnum)
{
  LPCXACHAR lpsz;
  LPCXACHAR lpszStart = c_str();
  LPCXACHAR lpszRemainder = lpszStart;
  LPCXACHAR lpszret = lpszRemainder;
  size_t retlen = 0;
  size_t lenDelim = strlen(delim);

	while (fieldnum-- >= 0)
	{
		lpszret = lpszRemainder;
		lpsz = strstr(lpszRemainder, delim);
		if (lpsz)
		{
			// We did find the delim
			retlen = lpsz - lpszRemainder;
			lpszRemainder = lpsz + lenDelim;
		}
		else
		{
			retlen = strlen(lpszRemainder);
			lpszRemainder += retlen;	// change to empty string
		}
	}
	return Mid(lpszret - lpszStart, retlen);
}

xastring xastring::GetField(XACHAR delim, int fieldnum)
{
  LPCXACHAR lpsz;
  LPCXACHAR lpszStart = c_str();
  LPCXACHAR lpszRemainder = lpszStart;
  LPCXACHAR lpszret = lpszRemainder;
  size_t retlen = 0;

	while (fieldnum-- >= 0)
	{
		lpszret = lpszRemainder;
		lpsz = strchr(lpszRemainder, delim);
		if (lpsz)
		{
			// We did find the delim
			retlen = lpsz - lpszRemainder;
			lpszRemainder = lpsz + 1;
		}
		else
		{
			retlen = strlen(lpszRemainder);
			lpszRemainder += retlen;	// change to empty string
		}
	}
	return Mid(lpszret - lpszStart, retlen);
}

void xastring::MakeLower()
{
  // unter LINUX existiert transform nicht
  // transform(begin(), end(), begin(), tolower);
  xastring::iterator is;
  for (is = begin(); is != end(); is++)
    *is = tolower(*is);
}

void xastring::MakeUpper()
{
  // unter LINUX existiert transform nicht
  // transform(begin(), end(), begin(), toupper);
  xastring::iterator is;
  for (is = begin(); is != end(); is++)
    *is = toupper(*is);
}
