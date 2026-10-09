
#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"

#include "DbTool.h"
#include "DbPattern.h"
#include "DbFeature.h"
#include "DbCommand.h"
#include "DbSequence.h"
#include "DbIterator.h"
#include "DbSeqIterator.h"
#include "WorkPkg.h"
#include "Optimizer.h"
#include "ModelClfile.h"
#include "ViewMgr.h"

#include "SequenceProcess.h"

bool CanDisplay( const CDbEntity* dbEntity );

#define BEFORE	0
#define AFTER	1


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// static const int BEFORE = -1;  TRUE
// static const int AFTER  =  1;  FALSE

static int DEFAULT_COLOR = DCOLOR_WHITE;

CDbSeqIterator g_seqiter;

void DataSet( CDbEntity* dbEntity, CCommand* io_cmd );

// #define TIME_TEST
#ifdef TIME_TEST
CReturn g_time;
int g_seqinit = g_time.Profile( "Seq:Init:" );
int g_seqmove = g_time.Profile( "Seq:Move:" );
int g_iterinit = g_time.Profile( "Seq:Iter_init:" );
int g_iternext = g_time.Profile( "Seq:Iter_next:" );
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: At first implementation, sequence objects can only
//       contain curves and other sequence objects.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Init:
//
// This Portal command is called upon opening the Seq_Order dialog.
// It establishes the 'root' and 'child' sequence objects, one
// child per work-zone.  Work-zone entities that have not yet been
// sequenced are appended to the ends of there respective sequence
// objects.
//
// 'Insert markers' are added when necessary.
//
CReturn 
CSequenceProcessApp::Init( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbIterator		iter;
	CDbPattern*		sheet;
	CDbFeature*		dbFeature;

	CModel& model = io_cmd->getModel();

#ifdef TIME_TEST
	g_time.In( g_seqinit );
#endif

	model.UndoBufferPrepare();

	// This trickery is required by CModelClfile::SequenceInit()
	model.EntityCreate( DBPATTERN, (CDbEntity**) &sheet );
	sheet->Name("main");
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
			sheet->Append( dbFeature, FALSE );

		iter.Next();
	}

	status = CModelClfile::SequenceInit( &model, TRUE );

	sheet->BenignFlush();
	sheet->Delete();

	model.UndoBufferCommit();

#ifdef TIME_TEST
	g_time.Out( g_seqinit );
#endif

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Wrap:
//		Wraps the selected entities and places the wrapper
//		object at the location of the first selected entity.
//
// Seq:Wrap:id=%d, mode=%d
//		Wraps the selected entities and places the wrapper
//		object with respect to the specified location.
//
CReturn 
CSequenceProcessApp::Wrap( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	/*
	CModel& model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int count = selector.Count();
	if (count > 0)
	{
		ID id = io_cmd->VarList().getInt( "id", selector[0]->Id() );
		int mode = io_cmd->VarList().getInt( "mode", AFTER );

		CDbEntity* dbEntity = EntityFind( model, id );

		if (dbEntity != NULL)
		{
			CDbSequence* owner;
			CDbSequence* dbSequence;
			int insertIndx, indx;

			owner = dbEntity->Sequence();
			insertIndx = ((owner == NULL) ? -1 : owner->Position( dbEntity ));

			model.UndoBufferPrepare();

			model.EntityCreate( DBSEQUENCE, (CDbEntity**) &dbSequence );

			if (insertIndx >= 0)
			{
				if (mode == BEFORE)
					owner->InsertBefore( insertIndx, dbSequence );
				else
					owner->InsertAfter( insertIndx, dbSequence );
			}

			for (indx = 0; indx < count; ++indx)
			{
				dbSequence->Append( selector[indx] );
			}

			model.UndoBufferCommit();
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcess::Wrap()" );
		}
	}
	*/
	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Explode:
//		Explodes all selected wrappers 'in situ'
//
// Seq:Explode: id=%d
//		Explodes the given wrapper 'in situ'
//
CReturn 
CSequenceProcessApp::Explode( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel& model = io_cmd->getModel();
	
	ID id = io_cmd->VarList().getInt( "id", 0 );

	model.UndoBufferPrepare();

	if (id == 0)
	{
		CSelectorStack& selectorStack = model.SelectorStack();
		CSelector& selector = selectorStack();

		int count = selector.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			status += Explode( model, selector[indx]->Id() );
		}
	}
	else
	{
		status = Explode( model, id );
	}
	
	model.UndoBufferCommit();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Move: id=%d, mode=%d
//		Moves all selected entities relative to the insert position
//
CReturn 
CSequenceProcessApp::Move( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity*		dbEntity;
	CDbSequence*	dbSequence;
	CDbCommand*		dbCommand;
	CDbEntityList	list;
	int				count, indx;
	int				mode, pos;
	ID				id, insID;

	CModel& model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	id = io_cmd->VarList().getInt( "id", 0 );
	mode = io_cmd->VarList().getInt( "mode", AFTER );

#ifdef TIME_TEST
	g_time.In( g_seqmove );
#endif

	model.EntityFind( id, (CDbEntity**) &dbSequence, DBSEQUENCE, DBSEQUENCE );
	if (dbSequence == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcess::Move()" );
		return status;
	}

	insID = (ID) dbSequence->IntGet( "_insert_id", 0 );
	model.EntityFind( insID, (CDbEntity**) &dbCommand, DBCOMMAND, DBCOMMAND );
	if (dbCommand == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcess::Move()" );
		return status;
	}

	// model.UndoBufferPrepare();
	model.UndoBufferSuppress();

	count = selector.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = selector[indx];

		if (dbEntity->Sequence() == dbSequence)
		{
			list.Append( dbEntity );
			dbSequence->Disown( dbEntity );
		}
	}

	pos = dbSequence->Position( dbCommand );
	if (pos < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcess::Move()" );
		count = 0;
		//return status;
	}
	else
	{
		count = list.Count();
	}

	// Move the entities into their new wrapper.
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = list[indx];

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
	}

	// model.UndoBufferCommit();
	model.UndoBufferActivate();

