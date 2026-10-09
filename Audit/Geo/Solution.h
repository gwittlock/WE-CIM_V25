#if !defined(_SOLUTION_H)
#define _SOLUTION_H

// ==================================================================
//		Solution
//
//	Geometric Solutions
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"


// ==================================================================

class dllExport CSolution
{
public:
	virtual ~CSolution();

	static int Intersect( const CGeoCurve& in_one, const CGeoCurve& in_two, bool onseg, C3dCoord* io_pt );

	static CGeoArc* Blend( const CGeoCurve& curveA, const CGeoCurve& curveB, double radius, double tol );
	
	static int Blend(
				const CGeoCurve& in_one,
				const CGeoCurve& in_two,
				double in_radius,
				const C2dCoord* p1,
				const C2dCoord* p2,
				C3dCoord* io_pt );

	static int TangentLine( const CGeoArc& in_one, const CGeoArc& in_two, C3dCoord* io_st_pt, C3dCoord* io_en_pt );

	// Gives 2d solution in XY plane with pc.Z = ps.Z
	static int  ArcCenter(
					const C3dCoord& ps,
					const C3dCoord& pe,
					double radius,
					int dir,
					double radians,  // included angle
					C3dCoord* pc );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Calculates the area of a closed profile.
	// The sign of the returned area indicates winding direction.
	// Returns a positive value when the profile winds CCW.
	// Can work for an open profile if it is not ambiguous (eg. S-shaped)
	static double Area( const CGeoCurveArray& curves );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Finds the set of circles through the given point and tangent to the
	// given arc and line.  The solutions are a returned as a set of
	// center points and radii.  A return value of zero indicates no
	// solution.
	static int ArcPAL(
				  const C3dCoord&	refPt,
				  const CGeoArc&	refArc,
				  const CGeoLine&	refLine,
				  C3dCoord*			solnCenter,
				  double*			solnRadius );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Finds the set of circles having the specified radius (rc) and
	// that are tangent to the given curves.
	// (CSolution2.cpp)
	static int ArcTT(
				  const CGeoCurve&	refCurve1,
				  const CGeoCurve&	refCurve2,
				  double			rc,
				  C3dCoord*			solnCenter );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Finds the set of circles having the specified radius (rc) and
	// that are tangent to the given circles.
	// (CSolution2.cpp)
	static int ArcTT(
				  const CGeoArc&	refArc1,
				  const CGeoArc&	refArc2,
				  double			rc,
				  C3dCoord*			solnCenter );
	
	// CSolution2.cpp
	static int quadratic_roots( double a, double b, double c, double* root );

	static int line_ps_tan(
					const C3dCoord&	ps,
					const CGeoArc&	geoArc,
					const C3dCoord&	hint,
					C3dCoord*		pe );

	static int line_tan_tan(
					const CGeoArc&	geoArcA,
					const C3dCoord&	hintA,
					const CGeoArc&	geoArcB,
					const C3dCoord&	hintB,
					C3dCoord*		ptA,
					C3dCoord*		ptB );

protected:

private:
	// Disabled.
	CSolution();
	CSolution( const CSolution& );
	const CSolution& operator = ( const CSolution& );
	int operator == ( const CSolution& ) const;
	int operator != ( const CSolution& ) const;

private:
	static int	intersect_line_line( const CGeoLine& in_line1, const CGeoLine& in_line2, bool onseg, C3dCoord*io_pt );
	static int	intersect_line_arc( const CGeoLine& in_line, const CGeoArc& in_arc, bool onseg, C3dCoord*io_pt );
	static int	intersect_arc_arc( const CGeoArc& in_arc1, const CGeoArc& in_arc2, bool onseg, C3dCoord*io_pt );

	static int	blend_line_line(
							const CGeoLine& in_line1,
							const CGeoLine& in_line2,
							double in_radius,
							const C2dCoord* p1,
							const C2dCoord* p2,
							C3dCoord*io_pt );

	static int	blend_line_arc(
							const CGeoLine& in_line,
							const CGeoArc& in_arc,
							double in_radius,
							const C2dCoord* p1,
							const C2dCoord* p2,
							C3dCoord*io_pt );

	static int	blend_arc_arc(
							const CGeoArc& in_arc1,
							const CGeoArc& in_arc2,
							double in_radius,
							const C2dCoord* p1,
							const C2dCoord* p2,
							C3dCoord*io_pt );

	static void		line_coeff( const CGeoLine& in_line, double* out_a, double* out_b, double* out_c );
	static double	line_coeff_dist( const C2dCoord&	in_pnt, double in_a, double in_b, double in_c );

	// CSolution2.cpp
	static void ArcTT_roots(
					double		term0,
					double		term1,
					double		term2,
					C3dCoord*	solnCenter,
					int*		nsolns );
	// CSolution2.cpp
	static void ArcTT(
					const CGeoCurve&	refCurve1,
					int					dir1,
					const CGeoCurve&	refCurve2,
					int					dir2,
					double				rc,
					C3dCoord*			solnCenter,
					int*				nsolns );

};
#endif

