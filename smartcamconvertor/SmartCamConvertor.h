// SmartCamConvertor.h : main header file for the SMARTCAMCONVERTOR application
//

#if !defined(AFX_SMARTCAMCONVERTOR_H__BAEC0F84_118A_11D6_919E_0040335A7818__INCLUDED_)
#define AFX_SMARTCAMCONVERTOR_H__BAEC0F84_118A_11D6_919E_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSmartCamConvertorApp:
// See SmartCamConvertor.cpp for the implementation of this class
//

class CSmartCamConvertorApp : public CWinApp
{
public:

	CSmartCamConvertorApp();

	void ConvertorSet();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartCamConvertorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSmartCamConvertorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMARTCAMCONVERTOR_H__BAEC0F84_118A_11D6_919E_0040335A7818__INCLUDED_)
