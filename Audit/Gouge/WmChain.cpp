
#include "stdafx.h"
#include <math.h>

#include "MathConst.h"
#include "Return.h"
#include "Register.h"

#include "2dVec.h"
#include "3dBox.h"
#include "GeoElem.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "GeoPoly.h"
#include "Int2d.h"
#include "Solution.h"
#include "WmElem.h"
#include "WmChain.h"

#include "WmChainIterator.h"
#include "WmChainDegouger.h"

static void AdjustCurveEndPoints( CGeoCurve* curveA, CGeoCurve* curveB );

// 2009.07.03 (PE) -- An *arbitrary* tolerance chosen to reflect
// nearly tangent vectors. See also notes below of same date.
#define TANGENT_LIMIT 0.99999

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// To be used in the event the IDE's QuickWatch returns the
// message "function not present" when you try to evaluate
// something like 'poly->Draw()'.
static void ChainDraw( CWmChain* chain )
	{ chain->Draw(); }


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChain::CWmChain()

	: m_head( NULL ),
	  m_tail( NULL ),
	  m_attribs( NULL ),
	  m_setNo( 0 )
{
	CondInit();

	m_propogate = false;
}

CWmChain::CWmChain( const CWmChain& chain )

	: m_head( NULL ),
	  m_tail( NULL ),
	  m_attribs( NULL ),
	  m_setNo( 0 )
{
	CWmChainIterator iter( chain );

	CondInit();

	m_propogate = false;

	CopyAppend( chain );
}

CWmChain::~CWmChain()
{
	Reinit();
}

void
CWmChain::Reinit()
{
	CWmNode* curr = m_head;

	if (m_attribs != NULL)
	{
		delete m_attribs;
		m_attribs = NULL;
	}

	while (curr != NULL)
	{
		CWmNode* next = curr->Next();

 		curr->Unlink();
		delete curr;

		curr = next;
	}

	m_head = NULL;
	m_tail = NULL;
	m_setNo = 0;
}

bool
CWmChain::IsClosed() const
{
	ASSERT( (m_head != NULL) );

	C3dCoord start = CWmChain::StartPt();
	C3dCoord end = CWmChain::EndPt();

	C2dVec vec = (end - start);

	double dist = vec.Length();

	return (dist < SMALL);
}

int
CWmChain::Count() const
{
	CWmChainIterator iter( (*this) );

	int count = 0;
	while ( !iter.AtEnd() )
	{
		++count;
		iter.NextElem();
	}

	return count;
}

C3dBox
CWmChain::Box() const
{
	CWmChainIterator	iter;
	C3dBox				world;
	CWmElem*			wmelem;

	iter.Init( (*this) );
	while ( !iter.AtEnd() )
	{
		wmelem = iter.Elem();

		if (wmelem != NULL)
		{
			const C3dBox& box = wmelem->Elem()->Box();
			if (box.IsDefined())
				world += box;
		}

		iter.NextElem();
	}

	return world;
}

int
CWmChain::SetNo() const
{
	return m_setNo;
}

void
CWmChain::SetNo( int setNo )
{
	m_setNo = setNo;
}

C3dCoord
CWmChain::StartPt() const
{
	C3dCoord ps;

	CWmElem* wmelem = dynamic_cast<CWmElem*>( m_head->Next() );
	if (wmelem != NULL)
		ps = wmelem->Elem()->StartPt();

	return ps;
}

C3dCoord
CWmChain::EndPt() const
{
	C3dCoord pe;

	CWmElem* wmelem = dynamic_cast<CWmElem*>( m_tail->Prev() );
	if (wmelem != NULL)
		pe = wmelem->Elem()->EndPt();

	return pe;
}

void
CWmChain::Append( CGeoElem* elem )
{
	ASSERT( (elem != NULL) );

	CondInit();

	CWmElem* wmelem = new CWmElem( m_tail->SubchnPrev(), elem );
	m_tail->Prev( wmelem );
}

void
CWmChain::CopyAppend( const CGeoElem& elem )
{
	CGeoElem* copy = elem.Clone( m_propogate );

	Append( copy );
}

void
CWmChain::CopyAppend( const CWmChain& chain )
{
	CWmChainIterator iter( chain );

	while ( !iter.AtEnd() )
	{
		CWmElem* elem = iter.Elem();
		CGeoElem* geo = elem->Elem();

		CopyAppend( (*geo) );

		iter.NextElem();
	}
}

