

#define CATCH_EXCEPTIONS 0

#include "stdafx.h"

#include "Register.h"

#include "2dCoord.h"
#include "3dCoord.h"
#include "Return.h"
#include "Type.h"

#include "RouteList.h"
#include "ViewMgr.h"
#include "ProcessMgr.h"
#include "DbLayer.h"
#include "DbWorkplane.h"
#include "SelectorStack.h"
#include "TreeViewSupport.h"
#include "Model.h"
#include "Vm.h"

#include "CmdRecorder.h"

#if (_CI || _NST)
	#include "Civd.h"
	#include "CiModel.h"
	#include "CiSelector.h"
#endif

#include "cmn_resource.h"

#include "Portal.h"

extern void CmdParseTerminate();
extern void CmdLexerTerminate();


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Application performance profiler (at Portal level)
//
#include "Profiler.h"
static CProfiler global_profiler;
static bool global_is_profiling = FALSE;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Supporting Information (singleton objects) necessary to run the system.
//
// NOTE: As these objects are declared on the stack, they may become
// wrt to shutting down the application because they are destroyed when
// the dlls are released(?)  This was the case with global_cimodel,
// which called back into VDraw and caused crashing during shutdown.
//
static CRouteList		global_route_list;
static CProcessMgr		global_process_mgr;
static CViewMgr			global_view_mgr;
static CCommand			global_command;
static CSelectorStack	global_selector_stack;
static CTreeViewSupport	global_treeview_support;
static CModel			global_model;
static CModel*			hot_model;
static CVm*				global_vm = NULL;

#if (_CI || _NST)
	static CCiModel*	global_cimodel = NULL;
	static CCiModel*	hot_cimodel = NULL;
#endif

static CString		DEBUG_KEY;

// Used only for file preview.
static CModel			global_preview_model;
static CSelectorStack	global_preview_ss;
static CTreeViewSupport	global_preview_ts;

static CDbWorkplane*	global_default_work = NULL;
static CDbLayer*		global_default_layer = NULL;

// For recording log files.
static CCmdRecorder*	global_recording = NULL;

static CStdioFile*		global_playing = NULL;
static bool				global_verbose = FALSE;

// To minimize allocations on stack.
static CString			global_msg;

static CString			g_exe_folder;

CGeoRenderer theGeoRenderer;  // TODO: We have to make 'theGeoRenderer' a member of CPortal

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn CPortal::Init(
	const CString& dllPath,
	const CString& jvmDllFolder,
	const CString& classPath,
	const CString& storagePath,
	bool undoBufferSuppress )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CReturn status = CPortal::Register( dllPath );

	int indx = dllPath.ReverseFind('\\');
	if (indx > 0)
		g_exe_folder = dllPath.Left( indx );

	hot_model = &global_model;

#if (_CI || _NST)
	global_cimodel = new CCiModel(true);
	hot_cimodel = global_cimodel;

	CCiSelector::Init();
#endif

	global_model.Init( global_selector_stack, global_treeview_support );

	if ( undoBufferSuppress )
		global_model.UndoBufferSuppress();
	else
		global_model.UndoBufferActivate();

	global_preview_model.Init( global_preview_ss, global_preview_ts );

	if (status.IsOk() && classPath.GetLength() > 2)  // 2 is because of friggin' VB
	{
		// NOTE: We can disable the java virtual machine by not
		// providing a class path.
		//
		// NOTE: In the past, deleting the virtual machine was a
		// nebulous affair.  There were reports that it did not
		// clean up after itself.  We should probably prevent
		// reinitialization of the vm until that aspect of the
		// vm is corrected,

		status = CVm::Init( jvmDllFolder, classPath, storagePath );
	}

	GeoRendererSet( theGeoRenderer );

	return status;
}

#if (_CI || _NST)

	CReturn CPortal::Hookup(
		LPDISPATCH	docObject,
		LPDISPATCH	cmdObject,
		eCiHookup	which_obj )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CReturn status;

		CCivd::Init( docObject, cmdObject, which_obj );

		if (which_obj == CI_MAIN)
			global_cimodel->Init();

		return (STATUS_OKAY);
	}

