#if !defined(_XPUBTOOLS_TYPES_H_)
#define _XPUBTOOLS_TYPES_H_


#if defined(LINUX_OS)

#include <sys/types.h>

typedef uint            UINT;

typedef void*           LPVOID;
typedef const void*     LPCVOID;

#if defined(UNICODE) || defined(_UNICODE)

typedef wchar_t*          LPCSTR;
typedef const wchar_t*    LPCTSTR;

#else // #if defined(UNICODE) || defined(_UNICODE)

typedef char*             LPCSTR;
typedef const char*       LPCTSTR;

#endif // #if defined(UNICODE) || defined(_UNICODE)

#endif // LINUX_OS



#if defined(WIN32)

#endif // WIN32



#if defined(MAC_OS_MWE)

#include <sys/types.h>

typedef unsigned int            UINT;

typedef void*           LPVOID;
typedef const void*     LPCVOID;

typedef char*             LPCSTR;
typedef const char*       LPCTSTR;


#endif // MAC_OS_MWE



#endif // !defined(_XPUBTOOLS_TYPES_H_)
