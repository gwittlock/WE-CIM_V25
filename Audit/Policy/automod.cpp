
#include "stdafx.h"
#include <afx.h>
#include <math.h>
#include "cmn_resource.h"
#include "ColorConst.h"
#include "MathConst.h"
#include "StringConst.h"
#include "Register.h"

#include "DbAllEntities.h"
#include "DbIterator.h"
#include "AutoModDb.h"
#include "DbToolGen.h"
#include "AutoMod.h"
#include "AutoTool.h"
#include "Conversion.h"
#include "Offsetter.h"
#include "ModelUtil.h"
#include "ProfileBuilder.h"

#include "PunchDecomposer.h"
#include "PunchMatcher.h"
#include "AutoPuncher.h"

static CString CAD_LAYER( "CAD_Layer" );
static CString CAM_LAYER( "CAM_Layer" );
static CString TOOL_TYPE( "Tool_Type" );
static CString STATION( "Station" );
static CString DISTANCE( "Distance" );
static CString ZLEVEL( "Z_Level" );
static CString ROUTER_BIT( "Router_Bit" );
static CString AUTO_TOOL( "**Auto**" );
static CString NO_TOOL( "None" );
static CString PLUNGE( "Plunge" );

// TODO: Work out the intricacies of the various feed and speed attributes
//static CString PLUNGE( "Plunge" );


////////////////////////////////////////////////////////////////////////

CAutoMod::CAutoMod()
	: m_autoModDb( NULL ),
	  m_model( NULL ),
	  m_parameterizedTools()
{
}

CAutoMod::~CAutoMod()
{
	m_parameterizedTools.DestructiveFlush();
}

// NOTE: The ToolSetup of the autoModDb might be modified when
//       auto-punching with 'build tool setup' set to true.
void
CAutoMod::Init( CAutoModDb* autoModDb, CModel* model )
{
	m_autoModDb = autoModDb;
	m_model = model;
	m_parameterizedTools.DestructiveFlush();
}

CReturn
CAutoMod::ModelPostProcess( bool buildToolSetup, bool raw )
{
	CReturn		status;
	CDbToolGen	gen;
	double		gapTol;
	int			count, indx;

	status = gen.DbToolsCreate( m_autoModDb->TooledStations(), m_model );
	if ( !status.IsOk() )
		return status;

	PunchToolsParameterize();

	if ( raw )
	{
		gapTol = m_autoModDb->GapTol();
		DuplicatesFilter( m_model, gapTol );
	}
	else
	{
		count = m_autoModDb->Count();

		// For each layer in the database, perform the specified actions.
		for (indx = 0; indx < count; ++indx)
		{
			status = LayerPostProcess( indx, buildToolSetup );

			if ( !status.IsOk() )
				break;
		}
	}

	return status;
}

// private
CReturn
CAutoMod::LayerPostProcess( int recIndx, bool buildToolSetup )
{
	CReturn			status;
	CString			toolNoString;
	CDbEntityList	profilableEntities;
	CDbEntityList	entities;
	CDbTool*		dbLayer;
	CDbTool*		dbTool;
	int				count, indx;
	int				toolNo;


	m_autoModDb->CurrentRecordSet( recIndx );

	// Get the CAM Layer associated with the current database record.
	dbLayer = CamLayer();
	if (dbLayer == NULL)
	{
		// The layer wasn't created because there was nothing on it.
		return CReturn( STATUS_OKAY );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	PartProfilesGet( dbLayer, &profilableEntities );

	// Associate Auto-tool'd or fixed-tool'd feature with
	// the geometry of this layer as required.
	toolNoString = m_autoModDb->StringGet( STATION );

	if (toolNoString.CompareNoCase( AUTO_TOOL ) == 0)
	{
		// Auto-tool as many holes as possible.

		status = AutoTooledFeaturesCreate( &profilableEntities, buildToolSetup );
	}
	else if (toolNoString.CompareNoCase( NO_TOOL ) != 0)
	{
		// We've been requested to use a specific tool.
		// We must still auto-tool what we can, however.

		toolNo = m_autoModDb->IntGet( STATION );

		dbTool = CModelUtil::DbToolFind( m_model->Db(), STR_NC_CODE_NUMBER, toolNo );

		if (dbTool == NULL)
		{
			// We've failed to find the specified tool.
			status.Internal( IDS_AUTOMOD_NO_TOOL, dbLayer->Name() ); 

			// Auto-tool as many holes as possible.
			status = AutoTooledFeaturesCreate( &profilableEntities, buildToolSetup );
		}
		else
		{
			// We've found the specified tool.  The tool type affects
			// the order in which associated operations are generated.

			// Tool-up any imported points that are associated with this tool.
			// Also, process and circle if this is a drilling tool.
			// See also: comments at top of CAutoMod::HoledFeaturesCreate()
			status += HoledFeaturesCreate( dbTool );

			// Auto-tool as many holes as possible.
			// Where punch-capable machines are concerned, some
			// profile shapes may be recognized as punchable.  As
			// such, they will be punched and then removed from
			// further consideration.
			status += AutoTooledFeaturesCreate( &profilableEntities, buildToolSetup );

			// Create some type of offset toolpath for the remaining profiles/curves/holes.
			status += ProfiledFeaturesCreate( profilableEntities, dbTool );

			status += CommandsProcess( dbLayer, dbTool );
		}
	}

	dbLayer->RefdBy( &entities );
	count = entities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = entities[ indx ];
		dbEntity->AttribDelete( "Used" );
	}

	return status;
}

