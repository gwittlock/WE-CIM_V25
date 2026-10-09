// SelectProcess.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "cmn_resource.h"
#include "StringConst.h"

#include "2dBox.h"
#include "3dBox.h"
#include "DbEntity.h"
#include "DbContainer.h"
#include "SelectorStack.h"
#include "Model.h"
#include "SelectProcess.h"

CRouteList CSelectProcessApp::m_selectorRouter;


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
// CSelectProcessApp

BEGIN_MESSAGE_MAP(CSelectProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CSelectProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectProcessApp construction

CSelectProcessApp::CSelectProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSelectProcessApp object

CSelectProcessApp theApp;

CReturn 
CSelectProcessApp::RegisterProcess( CRouteList* io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_route->addSubrouter( CString("Selector"), &m_selectorRouter );

	m_selectorRouter.addProcess( CString("IsSelected"), IsSelected );
	m_selectorRouter.addProcess( CString("IsSelectable"), IsSelectable );

	m_selectorRouter.addProcess( CString("Count"), Count );
	m_selectorRouter.addProcess( CString("Get"), Get );

	m_selectorRouter.addProcess( CString("Select"), Select );
	m_selectorRouter.addProcess( CString("Unselect"), Unselect );

	m_selectorRouter.addProcess( CString("Empty"), Empty );

	m_selectorRouter.addProcess( CString("Add"), Add );
	m_selectorRouter.addProcess( CString("Remove"), Remove );

	m_selectorRouter.addProcess( CString("SelectAllRefsTo"), SelectAllRefsTo );

	m_selectorRouter.addProcess( CString("SelectBox"), SelectBox );
	m_selectorRouter.addProcess( CString("SelectAll"), SelectAll );

	m_selectorRouter.addProcess( CString("Clear"), SelectionsClear );
	m_selectorRouter.addProcess( CString("Filter"), SelectionFilter );
	m_selectorRouter.addProcess( CString("Restrictions"), Restrictions );
	m_selectorRouter.addProcess( CString("System"), System );

	m_selectorRouter.addProcess( CString("Push"), SelectorPush );
	m_selectorRouter.addProcess( CString("Pop"),  SelectorPop );

	m_selectorRouter.addProcess( CString("SystemFlag"), SelectorSystemFlag );

	return CReturn( STATUS_OKAY );
}

CReturn 
CSelectProcessApp::Selector( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_selectorRouter.Dispatch( io_cmd );
}

// Selector:IsSelected: id = %d
CReturn
CSelectProcessApp::IsSelected( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	if (id < 1)
		status.Internal( IDS_SELECT_PARAM_MISSING, "id" );

	if ( status.IsOk() )
	{
		bool isSelected = selector.IsSelected( id );
		io_cmd->setInt( "bool", (isSelected ? 1 : 0) );
	}

	return status;
}

// Selector:IsSelectable: id = %d, [state = %d]
CReturn
CSelectProcessApp::IsSelectable( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	int state = io_cmd->VarList().getInt( "state", -1 );

	if (id < 1)
	{
		status.Internal( IDS_SELECT_PARAM_MISSING, "id" );
		return status;
	}


	CDbEntity* dbEntity = NULL;
	status = model.EntityFind( id, &dbEntity );

	if ( status.IsOk() )
	{
		if (state < 0)
			state = (dbEntity->IsSelectable() ? 1 : 0);
		else
			dbEntity->SelectableFlag( (state != 0) );

		io_cmd->setInt( "bool", state );
	}

	return status;
}

// Selector:Count:
CReturn
CSelectProcessApp::Count( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int count = selector.Count();

	io_cmd->setInt( "count", count );
	return CReturn( STATUS_OKAY );
}

// Selector:Get: index = %d
CReturn
CSelectProcessApp::Get( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int indx = io_cmd->VarList().getInt( "index", -1 );
	if (indx < 0 || indx >= selector.Count())
	{
		CString msg;
		msg.Format( "CSelectProcess::SelectionsGet() bad index" );
		status.Internal( IDS_INTERNAL_ERROR, msg );
		return status;
	}

	CDbEntity* dbEntity = selector[indx];
	ID id = dbEntity->Id();

	io_cmd->setInt( "id", id );

	return status;
}

// Selector:Select: id=%d, zone=%b
CReturn
CSelectProcessApp::Select( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSelectProcessApp::Select()" );
		return status;
	}

	bool zone = (io_cmd->VarList().getInt( "zone", 0) != 0);
	selector.AllowWorkzone(zone);

	status = selector.FilteredSelect( id );
	selector.AllowWorkzone(FALSE);

	CDbEntity* dbSelected = selector.FirstSelected();

	id = ((dbSelected == NULL) ? 0 : dbSelected->Id());

	io_cmd->setInt( "id", id );

	return status;
}

