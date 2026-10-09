
#include "StdAfx.h"
#include "MathConst.h"
#include "QuickHull.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  2008.08.29 (PE) -- CQuickHull() is implemented using the STL vector class. At
//  first implementation, wherever we loop over points in the vector, we get the
//  upper bound as "count = (int) vec.size()". This is a potential problem because
//  it is possible (though unlikely?) for the count of elements exceed INT_MAX.
//
//  2008.09.03 (PE) -- We could choose to place the calls to DestructiveClear()
//  in a manner that allows the algorithm to recover memory more regularly (as
//  opposed to waiting for the call-stack to unwind).
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// The original (java) source for this algorithm was found at:
//    http://www.piler.com/convexhull/ConvexHull.java
//
// A local copy of this file can be found at:
//    http://smartcamcnc.loc/mrsdoc/Development/docs/QuickHull.java
//
// There was no copyright included with the source. However, let's
// give credit where credit is do:
//    author: Jeff C. So
//    contact: jcs@piler.com


/* Courtesy of Alejo HausnerFrom, an explanation of the algorithm at:
 *    http://www.cs.princeton.edu/~ah/alg_anim/version1/QuickHull.html
 *
 * Note; Alternate algorithms (Graham's scan & Jarvis's march) are at:
 *    http://www.cs.princeton.edu/~ah/alg_anim/version1/ConvexHull.html
 *
 * Here's an algorithm that deserves its name. It's a fast way to compute the
 * convex hull of a set of points on the plane. It shares a few similarities
 * with its namesake, QuickPartition-sort:
 *
 *  * it is recursive
 *  * each recursive step partitions data into several groups.
 *
 * The partitioning step does all the work. The basic idea is as follows:
 *
 * 1. We are given a some points, and line segment AB which we know is a
 *    chord of the convex hull (IE, it's endpoints are known to be on the
 *    convex hull). A good chord to start the algorithm goes from the
 *    leftmost to the rightmost point in the set.
 * 2. Among the given points, find the one which is farthest from AB. Let's
 *    call this point C.
 * 3. The points inside the triangle ABC cannot be on the hull. Put them in
 *    set s0.
 * 4. Put the points which lie outside edge AC in set s1, and points outside
 *    edge BC in set s2. 
 *
 * Once the partitioning is done, we recursively invoke QuickPartition-hull on sets
 * s1 and s2. The algorithm works fast on random sets of points because
 * step 3 of the partition typically discards a large fraction of the points.
 */

static void VectorReverse( Tchvector* vec );

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CQuickHull::CQuickHull()
{
}

CQuickHull::~CQuickHull()
{
	m_hull.clear();
}

