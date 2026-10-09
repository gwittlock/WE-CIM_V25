// ==================================================================
//		View Solid
//
//	Acis-solids renderer
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#include "MathConst.h"
#include "StringConst.h"

#include "dbCurve.h"
#include "dbLine.h"
#include "dbArc.h"

#include "dbTool.h"
#include "DbIterator.h"
#include "Conversion.h"
#include "Worm.h"

#include "ViewSolid.h"

// ==================================================================

CViewSolid::CViewSolid( 
	ID in_id )
	: CViewBase( in_id )
{
	m_view = NULL;
	m_context = NULL;
	m_rubber = NULL;
	m_rubber_mode = RUBBER_OFF;

	m_solid = TRUE;
}

CViewSolid::~CViewSolid()
{
	reset_context();

	if (m_context) 
	{ 
		if (m_view)
		{
			m_context->remove_view( m_view );
		}
		delete m_context; 
		m_context = NULL;
	}
	if (m_view) { delete m_view; m_view = NULL; }

	m_acis.Terminate();
}

// ==================================================================
//		Create
//
CReturn 
CViewSolid::Create( 
	HDC	in_dc )
{
	return CReturn( STATUS_ERROR );
}

CReturn 
CViewSolid::Create( 
	HWND	in_hwnd )
{
	CReturn	ret;

	m_acis.Init();

	ret = CViewBase::Create( in_hwnd );

	m_view = new view_3d_MS( in_hwnd, GetParent( in_hwnd ) );
	if (m_view)
	{
		m_view->set_edges( TRUE );
		m_view->set_polygonoffset( TRUE );
		m_view->set_perspective( FALSE );
		m_view->set_shaded( TRUE );
		m_view->set_vertex( FALSE );

		m_context = new gl_context();
		if (m_context)
		{
			m_context->add_view( m_view );
		}
		else
			ret.setStatus( STATUS_ERROR );
	}
	else
		ret.setStatus( STATUS_ERROR );

	return ret;
}

// ==================================================================
//		Clear
//	
//		Clear the screen...
//
void
CViewSolid::Clear( void )
{
}

// ==================================================================
//		Refresh
//
//	Re-draw the information, as it already sits in our lists
//
void	
CViewSolid::Refresh( void )
{
	rendering_context*	context;
	double				scale;
	double				hither, yon;
	position			target, eye;
	position			new_eye;
	double				delta;

	//
	// Viewing parameters... tidy up
	//
	{
		scale = m_view->get_image_scale();
		target = m_view->get_target();
		eye = m_view->get_eye();

		delta = eye.z();

		context = m_view->get_rendering_context();
		context->update_view_extrema( m_view );

		new_eye = m_view->get_eye();
		delta = fabs( delta - new_eye.z() );

		hither = m_view->get_hither();
		yon = m_view->get_yon();

		m_view->set_image_scale( scale );
		m_view->set_eye( eye );
		m_view->set_target( target );

		m_view->set_hither( hither );
		m_view->set_yon( yon+delta );
	}

	//
	// Match to the window, and refresh...
	//
	m_view->resize();
	m_view->refresh();
}

void	
CViewSolid::Refresh( 	
	ID		in_id,
	BOOL	in_marker )
{
}


// ==================================================================
//		Regenerate
//
//	Re-build our lists of entities, and redraw the new information
//
void	
CViewSolid::Regenerate( 
	const CModel&		model,
	BOOL				in_anim )
{
	CReturn		ret;

	CDbIterator iter( model.Db() );

	reset_context();
	C3dCoord	empty;
	m_tooltip = empty;

	//
	// Stock...
	//
	BODY*	stock;
	{
		double	width = 1;
		double	length = 1;
		double	thick = 1;
		DWORD	color = 0xff0000;

		model.Header().getReal( STR_WIDTH, &width );
		model.Header().getReal( STR_LENGTH, &length );
		model.Header().getReal( STR_THICKNESS, &thick );
		color = model.Header().getColor( color );

		stock = make_stock( length, width, thick, color );
		if (!stock)
			return;
	}

	// --------------------------------------------------------------
	//	Two-pass regenerate:
	//		1) Generate all un-tooled entities
	//		2) Step through the cut-order list and generate those features
	//
	//	As we traverse (each time), we have a world-coordinate that
	//	represents the current tool position.  When we are traversing
	//	tooled entities, we draw a tool-marker on each entity that
	//	starts a new chain (profile, whatever... connected series of
	//	tooled curves).
	//
	// First traversal:  Un-tooled entities
	//
#ifdef SOLID_LAYER_DISPLAY
	CDbEntity*			entity;
	CDisplayEntity*		disp_ent;

	iter.Init( DBLAYER );
	while (TRUE)
	{
		entity = iter();
		if (!entity)
			break;

		//
		// Determine if this entity should be hidden...
		//
		if (!is_hidden( entity ))
		{
			if ( !entity->IsToolpath())
			{
				disp_ent = new CDisplayEntity( entity );
				ret = entity->Describe3d( disp_ent->getCommandList(), disp_ent->getWorldList(), &m_tooltip );
				if (ret.getStatus() != STATUS_NONE)
					route_entity( stock, *disp_ent, in_anim );

				delete disp_ent;
			}
		}

		iter.Next();
	}
#endif

	//
	// Second traversal, cut the features in-order
	//	(unless they are tagged)
	//
	CEntityDb&	db = model.Db();
	CDbEntityList	cut_order;

	if (db.CutOrder( &cut_order, TRUE ).isOkay())
	{
		int num = cut_order.Count();

		for (int idx=0; idx<num; idx++)
		{
			CDbFeature* feature = (CDbFeature*)(cut_order[idx]);

			if (!Regenerate( model, stock, feature, in_anim ))
			{
				ret.Internal( IDS_ACIS_ABORT );
				reset_context();
				return;
			}
		}
	}

	m_context->update( stock );
}

