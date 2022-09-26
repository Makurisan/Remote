// XPubModel.cpp : Defines the initialization routines for the DLL.
//


#if defined(LINUX_OS)

#include <stdio.h>
#include "STools/GlobalSettings.h"

/*
5.2. Library constructor and destructor functions
Libraries should export initialization and cleanup routines using the gcc __attribute__((constructor)) and __attribute__((destructor)) function attributes. See the gcc info pages for information on these. Constructor routines are executed before dlopen returns (or before main() is started if the library is loaded at load time). Destructor routines are executed before dlclose returns (or after exit() or completion of main() if the library is loaded at load time). The C prototypes for these functions are: 
  void __attribute__ ((constructor)) my_init(void);
  void __attribute__ ((destructor)) my_fini(void);
Shared libraries must not be compiled with the gcc arguments ``-nostartfiles'' or ``-nostdlib''. If those arguments are used, the constructor/destructor routines will not be executed (unless special measures are taken).

5.2.1. Special functions _init and _fini (OBSOLETE/DANGEROUS)
Historically there have been two special functions, _init and _fini that can be used to control constructors and destructors. However, they are obsolete, and their use can lead to unpredicatable results. Your libraries should not use these; use the function attributes constructor and destructor above instead.
If you must work with old systems or code that used _init or _fini, here's how they worked. Two special functions were defined for initializing and finalizing a module: _init and _fini. If a function ``_init'' is exported in a library, then it is called when the library is first opened (via dlopen() or simply as a shared library). In a C program, this just means that you defined some function named _init. There is a corresponding function called _fini, which is called whenever a client finishes using the library (via a call dlclose() that brings its reference count to zero, or on normal exit of the program). The C prototypes for these functions are: 
  void _init(void);
  void _fini(void);
In this case, when compiling the file into a ``.o'' file in gcc, be sure to add the gcc option ``-nostartfiles''. This keeps the C compiler from linking the system startup libraries against the .so file. Otherwise, you'll get a ``multiple-definition'' error. Note that this is completely different than compiling modules using the recommended function attributes. My thanks to Jim Mischel and Tim Gentry for their suggestion to add this discussion of _init and _fini, as well as help in creating it.
*/

void __attribute__ ((constructor)) XPubModel_Init(void);
void __attribute__ ((destructor)) XPubModel_Finish(void);

static CGS_DateTimeNumber* g_cgs;
void XPubModel_Init(void)
{
  g_cgs = new CGS_DateTimeNumber(CGlobalSettings::tGSU_Creator);
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubModel_Init <<<\n");
#endif
}

void XPubModel_Finish(void)
{
  if (g_cgs)
    delete g_cgs;
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubModel_Finish <<<\n");
#endif
}

#elif defined(WIN32)


#include "stdafx.h"
#include <afxdllx.h>

#include "STools/GlobalSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGS_DateTimeNumber* gpGlobalSettings = 0;

static AFX_EXTENSION_MODULE XPubModelDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("XPubModel.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(XPubModelDLL, hInstance))
			return 0;

    gpGlobalSettings = new CGS_DateTimeNumber(CGlobalSettings::tGSU_Creator);

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

		new CDynLinkLibrary(XPubModelDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("XPubModel.DLL Terminating!\n");

    if (gpGlobalSettings)
      delete gpGlobalSettings;

    // Terminate the library before destructors are called
		AfxTermExtensionModule(XPubModelDLL);
	}
	return 1;   // ok
}

#elif defined(MAC_OS_MWE)

#include <stdio.h>
#include "STools/GlobalSettings.h"

static CGS_DateTimeNumber* g_cgs;

class InitGlobal
{
public:
  InitGlobal()
  {
  	g_cgs = new CGS_DateTimeNumber(CGlobalSettings::tGSU_Creator);

#if !defined(_DEBUG)
  fprintf(stdout, "\t\t>>>XPubModel_Init\n");
#endif
  	
  };
  	
};
   
  InitGlobal ginitGlobal;
   


  pascal OSErr XPubModel_Init(const CFragInitBlock* theInitBlock);
  pascal void  XPubModel_Finish(void);

  pascal OSErr __initialize(const CFragInitBlock* theInitBlock);
  pascal void  __terminate(void);

  pascal OSErr XPubModel_Init(const CFragInitBlock* theInitBlock)
  {
    g_cgs = 0;
    
    OSErr theError;
    theError = __initialize (theInitBlock);
    if(theError != noErr)
      return(theError);
    
    g_cgs = new CGS_DateTimeNumber(CGlobalSettings::tGSU_Creator);
    
    return noErr;
  
}

  pascal void XPubModel_Finish(void)
  {
    if (g_cgs)
      delete g_cgs;

    __terminate ();

  }

#else
#error "not implemented"
#endif

