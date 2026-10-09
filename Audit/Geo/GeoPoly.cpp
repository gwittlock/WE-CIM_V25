// ==================================================================
//		GeoPoly
//
//	This is a geometric polygon -- a figure composed of a series
//	of points with implied line connections.
//
//	Poly's have a variety of powerful testing options, specifically,
//	true-shape inclusion and exclusion of each other.
//
//	The main purpose of this entity is to perform inclusion / exclusion
//	tests so all geometry is X,Y only.
//
//	ALSO includes boolean operations on polygons.
//
//	Code based closely on PolySolid2 code from Magic Software, Inc.
//		http://www.magic-software.com
//		http://www.magic-software.com/License/free.pdf
//
//	Algorithm is (not well) described in David Eberly's paper,
//	"Polysolids and Boolean Operations" at 
//	http://www.magic-software.com/License/free.pdf
//
//	But this is just a version of the algorithm for constructive
//	planar geometry described by Michael Leonov for his poly_Boolean
//	software (found at http://www.afti.nsu.ru/~leonov/clipdoc.html).
//
//	Of course, this is just an extension of the older work by Klamer
//	Schutte, as described in "An edge labeling approach to concave polygon
//	clipping" as found at http://www.ph.tn.tudelft.nl/~klamer/Klamer.html.
//
//	This, in turn, is founded on the decomposition and classification of
//	polygons using BSP trees... 
//
//	Our implementation, however, also includes native treatment for arcs.
// ==================================================================

#include "stdafx.h"

#ifndef _DYNAMICARRAY_H
#include "DynamicArray.h"
#endif

#include "Common.h"
#include "MathConst.h"
#include "Register.h"
#include "Path.h"

#include "3dCoord.h"
#include "GeoCurve.h"
#include "GeoArc.h"
#include "GeoLine.h"

#include "GeoReducer.h"

#include "PolyEdge.h"
#include "PolySegment.h"
#include "hull.h"  // Convex_Hull_2D()

#include "GeoPoly.h"


static C3dCoord UNDEFINED_PT;

// ==================================================================

CGeoPoly::CGeoPoly()
{
	m_extent.Invalidate();
	m_area = 0.0;
//	m_propogate = false;
	m_propogate = true;

	m_polyp_trim = CRegister::BoolGetV( "Nesting", "polyp_trim", true );
}

CGeoPoly::CGeoPoly( const C2dBox& box )
{
	CGeoLine geo_line;

	m_extent.Invalidate();
	m_area = 0.0;
//	m_propogate = false;
	m_propogate = true;

	// Right, Top to Bottom
	geo_line.StartPt( C3dCoord( box.Xmax(), box.Ymax(), 0.0 ) );
	geo_line.EndPt( C3dCoord( box.Xmax(), box.Ymin(), 0.0 ) );
	do_append( geo_line );

	// Bottom, Right to Left
	geo_line.StartPt( C3dCoord( box.Xmax(), box.Ymin(), 0.0 ) );
	geo_line.EndPt( C3dCoord( box.Xmin(), box.Ymin(), 0.0 ) );
	do_append( geo_line );

	// Left, Bottom to Top
	geo_line.StartPt( C3dCoord( box.Xmin(), box.Ymin(), 0.0 ) );
	geo_line.EndPt( C3dCoord( box.Xmin(), box.Ymax(), 0.0 ) );
	do_append( geo_line );

	// Top, Left to Right
	geo_line.StartPt( C3dCoord( box.Xmin(), box.Ymax(), 0.0 ) );
	geo_line.EndPt( C3dCoord( box.Xmax(), box.Ymax(), 0.0 ) );
	do_append( geo_line );

	m_polyp_trim = CRegister::BoolGetV( "Nesting", "polyp_trim", true );
}

CGeoPoly::~CGeoPoly()
{
	Flush();
}

CGeoPoly& CGeoPoly::operator = ( const CGeoPoly& poly )
{
	Flush();

	int count = poly.Count();
	for (int indx = 0; indx < count; ++indx)
	{ 
		m_curves.Append( (CGeoCurve*)poly[indx].Clone( m_propogate ) );
	}

	m_extent = poly.m_extent;
	m_area = poly.m_area;

	UserData( poly.UserData() );
	*(pAttrib()) = poly.Attrib();

    return (*this);
}

// ==================================================================
void CGeoPoly::Flush( void )
{
	m_curves.DestructiveFlush();
	m_extent.Invalidate();
	m_area = 0.0;

	pAttrib()->Reset();
}

// ==================================================================

// Added to support Gouge\CWmChain.cpp (which owns the curves)
void CGeoPoly::BenignFlush( void )
{
	m_curves.BenignFlush();
	m_extent.Invalidate();
	m_area = 0.0;
}


// ==================================================================
//	Appends a CLONE of the curve into the poly.
//	ASSUMES each curve is in the same reference plane.
//  ASSUMES that the only curve types are GEOLINE and GEOARC
//
//	Note that UserData is preserved.  Note also that the curve is
//	Cloned, not referenced.
//
CReturn CGeoPoly::CopyAppend( const CGeoPoly& poly )
{
	CReturn ret;

	int num = poly.Count();
	for (int idx=0; idx<num; idx++)
	{
		ret += CopyAppend( poly[idx] );
	}

	return ret;
}

CReturn	CGeoPoly::CopyAppend( const CGeoCurve& curve )
{
	CReturn status;

	if (curve.Type() == GEOARC)
	{
		CGeoArc* arc = (CGeoArc*)&curve;

		// Force quadrant arcs; break 'em down
		CGeoArcList	qarc;

		arc->QuadrantArcs( &qarc );
		
		int num = qarc.Count();
		for (int idx=0; idx<num; idx++)
		{
			CGeoArc* quadrant = qarc.GetAt( idx );

			if (m_propogate && curve.HasAttrib())
				*(quadrant->pAttrib()) = curve.Attrib();

			status += do_append( *quadrant );
		}

		qarc.DestructiveFlush();
	}
	else
	{
		status = do_append(curve);
	}

	return status;
}

void CGeoPoly::Append( CGeoCurve* curve )
{
	// TODO: 2004.05.16 (PE) -- Hmmmmmm, not sure if arcs have
	// to be broken into quadrants because that was done for
	// nestings' sake.  Defering that until necessary.

	m_curves.Append( curve );
	m_area += sub_area( *curve );
	m_extent += curve->Box();
}

CReturn	CGeoPoly::do_append( const CGeoCurve& curve )
{
	CReturn ret;

	CGeoCurve* new_curve = (CGeoCurve*)curve.Clone( m_propogate );
	new_curve->UserData( curve.UserData() );

	m_curves.Append( new_curve );
	m_area += sub_area( *new_curve );
	m_extent += new_curve->Box();

	return ret;
}

