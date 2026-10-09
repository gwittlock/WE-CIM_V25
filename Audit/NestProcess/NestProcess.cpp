
#include "stdafx.h"
#include "Register.h"

#include "ActionConst.h"
#include "MathConst.h"
#include "CommonFlags.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Path.h"
#include "oglViewWire.h"
#include "Portal.h"

#include "NestConfig.h"
#include "PanelNestMgr.h"

#include "RepoZone.h"
#include "Repo.h"

#include "PartBin.h"
#include "Sheet.h"

#include "Model.h"
#include "ModelUtil.h"
#include "ViewMgr.h"
#include "db.h"
#include "DbIterator.h"
#include "DbTool.h"
#include "DbArc.h"

#include "CutBack.h"

#include "NestProcess.h"

#include "MM2.h"

#include "Optimizer.h"

#include "GeoPoly.h"
#include "Lead.h"

// Thank the Lord for people who share their hard work!
#include "ProgressWnd.h"

#include "PathFinder.h"
#if (_CI || _NST)
	#include "ImportUtil.h"
	#include "CiModelConvertor.h"
#endif

enum EBumpOption
{
	BUMP_INIT   = 0,
	BUMP_ANCHOR = 1,
	BUMP_ORIENT = 2,
	BUMP_DRAG   = 3,
	BUMP_MOVE   = 4,
	BUMP_COPY   = 5,
	BUMP_CANCEL = 6
};


static C3dCoord ORIGIN( 0., 0., 0. );

C2dCoord CNestProcessApp::m_handle = C2dCoord( 0., 0. );
C2dCoord CNestProcessApp::m_anchor = C2dCoord( 0., 0. );
C2dVec CNestProcessApp::m_vec = C2dVec( 0., 0. );
double CNestProcessApp::m_radians = 0.;
double CNestProcessApp::m_delta_radians = 0.;
double CNestProcessApp::m_shift = 0.;

// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNestProcessApp

BEGIN_MESSAGE_MAP(CNestProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CNestProcessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNestProcessApp construction

CNestProcessApp::CNestProcessApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNestProcessApp object

CNestProcessApp theApp;

CRouteList CNestProcessApp::m_router;
CRouteList CNestProcessApp::m_manual;

CManualData	CNestProcessApp::m_mandata;

static CProfiler g_nesting_profiler;

// ==================================================================

static CSelectorStack		dummy_stack;
static CTreeViewSupport		dummy_tree;

// Used by Nest:Manual:Bump:
void WorldToScreen( const C3dCoord& world, CPoint* screen )
{
	CoglViewWire* oglview = dynamic_cast<CoglViewWire*>( CViewMgr::ActiveView() );

	screen->x = (int) (world.X() * oglview->Scale());
	screen->y = (int) -(world.Y() * oglview->Scale());
}

// ==================================================================

CReturn 
CNestProcessApp::RegisterProcess( 
	CRouteList*	io_route )
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;

	ret = io_route->addSubrouter( "Nest", &m_router );

	ret += m_router.addProcess( "True", NestTrue );
	ret += m_router.addProcess( "Panel", NestPanel );
	ret += m_router.addProcess( "Repo", NestRepo );

	ret += m_router.addProcess( "pseq_init", PartSeqInit );
	ret += m_router.addProcess( "pseq_next", PartSeqNext );

	ret += m_router.addProcess( "Generate", Generate );

	ret += m_router.addProcess( "Chain", Chain );
	ret += m_router.addProcess( "RemnantCommit", RemnantCommit );
	ret += m_router.addProcess( "RemnantStats", RemnantStats );
	ret += m_router.addProcess( "Test", Test );

	ret += m_router.addProcess( "FitTest", FitTest );

	ret += m_router.addProcess( "CutBack", &CutBack );

	// ------------------------------------------------

	ret += m_router.addSubrouter( "Manual", &m_manual );

	ret += m_manual.addProcess( "Sheet", &ManualSheet );
	ret += m_manual.addProcess( "Pattern", &ManualPattern );
	ret += m_manual.addProcess( "Instance", &ManualInstance );
	ret += m_manual.addProcess( "Selection", &ManualSelection );
	ret += m_manual.addProcess( "JumpTo", &ManualJumpTo );
	ret += m_manual.addProcess( "Bump", &ManualBump );
	ret += m_manual.addProcess( "Punch", &ManualPunch );

	return CReturn( STATUS_OKAY );
}

// ==================================================================
//	Route to the sub-command under Nest
CReturn CNestProcessApp::Nest( CCommand* io_cmd )
	{ return m_router.Dispatch( io_cmd ); }


// ==================================================================
//	Nest:True:configdb=%s, partdb=%s
//
//	Nest:True:configdb="e:/work/bbi5/advmach/debug/database/cmdb.mdb", partdb="e:/work/bbi5/advmach/debug/database/3.pdb"
//	Nest:True:configdb="e:/work/bbi5/advmach/debug/database/cmdb.mdb", partdb="e:/work/bbi5/advmach/debug/database/fred.pdb"
//
// TODO:  move the config Pass and Counter controls out of the sub-methods,
// so it is explicit and not implied during other actions.
//
CReturn CNestProcessApp::NestTrue( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	CReturn		status;

	CSheet		sheet;
	CPartBin	partbin;
	CNestMgr	nest;
	CProgressWnd progress_wnd;

	const CVarList& args = io_cmd->VarList();

	// 2009.08.08 (PE) -- Introduced to handle ITI-specific behaviors.
	int ccs = args.getInt( "ccs", 0 );
	if (ccs == 0)
	{
		// A registry over-ride to make things easier.
		ccs = CRegister::Debug( "ccs" );
		CNestConfig::CcsNesting( ccs != 0 );
	}

	CNestConfig& config = NestConfigGet();
	config.NestConfigInit();
	config.CcsNesting( (ccs != 0) );

	CString configdb = args.getString( "configdb", "" );
	CString partdb = args.getString( "partdb", "" );

	bool silent = CReturn::IsSilentMode();
	if ( silent )
	{
		if ( ccs )
		{
			CPath path( partdb );
			path.ForceExt( "log" );
			CReturn::LogFile( path.FullPath() );
		}
	}
	else
	{
		progress_wnd.ProgressWindowInit( NULL, "Nesting", TRUE );
		progress_wnd.SetWindowSize( 0, 200 );
		progress_wnd.SetRange( 0, 50 );
	}

	// ------------------------------------------

	if (configdb.IsEmpty() || partdb.IsEmpty())
	{
		status.Internal( IDS_NEST_PARAM_MISSING );
		return status;
	}

	bool did_print = view.ActiveView()->PrintText();
	view.ActiveView()->PrintText( false );

	EWMNesting( "TRUE SHAPE NESTING" );

	status += CProgressWnd::ProgressWndUpdate();

	// ----------------------------------------------------

	// TODO: Based enabling on registry entry.
	g_nesting_profiler.Enable( true );

	g_nesting_profiler.In( "NestTrue" );  // track overall time

	config.GridNest( true );
	status += do_load_info(config, model, sheet, partbin, configdb, partdb );

	// ----------------------------------------------------
	// Prepare parts for nesting...
	//		Simplify, Rotate, Lead, Generate, Assemble, Area
	//
	status += CProgressWnd::ProgressWndUpdate();

	if (status.isOkay())
	{
		CProgressWnd::SetText("Analyzing Parts");

		// 2012.04.28 (PE) -- A complete hack!
		// It took over 10 years for this problem to be reported:
		//   "Nesting fails to find the piece tool for a lead setup."
		// This is a pragamtic (if inelegant) way to make the
		// required information available to the leading-subsystem.
		//
		// NOTE: We cast away const only because downstream methods
		// on CModel (et al.) do not have the const qualifier ...
		// and sorting all that out is not worth the effort.

		CLead::MasterModel( &model );
		status += do_simplify_parts(config, sheet, partbin );
		CLead::MasterModel( NULL );

		status += do_make_nestable(config, partbin, view);
	}

	if (status.isOkay())
	{
		double filter_area = max( config.SmallHoleArea(), config.SmallPartArea() );
		status += do_filter(config, partbin, filter_area, view);
	}

	status += CProgressWnd::ProgressWndUpdate();

	if (status.isOkay())
		status += do_ordering(config, partbin, view);

	status += CProgressWnd::ProgressWndUpdate();

	if (status.isOkay())
		status += do_resolution(config, partbin, view);

	status += CProgressWnd::ProgressWndUpdate();

	if (status.isOkay())
		status += sheet.Prepare( config, view, model );

	status += CProgressWnd::ProgressWndUpdate();

	if (status.isOkay())
		status += do_slice(config, partbin, view);

	status += CProgressWnd::ProgressWndUpdate();

	if ( status.isOkay() )
		status += do_gridlink( config, partbin, model, nest, view );

	status += CProgressWnd::ProgressWndUpdate();

	if (status.isOkay())
	{
#if _NST
		// 2005.02.09 (PE) -- Otherwise we crash :-(
		model.pHeader()->setReal( STR_LENGTH, sheet.Extent().Dx() );
		model.pHeader()->setReal( STR_WIDTH, sheet.Extent().Dy() );
#endif
		status += do_prenest( config, partbin, model, nest, view );
	}

	// ----------------------------------------------------
	// Perform actual nesting
	if (status.isOkay())
	{
		EWMNesting( "...Execute nest" );
		
		g_nesting_profiler.In( "nest.Execute" );

		status += CProgressWnd::ProgressWndUpdate();
		status += nest.Execute( io_cmd, config, &sheet, &partbin, view );
		
		g_nesting_profiler.Out( "nest.Execute" );

		view.ActiveView()->PrintText( did_print );

		view.Clear();
		view.ModelSet( model );
		view.Refresh( false );  // true );
	}

	g_nesting_profiler.Out( "NestTrue" );  // total time

	g_nesting_profiler.Dump( "g_nesting_profiler" );
	g_nesting_profiler.ResetAll();

	return status;
}

// ----------------------------------------------------------------------------
//	Load parts and nesting information
CReturn CNestProcessApp::do_load_info(
	CNestConfig& config,
	CModel& model,
	CSheet& sheet,
	CPartBin& partbin,
	const CString& configdb,
	const CString& partdb )
{
	CReturn status;

	EWMNesting( "...Load configuration" );
	g_nesting_profiler.In( "do_load_info" );

	// ************************************
	status += config.Load( configdb, partdb );

#if (_CI || _NST)
	if (status.isOkay())
	{
		CImportUtil			importUtil;
		CCiModelConvertor	convertor;
		importUtil.Init( &model, NULL);

		// *****************************************************
		importUtil.StandardPlanesCreate( 96., 48., 1., 1, 0 );
		status += convertor.ToolSetupConvert( config.ToolSetupKey(), &model );
	}
#endif

	if (status.isOkay())
	{
		EWMNesting( "...Load sheet" );

		// *************************
		status += sheet.Load( config );
	}

	if (!status.isOkay())
	{
		status.User( IDS_NEST_FIRST_SHEET );
	}
	else
	{
		EWMNesting( "...Loading Parts" );
		CProgressWnd::SetText("Loading Parts");

		// *********************************
		status += partbin.Load( config );
	}

	g_nesting_profiler.Out( "do_load_info" );
	
	return status;
}

