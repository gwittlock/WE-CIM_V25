#pragma once

#include <stdafx.h>

#include "Return.h"
#include "2dBox.h"
#include "3dCoord.h"
#include "DbCurve.h"
#include "DbProfile.h"
#include "Profile.h"
#include "GeoPoly.h"
#include "NestConfig.h"
#include "ViewMgr.h"

// ==================================================================
// marks
#define MARK_NONE		0x00  // 00000000 --   0

// Final part
#define MARK_TOOL		0x01  // 00000001 -- 1
#define MARK_BODY		0x02  // 00000010 -- 2
#define MARK_HOLE		0x04  // 00000100 -- 4
#define MARK_DEAD		0x08  // 00001000 -- 8

// Sheet
#define MARK_BORDER		0x10  // 00010000 -- 16
#define MARK_CLAMP		0x20  // 00000000 -- 32

// Part Slice
#define MARK_SPACE		0x40  // 00100000 -- 64
#define MARK_FILL_OKERF	0x80  // 01000000 -- 128
#define MARK_FILL_BODY	0x100 // 10000000 -- 256
#define MARK_FILL_IKERF	0x200 // 00000000 -- 512
#define MARK_FILL_HOLE	0x400 // 00000000 -- 1024

#define MARK_FILL_BITS (MARK_FILL_OKERF | MARK_FILL_BODY | MARK_FILL_IKERF | MARK_FILL_HOLE)

#define MARK_BUMPER		(MARK_TOOL|MARK_BODY)

// Remnants
#define MARK_GRID		MARK_DEAD

// mark tests
// Specifically NOT including MARK_TOOL; that is managed separately.
#define TEST_OCCUPIED_CLAMP	(MARK_BODY | MARK_BORDER | MARK_CLAMP)
#define TEST_OCCUPIED_NOCLAMP (MARK_BODY | MARK_BORDER)

extern char GridSymbolGet( int mark );

// ==================================================================

class dllExport CGrid
{
public:
	CGrid();
	virtual ~CGrid();

	CReturn	Create( const C2dBox& in_extent, const C2dCoord& in_center, double in_resolution, double in_angle );
	CReturn	Create( const C2dBox& in_extent, const C2dCoord& in_center, double in_resolution );

	int		Width( void ) const								{ return m_width; }
	int		Height( void ) const							{ return m_height; }
	double	Resolution( void ) const						{ return m_resolution; }

	const C2dCoord&	Origin( void ) const					{ return m_origin; }

	const C2dCoord& Center( void ) const					{ return m_center; }

	CReturn	Slice( const CDbProfile& in_prof, int fill_mark );
	CReturn	Slice( const CProfile& in_prof );
	CReturn	Slice( const CGeoPoly& in_poly, int fill_mark );
	CReturn	Slice( CGeoElem* geo_ent, double spacing );

	CReturn	Fill( const CNestConfig& in_config, int fill_mark );
	CReturn Clean(void);

	CReturn Invert( void );
	bool	Verify( int flag, int thresh );

	int	Cell( int in_x, int in_y ) const				{ return *(m_grid + in_x + in_y*m_width); }
	int* RowAddress( int in_y ) const					{ return m_grid + in_y*m_width; }

	void	MarkBox( const C2dBox& box, int pattern, bool draw );
	void	MarkDisk( const C2dCoord& ctr, int dia, int pattern, bool draw );

	CReturn	Border( int in_width );

	void	Debug( const CString& in_file ) const;

	void	Draw( CViewMgr& in_view, int in_x, int in_y, bool invert, bool erase ) const;

	void BodyFill( const C2dCoord& interior_pt );

private:

	void do_slice( const CGeoCurve& curve, C3dCoordList* ptarray );

	void FloodFillPrep();
	void QueueFloodFill( int row, int col );
	void QueueFloodFill8( int row, int col );
	void QueueFloodFill4( int row, int col );

	void CheezyBodyFill();
	void RowFill( int row );
	int RowScan( int* markers, int colA, int mark );

	void CellMark( const C3dCoord& pt, double spacing );
	void ConditionalCellMark( int row, int col, int mark );

private:
	// Disabled.
	CGrid( const CGrid& );
	const CGrid& operator = ( const CGrid& );
	int operator == ( const CGrid& ) const;
	int operator != ( const CGrid& ) const;

private:
	// Actual Grid
	int*		m_grid;			// Grid array
	int			m_width;		// Dimensions
	int			m_height;
	int			m_size;
	double		m_resolution;	// Slicing resolution
	C2dCoord	m_origin;		// Origin position (min of MER on model)
	C2dCoord	m_center;		// Center of rotation

	// As much as I prefer using CDynamicArray, CIndxList
	// is much more efficient for use by QueueFloodFill().
	CIndxList<CSize*> m_queue;
};
