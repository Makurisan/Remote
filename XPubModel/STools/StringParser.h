// StringParser.h: interface for the CStringParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGPARSER_H__E3708777_30A5_11D5_A483_00105ADBB436__INCLUDED_)
#define AFX_STRINGPARSER_H__E3708777_30A5_11D5_A483_00105ADBB436__INCLUDED_


#include "SModelInd/ModelExpImp.h"

#include <assert.h>

#include "../SSTLdef/STLdef.h"


typedef unsigned char __BYTE;

//typedef XACHAR __TCHAR;
//typedef XWCHAR __TCHAR;

//#define ___T(x)      _XA_(x)
//#define ___T(x)      _XW_(x)

//#define __LPCTSTR LPCXACHAR
//#define __LPCTSTR LPCXWCHAR

#define __NULL 0


namespace ParserChar
{
  const XACHAR     NUL_A     = _XA_('\0');
  const XACHAR     TAB_A     = _XA_('\t');
  const XACHAR     QUOTE_A   = _XA_('\"');
  const XACHAR     SPACE_A   = _XA_(' ');
  const XACHAR     SEMICOL_A = _XA_(';');
  const XWCHAR     NUL_W     = _XW_('\0');
  const XWCHAR     TAB_W     = _XW_('\t');
  const XWCHAR     QUOTE_W   = _XW_('\"');
  const XWCHAR     SPACE_W   = _XW_(' ');
  const XWCHAR     SEMICOL_W = _XW_(';');
};





class XPUBMODEL_EXPORTIMPORT CParseOptions_A
{
public:
  CParseOptions_A(XACHAR chDelimiter, 
                  XACHAR chQuoter = ParserChar::QUOTE_A,
                  XACHAR chEscape = ParserChar::NUL_A,
                  bool bGather   = false, 
                  bool bRTrim    = true, 
                  bool bKeepQuote  = false )
    : m_chDelimiter( chDelimiter )
    , m_chQuoter( chQuoter )
    , m_chEscape( chEscape )
    , m_bGather( bGather )
    , m_bRTrim( bRTrim )
    , m_bKeepQuote( bKeepQuote )
  {}

  inline void SetDelimiter( XACHAR chDelimiter ) { m_chDelimiter = chDelimiter; }
  inline void SetQuoter( XACHAR chQuoter )     { m_chQuoter = chQuoter; }
  inline void SetEscape( XACHAR chEscape )     { m_chEscape = chEscape; }

  inline XACHAR GetDelimiter() const       { return( m_chDelimiter ); }
  inline XACHAR GetQuoter() const          { return( m_chQuoter ); }
  inline XACHAR GetEscape() const          { return( m_chEscape ); }

  inline void NoDelimiter()           { m_chDelimiter = ParserChar::NUL_A; }
  inline void NoQuoter()              { m_chQuoter = ParserChar::NUL_A; }
  inline void NoEscape()              { m_chEscape = ParserChar::NUL_A; }

  inline bool IsDelimiter( XACHAR ch ) const   { return( m_chDelimiter != ParserChar::NUL_A && m_chDelimiter == ch ); }
  inline bool IsQuoter( XACHAR ch ) const      { return( m_chQuoter != ParserChar::NUL_A && m_chQuoter == ch ); }
  inline bool IsEscape( XACHAR ch ) const      { return( m_chEscape != ParserChar::NUL_A && m_chEscape == ch ); }

  inline void SetGather( bool bGather )     { m_bGather = bGather; }
  inline bool IsGather() const          { return( m_bGather ); }

  inline void SetRTrim( bool bRTrim )       { m_bRTrim = bRTrim; }
  inline bool IsRTrim() const           { return( m_bRTrim ); }

  inline void SetKeepQuote( bool bKeepQuote )   { m_bKeepQuote = bKeepQuote; }
  inline bool IsKeepQuote() const         { return( m_bKeepQuote ); }

protected:

  XACHAR   m_chDelimiter;  // to separate each string
  XACHAR   m_chQuoter;   // to introduce a quoted string
  XACHAR   m_chEscape;   // to escape to next character
  bool    m_bGather;    // to treat adjacent delimiters as one delimiter
  bool    m_bRTrim;   // to ignore empty trailing argument
  bool    m_bKeepQuote; // to keep the quote of a quoted argument
};

class XPUBMODEL_EXPORTIMPORT CStringParser_A
{
public:
  CStringParser_A();
  virtual ~CStringParser_A();

  void Empty();
  int Parse( LPCXACHAR pszStr, const CParseOptions_A& po );
  int GetCount() const;
  LPCXACHAR GetAt(int nIndex) const;

//#ifndef NDEBUG 
//  void Dump() const;
//#endif // #ifndef NDEBUG

protected:

  __BYTE*   m_pAlloc;
  XACHAR**   m_argv;
  int     m_argc;

