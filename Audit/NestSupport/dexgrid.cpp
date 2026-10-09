// ==================================================================
//		DexGrid
//
// ==================================================================

#include <stdafx.h>
#include "common.h"

#include "Path.h"
#include "DexGrid.h"
#include "NestingPart.h"  // for COMMON_PASS

// ==================================================================

CDexGrid::CDexGrid( const CGrid& grid )
	: m_iterator( -1, -1 ),
	  m_itdex(nullptr),
	  m_itrow(nullptr)
{
	// Construct a pair of dex-grids from the regular grid
	//
	m_width = grid.Width();
	m_height = grid.Height();

	m_origin = grid.Origin();
	m_center = grid.Center();

	m_res = grid.Resolution();

	scan_horz( grid );
}

CDexGrid::~CDexGrid( void )
{
	int num = m_dex_list.Count();
	for (int idx=0; idx<num; idx++)
	{
		CDex* dex = m_dex_list[idx];
		while (dex)
		{
			CDex* next = dex->Next();
			delete dex;
			dex = next;
		}
	}
	m_dex_list.BenignFlush();
}


// ==================================================================
void
CDexGrid::scan_horz( const CGrid& grid )
{
	CDex*	dex;
	int*	row;
	int		x, y;

	// NOTE: grid is a one-dimensional array of bytes
	// representing a 2 X 2 grid of grid-space points.
	for (y = 0; y < m_height; ++y)
	{
		row = grid.RowAddress( y );

		// Start the row with the first grid point
		dex = new CDex();

		dex->Type( *(row++) );
		m_dex_list.Append( dex );

		for (x = 1; x < m_width; ++x)
		{
			if (*row != dex->Type())
			{
				// In a new span type...
				dex->Last( x-1 );
				dex->Append( new CDex() );

				dex = dex->Next();
				dex->Type( *row );
				dex->First( x );
			}
			row++;
		}
		dex->Last( m_width-1 );
	}

	Debug( CString("scan_horz.txt") );
}


// =======================================================================
//		Cell
//
//	Return the dex contents at the named point.
//	Uses m_iterator, m_itdex, and m_itrow for efficiency in scanning.
//
int	
CDexGrid::Cell( C2dCoord& pos )
{
	int y = (int)pos.Y();
	int x = (int)pos.X();

	if ( !InRangeX( x ) || !InRangeY( y ) )
		return MARK_BORDER;

	try
	{
		if (!EQUAL(pos.Y(), m_iterator.Y()) || !m_itdex)
		{
			m_itrow = m_dex_list[(int)pos.Y()];
			m_itdex = m_itrow;
		}
		else if (pos.X() < m_itdex->First())
			m_itdex = m_itrow;

		while (pos.X() > m_itdex->Last())
		{
			m_itdex = m_itdex->Next();
			if (!m_itdex)
				return MARK_BORDER;
		}

		m_iterator = pos;
		return m_itdex->Type();
	}
	catch (...)
	{
		return MARK_BORDER;
	}
}


// =======================================================================
//		Draw
//
//	Bit-Blit this grid to the DC
//
void
CDexGrid::Draw( 
	CViewMgr&	in_view,
	int			in_x,		// Origin (tl) X, Y
	int			in_y,
	bool		invert,
	bool		erase ) const
{
	CWnd*		wnd;
	CDC*		dest;
	COLORREF	mark;
	COLORREF	color;

	wnd = in_view.getCWnd();
	if (wnd == nullptr)
		return;

	dest = wnd->GetDC();

	int at_y = in_y;
	for (int cnt_y=0; cnt_y<m_height; cnt_y++)
	{
		CDex* dex = m_dex_list[cnt_y];
		int at_x = in_x;
		
		while (dex)
		{
			int type = dex->Type();

			if ( type & MARK_DEAD)
				mark = 0x808080;
			else
			if ( type & MARK_CLAMP)
				mark = 0x00ffff;
			else
			if ( type & MARK_BORDER)
				mark = 0x0000ff;
			else
			if ( type & MARK_TOOL)
				mark = 0xffff00;
			else
			if ( type & MARK_BODY)
				mark = 0x00ff00;
			else
			if ( type & MARK_HOLE)
				mark = 0xff0000;
			else
				// mark = 0x000000;
				mark = 0xffffff;

			for (int cnt_x=dex->First(); cnt_x<=dex->Last(); cnt_x++)
			{
				if (mark || erase)
				{
					if (invert)
						color = mark ^ dest->GetPixel( at_x, at_y );
					else
						color = mark;

					dest->SetPixelV( at_x, at_y, color );
				}
				at_x++;
			}

			dex = dex->Next();
		}

		at_y++;
	}
}


