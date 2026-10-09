
#include "stdafx.h"
#include <float.h>
#include "StringConst.h"
#include "Register.h"
#include "2dBox.h"

#include "Profile.h"

#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbEntity.h"
#include "DbCurve.h"
#include "DbHole.h"
#include "DbCommand.h"

#include "CodeUtil.h"

#include "Model.h"

#include "TSP.h"
#include "AStar.h"
#include "PrimsTSP2.h"

#include "ShortestPath.h"

// See also StdAfx.h for notes on _CRTDBG_MAP_ALLOC
#ifndef _CRTDBG_MAP_ALLOC
#include <malloc.h>
#endif

#include <errno.h>
////////////////////////////////////////////////////////////////////////

const int TOOL_PREFERENCE = 10;
const double MIXED_DROP_FACTOR = 0.5;
const double FACE_INERTIA = 10;

// TODO:  Use real gap tolerance from the user
#define MAX_GAP				SMALL

#if REQUIRED
	class Ccpr
	{
	public:

		Ccpr( double score, int low, int hi )
		{
			m_score = score;
			m_low = low;
			m_hi = hi;
		}

		double Score()	{ return m_score; }
		int Low()		{ return m_low; }
		int Hi()		{ return m_hi; }

		~Ccpr()
		{
		}

	public:

		static int QSortCompare( const void* ptrA, const void* ptrB )
		{
			Ccpr* objA = (*(Ccpr**) ptrA);
			Ccpr* objB = (*(Ccpr**) ptrB);

			return SGN(objA->m_score - objB->m_score);
		}

	private:

		double	m_score;
		int		m_low;
		int		m_hi;
	};
#endif

////////////////////////////////////////////////////////////////////////

CShortestPath::CShortestPath()
{
	// See also CCodeGenProcessApp::Generate() where the value
	// is set in the registry and CShortestPath::OptimizePath()
	// to see how the value is used.

	m_group_hits = CRegister::BoolGetV( "CodeGeneration", "group_hits", false );
}

CShortestPath::~CShortestPath()
{
#if REQUIRED
	m_cpr.DestructiveFlush();
#endif
}

