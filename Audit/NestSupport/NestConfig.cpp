// ==================================================================
//		NestConfig
//
//	Holding class to load and store all relevant nesting control
//	parameters.  Doesn't *do* anything; a glorified box.
//
// ==================================================================

#include "stdafx.h"
#include <float.h>

#include "CommonFlags.h"
#include "StringConst.h"
#include "Portal.h"
#include "Register.h"

#include "DbArc.h"

#include "NestConfig.h"
#include "Sheet.h"
#include "Path.h"
#include "ModelUtil.h"
#include "PartBin.h"
#include "DaoQuery.h"
#include "FileSnoop.h"

#include "DaoAttrib.h"

const int DB_NOP = 0;
const int DB_INT = 1;
const int DB_DBL = 2;
const int DB_STR = 3;


bool CNestConfig::m_bitmap_nest = false;
bool CNestConfig::m_is_ccs_nesting = false;

static CNestConfig g_nest_config;
CNestConfig& NestConfigGet()  { return g_nest_config; }


// ==================================================================

CNestConfig::CNestConfig()
{
	NestConfigInit();
}

CNestConfig::~CNestConfig()
{
	if (m_narrow)
		delete[] m_narrow;

	if (m_active)
		delete[] m_active;
}

void CNestConfig::NestConfigInit()
{
	// Don't much care about attribute defaults; they are either
	// all set from a DB, or all bogus.

	// However, status info needs a preset
	m_large_part = DBL_MIN;
	m_small_part = DBL_MAX;
	m_large_hole = DBL_MIN;
	m_small_hole = DBL_MAX;

	m_gaptol = SMALL;
	m_mintool = 0.0;

	m_pass_num = 0;
	m_pass = 0;

	m_narrow = NULL;
	m_active = NULL;

	m_machkey = 0;
	m_nestkey = 0;

	m_repo_overlap = 0.0;

	m_lead_setup = -1;

	m_display_mode = 0;

	m_yield_min = 0;

	m_label_angle = 0.0;
	m_label_size = 10;

	m_flag1 = 0;
	m_flag2 = 0;
	GridNest(true);
	RepoShort(true);
	RepoLate(true);
	RepoToBurn(false);
	StripLayers(true);
	StripTools(true);
//	KerfOnSheet(true);

	// NOTE: Will be true for Modular Services.  In that case,
	// we must read the sequence objects in the MM2 file and
	// maintain the cutting order of the part (ie. the pattern
	// must maintain a cut order).
	KeepSequence(false);

	m_debug_draw = CRegister::IntGetV( "Nesting", "graphics", 0 );

	// Introduced to support DYNATORCH, but may become
	// useful to our direct customers.
	m_bitmap_nest = CRegister::BoolGetV( "Nesting", "bn", false );

	m_is_ccs_nesting = false;

	// The application will (by default) set the reg-entry for
	// resolution to "-1" if the Prefences dialog has been accessed.
	m_resolution = CRegister::DoubleGetV( "Nesting", "resolution", -UNDEFINED );

	// If we do not find a user-defined resolution then we must
	// ensure that we have one.
	if (m_resolution < 0)
	{
		// When using standard nesting, the resolution (here UNDEFINED)
		// is determined later by CNestProcessApp::do_resolution().
		m_resolution = (IsBitmapNest() ? 0.05 : UNDEFINED);
	}

	m_must_seed_barrier = false;
	m_have_large_parts = false;

	m_lead_restrictions = 0;  // None
	m_curr_zone = -1;  // invalid

	m_opposite_clamps = TRUE;
	m_first_part_inspection = 0;

	m_consider_clamps = false;
	m_clamp_count = 0;

	m_is_ccs_nesting = false;
	m_cut_back_station_id = 0;

	m_shifted_onto_sheet = false;
	m_placement_failed = true;

	// See also ise of UseBooleans() in CNestingPart::do_toolhit_burn().
	m_use_booleans = CRegister::BoolGetV( "Nesting", "use_booleans", true );
}

void CNestConfig::ResultPath( const CString& path )
{
	CString	sval = CRegister::StringGetV( "Debug", "$RESULT", "" );
	if ( sval.IsEmpty() )
	{
		m_result_path = path;
	}
	else
	{
		// We must be executing a RTL test.
		m_result_path = sval;
	}
}

void CNestConfig::Narrow( int in_pass, double in_size )
{
	m_narrow[in_pass] = in_size;
}

double CNestConfig::Resolution( void ) const
{
	return m_resolution;
}

void CNestConfig::Resolution( double res )
{
	m_resolution = res;
}

CReturn CNestConfig::Load( 
	const CString&	cmdb_path,
	const CString&	pdb_path )
{
	static int PT2PT = 8;
	CReturn	status;
	int		work_plane, type_id;

	// CRITICAL: To LoadSetup(), LoadRemnant(), LoadMachine().
	CmdbPath( cmdb_path );
	PdbPath( pdb_path );

	// Load Setup Table
#if (_CI || _NST)
	status += LoadSetupCI();
#else
	status += MachineIdFromPdb();
	status += LoadMachine();
	status += LoadSetup();
	status += LoadRemnant();
#endif

	// Are we Y-Negative quadrant?
	work_plane = MachineInt( "Workplane Type ID", 0);
	type_id = MachineInt( "Type ID", 0 );

	YNegative( ((work_plane == 2) && (type_id != PT2PT)) );

#if BEFORE_V18_0_59_1
	if (FillDir() == FILL_Y)
	{
		if ( YNegative() )
			Progression( FillBottom() ? PROGRESS_PNY_SPX : PROGRESS_PPY_SPX );
		else
			Progression( FillBottom() ? PROGRESS_PPY_SPX : PROGRESS_PNY_SPX );
	}
	else
	{
		if ( YNegative() )
			Progression( FillBottom() ? PROGRESS_PPX_SNY : PROGRESS_PPX_SPY );
		else
			Progression( FillBottom() ? PROGRESS_PPX_SPY : PROGRESS_PPX_SNY );
	}
#else
	if (FillDir() == FILL_Y)
	{
		if ( YNegative() )
			Progression( OppositeClamps() ? PROGRESS_PPY_SPX : PROGRESS_PNY_SPX );
		else
			Progression( OppositeClamps() ? PROGRESS_PNY_SPX : PROGRESS_PPY_SPX );
	}
	else
	{
		if ( YNegative() )
			Progression( OppositeClamps() ? PROGRESS_PPX_SPY : PROGRESS_PPX_SNY );
		else
			Progression( OppositeClamps() ? PROGRESS_PPX_SNY : PROGRESS_PPX_SPY );
	}
#endif

	return status;
}

