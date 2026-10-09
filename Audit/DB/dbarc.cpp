
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "3x4Matrix.h"
#include "GeoArc.h"

#include "EntityDb.h"
#include "DbWorkplane.h"
#include "DbPoint.h"
#include "DbArc.h"
#include "DbTool.h"
#include "DbContainer.h"
#include "DbSequence.h"
#include "DbEntityVisitor.h"
#include "DisplayEntity.h"

#include "Solution.h"

////////////////////////////////////////////////////////////////////////

CDbArc::CDbArc( CEntityDb* db )

	: CDbCurve( db ),
	  m_ps( NULL ),
	  m_pe( NULL ),
	  m_pc( NULL )
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	(*pAttrib()) = CDbEntity::Db()->Default();
}

CDbArc::CDbArc( const CDbArc& dbArc )

	: CDbCurve( dbArc ),
	  m_ps( dbArc.m_ps ),
	  m_pe( dbArc.m_pe ),
	  m_pc( dbArc.m_pc ),
	  m_dir( dbArc.m_dir )
{
}

CDbArc::~CDbArc()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

CReturn
CDbArc::Init( CDbTool* tool, CDbWorkplane* workplane, const CGeoArc& arc )
{
	CReturn status = Init( tool, workplane, arc.StartPt(), arc.EndPt(), arc.CenterPt(), arc.Dir() );

	return status;
}

CReturn
CDbArc::Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& ps, const C3dCoord& pe, const C3dCoord& pc, int dir )
{
	CReturn status = Validate( ps, pe, pc );

	if ( status.IsOk() )
	{
		CDbEntity::Record();

		if ( !IsAssociated() )
			status = Associate();
	}

	if ( status.IsOk() )
	{
		CDbEntity::Tool( tool );
		CDbEntity::Workplane( workplane );

		m_ps->Init( tool, workplane, ps );
		m_pe->Init( tool, workplane, pe );
		m_pc->Init( tool, workplane, pc );

		m_dir = ((dir < 0) ? -1 : 1);

		Update();
	}

	return status;
}

CReturn
CDbArc::Init( CDbTool* tool, CDbWorkplane* workplane, CDbPoint* start, CDbPoint* end, CDbPoint* center, int dir )
{
	ID wpId = workplane->Id();

	const C3dCoord& ps = start->Coord( wpId );
	const C3dCoord& pe = end->Coord( wpId );
	const C3dCoord& pc = center->Coord( wpId );

	CReturn status = Validate( ps, pe, pc );

	if ( !status.IsOk() )
	{
		// For salvaging bad arcs via the debugger.
		if (0)
		{
			C2dVec vecA = ps - pc;
			C2dVec vecB = pe - pc;

			double lenA = vecA.Length();
			double lenB = vecB.Length();
			double radius = ((lenA > lenB) ? lenA : lenB);

			C3dCoord new_pc = pc;
			int okay = CSolution::ArcCenter( ps, pe, radius, dir, PI, &new_pc );
			if (okay)
			{
				center->Init( center->Tool(), center->Workplane(), new_pc ); 
				status = STATUS_OKAY;
			}
		}
	}

	if ( status.IsOk() )
	{
		CDbEntity::Record();

		CDbEntity::Tool( tool );
		CDbEntity::Workplane( workplane );

		if ( IsAssociated() )
		{
			m_ps->RefDec();
			m_pe->RefDec();
			m_pc->RefDec();
		}

		m_ps = start;
		m_pe = end;
		m_pc = center;

		m_ps->RefInc();
		m_pe->RefInc();
		m_pc->RefInc();

		m_dir = ((dir < 0) ? -1 : 1);

		Update();
	}

	return status;
}

CReturn
CDbArc::Init( ID start, ID end, ID center, int dir )
{
	CDbPoint* dbPs;
	CDbPoint* dbPe;
	CDbPoint* dbPc;

	CReturn status = Find( start, (CDbEntity**) &dbPs, DBPOINT, DBPOINT );

	if ( status.IsOk() )
		status = Find( end, (CDbEntity**) &dbPe, DBPOINT, DBPOINT );

	if ( status.IsOk() )
		status = Find( center, (CDbEntity**) &dbPc, DBPOINT, DBPOINT );

	if ( status.IsOk() )
	{
		CDbTool* dbTool = CDbEntity::Tool();
		CDbWorkplane* dbWorkplane = (CDbWorkplane*) CDbEntity::Workplane();

		status = Init( dbTool, dbWorkplane, dbPs, dbPe, dbPc, dir );
	}

	return status;
}

