// ==================================================================
//		PathFinder
//
//	Given an array of points (which may have arbitrary references
//	in them, for accounting purposes) -- find a short path that
//	touches all of them.
//
//	See PathFinder.h for more details.
//
// ==================================================================

#include "stdafx.h"

#include <float.h>
#include "2dBox.h"
#include "PathFinder.h"

// ==================================================================

CPathFinder::CPathFinder( 
	ePathMode mode )
{
	node_defaults( mode );
}

CPathFinder::~CPathFinder( void )
{
	Reset();
}

CReturn
CPathFinder::Reset( void )
{
	CReturn ret;

	m_city_array.DestructiveFlush();
	m_node_list.DestructiveFlush();
	m_user_list.DestructiveFlush();

	return ret;
}

// ==================================================================
CReturn	
CPathFinder::CityAppend( 
	const C2dCoord&	city_pt, 
	void*			ref )
{
	CReturn ret;

	CPathCity*	new_city = new CPathCity;

	new_city->XY( city_pt.X(), city_pt.Y() );

	new_city->m_reference = ref;
	new_city->m_latched = FALSE;

	m_city_array.Append( new_city );

	return ret;
}


// ==================================================================
CReturn	
CPathFinder::CityPrepare( void )
{
	CReturn ret;

	if (!CityCount() )
	{
		ret.setStatus( STATUS_ERROR );
		return ret;
	}

	// ---------------------------------
	// Get city extents
	//	Note: this *could* be done during city creation
	//
	C2dBox	extent;
	int idx;
	for (idx=0; idx<CityCount(); idx++)
	{
		CPathCity* city = m_city_array[idx];

		extent.X( city->X() );
		extent.Y( city->Y() );
	}

	//
	// Calculate the normalization factors
	//
	C2dCoord ctr( extent.Xc(), extent.Yc() );
	double dx = extent.Dx() / 2.0;
	double dy = extent.Dy() / 2.0;

	m_scale = max( dx, dy );

	//
	// Now, normalize the cities into a +- unit grid
	//
	for (idx=0; idx<CityCount(); idx++)
	{
		CPathCity* city = m_city_array[idx];

		city->X( (city->X() - ctr.X()) / m_scale );
		city->Y( (city->Y() - ctr.Y()) / m_scale );
	}

	return ret;
}


// ==================================================================
CReturn	
CPathFinder::NodeAppend( 
	const C2dCoord&	node_pt )
{
	CReturn ret;

	if (!CityCount())
	{
		ret.setStatus( STATUS_ERROR );
		return ret;
	}

	// ---------------------------------

	CPathNode*	new_node = new CPathNode;

	new_node->XY( node_pt.X() / m_scale, node_pt.Y() / m_scale );

	m_user_list.Append( new_node );

	return ret;
}

// ==================================================================
CReturn	
CPathFinder::NodePrepare( 
	int			node_num )	// Ignored for PRESET
{
	CReturn ret;

	if (!CityCount())
	{
		ret.setStatus( STATUS_ERROR );
		return ret;
	}

	// ---------------------------------

	if (m_mode == PATH_PRESET)
	{
		if (!NodeCount())
			ret.setStatus( STATUS_ERROR );

		node_preset();
	}
	else
	{
		int num = node_num;
		if (!num)
		{
			num = CityCount();
			if (m_mode == PATH_CLOSEST)
				num >>= 1;
		}

		int	sgn_x = -1;
		int sgn_y = -1;
		CPathNode* first_node = NULL;
		if (m_user_list.Count())
		{
			first_node = m_user_list[0];
			sgn_x = SGN( first_node->X() );
			sgn_y = SGN( first_node->Y() );
		}

		switch (m_mode)
		{
			case PATH_CLOSEST:
				node_circle( num );
				if (first_node)
					node_sort( *first_node );
				break;

			case PATH_ZIG_X:
				node_zig_x( num, sgn_x, sgn_y );
				break;

			case PATH_ZAG_Y:
				node_zag_y( num, sgn_x, sgn_y );
				break;
		}

		if (first_node)
			node_latch();
	}

	return ret;
}


// ==================================================================

