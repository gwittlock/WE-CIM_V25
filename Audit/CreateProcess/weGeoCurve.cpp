
#include "stdafx.h"

#include "cmn_resource.h"
#include "GeoCurve.h"
#include "DbCurve.h"
#include "Model.h"

#include "CreateProcess.h"


// ==================================================================
// Create:Split: id=%d, pos=%d, [dist=%f], [, px=%f, py=%f, pz=%f] [,gap=%f]
//
//
enum eSplitPos
{
	SPLIT_MID		= 0,
	SPLIT_START		= 1,
	SPLIT_END		= 2,
	SPLIT_PICK		= 3
};

CReturn CCreateProcessApp::Split( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt("id", 0);
	eSplitPos pos = (eSplitPos)io_cmd->VarList().getInt("pos", 0);
	double dist = io_cmd->VarList().getReal("dist", 0.5);
	double gap = io_cmd->VarList().getReal("gap", 0.);

	CDbCurve* dbCurveA;
	model.EntityFind( id, (CDbEntity**) &dbCurveA, DBLINE, DBARC );

	if (dbCurveA == nullptr)
	{
		status.Internal(IDS_INTERNAL_ERROR, "CCreateProcessApp::Split(#1)");
	}
	else
	{
		C3dCoord split;

		CGeoCurve* geoCurveA = dbCurveA->Curve();

		if (pos == SPLIT_PICK)
		{
			double px = io_cmd->VarList().getReal("px", 0.);
			double py = io_cmd->VarList().getReal("py", 0.);
			double pz = io_cmd->VarList().getReal("pz", 0.);

			double u = 0.0;
			geoCurveA->PointClosest( C3dCoord(px, py, pz), &split, &u);
			
			double stol = SMALL*3.0; // Minimum length to avoid degenerate split curve
			if (!BETWEEN(stol, u, 1.0-stol))
				status.setStatus(STATUS_ERROR);
		}
		else
		{
			if (pos==SPLIT_MID)
				dist *= geoCurveA->Length2d();

			split = geoCurveA->PointAtDist( dist, (pos != SPLIT_END) );
		}

		delete geoCurveA;

		CDbCurve* dbCurveB = nullptr;
		if ( status.IsOk() )
		{
			CDbCurve* dbCurveB = dbCurveA->Split( split, gap );

			io_cmd->setReal("px", split.X());
			io_cmd->setReal("py", split.Y());
		}

		// NOTE: 'dbCurveB' can be null if either 1) status is bad or 2) Split() failed.
		io_cmd->setInt( "id", ((dbCurveB == nullptr) ? 0 : dbCurveB->Id()) );
	}

	return status;
}
