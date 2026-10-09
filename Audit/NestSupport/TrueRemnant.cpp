
#include "stdafx.h"

#include <float.h>

#include "Register.h"
#include "DbTool.h"
#include "DbLine.h"
#include "MathConst.h"
#include "StringConst.h"

#include "TrueRemnant.h"
#include "Worm.h"


const WORD REMNANT_COLOR=0x0000ff;

// ==================================================================

CTrueRemnant::CTrueRemnant( const CNestConfig& config, const CViewMgr& view )
{
	m_config = &config;
	m_view = &view;
}

CTrueRemnant::~CTrueRemnant()
{
}

// ==================================================================
//	Generate the true-shape remnant from the sheet information
void CTrueRemnant::TrueShape( const CSheet& sheet )
{
	C2dBox	sheet_box;
	C2dBox	box;
	CString note;
	double	offset = 0.;

	const double REMNANT_PADDING = 0.1;

	// 2004/02/20 -- Textile Industrial encountered failures when generating
	// remnant (with a particular part) because the gap tolerance for
	// constructing a closed profile was set too low.
	// TODO: Determine and fix (if either is possible) why there is such
	// a large gap in the first place.  Does it have to do with boolean
	// operations on CGeoPoly objects?
	//
	//	 const double REMNANT_TOLERANCE = 0.01;
	//
	double REMNANT_TOLERANCE = CRegister::DoubleGetV( "Nesting", "RemnantTolerance" , 0.01 );

	sheet_box = sheet.Extent();
	box.Xmin( sheet_box.Xmin() + m_config->Border(BORDER_LEFT) );
	box.Ymin( sheet_box.Ymin() + m_config->Border(BORDER_BOTTOM) );
	box.Xmax( sheet_box.Xmax() - m_config->Border(BORDER_RIGHT) );
	box.Ymax( sheet_box.Ymax() - m_config->Border(BORDER_TOP) );

#if BEFORE_2007_12_31
	CGeoPoly material( box );
	CGeoPolyArray* kerf_array = sheet.ToolHitPoly( HIT_NEST_OUTSIDE );
	
	((CGeoCurve&) material[0]).DoubleSet( "dx", -m_config->Border(BORDER_LEFT) );
	((CGeoCurve&) material[1]).DoubleSet( "dy", -m_config->Border(BORDER_BOTTOM) );
	((CGeoCurve&) material[2]).DoubleSet( "dx", -m_config->Border(BORDER_RIGHT) );
	((CGeoCurve&) material[3]).DoubleSet( "dy", -m_config->Border(BORDER_TOP) );


#if BEFORE_2007_12_29  // Why was this block of code active?
	// Assemble the kerf punchouts
	offset = REMNANT_PADDING + m_config->Border(BORDER_REMNANT);
	generate_offset( &material, -1, offset );
#endif

	int num = kerf_array->Count();
	for (int idx=RESERVE_NUM; idx<num; idx++)
	{
		CGeoPoly* poly = (*kerf_array)[idx];

//		generate_offset( poly, 1, offset );
//		poly->Reduce();

		material.MINUS( *poly );
//material.Append( *poly );

		extract_max( &material, REMNANT_TOLERANCE );
		material.Reduce();
	}
//	extract_max( &material, REMNANT_TOLERANCE );
//	material.Reduce();

//	offset = REMNANT_PADDING;
//	generate_offset( &material, 1, offset );

	C3dCoord	ps;
	C3dCoord	pe;
	double		dx, dy;
	int	cnt = material.Count();
	for (int cndx = 0; cndx < cnt; ++cndx)
	{
		CGeoCurve&	crv = (CGeoCurve&) material[cndx];

		ps = crv.StartPt();
		pe = crv.EndPt();

		dx = pe.X() - ps.X();
		dy = pe.Y() - ps.Y();

		if (fabs(dx) < SMALL)
		{
			if (fabs(pe.X() - (box.Xmin() + offset)) < SMALL)
			{
				crv.Shift( C3dVec( -m_config->Border(BORDER_LEFT), 0., 0. ) );
			}
			else if (fabs(pe.X() - (box.Xmax() - offset)) < SMALL)
			{
				crv.Shift( C3dVec( m_config->Border(BORDER_RIGHT), 0., 0. ) );
			}
		}
		else if (fabs(dy) < SMALL)
		{
			if (fabs(pe.Y() - (box.Ymin() + offset)) < SMALL)
			{
				crv.Shift( C3dVec( 0., -m_config->Border(BORDER_BOTTOM), 0. ) );
			}
			else if (fabs(pe.Y() - (box.Ymax() - offset)) < SMALL)
			{
				crv.Shift( C3dVec( 0., m_config->Border(BORDER_TOP), 0. ) );
			}
		}
	}

	material.GapsClose();
#else
	// 2007.12.31 (PE) -- Experiment
	CNestedArea* nest_area = sheet.SheetNestedArea();
	// CGeoPoly* hole = nest_area->HolePolyGet();
	CGeoPoly* hole = sheet.MaterialPoly();
	CGeoPoly material;
	
	material = (*hole);

#endif

	// Remove the outermost parts from the material.
	PartsRemove( sheet, &material );

	// Set the internal remnant accordingly
	m_remnant.Flush();
	m_remnant = material;
}