#endif

CReturn CPortal::Terminate()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// global_route_list.Dump( "c:\\cmd.txt" );

#if (_CI || _NST)
	CCiSelector::Terminate();
#endif

	global_view_mgr.Reset();
	global_command.Reset();
	global_selector_stack.Flush();
	global_model.Flush();

	global_preview_model.Flush();

	// Historical remnant --
	// TODO: Previously, the call to global_process_mgr.Reset() appeared before
	// the call to global_model.Flush().  It has since been moved to avoid a bug
	// where an entity (or CVarList object) is twice deleted (causing a crash).
	// Said bug is somehow introduced via CLead::Manual() but is very difficult
	// to isolate.
	//    global_process_mgr.Reset();

#if (_CI || _NST)
	delete global_cimodel;
	global_cimodel = NULL;
#endif

	CVm::Term();

#if (_CI || _NST)
	CCivd::Terminate();
#endif

#if 0
	CReturn::EWM_Handler_Terminate();
#endif

	// This call unloads all of the dlls (via FreeLibrary).
	// As each library is free'd, it destroys its own routelist.
	global_process_mgr.Reset();

	// As of V20, unloading the process dlls has beed defered to ~CProcMgr()
	// because memory-leak information about the source-file is lost when:
	//   "The memory block was allocated in a DLL that was unloaded
	//    prior to the _CrtMemDumpAllObjectsSince() call."
	//
	// The lost information manifests as "#File Error#" in the Output window
	// when the memory leaks are being dumped.
	//
	// Note, the file information is made available by including the MSVC
	// defined symbol _CRTDBG_MAP_ALLOC (and a couple of headers) into stdafx.h
	//
	// Note, _CrtDumpMemoryLeaks() is typically automatically called when
	// terminating an application. In turn, if CrtDumpMemoryLeaks() detects
	// any leaks, it calls _CrtMemDumpAllObjectsSince().
#if BEFORE_V20
	global_route_list.Reset();
#endif

	// v20. Clean-up memory used by parser.
	CmdParseTerminate();
	CmdLexerTerminate();

	return CReturn( STATUS_OKAY );
}

CString CPortal::ExeFolder()
{
	return g_exe_folder;
}


CReturn CPortal::Register( const CString& dllPath )
{
	CReturn	ret;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	global_route_list.Reset();

	ret = global_process_mgr.Register( dllPath, &global_route_list );
	ret += global_view_mgr.Register( &global_route_list );

	//
	// Register meta-commands, for logging command stream
	//
	global_route_list.addProcess( "CmdLog", CmdLog );
	global_route_list.addProcess( "CmdPlay", CmdPlay );
	global_route_list.addProcess( "CmdStop", CmdStop );

	global_route_list.addProcess( "Profiling", Profiling );
	global_route_list.addProcess( "Dump", pDump );

	return ret;
}

