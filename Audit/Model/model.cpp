
#include "stdafx.h"
#include "Msg.h"
#include "Vm.h"
#include "DbEntity.h"
#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "DbIterator.h"
#include "DbCommand.h"
#include "Selector.h"
#include "Model.h"

static const int ROUTER = 7;  // Router machine designation
static const int PT2PT  = 8;  // Point-to-Point machine designation


////////////////////////////////////////////////////////////////////////

CModel::CModel()

	: m_db(),
	  m_selectorStack( NULL ),
	  m_treeViewSupport( NULL ),
	  m_tool( NULL ),
	  m_workplane( NULL ),
	  m_pattern( NULL ),
	  m_pattern_xform( NULL )
{
}

CModel::~CModel()
{
	m_db.UndoBufferSuppress();
	m_header.Reset();
	// m_varsys.Reset();
}

void
CModel::Init( CSelectorStack& selectorStack, CTreeViewSupport& treeViewSupport )
{
	m_selectorStack = &selectorStack;
	m_treeViewSupport = &treeViewSupport;

	m_selectorStack->Init( (*this) );
	m_treeViewSupport->Init( (*this) );
}

CReturn
CModel::Flush()
{
	CReturn status = m_db.Flush();

	m_header.Reset();
	// m_varsys.Reset();
	
	m_tool = NULL;
	m_workplane = NULL;
	m_pattern = NULL;
	m_pattern_xform = NULL;

	return status;
}

C3dBox
CModel::Box( ID workId ) const
{
	CDbIterator	iter;
	C3dBox		world;
	C3dBox		box;

	iter.Init( m_db, DBPOINT );
	while (1)
	{
		CDbEntity* dbEntity = iter();

		if (dbEntity == NULL)
			break;

		if (dbEntity->Type() > DBCOMMAND)
			break;

		if ( !dbEntity->IsSystem() && dbEntity->Type() != DBPROFILE )
		{
			if ( (dbEntity->Type() != DBCOMMAND) ||
				 (((CDbCommand*) dbEntity)->IsInstance()) )
			{
				box = dbEntity->Box( workId );

				if ( box.IsDefined() )
					world += box;
			}
		}

		iter.Next();
	}

	return world;
}

C3dBox
CModel::BoxUser( ID workId ) const
{
	CDbIterator	iter;
	C3dBox		world;

	iter.Init( Db(), DBLINE );	// Grab extent of user geometry; Line through Hole
	while (1)
	{
		const CDbEntity* dbEntity = iter();
		if (dbEntity == NULL)
			break;
		if (dbEntity->Type() > DBCOMMAND)
			break;	// Only through command
		iter.Next();

		if (dbEntity->IsSystem())
			continue;

#if 0
		if (dbEntity->Type() == DBPROFILE)
			continue;
#else
		EDbEntityType type = dbEntity->Type();
		if ((type == DBLINE) || (type == DBARC))
		{
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity->Owner() );
			if (dbProfile != nullptr)
				continue;
		}
#endif

		CDbEntity* owner = dbEntity->Owner();
		while (owner)
		{ 
			CDbEntity* next = owner->Owner();
			if (!next)
				break;
			owner = next;
		}

		if ( owner && (owner->Type() == DBPATTERN) )
			continue;

		if (dbEntity->Type() == DBCOMMAND)
		{
			const CDbCommand* cmd = dynamic_cast<const CDbCommand*>(dbEntity);
			if ( !cmd->IsInstance() )
				continue;
		}

		const C3dBox& box = dbEntity->Box( workId );

		if ( box.IsDefinedXY() )
			world += box;
	}

	if (world.Zmin() <= -UNDEFINED)
		world.Z(0.0);

	if (world.Zmax() >= UNDEFINED)
		world.Z(0.0);

	return world;
}


bool
CModel::IsRegenPending()
{
	CDbEntityList entities;
	RegenList( &entities );
	return (entities.Count() > 0);
}

void
CModel::RegenList( CDbEntityList* list )
{
	// Regeneration must occur if any of
	// the following conditions is satisfied:
	//
	// 1. Operational parameters have changed.
	// 2. Tooling parameters have changed.
	// 3. Geometry or features have changed.
	//
	// TODO: If speed becomes an issue, we may be
	// able to optimize by checking condition 1 first,
	// for instance.  This may be especially true when
	// we have a large number of entities.

	CDbEntityList foo;
	m_db.DirtyEntities( &foo );

	int count = foo.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( foo[indx] );

		if (dbFeature != NULL)
			list->ConditionalAppend( dbFeature );
	}

	if (list->Count() < 1)
	{
		// An optimization that's supposed save time
		// on the next call to CModel::RegenList().
		for (int jndx = 0; jndx < count; ++jndx)
		{
			CDbEntity* dbEntity = foo[jndx];
			dbEntity->ModifyFlag( false );
		}
	}

	return;
}

void
CModel::ClearDirty()
{
	m_db.ClearDirty();
}

bool
CModel::IsYNegative() const
{
	return (Header().getInt("WorkplaneType", 1) == 2);
}

bool
CModel::IsRightHanded() const
{
	int workType = Header().getInt( "WorkplaneType", 1 );
	int machType = Header().getInt( "MachineType", ROUTER );

	return (workType == 1 || (workType == 2 && machType != PT2PT));
}

// Context management.

CDbTool*
CModel::ActiveTool() const
{
	return m_tool;
}

