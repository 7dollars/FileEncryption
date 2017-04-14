// loadsysDlg.h : header file
//

#if !defined(AFX_LOADSYSDLG_H__8DADEC90_9407_42B5_88F9_09C15D198EA7__INCLUDED_)
#define AFX_LOADSYSDLG_H__8DADEC90_9407_42B5_88F9_09C15D198EA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLoadsysDlg dialog

class CLoadsysDlg : public CDialog
{
// Construction
public:
	CLoadsysDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadsysDlg)
	enum { IDD = IDD_LOADSYS_DIALOG };
	CString	m_syspathname;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadsysDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLoadsysDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonLoadsys();
	afx_msg void OnButtonUnloadsys();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADSYSDLG_H__8DADEC90_9407_42B5_88F9_09C15D198EA7__INCLUDED_)