#ifdef TIME_TEST
	g_time.Out( g_seqmove );
#endif

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Optimize:
CReturn 
CSequenceProcessApp::Optimize( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	/*
	CSeqRules		seqRules;
	COptimizer		opti;
	CDbEntityArray	cutOrder;

	CModel& model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	C3dBox box = model.Box( 0 );
	
	CWorkPkg workPkg( NULL, FALSE, -UNDEFINED, box.Ymin(), UNDEFINED, box.Ymax() );

	CDbEntity::NewAction();

	status = Flatten( selector, &workPkg );

	if ( status.IsOk() )
	{
		seqRules.Init( &model, io_cmd->VarList() );
		status = opti.OptimizeWorkPkg( &seqRules, &workPkg, &cutOrder );
	}

	if ( status.IsOk() )
	{
		CDbSequence* dbSequence;
		int count, indx;

		model.UndoBufferPrepare();

		model.EntityCreate( DBSEQUENCE, (CDbEntity**) &dbSequence );

		count = cutOrder.Count();
		for (indx = 0; indx < count; ++indx)
		{
			CDbEntity* dbEntity = cutOrder[indx];
			dbSequence->Append( dbEntity );
		}

		(*(dbSequence->pAttrib())) += io_cmd->VarList();

		CleanUp( &model );

		model.UndoBufferCommit();
	}
	*/
	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// To set the insert position use:
// Seq:Insert: seqid=%d, refid=%d, refba=%d
//    seqid -- id of sequence object to which operation is applied.
//    refid -- id of reference entity for new insert position.
//    refba -- whether to insert before or after reference entity
//             (0) BEFORE / (1) AFTER
// 
// To set the insert mode use:
// Seq:Insert: seqid=%d, insba=%d
//    seqid -- id of sequence object to which operation is applied.
//    insba -- whether to insert entities before or after the insert position.
//             (0) BEFORE / (1) AFTER
//
CReturn
CSequenceProcessApp::InsertPosition( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbSequence*	dbSequence;
	CDbCommand*		dbCommand;
	CDbEntity*		dbReference;
	ID				seqID, refID, insID;
	BOOL			refBA, insBA;
	int				indx, pos;

	CModel& model = io_cmd->getModel();

	seqID = io_cmd->VarList().getInt( "seqid", 0 );
	refID = io_cmd->VarList().getInt( "refid", 0 );
	refBA = io_cmd->VarList().getInt( "refba", IUNDEFINED );
	insBA = io_cmd->VarList().getInt( "insba", IUNDEFINED );

	model.EntityFind( seqID, (CDbEntity**) & dbSequence, DBSEQUENCE, DBSEQUENCE );

	if (dbSequence == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcessApp::InsertPosition()" );
	}

	if (refID > 0)
	{
		model.EntityFind( refID, (CDbEntity**) & dbReference, DBLINE, DBCOMMAND );
		if (dbReference == NULL || refBA == IUNDEFINED)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcessApp::InsertPosition()" );
		}
	}
	else if (insBA == IUNDEFINED)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcessApp::InsertPosition()" );
	}

	if ( status.IsOk() )
	{
		insID = dbSequence->IntGet( "_insert_id", 0 );
		model.EntityFind( insID, (CDbEntity**) & dbCommand, DBCOMMAND, DBCOMMAND );

		if (dbCommand == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcessApp::InsertPosition()" );
		}
	}

	if ( status.IsOk() )
	{
		model.UndoBufferSuppress();

		if (refID > 0)
		{
			indx = dbSequence->Position( dbReference );
			pos = dbSequence->Position( dbCommand );
			if (indx < 0 || pos < 0)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcessApp::InsertPosition()" );
			}
			else
			{
				if (pos < indx)
					--indx;

				dbSequence->Remove( pos );

				if (refBA == BEFORE)
					dbSequence->InsertBefore( indx, dbCommand );
				else
					dbSequence->InsertAfter( indx, dbCommand );

				// dbSequence->IntSet( "_insert_ba", refBA );
			}
		}
		else
		{
			dbCommand->IntSet( "_insert_ba", insBA );
		}

		model.UndoBufferActivate();
	}
	
	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// To start iteration from the 'root' node
