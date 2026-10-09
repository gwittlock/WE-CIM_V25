#if !defined(_NIBBLER_H)
#define _NIBBLER_H
#pragma once

// ==================================================================
//		Nibbler
//
//	This class is given geometry or profiles, and returns a list of
//	points that represent punch hits on that geometry.  Points are
//	appended to the list, so a single list can be grown via multiple
//	calls.  Geometry that does not have punch tooling assigned to
//	it will generate zero hits.
// ==================================================================

#include "Common.h"

#include "3dCoord.h"

#include "DbTool.h"
#include "DbEntity.h"
#include "DbCurve.h"
#include "DbProfile.h"

#include "GeoPoint.h"
#include "GeoCurve.h"

// ==================================================================

enum eNibbleMode
{
	NIBBLE_NONE,

	NIBBLE_FIXED,		// Space the nibbles by the specified feedrate
	NIBBLE_BALANCED,	// Adjust feedrate for even spacing along element
	NIBBLE_BRIDGE		// Fixed spacing, but cut from ends to center
};

class CModel;

// ==================================================================

class dllExport CNibbler
{
public:

	CNibbler( void );
	~CNibbler( void );

	// --- Test ---
	static bool canNibble( const CDbEntity& db_ent, bool nesting );

	// --- Operation ---

	CReturn Nibble( const CDbEntity& db_ent, bool nesting, C3dCoordList* ptlist );
	CReturn Nibble( const CGeoCurve& curve, double max_feed, eNibbleMode mode, C3dCoordList* ptlist );

public:

	static int OptimalFeedrate( const CGeoCurve& curve, double* feedrate );

	static double DefaultFeedrate( const CDbCurve& db_curve );

	static int Nibble( CModel* model );

private:

	CReturn nibble( const CDbProfile& db_prof, C3dCoordList* ptlist );
	CReturn nibble( const CDbCurve& db_curve, C3dCoordList* ptlist );

private:

	static int FeaturesReplace( CModel* model );
	static int ProfilesReplace( CModel* model );
	static void ProfileCurvesGet( const CDbProfile& dbProfile, CDbCurveArray* dbCurves );
	static void HitPointsGet( const CDbCurveArray& dbCurves, C3dCoordList* hits );
	static int EntityReplace( const C3dCoordList& hits, const CDbEntity* dbEntity );
	static bool CanReplace( const CDbCurve* dbCurve );
};


#endif
