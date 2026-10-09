
#ifndef _WORM_H
#define _WORM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include "Indxlist.h"
#include "Profile.h"
#include "GeoPoly.h"
#include "WmChain.h"

// Overlap classification constants
// PS -- part start
// PE -- part end
// TS -- tool start
// TE -- tool end
//
const int PS_IN_TS_TE = 8;
const int PE_IN_TS_TE = 4;
const int TS_IN_PS_PE = 2;
const int TE_IN_PS_PE = 1;

enum EEdge
{
	BACK_EDGE,
	FRONT_EDGE
};

class CReturn;
class C3dCoord;
class CGeoCurve;



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWorm  
{
public:

	static CReturn ProfOffset(
					const CProfile&	refProf,
					int				offsetDir,
					double			offsetAmt,
					double			sharpAngle,
					bool			closeGaps,
					CProfileList*	results );

	static CReturn PolyOffset(
					const CGeoPoly&	refPoly,
					int				offsetDir,
					double			offsetAmt,
					double			sharpAngle,
					CGeoPolyArray*	results );

	static CReturn SpiralPocket(
					const CProfile& refProf,
					double			toolDiam,
					int				toolUp,
					double			stepOver,
					double			wallAllow,
					int				cwResults,
					int				insideOut,
					double			sharpAngle,
					double			passDepth,
					double			floorAllow,
					CProfile*		dstProf );

	static void Convert( const CWmChain& chain, CProfile* prof );
	static void Convert( const CWmChainList& chains, CProfile* prof );
	static void Convert( const CWmChainList& chains, CProfileList* profs );
	static void Convert( const CProfile& prof, bool closeGaps, CWmChain* chain );

	static void Convert( const CGeoPoly& prof, CWmChain* chain );
	static void Convert( const CWmChain& chain, CGeoPoly* prof );

	static int ArcOverlapClassify(
					const C2dUnitVec&	partArcStartTan,
					const C2dUnitVec&	partArcEndTan,
					int					partArcDir,
					const C2dUnitVec&	toolArcStartTan,
					const C2dUnitVec&	toolArcEndTan,
					int					toolArcDir,
					int					offsetDir );
	
	static int Sign( double dval );

private:

	// Disabled.
	CWorm();
	CWorm( const CWorm& );
	virtual ~CWorm();
	CWorm& operator = ( const CWorm& );
	int operator == ( const CWorm& );
	int operator != ( const CWorm& );

private:

};

#endif
