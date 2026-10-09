
#include "stdafx.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "GeoReducer.h"



////////////////////////////////////////////////////////////////////////


// Merge 'like' arcs.
// ASSUMPTION: Profile entities have proper order, direction and continuity.
void CGeoReducer::ArcsReduce( CGeoCurveArray& curves, bool wrap, double tol )
{
	CGeoArc* arcA;
	CGeoArc* arcB;
	bool reduce;
	int count, indxA, indxB;

	indxA = 0;
	while (1)
	{
		count = curves.Count();
		if (count < 2)
			break;  // complete reduction or no processing required.

		indxB = indxA + 1;
		if (indxB >= count)
		{
			if ( !wrap )
				break;

			// Boundary condition, cross-over from end to start.
			indxA = count - 1;
			indxB = 0;
		}

		arcA = dynamic_cast<CGeoArc*>( curves[indxA] );
		arcB = dynamic_cast<CGeoArc*>( curves[indxB] );

		reduce = FALSE;

		if (arcA != NULL && arcB != NULL)
		{
			const C3dCoord& centerA = arcA->CenterPt();
			const C3dCoord& centerB = arcB->CenterPt();

			if ( centerA.WithinTol( centerB, tol ) )
			{
				const C3dCoord& ptA = arcA->EndPt();
				const C3dCoord& ptB = arcB->StartPt();

				reduce = ptA.WithinTol( ptB, tol );
			}
		}

		if ( reduce )
		{
			// TODO: Replacement in this fashion is not sufficient.
			// TODO: Need to create the average arc.
			arcA->EndPt( arcB->EndPt() );
			delete curves.Remove( indxB );
		}
		else
		{
			++indxA;
		}

		if (indxB == 0)
			break;  // we've come full circuit
	}
}

// Colinear point reduction.
// ASSUMPTION: Profile entities have proper order and direction
// and are well behaved (as in not laying on one another).
//
// BugID 640 -- colinear point removal should not apply to wrap
// condition when offsetting profile for toolpath generation.
void
CGeoReducer::LinesReduce(  CGeoCurveArray& curves, bool wrap, double tol )
{
	C2dUnitVec tanA;
	C2dUnitVec tanB;
	C3dCoord closestPt;
	C3dCoord ptA;
	C3dCoord ptB;
	CGeoLine* lineA;
	CGeoLine* lineB;
	double dot, dist, uparam;
	int indxA, indxB;
	bool potentially_colinear;

	indxA = 0;
	while (1)
	{

		if (indxA >= curves.Count())
			break;

		// There reference line for colinear measurement.
		lineA = dynamic_cast<CGeoLine*>( curves[indxA] );

		potentially_colinear = false;

		if (lineA != NULL)
		{
			ptA = lineA->EndPt();
			tanA = lineA->StartTan();

			indxB = indxA + 1;
			while (1)
			{
				// Large and painful adjustment to allow last-to-first line merging
				// 23 Aug 2002 Edwin
				if (indxB > curves.Count())
					break;

				// The next line in a sequence of lines.
				if (indxB == curves.Count())
				{ 
					if (!wrap || !indxA)
						break;

					lineB = dynamic_cast<CGeoLine*>( curves[0] );
				}
				else
				{
					lineB = dynamic_cast<CGeoLine*>( curves[indxB] );
				}

				if (lineB == NULL)
					break;

				ptB = lineB->StartPt();
				if ( !ptB.WithinTol( ptA, tol ) )
					break;  // Break in C0 continuity

				tanB = lineB->StartTan();
				dot = tanA * tanB;

				if (dot < -VECTOR_SMALL)
					break;  // Assume a sharp turn or backing up

				if (dot < (1.0 - VECTOR_SMALL))
				{
					potentially_colinear = true;
					dist = lineA->PointClosest( lineB->EndPt(), &closestPt, &uparam );
					if (dist > tol)
						break;  // A break in colinear run.
				}

				ptA = lineB->EndPt();
				++indxB;
			}

			if (indxB > (indxA + 1))
			{
				// We encountered a run of colinear points.

				C3dCoord runEndPt;
				CGeoCurve* geoCurve;
				
				bool zero = false;
				if (indxB >= (curves.Count()+1))
				{ 
					geoCurve = dynamic_cast<CGeoCurve*>( curves[0] );
					runEndPt = geoCurve->EndPt();
					zero = true;
					indxB--;
				}
				else
				if (indxB >= curves.Count())
				{
					geoCurve = dynamic_cast<CGeoCurve*>( curves[indxB-1] );
					runEndPt = geoCurve->EndPt();
				}
				else
				{
					geoCurve = dynamic_cast<CGeoCurve*>( curves[indxB] );
					runEndPt = geoCurve->StartPt();
				}


				// Remove the lines [indxA+1 .. indxB-1] inclusive.
				while (indxB > (indxA + 1))
				{
					delete curves.Remove( indxB-1 );
					--indxB;
				}
				if (zero)
				{ 
					delete curves.Remove( 0 );
					--indxB;
				}

				lineA->EndPt( runEndPt );
				indxA = indxB;
			}
			else if ( potentially_colinear )
			{
				// 2014.04.26 (PE) -- Discovered while addressing a nesting problem for ITI.
				// The geometric configuration is arcA, lineB, lineC and lineB is *very* short.
				dist = lineB->PointClosest( lineA->StartPt(), &closestPt, &uparam );
				if (dist < 1.e-4)  // arbitrary tolerance (but based on likely machine accuracy)
				{
					lineB->StartPt( lineA->StartPt() );
					delete curves.Remove( indxB-1 );
					--indxB;
				}
				else
				{
					++indxA;
				}
			}
			else
			{
				++indxA;
			}
		}
		else
		{
			++indxA;
		}
	}
}

