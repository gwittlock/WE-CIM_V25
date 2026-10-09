
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "2dUnitVec.h"
#include "3dVec.h"
#include "2dVec.h"


C2dVec::C2dVec()
{
	m_x = UNDEFINED;
	m_y = UNDEFINED;
}

C2dVec::C2dVec( double x, double y )
{
	m_x = x;
	m_y = y;
}

C2dVec::C2dVec( const C2dVec& vec )
{
	m_x = vec.m_x;
	m_y = vec.m_y;
}

void
C2dVec::Init( double dx, double dy )
{
	m_x = dx;
	m_y = dy;
}

void
C2dVec::Init( const C2dVec& vec )
{
	m_x = vec.m_x;
	m_y = vec.m_y;
}

const C2dVec&
C2dVec::operator = ( const C2dVec& vec )
{
	m_x = vec.m_x;
	m_y = vec.m_y;
	return *this;
}

C2dVec::operator C2dUnitVec() const
{
	C2dUnitVec vec( m_x, m_y );
	return vec;
}

C2dVec::operator C3dVec() const
{
	C3dVec vec( m_x, m_y, 0.0 );
	return vec;
}

C2dVec::~C2dVec()
{
}

double
C2dVec::Length() const
{
	double magSqrd = (*this) * (*this);
	double length = sqrt( magSqrd );
	return length;
}

// subtraction
C2dVec
C2dVec::operator - ( const C2dVec& vec ) const
{
	double dx = m_x - vec.m_x;
	double dy = m_y - vec.m_y;
	return C2dVec( dx, dy );
}

// subtract
C2dVec&
C2dVec::operator -= ( const C2dVec& vec )
{
	m_x -= vec.m_x;
	m_y -= vec.m_y;
	return *this;
}

// add
C2dVec
C2dVec::operator + ( const C2dVec& vec ) const
{
	double x = m_x + vec.m_x;
	double y = m_y + vec.m_y;
	return C2dVec( x, y );
} 

// accumulate
C2dVec&
C2dVec::operator += ( const C2dVec& vec )
{
	m_x += vec.m_x;
	m_y += vec.m_y;
	return *this;
}

// rotation
C2dVec
C2dVec::operator + ( double radians ) const
{
	double len = Length();
	double orient = Radians() + radians;
	return ( C2dVec( (len * cos(orient)), (len * sin(orient)) ) );
}

C2dVec&
C2dVec::operator += ( double radians )
{
	double len = Length();
	double orient = Radians() + radians;
	m_x = len * cos(orient);
	m_y = len * sin(orient);
	return (*this);
}

C2dVec
C2dVec::operator - ( double radians ) const
{
	double len = Length();
	double orient = Radians() - radians;
	return ( C2dVec( (len * cos(orient)), (len * sin(orient)) ) );
}

C2dVec&
C2dVec::operator -= ( double radians )
{
	double len = Length();
	double orient = Radians() - radians;
	m_x = len * cos(orient);
	m_y = len * sin(orient);
	return (*this);
}

// multiply by scalar 
C2dVec
C2dVec::operator * ( double scalar ) const
{
	double x = m_x * scalar;
	double y = m_y * scalar;
	return C2dVec( x, y );
}

C2dVec&
C2dVec::operator *= ( double scalar )
{
	m_x *= scalar;
	m_y *= scalar;
	return (*this);
}

// dot product
double
C2dVec::operator * ( const C2dVec& vec ) const
{
	double dot = m_x * vec.m_x + m_y * vec.m_y;
	return dot;
}

// perp-dot
double
C2dVec::PerpDot( const C2dVec& vec ) const
{
	double pdot = m_x*vec.m_y - m_y*vec.m_x;
	return pdot;
}


// cross product
double
C2dVec::operator ^ ( const C2dVec& vec ) const
{
	double cross = m_x * vec.m_y - m_y * vec.m_x;
	return cross;
}


// Result in radians where ( 0 <= result < TWOPI).
// Cloned from UnitVec
double
C2dVec::Radians() const
{
	double dir;

	// TODO:  Simplify, and use atan2() ???  NOTE: atan2() is
	// platform dependent!  And, as a general rule, you should
	// avoid using intrinsic functions.
	// But, what GOOD are the damn things if we can't use them? 
	//
	if (fabs( m_x ) < VECTOR_SMALL)
	{
		dir = ((m_y < 0) ? THREEHALFPI : HALFPI);
	}
	else if (fabs( m_y ) < VECTOR_SMALL)
	{
		dir = ((m_x < 0) ? PI : 0.0);
	}
	else
	{
		dir = atan( m_y/m_x );

		if (m_x < 0)
			dir += PI;

		if (dir < 0)
			dir += TWOPI;
	}

	return dir;
}
