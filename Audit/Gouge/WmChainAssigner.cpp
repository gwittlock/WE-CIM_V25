
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"
#include "WmConst.h"

#include "GeoLine.h"
#include "GeoArc.h"
#include "WmChainIterator.h"
#include "WmChainAssigner.h"

#include "portable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

typedef struct
{
	char* id;
	EIntersectionType type;
} id_type_xref;

id_type_xref m_map[] =
{
	{ "adbc", Engage },     // common case
	{ "acbd", Disengage },  // common case
	{ "||||", Graze },      // common case
	{ "abdc", Graze },
	{ "abcd", Graze },
	{ "acdb", Graze },
	{ "adcb", Graze },
	{ "ab||", Graze },
	{ "||cd", Graze },
	{ "||dc", Graze },
	{ "a||b", Graze },
	{ "||bd", LeftExitSame },
	{ "||bc", LeftExitOpposite },
	{ "a||c", LeftEntrySame },
	{ "a||d", LeftEntryOpposite },
	{ "||db", RightExitSame },
	{ "||cb", RightExitOpposite },
	{ "ac||", RightEntrySame },
	{ "ad||", RightEntryOpposite },
	{     "", Undefined }
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChainAssigner::CWmChainAssigner()
{
	m_flags = ASSIGNER_NORMAL_USAGE;
}

CWmChainAssigner::~CWmChainAssigner()
{
	m_used.BenignFlush();
}

void
CWmChainAssigner::FlagsSet( int flags )
{
	m_flags = flags;
}

CReturn
CWmChainAssigner::InterferenceIndicesAssign( CWmChainList* chainList )
{
	CReturn				status;
	CWmChainIterator	iter;
	CWmChain*			chain;
	CWmSubchn*			subchnA;
	CWmSubchn*			subchnB;
	int					nChains, indx;
	int					setNo;

	setNo = 0;
	nChains = chainList->Count();

	for (indx = 0; indx < nChains; ++indx)
	{
		chain = (*chainList)[indx];

		if ( !Used( chain ) )
		{
			CWmChainList iSet;

			// Build the list of all immediate chains that intersect the
			// given chain.  The given chain will be included in the set.
			IntersectionSetBuild( chain, ++setNo, &iSet );

			// While looping over each chain in the set, assign interference
			// indices at the points of intersection and then remove the
			// chain from further consideration.
			int iCount = iSet.Count();

			for (int jndx = 0; jndx < iCount; ++jndx)
			{
				CWmChain* iChain = iSet[jndx];

				status = InterferenceIndexAssign( iChain );

				if ( !status.IsOk() )
					break;  // BAD problem!

				Mark( iChain );
			}
		}
	}

	// TODO: Find a better way to get closure on
	// the unassigned interference indices.

	for (indx = 0; indx < nChains; ++indx)
	{
		chain = (*chainList)[indx];

		if ( chain->IsClosed() )
		{
			iter.Init( (*chain) );

			iter.GotoEnd();
			subchnB = iter.Subchn();  // ie. the terminal subchn.
			if (subchnB != NULL && !subchnB->IsAssigned()) // which should never be assigned(?)
			{
				iter.PrevSubchn();
				subchnA = iter.Subchn();  // had better be assigned!

				/*  V16.5 forego assigning to terminal subchn.
				if (subchnA != NULL && subchnA->IsAssigned())
				{
					subchnB->Iindex( subchnA->Iindex() );
				}
				*/

				iter.GotoStart();
				subchnB = iter.Subchn();
				if (subchnB != NULL && !subchnB->IsAssigned())
				{
					// Because the end of the chain is connected to the start ...
					subchnB->Iindex( subchnA->Iindex() );
				}
			}
		}
	}

	return status;
}

void
CWmChainAssigner::IntersectionSetBuild( CWmChain* chain, int setNo, CWmChainList* iSet )
{
	CWmChainIterator iter;
	CWmChain*	candidate;
	int	indx;

	chain->SetNo( setNo );
	iSet->Append( (CWmChain*) chain );

	indx = 0;
	while (indx < iSet->Count())
	{
		candidate = (*iSet)[indx];

		iter.Init( (*candidate) );

		while (1)
		{
			if ( iter.AtEnd() )
				break;

			CWmSubchn* subchn = iter.Subchn();

			CWmSubchn* other = subchn->Other();

			if (other != NULL)
			{
				CWmChain* owner = other->Owner();

				if (owner != NULL)
				{
					owner->SetNo( setNo );
					iSet->ConditionalAppend( owner );
				}
			}

			iter.NextSubchn();
		}

		++indx;
	}

	return;
}

bool
CWmChainAssigner::Used( const CWmChain* chain )
{
	int count = m_used.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		if (chain == m_used[indx])
			return TRUE;
	}

	return FALSE;
}

void
CWmChainAssigner::Mark( const CWmChain* chain )
{
	m_used.Append( (CWmChain*) chain );
}

