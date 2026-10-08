#if !defined(AFX_OFFSETWINDOW_H__CB6DBCC1_AA3A_11D6_B650_0003B3008E71__INCLUDED_)
#define AFX_OFFSETWINDOW_H__CB6DBCC1_AA3A_11D6_B650_0003B3008E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OffsetWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COffsetWindow window

class COffsetWindow : public CEdit
{
// Construction
public:

	COffsetWindow();

	void	OffsetDisplay( int base );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COffsetWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COffsetWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(COffsetWindow)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OFFSETWINDOW_H__CB6DBCC1_AA3A_11D6_B650_0003B3008E71__INCLUDED_)
