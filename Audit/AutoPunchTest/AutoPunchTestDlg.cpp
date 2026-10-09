// AutoPunchTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Return.h"
#include "Register.h"
#include "Path.h"
#include "Portal.h"
#include "DaoDB.h"
#include "DaoQuery.h"
#include "AutoPunchTest.h"
#include "AutoPunchTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static char BASED_CODE CMDB_FILTER[] = "CMDB Files (*.mdb)|*.mdb||";
static char BASED_CODE SOURCE_FILTER[] = "CAD Files (*.dxf)|*.dxf|MM2 Files (*.mm2)|*.mm2||";
static char BASED_CODE RESULT_FILTER[] = "MM2 Files (*.mm2)|*.mm2||";
static DWORD FFLAGS = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

static CString REG_KEY = "\\Customizations\\AutoPunchTest";


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
// CAutoPunchTestDlg dialog

CAutoPunchTestDlg::CAutoPunchTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoPunchTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoPunchTestDlg)
	m_build = FALSE;
	m_folder = FALSE;
	m_mtol = 0.01;
	m_path = _T("");
	m_ptol = 0.01;
	m_cmdbPath = _T("");
	m_mm2Path = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoPunchTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoPunchTestDlg)
	DDX_Control(pDX, IDC_MATERIAL_COMBO, m_materialCombo);
	DDX_Control(pDX, IDC_LAYER_SETUP_COMBO, m_layerSetupCombo);
	DDX_Control(pDX, IDC_TOOL_SETUP_COMBO, m_toolSetupCombo);
	DDX_Control(pDX, IDC_MACHINE_COMBO, m_machineCombo);
	DDX_Check(pDX, IDC_BUILD, m_build);
	DDX_Check(pDX, IDC_FOLDER, m_folder);
	DDX_Text(pDX, IDC_MTOL, m_mtol);
	DDX_Text(pDX, IDC_PATH, m_path);
	DDX_Text(pDX, IDC_PTOL, m_ptol);
	DDX_Text(pDX, IDC_CMDB_PATH, m_cmdbPath);
	DDX_Text(pDX, IDC_MM2_PATH, m_mm2Path);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutoPunchTestDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoPunchTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_CLOSEUP(IDC_MACHINE_COMBO, OnCloseupMachineCombo)
	ON_BN_CLICKED(IDC_CMDB_BROWSE, OnCmdbBrowse)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_MM2_BROWSE, OnMm2Browse)
	ON_BN_CLICKED(IDC_FOLDER, OnFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoPunchTestDlg message handlers

BOOL CAutoPunchTestDlg::OnInitDialog()
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

	{
		CString	dllPath;

		dllPath.Format( "%s\\dlls", AppDir() );
		CPortal::Init( dllPath, "", "", TRUE );

		InitVars();

		InitMachineCombo( TRUE );
		InitToolSetupCombo( TRUE );
		InitLayerSetupCombo( TRUE );
		InitMaterialCombo( TRUE );

		UpdateData( FALSE );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAutoPunchTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAutoPunchTestDlg::OnPaint() 
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
HCURSOR CAutoPunchTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAutoPunchTestDlg::OnCloseupMachineCombo() 
{
	InitToolSetupCombo( FALSE );
	InitLayerSetupCombo( FALSE );
}

void CAutoPunchTestDlg::OnCmdbBrowse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_cmdbPath, FFLAGS, CMDB_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_cmdbPath = dlg.GetPathName();
		UpdateData( FALSE );
	}
}

void CAutoPunchTestDlg::OnBrowse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_path, FFLAGS, SOURCE_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		CPath	path;
		CString	ext;

		m_path = dlg.GetPathName();
		m_filter = dlg.GetFileExt();

		path.Set( m_path );

		if ( m_folder )
		{
			m_path = path.DriveDir();
			m_mm2Path = m_path;
		}
		else
		{
			ext = path.Ext();
			
			if (ext.CompareNoCase("mm2") == 0)
			{
				m_mm2Path = path.DriveDirFileName() + "-2.mm2";
			}
			else
			{
				m_mm2Path = path.DriveDirFileName() + ".mm2";
			}
		}

		UpdateData( FALSE );
	}
}

