
#ifndef _CODEGEOREC_H
#define _CODEGEOREC_H

#include "GeoElem.h"
#include "DbEntity.h"

#include "Indxlist.h"


// =======================================================================
// This class correlates a block of code with the entity from which
// the code was generated.  The class is necessary because there can
// be a many-to-one relationship between blocks of code and an entity.
//
class dllExport CCodeGeoRec
{
public:

	CCodeGeoRec( int recType, const CDbEntity* dbEntity, const CString& text );

	ID Id() const						{ return ((m_dbEntity == NULL) ? 0 : m_dbEntity->Id()); }

	int RecType() const					{ return m_recType; }

	const CString& Text() const			{ return m_text; }

	const CDbEntity* Entity() const		{ return m_dbEntity; }

	// NOTE: This object takes ownership of the geometry.
	void GeoElemSet( CGeoElem* elem )	{ m_geoElem = elem; }
	const CGeoElem* GeoElemGet() const	{ return m_geoElem; }

	virtual ~CCodeGeoRec();

private:  // disabled

	CCodeGeoRec();

private:  // data

	int			m_recType;
	const CDbEntity*	m_dbEntity;
	CString		m_text;

	// 2007.05.03 (PE) -- Introduced to support display of clamp
	// avoidance movements, but more generally, this data can be
	// used to model data that is not in the clfile.
	CGeoElem*	m_geoElem;
};


typedef CIndxList<CCodeGeoRec*> CCodeGeoRecList;

#endif
