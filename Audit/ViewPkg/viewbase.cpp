// ==================================================================
//		Base View
//
//	A single view, base class.  Specific view child-classes
//	know how to draw entities in particular ways.  Each view,
//	however, has a view transform and a destination DC.
//
// ==================================================================

#include "stdafx.h"

#include "DbCommand.h"
#include "ViewBase.h"

// TODO: Perhaps these shouldn't be globals?
bool	CViewBase::m_stock = TRUE;
bool	CViewBase::m_markers = TRUE;
bool	CViewBase::m_zonemarkers = TRUE;
int		CViewBase::m_markscale = 5;
bool	CViewBase::m_hotdot = TRUE;
bool	CViewBase::m_tooldash = TRUE;
bool	CViewBase::m_text = TRUE;
bool	CViewBase::m_legend = TRUE;
bool	CViewBase::m_mono = FALSE;
bool	CViewBase::m_verbatim = FALSE;
bool	CViewBase::m_nibble = TRUE;
bool	CViewBase::m_gdi = FALSE;
bool	CViewBase::m_highcontainers = FALSE;
bool	CViewBase::m_endpts = FALSE;
bool	CViewBase::m_fill = FALSE;

bool	CViewBase::m_rapids = TRUE;
double	CViewBase::m_view_factor = 0.05;

// int		CViewBase::m_pick_tol = 7;
int		CViewBase::m_pick_tol = 5;
double	CViewBase::m_grid_tol = 1.e-4;		// Do not set less than SMALL!
double	CViewBase::m_view_limit = 1.e-4;


// ==================================================================

CViewBase::CViewBase( ID in_id )
{
	m_id = in_id;

	m_cwnd = NULL;
	m_solid = FALSE;
	m_animate_speed = 50;
}

CViewBase::~CViewBase()
{
	terminate();
}

void
CViewBase::terminate()
{
	// There are three ways that m_cwnd could have been assigned:
	// 1. extracted from a device context
	// 2. generated from a window handle, using CWnd::FromHandlePermanent
	// 3. allocated from scratch
	// Unfortunately, if we make any attempt to delete the m_cwnd, under
	// any of these conditions, Micro$oft throws and assertion at us eventually...

	m_cwnd = NULL;
}


// ==================================================================

CReturn 
CViewBase::Create( HDC in_hdc )
{
	CDC* context = new CDC;
	context->Attach( in_hdc );

	m_cwnd = context->GetWindow();

	delete context;

	return CReturn( STATUS_OKAY );
}


CReturn 
CViewBase::Create( HWND in_hwnd )
{
//	m_cwnd = CWnd::FromHandlePermanent( in_hwnd );

// experiment
	m_cwnd = CWnd::FromHandle( in_hwnd );
	if (!m_cwnd)
	{
		m_cwnd = new CWnd;
		m_cwnd->Attach( in_hwnd );
	}

	return CReturn( STATUS_OKAY );
}

// And just why were we mucking around with the window
// position? We don't own the window, the app does.
bool
CViewBase::Show( bool show )
{
	BOOL status = FALSE;  // assume failure
	if (m_cwnd != NULL)
	{
		if ( show )
		{
			CWnd* parent = m_cwnd->GetParent();
			if ((parent != NULL) && !parent->IsWindowVisible())
			{
				status = m_cwnd->ShowWindow( SW_SHOW );
			}
		}
		else
		{
			status = m_cwnd->ShowWindow( SW_HIDE );
		}
	}

	return (status != FALSE);
}

/*
CReturn	
CViewBase::Annotate( 
	CString*	text )	// If NULL, reset to top line
{
	CReturn ret;

	if (text)
	{
		CWnd*	window = getCWnd();
		CDC*	context = window->GetDC();

		CString	block = *text;
		while (block.GetLength()>0)
		{
			CString sub;
			sub = block.SpanExcluding( "\n" );

			CSize size = context->GetTextExtent( sub );

			int old_mode = context->SetBkMode( OPAQUE );
			context->TextOut( m_carat.x, m_carat.y, sub );
			context->SetBkMode( old_mode );

			m_carat.x += size.cx;

			int len = sub.GetLength();
			if (len >= block.GetLength())
				block = "";
			else
			{
				if (block.Mid( len, 1)[0] == '\n')
				{
					m_carat.x = 0;
					m_carat.y += size.cy;
				}
				block = block.Mid( len+1 );
			}
		}
	}
	else
	{
		m_carat = CPoint( 0,0 );
	}
	return ret;
}
*/