CReturn CPortal::Execute( const CString& command )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CString	cmd;
	CString	prefix;
	bool	okay;

	if (command.Find( "//" ) == 0)
		return STATUS_OKAY;  // The command is commented out.

	if (command.GetLength() < 2)
	{
		status.Internal( IDS_CMD_EMPTY_COMMAND );
		return status;
	}

	if ( global_is_profiling )
		global_profiler.ResetAll();

	if (command[0] == '*')
	{
		cmd = command.Mid( 1 );
		okay = global_verbose;
	}
	else if (command[0] == '!')
	{
		cmd = command.Mid( 1 );
		okay = EWMUiAllow();
	}
	else
	{
		cmd = command;
		okay = TRUE;
	}

	if (okay && global_recording)
	{
		global_recording->Record( cmd );
		global_recording->Record( "\n" );
	}

	prefix = cmd.Left(4);
	if ((prefix.CompareNoCase("view") == 0) || (prefix.CompareNoCase("attr") == 0))
	{
		// Argh! Myriad commands.
	}
	else
	{
		// A convenient breakpoint location that avoids view commands.
		int foo = 0;
	}

	// (2) Portal Commands (as seen in Admin/Settings).
	// if (CReturn::Debug() >= 2)
	{
		// Echo most low-level Portal commands to the EWM.
#if 0
	#if (_CI || _NST)
			if (cmd.Find("Db:") == 0)
			{
				// ie. suppress echo of simple things like "Db:IntGet:"
				okay = (cmd.Find("QueryExecute:") > 0);
			}
	#else
			if ( okay )
			{
				// Suppress display of mouse movement commands because
				// there are so many that it clutters the output.
				prefix = cmd.Left(11);
				okay = (prefix.CompareNoCase("view:mouse:") != 0);
			}
	#endif
			if (okay)
			{
				CReturn status;
				status.Portal( cmd );
			}
#else
		if ( okay )
		{
			// Suppress display of mouse movement commands because
			// there are so many that it clutters the output.
			prefix = cmd.Left(11);
			okay = (prefix.CompareNoCase("view:mouse:") != 0);
		}
#endif
		if (okay)
		{
			CReturn status;
			status.Portal( cmd );
		}
	}

	return ( CoreExecute( cmd ) );
}

CReturn CPortal::CoreExecute( const CString& cmd )
{
	CReturn	status;

	if ( DEBUG_KEY.IsEmpty() )
		DEBUG_KEY = CRegister::RootPathV() + "\\Debug";

	// TODO: Move this call to PortalInit() (?)
	bool exceptions = CRegister::BoolGet( DEBUG_KEY, "Exceptions", true );

	if (exceptions)
	{
		// This should be the active branch in shipped product!
		try
		{
			status = lowest_level_execute( cmd );
		}
		catch (...)  // catches all errors.
		{
			// NOTE: Using "catch(CException* e)" gives blue-screen-of-death.

			// e->ReportError();
			// e->Delete();
			global_msg.Format( "Portal::Execute(\"%s\")\n", cmd );
			status = ExceptionRecord( global_msg );
		}
	}
	else
	{
		status = lowest_level_execute( cmd );
	}

	return status;
}

CReturn CPortal::lowest_level_execute( const CString& cmd )
{
	CReturn	status;

	// Explicitly process any "admin:prepare:" and "admin:commit:" commands.
	if ( !undo_command_execute( cmd ) )
	{
		// Barring that, route the command through the command system.
		status = global_command.setCommand( cmd, hot_model, &global_view_mgr );

		if ( status.IsOk() )
			status = global_route_list.Dispatch( &global_command );
	}

	if (global_recording && global_is_profiling)
	{
		global_msg.Format( "// %7.4f %s\n", global_profiler.Delta(0), cmd );
		global_recording->Record( global_msg );
	}

	return status;
}

bool CPortal::undo_command_execute( const CString& cmd )
{
	bool is_undo_command = false;

	if (cmd[0] == 'a')
	{
		// 2007.04.21 (PE) -- Implemented fail safe measures for the
		// "undo system" because we received several reports of it
		// failing (unpredictably) to work. This has been a very
		// difficult problem to reproduce (see also modAdmin.bas).
		//
		// Bottom line: We now explicitly handle "admin:prepare:" and
		// "admin:commit:" instead of routing the commands through
		// the command system (which failed for some unknown reason).
		//
		// PREREQUISITE: For the sake of efficiency, these admin
		// commands *must* be lowercase and have no whitespace.
		//
		// ALSO NOTE: 1) hot_model had better be the 'actual model'
		// and 2) these admin commands had better be executed only
		// when the hot_model is the actual model.
		if (cmd.Compare("admin:prepare:") == 0)
		{
			is_undo_command = true;
			hot_model->UndoBufferPrepare();
		}
		else if (cmd.Compare("admin:commit:") == 0)
		{
			is_undo_command = true;
			hot_model->UndoBufferCommit();
		}
	}

	return is_undo_command;
}

