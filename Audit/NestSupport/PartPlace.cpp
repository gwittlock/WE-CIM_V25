#include "stdafx.h"
#include "MathConst.h"
#include "PartPlace.h"

CPartPlace::CPartPlace()
	: m_bump_score( -UNDEFINED ),
	  m_overlap_score( -UNDEFINED ),
	  m_packing_score( UNDEFINED ),
	  m_toolhit( NULL )
{
}

CPartPlace::~CPartPlace()
{
}

void CPartPlace::Init(
	const C2dCoord&	world,
	const C2dCoord&	grid,
	CToolHit*		toolhit )  // Should really be const?
{
	m_world = world;
	m_grid = grid;
	m_toolhit = toolhit;
}

const CPartPlace& CPartPlace::operator = ( const CPartPlace& other )
{
	m_world = other.m_world;
	m_grid = other.m_grid;
	m_bump_score = other.m_bump_score;
	m_overlap_score = other.m_overlap_score;
	m_packing_score = other.m_packing_score;
	m_toolhit = other.m_toolhit;
	return (*this);
}

C2dCoord CPartPlace::WorldPointGet() const
	{ return m_world; }

C2dCoord CPartPlace::GridPointGet() const
	{ return m_grid; }

void CPartPlace::ScoresInit( double bump_score, double overlap_score )
{
	m_bump_score = bump_score;
	m_overlap_score = overlap_score;
}

double CPartPlace::BumpScoreGet() const
	{ return m_bump_score; }

void CPartPlace::BumpScoreSet( double bump_score )
{
	m_bump_score = bump_score;
}

double CPartPlace::OverlapScoreGet() const
	{ return m_overlap_score; }

void CPartPlace::OverlapScoreSet( double overlap_score )
{
	m_overlap_score = overlap_score;
}

double CPartPlace::PackingScoreGet() const
	{ return m_packing_score; }

void CPartPlace::PackingScoreSet( double packing_score )
{
	m_packing_score = packing_score;
}

// Returns (true) when this part place scores better than other.
// NOTE: This version of HasBetterScore() is subtlely different
// than the previous version in that, when comparing overlap
// scores, it uses the >= operator in place of the = operator
// because it assumes that subsequent iterations of part placement
// are better.
bool CPartPlace::HasBetterScore( const CPartPlace& other ) const
{
	bool	has_better_score = false;

	if (m_bump_score > SMALL)
	{
		if ( EQUAL( m_bump_score, other.m_bump_score ) )
		{
			if ( CNestConfig::IsBitmapNest() )  // DYNATORCH
				has_better_score = (m_overlap_score < other.m_overlap_score);
			else
				has_better_score = (m_overlap_score >= other.m_overlap_score);
		}
		else
		{
			has_better_score = (m_bump_score > other.m_bump_score);
		}
	}

	return has_better_score;
}

bool CPartPlace::HasSameScore( const CPartPlace& other ) const
{
	double	delta = m_bump_score - other.m_bump_score;
	return (fabs(delta) < 1.e-6);  // arbitrary tolerance.
}

CToolHit* CPartPlace::ToolHitGet() const
	{  return m_toolhit; }

// m_toolhit should NEVER be NULL.  If it is NULL
// then something is wrong on the client-side.
CNestingPart* CPartPlace::PartGet() const
	{ return ( m_toolhit->Part() ); }
