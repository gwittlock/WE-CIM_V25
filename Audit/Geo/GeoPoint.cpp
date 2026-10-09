
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "GeoPoint.h"


//////////////////////////////////////////////////////////////////////

CGeoPoint::CGeoPoint()

	: CGeoElem(),
	  m_pt()
{
}

CGeoPoint::CGeoPoint( const C3dCoord& pt )

	: CGeoElem(),
	  m_pt( pt )
{
	BoxUpdate();
}

CGeoPoint::CGeoPoint( const CGeoPoint& pt )

	: CGeoElem(),
	  m_pt( pt.m_pt )
{
	BoxUpdate();
}

CGeoPoint::CGeoPoint( double x, double y, double z )

	: CGeoElem(),
	  m_pt( x, y, z )
{
	BoxUpdate();
}

CGeoElem*
CGeoPoint::Clone( bool attribs_copy ) const
{
	CGeoPoint* pt = new CGeoPoint( *this );
	if (attribs_copy && this->HasAttrib())
		(*pt->pAttrib()) = Attrib();
	return pt;
}

const CGeoPoint&
CGeoPoint::operator = ( const C3dCoord& pt )
{
	m_pt = pt;
	BoxUpdate();
	return (*this);
}

const CGeoPoint&
CGeoPoint::operator = ( const CGeoPoint& pt )
{
	m_pt = pt.m_pt;
	BoxUpdate();
	return (*this);
}

CGeoPoint::~CGeoPoint()
{
}

ElemType
CGeoPoint::Type() const
{
	return GEOPOINT;
}

const C3dCoord&
CGeoPoint::StartPt() const
{
	return m_pt;
}

const C3dCoord&
CGeoPoint::EndPt() const
{
	return m_pt;
}

double
CGeoPoint::Length2d() const
{
	return 0.0;
}

void
CGeoPoint::StartPt( const C3dCoord& pt )
{
	m_pt = pt;
}

void
CGeoPoint::StartPt( double x, double y, double z )
{
	m_pt.X( x );
	m_pt.Y( y );
	m_pt.Z( z );

	BoxUpdate();
}

void
CGeoPoint::EndPt( const C3dCoord& pt )
{
	m_pt = pt;
}

void
CGeoPoint::EndPt( double x, double y, double z )
{
	m_pt.X( x );
	m_pt.Y( y );
	m_pt.Z( z );

	BoxUpdate();
}

void
CGeoPoint::Xform( const C3x4Matrix& xform )
{
	xform.Transform( &m_pt );
	BoxUpdate();
}

void
CGeoPoint::Shift( const C3dVec& delta )
{
	m_pt += delta;

	((C3dBox&)CGeoElem::Box()).Shift( delta );
//	BoxUpdate();
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Update the bounding box of the line.
void
CGeoPoint::BoxUpdate()
{
	double xmin = m_pt.X() - SMALL;
	double ymin = m_pt.Y() - SMALL;
	double zmin = m_pt.Z() - SMALL;

	double xmax = m_pt.X() + SMALL;
	double ymax = m_pt.Y() + SMALL;
	double zmax = m_pt.Z() + SMALL;

	C3dBox box( xmin, ymin, zmin, xmax, ymax, zmax );

	CGeoElem::Box( box );
}
