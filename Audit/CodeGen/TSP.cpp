// ============================================================================
//		TSP - Traveling Salesman Problem
//
//	Growing Elastic Net with greed-swap
//
// ============================================================================

#include "stdafx.h"

#include "ViewMgr.h"
#include "ViewBase.h"
#include "oglViewWire.h"

#include "tsp.h"

// ============================================================================
//
// Control constants.  Change these to change the behavior of the TSP search.
//
const double CTSP::m_c_attract = 1.00;		// City to nearest node
const double CTSP::m_n_attract = 0.50;		// Node to nearest city
const double CTSP::m_tension = 1.00;		// Path surface tension

const double CTSP::m_die = 2.00;			// Death factor
const double CTSP::m_grow = 2.00;			// Growth factor

const double CTSP::m_neighborhood = 0.25;	// Attraction diameter
const double CTSP::m_neighbormin = 0.01;	// Threshold
const double CTSP::m_neighbordecay = 0.96;	// Decay factor when used

const double CTSP::m_radius = 0.50;			// Path radius (for round path)

const int CTSP::m_kick = 20;				// Boredom kick point

const double CTSP::m_terminal_dist = 0.005;
const double CTSP::m_terminal_dist2 = (m_terminal_dist*m_terminal_dist);

const int CTSP::m_nodes_per_zig = 3;		// Starting node count per path pass

// ==================================================================

CTSP::CTSP()
{
}

CTSP::~CTSP(void)
{
	do_reset();
}

void
CTSP::do_reset()
{
	m_neighbor = m_neighborhood;
	m_boredom = 0;

	m_swap = false;
	m_done = false;

	m_node_array.DestructiveFlush();
	m_city_array.DestructiveFlush();
}

void CTSP::Init( CSeqRules* rules, C3dCoordArray* waypoint )
{
	do_reset();
	//
	// Load the waypoints into the "cities"
	//
	C2dBox cbox;

	int idx, num = waypoint->Count();
	for (idx=0; idx<num; idx++)
	{
		C3dCoord* pt = (*waypoint)[idx];
		City* city = new City();
		m_city_array.Append(city);

		city->XY(pt->X(), pt->Y());
		city->ref = (int)pt->Z();
		city->latched = false;

		cbox += *city;
	}
	//
	// Auto-calculate the number of passes.
	// Also sets the tropism to match the linearity of the
	// passes.
	//
	int memo = rules->linear_trend;
	if (rules->grid_opt)
	{ rules->linear_trend = calculate_columns(rules, waypoint, cbox); }
	//
	// Center and scale the waypoints
	//
	m_ctr.XY(cbox.Xc(), cbox.Yc());
	m_scale = max(cbox.Dx(), cbox.Dy()) * 0.5;

	cbox.Update( (cbox.Xmin()-m_ctr.X()) / m_scale,
				(cbox.Ymin()-m_ctr.Y()) / m_scale,
				(cbox.Xmax()-m_ctr.X()) / m_scale,
				(cbox.Ymax()-m_ctr.Y()) / m_scale );
	for (idx=0; idx<num; idx++)
	{
		City* city = m_city_array[idx];

		city->XY((city->X()-m_ctr.X()) / m_scale,
				(city->Y()-m_ctr.Y()) / m_scale );
	}
	//
	// Lay down the prototype path(s)
	//
	if (rules->grid_opt)
	{ 
		do_build_zig(rules, cbox);
		rules->linear_trend = memo;
	}
	else
	{ 
		m_tropism.X(1.0);
		m_tropism.Y(1.0);

		C2dCoord anchor( (rules->Anchor().X()-m_ctr.X()) / m_scale,
						(rules->Anchor().Y()-m_ctr.Y()) / m_scale );
		do_build_round(rules, cbox, anchor);
	}
	//
	//
	if (CReturn::Debug() >= 2)
	{ debug_nodes(true); }
}

// ============================================================================

