#if !defined(AFX_MSGLISTBOX_H__223D85C1_CCF8_11D4_919D_0040335A7818__INCLUDED_)
#define AFX_MSGLISTBOX_H__223D85C1_CCF8_11D4_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgListBox window

class CMsgListBox : public CListBox
{
// Construction
public:

	CMsgListBox();

// Attributes
public:

// Operations
public:

	int AddMsg( const CString& msg );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgListBox)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMsgListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgListBox)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPopmenuClear();
	afx_msg void OnPopmenuSetmaxmsgs();
	afx_msg void OnPopmenuPrint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int		m_maxmsgs;
	CString	m_dumppath;
	CFile	m_dumpfile;
	bool	m_dump;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGLISTBOX_H__223D85C1_CCF8_11D4_919D_0040335A7818__INCLUDED_)
