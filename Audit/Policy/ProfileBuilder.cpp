
#include "stdafx.h"
#include "cmn_resource.h"
#include "MathConst.h"
#include "ColorConst.h"
#include "Register.h"

#include "GeoPoly.h"

#include "DbArc.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "DbIterator.h"

#include "Profile.h"
#include "Conversion.h"
#include "Worm.h"
#include "Model.h"
#include "ProfileBuilder.h"
//
// NOTE:  If we are ever so inspired, we could merge parts of ProfileBuilder
// with the overlap in GeoProfileBuilder.
//
//
int QSortIDCompare( const void* ptrA, const void* ptrB )
{
	CDbCurve* curveA = (*(CDbCurve**) ptrA);
	CDbCurve* curveB = (*(CDbCurve**) ptrB);

	ID idA = curveA->Id();
	ID idB = curveB->Id();

	return (idA - idB);
}

void Pm4ProfileGrow(
		CDbCurveList*	inputCurves,
		CDbCurveList*	outputCurves,
		double			tol );

////////////////////////////////////////////////////////////////////////

CReturn
CProfileBuilder::ProfileGrow(
					CDbCurve* dbSeedCurve,
					double gapTol,
					CDbCurveList* inputCurves,
					CDbCurveList* outputCurves )
{
	CReturn status;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Grow the profile in the direction of the seed curve.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CDbCurve* dbCurve;
	C3dCoord seedPt;
	C3dCoord startPt;
	C3dCoord endPt;
	C3dVec vec;
	bool reverse;
	double minDist, dist;
	int seedIndx, indx;
	boolean	proceed;

	indx = inputCurves->Find( dbSeedCurve );
	if (indx >= 0)
		inputCurves->Remove( indx );
	
	outputCurves->Append( dbSeedCurve );

	if (dbSeedCurve->Type() == DBARC)
	{
		// Special-casing full circles can save time loads of
		// processing time wrt. files containing many entities.
		startPt = dbSeedCurve->StartPt();
		endPt = dbSeedCurve->EndPt();
		proceed = (startPt.WithinTolXY( endPt, SMALL ) == FALSE);
	}
	else
	{
		proceed = true;
	}

	if (proceed)
	{
		while (1)
		{
			minDist = UNDEFINED;

			seedPt = dbSeedCurve->EndPt(0);
			seedIndx = -1;

			for (indx = 0; indx < inputCurves->Count(); ++indx)
			{
				dbCurve = (*inputCurves)[indx];

				startPt = dbCurve->StartPt(0);
				endPt = dbCurve->EndPt(0);

				if ( startPt.WithinTol( seedPt, gapTol ) )
				{
					vec = startPt - seedPt;
					dist = vec.Length();

					if (dist < minDist)
					{
						minDist = dist;
						seedIndx = indx;
						reverse = FALSE;
					}
				}

				if ( endPt.WithinTol( seedPt, gapTol ) )
				{
					vec = endPt - seedPt;
					dist = vec.Length();

					if (dist < minDist)
					{
						minDist = dist;
						seedIndx = indx;
						reverse = TRUE;
					}
				}
			}

			if (seedIndx < 0 || inputCurves->Count() < 1)
				break;  // Done. No curve end point within gapTolerance.

			dbSeedCurve = inputCurves->Remove( seedIndx );

			if ( reverse )
				dbSeedCurve->Reverse();

			outputCurves->Append( dbSeedCurve );
		}


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Grow the profile opposite the direction of the seed curve.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		dbSeedCurve = (*outputCurves)[0];
		while (1)
		{
			minDist = UNDEFINED;

			seedPt = dbSeedCurve->StartPt(0);
			seedIndx = -1;

			for (indx = 0; indx < inputCurves->Count(); ++indx)
			{
				dbCurve = (*inputCurves)[indx];

				startPt = dbCurve->StartPt(0);
				endPt = dbCurve->EndPt(0);

				if ( startPt.WithinTol( seedPt, gapTol ) )
				{
					vec = startPt - seedPt;
					dist = vec.Length();

					if (dist < minDist)
					{
						minDist = dist;
						seedIndx = indx;
						reverse = TRUE;
					}
				}

				if ( endPt.WithinTol( seedPt, gapTol ) )
				{
					vec = endPt - seedPt;
					dist = vec.Length();

					if (dist < minDist)
					{
						minDist = dist;
						seedIndx = indx;
						reverse = FALSE;
					}
				}
			}

			if (seedIndx < 0 || inputCurves->Count() < 1)
				break;  // Done. No curve end point within gapTolerance.

			dbSeedCurve = inputCurves->Remove( seedIndx );

			if ( reverse )
				dbSeedCurve->Reverse();

			outputCurves->Prepend( dbSeedCurve );
		}
	}

	return status;
}

