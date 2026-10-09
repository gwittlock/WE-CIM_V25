// Policy.h : main header file for the POLICY DLL
//

#if !defined(AFX_POLICY_H__93A45655_2BCE_42CD_B551_AB6DE4E2D755__INCLUDED_)
#define AFX_POLICY_H__93A45655_2BCE_42CD_B551_AB6DE4E2D755__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPolicyApp
// See Policy.cpp for the implementation of this class
//

class CPolicyApp : public CWinApp
{
public:
	CPolicyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolicyApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPolicyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLICY_H__93A45655_2BCE_42CD_B551_AB6DE4E2D755__INCLUDED_)
