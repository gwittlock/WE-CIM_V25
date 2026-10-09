// ==================================================================
//		Repo
//
// ==================================================================

#include "stdafx.h"

#include "StringConst.h"
#include "Register.h"

#include "Command.h"
#include "DbCommand.h"
#include "VarList.h"
#include "Solution.h"

#include "DbWorkplane.h"
#include "DbIterator.h"
#include "DbSequence.h"
#include "DbFeature.h"
#include "DbProfile.h"
#include "DbCurve.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbTool.h"

#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoPoly.h"

#include "Conversion.h"
#include "Profile.h"
#include "Lead.h"
#include "StdCurveFilter.h"
#include "ProfileBuilder.h"
#include "ModelUtil.h"
#include "DisplayEntity.h"
#include "ToolConst.h"  // for enum eHoldType

#include "Repo.h"

#define REPO_ZONE_NAME	"_repo_zone_%d"
#define CLAMP_ZONE_NAME	"_repo_zone_%d"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "DbEntityVisitor.h"

class CZoner : public CDbEntityVisitor
{
public:

	CZoner();

	~CZoner();

	void InitForClamp(
		const C2dBox&		this_clamp,
		const C2dBox&		next_zone,
		const C2dBoxArray&	next_clamps );

	void InitForWorkZone( const C2dBox& this_zone );

	virtual void Visit( CDbEntity* dbEntity );

private:

	void InitCommon(
		const C2dBox*		this_zone,
		const C2dBox*		this_clamp,
		const C2dBox*		next_zone,
		const C2dBoxArray*	next_clamps );

private:

	CDbEntityArray	m_entities;

	const C2dBox*		m_this_zone;
	const C2dBox*		m_this_clamp;

	const C2dBox*		m_next_zone;
	const C2dBoxArray*	m_next_clamps;
};

CZoner::CZoner()
{
	InitCommon( NULL, NULL, NULL, NULL );
}

CZoner::~CZoner()
{
}

void
CZoner::InitForClamp(
	const C2dBox&		this_clamp,
	const C2dBox&		next_zone,
	const C2dBoxArray&	next_clamps )
{
	InitCommon( NULL, &this_clamp, &next_zone, &next_clamps );
}

void
CZoner::InitForWorkZone( const C2dBox& this_zone )
{
	InitCommon( &this_zone, NULL, NULL, NULL );
}

void
CZoner::Visit( CDbEntity* dbEntity )
{
	CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
	if (dbCurve != NULL)
	{
		const C3dBox& box = dbCurve->Box(0);

		if (m_this_clamp != NULL)
		{
			// ASSUMPTION: InitForClamp() must be the use-case.

			// From earlier code:
			// 2006.02.25 (PE) -- With a user-defined clamp_buffer of zero,
			// earlier nesting procedures set clamp_buffer as 2.e-6, causing
			// failures at this point.  Probably safe to simply increase tol.
			if ( m_this_clamp->ContainsXY( box, 1.e-3 ) )
			{
				// The curve is under this clamp. As such, before we decide
				// to introduce a repo-zone (just to access the curve) we
				// must first determine whether the curve can be accessed
				// in another zone.
				if ( m_next_zone->ContainsXY( box, SMALL ) )
				{
					// The curve is in the next zone. Now we must
					// determine if it is obstructed by a clamp.
					int indx, count = m_next_clamps->Count();
					for (indx = 0; indx < count; ++indx)
					{
						const C2dBox* next_box = m_next_clamps->GetAt(indx);
						if ( next_box->ContainsXY( box, 1.e-3 ) )
							break;
					}

					if (indx < count)
					{
						// We must introduce a clamp-zone to access the curve.
						m_entities.Append( dbCurve );
					}
				}
			}
		}
		else
		{
			// ASSUMPTION: InitForWorkZone() must be the use-case.
			if  ( m_this_zone->Contains( box, SMALL ) )
			{
				m_entities.Append( dbCurve );
			}
		}
	}
	else
	{
		CDbHole* dbHole = dynamic_cast<CDbHole*>( dbEntity );
		if (dbHole != NULL)
		{
			// NOTE: It's probably far better to use the punch outline!!!!
			C3dCoord pc = dbHole->Center(0);

			if (m_this_clamp != NULL)
			{
				// ASSUMPTION: InitForClamp() must be the use-case.

				// From earlier code:
				// 2006.11.11 (PE) -- A recent change (introduced to support
				// part_outline generation for punch-only parts) affected the
				// results of bounding box calculations for hole entities.
				// Prior to this change, the bounding box was returned as a
				// tiny box at the center of the hole. Now, instead of comparing
				// bounding boxes, we simply determine which zone contains
				// the hit point of the hole.
				// NOTE: The punch itself may extend beyond the zone, but more
				// critically, we can not violate travel limits.
				if  ( m_this_clamp->Contains( pc, SMALL ) )
				{
					// The hole is under this clamp. As such, before we decide
					// to introduce a repo-zone (just to access the hole) we
					// must first determine whether the hole can be accessed
					// in another zone.
					if ( m_next_zone->Contains( pc, SMALL ) )
					{
						// The hole is in the next zone. Now we must
						// determine if it is obstructed by a clamp.
						int indx, count = m_next_clamps->Count();
						for (indx = 0; indx < count; ++indx)
						{
							const C2dBox* next_box = m_next_clamps->GetAt(indx);
							if ( next_box->Contains( pc, SMALL ) )
								break;
						}

						if (indx < count)
						{
							// We must introduce a clamp-zone to access the hole.
							m_entities.Append( dbHole );
						}
					}
				}
			}
			else
			{
				// ASSUMPTION: InitForWorkZone() must be the use-case.
				if  ( m_this_zone->Contains( pc, SMALL ) )
				{
					m_entities.Append( dbHole );
				}
			}
		}
	}
}

void
CZoner::InitCommon(
	const C2dBox*		this_zone,
	const C2dBox*		this_clamp,
	const C2dBox*		next_zone,
	const C2dBoxArray*	next_clamps )
{
	m_entities.BenignFlush();  // Just in case ....

	m_this_zone = this_zone;
	m_this_clamp = this_clamp;
	m_next_zone = next_zone;
	m_next_clamps = next_clamps;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Prior to V18, nesting would *not* strictly honor the sorting
// order of parts. That is, nesting would always place *oversized*
// parts first, regardless of the sorting order. We use
// g_v17_large_part_pass to enable this legacy behavior.
static int g_v17_large_part_pass;

CRepo::CRepo()
{
	m_clamp_array = NULL;
	m_must_rebuild = false;

	// lpp (ie. Large Part Pass)
	g_v17_large_part_pass = CRegister::BoolGetV( "Nesting", "lpp", false );
}

CRepo::CRepo( 
	CModel*			model, 
	const CString&	config_db, 
	int				autolead_id )
{
	m_clamp_array = NULL;
	m_must_rebuild = false;
	m_model = model;
	m_configdb = config_db;
	m_autoleadid = autolead_id;
}


CRepo::~CRepo()
{
	if (m_clamp_array)
		delete[] m_clamp_array;
}

bool
CRepo::HasClamps() const
{
	return ((m_clamp_array == NULL) ? false : (m_clamp_array->GetSize() > 0));
}

// ==================================================================
//		ExtractZone
//
//	Extract all of the Zone markers from the model.
//
CReturn		
CRepo::ExtractZone( void )
{
	CReturn		ret;

	//
	// Extract all zone markers from the model
	//
	CSelector	select( *m_model );
	select.All( 0 );
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( TRUE );
	select.SystemFlag( FALSE );
	select.SelectAll( TRUE );

	int num = select.Count();
	for (int idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);

		if (db_cmd->IsA(STR_ZONE))
		{
			const CVarList& var = db_cmd->Attrib();
			int idx = var.getInt( "num", 0 );
			if (idx)
			{
				C2dBox	box;

				box.Xmax( var.getReal( STR_XMAX, 0.0 ) );
				box.Ymax( var.getReal( STR_YMAX, 0.0 ) );
				box.Xmin( var.getReal( STR_XMIN, 0.0 ) );
				box.Ymin( var.getReal( STR_YMIN, 0.0 ) );

				idx--;
				m_zone_array.SetAtGrow( idx, box );

//				m_model->EntityDelete( db_cmd->Id() );
			}
		}
	}

	select.Clear();

	return ret;
}

// ==================================================================
//		ExtractClamp
//
//	Extract all of the Clamp markers from the model.
//
CReturn		
CRepo::ExtractClamp(  void )
{
	CReturn		ret;

	if (m_clamp_array)
		delete[] m_clamp_array;

	int size = m_zone_array.GetSize();
	if (!size)
		return ret;

	m_clamp_array = new CArray<C2dBox, C2dBox>[size];

	//
	// Extract all zone markers from the model
	//
	CSelector	select( *m_model );
	select.All( 0 );
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( TRUE );
	select.SystemFlag( FALSE );

	select.SelectAll( TRUE );

	int num = select.Count();
	for (int idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);

		if (db_cmd->IsA("CLAMP"))
		{
			const CVarList& var = db_cmd->Attrib();
			int zone = var.getInt( "repo", 0 );
			int idx = var.getInt( "num", 0 );
			if (idx>=0)
			{
				C2dBox	box;

				const CDbWorkplane* dbWorkplane = db_cmd->Workplane();
				const C3x4Matrix& xform = dbWorkplane->Transform();

				C3dCoord	pt(0,0,0);
				C3dCoord	ptx;
				C3dCoord	ptn;

				pt.X(var.getReal( STR_XMAX, 0.0 ));
				pt.Y(var.getReal( STR_YMAX, 0.0 ));
				xform.TransformTo( pt, &ptx );

				pt.X(var.getReal( STR_XMIN, 0.0 ));
				pt.Y(var.getReal( STR_YMIN, 0.0 ));
				xform.TransformTo( pt, &ptn );

				box.Xmax( max( ptn.X(), ptx.X() ) );
				box.Ymax( max( ptn.Y(), ptx.Y() ) );
				box.Xmin( min( ptn.X(), ptx.X() ) );
				box.Ymin( min( ptn.Y(), ptx.Y() ) );

				idx--;
				zone--;
				m_clamp_array[zone].SetAtGrow( idx, box );

//					model->EntityDelete( db_cmd->Id() );
			}
		}
	}

	select.Clear();

	return ret;
}