// ==================================================================
//		OptimizePath
//
//		Re-order the elements, using a simple closest-point arrangement.
//
//	ASSUMES that the source list has been exploded: eg, it contains ONLY
//	low-level geometry.  Line, Arc, Hole, and Point.
//
//	Sorts these into the (allocated) destination list in some semblance of
//	efficient order... 
//
//	Command Parameters are:
//		stx=%g			Start X
//		sty=%g			Start Y
//		mode=%d			Ordering mode; 0=Mixed Path, 1=Bore 1st, 2=Route 1st, 3=as is
//		edges=%d		Face ordering?  0=mixed, 1=top first, 2=top last
//
CReturn
CShortestPath::OptimizePath( 
	CSeqRules*				seqRules,		// Package of Parameters
	const CDbEntityArray&	tool_list,		// Can be NULL unless mode is PATH_TOOL
	const CDbEntityArray&	src_list,		// List of geometry references
	CDbEntityArray*			dst_list )		// destination, in cut-order
{
	CReturn			ret;
	CDbEntity*		db_ent;
	CDbCommand*		dbCommand;
	CDbEntityArray	tmp_list;
	CDbEntityArray	sorted_hits;
	C3dCoord		tsp_seed_pt;
	int				count, indx;
	bool			okay;

	double TOL = 1.e-4;
	double dot;
	C3dCoord	pt;
	C3dVec		vec;

	int src_num = src_list.Count();
	if (src_num <= 0)
		return ret;  // early exit.

	// Clear source tags and fill the intermediate entity list.
	CDbEntity::NewAction();
	if (seqRules->mode != PATH_TOOL)
	{
		for (int idx=0; idx<src_num; idx++)
		{
			db_ent = src_list[idx];
			tmp_list.Append( db_ent );
		}
	}

	// Rarely used ... but harmless.
	tsp_seed_pt = seqRules->Anchor();

	// -------------------------------------------------------
	//	Multiple passes.  If PATH_MIXED, allow both bore and route
	//	otherwise, alternate bore and route.  TODO: PATH_AS_IS
	//
	int	chunk_st = 0;
	int chunk_en = 0;

	int	prof_st = 0;
	int prof_en = 0;

	for (int pass_cnt = 0; pass_cnt < seqRules->PassCountGet(); pass_cnt++)
	{
		// -------------------------------------------------------
		//	Process each element in turn... until we run out of
		//	places to go
		//
		int src_idx = 0;

		if (seqRules->mode == PATH_TOOL)
		{
			src_num = src_list.Count();
			seqRules->tool_id = tool_list[pass_cnt]->Id();
		}

		while (TRUE)
		{
			if (src_idx >= src_num)
				break;

			//
			// Determine our operating range...
			//
			if ( seqRules->fixed_chunks )
			{
				//
				// Describe a "chunk" of homogenous entity types
				//
				chunk_st = src_idx;
				db_ent = tmp_list[src_idx];
				CDbCurve* context = dynamic_cast<CDbCurve*>(db_ent);

				while (TRUE)
				{
					src_idx++;
					if (src_idx == src_num)
						break;

					db_ent = tmp_list[src_idx];
					CDbCurve* curve = dynamic_cast<CDbCurve*>(db_ent);
					if ((curve == NULL) != (context == NULL))
						break;
				}
				chunk_en = src_idx-1;
			}
			else if (seqRules->mode == PATH_TOOL)
			{
				CDbTool*	active_tool;
				bool		special_case;

				special_case = false;
				active_tool = dynamic_cast<CDbTool*>( tool_list[pass_cnt] );

				// See also CShortestPath::CShortestPath().
				if ( m_group_hits )
				{
					// Special behavior introduced for Cequent Towing.
					// Said behavior processes auto-indexing punch hit in
					// batches where the hits all have the same punch orientation.
					// This is, of course, not the most efficient way to process
					// the sheet, but it necessary because (according to Gary) the
					// head on their Whitney gets confused by frequent changes in
					// orientation.
					special_case = active_tool->IsIndexable();

					if (special_case && !active_tool->DidAction())
					{
						SortedHitsGet( active_tool, src_list, &sorted_hits );
						active_tool->DoAction();
					}
				}

				if ( special_case )
				{
					double	prev_orient, curr_orient, delta;

					tmp_list.BenignFlush();

					prev_orient = UNDEFINED;
					while (1)
					{
						if (sorted_hits.Count() < 1)
							break;

						db_ent = sorted_hits[0];

						curr_orient = db_ent->DoubleGet( "orient", -UNDEFINED );

						if (prev_orient < UNDEFINED)
						{
							delta = prev_orient - curr_orient;
							if (fabs( delta ) >= 1.e-2)  // arbitrary
								break;
						}

						// ie. remove db_ent from the list.
						sorted_hits.Remove(0);

						tmp_list.Append( db_ent );
						prev_orient = curr_orient;
					}

					chunk_st = 0;
					chunk_en = tmp_list.Count() - 1;

					if (sorted_hits.Count() > 0)
					{
						// We still have pending hits, so we must
						// fool the while(TRUE) loop into continuing.
						src_num = tmp_list.Count();
						src_idx = src_num - 1;
					}
					else
					{
						// Cause the while(TRUE) loop to terminate.
						src_idx = tmp_list.Count();
						src_num = src_idx;
					}
				}
				else
				{
					count = src_list.Count();
					for (indx = 0; indx < count; ++indx)
					{
						db_ent = src_list[indx];
						dbCommand = dynamic_cast<CDbCommand*>(db_ent);

						if (db_ent->Tool() == active_tool)
						{
							tmp_list.Append( db_ent );
						}
						else if (dbCommand != NULL)
						{
							if (dbCommand->IsInstance() || dbCommand->IsTooledText())
								tmp_list.Append( db_ent );
						}
					}
					chunk_st = 0;
					chunk_en = tmp_list.Count() - 1;
					src_idx = tmp_list.Count();
					src_num = src_idx;
				}
			}
			else
			{
				chunk_st = 0;
				chunk_en = src_num-1;
				src_idx = src_num;
			}

			if (seqRules->tsp_opt)
			{
#if ORIGINAL_CODE
				OptimizeTSP( seqRules, (CDbTool*)tool_list[pass_cnt], tmp_list, chunk_st, chunk_en, dst_list );

//				if (seqRules->cut_avoid)
//				{ CutAvoidance(dst_list); }
#else
				TIndexPairArray	spans;
				CIndexPair*		span;
				int				count, indx, jndx;

				GetSpans( tmp_list, &spans );

				count = spans.Count();
				if (count > 2)
				{
					// We reassign tsp_seed_pt so as to start the next
					// optimization cycle near the end of this cycle.
					tsp_seed_pt = TSP_Solve( tsp_seed_pt, tmp_list, &spans );
					
					seqRules->Anchor( tsp_seed_pt );
				}

				// This is bogus. Must find correct way to Transfer() ?
				for (indx = 0; indx < count; ++indx)
				{
					span = spans[indx];

					// NOTE: The span represents the inclusive range of indices.
					// Hence, we add 1 to the end of the span (for loop control).
					for (jndx = span->Start(); jndx < (span->End() + 1); ++jndx)
					{
						CDbEntity* db_ent = tmp_list[jndx];
						db_ent->DoAction();
						dst_list->Append( db_ent );
					}
				}
#endif
			}
			else // Original score-based optimization
			{
				while (1)
				{
					okay = find_cheapest_profile(
								tmp_list, (*seqRules), chunk_st, chunk_en, &prof_st, &prof_en );

					if ( !okay )
						break;

#if REQUIRED
					prof_st = m_cpr[0]->Low();
					prof_en = m_cpr[0]->Hi();
#endif
					db_ent = tmp_list[ prof_st ];

					if (seqRules->flag & PATH_RULE_TREND)
					{
						// FUTURE
						entity_pnt( db_ent, FALSE, &pt );

						vec = (pt - seqRules->Buoy());
						dot = vec.X() * seqRules->trend_vec.X() + vec.Y() * seqRules->trend_vec.Y();

						if ( seqRules->bidirectional )
						{
							if (dot < 0)
								seqRules->TrendSet( !seqRules->forward );
							else
								Transfer( db_ent, tmp_list, prof_st, prof_en, dst_list, seqRules );
						}
						else
						{
							if (dot <= 0)
								seqRules->TrendSet( TRUE );
							else
								Transfer( db_ent, tmp_list, prof_st, prof_en, dst_list, seqRules );
						}
					}
					else
					{
						// V15.1 & V16.0
						// NOTE: IsGoodTrend() always returns true when bidirection is true.
						if ( IsGoodTrend( (*seqRules), db_ent ) )
						{
							Transfer( db_ent, tmp_list, prof_st, prof_en, dst_list, seqRules );
						}
						else if ( !(seqRules->bidirectional) )
						{
							seqRules->TrendSet( TRUE );
						}
					}
				}
			}
		}

		if (seqRules->mode == PATH_TOOL)
		{
			tmp_list.BenignFlush();
		}
		else
		{
			seqRules->bore = !seqRules->bore;
			seqRules->route = !seqRules->route;
		}
	}

	return ret;
}

