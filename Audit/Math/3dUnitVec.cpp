
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "3dVec.h"
#include "3dUnitVec.h"



C3dUnitVec::C3dUnitVec()
{
	Init( 0., 0., 0. );
}

C3dUnitVec::C3dUnitVec( double x, double y, double z )
{
	Init( x, y, z );
}

C3dUnitVec::C3dUnitVec( const C3dUnitVec& vec )
{
//	Init( vec.m_x, vec.m_y );
	m_x = vec.m_x;
	m_y = vec.m_y;
	m_z = vec.m_z;
}

C3dUnitVec::C3dUnitVec( double radians )
{
//	double x = cos( radians );
//	double y = sin( radians );
//	Init( x, y );
	m_x = cos( radians );
	m_y = sin( radians );
}

C3dUnitVec::~C3dUnitVec()
{
}

const C3dUnitVec&
C3dUnitVec::operator = ( const C3dUnitVec& vec )
{
	m_x = vec.m_x;
	m_y = vec.m_y;
	m_z = vec.m_z;
	return *this;
}

C3dUnitVec::operator C3dVec() const
{
	C3dVec vec( m_x, m_y, m_z );
	return vec;
}

double
C3dUnitVec::X() const
{
	return m_x;
}

double
C3dUnitVec::Y() const
{
	return m_y;
}

double
C3dUnitVec::Z() const
{
	return m_z;
}

void
C3dUnitVec::X( double x )
{
	m_x = x;
	Unitize();
}

void
C3dUnitVec::Y( double y )
{
	m_y = y;
	Unitize();
}

void
C3dUnitVec::Z( double z )
{
	m_z = z;
	Unitize();
}

// subtraction
C3dUnitVec
C3dUnitVec::operator - ( const C3dUnitVec& vec ) const
{
	double dx = m_x - vec.m_x;
	double dy = m_y - vec.m_y;
	double dz = m_z - vec.m_z;
	return C3dUnitVec( dx, dy, dz );
}

// subtract
C3dUnitVec&
C3dUnitVec::operator -= ( const C3dUnitVec& vec )
{
	m_x -= vec.m_x;
	m_y -= vec.m_y;
	m_z -= vec.m_z;

	Init( m_x, m_y, m_z );

	return *this;
}

C3dVec
C3dUnitVec::operator * ( double scalar ) const
{
	C3dVec vec(*this);
	return (vec * scalar);
}

// dot product
double
C3dUnitVec::operator * ( const C3dUnitVec &vec ) const
{
	// Returns the cosine of the angle between the two vectors,
	// times the product of the length (unit) of the two vectors.
	// Cool, huh?
	double dot = m_x * vec.m_x + m_y * vec.m_y + m_z * vec.m_z;
	return dot;
}

// cross product
C3dUnitVec
C3dUnitVec::operator ^ ( const C3dUnitVec &vec ) const
{
	double dx = m_y * vec.m_z - m_z * vec.m_y;
	double dy = m_z * vec.m_x - m_x * vec.m_z;
	double dz = m_x * vec.m_y - m_y * vec.m_x;

	return C3dUnitVec( dx, dy, dz );
}

void
C3dUnitVec::Reverse()
{
	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
}

void
C3dUnitVec::Init( double dx, double dy, double dz )
{
	m_x = dx;
	m_y = dy;
	m_z = dz;

	Unitize();
}

// Insure that we do have a unit vector.
void
C3dUnitVec::Unitize()
{
	double dSqrd = m_x * m_x + m_y * m_y + m_z * m_z;

	double arcLen = sqrt( dSqrd );

	if (arcLen < SMALL)
	{
		m_x = 0.0;
		m_y = 0.0;
		m_z = 0.0;
	}
	else
	{
		m_x = m_x / arcLen;
		m_y = m_y / arcLen;
		m_z = m_z / arcLen;
	}

	Adjust();
}

// Help avoid accumulating error.
void
C3dUnitVec::Adjust()
{
	if (fabs( m_x ) < VECTOR_SMALL)
		m_x = 0.0;

	if (fabs( m_y ) < VECTOR_SMALL)
		m_y = 0.0;

	if (fabs( m_z ) < VECTOR_SMALL)
		m_z = 0.0;
}