void
CWmChain::Prepend( CGeoElem* elem )
{
	ASSERT( (elem != NULL) );

	CondInit();

	CWmElem* wmelem = new CWmElem( m_head, elem );
	m_head->Next( wmelem );
}

void
CWmChain::CopyPrepend( const CGeoElem& elem )
{
	CGeoElem* copy = elem.Clone( m_propogate );

	Prepend( copy );
}

void
CWmChain::CopyAppend( const CWmSubchn& subchn )
{
	CWmElem* node = dynamic_cast<CWmElem*>( subchn.Next() );

	if (node == NULL)
		return;  // Shouldn't happen, but ....

	CWmSubchn* owner = node->Owner();

	while (1)
	{
		if (node == NULL)
			break;

		if (node->Owner() != owner)
			break;

		CGeoElem* elem = node->Elem();
		CopyAppend( (*elem) );

		node = dynamic_cast<CWmElem*>( node->Next() );
	}
}

// NOTE: When checkForClosure is set to false, a single 'open curve'
// is considered here as C0 continuous.
bool
CWmChain::IsC0Continuous( double tol, bool checkForClosure )
{
	C2dCoord ptA;
	C2dCoord ptB;
	CWmElem* nodeA;
	CWmElem* nodeB;
	CGeoElem* elemA;
	CGeoElem* elemB;

	CWmChainIterator iter( (*this) );

	//=-=-=-=-=-= empty chain =-=-=-=-=-=

	nodeA = iter.Elem();
	if (nodeA == NULL)
		return FALSE;

	//=-=-=-=-=-= single curve chain =-=-=-=-=-=

	iter.NextElem();
	nodeB = iter.Elem();
	if (nodeB == NULL)
	{
		// We have a single entity chain.
		CGeoArc* arc = dynamic_cast<CGeoArc*>( nodeA->Elem() );
		if (arc == NULL)
		{
			// Assume that we have a line.
			return ((checkForClosure) ? FALSE : TRUE);
		}
		else
		{
			if ( checkForClosure )
			{
				ptA = arc->StartPt();
				ptB = arc->EndPt();
				return ( ptA.WithinTol( ptB, tol ) );
			}
			else
				return TRUE;
		}
	}

	//=-=-=-=-=-= multiple curve chain =-=-=-=-=-=

	while (nodeB != NULL)
	{
		elemA = nodeA->Elem();
		elemB = nodeB->Elem();

		ptA = elemA->EndPt();
		ptB = elemB->StartPt();

		if ( !ptA.WithinTol( ptB, tol ) )
			return FALSE;  // We found a break in C0 continuity.

		nodeA = nodeB;

		iter.NextElem();
		nodeB = iter.Elem();
	}

	if ( checkForClosure )
	{
		iter.GotoStart();
		nodeB = iter.Elem();

		elemA = nodeA->Elem();
		elemB = nodeB->Elem();

		if ( !ptA.WithinTol( ptB, tol ) )
			return FALSE;  // We found a break in C0 continuity.
	}

	return TRUE;
}

void
CWmChain::C0ContinuousCopyAppend( const CWmSubchn& subchn, double tol )
{
	CWmElem* node = dynamic_cast<CWmElem*>( subchn.Next() );

	if (node == NULL)
		return;  // Shouldn't happen, but ....

	CGeoElem* prev = NULL;
	CGeoElem* curr = NULL;
	CWmSubchn* owner = node->Owner();

	while (1)
	{
		if (node == NULL)
			break;

		if (node->Owner() != owner)
			break;

		curr = node->Elem();

		if (prev == NULL)
		{
			CopyAppend( (*curr) );
		}
		else
		{
			const C2dCoord pe = prev->EndPt();
			const C2dCoord ps = curr->StartPt();
			if ( ps.WithinTol( pe, tol ) )
				CopyAppend( (*curr) );
		}

		prev = curr;

		node = dynamic_cast<CWmElem*>( node->Next() );
	}
}

void
CWmChain::Reverse()
{
	ASSERT( (m_head->SubchnNext() == m_tail) );  // We can only process 'virgin' chains.

	CDynamicArray<CWmElem*> array;
	CWmElem* wmelem;
	CGeoCurve* curve;
	int count, indx;

	while (1)
	{
		wmelem = dynamic_cast<CWmElem*>( m_head->Next() );
		if (wmelem == NULL)
			break;

		curve = dynamic_cast<CGeoCurve*>( wmelem->Elem() );
		if (curve != NULL)
			curve->Reverse();

		wmelem->Unlink();

		array.Append( wmelem );
	}

	count = array.Count();
	for (indx = count - 1; indx >= 0; --indx)
	{
		wmelem = dynamic_cast<CWmElem*>( array[indx] );

		m_tail->Prev( wmelem );
		wmelem->Owner( m_head );
	}
}

