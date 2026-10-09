
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "2dVec.h"
#include "2dUnitVec.h"
#include "3dCoord.h"
#include "3dVec.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


C3dVec::C3dVec()
{
	Init( UNDEFINED, UNDEFINED, UNDEFINED );
}

C3dVec::C3dVec( double x, double y, double z )
{
	Init( x, y, z );
}

C3dVec::C3dVec( const C3dVec& vec )
{
	Init( vec.m_x, vec.m_y, vec.m_z );
}

void
C3dVec::Init( double dx, double dy, double dz )
{
	m_x = dx;
	m_y = dy;
	m_z = dz;
}

const C3dVec&
C3dVec::operator = ( const C3dVec& vec )
{
	m_x = vec.m_x;
	m_y = vec.m_y;
	m_z = vec.m_z;
	return *this;
}

C3dVec::operator C3dCoord() const
{
	C3dCoord coord( m_x, m_y, m_z );
	return coord;
}

C3dVec::operator C2dVec() const
{
	C2dVec vec( m_x, m_y );
	return vec;
}

C3dVec::operator C2dUnitVec() const
{
	C2dUnitVec vec( m_x, m_y );
	return vec;
}

C3dVec::~C3dVec()
{
}


double
C3dVec::Length() const
{
	double magSqrd = (*this) * (*this);
	double length = sqrt( magSqrd );
	return length;
}

// subtraction
C3dVec
C3dVec::operator - ( const C3dVec& vec ) const
{
	double dx = m_x - vec.m_x;
	double dy = m_y - vec.m_y;
	double dz = m_z - vec.m_z;
	return C3dVec( dx, dy, dz );
}

// subtract
C3dVec&
C3dVec::operator -= ( const C3dVec& vec )
{
	m_x -= vec.m_x;
	m_y -= vec.m_y;
	m_z -= vec.m_z;
	return *this;
}

// add
C3dVec
C3dVec::operator + ( const C3dVec& vec ) const
{
	double x = m_x + vec.m_x;
	double y = m_y + vec.m_y;
	double z = m_z + vec.m_z;
	return C3dVec( x, y, z );
} 

// accumulate
C3dVec&
C3dVec::operator += ( const C3dVec& vec )
{
	m_x += vec.m_x;
	m_y += vec.m_y;
	m_z += vec.m_z;
	return *this;
}

// multiply by scalar 
C3dVec
C3dVec::operator * ( double scalar ) const
{
	double x = m_x * scalar;
	double y = m_y * scalar;
	double z = m_z * scalar;
	return C3dVec( x, y, z );
}

// dot product
double
C3dVec::operator * ( const C3dVec& vec ) const
{
	double dot = m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z;
	return dot;
}

// cross product v = v0 ^ v1
C3dVec
C3dVec::operator ^ ( const C3dVec& vec ) const
{
	double a = m_y * vec.m_z - m_z * vec.m_y;
	double b = m_z * vec.m_x - m_x * vec.m_z;
	double c = m_x * vec.m_y - m_y * vec.m_x;
	return C3dVec( a, b, c );
}

