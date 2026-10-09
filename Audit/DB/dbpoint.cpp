
#include "stdafx.h"
#include "StringConst.h"
#include "3dBox.h"
#include "3x4Matrix.h"
#include "3dCoord.h"
#include "GeoPoint.h"

#include "EntityDb.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbPoint.h"
#include "DbContainer.h"
#include "DbEntityVisitor.h"
#include "DisplayEntity.h"



////////////////////////////////////////////////////////////////////////

CDbPoint::CDbPoint( CEntityDb* db )

	: CDbEntity( db ),
	  m_pt()
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	int color = CDbEntity::Db()->Default().getColor( DCOLOR_RED );
	ColorSet( color );

	// By default, points are not 'user defined'.
	CDbEntity::SystemFlag( true );
}

CDbPoint::CDbPoint( const CDbPoint& dbPoint )

	: CDbEntity( dbPoint ),
	  m_pt( dbPoint.m_pt )
{
}

CDbPoint::~CDbPoint()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

C3dBox
CDbPoint::Box( ID workplaneId ) const
{
	if ( CDbEntity::IsDeleted() )
		return C3dBox();

	C3dCoord pt = Coord( workplaneId );
	CGeoPoint geoPt( pt );
	return geoPt.Box();
}

void
CDbPoint::Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& pt )
{
	ASSERT( workplane );
	ASSERT( tool );

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	CDbEntity::Tool( tool );
	CDbEntity::Workplane( workplane );

	m_pt = pt;
}

void
CDbPoint::Init( CDbTool* tool, CDbWorkplane* workplane, double x, double y, double z )
{
	C3dCoord pt( x, y, z );

	Init( tool, workplane, pt );
}

void
CDbPoint::Init( const C3dCoord& pt )
{
	Init( pt.X(), pt.Y(), pt.Z() );
}

void
CDbPoint::Init( double x, double y, double z )
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	m_pt.XYZ( x, y, z );
}

CGeoPoint
CDbPoint::Point() const
{
	CGeoPoint pt( m_pt );
	return pt;
}

C3dCoord
CDbPoint::Coord( ID workplaneId ) const
{
	C3dCoord result;

	if (workplaneId == ~0 || workplaneId == CDbEntity::WorkplaneId())
	{
		// No transformation required.
		result = m_pt;
	}
	else
	{
		// Transform to global coordinates.
		const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
		const C3x4Matrix& xform = dbWorkplane->Transform();
		xform.TransformTo( m_pt, &result );

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

void
CDbPoint::RefsTo( CDbEntityList* list ) const
{
	// Do nothing.  A point does not reference any entities.
}

void
CDbPoint::Subordinates( CDbEntityList* list ) const
{
	// Do nothing.  A point does not have any subordinate entities.
}

bool
CDbPoint::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

	return FALSE;
}

void
CDbPoint::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Divorce this entity from its owner.
	// Oh yuck.  When importing a PM4 file, points are in essence
	// converted to holes.  This is done by grouping points into
	// their respective features, creating the corresponding holes
	// and deleting the points.  As such, and in the current scheme
	// of things, a user-defined point can have references.  we
	// eliminate the reference by divorcing the point from its container,
	//
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( CDbEntity::Owner() );
	if (dbContainer != NULL)
		dbContainer->Disown( this );

	int refCnt = CDbEntity::RefCnt();

	// NOTE: In addition to indicating that a point has been deleted,
	// the DeleteFlag is used to to control the rendering of point
	// entities.  The majority of point entities are referenced by
	// curves and are never seen or directly manipulated by the user.
	// Such points are considered as 'system' entities.  A user is
	// allowed to create explicit point entities, but this will be
	// a rare occurence.  In either case, a given point can be referenced
	// by multiple entities (as a curve end point, for instance).
	// Also, it should be noted that points are at bottom of the 
	// entity hierarchy.  With all this said, deleting a point is
	// a complicated and error prone process.

	if ( CDbEntity::IsSystem() )
	{
		// System points can not be directly deleted by the user;
		// they must be deleted by a referencing entity.

		if (CDbEntity::RefCnt() == 1)
		{
			// We can not set the delete flag until such time
			// as the point will no longer have any references.
			// Doing otherwise will bugger the undo system and
			// the file writing system.

			CDbEntity::DeleteFlag( true );
		}
	}
	else if (refCnt == 0)
	{
		// We have an unreferenced user-defined point.

		CDbEntity::DeleteFlag( true );
	}

	CDbEntity::Record();



	if ( CDbEntity::IsSystem() )
	{
		RefDec();

		if (refCnt == 0)
		{
			CDbEntity::Workplane( NULL );
			CDbEntity::Tool( NULL );
		}
	}
	else
	{
		// A user-defined point that is referenced by another
		// entity can not be truely deleted until it is no longer
		// referenced by another entity.  We achieve this by
		// changing the point to a system-defined point, in
		// which case, the final entity left referencing the
		// point, will own the point.  This way, the point
		// will be deleted when the referencing entity is
		// deleted.

		// NOTE: System points are not displayed.
		CDbEntity::SystemFlag( true );

		if (refCnt <= 1)
		{
			CDbEntity::Workplane( NULL );
			CDbEntity::Tool( NULL );
		}
		else
		{
			CDbEntity::Workplane()->RefDec();
			CDbEntity::Tool()->RefDec();
		}
	}
}

