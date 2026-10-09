#if !defined(_NESTMGR_H)
#define _NESTMGR_H

// ==================================================================
//		NestMgr
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Common.h"
#include "Command.h"
#include "NestConfig.h"
#include "ToolHit.h"
#include "Sheet.h"
#include "PartBin.h"
#include "Repo.h"
#include "RepoZone.h"
#include "PartPlace.h"
#include "SeedMgr.h"
#include "NestedArea.h"


// ==================================================================

class CNestMgr
{
public:

	CNestMgr();

	CReturn Execute(
		CCommand*		io_cmd, 
		CNestConfig&	config, 
		CSheet*			sheet, 
		CPartBin*		partbin,
		CViewMgr&		view );

	CReturn GridNest(
		CNestConfig&	config,
		CPartBin*		partbin,
		CNestingPart*	pre_part,
		CSheet*			sheet,
		CModel&			model,
		CViewMgr&		view );

	CReturn PreNest(
		CNestConfig&	config,
		CPartBin*		partbin,
		CNestingPart*	pre_part,
		CSheet*			sheet,
		CViewMgr&		view );

	void remnant_flip( CModel* remnant_model );

	virtual ~CNestMgr();

public:

	static CReturn	Chain(
					CModel& model,
					int chain_num,
					double offset );

private:
	// Disabled.
	CNestMgr( const CNestMgr& );
	const CNestMgr& operator = ( const CNestMgr& );
	int operator == ( const CNestMgr& ) const;
	int operator != ( const CNestMgr& ) const;

private:

	CReturn nest_sheet(
					CNestConfig&	config,
					CRepoZone&		repo_zone,
					CPartBin*		partbin,
					CSheet*			sheet,
					CViewMgr&		view );

	CReturn nest_repo(
					CNestConfig&	config,
					int				repo_cnt,
					CRepoZone&		repo_zone,
					CPartBin*		partbin,
					CSheet*			sheet,
					CViewMgr&		view );

	CReturn nest_part_in_part(
					CNestConfig&	config,
					const C2dBox&	edge,			
					CNestedArea*	area_to_nest,
					int				zone_num,
					bool			torch_shift,
					CPartBin*		partbin,
					CSheet*			sheet,
					CViewMgr&		view );

	CReturn	nest_area(
					CNestConfig&	config,
					const C2dBox&	edge,
					CGeoPolyArray*	poly_array,
					CNestedArea*	area_to_nest,
					bool			hole,
					int				zone_num,
					bool			torch_shift,
					CPartBin*		partbin,
					CSheet*			sheet,
					CViewMgr&		view );


	CReturn nest_sheet_repo(
							CNestConfig&	config,
							CRepo&			repo,
							CRepoZone&		repo_zone,
							CSheet*			sheet,
							CViewMgr&		view );

	CReturn nest_sheet_attrib(
							CNestConfig&	config,
							CSheet*			sheet,
							CViewMgr&		view );

	CReturn nest_sheet_lead(
							CNestConfig&	config,
							CSheet*			sheet,
							CViewMgr&		view );

#if BEFORE_V19
	void ZoneAdjust(
					CNestConfig&	config,
					const C2dBox&	edge,
					CSheet*			sheet );
#else
	void ZoneAdjust(
					CNestConfig&	config,
					const C2dBox&	edge,
					CSheet*			sheet,
					bool			nesting_in_part );
#endif

#if BEFORE_V19
	CPartPlace	score_parts( 
					CNestConfig&	in_config, 
					CPartBin*		partbin, 
					CNestingPart*	force_part,
					CSheet*			sheet, 
					bool			hole,
					CViewMgr&		view );
#else
	CPartPlace	score_parts( 
					CNestConfig&	in_config, 
					CPartBin*		partbin, 
					CNestedArea*	area_to_nest,
					CNestingPart*	force_part,
					CSheet*			sheet, 
					bool			hole,
					CViewMgr&		view );
#endif

