
#include "StdAfx.h"
#include "Resource.h"
#include "Register.h"
#include "RcvrApp.h"
#include "RecvDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
// CIpcRecvDlg dialog

CIpcRecvDlg::CIpcRecvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIpcRecvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIpcRecvDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIpcRecvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIpcRecvDlg)
	DDX_Control(pDX, IDC_LIST1,	m_msgs);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIpcRecvDlg, CDialog)
	//{{AFX_MSG_MAP(CIpcRecvDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CIpcRecvDlg message handlers

BOOL CIpcRecvDlg::OnInitDialog()
{
	int top, left, height, width;

	CDialog::OnInitDialog();

	// Set the icon for this dialog.
	// The framework does this automatically when
	// when the app's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// extra initialization
	top = CRegister::IntGetV( "Preferences\\Errors", "Top", 0 );
	left = CRegister::IntGetV( "Preferences\\Errors", "Left", 0 );
	height = CRegister::IntGetV( "Preferences\\Errors", "Height", 200 );
	width = CRegister::IntGetV( "Preferences\\Errors", "Width", 500 );

	if (left < 0) left = 0;
	if (top < 0) top = 0;

	// SetWindowPos( &wndTopMost, left, top, width, height, 0 );
	SetWindowPos( &wndTop, left, top, width, height, 0 );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Set the dialog caption.  This is critical to allowing the
	// posting application to find the window handle of this dialog
	//
	CString title;
	title.LoadString( IDS_RCVR_TITLE );
	SetWindowText( title );

#define REQUIRED 0
#if REQUIRED
	CFont cFont ;
	LOGFONT lFont ;

	memset((void*)&lFont, 0, sizeof(lFont)) ;
	//_tcscpy(lFont.lfFaceName, _T("Times New Roman")) ;
	//_tcscpy(lFont.lfFaceName, _T("Courier New")) ;

	// lFont.lfHeight = 18 ;
	// lFont.lfWidth = 18 ;
	lFont.lfHeight = 0 ;
	lFont.lfWidth = 0 ;
	lFont.lfWeight = FW_THIN ;
	// lFont.lfCharSet = DEFAULT_CHARSET;  // ANSI_CHARSET ;
	lFont.lfCharSet = ANSI_CHARSET ;


	// lFont.lfOutPrecision = OUT_TT_PRECIS;
	lFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lFont.lfQuality = PROOF_QUALITY;
	// lFont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
	lFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;

	cFont.CreateFontIndirect(&lFont) ;
	m_msgs.SetFont(&cFont) ;
#endif

	return TRUE;
}


// If you add a minimize button to your dialog,
// you will need the code below to draw the icon.  

void CIpcRecvDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display
// while the user drags the minimized window.
HCURSOR CIpcRecvDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CIpcRecvDlg::OnClose() 
{
	// trap alt+f4
	TRACE( "CIpcRecvDlg::OnClose\n" );

	CDialog::OnClose();
}


BOOL CIpcRecvDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pData) 
{
	// if size doesn't match we don't know what this is

	if( pData->cbData == sizeof( MsgCmd ) )
	{
		MsgCmd msg;
		memcpy( &msg, pData->lpData, sizeof( MsgCmd ) );

		// process message

		switch( msg.command )
		{
		case MSGCMD_TIME :
			break;

		case MSGCMD_EXIT :
			{
			EndDialog( IDOK );
			break;
			}
		case MSGCMD_TEXT :
			{
			MessageDisplay( msg.text );
			break;
			}
		default :
			{
			char buffer[64];
			sprintf_s( buffer, 64, "Received Message Command %d", msg.command );
			MessageBox( buffer, "Error" );
			}
		}

		return TRUE;
	}

	return CDialog::OnCopyData(pWnd, pData);
}

void
CIpcRecvDlg::MessageDisplay( const char* text )
{
	if (m_msgs.GetCount() == 0)
	{
		// TODO: The horizontal extent of scrolling should be dynamically
		// calculated and tracked using the longest string encountered
		// and the current font but at this time any horizontal scroll
		// bar is better than none.
		CRect rect;
		m_msgs.GetClientRect( &rect );
		m_msgs.SetHorizontalExtent( 2*rect.Width() );
	}

	// ShowWindow( SW_SHOW );
	BringWindowToTop();

	CString msg( text );
	int idx = m_msgs.AddMsg( msg );
	m_msgs.SetCurSel( idx );
}

void CIpcRecvDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CListBox* listBox = (CListBox*) GetDlgItem( IDC_LIST1 );
	if (listBox != NULL)
	{
		CRect dialogRect;
		CRect listRect;

		GetClientRect( &dialogRect );
		m_msgs.GetClientRect( &listRect );

		m_msgs.SetWindowPos( &wndTop, listRect.left, listRect.top,
			dialogRect.Width() - 18, dialogRect.Height() - 18, SWP_NOMOVE );
	}
}

void CIpcRecvDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	CRect rect;
	GetWindowRect( &rect );

	CRegister::IntSetV( "Preferences\\Errors", "Top", rect.top );
	CRegister::IntSetV( "Preferences\\Errors", "Left", rect.left );
	CRegister::IntSetV( "Preferences\\Errors", "Height", rect.Height() );
	CRegister::IntSetV( "Preferences\\Errors", "Width", rect.Width() );
}
