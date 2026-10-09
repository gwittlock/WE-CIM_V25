
#include "stdafx.h"
#include "cmn_resource.h"
#include "CommonFlags.h"
#include "ColorConst.h"
#include "MathConst.h"
#include "StringConst.h"

#include "GeoPoly.h"

#include "DbAllEntities.h"
#include "EntityCopier.h"
#include "DbIterator.h"

#include "Profile.h"
#include "Conversion.h"
#include "Worm.h"
#include "QuickHull.h"
#include "Model.h"
#include "ModelUtil.h"


////////////////////////////////////////////////////////////////////////

// ==================================================================
//		MarkIntExt
//
//	Traverse the list of profiles and mark each as to
//	whether it is an interior (eg: is contained by another) or 
//	exterior (eg: not contained by another) profile
//
//  For the simple case where profiles do not intersect,
//  it is sufficient to check for enclosure of a single
//  point.  In this case, set robustCheck to FALSE.
//
CReturn 
CModelUtil::MarkIntExt( 
					CDbEntityList* proflist,
					bool robustCheck,
					CModel* model )
{
	CReturn		ret;
	bool		encloses;

	int maxDepth = 0;
	model->pHeader()->setInt( STR_PROFILE_DEPTH, maxDepth );

	int num = proflist->Count();
	if (num < 1)
		return ret;

	//
	// Create Poly versions of each profile
	//
	CGeoPoly* poly_array = new CGeoPoly[num];

	for (int pidx=0; pidx<num; pidx++)
	{
		CDbEntity* dbEntity = (*proflist)[pidx];

		// First, un-mark the profile; that is, assume to be exterior
		dbEntity->IntSet( STR_PROFILE_DEPTH, 0 );

		CDbArc* dbarc = dynamic_cast<CDbArc*>( dbEntity );
		CDbProfile* dbprof = dynamic_cast<CDbProfile*>( dbEntity );

		// Then, convert to poly
		CGeoPoly&	poly = poly_array[pidx];
		if (dbarc != nullptr)
		{
			CGeoCurve* geoCurve = dbarc->Curve();
			poly.CopyAppend( *geoCurve );//, geoCurve->Length2d() / 1000 );
			delete geoCurve;
		}
		else if (dbprof != nullptr)
		{
			for (int elidx=0; elidx<dbprof->Count(); elidx++)
			{
				CGeoCurve*	curve = ((CDbCurve*)(*dbprof)[elidx])->Curve();

				// Make the tolerance be 1% of the curve length... for jollies
				poly.CopyAppend( *curve );//, curve->Length2d() / 1000 );

				delete curve;
			}
		}
	}

	//
	// Test each poly against each other, for inclusion
	//
	for (int idx1=0; idx1<num; idx1++)
	{
		for (int idx2=0; idx2<num; idx2++)
		{
			if (idx2==idx1)
				continue;

			if ( robustCheck )
				encloses = poly_array[idx1].Encloses( poly_array[idx2] );//, 0.0 );
			else
				encloses = poly_array[idx1].PtInPoly( poly_array[idx2][0].StartPt() );//, 0.0 );

			if ( encloses )
			{
				CDbEntity*	db_ent = (*proflist)[idx2];

				// For each time a profile is enclosed, it gets its 
				// value flipped... a sort of enclosure parity.  Seems to work!
				int depth = db_ent->IntGet( STR_PROFILE_DEPTH, 0 );

				depth++;
				db_ent->IntSet( STR_PROFILE_DEPTH, depth );

				if (depth > maxDepth)
					maxDepth = depth;
			}
		}
	}

	if (poly_array) delete[] poly_array;

	model->pHeader()->setInt( STR_PROFILE_DEPTH, maxDepth );

	return ret;
}

// ==================================================================
//	Generic matrix-based transformation
CReturn CModelUtil::Transform( 
	CModel*				model,
	const C3x4Matrix&	in_xform, 
	int					in_copies,
	BYTE				control )
{
	CReturn			ret;

	tDbEntityMap	selectedEntities;
	POSITION		selPos;
	int				selCount;

	CMap<CDbEntity*, CDbEntity*, int, int>	endpts;
	POSITION		endptsPos;
	int				endptsCount;

	CDbEntityArray	xformEntities;
	CDbEntityList	refdEntities;

	CDbFeature*		dbFeature;
	CDbEntity*		dbEntity;
	CDbEntity*		selectedEntity;
	CDbEntity*		refent;
	CDbCurve*		dbCurve;
	CDbArc*			dbArc;
	CDbPoint*		dbPoint;
	CDbPoint*		dbNewPt;
	CString			sval;
	int				count, indx, jndx;
	int				pass_num;
	int				refcnt;


	C3x4Matrix xform = in_xform;

	CDbEntity::NewAction();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Build the list of unique selected entities.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CSelectorStack& selectorStack = model->SelectorStack();
	CSelector& selector = selectorStack();

	count = selector.Count();

	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = selector[indx];

		refdEntities.BenignFlush();

		dbFeature = dynamic_cast<CDbFeature*>( dbEntity );
		ConditionalDisassociate( dbFeature );

		dbEntity->RefsTo( &refdEntities );
		for (jndx = 0; jndx < refdEntities.Count(); jndx++)
		{
			refent = refdEntities[jndx];
			selectedEntities.SetAt( refent, refent );
		}
		selectedEntities.SetAt( dbEntity, dbEntity );
	}

	selCount = selectedEntities.GetCount();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy the selected entities as necessary and build a list of the atomic
	// entities that will be transformed (ie. point and command entities).
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	pass_num = ((in_copies <= 0) ? 1 : in_copies);

	for (indx = 0; indx < pass_num; indx++)
	{
		// Create the list of entities to be transformed
		model->EntityPrepareCopy();
		
		xformEntities.BenignFlush();
		endpts.RemoveAll();
	
		selPos = selectedEntities.GetStartPosition();
		for (jndx = 0; jndx < selCount; jndx++)
		{
			selectedEntities.GetNextAssoc( selPos, selectedEntity, selectedEntity );

			if ( !(control & XFORM_NO_SYSTEM) || !selectedEntity->IsSystem() )
			{
				if (in_copies > 0)
				{
					model->EntityCopy( (*selectedEntity), &dbEntity );
					dbEntity->SelectFlag( false );
					
					if (model->ActivePattern() != nullptr && dbEntity->Owner() == nullptr)
						model->ActivePattern()->Append( dbEntity );
				}
				else
				{
					dbEntity = selectedEntity;
				}

				xformEntities.Append( dbEntity );

				// Since curve end-points may be associated,
				// we must track the number of selected curves
				// that reference a given point.
				dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
				if (dbCurve != nullptr)
				{
					CDbEntityList	pts;
					dbCurve->RefsTo( &pts );

					int pcnt = pts.Count();
					for (int pndx = 0; pndx < pcnt; ++pndx)
					{
						dbEntity = pts[pndx];

						if ( !endpts.Lookup( dbEntity, refcnt ) )
							refcnt = 0;

						++refcnt;
						endpts.SetAt( dbEntity, refcnt );
					}
				}
				else
				{
					if ((dbEntity->Type() == DBPOINT) ||
						(dbEntity->Type() == DBCOMMAND))
					{
						if ( !endpts.Lookup( dbEntity, refcnt ) )
							refcnt = 0;

						++refcnt;
						endpts.SetAt( dbEntity, refcnt );
					}
				}
			}
		}


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Break curve end-point associativity as necessary.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


		endptsCount = endpts.GetCount();
		endptsPos = endpts.GetStartPosition();
		for (jndx = 0; jndx < endptsCount; jndx++)
		{
			endpts.GetNextAssoc( endptsPos, dbEntity, refcnt );

			dbPoint = dynamic_cast<CDbPoint*>( dbEntity );
			if (dbPoint != nullptr && dbPoint->RefCnt() > refcnt)
			{
				// The count of curves that reference this end point
				// is greater than the count of curves that are being
				// transformed.  As such, we must be careful to break
				// end-point associativity where arcs are concerned,
				// because transforming an arc's end-point (without
				// transforming the arc) can cause grievous harm.

				CDbEntityList refingEntities;
				dbPoint->RefdBy( &refingEntities );

				int rcnt = refingEntities.Count();
				for (int rndx = 0; rndx < rcnt; ++rndx)
				{
					refent = refingEntities[rndx];

					if ( !selectedEntities.Lookup( refent, refent ) )
					{
						dbArc = dynamic_cast<CDbArc*>( refent );
						if (dbArc != nullptr)
						{
							dbNewPt = dbArc->ConditionalDisassociate( dbPoint );
						}
					}
				}
			}
		}

		// Sort the entities such that higher order entities are first.
		// This simplifies matters, especially when processing profiles,
		// whose curve order must be reversed during mirroring operations.
		// TODO: Perhaps restrict this to mirroring operations(?)
		xformEntities.Qsort( &CModelUtil::SortFunc );

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Finally, transform the entities.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		for (jndx = 0; jndx < xformEntities.Count(); jndx++)
		{
			dbEntity = xformEntities[jndx];

			if (!dbEntity->DidAction() && !dbEntity->IsDeleted())
			{
				dbEntity->Transform( xform );
				dbEntity->ModifyFlag( true );
			}

			// 2005.11.10 (PE) -- Sunflower reported that mirrored
			// parts in a nest had the wrong winding direction.
			if (control & XFORM_REV_PROFS)
			{
				// Reversing the profile corrects both the
				// winding direction and the cutside.
				if (dbEntity->Type() == DBPROFILE)
					((CDbProfile*) dbEntity)->Reverse();
			}
		}

		// IT#376, since we don't change the selection,
		// we need to adjust the transform each pass
		in_xform.Transform( &xform );
	}

	return ret;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TransformGrid
