// SmartCamConvertorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Return.h"
#include "Path.h"
#include "Register.h"
#include "DaoDB.h"
#include "DaoQuery.h"
#include "Pm4JofConvertor.h"
#include "SmartCamConvertor.h"
#include "SmartCamConvertorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE CMDB_FILTER[]	= "CMDB Files (*.mdb)|*.mdb||";
static char BASED_CODE PM4_FILTER[]		= "PM4 Files (*.pm4)|*.pm4||";
static char BASED_CODE JOF_FILTER[]		= "JOF Files (*.jof)|*.jof||";
static char BASED_CODE RESULT_FILTER[]	= "MM2 Files (*.mm2)|*.mm2||";
static DWORD FFLAGS = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

static CString REG_KEY = "\\Customizations\\PM4Convertor";


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CString AppDir()
{
	CString path;

	GetCurrentDirectory( 256, path.GetBuffer(256) );
	path.ReleaseBuffer();

	return path;
}


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
// CSmartCamConvertorDlg dialog

CSmartCamConvertorDlg::CSmartCamConvertorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmartCamConvertorDlg::IDD, pParent),
	  m_action( BAD_USAGE ),
	  m_debug( FALSE ),
	  m_interactive( FALSE )
{
	//{{AFX_DATA_INIT(CSmartCamConvertorDlg)
	m_cmdbPath = _T("");
	m_pm4Path = _T("");
	m_jofPath = _T("");
	m_mm2Path = _T("");
	m_commonJof = FALSE;
	m_part_outline = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSmartCamConvertorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmartCamConvertorDlg)
	DDX_Control(pDX, IDC_TOOLSETUP_COMBO, m_toolsetupCombo);
	DDX_Control(pDX, IDC_ACTION, m_actionCombo);
	DDX_Control(pDX, IDC_MACHINE_COMBO, m_machineCombo);
	DDX_Control(pDX, IDC_MATERIAL_COMBO, m_materialCombo);
	DDX_Text(pDX, IDC_CMDB_PATH, m_cmdbPath);
	DDX_Text(pDX, IDC_PM4_PATH, m_pm4Path);
	DDX_Text(pDX, IDC_JOF_PATH, m_jofPath);
	DDX_Text(pDX, IDC_MM2_PATH, m_mm2Path);
	DDX_Check(pDX, IDC_COMMON_JOF, m_commonJof);
	DDX_Check(pDX, IDC_PART_OUTLINE, m_part_outline);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSmartCamConvertorDlg, CDialog)
	//{{AFX_MSG_MAP(CSmartCamConvertorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CMDB_BROWSE, OnCmdbBrowse)
	ON_BN_CLICKED(IDC_PM4_BROWSE, OnPm4Browse)
	ON_BN_CLICKED(IDC_JOF_BROWSE, OnJofBrowse)
	ON_BN_CLICKED(IDC_MM2_BROWSE, OnMm2Browse)
	ON_CBN_SELCHANGE(IDC_ACTION, OnSelchangeAction)
	ON_CBN_SELCHANGE(IDC_MACHINE_COMBO, OnSelchangeMachineCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
CSmartCamConvertorDlg::Debug( bool debug )
{
	m_debug = debug;
}

void
CSmartCamConvertorDlg::Interactive( bool interactive )
{
	m_interactive = interactive;
}

/////////////////////////////////////////////////////////////////////////////
// CSmartCamConvertorDlg message handlers

BOOL CSmartCamConvertorDlg::OnInitDialog()
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
	{
		InitVars();

		InitMachineCombo();
		InitToolSetupCombo();
		InitMaterialCombo();

		UpdateData( FALSE );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSmartCamConvertorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSmartCamConvertorDlg::OnPaint() 
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
HCURSOR CSmartCamConvertorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSmartCamConvertorDlg::OnCmdbBrowse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_cmdbPath, FFLAGS, CMDB_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_cmdbPath = dlg.GetPathName();

		InitMachineCombo();
		InitToolSetupCombo();
		InitMaterialCombo();

		UpdateData( FALSE );
	}
}

