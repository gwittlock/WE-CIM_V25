// FilePeekDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DspConsts.h"
#include "Return.h"
#include "dwg_file.h"
#include "FilePeek.h"

#include "Readout.h"
#include "OffsetWindow.h"
#include "DataWindow.h"

#include "FilePeekDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE ALL_FILTER[] = "ALL Files (*.*)|*.*||";
static DWORD FFLAGS = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

BYTE	g_buf[MAXBUF];


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
// CFilePeekDlg dialog

CFilePeekDlg::CFilePeekDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilePeekDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilePeekDlg)
	m_path = _T("c:\\_tmp\\*.*");
	m_bits = 0;
	m_bytes = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);  // TODO: WHY CRASH ???
}

void CFilePeekDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilePeekDlg)
	DDX_Control(pDX, IDC_OFFSET, m_offsetWin);
	DDX_Control(pDX, IDC_SHORT, m_short);
	DDX_Control(pDX, IDC_LONG, m_long);
	DDX_Control(pDX, IDC_INTEGER, m_integer);
	DDX_Control(pDX, IDC_HANDLE, m_handle);
	DDX_Control(pDX, IDC_BYTE, m_byte);
	DDX_Control(pDX, IDC_DATA, m_dataWin);
	DDX_Control(pDX, IDC_BIT_SPINNER, m_bitSpinner);
	DDX_Control(pDX, IDC_BYTE_SPINNER, m_byteSpinner);
	DDX_Text(pDX, IDC_PATH, m_path);
	DDX_Text(pDX, IDC_BITS, m_bits);
	DDX_Text(pDX, IDC_BYTES, m_bytes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFilePeekDlg, CDialog)
	//{{AFX_MSG_MAP(CFilePeekDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BYTE_SPINNER, OnDeltaposByteSpinner)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BIT_SPINNER, OnDeltaposBitSpinner)
	ON_EN_CHANGE(IDC_BYTES, OnChangeBytes)
	ON_WM_CHAR()
	ON_MESSAGE( DATA_HIGHLIGHT, DataHighlight )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePeekDlg message handlers

BOOL CFilePeekDlg::OnInitDialog()
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

	m_byteSpinner.SetRange( 0, 1 );
	m_bitSpinner.SetRange( 0, 1 );

	m_byte.TypeSet( EBYTE );
	m_short.TypeSet( ESHORT );
	m_integer.TypeSet( EINTEGER );
	m_long.TypeSet( ELONG );
	m_handle.TypeSet( EHANDLE );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFilePeekDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFilePeekDlg::OnPaint() 
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
HCURSOR CFilePeekDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFilePeekDlg::OnBrowse() 
{
	UpdateData( TRUE );

	CFileDialog dlg( TRUE, NULL, m_path, FFLAGS, ALL_FILTER );
	if (dlg.DoModal() == IDOK)
	{
		CReturn status;

		m_path = dlg.GetPathName();
		UpdateData( FALSE );

		if ( !m_file.getName().IsEmpty() )
			m_file.Close();

		status = m_file.Open( m_path, FILEMODE_READ );
		if ( status.IsOk() )
		{
			m_base	= -1;
			m_bytes =  0;
			m_bits	=  0;
			DataSeekDisplay( 0, 0 );
		}
	}
}

void CFilePeekDlg::OnOK() 
{
	CString buf;

	m_dataWin.GetWindowText( buf );
	
	CDialog::OnOK();
}

void CFilePeekDlg::OnDeltaposByteSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// long	bytes = m_bytes + (pNMUpDown->iDelta * BYTES_PER_ROW);
	long	bytes = m_bytes + pNMUpDown->iDelta;

	if (bytes >= 0 && bytes <= m_file.Length())
	{
		m_bytes = bytes;
		UpdateData( FALSE );
		DataSeekDisplay( m_bytes, m_bits );
	}

	*pResult = 0;
}

