#if !defined(_STLDEF_H_)
#define _STLDEF_H_

#include "../SModelInd/ModelExpImp.h"
#include "../SOpSys/charwchar.h"


#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>
#include <list>

#if defined(LINUX_OS)
//#include <sstream>
#include <backward/strstream>
#elif defined(WIN32)
#include <strstream>

#elif defined(MAC_OS_MWE)

#include <strstream>

#else
#error "not implemented"
#endif

#pragma  warning(disable:4251)
#pragma  warning(disable:4172)
#pragma  warning(disable:4996)
                          
using namespace std;

//template class XPUBMODEL_EXPORTIMPORT std::allocator<char>;
//template class XPUBMODEL_EXPORTIMPORT std::allocator<wchar_t>;
                                                      
// default CPs
#define DEFAULT_CODEPAGE      0     // CP_ACP
#define DEFAULT_XPUB_CODEPAGE 1252  // west europe
#define UTF8_CODEPAGE         65001 // 

template <class T> class xpub_allocator;

// xpub_allocator for void
template <> class xpub_allocator<void>
{
public:
  typedef void*       pointer;
  typedef const void* const_pointer;
  // reference to void members are impossible.

  typedef void value_type;
  template <class U> struct rebind { typedef xpub_allocator<U> other; };
};

// xpub_allocator for T
template <class T> class xpub_allocator
{
public:
  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef T&        reference;
  typedef const T&  const_reference;
  typedef T         value_type;
  template <class U> struct rebind { typedef xpub_allocator<U> other; };

  xpub_allocator(){};
  xpub_allocator(const xpub_allocator&){};
  template <class U> xpub_allocator(const xpub_allocator<U>&){};
  ~xpub_allocator(){};

  pointer address(reference x) const {return &x;};
  const_pointer address(const_reference x) const {return &x;};

  pointer allocate(size_type size, xpub_allocator<void>::const_pointer hint = 0)
    {return ((pointer)(malloc(size * sizeof(T))));};
  void deallocate(pointer p, size_type n)
    {free((void*)(p));};
  size_type max_size() const throw() {return size_t(-1) / sizeof(value_type);};

  void construct(pointer p, const T& val)
    {new (static_cast<void*>(p)) T(val);};
  void destroy(pointer p)
    {p->~T();;};
};

template <typename T, typename U>
inline bool operator==(const xpub_allocator<T>&, const xpub_allocator<U>){return true;}

template <typename T, typename U>
inline bool operator!=(const xpub_allocator<T>&, const xpub_allocator<U>){return false;}










// wenn man XPub Projekte in UNICODE Projekten benutzen will,
// man muss FORCE_TO_ANSI_VERSION definieren
#ifdef FORCE_TO_ANSI_VERSION

#ifdef UNICODE
#define UNICODE_WAS_DEFINED
#undef UNICODE
#endif // UNICODE

#ifdef _UNICODE
#define _UNICODE_WAS_DEFINED
#undef _UNICODE
#endif // _UNICODE

#endif // FORCE_TO_ANSI_VERSION







typedef wchar_t         XWCHAR;
typedef wchar_t*        LPXWCHAR;
typedef const wchar_t*  LPCXWCHAR;
#define _XW_(x)         L##x

typedef char            XACHAR;
typedef char*           LPXACHAR;
typedef const char*     LPCXACHAR;
#define _XA_(x)         x

#if defined(UNICODE) || defined(_UNICODE)
#define _XT(x)          _XW_(x)
typedef XWCHAR          XTCHAR;
typedef LPXWCHAR        LPXTCHAR;
typedef LPCXWCHAR       LPCXTCHAR;
#else // #if defined(UNICODE) || defined(_UNICODE)
#define _XT(x)          _XA_(x)
typedef XACHAR          XTCHAR;
typedef LPXACHAR        LPXTCHAR;
typedef LPCXACHAR       LPCXTCHAR;
#endif // #if defined(UNICODE) || defined(_UNICODE)

typedef unsigned char UTF8;
XPUBMODEL_EXPORTIMPORT bool isLegalUTF8String(UTF8 *source, size_t length);

class xwstring;
class xastring;

class XPUBMODEL_EXPORTIMPORT xwstring
{
public:

  typedef wstring::iterator iterator;
  typedef wstring::const_iterator const_iterator;
  typedef wstring::reverse_iterator reverse_iterator;
  typedef wstring::reference reference;
  typedef wstring::const_reference const_reference;

  static const size_t npos;// = -1;

  void ConvertAndAssign(const xastring& s, unsigned int nCodePage);
  bool isUTF8String() const {return 0;};
 
