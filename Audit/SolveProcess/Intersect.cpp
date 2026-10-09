
#include "stdafx.h"
#include "float.h"
#include "cmn_resource.h"
#include "MathConst.h"

#include "Int2d.h"
#include "3dCoord.h"
#include "DbEntity.h"
#include "DbPoint.h"
#include "DbCurve.h"
#include "DbProfile.h"
#include "Model.h"
#include "Profile.h"
#include "Conversion.h"
#include "Solution.h"
#include "IntsctRec.h"
#include "Portal.h"

#include "TrimExtDlg.h"
#include "ViewMgr.h"

#include "SolveProcess.h"

static CIntsctRecList g_intersections;

// Solve:MultiIntersect: idA=%d, idB=%d
CReturn 
CSolveProcessApp::MultiIntersect( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbCurveList dbCurvesA;
	CDbCurveList dbCurvesB;
	C3dCoord soln[2];
	int solnCount;

	g_intersections.DestructiveFlush();

	ID idA = 0;
	ID idB = 0;

	status += io_cmd->getInt( "idA", (int*) &idA );
	status += io_cmd->getInt( "idB", (int*) &idB );

	if ( status.IsOk() )
	{
		CurvesGet( io_cmd->getModel(), idA, &dbCurvesA );
		CurvesGet( io_cmd->getModel(), idB, &dbCurvesB );
	}

	if (dbCurvesA.Count() < 1 || dbCurvesB.Count() < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::MultiIntersect()" );
		return status;
	}

	for (int indxA = 0; indxA < dbCurvesA.Count(); ++indxA)
	{
		CDbCurve* dbCurveA = dbCurvesA[indxA];

		CGeoCurve* curveA = dbCurveA->Curve();

		for (int indxB = 0; indxB < dbCurvesB.Count(); ++indxB)
		{
			CDbCurve* dbCurveB = dbCurvesB[indxB];

			CGeoCurve* curveB = dbCurveB->Curve();

			solnCount = CSolution::Intersect( (*curveA), (*curveB), TRUE, soln );
			for (int sndx = 0; sndx < solnCount; ++sndx)
			{
				g_intersections.Append( new CIntsctRec( soln[sndx], dbCurveA, dbCurveB ) );
			}

			delete curveB;
		}

		delete curveA;
	}

	io_cmd->setInt( "count", g_intersections.Count() );

	return status;
}

// Solve:MultiIntersectCount:
CReturn 
CSolveProcessApp::MultiIntersectCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->setInt( "count", g_intersections.Count() );

	return ( CReturn( STATUS_OKAY) );
}

// Solve:MultiIntersectGet: index=%d
CReturn 
CSolveProcessApp::MultiIntersectGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int indx = -1;
	io_cmd->getInt( "index", &indx );

	if (indx < 0 || indx > g_intersections.Count())
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::MultiIntersectGet()" );
		return status;
	}

	CIntsctRec* rec = g_intersections[indx];
	io_cmd->setReal( "x",  rec->Pt().X() );
	io_cmd->setReal( "y",  rec->Pt().Y() );
	io_cmd->setReal( "z",  rec->Pt().Z() );
	io_cmd->setInt( "idA", rec->CurveA()->Id() );
	io_cmd->setInt( "idB", rec->CurveB()->Id() );

	return status;
}

// Solve:MultiIntersectFlush:
CReturn 
CSolveProcessApp::MultiIntersectFlush( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_intersections.DestructiveFlush();

	return ( CReturn( STATUS_OKAY) );
}

