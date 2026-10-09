// ==================================================================
//		Solution
//
// ==================================================================

#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"

#include "2dUnitVec.h"
#include "Int2d.h"
#include "Solution.h"


// ==================================================================

CSolution::CSolution()
{
}

CSolution::~CSolution()
{
}


// ==================================================================
//	Intersect two curves, returning a list of 0 to 2 intersection points.
//	TODO:  When we add ellipses, 0 to 4 intersections.
//
//	The calling code supplies the D2dCoord array that is set, since
//	I don't want to perform memory allocations here.
//
//	Intersections are 2-dimensional, with the Z ordinate set to zero.
//	If elements miss in Z, these intersections don't notice.
//	To determine segment hits, parameterize the results with PointClosest
//
int CSolution::Intersect( 
	const CGeoCurve& in_one,	// First curve
	const CGeoCurve& in_two,	// Second curve
	bool			onseg,		// TRUE if require on-segment
	C3dCoord*		io_pt )		// Array[2] to return hits int
{
	switch (in_one.Type())
	{
		case GEOLINE:
			switch (in_two.Type())
			{
				case GEOLINE:
					return intersect_line_line( (CGeoLine&)in_one, (CGeoLine&)in_two, onseg, io_pt );
				case GEOARC:
					return intersect_line_arc( (CGeoLine&)in_one, (CGeoArc&)in_two, onseg, io_pt );
			}
			break;

		case GEOARC:
			switch (in_two.Type())
			{
				case GEOLINE:
					return intersect_line_arc( (CGeoLine&)in_two, (CGeoArc&)in_one, onseg, io_pt );
				case GEOARC:
					return intersect_arc_arc( (CGeoArc&)in_one, (CGeoArc&)in_two, onseg, io_pt );
			}
			break;
	}
	return 0;
}

// ------------------------------------------------------------------

bool IsOnSeg( const C3dCoord& psA, const C3dCoord& peA, const C3dCoord& soln )
{
	if ( soln.WithinTolXY( psA, SMALL ) )
		return TRUE;

	if ( soln.WithinTolXY( peA, SMALL ) )
		return TRUE;

	C2dUnitVec vecA = soln - psA;
	C2dUnitVec vecB = soln - peA;
	double dot = vecA * vecB;
	return ((dot < 0.) ? TRUE : FALSE);
}

int CSolution::intersect_line_line(
	const CGeoLine&	lineA, 
	const CGeoLine&	lineB, 
	bool			onseg,		// TRUE if require on-segment
	C3dCoord*		io_pt )
{
	const C3dCoord& psA = lineA.StartPt();
	const C3dCoord& peA = lineA.EndPt();

	const C3dCoord& psB = lineB.StartPt();
	const C3dCoord& peB = lineB.EndPt();

	// From Graphics Gems III, Faster Line Segment Intersection,
	// Franklin Antonion, p.199

	// (Ax, Ay) 2D vector form of line 1
	C3dVec delta_1 = peA - psA;
	// (Bx, By) 2D vector form of line 2
	C3dVec delta_2 = peB - psB;
	// (Cx, Cy) 2D vector from line 2 to line 1
	C3dVec delta_3 = psA - peB;

	// Some intermediary values... see the book for their meaning
	double denom = delta_1.Y()*delta_2.X() - delta_1.X()*delta_2.Y();  // (f)
	if (EQUAL( denom, 0.0 ))
		return 0;		// Co-linear

	// (d) numerator from lines 2 and 3
	double numer_23 = delta_2.Y()*delta_3.X() - delta_2.X()*delta_3.Y();
	// (e) numerator from lines 1 and 3
	double numer_13 = delta_1.X()*delta_3.Y() - delta_1.Y()*delta_3.X();

	double alpha = numer_23 / denom;

	C3dCoord soln = lineA.StartPt() + (delta_1 * alpha);

	if (onseg)
	{
		onseg = (IsOnSeg( psA, peA, soln ) && IsOnSeg( psB, peB, soln ));
		if ( !onseg )
			return 0;
	}

	// Set the values for the single intersection
	io_pt[0] = soln;

	return 1;
}

// ------------------------------------------------------------------