// ==================================================================
//	Calculate the area component of this curve.
//	This is an incremental way of performing the Area calculate from
//	CSolution::Area().
//
//		Math reference:  (CRC) Standard Mathematic Tables & Formulae 
//
//		The area of a closed linear profile can be found by:
//			1/2 sum of (1<=i<=k) X[i]*Y[i+1] - X[i+1]*Y[i]
//
//		With arc sections, we need to adjust +- by the arc-cap area:
//			1/2 R^2(ang - sin(ang)) gives the cap area.
//
//		If the area is positive, the profile winds CCW, else it winds CW
//
double CGeoPoly::sub_area( const CGeoCurve& curve ) const
{
	double area = 0.0;

	C2dCoord ptA = curve.StartPt();
	C2dCoord ptB = curve.EndPt();

	switch (curve.Type())
	{
	case GEOLINE:
		area = (ptA.X() * ptB.Y()) - (ptB.X() * ptA.Y());
		break;

	case GEOARC:
		{
		const CGeoArc* arc = dynamic_cast<const CGeoArc*>( &curve );
		double radius = arc->Radius();

		if ( ptA.WithinTol( ptB, SMALL ) )
		{
			area = (arc->Dir() * TWOPI * radius * radius);
		}
		else
		{
			// Triangular area, across arc's chord
			area = (ptA.X() * ptB.Y()) - (ptB.X() * ptA.Y());

			// Adjust for curved portion...
			double angle = arc->IncludedAngle();
			double arcarea  = radius * radius * (angle - sin(angle));

			area += (arc->Dir() * arcarea);
		}
		}
		break;
	}

	return area / 2.0;
}

// Reverse the winding of the poly
void CGeoPoly::Reverse( void )
{
	int st = 0;
	int en = m_curves.Count()-1;

	while (st < en)
	{
		CGeoCurve* one = m_curves[st];
		CGeoCurve* two = m_curves[en];

		one->Reverse();
		two->Reverse();

		m_curves.Replace( st, two );
		m_curves.Replace( en, one);

		st++;
		en--;
	}
	if (st == en)
	{
		CGeoCurve* tmp = m_curves[st];
		tmp->Reverse();
	}

	m_area = -m_area;
}

// (<0) left shift / (>0) right shift
void CGeoPoly::Shift( int delta )
{
	// TODO: CGeoPoly::Shift() was "fixed" while reimplementing
	// the way leads are applied to a nested part. This may have
	// broken something ... so beware.
	if (delta != 0)
	{
		int count = m_curves.Count();
		if (abs(delta) < count)
			m_curves.Shift( delta );
	}
}

bool CGeoPoly::IsClosed( double tol ) const
{
	bool is_closed = false;

	int count = m_curves.Count();
	if (count > 0)
	{
		if (count == 1)
		{
			is_closed = m_curves[0]->IsClosed( tol );
		}
		else
		{
			CGeoCurve* geoCurveA = m_curves[0];
			CGeoCurve* geoCurveB = m_curves[count-1];

			C2dCoord ptA = geoCurveA->StartPt();
			C2dCoord ptB = geoCurveB->EndPt();

			is_closed = (ptA.WithinTol( ptB, tol ) != 0);
		}
	}

	return is_closed;
}

bool CGeoPoly::IsConvex() const
{
	bool isConvex = false;

	int count = m_curves.Count();
	if ( IsCircle( SMALL ) )
	{
		isConvex = true;
	}
	else if (count > 1)
	{
		double prevCross = 0.;

		int indxA;
		for (indxA = 0; indxA < count; ++indxA)
		{
			CGeoCurve* geoCurveA = m_curves[indxA];
			C2dUnitVec tanA = geoCurveA->EndTan();

			int indxB = indxA + 1;
			if (indxB >= count)
			{
				if ( IsClosed( SMALL ) )
					indxB = 0;
				else
					break;
			}

			CGeoCurve* geoCurveB = m_curves[indxB];
			C2dUnitVec tanB = geoCurveB->StartTan();

			double cross = tanA ^ tanB;

			if ((indxA > 0) && (SGN(cross) != SGN(prevCross)))
				break;

			prevCross = cross;
		}

		isConvex = (indxA >= count);
	}

	return isConvex;
}

bool CGeoPoly::IsCircle( double tol ) const
{
	bool is_circle = false;

	if ((m_curves.Count() == 1) && (m_curves[0]->Type() == GEOARC))
	{
		CGeoArc* geoArc = dynamic_cast<CGeoArc*>( m_curves[0] );
		is_circle = geoArc->IsCircle( tol );
	}

	return is_circle;
}

void CGeoPoly::GapsClose()
{
	CGeoCurveArray	temp;
	CGeoCurve*		curveA;
	CGeoCurve*		curveB;
	C3dCoord		ptA;
	C3dCoord		ptB;
	int	count, indxA, indxB;

	count = m_curves.Count();
	if (count > 1)
	{
		// Close the gaps while moving the curves of this
		// poly to temp.  This is more efficient than
		// inserting new curves into m_curves.
		indxA = 0;
		while (1)
		{
			curveA = m_curves[indxA];

			indxB = ((indxA >= (count - 1)) ? 0 : (indxA + 1));
			curveB = m_curves[indxB];

			ptA = curveA->EndPt();
			ptB = curveB->StartPt();

			temp.Append( curveA );
			if ( !ptA.WithinTol( ptB, SMALL ) )
				temp.Append( new CGeoLine( ptA, ptB ) );

			if (indxB == 0)
				break;  // We've come full circuit.

			++indxA;
		}

		// Move the curves from temp back to m_curves.
		m_curves.BenignFlush();
		m_extent.Invalidate();
		m_area = 0.0;
		
		count = temp.Count();
		for (indxA = 0; indxA < count; ++indxA)
		{
			curveA = temp[indxA];
			m_curves.Append( curveA );
			m_area += sub_area( *curveA );
			m_extent += curveA->Box();
		}
		temp.BenignFlush();
	}
}

// ==================================================================
//	Returns TRUE if this Poly completely encloses the specified Poly.
//	Performs the test through a comprehensive point-in-poly test, which
//	will FAIL unless the two polygons are moderately well-behaved.  If,
//	for example, the interior poly has a line segment that jumps across
//	a narrow concavity in the containing poly, this algorithm might miss
//	the violation; so this system works best for convex simple polygons.  
//
bool CGeoPoly::Encloses( CGeoPoly& poly )
{
	if (!Extent().Intersects(poly.Extent(), SMALL))
		return FALSE;

	for (int idx=0; idx<poly.Count(); idx++)
	{
		const CGeoCurve& curve = poly[idx];

		// Assumes connectec curves in the poly, so only test starts.
		if ( !PtInPoly( curve.StartPt() ) )
			return FALSE;
	}
	return TRUE;
}

