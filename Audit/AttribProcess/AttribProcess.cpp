// AttribProcess.cpp : Defines the initialization routines for the DLL.
//


#include "stdafx.h"

#include "ColorConst.h"
#include "StringConst.h"

#include "Portal.h"
#include "MathConst.h"
#include "cmn_resource.h"
#include "DbEntity.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "Model.h"
#include "ModelUtil.h"

#include "Lead.h"

#include "AttribProcess.h"



// ==================================================================

static CVarList global_headerVarset;
static CVarList global_attribs;

CRouteList CAttribProcessApp::m_attribRouter;
CRouteList CAttribProcessApp::m_listRouter;
CRouteList CAttribProcessApp::m_selectedRouter;

const char STRING_PREFIX = '$';
const char INT_PREFIX = '#';

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
// CAttribProcessApp

BEGIN_MESSAGE_MAP(CAttribProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CAttribProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttribProcessApp construction

CAttribProcessApp::CAttribProcessApp()
{
	// TO_DO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAttribProcessApp object

CAttribProcessApp theApp;

CReturn 
CAttribProcessApp::RegisterProcess( CRouteList* io_route )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	status += io_route->addSubrouter( "Attrib", &m_attribRouter );

	status += m_attribRouter.addProcess( "Get", GetAtt );
	status += m_attribRouter.addProcess( "Set", SetAtt );
	status += m_attribRouter.addProcess( "Del", DelAtt );
	status += m_attribRouter.addProcess( "Multi", SetMulti );

	status += m_attribRouter.addProcess( "Update", UpdateAtt );
	status += m_attribRouter.addProcess( "UpdateHide", UpdateHide );
	status += m_attribRouter.addProcess( "UpdateShow", UpdateShow );
	status += m_attribRouter.addProcess( "UpdateFeature", UpdateFeatureAtt );
	status += m_attribRouter.addProcess( "UpdateLayer", UpdateLayerAtt );
	status += m_attribRouter.addProcess( "UpdateDel", UpdateDelAtt );

	status += m_attribRouter.addProcess( "UpdateExternal", UpdateExternal );

	status += m_attribRouter.addProcess( "DefGet", DefaultGetAtt );
	status += m_attribRouter.addProcess( "DefSet", DefaultSetAtt );
	status += m_attribRouter.addProcess( "DefDel", DefaultDelAtt );

	status += m_attribRouter.addProcess( "HeadCount", HeaderCountAtt );
	status += m_attribRouter.addProcess( "HeadGet", HeaderGetAtt );
	status += m_attribRouter.addProcess( "HeadSet", HeaderSetAtt );
	status += m_attribRouter.addProcess( "HeadDel", HeaderDelAtt );

	status += m_attribRouter.addProcess( "HeadVarsetExtract", HeaderVarsetExtract );
	status += m_attribRouter.addProcess( "HeadVarNameGet", HeaderVarNameGet );

	status += m_attribRouter.addProcess( "SetChain", SetChainAtt );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//     Introduced for used-defined attribute lists via Java
	//     These portal commands are of the form Attrib:List:foo:

	status += m_attribRouter.addSubrouter( "List", &m_listRouter );

	status += m_listRouter.addProcess( "New",		AttribListNew );
	status += m_listRouter.addProcess( "Destroy",	AttribListDestroy );
	status += m_listRouter.addProcess( "Flush",		AttribListFlush );
	status += m_listRouter.addProcess( "Count",		AttribListCount );
	status += m_listRouter.addProcess( "Name",		AttribListName );
	status += m_listRouter.addProcess( "Get",		AttribListGet );
	status += m_listRouter.addProcess( "Set",		AttribListSet );
	status += m_listRouter.addProcess( "Del",		AttribListDel );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//		Introduced for the V16 Edit Attributes dialog.

	status += m_attribRouter.addSubrouter( "Selected", &m_selectedRouter );

	status += m_selectedRouter.addProcess( "Count",		SelectedCount );
	status += m_selectedRouter.addProcess( "Get",		SelectedGet );
	status += m_selectedRouter.addProcess( "Set",		SelectedSet );
	status += m_selectedRouter.addProcess( "Delete",	SelectedDelete );

	return status;
}

/////////////////////////////////////////////////////////////////////////////

CReturn 
CAttribProcessApp::Attrib( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_attribRouter.Dispatch( io_cmd );
}

CReturn 
CAttribProcessApp::List( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_listRouter.Dispatch( io_cmd );
}


// ==================================================================
// Attrib:Get:
//			id
//			name		( $string, #integer, real)
//
//	returns: val
//
CReturn 
CAttribProcessApp::GetAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID			id;
	CString	name;
	
	ret += io_cmd->getInt( "id", (int*)&id );
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Get the attribute...
	//
	CDbEntity*	db_ent;

	ret += model.EntityFind( id, &db_ent );
	if (!db_ent)
	{
		// Guarding against id==0 is done to limit output to the EWM.
		// Disabling may cause problems when trying to debug app issues
		// so re-enable as necessary. The volume of output becomes
		// evindent when using the list view (for example).
		if (id != 0)
		{
			ret.Internal( IDS_ENTITY_NO_EXIST, id );
		}
	}
	else
	{
		if (name.CompareNoCase( "$name" ) == 0)
			io_cmd->setString( "val", db_ent->Name() );
		else if (name.CompareNoCase("#color") == 0)
			io_cmd->setInt( "val", db_ent->ColorGet( DCOLOR_WHITE ) );
		else
			ret += do_get_att( db_ent->Attrib(), name, io_cmd );
	}

	return ret;
}

