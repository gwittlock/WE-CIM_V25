// ==================================================================
//	Support class for NestingPart -- holds the various levels of
//	GeoPoly outlines from the tool hits
// ==================================================================

#include "stdafx.h"

#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbLine.h"
#include "DbArc.h"
#include "StringConst.h"

#if (_CI || _NST)
#include "Portal.h"
#include "CiLine.h"
#include "CiArc.h"
#include "CiModel.h"
#endif

#include "Register.h"

#include "ToolHit.h"

// ==================================================================

CToolHit::CToolHit( void )
{
	for (int indx = 0; indx < HIT_LIMIT; ++indx)
	{ m_profile[indx] = new CGeoPolyArray(); }

	// Relevant only when HardPierce() returns true.
	m_extremes = NULL;

	m_dexgrid = NULL;

	m_has_patterns = FALSE;
	m_pat_burn = NULL;
	m_pat_punch = NULL;
	m_pat_other = NULL;

	m_part = NULL;
	m_hit = -1;

	m_height = -1.0;

	m_rot = 0.0;
	m_mirror = false;

	m_just_placed = false;
}

CToolHit::~CToolHit( void )
{
	for (int indx = 0; indx < HIT_LIMIT; ++indx)
	{
		CGeoPolyArray* gpa = m_profile[indx];
		m_profile[indx] = NULL;

		gpa->DestructiveFlush();
		delete(gpa);
	}

	delete m_extremes;

	if (m_dexgrid) { delete m_dexgrid; m_dexgrid = NULL; }

	m_link_array.DestructiveFlush();
}

// Set the Grid
void CToolHit::Grid( CDexGrid* grid )
{
	if (m_dexgrid) delete m_dexgrid;

	m_dexgrid = grid;
}

C2dCoord CToolHit::WorldToGrid( double x_world, double y_world ) const
{
	double	dx, dy;
	double	rx, ry;
	int		nx, ny;
	int	x, y;

	// 2006.04.15 (PE) -- Because floating point differences
	// would cause wrong orientation of part to score higher.
	x_world = ROUND( x_world, SMALL );
	y_world = ROUND( y_world, SMALL );

	dx = (x_world - m_dexgrid->Origin().X()) / m_dexgrid->Resolution();
	dy = (y_world - m_dexgrid->Origin().Y()) / m_dexgrid->Resolution();

	nx = (int) dx;
	ny = (int) dy;

	rx = dx - nx;
	ry = dy - ny;

#if ORIGINAL_CODE // seems fishy ... why 0.5?
	x = ((rx < 0.5) ? nx : (nx + 1));
	y = ((ry < 0.5) ? ny : (ny + 1));
#else
	x = ((rx < m_dexgrid->Resolution()) ? nx : (nx + 1));
	y = ((ry < m_dexgrid->Resolution()) ? ny : (ny + 1));
#endif

	return C2dCoord( x, y );
}

C2dCoord CToolHit::GridToWorld( const C2dCoord& grid_pt ) const
{
	double x = (grid_pt.X() * m_dexgrid->Resolution()) + m_dexgrid->Origin().X();
	double y = (grid_pt.Y() * m_dexgrid->Resolution()) + m_dexgrid->Origin().Y();

	return C2dCoord( x, y );
}

void CToolHit::LinkCopyAppend( const CLink& link )
{
	static double TOL = 1.e-4;  // arbitrary tolerance

	int indx, count = m_link_array.Count();
	for (indx = 0; indx < count; ++indx)
	{
		CLink* tmp = m_link_array.GetAt( indx );
		if ( tmp->IsEqual( link, TOL ) )
			break;
	}

	if (indx >= count)
	{
		// 'link' is unique.
		// TODO: Do what about links having same delta but different priority (?)
		CLink* copy = new CLink( link );
		if (copy != NULL)
			m_link_array.Append( copy );
	}
}

void CToolHit::LinksSort()
{
	m_link_array.Sort();
}