CReturn
CProfileBuilder::ProfileGrow(
					CModel*			model,
					double			gapTol,
					CDbCurve*		dbSeedCurve,
					CDbCurveList*	dbCandidateCurves,
					CDbProfile**	dbProfile )
{
	CDbCurveList outputCurves;

	CReturn status = ProfileGrow( dbSeedCurve, gapTol, dbCandidateCurves, &outputCurves );

	CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( dbSeedCurve->Owner() );

	if (outputCurves.Count() > 0)
	{
		status = model->EntityCreate( DBPROFILE, (CDbEntity**) dbProfile );

		if ( status.IsOk() )
		{
			// Reordered appendages, to preserve tools down from feature to geometry
			if (dbFeature != NULL)
				dbFeature->Append( (*dbProfile) );

			(*dbProfile)->Append( outputCurves );
		}
	}
	else
	{
		status.Internal( IDS_PROFILE_FAILED );
	}

	return status;
}

CReturn
CProfileBuilder::ProfileSelectedGrow(
	CModel*			model,
	double			gapTol,
	CDbCurveList*	inputCurves,
	CDbProfile**	dbProfile )
{
	CReturn		status;
	CDbCurve*	dbCurveA;
	CDbCurve*	dbCurveB;
	
	dbCurveA = inputCurves->Remove(0);

	(*dbProfile) = dynamic_cast<CDbProfile*>( dbCurveA->Owner() );
	if ((*dbProfile) == NULL)
		status = model->EntityCreate( DBPROFILE, (CDbEntity**) dbProfile );

	if ((*dbProfile) != NULL)
	{
		while(1)
		{
			(**dbProfile).Append( dbCurveA );

			if (inputCurves->Count() < 1)
				break;  // We've exhausted the list.

			dbCurveB = inputCurves->GetAt(0);
			if ( !CurvesAdjust( gapTol, dbCurveA, dbCurveB ) )
				break;  // dbCurveB is not in proximity of dbCurveA

			// This dbCurveA was dbCurveB (from the perspective of CurvesAdjust())
			// except that its direction may have changed (transparent here).
			dbCurveA = inputCurves->Remove(0);
		}
	}

	return status;
}

