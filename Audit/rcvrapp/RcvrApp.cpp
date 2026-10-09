
#include "StdAfx.h"
#include "resource.h"		// main symbols

#define MAIN
#include "RcvrApp.h"
#include "RecvDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Register.h"


///////////////////////////////////////////////////////////////////////
// CRcvrApp

BEGIN_MESSAGE_MAP(CRcvrApp, CWinApp)
	//{{AFX_MSG_MAP(CRcvrApp)
	//}}AFX_MSG
END_MESSAGE_MAP()


// CRcvrApp construction

CRcvrApp::CRcvrApp()
{
	// Place all significant initialization in InitInstance

	m_AppMutex = NULL;
	m_AppLock = NULL;
}


int	CRcvrApp::GetAppType( int stringid )
{
	CString mtxname;
	mtxname.LoadString( stringid );

//CString msg;
//msg.Format( "id <%d>  string <%s>", stringid, mtxname );
//MessageBox( NULL, msg, "Error", MB_OK );

	m_AppMutex = new CMutex( FALSE, mtxname );
	m_AppLock = new CSingleLock( m_AppMutex );
	if( m_AppLock->Lock( 0 ) )
		return TRUE;	// we have the lock

	// lock failed

	delete m_AppLock;
	delete m_AppMutex;
	return 0;
}


// CRcvrApp initialization

BOOL CRcvrApp::InitInstance()
{
	// Standard initialization

	Enable3dControls();		// using MFC in a shared DLL

	// get profile name

	GetWindowsDirectory( m_IniFile, sizeof( m_IniFile ) );
	strcat( m_IniFile, "\\RcvrApp.ini" );

	// decide which type of app we will be
	int dlgid = GetAppType( IDS_MUTEX_RECV );
	if( !dlgid )
	{
		char appname[MAX_PATH];
		GetModuleFileName( AfxGetInstanceHandle(), appname, sizeof( appname ) );

// Annoying error!  Doesn't matter! eww
//		CString msg;
//		msg.Format( "%s failed to initialize.", appname );
//		MessageBox( NULL, msg, "Error", MB_OK );
		return FALSE;
	}

	// CRITICAL! See also CIpcSender::Init()
	CRegister::SetRootPath( __argv[0] );

	CIpcRecvDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	delete m_AppLock;
	delete m_AppMutex;

	return FALSE;
}