void
CWmChain::Xform( const C3x4Matrix& xform )
{
	CWmChainIterator	iter;
	CWmElem*			wmelem;

	iter.Init( (*this) );
	while ( !iter.AtEnd() )
	{
		wmelem = iter.Elem();

		if (wmelem != NULL)
		{
			wmelem->Elem()->Xform( xform );
		}

		iter.NextElem();
	}
}

CReturn
CWmChain::Offset( 
	int				offsetDir, 
	double			offsetAmt, 
	double			sharpAngle, 
	CWmChainList*	results ) const
{
	CReturn status;

#ifdef _DEBUG
	// CGeoLine::Debug( FALSE );
#endif

	if (offsetDir != 0 && fabs(offsetAmt) >= SMALL)
	{
		CWmChain raw;

		raw.AttribsPropogate( true );
		RawOffset( offsetDir, offsetAmt, sharpAngle, &raw );

		if (offsetDir > 0)
		{
			// We're offsetting to the left.  As the degouging
			// algorithm is designed for pocketing closed chains,
			// it assumes the chain has a clockwise orientation
			// and that the offset is to the right.  By reversing
			// the chain, we fool the degouger.
			raw.Reverse();
		}

		status = raw.Degouge( offsetDir, results );

		// 2006.08.26 (PE) -- Using Dynatorch "yard art" type parts,
		// small open offset profiles were being generated from a
		// closed profile. This happens primarily because the offset
		// is large compared with the entity arc length.
		if ( CWmChain::IsClosed() )
		{
			int indx = 0;
			while (indx < results->Count())
			{
				CWmChain* chain = results->GetAt( indx );

				if ( !chain->IsC0Continuous( SMALL, TRUE ) )
				{
					if (0)
						chain->Dump();

					delete results->Remove( indx );
				}
				else
				{
					++indx;
				}
			}
		}
	}
	else
	{
		CWmChain* copy = new CWmChain( (*this) );
		results->Append( copy );
	}

#ifdef _DEBUG
	// CGeoLine::Debug( FALSE );
#endif

	return status;
}

CReturn
CWmChain::Degouge( int offsetDir, CWmChainList* degougedChains )
{
	CReturn status;

	int count = Count();

	if (offsetDir != 0 && count > 1)
	{
		CWmChainDegouger degouger;
		
		degouger.Init( this, SMALL );

		count = degouger.Count();

		for (int indx = 0; indx < count; ++indx)
		{
			CWmChain* theChain = degouger.Results().Remove( 0 );

			if (offsetDir > 0)
			{
				// Set things straight.  See previous comment.
				theChain->Reverse();
			}

			if ( IsAbhorrentChain( (*theChain) ) )
			{
				delete theChain;
				theChain = NULL;
			}

			if (theChain != NULL)
				degougedChains->Append( theChain );
		}
	}
	else if (count > 0)
	{
		// No need to degouge because we have either
		// 1) a single line/arc
		// 2) no offset direction
		//
		// ALSO, use this branch if you want to see
		// the raw input state of this chain.  In other
		// words, use this branch to bypass the degouger.

		CWmChain* single = new CWmChain( (*this) );

		if (offsetDir > 0)
		{
			// Set things straight.  See previous comment.
			single->Reverse();
		}

		degougedChains->Append( single );
	}

	return status;
}

