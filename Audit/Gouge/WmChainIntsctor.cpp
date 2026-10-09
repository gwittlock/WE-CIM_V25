
#include "stdafx.h"
#include <math.h>

#include "Return.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "WmSubchn.h"
#include "WmChainIterator.h"
#include "Int2d.h"
#include "WmCrvRec.h"
#include "WmChainIntsctor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChainIntsctor::CWmChainIntsctor()

	: m_ints(),
	  m_crvs()
{

}

CWmChainIntsctor::~CWmChainIntsctor()
{
	m_ints.DestructiveFlush();
	m_crvs.DestructiveFlush();
}

bool
CWmChainIntsctor::Intersect( const CWmChain& chainA, const CWmChain& chainB, double tol )
{
	CWmChainIterator iterA;
	CWmChainIterator iterB;
	bool does_intersect;

	does_intersect = false;

	iterA.Init( chainA );
	iterB.Init( chainB );

	while ( !iterA.AtEnd() )
	{
		while ( !iterB.AtEnd() )
		{
			if (Intersect( iterA.Elem(), iterB.Elem(), tol ) == true)
				does_intersect = true;

			iterB.NextElem();
		}

		iterB.GotoStart();

		iterA.NextElem();
	}

	return does_intersect;
}

bool
CWmChainIntsctor::Intersect( CWmElem* nodeA, CWmElem* nodeB, double tol )
{
	if ( !OkayToIntersect( nodeA, nodeB ) )
		return false;

	C2dCoord pt[2];
	double uA[2];
	double uB[2];

	int count = 0;

	CGeoLine* lineA = dynamic_cast<CGeoLine*>( nodeA->Elem() );
	CGeoLine* lineB = dynamic_cast<CGeoLine*>( nodeB->Elem() );

	CGeoArc* arcA = dynamic_cast<CGeoArc*>( nodeA->Elem() );
	CGeoArc* arcB = dynamic_cast<CGeoArc*>( nodeB->Elem() );

	if (lineA != NULL)
	{
		if (lineB != NULL)
		{
			Int2dSegSeg( (*lineA), (*lineB), tol, VECTOR_SMALL, TRUE, pt, uA, uB, &count );
		}
		else if (arcB != NULL)
		{
			Int2dArcSeg( (*arcB), (*lineA), tol, TRUE, pt, uB, uA, &count );
		}
	}
	else if (arcA != NULL)
	{
		if (lineB != NULL)
		{
			Int2dArcSeg( (*arcA), (*lineB), tol, TRUE, pt, uA, uB, &count );
		}
		else if (arcB != NULL)
		{
			Int2dArcArc( (*arcA), (*arcB), tol, TRUE, pt, uA, uB, &count );
		}
	}

	CWmIntRec* intRec;
	CWmCrvRec* crvRecA;
	CWmCrvRec* crvRecB;
	CWmCrvRecExtData* crvExtA;
	CWmCrvRecExtData* crvExtB;
	for (int indx = 0; indx < count; ++indx)
	{
		intRec = FindCreate( pt[indx], tol );

		crvRecA = FindCreate( nodeA );
		crvRecB = FindCreate( nodeB );

		crvExtA = crvRecA->Update( intRec, uA[indx] );
		crvExtB = crvRecB->Update( intRec, uB[indx] );

		intRec->Update( crvExtA );
		intRec->Update( crvExtB );
	}

	return (count > 0);
}

CWmIntRec*
CWmChainIntsctor::FindCreate( const C2dCoord& pt, double tol )
{
	CWmIntRec* theRec = NULL;
	
	int indx, count = m_ints.Count();

	for (indx = 0; indx < count; ++indx)
	{
		theRec = m_ints[indx];

		if ( pt.WithinTol( theRec->Point(), tol ) )
			break;
	}

	if (indx >= count)
	{
		theRec = new CWmIntRec( pt );
		m_ints.Append( theRec );
	}

	return theRec;
}

CWmCrvRec*
CWmChainIntsctor::FindCreate( CWmElem* node )
{
	CWmCrvRec* theRec = NULL;
	
	int indx, count = m_crvs.Count();

	for (indx = 0; indx < count; ++indx)
	{
		theRec = m_crvs[indx];

		if (theRec->WmElem() == node)
			break;
	}

	if (indx >= count)
	{
		theRec = new CWmCrvRec( node );
		m_crvs.Append( theRec );
	}

	return theRec;
}