//
//	Generic matrix-based transformation
//
CReturn 
CModelUtil::TransformGrid( 
	CModel*		model,
	double		dx,
	double		dy,
	int			xcnt,
	int			ycnt )
{
	CReturn			ret;

	tDbEntityMap	selectedEntities;
	POSITION		selPos;
	int				selCount;

	CMap<CDbEntity*, CDbEntity*, int, int>	endpts;
	POSITION		endptsPos;
	int				endptsCount;

	CDbEntityArray	xformEntities;
	CDbEntityList	refdEntities;

	CDynamicArray<C3dVec*>	delta;
	C3x4Matrix		xform;

	CDbFeature*		dbFeature;
	CDbEntity*		dbEntity;
	CDbEntity*		selectedEntity;
	CDbEntity*		refent;
	CDbCurve*		dbCurve;
	CDbArc*			dbArc;
	CDbPoint*		dbPoint;
	CDbPoint*		dbNewPt;
	CString			sval;
	int				count, indx, jndx;
	int				refcnt;

	CDbEntity::NewAction();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Build the list of unique selected entities.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CSelectorStack& selectorStack = model->SelectorStack();
	CSelector& selector = selectorStack();

	count = selector.Count();

	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = selector[indx];

		refdEntities.BenignFlush();

		dbFeature = dynamic_cast<CDbFeature*>( dbEntity );
		ConditionalDisassociate( dbFeature );

		dbEntity->RefsTo( &refdEntities );
		for (jndx = 0; jndx < refdEntities.Count(); jndx++)
		{
			refent = refdEntities[jndx];
			selectedEntities.SetAt( refent, refent );
		}
		selectedEntities.SetAt( dbEntity, dbEntity );
	}

	selCount = selectedEntities.GetCount();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Build a list of deltas to the transformed positions (row major).
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	for (jndx = 0; jndx < ycnt; ++jndx)
	{
		for (indx = 0; indx < xcnt; ++indx)
		{
			if ((indx == 0) && (jndx == 0))
				continue;  // Prevent duplicates at local origin.

			delta.Append( new C3dVec( (dx * indx), (dy * jndx), 0. ) );
		}
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Copy the selected entities as necessary and build a list of the atomic
	// entities that will be transformed (ie. point and command entities).
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = delta.Count();
	for (indx = 0; indx < count; indx++)
	{
		// Create the list of entities to be transformed
		model->EntityPrepareCopy();
		
		xformEntities.BenignFlush();
		endpts.RemoveAll();
	
		selPos = selectedEntities.GetStartPosition();
		for (jndx = 0; jndx < selCount; jndx++)
		{
			selectedEntities.GetNextAssoc( selPos, selectedEntity, selectedEntity );

			if ( !selectedEntity->IsSystem() )
			{
				model->EntityCopy( (*selectedEntity), &dbEntity );
				dbEntity->SelectFlag( false );
				
				if (model->ActivePattern() != nullptr && dbEntity->Owner() == nullptr)
					model->ActivePattern()->Append( dbEntity );

				xformEntities.Append( dbEntity );

				// Since curve end-points may be associated,
				// we must track the number of selected curves
				// that reference a given point.
				dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
				if (dbCurve != nullptr)
				{
					CDbEntityList	pts;
					int				pcnt, pndx;

					dbCurve->RefsTo( &pts );
					pcnt = pts.Count();
					for (pndx = 0; pndx < pcnt; ++pndx)
					{
						dbEntity = pts[pndx];

						if ( !endpts.Lookup( dbEntity, refcnt ) )
							refcnt = 0;

						++refcnt;
						endpts.SetAt( dbEntity, refcnt );
					}
				}
				else
				{
					if ((dbEntity->Type() == DBPOINT) ||
						(dbEntity->Type() == DBCOMMAND))
					{
						if ( !endpts.Lookup( dbEntity, refcnt ) )
							refcnt = 0;

						++refcnt;
						endpts.SetAt( dbEntity, refcnt );
					}
				}
			}
		}


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Break curve end-point associativity as necessary.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		endptsCount = endpts.GetCount();
		endptsPos = endpts.GetStartPosition();
		for (jndx = 0; jndx < endptsCount; jndx++)
		{
			endpts.GetNextAssoc( endptsPos, dbEntity, refcnt );

			dbPoint = dynamic_cast<CDbPoint*>( dbEntity );
			if (dbPoint != nullptr && dbPoint->RefCnt() > refcnt)
			{
				// The count of curves that reference this end point
				// is greater than the count of curves that are being
				// transformed.  As such, we must be careful to break
				// end-point associativity where arcs are concerned,
				// because transforming an arc's end-point (without
				// transforming the arc) can cause grievous harm.

				CDbEntityList	refingEntities;
				int				rcnt, rndx;

				dbPoint->RefdBy( &refingEntities );
				rcnt = refingEntities.Count();

				for (rndx = 0; rndx < rcnt; ++rndx)
				{
					refent = refingEntities[rndx];

					if ( !selectedEntities.Lookup( refent, refent ) )
					{
						dbArc = dynamic_cast<CDbArc*>( refent );
						if (dbArc != nullptr)
						{
							dbNewPt = dbArc->ConditionalDisassociate( dbPoint );
						}
					}
				}
			}
		}

		// Sort the entities such that higher order entities are first.
		// This simplifies matters, especially when processing profiles,
		// whose curve order must be reversed during mirroring operations.
		// TODO: Perhaps restrict this to mirroring operations(?)
		xformEntities.Qsort( &CModelUtil::SortFunc );


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Finally, transform the entities.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		xform.setUnit();
		xform.Shift( *(delta[indx]) );
		for (jndx = 0; jndx < xformEntities.Count(); jndx++)
		{
			dbEntity = xformEntities[jndx];

			if (!dbEntity->DidAction())
			{
				dbEntity->Transform( xform );
				dbEntity->ModifyFlag( true );
			}
		}
	}

	delta.DestructiveFlush();

	return ret;
}