// ==================================================================
// Attrib:Set:
//			id
//			name		( $string, #integer, real)
//			val
//
CReturn 
CAttribProcessApp::SetAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	ID id = 0;
	
	ret += io_cmd->getInt( "id", (int*)&id );
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Set the attribute...
	//
	CDbEntity*	db_ent;

	ret += model.EntityFind( id, &db_ent );
	if (!db_ent)
	{
		ret.Internal( IDS_ENTITY_NO_EXIST, id );
	}
	else
	{
		if ( !name.CompareNoCase( "$name" ) )
		{
			CString sval;
			ret += io_cmd->getString( "val", &sval );
			db_ent->Name( sval );
		}
		else
			ret += do_set_att( db_ent->pAttrib(), name, io_cmd );
	}

	return ret;
}

// ==================================================================
// Attrib:SetChain:
//			id=%d
//			val=%b
//
CReturn 
CAttribProcessApp::SetChainAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID id = 0;
	int val = 0;
	
	ret += io_cmd->getInt( "id", (int*)&id );
	ret += io_cmd->getInt( "val", &val );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Set the attribute...
	//
	CDbEntity* db_ent;
	ret += model.EntityFind( id, &db_ent );
	if (!db_ent)
	{ ret.Internal( IDS_ENTITY_NO_EXIST, id ); }
	else
	{ 
		CDbContainer* owner = dynamic_cast<CDbContainer*>(db_ent->Owner());
		if (owner)
		{
			int num = owner->Count();
			for (int idx=0; idx<num; idx++)
			{ do_set_chain( (*owner)[idx], 0); }
		}
		ret += do_set_chain(db_ent, val);
	}

	return ret;
}

// ==================================================================
// Attrib:Del:
//			id
//			name		( $string, #integer, real, or "all")
//
CReturn 
CAttribProcessApp::DelAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	ID id = 0;
	
	ret += io_cmd->getInt( "id", (int*)&id );
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//	Delete the attribute...
	//
	CDbEntity*	db_ent;

	ret += model.EntityFind( id, &db_ent );
	if (!db_ent)
		ret.Internal( IDS_ENTITY_NO_EXIST, id );
	else
		ret += do_del_att( db_ent->pAttrib(), name );

	return ret;
}