#if (_CI || _NST)
	bool		
	CGeoPoly::CI_Encloses( CGeoPoly& poly )
	{
		if (!Extent().Intersects(poly.Extent(), SMALL))
			return FALSE;

		for (int idx=0; idx<poly.Count(); idx++)
		{
			const CGeoCurve& curve = poly[idx];

			// Assumes connected curves in the poly, so only test starts.
			if ( !PtInPoly( curve.StartPt() ) )
			{
				if (poly.Count() > 1)
				{
					return FALSE;
				}
				else
				{
					// Special case, during import as extended-asc file.
					// A single-line route that crosses sheet should be
					// identified as internal.
					if ( !PtInPoly( curve.MidPt() ) )
						return FALSE;
				}
			}
		}
		return TRUE;
	}
#endif

// ==================================================================
//	Returns TRUE if this Poly partially encloses the specified Poly.
bool CGeoPoly::Overlaps( CGeoPoly& poly )
{
	if (!Extent().Intersects(poly.Extent(), SMALL))
		return FALSE;

	for (int idx=0; idx<poly.Count(); idx++)
	{
		const CGeoCurve& curve = poly[idx];

		if (PtInPoly( curve.StartPt() ))
			return TRUE;
	}
	return FALSE;
}

#if (_CI || _NST)
	bool		
	CGeoPoly::CI_Overlaps( CGeoPoly& poly )
	{
		if (!Extent().Intersects(poly.Extent(), SMALL))
			return FALSE;

		for (int idx=0; idx<poly.Count(); idx++)
		{
			const CGeoCurve& curve = poly[idx];

			// TODO: If people start to complain about the file import
			// process failing to properly identify exterior/interior
			// profiles, then we may have to switch to a solution that
			// employs the intersection between two profiles.
			//
			// ALSO NOTE: This change does affect CNestingPart::Assemble().
			//
			if (PtInPoly( curve.StartPt() ))
				return TRUE;
			else if (PtInPoly( curve.MidPt() ))
				return TRUE;
		}
		return FALSE;
	}
#endif

// ==================================================================
//	Returns TRUE if this Poly completely excludes the specified Poly.
//	See caveat notes for Encloses.
bool CGeoPoly::Excludes( const CGeoPoly& poly )
{
	// Simple extent checks are not reliable for this form
	//
	for (int idx=0; idx<poly.Count(); idx++)
	{
		const CGeoCurve& curve = poly[idx];

		// Assumes connectec curves in the poly, so only test starts.
		if (PtInPoly( curve.StartPt() ))
			return FALSE;
	}
	return TRUE;
}

// ==================================================================
//	Test if the specified point is contained within this poly.
//
//	SIMILAR TO Point in Polygon test from Graphics Gems IV,
//		K. Weiler, P.16 (old form)
//
//	USES "Fast Winding Number Inclusion of a Point in a Polygon" by Dan Sunday,
//	March 2001 Algorithm, http://geometryalgorithms.com/Archive/algorithm_0103.htm
//
//	Extended be Edwin Wise to use quadrant arcs (which are valid for the algorithm), 
//	
bool CGeoPoly::PtInPoly( const C2dCoord& pnt ) const
{
	CGeoLine	geo_line;
	CGeoArc		geo_arc;
	CGeoCurve*	geo_curve=NULL;

	// Trivial extent rejection
	if ( !Extent().Contains(pnt, SMALL) )
		return FALSE;

	// Loop over the Poly
	int num = Count();
	if (num < 1)
		return FALSE;

	if (num == 1)
	{
		const CGeoArc* geoArc = dynamic_cast<const CGeoArc*>( m_curves[0] );
		if (geoArc == NULL)
			return FALSE;

		if ( !geoArc->IsCircle( SMALL ) )
			return FALSE;

		double dist = geoArc->CenterPt().DistXY( pnt );
		return (dist <= (geoArc->Radius() - SMALL));
	}

	// Decrementing simplifies following loop logic.
	--num;

#if BEFORE_V18_0_52_1
	is_closed = IsClosed( SMALL );
#else
	// Why do we care if it is closed because we force
	// closure upon reaching the fence post condition?
#endif

	// Winding containment test
	int winding = 0;

	// TODO: Cope with tab gaps?
	CGeoCurve* zero = m_curves[0];
	for (int idx=0; idx<=num; idx++)
	{
		// Extract the geometry from the list
		//
		CGeoCurve* curve = m_curves[idx];
		const C2dCoord& start = curve->StartPt();
#if BEFORE_V18_0_52_1
		const C2dCoord& end = ((idx<num)||!is_closed)?curve->EndPt():zero->StartPt();
#else
		const C2dCoord& end = ((idx < num) ? curve->EndPt() : zero->StartPt());
#endif

		//
		// Interpret the winding of the current curve
		// An on-curve hit is considered to be inside
		//
		if (start.Y() <= (pnt.Y()+SMALL) )
		{
			if (end.Y() > (pnt.Y()+SMALL))
			{
				int side = curve->PointSide(pnt);
				if ((side == 0)	&& curve->PointOnSeg(pnt))
					return TRUE;

				if (side < 0)  // Left of edge
					++winding;
			}
		}
		else // Start->Y() > pnt->Y()
		{
			if (end.Y() <= (pnt.Y()+SMALL) )
			{
				int side = curve->PointSide(pnt);
				if ((side == 0)	&& curve->PointOnSeg(pnt))
					return TRUE;

				if (curve->PointSide(pnt) >= 0) // Right of edge
					--winding;
			}
		}
	}

	return winding!=0;
}

