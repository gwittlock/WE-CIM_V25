
#include "stdafx.h"
#include "assert.h"
#include <math.h>
#include <float.h>

#include "MathConst.h"
#include "CommonFlags.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Register.h"
#include "Path.h"
#include "lead.h"

#include "DbIterator.h"
#include "DbSequence.h"
#include "DbEntity.h"
#include "DbFeature.h"
#include "DbTool.h"
#include "DbArc.h"
#include "DbLine.h"
#include "ImportUtil.h"

#include "DbPoint.h"
#include "Solution.h"

#include "Nibbler.h"
#include "Profile.h"
#include "Worm.h"
#include "ModelUtil.h"
#include "ModelText.h"
#include "Conversion.h"

#include "MM2.h"

#if (_CI || _NST)
#include "Portal.h"
#include "CiLine.h"
#include "CiArc.h"
#include "CiModel.h"
#endif

#include "Profiler.h"

#include "hull.h"
#include "NestingPart.h"

#if (!_CI && !_NST)
#include "ToolShapeExtruder.h"
#endif

// Thank the Lord for people who share their hard work!
#include "ProgressWnd.h"

static CProfiler g_nesting_part_profiler;

// ==================================================================

static CSelectorStack		dummy_stack;
static CTreeViewSupport		dummy_tree;


// To be used in the event the IDE's QuickWatch returns the
// message "function not present" when you try to evaluate
// something like 'poly->Draw()'.
static void PolyDraw( CGeoPoly* poly )
	{ poly->Draw(); }

// ==================================================================

CNestingPart::CNestingPart()
{
	m_id = -1;
	m_idx = -1;

	m_flag = 0;

	m_quantity = 0;
	m_totquant = 0;
	m_pass = 0;
	m_used = 0;

	m_cost = 0.0;
	m_rotation = 1;
	m_startang = 0.0;

	m_mirror = false;
	m_startmirror = false;

	m_error_msg = "";

	m_toolhits = NULL;

	m_exclusions = 0;

	// (0) do not close gaps / (1) V16.5 / (2) experimental.
	m_use_gap_tool = CRegister::IntGetV( "Nesting", "use_gap_tool", 2 );

	// (0) V16.5 / (1) experimental.
	// As I remember (rightly or wrongly) punch-only parts
	// nest better for Rittal when use_tool_extruder is true.
	m_use_tool_extruder = CRegister::IntGetV( "Nesting", "use_tool_extruder", 1 );

	m_filled_gaps = false;
	m_gap_tool = NULL;

	m_kerf_adjust = 0.;

	m_representation = 0;
	m_prenest = false;
	m_first_inspection = false;
	m_is_oversized = false;
}

CNestingPart::~CNestingPart()
{
	m_modelarray.DestructiveFlush();
	m_toolhitarray.DestructiveFlush();

	if (m_toolhits)
		delete m_toolhits;
}

void CNestingPart::Filepath( const CString& fq_path )
{
	CString	sval = CRegister::StringGetV( "Debug", "$SOURCE", "" );
	if ( sval.IsEmpty() )
	{
		m_filepath = fq_path;
	}
	else
	{
		// We must be executing a RTL test.
		CPath path( fq_path );
		m_filepath = sval + "\\" + path.FileNameExt();
	}
}

// ==================================================================
//		Simplify
//
//	Master driver for all simplification... operates on the base
//	model at index 0
//
CReturn CNestingPart::Simplify( CNestConfig& config )
{
	CReturn status;

	EWMNesting( "Simplify:" );

	CModel* model = m_modelarray[0];
	model->UndoBufferSuppress();

	status = do_pattern_explode( model );

	// Note that StripTools is tested internally.
	// simplify_tooling ALSO updates the tab gap info
	if (status.IsOk())
		status = do_simplify_tooling( config, model );

	if ( status.IsOk() && config.StripLayers() )
		status = do_strip_layers( model );

	if ( status.IsOk() )
		status = do_transform( model );

	// NOTE: do_simplify_geometry() updates the GapTolerance().
	if (status.IsOk())
		status = do_simplify_geometry( config, model );

	if (status.IsOk())
		status = do_simplify_workplane( model );

	if (status.IsOk())
		status = do_simplify_rotation( model );

	return status;
}

// ==================================================================

CReturn CNestingPart::do_pattern_explode( CModel* model )
{
	CReturn ret;

	CDbIterator	iter;
	iter.Init( model->Db(), DBPATTERN );
	while (1)
	{
		CDbPattern* dbPattern = dynamic_cast<CDbPattern*>( iter() );
		if (dbPattern == NULL)
			break;
		iter.Next();

		ret += CModelUtil::PatternExplode( (*dbPattern) );
		dbPattern->Delete();
	}

	return ret;
}
	
// ==================================================================
//	Take the master part and remove *all* tool entities that are
//	not referenced by geometry.
//
char* gLeadContextName[8] =
{
	"Lead In, Interior Line",
	"Lead In, Interior Arc",
	"Lead In, Exterior Line",
	"Lead In, Exterior Arc",
	"Lead Out, Interior Line",
	"Lead Out, Interior Arc",
	"Lead Out, Exterior Line",
	"Lead Out, Exterior Arc"
};

CReturn CNestingPart::do_simplify_tooling( CNestConfig& config, CModel* model )
{
	CReturn	ret;
	CDbEntityList reserved_tool;
	CDbIterator iter;
	CDbWorkplane* dbWork;
	CEntityDb*	db;
	double dia;
	int num;

	double min_dia = DBL_MAX;

	EWMNesting( "   Simplify Tooling" );

	// Using the lead system, generate a list of all possible reserved
	//	pierce tools.  <sigh>
	//
	// Assumes we only pierce in TOP plane
	//
	model->EntityFind( STR_TOP, (CDbEntity**)&dbWork, DBWORKPLANE, DBWORKPLANE );

	if (config.LeadSetup() > 0)
	{
		CLead lead(*model);

		for (int context=0; context<LEAD_CONTEXT_NUM; context++)
		{
			double dia;
			CDbTool* tool = lead.PierceTool( config.CmdbPath(), config.LeadSetup(), context, dbWork, &dia );

			if (tool != NULL)
			{
				// 2012.04.28 (PE) -- A complete hack.
				if (tool->IntGet( "~create", 0 ) != 0)
				{
					// The tool was not found in the part-model but it
					// was found in the master-model (ie. the model
					// normally associated with the gview).
					CModel* master = CLead::MasterModel();
					master->EntityPrepareCopy( model );

					CDbTool* copy;
					master->EntityCopy( (CDbEntity&)*tool, (CDbEntity**) &copy );

					// Lest we continue making copies ....
					copy->AttribDelete( "~create" );

					tool = copy;
				}

				// TODO: Use map to ensure unique entries (?)
				reserved_tool.Append( tool );
			}
			else if (!ZERO(dia))
			{
				// Cheat because I don't feel like making an IDS for this.
				CString note;
				note.Format( "Part '%s' %s: Pierce tool diameter %f not found", Name(), gLeadContextName[context], dia );
				EWMWarning( (LPCSTR) note );
			}
		}
	}

	// Now, check all tools against (a) use and (b) reserved

	num = reserved_tool.Count();
	db = (CEntityDb*)&model->Db();
	iter.Init( (*db), DBTOOL );
	while ( TRUE )
	{
		CDbTool* db_tool = dynamic_cast<CDbTool*>( iter() );
		if (!db_tool)
			break;
		iter.Next();

		if (db_tool->IsRefd())
		{
			dia = db_tool->EffectiveDiameter();
			if ( (dia < min_dia) && !ZERO(dia) )
				min_dia = dia;

			// 2005.01.25 (PE) -- Nestings "Pierce Hole / Exclude Kerf" option
			// failed to yield good nest.  In short, we now offset the pierce
			// hole to its outside so that HIT_NEST_OUTSIDE is better adjusted.
			if ( config.HardPierce() )
			{
				// 2008.01.16 (PE) -- Disabled ... now that HIT_NEST_OUTSIDE,
				// HIT_PART_OUTSIDE, etc. more accurately represent things.
#if BEFORE_2008_01_16
				// We want the pierce hole placed outside the kerf.
	#if BEFORE_2005_03_04
					// When the machine is doing kerf compensation, we
					// must adjust by the beam diameter.  Otherwise, we
					// must adjust by the bream radius.
					if (model->Header().getInt( "PartProf", 0 ) == 0)
						dia /= 2.;

					if (db_tool->IsCuttingTool() && (dia > m_kerf_adjust))
						m_kerf_adjust = dia;
	#else
					// The previous solution was (in theory) okay but
					// Sunflower reported that the pierce hole (being so
					// close to the adjacent part) diminished web integrity.
					if ( db_tool->IsCuttingTool() )
						m_kerf_adjust = config.Spacing();
	#endif
#endif
			}

			continue;
		}

		bool reserved = FALSE;
		for (int idx=0; idx<num; idx++)
		{
			if (db_tool == reserved_tool[idx])
			{
				reserved = TRUE;
				// Don't really care to get sizes of reserved tools; these
				// will be lead punches of minor importance.
				break;
			}
		}

		if ( reserved || !config.StripTools() )
			continue;

		db->Delete( (CDbEntity**)&db_tool );
	}

	// Now, upgrade our gap tolerance to minimum tool radius.  This will still
	// prevent jumping across kerf, but also allows jumping across major
	// imperfections.
	if (min_dia < (DBL_MAX-SMALL))
	{
		if ( ZERO(config.MinTool())	|| (min_dia < config.MinTool()) )
		{
			config.MinTool(min_dia);
		}
	}

	// Optionally report on tools in the part
	if (CReturn::Debug()>=1)
	{
		CString note;
		note.Format( "====================================================================" );
		EWMNesting( note );
		note.Format( "  Tooling used for Part '%s' (min diameter %f)", m_filepath, min_dia );
		EWMNesting( note );

		iter.Init( (*db), DBTOOL );
		while ( TRUE )
		{
			CDbTool* db_tool = dynamic_cast<CDbTool*>( iter() );
			if (!db_tool)
				break;

			const CVarList& attrib = db_tool->Attrib();
			note.Format( "    T%d : %s",
				attrib.getInt( STR_NC_CODE_NUMBER, -1 ),
				attrib.getString( STR_DESCRIPTION, "<error>" ) );

			EWMNesting( note );
		
			iter.Next();
		}
	}

	return ret;
}

// ==================================================================
//	Remove all layers from the master part.
CReturn CNestingPart::do_strip_layers( CModel* model )
{
	CReturn	ret;

	EWMNesting( "   Strip Layers" );

	ret = CImportUtil::LayersStrip( model );

	return ret;
}

CReturn CNestingPart::do_transform( CModel* model )
{
	CReturn	ret;
	C3dBox	box;

	EWMNesting( "   Setting local origin" );

#if BEFORE_SUNFLOWER_BUG  // maybe just do this for 4th quad?
// This was causing caching problems!!!!
//	box = model->Box(~0);
	box = model->Box(0);
#else
	box = model->Box(~0);
#endif
	if (fabs(box.Xmin()) > SMALL || fabs(box.Ymin()) > SMALL)
	{
		CDbIterator	iter;
		C3x4Matrix	xform;

		CDbEntity::NewAction();

		xform.Shift( C3dVec( -box.Xmin(), -box.Ymin(), 0. ) );

		iter.Init( model->Db(), DBPOINT );
		while (1)
		{
			CDbEntity* dbEntity = iter();
			if (dbEntity == NULL)
				break;

			if ((dbEntity->Type() == DBPOINT) ||
				(dbEntity->Type() == DBHOLE))
			{
				dbEntity->Transform( xform );
			}
			else
			{
				dbEntity->BoxInvalidate();
			}

			iter.Next();
		}
	}

	if (0)
	{
			CModelText text;
			text.Enable( true );
			ret = text.Dump( CString("_foo"), (*model) );
	}

	return ret;
}