// Selector:Unselect: id=%d
CReturn
CSelectProcessApp::Unselect( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	if (id < 1)
	{
		// status.Internal( IDS_SELECT_PARAM_MISSING, "id" );
		return status;
	}


	status = selector.FilteredDeselect( id );

	return status;
}

// Selector:Empty:
//
// Designed for 'deselection via the Selection Toolbar', maintains selection
// set integrity by removing all 'wrongly selected' containers.  A 'wrongly
// selected' container is a selected container whose children are not all
// selected.
//
// From the UI perspective, selecting a container is an all-or-nothing proposition.
// For instance, if a CDbProfile entity is in the selection set then all of its
// children must be in the selection set.
//
CReturn
CSelectProcessApp::Empty( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int indx = 0;
	while (indx < selector.Count())
	{
		CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( selector[indx] );

		bool remove = (dbContainer != NULL);

		if ( remove )
		{
			int count = dbContainer->Count();
			if (count > 0)
			{
				int jndx;
				for (jndx = 0; jndx < count; ++jndx)
				{
					CDbEntity* dbEntity = (*dbContainer)[jndx];
					if ( !dbEntity->IsSelected() )
						break;
				}

				remove = (jndx < count);
			}
		}

		if ( remove )
		{
			selector.Remove( dbContainer->Id() );
			dbContainer->SelectFlag( false );
		}
		else
			++indx;
	}

	return ( CReturn( STATUS_OKAY ) );
}

// Selector:Add: id=%d
// NOTE: This appears to be a Java-only portal command.
CReturn
CSelectProcessApp::Add( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSelectProcessApp::Add()" );
		return status;
	}

	status = selector.Add( id );

	return status;
}

// Selector:Remove: id=%d
// NOTE: This appears to be a Java-only portal command.
CReturn
CSelectProcessApp::Remove( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSelectProcessApp::Remove()" );
		return status;
	}

	status = selector.Remove( id );

	return status;
}

// Selector:SelectAllRefsTo: id = %d
CReturn
CSelectProcessApp::SelectAllRefsTo( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSelectProcessApp::SelectAllRefsTo()" );
		return status;
	}


	status = selector.SelectAllRefsTo( id );

	return status;
}

// Selector:SelectBox: xmin, ymin, zmin, xmax, ymax, zmax, [tol]
CReturn
CSelectProcessApp::SelectBox( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double xmin, ymin, zmin;
	double xmax, ymax, zmax;
	double tmp;
	double tol = 1.e-3;  // the chordal tolerance of arc bounding boxes.

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CReturn status;

	status += io_cmd->getReal( STR_XMIN, &xmin );
	status += io_cmd->getReal( STR_YMIN, &ymin );
	status += io_cmd->getReal( "zmin", &zmin );
	status += io_cmd->getReal( STR_XMAX, &xmax );
	status += io_cmd->getReal( STR_YMAX, &ymax );
	status += io_cmd->getReal( "zmax", &zmax );
	
	io_cmd->getReal( "tol", &tol );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSelectProcessApp::SelectBox() missing param" );
		return status;
	}

	// Correct any min/max problems.

	if (xmin > xmax)
	{
		tmp = xmax;
		xmax = xmin;
		xmin = tmp;
	}

	if (ymin > ymax)
	{
		tmp = ymax;
		ymax = ymin;
		ymin = tmp;
	}

	if (zmin > zmax)
	{
		tmp = zmax;
		zmax = zmin;
		zmin = tmp;
	}

	// C3dBox box( xmin, ymin, zmin, xmax, ymax, zmax );
	C2dBox box( xmin, ymin, xmax, ymax );
	selector.SelectBox( box, tol );

	return CReturn( STATUS_OKAY );
}

// Selector:SelectAll: chs=%d
CReturn
CSelectProcessApp::SelectAll( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	bool considerHiddenStatus = (io_cmd->VarList().getInt( "chs", TRUE ) != 0);

	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();
	selector.SelectAll( considerHiddenStatus );

	return CReturn( STATUS_OKAY );
}

