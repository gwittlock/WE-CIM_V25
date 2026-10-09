
#include "stdafx.h"
#include "DbContainer.h"



////////////////////////////////////////////////////////////////////////

CDbContainer::CDbContainer( CEntityDb* db )
	: CDbEntity( db )
{
}

CDbContainer::CDbContainer( const CDbContainer& dbContainer )
	: CDbEntity( dbContainer )
{
}

CDbContainer::~CDbContainer()
{
}

// Obtain the count of entities owner by this container.
// virtual int Count() const = 0;

// Obtain a pointer to the Ith entity.
// virtual CDbCurve* operator[]( int indx ) const = 0;

// Add an entity to this container.
// virtual CReturn Append( CDbCurve* dbEntity ) = 0;

// Insert an entity behind a known entity in this container.
// virtual int InsertAfter( CDbEntity* leadEntity, CDbEntity* trailEntity ) = 0;

// Release ownership of the given entity.
// virtual bool Disown( CDbEntity* dbEntity ) = 0;
