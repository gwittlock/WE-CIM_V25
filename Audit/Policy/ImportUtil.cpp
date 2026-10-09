 
#include "stdafx.h"
#include "cmn_resource.h"
#include "Register.h"
#include "ColorConst.h"
#include "MathConst.h"
#include "StringConst.h"
#include "Return.h"

#include "token.h"

#include "3dVec.h"
#include "3dBox.h"
#include "GeoPoint.h"
#include "DbAllEntities.h"
#include "DbIterator.h"
#include "AutoModDb.h"
#include "AutoMod.h"
#include "ProfileBuilder.h"

#include "CadEntity.h"
#include "ImportUtil.h"


// AutoDesk
static CString XY_POS( "XY_POS" );
static CString XY_NEG( "XY_NEG" );
static CString YZ_POS( "YZ_POS" );
static CString YZ_NEG( "YZ_NEG" );
static CString ZX_POS( "ZX_POS" );
static CString ZX_NEG( "ZX_NEG" );
static CString EMPTY( "" );

// Ascii
static CString A_TOP( "A_TOP" );
static CString A_RGHT( "A_RIGHT" );
static CString A_LFT( "A_LEFT" );
static CString A_FRONT( "A_FRONT" );
static CString A_BACK( "A_BACK" );


// View Planes
static CString VRIGHT( "VRIGHT" );
static CString VLEFT( "VLEFT" );
static CString VFRONT( "VFRONT" );
static CString VBACK( "VBACK" );
static CString VTOP( "VTOP" );
static CString VISO( "VIsoID" );

// MM2 Standard
static CString RGHT( "RIGHT" );
static CString LFT( "LEFT" );
static CString FRONT( "FRONT" );
static CString BACK( "BACK" );

// SmartCAM
static CString XY_PLANE( "XY_PLANE" );
static CString YZ_PLANE( "YZ_PLANE" );
static CString XZ_PLANE( "XZ_PLANE" );

static CTokenList global_tokens;

const double NORMAL_TOL = 1. - VECTOR_SMALL;
const int DEFAULT_COLOR = RGB(255,0,0);

////////////////////////////////////////////////////////////////////////

CImportUtil::CImportUtil()
{
	m_model = NULL ;
	m_autoModDb = NULL;
	m_currLayerRecIndx = -1;
}

CImportUtil::~CImportUtil()
{
	m_layers.DestructiveFlush();
}

void
CImportUtil::Init( CModel* model, CAutoModDb* autoModDb )
{
	  m_model = model;
	  m_autoModDb = autoModDb;
	  m_layers.DestructiveFlush();

	  m_currLayerName.Empty();
	  m_currLayerRecIndx = -1;
}

// static
void
CImportUtil::AcceptedLayersGet(
					const CAutoModDb&	autoModDb,
					CCadLayerList*		acceptedLayers )
{
	const CStrgList& layers = autoModDb.LayerNames();

	int	count = layers.Count();
	if (count == 0)
	{
		// Assume file "preview" mode.
		acceptedLayers->Append( "*default*" );
	}
	else
	{
		for (int indx = 0; indx < count; ++indx)
		{
			acceptedLayers->Append( (*layers[indx]) );
		}
	}
}

CReturn
CImportUtil::CadEntitiesConvert( const CCadEntityList& cadEntities )
{
	CReturn			status;
	CCadEntity*		cadEntity;
	int				autoModRecIndx;
	int				count, indx;

	status = AutoDeskPlanesCreate();

	count = cadEntities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		cadEntity = cadEntities[indx];

		autoModRecIndx = m_autoModDb->SourceLayerFind( cadEntity->LayerName() );

		if (autoModRecIndx >= 0)
			EntityConvert( cadEntity->Entity(), cadEntity->Normal(), autoModRecIndx );
	}

	return status;
}

CReturn
CImportUtil::AcadPostProc( double rotation, bool raw )
{
	CReturn			status;
	C3dBox			box;
	CDbWorkplane*	dbWork;
	double			zStock, thickness;
	int				workType, machType;

	thickness = m_autoModDb->MaterialThick();
	workType = m_autoModDb->WorkplaneType();
	machType = m_autoModDb->MachineType();

	if (fabs(rotation) > SMALL)
	{
		CDbIterator	iter;
		C3x4Matrix	xform;
		CDbEntity*	dbEntity;

		xform.setXYAngle( -(rotation*DEG2RAD) );

		CDbEntity::NewAction();

		iter.Init( m_model->Db(), DBPOINT );
		while ( TRUE )
		{
			dbEntity = iter();
			if (dbEntity == NULL)
				break;

			dbEntity->Transform( xform );

			iter.Next();
		}

		box = m_model->Box();

		xform.setUnit();
		xform.Shift( C3dVec( -box.Xmin(), - box.Ymin(), 0. ) );

		CDbEntity::NewAction();

		iter.Init( m_model->Db(), DBPOINT );
		while ( TRUE )
		{
			dbEntity = iter();
			if (dbEntity == NULL)
				break;

			dbEntity->Transform( xform );

			iter.Next();
		}
	}

	status = AutoDeskPlanesCreate();

	if ( status.IsOk() )
	{
		int dir = StockDirection();
		status = HeaderUpdate( XY_POS, dir, thickness, &box, &zStock );
	}

	if ( status.IsOk() )
		status = ViewPlanesCreate( box.Dx(), box.Dy(), thickness );

	if ( status.IsOk() )
		status = StandardPlanesCreate( box.Dx(), box.Dy(), thickness, workType, machType );

	if ( status.IsOk() )
		status = AutoDeskPlanesTransform( -box.Xmin(), -box.Ymin(), (thickness-zStock) );

	if ( status.IsOk() && !raw )
		status = ProfilesCreate();

	if ( status.IsOk() )
		status = SystemFlagsSet();

	status += AutoDeskPlanesDestroy();

	status += m_model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	if (dbWork != NULL)
		m_model->ActiveWorkplane( dbWork );

	return status;
}

