
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "3dCoord.h"
#include "2dCoord.h"




C2dCoord::C2dCoord()
{
	m_x = UNDEFINED;
	m_y = UNDEFINED;
}

C2dCoord::C2dCoord( const C2dCoord& pt )
{
	m_x = pt.m_x;
	m_y = pt.m_y;
}

C2dCoord::C2dCoord( double x, double y )
{
	m_x = x;
	m_y = y;
}

C2dCoord::~C2dCoord()
{
}

const C2dCoord&
C2dCoord::operator = ( const C2dCoord& pt )
{
	m_x = pt.m_x;
	m_y = pt.m_y;
	return *this;
}

C2dCoord::operator C3dCoord() const
{
	C3dCoord pt( m_x, m_y, UNDEFINED );
	return pt;
};


C2dVec
C2dCoord::operator - ( const C2dCoord& pt ) const
{
	C2dVec vec( (m_x - pt.m_x), (m_y - pt.m_y) );
	return vec;
}

C2dCoord
C2dCoord::operator + ( const C2dVec& vec ) const
{
	C2dCoord pt( *this );
	pt += vec;
	return pt;
}

C2dCoord
C2dCoord::operator - ( const C2dVec& vec ) const
{
	C2dCoord pt( *this );
	pt -= vec;
	return pt;
}

const C2dCoord&
C2dCoord::operator += ( const C2dVec& vec )
{
	m_x += vec.X();
	m_y += vec.Y();
	return *this;
}

const C2dCoord&
C2dCoord::operator -= ( const C2dVec& vec )
{
	m_x -= vec.X();
	m_y -= vec.Y();
	return *this;
}

bool
C2dCoord::WithinTol( const C2dCoord& pt, double tol ) const
{
	double dx = fabs( m_x - pt.m_x );

	if (dx > tol)
		return FALSE;

	double dy = fabs( m_y - pt.m_y );

	if (dy > tol)
		return FALSE;

	double magSqrd = dx * dx + dy * dy;
	double tolSqrd = tol * tol;

	return ((magSqrd <= tolSqrd) ? TRUE : FALSE);
}



double 
C2dCoord::Dist2(const C2dCoord& dest)
{
	double dx = m_x - dest.m_x;
	double dy = m_y - dest.m_y;

	return dx*dx + dy*dy;
}

double C2dCoord::Dist(const C2dCoord& dest)
{
	return sqrt(Dist2(dest));
}


bool C2dCoord::IsDefined() const
{
	return (DEFINED(m_x) && DEFINED(m_y));
}
