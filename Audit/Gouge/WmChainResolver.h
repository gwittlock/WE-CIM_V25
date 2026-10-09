
#ifndef _WMCHAINRESOLVER_H
#define _WMCHAINRESOLVER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _WMCHAIN_H
#include "WmChain.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmChainResolver
{
public:

	CWmChainResolver();

	void Resolve( const CWmChainList& chainList, double tol );

	int Count() const;

	const CWmChain* GetAt( int index ) const;

	virtual ~CWmChainResolver();

private:  // Disabled.

private:  // Methods.

	void IntersectionSetBuild( int setNo, const CWmChainList& chains, CWmChainList* set );

	void MinimalLoopsBuild( const CWmChainList& set );

	int MinIindexGet( const CWmChainList& set );

	void MinSubchnsGet( const CWmChainList& chains, int minIindex,  CWmSubchnList* subchns );

	void MinChainsBuild( CWmSubchnList* subchns );

	CWmSubchn* FindNextSubchn( CWmSubchnList* subchns, CWmSubchn* next );

private:  // Data.

	CWmChainList m_list;
	double m_tol;
};

#endif
