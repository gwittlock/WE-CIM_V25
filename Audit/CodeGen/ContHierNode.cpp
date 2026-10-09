
#include "stdafx.h"
#include "cmn_resource.h"
#include "StringConst.h"
#include "ContHierNode.h"


////////////////////////////////////////////////////////////////////////

CContHierNode::CContHierNode()
	: m_entity( NULL ),
	  m_contained()
{
}

CContHierNode::~CContHierNode()
{
	// Delete the 'containment index'.
	if (m_entity != NULL)
		m_entity->AttribDelete( STR_HIER );
}

void
CContHierNode::Root( CDbEntity* dbEntity )
{
	m_entity = dbEntity;
	m_entity->IntSet( STR_HIER, (int) this );
}

void
CContHierNode::Append( CDbEntity* dbEntity )
{
	m_contained.Append( dbEntity );
}

int
CContHierNode::RootDepth()
{
	return (m_entity->IntGet( STR_CD, -1 ));
}

void
CContHierNode::Sort()
{
	m_contained.Qsort( CContHierNode::Compare );
}

void
CContHierNode::Reduce()
{
	CDbEntity*	dbEntity;
	int			depthA;
	int			depthB;
	int			indx;

	if (m_entity == NULL)
		depthA = -1;  // for the case where this node represent the recursive root node.
	else
		depthA = m_entity->IntGet( STR_CD, -1 );

	indx = 0;
	while (indx < m_contained.Count())
	{
		dbEntity = m_contained[indx];

		depthB = dbEntity->IntGet( STR_CD, -1 );

		if (depthB != (depthA + 1))
		{
			m_contained.Remove( indx );
		}
		else
		{
			++indx;
		}
	}
}

// For sorting the hier on increasing order of containment.
int CContHierNode::Compare( const void* ptrA, const void* ptrB )
{
	CDbEntity* entityA = ( *(CDbEntity**) ptrA);
	CDbEntity* entityB = ( *(CDbEntity**) ptrB);

	int depthA = entityA->IntGet( STR_CD, 0 );
	int depthB = entityB->IntGet( STR_CD, 0 );

	return (depthA - depthB);
}