// =======================================================================
//		TestOverlap
//
//	See if the two grids overlap -- taking into account only Part/Part and
//	Tool/Part hits.  Tool/Tool overlap is okay.
//
//	This is assumed to be the "sheet" or fixed grid;  the target is the 
//	grid that is being tested for overlap (the part).
//
ePartFit	
CDexGrid::TestOverlap( 
	const CNestConfig& config,
	const CDexGrid& target, 
	const C2dCoord&	origin,
	const C2dBox&	workzone,
	bool			ignore_clamp )
{
	// The y-ordinate of the placement point is really the
	// integer index into the span list... starting at the
	// bottom (low-valued Y).
	//

	int test = ((ignore_clamp) ? TEST_OCCUPIED_NOCLAMP : TEST_OCCUPIED_CLAMP);

	int target_max_x = target.Width();
	int target_max_y = target.Height();
	
	// ---------------------------------------
	// Check part against sheet size, bad placemenet,
	// early error.
	//
	if ( (target_max_y >= m_height)
		|| (target_max_x >= m_width)
		|| (origin.X() < 0)
		|| (origin.Y() < 0) )
	{
		return PART_FIT_ERROR;
	}

	// ---------------------------------------
	// Force the extremes to fit in sheet, plus mark
	//	the status as an off-sheet fit
	//
	ePartFit fit = PART_FIT;

	int extreme = (int)origin.X() + target_max_x;
	int grid_max_x = m_width;
	grid_max_x = min( grid_max_x, (int)workzone.Xmax() );
	if ( extreme > grid_max_x )
	{
		fit = PART_FIT_OFF_SHEET;
		target_max_x -= (extreme - grid_max_x);
	}

	extreme = (int)origin.Y() + target_max_y;
	int grid_max_y = m_height;
	grid_max_y = min( grid_max_y, (int)workzone.Ymax() );
	if ( extreme > grid_max_y )
	{
		fit = PART_FIT_OFF_SHEET;
		target_max_y -= (extreme - grid_max_y);
	}

	if (origin.X() < workzone.Xmin() )
		fit = PART_FIT_OFF_SHEET;
	if (origin.Y() < workzone.Ymin() )
		fit = PART_FIT_OFF_SHEET;

	// ---------------------------------------
	// Scan for rejection
	//
	int sheet_y = (int)origin.Y();
	for (int target_y=0; target_y<target_max_y; target_y++)
	{
		const CDex* target_dex = &(target.Dex( target_y ));
		CDex* this_dex = m_dex_list[ sheet_y ];


		int target_cell = target_dex->Type();
		int this_cell = this_dex->Type();

		int sheet_x = (int)origin.X();
		for (int target_x=0; target_x<target_max_x; target_x++)
		{
			while (target_x > target_dex->Last() )
			{
				target_dex = target_dex->Next();
				target_cell = target_dex->Type();
			}

			while (sheet_x > this_dex->Last())
			{
				this_dex = this_dex->Next();
				this_cell = this_dex->Type();
			}

			if ( (target_cell & test)
				&& (this_cell & test) )
			{
				// Part-part conflict
				return PART_FIT_ERROR;
			}
			else
			if ( ( (target_cell & test)
					&& (this_cell & MARK_TOOL))
				||
				 ( (this_cell & test)
					&& (target_cell & MARK_TOOL))
				)
			{
				fit = PART_FIT_WARNING;
			}

			sheet_x++;
		}

		sheet_y++;
	}
	return fit;
}