// ==================================================================
//		explode_profiles
//
//	Any profiles in this (recursive) feature get eliminated, with their
//	contents "bubbled-up" to the containing feature
//
CReturn
CRepo::explode_profiles( 
	CDbFeature*		db_feat,
	CDbFeature*		skip_feat,
	C2dBox*			skipbox,
	C2dBox*			clampbox )
{
	CReturn	ret;

	// When exploding profiles, strip out lead features.
	if ( db_feat->IsLead() )
		return ret;

	db_feat->IntSet( "rebuild", 1 );
	m_must_rebuild = true;

//	int num = db_feat->Count();
//	for (int idx=0; idx<num; idx++)
	int num = db_feat->Count()-1;
	for (int idx=num; idx>=0; idx--)
	{
		CDbEntity*	db_ent = (*db_feat)[idx];
		
		CDbFeature*	db_subfeat = dynamic_cast<CDbFeature*>(db_ent);
		if (db_subfeat)
		{
			ret += explode_profiles( db_subfeat, skip_feat, skipbox, clampbox );
			continue;
		}

		CDbProfile*	db_prof = dynamic_cast<CDbProfile*>(db_ent);
		if (db_prof)
		{
			if ( skipbox )
			{
				C2dBox prof_box = db_prof->Box(0);
				if ( skipbox->Contains( prof_box, SMALL ) &&
					 clampbox->Intersects( prof_box, SMALL ) )
				{ 
					// 2006.11.13 (PE) -- At this point, it appears that we can
					// move the part into the clamp zone. However, we must first
					// check that the part does not intersect a repositioned clamp.
					if ( !PostRepoInterference( prof_box ) )
					{
						if (skip_feat)
							skip_feat->Append(db_prof);

						continue; 
					}
					else
					{
						// We want to fall through so the part gets split.
					}
				}
			}

			//
			//
			CProfile prof;
			CDbWorkplane* profWork = db_prof->Workplane();
			CConversion::Convert( profWork, db_prof, &prof );
			int winding = SGN(prof.Area());

			int depth = db_prof->IntGet( STR_PROFILE_DEPTH, 0 );

			db_prof->Disassociate();

			int pnum = db_prof->Count();
			for (int pidx=0; pidx<pnum; pidx++)
			{
				db_ent = (*db_prof)[0];

				db_ent->IntSet( "winding", winding );
				db_ent->IntSet( STR_PROFILE_DEPTH, depth );

				db_prof->Disown( db_ent );

				db_feat->InsertBefore( db_prof, db_ent );
			}

			db_prof->Delete();
			db_prof->ModifyFlag( true );
		}
	}

	return ret;
}


// ==================================================================
//		split_profiles
//
//	Actually, expects all profiles to have been eliminated with
//	explode_profiles prior to calling.
//
//	Takes all geometry in the (recursive) feature and tries to split
//	it against the array of split lines
//
CReturn
CRepo::split_profiles( 
	CDbFeature*		db_feat,
	CGeoCurveArray&	split_array,
	C2dBox*			inhibit,
	bool*			did_split )
{
	// CRITICAL: Because split_profiles() is recursive.
	static int call_depth = 0;

	CReturn	status;

	if (call_depth == 0)
		(*did_split) = false;

	++call_depth;

	for (int idx=0; idx<db_feat->Count(); idx++)
	{
		CDbEntity*	db_ent = (*db_feat)[idx];
		
		CDbFeature*	db_subfeat = dynamic_cast<CDbFeature*>(db_ent);
		if (db_subfeat)
		{
			status += split_profiles( db_subfeat, split_array, inhibit, did_split );
			continue;
		}

		CDbCurve*	db_curve = dynamic_cast<CDbCurve*>(db_ent);
		if ( IsSplittable( db_curve ) )
		{
			CGeoCurve*	geo_curve = db_curve->Curve(0);
			C3dCoord	intpt[2];

			// Try to split this curve against all of the split lines
			int snum = split_array.Count();
			for (int sidx=0; sidx<snum; sidx++)
			{
				int inum = CSolution::Intersect( *geo_curve, *split_array[sidx], TRUE, intpt );

				if (inhibit && !inhibit->Intersects( db_curve->Box(0), SMALL ))
					inum = 0;

				if (inum > 0)
				{
					for (int iidx=0; iidx<inum; iidx++)
					{
						// If two splits, will they come out right?  I don't know.
						C3dCoord local_pt;
						db_curve->Workplane()->Inverse().TransformTo(intpt[iidx], &local_pt);
						CDbEntity* db_split = db_curve->Split( local_pt, 0.0 );

						(*did_split) = true;

						if (db_split != NULL)
						{
							// -1   lead    1 -1   trail   1
							// --------------*--------------
							db_curve->IntSet( "_split",  1 );
							db_split->IntSet( "_split", -1 );
						}

						if (inhibit)
						{
							if (db_split)
							{
								if (inhibit->Contains( db_split->Box(0), SMALL ))
									db_split->IntSet( "dropskip", 0 );
								else
									db_split->IntSet( "dropskip", 1 );
							}

							if (db_curve)
							{
								if (inhibit->Contains( db_curve->Box(0), SMALL ))
									db_curve->IntSet( "dropskip", 0 );
								else
									db_curve->IntSet( "dropskip", 1 );
							}
						}
					}
				}
			}

			delete geo_curve;
		}
	}

	--call_depth;

	return status;
}


// ==================================================================
// rebuild_profiles
//
CReturn
CRepo::rebuild_profiles(
	CDbFeature*	src_feature,
	CDbFeature*	dst_feature,
	C3dBox&		selbox )
{
	CReturn ret;
	CDbEntity*		db_ent;
	CDbFeature*		sub_feat;
	CDbHole*		dbHole;
	CDbCurve*		db_curve;
	CDbContainer*	owner;
	bool			is_contained;

	//
	// Now, select everything in the feature that lies fully within the 
	//	zone and turn it into profiles.
	//
	CDbCurveList	src_curves;
	for (int fidx=src_feature->Count()-1; fidx>=0; fidx--)
	{
		db_ent = (*src_feature)[fidx];

		sub_feat = dynamic_cast<CDbFeature*>(db_ent);

		if ((sub_feat != NULL) && (sub_feat != dst_feature))
		{
			ret += rebuild_profiles( sub_feat, dst_feature, selbox );
			continue;
		}

		dbHole = dynamic_cast<CDbHole*>( db_ent );
		if (dbHole == NULL)
		{
			// 2006.02.25 (PE) -- With a user-defined clamp_buffer of zero,
			// earlier nesting procedures set clamp_buffer as 2.e-6, causing
			// failures at this point.  Probably safe to simply increase tol.
			//    if (!selbox.ContainsXY( db_ent->Box(0), SMALL ))
			is_contained = selbox.ContainsXY( db_ent->Box(0), 1.e-3 );
		}
		else
		{
			// 2006.11.11 (PE) -- A recent change (introduced to support
			// part_outline generation for punch-only parts) affected the
			// results of bounding box calculations for hole entities.
			// Prior to this change, the bounding box was returned as a
			// tiny box at the center of the hole. Now, instead of comparing
			// bounding boxes, we simply determine which zone contains
			// the hit point of the hole.
			// NOTE: The punch itself may extend beyond the zone, but more
			// critically, we can not violate travel limits.
			C3dCoord pc = dbHole->Center(0);
			is_contained = selbox.Contains( pc, SMALL );
		}

		if ( !is_contained )
			continue;

		db_curve = dynamic_cast<CDbCurve*>(db_ent);
		if (!db_curve)
		{
			// We have a point or hole or something... 
			// it's in the zone, so do it.
			// TODO:  find a clever logic way to eliminate this cut&pasted append code
			dst_feature->Append( db_ent );

			continue;
		}

		owner = dynamic_cast<CDbContainer*>(db_curve->Owner());
		if (owner)
			owner->Disown( db_curve );

		src_curves.Append( db_curve );
	}

	// Profile growth code stolen, er, copied, er, inspired by Profile:Selected:
	CStdCurveFilter	filter;
	CDbCurveList	dst_curves;
	CDbCurve*		db_seed;

	while (TRUE)
	{
		if (filter.Curves().Count() < 1)
		{
			if (src_curves.Count() < 1)
				break;	// Done!

			db_seed = src_curves.Remove(0);
			filter.Init( &src_curves, db_seed );
		}
		else
		{
			db_seed = filter.Curves().Remove( 0 );
		}

		ret += CProfileBuilder::ProfileGrow( db_seed, SMALL, &(filter.Curves()), &dst_curves );
		if ( !ret.IsOk()
			|| (dst_curves.Count() < 1) )
			break;

		CDbProfile* db_prof = NULL;
		ret += m_model->EntityCreate( DBPROFILE, (CDbEntity**)&db_prof );

		db_prof->Append( dst_curves );

		//
		// Find a winding value to set into the profile
		// Also bubble-up the profile depth and dropskip flags.
		//
		int dnum = db_prof->Count();
		bool wound = FALSE;
		bool dropped = FALSE;
		for (int didx=0; didx<dnum; didx++)
		{
			db_ent = (*db_prof)[didx];
			if (!wound)
			{
				int depth = db_ent->IntGet( STR_PROFILE_DEPTH, 0 );
				db_prof->IntSet( STR_PROFILE_DEPTH, depth );

				int winding = db_ent->IntGet( "winding", 0 );
				if (winding)
				{
					wound = TRUE;
					db_prof->IntSet( "winding", winding );
				}
			}

			if (!dropped)
			{
				int dropskip = db_ent->IntGet( "dropskip", 0 );
				if (dropskip)
				{
					dropped = TRUE;
					db_prof->IntSet( "dropskip", dropskip );
				}
			}

			if (wound && dropped)
				break;
		}

		//
		// Now, these curves used to belong in a feature; and may have been toolpath.
		// Recreate this feature...
		//
		dst_feature->Append( db_prof );


		// Cleanup
		dst_curves.BenignFlush();

	} // end While TRUE

	return ret;
}

// ==================================================================
//		clean_empty
//
//	Do housekeeping; remove empty containers
CReturn
CRepo::clean_empty( void )
{
	CReturn		ret;

	CDbIterator iter;

	//
	// Simple empty profiles
	//
	iter.Init( m_model->Db(), DBPROFILE );
	while (1)
	{
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		if (dbProfile->Count() < 1)
			dbProfile->Delete();

		iter.Next();
	}

	//
	// Empty features -- empty of *geometry* (commands don't count to fullness)
	//
	iter.Init( m_model->Db(), DBFEATURE );
	while (1)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if (is_feature_empty(dbFeature))
		{ dbFeature->Delete(); }

		iter.Next();
	}

	return ret;
}

