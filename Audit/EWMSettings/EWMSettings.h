// EWMSettings.h : main header file for the EWMSETTINGS application
//

#if !defined(AFX_EWMSETTINGS_H__3A33EF99_71C9_44C3_95F5_AE379174B712__INCLUDED_)
#define AFX_EWMSETTINGS_H__3A33EF99_71C9_44C3_95F5_AE379174B712__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEWMSettingsApp:
// See EWMSettings.cpp for the implementation of this class
//

class CEWMSettingsApp : public CWinApp
{
public:
	CEWMSettingsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEWMSettingsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEWMSettingsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EWMSETTINGS_H__3A33EF99_71C9_44C3_95F5_AE379174B712__INCLUDED_)
