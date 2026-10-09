
// EWM.h : main header file for the EWM application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include <afxmt.h>  // for CMutex


// CEWMApp:
// See EWM.cpp for the implementation of this class
//

class CEWMApp : public CWinApp
{
public:
	CEWMApp();

	TCHAR	m_IniFile[MAX_PATH];

	CMutex* m_AppMutex;
	CSingleLock* m_AppLock;

	int		GetAppType( int stringid );

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEWMApp theApp;
