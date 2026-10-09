// SolveProcess.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "float.h"
#include "cmn_resource.h"
#include "MathConst.h"

#include "2dVec.h"
#include "2dUnitVec.h"
#include "DbEntity.h"
#include "DbPoint.h"
#include "DbCurve.h"
#include "DbProfile.h"
#include "GeoCurve.h"
#include "GeoArc.h"
#include "Model.h"
#include "Profile.h"
#include "Conversion.h"
#include "Int2d.h"
#include "Solution.h"

#if BEFORE_V18
	#include "ExpLexer.h"
	#include "ExpParser.h"
#else
	// These functions are defined ExpParser.y
	extern int ExpParse( const char* expression );
	extern double ExpParseValueGet();
#endif

#if (_CI)
	#include "CiCurve.h"
	#include "CiModel.h"
	#include "Portal.h"
#endif

#include "SolveProcess.h"

CRouteList CSolveProcessApp::m_solveRouter;


// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ==================================================================
//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" bool PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CSolveProcessApp

BEGIN_MESSAGE_MAP(CSolveProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CSolveProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSolveProcessApp construction

CSolveProcessApp::CSolveProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSolveProcessApp object

CSolveProcessApp theApp;


CReturn 
CSolveProcessApp::RegisterProcess( 
	CRouteList*	io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	// -------------------------------------------
	// solutions:
	// -------------------------------------------
	//
	// ARC:
	//		SEP: Start, End, 3rd Point
	//		SCI: Start, Center, Included angle
	//		SER: Start, End, Radius
	//		TER: start Tangent, End, Radius
	//		TT: Tangent to two curves (line, circle)
	//		CR: Center, Radius for full circle
	//
	// LINE:
	//		SAL: Start, Angle, Length
	//		ST: Start, end Tangent to Circle
	//		TT: Tangent to two Circles
	//	
	//	INTERSECT
	//	CHAMFER
	//	SPLIT
	// -------------------------------------------

	ret += io_route->addSubrouter( "Solve", &m_solveRouter);

	ret += m_solveRouter.addProcess( CString("ArcSEP"), ArcSEP );
	ret += m_solveRouter.addProcess( CString("ArcSCI"), ArcSCI );
	ret += m_solveRouter.addProcess( CString("ArcSER"), ArcSER );
	ret += m_solveRouter.addProcess( CString("ArcPTR"), ArcPTR );
	ret += m_solveRouter.addProcess( CString("ArcTT"), ArcTT );
	ret += m_solveRouter.addProcess( CString("ArcCR"), ArcCR );
	ret += m_solveRouter.addProcess( CString("ArcPAL"), ArcPAL );
#if (_CI)
	ret += m_solveRouter.addProcess( CString("ArcPTR_CI"), ArcPTR_CI );
	ret += m_solveRouter.addProcess( CString("ArcLLA"), ArcLLA );
#endif

	// Lives in SolveProcess2.cpp
	ret += m_solveRouter.addProcess( "ArcAngles", ArcAngles );

	ret += m_solveRouter.addProcess( CString("LineSAL"), LineSAL );
	ret += m_solveRouter.addProcess( CString("LineST"), LineST );
	ret += m_solveRouter.addProcess( CString("LineTT"), LineTT );

	ret += m_solveRouter.addProcess( CString("Intersect"), Intersect );
	ret += m_solveRouter.addProcess( CString("TrimExtend"), TrimExtend );
	ret += m_solveRouter.addProcess( CString("Chamfer"), Chamfer );
	ret += m_solveRouter.addProcess( CString("Blend"), Blend );
	ret += m_solveRouter.addProcess( CString("Split"), Split );

	ret += m_solveRouter.addProcess( CString("ChainCut"), ChainCut );

	ret += m_solveRouter.addProcess( CString("Eval"), Evaluate );

	ret += m_solveRouter.addProcess( "StartTan", StartTan );
	ret += m_solveRouter.addProcess( "EndTan", EndTan );
	ret += m_solveRouter.addProcess( "StartPt", StartPt );
	ret += m_solveRouter.addProcess( "EndPt", EndPt );
	ret += m_solveRouter.addProcess( "Length", Length );
	ret += m_solveRouter.addProcess( "CenterPt", CenterPt );
	ret += m_solveRouter.addProcess( "Radius", Radius );
	ret += m_solveRouter.addProcess( "Invert", Invert );

	// These three live in SolveProcess2.cpp
	ret += m_solveRouter.addProcess( "Dir", Dir );
	ret += m_solveRouter.addProcess( "Area", Area );
	ret += m_solveRouter.addProcess( "Reduce", Reduce );

	ret += m_solveRouter.addProcess( "PointClosest", PointClosest );

	ret += m_solveRouter.addProcess( "MultiIntersect",		MultiIntersect );
	ret += m_solveRouter.addProcess( "MultiIntersectCount",	MultiIntersectCount );
	ret += m_solveRouter.addProcess( "MultiIntersectGet",	MultiIntersectGet );
	ret += m_solveRouter.addProcess( "MultiIntersectFlush",	MultiIntersectFlush );

	return ret;
}

CReturn 
CSolveProcessApp::Solve( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_solveRouter.Dispatch( io_cmd );
}


// ==================================================================
// Solve:ArcSEP:
//
//		Define an arc from Start, End, and a 3rd Point:
//			sx, sy
//			ex, ey
//			px, py
//
//		Out:
//			cx, cy, rad, dir
//
//	Math from Graphics Gems p.22, "Triangles", Ronald Goldman.
//
CReturn 
CSolveProcessApp::ArcSEP(
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	// -----------------------------------------------------
	//		Extract command
	//
	double	p1x, p1y;
	double	p2x, p2y;
	double	p3x, p3y;
	int		ccw;
	
	ret = io_cmd->getReal( "sx", &p1x );
	ret += io_cmd->getReal( "sy", &p1y );

	ret += io_cmd->getReal( "ex", &p2x );
	ret += io_cmd->getReal( "ey", &p2y );

	ret += io_cmd->getReal( "px", &p3x );
	ret += io_cmd->getReal( "py", &p3y );
	
	ccw = io_cmd->VarList().getInt( "dir", 0 );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	bool isRightHanded = io_cmd->getModel().IsRightHanded();

	// -----------------------------------------------------
	// Dot the deltas to d...
	//
	double d1, d2, d3;

	d1 = ((p3x - p1x) * (p2x - p1x)) + ((p3y - p1y) * (p2y - p1y));
	d2 = ((p3x - p2x) * (p1x - p2x)) + ((p3y - p2y) * (p1y - p2y));
	d3 = ((p1x - p3x) * (p2x - p3x)) + ((p1y - p3y) * (p2y - p3y));

	// -----------------------------------------------------
	// Now, information on the circum-radius and circum-center
	//
	double c1, c2, c3, cc;

	c1 = d2 * d3;
	c2 = d3 * d1;
	c3 = d1 * d2;
	cc = c1 + c2 + c3;

	double	cx, cy;

	cx = ( ((c2 + c3) * p1x) + ((c3 + c1)*p2x) + ((c1 + c2)*p3x) ) / (2.0*cc);
	cy = ( ((c2 + c3) * p1y) + ((c3 + c1)*p2y) + ((c1 + c2)*p3y) ) / (2.0*cc);

	double dx, dy, rad;

	dx = p1x - cx;
	dy = p1y - cy;
	rad = sqrt( dx*dx + dy*dy );

	C2dUnitVec vecA( (p1x - cx), (p1y - cy) );
	C2dUnitVec vecB( (p3x - cx), (p3y - cy) );
	double dot = ((isRightHanded) ? (vecA ^ vecB) : (vecB ^ vecA));

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "cx", cx );
	io_cmd->setReal( "cy", cy );
	io_cmd->setReal( "rad", rad );
	io_cmd->setInt( "dir", ((dot < 0) ? CW : CCW) );

	return ret;
}

