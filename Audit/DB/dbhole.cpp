
#include "stdafx.h"
#include "MathConst.h"
#include "StringConst.h"
#include "GeoPoint.h"
#include "3dCoord.h"
#include "GeoArc.h"

#include "EntityDb.h"
#include "DbWorkplane.h"
#include "DbHole.h"
#include "DbTool.h"
#include "DbContainer.h"
#include "DbSequence.h"
#include "DbEntityVisitor.h"
#include "DisplayEntity.h"

#define ANG_TOL 1.e-2  // arbitrary

DWORD g_draw_color = 0;
DWORD g_rapid_color = 0;


////////////////////////////////////////////////////////////////////////

CDbHole::CDbHole( CEntityDb* db )

	: CDbEntity( db ),
	  m_center(),
	  m_diam( UNDEFINED ),
	  m_depth( UNDEFINED )
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	(*pAttrib()) = CDbEntity::Db()->Default();
}

CDbHole::CDbHole( const CDbHole& dbHole )

	: CDbEntity( dbHole ),
	  m_center( dbHole.m_center ),
	  m_diam( dbHole.m_diam ),
	  m_depth( dbHole.m_depth )
{
}

CDbHole::~CDbHole()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

#if BEFORE_V18
C3dBox
CDbHole::Box( ID workplaneId ) const
{
	C3dCoord pt = Center( workplaneId );
	double rad = m_diam / 2;

	double xmin = pt.X() - rad;
	double ymin = pt.Y() - rad;
	double zmin = pt.Z() - m_depth;

	double xmax = pt.X() + rad;
	double ymax = pt.Y() + rad;
	double zmax = pt.Z();

	C3dBox box( xmin, ymin, zmin, xmax, ymax, zmax );
	return box;
}
#else
C3dBox
CDbHole::Box( ID workplaneId ) const
{
	C3dBox		box;
	C3dCoord	pt;
	CDbTool*	dbTool;

	pt = Center( workplaneId );

	dbTool = Tool();
	if ((dbTool != NULL) && !dbTool->IsLayer())
	{
		CGeoPoly	geoPoly;
		double		orient;

		dbTool->Convert( &geoPoly );

		orient = DoubleGet( "orient", 0. );
		if (fabs( orient ) > 1.e-2)
		{
			C3x4Matrix	xform;

			xform.setXYAngle( orient * DEG2RAD );

			geoPoly.Xform( xform );
		}

		box = geoPoly.Extent();

		// 2012.02.02 (PE) -- Sheesh!
		box.Zmin( pt.Z() );
		box.Zmax( pt.Z() );

		box.Shift( C3dVec( pt.X(), pt.Y(), 0. ) );
	}
	else
	{
		double rad = m_diam / 2;

		box.Update(
			(pt.X() - rad), (pt.Y() - rad), (pt.Z() - m_depth),
			(pt.X() + rad), (pt.Y() + rad), pt.Z() );
	}

	// 2012.02.02 -- Sheeesh!
	((CDbHole*) this)->m_box_cache = box;


	return box;
}
#endif

void
CDbHole::Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& center, double diam, double depth )

{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	m_center = center;
	m_diam = diam;
	m_depth = depth;

	C2dBox box;

	double radius = diam / 2.0;

	box.Xmin( center.X() - radius );
	box.Ymin( center.Y() - radius );
	box.Xmax( center.X() + radius );
	box.Ymax( center.Y() + radius );

	CDbEntity::Tool( tool );
	CDbEntity::Workplane( workplane );
}

C3dCoord
CDbHole::Center( ID workplaneId ) const
{
	C3dCoord result;

	if (workplaneId == ~0 || workplaneId == CDbEntity::WorkplaneId())
	{
		// No transformation required.
		result = m_center;
	}
	else
	{
		// Transform to global coordinates.
		const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
		const C3x4Matrix& xform = dbWorkplane->Transform();
		xform.TransformTo( m_center, &result );

		if (workplaneId > 0)
		{
			// Transform back to the specified workplane.
			CDbEntity::Find( workplaneId, (CDbEntity**) &dbWorkplane, DBWORKPLANE, DBWORKPLANE );
			ASSERT( (dbWorkplane != NULL) );
			const C3x4Matrix& inverse = dbWorkplane->Inverse();
			inverse.TransformTo( result, &result );
		}
	}

	return result;
}

