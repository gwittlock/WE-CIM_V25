
#include "stdafx.h"
#include "Return.h"
#include "EntityDb.h"
#include "DbSequence.h"
#include "DbSeqIterator.h"



////////////////////////////////////////////////////////////////////////

CDbSeqIterator::CDbSeqIterator()
	: m_db( NULL ),
	  m_entities(),
	  m_indx( -1 )
{
}

CDbSeqIterator::~CDbSeqIterator()
{
	m_entities.BenignFlush();
}

// Standard init, starts at the "RootSequence" node.  Also sets the current
// index to zero, allowing the first access operation to return a valid entity.
// Subsequently, must call Next() prior to accessing the next entity.
CReturn
CDbSeqIterator::Init( CEntityDb* db, ID id )
{
	CReturn status;
	CDbSequence* dbSeq;

	m_db = db;

	status = m_db->Find( id, (CDbEntity**) &dbSeq, DBSEQUENCE, DBSEQUENCE );
	if ( status.IsOk() )
	{
		m_entities.BenignFlush();

		// Flatten the root sequence such that the flattened sequence contains
		// sequence entities, and each entity carries 'nesting level' information.
		dbSeq->Flatten( DBSEQ_NO_FLAGS, &m_entities );

		m_indx = 0;
	}

	return status;
}

CReturn
CDbSeqIterator::Next()
{
	CReturn status;

	int count = m_entities.Count();
	if (count <= 0)
	{
		status.Diagnostic( "CDbSeqIterator::Next() -- iterator has not been initialized?" );
	}
	else
	{
		if (m_indx < count)
			++m_indx;
		else if (m_indx >= count)
			status.Diagnostic( "CDbSeqIterator::Next() -- attempt to iterate beyond end?" );
	}

	return status;
}

// Access operator
CDbEntity*
CDbSeqIterator::operator()()
{
	int count = m_entities.Count();
	return ((m_indx < 0 || m_indx >= count) ? NULL : m_entities[m_indx]);
}
