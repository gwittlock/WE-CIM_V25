
#include "stdafx.h"
#include <float.h>
#include "assert.h"
#include "MathConst.h"
#include "StringConst.h"
#include "AppConst.h"
#include "CommonFlags.h"  // for XFORM_ flags
#include "Register.h"

#include "NestMgr.h"
#include "FileSnoop.h"
#include "DbLine.h"
#include "DbTool.h"
#include "DbIterator.h"
#include "MM2.h"

#include "Lead.h"
#include "Selector.h"
#include "ModelUtil.h"
#include "AutoMod.h"

#include "SeqRules.h"
#include "TSP.h"

#include "Command.h"

#include "TrueRemnant.h"
#include "PartPlace.h"
#include "NestedArea.h"

#if (_CI || _NST)
#include "Portal.h"
#include "CiModelConvertor.h"
#include "ModelText.h"
#endif

#if (_NST)
#include "ModelText.h"
#endif

#if (_PM4)
#include "ScModel.h"
#include "ScModelConvertor.h"
#endif

#include "Register.h"

// Thank the Lord for people who share their hard work!
#include "ProgressWnd.h"

// ==================================================================

static const int SEED_PRIORITY = 100;
static const int MARKOV_PRIORITY = 100;
static const int GRID_PRIORITY = 110;
static const int START_PRIORITY = 120;

static const int PRENEST_FACTOR = 2;

static CString g_msg;

// ==================================================================

// Prior to V18, nesting would *not* strictly honor the sorting
// order of parts. That is, nesting would always place *oversized*
// parts first, regardless of the sorting order. We use
// g_v17_large_part_pass to enable this legacy behavior.
static bool g_v17_large_part_pass;

CNestMgr::CNestMgr()
{
	m_lasthit = NULL;

	// lpp (ie. Large Part Pass)
	g_v17_large_part_pass = (CRegister::IntGetV( "Nesting", "lpp", FALSE ) != FALSE);

}

CNestMgr::~CNestMgr()
{
}

CReturn CNestMgr::Execute(
	CCommand*			io_cmd, 
	CNestConfig&		config, 
	CSheet*				sheet, 
	CPartBin*			partbin,
	CViewMgr&			view )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;
	CString note;
	CCommand window;
	CString cmdstr;
	int	idx;

	CModel&	model = io_cmd->getModel();

	sheet->SeedMgr( &m_seed_mgr );

	// Prepare for nesting...
	config.DidTaskReset();
	config.DidTask(DID_TOOL);	// Set it; may be cleared later on a failure

	ret += config.ClearResults();
	if (!ret.isOkay())
		return ret;

	// How many parts to nest?
	int parts_to_cut = 0;
	config.FillParts(false);
	for (idx = 0; idx < partbin->Count(); ++idx)
	{
		CNestingPart* npart = partbin->GetAt(idx);

		if (npart->Error())
			continue;

		if ( npart->Filler() )
			config.FillParts(true);
		else
			parts_to_cut += partbin->GetAt(idx)->Quantity();
	}

	config.OneSheet(false);
	if ((partbin->Count() > 0) && (parts_to_cut < 1))
		config.OneSheet(true);

	config.PartsToCut( parts_to_cut );

	int sheet_count = 0;

#if (_CI || _NST)
	{
		int bg_color = CRegister::IntGetV( "Preferences", "back_color", 0 );

		cmdstr.Format( "view:mode: profmark=0, tooldash=0, color_bg=%d",
			((bg_color == 0) ? DCOLOR_BLACK : DCOLOR_WHITE) );

		window.setCommand( cmdstr, sheet->pModel(), &view );
		view.Mode(&window);

		// Set our View to encompass the sheet
		cmdstr.Format( "view:window: regen=0, sx=-10, sy=-10, ex=%f, ey=%f",
			sheet->Length()+10, sheet->Width()+10 );
	}
#else
	// Set our View to encompass the sheet
	cmdstr.Format( "view:window: regen=0, sx=0, sy=0, ex=%f, ey=%f",
		sheet->Length(), sheet->Width() );
#endif


	window.setCommand( cmdstr, sheet->pModel(), &view );
	view.Window(&window);

	view.ModelSet( *(sheet->pModel()) );

	// Until we are done, nest sheets
	while ( ((config.PartsToCut() > 0) && ret.isOkay()) || config.OneSheet() )
	{
		// New sheet, so redraw and do some prompting
		view.Refresh( true );
		
		CProgressWnd::SetText("Nesting ...");

		// Reset any part/pattern flags as necessary.
		for (int idx=0; idx<partbin->Count(); idx++)
		{
			CNestingPart* npart = partbin->GetAt(idx);
			npart->Used(0);
			npart->Tooled( FALSE );
			for (int hit=0; hit<npart->Count(); hit++)
			{
				CToolHit* toolhit = npart->ToolHit(hit);
				toolhit->hasPatterns( FALSE );
				toolhit->PatternBurn( NULL );
				toolhit->PatternPunch( NULL );
				toolhit->PatternOther( NULL );
			}
		}

		// Mark parts as large, record the longest
		// part and strip leads from large parts.
		LargePartsPrep( (*sheet), partbin, &config );

		// Start up the repo zone tracking system for this sheet
		CRepoZone repo_zone;
		repo_zone.InitRepo( config, *sheet );

		// Now fill this one sheet
		ret += nest_sheet( config, repo_zone, partbin, sheet, view );

		// If we achieved a nest, mangle the resulting sheet
		if (ret.IsOk() && config.DidNest())
		{ 
			CDbLine* cutback;

			sheet_count++;

#if (!_CI && !_NST)
			// Repositions
			CRepo repo( sheet->pModel(), config.CmdbPath(), config.LeadSetup() );
			ret += nest_sheet_repo( config, repo, repo_zone, sheet, view );

			// DropStop... duh
			if (config.DropStop())
				sheet->DropStop( config );
#endif

			// Update the part & material inventories, and also determine
			// whether this nested sheet of parts can be duplicated.
			PartsAccounting( &config, partbin, sheet );

			// Remnants and Cutback
			C2dBox content = sheet->BoxUser();

			// A small value... left to cutback position in the right
			sheet->Cutback( content.Xmax() + config.Border(BORDER_RIGHT) );

			cutback = NULL;
			if ( config.Cutback() && !sheet->IsRemnant() )
				cutback = cutback_sheet( config, sheet, view );

			if ( config.Remnant() )
			{
				note = "Calculating true-shape remnant";
				CProgressWnd::SetText( note );

				remnant_sheet( config, sheet, content, view );
			}

#if (!_CI && !_NST)
			// Chain Grid?
			if (config.GridChain())
			{
				Chain(*(sheet->pModel()), config.ChainNum(), config.ChainOffset());
			}

			// Lead 
			ret += nest_sheet_lead( config, sheet, view );

			// Shift pierce holes out into main zones
			if (config.ShiftPierce())
				repo.ShiftPierce(config.MachineReal( "Torch Offset in X", 0.0 ) );

			// Expand remaining @CMD attributes and/or commands
			// Currently the Repo ones are managed by repo... but
			// we do have the @DROPSTOP still.
			ret += nest_sheet_attrib( config, sheet, view );

			// Special progressive packaging for Franklin
			if (config.Progressive())
			{ 
				CRepo repo( sheet->pModel(), config.CmdbPath(), 0 );

				ret += repo.Progressive( config.MachineReal( "Torch Offset In X", 0.0 ),
										config.MachineTravelX(),
										sheet->Length(),
										sheet->Width() );
			}
#endif

			// Save the sheet
			note = "Saving nested sheet.";
			CProgressWnd::SetText( note );

			// BugID 650 -- Unused tools were always stripped.
			if ( config.StripTools() )
				CModelUtil::EmptyStationsDelete( (&sheet->pModel()->Db()) );

#if (_CI || _NST)
			ret = SaveSheet( config, (*partbin), sheet_count, sheet );
			config.SavePassResults( sheet_count, *sheet, *partbin );
#else
			sheet->Label( *partbin );
			sheet->Save( config.ResultName(), config, sheet_count );
			config.SavePassResults( *sheet, *partbin );
#endif

			// Clear any temporary graphics.
			if (config.DisplayParts())
				view.Refresh( 0, false );
		}

		// If we aren't done yet, prepare the next sheet of material
		if (config.PartsToCut())
		{
			if (!config.DidNest())
				ret += sheet->Next( config );
			ret += sheet->Prepare( config, view, io_cmd->getModel() );
		}
	}

	// Tell our client how many sheets we built
	io_cmd->setInt( "Sheet Count", sheet_count );

#if (!_CI && !_NST)
	// Store the final statistics in the database
	config.SaveFinalResults( *sheet, *partbin );
#endif

	// Report on our activities
	if ( !sheet_count || ret.isError())
	{
		WORD didit = config.DidTask();

		if (!sheet_count)
		{
			ret.User( IDS_NEST_DIDIT1 );
			ret.User( IDS_NEST_DIDIT2 );
			if ( (didit == DID_ALL)
				&& !config.Oversize() )
			{
				ret.User( IDS_NEST_DIDIT3A );
			}
			else
				ret.User( IDS_NEST_DIDIT3B  );
		}

		didit = ~didit;
		if (didit & DID_COUNT)
			ret.User( IDS_NEST_DIDIT_COUNT );
		if (didit & DID_SEED)
			ret.User( IDS_NEST_DIDIT_SEED );
		if (didit & DID_SCORE )
			ret.User( IDS_NEST_DIDIT_SCORE );
		if (didit & DID_TOOL )
		{
			ret.User( IDS_NEST_DIDIT_TOOL, config.DidFailTool() );
		}
		if (didit & DID_YIELD)
			ret.User( IDS_NEST_DIDIT_YIELD );
		if (config.Oversize())
			ret.User( IDS_NEST_OVERSIZE );
	}

	return ret;
}

// Fill one sheet in as many repositions as needed
CReturn CNestMgr::nest_sheet(
	CNestConfig&	config,
	CRepoZone&		repo_zone,
	CPartBin*		partbin,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn ret;
	CString note;

	sheet->ViewMgr( &view );

	config.DidNest(false);
	config.DidLarge(false);
	config.TryTask( DID_SCORE );

	// See how the largest large-part fits into the zone list...
	int min_hot = 0;
	int repo_num = repo_zone.Count();

	bool get_hot = (g_v17_large_part_pass ? config.Active(0) : config.HaveLargeParts());
	if (get_hot)
	{
		for (int repo_cnt=0; repo_cnt<repo_num; repo_cnt++)
		{
			C2dBox zone = repo_zone.Zone(repo_cnt);
			if (config.LargePartSize() < zone.Xmax())
			{
				min_hot = repo_cnt+1;
				break;
			}
		}
	}

	// Cut the sheet in as many passes as needed
	int repo_cnt;
	for (repo_cnt=0; (repo_cnt<repo_num) 
						&& ( config.PartsToCut() 
							|| ( config.OneSheet()
							|| config.FillToSheet()) );
						repo_cnt++)
	{
		note.Format( "Zone: %d of %d", repo_cnt+1, repo_num );
		CProgressWnd::SetText( note );

		if (config.ClampAround())
		{ repo_zone.ClampGrid( config, *sheet, repo_cnt, TRUE ); }


		if (config.FirstPartInspection() == 1)
		{
			config.MustSeedBarrier( true );
			ret += nest_repo( config, repo_cnt, repo_zone, partbin, sheet, view );
			config.FirstPartInspection(0);
		}

		// Do the nest for this repo
		config.MustSeedBarrier( true );
		ret += nest_repo( config, repo_cnt, repo_zone, partbin, sheet, view );

		// End of nest reset for this repo zone
		if (config.ClampAround())
		{ repo_zone.ClampGrid( config, *sheet, repo_cnt, FALSE ); }
		repo_zone.ClampMark( config, sheet, repo_cnt );

		repo_zone.Mark( sheet, repo_cnt );

	}  // end for repo_cnt
	repo_zone.HotRepo( max( repo_cnt, min_hot ) );
//	repo_zone.DidRepo( repo_cnt );

	config.OneSheet(false);
	config.FillToSheet(false);

	// Check efficiency (so far) and put these (and the remaining) 
	// parts on hold if it's not good enough... and cancel this nest.
	config.TryTask(DID_YIELD);
	if (config.DoSheetHold( *sheet, *partbin ))
		config.DidNest(false);
	else
		config.DidTask(DID_YIELD);
				
	return ret;
}

