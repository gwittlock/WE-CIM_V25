
#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"

#include "cmn_resource.h"
#include "DbEntity.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "Model.h"
#include "EntityProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDbEntityList	g_refsTo;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Type: id = %d
// Returns type = %d
//
CReturn 
CEntityProcessApp::EntityType( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityType()" );
	else
		io_cmd->setInt( "type", dbEntity->Type() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:VBType: id = %d
// Returns type = %s
//
CReturn 
CEntityProcessApp::EntityVBType( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CString vbType;
	CString sval;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityVBType()" );
	}
	else
	{
		switch ( dbEntity->Type() )
		{
		case DBWORKPLANE:
			vbType = "Workplane";
			break;
		case DBTOOL:
			{
				CDbTool* dbTool = (CDbTool*) dbEntity;
				if ( dbTool->IsLayer() )
					vbType = "Layer";
				else
					vbType = "Tool";
			}
			break;
		case DBPOINT:
			vbType = "Point";
			break;
		case DBLINE:
			vbType = "Line";
			break;
		case DBARC:
			vbType = "Arc";
			break;
		case DBHOLE:
			sval = dbEntity->StringGet( STR_TYPE, "" );
			if (sval.CompareNoCase("_pierce") == 0)
				vbType = "Pierce";
			else
				vbType = "Hole";
			break;
		case DBPROFILE:
			vbType = "Profile";
			break;
		case DBCOMMAND:
			sval = dbEntity->StringGet( STR_TYPE, "" );
			if (sval.CompareNoCase("_instance") == 0)
				vbType = "Instance";
			else if (sval.CompareNoCase("_insert") == 0)
				vbType = "Insert";
			else
				vbType = "Command";
			break;
		case DBFEATURE:
			sval = dbEntity->StringGet( STR_TYPE, "" );
			if (sval.CompareNoCase("_zone") == 0)
				vbType = "WorkZone";
			else if (sval.CompareNoCase("_lead_in") == 0)
				vbType = "LeadIn";
			else if (sval.CompareNoCase("_lead_out") == 0)
				vbType = "LeadOut";
			else if (sval.CompareNoCase("_part") == 0)
				vbType = "Part";
			else
				vbType = "Feature";
			break;
		case DBSEQUENCE:
			sval = dbEntity->StringGet( STR_TYPE, "" );
			if (sval.CompareNoCase("_root") == 0)
				vbType = "RootSeq";
			else if (sval.CompareNoCase("_workzone") == 0)
				vbType = "WorkSeq";
			else
				vbType = "Sequence";
			break;
		case DBPATTERN:
			vbType = "Pattern";
			break;
		default:
			break;
		}
	}

	io_cmd->setString( "type", vbType );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Name: id = %d
//    Returns name = %s
//
// Entity:Name: id = %d, name = %s
//    Returns bool = %d
//
CReturn 
CEntityProcessApp::EntityName( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name;
	io_cmd->getString( "name", &name );

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityName()" );
	}
	else
	{
		if (name.GetLength() > 0)
		{
			CModel&	model = io_cmd->getModel();

			model.UndoBufferPrepare();
			bool okay = dbEntity->Name( name );
			model.UndoBufferCommit();

			io_cmd->setInt( "bool", (okay ? 1 : 0) );
		}
		else
		{
			io_cmd->setString( "name", dbEntity->Name() );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Work: id = %d
// Returns id = %d
//
CReturn 
CEntityProcessApp::EntityWork( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityWork()" );
	}
	else
	{
		CDbWorkplane* dbWork = dbEntity->Workplane();
		io_cmd->setInt( "id", ((dbWork == NULL) ? 0 : dbWork->Id()) );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Tool: id = %d
// Entity:Tool: id = %d [, tool = %d]
// Returns id = %d
//
CReturn 
CEntityProcessApp::EntityTool( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbEntity*	dbEntity;
	CDbTool*	dbTool;
	ID			toolID;

	dbTool = NULL;
	dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityTool()" );
	}
	else
	{
		toolID = io_cmd->VarList().getInt( "tool", 0 );
		if (toolID > 0)
		{
			// Assigning a tool.
			io_cmd->getModel().EntityFind( toolID, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

			if (dbTool == NULL)
				status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityTool()" );
			else
				dbEntity->Tool( dbTool );
		}
		else
		{
			// Getting the tool.
			dbTool = dbEntity->Tool();
		}
	}

	io_cmd->setInt( "id", ((dbTool == NULL) ? 0 : dbTool->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:RefCnt: id = %d
// Returns count = %d
//
CReturn 
CEntityProcessApp::EntityRefCnt( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityRefCnt()" );
	}
	else
	{
		io_cmd->setInt( "count", dbEntity->RefCnt() );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Box: id = %d
// Returns xmin, ymin, zmin, xmax, ymax, zmax = %f
//
CReturn 
CEntityProcessApp::EntityBox( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityBox()" );
	}
	else
	{
		C3dBox box = dbEntity->Box();

		io_cmd->setReal( STR_XMIN, box.Xmin() );
		io_cmd->setReal( STR_YMIN, box.Ymin() );
		io_cmd->setReal( "zmin", box.Zmin() );
		io_cmd->setReal( STR_XMAX, box.Xmax() );
		io_cmd->setReal( STR_YMAX, box.Ymax() );
		io_cmd->setReal( "zmax", box.Zmax() );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:IsLead: id = %d, type = %d
// Returns bool = %d
//
CReturn 
CEntityProcessApp::EntityIsLead( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbFeature*	dbFeature;
	CDbEntity*	dbEntity;
	int			type;
	bool		is_lead;

	is_lead = FALSE;

	dbEntity = EntityGet( io_cmd );
	type = io_cmd->VarList().getInt( "type", -1 );

	if (dbEntity == NULL || type < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::EntityIsLead()" );
	}
	else
	{
		// NOTE: dbEntity can represent either a curve or a feature.
		// Typical db structure is
		//   e.g.
		//   Feature
		//     Lead In (is a feature)
		//       Line
		//     Profile
		//       Line
		//       Line
		//       Line
		//       Line
		//     Lead Out (is a feature)
		//       Line
		//
		dbFeature = dynamic_cast<CDbFeature*>( dbEntity );
		if (dbFeature == NULL)
		{
			dbFeature = dynamic_cast<CDbFeature*>( dbEntity->Owner() );
			if (dbFeature != NULL)
				is_lead = IsLead( (*dbFeature), type );
		}
		else
		{
			is_lead = IsLead( (*dbFeature), type );
		}
	}

	io_cmd->setInt( "bool", is_lead );

	return status;
}

// ==================================================================
// Entity:ViewBehavior: id=%d[,snap=%b][,dot=%b]
// At first implementation, applies only to tool (layer) entities.
//
CReturn 
CEntityProcessApp::ViewBehavior( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	const CVarList& params = io_cmd->VarList();
	CModel&	model = io_cmd->getModel();

	ID id = params.getInt( "id", 0 );
	int snap = params.getInt( "snap", -1 );
	int dot = params.getInt( "dot", -1 );

	CDbTool* dbTool;
	status = model.EntityFind( id, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	if ( status.IsOk() )
	{
		if (snap >= 0)
			dbTool->SnappableFlag( (snap > 0) );

		if (dot >= 0)
			dbTool->HotDotFlag( (dot > 0) );
	}

	return status;
}

// ==================================================================
// Entity:Hide: id=%d
//
CReturn 
CEntityProcessApp::Hide( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID id;
	
	ret += io_cmd->getInt( "id", (int*)&id );
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
		ret.Internal( IDS_ENTITY_NO_EXIST, id );
	else
		db_ent->Hide();

	return ret;
}

// ==================================================================
// Entity:Show: id=%d
//
CReturn 
CEntityProcessApp::Show( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID id;
	
	ret += io_cmd->getInt( "id", (int*)&id );
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
		ret.Internal( IDS_ENTITY_NO_EXIST, id );
	else
		db_ent->Seek();

	return ret;
}

// ==================================================================
// Entity:IsHidden: id=%d
//
//	returns: val
//
CReturn 
CEntityProcessApp::IsHidden( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// -----------------------------------------------------
	//		Extract command
	//
	ID			id;
	
	ret += io_cmd->getInt( "id", (int*)&id );
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
		ret.Internal( IDS_ENTITY_NO_EXIST, id );
	else
		io_cmd->setInt( "val", db_ent->IsHidden() );

	return ret;
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:System: id=%d
// NOTE: The system flag is not an attribute like those that
// we attach to entities, but it is conceptually an attribute.
CReturn 
CEntityProcessApp::System( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::System()" );
	else
		dbEntity->SystemFlag( true );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:User: id=%d
// NOTE: The system flag is not an attribute like those that
// we attach to entities, but it is conceptually an attribute.
CReturn 
CEntityProcessApp::User( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	if (dbEntity == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::System()" );
	else
		dbEntity->SystemFlag( false );

	return status;
}

CReturn 
CEntityProcessApp::RefsToInit( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );

	g_refsTo.BenignFlush();

	if (dbEntity == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::RefsToInit()" );
	else
		dbEntity->RefsTo( &g_refsTo );

	return status;
}

CReturn 
CEntityProcessApp::RefsToCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	io_cmd->setInt( "count", g_refsTo.Count() );

	return status;
}

CReturn 
CEntityProcessApp::RefsToGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int	indx = io_cmd->VarList().getInt( "indx", -1 );
	int	id = 0;

	id = ((indx < 0 || indx > g_refsTo.Count()) ? 0 : g_refsTo[indx]->Id());

	if (id == 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::RefsToGet()" );
	}

	io_cmd->setInt( "id", id );

	return status;
}

CReturn 
CEntityProcessApp::RefsToTerm( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	g_refsTo.BenignFlush();
	return status;
}

bool 
CEntityProcessApp::IsLead( const CDbEntity& dbEntity, int type )
{
	CString	sval;
	bool	is_lead;

	is_lead = FALSE;
	
	sval = dbEntity.StringGet( STR_TYPE, "" );
	if ( !sval.IsEmpty() )
	{
		if (type == 0)
			is_lead = (sval.CompareNoCase( "_lead_in" ) == 0);
		else
			is_lead = (sval.CompareNoCase( "_lead_out" ) == 0);
	}

	return is_lead;
}

