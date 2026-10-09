
#include "stdafx.h"
#include "MathConst.h"
#include "GeoCurve.h"


//////////////////////////////////////////////////////////////////////

CGeoCurve::CGeoCurve()
{
}

CGeoCurve::~CGeoCurve()
{
}

C2dUnitVec CGeoCurve::StartToEndVec() const
{
	C2dUnitVec result = ((Type() == GEOLINE) ? StartTan() : (EndPt() - StartPt()));
	return result;
}

C3dCoord CGeoCurve::PointAtUparam( double uparam ) const
{
	double len = Length2d();
	return ( PointAtDist( (len * uparam), TRUE ) );
}


void CGeoCurve::ConditionalAppend( const C3dCoord& ptA, C3dCoordArray* pts ) const
{
	int indx, count = pts->Count();
	for (indx = 0; indx < count; ++indx)
	{
		C3dCoord* ptB = pts->GetAt( indx );
		if ( ptB->WithinTolXY( ptA, SMALL ) )
			break;
	}

	if (indx >= count)
		pts->Append( new C3dCoord( ptA ) );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

C2dBox CGeoCurveArray::Box() const
{
	C3dBox box;

	int count = Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* geoCurve = GetAt( indx );
		box += geoCurve->Box();
	}

	return box;
}

void CGeoCurveArray::Draw() const
{
	GeoRendererGet().DrawGeo( (CGeoElemArray&) *this );
}

void CGeoCurveList::Draw() const
{
	GeoRendererGet().DrawGeo( (CGeoElemList&) *this );
}
