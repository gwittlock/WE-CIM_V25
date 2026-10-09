
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "..\common\stdafx.h"
#include "EWM.h"

// NOTE: It might be preferable to include EWMDoc.h in EWMView.h
// since it is required by class CEWMView to reference.
#include "EWMDoc.h"
#include "EWMView.h"

#include "MainFrm.h"

#include "Register.h"
#include "IpcSender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_COPYDATA()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// From http://forums.codeguru.com/showthread.php?284001-MFC-Dialog-How-to-enable-disable-the-Close-button-of-your-dialog-at-run-time
	// BOOL bEnable = TRUE;     // To enable
	BOOL bEnable = FALSE;    // To disable

	UINT menuf = bEnable ? (MF_BYCOMMAND) : (MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);

	CMenu* pSM = GetSystemMenu(FALSE);
	if(pSM)
		pSM->EnableMenuItem(SC_CLOSE, menuf);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers


BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// if size doesn't match we don't know what this is

	if( pCopyDataStruct->cbData == sizeof( MsgCmd ) )
	{
		MsgCmd msg;
		memcpy( &msg, pCopyDataStruct->lpData, sizeof( MsgCmd ) );

		// process message

		switch( msg.command )
		{
		case MSGCMD_TIME :
			break;

		case MSGCMD_EXIT :
			PostMessage(WM_CLOSE);
			break;

		case MSGCMD_TEXT :
			{
			CEWMView* view = (CEWMView*) GetActiveView();
			view->MessageDisplay( msg.text );
			break;
			}
		default :
			{
			CString buffer;
			buffer.Format( TEXT("Received Message Command %d"), msg.command );
			::MessageBox( NULL, buffer, TEXT("Error"), MB_OK );
			}
		}

		return TRUE;
	}

	return CFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}


void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// TODO: Add your message handler code here
	CRect rect;
	GetWindowRect( &rect );

	CRegister::IntSetV( "Preferences\\Errors", "Top", rect.top );
	CRegister::IntSetV( "Preferences\\Errors", "Left", rect.left );
	CRegister::IntSetV( "Preferences\\Errors", "Height", rect.Height() );
	CRegister::IntSetV( "Preferences\\Errors", "Width", rect.Width() );

	// So the CIpcSender instance knows this EWM is closed.
	CRegister::IntSetV( "Debug", "ewmact", 0 );
}
