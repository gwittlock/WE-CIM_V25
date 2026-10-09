#if !defined(_PATHFINDER_H)
#define _PATHFINDER_H
#pragma once

// ==================================================================
//		PathFinder
//
//	Given an array of points in (which may have arbitrary references
//	in them, for accounting purposes) -- find a short path that
//	touches all of them.
//
//	The PathFinder can be given clues as to what the path should be;
//	a zig-zag start, or an expected outline.  Or, it can start with
//	a generic circle of "nodes" and try to find the best path
//	point-to-point.
//
//	SOM-OPT algorithm developed by Edwin Wise, May 2001.
//
//	Related work:
//
//		The Self-Organizing Map (SOM)
//		Teuvo Kohonen
//		http://www.cis.hut.fi/projects/somtoolbox/theory/somalgorithm.shtml
//
//		Traveling Salesman Problem by Neural Approaches
//		Roland Fox, Huei-Juen Lin, Fan-Yi Jiang
//		http://neural.cs.nthu.edu.tw/chu/proj/project/11/index2.html
//		
//		An Analysis of Various Elastic Net Algorithms
//		Jan van den Berg, Jock H. Geselschap
//		http://netec.wustl.edu/WoPEc/data/Papers/dgreureco199710.html
//		
//		The Optimal Elastic Net:  Finding Solutions to the Traveling Salesman Problem
//		J.V. Stone
//		ftp://ftp.shef.ac.uk/pub/misc/personal/pc1jvs/papers/tsp_en.ps.gz
//		
//		Growing Cell Structures - A Self-organizing Network for Unsupervised and Supervised Learning.
//		Bernd Fritzke
//		ICSI TR-93-026, Berkely, CA
//
// ==================================================================

#include "Common.h"

#include "2dCoord.h"
#include "DynamicArray.h"
#include "IndxList.h"
//#include "glCanvas.h"

// ==================================================================

#define CglCanvas void

enum ePathMode
{
	PATH_NONE,

	PATH_PRESET,		// User defines initial node layout
	PATH_CLOSEST,		// Closest-point path
	PATH_ZIG_X,			// Multiple passes in X, one pass in Y
	PATH_ZAG_Y			// Multiple passes in Y, one pass in X
};

// ==================================================================

#define PATH_DEFAULT_CATTRACT	1.00
#define PATH_DEFAULT_NATTRACT	0.50
#define PATH_DEFAULT_TENSION	1.00
#define PATH_DEFAULT_SCOPE		0.25
#define PATH_DEFAULT_DIE		-2.00
#define PATH_DEFAULT_GROW		2.00

#define PATH_CIRCLE_RADIUS		0.50
#define PATH_ZIG_RADIUS			1.00
#define PATH_DEFAULT_ZIG		4

#define PATH_BOREDOM_KICK		15
#define PATH_BOREDOM_EASE		5

#define PATH_TROPISM_FACTOR		0.50

#define PATH_TERMINAL_DIST		0.005
#define PATH_TERMINAL_DIST2		(TERMINAL_DIST*TERMINAL_DIST)

// ==================================================================
//	Private data classes; used only here, so I've taken gross
//	liberties with them.
//	
class CPathCity;
class CPathNode: public C2dCoord
{
	friend class CPathFinder;

private:
	double	Dist2( const C2dCoord& pt ) const	{ double dx=X()-pt.X(); double dy=Y()-pt.Y(); return dx*dx+dy*dy; }

	C2dCoord	m_step;			// Accumulated step vector
	double		m_factor;		// step factor (as per fuzzy value weight)

	double		m_vitality;		// Growth or death "vitality" measure (was weight)

	CPathCity*	m_cmin;			// Index of closest city
	double		m_cdist;		// Distance to closest city
};

// ------------------------------------

class CPathCity : public C2dCoord
{
	friend class CPathFinder;

private:
	bool		m_latched;		// TRUE if this city has been chosen by a node

	CPathNode*	m_nmin;			// Closest node
	double		m_ndist;		// Distance to closest node

	void*		m_reference;	// Generic user reference, so tha path will have meaning
};

// ------------------------------------

