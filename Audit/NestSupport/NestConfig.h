#pragma once

// ==================================================================
//		NestConfig
//
//	Holding class to load and store all relevant nesting control
//	parameters.  Doesn't *do* anything; a glorified box.
//
// ==================================================================

#include "Common.h"
#include "DaoDB.h"
#include "2dBox.h"

#include "Model.h"

class CSheet;
class CPartBin;

// ==================================================================

enum eNestProgression
{
	PROGRESS_PPY_SPX = 0,  // Primary Positive Y - Secondary Positive X
	PROGRESS_PNY_SPX = 1,  // Primary Negative Y - Secondary Positive X
	PROGRESS_PPX_SPY = 2,  // Primary Positive X - Secondary Positive Y
	PROGRESS_PPX_SNY = 3   // Primary Positive X - Secondary Negative Y
};

enum eNestOrder
{
	NEST_PRESET,		// Order preset in the database
	NEST_INORDER,		// Nest in order of presentation
	NEST_LARGE,			// Large ones first!
	NEST_SMALL			// Small ones first!
};

enum eFill
{
	FILL_X,
	FILL_Y
};

enum eBorderEdge
{
	BORDER_LEFT,
	BORDER_TOP,
	BORDER_RIGHT,
	BORDER_BOTTOM,
	BORDER_REMNANT,
	BORDER_NUM
};

enum eGridMode
{
	GRID_LOOSE		= 0,  // TODO: Wish this into the cornfield
	GRID_GRID		= 1,
	GRID_STRONG		= 2,
	GRID_FORCE		= 3,
	GRID_CHAIN		= 4
};
//
// Bitfields for the many (many) configuration flags
//
// flag1
const DWORD CONFIG_FILTER		= 0x00000001;
const DWORD CONFIG_DROPSTOP		= 0x00000002;
const DWORD CONFIG_PRENEST		= 0x00000004;
const DWORD CONFIG_GRIDSQUARE	= 0x00000008;
const DWORD CONFIG_FILLPARTS	= 0x00000010;
// const DWORD CONFIG_INHOLE		= 0x00000020;  obsolete
const DWORD CONFIG_AREAHOLES	= 0x00000040;
const DWORD CONFIG_AREAOFFSET	= 0x00000080;
const DWORD CONFIG_REPOLATE		= 0x00000100;
const DWORD CONFIG_DISPLAYPARTS	= 0x00000200;
// const DWORD CONFIG_DISPLAYDEBUG	= 0x00000400;  replaced by m_debug_draw
const DWORD CONFIG_FILLBOTTOM	= 0x00000800;
const DWORD CONFIG_NEWLINE		= 0x00001000;
const DWORD CONFIG_REPOBURN		= 0x00002000;
const DWORD CONFIG_FILLLIMIT	= 0x00004000;
const DWORD CONFIG_CUTBACK		= 0x00008000;
const DWORD CONFIG_REMNANT		= 0x00010000;
const DWORD CONFIG_YNEGATIVE	= 0x00020000;
const DWORD CONFIG_STRIPLAYERS	= 0x00040000;
const DWORD CONFIG_SHIFTPIERCE	= 0x00080000;
const DWORD CONFIG_REPOSHORT	= 0x00100000;
const DWORD CONFIG_CLAMPAROUND	= 0x00200000;
const DWORD CONFIG_ONESHEET		= 0x00400000;
//const DWORD CONFIG_KERFONSHEET	= 0x00800000;
const DWORD CONFIG_PARTINPART	= 0x01000000;
const DWORD CONFIG_REMNANTFIRST	= 0x02000000;
const DWORD CONFIG_GRIDNEST		= 0x04000000;
const DWORD CONFIG_DIDSCORE		= 0x08000000;
const DWORD CONFIG_DIDNEST		= 0x10000000;
const DWORD CONFIG_DIDLARGE		= 0x20000000;
const DWORD CONFIG_LABELNAME	= 0x40000000;
const DWORD CONFIG_FILLSEED		= 0x80000000;
//
// flag2
const DWORD CONFIG_TOPGRAIN		= 0x00000001;
const DWORD CONFIG_BADRATIO		= 0x00000002;
const DWORD CONFIG_OFFSHEET		= 0x00000004;
const DWORD CONFIG_OVERSIZE		= 0x00000008;
const DWORD CONFIG_FORCESEED	= 0x00000010;
const DWORD CONFIG_DISPLAYTEXT	= 0x00000020;
const DWORD CONFIG_SINGLESHEET	= 0x00000040;
const DWORD CONFIG_GRIDBARRIER	= 0x00000080;
const DWORD CONFIG_STRIPTOOLS	= 0x00000100;
const DWORD CONFIG_PROGRESSIVE	= 0x00000200;
const DWORD CONFIG_HOLDX		= 0x00000400;
const DWORD CONFIG_HOLDY		= 0x00000800;
const DWORD CONFIG_SMALLREPO	= 0x00001000;
const DWORD CONFIG_LABELTAG		= 0x00002000;
const DWORD CONFIG_KEEP_SEQ		= 0x00004000;
const DWORD CONFIG_GRIDCHAIN	= 0x00008000;
const DWORD CONFIG_HARDPIERCE	= 0x00010000;
const DWORD CONFIG_PATTERNSPLIT = 0x00020000;
const DWORD CONFIG_FILLTOREPO	= 0x00040000;
const DWORD CONFIG_FILLTOSHEET	= 0x00080000;
//
// Task Completion flags
//
const WORD DID_COUNT	= 0x0001;
const WORD DID_SEED		= 0x0002;
const WORD DID_SCORE	= 0x0004;
const WORD DID_TOOL		= 0x0008;
const WORD DID_YIELD	= 0x0010;