C3dCoord
CDbHole::StartPt( ID workplaneId ) const
{
	C3dCoord	ps;
	int			sncs;

	ps = Center( workplaneId );

	sncs = IntGet( "sncs", 0 );
	switch (sncs)
	{
	case 307:  // bhc
		{
		double rad = DoubleGet( "rad", 0. );
		double as = DoubleGet( "as", 0. ) * DEG2RAD;

		ps.X( ps.X() + (rad * cos(as)) );
		ps.Y( ps.Y() + (rad * sin(as)) );
		}
		break;

	default:
		break;
	}

	return ps;
}

C3dCoord
CDbHole::EndPt( ID workplaneId ) const
{
	C3dCoord	pe;
	int			sncs;

	pe = Center( workplaneId );

	sncs = IntGet( "sncs", 0 );
	switch (sncs)
	{
	case 302:  // row in x
	case 303:  // row in y
	case 304:  // laa / dist normal
	case 305:  // laa / dist parallel
		{
		// Get the params in common between all LAA forms.
		// NOTE: We default 'cnt' to 1 so as to get 0 if the
		// attribute is missing. Either way, the Nth hole is
		// at a distance of (cnt - 1) from the 0th hole.
		double ang = DoubleGet( "ang", 0. );
		double dst = DoubleGet( "dst", 0. );
		int cnt = IntGet( "cnt", 1 ) - 1;

		ang *= DEG2RAD;
		pe.X( (pe.X() + ((dst * cos(ang)) * cnt)) );
		pe.Y( (pe.Y() + ((dst * sin(ang)) * cnt)) );
		}
		break;

	case 306:  // grid
		{
		int prim = IntGet( "prim", 0 );
		double xdst = DoubleGet( "xdst", 0. );
		int xcnt = IntGet( "xcnt", 1 ) - 1;
		double ydst = DoubleGet( "ydst", 0. );
		int ycnt = IntGet( "ycnt", 1 ) - 1;

		// The end point of the grid is *not* the point
		// at the furthest extent but it is the point that
		// would be last processed in a canned cycle.
		if (prim == 0)
		{
			if (ycnt & 1)
				xdst = 0.;
		}
		else
		{
			if (xcnt & 1)
				ydst = 0.;
		}

		pe.X( pe.X() + (xdst * xcnt) );
		pe.Y( pe.Y() + (ydst * ycnt) );
		}
		break;

	case 307:  // bhc
		{
		double rad = DoubleGet( "rad", 0. );
		double as = DoubleGet( "as", 0. ) * DEG2RAD;
		double ai = DoubleGet( "ai", 0. ) * DEG2RAD;
		int cnt = IntGet( "cnt", 1 ) - 1;

		double ae = as + (ai * cnt);

		pe.X( pe.X() + (rad * cos(ae)) );
		pe.Y( pe.Y() + (rad * sin(ae)) );
		}
		break;

	default:
		break;
	}

	return pe;
}

void
CDbHole::RefsTo( CDbEntityList* list ) const
{
	// Do nothing.  A hole does not reference any entities.
}

void
CDbHole::Subordinates( CDbEntityList* list ) const
{
	// Do nothing.  A hole does not have any subordinate entities.
}

bool
CDbHole::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

	return FALSE;
}

void
CDbHole::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	// Divorce this entity from its owner.
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( CDbEntity::Owner() );
	if (dbContainer != NULL)
		dbContainer->Disown( this );

	CDbSequence* dbSequence = CDbEntity::Sequence();
	if (dbSequence != NULL)
		this->Sequence( NULL );

	// Sever the ties between all other entities and this entity.
	CDbEntity::RemoveRefs();

	CDbEntity::Workplane( NULL );
	CDbEntity::Tool( NULL );
}