// ==================================================================
// Solve:ArcSCI:
//
//		Define an arc from Start, Center, and Included angle:
//			sx, sy
//			cx, cy
//			ang					+ CCW, - CW
//
//		Out:
//			ex, ey
//
CReturn 
CSolveProcessApp::ArcSCI(
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	// -----------------------------------------------------
	//		Extract command
	//
	double	sx, sy;
	double	cx, cy;
	double	inc_ang;

	ret = io_cmd->getReal( "sx", &sx );
	ret += io_cmd->getReal( "sy", &sy );

	ret += io_cmd->getReal( "cx", &cx );
	ret += io_cmd->getReal( "cy", &cy );

	ret += io_cmd->getReal( "ang", &inc_ang );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	bool isRightHanded = io_cmd->getModel().IsRightHanded();
	if ( !isRightHanded )
		inc_ang = -inc_ang;

	// -----------------------------------------------------
	// Calculate the end...
	//
	double		rad;
	double		en_ang;
	double		dx, dy;
	double		ex, ey;

	dx = sx - cx;
	dy = sy - cy;
	rad = sqrt( dx*dx + dy*dy );
	en_ang = inc_ang + atan2( dy, dx );

	ex = cx + rad * cos( en_ang );
	ey = cy + rad * sin( en_ang );

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "ex", ex );
	io_cmd->setReal( "ey", ey );

	return ret;
}

// ==================================================================
// Solve:ArcSER:
//
//		Define an arc from Start, End, Radius
//			sx, sy
//			ex, ey
//			rad
//			dir
//			big				0 for small arc, 1 for big arc
//
//		Out:
//			cx, cy
//
CReturn 
CSolveProcessApp::ArcSER(
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	// -----------------------------------------------------
	//		Extract command
	//
	double	sx, sy;
	double	ex, ey;
	double	radius;
	BOOL	dir, big;

	ret = io_cmd->getReal( "sx", &sx );
	ret += io_cmd->getReal( "sy", &sy );

	ret += io_cmd->getReal( "ex", &ex );
	ret += io_cmd->getReal( "ey", &ey );

	ret += io_cmd->getReal( "rad", &radius );
	ret += io_cmd->getInt( "dir", &dir );
	ret += io_cmd->getInt( "big", &big );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	bool isRightHanded = io_cmd->getModel().IsRightHanded();
	if ( !isRightHanded )
		big = ((big == 0) ? 1 : 0);

	// -----------------------------------------------------
	// Create intersection of two equal arcs to get
	//	potential centers...
	//
	double		chord_dist;
	double		chord_angle;
	double		angle;
	double		dx, dy;
	double		cx[2], cy[2];

	dx = ex - sx;
	dy = ey - sy;
	chord_dist = sqrt( dx*dx + dy*dy ) / 2.0;
	chord_angle = atan2( dy, dx );

	if ((chord_dist - radius) > SMALL)
	{
		ret.Internal( IDS_SOLVE_NO_SOLUTION );
		return ret;
	}

	angle = acos( chord_dist / radius );

	cx[0] = sx + cos( chord_angle + angle) * radius;
	cy[0] = sy + sin( chord_angle + angle) * radius;

	cx[1] = sx + cos( chord_angle - angle) * radius;
	cy[1] = sy + sin( chord_angle - angle) * radius;

	// -----------------------------------------------------
	//	Determine which arc to choose, based on "big"
	//
	CGeoArc arc0( C3dCoord( sx, sy, 0.0 ),
				  C3dCoord( ex, ey, 0.0 ),
				  C3dCoord( cx[0], cy[0], 0.0 ),
				  dir );

	CGeoArc arc1( C3dCoord( sx, sy, 0.0 ),
				  C3dCoord( ex, ey, 0.0 ),
				  C3dCoord( cx[1], cy[1], 0.0 ),
				  dir );

	int idx = big ^ (arc0.Length2d() > arc1.Length2d() );

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "cx", cx[idx] );
	io_cmd->setReal( "cy", cy[idx] );

	return ret;
}


// ==================================================================
// Solve:ArcPTR:
//
//	Finds the tangent point and center point of an arc having a known radius, and
//	that runs through a known point, and that is tangent to a known entity.
//
//		px, py		know point on resulting arc
//		stan		ID of known tangent entity
//		rad			known radius of resulting arc
//		hx, hy		hint point (approx solution tangent point on known entity)
//					aides in solution selection
//
//	Optional inputs:
//		send		alternative for hx,hy.
//					(0) use start point of known entity as hx,hy 
//					(?) use end point of known entity as hx,hy 
//
//	Out:
//		tx, ty		the tangent point of the resulting arc on the known entity
//		cx, cy		the center point of the resulting arc
//
CReturn 
CSolveProcessApp::ArcPTR( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	// -----------------------------------------------------
	//		Extract command data
	//
	ID		st_tan;
	bool	st_end;

	double	px, py, radius;

	ret = io_cmd->getInt( "stan", (int*)&st_tan );

	ret += io_cmd->getReal( "px", &px );
	ret += io_cmd->getReal( "py", &py );

	ret += io_cmd->getReal( "rad", &radius );

	st_end = (io_cmd->VarList().getInt( "send", TRUE ) != FALSE);

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the reference curve

	CDbEntity* db_entity;
	ret += io_cmd->getModel().EntityFind( st_tan, &db_entity, DBLINE, DBARC );

	CDbCurve* db_curve = dynamic_cast<CDbCurve*>(db_entity);
	if (!db_curve)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, st_tan );
		return ret;
	}
	const CGeoCurve* st_curve = db_curve->Curve();

	//
	//	Pick point, which controls the solution discrimination...
	//	This may be off of the curve, if not explicitly defined.
	//
	C2dCoord hint_pt;
	CReturn	pick_ret;
	double hx, hy;
	pick_ret = io_cmd->getReal( "hx", &hx );
	pick_ret += io_cmd->getReal( "hy", &hy );
	if (pick_ret.isOkay())
	{
		hint_pt = C2dCoord( hx, hy );
	}
	else
	{
		if (st_end)
			hint_pt = st_curve->EndPt();
		else
			hint_pt = st_curve->StartPt();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Find all possible solutions.  Note, the known point is
	// treated as a degenerate arc having a radius of zero.

	C3dCoord	ct_pt[8];
	CGeoArc		degen( C3dCoord( px, py, 0. ), 0., CW );

	C3dCoord	tan_pt;
	double		distBest = UNDEFINED;
	int			indxBest = -1;

	int nsolns = CSolution::Blend( (*st_curve), degen, radius, NULL, NULL, ct_pt );
	if (nsolns > 0)
	{
		// -----------------------------------------------------
		// Given up to 8 solution centers, find the arc that
		//	best fits the criteria (defined above)
		//
		CGeoArc		soln;
		C3dCoord	tanpts[2];
		int			ntans;

		for (int indx = 0; indx < nsolns; indx++)
		{
			// Find the tangent point between the known
			// curve and the candidate solution.

			soln.Init( ct_pt[indx], radius, CW );

			ntans = CSolution::Intersect( soln, (*st_curve), FALSE, tanpts );
			if (ntans > 0)
			{
				// Determine if this is the solution whose
				// tangent point is nearest the hint point.

				C2dVec vec = hint_pt - tanpts[0];
				double dist = vec.Length();

				if (dist < distBest)
				{
					tan_pt = tanpts[0];
					indxBest = indx;
					distBest = dist;
				}
			}
		}
	}

	if (indxBest < 0)
	{
		ret.Internal( IDS_SOLVE_NO_SOLUTION );
	}
	else
	{
		// Set the values in the command
		io_cmd->setReal( "tx", tan_pt.X() );
		io_cmd->setReal( "ty", tan_pt.Y() );
		io_cmd->setReal( "tz", tan_pt.Z() );

		io_cmd->setReal( "cx", ct_pt[indxBest].X() );
		io_cmd->setReal( "cy", ct_pt[indxBest].Y() );
		io_cmd->setReal( "cz", ct_pt[indxBest].Z() );
	}

	delete st_curve;

	return ret;
}

