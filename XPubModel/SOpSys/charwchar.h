#if !defined(_CHARWCHAR_H_)
#define _CHARWCHAR_H_


#if defined(LINUX_OS) || defined(MAC_OS_MWE)


#if defined(UNICODE) || defined(_UNICODE)

#define _tcscmp    wcscmp
#define _tcsncmp   wcsncmp
#define _tcsnicmp  wcsncasecmp
#define _tcslen    wcslen
#define _tcsstr    wcsstr
#define _tcschr    wcschr
#define _tcscpy    wcscpy
#define _ttoi      wtoi
#define _ttol      wtol
#define _tstof     wtof
#define _tcstoul   wstrtoul
#define _stprintf  swprintf

#else

#define _tcscmp    strcmp
#define _tcsncmp   strncmp
#define _tcsnicmp  strncasecmp
#define _tcslen    strlen
#define _tcsstr    strstr
#define _tcschr    strchr
#define _tcscpy    strcpy
#define _ttoi      atoi
#define _ttol      atol
#define _tstof     atof
#define _tcstoul   strtoul
#define _stprintf  sprintf

#endif


#elif defined(WIN32)

#include <tchar.h>

#else
#error "not implemented"
#endif


#endif // !defined(_CHARWCHAR_H_)
