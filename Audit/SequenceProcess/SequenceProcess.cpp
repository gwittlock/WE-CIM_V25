// SequenceProcess.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"

#include "DbSequence.h"

#include "Model.h"
#include "ViewMgr.h"

#include "SequenceProcess.h"

CRouteList CSequenceProcessApp::m_seqRouter;

// ==================================================================

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
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
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
// CSequenceProcessApp

BEGIN_MESSAGE_MAP(CSequenceProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CSequenceProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequenceProcessApp construction

CSequenceProcessApp::CSequenceProcessApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSequenceProcessApp object

// CSequenceProcessApp theApp;

CReturn 
CSequenceProcessApp::RegisterProcess( CRouteList* io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	status += io_route->addSubrouter( "Seq", &m_seqRouter );

	status += m_seqRouter.addProcess( "Init", Init );
	status += m_seqRouter.addProcess( "Wrap", Wrap );
	status += m_seqRouter.addProcess( "Explode", Explode );
	status += m_seqRouter.addProcess( "Move", Move );
	status += m_seqRouter.addProcess( "Optimize", Optimize );
	status += m_seqRouter.addProcess( "Insert", InsertPosition );

	status += m_seqRouter.addProcess( "Iter_init", Iter_init );
	status += m_seqRouter.addProcess( "Iter_next", Iter_next );
	status += m_seqRouter.addProcess( "Iter_get", Iter_get );

	status += m_seqRouter.addProcess( "Create", Create );
	status += m_seqRouter.addProcess( "Count", Count );
	status += m_seqRouter.addProcess( "Get", Get );
	status += m_seqRouter.addProcess( "Append", Append );
	status += m_seqRouter.addProcess( "BenignFlush", BenignFlush );

	// For debugging.
	status += m_seqRouter.addProcess( "Dump", Dump );

	return status;
}

/////////////////////////////////////////////////////////////////////////////

CReturn 
CSequenceProcessApp::Seq( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_seqRouter.Dispatch( io_cmd );
}
