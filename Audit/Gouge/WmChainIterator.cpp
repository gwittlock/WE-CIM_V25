
#include "stdafx.h"
#include "WmNode.h"
#include "WmElem.h"
#include "WmChain.h"
#include "WmSubchn.h"
#include "WmChainIterator.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChainIterator::CWmChainIterator()
	: m_chain( NULL ),
	  m_node( NULL )
{
}

CWmChainIterator::CWmChainIterator( const CWmChain& chain )
{
	Init( chain );
}

CWmChainIterator::~CWmChainIterator()
{
}

void
CWmChainIterator::Init( const CWmChain& chain )
{
	m_chain = (CWmChain*) &chain;
	GotoStart();
}

const CWmChainIterator&
CWmChainIterator::operator = ( const CWmChainIterator& iter )
{
	m_chain = iter.m_chain;
	m_node  = iter.m_node;
	return (*this);
}

CWmSubchn*
CWmChainIterator::Subchn()
{
	CWmSubchn* subchn = dynamic_cast<CWmSubchn*>( m_node );
	if (subchn == NULL)
	{
		CWmElem* wmelem = dynamic_cast<CWmElem*>( m_node );
		if (wmelem != NULL)
			subchn = wmelem->Owner();
	}

	return subchn;
}

CWmElem*
CWmChainIterator::Elem()
{
	CWmElem* wmelem = dynamic_cast<CWmElem*>( m_node );
	if (wmelem == NULL)
	{
		CWmSubchn* subchn = dynamic_cast<CWmSubchn*>( m_node );
		if (subchn != NULL)
			wmelem = dynamic_cast<CWmElem*>( m_node->Next() );
	}

	return wmelem;
}

bool CWmChainIterator::AtStart()
{
	return (m_node == m_chain->First());
}

bool CWmChainIterator::AtEnd()
{
	return (m_node == NULL);	// (m_node == m_chain->Last());
}

void
CWmChainIterator::GotoStart()
{
	m_node = m_chain->First();
	NextElem();
}

void
CWmChainIterator::GotoEnd()
{
	m_node = m_chain->Last();
}

void
CWmChainIterator::NextSubchn()
{
	CWmSubchn* subchn = dynamic_cast<CWmSubchn*>( m_node );
	if (subchn == NULL)
	{
		CWmElem* wmelem = dynamic_cast<CWmElem*>( m_node );
		if (wmelem != NULL)
			subchn = wmelem->Owner();
	}

	if (subchn != NULL)
		m_node = subchn->SubchnNext();
}

void
CWmChainIterator::NextElem()
{
	if (m_node == NULL)
		return;

	while (1)
	{
		m_node = m_node->Next();
		if (m_node == NULL)
			break;

		if (dynamic_cast<CWmElem*>( m_node ) != NULL)
			break;
	}
}

void
CWmChainIterator::PrevSubchn()
{
	CWmSubchn* subchn = dynamic_cast<CWmSubchn*>( m_node );
	if (subchn == NULL)
	{
		CWmElem* wmelem = dynamic_cast<CWmElem*>( m_node );
		if (wmelem != NULL)
			subchn = wmelem->Owner()->SubchnPrev();
	}
	else
	{
		subchn = subchn->SubchnPrev();
	}

	if (subchn != NULL)
		m_node = subchn;
}

void
CWmChainIterator::PrevElem()
{
	if (m_node == NULL)
		return;

	while (1)
	{
		m_node = m_node->Prev();
		if (m_node == NULL)
			break;

		if (dynamic_cast<CWmElem*>( m_node ) != NULL)
			break;
	}
}


