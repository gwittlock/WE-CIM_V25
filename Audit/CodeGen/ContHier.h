
#ifndef _CONTHIER_H
#define _CONTHIER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"
#include "GeoPoly.h"
#include "SeqRules.h"
#include "ContHierNode.h"

class CSeqRules;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CContHier
{
public:

	CContHier();
	
	CReturn Init( const CDbEntityArray& entityPool );

	CReturn Sequence( CSeqRules* seqRules );

	const CDbEntityArray& FinalOrder()		{ return m_finalOrder; }

	// CRITICAL: This *must* be called prior to calling Init().
	void PhenolicInit( const CDbEntityArray* toolOrder, ESeqTrend trend );

	void Debug() const;

	virtual ~CContHier();

public:

	static void GapTolSet( double gap_tol );

protected:

	int Count() const	{ return m_nodes.Count(); }

	CContHierNode* operator [] ( int indx ) const	{ return m_nodes[indx]; }

private:  // Methods

	CReturn SequenceByRecursion( CSeqRules* seqRules );
	CReturn SequenceByGlobalContainment( CSeqRules* seqRules );
	CReturn SequenceByLocalContainment( CSeqRules* seqRules );
	CReturn SequenceByProgressive( CSeqRules* seqRules );
	CReturn SequenceByAvoidance( CSeqRules* seqRules );

	void PolysCreate(
				const CDbEntityArray& entityPool,
				CGeoPolyArray* polyArray,
				CDbEntityArray* entityArray );

	void	Sort( bool increasingOrder, CCHNArray* nodes );
	void	SortProgressive( CCHNArray* nodes );

	int		NodesScan( const CCHNArray& nodes, int indxA );

	int		ContainmentDepth( const CCHNArray& nodes, int indx );

	void	EntitiesCollect(
					const CCHNArray&	nodes,
					int					indxA,
					int					indxB,
					CDbEntityArray*		rawHole,
					CDbEntityArray*		rawShallow,
					CDbEntityArray*		rawDeep );

	void	ProgressiveCollect(
					CSeqRules*			seqRules,
					const CCHNArray&	nodes,
					int					indxA,
					int					indxB,
					CDbEntityArray*		punch,
					CDbEntityArray*		torchShallow,
					CDbEntityArray*		torchDeep );

	void	AvoidanceCollect(
					CSeqRules*			seqRules,
					const CCHNArray&	nodes,
					int					indxA,
					int					indxB,
					CDbEntityArray*		deep,
					CDbEntityArray*		smallShallow,
					CDbEntityArray*		largeShallow );


	void	SiblingsAppend(
					CDbEntity*		dbEntity,
					CDbEntityArray*	dbEntities );

	void	NodesReorder(
					int						indxA,
					int						indxB,
					const CDbEntityArray&	dbEntities );

	void	RecursiveOrder(
					CSeqRules*				seqRules,
					const CDbEntityArray&	toolOrder,
					const CDbEntityArray&	contained,
					CDbEntityArray*			finalOrder );

	void	EntitiesCollect(
					const CDbEntityArray&	dbEntities,
					CDbEntityArray*			rawHoles,
					CDbEntityArray*			rawShallow,
					CDbEntityArray*			rawDeep );

	void	LocalOrder(
					CSeqRules*				seqRules,
					const CDbEntityArray&	toolOrder,
					CContHierNode*			hier,
					CDbEntityArray*			finalOrder );

	void	NodesCollect(
					CContHierNode*	parent,
					CCHNArray*		nodes );

	void	HolesCollect(
				CDbEntity*		root,
				CDbEntityArray*	rawHoles );

	void	LocalHolesProcess(
					CSeqRules*				seqRules,
					const CDbEntityArray&	toolOrder,
					CDbEntity*				parent,
					CDbEntityArray*			finalOrder );

	void	SpecialInsert(
					const CDbEntityArray&	cutOrder,
					CDbEntityArray*			finalOrder );

	CGeoPoly*	PolyFromExplicitProfile( const CDbProfile& dbprofile, int* indx );
	CGeoPoly*	PolyFromImplicitProfile( const CDbEntityArray& entityPool, int* indx );

	bool IsDoubleHit( const CDbEntity& entityA, const CDbEntity& entityB );

	bool IsClosed( CGeoPoly* poly );

	CReturn PhenolicHierInit( const CDbEntityArray& entityPool );

	int PhenolicCutsCollect(
		const CDbEntityArray&	entityPool,
		int						indx,
		CDbEntityArray*			cuts );

	void PhenolicCutsOrder( CDbEntityArray* cuts );
	void PhenolicOrderSet( const CDbEntityArray& cuts );

	int PhenolicBestSeedGet( const CDbEntityArray& cuts );
	bool IsBetterSeed( const C3dCoord& best, const C3dCoord& pt );

private:

	static int IncreasingOrderCompare( const void* ptrA, const void* ptrB );
	static int DecreasingOrderCompare( const void* ptrA, const void* ptrB );

private:  // Disabled

	CContHier( const CContHier& );
	const CContHier& operator = ( const CContHier& );
	int operator == ( const CContHier& ) const;
	int operator != ( const CContHier& ) const;

private:

	static double m_gap_tol;

private:  // Data

	CCHNArray m_nodes;

	CDbEntityArray	m_finalOrder;

	const CDbEntityArray*	m_phenolicToolOrder;
	ESeqTrend	m_phenolicTrend;
};

#endif

