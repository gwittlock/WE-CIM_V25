// EntityProcess.cpp : Defines the initialization routines for the DLL.
//


#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"
#include "DbEntity.h"
#include "DbWorkplane.h"
#include "Model.h"
#include "EntityProcess.h"

CRouteList CEntityProcessApp::m_entityRouter;
CRouteList CEntityProcessApp::m_containerRouter;


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
// CEntityProcessApp

BEGIN_MESSAGE_MAP(CEntityProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CEntityProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntityProcessApp construction

CEntityProcessApp::CEntityProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEntityProcessApp object

CEntityProcessApp theApp;

CReturn 
CEntityProcessApp::RegisterProcess( CRouteList* io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods in EPEntity.cpp
	//
	status += io_route->addSubrouter( "Entity", &m_entityRouter );

	status += m_entityRouter.addProcess( "Type", EntityType );
	status += m_entityRouter.addProcess( "VBType", EntityVBType );
	status += m_entityRouter.addProcess( "Name", EntityName );
	status += m_entityRouter.addProcess( "Work", EntityWork );
	status += m_entityRouter.addProcess( "Tool", EntityTool );
	status += m_entityRouter.addProcess( "RefCnt", EntityRefCnt );
	status += m_entityRouter.addProcess( "Box", EntityBox );
	status += m_entityRouter.addProcess( "IsLead", EntityIsLead );
	status += m_entityRouter.addProcess( "ViewBehavior", ViewBehavior );
	status += m_entityRouter.addProcess( "Hide", Hide );
	status += m_entityRouter.addProcess( "Show", Show);
	status += m_entityRouter.addProcess( "IsHidden", IsHidden);
	status += m_entityRouter.addProcess( "System", System );
	status += m_entityRouter.addProcess( "User",   User );

	status += m_entityRouter.addProcess( "RefsToInit", RefsToInit );
	status += m_entityRouter.addProcess( "RefsToCount", RefsToCount );
	status += m_entityRouter.addProcess( "RefsToGet", RefsToGet );
	status += m_entityRouter.addProcess( "RefsToTerm", RefsToTerm );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods in EPContainer.cpp
	//
	status += m_entityRouter.addSubrouter( "Container", &m_containerRouter );

	status += m_containerRouter.addProcess( "Count", ContainerCount );
	status += m_containerRouter.addProcess( "Get", ContainerGet );
	status += m_containerRouter.addProcess( "Set", ContainerSet );
	status += m_containerRouter.addProcess( "Append", ContainerAppend );
	status += m_containerRouter.addProcess( "InsertBefore", ContainerInsertBefore );
	status += m_containerRouter.addProcess( "InsertAfter", ContainerInsertAfter );
	status += m_containerRouter.addProcess( "Disown", ContainerDisown );
	status += m_containerRouter.addProcess( "BenignFlush", ContainerBenignFlush );
	status += m_containerRouter.addProcess( "DestructiveFlush", ContainerDestructiveFlush );

	return status;
}

CReturn 
CEntityProcessApp::Entity( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_entityRouter.Dispatch( io_cmd );
}

CReturn 
CEntityProcessApp::Container( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_containerRouter.Dispatch( io_cmd );
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Private utility method.
//
CDbEntity* 
CEntityProcessApp::EntityGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDbEntity* dbEntity = NULL;

	ID id = io_cmd->VarList().getInt( "id", 0 );

	if (id > 0)
	{
		CModel&	model = io_cmd->getModel();
		model.EntityFind( id, &dbEntity, DBWORKPLANE, DBSEQUENCE );
	}

	return dbEntity;
}
