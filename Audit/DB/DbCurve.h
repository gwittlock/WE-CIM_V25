
#ifndef _DBCURVE_H
#define _DBCURVE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"

class CGeoCurve;
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

class dllExport CDbCurve : public CDbEntity
{
	friend class CEntityDb;
	friend class CDbLine;
	friend class CDbArc;

public:

	// Get the transformed geometric representation of
	// this curve in the specified coordinate system.
	virtual CGeoCurve* Curve( ID workplaneId = ~0 ) const = 0;

	virtual C3dCoord StartPt( ID workplaneId = ~0 ) const = 0;
	virtual C3dCoord EndPt( ID workplaneId = ~0 ) const = 0;

	// Fetch the start point of this curve.
	virtual CDbPoint* DbStartPt() const = 0;

	// Replace the start point of this curve.
	virtual void DbStartPt( CDbPoint* pt ) = 0;

	// Fetch the end point of this curve.
	virtual CDbPoint* DbEndPt() const = 0;

	// Replace the end point of this curve.
	virtual void DbEndPt( CDbPoint* pt ) = 0;

	virtual void Reverse() = 0;

	// Split the curve at the point on the curve nearest
	// the given point.  If successful, returns the newly
	// created 'trailing' curve.  Otherwise, returns NULL.
	virtual CDbCurve* Split( const C3dCoord& pt, double gap = 0.0 );

	// Associate the end point of this curve
	// with the start point of the given curve.
	void Associate( CDbCurve* dbCurve, double tol );

	// Disassociate the end point of this curve
	// from the start point of the given curve.
	CDbPoint* Disassociate( CDbCurve* dbCurve );

	CDbPoint* ConditionalDisassociate( CDbPoint* pt );

	virtual ~CDbCurve();  // TODO: Hide?

protected:

	CDbCurve( const CDbCurve& dbCurve );

	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

private:  // Methods

	CDbCurve( CEntityDb* db );

	void GapPointsCalc(
					const C3dCoord&	closestPt,
					double			gap,
					C3dCoord*		splitA,
					C3dCoord*		splitB );

private:  // Disabled

	CDbCurve();
	const CDbCurve& operator = ( const CDbCurve& );
	int operator == ( const CDbCurve& ) const;
	int operator != ( const CDbCurve& ) const;

private:  // Data

};


typedef CDynamicArray<CDbCurve*> CDbCurveArray;

#endif

