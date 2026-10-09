// RepoSupport.h : main header file for the REPOSUPPORT DLL
//

#if !defined(AFX_REPOSUPPORT_H__39D566C5_4121_11D4_B7F2_0020E0580B5B__INCLUDED_)
#define AFX_REPOSUPPORT_H__39D566C5_4121_11D4_B7F2_0020E0580B5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRepoSupportApp
// See RepoSupport.cpp for the implementation of this class
//

class CRepoSupportApp : public CWinApp
{
public:
	CRepoSupportApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepoSupportApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CRepoSupportApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPOSUPPORT_H__39D566C5_4121_11D4_B7F2_0020E0580B5B__INCLUDED_)
