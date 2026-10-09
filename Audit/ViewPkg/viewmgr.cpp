// ==================================================================
//
//	Keeps track of the various views.  Routes commands to the
//	active view as needed.  Manages the view transforms.
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#include "FileSnoop.h"
#include "ViewMgr.h"

#include "dlgViewOptions.h"
#include "oglViewWire.h"

#include "ViewBogus.h"


// To prevent overhead of char* to CString conversion
const CString STR_EX( "ex" );
const CString STR_EY( "ey" );
const CString STR_MODE( "mode" );
const CString STR_SNAP( "snap" );
const CString STR_SX( "sx" );
const CString STR_SY( "sy" );
const CString STR_X( "x" );
const CString STR_Y( "y" );

CViewBogus g_bogus_view;

// ==================================================================

ID								CViewMgr::m_next_id = 1;

CRouteList						CViewMgr::m_router;

CArray<CViewBase*, CViewBase*>	CViewMgr::m_view_list;
CViewBase*						CViewMgr::m_active_view = &g_bogus_view;

CglCanvas*						CViewMgr::m_debug_canvas = NULL;
bool							CViewMgr::m_viewmgr_enabled = true;

// ==================================================================

CViewMgr::CViewMgr()
{
	m_active_view = &g_bogus_view;
}

CViewMgr::~CViewMgr()
{
	Reset();
}

void
CViewMgr::Reset( void )
{
	int			idx;
	CViewBase*	view;

	m_router.Reset();

	for (idx=0; idx<m_view_list.GetSize(); idx++)
	{
		view = m_view_list.GetAt( idx );
			delete view;
	}
	m_view_list.RemoveAll();
	m_active_view = NULL;
}


// ==================================================================
//	Add all possible view operations to the command route list.
//
CReturn
CViewMgr::Register( CRouteList* io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	ret = io_route->addSubrouter( "View", &m_router );
	//
	// View:*
	//
	ret += m_router.addProcess( "Enable", Enable );

	ret += m_router.addProcess( "New", Create );
	ret += m_router.addProcess( "Delete", Delete );
	ret += m_router.addProcess( "Select", Select );

	ret += m_router.addProcess( "Highlight", Highlight );
	ret += m_router.addProcess( "Erase", Erase );
	ret += m_router.addProcess( "Draw", Draw );

	ret += m_router.addProcess( "Resize", Resize );
	ret += m_router.addProcess( "Show", Show );
	ret += m_router.addProcess( "Clear", Clear );

	ret += m_router.addProcess( "Mode", Mode );
	ret += m_router.addProcess( "ModeGet", ModeGet );
	ret += m_router.addProcess( "ModeDlg", ModeDlg );

	ret += m_router.addProcess( "Refresh", Refresh );
	ret += m_router.addProcess( "Export", Export );

	ret += m_router.addProcess( "Save", Save );
	ret += m_router.addProcess( "Next", Next );
	ret += m_router.addProcess( "Prev", Prev );

	ret += m_router.addProcess( "Full", Full );
	ret += m_router.addProcess( "MaterialExtents", MaterialExtents );
	ret += m_router.addProcess( "Window", Window );
	ret += m_router.addProcess( "Zoom", Zoom );
	ret += m_router.addProcess( "Pan", Pan );
	ret += m_router.addProcess( "Rotate", Rotate );

	ret += m_router.addProcess( "Angle", Angle );
	ret += m_router.addProcess( "Plane", Plane );

	ret += m_router.addProcess( "RubberStart", RubberStart );
	ret += m_router.addProcess( "RubberStop", RubberStop );

	ret += m_router.addProcess( "Mouse", Move );
	ret += m_router.addProcess( "Pick", Pick );

	ret += m_router.addProcess( "Print", Print );

	ret += m_router.addProcess( "RGB", RGB_Value );

	return ret;
}

// view:enable: enable=%b
CReturn 
CViewMgr::Enable( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_viewmgr_enabled = (io_cmd->VarList().getInt("enable",1) > 0);
	return STATUS_OKAY;
}

// ==================================================================
//	Route to the sub-command under View
//		View:Execute:
//
CReturn 
CViewMgr::Execute( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_router.Dispatch( io_cmd );
}