// ==================================================================
//	1. Force circles into profiles
//	(x). USED TO explode sequences, but that is no longer necessary
//	2. Fills in gaps in profiles
//	3. Conditionally sets the default for config.GapTolerance().
CReturn CNestingPart::do_simplify_geometry( CNestConfig& config, CModel* model )
{
	CReturn		ret;
	CDbIterator	iter;
	C3dCoord	a_pt;
	C3dCoord	b_pt;
	CDbProfile*	db_profile;
	CDbLine*	db_line;
	CDbArc*		db_arc;
	CDbCurve*	a_curve;
	CDbCurve*	b_curve;
	double		tab_gap, dist;

	EWMNesting( "   Simplify Geometry" );

	if (m_use_gap_tool > 1)
	{
		// Create a bogus tool to fill the gaps.

		model->EntityCreate( DBTOOL, (CDbEntity**) &m_gap_tool );

		m_gap_tool->IntSet( STR_TYPE_ID, TTYPE_BURNER );
		m_gap_tool->DoubleSet( "kerf", 0.1 );

		// Mark this tool so that we can delete it later.
		m_gap_tool->IntSet( "gap_tool", 1 );
	}

	// NOTE: CDbSequence objects are not in model because the 
	//       mm2 is read using the MM2_SKIP_SEQ_OBJS flag.

	//
	// HORRIBLE FUCKING HACK ALERT
	//	Convert any full circles that are not currently in profiles to profiles
	//
	// TODO:  Pull out into a separate method, maybe even in Policy
	//

	// NOTE: Failing to place lone-arcs into profiles causes the message
	// "Selection reduces to empty space -- no closed, tooled profiles to apply leads on"
	// 
	iter.Init( model->Db(), DBARC );
	while ( TRUE )
	{
		db_arc = dynamic_cast<CDbArc*>( iter() );
		if (!db_arc)
			break;

		iter.Next();

		if ( !db_arc->StartPt().WithinTol( db_arc->EndPt(), SMALL ) )
			continue;

		db_profile = dynamic_cast<CDbProfile*>( db_arc->Owner() );
		if (!db_profile)
		{
			model->EntityCreate( DBPROFILE, (CDbEntity**)&db_profile);

			CDbContainer* owner = dynamic_cast<CDbContainer*>(db_arc->Owner());
			if (owner)
			{
				owner->InsertBefore(db_arc, db_profile);
			}
			db_profile->Append( db_arc );
		}
	}

	// Spackle over any and all gaps in profiles
#if BEFORE_V18_0_109_1
	if (m_use_gap_tool > 0)
#else
	// Now we let do_simplify_geometry() determine the gap_tolerance.
#endif
	{
		double max_area = 0.;
		CDbProfile* outside_profile = nullptr;  // assumed outside profile

		tab_gap = 1.e-4;  // arbitrary

		iter.Init( model->Db(), DBPROFILE );
		while ( TRUE )
		{
			db_profile = dynamic_cast<CDbProfile*>( iter() );
			if (!db_profile)
				break;

			iter.Next();

			int num = db_profile->Count()-1;
			if (num < 0)
				continue;

			b_curve = (CDbCurve*) db_profile->GetAt(num);

			for (int idx=num; idx>=1; idx--)
			{
				a_curve = (CDbCurve*) db_profile->GetAt(idx-1);

				a_pt = a_curve->EndPt();
				b_pt = b_curve->StartPt();
				if (!a_pt.WithinTolXY(b_pt, SMALL))
				{
					if (m_use_gap_tool > 0)
					{
						// Fill the gap

						model->EntityCreate(DBLINE, (CDbEntity**)&db_line);
						
						// Mark this entity for deletion by GapEntitiesRemove().
						db_line->IntSet( "is_gap_elem", TRUE );

						if (m_use_gap_tool == 1)
						{
							// V16.5 and earlier.
							db_line->Init(a_curve->Tool(), a_curve->Workplane(), a_pt, b_pt);
							db_profile->InsertBefore(b_curve, db_line);
						}
						else
						{
							// Experimental (the default for V18).
							db_line->Init(m_gap_tool, a_curve->Workplane(), a_pt, b_pt);
							db_profile->InsertBefore(b_curve, db_line);
						}

						m_filled_gaps = true;
					}
					else
					{
						// We track the gap size and let ExtractProfile() do the
						// nasty, error prone, work of piecing things together.
						dist = a_pt.Dist( b_pt );
						if (dist > tab_gap)
							tab_gap = dist;
					}
				}

				b_curve = a_curve;
			}

			if ( CanClose( *db_profile ) )
			{
				num = db_profile->Count()-1;

				a_curve = (CDbCurve*) db_profile->GetAt(num);
				b_curve = (CDbCurve*) db_profile->GetAt(0);

				a_pt = a_curve->EndPt();
				b_pt = b_curve->StartPt();
				if (!a_pt.WithinTolXY(b_pt, SMALL))
				{
					if (m_use_gap_tool > 0)
					{
						// 2013.08.11 (PE) -- This next conditional was introduced to address
						// an issue encountered by ITI where an open (interior) profile was
						// closed but "should not have been". This is a difficult problem
						// to solve; I have not been able to find a robust solution. So here
						// we try a simple low-cost solution. The intent here is to close
						// the start/end gap only when the gap "seems like a tab gap".
						dist = a_pt.Dist( b_pt );
						if (dist < (1.25 * tab_gap))
						{
							// Fill the gap

							model->EntityCreate(DBLINE, (CDbEntity**)&db_line);
						
							// Mark this entity for deletion by GapEntitiesRemove().
							db_line->IntSet( "is_gap_elem", TRUE );

							if (m_use_gap_tool == 1)
							{
								// V16.5 and earlier.
								db_line->Init(a_curve->Tool(), a_curve->Workplane(), a_pt, b_pt);
								db_profile->InsertBefore(b_curve, db_line);
							}
							else
							{
								// Experimental (the default for V18).
								db_line->Init(m_gap_tool, a_curve->Workplane(), a_pt, b_pt);
								db_profile->InsertBefore(b_curve, db_line);
							}

							m_filled_gaps = true;
						}
					}
					else
					{
						// We track the gap size and let ExtractProfile() do the
						// nasty, error prone, work of piecing things together.
						dist = a_pt.Dist( b_pt );
						if (dist > tab_gap)
						{
							// We must be careful here.  We do not want an truely
							// open profile to unduly influence the tab gap because
							// that affects the result of ExtractProfile() which,
							// in turn, can cause nesting failures like overlapping
							// parts and parts overlapping the material border.
							// NOTE: This may always be problematic!
							if (dist < (1.25 * tab_gap))
								tab_gap = dist;
						}
					}
				}
			}

			C2dBox box = db_profile->Box();
			double area = box.Area();
			if (area > max_area)
			{
				max_area = area;
				outside_profile = db_profile;
			}
		}

		if (outside_profile != nullptr)
			outside_profile->IntSet( "~outside_profile", 1 );

		if (tab_gap > config.GapTolerance())
			config.GapTolerance( tab_gap );
	}
	/*
	//
	// Delete any "interior" geometry if we are in solid parts mode
	//
	if (!config.PartInPart())
	{
		iter.Init( model->Db(), DBPROFILE );
		while ( TRUE )
		{
			CDbProfile* db_prof = dynamic_cast<CDbProfile*>( iter() );
			if (!db_prof)
				break;
			iter.Next();

			if (db_prof->IntGet(STR_PROFILE_DEPTH, 0) & 0x01)
			{ db_prof->Delete(); }
		}
	}
	*/


	return ret;
}

// ==================================================================
//	HACK!  Scan the tooling in the part, and if necessary, reduce
//	the rotation (and start-angle) values for this part to satisfy
//	the most restrictive tool in the lexicon.
//
//	TODO:  Fix transform to reset the tooling on rotation
//
CReturn CNestingPart::do_simplify_rotation( CModel* model )
{
	CReturn	ret;

	CEntityDb* db = (CEntityDb*)&model->Db();

	EWMNesting( "   Simplify Rotation" );

	eToolSymmetry symmetry = TSYM_1;

	CDbIterator iter;
	iter.Init( (*db), DBTOOL );
	while ( TRUE )
	{
		CDbTool* db_tool = dynamic_cast<CDbTool*>( iter() );
		if (!db_tool)
			break;
		iter.Next();

		if (db_tool->IsRefd())
		{ symmetry = min( symmetry, db_tool->Symmetry() ); }
	}

	double start_ang = fabs( m_startang );

	// Be sure to avoid inappropriate rotation promotion!
	int nrotations, rotsign;
	if (m_rotation >= 0)
	{
		// This is the normal branch.
		nrotations = m_rotation;
		rotsign = 1;
	}
	else
	{
		// This branch was introduced for ITI.
		// ie. (-2) (0, 90), (-3) (0, 45, 90), etc.
		nrotations = abs(m_rotation);
		rotsign = -1;
	}
	
	switch (symmetry)
	{
	case TSYM_NONE:
		m_rotation = 0;
		m_startang = 0.0;
		m_mirror = false;
		m_startmirror = false;
		break;

	case TSYM_180:
		if (nrotations & 0x01)
			m_rotation = 0;  // Odd rotations are forbidden
		else
			m_rotation = rotsign * min(nrotations, 2);  // Even rotations go to 180'

		if (!EQUAL( start_ang, 180 ))
			m_startang = 0;
		break;

	case TSYM_90:
		if (nrotations & 0x03)
		{
			if (nrotations & 0x01)
				m_rotation = 0;  // Odd rotations are forbidden
			else
				m_rotation = rotsign * min(nrotations, 2);  // Even rotations go to 180'
		}
		else
		{
			m_rotation = rotsign * min(nrotations, 4);
		}

		if (!EQUAL( start_ang, ((int)(start_ang/90.0)*90.0 )) )
			m_startang = 0.0;	// Not a multiple of 90', zero it
		break;

	case TSYM_1:
		// Life is grand (round tool?)
		break;
	}

	return ret;
}

// ==================================================================
//	Dammit, we don't want a mish-mash of Top vs. World geometry.  It 
//	mucks up the Pattern/Instance stuff.
//
//	Force all the stuff in World over to Top.
//
CReturn CNestingPart::do_simplify_workplane( CModel* model )
{
	CReturn ret;
	CDbIterator iter;

	EWMNesting( "   Simplify Workplane" );

	CDbWorkplane* top = NULL;
	model->EntityFind( STR_TOP, (CDbEntity**)&top, DBWORKPLANE, DBWORKPLANE );

	CDbWorkplane* world = NULL;
	model->EntityFind( STR_WORLD, (CDbEntity**)&world, DBWORKPLANE, DBWORKPLANE );

	if ( !top || !world )
		return ret;

	const C3x4Matrix& to_top = top->Inverse();

	CDbEntity::NewAction();

	iter.Init( model->Db(), DBPOINT );
	while ( TRUE )
	{
		CDbEntity* db_ent = iter();
		if (!db_ent)
			break;

		if (db_ent->Type() > DBHOLE)
			break;

		iter.Next();

		if (db_ent->Workplane() == world)
		{ 
			db_ent->Transform( to_top );
			db_ent->Workplane( top );
		}
	}

	return ret;
}

// ==================================================================
//	Calculate the total area of the part (outline)... with or without
//	holes accounted for.
//
// 2004.02.19 (PE) -- Cad Nester was dropping the inside kerf profiles,
// thereby preventing part-in-part nesting where such was possible.
CReturn CNestingPart::calcArea( CNestConfig& config )
{
	CReturn	status;
	CToolHit* toolhit;
	double	inside_area;
	double	min_area;

	// Set up our context of information
	toolhit = m_toolhitarray[0];
	if (!toolhit)
		return status.Fatal( IDS_NEST_PART_EMPTY );

	// Get Part area
	if ( CNestConfig::IsBitmapNest() )
	{
		// When bitmap nesting, we do not have an outside kerf
		// because we assume we are dealing with nasty yard-art
		// type parts that have handdrawn broken profiles.
		//
		// CNestingPart::bitmap_generate() is generates the
		// HIT_NEST_OUTSIDE data.
		m_max_area = do_calc_area(
			toolhit->PolysGet(HIT_NEST_OUTSIDE), AREA_SUM );

		m_outer_kerf_area = m_max_area;
	}
	else
	{
		m_max_area = do_calc_area(
			toolhit->PolysGet(HIT_PART_OUTSIDE), AREA_SUM );

		m_outer_kerf_area = do_calc_area(
			toolhit->PolysGet(HIT_KERF_OUTSIDE), AREA_SUM );
	}


	// Get the outside area.
#if (_NST)
	m_area = m_outer_kerf_area;
#else
	if (config.AreaOffset())
		m_area = m_outer_kerf_area;
	else
		m_area = m_max_area;
#endif

	// Now get Hole area... min and total, as needed
#if (_NST)
	inside_area = do_calc_area( (*toolhit)[HIT_NEST_INSIDE], AREA_SUM );
	m_area -= inside_area;

	min_area = inside_area;
#else
	m_min_area = do_calc_area( toolhit->PolysGet(HIT_PART_INSIDE), AREA_MIN );

	if (config.AreaLessHoles() )
	{
		if ( config.AreaOffset() )
			inside_area = do_calc_area( toolhit->PolysGet(HIT_KERF_INSIDE), AREA_SUM );
		else
			inside_area = do_calc_area( toolhit->PolysGet(HIT_PART_INSIDE), AREA_SUM );

		m_area -= inside_area;
	}

	min_area = m_extent.Area();
#endif

	config.LargePartArea( max( config.LargePartArea(), m_outer_kerf_area ) );
	config.SmallPartArea( min( config.SmallPartArea(), m_outer_kerf_area ) );

	config.LargeHoleArea( max( config.LargeHoleArea(), do_calc_area( toolhit->PolysGet(HIT_PART_INSIDE), AREA_MAX)) );
	config.SmallHoleArea( min( config.SmallHoleArea(), m_min_area ) );

	return status;
}