// ============================================================================
CReturn
CShortestPath::OptimizeTSP( 
		CSeqRules*			seqRules,		// Package of Parameters
		const CDbTool*		dbTool,
		CDbEntityArray&		src_list,
		int					chunk_st,
		int					chunk_en,
		CDbEntityArray*		dst_list )		// destination, in cut-order
{
	CReturn ret;
	CString str;
	if (CReturn::Debug() >= 2)
	{ 
		str.Format("OptimizeTSP.");
		ret.Diagnostic( str );
	}

	int	prof_st = 0;
	int prof_en = 0;
	//
	// Organize the operating range by closest path
	//	From chunk_st to chunk_en in src_list
	//
	C3dCoordArray* waypoint = new C3dCoordArray;
	C3dCoord* coord;
	prof_st = chunk_st;
	while (prof_st <= chunk_en)
	{
		prof_en = scan_profile(src_list, prof_st, MAX_GAP);

		CDbEntity* ent = src_list[prof_st];
		//
		//
		if ( (seqRules->mode != PATH_TOOL)
			|| (dbTool && (ent->Tool() == dbTool))
			|| !dbTool )
		{
			coord = new C3dCoord();
			entity_pnt(ent, false, coord);
			coord->Z(prof_st);
			waypoint->Append(coord);
		}

		prof_st = prof_en + 1;
	}

	if (waypoint->Count())
	{
		CTSP tsp;
		tsp.Init( seqRules, waypoint );
		waypoint->DestructiveFlush();

		tsp.Evaluate(waypoint, 1000);

		if (CReturn::Debug() >= 2)
		{ 
			str.Format("OptimizeTSP reorganize geometry.");
			ret.Diagnostic( str );
		}
		int num = waypoint->Count();
		for (int idx=0; idx<num; idx++)
		{
			C3dCoord* pt = (*waypoint)[idx];
			prof_st = (int)pt->Z();
			prof_en = scan_profile( src_list, prof_st, MAX_GAP );

			CDbEntity* db_ent = src_list[prof_st];
			Transfer( db_ent, src_list, prof_st, prof_en, dst_list, seqRules );

		}
	}
	waypoint->DestructiveFlush();
	delete waypoint;

	if (CReturn::Debug() >= 2)
	{ 
		str.Format("OptimizeTSP done.");
		ret.Diagnostic( str );
	}
	return ret;
}


// ============================================================================
//		find_cheapest_profile
//
//	Given the current anchor point and other path-traversal rules,
//	find the next profile to cut
//
bool
CShortestPath::find_cheapest_profile( 
	const CDbEntityArray&	list,
	const CSeqRules&		rules, 
	int						in_st, 
	int						in_en,
	int*					out_st,
	int*					out_en)
{
	int			at_st;
	int			at_en;
	CDbEntity*	db_ent;
	double		score;

	double low_score = DBL_MAX;
	int low_st = -1;
	int low_en = -1;

#if REQUIRED
	m_cpr.DestructiveFlush();	
#endif

	int idx = in_st;
	while (idx <= in_en )
	{
		at_st = idx;
		db_ent = list[idx];

		// Skipped anything already in use...
		if (db_ent->DidAction())
		{
			idx++;
			continue;
		}

		// Get any full profile from this entity
		at_en = scan_profile( list, at_st, MAX_GAP );

		// Score the profile...
		if (db_ent->Type() == DBHOLE)
		{
			if (rules.bore)
				score = score_path( rules, *db_ent );
			else
				score = low_score+1;
		}
		else	// Curve
		{
			if (rules.route)
				score = score_path( rules, *db_ent );
			else
				score = low_score+1;
		}

		// Weiner?
		if (score < low_score)
		{
			low_score = score;
			low_st = at_st;
			low_en = at_en;

			(*out_st) = at_st;
			(*out_en) = at_en;

#if REQUIRED
			m_cpr.Append( new Ccpr( score, at_st, at_en ) );
#endif
		}

		idx = at_en+1;
	}

#if REQUIRED
	m_cpr.Qsort( Ccpr::QSortCompare );
#endif

	return (low_st >= 0);
}