// ==================================================================
//	See if the feature is empty.  If it has sub-features, it is
// empty if the sub-features are empty.  Whee.
//
bool CRepo::is_feature_empty(CDbFeature* db_feat)
{
	int num = db_feat->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*db_feat)[idx];

		switch (db_ent->Type())
		{
			case DBFEATURE:
				if (!is_feature_empty((CDbFeature*)db_ent))
				{ return false; }
				break;

			case DBCOMMAND:
			{
				CDbCommand* cmd = dynamic_cast<CDbCommand*>(db_ent);
				if ( cmd->IsInstance() )
				{
					return false;
				}
			}
			break;

			default:
				return false;
				break;
		}
	}

	return true;
}

#if ORIGINAL_CODE
// ==================================================================
//		Rebuild
//
//	Find geometry that fits in the relevant rectangular area, build
//	it up into profiles, and auto-lead it... tucking it into the 
//	proper feature in the process.
//
//	Note:  rebuild_repo_profiles, extend for clamps, including
//			capture_geometry.
//
CReturn
CRepo::Rebuild( double clamp_buffer )
{
	CReturn	status;

	// We are only rebuilding "part" features
	// TODO:  Change this to top-level features, w/o owners?
	CSelector select( *m_model );
	select.All( 0 );
	select.Filter( DBFEATURE, 1 );
	select.Restrictions( TRUE );
	select.SelectAll( TRUE );

	int count = select.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbFeature*	dbFeature = dynamic_cast<CDbFeature*>(select[indx]);
		if ( IsRepoPart( dbFeature ) )
		{
			dbFeature->AttribDelete( "rebuild" );

			RebuildClampedPart( dbFeature, clamp_buffer );
			RebuildZonedPart( dbFeature );
		}
	}

	return status;
}

// Rebuild the contents of each clamp in each zone
// (which pulls geometry out of the zone.
CReturn
CRepo::RebuildClampedPart( CDbFeature* dbFeature, double clamp_buffer )
{
	CReturn	status;

	int fzone = dbFeature->IntGet( STR_ZONE, ZONE_NONE );

	int is_large_part = (g_v17_large_part_pass
					  ? (fzone == LARGE_PART_ZONE)
					  : dbFeature->IntGet( "_large", 0 ));

	int zcnt = m_zone_array.GetSize();
	for (int zidx = 0; zidx < zcnt; ++zidx)
	{
		if ((fzone == ZONE_NONE) || (fzone == zidx) || is_large_part)
		{
			if (m_clampfeat_array.GetSize() > zidx)
			{
				// Create a beta sub-zone to hold geometry under the clamps
				CDbFeature* clamp_zone = m_clampfeat_array[zidx];

				// Foreach clamp in that zone...
				int ccnt = m_clamp_array[zidx].GetSize();
				for (int cidx = 0; cidx < ccnt; ++cidx)
				{
					C2dBox clamp = (m_clamp_array[zidx])[cidx];

					clamp += clamp_buffer;

					C3dBox selbox( clamp.Xmin(), clamp.Ymin(), -1e6,
									clamp.Xmax(), clamp.Ymax(), 1e6 );

					status += rebuild_profiles( dbFeature, clamp_zone, selbox );
				}
			}
		}
	}

	return status;
}

// Rebuild the contents of each zone now.
CReturn
CRepo::RebuildZonedPart( CDbFeature* dbFeature )
{
	CReturn	status;

	int fzone = dbFeature->IntGet( STR_ZONE, ZONE_NONE );

	int is_large_part = (g_v17_large_part_pass
					  ? (fzone == LARGE_PART_ZONE)
					  : dbFeature->IntGet( "_large", 0 ));

	int zcnt = m_zone_array.GetSize();
	for (int zidx = 0; zidx < zcnt; ++zidx)
	{
		if ((fzone == ZONE_NONE) || (fzone == zidx) || is_large_part)
		{
			CDbFeature* work_zone = m_zonefeat_array[zidx];

			C2dBox zone = m_zone_array[zidx];

			C3dBox selbox( zone.Xmin(), zone.Ymin(), -1e6,
							zone.Xmax(), zone.Ymax(), 1e6 );

			status += rebuild_profiles( dbFeature, work_zone, selbox );
		}
	}

	return status;
}

#else

CReturn
CRepo::Rebuild( double clamp_buffer )
{
	CReturn	status;

	// We are only rebuilding "part" features
	// TODO:  Change this to top-level features, w/o owners?
	CSelector select( *m_model );
	select.All( 0 );
	select.Filter( DBFEATURE, 1 );
	select.Restrictions( TRUE );
	select.SelectAll( TRUE );

	int count = select.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbFeature*	dbFeature = dynamic_cast<CDbFeature*>(select[indx]);
		if ( IsRepoPart( dbFeature ) )
		{
			dbFeature->AttribDelete( "rebuild" );

			RebuildClampedPart( dbFeature, clamp_buffer );
			RebuildZonedPart( dbFeature );
		}
	}

	return status;
}

// Rebuild the contents of each clamp in each zone
// (which pulls geometry out of the zone).
CReturn
CRepo::RebuildClampedPart( CDbFeature* dbFeature, double clamp_buffer )
{
	CReturn	status;

	int fzone = dbFeature->IntGet( STR_ZONE, ZONE_NONE );

	int is_large_part = (g_v17_large_part_pass
					  ? (fzone == LARGE_PART_ZONE)
					  : dbFeature->IntGet( "_large", 0 ));

	int zcnt = m_zone_array.GetSize();
	for (int zidx = 0; zidx < zcnt; ++zidx)
	{
		if ((fzone == ZONE_NONE) || (fzone == zidx) || is_large_part)
		{
			if (m_clampfeat_array.GetSize() > zidx)
			{
				// Create a beta sub-zone to hold geometry under the clamps
				CDbFeature* clamp_zone = m_clampfeat_array[zidx];

				// Foreach clamp in that zone...
				int ccnt = m_clamp_array[zidx].GetSize();
				for (int cidx = 0; cidx < ccnt; ++cidx)
				{
					C2dBox clamp = (m_clamp_array[zidx])[cidx];

					clamp += clamp_buffer;

					C3dBox selbox( clamp.Xmin(), clamp.Ymin(), -1e6,
									clamp.Xmax(), clamp.Ymax(), 1e6 );

					status += rebuild_profiles( dbFeature, clamp_zone, selbox );
				}
			}
		}
	}

	return status;
}

// Rebuild the contents of each zone now.
CReturn
CRepo::RebuildZonedPart( CDbFeature* dbFeature )
{
	CReturn	status;

	int fzone = dbFeature->IntGet( STR_ZONE, ZONE_NONE );

	int is_large_part = (g_v17_large_part_pass
					  ? (fzone == LARGE_PART_ZONE)
					  : dbFeature->IntGet( "_large", 0 ));

	int zcnt = m_zone_array.GetSize();
	for (int zidx = 0; zidx < zcnt; ++zidx)
	{
		if ((fzone == ZONE_NONE) || (fzone == zidx) || is_large_part)
		{
			CDbFeature* work_zone = m_zonefeat_array[zidx];

			C2dBox zone = m_zone_array[zidx];

			C3dBox selbox( zone.Xmin(), zone.Ymin(), -1e6,
							zone.Xmax(), zone.Ymax(), 1e6 );

			status += rebuild_profiles( dbFeature, work_zone, selbox );
		}
	}

	return status;
}
#endif