// =======================================================================
//		TestCore
//
//	Like TestOverlap, but just checking a few pixels in the center.
//
// TODO: DYNATORCH -- Can we make this use Y-scanlines when nesting
// along the X axis?  Doing so would allow us short circuit the
// scanning as soon as we encounter a PART_FIT_OFF_SHEET condition.
// Otherwise, we must continue scanning until we find (at least) a
// PART_FIT_ERROR condition.
ePartFit	
CDexGrid::TestCore( 
	const CNestConfig& config,
	const CDexGrid& target, 
	const C2dCoord&	origin,
	const C2dBox&	workzone,
	bool			hole,
	bool			ignore_clamp,
	int			test_override )
{
	ePartFit	fit;
	int			grid_max_x, grid_max_y;
	int			target_max_x, target_max_y;
	int			extreme, in_x, in_y;
	int		test;

	// The y-ordinate of the placement point is really the
	// integer index into the span list... starting at the
	// bottom (low-valued Y).
	//
	if ( test_override )
	{
		// 2006.09.05 (PE) Introduced to check for body/body
		// interference on second (or greater) pass of part
		// against sheet (because we got severly overlapping
		// parts, both of whose representation was bounding box).
		test = test_override;
	}
	else
	{
		test = ((ignore_clamp) ? TEST_OCCUPIED_NOCLAMP : TEST_OCCUPIED_CLAMP);
		if (!hole)
			test |= MARK_HOLE;
	}

	target_max_x = target.Width();
	target_max_y = target.Height();

	// ---------------------------------------
	// Force the extremes to fit in sheet, plus mark
	//	the status as an off-sheet fit
	//
	fit = PART_FIT;

	// Move responsability for zone fits to the caller

	extreme = (int)origin.X() + target_max_x;
	grid_max_x = m_width;

	if ( CNestConfig::IsBitmapNest() )
		// Because m_width includes the border and workzone.Xmax() does not.
		grid_max_x = min( grid_max_x, m_width );
	else
		grid_max_x = min( grid_max_x, (int)workzone.Xmax() );

	if ( extreme > grid_max_x )
	{
		target_max_x -= (extreme - grid_max_x);
	}

	extreme = (int)origin.Y() + target_max_y;
	grid_max_y = m_height;

	if ( CNestConfig::IsBitmapNest() )
		// Because m_height includes the border and workzone.Ymax() does not.
		grid_max_y = min( grid_max_y, m_height );
	else
		grid_max_y = min( grid_max_y, (int)workzone.Ymax() );

	if ( extreme > grid_max_y )
		target_max_y -= (extreme - grid_max_y);

	in_x = 0;
	in_y = 0;
	if (origin.X() < workzone.Xmin() )
		in_x = -(int)origin.X();

	if (origin.Y() < workzone.Ymin() )
		in_y = -(int)origin.Y();

	if ((in_x < 0) || (in_y < 0))
		return PART_FIT_OFF_SHEET;

	// ---------------------------------------
	// Scan for rejection
	//
	int	sheet_y, sheet_x;

	sheet_y = (int)origin.Y() + in_y;
	if (sheet_y < 0)
		return PART_FIT_OFF_SHEET;

	const CDex*	target_dex;
	const CDex*	this_dex;
	ePartFit	score;
	int			target_y;
	int			target_x;

	Debug( CString("testcore_sheet.txt") );
	target.Debug( CString("testcore_part.txt") );

	for (target_y=in_y; target_y<target_max_y; target_y++)
	{
		target_dex = &(target.Dex( target_y ));
		this_dex = m_dex_list[ sheet_y ];

		sheet_x = (int)origin.X() + in_x;
		for (target_x=in_x; target_x<target_max_x; target_x++)
		{
			while (target_x > target_dex->Last() )
			{
				target_dex = target_dex->Next();
			}

			while (sheet_x > this_dex->Last())
			{
				this_dex = this_dex->Next();
			}

			score = DexTest( this_dex, target_dex, test );

			if ( CNestConfig::IsBitmapNest() )
			{
				if (score > PART_FIT_OFF_SHEET)
					return score;
			}
		
			if (score > fit)
			{
				// 2006.10.08 (PE) There is no need to check
				// any further if we encounter an error.
				if (score == PART_FIT_ERROR)
					return score;

				fit = score;

				// 2006.09.05 (PE) Introduced to check for body/body
				// interference on second (or greater) pass of part
				// against sheet (because we got severly overlapping
				// parts, both of whose representation was bounding box).
				//
				// NOTE: This may be moot since the addition of preceding
				// conditional "if (score == PART_FIT_ERROR)". I can not
				// yet prove this conjecture and we are to close to
				// releasing product, so I dare not remove the following
				// conditional statement.
				if ( test_override )
					break;
			}
			
			sheet_x++;
		}

		sheet_y++;
	}

	return fit;
}

