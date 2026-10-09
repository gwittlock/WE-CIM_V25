
#include "stdafx.h"
#include "DbAllEntities.h"
#include "CodeUtil.h"



////////////////////////////////////////////////////////////////////////

// NOTE: This is probably not the most robust method of
// finding the start point of the first entity in a feature.
C3dCoord
CCodeUtil::StartPoint( const CDbEntity* dbEntity )
{
	C3dCoord ps;

	switch (dbEntity->Type())
	{
	case DBPOINT:
		{
			const CDbPoint* dbPoint = dynamic_cast<const CDbPoint*>( dbEntity );
			if (dbPoint != NULL)
				ps = dbPoint->Coord();
		}
		break;

	case DBLINE:
	case DBARC:
		{
			const CDbCurve* dbCurve = dynamic_cast<const CDbCurve*>( dbEntity );
			if (dbCurve != NULL)
				ps = dbCurve->StartPt();
		}
		break;

	case DBHOLE:
		{
			const CDbHole* dbHole = dynamic_cast<const CDbHole*>( dbEntity );
			if (dbHole != NULL)
				ps = dbHole->StartPt();
		}
		break;

	case DBPROFILE:
		{
			const CDbProfile* dbProfile = dynamic_cast<const CDbProfile*>( dbEntity );
			if (dbProfile != NULL)
				ps = StartPoint( (*dbProfile)[0] );
		}
		break;

	case DBCOMMAND:
		{
			const CDbCommand* dbCommand = dynamic_cast<const CDbCommand*>( dbEntity );
			if (dbCommand != NULL)
				ps = dbCommand->Coord();
		}
		break;

	case DBFEATURE:
		{
			const CDbFeature* dbFeature = dynamic_cast<const CDbFeature*>( dbEntity );
			if (dbFeature != NULL)
				ps = StartPoint( (*dbFeature)[0] );
		}
	}

	return ps;
}


// NOTE: This is probably not the most robust method of
// finding the end point of the last entity in a feature.
C3dCoord
CCodeUtil::EndPoint( const CDbEntity* dbEntity )
{
	C3dCoord pe;

	switch (dbEntity->Type())
	{
	case DBPOINT:
		{
			const CDbPoint* dbPoint = dynamic_cast<const CDbPoint*>( dbEntity );
			if (dbPoint != NULL)
				pe = dbPoint->Coord();
		}
		break;

	case DBLINE:
	case DBARC:
		{
			const CDbCurve* dbCurve = dynamic_cast<const CDbCurve*>( dbEntity );
			if (dbCurve != NULL)
				pe = dbCurve->EndPt();
		}
		break;

	case DBHOLE:
		{
			const CDbHole* dbHole = dynamic_cast<const CDbHole*>( dbEntity );
			if (dbHole != NULL)
				pe = dbHole->EndPt();
		}
		break;

	case DBPROFILE:
		{
			const CDbProfile* dbProfile = dynamic_cast<const CDbProfile*>( dbEntity );
			if (dbProfile != NULL)
				pe = EndPoint( (*dbProfile)[ dbProfile->Count()-1 ] );
		}
		break;

	case DBCOMMAND:
		{
			const CDbCommand* dbCommand = dynamic_cast<const CDbCommand*>( dbEntity );
			if (dbCommand != NULL)
				pe = dbCommand->Coord();
		}
		break;

	case DBFEATURE:
		{
			const CDbFeature* dbFeature = dynamic_cast<const CDbFeature*>( dbEntity );
			if (dbFeature != NULL)
				pe = EndPoint( (*dbFeature)[ dbFeature->Count()-1 ] );
		}
	}

	return pe;
}

