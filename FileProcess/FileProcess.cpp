// FileProcess.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Path.h"
#include "StrgList.h"
#include "Register.h"

#include "DbAllEntities.h"
#include "DbIterator.h"

#include "AutoModDb.h"
#include "AutoMod.h"
#include "ImportUtil.h"

#include "MM2.h"
#include "DXF.h"
// #include "xml.h"
#include "DxfWriter.h"
#include "ExtAscii.h"

// #include "ModelHtml.h"
#include "ModelText.h"
#include "ProfileBuilder.h"

#include "FileProcess.h"


CRouteList CFileProcessApp::m_fileRouter;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CReturn CFileProcessApp::RegisterProcess( CRouteList* io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_route->addSubrouter( "File", &m_fileRouter );
	m_fileRouter.addProcess( CString("ExportMM2"), ExportMM2 );
	m_fileRouter.addProcess( CString("ExportXML"), ExportXML );
	m_fileRouter.addProcess( CString("ImportMM2"), ImportMM2 );
	m_fileRouter.addProcess( CString("MergeMM2"), MergeMM2 );

	m_fileRouter.addProcess( CString("ExportMacro"), ExportMacro );

	m_fileRouter.addProcess( CString("ImportDXF"), ImportDXF );
	m_fileRouter.addProcess( CString("AnalyzeDXF"), AnalyzeDXF );
	m_fileRouter.addProcess( CString("ExportDXF"), ExportDXF );

	m_fileRouter.addProcess( CString("ImportExtAscii"), ImportExtAscii );
	m_fileRouter.addProcess( CString("AnalyzeExtAscii"), AnalyzeExtAscii );

	m_fileRouter.addProcess( CString("ImportIGES"), ImportIGES );
	m_fileRouter.addProcess( CString("AnalyzeIGES"), AnalyzeIGES );

	m_fileRouter.addProcess( CString("ImportXML"), ImportXML );

	m_fileRouter.addProcess( CString("TokenValue"), TokenValue );

	m_fileRouter.addProcess( CString("ModelDump"), ModelDump );
	m_fileRouter.addProcess( CString("MM2Dump"), MM2Dump );

	return CReturn( STATUS_OKAY );
}

// ==================================================================

CReturn CFileProcessApp::File( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_fileRouter.Dispatch( io_cmd );
}


// ==================================================================
// File:ExportMM2: file=%s
//		
CReturn CFileProcessApp::ExportMM2( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;
	CString	caption;
	CString	message;
	CString	filename;
	CMM2	file;

	CModel&	model = io_cmd->getModel();

	ret = io_cmd->getString( "file", &filename );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_FILE_PARAM_MISSING );
		return ret;
	}
	
	// These attributes are set by CCodeGenProcessApp::Generate()
	bool sheets = (model.Header().getInt( "sheets", FALSE ) != FALSE);
	bool subs = (model.Header().getInt( "subs", FALSE ) != FALSE);

	if (sheets || subs)
	{
		// I give up.  Resource is defined and things compile
		// but the ~!@#$%^&*( string will not load.  Ugh.
		//
		// caption.LoadString( IDS_ERROR );
		// message.LoadString( IDS_FILE_SAVE_ERR );
		caption = "Error";
		message = "Can not save this model because it has been used to code subroutines or multiple sheets.";
		MessageBox( NULL, message, caption, (MB_OK|MB_ICONEXCLAMATION) );
	}
	else
	{
		CPath fullpath( filename );
		DirectoryCreate( fullpath.DriveDir() );

		// NOTE: If we are processing RTLs, avoid writing the project name
		// to the model header.  Otherwise, we might generate an RTL diff
		// that differs only by the project name.
		ret += file.Write( filename, io_cmd->getModel() );
	}

	return ret;
}


// ==================================================================
// File:ImportMM2: file=%s [, preview=%d]
//		
CReturn CFileProcessApp::ImportMM2( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CMM2	file;
	int		mode;

	CModel&	model = io_cmd->getModel();

	bool isPreview = (io_cmd->VarList().getInt( "preview", FALSE ) != 0);

	CString filename = io_cmd->VarList().getString( "file", "" );
	if ( filename.IsEmpty() )
	{
		status.Internal( IDS_FILE_PARAM_MISSING );
	}
	else
	{
		mode = (isPreview ? MM2_PREVIEW_MODE : MM2_STD_MODE);
		status = file.Read( filename, &model, mode );

		if ( status.IsOk() && !isPreview )
		{
			filename = model.Header().getString( "Clamp_CTG", "" );
			CDbFeature::ClampInitFromCTG( filename );

			filename = model.Header().getString( "Hold_CTG", "" );
			CDbFeature::HoldInitFromCTG( filename );
		}
	}

	return status;
}

