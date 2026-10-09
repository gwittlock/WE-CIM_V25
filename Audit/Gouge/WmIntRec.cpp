
#include "stdafx.h"

#include "GeoElem.h"
#include "WmSubchn.h"
#include "WmIntRec.h"



////////////////////////////////////////////////////////////////////////

CWmIntRec::CWmIntRec( const C2dCoord& pt )

	: m_pt( pt ),
	  m_list()
{
}

CWmIntRec::~CWmIntRec()
{
	m_list.BenignFlush();
}

const CWmCrvRecExtData*
CWmIntRec::ExtData( int indx ) const
{
	if ((m_list.Count() > 0 && m_list.Head() == NULL) || indx >= m_list.Count())
	{
		int foo = 1;
	}
	return m_list[indx];
}

void
CWmIntRec::Update( const CWmCrvRecExtData* rec )
{
	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		if (m_list[indx] == rec)
			return;  // already recorded
	}

	m_list.Append( rec );
}

// See also comment in CWmChainIntsctor::Reduce()
void
CWmIntRec::Reduce()
{
	if (m_list.Count() > 2)
	{
		int indx = 0;
		while (1)
		{
			if (indx >= m_list.Count())
				break;

			const CWmCrvRecExtData* ext = m_list[indx];
			if ( ext->IsTagged() )
				m_list.Remove( indx );  // is tagged via CWmChainSplitter::Split()
			else
				++indx;
		}
	}
}

bool
CWmIntRec::IsValid() const
{
	if (m_list.Count() == 0)
		return true;

	return ((m_list.Head() != NULL) && (m_list.Tail() != NULL));
}
