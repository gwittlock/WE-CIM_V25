
#ifndef _NESTEDAREA_H
#define _NESTEDAREA_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DynamicArray.h"
#include "2dCoord.h"
#include "NestingPart.h"
#include "ToolHit.h"
#include "PartPlace.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CNestedArea;
typedef CDynamicArray<CNestedArea*> TNestedAreaArray;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CNestedArea
{
public:

	CNestedArea( const CPartPlace& part_place );

	CNestingPart* PartGet() const;

	CToolHit* ToolHitGet() const;

	C2dCoord HandlePointGet() const;

	int InteriorAreasCount() const;

	CNestedArea* InteriorAreaGet( int indx ) const;

	void InteriorAreaAdd( CNestedArea* nest_area );

	CGeoPoly* HolePolyGet() const;

	CGeoPolyArray* PolysGet(int depth) const;

	double Pack(
		bool				do_packing,
		const CPartPlace&	part_place,
		eNestProgression	progression,
		CViewMgr&			view );

	virtual ~CNestedArea();

private:

	CGeoPoly* PartShadow(
		const CPartPlace&	part_place,
		eNestProgression	progression );

	void PolyRender( const CGeoPoly& poly );

private:

	// Disabled.
	CNestedArea();
	CNestedArea( const CNestedArea& );
	const CNestedArea& operator = ( const CNestedArea& );
	int operator == ( const CNestedArea& ) const;
	int operator != ( const CNestedArea& ) const;

private:

	CToolHit*			m_toolhit;
	C2dCoord			m_handle;
	TNestedAreaArray	m_interior_areas;

	CGeoPoly*			m_packing;
};

#endif

