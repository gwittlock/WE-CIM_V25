
#ifndef _WMCHAINSPLITTER_H
#define _WMCHAINSPLITTER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _WMCHAININTSCTOR_H
#include "WmChainIntsctor.h"
#endif



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmChainSplitter
{
public:

	CWmChainSplitter();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Splits the chains whose curves are recorded in the given
	// intsctor as having intersections with other curves.
	//
	// As a precursor to the splitting activity, the given
	// intsctor is 'finalized', ensuring the recorded curves
	// have data for the bounds U in [0,1]
	//
	void Split( CWmChainIntsctor* intsctor );

	virtual ~CWmChainSplitter();

private:

	// Disabled.
	CWmChainSplitter( const CWmChainSplitter& );
	const CWmChainSplitter& operator = ( const CWmChainSplitter& );
	int operator == ( const CWmChainSplitter& ) const;
	int operator != ( const CWmChainSplitter& ) const;

private:
};

#endif