//		Seq:Iter_init:id=0
//
// To start iteration from a known node
//		Seq:Iter_init:id=%d
//
// Sets iterator to first 'displayable' entity.
//
CReturn
CSequenceProcessApp::Iter_init( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbSequence* dbSeq;
	CDbEntity* dbEntity;
	ID id;

	CModel& model = io_cmd->getModel();

#ifdef TIME_TEST
	g_time.In( g_iterinit );
#endif

	id = io_cmd->VarList().getInt( "id", 0 );

	if (id == 0)
	{
		model.EntityFind( "RootSequence", (CDbEntity**) &dbSeq, DBSEQUENCE, DBSEQUENCE );
	}
	else
	{
		model.EntityFind( id, (CDbEntity**) &dbSeq, DBSEQUENCE, DBSEQUENCE );
	}

	if (dbSeq != NULL)
	{
		g_seqiter.Init( &( model.Db() ), dbSeq->Id() );

		dbEntity = g_seqiter();

		if (dbEntity == NULL)
		{
			io_cmd->setInt( "id", 0 );
		}
		else if ( CanDisplay( dbEntity ) )
		{
			DataSet( dbEntity, io_cmd );
		}
		else
		{
			Iter_next( io_cmd );
		}
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR,
			"CSequenceProcessApp::Iter_init() -- need to first call Seq:Init:" );
	}

#ifdef TIME_TEST
	g_time.Out( g_iterinit );