bool
CWmChainIntsctor::OkayToIntersect( const CWmElem* nodeA, const CWmElem* nodeB )
{
	if (nodeA == nodeB)
		return FALSE;  // can't intersect an element with itself

	if (dynamic_cast<CWmElem*>( nodeA->Next() ) == nodeB)
	{
		// Adjacent entities.
		C2dCoord pe = nodeA->Elem()->EndPt();
		C2dCoord ps = nodeB->Elem()->StartPt();

		return ( !pe.WithinTol( ps, SMALL ) );
	}
	else if (dynamic_cast<CWmElem*>( nodeA->Prev() ) == nodeB)
	{
		// Adjacent entities.
		C2dCoord ps = nodeB->Elem()->EndPt();
		C2dCoord pe = nodeA->Elem()->StartPt();

		return ( !pe.WithinTol( ps, SMALL ) );
	}

	return TRUE;
}

void
CWmChainIntsctor::Debug( const char* caption ) const
{
	CReturn trace;
	CString msg;
	CString addr;
	C2dCoord pt;
	const CWmIntRec* intRec;
	const CWmCrvRec* crvRec;
	int icnt, indx;
	int jcnt, jndx;

	if (caption != NULL)
		trace.Diagnostic( caption );

	trace.Diagnostic( "=-=-=-=-=-=-= intersections =-=-=-=-=-=-=" );
	icnt = m_ints.Count();
	for (indx = 0; indx < icnt; ++indx)
	{
		intRec = m_ints[indx];

		pt = intRec->Point();
		jcnt = intRec->Count();

		msg.Format( "%d) this <0x%x>   x=%10.6f y=%10.6f   count=%d",
						indx, intRec, pt.X(), pt.Y(), jcnt );
		trace.Diagnostic( msg );

		msg = "    ";
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			addr.Format( " crvext <0x%x>", intRec->ExtData(jndx) );
			msg += addr;
		}

		trace.Diagnostic( msg );
	}

	trace.Diagnostic( "\n" );
	trace.Diagnostic( "=-=-=-=-=-=-= curves =-=-=-=-=-=-=" );
	icnt = m_crvs.Count();
	for (indx = 0; indx < icnt; ++indx)
	{
		crvRec = m_crvs[indx];

		jcnt = crvRec->Count();

		msg.Format( "%d) this <0x%x>   count=%d", indx, crvRec, jcnt );
		trace.Diagnostic( msg );

		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			const CWmCrvRecExtData* ext = crvRec->ExtData( jndx );

			msg.Format( "    this <0x%x> uparam=%10.6f intrec <0x%x> subchn <0x%x>",
						  ext, ext->Uparam(), ext->IntRec(), ext->Subchn() );

			trace.Diagnostic( msg );
		}
	}
}

void
CWmChainIntsctor::Finalize()
{
	CWmCrvRec* crvrec;
	CWmCrvRecExtData* ext;

	for (int indx = 0; indx < m_crvs.Count(); ++indx)
	{
		crvrec = m_crvs[indx];

		ext = crvrec->ExtData(0);

		if (fabs( ext->Uparam() ) > SMALL)
			crvrec->Update( NULL, 0.0 );

		ext = crvrec->ExtData( crvrec->Count() - 1 );

		if ((1.0 - fabs( ext->Uparam() )) > SMALL)
			crvrec->Update( NULL, 1.0 );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Remove redundant intersection records, otherwise we'll get
// a ComplexIindexAssign() error.
//
// The need for this arises when the common end point of two
// adjacent curves is an intersection point with another curve.
//
//				|B
//			A   |
//			----*----
//				|   C
//			   D|
//
// In the above case, the intersection record will reverence
// four curves (A,B,C,D).  As the end point of curve A is also
// the start point of curve C, we can remove one of the curves
// from the intersection record.  In fact, this is critical to
// the success of interference index assignment, which considers
// the number of curves running through an intersection point.
//
// An intersection point that is associated with two curves is
// considered as a 'simple intersection' and is therefore easy
// to resolve.  An intersection point that is associated with
// more than 2 curves is considered as a 'complex intersection',
// and though this case can be resolved, CWmChainAssigner does
// not yet handle this case.
void
CWmChainIntsctor::Reduce()
{
	int count = m_ints.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CWmIntRec* intrec = m_ints[indx];
		intrec->Reduce();
	}
}
