#if !defined(_POLYSCAN_H)
#define _POLYSCAN_H

// ==================================================================
//		PolyScan
//
//		"Concave Polygon Scan Conversion", Paul Heckbert
//		Graphics Gems, p.87 and p.681
//
//
//	TODO:  Merge with CProfile???
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _3DCOORD_H
#include "3dCoord.h"
#endif

// ==================================================================
//		Edge structure
//
typedef struct
{
	double		x;		// X coordinage of intersection with scanline
	double		dx;		// Change in X wrt Y
	int			num;	// Edge number
} sEdge;


// ==================================================================

class CPolyScan
{
public:

	CPolyScan();

	virtual ~CPolyScan();

	void Scan( C3dCoordList& clist, int* grid, int width, int height, int mark );

	void Transfer( C3dCoordList& clist, int* grid, int width, int height, int mark );

private:

	static int compare_index( const void* u, const void* v );
	static int compare_edge( const void* u, const void* v );
	static C3dCoord* Vert( int idx );

private:  // Disabled.

	CPolyScan( const CPolyScan& );
	const CPolyScan& operator = ( const CPolyScan& );
	int operator == ( const CPolyScan& ) const;
	int operator != ( const CPolyScan& ) const;

	void delete_edge( int idx );
	void insert_edge( int idx, int at_y );

private:

	// Source Vertex Array
	// m_vert is static because (in reality) there is only one
	// Scan() running at any given time.
	static C3dCoord**	m_vert;
	static int			m_vert_count;

private:

	// Active Edge List
	int		m_act_num;			// nact = number of active edges
	sEdge*	m_active;			// *active = fixed array of edges
};

#endif