// ==================================================================
// Attrib:Update:
//			name		( $string, #integer, real)
//			val
//
//	Sets this attribute in all selected entities... 
//
CReturn 
CAttribProcessApp::UpdateAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Set the attribute... everywhere!
	//
	CDbEntity*	db_ent;
	int num = selector.Count();
	for (int idx = 0; idx<num; idx++)
	{
		db_ent = selector[idx];
		if (db_ent)
			ret += do_set_att( db_ent->pAttrib(), name, io_cmd );
	}

	return ret;
}

// ==================================================================
// Attrib:UpdateHide:
//
//	Sets this attribute in all selected entities... 
//
CReturn 
CAttribProcessApp::UpdateHide( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CReturn			ret;

	// -----------------------------------------------------
	//		Set the attribute... everywhere!
	//
	CDbEntity*	db_ent;
	int num = selector.Count();
	for (int idx = 0; idx<num; idx++)
	{
		db_ent = selector[idx];
		if (db_ent)
			db_ent->Hide();
	}

	return ret;
}

// ==================================================================
// Attrib:UpdateShow:
//
//	Sets this attribute in all selected entities... 
//
CReturn 
CAttribProcessApp::UpdateShow( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CReturn			ret;

	// -----------------------------------------------------
	//		Set the attribute... everywhere!
	//
	CDbEntity*	db_ent;
	int num = selector.Count();
	for (int idx = 0; idx<num; idx++)
	{
		db_ent = selector[idx];
		if (db_ent)
			db_ent->Seek();
	}

	return ret;
}


// ==================================================================
// Attrib:UpdateFeature: feature=%d
//
//	Re-tool all selected entities, and re-parent them to the given
//	feature.
//
CReturn 
CAttribProcessApp::UpdateFeatureAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID feature = 0;
	
	ret += io_cmd->getInt( "feature", (int*)&feature );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	//
	// First, clear the tag on everything...
	//
	CDbEntity::NewAction();

	// -----------------------------------------------------
	//		Set the attribute... everywhere!
	//
	int num = selector.Count();
	for (int idx = 0; idx<num; idx++)
	{
		CDbEntity* db_ent = selector[idx];
		if (db_ent)
		{
			if (!db_ent->DidAction())
				ret += do_feature_att( model, db_ent, feature );
		}
	}

	return ret;
}

// ==================================================================
// Attrib:UpdateLayer: layer=%d
//
//	Re-layer all selected entities
//
CReturn 
CAttribProcessApp::UpdateLayerAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	ID id = io_cmd->VarList().getInt( "layer", 0 );

	CDbTool* dbTool;
	model.EntityFind( id, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	if (dbTool == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::UpdateLayerAtt()" );
	}
	else
	{
		int color = dbTool->ColorGet( DCOLOR_WHITE );

		int count = selector.Count();
		for (int indx = 0; indx < count; indx++)
		{
			CDbEntity* dbEntity = selector[indx];
			if (dbEntity)
			{
				dbEntity->Tool( dbTool );
				dbEntity->ColorSet( color );
			}
		}
	}

	return status;
}

// ==================================================================
// Attrib:UpdateDel:
//			name		( $string, #integer, real)
//
//	Delete this attribute in all selected entities... 
//
CReturn 
CAttribProcessApp::UpdateDelAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Set the attribute... everywhere!
	//
	CDbEntity*	db_ent;
	int num = selector.Count();
	for (int idx = 0; idx<num; idx++)
	{
		db_ent = selector[idx];
		if (db_ent)
			ret += do_del_att( db_ent->pAttrib(), name );
	}

	return ret;
}

// ==================================================================
// Attrib:Multi: id=%d, val=%s
//
//	The Value holds a string, which encodes multiple name=value
//	pairs.... shorthand for setting many attributes.
//
CReturn 
CAttribProcessApp::SetMulti( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID			id;
	CString		multi_str;
	
	ret += io_cmd->getInt( "id", (int*)&id );
	ret += io_cmd->getString( "val", &multi_str);
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}


	// -----------------------------------------------------
	//		Set the attribute(s)...
	//
	CDbEntity*	db_ent;

	ret += model.EntityFind( id, &db_ent );
	if (!db_ent)
		ret.Internal( IDS_ENTITY_NO_EXIST, id );
	else
		ret = db_ent->setMulti( multi_str );

	return ret;
}