// ==================================================================
//	Regenerate -- feature
//	Regenerate -- profile
//
//	Slave to regenerate - model (above)
//
BOOL
CViewSolid::Regenerate( 
	const CModel&	model,
	BODY*			io_stock,
	CDbFeature*		io_feature,
	BOOL			in_anim )
{
	BOOL				abort = FALSE;
	CDbEntity*			entity;
	CDbCurve*			curve;
	CDisplayEntity*		disp_ent;
	CReturn				ret;


	// --------------------------------------------------------------
	// Offset feature sub-traversal:
	//
	int num = io_feature->Count();
	for (int idx=0; idx<num; idx++)
	{
		entity = (*io_feature)[idx];
		if (!entity)
			break;

		CDbFeature* feature = dynamic_cast<CDbFeature*>(entity);
		CDbProfile* profile = dynamic_cast<CDbProfile*>(entity);
		if (feature)
		{
			// Recursive Regenerate for features... sets TAG, too
			abort = !Regenerate( model, io_stock, feature, in_anim );
		}
		else // NOT feature
		if (profile) 
		{
			abort = !Regenerate( model, io_stock, profile, in_anim );
		}

		if (abort)
			return FALSE;

		//
		// Determine if this entity should be hidden...
		//
		if (!is_hidden( entity ))
		{
			if (!feature)
			{
				// If we have a point or hole, or curves with no offset, generate them directly... 
				// otherwise, build a CDbProfile list of entities with common offsets and send it to
				// a different regenerate
				//
				BOOL	do_prof = TRUE;

				curve = dynamic_cast<CDbCurve*>(entity);
				if (!curve)
					do_prof = FALSE;

				int dir = 0;
				int partprof = entity->IntGet( STR_PARTPROF, 0 );
				if (partprof)
					dir = entity->IntGet( STR_CUTSIDE, 0 );

				if (!dir)
					do_prof = FALSE;

				if (do_prof)
				{
					CProfile	sub_prof;
					int			sub_dir;

					CDbEntity*	anchor = entity;

					// This should probably be a sub-method, but heck with it.
					//
					for (; idx<num; idx++)
					{
						entity = (*io_feature)[idx];
						if (!entity)
							break;

						curve = dynamic_cast<CDbCurve*>(entity);
						if (!curve)
							do_prof = FALSE;

						sub_dir = 0;
						int partprof = entity->IntGet( STR_PARTPROF, 0 );
						if (partprof)
							sub_dir = entity->IntGet( STR_CUTSIDE, 0 );

						if (sub_dir != dir)
							break;

						sub_prof.Append( curve->Curve() );
					}

					abort = !Regenerate( model, io_stock, anchor, &sub_prof, in_anim );
				}
				else
				{
					disp_ent = new CDisplayEntity( entity );
					ret = entity->Describe3d( disp_ent->getCommandList(), disp_ent->getWorldList(), &m_tooltip );
					if (ret.getStatus() != STATUS_NONE)
						abort = !route_entity( io_stock, *disp_ent, in_anim );

					delete disp_ent;
				}
			}
		}

		if (abort)
			return FALSE;
	}
	return TRUE;
}


BOOL
CViewSolid::Regenerate( 
	const CModel&	model,
	BODY*			io_stock,
	CDbProfile*		io_profile,
	BOOL			in_anim )
{
	CReturn				ret;
	BOOL				abort = FALSE;
	CDbEntity*			entity;

	//
	// Some supporting information for offset...
	//
	entity = (*io_profile)[0];
	if (is_hidden( entity ))
		return TRUE;

	//
	// Do the offset...
	//
	CProfile		srcProf;
	ret += CConversion::Convert( io_profile->Workplane(), io_profile, &srcProf );

	abort = TRUE;
	if (ret.isOkay())
		abort = !Regenerate( model, io_stock, entity, &srcProf, in_anim );

	return !abort;
}

BOOL
CViewSolid::Regenerate( 
	const CModel&	model,
	BODY*			io_stock,
	CDbEntity*		in_entity,
	CProfile*		in_prof,
	BOOL			in_anim )
{
	BOOL				abort = FALSE;
	CDbCurve*			curve;
	CDisplayEntity*		disp_ent;
	CReturn				ret;

	CDbWorkplane*		plane = in_entity->Workplane();
	CDbLayer*			layer = in_entity->Layer();
	CDbTool*			tool = in_entity->Tool();

	if (!tool)
		return TRUE;	// Yes it failed, but not catastrophically

//	int dir = 0;
//	int partprof = tool->IntGet( STR_PARTPROF, 0 );
//	if (partprof)
//		dir = tool->IntGet( STR_CUTSIDE, 0 );

	double dist = tool->EffectiveDiameter()/2.0;
	double sharp = PI/2;	// Round corners that are <90'

	curve = dynamic_cast<CDbCurve*>(in_entity);

	int do_prof = TRUE;
	if (!curve)
		do_prof = FALSE;

	int dir = 0;
	int partprof = in_entity->IntGet( STR_PARTPROF, 0 );
	if (partprof)
		dir = in_entity->IntGet( STR_CUTSIDE, 0 );

	dir *= plane->ToolUp();

	if (!dir)
		do_prof = FALSE;

	double zlevel;
	if (curve)
		zlevel = curve->StartPt().Z();
	else
		zlevel = 0;

	CProfile* offProf;
	if (dir != 0)
	{
		CProfileList	profList;
		ret += CWorm::ProfOffset( (CProfile&)*in_prof, dir, dist, sharp, FALSE, &profList );

		offProf = profList[0];
	}
	else
		offProf = in_prof;

	// --------------------------------------------------------------
	// Profile sub-traversal:
	//
	((CModel&)model).UndoBufferSuppress();

	int num = offProf->Count();
	for (int idx=0; idx<num; idx++)
	{
		CGeoCurve*	geo_curve = (*offProf)[idx];
		if (!geo_curve)
			break;

		// FUCKING Z VALUES
		C3dCoord& ps = (C3dCoord&) geo_curve->StartPt();
		C3dCoord& pe = (C3dCoord&) geo_curve->EndPt();

		// TODO:  THis is hozed for Ramp-In
		if (dir != 0)
		{
			ps.Z( zlevel );
			pe.Z( zlevel );
		}

		CGeoArc* geo_arc = dynamic_cast<CGeoArc*>( geo_curve );
		if (geo_arc != NULL)
		{
			C3dCoord& pc = (C3dCoord&) geo_arc->CenterPt();

			if (dir != 0)
				pc.Z( zlevel );
		}

		//
		// Go from geo_curve to database curve...
		//
		CEntityDb& db = model.Db();

		if (geo_curve->Type() == GEOLINE)
		{
			CDbLine* dbLine;

			ret += db.Create( DBLINE, (CDbEntity**)&dbLine );

			if ( ret.IsOk() )
				ret += dbLine->Init( layer, plane, (const CGeoLine&)*geo_curve );

			if ( ret.IsOk() )
			{
				curve = (CDbCurve*)dbLine;
			}
		}
		else if (geo_curve->Type() == GEOARC)
		{
			CDbArc* dbArc;
			ret += db.Create( DBARC, (CDbEntity**)&dbArc );

			if ( ret.IsOk() )
				ret += dbArc->Init( layer, plane, (const CGeoArc&)*geo_curve );

			if ( ret.IsOk() )
				curve = (CDbCurve*)dbArc;
		}
		else
			curve = NULL;
		

		//
		// Now, route the damn thing
		//
		if (curve)
		{
			curve->Tool( tool );

			disp_ent = new CDisplayEntity( curve );
			ret += curve->Describe3d( disp_ent->getCommandList(), disp_ent->getWorldList(), &m_tooltip );
			if (ret.getStatus() != STATUS_NONE)
				abort = !route_entity( io_stock, *disp_ent, in_anim );

			delete disp_ent;
			db.Delete( (CDbEntity**)&curve );
		}

		if (abort)
			return FALSE;
	}

	((CModel&)model).UndoBufferActivate();
	return TRUE;
}

