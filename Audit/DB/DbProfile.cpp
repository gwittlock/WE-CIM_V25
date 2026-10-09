
#include "stdafx.h"
#include "cmn_resource.h"
#include "MathConst.h"
#include "StringConst.h"

#include "GeoCurve.h"
#include "EntityDb.h"
#include "DbEntityVisitor.h"
#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbCurve.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbProfile.h"
#include "DisplayEntity.h"


////////////////////////////////////////////////////////////////////////

CDbProfile::CDbProfile( CEntityDb* db )
	: CDbContainer( db ),
	  m_list()
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	//(*pAttrib()) = CDbEntity::Db()->Default();

	// 2006.09.19 (PE) -- A white color is indicative of failure. That is
	// to say, either the color was not set as a default or the method that
	// created the profile did not set the color of the profile post facto.
	// color = CDbEntity::Db()->Default().getColor( DCOLOR_WHITE );
	// if (color > 0)
		// pAttrib()->setColor( color );

	m_associated = FALSE;
}

CDbProfile::CDbProfile( const CDbProfile& dbProfile )
	: CDbContainer( dbProfile ),
	  m_list()
{
	dbProfile.RefsTo( (CDbEntityList*) &m_list );
	m_associated = dbProfile.m_associated;
}

CDbProfile::~CDbProfile()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

CDbWorkplane* CDbProfile::Workplane() const
{
	if (m_list.Count() > 0)
		return m_list[0]->Workplane();

	return NULL;
}

CDbTool* CDbProfile::Tool() const
{
	CDbTool* dbTool = NULL;

	int count = m_list.Count();
	if (count > 0)
	{
		dbTool = m_list[0]->Tool();
		if ((count > 1) && dbTool->IsGapTool())
		{
			// Return a tool that is more representative.
			// NOTE: Gap-tooled entities are injected into a profile
			// by CNestingPart::do_simplify_geometry(). As such, we
			// are guaranteed not to have two adjacent gap-tooled entities.
			dbTool = m_list[1]->Tool();
		}
	}

	return dbTool;
}

int CDbProfile::Count() const
	{ return m_list.Count(); }

CDbEntity* CDbProfile::GetAt( int indx ) const
	{ return m_list[indx]; }

CDbEntity* CDbProfile::operator[]( int indx ) const
	{ return m_list[indx]; }

// NOTE: Must be careful when using ReplaceAt()!
CDbEntity* CDbProfile::ReplaceAt( int indx, CDbEntity* dbEntity )
{
	CDbEntity* prev = NULL;

	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
	if ((dbEntity != NULL) && (dbCurve == NULL))
	{
		CReturn status;
		status.Diagnostic( "CDbProfile::ReplaceAt()" );
	}
	else
	{
		// NOTE: We don't bother to check if the curve is
		// already in this list but we probably should.
		prev = m_list.GetAt( indx );
		if (prev != NULL)
		{
			prev = m_list.Replace( indx, dbCurve );  // 'prev' should still point to same
			if (dbCurve != NULL)
				dbCurve->Owner( this );
			prev->Orphan();
		}
	}

	return prev;
}

CReturn CDbProfile::Prepend( CDbEntity* dbEntity, bool copy )
{
	CReturn status;

	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

	if (dbCurve == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CDbProfile::Prepend()" );
	}
	else
	{
		if ( copy )
		{
			tDbEntityMap refmap;
			CDbCurve* dbCurveCopy;

			status = dbCurve->CopyTo( m_db, &refmap, (CDbEntity**) &dbCurveCopy );

			if ( status.IsOk() )
				status = Prepend( dbCurveCopy );
		}
		else
		{
			status = Prepend( dbCurve );
		}
	}

	return status;
}

CReturn CDbProfile::Append( CDbEntity* dbEntity, bool copy )
{
	CReturn status;

	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

	if (dbCurve == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CDbProfile::Append()" );
	}
	else
	{
		if ( copy )
		{
			tDbEntityMap refmap;
			CDbCurve* dbCurveCopy;

			status = dbCurve->CopyTo( m_db, &refmap, (CDbEntity**) &dbCurveCopy );

			if ( status.IsOk() )
				status = Append( dbCurveCopy );
		}
		else
		{
			status = Append( dbCurve );
		}
	}

	return status;
}