// ----------------------------------------------------------------------------

CReturn CNestProcessApp::do_simplify_parts(
	CNestConfig& config,
	CSheet& sheet,
	CPartBin& partbin )
{
	CReturn	ret;
	CString	note;
	double	min_gap, max_gap;

	g_nesting_profiler.In( "do_simplify_parts" );

	config.Counter(0);
	for (int idx=0; idx<partbin.Count(); idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);

		Sleep(0);

		if (npart->pModel())
		{
			CReturn part_ret;
			// *******************************************************
			part_ret = npart->Simplify( config );
			// *******************************************************
			if (part_ret.isError())
			{
				part_ret.Internal( IDS_NEST_ERR_SIMPLIFY, partbin.GetAt(idx)->Filepath() );
				npart->Error( part_ret.LastErrorMsg() );
				continue;
			}
		}
		config.Counter(config.Counter()+1);
	}

	if ( CNestConfig::IsBitmapNest() )
	{
		min_gap = 1.e-3;
	}
	else
	{
		min_gap = CRegister::DoubleGetV( "Nesting", "gap_tol", -UNDEFINED );
	}

	if (min_gap > 0.)
	{
		config.GapTolerance( min_gap );
	}
	else
	{
		// Now accept the analysis made by simplify
#if BEFORE_V18
		min_gap = config.MinTool() * 0.25;
#else
		min_gap = config.GapTolerance();
#endif
		max_gap = min_gap * 2.0;
		if (min_gap > config.GapTolerance())
		{
			config.GapTolerance( min_gap );

			if ( EWMNestingAllow() )
			{
				note.Format( "Simplify Tooling: adjust tab gap to %f", min_gap );
				EWMNesting( note );
			}
		}
		else
		{
			if ((config.GapTolerance() > max_gap) && EWMNestingAllow())
			{
				note.Format( "Recommended gap tolerance is between %f and %f (user value of %f may cause nesting problems)", min_gap, max_gap, config.GapTolerance() );
				EWMNesting( note );
			}
		}

		if ( EWMNestingAllow() )
		{
			note.Format( "Gap Tolerance %f", config.GapTolerance());
			EWMNesting( note );
		}
	}

	g_nesting_profiler.Out( "do_simplify_parts" );

	return ret;
}

// ----------------------------------------------------------------------------

#include "ModelText.h"

CReturn
CNestProcessApp::do_make_nestable(
	CNestConfig& config,
	CPartBin& partbin,
	CViewMgr& view)
{
	CReturn status;
	CString note;
	CPath	path;
	C2dBox	max_leadbox;
	double	delta;
	
	g_nesting_profiler.In( "do_make_nestable" );

	config.Counter( 0 );
	for (int idx=0; idx<partbin.Count(); idx++)
	{
		if ( EWMNestingAllow() )
		{ 
			CString note;
			note.Format( "... Part %d:", idx );
			EWMNesting( note );
		}

		CNestingPart* npart = partbin.GetAt(idx);
		if (npart->Error())
			continue;

		Sleep(0);

		if (npart->pModel())
		{
			if (config.DebugWire() || config.DebugBmp())
			{
				// Draw full part to be nested
				view.Clear();
				view.ModelSet( (*(npart->pModel())) );
				view.Refresh( false );  // true );
			}

			path.Set( npart->Filepath() );
			note.Format( "Conditioning: %s", (LPCSTR)path.FileName() );
			CProgressWnd::SetText( note );

			EWMNesting( "...Rotate part" );

			// *********************************
			status += npart->Rotate();
			// *********************************

			if (!config.GridChain())
			{
				EWMNesting( "...Lead part" );

				g_nesting_profiler.In( "LeadPart" );

				status += npart->Lead( config );

				g_nesting_profiler.Out( "LeadPart" );
			}

			if ( CNestConfig::IsBitmapNest() )
				status += npart->Translate();

			// TODO:  Catch Rotate and Lead errors

			EWMNesting( "...Generate and Assemble part" ); 

			// TODO:  Generate, Assemble, and Space BEFORE rotation, then rotate results???
			delta = 360. / npart->Count();
			for (int ang=0; ang<npart->Count(); ang++)
			{
				note.Format( "Conditioning: %s at %d",
					(LPCSTR)path.FileName(), (int)(npart->StartAngle() + (ang * delta)) );
				CProgressWnd::SetText( note );

				// Temporarily suppress Diagnostic() messages.
				int debug = CReturn::Debug();
				CReturn::Debug(0);

				status += CProgressWnd::ProgressWndUpdate();
				if ( status.IsOk() )
				{
					// We must generate the part kerf.
					g_nesting_profiler.In( "Generate" );
					status += npart->Generate( ang );
					g_nesting_profiler.Out( "Generate" );

					if ( !status.IsOk() )
						break;

					// And various other stuff for the exact-spacing algorithm.
					if ( !CNestConfig::IsBitmapNest() )  // DYNATORCH
					{
						g_nesting_profiler.In( "Assemble" );
						status += npart->Assemble( config, ang );
						g_nesting_profiler.Out( "Assemble" );

						if ( !npart->Error() )
						{				
							if ( !ZERO(config.Spacing()) )
							{
								g_nesting_profiler.In( "AddSpace" );
								status += npart->AddSpace( config, ang, HIT_NEST_OUTSIDE );
								status += npart->AddSpace( config, ang, HIT_NEST_INSIDE );
								g_nesting_profiler.Out( "AddSpace" );
							}

							g_nesting_profiler.In( "AddSpace" );
							status += npart->Accessorize( config, ang );
							g_nesting_profiler.Out( "AddSpace" );
						}
					}
				}

				CReturn::Debug(debug);
				if ( npart->Error() )
					break;
			}

			if ( !status.IsOk() )
				break;

			if ( !npart->Error() )
			{
				// Determine areas; sets up inside/outside info
				// If mode is actually AreaOffset(), then re-calculated later
				npart->calcArea( config );

				// Remove these temporary entities, lest they appear in the nest.
				npart->GapEntitiesRemove();
			}

			// Part counter.
			config.Counter( config.Counter() + 1);
		}

		if ( !status.IsOk() )
			break;
	}

	g_nesting_profiler.Out( "do_make_nestable" );

	return status;
}

// ----------------------------------------------------------------------------

CReturn CNestProcessApp::do_gridlink(
	CNestConfig& config,
	CPartBin& partbin,
	CModel& model,
	CNestMgr& nest,
	CViewMgr& view )
{
	CReturn	status;

	EWMFile( FILE_INFO );
	EWMNesting( "do_gridlink( start )" );
	g_nesting_profiler.In( "do_gridlink" );

	// Otherwise, the reported "Pending" value in the "Cancel" dialog is goofy.
	config.PartsToCut(0);

	CSheet sheet;
	C2dBox extent;
	CString note;
	CPath path;

	int count = partbin.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CNestingPart* npart = partbin.GetAt(indx);

		note.Format( "Start Griding for: %s (%d)", (LPCSTR)path.FileName(), npart->Id() );
		EWMNesting( note );
		
		// For each rotation of this part...
		int orient, orientations = npart->Count();
		for (orient = 0; orient < orientations; ++orient)
		{
			// Calculate the initial bounding box dimensions for this part orientation.
			npart->GridLink( config, orient );

			// As pilfered from CNestMgr::Execute()
			CToolHit* toolhit = npart->ToolHit( orient );

			// This next statement is critical to the subsequent call to
			// cut_part(). Without it, you are unable to see the instance
			// get placed when you are debugging.
			toolhit->hasPatterns( FALSE );
			toolhit->PatternBurn( NULL );
			toolhit->PatternPunch( NULL );
			toolhit->PatternOther( NULL );
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		}
	
		sheet.ViewMgr( &view );

		config.Pass( npart->Pass() );

		// For each rotation of this part...
        double delta = 360 / orientations;
		for (orient = 0; orient < orientations; ++orient)
		{
			EWMFile( FILE_INFO );

			path.Set( npart->Filepath() );
			note.Format( "Griding: %s at %d (%d)",
				(LPCSTR)path.FileName(), (int)(npart->StartAngle() + (orient * delta)), npart->Id() );
			CProgressWnd::SetText( note );

#if 0
			// Disabled because so much output is generated it slows things done.
			// Re-enable as necessary.
			EWMNesting( note );
#endif

			// TODO: Move the config calls outside of loop (?)
			config.GridFlags(GRID_FORCE);
			bool onesheet = config.OneSheet();
			config.OneSheet( TRUE );	// To allow pre-nesting of fillers

			npart->PreHit( orient );
			status += nest.GridNest( config, &partbin, npart, &sheet, model, view );

			// Clear any temporary graphics.
			if ( config.DisplayParts() )
			{
				CViewBase* vbase = view.ActiveView();
				vbase->Clear( TEMP_LISTS );
				vbase->BufferShow( BACK_BUFFER );
			}

			config.OneSheet(onesheet);
		}
	}

	config.GridFlags(config.GridMode());

	EWMNesting( "do_gridlink( end )" );
	g_nesting_profiler.Out( "do_gridlink" );

	return status;
}

// ----------------------------------------------------------------------------

CReturn CNestProcessApp::do_filter(
	CNestConfig& config,
	CPartBin& partbin,
	double filter_area,
	CViewMgr& view)
{
	CReturn status;
	CString note;
	CPath	path;

	EWMFile( FILE_INFO );

	for (int idx=0; idx<partbin.Count(); idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);

		if (npart->Error())
			continue;

		double delta = 360. / npart->Count();
		for (int ang=0; ang<npart->Count(); ang++)
		{
			path.Set( npart->Filepath() );
			note.Format( "Filtering: %s at %d",
				(LPCSTR)path.FileName(), (int)(npart->StartAngle() + (ang * delta)) );
			CProgressWnd::SetText( note );

			if ( npart->pModel(ang) )
				status += npart->Filter( ang, filter_area );
		}
	}

	return status;
}

