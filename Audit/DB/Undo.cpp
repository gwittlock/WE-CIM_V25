
#include "stdafx.h"
#include "cmn_resource.h"
#include "DbEntity.h"
#include "EntityDb.h"
#include "Undo.h"

// NOTE: Problems related to the Undo Systen are usually
//       caused by one of the following:
//
//       1. An entity class method forgot to call CDbEntity::Record()
//       2. The CDbEntity::Record() method is not called before an
//          entity's state is changed.
//       3. An entity copy method is defective, perhaps not copying
//          all relevant data (for instance).
//
static int DUMP = 0;  // For debugging.


////////////////////////////////////////////////////////////////////////

CUndo::CUndo( CEntityDb* db )
	: m_db( db ),
	  m_undoBuffer(),
	  m_intermediate( NULL ),
	  m_isActive( TRUE ),
	  m_bufferDepth( 20 ),
	  m_open( 0 ),
	  m_curr( -1 )
{
	ASSERT( (m_db != NULL) );
}

CUndo::~CUndo()
{
	Flush();
}

void
CUndo::Flush()
{
	CDbEntityArray* temp;
	int	indx;

	indx = m_undoBuffer.Count() - 1;

	while (indx >= 0)
	{
		temp = m_undoBuffer.Remove( indx );

		temp->DestructiveFlush();

		delete temp;

		--indx;
	}

	m_curr = -1;
}

void
CUndo::Activate()
{
	m_isActive = TRUE;
}

void
CUndo::Suppress()
{
	m_isActive = FALSE;
}

void
CUndo::Open()
{
	if ( !m_isActive )
		return;

	if (m_open == 0)
	{
		// Create the new intermediate buffer.
		m_intermediate = new CDbEntityArray();

		if (m_intermediate == NULL)
		{
			CReturn status;
			status.Fatal( IDS_MEM_ALLOC_FAILURE, "CUndo::Open(#1)" );
		}
	}

	++m_open;
}

void
CUndo::Close()
{
	if ( !m_isActive )
		return;

	--m_open;

	if (m_open < 0)
	{
		CReturn note;
		note.Internal( IDS_INTERNAL_ERROR, "CUndo::Close() unbalanced open/close operations" );
		return;
	}

	if (m_open == 0 && m_intermediate != NULL)
	{
		if ( DUMP ) Dump( "Close() -- beginning" );

		if (m_intermediate->Count() > 0)
		{
			// Purge any deltas that go out of scope.
			FlushLost();

			// Commit the intermediate buffer.
			m_undoBuffer.Append( m_intermediate );
			++m_curr;
		}
		else
		{
			delete m_intermediate;
			m_intermediate = NULL;
		}

		if ( DUMP ) Dump( "Close() -- end" );
	}
}

void
CUndo::Record( CDbEntity* dbEntity )
{
	if ( !m_isActive )
		return;


	if (m_open < 1)
	{
		CReturn note;
		note.Internal( IDS_INTERNAL_ERROR, "CUndo::Record() buffer is not open" );
		return;  // ASSERT( (m_open > 0) );
	}

	int count, indx;

	CDbEntity* undoEntity = NULL;
	bool found = FALSE;

	count = m_intermediate->Count();

	// Determine whether a copy of the entity is
	// already in the undo buffer.  We must search
	// by entity id, as opposed to address, because
	// modifying the entity will adversely affect
	// the associative nature of the database.
	for (indx = 0; indx < count; ++indx)
	{
		undoEntity = (*m_intermediate)[ indx ];

		found = (undoEntity->Id() == dbEntity->Id());

		if ( found )
			break;
	}

	if ( !found )
	{
		dbEntity->Clone( &undoEntity );
		undoEntity->ReferenceFlagClear();

		m_intermediate->Append( undoEntity );
	}
}

CReturn
CUndo::Undo()
{
	CReturn			status;
	CString			name;
	EDbEntityType	type;
	CDbEntityArray* undoEntities;
	CDbEntity*		undoEntity;
	CDbEntity*		dbEntity;
	int				count, indx;

	if (m_open != 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CUndo::Undo() buffer is not closed" );
		return status;
	}

	if (m_curr >= 0)
	{
		if ( DUMP ) Dump( "Undo() -- beginning" );

		undoEntities = m_undoBuffer[ m_curr ];

		count = undoEntities->Count();

		// Traverse backwards in history.
		for (indx = (count-1); indx >= 0; --indx)
		{
			undoEntity = (*undoEntities)[ indx ];

			type = undoEntity->Type();

			m_db->UndoFind( undoEntity->Id(), &dbEntity, type, type );
			if (dbEntity == NULL)
				continue;

			if ( undoEntity->WasCreated() )
			{
				dbEntity->ContentsSwap( undoEntity );

				// Clearing the reference flag prevents this entity
				// from affecting the database when it is deleted.
				undoEntity->ReferenceFlagClear();

				// Temporarily tag this entity as created so
				// we can catch it during a redo operation
				undoEntity->CreateFlag( true );

				// Setting the delete flag prevents the from being
				// considered by display (and other) functions.
				dbEntity->DeleteFlag( true );

				dbEntity->ReferenceFlagSet();
			}
			else if ( undoEntity->IsDeleted() )
			{
				dbEntity->ContentsSwap( undoEntity );

				// Clearing the reference flag prevents this entity
				// from affecting the database when it is deleted.
				undoEntity->ReferenceFlagClear();

				dbEntity->ReferenceFlagSet();
				dbEntity->DeleteFlag( false );

				name = dbEntity->Name();
				if (name.GetLength() > 0 && name[0] != '_')
				{
					// Re-establish this entity's user-defined name.
					dbEntity->Name( name );
				}
			}
			else
			{
				dbEntity->ContentsSwap( undoEntity );

				// Clearing the reference flag prevents this entity
				// from affecting the database when it is deleted.
				undoEntity->ReferenceFlagClear();

				dbEntity->ReferenceFlagSet();
			}
		}

		--m_curr;

		if ( DUMP ) Dump( "Undo() -- end" );
	}

	return status;
}

