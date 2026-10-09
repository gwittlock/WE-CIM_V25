// EWMSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EWMSettings.h"
#include "EWMSettingsDlg.h"

#include "Register.h"
#include "ewmconst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEWMSettingsDlg dialog

CEWMSettingsDlg::CEWMSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEWMSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEWMSettingsDlg)
	m_check_dao = FALSE;
	m_check_diagnostics = FALSE;
	m_check_enable = FALSE;
	m_check_file = FALSE;
	m_check_filter = FALSE;
	m_check_nesting = FALSE;
	m_check_portal = FALSE;
	m_check_seeds = FALSE;
	m_check_ui = FALSE;
	m_check_user = FALSE;
	m_check_warnings = FALSE;
	m_check_internal = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEWMSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEWMSettingsDlg)
	DDX_Check(pDX, IDC_CHECK_DAO, m_check_dao);
	DDX_Check(pDX, IDC_CHECK_DIAGNOSTICS, m_check_diagnostics);
	DDX_Check(pDX, IDC_CHECK_ENABLE, m_check_enable);
	DDX_Check(pDX, IDC_CHECK_FILE, m_check_file);
	DDX_Check(pDX, IDC_CHECK_FILTER, m_check_filter);
	DDX_Check(pDX, IDC_CHECK_NESTING, m_check_nesting);
	DDX_Check(pDX, IDC_CHECK_PORTAL, m_check_portal);
	DDX_Check(pDX, IDC_CHECK_SEEDS, m_check_seeds);
	DDX_Check(pDX, IDC_CHECK_UI, m_check_ui);
	DDX_Check(pDX, IDC_CHECK_USER, m_check_user);
	DDX_Check(pDX, IDC_CHECK_WARNINGS, m_check_warnings);
	DDX_Check(pDX, IDC_CHECK_INTERNAL, m_check_internal);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEWMSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CEWMSettingsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BUTTON_UPDATE, &CEWMSettingsDlg::OnBnClickedButtonUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEWMSettingsDlg message handlers

BOOL CEWMSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CRegister::SetRootPath( "Software\\WE-CIM\\22.0" );
	int ewmbits = CRegister::IntGetV( "Debug", "ewmbits", 0 );

	m_check_enable      = ((ewmbits & EWM_ACTIVE) != 0);
	m_check_filter      = ((ewmbits & EWM_FILTERED) != 0);
	m_check_warnings    = ((ewmbits & EWM_WARNING) != 0);
	m_check_user        = ((ewmbits & EWM_USER) != 0);
	m_check_internal    = ((ewmbits & EWM_INTERNAL) != 0);
	m_check_diagnostics = ((ewmbits & EWM_DIAGNOSTIC) != 0);
	m_check_portal      = ((ewmbits & EWM_PORTAL) != 0);
	m_check_ui          = ((ewmbits & EWM_UI) != 0);
	m_check_dao         = ((ewmbits & EWM_DAO) != 0);
	m_check_file        = ((ewmbits & EWM_FILE) != 0);
	m_check_nesting     = ((ewmbits & EWM_NESTING) != 0);
	m_check_seeds       = ((ewmbits & EWM_SEED) != 0);

	UpdateData( FALSE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEWMSettingsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEWMSettingsDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEWMSettingsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEWMSettingsDlg::OnBnClickedButtonUpdate()
{
	int ewmbits = 0;

	UpdateData( TRUE );

	if ( m_check_enable )
	{
		ewmbits = EWM_ACTIVE;

		if ( m_check_filter )
			ewmbits |= EWM_FILTERED;

		if ( m_check_warnings )
			ewmbits |= EWM_WARNING;

		if ( m_check_user )
			ewmbits |= EWM_USER;

		if ( m_check_internal )
			ewmbits |= EWM_INTERNAL;

		if ( m_check_diagnostics )
			ewmbits |= EWM_DIAGNOSTIC;

		if ( m_check_portal )
			ewmbits |= EWM_PORTAL;

		if ( m_check_ui )
			ewmbits |= EWM_UI;

		if ( m_check_dao )
			ewmbits |= EWM_DAO;

		if ( m_check_file )
			ewmbits |= EWM_FILE;

		if ( m_check_nesting )
			ewmbits |= EWM_NESTING;

		if ( m_check_seeds )
			ewmbits |= EWM_SEED;
	}

	CRegister::IntSetV( "Debug", "ewmbits", ewmbits );
}
