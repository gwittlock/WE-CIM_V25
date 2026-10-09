// PortalDLL.h : main header file for the PORTALDLL DLL
//

#if !defined(AFX_PORTALDLL_H__8B3AEE65_020A_11D3_B7F0_80100AC10071__INCLUDED_)
#define AFX_PORTALDLL_H__8B3AEE65_020A_11D3_B7F0_80100AC10071__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPortalDLLApp
// See PortalDLL.cpp for the implementation of this class
//

class CPortalDLLApp : public CWinApp
{
public:
	CPortalDLLApp();
	~CPortalDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPortalDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPortalDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PORTALDLL_H__8B3AEE65_020A_11D3_B7F0_80100AC10071__INCLUDED_)
