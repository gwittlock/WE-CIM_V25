
// ============================================================================
//	CPPattern
//
//	An obvious clone of CPFeature
//
// TODO:  Merge with CPFeature but switch-set between Feature and Pattern??
//	This seems terribly wasteful here.
//
// ============================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"
#include "StringConst.h"

#include "DbTool.h"
#include "DbCurve.h"
#include "DbCurveList.h"
#include "DbFeature.h"
#include "DbPattern.h"
#include "DbCommand.h"
#include "DbIterator.h"
#include "Model.h"
#include "ModelUtil.h"

#include "mm2.h"
#include "path.h"

#include "ViewMgr.h"
#include "DisplayEntity.h"

#include "CreateProcess.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Pattern:Create: [add = %d] [file=%s]
//
//    add  -- The id of an entity to be added to the pattern; -1 for selection
//	  file -- The filename to import as a pattern
//
//    Returns id.
//
CReturn 
CCreateProcessApp::PatternCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();

	CDbPattern* dbPattern = NULL;

	int id = 0;
	CString filename = "";

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "add", (int*)&id );
	io_cmd->getString( "file", &filename );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Create the Feature.
	status = CModelUtil::PatternCreate( &(model.Db()), &dbPattern );

	if ( !status.IsOk() )
		return status;

	// 2004.09.20 (PE) -- must uniquely identify the pattern so
	// that CNestProcessApp::SimilarPatternFind() can locate it.
	// NOTE: We use a negative id because pattern created by the
	// nesting engine have positive ids.
	dbPattern->IntSet( "part_id", (-(int)dbPattern->Id()) );

	if (id > 0)
	{
		CDbEntity* dbEntity;
		model.EntityFind( id, (CDbEntity**) &dbEntity, DBPOINT, DBFEATURE );
		if (dbEntity == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::PatternCreate()" );
		}
		else
		{
			status = dbPattern->Append( dbEntity, false );
		}
	}
	else if (id < 0)
	{
		CSelectorStack& selectorStack = model.SelectorStack();
		CSelector& selector = selectorStack();

		CDbEntity::NewAction();

		int num = selector.Count();
		if (num)
		{
			for (int idx = 0; idx<num; idx++)
			{
				CDbEntity* db_ent = selector[idx];
				if (db_ent->IsDeleted())
				{ continue; }

				while ( db_ent->Owner()
						&& db_ent->Owner()->IsSelected() )
				{
					db_ent = db_ent->Owner();
				}

				if (!db_ent->DidAction())
				{
					dbPattern->Append( db_ent, false );
					db_ent->DoAction();
				}
			}
			selector.Clear();
		}
	}
	else if (filename.GetLength() > 2)  // No ID
	{
		CMM2 file;

		status += file.Merge( filename, C3dCoord(0,0,0), &model, dbPattern );
	}

	if ( status.IsOk() )
		io_cmd->setInt( "id", dbPattern->Id() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Pattern:Modify: id = %d
//				[,flush = %d] [,add = %d]
//				[,before = %d] [,after = %d]
//
//		id		-- The id of the feature to find.
//		flush	-- [0] false / [1] true, detroy the contents.
//					Defaults to false.
//		toolid	-- The id of the tool to which this feature is associated.
//					An id of zero implies this is a part feature.
//		copy	-- [0] false / [1] true, add a copy the selected
//					entities to the feature.  Defaults to false.
//		add		-- The id of an entity to be added to the feature.
//					Any entity (except a feature) that is added to this
//					feature, will inherit the tool id from this feature.
//		before -- The id of an entity in the feature to add before
//		after  -- The id of an entity in the feature to add after
//					NOTE:  only specify one, before or after, only if add != 0
//
//    Returns id.
//
CReturn 
CCreateProcessApp::PatternModify( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbPattern* dbPattern = NULL;

	ID id = 0;
	ID add = 0;
	ID before = 0;
	ID after = 0;
	int flush = 0;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getInt( "add", (int*)&add );
	io_cmd->getInt( "before", (int*)&before );
	io_cmd->getInt( "after", (int*)&after );
	io_cmd->getInt( "flush", &flush );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Pattern.
	status = CModelUtil::PatternFind( model.Db(), id, &dbPattern );

	if ( !status.isOkay() )
		return status;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Manipulate the Pattern
	if ( flush )
		dbPattern->DestructiveFlush();

	if (add > 0)
	{
		CDbEntity* dbEntity;
		model.EntityFind( add, (CDbEntity**) &dbEntity );
		if (dbEntity == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::PatternModify()" );
		}
		else
		{
			CDbEntity*	dbBefore = NULL;
			CDbEntity*	dbAfter = NULL;
			
			if (before)
				model.EntityFind( before, (CDbEntity**) &dbBefore );
			if (after)
				model.EntityFind( after, (CDbEntity**) &dbAfter );

			if (dbBefore)
			{
				if (dbPattern->InsertBefore( dbBefore, dbEntity ) < 0)
					status += CReturn( STATUS_ERROR );
			}
			else
			if (dbAfter)
			{
				if (dbPattern->InsertAfter( dbAfter, dbEntity ) < 0)
					status += CReturn( STATUS_ERROR );
			}
			else
				status += dbPattern->Append( dbEntity, false );
		}
	}

	CModelUtil::EmptyContainers( model, false );


	if ( status.IsOk() )
		io_cmd->setInt( "id", dbPattern->Id() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Pattern:Count: id = %d
//
CReturn 
CCreateProcessApp::PatternCount( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbPattern* dbPattern = NULL;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	ID id = io_cmd->VarList().getInt( "id", 0 );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Pattern.
	status = CModelUtil::PatternFind( model.Db(), id, &dbPattern );	

	io_cmd->setInt( "count", ((dbPattern == NULL) ? 0 : dbPattern->Count()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Pattern:Entity: id = %d, index = %d
//
//     Returns entity id.
//
CReturn 
CCreateProcessApp::PatternEntity( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbPattern* dbPattern  = NULL;

	ID id = 0;
	int indx = -1;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getInt( "index", &indx );

	if (indx < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::PatternEntity()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Pattern.
	status = CModelUtil::PatternFind( model.Db(), id, &dbPattern );	
	
	if ( status.IsOk() )
	{
		CDbEntity* dbEntity = (*dbPattern)[ indx ];

		if (dbEntity == NULL)
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::PatternEntity()" );
		else
			io_cmd->setInt( "id", dbEntity->Id() );
	}

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Pattern:Edit: id = %d
//
//	Pass the ID of a CPattern entity, or 0 to return to the model
//
CReturn 
CCreateProcessApp::PatternEdit( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbWorkplane*	dbWork;

	CModel&	model = io_cmd->getModel();

	ID id = 0;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );

	CDbPattern* dbPattern = NULL;
	if (id)
	{
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Fetch the Pattern.
		status = CModelUtil::PatternFind( model.Db(), id, &dbPattern );	
	}

	model.ActivePattern(dbPattern);

	model.EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		model.ActiveWorkplane( dbWork );

	io_cmd->getViewMgr().ModelSet( model );
	io_cmd->getViewMgr().Refresh( true );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Pattern:Instance: x=%f, y=%f [,angle=%f], pat = %d
//
//	Returns "id" of the instance
//
CReturn 
CCreateProcessApp::PatternInstance( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	CModel&	model = io_cmd->getModel();

	ID id = 0;
	double px=0.0;
	double py=0.0;
	double ang=0.0;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	//
	io_cmd->getReal( "x", &px );
	io_cmd->getReal( "y", &py );
	io_cmd->getReal( "angle", &ang );
	io_cmd->getInt( "pat", (int*)&id );

	CDbPattern* dbPattern = NULL;
	if (id)
	{
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Fetch the Pattern.
		ret = CModelUtil::PatternFind( model.Db(), id, &dbPattern );	
	}

	if (dbPattern)
	{
		CDbTool* dbTool = model.ActiveTool();
		CDbWorkplane* dbWork = model.ActiveWorkplane();

		CDbCommand* cmd = NULL;
		ret += model.EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );

		// -----------------------------------------------------
		// Fill the contents
		//
		if (cmd)
		{
			CString inst;
			inst.Format( "@INSTANCE: instang=%f, patid=%d", ang, id );

			cmd->Init( dbTool, dbWork, C3dCoord( px, py, 0.0 ), inst );
			cmd->SystemFlag( false );

			cmd->DoubleSet( "angle", 0.0 );
			cmd->IntSet( "pos", TEXTPOS_DEFAULT );

			// Redundant, but makes life easier in VB
			cmd->StringSet( STR_TYPE, "_instance" );

			id = cmd->Id();
			io_cmd->setInt( "id", id );
//#if OKAY
			io_cmd->getViewMgr().ModelSet( model );
			io_cmd->getViewMgr().Refresh( id, TRUE );
//#endif
		}
	}
	return ret;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Pattern:Explode: [id=%d] [pat=%d] [all=%d]
//
//	Find the pattern referenced by this instance, and explode the geometry
//	to where the instance is.  Delete the instance.
//
//	NOTE: CLONE of RepoSupport explode_instance()
//
CReturn
CCreateProcessApp::PatternExplode( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;

	CModel&	model = io_cmd->getModel();

	CDbPattern* dbPattern = NULL;
	CDbCommand* db_cmd = NULL;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	//
	ID id  = io_cmd->VarList().getInt( "id", 0 );
	bool all = io_cmd->VarList().getInt( "all", FALSE );

	if ( all )
	{
		CDbIterator	iter;

		iter.Init( model.Db(), DBPATTERN );
		while (1)
		{
			dbPattern = dynamic_cast<CDbPattern*>( iter() );
			if (dbPattern == NULL)
				break;

			ret += CModelUtil::PatternExplode( (*dbPattern) );

			iter.Next();
		}
	}
	else
	{
		ret = model.Db().Find( id, (CDbEntity**)&db_cmd, DBCOMMAND, DBCOMMAND);
		if (!db_cmd)
		{
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Fetch the Pattern.
			ret = CModelUtil::PatternFind( model.Db(), id, &dbPattern );
		}

		if (db_cmd && db_cmd->IsInstance())
		{
			CDbFeature* boom;
			ret += CModelUtil::PatternExplode( &db_cmd, &boom );
		}
		else
		if (dbPattern)
		{
			ret += CModelUtil::PatternExplode( *dbPattern );
		}
	}
	return ret;
}

