// TER_HDR.H This file is to be included on top of each file

#define  STRICT

// include stdlib.h before windows.h to handle a bug in 
// BC45 and MSVC20 in the objbase.h file
#if !defined(INC_OLE1) && defined(__FLAT__)
  #include "stdlib.h"
#endif

#include "windows.h"

#if defined (_WIN32)
   #if !defined(WIN32)
     #define WIN32
   #endif
#endif
#if !defined(WIN32) 
  #include "print.h"
  #if defined (__TURBOC__)
    typedef unsigned short USHORT;     // missing in compiler
  #endif
#endif

#include "stdlib.h"
#include "stdio.h"
#include "ctype.h"
#include "dos.h"
#include "fcntl.h"
#include "io.h"
#include "sys\types.h"
#include "sys\stat.h"
#include "math.h"
#if !defined(NO_OLE)
  #if defined(INC_OLE1) 
     #include "ole.h"
  #else
     #include "ole2.h"
     #if defined (WIN32)
       #include "olectl.h"
     #endif
  #endif
#endif
#include "shellapi.h"

#if defined(WIN32) 
//  #if !defined (__TURBOC__)
    #define USE_WININET
    #include "wininet.h"
//  #endif
#endif

#include "commdlg.h"
#include "string.h"
#include "winnls.h"

#include "ter.h"
#include "ter1.h"
