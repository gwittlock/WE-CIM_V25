#if !defined(AFX_READOUT_H__0DBC78E2_A7D0_11D6_B650_0003B3008E71__INCLUDED_)
#define AFX_READOUT_H__0DBC78E2_A7D0_11D6_B650_0003B3008E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Readout.h : header file
//

enum EReadoutType
{
	EBYTE,
	ESHORT,
	EINTEGER,
	ELONG,
	EHANDLE
};


/////////////////////////////////////////////////////////////////////////////
// CReadout window

class CReadout : public CEdit
{
// Construction
public:

	CReadout();

// Attributes
public:

	void TypeSet( EReadoutType type );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadout)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReadout();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReadout)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	EReadoutType	m_type;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READOUT_H__0DBC78E2_A7D0_11D6_B650_0003B3008E71__INCLUDED_)
