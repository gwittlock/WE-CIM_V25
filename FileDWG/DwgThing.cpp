
#include "stdafx.h"
#include "cmn_resource.h"
#include "MathConst.h"
#include "StringConst.h"
#include "Return.h"
#include "Path.h"

#include "3dBox.h"
#include "DbEntity.h"
#include "DbPoint.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbIterator.h"

#include "Mm2.h"
#include "AutoMod.h"

// AutoDesk includes.
#include "dbsymtb.h"
#include "dbents.h"
#include "dbapserv.h"
#include "dbpl.h"

// #include "adscodes.h"
// #include "adsdef.h"
// #include "acedads.h"
// #include "adsmigr.h"

#include "DwgThing.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static int doExtents = 0;
static CString XY_POS( "XY_POS" );

void XformsGet( AcDbEntity* pEnt, C3x4Matrix* forward, C3x4Matrix* inverse );

const double INVERT_TOL = -0.9999;
BOOL MustInvert( C3x4Matrix& xform );

void SetZ(
			CAutoModDb&		autoModDb,
			int				recIndx,
			double			depth,
			C3dCoord*		pt );

C3dCoord ArcSolution( const C3dCoord& ps, const C3dCoord& pe, double radians );


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: Textron DWG files are really wacked!  The drafters apparently
// forget to reset their UCS while continuing to draw graphically (as
// opposed to using the command line).  Hence, the entities look as if
// they are on one plane but are not.  So, for the meantime, we map
// everything to the TOP workplane.  This will cause problems whenever
// we reintroduce other workplanes.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CDwgThing::CDwgThing()
	: m_db( NULL ),
	  m_msgs( FALSE )
{
}

CDwgThing::~CDwgThing()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDwgThing commands

CReturn
CDwgThing::Read(
				const CString&	dwgFilePath,
				const CString&	cmdbPath,
				int				toolSetupId,
				int				layerSetupId,
				int				materialId,
				BOOL			raw,
				BOOL			buildToolSetup,
				double			rotation,
				BOOL			stripLayers )
{
	ASSERT( (m_db == NULL) );

	CReturn status;
	Acad::ErrorStatus	acadStatus;

	CPath path( dwgFilePath );
	CString ext = path.Ext();

	if ( !ext.CompareNoCase( "dwg" ) || !ext.CompareNoCase( "dxf" ) )
	{
		m_model.Flush();

		m_db = new AcDbDatabase(Adesk::kFalse);

		if ( !ext.CompareNoCase( "dwg" ) )
			acadStatus = m_db->readDwgFile( dwgFilePath, _SH_DENYNO );
		else
			acadStatus = m_db->dxfIn( dwgFilePath );

		// if ( okay )
		if (acadStatus == Acad::eOk)
		{ 
			acdbHostApplicationServices()->setWorkingDatabase(m_db);

			status = Convert(
							cmdbPath,
							toolSetupId,
							layerSetupId,
							materialId,
							raw,
							rotation,
							stripLayers );

			if (m_db)
			{
				delete m_db;
				m_db = NULL;
			}
		}
		else
		{
			status.User( IDS_INTERNAL_ERROR, "CDwgThing::Read() -- bad file." );
		}
	}
	else if ( !ext.CompareNoCase( "mm2" ) )
	{
		// For interactive testing purposes only!
		CMM2 mm2;
		m_model.Flush();
		mm2.Read( dwgFilePath, &m_model, MM2_SKIP_SEQ_OBJS );
	}
    else
	{
		status.User( IDS_INTERNAL_ERROR, "CDwgThing::Read() -- bad file type." );
	}

	return status;
}

CReturn
CDwgThing::Analyze( const CString& dwgFilePath, const CString& anlFilePath ) 
{
	ASSERT( (m_db == NULL) );

	CReturn status;
	AcDbLayerTable* p;

	CPath path( dwgFilePath );
	CString ext = path.Ext();

	if ( !ext.CompareNoCase( "dwg" ) )
	{
		m_db = new AcDbDatabase(Adesk::kFalse);

		// if (m_db->readDwgFile( dwgFilePath, _SH_DENYWR ) == Acad::eOk)
		if (m_db->readDwgFile( dwgFilePath, _SH_DENYNO ) == Acad::eOk)
		{ 
			acdbHostApplicationServices()->setWorkingDatabase(m_db);

			if (m_db->getLayerTable(p, AcDb::kForRead) == Acad::eOk)
			{
				dumpLayerTable(p);
				p->close();
			}

			if (m_db)
			{
				delete m_db;
				m_db = NULL;
			}

			status = AnalysisWrite( anlFilePath );
		}
		else
		{
			status.User( IDS_INTERNAL_ERROR, "CDwgThing::Analyze() -- bad file." );
		}
	}
    else
	{
		status.User( IDS_INTERNAL_ERROR, "CDwgThing::Analyze() -- bad file type." );
	}

	return status;
}

