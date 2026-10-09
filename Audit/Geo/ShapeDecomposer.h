
#ifndef _SHAPEDECOMPOSER_H
#define _SHAPEDECOMPOSER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _SHAPE_H
#include "Shape.h"
#endif



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Abstract base class.
//
class dllExport CShapeDecomposer
{
public:

	CShapeDecomposer();

	virtual CReturn Decompose(
							const CShape&	theShape,
							int				iteration,
							TShapeArray*	shapes ) const = 0;

	virtual ~CShapeDecomposer();

protected:

private:  // Methods

private:  // Disabled

	CShapeDecomposer( const CShapeDecomposer& );
	const CShapeDecomposer& operator = ( const CShapeDecomposer& );
	int operator == ( const CShapeDecomposer& ) const;
	int operator != ( const CShapeDecomposer& ) const;

private:  // Data

};

#endif

