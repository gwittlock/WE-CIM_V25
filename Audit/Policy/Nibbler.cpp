
// ==================================================================
//		Nibbler
//
//	This class is given geometry or profiles, and returns a list of
//	points that represent punch hits on that geometry.  Points are
//	appended to the list, so a single list can be grown via multiple
//	calls.  Geometry that does not have punch tooling assigned to
//	it will generate zero hits.
//
//	NOTE that the (irrelevant) Z ordinate in the returned hit list 
//	is OVERLOADED to contain the tangent angle of the curve at that 
//	hit point (for easy access).
//
//	NOTE that the point list must be destructively flushed at end
// ==================================================================

#include "stdafx.h"

#include "ColorConst.h"
#include "StringConst.h"
#include "3dCoord.h"
#include "Profile.h"
#include "Conversion.h"
#include "Offsetter.h"
#include "GeoArc.h"
#include "DbWorkplane.h"
#include "DbHole.h"
#include "DbSequence.h"
#include "DbIterator.h"

#include "Nibbler.h"

// ==================================================================

static double DEFAULT_FEED_FACTOR = 0.80;

// ==================================================================

CNibbler::CNibbler( void )
{
}

CNibbler::~CNibbler( void )
{
}

// ==================================================================
bool
CNibbler::canNibble( const CDbEntity& db_ent, bool nesting )
{
	CDbTool* db_tool = NULL;

	switch (db_ent.Type())
	{
	case DBPROFILE:
		db_tool = ((const CDbProfile&)db_ent).Tool();
		break;

	case DBLINE:
	case DBARC:
		// 2007.04.22 (PE) -- It makes no sense to nibble a hole because it is
		// already a single hit. This was discovered after implementing canned
		// cycles for hole patterns, in which case, the hole pattern would not
		// be rendered when View/Options -- Show Nibble Hits was active.
		//   case DBHOLE:

		db_tool = db_ent.Tool();
		break;

	case DBHOLE:
		if ( nesting )
		{
			// 2008.01.15 (PE) -- Wrt the preceding comment, except for
			// "pierce holes".and hole patterns.
			CDbHole& dbHole = (CDbHole&) db_ent;
			if (dbHole.IsPierce() || (dbHole.IntGet( "sncs", 0 ) == 0))
				db_tool = dbHole.Tool();
		}
		break;

	default:
		break;
	}

	if (!db_tool)
		return FALSE;

	if (!db_tool->IsPunchTool())
		return FALSE;

	// In some cases, we are given partprof...
	int partprof = db_ent.IntGet( STR_PARTPROF, 0 );
	if ( partprof && (db_ent.Type() != DBHOLE) )
	{
		return FALSE;
	}

	return TRUE;
}

// ==================================================================
//		Nibble -- generic
//
//	Switch out to the relevant nibble method
//
CReturn 
CNibbler::Nibble( 
	const CDbEntity&	db_ent,
	bool				nesting,
	C3dCoordList*		ptlist )
{
	CReturn ret;

	if (!ptlist)
		return ret;

	if ( !canNibble( db_ent, nesting ) )
		return ret;

	switch (db_ent.Type())
	{
		case DBPROFILE:
			return nibble( (const CDbProfile&)db_ent, ptlist );

		case DBLINE:
		case DBARC:
			return nibble( (const CDbCurve&)db_ent, ptlist );

		case DBHOLE:
			// 2007.04.22 (PE) -- This may well be dead code.
			//   See also comment with same date in CNibbler::canNibble().
			C3dCoord* hit = new C3dCoord(((const CDbHole&)db_ent).Center());

			((const CDbHole&)db_ent).TransientOrientationSet();

			CDbTool* tool = db_ent.Tool();
//			double radians = tool->DoubleGet( STR_INDEX_ANGLE, 0.0 ) * DEG2RAD;
			double radians = tool->DoubleGet( "~orient", 0.0 ) * DEG2RAD;

			hit->Z(radians);

			ptlist->Append( hit );
			return ret;
	}

	return ret;
}


