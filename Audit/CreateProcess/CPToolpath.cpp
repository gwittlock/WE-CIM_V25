
#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"
#include "CommonFlags.h"
#include "cmn_resource.h"

#include "Register.h"  // TODO: Remove this

#include "3x4Matrix.h"
#include "3dBox.h"
#include "WmChainIterator.h"

#include "DbAllEntities.h"
#include "DbIterator.h"
#include "EntityCopier.h"
#include "Selector.h"
#include "Model.h"
#include "ModelUtil.h"

#include "ViewMgr.h"

#include "Profile.h"
#include "Worm.h"
#include "Offsetter.h"
#include "Conversion.h"

#include "AutoModDb.h"
#include "AutoMod.h"
#include "AutoPuncher.h"
#include "Nibbler.h"

#include "Raster.h"

#include "CreateProcess.h"

#ifdef NURB
#include "nurb.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Command format
//
//    Toolpath: Spiral: [id=%d,] toolid = %d, profid = %d,
//			  stepover = %f, wallallow = %f, cw = %d, insideout = %d,
//			  sharp = %f, passdepth = %f, floorallow = %f
//
//    id -- (0) create a new feature / (?) regenerate the existing feature.
//
// Output parameters
//
//    "id"   int     id of spiral toolpath
//
CReturn
CCreateProcessApp::ToolpathSpiral( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	CReturn	status;

	CProfile srcProf;
	CProfile dstProf;
	CString command;

	ID id = 0;
	ID toolId = 0;
	ID profId = 0;

	int cw, insideOut;  //, defColor, toolColor;
	double toolDiam, stepOver, wallAllow, passDepth, floorAllow, sharpAngle;

	CDbEntity* dbEntity = NULL;
	CDbTool* dbTool = NULL;
	CDbWorkplane* dbWork = NULL;
	CDbFeature* dbFeature = NULL;

	CModel& model = io_cmd->getModel();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );

	status += io_cmd->getInt( "toolid", (int*)&toolId );
	status += io_cmd->getInt( "profid", (int*)&profId );
	status += io_cmd->getReal( "stepover", &stepOver );
	status += io_cmd->getReal( "wallallow", &wallAllow );
	status += io_cmd->getInt( "cw", &cw );
	status += io_cmd->getInt( "insideout", &insideOut );
	status += io_cmd->getReal( "sharp", &sharpAngle );
	status += io_cmd->getReal( "passdepth", &passDepth );
	status += io_cmd->getReal( "floorallow", &floorAllow );

#if (!_CI)
	// Must have some kind of passDepth for fab
	// even though it not used ....
	passDepth = 1.;
#endif

	if ( !status.IsOk() ||
		 toolId < 1 ||
		 profId < 1 ||
		 fabs(passDepth) < SMALL || stepOver < SMALL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ToolpathSpiral()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the profile and tool
	status = model.EntityFind( profId, &dbEntity, DBLINE, DBPROFILE );
	if ( !status.IsOk() )
		return status;

	status = model.EntityFind( toolId, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	if ( !status.IsOk() )
		return status;

	model.pDefault()->setInt( STR_CUTSIDE, 0 );

	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	dbWork = dbTool->Workplane();
	status = CConversion::Convert( dbWork, dbEntity, &srcProf );

	if ( status.IsOk() )
		status = model.EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );

	// Generate the raw cut data.
	if ( status.IsOk() )
	{
		if (model.ActivePattern() != NULL)
			model.ActivePattern()->Append( dbFeature );

		dbFeature->DestructiveFlush();

		toolDiam = dbTool->EffectiveDiameter();

		status = CWorm::SpiralPocket( srcProf, toolDiam, dbWork->ToolUp(),
							stepOver, wallAllow, cw, insideOut, sharpAngle,
							passDepth, floorAllow, &dstProf );
	}

	// Stuff the results into the toolpath's feature.
	if ( status.IsOk() )
		status = CConversion::Convert( dstProf, UNDEFINED, dbTool, dbWork, dbFeature );

#if (!_CI)
	passDepth = 0.;
#endif

	// Name the toolpath and return its id.
	if ( status.IsOk() )
	{
		dbFeature->RefsFlush();
		dbFeature->AddRef( dbEntity );

		command.Format( "function = \"Toolpath: Spiral:\", " \
						"id = %d, toolid = %d, profid = %d, " \
						"stepover = %f, wallallow = %f, " \
						"cw = %d, insideout = %d, sharp = %f, " \
						"passdepth = %f, floorallow = %f",

						dbFeature->Id(), toolId, profId,
						stepOver, wallAllow,
						cw, insideOut, sharpAngle,
						passDepth, floorAllow );

		dbFeature->setMulti( command );

		io_cmd->setInt( "id", dbFeature->Id() );
	}

	return status;
}

