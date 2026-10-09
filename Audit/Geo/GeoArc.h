
#ifndef _GEOARC_H
#define _GEOARC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IndxList.h"
#include "GeoCurve.h"
#include "2dUnitVec.h"
#include "3dCoord.h"
#include "3x4Matrix.h"

class CGeoArc;
typedef CIndxList<CGeoArc*> CGeoArcList;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CGeoArc : public CGeoCurve  
{
public:

	CGeoArc( const C3dCoord& start, const C3dCoord& end, const C3dCoord& center, int dir );

	CGeoArc( const C3dCoord& center, double radius, int dir );

	CGeoArc( const CGeoArc& arc );

	// An alternate paired construction sequence.
	// As the CGeoArc() constructor creates an uninitialized
	// arc, the arc object MUST BE INITIALIZED using Init().
	// If you attempt to initialize the arc using StartPt(),
	// etc... an assertion will fired.
	CGeoArc();

	virtual ~CGeoArc();

	void Init( const C3dCoord& start, const C3dCoord& end, const C3dCoord& center, int dir );

	void Init( const C3dCoord& center, double radius, int dir );
	
	virtual CGeoElem* Clone( bool attribs_copy ) const;
	
	const CGeoArc& operator = ( const CGeoArc& arc );

	virtual ElemType Type() const;

	virtual const C3dCoord& StartPt() const;
	
	virtual void StartPt( const C3dCoord& pt );
	virtual void StartPt( double xs, double ys, double zs );

	virtual const C3dCoord& EndPt() const;

	virtual void EndPt( const C3dCoord& pt );
	virtual void EndPt( double xe, double ye, double ze );

	virtual double Length2d() const;

	virtual C2dUnitVec StartTan() const;
	virtual C2dUnitVec EndTan() const;

	virtual C2dUnitVec TanAtPt( const C3dCoord& pt ) const;
	virtual C2dUnitVec TanAtPt( double x, double y ) const;

	// IsCircle() and IsClosed() are synonomous.
	bool IsCircle( double tol ) const;
	virtual bool IsClosed( double tol ) const;

	// Create an element offset from this element by the
	// given amount and in the given direction.  The offset
	// distance is taken as abs(amt) and the direction is
	// taken as the sign(dir), where (-1) left / (+1) right.
	// ERROR:  Left is +1, Right is -1
	virtual CGeoCurve* Offset( int dir, double amt ) const;
	bool Convex( int side ) const;

	virtual void Reverse();

	// Makes this arc its compliment, maintaining direction.
	void Compliment();

	// Return value is the 2d distance from the point to
	// the element.  The closest point does not necessarily
	// lay on the element, therefore the u-value should be
	// examined.
	virtual double PointClosest( const C3dCoord& pt, C3dCoord* closestPt, double* u ) const;
	virtual double PointUparam( const C3dCoord& pt ) const;

	virtual int PointSide( const C3dCoord& pt, double tol=SMALL ) const;

	// 2d solution
	virtual bool PointOnSeg( const C3dCoord& pt ) const;

	virtual C3dCoord MidPt() const;

	virtual C3dCoord PointAtDist( double dist, bool fromStart ) const;

	const C3dCoord& CenterPt() const;
	void CenterPt( const C3dCoord& pt );
	void CenterPt( double xe, double ye, double ze );

	double Radius() const;

	// Returns (-1) ccw / (+1) cw
	int Dir() const;
	void Dir( int dir );

	void Angles( double* startAngle, double* endAngle ) const;
	void Angles( double xs, double ys, double xe, double ey, double* startAngle, double* endAngle ) const;

	double IncludedAngle() const;
	double IncludedAngle( double xs, double ys, double xe, double ey ) const;

	// Result in radians where ( 0 <= result < TWOPI).
	double Angle( double x, double y ) const;

	C3dCoordList* Explode( double in_tolerance, const C3x4Matrix* in_xform ) const;

	// Explode this arc into its constituent 'single quadrant' arcs.
	void QuadrantArcs( CGeoArcList* quadArcs );

	virtual double InterceptX( double at_y ) const;
	virtual double InterceptY( double at_x ) const;
	virtual double InterceptXY( int prim_ord, double at_x ) const;

	virtual void Xform( const C3x4Matrix& xform );
	virtual void Shift( const C3dVec& delta );

	virtual void Tabulate(
		double			chordal_tol,
		const C3dVec&	shift,
		C3dCoordArray*	pts ) const;

	// For debugging.
	virtual void Dump() const;

private:  // Disabled.

	int operator == ( const CGeoArc& arc ) const;
	int operator != ( const CGeoArc& arc ) const;

private:  // Methods

	void Adjust( double as, double ae, double* ap, double* uParam ) const;
	void BoxUpdate();
	bool QuadIterDone( int dir, double ai, double ae );

private:  // Data

	C3dCoord m_ps;
	C3dCoord m_pe;
	C3dCoord m_pc;

	double m_radius;

	// Arc direction (-1) cw / (+1) ccw
	// The direction is stored as an integer
	// in order to simplify offset calculations.
	int m_dir;
};


#endif // !defined(AFX_ARCELEM_H__B9918899_F186_11D2_8FBA_0040335A7848__INCLUDED_)
