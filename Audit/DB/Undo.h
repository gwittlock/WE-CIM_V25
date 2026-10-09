
#ifndef _UNDO_H
#define _UNDO_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"

class CEntityDb;

//typedef CIndxList<CDbEntityList*> CUndoBuffer;
typedef CDynamicArray<CDbEntityArray*> CUndoBuffer;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CUndo
{
public:

	// By default, the undo buffer is active.
	CUndo( CEntityDb* db );

	// Remove all deltas from the undo buffer.
	void Flush();

	// Allows Record() to work.
	void Activate();

	// Suppresses any calls to Record().  This action is
	// necessary when excuting a File/Read, for instance.
	void Suppress();

	// Open the undo buffer to Record()'ing actions.
	// NOTE: If the undo buffer is not Open()'d prior
	// to calling Record(), an assertion will be fired.
	void Open();

	// Close the undo buffer to Record()'ing actions and
	// commit all recordings, since the last call to Open(),
	// to the undo buffer.  NOTE: Calls to Open()/Close()
	// are paired and can be nested.  Record()'d a items
	// will not be committed until the outermost Close()
	// is executed.
	void Close();

	// Record the current state of the given entity in the
	// undo buffer.  As the undo buffer is really a historical
	// log, Record() should be called prior to any changes in
	// entity state.  That is, the database always retains the
	// most recent form of the entity, and the undo buffer
	// retains the previous form.  NOTE: If the undo buffer
	// is not Open()'d prior to calling Record(), an
	// assertion will be fired.
	void Record( CDbEntity* dbEntity );

	// Undo all of the changes recorded between the last
	// pair of calls to Open()/Close().
	CReturn Undo();

	// Redo all of the changes that were just undone.
	// NOTE: If any new action is Record()'d after an
	// Undo(), all changes residing in the undo buffer
	// that lay beyond the current undo record, will be
	// lost.
	CReturn Redo();

	// Get the transaction depth of the undo buffer.
	int Depth();

	// Set the transaction depth of the undo buffer.
	void Depth( int depth );

	virtual ~CUndo();

protected:

private:  // Methods

	void FlushLost();
	void Dump( const char* caption );

private:  // Data

	// Disabled.
	CUndo();
	CUndo( const CUndo& );
	const CUndo& operator = ( const CUndo& );
	int operator == ( const CUndo& ) const;
	int operator != ( const CUndo& ) const;

private:

	// The entity database associated with this undo buffer.
	CEntityDb* m_db;

	// The undo buffer.
	CUndoBuffer m_undoBuffer;

	// The buffer for intermediate results.
	CDbEntityArray* m_intermediate;

	bool m_isActive;

	int m_bufferDepth;

	// Count of conecutive calls to Open().
	int m_open;

	// The index to the current record.
	int m_curr;
};

#endif
