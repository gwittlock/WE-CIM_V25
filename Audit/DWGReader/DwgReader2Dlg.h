// DwgReader2Dlg.h : header file
//

#if !defined(AFX_DWGREADER2DLG_H__98727AA6_8DD8_11D6_B650_0003B3008E71__INCLUDED_)
#define AFX_DWGREADER2DLG_H__98727AA6_8DD8_11D6_B650_0003B3008E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDwgReader2Dlg dialog

class CDwgReader2Dlg : public CDialog
{
// Construction
public:
	CDwgReader2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDwgReader2Dlg)
	enum { IDD = IDD_DWGREADER2_DIALOG };
	CString	m_dwgPath;
	BOOL	m_test;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDwgReader2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDwgReader2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnDwgBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWGREADER2DLG_H__98727AA6_8DD8_11D6_B650_0003B3008E71__INCLUDED_)
