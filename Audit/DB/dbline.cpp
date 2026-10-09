
#include "stdafx.h"
#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "3x4Matrix.h"
#include "DbWorkplane.h"
#include "EntityDb.h"
#include "GeoLine.h"
#include "DbPoint.h"
#include "DbLine.h"
#include "DbTool.h"
#include "DbContainer.h"
#include "DbSequence.h"
#include "DbEntityVisitor.h"
#include "DisplayEntity.h"


////////////////////////////////////////////////////////////////////////

CDbLine::CDbLine( CEntityDb* db )

	: CDbCurve( db ),
	  m_ps( nullptr ),
	  m_pe( nullptr )
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	(*pAttrib()) = CDbEntity::Db()->Default();
}

CDbLine::CDbLine( const CDbLine& dbLine )

	: CDbCurve( dbLine ),
	  m_ps( dbLine.m_ps ),
	  m_pe( dbLine.m_pe )
{
}

CDbLine::~CDbLine()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

C3dBox
CDbLine::Box( ID workplaneId ) const
{
	if ( CDbEntity::IsDeleted() )
		return C3dBox();

	C3dBox box;
	if (workplaneId == m_box_workid)
	{
		box = m_box_cache;
	}
	else
	{
		if (m_ps && m_pe)
		{
			C3dCoord ps = m_ps->Coord( workplaneId );
			C3dCoord pe = m_pe->Coord( workplaneId );

			box = C3dBox( ps, pe );
		}

		((CDbLine*) this)->m_box_workid = workplaneId;
		((CDbLine*) this)->m_box_cache = box;
	}

	return box;
}

CReturn
CDbLine::Init( CDbTool* tool, CDbWorkplane* workplane, const CGeoLine& line )
{
	CReturn status = Init( tool, workplane, line.StartPt(), line.EndPt() );
	return status;
}

CReturn
CDbLine::Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& ps, const C3dCoord& pe )
{
	CReturn status = Validate( ps, pe );

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

		Update();
	}

	return status;
}

CReturn
CDbLine::Init( const C3dCoord& ps, const C3dCoord& pe )
{
	CReturn status = Validate( ps, pe );

	if ( status.IsOk() )
	{
		CDbEntity::Record();

		if ( !IsAssociated() )
			status = Associate();
	}

	if ( status.IsOk() )
	{
		m_ps->Init( Tool(), Workplane(), ps );
		m_pe->Init( Tool(), Workplane(), pe );

		Update();
	}

	return status;
}

CReturn
CDbLine::Init( CDbTool* tool, CDbWorkplane* workplane, CDbPoint* start, CDbPoint* end )
{
	ID wpId = workplane->Id();

	const C3dCoord& ps = start->Coord( wpId );
	const C3dCoord& pe = end->Coord( wpId );

	CReturn status = Validate( ps, pe );

	if ( status.IsOk() )
	{
		CDbEntity::Record();

		CDbEntity::Tool( tool );
		CDbEntity::Workplane( workplane );

		if (m_ps != nullptr)
			m_ps->RefDec();

		if (m_pe != nullptr)
			m_pe->RefDec();

		m_ps = start;
		m_pe = end;

		m_ps->RefInc();
		m_pe->RefInc();

		Update();
	}

	return status;
}

CReturn
CDbLine::Init( ID start, ID end )
{
	CDbPoint* dbPs;
	CDbPoint* dbPe;

	CReturn status = Find( start, (CDbEntity**) &dbPs, DBPOINT, DBPOINT );

	if ( status.IsOk() )
		status = Find( end, (CDbEntity**) &dbPe, DBPOINT, DBPOINT );

	if ( status.IsOk() )
	{
		status = Init( Tool(), Workplane(), dbPs, dbPe );
	}

	return status;
}

CGeoCurve*
CDbLine::Curve( ID workplaneId ) const
{
	return Line( workplaneId );
}

CGeoLine*
CDbLine::Line( ID workplaneId ) const
{
	if (workplaneId == ~0)
		workplaneId = CDbEntity::WorkplaneId();

	C3dCoord ps = m_ps->Coord( workplaneId );
	C3dCoord pe = m_pe->Coord( workplaneId );

	CGeoLine* line = new CGeoLine( ps, pe );

	return line;
}

