#if !defined(_NESTINGPART_H)
#define _NESTINGPART_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Common.h"
#include "NestConfig.h"
#include "ViewMgr.h"

#include "DbHole.h"
#include "Model.h"
#include "GeoPoly.h"
#include "ToolHit.h"

// ==================================================================

#if !defined(_NESTPART_H)

const int LARGE_PART_PASS	= 0;
const int SPECIAL_PASS		= 1;
const int COMMON_PASS		= 2;
const int FILLER_PASS_DELTA	= 20;
const double PASS_RESOLUTION = 20.0; // number of pass divisions to use (20 = 5%)

#endif


const WORD FLAG_FILLER		= 0x0001;
const WORD FLAG_LARGE		= 0x0002;
const WORD FLAG_SMALL		= 0x0004;
const WORD FLAG_TOOLED		= 0x0008;
const WORD FLAG_TOOLMATCH	= 0x0010;
const WORD FLAG_SPECIAL		= 0x0020;
const WORD FLAG_PARTOUTLINE	= 0x0040;

// -----------------------------

enum eAreaCalc
{
	AREA_SUM,
	AREA_MAX,
	AREA_MIN
};


// ==================================================================

class dllExport CNestingPart
{
public:

	CNestingPart();

	virtual ~CNestingPart();

	// Part representations
	const CModel&	Model( int hit=0 ) const		{ return *(m_modelarray[hit]); }
	CModel*			pModel( int hit=0 ) const		{ return m_modelarray[hit]; }
	void			Model( CModel* in_model )		{ m_modelarray.DestructiveFlush(); m_modelarray.Append(in_model); }
	int				Count( void ) const				{ return m_modelarray.Count(); }

	CToolHit*				ToolHit(int hit) const						{ return m_toolhitarray[hit]; }
	const CGeoPolyArray*	ToolHitPoly( int hit, int depth ) const		{ return m_toolhitarray[hit]->PolysGet(depth); }

	const CDexGrid&	Grid( int hit=0 ) const			{ return *(m_toolhitarray[hit]->Grid()); }
	CDexGrid*		pGrid( int hit=0 ) const		{ return m_toolhitarray[hit]->Grid(); }

	const C2dBox&	Extent( void ) const			{ return m_extent; }
	void			Extent( const C2dBox& ext )		{ m_extent = ext; }

	// Part Manipulations... 
	CReturn		Simplify( CNestConfig& config );
	CReturn		Rotate();
	CReturn		Translate();

	CReturn		Lead( CNestConfig& config );
	CReturn		StripLeads(void);

	CReturn		Generate( int hit );
	CReturn		GenerateBox( int hit );

	CReturn		Accessorize( CNestConfig& config, int hit );

	CReturn		Assemble( const CNestConfig& config, int hit );
	CReturn		AddSpace( const CNestConfig& config, int hit, int depth );

	void		ReOrder( const CNestConfig& config );

	CReturn		Filter( int hit, double area );
	CReturn		Slice( CNestConfig& config, CViewMgr& view, int hit);

	double TestResolution(
		const CNestConfig&	config, 
		double				res,
		CViewMgr&			view );

	CReturn		CopyTo( int hit, CModel* dest_model, CDbContainer* owner );
	CReturn		CopyTo( int hit, CModel* dest_model, CDbContainer* owner, EDbEntityType from, EDbEntityType to );

	void GapEntitiesRemove();

	// 2006.04.23 (PE) -- Added Representation to provide various ways of
	// assisting the end-user in getting better nests.
	// (0) As Given / (1) Bounding Box / (2) Convex Hull
	void RepresentationAdjust();

	// For Panel Nesting
	CReturn		SimplifyRotation( void )			{ return do_simplify_rotation( m_modelarray[0] ); }

	// Data Access
	const CString&	Filepath( void ) const				{ return m_filepath; }
	void			Filepath( const CString& fq_path );

	CString			Name( void ) const					{ return m_name; }
	void			Name( const CString& name )			{ m_name = name; }

	CString			Label( void ) const					{ return m_label; }
	void			Label( const CString& label )		{ m_label = label; }

	void			Id( ID id )							{ m_id = id; }
	ID				Id( void ) const					{ return m_id; }

	void			SortedID( ID id )					{ m_sorted_id = id; }
	ID				SortedID() const					{ return m_sorted_id; }

	// 2005.03.06 (PE) -- added Actual_ID to the Parts tables to allow
	// the CI reporting system to trace data back to the ComponentVar table.
	void			ActualID( ID id )					{ m_actual_id = id; }
	ID				ActualID( void ) const				{ return m_actual_id; }

	void			Index( int idx )					{ m_idx = idx; }
	int				Index( void ) const					{ return m_idx; }

	// PreHit holds the index of the part rotation to force during pre-nesting
	// PreNest is a flag that determines if this part is pre-nested or not.
	void			PreHit(int hit)						{ m_pre_hit = hit; }
	int				PreHit(void) const					{ return m_pre_hit; }

	double		Cost( void ) const			{ return m_cost; }
	void		Cost( double in_cost )		{ m_cost = in_cost; }