CReturn
CDwgThing::ModelWrite( const CString& mm2FilePath ) 
{
	CMM2 mm2;

	CReturn status = mm2.Write( mm2FilePath, m_model );
	
	return status;
}

CReturn
CDwgThing::AnalysisWrite( const CString& anlFilePath ) 
{
	CReturn status;
	CString msg;

	FILE* f = fopen( anlFilePath, "w" );

	if (f == NULL)
	{
		status.Internal( IDS_FILE_WRITE_ERR, anlFilePath );
	}
	else
	{
		if ( m_msgs )
		{
			msg.Format( "Layers written to <%s>", anlFilePath );
			status.Diagnostic( msg );
		}

		int count = m_importUtil.LayerCount();
		for (int indx = 0; indx < count; ++indx)
		{
			CString layerName = m_importUtil.LayerName( indx );

			fprintf( f, "%s\n", layerName );

			if ( m_msgs )
			{
				status.Diagnostic( layerName );
			}
		}
		fclose( f );
	}
	
	return status;
}

// NOTE: raw and stripLayers should be mutually exclusive(?)
CReturn
CDwgThing::Convert(
				const CString&	dbPath,
				int				toolSetupId,
				int				layerSetupId,
				int				materialId,
				BOOL			raw,
				double			rotation,
				BOOL			stripLayers )
{
	CReturn		status;
	CString		msg;
	CAutoMod	autoMod;
	BOOL		buildToolSetup = FALSE;

	if ( m_msgs )
	{
		msg.Format( "dbPath <%s>\ntoolSetupId <%d>\nlayerSetupId <%d>\nmaterialId <%d>",
			dbPath, toolSetupId, layerSetupId, materialId  );

		status.Diagnostic( "CDwgThing::Convert()" );
		status.Diagnostic( msg );
	}

	status = m_autoModDb.DbOpen( dbPath );
	if (status.IsOk() )
	{
		if (toolSetupId <= 0)
			layerSetupId = 0;  // Assume "preview mode"

		status = m_autoModDb.Init( dbPath, toolSetupId, layerSetupId, materialId, FALSE );
	}

	if ( m_msgs  && status.IsOk())
	{
		status.Diagnostic( "Initialized autoModDb" );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// We gain dramatic improvement in import times by
	// suppressing the undo buffer.
	m_model.UndoBufferSuppress();

	m_importUtil.Init( &m_model, &m_autoModDb );
	if ( status.IsOk() )
	{
#if 0
		// 2007.05.06 (PE) -- Introduced for Dynatorch to support piercing
		// with torch. Per conversation with Gary, Walt wants to simply
		// blow through the materal wherever a point is defined. We
		// accomplish this by first converting the CAD point to an arc
		// whose diameter exactly matches the kerf of the torch. Upon
		// auto-tooling the model, we associate the torch to these arcs
		// via a hole entity.
		CString machine_name = m_autoModDb.MachineName();
		machine_name.MakeLower();
		if (machine_name.Find("dynatorch") >= 0)
		{
			CVarList* tooled_stations = m_autoModDb.TooledStations().GetAt(0);
			if (tooled_stations != NULL)
			{
				double pt_diam = tooled_stations->getReal( "Kerf", UNDEFINED );
				if (pt_diam < UNDEFINED)
					m_importUtil.PointDiameterSet( pt_diam );
			}
		}
#endif
	}

	status = m_importUtil.AutoDeskPlanesCreate();

	if ( m_msgs ) status.Diagnostic( "Passed AutoDeskPlanes()" );

	if ( status.IsOk() )
		status = dumpDatabase();

	if ( m_msgs ) status.Diagnostic( "Passed dumpDatabase()" );

	if ( status.IsOk() )
	{
		m_importUtil.AcadPostProc( rotation, raw );

		m_model.PostReadInit();
	}

	if ( m_msgs ) status.Diagnostic( "Passed AcadPostProc()" );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Tool-up the geometry as necessary.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (status.IsOk() && toolSetupId > 0)
	{
		autoMod.Init( &m_autoModDb, &m_model );

		status = autoMod.ModelPostProcess( buildToolSetup, raw );

		if ( m_msgs ) status.Diagnostic( "Passed ModelPostProcess()" );

		if ( stripLayers && !raw )
			m_importUtil.LayersStrip( &m_model );

		if ( m_msgs ) status.Diagnostic( "Passed LayersStrip()" );
	}

	m_autoModDb.DbClose();

	if (status.IsOk() && toolSetupId > 0)
		status = m_importUtil.WorkZoneCreate( m_model.Box(), 1, 0.1 );

	m_model.UndoBufferActivate();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: The original source for the dump methods was obtained
// from ObjectDBX2000/Samples/dumpdwg.  This is a good source
// for other methods, such as polyline processing, that may
// be needed in the future.
//
CReturn
CDwgThing::dumpDatabase()
{
	CReturn status;

	if ( m_msgs ) status.Diagnostic( "begin dumpDatabase()" );

    AcDbLayerTable *pLayerTable;
    if (m_db->getLayerTable( pLayerTable, AcDb::kForRead ) == Acad::eOk)
	{
		dumpLayerTable( pLayerTable );
        pLayerTable->close();
    }

    AcDbBlockTable *pBlockTable;
    if (m_db->getSymbolTable( pBlockTable, AcDb::kForRead ) == Acad::eOk)
	{
		char blockName[50];
		AcDbBlockTableRecord *pBlockTableRecord;

		strcpy( blockName, ACDB_MODEL_SPACE );
		pBlockTable->getAt( blockName, pBlockTableRecord, AcDb::kForRead );
		pBlockTable->close();

		dumpBlockTable( pBlockTableRecord );
    }

	if ( m_msgs ) status.Diagnostic( "end dumpDatabase()" );

	return status;
}

CReturn
CDwgThing::dumpLayerTable( AcDbLayerTable* pLayerTable )
{
	CReturn status;

    AcDbLayerTableIterator *pIter;
    AcDbLayerTableRecord *pRecord;
    pLayerTable->newIterator(pIter);

    while (!pIter->done())
    {
        if (pIter->getRecord(pRecord, AcDb::kForRead) == Acad::eOk)
        {
            char* pName;
            if (pRecord->getName(pName) == Acad::eOk)
            {
				m_importUtil.LayerAdd( pName );
                acdbFree(pName);
            }
            pRecord->close();
        }
        pIter->step();
    }
    delete pIter;

	return status;
}

CReturn
CDwgThing::dumpBlockTable( AcDbBlockTableRecord *pBlockTableRecord )
{
	CReturn status;
    AcDbBlockTableRecordIterator *pBlockIter;
	AcDbEntity *pEnt;

    pBlockTableRecord->newIterator( pBlockIter );

	while (!pBlockIter->done())
	{
		while ( !pBlockIter->done() )
		{
			if (pBlockIter->getEntity(pEnt, AcDb::kForRead) == Acad::eOk)
			{
				status += dumpEntity( pEnt );
				pEnt->close();
			}
			pBlockIter->step();
		}                            
	}
	delete pBlockIter;

	pBlockTableRecord->close();

	return status;
}

CReturn
CDwgThing::dumpEntity( AcDbEntity* pEnt )
{
	CReturn status;

    const char *p = pEnt->isA()->name();

	if ( status.IsOk() )
	{
		if (strcmp(p, "AcDbPoint") == 0)
			status = dumpAcDbPoint( pEnt );
		else
		if (strcmp(p, "AcDbLine") == 0)
			status = dumpAcDbLine( pEnt );
		else
		if (strcmp(p, "AcDbCircle") == 0)
			status = dumpAcDbCircle( pEnt );
		else
		if (strcmp(p, "AcDbArc") == 0)
			status = dumpAcDbArc( pEnt );
		else
		if (strcmp(p, "AcDbPolyline") == 0)
			status = dumpAcDbPolyline( pEnt );
		else
		if (strcmp(p, "AcDb2dPolyline") == 0)
			status = dumpAcDb2dPolyline( pEnt );
		else
		if (strcmp(p, "AcDbText") == 0)
			status = dumpAcDbText( pEnt );
		else
		if (strcmp(p, "AcDbMText") == 0)
			status = dumpAcDbMText( pEnt );
		else   // not handled
		{
			int i = 0;  // breakpoint for debugging
		}
	}

	return status;
}

CReturn
CDwgThing::dumpAcDbCircle( AcDbEntity* pEnt )
{
	CReturn status;

    char* layerName = pEnt->layer();

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		CGeoArc			geoArc;
		C3x4Matrix		forward;
		C3x4Matrix		inverse;
		C3dCoord		ps;
		C3dCoord		pc;
		C3dVec			normal;
		CDbTool*		dbTool;
		CDbWorkplane*	dbWork;
		CDbArc*			dbArc;

		AcDbCircle *pAcDbCircle = (AcDbCircle*) pEnt;

		double a_radius = pAcDbCircle->radius();
		AcGePoint3d a_center = pAcDbCircle->center();
		double depth = pAcDbCircle->thickness();
		AcGeVector3d a_normal = pAcDbCircle->normal();

		XformsGet( pEnt, &forward, &inverse );

		// AutoDesk coordinates are retrieved in world.
		pc.XYZ( a_center[X], a_center[Y], a_center[Z] );

		// Transform to local.
		forward.Transform( &pc );

		// SetZ( m_autoModDb, layerRecIndx, depth, &pc );

		ps.XYZ( pc.X() + a_radius, pc.Y(), pc.Z() );

		// Transform back to world.
		inverse.Transform( &pc );
		inverse.Transform( &ps );

		SetZ( m_autoModDb, layerRecIndx, depth, &pc );
		SetZ( m_autoModDb, layerRecIndx, depth, &ps );

		geoArc.Init( ps, ps, pc, CCW );
		geoArc.Xform( inverse );

		status = m_model.EntityCreate( DBARC, (CDbEntity**) &dbArc );

		if ( status.IsOk() )
		{
			m_importUtil.CurrentRecordSet( layerRecIndx );

			dbTool = m_importUtil.TargetTool();

			normal.Init( a_normal[X], a_normal[Y], a_normal[Z] );
			// normal.Init( 0, 0, 1 );
			dbWork = m_importUtil.TargetWork( normal );

			dbArc->Init( dbTool, dbWork, geoArc );
			m_importUtil.Color( dbArc );
		}
	}

	return status;
}

CReturn
CDwgThing::dumpAcDbArc( AcDbEntity* pEnt )
{
	CReturn status;

    char* layerName = pEnt->layer();

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		CGeoArc			geoArc;
		C3x4Matrix		forward;
		C3x4Matrix		inverse;
		C3dCoord		ps;
		C3dCoord		pe;
		C3dCoord		pc;
		C3dVec			normal;
		CDbTool*		dbTool;
		CDbWorkplane*	dbWork;
		CDbArc*			dbArc;
		double			tol;

		AcDbArc *pAcDbArc = (AcDbArc*) pEnt;

		double a_radius = pAcDbArc->radius();
		AcGePoint3d a_center = pAcDbArc->center();
		double startAngle = pAcDbArc->startAngle();
		double endAngle = pAcDbArc->endAngle();
		double depth = pAcDbArc->thickness();
		AcGeVector3d a_normal = pAcDbArc->normal();

		XformsGet( pEnt, &forward, &inverse );

		// AutoDesk coordinates are retrieved in world.
		pc.XYZ( a_center[X], a_center[Y], a_center[Z] );

		SetZ( m_autoModDb, layerRecIndx, depth, &ps );

		// Transform to local.
		forward.Transform( &pc );

		// SetZ( m_autoModDb, layerRecIndx, depth, &pc );

		// Create the entity in our database.
		ps.XYZ( pc.X() + a_radius * cos( startAngle ),
				pc.Y() + a_radius * sin( startAngle ),
				pc.Z() );

		pe.XYZ( pc.X() + a_radius * cos( endAngle ),
				pc.Y() + a_radius * sin( endAngle ),
				pc.Z() );

		// Transform back to world.
		inverse.Transform( &pc );
		inverse.Transform( &ps );
		inverse.Transform( &pe );

		SetZ( m_autoModDb, layerRecIndx, depth, &pc );
		SetZ( m_autoModDb, layerRecIndx, depth, &ps );
		SetZ( m_autoModDb, layerRecIndx, depth, &pe );

		geoArc.Init( ps, pe, pc, CCW );
		if ( MustInvert( forward ) )
			geoArc.Dir( CW );

		tol = m_autoModDb.FilterTol();
		if (geoArc.Length2d() > tol)
		{
			status = m_model.EntityCreate( DBARC, (CDbEntity**) &dbArc );

			if ( status.IsOk() )
			{
				m_importUtil.CurrentRecordSet( layerRecIndx );

				dbTool = m_importUtil.TargetTool();

				// NOTE: For some unknown reason, arcs behave differently than circles.
				// normal.Init( a_normal[X], a_normal[Y], a_normal[Z] );
				normal.Init( 0, 0, 1 );
				dbWork = m_importUtil.TargetWork( normal );

				dbArc->Init( dbTool, dbWork, geoArc );
				m_importUtil.Color( dbArc );
			}
		}
	}

	return status;
}

