#if !defined(_DEXGRID_H)
#define _DEXGRID_H
#pragma once

// ==================================================================
//		DexGrid
//
// ==================================================================

#include "Common.h"
#include "IndxList.h"

#include "Grid.h"
#include "Dex.h"

// ==================================================================

enum ePartFit
{
	PART_FIT = 0,				// Fits fine
	PART_FIT_WARNING = 1,		// Kerf overlaps Part (curable error)
	PART_FIT_OFF_SHEET = 2,		// Off edge of sheet (can be recovered from)
	PART_FIT_ERROR = 3			// Part overlaps part, or other fatal placement error
};

typedef CDynamicArray<TDexArray*> T2dDexArray;

// ==================================================================

class dllExport CDexGrid
{
public:
	CDexGrid( const CGrid& grid );
	~CDexGrid( void );

	// ---------------------

	int		Width( void ) const				{ return m_width; }		// Dimensions in GRID
	int		Height( void ) const			{ return m_height; }

	const C2dCoord&	Origin( void ) const	{ return m_origin; }	// Origin in WORLD

	const C2dCoord& Center( void ) const	{ return m_center; }

	double Resolution( void ) const			{ return m_res; }

	// ---------------------

	ePartFit	TestOverlap( 
							const CNestConfig&	config,
							const CDexGrid&		target, 
							const C2dCoord&		origin, 
							const C2dBox&		workzone,
							bool				ignore_clamp );

	ePartFit	TestCore( 
							const CNestConfig&	config,
							const CDexGrid&		target, 
							const C2dCoord&		origin, 
							const C2dBox&		workzone,
							bool				hole,
							bool				ignore_clamp,
							int				test_override );


	void		Punch( 
						const CNestConfig&	config,
						const CDexGrid&		target, 
						const C2dCoord&		origin );

	void	Filter( 
					const CNestConfig&	config,
					const C2dCoord&		in_origin,		// Point at origin corner
					const C2dCoord&		in_extent,		// Point at far corner
					double				in_scrap,		// scrap length cutoff
					CViewMgr&			view );

	void	MarkBox( const C2dBox& box, int pattern, bool draw );
	void	MarkDisk( const C2dCoord& ctr, int dia, int pattern, bool draw );

	int	Cell( C2dCoord& pos );

	void	Debug( const CString& in_file ) const;
	void	Debug( HANDLE in_file ) const;

	void	Draw( CViewMgr& in_view, int in_x, int in_y, bool invert, bool erase ) const;

	bool	Validate( void );

	double OverlapCalc( const C2dCoord& origin, const CDexGrid& part, int y_sign );

protected:

	const CDex&	Dex( int idx ) const	{ return *m_dex_list[idx]; }
	CDex*		pDex( int idx )			{ return m_dex_list[idx]; }

private:

	void	scan_horz( const CGrid& grid );
//	void	scan_vert( const CGrid& grid, CDexList* dest );
	boolean InRangeX( int x );
	boolean InRangeY( int y );

	ePartFit DexTest(
		const CDex*	sheet_dex,
		const CDex*	part_dex,
		int		test );

private:

	int			m_width;		// Dimensions
	int			m_height;

	C2dCoord	m_origin;		// Origin position (min of MER on model)
	C2dCoord	m_center;		// Center of rotation

	C2dCoord	m_iterator;		// Iterator position.  TODO: Break out into seperate class?
	CDex*		m_itdex;
	CDex*		m_itrow;

	double		m_res;			// Resolution from Grid

	CDexList	m_dex_list;
	// T2dDexArray	m_dex_grid;

	// NOT vertical dexgrid!  All testing is done on the horizontal, and the vert is just for
	// vertical scoring and shift calculations.  Keep one (or two) "manhattan" lists, that give the
	// the height of the skyline only... a list of byte or word length width.
};


#endif