// NOTE: Originally known as CNestingPart::do_strip_layers()
CReturn
CImportUtil::LayersStrip( CModel* model )
{
	CReturn		status;
	CDbIterator iter;
	CDbEntity*	db_ent;
	CDbTool*	dbTool;
	CDbFeature*	feature;
	CDbProfile*	dbProfile;

	// We can save time by eliminating geometry at the
	// profile level.  By deleting a profile, you also
	// delete its contained entities.
	iter.Init( model->Db(), DBPROFILE );
	while (TRUE)
	{
		dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		iter.Next();

		dbTool = dbProfile->Tool();
		if (dbTool == NULL)
		{
			// Must be empty profile.
			dbProfile->Delete();
		}
		else if (dbTool->IsLayer() && (dbTool->Name().CompareNoCase(STR_PART_OUTLINE) != 0))
		{
			dbProfile->Delete();
		}
	}

#if V16_ORIGINAL
	iter.Init( model->Db(), DBPOINT );
#else
	// We can save time by eliminating geometry starting
	// at the curve level.  By deleting a curve, you also
	// delete its end points.
	//
	// How would point entities (on a layer) get into the model?
	// Answer: Only if a user manually creates them.
	//
	// What is the likelyhood of that happening and do we really care?
	// Answer: Not very likely, and probably not.
	//
	iter.Init( model->Db(), DBLINE );
#endif
	while ( TRUE )
	{
		db_ent = iter();
		if (db_ent == NULL)
			break;

		iter.Next();

		if (db_ent->Type() == DBPROFILE)
			continue;  // been there done that

		//
		// Keep only tooled geometry
		//
		dbTool = db_ent->Tool();
		if (dbTool != NULL)
		{
			if (dbTool->IsLayer() && (dbTool->Name().CompareNoCase(STR_PART_OUTLINE) != 0))
			{
				db_ent->Delete();
			}
			else
			{
				// Remove any and all associations (to prevent regeneration)

				feature = dynamic_cast<CDbFeature*>(db_ent);
				if (feature)
				{
					feature->RefsFlush();
					feature->AttribDelete( "function" );
				}
			}
		}
	}

	return status;
}

CReturn
CImportUtil::EntityConvert(
						const CGeoElem*	geoElem,
						const C3dVec&	normal,
						int				autoModDbRecIndx )
{
	CReturn		status;
	C3dCoord	ps;
	C3dCoord	pe;
	C3dCoord	pc;

	double tol = m_autoModDb->FilterTol();

	CurrentRecordSet( autoModDbRecIndx );

	CDbTool* dbTool = TargetTool();
	CDbWorkplane* dbWork = TargetWork( normal );

	double zLevel = UNDEFINED;
	EAutoModMode zLevelMode = m_autoModDb->ZLevelMode();

	if (IsPhenolicProject2() && (zLevelMode == AUTOMOD_USE_CAD_Z))
	{
		zLevel = geoElem->StartPt().Z();
	}
	else
	{
		// Now we just slam Z to zero because it simplifies the product ;-)
		zLevel = 0.;
	}

	switch( geoElem->Type() )
	{
	case GEOPOINT:
		{
			CGeoPoint* geoPoint = (CGeoPoint*) geoElem;
			if ( geoPoint->HasAttrib() )
			{
				CString text = geoPoint->StringGet( "text", "" );
				if (text.GetLength() > 0)
				{
					CDbCommand* dbCommand;
					m_model->EntityCreate( DBCOMMAND, (CDbEntity**) &dbCommand );

					dbCommand->Init( dbTool, dbWork, geoPoint->EndPt(), text );

					double orient = geoPoint->DoubleGet( "orient", 0. );
					dbCommand->DoubleSet( "angle", orient );
				}
			}
			else if ( m_autoModDb->AllowTorchPiercing() )
			{
				// 2007.05.06 (PE) -- Introduced for Dynatorch to support piercing
				// with torch. Per conversation with Gary, Walt wants to simply
				// blow through the materal wherever a point is defined. We
				// accomplish this by first converting the CAD point to an arc
				// whose diameter exactly matches the kerf of the torch. Upon
				// auto-tooling the model, we associate the torch to these arcs
				// via a hole entity.

				double kerf = m_autoModDb->KerfGet();
				if (kerf < UNDEFINED)
				{
					CDbArc* dbArc;
					m_model->EntityCreate( DBARC, (CDbEntity**) &dbArc );

					CGeoArc geo_arc;
					geo_arc.Init( geoPoint->EndPt(), (0.5 * kerf), CW );

					dbArc->Init( dbTool, dbWork, geo_arc );
				}
			}
		}
		break;

	case GEOLINE:
		{
			CGeoLine* geoLine = (CGeoLine*) geoElem;

			ps = geoLine->StartPt();
			pe = geoLine->EndPt();

			if (zLevel < UNDEFINED)
			{
				ps.Z( zLevel );
				pe.Z( zLevel );
			}

			if ( !ps.WithinTol( pe, tol ) )
			{
				CDbLine* dbLine;
				m_model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );

				dbLine->Init( dbTool, dbWork, ps, pe );
			}
		}
		break;

	case GEOARC:
		{
			CGeoArc* geoArc = (CGeoArc*) geoElem;

			ps = geoArc->StartPt();
			pe = geoArc->EndPt();
			pc = geoArc->CenterPt();

			if (zLevel < UNDEFINED)
			{
				ps.Z( zLevel );
				pe.Z( zLevel );
				pc.Z( zLevel );
			}

			if (geoArc->Length2d() > tol)
			{
				CDbArc* dbArc;
				m_model->EntityCreate( DBARC, (CDbEntity**) &dbArc );

				dbArc->Init( dbTool, dbWork, ps, pe, pc, geoArc->Dir() );
			}
		}
		break;

	default:
		break;
	}

	return status;
}

bool
CImportUtil::IsLayerOk( const CString& layerName, bool force_visible )
{
	if ( force_visible )
	{
		// Forced visible, but still extra workaround for 'Stock'... fuck it
		if ( layerName.CompareNoCase( STR_STOCK ) == 0)
			return TRUE;
	}
	else
	{
		// Reject the layer if it does not appear in the layer table.
		return (m_autoModDb->SourceLayerFind( layerName ) >= 0);
	}

	return FALSE;
}

int
CImportUtil::LayerRecFind( const CString& sourceLayerName )
{
	if (m_currLayerName.CompareNoCase( sourceLayerName ) == 0)
		return m_currLayerRecIndx;

	return ( m_autoModDb->SourceLayerFind( sourceLayerName ) );
}

CReturn
CImportUtil::CurrentRecordSet( int indx )
{
	CReturn status = m_autoModDb->CurrentRecordSet( indx );

	if ( status.IsOk() )
		m_currLayerRecIndx = indx;

	return status;
}