// Hopefully the concept of processing holes is sufficiently abstracted
// such that when we move to fab, we won't have a huge rewrite.
CReturn
CAutoMod::AutoTooledFeaturesCreate(
							CDbEntityList*	profilableEntities,
							bool			buildToolSetup )
{
	CReturn status;

	// Get the CAM Layer associated with the current database record.
	CDbTool* dbLayer = CamLayer();
	if (dbLayer == NULL)
	{
		// The layer wasn't created because there was nothing on it.
		return status;
	}

	// NOTE: Whether we auto-drill any holes is determined by the 'Type ID' field.
	// Said field will have a value of zero when either 1) station is set to 'None'
	// or 2) station is set to a specific tool.
	int toolTypeId = m_autoModDb->IntGet( TOOL_TYPE );
	if (toolTypeId > 0)
	{
		CDbEntityList candidateTools;
		SimilarToolsFind( toolTypeId, &candidateTools );
		if (candidateTools.Count() > 0)
		{
			CDbEntityList holes;
			PartCirclesGet( dbLayer, &holes );
			if (holes.Count() > 0)
			{
				status += AutoDrill( candidateTools, HolePlusTol(), HoleMinusTol(), &holes );
			}
		}
	}

	if (profilableEntities != NULL && IsPunchCapable())
	{
		// Auto-punch all profiles, except the outermost profile.
		// Those profiles that can not be auto-punched, remain the
		// in the list of profilable entities.

		CAutoPuncher	puncher;
		CDbEntity*		dbProfile;
		CDbEntity*		outermostProfile = NULL;
		EMatchingRigour	rigour = RIGOUR_DECOMPOSE;
		int				depth;

		// BugID: 406 -- AutoPunch is wrongly applied to outside profile
		int count = profilableEntities->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			dbProfile = (*profilableEntities)[indx];
			depth = dbProfile->IntGet( STR_PROFILE_DEPTH, -1 );
			if (depth == 0)
			{
				outermostProfile = dbProfile;
				profilableEntities->Remove( indx );
				break;
			}
		}
		
		puncher.AutoPunch(
						profilableEntities,
						m_model,
						&(m_autoModDb->ToolSetup()),
						buildToolSetup,
						HolePlusTol(),
						HoleMinusTol(),
						rigour );

		if (outermostProfile != NULL)
		{
			profilableEntities->Append( outermostProfile );
		}
	}

	return status;
}

bool
CAutoMod::IsPunchCapable()
{
	int machineType = m_model->Header().getInt( "MachineType", IUNDEFINED );

	bool punchCapable =
			( machineType == PUNCH ||
			  machineType == PUNCH_PLASMA ||
			  machineType == PUNCH_LASER );

	// 2007.05.06 (PE) -- Introduced for Dynatorch to support piercing
	// with torch. Per conversation with Gary, Walt wants to simply
	// blow through the materal wherever a point is defined. We
	// accomplish this by first converting the CAD point to an arc
	// whose diameter exactly matches the kerf of the torch. Upon
	// auto-tooling the model, we associate the torch to these arcs
	// via a hole entity.
	if ( !punchCapable )
	{
		CString machine_name = m_autoModDb->MachineName();
		machine_name.MakeLower();
		punchCapable = (machine_name.Find("dynatorch") >= 0);
	}

	return punchCapable;
}

CReturn
CAutoMod::AutoDrill(
				const CDbEntityList&	candidateTools,
				double					ptol,
				double					mtol,
				CDbEntityList*			holes )
{
	CReturn status;

	// Auto-tool those holes for which there is a matching tool.
	int indx = 0;
	while ( status.IsOk() )
	{
		if (holes->Count() < 1)
			break;

		// This hole is possibly the first in a batch and
		// is the representative for finding a matching tool.
		CDbArc* dbArc = dynamic_cast<CDbArc*>( (*holes)[0] );

		// Find a matching tool.
		CDbTool* dbTool = AutoToolFind( dbArc, ptol, mtol, candidateTools );

		if (dbTool == NULL)
		{
			// Didn't find a matching tool.  Advance to the next hole.
			// TODO: Perhaps we could can gain some effiency by jumping
			// to the start of the next batch of 'like' holes.  Finding
			// a robust scheme for this purpose in non-trivial, however.
			holes->Remove( 0 );
		}
		else
		{
			// We've found a suitable tool.  Find the batch of 'like'
			// holes that are within some delta of the given tool.
			CDbEntityList likeHoles;
			CAutoMod::LikeHolesGet( (*dbTool), holes, &likeHoles );

			status = HoledFeaturesCreate( likeHoles, dbTool );

			likeHoles.BenignFlush();
		}
	}

	return status;
}