// ==================================================================
// View:New: type=%d, wnd=%d
//   where: type (1) main / (2) preview
CReturn 
CViewMgr::Create( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CViewBase*	view;
	HWND		hwnd;
	ID			view_id;
	int			type;

	if (!io_cmd->ModelExists())
		return status;

	status += io_cmd->getInt( "type", (int*) &type );
	status += io_cmd->getInt( "wnd", (int*) &hwnd );
	if (!status.isOkay())
		return status;

	view = new CoglViewWire( m_next_id );
	if (view != NULL)
	{
		status += view->Create( hwnd );
		if ( status.IsOk() )
			view->ModelSet( io_cmd->getModel() );
	}

	type = VIEW_OGLWIRE;  // ignore the given type

	switch (type)
	{
	case VIEW_OGLWIRE:
		break;

	default:
		status.Internal( IDS_VIEW_UNKNOWN_TYPE, type );
		break;
	}

	if ( status.IsOk() )
	{
		view_id = m_next_id;
		++m_next_id;

		io_cmd->setInt( "id", view_id );

		m_active_view = view;
		m_view_list.Add( view );

		// WTF is this about?
		ID plane;
		if (io_cmd->getInt( "plane", (int*)&plane).isOkay())
		{
			status += m_active_view->Plane( io_cmd->getModel(), plane );
		}
	}

	return status;
}

// ==================================================================
// View:Delete: id=%d
//
CReturn
CViewMgr::Delete( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	ID			view_id;
	int			idx;
	int			num;
	CViewBase*	view;

	view_id = 0;
	ret = io_cmd->getInt( "ID", (int*)&view_id );

	if (ret.isOkay())
	{
		num = m_view_list.GetSize();
		for (idx=0; idx<num; idx++)
		{
			view = m_view_list.GetAt( idx );
			if (view->getID() == view_id)
			{
				if (m_active_view == view)
					m_active_view = NULL;

				m_view_list.RemoveAt( idx );
				delete view;

				if (CReturn::Debug())
				{
					CString note;
					note.Format( "Delete view %d gives %d views", view_id, m_view_list.GetSize() );
					ret.Diagnostic( note );
				}

				break;
			}
		}
	}

	return ret;
}