// TODO: For some as-yet-not-understood reason, the data members used
// by these methods fail to resolve when the methods are made inline.
void	CViewBase::Stock( bool show )			{ m_stock = show; }
bool	CViewBase::Stock()						{ return m_stock; }

void	CViewBase::Markers( bool in_mark ) 		{ m_markers = in_mark; }
bool	CViewBase::Markers( void )				{ return m_markers; }

void	CViewBase::ZoneMarkers( bool in_mark ) 	{ m_zonemarkers = in_mark; }
bool	CViewBase::ZoneMarkers( void )			{ return m_zonemarkers; }

void	CViewBase::MarkerScale( int in_mark ) 	{ m_markscale = in_mark; }
int		CViewBase::MarkerScale( void )			{ return m_markscale; }

void	CViewBase::HotDot( bool dot )			{ m_hotdot = dot; }
bool	CViewBase::HotDot( void ) 				{ return m_hotdot; }

void	CViewBase::ToolDash( bool dash )		{ m_tooldash = dash; }
bool	CViewBase::ToolDash( void )				{ return m_tooldash; }

void	CViewBase::PrintText( bool text )		{ m_text = text; }
bool	CViewBase::PrintText( void )			{ return m_text; }

void	CViewBase::PrintLegend( bool legend )	{ m_legend = legend; }
bool	CViewBase::PrintLegend( void )			{ return m_legend; }

// Allow printing of text unless disabled but even then
// simply suppress printing of text of instances.
bool	CViewBase::PrintText( const CDbEntity* dbEntity )
{
	bool okay_print = PrintText();
	if ( !okay_print)
	{
		const CDbCommand* dbCommand =
			dynamic_cast<const CDbCommand*>( dbEntity );

		if (dbCommand != NULL)
			// okay_print = !dbCommand->IsInstance();
			okay_print = (dbCommand->IntGet( "patid", 0 ) == 0);
	}

	return okay_print;
}

void	CViewBase::PrintVerbatim( bool active )	{ m_verbatim = active; }
bool	CViewBase::PrintVerbatim( void )		{ return m_verbatim; }

void	CViewBase::Monochrome( bool mono )		{ m_mono = mono; }
bool	CViewBase::Monochrome( void )			{ return m_mono; }

void	CViewBase::Nibble( bool nibble )		{ m_nibble = nibble; }
bool	CViewBase::Nibble( void )				{ return m_nibble; }

void	CViewBase::GDI( bool on )				{ m_gdi = on; }
bool	CViewBase::GDI( void )					{ return m_gdi; }

void	CViewBase::PickTol( int pixels )		{ m_pick_tol = pixels; }
int		CViewBase::PickTol( void )				{ return m_pick_tol; }

void	CViewBase::GridTol( double tol )		{ m_grid_tol = tol; }
double	CViewBase::GridTol( void )				{ return m_grid_tol; }

void	CViewBase::ViewLimit( double tol )		{ m_view_limit = tol; }
double	CViewBase::ViewLimit( void )			{ return m_view_limit; }

void	CViewBase::ViewFactor( double factor )	{ m_view_factor = factor; }
double	CViewBase::ViewFactor( void )			{ return m_view_factor; }

void	CViewBase::HighContainers(bool high)	{ m_highcontainers = high; }
bool	CViewBase::HighContainers(void)			{ return m_highcontainers; }

void	CViewBase::Rapids(bool set)				{ m_rapids = set; }
bool	CViewBase::Rapids(void)					{ return m_rapids; }

void	CViewBase::EndPoints(bool set)			{ m_endpts = set; }
bool	CViewBase::EndPoints(void)				{ return m_endpts; }

void	CViewBase::Fill( bool enable )			{ m_fill = enable; }
bool	CViewBase::Fill()						{ return m_fill; }