// ----------------------------------------
double CNestingPart::do_calc_area( CGeoPolyArray* geopoly_array, eAreaCalc calc )
{
	if ( !geopoly_array || !geopoly_array->Count() )
		return 0.;

	double totarea = 0.0;
	for (int idx=0; idx<geopoly_array->Count(); idx++)
	{
		double area = fabs( geopoly_array->GetAt(idx)->Area());
		switch (calc)
		{
		case AREA_SUM:
			totarea += area;
			break;

		case AREA_MIN:
			if ( ZERO( totarea ) || ( area < totarea ) )
				totarea = area;
			break;

		case AREA_MAX:
			totarea = max( totarea, area );
			break;
		}
	}

	return totarea;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// We strive to assign part priorities (ie. pass number) such that:
//
//   pass 0 is reserved for large parts.
//   pass 1 is reserved for "special" parts (whatever that means) :-(
//   pass 2..(2+N_non_filler_parts) is for non filler parts.
//   pass (2+N_non_filler_parts+1..?) is for filler parts.
//
// We also strive to ensure the part priority values are both
// unique and consecutive (consecutive, assuming the priorites
// were sorted).
//
// TODO: Perhaps this method should be moved into CPartBin
// and the prototype changed to accept a sorting method?
//
// PREREQUISITE: Must have called CPartBin::PartsSort()
//
// NOTE: The name ReOrder() is misleading.  This method simply
// assigns as pass number to each part in the partbin.
void CNestingPart::ReOrder( const CNestConfig& config )
{
	int	pass;

	if ( Special() )
	{
		// Dunno what this means ... or its effect.
		pass = SPECIAL_PASS;
	}
	else
	{
		pass = COMMON_PASS + config.Counter();
	}

	Pass( pass );
}

// =======================================================================
//	Rotate a model (in-place) by the given radians.
//
CReturn CNestingPart::do_rotate( CModel* model, double rotang, bool mirror )
{
	CReturn ret;

	ASSERT( model );

	if ( ZERO(rotang) && !mirror )
		return ret;

	// Select everything
	CSelectorStack& selectorStack = model->SelectorStack();
	selectorStack.Init( *model );

	CSelector& model_select = selectorStack();
	model_select.All( 1 );
	model_select.SystemFlag( FALSE );
	model_select.SelectAll(TRUE);

	C3x4Matrix	xform;
	if (mirror)
	{
		xform.Scale( 1.0, -1.0, 1.0 );

		// 2005.11.10 (PE) -- Sunflower reported that mirrored
		// parts in a nest had the wrong winding direction.
		//
		// Previously, XFORM_REV_PROFS was not used.
		// See also CModelUtil::Transform().
		ret += CModelUtil::Transform( model, xform, 0, XFORM_REV_PROFS );

		// Reset for next transform.
		xform.setUnit();
	}

	if ( !ZERO(rotang) )
	{
		xform.setXYAngle( rotang );
		ret += CModelUtil::Transform( model, xform, 0, 0 );
	}

	model_select.Clear();

	return ret;
}

CReturn CNestingPart::Generate( int hit )
{
	CReturn	status;

	// TODO: Based enabling on registry entry.
	g_nesting_part_profiler.Enable( true );

	if ( CNestConfig::IsBitmapNest() )
		status = bitmap_generate( hit );
	else
		status = exact_generate( hit );

	return status;
}

// ==================================================================
//	Create the various critical outlines necessary for nesting
//	Does this in the starting rotation only
//
//	Assumes you have run Simplify() first to minimize m_model
//
//	Creates m_toolhits geometry, EXCEPT for leads and pierces.
//	Call Accessorize() to add in lead junk.
// ==================================================================
CReturn CNestingPart::exact_generate( int hit )
{
	CReturn		status;
	CDbIterator	iter;
	CDbEntity*	dbEntity;
	CDbProfile*	dbProfile;
	CDbTool*	dbTool;

	if (m_toolhits)
		delete m_toolhits;

	m_toolhits = new CGeoPoly();

	CModel* model = m_modelarray[hit];

	g_nesting_part_profiler.In( "exact_generate" );

	// Do two passes.  First pass, we try to find any PART_OUTLINE geometry.
	// If we don't find any, do the second pass with tooling
	iter.Init( model->Db(), DBLINE );
	while ( TRUE )
	{
		dbEntity = iter();
		if (dbEntity == NULL)
			break;

		iter.Next();

		// ASSUMPTION/RULE: The "part outline" is a single closed dbProfile.
		// NOTE: This change makes a dramatic performance improvement for some
		// parts.  Discovered when dealing with sample parts from Dynatorch.
		if ( !dbEntity->IsToolpath() )
		{
			dbTool = dbEntity->Tool();
			if ( dbTool && dbTool->Name().CompareNoCase(STR_PART_OUTLINE) == 0)
			{
				dbProfile = dynamic_cast<CDbProfile*>( dbEntity->Owner() );
				if (dbProfile)
				{
					do_outline(dbProfile, hit);
					PartOutline(true);
					break;
				}
			}
		}
	}

	iter.Init( model->Db(), DBLINE );
	while ( !PartOutline() )
	{
		// Catch any user abort.
		status += CProgressWnd::ProgressWndUpdate();
		if ( !status.IsOk() )
			break;

		dbEntity = iter();
		if (!dbEntity)
			break;

		iter.Next();

		// Only converts tooled geometry, which MAY NOT LIVE in tooled features anymore.
		//	Note that we are guaranteeded NOTHING -- no profiles, no winding, nothing
		//	Test part as PunchDemo3 illustrates the horror.
		CDbProfile* profile = dynamic_cast<CDbProfile*>(dbEntity);
		CDbCurve* curve = dynamic_cast<CDbCurve*>(dbEntity);
		CDbHole* hole = dynamic_cast<CDbHole*>(dbEntity);
		CDbProfile* owner_profile = dynamic_cast<CDbProfile*>(dbEntity->Owner());
		CDbFeature* owner_feature = dynamic_cast<CDbFeature*>(dbEntity->Owner());

		if ( dbEntity->IsToolpath() )
		{
			// Skip Leads and Pierces
			bool skip = false;
			if (owner_feature && owner_feature->IsLead())
				skip = true;

			if (hole && hole->IsPierce())
				skip = true;

			// Ignore other tooling if we are using part_outline
			if (PartOutline())
				skip = true;

			// Cut it
			if (!skip)
			{
				if ( profile )
					do_toolhit(profile, false);
				else if ( ( curve || hole) && !owner_profile )
					do_toolhit(dbEntity, false);
			}
		}
	}

	g_nesting_part_profiler.Out( "exact_generate" );
	g_nesting_part_profiler.Dump( "g_nesting_part_profiler" );
	g_nesting_part_profiler.ResetAll();

	return status;
}

CReturn CNestingPart::bitmap_generate( int hit )
{
	CReturn		status;
	CDbIterator	iter;
	CGeoPolyArray	geoPolyArray;
	CGeoPoly	tmp;
	CGeoPoly*	geoPoly;
	CModel*		model;
	CToolHit*	toolhit;
	CDbFeature*	dbFeature;
	CDbProfile*	dbProfile;
	CDbCurve*	dbCurve;
	double		max_area, area;
	int			count, indx, max_indx;

	// Set up our context of information
	model = m_modelarray[hit];
	toolhit = m_toolhitarray[hit];
	if ((model == NULL) || (toolhit == NULL))
		return status.Fatal( IDS_NEST_PART_EMPTY );

	max_area = -UNDEFINED;
	max_indx = 0;

	// Do two passes.  First pass, we try to find any PART_OUTLINE geometry.
	// If we don't find any, do the second pass with tooling
	iter.Init( model->Db(), DBLINE );
	while ( TRUE )
	{
		dbCurve = dynamic_cast<CDbCurve*>( iter() );
		if (dbCurve == NULL)
			break;

		// ASSUMPTION/RULE: The "part outline" is a single closed dbProfile.
		// NOTE: This change makes a dramatic performance improvement for some
		// parts.  Discovered when dealing with sample parts from Dynatorch.
		if ( IsPartOutline( (*dbCurve) ) )
		{
			dbProfile = dynamic_cast<CDbProfile*>( dbCurve->Owner() );
			if (dbProfile != NULL)
			{
				ContainerToGeopoly( (*dbProfile), &tmp );
				PartOutline(true);
				break;
			}
		}

		iter.Next();
	}

	if ( PartOutline() )
	{
		area = tmp.Area();
		if (area > 0.)
			tmp.Reverse();  // force CW winding direction

		geoPoly = new CGeoPoly;
		PolyTransfer( &tmp, geoPoly );
		geoPolyArray.Append( geoPoly );
	}
	else
	{
		iter.Init( model->Db(), DBLINE );
		while ( TRUE )
		{
			dbCurve = dynamic_cast<CDbCurve*>( iter() );
			if (dbCurve == NULL)
				break;

			if ( IsToolPath( (*dbCurve) ) )
			{
				dbProfile = dynamic_cast<CDbProfile*>( dbCurve->Owner() );
				if (dbProfile == NULL)
				{
					dbFeature = dynamic_cast<CDbFeature*>( dbCurve->Owner() );
					if (dbFeature != NULL)
					{
						// We have a lead in/out?

						// Short-circuit next reference to this profile.
						dbFeature->IntSet( "tp", 1 );

						// Move the curves directly to geoPoly.
						geoPoly = new CGeoPoly;
						ContainerToGeopoly( (*dbFeature), geoPoly );
						geoPolyArray.Append( geoPoly );
					}
				}
				else
				{
					// Short-circuit next reference to this profile.
					dbProfile->IntSet( "tp", 1 );

					ContainerToGeopoly( (*dbProfile), &tmp );

					area = tmp.Area();
					if (area > 0.)
						tmp.Reverse();  // force CW winding direction

					// Move the curves from tmp to geoPoly.
					geoPoly = new CGeoPoly;
					PolyTransfer( &tmp, geoPoly );
					geoPolyArray.Append( geoPoly );

					area = fabs( area );
					if (area > max_area)
					{
						// ASSUMPTION: The profile having the largest
						// area is the external profile.
						max_area = area;
						max_indx = geoPolyArray.Count() - 1;
					}
				}
			}

			iter.Next();
		}
	}

	// Tabulate and segregate ....
	count = geoPolyArray.Count();
	for (indx = 0; indx < count; ++indx)
	{
		geoPoly = geoPolyArray.GetAt( indx );

		// ASSUMPTION: We have a single closed outer polyline.
		// As such, the other polylines are either interior
		// polylines or lead in/outs.  In the latter case, it
		// may not matter that we designate them as HIT_NEST_INSIDE.
		//
		// NOTE: toolhit takes ownership of geoPoly.
		if (indx == max_indx)
			toolhit->PolysGet(HIT_NEST_OUTSIDE)->Append( geoPoly );
		else
			toolhit->PolysGet(HIT_NEST_INSIDE)->Append( geoPoly );
	}

	geoPolyArray.BenignFlush();

	return status;
}

// ==================================================================
CReturn CNestingPart::GenerateBox( int hit )
{
	CReturn		ret;
	CDbIterator	iter;

	if (m_toolhits)
	{ delete m_toolhits; }
	m_toolhits = new CGeoPoly();

	CModel* model = m_modelarray[hit];
	C3dBox extent = model->BoxUser(0);

	PartOutline(true);

	CToolHit* toolhit = m_toolhitarray[hit];
	//
	// Prep the profile for offset
	//
	CProfile profile;
	profile.Append( new CGeoLine(extent.Xmin(), extent.Ymin(), extent.Xmax(), extent.Ymin()) );
	profile.Append( new CGeoLine(extent.Xmax(), extent.Ymin(), extent.Xmax(), extent.Ymax()) );
	profile.Append( new CGeoLine(extent.Xmax(), extent.Ymax(), extent.Xmin(), extent.Ymax()) );
	profile.Append( new CGeoLine(extent.Xmin(), extent.Ymax(), extent.Xmin(), extent.Ymin()) );

	int profdir = SGN(profile.Area());
	if (profdir > 0)
		profile.Reverse();

	CGeoPoly* nibblepoly = new CGeoPoly;
	int idx;
	for (idx=0; idx<profile.Count(); idx++)
	{
		CGeoCurve* curve = profile.GetAt(idx);
		nibblepoly->CopyAppend( *curve );
	}

	profile.Reverse();
	for (idx=0; idx<profile.Count(); idx++)
	{
		CGeoCurve* curve = profile.GetAt(idx);
		nibblepoly->CopyAppend( *curve );
	}

	m_toolhits->OR( *nibblepoly );
	profile.DestructiveFlush();

	return ret;
}


// ==================================================================
//	Add the lead and pierce geometry in as Kerf, NOT doing a boolean
//	on it, but just dumping it into the appropriate array.
//
//	Avoids booleans on the delicate tangency cases.
// ==================================================================
CReturn CNestingPart::Accessorize( CNestConfig& config, int hit )
{
	CReturn		ret;
	CDbIterator	iter;
	CDbHole*	pierce_hole;
	int			count, indx;

	if ( PartOutline() )  // || config.GridChain())
		return ret;

	if (m_toolhits)
		delete m_toolhits;

	CModel* model = m_modelarray[hit];
	CToolHit* toolhit = m_toolhitarray[hit];

	m_toolhits = new CGeoPoly();

	pierce_hole = NULL;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Generate lead geometry in m_toolhits.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( model->Db(), DBLINE );
	while ( TRUE )
	{
		CDbEntity* db_ent = iter();
		if (!db_ent)
			break;

		iter.Next();

		// Only converts tooled geometry, which MAY NOT LIVE in tooled features anymore.
		//	Note that we are guaranteeded NOTHING -- no profiles, no winding, nothing
		//	Test part as PunchDemo3 illustrates the horror.

		int sncs = (db_ent->IntGet("_sncs", 0) / 10000);

		if ( db_ent->IsToolpath() )
		{
			CDbFeature* owner_feature = dynamic_cast<CDbFeature*>(db_ent->Owner());

			// ASSUMPTION: Unless someone has messed up the model, lead
			// entities will always be in a feature.
			bool is_lead = (owner_feature && owner_feature->IsLead());
			bool skip = (is_lead ? false : true);

			if ( config.HardPierce() )
			{
				// Record the (outside) pierce hole for later processing.
				CDbHole* hole = dynamic_cast<CDbHole*>(db_ent);

				if (hole && hole->IsPierce() && (pierce_hole == NULL))
					pierce_hole = PierceHoleGet( (*toolhit), hole );
			}

			if (sncs == 5) // CI SNCS code category for leads
				skip = false;

			if (!skip)
			{
				CDbProfile* profile = dynamic_cast<CDbProfile*>(db_ent);

				if ( profile )
				{
					do_toolhit(profile, true);
				}
				else
				{
					CDbCurve* curve = dynamic_cast<CDbCurve*>(db_ent);
					CDbProfile* owner_profile = dynamic_cast<CDbProfile*>(db_ent->Owner());

					// 2013.12.01 (PE) -- Prior to now, 'is_lead' was not a consideration.
					// However, ITI presented a part where the leads were a simple line
					// having a length of 0.001 and this caused problems for the booleans.
					// Here it seems we can ignore individual lead entities because the
					// "lead hull" profile serves as a robust (?) substitute.
					if (curve && !owner_profile && !is_lead)
						do_toolhit( db_ent, true );
				}
			}
		}
	}

	// Now, pseudo-assemble
	CGeoPolyArray raw_prof;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// First, extract the lead profiles
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	double tab_gap = config.GapTolerance();

	static bool DBUG = false;

	while (true)
	{
		CGeoPoly* profile = m_toolhits->ExtractProfile( +1, tab_gap, false ); // +1 left, -1 right

		if (!profile)
			break;

		if (DBUG)
			profile->Draw();

		profile->TrimLoops( 3, 0.001 );	// Magic numbers.  Sorry.

		if (DBUG)
			profile->Draw();

		count = profile->Count();
		for (indx = 0; indx < count; ++indx)
		{
			CGeoCurve* crv = (CGeoCurve*) &(profile->GetAt(indx));
			crv->IntSet( "lead", 1 );
		}

		profile->IntSet( "lead", 1 );

		raw_prof.Append( profile );
	}


	CGeoPolyArray* polyarray;
	CGeoPoly* profile;
	CGeoPoly* poly;
	int depth, polynum, polyidx;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Check all mutual containments and insert profiles into relevant lists.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	polyarray = toolhit->PolysGet(HIT_PART_OUTSIDE);

	int rawnum = raw_prof.Count();
	for (int rawidx=0; rawidx<rawnum; rawidx++)
	{
		profile = raw_prof[rawidx];

		// 2005.01.25 (PE) -- See also other comments with same timestamp.
#if BEFORE_2005_01_25
		if (config.HardPierce())
			depth=HIT_PART_OUTSIDE;  // Was HIT_NEST_OUTSIDE, changed for IT685
		else
			depth=HIT_NEST_OUTSIDE;
#else
		depth = HIT_NEST_OUTSIDE;
#endif

		polynum = polyarray->Count();
		for (polyidx=polynum-1; polyidx>=0; polyidx--)
		{
			poly = polyarray->GetAt(polyidx);
			if (poly->Encloses( *profile ))
			{
				depth = (config.HardPierce() ? HIT_PART_INSIDE : HIT_NEST_INSIDE);
				break;
			}
		}

		if ((depth == HIT_NEST_OUTSIDE) && config.HardPierce())
		{
			toolhit->PolyAdd( HIT_PART_OUTSIDE, (CGeoPoly*) profile->Clone( true ) );
		}
		else if (depth == HIT_PART_INSIDE)
		{
			toolhit->PolyAdd( HIT_PART_INSIDE, (CGeoPoly*) profile->Clone( true ) );
			toolhit->PolyAdd( HIT_NEST_INSIDE, (CGeoPoly*) profile->Clone( true )  );
		}
	}

	raw_prof.DestructiveFlush();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// And now for the pierce hole (if any). This is step is done post-facto
	// because it eliminates the logic that would otherwise be necessary to
	// manage the attributing.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (pierce_hole != NULL)
	{
		// ASSUMPTION: config.HardPierce() must be true.

		// BIG ASSUMPTION: pierce_hole represents and outside pierce hole.

		m_toolhits->Flush();

		// do_toolhit guarantees the pierce hole will be converted
		// to a geopoly that consists of four quadrant arcs.
		do_toolhit( pierce_hole, true );

		profile = m_toolhits->ExtractProfile( +1, tab_gap, false ); // +1 left, -1 right

		if (DBUG)
			profile->Draw();

		//----

		// We record this information so that it can be used by
		// CSheet::exact_score() for calculating the pierce_mer.
		double radius = 0.5 * pierce_hole->Tool()->EffectiveDiameter();

		toolhit->IntSet( "lead", 2 );
		toolhit->DoubleSet( "radius", radius );
		toolhit->DoubleSet( "xc", pierce_hole->Center().X() );
		toolhit->DoubleSet( "yc", pierce_hole->Center().Y() );

		profile->IntSet( "lead", 2 );

		count = profile->Count();
		for (indx = 0; indx < count; ++indx)
		{
			CGeoCurve* crv = (CGeoCurve*) &(profile->GetAt(indx));
			crv->IntSet( "lead", 2 );
		}

		//----

		depth = HIT_NEST_OUTSIDE;

		polyarray = toolhit->PolysGet(HIT_PART_OUTSIDE);

		polynum = polyarray->Count();
		for (polyidx=polynum-1; polyidx>=0; polyidx--)
		{
			poly = polyarray->GetAt(polyidx);
			if (poly->Encloses( *profile ))
			{
				depth = (config.HardPierce() ? HIT_PART_INSIDE : HIT_NEST_INSIDE);
				break;
			}
		}

		// CRITICAL: We add the pierce to HIT_PART_OUTSIDE. Otherwise,
		// CSheet::Bump() will fail to yield correct results when it
		// checks the HIT_NEST_OUTSIDE of the candidate part against
		// the HIT_PART_OUTSIDE of the parts that are within the
		// current nesting area.
		if (depth == HIT_NEST_OUTSIDE)
		{
			toolhit->PolyAdd( HIT_PART_OUTSIDE, (CGeoPoly*) profile->Clone( true ) );
		}
		else if (depth == HIT_PART_INSIDE)
		{
			toolhit->PolyAdd( HIT_PART_INSIDE, (CGeoPoly*) profile->Clone( true ) );
			toolhit->PolyAdd( HIT_NEST_INSIDE, (CGeoPoly*) profile->Clone( true )  );
		}

		delete profile;
	}

	return ret;
}

void CNestingPart::do_toolhit( CDbProfile* db_prof, bool accessory )
{
	g_nesting_part_profiler.In( "do_toolhit( CDbProfile* db_prof, ...)" );

	if ((m_use_gap_tool > 1) || m_use_tool_extruder)
	{
		// Experimental.

		if (do_burn_test(db_prof))
		{
			do_toolhit_burn(db_prof, accessory);
		}
		else
		{
			CDbEntity*	dbEntity;
			int			count, indx;

			count = db_prof->Count();
			for (indx = 0; indx < count; ++indx)
			{
				dbEntity = db_prof->GetAt(indx);
				if (do_burn_test( dbEntity ))
					do_toolhit_burn( dbEntity, accessory );
				else
					do_toolhit_nibble( dbEntity, accessory );
			}
		}
	}
	else
	{
		// V16.5 and earlier.

		if (do_burn_test(db_prof))
			do_toolhit_burn(db_prof, accessory);
		else
			do_toolhit_nibble(db_prof, accessory);
	}

	g_nesting_part_profiler.Out( "do_toolhit( CDbProfile* db_prof, ...)" );
}

void CNestingPart::do_toolhit( CDbEntity* db_ent, bool accessory )
{
	if (do_burn_test( db_ent ))
		do_toolhit_burn( db_ent, accessory );
	else
		do_toolhit_nibble( db_ent, accessory );
}

bool CNestingPart::do_burn_test( CDbEntity* db_ent )
{
	CDbTool* tool;

	if (db_ent->Type() == DBHOLE)
		return FALSE;

	tool = db_ent->Tool();

	if (tool == NULL)
		return FALSE;

	if ( CNibbler::canNibble( *db_ent, true ) )
	{
		// We *can* nibble this, but do we want to?
		
		if ( tool->IsRoundTool() )
			return TRUE;
	}

	return ( !tool->IsPunchTool() );
}

// --------------------------------------

// NOTE: When m_use_tool_extruder is true, we still use CNibbler for
// those cases that CToolShapeExtruder does not handle.  That way,
// we can incrementally extend the implement of CToolShapeExtruder.
void CNestingPart::do_toolhit_nibble( CDbEntity* db_ent, bool accessory )
{
	CGeoPoly	nibblepoly;
	bool		use_nibbler;

	use_nibbler = true;

#if (!_CI && !_NST)  // CI should never get here anyways!
	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( db_ent );
	if (m_use_tool_extruder && (dbCurve != NULL))
	{
		CToolShapeExtruder extruder;

		CGeoCurve* geoCurve = dbCurve->Curve();

		extruder.Extrude( (*geoCurve), dbCurve->Tool()->Attrib(), &nibblepoly );

		delete geoCurve;

		if (nibblepoly.Count() > 0)
		{
			nibblepoly.UserData( (void*) (accessory ? 1 : NULL) );

			// m_toolhits->Append( nibblepoly );  // <--- use this to see the raw geometry
			m_toolhits->OR( nibblepoly );

			use_nibbler = false;
		}
	}
#endif

	if (use_nibbler)
	{
		CNibbler nibbler;
		CGeoPoly hitpoly;
		C3x4Matrix xform;
		C3dCoordList hitlist;
		CDbHole*	dbHole;
		C3dCoord* hitpt;
		CDbTool* tool;
		double	tangent;
		int		num, idx;
		bool	pierce;

		tool = db_ent->Tool();

		dbHole = dynamic_cast<CDbHole*>( db_ent );
		pierce = (dbHole && dbHole->IsPierce());

		nibbler.Nibble( *db_ent, true, &hitlist );
		num = hitlist.Count();
		for (idx=0; idx < num; ++idx)
		{
			if (pierce)
			{
				CGeoArc arc;
				C3dCoord pc;
				double	radius;

				pc.XYZ( 0., 0., 0. );
				radius = (0.5 * tool->EffectiveDiameter()) + m_kerf_adjust;
				arc.Init( pc, radius, CCW );

				hitpoly.CopyAppend( arc );
			}
			else
			{
				tool->Convert( &hitpoly );
			}

			if (hitpoly.Winding() > 0)
				hitpoly.Reverse();

			hitpt = hitlist[idx];

			tangent = (tool->IsIndexable() ? hitpt->Z() : 0.);
			hitpt->Z( 0.0 );

			xform.setXYAngle( tangent );
			xform.setT( *hitpt );

			hitpoly.Xform( xform );

			nibblepoly.OR( hitpoly );
			nibblepoly.Reduce();

			hitpoly.Flush();
		}

		nibblepoly.UserData( (void*) (accessory ? 1 : NULL) );

		// m_toolhits->Append( nibblepoly );  // <--- use this to see the raw geometry
		m_toolhits->OR( nibblepoly );
		if (pierce)
			m_toolhits->IntSet( "lead", 2 );

		hitlist.DestructiveFlush();
	}
}

// --------------------------------------

void CNestingPart::do_toolhit_burn( CDbContainer* dbContainer, bool accessory )
{
	CProfile profile;

	// ASSUMES tool center toolpath.
	CDbTool* tool = dbContainer->Tool();

	g_nesting_part_profiler.In( "do_toolhit_burn( CDbContainer* dbContainer, ...)" );

	// Prep the profile for offset
	int count = dbContainer->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbContainer->GetAt( indx );

		// 'sncs' appears to be CI-related. Can all refs to it be deleted (?)
		int sncs = (dbEntity->IntGet("_sncs", 0) / 10000);
		if ( (sncs == 5) && (!accessory) )
			continue;

		CDbCurve* dbCurve = dynamic_cast<CDbCurve*>(dbEntity);

		CGeoCurve* geoCurve = dbCurve->Curve();

		profile.Append( geoCurve );
	}

	// Force coincidence of the start/end of profiles (eliminates both gaps,
	// and more importantly, overlaps).
	if ((profile.Count() > 0) &&
		!profile.IsClosed(SMALL) &&
		!accessory &&
		!tool->IsPunchTool() )	// Punched ones may be OPEN... so don't fuck with 'em
	{
		int num = profile.Count()-1;
		CGeoCurve* first = profile.GetAt(0);
		CGeoCurve* last = profile.GetAt(num);

		// This is just a hack to get nesting through the day.
		// Any minor deviations because there is a gap in the CORNER
		// of the part should be absorbed by the web thickness.
		if (last->Type() == GEOLINE)
		{
			last->EndPt(first->StartPt());
		}
		else
		if (first->Type() == GEOLINE)
		{
			first->StartPt(last->EndPt());
		}
		else // Both DBARC
		{
			CGeoArc* st_arc = (CGeoArc*)first;
			CGeoArc* en_arc = (CGeoArc*)last;

			C3dCoord hit[2];
			int num = CSolution::Intersect( *st_arc, *en_arc, true, hit );
			if (num)
				last->StartPt(hit[0]);
		}
	}

	CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbContainer );
	if ((dbProfile != NULL) && dbProfile->IsLeadHull())
	{
		int lead_hull = dbProfile->IntGet( STR_LEAD_HULL, 0 );
		profile.IntSet( STR_LEAD_HULL, lead_hull );
	}

	g_nesting_part_profiler.Out( "do_toolhit_burn( CDbContainer* dbContainer, ...)" );

	do_toolhit_burn( &profile, tool, accessory );
}