CReturn CNestConfig::LoadSetup()
{
	const CString& pdb_path = PdbPath();
	if (pdb_path.GetLength() < 3)
		return STATUS_OKAY;  // not really :-(

	CDaoDB	part_db;
	CReturn ret = part_db.Open( pdb_path );
	if (!ret.isOkay())
		return ret;

	// --------------------------------------------------------------

	CDaoAttrib att( &part_db, CString("Setup") );
	//
	//
	MachKey( att.Int( "Machine ID", -1 ) );
	LeadSetup( att.Int( "Lead Setup ID", -1 ) );

	// === Results
	ResultPath( att.String( "Result Path", "" ) );
	ResultName( att.String( "RootName", "" ) );
	ResultDigits( att.Int( "ResultDigits", 2 ) );
	DisplayParts( att.Int( "DisplayParts", 1 ) != 0 );
	DisplayAnnotation( att.Int( "DisplayText", 1 ) != 0 );

	// === Nest Controls
	YieldMin( att.Int( "Yield", 0 ) );
	GapTolerance( att.Double( "GapTol", 0.0 ) );
	SingleSheet( att.Int( "SingleSheet", 0 ) == 0);
	ShiftPierce( att.Int( "GroupPierce", 0 ) != 0);
	StripLayers( att.Int( "StripLayers", 0 ) != 0);
	StripTools( att.Int( "StripTools", 1 ) != 0);
	DropStop(att.Int( "DropStop", 0 ) != 0);
	PatternSplit(att.Int("SplitPattern", 0) != 0);
	LeadRestrictions( att.Int("LeadRestrictions", 0 ) );  // default to (0) None

	// === Part Labeling
	int label_mode = att.Int("Labeling", 1);
	LabelName((label_mode&0x01) != 0);
	LabelTag(label_mode<2);
	LabelAngle(att.Double( "LabelAngle", 0.0 ));
	LabelSize(att.Int( "LabelSize", 10 ));
	LabelTool(att.Int( "LabelTool", -1));

	// === Part Placement
	NestOrder( (eNestOrder)att.Int( "Ordering", 2 ) );
	HardPierce(att.Int("HardPierce", 0) != 0);
	PartInPart( att.Int( "PartInPart", 0 ) != 0);
	FillDir( (eFill)att.Int( "FillDirection", 1 ) );

	// (0) Near Clamps / (1) Opposite Clamps
	int defval = (IsBitmapNest() ? 0 : 1);
	OppositeClamps( att.Int( "FillBottom", defval ) != 0 );

	// (0) Under Clamps / (1) Around Clamps
	ClampAround( att.Int( "NestAround", 1 ) != 0);

#if BEFORE_V18_0_59_1
	if ( IsBitmapNest() )  // DYNATORCH
		// DYNATORCH -- 1st quadrant machine.  Nest + direction from Y0.
		FillBottom( att.Int( "FillBottom", 0 ) == 0);
	else
		// Note I reversed the logic here; makes sense wrt. 
		// the name, vs. changing the DAT
		FillBottom( att.Int( "FillBottom", 1 ) == 0);
#endif

	Spacing( att.Double( "Spacing", 0.0 ) );
	GridMode((eGridMode)att.Int("GridMode", (int)GRID_STRONG));
	GridFlags(GridMode());

	int fill_to = att.Int("FillLimit", 0);
	FillToRepo(fill_to > 0);
	FillToSheet(fill_to > 1);

	ChainNum( att.Int( "ChainNum", 1 ) + 1 );
	ChainOffset( att.Double("ChainOff", 0.25) );

	// === Reposition Controls
	int repo_mode = att.Int( "RepoMode", 1 );
	MachineTravelX( att.Double( "RepoTravelX", 0.0 ) );		// Machine Override
	RepoOverlap( att.Double( "RepoOverlap", 0.0 ) );
	RepoShort(att.Int( "ProcessShortPiece", 0 ) != 0);
	RepoLate(att.Int( "CutLate", 0 ) != 0);
	RepoToBurn(att.Int("RepoBurn", 0) != 0);

	int hold_mode = att.Int( "HoldMode", 0 );
	ForceHoldX( ((hold_mode & 0x01) != 0) );
	ForceHoldY( ((hold_mode & 0x02) != 0) );
	HoldDefaultX( att.Double( "HoldDefaultX", 0.0 ) );
	HoldDefaultY( att.Double( "HoldDefaultY", 0.0 ) );

	// if ( !IsBitmapNest() )  // DYNATORCH
	{
		// PREREQUISITE: LoadMachine() must be called before LoadSetup().
		// Clamp1..Clamp4
		bool got_clamp = false;
		for (int indx = 0; indx < MaxAllowedClamps(); indx++)
		{
			// Default to non-existent clamp.
			m_clamp[indx] = false;
			m_clamp_pos[indx] = 0.;

			// Note, bitmap nesting does not support clamps
			// because it is assumed to be a burner scenario.
			if ((indx < ClampCountGet()) && !IsBitmapNest())
			{
				CString name;
				name.Format( "Clamp%d", indx+1 );

				double pos = att.Double( name, -999. );
				if (pos > SMALL)
				{
					m_clamp[indx] = true;
					m_clamp_pos[indx] = pos;
					got_clamp = true;
				}
			}
		}

		if (!got_clamp)
			ClampAround(true);	// Disable nest-under-clamp code if not clamps
	}

	// === Borders
	Border( BORDER_LEFT, att.Double( "LeftBorder", 0.0 ) );
	Border( BORDER_RIGHT, att.Double( "RightBorder", 0.0 ) );
	Border( BORDER_TOP, att.Double( "TopBorder", 0.0 ) );
	Border( BORDER_BOTTOM, att.Double( "BottomBorder", 0.0 ) );
	Border( BORDER_REMNANT, att.Double( "RemnantBorder", 0.0 ) );

	// === Accounting
	AreaLessHoles( att.Int( "AreaLessHoles", 0 ) != 0);
	AreaOffset( att.Int( "AreaOffset", 1 ) != 0);

	// === Advanced
	// Default first part inspection to disabled.
	FirstPartInspection( ((att.Int( "FirstPartInspection", 0 ) == 1) ? 1 : 0) );
	KeepSequence( (att.Int( "KeepSequence", 0 ) != 0) );	// For Modular Services.
	// SubsAcrossSheets n/a
	HFactor( att.Double( "hfactor", 1.0 ) );
	VFactor( att.Double( "vfactor", 3.0 ) );

	// ------------------------------------------------------------------------
	//	Calculated, Modified, and Fixed values
	//
	Filter(att.Int( "Filter", !PartInPart()) != 0);
	RemnantTopGrain(false);	// att.Int( "TopGrain", 0 ) != 0 );

	CutBackStationID( att.Int( "CutBackTool", 0 ) );

	// ----------------------------------------------------
	if (ResultPath().GetLength() > 2)
	{
		CPath rpath( m_result_path );
		ResultPath(rpath.DriveDir());
	}
	// ----------------------------------------------------
	if (ResultName().GetLength() < 2)
		ResultName(CFileSnoop::stripSuffix( CFileSnoop::getFilename( pdb_path ) ));

	switch (repo_mode)
	{
	case 0:
		Progressive(false);
		RepoSmall(true);
		break;
	case 1:
		Progressive(false);
		RepoSmall(false);
		break;
	case 2:
		Progressive(true);
		RepoSmall(false);
		break;
	}

	part_db.Close();

	return ret;
}

