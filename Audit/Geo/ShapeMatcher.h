
#ifndef _SHAPEMATCHER_H
#define _SHAPEMATCHER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _SHAPE_H
#include "Shape.h"
#endif

class CToolSetup;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Abstract base class.
//
class dllExport CShapeMatcher
{
public:

	CShapeMatcher();

	virtual int ExactMatch( const CToolSetup& toolSetup, TShapeArray& exemplars ) const
				{ return 0; }

	virtual int AdequateMatch( const CToolSetup& toolSetup, TShapeArray& exemplars ) const
				{ return 0; }

	virtual CShape* ExactMatch( const TSortedShapes& catalog, const CShape& exemplar ) const
				{ return NULL; }

	virtual CShape* AdequateMatch( const TSortedShapes& catalog, const CShape& exemplar ) const
				{ return NULL; }

	virtual ~CShapeMatcher();

protected:

private:  // Methods

private:  // Disabled

	CShapeMatcher( const CShapeMatcher& );
	const CShapeMatcher& operator = ( const CShapeMatcher& );
	int operator == ( const CShapeMatcher& ) const;
	int operator != ( const CShapeMatcher& ) const;

private:  // Data

};

#endif