// ----------------------------------------------------------------------------
// With all the parts in hand, prioritize and manage
// some statistics
//
// ALSO handles configuration pass initialization.
//
// NOTE: Upon calling this method, the partbin will contain
// only those parts having a non-zero count (ie. regular
// parts and filler parts).
//
CReturn CNestProcessApp::do_ordering(
	CNestConfig& config,
	CPartBin& partbin,
	CViewMgr& view)
{
	CReturn			status;
	CNestingPart*	npart;
	int				count, indx;

	EWMFile( FILE_INFO );

	config.PartNum( config.Counter() );

	config.Counter(0);

	// Large-to-small, small-to-large, whatever ....
	partbin.PartsSort( config.NestOrder() );

	count = partbin.Count();
	for (indx = 0; indx < count; ++indx)
	{
		npart = partbin.GetAt(indx);
		if ( !npart->Error() )
		{
			// Assign the part a "pass number" based on
			// the end-user-specified part ordering method.
			npart->ReOrder( config );

			// This is critical to getting the correct pattern label.
			// The pattern label represents the sorted-order of the
			// part in the part bin, but not necessarily the order
			// in which the part is placed.
			npart->Index( indx );

			if (npart->pModel())
			{
				// CRITICAL: config.PassNum() is used CNestMgr::nest_repo()
				// as the upper bound on interations. Failure to properly
				// set the upper bound causes major grief.
				int pass = config.PassNum();
				pass = npart->Pass();
				pass = max( config.PassNum(), npart->Pass() );
				config.PassNum( pass );
			}

			// This is used by case NEST_INORDER in ReOrder().
			config.Counter(config.Counter()+1);
		}
	}

	// *********************************
	config.PassInit( config.PassNum() );
	// *********************************

	return status;
}

// ----------------------------------------------------------------------------
#define AREA_FACTOR 0.10
#define MIN_RES 0.2  // arbitrary (was 0.25)

CReturn CNestProcessApp::do_resolution(
	CNestConfig& config,
	CPartBin& partbin,
	CViewMgr& view)
{
	CReturn			status;
	CNestingPart*	npart;
	double			test_res, reqd_res;
	double			min_res;
	int				count, indx;

	g_nesting_profiler.In( "do_resolution" );

	EWMFile( FILE_INFO );

	count = partbin.Count();

	// Determine the most narrow part. The result is used as the
	// basis for determining our initial testing resolution.
	for (indx = 0; indx < count; ++indx)
	{
		CNestingPart* npart = partbin.GetAt(indx);
		if ( npart->Error() )
			continue;

		int pass_num = npart->Pass();
		const C2dBox& extent = npart->Extent();
		double min_dim = min( extent.Dx(), extent.Dy() );

		config.Narrow( pass_num, min( config.Narrow( pass_num ), min_dim ) );
		config.Active( pass_num, TRUE );
	}

	if ( CNestConfig::IsBitmapNest() )  // DYNATORCH
	{
		// The resolution is either hard-coded in
		// CNestConfig::CNestConfig() or it is yanked
		// from the registry (in the same ctor).
	}
	else if (config.Resolution() >= UNDEFINED)
	{
		// Set the base minimum resolution as a function of the most
		// narrow part. This is, of course, a rather arbitrary solution.
		double size = config.MostNarrow();
		if (size < 2)
			min_res = 0.01;	// Tiny part
		else if (size < 10.)
			min_res = 0.05;	// English (?)
		else if (size < 50.)
			min_res = 0.1;	// Big English, small Metric (?)
		else
			min_res = 1.;	// Metric (?)

		config.Resolution( min_res );

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// CRITICAL NOTE: Determination of the initial test resolution
		// is problematic. A poor choice can lead to less-than-optimal
		// placement of parts.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		// The initial testing resolution, as a function of the narrow part.
		//    test_res = (double)((int)(size*AREA_FACTOR*1000.0+0.5)) / 1000.;
		test_res = 10. * config.Resolution();

		// Find the resolution that is suitable for all parts.
		for (indx = 0; indx < count; ++indx)
		{
			npart = partbin.GetAt(indx);
			if ( npart->Error() )
				continue;

			reqd_res = npart->TestResolution( config, test_res, view );
			if (reqd_res < test_res)
				test_res = reqd_res;
		}

		// 2008.04.20 (PE) -- just in case (?)
		// ASSUMPTION: The resolution should be at least as fine
		// as our spacing distance because it affects testing of
		// the initial candidate part placement.
		if (test_res > config.Spacing())
			test_res = config.Spacing();

		// 2008.04.20 (PE) -- just in case (?)
		// And, of course, I encountered a case where the initial
		// part placement was 'wrongly' categorized as interfering.
		// We must still honor the minimum resolution, however,
		// because a resolution that is too small can lead to both
		// memory allocation failures and excessive computation times.
		test_res /= 2.;
		if (test_res < min_res)
			test_res = min_res;

		// TODO: Report any problems encountered while determining resolution.
		// This would require the part to have an attribute list. As such,
		// perhaps we should move the reporting into TestResolution() (?)

		config.Resolution( test_res );
	}

	EWMNesting( "...do_resolution() completed" );
	g_nesting_profiler.Out( "do_resolution" );

	return status;
}


// ----------------------------------------------------------------------------

CReturn CNestProcessApp::do_slice(
	CNestConfig& config,
	CPartBin& partbin,
	CViewMgr& view)
{
	CReturn status;
	CString note;
	CPath	path;
	double	delta;

	EWMFile( FILE_INFO );
	g_nesting_profiler.In( "do_slice" );

	for (int idx=0; idx<partbin.Count(); idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);

		if (npart->Error())
			continue;

		delta = 360. / npart->Count();
		for (int ang=0; ang<npart->Count(); ang++)
		{
			path.Set( npart->Filepath() );
			note.Format( "Slicing: %s at %d",
				(LPCSTR)path.FileName(), (int)(npart->StartAngle() + (ang * delta)) );
			CProgressWnd::SetText( note );

			if (npart->pModel(ang))
			{
				// **************************************
				status += npart->Slice( config, view, ang );
				// **************************************
				if (status.isError())
				{
					status.Internal( IDS_NEST_ERR_SLICE, partbin.GetAt(idx)->Filepath() );
					break;
				}
			}
		}
	} 

	g_nesting_profiler.Out( "do_slice" );

	return status;
}


// ----------------------------------------------------------------------------

CReturn CNestProcessApp::do_prenest(
	CNestConfig& config,
	CPartBin& partbin,
	CModel& model,
	CNestMgr& nest,
	CViewMgr& view)
{
	CReturn	status;

	EWMFile( FILE_INFO );
	EWMNesting( "Prenest( start )" );
	g_nesting_profiler.In( "do_prenest" );

	// Otherwise, the reported "Pending" value in the "Cancel" dialog is goofy.
	config.PartsToCut(0);

	int count = partbin.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CNestingPart* npart = partbin.GetAt(indx);
		if ( npart->PreNest() )
		{
			CSheet sheet;
			C2dBox extent;
			CString note;
			CPath path;
			double dx, dy, delta;
			int orientations, orient;
	
			sheet.ViewMgr( &view );

			config.Pass(npart->Pass());

			extent = npart->Extent();
			dx = extent.Dx();
			dy = extent.Dy();
			// ie. spacing | part | spacing | part | spacing
			delta = (2. * ((dx > dy) ? dx : dy)) + (3. * config.Spacing());

			// Fudge factor (really just for HardPierce() (?))
			delta += (2. * config.Spacing());

			extent.Update( 0., 0., delta, delta );

			orientations = npart->Count();
            delta = 360 / orientations;

			// For each rotation of this part...
			for (orient = 0; orient < orientations; ++orient)
			{
				EWMFile( FILE_INFO );

				sheet.PrepareBlank(config, extent, model);

				path.Set( npart->Filepath() );
				note.Format( "Prenesting: %s at %d",
					(LPCSTR)path.FileName(), (int)(npart->StartAngle() + (orientations * delta)) );
				CProgressWnd::SetText( note );

				EWMNesting( note );

				npart->PreHit( orient );
				config.GridFlags(GRID_FORCE);
				bool onesheet = config.OneSheet();
				config.OneSheet( TRUE );	// To allow pre-nesting of fillers

				status += nest.PreNest(config, &partbin, npart, &sheet, view);

				// Clear any temporary graphics.
				if ( config.DisplayParts() )
				{
					CViewBase* vbase = view.ActiveView();
					vbase->Clear( TEMP_LISTS );
					vbase->BufferShow( BACK_BUFFER );
				}

				config.OneSheet(onesheet);
			}
		}
	}

	config.GridFlags(config.GridMode());

	EWMNesting( "Prenest( end )" );
	g_nesting_profiler.Out( "do_prenest" );

	return status;
}


// ==================================================================
//	Nest:Panel:configdb="e:/work/bbi5/advmach/debug/database/cmdb.mdb", partdb="e:/work/bbi5/advmach/debug/database/PanelNest.pdb"
//
//	Note that the front-end form of Panel Nesting was taken from
//	True-Shape nesting.  It isn't necessarily a great fit, but was
//	easier than re-inventing from scratch.
//
//	Though there *are* some significant differences between the two
//	nesting forms, Panel Nesting is derived from True-Shape nesting,
//	and it uses a common nesting-support library with such entities as 
//	Sheet, PartBin, Part, Config, and Remnant classes.  It also adds
//	all of the Panel* classes plus SheetNode, for it's own unique
//	representation -- much like the Model and Geo dichotomy.
//
CReturn CNestProcessApp::NestPanel( CCommand* io_cmd )
{
	CReturn		ret;
#if NO_LONGER_OBSOLETE
	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	CString		configdb;
	CString		partdb;

	CSheet			sheet;
	CPanelNestMgr	nest;
	CPartBin		partbin;

	ret += io_cmd->getString( "configdb", &configdb );
	ret += io_cmd->getString( "partdb", &partdb );
	if (!ret.isOkay())
	{
		ret.Internal( IDS_NEST_PARAM_MISSING );
		return ret;
	}

//	view.HotDot( FALSE );
	CNestConfig& config = NestConfigGet();
	config.NestConfigInit();

	// ----------------------------------------------------
	//	Load parts and nesting information
	//
	config.GridNest( false );
	ret += config.Load( configdb, partdb );

	if (ret.isOkay())
		ret += sheet.Load( config );
	if (ret.isOkay())
		ret += partbin.Load( config, view );

	// ----------------------------------------------------
	// Prepare parts for nesting
	//
	config.Counter( 0 );
	for (int idx=0; idx<partbin.Count(); idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);

		if (npart->pModel())
		{
//			npart->calcSquareArea( config );
			config.Counter( config.Counter() + 1);
		}
	}

	config.Counter( 0 );
	for (idx=0; idx<partbin.Count(); idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);

		if (npart->pModel())
		{
			// Simplify part into basic outlines and minimal tooling
			CReturn part_ret;

			part_ret += npart->SimplifyRotation();

			npart->ReOrder(config);
			if (npart->pModel())
			{
				config.PassNum( max( config.PassNum(), npart->Pass() ) );
				config.Counter( config.Counter() + 1);
			}
		}
	}
	config.PassInit( config.PassNum() );

	// ----------------------------------------------------
	// Perform actual nesting
	//
	if (ret.isOkay())
		ret += nest.Execute( io_cmd, config, &sheet, &partbin, view );

	view.Clear();
	view.Regenerate( model, FALSE );
	view.Refresh();

//	view.HotDot( TRUE );
#endif
	return ret;
}