// ==================================================================
// Attrib:DefGet:
//			name		( $string, #integer, real)
//
//	returns: val
//
CReturn 
CAttribProcessApp::DefaultGetAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	ret += do_get_att( model.Default(), name, io_cmd );

	return ret;
}

// ==================================================================
// Attrib:DefSet:
//			name		( $string, #integer, real)
//			val
//
// NOTE: Calling this function causes the given attributes to be
// attached to the parent model.  Therefore, any that is created
// in the model after this point will 'inherit' these attributes
// from the parent model.  This can be seen in CDbEntity::CDbEntity().
//
CReturn 
CAttribProcessApp::DefaultSetAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	ret += do_set_att( model.pDefault(), name, io_cmd );

	return ret;
}

// ==================================================================
// Attrib:DefDel:
//			name		( $string, #integer, real, or "all")
//
CReturn 
CAttribProcessApp::DefaultDelAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	ret += do_del_att( model.pDefault(), name );

	return ret;
}

// ==================================================================
// Attrib:HeadCount:
//
//	returns: count
//
CReturn 
CAttribProcessApp::HeaderCountAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();

	io_cmd->setInt( "count", model.Header().countVar() );

	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Attrib:HeadGet:name=%s
//	gets the named attributed
//	where the prefix character of 'name' affects the result type ($string, #integer, real)
//	returns: val
//
// Attrib:HeadGet:indx=%d
//	gets the ith attribute
//	returns: (s) name, (s) val, (i) type
//
CReturn 
CAttribProcessApp::HeaderGetAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name = io_cmd->VarList().getString( "name", "" );
	int indx = io_cmd->VarList().getInt( "indx", -1 );

	// See also CAttribProcessApp::HeaderSetAtt()
	//
	//      CModel& model = io_cmd->getModel();
	//
	CModel& model = (*CPortal::GlobalModel());

	if (indx >= 0)
	{
		CVar* var = model.Header().getVar( indx );
		io_cmd->setString( "name", var->getName() );
		io_cmd->setString( "val", var->getString() );
		io_cmd->setInt( "type", var->getType() );
	}
	else if ( !name.IsEmpty() )
	{
		ret += do_get_att( model.Header(), name, io_cmd );
	}
	else
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
	}


	return ret;
}

// ==================================================================
// Attrib:HeadSet:
//			name		( $string, #integer, real)
//			val
//
CReturn 
CAttribProcessApp::HeaderSetAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE  NOTE
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// This is a little difficult to explain ....
	// Fab really only knows about the global_model (as defined
	// in CPortal, and said model is the model that is usually referenced
	// by the CCommand system.  However, during code generation, particularly
	// the Code Viewer, the global_model is swapped-out because the contents
	// of the global_model must be copied to a temporary model (a side effect
	// of introducing patterns & instances).  So, prior to this change, when
	// you executed the following sequence 1) select the Refresh button of the
	// Code Viewer 2) change coding parameters via frmCodeGen  3) select the
	// Refresh button again, the parameters you changed would not appear in
	// the cnc code.
	//
	// Reiterating, Fab really only knows about the global_model.  As such,
	// now we simply update the header of the global_modal.
	//
	// See also CAttribProcessApp::HeaderGetAtt()
	//
	//      CModel& model = io_cmd->getModel();
	//
	CModel& model = (*CPortal::GlobalModel());

	ret += do_set_att( model.pHeader(), name, io_cmd );

	return ret;
}

// ==================================================================
// Attrib:DefDel:
//			name		( $string, #integer, real, or "all")
//
CReturn 
CAttribProcessApp::HeaderDelAtt( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	CString	name;
	
	ret += io_cmd->getString( "name", &name );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_ATTRIB_PARAM_MISSING );
		return ret;
	}

	ret += do_del_att( model.pHeader(), name );

	return ret;
}