// ==================================================================
// View:Select: id=%d
//
CReturn 
CViewMgr::Select( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	if ( io_cmd->ModelExists() )
	{
		ID view_id = io_cmd->VarList().getInt( "ID", 0 );

		CViewBase* view = ViewFind( view_id );
		if (view != NULL)
		{
			m_active_view = view;
			m_active_view->Show( true );

			m_active_view->Clear( ALL_LISTS );
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CViewMgr::Select(#1)" );
		}
	}

	return status;
}


// ==================================================================
//
// View:Mode: id=%d,
//		[stock=%b,][workzones=%b]
//		[profmark=%b,][hotdot=%b,][tooldash=%b,][picktol=%d,][gridtol=%f][viewlimit=%f]
//		[text=%b][legend=%b][mono=%b][verbatim=%b][solid=%b]
//		[color_fg=%d][color_bg=%d][color_hot=%d][color_sheet=%d]
//		[speed=%d][nibble=%b][border=%f][gdi=%b]
//		[highc=%d][endpts=%d][rapids=%d][solidhits=%d]
//				
//
// profmark -- profile markers ON/OFF
// profscale -- relative size of the profile markers
// hotdot   -- hot dots ON/OFF
// tooldash -- tooling dashed style ON/OFF
// picktol  -- length of 'hot dot' side
// gridtol  -- the snap grid used to filter mouse motions
// viewtol  -- lower bound on world bounding box for zoom-in
// text		-- print text elements
// mono		-- monochrome display?
// solid	-- solid tool display
// color_fg	-- foreground color (used for some interactive graphic; XOR to bg!)
// color_bg -- background color (e.g. the main color of the graphics view )
// color_hot -- hot color (selected color)
// color_sheet -- solid view sheet color
// color_pat -- pattern color
// nibble	-- TRUE if we want to display the tool nibbles in wireframe
// border	-- 0.05 for a standard border, 0.00 for printing
// gdi		-- TRUE if we want to force GDI display
// workzones -- TRUE if we want to display the zones and clamps
// highc	-- TRUE if we want to be able to highlight containers
// rapids	--- TRUE if we want to display rapid moves during code display
// endpts	--- TRUE if we want to display markers on all endpoints
// white_bg --- TRUE white background / FALSE black background (default)
//
CReturn 
CViewMgr::Mode( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	ID id = io_cmd->VarList().getInt( "id", 0 );
	CViewBase* view = ViewFind( id );
	if (view != NULL)
	{
		CReturn field;
		int ival;
		double dval;

		field = io_cmd->getInt( "stock", &ival );
		if (field.isOkay())
			view->Stock( ival != 0 );

		field = io_cmd->getInt( "workzones", &ival );
		if (field.isOkay())
			view->ZoneMarkers( ival != 0 );

		field = io_cmd->getInt( "Text", &ival );
		if (field.isOkay())
			view->PrintText( ival != 0 );

		ival = io_cmd->VarList().getInt( "legend", IUNDEFINED );
		if (ival != IUNDEFINED)
		{
			view->PrintLegend( ival != 0 );
			CDbEntity::CanDrawLegend( ival != 0 );
		}

		field = io_cmd->getInt( "Verbatim", &ival );
		if (field.isOkay())
			view->PrintVerbatim( ival != 0 );

		field = io_cmd->getInt( "Mono", &ival );
		if (field.isOkay())
			view->Monochrome( ival != 0 );

		field = io_cmd->getInt( "profmark", &ival );
		if (field.isOkay())
			view->Markers( ival != 0 );

		ival = io_cmd->VarList().getInt( "handles", IUNDEFINED );
		if (ival != IUNDEFINED)
			CDbEntity::CanDrawHandles( ival != 0 );

		field = io_cmd->getInt( "highc", &ival );
		if (field.isOkay())
			view->HighContainers( ival != 0 );

		field = io_cmd->getInt( "rapids", &ival );
		if (field.isOkay())
			view->Rapids( ival != 0 );

		field = io_cmd->getInt( "solidhits", &ival );
		if (field.isOkay())
			view->Fill( ival != 0 );

		field = io_cmd->getInt( "endpts", &ival );
		if (field.isOkay())
			view->EndPoints( ival != 0 );

		field = io_cmd->getInt( "profscale", &ival );
		if (field.isOkay())
			view->MarkerScale( ival );

		field = io_cmd->getInt( "hotdot", &ival );
		if (field.isOkay())
			view->HotDot( ival != 0 );

		field = io_cmd->getInt( "tooldash", &ival );
		if (field.isOkay())
			view->ToolDash( ival != 0 );

		field = io_cmd->getInt( "nibble", &ival );
		if (field.isOkay())
			view->Nibble( ival != 0 );

		field = io_cmd->getInt( "picktol", &ival );
		if (field.isOkay())
			view->PickTol( ival );

		field = io_cmd->getReal( "gridtol", &dval );
		if (field.isOkay())
			view->GridTol( dval );

		field = io_cmd->getReal( "viewlimit", &dval );
		if (field.isOkay())
			view->ViewLimit( dval );

		field = io_cmd->getInt( "solid", &ival );
		if (field.isOkay())
			view->Solid( ival != 0 );


		field = io_cmd->getInt( "white_bg", &ival );
		if (field.isOkay())
		{
			if (ival > 0)
			{
				view->Color( COLOR_BG, DCOLOR_WHITE );
				view->Color( COLOR_FG, DCOLOR_BLACK );
			}
			else
			{
				view->Color( COLOR_BG, DCOLOR_BLACK );
				view->Color( COLOR_FG, DCOLOR_WHITE );
			}
		}
		else
		{
			field = io_cmd->getInt( "color_bg", &ival );
			if (field.isOkay())
				view->Color( COLOR_BG, ival );

			field = io_cmd->getInt( "color_fg", &ival );
			if (field.isOkay())
				view->Color( COLOR_FG, ival );
		}

		field = io_cmd->getInt( "color_hot", &ival );
		if (field.isOkay())
			view->Color( COLOR_HOT, ival );

		field = io_cmd->getInt( "color_sheet", &ival );
		if (field.isOkay())
			view->Color( COLOR_SHEET, ival );

		field = io_cmd->getInt( "color_pat", &ival );
		if (field.isOkay())
			view->Color( COLOR_PATTERN, ival );

		field = io_cmd->getReal( "border", &dval );
		if (field.isOkay())
			view->ViewFactor( dval );

		field = io_cmd->getInt( "gdi", &ival );
		if (field.isOkay())
			view->GDI( ival != 0 );
	}

	return status;
}

// View:ModeGet: [profmark=%b,][hotdot=%b,][tooldash=%b,][picktol=%d,][gridtol=%f][viewlimit=%f]
//				[text=%b][mono=%b][verbatim=%b]
CReturn 
CViewMgr::ModeGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (m_active_view)
	{
		CReturn field;
		int ival;
		double dval;

		field = io_cmd->getInt( "Text", &ival );
		if (field.isOkay())
			io_cmd->setInt( "Text", m_active_view->PrintText() );

		field = io_cmd->getInt( "Verbatim", &ival );
		if (field.isOkay())
			io_cmd->setInt( "Verbatim", m_active_view->PrintVerbatim() );

		field = io_cmd->getInt( "Mono", &ival );
		if (field.isOkay())
			io_cmd->setInt( "mono", m_active_view->Monochrome() );

		field = io_cmd->getInt( "profmark", &ival );
		if (field.isOkay())
			io_cmd->setInt( "profmark", m_active_view->Markers() );

		field = io_cmd->getInt( "workzones", &ival );
		if (field.isOkay())
			io_cmd->setInt( "workzones", m_active_view->ZoneMarkers() );

		field = io_cmd->getInt( "hotdot", &ival );
		if (field.isOkay())
			io_cmd->setInt( "hotdot", m_active_view->HotDot() );

		field = io_cmd->getInt( "tooldash", &ival );
		if (field.isOkay())
			io_cmd->setInt( "tooldash", m_active_view->ToolDash() );

		field = io_cmd->getInt( "picktol", &ival );
		if (field.isOkay())
			io_cmd->setInt( "picktol", m_active_view->PickTol() );

		field = io_cmd->getReal( "gridtol", &dval );
		if (field.isOkay())
			io_cmd->setReal( "gridtol", m_active_view->GridTol() );

		field = io_cmd->getReal( "viewlimit", &dval );
		if (field.isOkay())
			io_cmd->setReal( "viewlimit", m_active_view->ViewLimit() );

		field = io_cmd->getInt( "solid", &ival );
		if (field.isOkay())
			io_cmd->setInt( "solid", m_active_view->Solid() );

		field = io_cmd->getInt( "color_hot", &ival );
		if (field.isOkay())
			io_cmd->setInt( "solid", m_active_view->Color( COLOR_HOT ) );
	}

	return ret;
}

