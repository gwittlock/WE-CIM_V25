#if !defined(AFX_DLGVIEWOPTIONS_H__108B5B75_E2BD_4467_8070_7066EA021996__INCLUDED_)
#define AFX_DLGVIEWOPTIONS_H__108B5B75_E2BD_4467_8070_7066EA021996__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgViewOptions.h : header file
//

#include "resource.h"
#include "ViewMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CdlgViewOptions dialog

class CdlgViewOptions : public CDialog
{
// Construction
public:
	CdlgViewOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgViewOptions)
	enum { IDD = IDD_VIEW_OPTIONS };
	int m_tool_dash;
	int m_text;
	int m_stock;
	int m_nibble;
	int m_prof_marker;
	double m_grid;
	int m_gdi;
	int m_mono;
	int m_white;
	int m_picktol;
	int  m_profscale;
	//}}AFX_DATA

	void ReadDefaults();
	void SetViewMode( const CViewMgr& view );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgViewOptions)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgViewOptions)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CdlgViewOptions)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVIEWOPTIONS_H__108B5B75_E2BD_4467_8070_7066EA021996__INCLUDED_)