// (ew) You know, CProfile has an Area function, too...
// (pe) Yea, but ...
double
CWmChain::Area() const
{
	CWmChainIterator iter( (*this) );

	CWmElem* wmelem = iter.Elem();
	if (wmelem == NULL)
		return 0.0;


	CGeoArc* arc = NULL;

	if (dynamic_cast<CWmElem*>( wmelem->Next() ) == NULL && IsClosed())
	{
		// Special case of single 360 arc.
		arc = dynamic_cast<CGeoArc*>( wmelem->Elem() );
		double radius = arc->Radius();

		return (arc->Dir() * PI * radius * radius);
	}

	double area, rad, angle;
	double totalArea = 0.0;

	while ( !iter.AtEnd() )
	{
		CGeoCurve* curve = dynamic_cast<CGeoCurve*>( iter.Elem()->Elem() );

		if (curve != NULL)
		{
			const C2dCoord ptA = curve->StartPt();
			const C2dCoord ptB = curve->EndPt();

			switch (curve->Type())
			{
			case GEOLINE:
				area = (ptA.X() * ptB.Y()) - (ptB.X() * ptA.Y());
				totalArea += area;
				break;

			case GEOARC:
				arc = (CGeoArc*) curve;

				// Triangular area, across arc's chord
				area = (ptA.X() * ptB.Y()) - (ptB.X() * ptA.Y());
				totalArea += area;

				// Adjust for curved portion...
				rad   = arc->Radius();
				angle = arc->IncludedAngle();
				area  = rad * rad * (angle - sin(angle));

				totalArea = totalArea + (arc->Dir() * area);
				break;
			}
		}

		iter.NextElem();
	}

	return totalArea / 2.0;
}

CWmSubchn*
CWmChain::First() const
{
	return m_head;
}

CWmSubchn*
CWmChain::Last() const
{
	return m_tail;
}

void
CWmChain::Debug( const char* caption ) const
{
	if (!CRegister::Debug("WmChain"))
		return;

	CReturn trace;
	CString msg;
	C2dCoord ps;
	C2dCoord pe;
	C2dCoord pc;

	if (caption != NULL)
		trace.Diagnostic( caption );

	msg.Format( "=-=-=-=-=-=-= chain <0x%x> =-=-=-=-=-=-=", this );
	trace.Diagnostic( msg );

	CWmNode* curr = m_head;
	while (1)
	{
		if (curr == NULL)
			break;

		CWmSubchn* subchn = dynamic_cast<CWmSubchn*>( curr );
		CWmElem* wmelem = dynamic_cast<CWmElem*>( curr );

		if (subchn != NULL)
		{
			msg.Format( "subchn <0x%x>, index <%d>, prev <0x%x>, next <0x%x>",
				subchn, subchn->Iindex(), subchn->SubchnPrev(), subchn->SubchnNext() );
			trace.Diagnostic( msg );
		}
		else if (wmelem != NULL)
		{
			CGeoLine* line = dynamic_cast<CGeoLine*>( wmelem->Elem() );
			CGeoArc* arc = dynamic_cast<CGeoArc*>( wmelem->Elem() );

			if (line != NULL)
			{
				ps = line->StartPt();
				pe = line->EndPt();
				msg.Format( "  line <0x%x>: xs <%9.6f> ys <%9.6f> xe <%9.6f> ye <%9.6f>",
					wmelem, ps.X(), ps.Y(), pe.X(), pe.Y() );
				trace.Diagnostic( msg );

				if ( line->HasAttrib() )
				{
					int count = line->Attrib().countVar();
					for (int indx = 0; indx < count; ++indx)
					{
						CVar* attrib = line->Attrib().getVar(indx);
						msg.Format( "      name <%s> val <%s>",
							attrib->getName(), attrib->getString() );
						trace.Diagnostic( msg );
					}
				}
			}
			else if (arc != NULL)
			{
				ps = arc->StartPt();
				pe = arc->EndPt();
				pc = arc->CenterPt();
				msg.Format( "  arc <0x%x>: xs <%9.6f> ys <%9.6f> xe <%9.6f> ye <%9.6f> xc <%9.6f> yc <%9.6f> rad <%9.6f> dir<%d>",
					wmelem, ps.X(), ps.Y(), pe.X(), pe.Y(), pc.X(), pc.Y(), arc->Radius(), arc->Dir() );
				trace.Diagnostic( msg );

				if ( arc->HasAttrib() )
				{
					int count = arc->Attrib().countVar();
					for (int indx = 0; indx < count; ++indx)
					{
						CVar* attrib = arc->Attrib().getVar(indx);
						msg.Format( "      name <%s> val <%s>",
							attrib->getName(), attrib->getString() );
						trace.Diagnostic( msg );
					}
				}
			}
		}

		curr = curr->Next();
	}
}

