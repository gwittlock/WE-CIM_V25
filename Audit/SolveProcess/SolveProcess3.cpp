// SolveProcess3.cpp 
//

#include "stdafx.h"
#include "float.h"
#include "cmn_resource.h"
#include "MathConst.h"

#include "2dCoord.h"
#include "2dVec.h"
#include "2dUnitVec.h"
#include "3dCoord.h"

#include "SolveProcess.h"

C2dVec	P11;	// Start point of first line
C2dVec	P12;	// End point of first line
C2dVec	P21;	// Start point of second line
C2dVec	P22;	// End point of second line
C2dVec	P0;		// Center point of circle
C2dVec	P;		// A soln. center point.
C2dVec	P1U;
C2dVec	P2U;
C2dVec	PPU1;	// Unit vector perpendicular to line 1
C2dVec	PPU2;	// Unit vector perpendicular to line 2
C2dVec	PI12;	// Intersection of lines 1 and 2.
C2dVec	PI12D;	// Intersection of displaced lines 1 and 2.
C2dVec	PBU;	// Unit vector along angle bisectors of pair of given lines.
double	R;
double	MB;
double	DISCR;
double	TO_LINE;
double	TO_CENTER;
double	M;
double	A, B, C, D;

CReturn ArcLLA_DO_PARALLEL( C3dCoordArray* results );  // forward declaration
CReturn ArcLLA_DO_CROSSING( C3dCoordArray* results );  // forward declaration
CReturn ArcLLA_GET_CENTERS( C3dCoordArray* results );  // forward declaration
bool COINCIDENT( const C2dVec& p1, const C2dVec& p2 );

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Arc tangent to two lines and another arc.
//
CReturn 
CSolveProcessApp::ArcLLA( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	C3dCoordArray results;
	CString		param;
	C3dCoord*	pc;
	double		xp, yp;
	double		cross;
	int			count, indx;

	xp = io_cmd->VarList().getReal( "xsA", 0. );
	yp = io_cmd->VarList().getReal( "ysA", 0. );
	P11.Init( xp, yp );

	xp = io_cmd->VarList().getReal( "xeA", 0. );
	yp = io_cmd->VarList().getReal( "yeA", 0. );
	P12.Init( xp, yp );

	xp = io_cmd->VarList().getReal( "xsB", 0. );
	yp = io_cmd->VarList().getReal( "ysB", 0. );
	P21.Init( xp, yp );

	xp = io_cmd->VarList().getReal( "xeB", 0. );
	yp = io_cmd->VarList().getReal( "yeB", 0. );
	P22.Init( xp, yp );

	xp = io_cmd->VarList().getReal( "xcC", 0. );
	yp = io_cmd->VarList().getReal( "ycC", 0. );
	P0.Init( xp, yp );

	R = io_cmd->VarList().getReal( "rad", 0. );

	if (COINCIDENT(P11,P12) || COINCIDENT(P21,P22) || (R < SMALL))
	{
		// Obvious input errors.
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::ArcLLA(#1)" );
	}
	else
	{
		P1U = C2dUnitVec( P12 - P11 );
		P2U = C2dUnitVec( P22 - P21 );

		cross = P1U ^ P2U;
		if (fabs(cross) < SMALL )
		{
			status = ArcLLA_DO_PARALLEL( &results );
		}
		else
		{
			status = ArcLLA_DO_CROSSING( &results );
		}
	}

	count = results.Count();
	io_cmd->setInt( "count", count );

	for (indx = 0; indx < count; ++indx)
	{
		pc = results[indx];

		param.Format( "xc%d", indx );
		io_cmd->setReal( param, pc->X() );

		param.Format( "yc%d", indx );
		io_cmd->setReal( param, pc->Y() );

		param.Format( "rad%d", indx );
		io_cmd->setReal( param, pc->Z() );
	}

	results.DestructiveFlush();

	return status;
}

CReturn
ArcLLA_DO_PARALLEL( C3dCoordArray* results )
{
	CReturn		status;
	C2dUnitVec	DV;
	C2dUnitVec	PBU;	// Unit vector along angle bisectors of pair of given lines.
	double		MB;
	double		A, B, C;
	double		DELTA;

	// If the lines are parallel, there is only one "angle bisector",
	// and it is a third line parallel to the other two, midway between
	// them.  The distance from this "angle bisector" to either of the
	// lines is TO_LINE, and the radius of the desired circle. A unit 
	// vector normal to this "angle bisector" is PPU1.

	// This procedure is also used for lines that are not quite 
	// parallel, and in such cases yields only approximate solutions.
	PPU1 = C2dUnitVec(P1U) + HALFPI;
	TO_LINE = fabs((P21 - P11) * PPU1) / 2.0;  // Non-negative.
	if (TO_LINE < SMALL)
	{
		// The given lines are essentially coincident.
		status.Internal( IDS_INTERNAL_ERROR, "ArcLLA_DO_PARALLEL(#1)" );
	}
	else
	{
		// The center of the desired circle center, P, is on the "angle
		// bisector" P = 0.5 * (P11 + P21) + M * P1U. TO_LINE is the
		// distance from the "angle bisector" to either line, and is the
		// radius of the desired circle.  R +- TO_LINE is the distance
		// between P and P0. (P - P0)**2 = (R +- TO_LINE)**2, so
		// (0.5 * (P11 + P21) + MB * P1U - P0)**2 = (R +- TO_LINE)**2, or
		// A * MB**2 + 2 * B * MB + C = 0, where

		PBU = P1U;
		A = 1.0;
		PI12 = (P11 + P21) * 0.5;
		DV = PI12 - P0;
		B = P1U * DV;
		for (int indx = 0; indx < 2; ++indx)
		{
			DELTA = ((indx == 0) ? (R + TO_LINE) : (R - TO_LINE));
			C = (DV * DV) - (DELTA * DELTA);

			DISCR = 1.0 - C / (B * B);
			if (fabs(DISCR) < SMALL)
			{
				MB = -B;
				P = PI12 + (PBU * MB);
				if ( !COINCIDENT(P,P0) )
					results->Append( new C3dCoord( P.X(), P.Y(), TO_LINE ) );
			}
			else if (DISCR >= SMALL)
			{
				MB = B * (-1.0 + sqrt(DISCR));
				P = PI12 + (PBU * MB);
				if ( !COINCIDENT(P,P0) )
					results->Append( new C3dCoord( P.X(), P.Y(), TO_LINE ) );
				MB = B * (-1.0 - sqrt(DISCR));
				P = PI12 + (PBU * MB);
				if ( !COINCIDENT(P,P0) )
					results->Append( new C3dCoord( P.X(), P.Y(), TO_LINE ) );
			}
		}
	}

	return status;
}

CReturn
ArcLLA_DO_CROSSING( C3dCoordArray* results )
{
	CReturn		status;

	// The equation of line 1 is P11 + M * P1U.
	// The equation of line 2 is P21 + N * P2U.
	// These two expressions are equal at the intersection of lines 1
	// and 2.  Actually, we need only M to find the intersection.  We
	// set these two expressions equal and take the cross product of
	// both sides with P2U, so that N drops out, and solve for M:

	M = ((P21 - P11) ^ P2U) / (P1U ^ P2U);

	// The intersection of the two lines is PI12,

	PI12 = P11 + (P1U * M);

	// Find the unit vector PPU1 perpendicular to line 1. Sense of
	// vector is not known.

	PPU1 = C2dUnitVec(P1U) + HALFPI;

	// Repeat for line 2:

	PPU2 = C2dUnitVec(P2U) + HALFPI;

	// Displace both lines a distance R perpendicular to themselves.
	// The resultant lines are R * PPU1 + P11 + M * P1U and
	// R * PPU2 + P21 + N * P2U. These two expressions are equal at
	// the intersection of the displaced lines 1 and 2.  Again, we
	// need only M to find the intersection.  We set these two
	// expressions equal and take the cross product of both sides with
	// P2U, so that N drops out, and solve for M:

	M = ((P21 - P11 + ((PPU2 - PPU1) * R)) ^ P2U) / (P1U ^ P2U);

	// The intersection of the two lines is

	PI12D = (PPU1 * R) + P11 + (P1U * M);

	// A unit vector in the direction of one angle bisector of the
	// given lines is PBU.

	PBU = C2dUnitVec(PI12D - PI12);

	ArcLLA_GET_CENTERS( results );

	// A unit vector in the direction of the other angle bisector of the
	// given lines is

	PBU = C2dUnitVec(PBU) + HALFPI;

	ArcLLA_GET_CENTERS( results );

	return status;
}

CReturn
ArcLLA_GET_CENTERS( C3dCoordArray* results )
{
	CReturn	status;

	// One angle bisector of the original lines is PI12 + MB * PBU. The
	// distance from a point on the angle bisector to the nearest point
	// on line 1 is the magnitude of the quantity MB times the sine of
	// the angle between line 1 and the angle bisector.  This is the
	// magnitude of the quantity MB times the cross product of P1U and
	// PBU; namely, | MB * P1U ** PBU |.  The distance between the point
	// on the angle bisector and the nearest point on the displaced line
	// 1 is the magnitude of this magnitude plus or minus R, depending
	// on the direction in which the line was displaced and which angle
	// bisector we are on; that is, | MB * P1U ** PBU | +- R; we'll look
	// for solutions with both plus and minus R.

	// A vector from this point on the angle bisector to the center of
	// the given circle is P0 - PI12 - MB * PBU.  We must find MB such
	// that the magnitude of this vector equals the distance from the
	// same point on the angle bisector to the displaced line, or
	// equivalently, the square of the distance from the point on the
	// angle bisector to the displaced line equals the square of the
	// distance from the point on the angle bisector to the center of
	// the given circle.  Doing all the algebra, we end up with a
	// quadratic equation in MB, A MB**2 + 2 B MB + C = 0, where A =
	// (1 - (P1U ** PBU)**2),
	// B = (PI12 - P0) * PBU +-  R * (P1U ** PBU),
	// C = (P0 - PI12)**2 - R**2.  Finding all the real roots of this
	// equation provides us with the locations of all the desired circle
	// centers. 

	D = P1U ^ PBU;
	A = 1.0 - (D * D);  // Possible problem when P1U^PBU near 1.
	for (int indx = 0; indx < 2; ++indx)
	{
		if (indx == 0)
		  B = (PI12 - P0) * PBU +  R * D;
		else
		  B = (PI12 - P0) * PBU -  R * D;

		C = (P0 - PI12) * (P0 - PI12) - (R * R);
		if (fabs(B) < SMALL)
		{
			DISCR = sqrt(-C / A);
			MB = DISCR;
			P = PI12 + (PBU * MB);
			if ( !COINCIDENT(P,P0) )
			{
			    TO_LINE = fabs( (P1U * MB) ^ PBU );
				results->Append( new C3dCoord( P.X(), P.Y(), TO_LINE ) );
			}

			MB = -DISCR;
			P = PI12 + (PBU * MB);
			if ( !COINCIDENT(P,P0) )
			{
			    TO_LINE = fabs( (P1U * MB) ^ PBU );
				results->Append( new C3dCoord( P.X(), P.Y(), TO_LINE ) );
			}
		}
		else
		{
			DISCR = 1.0 -  A * C / (B * B);
			if (fabs(DISCR) < SMALL)
			{
				MB = -B / A;
				P = PI12 + (PBU * MB);
				P = PI12 + (PBU * MB);
				if ( !COINCIDENT(P,P0) )
				{
					TO_LINE = fabs( (P1U * MB) ^ PBU );
					results->Append( new C3dCoord( P.X(), P.Y(), TO_LINE ) );
				}
			}
			else if (DISCR > SMALL)
			{
				for (int indx = 0; indx < 2; ++indx)
				{
					if (indx == 0)
						MB = B * (-1.0 + sqrt(DISCR)) / A;
					else
						MB = B * (-1.0 - sqrt(DISCR)) / A;

					P = PI12 + (PBU * MB);
					if ( !COINCIDENT(P,P0) )
					{
						TO_LINE = fabs( (P1U * MB) ^ PBU );
						results->Append( new C3dCoord( P.X(), P.Y(), TO_LINE ) );
					}
				}
			}
		}
	}

	return status;
}

bool
COINCIDENT( const C2dVec& p1, const C2dVec& p2 )
{
	return ((fabs(p2.X() - p1.X()) < SMALL) && (fabs(p2.Y() - p1.Y()) < SMALL));
}

