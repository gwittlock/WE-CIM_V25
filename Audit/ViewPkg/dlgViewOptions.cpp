// dlgViewOptions.cpp : implementation file
//

#include "stdafx.h"

#include "common.h"
#include "command.h"
#include "viewpkg.h"
#include "Register.h"

#include "dlgViewOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CdlgViewOptions dialog


CdlgViewOptions::CdlgViewOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgViewOptions::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(CdlgViewOptions)
	m_tool_dash = FALSE;
	m_text = FALSE;
	m_stock = FALSE;
	m_nibble = FALSE;
	m_prof_marker = FALSE;
	m_grid = 0.0;
	m_gdi = FALSE;
	m_mono = FALSE;
	m_white = FALSE;
	m_picktol = 0;
	m_profscale = 0;
	//}}AFX_DATA_INIT

	ReadDefaults();
}


void
CdlgViewOptions::ReadDefaults()
{
	CString key = "Preferences\\ViewOptions";

	m_tool_dash = CRegister::IntGetV( key, "ToolpathGeometry", 0 );
	m_text = CRegister::IntGetV( key, "TextCommands", 1 );
	m_stock = CRegister::IntGetV( key, "ShowStock", 1 );
	m_nibble = CRegister::IntGetV( key, "ShowNibbleHits", 0 );
	m_prof_marker = CRegister::IntGetV( key, "ProfileMarkers", 0 );
	m_grid = CRegister::DoubleGetV( key, "SnapGridResolution", 0.01 );

	m_gdi = CRegister::IntGetV( key, "GDI", 0 );
	m_mono = CRegister::IntGetV( key, "Mono", 0 );
	m_white = CRegister::IntGetV( key, "White", 0 );
	m_picktol = CRegister::IntGetV( key, "PickTolerance", 1 );	// ???
	m_profscale = CRegister::IntGetV( key, "MarkerScale", 1 );	// ???
}

void
CdlgViewOptions::SetViewMode( const CViewMgr& view )
{
	CString key ="Preferences\\ViewOptions";

	CRegister::IntSetV( key, "ToolpathGeometry", m_tool_dash );
	CRegister::IntSetV( key, "TextCommands", m_text );
	CRegister::IntSetV( key, "ShowStock", m_stock );
	CRegister::IntSetV( key, "ShowNibbleHits", m_nibble );
	CRegister::IntSetV( key, "ProfileMarkers", m_prof_marker );
	CRegister::DoubleSetV( key, "SnapGridResolution", m_grid );

	CRegister::IntSetV( key, "GDI", m_gdi );
	CRegister::IntSetV( key, "Mono", m_mono );
	CRegister::IntSetV( key, "White", m_white );
	CRegister::IntSetV( key, "PickTolerance", m_picktol );
	CRegister::IntSetV( key, "MarkerScale", m_profscale );

	int fg = (m_white?0x000000:0xffffff);
	int bg = (m_white?0xffffff:0x000000);
	int hot = (m_white?0x000080:0x0000ff);

// View:Mode: [profmark=%b,][hotdot=%b,][tooldash=%b,][picktol=%d,][gridtol=%f][viewlimit=%f]
//				[text=%b][mono=%b][verbatim=%b][solid=%b]
//				[color_fg=%d][color_bg=%d][color_hot=%d][color_sheet=%d]
//				[speed=%d][nibble=%b][border=%f][gdi=%b]
	CString cmd_str;
	cmd_str.Format( "View:Mode: ProfMark=%d, ToolDash=%d, Text=%d, Nibble=%d, gridtol=%f, gdi=%d, mono=%d,\
					color_fg=%d, color_bg=%d, color_hot=%d, picktol=%d, profscale=%d",
						m_prof_marker,
						m_tool_dash,
						m_text,
						m_nibble,
						m_grid,
						m_gdi,
						m_mono,
						fg, bg, hot,
						m_picktol,
						m_profscale );

	CCommand view_cmd;
	view_cmd.setCommand( cmd_str, NULL, NULL );
	view.Mode( &view_cmd );
}

void CdlgViewOptions::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CdlgViewOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgViewOptions)
	DDX_Check(pDX, IDC_VIEW_TOOLDASH, m_tool_dash);
	DDX_Check(pDX, IDC_VIEW_TEXT, m_text);
	DDX_Check(pDX, IDC_VIEW_STOCK, m_stock);
	DDX_Check(pDX, IDC_VIEW_NIBBLE, m_nibble);
	DDX_Check(pDX, IDC_VIEW_PROFMARKER, m_prof_marker);
	DDX_Text(pDX, IDC_GRID, m_grid);
	DDV_MinMaxDouble(pDX, m_grid, 1.e-005, 1.);
	DDX_Check(pDX, IDC_GDI, m_gdi);
	DDX_Check(pDX, IDC_MONO, m_mono);
	DDX_Check(pDX, IDC_WHITE, m_white);
	DDX_Text(pDX, IDC_PICKTOL, m_picktol);
	DDV_MinMaxInt(pDX, m_picktol, 1, 20);
	DDX_Text(pDX, IDC_PROFSCALE, m_profscale);
	DDV_MinMaxInt(pDX, m_profscale, 1, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgViewOptions, CDialog)
	//{{AFX_MSG_MAP(CdlgViewOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CdlgViewOptions, CDialog)
	//{{AFX_DISPATCH_MAP(CdlgViewOptions)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IdlgViewOptions to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {FB98D4EF-E645-42F6-86CF-B62826545475}
static const IID IID_IdlgViewOptions =
{ 0xfb98d4ef, 0xe645, 0x42f6, { 0x86, 0xcf, 0xb6, 0x28, 0x26, 0x54, 0x54, 0x75 } };

BEGIN_INTERFACE_MAP(CdlgViewOptions, CDialog)
	INTERFACE_PART(CdlgViewOptions, IID_IdlgViewOptions, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgViewOptions message handlers

