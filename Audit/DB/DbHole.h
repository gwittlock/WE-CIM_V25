
#ifndef _DBHOLE_H
#define _DBHOLE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DynamicArray.h"
#include "DbEntity.h"

class CEntityDb;
class C3dCoord;


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

class dllExport CDbHole : public CDbEntity
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBHOLE; }

	// Obtain the bounding box of this entity in the given coordinate system.
	virtual C3dBox Box( ID workplaneId = ~0 ) const;

	// Establish a hole in the database on the given layer and relative to
	// the given workplane.  The layer and workplane reference counts will
	// be updated by this entity.  The center point models the top of the
	// hole and the depth is a positive incremental value.
	void Init( CDbTool* tool, CDbWorkplane* workplane, const C3dCoord& center, double diam, double depth );

	// Obtain the coordinates in another workplane.
	C3dCoord Center( ID workplaneId = ~0 ) const;

	// Introduced to support canned cycles like LAA/BHC.
	// NOTE: StartPt() is synonymous with Center().
	C3dCoord StartPt( ID workplaneId = ~0 ) const;
	C3dCoord EndPt( ID workplaneId = ~0 ) const;

	double Diam() const				{ return m_diam; }
	void Diam( double diam )		{ m_diam = diam; }

	double Depth() const			{ return fabs(m_depth); }
	void Depth( double depth )		{ m_depth = depth; }

	bool IsPierce() const;

	// Obtain the list of entities that are referenced by this entity.
	// The entities are appended to the end of the given list.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	// Delete this hole.  The hole is really only marked as deleted and its
	// associated elements are updated reflect the deletion of the hole.
	// The hole is truely delete once it falls out of scope (see also CUndo).
	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	virtual bool canDescribe2d( void ) const { return TRUE; }
	virtual void  Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

	virtual void Transform( const C3x4Matrix& in_xform );

	// Obtain the coordinates in another workplane.
	C3dCoord Coord( ID workplaneId = ~0 ) const;

protected:

private:  // Methods

	CDbHole( CEntityDb* db );

	CDbHole( const CDbHole& dbHole );

	const CDbHole& operator = ( const CDbHole& dbHole );

	virtual ~CDbHole();

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

	void DescribeHits( 
		int					regen,
		const CDbTool&		dbTool,
		double				in_tolerance ) const;

	void DescribeLAA(
		int					regen,
		const CDbTool&		dbTool,
		double				in_tolerance ) const;

	void DescribeBHC(
		int					regen,
		const CDbTool&		dbTool,
		double				in_tolerance ) const;

	void DescribeGrid(
		int					regen,
		const CDbTool&		dbTool,
		double				in_tolerance ) const;

	void DescribeRow(
		int					regen,
		const CDbTool&		dbTool,
		const C3dCoord&		ps,
		const C2dVec&		vec,
		int					count,
		double				in_tolerance ) const;

	void DescribeRapid(
		int					regen,
		const CDbTool&		dbTool,
		const C3x4Matrix&	xform,
		const C3dCoord&		ps,
		const C3dCoord&		pe ) const;
	
	void DescribeHandle(
		int					regen,
		const C3dCoord&		pt,
		const char*			label,
		double				in_tolerance ) const;

	void AttributesUpdate( const C3x4Matrix& xform );

	double AngNormalize( double ang );
	int SncsFromAng( double ang );

	bool IsZero( double ang );
	bool IsMultipleOf180( double ang );

private:  // Disabled

	CDbHole();
	int operator == ( const CDbHole& ) const;
	int operator != ( const CDbHole& ) const;

private:  // Data

	C3dCoord m_center;
	double m_diam;
	double m_depth;
};

typedef CDynamicArray<CDbHole*> CDbHoleArray;

#endif

