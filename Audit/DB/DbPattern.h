
#ifndef _DBPATTERN_H
#define _DBPATTERN_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DynamicArray.h"
#include "DbEntity.h"
#include "DbContainer.h"
#include "DbCommand.h"
#include "DbFeature.h"

class CGeoElem;
class CEntityDb;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Transferred here from sheet.h, these are different types for 
// pattern instances.
#define PATTERN_MARK_BURN	"b"
#define PATTERN_MARK_PUNCH	"p"
#define PATTERN_MARK_OTHER	"x"
#define PATTERN_MARK_ALL	"."


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// A feature can be either a 'part feature' or a 'manufacturing feature'.
// In either case, a feature can contain subfeatures.  When a feature is
// deleted, all of its subfeatures will be deleted as well.  If you do
// not want to delete a subfeature, it must first be removed from the
// feature before deleting the feature.
//
// BEWARE: Current policy dictates that features contain only part or
// toolpath entities, but not both.  There is no enforcement of this
// policy, therefore the client must be responsible.
//
class dllExport CDbPattern : public CDbContainer
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBPATTERN; }

	virtual CDbWorkplane* Workplane() const		{ return NULL; }
	virtual CDbTool* Tool() const				{ return NULL; }

	virtual void Workplane( CDbWorkplane* dbWork )	{ dbWork;  /* compiler fodder, do nothing */ }
	virtual void Tool( CDbTool* dbTool )			{ dbTool;  /* compiler fodder, do nothing */ }

	virtual CDbWorkplane* ContainedWorkplane() const	{ return NULL; }
	virtual CDbTool* ContainedTool() const				{ return NULL; }

	// Return the count of immediate children.
	virtual int Count() const;

	// Get the ith immediate child entity.
	virtual CDbEntity* GetAt( int indx ) const;
	virtual CDbEntity* operator[]( int indx ) const;

	virtual CDbEntity* ReplaceAt( int indx, CDbEntity* dbEntity );

	// Make the given entity a child of this feature.
	virtual CReturn Prepend( CDbEntity* dbEntity, bool copy = FALSE );

	// Make the given entity a child of this feature.
	virtual CReturn Append( CDbEntity* dbEntity, bool copy = FALSE );

	// Insert an entity in front of a known entity in this container.
	virtual int InsertBefore( CDbEntity* refEntity, CDbEntity* newEntity );
	virtual int InsertBefore( int indx, CDbEntity* newEntity );

	// Insert an entity behind a known entity in this container.
	virtual int InsertAfter( CDbEntity* refEntity, CDbEntity* newEntity );
	virtual int InsertAfter( int indx, CDbEntity* newEntity );

	// Find the position of the given curve in the profile (0 indexed).
	virtual int Position( const CDbEntity* refEntity ) const;

	// CDbContainer requirement -- releases ownership of the given entity.
	// Returns true if successful.
	virtual bool Disown( CDbEntity* dbEntity );

	virtual void BenignFlush();
	virtual CReturn DestructiveFlush();

	// Obtain a list of entities that this feature references.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this feature has a reference to the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Obtain a list of entities owned by this entity.
	virtual void Subordinates( CDbEntityList* list ) const;

	// Get the atomic entities.
	virtual void Flatten( CDbEntityList* entities ) const;

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	// Obtain the bounding box containing all feature entities.
	virtual C3dBox Box( ID workplaneId = ~0 ) const;

	virtual void	Transform( const C3x4Matrix& in_xform );

	// Convenience methods.

	bool	IsMain() const;
	bool	IsSheet() const;
	bool	IsA( CString cmd ) const;
	
	void Describe2d(
		int				regen,
		C3dCoord*		io_tooltip,
		double			in_tolerance,
		CDisplayEntity*	dispent ) const;

protected:

	CDbPattern( const CDbPattern& dbPattern );

	CDbPattern( CEntityDb* db );

	virtual ~CDbPattern();

	const CDbPattern& operator = ( const CDbPattern& dbPattern );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by CEntityDb.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual void RemoveRef( CDbEntity* dbEntity );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by indirectly by CUndo.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual CReturn Clone( CDbEntity** dbEntity ) const;

	virtual CReturn ContentsSwap( CDbEntity* dbEntity );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual CReturn CopyTo( CEntityDb* io_dest, tDbEntityMap* io_refmap, CDbEntity** dbEntity );

	virtual bool canDescribe2d( void ) const { return FALSE; }

private:  // Disabled

	CDbPattern();
	int operator == ( const CDbPattern& ) const;
	int operator != ( const CDbPattern& ) const;

private:  // Data

	// The entities owned by this feature.
	CDbEntityList m_list;
};

typedef CDynamicArray<CDbPattern*> CDbPatternArray;

#endif