typedef CIndxList<CPathNode*> NodeList;
typedef CDynamicArray<CPathCity*> CityArray;

// ==================================================================

class dllExport CPathFinder
{
public:
	CPathFinder( ePathMode mode );
	~CPathFinder( void );

	CReturn	Reset( void );

	//
	// Cities are the places that will be visited.
	//
	//	Place cities BEFORE nodes.
	//
	int		CityCount( void ) const			{ return m_city_array.Count(); }
	CReturn	CityAppend( const C2dCoord& city_pt, void* ref=NULL );
	CReturn	CityPrepare( void );

	void*	CityRef( int idx ) const		{ return m_city_array[idx]->m_reference; }

	//
	// Nodes are the working portion of PathFinder, and their
	//	order defines the path.  Added nodes have different meanings
	//	depending on the mode:
	//
	//	Add nodes AFTER the cities have been created.
	//
	//	PATH_PRESET
	//		The user must add all initial nodes; it is assumed that they
	//		represent an approximation of the solved path -- such as for
	//		toolpath outline in Nesting.
	//
	//	PATH_CLOSEST
	//	PATH_ZIG_X
	//	PATH_ZIG_Y
	//		Any nodes added will be preset points in the path.  The first
	//		node added will be the first node visited; all others will be
	//		interspersed among the circle of initial nodes.  Normally, only
	//		one node is added -- the start point of the path -- if any.
	//
	//	After any manual nodes have been added, call NodePrepare to finish
	//	it off.
	//
	int		NodeCount( void ) const			{ return m_node_list.Count(); }
	CReturn	NodeAppend( const C2dCoord& node_pt );
	CReturn	NodePrepare( int node_num=0 );

	void*	NodeRef( int idx ) const		{ return m_node_list[idx]->m_cmin->m_reference; }

	//
	// Main Function
	//
	CReturn	Evaluate( CglCanvas* canvas = NULL );

	//
	// Various operating parameters.  Automatically set by constructor, but
	// you can override them afterwards, if desired.
	//
	double	CityAttract( void ) const	{ return m_c_attract; }
	void	CityAttract( double val )	{ m_c_attract = val; }

	double	NodeAttract( void ) const	{ return m_n_attract; }
	void	NodeAttract( double val )	{ m_n_attract = val; }

	double	NodeTension( void ) const	{ return m_n_tension; }
	void	NodeTension( double val )	{ m_n_tension = val; }

	double	NodeRadius( void ) const	{ return m_radius; }
	void	NodeRadius( double val )	{ m_radius = val; }

	int		NodeZigNum( void ) const	{ return m_zignum; }
	void	NodeZigNum( int val )		{ m_zignum = val; }

	double	Scope( void ) const			{ return m_scope; }
	void	Scope( double val )			{ m_scope = val; }

	double	DieThresh( void ) const		{ return m_die; }
	void	DieThresh( double val )		{ m_die = val; }

	double	GrowThresh( void ) const	{ return m_grow; }
	void	GrowThresh( double val )	{ m_grow = val; }

	double	CityScale( void ) const		{ return m_scale; }

private:
	void	node_defaults( ePathMode mode );	// Set operating defaults
	void	node_preset( void );				// Preset the nodes
	void	node_circle( int num );				// Nodes in a circle
	void	node_zig_x( int num, int start_x, int start_y );
	void	node_zag_y( int num, int start_x, int start_y );
	void	node_sort( const CPathNode& anchor );
	void	node_latch( void );
	void	node_kick( void );
	void	node_growdie( void );
	void	node_snaptocity( void );
	void	debug_draw( CglCanvas* canvas );

	ePathMode	m_mode;

	double		m_c_attract;		// Attraction of cities for nodes
	double		m_n_attract;		// Attraction of nodes for cities
	double		m_n_tension;		// Inter-node tension

	double		m_radius;			// Starting circle radius 
	int			m_zignum;			// Number of zigs

	double		m_scope;			// Scope of operation; neighborhood or learning factor
	double		m_die;				// Node death threshold
	double		m_grow;				// Node growth threshold

	double		m_scale;			// City scale factor

	CityArray	m_city_array;
	NodeList	m_node_list;
	NodeList	m_user_list;
};


#endif
