
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "3dCoord.h"


C3dCoord::C3dCoord()
{
	m_x = UNDEFINED;
	m_y = UNDEFINED;
	m_z = UNDEFINED;
}

C3dCoord::C3dCoord( const C3dCoord& pt )
{
	m_x = pt.m_x;
	m_y = pt.m_y;
	m_z = pt.m_z;
}

C3dCoord::C3dCoord( double x, double y, double z )
{
	m_x = x;
	m_y = y;
	m_z = z;
}

C3dCoord::~C3dCoord()
{
}

const C3dCoord& C3dCoord::operator = ( const C3dCoord& pt )
{
	m_x = pt.m_x;
	m_y = pt.m_y;
	m_z = pt.m_z;
	return *this;
}

C3dCoord::operator C3dVec() const
{
	C3dVec vec( m_x, m_y, m_z );
	return vec;
}

C3dCoord::operator C2dCoord() const
{
	C2dCoord pt( m_x, m_y );
	return pt;
};

void C3dCoord::XYZ( double x, double y, double z )
{
	m_x = x;
	m_y = y;
	m_z = z;
}

C3dVec C3dCoord::operator - ( const C3dCoord& pt ) const
{
	C3dVec vec( (m_x - pt.m_x), (m_y - pt.m_y), (m_z - pt.m_z) );
	return vec;
}

C3dCoord C3dCoord::operator + ( const C3dVec& vec ) const
{
	C3dCoord pt( *this );
	pt += vec;
	return pt;
}

C3dCoord C3dCoord::operator - ( const C3dVec& vec ) const
{
	C3dCoord pt( *this );
	pt -= vec;
	return pt;
}

const C3dCoord& C3dCoord::operator += ( const C3dVec& vec )
{
	m_x += vec.X();
	m_y += vec.Y();
	m_z += vec.Z();
	return *this;
}

const C3dCoord& C3dCoord::operator -= ( const C3dVec& vec )
{
	m_x -= vec.X();
	m_y -= vec.Y();
	m_z -= vec.Z();
	return *this;
}

bool C3dCoord::WithinTol( const C3dCoord& pt, double tol ) const
{
	double dx = fabs( m_x - pt.m_x );

	if (dx > tol)
		return FALSE;

	double dy = fabs( m_y - pt.m_y );

	if (dy > tol)
		return FALSE;

	double dz = fabs( m_z - pt.m_z );

	if (dz > tol)
		return FALSE;

	double magSqrd = dx * dx + dy * dy + dz * dz;
	double tolSqrd = tol * tol;

	return ((magSqrd <= tolSqrd) ? TRUE : FALSE);
}


bool C3dCoord::WithinTolXY( const C3dCoord& pt, double tol ) const
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



double C3dCoord::Dist(const C3dCoord& dest) const
{
	double dx = m_x - dest.m_x;
	double dy = m_y - dest.m_y;
	double dz = m_z - dest.m_z;

	return sqrt( dx*dx + dy*dy + dz*dz );
}

double C3dCoord::Dist(const C2dCoord& dest) const
{
	double dx = m_x - dest.X();
	double dy = m_y - dest.Y();

	return sqrt( dx*dx + dy*dy );
}

double C3dCoord::DistXY(const C3dCoord& dest) const
{
	double dx = m_x - dest.m_x;
	double dy = m_y - dest.m_y;

	return sqrt( dx*dx + dy*dy );
}


bool C3dCoord::IsDefined() const
{
	return (DEFINED(m_x) && DEFINED(m_y) && DEFINED(m_z));
}

bool C3dCoord::IsDefinedXY() const
{
	return (DEFINED(m_x) && DEFINED(m_y));
}
