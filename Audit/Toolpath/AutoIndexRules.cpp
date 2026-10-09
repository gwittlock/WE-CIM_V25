
#include "stdafx.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "Worm.h"
#include "AutoIndexRules.h"



////////////////////////////////////////////////////////////////////////

CAutoIndexRules::CAutoIndexRules()
	: m_rules(),
	  m_use_long_side( false )
{
}

CAutoIndexRules::~CAutoIndexRules()
{
	m_rules.DestructiveFlush();
}

void
CAutoIndexRules::Init(
	const CShape&	part,
	const CShape&	tool,
	int				offsetDir,
	bool			use_long_side )
{
	int pndx, bcnt;
	
	int pcnt = part.Count();
	int tcnt = tool.Count();

	if (pcnt < 1 || tcnt < 1)
		return;

	m_use_long_side = use_long_side;

	int* bestFace = new int[tcnt];

	// Assume CW part.
	// Assume CW tool.
	for (pndx = 0; pndx < pcnt; ++pndx)
	{
		const CGeoCurve& pCurve = (*(part[pndx]));

		// Find the tool faces that can dress this part face.
		ToolFacesFind( pCurve, tool, offsetDir, bestFace, &bcnt );

		if (bcnt > 0)
		{
			// There is at least one tool face that can dress the part face.

			// Sort the tool face in increasing order of tool rotation required
			// to align the tool face with the part face.  Minimizing rotation
			// is not so much an issue as is promoting even tool wear.  This
			// means we should find the tool face that is 'best aligned' with
			// the part face.  For instance, if we have a square tool and a
			// rectangular part, the left edge of tool should dress the left
			// edge of the part, the top edge of the tool should dress the
			// top edge of the part, and so on.

			int tndx;
			double trot;
			ToolFacesOrder( pCurve, tool, offsetDir, bestFace, bcnt, &tndx, &trot );

			RulesCreate( part, pndx, tool, tndx, trot, offsetDir );
		}
	}

	delete [] bestFace;
}

// Gets the count of rules required to offset the part.
int
CAutoIndexRules::Count() const
{
	return m_rules.Count();
}

// Gets the Ith rule.
const CAutoIndexRule*
CAutoIndexRules::operator [] ( int indx ) const
{
	return m_rules[indx];
}

// Finds candidate tool faces that can be used to dress the given part face.
void
CAutoIndexRules::ToolFacesFind(
						const CGeoCurve&	pCurve,
						const CShape&		tool,
						int					offsetDir,
						int*				bestFace,
						int*				bcnt )
{
	int tcnt = tool.Count();
	if (tcnt < 1)
		return;


	const CGeoLine* pLine = dynamic_cast<const CGeoLine*>( &pCurve );
	const CGeoArc* pArc = dynamic_cast<const CGeoArc*>( &pCurve );

	double plen = 0.0;
	bool outsideArc = FALSE;
	if (pLine != NULL)
	{
		plen = pLine->Length2d();
	}
	else if (pArc != NULL)
	{
		outsideArc = OutsideCut( pArc, offsetDir );
		plen = pArc->Radius();
	}
	else
		return;



	double* tlenArray = new double[tcnt];

	double tlen = UNDEFINED;
	double blen = 0.0;
	
	int lineCount = tool.LineCount();

	int tndx;

	// Find the length of the tool face that 'best fits' the part face.
	for (tndx = 0; tndx < tcnt; ++tndx)
	{
		const CGeoCurve* tCurve = tool[tndx];

		tlen = CandidateFaceMeasure( &pCurve, tCurve, offsetDir );
		tlenArray[tndx] = tlen;

		if ( Comparable( pCurve, (*tCurve), offsetDir, lineCount ) )
		{
			// By default, we want to consider all faces of
			// the tool so that we find the longest face.
			bool consider = m_use_long_side;

			if ( !consider )
			{
				// The preference is set to NOT use the longest face.
				//
				// When processing a part line, the result should be
				// that we find the longest tool face whose length is
				// less-than-or-equal-to the length of the part line.
				//
				// When processing a part arc, the result should be
				// that we simply find the longest tool face.  This
				// will guarantee the smallest cusp with the largest
				// step-over(?)

				if (outsideArc && tCurve->Type() == GEOLINE)
					consider = true;
				else
					consider = ((tlen - SMALL) < plen);
			}

			if (consider && (tlen > blen))
			{
				blen = tlen;
			}
		}
	}

	if (blen < SMALL)
	{
		// We did not find any tool face that was a best fit (and small enough)
		// to dress this face of the part.  Now find the shortest tool face that
		// is longer than the part face.

		blen = UNDEFINED;

		for (tndx = 0; tndx < tcnt; ++tndx)
		{
			const CGeoCurve* tCurve = tool[tndx];

			tlen = CandidateFaceMeasure( &pCurve, tCurve, offsetDir );
			tlenArray[tndx] = tlen;

			if ( Comparable( pCurve, (*tCurve), offsetDir, lineCount ) )
			{
				if ((tlen < blen) ||
					(outsideArc && tCurve->Type() == GEOLINE))
				{
					blen = tlen;
				}
			}
		}

		if (blen >= UNDEFINED)
			blen = 0.;
	}

	// Cull the tool faces to those that match the 'best fit' criteria.
	(*bcnt) = 0;
	for (tndx = 0; tndx < tcnt; ++tndx)
	{
		const CGeoCurve* tCurve = tool[tndx];
		tlen = tlenArray[tndx];

		if ( Comparable( pCurve, (*tCurve), offsetDir, lineCount ) )
		{
			if (fabs( tlen - blen ) <= SMALL)
			{
				bestFace[(*bcnt)] = tndx;
				++(*bcnt);
			}
		}
	}

	delete [] tlenArray;
}