CReturn
CDwgThing::dumpAcDbLine( AcDbEntity* pEnt )
{
	CReturn status;

    char* layerName = pEnt->layer();

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		C3x4Matrix		forward;
		C3x4Matrix		inverse;
		C3dCoord		ps;
		C3dCoord		pe;
		C3dVec			normal;
		CDbTool*		dbTool;
		CDbWorkplane*	dbWork;
		CDbLine*		dbLine;
		double			tol;

		AcDbLine *pAcDbLine = (AcDbLine *)pEnt;
		double depth = pAcDbLine->thickness();
		AcGeVector3d a_normal = pAcDbLine->normal();
		AcGePoint3d a_start = pAcDbLine->startPoint();
		AcGePoint3d a_end = pAcDbLine->endPoint();


		XformsGet( pEnt, &forward, &inverse );

		// AutoDesk coordinates are retrieved in world.
		ps.XYZ( a_start[X], a_start[Y], a_start[Z] );
		pe.XYZ( a_end[X], a_end[Y], a_end[Z] );

		tol = m_autoModDb.FilterTol();
		if ( ps.WithinTol( pe, tol ) )
			return status;  // early exit

		// Transform to local.
		forward.Transform( &ps );
		forward.Transform( &pe );

		// SetZ( m_autoModDb, layerRecIndx, depth, &ps );
		// SetZ( m_autoModDb, layerRecIndx, depth, &pe );

		inverse.Transform( &ps );
		inverse.Transform( &pe );

		SetZ( m_autoModDb, layerRecIndx, depth, &ps );
		SetZ( m_autoModDb, layerRecIndx, depth, &pe );

		// Create the entity in our database.
		status = m_model.EntityCreate( DBLINE, (CDbEntity**) &dbLine );

		if ( status.IsOk() )
		{
			m_importUtil.CurrentRecordSet( layerRecIndx );

			dbTool = m_importUtil.TargetTool();

			//normal.Init( a_normal[X], a_normal[Y], a_normal[Z] );
			normal.Init( 0, 0, 1 );
			dbWork = m_importUtil.TargetWork( normal );

			dbLine->Init( dbTool, dbWork, ps, pe );
			m_importUtil.Color( dbLine );
		}
	}

	return status;
}

