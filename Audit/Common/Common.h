// Common.h : main header file for the COMMON DLL
//

#if !defined(AFX_COMMON_H__3F1FC325_1984_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_COMMON_H__3F1FC325_1984_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "cmn_resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCommonApp
// See Common.cpp for the implementation of this class
//

class CCommonApp : public CWinApp
{
public:
	CCommonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCommonApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMON_H__3F1FC325_1984_11D3_B7F1_000039A6570C__INCLUDED_)
