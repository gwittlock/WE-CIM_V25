// ==================================================================
//		View Wire
//
//	Wire-frame renderer
//
// ==================================================================

#include "stdafx.h"
#include "float.h"

#include "cmn_resource.h"

#include "MathConst.h"
#include "StringConst.h"

#include "DbAllEntities.h"
#include "DisplayEntity.h"
#include "DbIterator.h"

#include "PrinterSettings.h"

#include "oglViewWire.h"

//=-=-=-=-=

#define OPENGL_EXPERIMENT 0
#define	CLASS_NAME		"WecimOpenGL"
#define WINDOW_TITLE	"WE-CIM Graphics"

HINSTANCE g_hInstance = 0;

LRESULT	CALLBACK oglWndProc(HWND, UINT, WPARAM, LPARAM);

//=-=-=-=-=


// ==================================================================
// To prevent overhead of char* to CString conversion
const CString STR_END( "end" );
const CString STR_PX( "px" );
const CString STR_PY( "py" );
const CString STR_PZ( "pz" );

// ==================================================================

const double SUB_PIXEL = 0.50;
const double MARK_PIXEL = 1.0;
const double PRINT_PIXEL = 0.75;
const double DRAG_PIXEL = SUB_PIXEL*4;

const double MINIMUM_DELAY = 0.1;

static int view_cnt = 0;

static int view_angle = 0;

// ==================================================================

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: Without instantiating g_app, AfxGetApp() returns NULL.
//       This is critical to obtaining the printer defaults!
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWinApp g_app;

// ==================================================================

CoglViewWire::CoglViewWire( ID in_id )
	: CViewBase( in_id )
{
	m_canvas = NULL;
	m_model = NULL;

	m_hot_color = DCOLOR_RED;
	m_white = DCOLOR_WHITE;
	m_black = DCOLOR_BLACK;
	m_background_color = DCOLOR_BLACK;
// Experiment -- sort of an off-white.
// m_background_color = 0xf0f0f0;
	m_foreground_color = DCOLOR_WHITE;

//	m_pattern_color = 0x606060;
	m_pattern_color = 0x3f3f3f;
	m_sheet_color = 0xd7b9b0;

	m_font_height = 10;
	// m_font_name = "Times New Roman";
	m_font_name = "Arial";  // looks cleaner wrt outline font rendering

	m_rubber_mode = RUBBER_OFF;

	m_drawing = false;
	m_drawtype = -1;

	m_printing = false;
	m_pen = NULL;
	m_old_pen = NULL;

	m_style = (eDisplayStyle)-1;
	m_meta = META_NONE;

	m_pat_id = -1;
	m_pat_selected = false;

	m_DrawAt_color = DCOLOR_RED;
	m_DrawAt_style = DSTYLE_SOLID;

	m_hot_entity = NULL;
	m_hot_id = 0;

	m_regen = FALSE;

	m_export_file = NULL;
}

#if OPENGL_EXPERIMENT

CoglViewWire::~CoglViewWire()
{
	ogl_purge();

	if (g_hInstance != 0)
	{
		UnregisterClass( CLASS_NAME, g_hInstance );
		g_hInstance = 0;
	}
}

#else

CoglViewWire::~CoglViewWire()
{
	ogl_purge();
}

#endif

void
CoglViewWire::ogl_purge( void )
{
	if (m_canvas)
	{
		delete m_canvas;
		m_canvas = NULL;
	}
}

// ==================================================================
//		Create
//
CReturn 
CoglViewWire::Create( HDC in_dc )
{
	CReturn	ret;

	ret = CViewBase::Create( in_dc );

	CWnd* wnd = getCWnd();

	m_canvas = new CglCanvas(wnd);
	m_canvas->Init();
	m_canvas->LoadFont( m_font_name, m_font_height );

	return ret;
}

#if OPENGL_EXPERIMENT

CReturn 
CoglViewWire::Create( HWND in_hwnd )
{
	CReturn	status;

	if ( RegisterWindowClass() )
	{
		CRect	rect;
		HWND	ogl_hwnd = 0;

		if ( GetClientRect( in_hwnd, &rect ) );
		{
			// DWORD windowStyle = WS_OVERLAPPEDWINDOW ;							// Define Our Window Style
			DWORD windowStyle = WS_CHILD;							// Define Our Window Style
			// DWORD windowExtendedStyle = WS_EX_APPWINDOW;
			DWORD windowExtendedStyle = WS_EX_LEFT;

			 WindowRectEx( &rect, windowStyle, 0, windowExtendedStyle );

			// NOTE: When in_hwnd becomes the parent of ogl_hwnd.
			// This is *critical* to placing the ogl window.
			ogl_hwnd = CreateWindowEx(
				windowExtendedStyle,
				CLASS_NAME, WINDOW_TITLE,
				windowStyle,
				rect.left, rect.top,
				rect.Width(), rect.Height(),
				in_hwnd,  // use we-cim's as the parent (?)
				NULL, NULL, NULL );
		}

		status = CViewBase::Create( ogl_hwnd );
		if ( status.IsOk() )
		{
			CWnd* window = getCWnd();

			window->SetFocus();

			m_canvas = new CglCanvas(window);
			m_canvas->Init();
			m_canvas->LoadFont( m_font_name, m_font_height );

			m_canvas->OglReport();
		}
	}
	else
	{
		status.Fatal( IDS_INTERNAL_ERROR, "CoglViewWire::Create(#1)" );
	}

	return status;
}

#else

CReturn CoglViewWire::Create( HWND in_hwnd )
{
	CReturn	ret = CViewBase::Create( in_hwnd );

	CWnd* window = getCWnd();
	window->SetFocus();

	m_canvas = new CglCanvas(window);
	m_canvas->Init();
	m_canvas->LoadFont( m_font_name, m_font_height );

	m_canvas->OglReport();

	return ret;
}

#endif


// ==================================================================
//		Regenerate
//
//	Re-build our lists of entities, and redraw the new information
//
void	
CoglViewWire::ModelSet( CModel& in_model )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_model = &in_model;
}

// NOTE: Something is still not quite right with CoglViewWire::Resize()
void CoglViewWire::Resize()
{
	CRect screen;
	CWnd* window = getCWnd();
	window->GetClientRect( &screen );

	C2dBox extent = m_canvas->ViewExtent();

	C2dCoord pc( extent.Xc(), extent.Yc() );

	double dx = extent.Dx();
	double dy = extent.Dy();

	// Determine our viewscale...
	double scale = min(
		(double) screen.Width() / dx,
		(double) screen.Height() / dy );

	if (scale >= m_canvas->ViewScale())
	{
		// Determine our viewscale...
		scale = max(
			(double) screen.Width() / dx,
			(double) screen.Height() / dy );
	}

	dx = (screen.Width() * 0.5) / scale;
	dy = (screen.Height() * 0.5) / scale;

	extent.Update( pc.X()-dx, pc.Y()-dy, pc.X()+dx, pc.Y()+dy );

	refresh_setup( screen, extent );
}

// ==================================================================
//		Clear
//	
//		Clear the screen...
//
void
CoglViewWire::Clear( EDisplayList which )
{
	if (which & MODEL_LIST)
	{
		m_canvas->ModelBegin();
		m_canvas->ModelEnd();
	}

	if (which & STEMP_LIST)
	{
		m_canvas->SemiTempBegin();
		m_canvas->SemiTempEnd();
	}

	if (which & PTEMP_LIST)
	{
		m_canvas->PureTempBegin();
		m_canvas->PureTempEnd();
	}
}

void
CoglViewWire::DisplayListBegin( EDisplayList which )
{
	if (which & MODEL_LIST)
		m_canvas->ModelBegin();

	if (which & STEMP_LIST)
		m_canvas->SemiTempBegin();

	if (which & PTEMP_LIST)
		m_canvas->PureTempBegin();
}

void
CoglViewWire::DisplayListEnd( EDisplayList which )
{
	if (which & MODEL_LIST)
		m_canvas->ModelEnd();

	if (which & STEMP_LIST)
		m_canvas->SemiTempEnd();

	if (which & PTEMP_LIST)
		m_canvas->PureTempEnd();
}

void
CoglViewWire::BufferShow( EViewBuffer which )
{
	m_canvas->Show( ((which == FRONT_BUFFER) ? GL_FRONT : GL_BACK) );
}

// ==================================================================
// view:refresh:
//	Re-draw the information, as it already sits in our lists
void CoglViewWire::Refresh( bool regen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_model)
		return;  // Nothing to do.

	m_regen = (regen ? TRUE : FALSE);

	CDbEntity::NewAction();

	not_hot();

	m_canvas->Start();

	CRect	screen;

	CWnd* window = getCWnd();
	window->GetClientRect( &screen );
	refresh_setup( screen, m_canvas->ViewExtent() );

	int prev_bg = m_background_color;
	int prev_fg = m_foreground_color;
	if ( Monochrome() )
	{
		m_foreground_color = DCOLOR_BLACK;
		m_background_color = DCOLOR_WHITE;
	}


	m_canvas->ClearColor( m_background_color );

	m_canvas->ModelBegin();

	// Recompute all entity display information.

	CDbPattern*	dbPattern = m_model->ActivePattern();
	if (dbPattern != NULL)
	{
		CDbWorkplane* dbWork = m_model->ActiveWorkplane();

		if (dbWork != NULL)
			draw_entity( dbWork, false, -1 );

		draw_entity( dbPattern, -1 );
		draw_entity( dbPattern, false, -1 );
	}
	else
	{
		draw_entities( m_model, -1 );
	}

	m_canvas->ModelEnd();

	BufferShow( BACK_BUFFER );

	m_background_color = prev_bg;
	m_foreground_color = prev_fg;

	m_regen = FALSE;
}

void	
CoglViewWire::Refresh( ID id, bool in_marker )
{
	if (!m_model)
		return;

	// Force draw, even if outside of view box... for nesting, where view isn't well defined
	CDbEntity::NewAction();

	m_canvas->Start();

	m_canvas->PureTempBegin();

	// NOTE: An id of zero is used to clear temporary graphics.
	if (id > 0)
		refresh_id( id, in_marker );

	m_canvas->PureTempEnd();

	BufferShow( FRONT_BUFFER );
}

void CoglViewWire::Export( FILE* f )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ((f == NULL) || (m_model == NULL))
		return;  // Nothing to do.

	m_export_file = f;
	Refresh( false );
	m_export_file = NULL;
}

// ==================================================================

void
CoglViewWire::refresh_setup( CRect& in_screen, const C2dBox& view_extent )
{

	CRect viewport = m_canvas->Viewport();
	int min_maxdim = min( viewport.Width(), viewport.Height() );
	int maxscr = max(in_screen.Width(), in_screen.Height());

	double dx_screen = in_screen.Width();
	double dy_screen = in_screen.Height();
	if (min_maxdim < maxscr)
	{
		double scr_scale = (double)min_maxdim / (double)maxscr;
		m_printscale = 1.0 / scr_scale;

		dx_screen *= scr_scale;
		dy_screen *= scr_scale;
	}

	// m_scrnctr is needed for the CoglViewWire::map.*() functions
	CRect screen( 0, 0, (int) dx_screen, (int) dy_screen );
	m_scrnctr = screen.CenterPoint();

	// Set viewport to cover the window.
	// NOTE: This also causes CoglCanvas to calculate the view scale.
	m_canvas->Viewport( screen, view_extent );

	if (m_printing)
	{
		glGetDoublev(GL_MODELVIEW_MATRIX, m_model_mat );
		glGetDoublev(GL_PROJECTION_MATRIX, m_proj_mat );
		glGetIntegerv(GL_VIEWPORT, m_viewport );

		m_color = -1;
		m_style = (eDisplayStyle)-1;
	}

	// Tracking values, reset
	C3dCoord empty;
	m_tooltip = empty;
	m_at = empty;
}

void
CoglViewWire::refresh_id( ID in_id, bool marker )
{
	CDbEntity* dbEntity = NULL;
	m_model->EntityFind( in_id, &dbEntity );
	refresh_entity( dbEntity, marker );
}

void
CoglViewWire::refresh_entity( CDbEntity* dbEntity, bool marker )
{
	if (dbEntity != NULL)
	{
		// We have to take explicit control over tool drawing 
		// and highlighting -- it was implied in the older view
		C3dCoord empty;
		m_tooltip = empty;
		CDbCurve* curve = dynamic_cast<CDbCurve*>(dbEntity);
		if (curve)
		{
			CDbProfile* prof = dynamic_cast<CDbProfile*>(dbEntity->Owner());
			if (prof && ((*prof)[0] != curve))
			{
				// Suppress draw display if inside (not start) of a profile
				m_tooltip = curve->StartPt(0);
			}
		}

		// Because we want to force regeneration for this entity!
		int prev_regen = m_regen;
		m_regen = TRUE;

		CDbContainer* dbContainer = dynamic_cast<CDbContainer*>(dbEntity);
		if (dbContainer == NULL)
		{
			draw_entity( dbEntity, marker, -1 );
		}
		else
		{ 
			draw_entity( dbContainer, -1 );
			draw_entity( dbEntity, marker, -1 );
		}

		m_regen = prev_regen;
	}
}