void
CSolveProcessApp::CurvesGet( const CModel& model, ID id, CDbCurveList* dbCurves )
{
	CDbEntity* dbEntity;
	model.EntityFind( id, &dbEntity, DBLINE, DBPROFILE );

	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
	if (dbCurve != NULL)
	{
		dbCurves->Append( dbCurve );
	}
	else
	{
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );
		if (dbProfile != NULL)
		{
			int count = dbProfile->Count();
			for (int indx = 0; indx < count; ++indx)
			{
				dbCurve = dynamic_cast<CDbCurve*>( (*dbProfile)[indx] );
				dbCurves->Append( dbCurve );
			}
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE 
// One use of Solve:Intersect: has been superceded by Solve:TrimExtend:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ==================================================================
// Solve:Intersect:
//
//		Find the intersection of two entites:
//			tan1				ID of entity to start with
//			end1				TRUE if end, FALSE if start of entity
//			tan2				ID of entity to end with
//			end2				TRUE if end, FALSE if start of entity
//
//		Optional inputs:
//			px1, py1			Pick Points (as opposed to start/end of entities)
//			px2, py2
//
//			onseg				1 if force intersection on segments (trim only)
//
//		Out:
//			ix, iy			Intersection, nearest selected ends
//
// TODO: Break out into bite-size chunks
//
CReturn 
CSolveProcessApp::Intersect( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	ID			tan1;
	BOOL		end1;
	ID			tan2;
	BOOL		end2;
	int			onseg;
	int			solnum;

	CViewMgr& view = io_cmd->getViewMgr();

	// -----------------------------------------------------
	//		Extract command
	//

	ret += io_cmd->getInt( "tan1", (int*)&tan1 );
	ret += io_cmd->getInt( "end1", &end1 );

	ret += io_cmd->getInt( "tan2", (int*)&tan2 );
	ret += io_cmd->getInt( "end2", &end2 );

	onseg = io_cmd->VarList().getInt( "onseg", 0 );

	solnum = -1;
	io_cmd->setInt( "soln", solnum );  // assume failure

	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//
	CDbEntity*	db_entity1;
	ret += io_cmd->getModel().EntityFind( tan1, &db_entity1, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, tan1 );
		return ret;
	}
	CDbCurve* db_curve1 = dynamic_cast<CDbCurve*>(db_entity1);
	if (!db_curve1)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, tan1 );
		return ret;
	}

	CDbEntity*	db_entity2;
	ret += io_cmd->getModel().EntityFind( tan2, &db_entity2, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, tan2 );
		return ret;
	}
	CDbCurve* db_curve2 = dynamic_cast<CDbCurve*>(db_entity2);
	if (!db_curve2)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, tan2 );
		return ret;
	}

	const CGeoCurve* curve1 = db_curve1->Curve();
	const CGeoCurve* curve2 = db_curve2->Curve();
	//
	//	Pick points, which control the solution discrimination...
	//	This may be from the curve, if not explicitly defined.
	//
	CReturn	pick_ret;

	double	px, py;
	C3dCoord pt1 = (end1 ? curve1->EndPt() : curve1->StartPt());

	pick_ret = io_cmd->getReal( "px1", &px );
	pick_ret += io_cmd->getReal( "py1", &py );
	if (pick_ret.isOkay())
		pt1 = C3dCoord( px, py, 0.0 );

	double u1 = 0.0;
	C3dCoord pick1;
	curve1->PointClosest(pt1, &pick1, &u1);
	//
	//
	C3dCoord pt2 = (end2 ? curve2->EndPt() : curve2->StartPt());

	pick_ret = io_cmd->getReal( "px2", &px );
	pick_ret += io_cmd->getReal( "py2", &py );
	if (pick_ret.isOkay())
		pt2 = C3dCoord( px, py, 0.0 );

	double u2 = 0.0;
	C3dCoord pick2;
	curve2->PointClosest(pt2, &pick2, &u2);
	//
	// -----------------------------------------------------
	//
	CInt2d int2d(SMALL, onseg);
	int hitnum = int2d.CrvCrv(*curve1, *curve2);

	if (hitnum > 0)
	{
		int besthit[2] = {-1, -1};
		double bestdist[2] = {DBL_MAX, DBL_MAX};
		bool bestend[2];
		//
		// Smallest move that includes the pick point
		for (int idx=0; idx<hitnum; idx++)
		{
			double uhit = int2d.Uparam(idx);
			if ( (uhit > 1.0)
				|| BETWEEN(uhit, u1, 1.0))
			{
				double dist = u1 - uhit;
				if (dist < bestdist[0])
				{
					besthit[0] = idx;
					bestdist[0] = dist;
					bestend[0] = true;
				}
			}
			if ( (uhit < 0.0)
				|| BETWEEN(0.0, u1, uhit))
			{
				double dist = uhit - u1;
				if (dist < bestdist[0])
				{
					besthit[0] = idx;
					bestdist[0] = dist;
					bestend[0] = false;
				}
			}
			//
			//
			uhit = int2d.Vparam(idx);
			if ( (uhit > 1.0)
				|| BETWEEN(uhit, u2, 1.0))
			{
				double dist = u2 - uhit;
				if (dist < bestdist[1])
				{
					besthit[1] = idx;
					bestdist[1] = dist;
					bestend[1] = true;
				}
			}
			if ( (uhit < 0.0)
				|| BETWEEN(-999, u2, uhit))
			{
				double dist = uhit - u2;
				if (dist < bestdist[1])
				{
					besthit[1] = idx;
					bestdist[1] = dist;
					bestend[1] = false;
				}
			}
		}
		//
		// -----------------------------------------------------
		//
		solnum = besthit[0];
	//	if (besthit[0] != besthit[1])
#if RESTORE
		if (hitnum > 1)
		{
			// Resolve interactively
			//
			db_curve1->Hide();
			db_curve2->Hide();
			view.Refresh();
			//
			// set up dialog with the various solutions
			//
			TrimExtDlg dlg(view.getCWnd());
		
			for (int idx=0; idx<hitnum; idx++)
			{
				int hitidx = idx + besthit[0];
				if (hitidx >= hitnum)
				{ hitidx = 0; }
				//
				CGeoCurve* tmp1 = (CGeoCurve*)curve1->Clone();
				if (bestend[0])
				{ tmp1->EndPt(int2d.Point(hitidx)); }
				else
				{ tmp1->StartPt(int2d.Point(hitidx)); }

				CGeoCurve* tmp2 = (CGeoCurve*)curve2->Clone();
				if (bestend[1])
				{ tmp2->EndPt(int2d.Point(hitidx)); }
				else
				{ tmp2->StartPt(int2d.Point(hitidx)); }

				dlg.addSolution(tmp1, tmp2);
			}
			dlg.setView(view.ActiveView());

			dlg.DoModal();
			solnum = besthit[0] + dlg.getSolution();
			if (solnum >= hitnum)
			{ solnum = 0; }

			// Note, the curves in the dialog are deleted by the dialog
			//
			//
			db_curve1->Seek();
			db_curve2->Seek();
			view.Refresh();
		}
#endif

		C2dCoord intpt = int2d.Point(solnum);
		C3dCoord solpt;
		curve1->PointClosest(intpt, &solpt, &u1);

		delete curve1;
		delete curve2;

		io_cmd->setReal( "ix", solpt.X() );
		io_cmd->setReal( "iy", solpt.Y() );
		io_cmd->setReal( "iz", solpt.Z() );
		io_cmd->setInt("end1", bestend[0]);
		io_cmd->setInt("end2", bestend[1]);
	}

	io_cmd->setInt( "soln", solnum );

	return ret;
}


