#pragma once

#include "2dVec.h"
#include "DynamicArray.h"

class C3dCoord;


class dllExport C2dCoord
{
public:

	C2dCoord();

	C2dCoord( const C2dCoord& pt );

	C2dCoord( double x, double y );

	// Assignment operator
	const C2dCoord& operator = ( const C2dCoord& pt );

	// Index operator
	double operator [] ( int idx ) const	{ return (&m_x)[idx]; }
	double& operator [] ( int idx )			{ return (&m_x)[idx]; }

	// Conversion operator
	operator C3dCoord() const;

	double inline X() const { return m_x; }
	double inline Y() const { return m_y; }

	void inline X( double x ) { m_x = x; }
	void inline Y( double y ) { m_y = y; }

	void inline XY( double x, double y )	{ m_x = x; m_y = y; }

	// Create the vector between two points.
	C2dVec operator - ( const C2dCoord& pt ) const;

	// Create an offset point.
	C2dCoord operator + ( const C2dVec& vec ) const;
	C2dCoord operator - ( const C2dVec& vec ) const;

	// Translate this point.
	const C2dCoord& operator += ( const C2dVec& vec );
	const C2dCoord& operator -= ( const C2dVec& vec );

	bool WithinTol( const C2dCoord& pt, double tol ) const;

	double Dist2(const C2dCoord& dest);
	double Dist(const C2dCoord& dest);

	bool IsDefined() const;

	virtual ~C2dCoord();

private:

	// Disabled.
	int operator == ( const C2dCoord& pt ) const;
	int operator != ( const C2dCoord& pt ) const;

private:

	double m_x;
	double m_y;
};


typedef CDynamicArray<C2dCoord*> C2dCoordArray;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Used by entity display system.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <afxtempl.h>

typedef CArray<C2dCoord, C2dCoord>		tPnt2Array;

#ifndef _TCMDARRAY
#define _TCMDARRAY
typedef CArray<__int64, __int64>		tCmdArray;
#endif