// ==================================================================
// NOTE: Really used for searching on NC_Code_Number or Tool ID
//
CDbTool*
CModelUtil::DbToolFind( const CEntityDb& db, const CString& field, int value )
{
	CDbIterator iter;

	if (value < 0)
		return nullptr;

	iter.Init( db, DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == nullptr)
			break;

		int ival = dbTool->IntGet( field, IUNDEFINED );
		if (ival == value)
			return dbTool;

		iter.Next();
	}

	return nullptr;
}

// ==================================================================
// Find a tool station given the station ID number (filled or empty)
//
CDbTool*
CModelUtil::FindToolByID( CEntityDb* db, int toolID, bool filled )
{
	CDbIterator iter;

	iter.Init( (*db), DBTOOL );
	while (1)
	{
		CDbTool* test = dynamic_cast<CDbTool*>( iter() );
		if (test == nullptr)
			break;

		if (toolID == test->IntGet( STR_TOOL_ID, IUNDEFINED ))
		{
			int typeID = test->IntGet( STR_TYPE_ID, IUNDEFINED );

			bool okay = (filled ? (typeID != TTYPE_OPEN) : (typeID == TTYPE_OPEN));
			if ( okay )
				return test;
		}

		iter.Next();
	}

	return nullptr;
}

/*
// ==================================================================
//	DbToolMatch
//
//	Find a tool in the database that matches the provided tool,
//	using the *content* of the tools as criteria.
//
///	Returns the first matching tool.
CDbTool*
CModelUtil::DbToolMatch(
	const CEntityDb&	db,
	const CDbTool&		src_tool )
{
	int			high_score = 0;
	CDbTool*	high_tool = nullptr;
	CDbTool*	test=nullptr;

	CDbIterator iter( db );
	iter.Init( DBTOOL );
	while (1)
	{
		test = dynamic_cast<CDbTool*>( iter() );
		if (test == nullptr)
			break;
		iter.Next();

		int score = src_tool.Matches( *test );
		if ( score
			&& (score > high_score) )
		{
			high_score = score;
			high_tool = test;
		}
	}

	return high_tool;
}

// ==================================================================
// Introduced for Nesting, to 'create tool setups on-the-fly'.
// At first implementation, it was deemed sufficient (by Gary)
// to match tools by 'tool crib id'.
//
//	TODO:  Determine if anyone actually uses this!
CDbTool*
CModelUtil::ToolFindCreate( 
	CEntityDb*		db, 
	const CDbTool&	dbTool,
	bool			create )
{
	int toolID = dbTool.IntGet( STR_TOOL_ID, IUNDEFINED );

	// First try to find a matching tool.
	CDbTool* result = DbToolMatch( db, dbTool );
	if (result == nullptr)
	{
		// Put a copy of the tool in the first empty station having the correct size.
		double reqdStationSize = dbTool.DoubleGet( STR_STATION_SIZE, 0.0 );

		CDbIterator iter( (*db) );
		iter.Init( DBTOOL );
		while (1)
		{
			CDbTool* candidate = dynamic_cast<CDbTool*>( iter() );
			if (candidate == nullptr)
				break;
			iter.Next();

			int type = candidate->IntGet( STR_TYPE_ID, IUNDEFINED );
			if (type == IUNDEFINED)
			{
				// We've found an empty station.

				double stationSize = candidate->DoubleGet( STR_STATION_SIZE, 0. );
				if (fabs(reqdStationSize-stationSize) <= 1.e-3)
				{
					// Close enough for government work!
					result = candidate;
					break;
				}
			}
		}

		if ( create
			&& result )
		{
			// Copy the parameters of the reference tool, being careful
			// to retain the NC_Code_Number associated with the station.
			int ncCodeNum = result->IntGet( STR_NC_CODE_NUMBER, IUNDEFINED );
			(*(result->pAttrib())) = dbTool.Attrib();
			result->IntSet( STR_NC_CODE_NUMBER, ncCodeNum );
		}
	}

	return result;
}
*/

// ==================================================================

void
CModelUtil::EmptyStationsDelete( CEntityDb* db )
{
	CDbIterator iter;

	iter.Init( (*db), DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == nullptr)
			break;

		if (dbTool->RefCnt() <= 0)
			dbTool->Delete();

		iter.Next();
	}
}

// ==================================================================
// NOTE: Nested features will retain their association to their original tool.
// NOTE: This method was introduced to support manual toolpath construction via
//       java.  As such, there was no need to mark this feature as dirty.  This
//       changed with the introduction of 'autouser.java', however.  In this
//       later case, the java file may reassociate an application generated
//       toolpath to a different tool.  In this case, we want the toolpath to
//       be regenerated using the parameters of the new tool.
//
//       See also CCreateProcessApp::FeatureToolAssociate()
//
CReturn
CModelUtil::ToolAssociate( CDbFeature* dbFeature, CDbTool* dbTool )
{
	CReturn status;
	int color;

	if (dbFeature->Tool() == dbTool)
		return status;  // nothing to do

	dbFeature->ModifyFlag( true );

	color = dbTool->ColorGet( DCOLOR_RED );

	CString function = dbFeature->StringGet( "function", "" );
	if (function.GetLength() > 0)
	{
		// This feature may be 'regeneration capable'.
		// Examples can be seen in:
		//     CCreateProcessApp::ProfileGenerate2()
		//     CCreateProcessApp::ToolpathSpiral()
		//     CCreateProcessApp::AutoIndexOffset()

		int toolid = dbFeature->IntGet( "toolid", IUNDEFINED );
		if (toolid != IUNDEFINED)
		{
			if (dbTool == nullptr)
			{
				// NOTE: The 'function' and 'toolid' attributes are immutable
				// for toolpaths that can be regenerated.  By removing these
				// attributes, we prevent a regen from occuring.  As we do not
				// know all of the attributes associated with the 'function'
				// attribute, the other attributes will appear as lint.

				dbFeature->AttribDelete( "function" );
				dbFeature->AttribDelete( "toolid" );
			}
			else
			{
				// Ensure the Portal command associated with this feature
				// will be reconstructed using the correct dbTool id.
				dbFeature->IntSet( "toolid", dbTool->Id() );
			}
		}
	}

	int count = dbFeature->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = (*dbFeature)[indx];

		CDbFeature* nestedFeature = dynamic_cast<CDbFeature*>( dbEntity );
		if (nestedFeature == nullptr)
		{
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );
			if (dbProfile == nullptr)
			{
				dbEntity->Tool( dbTool );
			}
			else
				CModelUtil::ToolAssociate( dbProfile, dbTool );

			// dbEntity->ColorSet( color );
		}
	}

	return status;
}