CReturn CDbProfile::Prepend( CDbCurve* dbCurve )
{
	CReturn status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	if (CDbEntity::Workplane() == NULL)
		CDbEntity::Workplane( dbCurve->Workplane() );

	dbCurve->Owner( this );
	m_list.Prepend( dbCurve );

	return status;
}

CReturn CDbProfile::Append( CDbCurve* dbCurve )
{
	CReturn status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	if (CDbEntity::Workplane() == NULL)
		CDbEntity::Workplane( dbCurve->Workplane() );

	dbCurve->Owner( this );
	m_list.Append( dbCurve );

	return status;
}

CReturn CDbProfile::Append( const CDbCurveList& curves )
{
	CReturn status;

	int count = curves.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbCurve* dbCurve = curves[indx];

		Append( dbCurve );
	}

	return status;
}

// NOTE: Mutually recursive with CDbEntity::Owner( CDbEntity* )
bool CDbProfile::Disown( CDbEntity* dbEntity )
{
	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

	if (dbCurve == NULL)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CDbProfile::Disown()" );
		return FALSE;
	}

	int indx = m_list.Find( dbCurve );

	if (indx < 0)
		return FALSE;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	m_list.Remove( indx );

	// Previously, CDbEntity::Owner( CDbEntity* ) was mutually recursive
	// with CDbContainer::Disown( CDbEntity* ).  Resolving ownership in
	// that manner is clearly problematic.  Though we could have used
	// entity flags to manage this (such as CDbEntity::TagSet()) that
	// method introduces its own set of problems.  In the end, I decided
	// to employ an encapsulated, non-recursive solution that leverages
	// the class friend construct.
	//
	//      dbEntity->Owner( NULL );
	//
	dbEntity->Orphan();

	return TRUE;
}

void CDbProfile::BenignFlush()
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	int count = m_list.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbCurve* dbCurve = m_list.Remove( 0 );
		dbCurve->Orphan();
	}
}

CReturn CDbProfile::DestructiveFlush()
{
	CReturn	status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	// A profile owns its curves. Therefore, when the
	// profile is FLUSHED, its curves must also be deleted.

	int count = m_list.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbCurve* dbCurve = m_list.Remove( 0 );

		// 2012.05.20 (PE) -- As of this date, is now possible for
		// a profile to have NULL entries. This arose because
		// CLead::LeadsCreate() calls prof->ReplaceAt( indx, NULL );
		if (dbCurve != NULL)
		{
			dbCurve->Orphan();
			dbCurve->Delete();
		}
	}

	return status;
}

void CDbProfile::Reverse()
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	CDbCurveList tmp;

	int count = Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbCurve* dbCurve = (CDbCurve*) m_list.Remove( 0 );

		dbCurve->Orphan();  // Must be done to keep refcnt synchronized.
		dbCurve->Reverse();

		tmp.Prepend( dbCurve );
	}

	Append( tmp );

	// Flip offset attribute direction, too... if it exists
	if ( CDbEntity::FlipCutside() )
	{
		int dir = IntGet( STR_CUTSIDE, 0 );
		if (dir != 0)
			IntSet( STR_CUTSIDE, -dir );
	}
}

// Force adjacent curves in a profile to share
// end points, thereby introducing associative
// behavior to curves in the profile.
CReturn CDbProfile::Associate( double tol )
{
	CReturn status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	int count = Count();

	CDbCurve* c1 = NULL;
	CDbCurve* c0 = m_list[0];

	for (int indx = 1; indx < count; ++indx)
	{
		c1 = m_list[indx];

		c0->Associate( c1, tol );

		c0 = c1;
	}

	c1 = m_list[0];

	if (c0 != c1)
		c0->Associate( c1, tol );

	m_associated = TRUE;

	return status;
}

