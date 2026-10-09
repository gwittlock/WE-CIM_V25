
#ifndef _TOOLSHAPEEXTRUDER_H
#define _TOOLSHAPEEXTRUDER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "VarList.h"
#include "GeoCurve.h"
#include "GeoPoly.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: CToolShape was used as the basis for implementation
// of this class.

class dllExport CToolShapeExtruder
{
public:

	CToolShapeExtruder();

	CReturn Extrude(
				const CGeoCurve&	geoCurve,
				const CVarList&		descrip,
				CGeoPoly*			geoPoly );

	virtual ~CToolShapeExtruder();

protected:

private:

	void extrude_2d_round(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly );

	void extrude_2d_square(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly );

	void extrude_2d_rect(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly );

	void extrude_2d_obround(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly );

	void extrude_2d_dd(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly );

	void extrude_2d_trapezoid(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly );

	void extrude_2d_hexagon(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly );

private:

	// Disabled.
	CToolShapeExtruder( const CToolShapeExtruder& );
	const CToolShapeExtruder& operator = ( const CToolShapeExtruder& );
	int operator == ( const CToolShapeExtruder& ) const;
	int operator != ( const CToolShapeExtruder& ) const;

private:
	//
	// type:
	//		1 == line
	//		2 == G02 (CW Arc)
	//		3 == G03 (CC Arc)
	//
	CArray<int,int> m_type;

	CArray<C3dCoord, C3dCoord> m_pt;
};

#endif