CDbTool*
CImportUtil::TargetTool()
{
	CDbTool* dbTool = NULL;

	CReturn status = m_autoModDb->CurrentRecordSet( m_currLayerRecIndx );
	if ( status.IsOk() )
	{
		const CString& targetToolName = m_autoModDb->TargetName();

		m_model->EntityFind( targetToolName, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if (dbTool == NULL)
		{
			m_model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
			dbTool->Name( targetToolName );
#if (_NST)
			dbTool->StringSet( STR_WORKPLANE, STR_TOP );
#endif
			Color( dbTool );
		}
	}

	return dbTool;
}

void
CImportUtil::Color( CDbEntity* dbEntity )
{
	int color;

	if ( m_autoModDb->IsPreview() )
	{
		color = DCOLOR_YELLOW;
	}
	else if (m_currLayerRecIndx >= 0)
	{
		color = m_autoModDb->IntGet( STR_COLOR );
	}
	else
	{
		color = DCOLOR_WHITE;  // indicates that something is wrong
	}

	dbEntity->ColorSet( color );
}

void
CImportUtil::ZLevel( double* zLevel )
{
	EAutoModMode zLevelMode = m_autoModDb->ZLevelMode();

	if (IsPhenolicProject2() && (zLevelMode == AUTOMOD_USE_CAD_Z))
	{
		// Nothing to do.
	}
	else
	{
		// Now we just slam Z to zero because it simplifies the product ;-)
		(*zLevel) = 0.;
	}
}

void
CImportUtil::PointTransform(
						const C3dVec&	normal,
						double			depth,
						C3dCoord*		pt )
{
	EAutoModMode zLevelMode = m_autoModDb->ZLevelMode();
	if (zLevelMode == AUTOMOD_USE_CMDB_Z)
	{
		// Use the explicit depth specified in the cmdb.

		ZLevel( &depth );

		if (fabs( normal.Z() ) > NORMAL_TOL)
			pt->Z( depth );
		else if (fabs( normal.X() ) > NORMAL_TOL)
			pt->X( depth );
		else if (fabs( normal.Y() ) > NORMAL_TOL)
			pt->Y( depth );
	}
	else
	{
		(*pt) += (normal * depth);
	}
}

CDbWorkplane*
CImportUtil::TargetWork( const C3dVec& normal )
{
	CDbWorkplane* dbWork = NULL;

	// "Extrusion" z-vector, defines OCS
	if (EQUAL( normal.X(), 1.0 ))
		m_model->EntityFind( YZ_POS, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.X(), -1.0 ))
		m_model->EntityFind( YZ_NEG, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Y(), 1.0 ))	
		m_model->EntityFind( ZX_POS, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Y(), -1.0 ))		
		m_model->EntityFind( ZX_NEG, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Z(), 1.0 ))
		m_model->EntityFind( XY_POS, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Z(), -1.0 ))
		m_model->EntityFind( XY_NEG, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	return dbWork;
}