CGeoCurve*
CDbArc::Curve( ID workplaneId ) const
{
	return Arc( workplaneId );
}

CGeoArc*
CDbArc::Arc( ID workplaneId ) const
{
	CReturn status;

	if (workplaneId == ~0)
		workplaneId = CDbEntity::WorkplaneId();

	// TODO: Drat, would rather not search the database here!
	// Perhaps we should introduce Arc( const CDbWorkplane* ).
	// This would, of course, have a ripple effect causing us
	// to introduced Curve( const CDbWorkplane* )
	//
	// NOTE: If workplaneId == 0, it means WORLD, and not a workplane
	//
	double cross;
	if (workplaneId > 0)
	{
		CDbWorkplane* dbWork;
		status = CDbEntity::Find( workplaneId, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

		if ( !status.IsOk() )
		{
			status.Internal( IDS_INTERNAL_ERROR, "CDbArc::Arc() invalid workplane" );
			return NULL;
		}

		const C3x4Matrix& xformA = CDbEntity::Workplane()->Transform();
		const C3x4Matrix& xformB = dbWork->Transform();

		C3dVec kvecA = xformA.getK();
		C3dVec kvecB = xformB.getK();

		cross = kvecA * kvecB;
	}
	else
	{
		const C3x4Matrix& xformA = CDbEntity::Workplane()->Transform();

		C3dVec kvecA = xformA.getK();
		C3dVec kvecB( 0, 0, 1.0 );		// World Z

		cross = kvecA * kvecB;
	}

	C3dCoord ps = m_ps->Coord( workplaneId );
	C3dCoord pe = m_pe->Coord( workplaneId );
	C3dCoord pc = m_pc->Coord( workplaneId );

	// TODO: Arc direction only works when transforming between
	// parallel planes.  Transforming to some other plane will
	// requires the arc to be exploded into line segements.
	CGeoArc* arc = new CGeoArc( ps, pe, pc, ((cross > 0) ? m_dir : -m_dir) );

	return arc;
}

C3dCoord
CDbArc::StartPt( ID workplaneId ) const
{
	return m_ps->Coord( workplaneId );
}

C3dCoord
CDbArc::EndPt( ID workplaneId ) const
{
	return m_pe->Coord( workplaneId );
}

C3dCoord
CDbArc::CenterPt( ID workplaneId ) const
{
	return m_pc->Coord( workplaneId );
}

CDbPoint*
CDbArc::DbStartPt() const
{
	return m_ps;
}

void
CDbArc::DbStartPt( CDbPoint* pt )
{
	CDbEntity::Record();

	m_ps->Delete();
	m_ps = pt;
	pt->RefInc();

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

CDbPoint*
CDbArc::DbEndPt() const
{
	return m_pe;
}

void
CDbArc::DbEndPt( CDbPoint* pt )
{
	CDbEntity::Record();

	m_pe->Delete();
	m_pe = pt;
	pt->RefInc();

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

CDbPoint*
CDbArc::DbCenterPt() const
{
	return m_pc;
}

void
CDbArc::Reverse()
{
	CDbPoint* tmp = m_ps;
	m_ps = m_pe;
	m_pe = tmp;
	m_dir = ((m_dir > 0) ? -1 : 1);

	// Flip offset attribute direction, too... if it exists
	if ( CDbEntity::FlipCutside() )
	{
		int dir = IntGet( STR_CUTSIDE, 0 );
		if (dir != 0)
			IntSet( STR_CUTSIDE, -dir );
	}
}

int
CDbArc::Dir() const
{
	return m_dir;
}

void
CDbArc::Dir( int in_dir )
{
	CDbEntity::Record();
	m_dir = in_dir;

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

double
CDbArc::Radius() const
{
	C3dVec vec = m_ps->Coord() - m_pc->Coord();
	double radius = vec.Length();
	return radius;
}

void
CDbArc::RefsTo( CDbEntityList* list ) const
{
	if (m_ps != NULL)
		list->Append( m_ps );

	if (m_pe != NULL)
		list->Append( m_pe );

	if (m_pc != NULL)
		list->Append( m_pc );
}

void
CDbArc::Subordinates( CDbEntityList* list ) const
{
	RefsTo( list );
}

CReturn
CDbArc::Validate( const C3dCoord& ps, const C3dCoord& pe, const C3dCoord& pc )
{
	CReturn status;

	C2dVec vecA = ps - pc;
	C2dVec vecB = pe - pc;

	double lenA = vecA.Length();
	double lenB = vecB.Length();

	if (fabs(lenB - lenA) > SMALL)
		status.Internal( IDS_DIFF_ARC_RADII, Id() );

	return status;
}

bool
CDbArc::IsAssociated()
{
	// TODO: Is it suffifient to check only one member?
	return (m_ps != NULL && m_pe != NULL && m_pc != NULL);
}

CReturn
CDbArc::Associate()
{
	CReturn status = Create( DBPOINT, (CDbEntity**) &m_ps );

	if ( status.IsOk() )
		status = Create( DBPOINT, (CDbEntity**) &m_pe );

	if ( status.IsOk() )
		status = Create( DBPOINT, (CDbEntity**) &m_pc );

	if ( status.IsOk() )
	{
		m_ps->RefInc();
		m_pe->RefInc();
		m_pc->RefInc();

		m_ps->SystemFlag( true );
		m_pe->SystemFlag( true );
		m_pc->SystemFlag( true );
	}
	else
	{
		// Clean-up after ourselves.
		CDbEntity::Delete( (CDbEntity**) &m_ps );
		CDbEntity::Delete( (CDbEntity**) &m_pe );
		CDbEntity::Delete( (CDbEntity**) &m_pc );
	}

	return status;
}

void
CDbArc::Update()
{
	CDbEntity::CreateFlag( false );

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

bool
CDbArc::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::IsDeleted() )
		return FALSE;

	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

//	ASSERT( (m_ps != NULL && m_pe != NULL && m_pc != NULL) );

	if ( m_ps
		&& (m_ps == refdEntity) )
		return TRUE;

	if ( m_pe
		&& (m_pe == refdEntity) )
		return TRUE;

	if ( m_pc
		&& (m_pc == refdEntity) )
		return TRUE;

	return FALSE;
}

void
CDbArc::Delete()
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

	// A system point is like a 'hot potato', the last
	// line referencing a point owns the point.  If the
	// last line is deleted, so are its end points.

	if (m_ps)
		m_ps->Delete();
	if (m_pe)
		m_pe->Delete();
	if (m_pc)
		m_pc->Delete();

	m_ps = NULL;
	m_pe = NULL;
	m_pc = NULL;

	CDbEntity::Workplane( NULL );
	CDbEntity::Tool( NULL );
}

void
CDbArc::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbArc::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  Curves live by their association
	// to points and therefore own the points.  Whereas
	// removing a curve from a profile might be okay,
	// removing a point from a curve would be fatal.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbArc&
CDbArc::operator = ( const CDbArc& dbArc )
{
	CDbEntity::CommonCopy( dbArc );

	m_ps = dbArc.m_ps;
	m_pe = dbArc.m_pe;
	m_pc = dbArc.m_pc;

	m_dir = dbArc.m_dir;

	return (*this);
}

CReturn
CDbArc::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbArc* dbArc = new CDbArc( (*this) );

	(*dbEntity) = dbArc;

	return status;
}

CReturn
CDbArc::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbArc* dbArc = dynamic_cast<CDbArc*>( dbEntity );

	ASSERT( (dbArc != NULL) );

	CDbArc tmp( (*this) );
	(*this) = (*dbArc);
	(*dbArc) = tmp;

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
void
CDbArc::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			in_tolerance ) const
{
	if ( !m_ps || !m_pe || !m_pc || is_hidden() )
		return;  // Nothing to do.

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );
	if (  !UseOverrideColor() && !regen && !allocated )
		return;  // Nothing to do.

	dispent->Flush();

	DWORD	color;
	int		psi, pmi, pei, pci;

	CDbCurve::Describe2d( regen, io_tooltip, in_tolerance );

	const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
	const C3x4Matrix& xform = dbWorkplane->Transform();
	CDbTool*			tool = Tool();

	// Prepare for arc tabulation.
	// TODO:  If we move off the XY plane, this will be a problem
	C3dCoord	ps = StartPt(0);
	C3dCoord	pe = EndPt(0);
	C3dCoord	pc = CenterPt(0);

	CGeoArc		arc( ps, pe, pc, m_dir );

	C3dCoord	pm = arc.MidPt();

	psi = dispent->CoordAppend( ps );
	pmi = dispent->CoordAppend( pm );
	pei = dispent->CoordAppend( pe );
	pci = dispent->CoordAppend( pc );

	// -----------------------------------------------------
	//		Header and Dots...
	//
	dispent->CommandAppend( DCMD_SYSDOT, (DWORD) psi );
	dispent->CommandAppend( DCMD_SYSDOT, (DWORD) pmi );
	dispent->CommandAppend( DCMD_SYSDOT, (DWORD) pei );
	dispent->CommandAppend( DCMD_DOT, (DWORD) pci );

	color = ColorGet( DCOLOR_BLUE );

	if ( IsToolpath() )
	{
		if (Flags() & DBSHOWPATH)
		{
			dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
			color = DCOLOR_RED;
		}
		else
			dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_TOOL );
	}
	else
	{
		dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
	}

	dispent->CommandAppend( DCMD_COLOR, color );

	// -----------------------------------------------------
	// Tooltip and tool marker processing
	// TODO:  Move to base class
	//
	if (IsToolpath())
	{
		if (!ps.WithinTol( *io_tooltip, SMALL ))
		{
			if (tool)
			{
				// Draw tool marker, at offset...
				CGeoArc*	curve = (CGeoArc*)Curve();
				C2dUnitVec	tangent = curve->StartTan();
				delete curve;

				int cutside = IntGet( STR_CUTSIDE, 0 );
				bool partprof = (IntGet( STR_PARTPROF, 0 ) != 0);
				cutside *= dbWorkplane->ToolUp();

				CDbEntity::TransientOrientationSet();

				tool->Describe2d( regen, xform, StartPt(), tangent,
					partprof, cutside, in_tolerance, dispent );
			}
			else
			{
				dispent->CommandAppend( DCMD_TOOLMARK, (DWORD) psi );
			}
		}

		*io_tooltip = pe;
	}


	// -----------------------------------------------------
	//		Arc body...
	//
	// NOTE: The arc mid-pt must be in the display list for the following reasons:
	//
	// 1. Splitting the arc into two halves allows the display system
	//    to determine which have of the arc was selected.  This is
	//    important to trim/extend (for instance).
	//
	// 2. The rendering system can not properly draw 180+ degree arcs.
	//
	dispent->CommandAppend( DCMD_START, (DWORD) 0 );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) psi );
	dispent->CommandAppend( DCMD_ARCCTR, (DWORD) pci );

	if (m_dir == CW)
	{
		dispent->CommandAppend( DCMD_ARCCWTO, (DWORD) pmi );
		dispent->CommandAppend( DCMD_END, (DWORD) 0 );
		dispent->CommandAppend( DCMD_ARCCWTO, (DWORD) pei );
	}
	else // CCW
	{
		dispent->CommandAppend( DCMD_ARCCCTO, (DWORD) pmi );
		dispent->CommandAppend( DCMD_END, (DWORD) 0 );
		dispent->CommandAppend( DCMD_ARCCCTO, (DWORD) pei );
	}
}


