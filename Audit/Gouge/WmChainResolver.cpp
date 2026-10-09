
#include "stdafx.h"
#include "MathConst.h"

#include "WmSubchn.h"
#include "WmChain.h"
#include "WmChainIterator.h"
#include "WmChainResolver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChainResolver::CWmChainResolver()
	: m_list(),
	  m_tol( SMALL )
{

}

CWmChainResolver::~CWmChainResolver()
{
	m_list.DestructiveFlush();
}

void
CWmChainResolver::Resolve( const CWmChainList& chainList, double tol )
{
	CWmChainIterator	iter;

	CWmChainList	set;
	CWmChain*		chain;
	int				maxSetNo;
	int				indx, count;

	m_tol = tol;

	// Determine the number of sets of intersecting chains.

	maxSetNo = 0;

	count = chainList.Count();
	for (indx = 0; indx < count; ++indx)
	{
		chain = chainList[indx];

		if (chain->SetNo() > maxSetNo)
			maxSetNo = chain->SetNo();
	}

	// While looping over each set of intersecting chains,
	// coalesce the subchains having the same minimal
	// interference index into integral chains.

	for (indx = 1; indx <= maxSetNo; ++indx)
	{
		// Build the Ith set of intersecting chains.
		IntersectionSetBuild( indx, chainList, &set );

		MinimalLoopsBuild( set );

		set.BenignFlush();
	}
}

int
CWmChainResolver::Count() const
{
	return m_list.Count();
}

const CWmChain* CWmChainResolver::GetAt( int index ) const
{
	return m_list[ index ];
}

void
CWmChainResolver::IntersectionSetBuild( int setNo, const CWmChainList& chains, CWmChainList* set )
{
	int count = chains.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CWmChain* chain = chains[indx];

		if (chain->SetNo() == setNo)
			set->Append( chain );
	}
}

void
CWmChainResolver::MinimalLoopsBuild( const CWmChainList& set )
{
	int minIindex = MinIindexGet( set );

	// Build the set of subchains having the minimum interference index.

	CWmSubchnList subchns;
	MinSubchnsGet( set, minIindex, &subchns );

	MinChainsBuild( &subchns );
}

// Find the minimum interference index.
int
CWmChainResolver::MinIindexGet( const CWmChainList& set )
{
	CWmChainIterator iter;
	int minIindex = IUNDEFINED;

	int count = set.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CWmChain* chain = set[indx];

		iter.Init( (*chain) );

		while ( !iter.AtEnd() )
		{
			CWmSubchn* subchn = iter.Subchn();

			int iindex = subchn->Iindex();

			// NOTE: Prevent consideration of terminal subchns.
			if (iindex < minIindex && subchn->IsTerminal() == false)
				minIindex = iindex;

			iter.NextSubchn();
		}
	}

	return minIindex;
}

// Build the set of subchains having the minimum interference index.
void
CWmChainResolver::MinSubchnsGet( const CWmChainList& chains, int minIindex,  CWmSubchnList* subchns )
{
	CWmChainIterator iter;

	int count = chains.Count();

	for (int indx = 0; indx < count; ++ indx)
	{
		CWmChain* chain = chains[indx];

		iter.Init( (*chain) );

		while ( !iter.AtEnd() )
		{
			CWmSubchn* subchn = iter.Subchn();

			// NOTE: Prevent inclusion of terminal subchns.
			if (subchn->Iindex() == minIindex && subchn->IsTerminal() == false)
				subchns->Append( subchn );

			iter.NextSubchn();
		}
	}
}

// NOTE: At this point, MinIindexGet() should have prevented inclusion of terminal subchns.
void
CWmChainResolver::MinChainsBuild( CWmSubchnList* subchns )
{
	while (subchns->Count() > 0)
	{
		CWmSubchn* subchn = subchns->Remove( 0 );

		if (0)
		{
			bool any = subchn->AnyElemAttribs();
			int foo = 0;
		}

		if (subchn->Next() != NULL)
		{
			CWmChain* chain = new CWmChain();

			chain->AttribsPropogate( true );
			chain->CopyAppend( (*subchn) );

			m_list.Append( chain );

			while (1)
			{
				// Get the next contiguous subchain (within m_tol).
				// NOTE: Up to the first official release of V16.5,
				// the next contiguous subchain was located by using
				// subchn->Other(). That approach caused failures
				// because it did not always generate closed chains
				// when it should have. In particular, the initial
				// subchn (at the start of the chain) did not necessarily
				// have an 'other' node.
				subchn = FindNextSubchn( subchns, subchn );
				if (subchn == NULL)
					break;

				int indx = subchns->Find( subchn );
				if (indx < 0)
					break;

				subchns->Remove( indx );
				chain->CopyAppend( (*subchn) );
			}
		}
	}
}

CWmSubchn*
CWmChainResolver::FindNextSubchn( CWmSubchnList* subchns, CWmSubchn* subchn )
{
	CWmSubchn*	temp;
	C2dCoord	ps;
	C2dCoord	pe;
	int			count, indx;

	pe = subchn->EndPt();

	count = subchns->Count();
	for (indx = 0; indx < count; ++indx)
	{
		temp = (*subchns)[indx];
		if (temp != subchn)
		{
			ps = temp->StartPt();

			if ( ps.WithinTol( pe, m_tol ) )
			{
				return temp;
			}
		}
	}

	return NULL;
}

