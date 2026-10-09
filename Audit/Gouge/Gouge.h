// Gouge.h : main header file for the GOUGE DLL
//

#if !defined(AFX_GOUGE_H__AE2620C5_432D_11D3_919D_0040335A7818__INCLUDED_)
#define AFX_GOUGE_H__AE2620C5_432D_11D3_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGougeApp
// See Gouge.cpp for the implementation of this class
//

class CGougeApp : public CWinApp
{
public:
	CGougeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGougeApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGougeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOUGE_H__AE2620C5_432D_11D3_919D_0040335A7818__INCLUDED_)