// ==================================================================
//		WrapProfiles
//
//	Given a sheet full of geometry, package that geometry into "_part"
//	features.  Note that there may be parts-in-parts.
//
//	1. Collect a list of all interior profiles and misc. geometry
//	2. Collect a list of all exterior profiles
//
//	3.	Foreach interior profile or entity
//	4.		Find the list of exterior profiles that contain it
//	5.		Foreach (candidate) containing profile
//	6.			Foreach other (test) containing profile
//	7.				If the candidate contains the test profile, reject the candidate
//	8.		There will be one remaining candidate
//	9.		If that candidate is parentless, create a "_part" feature and put it in
//	10.		Put the interior profile or entity into the candidate's parent feature
//
// This is gross and ugly; I had trouble wrapping my brain around what should have
//	been a fairly simple task.
// TODO: clean up
// TODO: Break out into helper methods
// TODO:  HUGE FUCKING issues wrt. the tool feature, ownership, splitting tools across
//		parts, and so forth.  Probably NOT suitable for multi-part use yet. REDO.
CReturn 
CRepo::WrapProfiles( void )
{
	CReturn		ret;

	CDbIterator iter;

	CDbWorkplane*	workplane = NULL;
	m_model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = m_model->ActiveWorkplane();

	//
	// Pre-process profiles, to find depth of nesting
	//
	CDbEntityList	proflist;

	iter.Init( m_model->Db(), DBPROFILE );
	while (TRUE)
	{
		CDbProfile* db_prof = dynamic_cast<CDbProfile*>( iter() );
		if (!db_prof)
			break;
		iter.Next();

		if (db_prof->Tool())
			proflist.Append( db_prof );
	}

	ret += CModelUtil::MarkIntExt( &proflist, FALSE, m_model );

	//
	// Take a tour through the database.  Collect external profiles in one list,
	//	and internal profiles plus not-in-profile geometry in another list.
	//	Only care about tooled profiles.
	//
	CDbEntityList	db_external;
	CDbEntityList	db_internal;

	iter.Init( m_model->Db(), DBLINE );
	while (TRUE)
	{
		CDbEntity* db_ent = iter();
		if (!db_ent)
			break;
		iter.Next();

		// Terminate search after profiles
		if (db_ent->Type() > DBPROFILE)
			break;

		if (!db_ent->Tool())
			continue;

		CDbProfile*	db_prof = dynamic_cast<CDbProfile*>(db_ent);
		if (db_prof)
		{
			int depth = db_prof->IntGet( STR_PROFILE_DEPTH, 0 );
			if ( (depth & 0x01) == 0)
			{
				// Even profiles are exterior
				db_external.Append( db_ent );
			}
			else
			{
				// Odd profiles are interior
				db_internal.Append( db_ent );
			}
		}
		else // NOT a profile
		{
			CDbProfile*	db_parent = dynamic_cast<CDbProfile*>(db_ent->Owner());

			if (!db_parent)
			{
				// A loner; add it in.
				db_internal.Append( db_ent );
			}
		}
	}

	//
	// Make sure that each external profile lives in a "_part" feature
	//
	int pidx, pnum = db_external.Count();
	for (pidx=0; pidx<pnum; pidx++)
	{
		CDbProfile*	db_prof = (CDbProfile*)db_external[pidx];
		CDbEntity* db_owner = db_prof;
		while (db_owner->Owner())
			db_owner = db_owner->Owner();

		CDbFeature* db_feature = dynamic_cast<CDbFeature*>(db_owner);
		bool part_feature = FALSE;
		if (db_feature)
		{
			CString type = db_feature->StringGet( STR_TYPE, "<error>" );
			if (type.CompareNoCase( "_part" ) == 0)
				part_feature = TRUE;
		}
		if (!part_feature)
		{
			// Create a new part feature here...
			m_model->EntityCreate( DBFEATURE, (CDbEntity**)&db_feature );

			CString name;
			name.Format( "_part_Automatic_%d", db_feature->Id() );
			name = db_feature->NameConvert( name );
			db_feature->SystemName( name );

//			db_feature->StringSet( "Name", name );
			db_feature->StringSet( STR_TYPE, "_part" );

			CDbEntity* db_ent = db_prof;
			while (db_ent)
			{
				db_ent = dynamic_cast<CDbContainer*>(db_ent->Owner());
				if (db_ent)
				{
					CDbFeature* db_toolfeat = dynamic_cast<CDbFeature*>(db_ent);
					if ( db_toolfeat
						&& db_toolfeat->Tool() )
					{
						CDbContainer* owner = (CDbContainer*)db_toolfeat->Owner();
						if ( owner
							&& (owner != db_feature) )
						{
							owner->Disown( db_toolfeat );
						}

						db_feature->Append( db_toolfeat );
						db_ent = NULL;
					}
				}
			}
		}
	}

	//
	// Create poly (testable) versions of the exterior profiles...
	//
	pnum = db_external.Count();
	CGeoPoly* poly_array = new CGeoPoly[pnum];
	for (pidx=0; pidx<pnum; pidx++)
	{
		CDbProfile*	db_prof = (CDbProfile*)db_external[pidx];
		CGeoPoly& poly = poly_array[pidx];

		int elnum = db_prof->Count();
		for (int elidx=0; elidx<elnum; elidx++)
		{
			CGeoCurve* curve = ((CDbCurve*)(*db_prof)[elidx])->Curve(0);

			poly.CopyAppend( *curve );

			delete curve;
		}
	}

	//
	// Process each interior element
	//
	int inum = db_internal.Count();
	for (int iidx=0; iidx<inum; iidx++)
	{
		CDbEntity*	db_ent = db_internal[iidx];
		C3dCoord ipt;
		switch (db_ent->Type())
		{
			case DBHOLE:
				ipt = ((CDbHole*)db_ent)->Coord();
				break;

			case DBLINE:
				ipt = ((CDbLine*)db_ent)->StartPt();
				break;

			case DBARC:
				ipt = ((CDbArc*)db_ent)->StartPt();
				break;

			case DBPROFILE:
				ipt = ((CDbCurve*) (* (CDbProfile*)(db_ent) )[0] )->StartPt();
				break;

			default:
				continue;
		}

		//
		// Build a list of exterior profiles that contain this element
		//
		CDbEntityList db_contain;

		int xnum = db_external.Count();
		for (int xidx=0; xidx<xnum; xidx++)
		{
			bool encloses = poly_array[xidx].PtInPoly( ipt );//, 0.0 );
			if (encloses)
				db_contain.Append( db_external[xidx] );
		}

		//
		// Now, process all of these candidates to find the one true container
		//
		int cnum = db_contain.Count()-1;
		for (int cidx=cnum; cidx>=0; cidx--)
		{
			// test against all others for containment...
			for (int tidx=cidx-1; tidx>=0; tidx--)
			{
				bool encloses = poly_array[cidx].PtInPoly( poly_array[tidx][0].StartPt() );//, 0.0 );
				if (encloses)
				{
					db_contain.Remove( cidx );
					cnum--;
					break;
				}
			}
		}

		//
		// Should be left with exactly one candidate profile
		//
		if (cnum < 0)
			continue;
		CDbProfile*	db_part = (CDbProfile*)db_contain[0];
		CDbEntity* db_owner = db_part;
		while (db_owner->Owner())
			db_owner = db_owner->Owner();

		CDbFeature* db_feature = dynamic_cast<CDbFeature*>(db_owner);

		//
		// Add the internal entity to the feature... after backing
		//	up to it's parent toolpath feature.  We KNOW we are tooled...
		// IT#136, losing the tooling
		//
		CDbEntity*	slave = db_internal[iidx];
		if (slave)
		{
			slave = dynamic_cast<CDbContainer*>(slave->Owner());
			if (slave)
			{
				CDbFeature* db_toolfeat = dynamic_cast<CDbFeature*>(slave);
				if ( db_toolfeat
					&& db_toolfeat->Tool() )
				{
					CDbContainer* owner = (CDbContainer*)db_toolfeat->Owner();
					if (owner != db_feature)
					{
						if ( owner )
							owner->Disown( db_toolfeat );

						db_feature->Append( db_toolfeat );
					}
				}
			}
		}

		// Tidy up...
		db_contain.BenignFlush();

	} // end for iidx

	// Tidy up...
	db_internal.BenignFlush();
	db_external.BenignFlush();

	return ret;
}



// ==================================================================
//		EnableCommands
//
//	Turns on all the commands; removes the system flag so 
//	they are visible again.
//
CReturn		
CRepo::EnableCommands( void )
{
	CReturn		ret;

	//
	// Cleanup all commands from the model
	//
	CSelector	select( *m_model );
	select.All( 0 );
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( TRUE );
	select.SystemFlag( FALSE );
	select.SelectAll( TRUE );

	int num = select.Count();
	for (int idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);
		CString		text = db_cmd->Text();

		if (text[0] == '@' )
		{
			db_cmd->SystemFlag( false );
		}
	}

	select.Clear();

	return ret;
}




// ==================================================================

CReturn
CRepo::Cleanup( void )
{
	CReturn ret;
	CDbIterator iter;

	//
	// Do the actual cleaning
	// TODO: Merge with overlap in ModelUtil::EmptyContainers() ?
	//
	ret += clean_empty();
	//
	// Now, order and compress the zones into a series...
	//
	ret += compress_zones();

	// Finally, strip the "winding" attribute off of all relevant database entities
	// (retains it for profiles and above)
	//
	iter.Init( m_model->Db(), DBLINE );
	while (TRUE)
	{
		CDbEntity*	db_ent = iter();
		if (db_ent == NULL)
			break;
		iter.Next();

		if (db_ent->Type() > DBPROFILE)
			break;

//		db_ent->AttribDelete( "winding" );		// This is needed by AutoOpen() later.
		db_ent->AttribDelete( "Profile Depth" );
		db_ent->AttribDelete( "dropskip" );
	}

	return ret;
}


// ==================================================================

CReturn
CRepo::ShiftPierce( double torch_offset )
{
	CReturn ret;

	if (!m_zone_array.GetSize())
		return ret;

	double punch_offset = -torch_offset; // NOT USED
	// TODO:  Be picky about burn vs. torch zones, or ignore this method
	// entirely for new repo.

	CSelector	select( *m_model );
	select.All( 0 );
	select.Filter( DBHOLE, 1 );
	select.Restrictions( TRUE );
	select.SelectAll( TRUE );

	int num = select.Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbHole* hole = dynamic_cast<CDbHole*>(select[idx]);

		// Is it a nested part?
		if (hole && hole->IsPierce())
		{
			// Rebuild the contents of each clamp in each zone (which pulls
			//	geometry out of the zone
			int znum = m_zone_array.GetSize();
			for (int zidx=0; zidx<znum; zidx++)
			{
				// The loops are almost certainly the hard-way around; should go zone
				// first, then hole?  Whatever...
				//
				CDbFeature* db_zone = m_zonefeat_array[zidx];
				C2dBox		zone = m_zone_array[zidx];

				C3dBox	selbox( zone.Xmin(), zone.Ymin(), -1e6,
								zone.Xmax(), zone.Ymax(), 1e6 );

				if (selbox.Contains( hole->Coord(0), SMALL ))
				{
					CDbContainer* owner = dynamic_cast<CDbContainer*>(hole->Owner());
					if (owner)
					{ owner->Disown(hole); }
					db_zone->Append(hole);
					break;

				}

			} // end for zidx
		}
	} //end for idx (feature)

	return ret;
}

// ==================================================================
//	deselect_part_holes
//
//	For all holes that are inside a _part feature, de-select them
//
void
CRepo::deselect_part_holes( 
	CSelector* select )
{
	int num = select->Count()-1;
	for (int idx=num; idx>=0; idx--)
	{
		CDbEntity*	db_ent = (*select)[idx];

		// Scan up looking for a _part owner feature
		if (db_ent->Type() == DBHOLE )
		{
			CDbContainer* db_owner = dynamic_cast<CDbContainer*>(db_ent->Owner());
			while (db_owner)
			{
				if (db_owner->Type() == DBPATTERN)
				{
					select->Remove(db_ent->Id());
					break;
				}
				else
				{
					CDbFeature* feature = dynamic_cast<CDbFeature*>(db_owner);
					if (feature)
					{
						CString type = feature->StringGet( STR_TYPE, "<error>" );

						if (type.CompareNoCase( "_part" ) == 0)
						{
							select->Remove(db_ent->Id());
							break;
						}

					} // end if feature
				}

				db_owner = dynamic_cast<CDbContainer*>(db_owner->Owner());

			} // end while owner

		} // end if DBHOLE

	} // end for idx
}



