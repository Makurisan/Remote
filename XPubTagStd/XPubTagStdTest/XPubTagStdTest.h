// XPubTagStdTest.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CXPubTagStdTestApp:
// See XPubTagStdTest.cpp for the implementation of this class
//

class CXPubTagStdTestApp : public CWinApp
{
public:
	CXPubTagStdTestApp();

// Overrides
	public:
	virtual BOOL InitInstance();

  CString GetAppFolder();
// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CXPubTagStdTestApp theApp;