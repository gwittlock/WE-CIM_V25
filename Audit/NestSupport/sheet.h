#if !defined(_SHEET_H)
#define _SHEET_T

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common.h"
#include "DaoDB.h"
#include "3dBox.h"
#include "ToolHit.h"
#include "NestingPart.h"
#include "DbCommand.h"

#include "profiler.h"

#include "SeedList.h"
#include "PartPlace.h"
#include "SeedMgr.h"
#include "NestedArea.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

enum ePanelAxis
{
	PANEL_COLUMN,		// Y axis
	PANEL_ROW			// X axis
};

enum eBumpDir
{
	BUMP_X = ORD_X,
	BUMP_Y = ORD_Y
};

enum eSheetProc
{
	SPROC_SCORE = 0,
	SPROC_BUMP = 1
};

enum eReservePoly
{
	RESERVE_BARRIER,
	RESERVE_CLAMP,
	RESERVE_REMNANT,	// MUST BE LAST, right before NUM
	RESERVE_NUM
};

enum eSheetRejection
{
	IS_ONSHEET			= 0,
	STARTED_OFFSHEET	= 1,
	SLIPPED_OFFSHEET	= 2
};

enum eNestingPhase
{
	PHASE_MAIN    = 0,
	PHASE_GRID    = 1,
	PHASE_PRENEST = 2
};

