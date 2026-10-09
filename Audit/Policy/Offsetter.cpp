
#include "stdafx.h"
#include "StringConst.h"

#include "GeoLine.h"
#include "GeoArc.h"
#include "Profile.h"
#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbFeature.h"
#include "Conversion.h"
#include "Worm.h"

#include "ChArc.h"
#include "ChPart.h"
#include "ChTool.h"
#include "ConvexHull.h"
#include "Shape.h"
#include "AutoIndex.h"
#include "ModelUtil.h"
#include "Offsetter.h"



////////////////////////////////////////////////////////////////////////

// See also NOTE: (below)
CReturn 
COffsetter::UniformOffset(
						CModel* model,
						CDbWorkplane* dbWork,
						CDbTool* dbTool,
						CDbEntity* dbEntity,
						int dir,
						double delta,
						double sharpAngle,
						double zlevel,
						CDbFeature* dbFeature ) 
{
	CProfile srcProf;
	CProfileList profList;
	CString command;
	CString toolStr;

	ID id = dbEntity->Id();

	CReturn status = CConversion::Convert( dbWork, dbEntity, &srcProf );

	// Generate the raw cut data.
	if ( status.IsOk() )
	{
		double dist = 0.0;

		if (dir != 0)
			dir = ((dir > 0) ? LEFT : RIGHT);

		if (dbTool != NULL && !dbTool->IsLayer())
		{
			// See also NOTE: (below)
			int codePartProf = model->Header().getInt( STR_PARTPROF, 0 );

			model->pDefault()->setInt( STR_CUTSIDE, dir );

			if ( !codePartProf || dbTool->IsPunchTool() )
				dist = dbTool->EffectiveDiameter() / 2;
		}

		dist += delta;

		// Correct the offset direction for point-to-point coordinate systems.
		int correctedDir = dir * dbWork->ToolUp();
		status = CWorm::ProfOffset(
			srcProf, correctedDir, dist, sharpAngle*DEG2RAD, FALSE, &profList );
	}

	if ( status.IsOk() )
	{
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Stuff the results into the toolpath's feature.
		//
		// NOTE: The resulting database entities inherit all of their
		// attributes from the model defaults.  Therefore, it is very
		// important to set the models' default attributes prior to
		// calling COffsetter::UniformOffset()
		//
		dbFeature->DestructiveFlush();
		status = CConversion::Convert( profList, zlevel, dbTool, dbWork, dbFeature );
		if ( !status.IsOk() )
			return status;

		dbFeature->RefsFlush();
		dbFeature->AddRef( dbEntity );

		if (dbTool != NULL)
		{
			CDbFeature* parent = CModelUtil::FirstFeature( dbEntity );
			if (parent != NULL)
				parent->Append( dbFeature, FALSE );

			// When we create toolpath, we want to associate the toolpath
			// to its reference geometry and the parameters that were used
			// in its creation.  This way, when the reference geometry or
			// parameters change, we can regenerate the toolpath.

			toolStr.Format( "toolid=%d,", dbTool->Id() );

			// BugID 640 -- added 'explode' for correct regeneration.
			command.Format( "function = \"Create:OffsetProfile:\", " \
							"id=%d,%sprofid=%d,dir=%d,dist=%f,explode=0,sharp=%f,zlevel=%f",
							dbFeature->Id(), toolStr, id, dir, delta, sharpAngle, zlevel );

			status = dbFeature->setMulti( command );

			model->pDefault()->deleteVar( STR_CUTSIDE );
		}
	}

	profList.DestructiveFlush();

	return status;
}

