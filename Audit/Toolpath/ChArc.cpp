
#include "stdafx.h"
#include <math.h>
#include "MathConst.h"
#include "GeoArc.h"
#include "Worm.h"
#include "ChArc.h"


////////////////////////////////////////////////////////////////////////

CChArc::CChArc(
		const C2dCoord& center,
		const C2dUnitVec& startTan,
		const C2dUnitVec& endTan,
		double radius,
		int dir )

	: m_pc( center ),
	  m_ts( startTan ),
	  m_te( endTan ),
	  m_rad( radius ),
	  m_dir( dir )
{
}

CChArc::~CChArc()
{
}

bool
CChArc::IsTurn() const
{
	return (m_dir == CHARC_TURN || m_dir == -CHARC_TURN);
}

bool
CChArc::IsMove() const
{
	return (m_dir == CHARC_MOVE || m_dir == -CHARC_MOVE);
}

double
CChArc::Radius() const
{
	return m_rad;
}

int
CChArc::Dir() const
{
	return m_dir;
}

void
CChArc::Dir( int dir )
{
	m_dir = dir;
}

C2dCoord
CChArc::CenterPt() const
{
	return m_pc;
}

C2dCoord
CChArc::StartPt() const
{
	C2dUnitVec vec( m_ts.X(), m_ts.Y() );
	vec += (-CWorm::Sign( m_dir ) * HALFPI);
	C2dCoord ps = m_pc + (vec * m_rad);
	return ps;
}

C2dCoord
CChArc::EndPt() const
{
	C2dUnitVec vec( m_te.X(), m_te.Y() );
	vec += (-CWorm::Sign( m_dir ) * HALFPI);
	C2dCoord pe = m_pc + (vec * m_rad);
	return pe;
}

C2dUnitVec
CChArc::StartTan() const
{
	return m_ts;
}

C2dUnitVec
CChArc::EndTan() const
{
	return m_te;
}

// Based on CGeoArc::QuadrantArcs()
void
CChArc::QuadrantArcs( CChArcList* quadArcs ) const
{
	double dot = m_ts * m_te;
	if (dot >= (1.0 - VECTOR_SMALL) && m_rad < SMALL)
		return;  // Nothing to do (because we have a line?)

	C2dUnitVec ts;
	C2dUnitVec te;
	CChArc* arc = NULL;

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
		ts.Radians( as + (m_dir * HALFPI) );
		te.Radians( ai + (m_dir * HALFPI) );

		arc = new CChArc( m_pc, ts, te, m_rad, m_dir );
		quadArcs->Append( arc );

		as = ai;
		ai += delta;
	}

	ts.Radians( as + (m_dir * HALFPI) );
	te.Radians( ae + (m_dir * HALFPI) );

	arc = new CChArc( m_pc, ts, te, m_rad, m_dir );
	quadArcs->Append( arc );
}

bool
CChArc::QuadIterDone( int dir, double ai, double ae ) const
{
	if (dir < 0)
		return ((ae - ai) > -SMALL);
	else
		return ((ai - ae) > -SMALL);
}

// Obtain the radial angles of the arc.
// Result in radians where ( 0 <= result < TWOPI).
void
CChArc::Angles( double* startAngle, double* endAngle ) const
{
	double as = m_ts.Radians() - (m_dir * HALFPI);
	double ae = m_te.Radians() - (m_dir * HALFPI);

	if (m_dir < 0)
	{
		if (ae > as)
			as += TWOPI;
	}
	else
	{
		if (as < 0.0)
		{
			as += TWOPI;
			ae += TWOPI;
		}

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

bool
CChArc::SameDirection( const CChArc* partArc, const CChArc* toolArc )
{
	if (toolArc->Dir() == 0)
		return TRUE;

	return (CWorm::Sign( toolArc->Dir() ) == CWorm::Sign( partArc->Dir() ));
}

#if BEFORE
int
CChArc::OverlapClassify( const CChArc* partArc, const CChArc* toolArc, int offsetDir )
{
	// Get the radial vectors for the CCW tool arc.
	C2dUnitVec ts( toolArc->StartTan().Y(), -toolArc->StartTan().X() );
	C2dUnitVec te( toolArc->EndTan().Y(), -toolArc->EndTan().X() );

	if ((CWorm::Sign( offsetDir ) == LEFT && partArc->Dir() == CW) ||
		(CWorm::Sign( offsetDir ) == RIGHT && partArc->Dir() == CCW))
	{
		ts.Radians( ts.Radians() + PI );
		te.Radians( te.Radians() + PI );
	}

	// Get the radial vectors for the part arc.
	int partArcDir = CWorm::Sign( partArc->Dir() );
	C2dUnitVec ps( partArc->StartTan().Radians() - (partArcDir * HALFPI) );
	C2dUnitVec pe( partArc->EndTan().Radians() - (partArcDir * HALFPI) );

	if ((ps * ts) < -VECTOR_SMALL)
		return 0;

	if ((ps * te) < -VECTOR_SMALL)
		return 0;

	if ((pe * ts) < -VECTOR_SMALL)
		return 0;

	if ((pe * te) < -VECTOR_SMALL)
		return 0;

	// Initialize the result.
	int classification = 0;

	// Determine the range of overlap in the vectors.
	double psXts = ps ^ ts;
	double psXte = ps ^ te;
	double peXts = pe ^ ts;
	double peXte = pe ^ te;

	int test;
	
	test = CWorm::Sign( psXts ) + CWorm::Sign( psXte );
	if (test > -2 && test < 2)
	{
		classification = (classification | PS_IN_TS_TE);
	}
	else
	{
		double tsXps = ts ^ ps;
		double tsXpe = ts ^ pe;

		test = CWorm::Sign( tsXps ) + CWorm::Sign( tsXpe );
		if (test > -2 && test < 2)
		{
			classification = (classification | TS_IN_PS_PE);
		}
	}

	test = CWorm::Sign( peXts ) + CWorm::Sign( peXte );
	if (test > -2 && test < 2)
	{
		classification = (classification | PE_IN_TS_TE);
	}
	else
	{
		double teXps = te ^ ps;
		double teXpe = te ^ pe;

		test = CWorm::Sign( teXps ) + CWorm::Sign( teXpe );
		if (test > -2 && test < 2)
		{
			classification = (classification | TE_IN_PS_PE);
		}
	}

	return classification;
}
#else
int
CChArc::OverlapClassify( const CChArc* partArc, const CChArc* toolArc, int offsetDir )
{
	int classification = CWorm::ArcOverlapClassify(
									partArc->StartTan(),
									partArc->EndTan(),
									partArc->Dir(),
									toolArc->StartTan(),
									toolArc->EndTan(),
									toolArc->Dir(),
									offsetDir );

	return classification;
}
#endif