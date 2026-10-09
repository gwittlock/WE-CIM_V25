// AdminProcess.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"

#include "Register.h"
#include "Path.h"

#include "Msg.h"
#include "Vm.h"

#include "GeoLine.h"
#include "WmChain.h"
#include "ConvexHull.h"
#include "Model.h"

#include "dbPoint.h"
#include "dbHole.h"
#include "dbLine.h"
#include "dbArc.h"

#include "ViewMgr.h"
#include "Profiler.h"

#include "Portal.h"

#include "AdminProcess.h"

static CStringArray global_javaVarset;

static CProfiler g_admin_profiler;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Sub-Routers

CRouteList CAdminProcessApp::m_adminRouter;
CRouteList CAdminProcessApp::m_seqRouter;
CRouteList CAdminProcessApp::m_rtlRouter;
CRouteList CAdminProcessApp::m_varsysRouter;
CRouteList CAdminProcessApp::m_timerRouter;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" bool PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAdminProcessApp

BEGIN_MESSAGE_MAP(CAdminProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CAdminProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdminProcessApp construction

CAdminProcessApp::CAdminProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAdminProcessApp object

CAdminProcessApp theApp;


CReturn 
CAdminProcessApp::RegisterProcess( 
	CRouteList*	io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_route->addSubrouter( CString("Admin"), &m_adminRouter );

	// ---

	m_adminRouter.addProcess( CString("New"), New );

	m_adminRouter.addProcess( CString("System"), System );
	m_adminRouter.addProcess( CString("User"), User );
	m_adminRouter.addProcess( CString("Prepare"), Prepare );
	m_adminRouter.addProcess( CString("Commit"), Commit );
	m_adminRouter.addProcess( CString("Cancel"), Cancel );
	m_adminRouter.addProcess( CString("Undo"), Undo );
	m_adminRouter.addProcess( CString("Redo"), Redo );

	m_adminRouter.addProcess( CString("Quit"), Quit );

	m_adminRouter.addProcess( CString("GlobalModel"), GlobalModel );

	m_adminRouter.addProcess( CString("IsRegenPending"), IsRegenPending );
	m_adminRouter.addProcess( CString("Regen"), Regen );

	m_adminRouter.addProcess( CString("MacroCompile"), MacroCompile );
	m_adminRouter.addProcess( CString("MacroRun"), MacroRun );

	m_adminRouter.addProcess( CString("TreeInit"), TreeInit );
	m_adminRouter.addProcess( CString("TreeCount"), TreeCount );
	m_adminRouter.addProcess( CString("TreeGet"), TreeGet );

	m_adminRouter.addProcess( "Lookup", Lookup );

	m_adminRouter.addProcess( CString("JavaVarsExtract"), JavaVarsExtract );
	m_adminRouter.addProcess( CString("JavaVarGet"),  JavaVarGet );

	m_adminRouter.addProcess( CString("RegGet"),  RegGet );
	m_adminRouter.addProcess( CString("RegPut"),  RegPut );

	m_adminRouter.addProcess( CString("Config"),  Config );
	m_adminRouter.addProcess( CString("Errors"),  Errors );
	m_adminRouter.addProcess( CString("EWM"),  EWM );

	// RTL commands
	m_adminRouter.addSubrouter( "RTL", &m_rtlRouter );
	m_rtlRouter.addProcess( "Dir",		RtlDir );

	// VarSys commands
	m_adminRouter.addSubrouter( "VarSys", &m_varsysRouter );
	m_varsysRouter.addProcess( "Count",		VarSysCount );
	m_varsysRouter.addProcess( "Flush",		VarSysFlush );
	m_varsysRouter.addProcess( "Name",		VarSysName );
	m_varsysRouter.addProcess( "Del",		VarSysDel );
	m_varsysRouter.addProcess( "StrGet",	VarSysStrGet );
	m_varsysRouter.addProcess( "StrPut",	VarSysStrPut );

	// Timer commands
	m_adminRouter.addSubrouter( "Timer", &m_timerRouter );
	m_timerRouter.addProcess( "Dump", TimersDump );
	m_timerRouter.addProcess( "Create", TimerCreate );
	m_timerRouter.addProcess( "Start", TimerStart );
	m_timerRouter.addProcess( "Stop", TimerStop );

	return CReturn( STATUS_OKAY );
}

// ==================================================================

CReturn 
CAdminProcessApp::Admin( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_adminRouter.Dispatch( io_cmd );
}
/*
CReturn 
CAdminProcessApp::Sequence( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_seqRouter.Dispatch( io_cmd );
}
*/

CReturn 
CAdminProcessApp::RTL( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_rtlRouter.Dispatch( io_cmd );
}

CReturn 
CAdminProcessApp::VarSys( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_varsysRouter.Dispatch( io_cmd );
}

// ==================================================================
// Admin:New:
//
CReturn 
CAdminProcessApp::New( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;

	CModel&	model = io_cmd->getModel();

	model.SelectorStack().Flush();
	model.SelectorStack().Push();

	// NOTE: The following action has the side effect
	// of resetting the undo system.
	status += model.Flush();

	return status;
}


// ==================================================================
// Admin:System:
//
CReturn 
CAdminProcessApp::System( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->getModel().UndoBufferSuppress();

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Admin:User:
//
CReturn 
CAdminProcessApp::User( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->getModel().UndoBufferActivate();

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Admin:Prepare:
//
CReturn 
CAdminProcessApp::Prepare( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->getModel().UndoBufferPrepare();

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Admin:Commit:
//
CReturn 
CAdminProcessApp::Commit( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->getModel().UndoBufferCommit();

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Admin:Cancel:
//
CReturn 
CAdminProcessApp::Cancel( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->getModel().UndoBufferFlush();

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Admin:Undo:
//
CReturn 
CAdminProcessApp::Undo( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->getModel().Undo();

	io_cmd->getViewMgr().Clear();
	io_cmd->getViewMgr().Refresh( true );

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Admin:Redo:
//
CReturn 
CAdminProcessApp::Redo( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_cmd->getModel().Redo();

	io_cmd->getViewMgr().Clear();
	io_cmd->getViewMgr().Refresh( true );

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Admin:Quit: val=%d
//
CReturn 
CAdminProcessApp::Quit( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int val = io_cmd->VarList().getInt( "val", 0 );

	io_cmd->setInt( "quit", val );

	return CReturn( STATUS_OKAY );
}


// ==================================================================
// Admin:GlobalModel:
//
// NOTE:
//   This method reinstates the 'global model' as the active model.
//   This method was introduced to support V16 code generation.  V16
//   code generation copies the entities from the global model to a
//   local model and then makes the local model the active model
//   (this is done so the graphics will reflect the contents of the
//   local model).  Swapping the model inadvertently affects the
//   command system, which causes subsequent failures in code
//   generation (if you attempt to regenerate code without first
//   closing and then reopening the code generation dialog).
//   Therefore, Admin:GlobalModel: should be called immediately
//   prior to CodeGen:Generate:
//
CReturn 
CAdminProcessApp::GlobalModel( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPortal::Model( CPortal::GlobalModel() );

	return CReturn( STATUS_OKAY );
}



// ==================================================================
// Determine whether a 'regeneration' of toolpath (etc.) is pending.
//
// Admin:IsRegenPending:
//
// Output parameters
//
//    name       value  note
//    "pending"  int    non-zero value means true
//
CReturn 
CAdminProcessApp::IsRegenPending( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool pending = io_cmd->getModel().IsRegenPending();

	io_cmd->setInt( "pending", (pending ? 1 : 0) );

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Regenerate toolpath (etc.)
//
// Admin:Regen:
//
// Output parameters -- none
//
CReturn 
CAdminProcessApp::Regen( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CModel& model = io_cmd->getModel();

	CDbEntityList entities;

	model.RegenList( &entities );

	int count = entities.Count();

	if (count > 0)
	{
		CVarList* defaults = model.pDefault();
		CVarList memo = (*defaults);

		model.UndoBufferPrepare();

		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* dbEntity = entities[indx];

			CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( dbEntity );

			if (dbFeature != NULL)
			{
				CString command = CAdminProcessApp::CommandAssemble( (*dbFeature) );

				if (command.GetLength() > 0)
				{
					// Overwrite the model default attributes,
					// causing any entity created after this
					// point to inherit the new model attributes.
					(*defaults) = dbFeature->Attrib();

					// Note:  Unless the command is prefixed by '*',
					// it will be recorded in any activity log.
					status = CPortal::Execute( command );
				}
			}

			if ( !status.IsOk() )
				break;
		}

		model.UndoBufferCommit();
		model.ClearDirty();

		(*defaults) = memo;
	}

	return status;
}

// ==================================================================
// Compile a java macro/code-generator.
//
// Admin:MacroCompile: javafile=%s
//
CReturn 
CAdminProcessApp::MacroCompile( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();
	const CVarList& attribs = io_cmd->VarList();

	CString javaFilePath = attribs.getString( "javafile", "" );

	if ( javaFilePath.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::MacroCompile()" );
	}
	else
	{
		MsgInit();  // Activated to trap any messages from java compiler/vm.

		status = CVm::Compile( javaFilePath );

		MsgTerm();
	}

	return status;
}

// ==================================================================
// Run a macro
//
// Admin:MacroRun: javafile=%s [,compile=%d]
// Compilation defaults to TRUE.
//
CReturn 
CAdminProcessApp::MacroRun( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();
	const CVarList& attribs = io_cmd->VarList();

	CString javaFilePath = attribs.getString( "javafile", "" );
	CString logFilePath = attribs.getString( "log", "" );

	bool compile = (attribs.getInt( "compile", FALSE ) != FALSE);

	if ( javaFilePath.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::MacroRun()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if ( !compile )
	{
		// In case the macro has been touched or has never been compiled ...

#if (_CI || _NST)
		CString storagePath = CRegister::StringGetV( "Java", "CiStoragePath", "<error>" );
#else
		CString storagePath = CRegister::StringGetV( "Java", "StoragePath", "<error>" );
#endif

		CPath path( javaFilePath );
		CString javaPath = path.DriveDirFileName() + ".java";
		CString classPath = storagePath + "\\" + path.FileName() + ".class";

		compile = CPath::IsNewer( javaPath, classPath );
	}

	model.UndoBufferPrepare();
	status = model.MacroRun( javaFilePath, attribs, compile, FALSE );
	model.UndoBufferCommit();

	return status;
}
		
		
// ==================================================================

// Admin:TreeInit:
CReturn
CAdminProcessApp::TreeInit( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CTreeViewSupport& tree = model.Tree();

	tree.Rebuild();

	return status;
}


// Admin:TreeCount:
CReturn
CAdminProcessApp::TreeCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CTreeViewSupport& tree = model.Tree();

	int count = tree.Count();

	io_cmd->setInt( "count", count );
	return CReturn( STATUS_OKAY );
}

// Admin:TreeGet: index = %d
CReturn
CAdminProcessApp::TreeGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CTreeViewSupport& tree = model.Tree();

	int indx;
	status = io_cmd->getInt( "index", &indx );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::TreeGet()" );
		return status;
	}

	if (indx < 0 || indx >= tree.Count())
	{
		CString msg;
		msg.Format( "CAdminProcess::SelectionsCount() bad index" );
		status.Internal( IDS_INTERNAL_ERROR, msg );
		return status;
	}

	CDbEntity* dbEntity = tree[indx];
	ID id = dbEntity->Id();

	io_cmd->setInt( "id", id );

	return status;
}

// Admin:Lookup: name = %s
//	Returns: id=%d
CReturn
CAdminProcessApp::Lookup( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	CString	name;
	status = io_cmd->getString( "name", &name );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::Lookup() name" );
		return status;
	}

	CDbEntity* dbEntity = NULL;
	ID id = -1;
	status += model.EntityFind( name, &dbEntity );
	if (dbEntity)
		id = dbEntity->Id();

	io_cmd->setInt( "id", id );
	return status;
}

		
// ==================================================================

CString 
CAdminProcessApp::CommandAssemble( const CDbFeature& dbFeature )
{
	CString command;
	int indx, count;

	const CVarList& attribs = dbFeature.Attrib();
	
	CString sval = attribs.getString( "function", "" );

	count = sval.GetLength();

	if (count < 1)
	{
		// All features that can be regenerated have a function name.
		return command;
	}

	if (sval[count-1] != ':')
	{
		// All features that can be regenerated have
		// a function name that ends with ':'
		return command;
	}

	command = sval;

	count = attribs.countVar();

	for (indx = 0; indx < count; ++indx)
	{
		CVar* attrib = attribs.getVar( indx );
		const CString& name = attrib->getName();

		if ( name.CompareNoCase( "function" ) )
		{
			command += name;
			command += "=";
			command += attrib->getString();
			command += ",";
		}
	}

	count = command.GetLength();

	if (command[count-1] == ',')
		command.SetAt( (count-1), '\0' );

	return command;
}
		
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Admin:JavaVarsExtract: file = string, class = string
//
//    eg. Admin:JavaVarsExtract: file = "Xilog.java", class = "cg"
//
CReturn 
CAdminProcessApp::JavaVarsExtract( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const int MAXBUF = 256;
	char buf[MAXBUF];
	int indx, len;
	CString javaPath;
	CString className;
	CString str;
	CString prefix;
	CReturn status;

	status  = io_cmd->getString( "file", &javaPath );
	status += io_cmd->getString( "class", &className );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::JavaVarsExtract()" );
		return status;
	}

	FILE* f = fopen( CPath::PathExpand( "$JAVA", javaPath ), "r" );
	if (f == NULL)
	{
		status.Internal( IDS_FILE_READ_ERR, javaPath );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// The BBI convention is that variables communicated via
	// attributes will be declared at the top of the java file
	// as comments of the form '// classname.varname|type|value'
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	global_javaVarset.SetSize( 0, 16 );

	prefix = className + ".";
	while (1)
	{
		if (fgets( buf, MAXBUF, f ) == NULL)
			break;  // Nothing else to read.

		len = strlen( buf );
		if (buf[--len] == '\n')
			buf[len] = '\0';  // Strip the line-feed character.

		str = buf;

		if (str.Find( "public") >= 0 && str.Find( "class" ) >= 0)
			break;  // Don't have to read beyond this point.

		indx = str.Find( prefix );
		if (indx > 0 && str.Find( "|" ) )
		{
			global_javaVarset.Add( str.Mid( indx ) );
		}
		else
		if (str.Left(3).Compare("//*") == 0)
		{
			global_javaVarset.Add( str.Mid( 2 ) );
		}

	}

	fclose( f );

	io_cmd->setInt( "count", global_javaVarset.GetSize() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Admin:JavaVarGet: index = int
CReturn 
CAdminProcessApp::JavaVarGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int index;
	status = io_cmd->getInt( "index", &index );

	if ( !status.IsOk() || index >= global_javaVarset.GetSize() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::JavaVarGet()" );
		return status;
	}

	io_cmd->setString( "data", global_javaVarset.GetAt( index ) );

	return status;
}

// Admin:RegGet: key=%s, subkey=%s [,defval=%s]
CReturn 
CAdminProcessApp::RegGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString key;
	CString subkey;
	CString val;

	status += io_cmd->getString( "key", &key );
	status += io_cmd->getString( "subkey", &subkey );

	val = io_cmd->VarList().getString( "defval", "" );

	if ( status.IsOk() )
	{
		val = CRegister::StringGet( key, subkey, val );
		io_cmd->setString( "val", val );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::RegGet()" );

	return status;
}

// Admin:RegPut: key=%s, subkey=%s, val=%s
CReturn
CAdminProcessApp::RegPut( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString key;
	CString subkey;
	CString val;

	status += io_cmd->getString( "key", &key );
	status += io_cmd->getString( "subkey", &subkey );
	status += io_cmd->getString( "val", &val );

	if ( status.IsOk() )
		CRegister::StringSet( key, subkey, val );
	else
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::RegPut()" );

	return status;
}

CReturn 
CAdminProcessApp::Config( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return ( CPortal::Config( io_cmd ) );
;
}

// Admin:Errors: flags=%d
//    See also ewmconst.h for values.
CReturn CAdminProcessApp::Errors( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int flags = 0;
	CReturn status = io_cmd->getInt( "flags", &flags );
	if ( status.IsOk() )
	{
		EWMRegistryBitsSet( flags );
		EWMBitsInitFromRegistry();
	}

	return status;
}

// Admin:EWM: [open=%b] [close=%b] [silent=%b] [file=%s]
CReturn 
CAdminProcessApp::EWM( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	const CVarList& args = io_cmd->VarList();

	if ( args.getInt( "open", FALSE ) )
	{
		EWMInitialize();
	}
	else if ( args.getInt( "close", FALSE ) )
	{
		EWMTerminate();
	}
	else if ( !args.getString( "file", "" ).IsEmpty() )
	{
		CString file = args.getString( "file", "" );
		if ( !file.IsEmpty() )
			status.LogFile( file );
	}
	else
	{
		CString msg = args.getString( "echo", "" );
		if ( !msg.IsEmpty() )
			status.Portal( msg );  // not the prefered method but good enough for now
	}

	return status;
}

// Admin:RTL:dir: [source=%s] [,result=%s]
CReturn 
CAdminProcessApp::RtlDir( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString source = io_cmd->VarList().getString( "$SOURCE", "" );
	CString result = io_cmd->VarList().getString( "$RESULT", "" );

	CRegister::StringSetV( "Debug", "$SOURCE", source );
	CRegister::StringSetV( "Debug", "$RESULT", result );

	return status;
}

// Admin:VarSys:Count:
CReturn 
CAdminProcessApp::VarSysCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel& model = io_cmd->getModel();
	io_cmd->setInt( "count", model.VarSys().countVar() );
	return CReturn( STATUS_OKAY );
}

// Admin:VarSys:Flush:
CReturn 
CAdminProcessApp::VarSysFlush( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel& model = io_cmd->getModel();
	model.pVarSys()->Reset();
	return CReturn( STATUS_OKAY );
}

// Admin:VarSys:Name: indx=%d
CReturn 
CAdminProcessApp::VarSysName( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CModel& model = io_cmd->getModel();

	int indx = io_cmd->VarList().getInt( "indx", -1 );
	if (indx < 0 || indx >= model.VarSys().countVar())
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::VarSysName()" );
	}
	else
	{
		CVar* var = model.VarSys().getVar( indx );
		io_cmd->setString( "name", var->getName() );
	}

	return status;
}

// Admin:VarSys:Del: name=%s
CReturn 
CAdminProcessApp::VarSysDel( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name = io_cmd->VarList().getString( "name", "" );
	if ( name.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::VarSysDel()" );
	}
	else
	{
		CModel& model = io_cmd->getModel();
		model.pVarSys()->deleteVar( name );
	}

	return status;
}

// Admin:VarSys:StrGet: name=%s [,defval=%s]
// returns string 'val'
CReturn 
CAdminProcessApp::VarSysStrGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CString name = io_cmd->VarList().getString( "name", "" );
	CString defval = io_cmd->VarList().getString( "defval", "" );

	if ( name.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::VarSysStrGet()" );
		io_cmd->setString( "val", defval );
	}
	else
	{
		CModel& model = io_cmd->getModel();
		io_cmd->setString( "val", model.VarSys().getString( name, defval ) );
	}

	return status;
}

// Admin:VarSys:StrPut: name=%s, val=%s
// returns string 'val'
CReturn 
CAdminProcessApp::VarSysStrPut( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CString name = io_cmd->VarList().getString( "name", "" );
	CString val  = io_cmd->VarList().getString( "val", "" );

	if ( name.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::VarSysStrPut()" );
	}
	else
	{
		CModel& model = io_cmd->getModel();
		model.pVarSys()->setString( name, val );
	}

	return status;
}

// Admin:Timer:Dump:
CReturn
CAdminProcessApp::TimersDump( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_admin_profiler.Dump( "CAdminProcessApp::TimersDump()" );
	return STATUS_OKAY;
}

// Admin:Timer:Create:name=%s
// returns 'id' (-1) failed / (0..n) valid id
CReturn
CAdminProcessApp::TimerCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

#if 0
	int id = -1;

	CString name = io_cmd->VarList().getString( "name", "" );

	if ( name.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::TimerCreate() -- no 'name'." );
	}
	else
	{
		id = CReturn::Profile( name );
	}

	io_cmd->setInt( "id", id );
#endif

	return status;
}

// Admin:Timer:Start:id=%d
CReturn
CAdminProcessApp::TimerStart( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name = io_cmd->VarList().getString( "name", "" );

	if ( name.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::TimerStart() -- no 'name'." );
	}
	else
	{
		g_admin_profiler.In( (LPCSTR) name );
	}

	return status;
}

// Admin:Timer:Stop:id=%d
CReturn
CAdminProcessApp::TimerStop( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name = io_cmd->VarList().getString( "name", "" );

	if ( name.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAdminProcessApp::TimerStop() -- no 'name'." );
	}
	else
	{
		g_admin_profiler.Out( (LPCSTR) name );
	}

	return status;
}