void CNestingPart::do_toolhit_burn( CDbEntity* db_ent, bool accessory )
{
	// ASSUMES tool center toolpath.
	CDbTool* tool = db_ent->Tool();

	// Now, what do we have here?
	CDbCurve* db_curve = dynamic_cast<CDbCurve*>(db_ent);
	if (db_curve != NULL)
	{
		// Prep the profile for offset
		CProfile profile;
		CGeoCurve* curve = db_curve->Curve();

		profile.Append( curve );

		do_toolhit_burn( &profile, tool, accessory );
	}
}

void CNestingPart::do_toolhit_burn( CProfile* profile, CDbTool* tool, bool accessory )
{
	double	sharp_ang, tool_rad;
	double	left_dist, right_dist;
	int		profdir, code_part_prof;

	// 2005.01.27 (PE) -- Through various circumstances, CI could
	// pass any empty profile to this function, causing a crash.
	// And though the client could check for an empty profile
	// before calling this method, we take the more defensive
	// solution and protect the client from himself.
	if (profile->Count() < 1)
		return;

	g_nesting_part_profiler.In( "do_toolhit_burn( CProfile* profile, ...)" );

#if (_NST)
	sharp_ang = 135. * DEG2RAD;
#else
	sharp_ang = 180. * DEG2RAD;
#endif

	// For a lead-hull, we set the tool radius to a small value because
	// we don't want the downstream kerf offset to represent much more
	// than just the part spacing (web distance).
	// Values for STR_LEAD_HULL (-1) applied to outside profile / (1) applied to inside profile.
	int isLeadHull = profile->IntGet( STR_LEAD_HULL, 0 );

	if ( isLeadHull )
	{
		// 2012.06.20 (PE) -- A magic value. The previous value of 1.e-4
		// causes problems for the booleans and I ain't going there unless
		// I'm paid to solve that long-standing problem. Even then, it could
		// take months of effort to rewrite the boolean code and to make it
		// more robust.
		//   tool_rad = 1.e-4;
		tool_rad = 1.e-3;
	}
	else
	{
		tool_rad = tool->EffectiveDiameter() / 2.0;

		if ( CNestConfig::CcsNesting() )
			profile->Clean();
	}

	left_dist = tool_rad;
	right_dist = tool_rad;

	profdir = SGN(profile->Area());
	if (!profdir)
		profdir = -1;	// Actually, profdir doesn't really matter! (?)

#if BEFORE_2008_01_12
#else
	// (PE) -- While "fixing" remnant generation, I realized the
	// profiles represented by HIT_NEST_OUTSIDE (wrongly) varied
	// as a function of 'code_part_prof'.
	code_part_prof = Model().Header().getInt( "PartProf", 0 );
	if ( code_part_prof )
	{
		if (profdir < 0)
		{
			// ASSUMPTION: This is the outside of a part (CW).
			left_dist = 2. * tool_rad;
			right_dist = 0.;
		}
		else
		{
			// ASSUMPTION: This is the inside of a part (CCW).
			left_dist = 0.;
			right_dist = 2. * tool_rad;
		}
	}
#endif

	if (CRegister::IntGetV( "Nesting", "reorder", 0 ) != 0)
	{
		double lower_bound = 2 * (tool_rad + SMALL);

		int indx, count = profile->Count();
		for (indx = 0; indx < count; ++indx)
		{
			const CGeoCurve* geoCurve = profile->GetAt( indx );

			double len = geoCurve->Length2d();
			if (len >= lower_bound)
				break;
		}

		if (indx < count)
		{
			profile->Split( indx );
			profile->Reorder( indx+1 );
		}
	}

	// Now create two offsets, left and right
	// TODO:  Put into array and do offsets in a loop?
	CGeoPoly nibblepoly;
	nibblepoly.AttribsPropogate( true );

	// 2013.12.01 (PE) -- NOTE: It is perhaps incorrect to do an early exit
	// when either 'left_prof' or 'right_prof' (below) is NULL because doing
	// an early exit prevents the 'accessory' branch from being executed.

	g_nesting_part_profiler.In( "ProfOffset( left )" );

	CProfileList left_offset;
	CWorm::ProfOffset( (*profile), -profdir, left_dist, sharp_ang, FALSE, &left_offset );

	g_nesting_part_profiler.Out( "ProfOffset( left )" );

	if (!left_offset.Count())
		return;

	CProfile* left_prof = left_offset[0];
	if (profdir > 0)
		left_prof->Reverse();

	int idx;
	for (idx=0; idx<left_prof->Count(); idx++)
	{
		CGeoCurve* curve = left_prof->GetAt(idx);
		nibblepoly.CopyAppend( *curve );
	}

	if (0)  // experiment with piecemeal approach 2014.02.02
	{
		m_toolhits->OR( nibblepoly );
		nibblepoly.Flush();
	}

	// --------------------------------------
	g_nesting_part_profiler.In( "ProfOffset( right )" );

	CProfileList right_offset;
	CWorm::ProfOffset( (*profile), profdir, right_dist, sharp_ang, FALSE, &right_offset );

	g_nesting_part_profiler.Out( "ProfOffset( right )" );

	if (!right_offset.Count())
		return;

	CProfile* right_prof = right_offset[0];	// Only use first curve for now
	if (profdir < 0)
		right_prof->Reverse();

	for (idx=0; idx<right_prof->Count(); idx++)
	{
		CGeoCurve* curve = right_prof->GetAt(idx);
		nibblepoly.CopyAppend( *curve );
	}

////// TODO: When don't we use nibblepoly.Append() instead of nibblepoly.CopyAppend() ?
////// The former can be used if we adjust the following bit of code.

	g_nesting_part_profiler.In( "accessory" );

	if (accessory)
	{
		// Different handling of open vs. closed profiles.
		// Open profiles need end-caps... closed profiles have complementary
		// windings.
		if (!profile->IsClosed())
		{
			CGeoArc arc;
			C3dCoord pc;

			int count = left_prof->Count() - 1;
			const C3dCoord& psA = left_prof->GetAt(count)->EndPt();
			const C3dCoord& peA = right_prof->GetAt(0)->StartPt();
			pc.XYZ( ((psA.X() + peA.X()) / 2), ((psA.Y() + peA.Y()) / 2), psA.Z() );
			arc.Init( psA, peA, pc, CW );
			if (!arc.StartPt().WithinTolXY(arc.EndPt(), SMALL))
			{
				nibblepoly.CopyAppend( arc );
			}

			count = right_prof->Count() - 1;
			const C3dCoord& psB = right_prof->GetAt(count)->EndPt();
			const C3dCoord& peB = left_prof->GetAt(0)->StartPt();
			pc.XYZ( ((psB.X() + peB.X()) / 2), ((psB.Y() + peB.Y()) / 2), psB.Z() );
			arc.Init( psB, peB, pc, CW );
			if (!arc.StartPt().WithinTolXY(arc.EndPt(), SMALL))
			{
				nibblepoly.CopyAppend( arc );
			}
		}

		nibblepoly.UserData( (void*)1 );
	}
	else
	{
		// Actually, while the end caps are more accurately descriptive as arcs, they will ALSO
		// work as lines, though they may give a somewhat more jaggy outside profile.
		// As a bonus, lines are more robust.  And faster.
		//
		if (!profile->IsClosed())
		{
			CGeoLine line;

			int count = left_prof->Count() - 1;
			const C3dCoord& psA = left_prof->GetAt(count)->EndPt();
			const C3dCoord& peA = right_prof->GetAt(0)->StartPt();
			line.StartPt(psA);
			line.EndPt(peA);

			nibblepoly.CopyAppend( line );

			count = right_prof->Count() - 1;
			const C3dCoord& psB = right_prof->GetAt(count)->EndPt();
			const C3dCoord& peB = left_prof->GetAt(0)->StartPt();
			line.StartPt(psB);
			line.EndPt(peB);

			nibblepoly.CopyAppend( line );
		}

		nibblepoly.UserData( (void*)NULL );
	}

	g_nesting_part_profiler.Out( "accessory" );

#if (_NST)
	m_toolhits->Append( nibblepoly );
#else
	if ( isLeadHull )
	{
		int count = nibblepoly.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CGeoCurve& crv = (CGeoCurve&) nibblepoly.GetAt(indx);
			crv.IntSet( STR_LEAD_HULL, isLeadHull );
		}
	}

	m_toolhits->AttribsPropogate( nibblepoly.AttribsPropogate() );

	// 'use_booleans' is an optimization introduced in V21 to address a
	// performance issue encountered by Connweld where nesting a part took
	// a very long time; the part contained 136 internal obround profiles.
	bool use_booleans = true;

	CNestConfig& config = NestConfigGet();
	if (!config.PartInPart() && !config.UseBooleans())
	{
		// Ideally, booleans should be required only by punch-only parts but,
		// unfortunately, nesting yields off-of-the-sheet contoured-parts if
		// we fail to boolean the leads to the outside profile
		use_booleans = (isLeadHull == -1);
	}

	if ( use_booleans )
	{
		g_nesting_part_profiler.In( "use_booleans" );
		m_toolhits->OR( nibblepoly );
		g_nesting_part_profiler.Out( "use_booleans" );
	}
	else
	{
		g_nesting_part_profiler.In( "no booleans" );
		m_toolhits->CopyAppend( nibblepoly );
		g_nesting_part_profiler.Out( "no booleans" );
	}