CReturn
CUndo::Redo()
{
	CReturn			status;
	CString			name;
	EDbEntityType	type;
	CDbEntityArray* undoEntities;
	CDbEntity*		undoEntity;
	CDbEntity*		dbEntity;
	int				count, indx;

	if (m_open != 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CUndo::Redo() buffer is not closed" );
		return status;
	}
	
	if ((m_curr + 1) < m_undoBuffer.Count())
	{
		if ( DUMP ) Dump( "Redo() -- beginning" );

		++m_curr;

		undoEntities = m_undoBuffer[ m_curr ];

		count = undoEntities->Count();

		// Traverse forwards in history.
		for (indx = 0; indx < count; ++indx)
		{
			undoEntity = (*undoEntities)[ indx ];

			type = undoEntity->Type();

			m_db->UndoFind( undoEntity->Id(), &dbEntity, type, type );

			if ( undoEntity->WasCreated() )
			{
				dbEntity->ContentsSwap( undoEntity );

				// Clearing the reference flag prevents this entity
				// from affecting the database when it is deleted.
				undoEntity->ReferenceFlagClear();

				// Probably not necessary, but keeps state clear.
				undoEntity->DeleteFlag( false );
				// Clear the temporary flag (see Undo()).
				dbEntity->CreateFlag( false );

				// Allow the entity to be considered by the
				// display (and other) functions.
				dbEntity->DeleteFlag( false );

				dbEntity->ReferenceFlagSet();
			}
			else if ( undoEntity->IsDeleted() )
			{
				dbEntity->ContentsSwap( undoEntity );

				// Clearing the reference flag prevents this entity
				// from affecting the database when it is deleted.
				undoEntity->ReferenceFlagClear();

				dbEntity->ReferenceFlagSet();
				dbEntity->DeleteFlag( true );
				undoEntity->DeleteFlag( true );
			}
			else
			{
				dbEntity->ContentsSwap( undoEntity );

				// Clearing the reference flag prevents this entity
				// from affecting the database when it is deleted.
				undoEntity->ReferenceFlagClear();

				dbEntity->ReferenceFlagSet();
			}
		}

		if ( DUMP ) Dump( "Redo() -- end" );
	}

	return status;
}

int
CUndo::Depth()
{
	return m_bufferDepth;
}

void
CUndo::Depth( int depth )
{
	m_bufferDepth = depth;
}

void
CUndo::FlushLost()
{
	// Purge any deltas that go out of scope.  Deltas can go
	// out of scope under either of the conditions 1) the depth
	// of the undo buffer is exceeded, or 2) a Record() operation
	// occurs when the current item index is not aligned with the
	// redo limit index.  This later condition occurs when you do
	// not Redo() and Undo() before executing a new command.
	//
	// In the former scenario, as the undo buffer is FIFO, deltas
	// are removed from the front of the buffer.  The current item
	// index must also be synchronized.
	//
	// In the later scenario, all items beyond the current item
	// index a removed.

	CDbEntityArray* deltas = NULL;
	CDbEntity* dbEntity = NULL;
	CDbEntity* proxy = NULL;
	EDbEntityType type;
	int count, indx, jndx;
	ID id;


	// case 2
	// if (m_curr >= 0)
	{
		indx = m_undoBuffer.Count() - 1;
		while (indx > m_curr)
		{
			deltas = m_undoBuffer.Remove( indx );

			count = deltas->Count();

			for (jndx = 0; jndx < count; ++jndx)
			{
				proxy = (*deltas)[ jndx ];
				if ( proxy->WasCreated() )
				{
					id = proxy->Id();
					type = proxy->Type();
					proxy->Find( id, &dbEntity, type, type );

					delete dbEntity;
				}
			}

			deltas->DestructiveFlush();
			delete deltas;

			indx = m_undoBuffer.Count() - 1;
		}
	}

	// case 1
	while (m_undoBuffer.Count() >= m_bufferDepth)
	{
		deltas = m_undoBuffer.Remove( 0 );

		count = deltas->Count();

		for (jndx = 0; jndx < count; ++jndx)
		{
			proxy = (*deltas)[ jndx ];

			if ( proxy->IsDeleted() )
			{
				id = proxy->Id();
				type = proxy->Type();
				proxy->Find( id, &dbEntity, type, type );

				if ((dbEntity != NULL) && !dbEntity->IsRefd())
					delete dbEntity;
			}
		}

		deltas->DestructiveFlush();
		delete deltas;

		--m_curr;
	}
}

void
CUndo::Dump( const char* caption )
{
#ifdef _DEBUG
	TRACE( "------------ %s ------------\n", caption );
	int depth = m_undoBuffer.Count();
	for (int indx = depth - 1; indx >= 0; --indx)
	{
		TRACE( "%d) ", indx );

		CDbEntityArray* dbEntityList = m_undoBuffer[indx];

		int count = dbEntityList->Count();
		for (int jndx = 0; jndx < count; ++ jndx)
		{
			CDbEntity* dbEntity = (*dbEntityList)[jndx];
			if ( dbEntity->IsDeleted() )
				TRACE( "X" );
			TRACE( "%d,", dbEntity->Id() );
		}

		TRACE( "\n" );
	}
#endif
}
