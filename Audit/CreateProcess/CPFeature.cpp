
#include "stdafx.h"

#include "MathConst.h"
#include "ColorConst.h"
#include "cmn_resource.h"

#include "DbTool.h"
#include "DbCurve.h"
#include "DbCurveList.h"
#include "DbFeature.h"
#include "DbSequence.h"
#include "DbEntityVisitor.h"
#include "Model.h"
#include "ModelUtil.h"

#include "CreateProcess.h"
#include "StringConst.h"
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	const int TAG_IT = 0x01;
	const int ORPHAN_IT = 0x02;

	class CMyVisitor : public CDbEntityVisitor
	{
	public:

		CMyVisitor()
		{
			m_flags = 0;
			m_topLevel = FALSE;
		}
		
		void BehaviorSet( int flags )
		{
			m_flags = flags;
		}

		virtual ~CMyVisitor()
		{
		}

		void TopLevel( bool topLevel )
		{
			m_topLevel = topLevel;
		}

		virtual void Visit( CDbEntity* dbEntity )
		{
			if (m_flags & TAG_IT)
				dbEntity->DoAction();

			if ((m_flags & ORPHAN_IT) && m_topLevel)
			{
				CDbContainer*	dbContainer;
				CDbSequence*	dbSequence;

				// BugID: 634 -- Edit/Zone/Extract caused empty profiles.
				dbContainer = dynamic_cast<CDbContainer*>( dbEntity->Owner() );
				if (dbContainer != NULL && dbContainer->Type() != DBPROFILE)
					dbContainer->Disown( dbEntity );

				dbSequence = dbEntity->Sequence();
				if (dbSequence != NULL)
					dbSequence->Disown( dbEntity );

				m_topLevel = FALSE;
			}
		}

		int		m_flags;
		bool	m_topLevel;
	};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Feature:Create: [toolid=%d,] [copy = %d,] [add = %d]
