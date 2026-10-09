
#ifndef _DBCONTAINER_H
#define _DBCONTAINER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"
#include "DynamicArray.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// An abstract class that objectifies entity ownership issues as
// pertains to container subclasses such as Profiles and Features.
//
class CDbContainer : public CDbEntity
{
	friend class CDbProfile;
	friend class CDbFeature;
	friend class CDbPattern;

public:

	// Obtain the count of entities owner by this container.
	virtual int Count() const = 0;

	// Obtain a pointer to the Ith entity.
	virtual CDbEntity* GetAt( int indx ) const = 0;
	virtual CDbEntity* operator[]( int indx ) const = 0;

	virtual CDbEntity* ReplaceAt( int indx, CDbEntity* dbEntity ) = 0;

	// Insert an entity (or a copy of) at the beginning of this container.
	virtual CReturn Prepend( CDbEntity* dbEntity, bool copy ) = 0;

	// Add an entity (or a copy of) to this container.
	virtual CReturn Append( CDbEntity* dbEntity, bool copy ) = 0;

	// Insert an entity in front of a known entity in this container.
	virtual int InsertBefore( CDbEntity* refEntity, CDbEntity* newEntity ) = 0;
	virtual int InsertBefore( int indx, CDbEntity* newEntity ) = 0;

	// Insert an entity behind a known entity in this container.
	virtual int InsertAfter( CDbEntity* refEntity, CDbEntity* newEntity ) = 0;
	virtual int InsertAfter( int indx, CDbEntity* newEntity ) = 0;

	// Find the position of the given curve in the profile (0 indexed).
	virtual int Position( const CDbEntity* refEntity ) const = 0;

	// Release ownership of the given entity.
	// Returns true if successful.
	virtual bool Disown( CDbEntity* dbEntity ) = 0;

	virtual void BenignFlush() = 0;
	virtual CReturn DestructiveFlush() = 0;

	// Get the atomic entities.
	virtual void Flatten( CDbEntityList* entities ) const = 0;

protected:

private:  // Methods

	CDbContainer( CEntityDb* db );
	CDbContainer( const CDbContainer& dbContainer );
	virtual ~CDbContainer();

private:  // Disabled

	CDbContainer();
	const CDbContainer& operator = ( const CDbContainer& );
	int operator == ( const CDbContainer& ) const;
	int operator != ( const CDbContainer& ) const;
};


typedef CDynamicArray<CDbContainer*> CDbContainerArray;

#endif