// ==================================================================
//	Intersection (P AND Q)
CGeoPoly* CGeoPoly::AND( CGeoPoly& poly )
{
    // segment the polysolids
    CPolyEdge segedge_L[4];
	CPolyEdge segedge_R[4];

	CReturn ret;
	CString note;
	if (CReturn::Debug()>=9)
	{
		note.Format( "==== AND =====(OIMP)==========" );
		ret.Diagnostic( note );

		int idx;
		for (idx=0; idx<4; idx++)
		{
			note.Format( "  Left edge[%d] = %x", idx, ((long)&segedge_L[idx])&0xff );
			ret.Diagnostic( note );
		}
		for (idx=0; idx<4; idx++)
		{
			note.Format( "  Right edge[%d] = %x", idx, ((long)&segedge_R[idx])&0xff );
			ret.Diagnostic( note );
		}
	}

    Segment( poly, segedge_R );
    poly.Segment( *this, segedge_L );

	if (CReturn::Debug()>=9)
	{
		note.Format( "==== (merging) =====================" );
		ret.Diagnostic( note );
	}

    // all ITAG edges are in the intersection
    segedge_L[ITAG].MergeAppend( segedge_R[ITAG], FALSE );

    // all PTAG edges are in the intersection, avoid duplicates
    segedge_L[PTAG].MergeUnique( segedge_R[PTAG], FALSE );

    // final merge
    segedge_L[ITAG].MergeAppend( segedge_L[PTAG], FALSE );

    // convert segment edges to a geo curves
//    return segedge_L[ITAG].GenerateCurves();
	replace_curves( segedge_L[ITAG].Curves() );

	return this;
}

// ==================================================================
//	Union (P OR Q)
CGeoPoly* CGeoPoly::OR( CGeoPoly& poly )
{
    // segment the polysolids
    CPolyEdge segedge_L[4];
	CPolyEdge segedge_R[4];

	CReturn ret;
	CString note;
	if (CReturn::Debug()>=9)
	{
		note.Format( "==== OR ======(OIMP)==========" );
		ret.Diagnostic( note );

		int idx;
		for (idx=0; idx<4; idx++)
		{
			note.Format( "  Left edge[%d] = %x", idx, ((long)&segedge_L[idx])&0xff );
			ret.Diagnostic( note );
		}
		for (idx=0; idx<4; idx++)
		{
			note.Format( "  Right edge[%d] = %x", idx, ((long)&segedge_R[idx])&0xff );
			ret.Diagnostic( note );
		}
	}

	if (0)
	{
		poly.Dump();
		poly.Draw();
		this->Dump();
		this->Draw();
	}

    Segment( poly, segedge_R );
    poly.Segment( *this, segedge_L );

	if (0)
	{
		segedge_R[OTAG].Draw();
		segedge_R[ITAG].Draw();
		segedge_R[MTAG].Draw();
		segedge_R[PTAG].Draw();

		segedge_L[OTAG].Draw();
		segedge_L[ITAG].Draw();
		segedge_L[MTAG].Draw();
		segedge_L[PTAG].Draw();
	}

    // all OTAG edges are in the union
    segedge_L[OTAG].MergeAppend( segedge_R[OTAG], FALSE );

    // all MTAG-MTAG edges are in the union
    segedge_L[MTAG].MergeEqual( segedge_R[MTAG] );

    // all PTAG-PTAG edges are in the union
    segedge_L[PTAG].MergeEqual( segedge_R[PTAG] );

    // final merge
    segedge_L[OTAG].MergeAppend( segedge_L[MTAG], FALSE );
    segedge_L[OTAG].MergeAppend( segedge_L[PTAG], FALSE );

    // convert segment edges to a geo curves
	replace_curves( segedge_L[OTAG].Curves() );

	if (0)
	{
		poly.Dump();
		poly.Draw();
		this->Dump();
		this->Draw();
	}

	return this;
}

// ==================================================================
//	Difference (P - Q == P AND !Q)
CGeoPoly* CGeoPoly::MINUS( CGeoPoly& poly)
{
    // segment the polysolids
    CPolyEdge segedge_L[4];
	CPolyEdge segedge_R[4];

	CReturn ret;
	CString note;
	if (CReturn::Debug()>=9)
	{
		note.Format( "==== DIF =====(OIMP)==========" );
		ret.Diagnostic( note );

		int idx;
		for (idx=0; idx<4; idx++)
		{
			note.Format( "  Left edge[%d] = %x", idx, ((long)&segedge_L[idx])&0xff );
			ret.Diagnostic( note );
		}
		for (idx=0; idx<4; idx++)
		{
			note.Format( "  Right edge[%d] = %x", idx, ((long)&segedge_R[idx])&0xff );
			ret.Diagnostic( note );
		}
	}
    Segment( poly, segedge_R );
    poly.Segment( *this, segedge_L );

    // all OTAG edges of P and ITAG edges of Q are in the difference
    segedge_L[OTAG].MergeAppend( segedge_R[ITAG], TRUE );

//	// all PTAG edges are in the difference, avoid duplicates
//	segedge_L[MTAG].MergeUnique( segedge_R[PTAG], TRUE );

	// all MTAG edges are in the difference, avoid duplicates
	segedge_L[MTAG].MergeUnique( segedge_R[MTAG], TRUE );

    // final merge
    segedge_L[OTAG].MergeAppend( segedge_L[MTAG], FALSE );

    // convert segment edges to a geo curves
//    return segedge_L[OTAG].GenerateCurves();
	replace_curves( segedge_L[OTAG].Curves() );

	return this;
}


// ==================================================================
//	replace_curves
//
//	Replace the poly's curves with these curves
//
#if BEFORE_V19
void
CGeoPoly::replace_curves( CGeoCurveList* curves )
{
	m_curves.DestructiveFlush();

	m_area = 0.;
	m_extent.Invalidate();

	int num = curves->Count();
	for (int idx=0; idx<num; idx++)
	{
		// Filter small bastards out
		//
		CGeoCurve* geo_curve = (*curves)[idx];
		if ( !geo_curve->StartPt().WithinTolXY( geo_curve->EndPt(), SMALL ) )
		{
			m_curves.Append( geo_curve );
			m_area += sub_area( (*geo_curve) );
		}
	}

	curves->BenignFlush();
}
#else
// replace_curves() appears to be called only by CGeoPoly boolean methods.
// ASSUMPTIONS: The input curves are 1) unique, 2) have the proper direction,
// 3) have subsets that are c0-continuous and 4) have no two curves starting
// at the end of a reference curve.
//
// NOTE: If the result *is not* a single c0-continuous sequence, the area
// calculate will be incorrect!
void CGeoPoly::replace_curves( CGeoCurveList* curves )
{
	CGeoCurve*	seed;
	CGeoCurve*	reference;
	CGeoCurve*	candidate;
	int			indx;

	m_curves.DestructiveFlush();

	m_area = 0.;
	m_extent.Invalidate();

	// Filter out all of the tiny segments (Hopefully there are none).
	// This may (of course) create tiny gaps in the data but that is
	// sorta' okay (by assumption #3 above).
	indx = 0;
	while (1)
	{
		if (indx >= curves->Count())
			break;

		seed = curves->GetAt( indx );

		const C3dCoord& ps = seed->StartPt();
		const C3dCoord& pe = seed->EndPt();
		if ( ps.WithinTolXY( pe, SMALL ) )
		{
			delete curves->Remove( indx );
		}
		else
		{
			++indx;
		}
	}

	// Create the ordered subsets of curves.
	seed = NULL;
	while (1)
	{
		if (curves->Count() < 1)
			break;  // Exhausted the input curves.

		// Arbitrary starting point.
		indx = 0;
		seed = curves->Remove( indx );

		m_curves.Append( seed );
		m_area += sub_area( (*seed) );

		reference = seed;
		while (1)
		{
			if (indx >= curves->Count())
				break;  // Exhausted the input curves.

			candidate = curves->GetAt( indx );

			const C3dCoord& pe = reference->EndPt();
			const C3dCoord& ps = candidate->StartPt();
			if ( ps.WithinTolXY( pe, SMALL ) )
			{
				candidate = curves->Remove( indx );
				m_area += sub_area( (*candidate) );

				m_curves.Append( candidate );
				reference = candidate;

				indx = 0;
			}
			else
			{
				++indx;
			}
		}
	}

	// TODO: collinear/coarc reduction?

	// 2008.02.16 (PE) -- I believe this is a memory leak because
	// the client replace_curves() owns the 'curves' unless, of
	// course, they are transfered to this poly.
	//
	//   curves->BenignFlush();
}
#endif