// ==================================================================

CReturn
CModelUtil::ToolAssociate( CDbProfile* dbProfile, CDbTool* dbTool )
{
	CReturn	status;
	int		color;

	if (dbProfile->Tool() == dbTool)
		return status;  // nothing to do.

	color = dbTool->ColorGet( DCOLOR_RED );

	int count = dbProfile->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = (*dbProfile)[indx];
		dbEntity->Tool( dbTool );
		// dbEntity->ColorSet( color );
	}

	return status;
}

// Finds the "first parent feature" of an entity.
// Returns nullptr if no such parent.
CDbFeature*
CModelUtil::FirstFeature( const CDbEntity* dbEntity )
{
	CDbFeature* parent = nullptr;
	
	CDbEntity* owner = dbEntity->Owner();

	while (owner != nullptr)
	{
		parent = dynamic_cast<CDbFeature*>( owner );
		if (parent != nullptr)
			break;

		owner = owner->Owner();
	}

	return parent;
}

//===========================================================================
// Moved here from CCreateProcess.  TODO: Move to some reasonable place :-(
//===========================================================================

CReturn 
CModelUtil::FeatureFind(
						const CEntityDb& db,
						ID id,
						CDbFeature** dbFeature )
{
	CReturn status;

	CDbEntity* dbEntity = nullptr;

	if (id > 0)
		status = db.Find( id, &dbEntity, DBFEATURE, DBFEATURE );

	(*dbFeature) = dynamic_cast<CDbFeature*>( dbEntity );

	if ((*dbFeature) == nullptr)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelUtil::FeatureFind()" );
	}

	return status;
}

// ==================================================================

CReturn 
CModelUtil::FeatureFindCreate(
						CEntityDb* db,
						ID id,
						CDbFeature** dbFeature )
{
	CReturn status;

	(*dbFeature) = nullptr;

	if (id > 0)
	{
		status = FeatureFind( (*db), id, dbFeature );
	}
	else
	{
		db->Create( DBFEATURE, (CDbEntity**) dbFeature );
	}

	return status;
}

// ==================================================================
// We need a feature to hold our tooled entities... but it must be under the 
// super-feature we specify.  SO... search around to see if a feature exists
// designed to hold this tool and, if not, create one.
CDbFeature*
CModelUtil::SubFeatureFindCreateByTool(
		CEntityDb*		db,				// The database
		CDbFeature*		superfeature,	// Where we start our investigations
		ID				toolId,			// Which tool are we hunting for?
		CDbWorkplane*	dbWork )		// If we create a feature, which workplane it is on
{
	CDbFeature*	dbFeature;
	CDbFeature* testfeat;
	int			num, idx;

	num = superfeature->Count();

	// Exhaustive search, but only one deep, so get your superfeature right the first time
	for (idx=0; idx<num; idx++)
	{
		testfeat = dynamic_cast<CDbFeature*>((*superfeature)[idx]);
		if (testfeat)
		{
			if ( testfeat->IsToolpath()
				&& (testfeat->ToolId() == toolId) )
				return testfeat;

// Don't need to go recursive now, but if we want to, uncomment this bit
//				CDbFeature* subfeature = SubFeatureFindCreateByTool( nullptr, testfeat, toolId, dbWork );
//				if (subfeature)
//					return subfeature;
		}
	}

	// Ummm, if we are deep in, exit here.
	if (!db)
		return nullptr;

	// Haven't returned anything, so MAKE something.
	db->Create( DBFEATURE, (CDbEntity**) &dbFeature );

	if (dbFeature)
		superfeature->Append( dbFeature, FALSE );

	return dbFeature;
}

// ==================================================================

CReturn 
CModelUtil::PatternFind(
						const CEntityDb& db,
						ID id,
						CDbPattern** dbPattern )
{
	CReturn status;

	CDbEntity* dbEntity = nullptr;

	if (id > 0)
		status = db.Find( id, &dbEntity, DBPATTERN, DBPATTERN );

	(*dbPattern) = dynamic_cast<CDbPattern*>( dbEntity );

	if ((*dbPattern) == nullptr)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelUtil::PatternFind()" );
	}

	return status;
}

// ==================================================================

CReturn
CModelUtil::PatternCreate(
						CEntityDb* db,
						CDbPattern** dbPattern )
{
	CReturn status;

	(*dbPattern) = nullptr;
	
	status = db->Create( DBPATTERN, (CDbEntity**) dbPattern );

	return status;
}

// ==================================================================

CReturn 
CModelUtil::PatternFindCreate(
						CEntityDb* db,
						ID id,
						CDbPattern** dbPattern )
{
	CReturn status;

	(*dbPattern) = nullptr;

	if (id > 0)
	{
		status = PatternFind( (*db), id, dbPattern );
	}
	else
	{
		status = PatternCreate( db, dbPattern );
	}

	return status;
}