// ==================================================================
// File:MergeMM2:file=%s, ox=%g, oy=%g, oz=%g
//
//		
CReturn 
CFileProcessApp::MergeMM2( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	CString		filename;
	C3dCoord	origin;
	double		val;
	CMM2		file;

	ret += io_cmd->getString( "file", &filename );

	ret += io_cmd->getReal( "ox", &val );
	origin.X( val );
	ret += io_cmd->getReal( "oy", &val );
	origin.Y( val );
	ret += io_cmd->getReal( "oz", &val );
	origin.Z( val );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_FILE_PARAM_MISSING );
		return ret;
	}

	ret += file.Merge( filename, origin, &io_cmd->getModel(), NULL );

	return ret;
}


// ==================================================================
// File:ExportXML: file=%s
//		
CReturn CFileProcessApp::ExportXML( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#if 0
	CString	filename;

	CModel&	model = io_cmd->getModel();

	CReturn status = io_cmd->getString( "file", &filename );
	if (!status.isOkay())
	{
		status.Internal( IDS_FILE_PARAM_MISSING );
		return status;
	}

	CPath fullpath( filename );
	DirectoryCreate( fullpath.DriveDir() );

	CXML file;
	status += file.Write( filename, io_cmd->getModel() );

	return status;
#else
	CReturn status;
	return status;
#endif
}


// ==================================================================
// File:ImportXML: file=%s
//		
CReturn CFileProcessApp::ImportXML( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#if 0
	CString	filename;

	CModel&	model = io_cmd->getModel();

	CReturn status = io_cmd->getString( "file", &filename );
	if (!status.isOkay())
	{
		status.Internal( IDS_FILE_PARAM_MISSING );
		return status;
	}

	CPath fullpath( filename );
	DirectoryCreate( fullpath.DriveDir() );

	CXML file;
	status += file.Read( filename, &model );

	return status;
#else
	CReturn status;
	return status;
#endif
}