void CNestConfig::ClampCountSet( int count )
{
	m_clamp_count = ((count > MaxAllowedClamps()) ? MaxAllowedClamps() : count);
}

// ============================================================================
// GridMode() doesn't set the working flags; must call THIS method
// to make GridMode() functional.  Done this way so the PreNest can
// temporarily override the behavior.
//
void
CNestConfig::GridFlags(	eGridMode grid_mode )
{
	GridSquare(false);
	FillSeed(false);
	GridBarrier(false);
	GridChain(false);

	switch (grid_mode)
	{
	case GRID_LOOSE:  // TODO: Wish this into the cornfield.
	case GRID_GRID:
		GridSquare(true);
		FillSeed(true);
		break;
	case GRID_STRONG:
		GridSquare(true);
		break;
	case GRID_FORCE:
		GridSquare(true);
		GridBarrier(true);
		break;
	case GRID_CHAIN:
		GridSquare(true);
		GridBarrier(true);
		GridChain(true);
		break;
	}
}
// ============================================================================

#if (_CI || _NST)

	CReturn
	CNestConfig::LoadSetupCI(
		const CString&	cmdb_path,
		const CString&	pdb_path )
	{
		CReturn ret;
		CDaoDB	part_db;
		CString	nesting_ids;
		CPath path;

		CmdbPath( cmd_path );
		PdbPath( pdb_path );
		if (m_pdb_path.GetLength() < 3)
			return ret;

		ret += part_db.Open( partdb_name );
		if (!ret.isOkay())
			return ret;

		// --------------------------------------------------------------

		CDaoAttrib att( &part_db, CString("Setup") );

#if (_NST)
		// Required by CPartBin::load_parts() when converting cad to mm2.
		LayerSetup( att.ciInt( "LayerSetup", 0 ) );
#endif

#if (_CI)
		nesting_ids = att.ciString( "NestingIDs", "" );
		NestingIDs( nesting_ids );
#endif
		if ( nesting_ids.IsEmpty() )
		{
			MachKey( -1 );
			ToolSetupKey( -1 );
		}
		else
		{
			int	job_id, machine_id, material_id, tool_setup_id, piece;

			sscanf( nesting_ids, "%d,%d,%d,%d,%d",
				&job_id, &machine_id, &material_id, &tool_setup_id, &piece );

			MachKey( machine_id );
			ToolSetupKey( tool_setup_id );
		}

		LeadSetup( att.ciInt( "Lead Setup ID", -1 ) );

		// === Results
		// ResultPath( att.ciString( "ResultPath", ".\\Nests" ) );
		path.Set( partdb_name );
		ResultPath( att.ciString( "ResultPath", path.DriveDir() ) );
		ResultName( att.ciString( "RootName", "" ) );
		ResultDigits( att.ciInt( "ResultDigits", 2 ) );
		DisplayParts( att.ciInt( "DisplayParts", 1 ) != 0 );
		DisplayAnnotation( att.ciInt( "DisplayText", 1 ) != 0 );

		// === Nest Controls
		YieldMin( att.ciInt( "Yield", 0 ) );
		GapTolerance( att.ciDouble( "GapTol", 0.0 ) );
		SingleSheet( att.ciInt( "SingleSheet", 1 ) == 0);
		ShiftPierce( att.ciInt( "GroupPierce", 0 ) != 0);
		StripLayers( att.ciInt( "StripLayers", 1 ) != 0);
		StripTools( att.ciInt( "StripTools", 1 ) != 0);
		DropStop( FALSE );
		PatternSplit( FALSE );

		// === Part Labeling
		int label_mode = att.ciInt("Labeling", 0);
		LabelName((label_mode&0x01) != 0);
		LabelTag(0);
		LabelAngle(att.ciDouble( "LabelAngle", 0. ));
		LabelSize(att.ciDouble( "LabelSize", 1. ));
		LabelTool(att.ciInt( "LabelTool", -1));

		// === Part Placement
		NestOrder( (eNestOrder)att.ciInt( "Ordering", 2 ) );
		HardPierce( FALSE );
		PartInPart( att.ciInt( "PartInPart", 0 ) != 0);
		FillDir( (eFill)att.ciInt( "FillDirection", 1 ) );
#if BEFORE_V18_0_59_1
		// Note I reversed the logic here; makes sense wrt. the name, vs. changing the DAT
		FillBottom( att.ciInt( "FillBottom", 0 ) == 0);
#else
		// (0) Near Clamps / (1) Opposite Clamps
		OppositeClamps( att.ciInt( "FillBottom", 0 ) != 0 );
#endif
		Spacing( att.ciDouble( "Spacing", 0.0 ) );
		GridMode((eGridMode)att.ciInt("GridMode", (int)GRID_STRONG));
		GridFlags(GridMode());

		int fill_to = att.ciInt("FillLimit", 2);
		FillToRepo(fill_to > 0);
		FillToSheet(fill_to > 1);

		ChainNum( att.ciInt( "ChainNum", 1 ) + 1 );
		ChainOffset( att.ciDouble("ChainOff", 0.25) );

		// === Reposition Controls
		int repo_mode = att.ciInt( "RepoMode", 1 );
		MachineTravelX( 0. );	// Machine Override - aka Work-zone size
		RepoOverlap( 0. );
		RepoShort( FALSE );
		RepoLate( FALSE );
		RepoToBurn( FALSE );
		ClampAround( TRUE );
		int hold_mode = att.ciInt( "HoldMode", 0 );
		ForceHoldX( hold_mode & 0x01 );
		ForceHoldY( hold_mode & 0x02 );
		HoldDefaultX( 0. );
		HoldDefaultY( 0. );

		// Clamp1..Clamp4
		bool got_clamp = false;
		for (int idx=0; idx<4; idx++)
		{
			CString name;
			name.Format( "Clamp%d", idx+1 );
			double pos = att.ciDouble( name, -999.0 );
			if ( ZERO(pos) || (pos < -900) )
			{
				m_clamp[idx] = false;
				m_clamp_pos[idx] = 0.0;
			}
			else
			{
				m_clamp[idx] = true;
				m_clamp_pos[idx] = pos;
				got_clamp = true;
			}
		}
		if (!got_clamp)
			ClampAround(true);	// Turns off nest-under-clamp code if not clamps

		// === Borders
		Border( BORDER_LEFT, att.ciDouble( "LeftBorder", 0.0 ) );
		Border( BORDER_RIGHT, att.ciDouble( "RightBorder", 0.0 ) );
		Border( BORDER_TOP, att.ciDouble( "TopBorder", 0.0 ) );
		Border( BORDER_BOTTOM, att.ciDouble( "BottomBorder", 0.0 ) );
		Border( BORDER_REMNANT, att.ciDouble( "RemnantBorder", 0.0 ) );

		// === Accounting
		AreaLessHoles( att.ciInt( "AreaLessHoles", 0 ) != 0);
		AreaOffset( att.ciInt( "AreaOffset", 1 ) != 0);

		// === Advanced
		KeepSequence( att.ciInt( "KeepSequence", 0 ) );	// For Modular Services.
		// SubsAcrossSheets n/a
		HFactor( att.ciDouble( "hfactor", 1.0 ) );
		VFactor( att.ciDouble( "vfactor", 3.0 ) );

		if ( !IsBitmapNest() )  // DYNATORCH
			Resolution( 0.1 );	// NOT USED; but put in a limiter here.

		// ------------------------------------------------------------------------
		//	Calculated, Modified, and Fixed values
		Filter(att.ciInt( "Filter", !PartInPart()) != 0);
		RemnantTopGrain(false);	// att.Int( "TopGrain", 0 ) != 0 );
		// ----------------------------------------------------
		if (ResultPath().GetLength() > 2)
		{
			CPath rpath( m_result_path );
			ResultPath(rpath.DriveDir());
		}
		// ----------------------------------------------------
		if (ResultName().GetLength() < 2)
		{ ResultName(CFileSnoop::stripSuffix( CFileSnoop::getFilename( partdb_name ) )); }
		if (ResultName().GetLength() < 2)
		{ ResultName("nest"); }
		// ----------------------------------------------------
		switch (repo_mode)
		{
			case 0:
				Progressive(false);
				RepoSmall(true);
				break;
			case 1:
				Progressive(false);
				RepoSmall(false);
				break;
			case 2:
				Progressive(true);
				RepoSmall(false);
				break;
		}

		part_db.Close();

		return ret;
	}

