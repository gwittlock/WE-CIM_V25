
#include "stdafx.h"
#include "CommonFlags.h"
#include "MathConst.h"
#include "StringConst.h"
#include "3x4Matrix.h"
#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "EntityDb.h"
#include "EntityCopier.h"
#include "DbWorkplane.h"
#include "DbPoint.h"
#include "DbCurve.h"
#include "DbLine.h"
#include "DbArc.h"

#include "DbProfile.h"
#include "DbFeature.h"
#include "Solution.h"
#include "DisplayEntity.h"

char EntityType( const CDbCurve* dbCurve );

////////////////////////////////////////////////////////////////////////

CDbCurve::CDbCurve()
	: CDbEntity()
{
}

CDbCurve::CDbCurve( CEntityDb* db )

	: CDbEntity( db )
{
}

CDbCurve::CDbCurve( const CDbCurve& dbCurve )

	: CDbEntity( dbCurve )
{
}

CDbCurve::~CDbCurve()
{
}

// Associate the end point of this curve
// with the start point of the given curve.
void
CDbCurve::Associate( CDbCurve* dbCurve, double tol )
{
	if (DbEndPt() == dbCurve->DbStartPt())
		return;  // Already associated.

	CReturn			status;
	CDbWorkplane*	dbWork;
	CDbTool*		dbTool;
	CDbLine*		dbLine;
	CDbArc*			dbArc;
	CGeoArc*		geoArc;
	C3dCoord		ps, pe, pc;
	C3dCoord		pmWorld;
	C3dVec			vec;
	double			len, rad, ang;
	int				dir;
	char			typeA, typeB;


	// NOTE: The mid-point between the end points of the adjacent
	// curves is calculated in world coordinates.  The mid-point
	// is then transformed back to the local coordinate system
	// of each curve.

	pe = EndPt(0);
	ps = dbCurve->StartPt(0);

	vec = pe - ps;
	len = vec.Length();

	if (len <= tol)
	{
		// The end points of the adjacent curves are within tolerance.

		if (len > SMALL)
		{
			dbTool = Tool();

			// 10/24/2002 -- Textron discovered a failure (using SmartCAM data)
			// where a lead-in arc joined a full circle.  In that case, the
			// start point of the circle was modified.  In turn, this violated
			// the circle because the start and end points were no longer
			// coincident.  And since the end point of the circle may be
			// 'attached' to another entity, we can not simple change the
			// end point.
			//
			// In brief, full circles are now unaffected but the treatment of
			// curves adjacent to full circles has changed slightly.  In this
			// case, the start/end point of the adjacent curve is made to be
			// coincident with the start/end point of the circle.
			//
			typeA = EntityType( this );
			typeB = EntityType( dbCurve );

			// Get the mid-point.
			pmWorld = ps + (vec * 0.5);

			if (typeA == 'L' || typeA == 'A')
			{
				dbWork = Workplane();
				ps = StartPt();

				if (typeB == 'C')
					pe = dbCurve->StartPt();
				else
					dbWork->Inverse().TransformTo( pmWorld, &pe );

				dbArc = dynamic_cast<CDbArc*>( this );
				dbLine = dynamic_cast<CDbLine*>( this );

				// Adjust the end point of this curve.
				if (dbArc != NULL)
				{
					geoArc = dbArc->Arc();
					rad = geoArc->Radius();
					dir = geoArc->Dir();
					ang = geoArc->IncludedAngle();
					if (CSolution::ArcCenter( ps, pe, rad, dir, ang, &pc ) > 0)
						status = dbArc->Init( dbTool, dbWork, ps, pe, pc, dir );

					delete geoArc;
				}
				else if (dbLine != NULL)
				{
					status = dbLine->Init( dbTool, dbWork, ps, pe );
				}
			}

			if (typeB == 'L' || typeB == 'A')
			{
				// Adjust the start point of the adjacent curve.
				dbWork = dbCurve->Workplane();
				pe = dbCurve->EndPt();

				if (typeA == 'C')
					ps = EndPt();
				else
					dbWork->Inverse().TransformTo( pmWorld, &ps );

				dbArc = dynamic_cast<CDbArc*>( dbCurve );
				dbLine = dynamic_cast<CDbLine*>( dbCurve );

				if (dbArc != NULL)
				{
					geoArc = dbArc->Arc();
					rad = geoArc->Radius();
					dir = geoArc->Dir();
					ang = geoArc->IncludedAngle();
					if (CSolution::ArcCenter( ps, pe, rad, dir, ang, &pc ) > 0)
						status = dbArc->Init( dbTool, dbWork, ps, pe, pc, dir );

					delete geoArc;
				}
				else if (dbLine != NULL)
				{
					status = dbLine->Init( dbTool, dbWork, ps, pe );
				}
			}
		}

		// Make the end point of this curve and the start point
		// of the adjacent curve share a common end point.
		dbCurve->DbStartPt( DbEndPt() );
	}

	m_box_workid = -2;
	m_box_cache.Invalidate();
}