CReturn
CImportUtil::AutoDeskPlanesCreate()
{
	CReturn status;

	CDbWorkplane* plane;

	// XY POS
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 1, 0 ),		// j
					C3dVec( 0, 0, 1 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( XY_POS );				// name
	plane->Hide();

	// XY NEG
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( -1, 0, 0 ),		// i
					C3dVec( 0, 1, 0 ),		// j
					C3dVec( 0, 0, -1 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( XY_NEG );				// name
	plane->Hide();

	// YZ POS
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( 0, 1, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 1, 0, 0 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( YZ_POS );				// name
	plane->Hide();

	// YZ NEG
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( 0, -1, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( -1, 0, 0 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( YZ_NEG );				// name
	plane->Hide();

	// ZX POS
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( -1, 0, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 0, 1, 0 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( ZX_POS );				// name
	plane->Hide();

	// ZX NEG
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 0, -1, 0 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( ZX_NEG );				// name
	plane->Hide();

	return status;
}

CReturn
CImportUtil::AutoDeskPlanesDestroy()
{
	CReturn status;

	CDbWorkplane* dbWork;

	status  = m_model->EntityFind( XY_POS, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( XY_NEG, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( YZ_POS, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( YZ_NEG, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( ZX_POS, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( ZX_NEG, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	return status;
}

CReturn
CImportUtil::AutoDeskPlanesTransform( double dx, double dy, double dz )
{
	CReturn status;

	status += Transform( XY_POS, STR_TOP,   dx, dy, dz );
	status += Transform( XY_NEG, STR_TOP,  -dx, dy, dz );
	status += Transform( YZ_POS, RGHT, dx, dy, dz );
	status += Transform( YZ_NEG, LFT,  dx, dy, dz );
	status += Transform( ZX_POS, BACK,  dx, dy, dz );
	status += Transform( ZX_NEG, FRONT, dx, dy, dz );

	return status;
}


CReturn
CImportUtil::AsciiPlanesCreate( double length, double width, double zStock, int workType )
{
	CReturn status;
	CDbWorkplane* dbWork;

	// Freakin' Morbidelli workplanes.
	dbWork = PlaneFindCreate( A_RGHT );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 0, 0, 1 ),		// i
					C3dVec( 0,-1, 0 ),		// j
					C3dVec(-1, 0, 0 ),		// k
					C3dCoord( length, width, 0 ),	// t
					-1 );					// up

	dbWork = PlaneFindCreate( A_LFT );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 0, 0, 1 ),		// i
					C3dVec( 0,-1, 0 ),		// j
					C3dVec( 1, 0, 0 ),		// k
					C3dCoord( 0, width, 0 ),	// t
					-1 );					// up

	dbWork = PlaneFindCreate( A_FRONT );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 0, 1, 0 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					-1 );					// up

	dbWork = PlaneFindCreate( A_BACK );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 0,-1, 0 ),		// k
					C3dCoord( 0, width, 0 ),	// t
					-1 );					// up

	dbWork = PlaneFindCreate( A_TOP );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0,-1, 0 ),		// j
					C3dVec( 0, 0,-1 ),		// k
					C3dCoord( 0, width, zStock ),	// t
					-1 );					// up

	return status;
}

CReturn
CImportUtil::AsciiPlanesDestroy()
{
	CReturn status;

	CDbWorkplane* dbWork;

	status += m_model->EntityFind( A_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( A_RGHT, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( A_LFT, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( A_FRONT, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	status += m_model->EntityFind( A_BACK, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork != NULL)
		dbWork->Delete();

	return status;
}

CReturn
CImportUtil::AsciiPlanesTransform( double dx, double dy, double dz )
{
	CReturn status;

	status += Transform( A_TOP, STR_TOP,   dx, dy, dz );
	status += Transform( A_RGHT, RGHT, dx, dy, dz );
	status += Transform( A_LFT, LFT,  dx, dy, dz );
	status += Transform( A_BACK, BACK,  dx, dy, dz );
	status += Transform( A_FRONT, FRONT, dx, dy, dz );

	return status;
}



CReturn
CImportUtil::Transform( const CString& source, const CString& target, double dx, double dy, double dz )
{
	CReturn status;
	CDbEntity* dbEntity;
	int count, indx;

	// I'm leaving these in just because I can... though I don't think there is a way
	// to set debug levels for all users of ImportUtil
	if (CReturn::Debug() > 4)
	{
		CReturn ret;
		CString str;
		str.Format("Transform %s by %f, %f to %s", source, dx, dy, target);
		ret.Diagnostic(str);
	}

	CDbWorkplane* planeA;
	m_model->EntityFind( source, (CDbEntity**) &planeA, DBWORKPLANE, DBWORKPLANE );

	CDbWorkplane* planeB;
	m_model->EntityFind( target, (CDbEntity**) &planeB, DBWORKPLANE, DBWORKPLANE );

	if (planeA == NULL || planeB == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CImportUtil::Transform()" );
		return status;
	}

	C3x4Matrix theShift;
	theShift.setUnit();
	theShift.Shift( C3dVec( dx, dy, dz ) );

	// Build the transformation matrix [source] X [target].
	C3x4Matrix xform = planeA->Transform();
	theShift.Transform( &xform );

	C3x4Matrix theXform = planeB->Inverse();
	xform.Transform( &theXform );

	// theShift.Transform( &theXform );

	// Find all of the entities that reside in the 'source' workplane.
	CDbEntityList entities;
	planeA->RefdBy( &entities );

	CDbEntity::NewAction();

	const C3dVec& zA = planeA->Transform().getK();
	const C3dVec& zB = planeB->Transform().getK();

	bool reverse = ((planeA->ToolUp() != planeB->ToolUp()) || (zA * zB < -SMALL));

	// Transform the entities and associate them with the target workplane.
	count = entities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = entities[ indx ];

		// I'm leaving these in just because I can... though I don't think there is a way
		// to set debug levels for all users of ImportUtil
		if (CReturn::Debug() > 4)
		{
			CReturn ret;
			CString str;
			str.Format("... entity %d by %f, %f", dbEntity->Id(), theXform.getT().X(), theXform.getT().Y());
			ret.Diagnostic(str);
		}

		dbEntity->Transform( theXform );

		// Reverse arcs... offsets should be managed automatically.
		if ( reverse )
		{
			CDbArc* dbArc = (CDbArc*) dynamic_cast<const CDbArc*>( dbEntity );
			if (dbArc != NULL)
				dbArc->Dir( -(dbArc->Dir()) );
		}

		dbEntity->Workplane( planeB );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Update the model header with the length, width & thickness of the stock.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// TODO: CImportUtil::HeaderUpdate() does quite a bit more than just update
// the model header; it also normalizes the model data such that it is
// guaranteed to have geometry representing the STR_TOP of the stock.  Likewise,
// the top of the stock is guaranteed to be above the part geometry.  This
// normalization should probably be moved into a separate method.
//
// There are five cases that must be considered:
//    where:  + means above XY plane
//            - means below XY plane
//            <stock means below the stock
//            ? means absent.
//
// ___________________________________________
//       |
// Geom  |               Z Level
// ______|____________________________________
// stock |     0     0      +      ?     ?
// part  |     -     +   <stock    -     +
//
CReturn
CImportUtil::HeaderUpdate(
					const CString&	planeName,
					int				stockDir,
					double			thick,
					C3dBox*			box,
					double*			zStock )
{
	C3dBox tmp;
	C3dBox modelBox;
	C3dBox stockBox;
	CDbEntity* dbEntity;
	CDbTool* stockLayer;
	double	dval;  // to simplify debugging
	int	hold_type;
	CReturn status;

	(*zStock) = UNDEFINED;
	box->Invalidate();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Determine the existence and placement of the 'Stock' geometry.
	// The layer named 'Stock' is an immutable property of the system.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	m_model->EntityFind( STR_STOCK, (CDbEntity**)&stockLayer, DBTOOL, DBTOOL );
	if (stockLayer == NULL)
	{
		m_model->EntityCreate( DBTOOL, (CDbEntity**)&stockLayer );
		stockLayer->Name( STR_STOCK );
		stockLayer->ColorSet( DCOLOR_BLUE );
	}

	if (stockLayer != NULL)
	{
		// The bounding box will be determined by the
		// geometry that lies on the stock layer.

		status = StockLayerProcess( (*stockLayer), &stockBox );

		(*zStock) = stockBox.Zmax();
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Obtain the bounding box of the whole model.  This will be used
	// to determine the placement of the geometry relative to the
	// stock geometry.  In the end, the stock should be positioned
	// as if it were drawn above the model geometry.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	modelBox = m_model->Box();

	if ((*zStock) >= UNDEFINED)
	{
		// We didn't find any stock geometry, so we must create some.

		// Two cases, but in both cases we want to create as if it
		// were drawn in the correct place relative to the geometry.
		// 1) All geometry is below the world XY plane
		// 2) All geometry is above the world XY plane

#if (_CI)
		(*zStock) = ((modelBox.Zmax() < -SMALL) ? 0.0 : thick);
#else
		// Slamming everything to Z0 simplifies the product ;-)
		(*zStock) = 0.;
#endif
		if (modelBox.Dx() >= UNDEFINED || modelBox.Dy() >= UNDEFINED)
		{
			status.Internal( IDS_INVALID_BOUNDING_BOX );
			status = STATUS_OKAY;

			modelBox.Xmin( 0.0 );
			modelBox.Xmax( m_autoModDb->MaterialLength() );
			modelBox.Ymin( 0.0 );
			modelBox.Ymax( m_autoModDb->MaterialWidth() );
		}

		status = StockCreate(
					planeName,
					stockDir,
					modelBox.Xmin(),
					modelBox.Xmax(),
					modelBox.Ymin(),
					modelBox.Ymax(),
					(*zStock) );

		stockBox = modelBox;
	}
	else if ((*zStock) <= SMALL)
	{
		// Assume the stock geometry was drawn on the world XY plane.

		if (modelBox.Zmax() > SMALL)
		{
			// The part geometry was drawn above the stock geometry.
			// Therefore the stock geometry must represent the bottom
			// of the stock.  Move the stock geometry.

			C3x4Matrix theShift;
			theShift.setUnit();
			theShift.Shift( C3dVec( 0.0, 0.0, thick ) );
	
			CDbEntityList stockEntities;
			stockLayer->RefdBy( &stockEntities );

			int count = stockEntities.Count();
			for (int indx = 0; indx < count; ++indx)
			{
				dbEntity = stockEntities[ indx ];
				dbEntity->Transform( theShift );
			}

			(*zStock) = thick;
		}
	}

	(*box) = stockBox;

	// For convenience of accessing data ....
	const CVarList& mach_attribs = m_autoModDb->MachineAttributes();

	// TODO: We may just want to copy all of the machine attributes
	// instead of selectively choosing attributes.  However, if we
	// choose to do that we will have to be careful with the
	// attribute names.

	CVarList* header = m_model->pHeader();
	header->setReal( STR_LENGTH, box->Dx() );
	header->setReal( STR_WIDTH, box->Dy() );
	header->setReal( STR_THICKNESS, thick );

	header->setString( "MachCfg", m_autoModDb->ToolSetup().Name() );
	header->setString( "MatCfg", m_autoModDb->MaterialDesc() );

	header->setInt( "Units", m_autoModDb->Units() );
	header->setInt( STR_PARTPROF, m_autoModDb->CodePartProf() );
	header->setInt( "ScribeWithTorch", mach_attribs.getInt( "Scribe_With_Torch", 0 ) );


	header->setString( "PoNum", "" );
	header->setString( "DueDate", "" );
	header->setString( "Customer", "" );
	header->setString( "ProjNum", "" );

	C2dBox limits = m_autoModDb->MachineLimits();
	header->setReal( STR_XMAX, limits.Xmax() );
	header->setReal( STR_XMIN, limits.Xmin() );
	header->setReal( STR_YMAX, limits.Ymax() );
	header->setReal( STR_YMIN, limits.Ymin() );

	header->setReal( "SpaceTol", m_autoModDb->SpaceTol() );
	header->setReal( "HoleMinusTol", m_autoModDb->HoleMinusTol() );
	header->setReal( "HolePlusTol", m_autoModDb->HolePlusTol() );

	header->setInt( "MachineType", m_autoModDb->MachineType() );
	header->setInt( "WorkplaneType", m_autoModDb->WorkplaneType() );

	const CVarList& mach = m_autoModDb->MachineAttributes();
	int holddown_type = mach.getInt( "Holddown_type", -1 );
	header->setInt( "Holddown_type", holddown_type );

	ClampDataSet( header );

	// Updated by code generator on an 'as needed' basis.
	header->setReal( "CycleTime", 0. );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Record the adjusted material weight.  This is required for
	// machines like the Whitney3700 that adjust table velocity
	// based on the weight of the material.
	//
	const CVarList& material = m_autoModDb->MaterialAttributes();
	double weight = material.getReal( "weight", 0.0 );
	double length = material.getReal( STR_LENGTH, box->Dx() );
	double width  = material.getReal( STR_WIDTH,  box->Dy() );

	weight = weight * ((box->Dx() * box->Dy()) / (length * width));
	header->setReal( "MaterialWeight", weight );

	header->setInt( "Ind_hits", mach_attribs.getInt( "Ind_hits", FALSE ) );

	hold_type = mach_attribs.getInt( "Holddown_Type", -IUNDEFINED );
	if (hold_type >= 0)
	{
		// (0) HOLD_2CIRCLE / (1) HOLD_RECTANGLE
		header->setInt( "hold_type", hold_type );

		dval = mach_attribs.getReal( "Holddown_Location_X1", 0. );
		header->setReal( "hold_x1", dval );

		dval = mach_attribs.getReal( "Holddown_Location_X2", 0. );
		header->setReal( "hold_x2", dval );

		dval = mach_attribs.getReal( "Holddown_Location_Y", 0. );
		header->setReal( "hold_y", dval );

		dval = mach_attribs.getReal( "Holddown_Diameter", 0. );
		header->setReal( "hold_dia", dval );
	}

	return status;
}

CReturn
CImportUtil::ViewPlanesCreate( double length, double width, double thick )
{
	CReturn status;
	CDbWorkplane* dbWork;

	dbWork = PlaneFindCreate( VRIGHT );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 0, 1, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 1, 0, 0 ),		// k
					C3dCoord( length, 0, 0 ),	// t
					1 );					// up

	dbWork = PlaneFindCreate( VLEFT );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 0, -1, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( -1, 0, 0 ),		// k
					C3dCoord( 0, width, 0 ),	// t
					1 );					// up

	dbWork = PlaneFindCreate( VFRONT );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 0, -1, 0 ),		// k
					C3dCoord( length, 0, 0 ),	// t
					1 );					// up

	dbWork = PlaneFindCreate( VBACK );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( -1, 0, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 0, 1, 0 ),		// k
					C3dCoord( length, width, 0 ),	// t
					1 );					// up

	dbWork = PlaneFindCreate( VTOP );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 1, 0 ),		// j
					C3dVec( 0, 0, 1 ),		// k
					C3dCoord( 0, 0, thick ),	// t
					1 );					// up

	dbWork = PlaneFindCreate( VISO );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( .7071, .7071, 0 ),		// i
					C3dVec( -.5, .5, .7071 ),		// j
					C3dVec( .5, -.5, .7071 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up

	return status;
}

CReturn
CImportUtil::StandardPlanesCreate(
							double	length,
							double	width,
							double	zStock,
							int		workType,
							int		machType )
{
	CReturn status;
	CDbWorkplane* dbWork;

	dbWork = PlaneFindCreate( STR_WORLD );
	if (dbWork == NULL)
		return CReturn( STATUS_ERROR );

	dbWork->Hide();
	dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 1, 0 ),		// j
					C3dVec( 0, 0, 1 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up

	// Otherwise, m_model->IsRightHanded() may return the wrong answer.
	// TODO: Find an appropriate central location for this initialization.
	m_model->pHeader()->setInt( "WorkplaneType", workType );
	m_model->pHeader()->setInt( "MachineType", machType );

	if ( m_model->IsRightHanded() )
	{
		// Standard right-handed workplanes (thank the lord).
		//
		// When the workplane type is 2, we are working in the 'lower' (4th)
		// quadrant of a standard right-handed workplane, as required by
		// some punch press and punch/plasma machines.

		dbWork = PlaneFindCreate( RGHT );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec( 0, 1, 0 ),		// i
						C3dVec( 0, 0, 1 ),		// j
						C3dVec( 1, 0, 0 ),		// k
						C3dCoord( length, 0, 0 ),	// t
						1 );					// up

		dbWork = PlaneFindCreate( LFT );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec( 0,-1, 0 ),		// i
						C3dVec( 0, 0, 1 ),		// j
						C3dVec(-1, 0, 0 ),		// k
						C3dCoord( 0, width, 0 ),	// t
						1 );					// up

		dbWork = PlaneFindCreate( FRONT );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
						C3dVec( 0, 0, 1 ),		// j
						C3dVec( 0,-1, 0 ),		// k
						C3dCoord( 0, 0, 0 ),	// t
						1 );					// up

		dbWork = PlaneFindCreate( BACK );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec(-1, 0, 0 ),		// i
						C3dVec( 0, 0, 1 ),		// j
						C3dVec( 0, 1, 0 ),		// k
						C3dCoord( length, width, 0 ),	// t
						1 );					// up

		dbWork = PlaneFindCreate( STR_TOP );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
						C3dVec( 0, 1, 0 ),		// j
						C3dVec( 0, 0, 1 ),		// k
						C3dCoord( 0, ((workType == 2) ? width : 0), zStock ),	// t
						1 );					// up

		return status;
	}
	else if (workType == 2)
	{
		// Freakin' Morbidelli workplanes.

		dbWork = PlaneFindCreate( RGHT );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec( 0, 0, 1 ),		// i
						C3dVec( 0,-1, 0 ),		// j
						C3dVec(-1, 0, 0 ),		// k
						C3dCoord( length, width, 0 ),	// t
						-1 );					// up

		dbWork = PlaneFindCreate( LFT );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec( 0, 0, 1 ),		// i
						C3dVec( 0,-1, 0 ),		// j
						C3dVec( 1, 0, 0 ),		// k
						C3dCoord( 0, width, 0 ),	// t
						-1 );					// up

		dbWork = PlaneFindCreate( FRONT );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
						C3dVec( 0, 0, 1 ),		// j
						C3dVec( 0, 1, 0 ),		// k
						C3dCoord( 0, 0, 0 ),	// t
						-1 );					// up

		dbWork = PlaneFindCreate( BACK );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Hide();
		dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
						C3dVec( 0, 0, 1 ),		// j
						C3dVec( 0,-1, 0 ),		// k
						C3dCoord( 0, width, 0 ),	// t
						-1 );					// up

		dbWork = PlaneFindCreate( STR_TOP );
		if (dbWork == NULL)
			return CReturn( STATUS_ERROR );

		dbWork->Init(	C3dVec( 1, 0, 0 ),		// i
						C3dVec( 0,-1, 0 ),		// j
						C3dVec( 0, 0,-1 ),		// k
						C3dCoord( 0, width, zStock ),	// t
						-1 );					// up

		return status;
	}
	else
	{
		// TODO: Custom work planes (future)
		return status;
	}

	return status;
}