// WARNING!  Thrown together.
CReturn
CDwgThing::dumpAcDb2dPolyline( AcDbEntity* pEnt )
{
	CReturn status;

    char* layerName = pEnt->layer();

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		C3x4Matrix		forward;
		C3x4Matrix		inverse;
		C3dCoord		ps;
		C3dCoord		pe;
		C3dVec			normal;
		CDbTool*		dbTool;
		CDbWorkplane*	dbWork;
		CDbLine*		dbLine;
		double			tol;

		AcGePoint3d pt;
		AcDb2dPolyline* pAcDb2dPolyline = (AcDb2dPolyline*) pEnt;
		double depth = pAcDb2dPolyline->thickness();
	    double elevation = pAcDb2dPolyline->elevation();
		AcGeVector3d a_normal = pAcDb2dPolyline->normal();

		tol = m_autoModDb.FilterTol();

		m_importUtil.CurrentRecordSet( layerRecIndx );
		m_importUtil.ZLevel( &depth );

		// normal.Init( a_normal[X], a_normal[Y], a_normal[Z] );
		normal.Init( 0, 0, 1 );
		dbWork = m_importUtil.TargetWork( normal );

		pAcDb2dPolyline->vertexPosition( pt );

		ps.XYZ( pt[X], pt[Y], pt[Z] );

		SetZ( m_autoModDb, layerRecIndx, depth, &ps );

		AcDbObjectIterator* pIter = pAcDb2dPolyline->vertexIterator();
		while ( !pIter->done() )
		{
			AcDbObjectId objId = pIter->objectId();
			AcDb2dVertex* pVert;
			if (pAcDb2dPolyline->openVertex( pVert, objId, AcDb::kForRead ) == Acad::eOk)
			{
				pt = pVert->position();
				pVert->close();

				pe.XYZ( pt[X], pt[Y], pt[Z] );

				SetZ( m_autoModDb, layerRecIndx, depth, &pe );

				if ( !ps.WithinTol( pe, tol ) )
				{
					// Create the entity in our database.
					status = m_model.EntityCreate( DBLINE, (CDbEntity**) &dbLine );

					if ( status.IsOk() )
					{
						dbTool = m_importUtil.TargetTool();

						dbLine->Init( dbTool, dbWork, ps, pe );
						m_importUtil.Color( dbLine );
					}

					ps = pe;
				}
			}
			pIter->step();
		}
		delete pIter;
	}

	return status;
}