// ============================================================================
//		scan_profile
//
//	Given an entity index into a list of entities, return the index of the
//	last entity that can be considered a part of a contiguous "profile"\
//
// I was so hoping to get away from this nonsense with the introduction
//	of the "profile" object in the new system... but toolpath optimization
//	 doesn't *do* that.... ergh....
//
#if BEFORE_2005_01_31
int
CShortestPath::scan_profile( 
	const CDbEntityArray&	list, 
	int						start, 
	double					max_gap )
{
	CDbEntity*	dbEntity;
	CDbCurve*	dbCurve;
	C3dCoord	end;
	C3dVec		delta;
	double		close;
	double		dist;
	int			count, indx;

	dbEntity = list[start];
	close = max( max_gap, SMALL );

	dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
	if (dbCurve == NULL)
		return start;

	end = dbCurve->EndPt( 0 );

	count = list.Count();
	for (indx = start+1; indx < count; ++indx)
	{
		dbEntity = list[indx];
		if (dbEntity)
		{
			// Early-out, prevents death.
			dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
			if (!dbCurve)
				return indx-1;

			delta = end - dbCurve->StartPt( 0 );
			dist = delta.Length();

			// Check for break in profile...
			// TODO: Use end.WithinTol() ???
			if ( (dist > close)	|| dbEntity->DidAction() )
			{
				// Ta-da! This element doesn't fit the profile,
				// so return previous.
				return indx-1;
			}
			end = dbCurve->EndPt( 0 );
		}
	}
	// Ran out of elements... so this is good I suppose.
	return indx-1;
}
#else
int
CShortestPath::scan_profile( 
	const CDbEntityArray&	list, 
	int						start, 
	double					max_gap )
{
	CDbEntity*	dbEntity;
	CDbCurve*	dbCurve;
	CDbEntity*	parent;
	CDbTool*	dbTool;
	C3dCoord	end;
	C3dVec		delta;
	double		close;
	double		dist;
	int			count, indx;
	bool		is_punch;

	dbEntity = list[start];
	close = max( max_gap, SMALL );

	dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
	if (dbCurve == NULL)
		return start;

	parent = dbCurve->Owner();
	dbTool = dbCurve->Tool();
	is_punch = dbTool->IsPunchTool();

	end = dbCurve->EndPt( 0 );

	count = list.Count();
	for (indx = start+1; indx < count; ++indx)
	{
		dbEntity = list[indx];
		if (dbEntity)
		{
			// Early-out, prevents death.
			dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
			if (!dbCurve)
				return indx-1;

			if ( IsPhenolicProject() )
			{
				if (dbCurve->Owner() != parent)
					return indx-1;
			}
			else
			{
				if ( is_punch )
				{
					if ((dbCurve->Owner() != parent) || (dbCurve->Tool() != dbTool))
						return indx-1;
				}
				else
				{
					delta = end - dbCurve->StartPt( 0 );
					dist = delta.Length();

					// Check for break in profile...
					// TODO: Use end.WithinTol() ???
					if ( (dist > close)	|| dbEntity->DidAction() )
					{
						// Ta-da! This element doesn't fit the profile,
						// so return previous.
						return indx-1;
					}
					end = dbCurve->EndPt( 0 );
				}
			}
		}
	}
	// Ran out of elements... so this is good I suppose.
	return indx-1;
}
#endif

// ============================================================================
//		score_path
//
//	Given the rules and our current context, what is the "cost" of moving
//	to this next path?
//
double
CShortestPath::score_path(
	const CSeqRules&	seqRules,
	const CDbEntity&	db_ent )
{
	static double PENALTY = 1.e-2;

	C3dCoord	dest;
	C3dVec		delta;
	ID			tool_id;
	double		score;

	anchor_pnt( &dest, &db_ent, FALSE );

	delta = C3dVec( (dest.X() - seqRules.Buoy().X()),
					(dest.Y() - seqRules.Buoy().Y()),
					0.0 );

	tool_id = db_ent.ToolId();

	//
	// Toolchange inertia
	//
	score = ((tool_id == seqRules.tool_id) ? 0 : TOOL_PREFERENCE);

	//
	// Gravity
	//
	if (seqRules.flag & PATH_RULE_XPOS_FACTOR)
		score = score + (dest.X() * seqRules.pos_x_factor);
	if (seqRules.flag & PATH_RULE_YPOS_FACTOR)
		score = score + (dest.Y() * seqRules.pos_y_factor);

	//
	// Motion penalty
	//
	if (seqRules.flag & PATH_RULE_XDIST_FACTOR)
		score = score + (fabs( delta.X() ) * seqRules.delta_x_factor);
	if (seqRules.flag & PATH_RULE_YDIST_FACTOR)
		score = score + (fabs( delta.Y() ) * seqRules.delta_y_factor);

	if (seqRules.flag & PATH_RULE_TREND)
	{
		double dot = delta.X() * seqRules.trend_vec.X() + delta.Y() * seqRules.trend_vec.Y();
		if (dot < SMALL)
		{
			score += PENALTY;
		}
	}

	//
	// Router operations
	//
	const CDbCurve* db_curve = dynamic_cast<const CDbCurve*>(&db_ent);
	if (db_curve)
	{
		if (seqRules.flag & PATH_RULE_TCHG_COST)
		{
			if (seqRules.tool_id != tool_id)
				score = score + seqRules.toolchange_cost;
		}

		if (seqRules.flag & PATH_RULE_DROP_MIN)
		{
			if (seqRules.drop_cnt < seqRules.drop_min)
				score = 99999.0;
			else
			{
				// If we have 3 drops, crank up the possibility of doing a route
				score = score * MIXED_DROP_FACTOR;
			}
		}

		// TODO: Don't take an element in the middle of a profile...
	}

	//
	// Face change inertia
	// TODO: Face 1 limitations?
	//
	score += (int)(db_ent.WorkplaneId() != seqRules.face_id) * FACE_INERTIA;

	return score;
}


// ============================================================================
//		anchor_pnt
//
void 
CShortestPath::anchor_pnt( 
					C3dCoord*			io_anchor, 
					const CDbEntity*	db_ent, 
					bool				in_end )
{
	C3dCoord		anchor;
	CDbTool*		tool;

	entity_pnt( db_ent, in_end, &anchor );

	//
	// Adjust by way of aggregate head offset
	//
	tool = db_ent->Tool();
	if (tool)
	{
		anchor += tool->AggregateOffset();
	}

	*io_anchor = anchor;
}

void 
CShortestPath::anchor_pnt( 
					CSeqRules*			seqRules,
					const CDbEntity*	db_ent, 
					bool				in_end )
{
	C3dCoord	newBuoy;
	C3dCoord	seed;

	// Get entity's anchor point in world
	entity_pnt( db_ent, in_end, &seed );

	switch (seqRules->trend)
	{
	case LL_XPOS:
	case LR_XNEG:
	case UR_XNEG:
	case UL_XPOS:
		newBuoy.X( seed.X() );
		newBuoy.Y( seqRules->Anchor().Y() );
		break;
	case LL_YPOS:
	case LR_YPOS:
	case UR_YNEG:
	case UL_YNEG:
		newBuoy.X( seqRules->Anchor().X() );
		newBuoy.Y( seed.Y() );
		break;
	default:
		newBuoy = seed;
		break;
	}

	seqRules->Buoy( newBuoy );
}

