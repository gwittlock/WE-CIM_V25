// ModelProcess.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"
#include "Model.h"
#include "ModelProcess.h"

CRouteList CModelProcessApp::m_modelRouter;

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
// CModelProcessApp

BEGIN_MESSAGE_MAP(CModelProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CModelProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelProcessApp construction

CModelProcessApp::CModelProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CModelProcessApp object

CModelProcessApp theApp;

CReturn 
CModelProcessApp::RegisterProcess( CRouteList* io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods in MPModel.cpp
	//
	status += io_route->addSubrouter( "Model", &m_modelRouter );

	status += m_modelRouter.addProcess( "Init", ModelInit );
	status += m_modelRouter.addProcess( "ActiveTool", ModelActiveTool );
	status += m_modelRouter.addProcess( "ActiveWorkplane", ModelActiveWorkplane );
	status += m_modelRouter.addProcess( "EntityCount", ModelEntityCount );
	status += m_modelRouter.addProcess( "Get", ModelGet );
	status += m_modelRouter.addProcess( "Get2", ModelGet2 );
	status += m_modelRouter.addProcess( "ToolGet", ModelToolGet );
	status += m_modelRouter.addProcess( "NameCheck", ModelNameCheck );
	status += m_modelRouter.addProcess( "PartExtent", ModelPartExtent );
	status += m_modelRouter.addProcess( "MachineUpdate", ModelMachineUpdate );
	status += m_modelRouter.addProcess( "ToolsUpdate", ModelToolsUpdate );
	status += m_modelRouter.addProcess( "StockUpdate", ModelStockUpdate );
	status += m_modelRouter.addProcess( "IsRightHanded", ModelIsRightHanded );
	status += m_modelRouter.addProcess( CString("VarsLoad"), ModelVarsLoad );
	status += m_modelRouter.addProcess( CString("CTGRead"), ModelCTGRead );
	status += m_modelRouter.addProcess( CString("CTGSave"), ModelCTGSave );
	status += m_modelRouter.addProcess( CString("Purge"), ModelPurge );
	status += m_modelRouter.addProcess( CString("LayerMapApply"), LayerMapApply );

	status += m_modelRouter.addProcess( CString("StatisticsGet"), StatisticsGet );

	status += m_modelRouter.addProcess( CString("WorkZonesWrite"), WorkZonesWrite );
	status += m_modelRouter.addProcess( CString("ClampsWrite"), ClampsWrite );

	return status;
}

CReturn 
CModelProcessApp::Model( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_modelRouter.Dispatch( io_cmd );
}