C3dBox
CDbArc::Box( ID workplaneId ) const
{
	C3dBox box;

	if ( CDbEntity::IsDeleted() )
		return box;

	if (workplaneId == ~0)
	{
//		const C3x4Matrix& inverse = Workplane()->Inverse();

		//		box = Box( inverse );
		// Special case Box Top, high-speed arc extent
		CGeoArc*	geo_arc = Arc();
		CGeoArcList	quad_arc;
		geo_arc->QuadrantArcs( &quad_arc );
		delete geo_arc;

		for (int idx=0; idx<quad_arc.Count(); idx++)
		{
			CGeoArc*	arc = quad_arc[idx];
			box += arc->StartPt();
			box += arc->EndPt();
		}
		quad_arc.DestructiveFlush();
	}
	else
	{
		if (workplaneId == m_box_workid)
		{
			box = m_box_cache;
		}
		else
		{
			// Since not testing box in place, we explode the arc to account for
			// any obscene elliptical or degenerate case -- which could probably
			// be dealt with directly if we had better math support on the team.
			// TODO:  Test dest. plane for flatness to current plane, and do quad check instead of explode
			if (workplaneId > 0)
			{
				CDbWorkplane* targetWorkplane;
				CDbEntity::Find( workplaneId, (CDbEntity**) &targetWorkplane, DBWORKPLANE, DBWORKPLANE );
				ASSERT( (targetWorkplane != NULL) );
				const C3x4Matrix& inverse = targetWorkplane->Inverse();
				box = Box( inverse );
			}
			else
			{
				C3x4Matrix inverse;
				inverse.setUnit();
				box = Box( inverse );
			}

			((CDbArc*) this)->m_box_workid = workplaneId;
			((CDbArc*) this)->m_box_cache = box;
		}
	}

	return box;
}

