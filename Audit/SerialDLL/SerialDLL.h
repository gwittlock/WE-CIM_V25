// SerialDLL.h : main header file for the SERIALDLL DLL
//

#if !defined(AFX_SERIALDLL_H__3595A773_65AC_4AF3_969B_DE2E6B7BA0EB__INCLUDED_)
#define AFX_SERIALDLL_H__3595A773_65AC_4AF3_969B_DE2E6B7BA0EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSerialDLLApp
// See SerialDLL.cpp for the implementation of this class
//

class CSerialDLLApp : public CWinApp
{
public:
	CSerialDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSerialDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALDLL_H__3595A773_65AC_4AF3_969B_DE2E6B7BA0EB__INCLUDED_)