void CTrueRemnant::extract_max( CGeoPoly* multipoly, double tol )
{
	// Reduce the multipoly to one closed profile
	// TODO:  Make this a method of CGeoPoly?
	//
	CGeoPoly* max_poly = NULL;
	double max_area = -1;

	while (true)
	{
		CGeoPoly* poly = multipoly->ExtractProfile( 1, tol, true );
		if (!poly)
		{ break; }

		double area = fabs(poly->Area());
		if (area > max_area)
		{
			max_area = area;
			if (max_poly)
			{ 
				max_poly->Flush();
				delete max_poly;
			}
			max_poly = poly;
		}
	}

	multipoly->Flush();
	(*multipoly) = (*max_poly);
}


void CTrueRemnant::generate_offset( CGeoPoly* poly, int dir, double dist )
{
	CGeoPoly off_poly;

	while (TRUE)
	{
		CGeoPoly* geo_prof = poly->ExtractProfile( 1, m_config->Resolution(), true );
		if (!geo_prof)
			break;

		CProfile base_prof;
		int idx, num = geo_prof->Count();
		for (idx=0; idx<num; idx++)
		{ base_prof.Append( (CGeoCurve*)&(*geo_prof)[idx] ); }

		CProfileList off_prof;
		CWorm::ProfOffset( base_prof, dir, dist, 180.0*DEG2RAD, false, &off_prof );

		num = off_prof.Count();
		for (idx=0; idx<num; idx++)
		{
			CProfile* sub_prof = off_prof[idx];

			int snum = sub_prof->Count();
			for (int sidx=0; sidx<snum; sidx++)
			{ off_poly.CopyAppend( *(sub_prof->GetAt(sidx)) ); }

			sub_prof->BenignFlush();
		}
		off_prof.DestructiveFlush();
	}

	poly->Flush();
	poly->CopyAppend( off_poly );
	off_poly.Flush();
}


// ==================================================================
//	Build a remnant outline for the right-hand side of the sheet,
//	with a width specified by cutback
void CTrueRemnant::Square( const C3dBox& extent, double cutback )
{
	C2dBox cut_extent( cutback, 0.0, extent.Dx(), extent.Dy() );

	m_remnant = CGeoPoly( cut_extent );
}


void CTrueRemnant::Extent( const C2dBox& extent )
{
	m_remnant = CGeoPoly( extent );
}

// ==================================================================
// Extract the remnant profile into the specified model
// Pilfered from CContHier::Init()
void CTrueRemnant::ContainmentDetermine( CGeoPolyArray& polys )
{
	C3dCoord origin( 0., 0., 0. );  // for debugging only

	int count = polys.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		// Set the default 'containment depth'
		CGeoPoly* poly = polys.GetAt( indx );
		poly->IntSet( "keep", 0 );
	}

	// Test each poly against the others, for inclusion
	for (int indxA = 0; indxA < count; indxA++)
	{
		CGeoPoly* polyA = polys.GetAt( indxA );

		for (int indxB = 0; indxB < count; indxB++)
		{
			if (indxB == indxA)
				continue;

			CGeoPoly* polyB = polys.GetAt( indxB );

			// ASSUMPTION: Nesting does not generate overlapping polys.
			bool encloses = polyA->PtInPoly( (*polyB)[0].StartPt() );
			if ( encloses )
			{
				// Each time a poly is enclosed, its value is flipped
				// ... a sort of enclosure parity.  Seems to work!
				int depth = polyB->IntGet( "keep", 0 ) + 1;
				polyB->IntSet( "keep", depth );

				if ( m_config->DebugWire() )
				{
					CViewBase* vbase = m_view->ActiveView();
					vbase->DisplayListBegin( PTEMP_LIST );
					vbase->DrawAtColor( DCOLOR_RED );
					vbase->DrawGeoAt( (*polyA), origin );
					vbase->DrawGeoAt( (*polyB), origin );
					vbase->DisplayListEnd( PTEMP_LIST );
					vbase->BufferShow( FRONT_BUFFER );
				}

			}
		}
	}
}

#if BEFORE_V20