// ==================================================================
//		nibble -- profile
//
//	Nibble out a profile.  With profiles, we can accept either part-prof
//	or tool-center geometry.  When possible, part-prof geometry is converted 
// to tool-center. When this is not possible, it is IGNORED.
//
CReturn 
CNibbler::nibble( 
	const CDbProfile&	db_prof, 
	C3dCoordList*		ptlist )
{
	CReturn ret;

	// 
	// We have in hand a profile list of geometry, in tool-center mode.
	// Finally, we take each curve in turn and do the punch dance on it.
	//
	int num = db_prof.Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbCurve* db_curve = (CDbCurve*)db_prof[idx];

		nibble( *db_curve, ptlist );
	}

	return ret;
}

// ==================================================================
//		nibble -- db curve
//
//	Nibble out a single curve, otherwise the same is nibble
//	profile, above.  Much like a clone of above, actually.
//
CReturn 
CNibbler::nibble( 
	const CDbCurve& db_curve, 
	C3dCoordList*	ptlist )
{
	CReturn ret;

	double feed = DefaultFeedrate( db_curve );

	CGeoCurve* curve = db_curve.Curve();
	ret += Nibble( *curve, 
					feed,
//					NIBBLE_FIXED,
					NIBBLE_BALANCED,
//					NIBBLE_BRIDGE,
//					(eNibbleMode)db_curve.IntGet( STR_FEEDMODE, 0 ), 
					ptlist );
	delete curve;

	return ret;
}


// ==================================================================
//		Nibble -- geo curve
//
//	Slave to the above db nibbles, does the actual nibbling.  Can be
//	called by client code if desired, but not that it REQURES the
//	geometry to be tool-center.
//
//	Note that the GeoPoint.Z() ordinate is actually the tangent
//	on the curve at that point, and NOT anything resembling a 
//	position along the vertical axis.
//
CReturn 
CNibbler::Nibble( 
	const CGeoCurve&	curve, 
	double				max_feed,
	eNibbleMode			mode,
	C3dCoordList*		ptlist )
{
	CReturn ret;
	C3dCoord* pt;
	C3dCoord tmp;

	if (mode == NIBBLE_NONE)
		return ret;

	double feedrate = max_feed;
	if (ZERO(feedrate))
		return ret;
	double length = curve.Length2d();
	int hitnum = (int)ceil( length / feedrate );

	double dist = 0.0;
	int idx = ptlist->Count()-1;
	switch (mode)
	{
		case NIBBLE_BALANCED:
			hitnum = OptimalFeedrate( curve, &feedrate );
		case NIBBLE_FIXED:
		{
			hitnum++;	// Run-off the end...

			// 2005.01.30 (PE) -- Rittal reported redundant
			// hit at begin/end of full circle.
			if (curve.Type() == GEOARC)
			{
				double ai = ((const CGeoArc&) curve).IncludedAngle();
				if (ai > 359.99)
					--hitnum;
			}

			for (int cnt=0; cnt<hitnum; cnt++)
			{
				if (dist > length)
					dist = length;	// force hit at end point (unles repeat for balanced; set later test)

				tmp = curve.PointAtDist( dist, TRUE );
				pt = new C3dCoord( tmp );
				dist += feedrate;

				if (curve.Type() == GEOARC)
					pt->Z( ((const CGeoArc*)&curve)->TanAtPt( pt->X(), pt->Y() ).Radians() );
				else
					pt->Z( curve.StartTan().Radians() );

				if (idx >= 0)
				{
					// If this hit is an exact repeat of the previous hit, skip it.
					// Note that this includes tangency test in the Z ordinate
					C3dCoord* prev = (*ptlist)[idx];
					if (prev->WithinTol( *pt, SMALL ))
					{
						delete pt; pt=NULL;
						continue;
					}
				}

				ptlist->Append( pt );
				idx++;
			}
		}
		break;

		case NIBBLE_BRIDGE:
		{
			bool start = TRUE;
			// NOTE that there may be one extra hit at the center that is not needed.  If the
			// the starting hitnum is odd, then we can skip the center hit (since we round up
			// to an even hit count, which takes care of it).
			//
			bool odd = FALSE;
			if (hitnum & 0x01)
			{ odd = TRUE; }
			//
			// If only one hit, though, just hit start and end.
			//
			if (hitnum != 1)
			{ hitnum = ((hitnum+1)|0x01)-1; } // Mangle the hit count so it comes out even...
			for (int cnt=0; cnt<=hitnum; cnt++)	// Final hit at center
			{
				C3dCoord* pt;
				if ( (cnt == hitnum)
					&& (hitnum != 1) )
				{
					// Center hit
					if (odd)
					{ continue; }
					tmp = curve.MidPt();
					pt = new C3dCoord( tmp );
				}
				else
				{
					tmp = curve.PointAtDist( start?dist:-dist, start );
					pt = new C3dCoord( tmp );
				}
				start = !start;
				if (start)
					dist += feedrate;

				if (curve.Type() == GEOARC)
					pt->Z( ((const CGeoArc*)&curve)->TanAtPt( pt->X(), pt->Y() ).Radians() );
				else
					pt->Z( curve.StartTan().Radians() );

				if (idx >= 0)
				{
					// If this hit is an exact repeat of the previous hit, skip it.
					// Note that this includes tangency in the Z ordinate
					C3dCoord* prev = (*ptlist)[idx];
					if (prev->WithinTol( *pt, SMALL ))
					{
						delete pt; pt=NULL;
						continue;
					}
				}

				ptlist->Append( pt );
				idx++;
			}
		}
		break;
	}


	return ret;
}