// Sort the tool faces in increasing order of tool rotation required
// to align the tool face with the part face.
void
CAutoIndexRules::ToolFacesOrder(
							const CGeoCurve&	pCurve,
							const CShape&		tool,
							int					offsetDir,
							const int*			bestFace,
							int					bestCount,
							int*				tndx,
							double*				trot )
{
	CRotPairArray rotPairArray;
	const CGeoCurve* tCurve;
	int indx, jndx;

	int tLineCount = 0;
	int tArcCount = 0;

	for (indx = 0; indx < bestCount; ++indx)
	{
		jndx = bestFace[indx];

		tCurve = tool[jndx];

		if (tCurve->Type() == GEOLINE)
			++tLineCount;
		else if (tCurve->Type() == GEOARC)
			++tArcCount;

		double theta = ToolRotation( &pCurve, tCurve, offsetDir );

		rotPairArray.Append( new CRotPair( jndx, theta ) );
	}

	rotPairArray.Qsort( CRotPair::Compare );

	for (indx = 0; indx < bestCount; ++indx)
	{
		jndx = rotPairArray[indx]->Index();

		tCurve = tool[jndx];

		if (tLineCount > 0 && tCurve->Type() == GEOLINE)
			break;
		else if (tArcCount > 0 && tCurve->Type() == GEOARC)
			break;
	}

	if (indx >= bestCount)
		indx = 0;

	(*tndx) = rotPairArray[indx]->Index();
	(*trot) = rotPairArray[indx]->Radians();

	if (tool[(*tndx)]->Type() == GEOLINE && offsetDir > 0)
		(*trot) += PI;

	rotPairArray.DestructiveFlush();
}

bool
CAutoIndexRules::OutsideCut( const CGeoArc* pArc, int offsetDir )
{
	int cross = (pArc->Dir() * offsetDir);

	return (cross < 0);
}

