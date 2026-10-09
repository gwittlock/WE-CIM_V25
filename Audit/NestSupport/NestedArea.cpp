
#include "assert.h"
#include "GeoPoly.h"
#include "NestedArea.h"

static int DBG6 = 0;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CNestedArea::CNestedArea( const CPartPlace& part_place )
{
	m_toolhit = new CToolHit();

	// Because m_toolhit->Pos() seems to get reset to (UNDEFINED,UNDEFINED).
	m_handle = part_place.WorldPointGet();

	m_toolhit->PartialCopy( (*(part_place.ToolHitGet())), m_handle );

	m_packing = NULL;
}

CNestedArea::~CNestedArea()
{
	m_interior_areas.DestructiveFlush();
	delete m_packing;
	delete m_toolhit;
}


CNestingPart*
CNestedArea::PartGet() const
{
	return ( m_toolhit->Part() );
}

CToolHit*
CNestedArea::ToolHitGet() const
{
	return m_toolhit;
}

C2dCoord
CNestedArea::HandlePointGet() const
{
	return m_handle;
}

int
CNestedArea::InteriorAreasCount() const
{
	return ( m_interior_areas.Count() );
}

CNestedArea*
CNestedArea::InteriorAreaGet( int indx ) const
{
	CNestedArea*	nested_area = NULL;

	if ((indx >= 0) && (indx < m_interior_areas.Count()))
		nested_area = m_interior_areas.GetAt( indx );

	return nested_area;
}

void
CNestedArea::InteriorAreaAdd( CNestedArea* nested_area )
{
	m_interior_areas.Append( nested_area );
}

CGeoPoly*
CNestedArea::HolePolyGet() const
{
	CGeoPolyArray*	polys;
	CGeoPoly*		the_poly;
	int				count;

	the_poly = NULL;
	
	polys = m_toolhit->PolysGet(HIT_PART_INSIDE);
	if (polys != NULL)
	{
		// TODO: This will have to change when we address
		// nested parts that have multiple holes.
		count = polys->Count();
		if (count > 0)
			the_poly = polys->GetAt(0);
	}

	return the_poly;
}


CGeoPolyArray*
CNestedArea::PolysGet(int depth) const
{
	return ( m_toolhit->PolysGet( depth ) );
}


double
CNestedArea::Pack(
	bool				do_packing,
	const CPartPlace&	part_place,
	eNestProgression	progression,
	CViewMgr&			view )
{
	CGeoPoly*	part_shadow;
	double		area;

	part_shadow = PartShadow( part_place, progression );
	PolyRender( (*part_shadow) );

	if (m_packing == NULL)
	{
		area = fabs( part_shadow->Area() );

		if ( do_packing )
		{
			m_packing = part_shadow;
			m_packing->DoubleSet( "area", area );
		}
		else
		{
			delete part_shadow;
		}
	}
	else
	{
		CGeoPoly tmp;
		tmp = (*m_packing);

		PolyRender( tmp );  // Before boolean

		tmp.OR( (*part_shadow) );
		delete part_shadow;

		PolyRender( tmp );  // After boolean

		area = fabs( tmp.Area() );

		if ( do_packing )
		{
			tmp.LinesReduce( true, SMALL );
			PolyRender( tmp );  // After boolean
			(*m_packing) = tmp;
			m_packing->DoubleSet( "area", area );
		}
	}

	return area;
}


CGeoPoly*
CNestedArea::PartShadow(
		const CPartPlace&	part_place,
		eNestProgression	progression )
{
	const CToolHit* toolhit = part_place.ToolHitGet();
	assert( (toolhit != NULL) );

	const CGeoPoly*	extremes = toolhit->ExtremesGet();
	assert( (extremes != NULL) );

	C2dCoord pt = part_place.WorldPointGet();

	//------
	// Blah, there must be a more efficient way :-(
	CGeoPoly tmp;
	tmp = (*extremes);

	tmp.Shift( C3dVec( pt.X(), pt.Y(), 0. ) );

	//------

#if BEFORE_2008_02_11
	const C2dBox& extentA = this->PolysGet(HIT_NEST_OUTSIDE)->GetAt(0)->Extent();
#else
	const C2dBox& extentA = this->PolysGet(HIT_PART_INSIDE)->GetAt(0)->Extent();
#endif

	double xminA = extentA.Xmin();
	double yminA = extentA.Ymin();
	double xmaxA = extentA.Xmax();
	double ymaxA = extentA.Ymax();

	const C2dBox& extentP = tmp.Extent();

	double xminP = extentP.Xmin();
	double yminP = extentP.Ymin();
	double xmaxP = extentP.Xmax();
	double ymaxP = extentP.Ymax();

	const C3dCoord& ps = tmp.StartPt();
	const C3dCoord& pe = tmp.EndPt();

	// TODO: part_shadow must account for the nesting direction (along Y).

	CGeoPoly* part_shadow = new CGeoPoly();

	// Generate in CW manner.
	if ((progression == PROGRESS_PPY_SPX) || (progression == PROGRESS_PPX_SPY))
	{
		part_shadow->Append( new CGeoLine( xminA, yminA, 0., xminA, ymaxP, 0. ) );

		part_shadow->Append( new CGeoLine( xminA, ymaxP, 0., xminP, ymaxP, 0. ) );

		part_shadow->CopyAppend( tmp );
		
		part_shadow->Append( new CGeoLine( xmaxP, yminP, 0., xmaxP, yminA, 0. ) );
		
		part_shadow->Append( new CGeoLine( xmaxP, yminA, 0., xminA, yminA, 0. ) );
	}
	else
	{
		part_shadow->Append( new CGeoLine( xminA, ymaxA, 0., xmaxP, ymaxA, 0. ) );

		part_shadow->Append( new CGeoLine( xmaxP, ymaxA, 0., xmaxP, ymaxP, 0. ) );

		part_shadow->CopyAppend( tmp );
		
		part_shadow->Append( new CGeoLine( xminP, yminP, 0., xminA, yminP, 0. ) );
		
		part_shadow->Append( new CGeoLine( xminA, yminP, 0., xminA, ymaxA, 0. ) );
	}

	part_shadow->GapsClose();
	part_shadow->LinesReduce( true, SMALL );

	return part_shadow;
}

void CNestedArea::PolyRender( const CGeoPoly& poly )
{
	if (DBG6)
		poly.Draw();
}
