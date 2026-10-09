// ==================================================================
//		Optimizer
//
// ==================================================================

#include "stdafx.h"
#include <float.h>

#include "Register.h"
#include "StringConst.h"

#include "EntityDb.h"
#include "DbTool.h"
#include "Optimizer.h"
#include "DbCommand.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "DbIterator.h"

#include "ShortestPath.h"
#include "ToolSequencer.h"

#include "Nibbler.h"
#include "Portal.h"


// ==================================================================

COptimizer::COptimizer()
{
	m_spacetol = SMALL;

	// NOTE: m_nibble should be active ONLY during code generation
	// and NOT during entity sequencing.  In this latter case,
	// Ind_Hits is temporarily suppressed by VB frmSeqMove::Form_Load().
	m_nibble = (CPortal::GlobalModel()->Header().getInt( "Ind_Hits", FALSE ) != FALSE);
	m_nibble = false;

	// Relevant only when IsPhenolicProject() returns true.
	m_phenolicPart = NULL;
}

COptimizer::~COptimizer()
{
}

CReturn	
COptimizer::PrepModel( 
				CModel*			model,
				CWorkPkgArray*	sheetWorkPkgs,
				CWorkPkgArray*	subdefWorkPkgs )
{
	CReturn	status;
	C3dBox	box;

	status.Diagnostic( "*COptimizer::PrepModel(#1)" );

	// As most processes don't clean-up their tags when
	// they're done processing entities, we must remove
	// the tags now before further processing.
	CDbEntity::NewAction();

	// The default work area is based upon the global bounding box.
	// The X limits of the bounding box will be over-ridden by the
	// X limits of a repo_zone bounding box, if one exists.
	box  = model->Box( 0 );

	SubdefsPrep( (*model), subdefWorkPkgs );

	MainPrep( (*model), box, sheetWorkPkgs );

	// <ahem>... our bore spacing tolerance, too
	m_spacetol = model->Header().getReal( "SpaceTol", SMALL );
	if (m_spacetol < SMALL)
		m_spacetol = SMALL;

	status.Diagnostic( "*COptimizer::PrepModel(#2)" );

	return status;
}