// ==================================================================
// Attrib:HeadVarsetExtract: class = string
// ==================================================================
//
// NOTE: HeadVarsetExtract() and HeaderVarNameGet() are coupled.
// HeadVarsetExtract() is used to extract from the model header,
// the set of all variables belonging to the same class.
// The global variable 'global_headerVarset' contains the
// extracted results.  HeaderVarNameGet() is used to get
// the name of the Ith variable in the set.  HeaderGetAtt()
// is then used to get the actual variable.
//
CReturn 
CAttribProcessApp::HeaderVarsetExtract( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn ret;

	CString	className;
	ret += io_cmd->getString( "class", &className );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::HeaderVarsetExtract()" );
		return ret;
	}

	global_headerVarset = VarsetExtract( model.Header(), className );

	io_cmd->setInt( "count", global_headerVarset.countVar() );

	return ret;
}

// ==================================================================
// Attrib:HeadVarNameGet: index = int
// ==================================================================
//
// NOTE: HeadVarsetExtract() and HeaderVarNameGet() are coupled.
// HeadVarsetExtract() is used to extract from the model header,
// the set of all variables belonging to the same class.
// The global variable 'global_headerVarset' contains the
// extracted results.  HeaderVarNameGet() is used to get
// the name of the Ith variable in the set.  HeaderGetAtt()
// is then used to get the actual variable.
//
CReturn 
CAttribProcessApp::HeaderVarNameGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	int index;
	ret += io_cmd->getInt( "index", &index );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::HeaderVarNameGet()" );
		return ret;
	}

	CVar* var = global_headerVarset.getVar( index );
	CString name = ((var == NULL) ? "" : var->getName());

	io_cmd->setString( "name", name );

	return ret;
}


// ==================================================================

CReturn
CAttribProcessApp::do_get_att(
	const CVarList&	in_att,
	const CString&	in_name,
	CCommand*		io_cmd )
{
	CReturn		ret;

	if (in_name.GetLength() < 2)
		return CReturn( STATUS_ERROR );

	if ( in_name[0] == INT_PREFIX )
	{
		int	ival = 0;

		ret += in_att.getInt( in_name.Mid( 1 ), &ival );
		io_cmd->setInt( "val", ival );
	}
	else
	if ( in_name[0] == STRING_PREFIX )
	{
		CString	sval = "";

		ret += in_att.getString( in_name.Mid( 1 ), &sval );
		io_cmd->setString( "val", sval );
	}
	else
	{
		double	rval = 0.0;

		ret += in_att.getReal( in_name, &rval );
		io_cmd->setReal( "val", rval );
	}

	return ret;
}


// ==================================================================

CReturn
CAttribProcessApp::do_set_att(
	CVarList*		io_att,
	const CString&	in_name,
	CCommand*		io_cmd )
{
	CReturn		ret;

	if (in_name.GetLength() < 2)
		return CReturn( STATUS_ERROR );

	if ( in_name[0] == INT_PREFIX )
	{
		int	ival;

		ret += io_cmd->getInt( "val", &ival );
		io_att->setInt( in_name.Mid( 1 ), ival );
	}
	else
	if ( in_name[0] == STRING_PREFIX )
	{
		CString	sval;

		ret += io_cmd->getString( "val", &sval );
		io_att->setString( in_name.Mid( 1 ), sval );
	}
	else
	{
		double	rval;

		ret += io_cmd->getReal( "val", &rval );
		io_att->setReal( in_name, rval );
	}

	return ret;
}

// ==================================================================

