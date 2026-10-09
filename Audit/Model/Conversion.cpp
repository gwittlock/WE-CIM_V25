
#include "stdafx.h"
#include "MathConst.h"
#include "ColorConst.h"
#include "cmn_resource.h"

#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "DbEntity.h"
#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbCurve.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "Profile.h"
#include "Model.h"
#include "Conversion.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CConversion::Convert(
				const CDbWorkplane*	dbWork,
				const CDbEntity*	dbEntity,
				CGeoPoly*			poly )
{
	CReturn status;

	ID workId = ((dbWork == NULL) ? 0 : dbWork->Id());

	CGeoCurve* curve = NULL;

	const CDbCurve* dbCurve = dynamic_cast<const CDbCurve*>( dbEntity );
	const CDbProfile* dbProfile = dynamic_cast<const CDbProfile*>( dbEntity );

	if (dbCurve != NULL)
	{
		curve = dbCurve->Curve( workId );
		poly->Append( curve );
	}
	else if (dbProfile != NULL)
	{
		CDbEntityList entities;

		dbProfile->RefsTo( &entities );

		int count = entities.Count();

		for (int indx = 0; indx < count; ++indx)
		{
			dbCurve = dynamic_cast<const CDbCurve*>( entities[ indx ] );

			if (dbCurve == NULL)
				continue;

			curve = dbCurve->Curve( workId );
			poly->Append( curve );
		}
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CConversion::Convert(#1)" );
	}

	return status;
}


CReturn
CConversion::Convert(
				const CDbWorkplane*	dbWork,
				const CDbEntity*	dbEntity,
				CProfile*			profile )
{
	CReturn status;

	ID workId = ((dbWork == NULL) ? 0 : dbWork->Id());

	CGeoCurve* curve = NULL;

	const CDbCurve* dbCurve = dynamic_cast<const CDbCurve*>( dbEntity );
	const CDbProfile* dbProfile = dynamic_cast<const CDbProfile*>( dbEntity );

	if (dbCurve != NULL)
	{
		curve = dbCurve->Curve( workId );
		profile->Append( curve );
	}
	else if (dbProfile != NULL)
	{
		CDbEntityList entities;

		dbProfile->RefsTo( &entities );

		int count = entities.Count();

		for (int indx = 0; indx < count; ++indx)
		{
			dbCurve = dynamic_cast<const CDbCurve*>( entities[ indx ] );

			if (dbCurve == NULL)
				continue;

			curve = dbCurve->Curve( workId );
			profile->Append( curve );
		}
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CConversion::Convert()" );
	}

	return status;
}

CReturn
CConversion::Convert(
				const CProfile&	profile,
				double			zlevel,
				CDbTool*		dbTool,
				CDbWorkplane*	dbWork,
				CDbContainer*	dbContainer )
{
	CReturn		status;
	CDbEntity*	dbEntity;

	bool assignZ = (zlevel < (UNDEFINED - SMALL));  // ooops

	int count = profile.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = profile.GetAt(indx);

		if ( assignZ )
		{
			C3dCoord& ps = (C3dCoord&) curve->StartPt();
			C3dCoord& pe = (C3dCoord&) curve->EndPt();

			ps.Z( zlevel );
			pe.Z( zlevel );

			curve->StartPt( ps );
			curve->EndPt( pe );

			CGeoArc* arc = dynamic_cast<CGeoArc*>( curve );
			if (arc != NULL)
			{
				C3dCoord& pc = (C3dCoord&) arc->CenterPt();
				pc.Z( zlevel );
				arc->CenterPt( pc );
			}
		}

		dbEntity = dbContainer->Db()->GeoConvert( (*curve), dbTool, dbWork );
		if (dbEntity != NULL)
			status = dbContainer->Append( dbEntity, FALSE );

		if ( !status.IsOk() )
			break;
	}

	return status;
}

CReturn
CConversion::Convert(
				const CProfileList&	profList,
				double				zlevel,
				CDbTool*			dbTool,
				CDbWorkplane*		dbWork,
				CDbFeature*			dbFeature )
{
	CReturn status;

	int count = profList.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbProfile* dbProfile;
		status = dbFeature->Db()->Create( DBPROFILE, (CDbEntity**) &dbProfile );
		if ( !status.IsOk() )
			break;

		const CProfile* prof = profList[indx];

		status = Convert( (*prof), zlevel, dbTool, dbWork, dbProfile );
		if ( !status.IsOk() )
			break;

		if (dbProfile->Count() < 1)
		{
			// TODO: Make CWmChain::RawOffset() more robust such that it does not
			// retain offset entities that are trimmed back to zero-length.  NOTE:
			// CDbProfile::CurveCreate() filters out zero-length curves.  Therefore,
			// it is possible that we have an empty profile.

			dbProfile->Delete();
		}
		else
		{
			dbFeature->Append( dbProfile );
		}
	}

	return status;
}

CReturn
CConversion::Convert(
				const CGeoElemList&	elems,
				CDbTool*			dbTool,
				CDbWorkplane*		dbWork,
				CDbFeature*			dbFeature )
{
	CReturn		status;
	CDbEntity*	dbEntity;

	int count = elems.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		const CGeoElem* elem = elems[indx];

		dbEntity = dbFeature->Db()->GeoConvert( (*elem), dbTool, dbWork );
		status = dbFeature->Append( dbEntity );

		if ( !status.IsOk() )
			break;
	}

	return status;
}