void
COptimizer::SubdefsPrep(
					const CModel&	model,
					CWorkPkgArray*	subdefWorkPkgs )
{
	CReturn		status;
	CDbIterator iter;
	CDbPattern*	dbPattern;
	CWorkPkg*	workPkg;
	C3dBox		box;
	C3dCoord	ps;
	int			subnum;

	status.Diagnostic( "*COptimizer::SubdefsPrep(#1)" );

	subnum = 1;

	iter.Init( model.Db(), DBPATTERN );
	while (1)
	{
		dbPattern = dynamic_cast<CDbPattern*>(iter());
		if (dbPattern == NULL)
			break;

		if ( !dbPattern->DidAction() && !dbPattern->IsMain() )
		{
			// NOTE: The extents of the workPkg are required by
			// CSeqRules::TrendSet() during code optimization.
			box = dbPattern->Box();
			workPkg = new CWorkPkg( NULL, TRUE, box.Xmin(), box.Ymin(), box.Xmax(), box.Ymax() );
			subdefWorkPkgs->Append( workPkg );

			prep_container( dbPattern, &(workPkg->Entities()) );

			dbPattern->IntSet( STR_SUBDEF, subnum );
			++subnum;

			// Record the 'reference point' for later use by
			// CModelClfile::SubdefsUpdate()
#ifdef V16_ORIGINAL
			ps = workPkg->StartPt();
			dbPattern->DoubleSet( STR_SUBX, ps.X() );
			dbPattern->DoubleSet( STR_SUBY, ps.Y() );
#else
			// NOTE: This was done for Modular Services.  Their Cincinatti
			// laser establishes coordinate systems for subcalls as:
			//    G0G53X(xmin)Y(ymin)
			//    G92X0Y0
			//
			// The G53 is used to establish the local coordinate offset.
			// This allows the machine to know where the head is in absolute
			// when it returns from the subcall.  The G92 establishes the
			// origin of the subroutine at the lower left corner of the
			// subroutine because the machine can not have negative Y values
			// beyond approx. -0.5.
			dbPattern->DoubleSet( STR_SUBX, box.Xmin() );
			dbPattern->DoubleSet( STR_SUBY, box.Ymin() );
#endif
		}

		iter.Next();
	}

	status.Diagnostic( "*COptimizer::SubdefsPrep(#2)" );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// When coding a single sheet (ie. an MM2 file that was loaded using
// File/Open), the main program may be any combination of features and
// subroutine call-outs.
//
// When coding multiple sheets (ie. via Nesting when glb.SubsAcrossSheets
// is true), the main program will be only(?) subroutine call-outs,
// one for each nested sheet.
//
void
COptimizer::MainPrep(
					const CModel&	model,
					const C3dBox&	box,
					CWorkPkgArray*	workPkgArray )
{
	CReturn			status;
	CDbIterator		iter;
	CWorkPkgArray	sheetWorkPkgs;
	CWorkPkg*		workPkg;
	CDbPattern*		dbPattern;
	int				count, indx;

	status.Diagnostic( "*COptimizer::MainPrep(#1)" );

	iter.Init( model.Db(), DBPATTERN );
	while (1)
	{
		dbPattern = dynamic_cast<CDbPattern*>(iter());
		if (dbPattern == NULL)
			break;

		if ( dbPattern->IsMain() )
		{
			PrepSheet( (*dbPattern), box, &sheetWorkPkgs );

			// Copy the work packages from the sheet.
			count = sheetWorkPkgs.Count();
			for (indx = 0; indx < count; ++indx)
			{
				workPkg = sheetWorkPkgs[indx];
				workPkgArray->Append( workPkg );
			}

			sheetWorkPkgs.BenignFlush();
		}

		iter.Next();
	}

	status.Diagnostic( "*COptimizer::MainPrep(#2)" );
}

void
COptimizer::PrepSheet(
					const CDbPattern&	sheet,
					const C3dBox&		box,
					CWorkPkgArray*		sheetWorkPkgs )
{
	CDbFeature*	dbFeature;
	CDbEntity*	dbEntity;
	CWorkPkg*	workPkg;
	CString		type;
	int			count, indx;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Process all of the repo-zone features in this sheet.  Note,
	// it is possible for a sheet to be devoid of repo-zones if
	// the model was not generated by nesting.
	//
	// As prep_container() is a recursive descent process, all
	// contained features will be tagged, preventing reprocessing
	// during subsequent passes.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = sheet.Count();

	for (indx = 0; indx < count; ++indx)
	{
		dbFeature = dynamic_cast<CDbFeature*>(sheet[indx]);

		if (dbFeature != NULL && !dbFeature->DidAction())
		{
			if ( dbFeature->IsWorkZone() )
			{
				workPkg = new CWorkPkg(
					dbFeature, FALSE, -UNDEFINED, box.Ymin(), UNDEFINED, box.Ymax() );

				prep_container( dbFeature, &(workPkg->Entities()) );

				// By suppressing empty workzones, we prevent
				// unnecessary repo events in the CNC code.
				if (workPkg->Entities().Count() > 0)
				{
					// NOTE: The extents of the workPkg are required by
					// CSeqRules::TrendSet() during code optimization.
					workPkg->BoxUpdate();  // see comment in CWorkPkg::BoxUpdate()

					sheetWorkPkgs->Append( workPkg );
				}
				else
				{
					delete workPkg;
				}
			}
		}
	}

	// Sort the work areas on increasing order of absolute repo position.
	sheetWorkPkgs->Qsort( COptimizer::ZoneCompare );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Process the remaining features, putting their
	// remaining entities into the default work area.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	workPkg = NULL;

	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = sheet[indx];

		if ( !dbEntity->DidAction() )
		{
			if (workPkg == NULL)
			{
				// NOTE: The extents of the workPkg are required by
				// CSeqRules::TrendSet() during code optimization.
				workPkg = new CWorkPkg( NULL, FALSE, box.Xmin(), box.Ymin(), box.Xmax(), box.Ymax() );
				sheetWorkPkgs->Append( workPkg );
			}

			dbFeature = dynamic_cast<CDbFeature*>(dbEntity);
			if (dbFeature == NULL)
				workPkg->Entities().Append( dbEntity );
			else
				prep_container( dbFeature, &(workPkg->Entities()) );
		}
	}
}