void
CDbHole::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbHole::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  A hole never references another entity.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbHole&
CDbHole::operator = ( const CDbHole& dbHole )
{
	CDbEntity::CommonCopy( dbHole );

	m_center = dbHole.m_center;
	m_diam   = dbHole.m_diam;
	m_depth  = dbHole.m_depth;

	return (*this);
}

CReturn
CDbHole::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbHole* dbHole = new CDbHole( (*this) );

	(*dbEntity) = dbHole;

	return status;
}

CReturn
CDbHole::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbHole* dbHole = dynamic_cast<CDbHole*>( dbEntity );

	ASSERT( (dbHole != NULL) );

	CDbHole tmp( (*this) );
	(*this) = (*dbHole);
	(*dbHole) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Describe2d
//
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
//	Arc explode into lines -- taken from Trace.cpp in the old nstPart,
//	inspired by Graphics Gem V Circular Arc Subdivision, K. Turkowski,
//	p.168...
//
void
CDbHole::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			in_tolerance ) const
{
	if ( is_hidden() )
		return;  // Nothing to do.

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );
	if ( !UseOverrideColor() && !regen && !allocated )
		return;  // Nothing to do.

	dispent->Flush();

	int	indx;


	CDbTool*	dbTool = Tool();

	// -----------------------------------------------------
	// Prepare for arc tabulation.
	//
	double		radius = m_diam / 2.0;

	C3dCoord	pc = Center(0);
	C3dCoord	ps = pc + C3dVec( radius, 0, 0 );
	CGeoArc		arc( ps, ps, pc, 1 );
	C3dCoord	pm = arc.MidPt();


	// -----------------------------------------------------
	//		Header, Dot, and depth...
	//
	indx = dispent->CoordAppend( pc );
	dispent->CommandAppend( DCMD_DOT, (DWORD) indx );

	g_draw_color = ColorGet( DCOLOR_BLUE );

	if ( IsToolpath() )
	{
		if (Flags() & DBSHOWPATH)
		{
			dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
			g_draw_color = DCOLOR_RED;
		}
		else
			dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_TOOL );
	}
	else
	{
		dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
	}

	// -----------------------------------------------------
	//		Arc body...
	//
	// NOTE: The arc mid-pt must be in the display list for the following reason:
	//
	// 1. The rendering system can not properly draw 180+ degree arcs.
	//
	if (IsToolpath() && dbTool)
	{
		(*io_tooltip) = pc;			// Move dbTool there...

		CDbEntity::TransientOrientationSet();

		DescribeHits( regen, (*dbTool), in_tolerance );
	}
	else
	{
		dispent->CommandAppend( DCMD_COLOR, (DWORD) g_draw_color );

		indx = dispent->CoordAppend( ps );
		dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

		indx = dispent->CoordAppend( pc );
		dispent->CommandAppend( DCMD_ARCCTR, (DWORD) indx );

		indx = dispent->CoordAppend( pm );
		dispent->CommandAppend( DCMD_ARCCWTO, (DWORD) indx );

		indx = dispent->CoordAppend( ps );
		dispent->CommandAppend( DCMD_ARCCWTO, (DWORD) indx );
	}

	// -----------------------------------------------------
	// Cleanup
	//
#if REQUIRED
	CDbEntity::Describe2d( regen, &pe, in_tolerance );
#endif
}

void 
CDbHole::DescribeHits( 
	int					regen,
	const CDbTool&		dbTool,
	double				in_tolerance ) const
{
	g_rapid_color = dbTool.ColorGet( DCOLOR_RED );

	int sncs = IntGet( "sncs", 0 );
	if (sncs > 0)
	{
		switch (sncs)
		{
		case 302:  // row in x
		case 303:  // row in y
		case 304:  // laa / dist normal
		case 305:  // laa / dist parallel
			DescribeLAA( regen, dbTool, in_tolerance );
			break;

		case 306:  // grid
			DescribeGrid( regen, dbTool, in_tolerance );
			break;

		case 307:  // bhc
			DescribeBHC( regen, dbTool, in_tolerance );
			break;
		}
	}
	else
	{
		const CDbWorkplane*	dbWorkplane = CDbEntity::Workplane();
		const C3x4Matrix&	xform = dbWorkplane->Transform();

		double radians = dbTool.DoubleGet( STR_INDEX_ANGLE, 0.0 ) * DEG2RAD;
		C2dUnitVec vec( radians );

		bool allocated;
		CDisplayEntity* dispent = DisplayEntityGet( &allocated );

		dispent->CommandAppend( DCMD_COLOR, (DWORD) g_draw_color );

		dbTool.Describe2d( regen, xform, Center(),
			vec, FALSE, 0, in_tolerance, dispent );
	}
}

