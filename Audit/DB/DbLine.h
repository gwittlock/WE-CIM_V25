
#ifndef _DBLINE_H
#define _DBLINE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GeoLine.h"
#include "DbCurve.h"

class CEntityDb;
class C3dCoord;
class CDbPoint;



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

class dllExport CDbLine : public CDbCurve
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBLINE; }

	// Obtain the bounding box of this entity in the given coordinate system.
	virtual C3dBox Box( ID workplaneId = ~0 ) const;

	// A variant of Init( const C3dCoord& startPt, const C3dCoord& endPt ),
	// the new database points are created from the endpoints of the line.
	// The end points of the line must be defined relative to the given
	// workplane.  The tool and workplane reference counts will be updated
	// by this entity.
	CReturn Init( CDbTool* tool, CDbWorkplane* workplane, const CGeoLine& line );

	// Establish two new points in the database and build a line based on
	// those points.  The end points must be defined relative to the given
	// workplane.  The tool and workplane reference counts will be updated
	// by this entity.
	CReturn Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& ps, const C3dCoord& pe );
	CReturn Init( const C3dCoord& ps, const C3dCoord& pe );

	// Build a line based on two existing database points, establishing an
	// associative relationship between the line and the points.  If either
	// point is moved, the tool will be altered.  The line and workplane
	// reference counts will be updated by this entity.
	CReturn Init( CDbTool* tool, CDbWorkplane* workplane, CDbPoint* ps, CDbPoint* pe );

	// Used during File/Read, the entity is built using entity id's.
	CReturn Init( ID ps, ID pe );

	// Get the display representation of this entity.
	virtual bool canDescribe2d( void ) const { return TRUE; }
	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

	virtual void Transform( const C3x4Matrix& in_xform );

	// Obtain the list of entities that are referenced by this entity.
	// The entities are appended to the end of the given list.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	// Get the polymorphic geometric representation of this line.
	virtual CGeoCurve* Curve( ID workplaneId = ~0 ) const;

	// Get the geometric representation of this line in the local coordinate system.
	CGeoLine* Line( ID workplaneId = ~0 ) const;

	virtual C3dCoord StartPt( ID workplaneId = ~0 ) const;
	virtual C3dCoord EndPt( ID workplaneId = ~0 ) const;

	// Fetch the start point of this curve.
	virtual CDbPoint* DbStartPt() const;

	// Replace the start point of this curve.
	virtual void DbStartPt( CDbPoint* pt );

	// Fetch the end point of this curve.
	virtual CDbPoint* DbEndPt() const;

	// Replace the end point of this curve.
	virtual void DbEndPt( CDbPoint* pt );

	virtual void Reverse();

	// Delete this line.  The line is really only marked as deleted and its
	// associated elents are updated reflect the deletion of the line.
	// The line is truely delete once it falls out of scope (see also CUndo).
	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

protected:

private:  // Methods

	CDbLine( CEntityDb* db );

	CDbLine( const CDbLine& dbLine );

	const CDbLine& operator = ( const CDbLine& dbLine );

	CReturn Validate( const C3dCoord& ps, const C3dCoord& pe ) const;

	bool IsAssociated();

	CReturn Associate();

	void Update();

	virtual ~CDbLine();

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

private:  // Disabled

	CDbLine();
	int operator == ( const CDbLine& ) const;
	int operator != ( const CDbLine& ) const;

private:

	CDbPoint* m_ps;
	CDbPoint* m_pe;
};

#endif

