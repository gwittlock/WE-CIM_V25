// ==================================================================
//		SeedList
//
// ==================================================================

#include <stdafx.h>
#include "common.h"
#include "return.h"
#include "mathconst.h"
#include "Sheet.h"

#include "SeedList.h"

CSeedList::CSeedList( int size )
{ 
	m_size = size; 
	m_list = new CSeed[m_size];
	m_head = m_tail = 0;

	m_curr = 0;
}

CSeedList::~CSeedList( void )
{ 
	if (m_list)
		delete[] m_list;
}

void
CSeedList::Reset( void )
{
	m_head = 0;
	m_tail = 0;
	m_curr = 0;
}

#if REQUIRED
bool
CSeedList::Prepend( const CSeed& seed )
{
	int indx;

	if (m_tail >= m_size)
		return false;  // we've run out of space

	indx = m_tail;
	while (indx > 0)
	{
		m_list[indx] = m_list[indx-1];
		--indx;
	}

	m_list[0] = seed;
	++m_tail;

	return true;
}
#endif

bool
CSeedList::Append( const CSeed& seed )
{
	int indx, next = (m_tail + 1) % m_size;
	if (next == m_head)
		return false;

	// 2006.05.14 (PE) -- It is much cheaper to prevent addition
	// of redundant seeds than it is to score the part multiple
	// times at the same location.
	const C2dCoord& ptB = seed.Placement();
	for (indx = 0; indx < next; ++indx)
	{
		const C2dCoord& ptA = m_list[indx].Placement();
		if ( ptA.WithinTol( ptB, 1.e-4 ) )  // arbitrary tolerance
			break;
	}

	if (indx >= next)
	{
		m_list[m_tail] = seed;
		m_tail = next;
	}

	return true;
}

const CSeed& CSeedList::PopTail( void )
{
	m_tail = (m_tail - 1) % m_size;
	return m_list[m_tail];
}


int compare_leftup( const void* u, const void* v )
{
	CSeed* s1 = (CSeed*)u;
	CSeed* s2 = (CSeed*)v;

//	if (s1->Priority() > s2->Priority())
//	{ return 1; }

	if ( EQUAL(s1->X(), s2->X()) )
	{ if (s1->Y() > s2->Y()) return 1; }

	if (s1->X() < s2->X()) return 1;

	return -1;
}

int	compare_leftdn( const void* u, const void* v )
{
	CSeed* s1 = (CSeed*)u;
	CSeed* s2 = (CSeed*)v;

//	if (s1->Priority() > s2->Priority())
//	{ return 1; }

	if ( EQUAL(s1->X(), s2->X()) )
	{ if (s1->Y() < s2->Y()) return 1; }

	if (s1->X() < s2->X()) return 1;

	return -1;
}


int compare_rightdn( const void* u, const void* v )
{
	CSeed* s1 = (CSeed*)u;
	CSeed* s2 = (CSeed*)v;

//	if (s1->Priority() > s2->Priority())
//	{ return 1; }

	if ( EQUAL(s1->Y(), s2->Y()) )
	{ if (s1->X() > s2->X()) return 1; }

//	if (s1->Y() < s2->Y()) return 1;
	if (s1->Y() > s2->Y()) return 1;

	return -1;
}

int	compare_rightup( const void* u, const void* v )
{
	CSeed* s1 = (CSeed*)u;
	CSeed* s2 = (CSeed*)v;

//	if (s1->Priority() > s2->Priority())
//	{ return 1; }

	if ( EQUAL(s1->Y(), s2->Y()) )
	{ if (s1->X() > s2->X()) return 1; }

	if (s1->Y() > s2->Y()) return 1;

	return -1;
}

int	PriorityCompare( const void* u, const void* v )
{
	CSeed* s1 = (CSeed*)u;
	CSeed* s2 = (CSeed*)v;

	return (s1->Priority() - s2->Priority());
}

int	GridScoreCompare( const void* u, const void* v )
{
	CSeed* s1 = (CSeed*)u;
	CSeed* s2 = (CSeed*)v;

	return (s1->GridScore() - s2->GridScore());
}

