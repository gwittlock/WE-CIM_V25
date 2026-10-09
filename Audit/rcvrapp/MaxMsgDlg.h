#if !defined(AFX_MAXMSGDLG_H__223D85C3_CCF8_11D4_919D_0040335A7818__INCLUDED_)
#define AFX_MAXMSGDLG_H__223D85C3_CCF8_11D4_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MaxMsgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMaxMsgDlg dialog

class CMaxMsgDlg : public CDialog
{
// Construction
public:

	CMaxMsgDlg( int currMax, CWnd* pParent = NULL );

	int MaxMsgs();

// Dialog Data
	//{{AFX_DATA(CMaxMsgDlg)
	enum { IDD = IDD_DIALOG1 };
	int		m_max;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMaxMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMaxMsgDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAXMSGDLG_H__223D85C3_CCF8_11D4_919D_0040335A7818__INCLUDED_)