	bool tool_part( 
					CNestConfig&	in_config, 
					CNestingPart*	part,
					CSheet*			sheet );

#if BEFORE_V19
	CReturn	cut_part( 
					CNestConfig&	in_config, 
					CSheet*				sheet, 
					int					zone,
					bool				torch_shift,
					int					hole,
					const CPartPlace&	part_place,
					CNestedArea*		area_to_nest,
					CViewMgr&			view );
#else
	CNestedArea* cut_part( 
					CNestConfig&	in_config, 
					CSheet*				sheet, 
					int					zone,
					bool				torch_shift,
					int					hole,
					const CPartPlace&	part_place,
					CNestedArea*		area_to_nest,
					CViewMgr&			view );
#endif

	int		PartsAccounting( 
					CNestConfig*	config,
					CPartBin*		partbin,
					CSheet*			sheet );

	CReturn	clear_results( const CNestConfig& config );
	void	save_pass_results( 
					const CNestConfig&	config, 
					const CSheet&		sheet,
					const CPartBin&		partbin );

	void	save_final_results( 
					const CNestConfig&	config, 
					const CSheet&		sheet,
					const CPartBin&		partbin );

	CDbLine*	cutback_sheet(
					CNestConfig&	config, 
					CSheet*			sheet,
					CViewMgr&		view );

	void	remnant_sheet(
					CNestConfig&	config, 
					CSheet*			sheet,
					C2dBox&			content,
					CViewMgr&		view );

	void	set_fill_barrier(
					CNestConfig&	config, 
					CPartBin*		partbin,
					CSheet*			sheet,
					bool			unlimited );
	void	set_grid_barrier(
					CNestConfig&	config, 
					CSheet*			sheet );	

	void	next_gridsquare( 
					CToolHitArray*		next_hit,
					CSeedArray*			next_pos, 
					CNestConfig&		config, 
					CPartBin*			partbin,
					CToolHit*			last_hit, 
					const C3dCoord&		last_pos );

	void	next_link( 
					CToolHitArray*		next_hit,
					CSeedArray*			next_pos, 
					CNestConfig&		config, 
					CPartBin*			partbin,
					const CSheet&		sheet, 
					const C3dCoord&		last_pos );

	void	next_seed( 
					CToolHitArray*		next_hit,
					CSeedArray*			next_pos, 
					CNestConfig&		config, 
					CPartBin*			partbin,
					// C3dCoord&			seed );
					const CSheet&		sheet );

	CReturn SaveSheet(
					CNestConfig&	config, 
					CPartBin&		partbin,
					int				sheet_count,
					CSheet*			sheet );

	void SeedsGenerate( 
					CNestConfig&		config, 
					CSheet&		sheet,
					CPartBin*			partbin,
					CToolHitArray*		next_hit,
					CSeedArray*			next_pos );

	void ConditionalSeedAdd(
					const C2dCoord&		candidate,
					CToolHit*			toolhit,
					CSeedArray*			next_pos,
					CToolHitArray*		next_hit );

	bool IsUniquePlace(
			const CSeedArray&		next_pos,
			const CToolHitArray&	next_hit,
			const C2dCoord&			candidate,
			const CToolHit*			tool_hit );

	void DoGridStuff(
			CNestConfig&	config,
			CGeoPolyArray*	poly_array,		// Polygon(s) that define seeds...
			int				idx,			// ????
			bool			hole,			// TRUE if we are nesting inside a hole
			CSheet*			sheet,
			const CPartPlace&	best );

	bool CanProcess(
		const CNestConfig&	config,
		const CNestingPart&	part );

	void LargePartsPrep(
		const CSheet&		sheet, 
		CPartBin*			partbin,
		CNestConfig*		config );

	CNestedArea* InnerAreaGet(
		const CNestConfig&	config,
		CNestedArea*		area_to_nest );

	void PackingScore(
		const CNestConfig&	config,
		const CNestedArea&	na,
		CPartPlace*			pp,
		CViewMgr&			view  );

	void PolyRender( const CGeoPoly& poly );

private:

	static CReturn	do_chain(
					CDbFeature* db_feat,
					CModel& model,
					int chain_num, 
					double offset );

	static void UselessFeaturesDelete(
			CModel*		model,
			CDbFeature*	work_zone );

private:

	CSeedMgr	m_seed_mgr;

	CToolHit*	m_lasthit;
	C3dCoord	m_lastpos;
	C3dCoord	m_lastcol;
};

#endif

