
#ifndef _GEOLINE_H
#define _GEOLINE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GeoCurve.h"


class dllExport CGeoLine : public CGeoCurve  
{
public:

	CGeoLine();

	CGeoLine( const C3dCoord& start, const C3dCoord& end );

	CGeoLine( double xs, double ys, double xe, double ye );

	CGeoLine( double xs, double ys, double zs, double xe, double ye, double ze );

	CGeoLine( const CGeoLine& elem );

	virtual ~CGeoLine();
	
	void Init( const C3dCoord& start, const C3dCoord& end );

	virtual CGeoElem* Clone( bool attribs_copy ) const;

	const CGeoLine& operator = ( const CGeoLine& elem );

	virtual ElemType Type() const;

	virtual const C3dCoord& StartPt() const;
	
	virtual void StartPt( const C3dCoord& pt );
	virtual void StartPt( double xs, double ys, double zs );

	virtual const C3dCoord& EndPt() const;

	virtual void EndPt( const C3dCoord& pt );
	virtual void EndPt( double xe, double ye, double ze );

	virtual double Length2d() const;

	// Silly yes but for the sake of polymorphism ....
	virtual C2dUnitVec StartTan() const;
	virtual C2dUnitVec EndTan() const;

	virtual C2dUnitVec TanAtPt( const C3dCoord& pt ) const;
	virtual C2dUnitVec TanAtPt( double x, double y ) const;

#pragma warning( push )
#pragma warning( disable : 4100 )
	virtual bool IsClosed( double tol ) const  { return false; }
#pragma warning( pop )

	// Create an element offset from this element by the
	// given amount and in the given direction.  The offset
	// distance is taken as abs(amt) and the direction is
	// taken as the sign(dir), where (-1) left / (+1) right.
	// ERROR:  Left is +1, Right is -1
	virtual CGeoCurve* Offset( int dir, double amt ) const;

	virtual void Reverse();

	// Return value is the 2d distance from the point to
	// the element.  The closest point does not necessarily
	// lay on the element, therefore the u-value should be
	// examined.
	virtual double PointClosest( const C3dCoord& pt, C3dCoord* closestPt, double* u ) const;
	virtual double PointUparam( const C3dCoord& pt ) const;

	// Return -1 if the point is to the left, +1 to the right, 0 on the line
	int PointSide( const C3dCoord& pt, double tol=SMALL ) const;

	// 2d solution
	virtual bool PointOnSeg( const C3dCoord& pt ) const;

	virtual C3dCoord MidPt() const;

	virtual C3dCoord PointAtDist( double dist, bool fromStart ) const;

	virtual double InterceptX( double at_y ) const;
	virtual double InterceptY( double at_x ) const;
	virtual double InterceptXY( int prim_ord, double at_x ) const;

	virtual void Xform( const C3x4Matrix& xform );
	virtual void Shift( const C3dVec& delta );

	virtual void Tabulate(
		double			chordal_tol,
		const C3dVec&	shift,
		C3dCoordArray*	pts ) const;

	virtual void Dump() const;

private:

	// Disabled.
	int operator == ( const CGeoLine& line ) const;
	int operator != ( const CGeoLine& line ) const;

private:

	// Update the bounding box of the line.
	void BoxUpdate();

	C3dCoord m_ps;
	C3dCoord m_pe;
};

#endif