void
CPathFinder::node_defaults( 
	ePathMode	mode )
{
	m_mode = mode;

	m_c_attract = PATH_DEFAULT_CATTRACT;
	m_n_attract = PATH_DEFAULT_NATTRACT;
	m_n_tension = PATH_DEFAULT_TENSION;

	m_scope = PATH_DEFAULT_SCOPE;
	m_die = PATH_DEFAULT_DIE;
	m_grow = PATH_DEFAULT_GROW;

	switch (mode)
	{
		case PATH_PRESET:
			m_zignum = 0;
			break;

		case PATH_CLOSEST:
			m_radius = PATH_CIRCLE_RADIUS;
			m_zignum = 0;
			break;

		case PATH_ZIG_X:
		case PATH_ZAG_Y:
			m_radius = PATH_ZIG_RADIUS;
			m_zignum = PATH_DEFAULT_ZIG;
			break;
	}
}

// ==================================================================
void	
CPathFinder::node_preset( void )
{
	//
	// Transfer the user nodes to the actual node list
	//
	int num = NodeCount();
	for (int idx=0; idx<num; idx++)
	{
		CPathNode* node = m_user_list[idx];

		node->m_vitality = 0.0;

		m_node_list.Append( node );
	}

	m_user_list.BenignFlush();
}

// ==================================================================
void	
CPathFinder::node_circle( 
	int num )
{
	//
	// Create a circle of nodes
	//
	double ang_step = TWOPI / num;
	double ang_at = 0.0;

	for (int idx=0; idx<num; idx++)
	{
		CPathNode* new_node = new CPathNode;

		new_node->XY( m_radius * cos(ang_at),
					  m_radius * sin(ang_at) );
		new_node->m_vitality = 0.0;

		m_node_list.Append(new_node);

		ang_at += ang_step;
	}
}

// ==================================================================
void	
CPathFinder::node_zig_x( 
	int		num, 
	int		start_x, 
	int		start_y )
{
	node_zag_y( num, start_y, start_x );

	//
	// Now, rotate for X zigging
	//
	num = NodeCount();
	for (int idx=0; idx<num; idx++)
	{
		CPathNode* node = m_node_list[idx];

		double x = node->X();
		double y = node->Y();

		node->XY( y, x );
	}
}

// ==================================================================
void	
CPathFinder::node_zag_y( 
	int		num, 
	int		start_x, 
	int		start_y )
{
	//
	// Wind the nodes back and forth;
	//	 Multiple passes in X, one in Y
	//
	// First, get our values aligned.
	//	Will round num up to a multiple of zigzagnum
	//
	int zigzagnum = m_zignum * 2;

	int nodes_per_zig = ((num + zigzagnum-1) / zigzagnum);
	if (nodes_per_zig < 2)
		nodes_per_zig = 2;
	num =  nodes_per_zig * zigzagnum;

	double at_x = start_x * m_radius;

	double diam = m_radius * 2.0;
	double step_x = -start_x * (diam / (zigzagnum-1));
	double step_y = -start_y * (diam / (nodes_per_zig-1));

	//
	// ZigZag...
	//
	int cnt_x, cnt_y;
	for (cnt_x=0; cnt_x<m_zignum; cnt_x++)
	{
		double at_y = start_y * m_radius;
		for (cnt_y=0; cnt_y<nodes_per_zig; cnt_y++)
		{
			CPathNode* new_node = new CPathNode;

			new_node->XY( at_x, at_y );
			new_node->m_vitality = 0.0;

			m_node_list.Append(new_node);

			at_y += step_y;
		}
		at_x += step_x;

		at_y = -start_y * m_radius;
		for (cnt_y=0; cnt_y<nodes_per_zig; cnt_y++)
		{
			CPathNode* new_node = new CPathNode;

			new_node->XY( at_x, at_y );
			new_node->m_vitality = 0.0;

			m_node_list.Append(new_node);

			at_y -= step_y;
		}
		at_x += step_x;
	}
}


// ==================================================================
void	
CPathFinder::node_sort( 
	const CPathNode&	anchor )
{
	//
	// Find the node closest to the anchor...
	//
	int		close_idx = -1;
	double	close = DBL_MAX;
	
	int num = NodeCount();
	for (int idx=0; idx<num; idx++)
	{
		CPathNode* node = m_node_list[idx];

		double dist2 = anchor.Dist2( *node );
		if (dist2 < close)
		{
			close = dist2;
			close_idx = idx;
		}
	}

	//
	// .. and make it the head of the list
	//
	while (--close_idx >= 0)
	{
		CPathNode* node = m_node_list[0];
		m_node_list.Append( node );
		m_node_list.Remove( 0 );
	}
}

