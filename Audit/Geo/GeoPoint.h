
#ifndef _GEOPOINT_H
#define _GEOPOINT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _3DCOORD_H
#include "3dCoord.h"
#endif

#ifndef _GEOELEM_H
#include "GeoElem.h"
#endif

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif


class C2dBox;


class dllExport CGeoPoint : public CGeoElem
{
public:

	CGeoPoint();

	CGeoPoint( const C3dCoord& pt );

	CGeoPoint( const CGeoPoint& pt );

	CGeoPoint( double x, double y, double z );

	virtual ~CGeoPoint();
	
	const CGeoPoint& operator = ( const C3dCoord& pt );
	
	const CGeoPoint& operator = ( const CGeoPoint& pt );

	virtual ElemType Type() const;

	virtual CGeoElem* Clone( bool attribs_copy ) const;

	virtual const C3dCoord& StartPt() const;

	virtual void StartPt( const C3dCoord& pt );
	virtual void StartPt( double xs, double ys, double zs );


	virtual const C3dCoord& EndPt() const;

	virtual void EndPt( const C3dCoord& pt );
	virtual void EndPt( double xe, double ye, double ze );

	virtual double Length2d() const;

	virtual void Xform( const C3x4Matrix& xform );
	virtual void Shift( const C3dVec& delta );

private:  // Disabled.

	int operator == ( const CGeoPoint& ) const;
	int operator != ( const CGeoPoint& ) const;

private:  // Methods

	// Update the bounding box of the line.
	void BoxUpdate();

private:  // Data

	C3dCoord m_pt;
};

typedef CDynamicArray<CGeoPoint*> tGeoPointArray;

#endif