int CSolution::intersect_line_arc(
	const CGeoLine&	in_line, 
	const CGeoArc&	in_arc, 
	bool			onseg,		// TRUE if require on-segment
	C3dCoord*		io_pt )
{
	//
	// Graphics Gems I, p.5
	//

	// Special check for line tangent to arc...
	C3dCoord close_pt;
	double param_u;

	double radius = in_arc.Radius();
	double dist = in_line.PointClosest( in_arc.CenterPt(), &close_pt, &param_u );

	if (EQUAL( dist, radius ))
	{
		if ( !onseg 
			|| (in_line.PointOnSeg( close_pt ) 
				&& in_arc.PointOnSeg( close_pt )) )
		{
			io_pt[0] = close_pt;
			return 1;
		}
		return 0;
	}

	if (dist > (radius+SMALL))
		return 0;

	// Intersect to the full circle, returning u1 and u2 u-parameters
	C2dVec	delta = in_arc.CenterPt() - in_line.StartPt();
	C2dVec	vector = in_line.EndPt() - in_line.StartPt();

	double a = (vector * vector);
	double b = 2 * (vector * delta );
	double c = (delta * delta) - radius*radius;
	double d = fabs( b*b - 4*a*c );

	if ( (d + SMALL) < 0.0 )
		return 0;


	double term = sqrt( d );
	double u1 = (b + term) / (2*a);
	double u2 = (b - term) / (2*a);

	// Set the intersection points...
	C3dCoord sol_pt;
	bool hit;
	int hitnum = 0;

	hit = TRUE;

	sol_pt = in_line.StartPt() + (vector * u1);

	if (onseg)
	{
		if ( !in_line.PointOnSeg( sol_pt ) || !in_arc.PointOnSeg( sol_pt ) )
			hit = FALSE;
	}

	if (hit)
		io_pt[hitnum++] = sol_pt;


	sol_pt = in_line.StartPt() + (vector * u2);

	hit = TRUE;

	if (onseg)
	{
		if ( !in_line.PointOnSeg( sol_pt ) || !in_arc.PointOnSeg( sol_pt ) )
			hit = FALSE;
	}

	if (hit)
		io_pt[hitnum++] = sol_pt;

	return hitnum;
}

// ------------------------------------------------------------------
//	arc/arc taken from Gouge/Int2d.cpp Int2dArcSeg(), from Patrick's world
//	purportedly very robust.
//
int CSolution::intersect_arc_arc(
	const CGeoArc&	in_arc1, 
	const CGeoArc&	in_arc2, 
	bool			onseg,		// TRUE if require on-segment
	C3dCoord*		io_pt )
{
	const double tol = SMALL;		// Hard-coded tolerance
	C3dCoord	sol_pt;
	bool		hit;

	// Setup the basic values
	C2dCoord ctr1 = in_arc1.CenterPt();
	C2dCoord ctr2 = in_arc2.CenterPt();
	double sol_z = in_arc1.CenterPt().Z();

	double radius1 = in_arc1.Radius();
	if (radius1 < tol)
		radius1 = 0.0;

	double radius2 = in_arc2.Radius();
	if (radius2 < tol)
		radius2 = 0.0;

	C2dVec ctr_vec = ctr2 - ctr1;
	double ctr_dist = ctr_vec.Length();

	// Solution support values, plus endless error tests
	if (ctr_dist < tol)
	{
		if (fabs(radius1 - radius2) < tol)
			return 0;	// Coincident circles
		else
			return 0;	// No intersection, common centers
	}

	// Normalize our center vector
	ctr_vec *= (1 / ctr_dist);

	// Check the circles one way...for outside tangency
	double delta = ctr_dist - (radius1 + radius2);
	if (delta > tol)
		return 0;		// Too far apart
	else
	if (fabs(delta) < tol)
	{
		// Tangent at one point
		sol_pt = ctr1 + (ctr_vec * radius1);
		sol_pt.Z( sol_z );

		hit = TRUE;
		if (onseg)
		{
			if ( !in_arc1.PointOnSeg( sol_pt )
				|| !in_arc2.PointOnSeg( sol_pt ) )
				hit = FALSE;
		}
		else
			hit = TRUE;
		if (hit)
		{
			io_pt[0] = sol_pt;
			return 1;
		}
		return 0;
	}
	
	// ... and another for inside tangency
	delta = fabs(radius1 - radius2) - ctr_dist;
	if (delta > tol)
		return 0;		// internally separated
	else
	if (fabs(delta) < tol)
	{
		// Tangent at one point
		if (radius2 < radius1)
			sol_pt = ctr1 + (ctr_vec * radius1);
		else
			sol_pt = ctr1 - (ctr_vec * radius1);
		sol_pt.Z( sol_z );

		hit = TRUE;
		if (onseg)
		{
			if ( !in_arc1.PointOnSeg( sol_pt )
				|| !in_arc2.PointOnSeg( sol_pt ) )
				hit = FALSE;
		}
		else
			hit = TRUE;
		if (hit)
		{
			io_pt[0] = sol_pt;
			return 1;
		}
		return 0;
	}

	// Finally, perform the two-point solution
	double dx = (ctr_dist*ctr_dist + radius1*radius1 - radius2*radius2) / (2*ctr_dist);
	double dy = radius1*radius1 - dx*dx;

	if (dy < 0.0)
	{
		// Imaginary roots, but we should never reach this anyway because
		// of the copius error tests above.
		return 0;
	}

	dy = sqrt(dy);

	// One
	double x = ctr1.X() + dx*ctr_vec.X() - dy*ctr_vec.Y();
	double y = ctr1.Y() + dx*ctr_vec.Y() + dy*ctr_vec.X();

	int hitnum = 0;

	sol_pt = C3dCoord( x, y, sol_z );
	hit = TRUE;
	if (onseg)
	{
		if ( !in_arc1.PointOnSeg( sol_pt )
			|| !in_arc2.PointOnSeg( sol_pt ) )
			hit = FALSE;
	}
	else
		hit = TRUE;
	if (hit)
		io_pt[hitnum++] = sol_pt;

	// Two
	x = ctr1.X() + dx*ctr_vec.X() + dy*ctr_vec.Y();
	y = ctr1.Y() + dx*ctr_vec.Y() - dy*ctr_vec.X();

	sol_pt = C3dCoord( x, y, sol_z );
	hit = TRUE;
	if (onseg)
	{
		if ( !in_arc1.PointOnSeg( sol_pt )
			|| !in_arc2.PointOnSeg( sol_pt ) )
			hit = FALSE;
	}
	else
		hit = TRUE;
	if (hit)
		io_pt[hitnum++] = sol_pt;

	return hitnum;
}