// ==================================================================
//	node_latch is mostly experimental -- I don't know if we
//	ever want to use it for anything other than specifiying a start
//	point... if even that
//
void	
CPathFinder::node_latch( void )
{
	//
	// For each user node, find the closest
	// preset node and replace the preset with the user node...
	//
	int node_num = m_node_list.Count();
	int user_num = m_user_list.Count();
	for (int user_idx=0; user_idx<user_num; user_idx++)
	{
		CPathNode* user_node = m_user_list[user_idx];

		int		close_idx = -1;
		double	close = DBL_MAX;

		for (int node_idx=0; node_idx<node_num; node_idx++)
		{
			CPathNode* node = m_node_list[node_idx];

			double dist2 = user_node->Dist2( *node );
			if (dist2 < close)
			{
				close = dist2;
				close_idx = node_idx;
			}
		}

		if (close_idx >= 0)
		{
			CPathNode* node = m_node_list[close_idx];

			node->XY( user_node->X(), user_node->Y() );
		}
	}

	m_user_list.DestructiveFlush();
}

// ==================================================================
void	
CPathFinder::node_kick( void )
{
	double jitter = PATH_TERMINAL_DIST;

	int n_num = NodeCount();

	CPathNode* node = NULL;
	for (int n_idx=0; n_idx<n_num; n_idx++)
	{
		node = m_node_list[n_idx];

		double jx = (((double)rand() / (double)RAND_MAX/2.0)-1.0) * jitter;
		double jy = (((double)rand() / (double)RAND_MAX/2.0)-1.0) * jitter;

		node->X( node->X() + jx);
		node->Y( node->Y() + jy);
	}
}

// ==================================================================
void	
CPathFinder::node_growdie( void )
{
	CPathNode* node = NULL;

	int n_idx, n_num = NodeCount();
	for (n_idx=0; n_idx<n_num; n_idx++)
	{
		node = m_node_list[n_idx];
		if (node->m_vitality < m_die)
		{
			// Delete node here
			m_node_list.Remove( n_idx );
			delete node;

			n_idx--;
			n_num--;
		}
	}

	n_num = NodeCount();
	for (n_idx=0; n_idx<n_num; n_idx++)
	{
		node = m_node_list[n_idx];

		if (node->m_vitality > m_grow)
		{
			// Insert node here
			CPathNode* new_node = new CPathNode;

			int prev;
			int next;
			if (m_zignum)
			{
				prev = ((n_idx>0)?n_idx-1:n_idx);
				next = ((n_idx+1)<n_num)?n_idx+1:n_idx;
			}
			else
			{
				prev = ((n_idx>0)?n_idx:n_num)-1;
				next = ((n_idx+1)<n_num)?n_idx+1:0;
			}

			CPathNode* p_node = m_node_list[prev];
			CPathNode* n_node = m_node_list[next];

			double pdist = p_node->Dist2( *node );
			double ndist = n_node->Dist2( *node );

			double jitter = PATH_TERMINAL_DIST;
			double jx = ((double)rand() / (double)RAND_MAX) * jitter;
			double jy = ((double)rand() / (double)RAND_MAX) * jitter;


			if (pdist > ndist)
			{
				node->m_vitality = 
				new_node->m_vitality = 0.0;


				new_node->X( node->X() 
							- jx*( node->X() - 2*p_node->X() + n_node->X() ) );
				new_node->Y( node->Y() 
							- jy*( node->Y() - 2*p_node->Y() + n_node->Y() ) );

				m_node_list.InsertBefore( n_idx, new_node );

				node->X( node->X()
							- jx*( node->X() - 2*n_node->X() + p_node->X() ) );
				node->Y( node->Y()
							- jy*( node->Y() - 2*n_node->Y() + p_node->Y() ) );
			}
			else
			{
				node->m_vitality = 
				new_node->m_vitality = 0.0;

				new_node->X( node->X() 
							- jx*( node->X() - 2*n_node->X() + p_node->X() ) );
				new_node->Y( node->Y() 
							- jy*( node->Y() - 2*n_node->Y() + p_node->Y() ) );

				m_node_list.InsertAfter( n_idx, new_node );

				node->X( node->X()
							- jx*( node->X() - 2*p_node->X() + n_node->X() ) );
				node->Y( node->Y()
							- jy*( node->Y() - 2*p_node->Y() + n_node->Y() ) );
			}

			n_idx++;
			n_num++;
		}
	}
}


