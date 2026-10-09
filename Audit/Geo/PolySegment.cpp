// ==================================================================
//		CPolySegment
//
//	Slave utility class of CGeoPoly.  Used during segmentation of
//	a curve with respect to a poly.
//
//	Preserves a list of "hit" points in order along the curve.
//
// ==================================================================

#include <stdafx.h>

#include "PolySegment.h"
#include "Solution.h"
#include "Int2d.h"

// ==================================================================

eSegTag CPolySegment::m_klein4[4][4] =
{
    { OTAG, ITAG, MTAG, PTAG },
    { ITAG, OTAG, PTAG, MTAG },
    { MTAG, PTAG, OTAG, ITAG },
    { PTAG, MTAG, ITAG, OTAG }
};

//---------------------------------------------------------------------------
// NOTE:  Arcs must be monotonic; eg. quadrant arcs, otherwise the intersection test
//		will not work correctly
//
CPolySegment::CPolySegment( const CGeoCurve& curve ) 
	: m_curve( curve )
{
	m_line.StartPt( m_curve.StartPt() );
	m_line.EndPt( m_curve.EndPt() );

	// TODO: Not real happy about this ....
	m_attribs = (curve.HasAttrib() ? ((CGeoCurve&) curve).pAttrib() : NULL);
}

CPolySegment::~CPolySegment()
{
	m_tpnt_list.DestructiveFlush();
}

//---------------------------------------------------------------------------
void CPolySegment::insert_pnt( const C2dCoord& pnt, eSegTag tag )
{
	double uparam = m_line.PointUparam( pnt );

	CReturn ret;
	CString note;

	int idx, num = m_tpnt_list.Count();
	for (idx=0; idx<num; idx++)
	{
		sTaggedPoint* tpnt = m_tpnt_list[idx];
		if (EQUAL( tpnt->m_uparam, uparam ))
		{
			if (CReturn::Debug()>=9)
			{
				note.Format( "      override (%f, %f tag %d) with tag %d (to %d)", tpnt->m_pnt.X(), tpnt->m_pnt.Y(), tpnt->m_tag, tag, m_klein4[tag][tpnt->m_tag]);
				ret.Diagnostic( note );
			}

			tpnt->m_tag = m_klein4[tag][tpnt->m_tag];
			return;
		}
		else
		if ( (uparam + SMALL) < tpnt->m_uparam )
			break;
	}

	if (CReturn::Debug()>=9)
	{
		note.Format( "      insert %f, %f (%d) at %f", pnt.X(), pnt.Y(), tag, uparam );
		ret.Diagnostic( note );
	}

    // point not in segmentation, add it
    sTaggedPoint* new_tpnt = new sTaggedPoint;
	new_tpnt->m_uparam = uparam;
	new_tpnt->m_pnt = pnt;
	new_tpnt->m_tag = tag;
	if ( (!num)
		|| (idx >= num) )
	{
		m_tpnt_list.Append( new_tpnt );
	}
	else
	{
		m_tpnt_list.InsertBefore( idx, new_tpnt );
	}
}

