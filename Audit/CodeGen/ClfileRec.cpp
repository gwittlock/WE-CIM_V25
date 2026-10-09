
#include "stdafx.h"
#include "ClfileRec.h"

ClfileRec::ClfileRec( int recType )

	: m_recType( recType ),
	  m_elem( NULL ),
	  m_dbEntity( NULL ),
	  m_owns( FALSE ),
	  m_attribs( NULL )
{
}

ClfileRec::~ClfileRec()
{
	delete m_elem;

	if ( m_owns )
		delete m_attribs;
}

#if 0
const CVarList&
ClfileRec::Attrib() const
{
	return ((m_attribs == NULL) ? CVarList::Bogus() : (*m_attribs));
}
#endif

void
ClfileRec::AttribsCopy( const CVarList& attribs )
{
	m_attribs = new CVarList( attribs );
	m_owns = TRUE;
}

void
ClfileRec::AttribsAppend( const CVarList& attribs )
{
	if ( m_owns )
		(*m_attribs) += attribs;
}



