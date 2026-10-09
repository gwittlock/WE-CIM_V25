
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"
#include "2dCoord.h"
#include "2dVec.h"
#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "WmChain.h"
#include "WmChainIterator.h"
#include "Worm.h"
#include "ChArc.h"
#include "ChArcListIterator.h"
#include "ConvexHull.h"

static bool g_debug = FALSE;
static bool g_bypass = FALSE;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// See also Docs/Minkowski.htm
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

////////////////////////////////////////////////////////////////////////

CReturn
CConvexHull::Convert( const CProfile& prof, int offsetDir, CChArcList* arcList )
{
	CReturn status;

	int count = prof.Count();
	int isClosed = 0; // prof.IsClosed();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* geoCurve = prof.GetAt(indx);

		CGeoLine* geoLine = dynamic_cast<CGeoLine*>( geoCurve );
		CGeoArc* geoArc = dynamic_cast<CGeoArc*>( geoCurve );

		CChArc* chArc = NULL;

		if (geoLine != NULL)
		{
			// Since the part profile is represented as a string
			// of arcs, we are guaranteed that the representation
			// will be C1 continuous.  As such, we need only create
			// a 'move' when the first and/or last element of a
			// profile is a line.

			C2dUnitVec ts = geoLine->StartTan();
			if (indx == 0)
			{
				chArc = new CChArc( geoLine->StartPt(), ts, ts, 0.0,
									((offsetDir < 0) ? -CHARC_MOVE : CHARC_MOVE) );
				arcList->Append( chArc );
			}
			
			if (indx == (count - 1))
			{
				chArc = new CChArc( geoLine->EndPt(), ts, ts, 0.0,
									((offsetDir < 0) ? -CHARC_MOVE : CHARC_MOVE) );
				arcList->Append( chArc );
			}
		}
		else if (geoArc != NULL)
		{
			CConvexHull::ArcConvert( geoArc, arcList );
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CConvexHull::Convert()" );
			break;
		}

		if (indx < (count - 1))
		{
			CConvexHull::Transition( geoCurve, prof.GetAt(indx+1), arcList );
		}
	}

	if (count > 1 && prof.IsClosed())
	{
		CConvexHull::Transition( prof.GetAt(count-1), prof.GetAt(0), arcList );
	}

	CConvexHull::Normalize( arcList );

	return status;
}

CReturn
CConvexHull::Convert( const CChArcList& arcList, CWmChain* chain )
{
	CReturn status;
	const CChArc* chArc;
	C3dCoord ps;
	C3dCoord pe;

	int count = arcList.Count();

	int indx = 0;
	while (indx < count)
	{
		chArc = arcList[indx];

		ps = chArc->StartPt();

		if ( chArc->IsTurn() )
		{
			// Try to construct an arc.

			pe = chArc->EndPt();

			if ( !ps.WithinTol( pe, SMALL ) )
			{
				CGeoArc* geoArc = new CGeoArc( ps, pe, chArc->CenterPt(), chArc->Dir() );
				chain->Append( geoArc );
			}

			ps = pe;
		}
		else if ( chArc->IsMove() )
		{
			// Defer construction of the line.
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CConvexHull::Convert()" );
		}

		++indx;

		if (indx < count)
		{
			chArc = arcList[indx];

			pe = chArc->StartPt();

			if ( !ps.WithinTol( pe, SMALL ) )
			{
				CGeoLine* geoLine = new CGeoLine( ps, pe );
				chain->Append( geoLine );
			}
		}
	}

	return status;
}

CReturn
CConvexHull::Offset(
				const CChArcList& part,
				const CChTool& tool,
				int offsetDir,
				CProfileList* offsetProfs )
{
	CReturn status;
	CChArcList rawOffset;

	status = Offset( part, tool, offsetDir, &rawOffset );

	if ( status.IsOk() )
		status = Finalize( rawOffset, offsetDir, offsetProfs );

	rawOffset.DestructiveFlush();

	return status;
}

CReturn
CConvexHull::ArcConvert( const CGeoArc* geoArc, CChArcList* arcList )
{
	CReturn status;

	CChArc* chArc = NULL;

	double rad = geoArc->Radius();
	int dir = geoArc->Dir();

	C2dUnitVec ts = geoArc->StartTan();
	C2dUnitVec te = geoArc->EndTan();

	chArc = new CChArc( geoArc->CenterPt(), ts, te, rad, dir );
	arcList->Append( chArc );

	return status;
}

