#if !defined(AFX_DATAWINDOW_H__0DBC78E3_A7D0_11D6_B650_0003B3008E71__INCLUDED_)
#define AFX_DATAWINDOW_H__0DBC78E3_A7D0_11D6_B650_0003B3008E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataWindow.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDataWindow window

class CDataWindow : public CEdit
{
// Construction
public:

	CDataWindow();

	void	DataDisplay( BYTE* data, int maxbuf, int base );

	// Indices are relative to start of displayed buffer.
	void	Highlight( int byteidx, int bitidx, int selBits );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDataWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDataWindow)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAWINDOW_H__0DBC78E3_A7D0_11D6_B650_0003B3008E71__INCLUDED_)