// Fill this reposition zone, in as many passes as needed
CReturn CNestMgr::nest_repo(
	CNestConfig&	config,
	int				repo_cnt,
	CRepoZone&		repo_zone,
	CPartBin*		partbin,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn ret;
	CString note;
	CNestedArea*	sheet_as_nested_area;

	// TODO:  Determine the correct sign use of Torch Offset
	double	torch_offset = fabs(config.MachineReal( "Torch Offset in X", 0.0 ));
	bool	do_pop = false;

	int first_pass = (g_v17_large_part_pass ? 0 : 1);
	// Multiple pass (priority) nesting through this area
	for (int nest_pass=first_pass; nest_pass<=config.PassNum(); nest_pass++)
	{
		if ( !ret.isOkay() )
			break;

		if (!config.Active( nest_pass ))
			continue;

		config.Pass( nest_pass );

		ret += CProgressWnd::ProgressWndUpdate();
		if ( !ret.IsOk() )
			break;

		note.Format( "Pass: %d, Pending: %d", nest_pass, config.PartsToCut() );
		CProgressWnd::SetText( note );

		// Prepare the actual working area for this pass/zone...
		// ...pass zero only effective on repo zero; for large parts use whole sheet

		C2dBox edge = repo_zone.Zone(repo_cnt);
		if (nest_pass == LARGE_PART_PASS)
		{
			if (repo_cnt == 0)
			{
				edge = sheet->Extent();
			}
			else
				continue;
		}

		bool torch_shift = false;
		if ((repo_cnt+1) < repo_zone.Count())
		{
			C2dBox next_edge = repo_zone.Zone(repo_cnt+1);
			double repo = next_edge.Xmin() - edge.Xmin();

			// TODO: Define proper signes for these
			if (repo <= torch_offset)
			{ torch_shift = true; }
		}

		CGeoPolyArray* out_array = sheet->ToolHitPoly(HIT_NEST_OUTSIDE);

		// If we've cut our final part, shrink the area to limit fills?
		if (!config.PartsToCut() && !config.OneSheet() && !config.FillToRepo())
		{
			C2dBox user = sheet->BoxUser();
			edge.Update( edge.Xmin(), edge.Ymin(), user.Xmax(), edge.Ymax() );

			config.FillToRepo(TRUE); // Lie so this BoxUser() is skipped next pass.
		}

		//**********************************************************************

		sheet_as_nested_area = sheet->SheetNestedArea();

		if (g_v17_large_part_pass)
		{
			m_seed_mgr.Push();
			ret += nest_area( config, edge, out_array, sheet_as_nested_area, false,
				nest_pass?repo_cnt:LARGE_PART_ZONE, torch_shift, partbin, sheet, view );
			m_seed_mgr.Pop();
		}
		else
		{
			// 2006.10.03 (PE) -- We do this conditionally because I discovered that
			// we were loosing valuable primary/secondary seeds that were being
			// generated by CSheet::ExactSeedsDerive(). This loss would cause
			// non-optimal nesting of a sheet of several different single large parts.
			//
			// NOTE: This 'fix' also yields an ever-so-slightly better nest in that
			// our standard demo will nest an additional dxfdemo7 when nesting along
			// the Y axis and an additional dxfdemo6 when nesting along the X axis.
			if ( !m_seed_mgr.HasSeeds() )
			{
				m_seed_mgr.Push();

				// CRITICAL: We must prevent the 'last placed part' from
				// adversely affecting placement of the next part.
				sheet->DidPlace( false );
			}

			ret += nest_area( config, edge, out_array, sheet_as_nested_area, false,
				repo_cnt, torch_shift, partbin, sheet, view );

			// 2006.10.03 (PE) -- We want to retain the valuable seeds.
			do_pop = (m_seed_mgr.ConditionalPop() == false);
		}
		//**********************************************************************

		if (config.FirstPartInspection() > 1)
			break;
	}

	if ( do_pop )
		m_seed_mgr.Pop();

	return ret;
}

CReturn CNestMgr::nest_part_in_part(
	CNestConfig&	config,
	const C2dBox&	edge,			// Area on the sheet to do our nesting
	CNestedArea*	area_to_nest,
	int				zone_num,		// Zone marker for nested aprts
	bool			torch_shift,	// True if split pattern torch put in next zone
	CPartBin*		partbin,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn		status;
	C2dBox		part_box;
	CGeoPoly*	hole_poly;


	// TODO: Extend to handle multiple holes in part.

	// TODO: Fix this for bitmap nesting.
	if ( CNestConfig::IsBitmapNest() )
		// hole_poly = sheet->ToolHitPoly(HIT_NEST_INSIDE)->GetAt(indx);
		hole_poly = area_to_nest->HolePolyGet();
	else
		// hole_poly = sheet->ToolHitPoly(HIT_PART_INSIDE)->GetAt(indx);
		hole_poly = area_to_nest->HolePolyGet();

	if (hole_poly != NULL)
	{
		part_box = hole_poly->Extent();

		// Consider only those holes that intersect the current workzone.
		// TODO: As a minor optimization, we can further restrict consideration
		// to those holes that are potentially large enough to encompass a part.
		if (part_box.Xmax() > edge.Xmin())
		{
			// Temporarily override the progression because we
			// (generally) get better nesting using PROGRESS_PPY_SPX.
			eNestProgression progression = config.Progression();
#if ORIGINAL_CODE
			config.Progression( PROGRESS_PPY_SPX );
#endif
			// By pushing a new bank of seeds, we restrict
			// candidate seeds to just those associated with
			// the current poly.
			m_seed_mgr.Push();

			// More state stuff that we need to maintain.
			C2dBox fit_zone = sheet->FitZone();

			int tmp = config.Pass();
			for (int idx = (tmp - COMMON_PASS + 1); idx < partbin->Count(); ++idx)
			{
				config.Pass( idx + COMMON_PASS );  // Otherwise CanProcess() won't work.

				status += nest_area( config, part_box, NULL, area_to_nest,
					true, zone_num, torch_shift, partbin, sheet, view );
			}

			m_seed_mgr.Pop();

			config.Pass( tmp );

			config.Progression( progression );
		}
	}

	return status;
}

CReturn CNestMgr::nest_area(
	CNestConfig&	config,
	const C2dBox&	edge,			// Area on the sheet to do our nesting
	CGeoPolyArray*	poly_array,		// Polygon(s) that define seeds...
	CNestedArea*	area_to_nest,
	bool			hole,			// TRUE if we are nesting inside a hole
	int				zone_num,		// Zone marker for nested aprts
	bool			torch_shift,	// True if split pattern torch put in next zone
	CPartBin*		partbin,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn ret;
	CString note;
	int		shift_seeds;
	CNestingPart* punched_part;
	CGeoPoly* hole_poly;
	bool	nesting_in_part;

	config.CurrentZoneNumber( zone_num );

	// Now the pattern flags...
	int idx;
	for (idx=0; idx<partbin->Count(); idx++)
	{
		CNestingPart* npart = partbin->GetAt(idx);

		if (config.FirstPartInspection() == 1)
		{
			// NOTE: This might not be the best place to reset first part inspection.
			if (npart->FirstPartInspection() < 2)
			{
				// ie. Seeds were generated for this part but the part was not placed.
				npart->FirstPartInspection( 0 );
			}
		}

		for (int hit=0; hit<npart->Count(); hit++)
		{
			CToolHit* toolhit = npart->ToolHit(hit);
			toolhit->Height(-1.0);
			toolhit->Pos( C2dCoord( UNDEFINED, UNDEFINED ) );
		}
	}

	nesting_in_part = (area_to_nest->PartGet() != NULL);
	ZoneAdjust( config, edge, sheet, nesting_in_part );

	sheet->clearRecentPart();
	set_fill_barrier( config, partbin, sheet, config.OneSheet() );

	// Prepare the seeding for this area
	config.NewLine(false);
	config.ForceSeed(true);

	bool fillseed = config.FillSeed();
	config.FillSeed(true);

	if (area_to_nest != NULL)
	{
		// We are nesting either to the interior of
		// a sheet or to the interior of a part.
		hole_poly = area_to_nest->HolePolyGet();

		if ( config.DebugWire() )
			hole_poly->Draw();

		// By using a 'flip' value of -1, we effectively treat
		// the hole_poly as if it is reversed.
		sheet->SeedPolyConfig( false );
		sheet->SeedPoly( config, hole_poly, -1.0, false );

		// 2007.11.04 (PE) -- I discovered a case where 1) the sheet had
		// sufficient room for more parts and 2) there are more smaller
		// parts that remain to be nested and 3) we failed to place a
		// candidate part just prior to getting *here*. Where as seeding
		// the inside poly is sufficient for an area that has not yet
		// been nested, we must also seed the outside of its contained polys.
		//
		// TODO: Minor optimization: defer sorting of seeds till
		// after they have all been generated.
		int ia_cnt = area_to_nest->InteriorAreasCount();
		for (int ia_indx = 0; ia_indx < ia_cnt; ++ia_indx)
		{
			CNestedArea* ia_na = area_to_nest->InteriorAreaGet( ia_indx );
			CGeoPoly* outside = ia_na->PolysGet( HIT_NEST_OUTSIDE )->GetAt(0);

			// NOTE: 'flip' determines if seeds get generated for
			// a curve based upon it sense of direction. Prior to
			// revamping 'nesting on remnants', flip was always 1.
			//    See also CSheet::MaterialProcess()
			double flip = 1.;
			if (outside->HasAttrib() && (outside->IntGet( "remnant", 0 ) > 0))
				flip = -1.;

			// 2009.12.19 (PE) -- When seeding the outside of a part, we
			// want to force CSheet::ExactSeedsDerive() to retain all seeds
			// that are derived from part exteriors. I am not so sure what
			// the expected/necessary behavior is wrt. seeds that are
			// derived from from a remnant (because we don't have much
			// experience with that scenario).
			sheet->SeedPolyConfig( (flip > 0.) );
			sheet->SeedPoly( config, outside, flip, true );
		}
	}
	else
	{
		// #if BEFORE_V19 -- this tag is here simply so that we can
		// find this dead block of code.
		// 
		// TODO: It appears this conditional block is dead code !!!
		int num = poly_array->Count();
		for (int idx=0; idx<num; idx++)
		{
			hole_poly = poly_array->GetAt(idx);

			if (config.MustSeedBarrier() && (idx == RESERVE_BARRIER))
			{
				sheet->SeedBarrier( config, hole_poly, +1.0 );
			}
			else if (idx >= RESERVE_NUM)
			{
				if (edge.Intersects( hole_poly->Extent(), SMALL ))
				{
					// 2009.12.19 (PE) -- According to a preceding comment,
					// this is inactive code BUT ... in case it isn't ...
					// we just default to the previous behavior.
					sheet->SeedPolyConfig( false );

					// The bounding box of this part-poly intersects
					// the bounding box of the repo-zone poly.  As such,
					// we can use this part to generate seeds.
					if ( CNestConfig::IsBitmapNest() )
						sheet->SeedPoly( config, hole_poly, -1.0, true );
					else
						sheet->SeedPoly( config, hole_poly, +1.0, true );
				}
			}
		}

		if (zone_num)
		{
			sheet->SeedPartEdge(config);
		}

		shift_seeds = (!hole && config.ClampAround() &&
			!config.OppositeClamps() && config.GridSquare());

		if ( shift_seeds )
		{
			// IF we are nesting around clamps, and we are starting on a fairly empty sheet
			// (e.g. no geometry), and we are starting the nest near the clamps, AND we 
			// are in grid/strong grid mode... we need to shift our post-seeds down past the clamps.
			// Fun, eh?
			// Width composite
			double dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Width", 0.0 );
			double dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Width", 0.0 );
			
			double width = max( dval1, dval2 );
			if (config.YNegative())
				width = -width;

			// TODO: Shifting should be done directly via the SeedMgr.
			if (fabs(width) > SMALL)
				sheet->SeedsShift( SECONDARY_BANK, C2dVec( 0., width ) );
		}
	}
	config.FillSeed(fillseed);

	config.TryTask( DID_COUNT );
	if ( !config.PartsToCut() && !config.FillParts() )
		return ret;

	config.DidTask( DID_COUNT );

	config.DidScore( true );

	// TODO: WTF is this about, especially the final assignment of 'm_lastcol' ???
	if (config.NestPosY())
		m_lastpos = C3dCoord( 0., 0., 0. );
	else
		m_lastpos = C3dCoord( 0., sheet->Extent().Dy(), 0. );
	m_lastcol = C3dCoord();

	sheet->PartPlacePush();

	C2dCoord prev_world_pt;

	// while ( (config.PartsToCut() || config.FillParts())	&& config.DidScore() )
	while ( (config.PartsToCut() || config.FillParts())	&& m_seed_mgr.HasSeeds() )
	{
		ret += CProgressWnd::ProgressWndUpdate();
		if ( !ret.IsOk() )
			break;

		//**********************************************************************
		CPartPlace best = score_parts( config, partbin, area_to_nest, NULL, sheet, hole, view );
		//**********************************************************************
		if (best.BumpScoreGet() > SMALL)
		{
			config.DidTask(DID_SCORE);

			// See if the tooling fits this sheet.  This can change per sheet
			// based on open stations and what has been placed prior to this point.
			bool did_tool = tool_part( config, best.PartGet(), sheet );
			if (did_tool)
			{
				CNestedArea* nested_area;

				CNestingPart* npart = best.PartGet();
				CToolHit* toolhit = best.ToolHitGet();

				// And just why do we do this here instead
				// of where the mer is calculated?
				if (toolhit->Height() < 0)
					toolhit->Height( toolhit->Mer().Dy() );

				m_lasthit = toolhit;
				m_lastpos = best.WorldPointGet();

				if ( prev_world_pt.IsDefined() )
				{
					C2dVec delta = m_lastpos - prev_world_pt;
					prev_world_pt = m_lastpos;
					
					toolhit->LinkCopyAppend( CLink( toolhit->Index(), 1., delta ) );
				}
				else
				{
					prev_world_pt = m_lastpos;
				}

				// Column tests
				if (config.GridBarrier())
				{
					// TODO: Where the heck does idx get set?
					// Relevant when Force Grid or Chain Grid is active.
					// Where:
					// poly_array -- Polygon(s) that define seeds...
					// idx -- Index to poly (in hole), or -1 for all (on sheet)
					// hole -- TRUE if we are nesting inside a hole
					DoGridStuff( config, poly_array, idx, hole, sheet, best );
				}

				// Now embed this part/rotation into our sheet and note
				//	in the flags what it was we did.
				nested_area = cut_part( config, sheet, zone_num,
					torch_shift, hole, best, area_to_nest, view );

				if (nested_area != NULL)
				{
					// We successfully cut the part.
					CToolHit* th = best.ToolHitGet();
					// Crap result.
					//   CGeoPoly* outside = th->PolysGet( HIT_PART_OUTSIDE )->GetAt(0);
					//
					// Better result but unexpected iterations.
					//   GeoPoly* outside = th->PolysGet( HIT_NEST_OUTSIDE )->GetAt(0);
					CGeoPoly* outside = nested_area->PolysGet( HIT_NEST_OUTSIDE )->GetAt(0);

					if ( config.DebugWire() )
						outside->Draw();

					// 2009.12.19 (PE) -- I believe this call to SeedPolyConfig()
					// is redundant but for the sake of consistency ....
					sheet->SeedPolyConfig( true );
					sheet->SeedPoly( config, outside, +1.0, true );
				}


				config.MustSeedBarrier( false );

				if (g_v17_large_part_pass)
				{
					if (zone_num == LARGE_PART_ZONE)
						config.DidLarge(true);
				}
				else
				{
					if (npart->Large())
						config.DidLarge(true);
				}
				config.DidNest(true);


				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				// Recursively nest parts in parts.
				ret += nest_part_in_part(
					config, edge, nested_area, zone_num,
					torch_shift, partbin, sheet, view );
				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


				// 2007.11.03 (PE) -- CRITICAL: Otherwise TestCore() often
				// (wrongly) fails with PART_OFF_SHEET.
				ZoneAdjust( config, edge, sheet, nesting_in_part );

				// NOTE: If ever punched_part is NULL then
				// something is very, very wrong.
				// ALSO NOTE: You would think this should be the first thing
				// that we check after calling cut_part(), but doing so causes
				// nesting failures.
				punched_part = sheet->PartPlaceGet()->PartGet();
				if (punched_part->Quantity() == 0)
				{
					// CRITICAL: Short circuit next call to CSheet::NextSeedGet().
					sheet->DidPlace( false );

					// Get the heck out of the loop.
					break;
				}

				if (config.FirstPartInspection() == 1)
				{
					// 2. Exit the loop. NOTE: we will re-enter the loop
					//    to place an instance of the next part.
					//
					// 2007.06.16 (PE) -- On second hand, maybe we don't
					// want to exit the loop if we are nesting within a part.
					if (area_to_nest == NULL)
						break;
				}
			}
			else
			{
				sheet->DidPlace( false );

				// Failed the tool, so clear that bit
				config.FailTask( DID_TOOL );
			}
		}
		else  // Failed
		{
			sheet->DidPlace( false );

			if ( config.Oversize() || config.OffSheet() )
			{ 
				m_lasthit = NULL;
			}

			if ( !config.NewLine() && !config.ForceSeed() )
			{
				if (!sheet->SeedNext( config, true ))
					break;
			}

			if (best.BumpScoreGet() <= -UNDEFINED)
				break;  // user-cancel within score_parts()?
		}

		if (config.FirstPartInspection() > 1)
			break;
	} // end while TRUE (inner nest loop)
	
	sheet->PartPlacePop();

	return ret;
}

