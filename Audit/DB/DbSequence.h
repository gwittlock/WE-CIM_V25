
#ifndef _DBSEQUENCE_H
#define _DBSEQUENCE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

// These attributes are used mostly by CSequenceProcessApp::Dump()
// for text formatting.
#define DBSEQ_NO_FLAGS		0x000
#define DBSEQ_ADD_SUBSEQS	0x001	// include subsequence entities
#define DBSEQ_TAG_ADDED		0x002	// tag only entities that are added to the result
#define DBSEQ_TAG_SUBSEQS	0x004	// tag subsequence entities as they are traversed
#define DBSEQ_LEVEL			0x008	// adds '~level' attribute to each entity, causing indenting
#define DBSEQ_RECURSE		0x010
#define DBSEQ_WORKZONE		0x020	// include the workzone in the sequence


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
class dllExport CDbSequence : public CDbEntity
{
	friend class CEntityDb;

public:

	virtual EDbEntityType Type() const   { return DBSEQUENCE; }

	virtual CDbWorkplane* Workplane() const		{ return NULL; }
	virtual CDbTool* Tool() const				{ return NULL; }

#pragma warning( push )
#pragma warning( disable : 4100 )
	virtual void Workplane( CDbWorkplane* workplane )	{ /* do nothing */ }
	virtual void Tool( CDbTool* Tool )					{ /* do nothing */ }
#pragma warning( pop )

	// Obtain the count of entities owner by this container.
	int Count() const;

	// Obtain a pointer to the Ith entity.
	CDbEntity* operator[]( int indx ) const;

	// Find the zero-based index position of the given entity
	// in this container.  Returns -1 when not found.
	int Position( const CDbEntity* dbEntity ) const;

	// Insert an entity at the beginning of this container.
	void Prepend( CDbEntity* dbEntity );

	// Add an entity to this container.
	void Append( CDbEntity* dbEntity );

	void InsertBefore( int indx, CDbEntity* dbEntity );

	void InsertAfter( int indx, CDbEntity* dbEntity );

	// Release ownership of the given entity.
	// Returns true if successful.
	bool Disown( CDbEntity* dbEntity );

	// Remove the entity from this sequence.  The value of
	// indx should be obtained from Position().
	CDbEntity* Remove( int indx );

	// Recursively descend this sequence, adding child entities to the list.
	//   See above #define DBSEQ_
	void Flatten( int flags, CDbEntityArray* entities );

	void BenignFlush();

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	virtual void RefsTo( CDbEntityList* list ) const;

	virtual void Subordinates( CDbEntityList* list ) const;

	virtual void RemoveRef( CDbEntity* dbEntity );

	virtual CReturn Clone( CDbEntity** dbEntity ) const;

	virtual CReturn ContentsSwap( CDbEntity* dbEntity );

public:

	static bool CanSequence( CDbEntity* dbEntity );

	static void ConditionalSequence(
							CDbEntity*	refEntity,
							CDbEntity*	newEntity,
							bool		after );
protected:

private:  // Methods

	CDbSequence( CEntityDb* db );
	CDbSequence( const CDbSequence& dbSequence );
	virtual ~CDbSequence();

	bool IsValid( const CDbEntity* dbEntity );
	void Error( const CString& msg );

private:  // Disabled

	CDbSequence();
	const CDbSequence& operator = ( const CDbSequence& );
	int operator == ( const CDbSequence& ) const;
	int operator != ( const CDbSequence& ) const;

private:  // Data

	CDbEntityList m_list;
};

#endif

