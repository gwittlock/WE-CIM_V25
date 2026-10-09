
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "2dVec.h"
#include "2dUnitVec.h"
#include "2dCoord.h"
#include "3dCoord.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "Int2d.h"

static bool ResultCheck( const CGeoCurve& curve, const C2dCoord& pt, double tol, double* u );

bool g_tangent;	// Global because the work is done outside of the object for some reason,
				// and I don't want to mess with the interface. 

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CInt2d::CInt2d()
	: m_tol( SMALL ),
	  m_parallel_tol( VECTOR_SMALL ),
	  m_onSeg( TRUE ),
	  m_count( 0 ),
	  m_tangent( FALSE )
{
}

CInt2d::CInt2d( double tol, bool onSeg )
	: m_tol( tol ),
	  m_onSeg( onSeg ),
	  m_parallel_tol( VECTOR_SMALL ),
	  m_count( 0 ),
	  m_tangent( FALSE )
{
}

CInt2d::~CInt2d()
{
}

void
CInt2d::Tol( double tol )
{
	m_tol = tol;
}

void
CInt2d::ParallelTol( double tol )
{
	m_parallel_tol = tol;
}

void
CInt2d::OnSeg( bool active )
{
	m_onSeg = active;
}

int
CInt2d::CrvCrv( const CGeoCurve& curveA, const CGeoCurve& curveB )
{
	Int2dCurveCurve( curveA, curveB, m_tol, m_parallel_tol, m_onSeg, m_pt, m_u, m_v, &m_count );
	m_tangent = g_tangent;
	return m_count;
}

int
CInt2d::SegSeg( const CGeoLine& lineA, const CGeoLine& lineB )
{
	Int2dSegSeg( lineA, lineB, m_tol, m_parallel_tol, m_onSeg, m_pt, m_u, m_v, &m_count );
	m_tangent = g_tangent;
	return m_count;
}

int
CInt2d::ArcSeg( const CGeoArc& arcA, const CGeoLine& lineB )
{
	Int2dArcSeg( arcA, lineB, m_tol, m_onSeg, m_pt, m_u, m_v, &m_count );
	m_tangent = g_tangent;
	return m_count;
}

int
CInt2d::ArcArc( const CGeoArc& arcA, const CGeoArc& arcB )
{
	Int2dArcArc( arcA, arcB, m_tol, m_onSeg, m_pt, m_u, m_v, &m_count );
	m_tangent = g_tangent;
	return m_count;
}

int
CInt2d::Count() const
{
	return m_count;
}

const C2dCoord&
CInt2d::Point( int indx ) const
{
	return m_pt[indx];
}

double
CInt2d::Uparam( int indx ) const
{
	return m_u[indx];
}

double
CInt2d::Vparam( int indx ) const
{
	return m_v[indx];
}

bool
CInt2d::Tangent() const
{ return m_tangent; }

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void Int2dCurveCurve(
		const CGeoCurve& curveA,
		const CGeoCurve& curveB,
		double tol,
		double parallel_tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln )
{
	(*nSoln) = 0;

	if (curveA.Type() == GEOLINE)
	{
		if (curveB.Type() == GEOLINE)
		{
			Int2dSegSeg( (CGeoLine&) curveA, (CGeoLine&) curveB, tol, parallel_tol, onSegment, pt, uA, uB, nSoln );
		}
		else if (curveB.Type() == GEOARC)
		{
			Int2dArcSeg( (CGeoArc&) curveB, (CGeoLine&) curveA, tol, onSegment, pt, uB, uA, nSoln );
		}
	}
	else if (curveA.Type() == GEOARC)
	{
		if (curveB.Type() == GEOLINE)
		{
			Int2dArcSeg( (CGeoArc&) curveA, (CGeoLine&) curveB, tol, onSegment, pt, uA, uB, nSoln );
		}
		else if (curveB.Type() == GEOARC)
		{
			Int2dArcArc( (CGeoArc&) curveA, (CGeoArc&) curveB, tol, onSegment, pt, uA, uB, nSoln );
		}
	}
}

