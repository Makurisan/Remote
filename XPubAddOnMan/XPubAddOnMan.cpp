// XPubAddOnMan.cpp : Defines the initialization routines for the DLL.
//


#if defined(LINUX_OS)

#include <stdio.h>


void __attribute__ ((constructor)) XPubAddOnMan_Init(void);
void __attribute__ ((destructor)) XPubAddOnMan_Finish(void);

void XPubAddOnMan_Init(void)
{
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubAddOnMan_Init <<<\n");
#endif
}

void XPubAddOnMan_Finish(void)
{
#if defined(_DEBUG)
  fprintf(stdout, "\t\t>>> XPubAddOnMan_Finish <<<\n");
#endif
}

#elif defined(WIN32)



#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HINSTANCE g_hinst;
char      g_app[MAX_PATH];

static AFX_EXTENSION_MODULE XPubAddOnManDLL = { NULL, NULL };

long FAR PASCAL WndRTFHelpProc( HWND hwnd,   UINT msg, UINT wParam, LONG lParam )
{
  return DefWindowProc( hwnd, msg, wParam, lParam );
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("XPubAddOnMan.DLL Initializing!\n");
		
		g_hinst = hInstance;
		::GetModuleFileName(hInstance, g_app, MAX_PATH);

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(XPubAddOnManDLL, hInstance))
			return 0;

		// Create the window class
		WNDCLASS  wc;
		if (!GetClassInfo(hInstance,"XPubRTFHelpClass",&wc))
		{
			wc.style = CS_DBLCLKS|CS_OWNDC|CS_HREDRAW|CS_VREDRAW|CS_GLOBALCLASS;  // allocate a DC for this class
			wc.lpfnWndProc = WndRTFHelpProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;                         // extra 4 bytes to store the data pointer
			wc.hInstance = hInstance;
			wc.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = ::LoadCursor(NULL,IDC_ARROW);
			wc.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName =  NULL;
			wc.lpszClassName = "XPubRTFHelpClass";
			if(RegisterClass(&wc))
			{
				HANDLE hMainWindow = CreateWindowEx(0, "XPubRTFHelpClass", 
					NULL, WS_POPUPWINDOW, 
					CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
					0, 0, hInstance, NULL); 
			}

		}

		new CDynLinkLibrary(XPubAddOnManDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("XPubAddOnMan.DLL Terminating!\n");

		HWND hWnd = FindWindow("XPubRTFHelpClass", NULL);
		if(IsWindow(hWnd))
			DestroyWindow(hWnd);

    UnregisterClass("XPubRTFHelpClass",hInstance);

		// Terminate the library before destructors are called
		AfxTermExtensionModule(XPubAddOnManDLL);
	}
	return 1;   // ok
}


#elif defined(MAC_OS_MWE) 

#else
#error "not implemented"
#endif