// ==================================================================
//		Move and Pick
//
//	Simple mouse interactions with the model
//
CReturn
CoglViewWire::Move( 
	const C3dCoord& in_mouse,
	bool			dosnap,		// TRUE if we want to snap to points
	CCommand*		io_cmd )
{
	CReturn status;

	if (!m_model)
		return STATUS_OKAY;

	CDbWorkplane* db_work = io_cmd->getModel().ActiveWorkplane();
	if (db_work == NULL)
		return STATUS_OKAY;

	const CModel& model = io_cmd->getModel();

	if ( m_canvas->Start() )
	{
		double viewscale = m_canvas->ViewScale();
		if ( ZERO(viewscale) )
			viewscale = 1.;

		double world_tolerance = PickTol() / viewscale;
		const double TOLERANCE_LIMIT = SMALL * 10;

		if (world_tolerance < TOLERANCE_LIMIT)
			world_tolerance = TOLERANCE_LIMIT;

		C3dCoord mouse( in_mouse.X(), in_mouse.Y(), 0. );

		// --------------------------------------------------------------
		//
		//	Track the mouse... and see if it is near something...
		//

		C3dCoord	snap;
		C3dCoord	pnt;

		bool end = FALSE;
		bool do_dot = FALSE;

		CDbEntity* dbEntity = NULL;
		if ( dosnap )
		{
			status = find_closest( world_tolerance, mouse,
				&dbEntity, &do_dot, &snap, &end );
		}


		ID id = ((dbEntity == NULL) ? 0 : dbEntity->Id());
		io_cmd->setInt( "id", id );

		bool update = FALSE;

		// Generate feedback values for VB.
	//	if (dosnap && (id > 0))
		if (do_dot || (dosnap && (id > 0)))
		{
			mapWorldToRef( snap, db_work, &pnt );

			io_cmd->setReal( STR_PX, pnt.X() );
			io_cmd->setReal( STR_PY, pnt.Y() );
			io_cmd->setReal( STR_PZ, pnt.Z() );
			io_cmd->setInt( STR_END, end );

			update = TRUE;
		}
		else
		{
			double grid = GridTol();
			if ( ZERO(grid) )
				grid = 1.;

			mapWorldToRef( mouse, db_work, &pnt );

			int ix = (int)(pnt.X() / grid + 0.5);
			int iy = (int)(pnt.Y() / grid + 0.5);
			int iz = (int)(pnt.Z() / grid + 0.5);

			io_cmd->setReal( STR_PX, ix * grid );
			io_cmd->setReal( STR_PY, iy * grid );
			io_cmd->setReal( STR_PZ, iz * grid );
		}

		m_hot_dot = do_dot;


		// Update the arrowhead when the id changes OR when
		// we're simply moving the cursor along an entity.
		//   if (update || (id > 0))
		//
		// On second thought, only update the arrowhead when
		// the cursor first makes contact with the entity.
		// That way, we can mimize calls to SwapBuffers().
		m_canvas->PureTempBegin();

		CDbEntity* old_hot_entity = m_hot_entity;
		if ( update )
		{
			// CRITICAL: The following statement is critical to
			// redrawing the hot entity using the 'hot' color.
			m_hot_entity = dbEntity;
			m_hot_id = id;

			// Redraw the entities.
			refresh_entity( old_hot_entity, TRUE );
			refresh_entity( m_hot_entity, TRUE );

			// Update the arrow tracking.
			if ( dosnap	|| m_hot_dot )
				hot_arrow( model, snap );
			else
				hot_arrow( model, mouse );
		}
		else if ((m_hot_entity != NULL) && !m_hot_entity->IsDeleted())
		{
			// CRITICAL: The order of statements here is critical
			// to redrawing the (formerly) hot entity using its
			// original color.
			m_hot_entity = NULL;
			m_hot_id = 0;
			refresh_entity( old_hot_entity, TRUE );
			update = TRUE;
		}

		// Track any rubber line stuff...
		if (m_rubber_mode != RUBBER_OFF)
		{
			C3dCoord world;

			mapRefToWorld( pnt, db_work, &world );

			if (!world.WithinTolXY(m_rubber_end, 2./viewscale ))
			{
				rubber( world );			// visible, new
	//			rubber( m_rubber_end );		// invisible, old
				m_rubber_end = world;
			}

			update = TRUE;
		}

		m_canvas->PureTempEnd();

		if ( update )
		{
			// NOTE: It may be possible to improve performance
			// and/or behavior using the following sequence:
			// 1) m_canvas->BackBuffer( BACK_BUFFER );
			// 2) update temporary display list
			// 3) m_canvas->Show( FRONT_BUFFER );

			BufferShow( BACK_BUFFER );
		}
	}

	return status;
}

CReturn
CoglViewWire::Pick( 
	const C3dCoord& in_mouse,
	CCommand*			io_cmd )
{
	CReturn	status;

	if (!m_model)
		return status;

	status = Move( in_mouse, TRUE, io_cmd );
	if (!status.isOkay())
	{
		return status;
	}

	ID id = 0;
	status += io_cmd->getInt( "id", (int*)&id );
	if (!status.isOkay())
		return status;

	CDbEntity* entity = NULL;
	status += io_cmd->getModel().EntityFind( id, &entity );
	if ( !status.isOkay()
		|| !entity)
		return status;

	if ( entity->IsSystem()
		  && entity->IsRefd() )
	{
		// Don't pick system points that make up other geometry.
		// Bubble up to the referrer -- use the first one
		// IT#17
		CDbEntityList list;
		entity->RefdBy( &list );
		entity = list[0];

		io_cmd->setInt( "id", entity->Id() );
	}

	io_cmd->setInt( STR_TYPE, entity->Type() );
	switch (entity->Type())
	{
	case DBLINE:
		{
			CDbLine* db_line = (CDbLine*)entity;
			const CGeoLine* line = db_line->Line();

			io_cmd->setReal( "len", line->Length2d() );
			io_cmd->setReal( "ang", line->StartTan().Radians() );

			delete line;
		}
		break;

	case DBARC:
		{
			CDbArc* db_arc = (CDbArc*)entity;

			io_cmd->setReal( "rad", db_arc->Radius() );
		}
		break;
	}

	return status;
}

// ==================================================================

CReturn
CoglViewWire::Highlight( ID in_id )
{
	if ((m_model != NULL) && (in_id != m_high_id))
	{
		m_canvas->Start();

		ID old_id = m_high_id;
		m_high_id = in_id;

		m_canvas->PureTempBegin();

		CDbEntity::NewAction();
		refresh_id( old_id, TRUE );
		refresh_id( m_high_id, TRUE );

		m_canvas->PureTempEnd();

		// TODO: Fix display behavior (?)
		BufferShow( FRONT_BUFFER );
	}

	return STATUS_OKAY;
}

// ==================================================================
CReturn
CoglViewWire::Erase( ID in_id )
{
	if (!m_model)
		return STATUS_OKAY;

	// Searching here and in refresh_id is a bit of a waste
	CDbEntity* db_ent = NULL;
	m_model->EntityFind( in_id, &db_ent );
	if (!db_ent)
		return STATUS_OKAY;

	CVarList* att = db_ent->pAttrib();

	int color = att->getColor( DCOLOR_RED );
	att->setColor( m_background_color );
	att->setInt( "Erase", 1 );

	int hot = m_hot_id;
	m_hot_id = -1;

	int high = m_high_id;
	m_high_id = -1;

	bool sel = db_ent->IsSelected();
	db_ent->SelectableFlag( false );

	CDbEntity::NewAction();

	m_canvas->Start();
	refresh_id( in_id, TRUE );

	// TODO: Fix display behavior (?)
	BufferShow( BACK_BUFFER );

	if (sel)
		db_ent->SelectableFlag( true );

	m_high_id = high;
	m_hot_id= hot;

	att->setColor( color );
	att->deleteVar( "Erase" );

	return STATUS_OKAY;
}

// ==================================================================
CReturn
CoglViewWire::Draw( ID in_id )
{
	if (m_model != NULL)
	{
		CDbEntity::NewAction();

		m_canvas->Start();
		refresh_id( in_id, TRUE );

		// TODO: Fix display behavior (?)
		BufferShow( BACK_BUFFER );
	}


	return STATUS_OKAY;
}


// ==================================================================
//		find_closest
//
//	Find and return the ID of the closest entity.
//
static EDbEntityType g_pickorder[] =
{
	DBFEATURE,
	DBPROFILE,
	DBCOMMAND,
	DBLINE,
	DBARC,
	DBHOLE,
	DBPOINT,
	DBTERMINAL
};

bool IsWorkZone( const CDbEntity* dbEntity )
{
	const CDbFeature* dbFeature =
		dynamic_cast<const CDbFeature*>( dbEntity );

	return ((dbFeature != NULL) && dbFeature->IsWorkZone());
}

bool IsStock( const CDbEntity* dbEntity )
{
	CDbTool* dbTool = dbEntity->Tool();
	return ((dbTool != NULL) && (dbTool->Name().CompareNoCase(STR_STOCK) == 0));
}

bool CoglViewWire::IsSnappable( const CDbEntity* dbEntity )
{
	CDbTool* dbTool = dbEntity->Tool();
	return ((dbTool != NULL) && dbTool->IsSnappable());
}

bool CoglViewWire::IsHotDottable( const CDbEntity* dbEntity )
{
	bool is_dottable = FALSE;

	if ((dbEntity->Type() != DBPROFILE) && HotDot())
	{
		CDbTool* dbTool = dbEntity->Tool();
		is_dottable = ((dbTool != NULL) && dbTool->IsHotDot());
	}

	return is_dottable;
}

bool CoglViewWire::SupportsInteraction( const CDbEntity* dbEntity )
{
	return (IsSnappable( dbEntity ) || IsHotDottable( dbEntity ));
}

CReturn
CoglViewWire::find_closest( 
	double				dist,		// Maximum distance
	const C2dCoord&		pnt,			// In view?
	CDbEntity**			dbEntity,		// Closest ID
	bool*				io_dot,
	C3dCoord*			io_snap,
	bool*				io_end )		// 0 start, 1 end
{
	CReturn		ret;
	CDbIterator	iter;
	C3dCoord	near_pnt;
	bool		end;

	double check_dist = dist * 2;
	double max_dist = dist;
	bool min_dot = FALSE;

	ret.setStatus( STATUS_ERROR );

	(*dbEntity) = NULL;
	(*io_dot) = FALSE;

	// For intersection tests
	CDisplayEntity* ent_1 = NULL;
	CDisplayEntity* ent_2 = NULL;

	// For each display entity...
	int pickidx = 0;
	while (TRUE)
	{
		EDbEntityType picktype = g_pickorder[pickidx++];
		if (picktype == DBTERMINAL)
			break;

		iter.Init( m_model->Db(), picktype );
		while (TRUE)
		{
			CDbEntity* entity = iter();
			iter.Next();
			if (!entity)
				break;

			EDbEntityType type = entity->Type();
			if (type != picktype)
				break;

			if ( !entity->canDescribe2d() )
				continue;

			if ( !SupportsInteraction( entity ) )
				continue;

			// Must must call CModel::is_hidden() instead of
			// CDbEntity::is_hidden() because we must consider
			// whether there is an active pattern.
			if ( m_model->is_hidden(entity) )
				continue;

			// Even though the workzones a may not be visible,
			// prevent the contained entities from highlighting
			// when cursor is dragged across a workzone boundary.
			if (IsWorkZone( entity ) && !ZoneMarkers())
				continue;

			// Do an extent check... is the pick in the box?
			C2dBox extent = entity->Box(0);
			if (!extent.IsDefined())
				continue;

			if (!extent.Contains( pnt.X(), pnt.Y(), check_dist ) )
				continue;

			if (type == DBFEATURE)
				entity->Workplane( m_model->ActiveWorkplane() );

			// Assumes the entity has already been drawn
			bool allocated;
			CDisplayEntity* disp_ent = entity->DisplayEntityGet( &allocated );
			if ( allocated )
				continue;

			if (type == DBFEATURE)
				entity->Workplane( NULL );

			// Get the actual distance from the entity...
			// NOTE: Hotdots are ignored for profiles. Otherwise, you'll
			// see 4 dots at each corner because the diamonds are drawn
			// using moveto and lineto meta-commands.
			bool dot = IsHotDottable( entity );

			// NOTE: We are passing a 2D coord in, so it expands to a 3D
			// coord having an UNKNOWN Z.  On return, we need force the Z
			// to zero, so it is better behaved for this view context.
			// 
			// ACTUALLY, instead of doing the display entity distance work
			// in 2D, do it in glorious 3D so we *do* have a Z. This way,
			// when it transforms to the ref, the Z doesn't get fucked up.

			double dist = disp_ent->PntDistance(
				pnt, check_dist, &dot, &near_pnt, &end );

			// 2008.06.19 (PE) -- Optimized and modified to prevent highlight
			// of feature when drag cursor across boundary. If you want to
			// highlight the feature, then place the cursor on the target.
			//
			// Okay: four cases...
			//	1. Don't have a min dot, and this IS a dot -- take this regardless
			//	2. Don't have a min dot, and this is not dot -- take min distance
			//	3. Have a min dot, and this is not a dot -- ignore regardless
			//	4. Have a min dot, and this is a dot -- take min distance
			//
			bool okay = (!min_dot && dot);
			if ( !okay )
			{
				if (min_dot == dot)  // both true, or both false
				{
					// 2011.02.06 (PE) -- NOTE: stock curves are not selectable.
					//   if ( IsWorkZone( entity ) || IsStock( entity ) )
					// 2012.02.05 (PE) -- done differently now.
					if ( !IsSnappable( entity ) || IsWorkZone( entity ) )
						okay = (dot && (dist < max_dist));
					else
						okay = (dist < max_dist);
				}
			}

			if ( okay )
			{
				min_dot = dot | min_dot;
				max_dist = dist;

				ent_2 = ent_1;
				ent_1 = disp_ent;

				(*dbEntity) = entity;
				*io_dot = min_dot;
				*io_snap = near_pnt;
				*io_end = end;

				ret.setStatus( STATUS_OKAY );
			}
		}
	}

// Good forward
	if (ent_1 && ent_2)
	{
		if (ent_1->Intersection( ent_2, io_snap ))
		{
			// Snap in WORLD
			ent_1->Entity()->Workplane()->Transform().Transform( io_snap );

			(*dbEntity) = NULL;
			*io_dot = TRUE;
			*io_end = 0;
		}
	}

	return ret;
}


