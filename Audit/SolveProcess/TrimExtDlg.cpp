// TrimExtDlg.cpp : implementation file
//

#include "stdafx.h"

#include "DbCurveList.h"
#include "solveprocess.h"
#include "TrimExtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TrimExtDlg dialog


TrimExtDlg::TrimExtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TrimExtDlg::IDD, pParent),
	  m_curvelist1(NULL),
	  m_curvelist2(NULL),
	  m_solution1(-1),
	  m_solution2(-1),
	  m_initialized(false),
	  m_top(0),
	  m_left(0)
{
	//{{AFX_DATA_INIT(TrimExtDlg)
	//}}AFX_DATA_INIT
}


void TrimExtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TrimExtDlg)
	DDX_Control(pDX, IDC_LABEL4, m_label4);
	DDX_Control(pDX, IDC_LABEL3, m_label3);
	DDX_Control(pDX, IDC_LABEL1, m_label1);
	DDX_Control(pDX, IDC_LABEL2, m_label2);
	DDX_Control(pDX, IDC_SPIN2, m_spin2);
	DDX_Control(pDX, IDC_SPIN1, m_spin1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TrimExtDlg, CDialog)
	//{{AFX_MSG_MAP(TrimExtDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TrimExtDlg message handlers

BOOL TrimExtDlg::OnInitDialog() 
{
	CString	text;
	CRect	rect;
	CWnd*	parent;

	CDialog::OnInitDialog();
	
	parent = GetParent();
	parent->GetWindowRect( &rect );

#if ORIGINAL_CODE
	SetWindowPos( parent, (rect.left + 20), (rect.top + 20), 0, 0,
		(SWP_NOSIZE | SWP_NOZORDER) );
#else
	SetWindowPos( parent, (rect.left + m_left), (rect.top + m_top), 0, 0,
		(SWP_NOSIZE | SWP_NOZORDER) );
#endif

	m_spin1.SetRange( 1, m_curvelist1->Count() );
	m_spin2.SetRange( 1, m_curvelist2->Count() );
	
	m_spin1.SetBuddy( &m_label1 );
	m_spin2.SetBuddy( &m_label2 );

	m_spin1.SetPos( m_solution1 );
	m_spin2.SetPos( m_solution2 );

	text.Format( " of %d", m_curvelist1->Count() );
	m_label3.SetWindowText( text );

	text.Format( " of %d", m_curvelist2->Count() );
	m_label4.SetWindowText( text );

	m_initialized = true;

	draw_solutions();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TrimExtDlg::OnOK() 
{
	m_view->Clear( PTEMP_LIST );
	m_view->BufferShow( BACK_BUFFER );

	CDialog::OnOK();
}

void TrimExtDlg::OnCancel() 
{
	m_view->Clear( PTEMP_LIST );
	m_view->BufferShow( BACK_BUFFER );

	CDialog::OnCancel();
}

void TrimExtDlg::Shift( int top, int left )
{
	m_top = top;
	m_left = left;
}

void 
TrimExtDlg::setSolutions(
			CGeoCurveArray& candidatesA,
			int				defSolnIndxA,
			CGeoCurveArray& candidatesB,
			int				defSolnIndxB )
{
	m_curvelist1 = &candidatesA;
	m_curvelist2 = &candidatesB;

	m_solution1 = defSolnIndxA;
	m_solution2 = defSolnIndxB;
}

void
TrimExtDlg::getSolutions(
			int*	indxA,
			int*	indxB )
{
	(*indxA) = m_solution1;
	(*indxB) = m_solution2;
}

void
TrimExtDlg::draw_solutions()
{
	CString	text;

	if (m_initialized && (m_solution1 >= 0) && (m_solution2 >= 0))
	{
		m_view->DisplayListBegin( PTEMP_LIST );

		eDisplayColor color = ((m_view->BackGroundColor() == DCOLOR_BLACK)
			? DCOLOR_WHITE : DCOLOR_BLACK);

		m_view->DrawAtColor( color );

		CGeoCurve* curveA = m_curvelist1->GetAt( m_solution1 );
		m_view->DrawGeoAt( (*curveA), C3dCoord(0,0,0) );

		CGeoCurve* curveB = m_curvelist2->GetAt( m_solution2 );
		m_view->DrawGeoAt( (*curveB), C3dCoord(0,0,0) );

		m_view->DisplayListEnd( PTEMP_LIST );

		// The following call has the side-effect of 'clearing' the view.
		m_view->BufferShow( BACK_BUFFER );
	}
}

LRESULT TrimExtDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// Sheeesh! It took me about an hour to figure this out.
	// And even then, this solution is not perfect.

	// Repaint the curve it the dialog was moved.
	switch (message)
	{
	//case WM_EXITSIZEMOVE:
	//case WM_WINDOWPOSCHANGED:
	//case WM_SYSCOMMAND:
	//case WM_NCACTIVATE:
	case WM_NCMOUSEMOVE:
		draw_solutions();
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void TrimExtDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int	soln;

	// NOTE: Something is still not right with this code.
	// The first time in, odd things happen.
	soln = m_spin1.GetPos() - 1;
	if (soln >= 0 && soln < m_curvelist1->Count())
	{
		m_solution1 = soln;
		draw_solutions();
	}

	*pResult = 0;
}

void TrimExtDlg::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int	soln;

	// NOTE: Something is still not right with this code.
	// The first time in, odd things happen.
	soln = m_spin2.GetPos() - 1;
	if (soln >= 0 && soln < m_curvelist2->Count())
	{
		m_solution2 = soln;
		draw_solutions();
	}

	*pResult = 0;
}