// ==================================================================
// Solve:ArcTT:
//
//		Define an arc from start Tangent, end Tangent, and Radius
//			stan				ID of entity to start with
//			send				TRUE if end, FALSE if start of entity
//			etan				ID of entity to end with
//			eend				TRUE if end, FALSE if start of entity
//			rad
//			[big]				0 for small arc, 1 for big arc, 2 for "other"
//			[dir]				-1 or +1, desired direction for solution (0 for n/a)
//
//		Optional inputs:
//			spx, spy			Start entity pick point (takes precedence over send)
//			epx, epy			End entity pick point (takes precedence over eend)
//
//			onseg				1 if *prefer* solutions to be on segment (trim only)
//
//		Out:
//			sx, sy
//			ex, ey
//			cx, cy
//			dir
//
CReturn 
CSolveProcessApp::ArcTT( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID		st_tan;
	ID		en_tan;
	BOOL	st_end = FALSE;
	BOOL	en_end = FALSE;

	double	radius;

	double spx = io_cmd->VarList().getReal( "spx", UNDEFINED );
	double spy = io_cmd->VarList().getReal( "spy", UNDEFINED );
	double epx = io_cmd->VarList().getReal( "epx", UNDEFINED );
	double epy = io_cmd->VarList().getReal( "epy", UNDEFINED );

	bool useHintPt = (spx != UNDEFINED && spy != UNDEFINED &&
					  epx != UNDEFINED && epy != UNDEFINED);

	if ( !useHintPt )
	{
		ret += io_cmd->getInt( "send", &st_end );
		ret += io_cmd->getInt( "eend", &en_end );
	}

	ret += io_cmd->getInt( "stan", (int*)&st_tan );
	ret += io_cmd->getInt( "etan", (int*)&en_tan );

	ret += io_cmd->getReal( "rad", &radius );

	// Optional...
	int big   = io_cmd->VarList().getInt( "big", IUNDEFINED );  // 0=min, 1=max, 2=other (huh?)
	int dir   = io_cmd->VarList().getInt( "dir", IUNDEFINED );
	int onseg = io_cmd->VarList().getInt( "onseg", FALSE );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	CDbCurve* db_curve1;
	ret += io_cmd->getModel().EntityFind( st_tan, (CDbEntity**) &db_curve1, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, st_tan );
		return ret;
	}

	CDbCurve* db_curve2;
	ret += io_cmd->getModel().EntityFind( en_tan, (CDbEntity**) &db_curve2, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, en_tan );
		return ret;
	}


	if ( !(io_cmd->getModel().IsRightHanded()) )
		big = ((big == 0) ? 1 : 0);

	CGeoCurve* st_curve = db_curve1->Curve();
	CGeoCurve* en_curve = db_curve2->Curve();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Hint points, which control the solution discrimination...
	//	These may come from the curve, or ge explicitly defined
	//
	C2dCoord hint_st;
	C2dCoord hint_en;

	if ( useHintPt )
	{
		hint_st = C2dCoord( spx, spy );
		hint_en = C2dCoord( epx, epy );
	}
	else
	{
		hint_st = ((st_end) ? st_curve->EndPt() : st_curve->StartPt());
		hint_en = ((en_end) ? en_curve->EndPt() : en_curve->StartPt());
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	The solutions
	//
	C3dCoord solnCenter[8];
	CGeoArc soln;
	C3dCoord psBest;
	C3dCoord peBest;
	C3dCoord pcBest;

	double distBest = UNDEFINED;

	int nsolns = CSolution::ArcTT( (*st_curve), (*en_curve), radius, solnCenter );
	if (nsolns > 0)
	{
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Choose the best solution.  The 'visibility' of the solution
		// center to the solution end points is compared with the
		// visibility of the hint points to each other.  The solution
		// whose end points are nearest the hint points is chosen.
		//
		CInt2d intsctA( SMALL, FALSE );
		CInt2d intsctB( SMALL, FALSE );

		// The 'visibility' of the hint points to each other.
		C2dUnitVec vecA = hint_en - hint_st;
		C2dUnitVec vecB = hint_st - hint_en;

		for (int indx = 0; indx < nsolns; ++indx)
		{
			C3dCoord pc = solnCenter[indx];

			soln.Init( pc, radius, CW );

			intsctA.CrvCrv( (*st_curve), soln );
			intsctB.CrvCrv( (*en_curve), soln );

			if (intsctA.Count() > 0 && intsctB.Count() > 0)
			{
				const C2dCoord& ptA = intsctA.Point(0);
				const C2dCoord& ptB = intsctB.Point(0);

				// The 'visibility' of the soln end points to the center.
				C2dUnitVec vecC = pc - ptA;
				C2dUnitVec vecD = pc - ptB;

				double dotA = vecA * vecC;
				double dotB = vecB * vecD;

#if BEFORE
				// The abhorrent case where hint_st and hint_end was not handled.
				// In this case, vecA and vecB are both (0,0), leading to the
				// dot products dotA and dotB both being 0.0
				if (dotA > 0 && dotB > 0)
#endif
				if (dotA >= 0 && dotB >= 0)
				{
					C2dVec vecE = hint_st - ptA;
					C2dVec vecF = hint_en - ptB;
					double dist = vecE.Length() + vecF.Length();

					if (dist < distBest)
					{
						psBest = ptA;
						peBest = ptB;
						pcBest = pc;
						distBest = dist;
					}
				}
			}
		}
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Set the values in the command
	//
	if (distBest < UNDEFINED)
	{
		if (big < IUNDEFINED && dir < IUNDEFINED)
		{
			soln.Init( psBest, peBest, pcBest, dir );

			double ang = soln.IncludedAngle();
			if ((big && ang < PI) || (!big && ang > PI))
				soln.Compliment();

			psBest = soln.StartPt();
			peBest = soln.EndPt();
		}
		else
		{
			C2dUnitVec vecA = psBest - pcBest;
			C2dUnitVec vecB = peBest - pcBest;

			dir = SGN( (vecA ^ vecB) );
		}

		io_cmd->setReal( "sx", psBest.X() );
		io_cmd->setReal( "sy", psBest.Y() );
		io_cmd->setReal( "sz", pcBest.Z() );

		io_cmd->setReal( "ex", peBest.X() );
		io_cmd->setReal( "ey", peBest.Y() );
		io_cmd->setReal( "ez", pcBest.Z() );

		io_cmd->setReal( "cx", pcBest.X() );
		io_cmd->setReal( "cy", pcBest.Y() );
		io_cmd->setReal( "cz", pcBest.Z() );

		io_cmd->setInt( "dir", dir );
	}
	else
	{
		ret.Internal( IDS_SOLVE_NO_SOLUTION );
	}

	delete st_curve;
	delete en_curve;

	return ret;
}

// ==================================================================
// Solve:ArcCR:
//
//		Define a full circle from a center and radius:
//			cx, cy
//			rad
//
//		Out:
//			sx, sy
//			ex, ey
//
CReturn 
CSolveProcessApp::ArcCR(
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	// -----------------------------------------------------
	//		Extract command
	//
	double	cx, cy;
	double	radius;

	ret = io_cmd->getReal( "cx", &cx );
	ret += io_cmd->getReal( "cy", &cy );

	ret += io_cmd->getReal( "rad", &radius );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "sx", cx + radius );
	io_cmd->setReal( "sy", cy );

	io_cmd->setReal( "ex", cx + radius );
	io_cmd->setReal( "ey", cy );

	return ret;
}