ePartFit
CDexGrid::DexTest(
	const CDex*	sheet_dex,
	const CDex*	part_dex,
	int		test )
{
	int sheet_cell = sheet_dex->Type();
	int part_cell = part_dex->Type();

	ePartFit fit = PART_FIT;

	if ((part_cell & test) && (sheet_cell & test))
	{
		// Part/part (or material_border/part) conflict
		fit = PART_FIT_ERROR;
	}
	else if ((sheet_cell & MARK_BORDER) && (part_cell & test))
	{
		fit = PART_FIT_OFF_SHEET;
	}
	else if ((part_cell & test) && (sheet_cell & MARK_TOOL))
	{
		fit = PART_FIT_WARNING;
	}
	else if ((sheet_cell & test) && (part_cell & MARK_TOOL))
	{
		fit = PART_FIT_WARNING;
	}

	return fit;
}

// =======================================================================
//		Punch
//
//	Transfer the target grid into this one.  **Assumes it fits**
//
//	Imp. note:  used to be "Scan()" from sheet.cpp -- new sequence is:
//		First, crude-nest using TestOverlap() and Score*()
//		Bump() in sheet.cpp adjusts the geometry with the resolution grid (micro-fit)
//		Then, the geometry (at the new origin) is re-scanned (OR -- will old grid work with new micro-origin?)
//		Finally, adjusted grid is punched in... tight.
//
void		
CDexGrid::Punch( 
	const CNestConfig&	config,
	const CDexGrid&		part, 
	const C2dCoord&		origin )

{
	CDex*		sheet_dex;
	const CDex*	part_dex;
	CDex*		prev_dex;
	int		part_cell, sheet_cell;
	int			sheet_x, sheet_y;
	int			part_x, part_y;

	// Similar in structure to TestOverlap, but we transfer data insteaed
	// of just looking at it
	//
	int part_max_x = part.Width();
	int part_max_y = part.Height();

	bool hit_hole = FALSE;
	
	sheet_y = (int)(origin.Y() + 0.5);
	for (part_y = 0; part_y < part_max_y; ++part_y)
	{
		part_dex = &(part.Dex( part_y ));

		prev_dex = nullptr;

		if (sheet_y < 0)
		{
			++sheet_y;
			continue;
		}
		else if (sheet_y >= m_dex_list.Count())
		{
			break;
		}

		sheet_dex = m_dex_list[ sheet_y ];

		part_cell = part_dex->Type();
		sheet_cell = sheet_dex->Type();

		// Scan out part into sheet
		// TODO:  copy by dex, not pixel...
		sheet_x = (int)(origin.X() + 0.5);
		for (part_x = 0; part_x < part_max_x; ++part_x)
		{
			while ( part_dex && (part_x > part_dex->Last()) )
			{
				part_dex = part_dex->Next();
				if (part_dex)
					part_cell = part_dex->Type();
			}

			if (!part_dex)
				break;

			while ( sheet_dex && (sheet_x > sheet_dex->Last()) )
			{
				prev_dex = sheet_dex;
				sheet_dex = sheet_dex->Next();
				if (sheet_dex)
					sheet_cell = sheet_dex->Type();
			}

			if (!sheet_dex)
				break;

			if (part_cell & (MARK_BODY | MARK_SPACE | MARK_HOLE | MARK_TOOL))
			{
				if (sheet_cell & MARK_HOLE)
					hit_hole = TRUE;

				sheet_cell &= MARK_HOLE;

				if (part_cell == MARK_SPACE)
					sheet_cell |= MARK_BODY;  // DYNATORCH
				else
					sheet_cell |= part_cell;

				if ((sheet_x == sheet_dex->First()) && prev_dex)
				{
					prev_dex->Insert( sheet_cell, sheet_x );
					sheet_dex = prev_dex->Next();
				}
				else
				{
					prev_dex = sheet_dex;
					sheet_dex->Insert( sheet_cell, sheet_x );
				}
			}

			++sheet_x;
		}

		++sheet_y;
	}

	Debug( CString("_dex_punch.txt") );
}


