// XPubZip.cpp : Defines the initialization routines for the DLL.
//

#if defined(LINUX_OS)

#include <stdio.h>


void __attribute__ ((constructor)) XPubZip_Init(void);
void __attribute__ ((destructor)) XPubZip_Finish(void);

void XPubZip_Init(void)
{
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubZip_Init <<<\n");
#endif
}

void XPubZip_Finish(void)
{
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubZip_Finish <<<\n");
#endif
}

#elif defined(WIN32)


#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE XPubZipDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("XPubZip.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(XPubZipDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(XPubZipDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("XPubZip.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(XPubZipDLL);
	}
	return 1;   // ok
}

#elif defined(MAC_OS_MWE)

 #include <stdio.h>

#else
#error "not implemented"
#endif

