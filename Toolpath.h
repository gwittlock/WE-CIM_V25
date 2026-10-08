// Toolpath.h : main header file for the TOOLPATH DLL
//

#if !defined(AFX_TOOLPATH_H__55E2219D_27C5_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_TOOLPATH_H__55E2219D_27C5_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CToolpathApp
// See Toolpath.cpp for the implementation of this class
//

class CToolpathApp : public CWinApp
{
public:
	CToolpathApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolpathApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CToolpathApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLPATH_H__55E2219D_27C5_11D3_B7F1_000039A6570C__INCLUDED_)
