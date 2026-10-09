
#ifndef _WMCHAINITERATOR_H
#define _WMCHAINITERATOR_H

#ifndef _WMNODE_H
#include "WmNode.h"
#endif

#ifndef _WMELEM_H
#include "WmElem.h"
#endif

#ifndef _WMSUBCHN_H
#include "WmSubchn.h"
#endif

#ifndef _WMCHAIN_H
#include "WmChain.h"
#endif



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmChainIterator
{
public:

	CWmChainIterator();

	CWmChainIterator( const CWmChain& chain );

	/////////////////////////////////////////////////////////////////
	// Force this iterator to iterate on the given chain.
	// The iterator will be set to the start of the chain.
	void Init( const CWmChain& chain );

	const CWmChainIterator& operator = ( const CWmChainIterator& chain );

	// Access operator.
	CWmSubchn* Subchn();
	CWmElem* Elem();

	bool AtStart();

	bool AtEnd();

	void GotoStart();

	void GotoEnd();

	/////////////////////////////////////////////////////////////////
	// Advance to the next subchain.
	void NextSubchn();

	/////////////////////////////////////////////////////////////////
	// Advance to the next element.
	void NextElem();

	/////////////////////////////////////////////////////////////////
	// Advance to the previous subchain.
	void PrevSubchn();

	/////////////////////////////////////////////////////////////////
	// Advance to the previous element.
	void PrevElem();

	virtual ~CWmChainIterator();

private:

	CWmChain*	m_chain;
	CWmNode*	m_node;
};

#endif