void CAutoPunchTestDlg::OnMm2Browse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_mm2Path, FFLAGS, RESULT_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		CPath path;

		m_mm2Path = dlg.GetPathName();
		path.Set( m_mm2Path );

		if ( m_folder )
			m_mm2Path = path.DriveDir();

		UpdateData( FALSE );
	}
}

void CAutoPunchTestDlg::OnFolder() 
{
	UpdateData( TRUE );

	if ( m_folder )
	{
		CPath	path;
		CString	ext;

		path.Set( m_path );
		ext = path.Ext();

		if ( !ext.IsEmpty() )
			m_path = path.DriveDir();

		path.Set( m_mm2Path );
		ext = path.Ext();

		if ( !ext.IsEmpty() )
			m_mm2Path = path.DriveDir();

		UpdateData( FALSE );
	}
}

void CAutoPunchTestDlg::OnOK() 
{
	CReturn	status;
	CStringArray	cadfiles;
	CString	cadfile;
	CString	mm2file;
	CPath	path;
	CString	ext;
	CString	cmd;
	CWnd*	okButton;
	CWnd*	cancelButton;
	int		toolID;
	int		layID;
	int		matID;
	int		count, indx;

	UpdateData( TRUE );

	okButton = GetDlgItem( IDOK );
	cancelButton = GetDlgItem( IDCANCEL );

	SaveVars();

	indx = m_toolSetupCombo.GetCurSel();
	toolID = m_toolSetupCombo.GetItemData( indx );

	indx = m_layerSetupCombo.GetCurSel();
	layID = m_layerSetupCombo.GetItemData( indx );

	indx = m_materialCombo.GetCurSel();
	matID = m_materialCombo.GetItemData( indx );

	path.Set( m_path );
	ext = path.Ext();

	okButton->EnableWindow( FALSE );
	cancelButton->EnableWindow( FALSE );

	if ( m_folder )
	{
		FilesGet( m_path, m_filter, cadfiles );
	}
	else
	{
		cadfiles.Add( m_path );
	}

	count = cadfiles.GetSize();
	for (indx = 0; indx < count; ++indx)
	{
		cadfile = cadfiles[indx];

		if ( m_folder )
		{
			path.Set( cadfile );
			mm2file = m_mm2Path + "\\" + path.FileName() + ".mm2";
		}
		else
		{
			mm2file = m_mm2Path;
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Import the cad file
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		CPortal::Execute( "Admin:New:" );

		if (m_filter.CompareNoCase("dxf") == 0)
		{
			cmd.Format(
				"File:ImportDXF:"	\
				"dxf=\"%s\","		\
				"db=\"%s\","		\
				"toolsetupid=%d,"	\
				"layersetupid=%d,"	\
				"materialindex=%d," \
				"buildsetup=%d,"	\
				"ptol=%5.4f,"		\
				"mtol=%5.4f",
					cadfile, m_cmdbPath, toolID, layID, matID, m_build, m_ptol, m_mtol );

			status = CPortal::Execute( cmd );
		}
		else if (m_filter.CompareNoCase("mm2") == 0)
		{
			cmd.Format( "File:ImportMM2: file=\"%s\"", m_path );

			status = CPortal::Execute( cmd );

			if ( status.IsOk() )
			{
				cmd.Format(
					"Toolpath:AutoPunch:"	\
					"db=\"%s\","			\
					"toolsetupid=%d,"		\
					"buildsetup=%d,"		\
					"ptol=%5.4f,"			\
					"mtol=%5.4f",
						m_cmdbPath, toolID, m_build, m_ptol, m_mtol );

				status = CPortal::Execute( cmd );
			}
		}


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Export the mm2 file
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		if ( status.IsOk() )
		{
			path.Set( m_path );

			cmd.Format( "File:ExportMM2: file=\"%s\"", mm2file );

			status = CPortal::Execute( cmd );
		}
	}

	okButton->EnableWindow( TRUE );
	cancelButton->EnableWindow( TRUE );

	// CDialog::OnOK();
}

void
CAutoPunchTestDlg::InitVars()
{
	CString	value;

	//=-=-=-=-=-=-=

	value = CRegister::StringGet(
				CRegister::RootPathV(REG_KEY), "build" );

	m_build = (value.CompareNoCase("true") == 0);

	//=-=-=-=-=-=-=

	value = CRegister::StringGet(
				CRegister::RootPathV(REG_KEY), "folder" );

	m_folder = (value.CompareNoCase("true") == 0);

	//=-=-=-=-=-=-=

	m_cmdbPath = CRegister::StringGet(
					CRegister::RootPathV(REG_KEY), "cmdbpath" );

	if ( m_cmdbPath.IsEmpty() )
	{
		m_cmdbPath = CRegister::StringGet(
						CRegister::RootPathV("\\ConfigurationManager"), "Database" );
	}

	//=-=-=-=-=-=-=

	m_path = CRegister::StringGet(
					CRegister::RootPathV(REG_KEY), "path" );

	if ( m_path.IsEmpty() )
		m_path = AppDir() + "\\Cad\\*.dxf";

	//=-=-=-=-=-=-=

	m_filter = CRegister::StringGet(
					CRegister::RootPathV(REG_KEY), "filter" );
	
	if ( m_filter.IsEmpty() )
		m_filter = "dxf";

	//=-=-=-=-=-=-=

	m_mm2Path = CRegister::StringGet(
					CRegister::RootPathV(REG_KEY), "mm2path" );

	if ( m_mm2Path.IsEmpty() )
		m_mm2Path = AppDir() + "\\Models\\*.mm2";

	//=-=-=-=-=-=-=

	m_ptol = CRegister::DoubleGet(
					CRegister::RootPathV(REG_KEY), "ptol" );
	if (m_ptol < SMALL || m_ptol >= UNDEFINED)
		m_ptol = 0.01;

	m_mtol = CRegister::DoubleGet(
					CRegister::RootPathV(REG_KEY), "mtol" );
	if (m_mtol < SMALL || m_mtol >= UNDEFINED)
		m_mtol = 0.01;
}

void
CAutoPunchTestDlg::SaveVars()
{
	CString	value;
	int		indx;

	CRegister::StringSet(
		CRegister::RootPathV(REG_KEY), "build", (m_build ? "true" : "false") );

	CRegister::StringSet(
		CRegister::RootPathV(REG_KEY), "folder", (m_folder ? "true" : "false") );

	indx = m_machineCombo.GetCurSel();
	m_machineCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "machine", value );

	indx = m_toolSetupCombo.GetCurSel();
	m_toolSetupCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "toolsetup", value );

	indx = m_layerSetupCombo.GetCurSel();
	m_layerSetupCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "layersetup", value );

	indx = m_materialCombo.GetCurSel();
	m_materialCombo.GetLBText( indx, value );
	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "material", value );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "cmdbpath", m_cmdbPath );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "path", m_path );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "filter", m_filter );

	CRegister::StringSet(
					CRegister::RootPathV(REG_KEY), "mm2path", m_mm2Path );

	CRegister::DoubleSet(
					CRegister::RootPathV(REG_KEY), "ptol", m_ptol );

	CRegister::DoubleSet(
					CRegister::RootPathV(REG_KEY), "mtol", m_mtol );
}