const WORD DID_ALL		= 0x001f;

// ==================================================================

class dllExport CNestConfig
{
public:

	CNestConfig();

	virtual ~CNestConfig();

	void NestConfigInit();

	void CmdbPath( const CString& path )				{ m_cmdb_path = path; }
	const CString& CmdbPath() const						{ return m_cmdb_path; }

	void PdbPath( const CString& path )					{ m_pdb_path = path; }
	const CString& PdbPath() const						{ return m_pdb_path; }

	CReturn	Load( const CString& cmdb_path, const CString& pdb_path );

	CReturn MachineIdFromPdb();

	// PREREQUISITE: Must first call CmdbPath() to set the path.
	CReturn	LoadMachine();

	// Setup

	int			MachKey( void ) const					{ return m_machkey; }
	void		MachKey( int key )						{ m_machkey = key; }

#if (_CI || _NST)
	CString		NestingIDs( void ) const				{ return m_nesting_ids; }
	void		NestingIDs( const CString ids )			{ m_nesting_ids = ids; }

	int			ToolSetupKey( void ) const				{ return m_toolSetupkey; }
	void		ToolSetupKey( int key )					{ m_toolSetupkey = key; }
#endif

#if (_NST)
	int			LayerSetup( void ) const					{ return m_layer_setup_id; }
	void		LayerSetup( int id )						{ m_layer_setup_id = id; }
#endif

	int			NestKey( void ) const					{ return m_nestkey; }
	void		NestKey( int key )						{ m_nestkey = key; }

	const CString&	ResultName( void ) const			{ return m_result; }
	void		ResultName( const CString& name )		{ m_result = name; }
	//
	// Data Access
	//
	double		Spacing( void ) const					{ return m_spacing; }
	void		Spacing( double space )					{ m_spacing = space; }

	double		ChainOffset(void) const					{ return m_chain_off; }
	void		ChainOffset(double off)					{ m_chain_off = off; }

	int			ChainNum(void) const					{ return m_chain; }
	void		ChainNum(int num)						{ m_chain = num; }

	double		Border( eBorderEdge idx ) const			{ return m_border[idx]; }
	int			BorderGrid( eBorderEdge idx ) const		{ return (int)(m_border[idx]/m_resolution+0.5); }
	void		Border( eBorderEdge idx, double val )	{ m_border[idx] = val; }

	bool		HasHoldDowns() const					{ return m_has_holddowns; }
	void		HasHoldDowns( bool has )				{ m_has_holddowns = has; }

