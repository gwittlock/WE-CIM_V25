//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// PM4/JOF Convertor.  Three uses (where "fqp" means full-qualified-path"):
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// NOTE: These particular comments are out-of-date.
//
// 1. Converting a pm4/jof combination to an mm2
//        -use 0 -out 1 -pm4 <fqp.pm4> -jof <fqp.jof> -mm2 <fqp.mm2>
//
// 2. Converting a jof to a mdb
//        -use 1 -jof <fqp.jof> -mdb <fqp.mdb>
//
// 3. Dumping the jof contents in native and converted forms.
//    In this case, two files are generated in the directory
//    containing the jof file:
//
//    i)  the raw jof form (attribs.txt)
//    ii) the we-cim form (tools.txt)
//
//        -use 2 -jof <fqp.jof>
//
//
#include "stdafx.h"
#include "Return.h"
#include "Register.h"
#include "Path.h"
#include "Var.h"
#include "VarList.h"
#include "Pm4JofConvertor.h"
#include "SmartCamConvertor.h"
#include "SmartCamConvertorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString EMPTY_STRING( "" );

static int ParseCmdLine( int argc, char** argv, CVarList* args );


/////////////////////////////////////////////////////////////////////////////
// CSmartCamConvertorApp

BEGIN_MESSAGE_MAP(CSmartCamConvertorApp, CWinApp)
	//{{AFX_MSG_MAP(CSmartCamConvertorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartCamConvertorApp construction

CSmartCamConvertorApp::CSmartCamConvertorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSmartCamConvertorApp object

CSmartCamConvertorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSmartCamConvertorApp initialization

