
#include "stdafx.h"
#include "DbCurve.h"
#include "DbHole.h"
#include "DbFeature.h"
#include "DbCommand.h"
#include "DbSequence.h"
#include "EntityDb.h"
#include "DbEntityVisitor.h"


////////////////////////////////////////////////////////////////////////

CDbSequence::CDbSequence( CEntityDb* db )
	: CDbEntity( db ),
	  m_list()
{
	// Prevent select when someone does "Select All" from UI
	SystemFlag( true );
}

CDbSequence::CDbSequence( const CDbSequence& dbSequence )
	: CDbEntity( dbSequence ),
	  m_list()
{
	dbSequence.RefsTo( &m_list );
}

CDbSequence::~CDbSequence()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

// Obtain the count of entities in this container.
int
CDbSequence::Count() const
{
	return m_list.Count();
}

// Obtain a pointer to the Ith entity.
CDbEntity*
CDbSequence::operator[]( int indx ) const
{
	return m_list[indx];
}

int
CDbSequence::Position( const CDbEntity* dbEntity ) const
{
	return ( m_list.Find( (CDbEntity*) dbEntity ) );
}

// Insert an entity at the beginning of this container.
void
CDbSequence::Prepend( CDbEntity* dbEntity )
{
	if ( IsValid( dbEntity ) )
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.Prepend( dbEntity );
		dbEntity->Sequence( this );
	}
	else
	{
		Error( "CDbSequence::Prepend()" );
	}
}

// Add an entity to this container.
void
CDbSequence::Append( CDbEntity* dbEntity )
{
	if ( IsValid( dbEntity ) )
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.Append( dbEntity );
		dbEntity->Sequence( this );
	}
	else
	{
		Error( "CDbSequence::Append()" );
	}
}

void
CDbSequence::InsertBefore( int indx, CDbEntity* dbEntity )
{
	if (IsValid( dbEntity ) && indx >= 0 && indx < m_list.Count())
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.InsertBefore( indx, dbEntity );
		dbEntity->Sequence( this );
	}
	else
	{
		Error( "CDbSequence::InsertBefore()" );
	}
}

void
CDbSequence::InsertAfter( int indx, CDbEntity* dbEntity )
{
	if (IsValid( dbEntity ) && indx >= 0 && indx < m_list.Count())
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		m_list.InsertAfter( indx, dbEntity );
		dbEntity->Sequence( this );
	}
	else
	{
		Error( "CDbSequence::InsertBefore()" );
	}
}

// Returns true if successful.
bool
CDbSequence::Disown( CDbEntity* dbEntity )
{
	int indx = m_list.Find( dbEntity );
	CDbEntity* theEntity = Remove( indx );

	return (theEntity != NULL);
}

CDbEntity*
CDbSequence::Remove( int indx )
{
	CDbEntity*	dbEntity = NULL;

	if (indx >= 0 && indx < m_list.Count())
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		dbEntity = m_list.Remove( indx );
		dbEntity->OrphanSeq();
	}

	return dbEntity;
}

// Recursively descend this sequence, adding child entities to the list.
void
CDbSequence::Flatten( int flags, CDbEntityArray* entities )
{
	CDbSequence*	subseq;
	CDbFeature*		workZone;
	CDbEntity*		dbEntity;
	int				count, indx;

	int level = -1;
	if (flags & DBSEQ_LEVEL)
	{
		CDbSequence* seq = Sequence();
		if (seq != NULL)
			level = seq->IntGet( "~level", -1 );

		++level;
		IntSet( "~level", level );
	}

	if (flags & DBSEQ_ADD_SUBSEQS)
		entities->Append( this );

	if (flags & DBSEQ_TAG_SUBSEQS)
		this->DoAction();

	workZone = NULL;
	count = m_list.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = m_list[indx];
		subseq = dynamic_cast<CDbSequence*>( dbEntity );

		if ( (subseq == NULL) ||
			 ((subseq != NULL) && !(flags & DBSEQ_RECURSE)) )
		{
			if ((workZone == NULL) && (flags & DBSEQ_WORKZONE))
			{
				workZone = dbEntity->WorkZone();
				if (workZone != NULL)
				{
					// Allow CModelClfile::UserCommandsProcess() to flag zone information so
					// that SequenceInit() can put entities into the correct sequence object.
					entities->Append( workZone );
				}
			}

			entities->Append( dbEntity );

			if ((indx == (count-1)) && (workZone != NULL))
			{
				// Allow CModelClfile::UserCommandsProcess() to output such things
				// as hold-down and repositioning events.  These events must occur
				// before we can processes the next work-zone.
				entities->Append( workZone );
			}

			if (flags & DBSEQ_LEVEL)
				dbEntity->IntSet( "~level", (level+1) );

			if (flags & DBSEQ_TAG_ADDED)
				dbEntity->DoAction();
		}
		else if (flags & DBSEQ_RECURSE)
		{
			subseq->Flatten( flags, entities );
		}
	}
}