CReturn
CDwgThing::dumpAcDbPolyline( AcDbEntity* pEnt )
{
	CReturn status;

    char* layerName = pEnt->layer();

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		C3x4Matrix		forward;
		C3x4Matrix		inverse;
		C3dVec			normal;
		CDbTool*		dbTool;
		CDbWorkplane*	dbWork;
		CDbLine*		dbLine;
		CDbArc*			dbArc;
		double			depth;
		double			elevation;
		double			tol;
		double			bulge;  // tangent of 1/4 of included angle of arc
		double			radians;
		int				count, indx;
		BOOL			err;

		AcGePoint3d		pt;
		AcGeVector3d	a_normal;
		AcDbPolyline*	pAcDbPolyline = (AcDbPolyline*) pEnt;

		C3dCoord		ptA;
		C3dCoord		ptB;
		C3dCoord		pc;
		C3dCoord*		vert;
		CDynamicArray<C3dCoord*> verts;


		count = pAcDbPolyline->numVerts();
		if (count < 2)
			return status;


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Collect the vertex data.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		err = FALSE;
		for (indx = 0; indx < count; ++indx)
		{
			if (pAcDbPolyline->getPointAt( indx, pt ) != Acad::eOk)
			{
				err = TRUE;
				break;
			}

			pAcDbPolyline->getBulgeAt( indx, bulge );

			vert = new C3dCoord( pt[0], pt[1], bulge );
			verts.Append( vert );
		}

		if (err == FALSE && pAcDbPolyline->isClosed())
		{
			vert = new C3dCoord( (*(verts[0])) );  // NOTE: don't really care about Z ordinate
			verts.Append( vert );
		}


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Convert the vertex data to entities.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		if (err == FALSE)
		{
			tol = m_autoModDb.FilterTol();

			depth = pAcDbPolyline->thickness();
			elevation = pAcDbPolyline->elevation();
			a_normal = pAcDbPolyline->normal();

			m_importUtil.CurrentRecordSet( layerRecIndx );
			dbTool = m_importUtil.TargetTool();

			// normal.Init( a_normal[X], a_normal[Y], a_normal[Z] );
			normal.Init( 0, 0, 1);
			dbWork = m_importUtil.TargetWork( normal );

			XformsGet( pEnt, &forward, &inverse );

			count = verts.Count() - 1;

			for (indx = 0; indx < count; ++indx)
			{
				ptA = *(verts[ indx ]);
				ptB = *(verts[indx+1]);

				bulge = ptA.Z();

				ptA.Z( elevation );
				ptB.Z( elevation );

#if REQUIRED
				// Transform to local.
				forward.Transform( &ps );

				// SetZ( m_autoModDb, layerRecIndx, depth, &ps );

				// Transform to world.
				inverse.Transform( &ps );
#endif

				SetZ( m_autoModDb, layerRecIndx, depth, &ptA );
				SetZ( m_autoModDb, layerRecIndx, depth, &ptB );

				if (fabs( bulge ) < SMALL)
				{
					// We have a line.
					if ( !ptA.WithinTol( ptB, tol ) )
					{
						// Create the entity in our database.
						status = m_model.EntityCreate( DBLINE, (CDbEntity**) &dbLine );

						if ( status.IsOk() )
						{
							dbLine->Init( dbTool, dbWork, ptA, ptB );
							m_importUtil.Color( dbLine );
						}
					}
				}
				else
				{
					// We have an arc.
					radians = 4. * atan( bulge );

					pc = ArcSolution( ptA, ptB, radians );

					status = m_model.EntityCreate( DBARC, (CDbEntity**) &dbArc );

					if ( status.IsOk() )
					{
						dbArc->Init( dbTool, dbWork, ptA, ptB, pc, SGN(bulge) );
						m_importUtil.Color( dbArc );
					}
				}
			}
		}

		verts.DestructiveFlush();
	}

	return status;
}