void CNestMgr::ZoneAdjust(
	CNestConfig&	config,
	const C2dBox&	edge,			// Area on the sheet to do our nesting
	CSheet*			sheet,
	bool			nesting_in_part )
{
	C2dBox	foo;
	double	xmin, ymin;
	double	xmax, ymax;
	double	xmin_delta;
	double	expand_delta;

	// When nesting in the sheet, the outside kerf of the part is allowed
	// to touch the edge of the sheet (regardless of spacing). However,
	// when nesting inside a part, the spacing must be taken into account
	// so as to separate the part edges. In the former case. expanding
	// the bounding box of the sheet appears to do the trick.
	expand_delta = (nesting_in_part ? 0. : config.Spacing());

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// 2006.11.05 (PE) -- Connweld noticed the left-most parts in every
	// workzone (beyond the initial workzone) were nest immediately
	// against the left edge of the workzone. Now we ensure the left
	// border value is considered under this condition,

	xmin_delta = ((config.CurrentZoneNumber() > 0) ? config.Border(BORDER_LEFT) : 0.);
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	foo = C2dBox(
		(edge.Xmin() - (xmin_delta + expand_delta)),
		(edge.Ymin() - expand_delta),
		(edge.Xmax() + expand_delta),
		(edge.Ymax() + expand_delta) );

	sheet->WorkZone( C2dBox( 
		ceil( foo.Xmin() / config.Resolution() ),
		ceil( foo.Ymin() / config.Resolution() ),
		floor( foo.Xmax() / config.Resolution() ),
		floor( foo.Ymax() / config.Resolution() ) ) );

	// ------  ------
	if ( nesting_in_part )
	{
		xmin = edge.Xmin();
		xmax = edge.Xmax();

		ymin = edge.Ymin();
		ymax = edge.Ymax();
	}
	else
	{
		xmin = max( config.Border(BORDER_LEFT), edge.Xmin() );
		xmax = min( sheet->Length() - config.Border(BORDER_RIGHT), edge.Xmax() );

		ymin = max( config.Border(BORDER_BOTTOM), edge.Ymin() );
		ymax = min( sheet->Width() - config.Border(BORDER_TOP), edge.Ymax() );

		xmin += xmin_delta;
	}

	foo.Update( xmin, ymin, xmax, ymax );
	// ------ ------

	sheet->FitZone( config, foo, expand_delta );
}

CReturn CNestMgr::GridNest(
	CNestConfig&	config,
	CPartBin*		partbin,
	CNestingPart*	pre_part,
	CSheet*			sheet,
	CModel&			model,
	CViewMgr&		view )
{
	CReturn status;

	EWMFile( FILE_INFO );

	// NOTE: 'toolhit' gets reused.
	CToolHit* ref_toolhit = pre_part->ToolHit( pre_part->PreHit() );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Calculate sufficient extents for a small sheet
	// on which to place the grid-nested parts.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	C2dBox toolhit_extent = ref_toolhit->Mer();
	double dx = toolhit_extent.Dx();
	double dy = toolhit_extent.Dy();

	// ie. spacing | part | spacing | part | spacing
	double delta = (2. * ((dx > dy) ? dx : dy)) + (3. * config.Spacing());

	// Fudge factor (really just for HardPierce() (?))
	delta += (2. * config.Spacing());

	C2dBox sheet_extent;
	sheet_extent.Update( 0., 0., delta, delta );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CSeedMgr	seed_mgr;
	CPartPlace	best;
	CPartPlace	currbest;

	CGeoPolyArray* poly_array = NULL;
	CNestedArea* area_to_nest = NULL;

	//=-=-=
	// grid link location refinement

	sheet->NestingPhaseSet( PHASE_GRID );
	
	ref_toolhit->LinksDestroy();

	// For each orientation of this part ....
	int orientations = pre_part->Count();
	for (int orient = 0; orient < orientations; ++orient)
	{
		// Managerial stuff.
		sheet->PartPlacePush();
		seed_mgr.Push();
		sheet->SeedMgr( &seed_mgr );

		CToolHit* toolhit = ref_toolhit;
		toolhit_extent = toolhit->Mer();
		
		double xpos, ypos;
		if ( config.NestPosY() )
		{
			// Nesting up the Y-axis so place in lower-left corner of sheet.
			xpos = -toolhit_extent.Xmin();
			ypos = -toolhit_extent.Ymin();
		}
		else
		{
			// Nesting down the Y-axis so place in upper-left corner of sheet.
			xpos = -toolhit_extent.Xmin();
			ypos = sheet_extent.Ymax() - toolhit_extent.Dy();
		}

		C2dCoord llA, llB;
		C2dCoord placeA( xpos, ypos );
		C2dCoord placeB = placeA;

		// TODO: What does this do and can it be moved outside the loop (?)
		// view.ModelSet( (*pre_part->pModel(orient)) ); 

		// Requires 3 iterations:
		//   1. initial part placement
		//   2. part placement in primary direction
		//   3. part placement in secondary direction
		C2dVec delta;
		for (int trial = 0; trial < 3; ++trial)
		{
			status += CProgressWnd::ProgressWndUpdate();
			if ( !status.IsOk() )
				break;

			if (trial == 0)
			{
				// CRITICAL: Without this next statement, attempts to nest later
				// orientations of a part will fail because the seed manager will
				// attempt to use a link to a (non-existing) placed part.
				sheet->DidPlace( false );

				if (currbest.ToolHitGet() == NULL)
				{
					// Inform the sheet of our working area
					sheet->PrepareBlank( config, sheet_extent, model );

					sheet->FillBarrier( config, sheet_extent );
					sheet->clearRecentPart();

					set_fill_barrier( config, partbin, sheet, config.OneSheet() );

					CCommand window;
					CString cmdstr;
					cmdstr.Format( "view:window: regen=0, sx=0, sy=0, ex=%f, ey=%f", sheet_extent.Dx(), sheet_extent.Dy() );
					window.setCommand( cmdstr, sheet->pModel(), &view );
					view.Window(&window);
					view.Clear();
				}

				// Prepare the seeding for this area
				config.NewLine(true);
				config.ForceSeed(true);
				config.PreNest(true);

				// As pilfered from CNestMgr::Execute()
				// Dunno if this is really necessary.
				pre_part->Used(0);
				pre_part->Tooled( FALSE );

				EWMFile( FILE_INFO );

				poly_array = sheet->ToolHitPoly( HIT_NEST_OUTSIDE );
				int count = poly_array->Count();
				for (int indx = 0; indx < count; ++indx)
				{
					if (indx == RESERVE_BARRIER)
					{
						sheet->SeedBarrier( config, poly_array->GetAt(indx), +1.0 );
					}
					else if (indx >= RESERVE_NUM)
					{
						// 2009.12.19 (PE) -- I'm fairly sure this is the correct
						// thing to do since we are seeding the outside of a part.
						sheet->SeedPolyConfig( true );

						CGeoPoly* poly = poly_array->GetAt(indx);
						if (sheet_extent.Intersects( poly->Extent(), SMALL ))
						{
							sheet->SeedPoly( config, poly_array->GetAt(indx), +1.0, true );
						}
					}
				}

				m_lasthit = NULL;

				area_to_nest = sheet->SheetNestedArea();
				toolhit = ref_toolhit;
			}
			else
			{
				toolhit = pre_part->ToolHit( orient );
			}

			// TODO: What does this do and can it be moved outside the loop (?)
			view.ModelSet( (*pre_part->pModel( pre_part->PreHit() )) ); 

			// An optimization which avoids renesting the part
			// corresponding to the current orientation.
			if ((currbest.ToolHitGet() == NULL) || (trial > 0))
			{
				if (trial == 1)
				{
					toolhit_extent = toolhit->Mer();
		
					if ( config.NestPosY() )
					{
						// Nesting up the Y-axis so place in upper-left corner of sheet.
						xpos = -toolhit_extent.Xmin();
						ypos = sheet_extent.Ymax() - toolhit_extent.Ymax();
					}
					else
					{
						// Nesting down the Y-axis so place in lower-left corner of sheet.
						xpos = -toolhit_extent.Xmin();
						ypos = -toolhit_extent.Ymin();
					}

					placeB.XY( xpos, ypos );
				}
				else if (trial == 2)
				{
					toolhit_extent = toolhit->Mer();

					if ( config.NestPosY() )
					{
						// Nesting up the Y-axis so place in lower-right corner of sheet.
						xpos = sheet_extent.Xmax() - toolhit_extent.Xmax();
						ypos = -toolhit_extent.Ymin();
					}
					else
					{
						// Nesting down the Y-axis so place in upper-right corner of sheet.
						xpos = sheet_extent.Xmax() - toolhit_extent.Dx();
						ypos = sheet_extent.Ymax() - toolhit_extent.Ymax();
					}

					placeB.XY( xpos, ypos );
				}

				best = sheet->Score( config, placeB, toolhit, area_to_nest, true, false );
			}
			else
			{
				best = currbest;
			}

			if (best.BumpScoreGet() > SMALL)
			{
				if (trial == 0)
				{
					if (currbest.ToolHitGet() == NULL)
					{
						// This next statement serves mostly as a visual aid (?)
						cut_part( config, sheet, 0, false, false, best, area_to_nest, view );

						// config.FillSeed(true);
						// config.GridSquare(false);
						// pre_part->PreHit(-1);

						currbest = best;
					}

					// The position of the reference part.
					placeA = best.WorldPointGet();
					C2dCoord tmpA = best.WorldPointGet();
					C2dCoord tmpB = toolhit_extent.BL();
					llA.XY( (tmpA.X() + tmpB.X()), (tmpA.Y() + tmpB.Y()) );

					// The seed position for the first grid location.
					//   placeB.XY( placeA.X(), (placeA.Y() + toolhit->Delta().Y()) );
				}
				else if (trial == 1)
				{
					// TODO: Fix to account for nesting progression directions !!!!
					//   delta.Y( best.WorldPointGet().Y() - placeA.Y() );
					C2dCoord tmpA = currbest.WorldPointGet();
					C2dCoord tmpB = best.WorldPointGet();
					CLink link( toolhit->Index(), PRENEST_FACTOR, (tmpB - tmpA) );

					link.PreLink( true );
					link.BumpScore( best.BumpScoreGet() );
					link.OverlapScore( best.OverlapScoreGet() );

					ref_toolhit->LinkCopyAppend( link );
				}
				else if (trial == 2)
				{
					// TODO: Fix to account for nesting progression directions !!!!
					//   delta.X( best.WorldPointGet().X() - placeA.X() );
					//   toolhit->Delta( delta ); <===== WHAT TO DO ABOUT THIS ?
					C2dCoord tmpA = currbest.WorldPointGet();
					C2dCoord tmpB = best.WorldPointGet();
					CLink link( toolhit->Index(), PRENEST_FACTOR, (tmpB - tmpA) );

					link.PreLink( true );
					link.BumpScore( best.BumpScoreGet() );
					link.OverlapScore( best.OverlapScoreGet() );

					ref_toolhit->LinkCopyAppend( link );
				}
			}
			else
			{
				// We failed ...
				if ( config.Oversize() || config.OffSheet() )
					m_lasthit = NULL;

				if ( !config.NewLine() && !config.ForceSeed() )
				{
					if (!sheet->SeedNext( config, true ))
						break;
				}
			}
		}

		seed_mgr.Pop();
		sheet->PartPlacePop();
	}

	// Sort the links in order of decreasing bump-score because
	// we want the seeds calculated by SeedsGenerate() to be
	// in prefered order (significant speed improvement).
	ref_toolhit->LinksSort();

	EWMFile( FILE_INFO );

	if ( status.IsOk() )
	{
		config.PreNest(false);
		config.GridFlags(config.GridMode());
		m_lasthit = NULL;
	}

	return status;
}

