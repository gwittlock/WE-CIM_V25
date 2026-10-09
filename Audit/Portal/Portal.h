#pragma once

#include "stdafx.h"
#include "Type.h"
#include "Return.h"

class C3dCoord;
class CModel;
class CVm;
class CCommand;

#if (_CI || _NST)
	class CCiModel;
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For all purposes the Portal is a singleton mediator object that serves
// as the interface between the ui and the remainder of the application.
//
// The application is requested to perform operations via string instructions
// that are interpreted by a targeted subsystem.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CPortal
{
public:

	// Portal initialization involves initializing the Java virtual machine.
	// If the virtual machine is not initialized, NC code generation, macro
	// execution and macro objects will be unusable.
	//
	// Wrt the virtual machine initialization, the classPath is critcal to
	// java file compilation and execution.  A known working configuration
	// is:
	//
	//     classPath = "c:\\Bbi\\AdvMach\\Debug\\Bbi.zip; \
	//                  c:\\jdk1.2\\lib\\tools.jar; \
	//                  c:\\jdk1.2\\jre\\lib\\rt.jar";
	//
	// The storagePath is the target directory where the compiled (.class)
	// files reside.  A known working configuration is:
	//
	//     storagePath = "c:\\Bbi\\AdvMach\\Store\\.";
	//
	static CReturn Init( const CString& dllPath,
						const CString& jvmDllFolder,
						const CString& classPath,
						const CString& storagePath,
						bool undoBufferSuppress );

#if (_CI || _NST)

	static CReturn Hookup( 
						LPDISPATCH	docObject,
						LPDISPATCH	cmdObject,
						eCiHookup	which_obj );

#endif

	static CReturn Terminate( void );

	// Given the name of a subdirectory, find any process DLL files,
	// load them, and register their functions with the routing list.
	static CReturn Register( const CString& subdir );

	// Execute a command that is registered with the routing list.
	// Internally, the command string is converted to a singleton
	// CCommand object.  Therefore, any necessary queries via GetInt(),
	// GetReal() and GetString() must be made prior to the next Execute().
	static CReturn Execute( const CString& command );

	// Verify if a command can be routed; first level test only
	static bool		Verify( const CString& command );

	// The parsing of Execute, but for reference only.
	static CReturn Parse( const CString& command );

	// Get the integer value associated with the named attribute.  For
	// example the 'sx' of the command 'Line:sx = 1.2, sy = ...'
	static CReturn GetInt( const CString& name, int* val );

	// Get the real value associated with the named attribute.  For
	// example the 'sx' of the command 'Line:sx = 1.2, sy = ...'
	static CReturn GetReal( const CString& name, double* val );

	// Get the string value associated with the named attribute.  For
	// example the 'sx' of the command 'Line:sx = 1.2, sy = ...'
	static CReturn GetString( const CString& name, char* val, int buflen );

	// Convert between screen and world coordinates.
	static bool ScreenToWorld( C3dCoord* point );

	// From world to local ref an dback
	static bool WorldToRef( C3dCoord* point );
	static bool RefToWorld( C3dCoord* point );

	static bool WorldToScreen( const C3dCoord& world, CPoint* screen );
	static bool ViewToScreen( const C3dCoord& view, CPoint* screen );

	// Obtain (or change) the pointer to the model.
	// DON'T MESS WITH THIS unless you have a strong need, or you'll
	// get *such* a pain.
	static CModel* Model();
	static CModel* Model( CModel* model );	// Sets active model, returns old model

	static CModel* GlobalModel();

	// Introduced for use by PortalDLL::vbModelSwap()
	static void ModelSwap( bool use_file_preview_model );

#if (_CI || _NST)
	static CCiModel* CiModel();
	static CCiModel* CiModel( CCiModel* model );	// Sets active model, returns old model
#endif

	static CReturn Config( CCommand* io_cmd );

	// Command stream record & playback
	static CReturn CmdLog( CCommand* io_cmd );
	static CReturn CmdPlay( CCommand* io_cmd );
	static CReturn CmdStop( CCommand* io_cmd );

	// Profiler tools
	static CReturn Profiling( CCommand* io_cmd );
	static CReturn pDump( CCommand* io_cmd );

	// Error reporting.
	static CReturn ExceptionRecord( const CString& errmsg );

	// Yearrrrgh.  So that we have a way to generate relative paths.
	static CString ExeFolder();

private:

	static void Expand( CString* command );
	static void Expand_core( const CString& key, CString* command );
	static bool CanExecute( const CString& command );

	static CReturn CoreExecute( const CString& cmd );
	static CReturn lowest_level_execute( const CString& cmd );
	static bool undo_command_execute( const CString& cmd );

	static bool IsUndoCommand( const CString& cmd );
};