// Associate a 'tooled feature' with all database entities
// that are within some tolerance of the given tool.
//
// At initial implementation, HoledFeaturesCreate() simply processed
// circles whose diameter was within tolerance of the diameter of
// the given tool.
//
// With the coming of Version 14, HoledFeaturesCreate() was modified
// in support of tooling-up points imported from a CAD file.  The
// import method converts the CAD point to a non-tooled hole entity
// and tags it with the required NC_Code_Number.  Under these
// circumstances, 'like' holes are then defined as holes being
// tagged with the same NC_Code_Number.
//
CReturn
CAutoMod::HoledFeaturesCreate( CDbTool* dbTool )
{
	CReturn status;

	// Get the CAM Layer associated with the current database record.
	CDbTool* dbLayer = CamLayer();
	if (dbLayer == NULL)
	{
		// The layer wasn't created because there was nothing on it.
		return CReturn( STATUS_OKAY );
	}

	CDbEntityList entities;

	if ( dbTool->IsHoleTool() )
	{
		// Get all of the circles on this layer.  The circles
		// are sorted into batches of 'like' circles.
		PartCirclesGet( dbLayer, &entities );
		if (entities.Count() > 0)
		{
			// Find the batch of 'like' holes that are
			// within some delta of the given tool.
			CDbEntityList likeHoles;
			CAutoMod::LikeHolesGet( (*dbTool), &entities, &likeHoles );

			status = HoledFeaturesCreate( likeHoles, dbTool );

			entities.BenignFlush();
		}
	}

	// Get all holes tagged with NC_Code_Number of the given tool.
	TaggedHolesGet( (*dbTool), &entities );
	if (entities.Count() > 0)
	{
		status = HoledFeaturesCreate( entities, dbTool );

		entities.BenignFlush();
	}

	return status;
}

CReturn
CAutoMod::HoledFeaturesCreate(
					const CDbEntityList& likeHoles,
					CDbTool* dbTool )
{
	int speed;
	double lfeed, pfeed;
	CVarList attribMemo;
	CVarList sfparams;
	CReturn status;

	int count = likeHoles.Count();
	if (count < 1)
		return status;  // Nothing to do.

	CDbWorkplane* dbWork;
	status = WorkplaneFind( dbTool, &dbWork );
	if ( !status.IsOk() )
		return status;

	double holeDiam = dbTool->EffectiveDiameter();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	attribMemo = m_model->Default();
	m_model->pDefault()->Reset();

	// TODO:  Determine which attributes should *really* be set here...
	int toolTypeId = dbTool->IntGet( STR_TYPE_ID, IUNDEFINED );

	m_autoModDb->SpeedFeed( toolTypeId, holeDiam, &sfparams );

	speed = sfparams.getInt( STR_SPEED, 0 );
	lfeed = sfparams.getReal( STR_FEED, 0. );
	pfeed = sfparams.getReal( PLUNGE, 0. );

	if (speed > SMALL)
		m_model->pDefault()->setInt( STR_SPEED, speed );

	if (lfeed > SMALL)
		m_model->pDefault()->setReal( STR_FEED, lfeed );

	if (pfeed > SMALL)
		m_model->pDefault()->setReal( PLUNGE, pfeed );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Create the recipient toolpath feature.
	CDbFeature* dbFeature;
	status = m_model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
	if ( !status.IsOk() )
		return status;

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = likeHoles[indx];

		CDbArc* dbArc = (CDbArc*) dynamic_cast<const CDbArc*>( dbEntity );
		CDbHole* dbHole = (CDbHole*) dynamic_cast<const CDbHole*>( dbEntity );

		if (dbArc != NULL)
		{
			C3dCoord pc = dbArc->CenterPt();
			C3dCoord holeTop( pc.X(), pc.Y(), 0.0 );

			status = m_model->EntityCreate( DBHOLE, (CDbEntity**) &dbHole );
			dbHole->Init( dbTool, dbWork, holeTop, holeDiam, fabs(pc.Z()) );

			dbArc->IntSet( "Used", 1 );
		}
		else
		{
			// This hole created much earlier (perhaps DXF import), but
			// still needs to inherit the models' default attributes.
			(* (dbHole->pAttrib()) ) += ( *(m_model->pDefault()) );

			// Otherwise, solid view won't work.
			dbHole->Depth( m_autoModDb->MaterialThick() );
		}

		dbHole->Tool( dbTool );
		dbFeature->Append( dbHole );
	}

	(*(m_model->pDefault())) = attribMemo;

	return status;
}

