
#include "stdafx.h"
#include "Return.h"
#include "Register.h"
#include "GeoCurve.h"
#include "GeoLine.h"
#include "WmChainIterator.h"
#include "WmElem.h"
#include "SpiralGraphNode.h"

CSpiralGraphNode::CSpiralGraphNode( CWmChain* chain )
{
	m_chain = chain;
}

CSpiralGraphNode::~CSpiralGraphNode()
{
	m_chain = NULL;
	m_children.DestructiveFlush();
	m_pts.DestructiveFlush();
}

int
CSpiralGraphNode::ChildCount()
{
	return m_children.Count();
}

void
CSpiralGraphNode::ChildAdd( CSpiralGraphNode* child )
{
	m_children.Append( child );
}

CSpiralGraphNode*
CSpiralGraphNode::ChildGet( int indx )
{
	return m_children[indx];
}

void
CSpiralGraphNode::Split( double dist )
{
	tClosestPointPair	cp_pair;
	CSpiralGraphNode*	child;
	int	count, indx;

	count = m_children.Count();
	for (indx = 0; indx < count; ++indx)
	{
		child = m_children[indx];

		if (m_chain != NULL)
		{
			cp_pair = Split( (*m_chain), (*(child->m_chain)), dist );

			this->m_pts.Append( new C3dCoord( cp_pair.pt_parent ) );
			this->m_wmelems.Append( cp_pair.wmelem_parent );

			child->m_pts.Append( new C3dCoord( cp_pair.pt_child ) );
			child->m_wmelems.Append( cp_pair.wmelem_child );
		}

		child->Split( dist );
	}
}

tClosestPointPair
CSpiralGraphNode::Split(
						const CWmChain&	parent,
						const CWmChain&	child,
						double			dist )
{
	tClosestPointPair	result;
	CWmChainIterator	iter_parent;
	CWmChainIterator	iter_child;
	CWmElem*			wmelem_parent;
	CWmElem*			wmelem_child;
	CGeoCurve*			curve_parent;
	CGeoCurve*			curve_child;
	C3dCoord			pt_parent;
	C3dCoord			pt_child;
	double				dist_between;
	double				abs_diff, min_diff;

	result.is_valid = false;
	min_diff = UNDEFINED;  // for debugging only.

	iter_parent.Init( parent );
	iter_child.Init( child );

	while (1)
	{
		if ( iter_parent.AtEnd() )
			break;

		wmelem_parent = iter_parent.Elem();
		curve_parent = (CGeoCurve*) wmelem_parent->Elem();

		iter_child.GotoStart();
		while (1)
		{
			if ( iter_child.AtEnd() )
				break;

			wmelem_child = iter_child.Elem();
			curve_child = (CGeoCurve*) wmelem_child->Elem();

			dist_between = ClosestPoints(
				(*curve_parent), (*curve_child), &pt_parent, &pt_child );

			abs_diff = fabs(dist_between - dist);
			if (abs_diff < min_diff)
				min_diff = abs_diff;

			if (abs_diff < 0.001)
			{
				Split( wmelem_parent, pt_parent, wmelem_child, pt_child );

				result.pt_parent = pt_parent;
				result.wmelem_parent = wmelem_parent;

				result.pt_child = pt_child;
				result.wmelem_child = wmelem_child;

				result.is_valid = true;
				break;
			}

			iter_child.NextElem();
		}

		if (result.is_valid)
			break;

		iter_parent.NextElem();
	}

	return result;
}

// Rudimentary check ... can be made to be MUCH more robust.
double
CSpiralGraphNode::ClosestPoints(
					const CGeoCurve&	curveA,
					const CGeoCurve&	curveB,
					C3dCoord*			ptA,
					C3dCoord*			ptB )
{
	double dist;

	// ASSUMPTION: If attributes exist then the curve must be a
	// "link".That is the curve is a zero-length line having either
	// a "_link_inward" of "_from" attribute.

	if ( !curveA.HasAttrib() && !curveB.HasAttrib() )
	{
		(*ptB) = curveB.MidPt();
		dist =curveA.PointClosest( (*ptB), ptA, NULL );
	}
	else
	{
		dist = UNDEFINED;
	}

	return dist;
}

