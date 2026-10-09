
#ifndef _CLFILEREC_H
#define _CLFILEREC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Type.h"

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _GEOELEM_H
#include "GeoElem.h"
#endif

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

#ifndef _VARLIST_H
#include "VarList.h"
#endif

enum ItemType { Int, Dbl, Str };


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class ClfileRec  
{
public:

	// recType  -- is used to branch in the post main.
	// dbEntity -- records the entity that is ref'd in generating this record.
	// varList  -- is used to obtain attributes like feed, speed ,etc.
	//
	// Historically, the varList was obtained from the dbEntity.  Supporting
	// Xilog controllers requires that we provide stock dimensions, however.
	// As stock dimensions are stored the model header, and the model is not
	// a dbEntity, the varList reference was seperated from the dbEntity
	// reference.
	ClfileRec( int recType );

	virtual ~ClfileRec();

	int RecType() const		{ return m_recType; }

	CGeoElem* Elem() const				{ return m_elem; }
	void Elem( CGeoElem* elem )			{ m_elem = elem; }

	const CDbEntity* Entity() const		{ return m_dbEntity; }
	const CVarList* Attrib() const		{ return m_attribs; }

	void Entity( const CDbEntity* dbEntity )		{ m_dbEntity = dbEntity; }
	void Attrib( const CVarList* attribs)			{ m_attribs = ((CVarList*) attribs); }

	// NOTE: These methods were added to support user-commands.
	void AttribsCopy( const CVarList& attribs );
	void AttribsAppend( const CVarList& attribs );

private:

	// Disabled.
	ClfileRec();
	ClfileRec& operator = ( const ClfileRec& );
	int operator == ( const ClfileRec& );
	int operator != ( const ClfileRec& );

private:

	int m_recType;

	CGeoElem* m_elem;

	const CDbEntity* m_dbEntity;
	
	bool m_owns;

	CVarList* m_attribs;
};

typedef CIndxList<ClfileRec*> ClfileRecList;

#endif