CReturn
CAutoMod::ProfiledFeaturesCreate( const CDbEntityList& entities, CDbTool* dbTool )
{
	static const int OFFSET_LEFT = 1;

	CReturn status;
	
	int count = entities.Count();
	if (count < 1)
		return status;


	CVarList attribMemo;
	CVarList sfparams;
	double delta, sharp;
	int cutSide;
	int codePartProf;
	bool offset_open_profs;

#if REQUIRED
	double lfeed, pfeed;
	double dia = 0.0;
	int speed;
#endif

	CDbCurve* dbCurve = NULL;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CDbWorkplane* dbWork;
	status = CAutoMod::WorkplaneFind( dbTool, &dbWork );
	if ( !status.IsOk() )
		return status;

	m_model->ActiveTool( dbTool );
	m_model->ActiveWorkplane( dbWork );

	codePartProf = m_model->Header().getInt( STR_PARTPROF, 0 );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	attribMemo = m_model->Default();
	m_model->pDefault()->Reset();

	cutSide = m_autoModDb->IntGet( "Cut_Side" );
	m_model->pDefault()->setInt( STR_CUTSIDE, cutSide );

	if ( codePartProf )
		m_model->pDefault()->setInt( STR_PARTPROF, 1 );

	delta = m_autoModDb->DoubleGet( DISTANCE );
	sharp = m_autoModDb->SharpAngle();
	offset_open_profs = !m_autoModDb->RestrictOffset();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = entities[indx];

		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

		CDbCurve* dbCurve = ((dbProfile == NULL)
							 ? dynamic_cast<CDbCurve*>( dbEntity )
							 : dynamic_cast<CDbCurve*>( (*dbProfile)[0] ) );

		if ( IsOkayToProfile( dbEntity ) )
		{
			if ( dbTool->IsPunchTool() && !dbTool->IsRoundTool() )
			{
				ID featid = 0;

				if ( dbTool->IsIndexable() )
				{
					COffsetter::AutoIndexOffset(
							m_model, &featid, dbEntity->Id(), dbTool->Id(), cutSide, FALSE );
				}
				else
				{
					COffsetter::ConvexHullOffset(
							m_model, &featid, dbEntity->Id(), dbTool->Id(), cutSide );
				}
			}
			else if (!offset_open_profs && IsOpenProfile( dbEntity ))
			{
				status = UniformOffset( dbWork, dbTool, dbEntity, 0, 0, sharp );
			}
			else
			{
				status = UniformOffset( dbWork, dbTool, dbEntity, cutSide, delta, sharp );
			}
		}
	}

	(*(m_model->pDefault())) = attribMemo;

	return status;
}

CReturn
CAutoMod::UniformOffset(
	CDbWorkplane*	dbWork,
	CDbTool*		dbTool,
	CDbEntity*		dbEntity,
	int				cutSide,
	double			offsetAmount,
	double			sharpAngle )
{
	CReturn status;

	CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

	CDbCurve* dbCurve = ((dbProfile == NULL)
						 ? dynamic_cast<CDbCurve*>( dbEntity )
						 : dynamic_cast<CDbCurve*>( (*dbProfile)[0] ) );

	if (dbCurve != NULL)
	{
		CDbFeature* dbFeature;
		status = m_model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );

		if (dbFeature != NULL)
		{
			double zlevel = 0.;
			if ( IsPhenolicProject2() )
			{
				// Is this behavior a left-over from Cam-squared?
				//   double zlevel = dbCurve->StartPt().Z();
				zlevel = dbCurve->StartPt().Z();
				CDbTool* dbLayer = dbCurve->Tool();
				if (dbLayer->Name().CompareNoCase( STR_STOCK ) == 0)
					zlevel = m_autoModDb->DoubleGet( "Z_Level" );
			}
			
			status = COffsetter::UniformOffset( m_model, dbWork, dbTool,
				dbEntity, cutSide, offsetAmount, sharpAngle, zlevel, dbFeature );
		}
	}

	return status;
}

CReturn
CAutoMod::CommandsProcess( CDbTool* dbLayer, CDbTool* dbTool )
{
	CReturn		status;
	CDbIterator	iter;
	CDbCommand*	dbCommand;

	int color = dbTool->ColorGet( DCOLOR_BLUE );

	iter.Init( m_model->Db(), DBCOMMAND );
	while (1)
	{
		dbCommand = dynamic_cast<CDbCommand*>( iter() );
		if (dbCommand == NULL)
			break;

		if (dbCommand->Tool() == dbLayer)
		{
			dbCommand->Tool( dbTool );
			// dbCommand->ColorSet( color ); 
		}

		iter.Next();
	}

	return status;
}