bool CPortal::Verify( const CString& command )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	cmd;

	if ( (command.GetLength() > 1)
		&& (command[0] == '*') )
	{
		cmd = command.Mid( 1 );
	}
	else
	{
		cmd = command;
	}

	CReturn status = global_command.setCommand( cmd, hot_model, &global_view_mgr );

	if ( status.IsOk() )
		return global_route_list.Verify( &global_command );

	return FALSE;
}


CReturn CPortal::Parse( const CString& command )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	cmd;

	if ( (command.GetLength() > 1)
		&& (command[0] == '*') )
	{
		cmd = command.Mid( 1 );
	}
	else
	{
		cmd = command;
	}

	CReturn status = global_command.setCommand( cmd, hot_model, &global_view_mgr );

	return status;
}

CReturn CPortal::GetInt( const CString& name, int* val )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	try
	{
		status = global_command.getInt( name, val );
	}
	catch (...)  // catches all errors.
	{
		global_msg.Format( "Portal::GetInt(\"%s\")\n", name );
		status = ExceptionRecord( global_msg );
	}

	return status;
}

CReturn CPortal::GetReal( const CString& name, double* val )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	try
	{
		status = global_command.getReal( name, val );
	}
	catch (...)  // catches all errors.
	{
		global_msg.Format( "Portal::GetReal(\"%s\")\n", name );
		status = ExceptionRecord( global_msg );
	}

	return status;
}

CReturn CPortal::GetString( const CString& name, char* val, int buflen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CString tmp;
	
	try
	{
		status = global_command.getString( name, &tmp );

		if ( status.IsOk() )
			strncpy( val, tmp, buflen );
	}
	catch (...)  // catches all errors.
	{
		global_msg.Format( "Portal::GetString(\"%s\")\n", name );
		status = ExceptionRecord( global_msg );
	}

	return status;
}

bool CPortal::ScreenToWorld( C3dCoord* point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint screen( (int) point->X(), (int) point->Y() );
	C3dCoord world;

	global_view_mgr.mapScreenToWorld( screen, &world ); 

	(*point) = world;
	point->Z(0);

	return TRUE;
}

bool CPortal::WorldToRef( C3dCoord* point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3dCoord ref;

	global_view_mgr.mapWorldToRef( *point, &ref ); 

	(*point) = ref;

	return TRUE;
}

bool CPortal::RefToWorld( C3dCoord* point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3dCoord world;

	global_view_mgr.mapRefToWorld( *point, &world ); 

	(*point) = world;

	return TRUE;
}

bool CPortal::WorldToScreen( const C3dCoord& world, CPoint* screen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	global_view_mgr.mapWorldToScreen( world, screen ); 

	return TRUE;
}

bool CPortal::ViewToScreen( const C3dCoord& view, CPoint* screen )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	global_view_mgr.mapViewToScreen( view, screen ); 

	return TRUE;
}



// ==================================================================
//	oh, the pain
//
//	Put here so I can run macros on non-central models.
//
CModel* CPortal::Model()
{
	return hot_model;
}

CModel* CPortal::Model( CModel*	model )
{
	CReturn report;
	CString msg;

	CModel* ret_model = hot_model;

	msg.Format( "CPortal::Model( %x <-- %x )", hot_model, model );
	report.Diagnostic( msg );

	hot_model = model;

	return ret_model;
}

CModel* CPortal::GlobalModel()
{
	return &global_model;
}

void CPortal::ModelSwap( bool use_file_preview_model )
{
	CReturn report;
	CString msg;

	if (use_file_preview_model)
	{
		//msg.Format( "CPortal::ModelSwap( %x <-- %x )", hot_model, global_preview_model );
		msg.Format( "CPortal::ModelSwap(  )" );
		report.Diagnostic( msg );
		hot_model = &global_preview_model;
	}
	else
	{
		global_preview_model.Flush();
		msg.Format( "CPortal::ModelSwap( )" );
		//msg.Format( "CPortal::ModelSwap( %x <-- %x )", hot_model, global_preview_model );
		report.Diagnostic( msg );
		hot_model = &global_model;
	}
}