#endif

	left_offset.DestructiveFlush();
	right_offset.DestructiveFlush();

	g_nesting_part_profiler.Out( "do_toolhit_burn( CProfile* profile, ...)" );
}

// ==================================================================

void CNestingPart::do_outline( CDbProfile* db_profile, int hit )
{
	CToolHit* toolhit = m_toolhitarray[hit];
	//
	// Prep the profile for offset
	//
	CProfile profile;
	int idx;
	for (idx=0; idx<db_profile->Count(); idx++)
	{
		CDbEntity* db_ent = db_profile->GetAt(idx);
		CDbCurve* db_curve = dynamic_cast<CDbCurve*>(db_ent);

		CGeoCurve* curve = db_curve->Curve();
		profile.Append( curve );
	}

	int profdir = SGN(profile.Area());
	if (profdir > 0)
		profile.Reverse();


	CGeoPoly* nibblepoly = new CGeoPoly;
	for (idx=0; idx<profile.Count(); idx++)
	{
		CGeoCurve* curve = profile.GetAt(idx);
		nibblepoly->CopyAppend( *curve );
	}

	profile.Reverse();
	for (idx=0; idx<profile.Count(); idx++)
	{
		CGeoCurve* curve = profile.GetAt(idx);
		nibblepoly->CopyAppend( *curve );
	}

	m_toolhits->OR( *nibblepoly );
	profile.DestructiveFlush();
}



// ==================================================================
//	Take the model in the first slot and pre-rotate it.  THEN,
//	continue to rotate and copy it into the additional model slots.
//
//	Assumes the model has been simplified.  ALSO assumes that the
//	 model array is empty except for slot 0.
// ==================================================================
CReturn CNestingPart::Rotate()
{
	CReturn status;
	CDbIterator iter;
	double hole_tol;

	CModel* model = m_modelarray[0];

	double delta_ang = 0.0;
	if (m_rotation > 1)
	{
		// This is the most commonly used branch.
		// ie. (2) (0, 180), (3) (0, 120, 240), (4) (0, 90, 180, 270), etc.
		delta_ang = (TWOPI / m_rotation);
	}
	else if (m_rotation < -1)
	{
		// This branch was introduced for ITI.
		// ie. (-2) (0, 90), (-3) (0, 45, 90), (-4) (0, 30, 60, 90), etc.
		delta_ang = (HALFPI / (abs(m_rotation) - 1));
		m_startang = 0.0;
	}
	else
	{
		// Override.
		m_rotation = 1;
	}

	int fnum = ((m_mirror) ? 2 : 1);

	// First, pre-rotate the base model
	double startangle = DEG2RAD * m_startang;

	status += do_rotate( model, startangle, m_startmirror );

	m_toolhitarray.DestructiveFlush();

	CToolHit* toolhit = new CToolHit();
	toolhit->Part( this );
	toolhit->Index( 0 );
	toolhit->Rotation( startangle );
	toolhit->Mirror( m_startmirror );

	m_toolhitarray.Append( toolhit );

	// Now, copy and rotate the model into the remaining slots
	double nrotations = abs( m_rotation );
	double angle = 0.;
	for (int idx = 0; idx < nrotations; idx++)
	{
		for (int fidx = 0; fidx < fnum; fidx++)
		{
			if (!idx && !fidx)
				continue;

			CModel* spin = new CModel();
			spin->Init( dummy_stack, dummy_tree );
			spin->UndoBufferSuppress();

			m_modelarray.Append( spin );
			model->EntityPrepareCopy( spin );

			bool mirror = (fidx==1);

			toolhit = new CToolHit();
			int hitnum = m_toolhitarray.Append( toolhit );

			toolhit->Part( this );
			toolhit->Index( hitnum-1 );
			toolhit->Rotation( startangle + angle );
			toolhit->Mirror( mirror );


			// Copy model[0] into spin and add to the model array
			iter.Init( model->Db(), DBTOOL );
			while (TRUE)
			{
				CDbTool* db_tool = dynamic_cast<CDbTool*>(iter());
							if (db_tool == NULL)
								break;

				CDbEntity* new_ent = NULL;
				model->EntityCopy( *db_tool, &new_ent );

							iter.Next();
			}

			iter.Init( model->Db(), DBWORKPLANE );
			while (TRUE)
			{
				CDbEntity* db_ent = iter();
				if (db_ent == NULL)
					break;

				CDbEntity* owner = db_ent;
				while (owner->Owner())
				{
					owner = owner->Owner();
				}

				CDbEntity* new_ent = NULL;
				model->EntityCopy( *owner, &new_ent );

				iter.Next();
			}

			// Now rotate the spun model in place
			do_rotate( spin, angle, mirror );

			// 2006.10.23 (PE) -- We must propogate these values. Otherwise,
			// some leads on rotated parts may be created without a pierce hole.
			hole_tol = model->Header().getReal( "HolePlusTol", 1.e-3 );
			spin->pHeader()->setReal( "HolePlusTol", hole_tol );
			
			hole_tol = model->Header().getReal( "HoleMinusTol", 1.e-3 );
			spin->pHeader()->setReal( "HoleMinusTol", hole_tol );
		}

		angle += delta_ang;
	}

	return status;
}

CReturn CNestingPart::Translate()
{
	CReturn status;

	int num = m_modelarray.Count();
	for (int idx=0; idx<num; idx++)
	{
		CModel* model = m_modelarray[idx];
		do_transform( model );
	}

	return status;
}

// ==================================================================
//	Apply leads to all models in the array, according to the
//	rules specified.
//
//	Assumes the model has been simplified and rotated (though it will
//	still work if not rotated).
// ==================================================================
CReturn CNestingPart::Lead( CNestConfig& config )
{
	CReturn status;

	if (config.LeadSetup() > 0)
	{
		int num = m_modelarray.Count();
		for (int idx=0; idx<num; idx++)
		{
			CModel* model = m_modelarray[idx];

			LeadEntitiesDelete( model );

			CSelector selector(*model);

			selector.All( 1 );
			selector.SystemFlag( FALSE );
			selector.SelectAll(TRUE);

			C3dBox pre_ext = model->BoxUser(0);

			CLead lead(*model);
			lead.PartName( Name() );

			// TODO: Should we really be ignoring the return status of CLead::Auto() (?)
			lead.Auto( config.CmdbPath(), config.LeadSetup(),
				selector, TRUE, TRUE, config.LeadRestrictions() );
		}
	}

	return status;
}