// ==================================================================
//		Nest:PSeq_Init
//
//	Initialize the part-sequence iterator (including putting up info
//	for the first part in the sequence).
//
//	Scan the model in memory, which SHOULD be a nested part, and
//	extract all of the part label user commands from it.
//	These are then sorted into x-major zig-zag order.
//
//	Note that VB can get the value from the command as "partnum".  
//
//	"partnum"	This will be -1 when done, otherwise a valid part index
//	"part_x"	X ordinate
//	"part_y"	Y ordinate
//
static CDbEntityList* g_pseq_list = NULL;
static int g_pseq_idx = -1;

CReturn
CNestProcessApp::PartSeqInit( CCommand* io_cmd )
{
	CReturn		ret;

	CModel&		model = io_cmd->getModel();

	const CVarList&	var = model.Header();
	CViewMgr& view = io_cmd->getViewMgr();

	double sheet_length = var.getReal( STR_LENGTH, 0.0 );
	double sheet_width = var.getReal( STR_WIDTH, 0.0 );
	double diam = max(sheet_length, sheet_width);

	int zignum = 2;
	io_cmd->getInt( "zig", &zignum );
	//
	//
	CPathFinder path(PATH_ZIG_X);
	path.NodeZigNum( zignum );
	//
	// First get all of the user commands and add any that are valid
	// part labels to the path
	//
	CSelector select(model);
	select.All( 0 );
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( TRUE );
	select.SystemFlag( FALSE );
	select.SelectAll( TRUE );

	int idx, num = select.Count();
	if (!num)
	{ return ret; }

	for (idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);
		CString		text = db_cmd->Text();

		C3dCoord& pt = db_cmd->Coord(0);
		if (db_cmd->Coord(0).X() > (sheet_length+SMALL))
		{ continue; }

		if (text.GetLength() > 4)
		{ continue; }

		int tag = atoi(text);
		if (!tag)
		{ continue; }

		C2dCoord city( pt.X(), pt.Y() );
		path.CityAppend( city, db_cmd );
	}
	//
	// Now that we have our tags, we need to create the zig-zag path
	// through them.
	//
	path.CityPrepare();
	path.NodePrepare( min(10, path.CityCount() ) );

	path.Evaluate();
//	path.Evaluate( view.GetCanvas() );
//	view.KillCanvas();
	//
	// Now that we have the path, we need to move it to our static
	// list so we can spoon-feed it to VB
	//
	if (g_pseq_list != NULL)
	{
		g_pseq_list->BenignFlush();
		delete g_pseq_list;
	}
	g_pseq_list = new CDbEntityList();
	g_pseq_idx = -1;

	num = path.NodeCount();
	for (idx=0; idx<num; idx++)
	{
		g_pseq_list->Append( (CDbEntity*)path.NodeRef( idx ) );
	}

	return PartSeqNext( io_cmd );
}

// ==================================================================
//		Nest:PSeq_Next
//
//	Step up to the next part in the sequence.
//
CReturn
CNestProcessApp::PartSeqNext( CCommand* io_cmd )
{
	CReturn ret;

//CString note;
//note.Format( "PartSeqNext cmd %lx", io_cmd );
//MessageBox( NULL, note, NULL, MB_OK );
	io_cmd->setInt( "partnum", -1 );

	if (g_pseq_list == NULL)
	{ 
//note.Format( "... null seq list, DONE");
//MessageBox( NULL, note, NULL, MB_OK );
		return ret;
	}

	int num = g_pseq_list->Count();
	g_pseq_idx++;
//note.Format( "... %d out of %d", g_pseq_idx, num);
//MessageBox( NULL, note, NULL, MB_OK );
	if (g_pseq_idx < num)
	{
		CDbCommand* cmd = dynamic_cast<CDbCommand*>((*g_pseq_list)[g_pseq_idx]);
//note.Format( "... user cmd %lx", cmd );
//MessageBox( NULL, note, NULL, MB_OK );
		if (cmd != NULL)
		{
//note.Format( "... set values");
//MessageBox( NULL, note, NULL, MB_OK );

			io_cmd->setInt( "partnum", atoi(cmd->Text())-1 );
			io_cmd->setInt( "partfeat", -1 );
			io_cmd->setReal( "part_x", cmd->Coord(0).X() );
			io_cmd->setReal( "part_y", cmd->Coord(0).Y() );

			CDbContainer* owner = dynamic_cast<CDbContainer*>(cmd->Owner());
//note.Format( "... owner %lx", owner);
//MessageBox( NULL, note, NULL, MB_OK );
			while (owner != NULL)
			{ 
				CString type = owner->StringGet( STR_TYPE, "<error8>" );
				if (type.CompareNoCase( "_part" ) == 0)
				{
//note.Format( "... part feature %d", owner->Id() );
//MessageBox( NULL, note, NULL, MB_OK );
					io_cmd->setInt( "partfeat", owner->Id() );
					break;
				}
				owner = dynamic_cast<CDbContainer*>(owner->Owner());
			}
			// Okay didn't find an owner, so default to the immediate owner...
			//
			if (owner == NULL)
			{
				owner = dynamic_cast<CDbContainer*>(cmd->Owner());
				if (owner)
				{
					io_cmd->setInt( "partfeat", owner->Id() );
				}
			}
		}
	}
//note.Format( "... DONE");
//MessageBox( NULL, note, NULL, MB_OK );

	return ret;
}


// ==================================================================
//		ManualSheet
//
//	Nest:Manual:Sheet:
//
//	Prepare the model in memory for manual nesting -- setting up the
//	sheet.  CALL THIS FIRST to init manual nesting.
//
//	Note that manual nesting requires multiple calls across time, so
//	we set up in a static structure.
//
//	We ASSUME that the model is either empty, or contains Pattern/Instance
//	data.  We only recognize the instances of patterns in manual
//	nesting.
//
CReturn
CNestProcessApp::ManualSheet( CCommand* io_cmd )
{
	CReturn ret;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	CString configdb = "";
	io_cmd->getString( "configdb", &configdb );

	m_mandata.InitSheet( configdb, &model, view  );

	return ret;
}

// ==================================================================
//		ManualPattern
//
//	Nest:Manual:Pattern:id=%d, x=%f, y=%f
//
//	Take an existing pattern and place it on the sheet at the specified
//	location.  From here, we can do nest movements on it.
//
CReturn
CNestProcessApp::ManualPattern( CCommand* io_cmd )
{
	CReturn ret;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	ID	pat_id;
	ret += io_cmd->getInt( "id", (int*)&pat_id );

	double x, y;
	ret += io_cmd->getReal( "x", &x );
	ret += io_cmd->getReal( "y", &y );
	C3dCoord place( x, y, 0.0 );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_NEST_PARAM_MISSING );
		return ret;
	}
	//
	// Ummm, move from local to world?  Because things punch
	// in assuming world coordinates.
	//
	CDbWorkplane* dbWork=NULL;// = m_model->ActiveWorkplane();
	model.EntityFind( STR_TOP, (CDbEntity**)&dbWork, DBWORKPLANE, DBWORKPLANE );
	dbWork->Transform().Transform( &place );

	int id = m_mandata.UsePattern( pat_id, place, view  );
	io_cmd->setInt( "id", id );

	return ret;
}


// ==================================================================
//		ManualInstance
//
//	Nest:Manual:Instance:id=%d
//
//	Take an existing instance and prep it for nest movements.
//
// To initialize the drag/drop process (ie. get poly geometry)
//		Nest:Manual:Instance: id=%d
//
// To initialize the drag/drop of holddown
//		Nest:Manual:Instance: hold=%b
//
// To drag an instance
//		Nest:Manual:Instance: id=%d, opt=%d, ....
//
// To move an instance
//		Nest:Manual:Instance: id=%d, opt=BUMP_MOVE, xp=%f, yp=%f, ang=%f
//
// To copy an instance
//		Nest:Manual:Instance: id=%d, opt=BUMP_COPY, xp=%f, yp=%f, ang=%f
//
// where ang is in degrees
//

// TODO: Make these data members of CNestProcessApp (?)
CDbCommand*	m_instance = NULL;
CDbPattern*	m_pattern = NULL;
CDbFeature* m_zone = NULL;

CReturn
CNestProcessApp::ManualInstance( CCommand* io_cmd )
{
	CReturn status;

	// "opt" represents the requested action.
	EBumpOption opt = (EBumpOption) io_cmd->VarList().getInt( "opt", -1 );

	switch (opt)
	{
	case BUMP_INIT:
		if (io_cmd->VarList().getInt( "id", 0 ) > 0)
			status = BumpInit( io_cmd );
		else if (io_cmd->VarList().getInt( "zone_id", 0 ) > 0)
			status = HoldDownInit( io_cmd );
		break;

	case BUMP_ANCHOR:
		status = BumpAnchor( io_cmd );
		break;

	case BUMP_ORIENT:
		if (m_pattern != NULL)
			status = BumpOrient( io_cmd );
		break;

	case BUMP_DRAG:
		status = BumpDrag( io_cmd );
		break;

	case BUMP_MOVE:
	case BUMP_COPY:
		if (m_pattern != NULL)
			status = BumpMoveCopy( io_cmd );
		break;

	case BUMP_CANCEL:
		status = BumpCancel( io_cmd );
		break;

	default:
		status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::ManualInstance(#1)" );
		break;
	}

	if ( !status.IsOk() )
	{
		io_cmd->setInt( "id", 0 );
		io_cmd->setInt( "part_id", 0 );
		io_cmd->setInt( "pattern_id", 0 );
	}

	return status;
}


// ==================================================================
//		ManualSelection
//
//	Nest:Manual:Selection:x=%f, y=%f
//
//	Take existing non-pattern geometry, turn it into a pattern, and
//	set it up for nesting.
//
CReturn
CNestProcessApp::ManualSelection( CCommand* io_cmd )
{
	CReturn ret;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	double x, y;
	ret += io_cmd->getReal( "x", &x );
	ret += io_cmd->getReal( "y", &y );
	C3dCoord place( x, y, 0.0 );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_NEST_PARAM_MISSING );
		return ret;
	}
	//
	// Ummm, move from local to world?  Because things punch
	// in assuming world coordinates.
	//
	CDbWorkplane* dbWork=NULL;// = m_model->ActiveWorkplane();
	model.EntityFind( STR_TOP, (CDbEntity**)&dbWork, DBWORKPLANE, DBWORKPLANE );
	dbWork->Transform().Transform( &place );

	int id = m_mandata.UseSelection( place, view  );
	io_cmd->setInt( "id", id );

	return ret;
}