// ==================================================================
CReturn
CoglViewWire::Save( bool reset )
{
	if (m_model != NULL)
	{
		if ( reset )
			m_view_stack.Purge();

		m_view_stack.newView(
			m_canvas->ViewExtent(), m_canvas->ViewScale() );
	}

	return STATUS_OKAY;
}

// ==================================================================
CReturn	
CoglViewWire::Next( const CModel& model )
{
	if ((m_model != NULL) && m_view_stack.isNext())
	{
		CViewXform* view_xform = m_view_stack.getNext();

		CRect screen;
		CWnd* window = getCWnd();
		window->GetClientRect( &screen );

		refresh_setup( screen, view_xform->ViewExtent() );
		m_canvas->Show( BACK_BUFFER );

		return STATUS_OKAY;
	}
	return STATUS_ERROR;
}

CReturn
CoglViewWire::Prev( const CModel& model )
{
	if ((m_model != NULL) && m_view_stack.isPrev())
	{
		CViewXform* view_xform = m_view_stack.getPrev();

		CRect screen;
		CWnd* window = getCWnd();
		window->GetClientRect( &screen );

		refresh_setup( screen, view_xform->ViewExtent() );
		m_canvas->Show( BACK_BUFFER );

		return STATUS_OKAY;
	}
	return STATUS_ERROR;
}

