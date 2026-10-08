#pragma once

#include "Type.h"
#include "Return.h"
#include "Model.h"

#include "binary_file.h"

#define MM2_STD_MODE		0x00
#define MM2_PREVIEW_MODE	0x01
#define	MM2_SKIP_SEQ_OBJS	0x02
#define	MM2_KEEP_SEQ_ORDER	0x04
#define MM2_SKIP_WORK_ZONES	0x08


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CMM2
{
public:

	CMM2();

	CReturn Read( const CString& fileName, CModel* model, int flags );

	CReturn Merge( const CString& fileName, const C3dCoord& origin, CModel* model, CDbContainer* owner );

	CReturn Write( const CString& fileName, const CModel& model );

	virtual ~CMM2();

private:  // Methods

	// File reading operations.

	CReturn Read( CBinaryFile& file );

	CReturn VersionRead( CBinaryFile& file, CString* version );

	CReturn ReadFile0_001( CBinaryFile& file );

	CReturn ReadAttrib0_001( CBinaryFile& file, CVarList* varlist, bool replaceWhiteSpace );

	CReturn ReadEntity0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadCommon0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadWorkplane0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadTool0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadPoint0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadLine0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadArc0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadHole0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadProfile0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadFeature0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadSequence0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadPattern0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadCommand0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadToolpath0_001( CBinaryFile& file, CDbEntity* dbEntity );

	CReturn ReadTriple( CBinaryFile& file, double* x, double* y, double* z );

	// File writing operations.

	CReturn Write( CBinaryFile& file );

	CReturn Write( CBinaryFile& file, const CVarList& varlist );

	CReturn Write( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteCommon( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteLayer( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteWorkplane( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteTool( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WritePoint( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteLine( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteArc( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteHole( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteProfile( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteFeature( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteSequence( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WritePattern( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteCommand( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteToolpath( CBinaryFile& file, const CDbEntity& dbEntity );

	CReturn WriteCoord( CBinaryFile& file, const C3dCoord& pt );

	CReturn WriteEntityList( CBinaryFile& file, const CDbEntityList& dbEntities );

	CReturn WriteTriple( CBinaryFile& file, double x, double y, double z );

	// For making file translation decisions.
	void VersionFlagsSet( const CString& version );
	bool IsValidVersion()		{ return m_isValidVersion; }
	bool IsPreVersion16()		{ return m_isPreVersion16; }

	void ConditionalToolingCopy( const CModel& incoming, CModel* model );
	CDbTool* LayerFind( const CModel& model, const CString& layerName );
	CDbTool* ToolFind( const CModel& model, int toolId );

private:  // Disabled

	CMM2( const CMM2& );
	const CMM2& operator = ( const CMM2& );
	int operator == ( const CMM2& ) const;
	int operator != ( const CMM2& ) const;

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
};

