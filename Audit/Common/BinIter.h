#ifndef _BINITER_H
#define _BINITER_H

#include "stdafx.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CBinIter
{
public:

	CBinIter();

	void IterInit( int min, int max );

	int Next( int dir );

	int Mid() const;

	virtual ~CBinIter();

private:

	int	m_max;
	int	m_min;
	int	m_mid;
};

#endif