BOOL CSmartCamConvertorApp::InitInstance()
{
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
		return FALSE;  // errored out.


	CPm4JofConvertor convertor;

	int use = args.getInt( "-use", BAD_USAGE );
	int debug = args.getInt( "-d", 0 );
	
	if (debug) CReturn::ErrorLevel( WARN_DIAGNOSTIC );

	CRegister::SetRootPath( "Software\\WE-CIM\\22.0" );


	if (use == INTERACTIVE)
	{
		// Must be running interactively.

		CSmartCamConvertorDlg dlg;
		m_pMainWnd = &dlg;

		dlg.Debug( ((debug > 0) ? TRUE : FALSE) );
		dlg.Interactive( TRUE );
		dlg.DoModal();
	}
	else
	{
		if (use == PM4JOF_TO_MM2)
		{

			convertor.Debug( ((debug > 0) ? TRUE : FALSE) );

			convertor.MM2Create(
							args.getString( "-mdb", EMPTY_STRING ),
							args.getString( "-pm4", EMPTY_STRING ),
							args.getString( "-jof", EMPTY_STRING ),
							args.getString( "-mm2", EMPTY_STRING ),
							args.getString( "-mach", EMPTY_STRING),
							args.getString( "-tool", EMPTY_STRING),
							args.getString( "-mat", EMPTY_STRING),
							(args.getInt( "-out", 0 ) != 0) );
		}
		else if (use == JOF_TO_MDB)
		{
			convertor.MDBCreate(
							args.getString( "-jof", EMPTY_STRING ),
							args.getString( "-mdb", EMPTY_STRING ) );
		}
		else if (use == DEBUG_DUMP)
		{
			convertor.Dump(	args.getString( "-jof", EMPTY_STRING ) );
		}

		if (debug)
		{
			CReturn tmp;
			tmp.Diagnostic( "Done." );
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
// NOTE: All filepaths must be fully qualified.
//
// For converting a pm4/jof to a mm2
//    -d    <debug (0) off  (1) on>
//    -use  <usage (0) pm4 & jof to mm2  (1) jof to mdb  (2) jof to dump>
//    -mach <machine name as found in [Machines] table of CMDB>
//    -tool <tool setup name>
//    -mat  <material name as found in [Material Inventory] table of CMDB>
//    -pm4  <source pm4 filepath>
//    -jof  <source jof filepath>
//    -mm2  <resulting mm2 filepath>
//    -mdb  <resulting mdb filepath>
//    -out  <part_outline (0) do not create  (1) do create>
//
int ParseCmdLine( int argc, char** argv, CVarList* args )
{
	CReturn status;

	CString msg;
	CString tmp;
	CString name;
	CString value;
	int count, use, debug;

	bool okay = FALSE;

	if (argc == 1)
		return 0;  // Nothing to do, must be running interactively?


	if (argc > 1)
	{
		value = argv[1];
		if (value.Find("//") == 0)
			return 0;  // Command line arguments (from debugger) are commented out.
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get all of the command line arguments as attributes
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = 0;  // the application name
	while (1)
	{
		++count;
		if (count >= argc)
			break;

		name = argv[ count ];

		++count;
		if (count >= argc)
			break;

		value = argv[ count ];

		value.TrimLeft();
		value.TrimRight();

		args->setString( name, value );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Verify that we have the necessary arguments.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	debug = args->getInt( "-d", 0 );
	if ( debug )
	{
		// Enable diagnostic messages.
		CReturn::ErrorLevel( WARN_DIAGNOSTIC );
	}

	if (debug > 0)
	{
		CString msg;
		CVar* var;

		int count = args->countVar();
		status.Diagnostic( " " );
		for (int indx = 0; indx < count; ++indx)
		{
			var = args->getVar( indx );
			msg.Format( "<%s> <%s>", var->getName(), var->getString() );
			status.Diagnostic( msg );
		}
	}

	use = args->getInt( "-use", BAD_USAGE );
	if (use == PM4JOF_TO_MM2)
	{
		CString materialName;
		CString toolSetupName;

		// supplied by the command string.
		materialName = args->getString( "-mat",  EMPTY_STRING );
		if ( materialName.IsEmpty() )
		{
			// Punt and get the value from the registry.
			materialName = CRegister::StringGet(
				CRegister::RootPathV("\\Global"), "MaterialSetup", "" );

			args->setString( "-mat", materialName );
		}

		toolSetupName = args->getString( "-tool",  EMPTY_STRING );
		if ( toolSetupName.IsEmpty() )
		{
			// There are two use cases:
			// 1. PM4 only -- in this case, the "-tool" argument should be a valid
			//    string.  This case assumes you are using an existing tool setup.
			// 2. PM4/JOF -- in this case, the "-tool" argument should be empty.
			//    In this case, the CMDB will be populated with a tool setup
			//    whose name matches the PM4 file name.

			CPath path( args->getString( "-pm4",  EMPTY_STRING ) );
			args->setString( "-tool", path.FileName() );
		}

		okay = ((args->getString( "-mdb",  EMPTY_STRING ).GetLength() > 0) &&
				(args->getString( "-pm4",  EMPTY_STRING ).GetLength() > 0) &&
				(args->getString( "-jof",  EMPTY_STRING ).GetLength() > 0) &&
				(args->getString( "-mm2",  EMPTY_STRING ).GetLength() > 0) &&
				(args->getString( "-mach", EMPTY_STRING ).GetLength() > 0) &&
				(args->getString( "-tool", EMPTY_STRING ).GetLength() > 0) &&
				(args->getString( "-mat",  EMPTY_STRING ).GetLength() > 0) );
	}
	else if (use == JOF_TO_MDB)
	{
		okay = ((args->getString( "-jof", EMPTY_STRING ).GetLength() > 0) &&
				(args->getString( "-mdb", EMPTY_STRING ).GetLength() > 0) );
	}
	else if (use == DEBUG_DUMP || use == INTERACTIVE)
	{
		okay = TRUE;
	}

	if ( !okay )
	{
		MessageBox( NULL,
					"Bad runtime parameters.",
					"Pm4JofConvertor.exe -- Fatal Error",
					(MB_OK|MB_ICONSTOP) );

		return -1;
	}

	return argc;
}
