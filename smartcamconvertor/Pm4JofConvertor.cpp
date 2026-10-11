
#include "stdafx.h"
#include "StringConst.h"
#include "AppConst.h"

#include "Register.h"
#include "2dUnitVec.h"
#include "3dBox.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "Jof.h"
#include "DbAllEntities.h"
#include "Model.h"
#include "DbIterator.h"
#include "AutoModDb.h"
#include "Offsetter.h"
#include "ModelUtil.h"
#include "ProfileBuilder.h"
#include "ImportUtil.h"
#include "MyPm4.h"
#include "Mm2.h"
#include "Pm4JofConvertor.h"

const CString TOP("Top");
const CString REFERENCE("Reference");
const CString PART_OUTLINE("Part_Outline");

static CString REG_ROOT = "";

const double METRIC_GAPTOL = 1;  // mm
const int DIR_AS_IS = 0;

static double EffectiveLength( const CDbTool& dbTool );
static void ColorsSet( CModel* model );
static void CleanUp( CModel* model );

const int MAX_COLORS = 8;
int PM4_COLOR_TABLE [ MAX_COLORS ];
int PM4_DEFAULT_COLOR = RGB( 255, 255, 255 );


////////////////////////////////////////////////////////////////////////

int QSortMyIDCompare( const void* ptrA, const void* ptrB )
{
	CDbEntity* entityA = (*(CDbEntity**) ptrA);
	CDbEntity* entityB = (*(CDbEntity**) ptrB);

	CDbProfile*	profA = dynamic_cast<CDbProfile*>( entityA );
	CDbProfile*	profB = dynamic_cast<CDbProfile*>( entityB );

	if (profA != NULL)
		entityA = (*profA)[0];

	if (profB != NULL)
		entityB = (*profB)[0];

	ID idA = entityA->Id();
	ID idB = entityB->Id();

	return (idA - idB);
}

////////////////////////////////////////////////////////////////////////

CPm4JofConvertor::CPm4JofConvertor()
	: m_debug( FALSE ),
	  m_refLayer( NULL ),
	  m_topWork( NULL )
{
	REG_ROOT = CRegister::RootPathV() + "\\ConfigurationManager";

	PM4_COLOR_TABLE[0] = RGB(   0, 255,   0 );
	PM4_COLOR_TABLE[1] = RGB(   0, 128, 255 );
	PM4_COLOR_TABLE[2] = RGB( 255,   0, 255 );
	PM4_COLOR_TABLE[3] = RGB( 255, 255,   0 );
	PM4_COLOR_TABLE[4] = RGB(   0, 255, 255 );
	PM4_COLOR_TABLE[5] = RGB( 128, 128, 255 );
	PM4_COLOR_TABLE[6] = RGB( 128, 255, 128 );
	PM4_COLOR_TABLE[7] = RGB( 255, 128, 128 );
}