// ==================================================================
// Solve:ArcPAL:
//
// Finds the set of circles through the given point and tangent to the
// given circle and unbounded line.
//
//	In:
//		px, py			-- point coordinates
//		cx, cy			-- circle center point
//		rad				-- circle radius
//		sx, sy, ex, ey	-- line end points
//		tx, ty			-- approximate tangent point on known circle (hint)
//						   best taken as 'pick' location on known circle
//
//	Out:
//		cx, cy, rad, tx, ty
//
CReturn 
CSolveProcessApp::ArcPAL( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	// -----------------------------------------------------
	//		Extract command
	//
	double px, py;
	double cx, cy, rad;
	double sx, sy, ex, ey;
	double tx, ty;

	status  = io_cmd->getReal( "px", &px );
	status += io_cmd->getReal( "py", &py );
	status += io_cmd->getReal( "cx", &cx );
	status += io_cmd->getReal( "cy", &cy );
	status += io_cmd->getReal( "rad", &rad );
	status += io_cmd->getReal( "sx", &sx );
	status += io_cmd->getReal( "sy", &sy );
	status += io_cmd->getReal( "ex", &ex );
	status += io_cmd->getReal( "ey", &ey );
	status += io_cmd->getReal( "tx", &tx );
	status += io_cmd->getReal( "ty", &ty );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::ArcPAL()" );
	}
	else
	{
		static bool DRAW_SOLUTIONS = FALSE;

		C2dUnitVec uvec;
		C2dVec vec;
		C2dCoord tanPt[4];
		C3dCoord centers[4];
		double radii[4];
		double dist, bestDist;
		int nsolns, indx, bestIndx;

		// Find all possible solutions.
		CGeoLine line(
					C3dCoord( sx, sy, 0. ),
					C3dCoord( ex, ey, 0. ) );

		CGeoArc arc(
					C3dCoord( cx+rad, cy, 0. ),
					C3dCoord( cx+rad, cy, 0. ),
					C3dCoord( cx, cy, 0. ), CW );

		C3dCoord pt( px, py, 0. );

		nsolns = CSolution::ArcPAL( pt, arc, line, centers, radii );

		if (DRAW_SOLUTIONS)
		{
			CModel& model = io_cmd->getModel();

			CDbTool* dbTool = model.ActiveTool();
			CDbWorkplane* dbWork = model.ActiveWorkplane();

			for (indx = 0; indx < nsolns; ++indx)
			{
				CDbArc* dbArc;
				model.EntityCreate( DBARC, (CDbEntity**) &dbArc );

				dbArc->Init( dbTool, dbWork,
							 C3dCoord( centers[indx].X() + radii[indx], centers[indx].Y(), 0.0 ),
							 C3dCoord( centers[indx].X() + radii[indx], centers[indx].Y(), 0.0 ),
							 C3dCoord( centers[indx].X(), centers[indx].Y(), 0.0 ), CW );
			}
		}

		// Find the best solution based on proximity to hint tangent point.
		bestDist = UNDEFINED;
		bestIndx = -1;
		for (indx = 0; indx < nsolns; ++indx)
		{
			vec.Init( (centers[indx].X() - cx), (centers[indx].Y() - cy) );

			dist = vec.Length();

			if (dist > radii[indx])
				uvec.Init( vec.X(), vec.Y() );
			else
				uvec.Init( -vec.X(), -vec.Y() );

			tanPt[indx].XY( (cx + (rad * uvec.X())), (cy + (rad * uvec.Y())) );

			vec.Init( (tx - tanPt[indx].X()), (ty - tanPt[indx].Y()) );
			
			dist = vec.Length();

			if (dist < bestDist)
			{
				bestDist = dist;
				bestIndx = indx;
			}
		}

		if (bestIndx < 0)
		{
			status.Internal( IDS_SOLVE_NO_SOLUTION );
		}
		else
		{
			io_cmd->setReal( "cx", centers[bestIndx].X() );
			io_cmd->setReal( "cy", centers[bestIndx].Y() );
			io_cmd->setReal( "rad", radii[bestIndx] );
			io_cmd->setReal( "tx", tanPt[bestIndx].X() );
			io_cmd->setReal( "ty", tanPt[bestIndx].Y() );
		}
	}

	return status;
}

// ==================================================================
// Solve:LineSAL:
//
//		Define a line from a Start, Angle, and Length
//			sx, sy
//			ang (radians)
//			len
//
//		Out:
//			ex, ey
//
CReturn 
CSolveProcessApp::LineSAL(
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	// -----------------------------------------------------
	//		Extract command
	//
	double	sx, sy;
	double	angle;
	double	length;

	ret = io_cmd->getReal( "sx", &sx );
	ret += io_cmd->getReal( "sy", &sy );

	ret += io_cmd->getReal( "ang", &angle );
	ret += io_cmd->getReal( "len", &length );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	if ( !(io_cmd->getModel().IsRightHanded()) )
		angle = -angle;

	// -----------------------------------------------------
	//	Simple solution
	//
	C2dUnitVec vec( angle );
	C2dVec		delta = vec * length;
	C2dCoord		end( sx + delta.X(), sy + delta.Y() );

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "ex", end.X() );
	io_cmd->setReal( "ey", end.Y() );

	return ret;
}

// ==================================================================
// Solve:LineST:
//
//		Define a line from a Start and end tangent entity
//			sx, sy
//			etan				ID of entity to end with
//			eend				TRUE if end, FALSE if start of entity
//
//		Optional inputs:
//			epx, epy			Pick Point (as opposed to start/end of tan el)
//
//		Out:
//			ex, ey
//
CReturn 
CSolveProcessApp::LineST(
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	// -----------------------------------------------------
	//		Extract command
	//
	double	sx, sy;
	ID			en_tan;
	int		en_end;

	ret = io_cmd->getReal( "sx", &sx );
	ret += io_cmd->getReal( "sy", &sy );

	ret += io_cmd->getInt( "etan", (int*)&en_tan );
	ret += io_cmd->getInt( "eend", &en_end );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//
	CDbEntity*	db_entity;
	ret += io_cmd->getModel().EntityFind( en_tan, &db_entity,DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, en_tan );
		return ret;
	}
	CDbCurve* db_curve = dynamic_cast<CDbCurve*>(db_entity);
	if (!db_curve)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, en_tan );
		return ret;
	}
	const CGeoCurve* en_curve = db_curve->Curve();

	//
	//	Pick point, which controls the solution discrimination...
	//	This may be off of the curve, if not explicitly defined.
	//
	CReturn	pick_ret;
	double px, py;

	C3dCoord	en_pt;
	if (en_end)
		en_pt = en_curve->EndPt();
	else
		en_pt = en_curve->StartPt();

	C2dCoord	pick_pt;
	pick_ret = io_cmd->getReal( "epx", &px );
	pick_ret += io_cmd->getReal( "epy", &py );
	if (pick_ret.isOkay())
		pick_pt = C2dCoord( px, py );
	else
		pick_pt = en_pt;

	C3dCoord st_pt( sx, sy, 0.0 );

	// -----------------------------------------------------
	//		Perform tangent-line solution, which gives
	//		multiple possible solution point sets
	//
	CGeoArc	st_arc( st_pt, st_pt, st_pt, -1 );

	C3dCoord		st_sol[4];
	C3dCoord		en_sol[4];
	C3dCoord		mid_pt;
	int			pt_num;

	if (en_curve->Type() != GEOARC)
	{
		CGeoArc	en_arc( en_pt, en_pt, en_pt, -1 );
		pt_num = CSolution::TangentLine( st_arc, en_arc, st_sol, en_sol );
		mid_pt = C2dCoord( (en_curve->StartPt().X() + en_curve->EndPt().X()) / 2.0,
								(en_curve->StartPt().Y() + en_curve->EndPt().Y()) / 2.0 );
	}
	else
	{
		pt_num = CSolution::TangentLine( st_arc, *((CGeoArc*)en_curve), st_sol, en_sol );
		mid_pt = ((CGeoArc*)en_curve)->CenterPt();
	}
	if (!pt_num)
	{
		delete en_curve;

		ret.Internal( IDS_SOLVE_NO_SOLUTION );
		return ret;
	}

	// -----------------------------------------------------
	//		Discriminate on solution side
	//
	C2dUnitVec	mid_vec = C2dCoord( sx, sy ) - mid_pt;
	C2dUnitVec	pick_vec = pick_pt - mid_pt;
	int			pick_side = SGN( mid_vec.PerpDot( pick_vec ) );

	// -----------------------------------------------------
	// Given up to 4 solution sets (start and end), find 
	// the one that is best... hahahahahahahahaaaa
	//
	int		idx;
	double	min_dist;
	int		min_idx;
	C2dVec	sol_vec;
	int		sol_side;
	double	dist;

	min_dist = DBL_MAX;
	min_idx = -1;
	for (idx=0; idx<pt_num; idx++)
	{
		sol_vec = en_sol[idx] - mid_pt;
		sol_side = SGN( mid_vec.PerpDot( sol_vec ) );
		if ( !sol_side
			|| (sol_side == pick_side) )
		{
			dist = sol_vec.Length();

			if (dist < min_dist)
			{
				min_dist = dist;
				min_idx = idx;
			}
		}
	}
	if ( min_idx < 0 )
	{
		delete en_curve;

		ret.Internal( IDS_SOLVE_NO_SOLUTION );
		return ret;
	}

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "ex", en_sol[min_idx].X() );
	io_cmd->setReal( "ey", en_sol[min_idx].Y() );

	delete en_curve;

	return ret;
}