// Assume symmetric tool.
// Assume all tool radii are identical (except for banana tool).
// Assume outside of part arc can be nibbled by either tool arc or line.
// Assume inside of part arc can be nibbled by tool arc only.
//
double
CAutoIndexRules::CandidateFaceMeasure(
						const CGeoCurve*	partCurve,
						const CGeoCurve*	toolCurve,
						int					offsetDir )
{
	const CGeoLine* pLine = dynamic_cast<const CGeoLine*>( partCurve );
	const CGeoArc* pArc = dynamic_cast<const CGeoArc*>( partCurve );

	const CGeoLine* tLine = dynamic_cast<const CGeoLine*>( toolCurve );
	const CGeoArc* tArc = dynamic_cast<const CGeoArc*>( toolCurve );

	double tlen = UNDEFINED;

	if (pLine != NULL)
	{
		// A line on the part can be dressed by either a line
		// or arc of the tool.  Where a banana tool is concerned,
		// the line can be dressed by CW arcs only (ie. convex).

		if (tLine != NULL)
		{
			// As a first try, we want to use the longest face
			// of the tool that does not exceed the length of
			// of the part face.  It is possible for this scheme
			// to fail when attempting to dress an inside notch,
			// but that situation should be caught later.
			//
			//               |
			//               |
			//              /
			//            /
			//           |
			//            \
			//              \
			//               |
			//               |
			//
			tlen = tLine->Length2d();
		}
		else if (tArc != NULL && tArc->Dir() == CW)
		{
			// Using the largest radius arc will certainly yield
			// the least number of hits when nibbling, but this
			// is true only for lines that are 'free and clear'
			//
			//           O
			//         -------------
			//        |             |
			//
			// as opposed to
			//
			//        |   O         |
			//         -------------
			//
			// This is all moot because of our basic assumption
			// that all tool radii are identical, however.
			//
			// TODO: There may well be better heuristics for
			// determining the best fit arc to a line.
			//
			if (tArc->IncludedAngle() >= PI)
			{
				tlen = 2 * tArc->Radius();
			}
			else
			{
				C2dVec vec = tArc->EndPt() - tArc->StartPt();
				tlen = vec.Length();
			}
		}
	}
	else if (pArc != NULL)
	{
		// Processing an arc on the part is more complicated.
		// We want to minimize the number of hits by finding
		// the tool face that yields the largest stepover for
		// a given cusp height.

		if (tLine != NULL && OutsideCut( pArc, offsetDir ))
		{
			// A line tool face can be used to dress the outside
			// of a part arc only, in which case the part arc will
			// appear faceted.  If we were to allow the line to
			// dress the inside of the arc, the result would be
			// a sawtooth pattern.
			//
			tlen = tLine->Length2d();
		}
		else if (tArc != NULL && (tArc->Dir() == CW || OutsideCut( pArc, offsetDir )))
		{
			// We can dress both the inside and outside of a
			// part arc with a convex portion of the tool, but
			// we can only use a concave portion of the tool
			// to dress the outside of a part arc.
			//
			tlen = tArc->Radius();
		}
	}

	return tlen;
}

double
CAutoIndexRules::ToolRotation(
						const CGeoCurve*	partCurve,
						const CGeoCurve*	toolCurve,
						int					offsetDir )
{
	double dot = UNDEFINED;
	double cross = UNDEFINED;
	double theta = UNDEFINED;

	const CGeoLine* pLine = dynamic_cast<const CGeoLine*>( partCurve );
	const CGeoArc* pArc = dynamic_cast<const CGeoArc*>( partCurve );

	const CGeoLine* tLine = dynamic_cast<const CGeoLine*>( toolCurve );
	const CGeoArc* tArc = dynamic_cast<const CGeoArc*>( toolCurve );
		
	C2dUnitVec ptan = partCurve->StartTan();

	if (pLine != NULL)
	{
		if (tLine != NULL)
		{
			// We must align the tool face with the part face.

			C2dUnitVec ttan = tLine->StartTan();

			theta = Rotation( ttan, ptan );
		}
		else if (tArc != NULL && tArc->Dir() == CW)
		{
			// The mid-point of the arc will be the contact point to
			// the line.  This way, there is equal cutting potential
			// by either half of the arc.
			//
			// NOTE: For round tools, outputting a rotation command
			// in NC code is unnecessary, but the rotation here is
			// necessary to maintain a generic algorithm.

			C2dCoord pc = tArc->CenterPt();

			C2dUnitVec antiNorm = ptan - (SGN(offsetDir) * HALFPI);
			C2dCoord arcPt = tArc->MidPt();

			C2dUnitVec radialVec( arcPt.X() - pc.X(), arcPt.Y() - pc.Y() );

			theta = Rotation( radialVec, antiNorm );
		}
	}
	else if (pArc != NULL)
	{
		if (tLine != NULL && OutsideCut( pArc, offsetDir ))
		{
			// We can dress the part arc on the outside only.
			// Simply base the decision on the start tangent.
			//
			// TODO: Do we need some approximate cusp height?
			// If we don't, then how do know whether the tool
			// can dress then entire arc?  The part arc may
			// not be 'free and clear'!

			C2dUnitVec ttan = tLine->StartTan();

			theta = Rotation( ttan, ptan );
		}
		else if (tArc != NULL && tArc->Dir() == CW)
		{
			// The tool must be rotated.
			// NOTE: The rotation does not have to appear in NC code for a round tool.

			C2dCoord pArcCtrPt = pArc->CenterPt();
			C2dCoord pArcMidPt = pArc->StartPt();

			C2dCoord tArcCtrPt = tArc->CenterPt();
			C2dCoord tArcMidPt = tArc->MidPt();

			C2dUnitVec antiNorm = ptan - (SGN(offsetDir) * HALFPI);

			C2dUnitVec radialVec( 
					(tArcMidPt.X() - tArcCtrPt.X()),
					(tArcMidPt.Y() - tArcCtrPt.Y()) );

			theta = Rotation( radialVec, antiNorm );
		}
	}


	return theta;
}