void
CDbHole::DescribeLAA(
	int					regen,
	const CDbTool&		dbTool,
	double				in_tolerance ) const
{
	C3dCoord	ps;
	C2dVec		vec;
	double		dst, ang;
	int			cnt;

	ang = DoubleGet( "ang", 0. ) * DEG2RAD;
	dst = DoubleGet( "dst", 0. );
	cnt = IntGet( "cnt", 0 );

	ps = StartPt();
	vec.Init( (dst * cos(ang)), (dst * sin(ang)) );

	DescribeHandle( regen, ps, "LAA", in_tolerance );

	DescribeRow( regen, dbTool, ps, vec, cnt, in_tolerance );
}

void
CDbHole::DescribeBHC(
	int					regen,
	const CDbTool&		dbTool,
	double				in_tolerance ) const
{
	C3dCoord	pc;
	C3dCoord	ps;
	C3dCoord	pe;
	double		as, ai, rad;
	int			count, indx;

	const CDbWorkplane*	dbWorkplane = CDbEntity::Workplane();
	const C3x4Matrix&	xform = dbWorkplane->Transform();

	double radians = dbTool.DoubleGet( STR_INDEX_ANGLE, 0.0 ) * DEG2RAD;
	C2dUnitVec align( radians );

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	as = DoubleGet( "as", 0. ) * DEG2RAD;
	ai = DoubleGet( "ai", 0. ) * DEG2RAD;
	rad = DoubleGet( "rad", 0. );
	count = IntGet( "cnt", 0 );

	pc = Center();
	pe.Z( pc.Z() );

	DescribeHandle( regen, pc, "BHC", in_tolerance );

	for (indx = 0; indx < count; ++indx)
	{
		radians = as + (ai * indx);

		pe.X( pc.X() + (rad * cos(radians)) );
		pe.Y( pc.Y() + (rad * sin(radians)) );

		dispent->CommandAppend( DCMD_COLOR, (DWORD) g_draw_color );

		dbTool.Describe2d( regen, xform, pe,
			align, FALSE, 0, in_tolerance, dispent );

		if (Flags() & DBSHOWPATH)
		{
			if (indx > 0)
			{
				DescribeRapid( regen, dbTool, xform, ps, pe );

				// Reset the line style.
				dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
			}

			ps = pe;
		}
	}
}