// ==================================================================
// AutoTool: db=%s, id=%d, build=%d
//
//		db  = fully qualified file path to machine database file
//		id  = layer setup id in the machine database
//		
CReturn 
CCreateProcessApp::AutoTool( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CString	databaseName;
	int		layerSetupID;
	int		toolSetupID;
	bool	buildToolSetup;

	const CVarList& params = io_cmd->VarList();

	databaseName = params.getString( "db", "" );
	layerSetupID = params.getInt( "id", 0 );
	toolSetupID  = params.getInt( "toolsetupid", 0 );
	buildToolSetup = params.getInt( "build", FALSE );

	if (databaseName.IsEmpty() || layerSetupID < 1 || toolSetupID < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::AutoTool()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CAutoModDb autoModDb;
	CAutoMod autoMod;
	CModel&	model = io_cmd->getModel();

	status = autoModDb.Init( databaseName, toolSetupID, layerSetupID, -1, buildToolSetup );

	if ( status.IsOk() )
	{
		autoMod.Init( &autoModDb, &model );
		status += autoMod.ModelPostProcess( buildToolSetup, FALSE );
	}

	return status;
}

// ==================================================================
// Toolpath: Disassociate: id=%d
//
//		id = id of toolpath to be diassociated from its reference geometry.
//		
CReturn 
CCreateProcessApp::Disassociate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	ID id;

	status = io_cmd->getInt( "id", (int*) &id );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Disassociate()" );
		return status;
	}

	CModel& model = io_cmd->getModel();
	CDbFeature* dbFeature;
	status = model.EntityFind( id, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Disassociate()" );
		return status;
	}

	dbFeature->RefsFlush();
	dbFeature->AttribDelete( "function" );

	return status;
}

// Toolpath:ConvexHullOffset: profid=%d, toolid=%d, dir=%d
CReturn 
CCreateProcessApp::ConvexHullOffset( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CModel& model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	ID profid = io_cmd->VarList().getInt( "profid", 0 );
	ID toolid = io_cmd->VarList().getInt( "toolid", 0 );
	int dir = io_cmd->VarList().getInt( "dir", 0 );

	status = COffsetter::ConvexHullOffset( &model, &id, profid, toolid, dir );
	
	io_cmd->setInt( "id", id );

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ConvexHullOffset()" );

	return status;
}

// Standard usage.
//     Toolpath:AutoIndexOffset: profid=%d, toolid=%d, dir=%d, longside=%d
// Regen usage:
//     Toolpath:AutoIndexOffset: id=%d, profid=%d, toolid=%d, dir=%d, longside=%d
CReturn 
CCreateProcessApp::AutoIndexOffset( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CModel& model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt( "id", 0 );
	ID profid = io_cmd->VarList().getInt( "profid", 0 );
	ID toolid = io_cmd->VarList().getInt( "toolid", 0 );
	int dir = io_cmd->VarList().getInt( "dir", 0 );
	int longside = io_cmd->VarList().getInt( "longside", 0 );

	status = COffsetter::AutoIndexOffset(
		&model, &id, profid, toolid, dir, (longside != 0) );

	io_cmd->setInt( "id", id );

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::AutoIndexOffset()" );

	return status;
}