CReturn
CWmChainAssigner::InterferenceIndexAssign( CWmChain* chain )
{
	CReturn status;

	CWmSubchn* start = NULL;
	CWmSubchn* subchn = NULL;
	int iindex;

	CWmChainIterator iter( (*chain) );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Determine where to start the traversal process.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	FirstAssignedFind( &iter );

	iindex = 0;
	if ( iter.AtEnd() )
	{
		// This chain hasn't been processed.
		iter.GotoStart();
		subchn = iter.Subchn();
		subchn->Iindex( iindex );
	}
	else
	{
		subchn = iter.Subchn();
		iindex = subchn->Iindex();
	}

	start = subchn;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Traverse the chain, assigning interference
	//      indices at each intersection.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	while (1)
	{
		iter.NextSubchn();

		if ( iter.AtEnd() )
			iter.GotoStart();

		subchn = iter.Subchn();

		if ( !subchn->IsAssigned() )
			status = InterferenceIndexAssign( subchn, iindex );

		if ( !status.IsOk() )
			break;

		if (subchn == start)
			break;  // We've come full circle.

		iindex = subchn->Iindex();
	}

	return status;
}

CReturn
CWmChainAssigner::InterferenceIndexAssign( CWmSubchn* subchn, int iindex )
{
	CReturn status;

	int count = subchn->Count();

	if (count == 0)
	{
		if (m_flags & ASSIGNER_PROPOGATE_IINDEX)
		{
			// 2008.01.05 (PE) -- Upon changing CTrueRemnant::PartsRemove()
			// to use CWmChainDegouger instead of the boolean operations
			// of CGeoPoly, I encountered incomplete interior chains.
			CWmChain* owner = subchn->Owner();
			if ((subchn == owner->First()) || (subchn == owner->Last()))
				subchn->Iindex( iindex );
		}
		else
		{
			// Original behavior.
			// Nothing to do because we hould be at start/end of chain.
		}
	}
	else if (count == 2)
	{
		// A trivial case (hopefully).
		status = SimpleIindexAssign( subchn, iindex );
	}
	else if ((count % 2) == 0)
	{
		// We have multiple curves intersecting at the same point.
		status = ComplexIindexAssign( subchn, iindex );
	}
	else
	{
		// An odd number of intersections means one of:
		// 1. We have encountered the dreaded 'exact tie' case
		//    for which we don't yet have a solution.
		// 2. The intersection is at the end point of the chain,
		//    in which case we can disregard the intersection (?)

		// status.Internal( IDS_INTERNAL_ERROR, "CWmChainAssigner::InterferenceIndexAssign()" );
		status = STATUS_WARNING;
	}

	return status;
}

CReturn
CWmChainAssigner::SimpleIindexAssign( CWmSubchn* subchn, int iindex )
{
	C2dUnitVec aTan, bTan, cTan, dTan;
	const char* tStatus = "";
	CReturn status;

	CWmSubchn* other = subchn->Sister();

	EIntersectionType type = Undefined;
	if (subchn != NULL && other != NULL)
	{
		tStatus = TangentsGet( subchn, other, &aTan, &bTan, &cTan, &dTan );

		type = Graze;
		if (strcmp( tStatus, "ABCD" ) == 0)
			type = IntersectionClassify( aTan, bTan, cTan, dTan );
	}

	// TODO: Required for ZigZag pocketing.
	// if (gifFunc != NULL)
	//	type = gifFunc(type);

	switch (type)
	{
	case Engage:
		subchn->Iindex( ++iindex );
		other->Iindex( --iindex );
		break;
	case Disengage:
		subchn->Iindex( --iindex );
		other->Iindex( ++iindex );
		break;
	case Graze:
		subchn->Iindex( iindex );
		other->Iindex( iindex );
		break;
	default:
		// TODO: We have encountered a non-trivial case that requires
		// further resolution.  This case most likely represents
		// one of:
		//
		// 1. The dreaded 'exact tie' case for which we don't yet have a solution.
		// 2. The intersection is at the end point of the chain, in which case
		//    we can disregard the intersection (?)

		// Resolve the amibguity.
		// resolve(type, iindex, node0);
#if BEFORE_V16
		status = STATUS_WARNING;
#else
		if (subchn != NULL)
			subchn->Iindex( iindex );

		if (other != NULL)
			other->Iindex( iindex );
#endif
		break;
	}

	return status;
}

CReturn
CWmChainAssigner::ComplexIindexAssign( CWmSubchn* subchn, int iindex )
{
	CReturn status;
	status.Internal( IDS_INTERNAL_ERROR, "CWmChainAssigner::ComplexIindexAssign()" );
	return status;
}

