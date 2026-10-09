// ==================================================================
//		ManualData
//
// ==================================================================

#include "stdafx.h"
#include <float.h>

#include "MathConst.h"

#include "StringConst.h"
#include "DbIterator.h"
#include "DbCommand.h"
#include "repo.h"
#include "SeedMgr.h"

#include "ManualData.h"

// ==================================================================

CManualData::CManualData()
{
	m_model = NULL;
	reset_part();
}

CManualData::~CManualData()
{
}

// ==================================================================

void
CManualData::reset_part()
{
	m_part = NULL;
	m_instance = NULL;
}

// ==================================================================
//		InitSheet
//
CReturn 
CManualData::InitSheet( CString& configdb, CModel* model, CViewMgr& view )
{
	CReturn		ret;
	CSeedMgr	seed_mgr;

//MessageBox( NULL, "INIT SHEET", NULL, MB_OK );
	reset_part();

	m_model = model;

	m_config.GridNest( true );

	// If we have the information in hand, load the config from
	// the database
	if (configdb.GetLength() > 2)
	{ ret += m_config.Load( configdb, "" ); }
	else
	{
		m_config.Resolution(0.1);
	}

	m_config.PassInit(1);
	m_config.Pass(0);

	// Get our sheet ready for use...
	m_sheet.Init( m_model );

	m_sheet.FillBarrier( m_config, m_sheet.Extent() );

	// Now take the parts and create the nesting geo data from them
	ret += m_partbin.Clone( m_config, *m_model );
	int num = m_partbin.Count();
	for (int idx=0; idx<num; idx++)
	{
		CNestingPart* npart = m_partbin.GetAt(idx);

		npart->Simplify( m_config );
		npart->Rotate();
//		npart->Lead( m_config );

		npart->Generate( 0 );
		npart->Assemble( m_config, 0 );

		CDbPattern* db_pattern = NULL;
		m_model->EntityFind( npart->Id(), (CDbEntity**)&db_pattern, DBPATTERN, DBPATTERN );
		npart->ToolHit(0)->PatternBurn( db_pattern );
	}

	seed_mgr.Push();
	m_sheet.SeedMgr( &seed_mgr );

	// Explode the nesting geometry to the instances
	CDbIterator iter;
	iter.Init( m_model->Db(), DBCOMMAND );
	while ( TRUE )
	{
		CDbCommand* db_command = dynamic_cast<CDbCommand*>( iter() );
		if (!db_command)
			break;
		iter.Next();

		// Use our internal command system to decode the contents
		if ( db_command->IsInstance() )
		{
			ID id = (ID) db_command->IntGet( "patid", 0 );

			int num = m_partbin.Count();
			for (int idx=0; idx<num; idx++)
			{
				CNestingPart* npart = m_partbin.GetAt(idx);

				if (id == npart->Id())
				{
					CPartPlace	best;
					C2dCoord	fodder;

					best.Init( db_command->Coord(0), fodder, npart->ToolHit(0) );

					// Bogus but necessary ....
					best.ScoresInit( 1., 1. );

					m_sheet.PunchGeo( m_config, best, NULL, view );

					break;
				}
			}
		}
	}

//m_sheet.DebugToSheet( view );
//m_sheet.DebugToModel( model, view );

	return ret;
}



// ==================================================================
//		UsePattern
//
//	Set up a pre-existing pattern for bump nesting.  Places it
//	on the sheet, displays it, and remembers it.
//
int
CManualData::UsePattern( ID id, const C3dCoord& place, CViewMgr& view )
{
	int	count, indx;

	reset_part();

	// Find the pattern based on the (original) ID
	count = m_partbin.Count();
	for (indx = 0; indx < count; ++indx)
	{ 
		CNestingPart* npart = m_partbin.GetAt(indx);

		if (npart->Id() == id)
		{
			m_part = npart;
			break;
		}
	}

	if (m_part != NULL)
	{
		CPartPlace	best;
		C2dCoord fodder;

		best.Init( place, fodder, m_part->ToolHit(0) );

		// Bogus but necessary ....
		best.ScoresInit( 1., 1. );

		// Place it on the sheet at the given position
		m_place = place;

		m_instance = m_sheet.PunchInstance( m_config, best, ZONE_NONE, false, view );
	}

	return ((m_instance == NULL) ? -1 : m_instance->Id());
}

// ==================================================================
//		UseInstance
//
//	Extract the named instance from the sheet's geo, and get
//	ready to re-bump it.
//
int
CManualData::UseInstance( ID id, CViewMgr& view )
{
	CReturn ret;

//MessageBox( NULL, "USE INSTANCE", NULL, MB_OK );
	reset_part();
	//
	// Locate our instance
	//
	m_instance = NULL;
	m_model->EntityFind( id, (CDbEntity**)&m_instance, DBCOMMAND, DBCOMMAND );
	if (!m_instance)
		return -1;
	//
	// Find the pattern called by this instance
	//

	id = (ID) m_instance->IntGet( "patid", 0 );

	int num = m_partbin.Count();
	for (int idx=0; idx<num; idx++)
	{ 
		CNestingPart* npart = m_partbin.GetAt(idx);

		if (npart->Id() == id)
		{
			m_part = npart;
			break;
		}
	}
	if (!m_part)
		return -1;
	//
	// Place it on the sheet at the given position
	//
	m_place = m_instance->Coord(0);
	m_sheet.ClearInstance( m_instance );
//	m_instance = m_sheet.PunchInstance( m_config, m_part, 0, place, ZONE_NONE, view );

//m_sheet.DebugToModel( m_model, view );

	return m_instance->Id();
}