// ==================================================================
// Recursively descend the container, adding tooled entities to cutOrder.
CReturn	
COptimizer::prep_container( 
					CDbContainer*	db_contain, 
					CDbEntityArray*	cutOrder )
{
	CReturn status;

	CDbFeature*		dbFeature;
	CDbEntity*		dbEntity;
	CDbContainer*	sub_contain;
	CDbCommand*		dbCommand;
	bool			isRepoFeature = FALSE;
	bool			okay;

	CDbTool* dbTool = db_contain->Tool();

	// status.Diagnostic( "*COptimizer::prep_container(#1)" );

	okay = FALSE;

	if (db_contain->Type() == DBPATTERN)
	{
		okay = TRUE;
	}
	else if (db_contain->Type() == DBFEATURE)
	{
		dbFeature = dynamic_cast<CDbFeature*>(db_contain);

		// Prevent reprocessing.
		dbFeature->DoAction();

		if ( IsPhenolicProject() )
		{
			if (dbFeature->IsPart())
				m_phenolicPart = dbFeature;
		}

		okay = TRUE;
	}
	else if (db_contain->Type() == DBPROFILE)
	{
		okay = TRUE;
	}

	if ( okay )
	{
		int count, indx;

		count = db_contain->Count();
		indx = 0;
		while (indx < count)
		{
			dbEntity = (*db_contain)[indx];
			sub_contain = dynamic_cast<CDbContainer*>(dbEntity);

			if (sub_contain && !sub_contain->DidAction())
			{
				prep_container( sub_contain, cutOrder );
			}
			else
			{
				dbCommand = dynamic_cast<CDbCommand*>(dbEntity);
				if (dbCommand != NULL)
				{
					if (dbCommand->IsInstance() || dbCommand->IsTooledText())
						cutOrder->Append( dbEntity );
				}
				else if ( (dbEntity->Tool() != NULL) &&
						  !dbEntity->Tool()->IsLayer() )
				{
					bool replaced = Keep( dbEntity, cutOrder );
					if ( replaced )
					{
						// Keep things sychronized because dbEntity 
						// was replace with individual hits

						--count;
						--indx;
					}

					if (IsPhenolicProject() && (m_phenolicPart != NULL))
					{
						// ASSUMPTION: Keep() added dbEntity to cutOrder.
						// "~pp" means 'phenolic-part'.
						dbEntity->IntSet( "~pp", m_phenolicPart->Id() );
					}
				}
			}

			++indx;
		}
	}

	// status.Diagnostic( "*COptimizer::prep_container(#2)" );

	return status;
}

// ==================================================================
//
C2dBox	
COptimizer::MachineExtents( const CModel& model, double y_origin )
{
	double xmin = 0.0;
	double ymin = 0.0;
	double xmax = 0.0;
	double ymax = 0.0;
	model.Header().getReal( STR_XMIN, &xmin );
	model.Header().getReal( STR_XMAX, &xmax );
	model.Header().getReal( STR_YMIN, &ymin );
	model.Header().getReal( STR_YMAX, &ymax );

	CDbWorkplane*	top = NULL;
	model.EntityFind( STR_TOP, (CDbEntity**)&top, DBWORKPLANE, DBWORKPLANE );

	C3dCoord	ext_max( xmax, ymax-y_origin, 0.0 );
	C3dCoord	ext_min( xmin, ymin-y_origin, 0.0 );
	if (top)
	{
		top->Transform().Transform( &ext_max );
		top->Transform().Transform( &ext_min );
	}

	return C2dBox( ext_min, ext_max );
}

CReturn
COptimizer::PrepToolOrder(
						ESeqToolOrder			toolOrder,
						bool					drillsOptimize,
						const CModel&			model,
						const CDbEntityArray&	dbEntityList,
						CDbEntityArray*			dbToolOrder )
{
	CReturn			status;
	CToolSequencer	toolSeq;

	dbToolOrder->BenignFlush();

	if (toolOrder == TOOL_ORDER_BY_ENCOUNTER)
	{
		toolSeq.ModelOrder( dbEntityList, dbToolOrder );
	}
	else if (toolOrder == TOOL_ORDER_BY_JOB)
	{
		toolSeq.ToolSetupOrder( model, drillsOptimize, dbToolOrder );
	}

	return status;
}


// ==================================================================

