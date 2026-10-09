
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"
#include "MathConst.h"

#include "Return.h"
#include "GeoLine.h"
#include "WmElem.h"
#include "WmChain.h"
#include "WmChainIterator.h"
#include "Spiral.h"

#include "WmChainDegouger.h"

static double LIMIT = 1.e-3;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CSpiral::CSpiral()
	: m_stepOver( -UNDEFINED ),
	  m_cwResults( FALSE ),
	  m_insideOut( FALSE ),
	  m_sharpAngle( PI ),
	  m_results()
{
}

CSpiral::~CSpiral()
{
	m_results.DestructiveFlush();
}

CReturn
CSpiral::Init(	const CWmChain&	refChain,
				double			toolDiam,
				double			stepOver,
				double			wallAllow,
				bool			cwResults,
				bool			insideOut,
				double			sharpAngle )
{
	double initialOffset = (toolDiam / 2) + wallAllow;

	return ( Init( refChain, initialOffset, stepOver, cwResults, insideOut, sharpAngle ) );
}

CReturn
CSpiral::Init(	const CWmChain&	refChain,
				double			initialOffset,
				double			stepOver,
				bool			cwResults,
				bool			insideOut,
				double			sharpAngle )
{
	CReturn status;
	CWmChainList results;

	if ( !refChain.IsClosed() )
	{
		status.User( IDS_INTERNAL_ERROR, "Spiral::Init() chain not closed." );
		return status;
	}

	// Private member initialization.
	m_stepOver   = stepOver;
	m_cwResults  = cwResults;
	m_insideOut  = insideOut;
	m_sharpAngle = sharpAngle;
	m_results.DestructiveFlush();

	// The stuff.
	CWmChain copy( refChain );
	double area = copy.Area();

	if (area > 0.0)
	{
		// The outer boundary must always have CW orientation.
		copy.Reverse();
	}

	status = copy.Offset( -1, initialOffset, sharpAngle, &results );

	if ( status.IsOk() )
	{
		double prevArea = fabs( refChain.Area() );

		int count = results.Count();

		for (int indx = 0; indx < count; ++indx)
		{
			CWmChain* chain = results.Remove( 0 );

			double currArea = fabs( chain->Area() );

			if (currArea > prevArea || currArea < LIMIT)
			{
				delete chain;
			}
			else
			{
				// The result will always have CW orientation.
				m_results.Append( chain );

				status = RecursiveCollapse( (*chain) ); 
			}

			if ( !status.IsOk() )
				break;
		}
	}

	if ( status.IsOk() )
		Orient();

	return status;
}

CWmChainList&
CSpiral::Results()
{
	return m_results;
}

// Create Spiral Pocketing toolpath for a planar region.
// Each offset is connected to its adjacent offset by a line.
//
CReturn
CSpiral::RecursiveCollapse( const CWmChain& refChain )
{
	CWmChainList results;

	CReturn status = refChain.Offset( -1, m_stepOver, m_sharpAngle, &results );

	if ( status.IsOk() )
	{
		double prevArea = fabs( refChain.Area() );

		int count = results.Count();

		for (int indx = 0; indx < count; ++indx)
		{
			CWmChain* chain = results.Remove( 0 );

			double currArea = fabs( chain->Area() );

			if ((currArea < prevArea) &&
				(SGN(currArea) == SGN(prevArea)) &&
				 chain->IsC0Continuous( SMALL, TRUE ))
			{
				// The result will always have CW orientation.
				if ( m_insideOut )
				{
					if (indx == 0)
						Connect( (*chain), refChain );

					m_results.Prepend( chain );
				}
				else
				{
					if (indx == 0)
						Connect( refChain, (*chain) );

					m_results.Append( chain );
				}

				status = RecursiveCollapse( (*chain) );
			}
			else
			{
				delete chain;
			}

			if ( !status.IsOk() )
				break;
		}
	}

	return status;
}