	// 2006.04.23 (PE) -- Added Representation to provide various ways of
	// assisting the end-user in getting better nests.
	int			Representation( void ) const	{ return m_representation; }
	void		Representation( int rep )		{ m_representation = rep; }

	int			TotalQuantity( void ) const	{ return m_totquant; }
	void		TotalQuantity( int total )	{ m_totquant = total; }

	int			Quantity( void ) const		{ return m_quantity; }
	void		Quantity( int in_qty )		{ m_quantity = in_qty; }

	// Maybe should wrap these methods with #if _CI  (?)
	// NOTE: This value is simply passed on to CCiModelConvertor.
	int			Exclusions( void) const		{ return m_exclusions; }
	void		Exclusions( int excl )		{ m_exclusions = excl; }

	// PREREQUISITE: calcArea() must have been called.
	double		OuterKerfArea() const		{ return m_outer_kerf_area; }

	// ---- Flags ---------

	bool		Filler( void ) const		{ return read_flag(FLAG_FILLER); }
	void		Filler( bool in_fill )		{ write_flag( FLAG_FILLER, in_fill ); }

	bool		Large( void ) const			{ return read_flag(FLAG_LARGE); }
	void		Large( bool large )			{ write_flag(FLAG_LARGE, large); }

	bool		Small( void ) const			{ return read_flag(FLAG_SMALL); }
	void		Small( bool in_small )		{ write_flag(FLAG_SMALL, in_small); }

	bool		Tooled( void ) const		{ return read_flag(FLAG_TOOLED); }
	void		Tooled( bool tooled )		{ write_flag(FLAG_TOOLED, tooled); }

	bool		ToolMatch( void ) const		{ return read_flag(FLAG_TOOLMATCH); }
	void		ToolMatch( bool tmatch )	{ write_flag(FLAG_TOOLMATCH, tmatch); }

	bool		Special( void ) const		{ return read_flag(FLAG_SPECIAL); }
	void		Special( bool special )		{ write_flag(FLAG_SPECIAL, special); }

	bool		PartOutline( void ) const	{ return read_flag(FLAG_PARTOUTLINE); }
	void		PartOutline( bool special )	{ write_flag(FLAG_PARTOUTLINE, special); }

	// PreHit holds the index of the part rotation to force during pre-nesting
	// PreNest is a flag that determines if this part is pre-nested or not.
	bool		PreNest() const				{ return m_prenest; }
	void		PreNest(bool set)			{ m_prenest = set; }

	int			FirstPartInspection() const			{ return m_first_inspection; }
	void		FirstPartInspection( int state )	{ m_first_inspection = state; }

	// ----------------

	int			Used( void ) const			{ return m_used; }
	void		Used( int in_num )			{ m_used = in_num; }

#if BEFORE_V18
	int			Pass( void ) const			{ return Large()?LARGE_PART_PASS:m_pass; }
	void		Pass( int in_pass )			{ m_pass = in_pass; }
#else
	int			Pass( void ) const			{ return m_pass; }
	void		Pass( int in_pass )			{ m_pass = in_pass; }
#endif

	// Standard usage: (2) (0, 180), (3) (0, 120, 240), (4) (0, 90, 180, 270), etc.
	// ITI usage: (-2) (0, 90), (-3) (0, 45, 90), (-4) (0, 30, 60, 90), etc.
	// No rotation: abs(n) < 2
	int			Rotation( void ) const		{ return m_rotation; }
	void		Rotation( int in_rot )		{ m_rotation = ((abs(in_rot) < 2) ? 0 : in_rot); }

	double		StartAngle( void ) const	{ return m_startang; }
	void		StartAngle( double in_ang )	{ m_startang = in_ang; }

	bool		Mirror( void ) const		{ return m_mirror; }
	void		Mirror( bool mirror )		{ m_mirror = mirror; }

	bool		StartMirror( void ) const	{ return m_startmirror; }
	void		StartMirror( bool mirror )	{ m_startmirror = mirror; }

	double		Area( void ) const			{ return m_area; }
	CReturn		calcArea( CNestConfig& in_config );

	bool		Error() const				{ return ( !m_error_msg.IsEmpty() ); }
	void		Error( const CString& msg )	{ m_error_msg += msg; }

	void Oversized( bool is_oversized )		{ m_is_oversized = is_oversized; }
	bool Oversized() const					{ return m_is_oversized; }

	const CString& ErrorMsg() const			{ return m_error_msg; }

	// ---- Linking and Pre-Nesting ---------
		
	CReturn GridLink( CNestConfig& config, int hit );

	// ---- Debugging ---------

	CReturn		DebugToModel( CViewMgr* view, CModel* model );
	CReturn		DebugToModel2( CViewMgr* view, CModel* model );

protected:

private:

	CReturn		exact_generate( int hit );
	CReturn		bitmap_generate( int hit );

	CReturn		exact_slice( CNestConfig& config, CViewMgr& view, int hit);
	CReturn		bitmap_slice( CNestConfig& config, CViewMgr& view, int hit);

