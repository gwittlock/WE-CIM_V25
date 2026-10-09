// DwgReader2.h : main header file for the DWGREADER2 application
//

#if !defined(AFX_DWGREADER2_H__98727AA4_8DD8_11D6_B650_0003B3008E71__INCLUDED_)
#define AFX_DWGREADER2_H__98727AA4_8DD8_11D6_B650_0003B3008E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDwgReader2App:
// See DwgReader2.cpp for the implementation of this class
//

class CDwgReader2App : public CWinApp
{
public:
	CDwgReader2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDwgReader2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDwgReader2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWGREADER2_H__98727AA4_8DD8_11D6_B650_0003B3008E71__INCLUDED_)
