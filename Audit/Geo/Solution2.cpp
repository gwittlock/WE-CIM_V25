
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"

#include "2dVec.h"
#include "2dUnitVec.h"
#include "Int2d.h"
#include "Solution.h"

// Finds the set of arcs through the given point and tangent to the
// given arc and line.  The solutions are a returned as a set of
// center points and radii.  A return value of zero indicates no
// solution.
int
CSolution::ArcPAL(
				  const C3dCoord&	refPt,
				  const CGeoArc&	refArc,
				  const CGeoLine&	refLine,
				  C3dCoord*			solnCenter,
				  double*			solnRadius )
{
	C3x4Matrix t1Xform;
	C3x4Matrix t2Xform;
	C3x4Matrix rXform;
	C3dCoord pt;
	C3dCoord ps;
	C3dCoord pc;

	double y0;				// the known point (x0 = 0)
	double x1, y1, z1, r1;	// the known circle
	double xc, yc;			// the unknown circle

	double a, b, c;			// quadratic terms
	double roots[2];		// quadratic solns
	double radians;
	int count, indx;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Transform the geometry such that the known point is
	// on the Y-axis and the known line is the X-axis.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	t1Xform.Shift( C3dVec( -refPt.X(), -refPt.Y(), 0. ) );
	t1Xform.TransformTo( refPt, &pt );
	t1Xform.TransformTo( refLine.StartPt(), &ps );
	t1Xform.TransformTo( refArc.CenterPt(), &pc );

	radians = refLine.StartTan().Radians();
	rXform.setXYAngle( -radians );
	rXform.Transform( &ps );
	rXform.Transform( &pc );

	t2Xform.Shift( C3dVec( 0., -ps.Y(), 0. ) );
	t2Xform.Transform( &pt );
	t2Xform.Transform( &pc );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Collect the terms required to generate the solution
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	x1 = pc.X();
	y1 = pc.Y();
	z1 = pc.Z();
	r1 = refArc.Radius();

	y0 = pt.Y();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Generate the solutions
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	int nsolns = 0;

	if (fabs(y0) < SMALL)
	{
		// The point is the on the line.

		if ((fabs(y1) - r1) < (2 * SMALL))
		{
			// Infinitely many solutions.
			return 0;
		}

		yc = ((x1 * x1) + (y1 * y1) - (r1 * r1)) / (2 * (r1 + y1));
		solnCenter[nsolns] = C3dCoord( 0., yc, z1 );
		solnRadius[nsolns] = fabs(yc);
		++nsolns;

		yc = ((r1 * r1) - (x1 * x1) - (y1 * y1)) / (2 * (r1 - y1));
		solnCenter[nsolns] = C3dCoord( 0., yc, z1 );
		solnRadius[nsolns] = fabs(yc);
		++nsolns;
	}
	else
	{
		a = y0 - y1 - (r1 * SGN(y0));
		b = -(2 * y0 * x1);
		c = y0 * ((x1 * x1) + (y1 * y1) - (y1 * y0) - (r1 * y0 * SGN(y0)) - (r1 * r1));

		count = quadratic_roots( a, b, c, roots );
		for (indx = 0; indx < count; ++indx)
		{
			xc = roots[indx];
			yc = (xc * xc) / (2 * y0) + (y0 / 2);

			solnCenter[nsolns] = C3dCoord( xc, yc, z1 );
			solnRadius[nsolns] = fabs( yc );
			++nsolns;
		}

		a = y0 -y1 + (r1 * SGN(y0));
		b = -(2 * y0 * x1);
		c = y0 * ((x1 * x1) + (y1 * y1) + (-y1 * y0) + (r1 * y0 * SGN(y0)) - (r1 * r1));

		count = quadratic_roots( a, b, c, roots );
		for (indx = 0; indx < count; ++indx)
		{
			xc = roots[indx];
			yc = (xc * xc) / (2 * y0) + (y0 / 2);

			solnCenter[nsolns] = C3dCoord( xc, yc, z1 );
			solnRadius[nsolns] = fabs( yc );
			++nsolns;
		}
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Transform the solutions back to model space.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (nsolns > 0)
	{
		C3x4Matrix t1Inverse;
		C3x4Matrix t2Inverse;
		C3x4Matrix rInverse;

		t1Xform.InvertTo( &t1Inverse );
		t2Xform.InvertTo( &t2Inverse );
		rXform.InvertTo( &rInverse );

		for (indx = 0; indx < nsolns; ++indx)
		{
			t2Inverse.Transform( &solnCenter[indx] );
			rInverse.Transform( &solnCenter[indx] );
			t1Inverse.Transform( &solnCenter[indx] );
		}
	}

	return nsolns;
}

int
CSolution::ArcTT(
				  const CGeoCurve&	refCurve1,
				  const CGeoCurve&	refCurve2,
				  double			rc,
				  C3dCoord*			solnCenter )
{
	int nsolns;

	if (refCurve1.Type() == GEOARC && refCurve2.Type() == GEOARC)
	{
		const CGeoArc& refArc1 = dynamic_cast<const CGeoArc&>( refCurve1 );
		const CGeoArc& refArc2 = dynamic_cast<const CGeoArc&>( refCurve2 );

		nsolns = CSolution::ArcTT( refArc1, refArc2, rc, solnCenter );
	}
	else
	{
		// nsolns = CSolution::Blend( refCurve1, refCurve2, rc, NULL, NULL, solnCenter );

		nsolns = 0;
		CSolution::ArcTT( refCurve1,  LEFT, refCurve2,  LEFT, rc, solnCenter, &nsolns );
		CSolution::ArcTT( refCurve1,  LEFT, refCurve2, RIGHT, rc, solnCenter, &nsolns );
		CSolution::ArcTT( refCurve1, RIGHT, refCurve2,  LEFT, rc, solnCenter, &nsolns );
		CSolution::ArcTT( refCurve1, RIGHT, refCurve2, RIGHT, rc, solnCenter, &nsolns );
	}

	return nsolns;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Finds the set of circles having the specified radius (rc) and that
// are tangent to the given circles.  Finds the solutions (xc,yc) by
// solving the simultaneous linear equations
//
//    (xc - x1)^2 + (yc - y1)^2 = (rc +/- r1)^2
//    (xc - x2)^2 + (yc - y2)^2 = (rc +/- r2)^2
//
//    where:
//
//       (x1,y1) -- center of first known circle
//       (x2,y2) -- center of second known circle
//       
int
CSolution::ArcTT(
				  const CGeoArc&	refArc1,
				  const CGeoArc&	refArc2,
				  double			rc,
				  C3dCoord*			solnCenter )
{
	C3x4Matrix t1Xform;
	C3x4Matrix rXform;
	C3dCoord pc1;
	C3dCoord pc2;
	C2dUnitVec uvec;
	C2dVec vec;

	double r1, r2, x2;

	pc1 = refArc1.CenterPt();
	pc2 = refArc2.CenterPt();

	r1 = refArc1.Radius();
	r2 = refArc2.Radius();

	vec = pc2 - pc1;
	if ((vec.Length() - r1 - r2) > (2 * rc))
		return 0;  // No solution, circles are too far apart.

	if (vec.Length() < SMALL)
		return 0;  // Infinitely many solutions.


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Transform the geometry such that pc1 is at the origin
	// and rotated such that pc2 is on the X-axis.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	t1Xform.Shift( C3dVec( -pc1.X(), -pc1.Y(), 0. ) );
	t1Xform.Transform( &pc1 );
	t1Xform.Transform( &pc2 );

	uvec.Init( vec.X(), vec.Y() );
	rXform.setXYAngle( -(uvec.Radians()) );
	rXform.Transform( &pc1 );
	rXform.Transform( &pc2 );

	x2 = pc2.X();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Generate the solutions
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	int nsolns = 0;

	ArcTT_roots( (rc + r2), (rc + r1), x2, solnCenter, &nsolns );
	ArcTT_roots( (rc + r2), (rc - r1), x2, solnCenter, &nsolns );
	ArcTT_roots( (rc - r2), (rc + r1), x2, solnCenter, &nsolns );
	ArcTT_roots( (rc - r2), (rc - r1), x2, solnCenter, &nsolns );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Transform the solutions back to model space.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (nsolns > 0)
	{
		C3x4Matrix t1Inverse;
		C3x4Matrix rInverse;

		t1Xform.InvertTo( &t1Inverse );
		rXform.InvertTo( &rInverse );

		for (int indx = 0; indx < nsolns; ++indx)
		{
			rInverse.Transform( &solnCenter[indx] );
			t1Inverse.Transform( &solnCenter[indx] );

			solnCenter[indx].Z( pc1.Z() );
		}
	}

	return nsolns;
}

int
CSolution::quadratic_roots( double a, double b, double c, double* root )
{
	if (fabs(a) < VECTOR_SMALL)
	{
		// When a is zero, ax^2 + bx + c = 0
		// reduces to bx + c = 0
		if (fabs(b) < VECTOR_SMALL)
			return 0;

		root[0] = (-c / b);
		return 1;  // no solution
	}

	double term = (b * b) - (4 * a * c);
	if (term < -SMALL)
		return 0;  // no solution

	term = ((term < 0) ? 0.0 : sqrt( term ));

	root[0] = (-b + term) / (2 * a);
	root[1] = (-b - term) / (2 * a);

	return ((fabs( root[1] - root[0] ) > SMALL) ? 2 : 1);
}

// TODO: Retain unique roots only?  May not be worth the bother.
void
CSolution::ArcTT_roots(
					double		term0,
					double		term1,
					double		term2,
					C3dCoord*	solnCenter,
					int*		nsolns )
{
	double xc, yc, term3;

	xc = ((term0 * term0) - (term1 * term1) - (term2 * term2)) / -(2 * term2);
	term3 = (term1 * term1) - (xc * xc);

	if (fabs(term3) < SMALL)
	{
		solnCenter[(*nsolns)] = C3dCoord( xc, 0.0, UNDEFINED );
		++(*nsolns);
	}
	else if (term3 > 0)
	{
		yc = sqrt(term3);
		solnCenter[(*nsolns)] = C3dCoord( xc, yc, UNDEFINED );
		++(*nsolns);
		solnCenter[(*nsolns)] = C3dCoord( xc, -yc, UNDEFINED );
		++(*nsolns);
	}
}

void
CSolution::ArcTT(
				const CGeoCurve&	refCurve1,
				int					dir1,
				const CGeoCurve&	refCurve2,
				int					dir2,
				double				rc,
				C3dCoord*			solnCenter,
				int*				nsolns )
{
	CGeoCurve* offsetA = refCurve1.Offset( dir1, fabs(rc) );
	CGeoCurve* offsetB = refCurve2.Offset( dir2, fabs(rc) );

	if (offsetA != NULL && offsetB != NULL)
	{
		CInt2d intersector( SMALL, FALSE );

		intersector.CrvCrv( (*offsetA), (*offsetB) );
		
		int count = intersector.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			solnCenter[(*nsolns)] = intersector.Point(indx);
			++(*nsolns);
		}
	}

	delete offsetA;
	delete offsetB;
}
