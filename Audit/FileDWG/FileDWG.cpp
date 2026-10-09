// FileDWG.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileDWG.h"
#include "FileDWGDlg.h"

#include "adesk.h"
#include "dbsymtb.h"

#include "dbapserv.h"

#include "Return.h"
#include "VarList.h"
#include "DwgThing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int ParseCmdLine( int argc, char** argv, CVarList* args );


/////////////////////////////////////////////////////////////////////////////
// CFileDWGApp

BEGIN_MESSAGE_MAP(CFileDWGApp, CWinApp)
	//{{AFX_MSG_MAP(CFileDWGApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileDWGApp construction

CFileDWGApp::CFileDWGApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFileDWGApp object

CFileDWGApp theApp;

class DumpDwgHostApp : public AcDbHostApplicationServices
{
    Acad::ErrorStatus findFile(char* pcFullPathOut, int nBufferLength,
                         const char* pcFilename, AcDbDatabase* pDb = NULL,
                         AcDbHostApplicationServices::FindFileHint = kDefault);
};

Acad::ErrorStatus 
DumpDwgHostApp::findFile(char* pcFullPathOut, int nBufferLength,
    const char* pcFilename, AcDbDatabase* pDb, 
    AcDbHostApplicationServices::FindFileHint hint)
{
    char pExtension[5];
    switch (hint)
    {
        case kCompiledShapeFile:
            strcpy(pExtension, ".shx");
            break;
        case kTrueTypeFontFile:
            strcpy(pExtension, ".ttf");
            break;
        case kPatternFile:
            strcpy(pExtension, ".pat");
            break;
        case kARXApplication:
            strcpy(pExtension, ".dbx");
            break;
        case kFontMapFile:
            strcpy(pExtension, ".fmp");
            break;
        case kXRefDrawing:
            strcpy(pExtension, ".dwg");
            break;
        case kFontFile:                // Fall through.  These could have
        case kEmbeddedImageFile:       // various extensions
        default:
            pExtension[0] = '\0';
            break;
    }
    char* filePart;
    DWORD result;
    result = SearchPath(NULL, pcFilename, pExtension, nBufferLength, 
                        pcFullPathOut, &filePart);
    if (result && result < (DWORD)nBufferLength)
        return Acad::eOk;
    else
        return Acad::eFileNotFound;
}


DumpDwgHostApp gDumpDwgHostApp;


/////////////////////////////////////////////////////////////////////////////
// CFileDWGApp initialization