CReturn
COptimizer::OptimizeWorkPkg(
						CSeqRules*		seqRules,
						CWorkPkg*		workPkg,
						CDbEntityArray*	cutOrder )
{
	CReturn			status;
	CDbEntityArray	dbDrillOpt;
	CDbEntityArray	dbPathOpt;
	CDbEntityArray	dbSeqOpt;
	const CModel&	model = seqRules->Model();

	status.Diagnostic( "*COptimizer::OptimizeWorkPkg(#1)" );

	cutOrder->BenignFlush();

	CDbEntityArray* pEntityList = NULL;

	CDbEntityArray& rawEntities = workPkg->Entities();

	// (V14.5 - 4/17/2001 - PE) Guard against 'empty' work zones.
	// Otherwise, the dx of the bounding box is 2 * UNDEFINED which,
	// in turn, appears to cause an infinite loop when doing grid opt.
	//
	if ((seqRules->path_opt || seqRules->drill_opt || seqRules->grid_opt) &&
		 (rawEntities.Count() > 0) )
	{
		CDbEntityArray dbToolList;

		pEntityList = &rawEntities;

		if ( seqRules->drill_opt )
		{
			// Apply 'drill drop optimization'.  This processes converts
			// mutiple hits to single hits with gang tooling.
			CDbEntityArray dbDrillOptToolList;

			C2dBox mach_ext = MachineExtents( model, seqRules->y_origin );

			status += PrepToolOrder( TOOL_ORDER_BY_JOB, TRUE,
							model, rawEntities, &dbDrillOptToolList );

			seqRules->PassCountSet( dbDrillOptToolList.Count() );

			status += OptimizeBore(
							(*pEntityList), dbDrillOptToolList, &dbDrillOpt, mach_ext );

			if (status.getStatus() != STATUS_ERROR)
				pEntityList = &dbDrillOpt;
		}

		status += PrepToolOrder(
							seqRules->toolOrder, FALSE, model, rawEntities, &dbToolList );

		if (seqRules->toolOrder == TOOL_ORDER_BY_JOB)
		{
			seqRules->PassCountSet( dbToolList.Count() );
		}

		if ( seqRules->path_opt )
		{
			// At this point in the process, the input to OptimizePath()
			// is either the rawEntities or the dbDrillOpt entities.
			CShortestPath	shortest;

			status = shortest.OptimizePath( seqRules, dbToolList, (*pEntityList), &dbPathOpt );
			if ( status.IsOk() )
				pEntityList = &dbPathOpt;
		}

		if ( seqRules->grid_opt )
		{
			if (seqRules->gridSeqOption == SEQUENCE_BY_TOOL_ORDER)
			{
				status = SequenceByToolOrder(
							seqRules, (*pEntityList), dbToolList, &dbSeqOpt );
			}
			else if (seqRules->gridSeqOption == SEQUENCE_BY_FLOW_CHART)
			{
				status = SequenceByFlowChart(
							seqRules, (*pEntityList), dbToolList, &dbSeqOpt );
			}

			if ( status.IsOk() )
				pEntityList = &dbSeqOpt;
		}
	}
	else
	{
		pEntityList = &rawEntities;
	}

	CDbFeature* topLevelFeature = workPkg->Feature();

	// Move this bucket of entities to the final cut order.
	CutOrderAppend( topLevelFeature, pEntityList, cutOrder );

	if ( seqRules->drill_opt )
		dbDrillOpt.BenignFlush();

	if ( seqRules->path_opt )
		dbPathOpt.BenignFlush();

	if ( seqRules->grid_opt )
		dbSeqOpt.BenignFlush();

	status.Diagnostic( "*COptimizer::OptimizeWorkPkg(#2)" );

	return status;
}

void
COptimizer::CutOrderAppend(
						CDbFeature* topLevelFeature,
						CDbEntityArray* dbEntities,
						CDbEntityArray* cutOrder )
{
	int count, indx;

	if (topLevelFeature != NULL)
	{
		// Add the containing feature (in the event that it has @HOLD and/or @REPO
		// attributes).  Clearly, hold-down and repo commands must be executed before
		// anything else in the workzone has been processed.
		cutOrder->Append( topLevelFeature );

		// Add any clamp commands, etc.
		count = topLevelFeature->Count();
		for (indx = 0; indx < count; ++indx)
		{
			CDbCommand* dbCommand = dynamic_cast<CDbCommand*>( (*topLevelFeature)[indx] );
			if (dbCommand == NULL)
				break;

			cutOrder->Append( dbCommand );
		}
	}

	// Add the sequenced cutting entities.
	count = dbEntities->Count();
	for (indx = 0; indx < count; ++indx)
	{
		cutOrder->Append( (*dbEntities)[indx] );
	}
	dbEntities->BenignFlush();
}


