
#ifndef _MODEL_H
#define _MODEL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"

#include "EntityDb.h"
#include "DbPattern.h"

#include "SelectorStack.h"
#include "TreeViewSupport.h"

class CDbWorkplane;



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CModel
{
public:

	CModel();

	// NOTE: The selector stack and tree view support objects
	// are not used by the model, but are communicated to
	// other objects via the model.  This was done primarily
	// to simplify life in the process dll area.
	void Init( CSelectorStack& selectorStack, CTreeViewSupport& treeViewSupport );

	// Delete all database entities.
	CReturn Flush();

	// Obtain the bounding box containing all model entities.
	C3dBox Box( ID workId = 0 ) const;

	// Bounding box for everything EXCEPT system entities
	C3dBox BoxUser( ID workId = 0 ) const;

	// Determine whether regeneration should be performed.
	bool IsRegenPending();

	// Obtain the list of entities that require regeneration.
	void RegenList( CDbEntityList* list );

	void ClearDirty();

	CSelectorStack& SelectorStack()  { return (*m_selectorStack); }
	CTreeViewSupport& Tree()  { return (*m_treeViewSupport); }

	// Returns true when the sheet is in the 4th quadrant (like a Whitney).
	bool IsYNegative() const;

	// Return true when model is built in a right-handed coordinate system.
	bool IsRightHanded() const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Default Attribute management, through the database
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	const CVarList&	Default( void ) const		{ return m_db.Default(); }
	const CVarList&	Header( void )	const		{ return m_header; }
	CVarList*		pDefault( void )			{ return m_db.pDefault(); }
	CVarList*		pHeader( void )				{ return &m_header; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Used during macro (java) execution
	const CVarList&	VarSys( void )	const		{ return m_varsys; }
	CVarList*		pVarSys( void )				{ return &m_varsys; }

	CReturn MacroRun(
				const CString&	javaFilePath,
				const CVarList&	javaCmdLineParams,
				bool			compile,
				bool			isRTL );


	CDbTool* ActiveTool() const;

	void ActiveTool( CDbTool* tool );

	CDbWorkplane* ActiveWorkplane() const;

	void ActiveWorkplane( CDbWorkplane* workplane );

	CDbPattern* ActivePattern( void ) const				{ return m_pattern; }
	CDbPattern* ActivePattern( CDbPattern* pattern )	{ CDbPattern* old = m_pattern; m_pattern = pattern; return old; }

	C3x4Matrix*	PatternTransform( void ) const			{ return m_pattern_xform; }
	C3x4Matrix* PatternTransform( C3x4Matrix* xform )	{ C3x4Matrix* old=m_pattern_xform; m_pattern_xform = xform; return old; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Entity database management.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	int EntityCount() const;

	CReturn EntityCreate( EDbEntityType type, CDbEntity** dbEntity );

	CReturn EntityPrepareCopy( CModel* io_dest=NULL );
	CReturn EntityCopy( CDbEntity& in_entity, CDbEntity** dbEntity );

	CReturn EntityDelete( ID id );

	CReturn EntityFind(
				ID id,
				CDbEntity** dbEntity,
				EDbEntityType startType = DBWORKPLANE,
				EDbEntityType endType = DBTERMINAL ) const;

	CReturn EntityFind(
				const CString& name,
				CDbEntity** dbEntity,
				EDbEntityType startType = DBWORKPLANE,
				EDbEntityType endType = DBTERMINAL ) const;

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

	CReturn Undo();
	CReturn Redo();

	int  UndoBufferDepth() const;
	void UndoBufferDepth( int depth );

	// Used by file manager methods immediately after
	// reading a file.  Clears entity dirty flag and
	// sets m_nextId based on the highest id of the
	// entities that were just read.
	void PostReadInit();

	bool is_hidden( const CDbEntity* entity ) const;

	// For debugging.
	void ToolsTrace( const char* heading ) const;

	// TODO: Perhaps introduce a CModelIterator()?
	CEntityDb& Db() const;

	virtual ~CModel();

private:

	// Disabled.
	CModel( const CModel& );
	const CModel& operator = ( const CModel& );
	int operator == ( const CModel& ) const;
	int operator != ( const CModel& ) const;

private:

	CEntityDb		m_db;

	CSelectorStack*	m_selectorStack;
	CTreeViewSupport* m_treeViewSupport;

	CDbTool*		m_tool;
	CDbWorkplane*	m_workplane;

	CDbPattern*		m_pattern;
	C3x4Matrix*		m_pattern_xform;

	CVarList		m_header;
	CVarList		m_varsys;
};

typedef CDynamicArray<CModel*> CModelArray;

#endif

