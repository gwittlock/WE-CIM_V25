// MsgListBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MathConst.h"
#include "Register.h"
#include "MaxMsgDlg.h"
#include "MsgListBox.h"

// #include "PrinterSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NOTE: Once again, ~!@#$%^&* friggin' Microsoft makes trivial things difficult.
//
// As it is very difficult to get a listbox control to display it contents with
// a specific font, and in particular a font whose ' ' character has the same
// width as all other characters, we choose instead to use friggin' magic in the
// resource file.
//
// The appearance is important because the Java compiler generates an error
// string which points to the error, in the form "      ^".  Of course, we want
// the caret to point at the actual error!
//
// Notice the 'FONT' declaration in the resource file.
//
//	IDD_RECV_DLG DIALOGEX 0, 0, 369, 98
//	STYLE DS_SETFOREGROUND | DS_3DLOOK | WS_MINIMIZEBOX | WS_POPUP | WS_CAPTION | 
//		WS_SYSMENU | WS_THICKFRAME
//	EXSTYLE WS_EX_CLIENTEDGE
//	CAPTION "Diagnostic Messages"
//	FONT 8, "Courier New"
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMsgListBox

CMsgListBox::CMsgListBox()
{
	m_maxmsgs = CRegister::IntGetV( "Preferences\\Errors", "Max", 500 );

	m_dumppath = CRegister::StringGetV( "Preferences\\Errors", "File", "" );

	m_dump = FALSE;
	if (m_dumppath.GetLength() < 2)
	{
		CRegister::StringSetV( "Preferences\\Errors", "File", "" );
	}
	else
	{
		CFileException e;
		if (!m_dumpfile.Open( m_dumppath, CFile::modeCreate | CFile::modeWrite, &e ))
			e.ReportError();
		else
			m_dump = TRUE;
	}
}

CMsgListBox::~CMsgListBox()
{
	if (m_dump)
		m_dumpfile.Close();
}


BEGIN_MESSAGE_MAP(CMsgListBox, CListBox)
	//{{AFX_MSG_MAP(CMsgListBox)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_POPMENU_CLEAR, OnPopmenuClear)
	ON_COMMAND(ID_POPMENU_SETMAXMSGS, OnPopmenuSetmaxmsgs)
	ON_COMMAND(ID_POPMENU_PRINT, OnPopmenuPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

int CMsgListBox::AddMsg( const CString& msg )
{
	CString copy( msg );

	if (m_dump)
	{
		CString termcopy( msg );
		termcopy += "\n";
		m_dumpfile.Write( termcopy, termcopy.GetLength() );
	}


	while (1)
	{
		if (GetCount() >= m_maxmsgs)
			DeleteString( 0 );		// make room

		int indx = copy.Find( '\n' );
		if (indx < 0)
		{
			if (copy.GetLength() > 0)
				AddString( copy );

			SetCaretIndex( GetCount() - 1, false );
			
			return GetCount();
		}
		else
		{
			AddString( copy.Left( indx - 1 ) );
			copy = copy.Mid( indx + 1 );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMsgListBox message handlers

void CMsgListBox::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU1));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	ClientToScreen( &point );
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	CListBox::OnRButtonUp(nFlags, point);
}

void CMsgListBox::OnPopmenuClear() 
{
	int selnum = GetSelCount();
	if (selnum > 0)
	{
		int* idxs = new int[selnum+1];
		GetSelItems( selnum, idxs );

		int idx = selnum-1;
		while (idx >= 0)
		{ 
			DeleteString(idxs[idx]);
			idx--;
		}

		delete[] idxs;
	}
	else
	{ ResetContent(); }
}

void CMsgListBox::OnPopmenuSetmaxmsgs() 
{
	CMaxMsgDlg dlg( m_maxmsgs );

	if (dlg.DoModal() == IDOK)
	{
		m_maxmsgs = dlg.MaxMsgs();
		CRegister::IntSetV( "Preferences\\Errors", "Max", m_maxmsgs );
	}
}

void CMsgListBox::OnPopmenuPrint() 
{
#if 0
	// ------------------------------------------
	// Get the default print info, and ask the user
	//	to confirm it (if !quiet)
	//
	CPrinterSettings printerSettings;
	printerSettings.CopyDefaultMfcPrinter();
//	printerSettings.Load( "msgbox.sav" );

	if (!printerSettings.PrinterSetup( NULL, TRUE ))
		return;
	// Make this the MFC standard printer
	printerSettings.SetThisPrinter();
//	printerSettings.Save( "msgbox.sav" );

	// Get DC of actual standard printer
	CDC pdc;
	AfxGetApp()->CreatePrinterDC( pdc );

	int x_edge = pdc.GetDeviceCaps( PHYSICALOFFSETX );
	int y_edge = pdc.GetDeviceCaps( PHYSICALOFFSETY );
	int dx = pdc.GetDeviceCaps( PHYSICALWIDTH );
	int dy = pdc.GetDeviceCaps( PHYSICALHEIGHT );

	// ------------------------------------------
	//	Start the document
	//
	DOCINFO di;
	ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName	= "MessageBox";
	di.lpszOutput	= (LPSTR)NULL;
	di.lpszDatatype = (LPSTR)NULL;
	di.fwType		= 0;

	pdc.StartDoc( &di );
	pdc.StartPage( );

	//
	// Setup our Font
	//
	int fontsize = 9 * 10;	// Initial fontsize
	CFont pt_font;
	pt_font.CreatePointFont( fontsize, 	"Times New Roman", &pdc );
	CFont* old_font = pdc.SelectObject( &pt_font );

	// Print
	//
	int title_x = x_edge;
	int title_y = y_edge;
	CString str;
	GetText( 0, str );
	CSize size = pdc.GetTextExtent( str );
	int title_down = size.cy;

	int num = GetCount();
	int selnum = GetSelCount();
	for (int idx=0; idx<num; idx++)
	{
		if ( !selnum
			|| GetSel(idx) )
		{
			GetText( idx, str );

			pdc.TextOut( title_x, title_y, str );
			title_y += title_down;

			if (title_y > (dy-y_edge*3))
			{
				pdc.EndPage();
				title_y = y_edge;
				pdc.StartPage( );
			}
		}
	}

	// ------------------------------------------
	//	Go back to normal
	//
	pdc.EndPage();
	pdc.EndDoc();

	pdc.SelectObject( old_font );
	pdc.DeleteDC();
#endif
}


BOOL CMsgListBox::DestroyWindow() 
{
	ResetContent();

	return CListBox::DestroyWindow();
}