// ==================================================================
// Solve:TrimExtend: crv1=%d, crv2=%d, px1=%f, py1=%f, px2=%f, py2=%f, onseg=%d, hwnd=%d
//
CReturn 
CSolveProcessApp::TrimExtend( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	CGeoCurveArray	candidatesA;
	CGeoCurveArray	candidatesB;

	CString		resultA;
	CString		resultB;
	C2dCoord	intsct;
	C3dCoord	pt1;
	C3dCoord	pt2;
	double		px1, py1;
	double		px2, py2;
	double		uparam;
	int			indx, jndx;
	ID			crv1;
	ID			crv2;
	int			hWnd;
	int			onseg;
	int			top, left;

	CViewMgr& view = io_cmd->getViewMgr();

	crv1 = io_cmd->VarList().getInt( "crv1", 0 );
	crv2 = io_cmd->VarList().getInt( "crv2", 0 );

	px1 = io_cmd->VarList().getReal( "px1", UNDEFINED );
	py1 = io_cmd->VarList().getReal( "py1", UNDEFINED );

	px2 = io_cmd->VarList().getReal( "px2", UNDEFINED );
	py2 = io_cmd->VarList().getReal( "py2", UNDEFINED );

	onseg = io_cmd->VarList().getInt( "onseg", 0 );
	hWnd = io_cmd->VarList().getInt( "hWnd", 0 );

	top = io_cmd->VarList().getInt( "top", 20 );
	left = io_cmd->VarList().getInt( "left", 20 );

	// -----------------------------------------------------
	//
	CDbEntity*	db_entity1;
	ret += io_cmd->getModel().EntityFind( crv1, &db_entity1, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, crv1 );
		return ret;
	}
	CDbCurve* db_curve1 = dynamic_cast<CDbCurve*>(db_entity1);
	if (!db_curve1)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, crv1 );
		return ret;
	}

	CDbEntity*	db_entity2;
	ret += io_cmd->getModel().EntityFind( crv2, &db_entity2, DBLINE, DBARC );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_SOLVE_TANGENT_MISSING, crv2 );
		return ret;
	}
	CDbCurve* db_curve2 = dynamic_cast<CDbCurve*>(db_entity2);
	if (!db_curve2)
	{
		ret.Internal( IDS_SOLVE_TANGENT_TYPE, crv2 );
		return ret;
	}

	// Get the curves in the world coordinate system.
	// Without this step, the candidate solutions will not
	// be drawn in the correct place for 4th quadrant machines.
	const CGeoCurve* curve1 = db_curve1->Curve(0);
	const CGeoCurve* curve2 = db_curve2->Curve(0);

	//	Pick points, which control the solution discrimination...
	//	This may be from the curve, if not explicitly defined.
	//
	pt1 = C3dCoord( px1, py1, 0.0 );
	pt2 = C3dCoord( px2, py2, 0.0 );
	CPortal::RefToWorld( &pt1 );
	CPortal::RefToWorld( &pt2 );

	// -----------------------------------------------------
	// Get all possible solution pieces.
	CInt2d int2d(SMALL, onseg);
	int hitnum = int2d.CrvCrv(*curve1, *curve2);

	for (indx = 0; indx < hitnum; ++indx)
	{
		uparam = int2d.Uparam(indx);
		intsct = int2d.Point(indx);
		GetCandidates( (*curve1), uparam, intsct, indx, &candidatesA );

		uparam = int2d.Vparam(indx);
		intsct = int2d.Point(indx);
		GetCandidates( (*curve2), uparam, intsct, indx, &candidatesB );
	}

	GetDefaultSolutions(
			candidatesA, candidatesB, int2d,
			pt1, pt2, &indx, &jndx );

	// -----------------------------------------------------
	// Get the best solution.
	resultA = "";
	resultB = "";

	if (hitnum > 0)
	{
		if (hWnd > 0)
		{
			// Resolve interactively
			//TrimExtDlg dlg(view.getCWnd());
			TrimExtDlg dlg( CWnd::FromHandlePermanent((HWND) hWnd) );

			// TODO: Perhaps draw to the temporary buffer?

			db_curve1->Hide();
			db_curve2->Hide();
			view.Refresh( true );

			// set up dialog with the various solutions
			dlg.setView(view.ActiveView());
			dlg.setSolutions( candidatesA, indx, candidatesB, jndx );

			dlg.Shift( top, left );
			if (dlg.DoModal() == IDOK)
			{
				dlg.getSolutions( &indx, &jndx );

				resultA = ResultFormat( db_curve1->Id(), candidatesA[indx] );
				resultB = ResultFormat( db_curve2->Id(), candidatesB[jndx] );
			}

			db_curve1->Seek();
			db_curve2->Seek();
			view.Refresh( true );
		}
		else
		{
			resultA = ResultFormat( db_curve1->Id(), candidatesA[indx] );
			resultB = ResultFormat( db_curve2->Id(), candidatesB[jndx] );
		}
	}

	delete curve1;
	delete curve2;

	candidatesA.DestructiveFlush();
	candidatesB.DestructiveFlush();

	io_cmd->setString( "soln1", resultA );
	io_cmd->setString( "soln2", resultB );

	return ret;
}

