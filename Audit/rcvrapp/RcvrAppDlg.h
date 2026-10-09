// RcvrAppDlg.h : header file
//

#if !defined(AFX_RCVRAPPDLG_H__BA07E7B6_3E00_11D5_919D_0040335A7818__INCLUDED_)
#define AFX_RCVRAPPDLG_H__BA07E7B6_3E00_11D5_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRcvrAppDlg dialog

class CRcvrAppDlg : public CDialog
{
// Construction
public:
	CRcvrAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRcvrAppDlg)
	enum { IDD = IDD_RECV_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRcvrAppDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRcvrAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RCVRAPPDLG_H__BA07E7B6_3E00_11D5_919D_0040335A7818__INCLUDED_)
