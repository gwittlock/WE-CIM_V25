#pragma once

#include "3dVec.h"
#include "2dCoord.h"
#include "IndxList.h"
#include "DynamicArray.h"


class dllExport C3dCoord
{
public:

	C3dCoord();

	C3dCoord( const C3dCoord& pt );

	C3dCoord( double x, double y, double z );

	// Assignment operator
	const C3dCoord& operator = ( const C3dCoord& pt );

	// Index operator
	double operator []( int idx ) const	{ return (&m_x)[idx]; }
	double& operator []( int idx )		{ return (&m_x)[idx]; }

	// Conversion operator
	operator C3dVec() const;
	operator C2dCoord() const;

	double inline X() const { return m_x; }
	double inline Y() const { return m_y; }
	double inline Z() const { return m_z; }

	void inline X( double x ) { m_x = x; }
	void inline Y( double y ) { m_y = y; }
	void inline Z( double z ) { m_z = z; }

	void XYZ( double x, double y, double z );

	// Create the vector between two points.
	C3dVec operator - ( const C3dCoord& pt ) const;

	// Create an offset point.
	C3dCoord operator + ( const C3dVec& vec ) const;
	C3dCoord operator - ( const C3dVec& vec ) const;

	// Translate this point.
	const C3dCoord& operator += ( const C3dVec& vec );
	const C3dCoord& operator -= ( const C3dVec& vec );

	bool WithinTol( const C3dCoord& pt, double tol ) const;
	bool WithinTolXY( const C3dCoord& pt, double tol ) const;

	double Dist(const C3dCoord& dest) const;
	double Dist(const C2dCoord& dest) const;
	double DistXY(const C3dCoord& dest) const;

	bool IsDefined() const;
	bool IsDefinedXY() const;

	virtual ~C3dCoord();

private:

	// Disabled.
	int operator == ( const C3dCoord& pt ) const;
	int operator != ( const C3dCoord& pt ) const;

private:

	double	m_x;
	double	m_y;
	double	m_z;
};

typedef CIndxList<C3dCoord*>		C3dCoordList;
typedef CDynamicArray<C3dCoord*>	C3dCoordArray;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Used by entity display system.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <afxtempl.h>

typedef CArray<C3dCoord, C3dCoord>		tPnt3Array;

#ifndef _TCMDARRAY
#define _TCMDARRAY
typedef CArray<__int64, __int64>		tCmdArray;
#endif