CString
CImportUtil::TokenValue( const CString& string, const CString& myTokenName )
{
	CToken tok;

	tok.setLine( string );

	tok.addDelimit( ' ' );
	tok.addDelimit( '\t' );
	tok.addDelimit( '_' );
	tok.setBreak( TOKEN_BREAK_ALNUM );

	while (TRUE)
	{
		CString tokenName = tok.getToken();
		if (tokenName.IsEmpty())
			break;

		if ( !tokenName.CompareNoCase( myTokenName ) )
		{
			CString tokenValue = tok.getToken();
			return tokenValue;
		}
	}

	return "";
}

#ifdef CUT_THIS_OUT
// ============================================================================
CReturn
CImportUtil::SmartCAMPlanesCreate()
{
	CReturn status;

	CDbWorkplane* plane;

	// XY_PLANE
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 1, 0 ),		// j
					C3dVec( 0, 0, 1 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( XY_PLANE );				// name
	plane->Hide();

	// YZ_PLANE
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( 0, 1, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 1, 0, 0 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( YZ_PLANE );				// name
	plane->Hide();

	// XZ_PLANE
	status = m_model->EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
	if ( !status.IsOk() )
		return status;

	plane->Init(	C3dVec( 1, 0, 0 ),		// i
					C3dVec( 0, 0, 1 ),		// j
					C3dVec( 0, -1, 0 ),		// k
					C3dCoord( 0, 0, 0 ),	// t
					1 );					// up
	plane->Name( XZ_PLANE );				// name
	plane->Hide();

	return status;
}
#endif


CReturn
CImportUtil::SmartCAMPlanesDestroy( const CStringArray& in_names )
{
	CReturn			status;
	CDbWorkplane*	work;

	int num = in_names.GetSize();
	for (int idx=0; idx<num; idx++)
	{
		work = NULL;
		status += m_model->EntityFind( in_names[idx], (CDbEntity**)&work, DBWORKPLANE, DBWORKPLANE );
		if (work != NULL)
			work->Delete();
	}

	return status;
}

CReturn
CImportUtil::SmartCAMPlanesTransform( const CStringArray& in_names, double dx, double dy, double dz )
{
	CReturn			status;
	CDbWorkplane*	source;
	CDbWorkplane*	dest;

	int num = in_names.GetSize();
	for (int idx=0; idx<num; idx++)
	{
		source = NULL;
		status += m_model->EntityFind( in_names[idx], (CDbEntity**)&source, DBWORKPLANE, DBWORKPLANE );
		if (source)
		{
			dest = StandardTargetWork( source->Transform().getK() );
			if (dest)
			{
				status += Transform( source->Name(), dest->Name(), dx, dy, dz );
			}
		}
	}
	return status;
}

CDbWorkplane*
CImportUtil::StandardTargetWork( const C3dVec& normal )
{
	CDbWorkplane* dbWork = NULL;

	// "Extrusion" z-vector, defines OCS
	if (EQUAL( normal.X(), 1.0 ))
		m_model->EntityFind( RIGHT, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.X(), -1.0 ))
		m_model->EntityFind( LEFT, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Y(), 1.0 ))	
		m_model->EntityFind( BACK, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Y(), -1.0 ))		
		m_model->EntityFind( FRONT, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Z(), 1.0 ))
		m_model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	else
	if (EQUAL( normal.Z(), -1.0 ))
		m_model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	return dbWork;
}