#endif

CReturn CNestConfig::LoadRemnant()
{
	CDaoDB	config_db;

	int shop_tab = config_db.addTable( "Remnant Control" );

	CReturn ret = config_db.Open( CmdbPath() );
	if (ret.isOkay())
	{
		// Load Shop Management (now Remnant Control) details
		if (config_db.countRecord( shop_tab ) > 0)
		{
			config_db.moveRecord( shop_tab, 0 );

			Remnant( config_db.getInt( shop_tab, "Remnant" ) != 0);
			Cutback( config_db.getInt( shop_tab, "Cutback" ) != 0);

			m_remn_width		= config_db.getDouble( shop_tab, "Remnant_Width" );
			m_remn_length		= config_db.getDouble( shop_tab, "Remnant_Length" );
			m_remn_area			= config_db.getDouble( shop_tab, "Remnant_Area" );
			m_remn_counter		= config_db.getInt( shop_tab, "Remnant_Counter" );
			RemnantFirst( config_db.getInt( shop_tab, "Remnant_First" ) != 0);

#if BEFORE_V17
			m_remn_path = config_db.getString( shop_tab, "Remnant_Path" );
			if (m_remn_path.GetLength() > 2)
			{
				CPath npath( m_remn_path );
				m_remn_path = npath.DriveDir();
			}
#else
			// With V17, we generate remnants in .rem files and place them
			// in the Remnants folder.  This simplifies things for the
			// remnant manager dialog.
			m_remn_path.Format( "%s\\Remnants", CPortal::ExeFolder() );
#endif

			m_remn_digits = config_db.getInt( shop_tab, "Remnant_Digits" );
		}
		else
		{
			Remnant(false);
			Cutback(false);
		}
		config_db.Close();
	}

	return ret;
}

CReturn CNestConfig::MachineIdFromPdb()
{
	CReturn status;

	const CString& pdb_path = PdbPath();

	CDaoDB part_db;
	status = part_db.Open( pdb_path );
	if ( status.IsOk() )
	{
		CDaoAttrib att( &part_db, CString("Setup") );

		int id = att.Int( "Machine ID", -1 );
		MachKey( id );

		part_db.Close();

		if (id < 0)
			status.Internal( IDS_INTERNAL_ERROR, "MachineIdFromPdb()" );
	}

	return status;
}

CReturn CNestConfig::LoadMachine()
{
	CDaoDB config_db;
	CReturn ret = config_db.Open( CmdbPath() );
	if (ret.isOkay())
	{
		// Stolen from AutoModDb
		CDaoQuery	query;
		CString		sql;

		// PREQUISITE: 'm_machkey' must be properly set (eg. via MachineIdFromPdb())
		sql.Format( "SELECT * FROM Machines WHERE ([ID]=%d)", m_machkey );
		ret += query.Init( config_db.Database(), sql );

		m_mach_attrib.Reset();

		int fieldCount = query.FieldCount();
		for (int indx = 0; indx < fieldCount; ++indx)
		{
			CString attribName = query.FieldNameGet( indx );
			CString attribValue = query.StringGet( indx );
			m_mach_attrib.setString( attribName, attribValue );
		}

		sql.Format(
			"SELECT [Machine Attribute Types].Description, [Machine Attributes].Value " \
			"FROM [Machine Attribute Types] INNER JOIN [Machine Attributes] ON "
			"[Machine Attribute Types].ID = [Machine Attributes].[Attribute Type ID] " \
			"WHERE ((([Machine Attributes].[Machine ID])=%d))",
					m_machkey );

		ret += query.Init( config_db.Database(), sql );

		if (ret.IsOk())
		{
			// Extract all of the queried machine attributes into a handy varset.
			query.Move( 0 );
			int num = query.RecordCount();
			for (int idx=0; idx<num; idx++)
			{
				query.Move( (idx==0)? 0 : 1);

				CString name = query.StringGet( "Description" );
				double value = query.DoubleGet( "Value" );
				m_mach_attrib.setReal( name, value );
			}
		}

		int cnt = m_mach_attrib.getInt( "Number_of_Clamps", -1 );
		ClampCountSet( cnt );

		int type = m_mach_attrib.getInt( "Holddown_type", -1 );
		HasHoldDowns( (type >= 0) );

		config_db.Close();
	}

	return ret;
}

