#if !defined(_SHORTESTPATH_H)
#define _SHORTESTPATH_H

// ==================================================================
//		ShortestPath
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Type.h"
#include "Return.h"
#include "DynamicArray.h"
#include "3dCoord.h"
#include "DbEntity.h"
#include "PathState.h"

#include "SeqRules.h"
#include "City.h"

class CIndexPair
{
public:

	CIndexPair( int indxA, int indxB )
	{
		m_indxA = indxA;
		m_indxB = indxB;
	}

	~CIndexPair()  { }

	int Start()	{ return m_indxA; }
	int End()	{ return m_indxB; }

private:

	int	m_indxA;
	int	m_indxB;
};

typedef CDynamicArray<CIndexPair*> TIndexPairArray;


// ==================================================================

#if REQUIRED
// (Cheapest Path Results)
// Forward class declaration for private class.
class Ccpr;
#endif


class dllExport CShortestPath
{
public:

	CShortestPath();

	CReturn	OptimizePath(
						CSeqRules*				seqRules,
						const CDbEntityArray&	tool_list,
						const CDbEntityArray&	src_list,
						CDbEntityArray*			dst_list );

	CReturn OptimizeTSP( 
						CSeqRules*			seqRules,
						const CDbTool*		tool_list,
						CDbEntityArray&		src_list,
						int					chunk_st,
						int					chunk_en,
						CDbEntityArray*		dst_list );

	CReturn CutAvoidance(CDbEntityArray* cut_list);

	virtual ~CShortestPath();

private:

	// Comparison method used by SortedHitsGet().
	static int OrientCompare( const void* ptrA, const void* ptrB );

private:

	bool tsp_waypoints( 
					const CDbEntityArray&	list,
					int						in_st, 
					int						in_en, 
					C3dCoordArray&			waypoint );

	bool tsp_optimize(
					const CSeqRules&		rules, 
					C3dCoordArray&			waypoint );

	bool find_cheapest_profile( 
					const CDbEntityArray&	list,
					const CSeqRules&		rules, 
					int						in_st, 
					int						in_en,
					int*					out_st,
					int*					out_en);

	double score_path(
					const CSeqRules&		rules, 
					const CDbEntity&		db_ent );

	int scan_profile( 
					const CDbEntityArray&	list, 
					int						start, 
					double					max_gap );

	void	anchor_pnt( 
					C3dCoord*				io_anchor, 
					const CDbEntity*		db_ent, 
					bool					in_end );

	void	anchor_pnt( 
					CSeqRules*				seqRules,
					const CDbEntity*		db_ent, 
					bool					in_end );
	
	bool	IsGoodTrend(
					const CSeqRules&		seqRules,
					const CDbEntity*		dbEntity );

	void	entity_pnt( 
					const CDbEntity*		dbEntity, 
					bool					end,
					C3dCoord*				pt );

	void	Transfer(
					CDbEntity*		db_ent,
					CDbEntityArray&	tmp_list,
					int				indxA,
					int				indxB,
					CDbEntityArray*	dst_list,
					CSeqRules*		seqRules );

	void validate_path(CAStateArray* path);

	typedef enum
	{
		RIGHT_EDGE = 0,
		TOP_EDGE = 1,
		LEFT_EDGE = 2,
		BOTTOM_EDGE = 3
	} eEdgeCode;

	int dogleg(CDbEntity* out_ent, C3dCoord* out_pt, C3dCoord& in_pt, C2dBox& mer );
	int dogleg_code(const C2dBox& box, const C3dCoord& in_pt, const C3dCoord& out_pt );

	int rapid_to(CDbEntity* dog_ent, const C2dBox& box, C3dCoord* at, int num, eEdgeCode edge );
	int rapid_to(CDbEntity* dog_ent, C3dCoord* at, C3dCoord& dest, int num);

	void SortedHitsGet(
			const CDbTool*			dbTool,
			const CDbEntityArray&	src_list,
			CDbEntityArray*			sorted_hits );

	void GetSpans(
		const CDbEntityArray&	passes,
		TIndexPairArray*		spans );

	C3dCoord TSP_Solve(
		const C3dCoord&			seed,
		const CDbEntityArray&	passes,
		TIndexPairArray*		spans );

	tCity* GetCities(
		const C3dCoord&			seed,
		const CDbEntityArray&	passes,
		const TIndexPairArray&	spans );

	void Reorder(
		tCity*				cities,
		int					lower_bound,
		int					upper_bound,
		TIndexPairArray*	spans );

private:
	// Disabled.
	CShortestPath( const CShortestPath& );
	const CShortestPath& operator = ( const CShortestPath& );
	int operator == ( const CShortestPath& ) const;
	int operator != ( const CShortestPath& ) const;

private:

	bool	m_group_hits;

#if REQUIRED
	CDynamicArray<Ccpr*>	m_cpr;
#endif
};

#endif