void
CAutoPunchTestDlg::InitMachineCombo( BOOL initFromReg )
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

		if ( initFromReg )
		{
			value = CRegister::StringGet(
						CRegister::RootPathV(REG_KEY), "machine" );

			indx = m_machineCombo.FindString( -1, value );
			if (indx >= 0)
				m_machineCombo.SetCurSel( indx );
		}
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAutoPunchTestDlg::InitMachineCombo()" );
	}

	cmdb.Close();
}

void
CAutoPunchTestDlg::InitToolSetupCombo( BOOL initFromReg )
{
	CReturn		status;
	CDaoDB		cmdb;
	CDaoQuery	query;
	CString		sql;
	CString		value;
	int			count, indx;

	indx = m_machineCombo.GetCurSel();
	if (indx >= 0)
	{
		m_toolSetupCombo.ResetContent();

		indx = m_machineCombo.GetItemData( indx );
		
		cmdb.Open( m_cmdbPath );

		sql.Format( "SELECT * FROM [Tool Setups] WHERE ([Machine ID]=%d)", indx );

		status = query.Init( cmdb.Database(), sql );

		if ( status.IsOk() )
		{
			count = query.RecordCount();
			for (indx = 0; indx < count; ++indx)
			{
				query.Move( ((indx == 0) ? 0 : 1) );

				m_toolSetupCombo.AddString( query.StringGet("Description") );
				m_toolSetupCombo.SetItemData( indx, query.IntGet("ID") );
			}

			if ( initFromReg )
			{
				value = CRegister::StringGet(
							CRegister::RootPathV(REG_KEY), "toolsetup" );

				indx = m_toolSetupCombo.FindString( -1, value );
				if (indx >= 0)
					m_toolSetupCombo.SetCurSel( indx );
			}
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CAutoPunchTestDlg::InitToolSetupCombo()" );
		}

		cmdb.Close();
	}
}