// ==================================================================
void
CNestConfig::PassInit(
	int		in_size )
{
	if (m_narrow)
		delete[] m_narrow;

	m_narrow = new double[in_size+1];

	if (m_active)
		delete[] m_active;

	m_active = new bool[in_size+1];

	for (int idx=0; idx<=in_size; idx++)
	{
		m_narrow[idx] = DBL_MAX;
		m_active[idx] = FALSE;
	}
}

// ==================================================================
double
CNestConfig::MostNarrow( void ) const
{
	double narrow = DBL_MAX;
	for (int idx=0; idx<=m_pass_num; idx++)
	{
		// Ignore active flag... not needed, doesn't significantly speed up
		double test = m_narrow[idx];
		if (test < narrow)
		{ narrow = test; }
	}

	return narrow;
}

// ==================================================================
//		ChangeModelY
//
//	Adjust all of the geometry in the model with respect to the Y axis
//
void
CNestConfig::ChangeModelY(
	double		x_axis,
	CModel*		model,
	bool		flip,
	bool		geo ) const
{
	CSelectorStack& selectorStack = model->SelectorStack();
	selectorStack.Push();

	CSelector& selector = selectorStack();

	selector.All( 0 );
	if (flip || geo)
	{
		selector.Filter( DBPOINT, 1 );
		selector.Filter( DBLINE, 1 );
		selector.Filter( DBARC, 1 );
		selector.Filter( DBHOLE, 1 );
	}
	selector.Filter( DBCOMMAND, 1 );

	selector.SystemFlag( TRUE );
	selector.SelectAll( TRUE );

	//
	// Build the "mirror" transform
	//	TODO:  Move some of the Transform support out of Process into a library
	//
	C3x4Matrix	to_origin;
	C3x4Matrix	mirror;
	C3x4Matrix	from_origin;

	to_origin.setUnit();
	mirror.setUnit();

	to_origin.Shift( C3dVec(0.0, -x_axis, 0.0) );

	if (flip)
	{
		// TODO:  We need to affect the flippage of punch tools... somehow... even if they are fixed!
		to_origin.InvertTo( &from_origin );
		mirror.Scale( 1, -1, 1 );

		to_origin.Transform( &mirror );
		mirror.Transform( &from_origin );
	}

	if (flip)
		CModelUtil::Transform( model, from_origin, 0, XFORM_NO_SYSTEM );
	else
		CModelUtil::Transform( model, to_origin, 0, XFORM_NO_SYSTEM );

	selector.Clear();
	selectorStack.Pop();
}

#if (_CI || _NST)

	// ==================================================================
	//		ClearResults
	//
	//	Wipe out any pre-existing result tables in the database
	//
	CReturn
	CNestConfig::ClearResults( void )
	{
		CReturn		status;

		CDaoDB	result_db;
		int		table;

		status += result_db.Open( PdbPath() );

		// First, delete any results tables
		status += result_db.delTable( "SheetResult" );
		status += result_db.delTable( "PartResult" );
		status += result_db.delTable( "Stock Result" );
		status += result_db.delTable( "Failed Part Result" );
		status += result_db.delTable( "Hold Part Result" );
		status += result_db.delTable( "New Remnant Result" );

		// Then, re-create them as we desire
		table = AddStdTable( "SheetResult", "SheetID", &result_db );
		//result_db.setIndex( table, "SheetFilename", FALSE );  // huh?

		// --------------------------------------------------
		table = AddStdTable( "PartResult", "SheetID", &result_db );
		//result_db.setIndex( table, "SheetFilename", FALSE );  // huh?

		// --------------------------------------------------
		table = result_db.createTable( "Stock Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newInt(		table, "Material ID", FALSE );
		result_db.newInt(		table, "Remnant ID", FALSE );
		result_db.newInt(		table, "Quantity Used", FALSE );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "Failed Part Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Part Name", 255 );
		result_db.newInt(		table, "Part ID", FALSE );
		result_db.newInt(		table, "Quantity", FALSE );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "dX" );
		result_db.newDouble(	table, "dY" );

		result_db.newDouble(	table, "Cost" );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "Hold Part Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Part Name", 255 );
		result_db.newInt(		table, "Part ID", FALSE );
		result_db.newInt(		table, "Quantity", FALSE );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "dX" );
		result_db.newDouble(	table, "dY" );

		result_db.newDouble(	table, "Cost" );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "New Remnant Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Filename", 255 );
		result_db.newInt(		table, "Type ID", FALSE );
		result_db.newTime(		table, "Creation Date" );
		result_db.newDouble(	table, STR_LENGTH );
		result_db.newDouble(	table, STR_WIDTH );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "Thickness" );
		result_db.newDouble(	table, "Weight" );
		result_db.newInt(		table, "Quantity", FALSE );
		result_db.newDouble(	table, "Cost Per Unit" );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// Done!
		status += result_db.Close();

		return status;
	}