void
CWmChainAssigner::FirstAssignedFind( CWmChainIterator* iter )
{
	iter->GotoStart();

	CWmSubchn* subchn = NULL;

	while (1)
	{
		subchn = iter->Subchn();

		if ( iter->AtEnd() )
			break;  // Didn't find one.

		if ( subchn->IsAssigned() )
			break;

		iter->NextSubchn();
	}

}

const char*
CWmChainAssigner::TangentsGet( CWmSubchn* subchnA, CWmSubchn* subchnB,
	C2dUnitVec* aTan, C2dUnitVec* bTan, C2dUnitVec* cTan, C2dUnitVec* dTan )
{
	static char status[5];

	// Get the elements such that:
	//   ---a---> * ---b--->  where * is node0
	//   ---c---> * ---d--->  where * is node1

	CWmElem* elemA = dynamic_cast<CWmElem*>( subchnA->Prev() );
	CWmElem* elemB = dynamic_cast<CWmElem*>( subchnA->Next() );

	CWmElem* elemC = dynamic_cast<CWmElem*>( subchnB->Prev() ); 
	CWmElem* elemD = dynamic_cast<CWmElem*>( subchnB->Next() );

	// Return the tangents such that:
	//   <---a--- * ---b--->
	//   <---c--- * ---d--->

	status[0] = ((elemA == NULL) ? '-' : 'A');
	status[1] = ((elemB == NULL) ? '-' : 'B');
	status[2] = ((elemC == NULL) ? '-' : 'C');
	status[3] = ((elemD == NULL) ? '-' : 'D');
	status[4] = '\0';

	if (strcmp( status, "ABCD" ) == 0)
	{
		*aTan = TangentGet( elemA, TRUE );
		*bTan = TangentGet( elemB, FALSE );
		*cTan = TangentGet( elemC, TRUE );
		*dTan = TangentGet( elemD, FALSE );
	}
	return status;
}

C2dUnitVec
CWmChainAssigner::TangentGet( CWmElem* node, bool entry )
{
	static double FACTOR = 1.5e-4;
	static double DELTA = FACTOR * DEG2RAD;

	CGeoCurve* curve = dynamic_cast<CGeoCurve*>( node->Elem() );

	ASSERT( (curve != NULL) );  // Can only process curves (like duh!)

	C2dUnitVec tan = ((entry) ? curve->EndTan() : curve->StartTan());

	CGeoArc* arc = dynamic_cast<CGeoArc*>( curve );

	if (arc != NULL)
	{
		// Arcs can be very problematic in that they can have
		// a tangent intersection with another entity. Skewing
		// the arcs tangent vectors at the intersection helps
		// us to classify the intersection point.
		//
		//    See also CWmChainAssigner::IntersectionClassify().

		double adjust;

		if (arc->Dir() > 0)
			adjust = ((entry) ? -1 : 1) * DELTA;
		else
			adjust = ((entry) ? 1 : -1) * DELTA;

		tan += adjust;
	}

	if ( entry )
		tan += PI;

	return tan;
}

EIntersectionType
CWmChainAssigner::IntersectionClassify(
							C2dUnitVec& aTan,
							C2dUnitVec& bTan,
							C2dUnitVec& cTan,
							C2dUnitVec& dTan )
{
	double angle[4];
	double delta;
	char id[5];

	strcpy_s(id, 5, "abcd");

	angle[0] = aTan.Radians();
	angle[1] = bTan.Radians();
	angle[2] = cTan.Radians();
	angle[3] = dTan.Radians();

	delta = angle[0] - angle[1];

	if (angle[1] < angle[0] && delta > SMALL)
		angle[1] += TWOPI;

	delta = angle[0] - angle[2];

	if (angle[2] < angle[0] && delta > SMALL)
		angle[2] += TWOPI;

	delta = angle[0] - angle[3];

	if (angle[3] < angle[0] && delta > SMALL)
		angle[3] += TWOPI;

	for (int i = 0; i < 3; i++)
	{
		for (int j = (i + 1); j < 4; j++)
		{
			double delta = angle[j] - angle[i];

			if (delta > VECTOR_SMALL)
				continue;

			if (fabs(delta) < VECTOR_SMALL)
			{
				id[i] = '|';
				id[j] = '|';
			}

			double dtmp = angle[i];
			angle[i] = angle[j];
			angle[j] = dtmp;

			char ctmp = id[i];
			id[i] = id[j];
			id[j] = ctmp;
		}
	}
	
	int index = 0;
	while (1)
	{
		if (m_map[index].type == Undefined)
			break;

		if (strcmp(id, m_map[index].id) == 0)
			return m_map[index].type;

		++index;
	}

	return Graze;
}

#if ZIGZAG
static intsct_type
gifFunc( intsct_type type )
{
	switch (type)
	{
	case LeftExitSame  : return Disengage;
	case LeftEntrySame : return Engage;
	case RightExitSame : return Engage;
	case RightEntrySame: return Disengage;
	default            : return type;
	}
}
#endif