void
CModel::ActiveTool( CDbTool* tool )
{
	m_tool = tool;
}

CDbWorkplane*
CModel::ActiveWorkplane() const
{
	return m_workplane;
}

void
CModel::ActiveWorkplane( CDbWorkplane* workplane )
{
	if (m_workplane)
		m_workplane->Hide();

	m_workplane = workplane;

	if (m_workplane)
		m_workplane->Seek();
}

// Entity database management.

int CModel::EntityCount() const
{
	return m_db.Count();
}

CReturn CModel::EntityCreate( EDbEntityType type, CDbEntity** dbEntity )
{
	CReturn status = m_db.Create( type, dbEntity );

	return status;
}

CReturn CModel::EntityPrepareCopy( CModel* io_dest )
{
	CReturn	status;

	// If the destination is NULL, perform the copy within this
	// model.  Otherwise, the entities are copied from this model
	// into the destination model.

	if (io_dest)
		status = m_db.PrepareCopy( &io_dest->m_db );
	else
		status = m_db.PrepareCopy( &m_db );

	return status;
}

CReturn CModel::EntityCopy( CDbEntity& in_entity, CDbEntity** dbEntity ) 
{
	CReturn status = m_db.Copy( in_entity, dbEntity );

	return status;
}

CReturn CModel::EntityDelete( ID id )
{
	CDbEntity* dbEntity;

	CReturn status = m_db.Find( id, &dbEntity );
	if ( status.IsOk() )
	{
		dbEntity->Delete();
	}

	return status;
}

CReturn
CModel::EntityFind(
				ID id,
				CDbEntity** dbEntity,
				EDbEntityType startType,
				EDbEntityType endType ) const
{
	CReturn status = m_db.Find( id, dbEntity, startType, endType );

	return status;
}

CReturn
CModel::EntityFind(
				const CString& name,
				CDbEntity** dbEntity,
				EDbEntityType startType,
				EDbEntityType endType ) const
{
	CReturn status = m_db.Find( name, dbEntity, startType, endType );

	return status;
}

// Undo system management.

void
CModel::UndoBufferActivate()
{
	m_db.UndoBufferActivate();
}

void
CModel::UndoBufferSuppress()
{
	m_db.UndoBufferSuppress();
}

void
CModel::UndoBufferFlush()
{
	m_db.UndoBufferFlush();
}

void
CModel::UndoBufferPrepare()
{
	m_db.UndoBufferPrepare();
}

void
CModel::UndoBufferCommit()
{
	m_db.UndoBufferCommit();
}

CReturn
CModel::Undo()
{
	CReturn status = m_db.Undo();
	return status;
}

CReturn
CModel::Redo()
{
	CReturn status = m_db.Redo();
	return status;
}

int
CModel::UndoBufferDepth() const
{
	return m_db.UndoBufferDepth();
}

void
CModel::UndoBufferDepth( int depth )
{
	m_db.UndoBufferDepth( depth );
}

void
CModel::PostReadInit()
{
	CDbIterator iter;

	ClearDirty();
	m_db.IdMaxInit();

	iter.Init( Db(), DBWORKPLANE );

	// Hide all workplanes; none are selected at start.
	const CDbEntity* entity;
	while ( entity = iter() )
	{
		if (entity->Type() != DBWORKPLANE)
			break;

		((CDbEntity*)entity)->Hide();

		iter.Next();
	}

	// Reset the insert sequence counter for Features; otherwise,
	// new features step on the sequence position of old features.
//	m_db.InsertReset();
}

CEntityDb&
CModel::Db() const
{
	return ((CModel*) this)->m_db;
}

CReturn 
CModel::MacroRun(
			const CString&	javaFilePath,
			const CVarList&	javaCmdLineParams,
			bool			compile,
			bool			isRTL )
{
	CReturn status;

	MsgInit();  // Activated to trap any messages from java compiler/vm.

	m_varsys += javaCmdLineParams;

	if ( compile )
		status = CVm::Compile( javaFilePath );

	if ( status.IsOk() )
		status = CVm::Execute( javaFilePath, isRTL );

	MsgTerm();

	return status;
}

// ==================================================================
// NOTE: CoglViewWire::is_hidden() has been moved here so that its
// functionality will be common to both CSelector and CoglViewWire.
//
// Ideally, this method should be somewwhere in the Policy library
// but library dependencies prevent that (at this time -- V16).
//
// TODO:  put test into common place in policy.
//
// static
bool
CModel::is_hidden( const CDbEntity* entity ) const
{
	if ( entity->is_hidden() )
		return TRUE;

	switch (entity->Type())
	{
	case DBPOINT:
		if (entity->IsSystem())
			return TRUE;
		break;

	case DBPATTERN:
		if (entity != ActivePattern())
			return TRUE;
		break;
	}

	if (entity->Owner())
	{
		if (is_hidden(entity->Owner()))
			return TRUE;
	}
	else
	{
		// This is the TOP LEVEL dude...
		if ( ActivePattern() && (entity != ActivePattern()) )
			return TRUE;
	}

	return FALSE;
}

// For debugging.
void CModel::ToolsTrace( const char* heading ) const
{
	CDbIterator iter;

	if (heading != NULL)
	{
		CReturn tmp;
		tmp.Diagnostic( heading );
	}

	iter.Init( m_db, DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		dbTool->Trace();

		iter.Next();
	}
}

