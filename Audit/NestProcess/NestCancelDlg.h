#if !defined(_NEST_CANCEL_DLG_H)
#define _NEST_CANCEL_DLG_H
#pragma once


#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// NestCancelDlg

class CNestCancelDlg: public CDialog
{
// Construction
public:
	CNestCancelDlg( CString& label );

// Dialog Data
	//{{AFX_DATA(CNestCancelDlg)
	enum { IDD = IDD_NESTCANCEL };
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNestCancelDlg)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNestCancelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

public:

	CProgressCtrl& ProgressBar()	{ return m_progress; }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATEDIALOG_H__02F7EAD0_0638_49D1_BDB4_E0BD2A4CB4E3__INCLUDED_)