void
CDbSequence::BenignFlush()
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	while (m_list.Count() > 0)
	{
		CDbEntity* dbEntity = Remove(0);
		dbEntity->OrphanSeq();
	}
}

void
CDbSequence::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	BenignFlush();

	// Delete the associated 'insert position'.
	ID insertID = IntGet( "_insert_id", 0 );
	if (insertID > 0)
	{
		CDbCommand*	dbCommand;
		Db()->Find( insertID, (CDbEntity**) &dbCommand, DBCOMMAND, DBCOMMAND );
		if (dbCommand != NULL)
		{
			dbCommand->Delete();
		}
	}

	CDbSequence* dbSequence = CDbEntity::Sequence();
	if (dbSequence != NULL)
		dbSequence->Sequence( NULL );
}

void
CDbSequence::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

void
CDbSequence::RefsTo( CDbEntityList* list ) const
{
	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		list->Append( m_list[indx] );
	}
}

void
CDbSequence::Subordinates( CDbEntityList* list ) const
{
	RefsTo( list );
}

void
CDbSequence::RemoveRef( CDbEntity* dbEntity )
{
	// A sequence object does not reference anything.
}

const CDbSequence&
CDbSequence::operator = ( const CDbSequence& dbSequence )
{
	CDbEntity::CommonCopy( dbSequence );

	m_list.BenignFlush();
	dbSequence.RefsTo( &m_list );

	return (*this);
}

CReturn
CDbSequence::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbSequence* dbSequence = new CDbSequence( (*this) );

	(*dbEntity) = dbSequence;

	return status;
}

CReturn
CDbSequence::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbSequence* dbSequence = dynamic_cast<CDbSequence*>( dbEntity );

	ASSERT( (dbSequence != NULL) );

	CDbSequence tmp( (*this) );
	(*this) = (*dbSequence);
	(*dbSequence) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}

bool
CDbSequence::IsValid( const CDbEntity* dbEntity )
{
	if (dbEntity->Tool() == NULL)
	{
		if (dynamic_cast<const CDbSequence*>( dbEntity ) != NULL)
			return TRUE;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// For the next two conditionals:
		//		See also CModelClfile::CutOrderAppend()
		//		where "if (topLevelFeature != NULL)"
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		if (dynamic_cast<const CDbFeature*>( dbEntity ) != NULL)
			return TRUE;
	}
	else
	{
		if (dynamic_cast<const CDbCurve*>( dbEntity ) != NULL)
			return TRUE;

		if (dynamic_cast<const CDbHole*>( dbEntity ) != NULL)
			return TRUE;

		if (dynamic_cast<const CDbCommand*>( dbEntity ) != NULL)
		{
			return ( ((CDbCommand*) dbEntity)->IsInsert() ||
					 ((CDbCommand*) dbEntity)->IsInstance() ||
					 ((CDbCommand*) dbEntity)->IsTooledText() );
		}
	}

	return FALSE;
}

void
CDbSequence::Error( const CString& msg )
{
	CReturn status;
	status.Internal( IDS_INTERNAL_ERROR, msg );
}

// static
bool
CDbSequence::CanSequence( CDbEntity* dbEntity )
{
	if (dbEntity == NULL)
		return FALSE;

	EDbEntityType type = dbEntity->Type();

	//if (type == DBFEATURE && ((CDbFeature*) dbEntity)->IsWorkZone())
	//	return TRUE;

	return (type >= DBLINE && type <= DBCOMMAND);
}

// static
void
CDbSequence::ConditionalSequence(
							CDbEntity*	refEntity,
							CDbEntity*	newEntity,
							bool		after )
{
	CDbSequence* dbSequence = refEntity->Sequence();
	if (dbSequence == NULL)
		return;

	int	pos = dbSequence->Position( refEntity );
	if (pos < 0)
		return;

	if ( after )
		dbSequence->InsertAfter( pos, newEntity );
	else
		dbSequence->InsertBefore( pos, newEntity );
}