// ==================================================================
// Solve:LineTT:
//
//		Define a line from two tangent entities
//			stan				ID of entity to start with
//			send				TRUE if end, FALSE if start of entity
//			etan				ID of entity to end with
//			eend				TRUE if end, FALSE if start of entity
//
//		Optional inputs:
//			spx, spy			Pick Points (as opposed to start/end of tan els)
//			epx, epy
//
//		Out:
//			sx, sy
//			ex, ey
//
CReturn CSolveProcessApp::LineTT( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID			st_tan;
	BOOL		st_end;
	ID			en_tan;
	BOOL		en_end;

	ret = io_cmd->getInt( "stan", (int*)&st_tan );
	ret += io_cmd->getInt( "send", &st_end );

	ret = io_cmd->getInt( "etan", (int*)&en_tan );
	ret += io_cmd->getInt( "eend", &en_end );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//
	CDbEntity*	db_entity1;
	ret += io_cmd->getModel().EntityFind( st_tan, &db_entity1, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, st_tan );
		return ret;
	}
	CDbCurve* db_curve1 = dynamic_cast<CDbCurve*>(db_entity1);
	if (!db_curve1)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, st_tan );
		return ret;
	}

	CDbEntity*	db_entity2;
	ret += io_cmd->getModel().EntityFind( en_tan, &db_entity2, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, en_tan );
		return ret;
	}
	CDbCurve* db_curve2 = dynamic_cast<CDbCurve*>(db_entity2);
	if (!db_curve2)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, en_tan );
		return ret;
	}

	const CGeoCurve* st_curve = db_curve1->Curve();
	const CGeoCurve* en_curve = db_curve2->Curve();

	//
	//	Pick points, which control the solution discrimination...
	//	This may be from the curve, if not explicitly defined.
	//
	CReturn	pick_ret;
	double	px, py;

	C3dCoord	st_pt;
	if (st_end)
		st_pt = st_curve->EndPt();
	else
		st_pt = st_curve->StartPt();

	C2dCoord	pick_st;
	pick_ret = io_cmd->getReal( "spx", &px );
	pick_ret += io_cmd->getReal( "spy", &py );
	if (pick_ret.isOkay())
		pick_st = C2dCoord( px, py );
	else
		pick_st = st_pt;

	C3dCoord	en_pt;
	if (en_end)
		en_pt = en_curve->EndPt();
	else
		en_pt = en_curve->StartPt();

	C2dCoord	pick_en;
	pick_ret = io_cmd->getReal( "epx", &px );
	pick_ret += io_cmd->getReal( "epy", &py );
	if (pick_ret.isOkay())
		pick_en = C2dCoord( px, py );
	else
		pick_en = en_pt;

	// -----------------------------------------------------
	//		Perform tangent-line solution, which gives
	//		multiple possible solution point sets
	//
	C2dCoord		mid1;
	C2dCoord		mid2;
	C3dCoord		st_sol[4];
	C3dCoord		en_sol[4];
	int			pt_num;

	if ( (st_curve->Type() != GEOARC)
		&& (en_curve->Type() != GEOARC) )
	{
		mid1 = C2dCoord( (st_curve->StartPt().X() + st_curve->EndPt().X()) / 2.0,
								(st_curve->StartPt().Y() + st_curve->EndPt().Y()) / 2.0 );

		mid2 = C2dCoord( (en_curve->StartPt().X() + en_curve->EndPt().X()) / 2.0,
								(en_curve->StartPt().Y() + en_curve->EndPt().Y()) / 2.0 );

		st_sol[0] = st_pt;
		en_sol[0] = en_pt;
		pt_num = 1;
	}
	else
	if ( st_curve->Type() != GEOARC )
	{
		CGeoArc	st_arc( st_pt, st_pt, st_pt, -1 );
		mid1 = C2dCoord( (st_curve->StartPt().X() + st_curve->EndPt().X()) / 2.0,
								(st_curve->StartPt().Y() + st_curve->EndPt().Y()) / 2.0 );
		mid2 = ((CGeoArc*)en_curve)->CenterPt();

		pt_num = CSolution::TangentLine( st_arc, *((CGeoArc*)en_curve), st_sol, en_sol );
	}
	else
	if ( en_curve->Type() != GEOARC )
	{
		const CGeoArc	en_arc( en_pt, en_pt, en_pt, -1 );
		mid1 = ((CGeoArc*)st_curve)->CenterPt();
		mid2 = C2dCoord( (en_curve->StartPt().X() + en_curve->EndPt().X()) / 2.0,
								(en_curve->StartPt().Y() + en_curve->EndPt().Y()) / 2.0 );

		pt_num = CSolution::TangentLine( *((CGeoArc*)st_curve), en_arc, st_sol, en_sol );
	}
	else
	{
		mid1 = ((CGeoArc*)st_curve)->CenterPt();
		mid2 = ((CGeoArc*)en_curve)->CenterPt();
		pt_num = CSolution::TangentLine( *((CGeoArc*)st_curve), *((CGeoArc*)en_curve), st_sol, en_sol );
	}

	if (!pt_num)
	{
		delete st_curve;
		delete en_curve;

		ret.Internal( IDS_SOLVE_NO_SOLUTION );
		return ret;
	}

	
	// -----------------------------------------------------
	//		Discriminate on solution side
	//
	C2dUnitVec	mid_vec = mid2 - mid1;
	C2dUnitVec	st_vec = pick_st - mid1;
	C2dUnitVec	en_vec = pick_en - mid1;
	int			st_side = SGN( mid_vec.PerpDot( st_vec ) );
	int			en_side = SGN( mid_vec.PerpDot( en_vec ) );

	// -----------------------------------------------------
	// Given up to 4 solution sets (start and end), find 
	// the one that is best... hahahahahahahahaaaa
	//
	int		idx;
	double	min_dist;
	int		min_idx;
	C2dVec	sol_vec;
	C2dVec	sol_stvec;
	C2dVec	sol_envec;
	int		sol_stside;
	int		sol_enside;
	double	dist;

	min_dist = DBL_MAX;
	min_idx = -1;
	for (idx=0; idx<pt_num; idx++)
	{
		sol_stvec = st_sol[idx] - mid1;
		sol_stside = SGN( mid_vec.PerpDot( sol_stvec ) );

		sol_envec = en_sol[idx] - mid1;
		sol_enside = SGN( mid_vec.PerpDot( sol_envec ) );

		if ( (st_side == sol_stside)
			&& (en_side == sol_enside) )
		{
			sol_vec = en_sol[idx] - st_sol[idx];
			dist = sol_vec.Length();

			if (dist < min_dist)
			{
				min_dist = dist;
				min_idx = idx;
			}
		}
	}
	if ( min_idx < 0 )
	{
		delete st_curve;
		delete en_curve;

		ret.Internal( IDS_SOLVE_NO_SOLUTION );
		return ret;
	}

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "sx", st_sol[min_idx].X() );
	io_cmd->setReal( "sy", st_sol[min_idx].Y() );
	io_cmd->setReal( "sz", st_sol[min_idx].Z() );

	io_cmd->setReal( "ex", en_sol[min_idx].X() );
	io_cmd->setReal( "ey", en_sol[min_idx].Y() );
	io_cmd->setReal( "ez", en_sol[min_idx].Z() );

	delete st_curve;
	delete en_curve;

	return ret;
}