void
CDbPoint::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbPoint::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  A point never references another entity.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbPoint&
CDbPoint::operator = ( const CDbPoint& dbPoint )
{
	CDbEntity::CommonCopy( dbPoint );

	m_pt = dbPoint.m_pt;

	return (*this);
}

CReturn
CDbPoint::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbPoint* dbPoint = new CDbPoint( (*this) );

	(*dbEntity) = dbPoint;

	return status;
}

CReturn
CDbPoint::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbPoint* dbPoint = dynamic_cast<CDbPoint*>( dbEntity );

	ASSERT( (dbPoint != NULL) );

	CDbPoint tmp = (*this);
	(*this) = (*dbPoint);
	(*dbPoint) = tmp;

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
CDbPoint::Describe2d( 
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

	CReturn	ret;
	DWORD	color;
	int		indx;

	const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
	// How *do* I get a point w/o a workplane?  Should not be possible
	C3x4Matrix xform;
	if (dbWorkplane)
		xform = dbWorkplane->Transform();
	else
		xform.setUnit();

	C3dCoord pt = Coord();
	C3dCoord tip;
	C3dCoord pt2;

	xform.TransformTo( pt, &tip );

	indx = dispent->CoordAppend( tip );
	dispent->CommandAppend( DCMD_DOT, (DWORD) indx );

	if (!IsSystem())
	{
		double	space = in_tolerance * 5;
		double	mark = in_tolerance * 10;

		C3dCoord	mpt;

		if (IsSelected())
		{
			dispent->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_SELECT );
		}
		else
		{
			color = ColorGet( DCOLOR_RED );
			dispent->CommandAppend( DCMD_COLOR, (DWORD) color );
			dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
		}


		// Only create point markers for user-points
		// TODO:  Drop into a loop, with offsets in an array?
		if (IsSelected())
		{
			mpt.X( pt.X() - space );
			mpt.Y( pt.Y() - space );
			mpt.Z( pt.Z() );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.X( mpt.X() - mark );
			mpt.Y( mpt.Y() - mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			mpt.X( pt.X() - space );
			mpt.Y( pt.Y() + space );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.X( mpt.X() - mark );
			mpt.Y( mpt.Y() + mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			mpt.X( pt.X() + space );
			mpt.Y( pt.Y() - space );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.X( mpt.X() + mark );
			mpt.Y( mpt.Y() - mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			mpt.X( pt.X() + space );
			mpt.Y( pt.Y() + space );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.X( mpt.X() + mark );
			mpt.Y( mpt.Y() + mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );
		}

		{
			mpt.X( pt.X() );
			mpt.Y( pt.Y() - space );
			mpt.Z( pt.Z() );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.Y( mpt.Y() - mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			mpt.Y( pt.Y() + space );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.Y( mpt.Y() + mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			mpt.X( pt.X() - space );
			mpt.Y( pt.Y() );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.X( mpt.X() - mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

			mpt.X( pt.X() + space );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

			mpt.X( mpt.X() + mark );
			xform.TransformTo( mpt, &pt2 );
			indx = dispent->CoordAppend( pt2 );
			dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );
		}
	}

	// -----------------------------------------------------
	// Tooltip and tool marker processing
	//
	if (IsToolpath())
	{
		CDbTool* tool = Tool();
		if (tool != NULL)
		{
			int cutside = 0;
			bool partprof = false;
			// int cutside = IntGet( STR_CUTSIDE, 0 );
			// int partprof = IntGet( STR_PARTPROF, 0 );
			cutside *= dbWorkplane->ToolUp();

			CDbEntity::TransientOrientationSet();

			tool->Describe2d( regen, xform, pt, C2dUnitVec( 1., 0. ),
				partprof, cutside, in_tolerance, dispent );
		}
		*io_tooltip = tip;
	}

#if REQUIRED
	CDbEntity::Describe2d( regen, &tip, in_tolerance );
#endif
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void
CDbPoint::Transform( 
	const C3x4Matrix&	in_xform )
{
	if (DidAction())
		return;

	CDbEntity::Transform( in_xform );

	in_xform.Transform( &m_pt );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbPoint::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn		status;
	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbPoint* dbPoint = (CDbPoint*)(*dbEntity);

	dbPoint->Init( dbPoint->Tool(), dbPoint->Workplane(), m_pt );

	return status;

}