CReturn
CSpiral::Connect( const CWmChain& from, const CWmChain& to )
{
	CReturn status;

	CWmChain* connection = new CWmChain();

	C3dCoord ps = from.StartPt();
	C3dCoord pe = to.EndPt();

	CGeoLine* line = new CGeoLine( ps, pe );

	connection->Append( line );

	if ( m_insideOut )
		m_results.Prepend( connection );
	else
		m_results.Append( connection );

	return status;
}

void
CSpiral::Orient()
{
	if ( m_cwResults )
		return;  // Nothing to do because things already have cw orientation.

	int count = m_results.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CWmChain* chain = m_results[ indx ];

		if ( chain->IsClosed() )
		{
			// Conversely, connections will be open.
			chain->Reverse();
		}
	}
}

CReturn
CSpiral::Init(
			CWmChain*		outer,
			CWmChainList*	islands,
			double			outer_offset,
			double			island_offset,
			double			stepOver,
			bool			cwResults,
			bool			insideOut,
			double			sharpAngle )
{
	CReturn				status;
	CGeoElemArray		geo_elems;
	CWmChainDegouger	degouger;
	CWmChainIterator	iter;
	CWmChainList		offset_islands;
	CWmChainList		results;
	CSpiralGraphNode	root(NULL);
	CSpiralGraphNode*	child;
	CWmChain*			containment;
	CWmChain*			island;
	CWmChain*			result_chain;
	CWmElem*			start_wmelem;
	double				prev_area;
	double				curr_area;
	int					count, indx;

	if ( !outer->IsClosed() )
	{
		status.User( IDS_INTERNAL_ERROR, "Spiral::Init() chain not closed." );
		return status;
	}

	// Private member initialization.
	m_stepOver   = stepOver;
	m_cwResults  = cwResults;
	m_insideOut  = insideOut;
	m_sharpAngle = sharpAngle;
	m_results.DestructiveFlush();

	// Force the outer boundary to have CW orientation.
	prev_area = outer->Area();
	if (prev_area > 0.0)
	{
		outer->Reverse();
		prev_area = -prev_area;
	}

	// Force the islands to have CCW orientation.
	count = islands->Count();
	for (indx = 0; indx < count; ++indx)
	{
		island = (*islands)[indx];
		if (island->Area() < 0)
			island->Reverse();
	
		status = island->Offset( -1, island_offset, sharpAngle, &results );
		offset_islands.Append( results.Remove(0) );

		results.DestructiveFlush();
	}

	status = outer->Offset( -1, outer_offset, sharpAngle, &results );
	m_results.Append( outer );
	root.ChildAdd( new CSpiralGraphNode( outer ) );

	count = results.Count();
	for (indx = 0; indx < count; ++indx)
	{
		containment = results[indx];

		curr_area = containment->Area();
		if ((SGN(curr_area) == SGN(prev_area)) &&
			(fabs(curr_area) < fabs(prev_area)))
		{
			degouger.Init( containment, &offset_islands, SMALL );
			
			while (degouger.Count() > 0)
			{
				result_chain = degouger.Results().Remove(0);

				child = new CSpiralGraphNode( result_chain );
				root.ChildGet( root.ChildCount() - 1 )->ChildAdd( child );

				// By pre-appending, the consecutive offsets
				// will be ordered from the outside to the inside.
				m_results.Append( result_chain );
				RecursiveCollapse( result_chain, &offset_islands, child );
			}
		}
	}

	if ( status.IsOk() )
		Orient();

	root.Split( m_stepOver );
	root.Dump(0);

	result_chain = m_results[0];
	iter.Init( (*result_chain) );
	start_wmelem = iter.Elem();
	Link( start_wmelem, &geo_elems );

	// At this point, the actual geometry has been moved from
	// m_results to geo_elems.  As such, we can recover the
	// memory used by m_results and rebuild the chain.
	m_results.DestructiveFlush();

	result_chain = new CWmChain();
	m_results.Append( result_chain );
	count = geo_elems.Count();
	for (indx = 0; indx < count; ++indx)
	{
		result_chain->Append( geo_elems[indx] );
	}

	geo_elems.BenignFlush();

	return status;
}