// ==================================================================
//	Assemble the toolhits into classified, organized, and sorted
//	profiles.
//
//	Assumes you have run Generate() first to create m_toolhits geometry
// ==================================================================
CReturn CNestingPart::Assemble( const CNestConfig& config, int hit )
{
	CReturn ret;

	CGeoPolyArray prof_array;
	CGeoPolyArray raw_prof;

	static bool DBUG = false;

	// 2006.10.31 (PE) -- Prior to this date (ie. V17), the tab_gap was
	// set by GapTolerance(). This caused failures in some punch-only
	// parts after we introduced CHPartOutlineCreate(). Now, since a
	// "part profile" is supposed to be closed and C0 continuous, we
	// adjust the gap tolerance accordingly.
	double tab_gap = (PartOutline() ? 1.e-3 : config.GapTolerance());

	bool okay_to_append = true;

	// First, extract the profiles
	while (true)
	{
		CGeoPoly* profile = m_toolhits->ExtractProfile( +1, tab_gap, false ); // +1 left, -1 right
		if (!profile)
			break;

		if (0)
			profile->Dump();

		if ( DBUG )
			profile->Draw();

		profile->TrimLoops( 3, 0.001 );	// Magic numbers.  Sorry.

		if (0)
			profile->Dump();

		if ( DBUG )
			profile->Draw();

		if ( config.CcsNesting() )
		{
			// v20.0.85.1 -- An ITI part would not nest. In fact, it was tagged as
			// a problematic part (see the next call to config.CcsNesting() below)
			// and inadvertantly caused a crash downstream. Upon further inspection,
			// I noticed ExtractProfile() was returning a couple of very short
			// line segments. This might be expected in a punching situation but
			// not a burning situation and so now the offending line segments are
			// simply discarded. A more appropriate solution is to prevent the
			// line segments from being created at all but the offsetter is the
			// the likely source ... and I'm not getting paid to do this ... so
			// the solution employed here is low-cost and pragmatic.
			okay_to_append = profile->IsClosed( 1.e-4 );  // arbitrary tolerance
		}

		if ( okay_to_append )
			raw_prof.Append( profile );
		else
			delete profile;
	}
	//
	// Check all mutual containments and insert profiles into relevant
	// lists.  Not combined with above for clarity.
	//
	CToolHit* toolhit = m_toolhitarray[hit];

	int rawnum = raw_prof.Count();
	for (int rawidx=0; rawidx<rawnum; rawidx++)
	{
		CGeoPoly* profile = raw_prof[rawidx];

		// Determine the containment relationship of the current profile.
		int depth=0;
		for (int test = HIT_PART_INSIDE; test >= HIT_NEST_OUTSIDE; --test)
		{
			CGeoPolyArray* polyarray = toolhit->PolysGet(test);
			CGeoPolyArray* subarray = toolhit->PolysGet(test+1);

			int polynum = polyarray->Count();
			for (int polyidx=polynum-1; polyidx>=0; polyidx--)
			{
				CGeoPoly* poly = polyarray->GetAt(polyidx);

				// More robust if we KNOW we are nested profiles
				if (poly->Overlaps( *profile ))
				{
					depth = test+1;
					test = -1;
					break;
				}
				else if (profile->Encloses( *poly ))
				{
					depth = test;
					polyarray->Remove( polyidx );
					subarray->Append( poly );
				}

			}
		}

#if 0
		// Insert the profile into its corresponding poly array.
		if ((depth >= HIT_NEST_OUTSIDE) && (depth <= HIT_NEST_INSIDE) )
		{
			CGeoPolyArray* polyarray = toolhit->PolysGet(depth);
			polyarray->Append( profile );

			// 2009.08.08 (PE) -- ITI attempted to nest a part whose outside
			// profile had self-interections. (Assumption) This affected the
			// winding direction calculation and so the toolpath was generated
			// on the (perceived) inside of the part. Furthermore, because of
			// the self-intersections, the toolpath bifurcated, yielding more
			// than one outside profile. This is (of course) not physically
			// possible when burning a part. It is possible, however, when
			// punching a part because the outside profile can be realized by
			// more than one tool (we'll address that case whenever it arises).
			if ( config.CcsNesting() )
			{
				int count = polyarray->Count();
				if ((depth == HIT_PART_OUTSIDE) && (count > 1))
				{
					CReturn status;
					status.User( IDS_FAILED_PART_OUTSIDE_PROF );
					Error( status.LastErrorMsg() );
					if (0)
					{
						// For debugging ....
						for (int indx = 0; indx < count; ++indx)
						{
							CGeoPoly* poly = polyarray->GetAt( indx );
							poly->Draw();
						}
					}
					break;
				}
			}
		}
#else
		// Insert the profile into its corresponding poly array.
		if ((depth >= HIT_NEST_OUTSIDE) && (depth <= HIT_NEST_INSIDE) )
		{
			CGeoPolyArray* polyarray = toolhit->PolysGet(depth);

			bool append = true;
			if ( config.CcsNesting() )
			{
				int count = polyarray->Count();
				if ((depth == HIT_PART_OUTSIDE) && (count > 0))
				{
					CGeoPoly* stored = polyarray->GetAt(0);

					double profileArea = profile->Area();
					double storedArea = stored->Area();
					if (fabs(profileArea) > fabs(storedArea))
						polyarray->Replace( 0, profile );

					// We already have the one we want. It maybe
					// the original or it may have been replaced.
					append = false;
				}
			}
			
			if ( append )
				polyarray->Append( profile );
		}
#endif
	}

	// At this point, HIT_NEST_OUTSIDE simply represents the true outside
	// kerf and does *not* encorporate the web spacing. Likewise for
	// HIT_NEST_INSIDE,  it simply represents the true inside kerf and
	// does *not* encorporate the web spacing. The web spacing is added
	// later by CNestingPart::AddSpace().
	if ( !Error() )
	{
		KerfCopy( hit, HIT_NEST_OUTSIDE );
		KerfCopy( hit, HIT_NEST_INSIDE );
	}

	return ret;
}

CReturn CNestingPart::AddSpace( const CNestConfig& config, int hit, int depth )
{
	CReturn	ret;
	double	sharp_ang;

	CGeoPolyArray* poly_array = m_toolhitarray[hit]->PolysGet(depth);

	if ( !poly_array || !poly_array->Count() )
		return ret;

#if (_NST)
	sharp_ang = 135. * DEG2RAD;
#else
	sharp_ang = 180. * DEG2RAD;
#endif

	// Sigh, the tedious work of shifting data formats
	CProfile profile;
	for (int pidx=0; pidx<poly_array->Count(); pidx++)
	{
		CGeoPoly* poly = poly_array->GetAt( pidx );

		for (int idx=0; idx<poly->Count(); idx++)
		{
			const CGeoCurve& curve = poly->GetAt( idx );
			profile.Append( (CGeoCurve*)&curve );		// We retain ownership of the curve
		}

		// Do the actual offsetting
		CProfileList offset_list;
		CWorm::ProfOffset( profile, +1, config.Spacing(), sharp_ang, FALSE, &offset_list );

		profile.BenignFlush();
		if (offset_list.Count())
		{ 
			poly->Flush();

			for (int oidx=0; oidx<offset_list.Count(); oidx++)
			{
				CProfile* profile = offset_list.GetAt( oidx );

				for (int idx=0; idx<profile->Count(); idx++)
				{
					poly->CopyAppend( *(profile->GetAt(idx)) );
				}
			}
			offset_list.DestructiveFlush();
		}
	}
	
	return ret;
}

double CNestingPart::TestResolution(
	const CNestConfig&	config, 
	double				res,
	CViewMgr&			view )
{
	CString msg;
	CReturn status;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CToolHit* toolhit = m_toolhitarray[0];
	if (toolhit == NULL)
		return res;  // This indicates a problem upstream.

	CGeoPolyArray* kerf_array = toolhit->PolysGet(HIT_NEST_OUTSIDE);
	if ((kerf_array == NULL) || (kerf_array->Count() < 1))
		return res;  // This indicates a problem upstream.

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	double min_res = config.Resolution();
	int attempts = (int) (res / min_res);

	if ( EWMNestingAllow() )
	{
		msg.Format( "min_res <%f>  res <%f>  attempts <%d>", min_res, res, attempts );
		EWMNesting( msg );
	}

	double residual = res - (min_res * attempts);
	if (residual > 1.e-3)  // arbitrary tolerance
		++attempts;

	res = min_res * attempts;

	bool usable = false;
	while (1)
	{
		if ( EWMNestingAllow() )
		{
			msg.Format( "attempts <%d>", attempts );
			EWMNesting( msg );
		}

		usable = TestResolution_core( config, res, view );
		if ( usable )
			break;

		--attempts;
		if (attempts == 0)
			break;

		res /= 2.;
	}

	return res;
}

// ==================================================================
//	Do a trial slice of the part at this resolution.  If we don't
//	get any MARK_BODY hits, fail.
bool CNestingPart::TestResolution_core(
	const CNestConfig&	config, 
	double				res,
	CViewMgr&			view )
{
	CReturn status;
	bool	good = false;

	EWMNesting( "...TestResolution_core()" );

	// Set up our context of information
	CToolHit* toolhit = m_toolhitarray[0];
	CGeoPolyArray* kerf_array = toolhit->PolysGet(HIT_NEST_OUTSIDE);

	// ------------------------------------------------------------------------------
	// Create the grid
	//
	C2dBox extent;
	int num = kerf_array->Count();
	for (int idx=0; idx<num; idx++)
	{
		CGeoPoly* kerf = kerf_array->GetAt(idx);
		C2dBox sub_ext = kerf->Extent();
		if (sub_ext.IsDefined())
			extent += sub_ext;
	}

	CGrid* grid = new CGrid;
	if (!grid)
		return true; 	// Yeah it sucks, but not what we are testing for

	status = grid->Create( extent, C2dCoord( extent.Xc(), extent.Yc() ), res );
	if ( status.IsOk() )
	{
		// fill_mark is one of (MARK_FILL_OKERF .. MARK_FILL_HOLE)
		int fill_mark = MARK_FILL_OKERF;

		// ------------------------------------------------------------------------------
		// Slice into this "normal" grid, because that's a much easier form to work with
		//
		int limit = (config.PartInPart() ? HIT_NEST_INSIDE : HIT_PART_OUTSIDE);
		for (int depth = HIT_NEST_OUTSIDE; depth <= limit; ++depth)
		{
			CGeoPolyArray* geopoly_array = toolhit->PolysGet(depth);
			if ( !geopoly_array	|| !geopoly_array->Count() )
				continue;

			for (int idx=0; idx<geopoly_array->Count(); idx++)
			{
				CGeoPoly* poly = geopoly_array->GetAt(idx);

				grid->Slice( (*poly), fill_mark );
				grid->Fill( config, fill_mark );
			}

			fill_mark <<= 1;
		}

		if ( config.DebugBmp() )
		{
			grid->Draw(view, 50, 50, true, true);
		}

		// ------------------------------------------------------------------------------
		// Now, did we get any good, juicy MARK_BODY ???
		//
	const int MIN_PIXEL_VALID = 30;

	//	int pixel_min = max( grid->Width(), grid->Height() ) * 2;
	//	pixel_min = max(MIN_PIXEL_VALID, pixel_min);

		good = grid->Verify(MARK_BODY, MIN_PIXEL_VALID);
		if ( config.DebugBmp() )
		{
			grid->Draw(view, 50, 50, true, true);
		}
	}

	delete grid;

	EWMNesting( "...TestResolution_core() completed" );

	return good;
}

// ==================================================================
CReturn CNestingPart::Slice( 
	CNestConfig&	config, 
	CViewMgr&		view,
	int				hit )
{
	CReturn	status;

	if ( CNestConfig::IsBitmapNest() )
		status = bitmap_slice( config, view, hit );
	else
		status = exact_slice( config, view, hit );

	return status;
}

CReturn CNestingPart::exact_slice( 
	CNestConfig&	config, 
	CViewMgr&		view,
	int				hit )
{
	CReturn status;

	// Set up our context of information
	CToolHit* toolhit = m_toolhitarray[hit];
	if (!toolhit)
		return status.Fatal( IDS_NEST_PART_EMPTY );

	CGeoPolyArray* kerf_array = toolhit->PolysGet(HIT_NEST_OUTSIDE);
	if ( !kerf_array || !kerf_array->Count() )
		return status.Fatal( IDS_NEST_PART_EMPTY );

	// ------------------------------------------------------------------------------
	// Create the starting grid
	//
	double max_area = 0.0;
	int max_idx = -1;

	// This is bullshit; for some reason, in one part, in one case, HIT_PART_OUTSIDE
	// is giving a larger extent than HIT_NEST_OUTSIDE.  This fix is temporary....
	// @todo REVERT to single depth test at HIT_NEST_OUTSIDE.
	C2dBox extent;
	int depth;
	for (depth = HIT_NEST_OUTSIDE; depth <= HIT_NEST_INSIDE; ++depth)
	{
		CGeoPolyArray* kerf_array = toolhit->PolysGet(depth);
		if (kerf_array != NULL)
		{
			int num = kerf_array->Count();
			for (int idx=0; idx<num; idx++)
			{
				CGeoPoly* kerf = kerf_array->GetAt(idx);
				C2dBox sub_ext = kerf->Extent();
				if (sub_ext.IsDefined())
					extent += sub_ext;
			}
		}
	}

	CGrid* grid = new CGrid;
	if (!grid)
		return status.Fatal( IDS_MEM_ALLOC_FAILURE, "CNestingPart::exact_slice(#1)" );

	status += grid->Create( extent,
						C2dCoord( extent.Xc(), extent.Yc() ),
						config.Resolution() );

	// ------------------------------------------------------------------------------
	// Slice into this "normal" grid, because that's a much easier form to work with
	//
	int limit = (config.PartInPart() ? HIT_PART_INSIDE : HIT_PART_OUTSIDE);
	for (depth = HIT_NEST_OUTSIDE; depth <= limit; ++depth)
	{
		CGeoPolyArray* geopoly_array = toolhit->PolysGet(depth);
		if ((geopoly_array != NULL) && (geopoly_array->Count() > 0))
		{
			// fill_mark is one of (MARK_FILL_OKERF .. MARK_FILL_HOLE)
			int fill_mark = (MARK_FILL_OKERF << depth);

			for (int idx=0; idx<geopoly_array->Count(); idx++)
			{
				CGeoPoly* poly = geopoly_array->GetAt(idx);
				status += grid->Slice( (*poly), fill_mark );
				status += grid->Fill( config, fill_mark );
				grid->Debug( "exact_slice.txt" );
			}
		}
	}

	// ------------------------------------------------------------------------------
	// We use a -fill_mark to force the toolpath onto the edges. Without this,
	// failures occur in CDexGrid::TestCore().
	for (depth = 0; depth <= limit; depth += 2)
	{
		CGeoPolyArray* geopoly_array = toolhit->PolysGet(depth);
		if ((geopoly_array != NULL) && (geopoly_array->Count() > 0))
		{
			// fill_mark is one of (MARK_FILL_OKERF .. MARK_FILL_HOLE)
			int fill_mark = (MARK_FILL_OKERF << depth);

			for (int idx=0; idx<geopoly_array->Count(); idx++)
			{
				CGeoPoly* poly = geopoly_array->GetAt(idx);

				status += grid->Slice( (*poly), -fill_mark );
				status += grid->Fill( config, fill_mark );
				grid->Debug( "exact_slice.txt" );
			}
		}
	}

	grid->Clean();
	grid->Debug( "exact_slice.txt" );

	// ------------------------------------------------------------------------------
	// Convert to dexgrid for memory savings
	//
	CDexGrid* dexgrid = new CDexGrid( *grid );
	delete grid;

	toolhit->Grid( dexgrid );

	if ( config.DebugBmp() )
	{
		dexgrid->Debug( CString("exact_slice.txt") );

		dexgrid->Draw( view, 0, 0, FALSE, TRUE );
		dexgrid->Draw( view, 0, 0, TRUE, FALSE );
	}

	return status;
}

