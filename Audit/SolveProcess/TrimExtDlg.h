#if !defined(AFX_TRIMEXTDLG_H__E91F7C91_F7F9_4F95_959A_FECF5521CEDC__INCLUDED_)
#define AFX_TRIMEXTDLG_H__E91F7C91_F7F9_4F95_959A_FECF5521CEDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrimExtDlg.h : header file
//

#include "resource.h"
#include "ViewMgr.h"
#include "GeoCurve.h"

/////////////////////////////////////////////////////////////////////////////
// TrimExtDlg dialog

class TrimExtDlg : public CDialog
{
// Construction
public:

	TrimExtDlg(CWnd* pParent = NULL);   // standard constructor

	void Shift( int top, int left );

	void setView(CViewBase* view)		{ m_view = view; }

	void setSolutions(
			CGeoCurveArray& candidatesA,
			int				defSolnIndxA,
			CGeoCurveArray& candidatesB,
			int				defSolnIndxB );

	void getSolutions(
			int*	indxA,
			int*	indxB );

// Dialog Data
	//{{AFX_DATA(TrimExtDlg)
	enum { IDD = IDD_TRIMEXT };
	CStatic	m_label4;
	CStatic	m_label3;
	CStatic	m_label1;
	CStatic	m_label2;
	CSpinButtonCtrl	m_spin2;
	CSpinButtonCtrl	m_spin1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TrimExtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	void draw_solutions();

	// Generated message map functions
	//{{AFX_MSG(TrimExtDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	bool CanDraw();

private:

	CViewBase* m_view;
	int m_solution1;
	int m_solution2;
	CGeoCurveArray* m_curvelist1;
	CGeoCurveArray* m_curvelist2;

	bool	m_initialized;

	int		m_top;
	int		m_left;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIMEXTDLG_H__E91F7C91_F7F9_4F95_959A_FECF5521CEDC__INCLUDED_)
