
#include "stdafx.h"
#include "StringConst.h"
#include "GeoElemList.h"
#include "Shape.h"
#include "Profile.h"
#include "DbToolGen.h"
#include "DbFeature.h"
#include "Conversion.h"
#include "ModelUtil.h"
#include "PunchDecomposer.h"
#include "PunchMatcher.h"
#include "AutoTool.h"
#include "AutoPuncher.h"



////////////////////////////////////////////////////////////////////////

CAutoPuncher::CAutoPuncher()
{
}

CAutoPuncher::~CAutoPuncher()
{
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// "Auto punches" the given list of entities.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entities that are successfully punched are removed from the
// given entity list, leaving a list of the entities that could
// not be auto punched.
//
CReturn
CAutoPuncher::AutoPunch(
					CDbEntityList*	entities,
					CModel*			model,
					CToolSetup*		toolSetup,
					bool			buildSetup,
					double			ptol,
					double			mtol,
					EMatchingRigour	rigour )
{
	CReturn		status;
	CDbEntity*	dbEntity;
	CDbFeature*	dbFeature;
	int			indx;

	dbFeature = NULL;
	indx = 0;
	while (indx < entities->Count())
	{
		dbEntity = (*entities)[indx];

		if (dbFeature == NULL)
		{
			status = model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
			if ( !status.IsOk() )
				break;
		}
		else
		{
			// Otherwise, reuse the last feature since it is empty.
		}

		status = AutoPunch(
					dbEntity, model, toolSetup, buildSetup,
					ptol, mtol, rigour, dbFeature );

		if ( !status.IsOk() )
			break;

		if (dbFeature->Count() > 0)
		{
			// We successfully auto-punched the profile.
			entities->Remove( indx );
			dbFeature = NULL;
		}
		else
		{
			++indx;
		}
	}

	// Eliminate any empty feature as necessary.
	if (dbFeature != NULL && dbFeature->Count() < 1)
		dbFeature->Delete();

	return status;
}

// NOTE: This method can be used to both create and regenerate punched features.
CReturn
CAutoPuncher::AutoPunch(
					CDbEntity*		dbEntity,
					CModel*			model,
					CToolSetup*		toolSetup,
					bool			buildSetup,
					double			ptol,
					double			mtol,
					EMatchingRigour	rigour,
					CDbFeature*		dbFeature )
{
	CReturn			status;
	CProfile		partProf;
	CGeoElemList	results;

	status = CConversion::Convert( dbEntity->Workplane(), dbEntity, &partProf );

	if ( status.IsOk() )
	{
		CAutoTool			autoTool;
		CPunchDecomposer	decomposer;
		CPunchMatcher		matcher;
		CShape				theShape;
		double				atDepth;

		// NOTE: All punching activity must occur at thickness below the top of
		// the material, regardless of where the profile is defined.  Though
		// punching operations don't care about elevation, solid view does.
		atDepth = -( model->Header().getReal( STR_THICKNESS, 0.0 ) );
		partProf.ZSet( atDepth );

		theShape.Init( partProf.Curves(), TRUE );

		matcher.Tolerances( ptol, mtol );

		status = autoTool.ToolIt(
			toolSetup, buildSetup, rigour, theShape, matcher, decomposer, &results );
	}

	if ( status.IsOk() )
	{
		if ( buildSetup )
		{
			CDbToolGen	gen;
			gen.DbToolsCreate( toolSetup->TooledStationShapes(), model );
		}

		dbFeature->DestructiveFlush();
		status = CModelUtil::PunchedFeatureCreate( results, model, dbFeature );
	}

	if ( status.IsOk() )
	{
		CString		command;
		CDbFeature*	parent;

		dbFeature->RefsFlush();
		dbFeature->AddRef( dbEntity );

		parent = CModelUtil::FirstFeature( dbEntity );
		if (parent != NULL)
			parent->Append( dbFeature, FALSE );

		command.Format( "function = \"Toolpath:AutoPunch:\",id=%d,profid=%d,ptol=%f,mtol=%f",
							dbFeature->Id(), dbEntity->Id(), ptol, mtol );

		status = dbFeature->setMulti( command );
	}

	results.DestructiveFlush();

	return status;
}
