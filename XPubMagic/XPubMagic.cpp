// XPubMagic.cpp : Defines the entry point for the DLL application.
//

#if defined(LINUX_OS)

#include <stdio.h>


void __attribute__ ((constructor)) XPubMagic_Init(void);
void __attribute__ ((destructor)) XPubMagic_Finish(void);

void XPubMagic_Init(void)
{
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubMagic_Init <<<\n");
#endif
}

void XPubMagic_Finish(void)
{
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubMagic_Finish <<<\n");
#endif
}

#elif defined(WIN32)


#include "stdafx.h"

// Wegen XPubMagic.vcproj und XPubMagicLib.vcproj
// muessen wir ein bisschen zaubern
#if !defined(_LIB)
#define _LIB
#define _LIB_INTERNAL_DEFINED
#endif // !defined(_LIB)
#include <Magick++.h>
#if defined(_LIB_INTERNAL_DEFINED)
#undef _LIB
#endif // defined(_LIB_INTERNAL_DEFINED)

#include <string>
#include <list>
#include <vector>
#include <iostream> 

#include "XPicture.h"

#include "XPubMagicInit.h"


using namespace Magick;

#if !defined( ProvideDllMain )

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    XPubMagicInit(hModule, 0);
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    XPubMagicRelease();
  }
  return TRUE;
}
#endif

#elif defined(MAC_OS_MWE)


#else
#error "not implemented"
#endif