// 2004/01/05 -- Rittal reported infinite loop during coding.  This
// problem was caused by "sign problems" on the tolerances.
bool
CShortestPath::IsGoodTrend(
						const CSeqRules&	seqRules,
						const CDbEntity*	dbEntity )
{
	double TOL = 1.e-4;

	if ( seqRules.bidirectional )
	{
		return TRUE;
	}
	else
	{
		C3dCoord	pt;
		C3dVec		vec;

		entity_pnt( dbEntity, FALSE, &pt );

		vec = (pt - seqRules.Buoy());

		switch (seqRules.trend)
		{
		case LL_XPOS:
		case UL_XPOS:
			return ((seqRules.forward) ? (vec.X() > -TOL) : (vec.X() < TOL));
		case LR_XNEG:
		case UR_XNEG:
			return ((seqRules.forward) ? (vec.X() < TOL) : (vec.X() > -TOL));
		case LL_YPOS:
		case LR_YPOS:
			return ((seqRules.forward) ? (vec.Y() > -TOL) : (vec.Y() < TOL));
		case UR_YNEG:
		case UL_YNEG:
			return ((seqRules.forward) ? (vec.Y() < TOL) : (vec.Y() > -TOL));
		default:
			return TRUE;
		}
	}
}

void 
CShortestPath::entity_pnt( 
					const CDbEntity*	dbEntity, 
					bool				end,
					C3dCoord*			pt )
{
	const CDbCurve*		db_curve;
	const CDbHole*		db_hole;
	const CDbCommand*	dbCommand;

	switch (dbEntity->Type())
	{
	case DBLINE:
	case DBARC:
		db_curve = dynamic_cast<const CDbCurve*>(dbEntity);
		(*pt) = ((end) ? db_curve->EndPt(0) : db_curve->StartPt(0));
		break;

	case DBHOLE:
		db_hole = dynamic_cast<const CDbHole*>(dbEntity);
		(*pt) = ((end) ? db_hole->EndPt(0) : db_hole->StartPt(0));
		break;

	case DBCOMMAND:
		dbCommand = dynamic_cast<const CDbCommand*>(dbEntity);
		(*pt) = dbCommand->Coord( 0 );
		break;

	default:
		return;
	}
}

void
CShortestPath::Transfer(
		CDbEntity*		db_ent,
		CDbEntityArray&	tmp_list,
		int				indxA,
		int				indxB,
		CDbEntityArray*	dst_list,
		CSeqRules*		seqRules )
{
	// 2010.10.11 (PE) -- Wow, dunno how long this has been
	// this way but grid optimization was always getting the
	// end of the profile as the anchor point. I'm afraid to
	// change this now because of the potential fallout. As
	// such, using the start point of the profile as the
	// anchor is restricted to the phenolic project.
	bool useEndPt = (IsPhenolicProject() ? FALSE : TRUE);

	// New anchor at the end of the profile
	anchor_pnt( seqRules, db_ent, useEndPt );

	// Transfer this profile over to the destination
	for (int indx = indxA; indx <= indxB; ++indx)
	{
		db_ent = tmp_list[indx];
		db_ent->DoAction();
		dst_list->Append( db_ent );
	}

	seqRules->face_id = db_ent->WorkplaneId();
	if (db_ent->Type() == DBHOLE )
	{
		seqRules->drop_cnt++;
	}
	else
	{
		seqRules->tool_id = db_ent->ToolId();
		seqRules->drop_cnt = 0;
	}
}



