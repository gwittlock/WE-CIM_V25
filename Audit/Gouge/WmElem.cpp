
#include "stdafx.h"

#include "WmElem.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmElem::CWmElem( CWmSubchn* owner, CGeoElem* elem )
	: CWmNode(),
	  m_owner( owner ),
	  m_elem( elem )
{
}

CWmElem::~CWmElem()
{
	delete m_elem;
}

void
CWmElem::Unlink()
{
	m_owner = NULL;
	CWmNode::Unlink();
}

