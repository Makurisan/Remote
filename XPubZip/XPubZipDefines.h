#if !defined(_XPUBZIPDEFINES_H_)
#define _XPUBZIPDEFINES_H_

#undef _MFC_VER

#if (_MSC_VER < 1300) && !defined (__BORLANDC__)  && !defined (__GNUC__)
	#define ZIPINLINE inline
#else
	#define ZIPINLINE
#endif

#include "SZip/_features.h"

#define ZIP_ARCHIVE_STL

#if _MSC_VER > 1000
	#pragma once
//because of STL
	#pragma warning (disable : 4710) // 'function' : function not inlined
	#pragma warning (disable : 4514) // unreferenced inline/local function has been removed
	#pragma warning (disable:4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information
#endif // _MSC_VER > 1000


// some Windows typical definitions

#if defined (_UNICODE) && !defined (UNICODE)
	#define UNICODE
#endif
#if defined (UNICODE) && !defined (_UNICODE)
	#define _UNICODE
#endif

#ifndef _WIN32
	#ifndef NULL
		#define NULL    0
	#endif

	#include <ctype.h>
	typedef int HFILE;
	typedef void*				HANDLE;
	typedef unsigned long       DWORD;
	typedef long				LONG;
	typedef int                 BOOL;
	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;
	typedef unsigned int        UINT;

	#ifndef FALSE
		#define FALSE               (int)0
	#endif

	#ifndef TRUE
		#define TRUE                (int)1
	#endif


	typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
	typedef const WCHAR *LPCWSTR;
	typedef const char *LPCSTR;
	typedef WCHAR *LPWSTR;
	typedef char *LPSTR;

	#ifdef  _UNICODE
		typedef wchar_t TCHAR;
		typedef LPCWSTR LPCTSTR;
		typedef LPWSTR LPTSTR;
		#define _T(x)      L ## x
	#else   /* _UNICODE */               // r_winnt
		typedef char TCHAR;
		typedef LPCSTR LPCTSTR;
		typedef LPSTR LPTSTR;
		#define _T(x)      x
	#endif /* _UNICODE */                // r_winnt


#else
  	#include <TCHAR.H>
       	#include <windows.H>
      	#ifndef STRICT
		#define STRICT
	#endif
#endif	// #ifndef _WIN32

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT(f) assert((f))
#endif
#ifndef VERIFY
	#ifdef _DEBUG
		#define VERIFY(x) ASSERT((x))
	#else
		#define VERIFY(x) x
	#endif
#endif


#ifndef TRACE
	#define TRACE
#endif

#if _MSC_VER >= 1300
	#define ZIP_ULONGLONG ULONGLONG
	#define ZIP_LONGLONG LONGLONG
#else
	#define ZIP_ULONGLONG DWORD
	#define ZIP_LONGLONG LONG
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_XPUBZIPDEFINES_H_)