  void Parse( LPCXACHAR pszStr, const CParseOptions_A& po, int& numargs, int& numchars, XACHAR** argv = __NULL, XACHAR* args = __NULL );

};

inline int CStringParser_A::GetCount() const
{
  return( m_argc );
}

inline LPCXACHAR CStringParser_A::GetAt(int nIndex) const
{
static LPCXACHAR pEmpty = "";
  if (!m_argv || nIndex < 0 || nIndex >= m_argc)
    return pEmpty;
  assert( m_argv != __NULL && nIndex >= 0 && nIndex < m_argc );
  return( m_argv[nIndex] );
}




















































class XPUBMODEL_EXPORTIMPORT CParseOptions_W
{
public:
  CParseOptions_W(XWCHAR chDelimiter, 
                  XWCHAR chQuoter = ParserChar::QUOTE_W,
                  XWCHAR chEscape = ParserChar::NUL_W,
                  bool bGather   = false, 
                  bool bRTrim    = true, 
                  bool bKeepQuote  = false )
    : m_chDelimiter( chDelimiter )
    , m_chQuoter( chQuoter )
    , m_chEscape( chEscape )
    , m_bGather( bGather )
    , m_bRTrim( bRTrim )
    , m_bKeepQuote( bKeepQuote )
  {}

  inline void SetDelimiter( XWCHAR chDelimiter ) { m_chDelimiter = chDelimiter; }
  inline void SetQuoter( XWCHAR chQuoter )     { m_chQuoter = chQuoter; }
  inline void SetEscape( XWCHAR chEscape )     { m_chEscape = chEscape; }

  inline XWCHAR GetDelimiter() const       { return( m_chDelimiter ); }
  inline XWCHAR GetQuoter() const          { return( m_chQuoter ); }
  inline XWCHAR GetEscape() const          { return( m_chEscape ); }

  inline void NoDelimiter()           { m_chDelimiter = ParserChar::NUL_W; }
  inline void NoQuoter()              { m_chQuoter = ParserChar::NUL_W; }
  inline void NoEscape()              { m_chEscape = ParserChar::NUL_W; }

  inline bool IsDelimiter( XWCHAR ch ) const   { return( m_chDelimiter != ParserChar::NUL_W && m_chDelimiter == ch ); }
  inline bool IsQuoter( XWCHAR ch ) const      { return( m_chQuoter != ParserChar::NUL_W && m_chQuoter == ch ); }
  inline bool IsEscape( XWCHAR ch ) const      { return( m_chEscape != ParserChar::NUL_W && m_chEscape == ch ); }

  inline void SetGather( bool bGather )     { m_bGather = bGather; }
  inline bool IsGather() const          { return( m_bGather ); }

  inline void SetRTrim( bool bRTrim )       { m_bRTrim = bRTrim; }
  inline bool IsRTrim() const           { return( m_bRTrim ); }

  inline void SetKeepQuote( bool bKeepQuote )   { m_bKeepQuote = bKeepQuote; }
  inline bool IsKeepQuote() const         { return( m_bKeepQuote ); }

protected:

  XWCHAR   m_chDelimiter;  // to separate each string
  XWCHAR   m_chQuoter;   // to introduce a quoted string
  XWCHAR   m_chEscape;   // to escape to next character
  bool    m_bGather;    // to treat adjacent delimiters as one delimiter
  bool    m_bRTrim;   // to ignore empty trailing argument
  bool    m_bKeepQuote; // to keep the quote of a quoted argument
};

class XPUBMODEL_EXPORTIMPORT CStringParser_W
{
public:
  CStringParser_W();
  virtual ~CStringParser_W();

  void Empty();
  int Parse( LPCXWCHAR pszStr, const CParseOptions_W& po );
  int GetCount() const;
  LPCXWCHAR GetAt(int nIndex) const;

//#ifndef NDEBUG 
//  void Dump() const;
//#endif // #ifndef NDEBUG

protected:

  __BYTE*   m_pAlloc;
  XWCHAR**   m_argv;
  int     m_argc;

  void Parse( LPCXWCHAR pszStr, const CParseOptions_W& po, int& numargs, int& numchars, XWCHAR** argv = __NULL, XWCHAR* args = __NULL );

};

inline int CStringParser_W::GetCount() const
{
  return( m_argc );
}

inline LPCXWCHAR CStringParser_W::GetAt(int nIndex) const
{
static LPCXWCHAR pEmpty = L"";
  if (!m_argv || nIndex < 0 || nIndex >= m_argc)
    return pEmpty;
  assert( m_argv != __NULL && nIndex >= 0 && nIndex < m_argc );
  return( m_argv[nIndex] );
}



#endif // !defined(AFX_STRINGPARSER_H__E3708777_30A5_11D5_A483_00105ADBB436__INCLUDED_)