// Like nest_area, but restricted to the pre-nesting case
CReturn CNestMgr::PreNest(
	CNestConfig&	config,
	CPartBin*		partbin,
	CNestingPart*	pre_part,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn		status;
	CSeedMgr	seed_mgr;
	CPartPlace	best;
	double		ypos;
	int			count, indx;

	CGeoPolyArray* poly_array;

	EWMFile( FILE_INFO );

	// Inform the sheet of our working area
	C2dBox edge = sheet->Extent();

	sheet->FillBarrier( config, edge );

	// Managerial stuff.
	sheet->PartPlacePush();
	seed_mgr.Push();
	sheet->SeedMgr( &seed_mgr );

	sheet->clearRecentPart();

	// CRITICAL: Without this next statement,attempts to nest later
	// orientations of a part will fail because the seed manager will
	// attempt to use a link to a (non-existing) placed part.
	sheet->DidPlace( false );

	set_fill_barrier( config, partbin, sheet, config.OneSheet() );

	{
		CCommand window;
		CString cmdstr;
		cmdstr.Format( "view:window: regen=0, sx=0, sy=0, ex=%f, ey=%f", edge.Dx(), edge.Dy() );
		window.setCommand( cmdstr, sheet->pModel(), &view );
		view.Window(&window);
		view.Clear();
	}

	// Prepare the seeding for this area
	config.NewLine(true);
	config.ForceSeed(true);
	config.PreNest(true);

	// As pilfered from CNestMgr::Execute()
	// Dunno if this is really necessary.
	pre_part->Used(0);
	pre_part->Tooled( FALSE );

	EWMFile( FILE_INFO );

	poly_array = sheet->ToolHitPoly( HIT_NEST_OUTSIDE );
	count = poly_array->Count();
	for (indx = 0; indx < count; ++indx)
	{
		if (indx == RESERVE_BARRIER)
		{
			sheet->SeedBarrier( config, poly_array->GetAt(indx), +1.0 );
		}
		else if (indx >= RESERVE_NUM)
		{
			// 2009.12.19 (PE) -- I'm fairly sure this is the correct
			// thing to do since we are seeding the outside of a part.
			sheet->SeedPolyConfig( true );

			CGeoPoly* poly = poly_array->GetAt(indx);
			if (edge.Intersects( poly->Extent(), SMALL ))
			{
				sheet->SeedPoly( config, poly_array->GetAt(indx), +1.0, true );
			}
		}
	}

	m_lasthit = NULL;

	EWMFile( FILE_INFO );

	CNestedArea* area_to_nest = sheet->SheetNestedArea();

	sheet->NestingPhaseSet( PHASE_PRENEST );

	ypos = (config.NestPosY() ? 0. : edge.Dy());
	m_lastpos = C3dCoord( 0., ypos, 0. );

	// Because pre-nesting requires a pair of 'like' parts.
	for (int trial = 0; trial < 2; ++trial)
	{
		status += CProgressWnd::ProgressWndUpdate();
		if ( !status.IsOk() )
			break;

		view.ModelSet( (*pre_part->pModel()) ); 

		best = score_parts( config, partbin, area_to_nest, pre_part, sheet, false, view );
		if (best.BumpScoreGet() > SMALL)
		{
			CNestingPart* npart = best.PartGet();
			CToolHit* toolhit = best.ToolHitGet();

			if (m_lasthit)
			{
				// Make the pre-nest link!
				C2dVec delta = toolhit->Pos() - m_lastpos;
				CLink link( toolhit->Index(), PRENEST_FACTOR, delta );
				link.PreLink( true );

				m_lasthit->LinksDestroy();
				m_lasthit->LinkCopyAppend(link);
			}

			m_lasthit = toolhit;
			m_lastpos = best.WorldPointGet();

			// 2007.11.30 (PE) -- In the early days we used to 'cut' only the
			// first instance. I know why this restriction existed, perhaps
			// because there is a lot of overhead associate with cut_part()
			// (overhead that does not buy you anything)? Regardless, that
			// restriction has now been suppressed because there is value in
			// being able to see the nested pair when debugging.
			//   if (!trial)
			{
				cut_part( config, sheet, 0, false, false, best, area_to_nest, view );

				config.FillSeed(true);
				config.GridSquare(false);
				pre_part->PreHit(-1);
			}
		}
		else
		{
			// We failed ...
			if ( config.Oversize() || config.OffSheet() )
				m_lasthit = NULL;

			if ( !config.NewLine() && !config.ForceSeed() )
			{
				if (!sheet->SeedNext( config, true ))
					break;
			}
		}
	}

	EWMFile( FILE_INFO );

	if ( status.IsOk() )
	{
		config.PreNest(false);
		config.GridFlags(config.GridMode());
		m_lasthit = NULL;
	}

	seed_mgr.Pop();
	sheet->PartPlacePop();

	return status;
}

// Apply repositions to the sheet; includes hold downs and stuff.
CReturn CNestMgr::nest_sheet_repo(
	CNestConfig&	config,
	CRepo&			repo,
	CRepoZone&		repo_zone,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn ret;

	// Repo init (for sheet repo tools, different than in-nest RepoZone)

// if not holddown and/or clamps (?) can bypass all of this
// and jump directly to the call 'repo.EnableCommands();'

	// As determined by CRepoZone::Mark()
	int repo_num = repo_zone.DidRepo();

	int hot_repo = repo_zone.HotRepo();

	if ( !g_v17_large_part_pass )
	{
		if ( config.HaveLargeParts() )
		{
			CDbIterator	iter;

			double zone_xmax = repo_zone.Zone( repo_num ).Xmax();

			iter.Init( sheet->Model().Db(), DBCOMMAND );
			while (1)
			{
				CDbCommand* dbCommand = dynamic_cast<CDbCommand*>( iter() );
				if (dbCommand == NULL)
					break;

				if ( dbCommand->IsInstance() )
				{
					// As determined by CSheet::PunchInstance().
					double instance_xmax = dbCommand->DoubleGet( "_xmax", 0. );
					if (instance_xmax > zone_xmax)
					{
						hot_repo = repo_zone.Count() + 1;
						break;
					}
				}

				iter.Next();
			}
		}
	}

	if ( (hot_repo > 0)	|| config.ClampUnder() )
	{
		if ( EWMNestingAllow() )
		{
			CString note;
			note.Format( "Repo (hot repo %d, repo num %d)", hot_repo, repo_num );
			EWMNesting( note );
		}

		// Hold-downs and marks
		if ( CNestConfig::IsBitmapNest() ||
			 (!config.HasHoldDowns() && !config.HasClamps()) )
		{
			CProgressWnd::SetText( "Processing zones ...." );
		}
		else
		{
			CProgressWnd::SetText( "Processing zones and hold-downs ...." );

			// Put holds into all active repos, EXCEPT the last one?
			int repo_hold;
			for (repo_hold=0; repo_hold<hot_repo; repo_hold++)
			{
				// Don't drop holds onto the clamps...6
				if ( config.HasClamps() )
				{
					ret = repo_zone.ClampGrid( config, *sheet, repo_hold, TRUE );
					if ( !ret.IsOk() )
						return ret;
				}

				if ( config.HasHoldDowns() )
				{
					ret = repo_zone.HoldGrid( view, config, *sheet, repo_hold, TRUE );
					if ( !ret.IsOk() )
						return ret;
				}

				if ( config.HasClamps() )
				{
					ret = repo_zone.ClampGrid( config, *sheet, repo_hold, FALSE );
					if ( !ret.IsOk() )
						return ret;
				}
			}

			// repo-mark all zones...
			if (config.HasHoldDowns() && ((hot_repo > 1)	|| config.ClampUnder()))
			{
				// If we are under clamps, need to preserve the hold location
				for (repo_hold=0; repo_hold<repo_num; repo_hold++)
				{
					CDbCommand* hold = NULL;
					repo_zone.HoldMark( config, *sheet, repo_hold, repo_hold, &hold );
				}
			}

			// And large parts can escape the bounds of hot repo
			if (config.HasHoldDowns() && config.DidLarge() && (hot_repo != repo_num))
			{
				EWMNesting( "Large parts beyond hot repo" );

				//
				// I *thought* that repo_num would be >= hot_repo,
				// but there is a case where it is NOT.  So, instead of 
				// switching, I'll just take the limit.
				// This may be related to zero-base vs. one-base?
				// TODO:  Figure this out.
				int num = max(repo_num, hot_repo);
				for (; repo_hold<num; repo_hold++)
				{
					CDbCommand* hold = NULL;
					repo_zone.HoldMark( config, *sheet, repo_hold, repo_hold-1, &hold );
				}
			}
		}

		if ( config.HasClamps() )
		{
			// Fill out any unused repo zone marks...
			for (int repo_cnt=repo_num; repo_cnt<hot_repo; repo_cnt++)
			{
				repo_zone.Mark( sheet, repo_cnt );
				repo_zone.ClampMark( config, sheet, repo_cnt );
			}
		}

		// Now, process it all
		CProgressWnd::SetText( "Packaging entities ..." );

		ret += repo.ExtractZone();
		if (config.HasClamps() && config.ClampUnder())
			ret += repo.ExtractClamp();

		ret += repo.PackageInstances();

		if (g_v17_large_part_pass)
		{
			if ( (repo.ZoneCount() > 0)	&& config.Active( 0 ) )
				ret += repo.LargeInstances();
		}
		else
		{
			if ( (repo.ZoneCount() > 0)	&& config.DidLarge() )
				ret += repo.LargeInstances();
		}

		if (config.HasClamps() && config.ClampUnder() && repo.HasClamps())
		{
			CDbFeature* czone = NULL;
			CDbCommand* hold = NULL;

			EWMNesting( "Process under clamps" );

			for (int repo_cnt=0; repo_cnt<hot_repo; repo_cnt++)
			{
				ret += repo.ClampInstances(
					config.MachineReal( "Clamp_Buffer", 0.0 ), 
					config.MachineReal( "Min_Travel_Limit_X", 0.0 ), 
					config.MachineReal( "Torch Offset in X", 0.0 ),
					sheet->Length(),
					config.RepoSmall(),
					repo_cnt, 
					&czone );
			}
		}

		if ( repo.MustRebuild() )
		{
			// We must have split parts across zone boundaries, so ....
			ret += repo.Rebuild( config.MachineReal( "Clamp_Buffer", 0.0 ) );
		}

		CProgressWnd::SetText( "Cleaning up ..." );

		ret += repo.Cleanup();
	}

	repo.EnableCommands();

	return ret;
}

// @DROPSTOP from compressed to uncompressed form
// Delete all others.
char* g_command_kill[] = 
{
	"@ZONE",
	"@HOLD",
	"@CLAMP",
	"@REPO",
	NULL
};