#else

	// ==================================================================
	//		ClearResults
	//
	//	Wipe out any pre-existing result tables in the database
	//
	CReturn
	CNestConfig::ClearResults( void )
	{
		CReturn		ret;

		CDaoDB	result_db;

		ret += result_db.Open( PdbPath() );

		//
		// First, delete any results tables
		//
		ret += result_db.delTable( "Sheet Result" );
		ret += result_db.delTable( "Part Result" );
		ret += result_db.delTable( "Stock Result" );
		ret += result_db.delTable( "Failed Part Result" );
		ret += result_db.delTable( "Hold Part Result" );
		ret += result_db.delTable( "New Remnant Result" );

		//
		// Then, re-create them as we desire
		//
		// 
		// --------------------------------------------------
		int table = result_db.createTable( "Sheet Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Sheet Filename", 255 );
		result_db.newInt(		table, "Material ID", FALSE );
		result_db.newInt(		table, "Remnant ID", FALSE );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "Cut Back" );
		result_db.newDouble(	table, "CB Area" );
		result_db.newDouble(	table, "Efficiency" );
		result_db.newDouble(	table, "CB Efficiency" );
		result_db.newInt(		table, "Repeat", FALSE );
		result_db.newTime(		table, "Date" );
		result_db.newDouble(	table, "Cycle Time" );

		result_db.setIndex( table, "ID", TRUE );
		result_db.setIndex( table, "Sheet Filename", FALSE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "Part Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Sheet Filename", 255 );
		result_db.newInt(		table, "Sheet ID", FALSE );
		result_db.newString(	table, "Part Name", 255 );
		result_db.newInt(		table, "Part ID", FALSE );
		result_db.newInt(		table, "Legend ID", FALSE );
		result_db.newInt(		table, "Quantity", FALSE );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "Cut_Travel_Distance" );
		result_db.newInt(		table, "Number_Of_Pierces", FALSE );
		result_db.newDouble(	table, "Cost" );

		result_db.setIndex( table, "ID", TRUE );
		result_db.setIndex( table, "Sheet Filename", FALSE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "Stock Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newInt(		table, "Material ID", FALSE );
		result_db.newInt(		table, "Remnant ID", FALSE );
		result_db.newInt(		table, "Quantity Used", FALSE );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "Failed Part Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Part Name", 255 );
		result_db.newInt(		table, "Part ID", FALSE );
		result_db.newInt(		table, "Quantity", FALSE );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "dX" );
		result_db.newDouble(	table, "dY" );

		result_db.newDouble(	table, "Cost" );
		result_db.newString(	table, "Reason", 512 );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "Hold Part Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Part Name", 255 );
		result_db.newInt(		table, "Part ID", FALSE );
		result_db.newInt(		table, "Quantity", FALSE );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "dX" );
		result_db.newDouble(	table, "dY" );

		result_db.newDouble(	table, "Cost" );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// --------------------------------------------------
		table = result_db.createTable( "New Remnant Result" );

		result_db.newInt(		table, "ID", TRUE );
		result_db.newString(	table, "Filename", 255 );
		result_db.newInt(		table, "Type ID", FALSE );
		result_db.newTime(		table, "Creation Date" );
		result_db.newDouble(	table, STR_LENGTH );
		result_db.newDouble(	table, STR_WIDTH );
		result_db.newDouble(	table, "Area" );
		result_db.newDouble(	table, "Thickness" );
		result_db.newDouble(	table, "Weight" );
		result_db.newInt(		table, "Quantity", FALSE );
		result_db.newDouble(	table, "Cost Per Unit" );

		result_db.setIndex( table, "ID", TRUE );

		result_db.commitTable( table );

		// --------------------------------------------------
		// Done!
		ret += result_db.Close();

		return ret;
	}

#endif

#if (_CI || _NST)

	// ==================================================================
	//		save_pass_results
	//
	//	Record the statistical and other information related to this
	//	just-nested sheet
	//
	void
	CNestConfig::SavePassResults(
					int					sheet_id,
					const CSheet&		sheet,
					const CPartBin&		partbin )
	{
		CReturn			status;
		CString			value;
		CString			part_id;
		CDaoDB			result_db;
		CPath			path;
		CNestingPart*	npart;
		double			part_area;
		int				count, indx;

		int sheet_table = result_db.addTable( "SheetResult" );
		int part_table = result_db.addTable( "PartResult" );

		part_area = 0.0;

		count = partbin.Count();
		for (indx=0; indx<count; indx++)
		{
			npart = partbin[indx];
			if (npart->Used() < 1)
				continue;

			part_area += npart->Area() * npart->Used();
		}

		status = result_db.Open( PdbPath() );

		//	add sheet information
		path.Set( sheet.PathName() );


		value.Format( "#%d", sheet_id );

#if (_NST || _PM4)
		AddStdRecord(
			sheet_table, "sheet_filename",
			value, path.FileNameExt(), DB_NOP, "", 1,
			"SheetID", sheet_id, &result_db );
#else
		AddStdRecord(
			sheet_table, "sheet_filename",
			value, path.FileName(), DB_NOP, "", 1,
			"SheetID", sheet_id, &result_db );
#endif

		value.Format( "%d", sheet.Repeat() );
		AddStdRecord(
			sheet_table, "repeat",
			"Repeats", value, DB_INT, "", 1,
			"SheetID", sheet_id, &result_db );

		value.Format( "%-10.2f", sheet.Area() );
		AddStdRecord(
			sheet_table, "area",
			"Area", value, DB_DBL, "", 1,
			"SheetID", sheet_id, &result_db );

		value.Format( "%-10.2f", sheet.Cutback() );
		AddStdRecord(
			sheet_table, "cut_back",
			"Cut Back", value, DB_DBL, "", 1,
			"SheetID", sheet_id, &result_db );

		// TODO: date (?)

		value.Format( "%-10.2f", (part_area * 100) / sheet.Area() );
		AddStdRecord(
			sheet_table, "efficiency",
			"Efficiency", value, DB_DBL, "", 1,
			"SheetID", sheet_id, &result_db );

		if ( sheet.IsRemnant() )
		{
			AddStdRecord(
				sheet_table, "cb_area",
				"Cut Back Area", "0.", DB_DBL, "", 1,
				"SheetID", sheet_id, &result_db );

			AddStdRecord(
				sheet_table, "cb_efficiency",
				"Cut Back Efficiency", "0.", DB_DBL, "", 1,
				"SheetID", sheet_id, &result_db );
		}
		else
		{
			value.Format( "%-10.2f", sheet.AdjustedArea() );
			AddStdRecord(
				sheet_table, "cb_area",
				"Cut Back Area", value, DB_DBL, "", 1,
				"SheetID", sheet_id, &result_db );

			value.Format( "%-10.2f", (part_area * 100) / sheet.AdjustedArea() );
			AddStdRecord(
				sheet_table, "cb_efficiency",
				"Cut Back Efficiency", value, DB_DBL, "", 1,
				"SheetID", sheet_id, &result_db );
		}

		// Not visible.
		value.Format( "%d", sheet.MaterialId() );
		AddStdRecord(
			sheet_table, "material_id",
			"material_id", value, DB_INT, "", 0,
			"SheetID", sheet_id, &result_db );

		// Not visible.
		value.Format( "%d", sheet.RemnantId() );
		AddStdRecord(
			sheet_table, "remnant_id",
			"remnant_id", value, DB_INT, "", 0,
			"SheetID", sheet_id, &result_db );


		//	add part information (and correlate with sheet)
		count = partbin.Count();
		for (indx=0; indx<count; indx++)
		{
			npart = partbin[indx];
			if (npart->Used() < 1)
				continue;

			part_id.Format( "%d", npart->ActualID() );

#if 1  // BEFORE_V3_0
			value = npart->Name();
#else
			path.Set( npart->Filepath() );
			value = path.FileName();
#endif
			AddStdRecord(
				part_table, "Filename",
				value, value, DB_NOP, part_id, 1,
				"SheetID", sheet_id, &result_db );

			value.Format( "%d", npart->Used() );
			AddStdRecord(
				part_table, "Part_Quantity",
				"Part Quantity", value, DB_INT, part_id, 1,
				"SheetID", sheet_id, &result_db );

			value.Format( "%-10.2f", npart->Area() );
			AddStdRecord(
				part_table, "Area",
				"Area", value, DB_DBL, part_id, 1,
				"SheetID", sheet_id, &result_db );

			value.Format( "%-10.2f", npart->Cost() );
			AddStdRecord(
				part_table, "Cost",
				"Cost", value, DB_DBL, part_id, 1,
				"SheetID", sheet_id, &result_db );

			// Not visible.
			value = sheet.PathName();
			AddStdRecord(
				part_table, "SheetFilename",
				"Sheet Name", value, DB_STR, part_id, 0,
				"SheetID", sheet_id, &result_db );

			// Not visible.
			// 2005.03.06 (PE) -- added so that CI reporting system
			// can trace data back to the ComponentVar table.
			value.Format( "%d", npart->ActualID() );
			AddStdRecord(
				part_table, "Actual_ID",
				"Actual ID", value, DB_INT, part_id, 0,
				"SheetID", sheet_id, &result_db );
		}

		result_db.Close();
	}