// Chain all contiguous curves on the given layer.
// NOTE: At first implementation, all closed profiles
// and curves will be given a CCW orientation because
// they are assumed to represent holes in a panel.
// In later steps, this allows all associated toolpath
// to simply be offset to the left (or inside) of the
// profile.
//
// NOTE2:  No longer forced CCW, is specified in parameters
CReturn
CProfileBuilder::ProfileLayer(
						CModel*		model,
						CDbTool*	dbLayer,
						double		gapTol,
						double		cleanTol,
						bool		associate,
						int			dir,
						bool		sortByID)
{
	CReturn status;

	CDbEntityList	entities;
	CDbCurveList	inputCurves;
	CDbCurveList	outputCurves;
	CDbFeature* dbFeature = NULL;
	CDbProfile* dbProfile = NULL;
	CDbEntity* dbEntity = NULL;
	CDbCurve* dbCurve = NULL;
	CDbCurve* seedCurve = NULL;
	int	count, indx, color;
	bool	filter_duplicates;

	dbLayer->RefdBy( &entities );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Find all of the curves on this layer.
	// NOTE: It is possible for a layer setup to map
	// multiple CAD layers to the same CAM layer.  As
	// such, we must consider only those curves that
	// do not yet belong to a profile.
	//
	// NOTE: The curves are sorted by increasing order
	// of ID because the IDs represent the historical
	// order of creation.  This is necessary to get
	// converted PM4 files to maintain correct entity
	// ordering during chaining.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = entities.Count();

	if ( sortByID )		// for SmartCAM PM4 files
	{
		CDynamicArray<CDbCurve*> tmp;  // BenignFlush()'d during destruction

		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = entities[ indx ];
			dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

			if (dbCurve != NULL && dynamic_cast<CDbProfile*>( dbCurve->Owner() ) == NULL)
				tmp.Append( dbCurve );
		}

		tmp.Qsort( &QSortIDCompare );

		count = tmp.Count();
		for (indx = 0; indx < count; ++indx)
		{
			inputCurves.Append( tmp[indx] );
		}
	}
	else
	{
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = entities[ indx ];
			dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

			if (dbCurve != NULL && dynamic_cast<CDbProfile*>( dbCurve->Owner() ) == NULL)
				inputCurves.Append( dbCurve );
		}
	}

	// 2006.11.06 (PE) -- First attempt at filtering duplicate entities.
	// This was introduced (finally) because Dynatorch customers are
	// manufacturing 'yard art' which (by nature of being hand-drawn or
	// scanned) has a propensity for having duplicate entities.
	filter_duplicates = CRegister::BoolGetV( "CadToCode", "dups", true );

	if (filter_duplicates)
		 DuplicatesFilter( model, gapTol, &inputCurves );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Build profiles through the curves on this layer.
	while (1)
	{
		if (inputCurves.Count() < 1)
			break;  // proper termination

		if ( sortByID )		// for SmartCAM PM4 files
		{
			Pm4ProfileGrow( &inputCurves, &outputCurves, cleanTol );
			seedCurve = outputCurves[0];
		}
		else
		{
			seedCurve = inputCurves.Remove( 0 );

			status = CProfileBuilder::ProfileGrow(
				seedCurve, gapTol, &inputCurves, &outputCurves );
		}

		if (!status.IsOk() || outputCurves.Count() < 1)
			break;

		if (outputCurves.Count() == 1)
		{
			//
			// Single-curve profile...
			//
			CDbArc* dbArc = dynamic_cast<CDbArc*>( outputCurves[0] );

			if (dbArc != NULL)
			{
				// Give the arc a CCW orientation, as necessary.

				C3dCoord ps = dbArc->StartPt();
				C3dCoord pe = dbArc->EndPt();

				if (dir != 0)
				{
// Use this line, if we want to compensate for left-handed planes
					CDbWorkplane* arcPlane = dbArc->Workplane();

					int flipDir = dbArc->Dir() * arcPlane->ToolUp();
//					int flipDir = dbArc->Dir();

					if (ps.WithinTol( ps, SMALL ) && (flipDir != dir) )
						dbArc->Reverse();
				}
			}
		}
		else if (outputCurves.Count() > 1)
		{
			//
			// Multiple curves...
			// Need only generate profiles for two or more contiguous curves.
			//

			CDbProfile* dbProfile;
			status = model->EntityCreate( DBPROFILE, (CDbEntity**)&dbProfile );

			if ( !status.IsOk() )
				break;

			// 2006.09.19 (PE) -- The color of profiles that were generated by
			// importing a cad file were appearing in white in the entity list.
			dbCurve = outputCurves[0];
			color = dbCurve->ColorGet( DCOLOR_WHITE );
			// dbProfile->ColorSet( color );

			dbFeature = dynamic_cast<CDbFeature*>( outputCurves[0]->Owner() );

			dbProfile->Append( outputCurves );

			if (dbFeature != NULL)
				dbFeature->Append( dbProfile );

			if (cleanTol >= SMALL)
			{
				// Closed the gaps in the profile, as necessary.

				status = dbProfile->Associate( cleanTol );

				if ( !associate )
					dbProfile->Disassociate();
// This actually works fairly well on open profiles -- the winding at least 
// seems to work.  Gotta have it for Toe Kicks
//				if ( dbProfile->IsClosed() )
				{
					// Give the profile a CCW orientation, as necessary.

					if (dir != 0)
					{
						CProfile prof;
						CDbWorkplane*	profWork = dbProfile->Workplane();
						status = CConversion::Convert( profWork, dbProfile, &prof );

// Use this line, if we want to compensate for left-handed planes
						int flipArea = SGN(prof.Area()) * profWork->ToolUp();
//						int flipArea = SGN(prof.Area());

						if (status.IsOk() && (flipArea != dir) )
							dbProfile->Reverse();
					}
				}
			}

			if ( !status.IsOk() )
				break;
		}

		outputCurves.BenignFlush();
	}

	return status;
}