// ==================================================================
//		OptimalFeedrate
//
//	Get a nice feedrate that will balance the number of hits on the
//	curve.  Provided in static, external form so it can be easily used
//	by code desiring to mimic the function of Nibbler, e.g. find the
//	matching feedrate to send to a machine to mimic our nibbles.
//
//	Returns the number of hits, and modifies the passed-in feedrate
//	to be optimal.
//
int
CNibbler::OptimalFeedrate( 
	const CGeoCurve&	curve, 
	double*				feedrate )		// IN: max feedrate; OUT: optimal feedrate
{
	double curve_len = curve.Length2d();
	int hit_num = (int)ceil(curve_len / *feedrate );

	*feedrate = curve_len / (double)hit_num;

	return hit_num;
}

double
CNibbler::DefaultFeedrate( const CDbCurve& db_curve )
{
	double feed = db_curve.DoubleGet( STR_FEED, 0.0 );
	if (ZERO(feed))
	{
		CDbTool* tool = db_curve.Tool();
		feed = tool->EffectiveLength() * DEFAULT_FEED_FACTOR;
	}
	return feed;
}


// Toolpath:Nibble:
int 
CNibbler::Nibble( CModel* model )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	int		total_hits;


	// Replace all selected, punch-only profiles with individual hits.
	total_hits = ProfilesReplace( model );

	// For all selected features, replace all punch-only curves with individual hits.
	total_hits += FeaturesReplace( model );

	return total_hits;
}

int
CNibbler::FeaturesReplace( CModel* model )
{
	CDbCurveArray	curves;
	C3dCoordList	hits;
	CDbIterator		iter;
	CDbFeature*		dbFeature;
	CDbCurve*		dbCurve;
	int				indx;
	int				total_hits;

	total_hits = 0;

	iter.Init( model->Db(), DBFEATURE );
	while (1)
	{	
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsSelected() )
		{
			indx = 0;
			while (indx < dbFeature->Count())
			{
				dbCurve = dynamic_cast<CDbCurve*>( (*dbFeature)[indx] );
				if ( CanReplace( dbCurve ) )
				{
					curves.Append( dbCurve );

					HitPointsGet( curves, &hits );

					total_hits += EntityReplace( hits, dbCurve );

					dbCurve->Delete();

					hits.DestructiveFlush();
					curves.BenignFlush();
				}
				else
				{
					++indx;
				}
			}

		}

		iter.Next();
	}

	return total_hits;
}

