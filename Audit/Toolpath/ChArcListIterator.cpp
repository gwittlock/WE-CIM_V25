
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "ChArc.h"
#include "ChArcListIterator.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// See also Docs/Minkowski.htm
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


////////////////////////////////////////////////////////////////////////
	
CChArcListIterator::CChArcListIterator()
	: m_list( NULL ),
	  m_indx( 0 )
{
}

CChArcListIterator::CChArcListIterator( const CChArcList& arcList )
	: m_list( &arcList ),
	  m_indx( 0 )
{
}

void
CChArcListIterator::Init( const CChArcList& arcList )
{
	// Would rather m_list be const, but the compiler won't allow it.
	m_list = &arcList;
	m_indx = 0;
}

CChArcListIterator::~CChArcListIterator()
{
}

const CChArc*
CChArcListIterator::operator()() const
{
	if (m_indx < 0 || m_indx >= m_list->Count())
		return NULL;

	return (*m_list)[ m_indx ];
}

bool
CChArcListIterator::AtEnd() const
{
	int count = m_list->Count() - 1;
	return (m_indx == count);
}

const CChArc*
CChArcListIterator::Prev()
{
	--m_indx;
	if (m_indx < 0)
		m_indx = m_list->Count() - 1;

	return (*m_list)[ m_indx ];
}

const CChArc*
CChArcListIterator::Next()
{
	++m_indx;
	if (m_indx >= m_list->Count())
		m_indx = 0;

	return (*m_list)[ m_indx ];
}

int
CChArcListIterator::Advance( const CChArc* partArc, int offsetDir )
{
	int result = 0;
	int indx;

	//if ( CChArc::SameDirection( partArc, toolArc ) )
	if ( partArc->IsMove() )
	{
		indx = ((offsetDir > 0) ? PrevIndex( m_indx ) : NextIndex( m_indx ));
	}
	else
	{
		// indx = ((offsetDir > 0) ? PrevIndex( m_indx ) : NextIndex( m_indx ));
		indx = ((partArc->Dir() < 0) ? PrevIndex( m_indx ) : NextIndex( m_indx ));
	}

	result = CChArc::OverlapClassify( partArc, (*m_list)[indx], offsetDir );

	if (result > 0)
		m_indx = indx;

	return result;
}

// NOTE: This method should be called only when iterating
// a convex hull that represents a tool.  Remember that a
// tool must be truely convex an must have a CCW orientation.
int
CChArcListIterator::StartPosition( const CChArcListIterator& partIter, int offsetDir )
{
	int result = 0;

	int bestIndx = -1;
	int bestClass = -1;

	bool atEnd = partIter.AtEnd();

	const CChArc* partArc = partIter();
	if (partArc == NULL)
		return 0;

	int indx = m_indx;

	while (1)
	{
		CChArc* toolArc = (*m_list)[indx];

		int classification = CChArc::OverlapClassify( partArc, toolArc, offsetDir );

		if (classification == 0)
		{
			if (bestIndx >= 0)
			{
				m_indx = bestIndx;
				result = bestClass;
				break;
			}
		}
		else
		{
			bestIndx = indx;
			bestClass = classification;
		}

		if ( atEnd )
		{
			if ( partArc->IsMove() )
				indx = ((offsetDir > 0) ? NextIndex( indx ) : PrevIndex( indx ));
			else
				indx = ((partArc->Dir() > 0) ? NextIndex( indx ) : PrevIndex( indx ));
		}
		else
		{
			if ( partArc->IsMove() )
				indx = ((offsetDir > 0) ? PrevIndex( indx ) : NextIndex( indx ));
			else
				indx = ((partArc->Dir() > 0) ? PrevIndex( indx ) : NextIndex( indx ));
		}
	}

	return result;
}

int
CChArcListIterator::PrevIndex( int indx )
{
	--indx;;

	if (indx < 0)
		indx = m_list->Count() - 1;

	return indx;
}

int
CChArcListIterator::NextIndex( int indx )
{
	++indx;

	if (indx >= m_list->Count())
		indx = 0;

	return indx;
}
