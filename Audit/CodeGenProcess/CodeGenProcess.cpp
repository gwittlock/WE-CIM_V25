
#include <sstream>

#include "stdafx.h"

#include "MathConst.h"
#include "CommonFlags.h"
#include "StringConst.h"
#include "cmn_resource.h"

#include "Register.h"
#include "Path.h"
#include "Msg.h"
#include "ErrSys.h"
#include "OutSys.h"

#include "DaoDB.h"
#include "DaoQuery.h"
#include "DbAllEntities.h"
#include "DbIterator.h"
#include "EntityCopier.h"
#include "MM2.h"

#include "WorkPkg.h"

#include "Model.h"
#include "ModelUtil.h"
#include "ModelClfile.h"
#include "Weng_CodeGen_Clfile.h"

#include "Nibbler.h"
#include "ToolSequencer.h"

#include "CodeGeoXref.h"
#include "CodeGenOH.h"
#include "CodeUtil.h"

#include "ViewMgr.h"
#include "Vm.h"

#include "Portal.h"
#include "ModelText.h"
#include "LabelDbGenerator.h"

#include "ContHier.h"
#include "CodeGenProcess.h"

#define BEFORE	0
#define AFTER	1

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// The cross reference between database entities and NC code.
// This object must be instantiated via CodeGeoXrefCreate()
// and deleted using CodeGeoXrefDestroy().  If the object
// exists when a call is made to CodeGen(), the output of
// the code generator will be catured by this object.
static CCodeGeoXref* g_cgXref = NULL;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Sub-Routers

CRouteList CCodeGenProcessApp::m_codegenRouter;

CGeoPoly* CCodeGenProcessApp::m_door_poly = NULL;
bool CCodeGenProcessApp::m_can_draw_handles = TRUE;

C3dCoord NO_DELTA(0,0,0);

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Intermediate copy of model.
static CModel			g_cgModel;
static CSelectorStack	g_cgModel_SS;
static CTreeViewSupport	g_cgModel_TS;
static bool				g_cgModel_init = true;

static CDbTool*			g_cgStock;
static CDbWorkplane*	g_cgTop;

// For display of rapid traversals.
static CDbLine*			g_cgLine;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool UsingCodeViewer()
{
	return (g_cgXref != NULL);
}

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
// CCodeGenProcessApp

BEGIN_MESSAGE_MAP(CCodeGenProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CCodeGenProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeGenProcessApp construction

CCodeGenProcessApp::CCodeGenProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCodeGenProcessApp object

CCodeGenProcessApp theApp;


CReturn 
CCodeGenProcessApp::RegisterProcess( 
	CRouteList*	io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_route->addSubrouter( CString("CodeGen"), &m_codegenRouter );

	m_codegenRouter.addProcess( CString("XrefCreate"),  XrefCreate );
	m_codegenRouter.addProcess( CString("XrefDestroy"), XrefDestroy );
	m_codegenRouter.addProcess( CString("XrefCount"),   XrefCount );
	m_codegenRouter.addProcess( CString("XrefFetch"),   XrefFetch );
	m_codegenRouter.addProcess( CString("XrefRecNo"),   XrefRecNo );

	m_codegenRouter.addProcess( CString("Highlight"),   Highlight );
	m_codegenRouter.addProcess( CString("Generate"),    Generate );
	m_codegenRouter.addProcess( CString("Optimize"),	Optimize );

	m_codegenRouter.addProcess( CString("WorkZonesVerify"),	WorkZonesVerify );
	m_codegenRouter.addProcess( CString("ShowRapid"),	ShowRapid );

	return CReturn( STATUS_OKAY );
}

// ==================================================================

CReturn 
CCodeGenProcessApp::CodeGen( CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_codegenRouter.Dispatch( io_cmd );
}

// ==================================================================
// Instantiate the g_cgXref object that is used by
// the graphics/text editor to capture code generator output.
//
// CodeGen:XrefCreate:
//
CReturn 
CCodeGenProcessApp::XrefCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	// NOTE: If the cross references is already instantiated,
	// it will be flushed by the output handler before use.

	if (g_cgXref == NULL)
		g_cgXref = new CCodeGeoXref();

	// Prevent instance handles (and such) from being drawn.
	m_can_draw_handles = CDbEntity::CanDrawHandles();
	CDbEntity::CanDrawHandles( FALSE );

	delete m_door_poly;
	m_door_poly = NULL;

	return status;
}

// ==================================================================
// Destroy the g_cgXref object that is used by
// the graphics/text editor to capture code generator output.
//
// CodeGen:XrefDestroy:
//

CReturn 
CCodeGenProcessApp::XrefDestroy( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	if (g_cgXref != NULL)
	{
		g_cgXref->Flush();

		delete g_cgXref;
		g_cgXref = NULL;

		g_cgModel.ActivePattern( NULL );

		CPortal::GlobalModel()->ActivePattern( NULL );
		CPortal::Model( CPortal::GlobalModel() );

		// Restore the state recorded by XrefCreate().
		CDbEntity::CanDrawHandles( m_can_draw_handles );

		// 2005.09.18 (PE) -- This resets the view manager to use
		// the global model when the code viewer dialog is closed.
		// See also CCodeGenProcessApp::Highlight().
		io_cmd->getViewMgr().ActiveView()->ModelSet( (*CPortal::GlobalModel()) );
	}

	delete m_door_poly;
	m_door_poly = NULL;

	return status;
}

// ==================================================================
// CodeGen:XrefCount:
// Returns count = %d
//
CReturn 
CCodeGenProcessApp::XrefCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	io_cmd->setInt( "count", ((g_cgXref == NULL) ? 0 : g_cgXref->Count()) );

	return status;
}

// ==================================================================
// Fetch a cross reference record.
//
// CodeGen:XrefFetch: recno = %d
// Returns id = %d, text = %s
// Output parameters
//
CReturn 
CCodeGenProcessApp::XrefFetch( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	int recno;

	if (g_cgXref == NULL)
	{
		EWMInternal( "ERROR: Must first call XrefCreate()" );
		return status;
	}

	recno = io_cmd->VarList().getInt( "recno", -1 );

	if ( g_cgXref->Seek( recno ) )
	{
		io_cmd->setInt( "id", g_cgXref->Id() );
		io_cmd->setString( "text", g_cgXref->Text() );
	}
	else
	{
		EWMInternal( "ERROR: CCodeGenProcessApp::XrefFetch() index out of bounds" );
	}

	return status;
}

// ==================================================================
// Obtain the record number of the first record
// associated with the given entity id.
//
// CodeGen:XrefRecNo: id = %d
// Returns recno = %d -- valid entity ids are greater-than-zero
//
CReturn 
CCodeGenProcessApp::XrefRecNo( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	int id;

	if (g_cgXref == NULL)
	{
		EWMInternal( "ERROR: Must first call XrefCreate()" );
		return status;
	}

	id = io_cmd->VarList().getInt( "id", 0 );

	io_cmd->setInt( "recno", ((id > 0) ? g_cgXref->RecNo( id ) : -1) );

	return status;
}

// ==================================================================
// CodeGen:Highlight: id = %d
//
CReturn 
CCodeGenProcessApp::Highlight( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	C3dCoord		ps;
	C3dCoord		pe;
	const CCodeGeoRec*	recA;
	const CCodeGeoRec*	recB;
	CDbEntity*		dbEntityB;
	int				recno, skip;

	if (g_cgXref == NULL)
	{
		EWMInternal( "ERROR: Must first call XrefCreate()" );
		return status;
	}

	recno = io_cmd->VarList().getInt( "rec", 0 );
	if (recno > 0)
	{
		CViewMgr& vmgr = io_cmd->getViewMgr();
		vmgr.ActiveView()->ClearEnable( false );
		vmgr.ActiveView()->ModelShowEnable( false );

		recA = g_cgXref->GetAt( recno - 1 );
		recB = g_cgXref->GetAt( recno );

		skip = FALSE;

		// Get any sentinel (set by CCodeGeoXref::Append()).
		const CGeoElem* elem = recB->GeoElemGet();
		if (elem != NULL)
			skip = elem->IntGet( "skip", FALSE );

		DropDoorErase( io_cmd );

		if ( !skip )
		{
			// Conditionally display the rapid traversal.
			ERapidType rapid = CanDisplayRapid( recA, recB, &ps, &pe );
			if ( rapid )
			{
				C3dCoord ws, we;
				
				// POTENTIAL PROBLEM: does this record necessarily have an entity?
				CDbTool* dbTool = recB->Entity()->Tool();
				int color = dbTool->ColorGet( DCOLOR_RED );

				g_cgLine->Init( g_cgStock, g_cgTop, ps, pe );
				g_cgLine->Flags( g_cgLine->Flags() | DBSHOWPATH );

				//io_cmd->getViewMgr().ActiveView()->Refresh( g_cgLine->Id(), FALSE );

				vmgr.mapRefToWorld( ps, &ws );
				vmgr.mapRefToWorld( pe, &we );
				vmgr.ActiveView()->RapidLine( ws, we, color );
			}

			// Display the entity if there is one (unless, of course, we
			// were told otherwise by the code generator having generated
			// an explicit rapid move for something like clamp avoidance).
			dbEntityB = ((rapid == RAPID_EXPLICIT) ? NULL : (CDbEntity*) recB->Entity());
			if (dbEntityB != NULL)
			{
				CDbTool* dbTool = recB->Entity()->Tool();
				int color = dbTool->ColorGet( DCOLOR_RED );
				// 2005.09.18 (PE) -- The model used by view manager was

				// not synchronized with the xref. As such, entities would
				// not highlight when you selected a block of code, or when
				// you used the Step button.
				//
				// See also CCodeGenProcessApp::XrefDestroy(). Said method
				// resets the view manager to use the global model when
				// the code viewer dialog is closed.
				vmgr.ActiveView()->ModelSet( g_cgModel );

				dbEntityB->Flags( dbEntityB->Flags() | DBSHOWPATH );
				if (dbEntityB->Type() == DBCOMMAND)
				{
					// TODO: Leave permanently highlighted like other entities.
					vmgr.ActiveView()->Highlight( dbEntityB->Id() );
				}
				else
				{
					if (dbEntityB->Type() == DBHOLE)
					{
						dbEntityB->FilledFlag( vmgr.ActiveView()->Fill() );
					}

					vmgr.ActiveView()->Refresh( dbEntityB->Id(), FALSE );
				}
			}

			if (elem != NULL)
			{
				int door = elem->IntGet( "door", FALSE );
				if ( door )
					DropDoorDraw( elem, io_cmd );
			}
		}

		vmgr.ActiveView()->ClearEnable( true );
		vmgr.ActiveView()->ModelShowEnable( true );
	}
	else
	{
		// EWMInternal( "ERROR: CCodeGenProcessApp::Highlight() bad rec#" );
	}

	return status;
}
		