int
CNibbler::ProfilesReplace( CModel* model )
{
	CDbCurveArray	curves;
	C3dCoordList	hits;
	CDbIterator		iter;
	CDbProfile*		dbProfile;
	int				total_hits;

	total_hits = 0;

	iter.Init( model->Db(), DBPROFILE );
	while (1)
	{	
		dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		if (dbProfile->IsSelected() && dbProfile->Tool()->IsPunchTool())
		{
			ProfileCurvesGet( (*dbProfile), &curves );

			HitPointsGet( curves, &hits );

			total_hits += EntityReplace( hits, dbProfile );

			dbProfile->Delete();

			hits.DestructiveFlush();
			curves.BenignFlush();
		}

		iter.Next();
	}

	return total_hits;
}

void
CNibbler::ProfileCurvesGet( const CDbProfile& dbProfile, CDbCurveArray* dbCurves )
{
	CDbCurve*	dbCurve;
	int			count, indx;

	count = dbProfile.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbCurve = dynamic_cast<CDbCurve*>( dbProfile[indx] );
		dbCurves->Append( dbCurve );
	}
}

void
CNibbler::HitPointsGet( const CDbCurveArray& dbCurves, C3dCoordList* hits )
{
	CNibbler	nibbler;
	CDbCurve*	dbCurve;
	CGeoCurve*	geoCurve;
	double		feed, orient;
	int			count, indx;
	int			initial_count, jndx;

	count = dbCurves.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbCurve = dbCurves.GetAt( indx );

		initial_count = hits->Count();

		feed = CNibbler::DefaultFeedrate( (*dbCurve) );
		geoCurve = dbCurve->Curve();
		nibbler.Nibble(	(*geoCurve), feed, NIBBLE_BALANCED, hits );
		// orient = geoCurve->StartTan().Radians() * RAD2DEG;
		orient = dbCurve->DoubleGet( "orient", 0. );
		delete geoCurve;

		// Ugh ... not happy about this :-(
		// hit->Z() is already set by Nibble() but it represents
		// the curve tangent and not the tool orientation.
		for (jndx = initial_count; jndx < hits->Count(); ++jndx)
		{
			hits->GetAt( jndx )->Z( orient );
		}
	}
}

