// ==================================================================
//		aState
//
//	Base class that defines the State part of the A* search.  It 
//	represents the current position in state space.
//
//	Translated from Java, from "AI in Practice" by Edwin Wise,
//	McGraw-Hill, 2003.
// ==================================================================

#include <stdafx.h>
#include "aState.h"

// ==================================================================

CAState::CAState()
{
	m_prev = NULL;
}

CAState::~CAState()
{
}