// ==================================================================
//	Debug generate... instantiates the geometry in the model
//	while drawing it... may crash your program, so use wisely.
//
#if (_CI || _NST)

	CReturn		
	CToolHit::DebugToModel( C3dCoord* shift, CViewMgr* view, CModel* model )
	{
		CReturn			status;
		CDbTool*		layer;
		CDbWorkplane*	work;

		C3x4Matrix	shift_mat;
		
		model->EntityCreate( DBLAYER, (CToolHit**) &layer );
		layer->Name("nest_test");
		layer->ColorSet( 1 );  // red
		model->ActiveTool( layer );

		work = model->ActiveWorkplane();

		shift_mat.setUnit();
		if (shift)
			shift_mat.Shift( C3dVec( shift->X(), shift->Y(), 0.0 ) );

		int color_array[7] = { 0, 1, 2, 3, 4, 5, 6 };

		for (int depth=0; depth<Count(); depth++)
		{
			const CGeoPolyArray* resultarray = m_profile[depth];

			int num = resultarray->Count();
			for (int idx=0; idx<num; idx++)
			{
				CGeoPoly* result = (*resultarray)[idx];

				int cnum = result->Count();
				for (int cidx=0; cidx<cnum; cidx++)
				{
					const CGeoCurve& curve = (*result)[cidx];

					switch (curve.Type())
					{
					case GEOLINE:

						if (curve.Length2d() > SMALL)
						{
							CDbLine* db_line = NULL;
							status += model->EntityCreate( DBLINE, (CToolHit**)&db_line );
							if (status.isOkay())
							{
								status += db_line->Init( layer, work, *((CGeoLine*)&curve) );
								db_line->Transform( shift_mat );
								db_line->ColorSet( color_array[depth] );
							}
						}
						break;

					case GEOARC:

						if ( ((CGeoArc*)&curve)->Radius() > SMALL)
						{
							CDbArc* db_arc = NULL;
							status += model->EntityCreate( DBARC, (CToolHit**)&db_arc );
							if (status.isOkay())
							{
								status += db_arc->Init( layer, work, *((CGeoArc*)&curve) );
								db_arc->Transform( shift_mat );
								db_arc->ColorSet( color_array[depth] );
							}
						}
						break;
					}
				}
			}
		}

		return status;
	}

#else

	CReturn CToolHit::DebugToModel( const C3dCoord& shift, CViewMgr* view, CModel* model )
	{
		// static int LIMIT = HIT_NEST_LIMIT;
		static int LIMIT = HIT_LIMIT;
		CReturn	status;

		if ((view == NULL) || (model == NULL))
			return STATUS_OKAY;  // Well, not really ....

		CViewBase* vbase = view->ActiveView();

		CDbTool* tool = model->ActiveTool();
		CDbWorkplane* work = model->ActiveWorkplane();

		if ((tool == NULL) || (work == NULL))
			return STATUS_OKAY;  // Well, not really ....

		view->ModelSet( *model );

		C3x4Matrix shift_mat;
		shift_mat.Shift( C3dVec( shift.X(), shift.Y(), 0. ) );

		int color_array[7] = { 0xffffff, 0x0000ff, 0x00ff00, 0xff0000, 0x00ffff, 0xffff00, 0xff00ff };

		vbase->DisplayListBegin( STEMP_LIST );

		for (int depth = HIT_NEST_OUTSIDE; depth < LIMIT; ++depth)
		{
			const CGeoPolyArray* resultarray = m_profile[depth];

			int num = resultarray->Count();
			for (int idx=0; idx<num; idx++)
			{
				CGeoPoly* result = (*resultarray)[idx];

				int cnum = result->Count();
				for (int cidx=0; cidx<cnum; cidx++)
				{
					const CGeoCurve& curve = (*result)[cidx];
					CDbEntity* db_ent = NULL;

					((C3dCoord&)curve.StartPt()).Z(0.);
					((C3dCoord&)curve.EndPt()).Z(0.);
					switch (curve.Type())
					{
					case GEOLINE:

						if (curve.Length2d() > SMALL)
						{
							CDbLine* db_line = NULL;
							status += model->EntityCreate( DBLINE, (CDbEntity**)&db_line );
							if (status.isOkay())
							{
								status += db_line->Init( tool, work, *((CGeoLine*)&curve) );
								db_line->Transform( shift_mat );
							}
							
							if (status.isOkay())
								db_ent = db_line;
						}
						break;

					case GEOARC:

						((C3dCoord&)((CGeoArc&) curve).CenterPt()).Z(0.);

						if (((CGeoArc&) curve).Radius() > SMALL)
						{
							CDbArc* db_arc = NULL;
							status += model->EntityCreate( DBARC, (CDbEntity**)&db_arc );
							if (status.isOkay())
							{
								status += db_arc->Init( tool, work, *((CGeoArc*)&curve) );
								db_arc->Transform( shift_mat );
							}

							if (status.isOkay())
								db_ent = db_arc;
						}
						break;
					}

					if (db_ent)
					{
						db_ent->AttribsDelete();
						db_ent->ColorSet( color_array[depth] );

						vbase->DrawAtColor( DCOLOR_WHITE );
						vbase->DrawGeoAt( curve, shift );
					}
				}
			}
		}

		vbase->DisplayListEnd( STEMP_LIST );
		vbase->BufferShow( FRONT_BUFFER );

		return status;
	}

