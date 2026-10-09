
#ifndef _CHTOOL_H
#define _CHTOOL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "GeoCurve.h"
#include "ChArc.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Convex Hull Tool
//
class dllExport CChTool
{
public:

	CChTool();

	// Constructs a convex hull tool having CCW orientation.
	// Failure occurs under the following conditions:
	// 1. The given profile is not closed.
	// 2. The given profile is not convex.
	CReturn Init( const CProfile& profile );
	CReturn Init( const CGeoCurveArray& curves );

	const CChArcList& ArcList() const;

	// Creates a rotated copy of this object.
	CChTool* Copy( double radians ) const;

	virtual ~CChTool();

protected:

private:  // Methods

	CReturn CommonInit( CProfile* prof );

private:  // Disabled

	CChTool( const CChTool& );
	const CChTool& operator = ( const CChTool& );
	int operator == ( const CChTool& ) const;
	int operator != ( const CChTool& ) const;

private:  // Data

	CChArcList m_list;
	bool m_indexable;
};

#endif