CGeoArc* CSolution::Blend( const CGeoCurve& curveA, const CGeoCurve& curveB, double radius, double tol )
{
	int dir;

	CGeoCurve* castA = NULL;
	CGeoCurve* castB = NULL;

	CGeoCurve* offsetA = NULL;
	CGeoCurve* offsetB = NULL;

	CGeoArc* blend = NULL;

	if (curveA.Type() == GEOLINE)
		castA = (CGeoCurve*) (new CGeoLine( (CGeoLine&) curveA ));
	else if (curveA.Type() == GEOARC)
		castA = (CGeoCurve*) (new CGeoArc( (CGeoArc&) curveA ));

	if (curveB.Type() == GEOLINE)
		castB = (CGeoCurve*) (new CGeoLine( (CGeoLine&) curveB ));
	else if (curveB.Type() == GEOARC)
		castB = (CGeoCurve*) (new CGeoArc( (CGeoArc&) curveB ));

	if (castA != NULL && castB != NULL)
	{
		bool okay = TRUE;

		C3dCoord psA = castA->StartPt();
		C3dCoord peA = castA->EndPt();
		C3dCoord psB = castB->StartPt();
		C3dCoord peB = castB->EndPt();

		if ( peA.WithinTol( psB, tol ) )
		{
			// Desired case.  Nothing to do.
		}
		else if ( peA.WithinTol( peB, tol ) )
		{
			castB->Reverse();
		}
		else
		{
			if ( psA.WithinTol( psB, tol ) )
			{
				castA->Reverse();
			}
			else if ( psA.WithinTol( peB, tol ) )
			{
				castA->Reverse();
				castB->Reverse();
			}
			else
			{
				okay = FALSE;
			}
		}

		if ( okay )
		{
			C2dUnitVec tanA = castA->EndTan();
			C2dUnitVec tanB = castB->StartTan();

			double cross = tanA ^ tanB;
			if (fabs(cross) >= VECTOR_SMALL)
			{
				dir = ((cross > 0) ? 1 : -1);

				offsetA = castA->Offset( dir, radius );
				offsetB = castB->Offset( dir, radius );
			}
		}
	}

	if (offsetA != NULL && offsetB != NULL)
	{
		C2dCoord pts[2];
		double uA[2];
		double uB[2];
		int nSoln;

		Int2dCurveCurve( (*offsetA), (*offsetB), SMALL, VECTOR_SMALL, TRUE, pts, uA, uB, &nSoln );

		if ( nSoln == 1 &&
			(uA[0] > -SMALL && uA[0] <= (1.0+SMALL)) &&
			(uB[0] > -SMALL && uB[0] <= (1.0+SMALL)) )
		{
			C3dCoord clsA;
			C3dCoord clsB;
			double u0, u1;

			C3dCoord pc( pts[0].X(), pts[0].Y(), castA->StartPt().Z() );
			
			castA->PointClosest( pc, &clsA, &u0 );
			castB->PointClosest( pc, &clsB, &u1 );

			if ((u0 > -SMALL && u0 < (1.0+SMALL)) &&
				(u1 > -SMALL && u1 < (1.0+SMALL)) )
			{
				blend = new CGeoArc( clsA, clsB, pc, dir );
			}
		}
	}

	delete castA;
	delete castB;
	delete offsetA;
	delete offsetB;

	return blend;
}


