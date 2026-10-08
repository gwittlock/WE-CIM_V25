
#ifndef _GEOCURVE_H
#define _GEOCURVE_H

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// An abstract base class representing a geometric curve.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IndxList.h"
#include "DynamicArray.h"
#include "GeoElem.h"
#include "2dUnitVec.h"
#include "3dCoord.h"
#include "3dVec.h"
#include "Return.h"



class dllExport CGeoCurve : public CGeoElem  
{
public:

	CGeoCurve();
	
	virtual ~CGeoCurve();

	virtual C2dUnitVec StartTan() const = 0;

	virtual C2dUnitVec EndTan() const = 0;

	C2dUnitVec StartToEndVec() const;

	virtual bool IsClosed( double tol ) const = 0;

	// Create an element offset from this element by the
	// given amount and in the given direction.  The offset
	// distance is taken as abs(amt) and the direction is
	// taken as the sign(dir), where (-1) left / (+1) right.
	virtual CGeoCurve* Offset( int dir, double amt ) const = 0;

	// Reverse the direction of this curve.
	virtual void Reverse() = 0;

	// Return value is the 2d distance from the point to
	// the element.  The closest point does not necessarily
	// lay on the element, therefore the u-value should be
	// examined.
	virtual double PointClosest( const C3dCoord& pt, C3dCoord* closestPt, double* u ) const = 0;
	virtual double PointUparam( const C3dCoord& pt ) const = 0;

	virtual int PointSide( const C3dCoord& pt, double tol=SMALL ) const = 0;

	// Is this bad, putting this function in Curve?
	virtual bool PointOnSeg( const C3dCoord& pt ) const = 0;

	virtual double Length2d() const = 0;
	virtual C3dCoord MidPt() const = 0;

	virtual C3dCoord PointAtDist( double dist, bool fromStart ) const = 0 ;
	C3dCoord PointAtUparam( double uparam ) const;

	virtual C2dUnitVec TanAtPt( const C3dCoord& pt ) const = 0;
	virtual C2dUnitVec TanAtPt( double x, double y ) const = 0;

	virtual double InterceptX( double at_y ) const = 0;
	virtual double InterceptY( double at_x ) const = 0;
	virtual double InterceptXY( int prim_ord, double at_x ) const = 0;

	virtual void Tabulate(
		double			chordal_tol,
		const C3dVec&	shift,
		C3dCoordArray*	pts ) const = 0;

	virtual void Dump() const = 0;

protected:

	void ConditionalAppend( const C3dCoord& ptA, C3dCoordArray* pts ) const;

private:

	// Disabled.
	CGeoCurve( const CGeoCurve& );
	const CGeoCurve& operator = ( const CGeoCurve& );
	int operator == ( const CGeoCurve& ) const;
	int operator != ( const CGeoCurve& ) const;
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CGeoCurveArray : public CDynamicArray<CGeoCurve*>
{
public:

	CGeoCurveArray()  { }
	virtual ~CGeoCurveArray()  { }

	C2dBox Box() const;
	void Draw() const;
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CGeoCurveList : public CIndxList<CGeoCurve*>
{
public:

	CGeoCurveList()  { }
	virtual ~CGeoCurveList()  { }

	void Draw() const;
};

#endif