CReturn CNestMgr::nest_sheet_attrib(
	CNestConfig&	config,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn ret;
	CDbIterator iter;

	// Interpret any @DROPSTOP attributes into the correct, non-compact form.
	iter.Init( sheet->pModel()->Db(), DBLINE);
	while (TRUE)
	{
		CDbEntity*	db_ent = iter();
		if (db_ent == NULL)
			break;
		iter.Next();

		if (db_ent->Type() > DBFEATURE)
			break;

		CVarList* attrib = db_ent->pAttrib();
		//
		// -------------------------------------------------\
		//
		CVar* var = attrib->getVar( "@DROPSTOP" );
		if (var)
		{
			CCommand cmd;
			C3dCoord geo_pt;

			CVarList* dst_att = attrib;

			// SHOULD be a profile always, but I don't know if that is guaranteed
			if (db_ent->Type() == DBPROFILE)	
			{
				CDbCurve* db_curve = NULL;

				// ASSUMPTION: Leads already applied to profile.  Structure is:
				//
				// Feature
				//   Lead-in
				//   Profile
				///  Lead-out
				// 
				CDbProfile* db_prof = (CDbProfile*) db_ent;
				CDbContainer* dbContain = dynamic_cast<CDbContainer*>( db_prof->Owner() );
				if (dbContain != NULL)
				{
					int last = dbContain->Position(db_ent)+1;
					if (last < dbContain->Count())
					{
						CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( dbContain->GetAt(last) );
						if ((dbFeature != NULL) && dbFeature->IsLeadOut())
						{
							last = dbFeature->Count() - 1;
							db_curve = dynamic_cast<CDbCurve*>( dbFeature->GetAt(last) );
						}
					}
				}

				if (db_curve == NULL)
				{
					int last = db_prof->Count()-1;
					db_curve = (CDbCurve*) db_prof->GetAt(last);
				}

				geo_pt = db_curve->EndPt(0);
				dst_att = db_curve->pAttrib();
			}

			CString cmdstr;
			cmd.setCommand( var->getString().Mid(1), NULL, NULL );
			cmd.nextRoute( &cmdstr );

			// 2005.02.08 (PE) -- Sunflower Mfg wants the ability
			// to program explicit slide moves.  Prior to this
			// change they were unable to do so because the
			// code generator is unable to distinguish between
			// an explicit slide move and one that is generated
			// by nesting.  Since, to date, no code generators
			// have used the slide move values, we can pretend
			// that nesting never generated them.  And now, we
			// use the attributes _slide_dx and _slide_dy to
			// differentiate between old/new data.
			if (cmdstr.CompareNoCase("STOP") == 0)
			{
				dst_att->setInt( "_dropstop", STOP_CONST );
			}
			else if (cmdstr.CompareNoCase("DROP") == 0)
			{
				dst_att->setInt( "_dropstop", DROP_CONST );
			}

			attrib->deleteVar( "@DROPSTOP" );
		}

		int idx = 0;
		while (true)
		{
			char* varname = g_command_kill[idx];
			if (!varname)
			{ break; }
			idx++;

			var = attrib->getVar( varname );
			if (var)
			{ attrib->deleteVar( varname ); }

			if (db_ent->Type() == DBCOMMAND)
			{
				CString cmdstr = ((CDbCommand*)db_ent)->CommandString();
				if (cmdstr.CompareNoCase( varname+1 ) == 0)
				{
					db_ent->Delete();
				}
			}
		}
	}

	return ret;
}

// Apply repositions to the sheet; includes hold downs and stuff.
CReturn CNestMgr::nest_sheet_lead(
	CNestConfig&	config,
	CSheet*			sheet,
	CViewMgr&		view )
{
	CReturn ret;
	CString note;
	//
	// Finalize the leads on the sheet; both closed profiles
	//	AND split/open profiles
	//
	if (config.LeadSetup() > 0)
	{
		CSelector selector(*(sheet->pModel()));

		note = "Applying auto-leads";
		CProgressWnd::SetText( note );

		selector.All( 0 );
		selector.Filter( DBPROFILE, 1 );
		//selector.Filter( DBFEATURE, 1 );
		selector.Restrictions( TRUE );
		selector.SelectAll( TRUE );

		CLead	lead(*(sheet->pModel()));

		lead.AutoOpen( config.CmdbPath(), config.LeadSetup(), true, &selector );
		lead.Auto( config.CmdbPath(), config.LeadSetup(), selector, FALSE,
			!config.GridChain(), config.LeadRestrictions() );

		selector.Clear();
	}

	return ret;
}


// ==================================================================
//	Scan the part bin, and see who makes the best fit.
//	The return score has certain magic values:
//		
//
//const double MARKOV_FACTOR = 1.2;
//const double GRID_FACTOR = 1.2;
//const double GRID90_FACTOR = 1.0;
//const double GRID180_FACTOR = 1.5;

const double SCORE_SKIP_FACTOR = 0.95;

CPartPlace CNestMgr::score_parts( 
	CNestConfig&	config,
	CPartBin*		partbin,
	CNestedArea*	area_to_nest,
	CNestingPart*	force_part,
	CSheet*			sheet,
	bool			hole,
	CViewMgr&		view )
{
	CReturn status;
	CPartPlace best;
	CPartPlace trial;

	// Initialize winner to "no part"
	double high_score = -3.0;

	CToolHit*	high_hit = NULL;
	C2dCoord	high_place;
	C2dCoord	part_place;
	C2dCoord	mod_place;
	CSeed*		part_place_seed;

	config.DidScore(false);

	CSeedArray part_place_array;
	CToolHitArray toolhit_array;

	// Now assemble the various placement theories
	config.TryTask( DID_SEED );

	SeedsGenerate( config, *sheet, partbin, &toolhit_array, &part_place_array );

	// Test the theories
	for (int idx = 0; idx < part_place_array.Count(); ++idx)
	{
		status = CProgressWnd::ProgressWndUpdate();
		if ( !status.IsOk() )
		{
			best.ScoresInit( -UNDEFINED, -UNDEFINED );
			break;
		}

		CToolHit* toolhit = toolhit_array[idx];
		CNestingPart* npart = toolhit->Part();

		if (force_part)
		{
			if (npart != force_part)
				continue;

			if ((npart->PreHit() >= 0) && (npart->PreHit() != toolhit->Index()))
				continue;
		}

		if ( !config.DidNest() && npart->Filler() && !config.OneSheet() )
		{
			// IT #38
			// We won't treat filler parts until a non-filler has been nested.
			continue;
		}
		config.DidTask( DID_SEED );

		bool ignore_clamp = (config.ClampUnder() && !npart->Small());

		Sleep(0); // PumpMessages?

		config.DidScore(true);


		part_place_seed = part_place_array[idx];
		part_place = part_place_seed->Placement();

		mod_place = part_place;

		if ( EWMSeedAllow() )
		{
			g_msg.Format( "seed( %f, %f )", part_place.X(), part_place.Y() );
			EWMSeed( (LPCSTR) g_msg );
		}

		//**********************************************************************
		// At this point, mod_place represents the location at which to place
		// the lower-left corner of the part.
		trial = sheet->Score( config, mod_place, toolhit, area_to_nest, ignore_clamp, hole );
		//**********************************************************************

		if (config.ShiftedOntoSheet() && config.PlacementFailed())
			break;  // short circuit further attempts at using seeds in part_place_array.

		if (trial.BumpScoreGet() < -1)
		{
			toolhit->Height(-1.0);
			if ( config.Oversize() )
				npart->Oversized( true );
		}

		if (trial.BumpScoreGet() < -UNDEFINED)
		{
			// User cancel. Forcing termination of client.
			best = trial;
			break;
		}
		else if (trial.BumpScoreGet() > SMALL)
		{
			bool accept = false;

			if ( false ) // config.HardPierce() )
			{
				// ie. The combined parts require less area.
				// TODO: We might want to consider this as the defacto
				// acceptance method, eliminating use of OverlapScore().
				accept = (trial.PackingScoreGet() < best.PackingScoreGet());
			}
			else
			{
				if ( config.PreNest() )
				{
					if (area_to_nest->InteriorAreasCount() == 1)
					{
						CNestedArea* na = area_to_nest->InteriorAreaGet(0);
						PackingScore( config, (*na), &trial, view );
					}
				}
				else
				{
#if BEFORE_V20
					if ( trial.HasSameScore( best ) )
						sheet->OverlapScore( config, &best, &trial );
#else
					sheet->ScoresScrutinize( config, &best, &trial );
#endif
				}

				accept = trial.HasBetterScore( best );
			}

			if ( accept )
			{
				best = trial;

				// Update the lows...
				high_score = best.BumpScoreGet();
				high_hit = toolhit;  // isn't this best.ToolHitGet()?
				// high_place = mod_place;  // and what should this be?
				high_place = best.WorldPointGet();

				if (part_place_seed->PreLink())
					break;
			}
		}
	}

	// Cleanup
	part_place_array.DestructiveFlush();
	toolhit_array.BenignFlush();

	// Return, if success...
	if (high_hit)
	{
		high_hit->Pos( high_place );
	}
	else
	{
		config.ForceSeed(true);

		best.ScoresInit( 0., 0. );
	}

	return best;
}

void CNestMgr::next_gridsquare( 
	CToolHitArray*		next_hit,
	CSeedArray*			next_pos, 
	CNestConfig&		config, 
	CPartBin*			partbin,
	CToolHit*			last_hit, 
	const C3dCoord&		last_pos )
{
	if (!last_hit)
		return;

	CNestingPart* npart = last_hit->Part();
	if (npart->Quantity() == 0)
		return;

	int part_idx = last_hit->Part()->Index();
	int part_hit = last_hit->Index();

	double sign_y = config.NestPosY()?+1.0:-1.0;
	double sign_x = 1.0;

	if ((config.Progression() == PROGRESS_PPX_SPY) ||
		(config.Progression() == PROGRESS_PPX_SNY))
	{ sign_y = 0.0; }
	else
	{ sign_x = 0.0; }

	// Plain Grid
	CGeoPolyArray* outside_poly = last_hit->PolysGet(HIT_NEST_OUTSIDE);
	C2dBox part_mer;
	for (int idx=0; idx<outside_poly->Count(); idx++)
	{ part_mer += outside_poly->GetAt(idx)->Extent(); }

	double height = last_hit->Height();
	if (height < 0)
	{
		height = part_mer.Dy();
		last_hit->Height(height);
	}

	C2dVec delta = C2dVec( sign_x*part_mer.Dx(), sign_y*height );
	C3dCoord part_place( last_pos.X() + delta.X(),
						 last_pos.Y() + delta.Y(),
						 -1.0 );
	CSeed* part_seed = new CSeed( GRID_PRIORITY, part_place, last_hit, 0);

	next_pos->Append( part_seed );
	next_hit->Append( npart->ToolHit(part_hit) );

	// Mirror by rotation
	int rotnum = npart->Rotation();
	if ((rotnum > 0) && !(rotnum & 0x01))
	{
		if (npart->Mirror())
			rotnum *= 2;

		part_hit = (part_hit + rotnum/2) % rotnum;
		CToolHit* toolhit = npart->ToolHit(part_hit);

		C3dCoord last_origin = last_hit->Grid()->Origin();
		C3dCoord this_origin = toolhit->Grid()->Origin();

		delta = C2dVec( (sign_x*part_mer.Dx()) + last_origin.X()-this_origin.X(),
						(sign_y*part_mer.Dy()) + last_origin.Y()-this_origin.Y() );

		part_place = C3dCoord(  last_pos.X() + delta.X(),
								last_pos.Y() + delta.Y(),
								-1.0 );
		part_seed = new CSeed( GRID_PRIORITY, part_place, toolhit, 0);

		next_pos->Append( part_seed );
		next_hit->Append( toolhit );
	}

	// Pure Mirror
	if ( npart->Mirror() )
	{
		part_hit = part_hit ^ 0x01;
		CToolHit* toolhit = npart->ToolHit(part_hit);

		C3dCoord last_origin = last_hit->Grid()->Origin();
		C3dCoord this_origin = toolhit->Grid()->Origin();

		delta = C2dVec( (sign_x*part_mer.Dx()) + last_origin.X()-this_origin.X(),
						(sign_y*part_mer.Dy()) + last_origin.Y()-this_origin.Y() );

		part_place = C3dCoord(  last_pos.X() + delta.X(),
								last_pos.Y() + delta.Y(),
								-1.0 );
		part_seed = new CSeed( GRID_PRIORITY, part_place, toolhit, 0 );

		next_pos->Append( part_seed );
		next_hit->Append( toolhit );

		//
		// Yeah, fuck mirror for lead bump.  It's rare.
		//
	}
}

