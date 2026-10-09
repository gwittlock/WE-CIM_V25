
#ifndef _WMCHAINDEGOUGER_H
#define _WMCHAINDEGOUGER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _WMCHAIN_H
#include "WmChain.h"
#endif

#define DEGOUGE_NORMAL_USAGE			0x00
#define DEGOUGE_INCLUDE_ALL_ISLANDS		0x01
#define DEGOUGE_PROPOGATE_IINDEX		0x02

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmChainDegouger
{
public:

	CWmChainDegouger();

	void Init( CWmChain* chain, double tol );

	void Init(
			CWmChain*		outer,
			CWmChainList*	islands,
			double			tol );

	void FlagsSet( int flags );

	int Count() const;

	CWmChainList& Results();

	virtual ~CWmChainDegouger();

private:

	CWmChainList m_results;

	int	m_flags;
};

#endif
