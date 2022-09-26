// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

//#define _WIN32_WINNT 0x0501
//
//// Modify the following defines if you have to target a platform prior to the ones specified below.
//// Refer to MSDN for the latest info on corresponding values for different platforms.
//#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
//#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//#endif
//
//#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
//#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
//#endif						
//
//#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
//#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
//#endif
//
//#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
//#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
//#endif
//
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT




/*
das brauchen wir nicht mehr
#import "MSO.DLL"
#import "VBE6EXT.OLB"
#import "MSWORD.OLB" \
	rename ("FindText", "_FindText")\
	rename ("RGB", "_RGB")\
  rename ("ExitWindows", "_ExitWindows") no_implementation
#import "msado15.dll" rename ("EOF", "adoEOF") \
	rename ("FieldAttributeEnum", "adoFieldAttributeEnum") \
	rename ("DataTypeEnum", "adoDataTypeEnum") \
	rename ("ParameterDirectionEnum", "adoParameterDirectionEnum") \
	rename ("RecordStatusEnum", "adoRecordStatusEnum") \
	rename ("EditModeEnum", "adoEditModeEnum") \
	rename ("LockTypeEnum", "adoLockTypeEnum") 

#import "msadox.dll" 
#import "msjro.dll"
*/



#include "ter.h"
#include "ter_cmd.h"