void
CSpiralGraphNode::Split(
					CWmElem*		from_wmelem,
					const C3dCoord&	from_pt,
					CWmElem*		to_wmelem,
					const C3dCoord&	to_pt )
{
	CGeoElem*	from_elem;
	CGeoElem*	to_elem;
	CWmElem*	trail_wmelem;
	CGeoElem*	lead_elem;
	CGeoElem*	trail_elem;
	CWmElem*	from_link_wmelem;
	CWmElem*	to_link_wmelem;
	CGeoLine*	from_link_pt;
	CGeoLine*	to_link_pt;

	from_elem = from_wmelem->Elem();
	to_elem = to_wmelem->Elem();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Prepare the link data.

	// NOTE: Though we are creating zero-length lines here,
	// they are filtered out during the linking process.

	from_link_pt = new CGeoLine( from_pt, from_pt );
	from_link_wmelem = new CWmElem( from_wmelem->Owner(), from_link_pt );

	to_link_pt = new CGeoLine( to_pt, to_pt );
	to_link_wmelem = new CWmElem( to_wmelem->Owner(), to_link_pt );

	from_link_pt->IntSet( "_link_inward", (int) to_link_wmelem );
	to_link_pt->IntSet( "_from", (int) from_link_wmelem );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Insert the link data into the parent chain.

	if ( from_pt.WithinTolXY( from_elem->StartPt(), SMALL ) )
	{
		from_wmelem->Prev( from_link_wmelem );
	}
	else if (from_pt.WithinTolXY( from_elem->EndPt(), SMALL ) )
	{
		from_wmelem->Next( from_link_wmelem );
	}
	else
	{
		lead_elem = from_wmelem->Elem();
		trail_elem = lead_elem->Clone( false );

		lead_elem->EndPt( from_pt );
		trail_elem->StartPt( from_pt );

		trail_wmelem = new CWmElem( from_wmelem->Owner(), trail_elem );

		from_wmelem->Next( from_link_wmelem );
		from_link_wmelem->Next( trail_wmelem );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Insert the link data into the child chain.

	if ( to_pt.WithinTolXY( to_elem->StartPt(), SMALL ) )
	{
		to_wmelem->Prev( to_link_wmelem );
	}
	else if (to_pt.WithinTolXY( to_elem->EndPt(), SMALL ) )
	{
		to_wmelem->Next( to_link_wmelem );
	}
	else
	{
		lead_elem = to_wmelem->Elem();
		trail_elem = lead_elem->Clone( false );

		lead_elem->EndPt( to_pt );
		trail_elem->StartPt( to_pt );

		trail_wmelem = new CWmElem( to_wmelem->Owner(), trail_elem );

		to_wmelem->Next( to_link_wmelem );
		to_link_wmelem->Next( trail_wmelem );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//Reseed( from_wmelem );
	//Reseed( to_wmelem );
}

void
CSpiralGraphNode::Reseed( CWmElem* wmelem )
{
	CWmChainIterator	iter;
	CWmElem*	start_wmelem;
	CGeoElem*	geo_elem;

	// Reseed the chain as necessary.
	CWmChain* chain = wmelem->Owner()->Owner();

	bool reseeded = (chain->IntGet( "_reseeded", FALSE ) != FALSE);
	if ( !reseeded )
	{
		iter.Init( (*chain) );

		while (1)
		{
			iter.GotoStart();
			start_wmelem = iter.Elem();

			geo_elem = start_wmelem->Elem();

			if ( geo_elem->HasAttrib() )
			{
				const CVarList& attribs = geo_elem->Attrib();

				if (attribs.getInt( "_link_inward", 0 ) != 0)
					break;

				if (attribs.getInt( "_from", 0 ) != 0)
					break;
			}

			// Move the starting entity to the end of the chain.

			start_wmelem->Elem( NULL );

			start_wmelem->Unlink();
			delete start_wmelem;

			chain->Append( geo_elem );
		}

		chain->IntSet( "_reseeded", TRUE );
	}
}


void
CSpiralGraphNode::Dump( int level )
{
	CReturn		ewm;
	CString		white_space(' ',(level*4));
	CString		msg;
	C3dCoord*	pt;
	int			count, indx;

	if (!CRegister::Debug("SpiralGraphNode"))
		return;

	msg.Format( "%schain:%0x", white_space, m_chain );
	ewm.Diagnostic( msg );

	if (m_chain != NULL)
		m_chain->Debug("FOO");

	count = m_pts.Count();
	for (indx = 0; indx < count; ++indx)
	{
		pt = m_pts[indx];
		msg.Format( "%spt x:%f y:%f", white_space, pt->X(), pt->Y() );
		ewm.Diagnostic( msg );
	}

	count = m_children.Count();
	if (count > 0)
	{
		msg.Format( "%schildren:%d", white_space, count );
		ewm.Diagnostic( msg );

		for (indx = 0; indx < count; ++indx)
		{
			m_children[indx]->Dump( level+1 );
		}
	}
}
