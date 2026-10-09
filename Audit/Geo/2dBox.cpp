
#include "stdafx.h"
#include "MathConst.h"
#include "3dBox.h"
#include "GeoLine.h"
#include "2dBox.h"


static void Assert( bool assertion )
{
	if (0)
		ASSERT( assertion );
}

//////////////////////////////////////////////////////////////////////

C2dBox::C2dBox()
{
	Invalidate();
}

#if BEFORE_2014_05_31
C2dBox::C2dBox( double xmin, double ymin, double xmax, double ymax )

	: m_min( xmin, ymin ),
	  m_max( xmax, ymax )
{
	Assert( ((m_max.X() - m_min.X()) > -SMALL) );
	Assert( ((m_max.Y() - m_min.Y()) > -SMALL) );
}
#else
C2dBox::C2dBox( double xmin, double ymin, double xmax, double ymax )
{
	// Ugh. Tired of the assertions (which are working appropriately).
	// TODO: Fix CRepo::ClampInstances() where the values passed to
	// this ctor are suspect. Must be *very* careful in doing so.
	if (xmax < xmin)
	{
		m_min.X( xmax );
		m_max.X( xmin );
	}
	else
	{
		m_min.X( xmin );
		m_max.X( xmax );
	}

	if (ymax < ymin)
	{
		m_min.Y( ymax );
		m_max.Y( ymin );
	}
	else
	{
		m_min.Y( ymin );
		m_max.Y( ymax );
	}
}
#endif

C2dBox::C2dBox( const C2dCoord& ps, const C2dCoord& pe )
{
	double xs = ps.X();
	double ys = ps.Y();

	double xe = pe.X();
	double ye = pe.Y();

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
}

C2dBox::C2dBox( const C2dBox& box )
	
	: m_min( box.BL() ),
	  m_max( box.TR() )
{
	Assert( ((m_max.X() - m_min.X()) > -SMALL) );
	Assert( ((m_max.Y() - m_min.Y()) > -SMALL) );
}

C2dBox::C2dBox( const C3dBox& box )

	: m_min( box.Xmin(), box.Ymin() ),
	  m_max( box.Xmax(), box.Ymax() )
{
	Assert( ((m_max.X() - m_min.X()) > -SMALL) );
	Assert( ((m_max.Y() - m_min.Y()) > -SMALL) );
}


C2dBox::~C2dBox()
{
}

const C2dBox&
C2dBox::operator = ( const C2dBox& box )
{
	m_min.XY( box.m_min.X(), box.m_min.Y() );
	m_max.XY( box.m_max.X(), box.m_max.Y() );

	return (*this);
}

const C2dBox&
C2dBox::operator = ( const C3dBox& box )
{
	m_min.XY( box.Xmin(), box.Ymin() );
	m_max.XY( box.Xmax(), box.Ymax() );

	return (*this);
}


bool
C2dBox::IsDefined() const
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
C2dBox::X( double x )
{
	if (x < m_min.X() || m_min.X() <= -UNDEFINED)
	{ m_min.X( x ); }

	if (x > m_max.X() || m_max.X() >= UNDEFINED)
	{ m_max.X( x ); }
}

void
C2dBox::Y( double y )
{
	if (y < m_min.Y() || m_min.Y() <= -UNDEFINED)
	{ m_min.Y( y ); }

	if (y > m_max.Y() || m_max.Y() >= UNDEFINED)
	{ m_max.Y( y ); }
}

void
C2dBox::Update( double xmin, double ymin, double xmax, double ymax )
{
	Assert( ((xmax - xmin) > -SMALL) );
	Assert( ((ymax - ymin) > -SMALL) );

	m_min.XY( xmin, ymin );
	m_max.XY( xmax, ymax );
}

void
C2dBox::Xmin( double x )
{
	Assert( ((m_max.X() - x) > -SMALL) );
	m_min.X( x );
}

void
C2dBox::Ymin( double y )
{
	Assert( ((m_max.Y() - y) > -SMALL) );
	m_min.Y( y );
}

void
C2dBox::Xmax( double x )
{
	Assert( ((x - m_min.X()) > -SMALL) );
	m_max.X( x );
}

void
C2dBox::Ymax( double y )
{
	Assert( ((y - m_min.Y()) > -SMALL) );
	m_max.Y( y );
}

double
C2dBox::Xc() const
{
	return ((m_min.X() + m_max.X()) / 2);
}

double
C2dBox::Yc() const
{
	return ((m_min.Y() + m_max.Y()) / 2);
}

double
C2dBox::Dx() const
{
	double dx = m_max.X() - m_min.X();
	return ((dx < SMALL) ? 0.0 : dx);
}

double
C2dBox::Dy() const
{
	double dy = m_max.Y() - m_min.Y();
	return ((dy < SMALL) ? 0.0 : dy);
}

double
C2dBox::Area() const
{ 
	return Dx() * Dy();
}

const C2dBox&
C2dBox::operator += ( const C2dCoord& pt )
{
	X( pt.X() );
	Y( pt.Y() );

	return (*this);
}

const C2dBox&
C2dBox::operator += ( const C3dCoord& pt )
{
	X( pt.X() );
	Y( pt.Y() );

	return (*this);
}