void
CTSP::do_build_zig( 
	CSeqRules* rules,
	C2dBox& extent )
{
	// Set up control values
	//
	// Direction of motion defined by trend
	// ZigZag form if bidirectional set
	// Zig count in linear_trend
	//
	bool x_axis;
	double start_x;
	double start_y;
	double step_x;
	double step_y;
	//
	// The zero node doesn't move and doesn't associate with a city.
	//
	Node* node = new Node;
	node->ref = -1;
	node->fixed = true;
	node->weight = 0.0;
	node->factor = 0.0;
	m_node_array.Append(node);
	//
	//
	int trend_num = rules->linear_trend-1;
	if ( ZERO(extent.Dx())
		|| ZERO(extent.Dy()) )
	{ trend_num = 0; }

	int zig_num = m_city_array.Count()  / 10;
	zig_num = zig_num / (trend_num+1);
	zig_num = max(m_nodes_per_zig, zig_num);
	//
	// Now determine how to walk the zigs
	//
	switch ( rules->trend )
	{
	case LL_XPOS:
		x_axis = true;
		step_x = extent.Dx()/zig_num;
		step_y = extent.Dy()/trend_num;

		node->XY(-1.0, -1.0);
		start_x = extent.Xmin();
		start_y = extent.Ymin();
		break;

	case LR_XNEG:
		x_axis = true;
		step_x = -extent.Dx()/zig_num;
		step_y = extent.Dy()/trend_num;

		node->XY(1.0, -1.0);
		start_x = extent.Xmax();
		start_y = extent.Ymin();
		break;

	case UL_XPOS:
		x_axis = true;
		step_x = extent.Dx()/zig_num;
		step_y = -extent.Dy()/trend_num;

		node->XY(-1.0, 1.0);
		start_x = extent.Xmin();
		start_y = extent.Ymax();
		break;

	case UR_XNEG:
		x_axis = true;
		step_x = -extent.Dx()/zig_num;
		step_y = -extent.Dy()/trend_num;

		node->XY(1.0, 1.0);
		start_x = extent.Xmax();
		start_y = extent.Ymax();
		break;

	case LL_YPOS:
		x_axis = false;
		step_x = extent.Dx()/trend_num;
		step_y = extent.Dy()/zig_num;

		node->XY(-1.0, -1.0);
		start_x = extent.Xmin();
		start_y = extent.Ymin();
		break;

	case LR_YPOS:
		x_axis = false;
		step_x = -extent.Dx()/trend_num;
		step_y = extent.Dy()/zig_num;

		node->XY(1.0, -1.0);
		start_x = extent.Xmax();
		start_y = extent.Ymin();
		break;

	case UL_YNEG:
		x_axis = false;
		step_x = extent.Dx()/trend_num;
		step_y = -extent.Dy()/zig_num;

		node->XY(-1.0, 1.0);
		start_x = extent.Xmin();
		start_y = extent.Ymax();
		break;

	case UR_YNEG:
		x_axis = false;
		step_x = -extent.Dx()/trend_num;
		step_y = -extent.Dy()/zig_num;

		node->XY(1.0, 1.0);
		start_x = extent.Xmax();
		start_y = extent.Ymax();
		break;
	}
	//
	// Add two anchor nodes at the ends, to keep the tips from
	// going all dogleg on us.
	//
	zig_num += 2;
	switch ( rules->trend )
	{
	case LL_XPOS:
	case LR_XNEG:
	case UL_XPOS:
	case UR_XNEG:
		start_x -= step_x;
		break;

	case LL_YPOS:
	case LR_YPOS:
	case UL_YNEG:
	case UR_YNEG:
		start_y -= step_y;
		break;
	}
	//
	// Create the nodes, taking care to lock down the
	// first and last of each pass
	//
	C2dCoord at(start_x, start_y);

	bool fixed = true;
	for (int i=0; i<=trend_num; i++)
	{
		for (int j=0; j<=zig_num; j++)
		{
			node = new Node;
			node->XY(at.X(), at.Y());
			node->ref = -1;
			node->weight = 0.0;
			node->factor = 0.0;

			node->fixed = fixed;
			fixed = false;

			m_node_array.Append(node);
			//
			//
			if (x_axis)
			{ at.X( at.X() + step_x ); }
			else // y_axis
			{ at.Y( at.Y() + step_y ); }
		}

		if (rules->bidirectional)
		{
			if (x_axis)
			{ 
				at.Y( at.Y() + step_y );
				at.X( at.X() - step_x );

				step_x = -step_x;
			}
			else // y_axis
			{ 
				at.X( at.X() + step_x );
				at.Y( at.Y() - step_y );

				step_y = -step_y;
			}
		}
		else // Zig only
		{
			node->fixed = true;
			fixed = true;

			if (x_axis)
			{ 
				at.Y( at.Y() + step_y );
				at.X( start_x );
			}
			else // y_axis
			{ 
				at.X( at.X() + step_x );
				at.Y( start_y );
			}
		}
		node->fixed = true;
		fixed = true;
	}
}

