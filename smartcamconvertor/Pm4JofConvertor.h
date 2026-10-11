
#ifndef _PM4JOFCONVERTOR_H
#define _PM4JOFCONVERTOR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _DBWORKPLANE_H
#include "DbWorkplane.h"
#endif

#ifndef _DBCURVELIST_H
#include "DbCurveList.h"
#endif

#ifndef _DBLINE_H
#include "DbLine.h"
#endif

#ifndef _MODEL_H
#include "Model.h"
#endif

class CJof;
class CDbEntity;
class CDbTool;
class CDbCurve;
class CDbProfile;
class CAutoModDb;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CPm4JofConvertor
{
public:

	CPm4JofConvertor();

	CReturn MM2Create(
						const CString&	cmdbPath,
						const CString&	pm4Path,
						const CString&	jofPath,
						const CString&	mm2Path,
						const CString&	machineName,
						const CString&	toolSetupName,
						const CString&	materialName,
						bool			partOutlineCreate );

	CReturn MDBCreate(
						const CString& jofPath,
						const CString& mdbPath );

	void Debug( bool debug );

	CReturn Dump( const CString& jofPath );

	virtual ~CPm4JofConvertor();

protected:

private:  // Methods

	CReturn Initialize( CModel* model );

	CReturn ProfilesCreate( CModel* model );

	CReturn ToolsCreate( const CJof& jof, CModel* model );
	CReturn ToolsLink( CModel* model );

	CReturn HolesPunch( CModel* model );
	CReturn ProfilesCut( CModel* model );

	CReturn ProfileCut( CModel* model, CDbEntity* dbEntity, CDbTool* dbTool );
	CReturn FakePunchOffset( CModel* model, CDbEntity* refEntity, CDbTool* dbTool );

	CDbCurve* OffsetCurveCreate( CModel* model, CDbCurve* dbCurve, CDbTool* dbTool );

	CDbFeature* VerbatimProfileCut( CModel* model, CDbEntity* refEntity, CDbTool* dbTool );

	CReturn MoveToLayer( CDbEntity*	dbEntity, CDbTool* dbLayer );

	CReturn PartOutlineCreate( CModel* model );
	CDbProfile* PartOutlineCreate( CModel* model, CDbTool* refLayer );
	bool ProfileMend( CModel* model, CDbProfile* dbProfile );
	CDbLine* LineCreate(
						CModel*			model,
						CDbTool*		dbLayer,
						CDbWorkplane*	dbWork,
						const C3dCoord&	ps,
						const C3dCoord&	pe );

	CReturn PartBoundingBoxCreate( CModel* model );

	void SeedCurveFind(
						const CDbCurveList&	dbCurves,
						CDbCurve**			seedCurve,
						double*				gaptol );

	void Msg( const char* what, const CReturn& status );

private:  // Disabled

	CPm4JofConvertor( const CPm4JofConvertor& );
	const CPm4JofConvertor& operator = ( const CPm4JofConvertor& );
	int operator == ( const CPm4JofConvertor& ) const;
	int operator != ( const CPm4JofConvertor& ) const;

private:  // Data

	bool			m_debug;
	
	CDbTool*		m_refLayer;
	CDbWorkplane*	m_topWork;
};

#endif