// ==================================================================
//		ManualFile
//
//	Nest:Manual:File:file=%s, x=%f, y=%f
//
//	Take existing file, turn it into a pattern, and
//	set it up for nesting.
//
CReturn
CNestProcessApp::ManualFile( CCommand* io_cmd )
{
	CReturn ret;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	double x, y;
	ret += io_cmd->getReal( "x", &x );
	ret += io_cmd->getReal( "y", &y );
	C3dCoord place( x, y, 0.0 );

	CString filepath;
	ret += io_cmd->getString( "file", &filepath );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_NEST_PARAM_MISSING );
		return ret;
	}
	//
	// Ummm, move from local to world?  Because things punch
	// in assuming world coordinates.
	//
	CDbWorkplane* dbWork=NULL;// = m_model->ActiveWorkplane();
	model.EntityFind( STR_TOP, (CDbEntity**)&dbWork, DBWORKPLANE, DBWORKPLANE );
	dbWork->Transform().Transform( &place );

	int id = m_mandata.UseFile( filepath, place, view  );
	io_cmd->setInt( "id", id );

	return ret;
}


// ==================================================================
//		ManualJumpTo
//
//	Nest:Manual:JumpTo:x=%f, y=%f
//
CReturn
CNestProcessApp::ManualJumpTo( CCommand* io_cmd )
{
	CReturn ret;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	double x;
	double y;
	ret += io_cmd->getReal( "x", &x );
	ret += io_cmd->getReal( "y", &y );
	C3dCoord pos( x, y, 0.0 );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_NEST_PARAM_MISSING );
		return ret;
	}

	C3dCoord origin = m_mandata.Position();
	m_mandata.Position( pos );

	C3x4Matrix shift;
	shift.Shift( C3dVec( pos.X() - origin.X(), pos.Y() - origin.Y(), 0.0 ) );

	m_mandata.Instance()->Transform( shift );

	return ret;
}


// ==================================================================
//		ManualBump
//
//	Nest:Manual:Bump:[dx=%f, dy=%f]|[x=%f, y=%f]
//
CReturn
CNestProcessApp::ManualBump( CCommand* io_cmd )
{
	CReturn ret;

	double	x, y, dx, dy;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	dx = io_cmd->VarList().getReal( "dx", 0. );
	dy = io_cmd->VarList().getReal( "dy", 0. );
		x = io_cmd->VarList().getReal( "x", 0. );
		y = io_cmd->VarList().getReal( "y", 0. );

	C3dCoord last_pos = m_mandata.Position();
	if ((last_pos.X() >= UNDEFINED) || (last_pos.Y() >= UNDEFINED))
		m_mandata.Position( C3dCoord( x, y, 0. ) );

	if (ZERO(dx) && ZERO(dy))
	{

		last_pos = m_mandata.Position();
		dx = x - last_pos.X();
		dy = y - last_pos.Y();
	}

	C3dVec delta( dx, dy, 0.0 );

	m_mandata.Bump( delta, view );

//	view.Clear();
//	view.Regenerate( model, FALSE );
	view.Refresh( true );

	return ret;
}


// ==================================================================
//		ManualPunch
//
//	Nest:Manual:Punch:
//
CReturn
CNestProcessApp::ManualPunch( CCommand* io_cmd )
{
	CReturn ret;

	CModel&		model = io_cmd->getModel();
	CViewMgr&	view = io_cmd->getViewMgr();

	m_mandata.Punch( view );

	view.Clear();
	view.ModelSet( model );
	view.Refresh( true );

	return ret;
}





// ==================================================================
CReturn
CNestProcessApp::Generate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	CModel&	model = io_cmd->getModel();
	CViewMgr& view = io_cmd->getViewMgr();

	CSelectorStack& selectorStack = model.SelectorStack();

	// For each Pattern, process cuts and cut into sheet at instances
	CDbIterator iter;
	iter.Init( model.Db(), DBPATTERN );
	while ( TRUE )
	{
		CDbPattern* db_pattern = dynamic_cast<CDbPattern*>( iter() );
		if (!db_pattern)
			break;
		iter.Next();

		// Create new part to receive the selection
		CModel* nest_model = new CModel;
		nest_model->Init( dummy_stack, dummy_tree );
		nest_model->UndoBufferSuppress();

		CNestingPart part;
		part.Model( nest_model );
		part.Rotation(1);

		CNestConfig& config = NestConfigGet();
		config.NestConfigInit();
		config.PassInit(1);
		config.Pass(0);
		config.Resolution( 0.1 );

		// Move pattern into the part
		model.EntityPrepareCopy( nest_model );
		CDbEntity* new_ent;

		CDbIterator tooliter;
		tooliter.Init( model.Db(), DBTOOL );
		while ( TRUE )
		{
			CDbTool* db_tool = dynamic_cast<CDbTool*>( tooliter() );
			if (!db_tool)
				break;
			tooliter.Next();
			model.EntityCopy( (CDbEntity&)*db_tool, &new_ent );
		}

		CDbEntity* db_ent = NULL;
		int num = db_pattern->Count();
		for (int idx = 0; idx<num; idx++)
		{
			db_ent = (*db_pattern)[idx];
			if (db_ent->IsDeleted())
			{ continue; }

			model.EntityCopy( *db_ent, &new_ent );

			if (new_ent->Owner() == NULL
				&& (new_ent->Type() != DBFEATURE) )
			{
				CDbFeature* owner = NULL;
				nest_model->EntityCreate( DBFEATURE, (CDbEntity**)&owner );
				owner->Append( new_ent );
			}
		}

		// Now process the part
		part.Simplify( config );
		part.Rotate();
		part.Lead( config );

		part.Generate( 0 );
		part.Assemble( config, 0 );

		// Now, finally, explode at each instance point
		CDbIterator institer;
		institer.Init( model.Db(), DBCOMMAND );
		while ( TRUE )
		{
			CDbCommand* db_command = dynamic_cast<CDbCommand*>( institer() );
			if (!db_command)
				break;
			institer.Next();

			if ( db_command->IsInstance() )
			{
				ID id = (ID) db_command->IntGet( "patid", 0 );

				if (id == db_pattern->Id())
				{
					CToolHit* toolhit = part.ToolHit(0);
				}
			}
		}
	}
	
	return ret;
}



// ============================================================================
//	Nest:Chain: chain=%d, offset=%f
//
CReturn
CNestProcessApp::Chain( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;
	CModel&	model = io_cmd->getModel();

	int chain = 2;
	io_cmd->getInt( "chain", &chain );

	double offset = 0.25;
	io_cmd->getReal( "offset", &offset );

	return CNestMgr::Chain(model, chain, offset);
}


// ==================================================================
//		Nest:Test
//
//	Testing
//
//	nest:test:file="c:\work\wittlock\advmach\_bug\2003-11-06 Jonathon Overlap\FFS833.mm2", space=0.0625, gap=0.01
//	nest:test:file="c:\work\wittlock\advmach\_bug\2003-11-12 Jonathon Nest\Jack.mm2", space=0.5, gap=0.03
//	nest:test:file="c:\work\wittlock\advmach\_bug\2004-01-13 Jonathon Nest\50040935-90.mm2", space=0.25, gap=0.04
//	nest:test:file="c:\work\wittlock\advmach\_bug\2004-01-14 Jonathon McNest\FAT007.mm2", space=0.5, gap=0.04
//
#include "NestingPart.h"
#include "DbLine.h"
#include "DbArc.h"

#include "Profiler.h"

// Nest:Test: file=%s [, space=%f, gap=%f, pre_rotate=%f]
CReturn CNestProcessApp::Test( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	ret;
	CMM2	mm2;

	CString file = io_cmd->VarList().getString( "file", "" );
	double space = io_cmd->VarList().getReal( "space", 0. );
	double tab_gap = io_cmd->VarList().getReal( "gap", 0.1 );
	double pre_rotate = io_cmd->VarList().getReal( "pre_rotate", 0. );
	bool assemble = (io_cmd->VarList().getInt( "assemble", 1 ) != 0);

	// NOTE: The 'part' takes ownership of this part_model.
	CModel* part_model = new CModel();
	part_model->Init( dummy_stack, dummy_tree );

#if (_CI || _NST)
	CCiModelConvertor	convertor;
	CCiModel*	ciModel = CPortal::CiModel();
	int	tool_setup_id = io_cmd->VarList().getInt( "tool_setup_id", 0 );

	ret = ciModel->FileRead( file, SMALL );
	if (ret.IsOk())
		ret = convertor.ConvertToMM2( ciModel, part_model, tool_setup_id, 0 );
#else
	ret += mm2.Read( file, part_model, MM2_STD_MODE );
#endif

	if ( ret.isOkay() )
	{
		CNestingPart part;

		part_model->UndoBufferSuppress();

		part.Filepath( file );
		part.Model( part_model );
		part.Rotation(1);

		CNestConfig& config = NestConfigGet();
		config.NestConfigInit();
		config.PassInit(1);
		config.Pass(0);
		config.Resolution( 0.1 );
		config.GapTolerance( tab_gap );
		config.Spacing( space );

		part.Simplify( config );

		part.StartAngle( pre_rotate );
		part.Rotate();
		part.Lead( config );

		for (int idx = 0; idx < part.Count(); idx++)
		{
			part.Generate( idx );

			if (assemble)
			{
				part.Assemble( config, idx );

				if ( !ZERO(config.Spacing()) )
				{
					ret += part.AddSpace( config, idx, HIT_NEST_OUTSIDE );
					ret += part.AddSpace( config, idx, HIT_NEST_INSIDE );
				}
				part.Accessorize( config, idx );
			}
		}

		io_cmd->getModel().UndoBufferSuppress();
		if (assemble)
			part.DebugToModel( &io_cmd->getViewMgr(), &io_cmd->getModel() );
		else
			part.DebugToModel2( &io_cmd->getViewMgr(), &io_cmd->getModel() );
		io_cmd->getModel().UndoBufferActivate();

#if (_CI)
		if (0)
			ret = mm2.Write( "c:\\_fab\\debug\\debug\\nest_test.mm2", io_cmd->getModel() );

		// Argh. For this to work: when in the debugger, you must step
		// into ConvertToCI() and then step over m_ciModel->Init().
		ret = convertor.ConvertToCI( &io_cmd->getModel(), ciModel, false );
#endif
	}

#if (_CI)
	delete part_model;
#endif

	return ret;
}

