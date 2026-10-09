#include <string>
#include <vector>

#include <stdafx.h>
#include <math.h>

#include "cmn_resource.h"
#include "Path.h"
#include "Grid.h"
#include "3dBox.h"
#include "Solution.h"
#include "MathConst.h"

#include "PolyScan.h"

// ==================================================================

CGrid::CGrid()
{
	m_grid = NULL;
	m_width = 0;
	m_height = 0;
}

CGrid::~CGrid()
{
	if (m_grid) { delete[] m_grid; m_grid = NULL; }
}


// ==================================================================
//	Allocate the grid, and record certain information about it
CReturn CGrid::Create( 
	const C2dBox&	in_extent,		// Part extent in TOP
	const C2dCoord&	in_center,		// standardized part handle in TOP
	double			in_resolution,
	double			in_angle )
{
	CReturn ret = Create( in_extent, in_center, in_resolution );

	return ret;
}

CReturn CGrid::Create( 
	const C2dBox&	in_extent,		// Part extent in TOP
	const C2dCoord&	in_center,		// standardized part handle in TOP
	double			in_resolution )
{
	CReturn		ret;

	if (m_grid)
		delete m_grid;
	
	m_grid = NULL;

	m_origin = C2dCoord( in_extent.Xmin(), in_extent.Ymin() );
	m_center = in_center;

	m_width = (int)ceil( in_extent.Dx() / in_resolution ) + 1;
	m_height = (int)ceil( in_extent.Dy() / in_resolution ) + 1;
	m_size = m_width * m_height;

	m_resolution = in_resolution;

	// 2008.04.20 (PE) -- Crash during bitmap nesting.
	try
	{
		m_grid = new int[m_size];
	}
	catch (...)
	{
	}

	if (m_grid == NULL)
	{
		ret.Fatal( IDS_MEM_ALLOC_FAILURE, "Resolution is too fine." );
	}
	else
	{
		memset( m_grid, 0, (m_size * sizeof(int)) );
	}

	return ret;
}

// ==================================================================
//	Slice and Dice a single profile into this grid.
//
//	16 Nov 99 -- Changed to standard rasterization scheme, though it may reduce accuracy, it is more robust
//
//	See:
//
// 2007.12.30 (PE) -- Appears to be used only when preparing a remnant
// via CSheet::MaterialProcess().
CReturn CGrid::Slice( const CDbProfile& in_prof, int fill_mark )
{
	CReturn			ret;
	C3dCoordList	ptarray;
	CPolyScan		poly;

	//
	// Convert the profile into a series of (scaled) endpoints... exploding arcs
	//
	int ent_num = in_prof.Count();
	for (int ent_idx=0; ent_idx<ent_num; ent_idx++)
	{
		CDbEntity* db_ent = in_prof[ent_idx];
		CDbCurve* db_curve = dynamic_cast<CDbCurve*>(db_ent);
		if (!db_curve)
			continue;

		// Note we are working in local, presumably TOP.
		CGeoCurve* curve = db_curve->Curve();

		do_slice( *curve, &ptarray );

		delete curve;
	}

	if (ent_num)
	{
		poly.Scan( ptarray, m_grid, m_width, m_height, fill_mark );

		ptarray.DestructiveFlush();
	}

	return ret;
}

CReturn CGrid::Slice( const CProfile& in_prof )
{
	CReturn			ret;
	C3dCoordList	ptarray;
	CPolyScan		poly;

	//
	// Convert the profile into a series of (scaled) endpoints... exploding arcs
	//
	int ent_num = in_prof.Count();
	for (int ent_idx=0; ent_idx<ent_num; ent_idx++)
	{
		CGeoCurve* curve = in_prof.GetAt(ent_idx);
		if (!curve)
			continue;

		do_slice( *curve, &ptarray );
	}

	poly.Scan( ptarray, m_grid, m_width, m_height, MARK_FILL_OKERF );

	ptarray.DestructiveFlush();

	return ret;
}