	bool		ForceHoldX( void ) const				{ return get_flag2(CONFIG_HOLDX); }
	void		ForceHoldX( bool set )					{ set_flag2(CONFIG_HOLDX, set); }

	bool		ForceHoldY( void ) const				{ return get_flag2(CONFIG_HOLDY); }
	void		ForceHoldY( bool set )					{ set_flag2(CONFIG_HOLDY, set); }

	double		HoldDefaultX( void ) const				{ return m_hold_default.X(); }
	void		HoldDefaultX( double hdx )				{ m_hold_default.X( hdx ); }

	double		HoldDefaultY( void ) const				{ return m_hold_default.Y(); }
	void		HoldDefaultY( double hdy )				{ m_hold_default.Y( hdy ); }

	bool		PreNest( void ) const					{ return get_flag1(CONFIG_PRENEST); }
	void		PreNest( bool set )
	{
		set_flag1(CONFIG_PRENEST, set);
	}

	double		HFactor( void ) const					{ return m_hfactor; }
	void		HFactor( double factor )				{ m_hfactor = factor; }

	double		VFactor( void ) const					{ return m_vfactor; }
	void		VFactor( double factor )				{ m_vfactor = factor; }

	int			LeadSetup( void ) const					{ return m_lead_setup; }
	void		LeadSetup( int id )						{ m_lead_setup = id; }

	eNestOrder	NestOrder( void ) const					{ return m_nest_order; }
	void		NestOrder(eNestOrder order)				{ m_nest_order = order; }

	double		RepoOverlap( void ) const				{ return m_repo_overlap; }
	void		RepoOverlap( double overlap)			{ m_repo_overlap = overlap; }

	double		MachineTravelX( void ) const;
	double		MachineRepoTravel(void) const			{ return m_repo_travel; }
	void		MachineTravelX( double travel )			{ m_repo_travel = travel; }

	int			MaxAllowedClamps() const				{ return 4; }
	void		ClampCountSet( int count );
	int			ClampCountGet() const					{ return m_clamp_count; }
	bool		HasClamps() const						{ return (m_clamp_count > 0); }

	bool		UseClamp( int idx ) const				{ return m_clamp[idx]; }
	void		UseClamp(int idx, bool set)				{ m_clamp[idx] = set; }

	double		ClampPos( int idx ) const				{ return m_clamp_pos[idx]; }
	void		ClampPos(int idx, double pos)			{ m_clamp_pos[idx] = pos; }

	double		MachineReal( const CString& name, double defval ) const { return m_mach_attrib.getReal( name, defval ); }
	int			MachineInt( const CString& name, int defval ) const		{ return m_mach_attrib.getInt( name, defval ); }
	CString		MachineString( const CString& name, const CString& defval ) const { return m_mach_attrib.getString( name, defval ); }
	//
	//  Status information
	//
	int			Counter( void ) const					{ return m_counter; }
	void		Counter( int in_cnt )					{ m_counter = in_cnt; }

	int			PartNum( void ) const					{ return m_part_num; }
	void		PartNum( int in_num )					{ m_part_num = in_num; }

	int			PartsToCut( void ) const				{ return m_parts_to_cut; }
	void		PartsToCut( int num )					{ m_parts_to_cut = num; }

	const CString&	ResultPath( void ) const			{ return m_result_path; }
	void		ResultPath( const CString& path );

	int			ResultDigits( void ) const				{ return m_result_digits; }
	void		ResultDigits( int digits )				{ m_result_digits = digits; }

	double		LargePartArea( void ) const				{ return m_large_part; }
	void		LargePartArea( double in_area )			{ m_large_part = in_area; }

	double		LargePartSize( void ) const				{ return m_large_size; }
	void		LargePartSize( double size )			{ m_large_size = size; }

	double		SmallPartArea( void ) const				{ return m_small_part; }
	void		SmallPartArea( double in_area )			{ m_small_part = in_area; }

	double		LargeHoleArea( void ) const				{ return m_large_hole; }
	void		LargeHoleArea( double in_area )			{ m_large_hole = in_area; }

	double		SmallHoleArea( void ) const				{ return m_small_hole; }
	void		SmallHoleArea( double in_area )			{ m_small_hole = in_area; }

	double		Resolution( void ) const;
	void		Resolution( double res );

