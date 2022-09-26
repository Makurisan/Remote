// StringParser.cpp: implementation of the CStringParser class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include <stdlib.h>
#include "StringParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringParser_A::CStringParser_A()
{
  m_pAlloc    = __NULL;
  m_argv      = __NULL;
  m_argc      = 0;
}

CStringParser_A::~CStringParser_A()
{
  Empty();
}

void CStringParser_A::Empty()
{
  if ( m_pAlloc )
  {
    free( m_pAlloc );
    m_pAlloc    = __NULL;
    m_argv      = __NULL;
    m_argc      = 0;
  }
}

int CStringParser_A::Parse( LPCXACHAR pszCmd, const CParseOptions_A& po )
{
  Empty();

  if ( pszCmd )
  {
    int         numargs;
    int         numchars;
    XACHAR*    pBuf;

    Parse( pszCmd, po, numargs, numchars );

//#ifndef NDEBUG 
//    assert( numargs > 0 );
//    assert( numchars >= 0 );
//    _tprintf( _XA_("Count of args needed = %d\n"), numargs );
//    _tprintf( _XA_("Count of chars needed = %d\n"), numchars );
//#endif // #ifndef NDEBUG

    m_pAlloc = (__BYTE*)malloc( sizeof(XACHAR) * numchars + sizeof(XACHAR*) * numargs );
    if (!m_pAlloc) {
//      ::SetLastError(ERROR_OUTOFMEMORY);
      return( 0 );
    }

    m_argv  = (XACHAR**)m_pAlloc;
    pBuf  = (XACHAR*)( m_pAlloc  + ( sizeof(XACHAR*) * numargs ) );
    Parse( pszCmd, po, numargs, numchars, m_argv, pBuf );
    m_argc  = numargs - 1;

//#ifndef NDEBUG 
//    assert( numargs > 0 );
//    _tprintf( _XA_("Count of args returned = %d\n"), numargs );
//    _tprintf( _XA_("Count of chars returned = %d\n"), numchars );
//#endif // #ifndef NDEBUG
  }
    return( m_argc );
}

void CStringParser_A::Parse(LPCXACHAR pszStr,
                            const CParseOptions_A& po,
                            int& numargs,
                            int& numchars,
                            XACHAR** argv,
                            XACHAR* args)
{
    numchars  = 0;
    numargs   = 1;

  assert( (argv == __NULL && args == __NULL) || (argv != __NULL && args != __NULL) );

    if ( __NULL == pszStr || *pszStr == _XA_('\0') ) 
  {
        if ( argv ) argv[0] = __NULL;
        return;
    }

    LPCXACHAR       p     = pszStr;
    bool            bInQuote  = false;    /* true = inside quotes */
    bool            bCopyChar = true;     /* true = copy char to *args */
  bool      bNeedArg  = false;  /* true = found delimiter and not gathering */
    unsigned        numslash  = 0;    /* num of backslashes seen */

  assert( *p != _XA_('\0') );

  if ( po.IsQuoter( *p ) && p[1] != _XA_('\0') )
  {
    bInQuote = true;
    p++;
  }

    /* loop on each argument */
    for (;;) 
  {
        if (*p == _XA_('\0'))
    {
      if ( bNeedArg && !po.IsRTrim() )
      {
        /* scan an empty argument */
        if (argv)
          *argv++ = args;     /* store ptr to arg */
        ++numargs;
        /* add an empty argument */
        if (args)
          *args++ = _XA_('\0'); /* terminate string */
        ++numchars;
      }
      break;              /* end of args */
    }

        /* scan an argument */
        if (argv)
            *argv++ = args;     /* store ptr to arg */
        ++numargs;

        /* loop through scanning one argument */
        for (;;) 
    {     
      bCopyChar = true;
      numslash = 0;

      if ( po.IsEscape( *p )  )
      {
        /* 
        ** Rules: 2N backslashes + " ==> N backslashes and begin/end quote
        **      2N+1 backslashes + " ==> N backslashes + literal "
                **      N backslashes ==> N backslashes 
        */        
        /* count number of backslashes for use below */
        do { ++p; ++numslash; } while (*p == po.GetEscape());
      }
      
      if ( po.IsQuoter( *p ) ) 
      {
        /*
        ** if 2N backslashes before, start/end quote, otherwise
        ** copy literally 
        */
        if (numslash % 2 == 0) 
        {
          bool bChangeInQoute = true; // MIK
          if (bInQuote) 
          {
            if (p[1] == po.GetQuoter())
            { // MIK
              p++;    /* Double quote inside quoted string */
              bChangeInQoute = false; // MIK
            } // MIK
            else        /* skip first quote char and copy second */
              bCopyChar = po.IsKeepQuote();
          } else
            bCopyChar = po.IsKeepQuote();       /* don't copy quote */
          if (bChangeInQoute) // MIK
          bInQuote = !bInQuote;
        }
        numslash /= 2;          /* divide numslash by two */
      }

      /* copy slashes */
      while (numslash--) 
      {
        if (args)
          *args++ = po.GetEscape();
        ++numchars;
      }

            /* if at end of arg, break loop */
            if ( *p == _XA_('\0') )
      {
                break;
      }

      if ( !bInQuote && *p == po.GetDelimiter() )
      {
        do
        {
          p++;
        } while ( po.IsGather() && *p == po.GetDelimiter() );
        bNeedArg = true;
        break;
      }

            /* copy character into argument */
            if (bCopyChar) 
      {
                if (args)
                    *args++ = *p;
                ++numchars;
            }
            ++p;

      bNeedArg = false;
        }

        /* null-terminate the argument */
        if (args)
            *args++ = _XA_('\0');          /* terminate string */
        ++numchars;
    }

    /* We put one last argument in -- a null ptr */
    if (argv)
        *argv++ = __NULL;
}

