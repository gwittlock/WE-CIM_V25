
#include "stdafx.h"
#include <vector>

#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"

#include "DbPoint.h"
#include "DbLine.h"
#include "DbArc.h"

#include "DbCurve.h"
#include "DbCurveList.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "DbSequence.h"
#include "StdCurveFilter.h"
#include "Model.h"
#include "ProfileBuilder.h"
#include "Profile.h"
#include "Conversion.h"

#include "CreateProcess.h"

const CString TAG("~tag");

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Create:
//
CReturn CCreateProcessApp::ProfileCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();

	CDbProfile* dbProfile = NULL;

	// -----------------------------------------------------
	// Create the Profile.
	//
	status = model.EntityCreate( DBPROFILE, (CDbEntity**)&dbProfile );

	if (model.ActivePattern() != NULL)
		model.ActivePattern()->Append( dbProfile );

	if (dbProfile == NULL || !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileCreate()" );
	}
	else
	{
		io_cmd->setInt( "id", dbProfile->Id() );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Modify: id = %d, [flush = %d], add = %d
//
// NOTE: It is the client's responsibility to provide a valid curve id.
//       See also CCreateProcessApp::IsValidProfileCurve().
//
CReturn CCreateProcessApp::ProfileModify( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbProfile* dbProfile = NULL;

	ID id = 0;
	ID addId = 0;
	int flush = 0;
	
	// -----------------------------------------------------
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getInt( "add", (int*)&addId );
	io_cmd->getInt( "flush", &flush );

	if ((id < 1) || (addId < 1))
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileModify()" );
		return status;
	}


	// -----------------------------------------------------
	// Fetch the Profile.
	status = model.EntityFind( id, (CDbEntity**)&dbProfile, DBPROFILE, DBPROFILE );
	
	if ( !status.isOkay() )
		return status;

	// -----------------------------------------------------
	// Manipulate the Profile
	if ( flush )
		dbProfile->BenignFlush();

	if (addId > 0)
	{
		CDbEntity* dbEntity;
		model.EntityFind( addId, (CDbEntity**) &dbEntity );
		CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
		if (dbCurve == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileModify()" );
		}
		else
		{
			status = dbProfile->Append( dbCurve );
			dbProfile->ModifyFlag( true );
		}
	}

	if ( status.IsOk() )
		io_cmd->setInt( "id", dbProfile->Id() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:IsClosed: id = %d
//
// Returns bool = %d
//
CReturn CCreateProcessApp::ProfileIsClosed( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	ID id = 0;
	io_cmd->getInt( "id", (int*)&id );

	if (id <= 0)
	{
		status = STATUS_ERROR;
	}
	else
	{
		CModel&	model = io_cmd->getModel();

		CDbProfile* dbProfile;
		status = model.EntityFind( id, (CDbEntity**)&dbProfile, DBPROFILE, DBPROFILE );
	
		if (dbProfile == NULL)
		{
			status = STATUS_ERROR;
		}
		else
		{
			io_cmd->setInt( "bool", (( dbProfile->IsClosed() ) ? 1 : 0) );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileModify()" );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Grow: seed = %d [, assoc = %d] [, tol = %f]
//
//    seed    -- the id of the seed curve
//    assoc   -- [0] false / [1] true, associate adjacent curves
//               through shared end points.  Defaults to false.
//    tol     -- coincident point tolerance (defaults to SMALL)
//    clean   -- clean & join tolerance ( defaults to SMALL )
//
// NOTE: It is the client's responsibility to provide a valid curve id.
//       See also CCreateProcessApp::IsValidProfileCurve().
//
CReturn CCreateProcessApp::ProfileGrow( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CStdCurveFilter filter;

	CDbProfile* dbProfile = NULL;
	CDbEntity* dbEntity = NULL;
	CDbCurve* seedCurve = NULL;

	CModel&	model = io_cmd->getModel();
	
	// -----------------------------------------------------
	// Get the command parameters
	ID id			= io_cmd->VarList().getInt( "seed", 0 );
	double gapTol	= io_cmd->VarList().getReal( "tol", SMALL );
	double cleanTol	= io_cmd->VarList().getReal( "clean", SMALL );
	int assoc		= io_cmd->VarList().getInt( "assoc", FALSE );
	int special		= io_cmd->VarList().getInt( "special", FALSE );
	int same		= io_cmd->VarList().getInt( "same", FALSE );

	if (special == 0)
	{
		if (id < 1)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileGrow(#1)" );
			return status;
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Get the seed curve for growing the profile.
		status = model.EntityFind( id, &dbEntity, DBLINE, DBARC );
		if ( !status.IsOk() )
			return status;

		seedCurve = dynamic_cast<CDbCurve*>( dbEntity );
		if (seedCurve == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CreateProcess::ProfileGrow(#2) bad seed curve id" );
			return status;
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// If we're going to modify an existing profile ...
		dbProfile = dynamic_cast<CDbProfile*>( seedCurve->Owner() );
		if (dbProfile != NULL)
		{
			if ( !assoc )
				dbProfile->Disassociate();

			dbProfile->BenignFlush();
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Create/Modify the profile as required.

		// TODO:  Put these flag switches under an input parameter?
		filter.TestLoose( FALSE );
		filter.TestLayer( same );
		filter.TestHidden( TRUE );	// Don't chain hidden geometry

		filter.Init( model, seedCurve );

		if (dbProfile == NULL)
		{
			// We're creating a new profile.

			// NOTE: Prior to the use of CDbProfile::Init(), the profile
			// was created referencing the active layer and workplane.
			// Application policy dictates that the profile must reference
			// the same layer and workplane as its children, however.

			status = CProfileBuilder::ProfileGrow(
						&model, gapTol, seedCurve, &(filter.Curves()), &dbProfile );
		}
		else
		{
			// We're rechaining an existing profile.
			CDbCurveList profCurves;
			status = CProfileBuilder::ProfileGrow(
						seedCurve, gapTol, &(filter.Curves()), &profCurves );
			dbProfile->Append( profCurves );
			dbProfile->ModifyFlag( true );
		}
	}
	else if (special > 0)
	{
		// Chain using selection order.
		CDbCurveList	inputCurves;
		CDbCurve*		dbCurve;
		int				count, indx;

		CSelector&	selector = model.SelectorStack()();

		// NOTE: We ignore 'same' here because we have an explicit selection.
		count = selector.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbCurve = dynamic_cast<CDbCurve*>( selector[indx] );
			if (dbCurve != NULL)
				inputCurves.Append( dbCurve );
		}

		if (inputCurves.Count() < 1)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileGrow(#3)" );
			return status;
		}

		// We need the seed curve only for the purpose of setting attributes.
		seedCurve = inputCurves.GetAt(0);

		// Create a profile through the selected curves. Note, this process
		// may prematurely terminate when a selected curve is out of proximity
		// of the terminal curve in the profile. Also, it is possible for this
		// function to move curves between profiles.
		status = CProfileBuilder::ProfileSelectedGrow(
					&model, gapTol, &inputCurves, &dbProfile );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Do the common stuff.
	if ( status.IsOk() )
	{
		if (model.ActivePattern() != NULL)
			model.ActivePattern()->Append( dbProfile );
	}

	if (status.IsOk() && assoc)
		status = dbProfile->Associate( cleanTol );


	if ( status.IsOk() )
		io_cmd->setInt( "id", dbProfile->Id() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Explode: id=%d, group=%b
//
//    The curves in the profile are disassociated from their
//    common end points, and then the profile entity is deleted.
//
CReturn CCreateProcessApp::ProfileExplode( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CReturn	status;

	CModel&	model = io_cmd->getModel();
	const CVarList& args = io_cmd->VarList();

	ID id = args.getInt( "id", 0 );
	bool group = (args.getInt( "group", 0 ) != 0);
	if ((id == 0) && !group)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CreateProcess::ProfileExplode()" );
		return status;
	}


	std::vector<ID> profids;
	CDbProfile* dbProfile;

	if ( group )
	{
		// The group parameter takes precedence over id.
		CSelectorStack& selectorStack = model.SelectorStack();
		CSelector& selector = selectorStack();

		int count = selector.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			dbProfile = dynamic_cast<CDbProfile*>( selector[indx] );
			if (dbProfile != NULL)
				profids.push_back( dbProfile->Id() );
		}
		
	}
	else
	{
		profids.push_back( id );
	}

	int count = profids.size();
	for (int indx = 0; indx < count; ++indx)
	{
		id = profids[indx];
		status = model.EntityFind( id, (CDbEntity**) &dbProfile, DBPROFILE, DBPROFILE );

		if ( !status. IsOk() )
		{
			status.Internal( IDS_INTERNAL_ERROR, "CreateProcess::ProfileExplode()" );
			return status;
		}

		dbProfile->Disassociate();

		// Prepare to move the entities to the owner of the profile.
		CDbEntityList dbEntities;
		CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbProfile->Owner() );

		if (dbContainer != NULL)
		{
			// Move the curves out of the profile and into the owner.
			int count = dbProfile->Count();
			for (int indx = 0; indx < count; ++indx)
			{
				CDbEntity* dbEntity = (*dbProfile)[ 0 ];
				dbProfile->Disown( dbEntity );
				dbContainer->InsertBefore( dbProfile, dbEntity );
			}
		}
		else
		{
			// Simply flush the profile.
			dbProfile->BenignFlush();
		}


		dbProfile->Delete();
		dbProfile->ModifyFlag( true );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Selected: [assoc = %d,] [tol = %f]
//
//    assoc   -- [0] false / [1] true, associate adjacent curves
//               through shared end points.  Defaults to false.
//
//    tol     -- coincident point tolerance (defaults to SMALL)
//
CReturn CCreateProcessApp::ProfileSelected( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CStdCurveFilter filter;
	CDbCurveList inputCurves;
	CDbCurveList outputCurves;
	CDbCurve* seedCurve = NULL;

	CModel&	model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int assoc = 0;
	double tol = SMALL;
	
	// -----------------------------------------------------
	// Get the command parameters
	//
	io_cmd->getInt( "assoc", &assoc );
	io_cmd->getReal( "tol", &tol );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Filter the selection set for usable curves.
	int count = selector.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = selector[indx];

		CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

		if (dbCurve == NULL)
			continue;

		if ( !dbCurve->IsDeleted() )
			inputCurves.Append( dbCurve );
	}



	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Build profiles using the selected curves.
	while (1)
	{
		if (filter.Curves().Count() < 1)
		{
			if (inputCurves.Count() < 1)
				break;  // proper termination

			seedCurve = inputCurves.Remove( 0 );

			filter.Init( &inputCurves, seedCurve );
		}
		else
		{
			seedCurve = filter.Curves().Remove( 0 );
		}

		status = CProfileBuilder::ProfileGrow(
					seedCurve, tol, &(filter.Curves()), &outputCurves );

		if (!status.IsOk() || outputCurves.Count() < 1)
			break;

		CDbProfile* dbProfile;
		status = model.EntityCreate( DBPROFILE, (CDbEntity**)&dbProfile );

		if ( !status.IsOk() )
			break;

		if (model.ActivePattern() != NULL)
			model.ActivePattern()->Append( dbProfile );

		dbProfile->Append( outputCurves );

		if ( assoc )
			status = dbProfile->Associate( tol );

		if ( !status.IsOk() )
			break;

		outputCurves.BenignFlush();
	}

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Reverse: id=%d, group=%b
//
CReturn CCreateProcessApp::ProfileReverse( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;
	CDbEntity* dbEntity;

	CModel&	model = io_cmd->getModel();
	
	const CVarList& args = io_cmd->VarList();

	ID id = args.getInt( "id", 0 );
	bool group = (args.getInt( "group", 0 ) != 0);
	if ((id == 0) && !group)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CreateProcess::ProfileReverse(#1)" );
		return status;
	}

	if ( group )
	{
		// The group parameter takes precedence over id.
		// We're reversing the entities in the selection set.
		CDbEntityArray dbEntities;

		CSelector& selector = model.SelectorStack()();

		// Build the 'actual' selection set of curves and/or profiles.
		int indx, count = selector.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = selector[indx];

			CDbProfile* dbProfile = NULL;

			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
			if (dbCurve == NULL)
			{
				dbProfile = dynamic_cast<CDbProfile*>( dbEntity );
			}
			else
			{
				dbProfile = dynamic_cast<CDbProfile*>( dbCurve->Owner() );
				if (dbProfile == NULL)
				{
					// We have an orphan curve.
					dbEntities.Append( dbCurve );
				}
				else if ( !dbProfile->IsSelected() )
				{
					// We can not simply change the direction of a curve in the
					// profile. We must change the direction of the complete profile.
					dbProfile = NULL;
				}
			}

			// Ensure the uniqueness of profiles.
			if (dbProfile != NULL)
				dbEntities.ConditionalAppend( dbProfile );
		}

		count = dbEntities.Count();
		if (count < 1)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileReverse(#3)" );
			return status;
		}

		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = dbEntities.GetAt(indx);
			status = EntityReverse( dbEntity );
			if ( !status.IsOk() )
				break;
		}

		// Fodder for the 'returned entity id'.
		dbEntity = dbEntities.GetAt(0);
	}
	else
	{
		// Reverse the given curve/profile.
		status = model.EntityFind( id, &dbEntity, DBLINE, DBPROFILE );
		if ( !status.IsOk() )
			return status;

		if ( dbEntity->IsDeleted() )
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileReverse(#2)" );
			return status;
		}

		status = EntityReverse( dbEntity );
	}

	if ( status.IsOk() )
		io_cmd->setInt( "id", dbEntity->Id() );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Split: id = %d, x = %f, y = %f, z = %f [, gap = %f] [, reparent=%b]
// Profile:Split: id = %d, (midpts = %d | corners = %d) [, gap = %f] [, reparent=%b]
//
// Note: The given id must represent a curve.
//       The {x,y,z} must be in the entity's workplane
//       (eg. as returned by SolveProcess::Split())
//
CReturn CCreateProcessApp::ProfileSplit( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	const CVarList& params = io_cmd->VarList();
	CModel&	model = io_cmd->getModel();

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	ID id = params.getInt( "id", 0 );

	int midpts = params.getInt( "midpts", FALSE );
	int corners = params.getInt( "corners", FALSE );
	double gap = params.getReal( "gap", 0. );

	C3dCoord nearPt;
	if ((midpts == 0) && (corners == 0))
	{
		double xp = params.getReal( "px", UNDEFINED );
		double yp = params.getReal( "py", UNDEFINED );
		double zp = params.getReal( "pz", UNDEFINED );

		nearPt.XYZ( xp, yp, zp );
	}

	bool reparent = (params.getInt( "reparent", FALSE ) > 0);

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the entity.
	CDbEntity* dbEntity = NULL;
	if (id > 0 && status.IsOk())
		status = model.EntityFind( id, &dbEntity, DBLINE, DBARC );

	if ( dbEntity == NULL || dbEntity->IsDeleted() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileSplit(#1)" );
		return status;
	}
	
	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

	if (midpts || corners)
	{
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbCurve->Owner() );
		if (dbProfile != NULL)
		{
			if ( midpts )
				ProfileMidptsSplit( dbProfile, gap, reparent );

			if ( corners )
				ProfileCornersSplit( dbProfile, gap, reparent );
		}
	}
	else
	{
		ID id = ProfileCurveSplit( dbCurve, nearPt, gap, reparent );

		io_cmd->setInt( "id", id );
		if (id == 0)
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileSplit(#2)" );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Blend: id1=%d, end1=%d, x1=%f, y1=%f,	
//					id2=%d, end2=%d, x2=%f, y2=%f,
//					cx=%f, cy=%f, dir=%d
//
// Note: The given ids must represent curves.
//       The {x,y} must be in the entity's workplane
//       (eg. as returned by SolveProcess::ArcTT())
//
CReturn CCreateProcessApp::ProfileBlend( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	//
	ID		id1 = 0;
	int		end1;
	io_cmd->getInt( "id1", (int*)&id1 );
	status += io_cmd->getInt( "end1", &end1 );

	double	x1, y1;
	status += io_cmd->getReal( "x1", &x1 );
	status += io_cmd->getReal( "y1", &y1 );

	ID		id2 = 0;
	int		end2;
	io_cmd->getInt( "id2", (int*)&id2 );
	status += io_cmd->getInt( "end2", &end2 );

	double	x2, y2;
	status += io_cmd->getReal( "x2", &x2 );
	status += io_cmd->getReal( "y2", &y2 );

	double	cx, cy;
	status += io_cmd->getReal( "cx", &cx );
	status += io_cmd->getReal( "cy", &cy );

	int	dir;
	status += io_cmd->getInt( "dir", &dir );


	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileBlend()" );
		return status;
	}

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the entities
	//
	CDbEntity*		dbEntity1 = NULL;
	if (id1 > 0)
		status = model.EntityFind( id1, &dbEntity1, DBLINE, DBARC );

	CDbEntity*		dbEntity2 = NULL;
	if (id2 > 0)
		status = model.EntityFind( id2, &dbEntity2, DBLINE, DBARC );

	if ( !status.IsOk()
		|| !dbEntity1
		|| !dbEntity2 )
		return status;

	if ( dbEntity1->IsDeleted()
		|| dbEntity2->IsDeleted() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileBlend()" );
		return status;
	}

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Blend the entities
	//
	CDbCurve* dbCurve1 = dynamic_cast<CDbCurve*>( dbEntity1 );
	CDbCurve* dbCurve2 = dynamic_cast<CDbCurve*>( dbEntity2 );

	if ( (dbCurve1 != NULL)
		&& (dbCurve2 != NULL) )
	{
		CDbTool*		dbTool = dbCurve1->Tool();
		CDbWorkplane*	dbWorkplane = dbCurve1->Workplane();

		CDbContainer* dbContainer1 = dynamic_cast<CDbContainer*>( dbEntity1->Owner() );
		CDbContainer* dbContainer2 = dynamic_cast<CDbContainer*>( dbEntity2->Owner() );

		CDbProfile* dbProfile1 = dynamic_cast<CDbProfile*>( dbContainer1 );
		CDbProfile* dbProfile2 = dynamic_cast<CDbProfile*>( dbContainer2 );

		if (dbContainer1 != dbContainer2)
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileBlend()" );
		else
		{
			// Break up the profile?
			bool associated = ((dbProfile1 == NULL) ? FALSE : dbProfile1->IsAssociated() );
			if (associated)
				dbProfile1->Disassociate();

			// Adjust our two entities...
			CDbPoint*	pt1;
			CDbPoint*	pt2;
			if (end1)
			{
				pt1 = dbCurve1->DbEndPt();
				pt2 = dbCurve1->DbStartPt();
			}
			else
			{
				pt1 = dbCurve1->DbStartPt();
				pt2 = dbCurve1->DbEndPt();
			}

			C3dCoord	cd1( x1, y1, pt1->Coord().Z() );
			if (!cd1.WithinTol( pt2->Coord(), SMALL ))
			{
				pt1->Init( dbTool, dbWorkplane, cd1 );
				pt1->ModifyFlag( true );
			}

			if (end2)
			{
				pt2 = dbCurve2->DbEndPt();
				pt1 = dbCurve2->DbStartPt();
			}
			else
			{
				pt2 = dbCurve2->DbStartPt();
				pt1 = dbCurve2->DbEndPt();
			}

			C3dCoord	cd2( x2, y2, pt2->Coord().Z() );
			if (!cd2.WithinTol( pt1->Coord(), SMALL ))
			{
				pt2->Init( dbTool, dbWorkplane, cd2 );
				pt2->ModifyFlag( true );
			}

			// Create the blend arc...
			C3dCoord	ctr( cx, cy, cd1.Z() );
			CDbArc*		blend;
			model.EntityCreate( DBARC, (CDbEntity**)&blend );
			if (end1)
				blend->Init( dbTool, dbWorkplane, cd1, cd2, ctr, dir );
			else // end2
				blend->Init( dbTool, dbWorkplane, cd2, cd1, ctr, -dir );
			*(blend->pAttrib()) = dbCurve1->Attrib();

			blend->Tool( dbCurve1->Tool() );

			// Re-assemble?
			if (dbContainer1 != NULL)
			{
				C2dCoord pe = blend->EndPt();
				C2dCoord ps = dbCurve1->StartPt();

				if ( pe.WithinTol( ps, SMALL ) )
					dbContainer1->InsertBefore( dbCurve1, blend );
				else
					dbContainer1->InsertAfter( dbCurve1, blend );

				if (dbContainer1->Type() != DBFEATURE)
				{
					// Avoid the bug where (for instance) filleting
					// two lines in a toolpath causes the feature
					// to recalculate.
					dbContainer1->ModifyFlag( true );
				}

				if (associated)
					dbProfile1->Associate( SMALL );
			}
			else
			{
				if (model.ActivePattern() != NULL)
					model.ActivePattern()->Append( blend );
			}

			CDbSequence::ConditionalSequence( dbCurve1, blend, TRUE );

			if (blend != NULL)
			{
				io_cmd->setInt( "id", blend->Id() );
			}
		}
	}
	else
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileBlend()" );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Chamfer: id1=%d, end1=%d, x1=%f, y1=%f, 
//					id2=%d, end2=%d, x2=%f, y2=%f,
//
// Note: The given ids must represent curves.
//       The {x,y} must be in the entity's workplane
//       (eg. as returned by SolveProcess::Chamfer())
//
//	TODO:  Merge the huge overlap between Blend and Chamfer
//
CReturn CCreateProcessApp::ProfileChamfer( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	//
	ID		id1 = 0;
	int		end1;
	io_cmd->getInt( "id1", (int*)&id1 );
	status += io_cmd->getInt( "end1", &end1 );

	double	x1, y1;
	status += io_cmd->getReal( "x1", &x1 );
	status += io_cmd->getReal( "y1", &y1 );

	ID		id2 = 0;
	int		end2;
	io_cmd->getInt( "id2", (int*)&id2 );
	status += io_cmd->getInt( "end2", &end2 );

	double	x2, y2;
	status += io_cmd->getReal( "x2", &x2 );
	status += io_cmd->getReal( "y2", &y2 );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileChamfer()" );
		return status;
	}

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the entities
	//
	CDbEntity*		dbEntity1 = NULL;
	if (id1 > 0)
		status += model.EntityFind( id1, &dbEntity1, DBLINE, DBARC );

	CDbEntity*		dbEntity2 = NULL;
	if (id2 > 0)
		status += model.EntityFind( id2, &dbEntity2, DBLINE, DBARC );

	if ( !status.IsOk()
		|| !dbEntity1
		|| !dbEntity2 )
		return status;

	if ( dbEntity1->IsDeleted()
		|| dbEntity2->IsDeleted() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileChamfer()" );
		return status;
	}

	//=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Chamfer the entities
	//
	CDbCurve* dbCurve1 = dynamic_cast<CDbCurve*>( dbEntity1 );
	CDbCurve* dbCurve2 = dynamic_cast<CDbCurve*>( dbEntity2 );

	if ( (dbCurve1 != NULL)
		&& (dbCurve2 != NULL) )
	{
		CDbTool*		dbTool = dbCurve1->Tool();
		CDbWorkplane*	dbWorkplane = dbCurve1->Workplane();

		CDbContainer* dbContainer1 = dynamic_cast<CDbContainer*>( dbEntity1->Owner() );
		CDbContainer* dbContainer2 = dynamic_cast<CDbContainer*>( dbEntity2->Owner() );

		CDbProfile* dbProfile1 = dynamic_cast<CDbProfile*>( dbContainer1 );
		CDbProfile* dbProfile2 = dynamic_cast<CDbProfile*>( dbContainer2 );

		if (dbProfile1 != dbProfile2)
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileChamfer()" );
		else
		{
			// Break up the profile?
			bool associated = ((dbProfile1 == NULL) ? FALSE : dbProfile1->IsAssociated() );
			if (associated)
				dbProfile1->Disassociate();

			// Adjust our two entities...
			CDbPoint*	pt1;
			CDbPoint*	pt2;
			if (end1)
			{
				pt1 = dbCurve1->DbEndPt();
				pt2 = dbCurve1->DbStartPt();
			}
			else
			{
				pt1 = dbCurve1->DbStartPt();
				pt2 = dbCurve1->DbEndPt();
			}

			C3dCoord	cd1( x1, y1, pt1->Coord().Z() );
			if (!cd1.WithinTol( pt2->Coord(), SMALL ))
			{
				pt1->Init( dbTool, dbWorkplane, cd1 );
				pt1->ModifyFlag( true );
			}


			if (end2)
			{
				pt2 = dbCurve2->DbEndPt();
				pt1 = dbCurve2->DbStartPt();
			}
			else
			{
				pt2 = dbCurve2->DbStartPt();
				pt1 = dbCurve2->DbEndPt();
			}

			C3dCoord	cd2( x2, y2, pt2->Coord().Z() );
			if (!cd2.WithinTol( pt1->Coord(), SMALL ))
			{
				pt2->Init( dbTool, dbWorkplane, cd2 );
				pt2->ModifyFlag( true );
			}

			// Create the chamfer line...
			CDbLine*		chamfer;
			model.EntityCreate( DBLINE, (CDbEntity**)&chamfer );
			if (end1)
				chamfer->Init( dbTool, dbWorkplane, cd1, cd2 );
			else // end2
				chamfer->Init( dbTool, dbWorkplane, cd2, cd1 );
			*(chamfer->pAttrib()) = dbCurve1->Attrib();

			chamfer->Tool( dbCurve1->Tool() );

			// Re-assemble?
			if (dbContainer1 != NULL)
			{
				dbContainer1->InsertAfter( dbCurve1, chamfer );

				if (dbContainer1->Type() != DBFEATURE)
				{
					// Avoid the bug where (for instance) filleting
					// two lines in a toolpath causes the feature
					// to recalculate.
					dbContainer1->ModifyFlag( true );
				}
				
				if (associated)
					dbProfile1->Associate( SMALL );
			}
			else
			{
				if (model.ActivePattern() != NULL)
					model.ActivePattern()->Append( chamfer );
			}

			CDbSequence::ConditionalSequence( dbCurve1, chamfer, TRUE );

			if (chamfer != NULL)
			{
				io_cmd->setInt( "id", chamfer->Id() );
			}
		}
	}
	else
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileChamfer()" );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:IsUsableCurve: id = %d
//
CReturn CCreateProcessApp::IsUsableCurve( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbProfile* dbCurve = NULL;

	ID id = 0;
	
	// -----------------------------------------------------
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );

	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::IsUsableCurve()" );
		return status;
	}


	// -----------------------------------------------------
	// Fetch the curve.
	if (id > 0)
		status = model.EntityFind( id, (CDbEntity**)&dbCurve, DBLINE, DBARC );

	if ( status.IsOk() && CDbProfile::IsUsableCurve( (*dbCurve) ) )
		io_cmd->setInt( "bool", 1 );
	else
		io_cmd->setInt( "bool", 0 );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Count: id = %d
//
CReturn CCreateProcessApp::ProfileCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbProfile* dbProfile = NULL;

	ID id = 0;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );

	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileCount()" );
		return status;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Profile.
	status = model.EntityFind( id, (CDbEntity**)&dbProfile, DBPROFILE, DBPROFILE );
	
	if ( status.IsOk() )
	{
		io_cmd->setInt( "count", dbProfile->Count() );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Entity: id = %d, index = %d
//
CReturn CCreateProcessApp::ProfileCurve( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbProfile* dbProfile = NULL;

	ID id = 0;
	int indx = -1;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getInt( "index", &indx );

	if ((id < 1) || (indx < 0))
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileEntity()" );
		return status;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Profile.
	status = model.EntityFind( id, (CDbEntity**)&dbProfile, DBPROFILE, DBPROFILE );
	
	if ( status.IsOk() )
	{
		CDbEntity* dbEntity = (*dbProfile)[ indx ];
		if (dbEntity == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileEntity()" );
		}
		else
		{
			io_cmd->setInt( "id", dbEntity->Id() );
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Associate: id = %d, [tol = %d]
//
CReturn CCreateProcessApp::ProfileAssociate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbProfile* dbProfile = NULL;

	ID id = 0;
	double tol = SMALL;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getReal( "tol", &tol );

	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileAssociate()" );
		return status;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Profile.
	status = model.EntityFind( id, (CDbEntity**)&dbProfile, DBPROFILE, DBPROFILE );
	
	if ( status.IsOk() )
	{
		status = dbProfile->Associate( tol );
		if ( status.IsOk() )
			dbProfile->ModifyFlag( true );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Profile:Disassociate: id = %d, [tol = %d]
//
CReturn CCreateProcessApp::ProfileDisassociate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CModel&	model = io_cmd->getModel();
	CDbProfile* dbProfile = NULL;

	ID id = 0;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	io_cmd->getInt( "id", (int*)&id );

	if (id < 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileDisassociate()" );
		return status;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Fetch the Profile.
	status = model.EntityFind( id, (CDbEntity**)&dbProfile, DBPROFILE, DBPROFILE );
	
	if ( status.IsOk() )
	{
		status = dbProfile->Disassociate();
		if ( status.IsOk() )
			dbProfile->ModifyFlag( true );
	}

	return status;
}


void CCreateProcessApp::ProfileMidptsSplit(
	CDbProfile*	dbProfile,
	double		gap,
	bool		reparent )
{
	C3dCoord midpt;

	bool isAssociated = dbProfile->IsAssociated();
	if ( isAssociated )
		dbProfile->Disassociate();

	if ( reparent )
	{
		std::vector<CDbProfile*> stack;
		stack.push_back( dbProfile );

		int start_index = 0;
		CDbProfile* dbTrailingProfile = NULL;
		while ( !stack.empty() )
		{
			CDbProfile* dbLeadProfile = stack.back();
			stack.pop_back();

			// Find a candidate curve to split.
			CDbCurve* dbCurve = MidPointCandidateGet(
				*dbLeadProfile, start_index, gap, &midpt );

			if (dbCurve != NULL)
			{
				// Split it.
				CDbEntity* result = CurveSplit( dbCurve, midpt, gap, reparent );
				dbTrailingProfile = dynamic_cast<CDbProfile*>( result );

				if (dbTrailingProfile != NULL)
					stack.push_back( dbTrailingProfile );

				if (start_index == 0)
				{
					// Because we don't want to split the
					// trailing curve that was just created.
					start_index = 1;
				}
			}
		}
	}
	else
	{
		int indx = 0;

		// A while-loop is used because the number of curves can increase.
		while (indx < dbProfile->Count())
		{
			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbProfile->GetAt( indx ) );
			if ( IsMidPointCandidate( *dbCurve, gap, &midpt ) )
			{
				CurveSplit( dbCurve, midpt, gap, false );
				++indx;  // Skip past the trailing curve that was just created.
			}

			++indx;
		}

		if ( isAssociated )
			dbProfile->Associate( SMALL );

		dbProfile->ModifyFlag( true );
	}
}

// Splitting of profiles at corners is restricted to:
// 1) outside corners
// 2) line/line intersections
//
void CCreateProcessApp::ProfileCornersSplit(
	CDbProfile*	dbProfile,
	double		gap,
	bool		reparent )
{
	CProfile prof;
	CDbWorkplane* dbWork = dbProfile->GetAt(0)->Workplane();

	CConversion::Convert( dbWork, dbProfile, &prof );

	int count = prof.Count();
	if (count < 2)
		return;  // Nothing to do.

	double area = prof.Area();
	if (fabs(area) <= SMALL)
		return;  // Nothing to do.

	bool isAssociated = dbProfile->IsAssociated();
	if ( isAssociated )
		dbProfile->Disassociate();

	CDbEntityList curves;
	dbProfile->Subordinates( &curves );

	CornersSplit( curves, area, gap );

	if ( reparent )
	{
		Repackage( curves );	
	}
	else
	{
		if ( isAssociated )
			dbProfile->Associate( SMALL );
	}

	TagsRemove( curves );

	dbProfile->ModifyFlag( true );
}

CReturn CCreateProcessApp::EntityReverse( CDbEntity* dbEntity )
{

	CReturn	status;

	CDbCurve*	dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
	CDbProfile*	dbProfile = dynamic_cast<CDbProfile*>( dbEntity );

	if (dbCurve != NULL)
	{
		dbCurve->Reverse();
		dbCurve->ModifyFlag( true );
	}
	else if (dbProfile != NULL)
	{
		dbProfile->Reverse();
		dbProfile->ModifyFlag( true );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ProfileReverse(#3)" );
	}

	return status;
}

// TODO: Memory management in the (unlikely) case of failure.
ID CCreateProcessApp::ProfileCurveSplit(
	CDbCurve*		dbCurve,
	const C3dCoord	pt,
	double			gap,
	bool			reparent )
{
	CDbEntity* dbTrailingEntity = CurveSplit( dbCurve, pt, gap, reparent );

	CDbCurve* dbTrailingCurve = dynamic_cast<CDbCurve*>( dbTrailingEntity );
	if (dbTrailingCurve == NULL)
	{
		CDbProfile* dbTrailingProfile = dynamic_cast<CDbProfile*>( dbTrailingEntity );
		if (dbTrailingProfile != NULL)
			dbTrailingCurve = (CDbCurve*) dbTrailingProfile->GetAt( 0 );
	}

	ID id = ((dbTrailingCurve == NULL) ? 0 : dbTrailingCurve->Id());

	return id;
}

// TODO: Memory management in the (unlikely) case of failure.
CDbEntity* CCreateProcessApp::CurveSplit(
	CDbCurve*		dbCurve,
	const C3dCoord	pt,
	double			gap,
	bool			reparent )
{
	CDbEntity* result = NULL;

	CDbCurve* trail = dbCurve->Split( pt, gap );
	if (trail != NULL)
	{
		result = trail;

		dbCurve->ModifyFlag( true );

		CDbSequence::ConditionalSequence( dbCurve, trail, TRUE );

		if ( reparent )
		{
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( trail->Owner() );
			if (dbProfile != NULL)
			{
				CEntityDb* db = dbCurve->Db();

				int indx = dbProfile->Position( trail );

				CDbProfile* dbTrailingProfile;
				CReturn status = db->Create( DBPROFILE, (CDbEntity**) &dbTrailingProfile );
				if ( status.IsOk() )
				{
					result = dbTrailingProfile;

					int count = dbProfile->Count();
					for (int jndx = indx; jndx < count; ++jndx)
					{
						// We keep pulling from the same curve index because
						// the call to Append() transfers curve ownership.
						CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbProfile->GetAt( indx ) );
						dbTrailingProfile->Append( dbCurve );
					}

					CDbFeature* dbOwner = dynamic_cast<CDbFeature*>( dbProfile->Owner() );
					if (dbOwner != NULL)
						dbOwner->InsertAfter( dbProfile, dbTrailingProfile );
				}
			}
		}
	}

	return result;
}

CDbCurve* CCreateProcessApp::MidPointCandidateGet(
	const CDbProfile& dbProfile, int start_index, double gap, C3dCoord* midpt )
{
	CDbCurve* candidate = NULL;

	int count = dbProfile.Count();
	for (int indx = start_index; indx < count; ++indx)
	{
		CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbProfile.GetAt( indx ) );
		if ( IsMidPointCandidate( *dbCurve, gap, midpt ) )
		{
			candidate = dbCurve;
			break;
		}
	}

	return candidate;
}

bool CCreateProcessApp::IsMidPointCandidate(
	const CDbCurve& dbCurve, double gap, C3dCoord* midpt )
{
	bool is_candidate = false;

	CGeoCurve* geoCurve = dbCurve.Curve();
	if (geoCurve != NULL)
	{
		// Avoid splitting entities that are shorter than the gap.
		double length = geoCurve->Length2d();

		is_candidate = (gap < length);
		if ( is_candidate )
			(*midpt) = geoCurve->MidPt();

		delete geoCurve;
	}

	return is_candidate;
}

// The list is not modified but its contained curves are.
void CCreateProcessApp::CornersSplit(
	const CDbEntityList& curves, double area, double gap )
{
	int count = curves.Count();

	int tag = 1;

	gap *= 0.5;  // Because each curve end is shortened by half the distance.

	for (int indx = 0; indx < count; ++indx)
	{
		// Must be sure to handle wrap condition at start/end of profile.
		int jndx = ((indx == count-1) ? 0 : indx+1);

		CDbCurve* dbCurveA = dynamic_cast<CDbCurve*>( curves.GetAt( indx ) );
		CDbCurve* dbCurveB = dynamic_cast<CDbCurve*>( curves.GetAt( jndx ) );

		// Tag these two curves as adjacent and sharing a common end point.
		dbCurveA->IntSet( TAG, tag );
		dbCurveB->IntSet( TAG, tag );

		CDbLine* dbLineA = dynamic_cast<CDbLine*>( dbCurveA );
		CDbLine* dbLineB = dynamic_cast<CDbLine*>( dbCurveB );

		if ((dbLineA != NULL) && (dbLineB != NULL))
		{
			CGeoLine* geoLineA = dynamic_cast<CGeoLine*>( dbLineA->Curve() );
			CGeoLine* geoLineB = dynamic_cast<CGeoLine*>( dbLineB->Curve() );

			if ((geoLineA != NULL) && (geoLineB != NULL))
			{
				C3dCoord peA = geoLineA->EndPt();
				C3dCoord psB = geoLineB->StartPt();

				if ( peA.WithinTol( psB, SMALL ) )
				{
					C2dUnitVec tanA = geoLineA->EndTan() + PI;
					C2dUnitVec tanB = geoLineB->StartTan();

					double cross = (tanA ^ tanB) * SGN( area );
					if (cross < SMALL)
					{
						double dot = tanA * tanB;
						double theta = 0.5 * acos( dot );

						double dist = gap / sin( theta );

						peA += (tanA * dist);
						psB += (tanB * dist);

						C3dCoord psA = geoLineA->StartPt();
						C3dCoord peB = geoLineB->EndPt();

						dbLineA->Init( psA, peA );
						dbLineB->Init( psB, peB );

						// Tag the trailing line has having a different end point.
						++tag;
						dbLineB->IntSet( TAG, tag );
					}
				}
			}

			// Alas, we thrash memory via the construction/destruction cycle
			// but the alternative is to manage some type cache.
			delete geoLineA;
			delete geoLineB;
		}
	}
}

// NOTE: There might be some issues at start/end profile boundary condition.
void CCreateProcessApp::Repackage( const CDbEntityList& curves )
{
	int count = curves.Count();
	if (count > 1)
	{
		CDbProfile* dbProfile = NULL;

		CDbCurve* dbCurveA = (CDbCurve*) curves.GetAt(0);

		CDbProfile* dbOriginal = dynamic_cast<CDbProfile*>( dbCurveA->Owner() );
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( dbOriginal->Owner() );

		// Used to maintain profile ordering within 'dbFeature'.
		CDbProfile* dbReferenceProfile = dbOriginal;

		CEntityDb* db = dbCurveA->Db();

		// The returned tag had better be greater-than-zero!
		// Perhaps we should assert?
		//
		// ASSUMPTION: Curves have "tag == 1" are already
		// owned by the original profile.
		int tagA = dbCurveA->IntGet( TAG, 0 );

		for (int indx = 1; indx < count; ++indx)
		{
			CDbCurve* dbCurveB = (CDbCurve*) curves.GetAt(indx);

			int tagB = dbCurveB->IntGet( TAG, 0 );
			if ((tagB != tagA) && (tagB != 1))
			{
				db->Create( DBPROFILE, (CDbEntity**) &dbProfile );
				if ((dbProfile != NULL) && (dbFeature != NULL))
				{
					dbFeature->InsertAfter( dbReferenceProfile, dbProfile );
					dbReferenceProfile = dbProfile;
				}
			}

			if (dbProfile != NULL)
			{
				// This call "reparents" the curve, removing it from
				// its original profile an giving it to this profile.
				dbProfile->Append( dbCurveB );
			}
		}
	}
}

void CCreateProcessApp::TagsRemove( const CDbEntityList& curves )
{
	int count = curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		curves.GetAt( indx )->AttribDelete( TAG );
	}
}