	int			YieldMin( void ) const					{ return m_yield_min; }
	void		YieldMin( int yield )					{ m_yield_min = yield; }
	bool		YieldTest( void ) const					{ return m_yield_min>0; }

	double		GapTolerance( void ) const				{ return m_gaptol; }
	void		GapTolerance( double tol )				{ m_gaptol = tol; }

	double		MinTool( void ) const					{ return m_mintool; }
	void		MinTool( double min )					{ m_mintool = min; }

	WORD		DidTask( void ) const					{ return m_didtask; }
	void		DidTask( WORD task )					{ m_didtask |= task; }
	void		TryTask( WORD task )					{ m_didtask &= ~task; }
	void		FailTask( WORD task )					{ m_didtask &= ~task; }
	void		DidTaskReset( void )					{ m_didtask = ~0; }

	CString		DidFailTool( void ) const				{ return m_failtool; }
	void		DidFailTool( CString name )				{ m_failtool = name; }

	void		CurrentZoneNumber( int num )			{ m_curr_zone = num; }
	int			CurrentZoneNumber() const				{ return m_curr_zone; }
	//
	// Remnant status
	//
	double		RemnantWidth( void ) const				{ return m_remn_width; }
	double		RemnantLength( void ) const				{ return m_remn_length; }
	double		RemnantArea( void ) const				{ return m_remn_area; }

	const CString&	RemnantPath( void ) const			{ return m_remn_path; }
	int			RemnantDigits( void ) const				{ return m_remn_digits; }

	int			RemnantCounter( void ) const			{ return m_remn_counter; }
	void		RemnantCounter( int in_cnt )			{ m_remn_counter = in_cnt; }
	//
	// Various Flags
	//
	bool		DisplayAnnotation( void ) const			{ return get_flag2(CONFIG_DISPLAYTEXT); }
	void		DisplayAnnotation( bool set )			{ set_flag2(CONFIG_DISPLAYTEXT, set); }

	bool		DisplayParts( void ) const				{ return get_flag1(CONFIG_DISPLAYPARTS); }
	void		DisplayParts( bool set )				{ set_flag1(CONFIG_DISPLAYPARTS, set); }

	// bool		DisplayDebug( void ) const				{ return m_debug_draw; }
	bool		DebugWire() const						{ return ((m_debug_draw & 1) != 0); }
	bool		DebugBmp() const						{ return ((m_debug_draw & 2) != 0); }

	// bool		DidScore( void ) const					{ return get_flag1(CONFIG_DIDSCORE); }
	// void		DidScore( bool set )					{ set_flag1(CONFIG_DIDSCORE, set); }

	bool		DidScore( void ) const					{ return m_did_score; }
	void		DidScore( bool set )					{ m_did_score = set; }

	bool		DidNest( void ) const					{ return get_flag1(CONFIG_DIDNEST); }
	void		DidNest( bool set )						{ set_flag1(CONFIG_DIDNEST, set); }

	bool		DidLarge( void ) const					{ return get_flag1(CONFIG_DIDLARGE); }
	void		DidLarge( bool set )					{ set_flag1(CONFIG_DIDLARGE, set); }

	bool		Filter( void ) const					{ return get_flag1(CONFIG_FILTER); }
	void		Filter( bool set )						{ set_flag1(CONFIG_FILTER, set); }
	
	bool		DropStop( void ) const					{ return get_flag1(CONFIG_DROPSTOP); }
	void		DropStop( bool set )					{ set_flag1(CONFIG_DROPSTOP, set); }

	bool		AreaLessHoles( void ) const				{ return get_flag1(CONFIG_AREAHOLES); }
	void		AreaLessHoles( bool set )				{ set_flag1(CONFIG_AREAHOLES, set); }

	bool		AreaOffset( void ) const				{ return get_flag1(CONFIG_AREAOFFSET); }
	void		AreaOffset( bool set )					{ set_flag1(CONFIG_AREAOFFSET, set); }

	bool		GridSquare( void ) const				{ return get_flag1(CONFIG_GRIDSQUARE); }
	void		GridSquare( bool set )					{ set_flag1(CONFIG_GRIDSQUARE, set); }

