
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "Register.h"
#include "cmn_resource.h"
#include "GeoLine.h"


//////////////////////////////////////////////////////////////////////

CGeoLine::CGeoLine()

	: CGeoCurve(),
	  m_ps( UNDEFINED, UNDEFINED, UNDEFINED ),
	  m_pe( UNDEFINED, UNDEFINED, UNDEFINED )
{
}

CGeoLine::CGeoLine( const C3dCoord& start, const C3dCoord& end )

	: CGeoCurve()
{
	Init( start, end );
}

CGeoLine::CGeoLine( double xs, double ys, double xe, double ye )

	: CGeoCurve(),
	  m_ps( xs, ys, UNDEFINED ),
	  m_pe( xe, ye, UNDEFINED )
{
	BoxUpdate();
}

CGeoLine::CGeoLine( double xs, double ys, double zs, double xe, double ye, double ze )

	: CGeoCurve(),
	  m_ps( xs, ys, zs ),
	  m_pe( xe, ye, ze )
{
	BoxUpdate();
}

CGeoLine::CGeoLine( const CGeoLine& line )

	: CGeoCurve(),
	  m_ps( line.m_ps ),
	  m_pe( line.m_pe )
{
	CGeoElem::Box( line.Box() );
//	BoxUpdate();
}

void
CGeoLine::Init( const C3dCoord& start, const C3dCoord& end )
{
	m_ps = start;
	m_pe = end;
	BoxUpdate();
}

CGeoElem*
CGeoLine::Clone( bool attribs_copy ) const
{
	CGeoLine* line = new CGeoLine( *this );
	if (attribs_copy && this->HasAttrib())
		(*line->pAttrib()) = Attrib();
	return line;
}

const CGeoLine&
CGeoLine::operator = ( const CGeoLine& line )
{
	m_ps = line.m_ps;
	m_pe   = line.m_pe;
	CGeoElem::Box( line.Box() );
//	BoxUpdate();
	return (*this);
}

CGeoLine::~CGeoLine()
{
}

ElemType
CGeoLine::Type() const
{
	return GEOLINE;
}

const C3dCoord&
CGeoLine::StartPt() const
{
	return m_ps;
}

const C3dCoord&
CGeoLine::EndPt() const
{
	return m_pe;
}

double
CGeoLine::Length2d() const
{
	double dx = m_pe.X() - m_ps.X();
	double dy = m_pe.Y() - m_ps.Y();

	double lenSqrd = dx * dx + dy * dy;
	double len = sqrt( lenSqrd );

	return len;
}

C2dUnitVec CGeoLine::StartTan() const
{
	return C2dUnitVec( (m_pe - m_ps) );
}

C2dUnitVec CGeoLine::EndTan() const
{
	return C2dUnitVec( (m_pe - m_ps) );
}

C2dUnitVec CGeoLine::TanAtPt( const C3dCoord& pt ) const
{
	return C2dUnitVec( (m_pe - m_ps) );
}

C2dUnitVec CGeoLine::TanAtPt( double x, double y ) const
{
	return C2dUnitVec( (m_pe - m_ps) );
}

