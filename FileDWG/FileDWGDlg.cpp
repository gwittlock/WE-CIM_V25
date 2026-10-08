// FileDWGDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Path.h"
#include "DaoDB.h"
#include "DaoQuery.h"
#include "FileDWG.h"
#include "FileDWGDlg.h"

#include "Register.h"
#include "DwgThing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE DWG_FILTER[] = "DWG Files (*.dwg)|*.dwg||";
static char BASED_CODE ANL_FILTER[] = "ANL Files (*.anl)|*.anl||";
static char BASED_CODE MM2_FILTER[] = "MM2 Files (*.mm2)|*.mm2||";
static char BASED_CODE MDB_FILTER[] = "MDB Files (*.mdb)|*.mdb||";
static DWORD FFLAGS = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

CString REG_KEY("\\Preferences\\Debugging\\dwg");

CString AppDir();


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
// CFileDWGDlg dialog

CFileDWGDlg::CFileDWGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileDWGDlg::IDD, pParent),
	  m_dwgConvertor( NULL )
{
	//{{AFX_DATA_INIT(CFileDWGDlg)
	m_analyze = FALSE;
	m_convert = FALSE;
	m_anlPath = _T("");
	m_cmdbPath = _T("");
	m_dwgPath = _T("");
	m_mm2Path = _T("");
	m_toolSetupID = 1;
	m_layerSetupID = 1;
	m_materialID = 1;
	m_messages = FALSE;
	m_stripLayers = FALSE;
	m_rotation = 0.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileDWGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileDWGDlg)
	DDX_Control(pDX, IDC_TOOL_SETUP, m_toolSetupCombo);
	DDX_Control(pDX, IDC_MATERIAL, m_materialCombo);
	DDX_Control(pDX, IDC_LAYER_SETUP, m_layerSetupCombo);
	DDX_Check(pDX, IDC_ANALYZE, m_analyze);
	DDX_Check(pDX, IDC_CONVERT, m_convert);
	DDX_Text(pDX, IDC_ANL_PATH, m_anlPath);
	DDX_Text(pDX, IDC_CMDB_PATH, m_cmdbPath);
	DDX_Text(pDX, IDC_DWG_PATH, m_dwgPath);
	DDX_Text(pDX, IDC_MM2_PATH, m_mm2Path);
	DDX_Check(pDX, IDC_MESSAGES, m_messages);
	DDX_Check(pDX, IDC_STRIP_LAYERS, m_stripLayers);
	DDX_Text(pDX, IDC_ROTATION, m_rotation);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileDWGDlg, CDialog)
	//{{AFX_MSG_MAP(CFileDWGDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ANL_BROWSE, OnAnlBrowse)
	ON_BN_CLICKED(IDC_CMDB_BROWSE, OnCmdbBrowse)
	ON_BN_CLICKED(IDC_DWG_BROWSE, OnDwgBrowse)
	ON_BN_CLICKED(IDC_MM2_BROWSE, OnMm2Browse)
	ON_BN_CLICKED(IDC_ANALYZE, OnAnalyze)
	ON_BN_CLICKED(IDC_CONVERT, OnConvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileDWGDlg message handlers

BOOL CFileDWGDlg::OnInitDialog()
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

	InitVars();

	InitCMDBvars( m_cmdbPath );

	m_analyze = FALSE;
	m_convert = (!m_analyze);

	UpdateData( FALSE );

	DimmingUpdate();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileDWGDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileDWGDlg::OnPaint() 
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
HCURSOR CFileDWGDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFileDWGDlg::OnAnlBrowse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_anlPath, FFLAGS, ANL_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_anlPath = dlg.GetPathName();
		UpdateData( FALSE );
	}
}

void CFileDWGDlg::OnCmdbBrowse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_cmdbPath, FFLAGS, MDB_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_cmdbPath = dlg.GetPathName();

		InitCMDBvars( m_cmdbPath );

		UpdateData( FALSE );
	}
}

void CFileDWGDlg::OnDwgBrowse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_dwgPath, FFLAGS, DWG_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_dwgPath = dlg.GetPathName();
		UpdateData( FALSE );
	}
}

void CFileDWGDlg::OnMm2Browse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_mm2Path, FFLAGS, MM2_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_mm2Path = dlg.GetPathName();
		UpdateData( FALSE );
	}
}