//#ifndef NDEBUG 
//void CStringParser_A::Dump() const
//{
//  _tprintf( _XA_("Count of args = %d\n"), GetCount() );
//  for ( int i = 0; i < GetCount(); i++ )
//    _tprintf( _XA_("Arg[%d] = <%s>\n"), i, GetAt(i) );
//}
//#endif // #ifndef NDEBUG

















































//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringParser_W::CStringParser_W()
{
  m_pAlloc    = __NULL;
  m_argv      = __NULL;
  m_argc      = 0;
}

CStringParser_W::~CStringParser_W()
{
  Empty();
}

void CStringParser_W::Empty()
{
  if ( m_pAlloc )
  {
    free( m_pAlloc );
    m_pAlloc    = __NULL;
    m_argv      = __NULL;
    m_argc      = 0;
  }
}

int CStringParser_W::Parse( LPCXWCHAR pszCmd, const CParseOptions_W& po )
{
  Empty();

  if ( pszCmd )
  {
    int         numargs;
    int         numchars;
    XWCHAR*    pBuf;

    Parse( pszCmd, po, numargs, numchars );

//#ifndef NDEBUG 
//    assert( numargs > 0 );
//    assert( numchars >= 0 );
//    _tprintf( _XW_("Count of args needed = %d\n"), numargs );
//    _tprintf( _XW_("Count of chars needed = %d\n"), numchars );
//#endif // #ifndef NDEBUG

    m_pAlloc = (__BYTE*)malloc( sizeof(XWCHAR) * numchars + sizeof(XWCHAR*) * numargs );
    if (!m_pAlloc) {
//      ::SetLastError(ERROR_OUTOFMEMORY);
      return( 0 );
    }

    m_argv  = (XWCHAR**)m_pAlloc;
    pBuf  = (XWCHAR*)( m_pAlloc  + ( sizeof(XWCHAR*) * numargs ) );
    Parse( pszCmd, po, numargs, numchars, m_argv, pBuf );
    m_argc  = numargs - 1;

//#ifndef NDEBUG 
//    assert( numargs > 0 );
//    _tprintf( _XW_("Count of args returned = %d\n"), numargs );
//    _tprintf( _XW_("Count of chars returned = %d\n"), numchars );
//#endif // #ifndef NDEBUG
  }
    return( m_argc );
}