BOOL CFileDWGApp::InitInstance()
{
	eSeverity	error_level;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// __argc, __argv are globals analogous to those in main( argc, argv )
	//
	// Otherwise, must figure out how to use the MFC gibberish as follows:
	//
	//	// Parse command line for standard shell commands, DDE, file open
	//	CCommandLineInfo cmdInfo;
	//	ParseCommandLine(cmdInfo);
	//
	//	// Dispatch commands specified on the command line
	//	if (!ProcessShellCommand(cmdInfo))
	//		return FALSE;
	//
	CVarList args;

	int status = ParseCmdLine( __argc, __argv, &args );

	if (status < 0)
		return FALSE;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// AutoDesk stuff.
	//
    acdbSetHostApplicationServices(&gDumpDwgHostApp);
    long lcid = 0x00000409;  // English
    acdbValidateSetup(lcid);
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


	CDwgThing dwgConvertor;

	if (status == 0)
	{
		// Must be running interactively.

		CFileDWGDlg dlg;
		m_pMainWnd = &dlg;

		dlg.ConvertorSet( dwgConvertor );

		dlg.DoModal();
	}
	else
	{
		CReturn status;

		CString dwgPath  = args.getString( "dwg", "" );
		CString anlPath  = args.getString( "anl", "" );
		CString dbPath   = args.getString( "db", "" );
		CString mm2Path  = args.getString( "mm2", "" );
		int toolSetupId  = args.getInt( "tool_setup_id", 0 );
		int layerSetupId = args.getInt( "layer_setup_id", 0 );
		int materialId   = args.getInt( "material_index", 0 );
		BOOL raw = args.getInt( "raw", FALSE );

		BOOL	stripLayers = args.getInt( "striplayers", FALSE );
		double	rotation = args.getReal( "rotation", 0. );

		BOOL msgs = args.getInt( "msgs", FALSE );
		dwgConvertor.MsgsActivate( msgs );
		if ( msgs )
		{
			CReturn::EWM_Handler_Init();
			error_level = CReturn::ErrorLevel();
			CReturn::ErrorLevel( WARN_DIAGNOSTIC );
		}
		// dwgConvertor.MsgsActive( TRUE );

		if (anlPath.GetLength() > 0)
		{
			status = dwgConvertor.Analyze( dwgPath, anlPath );
		}
		else
		{
			status = dwgConvertor.Read(
									dwgPath,
									dbPath,
									toolSetupId,
									layerSetupId,
									materialId,
									raw,
									FALSE,
									rotation,
									stripLayers );

			if ( status.IsOk() )
				status = dwgConvertor.ModelWrite( mm2Path );
		}

		if ( msgs )
		{
			CReturn::ErrorLevel( error_level );
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//////////////////////////////////////////////////////////////
// Parse the command line parameters so the application can
// be run in batch mode.
//
// For converting a dwg to a mm2
//    -dwg <input_file_name.dwg>
//    -db  <config_mgr_database.cdb>
//    -tool_setup_id %d
//    -layer_setup_id %d
//    -material_index %d
//    -rotation %f
//    -striplayers %b
//    -mm2 <output_file_name.mm2>
//
// For analyzing the layers in a dwg
//    -dwg <input_file_name.dwg>
//    -anl <output_file_name.anl>
//
int ParseCmdLine( int argc, char** argv, CVarList* args )
{
	CReturn status;
	CString msg;

	if (argc == 1)
		return 0;  // Nothing to do, must be running interactively?

	if (argc > 1 && strcmp( argv[1], "//" ) == 0)
		return 0;  // Command line arguments are commented out.

	if ( strcmp( argv[1], "-c" ) )
		return -1;

	FILE* cmdFile = fopen( argv[2], "r" );
	if (cmdFile == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, argv[2] );
		return -2;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the parameter from the command file.

	const int MAXBUF = 256;
	char buf[MAXBUF];

	BOOL haveDwg  = FALSE;
	BOOL haveDb   = FALSE;
	BOOL haveLay  = FALSE;
	BOOL haveTool = FALSE;
	BOOL haveMat  = FALSE;
	BOOL haveMm2  = FALSE;
	BOOL haveAnl  = FALSE;
	BOOL haveRaw  = FALSE;
	BOOL okay = FALSE;

	msg = "dwg params --\n";
	while (fgets( buf, MAXBUF, cmdFile) != NULL)
	{
		int len = strlen( buf );
		if (buf[--len] == '\n')
			buf[len] = '\0';

		CString tmp = buf;
		int indx = tmp.Find( ':' );
		if (indx < 0)
			continue;

		CString name = tmp.Mid( 0, indx );
		CString value = tmp.Mid( indx+1 );

		args->setString( name, value );

		msg += (name + " " + value + "\n");

		if ( !strcmp( name, "dwg" ) )
			haveDwg = TRUE;

		if ( !strcmp( name, "db" ) )
			haveDb = TRUE;

		if ( !strcmp( name, "tool_setup_id" ) )
			haveTool = TRUE;

		if ( !strcmp( name, "layer_setup_id" ) )
			haveLay = TRUE;

		if ( !strcmp( name, "material_index" ) )
			haveMat = TRUE;

		if ( !strcmp( name, "mm2" ) )
			haveMm2 = TRUE;

		if ( !strcmp( name, "anl" ) )
			haveAnl = TRUE;

		if ( !strcmp( name, "raw" ) )
			haveRaw = TRUE;
	}

	fclose( cmdFile );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if ( haveAnl )
	{
		okay = haveDwg;
	}
	else
	{
		// Do we need to check for haveRaw?
		okay = (haveDwg && haveDb && haveLay && haveTool && haveMat && haveMm2);
	}

	if ( !okay )
	{
		MessageBox( NULL,
					"Bad runtime parameters.",
					"mm2dwg.exe -- Fatal Error",
					(MB_OK|MB_ICONSTOP) );

		return -3;
	}

	// MessageBox( NULL, msg, "mm2dwg.exe -- params", (MB_OK|MB_ICONSTOP) );

	return argc;
}
