#pragma once

#include <map>

#include "Type.h"
#include "Return.h"
#include "Model.h"

#include "tinyxml.h"

class MapOfIntTool : public std::map<int,CDbTool*> { };


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CXML
{
public:

	CXML();

	virtual ~CXML();

	CReturn Read( const CString& fileName, CModel* model );

	CReturn Write( const CString& fileName, const CModel& model );

protected:

private:  // Methods

	// File reading operations.

	void ElementRead( const TiXmlElement* element );

	void AttributesRead( const TiXmlElement* attributesElement, CVarList* attribs );

	void WorkplaneRead( const TiXmlElement* workplaneElement );
	void ToolRead( const TiXmlElement* toolElement );
	void LineRead( const TiXmlElement* lineElement );
	void ArcRead( const TiXmlElement* arcElement );
	void HoleRead( const TiXmlElement* pointElement );
	void ProfileRead( const TiXmlElement* profileElement );
	void FeatureRead( const TiXmlElement* featureElement );
	void PointRead( const TiXmlElement* pointElement, C3dCoord* pt );
	void VectorRead( const TiXmlElement* vectorElement, C3dVec* vec );
	void TripleRead( const TiXmlElement* tripleElement, double* vals );

	void WorkplanesCreate();
	CDbWorkplane* PlaneFindCreate( const CString& name );
	bool ReferenceEntitiesGet( const CVarList& common, CDbTool** dbTool, CDbWorkplane** dbWork );

	// File writing operations.

	void ModelWrite( const CModel& model, TiXmlDocument* doc );

	void EntityWrite( TiXmlElement* parent, const CDbEntity& dbEntity );
	void WorkplaneWrite( TiXmlElement* parent, const CDbWorkplane& dbWorkplane );
	void ToolWrite( TiXmlElement* parent, const CDbTool& dbTool );
	void LineWrite( TiXmlElement* parent, const CDbLine& dbLine );
	void ArcWrite( TiXmlElement* parent, const CDbArc& dbArc );
	void HoleWrite( TiXmlElement* parent, const CDbHole& dbHole );
	void ProfileWrite( TiXmlElement* parent, const CDbProfile& dbProfile );
	void FeatureWrite( TiXmlElement* parent, const CDbFeature& dbFeature );
	void PointWrite( TiXmlElement* parent, const char* elementName, const C3dCoord& pt );
	void VectorWrite( TiXmlElement* parent, const char* elementName, const C3dVec& pt );
	void CommonWrite( TiXmlElement* parent, const CDbEntity& dbEntity );
	void AttributesWrite( TiXmlElement* parent, const char* elementName, const CVarList& varlist );
	void TripleWrite( TiXmlElement* parent, double x, double y, double z );
	const char* ToString( double dval );

	// For making file translation decisions.
	void VersionFlagsSet( const CString& version );
	bool IsValidVersion()		{ return m_isValidVersion; }
	bool IsPreVersion16()		{ return m_isPreVersion16; }

private:  // Disabled

	CXML( const CXML& );
	const CXML& operator = ( const CXML& );
	int operator == ( const CXML& ) const;
	int operator != ( const CXML& ) const;

private:  // Methods

	CReturn FeaturesResolve();
	CReturn PatternsResolve();
	CReturn SequencesResolve();
	CReturn ToolsResolve();

	void	ToolsRepair();

	void	NestingDecorationsMigrate();
	CDbFeature* FeaturesMigrate();
	void	ClampMigrate( CDbCommand* dbCommand );
	void	DropStopMigrate( CDbContainer* dbContainer );
	void	HoldMigrate( CDbCommand* dbCommand );
	CDbFeature*	ZoneMigrate( CDbCommand* dbCommand );

private:   // Data

	CModel* m_model;

	CString m_currentVersion;
	CString m_version;

	bool	m_isValidVersion;
	bool	m_isPreVersion16;

	int m_flags;

	CArray<ID,ID> m_featureId;
	CArray<ID,ID> m_patternId;
	CArray<ID,ID> m_sequenceId;

	MapOfIntTool m_toolMap;

	CDbFeature* m_workzone;
	CDbProfile* m_dbProfile;
};

