
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"

#include "Return.h"
#include "3dCoord.h"
#include "3dVec.h"
#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "Worm.h"

#include "WmChain.h"
#include "WmChainIterator.h"
#include "WmChainDegouger.h"

#include "Spiral.h"

static int OFFSET_LEFT = 1;
static int OFFSET_RIGHT = -1;

// 2005.04.16 (PE) -- Change all references from VECTOR_SMALL to VECTOR_TOL
// because convexhull offset was sensitive dot & cross products.  As such,
// punching tools would sometimes be placed a wrong position.
static double VECTOR_TOL = 1.e-7;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CWorm::ProfOffset(
			const CProfile&	refProf,
			int				offsetDir,
			double			offsetAmt,
			double			sharpAngle,
			bool			closeGaps,
			CProfileList*	results )
{
	CReturn status;

	CWmChain chain;
	CWmChainList intermediate;

	chain.AttribsPropogate( true );
	Convert( refProf, closeGaps, &chain );

	if (0)
		chain.Dump();

	bool closed = chain.IsClosed();

	chain.Offset( offsetDir, offsetAmt, sharpAngle, &intermediate );

	int count = intermediate.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		const CWmChain* theChain = intermediate[indx];
#if 0
		// 2013.11.29 (PE) -- the following conditional was introduced to improve
		// nesting behavior for ITI. Doing this actually masks potential problems
		// but fixing the actual problem means fixing the offsetter :-(
		if (closed && !theChain->IsClosed())
			continue;
#endif
		CProfile* theProf = new CProfile();
		theProf->AttribsPropogate( true );

		Convert( (*theChain), theProf );

		results->Append( theProf );
	}

	intermediate.DestructiveFlush();

	return status;
}

CReturn
CWorm::PolyOffset(
			const CGeoPoly&	refPoly,
			int				offsetDir,
			double			offsetAmt,
			double			sharpAngle,
			CGeoPolyArray*	results )
{
	CReturn status;

	CWmChain chain;
	CWmChainList intermediate;

	Convert( refPoly, &chain );

	chain.Offset( offsetDir, offsetAmt, sharpAngle, &intermediate );

	int count = intermediate.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		const CWmChain* theChain = intermediate[indx];

		CGeoPoly* thePoly = new CGeoPoly();

		Convert( (*theChain), thePoly );

		results->Append( thePoly );
	}

	intermediate.DestructiveFlush();

	return status;
}

CReturn
CWorm::SpiralPocket(
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
				CProfile*		dstProf )
{
	CReturn status;

#if (_CI)
	if (passDepth < SMALL)
	{
		status.Internal( IDS_BAD_DEPTH_OF_CUT );
		return status;
	}
#endif

	CWmChain refChain;
	Convert( refProf, FALSE, &refChain );

	// Create the Spiral Pocketing toolpath for a planar region.
	CSpiral spiral;
	status = spiral.Init( refChain, toolDiam, stepOver, wallAllow, (cwResults != 0), (insideOut != 0), sharpAngle );

	if ( !status.IsOk() )
		return status;

	CProfile regionToolpath;
	Convert( spiral.Results(), &regionToolpath );

	if (regionToolpath.Count() < 1)
		return status;  // Either catastrophic failure, or tool too big?

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy the planar region toolpath to the various elevations.
	CProfile tmp;

	const C3dCoord& pt = regionToolpath.GetAt(0)->StartPt();

	// Per GW, the profile is drawn at the pocket floor.
	// Since the Z0 is the top of the stock, we want to calculate
	// the incremental depth of cut as (Z0 - floor) / nPasses.
	double floor = refProf.StartPt()->Z();
	double zDelta = fabs(floor - floorAllow);

	int nPasses;
	double iDepth;
#if (_CI)
	if (passDepth > zDelta)
	{
		iDepth = zDelta;
		nPasses = 1;
	}
	else
	{
		nPasses = (int) (zDelta / passDepth);
		if ((zDelta - (nPasses * passDepth)) > SMALL)
			++nPasses;  // So that we get an integral depth of cut.
		iDepth = zDelta / nPasses;
	}
#else
	iDepth = 0.;
	nPasses = 1;
#endif

	// Copy the base toolpath to the various cut levels.
	double prevDepth = 0.;
	for (int indx = 0; indx < nPasses; ++indx)
	{
		double currDepth = prevDepth - iDepth;

		if ((prevDepth - currDepth) > SMALL)
		{
			// Plunge to the next level.
			C3dCoord startPt( pt.X(), pt.Y(), prevDepth*toolUp );
			C3dCoord endPt( pt.X(), pt.Y(), currDepth*toolUp );

			CGeoLine connect( startPt, endPt );
			dstProf->CopyAppend( connect );
		}

		status = tmp.CopyAppend( regionToolpath );

		if ( !status.IsOk() )
			break;

		tmp.ZSet( currDepth*toolUp );
		dstProf->Append( &tmp );

		prevDepth = currDepth;
	}

	return status;
}

void
CWorm::Convert( const CWmChain& chain, CProfile* prof )
{
	CWmChainIterator iter( chain );

	while ( !iter.AtEnd() )
	{
		CWmElem* elem = iter.Elem();

		CGeoCurve* curve = dynamic_cast<CGeoCurve*>( elem->Elem() );

		if (curve != NULL)
			prof->CopyAppend( (*curve) );

		iter.NextElem();
	}
}

void
CWorm::Convert( const CWmChainList& chains, CProfile* prof )
{
	int count = chains.Count();

	for (int indx = 0; indx < count; ++indx )
	{
		const CWmChain* chain = chains[indx];
		Convert( (*chain), prof );
	}
}

