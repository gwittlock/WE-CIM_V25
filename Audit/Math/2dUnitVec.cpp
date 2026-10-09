
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "2dVec.h"
#include "2dUnitVec.h"



C2dUnitVec::C2dUnitVec()
{
	Init( 0., 0. );
}

C2dUnitVec::C2dUnitVec( double x, double y )
{
	Init( x, y );
}

C2dUnitVec::C2dUnitVec( const C2dUnitVec& vec )
{
	m_x = vec.m_x;
	m_y = vec.m_y;
}

C2dUnitVec::C2dUnitVec( double radians )
{
	m_x = cos( radians );
	m_y = sin( radians );
}

C2dUnitVec::~C2dUnitVec()
{
}

bool 
C2dUnitVec::IsValid() const
{
	return ((fabs(m_x) > 0.) || (fabs(m_y) > 0.));
}

const C2dUnitVec&
C2dUnitVec::operator = ( const C2dUnitVec& vec )
{
	m_x = vec.m_x;
	m_y = vec.m_y;
	return *this;
}

C2dUnitVec::operator C2dVec() const
{
	C2dVec vec( m_x, m_y );
	return vec;
}

double
C2dUnitVec::X() const
{
	return m_x;
}

double
C2dUnitVec::Y() const
{
	return m_y;
}

void
C2dUnitVec::X( double x )
{
	m_x = x;
	Unitize();
}

void
C2dUnitVec::Y( double y )
{
	m_y = y;
	Unitize();
}
	
C2dUnitVec
C2dUnitVec::operator + ( double radians ) const
{
	C2dUnitVec vec( *this );
	vec += radians;
	vec.Adjust();
	return vec;
}
	
const C2dUnitVec&
C2dUnitVec::operator += ( double radians )
{
	double sinR = sin( radians );
	double cosR = cos( radians );

	double x = m_x * cosR - m_y * sinR;
	double y = m_x * sinR + m_y * cosR;

	Init( x, y);

	return *this;
}
	
C2dUnitVec
C2dUnitVec::operator - ( double radians ) const
{
	C2dUnitVec vec( *this );
	vec -= radians;
	vec.Adjust();
	return vec;
}
	
const C2dUnitVec&
C2dUnitVec::operator -= ( double radians )
{
	double sinR = sin( -radians );
	double cosR = cos( -radians );

	double x = m_x * cosR - m_y * sinR;
	double y = m_x * sinR + m_y * cosR;

	Init( x, y);

	return *this;
}

// subtraction
C2dUnitVec
C2dUnitVec::operator - ( const C2dUnitVec& vec ) const
{
	double dx = m_x - vec.m_x;
	double dy = m_y - vec.m_y;
	return C2dUnitVec( dx, dy );
}

// subtract
C2dUnitVec&
C2dUnitVec::operator -= ( const C2dUnitVec& vec )
{
	m_x -= vec.m_x;
	m_y -= vec.m_y;

	Init( m_x, m_y );

	return *this;
}

C2dVec
C2dUnitVec::operator * ( double scalar ) const
{
	C2dVec vec(*this);
	vec *= scalar;
	return vec;
}

// dot product
double
C2dUnitVec::operator * ( const C2dUnitVec &vec ) const
{
	// Returns the cosine of the angle between the two vectors,
	// times the product of the length (unit) of the two vectors.
	// Cool, huh?
	double dot = m_x * vec.m_x + m_y * vec.m_y;
	return dot;
}

// perp-dot
double
C2dUnitVec::PerpDot( const C2dUnitVec& vec ) const
{
	// Returns the sine of the angle between the two vectors,
	// times the product of the length (unit) of the two vectors.
	// Like Dot, but rotated 90'
	// Pos if vec is ccw from this
	// Neg if vec is cw from this
	// zero if vec is parallel or anti-parallel
	double pdot = m_x*vec.m_y - m_y*vec.m_x;
	return pdot;
}

double
C2dUnitVec::AngleTo( const C2dUnitVec& vec, bool winding ) const
{
	// We gots choices; however, the sin-1 version can determine the correct
	// winding for the angle, and not just it's magnitude.
	double ang;
	if (winding)
		ang = asin(m_x*vec.m_y - m_y*vec.m_x);
	else
		ang = acos(m_x*vec.m_x + m_y*vec.m_y);

	return ang;
}

// cross product
double
C2dUnitVec::operator ^ ( const C2dUnitVec &vec ) const
{
	double cross = m_x * vec.m_y - m_y * vec.m_x;
	return cross;
}

// Result in radians where ( 0 <= result < TWOPI).
double
C2dUnitVec::Radians() const
{
	double dir;

	// TODO:  Simplify, and use atan2() ???  NOTE: atan2() is
	// platform dependent!  And, as a general rule, you should
	// avoid using intrinsic functions.
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

void
C2dUnitVec::Radians( double radians )
{
	Init( cos( radians ), sin( radians ) );
}

void
C2dUnitVec::Init( double dx, double dy )
{
	m_x = dx;
	m_y = dy;

	Unitize();
}

// Insure that we do have a unit vector.
void
C2dUnitVec::Unitize()
{
	double dSqrd = m_x * m_x + m_y * m_y;

	double arcLen = sqrt( dSqrd );

	if (arcLen < SMALL)
	{
		m_x = 0.0;
		m_y = 0.0;
	}
	else
	{
		m_x = m_x / arcLen;
		m_y = m_y / arcLen;
	}

	Adjust();
}

// Help avoid accumulating error.
void
C2dUnitVec::Adjust()
{
	if (fabs( m_x ) < VECTOR_SMALL)
		m_x = 0.0;

	if (fabs( m_y ) < VECTOR_SMALL)
		m_y = 0.0;
}
