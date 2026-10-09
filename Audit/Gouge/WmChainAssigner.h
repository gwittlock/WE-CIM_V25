
#ifndef _WMCHAINASSIGNER_H
#define _WMCHAINASSIGNER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _WMCONST_H
#include "WmConst.h"
#endif

#ifndef _WMCHAIN_H
#include "WmChain.h"
#endif


#define ASSIGNER_NORMAL_USAGE		0x00
#define ASSIGNER_PROPOGATE_IINDEX	0x01

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmChainAssigner
{
public:

	CWmChainAssigner();

	void FlagsSet( int flags );

	CReturn InterferenceIndicesAssign( CWmChainList* chainList );	
	
	virtual ~CWmChainAssigner();

private:  // Disabled.

private:  // Methods.

	void IntersectionSetBuild( CWmChain* chain, int setNo, CWmChainList* iSet );

	CReturn InterferenceIndexAssign( CWmChain* chain );
	CReturn InterferenceIndexAssign( CWmSubchn* subchn, int iindex );

	CReturn SimpleIindexAssign( CWmSubchn* subchn, int iindex );
	CReturn ComplexIindexAssign( CWmSubchn* subchn, int iindex );

	void FirstAssignedFind( CWmChainIterator* iter );

	const char* TangentsGet(
				CWmSubchn* subchnA,
				CWmSubchn* subchnB,
				C2dUnitVec* aTan,
				C2dUnitVec* bTan,
				C2dUnitVec* cTan,
				C2dUnitVec* dTan );

	C2dUnitVec TangentGet( CWmElem* node, bool entry );

	EIntersectionType IntersectionClassify(
							C2dUnitVec& aTan,
							C2dUnitVec& bTan,
							C2dUnitVec& cTan,
							C2dUnitVec& dTan );

	bool Used( const CWmChain* chain );

	void Mark( const CWmChain* chain );

private:  // Data.

	CWmChainList m_used;

	int	m_flags;
};

#endif
