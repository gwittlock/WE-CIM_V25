
#include "StdAfx.h"
#include "cmn_resource.h"
#include "Register.h"
#include "Path.h"
#include "IpcSender.h"

#include "portable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
// helper functions

typedef struct
{
	HWND hwnd;
	const char* title;
} FindWnd;


int CALLBACK CheckWindowTitle( HWND hwnd, LPARAM lParam )
{
	char buffer[MAX_PATH];
	GetWindowText( hwnd, buffer, sizeof( buffer ) );

	FindWnd* fw = (FindWnd *)lParam;
	if( strcmp( buffer, fw->title ) == 0 )
	{
		fw->hwnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

HWND FindWinTitle( const char* title )
{
	FindWnd	fw;
	fw.hwnd = 0;
	fw.title = title;
	
	EnumWindows( (WNDENUMPROC) CheckWindowTitle, (LPARAM) &fw );
	return fw.hwnd;
}


///////////////////////////////////////////////////////////////////////

CIpcSender::CIpcSender()
{
	m_hWndRecv = 0;
}

CIpcSender::~CIpcSender() 
{
	ASSERT( !IsInitialized() );
}

// Returns: (true) success / (false) failure.
bool CIpcSender::Initialize()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( !IsInitialized() )
	{
		STARTUPINFO	sui;
		PROCESS_INFORMATION pi;
		int size;

		// start up a receiver
		char appname[MAX_PATH];
		GetModuleFileName( AfxGetInstanceHandle(), appname, sizeof( appname ) );

		size = sizeof( sui );
		memset( & sui, 0, size );
		sui.cb = size;

		size = sizeof( pi );
		memset( & pi, 0, size );

		CPath path( appname );
		CString folder = path.DriveDir();

		int indx = folder.Find( "\\dlls" );
		if (indx > 0)
			folder = folder.Left( indx );

		CString exe = folder + "\\ewm.exe";

		const char* reg_root = CRegister::RootPathV();

		BOOL bTest = CreateProcess(
			exe,		// appname,	// executable image
			(char*) reg_root,	// command line (so app knows where to look)
			NULL,		// process security
			NULL,		// thread security
			FALSE,		// inherit handles
			0,			// creation flags
			NULL,		// environment block
			NULL,		// current directory
			&sui,		// startup info
			&pi );		// process info

		if( !bTest )
			return ((HWND) 0);  // Failed.

		// wait for it to start up
		if( WaitForInputIdle( pi.hProcess, 5000 ) != 0 )
			return ((HWND) 0);	// failed

		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		// Get a handle to the receiver.  This is critical to the counterpart
		// code segment found in CIpcRecvDlg::OnInitDialog() (ipc\RecvDlg.cpp)
		//
		CString rcvtitle;
		rcvtitle.LoadString( IDS_RCVR_TITLE );

		m_hWndRecv = FindWinTitle( rcvtitle );  // 0 indicates failure.

		CRegister::IntSetV( "Debug", "ewmact", 1 );
	}

	return IsInitialized();
}

void CIpcSender::Terminate() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( IsInitialized() )
	{
		bool is_open = (CRegister::Debug("ewmact") > 0);
		if ( is_open )
		{
			SendMsg( MSGCMD_EXIT, "" );  // terminate EWM window
			m_hWndRecv = 0;
		}
	}
}

int CIpcSender::SendMsg( int command, const char* text )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int status = 0;

	if (IsInitialized() && (text != NULL))
	{
		MsgCmd msg;

		msg.command = command;
		if(command != MSGCMD_TIME)
		{
			strncpy_s( msg.text, MSGCMD_TEXTSIZE, text, MSGCMD_TEXTSIZE-1 );
			msg.text[MSGCMD_TEXTSIZE] = '\0';  // just in case ....
		}

		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.cbData = sizeof( msg );
		cds.lpData = &msg;

		status = SendMessage( m_hWndRecv, WM_COPYDATA, (WPARAM) 0, (LPARAM) &cds );
	}

	return status;
}