	bool		GridBarrier( void ) const				{ return get_flag2(CONFIG_GRIDBARRIER); }
	void		GridBarrier( bool set )					{ set_flag2(CONFIG_GRIDBARRIER, set); }

	bool		GridChain( void ) const					{ return get_flag2(CONFIG_GRIDCHAIN); }
	void		GridChain( bool set )					{ set_flag2(CONFIG_GRIDCHAIN, set); }

	bool		FillSeed( void ) const					{ return get_flag1(CONFIG_FILLSEED); }
	void		FillSeed( bool set )					{ set_flag1(CONFIG_FILLSEED, set); }

	bool		ForceSeed( void ) const					{ return get_flag2(CONFIG_FORCESEED); }
	void		ForceSeed( bool set )					{ set_flag2(CONFIG_FORCESEED, set); }

	bool		Cutback( void ) const					{ return get_flag1(CONFIG_CUTBACK); }
	void		Cutback( bool set )						{ set_flag1(CONFIG_CUTBACK, set); }

	int			CutBackStationID()						{ return m_cut_back_station_id; }
	void		CutBackStationID( int id );

	bool		Remnant( void ) const					{ return get_flag1(CONFIG_REMNANT); }
	void		Remnant( bool set )						{ set_flag1(CONFIG_REMNANT, set); }

	bool		RemnantTopGrain( void ) const			{ return get_flag2(CONFIG_TOPGRAIN); }
	void		RemnantTopGrain( bool set )				{ set_flag2(CONFIG_TOPGRAIN, set); }

	bool		YNegative( void ) const					{ return get_flag1(CONFIG_YNEGATIVE); }
	void		YNegative( bool set )					{ set_flag1(CONFIG_YNEGATIVE, set); }

	bool		NestPosY( void ) const;

	eNestProgression Progression() const				{ return m_progression; }
	void Progression( eNestProgression progression )	{ m_progression = progression; }

	bool		StripLayers( void ) const				{ return get_flag1(CONFIG_STRIPLAYERS); }
	void		StripLayers( bool set )					{ set_flag1(CONFIG_STRIPLAYERS, set); }

	bool		StripTools( void ) const				{ return get_flag2(CONFIG_STRIPTOOLS); }
	void		StripTools( bool set )					{ set_flag2(CONFIG_STRIPTOOLS, set); }

	bool		ShiftPierce( void ) const				{ return get_flag1(CONFIG_SHIFTPIERCE); }
	void		ShiftPierce( bool set )					{ set_flag1(CONFIG_SHIFTPIERCE, set); }

	bool		Progressive( void ) const				{ return get_flag2(CONFIG_PROGRESSIVE); }
	void		Progressive( bool set )					{ set_flag2(CONFIG_PROGRESSIVE, set); }

	bool		RepoLate( void ) const					{ return get_flag1(CONFIG_REPOLATE); }
	void		RepoLate( bool set )					{ set_flag1(CONFIG_REPOLATE, set); }

	bool		RepoShort( void ) const					{ return get_flag1(CONFIG_REPOSHORT); }
	void		RepoShort( bool set )					{ set_flag1(CONFIG_REPOSHORT, set); }

	bool		RepoSmall( void ) const					{ return get_flag2(CONFIG_SMALLREPO); }
	void		RepoSmall( bool set )					{ set_flag2(CONFIG_SMALLREPO, set); }

	bool		RepoToBurn( void ) const				{ return get_flag1(CONFIG_REPOBURN); }
	void		RepoToBurn( bool set )					{ set_flag1(CONFIG_REPOBURN, set); }

	bool		OppositeClamps() const					{ return m_opposite_clamps; }
	void		OppositeClamps( bool set )				{ m_opposite_clamps = set; }

	bool		ClampAround( void ) const				{ return m_consider_clamps; }
	void		ClampAround( bool set )					{ m_consider_clamps = (set != 0); }
	bool		ClampUnder( void ) const				{ return !m_consider_clamps; }

	bool		OneSheet( void ) const					{ return get_flag1(CONFIG_ONESHEET); }
	void		OneSheet( bool set )					{ set_flag1(CONFIG_ONESHEET, set); }

