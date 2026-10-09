
#ifndef _DBITERATOR_H
#define _DBITERATOR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDbIterator
{
public:

	CDbIterator();

	void Init(
			const CEntityDb&	db,
			EDbEntityType		startType,
			bool				skipDeletedEntities = TRUE,
			int					startIndex = -1 );

	CDbEntity* operator()();

	void Next();

	virtual ~CDbIterator();

protected:

private:  // Methods

private:  // Disabled

	CDbIterator( const CDbIterator& );
	const CDbIterator& operator = ( const CDbIterator& );
	int operator == ( const CDbIterator& ) const;
	int operator != ( const CDbIterator& ) const;

private:  // Data

	const CEntityDb*	m_db;
	EDbEntityType		m_type;
	bool				m_skip;
	int					m_indx;
};

#endif

