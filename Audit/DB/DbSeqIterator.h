#if !defined(_DBSEQITERATOR_H)
#define _DBSEQITERATOR_H

// ==================================================================
//		DbSeqIterator
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "DbEntity.h"

class CEntityDb;


// ==================================================================

class dllExport CDbSeqIterator
{
public:

	CDbSeqIterator();

	CReturn Init( CEntityDb* db, ID id );

	CReturn Next();

	CDbEntity* operator()();

	virtual ~CDbSeqIterator();

protected:

private:
	// Disabled.
	CDbSeqIterator( const CDbSeqIterator& );
	const CDbSeqIterator& operator = ( const CDbSeqIterator& );
	int operator == ( const CDbSeqIterator& ) const;
	int operator != ( const CDbSeqIterator& ) const;

private:

	CEntityDb*		m_db;
	CDbEntityArray	m_entities;
	int				m_indx;
};

#endif