bool CSeedList::Qsort( eNestProgression progression, const CSheet& sheet )
{
	C2dCoord	pt;
	int			score, indx;

	if (Count() > 0)
	{
		// We assign a 'grid score' to each seed.
		// The grid score is used to order the seeds such
		// that they are aligned in columns/rows (row
		// major or column major being determined by the
		// progression direction).
		for (indx = 0; indx < m_tail; ++indx)
		{
			pt = m_list[indx].Placement();
			score = sheet.GridScore( progression, pt );
			m_list[indx].GridScore( score );
		}

		// We sort the seeds based on priority and the
		// we find the boundary index between priorities.
		qsort( (void*)&(m_list[0]), m_tail - m_head, sizeof(CSeed), PriorityCompare );

		for (indx = 1; indx < m_tail; ++ indx)
		{
			if (m_list[indx].Priority() > m_list[indx-1].Priority())
				break;
		}

		// Finally, we sort each priority grouping based upon
		// the grid score of the seeds in each grouping.
		qsort( (void*)&(m_list[0]), indx, sizeof(CSeed), GridScoreCompare );
		qsort( (void*)&(m_list[indx]), m_tail - indx, sizeof(CSeed), GridScoreCompare );
	}

	return true;
}

// TODO: ASSUMES that we NEVER wrap tail around the end... THIS MAY BE IN ERROR SOMEDAY
// But for now, with only PopTail() calls, we always go from zero to m_tail
void	
CSeedList::Reduce( double range )
{
	if (CReturn::Debug()>=5)
	{
		CString note;
		CReturn ret;
		note = "============================================";
		ret.Diagnostic( note );
		Dump();
		note = "---- REDUCE --------------------------------";
		ret.Diagnostic( note );
	}

	int good = 0;
	CSeed* good_seed = &(m_list[good]);

	int scan = 1;
	CSeed* scan_seed;

	int test;
	CSeed* test_seed;

	bool kill;
	while (scan < m_tail)
	{
		scan_seed = &(m_list[scan]);

		kill = false;
		double tol = (scan_seed->Priority() == 1) ? range : SMALL;

		for (test=0; test<=good; test++)
		{
			test_seed = &(m_list[test]);
			if (test_seed->Overlaps(*scan_seed, tol))
			{
				kill = true;
				break;
			}
		}

		if (!kill)
		{
			good++;
			good_seed = &(m_list[good]);

			if (good != scan)
			 (*good_seed) = (*scan_seed);
		}
		scan++;
	}
	m_tail = good+1;

	if (CReturn::Debug()>=5)
	{
		CString note;
		CReturn ret;

		Dump();
		note = "============================================";
		ret.Diagnostic( note );
	}
}

// Ugh. It is simplest to do the reduction using two
// passes since the list is implmented as an array.
void
CSeedList::ReduceToPriority( int priority_mask )
{
	int	count, indx;

	// Mark the seeds whose priorities are incompatible,
	for (indx = 0; indx < m_tail; ++indx)
	{
		if ( !(m_list[indx].Priority() & priority_mask) )
			m_list[indx].Priority( -1 );
	}

	// Eliminate the marked seeds by shuffling data.
	count = 0;
	for (indx = 0; indx < m_tail; ++indx)
	{
		if (m_list[indx].Priority() >= 0)
		{
			if (indx > count)
			{
				m_list[count] = m_list[indx];
				++count;
			}
		}
	}

#if BEFORE_V19
	m_tail = ((count > 0) ? (count + 1) : 0);
#else
	m_tail = ((count > 0) ? count : 0);
#endif
}

void	
CSeedList::Shift( C2dVec delta )
{
	C2dCoord	part_place;

	int idx=m_head;
	while (idx != m_tail)
	{
		CSeed& seed = m_list[idx];
		idx = (idx+1)%m_size;

		if ( DEFINED(seed.X()) && DEFINED(seed.Y()) )
		{
			part_place = seed.Placement() + delta;

			seed = CSeed( seed.Priority(), part_place, seed.ToolHit(), seed.Gravity() );
		}
	}
}


void
CSeedList::Dump()
{
	CString note;
	CReturn ret;

	note.Format( "(head %d, tail %d)", m_head, m_tail );
	ret.Diagnostic(note);

	int idx=m_head;
	while (idx != m_tail)
	{
		CSeed& seed = m_list[idx];
		idx = (idx+1)%m_size;

		if ( DEFINED(seed.X())
			&& DEFINED(seed.Y()) )
		{
			note.Format( "%d: (%d) %f, %f", idx, seed.Priority(), seed.X(), seed.Y() );
			ret.Diagnostic( note );
		}
	}
}


bool
CSeedList::AtEnd() const
{
	return (m_curr >= m_tail);
}

// DO NOT CALL unless AtEnd() returns false!!!!
const CSeed&
CSeedList::Next()
{
	int	indx = m_curr;
	++m_curr;
	return ( m_list[indx] );
}

void
CSeedList::MyReset()
{
	m_curr = 0;
}
