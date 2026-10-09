
// EWM.cpp : Defines the class behaviors for the application.
//

// #include "stdafx.h"
#include "..\common\stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "EWM.h"
#include "MainFrm.h"

#include "EWMDoc.h"
#include "EWMView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Register.h"


// CEWMApp

BEGIN_MESSAGE_MAP(CEWMApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CEWMApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, &CEWMApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CEWMApp::OnFileOpen)
END_MESSAGE_MAP()


// CEWMApp construction

CEWMApp::CEWMApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("EWM.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_AppMutex = NULL;
	m_AppLock = NULL;
}

// The one and only CEWMApp object

CEWMApp theApp;


int	CEWMApp::GetAppType( int stringid )
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

// CEWMApp initialization

BOOL CEWMApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CEWMDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CEWMView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Not sure if this is the correct place for this ....

	// get profile name

	::GetWindowsDirectory( m_IniFile, sizeof( m_IniFile ) );
	// strcat( m_IniFile, "\\RcvrApp.ini" );
	::StrCat( m_IniFile, TEXT("\\RcvrApp.ini") );

	// decide which type of app we will be
	int dlgid = GetAppType( IDS_MUTEX_RECV );
	if( !dlgid )
	{
		TCHAR appname[MAX_PATH];
		GetModuleFileName( AfxGetInstanceHandle(), appname, sizeof( appname ) );
		return FALSE;
	}

	// So the CIpcSender instance knows this EWM is open.
	CRegister::SetRootPath( "Software\\WE-CIM\\22.0" );
	CRegister::IntSetV( "Debug", "ewmact", 1 );
	
	// extra initialization
	int top = CRegister::IntGetV( "Preferences\\Errors", "Top", 0 );
	int left = CRegister::IntGetV( "Preferences\\Errors", "Left", 0 );
	int height = CRegister::IntGetV( "Preferences\\Errors", "Height", 200 );
	int width = CRegister::IntGetV( "Preferences\\Errors", "Width", 500 );

	if (left < 0) left = 0;
	if (top < 0) top = 0;

	// SetWindowPos( &wndTopMost, left, top, width, height, 0 );
	m_pMainWnd->SetWindowPos( &CWnd::wndTop, left, top, width, height, 0 );
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Set the dialog caption.  This is critical to allowing the
	// posting application to find the window handle of this dialog
	//
	CString title;
	title.LoadString( IDS_RCVR_TITLE );
	m_pMainWnd->SetWindowText( title );

#if 0  // this untested snippet (for changing the default font) came from the web.
	void CTextEdView::OnInitialUpdate() 
	{ 
		LOGFONT lf; 
		CEditView::OnInitialUpdate(); 
		memset(&lf, 0, sizeof(LOGFONT)); 
		lf.lfHeight = 15; 
		strcpy(lf.lfFaceName, "courier new"); 
		pFont.CreateFontIndirect(&lf); 
		CTextEdView::GetEditCtrl().SetFont(&pFont);
	}
#endif

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

int CEWMApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	delete m_AppLock;
	delete m_AppMutex;

	return CWinApp::ExitInstance();
}

// CEWMApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CEWMApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CEWMApp message handlers