	bool		HardPierce( void ) const				{ return get_flag2(CONFIG_HARDPIERCE); }
	void		HardPierce( bool set )					{ set_flag2(CONFIG_HARDPIERCE, set); }

	int			LeadRestrictions() const				{ return m_lead_restrictions; }
	void		LeadRestrictions( int flags )			{ m_lead_restrictions = flags; }

	bool		PartInPart( void ) const				{ return get_flag1(CONFIG_PARTINPART); }
	void		PartInPart( bool set )					{ set_flag1(CONFIG_PARTINPART, set); }

	bool		LabelName( void ) const					{ return get_flag1(CONFIG_LABELNAME); }
	void		LabelName( bool set )					{ set_flag1(CONFIG_LABELNAME, set ); }

	bool		LabelTag( void ) const					{ return get_flag2(CONFIG_LABELTAG); }
	void		LabelTag( bool set )					{ set_flag2(CONFIG_LABELTAG, set ); }

	double		LabelAngle( void ) const				{ return m_label_angle; }
	void		LabelAngle( double ang )				{ m_label_angle = ang; }

#if (_CI || _NST)
	double		LabelSize( void ) const					{ return m_label_size; }
	void		LabelSize( double size )				{ m_label_size = size; }
#else
	int			LabelSize( void ) const					{ return m_label_size; }
	void		LabelSize( int size )					{ m_label_size = size; }
#endif

	int			LabelTool( void ) const					{ return m_label_tool; }
	void		LabelTool( int tool )					{ m_label_tool = tool; }

	bool		KeepSequence( void ) const				{ return get_flag2(CONFIG_KEEP_SEQ); }
	void		KeepSequence( bool set )				{ set_flag2(CONFIG_KEEP_SEQ, set ); }

	bool		PatternSplit( void ) const				{ return get_flag2(CONFIG_PATTERNSPLIT); }
	void		PatternSplit( bool set )				{ set_flag2(CONFIG_PATTERNSPLIT, set ); }

	bool		FillToRepo( void ) const				{ return get_flag2(CONFIG_FILLTOREPO); }
	void		FillToRepo( bool set )					{ set_flag2(CONFIG_FILLTOREPO, set ); }

	bool		FillToSheet( void ) const				{ return get_flag2(CONFIG_FILLTOSHEET); }
	void		FillToSheet( bool set )					{ set_flag2(CONFIG_FILLTOSHEET, set ); }
	//
	// FillParts is true if we have filler parts in this nest
	//
	bool		FillParts( void ) const					{ return get_flag1(CONFIG_FILLPARTS); }
	void		FillParts( bool set )					{ set_flag1(CONFIG_FILLPARTS, set); }

	bool		RemnantFirst( void ) const				{ return get_flag1(CONFIG_REMNANTFIRST); }
	void		RemnantFirst( bool set )				{ set_flag1(CONFIG_REMNANTFIRST, set); }

	bool		GridNest( void ) const					{ return get_flag1(CONFIG_GRIDNEST); }
	void		GridNest( bool set )					{ set_flag1(CONFIG_GRIDNEST, set); }

	bool		NewLine( void ) const					{ return get_flag1(CONFIG_NEWLINE); }
	void		NewLine( bool set )						{ set_flag1(CONFIG_NEWLINE, set); }

	bool		BadRatio( void ) const					{ return get_flag2(CONFIG_BADRATIO); }
	void		BadRatio( bool set )					{ set_flag2(CONFIG_BADRATIO, set); }

	bool		OffSheet( void ) const					{ return get_flag2(CONFIG_OFFSHEET); }
	void		OffSheet( bool set )					{ set_flag2(CONFIG_OFFSHEET, set); }

	bool		Oversize( void ) const					{ return get_flag2(CONFIG_OVERSIZE); }
	void		Oversize( bool set )					{ set_flag2(CONFIG_OVERSIZE, set); }

	bool		SingleSheet( void ) const				{ return get_flag2(CONFIG_SINGLESHEET); }
	void		SingleSheet( bool set )					{ set_flag2(CONFIG_SINGLESHEET, set); }
	//
	// Stuff managed per-pass
	//
	void		PassInit( int in_size );