CGeoCurve* CGeoLine::Offset( int dir, double amt ) const
{
	if (dir == 0)
		amt = 0.0;
	else
		dir = ((dir < 0) ? -1 : 1);

	C2dVec vec = (StartTan() + (HALFPI * dir));

	C3dCoord start = m_ps + (vec * amt);
	C3dCoord end = m_pe + (vec * amt);

	CGeoLine* offset = new CGeoLine( start, end );

	if (offset == NULL)
	{
		CReturn status;
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CGeoLine::Offset(#1)" );
	}
	else
	{
		if (AttribCount() > 0)
			*(offset->pAttrib()) = Attrib();
	}

	return offset;
}

void
CGeoLine::Reverse()
{
	C3dCoord tmp = m_ps;
	m_ps = m_pe;
	m_pe = tmp;
}

double
CGeoLine::PointClosest( const C3dCoord& pt, C3dCoord* closestPt, double* u ) const
{
	// Calculate the mangitude^2 of the reference line.
	double dx = m_pe.X() - m_ps.X();
	double dy = m_pe.Y() - m_ps.Y();
	double magSqrd = dx*dx + dy*dy;

	// ASSERT( (magSqrd > (SMALL * SMALL)) );
	if(magSqrd <= (SMALL * SMALL))
		return UNDEFINED;  // fails on zero-length lines.

	double dot = dx * (pt.X()-m_ps.X()) + dy * (pt.Y()-m_ps.Y());

	double uParam = dot / magSqrd;

	C3dCoord cpt( m_ps.X()+dx*uParam,
				  m_ps.Y()+dy*uParam,
				  m_ps.Z() );

	if (u)
		(*u) = uParam;
	if (closestPt)
		(*closestPt) = cpt;

	dx = cpt.X() - pt.X();
	dy = cpt.Y() - pt.Y();
	return sqrt( dx*dx + dy*dy );
}

double
CGeoLine::PointUparam( const C3dCoord& pt ) const
{
	// Calculate the mangitude^2 of the reference line.
	double dx = m_pe.X() - m_ps.X();
	double dy = m_pe.Y() - m_ps.Y();
	double magSqrd = dx*dx + dy*dy;

	ASSERT( (magSqrd > (SMALL * SMALL)) );  // fails on zero-length lines.

	double dot = dx * (pt.X()-m_ps.X()) + dy * (pt.Y()-m_ps.Y());

	return dot / magSqrd;
}

// 2014.02.05 (PE) -- Overlapping parts in a nest were caused by failures in CGeoPoly::OR().
// Said method was calling CGeoLine::PointSide() with a tolerance of 5.e-5, a tolerance
// that was inappropriately large for the geometry under consideration. Edwin likely chose
// this solution, instead of calculating the closest point to the line, because of its
// efficiency. That said, it's current implementation may continue to cause problems.
int CGeoLine::PointSide( const C3dCoord& pt, double tol ) const
{
	double pdot = (m_pe.X()-m_ps.X()) * (pt.Y()-m_ps.Y()) - (m_pe.Y()-m_ps.Y()) * (pt.X()-m_ps.X());

	if (CLOSE(0.0, pdot, tol))
		return 0;

	return (pdot<0.0)?+1:-1;
}


// ASSUMPTION: The given point was derived from the intersection
// between this line and another curve.  That is, the given point
// lies on the unbounded representation of this line.
bool
CGeoLine::PointOnSeg( const C3dCoord& pt ) const
{
	C2dUnitVec vecA = pt - m_ps;
	C2dUnitVec vecB = pt - m_pe;
	double dot = vecA * vecB;
	return (dot < VECTOR_SMALL);
}

C3dCoord
CGeoLine::MidPt() const
{
	double x = 0.5 * (m_ps.X() + m_pe.X());
	double y = 0.5 * (m_ps.Y() + m_pe.Y());
	double z = 0.5 * (m_ps.Z() + m_pe.Z());
	return C3dCoord( x, y, z );
}

C3dCoord
CGeoLine::PointAtDist( double dist, bool fromStart ) const
{
	double dx, dy, dz;

	C3dVec vec = m_pe - m_ps;
	double len = vec.Length();

	if (len >= SMALL)
	{
		if (!fromStart)
			len = -len;

		dx = vec.X() / len;
		dy = vec.Y() / len;
		dz = vec.Z() / len;
	}
	else
	{
		dx = dy = dz = 0.0;
	}

	const C3dCoord& ref = ((fromStart) ? m_ps : m_pe);

	return C3dCoord(
					(ref.X() + (dist * dx)),
					(ref.Y() + (dist * dy)),
					(ref.Z() + (dist * dz)) );
}

void
CGeoLine::StartPt( const C3dCoord& pt )
{
	StartPt( pt.X(), pt.Y(), pt.Z() );
}

void
CGeoLine::StartPt( double xs, double ys, double zs )
{
	m_ps.X( xs );
	m_ps.Y( ys );
	m_ps.Z( zs );

	BoxUpdate();
}

void
CGeoLine::EndPt( const C3dCoord& pt )
{
	EndPt( pt.X(), pt.Y(), pt.Z() );
}

void
CGeoLine::EndPt( double xe, double ye, double ze )
{
	m_pe.X( xe );
	m_pe.Y( ye );
	m_pe.Z( ze );

	BoxUpdate();
}

void
CGeoLine::Xform( const C3x4Matrix& xform )
{
	xform.Transform( &m_ps );
	xform.Transform( &m_pe );
	BoxUpdate();
}

void
CGeoLine::Shift( const C3dVec& delta )
{
	m_ps += delta;
	m_pe += delta;

	((C3dBox&)CGeoElem::Box()).Shift( delta );
//	BoxUpdate();
}

void
CGeoLine::Tabulate(
	double			chordal_tol,
	const C3dVec&	shift,
	C3dCoordArray*	pts ) const
{
	C3dCoord	pt;

	pt.XYZ( (m_ps.X() + shift.X()), (m_ps.Y() + shift.Y()), (m_ps.Z() + shift.Z()) );
	ConditionalAppend( pt, pts );

	pt.XYZ( (m_pe.X() + shift.X()), (m_pe.Y() + shift.Y()), (m_pe.Z() + shift.Z()) );
	ConditionalAppend( pt, pts );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Update the bounding box of the line.
void
CGeoLine::BoxUpdate()
{
	C3dBox box( m_ps, m_pe );

	CGeoElem::Box( box );

	if ( CRegister::Debug("GeoLine") )
	{
		CReturn status;
		double len = Length2d();
		if (len < SMALL)
			status.Diagnostic( "CGeoLine::BoxUpdate() -- attempt to create zero-length line" );
	}
}




// ==================================================================
//
double 
CGeoLine::InterceptX( 
	double at_y ) const
{
	double dx = m_pe.X() - m_ps.X();
	if (fabs(dx) < SMALL)
		return m_ps.X();

	double dy = m_pe.Y() - m_ps.Y();
	if (fabs(dy) > SMALL)
	{
		double slope = dx / dy;
		return m_ps.X() + (at_y - m_ps.Y()) * slope;
	}

	return UNDEFINED;
}

// ==================================================================

double 
CGeoLine::InterceptY(
	double at_x ) const
{
	double dy = m_pe.Y() - m_ps.Y();
	if (fabs(dy) < SMALL)
		return m_ps.Y();

	double dx = m_pe.X() - m_ps.X();
	if (fabs(dx) > SMALL)
	{
		double slope = dy / dx;
		return m_ps.Y() + (at_x - m_ps.X()) * slope;
	}

	return UNDEFINED;
}

// ==================================================================

double 
CGeoLine::InterceptXY(
	int		prim_ord,
	double	at ) const
{
	int sec_ord = FLIP_XY(prim_ord);

	double d1 = m_pe[prim_ord] - m_ps[prim_ord];
	if (fabs(d1) < SMALL)
		return m_ps[prim_ord];

	double d2 = m_pe[sec_ord] - m_ps[sec_ord];
	if (fabs(d2) > SMALL)
	{
		double slope = d1 / d2;
		return m_ps[prim_ord] + (at - m_ps[sec_ord]) * slope;
	}

	return UNDEFINED;
}


void
CGeoLine::Dump() const
{
	CReturn ret;
	CString str;
	str.Format( "L %f, %f to %f, %f", ROUND(m_ps.X(), 0.01), ROUND(m_ps.Y(), 0.01), 
									ROUND(m_pe.X(), 0.01), ROUND(m_pe.Y(), 0.01) );
	ret.Diagnostic(str);
}