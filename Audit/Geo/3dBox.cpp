
#include "stdafx.h"
#include "MathConst.h"
#include "3dBox.h"



//////////////////////////////////////////////////////////////////////

C3dBox::C3dBox()
{
	Invalidate();
}

C3dBox::C3dBox( double xmin, double ymin, double zmin, double xmax, double ymax, double zmax )

	: m_min( xmin, ymin, zmin ),
	  m_max( xmax, ymax, zmax )
{
	ASSERT( ((m_max.X() - m_min.X()) > -SMALL) );
	ASSERT( ((m_max.Y() - m_min.Y()) > -SMALL) );
	ASSERT( ((m_max.Z() - m_min.Z()) > -SMALL) );
}

C3dBox::C3dBox( const C3dCoord& ps, const C3dCoord& pe )
{
	double xs = ps.X();
	double ys = ps.Y();
	double zs = ps.Z();

	double xe = pe.X();
	double ye = pe.Y();
	double ze = pe.Z();

	if (xs < xe)
	{
		m_min.X(xs);
		m_max.X(xe);
	}
	else
	{
		m_min.X(xe);
		m_max.X(xs);
	}

	if (ys < ye)
	{
		m_min.Y(ys);
		m_max.Y(ye);
	}
	else
	{
		m_min.Y(ye);
		m_max.Y(ys);
	}

	if (zs < ze)
	{
		m_min.Z( zs );
		m_max.Z( ze );
	}
	else
	{
		m_min.Z( ze );
		m_max.Z( zs );
	}
}

C3dBox::C3dBox( const C3dBox& box )

	: m_min( box.Min() ),
	  m_max( box.Max() )
{
	ASSERT( ((m_max.X() - m_min.X()) > -SMALL) );
	ASSERT( ((m_max.Y() - m_min.Y()) > -SMALL) );
	ASSERT( ((m_max.Z() - m_min.Z()) > -SMALL) );
}

C3dBox::~C3dBox()
{
}

const C3dBox&
C3dBox::operator = ( const C3dBox& box )
{
	m_min.XYZ( box.m_min.X(), box.m_min.Y(), box.m_min.Z() );
	m_max.XYZ( box.m_max.X(), box.m_max.Y(), box.m_max.Z() );

	return (*this);
}

const C3dBox&
C3dBox::operator = ( const C2dBox& box )
{
	m_min.X( box.Xmin() );
	m_min.Y( box.Ymin() );

	m_max.X( box.Xmax() );
	m_max.Y( box.Ymax() );

	return (*this);
}

bool
C3dBox::IsDefined() const
{
	if (m_min.X() <= -UNDEFINED)
		return FALSE;

	if (m_min.Y() <= -UNDEFINED)
		return FALSE;

	if (m_min.Z() <= -UNDEFINED)
		return FALSE;

	if (m_max.X() >=  UNDEFINED)
		return FALSE;

	if (m_max.Y() >=  UNDEFINED)
		return FALSE;

	if (m_max.Z() >=  UNDEFINED)
		return FALSE;

	return TRUE;
}

bool
C3dBox::IsDefinedXY() const
{
	if (m_min.X() <= -UNDEFINED)
		return FALSE;

	if (m_min.Y() <= -UNDEFINED)
		return FALSE;

	if (m_max.X() >=  UNDEFINED)
		return FALSE;

	if (m_max.Y() >=  UNDEFINED)
		return FALSE;

	return TRUE;
}


void
C3dBox::X( double x )
{
	if (x < m_min.X() || m_min.X() <= -UNDEFINED)
	{ m_min.X( x ); }

	if (x > m_max.X() || m_max.X() >= UNDEFINED)
	{ m_max.X( x ); }
}

void
C3dBox::Y( double y )
{
	if (y < m_min.Y() || m_min.Y() <= -UNDEFINED)
	{ m_min.Y( y ); }

	if (y > m_max.Y() || m_max.Y() >= UNDEFINED)
	{ m_max.Y( y ); }

}

void
C3dBox::Z( double z )
{
	if (z < m_min.Z() || m_min.Z() <= -UNDEFINED)
	{ m_min.Z( z ); }

	if (z > m_max.Z() || m_max.Z() >= UNDEFINED)
	{ m_max.Z( z ); }
}


C2dBox
C3dBox::XY() const
{
	return C2dBox( m_min.X(), m_min.Y(), m_max.X(), m_max.Y() );
}

C2dBox
C3dBox::YZ() const
{
	return C2dBox( m_min.Y(), m_min.Z(), m_max.Y(), m_max.Z() );
}

C2dBox
C3dBox::ZX() const
{
	return C2dBox( m_min.Z(), m_min.X(), m_max.Z(), m_max.X() );
}

void
C3dBox::Update( double xmin, double ymin, double zmin, double xmax, double ymax, double zmax )
{
	ASSERT( ((xmax - xmin) > -SMALL) );
	ASSERT( ((ymax - ymin) > -SMALL) );
	ASSERT( ((zmax - zmin) > -SMALL) );

	m_min.XYZ( xmin, ymin, zmin );
	m_max.XYZ( xmax, ymax, zmax );
}

void
C3dBox::Xmin( double x )
{
	ASSERT( ((m_max.X() - x) > -SMALL) );
	m_min.X( x );
}

void
C3dBox::Ymin( double y )
{
	ASSERT( ((m_max.Y() - y) > -SMALL) );
	m_min.Y( y );
}