void CSmartCamConvertorDlg::OnMm2Browse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_mm2Path, FFLAGS, RESULT_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_mm2Path = dlg.GetPathName();
		UpdateData( FALSE );
	}
}

void CSmartCamConvertorDlg::OnPm4Browse() 
{
	UpdateData( TRUE );

	CString	defPath = m_pm4Path;
	
	if ( defPath.IsEmpty() )
		defPath = AppDir() + "\\*.pm4";

	CFileDialog dlg( TRUE, NULL, defPath, FFLAGS, PM4_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_pm4Path = dlg.GetPathName();

		if (m_action == PM4JOF_TO_MM2)
		{
			CPath path( m_pm4Path );

			if ( !m_commonJof )
				m_jofPath = path.DriveDirFileName() + ".jof";

			m_mm2Path = path.DriveDirFileName() + ".mm2";
		}

		UpdateData( FALSE );
	}
}

void CSmartCamConvertorDlg::OnJofBrowse() 
{
	UpdateData( TRUE );

	CString	defPath = m_jofPath;
	
	if ( defPath.IsEmpty() )
		defPath = AppDir() + "\\*.jof";

	CFileDialog dlg( TRUE, NULL, defPath, FFLAGS, JOF_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_jofPath = dlg.GetPathName();
		UpdateData( FALSE );
	}
}

void CSmartCamConvertorDlg::OnOK() 
{
	CPm4JofConvertor convertor;

	CWnd* pm4Browse = GetDlgItem( IDC_PM4_BROWSE );
	CWnd* pm4Path = GetDlgItem( IDC_PM4_PATH );
	CWnd* jofBrowse = GetDlgItem( IDC_JOF_BROWSE );
	CWnd* jofPath = GetDlgItem( IDC_JOF_PATH );
	CWnd* mm2Browse = GetDlgItem( IDC_MM2_BROWSE );
	CWnd* mm2Path = GetDlgItem( IDC_MM2_PATH );

	UpdateData( TRUE );

	convertor.Debug( m_debug );

	if (m_action == PM4JOF_TO_MM2)
	{
		CPath	path;
		CString	machineName;
		CString	toolSetupName;
		CString	materialName;
		int		indx;

		// Disable the buttons during processing.
		pm4Browse->EnableWindow( FALSE );
		pm4Path->EnableWindow( FALSE );
		jofBrowse->EnableWindow( FALSE );
		jofPath->EnableWindow( FALSE );
		mm2Browse->EnableWindow( FALSE );
		mm2Path->EnableWindow( FALSE );
		UpdateData( FALSE );

		indx = m_machineCombo.GetCurSel();
		m_machineCombo.GetLBText( indx, machineName );

		indx = m_materialCombo.GetCurSel();
		m_materialCombo.GetLBText( indx, materialName );

		if (m_interactive)
		{
			indx = m_toolsetupCombo.GetCurSel();
			m_toolsetupCombo.GetLBText( indx, toolSetupName );
		}
		else
		{
			// ASSUMPTION: A tool setup having the jof name will appear in the
			// configuration manager.
			path.Set( m_jofPath );
			toolSetupName = path.FileName();
		}

		convertor.MM2Create( m_cmdbPath, m_pm4Path, m_jofPath, m_mm2Path,
			machineName, toolSetupName, materialName, m_part_outline );

		// Re-enable the buttons.
		pm4Browse->EnableWindow( TRUE );
		pm4Path->EnableWindow( TRUE );
		jofBrowse->EnableWindow( TRUE );
		jofPath->EnableWindow( TRUE );
		mm2Browse->EnableWindow( TRUE );
		mm2Path->EnableWindow( TRUE );
		UpdateData( TRUE );
	}
	else
	{
		// Disable the buttons during processing.
		jofBrowse->EnableWindow( FALSE );
		jofPath->EnableWindow( FALSE );
		UpdateData( FALSE );

		if (m_action == JOF_TO_MDB)
		{
			CPath path( m_jofPath );
		
			CString mdbPath = path.DriveDirFileName() + ".mdb";

			convertor.MDBCreate( m_jofPath, mdbPath );
		}
		else if (m_action == DEBUG_DUMP)
		{
			convertor.Dump(	m_jofPath );
		}

		// Re-enable the buttons.
		jofBrowse->EnableWindow( TRUE );
		jofPath->EnableWindow( TRUE );
		UpdateData( TRUE );
	}
	
	SaveVars();

	// CDialog::OnOK();
}