// ==================================================================
// View:ModeDlg:
//
CReturn
CViewMgr::ModeDlg( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;
	CdlgViewOptions dlg(getCWnd());
	if (dlg.DoModal() == IDOK)
	{ dlg.SetViewMode( io_cmd->getViewMgr() ); }
	return ret;
}

// view:resize:
CReturn 
CViewMgr::Resize( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (io_cmd->ModelExists() && m_viewmgr_enabled)
		m_active_view->Resize();

	return STATUS_OKAY;
}

// view:show:
CReturn 
CViewMgr::Show( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (io_cmd->ModelExists() && m_viewmgr_enabled)
		m_active_view->BufferShow( BACK_BUFFER );

	return STATUS_OKAY;
}

// ==================================================================
// View:Clear:
//
CReturn 
CViewMgr::Clear( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return Clear();
}

CReturn 
CViewMgr::Clear( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (m_active_view)
	{
		if (m_viewmgr_enabled)
			m_active_view->Clear( ALL_LISTS );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// ==================================================================
// View:Regenerate: [animate=%d]
//

CReturn 
CViewMgr::ModelSet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	if ( io_cmd->ModelExists() )
		status = ModelSet( io_cmd->getModel() );

	return status;
}

CReturn
CViewMgr::ModelSet( CModel& model )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	if ( m_active_view )
	{
		if (m_viewmgr_enabled)
			m_active_view->ModelSet( model );
	}
	else
	{
#if (!_CI && !_NST)
		status.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return status;
}

CReturn
CViewMgr::ClearEnable( bool enable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	if (m_active_view)
	{
		if (m_viewmgr_enabled)
			m_active_view->ClearEnable( enable );
	}

	return status;
}

CReturn
CViewMgr::ModelShowEnable( bool enable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	if (m_active_view)
	{
		if (m_viewmgr_enabled)
			m_active_view->ModelShowEnable( enable );
	}

	return status;
}

// ==================================================================
// View:Refresh:
//
CReturn 
CViewMgr::Refresh( CCommand* io_cmd )
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool regen = (io_cmd->VarList().getInt( "regen", 0 ) != 0);

	return Refresh( regen );
}