void
C3dBox::Zmin( double z )
{
	ASSERT( ((m_max.Z() - z) > -SMALL) );
	m_min.Z( z );
}

void
C3dBox::Xmax( double x )
{
	ASSERT( ((x - m_min.X()) > -SMALL) );
	m_max.X( x );
}

void
C3dBox::Ymax( double y )
{
	ASSERT( ((y - m_min.Y()) > -SMALL) );
	m_max.Y( y );
}

void
C3dBox::Zmax( double z )
{
	ASSERT( ((z - m_min.Z()) > -SMALL) );
	m_max.Z( z );
}

double
C3dBox::Xc() const
{
	return ((m_min.X() + m_max.X()) / 2);
}

double
C3dBox::Yc() const
{
	return ((m_min.Y() + m_max.Y()) / 2);
}

double
C3dBox::Zc() const
{
	return ((m_min.Z() + m_max.Z()) / 2);
}

double
C3dBox::Dx() const
{
	double dx = m_max.X() - m_min.X();
	return ((dx < SMALL) ? 0.0 : dx);
}

double
C3dBox::Dy() const
{
	double dy = m_max.Y() - m_min.Y();
	return ((dy < SMALL) ? 0.0 : dy);
}

double
C3dBox::Dz() const
{
	double dz = m_max.Z() - m_min.Z();
	return ((dz < SMALL) ? 0.0 : dz);
}

const C3dBox&
C3dBox::operator += ( const C3dCoord& pt )
{
	X( pt.X() );
	Y( pt.Y() );
	Z( pt.Z() );

	return (*this);
}

const C3dBox&
C3dBox::operator += ( const C3dBox& box )
{
	X( box.m_min.X() );
	Y( box.m_min.Y() );
	Z( box.m_min.Z() );

	X( box.m_max.X() );
	Y( box.m_max.Y() );
	Z( box.m_max.Z() );

	return (*this);
}

bool
C3dBox::Contains( const C3dCoord& pt, double tol ) const
{
	return Contains( pt.X(), pt.Y(), pt.Z(), tol );
}

bool
C3dBox::Contains( double x, double y, double z, double tol ) const
{
	ASSERT( IsDefined() );

	if (x < (m_min.X() - tol))
		return FALSE;
	if (x > (m_max.X() + tol))
		return FALSE;

	if (y < (m_min.Y() - tol))
		return FALSE;
	if (y > (m_max.Y() + tol))
		return FALSE;

	if (z < (m_min.Z() - tol))
		return FALSE;
	if (z > (m_max.Z() + tol))
		return FALSE;

	return TRUE;
}

bool
C3dBox::Contains( const C3dBox& box, double tol ) const
{
	if ( !IsDefined() )
		return FALSE;

	if ( !box.IsDefined() )
		return FALSE;

	if (box.Xmax() > (m_max.X() +tol))
		return FALSE;
	if (box.Xmin() < (m_min.X() -tol))
		return FALSE;

	if (box.Ymax() > (m_max.Y() +tol))
		return FALSE;
	if (box.Ymin() < (m_min.Y() -tol))
		return FALSE;

	if (box.Zmax() > (m_max.Z() +tol))
		return FALSE;
	if (box.Zmin() < (m_min.Z() -tol))
		return FALSE;

	return TRUE;
}

bool
C3dBox::ContainsXY( const C3dBox& box, double tol ) const
{
	if ( !IsDefinedXY() )
		return FALSE;

	if ( !box.IsDefinedXY() )
		return FALSE;

	if (box.Xmax() > (m_max.X() +tol))
		return FALSE;
	if (box.Xmin() < (m_min.X() -tol))
		return FALSE;

	if (box.Ymax() > (m_max.Y() +tol))
		return FALSE;
	if (box.Ymin() < (m_min.Y() -tol))
		return FALSE;

	return TRUE;
}


bool
C3dBox::Intersects( const C3dBox& box, double tol ) const
{
	if ( !IsDefined() )
		return FALSE;

	if ( !box.IsDefined() )
		return FALSE;

	if (m_min.X() > (box.Xmax()+tol))
		return FALSE;
	if (m_max.X() < (box.Xmin()-tol))
		return FALSE;

	if (m_min.Y() > (box.Ymax()+tol))
		return FALSE;
	if (m_max.Y() < (box.Ymin()-tol))
		return FALSE;

	if (m_min.Z() > (box.Zmax()+tol))
		return FALSE;
	if (m_max.Z() < (box.Zmin()-tol))
		return FALSE;

	return TRUE;
}

bool
C3dBox::Intersects( const C2dBox& box, double tol ) const
{
	if ( !IsDefined() )
		return FALSE;

	if ( !box.IsDefined() )
		return FALSE;


	if (m_min.X() > (box.Xmax()+tol))
		return FALSE;
	if (m_max.X() < (box.Xmin()-tol))
		return FALSE;

	if (m_min.Y() > (box.Ymax()+tol))
		return FALSE;
	if (m_max.Y() < (box.Ymin()-tol))
		return FALSE;

	return TRUE;
}


void
C3dBox::Invalidate()
{
	m_min.XYZ( -UNDEFINED, -UNDEFINED, -UNDEFINED );
	m_max.XYZ( UNDEFINED, UNDEFINED, UNDEFINED );
}



void
C3dBox::Shift( const C3dVec& delta )
{
	m_min += delta;
	m_max += delta;
}