
#ifndef _GEOREDUCER_H
#define _GEOREDUCER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _GEOCURVE_H
#include "GeoCurve.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CGeoReducer
{
public:

	//static void ArcsReduce( CGeoCurveList& curves, double tol );
	static void ArcsReduce( CGeoCurveArray& curves, bool wrap, double tol );

	// void LinesReduce( CGeoCurveList& curves, bool wrap, double tol );
	static void LinesReduce( CGeoCurveArray& curves, bool wrap, double tol );

protected:

private:  // Methods

private:  // Disabled

	CGeoReducer();
	CGeoReducer( const CGeoReducer& );
	const CGeoReducer& operator = ( const CGeoReducer& );
	int operator == ( const CGeoReducer& ) const;
	int operator != ( const CGeoReducer& ) const;
	virtual ~CGeoReducer();

private:  // Data

};

#endif