#endif

CGeoPolyArray* CToolHit::PolysGet( int depth ) const
{
	return m_profile[depth];
}

void CToolHit::PolyAdd( int depth, CGeoPoly* poly )
{
	m_profile[depth]->Append( poly );
}


void CToolHit::PartialCopy( const CToolHit& source, const C2dCoord& handle )
{
	C3x4Matrix shift_mat;


	// Who knows what data we really need at this point ....
	// m_dexgrid = NULL;

	// m_has_patterns = NULL;
	// m_pat_punch = NULL;
	// m_pat_burn = NULL;
	// m_pat_other = NULL;

	// m_height = source.m_height;

	// The part from which this object was derived.
	m_part = source.m_part;  // already set.

	// The "hit index" of this object in the part.
	// Each "hit index" represents a CToolHit object
	// having a different orientation.
	m_hit = source.m_hit;		// Hit index in the part

	// TRUE if this is a mirrored part relative to the original
	m_mirror = source.m_mirror;

	// Rotation angle of this toolhit relative to the original
	m_rot = source.m_rot;

	// Where the hit handle was (world)
	m_pos = handle;

	// Link to zero or more prioritized "next hit" links.
	// Replaces the "next_*()" system of calls.  Also
	// supports pre-nesting with critical-priority links.
	// m_link_array = ???'

	m_mer = source.m_mer;	// minimum enclosing rectangle of outer-kerf.
	m_delta = source.m_delta;


	shift_mat.Shift( C3dVec( handle.X(), handle.Y(), 0. ) );

	for (int depth = 0; depth <= HIT_KERF_OUTSIDE; ++depth)
	{
		const CGeoPolyArray* src_array = source.m_profile[depth];
		if (src_array != NULL)
		{
			for (int idx = 0; idx < src_array->Count(); ++idx)
			{
				CGeoPoly* dst_poly = PolyGet( (*src_array), depth, idx );
				if (dst_poly != NULL)
				{
					dst_poly->Xform( shift_mat );

#if BEFORE_V19
					// Well, this was actually early-V19 behavior but, as it turned
					// out, it seemed to cause problems, particularly where seeding
					// the sheet is concerned. See also CNestMgr::nest_area()
					// Hmmmmmm .....
					if (depth >= HIT_PART_INSIDE)
						dst_poly->Reverse();
#else
#endif

					m_profile[depth]->Append( dst_poly  );
				}
			}
		}
	}

	// 2008.02.02 (PE) -- I don't really like this but it is (for now)
	// the only way I can see to manage this information. The problem
	// at hand is to propogate any pierce hole information (generated
	// by CNestingPart::Accessorize()) so that it may be used by
	// methods like CSheet::TestFit().
	m_attribs = source.m_attribs;

	double radius = m_attribs.getReal( "radius", UNDEFINED );
	if (radius < UNDEFINED)
	{
		double dval;

		dval = m_attribs.getReal( "xc", 0. );
		m_attribs.setReal( "xc", (dval + handle.X()) );

		dval = m_attribs.getReal( "yc", 0. );
		m_attribs.setReal( "yc", (dval + handle.Y()) );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// 2006.10.23 (PE) -- Introduced for V18 because Sunflower reported
// cases when the pierce hole would overlap an adjacent part.
//
// This method returns either a simple unadulterated poly or a poly
// representing the outside kerf with pierce hole geometry included
// (when present in the data). It should never return a poly that
// represents a pierce hole by itself.
//
// ASSUMPTIONS:
// 1) The poly at index of zero is always the outside kerf.
// 2) Only poly's representing a pierce hole have attributes.
//
// NOTE: We can revert to previous behavior by simply always returning
// the unadulterated poly.
//
// NOTE: CLONE (IDENTICAL TO) CSheet::PolyGet()
// TODO: Eliminate redundancy.
CGeoPoly* CToolHit::PolyGet(
	const CGeoPolyArray&	src_array,
	int						depth,
	int						poly_indx )
{
	CGeoPoly* result = NULL;

	CGeoPoly* src_poly = src_array[poly_indx];

	if ((depth == HIT_NEST_OUTSIDE) && (src_poly->Count() > 0))
	{
		const CGeoCurve& geoCurve = src_poly->GetAt(0);
		if ( !geoCurve.HasAttrib() || (geoCurve.IntGet( "lead", 0 ) == 0))
		{
			// Simply copy the poly.
			result = (CGeoPoly*) src_poly->Clone( true );
		}

		if (poly_indx == 0)
		{
			// And append any lead / pierce hole geometry that we may encounter.
			int poly_cnt = src_array.Count();
			for (int poly_poly_indx = 1; poly_poly_indx < poly_cnt; ++poly_poly_indx)
			{
				CGeoPoly* pierce_poly = src_array[poly_poly_indx];
				
				int pierce_cnt = pierce_poly->Count();
				for (int pierce_poly_indx = 0; pierce_poly_indx < pierce_cnt; ++pierce_poly_indx)
				{
					const CGeoCurve& geoCurve = pierce_poly->GetAt( pierce_poly_indx );
					if ( !geoCurve.HasAttrib() || (geoCurve.IntGet( "lead", 0 ) == 0))
						break;

					result->CopyAppend( geoCurve );
				}
			}
		}
	}
	else
	{
		// Simply copy the poly.
		result = (CGeoPoly*) src_poly->Clone( true );
	}

	return result;
}

bool CToolHit::JustPlaced() const
{
	return m_just_placed;
}

void CToolHit::JustPlaced( bool just_placed )
{
	m_just_placed = just_placed;
}


CReturn CToolHit::ExtremesExtract( eNestProgression progression )
{
	CReturn		status;

	CGeoPolyArray* polys = m_profile[HIT_PART_OUTSIDE];
	CGeoPoly& poly = *(polys->GetAt(0));

	if ((progression == PROGRESS_PPY_SPX) || (progression == PROGRESS_PPX_SPY))
		m_extremes = TRExtremesExtract( poly );
	else
		m_extremes = BRExtremesExtract( poly );

	return status;
}

CGeoPoly* CToolHit::TRExtremesExtract( const CGeoPoly& poly )
{
	CGeoPoly*	extremes;
	double		xmax, ymax;
	int			xmax_indx, ymax_indx;
	int			count, indx, limit;

	xmax = -UNDEFINED;
	xmax_indx = -1;

	ymax = -UNDEFINED;
	ymax_indx = -1;

	// ASSUMPTION: The poly has a CCW winding direction.
	// We could assert on the winding direction but that adds
	// significant(?) computation.
	//   int dir = poly.Winding();

	count = poly.Count();
	for (indx = 0; indx < count; ++indx)
	{
		const CGeoCurve& curve = poly[indx];
		const C3dCoord& ps = curve.StartPt();
		const C3dCoord& pe = curve.EndPt();

		if (ps.Y() > ymax)
		{
			ymax = ps.Y();
			ymax_indx = indx;
		}
		
		if (ps.X() > xmax)
		{
			xmax = ps.X();
			xmax_indx = indx;
		}
	}

	C2dVec vec( poly[ymax_indx].StartTan() );
	C2dVec down( 0., -1. );

	double dot = vec * down;
	if (dot >= 0.)
	{
		--ymax_indx;
		if (ymax_indx < 0)
			ymax_indx = count - 1;
	}

	// Copy the relevant bits.

	extremes = new CGeoPoly();

	limit = ((xmax_indx <= ymax_indx) ? (ymax_indx+1) : count);
	for (indx = xmax_indx; indx < limit; ++indx)
	{
		extremes->CopyAppend( poly[indx] );
	}

	if (limit == count)
	{
		for (indx = 0; indx < ymax_indx; ++indx)
		{
			extremes->CopyAppend( poly[indx] );
		}
	}

	// We need a CW poly for booleans to work(?)
	extremes->Reverse();

	return extremes;
}

CGeoPoly* CToolHit::BRExtremesExtract( const CGeoPoly& poly )
{
	CGeoPoly*	extremes;
	double		xmax, ymin;
	int			xmax_indx, ymin_indx;
	int			count, indx, limit;

	xmax = -UNDEFINED;
	xmax_indx = -1;

	ymin = UNDEFINED;
	ymin_indx = -1;

	// ASSUMPTION: The poly has a CCW winding direction.
	// We could assert on the winding direction but that adds
	// significant(?) computation.
	//   int dir = poly.Winding();

	count = poly.Count();
	for (indx = 0; indx < count; ++indx)
	{
		const CGeoCurve& curve = poly[indx];
		const C3dCoord& ps = curve.StartPt();
		const C3dCoord& pe = curve.EndPt();

		if (ps.Y() < ymin)
		{
			ymin = ps.Y();
			ymin_indx = indx;
		}
		
		if (ps.X() > xmax)
		{
			xmax = ps.X();
			xmax_indx = indx;
		}
	}

	C2dVec vec( poly[ymin_indx].StartTan() );
	C2dVec up( 0., 1. );

	double dot = vec * up;
	if (dot >= 0.)
	{
		--ymin_indx;
		if (ymin_indx < 0)
			ymin_indx = count - 1;
	}

	// Copy the relevant bits.

	extremes = new CGeoPoly();

	limit = ((ymin_indx <= xmax_indx) ? (xmax_indx+1) : count);
	for (indx = ymin_indx; indx < limit; ++indx)
	{
		extremes->CopyAppend( poly[indx] );
	}

	if (limit == count)
	{
		for (indx = 0; indx < xmax_indx; ++indx)
		{
			extremes->CopyAppend( poly[indx] );
		}
	}

	// We need a CW poly for booleans to work(?)
	extremes->Reverse();

	return extremes;
}


// Attribute management.

int CToolHit::AttribCount() const
{
	return (m_attribs.countVar());
}

int CToolHit::IntGet( const CString& name, int defval ) const
{
	return (m_attribs.getInt( name, defval ));
}

double CToolHit::DoubleGet( const CString& name, double defval ) const
{
	return (m_attribs.getReal( name, defval ));
}

CString CToolHit::StringGet( const CString& name, const CString& defval ) const
{
	return (m_attribs.getString( name, defval ));
}

void CToolHit::IntSet( const CString& name, int ival )
{
	m_attribs.setInt( name, ival );
}

void CToolHit::DoubleSet( const CString& name, double dval )
{
	m_attribs.setReal( name, dval );
}

void CToolHit::StringSet( const CString& name, const CString& sval )
{
	m_attribs.setString( name, sval );
}

void CToolHit::AttribsDelete()
{
	m_attribs.Reset();
}

void CToolHit::AttribDelete( const CString& name )
{
	m_attribs.deleteVar( name );
}
