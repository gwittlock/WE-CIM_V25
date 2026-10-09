
#ifndef _ENTITYDB_H
#define _ENTITYDB_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "GeoElem.h"
#include "DbEntity.h"


class CGeoElem;
class CDbCurve;
class CUndo;
class CDbIterator;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define SEQUENCE_GAP	1000000

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CEntityDb
{
	friend class CDbEntity;
	friend class CUndo;
	friend class CDbIterator;

public:

	CEntityDb();

	// Deletes all database entities.
	CReturn Flush();

	// Obtain the total count of database entities.
	int Count() const;

	// Obtain the count of a specific entity type.
	int Count( EDbEntityType type ) const;

	// Obtain a pointer to the entity having the given id.
	// The search can be restricted to the range of entity
	// types startType..endType (inclusive).
	CReturn Find(
		ID id,
		CDbEntity** dbEntity,
		EDbEntityType startType = DBWORKPLANE,
		EDbEntityType endType = DBTERMINAL ) const;

	// Obtain a pointer to the entity having the given name.
	// NOTE: See also CDbEntity::Name()
	CReturn Find(
		const CString& name,
		CDbEntity** dbEntity,
		EDbEntityType startType = DBWORKPLANE,
		EDbEntityType endType = DBTERMINAL ) const;

	// Find a tool by its contents
	CDbTool* DbToolMatch( const CDbTool& src_tool );
	// Introduced for Nesting, to 'create tool setups on-the-fly'.
	CDbTool* ToolFindCreate( const CDbTool& dbTool, bool create );


	// Check for the existence of a user-defined of system constant name.
	int NameFind( const CString& name );

	// Create an uninitialized entity in the database.
	// As this is a factory method, you must cast the
	// entity to the appropriate subclass, and then
	// call one of its Init() methods.
	CReturn Create( EDbEntityType type, CDbEntity** dbEntity );

	CDbEntity* GeoConvert(
					const CGeoElem&	geoElem,
					CDbTool*		dbTool,
					CDbWorkplane*	dbWork );

	// Automatically inits the entity.... whee! eww
	CReturn PrepareCopy( CEntityDb* io_dest );
	CReturn Copy( CDbEntity& in_entity, CDbEntity** dbEntity );
	bool	WasCopied( const CDbEntity& dbEntity ) const;

	// Calls TagClear() for every entity in the database.
	// Introduced to fix a bug in do_transform() and to reduce redundant code.

	// Replaced with CDbEntity::NewAction()

	// Delete the entity having the given id.
	CReturn Delete( CDbEntity** dbEntity );

	// Set m_nextId based on the entities in the database.
	void IdMaxInit();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Insert-point management and query...
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	int		InsertNext( void );
//	int		InsertAt( void );
//	void	InsertAt( int in_pos );
//	void	InsertReset( void );
//	CReturn	CutOrder( CDbEntityList* cut_list, bool filter );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Default Attribute management, through the varlist
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	const CVarList&	Default( void ) const		{ return m_defvar; }
	CVarList*		pDefault( void )			{ return &m_defvar; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Database traversal methods
	//   -- Use class CDbIterator
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


	// Obtain the list of all dirty entities.
	void DirtyEntities( CDbEntityList* list );

	void ClearDirty();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Low-level call introduced for macros.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CDbEntity* Get( EDbEntityType type, int indx );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// For forward migration of pre-V16 MM2 files.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	void SortByID( EDbEntityType type );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Undo system management.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// By default, the undo buffer is active when the
	// model is constructed.  While the buffer is active,
	// entity creation and modification operations will
	// be recorded.  See also UndoBufferPrepare() and
	// UndoBufferCommit().
	void UndoBufferActivate();
	void UndoBufferSuppress();

	void UndoBufferFlush();

	void UndoBufferPrepare();
	void UndoBufferCommit();

	void UndoBufferRecord( CDbEntity* dbEntity );

	CReturn Undo();
	CReturn Redo();

	int  UndoBufferDepth() const;
	void UndoBufferDepth( int depth );

	virtual ~CEntityDb();

protected:

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by friend CDbEntity.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Traverse the database, removing all references to this entity.
	void RemoveRefs( CDbEntity* dbEntity );

	// Add a user-defined or system constant name.
	int NameAdd( const CString& name );

	// Remove a user-defined or system constant name.
	int NameRemove( const CString& name );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by friend CUndo.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CReturn Remove( const CDbEntity& dbEntity );

private:  // Disabled

	CEntityDb( const CEntityDb& );
	const CEntityDb& operator = ( const CEntityDb& );
	int operator == ( const CEntityDb& ) const;
	int operator != ( const CEntityDb& ) const;

private:  // Methods

	CReturn UndoFind(
				ID id,
				CDbEntity** dbEntity,
				EDbEntityType startType,
				EDbEntityType endType ) const;

	// Find the index to an entity in a given bucket.
	int EntityIndex( EDbEntityType type, ID id ) const;

private:  // Data

	// The entity database sorted entities into
	// buckets of a particular type of entity.
	// CDbEntityList m_list[DBTERMINAL];
	CDbEntityArray m_list[DBTERMINAL];

	// Increasing order lexically sorted array of entity names.
	// This array contains only those names that are user-defined
	// or that are 'system constants' (like "TOP", "tool1").
	CDynamicArray<CString*> m_entityNames;

	// The undo/redo system used by this database.
	CUndo* m_undo;

	// Instead, let's have a list of default variables
	CVarList	m_defvar;

	// The id of the next entity to be created.
	ID m_nextId;

	// Tooled-entity sequencing support
	// Note that EntityDB only provides *tools*... it doesnt apply the seq.
	int	m_insert_pos;		// Sequence number to use next

	// Reference map, used while copying groups of entities to preserve
	//	relationships.  Used by PrepareCopy() and Copy().  BE CAREFUL. eww
	tDbEntityMap	m_reference_map;
	CEntityDb*		m_dest_db;
};

#endif

