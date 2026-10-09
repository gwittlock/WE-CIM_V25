
#include "stdafx.h"
#include "float.h"
#include "cmn_resource.h"
#include "MathConst.h"

#include "Int2d.h"
#include "DbEntity.h"
#include "DbPoint.h"
#include "DbCurve.h"
#include "DbProfile.h"
#include "Model.h"
#include "Profile.h"
#include "Conversion.h"
#include "Solution.h"
#include "IntsctRec.h"

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

	CViewMgr& view = io_cmd->getViewMgr();

	// -----------------------------------------------------
	//		Extract command
	//
	ID			tan1;
	BOOL		end1;
	ID			tan2;
	BOOL		end2;

	ret += io_cmd->getInt( "tan1", (int*)&tan1 );
	ret += io_cmd->getInt( "end1", &end1 );

	ret += io_cmd->getInt( "tan2", (int*)&tan2 );
	ret += io_cmd->getInt( "end2", &end2 );

	int onseg = 0;
	io_cmd->getInt( "onseg", &onseg );

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
	C3dCoord pt1;
	if (end1)
		pt1 = curve1->EndPt();
	else
		pt1 = curve1->StartPt();

	pick_ret = io_cmd->getReal( "px1", &px );
	pick_ret += io_cmd->getReal( "py1", &py );
	if (pick_ret.isOkay())
		pt1 = C3dCoord( px, py, 0.0 );

	double u1 = 0.0;
	C3dCoord pick1;
	curve1->PointClosest(pt1, &pick1, &u1);
	//
	//
	C3dCoord pt2;
	if (end2)
		pt2 = curve2->EndPt();
	else
		pt2 = curve2->StartPt();

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
	int solnum = besthit[0];
//	if (besthit[0] != besthit[1])
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

		solnum = -1;  // in the event user selects the Cancel button
		if (dlg.DoModal() == IDOK)
		{
			solnum = besthit[0] + dlg.getSolution();
			if (solnum >= hitnum)
			{ solnum = 0; }
		}

		// Note, the curves in the dialog are deleted by the dialog
		db_curve1->Seek();
		db_curve2->Seek();
		view.Refresh();
	}

	io_cmd->setReal( "soln", solnum );

	if (solnum >= 0)
	{
		C2dCoord intpt = int2d.Point(solnum);
		C3dCoord solpt;
		curve1->PointClosest(intpt, &solpt, &u1);

		io_cmd->setReal( "ix", solpt.X() );
		io_cmd->setReal( "iy", solpt.Y() );
		io_cmd->setReal( "iz", solpt.Z() );
		io_cmd->setInt("end1", bestend[0]);
		io_cmd->setInt("end2", bestend[1]);
	}
	//
	//
	delete curve1;
	delete curve2;

	return ret;
}

