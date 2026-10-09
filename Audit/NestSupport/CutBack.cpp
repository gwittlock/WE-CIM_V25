
#include "StdAfx.h"
#include "StringConst.h"
#include "ModelUtil.h"
#include "DbIterator.h"
#include "CutBack.h"

#define CUTBACK_COLOR 0x0000ff

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CCutBack::CCutBack()
	{ m_model = NULL; }

CCutBack::~CCutBack()
	{ /* nothing to do */ }

void
CCutBack::ModelSet( CModel* model )
	{ m_model = model; }

CDbLine*
CCutBack::CutBackLineGet() const
{
	CDbLine* cutback = NULL;

	CDbFeature* workzone = MaxWorkzoneGet();
	if (workzone != NULL)
	{
		int count = workzone->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* dbEntity = workzone->GetAt( indx );
			CDbLine* dbLine = dynamic_cast<CDbLine*>( dbEntity );
			if ((dbLine != NULL) && dbLine->HasAttribs())
			{
				if (dbLine->IntGet( "cutback", 0 ) != 0)
				{
					cutback = dbLine;
					break;
				}
			}
		}
	}

	return cutback;
}

CReturn
CCutBack::Update( int station_id, double xp )
{
	CReturn status;

	CDbLine* cutback = CutBackLineGet();
	if (cutback == NULL)
	{
		// Create the workzone and cutback line.
		status = CutBackCreate( station_id, xp );
	}
	else
	{
		// Modify the existing workzone and cutback line.
		status = CutBackModify( station_id, xp, cutback );
	}

	return status;
}

CReturn
CCutBack::CutBackCreate( int station_id, double xp )
{
	CReturn status;

	CDbFeature* dbZone = MaxWorkzoneGet();
	if (dbZone != NULL)
	{
		CDbTool* dbTool = NULL;
		int color;

		if (station_id > 0)
		{
			dbTool = CModelUtil::DbToolFind( m_model->Db(), STR_STATION_ID, station_id );
			if (dbTool != NULL)
				color = dbTool->ColorGet( CUTBACK_COLOR );
		}

		if (dbTool == NULL)
		{
			m_model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
			dbTool->Name( "Cutback" );

			color = CUTBACK_COLOR;
			dbTool->ColorSet( color );
		}

		CDbWorkplane* dbWork = NULL;
		m_model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
		if ( !dbWork )
			dbWork = m_model->ActiveWorkplane();

		double ymax = m_model->Header().getReal( "Width", 0. );
		C3dCoord ps( xp, ymax, 0. );
		C3dCoord pe( xp, 0., 0. );
		dbWork->Inverse().Transform( &ps );
		dbWork->Inverse().Transform( &pe );

		CDbLine* cutback;
		m_model->EntityCreate( DBLINE, (CDbEntity**) &cutback );
		cutback->Name("CutBack");
		cutback->Init( dbTool, dbWork, ps, pe );
		cutback->ColorSet( color );
		cutback->IntSet( "cutback", 1 );

		CutbackWorkzoneCreate( dbZone, cutback );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCutBack::CutBackCreate()" );
	}

	return status;
}

CReturn
CCutBack::CutBackModify( int station_id, double xp, CDbLine* cutback )
{
	CReturn status;

	CDbFeature* dbZone = dynamic_cast<CDbFeature*>( cutback->Owner() );
	if ((dbZone != NULL) && dbZone->IsWorkZone())
	{
		CDbWorkplane* dbWork = cutback->Workplane();

		CDbTool* dbTool = cutback->Tool();
		if (station_id > 0)
		{
			CDbTool* temp = CModelUtil::DbToolFind( m_model->Db(), STR_STATION_ID, station_id );
			if (temp != NULL)
				dbTool = temp;
		}

		// In case someone modified the sheet ....
		double xmax = m_model->Header().getReal( "Length", 0. );
		double ymax = m_model->Header().getReal( "Width", 0. );

		C3dCoord ps = cutback->StartPt();
		C3dCoord pe = cutback->EndPt();

		ps.XYZ( xp, ymax, ps.Z() );
		pe.XYZ( xp, 0., pe.Z() );
		cutback->Init( dbTool, dbWork, ps, pe );
		cutback->ColorSet( dbTool->ColorGet( CUTBACK_COLOR ) );

		C2dBox box( xp, 0., xmax, ymax );
		WorkzoneUpdate( box, dbZone );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCutBack::CutBackModify()" );
	}

	return status;
}

CReturn
CCutBack::Delete()
{
	CReturn status;

	CDbLine* cutback = CutBackLineGet();
	if (cutback != NULL)
	{
		CDbFeature* owner = dynamic_cast<CDbFeature*>( cutback->Owner() );
		if ( CanDelete( owner ) )
			m_model->EntityDelete( owner->Id() );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCutBack::Delete()" );
	}

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Ugh. Wish there was a better way ....
CDbFeature*
CCutBack::MaxWorkzoneGet() const
{
	CDbIterator	iter;
	CDbFeature*	max_workzone = NULL;
	int			max_index = -1;

	iter.Init( m_model->Db(), DBFEATURE );
	while (1)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
		{
			int index = dbFeature->IntGet( "_zone_num", -1 );
			if (index > max_index)
			{
				max_workzone = dbFeature;
				max_index = index;
			}
		}

		iter.Next();
	}

	return max_workzone;
}

bool
CCutBack::CanDelete( CDbFeature* dbFeature ) const
{
	bool can_delete = false;

	if ((dbFeature != NULL) && dbFeature->IsWorkZone())
	{
		if (dbFeature->IntGet( "_zone_num", -1 ) > 0)
			can_delete = (dbFeature->Count() == 1);
	}

	return can_delete;
}

void
CCutBack::WorkzoneUpdate( const C2dBox& box, CDbFeature* dbZone )
{
	dbZone->StringSet( STR_TYPE, "_zone" );
	dbZone->DoubleSet( "_zone_left", box.Xmin() );
	dbZone->DoubleSet( "_zone_top", box.Ymax() );
	dbZone->DoubleSet( "_zone_right", box.Xmax() );
	dbZone->DoubleSet( "_zone_bottom", box.Ymin() );
}

#define REPO_ZONE_NAME	"_repo_zone_%d"

// Pilfered from CRepo::PackageInstances().
CDbFeature*
CCutBack::CutbackWorkzoneCreate( const CDbFeature* max_workzone, CDbLine* cutback_line )
{
	int zidx = max_workzone->IntGet( "_zone_num", 0 ) + 1;

	CDbFeature* dbZone = NULL;
	m_model->EntityCreate( DBFEATURE, (CDbEntity**) &dbZone );

	CString name;
	name.Format( REPO_ZONE_NAME, zidx );
	name = dbZone->NameConvert( name );
	dbZone->SystemName( name );

	// Zone numbers are [1..N] to make VB-side easier.
	dbZone->Owner( NULL );
	dbZone->IntSet( "_zone_num", zidx );

	dbZone->Append( cutback_line );

	double left = cutback_line->StartPt().X();
	double top = max_workzone->DoubleGet( "_zone_top", 0. );
	double right = m_model->Header().getReal( "Length", 0. );
	double bottom = max_workzone->DoubleGet( "_zone_bottom", 0. );

	C2dBox box( left, bottom, right, top );
	WorkzoneUpdate( box, dbZone );

	return dbZone;
}