// ============================================================================

void
CTSP::do_build_round( 
	CSeqRules* rules,
	C2dBox& extent,
	C2dCoord& anchor)
{
	// Set up control values
	//
	// Direction of motion defined by trend
	//
	double dx = extent.Dx() * 0.5 * m_radius;
	double dy = extent.Dy() * 0.5 * m_radius;
	//
	// The zero node doesn't move and doesn't associate with a city.
	//
	Node* node = new Node;
	node->ref = -1;
	node->fixed = true;
	node->weight = 0.0;
	node->factor = 0.0;
	m_node_array.Append(node);
	//
	//
	int zig_num = m_city_array.Count() / 10;
	zig_num = max(4*m_nodes_per_zig, zig_num);

	double start_ang;
	double step_ang = TWOPI / zig_num;

	switch ( rules->trend )
	{
	case LL_XPOS:
		start_ang = (5.0*PI) / 4.0;
		node->XY(-1.0, -1.0);
		break;

	case LR_XNEG:
		start_ang = (7.0*PI) / 4.0;
		step_ang *= -1.0;

		node->XY(1.0, -1.0);
		break;

	case UL_XPOS:
		start_ang = (3.0*PI) / 4.0;
		step_ang *= -1.0;
		node->XY(-1.0, 1.0);
		break;

	case UR_XNEG:
		start_ang = (1.0*PI) / 4.0;
		node->XY(1.0, 1.0);
		break;

	case LL_YPOS:
		start_ang = (5.0*PI) / 4.0;
		step_ang *= -1.0;
		node->XY(-1.0, -1.0);
		break;

	case LR_YPOS:
		start_ang = (7.0*PI) / 4.0;
		node->XY(1.0, -1.0);
		break;

	case UL_YNEG:
		start_ang = (3.0*PI) / 4.0;
		node->XY(-1.0, 1.0);
		break;

	case UR_YNEG:
		start_ang = (1.0*PI) / 4.0;
		step_ang *= -1.0;
		node->XY(1.0, 1.0);
		break;

	default:
		{
			C2dUnitVec origin(anchor.X(), anchor.Y());
			start_ang = origin.Radians();

			node->XY(anchor.X(), anchor.Y());
		}
		break;
	}
	//
	//
	double at_ang = start_ang;
	for (int i=0; i<zig_num; i++)
	{
		node = new Node;
		node->XY( dx*cos(at_ang), dy*sin(at_ang) );
		node->ref = -1;
		node->weight = 0.0;
		node->factor = 0.0;
		node->fixed = false;

		m_node_array.Append(node);

		at_ang += step_ang;
	}

	node = new Node;
	node->XY( anchor.X(), anchor.Y() );
	node->ref = -1;
	node->weight = 0.0;
	node->factor = 0.0;
	node->fixed = true;
	m_node_array.Append(node);

}