// ==================================================================
// Generate NC code.
//
// CodeGen:Generate: cgfile = %s, ncfile = %s [, compile = %d]
//
// Additional command parameters: ccs=%b, pdb=%s, label_db=%s
//    ccs -- controls code branching
//    pdb -- when ccs is true, represents the fully qualified path
//      to a text file containing a list of pdb files to process.
//    label_db -- independent if ccs, represents the fully qualified
//      path to the database that captures the "Label Result" table.
//
// Notes
//
//    The output can be captured by g_cgXref if the
//    XrefCreate command is executed prior to executing
//    the CodeGen command.  The g_cgXref object is
//    used by the graphic/text editor.
//
//    When coding subroutines and/or multiple sheets, the "subs"
//    and/or "sheets" attributes are set in the model header.
//    This information is used by the Portal command
//    File:ExportMM2: to prevent overwriting existing models.
//    This is necessary because in V15, the model is "abused"
//    during the coding process of sheets and subroutines.
//
CReturn CCodeGenProcessApp::Generate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	if (0)
	{
		// For debugging.
		// So that you don't have to have a CCS FAB environment to
		// test creating/updating the pdb's "Label Result" table.
		// Normally, the "label_db" parameter is part of the command.

		io_cmd->setString( "pdb", "C:\\_tmp\\_gary\\pdblist.txt" );
		io_cmd->setString( "label_db", "c:\\_tmp\\labels.mdb" );
	}

	// "ccs" should be non-zero IFF ccs-fab is coding to a file.
	int is_ccs_fab = io_cmd->VarList().getInt( "ccs", FALSE );
	int is_smc_fab = io_cmd->VarList().getInt( "smc", FALSE );
	if ( is_ccs_fab )
		status = CcsFabGenerate( io_cmd );
	else if ( is_smc_fab )
		status = SmcCodeGenerate( io_cmd );
	else
		status = StandardGenerate( io_cmd );

	return status;
}