CReturn
CImportUtil::ProfilesCreate()
{
	CReturn status;

	if (m_autoModDb != NULL && !m_autoModDb->IsPreview())
	{
		CAutoMod autoMod;
		autoMod.Init( m_autoModDb, m_model );
		status = autoMod.ProfilesCreate();
	}

	return status;
}

CReturn
CImportUtil::StockCreate(
					const CString&	planeName,
					int				stockDir,
					double			xmin,
					double			xmax,
					double			ymin,
					double			ymax,
					double			zStock )
{
	CReturn status;

	CDbTool* dbTool;
	CDbWorkplane* dbWork;
	CDbProfile* dbProfile;

	//=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Prepare for creating the stock geometry.
	//=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Get the workplane that the stock geometry will reference.
	status = m_model->EntityFind( planeName, (CDbEntity**) & dbWork, DBWORKPLANE, DBWORKPLANE );
	if ( !status.IsOk() )
		return status;

	// Fetch or Create the stock layer as necessary.
	status = m_model->EntityFind( STR_STOCK, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	if (dbTool == NULL)
	{
		status = m_model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
		if (dbTool != NULL)
		{
			dbTool->Name( STR_STOCK );
			dbTool->ColorSet( DCOLOR_BLUE );
		}
	}

	if ( !status.IsOk() )
		return status;


	status = m_model->EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );
	if ( !status.IsOk() )
		return status;

 	dbProfile->SystemFlag( true );  

	//=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	C3dCoord pts[5];
	StockPointsOrder( stockDir, xmin, xmax, ymin, ymax, zStock, pts );

	for (int indx = 0; indx < 4; ++indx)
	{
		CDbLine* dbLine;
		status = m_model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
		if ( !status.IsOk() )
			return status;

		dbLine->Init( dbTool, dbWork, pts[indx], pts[indx+1] );
 		dbLine->SystemFlag( true );  
		dbProfile->Append( dbLine );
	}
	
	return status;
}