// ============================================================================
//	CutAvoidance
//
CReturn CShortestPath::CutAvoidance( CDbEntityArray* cut_list)
{
	CReturn ret;
	CString str;
	if (CReturn::Debug() >= 2)
	{ 
		str.Format("CutAvoidance.");
		ret.Diagnostic( str );
	}
	//	TODO: Accumulate barriers for use with multiple tools
	C2dBoxArray mer_array;
	C2dBox* mer;

	CDbEntity* exit_ent;
	CDbEntity* enter_ent;

	C3dCoord exit_pt;
	C3dCoord enter_pt;
	//
	//
	CDbEntity::NewAction();

	int prof_st = 0;
	int chunk_en = cut_list->Count()-1;
	while (prof_st <= chunk_en)
	{
		int dog_num = 0;

		int prof_en = scan_profile( *cut_list, prof_st, MAX_GAP );
		exit_ent = (*cut_list)[prof_en];
		exit_ent->IntSet( "_dog_num", 0);

		if (CReturn::Debug() >= 2)
		{ 
			str.Format("... block %d to %d", prof_st, prof_en);
			ret.Diagnostic( str );
		}

		int next = prof_en+1;

		CDbEntity* st_ent = (*cut_list)[prof_st];
		int depth = st_ent->IntGet( STR_CD, 0 );

		if ( (depth & 0x01)
			&& (exit_ent->Type() != DBHOLE)
			&& (next<=chunk_en) )
		{
			// Build the representative poly and minimum enclosing rectangle
			// for this profile
			//
			mer = new C2dBox();

			int idx;
			for (idx=prof_st; idx<=prof_en; idx++)
			{
				CDbCurve* ent = dynamic_cast<CDbCurve*>((*cut_list)[idx]);
				if (ent)
				{ (*mer) += ent->Box(); }
			}
			//
			// Force the dogleg to the relevant corner of this profile
			//
			enter_ent = (*cut_list)[next];
			entity_pnt(exit_ent, TRUE, &exit_pt);
			entity_pnt(enter_ent, FALSE, &enter_pt);

if (CReturn::Debug() >= 2)
{ 
	str.Format("... initial dogleg.");
	ret.Diagnostic( str );
}

			dog_num = dogleg(exit_ent, &exit_pt, enter_pt, *mer);

if (CReturn::Debug() >= 2)
{ 
	str.Format("... prep for A*.");
	ret.Diagnostic( str );
}

			//
			// Make a list of mers that could conceivably interfere
			// with this path
			//
			C2dBox path_mer;
			path_mer += exit_pt;
			path_mer += enter_pt;

			C2dBoxArray hot_mer;
			int num = mer_array.Count();
			for (idx=0; idx<num; idx++)
			{
				C2dBox* box = mer_array[idx];

				if (path_mer.Intersects(*box, SMALL))
				{ hot_mer.Append(box); }
			}

			double dia = exit_ent->Tool()->EffectiveDiameter();
			double adjust = dia*2.0;
			mer->Update( mer->Xmin()-adjust, mer->Ymin()-adjust, mer->Xmax()+adjust, mer->Ymax()+adjust);
			mer_array.Append(mer);
			//
			// Now generate a path from exit to entrance, avoiding
			// the hot_mer obstacles
			//
			CPathState* start = new CPathState(&hot_mer, exit_pt, enter_pt);
			CAStar astar(start);

if (CReturn::Debug() >= 2)
{ 
	str.Format("... A* path");
	ret.Diagnostic( str );
}
			CAStateArray* path = astar.Search();
			//
			// Finally, annotate the path into the exit entity from the profile
			//
			if (path)
			{
if (CReturn::Debug() >= 2)
{ 
	validate_path(path);
	str.Format("... A* doglegs.");
	ret.Diagnostic( str );
}

				// Walk the path, skipping the first and last nodes
				for (int idx=1; idx<(path->Count()-1); idx++)
				{
if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... %d of %d", idx, path->Count()-1);
	ret.Diagnostic( str );
}

					CPathState* state = (CPathState*)(*path)[idx];
					if (state)
					{
						dog_num = rapid_to(exit_ent, &exit_pt, state->Pos(), dog_num );
					}
				}
if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... path->DestructiveFlush()");
	ret.Diagnostic( str );

	validate_path(path);
}
				path->DestructiveFlush();
			}
		}
if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... _dog_num %d", dog_num);
	ret.Diagnostic( str );
}

		CVarList* attrib = exit_ent->pAttrib();
		if (attrib)
		{ attrib->setInt( "_dog_num", dog_num); }

		prof_st = next;
	}

	if (CReturn::Debug() >= 2)
	{ 
		str.Format("CutAvoidance Done.");
		ret.Diagnostic( str );
	}

	mer_array.DestructiveFlush();

if (CReturn::Debug() >= 2)
{ 
	str.Format("... (return)");
	ret.Diagnostic( str );
}

	return ret;
}

void
CShortestPath::validate_path(
	CAStateArray* path)
{
	CString str;
	CReturn ret;
	//
	// Check for duplicates?  And nulls...
	//
	int num = path->Count();
	for (int idx=0; idx<num; idx++)
	{
		CAState* is = (*path)[idx];

		if (!is)
		{
			str.Format( "Validate path:  entry %d is null", idx);
			ret.Diagnostic(str);
			MessageBox(NULL, str, NULL, MB_OK);
			continue;
		}
		for (int jdx=0; jdx<num; jdx++)
		{
			if (idx == jdx)
			{ continue; }

			CAState* js = (*path)[jdx];

			if (is == js)
			{
				str.Format( "Validate path:  entry %d is the same as %d", idx, jdx);
				ret.Diagnostic(str);
				MessageBox(NULL, str, NULL, MB_OK);
			}
		}
	}

	// See also StdAfx.h for notes on _CRTDBG_MAP_ALLOC
#ifndef _CRTDBG_MAP_ALLOC
	int heapstatus = _heapchk();
	switch( heapstatus )
	{
	case _HEAPBADBEGIN:
	  ret.Diagnostic( "ERROR - bad start of heap" );
		MessageBox(NULL, "heap error", NULL, MB_OK);
	  break;
	case _HEAPBADNODE:
	  ret.Diagnostic( "ERROR - bad node in heap" );
		MessageBox(NULL, "heap error", NULL, MB_OK);
	  break;
	}
#endif
}

// ============================================================================
//
// Experimental dogleg around profile code... I could do this with the A* path 
// finder, but analysis indicates that this dedicated code will work just as well.
//
int
CShortestPath::dogleg(
		CDbEntity* out_ent,
		C3dCoord* out_pt,
		C3dCoord& in_pt,
		C2dBox&	mer )
{
	int num = out_ent->IntGet( "_dog_num", 0);

	if (out_ent->Type() == DBHOLE)
	{ return num; }
	//
	//
	int code = dogleg_code(mer, in_pt, *out_pt);
	//
	//
	double dia = out_ent->Tool()->EffectiveDiameter();
	double adjust = dia*2.0;
	C2dBox box(mer);
	box.Update( box.Xmin()-adjust, box.Ymin()-adjust, box.Xmax()+adjust, box.Ymax()+adjust);

	switch (code)	// see do_code() for the meaning of the codes.
	{
		case 0:
		case 1:
		case 2:
		case 3:
			num = rapid_to(out_ent, box, out_pt, num, (eEdgeCode)code);
			break;
		case 4:
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, TOP_EDGE);
			break;
		case 5:
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, TOP_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			break;
		case 6:
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, BOTTOM_EDGE);
			break;
		case 7:
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, BOTTOM_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			break;
		case 8:
			num = rapid_to(out_ent, box, out_pt, num, TOP_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			break;
		case 9:
			num = rapid_to(out_ent, box, out_pt, num, TOP_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			break;
		case 10:
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, TOP_EDGE);
			break;
		case 11:
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, TOP_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			break;
		case 12:
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, BOTTOM_EDGE);
			break;
		case 13:
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, BOTTOM_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			break;
		case 14:
			num = rapid_to(out_ent, box, out_pt, num, BOTTOM_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, RIGHT_EDGE);
			break;
		case 15:
			num = rapid_to(out_ent, box, out_pt, num, BOTTOM_EDGE);
			num = rapid_to(out_ent, box, out_pt, num, LEFT_EDGE);
			break;
	}

	out_ent->IntSet( "_dog_num", num);

	return num;
}