CReturn
CDwgThing::dumpAcDbPoint( AcDbEntity* pEnt )
{
	CReturn status;
	double	pt_diam;
	int		layerRecIndx;
	bool	okay;

	// 2007.05.06 (PE) -- Introduced for Dynatorch to support piercing
	// with torch. Per conversation with Gary, Walt wants to simply
	// blow through the materal wherever a point is defined. We
	// accomplish this by first converting the CAD point to an arc
	// whose diameter exactly matches the kerf of the torch. Upon
	// auto-tooling the model, we associate the torch to these arcs
	// via a hole entity.
#if 0
	pt_diam = m_importUtil.PointDiameterGet();
#else
	pt_diam = UNDEFINED;
#endif

	okay = (pt_diam < UNDEFINED);
	if ( okay )
	{
		char* layerName = pEnt->layer();

		layerRecIndx = m_importUtil.LayerRecFind( layerName );

		okay = (layerRecIndx >= 0);
	}

	if ( okay )
	{
		CGeoArc		geoArc;
		C3dCoord	pc;
		CDbArc*		dbArc;
		AcGePoint3d	pt;
		AcDbPoint*	pAcDbPoint;
		
		pAcDbPoint = (AcDbPoint*) pEnt;
		pt = pAcDbPoint->position();

		pc.XYZ( pt[X], pt[Y], pt[Z] );
		geoArc.Init( pc, (0.5 * pt_diam), CCW );

		status = m_model.EntityCreate( DBARC, (CDbEntity**) &dbArc );

		if ( status.IsOk() )
		{
			C3dVec			normal;
			CDbTool*		dbTool;
			CDbWorkplane*	dbWork;

			m_importUtil.CurrentRecordSet( layerRecIndx );

			dbTool = m_importUtil.TargetTool();

			normal.Init( 0, 0, 1 );
			dbWork = m_importUtil.TargetWork( normal );

			dbArc->Init( dbTool, dbWork, geoArc );
			m_importUtil.Color( dbArc );
		}
	}

#if OKAY
    AcDbPoint *pAcDbPoint = (AcDbPoint *)pEnt;
    entInfo(pEnt, sizeof(AcDbPoint));

    AcGePoint3d position = pAcDbPoint->position();
    double thickness = pAcDbPoint->thickness();
    AcGeVector3d normal = pAcDbPoint->normal();
    double ecsRotation = pAcDbPoint->ecsRotation();
#endif
	return status;
}

