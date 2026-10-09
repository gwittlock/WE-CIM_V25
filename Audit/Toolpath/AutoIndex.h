
#ifndef _AUTOINDEX_H
#define _AUTOINDEX_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _2DCOORD_H
#include "2dCoord.h"
#endif

#ifndef _2DUNITVEC_H
#include "2dUnitVec.h"
#endif

#ifndef _3X4MATRIX_H
#include "3x4Matrix.h"
#endif

#ifndef _GEOELEM_H
#include "GeoElem.h"
#endif

#ifndef _GEOCURVE_H
#include "GeoCurve.h"
#endif

#ifndef _PROFILE_H
#include "Profile.h"
#endif

#ifndef _SHAPE_H
#include "Shape.h"
#endif

#ifndef _WORM_H
#include "Worm.h"  // for enum EEdge
#endif

class CAutoIndexRule;
class CGeoLine;
class CGeoArc;



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CAutoIndex
{
public:

	CAutoIndex();

	CReturn Offset(	
				const CShape&	part,
				const CShape&	tool,
				int				offsetDir,
				bool			use_long_side,
				CGeoElemList*	result );

	virtual ~CAutoIndex();

protected:

private:  // Methods

	CGeoElem* MoveCreate(
					const C3dCoord&		ps,
					const C3dCoord&		pe,
					const CGeoCurve*	pCurve,
					int					stationID,
					int					offsetDir,
					double				toolRotation );

	C3dCoord PositionCalculate(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					int						offsetDir,
					const CShape&	part,
					const CShape&	tool );

	C3dCoord ToolPositionGet(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					int						offsetDir,
					const CShape&	part,
					const CShape&	tool );

	double ToolY(
				const CShape&	part,
				const CShape&	tool,
				int						offsetDir );

	double ToolY(
				const CGeoCurve&		partCurve,
				const CShape&	tool,
				int						offsetDir );

	double ToolY(
				const CGeoLine&		partLine,
				const CGeoLine&		toolLine,
				int					offsetDir );

	double ToolY(
				const CGeoLine&		partLine,
				const CGeoArc&		toolArc,
				int					offsetDir );

	double ToolY(
				const CGeoArc&		partArc,
				const CGeoLine&		toolLine,
				int					offsetDir );

	double ToolY(
				const CGeoArc&		partArc,
				const CGeoArc&		toolArc,
				int					offsetDir );

	double ToolY(
				const C2dCoord&		pt,
				const CGeoArc&		arc,
				bool				ptMinusInt );

	bool Overlap( const C3dBox& pbox, const C3dBox& tbox );

	bool InRange( double min, double max, double val );

	void PartXformInit(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					C3x4Matrix*				xform );

	void ToolXformInit(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					C3x4Matrix*				xform );

private:  // Disabled

	CAutoIndex( const CAutoIndex& );
	const CAutoIndex& operator = ( const CAutoIndex& );
	int operator == ( const CAutoIndex& ) const;
	int operator != ( const CAutoIndex& ) const;

private:  // Data

};

#endif

