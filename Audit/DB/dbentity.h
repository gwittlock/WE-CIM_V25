
#ifndef _DBENTITY_H
#define _DBENTITY_H

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// An abstract base class representing an entity in the database.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE:
//
//   Regarding workplane IDs, anywhere a method requires a workplane ID,
//   the default argument of ~0 will cause the method to return a result
//   in the local coordinate system of the entity.  An argument of 0 will
//   cause the method to return a result in the global coordinate system.
//   Any other argument will cause the method to return a result in the
//   specified local coordinate system.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Type.h"

#include "Return.h"
#include "2dBox.h"
#include "3dBox.h"
#include "DbConsts.h"
#include "VarList.h"
#include "3x4Matrix.h"
#include "ToolShape.h"

class CDbEntity;
#include "DisplayEntity.h"

//class C3dBox;
class CEntityDb;
class CDbTool;
class CDbWorkplane;
class CDbTool;
class CDbSequence;
class CUndo;
class CMM2;
class CDbEntityVisitor;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CDbEntity;
typedef CMap<CDbEntity*, CDbEntity*, CDbEntity*, CDbEntity*>	tDbEntityMap;

typedef CDynamicArray<CDbEntity*>	CDbEntityArray;
typedef CIndxList<CDbEntity*>		CDbEntityList;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDbEntity
{
	// Though friends are kinda hideous, they do offer
	// collaboration without exposing those methods
	// that should not be used by the general public.

	friend class CEntityDb;
	friend class CDbProfile;
	friend class CDbFeature;
	friend class CDbPattern;
	friend class CDbSequence;
	friend class CUndo;
	friend class CModelUtil;

public:

	CDbEntity( CEntityDb* db );
	virtual ~CDbEntity( void );

	// Poor man's RTTI.
	virtual EDbEntityType Type() const = 0;

	// Get/Set this entity's id.
	ID Id() const		{ return m_id; }
	void Id( ID id )	{ m_id = id; }

	// Get this entity's name.
	// If a user-defined name does not exist for this entity,
	// its derived name will be returned in the form _TypeId
	//
	// However, SystemName NEVER returns a derived name; either the
	// actual name or an empty string.
	CString Name() const;
	CString SystemName() const;

	// Set this entity's user-defined name.
	// Returns TRUE if successful.
	bool Name( const CString& name );
	bool SystemName( const CString& name );

	// Get the display representation of this entity.
	virtual bool canDescribe2d( void ) const { return FALSE; }
	virtual void Describe2d( int regen, C3dCoord* endpt, double in_tolerance ) const;

	void DescribeTarget( int regen, const CString& display, C3dCoord* endpt, double in_tolerance ) const;
	void DescribeDropStop( int regen, int dropstop, C3dCoord* endpt, double in_tolerance ) const;

	// Transform the entity, including it's associative points, by the transform
	// USES:  TAG flag to avoid double-transforms of shared entities; pain, agony
	virtual void	Transform( const C3x4Matrix& in_xform );

	// Obtain the bounding box of this entity in the given coordinate system.
	// The default method returns an undefined bounding box (for use by entities that do
	// not really have a bounding box).  See also C2dBox::IsDefined().
	virtual C3dBox Box( ID workplaneId = ~0 ) const;
	void BoxInvalidate()  { 	m_box_workid = -2; }


	virtual void Delete() = 0;

	virtual void Accept( CDbEntityVisitor* visitor ) = 0;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Attribute management methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool IsHidden() const;
	void Hide();
	void Seek();

	// Convenience method.
	// Get the id of the workplane that is referenced by this entity.
	// Beware: A 0 is returned if this entity does not reference a workplane.
	ID WorkplaneId() const;

	// Convenience method.
	// Get the id of the workplane that is referenced by this entity.
	// Beware: A 0 is returned if this entity does not reference a workplane.
	ID ToolId() const;

	// Get the workplane that is referenced by this entity.
	// BEWARE:
	// 1. NULL is returned if this entity does not reference a workplane.
	// 2. Some entity types over-ride the default Workplane() behavior.
	virtual CDbWorkplane* Workplane() const		{ return m_workplane; }

	// Get the tool that is referenced by this entity.
	// BEWARE:
	// 1. NULL is returned if this entity does not reference a tool.
	// 2. Some entity types over-ride the default Tool() behavior.
	virtual CDbTool* Tool() const		{ return m_tool; }

	bool IsToolpath() const;

	void TransientOrientationSet() const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Reference management methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool IsRefd() const;
	int  RefCnt() const;
	void RefInc();
	void RefDec();

	// Get the list of entities that reference this entity.
	void RefdBy( CDbEntityList* list ) const;

	// Get the list of entities that this entity references.
	virtual void RefsTo( CDbEntityList* list ) const = 0;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Obtain/set the pointer to the entity that owns this entity
	// An entity can be owned by only one 'container entity',
	// such as a feature or a profile.
	void Owner( CDbEntity* owner );
	CDbEntity* Owner() const			{ return m_owner; }

	void Sequence( CDbSequence* seq );
	CDbSequence* Sequence() const		{ return m_seq; }

	CDbFeature* WorkZone() const;

	bool IsChildOfLead() const;

	// Get the list of entities subordinate to this entity.
	// NOTE: 'Subordinate' implies neither ownership nor
	// reference.
	virtual void Subordinates( CDbEntityList* list ) const = 0;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Attribute management, through the varlist
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	int AttribCount() const;

	int IntGet( const CString& name, int defval ) const;
	double DoubleGet( const CString& name, double defval  ) const;
	CString StringGet( const CString& name, const CString& defval ) const;

	void IntSet( const CString& name, int ival );
	void DoubleSet( const CString& name, double dval );
	void StringSet( const CString& name, const CString& sval );

	void AttribsDelete();
	void AttribDelete( const CString& name );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Low-level attribute management (caution).
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool HasAttribs() const						{ return (m_attribs.countVar() > 0); }
	const CVarList&	Attrib( void ) const		{ return m_attribs; }
	CVarList* pAttrib( void )					{ return &m_attribs; }

	// Set multiple attributes
	CReturn setMulti( const CString& multi_str );

	void ColorSet( int color );
	virtual int ColorGet( int defaultColor ) const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// User-commands (a subset of attributes)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Gets the count of user commands attached to this entity.
	// Such commands appear as the [0..n-1] attributes.
	int UserCommandCount() const;


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	bool IsSelected() const;
	void SelectFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Some objects like the stock and
	// clamps are not normally selectable
	//
	bool IsSelectable() const;
	void SelectableFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Some objects like the end points
	// of lines are not normally displayed
	// or directly manipulated by the user.
	//
	bool IsSystem() const;
	void SystemFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	bool WasCreated() const;
	void CreateFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	bool IsDeleted() const;
	void DeleteFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	bool IsDirty() const;
	void ModifyFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Applicable to holes only.
	bool IsFilled() const;
	void FilledFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Applicable to tools (layers) only.
	bool IsSnappable() const;
	void SnappableFlag( bool set );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Applicable to tools (layers) only.
	bool IsHotDot() const;
	void HotDotFlag( bool set );

	// Call NewAction() instead of TagClear()
	// Call DoAction() instead of TagSet()
	// Call DidAction instead of IsTagged()
	//
	static int NewAction();
	void DoAction();
	bool DidAction() const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	FLAGS Flags() const;
	void Flags( FLAGS flags );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy the entity, but leaves off workplane and layer, and doesn't
	//	fuck up the ID, leaves a blank name, and adds it to the refmap.
	// Base function, used by all other copy functions; not all that good
	// by itself.  Not quite like CommonCopy().  This Copy() stuff is
	//	getting painful due to the reference counts, etc, to track... eww
	virtual CReturn CopyTo( CEntityDb*		io_dest,
							tDbEntityMap*	io_refmap, 
							CDbEntity**		dbEntity );

	void	CommonSubsetCopy( const CDbEntity& dbEntity );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Originally, these methods were used by
	// derived classes and CMM2.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Set the reference workplane of this entity.
	// The workplane reference count is updated.
	// BEWARE:
	// 1. Some entity types over-ride the default Workplane() behavior.
	virtual void Workplane( CDbWorkplane* workplane );

	// Set the reference Tool of this entity.
	// The Tool reference count is updated.
	// BEWARE:
	// 1. Some entity types over-ride the default Workplane() behavior.
	virtual void Tool( CDbTool* Tool );

	// low-level (1st use is CModel::is_hidden())
	bool is_hidden() const;

	// The database containing this entity.
	// This was protected for a reason, but I'll be damned if I know what it was
	CEntityDb* Db()	const	{ return m_db; }

	CDisplayEntity*	DisplayEntityGet( bool* did_allocate ) const;

public:

	// Check the lexical correctness of a name.
	static CString NameConvert( const CString& name );
	static bool NameCheck( const CString& name );
	static bool SystemNameCheck( const CString& name );

	static void FlipCutside( bool flip_cutside );
	static bool FlipCutside();

	static void draw_2d( 
		const CToolShape&	shape,
		tCmdArray*			io_cmd, 
		tPnt3Array*			io_pnt, 
		const C3x4Matrix&	ref2world,		// Local plane of reference
		double				tolerance,		// Tolerance to explode arcs to
		const C3dCoord&		ct,				// Punch origin
		double				radians,
		bool				fill );

	static void CanDrawTarget( bool active );
	static bool CanDrawTarget();

	static void CanDrawLegend( bool active );
	static bool CanDrawLegend();

	static void CanDrawHandles( bool active );
	static bool CanDrawHandles();

	// Ugh. Color override introduced for Pattern/Bump while dragging parts.
	static void OverrideColorSet( DWORD color );
	static void OverrideColorClear();
	static bool UseOverrideColor();

protected:  // Methods

	CDbEntity();

	CDbEntity( const CDbEntity& dbEntity );

	const CDbEntity& operator = ( const CDbEntity& dbEntity );

	// Obtain a pointer to the entity having the given id.
	// The search can be restricted to the range of entity
	// types startType..endType (inclusive).

	CReturn Find(
				ID id,
				CDbEntity** dbEntity,
				EDbEntityType startType = DBWORKPLANE,
				EDbEntityType endType = DBTERMINAL ) const;

	// Create an entity of the given type, in the database.
	CReturn Create( EDbEntityType type, CDbEntity** dbEntity );

	// Used by an entity to delete the entities that it owns.
	// The given entity is not really deleted but is marked
	// as deleted.  Actual deletion is managed by the undo
	// buffer as an entity falls out of scope.  The pointer
	// to the given entity is set to NULL to prevent reuse.
	void Delete( CDbEntity** dbEntity );

	void Remove( const CDbEntity& dbEntity );

	// Traverse the database, removing all references to this entity,
	// as might be required when and entity is Delete()'d, for instance.
	// This method eventually leads to the calling of RemoveRef().
	// This pair of calls is necessary to prevent dangling references
	// to deleted entities that are low in the hierarchy.
	void RemoveRefs();

	// Copy the common parameters of the given entity to this entity.
	void CommonCopy( const CDbEntity& dbEntity );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by containers.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Sever this entity from its owner.
	void Orphan();
	void OrphanSeq();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by CEntityDb.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Remove any references that this entity has to the given entity,
	// as might be required when and entity is Delete()'d, for instance.
	virtual void RemoveRef( CDbEntity* dbEntity ) = 0;


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by indirectly by CUndo.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Records the current state of this entity in the undo buffer
	// that is associated with the entity database.  Note: the
	// undo buffer must first be opened.  Also, the undo buffer
	// only records the first of this entity after the it is
	// opened.  Any number of changes may be made to the entity
	// after that point.  Upon executing an undo command, the
	// entity will be restored to its original state, that is,
	// the state it was in when it was first recorded.
	void Record();

	virtual CReturn Clone( CDbEntity** dbEntity ) const = 0;

	virtual CReturn ContentsSwap( CDbEntity* dbEntity ) = 0;

	bool IsReferencing() const;
	void ReferenceFlagSet();
	void ReferenceFlagClear();

	// 2004.12.22 (PE) -- Introduced to circumvent issues with displaying
	// clamps.  Note, we do not want to use CDbEntity::Workplane() in this
	// case because said default implementation affects the database and
	// undo-system states.
	void TempWorkplane( CDbWorkplane* workplane );

	// Cache of entity extent... to avoid a zillion transforms and explodes
	ID			m_box_workid;
	C3dBox		m_box_cache;

private:  // Methods

	CString CDbEntity::DerivedName() const;

private:  // Disabled

	int operator == ( const CDbEntity& ) const;
	int operator != ( const CDbEntity& ) const;

private:

	static int m_new_action;

	static bool m_flip_cutside;

	static bool m_target_draw;
	static bool m_legend_draw;
	static bool m_handles_draw;

	static DWORD m_override_color;
	static bool m_use_override_color;

private:

	// This entity's unique id.
	ID m_id;

	// This entity's user-defined (or system reserved) name.
	CString m_name;

	// Various flags indicating display and
	// database state of this entity.
	FLAGS m_flags;
	int	m_action;

	// The workplane of this entity.
	CDbWorkplane* m_workplane;

	// The tool this entity references.
	CDbTool* m_tool;

	// A count of the number of entities
	// that reference this entity.
	int m_refcnt;

	// This entitys owner (a container).
	CDbEntity* m_owner;

	CDbSequence* m_seq;

	// Attributes...
	CVarList m_attribs;
	
	// The database containing this entity.
	CEntityDb* m_db;

	CDisplayEntity*	m_disp;
};

#endif