// ==================================================================
//	the snap to city phase is not necessary -- we have our path
//	ordering already, so who cares if the nodes sit on the cities or
//	not?
//
//	If we want to get the path length, maybe we should move the nodes
//	to sit right on the cities, but until then, this can be a dummy
//	function.
//
void	
CPathFinder::node_snaptocity( void )
{
	return;
}


// ==================================================================
CReturn CPathFinder::Evaluate( CglCanvas* canvas )
{
	CReturn ret;

	if ( !CityCount() || !NodeCount() )
	{
		ret.setStatus( STATUS_ERROR );
		return ret;
	}

//	if (canvas)
//		debug_draw( canvas );
	//
	// Main loop -- crank some numbers!
	//
	int boredom = 0;
	double worst_distance = 0.0;
	do
	{
		int c_idx, c_num = CityCount();
		int n_idx, n_num = NodeCount();

		//
		// Are we bored yet?
		//
		boredom++;
		if (boredom > PATH_BOREDOM_KICK)
		{
			node_kick();
			boredom -= PATH_BOREDOM_EASE;
		}

		//
		// Reset working values for this pass 
		//
		CPathNode* node = NULL;
		for (n_idx=0; n_idx<n_num; n_idx++)
		{
			node = m_node_list[n_idx];

			node->m_cmin = NULL;
			node->m_cdist = DBL_MAX;
		}

		CPathCity* city;
		for (c_idx=0; c_idx<c_num; c_idx++)
		{
			city = m_city_array[c_idx];

			city->m_nmin = NULL;
			city->m_ndist = DBL_MAX;
			city->m_latched = FALSE;
		}


		//
		// Now, pair closest cities and nodes together...
		//
		double dist2;
		for (n_idx=0; n_idx<n_num; n_idx++)
		{
			node = m_node_list[n_idx];

			for (c_idx=0; c_idx<c_num; c_idx++)
			{
				city = m_city_array[c_idx];

				dist2 = node->Dist2( *city );

				if (dist2 < city->m_ndist)
				{
					city->m_nmin = node;
					city->m_ndist = dist2;
				}

				if (dist2 < node->m_cdist)
				{
					node->m_cmin = city;
					node->m_cdist = dist2;
				}
			}
		}

		//
		// For each node, move towards their closest city.
		//
		double suck;
		for (n_idx=0; n_idx<n_num; n_idx++)
		{
			node = m_node_list[n_idx];

			if (node->m_cmin)
			{
				city = node->m_cmin;

				city->m_latched = TRUE;

				suck = m_n_attract;

				node->m_step.X( (city->X() - node->X()) * suck );
				node->m_step.Y( (city->Y() - node->Y()) * suck );
				node->m_factor = suck;
			}
			else
			{
				node->m_step.XY( 0.0, 0.0 );
				node->m_factor = 0.0;
			}
		}

		//
		// For each city, move towards their favored node
		//	(also, determine our distance error)
		// TODO:  Move worst_distance into the application loop...
		//
		worst_distance = 0.0;
		for (c_idx=0; c_idx<c_num; c_idx++)
		{
			city = m_city_array[c_idx];

			if (city->m_nmin)
			{
				node = city->m_nmin;

				node->m_vitality += 1.0;

				suck = m_c_attract;
				if (!city->m_latched)
					suck *= 2;

				node->m_step.X( node->m_step.X() + ((city->X() - node->X()) * suck) );
				node->m_step.Y( node->m_step.Y() + ((city->Y() - node->Y()) * suck) );
				node->m_factor += suck;

				worst_distance = max( worst_distance, city->m_ndist );
			}
		}


		//
		// Apply the attractions and surface tension
		//
		double dist = 0.0;
		double dx;
		double dy;

		node = m_node_list[0];

		int prev = (m_zignum)?0:n_num-1;
		CPathNode* p_node = m_node_list[prev];

		CPathNode* n_node = NULL;
		int next;
		double tropx = (m_mode == PATH_ZIG_X?PATH_TROPISM_FACTOR:1.0);
		double tropy = (m_mode == PATH_ZAG_Y?PATH_TROPISM_FACTOR:1.0);
		for (n_idx=0; n_idx<n_num; n_idx++)
		{
			if (m_zignum)
				next = ((n_idx+1)<n_num)?n_idx+1:n_idx;
			else
				next = ((n_idx+1)<n_num)?n_idx+1:0;
			n_node = m_node_list[next];

			node->m_vitality -= 1.0;

			if (node->m_factor > SMALL)
			{
				dx = (node->m_step.X() / (double)node->m_factor)*tropx;
				dy = (node->m_step.Y() / (double)node->m_factor)*tropy;
			}
			else
				dx = dy = 0.0;

			if (m_scope > .1)
			{
				dx += m_scope * m_n_tension * (p_node->X() - 2.0*node->X() + n_node->X());
				dy += m_scope * m_n_tension * (p_node->Y() - 2.0*node->Y() + n_node->Y());
			}

			node->X( node->X() + dx);
			node->Y( node->Y() + dy);

			// For reporting purposes only
			dist += sqrt(node->Dist2( *n_node ));

			// Next!
			p_node = node;
			node = n_node;
		}

		node_growdie();

		m_scope = (m_scope + worst_distance/2.0) / 2.0;

//		if (canvas)
//			debug_draw( canvas );

	} while (sqrt(worst_distance) > PATH_TERMINAL_DIST );

	node_snaptocity();

	return ret;
}