CReturn
CImportUtil::StockUpdate( double dx, double dy, double dz, int workType, int machType )
{
	CReturn status;
	
	CDbEntityList refs;
	CDbEntityList lines;
	double	ymin, ymax;
	CDbTool* dbTool = NULL;

	m_model->EntityFind( STR_STOCK, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	dbTool->RefdBy( &refs );

	int count = refs.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbLine* dbLine = dynamic_cast<CDbLine*>( refs[indx] );
		if (dbLine != NULL)
			lines.Append( dbLine );
	}

	if (lines.Count() < 4)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CImportUtil::StockUpdate()" );
	}
	else
	{
		// ASSUMPTION: The first four lines are the stock outline.
		// The other lines on the stock layer (which really should
		// not be there) are introduced either by PM4 conversion or
		// maybe by nesting (but I don't yet know).
		
		CDbWorkplane* dbWork;
		status = m_model->EntityFind(
			STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

		CDbLine* dbLineA = dynamic_cast<CDbLine*>( lines[0] );
		CDbLine* dbLineB = dynamic_cast<CDbLine*>( lines[1] );
		CDbLine* dbLineC = dynamic_cast<CDbLine*>( lines[2] );
		CDbLine* dbLineD = dynamic_cast<CDbLine*>( lines[3] );

		C3dCoord ps = dbLineA->StartPt();

		if (workType == 2)
		{
			ymin = -dy;
			ymax = 0.;
		}
		else
		{
			ymin = 0.;
			ymax = dy;
		}

		// The stock outine is built with a CW winding direction.
		dbLineA->Init( dbTool, dbWork, C3dCoord( 0., ymin, ps.Z() ), C3dCoord( 0., ymax, ps.Z() ) );
		dbLineB->Init( dbTool, dbWork, C3dCoord( 0., ymax, ps.Z() ), C3dCoord( dx, ymax, ps.Z() ) );
		dbLineC->Init( dbTool, dbWork, C3dCoord( dx, ymax, ps.Z() ), C3dCoord( dx, ymin, ps.Z() ) );
		dbLineD->Init( dbTool, dbWork, C3dCoord( dx, ymin, ps.Z() ), C3dCoord( 0., ymin, ps.Z() ) );

		dy = fabs(dy);

		// Calling ModifyFlag( true ) will cause a Admin:Regen: to regenerate
		// any toolpath that is associate with the stock outline.
		dbLineA->ModifyFlag( true );
		dbLineB->ModifyFlag( true );
		dbLineC->ModifyFlag( true );
		dbLineD->ModifyFlag( true );

		m_model->pHeader()->setReal( STR_LENGTH, dx );
		m_model->pHeader()->setReal( STR_WIDTH, dy );
		m_model->pHeader()->setReal( STR_THICKNESS, dz );
		m_model->pHeader()->setInt( "WorkplaneType", workType );
		m_model->pHeader()->setInt( "MachineType", machType );

		status = ViewPlanesCreate( dx, dy, dz );

		if ( status.IsOk() )
			status = StandardPlanesCreate( dx, dy, dz, workType, machType );
	}

	return status;
}

CReturn
CImportUtil::SystemFlagsSet()
{
	CReturn status;

	CDbTool* stockLayer;

	m_model->EntityFind( STR_STOCK, (CDbEntity**) &stockLayer, DBTOOL, DBTOOL );

	if (stockLayer == NULL)
	{
		// If this error occurs, the call to SystemFlagsSet()
		// was probably made out of sequence.
		status.Internal( IDS_INTERNAL_ERROR, "CImportUtil::SystemFlagsSet()" );
		return status;
	}

	CDbEntityList dbEntities;
	stockLayer->RefdBy( &dbEntities );

	int count = dbEntities.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbEntities[indx];
		dbEntity->SystemFlag( true );
	}

	return status;
}

// Create a work-zone and put the imported entities into it.
CReturn
CImportUtil::WorkZoneCreate( const C3dBox& box, int zoneNum, double delta )
{
	C3dBox		wzbox;
	CString		name;
	CDbFeature*	workZone;

	CReturn status = m_model->EntityCreate( DBFEATURE, (CDbEntity**) &workZone );

	if ( status.IsOk() )
	{
		// Expand the bounding box to reduce entity selection
		// problems when working in the graphics view.
		wzbox.Xmin( box.Xmin() - delta );
		wzbox.Ymin( box.Ymin() - delta );
		wzbox.Xmax( box.Xmax() + delta );
		wzbox.Ymax( box.Ymax() + delta );

		workZone->ColorSet( DCOLOR_BLUE );
		workZone->StringSet( STR_TYPE, "_zone" );

		workZone->IntSet( "_zone_num", zoneNum );

		// name.Format( "WorkZone%d", zoneNum );
		// workZone->Name( name );
		name.Format( "_repo_zone_%d", zoneNum );
		workZone->SystemName( name );

		CVarList* header = m_model->pHeader();
		int type = header->getInt( "WorkplaneType", 1);
		if (type == 2)
		{
			workZone->DoubleSet( "_zone_bottom",	wzbox.Ymax() );
			workZone->DoubleSet( "_zone_top",		wzbox.Ymin() );
		}
		else
		{
			workZone->DoubleSet( "_zone_bottom",	wzbox.Ymin() );
			workZone->DoubleSet( "_zone_top",		wzbox.Ymax() );
		}

		workZone->DoubleSet( "_zone_left", wzbox.Xmin() );
		workZone->DoubleSet( "_zone_right", wzbox.Xmax() );

		CDbIterator	iter;
		iter.Init( m_model->Db(), DBPOINT );
		while (1)
		{
			CDbEntity* dbEntity = iter();
			if (dbEntity == NULL)
				break;

			if ( (dbEntity != workZone) &&
				 (dbEntity->Owner() == NULL) &&
				 !dbEntity->IsSystem() )
			{
				workZone->Append( dbEntity );
			}

			iter.Next();
		}
	}

	return status;
}

