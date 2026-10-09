
#ifndef _RASTER_H
#define _RASTER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WmChain.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CRaster
{
public:

	CRaster();

	void Trim( CWmChainList* boundaries, CWmChainList* paths, double tol, double set_back );

	int Count() const;

	const CWmChainList& Results() const;

	void Flush();

	virtual ~CRaster();

private:

	bool CanTrim( const CWmSubchn* raster, const CWmChain* material );

private:

	CWmChainList m_results;
};

#endif