CReturn
CProfileBuilder::ProfileAll(
						CModel*		model,
						double		gapTol,
						double		cleanTol,
						bool		associate,
						bool		sortByID)
{
	CReturn status;
	CDbIterator iter;

	iter.Init( model->Db(), DBTOOL );
	while (1)
	{
		CDbEntity* dbEntity = iter();

		CDbTool* dbLayer = dynamic_cast<CDbTool*>( dbEntity );

		if (dbLayer == NULL)
			break;

		// Note, that we can specify CW=1 or CCW=-1 as desired.  Left as CCW.
		// 14 Dec 99 eww
		status = CProfileBuilder::ProfileLayer(
					model, dbLayer, gapTol, cleanTol, associate, -1, sortByID );

		if ( !status.IsOk() )
			break;

		iter.Next();
	}

	return status;
}

// PM4 data is sequentially ordered and C0 continuous within a profile.
void Pm4ProfileGrow(
		CDbCurveList*	inputCurves,
		CDbCurveList*	outputCurves,
		double			tol )
{
	CDbCurve*	dbCurveA;
	CDbCurve*	dbCurveB;
	C3dCoord	ps, pe;

	dbCurveA = inputCurves->Remove(0);
	outputCurves->Append( dbCurveA );

	while (inputCurves->Count() > 0)
	{
		dbCurveB = (*inputCurves)[0];

		pe = dbCurveA->EndPt(0);
		ps = dbCurveB->StartPt(0);

		if ( !pe.WithinTol( ps, tol ) )
			break;

		dbCurveB = inputCurves->Remove(0);
		outputCurves->Append( dbCurveB );

		dbCurveA = dbCurveB;
	}
}

