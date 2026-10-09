
#ifndef _WMCHAININTSCTOR_H
#define _WMCHAININTSCTOR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _2DCOORD_H
#include "2dCoord.h"
#endif

#ifndef _WMINTREC_H
#include "WmIntRec.h"
#endif

#ifndef _WmCRVREC_H
#include "WmCrvRec.h"
#endif

#ifndef _WMCHAIN_H
#include "WmChain.h"
#endif

class CWmElem;
class CWmSubchn;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmChainIntsctor
{
	friend class CWmChainSplitter;

public:

	CWmChainIntsctor();

	bool Intersect( const CWmChain& chainA, const CWmChain& chainB, double tol );

	void Debug( const char* caption ) const;

	virtual ~CWmChainIntsctor();

protected:

	CWmIntRecList& IntRecs()		{ return m_ints; }

	CWmCrvRecList& CrvRecs()		{ return m_crvs; }

	void Finalize();

	void Reduce();

private:

	bool Intersect( CWmElem* nodeA, CWmElem* nodeB, double tol );

	CWmIntRec* FindCreate( const C2dCoord& pt, double tol );

	CWmCrvRec* FindCreate( CWmElem* node );

	bool OkayToIntersect( const CWmElem* nodeA, const CWmElem* nodeB );

private:

	// Disabled.
	CWmChainIntsctor( const CWmChainIntsctor& );
	const CWmChainIntsctor& operator = ( const CWmChainIntsctor& );
	int operator == ( const CWmChainIntsctor& ) const;
	int operator != ( const CWmChainIntsctor& ) const;

private:

	CWmIntRecList	m_ints;
	CWmCrvRecList	m_crvs;
};

#endif


