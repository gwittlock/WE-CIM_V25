// ==================================================================
//		PolyScan
//
//		"Concave Polygon Scan Conversion", Paul Heckbert
//		Graphics Gems, p.87 and p.681
//
//	Originally crappy C code with globals and opaque variable names.
//	God I hate that stuff.
//	I tried to make it nicer... some
//
// ==================================================================

#include "stdafx.h"
#include "PolyScan.h"

#include "return.h"

#include <math.h>
#include "grid.h"

// ==================================================================
// Why don't they make a stinking rotten qsort w/ objects?
//
C3dCoord** CPolyScan::m_vert;  // *pt = points themselves (reflected from parameters)
int CPolyScan::m_vert_count;

// ==================================================================

CPolyScan::CPolyScan()
{
	m_active = NULL;
}

CPolyScan::~CPolyScan()
{
	delete [] m_active;
}


// ==================================================================
void 
CPolyScan::Scan( 
	C3dCoordList&	clist,
	int*			grid,
	int				width,
	int				height,
	int				mark )
{
	int	vindx;

	m_vert_count = clist.Count();
	if (!m_vert_count)
		return;

	// 2008.08.23 (PE) -- With a small resolution, the number of points
	// in clist can be very large, making the application appear to hang
	// in qsort() because m_vert was (previously) a CIndxList (which has
	// very slow random access). Now, we convert clist to an array.
	m_vert = new C3dCoord*[m_vert_count];
	int* idx_array = new int[m_vert_count];
	for (vindx = 0; vindx < m_vert_count; ++vindx)
	{
		m_vert[vindx] = clist[vindx];
		idx_array[vindx] = vindx;
	}


	// 1. Sort points (in m_vert) by Y ordinate
	//		Sort only an index list into the array...
	qsort( idx_array, m_vert_count, sizeof(idx_array[0]), compare_index );

	// 2. Process each scanline, generating active Edges
	m_active = new sEdge[m_vert_count];
	m_act_num = 0;

	int	min_y = (int)ceil( m_vert[idx_array[0]]->Y() - .5 );
	int max_y = (int)floor( m_vert[idx_array[m_vert_count-1]]->Y() + .5 );

	int vert_idx = 0;
	for (int at_y=min_y; at_y<=max_y; at_y++)
	{
		// Check points between previous scanline and this one
		for ( ; vert_idx<m_vert_count && m_vert[idx_array[vert_idx]]->Y() <= at_y+0.5 
			  ; vert_idx++ )
		{
			int idx = idx_array[vert_idx];
			int prev_idx = (idx > 0) ? idx-1 : m_vert_count-1;

			if (m_vert[prev_idx]->Y() <= at_y - 0.5)
				delete_edge( prev_idx );
			else
			if (m_vert[prev_idx]->Y() > at_y + 0.5)
				insert_edge( prev_idx, at_y );

			int next_idx = ((idx+1)<m_vert_count) ? idx+1 : 0;
			if (m_vert[next_idx]->Y() <= at_y - 0.5)
				delete_edge( idx );
			else
			if (m_vert[next_idx]->Y() > at_y + 0.5)
				insert_edge( idx, at_y );
		}

		// 3. Sort edges by X
		qsort( m_active, m_act_num, sizeof(m_active[0]), compare_edge );

		// FOR SOME REASON, an odd vertex with crap data is intruding... kill it
		//
		if (m_act_num & 1)
			m_act_num--;

		// 4. Draw spans between edges
		for (int act_idx=0; act_idx<m_act_num; act_idx+=2)
		{
			sEdge*	active = &m_active[act_idx];
			sEdge*	next = &m_active[act_idx+1];

			int min_x = (int)ceil( active->x - 0.5 );
			min_x = max(0, min_x);

			int max_x = (int)floor( next->x + 0.5 );
			max_x = min(width-1, max_x);

			if (min_x <= max_x)
			{
				if ((at_y >= 0) && (at_y <= height))
				{
					int* row = grid + width * at_y;

//note.Format( "   y %d scans %d to %d as %x", at_y, min_x, max_x, mark );
//ret.Diagnostic( note );
					for (int at_x=min_x; at_x<=max_x; at_x++)
						row[at_x] |= mark;
				}

				active->x += active->dx;
				next->x += next->dx;
			}
		}
	}

	// 5. Clean up.
	delete [] idx_array;
	
	// Null-out the entries in m_vert. Otherwise, the array dtor
	// will destroy the data (which is not ours).
	memset( m_vert, 0, (m_vert_count * sizeof(C3dCoord*)) );
	delete [] m_vert;
	m_vert = NULL;
}


// ==================================================================
void 
CPolyScan::delete_edge( int idx )
{
	int at = 0;
	for (; at<m_act_num && m_active[at].num != idx; at++)
	{}

	m_act_num--;

	if (at >= m_act_num) return;

//	for (; at<=m_act_num; at++)  NO!  This runs off the end...
	for (; at<m_act_num; at++)
	{
		m_active[at] = m_active[at+1];
	}
}


// ==================================================================
void 
CPolyScan::insert_edge( int idx, int at_y )
{
	int	next = ((idx+1) < m_vert_count) ? idx+1 : 0;

	C3dCoord*	low;	// p
	C3dCoord*	high;	// q

	if ( m_vert[idx]->Y() < m_vert[next]->Y() )
	{
		low = m_vert[idx];
		high = m_vert[next];
	}
	else
	{
		low = m_vert[next];
		high = m_vert[idx];
	}

	double dx = (high->X() - low->X()) / (high->Y() - low->Y() );
	m_active[m_act_num].dx = dx;
	m_active[m_act_num].x = dx * (at_y + 0.5 - low->Y()) + low->X();
	m_active[m_act_num].num = idx;
	m_act_num++;
}

// ==================================================================
C3dCoord*	
CPolyScan::Vert( int idx )
{
	return m_vert[idx];
}

// ==================================================================
int	
CPolyScan::compare_index( const void* u, const void* v )
{
	return CPolyScan::Vert(*((int*)u))->Y() <= CPolyScan::Vert(*((int*)v))->Y() ? -1 : 1;
}


// ==================================================================
int	
CPolyScan::compare_edge( const void* u, const void* v )
{
	return ((sEdge*)u)->x <= ((sEdge*)v)->x ? -1 : 1;
}

void 
CPolyScan::Transfer( 
	C3dCoordList&	clist,
	int*			grid,
	int				width,
	int				height,
	int				mark )
{
	CReturn	status;
	int		count, indx;

	count = clist.Count();
	for (indx = 0; indx < count; ++indx)
	{
		C3dCoord* pt = clist[indx];
		if (pt == NULL)
			EWMFile( FILE_INFO );

		int x = (int) ceil( pt->X() );
		if (x < 0)
			x = 0;  // kludge

		int y = (int) floor( pt->Y() );
		if (y < 0)
			y = 0;  // kludge to fix crash

		int* row = grid + (y * width);

		// 2007.11.24 (PE) -- OR'd the mark because CDexGrid::TestCore()
		// was failing to return body/body intersection of cells for parts
		// that had narrow strips of material (eg. a 10in square having a
		// 9.5in concentric square hole).
		//    row[x] = mark;
		row[x] |= mark;
	}
}

