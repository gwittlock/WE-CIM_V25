#if !defined(_ASTATE_H)
#define _ASTATE_H
#pragma once

// ==================================================================
//		aState
//
//	Base class that defines the State part of the A* search.  It 
//	represents the current position in state space.
//
//	Translated from Java, from "AI in Practice" by Edwin Wise,
//	McGraw-Hill, 2003.
// ==================================================================

#include "Common.h"
#include "DynamicArray.h"

// ==================================================================
//
class CAState;
typedef CDynamicArray<CAState*> CAStateArray;

// ------------------------------------------------

class dllExport CAState
{
public:
	CAState();
	virtual ~CAState();

	CAState* Previous() const		{ return m_prev; }
	void Previous(CAState* state)	{ m_prev = state; }

	virtual void Neighbors(CAStateArray* array) = 0;
	virtual double Cost() = 0;
	virtual bool Done() = 0;
	virtual bool Equals(CAState* state) = 0;

private:
	CAState* m_prev;
};

#endif