// ============================================================================
//		OptimizeBore
//
//		Take multiple holes and collapse them into single holes with
//		multiple hits (if possible)
//
CReturn
COptimizer::OptimizeBore(
	const CDbEntityArray&	src_list,
	const CDbEntityArray&	tool_list,
	CDbEntityArray*			cutOrder,
	const C2dBox&			mach_ext )
{
	CReturn		ret;
	int			elem_idx;			// Element index in scan
	CDbEntity*	db_ent;				// Element being...
	int			first_hole;			// First hole in a block of holes
	int			last_hole;			// Last hole in a block...
	C3dCoord	last_drop(0,0,0);

	int src_num = src_list.Count();

	// -------------------------------------------------------
	//	Clear the flags we are about to use
	//
	CDbEntity::NewAction();
	for (elem_idx=0; elem_idx<src_num; elem_idx++)
	{
		// Clear any touch flags...
		db_ent = src_list[elem_idx];
		if (db_ent)
			db_ent->AttribDelete( STR_NC_CODE_NUMBER );
	}

	// -------------------------------------------------------
	//	Scan the database, working with blocks of holes
	//
	for (elem_idx=0; elem_idx<src_num; elem_idx++)
	{
		db_ent = src_list[elem_idx];

		if (db_ent->Type() == DBHOLE)
		{
			if (!db_ent->DidAction())
			{
				// Found an untouched hole!  Get the cluster...
				first_hole = elem_idx;
				last_hole = get_hole_block( src_list, first_hole );

				if (last_hole >= first_hole)
				{
					// For this hole, find the tool I place on it to hit the most other
					// holes in this block on this drop.  Then drill them and mark them
					// for death (leaving, of course, the master hole).
					if ( !optimize_one_drop( 
										src_list, 
										tool_list,
										mach_ext, 
										cutOrder, 
										elem_idx, 
										first_hole, 
										last_hole, 
										&last_drop ) )
					{
						ret.UserWarn( IDS_OPTHOLE_REACH, 
										elem_idx,
										((CDbHole*)db_ent)->Coord().X(),
										((CDbHole*)db_ent)->Coord().Y(),
										((CDbHole*)db_ent)->Coord().Z(),
										((CDbHole*)db_ent)->Depth() );

						db_ent->DoAction();
						cutOrder->Append( db_ent );
					}
				}
			}
		}
		else
		{
			// Preserve non-holes, in order found
			cutOrder->Append( db_ent );
		}
	}

	return ret;
}



// =======================================================================
//		get_hole_block
//
//		Given the index of a hole, see how many holes follow it and
//		return the index of the last one.
//
int
COptimizer::get_hole_block( 
	const CDbEntityArray&	src_list,
	int						in_start )
{
	CDbEntity*	db_ent;
	bool		touched;

	// Find the set of contiguous holes
	touched = TRUE;
	for (int idx=in_start; idx<src_list.Count(); idx++)
	{
		db_ent = src_list[idx];
		if ( db_ent->Type() != DBHOLE )
		{
			// Why did I do this?
			if (touched)
				return -1;

			return idx-1;
		}
		if (!db_ent->DidAction())
			touched = FALSE;
	}
	return src_list.Count()-1;
}