//
// This method calculates a code that describes both the relationship of the destination
// (in_pt) to the profile's MER box, it also describes the position of the source (out_pt)
// within that box.
//
int
CShortestPath::dogleg_code(
	const C2dBox& box,
	const C3dCoord& in_pt,
	const C3dCoord& out_pt )
{
	int code = 0;

	if (in_pt.X() > box.Xmax())
	{ code += 1; }
	else
	if (in_pt.X() < box.Xmin())
	{ code += 2; }

	if (in_pt.Y() > box.Ymax())
	{ code += 4; }
	else
	if (in_pt.Y() < box.Ymin())
	{ code += 8; }
	//
	//
	if (out_pt.X() > box.Xc())
	{
		if (out_pt.Y() > box.Yc())
		{ code += 0; }
		else
		{ code += 48; }
	}
	else
	{
		if (out_pt.Y() > box.Yc())
		{ code += 16; }
		else
		{ code += 32; }
	}
	//
	// Now compress the code
	//
	double dx;
	double dy;
	switch (code)
	{
		case 1:
		case 49:
			code = 0;
			break;
		case 2:
		case 6:
			code = 9;
			break;
		case 4:
		case 20:
			code = 1;
			break;
		case 8:
		case 9:
			code = 6;
			break;
		case 10:
			code = 7;
			break;
		case 17:
		case 21:
			code = 8;
			break;
		case 18:
		case 34:
			code = 2;
			break;
		case 25:
			code = 13;
			break;
		case 26:
			code = 12;
			break;
		case 33:
		case 41:
			code = 14;
			break;
		case 36:
		case 38:
			code = 10;
			break;
		case 37:
			code = 11;
			break;
		case 40:
		case 56:
			code = 3;
			break;
		case 50:
		case 58:
			code = 15;
			break;
		case 52:
		case 53:
			code = 4;
			break;
		case 54:
			code = 5;
			break;
		//
		// Special codes
		//
		case 5:
			dx = fabs(out_pt.X() - box.Xmax());
			dy = fabs(out_pt.Y() - box.Ymax());
			if (dx < dy)
			{ code = 0; }
			else
			{ code = 1; }
			break;
		case 22:
			dx = fabs(out_pt.X() - box.Xmin());
			dy = fabs(out_pt.Y() - box.Ymax());
			if (dx < dy)
			{ code = 2; }
			else
			{ code = 1; }
			break;
		case 42:
			dx = fabs(out_pt.X() - box.Xmin());
			dy = fabs(out_pt.Y() - box.Ymin());
			if (dx < dy)
			{ code = 2; }
			else
			{ code = 3; }
			break;
		case 57:
			dx = fabs(out_pt.X() - box.Xmax());
			dy = fabs(out_pt.Y() - box.Ymin());
			if (dx < dy)
			{ code = 0; }
			else
			{ code = 3; }
			break;
	}
	return code;
}

int
CShortestPath::rapid_to(
	CDbEntity* dog_ent,
	const C2dBox& box,
	C3dCoord* at,
	int num,
	eEdgeCode edge )
{
	if (!at)
	{ return num; }

CString str;
CReturn ret;
if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... ... Rapid to edge %d (%f, %f)", edge, at->X(), at->Y());
	ret.Diagnostic( str );
}

	switch (edge)
	{
		case TOP_EDGE:
			if (EQUAL(at->Y(), box.Ymax()))
			{ return num; }
			
			at->Y(box.Ymax());
			break;

		case LEFT_EDGE:
			if (EQUAL(at->X(), box.Xmin()))
			{ return num; }
			
			at->X(box.Xmin());
			break;

		case BOTTOM_EDGE:
			if (EQUAL(at->Y(), box.Ymin()))
			{ return num; }
			
			at->Y(box.Ymin());
			break;

		case RIGHT_EDGE:
			if (EQUAL(at->X(), box.Xmax()))
			{ return num; }
			
			at->X(box.Xmax());
			break;
	}
	//
	//
	CString name;

	num++;
if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... ... rapid set %d", num);
	ret.Diagnostic( str );
}

	CVarList* attrib = dog_ent->pAttrib();
	if (attrib)
	{
		name.Format("_dog_x%d", num);
		attrib->setReal( name, at->X() );

		name.Format("_dog_y%d", num);
		attrib->setReal( name, at->Y() );
	}

if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... ... (rapid_to)");
	ret.Diagnostic( str );
}

	return num;
}


int
CShortestPath::rapid_to(
	CDbEntity* dog_ent,
	C3dCoord* at,
	C3dCoord& dest,
	int num )
{
	if (!at)
	{ return num; }

	if (at->WithinTolXY(dest, SMALL))
	{ return num; }

CString str;
CReturn ret;
if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... ... rapid_to, easy");
	ret.Diagnostic( str );
}

	*at = dest;
	num++;
	//
	//
	CString name;

	CVarList* attrib = dog_ent->pAttrib();
	if (attrib)
	{
		name.Format("_dog_x%d", num);
		attrib->setReal( name, at->X() );

		name.Format("_dog_y%d", num);
		attrib->setReal( name, at->Y() );
	}