// ==================================================================
CReturn CoglViewWire::Full( const CModel& model, bool regen )
{
	CReturn			ret;
	CDbIterator		iter;
	C3dCoord		view_pnt;
	C2dBox			extent;

	if (!m_model)
		return ret;

	iter.Init( model.Db(), DBWORKPLANE );
	while (TRUE)
	{
		CDbEntity* entity = iter();
		if (entity == NULL)
			break;
		iter.Next();

		// Profiles are dimensionless and thus make no
		// contribution to the bounding box calculation.
		if (entity->Type() == DBPROFILE)
			continue;

		if ( !entity->canDescribe2d() )
			continue;

		// Must must call CModel::is_hidden() instead of
		// CDbEntity::is_hidden() because we must consider
		// whether there is an active pattern.
		if ( m_model->is_hidden(entity) )
			continue;

		bool is_workzone = false;
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>(entity);
		if (dbFeature != NULL)
		{
			// Features are dimensionless but we do want
			// to force regeneration of workzones ....
			is_workzone = dbFeature->IsWorkZone();
			if ( !is_workzone )
				continue;
		}

		entity->Describe2d( (regen ? 1 : 0), &m_tooltip, 0. );

		bool allocated;
		CDisplayEntity* dispent = entity->DisplayEntityGet( &allocated );
		if ( allocated )
			continue;
#if 0
		if ( is_workzone )
		{
			// Force regeneration on next draw
			bool allocated;
			CDisplayEntity* dispent = entity->DisplayEntityGet( &allocated );
			dispent->Flush();

		}
#endif
		// stretch the extents... selectively
		//
		// NOTE: Faster than calculating entity bounding boxes,
		// especially were arcs are concerned.  This technique
		// is flawed, however.  To illustrate, consider a model
		// containing a single full circle whose start/end points
		// are at zero degrees.
		//
		// Of course, the benefit of our PARTICULAR environment is
		// that things are almost always wrapped in the extent,
		// and we RARELY have a single circle as the outside profile.
		//
		// TODO: A cheesy method of overcoming this problem is
		// to add quadrant points to the display list.  Whether
		// wee draw this points is yet another issue.
		//
		int cmd_num = dispent->countCommand();
		for (int cmd_idx=0; cmd_idx<cmd_num; cmd_idx++)
		{
			switch (dispent->Command( cmd_idx ))
			{
			case DCMD_HOTTEXT:
			case DCMD_TEXT:
			{
				view_pnt = dispent->WorldCoord( dispent->Param( cmd_idx ) );
				if ((view_pnt.X() >= LARGE) || (view_pnt.Y() >= LARGE))
					break;

				cmd_idx++;

				CString text;
				cmd_idx += decode_text( dispent, cmd_idx, &text );

				if (PrintText())
				{
					extent += view_pnt;
//
// Now, extract the multiple lines
//
					int line_num = 1;
					int	max_wide = 0;
					C2dCoord size;
					CString	block = text;
					while (block.GetLength()>0)
					{
						CString sub;
						sub = block.SpanExcluding( "\n" );

						int wide = sub.GetLength();
						if (wide > max_wide)
						{
							max_wide = wide;
							size = m_canvas->TextOSize( sub );
						}

						if (wide >= block.GetLength())
							block = "";
						else
						{
							block = block.Mid( wide+1 );
							line_num++;
						}
					}
					view_pnt += C3dVec( size.X(), size.Y()*line_num, 0 );
					extent += view_pnt;
				}
			}
			break;

			case DCMD_MOVETO:
			case DCMD_LINETO:
			case DCMD_ARCCWTO:
			case DCMD_ARCCCTO:
				view_pnt = dispent->WorldCoord( dispent->Param( cmd_idx ) );
				if ((view_pnt.X() >= LARGE) || (view_pnt.Y() >= LARGE))
					break;

				extent += view_pnt;
				break;
			}

		}
	}

	CRect screen;
	CWnd* window = getCWnd();
	window->GetClientRect( &screen );
	
	//---
	C2dCoord pc( extent.Xc(), extent.Yc() );

	// Determine our viewscale...
	double scale = min( (double)screen.Width() / extent.Dx(),
						(double)screen.Height() / extent.Dy() );

	double dx = (screen.Width() * 0.5) / scale;
	double dy = (screen.Height() * 0.5) / scale;

	double adjustment = max( (dx * ViewFactor()), (dy * ViewFactor()) );

	ViewPortAdjustmentsApply( adjustment, dx, dy );

	extent = AdjustedExtent( pc.X()-dx, pc.Y()-dy, pc.X()+dx, pc.Y()+dy );

	//---

	refresh_setup( screen, extent );

	return ret;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn CoglViewWire::MaterialExtents( const CModel& model, bool regen )
{
	CReturn	status;

	if (!m_model)  // why?
		return status;

	CRect screen;
	CWnd* window = getCWnd();
	window->GetClientRect( &screen );

	double dx = model.Header().getReal( "Length", 0. );
	double dy = model.Header().getReal( "Width", 0. );
	
	C2dBox extent( 0, 0, dx, dy );
	
	//---
	C2dCoord pc( extent.Xc(), extent.Yc() );

	// Determine our viewscale...
	double scale = min( (double)screen.Width() / extent.Dx(),
						(double)screen.Height() / extent.Dy() );

	dx = (screen.Width() * 0.5) / scale;
	dy = (screen.Height() * 0.5) / scale;

	double adjustment = max( (dx * ViewFactor()), (dy * ViewFactor()) );

	ViewPortAdjustmentsApply( adjustment, dx, dy );

	extent = AdjustedExtent( pc.X()-dx, pc.Y()-dy, pc.X()+dx, pc.Y()+dy );

	refresh_setup( screen, extent );

	Refresh( false );

	return status;
}

// ==================================================================
CReturn CoglViewWire::Window( 
	const C2dCoord& in_one,			// View coordinates
	const C2dCoord& in_two )
{
	if (!m_model)
		return STATUS_OKAY;

	C2dCoord tl( min(in_one.X(), in_two.X()), max(in_one.Y(), in_two.Y()) );
	C2dCoord br( max(in_one.X(), in_two.X()), min(in_one.Y(), in_two.Y()) );

	CRect screen;
	CWnd* window = getCWnd();
	window->GetClientRect( &screen );

	C2dBox extent( tl.X(),br.Y(), br.X(), tl.Y() );

	C2dCoord pc( extent.Xc(), extent.Yc() );

	// Determine our viewscale...
	double scale = min( (double)screen.Width() / extent.Dx(),
						(double)screen.Height() / extent.Dy() );

	double dx = (screen.Width() * 0.5) / scale;
	double dy = (screen.Height() * 0.5) / scale;

	double adjustment = max( (dx * ViewFactor()), (dy * ViewFactor()) );

	ViewPortAdjustmentsApply( adjustment, dx, dy );

	extent = AdjustedExtent( pc.X()-dx, pc.Y()-dy, pc.X()+dx, pc.Y()+dy );

	refresh_setup( screen, extent  );

	return STATUS_OKAY;
}

C2dBox CoglViewWire::AdjustedExtent( double xll, double yll, double xur, double yur )
{
	double xmin = min( xll, xur );  // expect xll
	double ymin = min( yll, yur );

	double xmax = max( xll, xur );  // expect xur
	double ymax = max( yll, yur );

	return C2dBox( xmin, ymin, xmax, ymax );
}


// ==================================================================

CReturn	
CoglViewWire::Pan( 
	const CModel&	model, 
	const C2dCoord& ps,			// World Coordinates
	const C2dCoord& pe )
{
	if (m_model != NULL)
	{
		C2dVec delta = ps - pe;

		m_canvas->Pan( delta );
		m_canvas->Show( BACK_BUFFER );
	}

	return STATUS_OKAY;
}

// ==================================================================

CReturn	
CoglViewWire::Zoom( 
	const CModel&	model,
	const C2dCoord	world,
	double			factor )
{
	if (m_model != NULL)
	{
		m_canvas->Scale( factor, world );
		m_canvas->Show( BACK_BUFFER );
	}

	return STATUS_OKAY;
}

// ==================================================================
CReturn	
CoglViewWire::Rotate( 
	const CModel&	model, 
	const C2dCoord& in_one,			// View coordinates
	const C2dCoord& in_two )
{
	return STATUS_OKAY;
}


// ==================================================================
//		Angle
//
//	Rotate the view by given angles... rotates in order given in
//	parameter list:
//
//		1. around Z (in xy plane)
//		2. around X (in yz plane)
//		3. around Y (in zx plane)
//		
CReturn	
CoglViewWire::Angle( 
	const CModel&	model, 
	double in_xy, 
	double in_yz, 
	double in_zx )
{
	CReturn ret;

	// Distort the meaning of this command
	if (!ZERO(in_yz))
	{ view_angle = 1; }
	else
	if (!ZERO(in_zx))
	{ view_angle = 2; }
	else
	{ view_angle = 0; }

	return ret;
}

// ==================================================================
//		Plane
//
//		Force the view to be ortho to the id'ed ref... and doesn't
//		add it new (not undoable)
//
CReturn
CoglViewWire::Plane( 
	const CModel&	model, 
	ID				plane_id )
{
	return STATUS_OKAY;
}

// ==================================================================
void		
CoglViewWire::RubberStart( 
	eRubber	in_type, 
	const C3dCoord& in_mouse,	// in Active Ref
	const CModel&	in_model )
{
	double holdX, holdY;

	if (!m_model)
		return;

	double viewscale = m_canvas->ViewScale();

	if (m_rubber_mode != RUBBER_OFF)
		RubberStop();

	C3dCoord world;
	mapRefToWorld( in_mouse, in_model.ActiveWorkplane(), &world );

//mapRefToWorld( C3dCoord(0,0,0), in_model.ActiveWorkplane(), &world );
	m_rubber_start = world;
	m_rubber_end = world;
	m_rubber_mode = in_type;

	bool do_draw = TRUE;

	if ( (m_rubber_mode == RUBBER_SELECTION)
		|| (m_rubber_mode == RUBBER_HOLD) )
	{
		m_canvas->PureTempBegin();

		CDbEntity::NewAction();

		// TODO:  encapsulate this in a method
		CSelectorStack& selstack = ((CModel&)in_model).SelectorStack();
		CSelector& selector = selstack();

		int num = selector.Count();
		for (int idx= 0; idx<num; ++idx)
		{
			CDbEntity* db_ent = selector[idx];
			if ( (db_ent->Type() <= DBARC)
				|| (db_ent->Type() == DBCOMMAND)
				|| ( (m_rubber_mode == RUBBER_HOLD)
					&& (db_ent->Type() == DBFEATURE) ) )
			{
				bool was_sel = db_ent->IsSelected();
				db_ent->SelectFlag( false );

				if (m_rubber_mode == RUBBER_HOLD)
				{
					if (db_ent->Type() == DBFEATURE)
					{
						CDisplayEntity disp_ent( db_ent );
						C3dCoord tip(0,0,0);
						((CDbFeature*) db_ent)->DescribeHold( 
								1, 
								db_ent->IntGet( "_hold", IUNDEFINED ),
								&tip,
								SUB_PIXEL / viewscale );

						draw_display( &disp_ent, false, -1, db_ent->IsSelected() );
					}

					// Put the start at the actual hold position.
					holdX = db_ent->DoubleGet( "_hold_x", m_rubber_start.X() );
					holdY = db_ent->DoubleGet( "_hold_y", m_rubber_start.Y() );
					m_rubber_start.XYZ(holdX, holdY, m_rubber_start.Z());

					do_draw = FALSE;
				}
				else
				{
					draw_entity( db_ent, false, -1 );
				}

				if (was_sel)
					db_ent->SelectFlag( true );

			}
		}

		m_canvas->PureTempEnd();
	}
	else
	{
		holdX = 0.; // debugging fodder
	}

	if (do_draw)
		rubber( m_rubber_start );  // visible 
}


// ==================================================================

void
CoglViewWire::RubberStop( void )
{
	if (!m_model)
		return;

	rubber( m_rubber_end );	// invisible
	m_rubber_mode = RUBBER_OFF;

//	if (m_drag_list) { delete m_drag_list; m_drag_list=NULL; }
}

// ==================================================================

void
CoglViewWire::rubber( const C3dCoord& end )
{
	if (m_printing)
		return;

	double viewscale = m_canvas->ViewScale();

	color( m_foreground_color );
	style( DSTYLE_SOLID );

	switch (m_rubber_mode)
	{
	case RUBBER_LINE:
		m_canvas->LineOpen();
		m_canvas->Line( m_rubber_start, end );
		break;

	case RUBBER_BOX:
		{
			m_canvas->PolylineOpen(true);
			m_canvas->Line( m_rubber_start.X(), m_rubber_start.Y(),
							end.X(), m_rubber_start.Y() );
			m_canvas->LineTo( end.X(), end.Y() );
			m_canvas->LineTo( m_rubber_start.X(), end.Y() );
		}
		break;

	case RUBBER_CIRCLE:
		{
			double tolerance = DRAG_PIXEL / viewscale;

			//
			// Okay, break it down... how many steps?
			//
			CGeoArc arc( end, end, m_rubber_start, 0 );

			circle( tolerance, arc );
		}
		break;

	case RUBBER_HOLD:
	case RUBBER_SELECTION:
		m_canvas->PatternDraw(
			C2dCoord( end.X() - m_rubber_start.X(),
						end.Y() - m_rubber_start.Y() ) );
		// TODO: Fix display behavior (?)
		BufferShow( BACK_BUFFER );
		break;
	}
}

// ==================================================================
void	
CoglViewWire::DrawCurve( const CGeoCurve* curve, int draw )
{
	C3dCoord	ps;
	C3dCoord	pe;

	m_canvas->Start();

	color( ((draw == 0) ? m_background_color : m_hot_color) );

	style( DSTYLE_SOLID );

	switch (curve->Type())
	{
	case GEOLINE:
		moveto(curve->StartPt(), GL_LINE_STRIP);
		lineto(curve->EndPt());
//			m_canvas->LineOpen();
//			m_canvas->Line(curve->StartPt(), curve->EndPt());
		break;

	case GEOARC:
		ps = curve->StartPt();
		pe = curve->EndPt();
		m_ctr = ((CGeoArc*)curve)->CenterPt();

		if ( ps.WithinTol( pe, 1.e-3 ) )
		{
			// ASSUMPTION: 360 degree arc.
			moveto(ps, GL_LINE_STRIP);
			arcto(curve->MidPt(), ((CGeoArc*)curve)->Dir());
			arcto(pe, ((CGeoArc*)curve)->Dir());
		}
		else
		{
			moveto(ps, GL_LINE_STRIP);
			arcto(pe, ((CGeoArc*)curve)->Dir());
		}
		break;
	}

	m_canvas->Close();

	// TODO: Fix display behavior (?)
	BufferShow( BACK_BUFFER );
}



// ==================================================================
void	
CoglViewWire::mapRefToWorld( 
	const C3dVec& in_ref, 
	const CDbWorkplane* in_src, 
	C3dVec* io_world ) const
{
}

// ==================================================================
void	
CoglViewWire::mapWorldToView( 
	const	C3dVec& in_world, 
	C3dVec* io_view ) const
{
}


// ==================================================================
void	
CoglViewWire::mapRefToWorld( 
	const C3dCoord& in_ref, 
	const CDbWorkplane* in_src, 
	C3dCoord* io_world ) const
{
	if ( in_src )
		in_src->Transform().TransformTo( in_ref, io_world );
}

// ==================================================================
void	
CoglViewWire::mapWorldToView( 
	const C3dCoord& in_world, 
	C3dCoord* io_view ) const
{
}

// ==================================================================
void	
CoglViewWire::mapViewToScreen( 
	const C3dCoord& in_view, 
	CPoint* io_screen ) const
{
	const C2dCoord& pc = m_canvas->ViewCenter();
	double scale = m_canvas->ViewScale();

	// 2004.09.20 (PE) -- Introduced for Transform/Bump.
	io_screen->x = (int) (m_scrnctr.x + ((in_view.X() - pc.X()) * scale));
	io_screen->y = (int) (m_scrnctr.y - ((in_view.Y() - pc.Y()) * scale));
}

// ==================================================================
void	
CoglViewWire::mapScreenToView( 
	const CPoint& in_screen, 
	C3dCoord* io_view ) const
{
}

// ==================================================================
void	
CoglViewWire::mapViewToWorld( 
	const C3dCoord& in_view, 
	C3dCoord* io_world ) const
{
}


// ==================================================================
void	
CoglViewWire::mapScreenToWorld( 
	const CPoint& in_screen, 
	C3dCoord* io_world ) const
{
	const C2dCoord& pc = m_canvas->ViewCenter();
	double scale = m_canvas->ViewScale();

	io_world->X( ((in_screen.x - m_scrnctr.x) / scale) + pc.X() );
	io_world->Y( ((m_scrnctr.y - in_screen.y) / scale) + pc.Y() );
	io_world->Z( 0.0 );
}

// ==================================================================
void	
CoglViewWire::mapWorldToScreen( 
	const C3dCoord& in_world, 
	CPoint* io_screen ) const
{
	// 2004.09.20 (PE) -- Introduced for Transform/Bump.
	double scale = m_canvas->ViewScale();
	io_screen->x = (int) (in_world.X() * scale);
	io_screen->y = (int) (in_world.Y() * scale);
}

// ==================================================================
void	
CoglViewWire::mapWorldToRef( 
	const C3dCoord&		in_world, 
	const CDbWorkplane*	in_dest,
	C3dCoord*			io_ref ) const
{
	if (in_dest)
		in_dest->Inverse().TransformTo( in_world, io_ref );
}

// ==================================================================
void
CoglViewWire::projectViewToRef(
	const C3dCoord&		in_view,
	const CDbWorkplane&	in_dest,
	C3dCoord*			io_ref ) const
{
}


// ==================================================================
//
//	Update the hot arrow marker...
//
void
CoglViewWire::hot_arrow(
	const CModel&	in_model,
	const C3dCoord& in_snap )
{
	m_hot_arrow = FALSE;

	double viewscale = m_canvas->ViewScale();

	if (m_hot_id > 0)
	{
		CDbEntity*	db_entity;
		in_model.EntityFind( m_hot_id, &db_entity );
		ASSERT( db_entity != NULL);

		CDbCurve*	db_curve = dynamic_cast<CDbCurve*>(db_entity);
		if (db_curve)
		{
			m_hot_tip = in_snap;

			switch (db_curve->Type())
			{
			case DBLINE:
				{
					m_hot_arrow = TRUE;

					CGeoLine* line = ((CDbLine*)db_curve)->Line();
					m_hot_vec = line->StartTan();
					delete line;
				}
				break;

			case DBARC:
				{
					m_hot_arrow = TRUE;

					CGeoArc* arc = ((CDbArc*)db_curve)->Arc();
					C3dCoord	ref_pt;
					mapWorldToRef( in_snap, db_curve->Workplane(), &ref_pt);
					m_hot_vec = arc->TanAtPt( ref_pt.X(), ref_pt.Y() );  // * -1;
					delete arc;
				}
				break;
			}

			C3dVec	vec( m_hot_vec.X(), m_hot_vec.Y(), 0.0 );
			C3dVec	tmp;
			mapRefToWorld( vec, db_curve->Workplane(), &tmp );
			m_hot_vec = vec;
		}
	}

	// double arrow_len = (PickTol() / viewscale) * 2;
	double arrow_len = (7 / viewscale) * 2;

	if ( m_old_arrow
		&& m_hot_arrow
		&& m_old_tip.WithinTol(m_hot_tip, SMALL) )
	{
		return;
	}

	//	color( m_hot_color );
	color( m_foreground_color );
	style( DSTYLE_SOLID );

m_old_arrow = 0;
m_old_dot = 0;
	if (m_old_arrow)
	{
		arrow( m_old_tip, m_old_vec, arrow_len );
		m_old_arrow = FALSE;
	}
	else if (m_old_dot)
	{
		draw_dot( m_old_tip );
		m_old_dot = FALSE;
	}

	if ( DEFINED(in_snap.X()) && DEFINED(in_snap.Y()) )
	{
		if (m_hot_dot)
		{
			draw_dot( in_snap );
			m_old_dot = TRUE;
			m_old_tip = in_snap;
		}
		else if (m_hot_arrow)
		{
			arrow( m_hot_tip, m_hot_vec, arrow_len );
			m_old_arrow = TRUE;
			m_old_tip = m_hot_tip;
		}
		m_old_vec = m_hot_vec;
	}
}


// ==================================================================

void
CoglViewWire::dot( const C3dCoord& world, bool hot )
{
	if (m_printing)
		return;

	C3dCoord empty;
	m_at = empty;

	if (hot)
		color( m_hot_color );
	else
		color( (Solid() ? m_sheet_color : m_background_color) );

	style( DSTYLE_SOLID );
	draw_dot(world);
}

void
CoglViewWire::draw_dot( const C3dCoord& world )
{
	// Ultimately, should find out WHY the draw_dot is causing spurious lines in GDI
	// mode... however, since GDI is, technically, only used for printing, this
	// patch should be okay for now.  2004-01-07 eww
	if (GDI() || m_printing)
		return;

	// double pix = 0.6 / viewscale;
	double pix = 0.6 / m_canvas->ViewScale();
	if (m_meta == META_ENDS)
		pix *= 0.5;

	// double dot_side = PickTol() * pix;
	double dot_side = 7 * pix;

	double dx = world.X();
	double dy = world.Y();

	m_canvas->PolylineOpen(true);

	m_canvas->Vertex( dx-dot_side, dy-dot_side );
	m_canvas->Vertex( dx+dot_side, dy-dot_side );
	m_canvas->Vertex( dx+dot_side, dy+dot_side );
	m_canvas->Vertex( dx-dot_side, dy+dot_side );

	m_canvas->Close();
}

void
CoglViewWire::TargetDraw( const C3dCoord& world )
{
	// double pix = 0.5 / viewscale;
	double pix = 0.5 / m_canvas->ViewScale();

	double d1 = 3 * pix;
	double d2 = 6 * pix;

	double xp = world.X();
	double yp = world.Y();

	m_canvas->PolylineOpen(true);

	m_canvas->Vertex( xp-d2, yp-d1 );
	m_canvas->Vertex( xp-d2, yp+d1 );
	m_canvas->Vertex( xp-d1, yp+d2 );
	m_canvas->Vertex( xp+d1, yp+d2 );

	m_canvas->Vertex( xp+d2, yp+d1 );
	m_canvas->Vertex( xp+d2, yp-d1 );
	m_canvas->Vertex( xp+d1, yp-d2 );
	m_canvas->Vertex( xp-d1, yp-d2 );

	m_canvas->Vertex( xp-d2, yp-d1 );

	m_canvas->Close();

	m_canvas->LineOpen();
	m_canvas->Line( xp-d2, yp, xp+d2, yp );
	m_canvas->Close();

	m_canvas->LineOpen();
	m_canvas->Line( xp, yp-d2, xp, yp+d2 );
	m_canvas->Close();
}

// ==================================================================

void		
CoglViewWire::arrow( 
	const C3dCoord&		in_tip, 
	const C2dUnitVec&	in_vec, 
	double				in_len )
{
	if (m_printing)
		return;

	double viewscale = m_canvas->ViewScale();

	C3dCoord empty;
	m_at = empty;


	C2dUnitVec	back1( in_vec.Radians() - (QUARTERPI*3.5) );
	C2dUnitVec	back2( in_vec.Radians() + (QUARTERPI*3.5) );

	double tiplen = max( (2./viewscale), (in_len*0.25) );
	C2dCoord	st1( in_tip + (back1 * tiplen) );
	C2dCoord	st2( in_tip + (back2 * tiplen) );

	C2dCoord	en1( in_tip + (back1 * in_len) );
	C2dCoord	en2( in_tip + (back2 * in_len) );

	m_canvas->LineOpen();

	m_canvas->Line( st1.X(), st1.Y(),
					en1.X(), en1.Y() );

	m_canvas->Line( st2.X(), st2.Y(),
					en2.X(), en2.Y() );

	m_canvas->Close();
}

// ==================================================================

void		
CoglViewWire::circle( 
	double			in_tolerance,
	const CGeoArc&	in_arc )
{
	if (m_printing)
	{ return; }

	moveto( in_arc.StartPt(), GL_LINE_STRIP );
	m_ctr = in_arc.CenterPt();

	arcto( in_arc.MidPt(), in_arc.Dir() );
	arcto( in_arc.EndPt(), in_arc.Dir() );
}

// ==================================================================
void
CoglViewWire::not_hot( void )
{
	m_high_id = 0;
	m_hot_entity = NULL;
	m_hot_id = 0;
	m_hot_dot = FALSE;
	m_hot_arrow = FALSE;
	m_old_arrow = FALSE;
	m_old_dot = FALSE;
}


// ==================================================================
int
CoglViewWire::decode_text(
	CDisplayEntity*		disp_ent,
	int					cmd_idx,
	CString*			text )
{
	return disp_ent->decode_text(cmd_idx, text);
}

// ==================================================================
//	Traverse the model to draw the entities.
//
void
CoglViewWire::draw_entities( 
	const CModel* in_model,
	double			xclip )
{
	if (!in_model)
		return;

	CDbIterator	iter;
	CEntityDb&	db = in_model->Db();

	// --------------------------------------------------------------
	// If solid view, first clear the sheet
	// TODO:  Make smarter,to work with remnants
	//
	if (Solid())
	{
		double dx, dy;
		in_model->Header().getReal( STR_LENGTH, &dx );
		in_model->Header().getReal( STR_WIDTH, &dy );

		color( m_sheet_color );

		m_canvas->FanOpen();

		lineto( C3dCoord( 00, 00, 0 ) );
		lineto( C3dCoord( dx, 00, 0 ) );
		lineto( C3dCoord( dx, dy, 0 ) );
		lineto( C3dCoord( 00, dy, 0 ) );

		m_canvas->Close();
	}

	// --------------------------------------------------------------
	//	As we traverse, we have a world-coordinate that
	//	represents the current tool position.  When we are traversing
	//	tooled entities, we draw a tool-marker on each entity that
	//	starts a new chain (profile, whatever... connected series of
	//	tooled curves).
	//
	// Restructured slightly to draw top-down because tools were being
	// displayed at nearly every geometric intersection.  In short,
	// m_tooltip was not updated in a serial manner.
	//
	// Now, by allowing the owner to do the drawing of its contents,
	// m_tooltip is updated in a serial manner.
	//
	CDbContainer*	dbContainer;

	iter.Init( db, DBWORKPLANE );
	while (true)
	{
		CDbEntity* entity = iter();
		if (entity == NULL)
			break;
		iter.Next();

		if (entity->Type() >= DBPATTERN)
		{
			// Early exit because only the active pattern
			// is drawn, and that is handled by do_refresh()
			break;
		}

		if ( !entity->canDescribe2d() )
			continue;

		// The owner determines whether its children can be drawn.
		if (entity->Owner() != NULL)
			continue;

		// NOTE: We may gain some speed by using a different
		// 'visibility' method here.  CModel::is_hidden() is
		// relatively inefficient because it traverses up
		// the entity hierarchy.  This traversal is unnecessary
		// because because we are drawing top-down.
		if ( m_model->is_hidden(entity) )
			continue;

		dbContainer = dynamic_cast<CDbContainer*>(entity);
		if (dbContainer == NULL)
		{
			draw_entity( entity, FALSE, xclip );
		}
		else
		{
			draw_entity( dbContainer, xclip );
			draw_entity( dbContainer, FALSE, xclip );
		}
	}
}

// ==================================================================
//	traverse the container to draw the entities in it
//
void
CoglViewWire::draw_entity( 
	CDbContainer*	container,
	double			xclip )
{
	// --------------------------------------------------------------
	// Container sub-traversal:
	//
	CDbContainer*	childContainer;
	CDbEntity*		entity;
	int				num, idx;

	ID old_hot_id = m_hot_id;
	ID old_high_id = m_high_id;

	bool is_hot = HighContainers() && (container->Id() == m_hot_id);
	bool is_high = HighContainers() && (container->Id() == m_high_id);

	num = container->Count();
	for (idx=0; idx<num; idx++)
	{
		entity = (*container)[idx];
		if (!entity)
			break;

		// IT #660
		if ( m_model->is_hidden(entity) )
			continue;

		if (is_hot)
			m_hot_id = entity->Id();

		if (is_high)
			m_high_id = entity->Id();

		childContainer = dynamic_cast<CDbContainer*>(entity);
		if (childContainer)
		{
			draw_entity( childContainer, xclip );
		}

		if ( entity->canDescribe2d() )
		{
			draw_entity( entity, FALSE, xclip );
		}
	}

	m_hot_id = old_hot_id;
	m_high_id = old_high_id;
}

// ==================================================================
//		actually draw the entity now.
//
void
CoglViewWire::draw_entity( 
	CDbEntity*	entity,
	bool		in_marker,
	double		txt_xclip )
{
	if (!entity)
		return;

	if (entity->DidAction())
		return;

	double viewscale = m_canvas->ViewScale();

#if 0
	// This block is no longer necessary since our use
	// of ogl has been optimized wrt display list and
	// view (zoom / pan) management.

	C2dBox entbox = entity->Box(0);
	C3x4Matrix* pxform = m_model->PatternTransform();
	if ( entbox.IsDefined() && pxform )
	{
		// First, the transform offset
		//
		C2dCoord minpt( entbox.Xmin(), entbox.Ymin() );
		pxform->Transform( &minpt );

		C2dCoord maxpt( entbox.Xmax(), entbox.Ymax() );
		pxform->Transform( &maxpt );

		const C2dBox& viewextent = m_canvas->ViewExtent();
		if ( viewextent.IsDefined() )
		{
			C2dBox testbox( minpt, maxpt );
			if ( !testbox.Intersects( viewextent, SMALL ) )
				return;
		}
	}
#endif

	// TODO: Make tolerance global??
	if (viewscale < SMALL)
		viewscale = 1.;

	double tolerance = MARK_PIXEL / viewscale;

	const int WIRE = 0;
	const int BURN = 1;
	const int NIBBLE = 2;
	int action = WIRE;

	if (Solid())
	{
		if (entity->IsToolpath())
		{
			action = (CNibbler::canNibble( *entity, false ) ? NIBBLE : BURN);
		}
	}

	if ( Nibble() && CNibbler::canNibble( *entity, false ) )
		action = NIBBLE;

	switch (action)
	{
	case WIRE:
		wire_entity(entity, in_marker, txt_xclip, tolerance);
		break;
	case BURN:
		burn_curve(dynamic_cast<CDbCurve*>(entity), tolerance);
		break;
	case NIBBLE:
		nibble_entity(entity, tolerance);
		break;
	}
}

void
CoglViewWire::InstanceDraw( CDbCommand* dbInstance )
{
	ID id = (ID) dbInstance->IntGet( "patid", 0 );

	CDbPattern* pattern = NULL;
	m_model->EntityFind( id, (CDbEntity**)&pattern, DBPATTERN, DBPATTERN );
	if (pattern)
	{
		// dbInstance->StringSet( "display", pattern->Name() );

		// Force regeneration of the handle point and pattern display lists.
		C3dCoord fodder;
		dbInstance->Describe2d( TRUE, &fodder, 1.e-4 );

		C3dVec origin = dbInstance->Coord();

		double angle = DEG2RAD * dbInstance->DoubleGet( "instang", 0.0 );

		C3x4Matrix pat_xform;
		pat_xform.setXYAngle( angle );
		pat_xform.Shift( origin );

		m_pat_id = dbInstance->Id();
		m_pat_selected = dbInstance->IsSelected();

		DrawPatternAt( pattern, &pat_xform );

		m_pat_selected = false;
		m_pat_id = -1;
	}
}

void
CoglViewWire::SemiTempDelta( const C3dCoord& delta )
{
	m_canvas->SemiTempDelta( delta );
}

// ==================================================================
void
CoglViewWire::wire_entity( CDbEntity* db_ent, bool in_marker, double txt_xclip, double tolerance )
{
	CDisplayEntity*	de = NULL;
	bool	allocated;

	if ( !Stock() )
	{
		CDbTool* dbTool = db_ent->Tool();
		if ((dbTool != NULL) && (dbTool->IsStockLayer() || dbTool->IsMachineLayer()))
			return;  // bail!
	}


	switch (db_ent->Type())
	{
	case DBPROFILE:
		{
		int regen = m_regen;
		if (!regen && Markers())
			regen = TRUE;

		db_ent->Describe2d( regen, &m_tooltip, tolerance * (MarkerScale()+1)*0.2 );
		de = db_ent->DisplayEntityGet( &allocated );
		}
		break;

	case DBCOMMAND:
	{
		//
		// Very specific command interpretation code to dereference Instances
		//
		CDbCommand* command = (CDbCommand*)db_ent;

		if ( command->IsInstance() )
			InstanceDraw( command );

		if (m_background_color == DCOLOR_WHITE)
			command->TargetColor( DCOLOR_BLACK );

		db_ent->Describe2d( m_regen, &m_tooltip, tolerance );

		if (m_background_color == DCOLOR_WHITE)
			command->TargetColor( DCOLOR_WHITE );

		de = db_ent->DisplayEntityGet( &allocated );
	}
	break;

	case DBFEATURE:
		{
		CDbFeature* dbFeature = (CDbFeature*) db_ent;
		if (dbFeature->IsWorkZone() && !ZoneMarkers() )
			db_ent = NULL;

		if (db_ent != NULL)
		{
			// 2004.12.22 (PE) -- We must set the active workplane
			// so the clamps are drawn at the correct location.
			db_ent->Workplane( m_model->ActiveWorkplane() );
			db_ent->Describe2d( m_regen, &m_tooltip, tolerance );
			db_ent->Workplane( NULL );
			de = db_ent->DisplayEntityGet( &allocated );
		}
		}
		break;

	case DBPATTERN:
		{
		C3dCoord localOrigin(0,0,0);
		const CDbWorkplane* dbWork = m_model->ActiveWorkplane();
		const C3x4Matrix& xform = dbWork->Transform();

		de = db_ent->DisplayEntityGet( &allocated );

		// Force regeneration of the display list.
		de->Flush();

		de->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_WHITE );
		de->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );

		xform.TransformTo( localOrigin, &m_tooltip );

		((CDbPattern*) db_ent)->Describe2d( m_regen, &m_tooltip, tolerance, de );
		}
		break;

	default:

		db_ent->Describe2d( m_regen, &m_tooltip, tolerance );
		de = db_ent->DisplayEntityGet( &allocated );
		break;
	}

	// NOTE: If the display list was just now allocated then
	// it must be empty and so there is nothing to draw.
	// Additionally, this *should* probably never happen.
	if ((de != NULL) && !allocated)
		draw_display( de, in_marker, txt_xclip, db_ent->IsSelected() );
}


