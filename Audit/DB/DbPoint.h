
#ifndef _DBPOINT_H
#define _DBPOINT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

#ifndef _3DCOORD_H
#include "3dCoord.h"
#endif

class CEntityDb;
class CGeoPoint;



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

class dllExport CDbPoint : public CDbEntity
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBPOINT; }

	// Obtain the bounding box of this entity in the given coordinate system.
	virtual C3dBox Box( ID workplaneId = ~0 ) const;

	// Establish a point on the given Tool and relative to the given workplane.
	// The Tool and workplane reference counts will be updated by this entity.
	//
	void Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& pt );

	// Establish a point on the given Tool and relative to the given workplane.
	// The Tool and workplane reference counts will be updated by this entity.
	//
	void Init( CDbTool* tool, CDbWorkplane* workplane, double x, double y, double z );

	void Init( const C3dCoord& pt );
	void Init( double x, double y, double z );

	CGeoPoint Point() const;

	// Obtain the coordinates in another workplane.
	C3dCoord Coord( ID workplaneId = ~0 ) const;

	// Get the display representation of this entity.
	virtual bool canDescribe2d( void ) const { return TRUE; }
	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

	virtual void	Transform( const C3x4Matrix& in_xform );

	// Obtain the list of entities that are referenced by this entity.
	// The entities are appended to the end of the given list.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	// Delete this point.  The point is really only marked as deleted and its
	// associated elements are updated reflect the deletion of the point.
	// The point is truely delete once it falls out of scope (see also CUndo).
	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual CReturn CopyTo( CEntityDb* io_dest, tDbEntityMap* io_refmap, CDbEntity** dbEntity );

protected:

private:  // Methods

	CDbPoint( CEntityDb* db );

	CDbPoint( const CDbPoint& dbPoint );

	const CDbPoint& operator = ( const CDbPoint& dbPoint );

	void Update( const C3dCoord& pt );

	virtual ~CDbPoint();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by CEntityDb.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual void RemoveRef( CDbEntity* dbEntity );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by indirectly by CUndo.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual CReturn Clone( CDbEntity** dbEntity ) const;

	virtual CReturn ContentsSwap( CDbEntity* dbEntity );

private:  // Disabled

	CDbPoint();
	int operator == ( const CDbPoint& ) const;
	int operator != ( const CDbPoint& ) const;

private:  // Data

	C3dCoord m_pt;
};

#endif

