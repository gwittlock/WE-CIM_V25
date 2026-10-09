// ==================================================================
//		PathState
//
//	sState form used for path avoidance during code generation
//
//	Translated from Java, from "AI in Practice" by Edwin Wise,
//	McGraw-Hill, 2003.
// ==================================================================

#include <stdafx.h>
#include "GeoLine.h"

#include "PathState.h"

// ==================================================================

C2dBoxArray*	CPathState::g_obstacle_array;
C3dCoord		CPathState::g_start;
C3dCoord		CPathState::g_dest;

// ==================================================================
//
CPathState::CPathState( C2dBoxArray* obstacle_array,
			C3dCoord& start_pt,
			C3dCoord& end_pt )
{
	g_start = start_pt;
	g_dest = end_pt;
	g_obstacle_array = obstacle_array; // Does not own

	m_pos = start_pt;
	m_cost = 0.0;
	m_internal = false;
}	

CPathState::CPathState( C3dCoord& pos, double cost )
{
	m_pos = pos;
	m_cost = cost;
	m_internal = false;
}

CPathState::CPathState( C3dCoord& pos, C2dBox& box, double cost )
{
	m_pos = pos;
	m_cost = cost;
	m_box = box;
	m_internal = true;
}

CPathState::~CPathState()
{
}

// ==================================================================

void 
CPathState::Neighbors(CAStateArray* array)
{
	if (m_internal)
	{
		// Force the path to the adjacent corners of this box
		//
		double x = -(m_pos.X() - (m_box.Xmin()+m_box.Xmax()));
		double y = -(m_pos.Y() - (m_box.Ymin()+m_box.Ymax()));

		add_neighbor( array, C3dCoord(m_pos.X(), y, 0.0) );
		add_neighbor( array, C3dCoord(x, m_pos.Y(), 0.0) );
	}

	int num = g_obstacle_array->Count();
	for (int idx=0; idx<num; idx++)
	{
		C2dBox* box = (*g_obstacle_array)[idx];

		add_neighbor( array, C3dCoord(box->Xmin(), box->Ymin(), 0.0), box );
		add_neighbor( array, C3dCoord(box->Xmin(), box->Ymax(), 0.0), box );
		add_neighbor( array, C3dCoord(box->Xmax(), box->Ymin(), 0.0), box );
		add_neighbor( array, C3dCoord(box->Xmax(), box->Ymax(), 0.0), box );
	}
	add_neighbor( array, C3dCoord(g_dest), NULL ); // NULL box to force the mode
}

// ==================================================================

double 
CPathState::Cost()
{
	return m_cost + m_pos.Dist(g_dest);
}

// ==================================================================

bool
CPathState::Done()
{
	return m_pos.WithinTolXY(g_dest, SMALL);
}

// ==================================================================

bool 
CPathState::Equals(CAState* state)
{
	return m_pos.WithinTolXY(((CPathState*)state)->Pos(), SMALL);
}

// ==================================================================

void 
CPathState::add_neighbor(
	CAStateArray* array,
	C3dCoord& pt,
	C2dBox* box)
{
	if (m_pos.WithinTolXY(pt, SMALL))
	{ return; }

	CGeoLine path(m_pos, pt);

	int num = g_obstacle_array->Count();
	for (int idx=0; idx<num; idx++)
	{
		C2dBox* obst = (*g_obstacle_array)[idx];

		if (obst->Intersects(path, SMALL))
		{ return; }
	}

	CPathState* state;
	if (box)
	{ state = new CPathState(pt, *box, m_cost); } 
	else
	{ state = new CPathState(pt, m_cost); } 
	array->Append(state);
}


void 
CPathState::add_neighbor(
	CAStateArray* array,
	C3dCoord& pt )
{
	if (m_pos.WithinTolXY(pt, SMALL))
	{ return; }

	CPathState* state = new CPathState(pt, m_cost);
	array->Append(state);
}
