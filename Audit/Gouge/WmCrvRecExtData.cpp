
#include "stdafx.h"
#include "GeoElem.h"
#include "WmIntRec.h"
#include "WmSubchn.h"
#include "WmCrvRec.h"
#include "WmCrvRecExtData.h"



////////////////////////////////////////////////////////////////////////

CWmCrvRecExtData::CWmCrvRecExtData( const CWmCrvRec* owner, const CWmIntRec* intRec, double uparam )

	: m_owner( owner ),
	  m_intrec( intRec ),
	  m_uparam( uparam ),
	  m_subchn( NULL ),
	  m_tagged( FALSE )
{
}

CWmCrvRecExtData::~CWmCrvRecExtData()
{
}

C2dCoord
CWmCrvRecExtData::Point() const
{
	if (m_intrec == NULL)
	{
		const CGeoElem* elem = m_owner->Elem();

		if (fabs(m_uparam) < SMALL)
			return elem->StartPt();
		else
			return elem->EndPt();
	}
	else
	{
		return m_intrec->Point();
	}
}

void
CWmCrvRecExtData::Subchn( CWmSubchn* subchn )
{
	m_subchn = subchn;
	if (m_subchn != NULL)
		m_subchn->IntRec( m_intrec );
}