// At first implementation, finds only circles as would be drilled.
void
CAutoMod::PartCirclesGet( const CDbTool* dbLayer, CDbEntityList* entities )
{
	CReturn status;

	CDbEntityList tempList;

	dbLayer->RefdBy( &tempList );

	int count = tempList.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbArc* dbArc = dynamic_cast<CDbArc*>( tempList[indx] );
		if (dbArc != NULL)
			PartHoleAdd( dbArc, entities );
	}
}

// Gets a list of curves/profiles that reference the given layer.
void
CAutoMod::PartProfilesGet( const CDbTool* dbLayer, CDbEntityList* entities )
{
	CDbIterator iter;
	CDbTool*	dbTool;

	iter.Init( m_model->Db(), DBLINE );

	while (1)
	{
		CDbEntity* dbEntity = iter();
		if (dbEntity == NULL || dbEntity->Type() > DBPROFILE)
			break;  // We're finished.

		dbTool = dbEntity->Tool();

		if (dbTool == dbLayer)
		{
			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

			CDbEntity* dbOwner = dbEntity->Owner();

			bool okayToProcess = FALSE;
			if (dbCurve != NULL || dbProfile != NULL)
			{
				// Avoid processing this curve/profile because it is owned
				// by another entity.  If this entity is owned by a profile,
				// the curve will be processed with the profile.  If this
				// entity is a profile and it is owned, then it is assumed
				// to be toolpath.

				okayToProcess = (dbOwner == NULL);
			}

			if ( okayToProcess )
			{
				entities->Append( dbEntity );
			}
		}

		iter.Next();
	}
}

bool
CAutoMod::IsOkayToProfile( const CDbEntity* dbEntity )
{
	CDbEntity* owner = dbEntity->Owner();

	if (owner == NULL)
		return TRUE;

	if (owner->Type() == DBPROFILE)
		return TRUE;

	CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( owner );

	if (dbFeature == NULL)
		return TRUE;  // ????

	return (dbFeature->ToolId() == 0);
}

CDbTool*
CAutoMod::CamLayer()
{
	CString camLayerName = m_autoModDb->StringGet( CAM_LAYER );
	camLayerName = CDbEntity::NameConvert( camLayerName );

	CDbTool* dbTool;
	m_model->EntityFind( camLayerName, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	return dbTool;
}

CReturn
CAutoMod::WorkplaneFind( const CDbTool* dbTool, CDbWorkplane** dbWork )
{
	CString name = dbTool->StringGet( STR_WORKPLANE, "" );

	CReturn status = m_model->EntityFind( name, (CDbEntity**) dbWork, DBWORKPLANE, DBWORKPLANE );

	return status;
}

void
CAutoMod::PartHoleAdd( CDbArc* dbArc, CDbEntityList* entities )
{
	CVar* var = dbArc->Attrib().getVar( "Used" );
	if (var != NULL)
		return;  // Disregard.  Already been used.

	C3dCoord ps = dbArc->StartPt();
	C3dCoord pe = dbArc->EndPt();

	if ( !ps.WithinTol( pe, SMALL ) )
		return;  // We have an arc, not a circle.

	double radius = dbArc->Radius();

	int count = entities->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbArc* tmp = dynamic_cast<CDbArc*>( (*entities)[indx] );
		if (radius < tmp->Radius())
		{
			entities->InsertBefore( indx, dbArc );
			return;
		}
	}

	entities->Append( dbArc );
	return;
}

CDbTool*
CAutoMod::AutoToolFind(
				const CDbArc*			dbArc,
				double					ptol,
				double					mtol,
				const CDbEntityList&	candidateTools )
{
	double diam = dbArc->Radius() * 2;
	double depth = dbArc->CenterPt().Z();

	CDbWorkplane* dbWork = dbArc->Workplane();

	CDbTool* dbTool = AutoToolFind( candidateTools, ptol, mtol, dbWork, diam, depth );

	return dbTool;
}

//
// TODO:  Merge with "FindTool()" in EntityDb??
//
CDbTool*
CAutoMod::AutoToolFind(
				const CDbEntityList&	candidateTools,
				double					ptol,
				double					mtol,
				const CDbWorkplane*		dbWork,
				double					diam,
				double					depth )
{
	double minDiam = diam - mtol;
	double maxDiam = diam + ptol;

	CDbTool* dbTool = NULL;

	double best_score = DBL_MAX;
	
	int count = candidateTools.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbTool* candTool = dynamic_cast<CDbTool*>( candidateTools[indx] );
		if (candTool == NULL)
			break;

		CString candWork  = candTool->StringGet( STR_WORKPLANE, "" );
		double candDiam   = candTool->EffectiveDiameter();
		double candLength = candTool->DoubleGet( STR_LENGTH, UNDEFINED );

		if ( candWork.CompareNoCase( dbWork->Name() ) == 0 &&
			 candDiam >= minDiam &&
			 candDiam <= maxDiam &&
			 (depth - candLength) < 0 )
		{
			double delta = (candDiam - diam);
			double score = delta*delta;
			if (score < best_score)
			{
				// Show a preference for tools closer to the target diameter,
				// when possible.
				best_score = score;
				dbTool = candTool;
			}
//			break;
		}
	}

	return dbTool;
}