void
CSolveProcessApp::GetCandidates(
						const CGeoCurve&	geoCurve,
						double				uparam,
						const C2dCoord&		intsct,
						int					intnum,
						CGeoCurveArray*		candidates )
{
	C3dCoord	tmp;
	CGeoCurve*	clone;

	if (intnum < 1)  // prevent duplicate
	{
		// The untrimmed clone.
		clone = (CGeoCurve*) geoCurve.Clone( false );
		clone->StringSet( "action", "unmodified" );
		candidates->Append( clone );
	}

	// Ugh.  Otherwise, Z is UNDEFINED.
	tmp = intsct;
	tmp.Z( geoCurve.StartPt().Z() );

	if (uparam < 0.)
	{
		if ( !intsct.WithinTol( geoCurve.StartPt(), SMALL ) )
		{
			clone = (CGeoCurve*) geoCurve.Clone( false );
			clone->StartPt( tmp );
			if (clone->Length2d() > SMALL)
			{
				clone->StringSet( "action", "extend_start" );
				candidates->Append( clone );
			}
			else
			{
				delete clone;
			}
		}
	}
	else if (uparam > 1.0)
	{
		if ( !intsct.WithinTol( geoCurve.EndPt(), SMALL ) )
		{
			clone = (CGeoCurve*) geoCurve.Clone( false );
			clone->EndPt( tmp );
			if (clone->Length2d() > SMALL)
			{
				clone->StringSet( "action", "extend_end" );
				candidates->Append( clone );
			}
			else
			{
				delete clone;
			}
		}
	}
	else
	{
		if ( !intsct.WithinTol( geoCurve.StartPt(), SMALL ) )
		{
			clone = (CGeoCurve*) geoCurve.Clone( false );
			clone->StartPt( tmp );
			if (clone->Length2d() > SMALL)
			{
				clone->StringSet( "action", "trim_start" );
				candidates->Append( clone );
			}
			else
			{
				delete clone;
			}
		}

		if ( !intsct.WithinTol( geoCurve.EndPt(), SMALL ) )
		{
			clone = (CGeoCurve*) geoCurve.Clone( false );
			clone->EndPt( tmp );
			if (clone->Length2d() > SMALL)
			{
				clone->StringSet( "action", "trim_end" );
				candidates->Append( clone );
			}
			else
			{
				delete clone;
			}
		}
	}
}