CReturn CDbProfile::Disassociate()
{
	CReturn status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	int count = Count();

	CDbCurve* c1 = NULL;
	CDbCurve* c0 = m_list[0];

	for (int indx = 1; indx < count; ++indx)
	{
		c1 = m_list[indx];

		c0->Disassociate( c1 );

		c0 = c1;
	}

	c1 = m_list[0];

	if (c0 != c1)
		c0->Disassociate( c1 );

	m_associated = FALSE;

	return status;
}

bool CDbProfile::IsClosed() const
{
	return ( IsClosed( SMALL ) );
}

bool CDbProfile::IsClosed( double tol ) const
{
	int count = m_list.Count();

	if (count < 1)
		return FALSE;

	CDbCurve* cs = m_list[ 0 ];
	CDbCurve* ce = m_list[ count-1 ];

	C3dCoord ps = cs->StartPt( 0 );
	C3dCoord pe = ce->EndPt( 0 );

	return ( ps.WithinTol( pe, tol ) );
}

bool CDbProfile::CanClose() const
{
	bool can_close = false;

	int count = m_list.Count();
	if (count > 0)
	{
		CDbCurve* curveA = m_list[count-1];
		CDbCurve* curveB = m_list[0];

		if ((curveA != curveB) ||
			((curveA == curveB) && (curveA->Type() != DBLINE)))
		{
			C3dCoord ptA = curveA->EndPt();
			C3dCoord ptB = curveB->StartPt();

			can_close = ( !ptA.WithinTolXY( ptB, SMALL ) );
		}
	}

	return can_close;
}

void CDbProfile::RefsTo( CDbEntityList* list ) const
{
	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		list->Append( m_list[indx] );
	}
}

void CDbProfile::Subordinates( CDbEntityList* list ) const
{
	RefsTo( list );
}

// Get the atomic entities.
void CDbProfile::Flatten( CDbEntityList* entities ) const
{
	RefsTo( entities );
}

int CDbProfile::Position( const CDbCurve* curve ) const
{
	int indx = m_list.Find( (CDbCurve*) curve );
	return indx;
}

int CDbProfile::Position( const CDbEntity* refEntity ) const
{
	return Position( (CDbCurve*)refEntity );
}

int CDbProfile::InsertBefore( CDbEntity* refEntity, CDbEntity* newEntity )
{
	CDbCurve*	refCurve;
	int			indx;
	
	refCurve = dynamic_cast<CDbCurve*>( refEntity );

	if (refCurve == NULL)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CDbProfile::InsertBefore()" );
		indx = -1;
	}
	else
	{
		indx = Position( refCurve );

		if (indx >= 0)
			indx = InsertBefore( indx, newEntity );
	}

	return indx;
}

int CDbProfile::InsertBefore( int indx, CDbEntity* newEntity )
{
	CDbCurve* newCurve = dynamic_cast<CDbCurve*>( newEntity );

	if (newCurve == NULL)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CDbProfile::InsertBefore()" );
		indx = -1;
	}
	else
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.InsertBefore( indx, newCurve );

		newCurve->Owner( this );
	}

	return indx;
}

int CDbProfile::InsertAfter( CDbEntity* refEntity, CDbEntity* newEntity )
{
	CDbCurve*	refCurve;
	int			indx;
	
	refCurve = dynamic_cast<CDbCurve*>( refEntity );

	if (refCurve == NULL)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CDbProfile::InsertAfter()" );
		indx = -1;
	}
	else
	{
		indx = Position( refCurve );

		if (indx >= 0)
			indx = InsertAfter( indx, newEntity );
	}

	return indx;
}

int CDbProfile::InsertAfter( int indx, CDbEntity* newEntity )
{
	CDbCurve* newCurve = dynamic_cast<CDbCurve*>( newEntity );

	if (newCurve == NULL)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CDbProfile::InsertAfter()" );
		indx = -1;
	}
	else
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.InsertAfter( indx, newCurve );

		newCurve->Owner( this );
	}

	return indx;
}

int CDbProfile::InsertBefore( CDbCurve* refCurve, CDbCurve* newCurve )
{
	int indx = Position( refCurve );

	if (indx >= 0)
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.InsertBefore( indx, newCurve );

		newCurve->Owner( this );
	}

	return indx;
}