// ==================================================================
//	Create an arc of a given radius, tangent to two curves.
//
//	One of the curves may be a zero-radius circle, as a fixed
//	point.
//
//	This may generate up to 8 solutions; the solutions are given as
//	a circle's center point only.  Intersections of that circle
//	can be used to find the start and end points.
//
int CSolution::Blend( 
	const CGeoCurve& in_one,	// First curve
	const CGeoCurve& in_two,	// Second curve
	double			 in_radius,	// Blend radius
	const C2dCoord*  p1,		// Optional reference point on in_one
	const C2dCoord*  p2,		// Optional reference point on in_two
	C3dCoord*		 io_pt )	// Array[8] to return solution centers
{
	switch (in_one.Type())
	{
	case GEOLINE:
		switch (in_two.Type())
		{
		case GEOLINE:
			return blend_line_line( (CGeoLine&)in_one, (CGeoLine&)in_two, in_radius, p1, p2, io_pt );
		case GEOARC:
			return blend_line_arc( (CGeoLine&)in_one, (CGeoArc&)in_two, in_radius, p1, p2, io_pt );
		}
		break;

	case GEOARC:
		switch (in_two.Type())
		{
		case GEOLINE:
			return blend_line_arc( (CGeoLine&)in_two, (CGeoArc&)in_one, in_radius, p2, p1, io_pt );
		case GEOARC:
			return blend_arc_arc( (CGeoArc&)in_one, (CGeoArc&)in_two, in_radius, p1, p2, io_pt );
		}
		break;
	}
	return 0;
}

// ------------------------------------------------------------------

int CSolution::blend_line_line( 
	const CGeoLine& in_line1,	// First line
	const CGeoLine& in_line2,	// Second line
	double			in_radius,	// Blend radius
	const C2dCoord*  p1,		// Optional reference point on in_line1
	const C2dCoord*  p2,		// Optional reference point on in_line2
	C3dCoord*		io_pt )		// Array[1] to return solution centers
{
	if (p1 != NULL && p2 != NULL)
	{
		C2dUnitVec vecA, vecB, vecC;
		int sideA, sideB;
		double cross;
		int nSoln = 0;

		vecA = in_line1.StartTan();
		vecB = in_line2.StartTan();

		vecC.Init( (p2->X() - p1->X()), (p2->Y() - p1->Y()) );

		cross = vecC ^ vecA;
		sideA = ((cross > 0) ? RIGHT : LEFT);

		cross = vecC ^ vecB;
		sideB = ((cross > 0) ? LEFT : RIGHT);

		CGeoCurve* offsetA = in_line1.Offset( sideA, in_radius );
		CGeoCurve* offsetB = in_line2.Offset( sideB, in_radius );

		if (offsetA == NULL && offsetB == NULL)
		{
			CReturn status;
			status.Internal( IDS_INTERNAL_ERROR, "CSolution::blend_line_line()" );
		}
		else
		{
			CInt2d intersector( SMALL, FALSE );
			nSoln = intersector.CrvCrv( (*offsetA), (*offsetB) );

			if (nSoln == 1)
			{
				const C2dCoord& pt = intersector.Point( 0 );
				io_pt[0].X( pt.X() );
				io_pt[0].Y( pt.Y() );
				io_pt[0].Z( 0.0 );	// TODO:  find actual Z? 
			}
		}

		delete offsetA;
		delete offsetB;

		return nSoln;
	}
	else
	{
		// From Graphics Gems III, Joining Two Lines with a Circular
		//	Arc Fillet, Rober D. Miller, p.193
		//
		double	a1, b1, c1;
		C2dCoord	mid1( (in_line1.StartPt().X() + in_line1.EndPt().X()) / 2.0,
							(in_line1.StartPt().Y() + in_line1.EndPt().Y()) / 2.0 );
		line_coeff( in_line1, &a1, &b1, &c1 );

		double	a2, b2, c2;
		C2dCoord	mid2( (in_line2.StartPt().X() + in_line2.EndPt().X()) / 2.0,
							(in_line2.StartPt().Y() + in_line2.EndPt().Y()) / 2.0 );
		line_coeff( in_line2, &a2, &b2, &c2 );

		// Are the lines parallel?
		double dist = a1*b2 - a2*b1;
		if ( EQUAL( dist, 0.0 ))
			return 0;

		// Now, the distances between lines...
		double dist1 = line_coeff_dist( mid2, a1, b1, c1 );
		double dist2 = line_coeff_dist( mid1, a2, b2, c2 );

		if ( EQUAL( dist1, 0.0 )
			|| EQUAL( dist2, 0.0 ) )
			return 0;

		// C1 parameter...
		double	srad = in_radius * SGN(dist1);
		double	c1p = c1 - srad * sqrt( a1*a1 + b1*b1 );

		// C2 parameter...
		srad = in_radius * SGN(dist2);
		double	c2p = c2 - srad * sqrt( a2*a2 + b2*b2 );

		// Arc center...
		io_pt[0].X( (c2p*b1 - c1p*b2) / dist );
		io_pt[0].Y( (c1p*a2 - c2p*a1) / dist );
		io_pt[0].Z( 0.0 );	// TODO:  find actual Z?  from a line?  what about sloping lines?
	}

	// Issues of intersection points and arc directions
	// are left to the calling code... this does the hard part,
	// finding the arc center.
	//
	//	The point on each line can be found with PointClosest
	//	using the arc center.

	return 1;
}