void
CAutoMod::SimilarToolsFind( int type_id, CDbEntityList* tools )
{
	CReturn		status;
	CDbIterator	iter;

	iter.Init( m_model->Db(), DBTOOL );

	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		int toolTypeId = dbTool->IntGet( STR_TYPE_ID, IUNDEFINED );

		if (toolTypeId == type_id)
			tools->Append( dbTool );

		iter.Next();
	}
}

// Find all holes whose dimensions are within
// some delta of the tool dimensions.
void
CAutoMod::LikeHolesGet(
				const CDbTool& dbTool,
				CDbEntityList* entities,
				CDbEntityList* likeHoles )
{
	if (entities->Count() < 1)
		return;


	int indx = 0;
	CDbWorkplane* dbWork = (*entities)[indx]->Workplane();
	
	double toolDiam = dbTool.EffectiveDiameter();
	double minDiam = toolDiam - HoleMinusTol();
	double maxDiam = toolDiam + HolePlusTol();

	while (1)
	{
		if (indx >= entities->Count())
			break;

		CDbArc* dbArc = (CDbArc*) dynamic_cast<const CDbArc*>( (*entities)[indx] );

		double holeDiam = dbArc->Radius() * 2;

		// TODO: This may be problematic because, at this point,
		// the tolerances are with respect to the tool, but the
		// tool was selected based on a 'representative' hole.
		bool diamMatch = (holeDiam >= minDiam && holeDiam <= maxDiam);
		if ( !diamMatch )
			break;

		if (diamMatch && dbWork == dbArc->Workplane())
		{
			likeHoles->Append( dbArc );
			entities->Remove( indx );
		}
		else
		{
			++ indx;
		}
	}
}

void
CAutoMod::TaggedHolesGet(
				const CDbTool& dbTool,
				CDbEntityList* likeHoles )
{
	CDbIterator	iter;
	int ncCodeNum;
	
	ncCodeNum = dbTool.IntGet( STR_NC_CODE_NUMBER, IUNDEFINED );

	iter.Init( m_model->Db(), DBHOLE );
	while (1)
	{
		CDbHole* dbHole = dynamic_cast<CDbHole*>( iter() );
		if (dbHole == NULL)
			break;

		int tmp = dbHole->IntGet( STR_NC_CODE_NUMBER, IUNDEFINED );
		if (tmp == ncCodeNum)
		{
			likeHoles->Append( dbHole );
			dbHole->AttribDelete( STR_NC_CODE_NUMBER );  // remove the lint
		}

		iter.Next();
	}
}

CReturn
CAutoMod::ProfilesCreate()
{
	static const bool ASSOCIATE = TRUE;

	int	dir;
	CReturn status;

	if ( m_autoModDb->IsPreview() )
		return status;  // Bypass everything because we're previewing the file.

	double gapTol   = m_autoModDb->GapTol();
	double cleanTol = m_autoModDb->CleanTol();

	int count = m_autoModDb->Count();

	// For each layer in the database, perform the specified actions.
	for (int indx = 0; indx < count; ++indx)
	{
		m_autoModDb->CurrentRecordSet( indx );

		// Get the CAM Layer associated with the current database record.
		CDbTool* camLayer = CamLayer();
		if (camLayer == NULL)
		{
			// The layer wasn't created because there was nothing on it.
			continue;
		}

		if (camLayer->Name().CompareNoCase( STR_STOCK ) == 0)
		{
			// Avoid creating profiles on the stock layer as
			// that should have already been done by either
			// StockCreate() or StockLayerProcess().

			continue;
		}

		m_model->ActiveTool( camLayer );

		switch (m_autoModDb->IntGet( "Cut_Direction" ))
		{
		case 0:	 dir = CW;   break;
		case 1:  dir = CCW;  break;
		default: dir = NONE; break;
		}

		status = CProfileBuilder::ProfileLayer(
			m_model, camLayer, gapTol, cleanTol, ASSOCIATE, dir, FALSE );

		if ( !status.IsOk() )
			break;
	}

	if ( status.IsOk() )
		status = ProfilesAdjust();

	return status;
}