// =======================================================================
//		Filter
//
//		Filter out small spans...
//
//		... oh, yeah.. border spans are kept if they are near
//		a non-dead non-border span.  Argh!
//
//	Only filters horizontally -- vertical filtering would be very slow in 
//	a DexGrid
//
void CDexGrid::Filter( 
	const CNestConfig&	config,
	const C2dCoord&		origin,		// Point at origin corner; grid units
	const C2dCoord&		extent,		// Point at far corner; grid units;
	double				in_scrap,	// scrap length cutoff; real units
	CViewMgr&			view )
{
	CWnd* wnd = view.getCWnd();

	CDC* dest = ((config.DebugBmp() && (wnd != nullptr)) ? wnd->GetDC() : nullptr);

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// 2017.09.16 (PE) -- Ugh. This hack is to prevent "nesting on remnants"
	// from crashing. The client call in CSheet::Scan() looks like:
	//
	//   pGrid()->Filter( config, place, extent, config.Narrow( config.Pass() ), view );
	//
	// but one problem is "config.Narrow( config.Pass() )" makes no sense in
	// the context at hand. The return value of config.Pass() is zero and so
	// config.Narrow() returns a large uninitialized value, leading to a memory
	// allocation fault downstream.
	//
	// So here we punt and use what appear to be reasonable values, and they
	// may even be the same (or close to) the values for the parts being processed.
	//
	// Bottom line, properly fixing will likely take considerable investigation
	// and refactoring.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (config.Pass() < COMMON_PASS)
		in_scrap = fabs(extent.X() - origin.X());

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	int scrap_len = (int)ceil(in_scrap / config.Resolution());
	int scrap_extra = (int)(scrap_len * 1.5);

	// Filter horizontal short stuff
	int start_x = (int)origin.X() - scrap_extra;
	if (start_x < 0)
		start_x = 0;

	int end_x = (int)extent.X() + scrap_extra;
	if (end_x >= m_width)
		end_x = m_width-1;

	int sheet_y, start_y = max(0, (int)origin.Y());
	int end_y = min(m_dex_list.Count()-1, (int)extent.Y());
	for (sheet_y=start_y; sheet_y<=end_y; sheet_y++)
	{
		CDex* start_dex = m_dex_list[sheet_y];
		while (start_dex->Last() < start_x)
			start_dex = start_dex->Next();

		CDex* at_dex = start_dex->Next();
		while ( at_dex && (at_dex->First() < end_x) )
		{
			if ( ((at_dex->Last() - at_dex->First()) < scrap_len)
				&& at_dex->Next() )

			{
				at_dex->Type( at_dex->Type() | MARK_DEAD );

				if ( config.DebugBmp() && (dest != nullptr) )
				{
					for (int cnt_x=at_dex->First(); cnt_x<=at_dex->Last(); cnt_x++)
						dest->SetPixelV( cnt_x, sheet_y, 0x808080);
				}
			}
			at_dex = at_dex->Next();
		}

		at_dex = start_dex;
		while ( at_dex && (at_dex->First() < end_x))
		{
			CDex* next_dex = at_dex->Next();

			if ( next_dex && (at_dex->Type() == next_dex->Type()) )
			{
				at_dex->MergeNext();
//Validate();
			}
			else
				at_dex = next_dex;
		}
	}

	// -----------------------------------------------------
	// Filter vertical short stuff (the slow way)
	//
	start_x = max(0, (int)origin.X());
	end_x = min((int)extent.X(), m_width-1);

	start_y = max(0, (int)origin.Y()-scrap_extra);
	end_y = min(m_dex_list.Count()-1, (int)extent.Y()+scrap_extra);

	int ynum = (end_y - start_y)+1;
	CDex** vlist = new CDex*[ynum];
	for (sheet_y=0; sheet_y<ynum; sheet_y++)
	{
		vlist[sheet_y] = m_dex_list[sheet_y+start_y];
	}

	int sheet_x;
	int span;
	bool hit_sheet;

	CDex* at_dex=nullptr;
	CDex** at_row=nullptr;
	for (sheet_x=start_x; sheet_x<=end_x; sheet_x++)
	{
		at_row = vlist;
		for (sheet_y=0; sheet_y<ynum; sheet_y++)
		{
			at_dex = *at_row;
			while ( at_dex && (at_dex->Last() < sheet_x) )
			{
				*at_row = at_dex->Next();
				at_dex = *at_row;
			}

			at_row++;
		}

		at_row = vlist;
		span = 0;
		hit_sheet = false;
		for (sheet_y=0; sheet_y<ynum; sheet_y++)
		{
			at_dex = *at_row;
			at_row++;

			if ( at_dex && (at_dex->Type()&MARK_BODY) )
			{
				if ( span && ((span < scrap_len) || !hit_sheet) )
				{
					for (int kill_y=sheet_y-span; kill_y<sheet_y; kill_y++)
					{
						at_dex = vlist[kill_y];
						at_dex->Insert(MARK_DEAD, sheet_x);

						if ( config.DebugBmp() && (dest != nullptr) )
						{
							dest->SetPixelV( sheet_x, kill_y+start_y, 0x808080);
						}

					}
					span = 0;
					hit_sheet = false;
				}
			}
			else // Not in Body (e.g. Tool or Sheet)
			{
				if (!(at_dex->Type() & MARK_TOOL))
				{
					hit_sheet = true;
				}
				span++;
			}
		}
	}

	delete [] vlist;
}