void Int2dSegSeg(
		const CGeoLine& lineA,
		const CGeoLine& lineB,
		double tol,
		double parallel_tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln )
{
	bool okay = TRUE;

	(*nSoln) = 0;
	g_tangent = FALSE;

	const C2dCoord& psA = lineA.StartPt();
	const C2dCoord& peA = lineA.EndPt();
	const C2dCoord& psB = lineB.StartPt();
	const C2dCoord& peB = lineB.EndPt();

	C2dVec vecA = peA - psA;
	C2dVec vecB = peB - psB;

	double det = vecA.X() * vecB.Y() - vecA.Y() * vecB.X();
	if (fabs( det ) < parallel_tol)
		return;  // parallel lines

	double numerator = vecB.X() * (psA.Y() - psB.Y()) - vecB.Y() * (psA.X() - psB.X());
	double numeratorB = vecA.X() * (psB.Y() - psA.Y()) - vecA.Y() * (psB.X() - psA.X());

	double uparam = numerator / det;
	double uparamB = -numeratorB / det;

	C2dCoord intersection = psA + (vecA * uparam);

	if ( onSegment )
	{
		okay = ((uparam > -VECTOR_SMALL && uparam < (1.0 + VECTOR_SMALL)) &&
				(uparamB > -VECTOR_SMALL && uparamB < (1.0 + VECTOR_SMALL)));
	}

	if ( okay )
	{
		pt[0] = intersection;
		uA[0] = uparam;
		uB[0] = uparamB;

		++(*nSoln);
	}
}