// Selector:Clear:
CReturn
CSelectProcessApp::SelectionsClear( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();
	selector.Clear();
	return CReturn( STATUS_OKAY );
}

// Selector:Filter:
CReturn
CSelectProcessApp::SelectionFilter( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int all, layer, work, tool;
	int point, line, arc, hole;
	int prof, cmd, feat, seq, pat, encoded;

	encoded = io_cmd->VarList().getInt( "encoded", -1 );
	if (encoded >= 0)
	{
		// NOTE: The hex values must match those in Modeler/Selector.java
		all   = -1;
		layer = (encoded & 0x001);
		work  = (encoded & 0x002);
		tool  = (encoded & 0x004);
		point = (encoded & 0x008);
		line  = (encoded & 0x010);
		arc   = (encoded & 0x020);
		hole  = (encoded & 0x040);
		prof  = (encoded & 0x080);
		cmd   = (encoded & 0x100);
		feat  = (encoded & 0x200);
		seq   = (encoded & 0x400);
		pat   = (encoded & 0x800);
	}
	else
	{
		all   = io_cmd->VarList().getInt( "ALL", -1 );
		layer = io_cmd->VarList().getInt( "LY",  -1 );
		work  = io_cmd->VarList().getInt( "WK",  -1 );
		tool  = io_cmd->VarList().getInt( "TL",  -1 );
		point = io_cmd->VarList().getInt( "PT",  -1 );
		line  = io_cmd->VarList().getInt( "LN",  -1 );
		arc   = io_cmd->VarList().getInt( "AR",  -1 );
		hole  = io_cmd->VarList().getInt( "HL",  -1 );
		prof  = io_cmd->VarList().getInt( "PR",  -1 );
		cmd   = io_cmd->VarList().getInt( "CM",  -1 );
		feat  = io_cmd->VarList().getInt( "FE",  -1 );
		seq   = io_cmd->VarList().getInt( "SQ",  -1 );
		pat   = io_cmd->VarList().getInt( "PT",  -1 );
	}

	if (all >= 0)
		selector.All( all );
#ifdef LAYER
	if (layer >= 0)
		 selector.Filter( DBLAYER, layer );
#endif
	if (work >= 0)
		 selector.Filter( DBWORKPLANE, work );

	if (tool >= 0)
		 selector.Filter( DBTOOL, tool );

	if (point >= 0)
		 selector.Filter( DBPOINT, point );

	if (line >= 0)
		 selector.Filter( DBLINE, line );

	if (arc >= 0)
		 selector.Filter( DBARC, arc );

	if (hole >= 0)
		 selector.Filter( DBHOLE, hole );

	if (prof >= 0)
		 selector.Filter( DBPROFILE, prof );

	if (cmd >= 0)
		 selector.Filter( DBCOMMAND, cmd );

	if (feat >= 0)
		 selector.Filter( DBFEATURE, feat );

	if (seq >= 0)
		 selector.Filter( DBSEQUENCE, seq );

	if (pat >= 0)
		 selector.Filter( DBPATTERN, pat );

	return ( CReturn( STATUS_OKAY ) );
}

// Selector:Restrictions: active = %d
CReturn
CSelectProcessApp::Restrictions( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int active = io_cmd->VarList().getInt( "active", 0 );

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();
	selector.Restrictions( (active != 0) );

	return ( CReturn( STATUS_OKAY ) );
}

// Selector:System: allow = %d
CReturn
CSelectProcessApp::System( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int allow = io_cmd->VarList().getInt( "allow", 0 );

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();
	selector.SystemFlag( !(allow != 0) );

	return ( CReturn( STATUS_OKAY ) );
}


// Selector::Push:
CReturn
CSelectProcessApp::SelectorPush( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();

	selectorStack.Push();

	return ( CReturn( STATUS_OKAY ) );
}

// Selector:Pop:
CReturn
CSelectProcessApp::SelectorPop( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();

	selectorStack.Pop();

	CSelector& selector = selectorStack();
	io_cmd->setInt( "filters", selector.Filters() );

	return ( CReturn( STATUS_OKAY ) );
}

// Selector:SystemFlag:
CReturn
CSelectProcessApp::SelectorSystemFlag( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int state = 0;
	status = io_cmd->getInt( "state", &state );

	if ( status.IsOk() )
		selector.SystemFlag( (state != 0) );
	else
		status.Internal( IDS_INTERNAL_ERROR, "CSelectorProcessApp::SelectSystemFlag()" );

	return status;
}


