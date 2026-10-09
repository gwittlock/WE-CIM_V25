// NestSupport.h : main header file for the NESTSUPPORT DLL
//

#if !defined(AFX_NESTSUPPORT_H__6CD8DE65_DAEE_11D3_B7F2_0020E0580B5B__INCLUDED_)
#define AFX_NESTSUPPORT_H__6CD8DE65_DAEE_11D3_B7F2_0020E0580B5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNestSupportApp
// See NestSupport.cpp for the implementation of this class
//

class CNestSupportApp : public CWinApp
{
public:
	CNestSupportApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNestSupportApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNestSupportApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NESTSUPPORT_H__6CD8DE65_DAEE_11D3_B7F2_0020E0580B5B__INCLUDED_)