C3dCoord
CDbLine::StartPt( ID workplaneId ) const
{
	return m_ps->Coord( workplaneId );
}

C3dCoord
CDbLine::EndPt( ID workplaneId ) const
{
	return m_pe->Coord( workplaneId );
}

CDbPoint*
CDbLine::DbStartPt() const
{
	return m_ps;
}

void
CDbLine::DbStartPt( CDbPoint* pt )
{
	CDbEntity::Record();

	m_ps->Delete();
	m_ps = pt;
	pt->RefInc();

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

CDbPoint*
CDbLine::DbEndPt() const
{
	return m_pe;
}

void
CDbLine::DbEndPt( CDbPoint* pt )
{
	CDbEntity::Record();

	m_pe->Delete();
	m_pe = pt;
	pt->RefInc();

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

void
CDbLine::Reverse()
{
	CDbPoint* tmp = m_ps;
	m_ps = m_pe;
	m_pe = tmp;

	// Flip offset attribute direction, too... if it exists
	if ( CDbEntity::FlipCutside() )
	{
		int dir = IntGet( STR_CUTSIDE, 0 );
		if (dir != 0)
			IntSet( STR_CUTSIDE, -dir );
	}
}

void
CDbLine::RefsTo( CDbEntityList* list ) const
{
	if (m_ps != nullptr)
		list->Append( m_ps );

	if (m_pe != nullptr)
		list->Append( m_pe );
}

void
CDbLine::Subordinates( CDbEntityList* list ) const
{
	list->Append( m_ps );
	list->Append( m_pe );
}

CReturn
CDbLine::Validate( const C3dCoord& start, const C3dCoord& end ) const
{
	CReturn status;

	// C2dVec vec = end - start;  // Was this 2d for a reason?
	C3dVec vec = end - start;

	double len = vec.Length();

	if (len < SMALL)
		status.Internal( IDS_ZERO_LENGTH_LINE, Id() );

	return status;
}

bool
CDbLine::IsAssociated()
{
	// TODO: Is it suffifient to check only one member?
	return (m_ps != nullptr && m_pe != nullptr);
}

CReturn
CDbLine::Associate()
{
	CReturn status = Create( DBPOINT, (CDbEntity**) &m_ps );

	if ( status.IsOk() )
		status = Create( DBPOINT, (CDbEntity**) &m_pe );

	if ( status.IsOk() )
	{
		m_ps->RefInc();
		m_pe->RefInc();

		m_ps->SystemFlag( true );
		m_pe->SystemFlag( true );
	}
	else
	{
		// Clean-up after ourselves.
		CDbEntity::Delete( (CDbEntity**) &m_ps );
		CDbEntity::Delete( (CDbEntity**) &m_pe );
	}

	return status;
}

void
CDbLine::Update()
{
	CDbEntity::CreateFlag( false );

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

bool
CDbLine::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::IsDeleted() )
		return FALSE;

	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

//	ASSERT( (m_ps != nullptr && m_pe != nullptr) );

	if ( m_ps
		&& (m_ps == refdEntity) )
		return TRUE;

	if ( m_pe
		&& (m_pe == refdEntity) )
		return TRUE;

	return FALSE;
}

void
CDbLine::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	// Divorce this entity from its owner.
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( CDbEntity::Owner() );
	if (dbContainer != nullptr)
		dbContainer->Disown( this );

	CDbSequence* dbSequence = CDbEntity::Sequence();
	if (dbSequence != nullptr)
		this->Sequence( nullptr );

	// Sever the ties between all other entities and this entity.
	CDbEntity::RemoveRefs();

	// A system point is like a 'hot potato', the last
	// line referencing a point owns the point.  If the
	// last line is deleted, so are its end points.
	if (m_ps)
		m_ps->Delete();
	if (m_pe)
		m_pe->Delete();

	m_ps = nullptr;
	m_pe = nullptr;

	CDbEntity::Workplane( nullptr );
	CDbEntity::Tool( nullptr );
}

