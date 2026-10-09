
#include "stdafx.h"
#include "MathConst.h"

#include "IndxList.h"
#include "2dCoord.h"
#include "3dCoord.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "WmIntRec.h"
#include "WmElem.h"
#include "WmSubchn.h"

typedef CIndxList<CWmElem*> CWmElemList;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmSubchn::CWmSubchn( CWmChain* owner )

	: CWmNode(),
	  m_owner( owner ),
	  m_prev( NULL ),
	  m_next( NULL ),
	  m_iindex( -IUNDEFINED ),
	  m_intrec( NULL )
{
}

CWmSubchn::~CWmSubchn()
{
}

CWmChain*
CWmSubchn::Owner() const
{
	return m_owner;
}

void
CWmSubchn::Owner( CWmChain* owner )
{
	m_owner = owner;
}

bool
CWmSubchn::IsAssigned() const
{
	return (m_iindex > -IUNDEFINED);
}

int
CWmSubchn::Iindex() const
{
	return m_iindex;
}

void
CWmSubchn::Iindex( int iindex )
{
	m_iindex = iindex;
}

int
CWmSubchn::Count()
{
	return ((m_intrec == NULL) ? 0 : m_intrec->Count());
}

bool
CWmSubchn::IsTerminal() const
{
	return (m_next == NULL);
}

CWmSubchn*
CWmSubchn::Other() const
{
	CWmSubchn* other = Other( m_iindex ) ;
	return other;
}

CWmSubchn*
CWmSubchn::Other( int iindex ) const
{
	if (m_intrec != NULL && m_intrec->IsValid())
	{
		int count = m_intrec->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CWmSubchn* subchn = m_intrec->ExtData( indx )->Subchn();

			if ( (subchn != NULL) &&
				 (subchn != this) &&
				 (iindex == subchn->Iindex()) )
					return subchn;
		}
	}

	return NULL;
}

CWmSubchn*
CWmSubchn::Sister() const
{
	int count = m_intrec->Count();
	if (m_intrec != NULL && count == 2)
	{
		for (int indx = 0; indx < count; ++indx)
		{
			CWmSubchn* subchn = m_intrec->ExtData( indx )->Subchn();
			if (subchn != this)
				return subchn;
		}
	}

	return NULL;
}

C3dCoord
CWmSubchn::StartPt() const
{
	C3dCoord ps;

	CWmElem* wmelem = First();
	if (wmelem != NULL)
		ps = wmelem->Elem()->StartPt();

	return ps;
}

C3dCoord
CWmSubchn::EndPt() const
{
	C3dCoord pe;

	CWmElem* wmelem = Last();
	if (wmelem != NULL)
		pe = wmelem->Elem()->EndPt();

	return pe;
}

CWmSubchn*
CWmSubchn::SubchnPrev() const
{
	return m_prev;
}

CWmSubchn*
CWmSubchn::SubchnNext() const
{
	return m_next;
}

void
CWmSubchn::SubchnPrev( CWmSubchn* node )
{
	m_prev = node;
}

void
CWmSubchn::SubchnNext( CWmSubchn* node )
{
	m_next = node;
}

CWmElem*
CWmSubchn::First() const
{
	return (dynamic_cast<CWmElem*>( Next() ));
}

CWmElem*
CWmSubchn::Last() const
{
	CWmSubchn* next = SubchnNext();
	if (next == NULL)
		return NULL;

	return (dynamic_cast<CWmElem*>( next->Prev() ));
}

void
CWmSubchn::Unlink()
{
	if (m_prev != NULL)
		m_prev->m_next = m_next;

	if (m_next != NULL)
		m_next->m_prev = m_prev;

	m_prev = NULL;
	m_next = NULL;

	CWmNode::Unlink();
}

void
CWmSubchn::OwnerShipUpdate()
{
	CWmElem* wmelem = dynamic_cast<CWmElem*>( Next() );
	while (wmelem != NULL)
	{
		wmelem->Owner( this );
		wmelem = dynamic_cast<CWmElem*>( wmelem->Next() );
	}
}

// See also CWmChain::PurgeSubchns()
void
CWmSubchn::Purge()
{
	m_iindex =  -IUNDEFINED;
	m_intrec = NULL;
}

// For debugging.
bool CWmSubchn::AnyElemAttribs() const
{
	CWmElem* curr = First();
	if (curr != NULL)
	{
		CWmElem* terminal = Last();
		while ((curr != NULL) && (curr != terminal))
		{
			if ( curr->Elem()->HasAttrib() )
				return true;

			curr = (CWmElem*) curr->Next();
		}
	}

	return false;
}