// ==================================================================
// Replaces the given instance (dbCommand) with a feature containing
// copies of the entities of the pattern that is referenced by the
// instance.  The instance is deleted if the method is successful.
//
// Note an older form of (potentially recursive) Explode() later in this file.
//
CReturn
CModelUtil::PatternExplode(
	CDbCommand**	dbCommand,
	CDbFeature**	dbFeature )
{
	CReturn			status;
	C3x4Matrix		pat_xform;
	CEntityCopier	copier;
	CEntityDb*		db;
	CDbPattern*		dbPattern;
	CDbFeature*		owner;
	CDbEntity*		dbEntity;
	CDbEntity*		theCopy;
	CString			msg;
	double			angle;
	ID				patid;
	int				count, indx;

	(*dbFeature) = nullptr;

	if ( !(*dbCommand)->IsInstance() )
	{
		msg.Format(
			"CModelUtil::PatternExplode() -- dbCommand with id=%d is not an instance.",
			(*dbCommand)->Id() );

		status.Internal( IDS_INTERNAL_ERROR, msg );
	}
	else
	{
		db = (*dbCommand)->Db();
		
		C3dCoord delta = (*dbCommand)->Coord();
		delta.Z(0.0);

		patid = (ID) (*dbCommand)->IntGet( "patid", 0 );
		angle = (*dbCommand)->DoubleGet( "instang", 0. );

		db->Find( patid, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );

		if (dbPattern == nullptr)
		{
			msg.Format(
				"CModelUtil::PatternExplode() -- dbPattern with id=%d was not found.",	patid );

			status.Internal( IDS_INTERNAL_ERROR, msg );
		}
		else
		{
			// Create a _part feature to hold our explosion
			db->Create( DBFEATURE, (CDbEntity**) dbFeature );

			// 2006.04.17 (PE) -- So that we have some record of the
			// pattern from which this feature arose. This was done
			// only so that we could eliminate the "useless" features
			// that are generated by "chain nesting".
			(**(dbFeature)).IntSet( "exp", patid );

			owner = dynamic_cast<CDbFeature*>( (*dbCommand)->Owner() );
			if (owner != nullptr)
			{
				owner->InsertBefore( (*dbCommand), (*dbFeature) );
			}

			// Copy the pattern entities into the feature.
			copier.Init( db, FLAG_COPY_TOOL_VERBATIM );

			count = dbPattern->Count();
			for (indx = 0; indx < count; indx++)
			{
				dbEntity = (*dbPattern)[indx];
				if ( !dbEntity->IsDeleted() )
				{
					dbEntity->Accept( &copier );

					theCopy = copier.ForwardLookup( dbEntity );

					if (theCopy != nullptr)
						(*dbFeature)->Append( theCopy );
				}
			}
			CString name = dbPattern->StringGet( "_label", "" );
			if (name.GetLength() > 1)
			{
				CDbCommand* db_cmd = nullptr;
				db->Create( DBCOMMAND, (CDbEntity**)&db_cmd );
				db_cmd->Init( (*dbCommand)->Tool(), (*dbCommand)->Workplane(),
							dbPattern->DoubleGet( "_label_dx", 0.0 ),
							dbPattern->DoubleGet( "_label_dy", 0.0 ),
							0.0,
							name );
				db_cmd->SystemFlag( false );
#if (_CI || _NST)
				db_cmd->DoubleSet(
					"label_size", dbPattern->DoubleGet( "label_size", 1.) );
#endif
				db_cmd->DoubleSet(
					"angle", dbPattern->DoubleGet( "_label_angle", 0.0 ) );

				db_cmd->IntSet(
					"pos", dbPattern->IntGet( "pos", 0 ) );

				// 2006.09.27 (PE) -- I noticed that executing a Full View
				// (amongst other things) was causing the instance text to
				// be drawn, regardless of the corresponding view option.
				//
				// I am afraid set this attribute to the actual pattern id
				// because I can not predict the consequences.  On the flip
				// side, doing so would provide some derivation history.
				//
				// See also CViewBase::PrintText().
				db_cmd->IntSet( "patid", -1 );

				(*dbFeature)->Append( db_cmd );
			}

			// Transform the part to the correct position and orientation.
			pat_xform.setXYAngle( DEG2RAD * angle );
//			pat_xform.Shift( (*dbCommand)->Coord() );
			pat_xform.Shift( delta );

			(*dbFeature)->Transform( pat_xform );

			// -----------------

			(*dbCommand)->Delete();
			(*dbCommand) = nullptr;
		}
	}

	return status;
}
//
// ---------- Explode all
//
CReturn
CModelUtil::PatternExplode( CDbPattern& pattern )
{
	CReturn ret;

	CDbIterator institer;
	institer.Init( *pattern.Db(), DBCOMMAND );
	while ( TRUE )
	{
		CDbCommand* db_command = dynamic_cast<CDbCommand*>( institer() );
		if (!db_command)
			break;
		institer.Next();

		if ( db_command->IsInstance() )
		{
			ID patid = (ID)db_command->IntGet( "patid", -1 );

			if (patid == pattern.Id())
			{
				CDbFeature* boom;
				PatternExplode( &db_command, &boom );
			}
		}
	}

	return ret;
}

// ==================================================================

CReturn
CModelUtil::PunchedFeatureCreate(
							const CGeoElemList&	elems,
							CModel*				model,
							CDbFeature*			topLevelFeature )
{
	CReturn status;

	CDbFeature*		currFeature = nullptr;
	CDbTool*		dbTool;
	CDbWorkplane*	dbWork;
	CDbEntity*		dbEntity;
	int				color, partprof, cutside;
	int				count, indx, jndx;
	int				stationID, currStationID;

	CVarList		defaults = model->Default();

	currStationID = 0;

	count = elems.Count();
	for (indx = 0; indx < count; ++indx)
	{
		const CGeoElem* elem = elems[indx];

		stationID = elem->IntGet( STR_STATION_ID, 0 );
		if (stationID > 0)
		{
			dbTool = DbToolFind( model->Db(), STR_STATION_ID, stationID );

			if (stationID != currStationID)
			{
				dbWork = dbTool->Workplane();
	
				color = dbTool->ColorGet( DCOLOR_RED );

				model->pDefault()->setColor( color );

				if (currFeature == nullptr)
				{
					// topLevelFeature->ColorSet( color );
					currFeature = topLevelFeature;
				}
				else
				{
					model->EntityCreate( DBFEATURE, (CDbEntity**) &currFeature );

					topLevelFeature->Append( currFeature );
				}

				currStationID = stationID;
			}

			dbEntity = model->Db().GeoConvert( (*elem), dbTool, dbWork );

			if ( !currFeature->Append( dbEntity ).IsOk() )
				break;

			partprof = elem->IntGet( STR_PARTPROF, 0 );
			cutside = elem->IntGet( STR_CUTSIDE, 0 );

			jndx = currFeature->Count() - 1;
			(*currFeature)[jndx]->IntSet( STR_CUTSIDE, cutside );

			if ( partprof )
				(*currFeature)[jndx]->IntSet( STR_PARTPROF, partprof );

			// See also CDbEntity::Describe2d()
			if ( dbTool->IsIndexable() )
			{
				double orient = elem->DoubleGet( STR_ORIENT, 0.0 );
				(*currFeature)[jndx]->DoubleSet( STR_ORIENT, orient );
			}
		}
	}

	(*(model->pDefault())) = defaults;

	return status;
}


// =================================================================================
//	ExplodeAll
//
// Given a container, explode it, and explode the owner, back up the heirarchy until
// it's all just loose geometry.  BAM!
//
CReturn
CModelUtil::Explode( 
	CDbContainer*	db_container,
	bool			recurse )
{
	CReturn	status;

	CDbProfile* db_prof = dynamic_cast<CDbProfile*>(db_container);
	if (db_prof)
		db_prof->Disassociate();

	// Prepare to move the entities to the owner of the profile.
	CDbEntityList dbEntities;
	CDbContainer* db_owner = dynamic_cast<CDbContainer*>( db_container->Owner() );

	if (db_owner != nullptr)
	{
		// Move the curves out of the container and into the owner.
		int count = db_container->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* db_ent = (*db_container)[ 0 ];
			db_container->Disown( db_ent );
			db_owner->InsertBefore( db_container, db_ent );
		}
	}
	else
	{
		// Simply flush the profile.
		db_container->BenignFlush();
	}


	db_container->Delete();
	db_container->ModifyFlag( true );

	if ( recurse
		&& db_owner )
	{
		return CModelUtil::Explode( db_owner, TRUE );
	}

	return status;
}