void CSmartCamConvertorDlg::OnSelchangeAction() 
{
	CWnd* pm4Browse = GetDlgItem( IDC_PM4_BROWSE );
	CWnd* pm4Path = GetDlgItem( IDC_PM4_PATH );
	CWnd* mm2Browse = GetDlgItem( IDC_MM2_BROWSE );
	CWnd* mm2Path = GetDlgItem( IDC_MM2_PATH );
	CComboBox* action = ((CComboBox*) GetDlgItem( IDC_ACTION ));

	m_action = action->GetCurSel();

	switch (m_action)
	{
	case PM4JOF_TO_MM2:
		pm4Browse->EnableWindow( TRUE );
		pm4Path->EnableWindow( TRUE );
		mm2Browse->EnableWindow( TRUE );
		mm2Path->EnableWindow( TRUE );
		break;
	case JOF_TO_MDB:
		pm4Browse->EnableWindow( FALSE );
		pm4Path->EnableWindow( FALSE );
		mm2Browse->EnableWindow( FALSE );
		mm2Path->EnableWindow( FALSE );
		break;
	case DEBUG_DUMP:
		pm4Browse->EnableWindow( FALSE );
		pm4Path->EnableWindow( FALSE );
		mm2Browse->EnableWindow( FALSE );
		mm2Path->EnableWindow( FALSE );
		break;
	}

	UpdateData( FALSE );
}

void
CSmartCamConvertorDlg::InitVars()
{
	CString	value;
	int		indx;

	value = CRegister::StringGet(
				CRegister::RootPathV(REG_KEY), "action", "" );

	indx = m_actionCombo.FindString( -1, value );
	m_action = ((indx < 0) ? 0 : indx);
	m_actionCombo.SetCurSel( m_action );

	//=-=-=-=-=-=-=

	value = CRegister::StringGet(
				CRegister::RootPathV(REG_KEY), "common", "" );

	m_commonJof = (value.CompareNoCase("true") == 0);

	//=-=-=-=-=-=-=

	m_cmdbPath = CRegister::StringGet(
		CRegister::RootPathV(REG_KEY), "cmdbpath", "" );

	if ( m_cmdbPath.IsEmpty() )
	{
		m_cmdbPath = CRegister::StringGet(
			CRegister::RootPathV("\\ConfigurationManager"), "Database", "" );
	}

	//=-=-=-=-=-=-=

	m_pm4Path = CRegister::StringGet(
		CRegister::RootPathV(REG_KEY), "pm4path", (AppDir() + "\\*.pm4") );

	//=-=-=-=-=-=-=

	m_jofPath = CRegister::StringGet(
		CRegister::RootPathV(REG_KEY), "jofpath", (AppDir() + "\\*.jof") );

	//=-=-=-=-=-=-=

	m_mm2Path = CRegister::StringGet(
		CRegister::RootPathV(REG_KEY), "mm2path", (AppDir() + "\\*.mm2") );

	//=-=-=-=-=-=-=

}

