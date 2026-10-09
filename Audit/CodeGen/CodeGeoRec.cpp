
#include "stdafx.h"
#include "CodeGeoRec.h"


// =======================================================================

CCodeGeoRec::CCodeGeoRec( int recType, const CDbEntity* dbEntity, const CString& text )

	: m_recType( recType ),
	  m_dbEntity( dbEntity ),
	  m_text(text),
	  m_geoElem( NULL )
{
}

CCodeGeoRec::~CCodeGeoRec()
{
	delete m_geoElem;
}