// ==================================================================
void
CoglViewWire::burn_curve( CDbCurve* db_curve, double tolerance )
{
	if (!db_curve)
		return;

	CDbTool* tool = db_curve->Tool();
	if (tool->IsLayer())
		return;

	double viewscale = m_canvas->ViewScale();

	C3dCoordList hitlist;

	double pixel_feed = (max((tool->EffectiveDiameter()*viewscale)/8, 4) / viewscale );

	CGeoCurve* curve = db_curve->Curve();
	m_teeth.Nibble( *curve, pixel_feed, NIBBLE_FIXED, &hitlist );
	delete curve;

	int num = hitlist.Count();
	for (int idx=0; idx<num; idx++)
	{
		C3dCoord* hitpt = hitlist[idx];

		
		double tangent = hitpt->Z();
		hitpt->Z( 0.0 );
		C2dUnitVec tanvec( tangent );

		draw_entitytool( db_curve, hitpt, tanvec );
	}

	hitlist.DestructiveFlush();
}

// ==================================================================
void
CoglViewWire::nibble_entity( CDbEntity* db_ent, double tolerance )
{
	C3dCoordList hitlist;
	m_teeth.Nibble( *db_ent, false, &hitlist );

	if (db_ent->Type() == DBHOLE)
	{ db_ent->TransientOrientationSet(); }

	int num = hitlist.Count();
	for (int idx=0; idx<num; idx++)
	{
		C3dCoord* hitpt = hitlist[idx];
		double tangent = hitpt->Z();
		hitpt->Z( 0.0 );
		C2dUnitVec tanvec( tangent );

		draw_entitytool( db_ent, hitpt, tanvec );
	}

	hitlist.DestructiveFlush();
}

// ==================================================================
void
CoglViewWire::draw_entitytool( 
	CDbEntity*	db_ent, 
	C3dCoord*	hitpt, 
	C2dUnitVec&	tanvec )
{
	CDbTool* tool = db_ent->Tool();
	const CDbWorkplane*	dbWorkplane = db_ent->Workplane();
	const C3x4Matrix&	xform = dbWorkplane->Transform();

	CDisplayEntity disp_ent( db_ent );

	double viewscale = m_canvas->ViewScale();

	tool->Describe2d( 
		0, 
		xform, 
		*hitpt, 
		tanvec, 
		FALSE, 
		0, 
		SUB_PIXEL / viewscale, &disp_ent );

	bool hot = ( (disp_ent.ID() == m_hot_id)
				|| (m_pat_id == m_hot_id) );
	bool high = ( (disp_ent.ID() == m_high_id)
				|| (m_pat_id == m_high_id) );

	bool selected = db_ent->IsSelected() || m_pat_selected;

	if (selected)
	{
		color( m_hot_color );
		style( DSTYLE_SELECT );
	}
	else
	{
		int tcolor = tool->ColorGet( m_hot_color );
		if (Monochrome())
			tcolor = (Solid() ? m_background_color : m_foreground_color);
		else if (hot || high || selected)
			tcolor = m_hot_color;

		color( tcolor );
		style( DSTYLE_TOOL );
	}

	draw_display( &disp_ent, FALSE, -1, db_ent->IsSelected() );
}


// ==================================================================

void
CoglViewWire::DrawDirect( CDisplayEntity* disp_ent )
{
	bool manage_buffer = !m_canvas->IsTemporaryActive();

	if ( manage_buffer )
		m_canvas->PureTempBegin();

	draw_display( disp_ent, FALSE, UNDEFINED, FALSE );

	if ( manage_buffer )
	{
		m_canvas->PureTempEnd();
		// TODO: Fix display behavior (?)
		BufferShow( FRONT_BUFFER );
	}
}