//---------------------------------------------------------------------------
void CPolySegment::SegmentBy( const CGeoPoly& poly )
{
	CReturn ret;
	CString note;
	if (CReturn::Debug()>=9)
	{
		note.Format( "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );
		ret.Diagnostic( note );
		note.Format( "* curve: %f, %f to %f, %f", m_curve.StartPt().X(), m_curve.StartPt().Y(), m_curve.EndPt().X(), m_curve.EndPt().Y() );
		ret.Diagnostic( note );
	}

	C2dCoord hit[4];
	eSegTag tag = ITAG;

	int num = poly.Count();
	for (int idx=0; idx<num; idx++)
	{
		const CGeoCurve& edge = poly[idx];

		int st_side = m_curve.PointSide( edge.StartPt(), VECTOR_SMALL );
		int en_side = m_curve.PointSide( edge.EndPt(), VECTOR_SMALL );

		// TODO:  REPAIR FOR ARCS... the on-curve tests won't work for 
		// arcs of different radii or arcs on lines
		//
		// Test the nine combinations of left/right/on...
		int hitnum = 0;
		tag = ITAG;

		C3dCoord midpt;
		if (((m_curve.Type() == GEOARC) || (edge.Type() == GEOARC)) && !(st_side == -en_side))
		{
			hitnum = do_intersect( edge, hit , FALSE );
			if (hitnum == 2)
			{
				C3dCoord pt1 = hit[0];
				C3dCoord pt2 = hit[1];

				if (m_curve.Type() == GEOARC)
				{
					// This curve is an arc; external curve is a line
					midpt = C3dCoord( (pt1.X() + pt2.X()) * 0.5,
										(pt1.Y() + pt2.Y()) * 0.5,
										(pt1.Z() + pt2.Z()) * 0.5 );
				}
				else
				{
					C3dCoord tmp = C3dCoord( (pt1.X() + pt2.X()) * 0.5,
											(pt1.Y() + pt2.Y()) * 0.5,
											(pt1.Z() + pt2.Z()) * 0.5 );

					double uparam;

					edge.PointClosest( tmp, &midpt, &uparam );
				}
				
				int mid_side = m_curve.PointSide( midpt, VECTOR_SMALL );
				tag = ((mid_side < 0) ? MTAG : PTAG);
			}
			else
			if (hitnum == 1)
			{
				if ( !hit[0].WithinTol(edge.StartPt(), SEGMENT_TOL)
					&& !hit[0].WithinTol(edge.EndPt(), SEGMENT_TOL) )
				{
					if (!st_side)
					{ st_side = -en_side; }
					else
					{ en_side = -st_side; }
				}
				hitnum = 0;
				tag = ITAG;
			}
			else
			{ 
				hitnum = 0;
				tag = ITAG;
			}

		}

		if (CReturn::Debug()>=9)
		{
			note.Format( "    : edge %f, %f (%d) to %f, %f (%d)", edge.StartPt().X(), edge.StartPt().Y(), st_side, edge.EndPt().X(), edge.EndPt().Y(), en_side );
			ret.Diagnostic( note );
		}

		if (st_side > 0)
		{
			if (en_side > 0)
			{
				// Both ends on same side of curve; probably does not intersect
				tag = ITAG;
			}
			else
			if (en_side < 0)
			{
				// Edge may intersect curve; split it.
				hitnum = do_intersect( edge, hit, TRUE );
			}
			else // en_side == zero
			{
                // End point of edge is on curve
				if (!hitnum)
					insert_pnt( edge.EndPt(), PTAG );
			}
		}
		else
		if (st_side < 0)
		{
			if (en_side > 0)
			{
				// Edge intersects line; split it.
				hitnum = do_intersect( edge, hit, TRUE );
			}
			else
			if (en_side < 0)
			{
				// Both ends on same side of curve; probably does not intersect
				tag = ITAG;
			}
			else // en_side == zero
			{
                // End point of edge is on line
				if (!hitnum)
					insert_pnt( edge.EndPt(), MTAG );
			}
		}
		else // st_side == zero
		{
			if (en_side > 0)
			{
                // Start point of edge is on line
				if (!hitnum)
					insert_pnt( edge.StartPt(), PTAG );
			}
			else
			if (en_side < 0)
			{
                // Start point of edge is on line
				if (!hitnum)
	                insert_pnt( edge.StartPt(), MTAG );
			}
			else // en_side == zero
			{
				// Edge is co-incident with curve... make sure it represents
				// true equality, otherwise test the center of the curve
				// A 3-point test gives *true* equality for curves
				if ( (edge.Type() == GEOARC) || (m_curve.Type() == GEOARC) )
				{
					// Okay, if the midpoint doesn't fall on the edge,
					// then we need to set the endpoints to something different
					C2dCoord midpt = m_curve.MidPt();
					int midside = edge.PointSide(midpt, VECTOR_SMALL);
					if (midside<0)
					{
			            insert_pnt( edge.StartPt(), PTAG );
						insert_pnt( edge.EndPt(), PTAG );
					}
					else
					if (midside>0)
					{
			            insert_pnt( edge.StartPt(), MTAG );
						insert_pnt( edge.EndPt(), MTAG );
					}

					hitnum = 0;
				}
			}
		}

		while (--hitnum>=0)
		{ 
			if ( (hit[hitnum].WithinTol(edge.StartPt(), SMALL) )
				|| (hit[hitnum].WithinTol(edge.EndPt(), SMALL) ) )
			{ insert_pnt( hit[hitnum], tag ); }
			else
			{ insert_pnt( hit[hitnum], ITAG ); }
		}
	}
}

int
CPolySegment::do_intersect( 
		const CGeoCurve& edge,
		C2dCoord		hit[],
		bool			extended )
{
	CInt2d intsect;

	// The curve is treated as an infinite line, since we need to have intersections
	// that extend all the way to the edge of the shape.  However, the arc curve
	// doesn't work that way!  So, we treat arcs as if they were an infinite line with
	// a slight bubble in the middle of it... the arc.  We are guaranteed that our arcs
	// have a maximum arc angle of 90', so this works pretty good.
	//d)
	int idx, num = 0;

	if (m_curve.Type() == GEOARC)
	{
		// Source curve is an Arc... treat as extended arc
		// Dest curve may be arc or line.
		//
		// First, the source arc is tested... forced onseg for all parties
		//
		intsect.OnSeg(TRUE);
		int hitnum = intsect.CrvCrv( m_curve, edge );
		for (idx=0; idx<hitnum; idx++)
		{
			C2dCoord pt = intsect.Point(idx);
			hit[num++] = pt;
		}

		// May be one more hit on the line... 
		// If so, it enforces onseg for any destination arcs
		if (extended)
		{
			C2dCoord hit2[2];
			intsect.OnSeg(FALSE);
			hitnum = intsect.CrvCrv( m_line, edge );
			for (idx=0; idx<hitnum; idx++)
			{
				if (edge.Type() == GEOARC)
				{
					if ( (intsect.Vparam(idx) < (-SMALL))
						|| (intsect.Vparam(idx) > (1+SMALL)) )
					{ continue; }
				}

				// Accept a hit if it is off the curve bubble
				if ( (intsect.Uparam(idx) < (-SMALL))
					|| (intsect.Uparam(idx) > (1+SMALL)) )
				{
					C2dCoord pt = intsect.Point(idx);
					hit[num++] = pt;
				}
			}
		}
	}
	else
	{
		// Source curve is line
		// Dest curve may be arc or line
		//
		// If dest is arc, then we force onseg on the arc.  Don't need to
		// extend the arc, since it is extended naturally by it's remaining
		// profile.
		//
		intsect.OnSeg( FALSE );
		int hitnum = intsect.CrvCrv( m_curve, edge );
		for (int idx=0; idx<hitnum; idx++)
		{
			if (edge.Type() == GEOARC)
			{
				if ( (intsect.Vparam(idx) < (-SMALL))
					|| (intsect.Vparam(idx) > (1+SMALL)) )
				{ continue; }
			}

			C2dCoord pt = intsect.Point(idx);
			hit[num++] = pt;
		}
	}

	return num;
}


//---------------------------------------------------------------------------
void CPolySegment::Reduce( const CGeoCurve& curve )
{
	double st_uparam = m_line.PointUparam( curve.StartPt() );

	CReturn ret;
	CString note;

    eSegTag tag = OTAG;

	if (CReturn::Debug()>=9)
	{
		note.Format( "--------------------------------------------" );
		ret.Diagnostic( note );
		note.Format( "    s-tag %d (%f)", tag, st_uparam );
		ret.Diagnostic( note );
	}

    while ( m_tpnt_list.Count() )
    {
		sTaggedPoint* tpnt = m_tpnt_list[0];
		if (!tpnt)
			return;

		if (st_uparam < (tpnt->m_uparam-SMALL))
		{
			sTaggedPoint* new_tpnt = new sTaggedPoint;
			new_tpnt->m_uparam = st_uparam;
			new_tpnt->m_pnt = curve.StartPt();
			new_tpnt->m_tag = tag;

			if (CReturn::Debug()>=9)
			{
				note.Format( "    start tag %d", tag );
				ret.Diagnostic( note );
			}
			m_tpnt_list.Prepend( new_tpnt );
			break;
		}

		if (CReturn::Debug()>=9)
		{
			note.Format( "    skip %f (%f, %f) tag %d (to %d)", tpnt->m_uparam, tpnt->m_pnt.X(), tpnt->m_pnt.Y(), tpnt->m_tag, m_klein4[tag][tpnt->m_tag]);
			ret.Diagnostic( note );
		}

        tag = m_klein4[tag][tpnt->m_tag];
		m_tpnt_list.Remove( 0 );
		delete tpnt;
    }

	// ---------------------------------

    double en_uparam = m_line.PointUparam( curve.EndPt() );

    tag = OTAG;
	if (CReturn::Debug()>=9)
	{
		note.Format( "    e-tag %d (%f)", tag, en_uparam );
		ret.Diagnostic( note );
	}
    while ( m_tpnt_list.Count() )
    {
		int num = m_tpnt_list.Count();
		if (!num)
			return;

		num--;
		sTaggedPoint* tpnt = m_tpnt_list[num];

		// Ordering
        if ( en_uparam > (tpnt->m_uparam+SMALL) )
        {
            sTaggedPoint* new_tpnt = new sTaggedPoint;
			new_tpnt->m_uparam = en_uparam;
			new_tpnt->m_pnt = curve.EndPt();
			new_tpnt->m_tag = tag;

			if (CReturn::Debug()>=9)
			{
				note.Format( "    end tag %d", tag );
				ret.Diagnostic( note );
			}
			m_tpnt_list.Append( new_tpnt );
            break;
        }

		if (CReturn::Debug()>=9)
		{
			note.Format( "    skip %f (%f, %f) tag %d (to %d)", tpnt->m_uparam, tpnt->m_pnt.X(), tpnt->m_pnt.Y(), tpnt->m_tag, m_klein4[tag][tpnt->m_tag]);
			ret.Diagnostic( note );
		}

        tag = m_klein4[tag][tpnt->m_tag];
		m_tpnt_list.Remove( num );
		delete tpnt;
    }

	if (CReturn::Debug()>=9)
	{
		note.Format( "- - - - - - - - - - - - - - - - - - - - - - " );
		ret.Diagnostic( note );
		for (int idx=0; idx<m_tpnt_list.Count(); idx++)
		{
			sTaggedPoint* tpnt = m_tpnt_list[idx];
			note.Format( "    (%f, %f [%d] at %f)", tpnt->m_pnt.X(), tpnt->m_pnt.Y(), tpnt->m_tag, tpnt->m_uparam );
			ret.Diagnostic( note );
		}
		note.Format( "--------------------------------------------" );
		ret.Diagnostic( note );
	}

}


//---------------------------------------------------------------------------
void CPolySegment::ConvertToEdges( CPolyEdge segedge[4] )
{
	switch (m_curve.Type())
	{
	case GEOARC:
		convert_to_edges( *((CGeoArc*)&m_curve), segedge );
		break;
	case GEOLINE:
		convert_to_edges( *((CGeoLine*)&m_curve), segedge );
		break;
	}
}

void CPolySegment::convert_to_edges( const CGeoArc& arc, CPolyEdge segedge[4] )
{
	CGeoArc	arcseg(arc);

	arcseg.UserData( arc.UserData() );

	int num = m_tpnt_list.Count();
	sTaggedPoint* at_pt = m_tpnt_list[0];

	eSegTag tag = OTAG;
	for (int idx=1; idx<num; idx++)
	{
		sTaggedPoint* next_pt = m_tpnt_list[idx];

		if (CReturn::Debug()>=9)
		{
			CString note;
			note.Format( " (tag %d mod by %d to %d)", tag, at_pt->m_tag, m_klein4[tag][at_pt->m_tag] );
			CReturn ret;
			ret.Diagnostic( note );
		}

		tag = m_klein4[tag][at_pt->m_tag];

		arcseg.Init( at_pt->m_pnt, next_pt->m_pnt, arcseg.CenterPt(), arcseg.Dir() );

		segedge[tag].Attribs( m_attribs );
		segedge[tag].AddEdge( arcseg );

		at_pt = next_pt;
	}
}


void CPolySegment::convert_to_edges( const CGeoLine& line, CPolyEdge segedge[4] )
{
	CGeoLine lineseg(line);

	lineseg.UserData( line.UserData() );

	int num = m_tpnt_list.Count();
	sTaggedPoint* at_pt = m_tpnt_list[0];

	eSegTag tag = OTAG;
	for (int idx=1; idx<num; idx++)
	{
		sTaggedPoint* next_pt = m_tpnt_list[idx];

		if (CReturn::Debug()>=9)
		{
			CString note;
			note.Format( " (tag %d mod by %d to %d)", tag, at_pt->m_tag, m_klein4[tag][at_pt->m_tag] );
			CReturn ret;
			ret.Diagnostic( note );
		}

		tag = m_klein4[tag][at_pt->m_tag];

		lineseg.StartPt( at_pt->m_pnt );
		lineseg.EndPt( next_pt->m_pnt );

		segedge[tag].Attribs( m_attribs );
		segedge[tag].AddEdge( lineseg );

		at_pt = next_pt;
	}
}