// ------------------------------------------------------------------

int CSolution::blend_line_arc( 
	const CGeoLine& in_line,	// Line
	const CGeoArc&  in_arc,		// Arc
	double			in_radius,	// Blend radius
	const C2dCoord* p1,			// Optional reference point on in_line
	const C2dCoord* p2,			// Optional reference point on in_arc
	C3dCoord*		io_pt )		// Array[8] to return solution centers
{
	if (p1 != NULL && p2 != NULL)
	{
		C2dCoord pc;
		C2dVec vec, vecA, vecB;
		int sideA, sideB;
		double dist, radius, cross;
		int nSoln = 0;

		vecA = in_line.StartTan();
		vecB.Init( (p2->X() - p1->X()), (p2->Y() - p1->Y()) );

		cross = vecB ^ vecA;
		sideA = ((cross > 0) ? RIGHT : LEFT);

		pc = in_arc.CenterPt();
		vec.Init( (*p1) - pc );
		dist = vec.Length();
		radius = in_arc.Radius();
		if (dist < radius)
			sideB = ((in_arc.Dir() == CCW) ? LEFT : RIGHT);
		else
			sideB = ((in_arc.Dir() == CCW) ? RIGHT : LEFT);

		CGeoCurve* offsetA = in_line.Offset( sideA, in_radius );
		CGeoCurve* offsetB = in_arc.Offset( sideB, in_radius );

		if (offsetA == NULL && offsetB == NULL)
		{
			CReturn status;
			status.Internal( IDS_INTERNAL_ERROR, "CSolution::blend_line_arc()" );
		}
		else
		{
			CInt2d intersector( SMALL, FALSE );
			nSoln = intersector.CrvCrv( (*offsetA), (*offsetB) );

			for (int indx = 0; indx < nSoln; ++indx)
			{
				const C2dCoord& pt = intersector.Point( indx );
				io_pt[indx].X( pt.X() );
				io_pt[indx].Y( pt.Y() );
				io_pt[indx].Z( 0.0 );	// TODO:  find actual Z? 
			}
		}

		delete offsetA;
		delete offsetB;

		return nSoln;
	}
	else
	{
		// The arc may be zero radius, for the line_pnt solution
		//
		// Create two parallel lines, offset by the blend radius
		//
		CGeoLine*	off_line[2];

		off_line[0] = (CGeoLine*) in_line.Offset( +1, in_radius );
		off_line[1] = (CGeoLine*) in_line.Offset( -1, in_radius );

		// Now, intersect these lines with the circle's offsets...
		const C3dCoord& ct_pt = in_arc.CenterPt();
		double		radius = in_arc.Radius();
		CGeoArc		off_arc[2];
		int			arc_num;

		arc_num = 0;
		off_arc[arc_num++].Init( C3dCoord( ct_pt.X() + radius + in_radius, ct_pt.Y(), 0.0 ),
											C3dCoord( ct_pt.X() + radius + in_radius, ct_pt.Y(), 0.0 ),
											ct_pt,
											0 );
	//	if (in_radius <= radius)
		{
			off_arc[arc_num++].Init( C3dCoord( ct_pt.X() + radius - in_radius, ct_pt.Y(), 0.0 ),
												C3dCoord( ct_pt.X() + radius - in_radius, ct_pt.Y(), 0.0 ),
												ct_pt,
												0 );
		}

		// ... there are four intersections to try, each of which
		// may return zero, one, or two hits.  Yeow!
		int	int_num;
		int	line_idx;
		int	arc_idx;

		int_num = 0;
		for (arc_idx=0; arc_idx<arc_num; arc_idx++)
		{
			for (line_idx=0; line_idx<2; line_idx++)
			{
				int_num += Intersect( off_arc[arc_idx], *off_line[line_idx], FALSE,  &io_pt[int_num] );
			}
		}

		delete off_line[0];
		delete off_line[1];

		return int_num;
	}

	// Issues of intersection points and arc directions
	// are left to the calling code... this does the hard part,
	// finding the arc centers.
	//
	//	The point on each entity can be found with PointClosest
	//	using the arc centers.
}

// ------------------------------------------------------------------

