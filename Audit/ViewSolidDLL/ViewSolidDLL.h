// ViewSolidDLL.h : main header file for the VIEWSOLIDDLL DLL
//

#if !defined(AFX_VIEWSOLIDDLL_H__1C66ADC5_3A8C_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_VIEWSOLIDDLL_H__1C66ADC5_3A8C_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "ViewSolid.h"

/////////////////////////////////////////////////////////////////////////////
// CViewSolidDLLApp
// See ViewSolidDLL.cpp for the implementation of this class
//

class CViewSolidDLLApp : public CWinApp
{
public:
	CViewSolidDLLApp();

	static CViewBase	dllExport *NewViewSolid( ID in_id );
	static void			dllExport DeleteViewSolid( CViewBase* in_view );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewSolidDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CViewSolidDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWSOLIDDLL_H__1C66ADC5_3A8C_11D3_B7F1_000039A6570C__INCLUDED_)