void
CSmartCamConvertorDlg::SaveVars()
{
	CString	value;
	int		indx;

	indx = m_actionCombo.GetCurSel();
	m_actionCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "action", value );

	indx = m_machineCombo.GetCurSel();
	m_machineCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "machine", value );

	indx = m_toolsetupCombo.GetCurSel();
	m_toolsetupCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "toolsetup", value );

	indx = m_materialCombo.GetCurSel();
	m_materialCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "material", value );

	CRegister::StringSet(
		CRegister::RootPathV(REG_KEY), "common", (m_commonJof ? "true" : "false") );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "cmdbpath", m_cmdbPath );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "pm4path", m_pm4Path );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "jofpath", m_jofPath );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "mm2path", m_mm2Path );
}

void
CSmartCamConvertorDlg::InitMachineCombo()
{
	CReturn		status;
	CDaoDB		cmdb;
	CDaoQuery	query;
	CString		value;
	int			count, indx;
	
	m_machineCombo.ResetContent();

	cmdb.Open( m_cmdbPath );

	status = query.Init( cmdb.Database(), "SELECT * FROM [Machines]" );

	if ( status.IsOk() )
	{
		count = query.RecordCount();
		for (indx = 0; indx < count; ++indx)
		{
			query.Move( ((indx == 0) ? 0 : 1) );

			m_machineCombo.AddString( query.StringGet("Description") );
			m_machineCombo.SetItemData( indx, query.IntGet("ID") );
		}

		value = CRegister::StringGet(
			CRegister::RootPathV(REG_KEY), "machine", "" );

		indx = m_machineCombo.FindString( -1, value );
		if (indx >= 0)
			m_machineCombo.SetCurSel( indx );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAutoPunchTestDlg::InitMachineCombo()" );
	}

	cmdb.Close();
}

void
CSmartCamConvertorDlg::InitToolSetupCombo()
{
	CReturn		status;
	CDaoDB		cmdb;
	CDaoQuery	query;
	CString		value;
	CString		sql;
	int			count, indx;
	int			machine_id;
	
	m_toolsetupCombo.ResetContent();

	cmdb.Open( m_cmdbPath );

	indx = m_machineCombo.GetCurSel();
	machine_id = m_machineCombo.GetItemData(indx);

	sql.Format( "SELECT * FROM [Tool Setups] WHERE ([Machine ID]=%d)", machine_id );
	status = query.Init( cmdb.Database(), sql );

	if ( status.IsOk() )
	{
		count = query.RecordCount();
		for (indx = 0; indx < count; ++indx)
		{
			query.Move( ((indx == 0) ? 0 : 1) );

			m_toolsetupCombo.AddString( query.StringGet("Description") );
			m_toolsetupCombo.SetItemData( indx, query.IntGet("ID") );
		}

		value = CRegister::StringGet(
			CRegister::RootPathV(REG_KEY), "toolsetup", "" );

		indx = m_toolsetupCombo.FindString( -1, value );
		if (indx >= 0)
			m_toolsetupCombo.SetCurSel( indx );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAutoPunchTestDlg::InitToolSetupCombo()" );
	}

	cmdb.Close();
}

void
CSmartCamConvertorDlg::InitMaterialCombo()
{
	CReturn		status;
	CDaoDB		cmdb;
	CDaoQuery	query;
	CString		value;
	int			count, indx;

	m_materialCombo.ResetContent();

	cmdb.Open( m_cmdbPath );

	status = query.Init( cmdb.Database(), "SELECT * FROM [Material Inventory]" );

	if ( status.IsOk() )
	{
		count = query.RecordCount();
		for (indx = 0; indx < count; ++indx)
		{
			query.Move( ((indx == 0) ? 0 : 1) );

			m_materialCombo.AddString( query.StringGet("Description") );
			m_materialCombo.SetItemData( indx, query.IntGet("ID") );
		}

		value = CRegister::StringGet(
			CRegister::RootPathV(REG_KEY), "material", "" );

		indx = m_materialCombo.FindString( -1, value );
		if (indx >= 0)
			m_materialCombo.SetCurSel( indx );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAutoPunchTestDlg::InitMaterialCombo()" );
	}

	cmdb.Close();
}

void CSmartCamConvertorDlg::OnSelchangeMachineCombo() 
{
	InitToolSetupCombo();
}
