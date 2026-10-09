
#include "stdafx.h"

#include "Return.h"
#include "cmn_resource.h"
#include "StringConst.h"

#include "Path.h"
#include "3dBox.h"
#include "DbEntity.h"
#include "DbTool.h"
#include "DbIterator.h"
#include "Model.h"
#include "MM2.h"
#include "ModelUtil.h"
#include "NestMgr.h"
#include "NestProcess.h"


// Nest:RemnantCommit: nst=%s, rem=%s, rem_id=%d
// Returns: (%d) parent_id 
//   where:
//   nst  -- is the input file
//   rem  -- is the output file
//   rem_id -- the _rem_id to write to the model header
//   parent_id -- the previous _rem_id in the model header
//
CReturn 
CNestProcessApp::RemnantCommit( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CReturn	status;

	int parent_id = 0;  // assume failure.

	CString nst = io_cmd->VarList().getString( "nst", "" );
	CString rem = io_cmd->VarList().getString( "rem", "" );

	int rem_id = io_cmd->VarList().getInt( "rem_id", 0 );
	bool flip = (io_cmd->VarList().getInt( "flip", FALSE ) != FALSE);
	
	if (nst.IsEmpty() || rem.IsEmpty() || (rem_id < 1))
	{
		status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::RemnantCommit(#1)" );
	}
	else
	{
		CSelectorStack		selectorStack;
		CTreeViewSupport	treeViewSupport;	// necessary, but not used

		// CPath		path;
		CModel		model;
		C3dBox		box;
		CMM2		mm2;
		CDbTool*	dbRemnant;
		int			flags;

		model.Init( selectorStack, treeViewSupport );
		model.UndoBufferSuppress();

		flags = (MM2_STD_MODE | MM2_SKIP_SEQ_OBJS);

		status = mm2.Read( nst, &model, flags );
		if (status.IsOk())
		{
			model.UndoBufferSuppress();

			model.EntityFind( "Remnant", (CDbEntity**) &dbRemnant, DBTOOL, DBTOOL );
			if (dbRemnant == NULL)
			{
				status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::RemnantCommit(#3)" );
			}
			else
			{
				// Delete everything except the geometry
				// that resides in the "Remnant" layer.
				BucketDelete( &model, dbRemnant->Id(), DBLINE );
				BucketDelete( &model, dbRemnant->Id(), DBARC );
				BucketDelete( &model, dbRemnant->Id(), DBHOLE );
				BucketDelete( &model, 0, DBCOMMAND );
				BucketDelete( &model, 0, DBPATTERN );
				BucketDelete( &model, 0, DBSEQUENCE );

				status = CModelUtil::EmptyContainers( model, true );

				// path.Set(nst);
				// if (path.Ext().CompareNoCase("NST") == 0)
				if ( flip )
				{
					CNestMgr nest_mgr;
					nest_mgr.remnant_flip( &model );
				}

				// Note, when parent_id is greater-than-zero,
				// we are processing a remnant of a remnant.
				parent_id = model.Header().getInt( "_rem_id", 0 );
				model.pHeader()->setInt( "_rem_id", rem_id );

				// We want to be sure to ignore the stock layer.
#if REQUIRED
				box = model.BoxUser();
				model.pHeader()->setReal( "_rem_length", box.Dx() );
				model.pHeader()->setReal( "_rem_width", box.Dy() );
				model.pHeader()->setReal( "_rem_area", RemnantArea( model ) );
				model.pHeader()->setInt( "_rem_qty", 1 );
#endif

				status = mm2.Write( rem, model );

				model.Flush();
			}
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::RemnantCommit(#2)" );
		}
	}

	io_cmd->setInt( "parent_id", parent_id );

	return status;
}

// Nest:RemnantStats:
// Returns:
//   (%f) rem_length
//   (%f) rem_width
//   (%f) rem_area
//
CReturn 
CNestProcessApp::RemnantStats( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CReturn	status;
	C3dBox	box;
	double	area;

	CModel& model = io_cmd->getModel();

	status = RemnantStatistics( model, &box, &area );

	io_cmd->setReal( "rem_length", box.Dx() );
	io_cmd->setReal( "rem_width", box.Dy() );
	io_cmd->setReal( "rem_area", area );
	io_cmd->setReal( "rem_thickness", model.Header().getReal( "Thickness", 0. ) );

	return status;
}

void
CNestProcessApp::BucketDelete(
					CModel*			model,
					ID				remnant_layer_id,
					EDbEntityType	type )
{
	CDbIterator	iter;
	CDbEntity*	dbEntity;
	CDbTool*	dbTool;

	iter.Init( model->Db(), type );
	while (1)
	{
		dbEntity = iter();
		if (dbEntity == NULL)
			break;

		if (dbEntity->Type() > type)
			break;

		if (remnant_layer_id > 0)
		{
			// NOTE: It is the clients responsibility to ensure that
			// remnant_layer_id is non-zero only for [DBLINE..DBHOLE].
			dbTool = dbEntity->Tool();
			if ((dbTool == NULL) || (dbTool->Id() != remnant_layer_id))
				dbEntity->Delete();
		}
		else
		{
			dbEntity->Delete();
		}

		iter.Next();
	}
}

CReturn
CNestProcessApp::RemnantStatistics(
	const CModel&	model,
	C3dBox*			box,
	double*			area )
{
	CReturn		status;
	CDbIterator	iter;
	CDbTool*	dbRemnant;
	CDbProfile*	dbProfile;

	(*area) = 0.;  // assume failure

	// ASSUMPTION: RemnantArea() is not called unless this layer exists.
	model.EntityFind( "Remnant", (CDbEntity**) &dbRemnant, DBTOOL, DBTOOL );
	if (dbRemnant != NULL)
	{
		iter.Init( model.Db(), DBPROFILE );
		while (1)
		{
			dbProfile = dynamic_cast<CDbProfile*>( iter() );
			if (dbProfile == NULL)
				break;

			if (dbProfile->Tool() == dbRemnant)
				break;

			iter.Next();
		}

		if (dbProfile != NULL)
		{
			CProfile	profile;
			CDbCurve*	dbCurve;
			int			count, indx;

			count = dbProfile->Count();
			for (indx = 0; indx < count; ++indx)
			{
				dbCurve = dynamic_cast<CDbCurve*>( (*dbProfile)[indx] );
				if (dbCurve != NULL)
				{
					profile.Append( dbCurve->Curve() );
				}
			}

			(*box) = profile.Box3d();
			(*area) = fabs( profile.Area() );
		}
	}
	else
	{
		// So that unusual numbers don't appear in the VB dialog.
		box->Update( 0, 0, 0, 0, 0, 0 );

		status.Internal( IDS_INTERNAL_ERROR, "CNestProcessApp::RemnantStatistics(#1)" );
	}

	return status;
}