void CTrueRemnant::AcceptablePolysMark( CGeoPolyArray& polys )
{
	// TODO: Are these limits even relevant?  I think not.
	double xmin = m_config->RemnantLength();
	double ymin = m_config->RemnantWidth();
	double amin = m_config->RemnantArea();

	ContainmentDetermine( polys );

	int count = polys.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoPoly* poly = polys.GetAt( indx );

		int keep = poly->IntGet( "keep", 0 );
		if ((keep & 1) == 0)  // essentially same as ((keep % 2) == 0).
		{
			// If 'keep' is zero then we've encountered an outermost profile.
			// Otherwise, we've encountered a cutout of some sort. In most
			// cases, we expect to find only one reusable outermost profile.
			// It is, however, possible to encounter more.

			const C2dBox& extent = poly->Extent();

			bool discard = ((extent.Dx() < xmin) || (extent.Dy() < ymin));
			if ( !discard )
			{
				double area = poly->Area();

				// TODO: Do we need to check the winding direction? All
				// outermost profiles should have a CW winding direction.
				discard = (fabs(area) < amin);
			}

			if ( discard )
				poly->IntSet( "keep", -1 );
		}
	}
}

#else

void CTrueRemnant::AcceptablePolysMark( CGeoPolyArray& polys )
{
	ContainmentDetermine( polys );

	int count = polys.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoPoly* poly = polys.GetAt( indx );

		// ASSUMPTION: 'keep' values (0) sheet profile / (1) part outer profile.
		int keep = poly->IntGet( "keep", 0 );
		if (keep > 1)
			poly->IntSet( "keep", -1 );  // mark to avoid subsequent processing
	}
}

#endif

#define EXTRACT_TOL SMALL

void CTrueRemnant::Extract( CModel* model, const C3dVec& shift )
{
	C3x4Matrix		xform;
	C3x4Matrix		theXform;
	CGeoPolyArray	polys;
	CGeoPoly*		poly;
	CDbWorkplane*	wp_world;
	CDbWorkplane*	wp_top;
	CDbWorkplane*	prev_workplane;
	CDbTool*		tool;
	CDbTool*		prev_tool;
	CDbFeature*		feature;
	CDbProfile*		profile;
	int				prev_color;
	int				icnt, indx;
	int				jcnt, jndx;

	model->EntityFind( "Remnant", (CDbEntity**)&tool, DBTOOL, DBTOOL );
	if (tool == NULL)
	{
		model->EntityCreate( DBTOOL, (CDbEntity**)&tool );
		tool->Name( "Remnant" );
		tool->ColorSet( REMNANT_COLOR );
	}

	model->EntityFind( STR_WORLD, (CDbEntity**)&wp_world, DBWORKPLANE, DBWORKPLANE );
	model->EntityFind( STR_TOP, (CDbEntity**)&wp_top, DBWORKPLANE, DBWORKPLANE );
	
	// Build the transformation matrix [source] X [target].
	xform = wp_world->Transform();

	theXform = wp_top->Inverse();
	xform.Transform( &theXform );

	m_remnant.Xform( theXform );

	// State control
	prev_workplane = model->ActiveWorkplane();
	prev_tool = model->ActiveTool();
	prev_color = model->Default().getColor( DCOLOR_BLACK );

	model->ActiveWorkplane( wp_top );
	model->ActiveTool( tool );
	model->pDefault()->setColor( REMNANT_COLOR );

	// Destroy any previous remnant data.
	model->EntityFind( "Remnant", (CDbEntity**) &feature, DBFEATURE, DBFEATURE );
	if (feature != NULL)
		feature->Delete();

	// Create the remnant feature.
	// NOTE: When remnant creation is finished, it is possible for us
	// to end up with an empty feature. Arguably, maybe would should
	// defer creating the remnant feature until we know that it will
	// contain something. However, ending up with an empty feature is
	// not a bad thing because, at minimum, it indicates that we
	// actually went through the remnant creation process.
	model->EntityCreate( DBFEATURE, (CDbEntity**)&feature );
	if (!feature)
		return;

	feature->Name( "Remnant" );

	// Build the complete list of material, part and hole polys.
	while (true)
	{
		poly = m_remnant.ExtractProfile( 1, EXTRACT_TOL, true );
		if (!poly)
			break;

		if ( poly->IsClosed( EXTRACT_TOL ) )
			polys.Append( poly );
		else
			delete poly;
	}

	AcceptablePolysMark( polys );

	// Convert the remaining polys to profiles.
	icnt = polys.Count();
	for (indx = 0; indx < icnt; ++indx)
	{
		poly = polys.GetAt( indx );
		if (poly->IntGet( "keep", -1 ) < 0)
			continue;

		model->EntityCreate( DBPROFILE, (CDbEntity**)&profile );
		if (profile == NULL)
			continue;  // Ideally, this shouldn't happen.

		feature->Append( profile );

		// Now, convert the remnant poly into the model.
		// Oh so ugly :-(
		jcnt = poly->Count();
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			const CGeoCurve& curve = (*poly)[jndx];
			CDbEntity* db_ent = NULL;

			((C3dCoord&)curve.StartPt()).Z(0.);
			((C3dCoord&)curve.EndPt()).Z(0.);

			if (curve.Type() == GEOARC)
			{
				((C3dCoord&)((CGeoArc&)curve).CenterPt()).Z(0.);
			}

			db_ent = profile->Db()->GeoConvert( curve, tool, wp_top );

			profile->Append( db_ent );
		}
	}

	polys.DestructiveFlush();

	// Reset state.
	model->ActiveWorkplane( prev_workplane );
	model->ActiveTool( prev_tool );
	model->pDefault()->setColor( prev_color );
}