if (CReturn::Debug() >= 2)
{ 
	str.Format("... ... ... (rapid to)");
	ret.Diagnostic( str );
}

	return num;
}

int
CShortestPath::OrientCompare( const void* ptrA, const void* ptrB )
{
	CDbEntity* dbEntityA = ( *(CDbEntity**) ptrA);
	CDbEntity* dbEntityB = ( *(CDbEntity**) ptrB);

	double orientA = dbEntityA->DoubleGet( "orient", UNDEFINED );
	double orientB = dbEntityB->DoubleGet( "orient", UNDEFINED );

	// We may have to ajust range to [0..180] for
	// tools having 2-fold symmetry and to [0..90]
	// for tools having 4-fold symmetry.

	double diff = orientA - orientB;

	return ((fabs(diff) < 1.e-2) ? 0 : SGN(diff));  // arbitrary tolerance
}

void
CShortestPath::SortedHitsGet(
	const CDbTool*			dbTool,
	const CDbEntityArray&	src_list,
	CDbEntityArray*			sorted_hits )
{
	CDbEntity*	dbEntity;
	int			count, indx;

	sorted_hits->BenignFlush();

	count = src_list.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = src_list[indx];

		if ( dbEntity->DidAction() )
			continue;  // This entity has already been processed.

		if (dbEntity->Tool() == dbTool)
			sorted_hits->Append( dbEntity );
	}

	sorted_hits->Qsort( CShortestPath::OrientCompare );
}


// Build an intermediate list of profile start points and/or holes.
void
CShortestPath::GetSpans(
	const CDbEntityArray&	passes,
	TIndexPairArray*		spans )
{
	int	count, indxA, indxB;

	count = passes.Count();

	indxA = 0;
	while (indxA < count)
	{
		indxB = scan_profile( passes, indxA, MAX_GAP );
		spans->Append( new CIndexPair( indxA, indxB ) );
		indxA = indxB + 1;
	}
}

C3dCoord
CShortestPath::TSP_Solve(
	const C3dCoord&			seed,
	const CDbEntityArray&	passes,
	TIndexPairArray*		spans )
{
	// Defaults to (UNDEFINED, UNDEFINED, UNDEFINED).
	C3dCoord	pe;
	int			count;

	count = spans->Count();  // Must be greater-than 2.

	// Use the span & pass information to create the input to Solve().
	tCity*	cities = GetCities( seed, passes, (*spans) );
	if (cities != NULL)
	{
		CPrimsTSP2	tsp;

		tsp.Solve( cities, count );

		// Rearrange the order of passes by applying the ordered cities.
		Reorder( cities, 0, count, spans );

		pe.XYZ( cities[count-1].x, cities[count-1].y, 0. );

		delete [] cities;
	}

	return pe;
}

// Where each span provides indices into the passes.
// NOTE: Must have more that two passes for TSP to make any sense.
tCity*
CShortestPath::GetCities(
	const C3dCoord&			seed,
	const CDbEntityArray&	passes,
	const TIndexPairArray&	spans )
{
	tCity*		results;
	tCity		tmp;
	CIndexPair*	span;
	CDbEntity*	dbEntity;
	C3dCoord	ps;
	double		dist, dx, dy;
	double		min_dist;
	int			min_indx;
	int			count, indx;
	bool		use_seed_pt;
	
	results = NULL;

	min_dist = UNDEFINED;
	min_indx = -1;

	// The user is *not* required to select a seed point.
	use_seed_pt = (seed.X() < UNDEFINED);

	// Use the intermediate list to construct an array of cities.
	count = spans.Count();
	if (count > 2)
	{
		results = new tCity[count];

		for (indx = 0; indx < count; ++indx)
		{
			span = spans[indx];

			dbEntity = passes[ span->Start() ];

			results[indx].ptr = (void*) span;
			results[indx].tag = -1;

			switch ( dbEntity->Type() )
			{
			case DBHOLE:
				ps = ((CDbHole*) dbEntity)->Center();
				break;

			case DBCOMMAND:
				// Though we really should not optimize with instances,
				// we also do not want the application to crash by
				// assuming this entity represents a curve.
				ps = ((CDbCommand*) dbEntity)->Coord();
				break;

			default:
				// A large but safe(?) assumption.
				ps = ((CDbCurve*) dbEntity)->StartPt();
				break;
			}

			if ( use_seed_pt )
			{
				dx = ps.X() - seed.X();
				dy = ps.Y() - seed.Y();
				dist = sqrt( dx*dx + dy*dy );
				if (dist < min_dist)
				{
					min_dist = dist;
					min_indx = indx;
				}
			}

			results[indx].x = ps.X();
			results[indx].y = ps.Y();
		}

		if (min_indx > 0)
		{
			// Move the point (closest to the seed point) by
			// simply swapping the data.
			tmp = results[0];
			results[0] = results[min_indx];
			results[min_indx] = tmp;
		}
	}

	return results;
}


// NOTE: lower_bound and upper_bound are inclusive indices.
void
CShortestPath::Reorder(
	tCity*				cities,
	int					lower_bound,
	int					upper_bound,
	TIndexPairArray*	spans )
{
	for (int indx = lower_bound; indx < upper_bound; ++indx)
	{
		spans->Replace( indx, (CIndexPair*) cities[indx-lower_bound].ptr );
	}
}