//==================================================================
// File:ExportMacro: file = %s
//		
CReturn 
CFileProcessApp::ExportMacro( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CString filename;

	CModel&	model = io_cmd->getModel();

	status = io_cmd->getString( "file", &filename );

	if ( status.IsOk() )
	{
		status = MacroDump( model, filename );
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::ExportMacro()" );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// File:ImportDXF: dxf=%s, db=%s,
//					[layersetupid=%d,]
//					[toolsetupid=%d,]
//					[materialindex=%d]
//					[buildsetup=%d,]
//					[raw=%d"]
//					[ptol=%f,]
//					[mtol=%f,]
//					[rotation=%d,]  -- in degrees
//					[striplayers=%d]
//
//		dxf   = fully qualified file path to dxf file
//		db    = fully qualified file path to machine database file
//		id    = layer setup id in the machine database
//		toolsetupid = (0) no auto tooling / (else) auto tool
//
// raw -- when true, the model is to be created
//        without profiles and toolpath.
//
// toolSetupID -- when zero, implied "preview" mode
//
CReturn 
CFileProcessApp::ImportDXF( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CString	dxfPath;
	CString	dbPath;
	int		layerSetupID;
	int		toolSetupID;
	int		materialID;
	bool	buildSetup;
	bool	raw;
	double	ptol, mtol;
	double	rotation;
	bool	stripLayers;

	const CVarList& params = io_cmd->VarList();

	// --------------------------------------------------------------

	dxfPath			= params.getString( "dxf", "" );
	dbPath			= params.getString( "db", "" );

	toolSetupID		= params.getInt( "toolsetupid", 0 );
	layerSetupID	= params.getInt( "layersetupid", 0 );
	materialID		= params.getInt( "materialindex", 0 );
	buildSetup		= (params.getInt( "buildsetup", FALSE ) != FALSE);
	raw				= (params.getInt( "raw", FALSE ) != 0);
	ptol			= params.getReal( "ptol", 0.001 );
	mtol			= params.getReal( "mtol", 0.001 );
	rotation		= params.getReal( "rotation", 0. );
	stripLayers		= (params.getInt( "striplayers", FALSE ) != FALSE);

	// NOTE: When the toolSetupID, layerSetupID & materialID are zero
	// we can assume we are importing for the purpose of previewing the file.
	if ( dxfPath.IsEmpty() || dbPath.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::ImportDXF()" );
	}
	else
	{
		CImportUtil	importUtil;
		CAutoMod	autoMod;
		CAutoModDb	autoModDb;
		CDxf		file;
		CString		machine_name;
		double		pt_diam;
		CModel&		model = io_cmd->getModel();

		model.UndoBufferSuppress();

		// 2007.05.06 (PE) -- Introduced for Dynatorch to support piercing
		// with torch. Per conversation with Gary, Walt wants to simply
		// blow through the materal wherever a point is defined. We
		// accomplish this by first converting the CAD point to an arc
		// whose diameter exactly matches the kerf of the torch. Upon
		// auto-tooling the model, we associate the torch to these arcs
		// via a hole entity.
		pt_diam = UNDEFINED;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Extract the necessary data from the cmdb.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		status = autoModDb.DbOpen( dbPath );
		if ( status.IsOk() )
		{
			status = autoModDb.Init(
								dbPath,
								toolSetupID,
								layerSetupID,
								materialID,
								buildSetup );
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Read the CAD geometry into an intermediate format.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		if ( status.IsOk() )
		{
			CCadLayerList acceptedLayers;

			importUtil.AcceptedLayersGet( autoModDb, &acceptedLayers );

			status = file.Read( dxfPath, acceptedLayers, &autoModDb );
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Convert the CAD geoemtry into our model format.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		if ( status.IsOk() )
		{
			importUtil.Init( &model, &autoModDb );

			importUtil.CadEntitiesConvert( file.Entities() );

			importUtil.AcadPostProc( rotation, raw );

			model.PostReadInit();
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Tool-up the geometry as necessary.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		// NOTE: toolSetupID will be zero during file preview.
		if (status.IsOk() && (toolSetupID > 0))
		{
			autoMod.Init( &autoModDb, &model );

			status = autoMod.ModelPostProcess( buildSetup, raw );

			if ( stripLayers && !raw )
				importUtil.LayersStrip( &model );
		}

		autoModDb.DbClose();

		if (status.IsOk() && !raw && toolSetupID > 0)
			status = importUtil.WorkZoneCreate( model.Box(), 1, 0.1 );

		model.UndoBufferActivate();
	}

	return status;
}

// File:AnalyzeDXF: dxf=%s, anl=%s
// NOTE: This method of generating the output file is consistent
// with the sister method in CDwgThing::AnalysisWrite()
// (FileDWG\DwgThing.cpp)
CReturn 
CFileProcessApp::AnalyzeDXF( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	dxfFileName;
	CString anlFileName;
	CReturn status;

	status  = io_cmd->getString( "dxf", &dxfFileName );
	status += io_cmd->getString( "anl", &anlFileName );


	if ( !status.isOkay() || dxfFileName.GetLength() < 1 || anlFileName.GetLength() < 1 )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::AnalyzeDXF()" );
		return status;
	}

	CDxf dxf;
	status = dxf.Analyze( dxfFileName );

	if ( status.IsOk() )
	{
		FILE* f = fopen( anlFileName, "w" );

		if (f == NULL)
		{
			status.Internal( IDS_FILE_WRITE_ERR, anlFileName );
		}
		else
		{
			int count = dxf.LayerCount();
			if (count == 0)
			{
				fprintf( f, "*default*\n" );
			}
			else
			{
				for (int indx = 0; indx < count; ++indx)
				{
					CString layerName = dxf.LayerName( indx );

					// Zero-length strings cause the application
					// to crash because the corresponding field
					// in the CMDB does not support zero-length
					// strings.  And though VB could filter-out
					// these strings, I have decided to do it
					// here.  And just how did an empty layer
					// name appear?  Answer: It didn't; it
					// appeared because the dxf parser is imperfect.
					// Fixing it the right way is more bother
					// than it is worth.
					if ( !layerName.IsEmpty() )
						fprintf( f, "%s\n", layerName );
				}
			}
			fclose( f );
		}
	}

	return status;
}

// File:ExportDXF: dxf=%s, prec=%d, layers%b
CReturn 
CFileProcessApp::ExportDXF( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDxfWriter	writer;
	CString		dxfPath;
	int			mantissa;
	bool		layers;

	CModel&	model = io_cmd->getModel();
	const CVarList& params = io_cmd->VarList();

	dxfPath = params.getString( "dxf", "c:\\_tmp\\foo.dxf" );
	mantissa = params.getInt( "prec", 4 );
	layers = (params.getInt( "layers", FALSE ) != FALSE);

	status = writer.Write( dxfPath, model, mantissa, layers );

	return status;
}


// ==================================================================
// File:ImportIGES: iges=%s, db=%s, [layersetupid=%d,] [toolsetupid=%d,] [materialindex=%f]
//
//		iges   = fully qualified file path to iges file
//		db    = fully qualified file path to machine database file
//		id    = layer setup id in the machine database
//		toolsetupid = (0) no auto tooling / (else) auto tool
//		
CReturn 
CFileProcessApp::ImportIGES( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CString	igesFileName;
	CString dbPath;
	int layerSetupId = 0;
	int toolSetupId  = 0;
	int materialId   = 0;

	CModel&	model = io_cmd->getModel();

	// --------------------------------------------------------------
	// Get the parameters.  Standard usage requires all parameters.

	status  = io_cmd->getString( "iges", &igesFileName );
	status += io_cmd->getString( "db", &dbPath );

	io_cmd->getInt( "toolsetupid",   &toolSetupId );
	io_cmd->getInt( "layersetupid",  &layerSetupId );
	io_cmd->getInt( "materialindex", &materialId );

	// NOTE: When the toolSetupId, layerSetupId & materialId are zero
	// we can assume we are importing for the purpose of previewing the file.
	if ( !status.isOkay() ||
		 igesFileName.GetLength() < 1 ||
		 dbPath.GetLength() < 1 )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::ImportIGES()" );
		return status;
	}

#if OKAY
	model.UndoBufferSuppress();

	CAutoModDb autoModDb;
	CIges file;
	status = autoModDb.Open( dbPath );

	if ( status.IsOk() )
		status = autoModDb.Init( toolSetupId, layerSetupId, materialId );

	if ( status.IsOk() )
		status = file.Read( dxfFileName, &model, &autoModDb );

	autoModDb.Close();

	model.UndoBufferActivate();
#endif

	return status;
}

// File:AnalyzeIGES: iges=%s, anl=%s
CReturn 
CFileProcessApp::AnalyzeIGES( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	igesFileName;
	CString anlFileName;
	CReturn status;

	status  = io_cmd->getString( "iges", &igesFileName );
	status += io_cmd->getString( "anl", &anlFileName );


	if ( !status.isOkay() || igesFileName.GetLength() < 1 || anlFileName.GetLength() < 1 )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::AnalyzeIGES()" );
		return status;
	}


	return status;
}


// ==================================================================
// ==================================================================
// File:ImportExtAscii: asc=%s, db=%s, [layersetupid=%d,] [toolsetupid=%d,] [materialindex=%f]
//
//		asc   = fully qualified file path to dxf file
//		db    = fully qualified file path to machine database file
//		layersetupid    = layer setup id in the machine database
//		toolsetupid = (0) no auto tooling / (else) auto tool
//		materialindex   = 
//		
CReturn 
CFileProcessApp::ImportExtAscii( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
#if REQUIRED
	CString	ascFileName;
	CString dbPath;
	int layerSetupId = 0;
	int toolSetupId = 0;
	int materialId = 0;

	CExtAscii file;
	CModel&	model = io_cmd->getModel();

	// --------------------------------------------------------------
	// Get the parameters.  Standard usage requires all parameters.

	status  = io_cmd->getString( "asc", &ascFileName );
	status += io_cmd->getString( "db", &dbPath );

	io_cmd->getInt( "toolsetupid", &toolSetupId );
	io_cmd->getInt( "layersetupid", &layerSetupId );
	io_cmd->getInt( "materialindex", &materialId );

	// NOTE: When the toolSetupId, layerSetupId & materialId are zero
	// we can assume we are importing for the purpose of previewing the file.
	if ( !status.isOkay() ||
		 ascFileName.GetLength() < 1 ||
		 dbPath.GetLength() < 1 )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::ImportExtAscii()" );
		return status;
	}

	model.UndoBufferSuppress();

	CAutoModDb autoModDb;

	status = autoModDb.DbOpen( dbPath );
	if (status.IsOk() )
		status = autoModDb.Init( dbPath, toolSetupId, layerSetupId, materialId, FALSE );

	if ( status.IsOk() )
	{
		ascFileName = CPath::FileNameExpand( ascFileName );
		status = file.Read( ascFileName, autoModDb, &model );
	}
	
	autoModDb.DbClose();

	model.UndoBufferActivate();
#endif
	return status;
}

// File:AnalyzeExtAscii: asc = string, db = string, toolsetupid = int, anl = string
CReturn 
CFileProcessApp::AnalyzeExtAscii( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
#if REQUIRED
	CString	ascFileName;
	CString dbPath;
	CString anlFileName;
	int toolSetupId;

	status  = io_cmd->getString( "asc", &ascFileName );
	status += io_cmd->getString( "db", &dbPath );
	status += io_cmd->getString( "anl", &anlFileName );
	status += io_cmd->getInt( "toolsetupid", &toolSetupId );

	if ( !status.isOkay() ||
		 ascFileName.GetLength() < 1 ||
		 dbPath.GetLength() < 1 ||
		 anlFileName.GetLength() < 1 )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::AnalyzeExtAscii()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CAutoModDb autoModDb;
	CExtAscii asc;

	status = autoModDb.Init( dbPath, toolSetupId, -1, -1, FALSE );

	if ( status.IsOk() )
		status = asc.Analyze( ascFileName, autoModDb );

	if ( status.IsOk() )
		status = asc.AnalysisWrite( anlFileName );
#endif
	return status;
}


// ==================================================================
// File:TokenValue: string = %s, name = %s
//
CReturn 
CFileProcessApp::TokenValue( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CString string;
	CString name;

	status  = io_cmd->getString( "string", &string );
	status += io_cmd->getString( "name", &name );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::TokenValue()" );
		return status;
	}

	CString value = CImportUtil::TokenValue( string, name );

	io_cmd->setString( "value", value );

	return status;
}

