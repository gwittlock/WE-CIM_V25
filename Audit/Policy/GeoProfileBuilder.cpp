
#include "stdafx.h"
#include "cmn_resource.h"
#include "MathConst.h"

#include "GeoPoly.h"

//#include "DbArc.h"
//#include "DbWorkplane.h"
//#include "DbTool.h"
//#include "DbProfile.h"
//#include "DbFeature.h"
//#include "DbIterator.h"

//#include "Profile.h"
//#include "Conversion.h"

#include "GeoProfileBuilder.h"

////////////////////////////////////////////////////////////////////////

CReturn
CGeoProfileBuilder::ProfileGrow(
					CGeoCurve* geoSeedCurve,
					double gapTol,
					CGeoCurveList* inputCurves,
					CGeoCurveList* outputCurves )
{
	CReturn status;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Grow the profile in the direction of the seed curve.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CGeoCurve* geoCurve;
	C3dCoord seedPt;
	C3dCoord startPt;
	C3dCoord endPt;
	C3dVec vec;
	bool reverse;
	double minDist, dist;
	int seedIndx, indx;

	indx = inputCurves->Find( geoSeedCurve );
	if (indx >= 0)
		inputCurves->Remove( indx );
	
	outputCurves->Append( geoSeedCurve );

	while (1)
	{
		minDist = UNDEFINED;

		seedPt = geoSeedCurve->EndPt();
		seedIndx = -1;

		for (indx = 0; indx < inputCurves->Count(); ++indx)
		{
			geoCurve = inputCurves->GetAt(indx);

			startPt = geoCurve->StartPt();
			endPt = geoCurve->EndPt();

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

		geoSeedCurve = inputCurves->Remove( seedIndx );

		if ( reverse )
			geoSeedCurve->Reverse();

		outputCurves->Append( geoSeedCurve );
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Grow the profile opposite the direction of the seed curve.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	geoSeedCurve = outputCurves->GetAt(0);
	while (1)
	{
		minDist = UNDEFINED;

		seedPt = geoSeedCurve->StartPt();
		seedIndx = -1;

		for (indx = 0; indx < inputCurves->Count(); ++indx)
		{
			geoCurve = inputCurves->GetAt(indx);

			startPt = geoCurve->StartPt();
			endPt = geoCurve->EndPt();

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

		geoSeedCurve = inputCurves->Remove( seedIndx );

		if ( reverse )
			geoSeedCurve->Reverse();

		outputCurves->Prepend( geoSeedCurve );
	}

	return status;
}


CReturn
CGeoProfileBuilder::ProfileDirectionalGrow(
						CGeoCurve*		geoSeedCurve,
						CGeoCurve*		geoFinalCurve,
						bool			opposite_direction,
						double			gapTol,
						CGeoCurveList*	inputCurves,
						CGeoCurveList*	outputCurves )
{
	CReturn status;

	CGeoCurve* geoCurve;
	C3dCoord seedPt;
	C3dCoord startPt;
	C3dCoord endPt;
	C3dVec vec;
	bool reverse;
	double minDist, dist;
	int seedIndx, indx;

	indx = inputCurves->Find( geoSeedCurve );
	if (indx >= 0)
		inputCurves->Remove( indx );
	
	if (opposite_direction)
		geoSeedCurve->Reverse();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Grow the profile in the direction of the seed curve,
	// which may have been reversed.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	outputCurves->Append( geoSeedCurve );

	while (1)
	{
		if (geoSeedCurve == geoFinalCurve)
			break;

		minDist = UNDEFINED;

		seedPt = geoSeedCurve->EndPt();
		seedIndx = -1;

		for (indx = 0; indx < inputCurves->Count(); ++indx)
		{
			geoCurve = inputCurves->GetAt(indx);

			startPt = geoCurve->StartPt();
			endPt = geoCurve->EndPt();

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

		geoSeedCurve = inputCurves->Remove( seedIndx );

		if ( reverse )
			geoSeedCurve->Reverse();

		outputCurves->Append( geoSeedCurve );
	}

	return status;
}