void
CWorm::Convert( const CWmChainList& chains, CProfileList* profs )
{
	int count = chains.Count();

	for (int indx = 0; indx < count; ++indx )
	{
		CProfile* prof = new CProfile();

		const CWmChain* chain = chains[indx];
		Convert( (*chain), prof );

		profs->Append( prof );
	}
}

void
CWorm::Convert( const CProfile& prof, bool closeGaps, CWmChain* chain )
{
	// Voodoo magic but within tolerance of most machine precision!
	// Used to be SMALL (1.e-6) but this was problematic for certain
	// files (generated by SDRC's modeler) that contain numerous
	// C0 continuous short line segments (less than 1.e-4).
	//
	static double TOL = 1.e-5;

	if (prof.Count() > 0)
	{
		CProfile profCopy;
		profCopy.AttribsPropogate( true );

		CReturn status = profCopy.CopyAppend( prof );
		if ( status.IsOk() )
		{
			profCopy.LinesReduce( TOL );

			if ( closeGaps )
				profCopy.GapsClose();

			// Transfer ownership of the curves to the chain.
			int count = profCopy.Count();
			for (int indx = 0; indx < count; ++indx)
			{
				CGeoCurve* curve = profCopy.GetAt(indx);
				chain->Append( curve );
			}

			profCopy.BenignFlush();
		}
	}
}

void
CWorm::Convert( const CGeoPoly& poly, CWmChain* chain )
{
	if (poly.Count() > 0)
	{
		int count = poly.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			const CGeoCurve& curve = poly[indx];
			chain->CopyAppend( curve );
		}
	}
}

void
CWorm::Convert( const CWmChain& chain, CGeoPoly* poly )
{
	CWmChainIterator iter( chain );

	while ( !iter.AtEnd() )
	{
		CWmElem* elem = iter.Elem();

		CGeoCurve* curve = dynamic_cast<CGeoCurve*>( elem->Elem() );

		if (curve != NULL)
			poly->CopyAppend( *curve );

		iter.NextElem();
	}
}

// NOTE: Initially created in ChArc.cpp for Convex Hull Offset
// but later discovered to have utility in AutoIndexRules.cpp
int
CWorm::ArcOverlapClassify(
					const C2dUnitVec&	partArcStartTan,
					const C2dUnitVec&	partArcEndTan,
					int					partArcDir,
					const C2dUnitVec&	toolArcStartTan,
					const C2dUnitVec&	toolArcEndTan,
					int					toolArcDir,
					int					offsetDir )
{
	C2dUnitVec ts;
	C2dUnitVec te;

	// Get the radial vectors for the tool arc.
	// TODO: NOTE: Assumes convex tool !!!
	if (CWorm::Sign( toolArcDir ) == CCW)
	{
		ts.Init( toolArcStartTan.Y(), -toolArcStartTan.X() );
		te.Init( toolArcEndTan.Y(), -toolArcEndTan.X() );
	}
	else
	{
		// In this case, tooArc should represent a Move
		// (ie. its direction should be 2). I suppose
		// an assertion could be appropriate here.
		ts.Init( -toolArcStartTan.Y(), toolArcStartTan.X() );
		te.Init( -toolArcEndTan.Y(), toolArcEndTan.X() );
	}

	partArcDir = ((abs(partArcDir) == 1) ? CWorm::Sign( partArcDir ) : 0);
	offsetDir = CWorm::Sign( offsetDir );

	if (((offsetDir == LEFT) && (partArcDir == CW)) ||
		((offsetDir == RIGHT) && (partArcDir == CCW)))
	{
		ts.Radians( ts.Radians() + PI );
		te.Radians( te.Radians() + PI );
	}

	// Get the radial vectors for the part arc.z
	C2dUnitVec ps( partArcStartTan.Radians() - (offsetDir * HALFPI) );
	C2dUnitVec pe( partArcEndTan.Radians() - (offsetDir * HALFPI) );

	double psDts = ps * ts;
	double psDte = ps * te;
	double peDts = pe * ts;
	double peDte = pe * te;

	if (psDts < -VECTOR_TOL)
		return 0;

	if (psDte < -VECTOR_TOL)
		return 0;

	if (peDts < -VECTOR_TOL)
		return 0;

	if (peDte < -VECTOR_TOL)
		return 0;

	// Initialize the result.
	int classification = 0;

	// Determine the range of overlap in the vectors.
	double psXts = ps ^ ts;
	double psXte = ps ^ te;
	double peXts = pe ^ ts;
	double peXte = pe ^ te;

	int test;
	
	test = CWorm::Sign( psXts ) + CWorm::Sign( psXte );
	if (test > -2 && test < 2)
	{
		classification = (classification | PS_IN_TS_TE);
	}
	else
	{
		double tsXps = ts ^ ps;
		double tsXpe = ts ^ pe;

		test = CWorm::Sign( tsXps ) + CWorm::Sign( tsXpe );
		if (test > -2 && test < 2)
		{
			classification = (classification | TS_IN_PS_PE);
		}
	}

	test = CWorm::Sign( peXts ) + CWorm::Sign( peXte );
	if (test > -2 && test < 2)
	{
		classification = (classification | PE_IN_TS_TE);
	}
	else
	{
		double teXps = te ^ ps;
		double teXpe = te ^ pe;

		test = CWorm::Sign( teXps ) + CWorm::Sign( teXpe );
		if (test > -2 && test < 2)
		{
			classification = (classification | TE_IN_PS_PE);
		}
	}

	return classification;
}

int
CWorm::Sign( double dval )
{
	if (fabs( dval ) < VECTOR_TOL)
		return 0;

	return ((dval < 0) ? -1 : 1);
}