// ==================================================================
// File:ModelDump: file = %s [type=%d]
//
// Where type (0) html / (1) text.
// The default is html.
//
CReturn 
CFileProcessApp::ModelDump( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();
		
	CString	fileName = io_cmd->VarList().getString( "file", "" );
	int fileType = io_cmd->VarList().getInt( "type", 0 );

	if ( fileName.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::ModelDump()" );
	}
	else
	{
//		if (fileType == 1)
		{
			CModelText text;
			text.Enable( true );
			status = text.Dump( fileName, model );
		}
//		else
//		{
//			CModelHtml html;
//			status = html.Dump( fileName, model );
//		}
	}

	return status;
}

// ==================================================================
// File:MM2Dump: mm2 = %s, txt = %s
//
CReturn 
CFileProcessApp::MM2Dump( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CMM2 mm2;
	CModel model;
	CModelText formatter;
		
	CString	mm2FileName = io_cmd->VarList().getString( "mm2", "" );
	CString	txtFileName = io_cmd->VarList().getString( "txt", "" );

	if ( mm2FileName.IsEmpty() || txtFileName.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CFileProcessApp::MM2Dump()" );
	}
	else
	{
		status = mm2.Read( mm2FileName, &model, MM2_STD_MODE );
		if ( status.IsOk() )
			status = formatter.Dump( txtFileName, model );
	}

	return status;
}

// NOTE: Recursive
bool
CFileProcessApp::DirectoryCreate( const CString& dir )
{
	bool status;
	CFileFind finder;

	if (finder.FindFile( dir ) == 0)
	{
		// The directory was not found.
		status = FALSE;

		int indx = dir.ReverseFind( '\\' );
		if (indx > 0)
		{
			CString parent = dir.Left( indx );
			if ( DirectoryCreate( parent ) )
			{
				// The parent directory was either found, or created.
				status = (CreateDirectory( dir, NULL ) != 0);
			}
		}
	}
	else
	{
		status = TRUE;
	}

	return status;
}