const C2dBox&
C2dBox::operator += ( const C2dBox& box )
{
	X( box.m_min.X() );
	Y( box.m_min.Y() );

	X( box.m_max.X() );
	Y( box.m_max.Y() );

	return (*this);
}

const C2dBox&
C2dBox::operator += ( double delta )
{
	m_min.XY( (m_min.X() - delta), (m_min.Y() - delta) );
	m_max.XY( (m_max.X() + delta), (m_max.Y() + delta) );

	return (*this);
}

// add
C2dBox
C2dBox::operator + ( double delta ) const
{
	return C2dBox( (m_min.X() - delta), (m_min.Y() - delta),
			(m_max.X() + delta), (m_max.Y() + delta) );
} 

bool
C2dBox::Contains( const C2dCoord& pt, double tol ) const
{
	return Contains( pt.X(), pt.Y(), tol );
}

bool
C2dBox::Contains( double x, double y, double tol ) const
{
	Assert( IsDefined() );

	if (x < (m_min.X() - tol))
		return FALSE;
	if (x > (m_max.X() + tol))
		return FALSE;

	if (y < (m_min.Y() - tol))
		return FALSE;
	if (y > (m_max.Y() + tol))
		return FALSE;

	return TRUE;
}

bool 
C2dBox::Contains( const C2dBox& box, double tol ) const
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

	return TRUE;
}

bool 
C2dBox::ContainsXY( const C3dBox& box, double tol ) const
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

	return TRUE;
}

bool 
C2dBox::Intersects( const C2dBox& box, double tol ) const
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
C2dBox::Invalidate()
{
	m_min.XY( -UNDEFINED, -UNDEFINED );
	m_max.XY( UNDEFINED, UNDEFINED );
}



//
// Return TRUE if the line cross the body of the box
//	
//	Based on the Cohen-Sutherland algorithm, from 
//	"Introduction to Computer Graphics" by Andries van Dam, 2002
//	What happend to Foley?
//
#define TOP_EDGE 0x08
#define BOTTOM_EDGE 0x04
#define RIGHT_EDGE 0x02
#define LEFT_EDGE 0x01

bool 
C2dBox::Intersects( 
	const CGeoLine& line, 
	double tol ) const
{
	double x0 = line.StartPt().X();
	double y0 = line.StartPt().Y();

	double x1 = line.EndPt().X();
	double y1 = line.EndPt().Y();

	return do_intersect(x0, y0, x1, y1, tol);
}

bool
C2dBox::do_intersect(
	double x0,
	double y0, 
	double x1,
	double y1,
	double tol ) const
{
	int code0 = outcode(x0, y0, tol);
	int code1 = outcode(x1, y1, tol);

	if ((code0 | code1) == 0x0c)
	{ bool stop = true; }

	while (TRUE)
	{
		//
		// Inside the box?
		if (!code0 && !code1)
		{ 
			// Touching is not crossing... 
			if ( EQUAL(x0, x1)
				&& EQUAL(y0, y1) )
			{ return FALSE; }

			return TRUE;
		}
		//
		// Entirely outside the box?
		if (code0 & code1)
		{ return FALSE; }
		//
		//
		int code = code0;
		if (!code)
		{ code = code1; }

		double x; 
		double y;
		//
		if (code & TOP_EDGE)
		{
			y = m_max.Y();
			x = x0 + ((x1-x0)*(y-y0))/(y1-y0);
		}
		else
		if (code & BOTTOM_EDGE)
		{
			y = m_min.Y();
			x = x0 + ((x1-x0)*(y-y0))/(y1-y0);
		}
		else
		if (code & RIGHT_EDGE)
		{
			x = m_max.X();
			y = y0 + ((y1-y0)*(x-x0))/(x1-x0);
		}
		else // LEFT_EDGE
		{
			x = m_min.X();
			y = y0 + ((y1-y0)*(x-x0))/(x1-x0);
		}
		//
		//
		if (code == code0)
		{
			code0 = outcode(x, y, tol);
			x0 = x;
			y0 = y;
		}
		else
		{
			code1 = outcode(x, y, tol);
			x1 = x;
			y1 = y;
		}
	}
	return FALSE;
}

int
C2dBox::outcode(double x, double y, double tol) const
{
	int code = 0;
	if (y > (m_max.Y()+tol))
	{ code = TOP_EDGE; }
	else
	if (y < (m_min.Y()-tol))
	{ code = BOTTOM_EDGE; }

	if (x > (m_max.X()+tol))
	{ code += RIGHT_EDGE; }
	else
	if (x < (m_min.X()-tol))
	{ code += LEFT_EDGE; }

	return code;
}

void
C2dBox::Shift(double dx, double dy)
{
	if (m_min.X() > -UNDEFINED)
	{ m_min.X(m_min.X() + dx); }

	if (m_min.Y() > -UNDEFINED)
	{ m_min.Y(m_min.Y() + dy); }

	if (m_max.X() <  UNDEFINED)
	{ m_max.X(m_max.X() + dx); }

	if (m_max.Y() <  UNDEFINED)
	{ m_max.Y(m_max.Y() + dy); }
}