// Created for Auto-Tooling with punches, converts punching tools
// to both geometric and attribute form so shape comparisons can
// be made with part geometry during the Auto-Tooling process.
CReturn
CAutoMod::PunchToolsParameterize()
{
	CReturn		status;
	CDbIterator	iter;

	m_parameterizedTools.DestructiveFlush();

	iter.Init( m_model->Db(), DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if ( dbTool->IsHoleTool() )
		{
			CGeoCurveArray toolGeo;
			dbTool->Convert( &toolGeo );

			if (toolGeo.Count() > 0)
			{
				CShape* shape = new CShape();

				shape->Init( toolGeo, FALSE );

				shape->IntSet( STR_STATION_ID, dbTool->IntGet( STR_STATION_ID, 0 ) );

				shape->IntSet( STR_AUTO_INDEX, (dbTool->IsIndexable() ? 1 : 0) );

				m_parameterizedTools.Append( shape );
			}

			toolGeo.DestructiveFlush();
		}

		iter.Next();
	}

	return status;
}

bool
CAutoMod::IsProfilingTool( const CDbTool& dbTool )
{
	eToolType type = (eToolType) dbTool.IntGet( STR_TYPE_ID, TTYPE_NONE );

#if BEFORE_AUTOPUNCH
	switch (type )
	{
	case TTYPE_ROUTER_BIT:
	case TTYPE_DISC_SAW:
	case TTYPE_BURNER:
	case TTYPE_SCRIBE:
	case TTYPE_POWDER_MARK:
	case TTYPE_LASER:
	case TTYPE_WATERJET:
	case TTYPE_ROUND:
	case TTYPE_SQUARE:
	case TTYPE_RECTANGLE:
	case TTYPE_OBROUND:
	case TTYPE_DIAMOND:
	case TTYPE_CORNER_RADIUS:
	case TTYPE_SINGLE_D:
	case TTYPE_DOUBLE_D:
	case TTYPE_TRAPEZOID:
	case TTYPE_MARKING:
	case TTYPE_HEXAGON:
	case TTYPE_END_MILL:
		return TRUE;

	default:
		return FALSE;
	}
#else
	switch (type )
	{
	case TTYPE_ROUTER_BIT:
	case TTYPE_DISC_SAW:
	case TTYPE_BURNER:
	case TTYPE_SCRIBE:
	case TTYPE_POWDER_MARK:
	case TTYPE_LASER:
	case TTYPE_WATERJET:
	case TTYPE_ROUND:
	case TTYPE_MARKING:
	case TTYPE_END_MILL:
		return TRUE;

	default:
		return FALSE;
	}
#endif
}

// Adjust the directions of the profiles on selected layers based
// upon containment depth and offset direction.  Convention assumes
// climb cutting.
CReturn
CAutoMod::ProfilesAdjust()
{
	CReturn status;
	CDbEntityList profs;
	CDbTool* dbLayer;
	CString name;
	int cutDir, cutSide;
	int count, indx;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Determine the containment level of all profiles in the model.
	// This information is used to correct profile directions as
	// necessary, and to help remove the outermost profile from
	// consideration by AutoPunch() when importing a file.
	//
	// BugID: 406 -- AutoPunch is wrongly applied to outside profile
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	ProfilesGet( &profs );

	if (profs.Count() > 0)
	{
		status = CModelUtil::MarkIntExt( &profs, FALSE, m_model );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Correct the direction of any profiles as required.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = m_autoModDb->Count();
	for (indx = 0; indx < count; ++indx)
	{
		m_autoModDb->CurrentRecordSet( indx );

		cutDir = m_autoModDb->IntGet( "Cut_Direction" );
		if (cutDir == 2)
		{
			// The cut direction of profiles on this layer are determined
			// by their containment level and the 'cut side' setting.

			cutSide = m_autoModDb->IntGet( "Cut_Side" );
			if (cutSide != 0)
			{
				name = m_autoModDb->StringGet( "CAM_Layer" );

				m_model->EntityFind( name, (CDbEntity**) &dbLayer, DBTOOL, DBTOOL );

				if (dbLayer == NULL)
				{
					// Do nothing.  It is possible that the layer setup
					// identifies this layer, but the layer may not have
					// been created because it doesn't contain any geometry.
				}
				else
				{
					status = ProfilesAdjust( profs, dbLayer, cutSide );
					if ( !status.IsOk() )
						break;
				}
			}
		}
	}

	return status;
}

CReturn
CAutoMod::ProfilesAdjust( const CDbEntityList& profs, const CDbTool* dbLayer, int cutSide )
{
	CReturn		status;
	CDbTool*	dbTool;

	// Adjust the cut direction of each profile.
	int count = profs.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = profs[indx];

		dbTool = dbEntity->Tool();

		if (dbTool == dbLayer)
		{
			int profDir = ProfileDirection( dbEntity );

			int depth = dbEntity->IntGet( STR_PROFILE_DEPTH, 0 );

			// Convention assumes climb cutting.
			bool reverse = ((depth & 0x0001) ? (cutSide != profDir) : (cutSide == profDir));
			if ( reverse )
				ProfileReverse( dbEntity );
		}
	}

	return status;
}

