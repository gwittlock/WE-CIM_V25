
#include "stdafx.h"
#include "SelectorStack.h"



////////////////////////////////////////////////////////////////////////

CSelectorStack::CSelectorStack()
	: m_list(),
	  m_model( NULL )
{
}

CSelectorStack::~CSelectorStack()
{
	Flush();
}

void
CSelectorStack::Init( CModel& model )
{
	m_model = &model;
	Push();
}

void
CSelectorStack::Flush()
{
	int count = m_list.Count();
	for (int indx = (count-1); indx >= 0; --indx)
	{
		TopmostState( FALSE );
		delete m_list.Remove( indx );
	}
	m_list.DestructiveFlush();
}

CSelector&
CSelectorStack::operator() ()
{
	int count = m_list.Count();
	CSelector* selector = m_list[ count-1 ];
	return (*selector);
}

void
CSelectorStack::Push()
{
	TopmostState( FALSE );

	CSelector* selector = new CSelector( (*m_model) );

	m_list.Append( selector );
}

void
CSelectorStack::Pop()
{
	int count = m_list.Count();
	if (count < 2)
		return;  // We should always have at least one selector on the stack.

	TopmostState( FALSE );
	delete m_list.Remove( count-1 );
	TopmostState( TRUE );
}

void
CSelectorStack::TopmostState( bool select )
{
	int count = m_list.Count();

	if (count < 1)
		return;  // nothing to do

	CSelector* selector = m_list[ count-1 ];

	for (int indx = 0; indx < selector->Count(); ++indx)
	{
		CDbEntity* dbEntity = (*selector)[ indx ];

		dbEntity->SelectFlag( (select != 0) );
	}
}