CDbWorkplane*
CImportUtil::PlaneFindCreate( const CString& name )
{
	CDbWorkplane* dbWork;

	m_model->EntityFind( name, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	if (dbWork == NULL)
	{
		m_model->EntityCreate( DBWORKPLANE, (CDbEntity**) &dbWork );
		if (dbWork != NULL)
			dbWork->Name( name );
	}

	return dbWork;
}

// Creates a profile through the curves defined on the stock layer
// and calculates the bounding box.
//
// CONVENTIONS:
//
// The stock definition is restricted to four lines defining a rectangle.
//
// The top of the stock is always drawn in the world coordinate system
// above Z0.  The elevation of the stock geometry therefore determines
// the stock thickness.
//
CReturn
CImportUtil::StockLayerProcess( const CDbTool& stockLayer, C3dBox* box )
{
	CReturn status;

	CDbEntityList stockEntities;
	CDbCurveList curveList;
	C3dBox tmp;
	double ymin = UNDEFINED;

	CDbCurve* seedCurve = NULL;
	CDbProfile* dbProfile = NULL;
	bool reverse = FALSE;


	stockLayer.RefdBy( &stockEntities );

	int count = stockEntities.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = stockEntities[ indx ];

		CDbLine* dbLine = dynamic_cast<CDbLine*>( dbEntity );
		if (dbLine == NULL)
			continue;

		if (dbProfile == NULL)
			dbProfile = dynamic_cast<CDbProfile*>( dbLine->Owner() );

		// Grow the bounding box
		tmp = dbLine->Box( 0 );  // in global
		if ( tmp.IsDefined() )
			(*box) += tmp;

		curveList.Append( dbLine );

		// Determine whether this is the seed curve for the stock profile.
		CGeoCurve* geoCurve = dbLine->Curve();
		C2dUnitVec tan = geoCurve->StartTan();

		if (fabs( tan.Y() ) < SMALL)
		{
			// We've encountered a horizontal line.
			double yp = geoCurve->StartPt().Y();
			if (yp < ymin)
			{
				ymin = yp;
				seedCurve = dbLine;
				reverse = (tan.X() > 0);
			}
		}

		delete geoCurve;
	}

	if (seedCurve != NULL && dbProfile == NULL)
	{
		// There should not be a profile unless we're importing and extended ascii file.
		status = CProfileBuilder::ProfileGrow(
					m_model, SMALL, seedCurve, &curveList, &dbProfile );
	}

	if (dbProfile != NULL && reverse)
		dbProfile->Reverse();

	return status;
}

int
CImportUtil::StockDirection()
{
	int dir = CW;  // the default direction

	int indx = m_autoModDb->SourceLayerFind( STR_STOCK );
	if (indx > 0)
	{
		m_autoModDb->CurrentRecordSet( indx );

		CString toolNoString = m_autoModDb->StringGet( "Station" );
		if (toolNoString.CompareNoCase( "None" ) != 0)
			dir = m_autoModDb->IntGet( "Cut_Direction" );
	}

	return dir;
}

void
CImportUtil::StockPointsOrder(
					int				stockDir,
					double			xmin,
					double			xmax,
					double			ymin,
					double			ymax,
					double			zStock,
					C3dCoord*		pts )
{
	if (stockDir == CW)
	{
		pts[0] = C3dCoord( xmin, ymin, zStock );
		pts[1] = C3dCoord( xmin, ymax, zStock );
		pts[2] = C3dCoord( xmax, ymax, zStock );
		pts[3] = C3dCoord( xmax, ymin, zStock );
		pts[4] = C3dCoord( xmin, ymin, zStock );
	}
	else
	{
		pts[0] = C3dCoord( xmin, ymin, zStock );
		pts[1] = C3dCoord( xmax, ymin, zStock );
		pts[2] = C3dCoord( xmax, ymax, zStock );
		pts[3] = C3dCoord( xmin, ymax, zStock );
		pts[4] = C3dCoord( xmin, ymin, zStock );
	}
}

void
CImportUtil::ClampDataSet( CVarList* header )
{
	double	pcdl, pcdw, pcdc;
	double	tcdl, tcdw, tcdc;
	int		nclamps;

	const CVarList& mach = m_autoModDb->MachineAttributes();

	nclamps = mach.getInt( "Number_Of_Clamps", 0 );

	pcdc = mach.getReal( "Punch_Clamp_Deadzone_Center", 0. );
	pcdl = mach.getReal( "Punch_Clamp_Deadzone_Length", 0. );
	pcdw = mach.getReal( "Punch_Clamp_Deadzone_Width", 0. );

	tcdc = mach.getReal( "Torch_Clamp_Deadzone_Center", 0. );
	tcdl = mach.getReal( "Torch_Clamp_Deadzone_Length", 0. );
	tcdw = mach.getReal( "Torch_Clamp_Deadzone_Width", 0. );

	// Note: According to Gary, VB uses the larger of the two.

	if (pcdl > tcdl)
	{
		header->setReal( "Clamp_Center", pcdc );
		header->setReal( "Clamp_Length", pcdl );
	}
	else
	{
		header->setReal( "Clamp_Center", tcdc );
		header->setReal( "Clamp_Length", tcdl );
	}

	if (pcdw > tcdw)
		header->setReal( "Clamp_Width", pcdw );
	else
		header->setReal( "Clamp_Width", tcdw );

	header->setInt( "Number_Of_Clamps", nclamps );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// BEWARE!  These methods are restricted to analyzing files.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int
CImportUtil::LayerCount()
{
	return m_layers.Count();
}

CString
CImportUtil::LayerName( int index )
{
	CString layerName;

	if (index >= 0 && index < m_layers.Count())
		layerName = (*m_layers[ index ]);

	return layerName;
}

void
CImportUtil::LayerAdd( const CString& layerName )
{
	int count = m_layers.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CString tmp = (*m_layers[ indx ]);
		if ( !tmp.CompareNoCase( layerName ) )
			return;  // Already have the layer name.
	}

	CString* pLayerName = new CString( layerName );
	m_layers.Append( pLayerName );
}