// Quick Hull Algorithm implementation.
void
CQuickHull::QuickHull( const Tchvector& points, bool ccw, Tchvector* hull )
{
	Tchvector P1;
	Tchvector P2;
	const C2dCoord* l = points[0];
	const C2dCoord* r = points[0];

	double minX = l->X();
	double maxX = l->X();
	double minAt = 0;
	double maxAt = 0;	
	
	int count, indx, jndx;

	m_hull.clear();
	
	// Find the max and min x-C2dCoord point.
	count = (int) points.size();
	for (indx = 1; indx < count; indx++)
	{
		if (points[indx]->X() > maxX)
		{
			r = points[indx];
			maxX = points[indx]->X();
			maxAt = indx;
		};
		
		if (points[indx]->X() < minX)
		{
			l = points[indx];
			minX = points[indx]->X();
			minAt = indx;
		};
	}
	
	// The partition boundary (which trends left to right).
	chline lr( l, r );
	
	// Build two point sets. The upper set P1 is the set of all points
	// that are to the left of 'lr' and the lowe set P2 is the set of
	// all points that are to the right of 'lr'.
	count = (int) points.size();
	for (indx = 0; indx < count; indx++)
	{
		if ((indx != maxAt) && (indx != minAt))
		{
			const C2dCoord* pi = points[indx];

			EChSide side = lr.WhichSide( *pi );
			if (side == SLEFT)
				P1.push_back( new C2dCoord( *pi ) );
			else if (side == SRIGHT)
				P2.push_back( new C2dCoord( *pi ) );
		}
	};

	// Add the extreme points to the end of the point set (adding
	// the points to the end is a requirement of QuickPartition()).
	P1.push_back( new C2dCoord( *l ) );
	P1.push_back( new C2dCoord( *r ) );

	// Calculate the upper hull (it should have a CW winding).
	QuickPartition( P1, l, r, 0 );
	
	// Move the upper hull to the result.
	count = (int) m_hull.size();
	for (jndx = 0; jndx < count; ++jndx)
	{
		HullPointAppend( m_hull[jndx], hull );
	}		
	
	// Recover memory.
	DestructiveClear( &m_hull );
	
	// Add the extreme points to the end of the point set (adding
	// the points to the end is a requirement of QuickPartition()).
	P2.push_back( new C2dCoord( *l ) );
	P2.push_back( new C2dCoord( *r ) );

	// Calculate the lower hull (it should have a CCW winding).
	QuickPartition( P2, l, r, 1 );
	
	// Move the lower hull to the result.
	count = (int) m_hull.size();
	for (jndx = (count - 1); jndx >= 0; --jndx)
	{
		HullPointAppend( m_hull[jndx], hull );
	}
	
	// Recover memory.
	DestructiveClear( &m_hull );
	DestructiveClear( &P1 );
	DestructiveClear( &P2 );

	// CRITICAL: sum_convex_polyarcs() (the original client of the convex
	// hull created here) requires the polyline to have CCW winding.
	// Otherwise, it seems to get stuck in an infinite loop.
	if ( ccw )
	{
		// NOTE: An alternate (more efficient but more complicated) solution
		// is to restructure the quick hull code so that it builds the
		// convex hull with the correct winding (instead of correcting the
		// winding post-facto as we do here).
		VectorReverse( hull );
	}
}


/* Recursive method to find out the Hull.
 * faceDir is 0 if we are calculating the upper hull.
 * faceDir is 1 if we are calculating the lower hull.
 */
// NOTE: This method is a prime candidate for multi-threading.
void
CQuickHull::QuickPartition( const Tchvector& P, const C2dCoord* l, const C2dCoord* r, int faceDir )
{
	if (P.size() == 2)
	{
		HullPointAppend( P[0], &m_hull );
		HullPointAppend( P[1], &m_hull );
	}
	else
	{
		Tchvector P1;
		Tchvector P2;

		int hAt = QuickExtreme( P, l, r );  // ie. Step 2.
		
		chline lh( l, P[hAt] );
		chline hr( P[hAt], r );
		
		for (int i = 0; i < (int) (P.size() - 2); i++)
		{
			if (i != hAt)
			{
				const C2dCoord* pi = P[i];

				if (faceDir == 0)
				{
					if (lh.WhichSide( *pi ) == SLEFT)
						P1.push_back( new C2dCoord( *pi ) );
					
					if (hr.WhichSide( *pi ) == SLEFT)
						P2.push_back( new C2dCoord( *pi ) );
				}
				else
				{
					if (lh.WhichSide( *pi ) == SRIGHT)
						P1.push_back( new C2dCoord( *pi ) );
					
					if (hr.WhichSide( *pi ) == SRIGHT)
						P2.push_back( new C2dCoord( *pi ) );
				}
			}
		}
		
		// Add the extreme points to the end of the point set (adding
		// the points to the end is a requirement of QuickPartition()).
		P1.push_back( new C2dCoord( *l ) );
		P1.push_back( new C2dCoord( *P[hAt] ) );
		
		// Add the extreme points to the end of the point set (adding
		// the points to the end is a requirement of QuickPartition()).
		P2.push_back( new C2dCoord( *P[hAt] ) );
		P2.push_back( new C2dCoord( *r ) );
				
		if (faceDir == 0)
		{
			QuickPartition( P1, l, P[hAt], 0 );
			QuickPartition( P2, P[hAt], r, 0 );
		}
		else
		{
			QuickPartition( P1, l, P[hAt], 1 );
			QuickPartition( P2, P[hAt], r, 1 );
		}

		// Recover memory.
		DestructiveClear( &P1 );
		DestructiveClear( &P2 );
	}
}