void
CoglViewWire::draw_display(
	CDisplayEntity*	disp_ent,
	bool			in_marker,
	double			txt_xclip,
	bool			is_selected )
{
	const CDbEntity*	dbEntity;
	bool do_draw, hot, high, selected;

	hot = ( (disp_ent->Entity() != NULL) &&
			((disp_ent->ID() == m_hot_id) || (m_pat_id == m_hot_id)) );

	high = ( (disp_ent->Entity() != NULL) &&
			 ((disp_ent->ID() == m_high_id) || (m_pat_id == m_high_id)) );

	selected = is_selected || m_pat_selected;

	C3x4Matrix* pxform = m_model->PatternTransform();

	// For each command in that entity...
	//
	int cmd_num = disp_ent->countCommand();
	for (int cmd_idx=0; cmd_idx<cmd_num; cmd_idx++)
	{
		do_draw = TRUE;

		dbEntity = disp_ent->Entity();

		if ((dbEntity != NULL) && (dbEntity->Type() != DBWORKPLANE))
		{ 
			switch (m_meta)
			{
			case META_SYS:  do_draw = Markers();      break;
			case META_ZONE: do_draw = ZoneMarkers();  break;
			case META_ENDS: do_draw = EndPoints();    break;
			}
		}

		// Draw it...
		switch (disp_ent->Command( cmd_idx ))
		{
		case DCMD_COLOR:
			int do_color;
			if (Monochrome())
			{
				if (Solid())
					do_color = m_background_color;
				else
					do_color = m_foreground_color;
			}
			else
			{
				if (hot || high || selected)
					do_color = m_hot_color;
				else
				{
					do_color = disp_ent->Param( cmd_idx );
				}
			}

			if (do_color == DCOLOR_SELECT)
				do_color = m_hot_color;

			if (m_background_color != DCOLOR_BLACK)
				do_color = ColorCorrection( do_color );

			color( do_color );
			break;

		case DCMD_STYLE:
			if (selected)
				style( DSTYLE_SELECT );
			else
				style( (eDisplayStyle)disp_ent->Param( cmd_idx ) );
			break;

		case DCMD_META:
			m_meta = (eDisplayMeta) disp_ent->Param(cmd_idx);

			if ( m_canvas->Fill() )
			{
				// NOTE: At first implementation, we do not
				// support polygons having holes.

				// assert( (m_meta == NULL) );
				if (m_meta == META_NONE)
					m_canvas->Fill( false );
			}

			if (m_meta == META_FILL)
				m_canvas->Fill( true );
			break;

		case DCMD_TOOLHITCTR:
			if (Solid())
			{
				moveto( draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) ), GL_TRIANGLE_FAN );
			}
			break;

		case DCMD_MOVETO:
			if (do_draw)
			{
				if ( (m_meta == META_TOOL) && Solid() )
				{
					lineto( draw_coord( pxform, disp_ent->WorldCoord(
						disp_ent->Param( cmd_idx ) ) ) );
				}
				else
				{
					moveto( draw_coord( pxform, disp_ent->WorldCoord(
						disp_ent->Param( cmd_idx ) ) ), GL_LINE_STRIP );
				}
			}
			break;

		case DCMD_LINETO:
			if (do_draw)
			{
				lineto( draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) ) );
			}
			break;

		case DCMD_ARCCTR:
			if (do_draw)
			{
				m_ctr = draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) );
			}
			break;

		case DCMD_ARCCWTO:
			if (do_draw)
			{
				if (m_export_file != NULL)
					arcctr( m_ctr, CW );
				arcto( draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) ), CW );
			}
			break;

		case DCMD_ARCCCTO:
			if (do_draw)
			{
				if (m_export_file != NULL)
					arcctr( m_ctr, CCW );
				arcto( draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) ), CCW );
			}
			break;

		case DCMD_TEXTANG:
			text_angle( disp_ent->Float( cmd_idx ) );
			break;

		case DCMD_TEXTSIZE:
			text_size( disp_ent->Param( cmd_idx ) );
			break;

		case DCMD_TEXTPOS:
			text_pos( (eDisplayTextPos)disp_ent->Param( cmd_idx ) );
			break;

		case DCMD_HOTTEXT:
			if (do_draw)
			{
				C3dCoord wpt = draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) );
				cmd_idx++;

				CString str;
				cmd_idx += decode_text( disp_ent, cmd_idx, &str )-1;

				if ( !hot && !in_marker )
					break;

				int old_color = m_color;
				if (!hot)
					color( m_background_color );

				if ( PrintText( dbEntity )
					&& ( (txt_xclip < 0.0)
						|| (wpt.X() < txt_xclip) ) )
				{
					if (hot || high)
					{
						int a = 0;
					}
					text( wpt, str );
				}
				text_angle( 0.0 );
				text_size( 10 );
				text_pos( TEXTPOS_DEFAULT );

				color( old_color );
			}
			break;

		case DCMD_TEXT:
			if (do_draw)
			{
				C3dCoord wpt = draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) );
				cmd_idx++;

				CString str;
				cmd_idx += decode_text( disp_ent, cmd_idx, &str )-1;

				int default_text_size = 10;
				if (PrintText( dbEntity ) && ((txt_xclip < 0.) || (wpt.X() < txt_xclip)))
				{
					int size = dbEntity->IntGet( "label_size", default_text_size );
					text_size( size );
					text( wpt, str );
				}

				text_angle( 0. );
				text_size( default_text_size );
				text_pos( TEXTPOS_DEFAULT );
			}
			break;

		case DCMD_DOTMARK:
			if (do_draw)
			{
				C3dCoord world = disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) );
				draw_dot( draw_coord( pxform, world ) );
			}
			break;

		case DCMD_DOT:
			// NOTE: This segment of code is critical to displaying
			// the hot-dot when you are using the Code Viewer window.

			if (in_marker)
			{
				C3dCoord world = disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) );
				dot( world,  ((hot&&m_hot_dot) || high) );
			}
			else
			{
				C3dCoord empty;
				m_at = empty;
			}
			break;

		case DCMD_TARGET:
			// C3dCoord world = disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) );
			C3dCoord world = draw_coord( pxform, disp_ent->WorldCoord( disp_ent->Param( cmd_idx ) ) );
			TargetDraw( world );
			break;
		}
	}
}

C3dCoord
CoglViewWire::draw_coord( C3x4Matrix* xform, const C3dCoord& pt )
{
	if (!xform)
		return pt;

	C3dCoord dst;
	xform->TransformTo( pt, &dst );
	return dst;
}


// ==================================================================
void	
CoglViewWire::color( int color )
{ 
	if (m_printing)
	{
		if (color == m_white)
			m_color = m_black;	// Can't print white.
		else
			m_color = color;

		pen_select();
	}
	else
	{
		m_color = color;
		m_canvas->Color( color );
	}
}

int
CoglViewWire::ColorCorrection( int color )
{
#if 0
	switch (color)
	{
	case DCOLOR_BLACK:		return DCOLOR_WHITE;
	case DCOLOR_RED:		return 0x0000cc;
	case DCOLOR_YELLOW:		return 0x00cccc;
	case DCOLOR_GREEN:		return 0x00cc00;
	case DCOLOR_CYAN:		return 0xcccc00;
	case DCOLOR_BLUE:		return 0xcc0000;
	case DCOLOR_MAGENTA:	return 0xcc00cc;
	case DCOLOR_WHITE:		return DCOLOR_BLACK;
	default:				return 0xaaaaaa;  // some kinda grey
	}
#else
	return color;
#endif
}

// ==================================================================

void
CoglViewWire::style( eDisplayStyle style )
{
	m_style = style;

	if (m_printing)
	{
		pen_select();
	}
	else
	{
		if ((style == DSTYLE_TOOL) && !ToolDash())
			style = DSTYLE_SOLID;

		m_canvas->Style( style );
	}
}

// ==================================================================

void
CoglViewWire::pen_select( void )
{
	DWORD	pattern[4];
	int		patlen;

	LOGBRUSH brush;
	brush.lbStyle = BS_SOLID;
	brush.lbColor = m_color;

	if (m_old_pen)
		m_pdc.SelectObject( m_old_pen );
	if (m_pen) { delete m_pen; m_pen = NULL; }

	switch (m_style)
	{
		case DSTYLE_SELECT:
			pattern[0] = 10;
			pattern[1] = 12;
			patlen = 2;
			break;

		case DSTYLE_CENTER:
			pattern[0] = 20;
			pattern[1] = 15;
			patlen = 2;
			break;

		case DSTYLE_TOOL:
			if (ToolDash())
			{
				pattern[0] = 10;
				pattern[1] = 20;
				pattern[2] = 30;
				pattern[3] = 20;
				patlen = 4;
			}
			else
				patlen = 0;
			break;

		case DSTYLE_SOLID:
		default:
			patlen = 0;
			break;
	}

	m_pen = new CPen();

	if (patlen)
		m_pen->CreatePen( PS_GEOMETRIC
						  | PS_USERSTYLE,
							5,
							&brush,
							patlen,
							pattern );
	else
		m_pen->CreatePen( PS_GEOMETRIC,
							5,
							&brush,
							0,
							NULL );

	m_old_pen = m_pdc.SelectObject( m_pen );
}

// ==================================================================

void	
CoglViewWire::moveto( 
	const C3dCoord& start,
	int				mode )
{
//	if (start.WithinTol( m_at, SMALL ))
//		return;

	if (m_export_file != NULL)
	{
		fprintf( m_export_file, "0 %f, %f\n", start.X(), start.Y() );
	}
	else if ( m_printing )
	{
		moveto_print( start );
	}
	else if ( m_canvas->Fill() )
	{
		m_canvas->Vertex( start.X(), start.Y() );
	}
	else
	{
		switch (mode)
		{
		case GL_LINE_STRIP:
			m_canvas->PolylineOpen(false);
			break;
		case GL_TRIANGLE_FAN:
			m_canvas->FanOpen();
			break;
		}

		switch (view_angle)
		{
		case 0:
			m_canvas->Vertex( start.X(), start.Y() );
			break;
		case 1:
			m_canvas->Vertex( start.Y(), start.Z() );
			break;
		case 2:
			m_canvas->Vertex( start.X(), start.Z() );
			break;
		}
	}

	m_at = start;
}

void
CoglViewWire::moveto_print(
	const	C3dCoord&	start )
{
	double px, py, pz;

	gluProject( start.X(), start.Y(), start.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&px, &py, &pz );

	m_pdc.MoveTo( (int)(px*m_printscale), (int)((m_viewport[3]-py)*m_printscale)+m_printer_top );
}


// ==================================================================

void CoglViewWire::XorEnable( bool enable )
{
	if ( enable )
	{
		glEnable( GL_COLOR_LOGIC_OP );
		glLogicOp( GL_XOR );
	}
	else
	{
		glDisable( GL_COLOR_LOGIC_OP );
	}
}

void CoglViewWire::DebugLine( C3dCoord& st, C3dCoord& en )
{
	m_canvas->Start();
	style( DSTYLE_SOLID );
	color( 0xffffff );
	moveto( st, GL_LINE_STRIP );
	lineto( en );

	// TODO: Fix display behavior (?)
	BufferShow( FRONT_BUFFER );
}

void CoglViewWire::RapidLine( C3dCoord& st, C3dCoord& en, int clr )
{
	m_canvas->Start();

	m_canvas->PureTempBegin();

	if (Rapids())
	{
		style(DSTYLE_SELECT);
		color(clr);
		moveto( st, GL_LINE_STRIP );
		lineto( en );
	}
	else
	{ 
		style(DSTYLE_SOLID);
		color(m_hot_color);
		draw_dot(en);
	}

	m_canvas->PureTempEnd();

	BufferShow( FRONT_BUFFER );
}

void CoglViewWire::lineto( const C3dCoord& end )
{
	if (end.WithinTol( m_at, SMALL ))
		return;

	if (m_export_file != NULL)
	{
		fprintf( m_export_file, "1 %f, %f\n", end.X(), end.Y() );
	}
	else if ( m_printing )
	{
		lineto_print( end );
	}
	else
	{
		switch (view_angle)
		{
		case 0:
			m_canvas->Vertex( end.X(), end.Y() );
			break;
		case 1:
			m_canvas->Vertex( end.Y(), end.Z() );
			break;
		case 2:
			m_canvas->Vertex( end.X(), end.Z() );
			break;
		}
	}

	m_at = end;
}

void CoglViewWire::lineto_print( const C3dCoord& end )
{
	double px, py, pz;

	gluProject( end.X(), end.Y(), end.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&px, &py, &pz );
	m_pdc.LineTo( (int)(px*m_printscale), (int)((m_viewport[3]-py)*m_printscale)+m_printer_top );
}

// ==================================================================

void CoglViewWire::arcctr( const C3dCoord& pc, int dir )
{
	if (m_export_file != NULL)
	{
		fprintf( m_export_file, "%d %f, %f\n", ((dir > 0) ? 3 : 2), pc.X(), pc.Y() );
	}
}

void CoglViewWire::arcto( const C3dCoord& end, int dir )
{
	if (end.WithinTol( m_at, SMALL ))
		return;

	double viewscale = m_canvas->ViewScale();

	if (m_export_file != NULL)
	{
		fprintf( m_export_file, "0 %f, %f\n", end.X(), end.Y() );
	}
	else if ( m_printing )
	{
		arcto_print( end, dir );
	}
	else
	{
		double tolerance = SUB_PIXEL / viewscale;

		CGeoArc arc( m_at, end, m_ctr, dir );
		C3dCoordList* pt_list = arc.Explode( tolerance, NULL );

		if (pt_list != NULL)
		{
			int	num = pt_list->Count();
			for (int step=1; step<num; step++)
			{
				switch (view_angle)
				{
				case 0:
					m_canvas->Vertex( (*pt_list)[step]->X(), (*pt_list)[step]->Y() );
					break;
				case 1:
					m_canvas->Vertex( (*pt_list)[step]->Y(), end.Z() );
					break;
				case 2:
					m_canvas->Vertex( (*pt_list)[step]->X(), end.Z() );
					break;
				}
			}

			pt_list->DestructiveFlush();
			delete pt_list;
		}
	}

	m_at = end;
}

void CoglViewWire::arcto_print( const C3dCoord& end, int dir )
{
	double sx, sy, z;
	gluProject( m_at.X(), m_at.Y(), m_at.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&sx, &sy, &z );

	double ex, ey;
	gluProject( end.X(), end.Y(), end.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&ex, &ey, &z );

	double cx, cy;
	gluProject( m_ctr.X(), m_ctr.Y(), m_ctr.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&cx, &cy, &z );

	m_pdc.SetArcDirection( (dir==CW)?AD_CLOCKWISE:AD_COUNTERCLOCKWISE );

	double dx = ex - cx;
	double dy = ey - cy;
	double radius = sqrt( (dx*dx) + (dy*dy) );

	m_pdc.ArcTo( int((cx-radius)*m_printscale), 
				 int((m_viewport[3]-(cy-radius))*m_printscale)+m_printer_top,
				 int((cx+radius)*m_printscale), 
				 int((m_viewport[3]-(cy+radius))*m_printscale)+m_printer_top,
				 (int)(sx*m_printscale), 
				 (int)((m_viewport[3]-sy)*m_printscale)+m_printer_top,
				 (int)(ex*m_printscale), 
				 (int)((m_viewport[3]-ey)*m_printscale)+m_printer_top );

}


