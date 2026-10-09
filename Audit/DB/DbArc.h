
#ifndef _DBARC_H
#define _DBARC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "GeoArc.h"
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

class dllExport CDbArc : public CDbCurve
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBARC; }

	// Obtain the bounding box of this entity in the given coordinate system.
	virtual C3dBox Box( ID workplaneId = ~0 ) const;
	C3dBox Box( const C3x4Matrix& target ) const;

	// Establish an arc on the given tool and relative to the given workplane.
	// The tool and workplane reference counts will be updated by this entity.
	CReturn Init( CDbTool* tool, CDbWorkplane* workplane, const CGeoArc& arc );

	// Establish an arc on the given tool and relative to the given workplane.
	// The tool and workplane reference counts will be updated by this entity.
	CReturn Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& start, const C3dCoord& end, const C3dCoord& center, int dir );

	// Establish an arc on the given tool and relative to the given workplane.
	// The tool and workplane reference counts will be updated by this entity.
	CReturn Init( CDbTool* tool, CDbWorkplane* workplane, CDbPoint* start, CDbPoint* end, CDbPoint* center, int dir );

	// Used during File/Read, the entity is built using entity id's.
	CReturn Init( ID start, ID end, ID center, int dir );

	// Obtain the list of entities that are referenced by this entity.
	// The entities are appended to the end of the given list.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	// Get the polymorhic geometric representation of this arc.
	// Client is responsible for deleting the pointer.
	virtual CGeoCurve* Curve( ID workplaneId = ~0 ) const;

	// Get the geometric representation of this arc.
	// Client is responsible for deleting the pointer.
	CGeoArc* Arc( ID workplaneId = ~0 ) const;

	virtual C3dCoord StartPt( ID workplaneId = ~0 ) const;
	virtual C3dCoord EndPt( ID workplaneId = ~0 ) const;
	C3dCoord CenterPt(ID workplaneId = ~0 ) const;

	virtual void Reverse();

	// --- defined in class CDbCurve ---
	// virtual CReturn Split( const C3dCoord& pt );

	// Fetch the start point of this curve.
	virtual CDbPoint* DbStartPt() const;

	// Replace the start point of this curve.
	virtual void DbStartPt( CDbPoint* pt );

	// Fetch the end point of this curve.
	virtual CDbPoint* DbEndPt() const;

	// Replace the end point of this curve.
	virtual void DbEndPt( CDbPoint* pt );

	// Fetch the center point of this curve.
	CDbPoint* DbCenterPt() const;

	int Dir() const;
	void Dir( int in_dir );
	double Radius() const;

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	virtual bool canDescribe2d( void ) const { return TRUE; }
	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

	virtual void Transform( const C3x4Matrix& in_xform );

protected:

private:  // Methods

	CDbArc( CEntityDb* db );

	CDbArc( const CDbArc& dbArc );

	const CDbArc& operator = ( const CDbArc& dbArc );

	virtual ~CDbArc();

	CReturn Validate( const C3dCoord& ps, const C3dCoord& pe, const C3dCoord& pc );

	bool IsAssociated();

	CReturn Associate();

	void Update();

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

	CDbArc();
	int operator == ( const CDbArc& ) const;
	int operator != ( const CDbArc& ) const;

private:  // Data

	// Start, end and center points.
	CDbPoint* m_ps;
	CDbPoint* m_pe;
	CDbPoint* m_pc;

	// Arc direction (-1) cw / (+1) ccw
	// The direction is stored as an integer
	// in order to simplify offset calculations.
	int m_dir;
};

#endif