// TODO: Improve efficiency by eliminating some sqrt()s.
void Int2dArcSeg(
		const CGeoArc& arc,
		const CGeoLine& line,
		double tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln )
{
	(*nSoln) = 0;
	g_tangent = FALSE;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//        Trivial rejection.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	C3dCoord ptClosest;
	double u, v;

	double radius = arc.Radius();
	if (radius < SMALL)
		return;

	double dist = line.PointClosest( arc.CenterPt(), &ptClosest, &u );
	if (dist > (radius + tol))
		return;

	// NOTE: This "optimization" is intended to reduce the solution
	// to a single intersection.  Though this may cause problems,
	// removing it causes problems too (when offsetting a chain).
	//
	// TODO: Find better way of classifying intersection at
	// near-tangent condition.
	//
	if (fabs( radius - dist ) < SMALL)
	{ 
		dist = radius;
		g_tangent = TRUE;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//       On to the solutions.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	C2dCoord soln[2];

	double lenSqrd = radius*radius - dist*dist;

	double len = ((lenSqrd < (tol*tol)) ? 0.0 : sqrt(lenSqrd));

	C2dUnitVec vec = line.StartTan();

	soln[0] = C2dCoord(ptClosest) + (vec * len);
	soln[1] = C2dCoord(ptClosest) + (vec * -len);


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Filter the solutions.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool okay, okayA, okayB;

	for (int indx = 0; indx < 2; ++indx)
	{
		// TODO: Measure should be in Cartesian space, instead of parameter space.
		dist = arc.PointClosest( soln[indx], &ptClosest, &u );
		dist = line.PointClosest( soln[indx], &ptClosest, &v );

		okay = TRUE;

		if ( onSegment )
		{
			okayA = ResultCheck( arc, ptClosest, tol, &u );
			okayB = ResultCheck( line, ptClosest, tol, &v );
			okay = (okayA && okayB);
		}

		if ( okay )
		{
			if (indx == 0 || (indx == 1 && !soln[1].WithinTol( soln[0], tol )) )
			{
				pt[ (*nSoln) ] = soln[indx];
				uA[ (*nSoln) ] = u;
				uB[ (*nSoln) ] = v;
				++(*nSoln);
			}
		}
	}
}

void Int2dArcArc(
		const CGeoArc& arcA,
		const CGeoArc& arcB,
		double tol,
		bool onSegment,
		C2dCoord* pt,
		double* uA,
		double* uB,
		int* nSoln )
{
	// First and foremost.
	(*nSoln) = 0;
	g_tangent = FALSE;


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//                Setup.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	C2dCoord centerA = arcA.CenterPt();
	C2dCoord centerB = arcB.CenterPt();

	double radiusA = arcA.Radius();
	if (radiusA < tol)
	{ radiusA = 0.0; }

	double radiusB = arcB.Radius();
	if( radiusB < tol )
	{ radiusB = 0.0; }

	C2dVec vec = centerB - centerA;

	double centerDist = vec.Length();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//        Trivial rejection.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (centerDist < tol)
	{
		// Coincident centers.
		if (fabs( radiusA - radiusB ) < tol)
			return;  // Coincident circles.
		else
			return;  // No intersection.
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//       On to the solutions.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	double delta, dx, dy, dr, x, y;

	C2dCoord soln[2];
	int count = 0;

	// Unitize the vector.
	vec *= (1 / centerDist);

	delta = centerDist - (radiusA + radiusB);

	if (delta > tol)
	{
		// Externally separated.
		count = 0;
	}
	else if (fabs( delta ) < tol)
	{
		// Externally tangent.
		soln[0] = centerA + (vec * radiusA);
		count = 1;
		g_tangent = TRUE;
	}
	else
	{
		dr = fabs( radiusA - radiusB );
		delta = dr - centerDist;

		if (delta > tol)
		{
			// Internally separated.
			return;
		}
		else if (fabs( delta ) < (0.1 * tol))
		{
			// TODO: With V13, the term (tol) was changed to (0.1 * tol) to
			// deal with CAD data where two adjacent arcs really share the
			// same center and have the same radius, but they're really not
			// tangent.  Is there a better way?

			if (radiusB < radiusA)
			{
				// Internally tangent
				// 2nd circle interior
				soln[0] = centerA + (vec * radiusA);
			}
			else
			{
				// radiusA==radiusB implies |centerDist|==0, a case treated above
				// 1st circle interior
				soln[0] = centerA - (vec * radiusA);
			}
			count = 1;
		}
		else
		{
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			//           Two point solution.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			// Local "x" for int pts; along center's line
			dx = 0.5*(centerDist*centerDist + radiusA*radiusA - radiusB*radiusB) / centerDist;

			// Local "y" for int pts; along normal to cent line
			dy = radiusA*radiusA - dx*dx;

			if (dy < 0.0)
			{
				// Imaginary roots, no intersection.
				// Should never get this far.
			}
			else
			{
				dy = sqrt( dy );

				x = centerA.X() + dx * vec.X() - dy * vec.Y();
				y = centerA.Y() + dx * vec.Y() + dy * vec.X();
				soln[0].XY( x, y );

				x = centerA.X() + dx * vec.X() + dy * vec.Y();
				y = centerA.Y() + dx * vec.Y() - dy * vec.X();
				soln[1].XY( x, y );
				count = 2;
			}
		}
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Filter the solutions.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	C3dCoord ptClosest;
	double dist, u, v;
	bool okay;

	for (int indx = 0; indx < count; ++indx)
	{
		dist = arcA.PointClosest( soln[indx], &ptClosest, &u );
		dist = arcB.PointClosest( soln[indx], &ptClosest, &v );

		okay = TRUE;

		if ( onSegment )
		{
			okay = ((u > -VECTOR_SMALL && u < (1.0 + VECTOR_SMALL)) &&
					(v > -VECTOR_SMALL && v < (1.0 + VECTOR_SMALL)));
		}

		if ( okay )
		{
			if (indx == 0 || (indx == 1 && !soln[1].WithinTol( soln[0], tol )) )
			{
				pt[ (*nSoln) ] = soln[indx];
				uA[ (*nSoln) ] = u;
				uB[ (*nSoln) ] = v;
				++(*nSoln);
			}
		}
	}

	return;
}

bool ResultCheck( const CGeoCurve& curve, const C2dCoord& pt, double tol, double* u )
{
	bool okay = FALSE;

	if ((*u) > -VECTOR_SMALL && (*u) < (1.0 + VECTOR_SMALL))
	{
		// The intersection point is (without a doubt?) on the curve.
		okay = TRUE;
	}
	else
	{
		// Otherwise, we must check for virtual a intersection.

		if ((*u) <= -VECTOR_SMALL)
		{
			okay = pt.WithinTol( curve.StartPt(), SMALL );
			if ( okay )
				(*u) = 0.0;
		}
		else if ((*u) >= (1.0 + VECTOR_SMALL))
		{
			okay = pt.WithinTol( curve.EndPt(), SMALL );
			if ( okay )
				(*u) = 1.0;
		}
	}

	return okay;
}
