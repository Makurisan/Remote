// XPubTagStdTestDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CXPubTagStdTestDlg dialog
class CXPubTagStdTestDlg : public CDialog
{
// Construction
public:
	CXPubTagStdTestDlg(CWnd* pParent = NULL);	// standard constructor
  ~CXPubTagStdTestDlg();

// Dialog Data
	enum { IDD = IDD_XPUBTAGSTDTEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

  void OnShellFile(const CString& sFile);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  CEdit m_editTAG;
  afx_msg void OnBnClickedButtonPdf();
  afx_msg void OnBnClickedButtonRtf();
};