void CStringParser_W::Parse(LPCXWCHAR pszStr,
                            const CParseOptions_W& po,
                            int& numargs,
                            int& numchars,
                            XWCHAR** argv,
                            XWCHAR* args)
{
    numchars  = 0;
    numargs   = 1;

  assert( (argv == __NULL && args == __NULL) || (argv != __NULL && args != __NULL) );

    if ( __NULL == pszStr || *pszStr == _XW_('\0') ) 
  {
        if ( argv ) argv[0] = __NULL;
        return;
    }

    LPCXWCHAR       p     = pszStr;
    bool            bInQuote  = false;    /* true = inside quotes */
    bool            bCopyChar = true;     /* true = copy char to *args */
  bool      bNeedArg  = false;  /* true = found delimiter and not gathering */
    unsigned        numslash  = 0;    /* num of backslashes seen */

  assert( *p != _XW_('\0') );

  if ( po.IsQuoter( *p ) && p[1] != _XW_('\0') )
  {
    bInQuote = true;
    p++;
  }

    /* loop on each argument */
    for (;;) 
  {
        if (*p == _XW_('\0'))
    {
      if ( bNeedArg && !po.IsRTrim() )
      {
        /* scan an empty argument */
        if (argv)
          *argv++ = args;     /* store ptr to arg */
        ++numargs;
        /* add an empty argument */
        if (args)
          *args++ = _XW_('\0'); /* terminate string */
        ++numchars;
      }
      break;              /* end of args */
    }

        /* scan an argument */
        if (argv)
            *argv++ = args;     /* store ptr to arg */
        ++numargs;

        /* loop through scanning one argument */
        for (;;) 
    {     
      bCopyChar = true;
      numslash = 0;

      if ( po.IsEscape( *p )  )
      {
        /* 
        ** Rules: 2N backslashes + " ==> N backslashes and begin/end quote
        **      2N+1 backslashes + " ==> N backslashes + literal "
                **      N backslashes ==> N backslashes 
        */        
        /* count number of backslashes for use below */
        do { ++p; ++numslash; } while (*p == po.GetEscape());
      }
      
      if ( po.IsQuoter( *p ) ) 
      {
        /*
        ** if 2N backslashes before, start/end quote, otherwise
        ** copy literally 
        */
        if (numslash % 2 == 0) 
        {
          bool bChangeInQoute = true; // MIK
          if (bInQuote) 
          {
            if (p[1] == po.GetQuoter())
            { // MIK
              p++;    /* Double quote inside quoted string */
              bChangeInQoute = false; // MIK
            } // MIK
            else        /* skip first quote char and copy second */
              bCopyChar = po.IsKeepQuote();
          } else
            bCopyChar = po.IsKeepQuote();       /* don't copy quote */
          if (bChangeInQoute) // MIK
          bInQuote = !bInQuote;
        }
        numslash /= 2;          /* divide numslash by two */
      }

      /* copy slashes */
      while (numslash--) 
      {
        if (args)
          *args++ = po.GetEscape();
        ++numchars;
      }

            /* if at end of arg, break loop */
            if ( *p == _XW_('\0') )
      {
                break;
      }

      if ( !bInQuote && *p == po.GetDelimiter() )
      {
        do
        {
          p++;
        } while ( po.IsGather() && *p == po.GetDelimiter() );
        bNeedArg = true;
        break;
      }

            /* copy character into argument */
            if (bCopyChar) 
      {
                if (args)
                    *args++ = *p;
                ++numchars;
            }
            ++p;

      bNeedArg = false;
        }

        /* null-terminate the argument */
        if (args)
            *args++ = _XW_('\0');          /* terminate string */
        ++numchars;
    }

    /* We put one last argument in -- a null ptr */
    if (argv)
        *argv++ = __NULL;
}

//#ifndef NDEBUG 
//void CStringParser_W::Dump() const
//{
//  _tprintf( _XW_("Count of args = %d\n"), GetCount() );
//  for ( int i = 0; i < GetCount(); i++ )
//    _tprintf( _XW_("Arg[%d] = <%s>\n"), i, GetAt(i) );
//}
//#endif // #ifndef NDEBUG
