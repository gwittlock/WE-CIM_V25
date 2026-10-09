
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "cmn_resource.h"
#include "GeoLine.h"
#include "GeoArc.h"

//////////////////////////////////////////////////////////////////////

CGeoArc::CGeoArc()

	: CGeoCurve(),
	  m_ps( UNDEFINED, UNDEFINED, UNDEFINED ),
	  m_pe( UNDEFINED, UNDEFINED, UNDEFINED ),
	  m_pc( UNDEFINED, UNDEFINED, UNDEFINED ),
	  m_dir( 0 ),
	  m_radius( UNDEFINED )
{
}

CGeoArc::CGeoArc( const C3dCoord& start, const C3dCoord& end, const C3dCoord& center, int dir )

	: CGeoCurve()
{
	Init( start, end, center, dir );
}

CGeoArc::CGeoArc( const C3dCoord& center, double radius, int dir )

	: CGeoCurve()
{
	Init( center, radius, dir );
}

CGeoArc::CGeoArc( const CGeoArc& arc )

	: CGeoCurve(),
	m_ps(arc.m_ps),
	m_pe(arc.m_pe),
	m_pc(arc.m_pc),
	m_radius(arc.m_radius),
	m_dir(arc.m_dir)
{
//	Init( arc.m_ps, arc.m_pe, arc.m_pc, arc.m_dir );
	// Hmmmmmm ... should we copy the attributes too?
	CGeoElem::Box( arc.Box() );
}

CGeoElem* CGeoArc::Clone( bool attribs_copy ) const
{
	CGeoArc* arc = new CGeoArc( *this );
	if (attribs_copy && this->HasAttrib())
		(*arc->pAttrib()) = Attrib();
	return arc;
}

const CGeoArc&
CGeoArc::operator = ( const CGeoArc& arc )
{
//	Init( arc.m_ps, arc.m_pe, arc.m_pc, arc.m_dir );
	m_ps  = arc.m_ps;
	m_pe  = arc.m_pe;
	m_pc  = arc.m_pc;
	m_radius = arc.m_radius;
	m_dir = arc.m_dir;

	CGeoElem::Box( arc.Box() );

	return (*this);
}

void
CGeoArc::Init( const C3dCoord& start, const C3dCoord& end, const C3dCoord& center, int dir )
{
	m_ps  = start;
	m_pe  = end;
	m_pc  = center;
	m_dir = ((dir < 0) ? -1 : 1);

	C2dVec vecA = m_ps - m_pc;
	C2dVec vecB = m_pe - m_pc;

	double lenA = vecA.Length();
	double lenB = vecB.Length();

// NOTE:  In order to "merge" solutions into a few simple
//	cases, it is important to be able to represent a zero-radius
//	circle.  As disgusting as that may be.... once they are working,
// I can break the solutions out to have special cases (at an increase
//	of several hundred lines of code)... or we can filter zero-arcs out
//	at the UI end (which I added).  Sorry.  Edwin.
//	9 June 99
//
//	ASSERT( (m_radius >= SMALL) );

	double delta = fabs( lenA - lenB );

	ASSERT( (delta < SMALL) );

	m_radius = (lenA + lenB) / 2;

	BoxUpdate();
}

void
CGeoArc::Init( const C3dCoord& center, double radius, int dir )
{
	C3dCoord pt( (center.X() + radius), center.Y(), center.Z() );
	Init( pt, pt, center, dir );
}

CGeoArc::~CGeoArc()
{
}

ElemType
CGeoArc::Type() const
{
	return GEOARC;
}

bool CGeoArc::IsCircle( double tol ) const
{
	return ( IsClosed( tol ) );
}

bool CGeoArc::IsClosed( double tol ) const
{
	bool is_closed = false;
	
	C2dVec vec = m_pe - m_ps;
	if (vec.Length() < tol)
	{
		// Assume a full circle.
		is_closed = true;
	}
	else
	{
		C2dUnitVec vecA = StartToEndVec();
		C2dUnitVec vecB = EndTan();

		double dot = vecA * vecB;
		is_closed = (dot > (1. - 1.e-3));  // arbitrary tolerance
	}

	return is_closed;
}

const C3dCoord&
CGeoArc::StartPt() const
{
	return m_ps;
}

const C3dCoord&
CGeoArc::EndPt() const
{
	return m_pe;
}