int CSolution::blend_arc_arc( 
	const CGeoArc&  in_arc1,	// First arc
	const CGeoArc&  in_arc2,	// Second arc
	double			in_radius,	// Blend radius
	const C2dCoord* p1,			// Optional reference point on in_arc1
	const C2dCoord* p2,			// Optional reference point on in_arc2
	C3dCoord*		io_pt )		// Array[8] to return solution centers
{
	if (p1 != NULL && p2 != NULL)
	{
		C2dCoord pc;
		C2dVec vec;
		int sideA, sideB;
		double dist, radius;
		int nSoln = 0;

		pc = in_arc1.CenterPt();
		vec.Init( (*p2) - pc );
		dist = vec.Length();
		radius = in_arc1.Radius();
		if (dist < radius)
			sideA = ((in_arc1.Dir() == CCW) ? LEFT : RIGHT);
		else
			sideA = ((in_arc1.Dir() == CCW) ? RIGHT : LEFT);

		pc = in_arc2.CenterPt();
		vec.Init( (*p1) - pc );
		dist = vec.Length();
		radius = in_arc2.Radius();
		if (dist < radius)
			sideB = ((in_arc2.Dir() == CCW) ? LEFT : RIGHT);
		else
			sideB = ((in_arc2.Dir() == CCW) ? RIGHT : LEFT);

		CGeoCurve* offsetA = in_arc1.Offset( sideA, in_radius );
		CGeoCurve* offsetB = in_arc2.Offset( sideB, in_radius );

		if (offsetA == NULL && offsetB == NULL)
		{
			CReturn status;
			status.Internal( IDS_INTERNAL_ERROR, "CSolution::blend_arc_arc()" );
		}
		else
		{
			CInt2d intersector( SMALL, FALSE );
			nSoln = intersector.CrvCrv( (*offsetA), (*offsetB) );

			for (int indx = 0; indx < nSoln; ++indx)
			{
				const C2dCoord& pt = intersector.Point( indx );
				io_pt[indx].X( pt.X() );
				io_pt[indx].Y( pt.Y() );
				io_pt[indx].Z( 0.0 );	// TODO:  find actual Z? 
			}
		}

		delete offsetA;
		delete offsetB;

		return nSoln;
	}
	else
	{
		// One arc may be zero radius, for the arc_pnt solution.
		// Probably shouldn't have both be zero radius, since this
		// is the *hard* way to find that solution ;-}
		//
		// Create many offset arcs...
		//
		const C3dCoord& ct_pt1 = in_arc1.CenterPt();
		const C3dCoord& ct_pt2 = in_arc2.CenterPt();
		double		radius1 = in_arc1.Radius();
		double		radius2 = in_arc2.Radius();
		CGeoArc		off_arc1[2];
		CGeoArc		off_arc2[2];
		int			arc1_num;
		int			arc2_num;

		arc1_num = 0;
		off_arc1[arc1_num++].Init( C3dCoord( ct_pt1.X() + radius1 + in_radius, ct_pt1.Y(), 0.0 ),
											C3dCoord( ct_pt1.X() + radius1 + in_radius, ct_pt1.Y(), 0.0 ),
											ct_pt1,
											0 );
	//	if (in_radius <= radius1)
		{
			off_arc1[arc1_num++].Init( C3dCoord( ct_pt1.X() + radius1 - in_radius, ct_pt1.Y(), 0.0 ),
												C3dCoord( ct_pt1.X() + radius1 - in_radius, ct_pt1.Y(), 0.0 ),
												ct_pt1,
												0 );
		}

		arc2_num = 0;
		off_arc2[arc2_num++].Init( C3dCoord( ct_pt2.X() + radius2 + in_radius, ct_pt2.Y(), 0.0 ),
											C3dCoord( ct_pt2.X() + radius2 + in_radius, ct_pt2.Y(), 0.0 ),
											ct_pt2,
											0 );
	//	if (in_radius <= radius2)
		{
			off_arc2[arc2_num++].Init( C3dCoord( ct_pt2.X() + radius2 - in_radius, ct_pt2.Y(), 0.0 ),
												C3dCoord( ct_pt2.X() + radius2 - in_radius, ct_pt2.Y(), 0.0 ),
												ct_pt2,
												0 );
		}

		//
		// ... there are up to four intersections to try, each of which
		// may return zero, one, or two hits.  Yeow!
		//
		int	int_num;
		int	arc1_idx;
		int	arc2_idx;

		int_num = 0;
		for (arc1_idx=0; arc1_idx<arc1_num; arc1_idx++)
		{
			for (arc2_idx=0; arc2_idx<arc2_num; arc2_idx++)
			{
				int_num += Intersect( off_arc1[arc1_idx], off_arc2[arc2_idx], FALSE, &io_pt[int_num] );
			}
		}

		return int_num;
	}
	// Issues of intersection points and arc directions
	// are left to the calling code... this does the hard part,
	// finding the arc centers.
	//
	//	The point on each entity can be found with PointClosest
	//	using the arc centers.

}