  xwstring(xastring& s);
  xwstring(){};
  xwstring(const XWCHAR ch){  _imp.push_back(ch); }
  xwstring(LPCXWCHAR pText){_imp = (pText);};
  xwstring(xwstring& s){ _imp = s.c_str();};
  xwstring(const std::wstring& s){ _imp=s;};
  xwstring(std::wstring& s, size_t nOffset, size_t nCount){ _imp = s.substr(nOffset, nCount);};

  //const xwstring operator*() const{ return _imp._Myptr(); };
  const xwstring operator = (LPCXWCHAR pText) {return (_imp = (pText));};
  const xwstring operator = (const xwstring& s) {return _imp = s.c_str();;};
  const xwstring operator = (const XWCHAR ch){return _imp = (ch);};
  void operator = (const xastring& s);
  reference operator[](size_t off) { return ((_imp[off])); }
  const_reference operator[](size_t off) const { return ((_imp[off])); }
  const xwstring operator += (const XWCHAR ch){return _imp.append(&ch); };
  const xwstring operator += (const xwstring& s){return _imp.append(s.begin(), s.end()); };
  bool operator != (const xwstring& s) const{ return (!(wcscmp(_imp.c_str(),s.c_str())));};

  //////////////////////////////////////////////////
  void TrimLeft();
  void TrimRight();
  void Trim();
  void TrimLeft(LPCXWCHAR pText);
  void TrimRight(LPCXWCHAR pText);
  void Trim(LPCXWCHAR pText);
  void TrimLeft_AllChars(const xwstring& sChars);
  void TrimRight_AllChars(const xwstring& sChars);
  void Trim_AllChars(const xwstring& sChars);

  xwstring Left(size_t nLen) const;
  xwstring Right(size_t nLen) const;
  xwstring Mid(size_t nFirst) const;
  xwstring Mid(size_t nFirst, size_t nLen) const;

	size_t Find(XWCHAR ch, size_t startpos = 0) const;
	size_t Find(LPCXWCHAR lpszSub, size_t startpos = 0) const;
	size_t FindNoCase(XWCHAR ch, size_t startpos = 0) const;
	size_t FindNoCase(LPCXWCHAR lpszSub, size_t startpos = 0) const;
	size_t FindNoCase(const xwstring& szSub, size_t startpos = 0) const;

  int CompareNoCase(const xwstring& sText) const;
  void ReplaceAll(const xwstring& sT1, const xwstring& sT2);
  void ReplaceNoCaseAll(const xwstring& sT1, const xwstring& sT2);
  
  void Format(LPCXWCHAR lpszFormat, ...);

  int GetFieldCount(LPCXWCHAR delim);
  int GetFieldCount(XWCHAR delim);
	xwstring GetField(LPCXWCHAR delim, int fieldnum);
	xwstring GetField(XWCHAR delim, int fieldnum);

  void MakeLower();
  void MakeUpper();

  const wchar_t *c_str() const { return _imp.c_str(); }
 // const xwstring& c_str() const{ return _imp; }

  size_t size() const{ return _imp.size();}
  size_t length() const { return _imp.length();}

  xwstring append(wchar_t *s, size_t len) { _imp.append(s, len); return _imp;}
  xwstring append(xwstring& s) { _imp.append(s.c_str()); return _imp;}
  wchar_t at(int npos) { return _imp.at(npos);}
  bool empty() const { return _imp.empty();}

  xwstring erase(size_t Off = 0, size_t Count = std::wstring::npos) { _imp.erase(Off, Count); return _imp;}
  xwstring erase(iterator begin, iterator end) { _imp.erase(begin, end); return _imp;}

  iterator begin() { return _imp.begin(); }
  const_iterator begin() const { return _imp.begin(); }

  iterator end() { return _imp.end(); }
  const_iterator end() const { return _imp.end(); }

  size_t find(const wchar_t *str, size_t off = 0) const { return _imp.find(str, off);}
  size_t find(const wchar_t ch, size_t off = 0) const { return _imp.find(ch, off);}


private:
  typedef basic_string<wchar_t> xw_string; 
  std::wstring _imp;
};

class XPUBMODEL_EXPORTIMPORT xastring
{
public:

  typedef string::iterator iterator;
  typedef string::const_iterator const_iterator;
  typedef string::reverse_iterator reverse_iterator;
  typedef string::reference reference;
  typedef string::const_reference const_reference;

  static const size_t npos;// = -1;

  void ConvertAndAssign(const xwstring& s, unsigned int nCodePage);
  bool isUTF8String() const;
  xastring(const xwstring& s);

  
  xastring operator*() const{ return _imp._Myptr(); };