// Disassociate the end point of this curve
// from the start point of the given curve.
CDbPoint*
CDbCurve::Disassociate( CDbCurve* dbCurve )
{
	CReturn		status;
	CDbPoint*	dbEndPt;

	dbEndPt = DbEndPt();
	if (dbEndPt != dbCurve->DbStartPt())
		return NULL;  // Already disassociated.

	CEntityCopier	copier;
	CDbPoint*		dbNewPt;

	copier.Init( Db(), FLAG_NONE );
	dbNewPt = (CDbPoint*) copier.PointCopy( dbEndPt );

	if (dbNewPt != NULL)
	{
		// Initially, profile points are ...
		//   p0 --- p1 --- p2 --- p3
		//
		// After disassociating ...
		//   p0 --- p1 p4 --- p2 p5 --- p3
		//
		// After re-associating
		//   p0 --- p1 --- p2 --- p3
		dbCurve->DbStartPt( dbNewPt );
	}

	return dbNewPt;
}

CDbPoint*
CDbCurve::ConditionalDisassociate( CDbPoint* pt )
{
	CEntityCopier	copier;
	CDbPoint*		dbNewPt;

	dbNewPt = NULL;

	if (pt == DbStartPt())
	{
		if (Type() == DBARC)
		{
			copier.Init( Db(), FLAG_NONE );
			dbNewPt = (CDbPoint*) copier.PointCopy( pt );
			DbStartPt( dbNewPt );
		}
	}
	else if (pt == DbEndPt())
	{
		if (Type() == DBARC)
		{
			copier.Init( Db(), FLAG_NONE );
			dbNewPt = (CDbPoint*) copier.PointCopy( pt );
			DbEndPt( dbNewPt );
		}
	}

	return dbNewPt;
}

CDbCurve*
CDbCurve::Split( const C3dCoord& pt, double gap )
{
	CReturn status;
	CDbCurve* trail = NULL;

	C3dCoord closestPt;
	C3dCoord splitA;
	C3dCoord splitB;
	double u, dist;

	CGeoCurve* curve = Curve();

	dist = curve->PointClosest( pt, &closestPt, &u );

	// BugID: 613 -- Splitting under clamps creates zero-length
	// entities (which causes crash on opening of mm2).  This
	// method has been slightly reorganized, defering the copy
	// and split operations until we know we will not generate
	// zero-length entities.
	if (u > VECTOR_SMALL && u < (1.0 - VECTOR_SMALL))
	{
		// The closest point is on the bounded curve.

		tDbEntityMap	refmap;
		CDbPoint*		ps;
		CDbPoint*		pe;

		GapPointsCalc( closestPt, gap, &splitA, &splitB );

		ps = DbStartPt();
		if ( ps->Coord().WithinTolXY( splitA, SMALL ) )
			return NULL;

		pe = DbEndPt();
		if ( pe->Coord().WithinTolXY( splitB, SMALL ) )
			return NULL;

		status = CopyTo( Db(), &refmap, (CDbEntity**) &trail );
		if ( status.IsOk() )
		{
			CDbContainer* dbContainer = dynamic_cast<CDbContainer*>(CDbEntity::Owner() );
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbContainer );

			bool associated = ((dbProfile == NULL) ? FALSE : dbProfile->IsAssociated());

			if ( associated )
			{
				// Breaking the association between curves and their common
				// end points allows us to modify any single curve entity
				// without affecting its neighbors.  Though this method of
				// breaking the association is perhaps less efficient than
				// breaking it locally, it is much less complicated.
				dbProfile->Disassociate();
			}

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// BugID: 264 -- Undo of AutoLeads mangled database
				CDbEntity::Record();
				CDbEntity::ModifyFlag( true );
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			// Trim the end of the leading curve back to the split point.
			pe = DbEndPt();
			pe->Init( splitA );

			// Trim the start of the trailing curve back to the split point.
			ps = trail->DbStartPt();
			ps->Init( splitB );

			// Share tooling
			trail->Tool( Tool() );

			if (dbContainer != NULL)
			{
				dbContainer->InsertAfter( this, trail );

				if ( associated )
				{
					// We must re-establish the association between adjacent
					// curves and their common end points.  By virtue of the
					// previous association, the common end points will already
					// be in a tolerance of SMALL of each other.
					dbProfile->Associate( SMALL );
				}
			}
		}
	}
	else
	{
		status.setStatus( STATUS_ERROR );  // Intentionally no message.
	}

	delete curve;

	m_box_workid = -2;
	m_box_cache.Invalidate();

	return trail;
}