// ============================================================================
//
bool 
CTSP::Evaluate(
	C3dCoordArray* result, 
	int max_steps)
{
	CString str;
	CReturn ret;

	if (CReturn::Debug() >= 2)
	{ 
		str.Format("Begin TSP evaluation, %d waypoints", m_city_array.Count());
		ret.Diagnostic( str );
	}


	bool okay = false;
	while (max_steps--)
	{
		m_boredom++;

		do_match_nodes();

		do_attract_nodes(m_n_attract);
		double worst_distance = do_attract_cities(m_c_attract);

		do_shift_nodes();

		do_grow_die();

		worst_distance = sqrt(worst_distance);
		if(worst_distance < m_terminal_dist)
		{ break; }

		if (m_boredom > m_kick)
		{
			do_kick();
			m_boredom -= 5;
		}

		m_neighbor = min(m_neighbor, (m_neighbor + worst_distance*0.5) * 0.5);
//		m_neighbor = m_neighbor * m_neighbordecay;

		if (CReturn::Debug() >= 4)
		{ debug_nodes(false); }
	}

	if (CReturn::Debug() >= 2)
	{ 
		str.Format("Basic TSP done, begin greedy swap.");
		ret.Diagnostic( str );
	}
	if (max_steps)
	{ okay = do_greedy_swap(20, 10); }

	if (CReturn::Debug() >= 2)
	{ 
		str.Format("Latch nodes to their waypoints");
		ret.Diagnostic( str );
	}
	do_latch();

	if (CReturn::Debug() >= 2)
	{ debug_nodes(true); }
	else
	if (CReturn::Debug() >= 1)
	{ debug_nodes(false); }

	if (CReturn::Debug() >= 2)
	{ 
		str.Format("Transfer results.");
		ret.Diagnostic( str );
	}
	int num = m_node_array.Count();
	for (int idx=0; idx<num; idx++)
	{
		Node* node = m_node_array[idx];
		int ref = node->ref;
		
		if (ref >= 0)
		{
			C3dCoord* pt = new C3dCoord(*node);
			pt->Z( ref );
			result->Append(pt);
		}
	}

	if (CReturn::Debug() >= 2)
	{ 
		str.Format("TSP Evaluate Done.");
		ret.Diagnostic( str );
	}
	return okay;
}

// ============================================================================

void
CTSP::do_match_nodes()
{
	Node* node;
	City* city;
	//
	// Reset node's city info.
	//
	int n_idx, n_num = m_node_array.Count();
	for (n_idx=1; n_idx<n_num; n_idx++)
	{
		node = m_node_array[n_idx];

		node->factor = 0.0;
		node->step.XY(0.0, 0.0);
		node->cmin = -1;
		node->cdist = DBL_MAX;
	}
	//
	// Reset city's node info
	//
	int c_idx, c_num = m_city_array.Count();
	for (c_idx=0; c_idx<c_num; c_idx++)
	{
		City* city = m_city_array[c_idx];

		city->nmin = -1;
		city->ndist = DBL_MAX;
	}
	//
	// Associate each node to a city
	// Associate each city to a node
	//
	double dist2;

	for (n_idx=0; n_idx<n_num; n_idx++)
	{
		node = m_node_array[n_idx];
		if (node->fixed)
		{ continue; }

		for (c_idx=0; c_idx<c_num; c_idx++)
		{
			city = m_city_array[c_idx];

			double d = (node->X()-city->X()) * m_tropism.X();
			dist2 = d*d;//*m_tropism.X();
			d = (node->Y()-city->Y()) * m_tropism.Y();
			dist2 += d*d;//*m_tropism.Y();

			if (dist2 < city->ndist)
			{
				city->nmin = n_idx;
				city->ndist = dist2;
			}

			if (dist2 < node->cdist)
			{
				node->cmin = c_idx;
				node->cdist = dist2;
			}
		}
	}
}

// ============================================================================