  xastring(){};
  xastring(const XACHAR ch){  _imp.push_back(ch); }
  xastring(LPCXACHAR s){_imp = (s);};
  xastring(const xastring& s){ _imp.append(s.begin(), s.end());};
  xastring(const std::string& s){ _imp=s;};
  xastring(std::string& s, size_t nOffset, size_t nCount){ _imp = s.substr(nOffset, nCount);};
  xastring(const xastring& s, size_t nOffset, size_t nCount){ _imp = s.substr(nOffset, nCount).c_str();};
  void encodeBase64(xastring const& encoded_string);
  void encodeBase64(LPCXACHAR encodingBytes, unsigned int in_len);
  xastring decodeBase64();

  friend xastring xastring::operator+ (const xastring& lhs, const xastring& rhs) 
  {
    xastring dest;
    dest.append(lhs);
    return dest.append(rhs);
  } ; 
  void operator = (const xwstring& s);
  const xastring operator = (LPCXACHAR s){return (_imp = (s));};
  const xastring operator = (const xastring& s){return _imp = s.c_str();};
  const xastring operator = (const XACHAR ch){return _imp = (ch);};
  const xastring operator += (const XACHAR ch){return _imp.append(&ch); };
  const xastring operator += (const xastring& s){return _imp.append(s.begin(), s.end()); };
  const xastring operator += (LPCXACHAR pText){return _imp += pText; };
  reference operator[](size_t off) { return ((_imp[off])); };
  const_reference operator[](size_t off) const { return ((_imp[off])); }

  bool operator!=(const xastring& s) const{ return  _imp != s.c_str();};
  bool operator==(const xastring& s) const{ return  _imp == s.c_str();};
  bool operator==(const XACHAR ch) const{ return (_imp == &ch);};
  bool operator>(const xastring& s) const { return _imp > s.c_str();};
  bool operator<(const xastring& s) const { return _imp < s.c_str();};
  bool operator<=(const xastring& s) const { return _imp <= s.c_str();};
  bool operator>=(const xastring& s) const { return _imp >= s.c_str();};
  const xastring& operator<< (const xastring& rhs) 
  { 
    _imp += rhs.c_str();
    return _imp; 
  }; 

  string operator () (const xastring& s)
  {
    return _imp = s.c_str();
  };

  //////////////////////////////////////////////////
  void TrimLeft();
  void TrimRight();
  void Trim();
  void TrimLeft(LPCXACHAR pText);
  void TrimRight(LPCXACHAR pText);
  void Trim(LPCXACHAR pText);
  void TrimLeft_AllChars(const xastring& sChars);
  void TrimRight_AllChars(const xastring& sChars);
  void Trim_AllChars(const xastring& sChars);

  xastring Left(size_t nLen) const;
  xastring Right(size_t nLen) const;
  xastring Mid(size_t nFirst) const;
  xastring Mid(size_t nFirst, size_t nLen) const;

	size_t Find(XACHAR ch, size_t startpos = 0) const;
	size_t Find(LPCXACHAR lpszSub, size_t startpos = 0) const;
	size_t FindNoCase(XACHAR ch, size_t startpos = 0) const;
	size_t FindNoCase(LPCXACHAR lpszSub, size_t startpos = 0) const;

  int CompareNoCase(const xastring& sText) const;
  void ReplaceAll(const xastring& sT1, const xastring& sT2);
  void ReplaceNoCaseAll(const xastring& sT1, const xastring& sT2);
  
  void Format(LPCXACHAR lpszFormat, ...);

  int GetFieldCount(LPCXACHAR delim);
  int GetFieldCount(XACHAR delim);
	xastring GetField(LPCXACHAR delim, int fieldnum);
	xastring GetField(XACHAR delim, int fieldnum);

  void MakeLower();
  void MakeUpper();

  const char *c_str() const { return _imp.c_str(); }
 // const xastring& c_str() { return _imp; }

  size_t size() const { return _imp.size();}
  size_t length() const { return _imp.length();}
  size_t find_last_of(const xastring& s, size_t off = std::string::npos) const {
    return _imp.find_last_of(s.c_str(), off);
  }
  size_t find_first_of(const xastring& s, size_t off = 0) const { return _imp.find_first_of(s.c_str(), off);}
 
  void clear() { _imp.clear();};
  xastring append(const char *s, size_t len) { _imp.append(s, len); return _imp;}
  xastring append(const xastring& s) { _imp.append(s.c_str()); return _imp;}
  xastring append(const xastring& s, size_t off, size_t count) { _imp.append(s.c_str(), off, count); return _imp;}
  xastring append(const_iterator begin, const_iterator end) { _imp.append(begin, end); return _imp;}
  
