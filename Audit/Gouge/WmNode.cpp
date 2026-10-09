
#include "stdafx.h"

#include "WmNode.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmNode::CWmNode()

	: m_prev( NULL ),
	  m_next( NULL )
{
}

CWmNode::~CWmNode()
{
	ASSERT( (m_prev == NULL && m_next == NULL) );
}

CWmNode*
CWmNode::Prev() const
{
	return m_prev;
}

CWmNode*
CWmNode::Next() const
{
	return m_next;
}

CWmNode*
CWmNode::Prev( CWmNode* node )
{
	node->m_prev = m_prev;
	node->m_next = this;

	if (m_prev != NULL)
		m_prev->m_next = node;

	m_prev = node;

	return node->m_prev;
}

CWmNode*
CWmNode::Next( CWmNode* node )
{
	node->m_prev = this;
	node->m_next = m_next;

	if (m_next != NULL)
		m_next->m_prev = node;

	m_next = node;

	return node->m_next;
}

void
CWmNode::Unlink()
{
	if (m_prev != NULL)
		m_prev->m_next = m_next;

	if (m_next != NULL)
		m_next->m_prev = m_prev;

	m_prev = NULL;
	m_next = NULL;
}