CReturn 
CViewMgr::Refresh( bool regen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (m_active_view)
	{
		if (m_viewmgr_enabled)
			m_active_view->Refresh( regen );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

//
//	Refresh a single entity, by ID... used when it's attributes have
//	changed.
//
CReturn	
CViewMgr::Refresh( ID in_id, bool in_marker )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (m_active_view)
	{
		if (m_viewmgr_enabled)
			m_active_view->Refresh( in_id, in_marker );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

CReturn CViewMgr::Export( CCommand* io_cmd )
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString fname = io_cmd->VarList().getString( "file", "" );
	if (m_active_view && m_viewmgr_enabled)
	{
		if ( !fname.IsEmpty() )
		{
			FILE* f = fopen( fname, "w" );
			if (f != NULL)
			{
				m_active_view->Export( f );
				fclose( f );
			}
		}
	}

	return status;
}

// ==================================================================
// View:Save:
//
CReturn 
CViewMgr::Save( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (m_active_view)
	{
		bool reset = (io_cmd->VarList().getInt( "reset", 0 ) != 0);

		m_active_view->Save( reset );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// ==================================================================
// View:Next:
//
CReturn 
CViewMgr::Next( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;


	if (m_active_view)
	{
		m_active_view->Next( io_cmd->getModel() );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// View:Prev:
CReturn 
CViewMgr::Prev( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;


	if (m_active_view)
	{
		m_active_view->Prev( io_cmd->getModel() );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// View:Full: [regen=%d]
CReturn 
CViewMgr::Full( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;

	bool regen = (io_cmd->VarList().getInt( "regen", 0 ) != 0);

	if (m_active_view)
	{
		m_active_view->Full( io_cmd->getModel(), regen );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// View:MaterialExtents: [regen=%d]
CReturn 
CViewMgr::MaterialExtents( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;

	bool regen = (io_cmd->VarList().getInt( "regen", 0 ) != 0);

	if (m_active_view)
	{
		m_active_view->MaterialExtents( io_cmd->getModel(), regen );
	}

	return ret;
}

// View:Window: [regen=%d,] sx=%f, sy=%f, ex=%f, ey=%f
CReturn 
CViewMgr::Window( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	double		sx, sy;
	double		ex, ey;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;

	bool regen = (io_cmd->VarList().getInt( "regen", 0 ) != 0);

	ret += io_cmd->getReal( STR_SX, &sx );
	ret += io_cmd->getReal( STR_SY, &sy );
	ret += io_cmd->getReal( STR_EX, &ex );
	ret += io_cmd->getReal( STR_EY, &ey );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_VIEW_PARAM_MISSING );
		return ret;
	}

	if (m_active_view)
	{
		m_active_view->Window( C2dCoord( sx, sy), C2dCoord( ex, ey) );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// View:Zoom: scale=%f, x=%f, y=%f
CReturn 
CViewMgr::Zoom( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	double		scale;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;

	ret += io_cmd->getReal( "factor", &scale );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_VIEW_PARAM_MISSING );
		return ret;
	}

	if (m_active_view)
	{
		// Get the default center (hack);
		double xc, yc;
		m_active_view->CenterPt( &xc, &yc );

		// Get the app world coordinates.
		io_cmd->getReal( "x", &xc );
		io_cmd->getReal( "y", &yc );

		m_active_view->Zoom( io_cmd->getModel(), C2dCoord( xc, yc ), scale );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// View:Pan: sx=%f, sy=%f, ex=%f, ey=%f
CReturn 
CViewMgr::Pan( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	double		sx, sy;
	double		ex, ey;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;

	ret += io_cmd->getReal( STR_SX, &sx );
	ret += io_cmd->getReal( STR_SY, &sy );
	ret += io_cmd->getReal( STR_EX, &ex );
	ret += io_cmd->getReal( STR_EY, &ey );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_VIEW_PARAM_MISSING );
		return ret;
	}

	CPoint ps( (int) sx, (int) sy );
	CPoint pe( (int) ex, (int) ey );

	C3dCoord wps;
	C3dCoord wpe;

	m_active_view->mapScreenToWorld( ps, &wps );
	m_active_view->mapScreenToWorld( pe, &wpe );

	if (m_active_view)
	{
		m_active_view->Pan( io_cmd->getModel(),
			C2dCoord( wps.X(), wps.Y()), C2dCoord( wpe.X(), wpe.Y() ) );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// View:Rotate: sx=%f, sy=%f, ex=%f, ey=%f
CReturn 
CViewMgr::Rotate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	double		sx, sy;
	double		ex, ey;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;


	ret += io_cmd->getReal( STR_SX, &sx );
	ret += io_cmd->getReal( STR_SY, &sy );
	ret += io_cmd->getReal( STR_EX, &ex );
	ret += io_cmd->getReal( STR_EY, &ey );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_VIEW_PARAM_MISSING );
		return ret;
	}

	if (m_active_view)
	{
		m_active_view->Rotate( io_cmd->getModel(),C2dCoord( sx, sy), C2dCoord( ex, ey) );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}

// ==================================================================
// View:Angle: xy=%f, yz=%f, zx=%f
//
CReturn 
CViewMgr::Angle( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	double		xy = 0.0;
	double		yz = 0.0;
	double		zx = 0.0;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;


	io_cmd->getReal( "xy", &xy );
	io_cmd->getReal( "yz", &yz );
	io_cmd->getReal( "zx", &zx );

	if (m_active_view)
	{
		m_active_view->Angle( io_cmd->getModel(),xy, yz, zx );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}


// ==================================================================
// View:Plane: id=%d
//
CReturn 
CViewMgr::Plane( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	ID			id;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;


	ret += io_cmd->getInt( "id", (int*)&id );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_VIEW_PARAM_MISSING );
		return ret;
	}

	if (m_active_view)
	{
		m_active_view->Plane( io_cmd->getModel(), id );
	}
	else
	{
#if (!_CI && !_NST)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif
	}

	return ret;
}


// ==================================================================
// View:RubberStart: mode=%d, x=%f, y=%f
//
CReturn 
CViewMgr::RubberStart( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	double		mouse_x, mouse_y;
	eRubber		mode;

	if (!io_cmd->ModelExists() || !m_viewmgr_enabled)
		return ret;


	ret += io_cmd->getReal( STR_X, &mouse_x );
	ret += io_cmd->getReal( STR_Y, &mouse_y );

	ret += io_cmd->getInt( STR_MODE, (int*)&mode );

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

	if ( ret.isOkay() )
	{
		m_active_view->RubberStart( mode, C3dCoord( mouse_x, mouse_y, 0.0 ), io_cmd->getModel() );
	}

	return ret;
}


// View:RubberStop:
CReturn 
CViewMgr::RubberStop( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

	if ( ret.isOkay()  || !m_viewmgr_enabled)
		m_active_view->RubberStop();

	return ret;
}

// ==================================================================
// View:Mouse: x=%f, y=%f
//
CReturn 
CViewMgr::Move( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	static double last_x = 0.;
	static double last_y = 0.;

	CReturn		ret;

	if (m_viewmgr_enabled)
	{
		double		mouse_x, mouse_y;
		ret += io_cmd->getReal( STR_X, &mouse_x );
		ret += io_cmd->getReal( STR_Y, &mouse_y );
		
		bool snap = (io_cmd->VarList().getInt( STR_SNAP, TRUE ) != 0);

		if (!m_active_view)
			return ret;

		if ( ret.isOkay() )
		{
			ret += m_active_view->Move( C3dCoord( mouse_x, mouse_y, 0.0 ), snap, io_cmd );

			io_cmd->getReal( "px", &last_x );
			io_cmd->getReal( "py", &last_y );
		}
		else
		{
			// Return the last known coordinates.
			// Introduced (as a hack) for mousewheel zoom.
			io_cmd->setReal( "px", last_x );
			io_cmd->setReal( "py", last_y );
		}
	}

	return ret;
}

// ==================================================================
// View:Pick: x=%f, y=%f
//
CReturn 
CViewMgr::Pick( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	if (m_viewmgr_enabled)
	{
		double		mouse_x, mouse_y;

		ret += io_cmd->getReal( STR_X, &mouse_x );
		ret += io_cmd->getReal( STR_Y, &mouse_y );

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

		if ( ret.isOkay() )
		{
			ret += m_active_view->Pick( C3dCoord( mouse_x, mouse_y, 0.0 ), io_cmd );
		}
	}

	return ret;
}

// ==================================================================
// View:Highlight: id=%d
//
CReturn 
CViewMgr::Highlight( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	if (m_viewmgr_enabled)
	{
		ID			id;

		ret += io_cmd->getInt( "Id", (int*)&id );

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

		if ( ret.isOkay() )
		{
			ret += m_active_view->Highlight( id );
		}
	}

	return ret;
}

// ==================================================================
// View:Erase: id=%d
//
CReturn 
CViewMgr::Erase( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	if (m_viewmgr_enabled)
	{
		ID			id;

		ret += io_cmd->getInt( "Id", (int*)&id );

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

		if ( ret.isOkay() )
		{
			ret += m_active_view->Erase( id );
		}
	}

	return ret;
}

// ==================================================================
// View:Draw: id=%d
//
CReturn 
CViewMgr::Draw( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	if (m_viewmgr_enabled)
	{
		ID			id;

		ret += io_cmd->getInt( "Id", (int*)&id );

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

		if ( ret.isOkay() )
		{
			ret += m_active_view->Draw( id );
		}
	}

	return ret;
}


// ==================================================================
void	
CViewMgr::mapScreenToWorld( 
	const CPoint&	in_screen, 
	C3dCoord*		io_world ) const
{
	if (!m_active_view)
	{
		io_world->X( 0.0 );
		io_world->Y( 0.0 );
		return;
	}

	m_active_view->mapScreenToWorld( in_screen, io_world );
}

void	
CViewMgr::mapRefToWorld( 
	const C3dCoord&	ref, 
	C3dCoord*		world ) const
{
	if (!m_active_view)
	{
		world->X( 0.0 );
		world->Y( 0.0 );
		return;
	}

	CDbWorkplane* db_work = m_active_view->ActiveModel()->ActiveWorkplane();
	m_active_view->mapRefToWorld( ref, db_work, world);
}

void	
CViewMgr::mapWorldToRef( 
	const C3dCoord&	world, 
	C3dCoord*		ref ) const
{
	if (!m_active_view)
	{
		ref->X( 0.0 );
		ref->Y( 0.0 );
		return;
	}

	CDbWorkplane* db_work = m_active_view->ActiveModel()->ActiveWorkplane();
	m_active_view->mapWorldToRef( world, db_work, ref );
}

void
CViewMgr::mapWorldToScreen( const C3dCoord& world, CPoint* io_screen ) const
{
	if (!m_active_view)
	{
		io_screen->x = 0;
		io_screen->y = 0;
		return;
	}

	m_active_view->mapWorldToScreen( world, io_screen );
}

void
CViewMgr::mapViewToScreen( const C3dCoord& view, CPoint* io_screen ) const
{
	if (!m_active_view)
	{
		io_screen->x = 0;
		io_screen->y = 0;
		return;
	}

	m_active_view->mapViewToScreen( view, io_screen );
}



// ==================================================================

CViewBase* CViewMgr::ViewBaseGet()
	{ return ( CViewMgr::m_active_view ); }

CWnd* CViewMgr::getCWnd( void )
	{ return (m_active_view ? m_active_view->getCWnd() : NULL); }


// ==================================================================
//	View:Print: [Quiet=%b] [Wmf=%b] [file=%s]
CReturn 
CViewMgr::Print( CCommand* io_cmd )
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	if (!io_cmd->ModelExists())
		return ret;

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

	if ( ret.isOkay() )
	{
		bool quiet = (io_cmd->VarList().getInt( "Quiet", FALSE ) != 0);
		bool wmf = (io_cmd->VarList().getInt( "Wmf", FALSE ) != 0);
		CString file = io_cmd->VarList().getString( "file", "" );

		m_active_view->Print( io_cmd->getModel(), quiet, wmf, file);
	}

	return ret;
}


// ==================================================================
// For use by Java
CReturn 
CViewMgr::RGB_Value( CCommand* io_cmd )
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int red   = io_cmd->VarList().getInt( "red", 255 );
	int green = io_cmd->VarList().getInt( "green", 255 );
	int blue  = io_cmd->VarList().getInt( "blue", 255 );

	io_cmd->setInt( "rgb", RGB( red, green, blue ) );

	return CReturn( STATUS_OKAY );
}


// ==================================================================
//		Annotate
//
//	Print text to the view... processes /n characters, too.
//
CReturn CViewMgr::Annotate( CString* text )  // If NULL, reset to top line
{
	CReturn ret;

	if (m_viewmgr_enabled)
	{
#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

		if ( ret.isOkay() )
		{
			m_active_view->Annotate( text );
		}
	}

	return ret;

}


// ==================================================================
void	
CViewMgr::DebugLine( C3dCoord& st, C3dCoord& en )
{
	CReturn ret;

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

	if ( ret.isOkay() )
	{
		m_active_view->DebugLine( st, en );
	}
}

// ==================================================================
void	
CViewMgr::RapidLine( C3dCoord& st, C3dCoord& en, int clr )
{
	CReturn ret;

#if (!_CI && !_NST)
	if (!m_active_view)
		ret.Internal( IDS_VIEW_NO_ACTIVE );
#endif

	if ( ret.isOkay() )
	{
		m_active_view->RapidLine( st, en, clr );
	}
}


// ==================================================================
CglCanvas*	
CViewMgr::GetCanvas( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_debug_canvas)
	{
		CWnd* cWnd = getCWnd();
		m_debug_canvas = new CglCanvas( cWnd );
		m_debug_canvas->Init();
	}

	return m_debug_canvas;
}

void			
CViewMgr::KillCanvas( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_debug_canvas) delete m_debug_canvas;
	m_debug_canvas = NULL;
}



CViewBase*	
CViewMgr::ActiveView( void )
{ 
	return m_active_view;
}

CViewBase* 
CViewMgr::ViewFind( ID view_id )
{
	CViewBase* view = NULL;

	if (view_id > 0)
	{
		int count = m_view_list.GetSize();
		for (int indx = 0; indx < count; ++indx)
		{
			view = m_view_list.GetAt( indx );
			if (view->getID() == view_id)
				break;
		}
	}

	return view;

}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: It seems kind of silly to have so many redundant drawing methods
// (ie. one for elem list, curve list, elem array, curve array) but that
// may be my ignorance.

CGeoRenderer::CGeoRenderer()
{
	m_open = false;
	m_color = DCOLOR_RED;

	m_origin.XYZ( 0, 0, 0 );
}

CGeoRenderer::~CGeoRenderer()
{
}

void CGeoRenderer::DisplayListOpen()
{
	CViewBase* vbase = CViewMgr::ActiveView();
	if ((vbase != NULL) && !m_open)
	{
		vbase->DisplayListBegin( PTEMP_LIST );
		m_open = true;
	}
}

void CGeoRenderer::DisplayListClose()
{
	CViewBase* vbase = CViewMgr::ActiveView();
	if ((vbase != NULL) && m_open)
	{
		vbase->DisplayListEnd( PTEMP_LIST );
		m_open = false;
	}
}

void CGeoRenderer::Draw()
{
	CViewBase* vbase = CViewMgr::ActiveView();
	if ((vbase != NULL) && !m_open)
	{
		vbase->BufferShow( FRONT_BUFFER );
	}
}

eDisplayColor CGeoRenderer::DrawColorSet( eDisplayColor color )
{
	eDisplayColor prev = m_color;
	m_color = color;
	return prev;
}

void CGeoRenderer::WindowSet( const C2dBox& box )
{
	CViewBase* vbase = CViewMgr::ActiveView();
	if ((vbase != NULL) && !m_open)
	{
		vbase->Window( box.BL(), box.TR() );
		vbase->Clear( ALL_LISTS );
	}
}

void CGeoRenderer::OriginSet( double x, double y, double z )
{
	m_origin.XYZ( 0, 0, 0 );
}

void CGeoRenderer::OriginSet( const C3dCoord& pt )
{
	m_origin = pt;
}

void CGeoRenderer::DrawGeo( const CGeoElem& geo )
{
	CViewBase* vbase = CViewMgr::ActiveView();
	if (vbase != NULL)
	{
		bool oneshot = !m_open;
		if ( oneshot )
			vbase->DisplayListBegin( PTEMP_LIST );

		vbase->DrawAtColor( oneshot ? DCOLOR_RED : m_color );
		vbase->DrawGeoAt( geo, m_origin );

		if ( oneshot )
		{
			vbase->DisplayListEnd( PTEMP_LIST );
			vbase->BufferShow( FRONT_BUFFER );
		}
	}
}

void CGeoRenderer::DrawGeo( const CGeoElemList& list )
{
	CViewBase* vbase = CViewMgr::ActiveView();
	if (vbase != NULL)
	{
		int count = list.Count();
		if (count > 0)
		{
			bool oneshot = !m_open;
			if ( oneshot )
				vbase->DisplayListBegin( PTEMP_LIST );

			vbase->DrawAtColor( oneshot ? DCOLOR_RED : m_color );

			for (int indx = 0; indx < count; ++indx)
			{
				CGeoElem* geoelem = list.GetAt( indx );
				vbase->DrawGeoAt( *geoelem, m_origin );
			}

			if ( oneshot )
			{
				vbase->DisplayListEnd( PTEMP_LIST );
				vbase->BufferShow( FRONT_BUFFER );
			}
		}
	}
}

void CGeoRenderer::DrawGeo( const CGeoElemArray& array )
{
	CViewBase* vbase = CViewMgr::ActiveView();
	if (vbase != NULL)
	{
		int count = array.Count();
		if (count > 0)
		{
			bool oneshot = !m_open;
			if ( oneshot )
				vbase->DisplayListBegin( PTEMP_LIST );

			vbase->DrawAtColor( oneshot ? DCOLOR_RED : m_color );

			for (int indx = 0; indx < count; ++indx)
			{
				CGeoElem* geoElem = array.GetAt( indx );
				vbase->DrawGeoAt( *geoElem, m_origin );
			}

			if ( oneshot )
			{
				vbase->DisplayListEnd( PTEMP_LIST );
				vbase->BufferShow( FRONT_BUFFER );
			}
		}
	}
}