// Interactive usage (app loops over selection set):
//     Toolpath:AutoPunch: profid=%d [, ptol=%f, mtol=%f]
//
// File conversion usage:
//     Toolpath:AutoPunch: db=%s, toolsetupid=%d, buildsetup=%d, ptol=%f, mtol=%f
//
// Regen usage
//     Toolpath:AutoPunch: id=%d, profid=%d, ptol=%f, mtol=%f
//
CReturn 
CCreateProcessApp::AutoPunch( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CAutoPuncher	puncher;
	CToolSetup		toolSetup;
	CDbEntityList	dbEntities;
	CDbFeature*		dbFeature;
	CDbEntity*		dbEntity;
	CString			dbPath;
	ID				profid;
	ID				featid;
	int				toolSetupID;
	bool			buildSetup;
	double			ptol, mtol;

	CModel& model = io_cmd->getModel();
	const CVarList& params = io_cmd->VarList();

	ptol = model.Header().getReal( "HolePlusTol", 0.001 );
	mtol = model.Header().getReal( "HoleMinusTol", 0.001 );

	profid = params.getInt( "profid", 0 );
	featid = params.getInt( "id", 0 );

	toolSetupID = params.getInt( "toolsetupid", 0 );
	buildSetup = params.getInt( "buildSetup", FALSE );

	ptol = params.getReal( "ptol", ptol );
	mtol = params.getReal( "mtol", mtol );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	AutoPunchEntitiesGet( model, profid, &dbEntities );

	int count = dbEntities.Count();
	if (count > 0)
	{
		if (toolSetupID > 0)
		{
			dbPath = params.getString( "db", "" );
			if ( dbPath.IsEmpty() )
			{
				dbPath = CRegister::StringGetV( "ConfigurationManager", "Database", "<error>" );
			}

			status = toolSetup.Init( dbPath, toolSetupID, TS_ALL_FLAGS );
		}
		else
		{
			status = toolSetup.InitShapesFromDb( model.Db() );
		}

		if ( status.IsOk() )
		{
			toolSetup.ToolReferencesUpdate( model.Db() );

#if BEFORE_2005_04_14
			// BugID: 710 -- Any logic seen here regarding the
			// contents of dbFeature is done to	prevent the
			// creation of empty features (which causes grief
			// for nesting).
			dbFeature = NULL;
#else
			// Robin @ D&D reported holes on top of holes after transform/move.
			dbFeature = NULL;
			if (featid > 0)
			{
				// We must be regenerating an existing feature(?)
				model.EntityFind( featid, (CDbEntity**) &dbFeature, DBFEATURE, DBFEATURE );
				if (dbFeature != NULL)
					dbFeature->DestructiveFlush();
			}
#endif

			for (int indx = 0; indx < count; ++indx)
			{
				if (dbFeature == NULL)
				{
					model.EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
				}

				if (dbFeature == NULL)
				{
					status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::AutoPunch()" );
					break;
				}

				if (model.ActivePattern() != NULL)
					model.ActivePattern()->Append( dbFeature );

				dbEntity = dbEntities[indx];

				status = puncher.AutoPunch(
									dbEntity, &model, &toolSetup, buildSetup,
									ptol, mtol, RIGOUR_DECOMPOSE, dbFeature );

				if (dbFeature->Count() > 0)
				{
					// Allow creation of another feature.
					dbFeature = NULL;
				}
			}

			if (dbFeature != NULL && dbFeature->Count() < 1)
			{
				// Delete any final empty feature.
				model.EntityDelete( dbFeature->Id() );
			}
		}
	}
	return status;
}

void
CCreateProcessApp::AutoPunchEntitiesGet(
									CModel&			model,
									int				profid,
									CDbEntityList*	dbEntities )
{
	CDbEntity* dbEntity;

	if (profid > 0)
	{
		model.EntityFind( profid, &dbEntity, DBARC, DBPROFILE );
		if (dbEntity != NULL)
			dbEntities->Append( dbEntity );
	}
	else
	{
		CDbIterator		iter;
		EDbEntityType	type;

		iter.Init( model.Db(), DBARC );
		while (1)
		{
			dbEntity = iter();
			if (dbEntity == NULL)
				break;

			type = dbEntity->Type();
			if (type > DBPROFILE)
				break;

			if (type == DBARC)
			{
				if (dbEntity->Owner() == NULL)
				{
					dbEntities->Append( dbEntity );
				}
			}
			else if (type == DBPROFILE)
			{
				if (dbEntity->Tool()->Name().CompareNoCase(STR_STOCK) != 0)
				{
					if (dbEntity->Owner() == NULL)
					{
						dbEntities->Append( dbEntity );
					}
					else if (dbEntity->Tool() == NULL)
					{
						dbEntities->Append( dbEntity );
					}
				}
			}

			iter.Next();
		}
	}
}