// =======================================================================
//		MarkBox
//
//	Scan in a box of pixels to the grid; inclusive of fenceposts of box
//
void	
CDexGrid::MarkBox( 
	const C2dBox&	box, 
	int			pattern,	// Bit-pattern
	bool			draw )		// TRUE draw, FALSE erase
{
	for (int at_y=(int)floor(box.Ymin()); at_y<=(int)ceil(box.Ymax()); at_y++)
	{
		if ( !InRangeY( at_y ) )
			continue;

		CDex* at_dex = m_dex_list[at_y];
		int at_cell = at_dex->Type();
		for (int at_x=(int)floor(box.Xmin()); at_x<(int)ceil(box.Xmax()); at_x++)
		{
			if ( !InRangeX( at_x ) )
				continue;

			while (at_x > at_dex->Last())
			{
				at_dex = at_dex->Next();
				at_cell = at_dex->Type();
			}

			int new_cell = at_cell;
			if (draw)
				new_cell |= pattern;
			else
				new_cell &= ~pattern;
			if (new_cell != at_cell)
			{
				at_dex->Insert( new_cell, at_x );
//Validate();
			}
		}
	}
}

// =======================================================================
//		MarkDisk
//
void	
CDexGrid::MarkDisk( 
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
		if ( !InRangeY( at_y ) )
			continue;

		CDex* at_dex = m_dex_list[at_y];
		int at_cell = at_dex->Type();

		double rise = at_y - ctr.Y();
		double run = sqrt( radius2 - rise*rise );

		int from_x = (int)(ctr.X() - run + 0.5);
		int to_x = (int)(ctr.X() + run + 0.5);

		for (int at_x=from_x; at_x<=to_x; at_x++)
		{
			if ( !InRangeX( at_x ) )
				continue;

			while (at_x > at_dex->Last())
			{
				at_dex = at_dex->Next();
				at_cell = at_dex->Type();
			}

			int new_cell = at_cell;
			if (draw)
				new_cell |= pattern;
			else
				new_cell &= ~pattern;

			if (new_cell != at_cell)
			{
				at_dex->Insert( new_cell, at_x );
			}
		}
	}
}



