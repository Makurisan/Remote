<
// XPubTagStdTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XPubTagStdTest.h"
#include "XPubTagStdTestDlg.h"
#include ".\xpubtagstdtestdlg.h"

#include "XPubTagStdErr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXPubTagStdTestDlg dialog



CXPubTagStdTestDlg::CXPubTagStdTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXPubTagStdTestDlg::IDD, pParent)
{
  XPubTagStdInitialize();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CXPubTagStdTestDlg::~CXPubTagStdTestDlg()
{
  XPubTagStdTerminate();
}

void CXPubTagStdTestDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT, m_editTAG);
}

BEGIN_MESSAGE_MAP(CXPubTagStdTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUTTON_PDF, OnBnClickedButtonPdf)
  ON_BN_CLICKED(IDC_BUTTON_RTF, OnBnClickedButtonRtf)
END_MESSAGE_MAP()


// CXPubTagStdTestDlg message handlers

BOOL CXPubTagStdTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    CString sText = _T("");
    sText.LoadString(IDS_SAMPLE_TAG);
    m_editTAG.SetWindowText(sText);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXPubTagStdTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXPubTagStdTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CXPubTagStdTestDlg::OnShellFile(const CString& sFile)
{
  SHELLEXECUTEINFO	csSEI;

  memset(&csSEI, 0, sizeof(csSEI));
  csSEI.cbSize = sizeof(SHELLEXECUTEINFO);
  csSEI.fMask = SEE_MASK_FLAG_NO_UI;
  csSEI.lpVerb = _T("open");
  csSEI.lpFile = sFile;
  csSEI.nShow = SW_SHOWNORMAL; //SW_SHOWMAXIMIZED;
  ::ShellExecuteEx(&csSEI);
}

void CXPubTagStdTestDlg::OnBnClickedButtonPdf()
{
  CString sFile;
  sFile = theApp.GetAppFolder();
  sFile += _T("test.pdf");

  CString sTag;
  m_editTAG.GetWindowText(sTag);

  int nID = XPubTagStdInit();
  int nRet = XPubTagStdToPDFFile(nID, (LPTSTR)(LPCTSTR)sTag, (LPTSTR)(LPCTSTR)sFile);
  XPubTagStdClose(nID);

  if (nRet == XPubTagStd_OK)
    OnShellFile(sFile);
  else
  {
    CString sError;
    sError.Format(_T("Error: %ld"), nRet);
    AfxMessageBox(sError);
  }
}

void CXPubTagStdTestDlg::OnBnClickedButtonRtf()
{
  CString sFile;
  sFile = theApp.GetAppFolder();
  sFile += _T("test.rtf");

  CString sTag;
  m_editTAG.GetWindowText(sTag);

  int nID = XPubTagStdInit();
  int nRet = XPubTagStdToRTFFile(nID, (LPTSTR)(LPCTSTR)sTag, (LPTSTR)(LPCTSTR)sFile);
  XPubTagStdClose(nID);

  if (nRet == XPubTagStd_OK)
    OnShellFile(sFile);
  else
  {
    CString sError;
    sError.Format(_T("Error: %ld"), nRet);
    AfxMessageBox(sError);
  }
}