// For generating test files only! (temporary?)
// Create:PunchShapesCreate:
CReturn 
CCreateProcessApp::PunchShapesCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	CDbProfile*		dbProfile;
	CDbTool*		dbTool;
	CDbWorkplane*	dbWork;
	CDbEntity*		dbEntity;

	CModel& model = io_cmd->getModel();
	CEntityDb& db = model.Db();

	dbTool = model.ActiveTool();
	dbWork = model.ActiveWorkplane();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#if 1
	int count = db.Count( DBTOOL );
	for (int indx = 0; indx < count; ++indx)
	{
		dbTool = dynamic_cast<CDbTool*>( db.Get( DBTOOL, indx ) );
		if (dbTool != NULL && dbTool->IsHoleTool())
		{
			CGeoCurveArray toolGeo;
			dbTool->Convert( &toolGeo );

			if (toolGeo.Count() > 0)
			{
				model.EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );

				for (int jndx = 0; jndx < toolGeo.Count(); ++jndx)
				{
					dbEntity = dbProfile->Db()->GeoConvert( (*(toolGeo[jndx])), dbTool, dbWork );
					dbProfile->Append( dbEntity );
				}
			}
		}
	}
#endif

#ifdef NURB

if 0 //
#define CASE 1
#if (CASE == 0) ///
	// One arc quadrant
	CNurb nurb( 2, 3 );

	nurb.setPoint( 0, C3dCoord(  1., 0., 0. ) );
	nurb.setPoint( 1, C3dCoord(  1., 1., 0. ) );
	nurb.setPoint( 2, C3dCoord(  0., 1., 0. ) );

	nurb.setKnot( 0, 0.0 );
	nurb.setKnot( 1, 0.0 );
	nurb.setKnot( 2, 0.0 );
	nurb.setKnot( 3, 1.0 );
	nurb.setKnot( 4, 1.0 );
	nurb.setKnot( 5, 1.0 );

	nurb.setWeight( 1, 1 / sqrt( 2.0 ) );
#else  ///
	#if (CASE == 1)
		// Full circle
		CNurb nurb( 2, 9 );

		nurb.setPoint( 0, C3dCoord(  1.,  0., 0. ) );
		nurb.setPoint( 1, C3dCoord(  1.,  1., 0. ) );
		nurb.setPoint( 2, C3dCoord(  0.,  1., 0. ) );
		nurb.setPoint( 3, C3dCoord( -1.,  1., 0. ) );
		nurb.setPoint( 4, C3dCoord( -1.,  0., 0. ) );
		nurb.setPoint( 5, C3dCoord( -1., -1., 0. ) );
		nurb.setPoint( 6, C3dCoord(  0., -1., 0. ) );
		nurb.setPoint( 7, C3dCoord(  1., -1., 0. ) );
		nurb.setPoint( 8, C3dCoord(  1.,  0., 0. ) );

		int numKnots = nurb.numKnots();
		double delta = 1.0 / (numKnots - 1);
		for (int indx = 0; indx < numKnots; ++indx)
		{
			nurb.setKnot( indx, (indx * delta) );
		}

		double weight = 1 / (4 * sqrt( 2.0 ) );
		nurb.setWeight( 1, weight );
		nurb.setWeight( 3, weight );
		nurb.setWeight( 5, weight );
		nurb.setWeight( 7, weight );
	#else
		#if (CASE == 2)
			CNurb nurb( 2, 4 );

			nurb.setPoint( 0, C3dCoord( -1., -1., 0. ) );
			nurb.setPoint( 1, C3dCoord(  1., -1., 0. ) );
			nurb.setPoint( 2, C3dCoord( -1.,  1., 0. ) );
			nurb.setPoint( 3, C3dCoord(  1.,  1., 0. ) );

			int numKnots = nurb.numKnots();
			double delta = 1.0 / (numKnots - 1);
			for (int indx = 0; indx < numKnots; ++indx)
			{
				nurb.setKnot( indx, (indx * delta) );
			}
		#else
			return status;
		#endif
	#endif

	if ( nurb.validNurb() )
	{
		CString msg;
		CDbPoint* dbPoint;

		status.Diagnostic( "Nurb is valid" );

		double t = nurb.tMin();
		double delta = (nurb.tMax() - nurb.tMin()) / 10;
		while (t <= nurb.tMax())
		{
			C3dCoord pt = nurb.evaluate( t );

			msg.Format( "X: %f  Y:%f", pt.X(), pt.Y() );
			status.Diagnostic( msg );

			model.EntityCreate( DBPOINT, (CDbEntity**) &dbPoint );
			dbPoint->Init( dbLayer, dbWork, pt );
			dbPoint->SystemFlag( false );
			dbPoint->ColorSet( DCOLOR_RED );
			
			t += delta;
		}
	}
	else
	{
		status.Diagnostic( "Nurb is not valid" );
	}