#if (_CI || _NST)
	CCiModel*
	CPortal::CiModel()
	{
		return hot_cimodel;
	}

	CCiModel*
	CPortal::CiModel(
		CCiModel*	model )
	{
		CCiModel* ret_model = hot_cimodel;

		hot_cimodel = model;

		return ret_model;
	}
#endif

CReturn CPortal::Config( CCommand* io_cmd )
{
	CReturn	status;

	global_verbose = (CRegister::Debug("Verbose") != 0);

	return status;
}

// ==================================================================
// CmdLog: file=%s
//
CReturn CPortal::CmdLog( CCommand* io_cmd )
{
	CReturn	ret;

	CmdStop( io_cmd );

	CString filename = io_cmd->VarList().getString( "file", "" );

	if (filename.IsEmpty()
		|| global_recording
		|| global_playing )
	{
		ret.setStatus( STATUS_ERROR );
	}

	if (ret.isOkay())
	{
		global_recording = new CCmdRecorder();
		global_recording->Init( filename, true );

		if (!global_recording)
			ret.setStatus( STATUS_ERROR );
	}

	return ret;
}

// ==================================================================
// CmdPlay:file=%s
//
CReturn CPortal::CmdPlay( CCommand* io_cmd )
{
	CReturn	ret;

	CmdStop( io_cmd );

	CString filename = io_cmd->VarList().getString( "file", "" );
	int delay = io_cmd->VarList().getInt( "delay", 0 );

	if ( filename.IsEmpty() )
	{
		ret.setStatus( STATUS_ERROR );
	}

	if (ret.isOkay())
	{
		global_playing = new CStdioFile( filename, 
													CFile::modeRead 
													| CFile::typeText );
		if (!global_playing)
			ret.setStatus( STATUS_ERROR );
	}

	//
	// Perform playback now...
	//
	if (ret.isOkay())
	{
		CString	command;

		while (	global_playing && global_playing->ReadString( command ))
		{
			if ( CanExecute( command ) )
			{
				Expand( &command );

				Execute( command );

				if (delay)
					Sleep( delay );
			}
		}
	}

	CmdStop( io_cmd );

	return ret;
}

// ==================================================================
// CmdStop:
//
CReturn CPortal::CmdStop( CCommand* io_cmd )
{
	CReturn	ret;

	if ( !global_recording && !global_playing )
	{
		ret.setStatus( STATUS_ERROR );
	}

	if (ret.isOkay())
	{
		if (global_playing)
		{
			global_playing->Close();
			delete global_playing;
			global_playing = NULL;
		}

		if (global_recording)
		{
			global_recording->Terminate();

			delete global_recording;
			global_recording= NULL;
		}
	}

	return ret;
}

// ==================================================================
// Profiling: state=%d
CReturn CPortal::Profiling( CCommand* io_cmd )
{
	global_is_profiling = (io_cmd->VarList().getInt( "state", FALSE ) != 0);
	return CReturn( STATUS_OKAY );
}

// ==================================================================

CReturn CPortal::pDump( CCommand* io_cmd )
{
	CReturn ret;

	if ( global_is_profiling )
		global_profiler.Dump( "CPortal::pDump()" );

	return ret;
}

CReturn CPortal::ExceptionRecord( const CString& errmsg )
{
	CReturn	status;

	if (global_recording)
	{
		global_recording->Record( errmsg );
	}

	status.Exception( errmsg );

	return status;
}

void CPortal::Expand( CString* command )
{
	Expand_core( "$SOURCE", command );
	Expand_core( "$RESULT", command );
}

void CPortal::Expand_core( const CString& key, CString* command )
{
	int indx = command->Find( key );
	if ((indx >= 0) && (command->GetAt(indx+key.GetLength()) != '='))
	{
		CString path = CRegister::StringGetV( "Debug", key, "" );
		if ( !path.IsEmpty() )
			command->Replace( key, path );
	}
}

bool CPortal::CanExecute( const CString& command )
{
	if ( command.IsEmpty() )
		return false;

	return (command.Find("//") != 0);
}
