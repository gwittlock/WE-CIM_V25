#if !defined(_OPTIMIZER_H)
#define _OPTIMIZER_H

// ==================================================================
//		Optimizer
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "cmn_resource.h"
#include "MathConst.h"

#include "Command.h"
#include "model.h"
#include "3dcoord.h"
#include "GeoPoint.h"

#include "DbContainer.h"
#include "DbCurve.h"
#include "DbHole.h"
#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbFeature.h"

#include "WorkPkg.h"
#include "CodeGen.h"
#include "SeqRules.h"


// ==================================================================

class dllExport COptimizer
{
public:

	COptimizer();

	CReturn	PrepModel( 
						CModel*			model,
						CWorkPkgArray*	sheetWorkPkgs,
						CWorkPkgArray*	subdefWorkPkgs );

	C2dBox  MachineExtents( const CModel& model, double y_origin );

	CReturn PrepToolOrder(
						ESeqToolOrder			toolOrder,
						bool					drillsOptimize,
						const CModel&			model,
						const CDbEntityArray&	dbEntityList,
						CDbEntityArray*			dbToolOrder );

	CReturn OptimizeWorkPkg(
						CSeqRules*		seqRules,
						CWorkPkg*		workPkg,
						CDbEntityArray*	cutOrder );

	CReturn	OptimizeBore(
						const CDbEntityArray&	src_list,
						const CDbEntityArray&	tool_list,
						CDbEntityArray*			dst_list,
						const C2dBox&			mach_ext );

	CReturn SequenceByToolOrder(
						CSeqRules*				seqRules,
						const CDbEntityArray&	dbEntityList,
						const CDbEntityArray&	dbToolOrder,
						CDbEntityArray*			cutOrder );

	CReturn SequenceByFlowChart(
						CSeqRules*				seqRules,
						const CDbEntityArray&	dbEntityList,
						const CDbEntityArray&	dbToolOrder,
						CDbEntityArray*			cutOrder );

	CReturn SequenceByProgressive(
						CSeqRules*				seqRules,
						const CDbEntityArray&	dbEntityList,
						const CDbEntityArray&	dbToolOrder,
						CDbEntityArray*			cutOrder );

	CReturn SequenceByAvoidance(
						CSeqRules*				seqRules,
						const CDbEntityArray&	dbEntityList,
						const CDbEntityArray&	dbToolOrder,
						CDbEntityArray*			cutOrder );

	CReturn	prep_container( 
						CDbContainer*		db_contain, 
						CDbEntityArray*		dst_list );

	virtual ~COptimizer();

public:

//	static int scan_profile( 
//					const CDbEntityArray&	list, 
//					int						start, 
//					double					max_gap );

protected:

private:

	void	CutOrderAppend(
					CDbFeature*			topLevelFeature,
					CDbEntityArray*		dbEntities,
					CDbEntityArray*		cutOrder );

	void	SubdefsPrep(
					const CModel&		model,
					CWorkPkgArray*		subdefWorkPkgs );

	void	MainPrep(
					const CModel&		model,
					const C3dBox&		box,
					CWorkPkgArray*		workPkgArray );

	void	PrepSheet(
					const CDbPattern&	sheet,
					const C3dBox&		box,
					CWorkPkgArray*		sheetWorkPkgs );

#if ORIGINAL_CODE
	CReturn	prep_container( 
						CDbContainer*		db_contain, 
						CDbEntityArray*		dst_list );
#endif

	int		get_hole_block( 
					const CDbEntityArray&	src_list,
					int						in_start );

	bool	optimize_one_drop( 
					const CDbEntityArray&	src_list, 
					const CDbEntityArray&	tool_list,
					const C2dBox&			mach_ext,
					CDbEntityArray*			dst_list, 
					int						anchor_idx, 
					int						first_hole, 
					int						last_hole, 
					C3dCoord*				drop_pnt );

	bool	tool_match_elem( 
					const CDbHole&	anchor,
					const CDbTool&	tool );

	int		score_one_tool( 
					const CDbEntityArray&	src_list, 
					const CDbEntityArray&	tool_list, 
					const C2dBox&			mach_ext,
					const CDbHole&			anchor,
					const CDbTool&			tool,
					int						first_hole,
					int						last_hole,
					CDWordArray*			this_drop );

	bool	tool_reaches(
					 const CDbTool&		tool,
					 const C3dCoord&	pnt,
					 const C2dBox&		mach_ext );

	double	drop_distance( 
					const CDbEntityArray&	src_list,
					const CDbEntityArray&	tool_list,
					const C3dCoord&			drop_pnt,
					CDWordArray*			drop_array );

	static int	ZoneCompare( const void* ptrA, const void* ptrB );

	void	AttributesClear( const CDbEntityArray& masterPool );

	void	EntitiesCopy(
					const CDbEntityArray&	entities,
					CDbEntityArray*			copies );

	void	HolesTransfer(
					CDbEntityArray*	entityPool,
					CDbEntityArray*	holes );
	void	CommandsTransfer(
					CDbEntityArray*	entityPool,
					CDbEntityArray*	holes );
	void	ProgressiveTransfer(
					CDbEntityArray*	entityPool,
					CDbEntityArray*	holes );
	void	DepthTransfer(
					CDbEntityArray*	entityPool,
					CDbEntityArray*	deepHoles,
					CDbEntityArray*	shallowHoles );

	void	EntitiesTransfer(
						const CDbEntityArray&	order,
						CDbEntityArray*			masterPool,
						CDbEntityArray*			interPool );
	
	void	TooledEntitiesTransfer(
								const CDbTool*	dbTool,
								CDbEntityArray*	source,
								CDbEntityArray*	result );
	bool	Keep(
				CDbEntity*		dbEntity,
				CDbEntityArray*	cutOrder );
	

	int SpanGet( const CDbEntityArray& ents, int indx );
	int ProgressiveRange( const tGeoPointArray& pts, int indx );

	static int ProgressiveXsort( const void* ptrA, const void* ptrB );
	static int ProgressiveYsort( const void* ptrA, const void* ptrB );

private:
	// Disabled.
	COptimizer( const COptimizer& );
	const COptimizer& operator = ( const COptimizer& );
	int operator == ( const COptimizer& ) const;
	int operator != ( const COptimizer& ) const;

private:

	double	m_spacetol;
	bool	m_nibble;

	// Relevant only when IsPhenolicProject() returns true.
	CDbFeature*	m_phenolicPart;
};

#endif