// ==================================================================
//		PackageInstances
//
//	Take a nest and organize the instances into zones.  At this point
//	in the cycle, the ENTIRE NEST must be in the form of Instances of
//	patterns.
//
//	This is where controlling commands are deleted and their attributes
//	transferred to the zone feature.
//
CReturn		
CRepo::PackageInstances()
{
	CReturn		ret;

	CDbWorkplane*	workplane = NULL;
	m_model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = m_model->ActiveWorkplane();

	//
	// Create a set of zone features
	// TODO:  Make a separate method?
	//
	int znum = m_zone_array.GetSize();
	CDbFeature* prev_db_zone = NULL;
	C2dBox	prev_zone;
	for (int zidx=0; zidx<znum; zidx++)
	{
		C2dBox	zone = m_zone_array[zidx];

		if (!zone.IsDefined())
			continue;
		//
		// Create the repo-zone feature
		//
		CDbFeature* db_zone = NULL;
		m_model->EntityCreate( DBFEATURE, (CDbEntity**)&db_zone );

		CString name;
		name.Format( REPO_ZONE_NAME, zidx+1 );
		name = db_zone->NameConvert( name );
		db_zone->SystemName( name );

		// Zone numbers are [1..N] to make VB-side easier.
		db_zone->Owner( NULL );
		db_zone->IntSet( "_zone_num", (zidx+1) );

		m_zonefeat_array.SetAtGrow( zidx, db_zone );

		//	Some attributes...
		CVarList* var = db_zone->pAttrib();
		var->setString( STR_TYPE, "_zone" );
		var->setReal( "_zone_left", zone.Xmin() );
		var->setReal( "_zone_top", zone.Ymin() );
		var->setReal( "_zone_right", zone.Xmax() );
		var->setReal( "_zone_bottom", zone.Ymax() );

		//	A REPO command...
		if ((zone.Xmin() > SMALL) && prev_db_zone)
		{
			prev_db_zone->DoubleSet( "_zone_repo", zone.Xmin() - prev_zone.Xmin() );
		}

		prev_db_zone = db_zone;
		prev_zone = zone;
	}
	// ------------------------------------------------------------------------
	// Now capture all commands and decide their fate
	//
	CSelector	select( *m_model );
	select.All( 0 );
	select.Clear();		// This should be unnecessary
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( TRUE );
	select.SystemFlag( FALSE );
	select.SelectAll( TRUE );

	int num = select.Count();
	for (int idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);
		const CVarList& var = db_cmd->Attrib();

		int repoidx = -1;
		if (db_cmd->IsA(STR_ZONE))
		{
			repoidx = var.getInt( "num", -1 ) - 1;
		}
		else if ( db_cmd->IsA("CLAMP") ||
				  db_cmd->IsA("HOLD") ||
				  db_cmd->IsA("INSTANCE") )
		{
			repoidx = var.getInt( "repo", -1 ) - 1;
		}

		if ((repoidx >= 0) && (repoidx < m_zonefeat_array.GetSize()))
		{
			CDbFeature* feat = m_zonefeat_array[repoidx];
			CVarList* feat_var = feat->pAttrib();

			C2dCoord origin( db_cmd->Coord().X(), db_cmd->Coord().Y() );

			CString varname;
			if (db_cmd->IsA("CLAMP"))
			{
				int num = var.getInt("num", 0 );
				int clampnum = max( num, feat_var->getInt( "_clamp_num", 0 ) );
				int clampCount = m_model->Header().getInt( "Number_Of_Clamps", 0 );

				feat_var->setInt( "_clamp_num", clampnum );

				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				// Set clamp info in header to simplify VB
				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				if (repoidx == 0)
				{
					if (clampnum > clampCount)
						m_model->pHeader()->setInt( "Number_Of_Clamps", clampnum );

					varname.Format( "Clamp%dPos", num );
					m_model->pHeader()->setReal( varname, origin.X() );
				}
				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

				varname.Format( "_clamp%d_x", num );
				feat_var->setReal( varname, origin.X());

				varname.Format( "_clamp%d_y", num );
				feat_var->setReal( varname, origin.Y() );
				//
				// Yes, these get set redundantly.  That's okay.
				//
				//                                  TR(xmax,ymax)
				//                 ---------------*
				//                 |              |
				//                 |              |
				//                 |              |
				//                 *---------------
				//   LL(xmin,ymin)
				//
				//
				//
				feat_var->setReal( "_clamp_lldx", var.getReal( "xmin", 0.0 ) - origin.X() );
				feat_var->setReal( "_clamp_lldy", var.getReal( "ymin", 0.0 ) - origin.Y() );
				feat_var->setReal( "_clamp_trdx", var.getReal( "xmax", 0.0 ) - origin.X() );
				feat_var->setReal( "_clamp_trdy", var.getReal( "ymax", 0.0 ) - origin.Y() );

				db_cmd->Delete();
			}
			else if (db_cmd->IsA("HOLD"))
			{
				eHoldType type = (eHoldType) var.getInt( STR_TYPE, IUNDEFINED );
				feat_var->setInt( "_hold", type );

				int zidx = var.getInt( "repo", 1 )-1;
				C2dBox zone = m_zone_array[zidx];

#if BEFORE_V18_0
				feat_var->setReal( "_hold_x", origin.X()-zone.Xmin() );

				// The Hold value is already relative to zone start
				feat_var->setReal( "_hold_x", origin.X() );
				feat_var->setReal( "_hold_y", origin.Y() );
#else
				// 2006.09.27 (PE) -- According to conversation with Gary,
				// the coordinate of a hold position is treated just like
				// the coordinate of a hole. This kinda makes sense because
				// the hold down mechanism typically travels with the head.
				// So the previous implementation was wrong (for a very long time).
				feat_var->setReal( "_hold_x", origin.X() );
				feat_var->setReal( "_hold_y", origin.Y() );
#endif

				switch (type)
				{
				case HOLD_2CIRCLE:
					feat_var->setReal( "_hold_dia", var.getReal( "dia", 0.0 ) );

					feat_var->setReal( "_hold_c1dx", var.getReal( "c1x", 0.0 ) - origin.X() );
					feat_var->setReal( "_hold_c1dy", var.getReal( "c1y", 0.0 ) - origin.Y() );

					feat_var->setReal( "_hold_c2dx", var.getReal( "c2x", 0.0 ) - origin.X() );
					feat_var->setReal( "_hold_c2dy", var.getReal( "c2y", 0.0 ) - origin.Y() );
					break;

				case HOLD_RECTANGLE:
					feat_var->setReal( "_hold_tldx", var.getReal( "xmin", 0.0 ) - origin.X() );
					feat_var->setReal( "_hold_tldy", var.getReal( "ymin", 0.0 ) - origin.Y() );
					feat_var->setReal( "_hold_brdx", var.getReal( "xmax", 0.0 ) - origin.X() );
					feat_var->setReal( "_hold_brdy", var.getReal( "ymax", 0.0 ) - origin.Y() );
					break;
				}

				db_cmd->Delete();
			}
			else if (db_cmd->IsA(STR_ZONE))
			{
				db_cmd->Delete();
			}
			else // INSTANCE
			{
				// 2006.11.23 (PE) -- The following disabled statement is
				// a remnant of my first attempt to reduce the number of
				// reposition events by assigning parts to the leftmost
				// zone in which they fit. This turns out to be a rather
				// complicated problem because the current implementation
				// treats regular zones and clamp zones independently.
				// Correcting this problem involves a sizeable rewrite.
				//
				//   C2dBox box = db_cmd->RealBox();
				feat->Append( db_cmd );
			}
		}
	}

	select.Clear();

	return ret;
}

// ==================================================================
//		LargeInstances
//
//	Find all "large" instances -- in zone LARGE_PART_ZONE.  Explode
//	these into geometry.  Assumes you have already done PackageInstance.
//
//	Go through the list of zones and chop this geometry up at the
//	boundaries... and package the remains.
//
CReturn		
CRepo::LargeInstances( void )
{
	CReturn	ret;
	bool	did_split;

	// Build an array of zone midlines, where profiles will get split
	CGeoCurveArray	split_array;

	int idx, num = (g_v17_large_part_pass ? m_zone_array.GetSize() - 1 : m_zone_array.GetSize());

	for (idx=0; idx<num; idx++)
	{
		C2dBox z1 = m_zone_array[idx];

		CGeoLine* split = new CGeoLine;

#define SPLIT_OFFSET 0.1

		split->StartPt( z1.Xmax(), z1.Ymin()-SPLIT_OFFSET, 0. );
		split->EndPt( split->StartPt().X(), z1.Ymax()+SPLIT_OFFSET, 0. );

		split_array.Append( split );
	}

	// Now, find all instances in the large part zone, explode them,
	//	and whack them up at the zone boundaries
	CSelector	select( *m_model );
	select.All( 0 );
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( TRUE );
	select.SystemFlag( FALSE );
	select.SelectAll( TRUE );

	num = select.Count();
	for (idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);
		const CVarList& var = db_cmd->Attrib();

		int repoidx = -1;
		if ( db_cmd->IsA("INSTANCE") )
		{
			if (g_v17_large_part_pass)
			{
				repoidx = var.getInt( "repo", -1 ) - 1;

				if (repoidx == LARGE_PART_ZONE)
				{
					CDbFeature* db_large = explode_instance( db_cmd );

					// 2008.09.06 (PE) -- See also SubpiecesDistribute()
					// Any entities of a large part that are wholly accessible
					// by a zone are moved out of the large part and into the zone.
					SubpiecesDistribute( db_large );

					ret += explode_profiles( db_large, NULL, NULL, NULL );
					ret += split_profiles( db_large, split_array, NULL, &did_split );
				}
			}
			else
			{
				int is_large_part = var.getInt( "_large", 0 );

				if ( is_large_part )
				{
					CDbFeature* db_large = explode_instance( db_cmd );

					// 2008.09.06 (PE) -- See also SubpiecesDistribute()
					// Any entities of a large part that are wholly accessible
					// by a zone are moved out of the large part and into the zone.
					SubpiecesDistribute( db_large );

					// The remaining entities are exploded/split/rebuilt as necesary.
					ret += explode_profiles( db_large, NULL, NULL, NULL );
					ret += split_profiles( db_large, split_array, NULL, &did_split );
				}
			}
		}
	}

	// Appropriate cleanup
	split_array.DestructiveFlush();

	return ret;
}


