
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"
#include "2dUnitVec.h"
#include "GeoArc.h"
#include "Profile.h"
#include "ConvexHull.h"
#include "ChPart.h"



////////////////////////////////////////////////////////////////////////

CChPart::CChPart()
	: m_list()
{
}

CChPart::~CChPart()
{
	m_list.DestructiveFlush();
}

CReturn
CChPart::Init( const CProfile& prof, int offset_dir )
{
	return ( Init( prof.Curves(), offset_dir ) );
}

CReturn
CChPart::Init( const CGeoCurveArray& curves, int offset_dir )
{
	CProfile	copy;
	CReturn		status;

	int count = curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CGeoCurve* geoCurve = curves[indx];
		copy.CopyAppend( (*geoCurve) );
	}

	status = CommonInit( &copy, offset_dir );

	return status;
}

const CChArcList&
CChPart::ArcList() const
{
	return m_list;
}

CReturn
CChPart::CommonInit( CProfile* prof, int offset_dir )
{
	CReturn	status;
	CChArc*	chArc;
	int		count;

	status = CConvexHull::Convert( (*prof), offset_dir, &m_list );

	if ( status.IsOk() )
	{
		count = m_list.Count() - 1;
		if (count >= 0)
		{
			chArc = m_list[ count ];
			if ( chArc->IsTurn() )
			{
				if (chArc->Radius() < SMALL)
				{
					// Remove this degenerate turn from the end of the part
					// because it will cause the convex hull offset to gouge.
					// This chArc is generated when a profile is closed at a corner.

					delete m_list.Remove( count );
				}
				else
				{
					// Append a null chArc to the end of a profile that ends
					// with a bonified turn.  This is critical to CConvexHull::Offset()
					// with respect to the AtEnd() condition.

					m_list.Append( NULL );
				}
			}
		}
	}

	return status;
}
