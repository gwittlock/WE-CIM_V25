// LeadSupport.h : main header file for the LEADSUPPORT DLL
//

#if !defined(AFX_LEADSUPPORT_H__CA4F8D85_E090_11D3_B7F2_0020E0580B5B__INCLUDED_)
#define AFX_LEADSUPPORT_H__CA4F8D85_E090_11D3_B7F2_0020E0580B5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLeadSupportApp
// See LeadSupport.cpp for the implementation of this class
//

class CLeadSupportApp : public CWinApp
{
public:
	CLeadSupportApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeadSupportApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLeadSupportApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEADSUPPORT_H__CA4F8D85_E090_11D3_B7F2_0020E0580B5B__INCLUDED_)