// ==================================================================
void
CViewSolid::Regenerate( 
	const CModel&		model, 
	ID					in_id )
{
}


// ==================================================================
void
CViewSolid::Transform( void )
{
}

// ==================================================================
void
CViewSolid::Transform( 
	ID	in_id )
{
}

// ==================================================================
//		Move and Pick
////	Simple mouse interactions with the model
//
CReturn
CViewSolid::Move( 
	const C3dCoord& in_mouse,
	BOOL			snap,
	CCommand*		io_cmd )
{
	CReturn ret;

	//
	//	Track any rubber line stuff...
	//
	if (m_rubber_mode != RUBBER_OFF)
	{
		pick_event	pick;
		pick = pick_event( (int)in_mouse.X(), (int)in_mouse.Y(), 1, m_view );

		if (!m_rubber)
		{
			m_rubber = new rubberband_window( pick );
			rb_add_driver( m_rubber );
			m_rubber->start( pick );
		}

		m_rubber->update( pick );
	}

	return ret;
}

CReturn
CViewSolid::Pick( 
	const C3dCoord& in_mouse,
	CCommand*			io_cmd )
{
	CReturn ret;

	return ret;
}

CReturn
CViewSolid::Highlight( 
	ID		in_id )
{
	CReturn ret;

	return ret;
}

// ==================================================================
CReturn	
CViewSolid::Save( void )
{
		return CReturn( STATUS_OKAY );
}

CReturn	
CViewSolid::Next( const CModel& model )
{
		return CReturn( STATUS_OKAY );
}

CReturn
CViewSolid::Prev( const CModel& model )
{
	return CReturn( STATUS_OKAY );
}

// ==================================================================
CReturn
CViewSolid::Full( 
	const CModel& model,
	BOOL			regen )
{
	rendering_context*	context;
	double				scale;

	if (m_stock_ext.X() != UNDEFINED)
		m_view->set_distance( max( m_stock_ext.X(), m_stock_ext.Y()) * 10.0 );

	context = m_view->get_rendering_context();
	context->update_view_extrema( m_view );

	scale = m_view->get_image_scale();
	m_view->set_image_scale( scale * (1 - VIEW_FACTOR) );

	Refresh();

	return CReturn( STATUS_OKAY );
}


// ==================================================================
CReturn	
CViewSolid::Window( 
	const CModel&	model, 
	const C2dCoord& in_one,			// View coordinates
	const C2dCoord& in_two,
	BOOL			regen )
{
	pick_event	one = pick_event( (int)in_one.X(), (int)in_one.Y(), 1, m_view );
	pick_event	two = pick_event( (int)in_two.X(), (int)in_two.Y(), 1, m_view );
	
	zoom_window( one, two );

	return CReturn( STATUS_OKAY );
}



// ==================================================================
CReturn	
CViewSolid::Pan( 
	const CModel&	model, 
	const C2dCoord& in_one,			// View Coordinates
	const C2dCoord& in_two )
{
	pick_event	one = pick_event( (int)in_one.X(), (int)in_one.Y(), 1, m_view );
	pick_event	two = pick_event( (int)in_two.X(), (int)in_two.Y(), 1, m_view );

	rubberband_driver*	rubber = new rubberband_view( one, rubberband_view::PAN, TRUE );
	rb_add_driver( rubber );

	rubber->start( one );
	rubber->update( two );

	rubber->stop();
	rb_remove_driver( rubber );

	return CReturn( STATUS_OKAY );
}


// ==================================================================
CReturn	
CViewSolid::Zoom( 
	const CModel&	model, 
	const C2dCoord& in_one, 
	const C2dCoord& in_two )
{
	return CReturn( STATUS_OKAY );
}