// =================================================================================
// Find any empty containers and remove them
//
CReturn
CModelUtil::EmptyContainers( CModel& model, bool workzones )
{
	CReturn		status;
	CDbIterator	iter;

	iter.Init( model.Db(), DBPROFILE );
	while (1)
	{
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == nullptr)
			break;

		if (dbProfile->Count() < 1)
			dbProfile->Delete();

		iter.Next();
	}

	iter.Init( model.Db(), DBFEATURE );
	while (1)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == nullptr)
			break;

		if (dbFeature->Count() < 1)
		{
			if ( workzones )
			{
				// Delete all features, regardless.
				dbFeature->Delete();
			}
			else if ( !dbFeature->IsWorkZone() )
			{
				// Delete only those features that are not a workzone.
				dbFeature->Delete();
			}
		}

		iter.Next();
	}

	return status;
}

// ============================================================================

void
CModelUtil::EntityReverse( CDbEntity* dbEntity )
{
	CDbCurve*	dbCurve;
	CDbArc*		dbArc;
	// int			cutside;

	// Reverse arc direction
	dbArc = dynamic_cast<CDbArc*>( dbEntity );
	if (dbArc != nullptr)
		dbArc->Dir( -(dbArc->Dir()) );

	/* 2004.04.27 -- already handled by entity reversal code.
	// Flip cutside
	cutside = dbEntity->IntGet( STR_CUTSIDE, IUNDEFINED );
	if (cutside != IUNDEFINED)
		dbEntity->IntSet( STR_CUTSIDE, -cutside );
	*/

	// Reverse geometry
	if (dbEntity->Type() == DBPROFILE)
	{
		((CDbProfile*) dbEntity)->Reverse();
	}
	else
	{
		dbCurve = dynamic_cast<CDbCurve*>(dbEntity);
		if (dbCurve != nullptr)
		{
			CDbProfile* owner = dynamic_cast<CDbProfile*>(dbEntity->Owner());
			if (owner == nullptr)
			{
				// Only reverse if NOT in a profile.
				// (2004.04.27 -- why this restriction?)
				dbCurve->Reverse();
			}
		}
	}
}

// ============================================================================

CReturn
CModelUtil::ChainCut(
	CModel& model,
	CDbLine* dbLineA,
	CDbLine* dbLineB,
	double len,
	bool horz )
{
	CReturn ret;
	CDbCurveList chain;


	CDbProfile* dbProfA = dynamic_cast<CDbProfile*>(dbLineA->Owner());
	CDbProfile* dbProfB = dynamic_cast<CDbProfile*>(dbLineB->Owner());
	if ( !dbProfA || !dbProfB )
		return CReturn(STATUS_ERROR);

	double dist_AstBen = dbLineA->StartPt().DistXY( dbLineB->EndPt());
	double dist_AenBst = dbLineA->EndPt().DistXY( dbLineB->StartPt());
	if (dist_AstBen < dist_AenBst)
	{
		CDbLine* temp = dbLineA;
		dbLineA = dbLineB;
		dbLineB = temp;
		// 2004.04.27 -- Without this, chaining would fail.
		// See also "if (b_line < 0)"
		CDbProfile* tprf = dbProfA;
		dbProfA = dbProfB;
		dbProfB = tprf;
	}

	dbProfA->Disassociate();
	dbProfB->Disassociate();

	// Extract the B-prof curves first, all
	// except for the chosen line!
	int b_line = dbProfB->Position(dbLineB);
	int b_idx = (b_line + 1) % dbProfB->Count();

	if (b_line < 0)
	{
		// 2004.04.27 -- Without this, chaining would get into an infinite loop.
		ret.Internal( IDS_INTERNAL_ERROR, "CModelUtil::ChainCut()" );
		return ret;
	}

	CDbCurve* db_curve = nullptr;
	while (b_idx != b_line)
	{
		db_curve = (CDbCurve*)(*dbProfB)[b_idx];
		chain.Append(db_curve);

		b_idx = (b_idx+1) % dbProfB->Count();
	}

	// Extend that last line
	CDbPoint* db_st = db_curve->DbStartPt();
	CDbPoint* db_en = db_curve->DbEndPt();

	C2dVec vec;
	if (horz)
		vec.Init(0.0, db_en->Coord().Y() - db_st->Coord().Y());
	else
		vec.Init(db_en->Coord().X() - db_st->Coord().X(), 0.0);

	vec = vec * (len / vec.Length());

	C3dCoord new_en(db_en->Coord() + vec);

	CDbLine* new_line = nullptr;
	model.EntityCreate( DBLINE, (CDbEntity**)&new_line );
	new_line->Init( db_curve->Tool(), db_curve->Workplane(), db_en->Coord(), new_en );
	*(new_line->pAttrib()) = db_curve->Attrib();
	chain.Append(new_line);

	// Get the first line of the next profile, extend it, and join to it
	int a_line = dbProfA->Position(dbLineA);
	int a_idx = (a_line + 1) % dbProfA->Count();

	db_curve = (CDbCurve*)(*dbProfA)[a_idx];

	db_st = db_curve->DbStartPt();
	db_en = db_curve->DbEndPt();

//	vec.Init(db_st->Coord().X() - db_en->Coord().X(), 0.0);
	if (horz)
		vec.Init(0.0, db_st->Coord().Y() - db_en->Coord().Y());
	else
		vec.Init(db_st->Coord().X() - db_en->Coord().X(), 0.0);

	vec = vec * (len / vec.Length());

	C3dCoord new_st = db_st->Coord() + vec;

	model.EntityCreate( DBLINE, (CDbEntity**)&new_line );
	new_line->Init( db_curve->Tool(), db_curve->Workplane(), new_en, new_st );
	*(new_line->pAttrib()) = db_curve->Attrib();
	chain.Append(new_line);

	model.EntityCreate( DBLINE, (CDbEntity**)&new_line );
	new_line->Init( db_curve->Tool(), db_curve->Workplane(), new_st, db_st->Coord() );
	*(new_line->pAttrib()) = db_curve->Attrib();
	chain.Append(new_line);

	// Now extract the A curves... again, except for the chosen line
	while (a_idx != a_line)
	{
		db_curve = (CDbCurve*)(*dbProfA)[a_idx];
		chain.Append(db_curve);

		a_idx = (a_idx+1) % dbProfA->Count();
	}

	// Extend the A line to the B line
	db_en = dbLineA->DbEndPt();
	new_en = dbLineB->DbEndPt()->Coord();
	dbLineA->Init( dbLineA->Tool(), dbLineA->Workplane(), dbLineA->DbStartPt()->Coord(), new_en);
	chain.Append(dbLineA);
	dbLineB->Delete();
/*
	chain.Append(dbLineA);

	model.EntityCreate( DBLINE, (CDbEntity**)&new_line );
	new_line->Init( dbLineA->Tool(), dbLineA->Workplane(), dbLineA->EndPt(), dbLineB->StartPt() );
	*(new_line->pAttrib()) = dbLineA->Attrib();
	chain.Append(new_line);

	chain.Append(dbLineB);
*/
	// Now expunge and delete profile A, and fill profile B with our chained curves
	dbProfA->BenignFlush();
	dbProfA->Delete();

	dbProfB->BenignFlush();
	int num = chain.Count();
	for (int idx=0; idx<num; idx++)
	{
		dbProfB->Append( chain[idx] );
	}
	chain.BenignFlush();

	return ret;
}