void
CAutoIndexRules::RulesCreate(
						const CShape&	part,
						int				pndx,
						const CShape&	tool,
						int				tndx,
						double			trot,
						int				offsetDir )
{
	C3dVec pbTrans;
	C3dVec pfTrans;
	C3dVec tbTrans;
	C3dVec tfTrans;
	double pbRot, pfRot, tbRot, tfRot;
	bool oversizedToolFace = FALSE;

	const CGeoCurve* pCurve = part[pndx];
	const CGeoCurve* tCurve = tool[tndx];

	PartXformParams( BACK_EDGE, pCurve, &pbTrans, &pbRot );
	ToolXformParams( BACK_EDGE, pCurve, tCurve, offsetDir,
					 pbRot, trot, &tbTrans, &tbRot );

	PartXformParams( FRONT_EDGE, pCurve, &pfTrans, &pfRot );
	ToolXformParams( FRONT_EDGE, pCurve, tCurve, offsetDir,
					 pfRot, trot, &tfTrans, &tfRot );

	if ( (dynamic_cast<const CGeoLine*>(pCurve) != NULL) &&
		 (dynamic_cast<const CGeoLine*>(tCurve) != NULL) )
	{
		if (tCurve->Length2d() > pCurve->Length2d())
			oversizedToolFace = TRUE;
	}

	CAutoIndexRule* rule = new CAutoIndexRule(
									pndx, tndx, trot,
									pbTrans, pbRot, tbTrans, tbRot,
									pfTrans, pfRot, tfTrans, tfRot,
									oversizedToolFace );

	m_rules.Append( rule );
}

void
CAutoIndexRules::PartXformParams(
						EEdge				orient,
						const CGeoCurve*	curve,
						C3dVec*				trans,
						double*				radians )
{
	C2dUnitVec tan;
	C2dUnitVec vec;
	C2dCoord pt;

	const CGeoLine* pLine = dynamic_cast<const CGeoLine*>( curve );
	const CGeoArc* pArc = dynamic_cast<const CGeoArc*>( curve );

	(*radians) = 0.0;
	trans->Init( 0., 0., 0. );

	if (pLine != NULL || pArc != NULL)
	{
		if (orient == BACK_EDGE )
		{
			pt = curve->StartPt();
			tan = curve->StartTan();
			vec = C2dUnitVec( 0, 1 );
		}
		else
		{
			pt = curve->EndPt();
			tan = curve->EndTan();
			vec = C2dUnitVec( 0, -1 );
		}

		(*radians) = Rotation( tan, vec );

		trans->Init( -pt.X(), -pt.Y(), 0. );
	}
}

void
CAutoIndexRules::ToolXformParams(
						EEdge				orient,
						const CGeoCurve*	pCurve,
						const CGeoCurve*	tCurve,
						int					offsetDir,
						double				pRotation,
						double				tRotation,
						C3dVec*				trans,
						double*				radians )
{
	C2dCoord pt;

	const CGeoLine* pLine = dynamic_cast<const CGeoLine*>( pCurve );
	const CGeoArc* pArc = dynamic_cast<const CGeoArc*>( pCurve );

	const CGeoLine* tLine = dynamic_cast<const CGeoLine*>( tCurve );
	const CGeoArc* tArc = dynamic_cast<const CGeoArc*>( tCurve );

	(*radians) = 0.0;
	trans->Init( 0., 0., 0. );

	// During tool positioning, we know that the part is transformed
	// such that the part face under consideration is translated to
	// the origin and aligned with the Y-axis.
	//
	// For a tool face which is a line, the tool face must be transformed
	// such that it is aligned with the part face (in its transformed position).
	//
	// For a tool face which is an arc, rotation is about the arc contact point
	// with the part face.  The arc contact point is always the arc mid-point.

	if (pLine != NULL)
	{
		if (tLine != NULL || tArc != NULL)
		{
			pt = tCurve->MidPt();

			(*radians) = pRotation + tRotation;

			trans->Init( -pt.X(), -pt.Y(), 0. );
		}
	}
	else if (pArc != NULL)
	{
		if (tLine != NULL || tArc != NULL)
		{
			pt = tCurve->MidPt();

			(*radians) = pRotation + tRotation;
			if (orient == FRONT_EDGE)
			{
				double ai = (pArc->Dir() * pArc->IncludedAngle());
				(*radians) += ai;
			}

			trans->Init( -pt.X(), -pt.Y(), 0. );
		}
	}
}

