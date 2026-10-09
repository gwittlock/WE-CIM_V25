// ==================================================================
//		aStar
//
//	aStar path search.  Searches for the most efficient path from 
//	one aState to another aState, passing through legal aStates
//	in between.
//
//	Translated from Java, from "AI in Practice" by Edwin Wise,
//	McGraw-Hill, 2003.
// ==================================================================

#include <stdafx.h>

#include "Common.h"
#include "aStar.h"

// ==================================================================
//
// This is a one-shot class.  Re-create it for a second path.
//
CAStar::CAStar(
	CAState* start)	// I take control of the start state...
{
	m_open_array = new CAStateArray();
	m_closed_array = new CAStateArray();

	m_open_array->Append(start);
}

CAStar::~CAStar()
{
	reset();
}

void
CAStar::reset()
{
	if (m_open_array)
	{
		m_open_array->DestructiveFlush();
		delete m_open_array;
		m_open_array = NULL;
	}
	if (m_closed_array)
	{
		m_closed_array->DestructiveFlush();
		delete m_closed_array;
		m_closed_array = NULL;
	}
}

// ==================================================================

CAStateArray* 
CAStar::Search()
{
	CAState* active;
	int active_idx;

	while (m_open_array->Count())
	{
		active_idx = next_open();
		active = (*m_open_array)[active_idx];

		if (active->Done())
		{ return build_path(active); }
		//
		// Get list of all legal neighboring states from the
		// current active state.  Note that these are allocated
		// and all control over them reverts to us.
		//
		CAStateArray n_array;
		active->Neighbors(&n_array);
		int n_num = n_array.Count();
		for (int n_idx=0; n_idx<n_num; n_idx++)
		{
			CAState* next = n_array[n_idx];
			//
			// Is this state already represented on the closed list?
			int c_idx = closed_idx(next);
			if (c_idx >= 0)
			{
				CAState* closed = (*m_closed_array)[c_idx];
				if (next->Cost() < closed->Cost())
				{
					// Okay, this state is cheaper than the version on the
					// closed list, so pop it from that list and use it.
					m_closed_array->Remove(c_idx);
				}
				else
				{ 
					// Oops, not any better so we can't use it.
					delete next;
					next = NULL;
				}
			}
			else // Not in closed list
			{
				// Is it on the open list already?
				int o_idx = open_idx(next);
				if (o_idx >= 0)
				{
					// Same question; is the one in the open least cheaper
					// than this one or not?
					CAState* open = (*m_open_array)[o_idx];
					if (next->Cost() < open->Cost())
					{
						m_open_array->Remove(o_idx);
					}
					else
					{
						delete next;
						next = NULL; 
					}
				}
			}
			//
			//
			if (next)
			{
				next->Previous(active);
				m_open_array->Append(next); 
			}
		}
		m_closed_array->Append(active);
		m_open_array->Remove(active_idx);

		n_array.BenignFlush();
	}

	return NULL;
}

// ==================================================================

int 
CAStar::next_open(void)
{
	int best_state = -1;
	double best_cost = DBL_MAX;

	int num = m_open_array->Count();
	for (int idx=0; idx<num; idx++)
	{
		CAState* state = (*m_open_array)[idx];
		double cost = state->Cost();
		if (cost < best_cost)
		{
			best_cost = cost;
			best_state = idx; 
		}
	}

	return best_state;
}

// ==================================================================

int
CAStar::closed_idx(CAState* state)
{
	int num = m_closed_array->Count();
	for (int idx=0; idx<num; idx++)
	{
		CAState* test = (*m_closed_array)[idx];


		if (state->Equals(test))
		{ return idx; }
	}

	return -1;
}

// ==================================================================

int
CAStar::open_idx(CAState* state)
{
	int num = m_open_array->Count();
	for (int idx=0; idx<num; idx++)
	{
		CAState* test = (*m_open_array)[idx];


		if (state->Equals(test))
		{ return idx; }
	}

	return -1;
}

// ==================================================================

CAStateArray* 
CAStar::build_path(CAState* state)
{
	CAStateArray* result = new CAStateArray();

	while (state)
	{
		int idx = closed_idx(state);
		if (idx >= 0)
		{ m_closed_array->Remove(idx); }
		else
		{
			int idx = open_idx(state);
			if (idx >= 0)
			{ m_open_array->Remove(idx); }
		}

		result->Prepend(state);
		state = state->Previous();
	}

	reset();

	return result;
}


