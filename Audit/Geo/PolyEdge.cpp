// ==================================================================
//	PolyEdge
//
//	Slave utility class of CGeoPoly and CPolySegment.  A list manager
//	to keep track of various curves; includes a variety of ways of
//	adding a new "edges" to the list.
//
//	Maintains a list of curves (er, edges) in an un-ordered list
//
// ==================================================================

#include <stdafx.h>

#include "PolyEdge.h"
#include "GeoPoly.h"

//---------------------------------------------------------------------------
CPolyEdge::CPolyEdge()
{
	m_attribs = NULL;
}

//---------------------------------------------------------------------------
CPolyEdge::~CPolyEdge ()
{
	Reset();
}

//---------------------------------------------------------------------------
void CPolyEdge::AddEdge( const CGeoCurve& edge )
{
    // check if edge is already in list
/*
	int num = m_edge_list.Count();
	for (int idx=0; idx<num; idx++)
	{
		CGeoCurve* curve = m_edge_list[idx];

		if (within_tol( edge, *curve, SMALL ))
		{ 
			// TODO:  Override the user data, data over null?  Do we ever really get repeats?
			return;
		}
	}
*/

	if (CReturn::Debug()>=9)
	{
		CReturn ret;
		CString note;
		note.Format( "%x: edge: %f, %f to %f, %f", ((long)this)&0xff, edge.StartPt().X(), edge.StartPt().Y(), edge.EndPt().X(), edge.EndPt().Y() );
		ret.Diagnostic( note );
	}

	CGeoCurve* new_edge = (CGeoCurve*) edge.Clone( false );
	if (m_attribs != NULL)
		*(new_edge->pAttrib()) = (*m_attribs);

	new_edge->UserData( edge.UserData() );
	m_edge_list.Append( new_edge );
}

//---------------------------------------------------------------------------
bool CPolyEdge::within_tol( const CGeoCurve& c1, const CGeoCurve& c2, double tol )
{
	if (c1.Type() != c2.Type())
		return FALSE;

	switch (c1.Type())
	{
	case GEOARC:
		{
			CGeoArc* a1 = (CGeoArc*)&c1;
			CGeoArc* a2 = (CGeoArc*)&c2;

			if ( a1->CenterPt().WithinTolXY( a2->CenterPt(), SMALL )
				&& c1.StartPt().WithinTolXY(c2.StartPt(), SMALL)
				&& c1.EndPt().WithinTolXY(c2.EndPt(), SMALL) )
			{ 
				return TRUE;
			}
		}
		break;

	case GEOLINE:
		if ( c1.StartPt().WithinTolXY(c2.StartPt(), SMALL)
			&& c1.EndPt().WithinTolXY(c2.EndPt(), SMALL) )
		{
			return TRUE;
		}
		break;

	}

	return FALSE;
}

//---------------------------------------------------------------------------
void CPolyEdge::MergeAppend( CPolyEdge& segedge, bool reverse )
{
	if ( segedge.Count() )
	{
		int num = segedge.Count();
		for (int idx=0; idx<num; idx++)
		{
			CGeoCurve* edge = segedge[idx];

			CGeoCurve* new_edge = (CGeoCurve*)edge->Clone( true );
			new_edge->UserData( edge->UserData() );

			if (reverse)
				new_edge->Reverse();

			m_edge_list.Append( new_edge );
		}

		segedge.Reset();
	}
}


//---------------------------------------------------------------------------
void CPolyEdge::MergeUnique( CPolyEdge& segedge, bool reverse )
{
	if ( segedge.Count() )
	{
		int snum = segedge.Count();
		for (int sidx=0; sidx<snum; sidx++)
		{
			CGeoCurve* scurve = segedge[sidx];

			// check if edge is already in list
			bool unique = TRUE;
			int lnum = m_edge_list.Count();
			for (int lidx=0; lidx<lnum; lidx++)
			{
				CGeoCurve* lcurve = m_edge_list[lidx];

				if (within_tol(*scurve, *lcurve, SMALL))
				{
					unique = FALSE;
					break;
				}
			}

			if (unique)
			{ 
				CGeoCurve* new_scurve = (CGeoCurve*)scurve->Clone( true );
				new_scurve->UserData( scurve->UserData() );

				if (reverse)
				{ new_scurve->Reverse(); }

				m_edge_list.Append( new_scurve );
			}
		}

		segedge.Reset();
	}
}

//---------------------------------------------------------------------------
void CPolyEdge::MergeEqual( CPolyEdge& segedge )
{
	if ( segedge.Count() )
	{
		CGeoCurveList equal_list;

		int snum = segedge.Count();
		for (int sidx=0; sidx<snum; sidx++)
		{
			CGeoCurve* scurve = segedge[sidx];

			// check if edge is already in list
			int lnum = m_edge_list.Count();
			for (int lidx=0; lidx<lnum; lidx++)
			{
				CGeoCurve* lcurve = m_edge_list[lidx];

				if (within_tol( *scurve, *lcurve, SMALL ))
				{
					// Must clone, because the original gets destructed
					CGeoCurve* new_scurve = (CGeoCurve*)scurve->Clone( true );
					new_scurve->UserData( scurve->UserData() );

					equal_list.Append( new_scurve );
					break;
				}
			}
		}

		Reset();
		segedge.Reset();

		// TODO:  Add something more efficient than AddEdge() for this
		int num = equal_list.Count();
		for (int idx=0; idx<num; idx++)
		{
			m_edge_list.Append( (equal_list[idx]) );
		}
		equal_list.BenignFlush();
	}
}