void CFilePeekDlg::OnDeltaposBitSpinner(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int	bits = m_bits + pNMUpDown->iDelta;

	if (bits < 0)
	{
		if (m_bytes > 0)
		{
			--m_bytes;
			m_bits = 7;
		}
	}
	else if (bits > 7)
	{
		if (m_bits < m_file.Length())
		{
			++m_bytes;
			m_bits = 0;
		}
	}
	else
	{
		m_bits = bits;
	}

	UpdateData( FALSE );
	DataSeekDisplay( m_bytes, m_bits );

	*pResult = 0;
}

void CFilePeekDlg::OnChangeBytes() 
{
	UpdateData( TRUE );

	DataSeekDisplay( m_bytes, m_bits );
}

CReturn
CFilePeekDlg::DataSeekDisplay( int bytes, int bits )
{
	CReturn status;
	int	limit;
	int	base;
	int	diff;

	if (m_base < 0)
	{
		// opened new file

		m_base = 0;
		m_bits = 0;

		status = m_file.Seek( m_base, CFile::begin );

		if ( status.IsOk() )
			status = m_file.ReadBytes( g_buf, MAXBUF );
	}
	else
	{
		// Determines when to refill buffer and scroll.
		limit = BYTES_PER_ROW * (MAXROWS / 2);

		// 'base' is the index to the first displayed byte.
		// This should always be some multiple of BYTES_PER_ROW.
		base = (bytes / BYTES_PER_ROW) * BYTES_PER_ROW;

		diff = base - m_base;

		if (diff < 0 || diff >= limit)
		{
			if (diff < 0)
			{
				m_base = base;
			}
			else
			{
				m_base += BYTES_PER_ROW;
			}
				
			status = m_file.Seek( m_base, CFile::begin );

			if ( status.IsOk() )
				status = m_file.ReadBytes( g_buf, MAXBUF );
		}
	}

	if ( status.IsOk() )
		DataDisplay();

	return status;
}

void
CFilePeekDlg::DataDisplay()
{
	DataWinDisplay();
	ByteDisplay();
	ShortDisplay();
	IntegerDisplay();
	LongDisplay();
	HandleDisplay();
}

void
CFilePeekDlg::DataWinDisplay()
{
	m_offsetWin.OffsetDisplay( m_base );
	m_dataWin.DataDisplay( g_buf, MAXBUF, m_base );
}

void
CFilePeekDlg::ByteDisplay()
{
	CString	sval;
	BYTE	val;

	Seek( m_bytes, m_bits );

	val = m_file.ReadBYTE();
	sval.Format( "%d", val );

	WinDisplay( IDC_BYTE, sval );
}

void
CFilePeekDlg::ShortDisplay()
{
	CString	sval;
	short	val;

	Seek( m_bytes, m_bits );

	val = m_file.ReadRawShort();
	sval.Format( "%d", val );

	WinDisplay( IDC_SHORT, sval );
}

void
CFilePeekDlg::IntegerDisplay()
{
	CString	sval;
	int		val;
	
	val = m_file.ReadRawShort();
	sval.Format( "%d", val );

	WinDisplay( IDC_INTEGER, sval );
}

void
CFilePeekDlg::LongDisplay()
{
	CString	sval;
	long	val;
	
	val = m_file.ReadRawLong();
	sval.Format( "%ld", val );

	WinDisplay( IDC_LONG, sval );
}

void
CFilePeekDlg::HandleDisplay()
{
	CString	sval;
	long	val;

	val = m_file.ReadRawShort();
	sval.Format( "%ld", val );

	WinDisplay( IDC_HANDLE, sval );
}

void
CFilePeekDlg::Seek( int bytes, int bits )
{
	int actualBitidx = 7 - bits;
	m_file.BufferSeek( (m_bytes - m_base), ((actualBitidx == 0) ? -1 : actualBitidx) );
}

void
CFilePeekDlg::WinDisplay( int widgetID, const CString& sval )
{
	CEdit* win = (CEdit*) GetDlgItem( widgetID );
	win->SetWindowText( sval );
}

void
CFilePeekDlg::DataHighlight( WPARAM wParam, LPARAM lParam )
{
	int	selBits = (int) wParam;

	m_dataWin.Highlight( (m_bytes - m_base), m_bits, selBits );
}