// ==================================================================
//	Chop up the incoming CGeoPoly against this one, putting the resulting
//	edges into the four categories available
void CGeoPoly::Segment( const CGeoPoly& poly, CPolyEdge segedge[4] )
{
	int num = poly.Count();
	for (int idx=0; idx<num; idx++)
	{
		const CGeoCurve& edge = poly[idx];

        // segment curve against 'this' polygon
        CPolySegment segment( edge );

		CVarList* attribs = ((m_propogate && edge.HasAttrib()) ?
			((CGeoCurve&) edge).pAttrib() : NULL);

		// *Must* be careful lest things that shouldn't get attributes.
		segedge[OTAG].Attribs( attribs );
		segedge[ITAG].Attribs( attribs );
		segedge[MTAG].Attribs( attribs );
		segedge[PTAG].Attribs( attribs );

		// So the offspring result from a segment split against
		// this poly carry forth the attributes of the original.
		segment.Attribs( attribs );

        segment.SegmentBy( *this );

        if ( segment.Count() )
        {
            segment.Reduce( edge );

            if ( segment.Count() )
            {
                // compute tagged edge lists
                segment.ConvertToEdges( segedge );
            }
        }

        if ( !segment.Count() )
        {
            // edge did not intersect 'this', so must be outside
            segedge[OTAG].AddEdge( edge );
        }
    }
}



// ==================================================================
//	Take the GeoPoly and scan it in connected (profile) order
// (which may require PUTTING it into profile order, after a
//	boolean operation) and merge any colinear/coaxial entities,
//	hopefully reducing the entity count.
void CGeoPoly::Reduce()
{
	if (m_curves.Count() < 2)
		return;

	// Put into profile order
	CGeoCurveArray	reduced_curves;
	C2dCoord		seedPt;
	C2dCoord		startPt;
	CGeoCurve*		seed_curve;
	CGeoCurve*		curve;
	int				indx;

	// ----------------------------------------------
	// Cloned and copied from CProfileBuilder::ProfileGrow()
	//	But using Geo lists and whatnot, not Db lists.
	//	TODO:  Sort in-place, instead of across lists?
	//
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Grow the profile in the direction of the seed curve.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	int seed_indx = -1;
	while (m_curves.Count())
	{
		if ( seed_indx >= 0) 
		{
			seed_curve = m_curves.Remove( seed_indx );
			reduced_curves.Append( seed_curve );
		}
		else
		{
			seed_curve = m_curves.Remove(0);
			reduced_curves.Append( seed_curve );
		}

		// Lock in the 2d nature of things
		((C3dCoord&)seed_curve->StartPt()).Z(0.0);
		((C3dCoord&)seed_curve->EndPt()).Z(0.0);
		if (seed_curve->Type() == GEOARC)
			((C3dCoord&)((CGeoArc*)seed_curve)->CenterPt()).Z(0.0);

		seed_indx = -1;
		seedPt = seed_curve->EndPt();
		for (indx = 0; indx < m_curves.Count(); ++indx)
		{
			curve = m_curves[indx];

			startPt = curve->StartPt();

			if ( startPt.WithinTol( seedPt, SMALL ) )
			{
				seed_indx = indx;
				break;
			}
		}
	}

	// ----------------------------------------------

	if (m_curves.Count() != 0)
		MessageBox( NULL, "STOP! CGeoPoly::Reduce() losing data", NULL, MB_OK );

	m_curves.DestructiveFlush();

	// TODO: CDynamicArray::operator = ( ) ???
	for (indx = 0; indx < reduced_curves.Count(); ++indx)
	{
		m_curves.Append( reduced_curves.GetAt(indx) );
	}

	//
	// Now, reduce!
	//
	// Don't want to merge our quadrant arcs.. would be bad
	// CGeoReducer::ArcsReduce( *m_curves, SMALL );
	// CGeoReducer::LinesReduce( m_curves, true, SMALL );
}

void CGeoPoly::LinesReduce( bool wrap, double tol )
{
	CGeoReducer::LinesReduce( m_curves, wrap, tol );
}

void CGeoPoly::ArcsReduce( bool wrap, double tol )
{
	CGeoReducer::ArcsReduce( m_curves, wrap, tol );
}