void
CAutoMod::ProfilesGet( 
	CDbEntityList* profs,
	bool toolpath_only )
{
	CReturn		status;
	CDbIterator	iter;
	CDbEntity*	dbEntity;
	CDbTool*	dbTool;
	EDbEntityType type;

	iter.Init( m_model->Db(), DBARC );

	// Collect all of the profiles and circles in the model.
	while (1)
	{
		dbEntity = iter();
		if (dbEntity == NULL)
			break;
		iter.Next();

		if ( toolpath_only
			&& !dbEntity->IsToolpath())
			continue;

		type = dbEntity->Type();

		dbTool = dbEntity->Tool();

		if (type == DBARC)
		{
			CDbArc* dbArc = dynamic_cast<CDbArc*>( dbEntity );
			C3dCoord ps = dbArc->StartPt();
			C3dCoord pe = dbArc->EndPt();
			if (ps.WithinTol( pe, SMALL ) && dbTool->Name().CompareNoCase( STR_STOCK ) != 0)
			{
				// Assume a full circle.
				// .. but only add if it's not owned by a profile (otherwise, gets in twice)
				if (!dynamic_cast<CDbProfile*>(dbEntity->Owner()))
					profs->Append( dbEntity );
			}
		}
		else if (type == DBPROFILE)
		{
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

			if ( (dbTool->Name().CompareNoCase( STR_STOCK ) != 0) &&
				  dbProfile->IsClosed( CLOSED_ENOUGH ) )
			{
				profs->Append( dbEntity );
			}
		}
		else if (type > DBPROFILE)
		{
			break;
		}
	}
}

void
CAutoMod::ProfileReverse( CDbEntity* dbEntity )
{
	CDbArc* dbArc = dynamic_cast<CDbArc*>( dbEntity );
	CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

	if (dbArc != NULL)
	{
		dbArc->Reverse();
	}
	else if (dbProfile != NULL)
	{
		dbProfile->Reverse();
	}
}

int
CAutoMod::ProfileDirection( const CDbEntity* dbEntity )
{
	CDbWorkplane* dbWork = dbEntity->Workplane();
	int dir = 0;

	const CDbArc* dbArc = dynamic_cast<const CDbArc*>( dbEntity );
	const CDbProfile* dbProfile = dynamic_cast<const CDbProfile*>( dbEntity );

	if (dbArc != NULL)
	{
		dir = dbArc->Dir() * dbWork->ToolUp();
	}
	else if (dbProfile != NULL)
	{
		CProfile prof;
		if ( CConversion::Convert( dbWork, dbProfile, &prof ).IsOk() )
			dir = SGN(prof.Area()) * dbWork->ToolUp();
	}

	return dir;
}

double
CAutoMod::HolePlusTol()
{
	return ((m_autoModDb == NULL) ? SMALL : m_autoModDb->HolePlusTol());
}

double
CAutoMod::HoleMinusTol()
{
	return ((m_autoModDb == NULL) ? SMALL : m_autoModDb->HoleMinusTol());
}

void
CAutoMod::DuplicatesFilter( CModel* model, double gap_tol )
{
	CReturn status;

	CDbIterator		iter;
	CDbCurveList	model_curves;
	CDbCurve*		dbCurve;
	CDbTool*		stock;

	// See also CProfileBuilder::ProfileLayer().
	bool filter_duplicates = CRegister::BoolGetV( "CadToCode", "dups", true );

	if (filter_duplicates)
	{
		model->EntityFind( "Stock", (CDbEntity**) &stock, DBTOOL, DBTOOL );

		iter.Init( model->Db(), DBLINE );
		while (1)
		{
			dbCurve = dynamic_cast<CDbCurve*>( iter() );
			if (dbCurve == NULL)
				break;

			if (dbCurve->Tool() != stock)
				model_curves.Append( dbCurve );

			iter.Next();
		}

		CProfileBuilder::DuplicatesFilter( model, gap_tol, &model_curves );
	}
}

// 2008.10.04 (PE) -- Introduced in support of Dynatorch customers,
// who often often cut open internal profiles representing detail
// features in 'yard art'.
bool
CAutoMod::IsOpenProfile( const CDbEntity* dbEntity )
{
	double OPEN_TOL = 1.e-4;
	bool is_open = false;

	const CDbProfile* dbProfile = dynamic_cast<const CDbProfile*>( dbEntity );
	if (dbProfile == NULL)
	{
		const CDbCurve* dbCurve = dynamic_cast<const CDbCurve*>( dbEntity );
		if (dbCurve != NULL)
		{
			C3dCoord ps = dbCurve->StartPt();
			C3dCoord pe = dbCurve->EndPt();
			is_open = !ps.WithinTolXY( pe, OPEN_TOL );
		}
	}
	else
	{
		is_open = !dbProfile->IsClosed( OPEN_TOL );
	}

	return is_open;
}