// Where fill_mark is in (MARK_FILL_OKERF .. MARK_FILL_HOLE)
CReturn CGrid::Slice( const CGeoPoly& in_poly, int fill_mark )
{
	C3dCoordList	ptarray;
	CPolyScan		poly;
	CReturn			status;

	// Convert the profile into a series of (scaled) endpoints... exploding arcs
	int ent_num = in_poly.Count();
	for (int ent_idx=0; ent_idx<ent_num; ent_idx++)
	{
		do_slice( in_poly[ent_idx], &ptarray );
	}

	if (fill_mark < 0)
		poly.Transfer( ptarray, m_grid, m_width, m_height, -fill_mark );
	else
		poly.Scan( ptarray, m_grid, m_width, m_height, fill_mark );

	ptarray.DestructiveFlush();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: The following comments are relevant only when
//   CNestConfig::IsBitmapNest() returns true.
//
// DYNATORCH -- (-) MARK_NONE / (*) MARK_BODY / (+) MARK_SPACE
//
// The first implementation created a part grid like this.
// This did not help wrt. properly spacing parts on the sheet.
//
// ----------------
// --************--
// -*------------*-
// -*------------*-
// -*------------*-
// --************--
// ----------------
//
// Now we create a part grid like this.  When this part grid is
// transfered to the sheet, any part cell marked with MARK_SPACE
// is marked as MARK_BODY in the corresponding sheet cell.
//
// -++++++++++++++-
// ++************++
// +*++++++++++++*+
// +*+----------+*+
// +*++++++++++++*+
// ++************+-
// -++++++++++++++-
//
// After BodyFill(), the part grid looks like this.
//
// -++++++++++++++-
// ++************++
// +**************+
// +**************+
// +**************+
// ++************+-
// -++++++++++++++-
//
// After being punched into the sheet.
//
// -**************-
// ****************
// ****************
// ****************
// ****************
// ****************
// -**************-
//
CReturn CGrid::Slice( CGeoElem* geo_ent, double spacing )
{
	C3dCoordList	ptarray;
	CPolyScan		poly;
	CReturn			ret;

	// Convert the entity into a series of (scaled) endpoints... exploding arcs
	CGeoCurve* curve = dynamic_cast<CGeoCurve*>(geo_ent);
	if (!curve)
		return ret;

	do_slice( *curve, &ptarray );

	if ( CNestConfig::IsBitmapNest() )
	{
		C3dCoord*	pt;
		int			count, indx;

		count = ptarray.Count();
		for (indx = 0; indx < count; ++indx)
		{
			pt = ptarray[indx];

			CellMark( (*pt), spacing );
		}
	}
	else
	{
		poly.Scan( ptarray, m_grid, m_width, m_height, MARK_FILL_OKERF );
	}

	ptarray.DestructiveFlush();

	return ret;
}

void CGrid::CellMark( const C3dCoord& pt, double spacing )
{
	int	col, row;
	int	count, indx, jndx;

	count = ((int) ceil( spacing / m_resolution ));

	col = ((int) ceil( pt.X() - 0.5 ));
	row = ((int) ceil( pt.Y() - 0.5 ));

	for (indx = -count; indx <= count; ++indx)
	{
		for (jndx = -count; jndx <= count; ++jndx)
		{
			ConditionalCellMark( (row+indx), (col+jndx), MARK_SPACE );
		}
	}

	ConditionalCellMark( row, col, MARK_FILL_OKERF );
}

void CGrid::ConditionalCellMark( int row, int col, int mark )
{
	int indx = (m_width * row) + col;
	if ((indx < 0) || (indx >= m_size))
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CGrid::ConditionalCellMark(#1)" );
	}
	else
	{
		if (mark == MARK_SPACE)
		{
			if (m_grid[indx] == MARK_NONE)
				m_grid[indx] = mark;
		}
		else
		{
			m_grid[indx] = mark;
		}
	}
}

void CGrid::do_slice( const CGeoCurve& curve, C3dCoordList* ptarray )
{
	C3dCoord*	pt;
	double		arclen;
	int			count, indx;

	// TODO: Speed up by tabulating directly in grid-space?
	arclen = curve.Length2d();

	// DYNATORCH -- Factor of 2 hack alert!  BodyFill() was failing because
	// there were gaps in the edge of the part (due to poor tabulation?)
	count = (int) (arclen / (0.5 * m_resolution));
	if (count == 0)
		count = 1;  // we must get back at least one point for small entities

	if ((arclen - (m_resolution * count)) > 1.e-3)  // arbitrary tolerance
		++count;

	switch (curve.Type())
	{
		case GEOLINE:
		{
			C2dUnitVec	vec;
			C3dCoord	ps;
			C3dCoord*	pt;
			double		delta;

			ps = curve.StartPt();
			vec = curve.StartTan();
			delta = arclen / count;

			for (indx = 0; indx <= count; ++indx)
			{
				pt = new C3dCoord;

				pt->X( ps.X() + ((delta * indx) * vec.X()) );
				pt->Y( ps.Y() + ((delta * indx) * vec.Y()) );
				pt->Z( ps.Z() );

				pt->X( (pt->X() - m_origin.X()) / m_resolution );
				pt->Y( (pt->Y() - m_origin.Y()) / m_resolution );

				ptarray->Append( pt );
			}
		}
		break;

		case GEOARC:
		{
			C3dCoord	pc;
			double		rad;
			double		as, ae, ai, da;

			const CGeoArc& garc = (CGeoArc&) curve;

			pc = garc.CenterPt();
			rad = garc.Radius();

			garc.Angles( &as, &ae );
			da = (ae - as) / count;

			for (indx = 0; indx <= count; ++indx)
			{
				pt = new C3dCoord;

				ai = as + (da * indx);

				pt->X( pc.X() + (rad * cos(ai)) );
				pt->Y( pc.Y() + (rad * sin(ai)) );
				pt->Z( pc.Z() );

				pt->X( (pt->X() - m_origin.X()) / m_resolution );
				pt->Y( (pt->Y() - m_origin.Y()) / m_resolution );

				ptarray->Append( pt );
			}
		}
		break;
	}
}


// ==================================================================
//	Transfer Marks into Profile Bodies / Holes
//
// Fill Sequence, increments the marker at edges:
//		x10 = TOOL
//		x20 = BODY
//		x30 = BUMPER (TOOL+BODY)
//		x40 = HOLE
//
CReturn CGrid::Fill( const CNestConfig& in_config, int fill_mark )
{
	CReturn ret;
	int		at_x;
	int		at_y;
	int*	col;

	for (at_y = 0; at_y < m_height; ++at_y)
	{
		col = m_grid + (at_y * m_width);
		for (at_x = 0; at_x < m_width; ++at_x)
		{
			if ((*col) & fill_mark)
			{
				switch (fill_mark)
				{
				case MARK_FILL_OKERF:
				case MARK_FILL_IKERF:
					(*col) |= MARK_TOOL;
					break;

				case MARK_FILL_BODY:
					// NOTE: Nesting seems to hang without this next statement.
					(*col) &= ~MARK_TOOL;  // NECESSARY?
					// if ( !((*col) & MARK_TOOL) )
						(*col) |= MARK_BODY;
					break;

				case MARK_FILL_HOLE:
					// NOTE: Nesting seems to hang without this next statemnt.
					(*col) &= ~MARK_TOOL;  // NECESSARY?
					// if ( !((*col) & MARK_TOOL) )
						(*col) |= MARK_HOLE;
					break;
				}
			}

			(*col) &= ~fill_mark;

			++col;
		}
	}

	return ret;
}

// ==================================================================
//	Do any final post-processing to tidy the grid after repeated
//	Fill() calls.
//
//		(X + TOOL+BODY) => (X + TOOL)
CReturn CGrid::Clean()
{
	CReturn ret;
	int		at_x;
	int		at_y;
	int*	col;

	for (at_y=0; at_y<m_height; at_y++)
	{
		col = m_grid + at_y * m_width;

		for (at_x=0; at_x<m_width; at_x++)
		{
			if ((*col & MARK_BUMPER) == MARK_BUMPER)
			{ *col &= ~MARK_BODY; }

			col++;
		}
	}

	// This may not be quite right. Perhaps We need to scan from the edges
	// of the grid until we encounter either MARK_BODY or MARK_OKERF (?)
	for (at_y = 0; at_y < m_height; ++at_y)
	{
		col = m_grid + (at_y * m_width);

		// Scan from leftmost to right.
		at_x = 0;
		while (1)
		{
			if ( !((*col) & MARK_BODY) )
				break;

			(*col) &= ~MARK_BODY;

			++at_x;
			if (at_x >= m_width)
				break;

			++col;
		}

		// Scan from rightmost to left.
		at_x = m_width - 1;
		col = m_grid + (at_y * m_width) + at_x;
		while (1)
		{
			if ( !((*col) & MARK_BODY) )
				break;

			(*col) &= ~MARK_BODY;

			--at_x;
			if (at_x < 0)
				break;

			--col;
		}
	}

	return ret;
}


// =======================================================================
//	Set a border of the grid to MARK_BORDER
CReturn CGrid::Border( int in_width )
{
	CReturn	ret;

	if (!in_width)
		return ret;

	// Top and Bottom strips
	{
		int*	top = m_grid;
		int*	btm = m_grid + (m_height-in_width)*m_width;

		for (int at_y=0; at_y<in_width; at_y++)
		{
			for (int at_x=0; at_x<m_width; at_x++)
			{
				*top++ |= MARK_BORDER;
				*btm++ |= MARK_BORDER;
			}
		}
	}

	// Left and Right strips
	{
		int*	left = m_grid;
		int*	right = m_grid + m_width-1;

		for (int at_y=0; at_y<m_height; at_y++)
		{
			if (at_y)
			{
				left += m_width;
				right += m_width;
			}
			for (int at_x=0; at_x<in_width; at_x++)
			{
				*(left+at_x) |= MARK_BORDER;
				*(right-at_x) |= MARK_BORDER;
			}
		}
	}

	return ret;
}

// ==================================================================
//	Convert the filled grid from Part form to Remnant form
//	(Inverts the filled/unfilled areas)
//
//	NOT SYMETRICAL -- can only be run once
//
CReturn CGrid::Invert( void )
{
	CReturn ret;
	int		at_x;
	int		at_y;
	int*	col;

	for (at_y=0; at_y<m_height; at_y++)
	{
		col = m_grid + at_y * m_width;
		for (at_x=0; at_x<m_width; at_x++)
		{
			if (*col == 0)
				*col = MARK_BORDER;
			else if (*col == MARK_BORDER)
				*col = 0;
			else
				*col = MARK_BODY;

			col++;
		}
	}
	return ret;
}


// =======================================================================
//	Scan in a box of pixels to the grid; inclusive of fenceposts of box
void CGrid::MarkBox( 
	const C2dBox&	box, 
	int			pattern,	// Bit-pattern
	bool			draw )		// TRUE draw, FALSE erase
{
	int*	row;
	int*	col;

	for (int at_y=(int)floor(box.Ymin()); at_y<=(int)ceil(box.Ymax()); at_y++)
	{
		if ( (at_y < 0)
			|| (at_y >= m_height))
			continue;

		row = RowAddress(at_y);

		for (int at_x=(int)floor(box.Xmin()); at_x<(int)ceil(box.Xmax()); at_x++)
		{
			if ( (at_x < 0)
				|| (at_x >= m_width) )
				continue;

			col = row+at_x;

			if (draw)
				*col |= pattern;
			else
				*col &= ~pattern;
		}
	}
}

void CGrid::MarkDisk( 
	const C2dCoord& ctr, 
	int				dia, 
	int			pattern, 
	bool			draw )
{
	int radius = dia / 2;
	double radius2 = radius*radius;

	int from_y = ((int)ctr.Y() - radius);
	int to_y = ((int)ctr.Y() + radius);

	for (int at_y=from_y; at_y<=to_y; at_y++)
	{
		if ( (at_y < 0)
			|| (at_y >= m_height))
			continue;

		int* row = RowAddress(at_y);

		double rise = at_y - ctr.Y();
		double run = sqrt( radius - rise*rise );

		int from_x = (int)(ctr.X() - run + 0.5);
		int to_x = (int)(ctr.X() + run + 0.5);

		for (int at_x=from_x; at_x<=to_x; at_x++)
		{
			if ( (at_x < 0)	|| (at_x >= m_width) )
				continue;

			int* col = row+at_x;

			if (draw)
				*col |= pattern;
			else
				*col &= ~pattern;
		}
	}
}

// =======================================================================
//	Verify that at least thresh pixel(s) of the grid has the attribute
//	specified by flag.
//
bool CGrid::Verify( int flag, int thresh )
{
	double ratio = (double)thresh / 100.0;

	int count = 0;
	int total = 0;
	for (int at_y=0; at_y<m_height; at_y++)
	{
		int* row = m_grid + at_y * m_width;
		for (int at_x=0; at_x<m_width; at_x++)
		{
			int* col = row + at_x;

			if (*col)
			{
				total++;

#if BEFORE_2007_11_24
				if (*col & flag)
#else
				// Changed to check for pure-body because a simply
				// doing a bitwise and includes cells that we may
				// not want to consider. The idea here is to force
				// the resolution to be fine enough that the body
				// is visibly distinct.
				if ((*col) == flag)
#endif
				{ 
					count++;
				}
			}
		}
	}

#if BEFORE_2012_02_18
	return ((double)count / (double)total) > ratio;
#else
	// The prior implementation used a ratio. That solution
	// caused problems when the offsets were large compared
	// to the size of the part (because the count of body
	// pixels was small compared to the total number of pixels.
	return (count >= thresh);
#endif
}


// =======================================================================
void CGrid::Debug( const CString& in_file ) const
{
	static bool enabled = false;

	if (enabled)
	{
		std::vector<std::string> symbolic( m_height, "" );

		std::string buf;
		for (int at_y = 0; at_y < m_height; ++at_y)
		{
			int* row = m_grid + (at_y * m_width);
			
			buf.clear();
			for (int at_x = 0; at_x < m_width; ++at_x)
			{
				int* col = row + at_x;

				buf += GridSymbolGet( *col );
			}

			buf += '\n';
			symbolic.push_back( buf );
		}

		if (0)
		{
			CString debug_path = CPath::DebugDir() + "\\" + in_file;

			HANDLE file = CreateFile(	debug_path,				// name
								GENERIC_WRITE,			// access mode
								FILE_SHARE_READ,		// share mode
								NULL,					// security descriptor
								CREATE_ALWAYS,			// how to create
								FILE_ATTRIBUTE_NORMAL,	// file attributes
								NULL );					// handle of file to match
			if (file == INVALID_HANDLE_VALUE)
				return;

			DWORD did_write;
			for ( size_t indx = 0; indx < symbolic.size(); ++indx )
			{
				std::string str = symbolic[indx];
				WriteFile( file, (LPCVOID) str.c_str(), (int) str.length(), &did_write, NULL );
			}
		
			CloseHandle( file );
		}
	}
}

// =======================================================================
//	Bit-Blit this grid to the DC
void CGrid::Draw( 
	CViewMgr&	in_view,
	int			in_x,		// Origin (tl) X, Y
	int			in_y,
	bool		invert,
	bool		erase ) const
{
	CWnd*		wnd;
	CDC*		dest;
	COLORREF	color;
	int			cnt_x;
	int			cnt_y;
	int			at_x;
	int			at_y;
	const int*	bit;

	wnd = in_view.getCWnd();
	if (wnd == NULL)
		return;

	dest = wnd->GetDC();

	bit = m_grid;
	at_y = in_y;
	for (cnt_y=0; cnt_y<m_height; cnt_y++)
	{
		at_x = in_x;
		for (cnt_x=0; cnt_x<m_width; cnt_x++)
		{
			if (*bit & MARK_HOLE)
				color = 0xff0000;
			else
			if (*bit & MARK_DEAD)
				color = 0x808080;
			else
			if (*bit & MARK_TOOL)
				color = 0xffff00;
			else
			if (*bit & MARK_BODY)
				color = 0x00ff00;
			else
			if (*bit & MARK_CLAMP)
				color = 0x00ffff;
			else
			if (*bit & MARK_BORDER)
				color = 0x0000ff;
			else
				color = 0x000000;

			if (color || erase)
			{
				if (invert)
					color = color ^ dest->GetPixel( at_x, at_y );

				dest->SetPixelV( at_x, at_y, color );
			}

			bit++;
			at_x++;
		}
		at_y++;
	}
}

void CGrid::BodyFill( const C2dCoord& interior_pt )
{
	// For the 'before' picture.
	Debug("_floodfill.txt");

	if ( !interior_pt.IsDefined() )
	{
		// Punt ...
		CheezyBodyFill();
	}
	else
	{
		FloodFillPrep();

		// For the 'after' picture.
		Debug("_floodfill.txt");

		// Map the world point to grid space.
		int x = (int) ((interior_pt.X() - m_origin.X()) / m_resolution);
		int y = (int) ((interior_pt.Y() - m_origin.Y()) / m_resolution);

		QueueFloodFill( x, y );
	}

	// For the 'after' picture.
	Debug("_floodfill.txt");
}

void CGrid::QueueFloodFill( int x, int y )
{
	CSize*	pt;

	if (0)
	{
		// This option bleeds across diagonals where
		// boundary lines are only one pixel thick!

		//start the loop
		QueueFloodFill8( x, y );

		//call next item on queue
		while (m_queue.Count() > 0)
		{
			pt = m_queue.Remove(0);

			QueueFloodFill8( pt->cx, pt->cy );

			delete pt;
		}
	}
	else
	{
		//start the loop
		QueueFloodFill4( x, y );

		//call next item on queue
		while (m_queue.Count() > 0)
		{
			pt = m_queue.Remove(0);

			QueueFloodFill4( pt->cx, pt->cy );

			delete pt;
		}
	}

	// Just in case I did an early exit in the debugger.
	m_queue.DestructiveFlush();
}

// NOTE: Could implement as single loop over [0..(m_width*m_height)].
void CGrid::FloodFillPrep()
{
	for (int indx = 0; indx < m_size; ++indx)
	{
		if (m_grid[indx] == MARK_FILL_OKERF)
			m_grid[indx] = MARK_BODY;
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// From http://www.codeproject.com/cs/media/floodfillincsharp.asp
// NOTE: There appears to be three basic solutions 1) scanline,
// 2) queue-based and 3) recursive.
//
// Recursive solutions are (by far) the fastest but they are also
// the most problematic because (for any 'real' problem) you can
// easily run out of stack space.
//
// Nobody seems real keen on scanline solutions.
//
// Queue solutions run slower, but they are also safer.
//
// NOTE: There are also differences between 4-way and 8-way solutions.
//
// NOTE: Sensitive to seed point.
void CGrid::QueueFloodFill8( int x, int y )
{
	if ((x < 0) || (x >= m_width))
		return;

	if ((y < 0) || (y >= m_height))
		return;

	int indx = (y * m_width) + x;
	if (m_grid[indx] != MARK_BODY)
	{
		m_grid[indx] = MARK_BODY;

		m_queue.Append( new CSize(x+1,y) );
		m_queue.Append( new CSize(x,y+1) );
		m_queue.Append( new CSize(x-1,y) );
		m_queue.Append( new CSize(x,y-1) );

		m_queue.Append( new CSize(x+1,y+1) );
		m_queue.Append( new CSize(x-1,y-1) );
		m_queue.Append( new CSize(x-1,y+1) );
		m_queue.Append( new CSize(x+1,y-1) );
	}
}

void CGrid::QueueFloodFill4( int x, int y )
{
	if ((x < 0) || (x >= m_width))
		return;

	if ((y < 0) || (y >= m_height))
		return;

	int indx = (y * m_width) + x;

	if (m_grid[indx] != MARK_BODY)
	{
		m_grid[indx] = MARK_BODY;

		m_queue.Append( new CSize(x+1,y) );
		m_queue.Append( new CSize(x,y+1) );
		m_queue.Append( new CSize(x-1,y) );
		m_queue.Append( new CSize(x,y-1) );
	}
}

void CGrid::CheezyBodyFill()
{
	for (int row = 0; row < m_height; ++row)
	{
		RowFill( row );
	}
}

void CGrid::RowFill( int row )
{
	int*	markers;
	int		mark;
	int		colA, colB, colC;

	colA = 0;

	markers = m_grid + (row * m_width);

	while (1)
	{
		if (colA >= m_width)
			break;  // done

		mark = markers[colA];

		// Find the column of the next mark that
		// differs from the current mark.
		colB = RowScan( markers, colA, mark );

		if ((mark != MARK_NONE) && (colB < m_width))
		{
			// ASSUMPTION: colB represents a MARK_NONE byte.
			colC = RowScan( markers, colB, MARK_NONE );
			if (colC < m_width)
				colB = colC;
		}

		// Mark the bytes between the columns.
		while (colA < colB)
		{
			// Unmarked bytes remain unmarked.
			if (mark != MARK_NONE)
				markers[colA] = MARK_BODY;

			++colA;
		}
	}
}

int CGrid::RowScan( int* markers, int colA, int mark )
{
	int	colB;

	colB = colA + 1;
	while (1)
	{
		if (colB >= m_width)
			break;  // exhausted bytes in this row.

		if (markers[colB] != mark)
			break;  // found a mismatch.

		++colB;
	}

	return colB;
}

char GridSymbolGet( int mark )
{
	char symbol = '.';

	switch (mark)
	{
	case MARK_SPACE:  symbol = '%';  break;
	case MARK_BODY:   symbol = '*';  break;
	case MARK_HOLE:   symbol = '^';  break;
	case MARK_TOOL:   symbol = 'o';  break;
	case MARK_BORDER: symbol = 'B';  break;
	case MARK_CLAMP:  symbol = 'C';  break;
	case MARK_DEAD:   symbol = 'X';  break;
	}

	return symbol;
}