// ==================================================================
// Solve:Split:
//
//		Return a point on the entity
//			id
//			pos
//			dist
//
enum eSplitPos
{
	SPLIT_MID,
	SPLIT_START,
	SPLIT_END,
	SPLIT_PICK	// 18 Dec 03 need to add split at pick/pnt
};

CReturn 
CSolveProcessApp::Split( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	C3dCoord	split;
	CDbEntity*	db_entity;
	CDbCurve*	db_curve;
	const CGeoCurve* curve;
	ID			id;
	eSplitPos	pos;
	double		dist;

	id = io_cmd->VarList().getInt( "id", 0 );
	pos = (eSplitPos) io_cmd->VarList().getInt( "pos", SPLIT_MID );

	if ((pos == SPLIT_START) || (pos == SPLIT_END))
		dist = io_cmd->VarList().getReal( "dist", 0. );
	else
		dist = 0.5;

	// -----------------------------------------------------
	//		Get the curve to split...
	//
	io_cmd->getModel().EntityFind( id, &db_entity, DBLINE, DBARC );

	db_curve = dynamic_cast<CDbCurve*>(db_entity);
	if (!db_curve)
	{
		ret.Internal( IDS_SOLVE_SPLIT_TYPE, id);
		return ret;
	}

	// -----------------------------------------------------
	//		Find the split point, based on curve type
	//
	curve = db_curve->Curve();

	if (pos == SPLIT_PICK)
	{
		const CVarList& var = io_cmd->VarList();

		double px = var.getReal("px", 0.0);
		double py = var.getReal("py", 0.0);
		double pz = var.getReal("pz", 0.0);

		double u = 0.0;
		curve->PointClosest( C3dCoord(px, py, pz), &split, &u);
		
		double stol = SMALL*3.0; // Minimum length to avoid degenerate split curve
		if (!BETWEEN(stol, u, 1.0-stol))
		{ 
			split.XYZ(UNDEFINED, UNDEFINED, UNDEFINED);
			ret.setStatus(STATUS_ERROR);
		}
	}
	else
	{
		if (pos == SPLIT_MID)
			dist *= curve->Length2d();

		split = curve->PointAtDist(dist, (pos != SPLIT_END));
	}

	delete curve;


	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "cx", split.X() );
	io_cmd->setReal( "cy", split.Y() );
	io_cmd->setReal( "cz", split.Z() );

	return ret;
}


// ==================================================================
// Solve:Chamfer:
//
//		Line across the corner of two other lines:
//			id1
//			id2
//			ang
//			off
//
//		Optional:
//			onseg				1 if force solutions to be on segment (trim only)
//
//		Returns:
//			sx, sy, sz
//			ex, ey, ez
//			end1
//			end2
//
CReturn 
CSolveProcessApp::Chamfer( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID			id1;
	ID			id2;
	double	ch_ang;
	double	offset;

	ret += io_cmd->getInt( "id1", (int*)&id1 );
	ret += io_cmd->getInt( "id2", (int*)&id2 );

	ret += io_cmd->getReal( "ang", &ch_ang );
	ret += io_cmd->getReal( "off", &offset );

	int onseg = 0;
	io_cmd->getInt( "onseg", &onseg );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Get the lines...
	//
	CDbEntity*	db_entity1;
	ret += io_cmd->getModel().EntityFind( id1, &db_entity1, DBLINE, DBLINE );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_CHAMFER_MISSING, id1 );
		return ret;
	}
	CDbLine* db_line1 = dynamic_cast<CDbLine*>(db_entity1);
	if (!db_line1)
	{
		ret.Internal( IDS_SOLVE_CHAMFER_TYPE, id1);
		return ret;
	}

	CDbEntity*	db_entity2;
	ret += io_cmd->getModel().EntityFind( id2, &db_entity2, DBLINE, DBLINE );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_CHAMFER_MISSING, id2 );
		return ret;
	}
	CDbLine* db_line2= dynamic_cast<CDbLine*>(db_entity2);
	if (!db_line2)
	{
		ret.Internal( IDS_SOLVE_CHAMFER_TYPE, id2 );
		return ret;
	}

	const CGeoLine* line1 = db_line1->Line();
	const CGeoLine* line2 = db_line2->Line();

	// -----------------------------------------------------
	//		Find where the damn lines connect
	//
	//	TODO:  This end-testing and flag setting is very clumsy.  Make it better.
	//
	C3dVec	vec1;
	C3dVec	vec2;
	bool		end1;
	bool		end2;

	// Desired configuration is end of 1 connected to start of 2
	if (line1->EndPt().WithinTol( line2->StartPt(), SMALL ))
	{
		vec1 = line1->EndPt() - line1->StartPt();
		vec2 = line2->EndPt() - line2->StartPt();

		end1 = TRUE;
		end2 = FALSE;
	}
	else
	if (line1->EndPt().WithinTol( line2->EndPt(), SMALL ))
	{
		vec1 = line1->EndPt() - line1->StartPt();
		vec2 = line2->StartPt() - line2->EndPt();

		end1 = TRUE;
		end2 = TRUE;
	}
	else
	if (line1->StartPt().WithinTol( line2->StartPt(), SMALL ))
	{
		vec1 = line1->StartPt() - line1->EndPt();
		vec2 = line2->EndPt() - line2->StartPt();

		end1 = FALSE;
		end2 = FALSE;
	}
	else
	if (line1->StartPt().WithinTol( line2->EndPt(), SMALL ))
	{
		vec1 = line1->StartPt() - line1->EndPt();
		vec2 = line2->StartPt() - line2->EndPt();

		end1 = FALSE;
		end2 = TRUE;
	}
	else
	{
		delete line1;
		delete line2;

		ret.Internal( IDS_SOLVE_CHAMFER_CONNECT, id1, id2 );
		return ret;
	}

	// -----------------------------------------------------
	//		Find a solution for the chamfer line...
	//
	//
	// Travel down the first line to get start...
	//
	C2dUnitVec	st_vec = vec1;
	C2dVec		off_vec = st_vec * -offset;
	C3dCoord		st_pt;

	if (end1)
		st_pt = line1->EndPt() + off_vec;
	else
		st_pt = line1->StartPt() + off_vec;

	//
	// Figure what angle the chamfer is at...
	//
	double		angle = st_vec.Radians();

	C2dUnitVec	en_vec = vec2;
	if (st_vec.PerpDot( en_vec ) <= 0.0)
		angle -= ch_ang;
	else
		angle += ch_ang;

	//
	//	Now generate a temporary line...
	//
	C2dUnitVec	ch_vec2d( angle );
	C3dVec		ch_vec( ch_vec2d.X(), ch_vec2d.Y(), 0.0 );

	CGeoLine	chamfer( st_pt, st_pt + ch_vec );

	//
	// ... and intersect for the end point
	//
	int			sol_num;
	C3dCoord		sol_pt[2];

	sol_num = CSolution::Intersect( chamfer, *line2, onseg, sol_pt );
	if (!sol_num)
	{
		delete line1;
		delete line2;

		ret.Internal( IDS_SOLVE_NO_SOLUTION );
		return ret;
	}

	// -----------------------------------------------------
	// Set the values in the command
	//
	io_cmd->setReal( "sx", st_pt.X() );
	io_cmd->setReal( "sy", st_pt.Y() );
	io_cmd->setReal( "sz", st_pt.Z() );

	io_cmd->setReal( "ex", sol_pt[0].X() );
	io_cmd->setReal( "ey", sol_pt[0].Y() );
	io_cmd->setReal( "ez", sol_pt[0].Z() );

	io_cmd->setInt( "end1", end1 );
	io_cmd->setInt( "end2", end2 );

	delete line1;
	delete line2;

	return ret;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:Blend: idA = %d, idB = %d, rad = %f, [, tol = %f]