void CFileDWGDlg::OnAnalyze() 
{
	CButton* convertCheckBox = (CButton*) GetDlgItem( IDC_CONVERT );
	// CButton* scanCheckBox = (CButton*) GetDlgItem( IDC_SCAN );

	convertCheckBox->SetCheck( 0 );
	// scanCheckBox->SetCheck( 0 );

	DimmingUpdate();
}

void CFileDWGDlg::OnConvert() 
{
	CButton* analyzeCheckBox = (CButton*) GetDlgItem( IDC_ANALYZE );
	// CButton* scanCheckBox = (CButton*) GetDlgItem( IDC_SCAN );

	analyzeCheckBox->SetCheck( 0 );
	// scanCheckBox->SetCheck( 0 );

	InitCMDBvars( m_cmdbPath );

	DimmingUpdate();
}

void CFileDWGDlg::DimmingUpdate()
{
	CButton* analyzeCheckBox = (CButton*) GetDlgItem( IDC_ANALYZE );
	CButton* convertCheckBox = (CButton*) GetDlgItem( IDC_CONVERT );

	BOOL analyzeEnabled = (analyzeCheckBox->GetCheck() == 1);
	BOOL convertEnabled = (convertCheckBox->GetCheck() == 1);

	GetDlgItem( IDC_ANL_PATH )->EnableWindow( analyzeEnabled );

	GetDlgItem( IDC_MM2_PATH )->EnableWindow( convertEnabled );
	GetDlgItem( IDC_CMDB_PATH )->EnableWindow( convertEnabled );
	GetDlgItem( IDC_TOOL_SETUP )->EnableWindow( convertEnabled );
	GetDlgItem( IDC_LAYER_SETUP )->EnableWindow( convertEnabled );
	GetDlgItem( IDC_MATERIAL )->EnableWindow( convertEnabled );
}

void CFileDWGDlg::OnOK() 
{
	CReturn status;
	CString msg;

	UpdateData( TRUE );

	GetDlgItem( IDC_CMDB_BROWSE )->EnableWindow( FALSE );
	GetDlgItem( IDC_DWG_BROWSE )->EnableWindow( FALSE );
	GetDlgItem( IDC_MM2_BROWSE )->EnableWindow( FALSE );

	if ( m_messages )
	{
		CReturn::ErrorLevel( WARN_DIAGNOSTIC );
	}
	m_dwgConvertor->MsgsActivate( m_messages );

	if ( m_analyze )
	{
		SaveVars();

		status = m_dwgConvertor->Analyze( m_dwgPath, m_anlPath );
	}
	else if ( m_convert )
	{
		IDsInit();

		SaveVars();

		//m_dwgConvertor->DumpsActivate( m_dumps );
		//m_dwgConvertor->DumpPathSet( m_dumpPath );

		status = m_dwgConvertor->Read(
									m_dwgPath,
									m_cmdbPath,
									m_toolSetupID,
									m_layerSetupID,
									m_materialID,
									FALSE,
									FALSE,
									m_rotation,
									m_stripLayers );

		if ( status.IsOk() )
			status = m_dwgConvertor->ModelWrite( m_mm2Path );
	}

	GetDlgItem( IDC_CMDB_BROWSE )->EnableWindow( TRUE );
	GetDlgItem( IDC_DWG_BROWSE )->EnableWindow( TRUE );
	GetDlgItem( IDC_MM2_BROWSE )->EnableWindow( TRUE );

	if ( m_messages )
	{
		msg.Format( "Completion status: %s", (status.IsOk() ? "succeeded" : "failed") );
		MessageBox( msg, "Completion status", MB_OK );
	}
}