// ==================================================================
//	Starting with the first curve in the Poly, extract all
//	connected (and semi-connected) curves into the given Poly.
//	Note that there WILL be (a) gaps that we must skip over, and
//	(b) multiple-choice junctions.  All junction decisions will be
//	made by taking the most extreme entity in the direction indicated.
//
//	The profile linker is exactly like the one in Reduce() in structure,
//	with the addition of the turning logic.
//
//	ASSUMES that we have CLOSED profiles.  Does NOT	try to link backwards.
//
//	Creates new CGeoPoly object to return... 
//
CGeoPoly* CGeoPoly::ExtractProfile( 
	int			dir,
	double		gap_tol,
	bool		creating_remnant )
{
	CString note;
	CReturn ret;

	if (CReturn::Debug()>=8)
	{
		note.Format( "Extract Profile direction %d, gap %f", dir, gap_tol );
		ret.Diagnostic( note );
	}

	if (m_curves.Count() <= 0)
		return NULL;

	// For memory management (garbage collection).
	CGeoCurveArray	spent;

	// TODO: THIS BASTARD IS A LEAK.. track down its use and free it
	CGeoPoly* profile = new CGeoPoly;

	// Necessary, particularly for "~isLeadHull".
	profile->AttribsPropogate( true );

	// Put into profile order
	//
	// ----------------------------------------------
	// Cloned and copied from CProfileBuilder::ProfileGrow()
	//	But using Geo lists and whatnot, not Db lists.
	//	TODO:  Sort in-place, instead of across lists?
	//
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Grow the profile in the direction of the seed curve.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CGeoCurve* prev_curve = NULL;
	CGeoCurve* seed_curve = NULL;
	while (m_curves.Count())
	{
		const C3dCoord* origin = NULL;
		int seed_idx = 0;

		seed_idx = rightmost_entity( m_curves );
		if (seed_idx < 0)
			seed_idx = 0;

		while (seed_idx >= 0)
		{
			seed_curve = m_curves.Remove( seed_idx );
			spent.Append( seed_curve );

			bool reject = seed_curve->StartPt().WithinTolXY( seed_curve->EndPt(), SMALL );
			if (seed_curve->Type() == GEOARC)
			{
				double angle = ((CGeoArc*)seed_curve)->IncludedAngle();;
				reject |= ( (angle <= SMALL) || (fabs(angle-TWOPI) <= SMALL) );
			}
			else // GEOLINE
			{
				reject |= (seed_curve->StartPt() - seed_curve->EndPt()).Length() <= SMALL;
			}

			if ( !reject )
			{
				if (origin == NULL)
					origin = &seed_curve->StartPt();

				if (prev_curve)
				{
					if (!prev_curve->EndPt().WithinTolXY( seed_curve->StartPt(), SMALL ) )
					{ 
						CGeoLine gap_line( prev_curve->EndPt(), seed_curve->StartPt() );
						profile->CopyAppend( gap_line );
					}
				}

				profile->CopyAppend( *seed_curve );

				// Loop-test exit
#if BEFORE_2005_01_13
				// if (seed_curve->EndPt().WithinTolXY( *origin, gap_tol ) )
				if (prev_curve && seed_curve->EndPt().WithinTolXY( *origin, gap_tol ) )
					break;
#else
				// This loop prematurely terminated for cad_nester, creating
				// an open profile.  The conditions leading to this failure are:
				//
				// 1. The initial outside kerf offset was 5.e-5
				// 2. A blending arc was produced between adjacent lines in the offset.
				// 3. CGeoPoly split the blending arc at quadrant boundaries.
				// 4. The arc-length of one of the resulting arcs was very small.
				//    ... smaller than the gap_tol.
				//
				// TODO: (?) Because the nesting engine stuffs many pieces (from
				// outside & inside kerf) into the same poly, it is possible for
				// ExtractProfile() to make bad decisions (and incorporate curves
				// from different offsets into the resulting profile).  Perhaps
				// we should tag each curve with some kind of group indicator?
				// That way, we could simply consider curves from the same group.
				//
				if (prev_curve && seed_curve->EndPt().WithinTolXY( *origin, SMALL ) )
					break;
#endif

				prev_curve = seed_curve;
			}

			// This is where it is different; I test ALL possible next
			// curves that are in range
			double min_delta = 999;
			double min_dist = 999;
			double min_pdot = 999;

			C3dCoord seedPnt = seed_curve->EndPt();
			C2dUnitVec seedTan = seed_curve->EndTan();

			seed_idx = -1;
			for (int idx= 0; idx<m_curves.Count(); idx++)
			{
				CGeoCurve* curve = m_curves[idx];

				C2dVec gap = curve->StartPt() - seedPnt;
				C2dUnitVec startTan = curve->StartTan();

				double dot = seedTan * startTan;
				double pdot = seedTan.PerpDot( startTan );
				double delta = fabs( dir - pdot );
				double dist = gap.Length();

				// Reject anti-parallel
				// This is to prevent it jumping to a parallel, reverse track on the other side
				// of the kerf.  However, this prevents tangencies in remnant creation
				//  So, for now, THIS TEST IS SUSPECT.  I think I'm confused on dot.
				//  Except, if I test dot for > SMALL, then regular extraction is failing.
// I suspect the problem is here!
				// Scoring:
				//	Regardless, must be within gap tol
				bool closer = (dist < min_dist);
				bool touching = ZERO(dist);
				bool tangent = ZERO(delta);
				bool turner = (delta < min_delta);
				bool anti = ( ZERO(pdot) && (dot < -SMALL) );

				bool win = false;
				if (dist <= gap_tol)
				{
					if ( !anti )
					{
						bool allow_gap = true;
						if (allow_gap)
						{
							if (touching)
							{
								if (turner)
									win = true;
							}
							else
							{
								// This gap stuff is KILLING ME
								// ie. better turn angle, or at least closer.
								if (turner || (EQUAL(delta, min_delta) && closer))
									win = true;
							}
						}
						else
						{
							// Eliminate the concept of "gap tolerance" -- values too small break profiles and
							// values too large break profiles.  The need to jump a gap in the course of defining
							// the outside of a kerf (versus the inside of a kerf) is a fine balancing act that
							// relies far too much on the user.  So, if you have tab gaps, put a damned
							// Part_outline around it.
							if (closer || (touching && turner))
								win = true;
						}
					}
					else if (creating_remnant)
					{
						// 2004.03.14 -- It is possible (and likely) that potentially
						// linked curves will be running in opposite directions.  Case
						// in point, an arc rounds the corner of one part and intersects
						// the offset of another part.
						//
						// At this implementation, we assume lines are coincident and
						// the solution is ambiguous.  However, for the time being, we
						// allow line/arc and arc/arc combinations to pass.
						win = ((seed_curve->Type() == GEOARC) || (curve->Type() == GEOARC));
					}
				}

				if (win)
				{
					min_dist = dist;
					min_delta = delta;
					min_pdot = pdot;
					seed_idx = idx; 
				}

				if (CReturn::Debug()>=9)
				{
					note.Format( "   ... %s %f, %f (turn %f, dist %f) (closer %d, touching %d, turner %d, tangent %d, anti %d)",
						(win ? "win" : "" ), curve->StartPt().X(), curve->StartPt().Y(), delta, dist, closer, touching, turner, tangent, anti );
					ret.Diagnostic( note );
				}

			}

			if ( (CReturn::Debug()>=8) && (seed_idx>=0) )
			{
				CGeoCurve* curve = m_curves[seed_idx];
				note.Format( "... %f, %f to %f, %f (turn %f, dist %f)",
					curve->StartPt().X(), curve->StartPt().Y(), curve->EndPt().X(), curve->EndPt().Y(), min_delta, min_dist );
				ret.Diagnostic( note );
			}

		} // while seed_idx

		if (profile->Count())
			break;
	} // while m_curves->Count();

	// Delete the curves that were remove from the poly.
	spent.DestructiveFlush();

	if (profile->Count() == 0)
	{ 
		delete profile;
		profile = NULL;
	}
	else
	{
		m_extent.Invalidate();
	}

	return profile;
}