//
//    copy -- [0] false / [1] true, add a copy the selected
//            entities to the feature.  Defaults to false.
//    add  -- The id of an entity to be added to the feature.
//            Any entity (except a feature) that is added to this
//            feature, will inherit the tool id from this feature.
//
//    Returns id.
//
CReturn 
CCreateProcessApp::FeatureCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbFeature*	dbFeature;
	CDbTool*	dbTool;
	int			tool_id;  //, color;
	ID			add;
	bool		copy;

	CModel&	model = io_cmd->getModel();

	dbFeature = NULL;
	
	add = io_cmd->VarList().getInt( "add", 0 );
	copy = io_cmd->VarList().getInt( "copy", FALSE );

	tool_id = io_cmd->VarList().getInt( "toolid", 0 );
	if (tool_id > 0)
		status = model.EntityFind( tool_id, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	if ( status.IsOk() )
		status = model.EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );

	if ( status.IsOk() )
	{
		if (model.ActivePattern() != NULL)
			model.ActivePattern()->Append( dbFeature );

		if (add > 0)
		{
			CDbEntity*	dbEntity;

			model.EntityFind( add, (CDbEntity**) &dbEntity, DBPOINT, DBFEATURE );
			if (dbEntity == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureCreate()" );
			}
			else
			{
				status = dbFeature->Append( dbEntity, copy );
			}
		}
	}

	io_cmd->setInt( "id", ((dbFeature == NULL) ? 0 : dbFeature->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Feature:Modify: id = %d [,add=%d] | [,selected]
//
// To add an entity to a feature
//    Feature:Modify: id=%d, add=%d
//
// To add all selected entities to a work-zone (for instance)
//    Feature:Modify: id=%d, selected=1
//
// To orphan all selected entities from any work-zone (for instance)
//    Feature:Modify: id=0, selected=1
//
// Where:
//    id  -- The id of the feature to find.
//    add -- The id of an entity to be added to the feature.
//
// Returns id of feature
//
CReturn 
CCreateProcessApp::FeatureModify( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbEntity*		dbEntity;

	CModel&	model = io_cmd->getModel();
	CDbFeature* dbFeature = NULL;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Retrieve and validate the command parameters
	ID id = io_cmd->VarList().getInt( "id", 0 );
	ID add = io_cmd->VarList().getInt( "add", 0 );
	bool selected = io_cmd->VarList().getInt( "selected", FALSE );

	if (id == 0 && !selected)
	{
		// NOTE: id=0 && selected is okay, implying we are going
		// to 'orphan' all selected entities (ie. they will no
		// longer belong to a work-zone).

		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureModify() 1" );
	}

	if ((add && selected) || !(add || selected))
	{
		// Can't have it both ways.
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureModify() 2" );
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (id > 0 && status.IsOk())
	{
		status = model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
		if ( !status.isOkay() )
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureModify() 3" );
	}

	if ( status.IsOk() )
	{
		CMyVisitor	tagger;

		tagger.BehaviorSet( ORPHAN_IT );

		if ( add )
		{
			model.EntityFind( add, (CDbEntity**) &dbEntity );
			if (dbEntity == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureModify() 4" );
			}
			else
			{
				dbFeature->Append( dbEntity, FALSE );
				dbEntity->Accept( &tagger );
			}
		}
		else if ( selected )
		{
			CDbEntityArray	dbEntities;
			int				count, indx;

			FeatureAdditions( model.SelectorStack()(), &dbEntities );

			count = dbEntities.Count();

			CString note;
			note.Format("The count is '%count'.  Program will now crash.", STR_TOP);
			MessageBox(NULL, note, NULL, MB_OK);

			for (indx = 0; indx < count; ++indx)
			{
				dbEntity = dbEntities[indx];

				tagger.TopLevel( TRUE );

				if (dbFeature == NULL)
				{
					// Must be 'orphaning' entities.
					dbEntity->Accept( &tagger );
				}
				else if (dbEntity != dbFeature)
				{
					// Re-parent the entity (and prevent adding to itself).
					dbEntity->Accept( &tagger );
					dbFeature->Append( dbEntity );
				}
			}
		}
	}

	io_cmd->setInt( "id", ((dbFeature == NULL) ? 0 : dbFeature->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Feature:Reorder: id = %d, src = %d [,before = %d] [,after = %d]
//
//		id		-- The id of the feature to find.
//		src		-- The id of an entity to be reordered within the feature.
//		before -- The id of an entity in the feature to move before
//		after  -- The id of an entity in the feature to move after
//					NOTE:  only specify one, before or after
//
CReturn 
CCreateProcessApp::FeatureReorder( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbFeature* dbFeature = NULL;

	ID id = 0;
	ID src = 0;
	ID before = 0;
	ID after = 0;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getInt( "src", (int*)&src );
	io_cmd->getInt( "before", (int*)&before );
	io_cmd->getInt( "after", (int*)&after );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Feature.
	status = model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	
	if ( !status.isOkay() )
		return status;

	if (src > 0)
	{
		CDbEntity* dbEntity;
		model.EntityFind( src, (CDbEntity**) &dbEntity );
		if (dbEntity == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureReorder()" );
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
				status += dbFeature->MoveBefore( dbBefore, dbEntity );
			else
			if (dbAfter)
				status += dbFeature->MoveAfter( dbAfter, dbEntity );
			else
				status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureReorder()" );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Feature:Count: id = %d
//
CReturn 
CCreateProcessApp::FeatureCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbFeature* dbFeature = NULL;

	ID id = io_cmd->VarList().getInt( "id", 0 );

	status = model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	
	io_cmd->setInt( "count", ((dbFeature == NULL) ? 0 : dbFeature->Count()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Feature:Entity: id = %d, index = %d
//
//     Returns entity id.
//
CReturn 
CCreateProcessApp::FeatureEntity( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();

	CDbFeature*	dbFeature = NULL;
	CDbEntity*	dbEntity = NULL;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	ID id = io_cmd->VarList().getInt( "id", 0 );
	int indx = io_cmd->VarList().getInt( "index", -1 );

	if (id == 0 || indx < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureEntity()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Feature.
	status = model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	
	if (dbFeature != NULL)
	{
		if (indx >= dbFeature->Count())
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureEntity()" );
		else
			dbEntity = (*dbFeature)[ indx ];
	}

	io_cmd->setInt( "id", ((dbEntity == NULL) ? 0 : dbEntity->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Feature:ToolAssociate: id = %d, toolid = %d
//
CReturn 
CCreateProcessApp::FeatureToolAssociate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbFeature* dbFeature = NULL;
	CDbTool* dbTool = NULL;

	ID id = 0;
	ID toolId = 0;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	status += io_cmd->getInt( "id", (int*) &id );
	status += io_cmd->getInt( "toolid", (int*) &toolId );
	status += model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	status += model.EntityFind( toolId, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	
	if ( status.IsOk() )
	{
		CModelUtil::ToolAssociate( dbFeature, dbTool );
	}
	else
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::FeatureToolAssociate()" );

	return status;
}


void
CCreateProcessApp::FeatureAdditions( CSelector&	selector, CDbEntityArray* dbEntities )
{
	CDbEntityArray	tmp;
	CMyVisitor		tagger;
	EDbEntityType	type;
	CDbEntity*		dbEntity;
	CDbSequence*	dbSequence;
	int				count, indx;

	tagger.BehaviorSet( TAG_IT );

	// Eliminate illegal entities from the selection set.
	CDbEntity::NewAction();

	count = selector.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = selector[indx];

		type = dbEntity->Type();
		if (type >= DBLINE && type <= DBPOINT)
		{
			dbSequence = dbEntity->Sequence();
			if (dbSequence != NULL)
				dbSequence->Disown( dbEntity );

			tmp.Append( dbEntity );
		}
	}

	// Build the final list of high-order entities to be added.
	count = tmp.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = HighestSelected( tmp[indx] );

		if ( !dbEntity->DidAction() )
		{
			dbEntity->Accept( &tagger );

			dbEntities->Append( dbEntity );
		}
	}
}

CDbEntity*
CCreateProcessApp::HighestSelected( CDbEntity* dbEntity )
{
	if ( dbEntity->DidAction() )
		return dbEntity;	// already processed

	while (1)
	{
		CDbEntity* owner = dbEntity->Owner();

		if (owner == NULL)
			break;

		if ( !owner->IsSelected() )
		{
			// BugID: 634 -- Edit/Zone/Extract caused empty profiles.
			if (owner->Type() == DBPROFILE)
				dbEntity = owner;

			break;
		}

		dbEntity = owner;
	}

	return dbEntity;
}