  xastring assign(const xastring& s) { _imp.assign(s.c_str()); return _imp;}
  xastring assign(const_iterator begin, const_iterator end)  { return _imp.assign(begin, end);}
  xastring assign(const xastring& s, size_t count){return _imp.assign(s.c_str(), _imp.size()?_imp.size()-1:0, count);}
  xastring assign(const xastring& s, size_t offs, size_t count){return _imp.assign(s.c_str(), offs, count);}
  xastring insert(size_t off, const xastring& s) { return _imp.insert(off, s.c_str()); }
  xastring insert(size_t off, const xastring& s, size_t count) { return _imp.insert(off, s.c_str(), count); }
  void insert(const_iterator begin, const_iterator& first, const_iterator& lasts) { _imp.insert(begin, first, lasts); }
  size_t rfind(const xastring& s, size_t off = std::string::npos) const { return _imp.rfind(s.c_str(), off); }

  const xastring replace(size_t off, size_t no, const xastring& s) { return _imp.replace(off, no, s.c_str(), 0, std::string::npos);}
  const xastring replace(const_iterator begin, const_iterator end, const xastring& s) { return _imp.replace(begin, end, s.c_str());}
  const xastring replace(const_iterator begin, const_iterator end, const_iterator begin2, const_iterator end2) { return _imp.replace(begin, end, begin2, end2);}
  void push_back(const XACHAR ch) { _imp.push_back(ch);}
  void push_back(const xastring& s) { _imp.push_back(*(s.c_str()));}

  char at(int npos) { return _imp.at(npos);}
  bool empty() const { return _imp.empty();}
  size_t find(const xastring& str, size_t off = 0) const { return _imp.find(str.c_str(), off); }
  xastring substr(size_t off = 0, size_t count = std::string::npos) const { return _imp.substr(off, count); }
 
  xastring erase(size_t Off = 0, size_t Count = std::string::npos) { _imp.erase(Off, Count); return _imp;}
  xastring erase(iterator begin, iterator end) { _imp.erase(begin, end); return _imp;}
  xastring erase(iterator begin) { _imp.erase(begin); return _imp;}

 iterator begin() { return _imp.begin(); }
 const_iterator begin() const { return _imp.begin(); }

 iterator end() { return _imp.end(); }
 const_iterator end() const { return _imp.end(); }
 
 size_t find(const char *str, size_t off = 0) const { return _imp.find(str, off);}
 size_t find(const char ch, size_t off = 0) const { return _imp.find(ch, off);}

 friend std::ostream& operator<< (ostream& rhs, const xastring& st); 
 
private:
  typedef basic_string<char> xa_string; 
  std::string _imp;

};




typedef xwstring::iterator CxwstringIterator;
typedef xwstring::const_iterator CxwstringConstIterator;
typedef xwstring::reverse_iterator CxwstringReverseIterator;

typedef xastring::iterator CxastringIterator;
typedef xastring::const_iterator CxastringConstIterator;
typedef xastring::reverse_iterator CxastringReverseIterator;



typedef vector<xwstring, xpub_allocator<xwstring> > CxwstringVector;
typedef CxwstringVector::iterator CxwstringVectorIterator;
typedef CxwstringVector::const_iterator CxwstringConstVectorIterator;
typedef CxwstringVector::reverse_iterator CxwstringVectorReverseIterator;

typedef vector<xastring, xpub_allocator<xastring> > CxastringVector;
typedef CxastringVector::iterator CxastringVectorIterator;
typedef CxastringVector::const_iterator CxastringConstVectorIterator;
typedef CxastringVector::reverse_iterator CxastringVectorReverseIterator;




typedef map<xwstring, xwstring, less<xwstring> > CxwstringMap;
typedef CxwstringMap::iterator CxwstringMapIterator;
typedef CxwstringMap::const_iterator CxwstringMapConstIterator;
typedef CxwstringMap::reverse_iterator CxwstringMapReverseIterator;
typedef pair<xwstring, xwstring> CxwstringMapPair;
typedef pair<CxwstringMapIterator, bool> CxwstringMapInsert;

typedef map<xastring, xastring, less<xastring> > CxastringMap;
typedef CxastringMap::iterator CxastringMapIterator;
typedef CxastringMap::const_iterator CxastringMapConstIterator;
typedef CxastringMap::reverse_iterator CxastringMapReverseIterator;
typedef pair<xastring, xastring> CxastringMapPair;
typedef pair<CxastringMapIterator, bool> CxastringMapInsert;