CReturn
CAttribProcessApp::do_del_att(
	CVarList*		io_att,
	const CString&	in_name )
{
	CReturn		ret;

	if (in_name.GetLength() < 2)
		return CReturn( STATUS_ERROR );

	if ( in_name.CompareNoCase( "all" ) == 0)
	{
		// Special case where we are deleting ALL attributes in the list
		io_att->Reset();
	}
	else if ( in_name[0] == INT_PREFIX )
	{
		io_att->deleteVar( in_name.Mid( 1 ) );
	}
	else if ( in_name[0] == STRING_PREFIX )
	{
		io_att->deleteVar( in_name.Mid( 1 ) );
	}
	else
	{
		io_att->deleteVar( in_name );
	}

	return ret;
}



// ==================================================================
// NOTE:  T ags all children of Profiles or Features it affects...
//
CReturn
CAttribProcessApp::do_feature_att(
	const CModel&	model,
	CDbEntity*		io_ent,
	ID				in_feature )	// If re-tooling, destination tool feature
{
	CReturn		ret;

	if (in_feature <= 0)
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::do_feature_att()" );
		return ret;
	}

	//
	// Refuse to re-feature tooled features...
	//
	CDbFeature*	feature = dynamic_cast<CDbFeature*>( io_ent );
	if (feature)
		return ret;

	// 
	// Get the new parent feature
	//
	CDbEntity*	parent= NULL;
	ret += model.EntityFind( in_feature, &parent, DBFEATURE, DBFEATURE );

	feature = dynamic_cast<CDbFeature*>( parent );
	if (feature == NULL)
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::do_feature_att()" );
		return ret;
	}

	//
	// Now, re-parent this entity to the given feature
	//
	feature->Append( io_ent );

	//
	// If our entity is a container, tag the children,
	//	AND, move them to the proper tool (if relevant)
	//
	CDbProfile* profile = dynamic_cast<CDbProfile*>( io_ent );
	if (profile)
	{
		CDbEntity*	db_ent;

		int num = profile->Count();
		for (int idx=0; idx<num; idx++)
		{
			db_ent = (*profile)[idx];
			if (db_ent)
			{
				db_ent->Tool( profile->Tool() );
				db_ent->DoAction();
			}
		}
	}

	return ret;
}



// ==================================================================

CVarList
CAttribProcessApp::VarsetExtract( const CVarList& header, const CString& className )
{
	CVarList varset;

	CString prefix = className + ".";

	int count = header.countVar();
	for (int indx = 0; indx < count; ++indx)
	{
		CVar* var = header.getVar( indx );
		const CString& name = var->getName();

		if (name.Find( prefix ) == 0)
		{
			varset.setVar( (*var) );
		}
	}
	
	return varset;
}



// ==================================================================
//	Attrib:UpdateExternal:
//
//	Scans the entire model and, for each tooled profile, marks that
//	profile (NOT it's contents) as Attribute External =1 or =0
//
CReturn 
CAttribProcessApp::UpdateExternal( 
	CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector selector(model);

	selector.All( 0 );
	selector.Filter( DBPROFILE, 1 );
	selector.Restrictions( TRUE );
	selector.SelectAll( TRUE );

	CDbEntityList proflist;

	status += CLead::Reduce( selector, TRUE, &proflist );
	status += CModelUtil::MarkIntExt( &proflist, TRUE, &model );

	selector.Clear();

	return status;
}

