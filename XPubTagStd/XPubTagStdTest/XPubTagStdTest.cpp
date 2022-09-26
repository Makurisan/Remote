// XPubTagStdTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XPubTagStdTest.h"
#include "XPubTagStdTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXPubTagStdTestApp

BEGIN_MESSAGE_MAP(CXPubTagStdTestApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CXPubTagStdTestApp construction

CXPubTagStdTestApp::CXPubTagStdTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CXPubTagStdTestApp object

CXPubTagStdTestApp theApp;


// CXPubTagStdTestApp initialization

BOOL CXPubTagStdTestApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CXPubTagStdTestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

CString CXPubTagStdTestApp::GetAppFolder()
{
  CString sFolder;
  sFolder.Empty();

  ::GetModuleFileName(AfxGetInstanceHandle(),
                      sFolder.GetBuffer(_MAX_PATH),
                      _MAX_PATH);
  sFolder.ReleaseBuffer();
  int nPos1 = sFolder.ReverseFind(_T('\\'));
  int nPos2 = sFolder.ReverseFind(_T('/'));
  if (nPos1 == -1) nPos1 = 0;
  if (nPos2 == -1) nPos2 = 0;

  if (nPos1 == 0 && nPos2 == 0)
    return sFolder;
  int nPos = max(nPos1, nPos2);
  sFolder = sFolder.Left(nPos + 1);
  return sFolder;
}
