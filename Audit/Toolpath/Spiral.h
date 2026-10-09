
#ifndef _SPIRAL_H
#define _SPIRAL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _WMCHAIN_H
#include "WmChain.h"
#endif

#include "SpiralGraphNode.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CSpiral
{
public:

	CSpiral();

	CReturn Init(	const CWmChain&	refChain,
					double			toolDiam,
					double			stepOver,
					double			wallAllow,
					bool			cwResults,
					bool			insideOut,
					double			sharpAngle );

	CReturn Init(	const CWmChain&	refChain,
					double			initialOffset,
					double			stepOver,
					bool			cwResults,
					bool			insideOut,
					double			sharpAngle );

	CReturn Init(
				CWmChain*		outer,
				CWmChainList*	islands,
				double			outer_offset,
				double			island_offset,
				double			stepOver,
				bool			cwResults,
				bool			insideOut,
				double			sharpAngle );

	CWmChainList& Results();

	virtual ~CSpiral();

protected:

private:  // Disabled

	CSpiral( const CSpiral& );
	const CSpiral& operator = ( const CSpiral& );
	int operator == ( const CSpiral& ) const;
	int operator != ( const CSpiral& ) const;

private:  // Methods

	CReturn RecursiveCollapse( const CWmChain& refChain );

	CReturn Connect( const CWmChain& from, const CWmChain& to );

	void Orient();

	CReturn RecursiveCollapse(
				CWmChain*			outer,
				CWmChainList*		islands,
				CSpiralGraphNode*	parent );

	bool IsEnclosedByIsland(
				CWmChain*		outer,
				CWmChainList*	islands );

	void Link(
			CWmElem*		start_wmelem,
			CGeoElemArray*	geo_elems );


private:  // Data

	double m_stepOver;
	bool m_cwResults;
	bool m_insideOut;
	double m_sharpAngle;

	CWmChainList m_results;
};

#endif