void
CTSP::do_attract_nodes(double attraction)
{
	Node* node;
	City* city;

	int num = m_node_array.Count();
	for (int idx=0; idx<num; idx++)
	{
		node = m_node_array[idx];
		if (node->fixed)
		{ continue; }

		if (node->cmin >= 0)
		{
			city = m_city_array[node->cmin];
			city->latched = true;
			
			node->step.XY( node->step.X() + (city->X()-node->X())*attraction,
							node->step.Y() + (city->Y()-node->Y())*attraction );
			node->factor += attraction;
		}
	}
}

// ============================================================================

double
CTSP::do_attract_cities(double attraction)
{
	City* city;
	Node* node;

	double max_dist = 0.0;
	int num = m_city_array.Count();
	for (int idx=0; idx<num; idx++)
	{
		city = m_city_array[idx];

		if (city->nmin >= 0)
		{
			node = m_node_array[city->nmin];
			node->weight++;

			double suck = attraction;
			if (city->latched)
			{ suck *= 2.0; }

			node->step.XY( node->step.X() + (city->X()-node->X())*suck, 
							node->step.Y() + (city->Y()-node->Y())*suck );
			node->factor += suck;

			max_dist = max(max_dist, city->ndist);
		}

		city->latched = false;
	}

	return max_dist;
}

// ============================================================================

void
CTSP::do_shift_nodes(void)
{
	int num = m_node_array.Count();
	int cnt = num-1;

	double dx;
	double dy;
	//
	// Note that the zero node does not move, giving us a
	// instant context.
	//
	Node* node = m_node_array[1];
	Node* p_node = m_node_array[0];
	Node* n_node = NULL;

	for (int idx=1; idx<cnt; idx++)
	{
		n_node = m_node_array[idx+1];

		if (!node->fixed)
		{
			node->weight -= 1.0;
			//
			// Step to closest
			//
			if (node->factor > SMALL)
			{
				dx = node->step.X() / node->factor;
				dy = node->step.Y() / node->factor;
			}
			else
			{ dx = dy = 0.0; }
			//
			// Adjust by surface tension
			//
			if (m_neighbor > m_neighbormin)
			{
				dx += m_neighbor * m_tension * (p_node->X() - 2*node->X() + n_node->X());
				dy += m_neighbor * m_tension * (p_node->Y() - 2*node->Y() + n_node->Y());
			}
			//
			// Incorporate and step
			//
			node->XY( node->X() + dx,
						node->Y() + dy );
		}

		p_node = node;
		node = n_node;
	}
}

// ============================================================================

void
CTSP::do_grow_die(void)
{
	Node* node;
	//
	// Die
	//
	int idx, num = m_node_array.Count();
	for (idx=0; idx<num; idx++)
	{
		node = m_node_array[idx];
		if (node->fixed)
		{ continue; }

		if (node->weight < -m_die)
		{
			m_node_array.Remove(idx);
			delete node;

			idx--;
			num--;
		}
	}
	//
	// Grow
	//
	for (idx=0; idx<num; idx++)
	{
		node = m_node_array[idx];
		if (node->fixed)
		{ continue; }

		if (node->weight > m_grow)
		{
			int next = ((idx+1)<num)?idx+1:idx;
			Node* n_node = m_node_array[next];

			int prev = (idx>0)?idx-1:idx;
			Node* p_node = m_node_array[prev];
			//
			//
			double pdist = p_node->Dist2(*node);
			double ndist = n_node->Dist2(*node);

			double jitter = m_terminal_dist;
			double jx = ((double)rand() / (double)RAND_MAX) * jitter;
			double jy = ((double)rand() / (double)RAND_MAX) * jitter;
			//
			//
			Node* new_node = new Node;
			node->weight *= 0.50;
			new_node->weight = node->weight;
			new_node->ref = -1;
			new_node->fixed = false;

			if (pdist > ndist)
			{
				new_node->XY( node->X()
							+ (p_node->X() - node->X()) * jx
							+ (p_node->X() - n_node->X()) * jx,

							node->Y()
							+ (p_node->Y() - node->Y()) * jy
							+ (p_node->Y() - n_node->Y()) * jy );

				m_node_array.InsertBefore(idx, new_node);

				if (!node->fixed)
				{
					node->XY( node->X()
							+ (n_node->X() - node->X()) * jx
							+ (n_node->X() - p_node->X()) * jx,

							node->Y()
							+ (n_node->Y() - node->Y()) * jy
							+ (n_node->Y() - p_node->Y()) * jy );
				}
			}
			else // ndist > pdist
			{
				new_node->XY( node->X()
							+ (n_node->X() - node->X()) * jx
							+ (n_node->X() - p_node->X()) * jx,

							node->Y()
							+ (n_node->Y() - node->Y()) * jy
							+ (n_node->Y() - p_node->Y()) * jy );

				m_node_array.InsertAfter(idx, new_node);

				if (!node->fixed)
				{
					node->XY( node->X()
							+ (p_node->X() - node->X()) * jx
							+ (p_node->X() - n_node->X()) * jx,

							node->Y()
							+ (p_node->Y() - node->Y()) * jy
							+ (p_node->Y() - n_node->Y()) * jy );
				}
			}

			idx++;
			num++;
		}
	}
}