CReturn
CWmChain::RawOffset(
			int			offsetDir,
			double		offsetAmt,
			double		sharpAngle,
			CWmChain*	result ) const
{
	static double GOODENOUGH = 1.e-5;

	CWmChainIterator iter( (*this) );
	CReturn status;

	C2dUnitVec tanA;
	C2dUnitVec tanB;
	C2dUnitVec normA;
	C2dUnitVec normB;
	C2dCoord ps;
	C2dCoord pe;
	C2dCoord pc;
	C2dCoord pt;
	double cross, dist, dx, dy;
	double dot = 0.;
	bool blend;
	bool trmext;
	bool trim;
	bool gap;
	bool owned;

	CGeoCurve* curveA  = NULL;
	CGeoCurve* curveB  = NULL;
	CGeoCurve* offsetA = NULL;
	CGeoCurve* offsetB = NULL;

	double limit = cos( PI - sharpAngle ) - SMALL;

	// TODO: Crashes when empty chain.  How does that happen?
	curveA = dynamic_cast<CGeoCurve*>( iter.Elem()->Elem() );

	offsetA = curveA->Offset( offsetDir, offsetAmt );
	if (offsetA != NULL)
		result->Append( offsetA );

	iter.NextElem();

	if ( iter.AtEnd() )
		return status;  // Single entity chain.


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CGeoCurve* startCurve = curveA;
	bool wrap = FALSE;

	while (1)
	{
		curveB = dynamic_cast<CGeoCurve*>( iter.Elem()->Elem() );

		offsetB = curveB->Offset( offsetDir, offsetAmt );
		owned = FALSE;  // ie. offsetB does not have an owner

		if (wrap && offsetB != NULL)
		{
			// Boundary condition on closed chain.
			CWmChainIterator tmp( (*result) );
			delete offsetB;
			offsetB = dynamic_cast<CGeoCurve*>( tmp.Elem()->Elem() );
			owned = TRUE;  // ie. offsetB does have an owner
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Determine whether we need a blend radius.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		// NOTE: We can not reference the offset curve
		// because there may be none.  Offsetting an arc
		// to its interior by a value greater that its
		// radius will result in a NULL offset entity.

		// One expects the input to be a set of contiguous curves.
		// However, with the introduction of 'tab stops' adjacent 
		// curves may well have a gap between them.  When this is
		// the case, we never want to blend the corresponding offset
		// curves, and we only want to trim them when they intersect
		// at at interior corner.
		ps = curveA->EndPt();
		pe = curveB->StartPt();
		gap = ( !ps.WithinTol( pe, SMALL ) );

		tanA = curveA->EndTan();
		tanB = curveB->StartTan();

		cross = tanA ^ tanB;
		blend = (gap ? FALSE : (cross * offsetDir) < -VECTOR_SMALL);

		dot = 0.;
		if ( blend )
		{
			// We've encountered an 'outside corner'.
			normA = tanA + (HALFPI * offsetDir);
			normB = tanB + (HALFPI * offsetDir);

			// Whether we continue with blending depends upon the
			// interior angle between the current adjacent curves.
			dot = normA * normB;
			blend = (dot < limit);
		}

		trmext = !blend;

		if ( blend )
		{
			// Insert a blend radius.
			pc = curveA->EndPt();
			ps = pc + (normA * offsetAmt);
			pe = pc + (normB * offsetAmt);
			result->CopyAppend( CGeoArc( ps, pe, pc, ((cross > 0) ? 1 : -1) ) );
		}
		else
		{
			// Interior corner, tangent or not sharp enough to blend.

			if (offsetA != NULL && offsetB != NULL)
			{
				ps = offsetA->EndPt();
				pe = offsetB->StartPt();

				trmext = ( !ps.WithinTol( pe, GOODENOUGH ) );

				if ( !trmext )
				{
					// Certainly within tolerance of current machine capability.
					// If we maintain c0 continuity (by nudging entity end points)
					// we can eliminate downstream failures.
					//
					// This kludge was introduced to handle the case where we're
					// offsetting to the outside of an intersection between a
					// CCW arc and a line, where the intersection is near tangent,
					// but in fact is really ever-so-slighty an interior corner
					// to the outside of the part (encountered whilst importing
					// sloppy AutoCAD data).
					//
					// Arc-Arc intersections are a bit more problematic.  In this
					// case, we must trim/extend the arcs if their common end points
					// are out of tolerance.

#if BEFORE_V14_5
					if (offsetA->Type() == GEOLINE)
						offsetA->EndPt( pe );
					else if (offsetB->Type() == GEOLINE)
						offsetB->StartPt( ps );
					else
						trmext = ( !ps.WithinTol( pe, SMALL ) );
#else
					AdjustCurveEndPoints( offsetA, offsetB );
#endif
				}
			}
			else
			{
				normA = tanA + (HALFPI * offsetDir);
				normB = tanB + (HALFPI * offsetDir);
				ps = curveA->EndPt() + (normA * offsetAmt);
				pe = curveB->StartPt() + (normB * offsetAmt);

				trmext = ( !ps.WithinTol( pe, GOODENOUGH ) );
			}
		}

		if ( trmext )
		{
			// 2009.07.03 (PE) -- ITI reported a nesting failure. The problem
			// arises when the kerf is being stitched together as the part is
			// being prepared for nesting. In particular, the stitching process
			// uses a distance tolerance that exceeds the length of small line
			// segments that are injected to bridge the gap between offset
			// curves. In the stated case, the reference curves were nearly
			// tangent and so the offset curves could have been simply extended.
			bool nearly_tangent = (dot >= TANGENT_LIMIT);

			// 2000/10/10 -- Introduced SGN(offsetAmt) to acount for negative offsets.
			trim = (nearly_tangent ||
				((cross * offsetDir * SGN(offsetAmt)) >= -VECTOR_SMALL));

			if (trim && offsetA != NULL && offsetB != NULL)
			{
				// 2009.07.03 (PE) -- In the nearly tangent case, we are
				// seeking solutions that *are not* on the curves. Otherwise,
				// we are seeking solutions that are on the curves.
				CInt2d int2d( SMALL, !nearly_tangent );

				int2d.CrvCrv( (*offsetA), (*offsetB) );

				int count = int2d.Count();  // Should be 1 ?
				if (count == 1)
				{
					pt = int2d.Point(0);
					offsetA->EndPt( pt );
					offsetB->StartPt( pt );
				}
				else if (count == 2)
				{
					double min_dist = UNDEFINED;
					int min_indx = -1;

					// Find the closest intersection to the end point
					// of offsetA because we are essentially moving from
					// offsetA to offsetB.
					C2dCoord tmp = offsetA->EndPt();
					for (int indx = 0; indx < count; ++indx)
					{
						pt = int2d.Point(indx);
						dx = pt.X() - tmp.X();
						dy = pt.Y() - tmp.Y();
						dist = dx*dx + dy*dy;  // no real need to do sqrt()
						if (dist < min_dist)
						{
							min_dist = dist;
							min_indx = indx;
						}
					}

					pt = int2d.Point(min_indx);
					offsetA->EndPt( pt );
					offsetB->StartPt( pt );
				}
				else
				{
					// 2006.08.26 (PE) -- Assume this is a near tangency case
					// where the offset curves are separated ever so slightly.
					dx = ps.X() - pe.X();
					dy = ps.Y() - pe.Y();

					// No real need to do sqrt()
					dist = dx*dx + dy*dy;

					// 2006.12.08 (PE) -- Sunflower reported a failure of a ever
					// part where the tolerance had to be increased ever so slightly.
					// (ie. approx 1.e-3 ^ 2)
					//    if (dist <= 1.e-6)
					if (dist <= 2.e-6)
						AdjustCurveEndPoints( offsetA, offsetB );
				}
			}
			else if ( !gap )
			{
				// Extend the adjacent offset entities.  When the offset
				// entity is a line, the line is simply extend.  This
				// helps minimize downstream work by keeping entity
				// counts to a minimum.  If the offset entity is an arc,
				// a line segment is inserted.
				//
				// Why all of the complexity?  If we can minimize the
				// introduction of short line segments, downstream
				// processing will be simplified.

				C3dCoord dmy;
				double u;
	
				Int2dSegSeg( ps, tanA, pe, tanB, &pt );

				if (offsetA != NULL && offsetA->Type() == GEOLINE)
					offsetA->EndPt( pt );

				if (offsetB != NULL && offsetB->Type() == GEOLINE)
					offsetB->StartPt( pt );

				if (offsetA != NULL && offsetA->Type() == GEOARC)
				{
					if (offsetB != NULL)
					{
						if (offsetB->Type() == GEOLINE)
						{
							dist = offsetB->PointClosest( ps, &dmy, &u );
							if (dist <= GOODENOUGH)
								offsetB->StartPt( ps );
							else
								result->CopyAppend( CGeoLine( ps, pt ) );
						}
					}
				}

				if (offsetB != NULL && offsetB->Type() == GEOARC)
				{
					if (offsetA != NULL && offsetA->Type() == GEOLINE)
					{
						dist = offsetA->PointClosest( pe, &dmy, &u );
						if (dist <= GOODENOUGH)
							offsetA->EndPt( pe );
						else
							result->CopyAppend( CGeoLine( pt, pe ) );
					}
					else if (offsetB->Type() == GEOARC)
					{
						if ( !ps.WithinTol( pt, SMALL ) )
							result->CopyAppend( CGeoLine( ps, pt ) );

						if ( !pe.WithinTol( pt, SMALL ) )
							result->CopyAppend( CGeoLine( pt, pe ) );
					}
				}
			}
		}

		if ( wrap )
		{
			// Terminal condition.
			// We've come full circle on a closed chain.
			if ( !owned )
				delete offsetB;
			break;
		}

		if (offsetB != NULL)
			result->Append( offsetB );


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Prepare for the next iteration.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		curveA = curveB;
		offsetA = offsetB;

		iter.NextElem();

		if ( iter.AtEnd() )
		{
			if ( !CWmChain::IsClosed() )
			{
				// Terminal condition.
				// We've reached the end of an open chain.
				break;
			}

			wrap = TRUE;
			iter.GotoStart();
		}
	}

	if ( status.IsOk() )
		result->DegenerateFilter();

	return status;
}

bool
CWmChain::Int2dSegSeg(
		const C2dCoord& psA,
		const C2dUnitVec& vecA,
		const C2dCoord& psB,
		const C2dUnitVec& vecB,
		C2dCoord* pt ) const
{
	double det = vecA.X() * vecB.Y() - vecA.Y() * vecB.X();

	if (fabs( det ) < VECTOR_SMALL)
		return FALSE;  // parallel lines

	double numerator = vecB.X() * (psA.Y() - psB.Y()) - vecB.Y() * (psA.X() - psB.X());

	double uparam = numerator / det;

	(*pt) = psA + (vecA * uparam);

	return TRUE;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Adjust the end point of curveA and the start point curveB such that
// both points have the same coordinates as the mid-point between them.
//
// NOTE: We must refit arcs!
//
void AdjustCurveEndPoints( CGeoCurve* curveA, CGeoCurve* curveB )
{
	C3dCoord pc;
	double rad, ang;
	int dir;

	C3dCoord ps = curveA->EndPt();
	C3dCoord pe = curveB->StartPt();

	C3dVec vec = pe - ps;
	C3dCoord pm = ps + (vec * 0.5);

	CGeoArc* arcA = dynamic_cast<CGeoArc*>( curveA );
	CGeoArc* arcB = dynamic_cast<CGeoArc*>( curveB );

	if (arcA != NULL)
	{
		ps  = arcA->StartPt();
		rad = arcA->Radius();
		dir = arcA->Dir();
		ang = arcA->IncludedAngle();
		CSolution::ArcCenter( ps, pm, rad, dir, ang, &pc );

		arcA->Init( ps, pm, pc, dir );
	}
	else
	{
		curveA->EndPt( pm );
	}

	if (arcB != NULL)
	{
		pe  = arcB->EndPt();
		rad = arcB->Radius();
		dir = arcB->Dir();
		ang = arcB->IncludedAngle();
		CSolution::ArcCenter( pm, pe, rad, dir, ang, &pc );

		arcB->Init( pm, pe, pc, dir );
	}
	else
	{
		curveB->StartPt( pm );
	}
}

void
CWmChain::CondInit()
{
	if (m_head == NULL)
	{
		m_head = new CWmSubchn( this );
		m_tail = new CWmSubchn( this );

		m_head->SubchnNext( m_tail );
		m_tail->SubchnPrev( m_head );

		m_head->Next( m_tail );
	}
}

void
CWmChain::DegenerateFilter()
{
	CWmNode* next;
	CWmNode* curr = m_head;
	while (1)
	{
		if (curr == NULL)
			break;

		next = curr->Next();

		CWmElem* wmelem = dynamic_cast<CWmElem*>( curr );
		if (wmelem != NULL)
		{
			double len = wmelem->Elem()->Length2d();
			if (len < SMALL)
			{
				wmelem->Unlink();
				delete wmelem;
			}
		}

		curr = next;
	}
}

// For filtering out abhorrent things like near zero-length chains.
bool
CWmChain::IsAbhorrentChain( const CWmChain& chain )
{
	if (chain.Count() == 1)
	{
		CWmChainIterator	iter;
		CWmElem*			wmElem;

		iter.Init( chain );
		wmElem = iter.Elem();
		if (wmElem == NULL)
			return true;

		if (wmElem->Elem()->Length2d() <= SMALL)
			return true;
	}

	return false;
}

bool
CWmChain::Encloses( const CWmChain& chain )
{
	CWmChainIterator	iter;
	CGeoPoly	this_poly;
	CGeoPoly	other_poly;
	CGeoCurve*	geoCurve;
	bool		does_enclose;

	iter.Init( (*this) );
	while( !iter.AtEnd() )
	{
		geoCurve = (CGeoCurve*) iter.Elem()->Elem();
		this_poly.CopyAppend( (*geoCurve) );
		iter.NextElem();
	}

	iter.Init( chain );
	while( !iter.AtEnd() )
	{
		geoCurve = (CGeoCurve*) iter.Elem()->Elem();
		other_poly.CopyAppend( (*geoCurve) );
		iter.NextElem();
	}

	does_enclose = this_poly.Encloses( other_poly );

	this_poly.BenignFlush();
	other_poly.BenignFlush();

	return does_enclose;
}

// This method was introduced because BoundsChecker encoutered dangling
// pointers during debugging of the MullionCycle.  The root problem is
// that chains representing islands are reused during subsequent offset
// resolution, but these chains were not cleared of previous intersection
// data.  An alternate solution would be to copy the island chains, but
// that would be more memory intensive.
void
CWmChain::PurgeSubchns()
{
	CWmNode*	curr;
	CWmNode*	next;
	CWmSubchn*	subchn;
	CWmElem*	elem;
	
	m_head->Purge();

	next = m_head->Next();
	while (next != m_tail)
	{
		curr = next;
		next = curr->Next();

		subchn = dynamic_cast<CWmSubchn*>(curr);
		elem = dynamic_cast<CWmElem*>(curr);

 		if (subchn != NULL)
		{
			curr->Unlink();
			delete curr;
		}
		else if (elem != NULL)
		{
			elem->Owner( m_head );
		}
	}

	m_tail->Purge();

	m_setNo = 0;
}


const CVarList* CWmChain::Attrib() const
{
	return m_attribs;
}

CVarList* CWmChain::pAttrib()
{
	if (m_attribs == NULL)
		m_attribs = new CVarList();

	return m_attribs;
}

int CWmChain::AttribCount() const
{
	return ((m_attribs == NULL) ? 0 : m_attribs->countVar());
}

int CWmChain::IntGet( const CString& name, int defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getInt( name, defval ));
}

double CWmChain::DoubleGet( const CString& name, double defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getReal( name, defval ));
}

