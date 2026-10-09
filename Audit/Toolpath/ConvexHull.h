
#ifndef _CONVEXHULL_H
#define _CONVEXHULL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

#ifndef _PROFILE_H
#include "Profile.h"
#endif

#ifndef _WMCHAIN_H
#include "WmChain.h"
#endif

#ifndef _CHARC_H
#include "ChArc.h"
#endif

#ifndef _CHTOOL_H
#include "ChTool.h"
#endif

#ifndef _CHARCLISTITERATOR_H
#include "ChArcListIterator.h"
#endif

class CGeoLine;
class CGeoArc;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CConvexHull
{
public:

	// Convert a part or tool profile to the representation needed
	// by CConvexHull::Offset().  A CProfile object can be obtained
	// by using Model\Conversion.cpp -- CConversion::Convert().
	//
	// The offsetDir is relevant to part profiles only, where
	//     (right) offsetDir > 0 / (left) offsetDir < 0.
	static CReturn Convert( const CProfile& prof, int offsetDir, CChArcList* arcList );

	// Given part and tool profiles generate the Minkowski sum of
	// the part and the tool.
	static CReturn Offset(
							const CChArcList& part,
							const CChTool& tool,
							int offsetDir,
							CProfileList* offsetProfs );

	// Convert all multiple quadrant arcs into single quadrant arcs.
	static CReturn Normalize( CChArcList* arcList );

#ifdef _DEBUG
	static void Dump( const CChArcList& arcList );
#endif

protected:

private:  // Methods

	static CReturn Convert( const CChArcList& arcList, CWmChain* chain );

	static CReturn ArcConvert( const CGeoArc* geoArc, CChArcList* arcList );
	static CReturn Transition( const CGeoCurve* geoCurveA, const CGeoCurve* geoCurveB, CChArcList* arcList );

	static CReturn ToolPositionAdd(
							int classification,
							int offsetDir,
							const CChArcListIterator& part,
							const CChArcListIterator& tool,
							CChArcList* result );

	static CChArc* ConvolutionOfOverlap(
							int classification,
							int offsetdir,
							const CChArc& part,
							const CChArc& tool );

	static void Cleanup( CWmChain* chain );

	static CReturn Offset(
							const CChArcList& part,
							const CChTool& tool,
							int offsetDir,
							CChArcList* rawOffset );

	static CReturn Finalize(
							const CChArcList& rawOffset,
							int offsetDir,
							CProfileList* offsetProfs );

private:  // Disabled

private:  // Data

};

#endif

