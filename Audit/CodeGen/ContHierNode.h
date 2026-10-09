
#ifndef _CONTHIERNODE_H
#define _CONTHIERNODE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

class CContHier;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Containment Hierarchy
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CContHierNode
{
	friend class CContHier;

public:

	CContHierNode();

	void Root( CDbEntity* dbEntity );

	void Append( CDbEntity* dbEntity );
	
	CDbEntity* Root() const						{ return m_entity; }

	const CDbEntityArray& Contained() const		{ return m_contained; }

	CDbEntityArray* pContained()				{ return &m_contained; }

	int RootDepth();

	void Reduce();

	virtual ~CContHierNode();

protected:

	void Sort();

private:  // Methods

	static int Compare( const void* ptrA, const void* ptrB );

private:  // Disabled

	CContHierNode( const CContHierNode& );
	const CContHierNode& operator = ( const CContHierNode& );
	int operator == ( const CContHierNode& ) const;
	int operator != ( const CContHierNode& ) const;

private:  // Data

	CDbEntity* m_entity;
	CDbEntityArray m_contained;
};

typedef CDynamicArray<CContHierNode*> CCHNArray;

#endif