double
CAutoIndexRules::Rotation( const C2dUnitVec& vecA, const C2dUnitVec& vecB )
{
	double dot = vecA * vecB;
	double cross = vecA ^ vecB;

	// 2205.02.23 (PE) -- Using a tolerance of VECTOR_SMALL lead to
	// failures on near horizontal lines.  The case at hand had vector
	// components of ( 1.0000000000000 , -1.8505890992120e-009 ).
	//    if (fabs(cross) < VECTOR_SMALL)

	if (fabs(cross) < 1.e-8)
		return ((dot > 0) ? 0.0 : PI);
	else
		return (acos( dot ) * SGN( cross ));
}

bool
CAutoIndexRules::Comparable(
						const CGeoCurve&	partCurve,
						const CGeoCurve&	toolCurve,
						int					offsetDir,
						int					lineCount )
{
	bool compare = FALSE;

	if (partCurve.Type() == GEOLINE)
	{
		if (toolCurve.Type() == GEOLINE)
		{
			compare = TRUE;
		}
		else if (toolCurve.Type() == GEOARC && lineCount != 1)
		{
			// NOTE: A line count of one indicates a D-shaped tool in
			// which case the line takes precedence in dressing the part.

			const CGeoArc& tArc = dynamic_cast<const CGeoArc&>( toolCurve );
			if (tArc.IncludedAngle() > (PI + 0.001))
			{
				compare = TRUE;
			}
		}
	}
	else if (partCurve.Type() == GEOARC)
	{
		const CGeoArc& pArc = dynamic_cast<const CGeoArc&>( partCurve );

		if ( OutsideCut( &pArc, offsetDir ) )
		{
			if (toolCurve.Type() == GEOLINE)
				compare = TRUE;
			else if (toolCurve.Type() == GEOARC && lineCount == 0)
				compare = TRUE;
		}
		else
		{
			compare = (toolCurve.Type() == GEOARC);
		}
	}

	return compare;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For sorting the hier on increasing order of containment.
int
CRotPair::Compare( const void* ptrA, const void* ptrB )
{
	CRotPair* rotA = ( *(CRotPair**) ptrA);
	CRotPair* rotB = ( *(CRotPair**) ptrB);

	double radiansA = fabs( rotA->Radians() );
	double radiansB = fabs( rotB->Radians() );

	return ((radiansA - radiansB) > 0.0);
}

CAutoIndexRule::CAutoIndexRule(
						int				partIndx,
						int				toolIndx,
						double			toolRotation,
						const C3dVec&	partBackEdgeTrans,
						double			partBackEdgeRot,
						const C3dVec&	toolBackEdgeTrans,
						double			toolBackEdgeRot,
						const C3dVec&	partFrontEdgeTrans,
						double			partFrontEdgeRot,
						const C3dVec&	toolFrontEdgeTrans,
						double			toolFrontEdgeRot,
						bool			oversizedToolFace )

	: m_partIndx( partIndx ),
	  m_toolIndx( toolIndx ),
	  m_toolRotation( toolRotation ),
	  m_partBackEdgeTrans( partBackEdgeTrans ),
	  m_partBackEdgeRot( partBackEdgeRot ),
	  m_toolBackEdgeTrans( toolBackEdgeTrans ),
	  m_toolBackEdgeRot( toolBackEdgeRot ),
	  m_partFrontEdgeTrans( partFrontEdgeTrans ),
	  m_partFrontEdgeRot( partFrontEdgeRot ),
	  m_toolFrontEdgeTrans( toolFrontEdgeTrans ),
	  m_toolFrontEdgeRot( toolFrontEdgeRot ),
	  m_oversizedToolFace( oversizedToolFace )
{
}