#else

	// ==================================================================
	//		save_pass_results
	//
	//	Record the statistical and other information related to this
	//	just-nested sheet
	//
	void CNestConfig::SavePassResults( const CSheet& sheet, const CPartBin& partbin )
	{
		CDaoDB result_db;

		int sheet_table = result_db.addTable( "Sheet Result" );
		int part_table = result_db.addTable( "Part Result" );

		double part_area = 0.;

		int indx, num = partbin.Count();
		for (indx = 0; indx < num; ++indx)
		{
			CNestingPart* npart = partbin.GetAt(indx);
			if (npart->Used() < 1)
				continue;

			part_area += npart->Area() * npart->Used();
		}

		result_db.Open( PdbPath() );

		// --------------------------------------------------
		//	add sheet information
		//
		result_db.createRecord( sheet_table );

		result_db.setString(	sheet_table, "Sheet_Filename", sheet.PathName() );
		result_db.setInt(		sheet_table, "Material_ID", sheet.MaterialId() );
		result_db.setInt(		sheet_table, "Remnant_ID", sheet.RemnantId() );

		result_db.setDouble(	sheet_table, "Area", sheet.Area() );
		result_db.setDouble(	sheet_table, "Cut_Back", sheet.Cutback() );
		result_db.setInt(		sheet_table, "Repeat", sheet.Repeat() );
		result_db.setTime(		sheet_table, "Date" );

		result_db.setDouble(	sheet_table, "Efficiency", (part_area * 100) / sheet.Area() );

		if ( sheet.IsRemnant() )
		{
			result_db.setDouble(	sheet_table, "CB_Area", 0.0 );
			result_db.setDouble(	sheet_table, "CB_Efficiency", 0.0 );
		}
		else
		{
			result_db.setDouble(	sheet_table, "CB_Area", sheet.AdjustedArea() );
			result_db.setDouble(	sheet_table, "CB_Efficiency", (part_area * 100) / sheet.AdjustedArea() );
		}

		int sheet_id = result_db.getInt( sheet_table, "ID" );

		result_db.commitRecord( sheet_table );

		// --------------------------------------------------
		//	add part information (and correlate with sheet)

		num = partbin.Count();
		for (indx = 0; indx < num; ++indx)
		{
			CNestingPart* npart = partbin.GetAt(indx);
			if (npart->Used() < 1)
				continue;

			double cutDistance;
			int numPierces;
			CModelUtil::StatisticsGet( npart->Model(), &cutDistance, &numPierces );

			result_db.createRecord( part_table );

			result_db.setString(	part_table, "Sheet_Filename", sheet.PathName() );
			result_db.setInt(		part_table, "Sheet_ID", sheet_id );
			result_db.setString(	part_table, "Part_Name", npart->Name() );
			result_db.setInt(		part_table, "Part_ID", npart->Id() );
			result_db.setInt(		part_table, "Legend_ID", npart->SortedID() );

			result_db.setInt(		part_table,	"Quantity", npart->Used() );
			result_db.setDouble(	part_table, "Area", npart->Area() );
			result_db.setDouble(	part_table, "Cut_Travel_Distance", cutDistance );
			result_db.setInt(		part_table, "Number_Of_Pierces", numPierces );
			result_db.setDouble(	part_table, "Cost", npart->Cost() );

			result_db.commitRecord( part_table );
		}

		result_db.Close();
	}

#endif

void
CNestConfig::SaveFinalResults(
	const CSheet&		sheet,
	const CPartBin&		partbin )
{
	CDaoDB	result_db;

	int failed_table = result_db.addTable( "Failed Part Result" );
	int stock_table = result_db.addTable( "Stock Result" );

	int	used_mat_table = result_db.addTable( "Material Inventory" );
	int used_rem_table = result_db.addTable( "Remnant Inventory" );

	result_db.Open( PdbPath() );

	// --------------------------------------------------
	//	Detail failed parts...
	//
	int idx, num = partbin.Count();
	for (idx=0; idx<num; idx++)
	{
		CNestingPart*	npart = partbin.GetAt(idx);

		if ((npart->Quantity() <= 0) || npart->Filler())
			continue;

		if ( npart->ErrorMsg().IsEmpty() )
		{
			CReturn status;
			if ( npart->Oversized() )
				status.Internal( IDS_FAILED_PART_OVERSIZED );
			else
				status.Internal( IDS_FAILED_PART_UNSPECD );

			npart->Error( status.LastErrorMsg() );
		}

		result_db.createRecord( failed_table );

		result_db.setString(	failed_table, "Part_Name", npart->Name() );
		result_db.setInt(		failed_table, "Part_ID", npart->Id() );

		result_db.setInt(		failed_table,	"Quantity", npart->Quantity() );
		result_db.setDouble(	failed_table, "Cost", npart->Cost() );

		result_db.setDouble(	failed_table, "Area", npart->Area() );
		result_db.setDouble(	failed_table, "dX", npart->Extent().Dx() );
		result_db.setDouble(	failed_table, "dY", npart->Extent().Dy() );

		result_db.setString(	failed_table, "Reason", npart->ErrorMsg() );

		result_db.commitRecord( failed_table );
	}

	// --------------------------------------------------
	//	add stock use information
	//
	CDaoDB config_db;

	int new_mat_table = config_db.addTable( "Material Inventory" );
	int new_rem_table = config_db.addTable( "Remnant Inventory" );

	config_db.Open( CmdbPath() );

	for (idx=0; idx<sheet.RemnantCount(); idx++)
	{
		int remnant_id = sheet.RemnantId( idx );
		int material_id = sheet.MaterialId( idx );

		int	qty = 0;

		if (remnant_id < 0)
		{
			// Determine material usage
			if ((config_db.findRecord( new_mat_table, "ID", material_id ) >= 0) &&
				(result_db.findRecord( used_mat_table, "ID", material_id ) >= 0))
			{
				qty = config_db.getInt( new_mat_table, "Quantity" )
					- result_db.getInt( used_mat_table, "Quantity" );
			}
		}
		else // material_id < 0)
		{
			// Determine remnant usage
			if ((config_db.findRecord( new_rem_table, "ID", remnant_id ) >= 0) &&
				(result_db.findRecord( used_rem_table, "ID", remnant_id ) >= 0))
			{
				qty = config_db.getInt( new_rem_table, "Quantity" )
					- result_db.getInt( used_rem_table, "Quantity" );
			}
		}

		result_db.createRecord( stock_table );

		result_db.setInt( stock_table, "Material_ID", material_id );
		result_db.setInt( stock_table, "Remnant_ID", remnant_id );
		result_db.setInt( stock_table, "Quantity_Used", qty );

		result_db.commitRecord( stock_table );
	}


	// --------------------------------------------------
	//	done!
	//
	result_db.Close();
	config_db.Close();
}


