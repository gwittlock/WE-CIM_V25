#if !defined(_SEED_H)
#define _SEED_H
#pragma once

// ==================================================================
//		Seed
//
// ==================================================================

#include "Common.h"
#include "3dCoord.h"
#include "2dbox.h"
#include "ToolHit.h"

// ==================================================================

class dllExport CSeed
{
public:

	CSeed();

	CSeed(
		int				priority,
		const C2dCoord&	part_place,
		CToolHit*		toolhit,
		int				gravity );

	CSeed( const CSeed& seed );

	CSeed& operator = ( const CSeed& seed );

	int Priority() const				{ return m_priority; }
	void Priority(int val)				{ m_priority = val; }

	const C2dCoord& Placement() const	{ return m_pt; }

	CToolHit* ToolHit()					{ return m_toolhit; }

	double X() const					{ return m_pt.X(); }
	double Y() const					{ return m_pt.Y(); }

	int Gravity(void) const				{ return m_gravity; }
	void Gravity(int gravity)			{ m_gravity = gravity; }

	bool Overlaps( const CSeed& seed, double range );

	// Flags

	bool DebugTouch() const				{ return m_touched; }
	void DebugTouch( bool touched )	{ m_touched = touched; }

	bool PreLink() const				{ return m_prelink; }
	void PreLink( bool prelink )		{ m_prelink = prelink; }

	void GridScore( int score )			{ m_grid_score = score; }
	int GridScore( void )				{ return m_grid_score; }

	~CSeed( void );

private:  // Disabled

	int operator == ( const CSeed& ) const;
	int operator != ( const CSeed& ) const;

private:

	static C2dBox m_tmp_box;

private:

	C2dCoord	m_pt;			// a world coordinate

	int			m_gravity;		// ORD_X for x gravity, ORD_Y for y gravity
	int			m_priority;		// 1 is basic priority, 2 is higher and so forth

	bool		m_touched;
	bool		m_prelink;

	CToolHit*	m_toolhit;

	int	m_grid_score;
};

typedef CDynamicArray<CSeed*>	CSeedArray;

#endif
