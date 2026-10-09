
#include "stdafx.h"
#include <math.h>
#include "GeoArc.h"
#include "Solution.h"


int 
CSolution::line_ps_tan(
				const C3dCoord&	ps,
				const CGeoArc&	geoArc,
				const C3dCoord&	hint,
				C3dCoord*		pe )
{
	CReturn		status;
	C3dCoord	st_sol[4];
	C3dCoord	en_sol[4];
	CGeoArc		falseArc;
	double		dx, dy;
	double		mindist, dist;
	int			nsoln, indx, jndx;

	falseArc.Init( ps, 0., CCW );

	nsoln = CSolution::TangentLine( falseArc, geoArc, st_sol, en_sol );

	mindist = UNDEFINED;
	for (indx = 0; indx < nsoln; ++indx)
	{
		dx = hint.X() - en_sol[indx].X();
		dy = hint.Y() - en_sol[indx].Y();
		dist = sqrt( dx*dx + dy*dy );
		if (dist < mindist)
		{
			mindist = dist;
			jndx = indx;
		}

		pe->X( en_sol[jndx].X() );
		pe->Y( en_sol[jndx].Y() );
		pe->Z( ps.Z() );
	}

	return nsoln;
}


int
CSolution::line_tan_tan(
				const CGeoArc&	geoArcA,
				const C3dCoord&	hintA,
				const CGeoArc&	geoArcB,
				const C3dCoord&	hintB,
				C3dCoord*		ptA,
				C3dCoord*		ptB )
{
	C3dCoord	st_sol[4];
	C3dCoord	en_sol[4];
	int			nsoln;

	nsoln = CSolution::TangentLine( geoArcA, geoArcB, st_sol, en_sol );

	if (nsoln > 0)
	{
		C2dUnitVec	tanA;
		C2dUnitVec	tanB;
		double		dx, dy;
		double		mindist, dist, dot;
		int			indx, jndx;

		mindist = UNDEFINED;
		for (indx = 0; indx < nsoln; ++indx)
		{
			dx = hintA.X() - st_sol[indx].X();
			dy = hintA.Y() - st_sol[indx].Y();
			dist = sqrt( dx*dx + dy*dy );
			if (dist < mindist)
			{
				mindist = dist;
				jndx = indx;
			}

			ptA->X( st_sol[jndx].X() );
			ptA->Y( st_sol[jndx].Y() );
			ptA->Z( geoArcA.CenterPt().Z() );
		}

		mindist = UNDEFINED;
		for (indx = 0; indx < nsoln; ++indx)
		{
			dx = hintB.X() - en_sol[indx].X();
			dy = hintB.Y() - en_sol[indx].Y();
			dist = sqrt( dx*dx + dy*dy );
			if (dist < mindist)
			{
				mindist = dist;
				jndx = indx;
			}

			ptB->X( en_sol[jndx].X() );
			ptB->Y( en_sol[jndx].Y() );
			ptB->Z( geoArcB.CenterPt().Z() );
		}

		tanA = geoArcA.TanAtPt( ptA->X(), ptA->Y() );
		tanB = (*ptB) - (*ptA);

		dot = tanA * tanB;
		if (fabs(dot) < (1. - SMALL))
			nsoln = 0;
	}

	return nsoln;
}