// All patterns representing a particular part should have a unique
// orientation (at least, this is how nesting works).
//
// When the orientation of the part within a pattern is changed,
// all instances representing the pattern should reflect the change.
//
// When simply translating an instance, the translation is applied
// directly to the instance.
//
// When rotating an instance (using the Move option) and that
// instance is the only instance representing its pattern, the
// rotation should be applied to the patterns part. However, if the
// orientation of the instance is *not* unique then the instance
// should be updated to represent the (existing) pattern having the
// matching orientation. Note, this last condition can lead to orphaned
// patterns (patterns unreferenced by any instances).
//
// When an instance is rotated (using the Move or Copy option) and
// there are other instances representing the same pattern, and the
// orientation of the instance is unique, then a new pattern should
// be created and the instance should be updated to reference the
// new pattern. However, if the orientation of the instance is *not*
// unique then the instance should be updated to represent the
// (existing) pattern having the matching orientation.
//
CReturn
CNestProcessApp::MoveCopy(
					CDbPattern*	dbPattern,
					CDbCommand*	dbInstance,
					CCommand*	io_cmd )
{
	CReturn		status;
	CString		tmp;
	CString		label;
	C3x4Matrix	xform;
	C3dVec		vec;
	C3dCoord	pt;
	CDbPattern*	cousin;
	CDbEntity*	dbCopy;
	double		radians;
	double		initial_orientation;
	bool		rotate;
	bool		translate;
	bool		unique_instance;
	bool		copy_pattern;
	bool		copy_instance;

	CModel&	model = io_cmd->getModel();

	double xp = io_cmd->VarList().getReal( "xp", 0. );
	double yp = io_cmd->VarList().getReal( "yp", 0. );
	double new_orientation = io_cmd->VarList().getReal( "ang", 0. );
	
	EBumpOption opt = (EBumpOption) io_cmd->VarList().getInt( "opt", -1 );

	// This is done piecewise only to support debugging.
	new_orientation = ((int)(new_orientation * 10.) / 10.);
	new_orientation *= DEG2RAD;

	initial_orientation = dbPattern->DoubleGet( "part_angle", 0. );
	radians = new_orientation - initial_orientation;

	rotate = (fabs(radians) >= (0.1 * DEG2RAD));

	if (radians < 0.)
		radians += TWOPI;
	else if (radians >= TWOPI)
		radians -= TWOPI;

	pt = dbInstance->Coord();
	vec.Init( (xp - pt.X()), (yp - pt.Y()), 0. );

	// translate = ((fabs(vec.X() > SMALL) || (fabs(vec.Y()) > SMALL)));  // TYPO ????
	translate = ((fabs(vec.X()) > SMALL) || (fabs(vec.Y()) > SMALL));

	if ( !translate && !rotate )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::MoveCopy(#1)" );
	}
	else
	{
		if (rotate)
		{
			if (opt == BUMP_MOVE)
			{
				unique_instance = (InstanceCount( dbPattern ) == 1);
				copy_pattern = ( !unique_instance );
				copy_instance = false;
			}
			else
			{
				copy_pattern = true;
				copy_instance = true;
			}

			// if (copy_pattern)
			{
				// Regardless of copying or moving, if there is an
				// existing pattern having this orientation, then it
				// is that pattern that should be referenced.
				cousin = SimilarPatternFind( dbPattern, new_orientation );
				if (cousin != NULL)
				{
					dbPattern = cousin;
					copy_pattern = false;
					rotate = false;
				}
			}
		}
		else
		{
			copy_pattern = false;
			copy_instance = (opt == BUMP_COPY);
		}

		if (copy_pattern)
		{
			model.EntityPrepareCopy( &model );
			model.EntityCopy( (*dbPattern), &dbCopy );
			dbPattern = dynamic_cast<CDbPattern*>( dbCopy );

			if (dbPattern == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::MoveCopy(#2)" );
			}
			else
			{
				// 2012.09.18 (PE) -- Count of part instances in the
				// part table of the database was not correct.
				int part_id = dbPattern->IntGet( "part_id", 0 );

				// Create a unique(?) label.
				label = dbPattern->StringGet( "_label", "" );
				tmp.Format( "%s-%d", (LPCSTR)label, dbPattern->Id() );
				dbPattern->StringSet( "_label", tmp );

				dbPattern->IntSet( "part_id", part_id );
			}
		}

		if ( status.IsOk() )
		{
			if (rotate)
			{
				// Whether creating a new copy or reorienting a unique instance
				// we must establish the new orientation.
				dbPattern->DoubleSet( "part_angle", new_orientation );
			}

			if (copy_instance)
			{
				model.EntityPrepareCopy( &model );
				model.EntityCopy( (*dbInstance), &dbCopy );
				dbInstance = dynamic_cast<CDbCommand*>( dbCopy );

				if (dbInstance == NULL)
				{
					status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::MoveCopy(#3)" );
				}
			}

			if ( status.IsOk() )
			{
				// NOTE: Always updating the reference does not cost much.
				// Otherwise, we should change the implementation so that
				// it updates the reference only when the relationship
				// between a pattern and an instance has changed.
				//
				// if (copy_pattern || copy_instance)
				// if (update_reference)
				{
					// Set the reference backto the associated pattern.
					dbInstance->IntSet( "patid", dbPattern->Id() );
					tmp.Format( "@INSTANCE: instang=%f, patid=%d", 0., dbPattern->Id() );
					dbInstance->Text( tmp );
				}

				// Orient the patterns part as necessary.
				if (rotate)
				{
					// 2008.09.28 (PE) -- Failure to establish a new action
					// results in failure to transform the geometry underlying
					// the pattern. Prior to introducing this statement, from
					// the end-user's perspective, things appeared to be working
					// ... until they selected the Drop button.
					CDbEntity::NewAction();

					xform.setXYAngle( radians );
					dbPattern->Transform( xform );

					pt.X( dbPattern->DoubleGet( "_label_dx", 0. ) );
					pt.Y( dbPattern->DoubleGet( "_label_dy", 0. ) );
					xform.Transform( &pt );
					dbPattern->DoubleSet( "_label_dx", pt.X() );
					dbPattern->DoubleSet( "_label_dy", pt.Y() );

					xform.setUnit();
				}

				// Translate the instance as necessary.
				if (translate)
				{
					xform.Shift( vec );
					dbInstance->Transform( xform );
				}
			}
		}
	}

	return status;
}

int
CNestProcessApp::InstanceCount( CDbPattern* dbPattern )
{
	CDbIterator	iter;
	CDbCommand*	dbCommand;
	ID			part_id;
	int			count;

	count = 0;

	iter.Init( (*dbPattern->Db()), DBCOMMAND, true );
	while (1)
	{
		dbCommand = dynamic_cast<CDbCommand*>( iter() );
		if (dbCommand == NULL)
			break;

		if ( dbCommand->IsInstance() )
		{
			part_id = dbCommand->IntGet( "patid", 0 );
			if (part_id == dbPattern->Id())
				++count;
		}

		iter.Next();
	}

	return count;
}

CDbPattern*
CNestProcessApp::SimilarPatternFind(
							CDbPattern*	dbPattern,
							double		radians )
{
	CDbIterator	iter;
	CDbPattern*	candidate;
	double		temp_part_angle;
	int			this_part_id;
	int			temp_part_id;

	// Assigned by nesting when part is loaded from database (?)
	this_part_id = dbPattern->IntGet( "part_id", 0 );

	iter.Init( (*dbPattern->Db()), DBPATTERN );
	while (1)
	{
		candidate = dynamic_cast<CDbPattern*>( iter() );
		if (candidate == NULL)
			break;

		if (candidate != dbPattern)
		{
			temp_part_id = candidate->IntGet( "part_id", 0 );
			if (temp_part_id == this_part_id)
			{
				temp_part_angle = candidate->DoubleGet( "part_angle", 0. );
				if (fabs(temp_part_angle - radians) <= DEG2RAD)
					break;
			}
		}

		iter.Next();
	}

	return candidate;
}

void
CNestProcessApp::PolyDataGenerate( CDbEntityArray& entities, CCommand* io_cmd )
{
	CGeoCurveArray	geoCurves;
	CDbCurve*		dbCurve;
	int				count, indx;

	count = entities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbCurve = dynamic_cast<CDbCurve*>( entities.GetAt(indx) );
		if (dbCurve != NULL)
		{
			geoCurves.Append( dbCurve->Curve() );
		}
	}

	PolyDataGenerate( geoCurves, io_cmd );

	geoCurves.DestructiveFlush();
}

void
CNestProcessApp::PointsMap(
	const CWnd&		wnd,
	const C2dBox&	box,
	C3dCoordArray*	pts )
{
	CRect rect;
	wnd.GetClientRect( &rect );

	// We change the rectangle dimensions to allow
	// for a border around the displayed part.
	double BORDER = 4;
	double wx = (double) (rect.Width() - BORDER);
	double wy = (double) (rect.Height() - BORDER);

	double dx = box.Dx();
	double dy = box.Dy();

	double xratio = wx / dx;
	double yratio = wy / dy;
	double scale = ((xratio < yratio) ? xratio : yratio);

	int wcx = rect.Width() / 2;
	int wcy = rect.Height() / 2;

	double pcx = box.Xmin() + (0.5 * dx);
	double pcy = box.Ymin() + (0.5 * dy);

	int count = pts->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		C3dCoord* pt = pts->GetAt( indx );

		double xp = pt->X();
		double yp = pt->Y();

		xp = wcx + (xp - pcx) * scale;
		yp = wcy - ((yp - pcy) * scale);

		pt->X( xp + 1 );  // kludge to shift image
		pt->Y( yp );
	}
}

void
CNestProcessApp::ShapeDraw(
	CDC*					dc,
	const C3dCoordArray&	pts )
{
	CPen pen;
	if (pen.CreatePen( PS_SOLID, 1, DCOLOR_RED ) != 0)
	{
		CPen* prev = dc->SelectObject( &pen );

		int count = pts.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			const C3dCoord& pt = *(pts.GetAt(indx));
			if (pt.Z() >= UNDEFINED)
				dc->MoveTo( (int) pt.X(), (int) pt.Y() );
			else
				dc->LineTo( (int) pt.X(), (int) pt.Y() );
		}

		dc->SelectObject( prev );
	}
}

void
CNestProcessApp::PolyDataGenerate( CGeoCurveArray& geoCurves, CCommand* io_cmd )
{
	C2dBox box;
	C3dCoordArray pts;

	CurvesTabulate( geoCurves, &pts, &box );
	if (pts.Count() > 0)
	{
		int hwnd = io_cmd->VarList().getInt( "hwnd", 0 );
		if (hwnd > 0)
		{
			CWnd wnd;
			if ( wnd.Attach( (HWND) hwnd ) )
			{
				CDC* dc = wnd.GetDC();
				if (dc != NULL)
				{
					PointsMap( wnd, box, &pts );
					ShapeDraw( dc, pts );

					wnd.ReleaseDC( dc );
				}
			}

			wnd.Detach();  // otherwise boom! when we return to VB
		}

		pts.DestructiveFlush();
	}

	io_cmd->setInt( "count", 0 );
}

void
CNestProcessApp::CurvesTabulate(
	const CGeoCurveArray&	geoCurves,
	C3dCoordArray*			pts,
	C2dBox*					box )
{
	int count = geoCurves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* geoCurve = geoCurves.GetAt( indx );

		if (geoCurve->Type() == GEOARC)
		{
			CGeoArc* geoArc = dynamic_cast<CGeoArc*>( geoCurve );
			GetArcPts( *geoArc, pts, box );
		}
		else
		{
			CGeoLine* geoLine = dynamic_cast<CGeoLine*>( geoCurve );
			GetLinePts( *geoLine, pts, box );
		}
	}
}

