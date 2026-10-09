
#include "stdafx.h"

#include "ActionConst.h"
#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Portal.h"
#include "dbPoint.h"
#include "dbLine.h"
#include "dbArc.h"
#include "Solution.h"
#include "Int2d.h"
#include "ViewMgr.h"

#include "CreateProcess.h"

// Cloned from weGeoLine.cpp.
// NOTE: We may need to move these to some common area
// so as to avoid dependencies between weng & ci.
typedef enum
{
	ARC_PC_RAD		= 201,
	ARC_PS_PI_PE	= 202,
	ARC_PS_PE_RAD	= 203,
	ARC_PS_PC_ANG	= 204,
	ARC_PS_PE_PC	= 205,
	ARC_PS_TAN		= 206,
	ARC_TAN_PE		= 207,
	ARC_TAN_TAN		= 208
} eArcDefn;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create a new arc
//		Create:Arc: action=CICREATE, sncs=ARC_PC_RAD, 
//					xc=%f, yc=%f, zc=%f, rad=%f, dir=%d
//		Create:Arc: action=CICREATE, sncs=ARC_PS_PI_PE, 
//					xs=%f, ys=%f, zs=%f,
//					xi=%f, yi=%f, zi=%f,
//					xe=%f, ye=%f, ze=%f
//		Create:Arc: action=CICREATE, sncs=ARC_PS_PE_RAD, 
//					xs=%f, ys=%f, zs=%f,
//					xe=%f, ye=%f, ze=%f, rad=%f, dir=%d
//		Create:Arc: action=CICREATE, sncs=ARC_PS_PC_ANG, 
//					xs=%f, ys=%f, zs=%f,
//					xc=%f, yc=%f, zc=%f, ang=%f
//		Create:Arc: action=CICREATE, sncs=ARC_PS_PE_PC, 
//					xs=%f, ys=%f, zs=%f,
//					xe=%f, ye=%f, ze=%f,
//					xc=%f, yc=%f, zc=%f, dir=%d
//		Create:Arc: action=CICREATE, sncs=ARC_PS_TAN, 
//					xs=%f, ys=%f, zs=%f,
//					te=%d, xe=%f, ye=%f, ze=%f,
//					rad=%f, dir=%d
//		Create:Arc: action=CICREATE, sncs=ARC_TAN_PE, 
//					ts=%d, xs=%f, ys=%f, zs=%f,
//					xe=%f, ye=%f, ze=%f,
//					rad=%f, dir=%d
//		Create:Arc: action=CICREATE, sncs=ARC_TAN_TAN, 
//					ts=%d, xs=%f, ys=%f, zs=%f,
//					te=%d, xe=%f, ye=%f, ze=%f,
//					rad=%f, dir=%d
//		Returns (s) id
//
// Update an existing arc
//		commands are same a those for creating arcs except:
//			Create:Arc: action=CIUPDATE, id=%d, ...
//		Returns (s) id
//
// Query an arc
//		Create:Arc: action=CIQUERY, id=%d
//		Returns (s) id
//				(d) xc
//				(d) yc
//				(d) zc
//				(d) as
//				(d) ae
//				(i) dir
//				(d) rad
//
CReturn 
CCreateProcessApp::Arc( CCommand* io_cmd )
{
	CReturn		status;
	eWeAction	action;
	eArcDefn	sncs;

	action = (eWeAction) io_cmd->VarList().getInt( "action", IUNDEFINED );
	sncs = (eArcDefn) io_cmd->VarList().getInt( "sncs", IUNDEFINED );

	if (action == WECREATE || action == WEUPDATE)
	{
		switch (sncs)
		{
		case ARC_PC_RAD:
			status = arc_pc_rad( io_cmd );
			break;
		case ARC_PS_PI_PE:
			status = arc_ps_pi_pe( io_cmd );
			break;
		case ARC_PS_PE_RAD:
			status = arc_ps_pe_rad( io_cmd );
			break;
		case ARC_PS_PC_ANG:
			status = arc_ps_pc_ang( io_cmd );
			break;
		case ARC_PS_PE_PC:
			status = arc_ps_pe_pc( io_cmd );
			break;
		case ARC_PS_TAN:
			status = arc_ps_tan( io_cmd );
			break;
		case ARC_TAN_PE:
			status = arc_tan_pe( io_cmd );
			break;
		case ARC_TAN_TAN:
			status = arc_tan_tan( io_cmd );
			break;
		default:
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Arc(#1)" );
			io_cmd->setInt( "id", 0 );  // failed.
			break;
		}
	}
	else if (action == WEQUERY)
	{
		status = arc_query( io_cmd );
	}
	else
	{
		// status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Arc(#2)" );
		status = OldArc( io_cmd );  // punt
		// io_cmd->setInt( "id", 0 );  // failed.
	}

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_PC_RAD, 
//				xc=%f, yc=%f, zc=%f, rad=%f, dir=%d
CReturn
CCreateProcessApp::arc_pc_rad( CCommand* io_cmd )
{
	CReturn		status;
	CDbArc*		dbArc;
	C3dCoord	ps, pc;
	double		rad;
	int			dir;
	ID			id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

	dbArc = GetArc( io_cmd );

	if (dbArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_pc_rad(#1)" );
	}
	else
	{
		pc = GetPt( io_cmd, 'c' );

		rad = io_cmd->VarList().getReal( "rad", 0. );
		dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );

		ps.XYZ( (pc.X() + rad), pc.Y(), pc.Z() );

		status = dbArc->Init(
			dbArc->Tool(), dbArc->Workplane(),
			ps, ps, pc, ((dir > 0) ? CCW : CW) );

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_PS_PI_PE, 
//				xs=%f, ys=%f, zs=%f,
//				xi=%f, yi=%f, zi=%f,
//				xe=%f, ye=%f, ze=%f
//
//	Essentially cloned from CSolveProcessApp::ArcSEP()
//
CReturn
CCreateProcessApp::arc_ps_pi_pe( CCommand* io_cmd )
{
	CReturn		status;
	C2dUnitVec	vecA, vecB;
	C3dCoord	ps, pe, pi, pc;
	CDbArc*		dbArc;
	double		d1, d2, d3, dot;
	double		c1, c2, c3, cc;
	ID			id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

	dbArc = GetArc( io_cmd );

	if (dbArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_ps_pi_pe(#1)" );
	}
	else
	{
		ps = GetPt( io_cmd, 's' );
		pi = GetPt( io_cmd, 'i' );
		pe = GetPt( io_cmd, 'e' );

		// -----------------------------------------------------
		// Dot the deltas to d...
		//

		d1 = ((pi.X() - ps.X()) * (pe.X() - ps.X())) + ((pi.Y() - ps.Y()) * (pe.Y() - ps.Y()));
		d2 = ((pi.X() - pe.X()) * (ps.X() - pe.X())) + ((pi.Y() - pe.Y()) * (ps.Y() - pe.Y()));
		d3 = ((ps.X() - pi.X()) * (pe.X() - pi.X())) + ((ps.Y() - pi.Y()) * (pe.Y() - pi.Y()));

		// -----------------------------------------------------
		// Now, information on the circum-radius and circum-center
		//

		c1 = d2 * d3;
		c2 = d3 * d1;
		c3 = d1 * d2;
		cc = c1 + c2 + c3;

		pc.X( (((c2+c3) * ps.X()) + ((c3+c1)*pe.X()) + ((c1+c2)*pi.X())) / (2.0*cc) );
		pc.Y( (((c2+c3) * ps.Y()) + ((c3+c1)*pe.Y()) + ((c1+c2)*pi.Y())) / (2.0*cc) );
		pc.Z( ps.Z() );

		vecA.Init( (ps.X() - pc.X()), (ps.Y() - pc.Y()) );
		vecB.Init( (pi.X() - pc.X()), (pi.Y() - pc.Y()) );
		dot = (vecA ^ vecB);

		status = dbArc->Init(
			dbArc->Tool(), dbArc->Workplane(),
			ps, pe, pc, ((dot > 0) ? CCW : CW) );

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_PS_PE_RAD, 
//				xs=%f, ys=%f, zs=%f,
//				xe=%f, ye=%f, ze=%f,
//				rad=%f, dir=%d, big=%d
//
//	Essentially cloned from CSolveProcessApp::ArcSER()
//
CReturn
CCreateProcessApp::arc_ps_pe_rad( CCommand* io_cmd )
{
	CReturn		status;
	CGeoArc		solnA, solnB;
	C3dCoord	ps, pe, pc;
	CDbArc*		dbArc;
	double		cx[2], cy[2];
	double		rad, angle;
	double		chord_dist;
	double		chord_angle;
	double		dx, dy;
	int			dir, big, which;
	ID			id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

	dbArc = GetArc( io_cmd );

	if (dbArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_ps_pe_rad(#1)" );
	}
	else
	{
		ps = GetPt( io_cmd, 's' );
		pe = GetPt( io_cmd, 'e' );
		rad = io_cmd->VarList().getReal( "rad", 0. );
		dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );
		big = io_cmd->VarList().getInt( "big", IUNDEFINED );

		// -----------------------------------------------------
		// Create intersection of two equal arcs to get
		//	potential centers...
		//

		dx = pe.X() - ps.X();
		dy = pe.Y() - ps.Y();
		chord_dist = sqrt( dx*dx + dy*dy ) / 2.0;
		chord_angle = atan2( dy, dx );

		if ((chord_dist - rad) > SMALL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_ps_pe_rad(#2)" );
		}
		else
		{
			angle = acos( chord_dist / rad );

			cx[0] = ps.X() + cos( chord_angle + angle) * rad;
			cy[0] = ps.Y() + sin( chord_angle + angle) * rad;

			cx[1] = ps.X() + cos( chord_angle - angle) * rad;
			cy[1] = ps.Y() + sin( chord_angle - angle) * rad;

			solnA.Init( ps, pe, C3dCoord( cx[0], cy[0], 0. ), dir );
			solnB.Init( ps, pe, C3dCoord( cx[1], cy[1], 0. ), dir );

			which = big ^ (solnA.Length2d() > solnB.Length2d() );

			if (which == 0)
			{
				status = dbArc->Init(
					dbArc->Tool(), dbArc->Workplane(),
					ps, pe, solnA.CenterPt(), dir );
			}
			else
			{
				status = dbArc->Init(
					dbArc->Tool(), dbArc->Workplane(),
					ps, pe, solnB.CenterPt(), dir );
			}
		}

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_PS_PC_ANG, 
//				xs=%f, ys=%f, zs=%f,
//				xc=%f, yc=%f, zc=%f, ang=%f
CReturn
CCreateProcessApp::arc_ps_pc_ang( CCommand* io_cmd )
{
	CReturn		status;
	CDbArc*		dbArc;
	C3dCoord	ps, pe, pc;
	double		ang, rad, ae;
	double		dx, dy;
	ID			id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

	dbArc = GetArc( io_cmd );

	if (dbArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_ps_pe_pc(#1)" );
	}
	else
	{
		ps = GetPt( io_cmd, 's' );
		pc = GetPt( io_cmd, 'c' );
		
		ang = io_cmd->VarList().getReal( "ang", 0. );

		dx = ps.X() - pc.X();
		dy = ps.Y() - pc.Y();

		rad = sqrt( dx*dx + dy*dy );
		ae = ang + atan2( dy, dx );

		pe.X( pc.X() + rad * cos(ae) );
		pe.Y( pc.Y() + rad * sin(ae) );
		pe.Z( pc.Z() );

		status = dbArc->Init(
			dbArc->Tool(), dbArc->Workplane(),
			ps, pe, pc, ((ang > 0) ? CCW : CW) );

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_PS_PE_PC, 
//				xs=%f, ys=%f, zs=%f,
//				xe=%f, ye=%f, ze=%f,
//				xc=%f, yc=%f, zc=%f, dir=%d
CReturn
CCreateProcessApp::arc_ps_pe_pc( CCommand* io_cmd )
{
	CReturn		status;
	CDbArc*		dbArc;
	C3dCoord	ps, pe, pc;
	int			dir;
	ID			id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

	dbArc = GetArc( io_cmd );

	if (dbArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_ps_pe_pc(#1)" );
	}
	else
	{
		ps = GetPt( io_cmd, 's' );
		pe = GetPt( io_cmd, 'e' );
		pc = GetPt( io_cmd, 'c' );

		dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );

		status = dbArc->Init(
			dbArc->Tool(), dbArc->Workplane(),
			ps, pe, pc, dir );

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_PS_TAN, 
//				xs=%f, ys=%f, zs=%f,
//				te=%d, xe=%f, ye=%f, ze=%f,
//				rad=%f, dir=%d
//
//	Essentially cloned from CSolveProcessApp::ArcPTR()
//
CReturn
CCreateProcessApp::arc_ps_tan( CCommand* io_cmd )
{
	CReturn		status;
	C3dCoord	ps, hint;
	CGeoArc		soln;
	CDbArc*		dbArc;
	CDbArc*		refArc;
	CGeoArc*	geoArc;
	double		rad;
	int			dir;
	ID			te, id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

	dbArc = GetArc( io_cmd );

	te = io_cmd->VarList().getInt( "te", 0 );

	model->EntityFind( te, (CDbEntity**) &refArc, DBARC, DBARC );

	if (dbArc == NULL || refArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_ps_tan(#1)" );
	}
	else
	{
		ps = GetPt( io_cmd, 's' );
		hint = GetPt( io_cmd, 'e' );
		rad = io_cmd->VarList().getReal( "rad", 0. );
		dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );

		geoArc = refArc->Arc();

		status = arc_pt_tan( ps, (*geoArc), hint, rad, dir, &soln );

		if ( !status.IsOk() )
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_ps_tan(#2)" );
		}
		else
		{
			status = dbArc->Init(
				dbArc->Tool(), dbArc->Workplane(),
				soln.StartPt(), soln.EndPt(), soln.CenterPt(), dir );
		}

		delete geoArc;

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_TAN_PE, 
//				ts=%d, xs=%f, ys=%f, zs=%f,
//				xe=%f, ye=%f, ze=%f,
//				rad=%f, dir=%d
//
//	Essentially cloned from CSolveProcessApp::ArcPTR()
//
CReturn
CCreateProcessApp::arc_tan_pe( CCommand* io_cmd )
{
	CReturn		status;
	C3dCoord	pe, hint;
	CGeoArc		soln;
	CDbArc*		dbArc;
	CDbArc*		refArc;
	CGeoArc*	geoArc;
	double		rad;
	int			dir;
	ID			ts, id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

	dbArc = GetArc( io_cmd );

	ts = io_cmd->VarList().getInt( "ts", 0 );

	model->EntityFind( ts, (CDbEntity**) &refArc, DBARC, DBARC );

	if (dbArc == NULL || refArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_tan_pe(#1)" );
	}
	else
	{
		hint = GetPt( io_cmd, 's' );
		pe = GetPt( io_cmd, 'e' );
		rad = io_cmd->VarList().getReal( "rad", 0. );
		dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );

		geoArc = refArc->Arc();

		status = arc_pt_tan( pe, (*geoArc), hint, rad, dir, &soln );

		if ( !status.IsOk() )
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_tan_pe(#2)" );
		}
		else
		{
			status = dbArc->Init(
				dbArc->Tool(), dbArc->Workplane(),
				soln.StartPt(), soln.EndPt(), soln.CenterPt(), dir );
		}

		delete geoArc;

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

//	Create:Arc: action=CICREATE, sncs=ARC_TAN_TAN, 
//				ts=%d, xs=%f, ys=%f, zs=%f,
//				te=%d, xe=%f, ye=%f, ze=%f,
//				rad=%f, dir=%d, big=%d
CReturn
CCreateProcessApp::arc_tan_tan( CCommand* io_cmd )
{
	CReturn		status;
	C3dCoord	solnCenter[8];
	CGeoArc		soln;
	CDbArc*		dbArc;
	CDbCurve*	dbCrvA;
	CDbCurve*	dbCrvB;
	CGeoCurve*	geoCrvA;
	CGeoCurve*	geoCrvB;
	C3dCoord	hintA, hintB;
	C3dCoord	pc;
	C3dCoord	psBest;
	C3dCoord	peBest;
	C3dCoord	pcBest;
	double		rad, ang;
	double		dist, distBest;
	double		dotA, dotB;
	int			dir, big;
	int			nsolns, indx;
	ID			ts, te, id;

	CModel* model = CPortal::Model();

	id = 0;  // assume failure.

#if PRIOR_TO_2022_03_11
	dbArc = GetArc( io_cmd );
#endif

	ts = io_cmd->VarList().getInt( "ts", 0 );
	te = io_cmd->VarList().getInt( "te", 0 );

	model->EntityFind( ts, (CDbEntity**) &dbCrvA, DBLINE, DBARC );
	model->EntityFind( te, (CDbEntity**) &dbCrvB, DBLINE, DBARC );

#if PRIOR_TO_2022_03_11
	if (dbArc == NULL || dbCrvA == NULL || dbCrvB == NULL)
	{
		status.Internal(IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_tan_tan(#1)");
	}
	else
#else
	if (dbCrvA == NULL || dbCrvB == NULL)
	{
		status.Internal(IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_tan_tan(#1)");
		io_cmd->setInt("id", id);
		return status;
	}

	dbArc = GetArc(io_cmd);
#endif
	{
		hintA = GetPt( io_cmd, 's' );
		hintB = GetPt( io_cmd, 'e' );

		rad = io_cmd->VarList().getReal( "rad", 0. );
		dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );
		big = io_cmd->VarList().getInt( "big", IUNDEFINED );

		geoCrvA = dbCrvA->Curve();
		geoCrvB = dbCrvB->Curve();

		distBest = UNDEFINED;

		nsolns = CSolution::ArcTT( (*geoCrvA), (*geoCrvB), rad, solnCenter );
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
			C2dUnitVec vecA = hintB - hintA;
			C2dUnitVec vecB = hintA - hintB;

			for (indx = 0; indx < nsolns; ++indx)
			{
				pc = solnCenter[indx];

				soln.Init( pc, rad, CW );

				intsctA.CrvCrv( (*geoCrvA), soln );
				intsctB.CrvCrv( (*geoCrvB), soln );

				if (intsctA.Count() > 0 && intsctB.Count() > 0)
				{
					const C2dCoord& ptA = intsctA.Point(0);
					const C2dCoord& ptB = intsctB.Point(0);

					// The 'visibility' of the soln end points to the center.
					C2dUnitVec vecC = pc - ptA;
					C2dUnitVec vecD = pc - ptB;

					dotA = vecA * vecC;
					dotB = vecB * vecD;

					if (dotA >= 0 && dotB >= 0)
					{
						C2dVec vecE = hintA - ptA;
						C2dVec vecF = hintB - ptB;

						dist = vecE.Length() + vecF.Length();
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

		delete geoCrvA;
		delete geoCrvB;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Set the values in the command
		//
		if (distBest < UNDEFINED)
		{
			if (big < IUNDEFINED && dir < IUNDEFINED)
			{
				soln.Init( psBest, peBest, pcBest, dir );

				ang = soln.IncludedAngle();
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

			psBest.Z( hintA.Z() );
			peBest.Z( hintA.Z() );
			pcBest.Z( hintA.Z() );

			status = dbArc->Init(
				dbArc->Tool(), dbArc->Workplane(),
				psBest, peBest, pcBest, dir );
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_tan_tan(#2)" );
		}

		id = dbArc->Id();
	}

	if ((id > 0) && !status.IsOk())
	{
		model->EntityDelete( id );
		id = 0;
	}
	
	io_cmd->setInt( "id", id );

	return status;
}

CReturn
CCreateProcessApp::arc_query( CCommand* io_cmd )
{
	CReturn		status;
	CDbArc*		dbArc;
	C3dCoord	pc;
	double		as, ae, rad;
	int			dir;

	dbArc = GetArc( io_cmd );

	if (dbArc == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_query(#1)" );
		io_cmd->setInt( "id", 0 );  // failed.
	}
	else
	{
		CGeoArc* geoArc = dbArc->Arc();

		geoArc->Angles( &as, &ae );

		pc = geoArc->CenterPt();
		rad = geoArc->Radius();
		dir = geoArc->Dir();

		io_cmd->setReal( "as", as );
		io_cmd->setReal( "ae", ae );

		io_cmd->setReal( "xc", pc.X() );
		io_cmd->setReal( "yc", pc.Y() );
		io_cmd->setReal( "zc", pc.Z() );

		io_cmd->setReal( "rad", rad );
		io_cmd->setInt( "dir", dir );

		delete geoArc;
	}

	return status;
}

CReturn
CCreateProcessApp::arc_pt_tan(
				const C3dCoord&	ps,
				const CGeoArc&	geoArc,
				const C3dCoord&	hint,
				double			rad,
				int				dir,
				CGeoArc*		result )
{
	CReturn		status;
	C3dCoord	ct_pt[8];	// possible center points
	C3dCoord	tanpts[2];
	C3dCoord	tan_pt;
	C2dVec		vec;
	CGeoArc		candidate;
	CGeoArc		degen;
	double		dist, distBest;
	int			indx, indxBest;
	int			nsolns, ntans;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Find all possible solutions.  Note, the known point is
	// treated as a degenerate arc having a radius of zero.

	degen.Init( ps, 0., CW );

	nsolns = CSolution::Blend( geoArc, degen, rad, NULL, NULL, ct_pt );
	if (nsolns <= 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_pt_tan(#1)" );
	}
	else
	{
		// -----------------------------------------------------
		// Given up to 8 solution centers, find the arc that
		//	best fits the criteria (defined above)
		//

		distBest = UNDEFINED;
		indxBest = -1;

		for (indx = 0; indx < nsolns; indx++)
		{
			// Find the tangent point between the known
			// curve and the candidate solution.

			candidate.Init( ct_pt[indx], rad, CW );

			ntans = CSolution::Intersect( candidate, geoArc, FALSE, tanpts );
			if (ntans > 0)
			{
				// Determine if this is the solution whose
				// tangent point is nearest the hint point.

				tanpts[0].Z( ps.Z() );

				vec = hint - tanpts[0];
				dist = vec.Length();

				if (dist < distBest)
				{
					tan_pt = tanpts[0];
					indxBest = indx;
					distBest = dist;
				}
			}
		}

		if (indxBest < 0)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::arc_pt_tan(#2)" );
		}
		else
		{
			result->Init( ps, tan_pt, ct_pt[indxBest], dir );
		}
	}

	return status;
}

CDbArc*
CCreateProcessApp::GetArc( CCommand* io_cmd )
{
	eWeAction	action;
	ID			id;

	CModel* model = CPortal::Model();

	action = (eWeAction) io_cmd->VarList().getInt( "action", IUNDEFINED );

	if (action == WECREATE)
	{
		CDbArc* dbArc;
		model->EntityCreate( DBARC, (CDbEntity**) &dbArc );
		if (dbArc != NULL)
		{
			dbArc->Tool( model->ActiveTool() );
			dbArc->Workplane( model->ActiveWorkplane() );

			if (model->ActivePattern() != NULL)
				model->ActivePattern()->Append( dbArc );
		}
		return dbArc;
	}
	else
	{
		CDbArc* dbArc;

		id = io_cmd->VarList().getInt( "id", 0 );
		model->EntityFind( id, (CDbEntity**) &dbArc, DBARC, DBARC );

		return dbArc;
	}
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// The old form of the arc portal commands.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// ==================================================================
// Create:Arc:
//			sx, sy, sz
//			ex, ey, ez
//			cx, cy, cz
//			dir				// +1 ccw, -1 cw
//			[id]
//
CReturn 
CCreateProcessApp::OldArc( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn	ret;
	ID		id = 0;
	bool	modify;

	// -----------------------------------------------------
	//		Extract command
	//
	double	sx, sy, sz;
	double	ex, ey, ez;
	double	cx, cy, cz;
	int		dir;
	
	id = io_cmd->VarList().getInt( "id", 0 );
	dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );

	ret += io_cmd->getReal( "sx", &sx );
	ret += io_cmd->getReal( "sy", &sy );
	ret += io_cmd->getReal( "sz", &sz );

	ret += io_cmd->getReal( "ex", &ex );
	ret += io_cmd->getReal( "ey", &ey );
	ret += io_cmd->getReal( "ez", &ez );

	ret += io_cmd->getReal( "cx", &cx );
	ret += io_cmd->getReal( "cy", &cy );
	ret += io_cmd->getReal( "cz", &cz );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_CREATE_PARAM_MISSING );
		return ret;
	}


	C2dVec	vecA = C2dCoord( sx, sy ) - C2dCoord( cx, cy );
	C2dVec	vecB = C2dCoord( ex, ey ) - C2dCoord( cx, cy );
	double	lenA = vecA.Length();
	double	lenB = vecB.Length();
	if ( EQUAL( lenA, 0.0 )
		|| EQUAL( lenB, 0.0 ) 
		|| !EQUAL( lenA, lenB )
		|| !EQUAL( sz, cz )
		|| !EQUAL( ez, cz ) )
	{
		ret.Internal( IDS_CREATE_DEGENERATE );
		return ret;
	}

	// -----------------------------------------------------
	//		Check for modify
	//
	CDbArc*	arc = NULL;

	if (id)
	{
		ret += model.EntityFind( id, (CDbEntity**)&arc, DBARC, DBARC );
		ASSERT( arc != NULL);
		if ( arc->Type() != DBARC )
		{
			ret.Internal( IDS_MODIFY_TYPE );
			return ret;
		}
	}
	modify = (arc != NULL);

	// -----------------------------------------------------
	//		Now, get the arc
	//
	CDbWorkplane* work = NULL;
	CDbTool* tool = NULL;

	if (modify)
	{
		tool = (CDbTool*)arc->Tool();
		work = (CDbWorkplane*)arc->Workplane();

		if (dir == IUNDEFINED)
			dir = arc->Dir();
	}
	else
	{
		tool = model.ActiveTool();
		work = model.ActiveWorkplane();
		if (!tool || !work)
			return ret;

		ret += model.EntityCreate( DBARC, (CDbEntity**)&arc );

		if (arc != NULL && model.ActivePattern() != NULL)
			model.ActivePattern()->Append( arc );

		if (dir == IUNDEFINED)
			dir = CW;  // default

		dir = dir * work->ToolUp();
	}

	id = arc->Id();

	// -----------------------------------------------------
	// Fill the contents
	//
	if (ret.isOkay())
	{
		if ( modify )
		{
			arc->ConditionalDisassociate( arc->DbStartPt() );
			arc->ConditionalDisassociate( arc->DbEndPt() );
		}

		arc->Init( tool, work, 
						C3dCoord( sx, sy, sz ), 
						C3dCoord( ex, ey, ez ), 
						C3dCoord( cx, cy, cz ), 
						dir );

		if ( modify )
			arc->ModifyFlag( true );

		io_cmd->setInt( "id", id );

		io_cmd->getViewMgr().ModelSet( model );
		io_cmd->getViewMgr().Refresh( id, TRUE );
	}

	return ret;
}