int CDbProfile::InsertAfter( CDbCurve* refCurve, CDbCurve* newCurve )
{
	int indx = Position( refCurve );

	if (indx >= 0)
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.InsertAfter( indx, newCurve );

		newCurve->Owner( this );
	}

	return indx;
}

bool CDbProfile::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::IsDeleted() )
		return FALSE;

	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

	CDbCurve* dbCurve = (CDbCurve*) dynamic_cast<const CDbCurve*>(refdEntity);

	if (dbCurve == NULL)
		return FALSE;

	int indx = m_list.Find( dbCurve );

	return (indx >= 0);
}

void CDbProfile::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	// A profile owns its curves. Therefore, when the
	// profile is deleted, its curves must also be deleted.

	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbCurve* dbCurve = m_list[0];
		dbCurve->Delete();
	}

	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( CDbEntity::Owner() );
	if (dbContainer != NULL)
		dbContainer->Disown( this );

	CDbEntity::RemoveRefs();
}

void CDbProfile::Accept( CDbEntityVisitor* visitor )
{
	int		count, indx;

	visitor->Visit( this );

	indx = 0;
	count = m_list.Count();

	while (indx < count)
	{
		m_list[indx]->Accept( visitor );
		if (count == m_list.Count())
			++indx;
		else
			count = m_list.Count();  // the list changed
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void CDbProfile::RemoveRef( CDbEntity* dbEntity )
{
	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

	if (dbCurve == NULL)
		return;

	int indx = m_list.Find( dbCurve );

	if (indx < 0)
		return;

	m_list.Remove( indx );

	dbEntity->RefDec();
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbProfile& CDbProfile::operator = ( const CDbProfile& dbProfile )
{
	CDbEntity::CommonCopy( dbProfile );

	m_list.BenignFlush();
	dbProfile.RefsTo( (CDbEntityList*) &m_list );

	m_associated = dbProfile.m_associated;

	return (*this);
}

CReturn CDbProfile::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbProfile* dbProfile = new CDbProfile( (*this) );

	(*dbEntity) = dbProfile;

	return status;
}

CReturn CDbProfile::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

	ASSERT( (dbProfile != NULL) );

	CDbProfile tmp( (*this) );
	(*this) = (*dbProfile);
	(*dbProfile) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool CDbProfile::IsUsableCurve( const CDbEntity& dbEntity )
{
	const CDbCurve* dbCurve = dynamic_cast<const CDbCurve*>( &dbEntity );

	if (dbCurve == NULL)
		return FALSE;  // The given entity is not a curve.

	if ( dbCurve->IsDeleted() )
		return FALSE;

	CDbEntity* owner = dbCurve->Owner();
	if (owner != NULL && owner->Type() == DBPROFILE)
	{
		// An entity can have only one owner.  As
		// this entity is already owned, it can
		// not be used in another profile.
		return FALSE;
	}

	return TRUE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn CDbProfile::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbProfile* dbProfile = (CDbProfile*)(*dbEntity);

	CDbEntity*	refto;
	int count = m_list.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		if (!io_refmap->Lookup( m_list[indx], refto ))
			m_list[indx]->CopyTo( io_dest, io_refmap, &refto );

		status += dbProfile->Append( (CDbCurve*)refto );
	}

	dbProfile->m_associated = m_associated;

	return status;

}

C3dBox CDbProfile::Box( ID workplaneId ) const
{
	C3dBox	world;

	for (int idx=0; idx<m_list.Count(); idx++)
	{
		const C3dBox& box = m_list[idx]->Box( workplaneId );
		if (box.IsDefined())
			world += box;
	}

	return world;
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
void CDbProfile::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			in_tolerance ) const
{
	const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
	if (dbWorkplane == NULL)
		return;  // early exit.

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );
	if (  !UseOverrideColor() && !regen && !allocated )
		return;  // Nothing to do.

	dispent->Flush();

	int			idx;
	int			num;
	C3dCoord	pt;
	C3dCoord	pt2;
	C3dCoord	wpt2;
	int			indx;

	double	space = in_tolerance * 2;
	double	mark = in_tolerance * 4;

	//CDbWorkplane* dbWorkplane;
	//((CDbProfile*) this)->Db()->Find( "Top", (CDbEntity**) &dbWorkplane );

	const C3x4Matrix& xform = dbWorkplane->Transform();

	dispent->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_SELECT );
	dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
	dispent->CommandAppend( DCMD_META, (DWORD) META_SYS );

	num = Count()-1;
	if (IsClosed())
		num--;

	CDbCurve* curve = NULL;
	for (idx=0; idx<=num; idx++)
	{
		curve = (CDbCurve*)( m_list[idx] );

		if (!idx)
		{
			CGeoCurve*	geo = curve->Curve();
			C2dUnitVec	vec;

			if (geo)
			{
				vec = geo->StartTan();
				delete geo;
			}
			else
				vec = C2dUnitVec( 1, 0 );

			pt = curve->StartPt();
			pt2.Z( pt.Z() );

			// Do start-point triangle
			pt2.X( pt.X() - vec.Y()*mark );
			pt2.Y( pt.Y() + vec.X()*mark );
			xform.TransformTo( pt2, &wpt2 );
			indx = dispent->CoordAppend( wpt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			pt2.X( pt.X() + vec.X()*2*mark );
			pt2.Y( pt.Y() + vec.Y()*2*mark );
			xform.TransformTo( pt2, &wpt2 );
			indx = dispent->CoordAppend( wpt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			pt2.X( pt.X() + vec.Y()*mark );
			pt2.Y( pt.Y() - vec.X()*mark );
			xform.TransformTo( pt2, &wpt2 );
			indx = dispent->CoordAppend( wpt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			pt2.X( pt.X() - vec.Y()*mark );
			pt2.Y( pt.Y() + vec.X()*mark );
			xform.TransformTo( pt2, &wpt2 );
			indx = dispent->CoordAppend( wpt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );
		}

		pt = curve->EndPt();

		C3dCoord	xpt;
		xform.TransformTo( pt, &xpt );

		pt2.X( xpt.Z() );
		if ( IsAssociated())
		{
			// Associated; put a dot (square)
			pt2.X( xpt.X() - space );
			pt2.Y( xpt.Y() - space );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			pt2.X( xpt.X() + space );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			pt2.Y( xpt.Y() + space );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );


			pt2.X( xpt.X() - space );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			pt2.Y( xpt.Y() - space );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );
		}
		else
		{
			// Non-associated; put a diamond
			pt2.X( xpt.X() );
			pt2.Y( xpt.Y() - mark );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			pt2.X( xpt.X() + mark );
			pt2.Y( xpt.Y() );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			pt2.X( xpt.X() );
			pt2.Y( xpt.Y() + mark );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			pt2.X( xpt.X() - mark );
			pt2.Y( xpt.Y() );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			pt2.X( xpt.X() );
			pt2.Y( xpt.Y() - mark );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );
		}
	}

	dispent->CommandAppend( DCMD_META, (DWORD) META_NONE );

	int count = m_list.Count();
	if (count)
	{
		CDbCurve* ce = m_list[ count-1 ];
		C3dCoord pe = ce->EndPt( 0 );

		CDbEntity::Describe2d( regen, &pe, in_tolerance );
	}
}

void CDbProfile::Transform( const C3x4Matrix& in_xform )
{
	if (DidAction())
		return;

	CDbEntity::Transform( in_xform );

	// NOTE: Curve direction is reversed during mirroring operations.
	int count = Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbCurve* dbCurve = m_list[ indx ];
		dbCurve->Transform( in_xform );
	}
}

// 2012.08.25 (PE) -- I'm not real keen on making IsLeadHull() a
// member of CDbProfile but it sure is convenient and similar
// precedent has been set by members of CDbFeature.
bool CDbProfile::IsLeadHull() const
{
	// Values for STR_LEAD_HULL (-1) applied to outside profile / (1) applied to inside profile.
	return (IntGet( STR_LEAD_HULL, IUNDEFINED ) != IUNDEFINED);
}
