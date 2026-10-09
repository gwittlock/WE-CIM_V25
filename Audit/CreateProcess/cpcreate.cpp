
#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"

#include "GeoPoly.h"

#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbCurve.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbIterator.h"

#include "Profile.h"
#include "Worm.h"
#include "Conversion.h"
#include "Offsetter.h"

#include "ModelUtil.h"

#include "ViewMgr.h"

#include "CreateProcess.h"


static void OffsetSelectionsGet( const CSelector& selector, CDbEntityList* entities );


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create:OffsetCurve: id = %d, dir = %d, dist = %f
// Returns id = %d of the offset curve
//
CReturn 
CCreateProcessApp::OffsetCurve( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	ID id;
	int dir;
	double dist;

	status += io_cmd->getInt( "id", (int*) &id );
	status += io_cmd->getInt( "dir", &dir );
	status += io_cmd->getReal( "dist", &dist );

	if ( status.IsOk() )
	{
		CDbCurve* dbCurve;
		status += model.EntityFind( id, (CDbEntity**) &dbCurve, DBLINE, DBARC );

		if ( status.IsOk() )
		{
			CDbTool* dbTool = model.ActiveTool();
			CDbWorkplane* dbWork = dbCurve->Workplane();

			CGeoCurve* geoCurve = dbCurve->Curve();

			CGeoCurve* offsetCurve = geoCurve->Offset( dir, dist );
			if (offsetCurve != NULL)
			{
				CGeoLine* geoLine = dynamic_cast<CGeoLine*>( offsetCurve );
				CGeoArc* geoArc = dynamic_cast<CGeoArc*>( offsetCurve );

				if (geoLine != NULL)
				{
					CDbLine* dbLine;
					status += model.EntityCreate( DBLINE, (CDbEntity**) &dbLine );

					if ( status.IsOk() )
					{
						dbLine->Init( dbTool, dbWork, (*geoLine) );

						if (model.ActivePattern() != NULL)
							model.ActivePattern()->Append( dbLine );

						io_cmd->setInt( "id", dbLine->Id() );
					}
				}
				else if (geoArc != NULL)
				{
					CDbArc* dbArc;
					status += model.EntityCreate( DBARC, (CDbEntity**) &dbArc );

					if ( status.IsOk() )
					{
						dbArc->Init( dbTool, dbWork, (*geoArc) );

						if (model.ActivePattern() != NULL)
							model.ActivePattern()->Append( dbArc );

						io_cmd->setInt( "id", dbArc->Id() );
					}
				}
			}

			delete offsetCurve;
			delete geoCurve;
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::CreateOffset()" );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create:OffsetProfile: [toolid=%d,] profid=%d, (dir=%d | climb=%d),
//                       dist=%f, sharp=%f, zlevel=%f, [explode=%d]
// Returns id=%d of the feature containing the offset curves.
//
// NOTE:
//
// Prior to V15, the UI required you select a profile/curve entity.  As such,
// the portal command required a legitimate 'profid'.  In V15, the portal
// command was changed (and the UI accordingly) such that it works with the
// selection set.  A 'profid' of zero now indicates that we are using entities
// in the selection set.  However, we must still support valid 'profid' for
// the case where Java is offsetting an entity.
//
CReturn 
CCreateProcessApp::OffsetProfile( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	// int toolColor = 0;
	int partprof = 0;

	ID id = 0;
	ID profId = 0;
	ID toolId = 0;

	int dir, climb;
	double delta = UNDEFINED;
	double zlevel = UNDEFINED;
	double sharpAngle = UNDEFINED;
	bool explode;
	int count, indx;

	CDbEntityList refEntities;
	CDbEntity* refEntity = NULL;
	CDbTool* dbTool = NULL;
	CDbFeature* dbFeature = NULL;

	CModel& model = io_cmd->getModel();
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	CDbWorkplane* dbWork = model.ActiveWorkplane();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Get the command parameters
	profId = io_cmd->VarList().getInt( "profid", 0 );
	status += io_cmd->getReal( "dist", &delta );
	status += io_cmd->getReal( "sharp", &sharpAngle );
	status += io_cmd->getReal( "zlevel", &zlevel );

	id = io_cmd->VarList().getInt( "id", 0 );
	toolId = io_cmd->VarList().getInt( "toolid", 0 );

	dir = io_cmd->VarList().getInt( "dir", IUNDEFINED );
	climb = io_cmd->VarList().getInt( "climb", IUNDEFINED );

	explode = (io_cmd->VarList().getInt( "explode", TRUE ) != FALSE);

	if ( !status.isOkay() || (dir == IUNDEFINED && climb == IUNDEFINED) )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::OffsetProfile()" );
		return status;
	}

	if (toolId > 0)
	{
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Set the default color to that of the tool.  This is especially
		// important when we're regenerating existing toolpath.  If we
		// don't do this, the toolpath will be the wrong color.
		status = model.EntityFind( toolId, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if ( !status.IsOk() )
			return status;

		model.Header().getInt( STR_PARTPROF, &partprof );
	}

	if (profId == 0)
	{
		OffsetSelectionsGet( selector, &refEntities );
	}
	else
	{
		model.EntityFind( profId, &refEntity, DBLINE, DBPROFILE );
		refEntities.Append( refEntity );
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Offset the entities.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = refEntities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		refEntity = refEntities[indx];

		if ( status.IsOk() )
			status = CModelUtil::FeatureFindCreate( &(model.Db()), id, &dbFeature );

		if ( status.IsOk() )
		{
			CVarList defMemo = model.Default();

			if (dbFeature->Count() > 0)
			{
				// ASSUMPTION: We are regenerating an existing feature.
				//
				// Set the model's default attributes using the current
				// attributes of the feature that is being regenerated
				// and judiciously over-ride select attributes.  This
				// way, all newly created entities will inherit an
				// appropriate set of attributes from the model defaults.
				//
				CDbEntity* firstEntity = (*dbFeature)[0];

				if ( !partprof )
					firstEntity->AttribDelete( STR_PARTPROF );

				(*(model.pDefault())) = firstEntity->Attrib();
			}

			if ( partprof )
			{
				if (dbTool != NULL && !dbTool->IsPunchTool())
					model.pDefault()->setInt( STR_PARTPROF , 1 );
			}

			if (climb != IUNDEFINED)
			{
				status = ProfileGenerate2( &model, dbWork, dbTool, refEntity,
					((climb != 0) ? TRUE : FALSE), delta, sharpAngle, zlevel, dbFeature );
			}
			else
			{
				status = ProfileGenerate( &model, dbWork, dbTool, refEntity,
							dir, delta, sharpAngle, zlevel, dbFeature );
			}

			if (model.ActivePattern() != NULL && dbFeature->Owner() == NULL)
			{
				model.ActivePattern()->Append( dbFeature );
			}

			(*(model.pDefault())) = defMemo;
		}

		// if (!dbTool && explode)
		if ( explode )
		{
			int fnum = dbFeature->Count()-1;
			for (int fcnt=fnum; fcnt>=0; fcnt--)
			{
				// 2006.10.07 (PE) -- Dunno why were doing this. I do know, however,
				// that if you tried to offset a rectangular profile, you would get
				// 4 unrelated lines (ie. not in a profile). This behavior just seems
				// plain wrong and undesirable. If you want the four lines (but
				// trimmed) offset the profile and then unchain the result.
				//    CDbContainer* db_cont = dynamic_cast<CDbContainer*>((*dbFeature)[fcnt]);
				CDbFeature* db_cont = dynamic_cast<CDbFeature*>((*dbFeature)[fcnt]);
				if (db_cont)
					CModelUtil::Explode( db_cont, FALSE );
			}
			CModelUtil::Explode( dbFeature, TRUE );
		}

		if ( status.IsOk() )
		{
			io_cmd->setInt( "count", indx );
			if (indx == 0)
			{
				io_cmd->setInt( "id", dbFeature->Id() );
			}
			else
			{
				CString attribName;
				attribName.Format( "id", indx );
				io_cmd->setInt( attribName, dbFeature->Id() );
			}
		}
		else
		{
			break;
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
CReturn 
CCreateProcessApp::ProfileGenerate(
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
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
				
	CReturn status;

	if ( (dbTool != NULL) &&
		 dbTool->IsPunchTool() &&
		 !dbTool->IsRoundTool() )
	{
		ID featid = dbFeature->Id();

		if ( dbTool->IsIndexable() )
		{
			status = COffsetter::AutoIndexOffset(
						model, &featid, dbEntity->Id(), dbTool->Id(), dir, true );
		}
		else
		{
			status = COffsetter::ConvexHullOffset(
						model, &featid, dbEntity->Id(), dbTool->Id(), dir );
		}
	}
	else
	{
		status = COffsetter::UniformOffset(
						model, dbWork, dbTool, dbEntity, dir, delta,
						sharpAngle, zlevel, dbFeature );
	}

	return status;
}
		
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
CReturn 
CCreateProcessApp::ProfileGenerate2(
						CModel* model,
						CDbWorkplane* dbWork,
						CDbTool* dbTool,
						CDbEntity* dbEntity,
						bool climbCut,
						double delta,
						double sharpAngle,
						double zlevel,
						CDbFeature* dbFeature ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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

		int dir = (( climbCut ) ? LEFT : RIGHT);

		if (dbTool != NULL)
		{
			// See also COffsetter::UniformOffset()
			int codePartProf = 0;
			model->Default().getInt( STR_PARTPROF, &codePartProf );

			model->pDefault()->setInt( STR_CUTSIDE, dir );

			if ( !codePartProf )
				dist = dbTool->EffectiveDiameter();
		}

		dist = (dist / 2) + delta;

		if ( srcProf.IsClosed( CLOSED_ENOUGH ) )
		{
			double profDir = srcProf.Area();
			if (profDir < 0)
				srcProf.Reverse();
		}

		// Correct the offset direction for point-to-point coordinate systems.
		int correctedDir = dir * dbWork->ToolUp();
		status = CWorm::ProfOffset(
			srcProf, correctedDir, dist, sharpAngle*DEG2RAD, FALSE, &profList );
	}

	if ( status.IsOk() )
	{
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Stuff the results into the feature.

		dbFeature->DestructiveFlush();
		status = CConversion::Convert( profList, zlevel, dbTool, dbWork, dbFeature );

		if ( !status.IsOk() )
			return status;

		dbFeature->RefsFlush();
		dbFeature->AddRef( dbEntity );

		if (dbTool != NULL)
		{
			// When we create toolpath, we want to associate the toolpath
			// to its reference geometry and the parameters that were used
			// in its creation.  This way, when the reference geometry or
			// parameters change, we can regenerate the toolpath.

			toolStr.Format( "toolid=%d,", dbTool->Id() );

			command.Format( "function = \"Create:OffsetProfile:\", " \
							"id=%d,%sprofid=%d,climb=%d,dist=%f,sharp=%f,zlevel=%f",
								dbFeature->Id(), toolStr, id,
								(climbCut?RIGHT:LEFT), delta, sharpAngle, zlevel );

			status = dbFeature->setMulti( command );

			model->pDefault()->deleteVar( STR_CUTSIDE );
		}
	}

	profList.DestructiveFlush();

	return status;
}

void
OffsetSelectionsGet( const CSelector& selector, CDbEntityList* entities )
{
	CDbEntity* dbEntity;
	CDbEntity* owner;
	EDbEntityType type;
	int count, indx;

	count = selector.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = selector[indx];
		type = dbEntity->Type();

		if (type == DBPROFILE)
		{
			entities->Append( dbEntity );
		}
		else if (type == DBLINE || type == DBARC)
		{
			owner = dbEntity->Owner();
			if (owner == NULL || !owner->IsSelected())
			{
				entities->Append( dbEntity );
			}
		}
	}
}



// ==================================================================
//	Boolean
//
//	Assumes six layers in the file, two source and four destination.
//	Takes the first profile from layer "poly0" and "poly1" and 
//	combines them using the various boolean poly operations, with
//	destinations to "and", "or", "minus", and "xor".
//
// Create:Boolean: p0=%d, p2=%d, op=%d
//		where op=	1, intersection
//					2, union
//					3, difference
//					4, xor
//
//	This is a test method, and not meant for general use
//
CReturn CCreateProcessApp::Boolean( CCommand* io_cmd )
{
	CReturn ret;
	//
	// Get the command parameters
	//
	ID prof0;
	ID prof1;
	int op;

	ret += io_cmd->getInt( "p0", (int*) &prof0 );
	ret += io_cmd->getInt( "p1", (int*) &prof1 );
	ret += io_cmd->getInt( "op", &op );
	if ( !ret.IsOk() )
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Boolean() parameters" );
		return ret;
	}
	//
	// Get the profiles themselves
	//
	CModel&	model = io_cmd->getModel();

	CDbProfile* db_prof0 = NULL;
	model.EntityFind( prof0, (CDbEntity**)&db_prof0, DBPROFILE, DBPROFILE );

	CDbProfile* db_prof1 = NULL;
	model.EntityFind( prof1, (CDbEntity**)&db_prof1, DBPROFILE, DBPROFILE );

	if ( !db_prof0
		|| !db_prof1 )
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Boolean() missing profile" );
		return ret;
	}
	//
	// Convert the profiles to GeoPoly form
	//
	CGeoPoly geo0;
	int idx, num = db_prof0->Count();
	for (idx=0; idx<num; idx++)
	{
		CDbCurve* db_curve = (CDbCurve*) db_prof0->GetAt( idx );
		CGeoCurve* curve = db_curve->Curve();
		curve->UserData( db_curve );

		geo0.CopyAppend( *curve );
		delete curve;
	}
	if (geo0.Winding() > 0)
	{ geo0.Reverse(); }		// Holes go one way, outlines the other.

	geo0.Reduce();

	// ------- yeah, it's inefficient, shoot me

	CGeoPoly geo1;
	num = db_prof1->Count();
	for (idx=0; idx<num; idx++)
	{
		CDbCurve* db_curve = (CDbCurve*)(*db_prof1)[idx];
		CGeoCurve* curve = db_curve->Curve();
		curve->UserData( db_curve );

		geo1.CopyAppend( *curve );
		delete curve;
	}
	if (geo1.Winding() > 0)
	{ geo1.Reverse(); }		// Holes go one way, outlines the other.

	geo1.Reduce();
	//
	// Now perform the operation
	//
	CGeoPoly* result;

	switch (op)
	{
		case 1:
			result = geo0.AND(geo1);
			break;
		case 2:
			result = geo0.OR(geo1);
			break;
		case 3:
			result = geo0.MINUS(geo1);
			break;
	}

	//
	// Put the result into the model
	//
	CDbTool* tool;
	CDbWorkplane* work;

	num = result->Count();
	for (idx=0; idx<num; idx++)
	{
		const CGeoCurve& curve = (*result)[idx];
		CDbEntity* db_ent = NULL;

		CDbEntity* source = (CDbEntity*)curve.UserData();
/*
		if (source)
		{
			tool = source->Tool();
			work = source->Workplane();
		}
		else
*/
		{
			tool = model.ActiveTool();
			work = model.ActiveWorkplane();
		}

		switch (curve.Type())
		{
			case GEOLINE:
			{
				CDbLine* db_line = NULL;
				ret += model.EntityCreate( DBLINE, (CDbEntity**)&db_line );
				if (ret.isOkay())
					ret += db_line->Init( tool, work, *((CGeoLine*)&curve) );

				if (ret.isOkay())
				{
					db_ent = db_line;

					if (model.ActivePattern() != NULL)
						model.ActivePattern()->Append( db_ent );
				}
			}
			break;

			case GEOARC:
			{
				CDbArc* db_arc = NULL;
				ret += model.EntityCreate( DBARC, (CDbEntity**)&db_arc );
				if (ret.isOkay())
					ret += db_arc->Init( tool, work, *((CGeoArc*)&curve) );

				if (ret.isOkay())
				{
					db_ent = db_arc;

					if (model.ActivePattern() != NULL)
						model.ActivePattern()->Append( db_ent );
				}
			}
			break;
		}

		if (db_ent)
		{
			io_cmd->getViewMgr().ModelSet( model );
			io_cmd->getViewMgr().Refresh( db_ent->Id(), TRUE );
		}
	}

	return ret;
}

