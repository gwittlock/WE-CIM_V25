// SolveProcess2.cpp 
//

#include "stdafx.h"
#include "cmn_resource.h"
#include "MathConst.h"

#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbCurve.h"
#include "DbCurveList.h"
#include "DbProfile.h"
#include "Model.h"
#include "Profile.h"
#include "Conversion.h"
#include "Solution.h"

#include "DbIterator.h"
#include "ModelUtil.h"

#include "SolveProcess.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:ChainCut: idA = %d, idB = %d, len= %f, horz=%b
//
CReturn 
CSolveProcessApp::ChainCut( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	ID idA;
	ID idB;
	double len;
	BOOL horz;

	CModel& model = io_cmd->getModel();

	status += io_cmd->getInt( "idA", (int*) &idA );
	status += io_cmd->getInt( "idB", (int*) &idB );
	status += io_cmd->getReal( "len", &len);
	status += io_cmd->getInt( "horz", &horz );

	CDbLine* dbLineA = NULL;
	CDbLine* dbLineB = NULL;

	if ( status.IsOk() )
	{
		CModel& model = io_cmd->getModel();

		CDbEntity* dbEntityA;
		CDbEntity* dbEntityB;

		model.EntityFind( idA, (CDbEntity**) &dbEntityA, DBLINE, DBLINE );
		model.EntityFind( idB, (CDbEntity**) &dbEntityB, DBLINE, DBLINE );

		dbLineA = dynamic_cast<CDbLine*>( dbEntityA );
		dbLineB = dynamic_cast<CDbLine*>( dbEntityB );
	}

	if ( !dbLineA || !dbLineB)
	{ return CReturn(STATUS_ERROR); }

	CModelUtil::ChainCut(model, dbLineA, dbLineB, len, horz);

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:ArcAngles: xs=%f, ys=%f, xe=%f, ye=%f, xc=%f, yc=%f, dir=%d
// Returns as=%f, ae=%f
//
CReturn 
CSolveProcessApp::ArcAngles( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CGeoArc	arc;
	double	xs, ys;
	double	xe, ye;
	double	xc, yc;
	double	as, ae;
	int		dir;

	const CVarList& args = io_cmd->VarList();

	xs = args.getReal( "xs", UNDEFINED );
	ys = args.getReal( "ys", UNDEFINED );
	xe = args.getReal( "xe", UNDEFINED );
	ye = args.getReal( "ye", UNDEFINED );
	xc = args.getReal( "xc", UNDEFINED );
	yc = args.getReal( "yc", UNDEFINED );
	dir = args.getInt( "dir", 0 );

	// USER BEWARE: Garbage in / Garbage out.
	arc.Init(
		C3dCoord( xs, ys, 0. ),
		C3dCoord( xe, ye, 0. ),
		C3dCoord( xc, yc, 0. ), dir );

	arc.Angles( &as, &ae );

	io_cmd->setReal( "as", as );
	io_cmd->setReal( "ae", ae );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:Dir: id = %d
//   Where id represents either an arc or a profile
//   Returns dir = %d
CReturn 
CSolveProcessApp::Dir( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		int dir = 0;

		CDbEntity* dbEntity;
		model.EntityFind( id, &dbEntity, DBARC, DBPROFILE );

		CDbArc* dbArc = dynamic_cast<CDbArc*>( dbEntity );
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

		if (dbArc != NULL)
		{
			dir = dbArc->Dir();
			io_cmd->setInt( "dir", dir );
		}
		else if (dbProfile != NULL)
		{
			CProfile prof;

			CDbWorkplane* dbWork = dbProfile->Workplane();

			status = CConversion::Convert( dbWork, dbProfile, &prof );

			if ( status.IsOk() )
			{
				double area = prof.Area();
				io_cmd->setInt( "dir", ((area < 0) ? -1 : 1) );
			}
		}
		else
		{
			status.setStatus( STATUS_ERROR );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::Dir()" );
		return status;
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Solve:Area: id = %d
//   Where id represents either a profile
//   Returns area = %f
CReturn 
CSolveProcessApp::Area( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = 0;
	status = io_cmd->getInt( "id", (int*) &id );

	if ( status.IsOk() )
	{
		int dir = 0;

		CDbProfile* dbProfile;
		model.EntityFind( id, (CDbEntity**) &dbProfile, DBPROFILE, DBPROFILE );

		if (dbProfile != NULL)
		{
			CProfile prof;

			CDbWorkplane* dbWork = dbProfile->Workplane();

			status = CConversion::Convert( dbWork, dbProfile, &prof );

			if ( status.IsOk() )
				io_cmd->setReal( "area", prof.Area() );
		}
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::Area()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Reduce the number of entities in a profile via colinear point
// reduction.  NOTE: This Portal command replaces entire contents
// of any affected profile.
//
// To reduce a given profile:
//   Solve:Reduce: id=%d [,tol=%f]
//   where tol defaults to 1.e-3
//
// To reduce all profiles:
//   Solve:Reduce: all=%b [,tol=%f]
//   where tol defaults to 1.e-3
//
CReturn 
CSolveProcessApp::Reduce( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbIterator	iter;
	CDbProfile*	dbProfile;
	ID			id;

	CModel& model = io_cmd->getModel();

	bool all = (io_cmd->VarList().getInt( "all", FALSE ) != FALSE);
	double tol = io_cmd->VarList().getReal( "tol", 1.e-3 );

	if (all == FALSE)
	{
		id = io_cmd->VarList().getInt( "id", 0 );
		model.EntityFind( id, (CDbEntity**) &dbProfile, DBPROFILE, DBPROFILE );
		if (dbProfile == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CSolveProcessApp::Reduce(#1)" );
		}
		else
		{
			status = ProfileReduce( dbProfile, tol );
		}
	}
	else
	{
		iter.Init( model.Db(), DBPROFILE );
		while (1)
		{
			dbProfile = dynamic_cast<CDbProfile*>( iter() );
			if (dbProfile == NULL)
				break;

			status += ProfileReduce( dbProfile, tol );

			iter.Next();
		}
	}

	return status;
}

CReturn
CSolveProcessApp::ProfileReduce( CDbProfile* dbProfile, double tol )
{
	CReturn			status;
	CProfile		prof;
	CDbWorkplane*	dbWork;
	CDbTool*		dbTool;
	CDbCurve*		dbCurve;
	double			level;

	if (dbProfile->Count() > 0)
	{
		// Be careful not to process the stock boundary.
		dbTool = dbProfile->Tool();
		if (dbTool->Name().CompareNoCase("STOCK") != 0)
		{
			dbWork = dbProfile->Workplane();

			dbCurve = dynamic_cast<CDbCurve*>( (*dbProfile)[0] );
			level = dbCurve->StartPt().Z();

			status = CConversion::Convert( dbWork, dbProfile, &prof );
			if ( status.IsOk() )
			{
				prof.Reduce( tol );

				dbProfile->DestructiveFlush();
				CConversion::Convert( prof, level, dbTool, dbWork, dbProfile );
			}
		}
	}

	return status;
}