void
CNestProcessApp::GetLinePts(
	const CGeoLine&	geoLine,
	C3dCoordArray*	pts,
	C2dBox*			box )
{
	int count = pts->Count();

	// The terminal point in the list;
	C3dCoord pe( UNDEFINED, UNDEFINED, UNDEFINED );
	if (count > 0)
		pe = *(pts->GetAt( count - 1 ));

	C3dCoord pt = geoLine.StartPt();
	if ( !pe.WithinTolXY( pt, SMALL ) )
	{
		pt.Z( UNDEFINED );  // this translates into a MoveTo()
		pts->Append( new C3dCoord( pt ) );
		(*box) += pt;
	}

	pt = geoLine.EndPt();
	pt.Z( 0. );
	pts->Append( new C3dCoord( pt ) );
	(*box) += pt;
}

void
CNestProcessApp::GetArcPts(
	const CGeoArc&	geoArc,
	C3dCoordArray*	pts,
	C2dBox*			box )
{
	int count = pts->Count();

	// The terminal point in the list;
	C3dCoord pe( UNDEFINED, UNDEFINED, UNDEFINED );
	if (count > 0)
		pe = *(pts->GetAt( count - 1 ));

	double as, ae;
	geoArc.Angles( &as, &ae );
	
	double delta = ae - as;

	// Arbitrary angular increment. NOTE: We may need to
	// base this upon some calculated chordal tolerance.
	double da = 10. * DEG2RAD;

	// The count of angular increments.
	count = (int) fabs( delta / da );
	if (count < 1)
		count = 1;

	// Normalize the angular increment.
	da = delta / count;


	const C3dCoord& pc = geoArc.CenterPt();

	C3dCoord pt;
	double radius = geoArc.Radius();
	for (int indx = 0; indx <= count; ++indx)
	{
		double ang = as + (indx * da); 

		pt.X( pc.X() + (radius * cos(ang)) );
		pt.Y( pc.Y() + (radius * sin(ang)) );
		pt.Z( 0. );

		if ((indx == 0) && !pt.WithinTolXY( pe, SMALL ))
			pt.Z( UNDEFINED );  // this translates into a MoveTo()

		pts->Append( new C3dCoord( pt ) );
		(*box) += pt;
	}
}

CReturn
CNestProcessApp::HoldDownInit( CCommand* io_cmd )
{
	const double EXPLODE_TOL = 1.e-3;  // arbitrary
	const double AT_RADIANS = 0.;

	CReturn status;

	CModel& model = io_cmd->getModel();

	CDbFeature* dbFeature = NULL;

	int id = io_cmd->VarList().getInt( "zone_id", 0 );
	if (id > 0)
		model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );

	if ((dbFeature != NULL) && dbFeature->IsWorkZone())
	{
		C3x4Matrix xform;  // defaults to unit
		C3dCoord delta;

		CViewMgr& view = io_cmd->getViewMgr();
		CViewBase* vbase = view.ActiveView();

		ShiftInit( io_cmd );

		m_zone = dbFeature;

		double x_hold = m_zone->DoubleGet( "_hold_x", 0. );
		double y_hold = m_zone->DoubleGet( "_hold_y", 0. );

		m_handle.X( x_hold );
		m_handle.Y( y_hold );
		m_anchor = m_handle;

		m_radians = 0;
		m_delta_radians = 0.;

		xform.setT( 0., -m_shift, 0. );

		CDisplayEntity dispent;
		CDbEntity::draw_2d( CDbFeature::HoldDownShape(),
			dispent.CommandList(), dispent.WorldList(),
			xform, EXPLODE_TOL, ORIGIN, AT_RADIANS, false );

		vbase->DisplayListBegin( STEMP_LIST );
		vbase->SemiTempDelta( ORIGIN );  // Is this necessary?
		vbase->DrawDirect( &dispent );  // 'dispent' treated as const (?)
		vbase->DisplayListEnd( STEMP_LIST );

		delta.XYZ( m_handle.X(), (m_handle.Y() - m_shift), 0. );
		vbase->SemiTempDelta( delta );
		vbase->BufferShow( FRONT_BUFFER );

		// So that we can echo the handle point location.
		io_cmd->setReal( "xp", m_handle.X() );
		io_cmd->setReal( "yp", m_handle.Y() );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::HoldDownInit()" );
	}

	return status;
}