// Toolpath:ConvexHullOffset: profid=%d, toolid=%d, dir=%d
CReturn 
COffsetter::ConvexHullOffset(
						CModel* model,
						ID* featid,
						ID profid,
						ID toolid,
						int dir )
{
	CReturn	status;

	if (profid == 0 || toolid == 0 || abs(dir) != 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAutoMod::ConvexHullOffset()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CProfile partProf;
	CGeoCurveArray toolGeo;

	CDbEntity* dbEntity = NULL;
	CDbProfile* dbProfile = NULL;
	CDbTool* dbTool = NULL;

	CDbWorkplane* dbWork = model->ActiveWorkplane();

	status = model->EntityFind( profid, &dbEntity, DBLINE, DBPROFILE );

	if ( status.IsOk() )
		status = CConversion::Convert( dbWork, dbEntity, &partProf );

	if ( status.IsOk() )
		status = model->EntityFind( toolid, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::ConvexHullOffset()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CChPart chPart;
	CChTool chTool;
	CProfileList offsetProfs;
	CDbFeature* dbFeature = NULL;

	// NOTE: The offset direction is set here as zero because it
	// is the clients responsibility to ensure the profiles has
	// the correct winding direction for the use-case at hand.
	status = chPart.Init( partProf, 0 );

	if ( status.IsOk() )
	{
		dbTool->Convert( &toolGeo );
		status = chTool.Init( toolGeo );
	}

	if ( status.IsOk() )
		status = CConvexHull::Offset( chPart.ArcList(), chTool, dir, &offsetProfs );

	if ( status.IsOk() )
	{
		// status = model->EntityCreate( DBFEATURE, (CDbEntity**) &dbFeature );
		status = CModelUtil::FeatureFindCreate(
			&(model->Db()), (*featid), &dbFeature );

		dbFeature->DestructiveFlush();
	}

	if ( status.IsOk() )
	{
		status = CConversion::Convert( offsetProfs, 0.0, dbTool, dbWork, dbFeature );
	}

	offsetProfs.DestructiveFlush();

	if ( status.IsOk() )
	{
		CString command;
		CDbFeature* parent;

		dbFeature->RefsFlush();
		dbFeature->AddRef( dbEntity );

		parent = CModelUtil::FirstFeature( dbEntity );

		if (parent != NULL)
			parent->Append( dbFeature, FALSE );

		command.Format( "function = \"Toolpath:ConvexHullOffset:\", " \
						"id=%d,profid=%d,toolid=%d,dir=%d",
							dbFeature->Id(), profid, toolid, dir );

		dbFeature->setMulti( command );

		(*featid) = dbFeature->Id();
	}
	else
	{
		(*featid) = 0;

		status.Internal( IDS_INTERNAL_ERROR, "CAutoMod::ConvexHullOffset()" );
	}

	return status;
}

// Standard usage.
//     Toolpath:AutoIndexOffset: profid=%d, toolid=%d, dir=%d. longside=%d
// Regen usage:
//     Toolpath:AutoIndexOffset: id=%d, profid=%d, toolid=%d, dir=%d, longside=%d
CReturn 
COffsetter::AutoIndexOffset(
						CModel* model,
						ID* featid,
						ID profid,
						ID toolid,
						int dir,
						bool use_long_side )
{
	CReturn	status;

	if (profid == 0 || toolid == 0 || abs(dir) != 1)
	{
		status.Internal( IDS_INTERNAL_ERROR, "COffsetter::AutoIndexOffset()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CProfile partProf;
	CGeoCurveArray toolGeo;

	CDbEntity* dbEntity = NULL;
	CDbTool* dbTool = NULL;

	CDbTool* dbLayer = model->ActiveTool();
	CDbWorkplane* dbWork = model->ActiveWorkplane();

	status = model->EntityFind( profid, &dbEntity, DBLINE, DBPROFILE );

	if ( status.IsOk() )
	{
		status = CConversion::Convert( dbWork, dbEntity, &partProf );
		if (status.IsOk() && partProf.Count() == 1)
		{
			CGeoArc* geoArc = dynamic_cast<CGeoArc*>( partProf.GetAt(0) );
			if (geoArc != NULL && (geoArc->IncludedAngle() >= (TWOPI - SMALL)))
			{
				// Split the arc (or else it will be culled).

				CGeoArc* copy = dynamic_cast<CGeoArc*>( geoArc->Clone( false ) );
				if (copy != NULL)
				{
					C3dCoord midpt = geoArc->MidPt();

					geoArc->EndPt( midpt );
					copy->StartPt( midpt );
					partProf.Append( copy );
				}
			}
		}
	}

	if ( status.IsOk() )
		status = model->EntityFind( toolid, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "COffsetter::AutoIndexOffset()" );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CGeoElemList result;

	CAutoIndex autoIndex;
	CShape part;
	CShape tool;
	CDbFeature* dbFeature = NULL;

	dbTool->Convert( &toolGeo );

	part.FullNormalization( FALSE );

	part.Init( partProf.Curves(), TRUE );
	tool.Init( toolGeo, FALSE );

	tool.IntSet( STR_STATION_ID, dbTool->Attrib().getInt( STR_STATION_ID, 0 ) );

	status = autoIndex.Offset( part, tool, dir, use_long_side, &result );

	if ( status.IsOk() )
		status = CModelUtil::FeatureFindCreate(	&(model->Db()), (*featid), &dbFeature );

	if ( status.IsOk() )
	{
		dbFeature->DestructiveFlush();
		status = CModelUtil::PunchedFeatureCreate( result, model, dbFeature );
	}

	result.DestructiveFlush();

	if ( status.IsOk() )
	{
		CString command;
		CDbFeature* parent;

		dbFeature->RefsFlush();
		dbFeature->AddRef( dbEntity );

		parent = CModelUtil::FirstFeature( dbEntity );
		if (parent != NULL)
			parent->Append( dbFeature, FALSE );

		command.Format( "function = \"Toolpath:AutoIndexOffset:\"," \
						"id=%d,profid=%d,toolid=%d,dir=%d,longside=%d",
						dbFeature->Id(), profid, toolid, dir, (use_long_side ? 1 : 0) );

		status = dbFeature->setMulti( command );
		if (featid != NULL)
			(*featid) = dbFeature->Id();
	}
	else
	{
		if (featid != NULL)
			(*featid) = 0;

		status.Internal( IDS_INTERNAL_ERROR, "COffsetter::AutoIndexOffset()" );
	}

	return status;
}


////////////////////////////////////////////////////////////////////////
//	ProfOffset
//
//	Given a CDbProfile, offset it into a CProfileList
//	New!  Improved!  Selects between CWorm (uniform), CConvexHull (non-round fixed),
//	and CAutoIndex (non-round autoindex) offsets, and converts data as needed.
//	
//	BUT -- does not do anything to create actual part geometry.
CReturn
COffsetter::ProfOffset(
	CProfile*		src_profile,
	int				off_dir,
	CDbTool*		dst_tool,
	CProfileList*	dst_proflist )
{
	CReturn ret;

	if (dst_tool->IsRoundTool())
	{
		// CWorm::Offset -- Uniform Offset
		ret += CWorm::ProfOffset( *src_profile, 
									off_dir, 
									dst_tool->EffectiveDiameter()/2.0, 
									TWOPI,
									TRUE,
									dst_proflist );
	}
	else
	{
		// sick-ass punch tool
		CGeoCurveArray toolcurves;
		dst_tool->Convert( &toolcurves );

		if (dst_tool->IsIndexable())
		{
			// CAutoIndex::Offset -- rotating toolshape
			// NOTE:  AutoIndex offset creates *fragmented* profiles
			// TODO:  Find a way to do Uniform Offset in this case?
			//
			CShape prof_shape;
			prof_shape.Init( src_profile->Curves(), TRUE );

			CShape tool_shape;
			tool_shape.Init( toolcurves, FALSE );
			tool_shape.IntSet( STR_STATION_ID, dst_tool->Attrib().getInt( STR_STATION_ID, 0 ) );

			CGeoElemList dst_geo;
			CAutoIndex	aidx;
			ret += aidx.Offset( prof_shape, tool_shape, off_dir, TRUE, &dst_geo );

			CProfile* profile = new CProfile;

			CGeoCurve* geo_curve;
			int num = dst_geo.Count();
			for (int idx=0; idx<num; idx++)
			{
				geo_curve = dynamic_cast<CGeoCurve*>(dst_geo[idx]);
				if (geo_curve)
					profile->Append( geo_curve );
			}

			dst_proflist->Append( profile );
		}
		else
		{
			// CConvexHull::Offset -- fixed toolshape
			CChPart chPart;
			CChTool chTool;

			ret = chPart.Init( (*src_profile), off_dir );
			if (ret.IsOk())
			{
				chTool.Init( toolcurves );

				CConvexHull::Offset( chPart.ArcList(), chTool, off_dir, dst_proflist );
			}
		}		
		toolcurves.DestructiveFlush();
	}

	return ret;
}