CPm4JofConvertor::~CPm4JofConvertor()
{
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Converts a pm4/jof combination to the equivalent mm2.
//
// Note: The complete process is a little complicated.  Briefly,
// here are some things to remember:
//
// 1. All we-cim entity names must be unique.
// 2. The we-cim application imposes restraints on the names of
//    'tooled' layers, namely, they appears as eg. "tool23".
// 3. Since 'tooled' layer names start with "tool" we can use
//    the same convention for naming tools.
// 4. Tool names (here) use the SmartCAM convention eg "step23"
// 5. SmartCAM geometry (here) references tools by 'layer name'.
//    Hence, a layer whose name is "tool_23" will be linked to
//    the tool whose name is "step_23".
// 6. Since nesting requires closed profile, and since punching
//    operations rarely create closed profiles, we must do some
//    trickery, creating a layer named "Part_Outline" that contains
//    a closed profile whose geometry encompasses all toolpath.
//
CReturn
CPm4JofConvertor::MM2Create(
						const CString&	cmdbPath,
						const CString&	pm4Path,
						const CString&	jofPath,
						const CString&	mm2Path,
						const CString&	machineName,
						const CString&	toolSetupName,
						const CString&	materialName,
						bool			partOutlineCreate )
{
	CReturn status;
	CModel model;
	CAutoModDb autoModDb;
	CMM2 mm2;
	CMyPm4 pm4;
	CJof jof;
	CString msg;

	status = jof.Read( jofPath );

	
	msg.Format( "Read JOF <%s>", jofPath );
	Msg( msg, status );

	if ( status.IsOk() )
	{
		pm4.SetJof( jof );

		model.UndoBufferSuppress();

		Initialize( &model );
		
		Msg( "Initialized", status );

		status = ToolsCreate( jof, &model );
		Msg( "Converted tools", status );

		if ( status.IsOk() )
		{
			status = autoModDb.DbOpen( cmdbPath );
			if ( status.IsOk() )
				status = autoModDb.MachineValuesLoad( autoModDb.MachineID( machineName ) );

			if ( status.IsOk() )
				status = pm4.Read( pm4Path, STR_STOCK, &model, &autoModDb );

			autoModDb.DbClose();

			msg.Format( "Read PM4 <%s>", pm4Path );
			Msg( msg, status );
		}

		if ( status.IsOk() )
		{
			status = ProfilesCreate( &model );
			Msg( "Created profiles", status );
		}

		if ( status.IsOk() )
		{
			status = ToolsLink( &model );
			Msg( "Linked tools", status );
		}

		if ( status.IsOk() && partOutlineCreate )
		{
			status = PartOutlineCreate( &model );
			Msg( "Created Part_Outline", status );
		}

		if ( status.IsOk() )
		{
			CImportUtil	importUtil;

			importUtil.Init( &model, NULL );
			status = importUtil.WorkZoneCreate( model.Box(0), 1, 0.1 );
		}

		if ( status.IsOk() )
		{
			// If a tool setup is created, it will have the
			// same name as the pm4 and jof.
			model.pHeader()->setString( "MachCfg", toolSetupName );

			model.pHeader()->setString( "MatCfg", materialName );

			status = mm2.Write( mm2Path, model );

			msg.Format( "Wrote MM2 <%s>", mm2Path );
			Msg( msg, status );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Converts a jof to its CMDB equivalent (actually subet of tables).
//
// Note: Deletes the target CMDB file and recreates it!
//
CReturn
CPm4JofConvertor::MDBCreate(
						const CString& jofPath,
						const CString& mdbPath )
{
	CReturn status;
	CJof jof;
	
	status = jof.Read( jofPath );
	if ( status.IsOk() )
	{
		TRY
		{
			CFile::Remove( mdbPath );
		}
		CATCH( CFileException, e )
		{
		}
		END_CATCH

		status = jof.DbCreate( mdbPath );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For debugging.
//
void
CPm4JofConvertor::Debug( bool debug )
{
	m_debug = debug;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For debugging.
//
CReturn
CPm4JofConvertor::Dump( const CString& jofPath )
{
	CReturn status;
	CString rootPath;
	CJof jof;
	int indx;

	status = jof.Read( jofPath );
	if ( status.IsOk() )
	{
		indx = jofPath.ReverseFind('\\');
		rootPath = jofPath.Left( indx );

		jof.AttribsDump( rootPath + "\\attribs.txt" );
		jof.ToolsDump( rootPath + "\\tools.txt" );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Establishes commonly referenced entities like the "Top" workplane
// and the "Reference" layer.
//
CReturn
CPm4JofConvertor::Initialize( CModel* model )
{
	CReturn status;

	status = model->EntityCreate( DBWORKPLANE, (CDbEntity**) &m_topWork );

	if ( status.IsOk() )
	{
		m_topWork->Init(
						C3dVec( 1, 0, 0 ),		// i
						C3dVec( 0, 1, 0 ),		// j
						C3dVec( 0, 0, 1 ),		// k
						C3dCoord( 0, 0, 0 ),	// t
						1 );					// up

		m_topWork->Name( TOP );				// name
		m_topWork->Hide();
	}

	status += model->EntityCreate( DBTOOL, (CDbEntity**) &m_refLayer );

	if ( status.IsOk() )
	{
		m_refLayer->Name( REFERENCE );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Chains all curves in each layer.
//
CReturn
CPm4JofConvertor::ProfilesCreate( CModel* model )
{
	const double CLEANTOL = 1.e-4;
	CReturn status;
	C3dBox box;
	CDbTool* dbLayer;
	double xdiff, ydiff, gaptol;
	CDbIterator iter;

	box = model->Box();
	xdiff = box.Dx();
	ydiff = box.Dy();

	if (xdiff > ydiff)
		gaptol = ((xdiff > 100) ? METRIC_GAPTOL : METRIC_GAPTOL / 25.4);
	else
		gaptol = ((xdiff > 100) ? METRIC_GAPTOL : METRIC_GAPTOL / 25.4);
	
	iter.Init( model->Db(), DBTOOL );
	while (1)
	{
		dbLayer = dynamic_cast<CDbTool*>( iter() );
		if (dbLayer == NULL)
			break;

		// Must be careful not to chain entities on Layer_99 because
		// this generates an empty profile when creating the Part_Outline
		//
		if (dbLayer->Name().CompareNoCase("Layer_99") != 0)
			status += CProfileBuilder::ProfileLayer(
				model, dbLayer, gaptol, CLEANTOL, FALSE, DIR_AS_IS, TRUE );

		iter.Next();
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Instantiates the tools of a jof in the model.
//
CReturn
CPm4JofConvertor::ToolsCreate( const CJof& jof, CModel* model )
{
	CReturn status;
	CString name;
	CString face;
	CDbTool* dbTool;
	CVarList* toolAttribs;

#if BEFORE_2004_09_09
	int count = jof.ToolsCount();
#else
	int count = jof.StepsCount();
#endif

	for (int indx = 0; indx < count; ++indx)
	{
		status = model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
		if ( !status.IsOk() )
			break;

		toolAttribs = dbTool->pAttrib();
		jof.ToolAttribsGet( indx, toolAttribs );

		name = toolAttribs->getString( "step.user_name", "" );
		if ( !name.IsEmpty() )
		{
			name.Format( "step_%d", name );
			dbTool->Name( name );
		}

		// This next statement is statement is necessary to get
		// tools to appear in the Tools dialog of the application.
		dbTool->StringSet( "Workplane", "Top" );

		// Associate the tool with a workplane.
		dbTool->Workplane( m_topWork );

		dbTool->ColorSet( PM4_COLOR_TABLE[indx%MAX_COLORS] );
	}
	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// A post processing function, links the converted pm4 geometry to
// its tooling.
//
CReturn
CPm4JofConvertor::ToolsLink( CModel* model )
{
	CReturn status;

	status = HolesPunch( model );

	if ( status.IsOk() )
		status = ProfilesCut( model );

	ColorsSet( model );

	CleanUp( model );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Holes are cloned and tooled into features.  The original holes are
// then moved to the 'Reference' layer.
//
CReturn
CPm4JofConvertor::HolesPunch( CModel* model )
{
	CReturn status;

	CDbIterator iter;
	CDbEntityList dbHoles;
	CDbTool* dbTool;
	CDbHole* dbHole;
	CDbHole* dbClone;
	CString name;
	double orient;
	int count, indx;

	// Get all of the hole entities.
	iter.Init( model->Db(), DBHOLE );
	while (1)
	{
		dbHole = dynamic_cast<CDbHole*>( iter() );
		if (dbHole == NULL)
			break;

		dbHoles.Append( dbHole );

		iter.Next();
	}

	// Match the holes to a tool and move the hole into a feature.
	count = dbHoles.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbHole = dynamic_cast<CDbHole*>( dbHoles[indx] );

		name = dbHole->Tool()->Name();

		model->EntityFind( name, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

		status = MoveToLayer( dbHole, m_refLayer );

		// Clone the hole.
		model->EntityCreate( DBHOLE, (CDbEntity**) &dbClone );
		dbClone->Init( dbTool, dbTool->Workplane(), dbHole->Center(), 0.1, 0. );

		if ( dbTool->IsIndexable() )
		{
			// It seems SmartCAM is 180 degrees different than WE-CIM.
			// orient = dbTool->DoubleGet( "Index_Angle", 0. ) + 180.;
			orient = dbHole->DoubleGet( "clear", 0. ) * RAD2DEG;
			dbClone->DoubleSet( "orient", orient );
		}

		iter.Next();
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// All profiles and curves are offset as required with the appropriate
// tooling.  Note: Since CModelUtil::ProfileLayer() does not appear to
// put 'single curves' into profiles, we must be careful not to select
// curves that belong to a profile.
//
CReturn
CPm4JofConvertor::ProfilesCut( CModel* model )
{
	CReturn status;

	CDbIterator		iter;
	CDbEntityArray	tmp;
	CDbEntityList	dbEntities;
	CDbEntity*		dbEntity;
	CDbTool*		dbTool;
	CString			name;
	int				count, indx;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the profilable entities in historical order.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( model->Db(), DBLINE );
	while (1)
	{
		dbEntity = iter();
		if (dbEntity == NULL)
			break;

		if (dbEntity->Type() > DBPROFILE)
			break;

		if (dbEntity->Owner() == NULL)
		{
			if (dbEntity->Type() != DBHOLE)
				tmp.Append( dbEntity );
		}

		iter.Next();
	}

	tmp.Qsort( &QSortMyIDCompare );

	count = tmp.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntities.Append( tmp[indx] );
	}

	tmp.BenignFlush();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Cut the entities in historical order.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = dbEntities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = dbEntities[indx];

		dbTool = dbEntity->Tool();
		name = dbTool->Name();

		if (name.Find("step_") == 0)
		{
			status += ProfileCut( model, dbEntity, dbTool );
		}

		iter.Next();
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create the toolpath for the given entity, based upon the type of
// the associated tool.  The original entity is then moved to the
// 'Reference' layer.
//
CReturn
CPm4JofConvertor::ProfileCut( CModel* model, CDbEntity* dbEntity, CDbTool* dbTool )
{
	CReturn		status;
	CDbFeature*	dbFeature;
	CDbProfile*	dbProfile;
	CDbEntity*	attrEntity;
	CString		dropstop;
	int			cutside, count;
	int			codePartProfile;

	codePartProfile = model->Header().getInt( STR_PARTPROF, 0 );
	if ( codePartProfile )
		model->pDefault()->setInt( STR_PARTPROF, codePartProfile );

	status = MoveToLayer( dbEntity, m_refLayer );

	if ( status.IsOk() )
	{
		if ( dbTool->IsPunchTool() && !dbTool->IsRoundTool() )
		{
			status = FakePunchOffset( model, dbEntity, dbTool );
		}
		else
		{
			dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

			if (dbProfile == NULL)
			{
				cutside = dbEntity->IntGet( "cutside", 0 );
				attrEntity = dbEntity;
			}
			else
			{
				cutside = (*dbProfile)[0]->IntGet( "cutside", 0 );
				count = dbProfile->Count();
				attrEntity = (*dbProfile)[count-1];
			}

			if ( codePartProfile )
			{
				dbFeature = VerbatimProfileCut( model, dbEntity, dbTool );
			}
			else
			{
				model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );

				status = COffsetter::UniformOffset(
										model,
										dbEntity->Workplane(),
										dbTool,
										dbEntity,
										cutside,
										0.,
										30.,			// degrees
										0.,				// Z0
										dbFeature );	// recipient feature
			}

			dropstop = attrEntity->StringGet( "@DROPSTOP", "" );
			if ( !dropstop.IsEmpty() )
			{
				count = dbFeature->Count();
				if (count > 0)
				{
					dbProfile = dynamic_cast<CDbProfile*>( (*dbFeature)[0] );
					if (dbProfile != NULL)
					{
						CDbEntity*	target;

						count = dbProfile->Count();
						target = (*dbProfile)[count-1];

						target->IntSet( "_dropstop", STOP_CONST );
						target->DoubleSet( "_slide_dx", 0. );
						target->DoubleSet( "_slide_dy", 0.);
					}

					attrEntity->AttribDelete("@DROPSTOP");
				}
			}
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Offset and trim the given entity (this accounts for SmartCAM's
// part profile modeling method).
//
CReturn
CPm4JofConvertor::FakePunchOffset( CModel* model, CDbEntity* refEntity, CDbTool* dbTool )
{
	CReturn status;

	CDbFeature* dbFeature;
	CDbProfile* dbProfile;
	CDbProfile* refProfile;
	CDbCurve* refCurve;
	CDbCurve* dbOffsetCurve;
	CString dropstop;
	int count, indx;

	refCurve = dynamic_cast<CDbCurve*>( refEntity );
	refProfile = dynamic_cast<CDbProfile*>( refEntity );

	status = model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
	status += model->EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );
	if ( status.IsOk() )
	{
		if (refProfile != NULL)
		{
			count = refProfile->Count();
			for (indx = 0; indx < count; ++indx)
			{
				refCurve = dynamic_cast<CDbCurve*>( (*refProfile)[indx] );

				dbOffsetCurve = OffsetCurveCreate( model, refCurve, dbTool );

				if (dbOffsetCurve != NULL)
				{
					dropstop = refCurve->StringGet( "@DROPSTOP", "" );
					if ( !dropstop.IsEmpty() )
					{
						refCurve->AttribDelete("@DROPSTOP");
						dbOffsetCurve->StringSet( "@DROPSTOP", dropstop );
					}

					dbProfile->Append( dbOffsetCurve );
				}
			}

			dbFeature->Append( dbProfile );
		}
		else if (refCurve != NULL)
		{
			dbOffsetCurve = OffsetCurveCreate( model, refCurve, dbTool );

			if (dbOffsetCurve != NULL)
			{
				dropstop = refCurve->StringGet( "@DROPSTOP", "" );
				if ( !dropstop.IsEmpty() )
				{
					refCurve->AttribDelete("@DROPSTOP");
					dbOffsetCurve->StringSet( "@DROPSTOP", dropstop );
				}

				dbProfile->Append( dbOffsetCurve );
			}

			dbFeature->Append( dbProfile );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CDbCurve*
CPm4JofConvertor::OffsetCurveCreate( CModel* model, CDbCurve* dbCurve, CDbTool* dbTool )
{
	CDbCurve* dbOffsetCurve = NULL;

	CDbLine* dbOffsetLine;
	CDbArc* dbOffsetArc;
	CGeoCurve* geoCurve;
	CGeoCurve* offCurve;
	CGeoArc* geoArc;
	C2dUnitVec ts, te;
	C3dCoord ps, pe;
	double orient;
	double length, width;	
	int cutside;

	cutside = dbCurve->IntGet( "cutside", 0 );

	// Get the tooling parameters.
	length = EffectiveLength( (*dbTool) ) / 2;
	width = dbTool->DoubleGet( "width", 0. ) / 2;

	if (length < SMALL)
		length = width;  // assuming square tool

	// Clone/Offset the line to the appropriate side.
	// (a cutside == 0 creates a clone)
	geoCurve = dbCurve->Curve();
	offCurve = geoCurve->Offset( cutside, width );

	ps = offCurve->StartPt();
	pe = offCurve->EndPt();

	if (cutside != 0)
	{
		// Trim the end points back so the tool will not
		// gouge adjacent geometry.  This is a BIG assumption,
		// of course.
		ts = geoCurve->StartTan();
		te = geoCurve->EndTan();

		ps += (ts * length);
		pe += (te * -length);
		offCurve->StartPt( ps );
		offCurve->EndPt( pe );
	}

	// Instantiate the offset curve in the database
	if (dynamic_cast<CGeoLine*>( offCurve ) != NULL)
	{
		if ( ps.WithinTol( pe, SMALL ) )
		{
			// Though trimmed offset curve vanishes, so lie about
			// the size of the resulting offset curve just to get
			// into the system.  Alternatively, we could generate
			// a hole entity at diminished location.

			pe = ps + (te * 0.001);
		}

		model->EntityCreate( DBLINE, (CDbEntity**) &dbOffsetLine );

		dbOffsetLine->Init( dbTool, dbCurve->Workplane(), ps, pe );

		dbOffsetCurve = dbOffsetLine;
	}
	else if (dynamic_cast<CGeoArc*>( offCurve ) != NULL)
	{
		geoArc = dynamic_cast<CGeoArc*>( offCurve );
		model->EntityCreate( DBARC, (CDbEntity**) &dbOffsetArc );

		dbOffsetArc->Init(
					dbTool,
					dbCurve->Workplane(),
					ps, pe, geoArc->CenterPt(), geoArc->Dir() );

		dbOffsetCurve = dbOffsetArc;
	}
	else
	{
		dbOffsetCurve = NULL;
	}

	delete offCurve;

	if (dbOffsetCurve != NULL)
	{
		// Inherit the attributes of the original line and
		// set the tool orientation for this line.
		(*(dbOffsetCurve->pAttrib())) = dbCurve->Attrib();

		if ( dbTool->IsIndexable() )
		{
			// It seems SmartCAM is 180 degrees different than WE-CIM.
			orient = ts.Radians() * RAD2DEG;
			dbOffsetCurve->DoubleSet( "orient", orient );
		}

		dbOffsetCurve->IntSet( "cutside", 0 );
	}
	
	return dbOffsetCurve;
}

CDbFeature*
CPm4JofConvertor::VerbatimProfileCut( CModel* model, CDbEntity* refEntity, CDbTool* dbTool )
{
	CDbFeature*	dbFeature;
	CDbProfile*	dbProfile;
	CDbProfile*	refProfile;
	CDbCurve*	refCurve;

	refCurve = dynamic_cast<CDbCurve*>( refEntity );
	refProfile = dynamic_cast<CDbProfile*>( refEntity );

	model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
	model->EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );

	if (refProfile != NULL)
	{
		while (refProfile->Count() > 0)
		{
			refCurve = dynamic_cast<CDbCurve*>( (*refProfile)[0] );
			refProfile->Disown( refCurve );

			refCurve->Tool( dbTool );
			dbProfile->Append( refCurve );
		}
	}
	else if (refCurve != NULL)
	{
		refCurve->Tool( dbTool );
		dbProfile->Append( refCurve );
	}

	dbFeature->Append( dbProfile );

	return dbFeature;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Move the given entity to the given layer.  This also sets
// the entity's tool to NULL.
//
CReturn
CPm4JofConvertor::MoveToLayer( CDbEntity* dbEntity, CDbTool* dbLayer )
{
	CReturn status;
	CDbContainer* dbContainer;
	int count, indx;

	dbEntity->Tool( dbLayer );

	dbContainer = dynamic_cast<CDbContainer*>( dbEntity );
	if (dbContainer != NULL)
	{
		count = dbContainer->Count();
		for (indx = 0; indx < count; ++indx)
		{
			(*dbContainer)[indx]->Tool( dbLayer );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Creates a closed part outline.
// First attempts to use part geometry.  Failing that, creates a
// rectangular boundary encompassing all of the tooled geoemetry.
//
// NOTE: The resulting part outline will most likely be undesirable
// for all but the simplest of cases.  This will have to be addressed
// in the future.
//
CReturn
CPm4JofConvertor::PartOutlineCreate( CModel* model )
{
	CReturn status;
	CDbTool* refLayer;
	CDbProfile* dbProfile = NULL;

	model->pDefault()->setColor( RGB(255,0,0) );

	model->EntityFind( "Layer_99", (CDbEntity**) &refLayer, DBTOOL, DBTOOL );
	if (refLayer != NULL)
	{
		// SmartCAM layer 99 takes precedence over all.
		dbProfile = PartOutlineCreate( model, refLayer );
	}

#if TODO
	if (dbProfile == NULL)
	{
		// Next, try the "Reference" layer.
		dbProfile = PartOutlineCreate( model, m_refLayer );
	}
	
	if (dbProfile == NULL)
	{
		// Punt ....
		PartBoundingBoxCreate( model );
	}
#endif

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
CDbProfile*
CPm4JofConvertor::PartOutlineCreate( CModel* model, CDbTool* refLayer )
{
	CDbTool* dbLayer;
	CDbProfile* dbProfile = NULL;
	CDbCurve* seedCurve;
	CDbCurve* dbCurve;
	CDbEntityList dbEntities;
	CDbCurveList dbCurves;
	double gaptol;
	int count, indx;

	refLayer->RefdBy( &dbEntities );

	// Copy any curves on the reference layer.
	count = dbEntities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbCurve = dynamic_cast<CDbCurve*>( dbEntities[indx] );
		if (dbCurve != NULL)
		{
			dbCurves.Append( dbCurve );
		}
	}

	// Attempt to create a profile through the curves.
	count = dbCurves.Count();
	if (count > 0)
	{
		SeedCurveFind( dbCurves, &seedCurve, &gaptol );

		CProfileBuilder::ProfileGrow( model, gaptol, seedCurve, &dbCurves, &dbProfile );

		if ( ProfileMend( model, dbProfile ) )
		{
			model->EntityCreate( DBTOOL, (CDbEntity**) &dbLayer );

			dbLayer->Name( PART_OUTLINE );
			dbLayer->Workplane( m_topWork );

			MoveToLayer( dbProfile, dbLayer );
		}
		else
		{
			model->EntityDelete( dbProfile->Id() );
			dbProfile = NULL;
		}
	}

	return dbProfile;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Closes any gaps in the profile.
//
bool
CPm4JofConvertor::ProfileMend( CModel* model, CDbProfile* dbProfile )
{
	CDbCurve* dbCurveA;
	CDbCurve* dbCurveB;
	CDbLine* dbLine;
	CDbArc* dbArc;
	C3dCoord ps, pe;
	int count, indxA, indxB;

	if (dbProfile == NULL)
		return FALSE;  // early exit (no profile)

	count = dbProfile->Count();
	if (count < 1)
		return FALSE;  // early exit (general failure)

	if (count == 1)
	{
		dbCurveA = dynamic_cast<CDbCurve*>( (*dbProfile)[0] );

		dbLine = dynamic_cast<CDbLine*>( dbCurveA );
		if (dbLine != NULL)
			return FALSE;

		dbArc = dynamic_cast<CDbArc*>( dbCurveA );
		if (dbArc == NULL)
			return FALSE;

		ps = dbArc->StartPt();
		pe = dbArc->EndPt();

		if ( !ps.WithinTol( pe, SMALL ) )
		{
			dbLine = LineCreate( model, dbArc->Tool(), dbArc->Workplane(), ps, pe );
			dbProfile->Append( dbLine );
		}
	}
	else if (count > 1)
	{
		CDbCurveList temp;

		// Copy the profile curves to a temporary buffer.
		for (indxA = 0; indxA < count; ++indxA)
		{
			dbCurveA = dynamic_cast<CDbCurve*>( (*dbProfile)[indxA] );
			temp.Append( dbCurveA );
		}
		dbProfile->BenignFlush();

		// Close any gaps in the profile.
		indxA = 0;
		indxB = 1;
		while (indxA < count)
		{
			if (indxB >= count)
				indxB = 0;	// wrap condition

			dbCurveA = temp[indxA];
			dbCurveB = temp[indxB];

			ps = dbCurveA->EndPt();
			pe = dbCurveB->StartPt();

			dbProfile->Append( dbCurveA );

			if ( !ps.WithinTol( pe, SMALL ) )
			{
				dbLine = LineCreate( model, dbCurveA->Tool(), dbCurveA->Workplane(), ps, pe );
				dbProfile->Append( dbLine );
			}

			++indxA;
			++indxB;
		}
	}

	return TRUE;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
CDbLine*
CPm4JofConvertor::LineCreate(
						CModel*			model,
						CDbTool*		dbTool,
						CDbWorkplane*	dbWork,
						const C3dCoord&	ps,
						const C3dCoord&	pe )
{
	CDbLine* dbLine;

	model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );

	dbLine->Init( dbTool, dbWork, ps, pe );

	return dbLine;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Creates a closed rectangular bounding box that encompasses
// all model geometry.
//
CReturn
CPm4JofConvertor::PartBoundingBoxCreate( CModel* model )
{
	CReturn status;
	CDbFeature* dbFeature;
	CDbProfile* dbProfile;
	CDbTool* dbTool;
	CDbLine* dbLine;
	C3dBox box;
	int count;

	count = model->Db().Count( DBFEATURE );
	if (count > 0)
	{
		CDbIterator iter;

		iter.Init( model->Db(), DBFEATURE );
		while (1)
		{
			dbFeature = dynamic_cast<CDbFeature*>( iter() );
			if (dbFeature == NULL)
				break;

			box += dbFeature->Box();

			iter.Next();
		}

		if ( box.IsDefined() )
		{
			model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
			dbTool->Name( PART_OUTLINE );

			model->EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );

			//--------------------------

			model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
			dbLine->Init(
						dbTool, m_topWork,
						C3dCoord( box.Xmin(), box.Ymin(), 0. ),
						C3dCoord( box.Xmin(), box.Ymax(), 0. ) );

			dbProfile->Append( dbLine );

			//--------------------------

			model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
			dbLine->Init(
						dbTool, m_topWork,
						C3dCoord( box.Xmin(), box.Ymax(), 0. ),
						C3dCoord( box.Xmax(), box.Ymax(), 0. ) );

			dbProfile->Append( dbLine );

			//--------------------------

			model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
			dbLine->Init(
						dbTool, m_topWork,
						C3dCoord( box.Xmax(), box.Ymax(), 0. ),
						C3dCoord( box.Xmax(), box.Ymin(), 0. ) );

			dbProfile->Append( dbLine );

			//--------------------------

			model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
			dbLine->Init(
						dbTool, m_topWork,
						C3dCoord( box.Xmax(), box.Ymin(), 0. ),
						C3dCoord( box.Xmin(), box.Ymin(), 0. ) );

			dbProfile->Append( dbLine );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Finds the left-most curve (assumed to be part of the outer boundary)
// and also give some sense of scale so the curves can be chained.
//
void
CPm4JofConvertor::SeedCurveFind(
							const CDbCurveList&	dbCurves,
							CDbCurve**			seedCurve,
							double*				gaptol )
{
	C3dBox box;
	CDbCurve* dbCurve;
	double xmin = UNDEFINED;
	double xmax = -UNDEFINED;
	double ymin = UNDEFINED;
	double ymax = -UNDEFINED;
	double xdiff, ydiff;
	int count, indx;

	count = dbCurves.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbCurve = dbCurves[indx];

		box = dbCurve->Box();
		if (box.Xmin() < xmin)
		{
			xmin = box.Xmin();
			(*seedCurve) = dbCurve;
		}

		if (box.Xmax() > xmax)
			xmax = box.Xmax();

		if (box.Ymin() < ymin)
			ymin = box.Ymin();

		if (box.Ymax() > ymax)
			ymax = box.Ymax();
	}

	xdiff = xmax - xmin;
	ydiff = ymax - ymin;

	if (xdiff > ydiff)
	{
		(*gaptol) = ((xdiff > 100) ? METRIC_GAPTOL : METRIC_GAPTOL / 25.4);
	}
	else
	{
		(*gaptol) = ((xdiff > 100) ? METRIC_GAPTOL : METRIC_GAPTOL / 25.4);
	}
}

void
CPm4JofConvertor::Msg( const char* what, const CReturn& status )
{
	if ( m_debug )
	{
		CReturn status;
		CString msg;
		msg.Format( "%s - %s", what, (status.IsOk() ? "OK" : "FAILED") );
		status.Diagnostic( msg );
		// MessageBox( NULL, msg, "Debug", MB_OK );
	}
}

double EffectiveLength( const CDbTool& dbTool )
{
	double length, width, rise, radius;

	const CVarList& attribs = dbTool.Attrib();
	int type = attribs.getInt( "Type_ID", 0 );

	switch (type)
	{
	case TTYPE_SQUARE:
		length = attribs.getReal( "Width", 0. );
		break;
	case TTYPE_RECTANGLE:
		length = attribs.getReal( "Length", 0. );
		break;
	case TTYPE_OBROUND:
		width = attribs.getReal( "Width", 0. );
		length = attribs.getReal( "Length", 0. ) - (2 * width);
		break;
	case TTYPE_DIAMOND:
		length = attribs.getReal( "Length", 0. );
		break;
	case TTYPE_SINGLE_D:
		width = attribs.getReal( "Width", 0. );
		radius = attribs.getReal( "Diameter", 0. ) / 2;
		rise = fabs( width - radius );
		length = 2 * sqrt( radius*radius - rise*rise );
		break;
	case TTYPE_DOUBLE_D:
		width = attribs.getReal( "Width", 0. ) / 2;
		radius = attribs.getReal( "Diameter", 0. ) / 2;
		length = 2* sqrt( radius*radius - width*width );
		break;
	case TTYPE_TRAPEZOID:
		length = attribs.getReal( "Length", 0. );
		break;
	default:
		length = 0.;
		break;
	}

	return length;
}

void ColorsSet( CModel* model )
{
	CDbEntity*	dbEntity;
	CDbTool*	dbTool;
	CString		name;
	int			color;

	CDbIterator	iter;

	iter.Init( model->Db(), DBLINE );
	while (1)
	{
		dbEntity = iter();
		if (dbEntity == NULL || dbEntity->Type() > DBFEATURE)
			break;

		dbTool = dbEntity->Tool();
		if (dbTool != NULL)
		{
			color = dbTool->ColorGet( PM4_DEFAULT_COLOR );
			dbEntity->ColorSet( color );
		}

		iter.Next();
	}
}

void CleanUp( CModel* model )
{
	CString		name;
	CDbFeature*	dbFeature;
	CDbProfile*	dbProfile;
	CDbTool*	dbTool;

	CDbIterator	iter;

	iter.Init( model->Db(), DBTOOL );
	while (1)
	{
		dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		name = dbTool->Name();
		if (name.Find("step_") == 0)
			dbTool->Name("");
		else if (dbTool->RefCnt() < 1)
			model->Db().Delete( (CDbEntity**) &dbTool );

		iter.Next();
	}

	iter.Init( model->Db(), DBPROFILE );
	while (1)
	{
		dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		if (dbProfile->Count() < 1)
			model->Db().Delete( (CDbEntity**) &dbProfile );

		iter.Next();
	}

	iter.Init( model->Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if (dbFeature->Count() < 1)
			model->Db().Delete( (CDbEntity**) &dbFeature );

		iter.Next();
	}
}