CReturn
CDwgThing::dumpAcDbText( AcDbEntity* pEnt )
{
	CReturn status;

    AcDbText *pAcDbText = (AcDbText *)pEnt;

    AcGePoint3d position = pAcDbText->position();

	return status;
}

CReturn
CDwgThing::dumpAcDbMText( AcDbEntity* pEnt )
{
	CReturn status;

	if ( m_autoModDb.LayerSetupAttributes().getInt( "ProcessText", FALSE ) )
	{
		char* layerName = pEnt->layer();

		int layerRecIndx = m_importUtil.LayerRecFind( layerName );
		if (layerRecIndx >= 0)
		{
			AcDbMText *pAcDbMText = (AcDbMText *)pEnt;

			AcGePoint3d ps = pAcDbMText->location();
			char*	text = pAcDbMText->contents();

			// Create the entity in our database.
			CDbCommand* dbCommand;
			status = m_model.EntityCreate( DBCOMMAND, (CDbEntity**) &dbCommand );

			if ( status.IsOk() )
			{
				C3dVec			normal;
				CDbTool*		dbTool;
				CDbWorkplane*	dbWork;

				m_importUtil.CurrentRecordSet( layerRecIndx );

				dbTool = m_importUtil.TargetTool();

				//normal.Init( a_normal[X], a_normal[Y], a_normal[Z] );
				normal.Init( 0, 0, 1 );
				dbWork = m_importUtil.TargetWork( normal );

				dbCommand->Init( dbTool, dbWork, ps[X], ps[Y], 0., text );
				m_importUtil.Color( dbCommand );
			}
		}
	}
	return status;
}

