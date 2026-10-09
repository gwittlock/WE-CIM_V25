
#include "stdafx.h"

#include "DbEntity.h"
#include "EntityDb.h"
#include "DbIterator.h"


////////////////////////////////////////////////////////////////////////

CDbIterator::CDbIterator()
	: m_db( NULL ),
	  m_type( DBTERMINAL ),
	  m_skip( TRUE ),
	  m_indx( -1 )
{
}

CDbIterator::~CDbIterator()
{
}

void
CDbIterator::Init(
				const CEntityDb&	db,
				EDbEntityType		startType,
				bool				skipDeletedEntities,
				int					startIndex )
{
	m_db	= &db;
	m_type	= startType;
	m_indx	= startIndex;
	m_skip	= skipDeletedEntities;

	Next();
}

CDbEntity*
CDbIterator::operator()()
{
	if (m_type >= DBTERMINAL)
		return NULL;

	CDbEntity* dbEntity = m_db->m_list[ m_type ][ m_indx ];

	return dbEntity;
}

void
CDbIterator::Next()
{
	++m_indx;

	while (1)
	{
		if (m_type >= DBTERMINAL)
			return;  // At end.  Nothing to do.

		int count = m_db->m_list[ m_type ].Count();

		while (m_indx < count)
		{
			if ( m_skip )
			{
				CDbEntity* dbEntity = m_db->m_list[ m_type ][ m_indx ];
				if ( dbEntity->IsDeleted() )
					++m_indx;
				else
					return;
			}
			else
				return;
		}

		int tmp = m_type;

		m_type = (EDbEntityType) (tmp + 1);
		m_indx = 0;
	}
}

