#if !defined(_ASTAR_H)
#define _ASTAR_H
#pragma once

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

#include "aState.h"

// ==================================================================
//
//
class dllExport CAStar
{
public:
	CAStar(CAState* start);
	virtual ~CAStar();

	CAStateArray* Search();

private:
	void reset(void);

	int next_open(void);
	int closed_idx(CAState* state);
	int open_idx(CAState* state);

	CAStateArray* build_path(CAState* start);

private:
	CAStateArray* m_open_array;
	CAStateArray* m_closed_array;
};


#endif