//=============================================================================
/*
void
CPathFinder::debug_draw( 
	CglCanvas* canvas )
{
	canvas->Clear( C2dBox( -1.05, -1.05, 1.05, 1.05 ) );

	double delta = 4.0 * canvas->Scale();

	//
	// Cities...
	//
	canvas->Color( CglColor( 0.25, 0.75, 0.75 ) );

	int num = CityCount();
	canvas->QuadOpen();
	for (int idx=0; idx<num; idx++)
	{
		CPathCity* city = m_city_array[idx];

		canvas->Quad(	XY( city->X(), city->Y() - delta ),
						XY( city->X() + delta, city->Y() ),
						XY( city->X(), city->Y() + delta ),
						XY( city->X() - delta, city->Y() ) );

	}
	canvas->Close();

	//
	// Nodes...
	//
	canvas->Color( CglColor( 1.00, 0.25, 0.25 ) );

	num = NodeCount();
	CPathNode* prev_node = NULL;
	canvas->LineOpen();
	for (idx=0; idx<num; idx++)
	{
		CPathNode* node = m_node_list[idx];

		canvas->Line(	XY( node->X()-delta, node->Y() ),
						XY( node->X()+delta, node->Y() ) );
		canvas->Line(	XY( node->X(), node->Y()-delta ),
						XY( node->X(), node->Y()+delta) );

		if (prev_node)
		{
			canvas->Line(	XY( prev_node->X(), prev_node->Y() ),
							XY( node->X(), node->Y() ) );
		}

		prev_node = node;
	}

	canvas->Close();

	canvas->Show();

	Sleep( 500 );
}

*/

#if TEST_CODE_HERE

#include "PathFinder.h"
#include "DbHole.h"
CReturn
CNestProcessApp::test( CCommand* io_cmd )
{
	CReturn ret;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	CPathFinder tsp( PATH_ZAG_Y );

	CSelector select(model);
	select.All( 0 );
	select.Filter( DBHOLE, 1 );
	select.SelectAll( TRUE );

	int num = select.Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbHole* hole = (CDbHole*)select[idx];

		tsp.CityAppend( hole->Coord(0), hole );
	}
	select.Clear();
	tsp.CityPrepare();

	tsp.NodeAppend( C2dCoord( -1, 1 ) );
	tsp.NodeZigNum( 5 );
	tsp.NodePrepare( 60 );

	tsp.Evaluate( view.GetCanvas() );

	view.KillCanvas();

	return ret;
}


#endif