void CFileDWGDlg::InitVars()
{
	CString	value;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	m_dwgPath.Empty();

	m_dwgPath = CRegister::StringGet( CRegister::RootPathV( REG_KEY ), "dwgPath", "" );

	if ( m_dwgPath.IsEmpty() )
	{
		m_dwgPath = CRegister::StringGet(
			CRegister::RootPathV("\\Fabrication\\FileOpen"), "LastDirectory", "" ) + "*.dwg";
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	m_mm2Path = CRegister::StringGet(
		CRegister::RootPathV( REG_KEY ), "mm2Path", "" );

	if ( m_mm2Path.IsEmpty() )
	{
		m_mm2Path = CRegister::StringGet(
			CRegister::RootPathV("\\Fabrication\\FileOpen"), "LastDirectory", "" ) + "*.mm2";
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	m_cmdbPath = CRegister::StringGet(
		CRegister::RootPathV( REG_KEY ), "cmdbPath", "" );

	if ( m_cmdbPath.IsEmpty() )
	{
		m_cmdbPath = CRegister::StringGet(
				CRegister::RootPathV("\\ConfigurationManager"), "Database", "" );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	m_anlPath = AppDir() + "\\temp\\*.anl";

	m_lastToolSetup = CRegister::StringGet(
		CRegister::RootPathV( REG_KEY ), "toolSetup", "" );

	m_lastLayerSetup = CRegister::StringGet(
		CRegister::RootPathV( REG_KEY ), "layerSetup", "" );

	m_lastMaterial = CRegister::StringGet(
		CRegister::RootPathV( REG_KEY ), "material", "" );
}

void CFileDWGDlg::SaveVars()
{
	CRegister::StringSet(
		CRegister::RootPathV( REG_KEY ), "dwgPath", m_dwgPath );

	CRegister::StringSet(
		CRegister::RootPathV( REG_KEY ), "mm2Path", m_mm2Path );

	CRegister::StringSet(
		CRegister::RootPathV( REG_KEY ), "cmdbPath", m_cmdbPath );

	CRegister::StringSet(
		CRegister::RootPathV( REG_KEY ), "toolSetup", m_lastToolSetup );

	CRegister::StringSet(
		CRegister::RootPathV( REG_KEY ), "layerSetup", m_lastLayerSetup );

	CRegister::StringSet(
		CRegister::RootPathV( REG_KEY ), "material", m_lastMaterial );
}

void CFileDWGDlg::InitCMDBvars( const CString& cmdbPath )
{
	if ( !cmdbPath.IsEmpty() )
	{
		CReturn status;
		CDaoDB cmdb;
		CDaoQuery query;
		int count, indx;

		m_toolSetupCombo.ResetContent();
		m_layerSetupCombo.ResetContent();
		m_materialCombo.ResetContent();

		cmdb.addTable( "Machines" );
		cmdb.Open( cmdbPath );

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		status = query.Init( cmdb.Database(), "SELECT * FROM [Tool Setups]" );

		count = query.RecordCount();
		for (indx = 0; indx < count; ++indx)
		{
			query.Move( ((indx == 0) ? 0 : 1) );

			m_toolSetupCombo.AddString( query.StringGet("Description") );
			m_toolSetupCombo.SetItemData( indx, query.IntGet("ID") );
		}

		m_toolSetupCombo.SelectString( -1, m_lastToolSetup );

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		status = query.Init( cmdb.Database(), "SELECT * FROM [Layer Setups]" );

		count = query.RecordCount();
		for (indx = 0; indx < count; ++indx)
		{
			query.Move( ((indx == 0) ? 0 : 1) );

			m_layerSetupCombo.AddString( query.StringGet("Description") );
			m_layerSetupCombo.SetItemData( indx, query.IntGet("ID") );
		}

		m_layerSetupCombo.SelectString( -1, m_lastLayerSetup );

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		status = query.Init( cmdb.Database(), "SELECT * FROM [Material Inventory]" );

		count = query.RecordCount();
		for (indx = 0; indx < count; ++indx)
		{
			query.Move( ((indx == 0) ? 0 : 1) );

			m_materialCombo.AddString( query.StringGet("Description") );
			m_materialCombo.SetItemData( indx, query.IntGet("ID") );
		}

		m_materialCombo.SelectString( -1, m_lastMaterial );

		cmdb.Close();
	}
}

void CFileDWGDlg::IDsInit()
{
	int indx;

	indx = m_toolSetupCombo.GetCurSel();
	if (indx < 0)
	{
		m_toolSetupID = 0;
		m_lastToolSetup = "";
	}
	else
	{
		m_toolSetupID = m_toolSetupCombo.GetItemData( indx );
		m_toolSetupCombo.GetLBText( indx, m_lastToolSetup ); 
	}

	indx = m_layerSetupCombo.GetCurSel();
	if (indx < 0)
	{
		m_layerSetupID = 0;
		m_lastLayerSetup = "";
	}
	else
	{
		m_layerSetupID = m_layerSetupCombo.GetItemData( indx );
		m_layerSetupCombo.GetLBText( indx, m_lastLayerSetup ); 
	}

	indx = m_materialCombo.GetCurSel();
	if (indx < 0)
	{
		m_materialID = 0;
		m_lastMaterial = "";
	}
	else
	{
		m_materialID = m_materialCombo.GetItemData( indx );
		m_materialCombo.GetLBText( indx, m_lastMaterial ); 
	}
}

CString AppDir()
{
	CString path;

	GetCurrentDirectory( 256, path.GetBuffer(256) );
	path.ReleaseBuffer();

	return path;
}