#endif ///

#endif //
	return status;
}

// ToolPath:Raster: [id=%d,] bdry=%d, toolid=%d, ang=%f, step=%f
CReturn 
CCreateProcessApp::Raster( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;

	CReturn			error;
	CRaster			raster;
	CWmChainList	trimmingChains;
	CWmChainList	rasterChains;
	CWmChain*		bdryChain;
	CDbFeature*		dbFeature;
	CDbEntity*		dbBoundary;
	CDbTool*		dbTool;
	double			ang, step;
	int				toolID;
	int				featureID;
	int				boundaryID;

	CModel& model = io_cmd->getModel();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Retrieve and validate the parameters.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	featureID = io_cmd->VarList().getInt( "id", 0 );  // TODO: for regen

	toolID = io_cmd->VarList().getInt( "toolid", 0 );
	boundaryID = io_cmd->VarList().getInt( "bdry", 0 );
	ang = io_cmd->VarList().getReal( "ang", 0. );
	step = io_cmd->VarList().getReal( "step", 0. );

	model.EntityFind( toolID, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	model.EntityFind( boundaryID, (CDbEntity**) &dbBoundary, DBLINE, DBPROFILE );

	if (dbTool == NULL)
	{
		error.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Raster() -- invalid tool." );
		status += error;
	}

	if (dbBoundary == NULL)
	{
		error.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Raster() -- invalid boundary." );
		status += error;
	}

	if (step < SMALL)
	{
		error.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Raster() -- invalid step." );
		status += error;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Create the paths.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	dbFeature = NULL;

	if ( status.IsOk() )
	{
		bdryChain = ProfileConvert( dbBoundary );

		if (bdryChain == NULL)
		{
			error.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Raster() -- invalid boundary." );
			status += error;
		}
		else
		{
			trimmingChains.Append( bdryChain );

			RasterChainsCreate( bdryChain, ang, step, &rasterChains );

			raster.Trim( &trimmingChains, &rasterChains, SMALL, 0. );

			dbFeature = ToolPathCreate( raster, dbTool, &model );

			rasterChains.DestructiveFlush();
			trimmingChains.DestructiveFlush();
		}
	}

	io_cmd->setInt( "id", ((dbFeature == NULL) ? 0 : dbFeature->Id()) );

	return status;
}

// ToolPath:Slit: toolid=%d, ang=%f, step=%f [,setback=%f]
//
// Slits the material skeleton.
//
CReturn 
CCreateProcessApp::Slit( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;

	CReturn			error;
	CModel			copiedModel;
	CRaster			raster;
	CWmChainList	trimmingChains;
	CWmChainList	rasterChains;
	CWmChain*		bdryChain;
	CDbFeature*		dbFeature;
	CDbTool*		dbTool;
	double			ang, step;
	double			set_back;
	int				toolID;

	CModel& model = io_cmd->getModel();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Retrieve and validate the parameters.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	toolID = io_cmd->VarList().getInt( "toolid", 0 );
	ang = io_cmd->VarList().getReal( "ang", 0. );
	step = io_cmd->VarList().getReal( "step", 0. );

	// 2007.06.06 (PE) -- Introduced set-back for General Thermodynamics
	// because the ends of the slitting moves actually nicked the parts.
	set_back = io_cmd->VarList().getReal( "setback", 0. );

	model.EntityFind( toolID, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	if (dbTool == NULL)
	{
		error.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Slit() -- invalid tool." );
		status += error;
	}

	if (step < SMALL)
	{
		error.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Slit() -- invalid step." );
		status += error;
	}

	copiedModel.UndoBufferSuppress();
	ModelCopy( model, &copiedModel );

	TrimmingChainsGet( &copiedModel, &trimmingChains );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Create the paths.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	dbFeature = NULL;

	if ( status.IsOk() )
	{
		bdryChain = trimmingChains[0];

		RasterChainsCreate( bdryChain, ang, step, &rasterChains );

		raster.Trim( &trimmingChains, &rasterChains, SMALL, set_back );

		dbFeature = ToolPathCreate( raster, dbTool, &model );

		rasterChains.DestructiveFlush();
		trimmingChains.DestructiveFlush();
	}

	io_cmd->setInt( "id", ((dbFeature == NULL) ? 0 : dbFeature->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Edit:Drop: action=%d [, x=%f, y=%f]
//
// Affects all selected line/arc/hole entities
//   where:
//   action -- (0) drop / (1) stop / (2) remove
//   x -- x slide distance
//   y -- y slide distance
//
// Kind of a weird home for this command.
//
// Note that there are three ways the Edit Drop/Stop feature may be used:
//	1. Select a single curve and adjust it's markings.
//	2. Select a single profile or (lead) feature and adjust its markings.
//	3. Do a filtered-select to get all the curves, profiles, and/or
//		features (e.g. not holes) and adjust the markings
//
//	It is common for the profiles to be in the pattern:  Lead in, Profile, Lead out.
//	In this case, we ONLY want drop/stop on the lead-out.  If there is no lead out,
//	then we ONLY want it on the last curve in the profile.  If we have a naked
//	curve, e.g. not in a selected profile, we apply to that.
//
CReturn 
CCreateProcessApp::DropStop( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn			status;
	EDbEntityType	type;
	CDbContainer*	dbContainer;
	CDbProfile*		dbProfile;
	CDbEntity*		dbEntity;
	double			xslide, yslide;
	int				action; //, ecnt;
	int				count, indx;

	CModel& model = io_cmd->getModel();
	CSelector& selector = model.SelectorStack()();

	action = io_cmd->VarList().getInt( "action", IUNDEFINED );
	xslide = io_cmd->VarList().getReal( "x", UNDEFINED );
	yslide = io_cmd->VarList().getReal( "y", UNDEFINED );

	if (action < 0 || action > 2)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::DropStop()" );
		return status;
	}

	count = selector.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = selector[indx];
		//
		// Note:  If an entity has a PROFILE or FEATURE parent, use it from the
		// parent!  Currently, *all* curves are getting led, and its really annoying.
		//
		type = dbEntity->Type();
		
		if (action == 0 || action == 1)
		{
			// Ignore features.  Lead-out is captured by the profile.
			if (type == DBPROFILE)
			{
				dbProfile = dynamic_cast<CDbProfile*>( dbEntity );
				dbEntity = CModelUtil::FindLeadOut(dbProfile);
				type = dbEntity->Type();
			}
			else
			{ dbProfile = NULL; }

			if (type == DBLINE || type == DBARC) // || type == DBHOLE)  //Holes?  Really?
			{
				if (dbProfile == NULL)
				{
					dbContainer = dynamic_cast<CDbContainer*>( dbEntity->Owner() );
					if (dbContainer)
					{
						type = dbContainer->Type();
						if ( ( (type == DBPROFILE)
								|| (type == DBFEATURE) )
							&& dbContainer->IsSelected() )
						{
							continue;
						}
					}
				}

				dbEntity->IntSet( "_dropstop", (action+1) );

				if (xslide < UNDEFINED && yslide < UNDEFINED)
				{
					dbEntity->DoubleSet( "_slide_dx", xslide );
					dbEntity->DoubleSet( "_slide_dy", yslide );
				}
			}
		}
		else
		{
			dbEntity->AttribDelete("_dropstop");

			// Remove the older form of data ... just because we can.
			// This step is not really necessary ... but leaves things cleaner.
			dbEntity->AttribDelete("_dropstop_dx");
			dbEntity->AttribDelete("_dropstop_dy");

			dbEntity->AttribDelete("_slide_dx");
			dbEntity->AttribDelete("_slide_dy");
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CWmChain*
CCreateProcessApp::ProfileConvert( const CDbEntity* boundary )
{
	const CDbProfile*	dbProfile;
	const CDbCurve*		dbCurve;
	const CDbArc*		dbArc;
	const CDbEntity*	bdry;
	CWmChain*			chain;
	int					count, indx;

	chain = NULL;

	bdry = boundary;

	if (bdry->Type() == DBLINE)
		bdry = bdry->Owner();

	if (bdry != NULL)
	{
		if (bdry->Type() == DBARC)
		{
			dbArc = (const CDbArc*) bdry;  // assume 360 degree

			chain = new CWmChain();
			chain->Append( dbArc->Arc() );
		}
		else if (bdry->Type() == DBPROFILE)
		{
			dbProfile = (const CDbProfile*) bdry;

			chain = new CWmChain();

			count = dbProfile->Count();
			for (indx = 0; indx < count; ++indx)
			{
				dbCurve = dynamic_cast<CDbCurve*>( (*dbProfile)[indx] );
				chain->Append( dbCurve->Curve() );
			}
		}
	}

	return chain;
}

void
CCreateProcessApp::RasterChainsCreate(
							CWmChain*		bdryChain,
							double			angle,
							double			step,
							CWmChainList*	rasterChains )
{
	C3x4Matrix			forward;
	C3x4Matrix			inverse;
	C3dBox				box;
	CWmChain*			path = NULL;
	double				ymin, ymax, ypt;
	double				xmin, xmax;
	double				residual;
	double				radians;
	int					count, indx;

	radians = angle * DEG2RAD;
	forward.setXYAngle( radians );
	forward.InvertTo( &inverse );

	bdryChain->Xform( forward );
	box = bdryChain->Box();

	ymin = box.Ymin();
	ymax = box.Ymax();

	xmin = box.Xmin() - 0.001;  // ensure overlap
	xmax = box.Xmax() + 0.001;  // ensure overlap

	count = (int) (box.Dy() / step);
	residual = ymax - (ymin + (count * step));
	if (residual > 1.e-3)
		++count;

	for (indx = 1; indx < count; ++indx)
	{
		ypt = ymin + (indx * step);

		path = new CWmChain();
		path->Append( new CGeoLine( xmin, ypt, xmax, ypt ) );

		path->Xform( inverse );
		rasterChains->Append( path );
	}

	bdryChain->Xform( inverse );
}

CDbFeature*
CCreateProcessApp::ToolPathCreate(
						const CRaster&	raster,
						CDbTool*		dbTool,
						CModel*			model )
{
	CDbFeature*			dbFeature;
	CDbWorkplane*		dbWork;
	CDbEntity*			dbEntity;
	CWmChainIterator	iter;
	CWmElem*			elem;
	int					color, prevColor;
	int					count, indx;

	const CWmChainList&	chains = raster.Results();

	color = dbTool->ColorGet( DCOLOR_RED );
	prevColor = model->Default().getColor( color );

	dbWork = model->ActiveWorkplane();

	model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );

	if (model->ActivePattern() != NULL)
		model->ActivePattern()->Append( dbFeature );

	count = chains.Count();

	for (indx = 0; indx < count; ++indx)
	{
		iter.Init( (* (chains[indx]) ) );

		while ( !iter.AtEnd() )
		{
			elem = iter.Elem();

			dbEntity = model->Db().GeoConvert( (*(elem->Elem())), dbTool, dbWork );
			dbFeature->Append( dbEntity );

			iter.NextElem();
		}
	}

	model->pDefault()->setColor( prevColor );

	return dbFeature;
}

static bool IsAcceptedTool( const CDbTool* dbTool )
{
	bool is_accepted = false;
	if (dbTool != NULL)
	{
		is_accepted = (!dbTool->IsLayer() && !dbTool->IsPunchTool());
	}
	return is_accepted;
}

// NOTE: This method was introduced for Modular Services for the
// purpose of slitting the material skeleton with laser.
//
CReturn
CCreateProcessApp::TrimmingChainsGet(
							CModel*			model,
							CWmChainList*	trimmingChains )
{
	CReturn			status;
	CDbIterator		iter;
	CDbEntityList	profs;
	CDbTool*		dbTool;
	CDbProfile*		dbProfile;
	CWmChain*		chain;
	double			dx, dy;
	int				workType;
	int				count, indx;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the stock boundary.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	workType = model->Header().getInt( "WorkplaneType", 1 );

	dx = model->Header().getReal( STR_LENGTH, 0. );
	dy = model->Header().getReal( STR_WIDTH, 0. );

	chain = new CWmChain();
	chain->Append( new CGeoLine( 0., 0., 0.,  0., dy, 0. ) );
	chain->Append( new CGeoLine( 0., dy, 0.,  dx, dy, 0. ) );
	chain->Append( new CGeoLine( dx, dy, 0.,  dx, 0., 0. ) );
	chain->Append( new CGeoLine( dx, 0., 0.,  0., 0., 0. ) );

	if (workType == 2)
	{
		C3x4Matrix	xform;

		xform.Shift( C3dVec( 0., -dy, 0. ) );
		chain->Xform( xform );
	}

	trimmingChains->Append( chain );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get all of the interior profiles / circles.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( model->Db(), DBPROFILE );
	while (1)
	{
		dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		dbTool = dbProfile->Tool();
		if ( IsAcceptedTool( dbTool ) ) // ie. only profiling tools
			profs.Append( dbProfile );

		iter.Next();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Reduce this set of profiles to the set of outermost profiles.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	status = CModelUtil::MarkIntExt( &profs, FALSE, model );

	count = profs.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbProfile = (CDbProfile*) profs[indx];

		if (dbProfile->IntGet( STR_PROFILE_DEPTH, IUNDEFINED) == 0)
		{
			chain = ProfileConvert( dbProfile );
			if (chain->Area() < 0)
				chain->Reverse();

			trimmingChains->Append( chain );
		}
	}

	return status;
}


// V16 introduced pattern-based nesting.  As such, we must
// copy the model and explode the copied instances inline.
// The copied model entities are then used.
void
CCreateProcessApp::ModelCopy(
							const CModel&	srcModel,
							CModel*			cpyModel )
{
	CEntityCopier	copier;
	CDbIterator		iter;
	CDbFeature*		dbFeature;
	CDbPattern*		dbPattern;
	CDbProfile*		dbProfile;
	CDbCommand*		dbCommand;
	CDbTool*		dbTool;
	CDbEntity*		dbEntity;
	CDbEntity*		theCopy;
	ID				patid;

	copier.Init( &(cpyModel->Db()), FLAG_COPY_TOOL_VERBATIM );

	(*(cpyModel->pHeader())) = srcModel.Header();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy the basic reference entities.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( srcModel.Db(), DBWORKPLANE );
	while (1)
	{
		dbEntity = iter();
		if (dbEntity->Type() > DBTOOL)
			break;

		dbEntity->Accept( &copier );
		
		iter.Next();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy any patterns (subroutine definitions).
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( srcModel.Db(), DBPATTERN );
	while (1)
	{
		dbPattern = dynamic_cast<CDbPattern*>( iter() );
		if (dbPattern == NULL)
			break;

		dbPattern->Accept( &copier );
		theCopy = copier.ForwardLookup( dbPattern );

		if (theCopy != NULL)
			theCopy->StringSet( STR_TYPE, "_part" );

		iter.Next();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy and explode any instances (subcalls).
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( srcModel.Db(), DBCOMMAND );
	while (1)
	{
		dbCommand = dynamic_cast<CDbCommand*>(iter());
		if (dbCommand == NULL)
			break;

		if ( dbCommand->IsInstance() )
		{
			// Remap the pattern reference of this instance.
			patid = (ID) dbCommand->IntGet( "patid", 0 );
			srcModel.Db().Find( patid, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );

			// Copy the instance and link it to its copied pattern.
			dbCommand->Accept( &copier );
			dbCommand = dynamic_cast<CDbCommand*>( copier.ForwardLookup( dbCommand ) );

			dbPattern = dynamic_cast<CDbPattern*>( copier.ForwardLookup( dbPattern ) );
			dbCommand->IntSet( "patid", dbPattern->Id() );

			CModelUtil::PatternExplode( &dbCommand, &dbFeature );

			if (dbFeature != NULL)
				dbFeature->StringSet( STR_TYPE, "_part" );
		}

		iter.Next();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy all profiles.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( srcModel.Db(), DBPROFILE );
	while (1)
	{
		dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		dbTool = dbProfile->Tool();
		if (dbTool != NULL && !dbTool->IsLayer())
			dbProfile->Accept( &copier );

		iter.Next();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Delete the patterns (otherwise they cause grief for Slit).
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	iter.Init( cpyModel->Db(), DBPATTERN );
	while (1)
	{
		dbPattern = dynamic_cast<CDbPattern*>( iter() );
		if (dbPattern == NULL)
			break;

		dbPattern->Delete();

		iter.Next();
	}
}

// Toolpath:Nibble:
CReturn 
CCreateProcessApp::Nibble( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	int		total_hits;

	CModel& model = io_cmd->getModel();

	total_hits = CNibbler::Nibble( &model );

	io_cmd->setInt( "cnt", total_hits );

	return status;
}
