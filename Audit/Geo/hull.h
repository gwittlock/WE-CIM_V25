#ifndef _HULL_H
#define _HULL_H

#include "stdafx.h"

typedef double VECT2[2];

#if 0
extern "C"
{
	int Convex_Hull_2D( int Nb, const VECT2* Pts, int* Ind );
}
#else
//	extern int Convex_Hull_2D( int Nb, const VECT2* Pts, int* Ind );
	int dllExport Convex_Hull_2D( int Nb, const VECT2* Pts, int* Ind );
#endif
#endif