void CNestMgr::next_link( 
	CToolHitArray*		next_hit,
	CSeedArray*			next_pos, 
	CNestConfig&		config, 
	CPartBin*			partbin,
	const CSheet&		sheet, 
	const C3dCoord&		last_pos )
{
	// CRITICAL: We *cannot* arrive here unless CSheet::DidPlace() returns true.

	CToolHit* last_hit = sheet.PartPlaceGet()->ToolHitGet();
	assert( (last_hit != NULL) );

	CNestingPart* npart = last_hit->Part();
	assert( (npart != NULL) );

	int count = last_hit->LinksCount();
	if (count > 0)
	{
		C2dCoord candidate;
		double xp, yp;

		// Because prenested parts come in pairs ....
		for (int trial = 0; trial < 2; trial++)
		{
			for (int indx = 0; indx < count; indx++)
			{
				CLink* link = last_hit->LinkGet(indx);

				// First pass, do the PreLink hits!
				if (link->PreLink())
				{
					if (trial || config.NewLine())
						continue;
				} 
				else // !PreLink
				{
					if (!trial)
						continue;
				}

				// TODO: Is last_pos ever undefined?
				if (!last_pos.IsDefinedXY())
				{
					xp = link->Delta().X();
					yp = link->Delta().Y();
				}
				else
				{
					xp = last_pos.X() + link->Delta().X();
					yp = last_pos.Y() + link->Delta().Y();
				}

				candidate.XY( xp, yp );

				CToolHit* tool_hit = npart->ToolHit( link->Hit() );

				if ( IsUniquePlace( (*next_pos), (*next_hit), candidate, tool_hit ) )
				{
					CSeed* part_seed = new CSeed(
						(int)(link->Priority()*GRID_PRIORITY), candidate, NULL, 0 );

					part_seed->PreLink(link->PreLink());

					next_pos->Append( part_seed );
					next_hit->Append( tool_hit );
				}
			}
		}
	}
}

void CNestMgr::next_seed( 
	CToolHitArray*		next_hit,
	CSeedArray*			next_pos, 
	CNestConfig&		config, 
	CPartBin*			partbin,
	const CSheet&		sheet )
{
	C2dCoord	part_place;
	CToolHit*	toolhit;
	CSeed*		part_seed;
	C2dVec		delta;

	int num = partbin->Count();
	for (int part_idx=0; part_idx<num; part_idx++)
	{
		CNestingPart* npart = partbin->GetAt(part_idx);

		if (npart->Error())
			continue;

		if ( (npart->Quantity() != 0)				// Only test if more to cut
			&& (npart->Pass() == config.Pass())			// Only test relevant parts...
			&& (npart->ToolMatch() || !npart->Tooled()) )
		{
			int num = npart->Count();
			for (int part_hit=0; part_hit<num; part_hit++)
			{
				toolhit = npart->ToolHit(part_hit);

				if ( config.NestPosY() )
					delta.Init( -toolhit->Mer().Xmin(), -toolhit->Mer().Ymin() );
				else
					delta.Init( -toolhit->Mer().Xmin(), -toolhit->Mer().Ymax() );

				part_place = sheet.Seed().Placement() + delta;

				part_seed = new CSeed( SEED_PRIORITY, part_place, NULL, 0 );

				next_pos->Append( part_seed );
				next_hit->Append( toolhit );
			}
		}
	}
}

// Make sure this part fits the sheet's tooling setup.
bool CNestMgr::tool_part( 
	CNestConfig&		config,
	CNestingPart*		part,
	CSheet*				sheet )
{
	if (!part->Tooled())
	{
		// Require that all tool stations in the part model exist in the sheet model
		// CLONED some of this from ToolFindCreate() -- but we have special needs; the entire
		//	part must be tested against empty stations WITHOUT filling the stations, but WITH
		//	cascading over "filled" stations... hence, the use of Tags.  
		// TODO:  Incorporate TAG stuff into ToolFindCreate() ??
		//
		// Un-tag all destination tools
		//
		CDbEntity::NewAction();

		bool	fits = TRUE;
		//
		// For every tool in the part...
		//
		CDbIterator np_iter;
		np_iter.Init( part->Model().Db(), DBTOOL );
		while (1)
		{
			CDbTool* test_tool = dynamic_cast<CDbTool*>( np_iter() );
			if (test_tool == NULL)
				break;
			np_iter.Next();

			if (test_tool->DidAction())
				continue;

			// First try to find a matching tool.
			CDbTool* match_tool = sheet->Model().Db().ToolFindCreate( *test_tool, FALSE );
			if (!match_tool)
			{
				if (!test_tool->IsLayer())
				{
					CReturn status;
					CString descrip = test_tool->StringGet( STR_DESCRIPTION, "" );

					fits = FALSE;
					config.DidFailTool( descrip );

					status.Internal( IDS_NEST_DIDIT_TOOL, descrip );
					part->Error( status.LastErrorMsg() );
				}
			}
			else
				match_tool->DoAction();
		}

		part->Tooled( TRUE );
		part->ToolMatch( fits );
	}

	return part->ToolMatch();
}