CString
CSolveProcessApp::ResultFormat( ID id, CGeoCurve* geoCurve )
{
	CString		text;
	C3dCoord	ps;
	C3dCoord	pe;

	ps = geoCurve->StartPt();
	pe = geoCurve->EndPt();

	CPortal::WorldToRef( &ps );
	CPortal::WorldToRef( &pe );

	text.Format( "%d|%-12.8f|%-12.8f|%-12.8f|%-12.8f|%-12.8f|%-12.8f",
			id, ps.X(), ps.Y(), ps.Z(), pe.X(), pe.Y(), pe.Z() );

	text.Remove(' ');

	return text;
}

void
CSolveProcessApp::GetDefaultSolutions(
							const CGeoCurveArray&	candidatesA,
							const CGeoCurveArray&	candidatesB,
							const CInt2d&			int2d,
							const C3dCoord&			pickPtA,
							const C3dCoord&			pickPtB,
							int*					indxA,
							int*					indxB )
{
	int	soln;

	(*indxA) = 0;
	(*indxB) = 0;

	if (int2d.Count() == 1)
	{
		// Must be line-line intersection.

		(*indxA) = FindDefaultSolution(
			candidatesA, pickPtA, int2d.Point(0), int2d.Uparam(0) );

		(*indxB) = FindDefaultSolution(
			candidatesB, pickPtB, int2d.Point(0), int2d.Vparam(0) );

	}
	else if (int2d.Count() == 2)
	{
		soln = FindBestIntersection( int2d, pickPtA, pickPtB );

		(*indxA) = FindDefaultSolution(
			candidatesA, pickPtA, int2d.Point(soln), int2d.Uparam(soln) );

		(*indxB) = FindDefaultSolution(
			candidatesB, pickPtB, int2d.Point(soln), int2d.Vparam(soln) );
	}
}

int
CSolveProcessApp::FindDefaultSolution(
						const CGeoCurveArray&	candidates,
						const C3dCoord&			pickPt,
						const C2dCoord&			intsct,
						double					uparam )
{
	C3dCoord	closestPt;
	double		upick;
	int			indx;

	indx = -1;

	// The unmodified curve is candidates[0]

	if ( candidates[0]->StartPt().WithinTolXY( intsct, 1.e-4 ) )
	{
		indx = 0;
	}
	else if ( candidates[0]->EndPt().WithinTolXY( intsct, 1.e-4 ) )
	{
		indx = 0;
	}
	else if (uparam < 0.)
	{
		indx = FindDefaultSolution( candidates, "extend_start" );
	}
	else if (uparam > 1.0)
	{
		indx = FindDefaultSolution( candidates, "extend_end" );
	}
	else
	{
		candidates[0]->PointClosest( pickPt, &closestPt, &upick );

		if (uparam > upick)
		{
			indx = FindDefaultSolution( candidates, "trim_start" );
		}
		else
		{
			indx = FindDefaultSolution( candidates, "trim_end" );
		}
	}

	return indx;
}

int
CSolveProcessApp::FindDefaultSolution(
						const CGeoCurveArray&	candidates,
						const CString&			action )
{
	CString	temp;

	int indx, count = candidates.Count();
	for (indx = 0; indx < count; ++indx)
	{
		temp = candidates[indx]->StringGet( "action", "");

		if (temp.CompareNoCase( action ) == 0)
			break;
	}

	return ((indx < count) ? indx : -1);
}

int
CSolveProcessApp::FindBestIntersection(
						const CInt2d&	int2d,
						const C3dCoord&	pickPtA, 
						const C3dCoord& pickPtB )
{
	CGeoLine	geoLine;
	C3dCoord	closestPt;
	double		distA, distB;
	double		uparam;

	geoLine.Init( pickPtA, pickPtB );

	distA = geoLine.PointClosest( int2d.Point(0), &closestPt, &uparam );
	distB = geoLine.PointClosest( int2d.Point(1), &closestPt, &uparam );

	return ((distA < distB) ? 0 : 1);
}