// ==================================================================
void	
CoglViewWire::text( 
	C3dCoord&		coord, 
	const CString&	text )
{
	if (Solid())
		return;

	if (m_printing)
	{
		text_print(coord, text);
	}
	else
	{
		if (Monochrome())
		{
			if (Solid())
				color( m_background_color );
			else
				color( m_foreground_color );
		}

		m_canvas->MoveTo( coord );
		m_canvas->Print( text, m_txtpos, m_txtang, m_txtsize );
	}

	m_at = coord;
}

// ==================================================================

void
CoglViewWire::text_print(
	C3dCoord&		coord, 
	const CString&	text )
{
	double px, py, pz;

	gluProject( coord.X(), coord.Y(), coord.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&px, &py, &pz );
	CPoint pt( (int)(px*m_printscale), (int)((m_viewport[3]-py)*m_printscale)+m_printer_top );

	//
	// Force the correct font at angle...
	//
	LOGFONT lf;
	memset( &lf, 0, sizeof(lf) );
	lstrcpy( lf.lfFaceName, m_font_name );

	lf.lfHeight = -MulDiv(m_txtsize, m_pdc.GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_BOLD;
	lf.lfEscapement = (long)(m_txtang*10);
	lf.lfOrientation = (long)(m_txtang*10);

	CFont* font = new CFont();
	font->CreateFontIndirect( &lf );

	CFont* old_font = m_pdc.SelectObject( font );

	//
	// Break the text down into individual lines
	//	TODO:  A method for this!  It's used here, and in extract_labels, and 
	//		umm, somewhere else,too.  Search for block or sub.
	//
	CStringArray	txt_array;
	CString			maxstr = "";

	CString	block = text;
	while (block.GetLength()>0)
	{
		CString sub;
		sub = block.SpanExcluding( "\n" );

		txt_array.Add( sub );

		int len = sub.GetLength();
		if (len > maxstr.GetLength())
			maxstr = sub;

		if (len >= block.GetLength())
			block = "";
		else
			block = block.Mid( len+1 );
	}
	int num = txt_array.GetSize();

	//
	// How tall is text right now?
	//
	CSize size = m_pdc.GetTextExtent( maxstr );

	C2dUnitVec right_dir( DEG2RAD*m_txtang );
	C2dVec left = right_dir * -(size.cx/2);

	C2dUnitVec down_dir( DEG2RAD*(m_txtang-90.0) );
	C2dVec down = down_dir * size.cy;
	C2dVec up = down_dir * -size.cy;

	switch (m_txtpos)
	{
		case TEXTPOS_BTMCTR:
			pt.x += (long)left.X();
			pt.y -= (long)left.Y();
			break;

		case TEXTPOS_TOPCTR:
			pt.x += (long)(up.X()*num + left.X());
			pt.y -= (long)(up.Y()*num + left.Y());
			break;

		default:
			break;	// Keep it boring
	}

	//
	// Now, print the multiple lines
	//
	m_pdc.SetTextColor( m_black );
	for (int idx=0; idx<num; idx++)
	{
		m_pdc.TextOut( pt.x, pt.y, txt_array[idx] );

		pt.x += (int)down.X();
		pt.y -= (int)down.Y();
	}

	m_pdc.SelectObject( old_font );
	if (font) delete font;
}


// ==================================================================
// view:print:
void
CoglViewWire::Print( 
	const CModel& model,
	bool	quiet,
	bool	wmf, 
	CString	file )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	if (wmf)
	{
		PrintWMF( model, file );
		return;
	}

	// ------------------------------------------
	// Get the default print info, and ask the user
	//	to confirm it (if !quiet)
	//
	CPrinterSettings printerSettings;
	printerSettings.CopyDefaultMfcPrinter();

	if (!quiet)
	{
		if (!printerSettings.PrinterSetup( NULL ))
			return;
		// Make this the MFC standard printer
		printerSettings.SetThisPrinter();
		printerSettings.Save( "printer.sav" );
	}
	else
	{
		printerSettings.Load( "printer.sav" );
		printerSettings.SetThisPrinter();
	}

	bool landscape = printerSettings.IsLandscape();

	// Get DC of actual standard printer
	AfxGetApp()->CreatePrinterDC( m_pdc );

	int x_edge = m_pdc.GetDeviceCaps( PHYSICALOFFSETX );
	int y_edge = m_pdc.GetDeviceCaps( PHYSICALOFFSETY );
	int dx = m_pdc.GetDeviceCaps( PHYSICALWIDTH );
	int dy = m_pdc.GetDeviceCaps( PHYSICALHEIGHT );

	m_printer_top = (int)(dy*(landscape?.25:.30));	// Reserve space at the top for "stuff"

	// ------------------------------------------
	//	Start the document
	//
	DOCINFO di;
	ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof(DOCINFO);

#if ORIGINA_CODE
	#if (_CCS_FAB)
		di.lpszDocName	= "CCS Fabrication";
	#else
		di.lpszDocName	= "WE-CIM Fabrication";
	#endif
#else
	di.lpszDocName	= "Fabrication";
#endif

	di.lpszOutput	= (LPSTR)NULL;
	di.lpszDatatype = (LPSTR)NULL;
	di.fwType		= 0;

	m_pdc.StartDoc( &di );
	m_pdc.StartPage( );

	// ------------------------------------------
	//	Set some flags
	//
	m_printing=true;


	// We don't currently support color or solid printing.
	bool monochrome = Monochrome();
	Monochrome( TRUE );

	bool solid=Solid();
	Solid( FALSE );

	// ------------------------------------------
	//	Render!
	//
	// Text Manipulations
	//
	CStringArray label_array;
	CString label_max = "";

	double sheet_dx = model.Header().getReal( STR_LENGTH, -1.0 );
	double xclip = (PrintVerbatim() ? -1 : sheet_dx );

	extract_labels( model, label_array, label_max, sheet_dx);

	//
	// Setup our Font
	//
	int fontsize = m_font_height * 10;	// Initial fontsize
	CFont pt_font;
	pt_font.CreatePointFont( fontsize, m_font_name, &m_pdc );
	CFont* old_font = m_pdc.SelectObject( &pt_font );

	//
	// Calculate where we print stuff...
	//
	CSize max_labelsize = m_pdc.GetTextExtent( label_max );
	m_pdc.LPtoDP( &max_labelsize );

	CRect printer;
	if (xclip < 0)
		printer = CRect( 0, m_printer_top, (dx - (int)(max_labelsize.cx/2.0)-x_edge), dy-y_edge );
	else
		printer = CRect( 0, m_printer_top, dx, dy-y_edge );

	m_printer_top -= y_edge;

	//
	// Print the titles
	//
	int title_x = x_edge;
	int title_y = y_edge;
	CString max_title = print_titles( model, title_x, title_y);

	int old_title_y = title_y;
	int old_title_x = x_edge;

	// Now calculate the remaining space with the titles in place
	title_y = y_edge;

	CSize max_titlesize = m_pdc.GetTextExtent( max_title );
	m_pdc.LPtoDP( &max_titlesize );
	title_x = max_titlesize.cx + (int)(x_edge*1.5);

	int column_space = printer.Width() - 2*x_edge - title_x;
	if (!max_labelsize.cx)
		max_labelsize.cx = column_space;

	int column_count = 1;
	int split;
	int title_down;
	while (TRUE)
	{
		// Divide the labels up into columns... but make 'em fit
		//
		column_count = column_space / max_labelsize.cx;
		if (!column_count)
		{
			column_count = 1;
			title_y = old_title_y;
			title_x = old_title_x;
		}
		
		split = (int)ceil( (float)label_array.GetSize() / (float)column_count);

		// Page space:  title_x, title_y to title_x+column_space, to screen_origin.y
		// Text block:	max_labelsize.cx, max_labelsize.cy * split
		// Does it fit?
		//
		title_down = max_labelsize.cy;
		if ( (title_y + title_down*split) > m_printer_top)
		{
			// Shrinky-dink
			m_pdc.SelectObject( old_font );

			const double FONT_SHRINK_FACTOR = 0.9;
			fontsize = (int)((double)fontsize * FONT_SHRINK_FACTOR);

			pt_font.CreatePointFont( fontsize, m_font_name, &m_pdc );
			old_font = m_pdc.SelectObject( &pt_font );

			max_labelsize = m_pdc.GetTextExtent( label_max );
			m_pdc.LPtoDP( &max_labelsize );
		}
		else
			break;
	}

	//
	// Print the labels
	//
	int title_right = column_space / column_count;
	int split_cnt = 1;
	for (int idx=0; idx<label_array.GetSize(); idx++)
	{
		m_pdc.TextOut( title_x, title_y, label_array[idx] );
		title_y += title_down;

		split_cnt++;
		if (split_cnt > split)
		{
			split_cnt = 1;
			title_y = y_edge;
			title_x += title_right;
		}
	}
	title_y += title_down;

	if (xclip > 0)
		title_y += title_down;

	//
	// Draw the part ... the LEAST of the rendering
	//
	glPushMatrix();

	m_printscale = 1.0;
	refresh_setup( printer, m_canvas->ViewExtent() );

	glPopMatrix();

	m_canvas->ModelBegin();

	draw_entities( m_model, xclip );

	m_canvas->ModelEnd();
	m_canvas->BackBuffer( true );

	glFinish();

	// ------------------------------------------
	//	Go back to normal
	//
	m_printing=false;

	if (m_old_pen)
	{
		CWnd*	window = getCWnd();
		CDC*	context = window->GetDC();

		context->SelectObject( m_old_pen );
		m_old_pen = NULL;

		window->ReleaseDC( context );
	}
	if (m_pen) { delete m_pen; m_pen = NULL; }

	m_pdc.EndPage();
	m_pdc.EndDoc();

	m_pdc.SelectObject( old_font );
	m_pdc.DeleteDC();

	Monochrome( monochrome );
	Solid( solid );
}


// -----------------------------

void
CoglViewWire::extract_labels( 
	const CModel&	model,
	CStringArray&	label_array,
	CString&		label_max,
	double			xclip )
{
	CDbIterator	iter;
	CDisplayEntity d_ent;

	double viewscale = m_canvas->ViewScale();

	iter.Init( model.Db(), DBCOMMAND );
	while (TRUE)
	{
		CDbCommand* entity = dynamic_cast<CDbCommand*>(iter());
		if (entity == NULL)
			break;
		iter.Next();

		// Must must call CModel::is_hidden() instead of
		// CDbEntity::is_hidden() because we must consider
		// whether there is an active pattern.
		if ( m_model->is_hidden(entity) )
			continue;

		d_ent.Init( entity );
		entity->Describe2d( 1, &m_tooltip, SUB_PIXEL / viewscale );

		CString text;
		int len = extract_text_xclip( text, &d_ent, xclip );

		if (len)
		{
			CString block = text;
			while (block.GetLength() > 0)
			{
				CString sub = block.SpanExcluding( "\n" );

				if (xclip > 0)
					label_array.Add( sub );

				len = sub.GetLength();
				if (len > label_max.GetLength())
					label_max = sub;

				if (len >= block.GetLength())
					block = "";
				else
					block = block.Mid( len+1 );
			}
		}
	}
}

// -----------------------------

int
CoglViewWire::extract_text_xclip(
	CString&			text,
	CDisplayEntity*		disp_ent,
	double				xclip )
{
	int num = disp_ent->countCommand();
	for (int idx=0; idx<num; idx++)
	{
		int cmd = disp_ent->Command(idx);
		if ( (cmd == DCMD_TEXT)
			|| (cmd == DCMD_HOTTEXT) )
		{
			C3dCoord pt = disp_ent->WorldCoord( disp_ent->Param( idx ) );
			idx++;

			idx += decode_text( disp_ent, idx, &text );

			if ( (xclip > 0.0)
				&& (pt.X() > (xclip-SMALL)) )
			{
				return text.GetLength();
			}
			return 0;
		}
	}
	return 0;
}

// -----------------------------

CString
CoglViewWire::print_titles( 
	const CModel&	model,
	int&			title_x, 
	int&			title_y )
{
	const CVarList& header = model.Header();

	CString text  = header.getString( "ProjNum", "<Unknown Project>" );
	CString cust  =	header.getString( "Customer", "" );
	CString due   = header.getString( "DueDate", "" );
	CString mat   = header.getString( "MatCfg", "" );
	double width  = header.getReal( STR_WIDTH, 0.0 );
	double length = header.getReal( STR_LENGTH, 0.0 );
	double thick  = header.getReal( STR_THICKNESS, 0.0 );

	CString max_title = text;
	CSize size = m_pdc.GetTextExtent( text );
	int title_down = size.cy;

	m_pdc.TextOut( title_x, title_y, text );
	title_y += title_down;

	text.Format( "%s (%4.2f x %4.2f x %4.2f)", mat, length, width, thick );
	m_pdc.TextOut( title_x, title_y, text );
	if (text.GetLength() > max_title.GetLength())
		max_title = text;
	title_y += title_down;

	text.Format( "Customer: %s    Due: %s", cust, due );
	m_pdc.TextOut( title_x, title_y, text );
	if (text.GetLength() > max_title.GetLength())
		max_title = text;
	title_y += title_down;

	CTime time = CTime::GetCurrentTime();
	text.Format( "Printed %s", time.Format( "%d %B %Y at %H:%M" ) );
	m_pdc.TextOut( title_x, title_y, text );
	if (text.GetLength() > max_title.GetLength())
		max_title = text;
	title_y += title_down;

	return max_title;
}


// ==================================================================

void	
CoglViewWire::Color( eColor idx, int rgb )
{
	switch (idx)
	{
	case COLOR_BG:
		m_background_color = rgb;
		m_canvas->ClearColor( m_background_color );
		break;
	case COLOR_FG:
		m_foreground_color = rgb;
		break;
	case COLOR_HOT:
		m_hot_color = rgb;
		break;
	case COLOR_SHEET:
		m_sheet_color = rgb;
		break;
	case COLOR_PATTERN:
		m_pattern_color = rgb;
		break;
	}
}