CReturn
CConvexHull::Transition( const CGeoCurve* geoCurveA, const CGeoCurve* geoCurveB, CChArcList* arcList )
{
	CReturn status;

	C2dUnitVec ts = geoCurveA->EndTan();
	C2dUnitVec te = geoCurveB->StartTan();

	double dot = ts * te;

	if (dot < (1.0 - VECTOR_SMALL))
	{
		// The adjacent curves are essentially non-tangent.

		double rad = 0.0;
		double cross = ts ^ te;
		int dir = ((cross > 0) ? 1 : -1 );

		CChArc* chArc = new CChArc( geoCurveA->EndPt(), ts, te, rad, dir );
		arcList->Append( chArc );
	}

	return status;
}

CReturn
CConvexHull::Normalize( CChArcList* arcList )
{
	CReturn status;

	CChArcList quadArcs;

	int indx = 0;
	while (indx < arcList->Count())
	{
		CChArc* currArc = (*arcList)[indx];

		currArc->QuadrantArcs( &quadArcs );

		if (quadArcs.Count() > 1)
		{
			// Replace the current arc with the 'single quadrant' arcs.

			while (quadArcs.Count() > 0)
			{
				CChArc* quadArc = quadArcs.Remove( 0 );
				arcList->InsertBefore( indx, quadArc );
				++indx;
			}

			delete arcList->Remove( indx );  // Same as currArc.
		}
		else
		{
			++indx;
		}

		quadArcs.DestructiveFlush();
	}

	return status;
}

CReturn
CConvexHull::ToolPositionAdd(
					int classification,
					int offsetDir,
					const CChArcListIterator& part,
					const CChArcListIterator& tool,
					CChArcList* result )
{
	CReturn status;

	const CChArc* partArc = part();
	const CChArc* toolArc = tool();

	if (partArc == NULL || toolArc == NULL)
		return status;

#ifdef _DEBUG
	if ( g_debug )
	{
		CString msg;
		msg.Format( "part: %d  tool: %d  classification: %d",
				part.List().Find( (CChArc*)partArc ),
				tool.List().Find( (CChArc*)toolArc ),
				classification );
		status.Diagnostic( msg );
	}
#endif

	CChArc* convol = ConvolutionOfOverlap( classification, offsetDir, (*partArc), (*toolArc) );

	if ( convol->IsMove() )
	{
		// Prevent adding unnecessary moves and
		// moves that cause the tool to 'back up'.

		int count = result->Count();
		if (count > 0)
		{
			CChArc* prev = (*result)[count-1];
			C2dCoord pe = prev->EndPt();
			C2dCoord ps = convol->StartPt();

			if ( ps.WithinTol( pe, SMALL ) )
			{
				// There is no net change in tool position.
				delete convol;
				convol = NULL;
			}
			else // if ( prev->IsMove() )
			{
				C2dVec vec = ps - pe;
				C2dUnitVec uvec( vec.X(), vec.Y() );
				double dot = uvec * prev->EndTan();

				if (dot < (-1.0 + VECTOR_SMALL))
				{
					// The tool will be backing up.
					delete convol;
					convol = NULL;
				}
			}
		}
	}

	if (convol != NULL)
		result->Append( convol );

	return status;
}