// ==================================================================
CReturn	
CViewSolid::Rotate( 
	const CModel&	model, 
	const C2dCoord& in_one,			// View coordinates
	const C2dCoord& in_two )
{
	pick_event	one = pick_event( (int)in_one.X(), (int)in_one.Y(), 1, m_view );
	pick_event	two = pick_event( (int)in_two.X(), (int)in_two.Y(), 1, m_view );

	rubberband_driver*	rubber = new rubberband_view( one, rubberband_view::ORBIT, TRUE );
	rb_add_driver( rubber );

	rubber->start( one );
	rubber->update( two );

	rubber->stop();
	rb_remove_driver( rubber );

	return CReturn( STATUS_OKAY );
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
CViewSolid::Angle( 
	const CModel&	model, 
	double in_xy, 
	double in_yz, 
	double in_zx )
{
	return Full( model, FALSE );
}

// ==================================================================
//		Plane
//
//		Force the view to be ortho to the id'ed ref... and doesn't
//		add it new (not undoable)
//
CReturn
CViewSolid::Plane( 
	const CModel&	model, 
	ID				in_plane_id )
{
	CReturn			ret;
	CDbWorkplane*	plane = NULL;
	C3x4Matrix		xform;
	C3dVec			jvec; NULL;
	C3dVec			kvec;

	ret += model.EntityFind( in_plane_id, (CDbEntity**)&plane );
	if (!plane)
	{
		ret.Internal( IDS_PLANE_MISSING, in_plane_id );
		return ret;
	}
	if ( plane->Type() != DBWORKPLANE )
	{
		ret.Internal( IDS_SELECT_TYPE );
		return ret;
	}

	xform = plane->Transform();
	jvec = xform.getJ();
	kvec = xform.getK();
	m_view->set_direction( unit_vector( kvec.X(), kvec.Y(), kvec.Z() ),
							unit_vector( jvec.X(), jvec.Y(), jvec.Z() ) );

	return Full( model, FALSE );
}

// ==================================================================
void		
CViewSolid::RubberStart( 
	eRubber		in_type, 
	const		C3dCoord& in_mouse,
	const		CModel& model )
{
	if (m_rubber_mode != RUBBER_OFF)
		RubberStop();

	m_rubber_mode = in_type;
}

void
CViewSolid::RubberStop( void )
{
	if (m_rubber)
	{
		m_rubber->stop();
		rb_remove_driver( m_rubber );
		m_rubber = NULL;
	}

	m_rubber_mode = RUBBER_OFF;
}

// ==================================================================
void	
CViewSolid::mapRefToWorld( 
	const C3dVec& in_ref, 
	const CDbWorkplane& in_src, 
	C3dVec* io_world ) const
{
}

// ==================================================================
void	
CViewSolid::mapWorldToView( 
	const	C3dVec& in_world, 
	C3dVec* io_view ) const
{
}


// ==================================================================
void	
CViewSolid::mapRefToWorld( 
	const C3dCoord& in_ref, 
	const CDbWorkplane& in_src, 
	C3dCoord* io_world ) const
{
}

// ==================================================================
void	
CViewSolid::mapWorldToView( 
	const	C3dCoord& in_world, 
	C3dCoord* io_view ) const
{
}

// ==================================================================
void	
CViewSolid::mapViewToScreen( 
	const C3dCoord& in_view, 
	CPoint* io_screen ) const
{
	io_screen->x = (int)in_view.X();
	io_screen->y = (int)in_view.Y();
}

// ==================================================================
void	
CViewSolid::mapScreenToView( 
	const CPoint& in_screen, 
	C3dCoord* io_view ) const
{
	io_view->X( in_screen.x );
	io_view->Y( in_screen.y );
	io_view->Z( 0.0 );
}

// ==================================================================
void	
CViewSolid::mapViewToWorld( 
	const C3dCoord& in_view, 
	C3dCoord* io_world ) const
{
}


// ==================================================================
void	
CViewSolid::mapScreenToWorld( 
	const CPoint& in_screen, 
	C3dCoord* io_world ) const
{
}

// ==================================================================
void	
CViewSolid::mapWorldToScreen( 
	const C3dCoord& in_world, 
	CPoint* io_screen ) const
{
}

// ==================================================================
void	
CViewSolid::mapWorldToRef( 
	const C3dCoord&		in_world, 
	const CDbWorkplane&	in_dest,
	C3dCoord*				io_ref ) const
{
}

// ==================================================================
void
CViewSolid::projectViewToRef(
	const C3dCoord&		in_view,
	const CDbWorkplane&	in_dest,
	C3dCoord*				io_ref ) const
{
}


// ==================================================================
void
CViewSolid::Print(
	const CModel&		model )
{
}


// ==================================================================================
//		makeStock
//
//		Make the "stock" that later operations occur upon.  Only one stock
//		is allowed at a given time.
//
//		Note that we keep a generic, content-less CLineElement to hole
//		the attributes, and generate a Cuboid for display.
//
BODY*
CViewSolid::make_stock( 
	double	in_width, 
	double	in_height, 
	double	in_depth,
	DWORD	in_color )
{
	transf			xform;
	vector			delta;
	BODY*			stock_ent;

	m_stock_ext = C3dCoord( in_width, in_height, in_depth );

	// Stock is a cuboid
	CAcis::CheckOutcome( api_make_cuboid( in_width, in_height, in_depth, stock_ent ) );
	if (!stock_ent)
		return NULL;

	// Transform the "cuboid" to the origin; zero at bottom
	delta = vector( in_width/2.0, in_height/2.0, in_depth/2.0 );
	xform = translate_transf( delta );
	CAcis::CheckOutcome( api_apply_transf( stock_ent, xform ) );

	// Add it to the display, and repaint
	if (in_color > 0)
		set_acis_color( stock_ent, in_color );
	else
		CAcis::CheckOutcome( api_gi_set_entity_color( stock_ent, COLOR_STOCK ) );

	m_context->add( stock_ent, FALSE );
	m_context->update( stock_ent );

	return stock_ent;
}

// ==================================================================================
void
CViewSolid::set_acis_color( 
	ENTITY*	in_ent,
	DWORD	in_color )
{
	double	red;
	double	green;
	double	blue;

	red = (in_color & 0x00ff0000) >> 16;
	green = (in_color & 0x0000ff00) >> 8;
	blue = (in_color & 0x000000ff);

	rgb_color rgb( red/255.0, green/255.0, blue/255.0 );
	CAcis::CheckOutcome( api_gi_set_entity_rgb( in_ent, rgb ) );
}




// ==================================================================================
void
CViewSolid::reset_context( void )
{
	ENTITY_LIST ent_list;
	ENTITY*		ent;

	if (m_context) 
	{
		m_context->get_entities( ent_list );
		ent_list.init();
		while (ent = ent_list.next())
		{
			m_context->remove( ent );
			CAcis::CheckOutcome( api_del_entity( ent ) );
		}
		ent_list.clear();
	}
}


// ==================================================================================
//		route_entity
//
//	Apply the commands in the display entity to the stock... occurs only
//	once for each entity, during Regenerate.  Acis handles future Refresh
//	and other calls...
//
BOOL
CViewSolid::route_entity( 
	BODY*					io_stock,
	const CDisplayEntity&	in_entity,
	BOOL					in_anim )
{
	BOOL		abort = FALSE;
	DWORD		color = 0;
	C3dCoord	at = C3dCoord( 0, 0, 0 );
	C3dCoord	to = C3dCoord( 0, 0, 0 );
	C3dCoord	ctr = C3dCoord( 0, 0, 0 );
	BODY*		body = NULL;
	WCS*		ref;
	BOOL		pt_anim = in_anim;
	CEdgeList	edge_list;

	int num = in_entity.countCommand();
	for (int idx=0; idx<num; idx++)
	{
		switch (in_entity.getCommand( idx ))
		{
			case DCMD_COLOR:
				color = in_entity.getParam( idx );
				break;

			case DCMD_MOVETO:
				at = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				break;

			case DCMD_OPENBODY:
				if (body)
					CAcis::CheckOutcome( api_del_entity( body ) );
				CAcis::CheckOutcome( api_body( body ) );
				break;

			case DCMD_CLOSEBODY:
				set_acis_color( body, color );
				break;

			case DCMD_UPVEC:
				{
					C3dCoord	pt = in_entity.getWorldCoord( in_entity.getParam( idx ) );
					C3dVec		up = C3dVec( pt.X(), pt.Y(), pt.Z() );
					C3dVec		right;
					C3dVec		left;

					left = up ^ C3dVec( 1, 0, 0 );
					if (left.Length() < SMALL*2 )
					{
						right = up ^ C3dVec( 0, 0, 1 );
						left = up ^ right;
					}
					else
					{
						right = up ^ left;
						left = up ^ right;
					}

					position	origin( 0, 0, 0 );
					position	x_axis( right.X(), right.Y(), right.Z() );
					position	y_axis( left.X(), left.Y(), left.Z() );
					CAcis::CheckOutcome( wcs_create( origin, x_axis, y_axis, ref ) );
				}
				break;

			case DCMD_UNION:
				break;

			case DCMD_SPHERE:
				tool_sphere( body, at, in_entity.getWorldCoord( in_entity.getParam( idx ) ) );
				break;

			case DCMD_CYLINDER:
				tool_cylinder( body, at, in_entity.getWorldCoord( in_entity.getParam( idx ) ) );
				break;

			case DCMD_CONE:
				tool_cone( body, at, in_entity.getWorldCoord( in_entity.getParam( idx ) ) );
				break;

			case DCMD_TORUS:
				tool_torus( body, at, in_entity.getWorldCoord( in_entity.getParam( idx ) ) );
				break;

			case DCMD_OPENEDGE:
				// MUST call OPENBODY first...
				// edge_list is already cleared, by nature
				break;

			case DCMD_CLOSEEDGE:
			{
				edge_extrude( edge_list, body );

				for (int idx=0; idx<edge_list.GetSize(); idx++)
				{
					CAcis::CheckOutcome( api_del_entity( edge_list[idx]) );
				}
				// MUST call CLOSEBODY after
			}
			break;

			case DCMD_EDGEAT:
				at = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				break;

			case DCMD_EDGECTR:
				ctr = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				break;

			case DCMD_EDGETO:
				to = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				edge_line( edge_list, at, to );
				at = to;
				break;

			case DCMD_EDGECCTO:
				to = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				if (!at.WithinTol( ctr, SMALL ))
					edge_arc( edge_list, at, to, ctr, FALSE );
				at = to;
				break;

			case DCMD_EDGECWTO:
				to = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				if (!at.WithinTol( ctr, SMALL ))
					edge_arc( edge_list, at, to, ctr, TRUE );
				at = to;
				break;

			case DCMD_ROUTEAT:
				to = in_entity.getWorldCoord( in_entity.getParam( idx ) );

				if ( //pt_anim
					//|| (
					(
#define TOO_NEAR_THE_EDGE 1e-4
						!CLOSE( to.X(), 0.0, TOO_NEAR_THE_EDGE )
						&& !CLOSE( to.X(), m_stock_ext.X(), TOO_NEAR_THE_EDGE )
						&& !CLOSE( to.Y(), 0.0, TOO_NEAR_THE_EDGE )
						&& !CLOSE( to.Y(), m_stock_ext.Y(), TOO_NEAR_THE_EDGE )
						)
					)
				{
					abort = !route_hole( *ref, io_stock, *body, to, pt_anim );
				}
				at = to;
				if (pt_anim && !abort)
					m_context->update( io_stock );
				break;

			case DCMD_ROUTETO:
				to = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				abort = !route_line( *ref, io_stock, *body, at, to );
				at = to;
				if (in_anim && !abort)
					m_context->update( io_stock );
				pt_anim = FALSE;
				break;

			case DCMD_ROUTECTR:
				ctr = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				break;

			case DCMD_ROUTECCTO:
				to = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				abort = !route_arc( *ref, io_stock, *body, at, to, ctr, FALSE );
				at = to;
				if (in_anim && !abort)
					m_context->update( io_stock );
				pt_anim = FALSE;
				break;

			case DCMD_ROUTECWTO:
				to = in_entity.getWorldCoord( in_entity.getParam( idx ) );
				abort = !route_arc( *ref, io_stock, *body, at, to, ctr, TRUE );
				at = to;
				if (in_anim && !abort)
					m_context->update( io_stock );
				pt_anim = FALSE;
				break;

			default:
				break;
		}

		if (abort)
			return FALSE;
	}

	if (body)
		CAcis::CheckOutcome( api_del_entity( body ) );

	return TRUE;
}


// ==================================================================================
//		tool_sphere
//
//	Create a sphere, as part of a tool body, and position it as specified.
//
void
CViewSolid::tool_sphere( 
	BODY*			io_body,		// Body to add sphere to...
	const C3dCoord&	in_at,			// Center position, in world (model)
	const C3dCoord&	in_size )		// X() holds radius
{
	BODY*	sphere = NULL;
	
	// Sphere!
	CAcis::CheckOutcome( api_make_sphere( in_size.X(), sphere ) );
	if (!sphere)
		return;

	// Move to correct center position...
	vector delta( in_at.X(), in_at.Y(), in_at.Z() );
	transf	xform = translate_transf( delta );
	CAcis::CheckOutcome( api_apply_transf( sphere, xform ) );

	// Merge with the incoming body...
	CAcis::CheckOutcome( api_boolean( sphere, io_body, UNION ) );
}

// ==================================================================================
//		tool_cylinder
//
//	Create a cylinder, as part of a tool body, and position it as specified.
//
void
CViewSolid::tool_cylinder(
	BODY*			io_body,		// Body to add sphere to...
	const C3dCoord&	in_at,			// Center position, in world (model)
	const C3dCoord&	in_size )		// X() holds radius, Y() length
{
	BODY*	cyl = NULL;
	
	// Cylinder!
	CAcis::CheckOutcome( api_make_frustum( in_size.Y(), in_size.X(), in_size.X(), in_size.X(), cyl ) );
	if (!cyl)
		return;

	// Move to correct center position...
	vector delta( in_at.X(), in_at.Y(), in_at.Z() );
	transf	xform = translate_transf( delta );
	CAcis::CheckOutcome( api_apply_transf( cyl, xform ) );

	// Merge with the incoming body...
	CAcis::CheckOutcome( api_boolean( cyl, io_body, UNION ) );
}

// ==================================================================================
//		tool_cone
//
//	Create a cone, as part of a tool body, and position it as specified.
//
void
CViewSolid::tool_cone(
	BODY*			io_body,		// Body to add sphere to...
	const C3dCoord&	in_at,			// Center position, in world (model)
	const C3dCoord&	in_size )		// X() holds radius, Y() length
{
	BODY*	cone = NULL;
	
	// Cone!
	CAcis::CheckOutcome( api_make_frustum( in_size.Y(), in_size.X(), in_size.X(), 0.0, cone ) );
	if (!cone)
		return;

	// Fucking acis makes the god-damn piece of shit CONE UPSIDE FUCKING DOWN
	// Reflect it
	transf	xform = reflect_transf( vector( 0, 0, 1 ) );
	CAcis::CheckOutcome( api_apply_transf( cone, xform ) );

	// Move to correct center position...
	vector delta( in_at.X(), in_at.Y(), in_at.Z() );
	xform = translate_transf( delta );
	CAcis::CheckOutcome( api_apply_transf( cone, xform ) );

	// Merge with the incoming body...
	CAcis::CheckOutcome( api_boolean( cone, io_body, UNION ) );
}

// ==================================================================================
//		tool_torus
//
//	Create a toroid, as part of a tool body, and position it as specified.
//
void
CViewSolid::tool_torus(
	BODY*			io_body,		// Body to add sphere to...
	const C3dCoord&	in_at,			// Center position, in world (model)
	const C3dCoord&	in_size )		// X() holds outside radius, Y() corner radius
{
	BODY*	donut = NULL;
	
	// Cone!
	CAcis::CheckOutcome( api_make_torus( in_size.X()-in_size.Y(), in_size.Y(), donut ) );
	if (!donut)
		return;

	// Move to correct center position...
	vector delta( in_at.X(), in_at.Y(), in_at.Z() );
	transf	xform = translate_transf( delta );
	CAcis::CheckOutcome( api_apply_transf( donut, xform ) );

	// Merge with the incoming body...
	CAcis::CheckOutcome( api_boolean( donut, io_body, UNION ) );
}


// ==================================================================================
BOOL
CViewSolid::route_hole( 
	const WCS&		in_ref,			// Defines "up" vector
	BODY*			io_stock,		// Stock to subtract from
	const BODY&		in_body,		// Body of object to subtract
	const C3dCoord&	in_tip,			// Position in world...
	BOOL			in_anim )
{
	BODY*	copy = NULL;

	CAcis::CheckOutcome( api_copy_body( (BODY*)&in_body, copy ) );
	if (!copy)
		return TRUE;	// Yes it failed, but not catastrophically

	// Rotate to correct "up"
	CAcis::CheckOutcome( api_transform_entity( copy, in_ref.to_model() ) );

	//Translate to the cut position
	vector delta( in_tip.X(), in_tip.Y(), in_tip.Z() );
	transf	xform = translate_transf( delta );
	CAcis::CheckOutcome( api_apply_transf( copy, xform ) );

	if (in_anim)
		m_context->add( copy, TRUE );

	if (!CAcis::CheckOutcome( api_boolean( copy, io_stock, SUBTRACTION ) ))
	{
		CAcis::CheckOutcome( api_del_entity( copy ) );
		return FALSE;
	}
	return TRUE;
}


// ==================================================================================
BOOL
CViewSolid::route_line( 
	const WCS&		in_ref,			// Defines "up" vector
	BODY*			io_stock,		// Stock to subtract from
	const BODY&		in_body,		// Body of object to subtract
	const C3dCoord&	in_st,			// Start pos in world
	const C3dCoord&	in_en )			// End pos in world
{
	// 1. Create an *outline* of the tool body
	BODY*	outline = cast_shadow( in_body );
	if (!outline)
		return TRUE;	// Yes it failed, but not catastrophically

	// 2. Create a path to follow
	EDGE*	path = NULL;

	position	start( in_st.X(), in_st.Y(), in_st.Z() );
	position	end( in_en.X(), in_en.Y(), in_en.Z() );

	CAcis::CheckOutcome( api_mk_ed_line( start, end, path ) );
	if (!path)
		return TRUE;	// Yes it failed, but not catastrophically

	// 3. Sweep and cut the outline down the path
	BOOL okay = route_curve( in_ref, io_stock, *outline, *path );

	// 4. Cleanup
	CAcis::CheckOutcome( api_del_entity( outline ) );
	CAcis::CheckOutcome( api_del_entity( path ) );

	return okay;
}


// ==================================================================================
BOOL
CViewSolid::route_arc( 
	const WCS&		in_ref,			// Defines "up" vector
	BODY*			io_stock,		// Stock to subtract from
	const BODY&		in_body,		// Body of object to subtract
	const C3dCoord&	in_st,			// Start pos in world
	const C3dCoord&	in_en,			// End pos in world
	const C3dCoord&	in_ct,			// Center pos in world
	BOOL			in_cw )			// TRUE if clockwise, else cc
{
	// 1. Create an *outline* of the tool body
	BODY*	outline = cast_shadow( in_body );
	if (!outline)
		return TRUE;	// Yes it failed, but not catastrophically

	// 2. Create a path to follow
	position	start( in_st.X(), in_st.Y(), in_st.Z() );
	position	end( in_en.X(), in_en.Y(), in_en.Z() );
	position	ctr( in_ct.X(), in_ct.Y(), in_ct.Z() );

	// The damn arc is given in world...  move it back to local
	transf		to_ref = in_ref.to_wcs();
	start *= to_ref;
	end *= to_ref;
	ctr *= to_ref;

	// There are quite a few things to determine now, about the arc
	double	delta_x = end.x() - ctr.x();
	double	delta_y = end.y() - ctr.y();
	double	en_ang = atan2( delta_y, delta_x );

	delta_x = start.x() - ctr.x();
	delta_y = start.y() - ctr.y();
	double st_ang = atan2( delta_y, delta_x );

	double	radius = sqrt( (delta_x*delta_x) + (delta_y*delta_y) );

	if (in_cw)
	{
		double temp = st_ang;
		st_ang = en_ang;
		en_ang = temp;
	}

	unit_vector	normal( 0, 0, 1 );
	vector		axis( radius, 0, 0 );

	EDGE*	path = NULL;
	CAcis::CheckOutcome( api_mk_ed_ellipse( ctr, normal, axis, 1.0, st_ang, en_ang, path ) );
	if (!path)
		return TRUE;	// Yes it failed, but not catastrophically

	// Move the resulting arc back to world...
	CAcis::CheckOutcome( api_transform_entity( path, in_ref.to_model() ) );

	// 3. Sweep and cut the outline down the path
	BOOL okay = route_curve( in_ref, io_stock, *outline, *path );

	// 4. Cleanup
	CAcis::CheckOutcome( api_del_entity( outline ) );
	CAcis::CheckOutcome( api_del_entity( path ) );

	return okay;
}



// ==================================================================================
//		cast_shadow
//
//	Given a tool body (tip on origin, up in world Z)... get a
//	2D cross-section
//
BODY*
CViewSolid::cast_shadow(
	const BODY&	in_body )
{
	//
	// Get the body extents... the hard way?
	//
	ENTITY_LIST	one_list;
	position	tl;
	position	br;

	one_list.add( (ENTITY*)(BODY*)&in_body );
	CAcis::CheckOutcome( api_get_entity_box( one_list, NULL, tl, br ) );
	one_list.clear();

	position	mid( (tl.x() + br.x()) / 2,
					(tl.y() + br.y()) / 2,
					(tl.z() + br.z()) / 2 );

	//
	// Create a cutting face/body, which will act as a cutting plane to get
	//	the body's shadow
	//
	position	origin( tl.x(), mid.y(), br.z() );
	position	x_pos( tl.x(), mid.y(), tl.z() );
	position	y_pos( br.x(), mid.y(), br.z() );
						
	FACE*		cut_plane = NULL;
	BODY*		cut_body = NULL;

	CAcis::CheckOutcome( api_make_plface( origin, x_pos, y_pos, cut_plane ) );
	if (!cut_plane)
		return NULL;

	CAcis::CheckOutcome( api_mk_by_faces( NULL, 1, &cut_plane, cut_body ) );
	if (!cut_body)
		return NULL;

	CAcis::CheckOutcome( api_body_to_2d( cut_body ) );

	// 
	// Slice the incoming body by the cutting body
	//
	unit_vector	normal( 0, 1, 0 );
	BODY*		shadow;

	CAcis::CheckOutcome( api_slice( cut_body, (BODY*)&in_body, normal, shadow ) );

	//
	// Cleanup...
	//
	CAcis::CheckOutcome( api_del_entity( cut_body ) );
	CAcis::CheckOutcome( api_clean_wire( shadow ) );

	return shadow;
}



// ==================================================================================
BOOL
CViewSolid::route_curve( 
	const WCS&		in_ref,			// Defines "up" vector
	BODY*			io_stock,		// Stock to subtract from
	const BODY&		in_outline,		// Outline of shape to cut (on origin, up in world Z)
	const ENTITY&	in_path )		// Path to drag outline along (in world)
{
	//
	// ... copy the originals, to mess with
	//
	BODY*		outline_copy = NULL;
	ENTITY*		path_copy = NULL;

	CAcis::CheckOutcome( api_copy_body( (BODY*)&in_outline, outline_copy ) );
	if (!outline_copy)
		return TRUE;	// Yes it failed, but not catastrophically

	CAcis::CheckOutcome( api_copy_entity( (ENTITY*)&in_path, path_copy ) );
	if (!path_copy)
		return TRUE;	// Yes it failed, but not catastrophically

	// Rotate to correct "up"
	CAcis::CheckOutcome( api_transform_entity( outline_copy, in_ref.to_model() ) );

	//
	//	Get varied information
	//
	parameter		st_param = ((EDGE*)path_copy)->start_param();
	CURVE*			curve_geom = ((EDGE*)path_copy)->geometry();
	curve const&	curve_math = curve_geom->equation();

	unit_vector		path_tan = curve_math.eval_direction( st_param );

	//
	// Take the outline at the origin, and whip it around to face
	//
	vector tool_normal = in_ref.y_axis();
	double tool_angle;
	double delta_angle;
	if ( (fabs(tool_normal.x()) < VECTOR_SMALL)
		&& (fabs(tool_normal.y()) < VECTOR_SMALL) )
	{
		tool_angle = atan2( tool_normal.z(), tool_normal.x() );
		delta_angle = atan2( path_tan.z(), path_tan.x() );
	}
	else
	{
		tool_angle = atan2( tool_normal.y(), tool_normal.x() );
		delta_angle = atan2( path_tan.y(), path_tan.x() );
	}

	delta_angle = delta_angle - tool_angle;

	vector			tool_axis = in_ref.z_axis();
	transf			xform = rotate_transf( delta_angle, tool_axis );

	CAcis::CheckOutcome( api_apply_transf( outline_copy, xform ) );

//m_context->add( outline_copy, FALSE );
//return;

	//
	// Now drag that outline to the start of the curve
	//
	position		st_pos = curve_math.eval_position( st_param );

	vector			delta( st_pos.x(), st_pos.y(), st_pos.z() );
	xform = translate_transf( delta );
	CAcis::CheckOutcome( api_apply_transf( outline_copy, xform ) );

	//
	// Sweep!  This if where things fail...
	//
	sweep_options	sweep_opt;
	
	sweep_opt.rigid		= 0;		// Required 0 for Arcs; 1 if delta-Z sweep?
	CAcis::CheckOutcome( api_sweep_with_options( 
									outline_copy,
									(EDGE*)path_copy,
									&sweep_opt,
									outline_copy ) );

	//
	// Route!
	//
	if (!CAcis::CheckOutcome( api_boolean( outline_copy, io_stock, SUBTRACTION ) ))
	{
		CAcis::CheckOutcome( api_del_entity( outline_copy ) );
		return FALSE;
	}
	return TRUE;
}



	

BOOL
CViewSolid::is_hidden( const CDbEntity*	entity )
{
	BOOL hide = FALSE;

	if (entity->IsDeleted()
		|| entity->IsHidden()
		|| !entity->canDescribe3d())
		hide = TRUE;
	if (!hide
		&& entity->Owner())
		entity->Owner()->IntGet( "hide", &hide );
	if (!hide)
		entity->IntGet( "hide", &hide );
	if (!hide
		&& entity->Layer())
		entity->Layer()->IntGet( "hide", &hide );

	return hide;
}


void
CViewSolid::edge_line( 
	CEdgeList&	edge_list, 
	const C3dCoord&	at, 
	const C3dCoord&	to )
{
	position	stpos( at.X(), at.Y(), 0.0 );
	position	enpos( to.X(), to.Y(), 0.0 );

	EDGE*	edge;

	CAcis::CheckOutcome( api_curve_line( stpos, enpos, edge ) );

	edge_list.Add( edge );
}


void
CViewSolid::edge_arc( 
	CEdgeList&	edge_list, 
	const C3dCoord&	at, 
	const C3dCoord&	to, 
	const C3dCoord&	ctr,
	BOOL		ccw )
{
	position	stpos( at.X(), at.Y(), 0.0 );
	position	enpos( to.X(), to.Y(), 0.0 );
	position	ctpos( ctr.X(), ctr.Y(), 0.0 );

	EDGE*	edge;

	if (ccw)
	{
		CAcis::CheckOutcome( api_curve_arc_center_edge( ctpos, enpos, stpos, NULL, edge ) );
	}
	else // cc
	{
		CAcis::CheckOutcome( api_curve_arc_center_edge( ctpos, stpos, enpos, NULL, edge ) );
	}

	edge_list.Add( edge );
}


void
CViewSolid::edge_extrude(
	const CEdgeList&	edge_list,
	BODY*&				body )
{
	//
	// Convert our edge list to a useable wire
	//

	int num = edge_list.GetSize();
	EDGE**	edge_array = NULL;
	edge_array = new EDGE*[num];

	for (int idx=0; idx<num; idx++)
		edge_array[idx] = edge_list[idx];

	CAcis::CheckOutcome( api_make_ewire( num, edge_array, body ) );

	//
	// Create a path to follow
	//
	EDGE*	path = NULL;

	#define PUNCH_DEPTH 50
	position	start( 0, 0, 0 );
	position	end( 0, 0, PUNCH_DEPTH );

	CAcis::CheckOutcome( api_mk_ed_line( start, end, path ) );
	if (!path)
	{
		if (edge_array) delete[] edge_array;
		return;	// Yes it failed, but not catastrophically
	}

	//
	// Sweep the wire down the path; a partial clone out of route_curve()
	//
	parameter		st_param = ((EDGE*)path)->start_param();
	CURVE*			curve_geom = ((EDGE*)path)->geometry();
	curve const&	curve_math = curve_geom->equation();

	unit_vector		path_tan = curve_math.eval_direction( st_param );

	sweep_options	sweep_opt;
	sweep_opt.rigid = 1;
	CAcis::CheckOutcome( api_sweep_with_options(
							body,
							(EDGE*)path,
							&sweep_opt,
							body ) );
	//
	// Cleanup
	//
	// Path absorbed by the sweep?

	if (edge_array) delete[] edge_array; edge_array = NULL;

	return;
}