void XformsGet( AcDbEntity* pEnt, C3x4Matrix* forward, C3x4Matrix* inverse )
{
	AcGeMatrix3d	matrix;
	pEnt->getEcs( matrix );

	forward->setI( matrix.entry[0][0], matrix.entry[0][1], matrix.entry[0][2] );
	forward->setJ( matrix.entry[1][0], matrix.entry[1][1], matrix.entry[1][2] );
	forward->setK( matrix.entry[2][0], matrix.entry[2][1], matrix.entry[2][2] );
	forward->InvertTo( inverse );
}

BOOL MustInvert( C3x4Matrix& xform )
{
	if (xform.getK().Z() < INVERT_TOL)
		return TRUE;
	else if (xform.getK().Y() < INVERT_TOL)
		return TRUE;
	else if (xform.getK().X() < INVERT_TOL)
		return TRUE;

	return FALSE;
}

// NOTE: TOTAL KLUDGE because mapping to TOP !!!
void SetZ(
			CAutoModDb&		autoModDb,
			int				recIndx,
			double			depth,
			C3dCoord*		pt )
{
#if BEFORE_2007_08_10
	CReturn status = autoModDb.CurrentRecordSet( recIndx );

	if ( status.IsOk() )
	{
		EAutoModMode zLevelMode = autoModDb.ZLevelMode();
		double zlevel, thick;

		if (zLevelMode == AUTOMOD_USE_CMDB_Z)
		{
			// Use the Z value explicitly specified in the cmdb.
			zlevel = autoModDb.DoubleGet( "Z_Level" );
		}
		else
		{
			zlevel = depth;
		}

		thick = autoModDb.MaterialThick();
		if (thick >= UNDEFINED)
		{
			// 2006.05.13 (PE) -- Otherwise file preview fails
			// because the bounding box is deemed to be invalid.
			thick = 0.;
		}

		pt->Z( zlevel + thick );
	}
#else
	// Sunflower Mfg reported that entities were not at Z0!!!
	pt->Z( 0. );
#endif
}

// The center point is found as the intersection between the radial line
// passing through the starting point of the arc and the radial line passing
// through the midpoint of the chord.
C3dCoord ArcSolution( const C3dCoord& ps, const C3dCoord& pe, double radians )
{
	CGeoLine	chord;
	C2dCoord	pm;
	C3dCoord	pc;
	double		xs, ys;
	double		xe, ye;
	double		xm, ym;
	int			dir;

	dir = SGN( radians );

	xs = ps.X();
	ys = ps.Y();

	xe = pe.X();
	ye = pe.Y();

	// The chord.
	chord.StartPt( xs, ys, 0. );
	chord.EndPt( xe, ye, 0. );

	pm = chord.MidPt();
	xm = pm.X();
	ym = pm.Y();

	// The given radians IS the included angle of the arc
	// This math is probably horribly inefficient...
	double half_radian = radians * 0.5;
	double radius = chord.Length2d() / (2.0 * sin(half_radian));
	double d = radius * cos(half_radian);

	radians = chord.StartTan().Radians() + HALFPI;
	pc = C3dCoord( xm + d*cos(radians), ym + d*sin(radians), ps.Z() );

	return pc;
}