double
CGeoArc::Length2d() const
{
	double radians = IncludedAngle();

	// Circumferance = 2pi * R = (included angle) * R
	double arcLen = radians * m_radius;

	return arcLen;
}

C2dUnitVec
CGeoArc::StartTan() const
{
	C2dUnitVec vec( (m_ps - m_pc) );

	C2dUnitVec tan = vec + (m_dir * HALFPI);

	return tan;
}

C2dUnitVec
CGeoArc::EndTan() const
{
	C2dUnitVec vec( (m_pe - m_pc) );

	C2dUnitVec tan = vec + (m_dir * HALFPI);

	return tan;
}

bool
CGeoArc::Convex( int side )	const // Left -1, Right +1
{
	return (side * m_dir) > 0;
}

CGeoCurve* CGeoArc::Offset( int dir, double amt ) const
{
	if (dir == 0)
		amt = 0.0;
	else
		dir = ((dir < 0) ? -1 : 1);
		// amt = fabs( amt );

	if ((dir * m_dir) > 0)
	{
		// Offsetting to the inside of the arc.
		if ((m_radius - amt) < SMALL)
				return NULL;  // Can't offset arc.  Can't cough an error either.
	}

	double rotation = dir * HALFPI;

	C2dVec startVec = (StartTan() + rotation) * amt;
	C2dVec endVec = (EndTan() + rotation) * amt;

	C3dCoord start = m_ps + startVec;
	C3dCoord end = m_pe + endVec;

	start.Z( m_ps.Z() );
	end.Z( m_pe.Z() );

	CGeoArc* offset = new CGeoArc( start, end, m_pc, m_dir );

	if (offset == NULL)
	{
		CReturn status;
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CGeoArc::Offset(#1)" );
	}
	else
	{
		if (AttribCount() > 0)
			*(offset->pAttrib()) = Attrib();
	}

	return offset;
}

void
CGeoArc::Reverse()
{
	C3dCoord tmp = m_ps;
	m_ps = m_pe;
	m_pe = tmp;

	m_dir = ((m_dir < 0) ? 1 : -1);
}

void
CGeoArc::Compliment()
{
	C3dCoord tmp = m_ps;
	m_ps = m_pe;
	m_pe = tmp;
}

double
CGeoArc::PointClosest( const C3dCoord& pt, C3dCoord* closestPt, double* u ) const
{
	double as, ae, ap;

	Angles( &as, &ae );
	ap = Angle( pt.X(), pt.Y() );

	Adjust( as, ae, &ap, u );

	int fullCircle = (fabs( TWOPI - fabs( ae - as ) ) < SMALL);

	if (u
		&& (fullCircle && fabs( 1.0 - (*u) ) < SMALL) )
		(*u) = 0.0;

	C2dUnitVec radVec( ap );

//	C3dCoord cpt = m_pc + (radVec * m_radius);
	C3dCoord cpt( m_pc.X() + cos(ap)*m_radius,
				  m_pc.Y() + sin(ap)*m_radius,
				  m_pc.Z() );

	if (closestPt)
		(*closestPt) = cpt;


//	C2dVec vec = cpt - pt;
//	return vec.Length();
	double dx = cpt.X() - pt.X();
	double dy = cpt.Y() - pt.Y();
	return sqrt( dx*dx + dy*dy );
}

double
CGeoArc::PointUparam( const C3dCoord& pt ) const
{
	double as, ae, ap, u;

	Angles( &as, &ae );
	ap = Angle( pt.X(), pt.Y() );

	Adjust( as, ae, &ap, &u );

	int fullCircle = (fabs( TWOPI - fabs( ae - as ) ) < SMALL);

	if ((fullCircle && fabs( 1.0 - u ) < SMALL) )
		return 0.0;

	return u;
}


// 2014.02.05 (PE) -- Overlapping parts in a nest were caused by failures in CGeoPoly::OR().
// Said method was calling CGeoArc::PointSide() with a tolerance of 5.e-5, a tolerance
// that was inappropriately large for the geometry under consideration. Edwin likely chose
// this solution, instead of calculating the closest point to the arc, because of its
// efficiency. That said, it's current implementation may continue to cause problems.
//
// Return -1 if the point is to the left, +1 to the right, 0 on the arc
int CGeoArc::PointSide( const C3dCoord& pt, double tol ) const
{
	// There are 2 cases that the arc PointSide algorithm considers:
	//
	//	1. For all points outside of the "wedge", the arc is approximated as 
	//		a line between the start and end points.  
	//		The point is tested against this line for sidedness.
	//
	//	2. For all points inside the arc's wedge, the point is on one side
	//		of the arc if it is inside the arc's radius, and on the other side
	//		if it is outside the radius (essentially, a circle containment test).
	//		The direction of the arc determines the left/right handedness.
	//
	// However, note that the analysis of this method was limited to QUADRANT
	// ARCS -- it should still be valid with all other arcs, but I guarantee only
	// quadrant arcs.
	//
	// Cloned relevant code from Line's PointClosest
	//
	// I apologize for the obfuscation, but the inline form is SIGNIFICANTLY faster

	double pdot = ((m_pe.X()-m_pc.X()) * (pt.Y()-m_pc.Y()) - (m_pe.Y()-m_pc.Y()) * (pt.X()-m_pc.X())) * m_dir;

	if (pdot < -tol)
	{
		// MIGHT be in wedge....
		pdot = ((m_ps.X()-m_pc.X()) * (pt.Y()-m_pc.Y()) - (m_ps.Y()-m_pc.Y()) * (pt.X()-m_pc.X())) * m_dir;
		if (pdot > tol)
		{
			//
			// Okay, in the wedge, so side is tested via circle containment
			//
			C2dVec distVec = pt - m_pc;
			double dist = distVec.Length();

			if (CLOSE( dist, m_radius, tol ))
			{ return 0; }
			
			return (dist>m_radius)?+m_dir:-m_dir;
		}
	}

	// NOT in the wedge, treat as a line
	// Off segment (as it were), so check side as-if a line
	C2dUnitVec ref = m_pe - m_ps;
	C2dUnitVec vec = pt - m_ps;
	pdot = ref.PerpDot( vec );
//	pdot = (m_pe.X()-m_ps.X()) * (pt.Y()-m_ps.Y()) - (m_pe.Y()-m_ps.Y()) * (pt.X()-m_ps.X());

	if (CLOSE(0.0, pdot, tol))
		return 0;

	return (pdot<0.0)?+1:-1;
}

// ASSUMPTION: The given point was derived from the intersection
// between this arc and another curve.  That is, the given point
// lies on the unbounded representation of this arc.
bool
CGeoArc::PointOnSeg( const C3dCoord& pt ) const
{
#if BEFORE
	double u;
	C3dCoord dummy;

	PointClosest( pt, &dummy, &u );

	return (u > -VECTOR_SMALL && u < (1.0 + VECTOR_SMALL));
#else
	// An inexpensive method: check that the point is on
	// the correct side of the vector between the arc end
	// points, relative to the arc direction.  Only when
	// the point is nearly coincident with the vector, do
	// we have to perform more expensive calculations.

	C2dUnitVec vecA = m_pe - m_ps;
	C2dUnitVec vecB = pt - m_ps;
	double cross = vecB ^ vecA;

	if ((cross <= -VECTOR_SMALL && m_dir < 0) ||
		(cross >=  VECTOR_SMALL && m_dir > 0))
			return TRUE;

	if ((cross >=  VECTOR_SMALL && m_dir < 0) ||
		(cross <= -VECTOR_SMALL && m_dir > 0))
			return FALSE;

	// The point is somewhere near an end point

	double u;
	C3dCoord dummy;

	PointClosest( pt, &dummy, &u );

	return (u > -VECTOR_SMALL && u < (1.0 + VECTOR_SMALL));
#endif
}

C3dCoord
CGeoArc::MidPt() const
{
	double as, ae;
	Angles( &as, &ae );

	double ai = 0.5 * (as + ae);
	C2dUnitVec radialVec( ai );

	double x = m_pc.X() + (m_radius * cos( ai ));
	double y = m_pc.Y() + (m_radius * sin( ai ));
	double z = m_pc.Z();
	return C3dCoord( x, y, z );
}

C3dCoord
CGeoArc::PointAtDist( double dist, bool fromStart ) const
{
	double as, ae;
	Angles( &as, &ae );

	// circumference = 2 * PI * R
	// delta = (dist / circumference) * (2 * PI)
	// reducing:
	//    delta = dist / R
	//
	double delta = (dist / m_radius) * m_dir;

	double ai = ((fromStart) ? (as + delta) : (ae + delta));

	return C3dCoord(
					(m_pc.X() + (cos(ai) * m_radius)),
					(m_pc.Y() + (sin(ai) * m_radius)),
					m_pc.Z() );
}

const C3dCoord&
CGeoArc::CenterPt() const
{
	return m_pc;
}

void
CGeoArc::CenterPt( const C3dCoord& pt )
{
	// TODO: Not at all happy with this!
	Init( m_ps, m_pe, pt, m_dir );
}

void
CGeoArc::CenterPt( double xc, double yc, double zc )
{
	// TODO: Not at all happy with this!
	C3dCoord pt( xc, yc, zc );
	CenterPt( pt );
}

double
CGeoArc::Radius() const
{
	return m_radius;
}

int
CGeoArc::Dir() const
{
	return m_dir;
}

void
CGeoArc::Dir( int dir )
{
	m_dir = ((dir < 0) ? -1 : 0);
}

void
CGeoArc::StartPt( const C3dCoord& pt )
{
	// TODO: Not at all happy with this!
	Init( pt, m_pe, m_pc, m_dir );
}

void
CGeoArc::StartPt( double xs, double ys, double zs )
{
	// TODO: Not at all happy with this!
	C3dCoord pt( xs, ys, m_ps.Z() );
	StartPt( pt );
}

void
CGeoArc::EndPt( const C3dCoord& pt )
{
	// TODO: Not at all happy with this!
	Init( m_ps, pt, m_pc, m_dir );
}

void
CGeoArc::EndPt( double xe, double ye, double ze )
{
	// TODO: Not at all happy with this!
	C3dCoord pt( xe, ye, m_pe.Z() );
	EndPt( pt );
}

// Result in radians where ( 0 <= result < TWOPI).
void
CGeoArc::Angles( double* startAngle, double* endAngle ) const
{
	const double TOL = 2.e-4;

	double as = Angle( m_ps.X(), m_ps.Y() );
	double ae = Angle( m_pe.X(), m_pe.Y() );

	if (m_dir < 0)
	{
		if (ae > as)
			as += TWOPI;
	}
	else
	{
		if (ae < as)
			ae += TWOPI;
	}

	if (fabs( ae-as ) < TOL)
	{
		// ASSUMPTION: zero-length arcs are not allowed.

		// Let 'a' represent fabs(ae-as).
		// Let 'd' represent the distance between the end points.
		// Let 'R' represent the arc radius.
		//
		// Then: (d/2)/R = sin(a/2)
		//
		// Solving for d: d = 2 * R * sin(a/2)
		//
		// Without this more rigorous test, as & ae will be roughly
		// identical.  In turn, midpoint calculations will be wrong,
		// and 'nearly full circles' appear to be 'invisible' in
		// the graphics (reported by Textron 1/8/2003).
		//
		double dist = 2 * m_radius * sin( 0.5 * fabs(ae-as) );
		if (dist < SMALL)
		{
			if (m_dir < 0)
				as += TWOPI;
			else
				ae += TWOPI;
		}
	}

	*startAngle = as;
	*endAngle = ae;
}

double
CGeoArc::IncludedAngle() const
{
	return IncludedAngle( m_ps.X(), m_ps.Y(), m_pe.X(), m_pe.Y() );
}

void
CGeoArc::Angles( double xs, double ys, double xe, double ye, double* startAngle, double* endAngle ) const
{
	double as = Angle( xs, ys );
	double ae = Angle( xe, ye );

	if (m_dir < 0)
	{
		if (ae > as)
			as += TWOPI;
	}
	else
	{
		if (ae < as)
			ae += TWOPI;
	}

	if (fabs( ae-as ) < SMALL)
	{
		// ASSUMPTION: zero-length arcs are not allowed.

		if (m_dir < 0)
			as += TWOPI;
		else
			ae += TWOPI;
	}

	*startAngle = as;
	*endAngle = ae;
}

double
CGeoArc::IncludedAngle( double xs, double ys, double xe, double ye ) const
{
	double as, ae;
	Angles( xs, ys, xe, ye, &as, &ae );
	return ( fabs( ae - as ) ); 
}

C2dUnitVec CGeoArc::TanAtPt( const C3dCoord& pt ) const
{
	return ( TanAtPt( pt.X(), pt.Y() ) );
}

C2dUnitVec CGeoArc::TanAtPt( double x, double y ) const
{
	double dx;
	double dy;

	// At first implementation, assume XY arc.
	if (m_dir < 0)
	{
		dx = y - m_pc.Y();
		dy = m_pc.X() - x;
	}
	else
	{
		dx = m_pc.Y() - y;
		dy = x - m_pc.X();
	}

	C2dUnitVec tan( dx, dy );

	return tan;
}

// Result in radians where ( 0 <= result < Const.TWOPI).
double
CGeoArc::Angle( double x, double y ) const
{
	double dx = x - m_pc.X();
	double dy = y - m_pc.Y();

// The sqrt is expensive... and not necessary for the angles
//	C2dUnitVec vec( dx, dy );
	C2dVec vec(dx, dy);

	return vec.Radians();
}

void
CGeoArc::Adjust( double as, double ae, double* ap, double* uParam ) const
{
	double ai = (*ap);

	// Adjust the angles to reflect any crossing of the 0/360 boundary.
	// NOTE: as & ae are always within 2PI of each other.  We therefore
	// attempt to move ai into the same range.
	//
	// Note:  If we hit the TWOPI case, treat it as over...
	// Otherwise, we can have a start of TWOPI to an end of 4.7 
	// (in dir -1) and an intermediate
	//	of 0.0... which gives a uparam of 4.0 instead of 0.0!!!
	// Found during poly boolean tests
	// 23 June 02 Edwin 
	if (m_dir < 0)
	{
		if ( (as >= (TWOPI-SMALL))
			&& (ai < as)
			&& (ai < ae) )  // ai < PI)
			ai += TWOPI;
	}
	else
	{
		if ( (ae >= (TWOPI-SMALL))
			&& (ai < ae)
			&& (ai < as) )  // ai < PI)
			ai += TWOPI;
	}

	double adiff0, adiff1;

	if (m_dir < 0)
	{
		adiff1 = as - ae;
		adiff0 = as - ai;
	}
	else
	{
		adiff1 = ae - as;
		adiff0 = ai - as;
	}

	ASSERT( (adiff1 >= SMALL) );  // Fails when zero-length arc.

	if (uParam)
		(*uParam)  = adiff0 / adiff1;
	(*ap) = ai;
}

void
CGeoArc::Xform( const C3x4Matrix& xform )
{
	xform.Transform( &m_ps );
	xform.Transform( &m_pe );
	xform.Transform( &m_pc );
	BoxUpdate();
}

void
CGeoArc::Shift( const C3dVec& delta )
{
	m_ps += delta;
	m_pe += delta;
	m_pc += delta;

	((C3dBox&)CGeoElem::Box()).Shift( delta );
//	BoxUpdate();
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Update the bounding box of the arc.
void
CGeoArc::BoxUpdate()
{
	//
	// Stretch the box for Start and End
	//
	C3dBox box;

	box.X( m_ps.X() );
	box.Y( m_ps.Y() );
	box.Z( m_ps.Z() );

	box.X( m_pe.X() );
	box.Y( m_pe.Y() );
	box.Z( m_pe.Z() );

	//
	// Now, do the four ordinal directions... if they lie on the arc.
	//
	double as, ae, ap;

	C3dCoord	arc_pt;
	double		arc_u;

	double xc = m_pc.X();
	double yc = m_pc.Y();

	Angles( &as, &ae );

	//
	// TODO:  drop into a loop?
	//
	arc_pt = C3dCoord( xc - m_radius, yc, 0.0 );
	ap = Angle( arc_pt.X(), arc_pt.Y() );
	Adjust( as, ae, &ap, &arc_u );
	if ( (arc_u >= 0.0)
		&& (arc_u <= 1.0) )
	{
		box.X( arc_pt.X() );
		box.Y( arc_pt.Y() );
	}

	arc_pt = C3dCoord( xc + m_radius, yc, 0.0 );
	ap = Angle( arc_pt.X(), arc_pt.Y() );
	Adjust( as, ae, &ap, &arc_u );
	if ( (arc_u >= 0.0)
		&& (arc_u <= 1.0) )
	{
		box.X( arc_pt.X() );
		box.Y( arc_pt.Y() );
	}

	arc_pt = C3dCoord( xc, yc - m_radius, 0.0 );
	ap = Angle( arc_pt.X(), arc_pt.Y() );
	Adjust( as, ae, &ap, &arc_u );
	if ( (arc_u >= 0.0)
		&& (arc_u <= 1.0) )
	{
		box.X( arc_pt.X() );
		box.Y( arc_pt.Y() );
	}

	arc_pt = C3dCoord( xc, yc + m_radius, 0.0 );
	ap = Angle( arc_pt.X(), arc_pt.Y() );
	Adjust( as, ae, &ap, &arc_u );
	if ( (arc_u >= 0.0)
		&& (arc_u <= 1.0) )
	{
		box.X( arc_pt.X() );
		box.Y( arc_pt.Y() );
	}

	// Finally, set our box
	CGeoElem::Box( box );
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Explode
//
//	Explodes the arc, to tolerance, into a list of points.
//	Each point is transformed according to the input xform.
//	The points are in a pointlist... which is created here,
//	and must be deleted by the calling process.
//
C3dCoordList*
CGeoArc::Explode( 
	double				in_tolerance, 
	const C3x4Matrix*	in_xform ) const			// This is *, so it can be NULL
{
	if ( EQUAL( m_radius, 0.0 ) || m_pc.WithinTol( m_ps, SMALL ) )
		return NULL;

	// Angle of step, based on chord height tolerance...
	double step_angle = HALFPI;	// A default, lame choice of 4 hits per circle
	if (m_radius > in_tolerance)
	{
		// Okay, our arc is bigger than our pixel, calculate the step
		step_angle = 2 * acos( (m_radius - in_tolerance) / m_radius );
	}

	// Start, end, and total angles...
	double	calc_angle;
	double	ea;
	Angles( &calc_angle, &ea );

	double	incl_angle = fabs( calc_angle - ea );

	// Okay, how many steps to cover the arc?
	// If step num is zero, we just hit the start and end; each step num above
	//	zero defines an intermediary point in the arc.
	int	step_num = (int)ceil( incl_angle / step_angle ) - 1;
	step_angle *= m_dir;


	// First, the start point!  Oh, and make a list.
	// TODO:  Memory allocation failure test.
	C3dCoordList* pt_list = new C3dCoordList;

	
	C3dCoord*	pt = new C3dCoord( m_ps );
	pt_list->Append( pt );

	// Now, the intermediary points...
	int			idx;
	for (idx=0; idx<step_num; idx++)
	{
		calc_angle += step_angle;

		pt = new C3dCoord(
							m_pc.X() + m_radius * cos( calc_angle ),
							m_pc.Y() + m_radius * sin( calc_angle ),
							m_pc.Z() );
		pt_list->Append( pt );
	}

	// Finally, the end point.
	pt = new C3dCoord( m_pe );
	pt_list->Append( pt );

	// Transform the list...
	step_num = pt_list->Count();
	if (in_xform)
	{
		for (idx=0; idx<step_num; idx++)
			in_xform->Transform( (*pt_list)[idx] );
	}

	return pt_list;
}

// Copied from Java/Source/Bbi/CodeGen/QuadIter.java
void
CGeoArc::QuadrantArcs( CGeoArcList* quadArcs )
{
	C3dCoord ps;
	C3dCoord pe;
	C2dUnitVec vs;
	C2dUnitVec ve;
	CGeoArc* arc = NULL;

	double as, ae, ai, adiff, delta;

	Angles( &as, &ae );

	// Shouldn't encounter zero-length arcs but ....
	adiff = ae - as;
	if (fabs(adiff) < SMALL)
		return;

	delta = ((adiff < 0) ? -HALFPI : HALFPI);

	// The intermediate angle.
	ai = ((int) (as / HALFPI)) * HALFPI;
	if (delta < 0 && fabs(ai-as) > SMALL)
		ai += HALFPI;

	ai += delta;
	while ( !QuadIterDone((int) delta, ai, ae) )
	{
		vs.Radians( as );
		ve.Radians( ai );

		ps = m_pc + (vs * m_radius);
		pe = m_pc + (ve * m_radius);

		if (!ps.WithinTolXY(pe, SMALL))
		{
			arc = new CGeoArc( ps, pe, m_pc, m_dir );
			quadArcs->Append( arc );
		}

		as = ai;
		ai += delta;
	}

	vs.Radians( as );
	ve.Radians( ae );

	ps = m_pc + (vs * m_radius);
	pe = m_pc + (ve * m_radius);

	if (!ps.WithinTolXY(pe, SMALL))
	{
		arc = new CGeoArc( ps, pe, m_pc, m_dir );
		quadArcs->Append( arc );
	}
}

bool
CGeoArc::QuadIterDone( int dir, double ai, double ae )
{
	if (dir < 0)
		return ((ae - ai) > -SMALL);
	else
		return ((ai - ae) > -SMALL);
}

void
CGeoArc::Tabulate(
	double			chordal_tol,
	const C3dVec&	shift,
	C3dCoordArray*	pts ) const
{
	C3dCoord pt;
	double as, ae, adiff;
	double theta, delta, residual;
	int count, indx;

	Angles( &as, &ae );

	// Shouldn't encounter zero-length arcs but ....
	adiff = ae - as;
	if (fabs(adiff) < SMALL)
		return;

	delta = 2 * acos( (m_radius - chordal_tol) / m_radius ) * SGN(adiff);

	count = (int) ( adiff / delta );
	residual = fabs(adiff) - (count * fabs(delta));
	if (residual > SMALL)
	{
		++count;
		delta = adiff / count;
	}

	pt.Z( m_pc.Z() + shift.Z() );

	for (indx = 0; indx <= count; ++indx)
	{
		theta = as + (indx * delta);

		pt.X( m_pc.X() + (m_radius * cos( theta )) + shift.X() );
		pt.Y( m_pc.Y() + (m_radius * sin( theta )) + shift.Y() );

		ConditionalAppend( pt, pts );
	}
}


// ==================================================================
//	ASSUMES quadrant arcs to limit the sdolution
//
double 
CGeoArc::InterceptX( 
	double at_y ) const
{
	double dy = m_pc.Y() - at_y;

	if (dy < (m_radius+SMALL))
	{
		double hyp = m_radius*m_radius - dy*dy;
		if (ZERO(hyp))
		{ return m_pc.X(); }

		double dx = sqrt( hyp );
		
		if ( m_pc.X() < ((m_ps.X() + m_pe.X())/2.0) )
			return m_pc.X() + dx;
		else
			return m_pc.X() - dx;
	}

	return UNDEFINED;
}

// ==================================================================
//	ASSUMES quadrant arcs to limit the sdolution
//
double 
CGeoArc::InterceptY(
	double at_x ) const
{
	double dx = m_pc.X() - at_x;

	if (dx <= (m_radius+SMALL))
	{
		double hyp = m_radius*m_radius- dx*dx ;
		if (ZERO(hyp))
		{ return m_pc.Y(); }

		double dy = sqrt( hyp );
		
		if ( m_pc.Y() < ((m_ps.Y() + m_pe.Y())/2.0) )
			return m_pc.Y() + dy;
		else
			return m_pc.Y() - dy;
	}

	return UNDEFINED;
}

// ==================================================================
//	ASSUMES quadrant arcs to limit the sdolution
//
double 
CGeoArc::InterceptXY(
	int	prim_ord,
	double at ) const
{
	int sec_ord = FLIP_XY(prim_ord);

	double d2 = m_pc[sec_ord] - at;
	if (d2 < (m_radius+SMALL))
	{
		double hyp = m_radius*m_radius - d2*d2;

		if (ZERO(hyp))
		{ return m_pc[prim_ord]; }

#if _DEBUG
		if (hyp < 0.)
			MessageBox(NULL, "CGeoArc::InterceptXY()", "foo", MB_OK );
#endif

		double d1 = sqrt( hyp );
		
		if ( m_pc[prim_ord] < ((m_ps[prim_ord] + m_pe[prim_ord])/2.0) )
			return m_pc[prim_ord] + d1;
		else
			return m_pc[prim_ord] - d1;
	}

	return UNDEFINED;
}



void
CGeoArc::Dump() const
{
	CReturn ret;
	CString str;
	str.Format( "A %f, %f to %f, %f c %f, %f", ROUND(m_ps.X(), 0.01), ROUND(m_ps.Y(), 0.01), 
												ROUND(m_pe.X(), 0.01), ROUND(m_pe.Y(), 0.01),
												ROUND(m_pc.X(), 0.01), ROUND(m_pc.Y(), 0.01) );
	ret.Diagnostic(str);
}