int
CNibbler::EntityReplace( const C3dCoordList& hits, const CDbEntity* dbEntity )
{
	CEntityDb*		db;
	CDbTool*		dbTool;
	CDbWorkplane*	dbWork;
	CDbFeature*		dbFeature;
	CDbHole*		dbHole;
	C3dCoord*		hit;
	C3dCoord*		first_hit;
	C3dCoord*		terminal_hit;
	int				ref_indx, color;
	int				count, indx;
	int				total_hits;
	bool			okay_create;

	total_hits = 0;

	// This is not a great thing to do but it does eliminate the
	// need to pass the model around as an argument everywhere.
	db = ((CDbEntity*) dbEntity)->Db();

	dbTool = dbEntity->Tool();
	dbWork = dbEntity->Workplane();

	color = dbTool->ColorGet( DCOLOR_YELLOW );

	dbFeature = dynamic_cast<CDbFeature*>( dbEntity->Owner() );
	if (dbFeature == NULL)
	{
		// We are replacing a profile that does not have an owner.
		// This should be a very rare condition because punch-only
		// profiles can only be generated via Create Associate.
		// It is possible, however, to remove a profile from its
		// owner by using the Extract function.

		db->Create( DBFEATURE, (CDbEntity**) &dbFeature );

		// dbFeature->ColorSet( color );

		ref_indx = -1;
	}
	else
	{
		// We are replacing a profile within a feature.
		ref_indx = dbFeature->Position( dbEntity );
	}

	first_hit = NULL;
	terminal_hit = NULL;

	count = hits.Count();
	for (indx = 0; indx < count; ++indx)
	{
		hit = hits.GetAt( indx );

		if (terminal_hit == NULL)
		{
			okay_create = true;

			first_hit = hit;
			terminal_hit = hit;
		}
		else
		{
			// NOTE: Arbitrary proximity tolerance.
			okay_create = ( !hit->WithinTolXY( (*first_hit), 1.e-3 ) );
			if ( okay_create )
				okay_create = ( !hit->WithinTolXY( (*terminal_hit), 1.e-3 ) );

			terminal_hit = hit;
		}

		if ( okay_create )
		{
			db->Create( DBHOLE, (CDbEntity**) &dbHole );

			dbHole->Init( dbTool, dbWork, (*hit), 0.01, 0. );
			// dbHole->ColorSet( color );

			// 2004.05.11 (PE) -- Index angle was failing to be output
			// by code generator.  Of course, we still have a problem
			// dealing with arcs, but that has not become a problem yet.
			// See also CNibbler::HitPointsGet().
			dbHole->DoubleSet( "orient", hit->Z() );

			if (ref_indx < 0)
				dbFeature->Append( dbHole );
			else
				dbFeature->InsertAfter( ref_indx, dbHole );

			++total_hits;
			++ref_indx;
		}
	}

	return total_hits;
}

bool
CNibbler::CanReplace( const CDbCurve* dbCurve )
{
	return ((dbCurve != NULL) && dbCurve->Tool()->IsPunchTool());
}


// Retained only so that we can see how we might need to manage sequence order.
#if 0

// returns count of holes created.
int
CNibbler::curve_replace(
					CDbCurve*	dbCurve,
					CModel*		model )
{
	CNibbler		nibbler;
	C3dCoordList	pts;
	CGeoCurve*		geoCurve;
	CDbContainer*	dbContainer;
	CDbSequence*	dbSequence;
	CDbHole*		dbHole;
	double			feed, orient;
	int				count, indx;
	int				con_insert_pos;
	int				seq_insert_pos;
	int				tool_color;

	feed = CNibbler::DefaultFeedrate( (*dbCurve) );
	geoCurve = dbCurve->Curve();
	nibbler.Nibble(	(*geoCurve), feed, NIBBLE_BALANCED, &pts );
	orient = geoCurve->StartTan().Radians() * RAD2DEG;
	delete geoCurve;

	count = pts.Count();
	if (count > 0)
	{
		dbContainer = dynamic_cast<CDbContainer*>( dbCurve->Owner() );
		con_insert_pos = dbContainer->Position( dbCurve );

		dbSequence = dbCurve->Sequence();
		seq_insert_pos = ((dbSequence == NULL) ? -1 : dbSequence->Position( dbCurve ));

		tool_color = dbCurve->Tool()->ColorGet( DCOLOR_RED );

		for (indx = 0; indx < count; ++indx)
		{
			model->EntityCreate( DBHOLE, (CDbEntity**) &dbHole );

			dbHole->Init(
				dbCurve->Tool(),
				dbCurve->Workplane(),
				(*pts[indx]), 0.01, 0. );
			
			// dbHole->ColorSet( tool_color );

			// 2004.05.11 (PE) -- Index angle was failing to be output
			// by code generator.  Of course, we still have a problem
			// dealing with arcs, but that has not become a problem yet.
			dbHole->DoubleSet( "orient", orient );

			dbContainer->InsertAfter( con_insert_pos, dbHole );
			++con_insert_pos;

			if (dbSequence != NULL)
			{
				dbSequence->InsertAfter( seq_insert_pos, dbHole );
				++seq_insert_pos;
			}
		}

		dbCurve->Delete();
	}

	return count;
}

#endif
