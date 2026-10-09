#if !defined(AFX_ANIMATEDIALOG_H__02F7EAD0_0638_49D1_BDB4_E0BD2A4CB4E3__INCLUDED_)
#define AFX_ANIMATEDIALOG_H__02F7EAD0_0638_49D1_BDB4_E0BD2A4CB4E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimateDialog.h : header file
//

#include "resource.h"

#include "ViewBase.h"

/////////////////////////////////////////////////////////////////////////////
// AnimateDialog dialog

class AnimateDialog : public CDialog
{
// Construction
public:
	AnimateDialog( int speed, CString& label );

	int Speed( void );
	BOOL Pause( void );
	BOOL FastNibble( void );
	BOOL ShowHits( void );

//	AnimateDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AnimateDialog)
	enum { IDD = IDD_ANIMATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AnimateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AnimateDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int				m_speed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATEDIALOG_H__02F7EAD0_0638_49D1_BDB4_E0BD2A4CB4E3__INCLUDED_)