	double		Narrow( int in_pass ) const				{ return m_narrow[in_pass]; }
	void		Narrow( int in_pass, double in_size );
	double		MostNarrow( void ) const;

	bool		Active( int in_pass ) const				{ return (m_active[in_pass] != 0); }
	void		Active( int in_pass, bool active )		{ m_active[in_pass] = active; }

	int			PassNum( void ) const					{ return m_pass_num; }
	void		PassNum( int in_pass )					{ m_pass_num = in_pass; }

	int			Pass( void ) const						{ return m_pass; }
	void		Pass( int in_pass )						{ m_pass = in_pass; }

	eGridMode	GridMode(void) const					{ return m_gridmode; }
	void		GridMode(eGridMode mode)				{ m_gridmode = mode; }
	void		GridFlags(eGridMode grid_mode);

	const C2dBox&	FillBarrier( void ) const			{ return m_fill_barrier; }	// in GRID
	void		FillBarrier( const C2dBox& barrier )	{ m_fill_barrier = barrier; }

	// An optimization that allows you to avoid generating
	// (and processing) seeds along the edge of a zone.
	// Barrier seeds need only be processed until such time
	// as we have punched a part into the sheet.  From that
	// time forward, subsequent seeds are generated from the
	// parts that have been punched into the sheet.
	bool MustSeedBarrier() const						{ return m_must_seed_barrier; }
	void MustSeedBarrier( bool must_seed_barrier )		{ m_must_seed_barrier = must_seed_barrier; }

	// Results Management.  Moved from the Nest Managers, to be in common
	CReturn ClearResults( void );

#if (_CI || _NST)
	void	SavePassResults(
							int					sheet_id,
							const CSheet&		sheet,
							const CPartBin&		partbin );
#else
	void	SavePassResults(
							const CSheet&		sheet,
							const CPartBin&		partbin );
#endif

	void	SaveFinalResults(
							const CSheet&		sheet,
							const CPartBin&		partbin );
	void	SaveHoldResults(
							const CPartBin&		partbin );

	bool	DoSheetHold(
						const CSheet&		sheet,
						const CPartBin&		partbin );
	//
	// THIS DOES NOT BELONG HERE... but NestConfig is common to the two places it
	// is used, NestMgr and PartBin, and I didn't want to make a new file.
	// TODO:  Put into some Transform Support library?
	//
	void	ChangeModelY( double	x_axis,
							CModel* model,
							bool	flip,
							bool	geo ) const;

	void HaveSeeds( bool have_seeds )	{ m_have_seeds = have_seeds; }
	bool HaveSeeds() const				{ return m_have_seeds; }

	void HaveLargeParts( bool have_large_parts )	{ m_have_large_parts = have_large_parts; }
	bool HaveLargeParts() const						{ return m_have_large_parts; }

	void FirstPartInspection( int state )			{ m_first_part_inspection = state; }
	int  FirstPartInspection() const				{ return m_first_part_inspection; }

	void ShiftedOntoSheet( bool shifted )			{ m_shifted_onto_sheet = shifted; }
	bool ShiftedOntoSheet() const					{ return m_shifted_onto_sheet; }

	void PlacementFailed( bool failed )				{ m_placement_failed = failed; }
	bool PlacementFailed() const					{ return m_placement_failed; }

	bool UseBooleans() const  { return m_use_booleans; }

public:

	static bool IsBitmapNest();

	// Special for ITI / CCS.
	static void CcsNesting( bool is_ccs_nesting );
	static bool CcsNesting();

private:

	eFill		FillDir( void ) const				{ return m_fill_dir; }
	void		FillDir( eFill fill )				{ m_fill_dir = fill; }

#if BEFORE_V18_0_59_1
	bool		FillBottom( void ) const			{ return get_flag1(CONFIG_FILLBOTTOM); }
	void		FillBottom( bool set )				{ set_flag1(CONFIG_FILLBOTTOM, set); }
#endif

private:  // disabled.

	CNestConfig( const CNestConfig& );
	const CNestConfig& operator = ( const CNestConfig& );
	int operator == ( const CNestConfig& ) const;
	int operator != ( const CNestConfig& ) const;

private:

	CReturn	LoadSetup();

#if (_CI || _NST)
	CReturn	LoadSetupCI();
#endif

	CReturn	LoadRemnant();

	// Flag access
	bool get_flag1( DWORD bit) const		{ return (m_flag1&bit)!=0; }
	void set_flag1( DWORD bit, bool set )	{ if (set) {m_flag1|=bit;} else {m_flag1&=~bit;} }

	bool get_flag2( DWORD bit) const		{ return (m_flag2&bit)!=0; }
	void set_flag2( DWORD bit, bool set )	{ if (set) {m_flag2|=bit;} else {m_flag2&=~bit;} }

	int AddStdTable(
			const CString& table_name,
			const CString& link_field_name,
			CDaoDB* db );
	
	int AddStdRecord(
			int				which_table,
			const CString&	internal_name,
			const CString&	display_name,
			const CString&	data_value,
			int				data_type,
			const CString&	options,
			int				flags,
			const CString&	link_field_name,
			int				link_field_value,
			CDaoDB*			db );

private:

	static bool	m_bitmap_nest;
	static bool m_is_ccs_nesting;

private:

	// Setup Data
	CString		m_cmdb_path;
	CString		m_pdb_path;

	int			m_machkey;
#if (_CI || _NST)
	CString		m_nesting_ids;
	int			m_toolSetupkey;
#endif

#if (_NST)
	int			m_layer_setup_id;
#endif

	int			m_nestkey;
	CString		m_result;

	bool		m_did_score;

	// Nesting control data...
	DWORD		m_flag1;
	DWORD		m_flag2;
	eGridMode	m_gridmode;

	double		m_spacing;
	double		m_resolution;
	double		m_border[BORDER_NUM];
	double		m_gaptol;
	double		m_mintool;

	double		m_chain_off;
	int			m_chain;

	C2dCoord	m_hold_default;

	double		m_hfactor;
	double		m_vfactor;

	WORD		m_display_mode;

	eNestOrder	m_nest_order;
	eFill		m_fill_dir;

	eNestProgression m_progression;

	int			m_yield_min;

	double		m_remn_width;
	double		m_remn_length;
	double		m_remn_area;
	CString		m_remn_path;
	int			m_remn_digits;
	int			m_remn_counter;

	int			m_cut_back_station_id;

	double		m_min_trap;
	double		m_max_trap;

	int			m_lead_setup;

	CString		m_result_path;
	int			m_result_digits;

	double		m_repo_overlap;
	double		m_repo_travel;

	double		m_label_angle;
#if (_CI || _NST)
	double		m_label_size;	// physical text size
#else
	int			m_label_size;	// font size
#endif
	int			m_label_tool;

	// Nest and Part status information
	int			m_counter;		// Part counter...
	int			m_part_num;		// How many distinct parts in this nest?
	int			m_parts_to_cut;	// How many countable instances to cut?

	WORD		m_didtask;
	CString		m_failtool;

	double		m_large_part;	// Some statistics
	double		m_small_part;
	double		m_large_hole;
	double		m_small_hole;

	double		m_large_size;

	int			m_pass_num;		// How many passes in this nest?
	int			m_pass;			// Current pass

	double*		m_narrow;
	bool*		m_active;

//	int			m_zone_num;		// How many repo zones?

	// Machine data...
	C2dBox		m_fill_barrier;	// Artificial seeding limits, in GRID

	bool		m_clamp[4];
	double		m_clamp_pos[4];

	CVarList	m_mach_attrib;

	int			m_debug_draw;

	bool		m_must_seed_barrier;

	bool		m_have_seeds;
	bool		m_have_large_parts;

	int			m_lead_restrictions;
	int			m_curr_zone;

	bool		m_opposite_clamps;
	int			m_first_part_inspection;

	bool		m_consider_clamps;

	bool		m_has_holddowns;
	int			m_clamp_count;

	bool		m_shifted_onto_sheet;
	bool		m_placement_failed;

	bool		m_use_booleans;
};

// NestConfigGet() wasn't introduced until V21; its existence simply
// reduces the need to pass a nest-config object as a function argument.
dllExport CNestConfig& NestConfigGet();
