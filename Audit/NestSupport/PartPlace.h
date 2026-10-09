#ifndef _PARTPLACE_H
#define _PARTPLACE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "2dCoord.h"
#include "ToolHit.h"
#include "NestingPart.h"


class dllExport CPartPlace
{
public:

	CPartPlace();

	void Init(
			const C2dCoord&	world,
			const C2dCoord&	grid,
			CToolHit*		toolhit );  // Should really be const?

	const CPartPlace& operator = ( const CPartPlace& other );

	C2dCoord WorldPointGet() const;

	C2dCoord GridPointGet() const;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	void ScoresInit( double bump_score, double overlap_score );

	double BumpScoreGet() const;
	void BumpScoreSet( double bump_score );

	double OverlapScoreGet() const;
	void OverlapScoreSet( double overlap_score );

	double PackingScoreGet() const;
	void PackingScoreSet( double packing_score );

	// Returns (true) when this part place scores better than other.
	bool HasBetterScore( const CPartPlace& other ) const;

	bool HasSameScore( const CPartPlace& other ) const;

	CToolHit* ToolHitGet() const;

	CNestingPart* PartGet() const;

	~CPartPlace();

private:

	C2dCoord	m_world;
	C2dCoord	m_grid;
	double		m_bump_score;
	double		m_overlap_score;
	double		m_packing_score;
	CToolHit*	m_toolhit;
};

typedef CDynamicArray<CPartPlace*> TPartPlaceArray;

#endif
