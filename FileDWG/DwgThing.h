
#ifndef _DWGTHING_H
#define _DWGTHING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _MODEL_H
#include "Model.h"
#endif

#ifndef _AUTOMODDB_H
#include "AutoModDb.h"
#endif

#ifndef _IMPORTUTIL_H
#include "ImportUtil.h"
#endif;

#include "dbmain.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CDwgThing : public CDocument
{
public:

	CDwgThing();

	CReturn Read(
				const CString&	dwgFilePath,
				const CString&	cmdbPath,
				int				toolSetupId,
				int				layerSetupId,
				int				materialId,
				BOOL			raw,
				BOOL			buildToolSetup,
				double			rotation,
				BOOL			stripLayers );

	CReturn Analyze( const CString& dwgFilePath, const CString& anlFilePath );

	CReturn ModelWrite( const CString& mm2FilePath );

	void MsgsActivate( BOOL active )	{ m_msgs = active; }

	virtual ~CDwgThing();

private:  // Methods

	CReturn AnalysisWrite( const CString& anlFilePath );

	CReturn Convert(
				const CString&	dbPath,
				int				toolSetupId,
				int				layerSetupId,
				int				materialId,
				BOOL			raw,
				double			rotation,
				BOOL			stripLayers );

	CReturn dumpDatabase();

	CReturn dumpLayerTable( AcDbLayerTable* pLayerTable );

	CReturn dumpBlockTable( AcDbBlockTableRecord *pBlockTableRecord );

	CReturn dumpEntity( AcDbEntity* pEnt );

	CReturn dumpAcDbEntity( AcDbEntity* pEnt );

	CReturn dumpAcDbCircle( AcDbEntity* pEnt );

	CReturn dumpAcDbArc( AcDbEntity* pEnt );

	CReturn dumpAcDbLine( AcDbEntity* pEnt );

	CReturn dumpAcDb2dPolyline( AcDbEntity* pEnt );

	CReturn dumpAcDbPolyline( AcDbEntity* pEnt );

	CReturn dumpAcDbPoint( AcDbEntity* pEnt );

	CReturn dumpAcDbText( AcDbEntity* pEnt );
	CReturn dumpAcDbMText( AcDbEntity* pEnt );

private:  // Data

	CModel m_model;
	CAutoModDb m_autoModDb;
	CImportUtil m_importUtil;

    AcDbDatabase* m_db;

	BOOL m_msgs;
};

#endif