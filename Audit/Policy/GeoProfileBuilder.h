
#ifndef _GEOPROFILEBUILDER_H
#define _GEOPROFILEBUILDER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "GeoCurve.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CGeoProfileBuilder
{
public:

	// Find the sequence of curves that is contiguous with
	// the given seed curve, regardless of curve 'properties'.
	static CReturn ProfileGrow(
						CGeoCurve* geoSeedCurve,
						double gapTol,
						CGeoCurveList* inputCurves,
						CGeoCurveList* outputCurves );

	static CReturn ProfileDirectionalGrow(
						CGeoCurve*		geoSeedCurve,
						CGeoCurve*		geoFinalCurve,
						bool			opposite_direction,
						double			gapTol,
						CGeoCurveList*	inputCurves,
						CGeoCurveList*	outputCurves );

protected:

private:  // Disabled.

	CGeoProfileBuilder();
	CGeoProfileBuilder( const CGeoProfileBuilder& );
	virtual ~CGeoProfileBuilder();
	const CGeoProfileBuilder& operator = ( const CGeoProfileBuilder& );
	int operator == ( const CGeoProfileBuilder& ) const;
	int operator != ( const CGeoProfileBuilder& ) const;

private:

};

#endif