// =======================================================================
//		optimize_one_drop
//
//		Given a single hole in a block and the range of indices of that block,
//		find the tool drop that correctly drills the most holes.  And 
//		drill them.
//
bool
COptimizer::optimize_one_drop( 
	const CDbEntityArray&	src_list, 
	const CDbEntityArray&	tool_list,
	const C2dBox&			mach_ext,
	CDbEntityArray*			cutOrder, 
	int						anchor_idx, 
	int						first_hole, 
	int						last_hole, 
	C3dCoord*				drop_pnt )
{
	bool			okay;
	int				idx;
	int				tool_idx;	// Current tool in scan
	CDbTool*		tool;
	int				score;		// Current score
	double			dist;
	double			low_dist = UNDEFINED;
	int				elem_idx;
	CDbEntity*		db_ent;
	DWORD			drop_one;

	CDWordArray* this_drop = new CDWordArray;
	CDWordArray* high_drop = new CDWordArray;
	CDWordArray* tmp_drop = NULL;

	high_drop->RemoveAll();

	db_ent = src_list[anchor_idx];
	CDbHole* anchor = dynamic_cast<CDbHole*>(db_ent);
	if (!anchor)
		return FALSE;

	// -------------------------------------------------
	// For each tool that can drill the anchor hole,
	//	see how many other holes could be drilled.
	//
	int high_score = -INT_MAX;
	int tool_num = tool_list.Count();
	for (tool_idx=0; tool_idx<tool_num; tool_idx++)
	{
		tool = dynamic_cast<CDbTool*>( tool_list[tool_idx] );
		if (!tool_match_elem( *anchor, *tool ))
			continue;

		this_drop->RemoveAll();
		score = score_one_tool( src_list, tool_list, mach_ext, *anchor, *tool, first_hole, last_hole, this_drop );

		if (score >= high_score)
			dist = drop_distance( src_list, tool_list, *drop_pnt, this_drop );

		if ( (score > high_score)
			|| ( (score == high_score)
				&& (dist < low_dist) ) )
		{
			high_score = score;
			low_dist = dist;

			// Swap drop lists... whee!
			tmp_drop = high_drop;
			high_drop = this_drop;
			this_drop = tmp_drop;
		}
	}

	// -------------------------------------------------
	//	Iterate the high-drop, and make a hole that drills
	//	all of those tools.  Note that each tool remembers
	//	what hole it was drilling! Love those attributes
	//
	if (high_score > 0)
	{
		CString	dropstr;
		int		station;

		CString stationstr = anchor->Tool()->StringGet( STR_NC_CODE_NUMBER, "" );
//		int low_station = atoi( stationstr );
//		CDbHole*	low_entity = anchor;

		int low_station = INT_MAX;
		CDbHole*	low_entity = NULL;

		int dropnum = high_drop->GetSize();
		int* sortidx = new int[dropnum];

		// stupid-ass insertion sort
		int minidx;
		int minstation;
		int exclude = -1;
		for (int idx1=0; idx1<dropnum; idx1++)
		{
			minidx = -1;
			minstation = INT_MAX;
			for (idx=0; idx<dropnum; idx++)
			{
				drop_one = high_drop->GetAt( idx );
				tool_idx = LOWORD(drop_one);

				if ( (tool_idx < minstation)
					&& (tool_idx > exclude) )
				{
					minstation = tool_idx;
					minidx = idx;
				}
			}
			sortidx[idx1] = minidx;
			exclude = minstation;
		}

		// Extract the stuff and build a string
		for (idx=0; idx<dropnum; idx++)
		{
			// Hack Patch... TODO:  Why does the drop get -1 indices on that bad file?
			if (sortidx[idx] < 0)
				continue;

			drop_one = high_drop->GetAt( sortidx[idx] );
			tool_idx = LOWORD(drop_one);
			elem_idx = HIWORD(drop_one);

			tool = dynamic_cast<CDbTool*>(tool_list[tool_idx]);
			db_ent = src_list[elem_idx];

			C3dVec offset = tool->AggregateOffset();

			db_ent->DoAction();

			if (idx)
				dropstr += " ";

			stationstr = tool->StringGet( STR_NC_CODE_NUMBER, "" );
			station = atoi( stationstr );

			dropstr += stationstr;

			if (station < low_station)
			{
				low_station = station;
				low_entity = (CDbHole*)db_ent;

				// TODO:  Collect the 3 or 4 places I do this aggregate anchoring and make a function
				*drop_pnt = low_entity->Coord(0) - offset;
			}
		}

		low_entity->StringSet( STR_NC_CODE_NUMBER, dropstr );

		cutOrder->Append( low_entity );

		delete sortidx;

		okay = TRUE;
	}
	else
	{
		okay = FALSE;
	}

	delete this_drop;
	delete high_drop;

	return okay;
}


// =======================================================================
//		tool_match_elem
//
//		Given an element index and a tool index,  ensure that the
//		tool can accurately cut the element
//
bool
COptimizer::tool_match_elem( 
	const CDbHole&	anchor,
	const CDbTool&	tool )
{
	// Setup the two tools we are comparing...
	CDbTool* anchor_tool = anchor.Tool();
	if (!anchor_tool)
		return FALSE;

	// Check various attributes and values... one at a time
	// for easy debugging and extension.
	//
	CString tface = tool.StringGet( STR_WORKPLANE, "ERROR" );
	CString eface = anchor.Workplane()->Name();
	if (tface.CompareNoCase( eface ) != 0)
		return FALSE;

	// Diameter
	double tdia = tool.EffectiveDiameter();
	double edia = anchor.Diam();
	if (!EQUAL( tdia, edia ))
		return FALSE;

	// Tool type
	CString ttype = tool.StringGet( STR_TYPE, "UNKNOWN" );
	CString etype = anchor.Tool()->StringGet( STR_TYPE, "UNDEFINED" );
	if ( ttype.CompareNoCase( etype ) != 0)
		return FALSE;

	// Tool length... can it do the job?
	double length = tool.DoubleGet( STR_LENGTH, 0. );
	if (length > 0)
	{
		if (anchor.Type() == DBHOLE)
		{
			if (anchor.Depth() > length)
			{
				return FALSE;
			}
		}
		else // NOT hole
		{
			// TODO:  Cope with non-holes, if we ever care
			return FALSE;
		}
	}

	return TRUE;
}