// Attrib:Selected:Count:
// returns (i) count
CReturn 
CAttribProcessApp::SelectedCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CVarList		mirror;
	CVarList*		refAttribs;
	CVar*			attrib;
	CVar*			temp;
	CDbEntity*		dbEntity;
	int				sndx;
	int				acnt, andx;
	int				count;

	CModel&			model = io_cmd->getModel();
	CSelectorStack&	selectorStack = model.SelectorStack();
	CSelector&		selector = selectorStack();

	// Find the instance count of each named attribute among
	// the selected entities.  Points are ignored because
	// they are never referenced by the user.
	refAttribs = NULL;
	count = selector.Count();
	for (sndx = 0; sndx < count; ++sndx)
	{
		dbEntity = selector[sndx];
		if (dbEntity->Type() > DBPOINT)
		{
			if (refAttribs == NULL)
				refAttribs = dbEntity->pAttrib();

			acnt = dbEntity->Attrib().countVar();
			for (andx = 0; andx < acnt; ++andx)
			{
				attrib = dbEntity->Attrib().getVar(andx);
				const CString name = attrib->getName();
				// NOTE: Attribute names starting with the underscore
				// character are deemed to be 'system' attributes and
				// therefore should not be visible to the user.
				if (name.GetAt(0) != '_')
				{
					count = mirror.getInt( name, 0 ) + 1;
					mirror.setInt( name, count );
				}
			}
		}
	}

	// Build the final list of attributes common to the entities.
	global_attribs.Reset();
	acnt = mirror.countVar();
	for (andx = 0; andx < acnt; ++andx)
	{
		attrib = (CVarInt*) mirror.getVar(andx);
		if (attrib->getInt() > 0)
		{
			temp = refAttribs->getVar( attrib->getName() );
			global_attribs.setVar( (*temp) );
		}
	}

	// Return the count of common attributes.
	io_cmd->setInt( "count", global_attribs.countVar() );

	return status;
}

// Attrib:Selected:Get:indx=%d
// returns (s) name, (i) type, (s) value
CReturn 
CAttribProcessApp::SelectedGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CVar*	attrib;
	int		indx;

	indx = io_cmd->VarList().getInt( "indx", -1 );

	if (indx < 0 || indx >= global_attribs.countVar())
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::SelectedGet() -- bad indx" );
		io_cmd->setString( "name", "" );
		io_cmd->setInt( "type", VAR_NONE );
		io_cmd->setString( "value", "" );
	}
	else
	{
		attrib = global_attribs.getVar(indx);
		io_cmd->setString( "name", attrib->getName() );
		io_cmd->setInt( "type", attrib->getType() );
		io_cmd->setString( "value", attrib->getString() );
	}

	return status;
}

// Attrib:Selected:Set:name=%s, type=%d, value=%s
CReturn 
CAttribProcessApp::SelectedSet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CString		name;
	CString		value;
	CDbEntity*	dbEntity;
	int			count, indx;
	int			type;

	CModel&			model = io_cmd->getModel();
	CSelectorStack&	selectorStack = model.SelectorStack();
	CSelector&		selector = selectorStack();

	name = io_cmd->VarList().getString( "name", "" );
	type = io_cmd->VarList().getInt( "type", VAR_NONE );
	value = io_cmd->VarList().getString( "value", "" );

	if (name.IsEmpty() || value.IsEmpty() || type == VAR_NONE)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::SelectedSet() -- bad parameter" );
	}
	else
	{
		count = selector.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = selector[indx];
			if (dbEntity->Type() > DBPOINT)
			{
				if (type == VAR_INT)
					dbEntity->IntSet( name, atoi(value) );
				else if (type == VAR_REAL)
					dbEntity->DoubleSet( name, atof(value) );
				else if (type == VAR_STRING)
					dbEntity->StringSet( name, value );
			}
		}
	}

	return status;
}

// Attrib:Selected:Delete:name=%s
CReturn 
CAttribProcessApp::SelectedDelete( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CString		name;
	CDbEntity*	dbEntity;
	int			count, indx;

	CModel&			model = io_cmd->getModel();
	CSelectorStack&	selectorStack = model.SelectorStack();
	CSelector&		selector = selectorStack();

	name = io_cmd->VarList().getString( "name", "" );

	if ( name.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::SelectedDelete() -- bad parameter" );
	}
	else
	{
		count = selector.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = selector[indx];
			if (dbEntity->Type() > DBPOINT)
			{
				dbEntity->AttribDelete( name );
			}
		}
	}

	return status;
}

// ============================================================================

CReturn CAttribProcessApp::do_set_chain( CDbEntity* db_ent, int val )
{
	CReturn ret;

	CVarList* var = db_ent->pAttrib();

	if (val)
		var->setInt("chain", 1);
	else
		var->deleteVar("chain");

	return ret;
}
