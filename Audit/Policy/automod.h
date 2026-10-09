
#ifndef _AUTOMOD_H
#define _AUTOMOD_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _TOOLSETUP_H
#include "ToolSetup.h"
#endif

class CDbTool;
class CDbArc;
class CAutoModDb;
class CModel;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// This class provides support for making automated
// modifications (database driven) to the entities in a model.
//
// Though the class was originally designed to support automated
// DXF file processing, its use is more general in that the class
// can be applied at any time to model resident entities.

class dllExport CAutoMod
{
public:

	CAutoMod();

	///////////////////////////////////////////////////////////
	// autoModDb -- the configuration manager database object.
	// model     -- the model that you are operation on.
	//
	// NOTE: The ToolSetup of the autoModDb might be modified when
	//       auto-punching with 'build tool setup' set to true.
	//
	void Init( CAutoModDb* autoModDb, CModel* model );

	///////////////////////////////////////////////////////////
	// Loops over all records in the database (one record per layer)
	// and applies the record directives to entities on that layer.
	CReturn ModelPostProcess( bool buildToolSetup, bool raw );

	///////////////////////////////////////////////////////////
	// Applies the database record directives to entities on
	// the layer associated with the current database record.
	// NOTE: When profilableEntities is not NULL and we are
	// using a punch-capable machine, you should give
	// AutoTooledFeaturesCreate() a list of candidate
	// profiles/closed-curves for shape-recognition/auto-tooling.
	// Those profiles that can be auto-tooled will be removed
	// from the list.
	CReturn AutoTooledFeaturesCreate(
				CDbEntityList*	profilableEntities,
				bool			buildToolSetup );

	///////////////////////////////////////////////////////////
	// Drill all circles with matching tools.
	CReturn AutoDrill(
				const CDbEntityList&	candidateTools,
				double					ptol,
				double					mtol,
				CDbEntityList*			holes );

	bool IsPunchCapable();

	///////////////////////////////////////////////////////////
	// Associate a 'tooled feature' with all database entities
	// that are within the given tolerance of the name tool.
	// TODO: Currently creates only drilling features for
	// points, holes and circles.
	CReturn HoledFeaturesCreate( CDbTool* dbTool );

	///////////////////////////////////////////////////////////
	// Associate a 'tooled feature' with all database entities
	// in the given set of 'like' holes.
	// TODO: Currently creates only drilling features for
	// points, holes and circles.
	CReturn HoledFeaturesCreate(
					const CDbEntityList& likeHoles,
					CDbTool* dbTool );

	CReturn ProfiledFeaturesCreate( const CDbEntityList& entities, CDbTool* dbTool );

	CReturn UniformOffset(
		CDbWorkplane*	dbWork,
		CDbTool*		dbTool,
		CDbEntity*		dbEntity,
		int				cutSide,
		double			offsetAmount,
		double			sharpAngle );

	CReturn CommandsProcess( CDbTool* dbLayer, CDbTool* dbTool );

	///////////////////////////////////////////////////////////
	// Locates all entities on the given layer that should
	// be treated as holes.
	void PartCirclesGet( const CDbTool* dbLayer, CDbEntityList* holes );

	void PartProfilesGet( const CDbTool* dbLayer, CDbEntityList* entities );

	void ProfilesGet( CDbEntityList* profs, bool toolpath_only=FALSE );

	CDbTool* AutoToolFind(
				const CDbArc*			dbArc,
				double					ptol,
				double					mtol,
				const CDbEntityList&	candidateTools );

	CDbTool* AutoToolFind(
				const CDbEntityList&	candidateTools,
				double					ptol,
				double					mtol,
				const CDbWorkplane*		dbWork,
				double					diam,
				double					depth );

	///////////////////////////////////////////////////////////
	// Find the tools having the given tool type.
	void SimilarToolsFind( int type_id, CDbEntityList* tools );

	///////////////////////////////////////////////////////////
	// Create profiles (ie. Sequence/Chain) contiguous curves
	// on each layer specified in the database.
	CReturn ProfilesCreate();

	///////////////////////////////////////////////////////////
	// Created for Auto-Tooling with punches, converts punching
	// tools to both geometric and attribute form so shape
	// comparisons can be made with part geometry during the
	// Auto-Tooling process.
	CReturn PunchToolsParameterize();

	virtual ~CAutoMod();

protected:

private:  // Methods

	///////////////////////////////////////////////////////////
	// Applies the database record directives to entities on
	// the layer associated with the current database record.
	CReturn LayerPostProcess( int recIndx, bool buildToolSetup );

	bool IsOkayToProfile( const CDbEntity* dbEntity );

	CDbTool* CamLayer();

	CReturn WorkplaneFind(
					const CDbTool* dbTool,
					CDbWorkplane** dbWork );

	void PartHoleAdd( CDbArc* dbArc, CDbEntityList* entities );

	void LikeHolesGet(
					const CDbTool& dbTool,
					CDbEntityList* entities,
					CDbEntityList* likeHoles );

	void TaggedHolesGet(
					const CDbTool& dbTool,
					CDbEntityList* likeHoles );

	bool IsProfilingTool( const CDbTool& dbTool );

	CReturn ProfilesAdjust();
	CReturn ProfilesAdjust( const CDbEntityList& profs, const CDbTool* dbLayer, int cutSide );

	void ProfileReverse( CDbEntity* dbEntity );
	int  ProfileDirection( const CDbEntity* dbEntity );

	double	HolePlusTol();
	double	HoleMinusTol();

	bool ProcessCircles( const CDbTool& dbTool );

	void DuplicatesFilter( CModel* model, double gap_tol );

	bool IsOpenProfile( const CDbEntity* dbEntity );

private:  // Disabled

	CAutoMod( const CAutoMod& );
	const CAutoMod& operator = ( const CAutoMod& );
	int operator == ( const CAutoMod& ) const;
	int operator != ( const CAutoMod& ) const;

private:  // Data

	CAutoModDb*		m_autoModDb;
	CModel*			m_model;
	TShapeArray		m_parameterizedTools;
};

#endif