// =======================================================================
//		score_one_tool
//
//		Find the score of the drop associated with placing the indicated
//		tool on the anchor hole.
//
//		Note that each matching tool will keep the index of it's hole
//		in its attributes for future reference.
//		
int
COptimizer::score_one_tool( 
	const CDbEntityArray&	src_list, 
	const CDbEntityArray&	tool_list, 
	const C2dBox&			mach_ext,
	const CDbHole&			anchor_hole,
	const CDbTool&			anchor_tool,
	int						first_hole,
	int						last_hole,
	CDWordArray*			this_drop )
{
	int				hole_idx;		// Current hole in scan
	int				tool_idx;		// Tool in scan
	int				score;			// score
	
	CDbHole*		hole;
	CString			hole_face;
	C3dCoord		hole_end;
	double			hole_depth;

	C3dVec			tool_offset;
	C3dCoord		tool_pnt;
	int				tool_station;

	DWORD			drop_one;

	if ( anchor_hole.DidAction() )
	{
		return -1;
	}

	//
	// Remember what face the anchor is on... we ONLY test 
	// holes on that face
	//
	CString anchor_face = anchor_hole.Workplane()->Name();
	C3dCoord anchor_end = anchor_hole.Coord( 0 );
	double anchor_depth = anchor_hole.Depth();
	
	int anchor_station = anchor_tool.IntGet( STR_NC_CODE_NUMBER, -1 );

	// Argh... force writeability in tool
	tool_offset = ((CDbTool&)anchor_tool).AggregateOffset();

	anchor_end -= tool_offset;

	//
	// For each hole in the block, see if it fits the tooling array as
	// anchored at the anchor hole.
	//
	score = 0;
	for (hole_idx=first_hole; hole_idx<=last_hole; hole_idx++)
	{
		hole = dynamic_cast<CDbHole*>( src_list[hole_idx] );
		if ( !hole->DidAction() )
		{
			// Find out some details about our new hole
			hole_end = hole->Coord(0);
			hole_face = hole->Workplane()->Name();
			hole_depth = hole->Depth();

			// Only optimize one face at a time
			if ( anchor_face.CompareNoCase( hole_face ) != 0)
				continue;

			if (!EQUAL( anchor_depth, hole_depth ))
				continue;

			// TODO: Check to see if it is in range of the anchor point
			//		(within gross tooling extents)

			// Check all the tools, to see if this element hits one
			for (tool_idx=0; tool_idx<tool_list.Count(); tool_idx++)
			{
				CDbTool* tool = dynamic_cast<CDbTool*>(tool_list[tool_idx]);
				if ( tool_match_elem( anchor_hole, *tool ) )
				{
					if (tool_reaches( *tool, hole_end, mach_ext ) )
					{
						tool_station = tool->IntGet( STR_NC_CODE_NUMBER, (anchor_station - 1) );

// Allow this "shift" condition
//						if (tool_station <= anchor_station)
						{
							tool_offset = tool->AggregateOffset();

							tool_pnt = anchor_end + tool_offset;

							if ( CLOSE( hole_end.X(), tool_pnt.X(), m_spacetol )
								&& CLOSE( hole_end.Y(), tool_pnt.Y(), m_spacetol ) )
							{
								// Hit!
								score++;

								// Record the element this tool hit...
								drop_one = hole_idx&0xffff;
								drop_one<<=16;
								drop_one += tool_idx&0xffff;
								this_drop->Add( drop_one );
								break;
							}
						}
					}
				}
			}
		}
	}

	return score;
}

// =======================================================================
//		tool_reaches
//
//		Determine if the given tool index can reach the specified
//		point.... on Face 1
//
bool
COptimizer::tool_reaches(
	 const CDbTool&		tool,
	 const C3dCoord&	pnt,
	 const C2dBox&		mach_ext )
{
	C3dCoord	anchor;

	// Argh... force writeability in tool
	C3dVec	offset = ((CDbTool&)tool).AggregateOffset();

	anchor = pnt - offset;

	if ( !mach_ext.Contains( anchor.X(), anchor.Y(), SMALL ) )
	{
		return FALSE;
	}

	return TRUE;
}