	CReturn		do_pattern_explode( CModel* model );
	CReturn		do_strip_layers( CModel* model );
	CReturn		do_transform( CModel* model );
	CReturn		do_simplify_tooling( CNestConfig& in_config, CModel* model );
	CReturn		do_simplify_geometry( CNestConfig& config, CModel* model );
	CReturn		do_simplify_rotation( CModel* model );
	CReturn		do_simplify_workplane( CModel* model );

	CReturn		do_rotate( CModel* model, double rotang, bool mirror );

	CReturn		do_generate_outline( const CNestConfig& config );
	CReturn		do_generate_toolhits( const CNestConfig& config );

	void		do_toolhit( CDbEntity* db_ent, bool accessory );
	void		do_toolhit( CDbProfile* db_prof, bool accessory );

	void		do_toolhit_nibble( CDbEntity* db_ent, bool accessory );

	bool		do_burn_test( CDbEntity* db_ent );
	void		do_toolhit_burn( CDbContainer* db_container, bool accessory );
	void		do_toolhit_burn( CDbEntity* db_ent, bool accessory );
	void		do_toolhit_burn( CProfile* profile, CDbTool* tool, bool accessory );
	void		do_outline( CDbProfile* db_profile, int hit );

	double		do_calc_area( CGeoPolyArray* geopoly_array, eAreaCalc calc );

	bool	read_flag( WORD flag ) const		{ return ((m_flag&flag) != 0); }
	void	write_flag( WORD flag, bool set )	{ set?(m_flag|=flag):(m_flag&=~flag); }

	bool IsPartOutline( const CDbEntity& dbEntity );
	bool IsToolPath( const CDbEntity& dbEntity );
	void ContainerToGeopoly( const CDbContainer& dbContainer, CGeoPoly* geoPoly );
	void PolyTransfer( CGeoPoly* tmp, CGeoPoly* geoPoly );
	C2dCoord InteriorPoint( const CGeoPoly& geoPoly );

	void KerfCopy( int hit, eToolhit which );
	CDbHole* PierceHoleGet( const CToolHit& toolhit, const CDbHole* hole );

	bool CanClose( const CDbProfile& dbProfile ) const;

	bool TestResolution_core(
		const CNestConfig&	config, 
		double				res,
		CViewMgr&			view );

	void LeadEntitiesDelete( CModel* model );

private:
	// Disabled.
	CNestingPart( const CNestingPart& );
	const CNestingPart& operator = ( const CNestingPart& );
	int operator == ( const CNestingPart& ) const;
	int operator != ( const CNestingPart& ) const;

private:

	CString		m_filepath;
	CString		m_label;
	CString		m_name;

	ID			m_id;
	ID			m_sorted_id;
	int			m_actual_id;			// Only used by CI (and maybe NST)

	int			m_idx;					// index into CPartBin
	int			m_pre_hit;

	CModelArray		m_modelarray;		// Model holding original geometry; NestPart OWNS this
	CGeoPoly*		m_toolhits;			// Temporary toolhit outline created by Generate; NestPart OWNS this
	CToolHitArray	m_toolhitarray;		// Poly holding the tool-hit outlines; NestPart OWNS this

	double		m_cost;			

	WORD		m_flag;

	int			m_quantity;		// Quantity of this part to make (-1 for infinite, forces fill TRUE)
	int			m_totquant;		// Original quantity set by the PartBin... used for display purposes only
	int			m_used;			// Quantity of this part cut this pass
	int			m_pass;			// Pass to cut part in; 0 Large Parts, 1 Special Parts, 2+ All Others

	int			m_rotation;		// Number of rotations to cut in (angle == 360/n)
	double		m_startang;		// Initial rotation, in degrees

	bool		m_mirror;		// True if we try mirrors of the part. Doubles count.
	bool		m_startmirror;	// True if we mirror before we start.  Does not effect count.

	C2dBox		m_extent;		// Set externally... TODO:  Move over to calcArea() ??

	double		m_area;			// Part area (possibly less holes)
	double		m_min_area;		// Smallest hole; HIT_PART_INSIDE
	double		m_max_area;		// Outside profile; HIT_PART_OUTSIDE

	CString		m_error_msg;

	// Maybe should wrap this declaration with #if _CI  (?)
	int			m_exclusions;

	// Experimental
	bool		m_filled_gaps;
	CDbTool*	m_gap_tool;
	int			m_use_gap_tool;
	int			m_use_tool_extruder;

	// 2005.01.25 (PE) -- Nestings "Pierce Hole / Exclude Kerf" option
	// failed to yield good nest.  In short, we now offset the pierce
	// hole to its outside so that HIT_KERF_OUTSIDE is better adjusted.
	double		m_kerf_adjust;
	double		m_outer_kerf_area;

	// 2006.04.23 (PE) -- Added Representation to provide various ways of
	// assisting the end-user in getting better nests.
	int			m_representation;

	bool		m_prenest;

	// 'First Part Inspection' flag.
	// (0) Not inspected.
	// (1) Considered by SeedsGenerate().
	// (2) Has been inspected.
	int			m_first_inspection;

	bool		m_is_oversized;
};

typedef CDynamicArray<CNestingPart*> CNestingPartArray;
typedef CIndxList<CNestingPart*> CNestingPartList;

#endif