// ============================================================================

void 
CTSP::do_latch(void)
{
	int c_idx, c_num = m_city_array.Count();
	int n_idx, n_num = m_node_array.Count();

	Node* node;
	City* city;
	//
	// Reset the node weights
	//
	for (n_idx=0; n_idx<n_num; n_idx++)
	{
		node = m_node_array[n_idx];
		node->weight = 0.0; 
	}
	//
	// Now snap each node to its beloved city
	//
	for (c_idx=0; c_idx<c_num; c_idx++)
	{
		city = m_city_array[c_idx];
		//
		// Find the closest node to this city
		//
		int close_node = -1;
		double close_dist = DBL_MAX;

		for (n_idx=0; n_idx<n_num; n_idx++)
		{
			node = m_node_array[n_idx];

			if (node->weight > 0.5)
			{ continue; }

			double dist2 = node->Dist2(*city);
			if (dist2 < close_dist)
			{
				close_node = n_idx;
				close_dist = dist2;
			}
		}
		//
		// Snap the found node to this city
		//
		if (close_node >= 0)
		{
			node = m_node_array[close_node];

			node->XY( city->X(),
					city->Y() );

			node->weight = 1.0;
			node->ref = city->ref;
		}
	}
	//
	// Delete any nodes that did not get snapped
	//
	for (n_idx=0; n_idx<n_num; n_idx++)
	{
		node = m_node_array[n_idx];
		if (node->weight < SMALL)
		{
			m_node_array.Remove(n_idx);
			delete node;

			n_idx--;
			n_num--;
		}
	}
}
// ============================================================================

void
CTSP::do_kick(void)
{
	double jitter = m_terminal_dist;

	Node* node;
	int num = m_node_array.Count();
	for (int idx=0; idx<num; idx++)
	{
		node = m_node_array[idx];
		if (!node->fixed)
		{
			double jx = (((double)rand() / (double)RAND_MAX/2.0)-1.0) * jitter;
			double jy = (((double)rand() / (double)RAND_MAX/2.0)-1.0) * jitter;

			node->XY( node->X() + jx,
					node->Y() + jx );
		}
	}
}



