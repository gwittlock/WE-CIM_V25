
#include "stdafx.h"

#include "MathConst.h"
#include "Register.h"
#include "WmChain.h"
#include "WmChainIntsctor.h"
#include "WmChainSplitter.h"
#include "WmChainAssigner.h"
#include "WmChainResolver.h"
#include "WmChainDegouger.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChainDegouger::CWmChainDegouger()
	: m_results()
{
	m_flags = DEGOUGE_NORMAL_USAGE;
}

CWmChainDegouger::~CWmChainDegouger()
{
	m_results.DestructiveFlush();
}

void CWmChainDegouger::Init( CWmChain* chain, double tol )
{
	CWmChainList chainList;
	CWmChainIntsctor intsctor;
	CWmChainSplitter splitter;
	CWmChainAssigner assigner;
	CWmChainResolver resolver;

	int debug = CRegister::Debug("WmChain");

	m_results.DestructiveFlush();

	if (debug) chain->Debug( "Raw" );

	if (0)
	{
		bool any = chain->AnyElemAttribs();
		int foo = 0;
	}

	intsctor.Intersect( (*chain), (*chain), tol );
	if (debug)
	{
		intsctor.Debug( "Before split" );
	}

	splitter.Split( &intsctor );
	if (debug)
	{
		chain->Debug( "Split" );
		intsctor.Debug( "After split" );
	}

	chainList.Append( chain );

	if (m_flags & DEGOUGE_PROPOGATE_IINDEX)
		assigner.FlagsSet( ASSIGNER_PROPOGATE_IINDEX );

	assigner.InterferenceIndicesAssign( &chainList );

	if (debug)
	{
		chain->Debug( "Assigned" );
	}

	// Build all closed loops through the intersecting chains.
	resolver.Resolve( chainList, (2*SMALL) );

	int count = resolver.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CWmChain* resolved = resolver.GetAt( indx );

		if (0)
		{
			bool any = resolved->AnyElemAttribs();
			int foo = 0;
		}

		CWmChain* copy = new CWmChain();
		copy->AttribsPropogate( true );
		copy->CopyAppend( *resolved );

		m_results.Append( copy );
	}
}

void CWmChainDegouger::Init(
	CWmChain*		outer,
	CWmChainList*	islands,
	double			tol )
{
	CWmChainList chainList;
	CWmChainIntsctor intsctor;
	CWmChainSplitter splitter;
	CWmChainAssigner assigner;
	CWmChainResolver resolver;
	CWmChain*	island;
	bool	does_intersect;
	int	icnt, indx;
	int	jcnt, jndx;

	int debug = CRegister::Debug("WmChain");

	m_results.DestructiveFlush();

	// See also CWmChain::PurgeSubchns()
	outer->PurgeSubchns();
	chainList.Append( outer );

	icnt = islands->Count();
	for (indx = 0; indx < icnt; ++indx)
	{
		island = islands->GetAt( indx );

		// See also CWmChain::PurgeSubchns()
		island->PurgeSubchns();

		does_intersect = false;

		jcnt = chainList.Count();
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			if (intsctor.Intersect( *(chainList.GetAt( jndx )), (*island), tol ) == true)
				does_intersect = true;
		}

		if ((does_intersect == true) || (m_flags & DEGOUGE_INCLUDE_ALL_ISLANDS))
			chainList.Append( island );
	}

	splitter.Split( &intsctor );
	if (debug)
	{
		intsctor.Debug( "After split" );
	}

	if (m_flags & DEGOUGE_PROPOGATE_IINDEX)
		assigner.FlagsSet( ASSIGNER_PROPOGATE_IINDEX );

	assigner.InterferenceIndicesAssign( &chainList );

	// Build all closed loops through the intersecting chains.
	resolver.Resolve( chainList, (2*SMALL) );

	icnt = resolver.Count();
	for (indx = 0; indx < icnt; ++indx)
	{
		const CWmChain* resolved = resolver.GetAt( indx );

		CWmChain* copy = new CWmChain();
		copy->AttribsPropogate( true );
		copy->CopyAppend( *resolved );

		m_results.Append( copy );
	}
}

void CWmChainDegouger::FlagsSet( int flags )
{
	m_flags = flags;
}

int CWmChainDegouger::Count() const
{
	return m_results.Count();
}

CWmChainList& CWmChainDegouger::Results()
{
	return m_results;
}