// =======================================================================
void	
CDexGrid::Debug( const CString& in_file ) const
{
	if (0)
	{
		HANDLE	file;

		CString debug_path = CPath::DebugDir() + "\\" + in_file;

		file = CreateFile(	debug_path,				// name
							GENERIC_WRITE,			// access mode
							FILE_SHARE_READ,		// share mode
							nullptr,					// security descriptor
							CREATE_ALWAYS,			// how to create
							FILE_ATTRIBUTE_NORMAL,	// file attributes
							nullptr );					// handle of file to match
		if (file == INVALID_HANDLE_VALUE)
			return;

		Debug( file );

		CloseHandle( file );
	}
}

void
CDexGrid::Debug( HANDLE	file ) const
{
	DWORD	did_write;

	CString buf = "-------------- Horizontal -------------- \n\n";
	WriteFile( file, (LPCVOID)buf, buf.GetLength(), &did_write, nullptr );

	for (int at_y=0; at_y<m_height; at_y++)
	{
		CDex* dex = m_dex_list[at_y];

		dex->Debug( file );
	}
}


bool
CDexGrid::Validate( void )
{
	for (int cnt_y=0; cnt_y<m_height; cnt_y++)
	{
		CDex* dex = m_dex_list[cnt_y];
		
		int at_x = 0;
		while (dex)
		{
			if ( (dex->First() > dex->Last())
				|| (dex->First() != at_x) )
				return FALSE;
			at_x = dex->Last() + 1;
			dex = dex->Next();
		}
	}
	return TRUE;
}

boolean
CDexGrid::InRangeX( int x )
{
	if (x < 0)
		return false;

	return (x < m_width);
}

boolean
CDexGrid::InRangeY( int y )
{
	if (y < 0)
		return false;

	return (y < m_height);
}

// Pilfered from TestOverlap()
// Where origin is a grid-space coordinate of the sheet.
double	
CDexGrid::OverlapCalc( const C2dCoord& origin, const CDexGrid& part, int y_sign )
{
	const CDex*	part_dex;
	CDex*		sheet_dex;
	int		part_cell, sheet_cell;
	int			sheet_x, sheet_y;
	int			part_x, part_y;
	int			part_max_x, part_max_y;
	double		overlap, contrib;
	double		dx, dy;

	// Subtract 1 because the extents of a part that
	// is 1x1 units coincides with the 'origin cell'.
	part_max_x = part.Width() - 1;
	part_max_y = part.Height() - 1;
	
	// Check part against sheet size, bad placement.
	if ((origin.X() < 0) || (origin.Y() < 0))
		return -1;

	if (((origin.X() + part_max_x) > m_width) ||
		((origin.Y() + part_max_y) > m_height))
			return -1;

	overlap = 0;

	sheet_y = (int) origin.Y();
	//sheet_y = (int) origin.Y() - 1;
	for (part_y = 0; part_y < part_max_y; ++part_y)
	{
		part_dex = &(part.Dex( part_y ));
		sheet_dex = m_dex_list[ sheet_y ];


		part_cell = part_dex->Type();
		sheet_cell = sheet_dex->Type();

		contrib = 0.;

		sheet_x = (int) origin.X();
		for (part_x = 0; part_x < part_max_x; ++part_x)
		{
			while (part_x > part_dex->Last() )
			{
				part_dex = part_dex->Next();
				part_cell = part_dex->Type();
			}

			while (sheet_x > sheet_dex->Last())
			{
				sheet_dex = sheet_dex->Next();
				sheet_cell = sheet_dex->Type();
			}

			if ((sheet_cell == MARK_NONE) && (part_cell == MARK_NONE))
			{
				dx = (double) (part_max_x - part_x);

				// ie. When nesting up the Y-axis, we want a higher score
				// when there is more empty space at the bottom of the part.
				// Conversly, when nesting down the Y-axis, we want a higher
				// score when there is more empty space a the top of the part.
				dy = (double) ((y_sign < 0) ? part_y : (part_max_y - part_y));

				contrib = dx * dy;

				overlap += contrib;
			}

			++sheet_x;
		}

		++sheet_y;
	}

	return overlap;
}