#if defined(UNICODE) || defined(_UNICODE)

typedef xwstring xtstring;
typedef CxwstringIterator CxtstringIterator;
typedef CxwstringConstIterator CxtstringConstIterator;
typedef CxwstringReverseIterator CxtstringReverseIterator;

typedef CxwstringVector CxtstringVector;
typedef CxwstringVectorIterator CxtstringVectorIterator;
typedef CxwstringConstVectorIterator CxtstringConstVectorIterator;
typedef CxwstringVectorReverseIterator CxtstringVectorReverseIterator;

typedef CxwstringMap CxtstringMap;
typedef CxwstringMapIterator CxtstringMapIterator;
typedef CxwstringMapConstIterator CxtstringMapConstIterator;
typedef CxwstringMapReverseIterator CxtstringMapReverseIterator;
typedef CxwstringMapPair CxtstringMapPair;
typedef CxwstringMapInsert CxtstringMapInsert;

#else // #if defined(UNICODE) || defined(_UNICODE)

typedef xastring xtstring;
typedef CxastringIterator CxtstringIterator;
typedef CxastringConstIterator CxtstringConstIterator;
typedef CxastringReverseIterator CxtstringReverseIterator;

typedef CxastringVector CxtstringVector;
typedef CxastringVectorIterator CxtstringVectorIterator;
typedef CxastringConstVectorIterator CxtstringConstVectorIterator;
typedef CxastringVectorReverseIterator CxtstringVectorReverseIterator;

typedef CxastringMap CxtstringMap;
typedef CxastringMapIterator CxtstringMapIterator;
typedef CxastringMapConstIterator CxtstringMapConstIterator;
typedef CxastringMapReverseIterator CxtstringMapReverseIterator;
typedef CxastringMapPair CxtstringMapPair;
typedef CxastringMapInsert CxtstringMapInsert;

#endif // #if defined(UNICODE) || defined(_UNICODE)




typedef unsigned char XTBYTE;
typedef unsigned long MODEL_COLOR;
typedef unsigned long COLORDEF;



typedef vector<void*>                 CvoidVector;
typedef CvoidVector::iterator         CvoidVectorIterator;
typedef CvoidVector::reverse_iterator CvoidVectorReverseIterator;

typedef vector<bool>                  CboolVector;
typedef CboolVector::iterator         CboolVectorIterator;
typedef CboolVector::reverse_iterator CboolVectorReverseIterator;

typedef vector<int>                   CintVector;
typedef CintVector::iterator          CintVectorIterator;
typedef CintVector::reverse_iterator  CintVectorReverseIterator;

typedef vector<size_t>                  CsizetVector;
typedef CsizetVector::iterator          CsizetVectorIterator;
typedef CsizetVector::reverse_iterator  CsizetVectorReverseIterator;

typedef vector<XTBYTE>                CBYTEVector;
typedef CBYTEVector::iterator         CBYTEVectorIterator;
typedef CBYTEVector::reverse_iterator CBYTEVectorReverseIterator;










class CERModelTableInd;
class CIFSEntityInd;

typedef std::vector<CERModelTableInd*>  CERTables;
typedef CERTables::iterator             CERTablesIterator;
typedef CERTables::const_iterator       CERTablesConstIterator;
typedef CERTables::reverse_iterator     CERTablesReverseIterator;

typedef map<xtstring, CERModelTableInd*>    CMapERTables;
typedef CMapERTables::iterator              CMapERTablesIterator;
typedef CMapERTables::const_iterator        CMapERTablesConstIterator;
typedef CMapERTables::reverse_iterator      CMapERTablesReverseIterator;
typedef pair<xtstring, CERModelTableInd*>   CMapERTablesPair;
typedef pair<CMapERTablesIterator, bool>    CMapERTablesInsert;

typedef std::vector<CIFSEntityInd*>     CIFSEntities;
typedef CIFSEntities::iterator          CIFSEntitiesIterator;
typedef CIFSEntities::const_iterator    CIFSEntitiesConstIterator;
typedef CIFSEntities::reverse_iterator  CIFSEntitiesReverseIterator;
















// wenn man XPub Projekte in UNICODE Projekten benutzen will,
// man muss FORCE_TO_ANSI_VERSION definieren
#ifdef FORCE_TO_ANSI_VERSION

#ifdef UNICODE_WAS_DEFINED
#define UNICODE
#endif // UNICODE_WAS_DEFINED
#ifdef _UNICODE_WAS_DEFINED
#define _UNICODE
#endif // _UNICODE_WAS_DEFINED

#endif // FORCE_TO_ANSI_VERSION


#endif // !defined(_STLDEF_H_)