//
// Get the parameters of a blend radius between two 'adjacent'
// entities that share a common end point (within a given tol).
//
// NOTE: Though it doesn't make a great deal of sense in
// many cases, you can provide a negative radius. In said
// case, the resulting blend will be inverted.
//
// Returns: sx,sy,sz, ex,ey,ez, cx,cy,cz, dir
//
CReturn 
CSolveProcessApp::Blend( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	ID idA;
	ID idB;
	double rad;

	status += io_cmd->getInt( "idA", (int*) &idA );
	status += io_cmd->getInt( "idB", (int*) &idB );
	status += io_cmd->getReal( "rad", &rad );

	double tol = SMALL;
	io_cmd->getReal( "tol", &tol );


	CDbCurve* dbCurveA = NULL;
	CDbCurve* dbCurveB = NULL;

	if ( status.IsOk() )
	{
		CModel& model = io_cmd->getModel();

		CDbEntity* dbEntityA;
		CDbEntity* dbEntityB;

		model.EntityFind( idA, (CDbEntity**) &dbEntityA, DBLINE, DBARC );
		model.EntityFind( idB, (CDbEntity**) &dbEntityB, DBLINE, DBARC );

		dbCurveA = dynamic_cast<CDbCurve*>( dbEntityA );
		dbCurveB = dynamic_cast<CDbCurve*>( dbEntityB );
	}

	if (dbCurveA == NULL || dbCurveB == NULL)
	{
		status.setStatus( STATUS_ERROR );
	}
	else
	{
		CGeoCurve* geoCurveA = dbCurveA->Curve();
		CGeoCurve* geoCurveB = dbCurveB->Curve();

		CGeoArc* blend = CSolution::Blend( (*geoCurveA), (*geoCurveB), fabs(rad), tol );

		delete geoCurveA;
		delete geoCurveB;

		if (blend == NULL)
		{
			status.setStatus( STATUS_ERROR );
		}
		else
		{
			if (rad < 0.)
				ArcInvert( blend );

			const C3dCoord& ps = blend->StartPt();
			const C3dCoord& pe = blend->EndPt();
			const C3dCoord& pc = blend->CenterPt();

			io_cmd->setReal( "sx", ps.X() );
			io_cmd->setReal( "sy", ps.Y() );
			io_cmd->setReal( "sz", ps.Z() );

			io_cmd->setReal( "ex", pe.X() );
			io_cmd->setReal( "ey", pe.Y() );
			io_cmd->setReal( "ez", pe.Z() );

			io_cmd->setReal( "cx", pc.X() );
			io_cmd->setReal( "cy", pc.Y() );
			io_cmd->setReal( "cz", pc.Z() );

			io_cmd->setInt( "dir", blend->Dir() );

			delete blend;
		}
	}

	return status;
}