#endif

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Iter_next:
//
// Gets the next 'displayable' entity.
//
CReturn
CSequenceProcessApp::Iter_next( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity;
	EDbEntityType type;

#ifdef TIME_TEST
	g_time.In( g_iternext );
#endif

	// Skip over any entities that should not be
	// displayed in the Sequence Order dialog.
	while(1)
	{
		g_seqiter.Next();

		dbEntity = g_seqiter();
		if (dbEntity == NULL)
		{
			io_cmd->setInt( "id", 0 );
			break;
		}

		type = dbEntity->Type();
		if ( CanDisplay( dbEntity ) )
		{
			DataSet( dbEntity, io_cmd );
			break;
		}
	}

#ifdef TIME_TEST
	g_time.Out( g_iternext );
#endif

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Iter_get:
//		Exists only for Java calls.
//
CReturn
CSequenceProcessApp::Iter_get( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbEntity* dbEntity = g_seqiter();
	if (dbEntity != NULL)
	{
		DataSet( dbEntity, io_cmd );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR,
			"CSequenceProcessApp::Iter_get() -- failed." );

		io_cmd->setInt( "id", 0 );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Create:
CReturn
CSequenceProcessApp::Create( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CString			name;
	CDbSequence*	dbseq;
	CModel&			model = io_cmd->getModel();

	dbseq = NULL;

	name = io_cmd->VarList().getString( "name", "" );
	if (name.GetLength() > 0)
	{
		status = model.EntityCreate( DBSEQUENCE, (CDbEntity**)&dbseq );
	}

	if (dbseq == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR,
			"CSequenceProcessApp::Create() -- failed." );
	}
	else
	{
		dbseq->Name( name );
	}

	io_cmd->setInt( "id", ((dbseq == NULL) ? 0 : dbseq->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Count: id=%d
CReturn
CSequenceProcessApp::Count( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbSequence*	dbseq;
	ID				id;
	CModel&			model = io_cmd->getModel();

	id = io_cmd->VarList().getInt( "id", 0 );

	model.EntityFind( id, (CDbEntity**) &dbseq, DBSEQUENCE, DBSEQUENCE );

	if (dbseq == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR,
			"CSequenceProcessApp::Count() -- failed." );
	}

	io_cmd->setInt( "count", ((dbseq == NULL) ? 0 : dbseq->Count()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Get: id=%d, indx=%d
CReturn
CSequenceProcessApp::Get( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbSequence*	dbseq;
	ID				id;
	int				indx;
	CModel&			model = io_cmd->getModel();

	id = io_cmd->VarList().getInt( "id", 0 );
	indx = io_cmd->VarList().getInt( "indx", -1 );

	model.EntityFind( id, (CDbEntity**) &dbseq, DBSEQUENCE, DBSEQUENCE );

	if ((dbseq == NULL) || (indx < 0) || (indx > dbseq->Count()))
	{
		status.Internal( IDS_INTERNAL_ERROR,
			"CSequenceProcessApp::Get() -- failed." );

		id = 0;
	}
	else
	{
		id = (*dbseq)[indx]->Id();
	}

	io_cmd->setInt( "id", ((dbseq == NULL) ? 0 : id) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Append: parent=%d, child=%d
CReturn
CSequenceProcessApp::Append( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CString			msg;
	CDbSequence*	parent;
	CDbEntity*		child;
	ID				parentID;
	ID				childID;
	CModel&			model = io_cmd->getModel();

	parentID = io_cmd->VarList().getInt( "parent", 0 );
	childID = io_cmd->VarList().getInt( "child", 0 );
	
	model.EntityFind( parentID, (CDbEntity**) &parent, DBSEQUENCE, DBSEQUENCE );

	model.EntityFind( childID, (CDbEntity**) &child, DBLINE, DBHOLE );
	if (child == NULL)
	{
		model.EntityFind( childID, (CDbEntity**) &child, DBCOMMAND, DBSEQUENCE );
	}

	if (parent == NULL || child == NULL)
	{
		msg.Format( "CSequenceProcessApp::Append() -- failed to append entity id %d", childID );
		status.Internal( IDS_INTERNAL_ERROR, msg );
	}
	else
	{
		parent->Append( child );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:BenignFlush: id=%d
CReturn
CSequenceProcessApp::BenignFlush( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbSequence*	dbseq;
	ID				id;
	CModel&			model = io_cmd->getModel();

	id = io_cmd->VarList().getInt( "id", 0 );

	model.EntityFind( id, (CDbEntity**) &dbseq, DBSEQUENCE, DBSEQUENCE );

	if (dbseq == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR,
			"CSequenceProcessApp::BenignFlush() -- failed." );
	}
	else
	{
		dbseq->BenignFlush();
	}

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Seq:Dump:
CReturn
CSequenceProcessApp::Dump( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CDbIterator iter;
	CDbEntityArray entities;
	CDbSequence* dbSequence;
	CDbEntity* dbEntity;
	CString data;
	int count, indx, level;

	const int FLAGS = DBSEQ_RECURSE |
					  DBSEQ_ADD_SUBSEQS |
					  DBSEQ_TAG_ADDED |
					  DBSEQ_TAG_SUBSEQS |
					  DBSEQ_LEVEL;

	CModel& model = io_cmd->getModel();

	iter.Init( model.Db(), DBSEQUENCE );
	while (1)
	{
		dbSequence = dynamic_cast<CDbSequence*>( iter() );
		if (dbSequence == NULL)
			break;

		if ( !dbSequence->DidAction() )
			dbSequence->Flatten( FLAGS, &entities );

		iter.Next();
	}

	status.Diagnostic( "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" );
	count = entities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = entities[indx];
		level = dbEntity->IntGet( "~level", 0 );

		{
			CString whiteSpace( ' ', (2 * level) );

			data.Format( "%s%s", whiteSpace, dbEntity->Name() );

			status.Diagnostic( data );
		}
	}

	CDbEntity::NewAction();	// WTF?

#ifdef TIME_TEST
	g_time.pDump();
#endif

	return ( CReturn( STATUS_OKAY ) );
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CDbEntity*
CSequenceProcessApp::EntityFind( const CModel& model, ID id )
{
	CDbEntity* dbEntity;

	model.EntityFind( id, &dbEntity, DBLINE, DBHOLE );

	if (dbEntity == NULL)
		model.EntityFind( id, &dbEntity, DBCOMMAND, DBCOMMAND );

	if (dbEntity == NULL)
		model.EntityFind( id, &dbEntity, DBSEQUENCE, DBSEQUENCE );

	return dbEntity;
}

void
CSequenceProcessApp::CleanUp( CModel* model )
{
	CDbIterator iter;

	iter.Init( model->Db(), DBSEQUENCE );
	while (1)
	{
		CDbSequence* dbSequence = dynamic_cast<CDbSequence*>( iter() );
		if (dbSequence == NULL)
			break;

		if (dbSequence->Count() <= 0)
			dbSequence->Delete();

		iter.Next();
	}
}

// TODO:  Make a common area for sequence explode, and point both this AND the version in nestpart.cpp to it
CReturn
CSequenceProcessApp::Explode( const CModel& model, ID id )
{
	CReturn status;

	CDbSequence* dbSequence;
	model.EntityFind( id, (CDbEntity**) &dbSequence, DBSEQUENCE, DBSEQUENCE );

	if (dbSequence != NULL)
	{
		CDbEntityList dbEntities;
		dbSequence->Subordinates( &dbEntities );

		CDbSequence* owner = dbSequence->Sequence();
		int insertIndx = ((owner == NULL) ? -1 : owner->Position( dbSequence ));

		int count = dbEntities.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* dbEntity = dbEntities[indx];
			dbEntity->Sequence( NULL );
			if (insertIndx >= 0)
			{
				owner->InsertAfter( insertIndx, dbEntity );
				++insertIndx;
			}
		}

		dbSequence->Delete();
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CSequenceProcess::Explode()" );
	}

	return status;
}

CReturn
CSequenceProcessApp::Flatten( const CSelector& selector, CWorkPkg* workPkg )
{
	CReturn status;
	
	const int FLAGS = DBSEQ_RECURSE | DBSEQ_TAG_ADDED | DBSEQ_TAG_SUBSEQS;

	CDbEntityArray& entities = workPkg->Entities();

	int count = selector.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = selector[indx];

		if ( !dbEntity->DidAction() )
		{
			CDbSequence* dbSequence = dynamic_cast<CDbSequence*>( dbEntity );

			if (dbSequence == NULL)
			{
				entities.Append( dbEntity );
				dbEntity->DoAction();
			}
			else
			{
				dbSequence->Flatten( FLAGS, &entities );
			}
		}
	}

	workPkg->BoxUpdate();  // see comment in CWorkPkg::BoxUpdate()

	return status;
}

void DataSet( CDbEntity* dbEntity, CCommand* io_cmd )
{
	ID id = 0;

	if (dbEntity != NULL)
	{
		CDbTool* dbTool;
		int toolNo;

		id = dbEntity->Id();
		dbTool = dbEntity->Tool();

		toolNo = ((dbTool == NULL) ? 0 : dbTool->IntGet( "NC_Code_Number", 0 ));

		io_cmd->setString( "name", dbEntity->Name() );
		io_cmd->setInt( "color", dbEntity->ColorGet( DEFAULT_COLOR ) );
		io_cmd->setInt( "level", dbEntity->IntGet( "~level", 1 ) );
		io_cmd->setInt( "tool", toolNo );
		io_cmd->setInt( "type", dbEntity->Type() );
	}

	io_cmd->setInt( "id", id );
}

bool CanDisplay( const CDbEntity* dbEntity )
{
	EDbEntityType type = dbEntity->Type();

	return ( type == DBLINE	||
			 type == DBARC	||
			 type == DBHOLE	||
			 type == DBCOMMAND  ||
			 type == DBSEQUENCE );
}