int CGeoPoly::rightmost_entity( const CGeoCurveArray& curvelist )
{
	double right = -DBL_MAX;
	int ridx = -1;

	int num = curvelist.Count();
	for (int idx=0; idx<num; idx++)
	{
		double rx = curvelist[idx]->Box().Xmax();
		if (rx > right)
		{ 
			right = rx;
			ridx = idx;
		}
	}
	return ridx;
}

// NOTE: We could introduce a function that simply splits
// a poly at the nearest point on the poly to 'pt' but
// 1) there may be multiple solutions and 2) for this version
// of the method, introduced when refactoring lead-generation,
// we have the appropriate input data.
bool CGeoPoly::Split( int indx, const C2dCoord& pt )
{
	bool split = false;

	int count = m_curves.Count();
	if ((indx >= 0) && (indx < count))
	{
		CGeoCurve* first = m_curves.GetAt( 0 );
		C3dCoord splitPt( pt.X(), pt.Y(), first->StartPt().Z() );

		CGeoCurve* curve = m_curves[indx];
		if (!splitPt.WithinTol( curve->StartPt(), SMALL ) &&
			!splitPt.WithinTol( curve->EndPt(), SMALL ))
		{
			CGeoCurve* copy = (CGeoCurve*) curve->Clone( m_propogate );
			if (copy != NULL)
			{
				curve->EndPt( splitPt );
				copy->StartPt( splitPt );

				m_curves.InsertAfter( indx, copy );

				split = true;
			}
		}
	}

	return split;
}

// ==================================================================
const C2dBox& CGeoPoly::Extent( void ) const
{ 
	if (!m_extent.IsDefined())
	{
		int num = m_curves.Count();
		for (int idx=0; idx<num; idx++)
		{
			CGeoCurve* curve = m_curves[idx];
			((CGeoPoly*) this)->m_extent += curve->Box();
		}
	}
	return m_extent;
}	


// ==================================================================
// Expecting only one poly in array; but if more, merge all extents together
//
// TODO:  instead of these bandaids, merge all of the profiles that we
//		EXPECT to be merged?
C2dBox CGeoPoly::Extent( CGeoPolyArray* array )
{
	C2dBox extent;
	int num = array->Count();
	for (int idx=0; idx<num; idx++)
	{
		CGeoPoly* poly = (*array)[idx];
		if (poly)
		{ extent += poly->Extent(); }
	}

	return extent;
}

// Expecting only one poly in array; but if more, take largest area.
double CGeoPoly::Area( CGeoPolyArray* array )
{
	double area = 0.0;
	C2dBox extent;
	int num = array->Count();
	for (int idx=0; idx<num; idx++)
	{
		CGeoPoly* poly = (*array)[idx];
		if (poly)
		{ area = max(area, poly->Area()); }
	}

	return area;
}



// ==================================================================
//	Step through the profile checking for sub-loops of closed
// entities (no longer than the specified length) and TRIM THEM
// LIKE THE CANCEROUS POLYPS they are.
//
void CGeoPoly::TrimLoops( int max_loop, double tol )
{
	CGeoCurve*	this_crv;
	CGeoCurve*	next_crv;
	int	num, skip;
	int	kill, kill_idx;
	int	this_idx, next_idx;
	bool trim;

	bool done = false;
	while (!done)
	{
		done = true;

#if BEFORE_2004_05_27
		num = m_curves.Count();
		if (num <= max_loop)
			return;
#else
		// 2004.05.27 (PE) -- Inside kerf offset failed to create a closed loop
		// on a simple rectangular profile when this_idx=0 and next_idx=3.
		//
		// The logic behind this changes follows:
		// Given a four-sided figure, cutting a loop will leave you with
		// a single open curve.  At minimum, you must be left with at least
		// 2 curves (to create a closed profile).  This logic may, of course,
		// be completely flawed :-(
		num = m_curves.Count();
		if (num <= (max_loop+1))
			return;
#endif

		for (this_idx = 0; this_idx < num && done; this_idx++)
		{
			this_crv = m_curves[this_idx];

			for (skip = 2; skip <= max_loop && done; skip++)
			{
				next_idx = (this_idx + skip) % num;
				next_crv = m_curves[next_idx];

				// 2005.01.09 -- The outside kerf of a D-shape was
				// inappropriately modified, partly because the
				// initial offset distance in cad_nester is so small
				// and partly because this algorithm was not appropriately
				// considering "curve adjacency". With this change, we
				// assume any polyp can appear only when curves have
				// a separation greater-than-one (ie. a polyp must have
				// at least two curves).
				if (m_polyp_trim)
				{
					if (next_idx > this_idx)
						trim = (((this_idx + num) - (next_idx + 1)) > 1);
					else
						trim = ((this_idx - (next_idx + 1)) > 1);
				}
				else
				{
					trim = TRUE;
				}

				if (trim)
					trim = this_crv->StartPt().WithinTolXY( next_crv->EndPt(), tol );

				if (trim)
				{
					// Found a closed loop!  Cut it.
					//
					skip--;
					if ((this_idx + skip) >= num)
					{
						for (kill_idx = (num-1); kill_idx > this_idx; kill_idx--)
						{ m_curves.Remove(kill_idx); }

						for (kill_idx = (this_idx+skip)-num; kill_idx >= 0; kill_idx--)
						{ m_curves.Remove(kill_idx); }
					}
					else
					{
						for (kill = skip; kill > 0; kill--)
						{
							kill_idx = (this_idx + kill) % num;
							m_curves.Remove(kill_idx);
						}
					}
					done = false;
				}
			}
		}
	}
}

ElemType CGeoPoly::Type() const
{
	return GEOPOLY;
}

CGeoElem* CGeoPoly::Clone( bool attribs_copy ) const
{
	CGeoPoly* copy = new CGeoPoly();
	(*copy) = (*this);
	return copy;
}

const C3dCoord& CGeoPoly::StartPt() const
{
	int count = m_curves.Count();
	return ((count > 0) ? m_curves[0]->StartPt() : UNDEFINED_PT);
}

void CGeoPoly::StartPt( const C3dCoord& pt )
{
	int count = m_curves.Count();
	if (count > 0)
		m_curves[0]->StartPt( pt );
}

