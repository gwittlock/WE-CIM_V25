
#ifndef _MODELUTIL_H
#define _MODELUTIL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "DbCurve.h"
#include "DbCurveList.h"
#include "DbHole.h"
#include "DbProfile.h"
#include "DbPattern.h"
#include "Model.h"
#include "QuickHull.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This helper class contains methods that enforce application constraints
// upon the engine (the coupling between tools and layers, for instance).
//   eg. entities associate with a tool having an NC_Code_Number attribute
//       of 101 must exist on a layer whose name is '_tool101'.
//

class dllExport CModelUtil
{
public:

	static CReturn MarkIntExt(
						CDbEntityList* proflist,
						bool robustCheck,
						CModel* model );

	// Moved over (and combined) from CreateProcess and NestConfig
	static CReturn Transform( 
						CModel*				model,
						const C3x4Matrix&	in_xform, 
						int					in_copies,
						BYTE				control );

	static CReturn TransformGrid( 
						CModel*		model,
						double		dx,
						double		dy,
						int			xcnt,
						int			ycnt );

	// Find a tool by its NC_Code_Number attribute.
	static CDbTool* DbToolFind( const CEntityDb& db, const CString& field, int value );

	// Find a tool given a tool ID (filled or empty)
	static CDbTool* FindToolByID( CEntityDb* db, int toolID, bool filled );

	// Introduced for Nesting, to remove all empty stations after
	// creating tool setups on-the-fly.
	static void EmptyStationsDelete( CEntityDb* db );

	// Associates a container with a new tool.  Per application behavior,
	// the top-level container and its immediate children will be associated
	// with the given tool, and will likewise become associated with the layer
	// whose name is derived from the tool.  NOTE: Nested features and their
	// children will not be affected.
	static CReturn ToolAssociate( CDbFeature* dbFeature, CDbTool* dbTool );
	static CReturn ToolAssociate( CDbProfile* dbProfile, CDbTool* dbTool );

	// Finds the "first parent feature" of an entity.
	// Returns NULL if no such parent.
	static CDbFeature* FirstFeature( const CDbEntity* dbEntity );

	//===========================================================================
	// Moved here from CCreateProcess.  TODO: Move to some reasonable place :-(
	//===========================================================================

	static CReturn FeatureFind(
							const CEntityDb& db,
							ID id,
							CDbFeature** dbFeature );

	static CReturn FeatureFindCreate(
							CEntityDb* db,
							ID id,
							CDbFeature** dbFeature );

	static CDbFeature* SubFeatureFindCreateByTool(
							CEntityDb*		db,
							CDbFeature*		superfeature,
							ID				toolId,
							CDbWorkplane*	dbWork );

	static CReturn PatternFind(
							const CEntityDb& db,
							ID id,
							CDbPattern** dbPattern );

	static CReturn PatternCreate(
							CEntityDb* db,
							CDbPattern** dbPattern );

	static CReturn PatternFindCreate(
							CEntityDb* db,
							ID id,
							CDbPattern** dbPattern );

	// Replaces the given instance (dbCommand) with a feature containing
	// copies of the entities of the pattern that is referenced by the
	// instance.  The instance is deleted if the method is successful.
	static CReturn	PatternExplode(
							CDbCommand**	dbCommand,
							CDbFeature**	dbFeature );
	static CReturn	PatternExplode( CDbPattern& pattern );

	static CReturn PunchedFeatureCreate(
						const CGeoElemList&	elems,
						CModel*				model,
						CDbFeature*			topLevelFeature );


	static CReturn Explode( CDbContainer* db_container, bool recurse );

	static CReturn EmptyContainers( CModel& model, bool workzones );

	static CReturn ChainCut(
						CModel& model,
						CDbLine* dbLineA,
						CDbLine* dbLineB,
						double len,
						bool horz );

	static CDbEntity* FindLeadOut(CDbProfile* prof);
	static CDbEntity* FindLeadIn(CDbProfile* prof);

	static bool IsLeadEntity( const CDbEntity* dbEntity );

	static void ConditionalDisassociate( CDbFeature* dbFeature );

	// Bounding box.
	static CDbProfile* BBPartOutlineCreate(
		const CDbEntityArray&	entities,
		CModel*					model );

	// Convex hull (aka. rubberband).
	static CDbProfile* CHPartOutlineCreate(
		const CDbEntityArray&	entities,
		double					chordal_tol,
		CModel*					model );

	// Restricted for line thru command.
	static int WorkZoneNum( const CDbEntity* dbEntity );

	static void StatisticsGet( const CModel& model, double* cutDistance, int* numPierces );

protected:

private:

	static void EntityReverse( CDbEntity* dbEntity );
	static int SortFunc( const void* ptrA, const void* ptrB );

	static void ConditionalAppend( const C3dCoord& ptA, C3dCoordArray* pts );

	static void ArcTabulate(
		const CGeoArc&	geoArc,
		double			chordal_tol,
		Tchvector*		pts );

	static void HoleTabulate(
		const CDbHole&	dbHole,
		double			chordal_tol,
		Tchvector*		pts );

private:  // Disabled.

	CModelUtil();
	CModelUtil( const CModelUtil& );
	virtual ~CModelUtil();
	const CModelUtil& operator = ( const CModelUtil& );
	int operator == ( const CModelUtil& ) const;
	int operator != ( const CModelUtil& ) const;

private:

};

#endif