/* Find out a point which is in the Hull for sure among a group of points
 * Since all the point are on the same side of the line formed by l and r,
 * so the point with the longest distance perpendicular to this line is 
 * the point we are looking for.
 * Return the index of this point in the Vector/
 */
int
CQuickHull::QuickExtreme( const Tchvector& P, const C2dCoord* l, const C2dCoord* r )
{
	chline lr( l, r );
	double max_dist = 0;
	int max_indx = 0;
	
	for (int indx = 0; indx < (int) (P.size() - 2); ++indx)
	{
		double dist = lr.DistanceTo( *(P[indx]) );
		if (dist > max_dist)
		{
			max_dist = dist;
			max_indx = indx;
		}
	}
	
	return max_indx;
}

bool
CQuickHull::HullPointAppend( const C2dCoord* pt, Tchvector* hull )
{
	int count = (int) hull->size();

	// Prevent the accumulation of coincident points.
	bool append = (count == 0);
	if ( !append )
	{
		const C2dCoord* other = (*hull)[count-1];
		append = (pt->WithinTol( *other, SMALL ) == FALSE);
	}

	if ( append )
		hull->push_back( new C2dCoord( *pt ) );

	return append;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

chline::chline( const C2dCoord* ps, const C2dCoord* pe )
{
	m_ps = ps;
	m_pe = pe;

	m_vec.Init( (pe->X() - ps->X()), (pe->Y() - ps->Y()) );
}

EChSide
chline::WhichSide( const C2dCoord& pi ) const
{
	EChSide side = NEITHER;

	C2dUnitVec	vecB( (pi.X() - m_ps->X()), (pi.Y() - m_ps->Y()) );

	double cross = m_vec ^ vecB;
	if (fabs(cross) > 1.e-8)  // arbitrary tolerance
		side = ((cross > 0) ? SLEFT : SRIGHT);

	return side;
}

// As pilfered from closest_pt_to_line().
double
chline::DistanceTo( const C2dCoord& pi ) const
{
	// Calculate the magnitude^2 of the reference line.
	double dx0 = m_vec.X();
	double dy0 = m_vec.Y();

	double mag_squared = dx0*dx0 + dy0*dy0;

	// Create a vector from the start of the line to the reference point.
	double dx1 = Delta( pi.X(), m_ps->X() );
	double dy1 = Delta( pi.Y(), m_ps->Y() );

	double dot = dx0*dx1 + dy0*dy1;

	// Project the vector on to the reference line.
	double u0 = dot / mag_squared;

	// The closest point on the line segment (pe-ps).
	double xp = m_ps->X() + (u0 * dx0);
	double yp = m_ps->Y() + (u0 * dy0);

	// Calculate the distance between the reference and closest points.
	dx0 = xp - pi.X();
	dy0 = yp - pi.Y();

	double dist = sqrt( dx0*dx0 + dy0*dy0 );

	return dist;
}
// ASSUMPTION: If the ordinal values of two points on a
// given axis are within SMALL, the values are coincident.
double
chline::Delta( double a, double b ) const
{
	double diff = a - b;
	return ((fabs(diff) < SMALL) ? 0. : diff);
}

void DestructiveClear( Tchvector* vec )
{
	int count = (int) vec->size();
	for (int indx = 0; indx < count; ++indx)
	{
		delete (*vec)[indx];
	}

	vec->clear();
}

void VectorReverse( Tchvector* vec )
{
	int indxA = 0;
	int indxB = (int) (vec->size() - 1);

	while (indxA < indxB)
	{
		const C2dCoord* ptA = (*vec)[indxA];
		const C2dCoord* ptB = (*vec)[indxB];

		(*vec)[indxA] = ptB;
		(*vec)[indxB] = ptA;

		++indxA;
		--indxB;
	}
}