// ============================================================================
//
// By now, the fixed nodes have been removed with latch()
//
bool
CTSP::do_greedy_swap(
	int max_steps, 
	int swap_num )
{
	swap_num += 3;
	int idx, swap_cnt = swap_num-1;

	int num = m_node_array.Count();
	if (num < swap_num)
	{ return true; }

	while (max_steps--)
	{
		bool did_swap = false;

		NodeArray swap_array;
		for (idx=0; idx<swap_num; idx++)
		{
			swap_array.Append( m_node_array[idx] );
		}

		for (idx=swap_cnt; idx<num; idx++)
		{
			swap_array.Replace(swap_cnt, m_node_array[idx]);
			//
			//
			double dist = 0.0;
			int i;
			for (i=0; i<swap_cnt; i++)
			{
				double d = (swap_array[i]->X()-swap_array[i+1]->X()) * m_tropism.X();
				dist += d*d;//*m_tropism.X();
				d = (swap_array[i]->Y()-swap_array[i+1]->Y()) * m_tropism.Y();
				dist += d*d;//*m_tropism.Y();
			}
			//
			//
			int swap = -1;
			for (int step=1; step<swap_num-2; step++)
			{
				int dst_idx = step + 1;

				double test = 0.0;
				for (int i=0; i<swap_cnt; i++)
				{
					int j = i;
					if (j == 1)
					{ j = dst_idx; }
					else
					if (j == dst_idx)
					{ j = 1; }
					//
					//
					int k = i+1;
					if (k == 1)
					{ k = dst_idx; }
					else
					if (k == dst_idx)
					{ k = 1; }
					//
					//
					double d = (swap_array[j]->X()-swap_array[k]->X()) * m_tropism.X();
					test += d*d;//*m_tropism.X();
					d = (swap_array[j]->Y()-swap_array[k]->Y()) * m_tropism.Y();
					test += d*d;//*m_tropism.Y();

				}
				//
				//
				if (test < (dist-SMALL))
				{
					swap = step;
					dist = test;
				}

			} // end for step
			//
			//
			if (swap > 0)
			{
				Node* node_a = swap_array[1];
				Node* node_b = swap_array[1 + swap];

				if ( !node_a->fixed && !node_b->fixed )
				{
					double tmp = node_a->X();
					node_a->X(node_b->X());
					node_b->X(tmp);

					tmp = node_a->Y();
					node_a->Y(node_b->Y());
					node_b->Y(tmp);

					int ref = node_a->ref;
					node_a->ref = node_b->ref;
					node_b->ref = ref;

					did_swap = true;
				}
			}
			//
			//
			for (i=0; i<swap_cnt; i++)
			{
				swap_array.Replace(i, swap_array[i+1]);
			}

		} // end for swap_cnt

		if (CReturn::Debug() >= 4)
		{ debug_nodes(false); }

		if (!did_swap)
		{ return true; }
	}

	return false;
}

// ============================================================================

void
CTSP::debug_nodes(bool stop)
{
	CViewBase* view = CViewMgr::ActiveView();
	CoglViewWire* oglview = dynamic_cast<CoglViewWire*>(view);

	if (CReturn::Debug() >= 2)
		view->Clear( PTEMP_LIST );

	int num = m_node_array.Count();
	int cnt = num-1;

	Node* prev_node = m_node_array[0];
	C3dCoord prev_pt( (prev_node->X() * m_scale) + m_ctr.X(),
						(prev_node->Y() * m_scale) + m_ctr.Y(),
						0.0 );
	Node* node;
	C3dCoord pt;

	for (int idx=1; idx<num; idx++)
	{
		node = m_node_array[idx];
		pt.XYZ( (node->X() * m_scale) + m_ctr.X(),
				(node->Y() * m_scale) + m_ctr.Y(),
				0.0 );

		if ( prev_node->fixed
			&& node->fixed )
		{ view->RapidLine( prev_pt, pt, 0xffffff ); }
		else
		{ view->DebugLine( prev_pt, pt ); }

		if (oglview)
		{ oglview->dot( pt, TRUE ); }

		prev_node = node;
		prev_pt = pt;
	}

	Sleep(25);
}

