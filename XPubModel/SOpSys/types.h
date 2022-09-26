#if !defined(_TYPES_H_)
#define _TYPES_H_


#if defined(LINUX_OS)

typedef signed long long INT64;
typedef unsigned long DWORD;
typedef DWORD COLORREF;

#define XPUB_MAX_PATH PATH_MAX

#elif defined(WIN32)

#include <basetsd.h>

#define XPUB_MAX_PATH MAX_PATH

#elif defined(MAC_OS_MWE)

#define XPUB_MAX_PATH 255

typedef unsigned long DWORD;
typedef signed long long INT64;

typedef unsigned int            UINT;

typedef void*           LPVOID;
typedef const void*     LPCVOID;

typedef char*             LPCSTR;
typedef const char*       LPCTSTR;
typedef DWORD		  COLORREF;


#else
#error "not implemented"
#endif

#endif // !defined(_TYPES_H_)