CReturn CCodeGenProcessApp::StandardGenerate( CCommand* io_cmd )
{
	CReturn			status;
	CModelClfile	clfile;
	CLabelDbGenerator	labelDb;
	CSeqRules		seqRules;
	CString			cgFilePath;
	CString			ncFilePath;
	CString			pdbPath;
	CString			labelDbPath;
	CString			cgInHeader;
	CDbPattern*		sheet;
	bool			multiSheet = FALSE;
	bool			part_order;
	HCURSOR			hCursor;
	double			gap_tol;

	CModel& model = io_cmd->getModel();

	EWMInternal( "Entered CCodeGenProcessApp::StandardGenerate()" );

	cgFilePath = io_cmd->VarList().getString( "cgfile", "" );
	ncFilePath = io_cmd->VarList().getString( "ncfile", "" );

    labelDbPath = io_cmd->VarList().getString( "label_db", "" );
	part_order = (!labelDbPath.IsEmpty() && (g_cgXref == NULL));
	if ( part_order )
		labelDb.Init( labelDbPath );

	if ( cgFilePath.IsEmpty() )
		status.Internal( IDS_CREATE_PARAM_MISSING );

	// Check for the existence of at least one parameter.  This insures
	// the model was properly prepared by recording the coding parameters
	// in the model header.  In particular, we should be able to recall
	// the coding parameters last used with this model.
	cgInHeader = model.Header().getString( "cgfile", "" );
	if ( cgInHeader.IsEmpty() )
	{
		EWMInternal( "ERROR: \"cgfile\" attribute is missing from model header (or empty)" );
		return status;
	}

	// Introduced for Cequent Towing, affects behavior of
	// CShortestPath::OptimizePath().  See comments there.
	int secret_incantation = model.Header().getInt( "cg.group_hits", -1 );
	if (secret_incantation >= 0)
	{
		CRegister::IntSetV( "CodeGeneration", "group_hits", secret_incantation );
	}

	if ( g_cgModel_init )
	{
		g_cgModel.Init( g_cgModel_SS, g_cgModel_TS );
		g_cgModel_init = false;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	pdbPath = io_cmd->VarList().getString( "pdb", "" );
	
	if ( multiSheet )  // needed for testing until UI is completed.
	{
		pdbPath = CRegister::StringGetV( "Preferences\\Debugging", "pdbpath", "" );
	}

	g_cgModel.UndoBufferSuppress();
	g_cgModel.Flush();

	CodingParametersCopy( io_cmd->VarList(), CPortal::GlobalModel() );

	(*(g_cgModel.pHeader())) = CPortal::GlobalModel()->Header();

	if ( pdbPath.IsEmpty() )
	{
		// We are processing a single in-memory sheet model.
		sheet = ModelCopy( CPortal::GlobalModel(), &g_cgModel, 1, TRUE );
		sheet->StringSet( STR_TYPE, "_main" );
	}
	else
	{
		// We are processing a multiple sheet model.
		sheet = FilesMerge( pdbPath, &g_cgModel );
		sheet->StringSet( STR_TYPE, "_main" );
	}

	if ( g_cgModel.Header().getInt( "Ind_Hits", FALSE ) )
		CNibbler::Nibble( &g_cgModel );

	// For debugging.
	if (0)
	{
		CModelText text;
		text.Enable( true );
		status = text.Dump( "c:\\_tmp\\model.txt", g_cgModel );
	}

	// Allow message boxes to be issued by the code generator. 
	ErrSysInit();
	MsgInit();

	if ( status.IsOk() )
		status = ConditionalCompile( cgFilePath );

	if ( status.IsOk() )
	{
		CCodeGenOH outputHandler;

        if ( UsingCodeViewer() )
		{
			g_cgModel.ActivePattern( sheet );
			CodeViewerInit( io_cmd, &clfile );
		}

		// 2009.03.14 (PE) -- BOMAG was having problems with containment
		// parity. Increasing the gap_tol during nesting caused problems.
		// After some discussion with Gary, we decided to use a "special"
		// user-defined cg variable to solve the problem (because it is
		// a solution that requires minimal effort).
		gap_tol = g_cgModel.Header().getReal( "cg.gap_tol", SMALL );
		if (gap_tol <= SMALL)
		{
			// 2008.03.05 (PE) -- The gap tolerance is used by code generation
			// to identify profiles that are 'logically closed'. In turn, this
			// helps avoid containment parity issues.
			//   See also CSheet::Save() & CContHier::PolyFromExplicitProfile()
			gap_tol = g_cgModel.Header().getReal( "code_gap", SMALL );
		}
		CContHier::GapTolSet( gap_tol );

		// Generate the clfile using the given sequencing options.
		seqRules.Init( &g_cgModel, io_cmd->VarList() );

		clfile.RecordPartOrder( part_order );
		clfile.Init( &seqRules );

		// Create the output handler for managing code generator output.
		outputHandler.Init( ncFilePath, g_cgXref );

		OutSysInit( &outputHandler );

		if ( status.IsOk() )
		{
			// Set the active clfile and generate the NC code.
			WENG_ClfileInit( &clfile );

			if (g_cgXref == NULL)
			{
				// ie. exporting to file
				// We must do this.  Otherwise, wacky things happen.
				CPortal::Model( &g_cgModel );
			}

			// clfile.Dump("c:\\_Weng\\V16\\AdvMach\\Data\\clfile.txt");
			std::stringstream ss;
			ss << "Executing <" << (LPCSTR) cgFilePath << ">";
			EWMInternal( ss.str().c_str() );
			ss.str("");

			status = CVm::Execute( cgFilePath, FALSE );

			ss << "Execution status: " << status.getStatus();
			EWMInternal( ss.str().c_str() );
			ss.str("");

			if (g_cgXref == NULL)
			{
				CPortal::Model( CPortal::GlobalModel() );
			}

			WENG_ClfileTerm();

			if ( part_order )
			{
				CString pdbPath = g_cgModel.Header().getString( "pdb_file", "" );
				labelDb.LabelTableUpdate( pdbPath, -1, clfile );
			}

			ModelDiffsCopy( g_cgModel, CPortal::GlobalModel() );
		}

		OutSysTerm();
	}

    MsgTerm();
	ErrSysTerm();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	EWMInternal( "Exited CCodeGenProcessApp::StandardGenerate()" );

	return status;
}

CReturn CCodeGenProcessApp::CcsFabGenerate( CCommand* io_cmd )
{
	CReturn	status;
	CString	txtPath;
	HCURSOR	hCursor;

	CModel& model = io_cmd->getModel();

	EWMInternal( "Entered CCodeGenProcessApp::CcsFabGenerate()" );

	std::stringstream ss;

	// (1) code all models into a single file
	// (2) code each model into its own file
	int ccs = io_cmd->VarList().getInt( "ccs", FALSE );

	CString cgFilePath = io_cmd->VarList().getString( "cgfile", "" );
	CString ncFilePath = io_cmd->VarList().getString( "ncfile", "" );

	ss << "ccs: " << ccs << "\n"
		<< "cgfile <" << (LPCSTR) cgFilePath << ">\n"
		<< "ncfile <" << (LPCSTR) ncFilePath << ">\n";

	EWMInternal( ss.str().c_str() );
	ss.str("");

	CLabelDbGenerator labelDb;
	CString labelDbPath = io_cmd->VarList().getString( "label_db", "" );
	bool part_order = (!labelDbPath.IsEmpty() && (g_cgXref == NULL));
	if ( part_order )
		labelDb.Init( labelDbPath );

	if ( cgFilePath.IsEmpty() )
		status.Internal( IDS_CREATE_PARAM_MISSING );

	// Check for the existence of at least one parameter.  This insures
	// the model was properly prepared by recording the coding parameters
	// in the model header.  In particular, we should be able to recall
	// the coding parameters last used with this model.
	CString cgInHeader = model.Header().getString( "cgfile", "" );

	ss << "cgInHeader: " << cgInHeader;
	EWMInternal( ss.str().c_str() );
	ss.str("");

	if ( cgInHeader.IsEmpty() )
		return status;

	if ( g_cgModel_init )
	{
		g_cgModel.Init( g_cgModel_SS, g_cgModel_TS );
		g_cgModel_init = false;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	txtPath = io_cmd->VarList().getString( "pdb", "" );

	// For debugging.
	if (0)
		txtPath = "C:\\_tmp\\_gary\\pdblist.txt";

	TCodeFileArray fileList;
	status = CcsFabFilesGet( txtPath, &fileList );
	if ( !status.IsOk() )
	{
		ss << "Early termination caused by CcsFabFilesGet(" << (LPCSTR) txtPath << ")";
		EWMInternal( ss.str().c_str() );
		ss.str("");

		fileList.DestructiveFlush();
		return status;
	}

	// Allow message boxes to be issued by the code generator. 
	ErrSysInit();
	MsgInit();

	// Conditionally compile the code generator.
	if ( status.IsOk() )
		status = ConditionalCompile( cgFilePath );

	if ( status.IsOk() )
	{
		CString msg;
		CMM2 mm2;
		CModel tempModel;
		CSeqRules seqRules;
		CDbPattern* sheet;
		double gap_tol;

		// We copy the parameters from the command object because its
		// var-list is altered (during code generation) by subsequent
		// calls to the Portal (via Java).
		CVarList coding_params = io_cmd->VarList();

		// For convenience ....
		CModel* global_model = CPortal::GlobalModel();

		// Since we don't care to track changes to the model ....
		g_cgModel.UndoBufferSuppress();

		int count = fileList.Count();

		msg.Format( "CCodeGenProcessApp::CcsFabGenerate() file count <%d>", count );
		status.Diagnostic( msg );

		for (int indx = 0; indx < count; ++indx)
		{
			CCodeFileRec* file_rec = fileList.GetAt( indx );

			// Discard the old models.
			g_cgModel.Flush();
			tempModel.Flush();

			msg.Format( "loading <%s>", file_rec->FilePath() );
			status.Diagnostic( msg );

			// Load the new model.
			status = mm2.Read( file_rec->FilePath(), &tempModel, MM2_STD_MODE );
			if ( !status.IsOk() )
				break;

			status.Diagnostic( "loaded ok" );

			// Copy the coding parameters to the model because they
			// are directives used by the sequencer.
			CodingParametersCopy( coding_params, &tempModel );

			(*(g_cgModel.pHeader())) = tempModel.Header();

			// We are processing a single in-memory sheet model.
			sheet = ModelCopy( &tempModel, &g_cgModel, 1, TRUE );
			sheet->StringSet( STR_TYPE, "_main" );

			// At initial implementation, this isn't necessary for CCS
			// but we leave it here in case it becomes relevant.
			if ( g_cgModel.Header().getInt( "Ind_Hits", FALSE ) )
				CNibbler::Nibble( &g_cgModel );

			// 2008.03.05 (PE) -- The gap tolerance is used by code generation
			// to identify profiles that are 'logically closed'. In turn, this
			// helps avoid containment parity issues.
			//   See also CSheet::Save() & CContHier::PolyFromExplicitProfile()
			// 2009.03.14 (PE) -- BOMAG was having problems with containment
			// parity. Increasing the gap_tol during nesting caused problems.
			// After some discussion with Gary, we decided to use a "special"
			// user-defined cg variable to solve the problem (because it is
			// a solution that requires minimal effort).
			gap_tol = g_cgModel.Header().getReal( "cg.gap_tol", SMALL );
			CContHier::GapTolSet( gap_tol );

			// CRITICAL: "cg.append" is used by the code generator
			// to determine the mode by which to open the cnc file.
			if (ccs == 2)
			{
				g_cgModel.pVarSys()->setInt( "cg.append", 0 );
				g_cgModel.pVarSys()->setInt( "cg.terminal", 1 );
			}
			else
			{
				g_cgModel.pVarSys()->setInt( "cg.append", (indx > 0) );
				g_cgModel.pVarSys()->setInt( "cg.terminal", (indx == (count-1) ) );
			}

			g_cgModel.pVarSys()->setInt( "cg.indx", indx );

			// Generate the clfile using the given sequencing options.
			//    seqRules.Init( &g_cgModel, io_cmd->VarList() );
			seqRules.Init( &g_cgModel, g_cgModel.Header() );

			// This block exists because there is a timing issue between
			// the clfile ctor and the terminal call to tempModel.Flush().
			// In particular, the ctor must remove attributes from entities
			// that reside in tempModel (before tempModel deletes the entity).
			// An alternate solution employs logic (which could get messy).
			{
				CModelClfile clfile;
				CCodeGenOH outputHandler;

				clfile.RecordPartOrder( part_order );
				clfile.Init( &seqRules );

				// Create the output handler for managing code generator output.
				CString theNcFilePath = NcPathFormat( ccs, ncFilePath, file_rec->FilePath() );
				outputHandler.Init( theNcFilePath, g_cgXref );

				OutSysInit( &outputHandler );

				if ( status.IsOk() )
				{
					// Set the active clfile and generate the NC code.
					WENG_ClfileInit( &clfile );

					// CRITICAL
					io_cmd->setModel( &g_cgModel );
					if (g_cgXref == NULL)
						CPortal::Model( &g_cgModel );

					// For debugging.
					if (0)
						clfile.Dump("c:\\_tmp\\clfile.txt");

					std::stringstream ss;
					ss << "Executing <" << (LPCSTR) cgFilePath << ">";
					EWMInternal( ss.str().c_str() );
					ss.str("");

					// Execute the code generator.
					status = CVm::Execute( cgFilePath, FALSE );

					ss << "Execution status: " << status.getStatus();
					EWMInternal( ss.str().c_str() );
					ss.str("");

					// CRITICAL
					io_cmd->setModel(  CPortal::GlobalModel() );
					if (g_cgXref == NULL)
						CPortal::Model( CPortal::GlobalModel() );

					WENG_ClfileTerm();

					if ( part_order )
					{
						CString pdbPath = g_cgModel.Header().getString( "pdb_file", "" );
						labelDb.LabelTableUpdate( pdbPath, file_rec->SheetNum(), clfile );
					}

					ModelDiffsCopy( g_cgModel, CPortal::GlobalModel() );
				}

				OutSysTerm();
			}
		}

		g_cgModel.Flush();
		tempModel.Flush();
	}

	MsgTerm();
	ErrSysTerm();

	fileList.DestructiveFlush();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	EWMInternal( "Exited CCodeGenProcessApp::CcsFabGenerate()" );

	return status;
}

CReturn CCodeGenProcessApp::SmcCodeGenerate( CCommand* io_cmd )
{
	CReturn			status;
	CModelClfile	clfile;
	CLabelDbGenerator	labelDb;
	CSeqRules		seqRules;
	CString			cgFilePath;
	CString			ncFilePath;
	CString			cgInHeader;
	CDbPattern*		sheet;
	bool			multiSheet = FALSE;
	HCURSOR			hCursor;
	double			gap_tol;

	CModel& model = io_cmd->getModel();

	EWMInternal( "Entered CCodeGenProcessApp::SmcCodeGenerate()" );

	cgFilePath = io_cmd->VarList().getString( "cgfile", "" );
	ncFilePath = io_cmd->VarList().getString( "ncfile", "" );

	if ( cgFilePath.IsEmpty() )
		status.Internal( IDS_CREATE_PARAM_MISSING );

	// Check for the existence of at least one parameter.  This insures
	// the model was properly prepared by recording the coding parameters
	// in the model header.  In particular, we should be able to recall
	// the coding parameters last used with this model.
	cgInHeader = model.Header().getString( "cgfile", "" );
	if ( cgInHeader.IsEmpty() )
	{
		EWMInternal( "ERROR: \"cgfile\" attribute is missing from model header (or empty)" );
		return status;
	}

	// Introduced for Cequent Towing, affects behavior of
	// CShortestPath::OptimizePath().  See comments there.
	int secret_incantation = model.Header().getInt( "cg.group_hits", -1 );
	if (secret_incantation >= 0)
	{
		CRegister::IntSetV( "CodeGeneration", "group_hits", secret_incantation );
	}

	if ( g_cgModel_init )
	{
		g_cgModel.Init( g_cgModel_SS, g_cgModel_TS );
		g_cgModel_init = false;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	g_cgModel.UndoBufferSuppress();
	g_cgModel.Flush();

	CodingParametersCopy( io_cmd->VarList(), CPortal::GlobalModel() );

	(*(g_cgModel.pHeader())) = CPortal::GlobalModel()->Header();

	// We are processing a single in-memory sheet model.
	sheet = ModelCopy( CPortal::GlobalModel(), &g_cgModel, 1, TRUE );
	sheet->StringSet( STR_TYPE, "_main" );

	if ( g_cgModel.Header().getInt( "Ind_Hits", FALSE ) )
		CNibbler::Nibble( &g_cgModel );

	// For debugging.
	if (0)
	{
		CModelText text;
		text.Enable( true );
		status = text.Dump( "c:\\_tmp\\model.txt", g_cgModel );
	}

	// Allow message boxes to be issued by the code generator. 
	ErrSysInit();
	MsgInit();

	if ( status.IsOk() )
		status = ConditionalCompile( cgFilePath );

	if ( status.IsOk() )
	{
		CCodeGenOH outputHandler;
#if 0
		if ( UsingCodeViewer() )
		{
			g_cgModel.ActivePattern( sheet );
			CodeViewerInit( io_cmd, &clfile );
		}
#else
		g_cgModel.ActivePattern( sheet );
#endif
		// 2009.03.14 (PE) -- BOMAG was having problems with containment
		// parity. Increasing the gap_tol during nesting caused problems.
		// After some discussion with Gary, we decided to use a "special"
		// user-defined cg variable to solve the problem (because it is
		// a solution that requires minimal effort).
		gap_tol = g_cgModel.Header().getReal( "cg.gap_tol", SMALL );
		if (gap_tol <= SMALL)
		{
			// 2008.03.05 (PE) -- The gap tolerance is used by code generation
			// to identify profiles that are 'logically closed'. In turn, this
			// helps avoid containment parity issues.
			//   See also CSheet::Save() & CContHier::PolyFromExplicitProfile()
			gap_tol = g_cgModel.Header().getReal( "code_gap", SMALL );
		}
		CContHier::GapTolSet( gap_tol );

		// Generate the clfile using the given sequencing options.
		seqRules.Init( &g_cgModel, io_cmd->VarList() );

		clfile.RecordPartOrder( false );
		clfile.Init( &seqRules );
#if 0
		// Create the output handler for managing code generator output.
		outputHandler.Init( ncFilePath, g_cgXref );

		OutSysInit( &outputHandler );
#else
#endif
		if ( status.IsOk() )
		{
			// Set the active clfile and generate the NC code.
			WENG_ClfileInit( &clfile );

			if (g_cgXref == NULL)
			{
				// ie. exporting to file
				// We must do this.  Otherwise, wacky things happen.
				CPortal::Model( &g_cgModel );
			}

			// clfile.Dump("c:\\_Weng\\V16\\AdvMach\\Data\\clfile.txt");
			std::stringstream ss;
			ss << "Executing <" << (LPCSTR) cgFilePath << ">";
			EWMInternal( ss.str().c_str() );
			ss.str("");

			status = CVm::Execute( cgFilePath, FALSE );

			ss << "Execution status: " << status.getStatus();
			EWMInternal( ss.str().c_str() );
			ss.str("");

			if (g_cgXref == NULL)
			{
				CPortal::Model( CPortal::GlobalModel() );
			}

			WENG_ClfileTerm();

#if 0
			ModelDiffsCopy( g_cgModel, CPortal::GlobalModel() );
#else
#endif
		}

#if 0
		OutSysTerm();
#else
#endif
	}

	MsgTerm();
	ErrSysTerm();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	EWMInternal( "Exited CCodeGenProcessApp::SmcCodeGenerate()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// CodeGen:Optimize:
//    Optimizes all work-zones.
//
// CodeGen:Optimize: id=%d
//    Optimizes all entities in the indicated work-zone.
//    where:
//       'id' is the id of the sequence object.
//
// CodeGen:Optimize: id=%d [,mode=%d]
//    Optimizes all selected entities in the indicated work-zone
//    and insert the optimized entities at the given position.
//    where:
//       'id' is the id of the sequence object.
//       'mode' is one of (0) BEFORE / (1) AFTER
//
CReturn
CCodeGenProcessApp::Optimize( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	ID			seqID;
	
	seqID = io_cmd->VarList().getInt( "id", 0 );
	if (seqID == 0)
	{
		status = OptimizeAll( io_cmd );
	}
	else
	{
		status = OptimizeSubset( io_cmd );
	}

	return	status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// This Portal command is called from the Seq_Order dialog when
// the 'root' node is selected and the Optimize button is pressed.
//
// Sequences the entire model.  By virtue of the Seq_Order order
// dialog having been opened, the Portal command "Seq:Init:" will
// have been called.  In turn, sequence objects will exist because
// CModelClfile::SequenceInit() will have been called, and the
// sequence objects will have 'insert markers'.
//
// Since the entire model is being resequenced, the 'insert markers'
// will lose their position and will be moved to the end of their
// respective sequence objects.
//
// ASSUMPTION: We are processing a single in-memory sheet model.
CReturn 
CCodeGenProcessApp::OptimizeAll( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CModelClfile	clfile;
	CSeqRules		seqRules;
	CDbIterator		iter;
	CString			name;
	CDbSequence*	rootSequence;
	CDbSequence*	dbSequence;
	CDbPattern*		sheet;
	CDbFeature*		dbFeature;
	CDbEntity*		dbEntity;
	HCURSOR			hCursor;
	int				count, indx;
	int				zoneNum;

	CModel& model = io_cmd->getModel();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Because we have to create and delete a wrapper (sheet).
	model.UndoBufferSuppress();

	// This trickery is required by CModelClfile::SequenceInit()
	sheet = SheetCreate( &model, 1 );
	sheet->StringSet( STR_TYPE, "_main" );

	// Collect all of the zones.
	// NOTE: Only entities in a work-zone can be coded.

	iter.Init( model.Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
			sheet->Append( dbFeature );

		iter.Next();
	}

	// Generate the clfile using the given sequencing options.
	seqRules.Init( &model, io_cmd->VarList() );
	clfile.Init( &seqRules );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Use the clfile to generate sequence entities.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CDbEntity::NewAction();

	dbSequence = NULL;

	model.EntityFind( "RootSequence", (CDbEntity**) &rootSequence, DBSEQUENCE, DBSEQUENCE );
	if (rootSequence == NULL)
	{
		model.EntityCreate( DBSEQUENCE, (CDbEntity**) &rootSequence );

		rootSequence->Name("RootSequence");
		rootSequence->StringSet( STR_TYPE, "_root" );
		rootSequence->IntSet( "_zone_num", IUNDEFINED );
		rootSequence->IntSet( "_insert_ba", IUNDEFINED );
		rootSequence->IntSet( "_insert_id", 0 );
	}
	else
	{
		count = rootSequence->Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbSequence = dynamic_cast<CDbSequence*>((*rootSequence)[indx]);
			if (dbSequence != NULL)
			{
				dbSequence->BenignFlush();
				dbSequence->IntSet( "_zone_num", IUNDEFINED );
				dbSequence->IntSet( "_insert_ba", AFTER );
				// dbSequence->IntSet( "_insert_id", 0 );
			}
		}

		dbSequence = NULL;
	}

	count = clfile.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = (CDbEntity*) clfile.Fetch( indx )->Entity();
		if (dbEntity != NULL && !dbEntity->DidAction())
		{
			if (dbEntity->Type() == DBFEATURE)
			{
				zoneNum = dbEntity->IntGet( "_zone_num", 0 );

				if (zoneNum > 0)
				{
					name.Format( "WorkPkg%d", zoneNum );

					model.EntityFind( name, (CDbEntity**) &dbSequence, DBSEQUENCE, DBSEQUENCE );
					if (dbSequence == NULL)
					{
						model.EntityCreate( DBSEQUENCE, (CDbEntity**) &dbSequence );

						dbSequence->Name( name );
						dbSequence->StringSet( STR_TYPE, "_workzone" );
						dbSequence->IntSet( "_zone_num", zoneNum );
					}

					if (dbSequence->Sequence() != rootSequence)
					{
						rootSequence->Append( dbSequence );
					}
				}

				dbEntity->DoAction();
			}

			if (dbSequence != NULL && CDbSequence::CanSequence(dbEntity))
			{
				dbSequence->Append( dbEntity );
				dbEntity->DoAction();
			}
		}
	}

	CModelClfile::InsertMarkersUpdate( &model );

	sheet->BenignFlush();
	sheet->Delete();

	model.UndoBufferActivate();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// CodeGen:Optimize:
//
CReturn
CCodeGenProcessApp::OptimizeSubset( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CModelClfile	clfile;
	CSeqRules		seqRules;
	CWorkPkgArray	workPkgs;
	CWorkPkg*		theWorkPkg;
	CDbEntityArray	cutOrder;
	CDbIterator		iter;
	CString			name;
	CDbSequence*	dbSequence;
	CDbFeature*		workZone;
	CDbCommand*		dbCommand;
	CDbEntity*		dbEntity;
	HCURSOR			hCursor;
	int				count, indx;
	int				pos;

	ID		seqID;
	ID		insID;
	int		mode;

	CModel& model = io_cmd->getModel();
	CSelector& selector = model.SelectorStack()();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Collect and validate the input parameters.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	seqID = io_cmd->VarList().getInt( "id", 0 );
	mode = io_cmd->VarList().getInt( "mode", IUNDEFINED );

 	dbSequence = NULL;

	if (seqID > 0)
	{
		model.EntityFind( seqID, (CDbEntity**) &dbSequence, DBSEQUENCE, DBSEQUENCE );
		if (dbSequence == NULL)
		{
			EWMInternal( "ERROR: CCodeGenProcessApp::Optimize()" );
			return status;
		}

		insID = dbSequence->IntGet( "_insert_id", 0 );
		model.EntityFind( insID, (CDbEntity**) &dbCommand, DBCOMMAND, DBCOMMAND );

		if (dbCommand == NULL)
		{
			EWMInternal( "ERROR: CCodeGenProcessApp::Optimize()" );
			return status;
		}
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Start the 'busy' indicator.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	model.UndoBufferSuppress();
	
	theWorkPkg = new CWorkPkg( NULL, FALSE, 0., 0., 0., 0. );
	workPkgs.Append( theWorkPkg );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Collect the entities that will be optimized.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (mode == BEFORE || mode == AFTER)
	{
		// Use selected entities in indicated work-zone.
		count = selector.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = selector[indx];
			if ( CDbSequence::CanSequence( dbEntity ) )
			{
				if (dbEntity->Sequence() == dbSequence)
				{
					theWorkPkg->Entities().Append( dbEntity );
					dbSequence->Disown( dbEntity );
				}
			}
		}

		pos = dbSequence->Position( dbCommand );
		if (pos < 0)
		{
			// Something is seriously wrong.
			hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
			SetCursor( hCursor );
			EWMInternal( "ERROR: CCodeGenProcessApp::Optimize()" );
			return status;
		}
	}
	else
	{
		// Use all entities in indicated work-zone.
		count = dbSequence->Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = (*dbSequence)[indx];
			if (dbEntity != dbCommand)
				theWorkPkg->Entities().Append( dbEntity );
		}

		dbSequence->BenignFlush();
		pos = -1;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Set the work package dimensions.  Otherwise,
	// sequencing will always optimize from lower left.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	dbEntity = theWorkPkg->Entities()[0];
	workZone = dbEntity->WorkZone();
	theWorkPkg->Box( workZone->Box() );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Generate the clfile using the given sequencing options.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	seqRules.Init( &model, io_cmd->VarList() );
	clfile.OptiCutOrder( workPkgs, &seqRules, &cutOrder );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Move the entities back to the sequence object.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = cutOrder.Count();
	if (count > 0)
	{
		CDbEntity::NewAction();

		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = cutOrder[indx];
			if ( !dbEntity->DidAction() )
			{
				if (mode == BEFORE)
				{
					// NOTE: The insert position moves down
					// with the insertion of each new entity.
					dbSequence->InsertBefore( pos, dbEntity );
					++pos;
				}
				else if (mode == AFTER)
				{
					// NOTE: The insert position remains constant.
					dbSequence->InsertAfter( pos, dbEntity );
					++pos;
				}
				else
				{
					dbSequence->Append( dbEntity );
				}

				dbEntity->DoAction();
			}
		}

		if (mode != BEFORE && mode != AFTER)
		{
			dbSequence->Append( dbCommand );
		}
	}

	model.UndoBufferActivate();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Stop the 'busy' indicator.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		SetCursor( hCursor );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CDbPattern*
CCodeGenProcessApp::FilesMerge(
								const CString&	pdbPath,
								CModel*			model )
{
	CReturn				status;
	TCodeFileArray		fileList;
	CCodeFileRec*		file_rec;
	CSelectorStack		dummy_stack;
	CTreeViewSupport	dummy_tree;
	CModel				dummy_model;
	CMM2				mm2;
	CDbIterator			iter;
	CDbPattern*			sheet;
	CDbCommand*			dbCommand;
	CString				text;
	int					count, indx;

	CDbPattern* main = NULL;

	status = FilesGet( pdbPath, -1, &fileList );

	if ( status.IsOk() )
	{
		dummy_model.UndoBufferSuppress();

		count = fileList.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dummy_model.Flush();
			dummy_model.Init( dummy_stack, dummy_tree );

			file_rec = fileList.GetAt(indx);

			status += mm2.Read( file_rec->FilePath(), &dummy_model, MM2_SKIP_SEQ_OBJS );
			if ( status.IsOk() )
			{
				sheet = ModelCopy( &dummy_model, model, (indx+1), (indx == 0) );
				sheet->StringSet( STR_TYPE, "_sheet" );

				// When coding multiple sheets, all output is written to a
				// single NC file and the main program calls each sheet as
				// a subroutine.

				if (main == NULL)
				{
					model->EntityCreate( DBPATTERN, (CDbEntity**) &main );
					main->Name("main");
				}

				text.Format( "@INSTANCE: instang=0.0, patid=%d", sheet->Id() );
				model->EntityCreate( DBCOMMAND, (CDbEntity**) &dbCommand );
				dbCommand->Init( sheet->Tool(), sheet->Workplane(), 0., 0., 0., text );
				dbCommand->StringSet( STR_TYPE, "_instance" );

				main->Append( dbCommand, FALSE );
			}
		}
	}

	fileList.DestructiveFlush();

	return main;
}

CReturn CCodeGenProcessApp::CcsFabFilesGet( const CString& txtPath, TCodeFileArray* fileList )
{
	CReturn	status;

	std::stringstream ss;
	ss << "CcsFabFilesGet <" << (LPCSTR) txtPath << ">";
	EWMInternal( ss.str().c_str() );
	ss.str("");

	FILE* f = fopen( txtPath, "r" );
	if (f != NULL)
	{
		char buf[1024];
		CString pdbPath;
		CString snum;
		int indx, len;

		while ( fgets( buf,1024, f ) && status.IsOk() )
		{
			pdbPath = buf;
			pdbPath.Replace( "\n", "" );

			ss << "CcsFabFilesGet <" << (LPCSTR) pdbPath << ">";
			EWMInternal( ss.str().c_str() );
			ss.str("");

			indx = pdbPath.Find('|');
			if (indx < 0)
			{
				EWMInternal( "ERROR: CcsFabFilesGet(#2)" );
				break;
			}

			// Extract the various bits of data.
			len = pdbPath.GetLength();
			snum = pdbPath.Right( len - (indx + 1) );
			pdbPath = pdbPath.Left( indx );

			// And just in case ...
			pdbPath.TrimLeft(" \t");
			pdbPath.TrimRight(" \t");

			if ( !pdbPath.IsEmpty() )
			{
				int sheet_num;
				sscanf( snum, "%d", &sheet_num );
				if (pdbPath.Find(".") > 0)
					status = FilesGet( pdbPath, sheet_num, fileList );
				else
					EWMInternal( "ERROR: CcsFabFilesGet(#2)" );
			}
		}
	
		fclose( f );
	}
	else
	{
		EWMInternal( "ERROR: CcsFabFilesGet(#1)" );
	}

	return status;
}

CReturn CCodeGenProcessApp::FilesGet(
	const CString&	pdbPath,
	int				sheet_num,
	TCodeFileArray*	fileList )
{
	CReturn		status;
	CDaoDB		pdb;
	CDaoQuery*	query;
	CString		filename;
	CString		msg;
	int			count, indx;

	std::stringstream ss;

	pdb.addTable( "Sheet Result" );
	status = pdb.Open( pdbPath );

	ss << "FilesGet <" << (LPCSTR) pdbPath << "> " << (status.IsOk() ? "ok" : "failed");
	EWMInternal( ss.str().c_str() );
	ss.str("");

	if ( status.IsOk() )
	{
		CString squery;

		if (sheet_num < 0)
			squery = "SELECT * FROM [Sheet Result]";
		else
			squery.Format( "SELECT * FROM [Sheet Result] WHERE ([ID]=%d)", sheet_num );

		ss << "  FilesGet <" << (LPCSTR) squery << ">";
		EWMInternal( ss.str().c_str() );
		ss.str("");

		query = pdb.QueryExecute( squery );
		if (query != NULL)
		{
			count = query->RecordCount();
			for (indx = 0; indx < count; ++indx)
			{
				query->Move( ((indx == 0) ? 0 : 1) );

				filename = query->StringGet( "Sheet Filename" );

				ss << "    FilesGet <" << (LPCSTR) filename << ">  sheet <" << sheet_num << ">";
				EWMInternal( ss.str().c_str() );
				ss.str("");

				fileList->Append( new CCodeFileRec( filename, sheet_num ) );
			}
		}

		if (fileList->Count() < 1)
		{
			msg.Format( "ERROR: CCodeGenProcessApp::FilesGet() -- no files from %s", pdbPath );
			EWMInternal( (LPCSTR) msg );
		}

		pdb.Close();
	}

	ss << "FilesGet status <" << (status.IsOk() ? "ok" : "failed") << ">";
	EWMInternal( ss.str().c_str() );
	ss.str("");

	return status;
}

// NOTE: "source" should be const.
CReturn
CCodeGenProcessApp::ReferenceEntitiesCopy( CModel& source, CEntityCopier* copier )
{
	CReturn		status;
	CDbIterator iter;
	CDbEntity*	dbEntity;

	iter.Init( source.Db(), DBWORKPLANE );
	while (1)
	{
		dbEntity = iter();
		if (dbEntity->Type() > DBTOOL)
			break;

		dbEntity->Accept( copier );
		
		iter.Next();
	}

	return status;
}

CDbPattern*
CCodeGenProcessApp::SheetCreate( CModel* model, int sheetNo )
{
	CDbPattern*		sheet;
	CDbTool*		dbTool;
	CString			layerName;
	CString			sheetName;

	layerName.Format( "SheetLayer_%d", sheetNo );
	sheetName.Format( "Sheet_%d", sheetNo );

	model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
	dbTool->Name( layerName );

	model->EntityCreate( DBPATTERN, (CDbEntity**) &sheet );
	sheet->Name( sheetName );

	return sheet;
}

CDbPattern*
CCodeGenProcessApp::ModelCopy(
							CModel*	srcModel,
							CModel*	cpyModel,
							int		sheetNo,
							bool	copyCommon )
{
	CReturn			status;
	CDbPattern*		sheet;
	CEntityCopier	copier;
	CDbIterator		iter;
	CDbTool*		dbTool;
	CDbLine*		dbLine;
	CDbFeature*		dbFeature;
	CDbPattern*		dbPattern;
	CDbEntity*		theCopy;

	bool explicit_ordering = (srcModel->Header().getInt( "_eo", FALSE ) != FALSE);

	copier.Init( &(cpyModel->Db()), FLAG_COPY_TOOL_VERBATIM );

	if ( copyCommon )
	{
		ReferenceEntitiesCopy( (*srcModel), &copier );

		if ( UsingCodeViewer() )
		{
			srcModel->EntityFind( STR_STOCK, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

			if (dbTool != NULL)
			{
				iter.Init( srcModel->Db(), DBLINE );
				while (1)
				{
					dbLine = dynamic_cast<CDbLine*>( iter() );
					if (dbLine == NULL)
						break;

					if (dbLine->Tool() == dbTool)
					{
						dbLine->Accept( &copier );
					}

					iter.Next();
				}
			}
		}
	}

	sheet = SheetCreate( cpyModel, sheetNo );

	// Copy any subroutine definitions.
	iter.Init( srcModel->Db(), DBPATTERN );
	while (1)
	{
		dbPattern = dynamic_cast<CDbPattern*>( iter() );
		if (dbPattern == NULL)
			break;

		if (dbPattern->Owner() == NULL)
		{
			dbPattern->Accept( &copier );

			theCopy = copier.ForwardLookup( dbPattern );
			if (theCopy != NULL)
				theCopy->StringSet( STR_TYPE, "_part" );
		}

		iter.Next();
	}

	// Copy all work-zone features.  Processing is restricted
	// to top-level features because the traversal is top-down
	// recursive, and we want to avoid unnecessary processing.
	//
	iter.Init( srcModel->Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
		{
			dbFeature->Accept( &copier );

			theCopy = copier.ForwardLookup( dbFeature );
			if (theCopy != NULL)
				sheet->Append( theCopy );
		}

		iter.Next();
	}

	if ( explicit_ordering )
	{
		bool optimize = (cpyModel->Header().getInt( "optalgorithm", FALSE ) != FALSE);
		if (optimize)
		{
			EWMInternal( "ERROR: You must disable \"Optimize Toolpath\" when coding this model." );
		}
		else
		{
			SpecialModelCopy( srcModel, cpyModel, &copier, sheet );
		}
	}
	else
	{
		StandardModelCopy( srcModel, cpyModel, &copier, sheet );
	}

	return sheet;
}

void
CCodeGenProcessApp::StandardModelCopy(
						CModel*			srcModel,
						CModel*			cpyModel,
						CEntityCopier*	copier,
						CDbPattern*		sheet )
{
	CDbIterator		iter;
	CDbFeature*		dbFeature;
	CDbPattern*		dbPattern;
	CDbSequence*	dbSequence;
	CDbContainer*	owner;
	CDbCommand*		dbCommand;
	ID				patid;

	// NOTE: This is set in CCodeGenProcessApp::Generate().
	bool codeSubs = (cpyModel->Header().getInt( "subs", FALSE ) != 0);
	bool optimize = (cpyModel->Header().getInt( "optalgorithm", FALSE ) != 0);

	iter.Init( srcModel->Db(), DBCOMMAND );
	while (1)
	{
		dbCommand = dynamic_cast<CDbCommand*>(iter());
		if (dbCommand == NULL)
			break;

		if ( IsValidInstance( (*dbCommand) ) )
		{
			if ( !optimize && !codeSubs )
			{
				CReturn status;
				status.User( IDS_CODING_ERROR1 );
				sheet->BenignFlush();
				return;  // early exit
			}

			// Remap the pattern reference of this instance.
			patid = (ID) dbCommand->IntGet( "patid", 0 );
			srcModel->Db().Find( patid, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );

			dbPattern = (CDbPattern*) copier->ForwardLookup( dbPattern );
			dbCommand = (CDbCommand*) copier->ForwardLookup( dbCommand );

			dbCommand->IntSet( "patid", dbPattern->Id() );

			if ( !codeSubs )
			{
				// Convert the instance to some set of features.
				owner = dynamic_cast<CDbContainer*>(dbCommand->Owner());

				CModelUtil::PatternExplode( &dbCommand, &dbFeature );

				if (dbFeature != NULL)
				{
					dbFeature->StringSet( STR_TYPE, "_part" );

					owner->Append( dbFeature, FALSE );
				}
			}
		}

		iter.Next();
	}

	if ( !optimize )
	{
		// Explicitly sequenced entities.

		iter.Init( srcModel->Db(), DBSEQUENCE );
		while (1)
		{
			dbSequence = dynamic_cast<CDbSequence*>(iter());
			if (dbSequence == NULL)
				break;

			if (dbSequence->Sequence() == NULL)
				dbSequence->Accept( copier );

			iter.Next();
		}
	}
}

void
CCodeGenProcessApp::SpecialModelCopy(
						CModel*			srcModel,
						CModel*			cpyModel,
						CEntityCopier*	copier,
						CDbPattern*		sheet )
{
	CReturn			status;
	CDbIterator		iter;
	CToolSequencer	toolSeq;
	CDbEntityArray	cutOrder;
	CDbEntityArray	toolOrder;
	CDbFeatureArray	parts;
	CDbContainer*	owner;
	CDbSequence*	rootSequence;
	CDbSequence*	dbSequence;
	CDbFeature*		dbFeature;
	CDbFeature*		workzone;
	CDbPattern*		dbPattern;
	CDbCommand*		dbCommand;
	CDbEntity*		dbEntity;
	CDbTool*		dbTool;
	ID				patid;
	int				tool_count, tndx;
	int				part_count, pndx;
	int				count, indx;

	// Collect all of the instances.
	iter.Init( srcModel->Db(), DBSEQUENCE );
	while (1)
	{
		dbSequence = dynamic_cast<CDbSequence*>(iter());
		if (dbSequence == NULL)
			break;

		if (dbSequence->Sequence() == NULL)
			dbSequence->Accept( copier );

		iter.Next();
	}

	// Flatten the model.
	status = CModelClfile::SequenceInit( cpyModel, FALSE );
	if ( status.IsOk() )
		cpyModel->EntityFind( "RootSequence", (CDbEntity**) &rootSequence, DBSEQUENCE, DBSEQUENCE );

	if (rootSequence != NULL && status.IsOk())
	{
		rootSequence->Flatten( (DBSEQ_RECURSE | DBSEQ_WORKZONE), &cutOrder );
	}
	else
	{
		EWMInternal( "ERROR: You must use \"Sequence/Order\" before coding this model." );
	}

	if ( status.IsOk() )
	{
		count = cutOrder.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbCommand = dynamic_cast<CDbCommand*>( cutOrder.GetAt(indx) );
			if (dbCommand == NULL)
				continue;

			if ( dbCommand->IsInstance() )
			{
				// Remap the pattern reference of this instance.
				patid = (ID) dbCommand->IntGet( "patid", 0 );
				srcModel->Db().Find( patid, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );

				dbPattern = (CDbPattern*) copier->ForwardLookup( dbPattern );
				// dbCommand = (CDbCommand*) copier->ForwardLookup( dbCommand );

				dbCommand->IntSet( "patid", dbPattern->Id() );

				// Convert the instance to some set of features.
				owner = dynamic_cast<CDbContainer*>(dbCommand->Owner());

				CModelUtil::PatternExplode( &dbCommand, &dbFeature );

				// NOTE: At this point, the resulting feature should be flat,
				// containing only atomic level entities.
				if (dbFeature != NULL)
				{
					dbFeature->StringSet( STR_TYPE, "_part" );

					// NOTE: At this point, owner had better be a workzone.
					owner->Append( dbFeature, FALSE );

					parts.Append( dbFeature );
				}
			}

			iter.Next();
		}

		toolSeq.ToolSetupOrder( (*cpyModel), FALSE, &toolOrder );

		// Resequence all cutting entities in tool/part order.
		tool_count = toolOrder.Count();
		part_count = parts.Count();
		for (tndx = 0; tndx < tool_count; ++tndx)
		{
			dbTool = dynamic_cast<CDbTool*>( toolOrder[tndx] );

			for (pndx = 0; pndx < part_count; ++pndx)
			{
				dbFeature = parts[pndx];

				workzone = dynamic_cast<CDbFeature*>( dbFeature->Owner() );
				if (workzone == NULL)
					break;

				indx = 0;
				while (indx < dbFeature->Count())
				{
					dbEntity = (*dbFeature)[indx];
					if (dbEntity->Tool() == dbTool)
					{
						// NOTE: Append() changes entity ownership,
						// thereby altering the count in dbFeature.
						workzone->Append( dbEntity );
					}
					else
					{
						++indx;
					}
				}
			}
		}

		// Now that we have flattened each workzone, 
		// we can discard all of the "part" features.
		for (pndx = 0; pndx < part_count; ++pndx)
			parts[pndx]->Delete();
	}
}

void
CCodeGenProcessApp::CodeViewerInit( CCommand* io_cmd, CModelClfile* clfile  )
{
	g_cgModel.EntityFind( STR_STOCK, (CDbEntity**) &g_cgStock, DBTOOL, DBTOOL );

	// View behavior will not be correct without this!
	g_cgModel.EntityFind( STR_TOP, (CDbEntity**) &g_cgTop, DBWORKPLANE, DBWORKPLANE );
	if (g_cgTop != NULL)
	{
		g_cgModel.ActiveWorkplane( g_cgTop );
	}

	// Create a line entity for display of rapid traversals.
	g_cgModel.EntityCreate( DBLINE, (CDbEntity**) &g_cgLine );
	g_cgLine->Init( g_cgStock, g_cgTop, C3dCoord( 0,0,0 ), C3dCoord( SMALL,SMALL,0 ) );
	g_cgLine->SelectFlag( true );

	CPortal::Model( &g_cgModel );
	io_cmd->getViewMgr().getCWnd()->Invalidate();
	io_cmd->getViewMgr().ActiveView()->Refresh( true );

	// Tell the xref where to get database ids.
	g_cgXref->Init( clfile );
}

CReturn
CCodeGenProcessApp::ConditionalCompile( const CString& cgFilePath )
{
	CReturn	status;
	CPath	path;
	CString	javaPath;
	CString	classPath;

	CString storagePath = CRegister::StringGetV( "Java", "StoragePath", "<error>" );

	path.Set( cgFilePath );
	javaPath = path.DriveDirFileName() + ".java";
	classPath = storagePath + "\\" + path.FileName() + ".class";

	bool is_newer = CPath::IsNewer( javaPath, classPath );

	std::stringstream ss;
	ss << "ConditionalCompile(): " << (LPCSTR) javaPath
		<< " is" << (is_newer ? "" : " not") << " newer than "
		<< (LPCSTR) classPath;

	EWMInternal( ss.str().c_str() );
	ss.str("");

	if ( CPath::IsNewer( javaPath, classPath ) )
	{
		// Compile the java file representing the code generator.
		ss << "compiling <" << (LPCSTR) cgFilePath << ">";
		EWMInternal( ss.str().c_str() );
		ss.str("");

		status = CVm::Compile( cgFilePath );

		ss << "compile status: " << status.getStatus();
		EWMInternal( ss.str().c_str() );
		ss.str("");
	}

	return status;
}

bool
CCodeGenProcessApp::IsValidInstance( const CDbCommand& dbCommand )
{
	CDbFeature*	dbFeature;
	bool		is_valid;
	
	is_valid = false;

	if ( dbCommand.IsInstance() )
	{
		// Of course someone could put an instance into a standard
		// feature but that would be lunacy ... so too bad for now.
		dbFeature = dynamic_cast<CDbFeature*>( dbCommand.Owner() );
		is_valid = ((dbFeature != NULL) && dbFeature->IsWorkZone());
	}

	return is_valid;
}

// CodeGen:WorkZonesVerify:
CReturn
CCodeGenProcessApp::WorkZonesVerify( CCommand* io_cmd )
{
	CReturn		status;
	CDbIterator	iter;
	CDbEntity*	dbEntity;
	int			wz, count;
	bool		done;

	CModel& model = io_cmd->getModel();

	count = 0;  // count of entities *not* in a workzone

	done = false;

	iter.Init( model.Db(), DBLINE );
	while (1)
	{
		dbEntity = iter();

		switch (dbEntity->Type())
		{
		case DBLINE:
		case DBARC:
		case DBHOLE:
		case DBCOMMAND:

			if ( dbEntity->IsToolpath() )
			{
				// NOTE: (-1) owner is pattern / (-IUNDEFINED) no workzone
				wz = CModelUtil::WorkZoneNum( dbEntity );
				if (wz < 0)
					++count;

				// At initial implementation, we bail on the first encounter.
				// Later, we may want to record all of the entities that are
				// not in a workzone, so that we can provide visual feedback
				// to the user wrt the offending entities.
				done = (count > 0);
			}
			break;

		default:
			done = (dbEntity->Type() > DBCOMMAND);
			break;
		}

		if ( done )
			break;  // we're finished.

		iter.Next();
	}

	io_cmd->setInt( "count", count );

	return status;
}

// CodeGen:ShowRapid: toolid=%d, xs=%f, ys=%f, xe=%f, ye=%f
CReturn
CCodeGenProcessApp::ShowRapid( CCommand* io_cmd )
{
	CReturn		status;

	// Foremost, ShowRapid() is relevant only when the Code Viewer is active.
	if (g_cgXref != NULL)
	{
		const CVarList& vars = io_cmd->VarList();
		CDbTool*	dbTool;
		int			tool_id;

		CModel& model = io_cmd->getModel();

		tool_id = vars.getInt( "toolid", 0 );
		model.EntityFind( tool_id, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

		if (dbTool != NULL)
		{
			C3dCoord	ps;
			C3dCoord	pe;
			double		xs, ys;
			double		xe, ye;

			xs = vars.getReal( "xs", 0. );
			ys = vars.getReal( "ys", 0. );
			xe = vars.getReal( "xe", 0. );
			ye = vars.getReal( "ye", 0. );

			ps.XYZ( xs, ys, 0. );
			pe.XYZ( xe, ye, 0. );

			if ( !ps.WithinTol( pe, SMALL ) )
			{
				C3dCoord	ws;
				C3dCoord	we;
				int			color;

				g_cgLine->Init( g_cgStock, g_cgTop, ps, pe );
				g_cgLine->Flags( g_cgLine->Flags() | DBSHOWPATH );

				color = dbTool->ColorGet( DCOLOR_RED );

				io_cmd->getViewMgr().mapRefToWorld( ps, &ws );
				io_cmd->getViewMgr().mapRefToWorld( pe, &we );
				io_cmd->getViewMgr().ActiveView()->RapidLine( ws, we, color );
			}
		}
	}

	return status;
}

// NOTE: The majority of records (meaning those that represent movement)
// are associated with a database entity. A relatively small number of
// records (essentially flags like EV_MAIN_BEGIN) are *not* associated
// with a database entity. And most recently (2007.05.03), a very small
// number of records are associated with geometric data that is produced
// by the code generator itself (and may or may not be associated with a
// database entity.
// 
ERapidType
CCodeGenProcessApp::CanDisplayRapid(
	const CCodeGeoRec*	recA,
	const CCodeGeoRec*	recB,
	C3dCoord*			ps,
	C3dCoord*			pe )
{
	ERapidType rapid_type = RAPID_NONE;

	const CGeoElem* elemA = recA->GeoElemGet();
	const CGeoElem* elemB = recB->GeoElemGet();

	if ( IsRapid( elemB ) )
	{
		const CGeoLine* geoLine = dynamic_cast<const CGeoLine*>( elemB );
		if (geoLine != NULL)
		{
			// As determined by CCodeGeoXref::GeoElemCreate().
			(*ps) = geoLine->StartPt();
			(*pe) = geoLine->EndPt();
			rapid_type = RAPID_EXPLICIT;
		}
	}
	else if (recA->Entity() != recB->Entity())
	{
		// ASSUMPTION: Neither record will ever be NULL.

		if (EndPtGet( (*recA), ps ) &&  StartPtGet( (*recB), pe ))
		{
			if ( !ps->WithinTolXY( (*pe), SMALL ) )
				rapid_type = RAPID_IMPLICIT;
		}
	}

	return rapid_type;
}

bool
CCodeGenProcessApp::IsRapid( const CGeoElem* elem )
{
	return ((elem != NULL) && (elem->IntGet( "rapid", FALSE ) != FALSE));
}

bool
CCodeGenProcessApp::StartPtGet( const CCodeGeoRec& rec, C3dCoord* pt )
{
	bool has_start_pt = false;

	// Get any sentinel (set by CCodeGeoXref::Append()).
	const CGeoElem* elem = rec.GeoElemGet();

	// NOTE: Geometric data (ie. generated by the code generator) takes precedence.
	if ((elem != NULL) && (elem->IntGet( "rapid", FALSE ) != FALSE))
	{
		(*pt) = rec.GeoElemGet()->StartPt();
		has_start_pt = true;
	}
	else if (rec.Entity() != NULL)
	{
		const CDbEntity* dbEntity = rec.Entity();

		if ((dbEntity->Type() >= DBPOINT) &&
			(dbEntity->Type() <= DBHOLE) &&
			(dbEntity->Type() != DBCOMMAND))
		{
			(*pt) = CCodeUtil::StartPoint( dbEntity );
			has_start_pt = true;
		}
	}

	return has_start_pt;
}

bool
CCodeGenProcessApp::EndPtGet( const CCodeGeoRec& rec, C3dCoord* pt )
{
	bool has_end_pt = false;

	// Get any sentinel (set by CCodeGeoXref::Append()).
	const CGeoElem* elem = rec.GeoElemGet();

	// NOTE: Geometric data (ie. generated by the code generator) takes precedence.
	if ((elem != NULL) && (elem->IntGet( "rapid", FALSE ) != FALSE))
	{
		(*pt) = rec.GeoElemGet()->EndPt();
		has_end_pt = true;
	}
	else if (rec.Entity() != NULL)
	{
		const CDbEntity* dbEntity = rec.Entity();

		if ((dbEntity->Type() >= DBPOINT) &&
			(dbEntity->Type() <= DBHOLE) &&
			(dbEntity->Type() != DBCOMMAND))
		{
			(*pt) = CCodeUtil::EndPoint( dbEntity );
			has_end_pt = true;
		}
	}

	return has_end_pt;
}

void
CCodeGenProcessApp::CodingParametersCopy( const CVarList& params, CModel* model )
{
	bool	gridOpt, pathOpt;

	CVarList* header = model->pHeader();

	// Note, mutually exclusive.
	int optAlg = params.getInt( "optalgorithm", 0 );

	if (optAlg == 0)		// closest point
	{
		pathOpt = true;
		gridOpt = false;
	}
	else if (optAlg == 2)	// progressive
	{
		pathOpt = false;
		gridOpt = false;
	}
	else					// grid (default)
	{
		pathOpt = false;
		gridOpt = true;
	}

	header->setInt( "optalgorithm", optAlg );

	// header->setInt( "drill_opt", params.getInt( "drill_opt", FALSE ) );
	header->setInt( "subs", params.getInt( "subs", FALSE ) );

	// Ugh.  gridOpt, pathOpt and optAlg are all supposed to be mutually
	// exclusive.  However, currently (V16.0.2.14) pathOpt = 1 when
	// optAlg  = 2.  We should not save the closest-point parameters
	// under these conditions.
	if ( pathOpt )
	{
		header->setInt( "mode", params.getInt( "mode", PATH_TOOL ) );
		header->setReal( "stx", params.getReal( "stx", 0. ) );
		header->setReal( "sty", params.getReal( "sty", 0. ) );
	}
	else  // according to Gary ...
	{
		header->setInt( "seq_opt", params.getInt( "seq_opt", SEQUENCE_UNDEFINED ) );
		header->setInt( "slice_opt", params.getInt( "slice_opt", LL_YPOS ) );
		header->setInt( "bidir", params.getInt( "bidir", TRUE ) );
		header->setInt( "sticky", params.getInt( "sticky", 4 ) );

		header->setInt( "AllHolesFirst", params.getInt( "AllHolesFirst", FALSE ) );
		header->setInt( "HolesByToolOrder", params.getInt( "HolesByToolOrder", FALSE ) );
		header->setInt( "HolesAcrossLocalNest", params.getInt( "HolesAcrossLocalNest", FALSE ) );
		header->setInt( "ByCompletePart", params.getInt( "ByCompletePart", BY_LOCAL_CONTAINMENT ) );
	}
}

void
CCodeGenProcessApp::DropDoorDraw( const CGeoElem* elem, CCommand* io_cmd )
{
	CModel& model = io_cmd->getModel();

	double dx = model.Header().getReal( "DoorDx", 0. );
	double dy = model.Header().getReal( "DoorDy", 0. );
	if ((dx > SMALL) && (dy > SMALL))
	{
		CViewBase* view = io_cmd->getViewMgr().ActiveView();

		C3dCoord wp;
		io_cmd->getViewMgr().mapRefToWorld( elem->EndPt(), &wp );

		C2dBox box( (wp.X() - dx), (wp.Y() - (0.5 * dy)), wp.X(), (wp.Y() + (0.5 * dy)) );

		m_door_poly = new CGeoPoly( box );

		// Blah, this is not working for a white background.
		//   view->DrawAtColor( (eDisplayColor) view->Color( COLOR_FG ) );
		view->DrawAtColor( DCOLOR_RED );
		view->DrawAtStyle( DSTYLE_DASH );
		view->XorEnable( true );
		view->DrawGeoAt( *m_door_poly, NO_DELTA );
		view->XorEnable( false );

		// Reset, just in case ....
		view->DrawAtStyle( DSTYLE_DASH );
	}
}

void
CCodeGenProcessApp::DropDoorErase( CCommand* io_cmd )
{
	if (m_door_poly != NULL)
	{
		CViewBase* view = io_cmd->getViewMgr().ActiveView();

		// Blah, this is not working for a white background.
		//   view->DrawAtColor( (eDisplayColor) view->Color( COLOR_FG ) );
		view->DrawAtColor( DCOLOR_RED );
		view->DrawAtStyle( DSTYLE_DASH );
		view->XorEnable( true );
		view->DrawGeoAt( *m_door_poly, NO_DELTA );
		view->XorEnable( false );

		// Reset, just in case ....
		view->DrawAtStyle( DSTYLE_DASH );

		delete m_door_poly;
		m_door_poly = NULL;
	}
}

// Expected values for 'ccs'.
// (1) code all models into a single file
// (2) code each model into its own file
CString
CCodeGenProcessApp::NcPathFormat( int ccs, const CString& ncPath, const CString& nstPath)
{
	CString thePath = ncPath;
	if (ccs == 2)
	{
		CPath temp;
		
		temp.Set( ncPath );
		CString fpath = temp.DriveDir();
		CString ext = temp.Ext();

		temp.Set( nstPath );
		CString nstFile = temp.FileName();

		thePath.Format( "%s\\%s.%s", fpath, nstFile, ext );
	}

	return thePath;
}

// At first implementation, copies only differences in header attributes. This was
// introduced so that ITI could write persistent model attributes to the header
// (for use by report generation).
void CCodeGenProcessApp::ModelDiffsCopy( const CModel& modelA, CModel* modelB )
{
	const CVarList& headerA = modelA.Header();
	CVarList* headerB = modelB->pHeader();

	int countA = headerA.countVar();
	for (int indxA = 0; indxA < countA; ++indxA)
	{
		CVar* varA = headerA.getVar( indxA );
		if (headerB->find( varA->getName() ) < 0)
		{
			// ASSUMPTION: 'varA' was written to 'modelA' during code generation.
			headerB->setVar( *varA );
		}
	}
}