// ==================================================================
//		explode_instance
//
//	Find the pattern referenced by this instance, and explode the geometry
//	to where the instance is.  Delete the instance.  Return a containing
//	feture.
//
CDbFeature* CRepo::explode_instance( CDbCommand* db_cmd )
{
	// Find the pattern for this instance
	const CVarList& cmdvar = db_cmd->Attrib();
	ID pattern_id = cmdvar.getInt( "patid", 0 );

	CDbPattern* db_pattern = NULL;
	m_model->EntityFind( pattern_id, (CDbEntity**)&db_pattern, DBPATTERN, DBPATTERN );
	if (!db_pattern)
		return NULL;

	const CVarList& patvar = db_pattern->Attrib();

	// Preserve our part name here
	// Don't know why we can't just re-use the instance
	// command, but it just don't work :-(
	CDbCommand* cmd;
	m_model->EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );

	CString name = patvar.getString("_label", db_pattern->Name());
	C3dCoord cmdpt = db_cmd->Coord();
	cmdpt += C3dVec( patvar.getReal("_label_dx", 0.0), patvar.getReal("_label_dy", 0.0), 0.0);

	cmd->Init( db_cmd->Tool(), db_cmd->Workplane(), cmdpt, name );
	cmd->SystemFlag( false );

	cmd->DoubleSet( "angle", patvar.getReal("_label_angle", 0.0));
	cmd->IntSet( "pos", (eDisplayTextPos)patvar.getInt("pos", TEXTPOS_BTMCTR ));

	cmd->IntSet( "label_size", patvar.getInt("_label_size", 10));

	// ie. To indicate the text represents a former instance.
	// This allows use to control the display of the text as
	// if it were an instance.  See also CViewBase::PrintText().
	cmd->IntSet( "patid", pattern_id );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Create a _part feature to hold our explosion
	CDbFeature* db_part = NULL;
	m_model->EntityCreate( DBFEATURE, (CDbEntity**)&db_part );

	db_part->StringSet( STR_TYPE, "_part" );
	if (g_v17_large_part_pass)
	{
		db_part->IntSet( STR_ZONE, LARGE_PART_ZONE );
	}
	else
	{
		db_part->IntSet( "_large", 1 );
	}

	// Now clone our internal geometry into the _part feature
	m_model->EntityPrepareCopy( m_model );
	int num = db_pattern->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*db_pattern)[idx];
		if (db_ent->IsDeleted())
			continue;

		CDbEntity* new_ent = NULL;
		m_model->EntityCopy( *db_ent, &new_ent );

		if (!new_ent->Owner())
			db_part->Append( new_ent );
	}

	// Strip leads from db_part
	strip_leads(db_part);

	// Now transform the part to the correct position and orientation
	double angle = DEG2RAD * db_cmd->DoubleGet( "instang", 0.0 );
	C3dVec origin = db_cmd->Coord();

	C3x4Matrix pat_xform;
	pat_xform.setXYAngle( angle );
	pat_xform.Shift( origin );

	db_part->Transform( pat_xform );

	// -----------------

	db_cmd->Delete();

	return db_part;
}

// NOTE: Recursive.
void CRepo::strip_leads( CDbFeature* db_feat )
{
	int num = db_feat->Count()-1;
	for (int idx = num; idx >= 0; idx--)
	{
		CDbEntity* dbEntity = db_feat->GetAt( idx );

		CDbFeature* subfeat = dynamic_cast<CDbFeature*>( dbEntity );
		if (subfeat)
		{ 
			if ( subfeat->IsLead() )
				subfeat->Delete();
			else
				strip_leads(subfeat);
		}
		else
		{
			CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );
			if (dbProfile)
			{
				if ( dbProfile->IsLeadHull() )
					dbProfile->Delete();
			}
			else
			{
				CDbHole* dbHole = dynamic_cast<CDbHole*>(dbEntity);
				if (dbHole && dbHole->IsPierce())
					dbEntity->Delete();
			}
		}
	}
}

// ==================================================================
//	At this point, you should have called PackageInstance and LargeInstance,
//	so things are in a predictable state.
//
//	For each zone, for each clamp in that zone:
//
//		Create a set of split-lines as defined by that clamp
//		For the current zone feature; create a sub-feature (B) in that zone
//			to hold geometry under the clamp
//		Move all entities entirely within clamp into B zone
//		Split remaining curves; move those under clamp into Bz (remember both 
//			halves of all splits)
//		Rebuild profiles of all split curves
//
//	This function is like the LargeInstances method, but with differences.
//
// TODO: Break out various bits as helper methods
//
CReturn CRepo::ClampInstances( 
	double			clamp_buffer,
	double			machine_left,
	double			torch_offset,
	double			sheet_right,
	bool			small_repo,	// Move the minimum dist for clamps, or move to mid between clamps?
	int				zidx,
	CDbFeature**	db_czone )
{
	CReturn	ret;
	double	xmin, xmax;

	int cidx, cnum = m_clamp_array[zidx].GetSize();
	if ( cnum == 0)
		return ret;

	if (torch_offset > -SMALL)	// Only cope with torches off the left side.
		torch_offset = 0.;

	// ------------------------------------------------
	//	Useful values and background information
	//
	if (ZERO(clamp_buffer))
		clamp_buffer = 2.0*SMALL;

	CDbWorkplane*	workplane = NULL;
	m_model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = m_model->ActiveWorkplane();
	//
	//
	CDbFeature* db_zone = m_zonefeat_array[zidx];
	//
	//
	C2dBox	zone_box = m_zone_array[zidx];

	double clamp_spacing = zone_box.Dx();
	for (cidx=0; cidx<(cnum-1); cidx++)
	{
		C2dBox c1_box = (m_clamp_array[zidx])[cidx];
		C2dBox c2_box = (m_clamp_array[zidx])[cidx+1];

		double delta = c2_box.Xc() - c1_box.Xc();

		clamp_spacing = min( clamp_spacing, c2_box.Xc() - c1_box.Xc() );
	}
	clamp_spacing -= clamp_buffer;
	// --------------------------------------------------------
	// Create a beta sub-zone to hold stuff under the clamps
	//
	*db_czone = NULL;
	m_model->EntityCreate( DBFEATURE, (CDbEntity**)db_czone );

	CString	name;
	name.Format( CLAMP_ZONE_NAME, zidx+1 );
	name = db_zone->NameConvert( name );
	(*db_czone)->SystemName( name );

	m_clampfeat_array.SetAtGrow( zidx, *db_czone );
	// ----------------------------------------------------------
	// Set values for the clamp zone
	// ... dimensions
	// ... reposition
	//
	C2dBox	clamp_box = (m_clamp_array[zidx])[0];
	double clamp_width = clamp_box.Dx() + clamp_buffer*2.0;

	double clamp_repo = max( clamp_width, clamp_spacing*0.5 );

	if (small_repo)
	{
		clamp_repo = clamp_width;
	}
	else
	{
		clamp_box = (m_clamp_array[zidx])[cnum-1];
		double off_sheet = (clamp_box.Xmax()+clamp_repo+clamp_buffer) - sheet_right;
		if (off_sheet > SMALL)
		{ clamp_repo -= off_sheet; }

		if (clamp_repo < 0)
		{ clamp_repo = clamp_width; }
		else
		{ clamp_repo = max( clamp_width, clamp_repo ); }
	}

	// 2006.11.13 (PE) -- See also explode_profiles().
	m_clamp_repo = clamp_repo;

	//
	//
	CVarList* clamp_var = (*db_czone)->pAttrib();
	CVarList* zone_var = db_zone->pAttrib();

	// Zone numbers are [1..N] to make VB-side easier.
	clamp_var->setInt( "_zone_num", (zidx+1) );
	clamp_var->setString( STR_TYPE, "_zone" );

	// The distance between the left edge of the zone
	// and the (center of the) clamp must remain constant.
	xmin = zone_box.Xmin() + clamp_repo;
	xmax = zone_box.Xmax() + clamp_repo;
	xmax = min( xmax, sheet_right );

	clamp_var->setReal( "_zone_left", xmin );
	clamp_var->setReal( "_zone_top", zone_box.Ymin() );
	clamp_var->setReal( "_zone_right", xmax );
	clamp_var->setReal( "_zone_bottom", zone_box.Ymax() );

	// Make the clamps visible at this position
	int clamp_num = zone_var->getInt("_clamp_num", 0);
	clamp_var->setInt("_clamp_num", clamp_num);

	CString varname;
	for (int idx=1; idx<=clamp_num; idx++)
	{
		varname.Format( "_clamp%d_x", idx );
		clamp_var->setReal(varname, zone_var->getReal(varname, 0.0) + clamp_repo );

		varname.Format( "_clamp%d_y", idx );
		clamp_var->setReal(varname, zone_var->getReal(varname, 0.0) );
	}
	clamp_var->setReal( "_clamp_lldx", zone_var->getReal("_clamp_lldx", 0.0) );
	clamp_var->setReal( "_clamp_lldy", zone_var->getReal("_clamp_lldy", 0.0) );
	clamp_var->setReal( "_clamp_trdx", zone_var->getReal("_clamp_trdx", 0.0) );
	clamp_var->setReal( "_clamp_trdy", zone_var->getReal("_clamp_trdy", 0.0) );

	// I don't like this var... but we don't differentiate clamp vs. regular zones
	// anymore, so there you have it.
	clamp_var->setInt( "_clamp_zone", 1 );
	//
	// Setup the hold information
	//
	int type = (eHoldType) zone_var->getInt( "_hold", IUNDEFINED );
	clamp_var->setInt( "_hold", type );

	double hold_x = zone_var->getReal( "_hold_x", 0.0 );
	double hold_y = zone_var->getReal( "_hold_y", 0.0 );
	clamp_var->setReal( "_hold_x", hold_x );
	clamp_var->setReal( "_hold_y", hold_y );

	switch (type)
	{
	case HOLD_2CIRCLE:
		clamp_var->setReal( "_hold_dia", zone_var->getReal( "_hold_dia", 0.0 ) );

		clamp_var->setReal( "_hold_c1dx", zone_var->getReal( "_hold_c1dx", 0.0 ) );
		clamp_var->setReal( "_hold_c1dy", zone_var->getReal( "_hold_c1dy", 0.0 ) );

		clamp_var->setReal( "_hold_c2dx", zone_var->getReal( "_hold_c2dx", 0.0 ) );
		clamp_var->setReal( "_hold_c2dy", zone_var->getReal( "_hold_c2dy", 0.0 ) );
		break;

	case HOLD_RECTANGLE:
		clamp_var->setReal( "_hold_tldx", zone_var->getReal( "_hold_tldx", 0.0 ) );
		clamp_var->setReal( "_hold_tldy", zone_var->getReal( "_hold_tldy", 0.0 ) );
		clamp_var->setReal( "_hold_brdx", zone_var->getReal( "_hold_brdx", 0.0 ) );
		clamp_var->setReal( "_hold_brdy", zone_var->getReal( "_hold_brdy", 0.0 ) );
		break;
	}

	// ------------------------------------
	// Manipulate the geometry under each clamp
	//	in our zone.  Find parts that are affected
	//	by the clamp; make exceptions for those parts
	//	which are left totally free by the clamp repo.
	//
	bool did_split = false;
	for (cidx=0; cidx<cnum; cidx++)
	{
		clamp_box = (m_clamp_array[zidx])[cidx];
		//
		// Define the clamp in terms of boxes of influence
		//
		CGeoCurveArray	split_array;

		C2dBox split_box( clamp_box.Xmin()-clamp_buffer,
						  clamp_box.Ymin()-clamp_buffer,
						  clamp_box.Xmax()+clamp_buffer,
						  clamp_box.Ymax()+clamp_buffer );
		C3dBox sel_box( split_box.Xmin(), split_box.Ymin(), -1e6,
						split_box.Xmax(), split_box.Ymax(), 1e6 );

		double left = (zone_box.Xmin() + machine_left) + clamp_repo;

		C2dBox* tst_clamp;
		if (cidx)
		{
			tst_clamp = &(m_clamp_array[zidx])[cidx-1];
			left = max( left, (tst_clamp->Xmax()+clamp_repo) ); 
		}

		// double right = zone_box.Xmax();
		double right = (zone_box.Xmax() + machine_left) + clamp_repo;
		right = min( right, (clamp_box.Xmin()+clamp_repo) );
		if ( (cidx+1) < cnum)
		{
			tst_clamp = &(m_clamp_array[zidx])[cidx+1];
			right = min( right, tst_clamp->Xmin() );
		}

		C2dBox containbox( left, zone_box.Ymin()-clamp_buffer, right, zone_box.Ymax()+clamp_buffer );
		//
		// While we are here, create split lines to whack bad geometry with
		//
		CGeoLine* split = new CGeoLine;
		split->StartPt( split_box.Xmin(), split_box.Ymin(), 0.0 );
		split->EndPt( split_box.Xmax(), split_box.Ymin(), 0.0 );
		split_array.Append( split );

		split = new CGeoLine;
		split->StartPt( split_box.Xmax(), split_box.Ymin(), 0.0 );
		split->EndPt( split_box.Xmax(), split_box.Ymax(), 0.0 );
		split_array.Append( split );

		split = new CGeoLine;
		split->StartPt( split_box.Xmax(), split_box.Ymax(), 0.0 );
		split->EndPt( split_box.Xmin(), split_box.Ymax(), 0.0 );
		split_array.Append( split );

		split = new CGeoLine;
		split->StartPt( split_box.Xmin(), split_box.Ymax(), 0.0 );
		split->EndPt( split_box.Xmin(), split_box.Ymin(), 0.0 );
		split_array.Append( split );

		// Find list of all affected instances and large-part features...
		CDbEntityList	src_list;

		clamped_instances( &src_list, clamp_box, containbox, zidx );
		C2dBox skipbox( containbox.Xmin()+torch_offset,
						containbox.Ymin(),
						containbox.Xmax(),
						containbox.Ymax() );

		int snum=src_list.Count();
		for (int sidx=0; sidx<snum; sidx++)
		{
			CDbFeature* db_feat = dynamic_cast<CDbFeature*>(src_list[sidx]);
			if (db_feat)
			{
				explode_profiles( db_feat, *db_czone, &skipbox, &clamp_box );
				split_profiles( db_feat, split_array, &clamp_box, &did_split );
			}
		}

		split_array.DestructiveFlush();
		src_list.BenignFlush();

	} // end for cidx

	// ------------------------------------
	// Did we clamp anything?
	//	If so, adjust the repos.
	//
	if ( !is_feature_empty( *db_czone ) || did_split )
	{
		double zone_repo = zone_var->getReal( "_zone_repo", 0.0 );
		//
		// Okay, we have a clamp.  This means the clamp inherits the zone repo, and the zone
		// repo is to move into the clamp.
		//
		if ( !DEFINED(zone_repo) || (zone_repo < SMALL) )
		{ zone_repo = 0.0; }
		else
		{ zone_repo -= clamp_repo; }

		zone_var->setReal( "_zone_repo", clamp_repo );

		clamp_var->setReal( "_zone_repo", zone_repo );
		if ( ZERO(zone_repo) || !DEFINED(zone_repo) )
		{ 
			clamp_var->setInt( "_hold", IUNDEFINED );
		}
	}

	return ret;
}


