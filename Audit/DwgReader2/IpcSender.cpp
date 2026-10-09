
#include "StdAfx.h"
#include "cmn_resource.h"
#include "Path.h"
#include "IpcSender.h"

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


CALLBACK CheckWindowTitle( HWND hwnd, LPARAM lParam )
{
	char	buffer[MAX_PATH];
	GetWindowText( hwnd, buffer, sizeof( buffer ) );

	FindWnd * fw = (FindWnd *)lParam;
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
	: m_hWndRecv( 0 )
{
}

CIpcSender::~CIpcSender() 
{
	if (m_hWndRecv != 0)
		SendMsg( MSGCMD_EXIT, "" );  // terminate receiver
}

int
CIpcSender::SendMsg( int command, const CString& text )
{
	COPYDATASTRUCT	cds;
	MsgCmd msg;

	int status = 0;

	msg.command = command;
	if(command != MSGCMD_TIME)
	{
		strncpy( msg.text, text, MSGCMD_TEXTSIZE );
		msg.text[MSGCMD_TEXTSIZE] = '\0';  // just in case ....
	}

	cds.dwData = 0;
	cds.cbData = sizeof( msg );
	cds.lpData = &msg;

	if (m_hWndRecv == 0)
	{
		// Assume first attempt, or failed attempt with previous message.

		m_hWndRecv = Init();
	}

	if (m_hWndRecv != 0)
	{
		// Assume we have a handle to a valid window.

		status = SendMessage( m_hWndRecv, WM_COPYDATA, (WPARAM) 0, (LPARAM) &cds );
		if (status == 0)
		{
			// Assume failure because the receiving window was closed.
			// Re-init and try again.

			// DWORD errorCode = GetLastError();  // doesn't appear to work for this usage.

			m_hWndRecv = Init();
			status = SendMessage( m_hWndRecv, WM_COPYDATA, (WPARAM) 0, (LPARAM) &cds );
		}
	}

	return status;
}

HWND
CIpcSender::Init()
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
	CString exe = path.DriveDir() + "\\rcvr.exe";

	BOOL bTest = CreateProcess(
						exe,		// appname,	// executable image
						NULL,		// command line
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

	return ( FindWinTitle( rcvtitle ) ) ;  // 0 indicates failure.
}