// ============================================================================
//	Given a profile, find the true exit entity for that profile.  If there is a
//	lead-out feature after the prof, use the last curve in THAT, otherwise it is
//	the last curve in the profile.
//
CDbEntity*
CModelUtil::FindLeadOut(
	CDbProfile* prof)
{
	CDbContainer* leadout = prof;

	CDbContainer* owner = dynamic_cast<CDbContainer*>(prof->Owner());
	if (owner)
	{
		int idx = owner->Position(prof)+1;
		if (idx < owner->Count())
		{
			CDbFeature* feat = dynamic_cast<CDbFeature*>((*owner)[idx]);
			if ( feat
				&& feat->IsLeadOut() )
			{
				leadout = feat; 
			}
		}
	}

	int end = leadout->Count()-1;
	return (*leadout)[end];
}

// ============================================================================
//	Given a profile, find the true ENTRY entity for that profile.  If there is a
//	lead-in feature before the prof, use the first curve in THAT, otherwise it is
//	the first curve in the profile.
//
CDbEntity*
CModelUtil::FindLeadIn(
	CDbProfile* prof)
{
	CDbContainer* leadin = prof;

	CDbContainer* owner = dynamic_cast<CDbContainer*>(prof->Owner());
	if (owner)
	{
		int idx = owner->Position(prof)-1;
		if (idx >= 0)
		{
			CDbFeature* feat = dynamic_cast<CDbFeature*>((*owner)[idx]);
			if ( feat
				&& feat->IsLeadIn() )
			{
				leadin = feat; 
			}
		}
	}

	return (*leadin)[0];
}

bool
CModelUtil::IsLeadEntity( const CDbEntity* dbEntity )
{
	CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( dbEntity->Owner() );

	return ((dbFeature != nullptr) && dbFeature->IsLead());
}

// Sort the entities such that higher order entities precede lower order entities.
int
CModelUtil::SortFunc( const void* ptrA, const void* ptrB )
{
	CDbEntity* entityA = (*(CDbEntity**) ptrA);
	CDbEntity* entityB = (*(CDbEntity**) ptrB);

	return (entityB->Type() - entityA->Type());
}

void 
CModelUtil::ConditionalDisassociate( CDbFeature* dbFeature )
{
	if (dbFeature != nullptr)
	{
		CString sval;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// BugID: 377
		// Break the associativity between any toolpath
		// and its reference geometry, as necessary.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Historically, we prevented transformations from
		// being applied to associative toolpath, but this
		// behavior was difficult for users to understand,
		// and required them to first break associativity
		// using the Edit/Disassociate function, before
		// applying the transformation.
		//
		// Then, with the introduction of V15, features
		// magically were allowed to transform, and their
		// reference geometry magically transformed with it.
		// This caused crashing during regeneration.
		//
		// Finally, if someone wants to transform toolpath,
		// we will just break the association, something
		// that is much easier to teach a user (since
		// association is not something most people take
		// advantage of).

		sval = dbFeature->StringGet( "function", "" );
		if ( !sval.IsEmpty() )
		{
			dbFeature->RefsFlush();
			dbFeature->AttribDelete( "function" );
		}
	}
}