void
CDbCurve::GapPointsCalc(
					const C3dCoord&	closestPt,
					double			gap,
					C3dCoord*		splitA,
					C3dCoord*		splitB )
{
	bool okay = FALSE;  // Assume failure.

	gap *= 0.5;

	if (gap > SMALL)
	{
		CGeoCurve* geoCurve = Curve();
		CGeoLine* geoLine = dynamic_cast<CGeoLine*>( geoCurve );
		CGeoArc* geoArc = dynamic_cast<CGeoArc*>( geoCurve );

		const C3dCoord& ps = geoCurve->StartPt();
		const C3dCoord& pe = geoCurve->EndPt();

		if (geoLine != NULL)
		{
			C2dUnitVec tan = geoLine->StartTan();

			(*splitA) = closestPt - (tan * gap);
			(*splitB) = closestPt + (tan * gap);

			// If we create a gap, it must be to the interior of the entity.
			okay = ( geoLine->PointOnSeg( (*splitA) ) && !splitA->WithinTol( ps, SMALL ) &&
					 geoLine->PointOnSeg( (*splitB) ) && !splitB->WithinTol( pe, SMALL ) );
		}
		else if (geoArc != NULL)
		{
			double radius = geoArc->Radius();
			int dir = geoArc->Dir();

			double theta = dir * asin( gap / radius );

			C3dCoord pc = geoArc->CenterPt();
			C2dUnitVec vec( (closestPt.X() - pc.X()), (closestPt.Y() - pc.Y()) );

			(*splitA) = pc + ((vec - theta) * radius);
			(*splitB) = pc + ((vec + theta) * radius);

			// If we create a gap, it must be to the interior of the entity.
			okay = ( geoArc->PointOnSeg( (*splitA) ) && !splitA->WithinTol( ps, SMALL ) &&
					 geoArc->PointOnSeg( (*splitB) ) && !splitB->WithinTol( pe, SMALL ) );
		}

		delete geoCurve;

		splitA->Z( closestPt.Z() );
		splitB->Z( closestPt.Z() );
	}

	if ( !okay )
	{
		(*splitA) = closestPt;
		(*splitB) = closestPt;
	}
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Describe2d
//
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
void
CDbCurve::Describe2d( 
	int			regen,
	C3dCoord*	io_tooltip,
	double		tolerance ) const
{
	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );
	if ( !UseOverrideColor() && !regen && !allocated )
		return;  // Nothing to do.

	dispent->Flush();
	
	int	indx;

	C3dCoord ps = StartPt(0);
	C3dCoord pe = EndPt(0);


	dispent->CommandAppend( DCMD_META, (DWORD) META_ENDS );

	if (!ps.WithinTol( *io_tooltip, SMALL ))
	{
		indx = dispent->CoordAppend( ps );
		dispent->CommandAppend( DCMD_DOTMARK, (DWORD) indx );
	}

	indx = dispent->CoordAppend( pe );
	dispent->CommandAppend( DCMD_DOTMARK, (DWORD) indx );
	dispent->CommandAppend( DCMD_META, (DWORD) META_NONE );


	CDbEntity::Describe2d( regen, io_tooltip, tolerance );
}

char EntityType( const CDbCurve* dbCurve )
{
	EDbEntityType	type = dbCurve->Type();

	if (type == DBARC)
	{
		C3dCoord ps = dbCurve->StartPt();
		C3dCoord pe = dbCurve->EndPt();
		return (ps.WithinTol( pe, SMALL ) ? 'C' : 'A');
	}
	else if (type == DBLINE)
	{
		return 'L';
	}
	else
	{
		return '-';
	}
}
