
#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"
#include "DbEntity.h"
#include "DbWorkplane.h"
#include "DbContainer.h"
#include "Model.h"
#include "EntityProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:Count: id = %d
// Returns count = %d
//
CReturn CEntityProcessApp::ContainerCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

#if 0
	// This branch has been disabled because of the myriad messages generated.
	// It seems the app (wrongly) calls this portal command on every entity
	// when the model view is opened.
	if (dbContainer == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerCount()" );
	else
		io_cmd->setInt( "count", dbContainer->Count() );
#else
	if (dbContainer == NULL)
		status.setStatus( STATUS_ERROR );
	else
		io_cmd->setInt( "count", dbContainer->Count() );
#endif

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:Get: id = %d, index = %d
//    Returns id = %d
//
CReturn CEntityProcessApp::ContainerGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	int index;

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

	status = io_cmd->getInt( "index", &index );

	if (dbContainer == NULL || !status.IsOk())
	{
		status.setStatus( STATUS_ERROR );
	}
	else
	{
		int count = dbContainer->Count();
		if (index >= 0 && index < count)
		{
			dbEntity = (*dbContainer)[ index ];
			io_cmd->setInt( "id", dbEntity->Id() );
		}
		else
			status.setStatus( STATUS_ERROR );
	}

#if 0
	// Disabled because of all the EWM output generated. At this point, I dunno
	// why so much output is generated but I surmise it's because the app is
	// not guarding against inappropriate use (eg. when opening the list view).
	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerGet()" );
#endif
	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:Set: id = %d, index = %d, entid=%d
//    Where: id = container id
//           index = position in container
//           entid = id of entity to place in container
//    Returns id = %d
//
CReturn CEntityProcessApp::ContainerSet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();
	const CVarList& args = io_cmd->VarList();

	int index = args.getInt( "index", -1 );
	int entid = args.getInt( "entid", -1 );

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

	dbEntity = NULL;
	model.EntityFind( entid, &dbEntity, DBPOINT, DBFEATURE );

	if ((dbContainer == NULL) || (index < 0) || (dbEntity == NULL))
	{
		status.setStatus( STATUS_ERROR );
	}
	else
	{
		int count = dbContainer->Count();
		if ((index >= 0) && (index < count))
		{
			CDbEntity* prev = dbContainer->ReplaceAt( index, dbEntity );
			io_cmd->setInt( "id", ((prev == NULL) ? 0 : prev->Id()) );
		}
		else
			status.setStatus( STATUS_ERROR );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerSet()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:Append: id = %d, newid = %d
// Returns count = %d
//
CReturn CEntityProcessApp::ContainerAppend( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	ID newId;

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

	status = io_cmd->getInt( "newid", (int*) &newId );

	if (dbContainer == NULL || !status.IsOk())
	{
		status.setStatus( STATUS_ERROR );
	}
	else
	{
		CModel&	model = io_cmd->getModel();

		model.EntityFind( newId, &dbEntity, DBWORKPLANE, DBFEATURE );

		if (dbEntity == NULL)
		{
			status.setStatus( STATUS_ERROR );
		}
		else
		{
			dbContainer->Append( dbEntity, FALSE );
			io_cmd->setInt( "count", dbContainer->Count() );
		}
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerAppend()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:InsertBefore: id = %d, refid = %d, newid = %d
// Returns index = %d
//
CReturn CEntityProcessApp::ContainerInsertBefore( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	ID refId;
	ID newId;

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

	status  = io_cmd->getInt( "refid", (int*) &refId );
	status += io_cmd->getInt( "newid", (int*) &newId );

	if (dbContainer == NULL || !status.IsOk())
	{
		status.setStatus( STATUS_ERROR );
	}
	else
	{
		CModel&	model = io_cmd->getModel();

		CDbEntity* dbRefEntity;
		model.EntityFind( refId, &dbRefEntity, DBWORKPLANE, DBFEATURE );

		CDbEntity* dbNewEntity;
		model.EntityFind( newId, &dbNewEntity, DBWORKPLANE, DBFEATURE );

		if (dbRefEntity == NULL || dbNewEntity == NULL)
		{
			status.setStatus( STATUS_ERROR );
		}
		else
		{
			int indx = dbContainer->InsertBefore( dbRefEntity, dbNewEntity );
			io_cmd->setInt( "index", indx );
		}
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerInsertBefore()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:InsertAfter: id = %d, refid = %d, newid = %d
// Returns index = %d
//
CReturn CEntityProcessApp::ContainerInsertAfter( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	ID refId;
	ID newId;

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

	status  = io_cmd->getInt( "refid", (int*) &refId );
	status += io_cmd->getInt( "newid", (int*) &newId );

	if (dbContainer == NULL || !status.IsOk())
	{
		status.setStatus( STATUS_ERROR );
	}
	else
	{
		CModel&	model = io_cmd->getModel();

		CDbEntity* dbRefEntity;
		model.EntityFind( refId, &dbRefEntity, DBWORKPLANE, DBFEATURE );

		CDbEntity* dbNewEntity;
		model.EntityFind( newId, &dbNewEntity, DBWORKPLANE, DBFEATURE );

		if (dbRefEntity == NULL || dbNewEntity == NULL)
		{
			status.setStatus( STATUS_ERROR );
		}
		else
		{
			int indx = dbContainer->InsertAfter( dbRefEntity, dbNewEntity );
			io_cmd->setInt( "index", indx );
		}
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerInsertAfter()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:Disown: id = %d, refid = %d
//
CReturn CEntityProcessApp::ContainerDisown( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CDbEntity* dbEntity = NULL;

	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( EntityGet( io_cmd ) );

	ID refid = io_cmd->VarList().getInt( "refid", 0 );
	if (refid > 0)
	{
		CModel&	model = io_cmd->getModel();
		model.EntityFind( refid, &dbEntity, DBLINE, DBFEATURE );
	}

	if (dbContainer == NULL || dbEntity == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerCount()" );
		io_cmd->setInt( "bool", FALSE );
	}
	else
	{
		io_cmd->setInt( "bool", dbContainer->Disown( dbEntity ) );
	}

	io_cmd->setInt( "count", dbContainer->Count() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:BenignFlush: id = %d
//
// BEWARE: This method (introduced for Java) does not
// orphan contained entities from their sequencee objects!
//
CReturn CEntityProcessApp::ContainerBenignFlush( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

	if (dbContainer == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerBenignFlush()" );
	else
		dbContainer->BenignFlush();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Entity:Container:DestructiveFlush: id = %d
CReturn CEntityProcessApp::ContainerDestructiveFlush( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = EntityGet( io_cmd );
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );

	if (dbContainer == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CEntityProcessApp::ContainerDestructiveFlush()" );
	else
		dbContainer->DestructiveFlush();

	return status;
}