CReturn CNestingPart::bitmap_slice(		// DYNATORCH
	CNestConfig&	config, 
	CViewMgr&		view,
	int				hit )  // ie. orientation index
{
	CReturn		status;
	CGeoPolyArray*	geoPolyArray;
	CGeoPoly*	geoPoly;
	C2dBox		extents;
	C2dCoord	pc;
	C2dCoord	interior_pt;
	CModel*		model;
	CToolHit*	toolhit;
	CGeoCurve*	geoCurve;
	CGrid*		grid;
	CDexGrid*	dexgrid;
	double		spacing;
	int			count, indx;
	int			jcnt, jndx;

	// Set up our context of information
	model = m_modelarray[hit];
	toolhit = m_toolhitarray[hit];
	if ((model == NULL) || (toolhit == NULL))
		return status.Fatal( IDS_NEST_PART_EMPTY );

	// As generated by CNestingPart::bitmap_generate().
	geoPolyArray = toolhit->PolysGet(HIT_NEST_OUTSIDE);

	count = geoPolyArray->Count();
	if (count > 0)
	{
		// Calculate the overall bounding box since this
		// is used to setup our maximum grid space.
		for (indx = 0; indx < count; ++indx)
		{
			geoPoly = geoPolyArray->GetAt( indx );
			extents += geoPoly->Extent();
		}

		spacing = config.Spacing();

		// We add the spacing to the part so that when it
		// is punched into the sheet, we can tag the spacing
		// area with MARK_BODY.
		//
		// NOTE: Spacing should be some integer multiple of resolution.
		extents.Xmin( extents.Xmin() - spacing );
		extents.Ymin( extents.Ymin() - spacing );
		extents.Xmax( extents.Xmax() + spacing );
		extents.Ymax( extents.Ymax() + spacing );

		pc.XY( extents.Xc(), extents.Yc() );

		grid = new CGrid;
		if (grid == NULL)
		{
			status.Fatal( IDS_MEM_ALLOC_FAILURE, "CNestingPart::bitmap_slice(#1)" );
		}
		else
		{
			status += grid->Create( extents, pc, config.Resolution() );

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Slice the outside kerf.  Additionally, we must
			// find a seed point for flood-filling.
			for (indx = 0; indx < count; ++indx)
			{
				geoPoly = geoPolyArray->GetAt( indx );

				// ASSUMPTION: We have a single closed outer polyline.
				interior_pt = InteriorPoint( (*geoPoly) );

				jcnt = geoPoly->Count();
				for (jndx = 0; jndx < jcnt; ++jndx)
				{
					// Harmless enough ...
					geoCurve = (CGeoCurve*) &(geoPoly->GetAt(jndx));

					// Tabulate the curve and map its points to the grid.
					status += grid->Slice( geoCurve, spacing );
				}
			}

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Similarly for the inside kerf.
			geoPolyArray = toolhit->PolysGet(HIT_NEST_INSIDE);

			count = geoPolyArray->Count();
			for (indx = 0; indx < count; ++indx)
			{
				geoPoly = geoPolyArray->GetAt( indx );

				jcnt = geoPoly->Count();
				for (jndx = 0; jndx < jcnt; ++jndx)
				{
					// Harmless enough ...
					geoCurve = (CGeoCurve*) &(geoPoly->GetAt(jndx));

					// Tabulate the curve and map its points to the grid.
					status += grid->Slice( geoCurve, spacing );
				}
			}

			grid->BodyFill( interior_pt );

			// Convert to dexgrid for memory savings
			dexgrid = new CDexGrid( *grid );
			delete grid;

			if (dexgrid == NULL)
			{
				status.Fatal( IDS_MEM_ALLOC_FAILURE, "CNestingPart::bitmap_slice(#2)" );
			}
			else
			{
				toolhit->Grid( dexgrid );

				if ( config.DebugBmp() )
				{
					dexgrid->Draw( view, 0, 0, FALSE, TRUE );
					dexgrid->Draw( view, 0, 0, TRUE, FALSE );
				}
			}
		}
	}

	return status;
}


// ==================================================================
//	Copy our model to the destination model... with the given owner
CReturn CNestingPart::CopyTo( 
	int				hit, 
	CModel*			dst_model, 
	CDbContainer*	owner )
{
	return CopyTo(hit, dst_model, owner, DBLINE, DBCOMMAND);
}

CReturn CNestingPart::CopyTo( 
	int				hit, 
	CModel*			dst_model, 
	CDbContainer*	owner,
	EDbEntityType	from_type,
	EDbEntityType	to_type )
{
	CReturn ret;
	CDbIterator iter;

	CModel* src_model = pModel(hit);
	src_model->EntityPrepareCopy( dst_model );

	CDbEntity*	new_ent = NULL;

	// Start our copy near the bottom... CopyTo is clever
	iter.Init( src_model->Db(), from_type );
	while (TRUE)
	{
		CDbEntity* db_ent = iter();
		if (!db_ent)
		{ break; }
		if (db_ent->Type() > to_type)
		{ break; }
		iter.Next();
		//
		//
		if ( src_model->Db().WasCopied( *db_ent ) )
			continue;
		//
		// Now, travel up ownership to the TOP DOG...
		//
		while (db_ent->Owner())
			db_ent = db_ent->Owner();

		// Don't carry the panel across
		if ( db_ent->Tool()
			&& db_ent->Tool()->IsLayer()
			&& db_ent->Tool()->Name().CompareNoCase( STR_STOCK ) == 0)
			continue;

		if (db_ent->IsHidden())
			continue;

		src_model->EntityCopy( *db_ent, &new_ent );

		if (owner)
		{ owner->Append( new_ent, FALSE ); }
	}

	return ret;
}



// ==================================================================
//	Debug generate... instantiates the geometry in the model
//	while drawing it... may crash your program, so use wisely.
CReturn CNestingPart::DebugToModel( CViewMgr* view, CModel* model )
{
	C3dCoord origin(0,0,0);

	for (int idx=0; idx<Count(); idx++)
	{
		CToolHit* toolhit = ToolHit(idx);
		toolhit->DebugToModel( origin, view, model );
	}

	return STATUS_OKAY;
}

#if (_CI || _NST)

	CReturn		
	CNestingPart::DebugToModel2( CViewMgr* view, CModel* model )
	{
		CReturn	status;
		int		count, indx;

		CCiModel* ciModel = CPortal::CiModel();

		ciModel->ActiveLayerSet( "nest_test" );
		ciModel->ActiveLayerGet()->Color(1);  // red

		count = m_toolhits->Count();
		for (indx = 0; indx < count; ++indx)
		{
			const CGeoCurve& curve = (*m_toolhits)[indx];

			switch (curve.Type())
			{
			case GEOLINE:

				if (curve.Length2d() > SMALL)
				{
					CCiLine* ciLine = (CCiLine*) ciModel->EntityCreate( CILINE );

					ciLine->Init(
						curve.StartPt(),
						curve.EndPt() );
				}
				break;

			case GEOARC:

				if ((((const CGeoArc&) curve).Radius() > SMALL)
					&& (curve.Length2d() > SMALL) )
				{
					CGeoArc& geoArc = (CGeoArc&) curve;

					CCiArc* ciArc = (CCiArc*) ciModel->EntityCreate( CIARC );

					ciArc->Init(
						geoArc.StartPt(),
						geoArc.EndPt(),
						geoArc.CenterPt(),
						geoArc.Dir() );
				}
				break;
			}
		}

		return status;
	}

#else

	CReturn		
	CNestingPart::DebugToModel2( CViewMgr* view, CModel* model )
	{
		CReturn ret;

		CDbTool* tool = model->ActiveTool();
		CDbWorkplane* work= model->ActiveWorkplane();

		int cnum = m_toolhits->Count();
		for (int cidx=0; cidx<cnum; cidx++)
		{
			const CGeoCurve& curve = m_toolhits->GetAt(cidx);

			CDbEntity* db_ent = NULL;

			((C3dCoord&)curve.StartPt()).Z(0.0);
			((C3dCoord&)curve.EndPt()).Z(0.0);
			switch (curve.Type())
			{
				case GEOLINE:
				{
					if (curve.Length2d() > SMALL)
					{
						CDbLine* db_line = NULL;
						ret = model->EntityCreate( DBLINE, (CDbEntity**)&db_line );
						if (ret.isOkay())
						{
							ret += db_line->Init( tool, work, *((CGeoLine*)&curve) );
						}
						
						if (ret.isOkay())
							db_ent = db_line;
					}
				}
				break;

				case GEOARC:
				{
					((C3dCoord&)((CGeoArc&)curve).CenterPt()).Z(0.0);

					if ( ( ((CGeoArc*)&curve)->Radius() > SMALL)
						&& (curve.Length2d() > SMALL) )
					{
						CDbArc* db_arc = NULL;
						ret = model->EntityCreate( DBARC, (CDbEntity**)&db_arc );
						if (ret.isOkay())
						{
							ret += db_arc->Init( tool, work, *((CGeoArc*)&curve) );
						}

						if (ret.isOkay())
						{
							db_ent = db_arc;
						}
						else // Try again as a line
						{
							CDbLine* db_line = NULL;
							ret = model->EntityCreate( DBLINE, (CDbEntity**)&db_line );
							if (ret.isOkay())
							{
								ret += db_line->Init( tool, work, curve.StartPt(), curve.EndPt() );
							}
							
							if (ret.isOkay())
								db_ent = db_line;
						}
					}
				}
				break;
			}

			if (db_ent)
			{
				db_ent->AttribsDelete();
				db_ent->ColorSet( 0xffffff );

				if (view)
				{
					view->ModelSet( *model );
					view->Refresh( db_ent->Id(), TRUE );
				}
			}
		}

		return ret;
	}

#endif

// ==================================================================
//	Delete any GeoPoly holes that are smaller than the specified area.
CReturn CNestingPart::Filter( int hit, double area )
{
	CReturn ret;
	//
	// Set up our context of information
	//
	CToolHit* toolhit = m_toolhitarray[hit];
	if (!toolhit)
		return ret.Fatal( IDS_NEST_PART_EMPTY );

	for (int depth=HIT_PART_INSIDE; depth<=HIT_NEST_INSIDE; depth++)
	{
		CGeoPolyArray* geopoly_array = toolhit->PolysGet(depth);
		// Note that we might have problems with the kerf... what if we took the kerf
		// but not the outside hole?  Could we get gouging?  Yes, if the kerf of the contained 
		// part was BIGGER than the kerf of the hole.  Possible, if unlikely.
		// TODO:  Only erase PART, leave KERF?

		if ( !geopoly_array || !geopoly_array->Count() )
			continue;

		int num = geopoly_array->Count()-1;
		for (int idx=num; idx>=0; idx--)
		{
			CGeoPoly* geo_poly = geopoly_array->GetAt(idx);

			if (fabs(geo_poly->Area()) < area)
			{ 
				CGeoPoly* poly = geopoly_array->Remove(idx);
				delete poly;
			}
		}
	}

	// Now, some fine tuning
	CGeoPolyArray* geopoly_array = toolhit->PolysGet(HIT_NEST_INSIDE);
	if ( !geopoly_array || !geopoly_array->Count() )
	{
		geopoly_array = toolhit->PolysGet(HIT_PART_INSIDE);

		// 2005.01.12 (PE) -- TODO: I think this is supposed to be "&& geopoly_array->Count()".
		if ( geopoly_array && !geopoly_array->Count() )
		{
			int num = geopoly_array->Count()-1;
			for (int idx=num; idx>=0; idx--)
			{
				geopoly_array->Remove(idx);
			}
		}
	}

	return ret;
}


// ============================================================================

CReturn CNestingPart::StripLeads(void)
{
	CReturn ret;

	CDbIterator iter;
	int num = m_modelarray.Count();
	for (int idx=0; idx<num; idx++)
	{
		CModel* model = m_modelarray[idx];

		iter.Init( model->Db(), DBHOLE );
		while ( TRUE )
		{
			CDbEntity* db_ent = iter();
			if (!db_ent)
				break;

			iter.Next();

			CDbFeature* feature = dynamic_cast<CDbFeature*>(db_ent);
			CDbHole* hole = dynamic_cast<CDbHole*>(db_ent);

			if (feature && feature->IsLead())
			{
				feature->Delete();
			}
			else if (hole && hole->IsPierce())
			{
				hole->Delete();
			}
		}
	}

	return ret;
}