typedef CDynamicArray<double> CDoubleArray;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CSheet
{
public:

	CProfiler* prof;


	CSheet();
	virtual ~CSheet();

	CReturn		Init( CModel* model );

	void		ViewMgr( CViewMgr* view_mgr );

	void		SeedMgr( CSeedMgr* seed_mgr );
	CSeedMgr*	SeedMgr();

	CReturn		Load( const CNestConfig& config );
	CReturn		LoadStock( const CNestConfig& config, int in_matkey );
	CReturn		LoadRemnant( const CNestConfig& config, int in_remnkey );

	CReturn		LoadCI( const CNestConfig& config );
	CReturn		LoadStockCI( const CNestConfig& config, int in_matkey );

	CReturn		Prepare( const CNestConfig& config, CViewMgr& view, CModel& proto );
	CReturn		PrepareBlank( const CNestConfig& config, C2dBox& extent, CModel& proto );
	CReturn		PrepareStock( const CNestConfig& config, CModel& proto );
	CReturn		PrepareRemnant( const CNestConfig& config, CModel& proto );

	CReturn		CreateStock( CModel& proto, C3dBox& extent, CModel* model );

	CReturn		Next( const CNestConfig& config );

	CReturn		FillBarrier( CNestConfig& config, const C2dBox& extent );
	CReturn		ClampBarrier( const C2dBox* extent, bool reset );

	CReturn		TestBarrier();

	void NestingPhaseSet( eNestingPhase phase );
	eNestingPhase NestingPhaseGet( ) const;

	void SeedsShift( eSeedBank which_bank, C2dVec delta );

	const CSeed& Seed( void ) const			{ return m_this_seed; }

	void SeedPolyConfig( bool retain )		{ m_retain_seeds = retain; }

	int SeedPoly(
			const CNestConfig&	config,
			CGeoPoly*			poly,
			double				flip,
			bool				box_seed );

	int SeedBarrier( CNestConfig& config, CGeoPoly* poly, double flip );
	int SeedPartEdge( CNestConfig& config );
	void SeedShift( C2dVec delta );

	bool SeedNext( CNestConfig& config, bool kill );

	CPartPlace Score(
		CNestConfig&	config,
		const C3dCoord&	place,		// in WORLD...
		CToolHit*		toolhit,
		CNestedArea*	area_to_nest,
		bool			ignore_clamp,
		bool			hole );

	bool HaveClampPolys();

	ePartFit	TestFit(
						const CNestConfig& config,
						const C3dCoord& shift,
						const CToolHit& toolhit,
						const CNestedArea&	area_to_nest );

	double		Bump( 
						const C3dCoord&		shift,
						const CToolHit&		toolhit,
						const CNestedArea&	area_to_nest,
						eBumpDir			dir,
						double				max_bump );

	CDbCommand* PunchInstance(
					const CNestConfig&	config,
					const CPartPlace&	part_place,
					int					zone,
					bool				torch_shift,
					CViewMgr&			view );

	CReturn		ClearInstance( CDbCommand* inst );

	CNestedArea* PunchGeo(
					const CNestConfig&	config,
					const CPartPlace&	part_place,
					CNestedArea*		area_to_nest,
					CViewMgr&			view );

	CReturn		Scan( 
					const CNestConfig&	config,
					const CDexGrid&		target,
					const C3dCoord&		place,
					CViewMgr&			view );


	CReturn		Filter( 
						const CNestConfig&	config,
						const C2dBox&		mer,
						CViewMgr&			view );

	CReturn		Label( const CPartBin& partbin );

	CReturn		DropStop( const CNestConfig& config );

	CReturn		Save(
						const CString&		in_result,
						const CNestConfig&	config,
						int					in_num );

	// Data Access
	CString		PathName( void ) const			{ return m_pathname; }
	void		PathName( CString& pathname )	{ m_pathname = pathname; }

	CString		Name( void ) const;

	int			TypeID( void ) const		{ return m_typeid; }
	const CString& Description( void ) const{ return m_descrip; }
	double		CostPerUnit( void ) const	{ return m_costper; }

	double		Length( void ) const		{ return m_extent.Dx(); }
	double		Width( void ) const			{ return m_extent.Dy(); }
	double		Thick( void ) const			{ return m_extent.Dz(); }
	double		Weight( void ) const		{ return m_weight; }

	ePanelAxis	LongAxis( void )		{ return (Width() > Length()) ? PANEL_COLUMN : PANEL_ROW; }
	ePanelAxis	ShortAxis( void )		{ return (Width() > Length()) ? PANEL_ROW : PANEL_COLUMN; }

	const C3dBox&	Extent( void ) const	{ return m_extent; }

	C2dBox	BoxUser() const;

	const C2dBox&	WorkZone() const;
	void		WorkZone( const C2dBox& zone );

	const C2dBox&	FitZone( void ) const		{ return m_fitzone; }	// WORLD
	void		FitZone( const CNestConfig& config, const C2dBox& zone, double delta );

	int			Quantity( void ) const		{ return m_quantity; }
	void		Quantity( int in_quantity )	{ m_quantity = in_quantity; }

	double		Cutback( void ) const		{ return m_cutback; }
	void		Cutback( double in_back )	{ m_cutback = in_back; }

	double		Area( void ) const;
	double		AdjustedArea( void ) const	{ return m_cutback * Width(); } // Area of sheet used, in front of cutback

	int			Repeat( void ) const		{ return m_repeat; }
	void		Repeat( int in_repeat )		{ m_repeat = in_repeat; }

	// Current remnant/material information
	bool		IsRemnant( void ) const		{ return (m_remnkey >= 0); }
	int			RemnantId( void ) const		{ return m_remnant_array[m_list_index]; }
	int			MaterialId( void ) const	{ return m_stock_array[m_list_index]; }

	// List remnant/material information
	int			RemnantCount( void ) const	{ return m_remnant_array.GetSize(); }
	int			RemnantId( int idx ) const	{ return m_remnant_array[idx]; }
	int			MaterialCount( void ) const	{ return m_stock_array.GetSize(); }
	int			MaterialId( int idx ) const	{ return m_stock_array[idx]; }

	const CModel&	Model( void ) const		{ return *m_model; }
	CModel*			pModel( void )			{ return m_model; }

	void		Filter( 
						const CNestConfig&	config,
						double		in_scrap,
						CViewMgr&	view )	
											{ pGrid()->Filter( config,
																C2dCoord( 0,0 ),
																C2dCoord( pGrid()->Width(), pGrid()->Height() ),
																in_scrap,
																view ); }


	const CDexGrid&	Grid( void ) const		{ return *(m_toolhit->Grid()); }
	CDexGrid*		pGrid( void )			{ return m_toolhit->Grid(); }

	const CToolHit* ToolHit() const { return m_toolhit; }

	CGeoPoly* ReservePart( eReservePoly num );
	CGeoPoly* ReserveKerf( eReservePoly num );

	CGeoPolyArray* ToolHitPoly( int depth ) const	{ return m_toolhit->PolysGet(depth); }

	C2dBox	getRecentPart()				{ return m_recentpart; }
	void	clearRecentPart()			{ m_recentpart = C2dBox(); }

	CReturn	 DebugToModel( CModel* model, CViewMgr* view );

	CPartPlace* PartPlaceGet() const;
	void PartPlacePush();
	void PartPlacePop();

	bool DidPlace() const;
	void DidPlace( bool did_place );

	CSeed NextSeedGet();

	int GridScore(
			eNestProgression	progression,
			const C2dCoord&		pt ) const;
	
	bool OffSheetRejection(
			const CNestConfig&	config,
			C2dBox				part_shift_mer,
			bool				test_vert,
			double*				vert_shift,
			double*				left_shift );

	//--- For debugging---
	void SheetRedraw() const;

	eSheetRejection RejectionScore(
			const CNestConfig&	config,
			const C2dBox&		part_shift_mer,
			int					shift_counter );

	ePartFit FinalRejectionScore(
			const C2dCoord&	place,
			const C2dBox&	part_mer );

	CReturn OverlapScore( const CNestConfig& config, CPartPlace* best, CPartPlace* trial ) const;

	void ScoresScrutinize(
		const CNestConfig& config, CPartPlace* best, CPartPlace* candidate );

	void ChopListReduce( CDoubleArray* chop ) const;

	//=-=-=-=-=
	// This api may eventually change.
	//=-=-=-=-=
	CNestedArea* SheetNestedArea() const { return m_nested_area; }
	int NestedAreasCount() const;
	CNestedArea* NestedAreaGet( int indx ) const;
	CGeoPoly* MaterialPoly() const	{ return &(((CSheet*) this)->m_material_poly); }

	// Blah, not the best ... but it gets the job done :-(
	void ConfigSet( const CNestConfig* config );

	CDbFeature* CutbackWorkzoneCreate( CDbLine* cutback_line );

private:  // disabled

	CSheet( const CSheet& );
	const CSheet& operator = ( const CSheet& );
	int operator == ( const CSheet& ) const;
	int operator != ( const CSheet& ) const;

private:

	static int compare_double( const void* u, const void* v );

private:

	CReturn MaterialCreate( double dx, double dy, double z, CModel* model );

	CToolHit* build_toolhit( CGrid* grid );

	void	set_pattern(const CNestConfig& config,
						CNestingPart* npart, 
						CToolHit* toolhit, 
						CDbPattern* dbPattern, 
						const CString& prefix);

	CDbCommand*	put_pattern( const CNestConfig& config,
							const C3dCoord& shift, 
							int zone,
							CDbWorkplane* dbWork,
							CDbTool* dbTool,
							CDbPattern* pattern,
							CViewMgr& view );

	int do_seed(
			eSeedBank	primary_bank,
			eSeedBank	secondary_bank,
			CGeoPoly*	poly,
			C2dCoord*	center,
			bool		box_seed,
			double		flip );
	
	int do_seed_poly(
			CGeoPoly*	poly,
			C2dCoord*	center,
			double		flip,
			eSeedBank	primary_bank,
			eSeedBank	secondary_bank );

	void seed_it( eSeedBank which_bank, double x, double y, int corner );
	C2dCoord seed_it_salvage( const C2dCoord& grid );

	int do_seed_hline( 
			eSeedBank			primary_bank,
			eSeedBank			secondary_bank,
			const CGeoLine&		raw_line, 
			const C2dUnitVec&	pvec, 
			const C2dUnitVec&	svec,
			const C2dCoord*		center,
			double				flip );

	int do_seed_hstrip( 
			eSeedBank	which_bank,
			double		step, 
			int			corner, 
			double		from_x, 
			double		to_x, 
			double		at_y );

	int do_seed_vline(
			eSeedBank			primary_bank,
			eSeedBank			secondary_bank,
			const CGeoLine&		raw_line, 
			const C2dUnitVec&	pvec, 
			const C2dUnitVec&	svec,
			const C2dCoord*		center,
			double				flip );

	int do_seed_vstrip( 
			eSeedBank	which_bank,
			double		step, 
			int			corner, 
			double		from_y, 
			double		to_y, 
			double		at_x );

	int do_seed_arc( 
		eSeedBank		which_bank,
		int				corner,
		const CGeoArc&	geoArc );

	int do_seed_decompose( 
			eSeedBank			primary_bank,
			eSeedBank			secondary_bank,
			const CGeoCurve&	raw_curve, 
			const C2dUnitVec&	pvec, 
			const C2dUnitVec&	svec,
			const C2dCoord*		center,
			double				flip );


	void render_part( CViewMgr&	view, CDbEntity* db_ent );

	// Some geometry utilities that actually belong somewhere else...
	void	flatten( CDbEntityList* list, CDbEntity* ent, int depth ) const;
	void	clear_tag( CDbEntity* ent ) const;
	void	show( CDbEntity* ent ) const;

	CPartPlace exact_score( 
					CNestConfig&	config,
					CToolHit*		toolhit,
					CNestedArea*	area_to_nest,
					bool			ignore_clamp,
					C3dCoord*		place,		// in WORLD...
					bool			hole,
					int				gravity );

	CPartPlace bitmap_score( 
					CNestConfig&	config,
					CToolHit*		toolhit,
					C3dCoord*		place,		// in WORLD...
					bool			hole );

	double ShiftGet(
				const CNestConfig&	config,
				bool*				vshift,
				bool*				hshift );

	double do_process(	int				prim_axis,
						eSheetProc		process,
						const C3dCoord& shift,
						CGeoPolyArray*	sheet_array,
						CGeoPolyArray*	part_array,
						bool			prof_reverse,
						double			sign,
						const C2dBox&	bump_zone ) const;

	double	BumpScore(
				CNestConfig&	config,
				const C3dCoord&	place,
				const CToolHit&	toolhit ) const;

	double	OverlapScore(
				CNestConfig&	config,
				const C3dCoord&	place,
				const CToolHit&	toolhit,
				int				sign_x,
				int				sign_y ) const;

	int		do_overlap(
						const C3dCoord&	shift, 
						CGeoPolyArray*	sheet_array,
						CGeoPolyArray*	part_array,
						const C2dBox&	bump_zone );

	bool	do_collect_geo_xy( 
						int						prim_axis,
						CGeoCurveArray*			dst_list, 
						const CGeoPolyArray&	src_array, 
						const C3dCoord&			offset, 
						bool					reverse,
						const C2dBox			bump_zone ) const;
	bool	do_collect_geo_box( 
						CGeoCurveArray*			dst_list, 
						const CGeoPolyArray&	src_array, 
						const C3dCoord&			offset, 
						const C2dBox			bump_zone ) const;



	bool	ent_intersect( const CGeoCurve& sheet_geo, 
						const CGeoCurve& part_geo, 
						const C3dCoord& part_shift ) const;

	double	ent_dist_xy( int prim_axis,
						const CGeoCurve& sheet_geo, 
						const CGeoCurve& part_geo, 
						double min_y,
						double max_y,
						double sign,
						bool* parallel ) const;


	double	ent_area_xy( int prim_axis,
						const CGeoCurve& sheet_geo, 
						const CGeoCurve& part_geo, 
						double min_x,
						double max_x ) const;
	double	ent_area_x( const CGeoCurve& sheet_geo, 
						const CGeoCurve& part_geo, 
						double min_y,
						double max_y ) const;
	double	ent_area_y( const CGeoCurve& sheet_geo, 
						const CGeoCurve& part_geo, 
						double min_x,
						double max_x ) const;

	void geo_offset( CGeoPoly* poly, int dir, double dist );

	double	distance_xy( int prim_axis,
						const CGeoCurve& curve1, 
						const CGeoCurve& curve2, 
						double min_y,
						double max_y,
						double sign ) const;
	double	dist_ln_ln_xy( int prim_axis,
						const CGeoLine& line1, 
						const CGeoLine& line2, 
						double min_y,
						double max_y,
						double sign ) const;
	double	dist_arc_ln_xy( int prim_axis,
						const CGeoArc& arc1, 
						const CGeoLine& line2, 
						double min_y,
						double max_y,
						double sign ) const;
	double	dist_arc_arc_xy( int prim_axis,
						const CGeoArc& arc1, 
						const CGeoArc& arc2, 
						double min_y,
						double max_y,
						double sign ) const;

	void	print( 
					CDbWorkplane*	work,
					CDbTool*		tool,
					const CString&	text,
					C3dCoord*		at );
	
	C3dBox remnant_shift( CModel* remnant_model );

	void SeedsDerive(
		const C2dBox&	box,
		eSeedBank		primary_bank,
		eSeedBank		secondary_bank );

	void ExactSeedsDerive(
		const C2dBox&	box,
		eSeedBank		primary_bank,
		eSeedBank		secondary_bank );

	void BitmapSeedsDerive(
		const C2dBox&	box,
		eSeedBank		primary_bank,
		eSeedBank		secondary_bank );

	void SeedsDump(
		const CString&	which_seeds,
		CSeedList*		seeds );

	void SeedsDisplay();

	CReturn OverlapScore( const CNestConfig& config, CPartPlace* pp ) const;

	CGeoPoly*  PolyGet(
		const CGeoPolyArray&	src_array,
		int						depth,
		int						poly_indx );

	// Creates m_toolhit and its grid representation.
	CReturn MaterialProcess( 
		const CNestConfig&	config,
		const C2dBox&		extents,
		bool				invert_grid );

	void ToolHitDelete();

	void RemnantWorkplaneSet();

	void StockBoundariesCreate(
		const CNestConfig&	config,
		CProfile*			material_boundary,
		CProfile*			material_inside,
		CProfile*			material_outside );

	void BorderShift(
		const CNestConfig&	config,
		const C2dBox&		fitzone,
		const C2dBox&		part_shift_mer,
		double*				hshift,
		double*				vshift );

	ePartFit TrivialFitStatus(
		const C2dBox&	fitzone,
		const C2dBox&	part_shift_mer );

	void PartPlaceSet( const CPartPlace& part_place );

	void Render(
		const CGeoPolyArray&	outside_poly,
		const C3dCoord&			place,
		int						redraw_sheet );

	void FitZoneGet(
		const CNestConfig&	config,
		bool				is_large_part,
		C2dBox*				fitzone );

	void TestPierceOverlap(
		const C3dCoord&		shift,
		const CToolHit&		toolhit,
		const CNestedArea&	area_to_nest,
		double*				left_shift,
		double*				vert_shift );

	void check_pierce_overlap(
		const C3dCoord&		shift,
		const CToolHit&		nested_toolhit,
		const CToolHit&		candidate_toolhit,
		double*				left_shift,
		double*				vert_shift );

	void check_pierce_overlap_core(
		double				xshift,
		double				yshift,
		const CToolHit&		toolhitA,
		const CToolHit&		toolhitB,
		double*				left_shift,
		double*				vert_shift );

	double evaluate(
		eSheetProc			process,
		int					prim_axis,
		const CGeoCurve&	sheet_curve,
		const CGeoCurve&	part_curve,
		double				curr_ord,
		double				next_ord,
		double				sign ) const;

	void ShiftValidate( double* shift );

	ePartFit PierceHoleAvoid(
		const CNestConfig&	config,
		const CNestedArea&	area_to_nest,
		const CToolHit&		toolhit,
		C3dCoord*			place );
	
	void ToolsCopy( CModel& from, CModel* to );

	// In support of CutbackWorkzoneCreate()
	CDbFeature* MaxWorkzoneGet();

	void LeadHullsDelete();

	void CollectionsDraw(
		const CGeoCurveArray& curvesA, const C3dCoord& shiftA,
		const CGeoCurveArray& curvesB, const C3dCoord& shiftB ) const;

private:

	int			m_matkey;		// Material key
	int			m_remnkey;		// Remnant key

	CString		m_remname;		// Remnant name, when using a remnant
	CString		m_pathname;		// Only valid after a Save()

	C3dBox		m_extent;
	C2dBox		m_workzone;		// Current (repo) working zone in GRID
	C2dBox		m_fitzone;		// Current (repo) zone to fit in WORLD
	C2dBox		m_recentpart;	// Box of recently placed HIT_PART_OUTSIDE geometry

	double		m_cutback;		// Reduce the length of the sheet by this much

	double		m_weight;
	int			m_quantity;		// Quantity available

	int			m_repeat;		// Current repeat count

	CModel*		m_model;		// Model of the sheet
	bool		m_weownthemodel;

	CToolHit*	m_toolhit;		// Bump geometry and dex grid... all the comforts of home

	CSeed		m_this_seed;

	double		m_textheight;

	CArray<int, int>	m_stock_array;
	CArray<int, int>	m_remnant_array;
	int					m_list_index;

	// tagalong data
	int			m_typeid;
	CString		m_descrip;
	double		m_costper;

	TPartPlaceArray		m_part_places;  // where the part is punched in sheet.

	CSeedMgr*	m_seed_mgr;

	CNestedArea*		m_nested_area;
	TNestedAreaArray	m_nested_areas;
	CGeoPoly	m_material_poly;

	const CNestConfig*	m_config;

	CViewMgr*	m_view_mgr;
	bool		m_ignore_lead;
	bool		m_pierce_bump;

	// 2009.12.19 (PE) -- Introduced to address ITI concerns for
	// orientation of final part on sheet.
	bool		m_retain_seeds;

	// 2012.08.21 (PE) -- Introduced to help improve
	// nesting results when a lead setup is active.
	C2dBoxArray m_lead_boxes;

	bool m_fine_tuning;
	CPartPlace m_pretuning_best;

	eNestingPhase m_nesting_phase;
};

#endif