// ==================================================================

void
CNestConfig::SaveHoldResults(
	const CPartBin&		partbin )
{
	CDaoDB	result_db;

	int hold_table = result_db.addTable( "Hold Part Result" );

	result_db.Open( PdbPath() );

	// --------------------------------------------------
	//	Detail hold parts... and take them out of hold
	//
	int num = partbin.Count();
	for (int idx=0; idx<num; idx++)
	{
		CNestingPart*	npart = partbin.GetAt(idx);

		int count = npart->Quantity() + npart->Used();

		if ((count <= 0) || npart->Filler())
			continue;

		result_db.createRecord( hold_table );

		result_db.setString(	hold_table, "Part_Name", npart->Name() );
		result_db.setInt(		hold_table, "Part_ID", npart->Id() );

		result_db.setInt(		hold_table,	"Quantity", count );
		result_db.setDouble(	hold_table, "Cost", npart->Cost() );

		result_db.setDouble(	hold_table, "Area", npart->Area() );
		result_db.setDouble(	hold_table, "dX", npart->Extent().Dx() );
		result_db.setDouble(	hold_table, "dY", npart->Extent().Dy() );

		result_db.commitRecord( hold_table );

		npart->Quantity( 0 );
		npart->Used( 0 );
	}

	// --------------------------------------------------
	//	done!
	//
	result_db.Close();
}


// ==================================================================
//		TestSheetHold
//
//	Test the sheet's efficiency rating -- if it's too low, return TRUE
//	to stop all further nesting.  This method also fills the Hold Part Result
//	table, and clears the use and quantity of all parts in the bin.
//
bool
CNestConfig::DoSheetHold(
	const CSheet&		sheet,
	const CPartBin&		partbin )
{
	if (!YieldTest())
		return FALSE;

	//
	// Calculate the part efficiency
	//
	double part_area = 0.0;
	int num = partbin.Count();
	for (int idx=0; idx<num; idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);
		if (npart->Used() < 1)
			continue;

		part_area += npart->Area() * npart->Used();
	}

	double yield = (part_area * 100) / sheet.Area();

	if (yield < YieldMin())
	{
		// Failed the test -- move it into the hold table
		SaveHoldResults( partbin );
		return TRUE;
	}

	return FALSE;
}


// ============================================================================

double
CNestConfig::MachineTravelX() const
{
	double travel = m_repo_travel;
	if (ZERO(travel))
	{ travel = MachineReal( "Max_Travel_Limit_X", 0.0 ) - MachineReal("Min_Travel_Limit_X", 0.0); }

	return travel;
}

#if (_CI || _NST)

	int
	CNestConfig::AddStdTable(
					const CString& table_name,
					const CString& link_field_name,
					CDaoDB* db )
	{
		int table;
		
		table = db->createTable( table_name );

		db->newInt(		table, "ID", TRUE );
		db->setIndex(	table, "ID", TRUE );

		db->newString(	table, "InternalName", 255 );
		db->newString(	table, "Display", 255 );
		db->newString(	table, "Value", 255 );
		db->newInt(		table, "Type", FALSE );
		db->newString(	table, "Options", 255 );
		db->newInt(		table, "Flags", FALSE );
		db->newInt(		table, link_field_name, FALSE );

		db->commitTable( table );

		return table;
	}

	int
	CNestConfig::AddStdRecord(
					int				which_table,
					const CString&	internal_name,
					const CString&	display_name,
					const CString&	data_value,
					int				data_type,
					const CString&	options,
					int				flags,
					const CString&	link_field_name,
					int				link_field_value,
					CDaoDB*			db )
	{
		int	record_id;

		db->createRecord( which_table );

		db->setString(	which_table, "InternalName",	internal_name );
		db->setString(	which_table, "Display",			display_name );
		db->setString(	which_table, "Value",			data_value );
		db->setInt(		which_table, "Type",			data_type );
		db->setString(	which_table, "Options",			options );
		db->setInt(		which_table, "Flags",			flags );
		db->setInt(		which_table, link_field_name,	link_field_value );

		record_id = db->getInt( which_table, "ID" );

		db->commitRecord( which_table );

		return record_id;
	}

#endif

bool CNestConfig::NestPosY( void ) const
{
	return ((m_progression == PROGRESS_PPY_SPX) ||
			(m_progression == PROGRESS_PPX_SPY));
}

// static public
bool CNestConfig::IsBitmapNest()
{
	// Which is initialized via registry.
	return 	m_bitmap_nest;
}

// static public
void CNestConfig::CcsNesting( bool is_ccs_nesting )
{
	m_is_ccs_nesting = is_ccs_nesting;
}

// static public
bool CNestConfig::CcsNesting()
{
	return m_is_ccs_nesting;
}


// Where 'id' represents the [Tool Setup Members].ID as provided
// by [Setup] via NestParams.dat
// NOTE: We get the station ID because it is unique (ie, multiple
// instance of a tool can be placed in different stations).
void
CNestConfig::CutBackStationID( int id )
{
	CDaoDB cmdb;

	m_cut_back_station_id = 0;

	CReturn status = cmdb.Open( CmdbPath() );
	if ( status.IsOk() )
	{
		CDaoQuery	query;
		CString		sql;

		sql.Format( "SELECT * FROM [Tool Setup Members] WHERE ([ID]=%d)", id );
		status = query.Init( cmdb.Database(), sql );
		if ( status.IsOk() )
			m_cut_back_station_id = query.IntGet("Station ID");
	}
}
