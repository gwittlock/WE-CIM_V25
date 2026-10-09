
#ifndef _INT2D_H
#define _INT2D_H

#ifndef _2DCOORD_H
#include "2dCoord.h"
#endif

#ifndef _GEOLINE_H
#include "GeoLine.h"
#endif

#ifndef _GEOARC_H
#include "GeoArc.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CInt2d
{
public:

	// Constructs an intersection object with default values of:
	// 1. Tol( SMALL )
	// 2. OnSeg( TRUE )
	CInt2d();

	CInt2d( double tol, bool onSeg );

	// Sets the 'virtual intersection' tolerance.
	void Tol( double tol );

	// Sets the "parallel" threshold for line/line intersection.
	// Default value is VECTOR_SMALL (ie. 1.e-12).
	void ParallelTol( double tol );

	// Sets the intersection mode.  When active, the results
	// exclude intersection points that do not lay on the
	// bounded portion of either curve.
	void OnSeg( bool active );

	// The intersection methods.  Returns the count of intersections.
	int CrvCrv( const CGeoCurve& curveA, const CGeoCurve& curveB );
	int SegSeg( const CGeoLine& lineA, const CGeoLine& lineB );
	int ArcSeg( const CGeoArc& arcA, const CGeoLine& lineB );
	int ArcArc( const CGeoArc& arcA, const CGeoArc& arcB );

	// Gets the count of solutions.
	int Count() const;

	// Gets the Ith intersection point.
	const C2dCoord& Point( int indx ) const;

	// Gets the Ith curve parameter of the first curve.
	double Uparam( int indx ) const;

	// Gets the Ith curve parameter of the second curve.
	double Vparam( int indx ) const;

	bool Tangent() const;

	virtual ~ CInt2d();

private:

	double		m_tol;
	double		m_parallel_tol;
	bool		m_onSeg;
	bool		m_tangent;
	int			m_count;
	C2dCoord	m_pt[2];
	double		m_u[2];
	double		m_v[2];
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

VOID_DLL Int2dCurveCurve(
		const CGeoCurve& curveA,
		const CGeoCurve& curveB,
		double tol,
		double parallel_tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln );

VOID_DLL Int2dSegSeg(
		const CGeoLine& lineA,
		const CGeoLine& lineB,
		double tol,
		double parallel_tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln );

VOID_DLL Int2dArcSeg(
		const CGeoArc& arc,
		const CGeoLine& line,
		double tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln );

VOID_DLL Int2dArcArc(
		const CGeoArc& arcA,
		const CGeoArc& arcB,
		double tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln );

#endif