void
CDbHole::DescribeGrid(
	int					regen,
	const CDbTool&		dbTool,
	double				in_tolerance ) const
{
	C3dCoord	orig;
	C3dCoord	ps;
	C3dCoord	pe;
	C2dVec		vec;
	double		xdst, ydst, ang;
	int			prim, xcnt, ycnt, indx;

	const CDbWorkplane*	dbWorkplane = CDbEntity::Workplane();
	const C3x4Matrix&	xform = dbWorkplane->Transform();

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	prim = IntGet( "prim", 0 );  // default to X-Axis
	ang = DoubleGet( "ang", 0. );  // rotation of grid
	xdst = DoubleGet( "xdst", 0. );
	xcnt = IntGet( "xcnt", 0 );
	ydst = DoubleGet( "ydst", 0. );
	ycnt = IntGet( "ycnt", 0 );

	orig = StartPt();
	ps.Z( orig.Z() );
	pe.Z( orig.Z() );

	DescribeHandle( regen, orig, "GRID", in_tolerance );

	if (prim == 0)  // arbitrary is-zero determination
	{
		// ie. G78 -- Primary direction is X-Axis.
		for (indx = 0; indx < ycnt; ++indx)
		{
			ps.Y( orig.Y() + (ydst * indx) );

			if ((indx & 1) == 0)
			{
				ps.X( orig.X() );
				vec.Init( xdst, 0. );
			}
			else
			{
				ps.X( orig.X() + (xdst * (xcnt - 1)) );
				vec.Init( -xdst, 0. );
			}

			if ((Flags() & DBSHOWPATH) && (indx > 0))
			{
				pe.X( ps.X() );
				pe.Y( ps.Y() - ydst );
				DescribeRapid( regen, dbTool, xform, pe, ps );

				// Reset the line style.
				dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
			}

			dispent->CommandAppend( DCMD_COLOR, (DWORD) g_draw_color );

			DescribeRow( regen, dbTool, ps, vec, xcnt, in_tolerance );
		}
	}
	else
	{
		// ie. G79 -- Primary direction is Y-Axis.
		for (indx = 0; indx < xcnt; ++indx)
		{
			ps.X( orig.X() + (xdst * indx) );

			if ((indx & 1) == 0)
			{
				ps.Y( orig.Y() );
				vec.Init( 0., ydst );
			}
			else
			{
				ps.Y( orig.Y() + (ydst * (ycnt - 1)) );
				vec.Init( 0., -ydst );
			}

			if ((Flags() & DBSHOWPATH) && (indx > 0))
			{
				pe.X( ps.X() - xdst );
				pe.Y( ps.Y() );
				DescribeRapid( regen, dbTool, xform, pe, ps );

				// Reset the line style.
				dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
			}

			dispent->CommandAppend( DCMD_COLOR, (DWORD) g_draw_color );

			DescribeRow( regen, dbTool, ps, vec, ycnt, in_tolerance );
		}
	}
}

void
CDbHole::DescribeRow(
	int					regen,
	const CDbTool&		dbTool,
	const C3dCoord&		ps,
	const C2dVec&		vec,
	int					count,
	double				in_tolerance ) const
{
	C3dCoord	pe;

	const CDbWorkplane*	dbWorkplane = CDbEntity::Workplane();
	const C3x4Matrix&	xform = dbWorkplane->Transform();

	double radians = dbTool.DoubleGet( STR_INDEX_ANGLE, 0.0 ) * DEG2RAD;
	C2dUnitVec align( radians );

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	dispent->CommandAppend( DCMD_COLOR, (DWORD) g_draw_color );

	for (int indx = 0; indx < count; ++indx)
	{
		pe.X( ps.X() + (vec.X() * indx) );
		pe.Y( ps.Y() + (vec.Y() * indx) );
		dbTool.Describe2d( regen, xform, pe,
			align, FALSE, 0, in_tolerance, dispent );
	}

	if (Flags() & DBSHOWPATH)
	{
		DescribeRapid( regen, dbTool, xform, ps, pe );
	}
}

void
CDbHole::DescribeRapid(
	int					regen,
	const CDbTool&		dbTool,
	const C3x4Matrix&	xform,
	const C3dCoord&		ps,
	const C3dCoord&		pe ) const
{
	C3dCoord	ps_world, pe_world;
	int			indx;

	ps_world = ps;
	pe_world = pe;

	xform.Transform( &ps_world );
	xform.Transform( &pe_world );

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	// Swiped (sort of) from CoglViewWire::RapidLine()
	dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SELECT );
	dispent->CommandAppend( DCMD_COLOR, (DWORD) g_rapid_color );

	indx = dispent->CoordAppend( ps_world );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( pe_world );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );
}

void
CDbHole::DescribeHandle(
	int					regen,
	const C3dCoord&		pt,
	const char*			label,
	double				in_tolerance ) const
{
	const CDbWorkplane*	dbWorkplane = CDbEntity::Workplane();
	const C3x4Matrix&	xform = dbWorkplane->Transform();

	C3dCoord tip;
	xform.TransformTo( pt, &tip );

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	dispent->CommandAppend( DCMD_COLOR, (DWORD) g_draw_color );
	dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );

	CDbEntity::DescribeTarget( regen, label, &tip, in_tolerance );
}