// Cloned (sort of) from CCreateProcessApp::PatternInstance()
CDbCommand*
CNestProcessApp::InstanceCreate(
	const CDbPattern&	dbPattern,
	double				ang,
	double				xp,
	double				yp )
{
	CEntityDb*		db;
	CDbTool*		dbTool;
	CDbWorkplane*	dbWork;
	CDbCommand*		dbInstance;

	// Kinda icky ... but creates in the correct database.
	// Everywhere else we tend to us model.EntityCreate().
	db = ((CDbPattern&) dbPattern).Db();
	db->Create( DBCOMMAND, (CDbEntity**) &dbInstance );

	db->Find( "Instances", (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	// 2005.12.17 (PE) -- Oooops, pattern does not have a workplane?
	//   dbWork = dbPattern.Workplane();
	db->Find( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	if (dbInstance != NULL)
	{
		CString inst;
		inst.Format( "@INSTANCE: instang=%f, patid=%d", ang, dbPattern.Id() );

		dbInstance->Init( dbTool, dbWork, C3dCoord( xp, yp, 0.0 ), inst );
		dbInstance->SystemFlag( false );

		dbInstance->DoubleSet( "angle", 0. );
		dbInstance->IntSet( "pos", TEXTPOS_DEFAULT );

		// Redundant, but makes life easier in VB
		dbInstance->StringSet( STR_TYPE, "_instance" );
	}

	return dbInstance;
}

void MinMax( const C2dCoordArray& pts, C2dCoord* minmax )
{
	int count, indx;

	minmax[0].XY( UNDEFINED, UNDEFINED );
	minmax[1].XY( UNDEFINED, UNDEFINED );
	minmax[2].XY( -UNDEFINED, -UNDEFINED );
	minmax[3].XY( -UNDEFINED, -UNDEFINED );

	count = pts.Count();
	for (indx = 0; indx < count; ++indx)
	{
		const C3dCoord& pt = *pts[indx];

		if (pt.X() < minmax[0].X())
			minmax[0] = pt;

		if (pt.X() > minmax[2].X())
			minmax[2] = pt;

		if (pt.Y() < minmax[1].Y())
			minmax[1] = pt;

		if (pt.Y() > minmax[3].Y())
			minmax[3] = pt;
	}
}

void HullTransform( const C3x4Matrix& matrix, C2dCoordArray* pts )
{

	int count, indx;

	count = pts->Count();
	for (indx = 0; indx < count; ++indx)
	{
		C2dCoord* pt = pts->GetAt(indx);
		matrix.Transform( pt );
	}
}


// Nest:FitTest:
CReturn
CNestProcessApp::FitTest( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbEntityArray	entities;
	CDbProfile*		dbProfile;
	int				count, indx;

	CModel&	model = io_cmd->getModel();
	CSelector& selector = model.SelectorStack()();

	dbProfile = NULL;  // assume failure

	count = selector.Count();
	if (count > 0)
	{
		for (indx = 0; indx < count; ++indx)
		{
			entities.Append( selector[indx] );
		}

		dbProfile = CModelUtil::CHPartOutlineCreate( entities, 1.e-3, &model );
		io_cmd->getViewMgr().Refresh(true);
	}

	C3dBox box;
	C2dVec vec2d;
	C2dUnitVec uvec;
	C3x4Matrix xform;
	double radians;

	double dx = model.Header().getReal( "Length", 0. );
	double dy = model.Header().getReal( "Width", 0. );

	C2dCoordArray pts;
	C2dCoord minmax[4];

	count = dbProfile->Count();
	for (indx = 0; indx < count; ++indx)
	{
		CDbLine* dbLine = dynamic_cast<CDbLine*>( (*dbProfile)[indx] );
		pts.Append( new C2dCoord( dbLine->EndPt() ) );
	}

	int jndx;
	for (indx = 0; indx < count; ++indx)
	{
		jndx = indx + 1;
		if (jndx >= count)
			jndx = 0;

		vec2d = *pts[jndx] - *pts[indx];

		uvec.Init( vec2d.X(), vec2d.Y() );

		radians = -uvec.Radians();
		
		xform.setXYAngle( radians );
		dbProfile->Transform( xform );

		HullTransform( xform, &pts );

		io_cmd->getViewMgr().Refresh(true);

		box = dbProfile->Box();
		if ((box.Dx() <= dx) && (box.Dy() <= dy))
			break;  // fits
	}

	pts.DestructiveFlush();

	xform.setUnit();
	xform.setT( -box.Xmin(), -box.Ymin(), 0. );
	dbProfile->Transform( xform );

	io_cmd->getViewMgr().Refresh( true );

	return status;
}


CReturn
CNestProcessApp::BumpInit( CCommand* io_cmd )
{
	CReturn status;

	if ((m_instance == NULL) && (m_pattern == NULL))
	{
		CModel& model = io_cmd->getModel();
		CViewMgr& view = io_cmd->getViewMgr();
		CViewBase* vbase = view.ActiveView();

		// "pattern" is non-zero when a pattern is selected from the
		// dropdown list of patterns. Otherwise, "pattern" is zero
		// because an instance was selected from the gview.
		int pattern = io_cmd->VarList().getInt( "pattern", FALSE );

		// Given the instance id.
		int id = io_cmd->VarList().getInt( "id", 0 );

		if ( !pattern )
		{
			model.EntityFind( id, (CDbEntity**) &m_instance, DBCOMMAND, DBCOMMAND );
			if (m_instance == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::BumpInit(#1)" );
			}
			else
			{
				// We will be using the pattern associated with this instance.
				id = (ID) m_instance->IntGet( "patid", 0 );
			}
		}

		if ( status.IsOk() )
		{
			model.EntityFind( id, (CDbEntity**) &m_pattern, DBPATTERN, DBPATTERN );
			if (m_pattern == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::BumpInit(#2)" );
			}
			else
			{
				// This is necessary for the success of VB.
				io_cmd->setInt( "pattern_id", id );
			}
		}

		if ( status.IsOk() )
		{
			ShiftInit( io_cmd );

			if (m_instance == NULL)
				// m_handle.XY( 0., 0. );
				m_handle.XY( 0., -m_shift );
			else
				m_handle = m_instance->Coord();

			m_anchor = m_handle;
			m_radians = RadiansNormalize( m_pattern->DoubleGet( "part_angle", 0. ) );
			m_delta_radians = 0.;

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Generate the display list for the select object.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			vbase->DisplayListBegin( STEMP_LIST );
			vbase->SemiTempDelta( ORIGIN );

			bool allocated;
			CDisplayEntity* de;

			CDbEntity::OverrideColorSet( DCOLOR_RED );

			if (m_instance == NULL)
			{
				C3x4Matrix xform;
				xform.setT( 0., -m_shift, 0. );
				// ASSUMPTION: A pattern was selected instead of an instance.
				vbase->DrawPatternAt( m_pattern, &xform );
				de = m_pattern->DisplayEntityGet( &allocated );
			}
			else
			{
				// Temporarily make the handle point of the instance
				// coincident with the world origin, then translation
				// of the image is handled by SemiTempDelta().
				C3dCoord origin( 0., -m_shift, 0. );
				C3dCoord curr = m_instance->Coord( origin );
				vbase->InstanceDraw( m_instance );
				m_instance->Coord( curr );

				de = m_instance->DisplayEntityGet( &allocated );
			}

			CDbEntity::OverrideColorClear();

			vbase->DrawDirect( de );
			vbase->DisplayListEnd( STEMP_LIST );

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			// So that the pattern can be displayed in the dialog preview.
			{
				COptimizer	opti;
				CDbEntityArray	entities;
				C3dCoord	pt;

				if (m_instance != NULL)
					pt = m_instance->Coord();
				else
					pt.XYZ( 0., 0., 0. );

				int part_id = m_pattern->IntGet( "part_id", 0 );

				opti.prep_container( m_pattern, &entities );
				PolyDataGenerate( entities, io_cmd );

				io_cmd->setInt( "part_id", part_id );
				io_cmd->setReal( "part_angle", m_radians );
				io_cmd->setReal( "xp", pt.X() );
				io_cmd->setReal( "yp", pt.Y() );
				io_cmd->setInt( "pattern_id", id );
			}
		}

		if ( status.IsOk() )
		{
			C3dCoord delta( m_handle.X(), (m_handle.Y() + m_shift), m_delta_radians );
			vbase->SemiTempDelta( delta );
			vbase->BufferShow( FRONT_BUFFER );

			// So that we can echo the handle point location.
			io_cmd->setReal( "xp", m_handle.X() );
			io_cmd->setReal( "yp", m_handle.Y() );
			io_cmd->setReal( "orient", (m_radians + m_delta_radians) );
		}
	}

	return status;
}

CReturn
CNestProcessApp::BumpAnchor( CCommand* io_cmd )
{
	CReturn status;

	// Establish the anchor point
	double xp = io_cmd->VarList().getReal( "xp", 0. );
	double yp = io_cmd->VarList().getReal( "yp", 0. );

	m_anchor.XY( xp, yp );
	m_vec.Init( UNDEFINED, UNDEFINED );

	return status;
}

CReturn
CNestProcessApp::BumpOrient( CCommand* io_cmd )
{
	CReturn status;

	// Establish the anchor point
	double radians = io_cmd->VarList().getReal( "radians", UNDEFINED );
	if (radians < UNDEFINED)
	{
		double curr_orient = RadiansNormalize( m_radians + m_delta_radians );
		double next_orient = RadiansNormalize( radians );

		double delta = next_orient - curr_orient;
		if ((fabs(delta) > SMALL))
		{
			// Translate (effectively) the anchor point to the origin.
			C2dVec tvec( m_anchor.X(), m_anchor.Y() );
			m_handle -= tvec;

			// Rotate about the anchor point.
			C3x4Matrix xform;
			xform.setXYAngle( delta );
			xform.Transform( &m_handle );

			// Translate the anchor point back to its original position.
			m_handle += tvec;

			m_delta_radians = RadiansAccumulate( delta );

			if ( status.IsOk() )
			{
				CViewMgr& view = io_cmd->getViewMgr();
				CViewBase* vbase = view.ActiveView();

				C3dCoord delta( m_handle.X(), (m_handle.Y() + m_shift), m_delta_radians );

				vbase->SemiTempDelta( delta );
				vbase->BufferShow( FRONT_BUFFER );
			}
		}
	}

	// So that we can echo the handle point location. We do this regardless
	// of whether we actually did anything because of VB's goofy ui behavior.
	// In particular, a mtbOrientation_Change() event is raised when
	// "Orientation" is the active input and you click on Move or Copy.
	io_cmd->setReal( "xp", m_handle.X() );
	io_cmd->setReal( "yp", m_handle.Y() );
	io_cmd->setReal( "orient", (m_radians + m_delta_radians) );

	return status;
}

CReturn
CNestProcessApp::BumpDrag( CCommand* io_cmd )
{
	CReturn status;

	double xr = io_cmd->VarList().getReal( "xr", UNDEFINED );
	double yr = io_cmd->VarList().getReal( "yr", UNDEFINED );

	if ((xr < UNDEFINED) && (yr < UNDEFINED))
	{
		C2dUnitVec vec( (xr - m_anchor.X()), (yr - m_anchor.Y()) );

		if (m_vec.X() < UNDEFINED)
		{
			// We're rotating the part about the anchor point.
			if ( vec.IsValid() )
			{
				// Translate (effectively) the anchor point to the origin.
				C2dVec tvec( m_anchor.X(), m_anchor.Y() );
				m_handle -= tvec;

				double delta = vec.Radians() - m_vec.Radians();

				// Rotate about the anchor point.
				C3x4Matrix xform;
				xform.setXYAngle( delta );
				xform.Transform( &m_handle );

				// Translate the anchor point back to its original position.
				m_handle += tvec;
				m_vec = vec;

				m_delta_radians = RadiansAccumulate( delta );
			}
		}
		else
		{
			// We're establishing a reference vector.
			m_vec = vec;
		}
	}
	else
	{
		// We're translating the part.
		double xp = io_cmd->VarList().getReal( "xp", 0. );
		double yp = io_cmd->VarList().getReal( "yp", 0. );

		C2dCoord pt( xp, yp );
		C2dVec delta( pt - m_anchor );
		m_anchor = pt;

		m_handle += delta;
	}

	if ( status.IsOk() )
	{
		CViewMgr& view = io_cmd->getViewMgr();
		CViewBase* vbase = view.ActiveView();

		C3dCoord delta( m_handle.X(), (m_handle.Y() + m_shift), m_delta_radians );

		vbase->SemiTempDelta( delta );
		vbase->BufferShow( FRONT_BUFFER );

		// So that we can echo the handle point location.
		if (m_zone == NULL)
		{
			io_cmd->setReal( "xp", m_handle.X() );
			io_cmd->setReal( "yp", m_handle.Y() );
			io_cmd->setReal( "orient", (m_radians + m_delta_radians) );
		}
		else
		{
			// Dragging the hold-down is somewhat different.
			io_cmd->setReal( "xp", m_handle.X() );
			io_cmd->setReal( "yp", (m_handle.Y() - m_shift) );
		}
	}

	return status;
}

CReturn
CNestProcessApp::BumpMoveCopy( CCommand* io_cmd )
{
	CReturn status;

	CModel& model = io_cmd->getModel();
	CViewMgr& view = io_cmd->getViewMgr();
	CViewBase* vbase = view.ActiveView();

	// "opt" represents the requested action.
	EBumpOption opt = (EBumpOption) io_cmd->VarList().getInt( "opt", -1 );

	if (m_pattern == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::BumpMoveCopy()" );
	}
	else
	{
		// radians
		double part_angle = m_pattern->DoubleGet( "part_angle", 0. );
		int part_id = m_pattern->IntGet( "part_id", 0 );

		if ((opt == BUMP_MOVE) || (opt == BUMP_COPY))
		{
			CDbCommand* dbInstance = m_instance;
			if ((m_pattern != NULL) && (dbInstance == NULL))
			{
				// ASSUMPTION: A pattern (instead of an instance) was selected.
				dbInstance = InstanceCreate( (*m_pattern), part_angle, 0., 0. );
				// And now that we're going to move an existing instance ....
				io_cmd->setInt( "opt", BUMP_MOVE );
			}

			status = MoveCopy( m_pattern, dbInstance, io_cmd );

			// And now that we're done we must set these values
			// to NULL because, otherwise, we will be unable to
			// properly deal with the next bumped pattern/instance.
			m_pattern = NULL;
			m_instance = NULL;

			// And so that we don't see graphic artifacts ....
			vbase->Clear( STEMP_LIST );
			vbase->BufferShow( BACK_BUFFER );
		}
	}

	return status;
}

CReturn
CNestProcessApp::BumpCancel( CCommand* io_cmd )
{
	CReturn status;

	CViewMgr& view = io_cmd->getViewMgr();
	CViewBase* vbase = view.ActiveView();

	// And now that we're done we must set these values
	// to NULL because, otherwise, we will be unable to
	// properly deal with the next bumped pattern/instance.
	m_pattern = NULL;
	m_instance = NULL;
	m_zone = NULL;

	// And so that we don't see graphic artifacts ....
	vbase->Clear( STEMP_LIST );
	vbase->BufferShow( BACK_BUFFER );

	return status;
}

void
CNestProcessApp::ShiftInit( CCommand* io_cmd )
{
	CModel& model = io_cmd->getModel();

	EQuadrant quadrant = (EQuadrant) model.Header().getInt( "WorkplaneType", QUADRANT_INVALID );
	m_shift = 0.;
	if (quadrant == QUADRANT_FOUR)
		m_shift = model.Header().getReal( "Width", 0. );
}

double
CNestProcessApp::RadiansAccumulate( double delta_radians )
{
	return ( RadiansNormalize( m_delta_radians + delta_radians ) );
}

double
CNestProcessApp::RadiansNormalize( double radians )
{
	if (radians >= TWOPI)
		radians -= TWOPI;
	else if (radians < 0.)
		radians += TWOPI;

	return radians;
}

// Query
//    Nest:CutBack: action=2
//    Returns: id, station_id, x
// Update
//    Nest:CutBack: action=1, station_id=%d, x=%f
// Delete
//    Nest:CutBack: action=3

CReturn
CNestProcessApp::CutBack( CCommand* io_cmd )
{
	CReturn status;

	const CVarList& params = io_cmd->VarList();
	CModel& model = io_cmd->getModel();

	CCutBack cb;
	cb.ModelSet( &model );

	eWeAction action = (eWeAction) params.getInt( "action", IUNDEFINED );
	if (action == WEQUERY)
	{
		CDbLine* cutback = cb.CutBackLineGet();
		if (cutback == NULL)
		{
			io_cmd->setInt( "id", 0 );
			io_cmd->setInt( "station_id", -1 );
			io_cmd->setReal( "x", -UNDEFINED );
		}
		else
		{
			int station_id = cutback->Tool()->IntGet( STR_STATION_ID, 0 );
			io_cmd->setInt( "id", cutback->Id() );
			io_cmd->setInt( "station_id", station_id );
			io_cmd->setReal( "x", cutback->StartPt().X() );
		}
	}
	else if (action == WEUPDATE)
	{
		int station_id = params.getInt( "station_id", 0 );
		double x = params.getReal( "x", -UNDEFINED );
		status = cb.Update( station_id, x );
	}
	else if (action == WEDELETE)
	{
		status = cb.Delete();
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::CutBack()" );
	}

	return status;
}