void
CDbLine::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbLine::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  Curves live by their association
	// to points and therefore own the points.  Whereas
	// removing a curve from a profile might be okay,
	// removing a point from a curve would be fatal.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbLine&
CDbLine::operator = ( const CDbLine& dbLine )
{
	CDbEntity::CommonCopy( dbLine );

	m_ps = dbLine.m_ps;
	m_pe = dbLine.m_pe;

	return (*this);
}

CReturn
CDbLine::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbLine* dbLine = new CDbLine( (*this) );

	(*dbEntity) = dbLine;

	return status;
}

CReturn
CDbLine::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbLine* dbLine = dynamic_cast<CDbLine*>( dbEntity );

	ASSERT( (dbLine != nullptr) );

	CDbLine tmp( (*this) );
	(*this) = (*dbLine);
	(*dbLine) = tmp;

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
CDbLine::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			in_tolerance ) const
{
	if ( !m_ps || !m_pe || is_hidden() )
		return;  // Nothing to do.

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );
	if ( !UseOverrideColor() & !regen && !allocated )
		return;  // Nothing to do.

	dispent->Flush();

	DWORD	color;
	int		psi, pmi, pei;

	CDbCurve::Describe2d( regen, io_tooltip, in_tolerance );

	const CDbWorkplane*	dbWorkplane = CDbEntity::Workplane();
	const C3x4Matrix&	xform = dbWorkplane->Transform();
	CDbTool*			tool = Tool();

	// Get the end points in the global coordinate system.
	C3dCoord st = StartPt(0);
	C3dCoord en = EndPt(0);
	C3dCoord ctr;

	ctr.X( (st.X() + en.X() ) / 2 );
	ctr.Y( (st.Y() + en.Y() ) / 2 );
	ctr.Z( (st.Z() + en.Z() ) / 2 );

	psi = dispent->CoordAppend( st );
	pmi = dispent->CoordAppend( ctr );
	pei = dispent->CoordAppend( en );

	// -----------------------------------------------------
	//		Header...
	//

	dispent->CommandAppend( DCMD_SYSDOT, (DWORD) psi );
	dispent->CommandAppend( DCMD_SYSDOT, (DWORD) pmi );
	dispent->CommandAppend( DCMD_SYSDOT, (DWORD) pei );


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
	//
	if (IsToolpath())
	{
		if (!st.WithinTol( *io_tooltip, SMALL ))
		{
			if (tool)
			{
				// Draw tool marker, at offset...
				CGeoLine*	curve = (CGeoLine*)Curve();
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

		*io_tooltip = en;
	}


	// -----------------------------------------------------
	//		Line body...
	//
	// NOTE: The line mid-pt must be in the display list for the following reason:
	//
	// 1. Splitting the arc into two halves allows the display system
	//    to determine which have of the arc was selected.  This is
	//    important to trim/extend (for instance).
	//
	dispent->CommandAppend( DCMD_START, (DWORD) 0 );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) psi );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) pmi );
	dispent->CommandAppend( DCMD_END, (DWORD) 0 );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) pei );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void CDbLine::Transform( const C3x4Matrix& in_xform )
{
	if (DidAction() || IsDeleted())
		return;

	CDbEntity::Transform( in_xform );

	m_ps->Transform( in_xform );
	m_pe->Transform( in_xform );
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn CDbLine::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	// Generic Copy... sets up the database and maps...
	CReturn status = CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbLine* dbLine = (CDbLine*)(*dbEntity);

	// References and specific data ...
	CDbEntity* tmp = nullptr;
	if (!io_refmap->Lookup( m_ps, tmp) )
		status += m_ps->CopyTo( io_dest, io_refmap, &tmp );

	CDbPoint* ps = dynamic_cast<CDbPoint*>(tmp);

	tmp = nullptr;
	if (!io_refmap->Lookup( m_pe, tmp) )
		status += m_pe->CopyTo( io_dest, io_refmap, &tmp );

	CDbPoint* pe = dynamic_cast<CDbPoint*>(tmp);

	status += dbLine->Init( dbLine->Tool(), dbLine->Workplane(), ps, pe );

	return status;
}