// ============================================================================
//
// Beware all ye who enter here... magic numbers, a horrible one-letter variable
// naming scheme, and an ugly coordinate inversion by operator[] lie within.
//
int
CTSP::calculate_columns(
	CSeqRules* rules,
	C3dCoordArray* waypoint,
	C2dBox& extent )
{
	CReturn ret;
	CString str;
	//
	// Lock the trend into 1..100 and calculate
	// some control factors.
	//
	int trend = min(100, rules->linear_trend);
	trend = max(1, trend);

	double tropism = 1.0 + trend/100.0;
	double thresh = 1.0 - trend/100.0;
	//
	// Determine the primary and secondary axes
	// Think about it as if we step in Y primary and then X second.
	// We reverse our thinking for X axis primary.
	//
	int x_idx = 0;
	int y_idx = 1;
	double rdx = rules->extents.Dx();

	switch ( rules->trend )
	{
	case LL_XPOS:
	case LR_XNEG:
	case UL_XPOS:
	case UR_XNEG:
		// Reverse
		x_idx = 1;
		y_idx = 0;

		m_tropism.X(1.0);
		m_tropism.Y(tropism);

		rdx = rules->extents.Dy();
		break;

	case LL_YPOS:
	case LR_YPOS:
	case UL_YNEG:
	case UR_YNEG:
		m_tropism.X(tropism);
		m_tropism.Y(1.0);
		break;

	}
	
	CDynamicArray<double> x_array;
	//
	// Count unique X positions
	//
	int num = waypoint->Count();
	for (int i=0; i<num; i++)
	{
		C3dCoord* ipt = (*waypoint)[i];

		double x = ((int)((*ipt)[x_idx] * 100)) / 100.0;
		bool hit = false;
		//
		// Do as a binary search or hash; this is horrible!
		//
		for (int j=0; j<x_array.Count(); j++)
		{
			double jx = x_array[j];
			if (EQUAL(jx, x))
			{
				hit = true;
			}
		}
		if (!hit)
		{
			x_array.Append(x);
		}
	}
	//
	// Find the maximum distance between columns
	//
	double left = DBL_MIN;
	double prev_x = UNDEFINED;
	double max_dx = DBL_MIN;
	while (true)
	{
		double min_x = DBL_MAX;

		for (int j=0; j<x_array.Count(); j++)
		{
			double x = x_array[j];
			if ( (x > left)
				&& (x < (min_x-SMALL)) )
			{
				min_x = x;
			}
		}

		if (min_x < DBL_MAX)
		{
			if (DEFINED(prev_x))
			{
				double dx = min_x - prev_x;

				if (dx > max_dx)
				{ max_dx = dx; }

			}
			left = min_x + SMALL;
			prev_x = min_x;
		}
		else
		{ break; }
	}
	//
	// If our optimization area is a tiny fraction of the total working
	// zone, get a little more perspective on it.
	//
	if (rdx > (3*max_dx))
	{ max_dx = (rdx + 2*max_dx)/3.0; }
	//
	// Do it AGAIN, but this time scale the deltas and check
	// against the threshhold, for column count.
	//
	int col = 1;

	if (CReturn::Debug()>=1)
	{
		str.Format("-- %f (t %f)-------------------------", thresh, tropism);
		ret.Diagnostic( str );
	}

	left = DBL_MIN;
	prev_x = UNDEFINED;
	while (true)
	{
		double min_x = DBL_MAX;

		for (int j=0; j<x_array.Count(); j++)
		{
			double x= x_array[j];

			if ( (x > left)
				&& (x < (min_x-SMALL)) )
			{
				min_x = x;
			}
		}

		if (min_x < DBL_MAX)
		{
			if (DEFINED(prev_x))
			{
				double dx = (int)(((min_x - prev_x)/max_dx) * 100.0) / 100.0;

				if (dx >= thresh)
				{
					col++;
					prev_x = min_x;
				}

				if (CReturn::Debug()>=1)
				{
					str.Format( "   (%f)", dx );
					ret.Diagnostic(str);
				}
			}
			else
			{
				prev_x = min_x;
			}
			left = min_x + SMALL;

			if (CReturn::Debug()>=1)
			{
				str.Format( "%f", min_x );
				ret.Diagnostic(str);
			}

		}
		else
		{ break; }
	}
	//
	//
//	x_array.DestructiveFlush();

	if (CReturn::Debug()>=1)
	{
		str.Format( "%d Columns", col);
		ret.Diagnostic(str);
	}

	return col;
}


