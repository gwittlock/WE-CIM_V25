#if !defined(_TSP_H)
#define _TSP_H

// ==================================================================
//		TSP - Traveling Salesman Problem
//
//	Growing Elastic Net with greed-swap
//
// ==================================================================
#pragma once

#include "common.h"
#include "2dCoord.h"
#include "3dCoord.h"

#include "SeqRules.h"


// ==================================================================
//		Internal utility classes
//
class Node : public C2dCoord
{
public:
	int			ref;

	C2dCoord	step;
	double		factor;
	double		weight;

	int			cmin;
	double		cdist;

	bool		fixed;
};
typedef CDynamicArray<Node*> NodeArray;


class City : public C2dCoord
{
public:
	int		ref;

	bool	latched;

	int		nmin;
	double	ndist;
};
typedef CDynamicArray<City*> CityArray;

// ==================================================================
//		TSP 
//
class dllExport CTSP
{
public:
	CTSP(void);
	~CTSP(void);

	void Init(CSeqRules* seqRules, C3dCoordArray* waypoint);

	bool Evaluate(C3dCoordArray* result, int max_steps);

private:
	void do_build_zig(CSeqRules* seqRules, C2dBox& extent);
	void do_build_round(CSeqRules* seqRules, C2dBox& extent, C2dCoord& anchor);

	void do_reset(void);

	void do_match_nodes();
	void do_attract_nodes(double attraction);
	double do_attract_cities(double attraction);
	void do_shift_nodes(void);
	void do_latch(void);

	void do_grow_die(void);

	void do_kick(void);

	bool do_greedy_swap(int max_steps, int swap_dist);

	void debug_nodes(bool stop);

int calculate_columns(
	CSeqRules* rules, 
	C3dCoordArray* waypoint,
	C2dBox& extent );

private:
	// Control Parameters
	//
	const static double m_c_attract;
	const static double m_n_attract;
	const static double m_tension;

	const static double m_die;
	const static double m_grow;

	const static double m_neighborhood;
	const static double m_neighbormin;
	const static double m_neighbordecay;

	const static double m_radius;

	const static int m_kick;

	const static double m_terminal_dist;
	const static double m_terminal_dist2;

	const static int m_nodes_per_zig;
	//
	// Working Data
	//
	NodeArray	m_node_array;
	CityArray	m_city_array;

	C2dCoord m_ctr;
	double m_scale;

	C2dCoord m_tropism;		// Tropism in X and Y.  >1 discourages motion in that axis

	double m_neighbor;		// Working copy of neighborhood diameter

	int	m_boredom;			// If we get bored, kick the nodes a bit

	bool m_swap;			// Greedy swap mode?
	bool m_done;			// Done?
};

#endif
