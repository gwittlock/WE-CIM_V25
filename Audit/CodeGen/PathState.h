#if !defined(_PATHSTATE_H)
#define _PATHSTATE_H
#pragma once

// ==================================================================
//		PathState
//
//	sState form used for path avoidance during code generation
//
//	Translated from Java, from "AI in Practice" by Edwin Wise,
//	McGraw-Hill, 2003.
// ==================================================================

#include "Common.h"
#include "2dBox.h"
#include "3dCoord.h"

#include "aState.h"

// ==================================================================
//
class dllExport CPathState : public CAState
{
public:
	CPathState( C2dBoxArray* obstacle_array,
			C3dCoord& start_pt,
			C3dCoord& end_pt );
	CPathState(C3dCoord& pos, double cost);
	CPathState(C3dCoord& pos, C2dBox& mer, double cost);

	virtual ~CPathState();

	void Neighbors(CAStateArray* array);

	double Cost();

	bool Done();

	bool Equals(CAState* state);

	C3dCoord& Pos()			{ return m_pos; }

private:
	void add_neighbor(CAStateArray* array, C3dCoord& pt, C2dBox* box);
	void add_neighbor(CAStateArray* array, C3dCoord& pt );

private:
	C3dCoord	m_pos;
	C2dBox		m_box;
	bool		m_internal;
	double		m_cost;


	static C2dBoxArray*	g_obstacle_array;
	static C3dCoord		g_start;
	static C3dCoord		g_dest;
};

#endif