// Cut this part out of the material, and also place it's geometry
// into the sheet's model.
CNestedArea* CNestMgr::cut_part(
	CNestConfig&		config,
	CSheet*				sheet, 
	int					zone,			// zone number
	bool				torch_shift,	// Shift punches to next zone?
	int					hole,
	const CPartPlace&	part_place,
	CNestedArea*		area_to_nest,
	CViewMgr&			view )
{
	CNestedArea*	nested_area;
	CString			note;

	// Note there are 3 aspects of the part to nest:
	//	Model (transformed)
	//	CToolHit GeoPoly outlines (transformed)
	//	CToolHit DexGrid (scanned in at its final resting place)
	//
	CNestingPart* npart = part_place.PartGet();
	CToolHit* toolhit = part_place.ToolHitGet();

	// Name this part... 
	CString	 part_name = npart->Filepath();
	CString part_pathname;
	part_name.Format( "%s%d", CFileSnoop::getFilename( part_pathname ),
		npart->Index() );

	if (!config.PreNest())
	{
		if (npart->Quantity() > 0)
		{
			npart->Quantity( npart->Quantity()-1 );
			config.PartsToCut( config.PartsToCut() - 1 );
		}

		npart->Used( npart->Used()+1 );
	}

	// Put the part into the sheet... geometry and dex
	sheet->PunchInstance( config, part_place, zone, torch_shift, view );
	nested_area = sheet->PunchGeo( config, part_place, area_to_nest, view );

	if ( false ) // config.HardPierce() )
		area_to_nest->Pack( true, part_place, config.Progression(), view );

	// CRITICAL: Now that we have placed a part, we must indicate as much.
	// This is critical to seed generation, especially wrt pre-nested parts.
	sheet->DidPlace( true );

	// Transfer the bitmap of the part to the sheet.
	sheet->Scan( config, *(toolhit->Grid()), part_place.GridPointGet(), view );

	if ( !config.PreNest() )
	{
		if (config.FirstPartInspection() == 1)
		{
			// Now that we've placed an instance of the part:
			npart->FirstPartInspection( 2 );
		}
		else
		{
			// We conditionally set DidPlace because otherwise, when
			// doing first part inspection, a second instance of the
			// current part might be nested.
			sheet->DidPlace( true );
		}
	}

	note.Format( "Pass: %d, Pending: %d", config.Pass(), config.PartsToCut() );
	CProgressWnd::SetText( note );

	return nested_area;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Update the part & material inventories, and also determine
// whether this nested sheet of parts can be duplicated.
int CNestMgr::PartsAccounting( 
	CNestConfig*	config,
	CPartBin*		partbin,
	CSheet*			sheet )
{
	int min_repeat = 0;
	int cut_num = 0;

	// Find our repeat count...
	if ( !config->SingleSheet() )
	{
		CNestingPart* npart = NULL;
		int quant, used, repeat;

		int indx, count = partbin->Count();

		min_repeat = INT_MAX;
		for (indx = 0; indx < count; ++indx)
		{
			npart = partbin->GetAt(indx);

			quant = npart->Quantity();
			used = npart->Used();

			if (quant < 0)
			{
				// Must be a filler part.
			}
			else if (quant == 0)
			{
				// The count of this part is exhausted.
				if (used > 0)
				{
					// config->PartsToCut( config->PartsToCut() - used );
					min_repeat = 0;
				}
			}
			else if (quant > 0)
			{
				if (!npart->Filler())
					cut_num += used;

				if (used > 0)
				{
					// config->PartsToCut( config->PartsToCut() - used );

					repeat = quant / used;
					min_repeat = min( repeat, min_repeat );
				}
			}
		}

		// Did it fail for some reason?  Default...
		if (min_repeat == INT_MAX)
			min_repeat = 0;

		if (sheet->Quantity() >= 0)
		{
			if (min_repeat > sheet->Quantity())
				min_repeat = sheet->Quantity();
		}

		if (min_repeat > 0)
		{
			// Decrement the repeated part quantities.
			for (indx = 0; indx < count; ++indx)
			{
				npart = partbin->GetAt(indx);

				quant = npart->Quantity();
				if (quant > 0)
				{
					used = npart->Used() * min_repeat;
					npart->Quantity( quant - used );
					config->PartsToCut( config->PartsToCut() - used );
				}
			}
		}
	}

	// Record the "repeat count" of the nested sheet.
	sheet->Repeat( min_repeat );

	// Decrement the material inventory as necessary.
	if (sheet->Quantity() > 0)
		sheet->Quantity( sheet->Quantity() - min_repeat );

	return (cut_num * min_repeat);
}

// Generate a "cut" line (if possible) at the far end of the sheet
// Only really useful if nesting in Y
#define CUTBACK_COLOR 0x0000ff
CDbLine* CNestMgr::cutback_sheet(
	CNestConfig&		config, 
	CSheet*				sheet,
	CViewMgr&			view )
{
	CDbLine* cutback_line = NULL;

	// 'cut_length' is the amount of untouched sheet to the right of the cutback
	double cut_length = sheet->Length() - sheet->Cutback();
	if (cut_length >= config.RemnantLength())
	{
		C3dCoord		ps( sheet->Cutback(), sheet->Width(), sheet->Thick() );
		C3dCoord		pe( sheet->Cutback(), 0, sheet->Thick() );
		CDbWorkplane*	workplane;

		CDbTool*		dbTool = NULL;
		int				color;

		CModel* model = sheet->pModel();

		// TODO: Manage actual cutback position wrt/Clamps

		// Set up the layer and workplane...

		int station_id = config.CutBackStationID();
		if (station_id > 0)
		{
			dbTool = CModelUtil::DbToolFind( model->Db(), STR_STATION_ID, station_id );
			if (dbTool != NULL)
				color = dbTool->ColorGet( CUTBACK_COLOR );
		}

		if (dbTool == NULL)
		{
			model->EntityCreate( DBTOOL, (CDbEntity**)&dbTool );
			dbTool->Name( "Cutback" );

			color = CUTBACK_COLOR;
			dbTool->ColorSet( color );
		}

		model->EntityFind( STR_TOP, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
		if ( !workplane )
			workplane = model->ActiveWorkplane();

		workplane->Inverse().Transform( &ps );
		workplane->Inverse().Transform( &pe );

		model->EntityCreate( DBLINE, (CDbEntity**) &cutback_line );
		cutback_line->Name("CutBack");
		cutback_line->Init( dbTool, workplane, ps, pe );
		cutback_line->ColorSet( color );
		cutback_line->IntSet( "cutback", 1 );

		if ( !dbTool->IsLayer() )
			sheet->CutbackWorkzoneCreate( cutback_line );
	}

	return cutback_line;
}

// Create a true-shape remnant out of whats left of this sheet
//
// TODO:  Break all this remnant stuff out into a seperate object
//

// Prior to V17, the nesting engine both created the remnant file
// and update the pdb accordingly.  This created material management
// problems for our users.  With the V17, we simply generate the
// remnant geometry in the .nst file, and the user explicitly
// manages remnants as in a separate process.
// See also CNestProcessApp::RemnantCommit().
void CNestMgr::remnant_sheet(
	CNestConfig&		config, 
	CSheet*				sheet,
	C2dBox&				content,
	CViewMgr&			view )
{
	CTrueRemnant remnant( config, view );
	CModel*	sheet_model = sheet->pModel();

	if ( config.Cutback() )
	{
		remnant.Square( sheet->Extent(), sheet->Cutback() );
	}
	else  
	{
		remnant.TrueShape( (*sheet) );
	}

	// 3. Extract into the cloned model (with delta offset to origin)
	remnant.Extract( sheet_model, C3dCoord(0,0,0) );
}

//	TODO:  X or Y based on Fill Direction
//	TODO:  Min or Max based on Fill Bottom
void CNestMgr::set_fill_barrier(
	CNestConfig&		config, 
	CPartBin*			partbin,
	CSheet*				sheet,
	bool				unlimited )
{
	const C2dBox& zone = sheet->FitZone();

	C2dBox grid_edge;
	C2dBox edge;
	//
	// See if we want to limit our fill to the edge of the parts in place or 
	// fill out the entire work zone.
	//
	bool limit = TRUE;
	if (unlimited || !partbin )
	{
		limit = FALSE;
	}
	else
	{
		for (int part_idx=0; part_idx<partbin->Count(); part_idx++)
		{
			CNestingPart* npart = partbin->GetAt(part_idx);

			if ((npart->Quantity() > 0) && !npart->Filler())
			{
				limit = FALSE;
				break;
			}
		}
	}

	// Set the relevant fill zone
	limit = false;
	if (limit && (config.Remnant() || config.Cutback()))
	{
		// TODO: Remove this block dead code (because limit is false) (?)
		CModel* model = sheet->pModel();

		C2dBox extent = sheet->BoxUser();
		double narrow = config.Narrow( config.PassNum() ) * 2;
		if (config.Remnant())
			narrow = max( narrow, max( config.RemnantLength(), config.RemnantWidth() ) );
		//
		// Limit fill zone to repo area
		//
		if (extent.Xmax() > zone.Xmax())
			extent.Xmax( zone.Xmax() );
		if (extent.Xmin() < zone.Xmin())
			extent.Xmin( zone.Xmin() );
		//
		// This should never happen, but it does... massively wierd in Y
		// TODO:  Figure the sucker out
		//
		if (extent.Ymin() > zone.Ymax())
		{
			extent.Ymin( zone.Ymin() );
			extent.Ymax( zone.Ymax() );
		}
		else
		{
			if (extent.Ymax() > zone.Ymax())
				extent.Ymax( zone.Ymax() );
			if (extent.Ymin() < zone.Ymin())
				extent.Ymin( zone.Ymin() );
		}
		//	
		// "snap" to the edges
		//
		if ( extent.Xmin() < narrow )
			extent.Xmin( 0.0 );
		if (extent.Ymin() < narrow )
			extent.Ymin( 0.0 );

		if ( fabs( extent.Xmax() - sheet->Length() ) < narrow )
			extent.Xmax( sheet->Length() );
		if ( fabs( extent.Ymax() - sheet->Width() ) < narrow )
			extent.Ymax( sheet->Width() );
		//
		// Resolution Grid Quantize
		//
		grid_edge.Update( 
			ceil( extent.Xmin() / config.Resolution() ),
			ceil( extent.Ymin() / config.Resolution() ),
			floor( extent.Xmax() / config.Resolution() ),
			floor( extent.Ymax() / config.Resolution() ) );

		edge = extent;
	}
	else // unlimited
	{
		edge = sheet->FitZone();

		if (config.GridBarrier())
		{
			// C2dBox user = sheet->BoxUser();
			C2dBox user = sheet->getRecentPart();

			// -------------------------------------------------------
			// COMMON WITH set_grid_barrier
			//
			if ((config.Progression() == PROGRESS_PPX_SPY) ||
				(config.Progression() == PROGRESS_PPX_SNY))
			{
				if (config.NestPosY())
				{
					// Y-
					if ( DEFINED(user.Ymax())
						&& (user.Ymax() > edge.Ymin()) )
					{ edge.Ymin( user.Ymax() ); }
				}
				else
				{
					// Y+
					if ( DEFINED(user.Ymin())
						&& (user.Ymin() < edge.Ymax()) )
					{ edge.Ymax( user.Ymin() ); }
				}
			}
			else // FILL_Y
			{
				if ( DEFINED(user.Xmax())
					&& (user.Xmax() > edge.Xmin()) )
				{ edge.Xmin( user.Xmax() ); }
			}
			// -------------------------------------------------------

			grid_edge.Update( 
				ceil( edge.Xmin() / config.Resolution() ),
				ceil( edge.Ymin() / config.Resolution() ),
				floor( edge.Xmax() / config.Resolution() ),
				floor( edge.Ymax() / config.Resolution() ) );
		}
		else
		{
			grid_edge = sheet->WorkZone();
		}
	}

	config.FillBarrier( grid_edge );
	sheet->FillBarrier( config, edge );
}

void CNestMgr::set_grid_barrier(
	CNestConfig&		config, 
	CSheet*				sheet )
{
	const C2dBox& zone = sheet->FitZone();

	C2dBox extent = sheet->FitZone();
	C2dBox grid_edge;
	//
	// Set the relevant fill zone
	//
	CModel* model = sheet->pModel();
	C2dBox user = sheet->getRecentPart();
	sheet->clearRecentPart();

	// -------------------------------------------------------
	// COMMON WITH set_fill_barrier
	//
	if ((config.Progression() == PROGRESS_PPX_SPY) ||
		(config.Progression() == PROGRESS_PPX_SNY))
	{
		if (config.NestPosY())
		{
			// Y-
			if ( !DEFINED(user.Ymax()) || (user.Ymax() < (extent.Ymin()-SMALL)) )
				return;

			extent = C2dBox( zone.Xmin(), user.Ymax(), zone.Xmax(), zone.Ymax() );
		}
		else
		{
			// Y+
			if ( !DEFINED(user.Ymin()) || (user.Ymin() > (extent.Ymax()+SMALL)) )
				return;

			extent = C2dBox( zone.Xmin(), zone.Ymin(), zone.Xmax(), user.Ymin() );
		}
	}
	else // FILL_Y
	{
		if ( !DEFINED(user.Xmax()) || (user.Xmax() < (extent.Xmin()-SMALL)) )
			return;

		extent = C2dBox( user.Xmax(), zone.Ymin(), zone.Xmax(), zone.Ymax() );
	}
	// -------------------------------------------------------

	//
	// Resolution Grid Quantize
	//
	grid_edge.Update( 
		ceil( extent.Xmin() / config.Resolution() ),
		ceil( extent.Ymin() / config.Resolution() ),
		floor( extent.Xmax() / config.Resolution() ),
		floor( extent.Ymax() / config.Resolution() ) );

	config.FillBarrier( grid_edge );
	sheet->FillBarrier( config, extent );
}

CReturn CNestMgr::Chain(
	CModel& model,
	int chain_num, 
	double offset )
{
	CReturn ret;
	//
	// Explode and remove patterns
	//
	CDbIterator	iter;
	iter.Init( model.Db(), DBPATTERN );
	while (1)
	{
		CDbPattern* dbPattern = dynamic_cast<CDbPattern*>( iter() );
		if (dbPattern == NULL)
			break;
		iter.Next();

		ret += CModelUtil::PatternExplode( (*dbPattern) );
		dbPattern->Delete();
	}
	//
	// Chain by workzones...
	//
	iter.Init( model.Db(), DBFEATURE );
	while (true)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>(iter());
		if (dbFeature == NULL)
			break;
		iter.Next();

		if ( dbFeature->IsWorkZone() )
		{ ret += do_chain(dbFeature, model, chain_num, offset); }
	}

	return ret;
}

#if (_CI || _NST)
#if (_PM4)

	CReturn
	CNestMgr::SaveSheet(
				CNestConfig&	config, 
				CPartBin&		partbin,
				int				sheet_count,
				CSheet*			sheet )
	{
		CReturn	status;

		CScModelConvertor	convertor;
		CScModel	scModel;
		CString		pathname;

		CCiModel* ciModel = CPortal::CiModel();

		if (false)
		{
			CModelText	report;
			status = report.Dump( "c:\\_tmp\\_ci_to_mm2.txt", sheet->Model() );
		}

		convertor.ConvertToPM4( sheet->Model(), &scModel, (config.LabelName() != 0) );

		sheet->Label( config, partbin );

		// This pathname construction WAS done by the sheet save.
		pathname.Format( "%s\\%s,sheet#%d.%s",
			config.ResultPath(), config.ResultName(), sheet_count, "pm4" );

		// This is written to the sheet results.
		sheet->PathName( pathname );
		scModel.FileWrite( pathname );

		return status;
	}

#else

	CReturn
	CNestMgr::SaveSheet(
				CNestConfig&	config, 
				CPartBin&		partbin,
				int				sheet_count,
				CSheet*			sheet )
	{
		CReturn	status;

		CCiModelConvertor	convertor;
		CString		vector;
		CString		pathname;

		CCiModel* ciModel = CPortal::CiModel();

		if (false)
		{
			CModelText	report;
			status = report.Dump( "c:\\_tmp\\_ci_to_mm2.txt", sheet->Model() );
		}

		ciModel->Flush();
		convertor.ConvertToCI( sheet->pModel(), ciModel, (config.LabelName() != 0) );
		
		if (false)
		{
			status = ciModel->ModelDump( "c:\\_tmp\\NestMgr.txt" );
		}

		// For code generation ...
		ciModel->HeaderDoubleSet( "_dx", sheet->Length() );
		ciModel->HeaderDoubleSet( "_dy", sheet->Width() );
		ciModel->HeaderDoubleSet( "_dz", sheet->Thick() );
		ciModel->HeaderIntSet( "_ts0", config.ToolSetupKey() );
		ciModel->HeaderStringSet( "_pdb", config.ResultName() );
		ciModel->HeaderStringSet( "_ni", config.NestingIDs() );

		sheet->Label( config, partbin );

		// vector.Format( "%%s\\%%s%%%02dd.%%s", config.ResultDigits() );
		vector = "%s\\%s,sheet#%d.%s";

		if (false)
		{
			CMM2 mm2;
			// This pathname construction WAS done by the sheet save.

			pathname.Format( vector,
						config.ResultPath(),
						config.ResultName(),
						sheet_count,
						"nst" );
			sheet->PathName(pathname);

			// mm2.Write( "c:\\_tmp\\_cinest.mm2", (*sheet->pModel()) );
			mm2.Write( pathname, (*sheet->pModel()) );
		}

#if (_NST)
		{
		// (0) DWG / (1) DXF
		int ofile_type = CRegister::IntGetV( "Preferences", "ofile", 0 );

		// This pathname construction WAS done by the sheet save.
		pathname.Format( vector,
					config.ResultPath(),
					config.ResultName(),
					sheet_count,
					((ofile_type == 1) ? "dxf" : "dwg") );
		}

		// This is written to the sheet results.
		sheet->PathName(pathname);
		ciModel->CadNesterFileSave( pathname );
#else
		// This pathname construction WAS done by the sheet save.
		pathname.Format( vector,
					config.ResultPath(),
					config.ResultName(),
					sheet_count,
					"dwg" );

		
		// This is written to the sheet results.
		sheet->PathName(pathname);
		ciModel->FileSave( pathname );
#endif

		return status;
	}

#endif
#endif

// Static
CReturn CNestMgr::do_chain(
	CDbFeature* work_zone,
	CModel& model,
	int chain_num, 
	double offset )
{
	CReturn ret;
	CDbCurveList	line_list;
	C3dCoordArray	waypoint;
	CSeqRules		rules;
	CTSP			tsp;

	// Find the lines marked for chaining...
	CSelector selector(model);

	selector.All(0);
	selector.Filter(DBFEATURE, 1);
	selector.Restrictions(FALSE);
	selector.AllowWorkzone(TRUE);
	selector.Add(work_zone, TRUE);

	int idx, num = selector.Count();
	for (idx=0; idx<num; idx++)
	{
		CDbLine* line = dynamic_cast<CDbLine*>(selector[idx]);
		if (line)
		{
			int chain = line->IntGet("chain", 0);
			if (chain)
			{ 
				C3dCoord st = line->StartPt();
				waypoint.Append( new C3dCoord(st.X(), st.Y(), line_list.Count()) );

				line_list.Append(line);
			}
		}
	}

	selector.Clear();

	// Get them into useable order.
	rules.linear_trend = 100;
	rules.grid_opt = TRUE;
	rules.trend = LL_YPOS;
	rules.bidirectional = FALSE;
	rules.extents = model.BoxUser();

	tsp.Init(&rules, &waypoint);
	waypoint.DestructiveFlush();

	tsp.Evaluate(&waypoint, 1000);

	// Now, chain the little buggers in clumps
	num = waypoint.Count();
	if (num > 1)
	{
		int chain = 1;
		C3dCoord* pt = waypoint[0];
		C3dCoord* prev_pt = pt;
		CDbLine* prev_line = (CDbLine*)line_list[(int)pt->Z()];

		C2dVec trend = *(waypoint[1]) - *pt;

		for (idx=1; idx<num; idx++)
		{
			pt = waypoint[idx];

			C2dVec move = *pt - *prev_pt;
			if ( (trend * move) < 0)
				chain = 0;

			CDbLine* line = (CDbLine*)line_list[(int)pt->Z()];
			if (chain)
			{
				CModelUtil::ChainCut(model, prev_line, line, offset, FALSE);
			}

			prev_line = line;
			prev_pt = pt;
			chain = (chain + 1) % chain_num;
		}
	}

	waypoint.DestructiveFlush();

	// 2006.04.17 (PE) -- Eliminate any "useless containers that
	// are generated as a side-effect of chaining the parts.
	//    CModelUtil::EmptyContainers( model );  // insufficient
	UselessFeaturesDelete( &model, work_zone );

	return ret;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// (2004.05.08) Flip the remnant about its mid-Y-axis.
// ASSUMPTION: Doing this allows the clean edge of the remnant to
// be placed against the stop pins when it is loaded for reuse.
//
// Scavenged from CNestConfig::ChangeModelY()
//
void CNestMgr::remnant_flip( CModel* remnant_model )
{
	C3x4Matrix	to_origin;
	C3x4Matrix	mirror;
	C3x4Matrix	from_origin;
	C3dBox		box;
	CDbProfile*	dbProfile;
	int			count, indx;

	CSelectorStack& selectorStack = remnant_model->SelectorStack();
	selectorStack.Push();

	CSelector& selector = selectorStack();

	selector.All( 0 );
	//selector.Filter( DBPOINT, 1 );
	//selector.Filter( DBLINE, 1 );
	//selector.Filter( DBARC, 1 );
	//selector.Filter( DBHOLE, 1 );		// unlikely to exist in remnant
	//selector.Filter( DBCOMMAND, 1 );	// unlikely to exist in remnant
	selector.Filter( DBPROFILE, 1 );

	// 2006.09.08 (PE) -- added when working on 'flip' feature for V18.
	selector.Restrictions( TRUE );

	selector.SystemFlag( TRUE );
	selector.SelectAll( TRUE );

	count = selector.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbProfile = dynamic_cast<CDbProfile*>( selector[indx] );
		if (dbProfile != NULL)
			dbProfile->Reverse();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Flip the remnant about its mid-Y-axis.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// TODO: We need to affect the flippage of punch tools (unlikely requirement).
	to_origin.InvertTo( &from_origin );
	mirror.Scale( -1, 1, 1 );

	to_origin.Transform( &mirror );
	mirror.Transform( &from_origin );

	CModelUtil::Transform( remnant_model, from_origin, 0, XFORM_NO_SYSTEM );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Move the left edge back to the Y axis.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	to_origin.setUnit();
	mirror.setUnit();

	box = remnant_model->Box();
	if (fabs(box.Xmin()) > 0.001)
	{
		// 2004.05.09 (PE) -- Whilst getting remnant tracker to work again.
		// Gary and I decided the remnant was being incorrectly flipped.
		// The previous incarnation used CNestConfig::ChangeModelY(), which
		// flipped the remnant about its X centerline.  We decided that
		// solution could make it difficult to reuse a remnant because the
		// left edge of the remnant may have no useful material with which
		// to position the material against the locating pins.  As such, we
		// now flip the material about its Y centerline so the (assumed) clean
		// right edge the material is now the left edge.  The user can, of
		// course, manually reorient the remnant by opening and modify its MM2.

		to_origin.Shift( C3dVec( -box.Xmin(), 0., 0. ) );

		CModelUtil::Transform(
			remnant_model, to_origin, 0, XFORM_NO_SYSTEM );
	}

	selector.Clear();
	selectorStack.Pop();
}

void CNestMgr::SeedsGenerate( 
	CNestConfig&		config, 
	CSheet&				sheet,
	CPartBin*			partbin,
	CToolHitArray*		next_hit,
	CSeedArray*			next_pos )
{
	if (CReturn::Debug() >= 3)
		m_seed_mgr.SeedsPrint();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// We get the next candidate part location from the sheet
	// because the sheet knows both the location of the last
	// successfully placed part and the seeds around the
	// borders of the sheet.
	//
	CSeed seed = sheet.NextSeedGet();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the the bottom-left corner of the candidate parts mer.
	// This point is generated by one of the methods of CSheet:
	// SeedBarrier(), SeedPoly(), SeedPartEdge() or SeedsDerive().
	//
	// NOTE: An undefined location indicates that all seeds
	// have been exhausted.
	//
	C2dCoord part_place = seed.Placement();

	bool did_process = false;

	int part_count = (part_place.IsDefined() ? partbin->Count() : 0);
	for (int part_indx = 0; part_indx < part_count; ++part_indx)
	{
		CNestingPart* npart = partbin->GetAt(part_indx);

		if ( CanProcess( config, (*npart) ) )
		{
			// For each orientation of this part ....
			int hit_count = npart->Count();
			for (int hit_indx = 0; hit_indx < hit_count; ++hit_indx)
			{
				if ( sheet.DidPlace() )
				{
					next_link( next_hit, next_pos, config, partbin,	sheet, part_place );
				}
				else
				{
					CToolHit* toolhit = npart->ToolHit( hit_indx );

					const C2dBox& mer = toolhit->Mer();

					// Calculate the candidate handle position.
					double dx = mer.Xmin();
					double dy = (config.NestPosY() ? mer.Ymin() : mer.Ymax());

					C2dCoord candidate( (part_place.X() - dx), (part_place.Y() - dy) );

					// 2006.02.21 (PE) -- I noticed that sometimes
					// the final row of parts would not nest.  I
					// determined the parts were rejected during
					// scoring. As such, we nudge the handle point
					// so that part stay on the sheet.
					//
					// NOTE: We may be able to restrict this adjustment
					// to certain propogation directions.
					dy = candidate.Y() + mer.Ymin();

					ConditionalSeedAdd( candidate, toolhit, next_pos, next_hit );
				}
			}

			did_process = true;
			if (config.FirstPartInspection() == 1)
			{
				npart->FirstPartInspection( 1 );
				break;
			}

		}
	}

	// Disable first part inspection after same has been done for all parts.
	if (!did_process && (config.FirstPartInspection() == 1))
		config.FirstPartInspection(2);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TODO: Replace separate arrays with one array of some objects?
void CNestMgr::ConditionalSeedAdd(
	const C2dCoord&		candidate,
	CToolHit*			toolhit,
	CSeedArray*			next_pos,
	CToolHitArray*		next_hit )
{
	CSeed*	seed;

	if ( IsUniquePlace( (*next_pos), (*next_hit), candidate, toolhit ) )
	{
		seed = new CSeed( SEED_PRIORITY, candidate, toolhit, 0 );

		next_pos->Append( seed );
		next_hit->Append( toolhit );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: It is far cheaper to filter out candidates than to allow
// score_parts() to process duplicates.  This should yeild significant
// improvements in performance.
bool CNestMgr::IsUniquePlace(
	const CSeedArray&		next_pos,
	const CToolHitArray&	next_hit,
	const C2dCoord&			candidate,
	const CToolHit*			tool_hit )
{
	CSeed*	tmp;
	int		count, indx;

	count = next_pos.Count();
	for (indx = 0; indx < count; ++indx)
	{
		tmp = next_pos.GetAt( indx );
		if ( tmp->Placement().WithinTol( candidate, 1.e-4 ) )
		{
			if (tool_hit == next_hit[indx])
				break;  // duplicate (within arbitrary tolerance
		}
	}

	return (indx >= count);
}


#define PRIMARY_FACTOR  0.75
#define SECONDARY_FACTOR  0.25	// Magic numbers; no real basis for their definition

void CNestMgr::DoGridStuff(
	CNestConfig&	config,
	CGeoPolyArray*	poly_array,		// Polygon(s) that define seeds...
	int				idx,
	bool			hole,			// TRUE if we are nesting inside a hole
	CSheet*			sheet,
	const CPartPlace&	best )
{
	CReturn	status;
	bool	column_skip = false;

	CNestingPart* npart = best.PartGet();
	CToolHit* toolhit = best.ToolHitGet();

	C2dCoord corner(
		toolhit->Grid()->Origin().X() + best.WorldPointGet().X(),
		toolhit->Grid()->Origin().Y() + best.WorldPointGet().Y() );

	if (m_lastcol.IsDefinedXY())
	{
		C2dVec c_delta = (corner - m_lastcol);

		if ((config.Progression() == PROGRESS_PPX_SPY) ||
			(config.Progression() == PROGRESS_PPX_SNY))
		{
			double x_overlap = npart->Extent().Dx()*PRIMARY_FACTOR;
			double y_overlap = npart->Extent().Dy()*SECONDARY_FACTOR;

			if ( (config.NestPosY() &&  (c_delta.Y() > y_overlap ) )
				|| (!config.NestPosY() && (c_delta.Y() < -y_overlap) )
				|| (c_delta.X() < -x_overlap) )
			{
				column_skip = true;
			}
		}
		else // FILL_Y
		{
			double y_overlap = npart->Extent().Dy()*PRIMARY_FACTOR;
			double x_overlap = npart->Extent().Dx()*SECONDARY_FACTOR;

			if ( (config.NestPosY() &&  (c_delta.Y() < -y_overlap ) )
				|| (!config.NestPosY() && (c_delta.Y() > y_overlap) )
				|| (c_delta.X() > x_overlap) )
			{
				column_skip = true;
			}
		}
	}

	m_lastcol = corner;
	if (column_skip)
	{
		// RETROGRADE placement
		//
		config.NewLine(true);

		if (!hole)
		{ 
			set_grid_barrier( config, sheet );
			if (idx == RESERVE_BARRIER)
				sheet->SeedBarrier( config, poly_array->GetAt(RESERVE_BARRIER), +1.0 );
		}

		EWMNesting( "Retrograde motion; reset column." );

		m_lastcol = C3dCoord();
	}
}

// 2006.04.17 (PE) -- Eliminate any "useless containers that
// are generated as a side-effect of chaining the parts.
void CNestMgr::UselessFeaturesDelete( CModel* model, CDbFeature* work_zone )
{
	CDbFeature*	dbFeatureA;
	CDbFeature*	dbFeatureB;
	int			indx;

	for (indx = 0; indx < work_zone->Count(); ++indx)
	{
		dbFeatureA = dynamic_cast<CDbFeature*>( work_zone->GetAt(indx) );
		if (dbFeatureA == NULL)
			continue;

		if (dbFeatureA->IntGet( "exp", 0 ) == 0)
			continue;

		// We have encountered a feature that we created by
		// exploding a pattern via CModelUtil::PatternExplode().
		if (dbFeatureA->Count() != 2)
			continue;

		// We may have a "classic" left over that should contain
		// an empty feature, followed by a command entity.
		dbFeatureB = dynamic_cast<CDbFeature*>( dbFeatureA->GetAt(0) );
		if (dbFeatureB == NULL)
			continue;

		if (dbFeatureB->Count() > 0)
			continue;  // guess not :-(

		// Delete the "useless" feature.
		model->EntityDelete( dbFeatureA->Id() );

		// Keep the loop index synchronized with the features
		// processed so far in the work zone.
		--indx;
	}
}

bool CNestMgr::CanProcess( const CNestConfig& config, const CNestingPart& part )
{
	if ( part.Error() )
		return false;

	if (part.Quantity() == 0)
		return false;  // This part has been exhausted.

	if (config.FirstPartInspection() == 1)
	{
		// ie. not considered and not placed.
		if (part.FirstPartInspection() > 0)
			return false;
	}
	else
	{
		if (part.Pass() != config.Pass())
			return false;  // Not appropriate time to nest this part.
	}

	return (part.ToolMatch() || !part.Tooled());
}

// Mark parts as large, record the longest
// part and strip leads from large parts.
void CNestMgr::LargePartsPrep(
	const CSheet&		sheet, 
	CPartBin*			partbin,
	CNestConfig*		config )
{
	C2dBox			test;
	CNestingPart*	npart;
	double			max_zone;
	double			max_large;
	double			max_part;
	int				indx;

	// Mark relevant parts as "large", for placement in Pass 0 for this sheet
	max_zone = max( sheet.Width(), config->MachineTravelX() );

	max_large = 0.;

	config->Active( 0, FALSE );
	for (indx = 0; indx < partbin->Count(); ++indx)
	{
		npart = partbin->GetAt(indx);
		if ( npart->Error() )
			continue;

		test = npart->Extent();

		max_part = max( test.Dx(), test.Dy() );
		max_large = max( max_large, max_part );

		if ((max_part + config->Spacing()) > (max_zone - SMALL))
		{
			if (config->Narrow(0) == DBL_MAX)
				config->Narrow( 0, config->Narrow(npart->Pass()) );
			else
				config->Narrow( 0, min( config->Narrow(0), config->Narrow(npart->Pass()) ) );

			npart->Large(TRUE);
			npart->StripLeads();

			config->Active( 0, TRUE );
			config->HaveLargeParts( true );
		}
		else
			npart->Large(FALSE);
	}

	config->LargePartSize( max_large );
}

CNestedArea* CNestMgr::InnerAreaGet(
	const CNestConfig&	config,
	CNestedArea*		area_to_nest )
{
	CNestedArea* inner_nested_area = NULL;

	// Clearly, when nesting from small to large parts, you will
	// never be able to nest a larger part inside a smaller part.
	if (config.PartInPart() && (config.NestOrder() != NEST_SMALL))
	{
		// TODO: We to change nesting so that it processes all
		// interior areas of the part. As currently implemented,
		// nesting behaves as if the part has (0..1) holes.
		int indx = area_to_nest->InteriorAreasCount() - 1;
		if (indx >= 0)
			inner_nested_area = area_to_nest->InteriorAreaGet( indx );
	}

	return inner_nested_area;
}

void CNestMgr::PackingScore(
	const CNestConfig&	config,
	const CNestedArea&	na,
	CPartPlace*			pp,
	CViewMgr&			view  )
{
	C3dCoordArray	pts;
	C3dVec			vec;
	CGeoPoly*		convex_hull;
	CGeoPoly*		polyA;
	CGeoPoly*		polyB;
	int				indx;

	indx = (config.IsBitmapNest() ? HIT_NEST_OUTSIDE : HIT_PART_OUTSIDE);

	polyA = na.PolysGet(indx)->GetAt(0);
	polyB = pp->ToolHitGet()->PolysGet(indx)->GetAt(0);

	vec.Init( 0., 0., 0. );
	polyA->Tabulate( 1.e-3, vec, &pts );

	vec.Init( pp->WorldPointGet().X(), pp->WorldPointGet().Y(), 0. );
	polyB->Tabulate( 1.e-3, vec, &pts );

	convex_hull = CGeoPoly::ConvexHull( pts );
	PolyRender( (*convex_hull) );

	pp->OverlapScoreSet( 1. / fabs( convex_hull->Area() ) );

	delete convex_hull;

	pts.DestructiveFlush();
}

static int DBG7 = 0;
static int DBG8 = 0;

void CNestMgr::PolyRender( const CGeoPoly& poly )
{
	if ( DBG7 )
		poly.Draw();
}