int		
CoglViewWire::Color( eColor idx )
{
	switch (idx)
	{
	case COLOR_BG:
		return m_background_color;
	case COLOR_FG:
		return m_foreground_color;
	case COLOR_HOT:
		return m_hot_color;
	case COLOR_SHEET:
		return m_sheet_color;
	case COLOR_PATTERN:
		return m_pattern_color;
	}

	return 0;
}


CReturn	
CoglViewWire::Annotate( CString* str )	// If NULL, reset to top line
{
	CReturn ret;

	double viewscale = m_canvas->ViewScale();

	m_canvas->ClearColor( m_background_color );

	m_canvas->Start();
	C2dCoord size = m_canvas->TextBSize("M", viewscale);

	const C2dBox& viewextent = m_canvas->ViewExtent();
	if ( !str || ((m_carat.Y() - size.Y()) < viewextent.Ymin()) )
	{
		C2dCoord size = m_canvas->TextBSize("M", viewscale);
		double dy = ((double)size.Y());

		m_carat = C2dCoord( viewextent.Xmin(), viewextent.Ymax()-2*dy );
	}

	if (str)
	{
		color(m_white);
		m_txtang = 0.0;

		CString	block = *str;
		while (block.GetLength()>0)
		{
			CString sub;
			sub = block.SpanExcluding( "\n" );

			if (sub.GetLength() > 1)
			{ size = m_canvas->TextBSize(sub, viewscale); }

			C2dVec down( 0.0, -((double)size.Y()) );
			C2dVec right( ((double)size.X()), 0.0 );

			text(C3dCoord(m_carat.X(), m_carat.Y(), 0.0), sub);
			m_carat += right;

			int len = sub.GetLength();
			if (len >= block.GetLength())
				block = "";
			else
			{
				if (block.Mid( len, 1)[0] == '\n')
				{
					m_carat += down;
					m_carat.X( viewextent.Xmin() );
				}
				block = block.Mid( len+1 );
			}
		}
	}

	// TODO: Fix display behavior (?)
	BufferShow( BACK_BUFFER );

	return ret;
}





// ==================================================================
void
CoglViewWire::PrintWMF( 
	const CModel& model,
	CString file )
{
	CglCanvas* old_canvas = m_canvas;

	bool old_gdi = GDI();
	bool old_mono = Monochrome();
	bool old_solid = Solid();

	int old_fg = m_foreground_color;
	int old_bg = m_background_color;

	CglCanvas* canvas = new CglCanvas( m_canvas->Window() );
	m_canvas = canvas;

	GDI(true);
	m_foreground_color = DCOLOR_BLACK;
	m_background_color = DCOLOR_WHITE;
	Monochrome( TRUE );
	Solid( FALSE );

//	m_printing=true;

	canvas->WMF( true );
	canvas->WMF_file( file );
	canvas->Init();

//	do_refresh();
	{
		CRect	screen;
		CWnd* window = getCWnd();
		window->GetClientRect( &screen );

		// TODO: Is this necessary?
		// 2005.07.03 (PE) -- This apparently causes the axes to drawn smaller.
		screen.SetRect( 0, 0, screen.Width()*10, screen.Height()*10 );

		refresh_setup( screen, m_canvas->ViewExtent() );

		m_canvas->ClearColor( m_background_color );

		m_canvas->ModelBegin();

		double sheet_dx = model.Header().getReal( STR_LENGTH, -1.0 );

		if (m_model->ActivePattern())
		{
			if (m_model->ActiveWorkplane())
			{ draw_entity( m_model->ActiveWorkplane(), false, sheet_dx ); }

			draw_entity( m_model->ActivePattern(), sheet_dx );
		}
		else
		{
			draw_entities( m_model, sheet_dx );
		}

		m_canvas->ModelEnd();
		m_canvas->BackBuffer( true );
	}

	m_canvas = old_canvas;
	delete canvas;

	GDI(old_gdi);
	Solid(old_solid);
	Monochrome(old_mono);

	m_foreground_color = old_fg;
	m_background_color = old_bg;

//	m_printing = false;

	return;
}

void
CoglViewWire::DrawAtColor( eDisplayColor color )
{
	m_DrawAt_color = color;
}

void
CoglViewWire::DrawAtStyle( eDisplayStyle style )
{
	m_DrawAt_style = style;
}

void
CoglViewWire::DrawPatternAt( CDbPattern* dbPattern, C3x4Matrix* xform )
{
	C3x4Matrix* old_xform = m_model->PatternTransform();
	if (old_xform)
		old_xform->Transform( xform );

	m_model->PatternTransform( xform );
	CDbPattern* old_pattern = m_model->ActivePattern( dbPattern );

	// A couple of issues here 1) we do not want to draw the handles of
	// hole-patterns (eg. LAA, BHC) when an instance is being drawn and
	// 2) we do not want to draw instance handles during code preview.
	// This latter issue is managed in XrefCreate() & XrefDestroy()
	// via calls to CDbEntity::CanDrawHandles().
	draw_entity( dbPattern, -1 );

	m_model->ActivePattern( old_pattern );
	m_model->PatternTransform( old_xform );
}

// 2008.06.15 (PE) -- At this point in history, DrawGeoAt()
// is used only by the nesting engine to render temporary
// geometry (ie. a part outline at a candidate position).
// As such, DrawGeoAt() only renders one thing at a time.
// Will we have to either extend DrawGeoAt() of introduce
// another method when we want to render multiple things,
// as we might when doing bump-nesting (ie. dragging a
// part across the screen in real-time).
//
// CRITICAL: You must establish the appropriate display list
// (by calling DisplayListBegin()) before calling DrawGeoAt()
// or its child methods. Bounding DrawGeoAt() with this call
// allows you to build the display list in batch.
//
// Example:
//    DisplayListBegin( PTEMP_LIST );
//    for (indx = 0; indx < count; ++indx)
//    {
//        DrawGeoAt( polys[indx], anchor );
//    }
//    DisplayListEnd( PTEMP_LIST );
//    BufferShow( FRONT_BUFFER );
//
void
CoglViewWire::DrawGeoAt( const CGeoElem& geo, const C3dCoord& delta )
{
	switch (geo.Type())
	{
	case GEOPOINT:
		DrawPointAt( ((const CGeoPoint&) geo).EndPt() );
		break;

	case GEOLINE:
		DrawLineAt( (const CGeoLine&) geo, delta );
		break;

	case GEOARC:
		DrawArcAt( (const CGeoArc&) geo, delta );
		break;

	case GEOPOLY:
		DrawPolyAt( (const CGeoPoly&) geo, delta );
		break;

	default:
		break;
	}
}

void
CoglViewWire::DrawPointAt( const C3dCoord& pt )
{
	CDisplayEntity	disp_ent;

	// Get the end points in the global coordinate system.
	C3dCoord p0 = pt + C3dCoord( -0.5, 0.,  0.5 );
	C3dCoord p1 = pt + C3dCoord(  0.5, 0., -0.5 );
	C3dCoord p2 = pt + C3dCoord( -0.5, 0., -0.5 );
	C3dCoord p3 = pt + C3dCoord(  0.5, 0.,  0.5 );

	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_STYLE, (DWORD) m_DrawAt_style ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_COLOR, (DWORD) m_DrawAt_color ) );

	// Point body...
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_START, (DWORD) 0 ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_MOVETO, (DWORD) disp_ent.WorldList()->Add( p0 ) ) );

	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_END, (DWORD) 0 ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_LINETO, (DWORD) disp_ent.WorldList()->Add( p1 ) ) );

	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_START, (DWORD) 0 ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_MOVETO, (DWORD) disp_ent.WorldList()->Add( p2 ) ) );

	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_END, (DWORD) 0 ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_LINETO, (DWORD) disp_ent.WorldList()->Add( p3 ) ) );
	
	DrawDirect( &disp_ent );
}


void
CoglViewWire::DrawLineAt( const CGeoLine& line, const C3dCoord& delta )
{
	CDisplayEntity	disp_ent;

	// Get the end points in the global coordinate system.
	C3dCoord st = line.StartPt() + delta;
	C3dCoord en = line.EndPt() + delta;

	eDisplayColor color = ((line.IntGet( STR_LEAD_HULL, 0 ) != 0) ? DCOLOR_YELLOW : m_DrawAt_color);

	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_STYLE, (DWORD) m_DrawAt_style ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_COLOR, (DWORD) color ) );

	// Line body...
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_START, (DWORD) 0 ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_MOVETO, (DWORD) disp_ent.WorldList()->Add( st ) ) );

	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_END, (DWORD) 0 ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_LINETO, (DWORD) disp_ent.WorldList()->Add( en ) ) );

	DrawDirect( &disp_ent );
}

// Stolen from CDbArc::Describe2d()
void
CoglViewWire::DrawArcAt( const CGeoArc& arc, const C3dCoord& delta )
{
	CDisplayEntity	disp_ent;

	// Prepare for arc tabulation.
	C3dCoord	ps = arc.StartPt() + delta;
	C3dCoord	pe = arc.EndPt() + delta;
	C3dCoord	pc = arc.CenterPt() + delta;
	C3dCoord	pm = arc.MidPt() + delta;

	eDisplayColor color = ((arc.IntGet( STR_LEAD_HULL, 0 ) != 0) ? DCOLOR_YELLOW : m_DrawAt_color);

	// Header and Dots...
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_STYLE, (DWORD) m_DrawAt_style ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_COLOR, (DWORD) color ) );

	// Arc body...
	//
	// NOTE: The arc mid-pt must be in the display list for the following reasons:
	//
	// 1. The rendering system can not properly draw 180+ degree arcs.
	//
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_START, (DWORD) 0 ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_MOVETO, (DWORD) disp_ent.WorldList()->Add( ps ) ) );
	disp_ent.CommandList()->Add(
		CDisplayEntity::Command( DCMD_ARCCTR, (DWORD) disp_ent.WorldList()->Add( pc ) ) );

	if (arc.Dir() == CW)
	{
		disp_ent.CommandList()->Add(
			CDisplayEntity::Command( DCMD_ARCCWTO, (DWORD) disp_ent.WorldList()->Add( pm ) ) );
		disp_ent.CommandList()->Add(
			CDisplayEntity::Command( DCMD_END, (DWORD) 0 ) );
		disp_ent.CommandList()->Add(
			CDisplayEntity::Command( DCMD_ARCCWTO, (DWORD) disp_ent.WorldList()->Add( pe ) ) );
	}
	else // CCW
	{
		disp_ent.CommandList()->Add(
			CDisplayEntity::Command( DCMD_ARCCCTO, (DWORD) disp_ent.WorldList()->Add( pm ) ) );
		disp_ent.CommandList()->Add(
			CDisplayEntity::Command( DCMD_END, (DWORD) 0 ) );
		disp_ent.CommandList()->Add(
			CDisplayEntity::Command( DCMD_ARCCCTO, (DWORD) disp_ent.WorldList()->Add( pe ) ) );
	}

	DrawDirect( &disp_ent );
}

void
CoglViewWire::DrawCurveAt( const CGeoCurve& curve, const C3dCoord& delta )
{
	switch (curve.Type())
	{
	case GEOLINE:
		DrawLineAt( (const CGeoLine&) curve, delta );
		break;

	case GEOARC:
		DrawArcAt( (const CGeoArc&) curve, delta );
		break;

	default:
		break;
	}
}

void
CoglViewWire::DrawPolyAt( const CGeoPoly& poly, const C3dCoord& delta )
{
	int	count, indx;

	count = poly.Count();
	for (indx = 0; indx < count; ++indx)
	{
		DrawCurveAt( poly[indx], delta );
	}
}

void
CoglViewWire::CenterPt( double* xc, double* yc )
{
	(*xc) = m_canvas->ViewCenter().X();
	(*yc) = m_canvas->ViewCenter().Y();
}

// NOTE: One of the rare occasions I've decided to use pass-by-reference.
void CoglViewWire::ViewPortAdjustmentsApply( double delta, double& dx, double& dy )
{
	if ((dx > SMALL) && (dy > SMALL))
	{
		if (dx > dy)
		{
			dx += ((dx / dy) * delta);
			dy += delta;
		}
		else
		{
			dx += delta;
			dy += ((dy / dx) * delta);
		}
	}
}

#if OPENGL_EXPERIMENT

bool
CoglViewWire::RegisterWindowClass()
{
	// NOTE: We must take care to register only once!
	if (g_hInstance == 0)
	{
		WNDCLASSEX windowClass;

		// See also MFC docs about CWinApp::m_hInstance!!!
		g_hInstance = AfxGetInstanceHandle();

		ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						// Make Sure Memory Is Cleared
		windowClass.cbSize			= sizeof (WNDCLASSEX);					// Size Of The windowClass Structure
		windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
		// windowClass.lpfnWndProc		= DefWindowProc;
		windowClass.lpfnWndProc		= (WNDPROC) oglWndProc;
		// windowClass.hInstance		= m_hInstance;				// Set The Instance
		windowClass.hInstance		= g_hInstance;
		windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			// Class Background Brush Color
		// windowClass.hCursor			= LoadCursor(IDC_ARROW);			// Load The Arrow Pointer
		windowClass.hCursor			= LoadCursor( g_hInstance, IDC_ARROW );
		windowClass.lpszClassName	= CLASS_NAME;				// Sets The Applications Classname

		if (RegisterClassEx (&windowClass) == 0)							// Did Registering The Class Fail?
		{
			// NOTE: Failure, Should Never Happen
			MessageBox(HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;													// Return False (Failure)
		}
	}

	return TRUE;														// Return True (Success)
}

LRESULT CALLBACK oglWndProc(
	HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		{
		int foo = 0;
		}
		break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

#else

#endif