CChArc* 
CConvexHull::ConvolutionOfOverlap(
					int classification,
					int offsetDir,
					const CChArc& partArc,
					const CChArc& toolArc )
{
	static double stockAmt = 0.0;
	double radius;

	C2dVec vec = partArc.CenterPt() - toolArc.CenterPt();
	C2dCoord pc( vec.X(), vec.Y() );

	if ( partArc.IsMove() )
	{
		radius = toolArc.Radius() + stockAmt;
	}
	else
	{
		if ((partArc.Dir() == CW && offsetDir == RIGHT) ||
			(partArc.Dir() == CCW && offsetDir == LEFT))
		{
			radius = partArc.Radius() - toolArc.Radius() - stockAmt;
		}
		else
		{
			radius = partArc.Radius() + toolArc.Radius() + stockAmt;
		}
	}

	if (fabs(radius) < SMALL)
		radius = 0.0;

	// See also -- CChArc::OverlapClassify()

	int sameDir = CChArc::SameDirection( &partArc, &toolArc );

	C2dUnitVec toolArcTs = (sameDir ? toolArc.StartTan() : toolArc.EndTan() );
	C2dUnitVec toolArcTe = (sameDir ? toolArc.EndTan() : toolArc.StartTan() );

	C2dUnitVec ts = toolArcTs;
	C2dUnitVec te = toolArcTe;

	if (classification & PS_IN_TS_TE)
		ts = partArc.StartTan();
	else if (classification & TS_IN_PS_PE)
		ts.Radians( toolArcTs.Radians() + ((offsetDir < 0) ? PI : 0.0) );

	if (classification & PE_IN_TS_TE)
		te = partArc.EndTan();
	else if (classification & TE_IN_PS_PE)
		te.Radians( toolArcTe.Radians() + ((offsetDir < 0) ? PI : 0.0) );

	CChArc* result = new CChArc( pc, ts, te, radius, partArc.Dir() );

	double dot = ts * te;
	if (dot > (1.0 - VECTOR_SMALL))
	{
		C2dCoord ps = result->StartPt();
		C2dCoord pe = result->EndPt();
		if ( ps.WithinTol( pe, SMALL ) )
		{
			C2dUnitVec vec;
			if ( partArc.IsMove() )
				vec.Radians( ts.Radians() + (CWorm::Sign( offsetDir ) * HALFPI) );
			else
				vec.Radians( ts.Radians() - (CWorm::Sign( partArc.Dir() ) * HALFPI) );

			pc = pc + (vec * fabs( radius ));

			delete result;
			result = NULL;

			result = new CChArc( pc, ts, te, 0.0, ((offsetDir < 0) ? -CHARC_MOVE : CHARC_MOVE) );
		}
	}

	return result;
}

#ifdef _DEBUG
void
CConvexHull::Dump( const CChArcList& arcList )
{
	CReturn trace;
	CString msg;

	int count = arcList.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CChArc* chArc = arcList[ indx ];

		if (chArc == NULL)
		{
			msg.Format( "arc[%d]  NULL", indx );
		}
		else
		{
			C2dCoord pc = chArc->CenterPt();
			C2dUnitVec ts = chArc->StartTan();
			C2dUnitVec te = chArc->EndTan();
			double rad = chArc->Radius();
			int dir = chArc->Dir();

			msg.Format(
					"arc[%d] xc=%-7.4f yc=%-7.4f ux=%-7.4f uy=%-7.4f vx=%-7.4f vy=%-7.4f rad=%-7.4f dir=%d",
					indx, pc.X(), pc.Y(), ts.X(), ts.Y(), te.X(), te.Y(), rad, dir );
		}

		trace.Diagnostic( msg );
	}
}
#endif

// Attempt to eliminate the overlap between the first
// and last elements of the offset as often happens
// when the first and last elements represent 'moves'.
// If this step is not taken, the degouger will crash.
void
CConvexHull::Cleanup( CWmChain* chain )
{
	if (chain->Count() <= 2)
		return;

	// Get the first and last elements.

	CWmChainIterator iter( (*chain) );

	CWmElem* elemA = iter.Elem();
	if (elemA == NULL)
		return;

	iter.GotoEnd();
	iter.PrevElem();

	CWmElem* elemB = iter.Elem();
	if (elemB == NULL)
		return;

	// For further consideration, the first and last elements must be lines.

	CGeoLine* lineA = dynamic_cast<CGeoLine*>( elemA->Elem() );
	CGeoLine* lineB = dynamic_cast<CGeoLine*>( elemB->Elem() );

	if (lineA == NULL || lineB == NULL)
		return;

	// For further consideration, the lines must be colinear

	C2dUnitVec tanA = lineA->StartTan();
	C2dUnitVec tanB = lineB->StartTan();

	double dot = tanA * tanB;
	if (dot < (1.0 - VECTOR_SMALL))
		return;

	// For further consideration, the lines must be coincident.

	C3dCoord closestPt;
	double uA, uB;

	const C3dCoord& ps = lineB->StartPt();
	const C3dCoord& pe = lineB->EndPt();

	lineA->PointClosest( ps, &closestPt, &uA );
	lineA->PointClosest( pe, &closestPt, &uB );

	if ((uA > -VECTOR_SMALL && uA < (1.0 + VECTOR_SMALL)) ||
		(uB > -VECTOR_SMALL && uB < (1.0 + VECTOR_SMALL)))
	{
		C2dUnitVec vec(
					pe.X() - lineA->StartPt().X(),
					pe.Y() - lineA->StartPt().Y() );

		dot = vec * tanA;

		if (dot > (1.0 - VECTOR_SMALL))
		{
			// Remove the last element in the offset.
			// NOTE: This is not a well recommended way of
			// doing things.  It would reek havoc on any
			// iterator that is using the chain.

#if BEFORE
			CWmSubchn* subchn = iter.Subchn();
			subchn->Unlink( elemB );
			delete elemB;
#else
			elemB->Unlink();
			delete elemB;
#endif
		}
	}
}