// ==================================================================
//		clamped_instances
//
//	Locate any and all part features that may be effected 
//	by this clamp.
//
CReturn
CRepo::clamped_instances( 
	CDbEntityList*	dst_list, 
	C2dBox&			clamp,
	C2dBox&			column,
	int				zidx )
{
	CReturn		ret;
	//
	// First, loose features, just like before.  
	// This takes care of large part geometry
	//
	CSelector	select( *m_model );
	select.All( 0 );
	select.Filter( DBFEATURE, 1 );
	select.Restrictions( TRUE );
	select.SelectAll( TRUE );

	int idx, num = select.Count();
	for (idx=0; idx<num; idx++)
	{
		CDbFeature*	feature = (CDbFeature*)select[idx];

		CString type = feature->StringGet( STR_TYPE, "<error>" );

		// Is it a nested part?
		// TODO:  Change this to top-level features, w/o owners?
		if (type.CompareNoCase( "_part" ) == 0)
		{
			if (g_v17_large_part_pass)
			{
				// Is it in the right zone?
				int fzone = feature->IntGet( STR_ZONE, ZONE_NONE );
				if ( (fzone == zidx) || (fzone == LARGE_PART_ZONE) )
				{
					// Does it cross our clamp?
					if (clamp.Intersects( feature->Box(0), SMALL ) )
						dst_list->Append( feature );
				}
			}
			else
			{
				// Is it in the right zone?
				int fzone = feature->IntGet( STR_ZONE, ZONE_NONE );
				int is_large_part = feature->IntGet( "_large", 0 );
				if ( (fzone == zidx) || (is_large_part) )
				{
					// Does it cross our clamp?
					if (clamp.Intersects( feature->Box(0), SMALL ) )
						dst_list->Append( feature );
				}
			}
		}
	}
	//
	// Now, instances, which fall under different rules of operation
	// TRY to avoid splitting them, for example, when possible.
	//
	CDbFeature* db_czone = m_clampfeat_array[zidx];

	select.Clear();

	select.All(0);
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( true );
	select.SystemFlag(false);
	select.SelectAll(true);

	num = select.Count();
	for (idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);

		if ( db_cmd->IsInstance() )
		{
			// Relevant to this repo?
			//
			int repoidx = db_cmd->IntGet( "repo", -1 ) - 1;
			if (repoidx != zidx)
			{ continue; }
			//
			// Get our pattern...
			//
			ID pattern_id = db_cmd->IntGet( "patid", -1);
			CDbPattern* db_pattern = NULL;
			m_model->EntityFind( pattern_id, (CDbEntity**)&db_pattern, DBPATTERN, DBPATTERN );
			if (!db_pattern)
			{ continue; }
			// 
			// Now determine where this pattern lies on the sheet
			//
			double angle = DEG2RAD * db_cmd->DoubleGet( "instang", 0.0 );
			C3dVec origin = db_cmd->Coord();

			C3x4Matrix pat_xform;
			pat_xform.setXYAngle( angle );
			pat_xform.Shift( origin );

			C3dBox inst_box = db_pattern->Box(0);
			C3dCoord tl( inst_box.Xmin(), inst_box.Ymin(), 0.0 );
			C3dCoord br( inst_box.Xmax(), inst_box.Ymax(), 0.0 );
			pat_xform.Transform( &tl );
			pat_xform.Transform( &br );
			inst_box = C3dBox( tl, br );
			//
			// Now, does it hit the clamp, but NOT fit in the column?
			//
			if (!clamp.Intersects( inst_box, SMALL ))
			{ continue; }

			if (column.Contains( inst_box, SMALL ))
			{ 
				// Since we won't be breaking this up, then just drop it into
				// the clamp zone

				db_czone->Append( db_cmd );
				continue;
			}
			//
			// Explode it to geometry, and add to the list
			//
			CDbFeature* db_part = explode_instance( db_cmd );
			dst_list->Append( db_part );
		}
	}

	select.Clear();

	return ret;
}


// ==================================================================
CReturn
CRepo::compress_zones( void )
{
	CReturn		ret;
	CString		name;
	CDbFeature*	db_feat;
	CVarList*	attribs;
	double		repo;
	int			main_zone;

	int znum = m_zonefeat_array.GetSize();
	int cnum = m_clampfeat_array.GetSize();

	// Zone numbers are [1..N] to make VB-side easier.
	int cidx = 0;
	int zone = 1;
	for (int zidx=0; zidx<znum; zidx++)
	{
		db_feat = m_zonefeat_array[zidx];
		if (!db_feat->IsDeleted())
		{
			attribs = db_feat->pAttrib();
			
			attribs->setInt("_zone_num", zone );
			name.Format( REPO_ZONE_NAME, zone );
			name = db_feat->NameConvert( name );
			db_feat->SystemName( name );

			repo = attribs->getReal( "_zone_repo", UNDEFINED );
			if (repo >= UNDEFINED)
			{
				attribs->setInt( "_hold", IUNDEFINED );  // ie. no hold (in case VB needs it)
			}

			zone++;
		}

		while (cidx < cnum)
		{
			db_feat = m_clampfeat_array[cidx];
			if (!db_feat->IsDeleted())
			{
				attribs = db_feat->pAttrib();
				main_zone = attribs->getInt( "_zone_num", 0 ) - 1;
				if (main_zone == zidx)
				{
					attribs->setInt("_zone_num", zone );
					name.Format( REPO_ZONE_NAME, zone );
					name = db_feat->NameConvert( name );
					db_feat->SystemName( name );

					zone++;
				}
				else
				{ break; }
			}
				
			cidx++;
		}
	}

	return ret;
}


// ==================================================================
/**
 *	A Progressive reposition packages all punch and burn entities
 * from left to right in such a way as to minimize backups.  Note
 * that punch and burn zones are at different base offsets, due to 
 * the offset of the burn and scribe heads from the punch head.
 *
 *	This feature was created specifically for Franklin Mfg.
 *
 * ASSUMES holes are punched
 * ASSUMES curves and profiles are otherwise cut
 *
 * TODO: Check actual tooling rather than rely on type.
 */