C3dBox
CDbArc::Box( const C3x4Matrix& targetInverse ) const
{
	if ( CDbEntity::IsDeleted() )
		return C3dBox();

	C3x4Matrix theXform;
	C3dBox box;

	ID myWorkplaneId = CDbEntity::WorkplaneId();
	CDbWorkplane* myWorkplane = CDbEntity::Workplane();

//	const C3x4Matrix& xform = myWorkplane->Transform();
	C3x4Matrix xform;
	if (myWorkplane)
		xform = myWorkplane->Transform();
	else
		// TODO:  ASSERT fail in this case?  Currently, assumes unity so I can get past this
		xform.setUnit();

	// Create a one-step transformation to reduce computation.
	xform.TransformTo( targetInverse, &theXform );

	// We should never get this condition, but I do sometimes; from the failure of an 
	// arc due to uneven radii... not an app killer, so test for it here.
	if ( !m_ps || !m_pe || !m_pc )
		return box;		// Undefined arc ==> undefined box, rather than crashes.

	// The defining points of the arc should all have the same Z.
	C3dCoord ps = StartPt( myWorkplaneId );
	C3dCoord pe = EndPt( myWorkplaneId );
	C3dCoord pc = CenterPt( myWorkplaneId );

	// Prepare for arc tabulation.

	CGeoArc arc( ps, pe, pc, m_dir );

	// Tabulate!

	C3dCoordList* pt_list = arc.Explode( 1.e-3, &theXform );

	if (pt_list != NULL)
	{
		int	num = pt_list->Count();
		for (int step=1; step<num; step++)
			box += *(*pt_list)[step];

		pt_list->DestructiveFlush();

		delete pt_list;
	}

	return box;
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbArc::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbArc* dbArc = (CDbArc*)(*dbEntity);

	//
	// References and specific data ...
	//
	CDbEntity*	tmp;
	CDbPoint*	ps;
	CDbPoint*	pe;
	CDbPoint*	pc;

	if (!io_refmap->Lookup( m_ps, tmp) )
		status += m_ps->CopyTo( io_dest, io_refmap, &tmp );
	ps = dynamic_cast<CDbPoint*>(tmp);

	if (!io_refmap->Lookup( m_pe, tmp) )
		status += m_pe->CopyTo( io_dest, io_refmap, &tmp );
	pe = dynamic_cast<CDbPoint*>(tmp);

	if (!io_refmap->Lookup( m_pc, tmp) )
		status += m_pc->CopyTo( io_dest, io_refmap, &tmp );
	pc = dynamic_cast<CDbPoint*>(tmp);

	status += dbArc->Init( dbArc->Tool(), dbArc->Workplane(), ps, pe, pc, m_dir );

	return status;

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void
CDbArc::Transform( 
	const C3x4Matrix&	in_xform )
{
	if (DidAction())
		return;

	double	ix, jy;
	bool	mirror;

	if (DidAction())
		return;

	// Flip the arc direction as necessary.
	ix = in_xform.getI().X();
	jy = in_xform.getJ().Y();

	mirror = (((ix * jy) - SMALL) <= -1.);
	if (mirror)
		m_dir = -m_dir;

	// Apply the remaining transformation.
	CDbEntity::Transform( in_xform );

	m_ps->Transform( in_xform );
	m_pe->Transform( in_xform );
	m_pc->Transform( in_xform );
}