CString CWmChain::StringGet( const CString& name, const CString& defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getString( name, defval ));
}

void CWmChain::IntSet( const CString& name, int ival )
{
	pAttrib()->setInt( name, ival );
}

void CWmChain::DoubleSet( const CString& name, double dval )
{
	pAttrib()->setReal( name, dval );
}

void CWmChain::StringSet( const CString& name, const CString& sval )
{
	pAttrib()->setString( name, sval );
}

void CWmChain::AttribDelete( const CString& name )
{
	if (m_attribs != NULL)
		m_attribs->deleteVar( name );
}

bool CWmChain::AnyElemAttribs() const
{
	CWmChainIterator iter( *this );

	while ( !iter.AtEnd() )
	{
		CWmElem* wmelem = iter.Elem();
		if (wmelem == NULL)
			break;

		if ( wmelem->Elem()->HasAttrib() )
			break;

		iter.NextElem();
	}

	return ( !iter.AtEnd() );
}

void CWmChain::Draw() const
{
	CGeoPoly tmp;

	CWmChainIterator iter;
	iter.Init( (*this) );

	while( !iter.AtEnd() )
	{
		CGeoCurve* geoCurve = dynamic_cast<CGeoCurve*>( iter.Elem()->Elem() );
		if (geoCurve != NULL)
			tmp.Append( geoCurve );
		iter.NextElem();
	}

	tmp.Draw();

	tmp.BenignFlush();
}

void CWmChain::Dump() const
{
	CGeoPoly tmp;

	CWmChainIterator iter;
	iter.Init( (*this) );

	while( !iter.AtEnd() )
	{
		CGeoCurve* geoCurve = dynamic_cast<CGeoCurve*>( iter.Elem()->Elem() );
		if (geoCurve != NULL)
			tmp.Append( geoCurve );
		iter.NextElem();
	}

	tmp.Dump();

	tmp.BenignFlush();
}