// ==================================================================================
// Convert a line from two-point form to ax + by + c = 0
//
//	TODO:  Move into CGeoLine?  Is this used anywhere else?
//
void CSolution::line_coeff( 
	const CGeoLine&	in_line, 
	double*		out_a, 
	double*		out_b, 
	double*		out_c )
{
	*out_a = in_line.EndPt().Y() - in_line.StartPt().Y();
	*out_b = in_line.StartPt().X() - in_line.EndPt().X();
	*out_c = (in_line.EndPt().X() * in_line.StartPt().Y()) - (in_line.StartPt().X() * in_line.EndPt().Y());

	return;
}

// ==================================================================================
// Get the perp. distance from a line to a point
//
//	TODO: This exists, sort of, in CGeoLine... this is just a handier form
//			for a solution here.
//
double CSolution::line_coeff_dist( 
	const C2dCoord&	in_pnt, 
	double		in_a, 
	double		in_b, 
	double		in_c )
{
	double	dist;

	dist = sqrt( in_a*in_a + in_b*in_b );
	if (EQUAL( dist, 0.0 ))
		return 0.0;

	return (in_a*in_pnt.X() + in_b*in_pnt.Y() + in_c) / dist;
}


// ==================================================================
//	Create a line tangent to two arcs.... like Blend, but with a line
//
//	One of the curves may be a zero-radius circle, as a fixed
//	point.
//
//	This may generate up to 4 solution sets, given as start and end
//	points.  
// TODO: return as CGeoLine instead of st/en pt??
//
int CSolution::TangentLine( 
	const CGeoArc& in_one,		// First arc
	const CGeoArc& in_two,		// Second arc
	C3dCoord* io_st_pt,			// Array[4] of start points
	C3dCoord* io_en_pt )			// Array[4] of end points
{
	int	nsoln = 0;

	// Some start and end points may be redundant, but their
	//	combinations should be unique... though with a degenerate
	//	arc, there may be combination redundancies.
	//
	const C3dCoord&	ctr1 = in_one.CenterPt();
	const C3dCoord&	ctr2 = in_two.CenterPt();

	double	radius1 = in_one.Radius();
	double	radius2 = in_two.Radius();
	double	dr = fabs( radius2 - radius1 );

	C2dVec	ctr_vec = (ctr1 - ctr2);
	double	ctr_dst = ctr_vec.Length();

	if (ctr_dst >= SMALL)
	{
		double	cross_dst;
		double	in_tan;
		double	out_tan;
		double	ratio;
		double	ctr_ang;
		
		ctr_ang = ((C2dUnitVec)ctr_vec).Radians();
		
		// Solutions meeting at the outside of the circles...
		//
		ratio = (radius2 - radius1) / ctr_dst;
		if (fabs(ratio) < (1. + SMALL))
		{
			// Because acos() is not tolerant of small errors at its boundaries.
			if (EQUAL( ratio, 1.0 ))
				out_tan = 0.0;
			else
			if (EQUAL( ratio, -1.0))
				out_tan = PI;
			else
				out_tan = acos( ratio );

			io_st_pt[0].X( ctr1.X() + radius1*cos( ctr_ang + out_tan ) );
			io_st_pt[0].Y( ctr1.Y() + radius1*sin( ctr_ang + out_tan ) );
			io_st_pt[0].Z( ctr1.Z() );

			io_en_pt[0].X( ctr2.X() + radius2*cos( ctr_ang + out_tan ) );
			io_en_pt[0].Y( ctr2.Y() + radius2*sin( ctr_ang + out_tan ) );
			io_en_pt[0].Z( ctr2.Z() );

			io_st_pt[1].X( ctr1.X() + radius1*cos( ctr_ang - out_tan ) );
			io_st_pt[1].Y( ctr1.Y() + radius1*sin( ctr_ang - out_tan ) );
			io_st_pt[1].Z( ctr1.Z() );

			io_en_pt[1].X( ctr2.X() + radius2*cos( ctr_ang - out_tan ) );
			io_en_pt[1].Y( ctr2.Y() + radius2*sin( ctr_ang - out_tan ) );
			io_en_pt[1].Z( ctr2.Z() );

			nsoln += 2;
		}

		cross_dst = ctr_dst * (radius1 / (radius1 + radius2));
		ratio = radius1 / cross_dst;

		if (fabs(ratio) < (1. + SMALL))
		{
			// Because acos() is not tolerant of small errors at its boundaries.
			if (EQUAL( ratio, 1.0 ))
				in_tan = 0.0;
			else
			if (EQUAL( ratio, -1.0))
				in_tan = PI;
			else
				in_tan = acos( ratio );

			io_st_pt[2].X( ctr1.X() + radius1*cos( ctr_ang - in_tan + PI ) );
			io_st_pt[2].Y( ctr1.Y() + radius1*sin( ctr_ang - in_tan + PI ) );
			io_st_pt[2].Z( ctr1.Z() );

			io_en_pt[2].X( ctr2.X() + radius2*cos( ctr_ang - in_tan ) );
			io_en_pt[2].Y( ctr2.Y() + radius2*sin( ctr_ang - in_tan ) );
			io_en_pt[2].Z( ctr2.Z() );

			io_st_pt[3].X( ctr1.X() + radius1*cos( ctr_ang + in_tan + PI ) );
			io_st_pt[3].Y( ctr1.Y() + radius1*sin( ctr_ang + in_tan + PI ) );
			io_st_pt[3].Z( ctr1.Z() );

			io_en_pt[3].X( ctr2.X() + radius2*cos( ctr_ang + in_tan ) );
			io_en_pt[3].Y( ctr2.Y() + radius2*sin( ctr_ang + in_tan ) );
			io_en_pt[3].Z( ctr2.Z() );

			nsoln += 2;
		}
	}

	return nsoln;
}

