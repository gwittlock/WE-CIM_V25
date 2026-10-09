// ==================================================================
//		Seed
//
// ==================================================================

#include <stdafx.h>
#include "MathConst.h"

#include "Seed.h"

// ==================================================================

C2dBox CSeed::m_tmp_box;

// ==================================================================

CSeed::CSeed()
{
	m_priority = -IUNDEFINED;
	m_gravity = IUNDEFINED;
	m_touched = false;
	m_prelink = false;
	m_toolhit = NULL;
}

CSeed::CSeed(
		int				priority,
		const C2dCoord&	part_place,
		CToolHit*		toolhit,
		int				gravity )
{
	m_priority = priority;
	m_pt = part_place;
	m_gravity = gravity;
	m_touched = false;
	m_prelink = false;
	m_toolhit = toolhit;
}
	
CSeed::CSeed( const CSeed& seed )
{
	(*this) = seed;
}

CSeed::~CSeed( void )
{
}

// ---------------------------------------------

CSeed& CSeed::operator = ( const CSeed& seed )
{
	m_priority = seed.m_priority;
	m_pt = seed.m_pt;
	m_toolhit = seed.m_toolhit;
	m_gravity = seed.m_gravity;

	m_touched = seed.m_touched;
	m_prelink = seed.m_prelink;

	return (*this);
}

// ==================================================================
//	Circular range of influence
bool CSeed::Overlaps( const CSeed& seed, double range )
{
	m_tmp_box.Update(
		m_pt.X() - range,
		m_pt.Y() - range,
		m_pt.X() + range,
		m_pt.Y() + range );

	if ( m_tmp_box.Contains( seed.Placement(), 0. ) )
	{
		C2dVec vec = m_pt - seed.Placement();
		if (vec.Length() < range)
			return true;
	}
	return false;
}
