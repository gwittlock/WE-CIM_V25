
#include "stdafx.h"
#include "StringConst.h"
#include "Register.h"

#include "SeqRules.h"

#define FUTURE 0

#define FACTOR_FACTOR		0.1		// Relative Cost of motion per/unit

// The higher the STICKY_FACTOR, the closer the points stay on trend.
// TODO: This value should be user adjustable [1 .. 100]
double STICKY_FACTOR = 4;


////////////////////////////////////////////////////////////////////////

CSeqRules::CSeqRules()
	: extents()
{
}

CSeqRules::~CSeqRules()
{
}

void
CSeqRules::Init( CModel* model, const CVarList& seqAttribs )
{
	double	xmin, ymin;
	double	xmax, ymax;
	int		optAlg;

	m_model = model;

#if BEFORE_V18
	tsp_opt		= seqAttribs.getInt( "adv", FALSE );

	optAlg	= seqAttribs.getInt( "optalgorithm", -1 );
	switch (optAlg)
	{
		case -1:	// NO optimization
			path_opt = FALSE;
			grid_opt = FALSE;
			tsp_opt = FALSE;
			break;

		case 2:		// Progressive
			path_opt = FALSE;
			grid_opt = TRUE;
			tsp_opt = TRUE;
			break;

		case 3:		// Closest Point
			path_opt = TRUE;
			grid_opt = FALSE;
			break;

		default:	// Default, otherwise, to Grid
			path_opt = FALSE;
			grid_opt = TRUE;
			break;
	}
#else
	// (-1) No Opt / (1) Grid / (2) Progressive / (3) Closest Pt
	optAlg = seqAttribs.getInt( "optalgorithm", -1 );

	// Flag the early incarnation of closest pt.
	path_opt = FALSE;

	// Flag the new incarnation of closest pt.
	tsp_opt = (optAlg == 3);

	// Flag grid optimization because we always want to honor
	// the sorting order (ie. part interior-to-exterior).
	grid_opt = (optAlg >= 0);
#endif

	if (CRegister::IntGetV( "CodeGeneration", "disable_grid", 0 ) == 1)
	{	
		grid_opt = FALSE;
		path_opt = TRUE;
	}

	if (CRegister::IntGetV( "CodeGeneration", "cut_avoid", 0 ) == 1)
	{ cut_avoid = TRUE; }
	else
	{ cut_avoid = FALSE; }

	avoid_min = CRegister::DoubleGetV( "CodeGeneration", "avoid_min", 0. );

	gridSeqOption	= (ESeqProcess) seqAttribs.getInt( "seq_opt",	SEQUENCE_UNDEFINED );

	scribe_with_torch = (model->Header().getInt("ScribeWithTorch", 0) == 1);


	trend = (ESeqTrend) seqAttribs.getInt( "slice_opt", -1);
	bidirectional = (seqAttribs.getInt( "bidir", TRUE ) != FALSE);

	linear_trend = seqAttribs.getInt( "sticky", 4 );
	STICKY_FACTOR = (double)linear_trend;
	if (STICKY_FACTOR < 1.)
		STICKY_FACTOR = 1.;

	xmin = seqAttribs.getReal( STR_XMIN, 0. );
	xmax = seqAttribs.getReal( STR_XMAX, 0. );
	ymin = seqAttribs.getReal( STR_YMIN, 0. );
	ymax = seqAttribs.getReal( STR_YMAX, 0. );

	if (tsp_opt)
	{
		// NOTE: The user may *not* have selected a location.
		double xs = seqAttribs.getReal( "xs", UNDEFINED );
		double ys = seqAttribs.getReal( "ys", UNDEFINED );
		m_anchor.XYZ( xs, ys, 0. );
	}
	else
	{
		// NOTE: m_anchor is set base on the trend.
		WorkPkgSet( C2dBox( xmin, ymin, xmax, ymax ) );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Setup from registry... here instead of main, more is defined (?)
	flag = 0;
	flag |= PATH_RULE_XDIST_FACTOR;
	flag |= PATH_RULE_YDIST_FACTOR;

#if FUTURE
	// TODO: Add penalty for backing-up.
	flag |= PATH_RULE_TREND;
#endif

	delta_x_factor = FACTOR_FACTOR;
	delta_y_factor = FACTOR_FACTOR;

	switch ( trend )
	{
	case LL_XPOS:
	case LR_XNEG:
	case UL_XPOS:
	case UR_XNEG:
		// reducing the weight yeilds a lower score, which is better.
		delta_x_factor /= STICKY_FACTOR;
		break;
	case LL_YPOS:
	case LR_YPOS:
	case UL_YNEG:
	case UR_YNEG:
		// reducing the weight yeilds a lower score, which is better.
		delta_y_factor /= STICKY_FACTOR;
		break;
	default:
		break;
	}

	forward = TRUE;

	// Cut-Order mode
	fixed_chunks = FALSE;
	m_pass_num = 0;

	drop_cnt = 0;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	mode = (ESeqMode) seqAttribs.getInt( "mode", PATH_TOOL );

	switch ( mode )
	{
	case PATH_MIXED:	// Mixed path
		bore = TRUE;
		route = TRUE;

		flag |= PATH_RULE_DROP_MIN;
		drop_min = 3;	// TODO:  parameterize

		m_pass_num = 1;
		fixed_chunks = FALSE;

		toolOrder = TOOL_ORDER_BY_ENCOUNTER;
		break;

	case PATH_BORE:	// Bore first
		bore = TRUE;
		route = FALSE;

		m_pass_num = 2;
		fixed_chunks = FALSE;

		toolOrder = TOOL_ORDER_BY_JOB;
		break;

	case PATH_ROUTE:	// Router first
		bore = FALSE;
		route = TRUE;

		m_pass_num = 2;
		fixed_chunks = FALSE;

		toolOrder = TOOL_ORDER_BY_JOB;
		break;

	case PATH_TOOL:		// By tool order
		bore = TRUE;
		route = TRUE;

		m_pass_num = 0;  // count of tools (set by using PassCountSet())
		fixed_chunks = FALSE;

		toolOrder = TOOL_ORDER_BY_JOB;
		break;

	case PATH_KEEP:		// As-Is chunk order, untouched routes
		bore = TRUE;
		route = TRUE;

		m_pass_num = 1;
		fixed_chunks = TRUE;

		toolOrder = TOOL_ORDER_BY_ENCOUNTER;
		break;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// "top" only optimzation, or edges too?  And which order?
	edges = seqAttribs.getInt( "edges", 1 );
	switch ( edges )
	{
	case 0:	// Mixed-in
		break;

	case 1:	// First
		face_1 = FALSE;
		break;

	case 2:	// Last
		face_1 = TRUE;
		break;
	}

	// Other static initialization
	tool_id = -1;
	face_id = 1;

//	TODO: Add in toolchange cost
//	flag |= PATH_RULE_TCHG_COST;
//	toolchange_cost = xxxx;
	
	y_origin = seqAttribs.getReal( "YOrigin", 0. );	// for drill drop optimization

	m_currTool = NULL;

	if (gridSeqOption == SEQUENCE_BY_TOOL_ORDER)
	{
		// map to default behavior
		// (assuming laser only operations -- ie. slitting nest and cutting parts)

		AllHolesFirst = TRUE;
		HolesByToolOrder = TRUE;
		HolesAcrossLocalNest = FALSE;
		ByCompletePart = BY_RECURSION;
	}
	else if (gridSeqOption == SEQUENCE_BY_CONTAINMENT_ORDER)
	{
		// map to default behavior
		gridSeqOption = SEQUENCE_BY_FLOW_CHART;
		AllHolesFirst = TRUE;
		HolesByToolOrder = TRUE;
		HolesAcrossLocalNest = FALSE;
		ByCompletePart = BY_LOCAL_CONTAINMENT;
	}
	else if (gridSeqOption == SEQUENCE_BY_FLOW_CHART)
	{
		AllHolesFirst = (seqAttribs.getInt( "AllHolesFirst", FALSE ) != FALSE);
		HolesByToolOrder = (seqAttribs.getInt( "HolesByToolOrder", TRUE ) != FALSE);
		HolesAcrossLocalNest = (seqAttribs.getInt( "HolesAcrossLocalNest", FALSE ) != FALSE);
		ByCompletePart = (ECTIterMethod) seqAttribs.getInt( "ByCompletePart", BY_LOCAL_CONTAINMENT );
	}
	//
	//
	if (optAlg == 2)
	{ gridSeqOption = SEQUENCE_BY_PROGRESSIVE; }

	if (gridSeqOption == SEQUENCE_BY_PROGRESSIVE)
	{
		mode = (ESeqMode)-1;
	}
}

void
CSeqRules::WorkPkgSet( const C2dBox& box )
{
	double	stx, sty;

	extents = box;

	stx = 0.;
	sty = 0.;

	switch ( trend )
	{
	case LL_XPOS:
	case LL_YPOS:
		stx = extents.Xmin();
		sty = extents.Ymin();
		break;
	case LR_XNEG:
	case LR_YPOS:
		stx = extents.Xmax();
		sty = extents.Ymin();
		break;
	case UL_XPOS:
	case UL_YNEG:
		stx = extents.Xmin();
		sty = extents.Ymax();
		break;
	case UR_XNEG:
	case UR_YNEG:
		stx = extents.Xmax();
		sty = extents.Ymax();
		break;
	default:
		break;
	}

	m_anchor = C3dCoord( stx, sty, 0.0 );
	m_buoy = m_anchor;
	TrendSet( TRUE );

#if REQUIRED  // appears to hose traversal by yielding incorrect start point
	model.EntityFind( STR_TOP, (CDbEntity**)&top, DBWORKPLANE, DBWORKPLANE );
	if (top != NULL)
		top->Transform().Transform( &seqRules->m_anchor );
#endif
}

void
CSeqRules::TrendSet( bool forward_trend )
{
	forward = forward_trend;

	switch (trend)
	{
	case LL_XPOS:
	case UL_XPOS:
		if ( forward )
		{
			m_buoy.X( extents.Xmin() );
			trend_vec.Init( 1, 0 );
		}
		else
		{
			m_buoy.X( extents.Xmax() );
			trend_vec.Init( -1, 0 );
		}
		break;
	case LR_XNEG:
	case UR_XNEG:
		if ( forward )
		{
			m_buoy.X( extents.Xmax() );
			trend_vec.Init( -1, 0 );
		}
		else
		{
			m_buoy.X( extents.Xmin() );
			trend_vec.Init( 1, 0 );
		}
		break;
	case LL_YPOS:
	case LR_YPOS:
		if ( forward )
		{
			m_buoy.Y( extents.Ymin() );
			trend_vec.Init( 0, 1 );
		}
		else
		{
			m_buoy.Y( extents.Ymax() );
			trend_vec.Init( 0, -1 );
		}
		break;
	case UR_YNEG:
	case UL_YNEG:
		if ( forward )
		{
			m_buoy.Y( extents.Ymax() );
			trend_vec.Init( 0, -1 );
		}
		else
		{
			m_buoy.Y( extents.Ymin() );
			trend_vec.Init( 0, 1 );
		}
		break;
	default:
		break;
	}
}