#include "worm.h"
#include "WmChainDegouger.h"

void CTrueRemnant::PartPolysGet( const CNestedArea& nested_area, CWmChainList* part_chains )
{
	CNestedArea*	interior_area;
	CGeoPolyArray*	polys;
	CGeoPoly*		poly;
	CWmChain*		part_chain;
	int				icnt, indx, jcnt, jndx;

	icnt = nested_area.InteriorAreasCount();
	for (indx = 0; indx < icnt; ++indx)
	{
		interior_area = nested_area.InteriorAreaGet( indx );

		// TODO: The problem with using HIT_NEST_OUTSIDE is that
		// it includes the spacing distance, which should not be
		// considered when building the remnant!!!
		//
		// Perhaps the toolhit object should have yet another poly
		// that represents the 'true' outside kerf (ie. sans the
		// spacing distance) that is generated only when remnant
		// creation is active (?)
		//    polys = interior_area->PolysGet( HIT_NEST_OUTSIDE );
		polys = interior_area->PolysGet( HIT_KERF_OUTSIDE );
		jcnt = polys->Count();  // Should be only one.
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			poly = polys->GetAt( jndx );
			part_chain = new CWmChain();
			CWorm::Convert( (*poly), part_chain );

			// if (poly->Winding() < 0)
			//     part_chain->Reverse();
			part_chains->Append( part_chain );
		}

#if REQUIRED
		// 2008.01.08 (PE) -- Per conversation with Gary, it is unlikely
		// that anyone would want to use a cutout as a remnant. The thought
		// being if a cutout is potentially large enough to be used as a
		// remnant, the user will most likely fill it with parts anyways.

		// TODO: Likewise for the interior!!!
		//    polys = interior_area->PolysGet( HIT_NEST_INSIDE );
		polys = interior_area->PolysGet( HIT_PART_INSIDE );
		jcnt = polys->Count();  // Should be only one.
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			poly = polys->GetAt( jndx );
			part_chain = new CWmChain();
			CWorm::Convert( (*poly), part_chain );

			part_chains->Append( part_chain );
		}

		// TODO: Recursively obtain the inner profiles. This aspect of the
		// remnant creation problem works just peachy. However, we encounter
		// downstream failures (ie. get an incorrect result) when the outside
		// kerf of a part intersects the inside kerf of the part that contains
		// the nested part. I suspect this has something to do with the
		// winding directions of the profiles.
		PartPolysGet( (*interior_area), part_chains );
#endif
	}
}

// Remove the outermost parts from the material.
void CTrueRemnant::PartsRemove( const CSheet& sheet, CGeoPoly* material )
{
	CNestedArea* nested_area = sheet.SheetNestedArea();

	int count = nested_area->InteriorAreasCount();
	if (count > 0)
	{
		CWmChainDegouger	degouger;
		CWmChainList		part_chains;
		CWmChain			material_chain;

		CWorm::Convert( (*material), &material_chain );

		PartPolysGet( (*nested_area), &part_chains );

		// At this point, material_chain *must* have a CW winding
		// and each part_chain *must* have a CCW winding.
		degouger.FlagsSet( (DEGOUGE_INCLUDE_ALL_ISLANDS | DEGOUGE_PROPOGATE_IINDEX) );
		degouger.Init( &material_chain, &part_chains, SMALL );

		count = degouger.Count();
		if (count > 0)
		{
			material->Flush();

			// Convert the results back into 'material'.
			for (int indx = 0; indx < count; ++indx)
			{
				CWmChain* chain = degouger.Results().GetAt( indx );
				CWorm::Convert( (*chain), material );
			}
		}
	}
}