// ==================================================================
// Gives 2d solution in XY plane with pc.Z = ps.Z
//
int CSolution::ArcCenter(
				const C3dCoord& ps,
				const C3dCoord& pe,
				double radius,
				int dir,
				double radians,  // included angle
				C3dCoord* pc )
{
	bool halfCircle = FALSE;

	// Get the chord and chord length between the arc end points.
	double dx = pe.X() - ps.X();
	double dy = pe.Y() - ps.Y();

	double len = 0.5 * sqrt( dx*dx + dy*dy );
	if (fabs( radius - len ) < (1.001 * SMALL))
	{
		// super fudge! (slip#123?)
		radius = len;
		halfCircle = TRUE;
	}
	else if (radius < len)
	{
		if (fabs( radians - PI ) < 1.e-4)
			halfCircle = TRUE;
		else
			return 0;  // No solution.
	}

	// Get the mid-point of the chord.
	C2dCoord pm( (ps.X() + 0.5 * dx), (ps.Y() + 0.5 * dy) );

	if ( halfCircle )
	{
		pc->X( pm.X() );
		pc->Y( pm.Y() );
	}
	else
	{
		if (radians > PI)
		{
			// Flipping the chord direction ensures that the circle
			// center solution will be on the correct side of the chord.

			dx = -dx;
			dy = -dy;
		}

		C2dUnitVec uVec( dx, dy );

		// Get the radial vector through mid-point and arc center.
		uVec += (dir * HALFPI);

		// Get the distance from the mid-point to the arc center.
		double dst = sqrt( radius*radius - len*len );

		// Calculate the center position.
		C2dCoord pc2d = pm + (uVec * dst);

		pc->X( pc2d.X() );
		pc->Y( pc2d.Y() );
	}

	pc->Z( ps.Z() );

	return 1;
}

// ============================================================================
//	Calculate profile area.  Only works correctly for closed profiles.
//
//		Math reference:  (CRC) Standard Mathematic Tables & Formulae 
//
//		The area of a closed linear profile can be found by:
//			1/2 sum of (1<=i<=k) X[i]*Y[i+1] - X[i+1]*Y[i]
//
//		With arc sections, we need to adjust +- by the arc-cap area:
//			1/2 R^2(ang - sin(ang)) gives the cap area.
//
//		If the area is positive, the profile winds CCW, else it winds CW
//
double CSolution::Area( const CGeoCurveArray& curves )
{
	C2dCoord ptA;
	C3dCoord ptB;
	CGeoCurve* curve;
	CGeoArc* arc;

	int count = curves.Count();
	if (count == 1)
	{
		arc = dynamic_cast<CGeoArc*>( curves[0] );
		if (arc != NULL)
		{
			ptA = arc->StartPt();
			ptB = arc->EndPt();
			if ( ptA.WithinTol( ptB, SMALL ) )
			{
				double radius = arc->Radius();
				return (arc->Dir() * PI * radius * radius);
			}
		}

	}


	double area, rad, angle;
	double totalArea = 0.0;

	for (int indx = 0; indx < count; ++indx)
	{
		curve = curves[ indx ];

		ptA = curve->StartPt();
		ptB = curve->EndPt();

		switch (curve->Type())
		{
		case GEOLINE:
			area = (ptA.X() * ptB.Y()) - (ptB.X() * ptA.Y());
			totalArea += area;
			break;

		case GEOARC:
			arc = dynamic_cast<CGeoArc*>( curve );

			// Triangular area, across arc's chord
			area = (ptA.X() * ptB.Y()) - (ptB.X() * ptA.Y());
			totalArea += area;

			// Adjust for curved portion...
			rad   = arc->Radius();
			angle = arc->IncludedAngle();
			area  = rad * rad * (angle - sin(angle));

			totalArea += (arc->Dir() * area);
			break;
		}
	}

	return totalArea / 2.0;
}