int
CProfileBuilder::DuplicatesFilter(
	CModel*			model,
	double			gap_tol,
	CDbCurveList*	dbCurves )
{
	CDbCurveArray	tmp;
	C2dBox			boxA;
	C2dBox			boxB;
	C3dCoord		psA;
	C3dCoord		peA;
	C3dCoord		psB;
	C3dCoord		peB;
	C3dCoord		pm;
	C3dCoord		closestPt;
	CDbTool*		duplicates;
	CDbCurve*		dbCurveA;
	CDbCurve*		dbCurveB;
	CDbCurve*		the_curve;
	CGeoCurve*		geoCurveA;
	CGeoCurve*		geoCurveB;
	double			dist, uparam;
	double			lenA, lenB;
	int				count, indxA, indxB;
	int				dup_count;
	bool			is_duplicate;

	dup_count = 0;

	model->EntityCreate( DBTOOL, (CDbEntity**) &duplicates );
	if (duplicates != NULL)
	{
		duplicates->Name("duplicates");
		duplicates->ColorSet( DCOLOR_YELLOW );
	}

	// Move the curves to an array for more efficient processing
	// (ie. CIndexList traversal is significantly slower).
	count = dbCurves->Count();
	for (indxA = 0; indxA < count; ++indxA)
	{
		tmp.Append( dbCurves->GetAt( indxA ) );
	}

	for (indxA = 0; indxA < count; ++indxA)
	{
		dbCurveA = tmp.GetAt( indxA );

		geoCurveA = dbCurveA->Curve();
		boxA = geoCurveA->Box();

		psA = geoCurveA->StartPt();
		peA = geoCurveA->EndPt();

		for (indxB = (indxA+1); indxB < count; ++indxB )
		{
			dbCurveB = tmp.GetAt( indxB );

			if (dbCurveB->Tool() == duplicates)
				continue;  // curve has already been eliminated

			geoCurveB = dbCurveB->Curve();
			boxB = geoCurveB->Box();

			psB = geoCurveB->StartPt();
			peB = geoCurveB->EndPt();

			if (geoCurveB->Type() == geoCurveA->Type())
			{
				is_duplicate = false;

				// 2006.12.22 (PE) -- Connweld experienced a problem where
				// some entities where improperly flagged as duplicates.
				// This happened because the gap_tol exceeded the length
				// of the wrongly-flagged entities. We could change the
				// duplicate-flagging code but that could be touchy.

				// NOTE: The following test may be totally inadequate.
				if ( boxA.Intersects( boxB, SMALL ) )
				{
					if ( psA.WithinTolXY( psB, gap_tol ) )
					{
						// The curves share a common end point. Now we
						// test whether the curves overlap by checking
						// the proximity of the other end point.
						dist = geoCurveB->PointClosest( peA, &closestPt, &uparam );
						is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						if ( !is_duplicate )
						{
							dist = geoCurveA->PointClosest( peB, &closestPt, &uparam );
							is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						}

					}
					else if ( psA.WithinTolXY( peB, gap_tol ) )
					{
						// The curves share a common end point. Now we
						// test whether the curves overlap by checking
						// the proximity of the other end point.
						dist = geoCurveB->PointClosest( peA, &closestPt, &uparam );
						is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						if ( !is_duplicate )
						{
							dist = geoCurveA->PointClosest( psB, &closestPt, &uparam );
							is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						}

					}
					else if ( peA.WithinTolXY( psB, gap_tol ) )
					{
						// The curves share a common end point. Now we
						// test whether the curves overlap by checking
						// the proximity of the other end point.
						dist = geoCurveB->PointClosest( psA, &closestPt, &uparam );
						is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						if ( !is_duplicate )
						{
							dist = geoCurveA->PointClosest( peB, &closestPt, &uparam );
							is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						}

					}
					else if ( peA.WithinTolXY( peB, gap_tol ) )
					{
						// The curves share a common end point. Now we
						// test whether the curves overlap by checking
						// the proximity of the other end point.
						dist = geoCurveB->PointClosest( psA, &closestPt, &uparam );
						is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						if ( !is_duplicate )
						{
							dist = geoCurveA->PointClosest( psB, &closestPt, &uparam );
							is_duplicate = IsDuplicate( gap_tol, dist, uparam );
						}

					}
					else
					{
						// ?????
					}

					if (is_duplicate && (geoCurveA->Type() == GEOARC))
					{
						pm = geoCurveA->MidPt();
						dist = geoCurveB->PointClosest( pm, &closestPt, &uparam );
						is_duplicate = IsDuplicate( SMALL, dist, uparam );
					}

					if (is_duplicate)
					{
						lenA = geoCurveA->Length2d();
						lenB = geoCurveB->Length2d();

						the_curve = ((lenA > lenB) ? dbCurveB : dbCurveA);

						the_curve->Tool( duplicates );
						// the_curve->ColorSet( DCOLOR_YELLOW );
					}
				}
			}

			delete geoCurveB;
		}

		delete geoCurveA;
	}

	indxA = 0;
	while (indxA < dbCurves->Count())
	{
		dbCurveA = dbCurves->GetAt( indxA );
		if (dbCurveA->Tool() == duplicates)
		{
			dbCurves->Remove( indxA );
			++dup_count;
		}
		else
		{
			++indxA;
		}
	}

	if (dup_count < 1)
	{
		// Remove the evidence :-)
		model->EntityDelete( duplicates->Id() );
	}

	return dup_count;
}

bool
CProfileBuilder::IsDuplicate( double gap_tol, double dist, double uparam )
{
	return ((dist <= gap_tol) && (uparam >= -SMALL) && (uparam <= (1.+SMALL)));
}

// CRITICAL ASSUMPTION: dbCurveA has the proper direction.
// NOTE: Originally designed to adjust both curves but only adjusts the trailing curve.
bool
CProfileBuilder::CurvesAdjust(
	double		gap_tol,
	CDbCurve*	dbCurveA,
	CDbCurve*	dbCurveB )
{
	int min_indx = -1;

	if (dbCurveB != NULL)
	{
		C3dCoord	ptA;
		C3dCoord	ptB;
		C3dVec		vec;
		double		min_dist, dist;
		int			indx;

		min_dist = UNDEFINED;
		ptA = dbCurveA->EndPt(0);

		for (indx = 0; indx < 2; ++indx)
		{
			ptB = ((indx & 0x1) ? dbCurveB->EndPt(0) : dbCurveB->StartPt(0));

			if ( ptA.WithinTol( ptB, gap_tol ) )
			{
				vec = ptB - ptA;
				dist = vec.Length();

				if (dist < min_dist)
				{
					min_dist = dist;
					min_indx = indx;
				}
			}
		}

		if (min_indx == 1)
			dbCurveB->Reverse();
	}

	return (min_indx >= 0);
}