CReturn 
CRepo::Progressive( 
	double burn_offset,
	double step,
	double length,		///< Sheet size
	double width )
{
	CReturn ret;

	if ( ZERO(step)
		|| ZERO(burn_offset) )
	{
		if (burn_offset < -SMALL)
		{
			ret.UserWarn( IDS_PROG_ZEROSTEP );
			return ret;
		}
	}
	//
	// First, explode the patterns
	//	Stolen from CPPattern's explode
	//
	CDbIterator	iter;
	iter.Init( m_model->Db(), DBPATTERN );
	while (true)
	{
		CDbPattern* dbPattern = dynamic_cast<CDbPattern*>( iter() );
		if (dbPattern == NULL)
		{ break; }
		iter.Next();

		ret += CModelUtil::PatternExplode( (*dbPattern) );
	}
	//
	// Now remove all of the existing zones
	//	Stolen from CPZone's explode
	//
	CDbIterator	sub_iter;
	iter.Init( m_model->Db(), DBFEATURE );
	while (true)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
		{ break; }
		iter.Next();

		CVarList* var = dbFeature->pAttrib();
		CString type = var->getString( STR_TYPE, "<error>" );
		if (!stricmp( type, "_zone" ))
		{
			int zone = var->getInt( "_zone_num", 0 );
			//
			// Find any associated sequence object.
			//
			sub_iter.Init( m_model->Db(), DBSEQUENCE );
			CDbSequence* dbSequence = NULL;
			while (1)
			{
				dbSequence = dynamic_cast<CDbSequence*>( sub_iter() );
				if (dbSequence == NULL)
				{ break; }
				iter.Next();

				if (zone == dbSequence->IntGet( "_zone_num", 0 ))
				{ break; }
			}

			if (dbSequence != NULL)
			{ dbSequence->Delete(); }

			dbFeature->BenignFlush();
			dbFeature->Delete();
		}
	}
	//
	// Different areas for burn (not-holes) and punch (holes)
	//
	double burn_right = step;
	double punch_right = step;

	if (burn_offset < -SMALL)
	{ punch_right = -(burn_offset + step); }
	else
	{ burn_right = burn_offset - step; }

	double zone_left = 0.0;
	double zone_right = max(burn_right, punch_right);

	C3dBox burn_column( 0.0, 0.0, -LARGE, burn_right, width, LARGE );
	C3dBox punch_column( 0.0, 0.0, -LARGE, punch_right, width, LARGE );
	//
	// Selectors.  Let's see if we can run two simultaneously.
	//
	CSelectorStack& selectorStack = m_model->SelectorStack();

	CSelector burn_sel(*m_model);
	burn_sel.All( 0 );
	burn_sel.Filter( DBPROFILE, 1 );
	burn_sel.Filter( DBLINE, 1 );
	burn_sel.Filter( DBARC, 1 );
	burn_sel.Restrictions( FALSE );

	CSelector punch_sel(*m_model);
	punch_sel.All( 0 );
	punch_sel.Filter( DBHOLE, 1 );
	punch_sel.Filter( DBCOMMAND, 1 );
	punch_sel.Restrictions( FALSE );
	//
	// Now expand our zones and collect geometry until done
	//
	int burn_first = 0;
	int punch_first = 0;
	int zone_idx = 0;
	int zone_num = (int)ceil(length / step) + 1;
	for (int cnt=0; cnt<zone_num; cnt++)
	{
		// Create the repo-zone feature
		//
		CDbFeature* db_zone = NULL;
		m_model->EntityCreate( DBFEATURE, (CDbEntity**)&db_zone );

		CString name;
		name.Format( REPO_ZONE_NAME, zone_idx+1 );
		name = db_zone->NameConvert( name );
		db_zone->SystemName( name );
		//
		// Zone numbers are [1..N] to make VB-side easier.
		db_zone->Owner( NULL );
		db_zone->IntSet( "_zone_num", (zone_idx+1) );
		//
		//	Some zone attributes...
		CVarList* var = db_zone->pAttrib();
		var->setString( STR_TYPE, "_zone" );
//		var->setReal( "_zone_left", zone_left );
		var->setReal( "_zone_left", 0.0 );
		var->setReal( "_zone_top", 0.0 );
		var->setReal( "_zone_right", zone_right );
		var->setReal( "_zone_bottom", width );
		//
		// Now, fill this zone
		//
		burn_sel.WithinBox( burn_column, SMALL );
		punch_sel.WithinBox( punch_column, SMALL );
		bool added = false;

		int idx, num = burn_sel.Count();
		for (idx=burn_first; idx<num; idx++)
		{
			CDbEntity* db_ent = burn_sel[idx];
			if (db_ent->Type() == DBPROFILE)
			{ 
				// If this profile has leads, be sure to move those across as well.
				//
				CDbFeature* owner = dynamic_cast<CDbFeature*>(db_ent->Owner());
				CDbFeature* leadin = NULL;
				CDbFeature* leadout = NULL;
				if (owner)
				{
					int idx = owner->Position(db_ent);
					int st = max(0, idx-2);
					int en = min(owner->Count()-1, idx+2);

					for (int tst=st; tst<=en; tst++)
					{
						CDbFeature* test = dynamic_cast<CDbFeature*>((*owner)[tst]);

						if ( test
							&& test->IsLead() )
						{
							if (test->IsLeadIn())
							{ leadin = test; }
							else
							{ leadout = test; }
						}
					}
					ret.Diagnostic("--------------------");
				}

				if (leadin)
				{ db_zone->Append(leadin); }

				db_zone->Append( db_ent );

				if (leadout)
				{ db_zone->Append(leadout); }

				added = true;
			}
			else
			{
				bool skip = false;

				// Only loose curves; if in a profile, wait till get full prof.
				CDbProfile* prof = dynamic_cast<CDbProfile*>(db_ent->Owner());
				if (prof)
				{ skip = true; }
				else
				{
					CDbFeature* feat = dynamic_cast<CDbFeature*>(db_ent->Owner());
					if ( feat
						&& feat->IsLead() )
					{ skip = true; }
				}

				if (!skip)
				{
					db_zone->Append( db_ent );
					added = true;
				}
			}
		}
		burn_first = num;

		num = punch_sel.Count();
		for (idx=punch_first; idx<num; idx++)
		{
			CDbEntity* db_ent = punch_sel[idx];
			db_zone->Append( db_ent );
			added = true;
		}
		punch_first = num;
		//
		// Advance to the next column and zone
		//
		punch_right += step;
		punch_column.Xmax( punch_right );

		burn_right += step;
		burn_column.Xmax( burn_right );

		zone_right += step;
		if (added)
		{ 
			zone_left += step;
			zone_idx++;
		}
		else
		{ db_zone->Delete(); }
	}

	burn_sel.Clear();
	punch_sel.Clear();


	// 2007.04.19 (PE) -- Not sure if we should use 'true' or 'false' here.
	// Since the code seems to have worked prior to changing EmptyContainers(),
	// we use 'true' because that matches the previous behavior.
	CModelUtil::EmptyContainers( (*m_model), true );

	return ret;
}

bool
CRepo::PostRepoInterference( const C2dBox& part_box )
{
	C2dBox	clamp;
	int		count, indx;
	int		zone;

	// NOTE: m_clampfeat_array is updated by ClampInstances().
	zone = m_clampfeat_array.GetSize() - 1;
	if (zone >= 0)
	{
		count = m_clamp_array[zone].GetSize();
		for (indx = 0; indx < count; ++indx)
		{
			clamp = (m_clamp_array[zone])[indx];
			clamp.Shift( m_clamp_repo, 0. );

			if ( clamp.Intersects( part_box, SMALL) )
				return true;
		}
	}
	else
	{
		// Something is probably very wrong.
	}

	return false;
}

bool
CRepo::IsSplittable( const CDbCurve* dbCurve )
{
	bool is_splittable = (dbCurve != NULL);
	if ( is_splittable )
	{
		CDbTool* dbTool = dbCurve->Tool();

		// We prevent the creation of unnecessary repos by filtering
		// out part-outline geometry, Otherwise, say for a part whose
		// edges are the edges of the sheet, the nesting engine would
		// think the part-outline geometry is something it has to cut.
		// As such, it would introduce additional repos so that it
		// could access any geometry that lays under a clamp.
		is_splittable = (dbTool->Name().CompareNoCase(STR_PART_OUTLINE) != 0);
	}


	return is_splittable;
}

bool
CRepo::IsRepoPart( const CDbFeature* dbFeature )
{
	bool is_repo_part = false;

	if (dbFeature != NULL)
	{
		CString type = dbFeature->StringGet( STR_TYPE, "<error>" );
		if (type.CompareNoCase( "_part" ) == 0)
		{
			// It is a nested part, so ....
			is_repo_part = (dbFeature->IntGet( "rebuild", 0 ) != 0);
		}
	}

	return is_repo_part;
}

// 2008.09.06 (PE)
// Where dbLargePart is an exploded-instance of a large-part.
// SubpiecesDistribute() distributes the profiles and sub-features
// of dbLargePart amongst the zone-features IFF said entities are
// clearly and totally accessible within a zone. Stated differently,
// SubpiecesDistribute() was introduced to prevent the creation of
// clamp-zones when none is really necessary (eg. this was happenning
// when a profile was under a clamps in zone1 but the entity could
// have been cut entirely in zone2, without having to introduce
// an additional repo for an additional clamp-zone).
void
CRepo::SubpiecesDistribute( CDbFeature* dbLargePart )
{
	int count = dbLargePart->Count();
	int indx = 0;
	while (indx < count)
	{
		CDbEntity* dbEntity = (*dbLargePart)[indx];

		CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );
		if (dbContainer == NULL)
		{
			++indx;
		}
		else
		{
			C2dBox box = dbContainer->Box();

			int zone = WhichZone( box );
			if (zone < 0)
			{
				++indx;
			}
			else
			{
				// This entity is wholly accessible within a zone and
				// does not require any exploding/splitting. As such,
				// we move the entity from the large part directly into
				// the containing zone.
				m_zonefeat_array[zone]->Append( dbContainer );
				--count;
			}
		}
	}
}

int
CRepo::WhichZone( const C2dBox& box )
{
	int zcnt, zndx;
	int ccnt, cndx;
	int ncnt, nndx;

	// Assume not wholly accessible in any zone.
	int which = -1;

	zcnt = m_zone_array.GetSize();
	for (zndx = 0; zndx < zcnt; ++zndx)
	{
		if ( m_zone_array[zndx].Contains( box, SMALL ) )
		{
			// The entity is wholly contained by this zone.
			which = zndx;

			ccnt = m_clamp_array[zndx].GetSize();
			for (cndx = 0; cndx < ccnt; ++cndx)
			{
				if ( m_clamp_array[zndx][cndx].Intersects( box, SMALL ) )
				{
					// This clamp interferes with the entity.
					which = -2;

					if (((zndx+1) < zcnt) && m_zone_array[zndx+1].Contains( box, SMALL ))
					{
						// The entity is wholly contained by the next zone.
						which = zndx + 1;
						
						ncnt = m_clamp_array[zndx+1].GetSize();
						for (nndx = 0; nndx < ncnt; ++nndx)
						{
							if ( m_clamp_array[zndx+1][nndx].Intersects( box, SMALL ) )
							{
								// This clamp interferes with the entity.
								which = -3;
								break;
							}
						}

					}

					// There is no need to check against subsequent clamps
					// because the entity is clearly accessible (or not).
					break;
				}
			}

			if (which >= 0)
			{
				// The entity is wholly accessible within a zone.
				// As such, we can forego further processing.
				// NOTE: This also ensures the entity is in the leftmost zone.
				break;
			}
		}
	}

	return which;
}