void
CAutoPunchTestDlg::InitLayerSetupCombo( BOOL initFromReg )
{
	CReturn		status;
	CDaoDB		cmdb;
	CDaoQuery	query;
	CString		sql;
	CString		value;
	int			count, indx;

	indx = m_machineCombo.GetCurSel();
	if (indx >= 0)
	{
		m_layerSetupCombo.ResetContent();

		indx = m_machineCombo.GetItemData( indx );
		
		cmdb.Open( m_cmdbPath );

		sql.Format( "SELECT * FROM [Layer Setups] WHERE ([Machine ID]=%d)", indx );

		status = query.Init( cmdb.Database(), sql );

		if ( status.IsOk() )
		{
			count = query.RecordCount();
			for (indx = 0; indx < count; ++indx)
			{
				query.Move( ((indx == 0) ? 0 : 1) );

				m_layerSetupCombo.AddString( query.StringGet("Description") );
				m_layerSetupCombo.SetItemData( indx, query.IntGet("ID") );
			}

			if ( initFromReg )
			{
				value = CRegister::StringGet(
							CRegister::RootPathV(REG_KEY), "layersetup" );

				indx = m_layerSetupCombo.FindString( -1, value );
				if (indx >= 0)
					m_layerSetupCombo.SetCurSel( indx );
			}
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CAutoPunchTestDlg::InitLayerSetupCombo()" );
		}

		cmdb.Close();
	}
}

void
CAutoPunchTestDlg::InitMaterialCombo( BOOL initFromReg )
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

		if ( initFromReg )
		{
			value = CRegister::StringGet(
						CRegister::RootPathV(REG_KEY), "material" );

			indx = m_materialCombo.FindString( -1, value );
			if (indx >= 0)
				m_materialCombo.SetCurSel( indx );
		}
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAutoPunchTestDlg::InitMaterialCombo()" );
	}

	cmdb.Close();
}

void
CAutoPunchTestDlg::FilesGet(
						const CString&	folder,
						const CString&	filter,
						CStringArray&	files )
{
	CPath			path;
	CString			fullpath;
	CString			ext;
	CString         query;			// Local copy of szStart
	BOOL            bFound;			// Whether or not a new file has been found
	HANDLE          hFile;			// Handle to found file
	WIN32_FIND_DATA stFindData;		// Info about the found file

	CString expandedPath = CPath::FileNameExpand( folder );

	// Append "*.*" to the end of the directory name passed in
	query.Format( "%s\\*.%s", expandedPath, filter );

	// First, see if there's anything in the directory
	hFile = FindFirstFile( (LPCTSTR)query, &stFindData );
	if (INVALID_HANDLE_VALUE == hFile)
	{
		// MessageBox( "FindFirstFile() returned INVALID_HANDLE_VALUE", "DEBUG", MB_OK );
		return;
	}

	// Next, loop through everything in the directory.  If the found file
	// is itself a directory, add it to the tree.
	while (INVALID_HANDLE_VALUE != hFile)
	{
		if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Do nothing.
		}
		else if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			path.Set( stFindData.cFileName );
			ext = path.Ext();

			if (ext.CompareNoCase( filter ) == 0)
			{
				fullpath = folder + "\\" + stFindData.cFileName;
				files.Add( fullpath );
			}
		}

		bFound = FindNextFile(hFile, &stFindData);
		if (!bFound)
			break;
	}
	FindClose(hFile);
}