CReturn
CConvexHull::Offset(
				const CChArcList& part,
				const CChTool& tool,
				int offsetDir,
				CChArcList* rawOffset )
{
	CReturn status;

	CChArcListIterator partIter( part );
	CChArcListIterator toolIter( tool.ArcList() );

#ifdef _DEBUG
	if ( g_debug )
	{
		status.Diagnostic( "\n" );
		status.Diagnostic( "Part representation" );
		Dump( part );
		status.Diagnostic( "Tool representation" );
		Dump( tool.ArcList() );
		status.Diagnostic( "\n" );
		status.Diagnostic( "Offset iteration" );
	}
#endif

	int moveOnTool = FALSE;
	int moveOnPart = FALSE;
	int classification = 0;

	while (1)
	{
		if ( !moveOnTool && !moveOnPart )
		{
			// Initial condition.  Find the most forward arc of the tool
			// that can be positioned at the first point on the part.

			classification = toolIter.StartPosition( partIter, offsetDir );
			if (classification == 0)
				break;  // We can't find a suitable starting point.

			status = ToolPositionAdd( classification, offsetDir, partIter, toolIter, rawOffset );
			if ( !status.IsOk() )
				break;

			moveOnTool = TRUE;
		}

		if ( moveOnTool )
		{
			if ( partIter.AtEnd() )
				break;  // We are done

			// Find the next arc on the tool that can be positioned
			// at the current point on the part.
			classification = toolIter.Advance( partIter(), offsetDir );
			if (classification == 0)
			{
				// We can not find a positioning point on the tool.

				if ( partIter.AtEnd() )
					break;  // We are done

				moveOnTool = FALSE;
				moveOnPart = TRUE;
			}
			else
			{
				status = ToolPositionAdd( classification, offsetDir, partIter, toolIter, rawOffset );
				if ( !status.IsOk() )
					break;
			}
		}

		if ( moveOnPart )
		{
			// Advance our current position on the part.

			partIter.Next();
			moveOnTool = FALSE;
			moveOnPart = FALSE;
		}
	}


#ifdef _DEBUG
	if ( g_debug )
	{
		status.Diagnostic( "\n" );
		status.Diagnostic( "Raw Offset represenation" );
		Dump( (*rawOffset) );
		status.Diagnostic( "\n" );
	}
#endif

	return status;
}

CReturn
CConvexHull::Finalize( const CChArcList& rawOffset, int offsetDir, CProfileList* offsetProfs )
{
	CReturn status;

	CWmChain rawOffsetChain;
	CWmChainList degougedChains;

	status = CConvexHull::Convert( rawOffset, &rawOffsetChain );

	if ( status.IsOk() )
	{
		Cleanup( &rawOffsetChain );

		if (offsetDir > 0)
			rawOffsetChain.Reverse();  // TODO: Is this the right thing to do?

#ifdef _DEBUG
		if ( g_debug )
		{
			status.Diagnostic( "\n" );
			rawOffsetChain.Debug("rawOffsetChain");
			status.Diagnostic( "\n" );
		}
#endif
		if ( g_bypass )
		{
			CProfile* rawProf = new CProfile();
			CWorm::Convert( rawOffsetChain, rawProf );
			offsetProfs->Append( rawProf );
		}
		else
		{
			status = rawOffsetChain.Degouge( offsetDir, &degougedChains );
		}
	}

	if ( status.IsOk() )
		CWorm::Convert( degougedChains, offsetProfs );

	degougedChains.DestructiveFlush();

	return status;
}

