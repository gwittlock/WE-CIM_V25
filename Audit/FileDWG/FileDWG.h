// FileDWG.h : main header file for the FILEDWG application
//

#if !defined(AFX_FILEDWG_H__B846D624_7A9A_11D5_919D_0040335A7818__INCLUDED_)
#define AFX_FILEDWG_H__B846D624_7A9A_11D5_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFileDWGApp:
// See FileDWG.cpp for the implementation of this class
//

class CFileDWGApp : public CWinApp
{
public:
	CFileDWGApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileDWGApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFileDWGApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDWG_H__B846D624_7A9A_11D5_919D_0040335A7818__INCLUDED_)