// ==================================================================
//	Generate the link data needed for all types of gridding of this part.
//
//	You must call Generate() and Assemble() first, to set up the hits,
//	and after Slice() which sets up the grid origin info.
// ==================================================================
CReturn CNestingPart::GridLink( CNestConfig& config, int hit )
{
	CReturn		ret;
	CDbIterator	iter;
	C2dBox		part_mer;
	C2dBox		merA;
	C2dBox		merB;
	C2dBox		merLeads;
	C2dBox		mer;
	C2dVec		delta;
	CGeoPolyArray*	polysA;
	CGeoPoly*	poly;
	int			idx;
	double		height, width;
	double		dx, dy;

	double sign_x = 1.0;  // 'cause we always nest in +X direction
	double sign_y = (config.NestPosY() ? +1. : -1.);

	if ((config.Progression() == PROGRESS_PPX_SPY) ||
		(config.Progression() == PROGRESS_PPX_SNY))
		sign_y = 0.;
	else
		sign_x = 0.;

	// Basic setup
	CToolHit* toolhit = m_toolhitarray[hit];

	if ( CNestConfig::IsBitmapNest() )  // DYNATORCH
	{
		polysA = toolhit->PolysGet(HIT_NEST_OUTSIDE);
		for (idx=0; idx<polysA->Count(); idx++)
		{
			merA += polysA->GetAt(idx)->Extent();
		}

		dx = toolhit->Grid()->Width() * toolhit->Grid()->Resolution();
		dy = toolhit->Grid()->Height() * toolhit->Grid()->Resolution();
	}
	else
	{
		polysA = toolhit->PolysGet(HIT_NEST_OUTSIDE);
		if (0)
			polysA->Draw();

		for (idx=0; idx<polysA->Count(); idx++)
		{
			poly = polysA->GetAt(idx);
			mer = poly->Extent();

			merA += mer;
		}

		dx = merA.Dx();
		dy = merA.Dy();
	}

	height = merA.Dy();
	width = merA.Dx();

	delta.Init( dx, dy );
	toolhit->Delta( delta );
	toolhit->Mer( merA );

	// 0' Grid Link
	delta.Init( sign_x*dx, sign_y*dy );

	// And now for the orthogonal grid link.
	sign_x = 1.0;  // 'cause we always nest in +X direction
	sign_y = (config.NestPosY() ? +1. : -1.);

	if ((config.Progression() == PROGRESS_PPY_SPX) ||
		(config.Progression() == PROGRESS_PNY_SPX))
		sign_y = 0.;
	else
		sign_x = 0.;

	delta.Init( sign_x*dx, sign_y*dy );

	// 180' Grid Link
	int nrotations = abs(m_rotation);
	if ( !(nrotations & 0x01) )
	{
		int rotnum = nrotations;
		if (m_mirror)
			rotnum *= 2;

		int rot_hit = (hit + rotnum/2) % rotnum;
		CToolHit* rot_toolhit = m_toolhitarray[rot_hit];

		C3dCoord this_origin = toolhit->Grid()->Origin();
		C3dCoord rot_origin = rot_toolhit->Grid()->Origin();

		C2dVec new_delta( delta.X() + (this_origin.X()-rot_origin.X()),
							delta.Y() + (this_origin.Y()-rot_origin.Y()) );

		CLink link( rot_hit, 1.0, new_delta );
		toolhit->LinkCopyAppend( link );

		// Pre-nest offset 180' Grid Link
		if (config.PreNest())
		{
			double factor_y = config.NestPosY()?+0.10:-0.10;	// 10% steps
			double factor_x = 0.10;

			if ((config.Progression() == PROGRESS_PPX_SPY) ||
				(config.Progression() == PROGRESS_PPX_SNY))
			{ factor_x = 0.0; }
			else
			{ factor_y = 0.0; }

			for (int step=1; step<5; step++)
			{
				C2dVec pre_delta(new_delta.X() + width*factor_x*(double)step, 
								new_delta.Y() + height*factor_y*(double)step);

				CLink link( rot_hit, 1., pre_delta );
				toolhit->LinkCopyAppend( link );
			}
		}
	}

	// Mirrored Grid link
	if (m_mirror)
	{
		int mirror_hit = hit ^ 0x01;
		CToolHit* mirror_toolhit = m_toolhitarray[mirror_hit];
		//
		//
		C3dCoord this_origin = toolhit->Grid()->Origin();
		C3dCoord mirror_origin = mirror_toolhit->Grid()->Origin();

		C2dVec new_delta( delta.X() + (this_origin.X()-mirror_origin.X()),
							delta.Y() + (this_origin.Y()-mirror_origin.Y()) );

		CLink link( mirror_hit, 1., new_delta );
		toolhit->LinkCopyAppend(link);
	}

	return ret;
}

void CNestingPart::GapEntitiesRemove()
{
	if (m_filled_gaps)
	{
		CDbIterator	iter;
		CDbLine*	dbLine;
		int			count, indx;

		count = m_modelarray.Count();
		for (indx = 0; indx < count; ++indx)
		{
			iter.Init( m_modelarray[indx]->Db(), DBLINE );

			while (1)
			{
				dbLine = dynamic_cast<CDbLine*>( iter() );
				if (dbLine == NULL)
					break;

				if ( dbLine->IntGet( "is_gap_elem", FALSE ) )
					dbLine->Delete();

				iter.Next();
			}
		}
	}

	if (m_gap_tool != NULL)
	{
		// Otherwise, the part will fail to nest, issuing the
		// message that the tooling for the part can not be matched.

		m_gap_tool->Delete();
	}
}

void ConditionalAppend( const C3dCoord& ptA, C3dCoordArray* pts )
{
	C3dCoord*	ptB;
	int			count, indx;

	count = pts->Count();
	for (indx = 0; indx < count; ++indx)
	{
		ptB = pts->GetAt( indx );
		if ( ptB->WithinTolXY( ptA, SMALL ) )
			break;
	}

	if (indx >= count)
		pts->Append( new C3dCoord( ptA ) );
}

void CNestingPart::RepresentationAdjust()
{
	CDbEntityArray	entities;
	CDbIterator		iter;
	CDbEntity*		dbEntity;

	CModel*		model = m_modelarray[0];
	CEntityDb&	db = model->Db();

	if (m_representation == 1)
	{
		// Bounding Box using Part_Outline layer.
		model->UndoBufferSuppress();

		iter.Init( db, DBLINE );
		while (1)
		{
			dbEntity = iter();
			if ((dbEntity == NULL) || (dbEntity->Type() > DBHOLE))
				break;

			if ( !dbEntity->Tool()->IsLayer() )
				entities.Append( dbEntity );

			iter.Next();
		}

		CModelUtil::BBPartOutlineCreate( entities, model );
	}
	else if (m_representation == 2)
	{
		// Convex Hull using Part_Outline layer.
		model->UndoBufferSuppress();

		iter.Init( db, DBLINE );
		while (1)
		{
			dbEntity = iter();
			if ((dbEntity == NULL) || (dbEntity->Type() > DBHOLE))
				break;

			if ( !dbEntity->Tool()->IsLayer() )
				entities.Append( dbEntity );

			iter.Next();
		}

		CModelUtil::CHPartOutlineCreate( entities, 1.e-3, model );
	}
	else
	{
		// Do nothing.  Default to "As Given".
	}
}

bool CNestingPart::IsPartOutline( const CDbEntity& dbEntity )
{
	bool is_part_outline = false;

	if ( !dbEntity.IsToolpath() )
	{
		CDbTool* dbTool = dbEntity.Tool();
		is_part_outline = (dbTool->Name().CompareNoCase(STR_PART_OUTLINE) == 0);
	}

	return is_part_outline;
}

bool CNestingPart::IsToolPath( const CDbEntity& dbEntity )
{
	CDbFeature*	dbFeature;
	CDbProfile*	dbProfile;
	int			tp_flag;

	bool is_tool_path = false;

	if ( dbEntity.IsToolpath() )
	{
		dbProfile = dynamic_cast<CDbProfile*>( dbEntity.Owner() );
		if (dbProfile == NULL)
		{
			dbFeature = dynamic_cast<CDbFeature*>( dbEntity.Owner() );
			if (dbFeature != NULL)
			{
				tp_flag = dbFeature->IntGet( "tp", 0 );
				is_tool_path = ((tp_flag == 0) && dbFeature->IsLead());
			}
		}
		else
		{
			tp_flag = dbProfile->IntGet( "tp", 0 );
			is_tool_path = (tp_flag == 0);
		}
	}

	return is_tool_path;
}

void CNestingPart::ContainerToGeopoly( const CDbContainer& dbContainer, CGeoPoly* geoPoly )
{
	CDbCurve*	dbCurve;
	CGeoCurve*	geoCurve;
	int			count, indx;

	count = dbContainer.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbCurve = dynamic_cast<CDbCurve*>( dbContainer[indx] );
		if (dbCurve != NULL)
		{
			geoCurve = dbCurve->Curve();
			geoPoly->Append( geoCurve );
		}
	}
}

void CNestingPart::PolyTransfer( CGeoPoly* tmp, CGeoPoly* geoPoly )
{
	int count = tmp->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		// TODO: DYNATORCH -- horrible hack alert!
		CGeoCurve* geoCurve = (CGeoCurve*) &(tmp->GetAt(indx));
		geoPoly->Append( geoCurve );
	}

	tmp->BenignFlush();
}

// ASSUMPTION: The poly has a CW winding direction.
C2dCoord CNestingPart::InteriorPoint( const CGeoPoly& geoPoly )
{
	C2dCoord pt,  midpt;

	// TODO: DYNATORCH -- find a more robust method
	// of finding a good interior point.

	int count = geoPoly.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CGeoCurve& geoCurve = geoPoly[indx];

		CGeoCurve* geoOffset = geoCurve.Offset( -1, 0.25 );
		if (geoOffset != NULL)
		{
			midpt = geoOffset->MidPt();
			delete geoOffset;

			if ((count == 1) && (geoCurve.Type() == GEOARC))
			{
				// Assume we have a circle.
				// Ya just gotta love special cases ....
				pt = midpt;
				break;
			}
			else if ( geoPoly.PtInPoly( midpt ) )
			{
				pt = midpt;
				break;
			}
		}
	}

	return pt;
}

// Copy HIT_NEST_OUTSIDE to HIT_KERF_OUTSIDE where the latter represents
// the outside kerf before it is offset by the spacing distance and is
// used to generate the remnant skeleton.
//
// NOTE: Wrt. remnant generation, the material profile *must* have a CW
// winding direction and the parts' HIT_KERF_OUTSIDE *must* have a CCW
// winding direction. Otherwise, CTrueRemnant::PartsRemove() will yield
// wrong results.
void CNestingPart::KerfCopy( int hit, eToolhit which )
{
	CToolHit*		toolhit;
	CGeoPolyArray*	polys;
	CGeoPoly*		kerf;
	CGeoPoly*		copy;
	int				count, indx;

	assert( ((which == HIT_NEST_OUTSIDE) || (which == HIT_NEST_INSIDE)) );

	eToolhit to = ((which == HIT_NEST_OUTSIDE) ? HIT_KERF_OUTSIDE : HIT_KERF_INSIDE);

	toolhit = m_toolhitarray[hit];
	polys = toolhit->PolysGet( which );

	// NOTE: 'count' should be just one when 'which' is HIT_NEST_OUTSIDE
	count = polys->Count();
	for (indx = 0; indx < count; ++indx)
	{
		kerf = polys->GetAt( indx );

		copy = (CGeoPoly*) kerf->Clone( true );
		if ((to == HIT_KERF_OUTSIDE) && (copy->Winding() < 0))
			copy->Reverse();

		toolhit->PolysGet( to )->Append( copy );
	}
}

CDbHole* CNestingPart::PierceHoleGet( const CToolHit& toolhit, const CDbHole* hole )
{
	CGeoPolyArray*	polys;
	CGeoPoly*		poly;
	CDbHole*		pierce_hole;

	pierce_hole = (CDbHole*) hole;

	polys = toolhit.PolysGet( HIT_PART_INSIDE );
	
	poly = polys->GetAt(0);
	if (poly == NULL)
	{
		// ASSUMPTION: Either this part does not have any nestable
		// interior holes or nest part-in-part is disabled. Regardless,
		// this *must* be the pierce hole. Nothing else to do.
	}
	else
	{
		polys = toolhit.PolysGet( HIT_PART_OUTSIDE );
		
		// ASSUMPTION: *The* outside profile.
		poly = polys->GetAt(0);
		if ( poly->PtInPoly( hole->Center() ) )
			pierce_hole = NULL;
	}

	return pierce_hole;
}

// 2008.08.03 (PE) -- NOTE: This can be very problematic.
// Then name CanClose is a bit misleading. It really means that
// it is okay for us to attempt to close the profile. There are
// cases in punch-only parts where closing a profile leads to
// failures in nesting.
bool CNestingPart::CanClose( const CDbProfile& dbProfile ) const
{
	bool can_close = false;

	if ( !dbProfile.IsClosed() && dbProfile.CanClose() )
	{
		// At this point, we are guaranteed to have a profile
		// containing more than one 'cutting' entity.

		CDbTool*dbTool = dbProfile.Tool();
		if ( dbTool->IsPunchTool() )
		{
			// Punch-only profiles can be very problematic!
			C3dCoord ps;
			C3dCoord pe;
			CDbLine* dbLine;
			double max_gap, dist;
			int count, indx;

			max_gap = 0.;

			// Find the biggest gap in the profile.
			count = dbProfile.Count();
			for (indx = 0; indx < count; ++indx)
			{
				dbLine = dynamic_cast<CDbLine*>( dbProfile[indx] );
				if ((dbLine != NULL) && dbLine->Tool()->IsGapTool() )
				{
					ps = dbLine->StartPt();
					pe = dbLine->EndPt();

					dist = ps.DistXY( pe );
					if (dist > max_gap)
						max_gap = dist;
				}
			}

			ps = ((CDbCurve*) dbProfile[0])->StartPt();
			pe = ((CDbCurve*) dbProfile[count-1])->EndPt();
	
			dist = ps.DistXY( pe );
			can_close = ((dist - max_gap) <= 1.e-4);  // arbitrary tolerance.

		}
		else
		{
			// Trivial.
			can_close = true;
		}
	}

	return can_close;
}

void CNestingPart::LeadEntitiesDelete( CModel* model )
{
	CDbIterator iter;

	iter.Init( model->Db(), DBHOLE );
	while (1)
	{
		CDbHole* dbHole = dynamic_cast<CDbHole*>( iter() );
		if (dbHole == NULL)
			break;  // exhausted the holes.

		if ( dbHole->IsPierce() )
			dbHole->Delete();

		iter.Next();
	}

	iter.Init( model->Db(), DBFEATURE );
	while (1)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;  // exhausted the features.

		if ( dbFeature->IsLead() )
			dbFeature->Delete();

		iter.Next();
	}
}