// ==================================================================
//		UseSelection
//
//	Set up a new pattern for bump nesting.  Places it
//	on the sheet, displays it, and remembers it.
//
int
CManualData::UseSelection( const C3dCoord& place, CViewMgr& view )
{
	CPartPlace	best;
	C2dCoord	fodder;

	reset_part();

	// Create a part from the selection
	int idx = m_partbin.AddSelection( m_config, *m_model );
	if (idx<0)
	{ return -1; }

	CNestingPart* npart = m_partbin.GetAt(idx);

	npart->Simplify( m_config );
	npart->Rotate();

	npart->Generate( 0 );
	npart->Assemble( m_config, 0 );

	CDbPattern* db_pattern = NULL;

	best.Init( place, fodder, npart->ToolHit(0) );

	// Bogus but necessary ....
	best.ScoresInit( 1., 1. );

	// Place it on the sheet at the given position
	m_place = place;
	m_part = npart;
	m_instance = m_sheet.PunchInstance( m_config, best, ZONE_NONE, false, view );
	npart->Id( npart->ToolHit(0)->PatternBurn()->Id() );

	if (!m_instance)
	{ return -1; }

	return m_instance->Id();
}

// ==================================================================
//		UseFile
//
//	Set up a new pattern from a file for bump nesting.  Places it
//	on the sheet, displays it, and remembers it.
//
int
CManualData::UseFile( const CString& filepath, const C3dCoord& place, CViewMgr& view )
{
	CPartPlace	best;
	C2dCoord	fodder;

	reset_part();

	// Create a part from an imported file.
	int idx = m_partbin.AddFile( m_config );
	if (idx<0)
	{ return -1; }

	CNestingPart* npart = m_partbin.GetAt(idx);

	npart->Simplify( m_config );
	npart->Rotate();

	npart->Generate( 0 );
	npart->Assemble( m_config, 0 );

	CDbPattern* db_pattern = NULL;

	best.Init( place, fodder, npart->ToolHit(0) );

	// Bogus but necessary ....
	best.ScoresInit( 1., 1. );

	// Place it on the sheet at the given position
	m_place = place;
	m_part = npart;
	m_instance = m_sheet.PunchInstance( m_config, best, ZONE_NONE, false, view );
	npart->Id( npart->ToolHit(0)->PatternBurn()->Id() );

	return ((m_instance == NULL) ? -1 : m_instance->Id());
}


// ==================================================================
//		Bump
//
//	Bump (move) the part in the specified way, or until it hits something
//
CReturn 
CManualData::Bump( const C3dVec& delta, CViewMgr& view )
{
	CReturn ret;

	if (!m_part)
	{ return ret; }

	double bump_x = 0.0;
	double bump_y = 0.0;

	double dx = delta.X();
	double dy = delta.Y();

	if ( ZERO(dx) && ZERO(dy) )
	{ return ret; }

#if required
//	if (SGN(delta.X()))
	if (fabs(dx) > fabs(dy))
	{
		bump_x = m_sheet.Bump( m_config, m_place, *(m_part->ToolHit(0)), BUMP_X, SGN(dx), false, view );
		if (EQUAL(bump_x, DBL_MAX))
			bump_x = dx;
		else
			bump_x *= SGN(dx);
	}
	else
//	if (SGN(delta.Y()))
	{
		bump_y = m_sheet.Bump( m_config, m_place, *(m_part->ToolHit(0)), BUMP_Y, SGN(dy), false, view );
		if (EQUAL(bump_y, DBL_MAX))
			bump_y = dy;
		else
			bump_y *= SGN(dy);
	}
#endif

#if ORIGINAL_CODE
	C3dVec bump_delta( min( fabs(bump_x), fabs(dx) ) * SGN(dx),
						min( fabs(bump_y), fabs(dy) ) * SGN(dy),
						0.0 );
#else
	C3dVec bump_delta( fabs(dx) * SGN(dx), fabs(dy) * SGN(dy), 0. );
#endif
	C3x4Matrix shift;
	shift.setUnit();
	shift.Shift( bump_delta );

	CDbEntity::NewAction();
	m_instance->Transform( shift );

	m_place += bump_delta;

	return ret;
}


// ==================================================================
//		Punch
//
//	Lock it down
//
CReturn 
CManualData::Punch( CViewMgr& view )
{
	CReturn		status;
	CPartPlace	best;
	C2dCoord	fodder;

	best.Init( m_place, fodder, m_part->ToolHit(0) );

	// Bogus but necessary ....
	best.ScoresInit( 1., 1. );

	m_sheet.PunchGeo( m_config, best, NULL, view );

//m_sheet.DebugToModel( m_model, view );

	reset_part();

	return status;
}