C3dCoord
CDbHole::Coord( ID workplaneId ) const
{
	C3dCoord result;

	if (workplaneId == ~0 || workplaneId == CDbEntity::WorkplaneId())
	{
		// No transformation required.
		result = m_center;
	}
	else
	{
		// Transform to global coordinates.
		const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
		const C3x4Matrix& xform = dbWorkplane->Transform();
		xform.TransformTo( m_center, &result );

		if (workplaneId > 0)
		{
			// Transform back to the specified workplane.
			CDbEntity::Find( workplaneId, (CDbEntity**) &dbWorkplane, DBWORKPLANE, DBWORKPLANE );
			ASSERT( (dbWorkplane != NULL) );
			const C3x4Matrix& inverse = dbWorkplane->Inverse();
			inverse.TransformTo( result, &result );
		}
	}

	return result;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void
CDbHole::Transform( const C3x4Matrix& in_xform )
{
	if (DidAction())
		return;

	CDbEntity::Transform( in_xform );

	in_xform.Transform( &m_center );

	AttributesUpdate( in_xform );
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbHole::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbHole* dbHole = (CDbHole*)(*dbEntity);

	dbHole->Init( dbHole->Tool(), dbHole->Workplane(), m_center, m_diam, m_depth );

	return status;
}



bool
CDbHole::IsPierce() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase( "_pierce" ) == 0);
}

void
CDbHole::AttributesUpdate( const C3x4Matrix& xform )
{
	/* From the dat file ...
	'sncs_disp [ 302 | Row In X ]
	'sncs_disp [ 303 | Row In Y ]
	'sncs_disp [ 304 | At Angle / Dist Normal ]
	'sncs_disp [ 305 | At Angle / Dist Parallel ]
	'sncs_disp [ 306 | Grid ]
	'sncs_disp [ 307 | Bolt Hole Circle ]
	'sncs_disp [ 308 | Barrier ]
	 */

	int sncs = IntGet( "sncs", 0 );
	if ((sncs >=302) && (sncs <= 308))
	{
		double	delta, ang;
		double	ix, iy, jy;
		bool	mirror;

		// As pilfered (sort of) from CDbEntity::Transform().
		ix = xform.getI().X();
		iy = xform.getI().Y();
		jy = xform.getJ().Y();

		mirror = (((ix * jy) - SMALL) <= -1.);
		delta = (mirror ? 180. : (RAD2DEG * atan2( iy, ix )));

		if ( !IsZero( delta ) )
		{
			switch (sncs)
			{
			case 302:
			case 303:
			case 304:
			case 305:  // Various LAA forms.
				ang = DoubleGet( "ang", 0. );
				ang = AngNormalize( ang + delta );
				sncs = SncsFromAng( ang );
				IntSet( "sncs", sncs );
				DoubleSet( "ang", ang );
				break;

			case 306:  // Grid (don't yet support rotated grid)
				break;

			case 307:  // BHC.
				ang = DoubleGet( "as", 0. );
				ang = AngNormalize( ang + delta );
				DoubleSet( "as", ang );
				break;
			}
		}
	}
}

double
CDbHole::AngNormalize( double ang )
{
	while (ang < 0.)  	{ ang += 360.; }

	ang = ((ang / ANG_TOL) + 0.1) * ANG_TOL;

	while (ang > 360.)  { ang -= 360.; }


	return ang;
}

// Requires (0. <= ang < 360.)
int
CDbHole::SncsFromAng( double ang )
{
	int sncs;

	if ( IsMultipleOf180( ang ) )
		sncs = 302;
	else if ( IsMultipleOf180( ang + 90. ) )
		sncs = 303;
	else
		sncs = 305;

	return sncs;
}

bool
CDbHole::IsZero( double ang )
{
	return (fabs( ang ) < ANG_TOL);
}

bool
CDbHole::IsMultipleOf180( double ang )
{
	int count = (int) (ang / 180.);
	double	residual = ang - (180. * count);

	return (fabs( residual ) < ANG_TOL);
}
