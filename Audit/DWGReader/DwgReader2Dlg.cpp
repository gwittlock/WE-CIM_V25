// DwgReader2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "dwg_file.h"
#include "DwgReader2.h"
#include "DwgReader2Dlg.h"

#include "DwgReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE DWG_FILTER[] = "DWG Files (*.dwg)|*.dwg||";
static DWORD FFLAGS = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;


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
// CDwgReader2Dlg dialog

CDwgReader2Dlg::CDwgReader2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDwgReader2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDwgReader2Dlg)
	m_dwgPath = _T("");
	m_test = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDwgReader2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDwgReader2Dlg)
	DDX_Text(pDX, IDC_DWG_PATH, m_dwgPath);
	DDX_Check(pDX, IDC_TEST, m_test);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDwgReader2Dlg, CDialog)
	//{{AFX_MSG_MAP(CDwgReader2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DWG_BROWSE, OnDwgBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDwgReader2Dlg message handlers

BOOL CDwgReader2Dlg::OnInitDialog()
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
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDwgReader2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDwgReader2Dlg::OnPaint() 
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
HCURSOR CDwgReader2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDwgReader2Dlg::OnOK() 
{
	UpdateData( TRUE );

	if ( m_test )
	{
		CDWGFile	file;
		BYTE		tstBytes[] = {0x40, 0x80, 0x6d, 0x0f, 0x80, 0x00};
		double		tstDouble = 1.0;
		short		bitShort;
		double		dbl;

		file.TestBufferSet( tstBytes, sizeof( tstBytes ) );
		bitShort = file.ReadBitShort();

		file.TestBufferSet( (BYTE*) &tstDouble, sizeof( tstDouble ) );
		dbl = file.ReadRawDouble();
	}
	else
	{
		CDwgReader reader;

		reader.Read( m_dwgPath );
	}

	// CDialog::OnOK();  // wait for Cancel button (to close app)
}

void CDwgReader2Dlg::OnDwgBrowse() 
{
	UpdateData( TRUE );

	if ( m_dwgPath.IsEmpty() )
		m_dwgPath = "c:\\_Weng\\DwgReader\\*.dwg";

	CFileDialog dlg( TRUE, NULL, m_dwgPath, FFLAGS, DWG_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		m_dwgPath = dlg.GetPathName();
		UpdateData( FALSE );
	}
}