// =======================================================================
//		drop_distance
//
//		Given a drop array, determine the anchor position and return
//		the distance from the specified point.
//
//		NOTE:  calculates City distance
//
double
COptimizer::drop_distance( 
	const CDbEntityArray&	src_list,
	const CDbEntityArray&	tool_list,
	const C3dCoord&			drop_pnt,
	CDWordArray*			drop_array )
{
	int			elem_idx;
	CDbHole*	db_hole;
	int			tool_idx;
	CDbTool*	tool;
	int			base_station;
	int			station;
	C3dCoord	base_pnt;
	int			idx;
	DWORD		drop;

	base_pnt = drop_pnt;
	base_station = INT_MAX;

	for (idx=0; idx<drop_array->GetSize(); idx++)
	{
		drop = drop_array->GetAt( idx );
		tool_idx = LOWORD(drop);
		tool = dynamic_cast<CDbTool*>(tool_list[tool_idx]);

		C3dVec	offset = tool->AggregateOffset();

		station = tool->IntGet( STR_NC_CODE_NUMBER, -1 );
		if (station < base_station)
		{
			elem_idx = HIWORD(drop);
			db_hole = dynamic_cast<CDbHole*>(src_list[elem_idx]);

			base_station = station;
			base_pnt = db_hole->Coord(0) - offset;
		}
	}


	return ( fabs(drop_pnt.X() - base_pnt.X())
				+ fabs (drop_pnt.Y() - base_pnt.Y()) );
}

// For sorting the workzones on increasing order of absolute repo position.
int
COptimizer::ZoneCompare( const void* ptrA, const void* ptrB )
{
	CWorkPkg* workPkgA = ( *(CWorkPkg**) ptrA);
	CWorkPkg* workPkgB = ( *(CWorkPkg**) ptrB);

	CDbFeature* dbFeatureA = workPkgA->Feature();
	int zoneA = dbFeatureA->IntGet( "_zone_num", 0 ); // [1..N]

	CDbFeature* dbFeatureB = workPkgB->Feature();
	int zoneB = dbFeatureB->IntGet( "_zone_num", 0 ); // [1..N]

	return (zoneA - zoneB);
}

// NOTE: This function is (should be) used ONLY when (the user has
// requested that punching code be output as individual hits AND
// he has not exploded his model into individual hits).
//
// returns (true) when the entity is replaced with individual hits.
bool
COptimizer::Keep( CDbEntity* dbEntity, CDbEntityArray* cutOrder )
{
	bool replaced = false;

	if ( m_nibble )
	{
		CDbCurve*	dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

		if ((dbCurve != NULL) && dbCurve->Tool()->IsPunchTool())
		{
			CNibbler		nibbler;
			C3dCoordList	pts;
			CGeoCurve*		geoCurve;
			double			feed, orient;
			int				count, indx;

			feed = CNibbler::DefaultFeedrate( (*dbCurve) );
			geoCurve = dbCurve->Curve();
			nibbler.Nibble(	(*geoCurve), feed, NIBBLE_BALANCED, &pts );
			orient = geoCurve->StartTan().Radians() * RAD2DEG;
			delete geoCurve;

			count = pts.Count();
			if (count > 0)
			{
				CDbHole*		dbHole;
				CDbContainer*	dbContainer = dynamic_cast<CDbContainer*>( dbCurve->Owner() );
				CEntityDb*		db = dbCurve->Db();

				for (indx = 0; indx < count; ++indx)
				{
					db->Create( DBHOLE, (CDbEntity**) &dbHole );
					dbHole->Init(
						dbCurve->Tool(),
						dbCurve->Workplane(),
						(*pts[indx]), 0.01, 0. );
					
					dbHole->ColorSet(
						dbCurve->Tool()->ColorGet(DCOLOR_RED) );

					// 2004.05.11 (PE) -- Index angle was failing to be output
					// by code generator.  Of course, we still have a problem
					// dealing with arcs, but that has not become a problem yet.
					dbHole->DoubleSet( "orient", orient );

					cutOrder->Append( dbHole );
				}

				dbCurve->Delete();
				replaced = true;
			}
		}
		else
		{
			cutOrder->Append( dbEntity );
		}
	}
	else
	{
		cutOrder->Append( dbEntity );
	}

	return replaced;
}
