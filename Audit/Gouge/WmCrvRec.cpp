
#include "stdafx.h"

#include "WmElem.h"
#include "WmSubchn.h"
#include "WmCrvRec.h"



////////////////////////////////////////////////////////////////////////

CWmCrvRec::CWmCrvRec( CWmElem* node )

	: m_wmelem( node ),
	  m_list()
{
}

CWmCrvRec::~CWmCrvRec()
{
	m_list.DestructiveFlush();
}

CWmCrvRecExtData*
CWmCrvRec::Update( const CWmIntRec* intRec, double uparam )
{
	CWmCrvRecExtData* crvrec = NULL;

	int indx, count = m_list.Count();
	for (indx = 0; indx < count; ++indx)
	{
		crvrec = m_list[indx];

		if (intRec != NULL && crvrec->IntRec() == intRec)
			break;  // already recorded

		double diff = uparam - crvrec->Uparam();

		if (fabs( diff ) < VECTOR_SMALL)
			break;  // already recorded (may need to check point proximity instead)

		if (diff < 0.0)
		{
			crvrec = new CWmCrvRecExtData( this, intRec, uparam );
			m_list.InsertBefore( indx, crvrec );
			break;
		}
	}

	if (indx >= count)
	{
		crvrec = new CWmCrvRecExtData( this, intRec, uparam );
		m_list.Append( crvrec );
	}

	return crvrec;
}


