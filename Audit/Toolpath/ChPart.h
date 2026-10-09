
#ifndef _CHPART_H
#define _CHPART_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "GeoCurve.h"
#include "ChArc.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Convex Hull Part
//
class dllExport CChPart
{
public:

	CChPart();

	// Constructs a convex hull tool having CCW orientation.
	// Failure occurs under the following conditions:
	// 1. The given profile is not closed.
	// 2. The given profile is not convex.
	CReturn Init( const CProfile& profile, int offset_dir );
	CReturn Init( const CGeoCurveArray& curves, int offset_dir );

	const CChArcList& ArcList() const;

	virtual ~CChPart();

protected:

private:  // Methods

	CReturn CommonInit( CProfile* prof, int offset_dir );

private:  // Disabled

	CChPart( const CChPart& );
	const CChPart& operator = ( const CChPart& );
	int operator == ( const CChPart& ) const;
	int operator != ( const CChPart& ) const;

private:  // Data

	CChArcList m_list;
};

#endif

