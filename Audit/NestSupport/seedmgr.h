
#ifndef _SEEDMGR_H
#define _SEEDMGR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "NestConfig.h"
#include "Seed.h"
#include "SeedList.h"
#include "2dVec.h"


enum eSeedBank
{
	PRIMARY_BANK	= 0,
	SECONDARY_BANK	= 1
};

typedef CDynamicArray<CSeedList*> TSLArray;

class CNestConfig;
class CSheet;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CSeedMgr
{
public:

	CSeedMgr();

	CReturn Push();

	CReturn Pop();

	// 2006.10.03 (PE) -- We want to retain the valuable seeds.
	// Returns (true) if popped / (false) if not.
	bool ConditionalPop();

	CReturn Deposit(
		eSeedBank		which_bank,
		const CSeed&	seed );

	CReturn SeedsSort(
		eNestProgression	progression,
		CSheet*				sheet );

	CSeed NextCandidate();

	bool HasSeeds() const;

	CReturn SeedsShift( eSeedBank which_bank, C2dVec delta );

	void SeedsPrint();

	virtual ~CSeedMgr();

public:

	static int compare_geo_left( const void* u, const void* v );
	static int compare_geo_right( const void* u, const void* v );

private:

	void SeedsDump(
		const CString&	which_seeds,
		CSeedList*		seeds );

	CSeed StandardCandidate();
	CSeed BumpScoreCandidate();

private:

	// Disabled.
	CSeedMgr( const CSeedMgr& );
	const CSeedMgr& operator = ( const CSeedMgr& );
	int operator == ( const CSeedMgr& ) const;
	int operator != ( const CSeedMgr& ) const;

private:

	static bool m_print_seeds;

	static bool m_use_bump_score;
	static eNestProgression	m_progression;

private:

	TSLArray	m_primary;
	TSLArray	m_secondary;
};

#endif

