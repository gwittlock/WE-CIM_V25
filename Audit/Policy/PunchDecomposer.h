
#ifndef _PUNCHDECOMPOSER_H
#define _PUNCHDECOMPOSER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _GEOLINE_H
#include "GeoLine.h"
#endif

#ifndef _GEOARC_H
#include "GeoArc.h"
#endif

#ifndef _SHAPE_H
#include "Shape.h"
#endif

#ifndef _SHAPEDECOMPOSER_H
#include "ShapeDecomposer.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CPunchDecomposer : public CShapeDecomposer
{
public:

	CPunchDecomposer();

	virtual CReturn Decompose(
							const CShape&	theShape,
							int				iteration,
							TShapeArray*	shapes ) const;

	virtual ~CPunchDecomposer();

protected:

private:  // Methods

	CReturn ObroundDecompose(
						const CShape&	theShape,
						int				iteration,
						TShapeArray*	shapes ) const;

	CReturn KeyholeDecompose(
						const CShape&	theShape,
						int				iteration,
						TShapeArray*	shapes ) const;

	void Rectangle(
					const CGeoLine*	lineA,
					const CGeoLine*	lineB,
					CGeoCurveArray*	curves ) const;

	void Obround(
					const CGeoLine*	lineA,
					const CGeoLine*	lineB,
					const CGeoArc*	arcC,
					CGeoCurveArray*	curves ) const;

private:  // Disabled

	CPunchDecomposer( const CPunchDecomposer& );
	const CPunchDecomposer& operator = ( const CPunchDecomposer& );
	int operator == ( const CPunchDecomposer& ) const;
	int operator != ( const CPunchDecomposer& ) const;

private:  // Data

};

#endif

