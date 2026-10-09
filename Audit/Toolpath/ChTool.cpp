
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"
#include "2dUnitVec.h"
#include "GeoArc.h"
#include "Profile.h"
#include "ConvexHull.h"
#include "ChTool.h"



////////////////////////////////////////////////////////////////////////

CChTool::CChTool()

	: m_list(),
	  m_indexable( FALSE )
{
}

CChTool::~CChTool()
{
	m_list.DestructiveFlush();
}

CReturn CChTool::Init( const CProfile& prof )
{
	CProfile copy;
	CReturn status = copy.CopyAppend( prof );

	if ( status.IsOk() )
		status = CommonInit( &copy );

	return status;
}

CReturn CChTool::Init( const CGeoCurveArray& curves )
{
	CReturn status;

	CProfile copy;

	int count = curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		copy.CopyAppend( *(curves.GetAt(indx)) );
	}

	status = CommonInit( &copy );

	return status;
}

const CChArcList& CChTool::ArcList() const
{
	return m_list;
}

CChTool* CChTool::Copy( double radians ) const
{
	CChTool* copy = new CChTool();

	double cosA = cos( -radians );
	double sinA = sin( -radians );

	// Rotate all data about the center of the tool
	// which is at the origin.

	int count = m_list.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CChArc* chArc = m_list[indx];

		C2dCoord center = chArc->CenterPt();
		C2dUnitVec tanA = chArc->StartTan();
		C2dUnitVec tanB = chArc->EndTan();
		double radius = chArc->Radius();
		int dir = chArc->Dir();

		center.XY( (center.X() * cosA + center.Y() * sinA),
				   (center.Y() * cosA - center.X() * sinA) );

		tanA.Init( (tanA.X() * cosA + tanA.Y() * sinA),
				   (tanA.Y() * cosA - tanA.X() * sinA) );

		tanB.Init( (tanB.X() * cosA + tanB.Y() * sinA),
				   (tanB.Y() * cosA - tanB.X() * sinA) );

		copy->m_list.Append( new CChArc( center, tanA, tanB, radius, dir ) );
	}

	CConvexHull::Normalize( &(copy->m_list) );

	return copy;
}

CReturn CChTool::CommonInit( CProfile* prof )
{
	CReturn status;

	if ( !prof->IsClosed() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CChTool::CommonInit() -- open profile" );
		return status;
	}

	// Ensure that we have a closed CCW oriented profile.
	double area = prof->Area();
	if (area < 0.0)
		prof->Reverse();

	int indx, count = prof->Count();
	for (indx = 0; indx < count; ++indx)
	{
		// We could also check for single valued curvature in
		// this loop, but that check is deferred till after
		// we create the convex hull, so as to minimize the
		// calculation of curve tangent vectors.

		CGeoArc* arc = dynamic_cast<CGeoArc*>( prof->GetAt(indx) );

		if (arc != NULL && arc->Dir() < 0)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CChTool::CommonInit() -- cw arc" );
			return status;
		}
	}

	CConvexHull::Convert( (*prof), 0, &m_list );

	// Ensure that we have single valued curvature.
	indx = 0;
	while (indx < m_list.Count())
	{
		CChArc* chArc = m_list[indx];

		C2dUnitVec ts = chArc->StartTan();
		C2dUnitVec te = chArc->EndTan();

		double cross = ts ^ te;
		if (cross < -VECTOR_SMALL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CChTool::CommonInit() -- concavity" );
			m_list.DestructiveFlush();
			return status;
		}

		if ( !chArc->IsTurn() )
		{
			// Moves may appear near the beginning and/or end
			// of the tool when the first and/or last element
			// of the tool profile is a line.  Since the tool
			// is guaranteed to be closed and C1 continuous, we
			// can simply remove the moves, thereby simplifying
			// the representation of the tool.

			delete m_list.Remove( indx );
		}
		else
		{
			++indx;
		}
	}

	return status;
}