void CGeoPoly::StartPt( double xs, double ys, double zs )
{
	int count = m_curves.Count();
	if (count > 0)
		m_curves[0]->StartPt( xs, ys, zs );
}

const C3dCoord& CGeoPoly::EndPt() const
{
	int count = m_curves.Count();
	return ((count > 0) ? m_curves[count-1]->EndPt() : UNDEFINED_PT);
}

void CGeoPoly::EndPt( const C3dCoord& pt )
{
	int count = m_curves.Count();
	if (count > 0)
		m_curves[count-1]->EndPt( pt );
}

void CGeoPoly::EndPt( double xe, double ye, double ze )
{
	int count = m_curves.Count();
	if (count > 0)
		m_curves[count-1]->EndPt( xe, ye, ze );
}

double CGeoPoly::Length2d() const
{
	double len = 0.;

	int count = m_curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		len += m_curves[indx]->Length2d();
	}

	return len;
}

void CGeoPoly::Xform( const C3x4Matrix& xform )
{
	int num = m_curves.Count();
	for (int idx=0; idx<num; idx++)
	{
		CGeoCurve* curve = m_curves[idx];
		curve->Xform( xform );
	}
	m_extent.Invalidate();
}

void CGeoPoly::Shift( const C3dVec& delta )
{
	int num = m_curves.Count();
	for (int idx=0; idx<num; idx++)
	{
		CGeoCurve* curve = m_curves[idx];
		curve->Shift( delta );
	}
	m_extent.Invalidate();
}

double CGeoPoly::PointClosest( const C3dCoord& pt, C3dCoord* closest ) const
{
	C3dCoord	tmp;
	C3dCoord	best;
	double		u;

	double min_dist = UNDEFINED;

	int count = m_curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = m_curves[indx];

		double dist = curve->PointClosest( pt, &tmp, &u );
		if ((u >= 0.) && (u <= 1.) && (dist < min_dist))
		{
			// ASSUMPTION: The closest point is on this curve.
			min_dist = dist;
			best = tmp;
		}
	}

	(*closest) = best;

	return min_dist;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void CGeoPoly::Tabulate(
	double			chordal_tol,
	const C3dVec&	shift,
	C3dCoordArray*	pts ) const
{
	int count = m_curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = m_curves.GetAt( indx );
		curve->Tabulate( chordal_tol, shift, pts );
	}
}

CGeoPoly* CGeoPoly::ConvexHull( const C3dCoordArray& pts )
{
	CGeoPoly*	poly;
	C3dCoord*	pt;
	double*		tmp;
	int*		ind;
	int			count, indx, jndx;

	// 2006.05.10 (PE) -- Encountered a crash that is difficult to
	// diagnose.  Instead of undertaking the laborious task of
	// understanding the real cause, I discovered that allocating
	// a bit more memory skirts the problem.  Hence, +10.
	count = pts.Count();
	tmp = (double*) malloc( ((2 * count) + 10) * sizeof(double) );
	ind = (int*) malloc( (count + 10) * sizeof( int ) );
	for (indx = 0; indx < count; ++indx)
	{
		pt = pts.GetAt( indx );

		tmp[indx*2] = pt->X();
		tmp[indx*2+1] = pt->Y();

		ind[indx] = indx;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	poly = new CGeoPoly();

	// TODO: Can/Should we implement Convex_Hull_2D() using tGeoPointArray?
	count = Convex_Hull_2D( count, (VECT2*) &tmp[0], ind );
	if (count > 1)
	{
		for (indx = 0; indx < count; ++indx)
		{
			jndx = indx + 1;
			if (jndx >= count)
				jndx = 0;  // wrap

			poly->Append( new CGeoLine(
				C3dCoord( tmp[2*ind[indx]], tmp[2*ind[indx]+1], 0. ),
				C3dCoord( tmp[2*ind[jndx]], tmp[2*ind[jndx]+1], 0. ) ) );
		}
	}

	free( ind );
	free( tmp );

	return poly;
}

// At first implementation, simply dumps end points.
// Yeargh! It would be nice to use the TRACE() macro for this
// but (currently) conflicts occur when using _DEBUG & MSVCRTD.
void CGeoPoly::Trace() const
{
	CReturn trace;
	CString buf;

	int count = m_curves.Count();
	buf.Format( "count: %d", count );
	trace.Diagnostic( (LPCSTR) buf );

	CGeoCurve* geoCurve = NULL;
	for (int indx = 0; indx < count; ++indx)
	{
		geoCurve = m_curves.GetAt( indx );
		const C3dCoord& ps = geoCurve->StartPt();
		buf.Format( "%d) x:%f y:%f\n", indx, ps.X(), ps.Y() );
		trace.Diagnostic( (LPCSTR) buf );
	}

	const C3dCoord& pe = geoCurve->EndPt();
	buf.Format( "%d) x:%f y:%f\n", count, pe.X(), pe.Y() );
	trace.Diagnostic( (LPCSTR) buf );
}

// NOTE: At first implementation, everything is output as lines.
void CGeoPoly::Dump() const
{
	static const char* lineTemplate =
		"Create:Line: action=0, sncs=101, id = 0, xs=%f, ys=%f, zs=0, xe=%f, ye=%f, ze=0\n";

	static const char* arcTemplate =
		"Create:Arc: action=CICREATE, sncs=205, xs=%f, ys=%f, zs=0, xe=%f, ye=%f, ze=0, xc=%f, yc=%f, zc=0, dir=%d\n";

	CString path = CPath::DebugDir() + "\\polydump.log";

	FILE* f = fopen( (LPCSTR) path, "w" );
	if (f != NULL)
	{
		CString buf;

		fprintf( f, "admin:prepare:\n" );

		int count = m_curves.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CGeoCurve* geoCurve = m_curves.GetAt( indx );
			const C3dCoord& ps = geoCurve->StartPt();
			const C3dCoord& pe = geoCurve->EndPt();
			if (geoCurve->Type() == GEOARC)
			{
				CGeoArc* geoArc = dynamic_cast<CGeoArc*>( geoCurve );
				const C3dCoord& pc = geoArc->CenterPt();

				buf.Format( arcTemplate, ps.X(), ps.Y(), pe.X(), pe.Y(), pc.X(), pc.Y(), geoArc->Dir() );
			}
			else
			{
				buf.Format( lineTemplate, ps.X(), ps.Y(), pe.X(), pe.Y() );
			}

			fprintf( f, buf );
		}

		fprintf( f, "admin:commit:\n" );

		fclose( f );
	}
}

void CGeoPolyArray::Draw() const
{
	GeoRendererGet().DrawGeo( (CGeoElemArray&) *this );
}