// TODO: We might gain performance by excluding islands that are outside of outer.
CReturn
CSpiral::RecursiveCollapse(
			CWmChain*			outer,
			CWmChainList*		islands,
			CSpiralGraphNode*	parent )
{
	CReturn				status;
	CWmChainDegouger	degouger;
	CWmChainList		intermediate_results;
	CSpiralGraphNode*	child;
	CWmChain*			containment;
	CWmChain*			result_chain;
	double				prev_area;
	double				curr_area;
	int					count, indx;

	prev_area = outer->Area();

	status = outer->Offset( -1, m_stepOver, m_sharpAngle, &intermediate_results );

	count = intermediate_results.Count();
	for (indx = 0; indx < count; ++indx)
	{
		containment = intermediate_results[indx];

		curr_area = containment->Area();
		if ((SGN(curr_area) == SGN(prev_area)) &&
			(fabs(curr_area) < fabs(prev_area)))
		{
			if (IsEnclosedByIsland( containment, islands ) == false)
			{
				degouger.Init( containment, islands, SMALL );
				
				while (degouger.Count() > 0)
				{
					result_chain = degouger.Results().Remove(0);

					if ( result_chain->IsClosed() )
					{
						child = new CSpiralGraphNode( result_chain );
						parent->ChildAdd( child );

						// By pre-appending, the consecutive offsets
						// will be ordered from the outside to the inside.
						m_results.Append( result_chain );
						RecursiveCollapse( result_chain, islands, child );
					}
					else
					{
						delete result_chain;
					}
				}
			}
		}
	}

	intermediate_results.DestructiveFlush();

	return status;
}

// TODO: We can gain performance by allowing CWmChain to cache the CGeoPoly.
bool
CSpiral::IsEnclosedByIsland(
				CWmChain*		outer,
				CWmChainList*	islands )
{
	CWmChain*	island;
	int			count, indx;

	count = islands->Count();
	for (indx = 0; indx < count; ++indx)
	{
		island = (*islands)[indx];
		if (island->Encloses( (*outer) ) == true)
			return true;
	}

	return false;
}

void
CSpiral::Link(
			CWmElem*		start_wmelem,
			CGeoElemArray*	geo_elems )
{
	CWmChain*		chain;
	CWmElem*		curr_wmelem;
	CWmElem*		adj_start_wmelem;
	CGeoElem*		geo_elem;
	C3dCoord		ps;
	C3dCoord		pe;
	const CVarList*	attribs;
	int				address;

	curr_wmelem = start_wmelem;
	chain = curr_wmelem->Owner()->Owner();

	while (1)
	{
		geo_elem = curr_wmelem->Elem();

		attribs = geo_elem->pAttrib();
		if (attribs != NULL)
		{
			address = attribs->getInt( "_link_inward", 0 );
			if (address != 0)
			{
				adj_start_wmelem = (CWmElem*) address;

				ps = geo_elem->StartPt();
				pe = adj_start_wmelem->Elem()->StartPt();

				geo_elems->Append( new CGeoLine( ps, pe ) );

				Link( adj_start_wmelem, geo_elems );

				geo_elems->Append( new CGeoLine( pe, ps ) );
			}
		}
		else
		{
			geo_elems->Append( geo_elem->Clone( false ) );
		}

		curr_wmelem = dynamic_cast<CWmElem*>( curr_wmelem->Next() );
		if (curr_wmelem == NULL)
		{
			CWmChainIterator iter;

			// ASSUMPTION: We are at the end of the chain.
			iter.Init( (*chain) );
			curr_wmelem = iter.Elem();
			if (curr_wmelem == NULL)
				break;  // should not happen. but ...
		}

		if (curr_wmelem == start_wmelem)
			break;  // We've come full circle.
	}
}