// ==================================================================
// Solve:Eval:
//
//		Evaluate an arithmetic expression
//			exp
//
//		Returns:
//			val
//
#if BEFORE_V18
CReturn 
CSolveProcessApp::Evaluate( 
	CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	CExpLexer	lexer;
	CExpParser	parser;

	CString express;
	io_cmd->getString( "exp", &express );

	express.MakeLower();
	if (!ret.isOkay())
	{
		// Default on empty string -- value 0
		io_cmd->setReal( "val", 0.0 );
		return CReturn( STATUS_OKAY );
	}

	lexer.Init( &parser );
	parser.Init( &lexer );

	lexer.setString( express );

	if (parser.yyparse() == YYEXIT_FAILURE)
	{
		ret.Internal( IDS_EXP_PARSE_ERROR, express );
		io_cmd->setReal( "val", 0.0 );

		return ret;
	}

	io_cmd->setReal( "val", parser.getValue() );

	return ret;
}
#else
CReturn 
CSolveProcessApp::Evaluate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;
	CString	express;

	io_cmd->getString( "exp", &express );

	express.MakeLower();
	if (!ret.isOkay())
	{
		// Default on empty string -- value 0
		io_cmd->setReal( "val", 0.0 );
		return CReturn( STATUS_OKAY );
	}

	if (ExpParse( express ) != 0)
	{
		ret.Internal( IDS_EXP_PARSE_ERROR, express );
		io_cmd->setReal( "val", 0.0 );

		return ret;
	}

	io_cmd->setReal( "val", ExpParseValueGet() );

	return ret;
}
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:StartTan: id = %d
//   Where id can represent either a curve or profile
//   Returns dx = %f, dy = %f, dz = %f
CReturn 
CSolveProcessApp::StartTan( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve = CurveGet( model, id );

		if (dbCurve != NULL)
		{
			CGeoCurve* geoCurve = dbCurve->Curve();
			C2dUnitVec startTan = geoCurve->StartTan();

			io_cmd->setReal( "dx", startTan.X() );
			io_cmd->setReal( "dy", startTan.Y() );
			io_cmd->setReal( "dz", 0.0 );

			delete geoCurve;
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::StartTan()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:EndTan: id = %d
//   Where id can represent either a curve or profile
//   Returns dx = %f, dy = %f, dz = %f
CReturn 
CSolveProcessApp::EndTan( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve = CurveGet( model, id );

		if (dbCurve != NULL)
		{
			CGeoCurve* geoCurve = dbCurve->Curve();
			C2dUnitVec endTan = geoCurve->EndTan();

			io_cmd->setReal( "dx", endTan.X() );
			io_cmd->setReal( "dy", endTan.Y() );
			io_cmd->setReal( "dz", 0.0 );

			delete geoCurve;
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::EndTan()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:StartPt: id = %d
//   Where id can represent either a curve or profile
//   Returns xs = %f, ys = %f, zs = %f
CReturn 
CSolveProcessApp::StartPt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve = CurveGet( model, id );

		if (dbCurve != NULL)
		{
			CDbPoint* dbPoint = dbCurve->DbStartPt();
			io_cmd->setReal( "id", dbPoint->Id() );
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::StartPt()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:EndPt: id = %d
//   Where id can represent either a curve or profile
//   Returns xe = %f, ye = %f, ze = %f
CReturn 
CSolveProcessApp::EndPt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve = CurveGet( model, id );

		if (dbCurve != NULL)
		{
			CDbPoint* dbPoint = dbCurve->DbEndPt();
			io_cmd->setReal( "id", dbPoint->Id() );
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::EndPt()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:Length: id = %d
//   Where id can represent either a curve or profile
//   Returns length = %f
CReturn 
CSolveProcessApp::Length( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve = CurveGet( model, id );

		if (dbCurve != NULL)
		{
			CGeoCurve* geoCurve = dbCurve->Curve();
			double len = geoCurve->Length2d();

			io_cmd->setReal( "len", len );

			delete geoCurve;
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::Length()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:CenterPt: id = %d
//   Where id can represent either a curve or profile
//   Returns xc = %f, yc = %f, zc = %f
CReturn 
CSolveProcessApp::CenterPt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve = CurveGet( model, id );

		CDbArc* dbArc = dynamic_cast<CDbArc*>( dbCurve );

		if (dbArc != NULL)
		{
			CDbPoint* dbPoint = dbArc->DbCenterPt();
			io_cmd->setReal( "id", dbPoint->Id() );
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::CenterPt()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:Radius: id = %d
//   Where id represents an arc
//   Returns rad = %f
CReturn 
CSolveProcessApp::Radius( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		CDbArc* dbArc;
		model.EntityFind( id, (CDbEntity**) &dbArc, DBARC, DBARC );

		if (dbArc != NULL)
		{
			double radius = dbArc->Radius();
			io_cmd->setReal( "rad", radius );
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::Radius()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:Invert: id = %d
//   Where id represents an arc.
//   Returns (success) id=%d / (0) failure
CReturn 
CSolveProcessApp::Invert( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt( "id", 0 );

	if (id > 0)
	{
		CDbCurve* dbCurve = CurveGet( model, id );

		CDbArc* dbArc = dynamic_cast<CDbArc*>( dbCurve );

		if (dbArc != NULL)
		{
			CGeoArc* geoArc = (CGeoArc*) dbArc->Curve();
			const C3dCoord& ps = geoArc->StartPt();
			const C3dCoord& pe = geoArc->EndPt();
			if ( !ps.WithinTolXY( pe, SMALL ) )
			{
				C3dCoord pc = geoArc->CenterPt();
				int dir = geoArc->Dir();

				// NOTE: The 'mid' point calculation is more complicated
				// than necessary but simplifying it requires adding
				// overloaded operators to C3dCoord.
				C3dCoord mid = ps + ((pe - ps) * 0.5);
				C2dVec vec = mid - pc;
				pc = mid + vec;

				CDbTool* dbTool = dbArc->Tool();
				CDbWorkplane* dbWork = dbArc->Workplane();
				dbArc->Init( dbTool, dbWork, ps, pe, pc, -dir );
			}

			delete geoArc;
		}
		else
		{
			id = 0;
		}
	}

	if (id == 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::Invert()" );
		status.setStatus( STATUS_ERROR );
	}

	io_cmd->setReal( "id", id );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:PointClosest: id=%d, x=%f, y=%f
//   Where id represents id of a curve
//   ASSUMPTION: The given point is in the same workplane as the curve.
CReturn 
CSolveProcessApp::PointClosest( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	double x, y;
	ID id = 0;

	status += io_cmd->getInt( "id", (int*) &id );
	status += io_cmd->getReal( "x", &x );
	status += io_cmd->getReal( "y", &y );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve;
		model.EntityFind( id, (CDbEntity**) &dbCurve, DBLINE, DBARC );

		if (dbCurve != NULL)
		{
			C3dCoord refPt( x, y, 0. );
			C3dCoord closestPt;
			double uparam;

			CGeoCurve* geoCurve = dbCurve->Curve();

			double dist = geoCurve->PointClosest( refPt, &closestPt, &uparam );

			delete geoCurve;

			io_cmd->setReal( "x", closestPt.X() );
			io_cmd->setReal( "y", closestPt.Y() );
			io_cmd->setReal( "dist", dist );
			io_cmd->setReal( "uparam", uparam );
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::PointClosest()" );
		return status;
	}

	return status;
}

CDbCurve* 
CSolveProcessApp::CurveGet( const CModel& model, ID id )
{
	CDbEntity* dbEntity;
	CReturn status = model.EntityFind( id, (CDbEntity**) &dbEntity, DBLINE, DBPROFILE );

	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
	CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

	if (dbProfile != NULL)
	{
		dbCurve = dynamic_cast<CDbCurve*>( (*dbProfile)[0] );
	}

	return dbCurve;
}

// See also ArcPTR().
#if (_CI)
CReturn 
CSolveProcessApp::ArcPTR_CI( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CCiModel* ciModel = CPortal::CiModel();

	// -----------------------------------------------------
	//		Extract command data
	//
	ID		st_tan;
	bool	st_end;

	double	px, py, radius;

	st_tan = io_cmd->VarList().getInt( "stan", 0 );

	px = io_cmd->VarList().getReal( "px", UNDEFINED );
	py = io_cmd->VarList().getReal( "py", UNDEFINED );

	radius =io_cmd->VarList().getReal( "rad", 0. );

	st_end = io_cmd->VarList().getInt( "send", TRUE );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the reference curve

	CCiCurve* ciCurve = (CCiCurve*) ciModel->EntityGetByID( st_tan, (CILINE | CIARC) );
	if (ciCurve == NULL)
	{
		status.Internal( IDS_SOLVE_TANGENT_TYPE, st_tan );
		return status;
	}
	const CGeoCurve* st_curve = ciCurve->GeoCurve();

	//
	//	Pick point, which controls the solution discrimination...
	//	This may be off of the curve, if not explicitly defined.
	//
	C2dCoord hint_pt;
	CReturn	pick_ret;
	double hx, hy;
	pick_ret = io_cmd->getReal( "hx", &hx );
	pick_ret += io_cmd->getReal( "hy", &hy );
	if (pick_ret.isOkay())
	{
		hint_pt = C2dCoord( hx, hy );
	}
	else
	{
		if (st_end)
			hint_pt = st_curve->EndPt();
		else
			hint_pt = st_curve->StartPt();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Find all possible solutions.  Note, the known point is
	// treated as a degenerate arc having a radius of zero.

	C3dCoord	ct_pt[8];
	CGeoArc		degen( C3dCoord( px, py, 0. ), 0., CW );

	C3dCoord	tan_pt;
	double		distBest = UNDEFINED;
	int			indxBest = -1;

	int nsolns = CSolution::Blend( (*st_curve), degen, radius, NULL, NULL, ct_pt );
	if (nsolns > 0)
	{
		// -----------------------------------------------------
		// Given up to 8 solution centers, find the arc that
		//	best fits the criteria (defined above)
		//
		CGeoArc		soln;
		C3dCoord	tanpts[2];
		int			ntans;

		for (int indx = 0; indx < nsolns; indx++)
		{
			// Find the tangent point between the known
			// curve and the candidate solution.

			soln.Init( ct_pt[indx], radius, CW );

			ntans = CSolution::Intersect( soln, (*st_curve), FALSE, tanpts );
			if (ntans > 0)
			{
				// Determine if this is the solution whose
				// tangent point is nearest the hint point.

				C2dVec vec = hint_pt - tanpts[0];
				double dist = vec.Length();

				if (dist < distBest)
				{
					tan_pt = tanpts[0];
					indxBest = indx;
					distBest = dist;
				}
			}
		}
	}

	if (indxBest < 0)
	{
		status.Internal( IDS_SOLVE_NO_SOLUTION );
	}
	else
	{
		// Set the values in the command
		io_cmd->setReal( "tx", tan_pt.X() );
		io_cmd->setReal( "ty", tan_pt.Y() );
		io_cmd->setReal( "tz", tan_pt.Z() );

		io_cmd->setReal( "cx", ct_pt[indxBest].X() );
		io_cmd->setReal( "cy", ct_pt[indxBest].Y() );
		io_cmd->setReal( "cz", ct_pt[indxBest].Z() );
	}

	delete st_curve;

	return status;
}
#endif

void CSolveProcessApp::ArcInvert( CGeoArc* geoArc )
{
	const C3dCoord& ps = geoArc->StartPt();
	const C3dCoord& pe = geoArc->EndPt();

	if ( !ps.WithinTolXY( pe, SMALL ) )
	{
		C3dCoord pc = geoArc->CenterPt();
		int dir = geoArc->Dir();

		// NOTE: The 'mid' point calculation is more complicated
		// than necessary but simplifying it requires adding
		// overloaded operators to C3dCoord.
		C3dCoord mid = ps + ((pe - ps) * 0.5);
		C2dVec vec = mid - pc;
		pc = mid + vec;

		geoArc->Init( ps, pe, pc, -dir );
	}
}