// Bounding box.
CDbProfile*
CModelUtil::BBPartOutlineCreate(
	const CDbEntityArray&	entities,
	CModel*					model )
{
	CReturn			status;
	C3dBox			extents;
	CDbWorkplane*	dbWork;
	CDbTool*		dbTool;
	CDbProfile*		dbProfile;
	CDbLine*		dbLine;
	CDbEntity*		dbEntity;
	int				count, indx;

	dbProfile = nullptr;  // assume failure.
	 
	count = entities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = entities.GetAt( indx );

		extents += dbEntity->Box();
	}

	if ( extents.IsDefinedXY() )
	{
		model->EntityFind( "Top", (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

		// Super secret incantation to automatically generate part outline.
		model->EntityFind( "Part_Outline", (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if (dbTool == nullptr)
		{
			model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
			dbTool->Name("Part_Outline");
			dbTool->Workplane( dbWork );
		}

		model->EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );

		model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
		dbLine->Init( dbTool, dbWork, 
			C3dCoord( extents.Xmin(), extents.Ymin(), 0. ),
			C3dCoord( extents.Xmin(), extents.Ymax(), 0. ) );
		dbProfile->Append( dbLine );

		model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
		dbLine->Init( dbTool, dbWork, 
			C3dCoord( extents.Xmin(), extents.Ymax(), 0. ),
			C3dCoord( extents.Xmax(), extents.Ymax(), 0. ) );
		dbProfile->Append( dbLine );

		model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
		dbLine->Init( dbTool, dbWork, 
			C3dCoord( extents.Xmax(), extents.Ymax(), 0. ),
			C3dCoord( extents.Xmax(), extents.Ymin(), 0. ) );
		dbProfile->Append( dbLine );

		model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
		dbLine->Init( dbTool, dbWork, 
			C3dCoord( extents.Xmax(), extents.Ymin(), 0. ),
			C3dCoord( extents.Xmin(), extents.Ymin(), 0. ) );
		dbProfile->Append( dbLine );
	}

	return dbProfile;
}

// Create a CCW Convex hull (aka. rubberband).
CDbProfile* CModelUtil::CHPartOutlineCreate(
	const CDbEntityArray&	entities,
	double					chordal_tol,
	CModel*					model )
{
	CReturn		status;
	CQuickHull	quick;
	Tchvector	pts;
	Tchvector	hull;

	CDbProfile* dbProfile = nullptr;  // assume failure.

	int indx, count = entities.Count();

	// Get the applicable entities.
	CDbEntityList applicable;
	for (indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = entities.GetAt( indx );
		
		CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );
		if (dbContainer != nullptr)
			dbContainer->Flatten( &applicable );
		else
			applicable.Append( dbEntity );
	}

	// TODO: We could probably optimize this stuff.
	count = applicable.Count();
	for (indx = 0; indx < count; ++indx)
	{
		CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( applicable.GetAt(indx) );
		if (dbCurve != nullptr)
		{
			if (dbCurve->Type() == DBARC)
			{
				CGeoArc* geoArc = ((CDbArc*) dbCurve)->Arc();

				ArcTabulate( (*geoArc), chordal_tol, &pts );

				delete geoArc;
			}
			else
			{
				pts.push_back( new C2dCoord( dbCurve->StartPt() ) );
				pts.push_back( new C2dCoord( dbCurve->EndPt() ) );
			}
		}
		else
		{
			// 2006.10.06 (PE) -- added to support punch-only parts.
			CDbHole* dbHole = dynamic_cast<CDbHole*>( entities.GetAt(indx) );
			if (dbHole != nullptr)
				HoleTabulate( (*dbHole), chordal_tol, &pts );
		}
	}

	// Create a CCW hull.
	quick.QuickHull( pts, true, &hull );

	count = pts.size();
	for (indx = 0; indx < count; ++indx)
	{
		delete pts[indx];
	}
	pts.clear();

	count = hull.size();
	if (count > 1)
	{
		CDbWorkplane* dbWork;
		model->EntityFind( "Top", (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

		// Super secret incantation to automatically generate part outline.
		CDbTool* dbTool;
		model->EntityFind( "Part_Outline", (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if (dbTool == nullptr)
		{
			model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
			dbTool->Name("Part_Outline");
			dbTool->Workplane( dbWork );
		}

		model->EntityCreate( DBPROFILE, (CDbEntity**) &dbProfile );

		--count;
		for (indx = 0; indx < count; ++indx)
		{
			const C2dCoord* ps = hull[indx];
			const C2dCoord* pe = hull[indx+1];

			CDbLine* dbLine;
			model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
			dbLine->Init( dbTool, dbWork, 
				C3dCoord( ps->X(), ps->Y(), 0. ),
				C3dCoord( pe->X(), pe->Y(), 0. ) );

			dbProfile->Append( dbLine );
		}
	}

	return dbProfile;
}

void
CModelUtil::ConditionalAppend( const C3dCoord& ptA, C3dCoordArray* pts )
{
	C3dCoord*	ptB;
	int			count, indx;

	count = pts->Count();
	for (indx = 0; indx < count; ++indx)
	{
		ptB = pts->GetAt( indx );
		if ( ptB->WithinTolXY( ptA, SMALL ) )
			break;
	}

	if (indx >= count)
		pts->Append( new C3dCoord( ptA ) );
}

// Returns (<0) not in workzone / (0) in pattern / (>0) in workzone
int
CModelUtil::WorkZoneNum( const CDbEntity* dbEntity )
{
	CDbFeature*	dbFeature;
	CDbPattern*	dbPattern;
	CDbEntity*	owner;
	CDbEntity*	temp;
	int			wznum;

	// Assume the entity is not in a workzone.
	wznum = -IUNDEFINED;

	switch (dbEntity->Type())
	{
	case DBLINE:
	case DBARC:
	case DBHOLE:
	case DBCOMMAND:
		temp = (CDbEntity*) dbEntity;
		while (1)
		{
			owner = temp->Owner();
			if (owner == nullptr)
				break;

			dbFeature = dynamic_cast<CDbFeature*>( owner );
			if ((dbFeature != nullptr) && dbFeature->IsWorkZone())
			{
				wznum = dbFeature->IntGet( "_zone_num", -IUNDEFINED );
				break;
			}

			dbPattern = dynamic_cast<CDbPattern*>( owner );
			if (dbPattern != nullptr)
			{
				wznum = 0;
				break;
			}

			temp = owner;
		}
		break;

	default:
		// CURRENTLY:
		// For all other entity types, we simply are not interested
		// in the workzone number. DBPOINT is, however, a very special
		// case; we do not ever allow points to be placed in features!
		break;
	}

	return wznum;
}

void
CModelUtil::ArcTabulate(
	const CGeoArc&	geoArc,
	double			chordal_tol,
	Tchvector*		pts )
{
	C3dCoordArray	tmp;
	
	geoArc.Tabulate( chordal_tol, C3dVec( 0., 0., 0. ), &tmp );

	int count = tmp.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		pts->push_back( new C2dCoord( *tmp[indx] ) );
	}

	tmp.DestructiveFlush();
}

void
CModelUtil::HoleTabulate(
	const CDbHole&	dbHole,
	double			chordal_tol,
	Tchvector*		pts )
{
	CDbTool*	dbTool;

	dbTool = dbHole.Tool();
	if ((dbTool != nullptr) && !dbTool->IsLayer())
	{
		CGeoPoly	geoPoly;
		C2dCoord	pt;
		C3x4Matrix	xform;
		double		orient;
		int			count, indx;
		bool		indexable;

		pt = dbHole.Center();

		dbTool->Convert( &geoPoly );

		// NOTE: Do we have to account for some initial
		// orientation on an indexable punch?
		indexable = (dbTool->IntGet( "Auto_Index", 0 ) != 0);
		if ( indexable )
			orient = dbHole.DoubleGet( "orient", 0. );
		else
			orient = dbTool->DoubleGet( "Index_Angle", 0. );
			
		// Rotate the tool geometry about its origin.
		if (fabs( orient ) > 1.e-2)
		{
			xform.setXYAngle( orient * DEG2RAD );

			geoPoly.Xform( xform );
		}

		// Move the tool geometry to the location of the hole.
		if ((fabs( pt.X() ) > 1.e-3) || (fabs( pt.Y() ) > 1.e-3))
		{
			xform.setUnit();
			xform.Shift( C3dVec( pt.X(), pt.Y(), 0. ) );

			geoPoly.Xform( xform );
		}

		// Tabulate.
		count = geoPoly.Count();
		for (indx = 0; indx < count; ++indx)
		{
			const CGeoCurve& geoCurve = geoPoly[indx];

			if (geoCurve.Type() == GEOARC)
			{
				ArcTabulate( (const CGeoArc&) geoCurve, chordal_tol, pts );
			}
			else
			{
				pts->push_back( new C2dCoord( geoCurve.StartPt() ) );
				pts->push_back( new C2dCoord( geoCurve.EndPt() ) );
			}
		}
	}
}

void CModelUtil::StatisticsGet( const CModel& model, double* cutDistance, int* numPierces )
{
	(*cutDistance) = 0.;
	(*numPierces) = 0;

	// It's unlikely anything will go awry but since nesting relies upon this function ....
	try
	{
		CDbIterator iter;
		iter.Init( model.Db(), DBLINE );

		double travel = 0.;

		while (1)
		{
			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( iter() );
			if (dbCurve == NULL)
				break;

			if ( dbCurve->IsToolpath() )
			{
				CGeoCurve* geoCurve = dbCurve->Curve();
				travel += geoCurve->Length2d();
				delete geoCurve;
			}

			iter.Next();
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		int pierces = 0;

		iter.Init( model.Db(), DBHOLE );

		while (1)
		{
			CDbHole* dbHole = dynamic_cast<CDbHole*>( iter() );
			if (dbHole == NULL)
				break;

			if ( dbHole->IsToolpath() )
				++pierces;

			iter.Next();
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		iter.Init( model.Db(), DBPROFILE );

		while (1)
		{
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( iter() );
			if (dbProfile == NULL)
				break;

			if ( dbProfile->IsToolpath() )
				++pierces;

			iter.Next();
		}

		(*cutDistance) = travel;
		(*numPierces) = pierces;
	}
	catch (...)
	{
	}
}
