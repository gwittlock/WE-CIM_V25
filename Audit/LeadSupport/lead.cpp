
#include "stdafx.h"
#include <float.h>
#include "ColorConst.h"
#include "StringConst.h"

#include "DbFeature.h"
#include "DbTool.h"
#include "DbWorkplane.h"
#include "DbArc.h"
#include "DbLine.h"
#include "DbSequence.h"

#include "GeoPoly.h"
#include "GeoElemList.h"

#include "Profile.h"
#include "Conversion.h"
#include "ModelUtil.h"

#include "Solution.h"
#include "AutoMod.h"
#include "AutoModDb.h"
#include "Int2d.h"

#include "Lead.h"

// ==================================================================

// context name MUST MATCH eLeadContext values
static char* context_name[LEAD_CONTEXT_NUM] =
{
	"IIL ID",
	"IIA ID",
	"IEL ID",
	"IEA ID",
	"OIL ID",
	"OIA ID",
	"OEL ID",
	"OEA ID"
};

static CString LOFF( "LOff" );
static CString DOFF( "DOff" );

// 2012.04.28 (PE) -- A complete hack.
CModel* CLead::m_masterModel = nullptr;

// ==================================================================

CLead::CLead()
{
	// This constructor NOT USED
	m_model = nullptr;
	m_root = nullptr;
}

CLead::CLead( CModel& model )
{
	m_model = &model;
	m_setup = -1;
	m_root = nullptr;

	int partprof = m_model->Header().getInt( STR_PARTPROF, 0 );
	if ( partprof )
		m_model->pDefault()->setInt( STR_PARTPROF , 1 );
}

CLead::~CLead()
{
	m_model->pDefault()->deleteVar( STR_PARTPROF );

	m_model = nullptr;
}

// ==================================================================
//	Manually apply leads at the specified point, which should belong
//	to a tooled profile.
CReturn	CLead::Manual( 
	ID					id,			// ID of curve to lead at
	int					side,		// (+1) left / (0) not used / (-1) right
	const CLeadData*	lead_in,	// Lead in parameters, or nullptr for none
	const CLeadData*	lead_out )	// Lead out parameters, or nullptr for none
{
	CReturn status;

	// Verify the id, and it's context
	CDbCurve*	dbcurve = nullptr;
	m_model->EntityFind( id, (CDbEntity**)&dbcurve, DBLINE, DBARC );
	if (!dbcurve)
	{
		status.Internal( IDS_LEAD_PICK_POINT );
		return status;
	}

	if (dbcurve->Tool() == nullptr)
	{
		status.Internal( IDS_LEAD_TOOL );
		return status;
	}

	CDbContainer* dbcontain = dynamic_cast<CDbContainer*>(dbcurve->Owner());
	if (dbcontain == nullptr)
	{
		// Force a container, as a feature, so we can lead geometry IT643
		m_model->EntityCreate(DBFEATURE, (CDbEntity**)&dbcontain);
		dbcontain->Append(dbcurve, FALSE);
	}

	if (side == 0)
		side = dbcurve->IntGet( STR_CUTSIDE, 0 );

	// Determine our base context for this lead
	int base_context = LEAD_X_LEFT;
	if (side != 0)
		base_context = ((side > 0) ? LEAD_X_LEFT : LEAD_X_RIGHT);

	// If we have a *profile* container, re-order it, and
	//	adjust the base context
	CDbProfile* dbprof = dynamic_cast<CDbProfile*>(dbcontain);
	if (dbprof)
	{
		if ((lead_in != nullptr) && dbprof->IsClosed())
			status += reorder_prof( dbprof, dbcurve );

		if (lead_in)
			lead_junction( dbprof, *lead_in );
		else if (lead_out)
			lead_junction( dbprof, *lead_out );
	}

	if (lead_in)
	{
		int context = base_context | LEAD_X_IN;
		if (dbprof)
			status += lead_prof( dbprof, *lead_in, context );
		else
			status += lead_curve( dbcurve, *lead_in, context );
	}

	if (lead_out)
	{
		int context = base_context | LEAD_X_OUT;
		if (dbprof)
			status += lead_prof( dbprof, *lead_out, context );
		else
			status += lead_curve( dbcurve, *lead_out, context );
	}

	return status;
}


// ==================================================================
//	Automatically lead all tooled profiles in the selection set.
//	Deletes any pre-existing leads.
//
//	Lead:Model:configdb="e:/work/bbi5/advmach/debug/database/cmdb.mdb", setup=1
CReturn	CLead::Auto(
	const CString&	configdb,	// Path and Name of configuration DB
	int				setup,		// index of lead setup table, or <=0 if none
	const CSelector& selector,	// Selector holding profiles to auto-lead
	bool			mark_int,	// TRUE if we need to mark interior profs
	bool			split,		// TRUE if force the split
	int				restrictions )
{
	CReturn status;

	if (setup > 0)
	{
		CDbEntityList proflist;

		if (setup != m_setup)
			status += load_setup( configdb, setup );

		if (status.isOkay())
			status += Reduce( selector, restrictions, &proflist );

		if (status.isOkay() && mark_int)
			status += CModelUtil::MarkIntExt( &proflist, FALSE, selector.pModel() );

		if ( status.IsOk() )
		{
			int count = proflist.Count();

			for (int indx = 0; indx < count; ++indx)
			{
				CDbProfile* dbProfile = dynamic_cast<CDbProfile*>(proflist.GetAt( indx ));
				if ( CanApplyLeads( *dbProfile, restrictions ) )
				{
					status = LeadExperiment( dbProfile, split );
					if ( !status.IsOk() )
						break;
				}
			}
		}
	}

	return status;
}

// =================================================================
//	Load the lead setup table, plus all sub parameter tables
CReturn CLead::load_setup( const CString& configdb, int setup )
{
	CReturn	status;
	CDaoDB	db;

	int setup_table = db.addTable( "Lead Setup" );
	int param_table = db.addTable( "Lead Parameter" );

	status += db.Open( configdb );
	if (status.isOkay())
	{
		if (db.findRecord( setup_table, "ID", setup ) < 0)
		{
			status.Internal( IDS_DB_RECORD, "CMDB", "Lead Setup", setup );
			return status;
		}

		for (int idx=0; idx<LEAD_CONTEXT_NUM; idx++)
		{
			int parm_idx = db.getInt( setup_table, context_name[idx] );

			if (parm_idx > 0)
			{
				status += m_context[idx].Load( db, param_table, parm_idx );
				m_context[idx].Context( (eLeadContext) idx );
			}
		}
	}
	db.Close();

	m_setup = (status.IsOk() ? setup : -1);

	return status;
}

// ==================================================================
//	Load one lead parameter record
CReturn CLead::load_parameter( 
	const CString&	configdb, 
	CLeadData*		data,
	int				parameter )
{
	CReturn	ret;
	CDaoDB	db;

	int param_table = db.addTable( "Lead Parameter" );

	ret += db.Open( configdb );
	if (ret.isOkay())
		ret += data->Load( db, param_table, parameter );

	db.Close();

	return ret;
}


// 2012.04.28 (PE) -- A complete hack.
void CLead::MasterModel( CModel* masterModel )
	{ m_masterModel = masterModel; }

CModel* CLead::MasterModel()
	{ return m_masterModel; }


// ==================================================================
//		LEAD METHODS wrt SELECTOR / PROFILE LIST
// ==================================================================

// ==================================================================
//	Traverse the selection set, and transfer only the tooled profiles
//	into the proflist.
//
//	ALSO... deletes any "Lead" features as it goes...
//
CReturn CLead::Reduce(
	const CSelector& selector, int restrictions, CDbEntityList* proflist )
{
	CReturn status;

	int num = selector.Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* dbent = selector.GetAt( idx );

		// Save the user the trouble of manually
		// deleting existing lead entities.
		CDbFeature* dbfeature = dynamic_cast<CDbFeature*>(dbent);
		if (dbfeature && dbfeature->IsLead())
		{
			selector.pModel()->EntityDelete( dbfeature->Id() );
		}

		CDbProfile* dbprof = dynamic_cast<CDbProfile*>(dbent);
		if (dbprof)
		{
			CDbTool* dbTool = dbprof->Tool();

			// 2016.08.25 (PE) -- Change to avoid processing scribes (Conweld).
			// if (dbTool && !dbTool->IsLayer())
			if (dbTool && dbTool->IsCuttingTool())
			{
				// 2004.07.05 (PE) -- Reported by J.Whitlock @ Warren AL
				if ((restrictions == 0) ||
					((restrictions == 1) && IsClosedForLeads( *dbprof )))
				{
					proflist->Append( (CDbEntity*)dbprof );
				}
			}
		}
	}

	return status;
}

bool CLead::CanApplyLeads( const CDbProfile& dbProfile, int restrictions )
{
	bool can_apply_leads = (dbProfile.Count() > 0);

	if (can_apply_leads && (restrictions == 1) && !IsClosedForLeads( dbProfile ))
		can_apply_leads = false;

	if ( can_apply_leads )
	{
		// We *don't* lead punch profiles.
		CDbTool* dbTool = dbProfile.Tool();
		can_apply_leads = ((dbTool != nullptr) && dbTool->IsLeadTool());
	}

	return can_apply_leads;
}

// 2006.11.01 (PE) -- Some Dynatorch customers were complaining that
// leads were being applying to open profiles. Upon implmenting the
// filter for open profiles, it became apparent that nesting & leads
// were more broken than we understood. In particular, the gaps in
// profiles are filled so that we can Generate() and Assemble() kerf
// in a predictable and robust manner. The entities spanning the gaps
// are attributed with the "is_gap_elem" flag. The improper application
// of leads occurred because the gap-filling entities make a profile
// C0-continuous, causing IsClosed() to always return true.
bool CLead::IsClosedForLeads( const CDbProfile& dbProfile )
{
	// As much as I dislike multiple exit points, it makes life easier here.

	int count = dbProfile.Count();
	if (count < 1)
		return false;

	// NOTE: We must check both the first and final curves
	// because the direction of the profile may have been reversed.

	CDbCurve* ce = (CDbCurve*) dbProfile.GetAt( count-1 );
	if ( ce->IntGet( "is_gap_elem", FALSE ) )
		return false;

	CDbCurve* cs = (CDbCurve*) dbProfile.GetAt( 0 );
	if ( cs->IntGet( "is_gap_elem", FALSE ) )
		return false;

	C3dCoord ps = cs->StartPt( 0 );
	C3dCoord pe = ce->EndPt( 0 );

	if ( ps.WithinTol( pe, SMALL ) )
		return true;

	const CVarList& attribs = dbProfile.Attrib();
	if ((attribs.getInt( "_is_overlapped", 0 ) != 0) ||
		(attribs.getInt( "_is_gapped", 0 ) != 0))
	{
		// The profile (essentially) had to be closed at the
		// time either attribute was applied.
		// ASSUMPTION: The profile has not been modified since leads were last applied.
		return true;
	}

	return false;
}

// ==================================================================
//	Traverse the list of profiles and 
//	generate the appropriate leads on it depending on context
//
// restrictions -- (0) None / (1) Closed Profiles Only
CReturn CLead::gen_lead( 
	CDbEntityList*	proflist,
	bool			split,
	int				restrictions )
{
	CReturn status;

	int count = proflist->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>((*proflist)[indx]);
		if (dbProfile != nullptr)
		{
			if ( CanApplyLeads( *dbProfile, restrictions ) )
				status += autolead_prof( dbProfile, split );
		}
	}

	return status;
}

// ==================================================================
//		LEAD METHODS wrt INDIVIDUAL PROFILE
// ==================================================================

// ==================================================================
//	Locate the split point on the profile
CReturn CLead::autosplit_prof( CDbProfile* dbprof, const CLeadData& data )
{
	CReturn status;

	// Generate a list of lead candidates
	CLeadPointArray lead_array;
	status += Locate( dbprof, data.Location(), data.Tolerance(), &lead_array );

	// ... now snap to an end of intersected element if possible
	if (lead_array.Count())
	{
		CLeadPoint* lead_pt = lead_array[0];

		CDbCurve* dbcurve = (CDbCurve*)(*dbprof)[lead_pt->Index()];

		// ... if couldn't snap, so split the relevant curve.
		if (lead_pt->doSplit() && !lead_pt->AtCorner())
		{
			if ((dbprof->Count() == 1) && dbprof->IsClosed())
			{
				// Must be a circle.
				CDbArc*	dbArc = dynamic_cast<CDbArc*>( dbcurve );
				C3dCoord pc = dbArc->CenterPt();
				C3dCoord ps = (*lead_pt);
				ps.Z( pc.Z() );

				dbArc->Init(
					dbArc->Tool(), dbArc->Workplane(), ps, ps, pc, dbArc->Dir() );
			}
			else
			{
				dbcurve = dbcurve->Split( *lead_pt );
			}
		}

		status += reorder_prof( dbprof, dbcurve );
	}

	// Tidy up and be done
	lead_array.DestructiveFlush();

	return status;
}

// ==================================================================
//	Travel the minimum distance necessary to find the curve
//	that lies on an outside corner (this curve to the next is
//	convex wrt our winding)
//
// Zig-zags back and from from the selected start to find the
// first outside corner.
//
// An alternate implementation would be to find ALL outside 
// corners and use the one physically closest to the start.
CDbCurve* CLead::outside_corner( CDbProfile* dbprof, CDbCurve* start, int winding )
{
	int num = dbprof->Count();
	if (num < 2)
	{ return start; }	// Early exit for circles

	int pos = dbprof->Position( start );

	int cnt=2;
	while (cnt<=num)	// Spin about halfway around the part
	{	
		int delta = -(cnt>>1);
		if (cnt&1) delta = -delta;

		int at = (pos+delta)%num;
		int next = (at + 1)%num;

		if (at < 0) at += num;
		if (next < 0) next += num;

		CDbCurve* at_dbcurve = (CDbCurve*)(*dbprof)[at];
		CDbCurve* next_dbcurve = (CDbCurve*)(*dbprof)[next];

		CGeoCurve* at_curve = at_dbcurve->Curve();
		CGeoCurve* next_curve = next_dbcurve->Curve();
		//
		// Note that tangent arcs give us fits -- we can take the tangent at the start,
		// which will show a tangency, but on a tight arc, COULD be an inside corner.  We 
		// can take the tangent across the chord, but that require more calculation.  It's
		// fairly easy to take the end instead of the start tangent and that SHOULD still
		// give us a sense of where we are going.
		//
		C2dUnitVec at_vec = at_curve->EndTan();
		C2dUnitVec next_vec = next_curve->EndTan();

		delete at_curve;
		delete next_curve;

		if ( SGN(at_vec.PerpDot( next_vec )) == winding)
		{ return next_dbcurve; }

		cnt++;
	}

	// Default, failure, but let's return SOMETHING.
	// We *could* return nullptr here and then do a split case... or something
	return start;	
}


// ==================================================================
//	Given a profile, and a pointer to a curve in it, reorder
//	the profile so that curve is first
CReturn CLead::reorder_prof( CDbProfile* dbprof, const CDbCurve* dbcurve )
{
	CReturn	ret;

	CDbEntity*	dbent = (CDbEntity*)dbcurve;

	int pos = dbprof->Position( (CDbCurve*)dbent );
	while (pos > 0)
	{
		dbent = (*dbprof)[0];
		dbprof->Disown( dbent );
		dbprof->Append( dbent );
		pos--;
	}

	return ret;
}

// ==================================================================
//	Given a profile, apply the appropriate leads to the start
//	and end of it... as sub-features in it's parent feature
//
//	TODO:  Dyna Pierce
//
CReturn CLead::autolead_prof( CDbProfile* dbprof, bool split )
{
	CReturn	status;

	// Global contexts
	int	base_context = dbprof->IntGet( STR_PROFILE_DEPTH, 0 );
	if (base_context & 1) 
		base_context = LEAD_X_INTERNAL;
	else
		base_context = LEAD_X_EXTERNAL;

	// NOTE: We intentionally default to the left.
	if (get_cutside( *dbprof ) < 0)
		base_context |= LEAD_X_RIGHT;
	else
		base_context |= LEAD_X_LEFT;

	// Lead Junction; use lead-in parameters
	int lead_context = base_context | LEAD_X_IN;
	CDbCurve* dbcurve = ActualStartCurve( (*dbprof) );
	if (dbcurve->Type() == DBARC)
		lead_context |= LEAD_X_ARC;
	else
		lead_context |= LEAD_X_LINE;

	CLeadData* leadParams = &m_context[lead_context&LEAD_CONTEXT_MASK];

	if (split && IsClosedForLeads( *dbprof ))
		autosplit_prof( dbprof, *leadParams );

	lead_junction( dbprof, *leadParams );

	// Lead-In
	lead_context = base_context | LEAD_X_IN;

	// We get the start curve again because the profile
	// may have been split.
	dbcurve = ActualStartCurve( (*dbprof) );
	if (dbcurve->Type() == DBARC)
		lead_context |= LEAD_X_ARC;
	else
		lead_context |= LEAD_X_LINE;

	leadParams = &m_context[lead_context&LEAD_CONTEXT_MASK];
	lead_prof( dbprof, *leadParams, lead_context );

	// Lead-Out
	lead_context = base_context | LEAD_X_OUT;

	dbcurve = ActualEndCurve( (*dbprof) );
	if (dbcurve->Type() == DBARC)
		lead_context |= LEAD_X_ARC;
	else
		lead_context |= LEAD_X_LINE;

	leadParams = &m_context[lead_context&LEAD_CONTEXT_MASK];
	lead_prof( dbprof, *leadParams, lead_context );

	return status;
}

// ==================================================================
//	Make sure the start/end of the profile fit the criteria of the lead
//	junction.
//
//	GAP -- move the start and end points back by 1/2 distance, to create
//			a lead gap
//	OVERLAP -- move the end point out over the start by distance, but ONLY
//			if the junction is two parallel lines
//	EXACT -- do nothing
//
// TODO:  What if GAP moves a point farther than the length of the entity? Huh?  Huh?
//
CReturn CLead::lead_junction( CDbProfile* dbprof, const CLeadData& data )
{
	CReturn status;

	CDbCurve* dbStartCurve = ActualStartCurve( (*dbprof) );
	CGeoCurve* geoStartCurve = dbStartCurve->Curve();

	C3dCoord st_pt = geoStartCurve->StartPt();
	C2dVec st_tan = geoStartCurve->StartTan();

	CDbCurve* dbEndCurve = ActualEndCurve( (*dbprof) );
	CGeoCurve* geoEndCurve = dbEndCurve->Curve();

	C3dCoord en_pt = geoEndCurve->EndPt();
	C2dVec en_tan = geoEndCurve->EndTan();

	CDbTool* tool = dbStartCurve->Tool();
	CDbWorkplane* plane = dbStartCurve->Workplane();

	// 2008.07.17 (PE) -- Why were we scaling the user-spec'd distance?
	//   dist = data.Distance() * dbprof->Tool()->EffectiveDiameter();
	double dist = data.Distance();

	switch (data.Junction())
	{
		case LEAD_EXACT:
			break;

		case LEAD_OVERLAP:
		{
			if ( (geoStartCurve->Type() != GEOLINE)
				|| (geoEndCurve->Type() != GEOLINE) )
				break;

			if ( !CLOSE(fabs(st_tan * en_tan), 1.0, VECTOR_SMALL) )
				break;

			if (dbprof->IsAssociated())
				dbprof->Disassociate();

			// TODO:  Incorporate the min value to other junctions?
			if ( (data.MinOverlap() > SMALL)
				&& (dist < data.MinOverlap()) )
			{
				dist = data.MinOverlap();
			}

			// Okay, we have two parallel lines... overlap the geoStartCurve one
			C2dVec lap_vec = st_tan * -dist;
			C3dCoord lap_pt = st_pt + C3dVec( lap_vec.X(), lap_vec.Y(), 0.0 );

			// Guaranteed to be a line type.
			CDbLine* dbline = (CDbLine*)dbStartCurve;

			dbline->Init( tool, plane, lap_pt, dbline->EndPt() );

			// See also CSheet::DropStop()
			dbprof->IntSet("_is_overlapped",1);
		}
		break;

		case LEAD_GAP:
		{
			if (dbprof->IsAssociated())
				dbprof->Disassociate();

			// Back the geoStartCurve and geoEndCurve off by 1/2 distance
			double half_dist = dist/2;

			C2dVec lap_vec = st_tan * half_dist;
			C3dCoord gap_pt = st_pt + C3dVec( lap_vec.X(), lap_vec.Y(), 0.0 );

			switch (dbStartCurve->Type())
			{
				case DBLINE:
				{
					CDbLine* dbline = (CDbLine*)dbStartCurve;

					dbline->Init( tool, plane, gap_pt, 
									dbline->EndPt() );
				}
				break;

				case DBARC:
				{
					CDbArc*	dbarc = (CDbArc*)dbStartCurve;

					C3dCoord close_pt;
					double u;
					((CGeoArc*)geoStartCurve)->PointClosest( gap_pt, &close_pt, &u );

					dbarc->Init( tool, plane, close_pt,
										dbarc->EndPt(),
										dbarc->CenterPt(), dbarc->Dir() );
				}
				break;
			}

			lap_vec = en_tan * -half_dist;
			gap_pt = en_pt + C3dVec( lap_vec.X(), lap_vec.Y(), 0.0 );

			switch (dbEndCurve->Type())
			{
				case DBLINE:
				{
					CDbLine* dbline = (CDbLine*)dbEndCurve;

					dbline->Init( tool, plane, 
										dbline->StartPt(), gap_pt );
				}
				break;

				case DBARC:
				{
					CDbArc*	dbarc = (CDbArc*)dbEndCurve;

					C3dCoord close_pt;
					double u;
					((CGeoArc*)geoEndCurve)->PointClosest( gap_pt, &close_pt, &u );

					dbarc->Init( tool, plane, 
								dbarc->StartPt(), close_pt,
								dbarc->CenterPt(), dbarc->Dir() );
				}
				break;
			}
		}
		break;

		case LEAD_TAB:
		{
			if ( (geoStartCurve->Type() != GEOLINE)
				|| (geoEndCurve->Type() != GEOLINE) )
				break;

			if ( !CLOSE(fabs(st_tan * en_tan), 1.0, VECTOR_SMALL) )
				break;

			if (dbprof->IsAssociated())
				dbprof->Disassociate();

			CDbContainer* db_owner = dynamic_cast<CDbContainer*>(dbStartCurve->Owner());
			if (db_owner == nullptr)
				db_owner = dbprof;

			// Okay, we have two parallel lines... overlap the geoStartCurve one
			C2dVec tab_vec = st_tan * -dist;
			C3dCoord tab_st = st_pt + C3dVec( tab_vec.X(), tab_vec.Y(), data.TabThick()*plane->ToolUp() );
			C3dCoord tab_en = st_pt + C3dVec( 0.0, 0.0, data.TabThick()*plane->ToolUp() );

			CDbLine* db_tab = nullptr;

			m_model->EntityCreate( DBLINE, (CDbEntity**)&db_tab );
			db_tab->Init( tool, plane, tab_st, st_pt );
			AttribsApply( *dbStartCurve, tool, db_tab );

			db_owner->InsertBefore( dbStartCurve, db_tab );
		}
		break;

	}

	delete geoStartCurve;
	delete geoEndCurve;

	return status;
}


// ==================================================================
//	Given a profile and some lead parameters, create a lead
CReturn CLead::lead_prof(
	CDbProfile*			dbprof,
	const CLeadData&	data,
	int					context )
{
	CReturn			status;
	CString			name;
	CDbSequence*	dbSequence;
	CDbEntity*		refCurve;
	CDbEntity*		leadCurve;
	int				pos, indx;

	if (data.Type() == LEAD_NONE)
		return status;

	// We expect to end up with a structure like
	//   Feature
	//     Pierce
	//     LeadIn
	//     Profile
	//     LeadOut
	//
	// IF there is an owner, get it...
	//
	CDbContainer* dbowner = dynamic_cast<CDbContainer*>(dbprof->Owner());
	if (dbowner == nullptr)
	{
		// Instead of wimping out, CREATE an owner so that the leads get placed correctly with
		// respect to the profile.
		//
		CDbFeature* new_owner;
		m_model->EntityCreate( DBFEATURE, (CDbEntity**)&new_owner);
		AttribsApply( *dbprof, nullptr, new_owner );

		new_owner->Append(dbprof);
		dbowner = new_owner;
	}
	else
	{
		CDbFeature* dbParent = dynamic_cast<CDbFeature*>( dbowner );
		
		if (dbParent == nullptr)
		{
			status.Diagnostic("CLead::lead_prof() -- dbParent is NULL");
			status.LastStatusMsgSet( "The parent of some profile is not a feature." );
			return (CReturn(STATUS_ERROR));  // as much as I dislike early returns .....
		}
		else
		{
			// Remove any existing lead
			// BEWARE: LEAD_X_IN is defined as 0x00 :-(
			if (context & LEAD_X_OUT)
				LeadOutRemove( dbParent, dbprof );
			else
				LeadInRemove( dbParent, dbprof );
		}
	}
	m_root = dbowner;

	// Get the actual curve to lead
	CDbCurve* dbcurve = ((context & LEAD_X_OUT) ? ActualEndCurve( *dbprof ) : ActualStartCurve( *dbprof ));

	CDbTool* dbtool = dbcurve->Tool();
	if (!dbtool)
		return CReturn( STATUS_ERROR );

	// Where to place it?
	eLeadSide lead_side = ((context & LEAD_X_RIGHT) ? LEAD_RIGHT : LEAD_LEFT);

	CDbWorkplane* dbwork = dbcurve->Workplane();

	CGeoPoly geoPoly;
	ProfileToGeopoly( *dbprof, &geoPoly );
	geoPoly.Reduce();  // TODO: Is this necessary?

	// Try the "real" lead parameters...
	// and if they fail, try again with "reduced" lead parameters
	double tool_rad = dbtool->EffectiveDiameter()/2.0;

	CGeoCurve* leadgeo[2] = { nullptr, nullptr };


	// Generate some geometry
	if ( data.Type() == LEAD_RAMP )
	{
		CGeoCurve* geoCurve = dbcurve->Curve();
		leadgeo[0] = gen_lead_ramp( *geoCurve, context, lead_side, data.LineAng(), data.UseTilt() );
		delete geoCurve;
	}
	else
	{
		// Verify some values, for validity... IT#27 19 Feb 02 eww
		CString note;
		if (tool_rad < SMALL)
		{
			int station = dbtool->IntGet( STR_NC_CODE_NUMBER, IUNDEFINED );

			status.User( IDS_LEAD_0TOOL, station );
		}
		else if ((data.ArcRad() < SMALL) && (data.LineLen() < SMALL))
		{
			status.User( IDS_LEAD_0LEN );
		}
		else
		{
			CGeoCurve* geoCurve = dbcurve->Curve();

			// Actually generate some leads!
			switch (data.Type())
			{
			case LEAD_ARC:
			case LEAD_LINEARC:
				{
					double radius = data.ArcRad();
					leadgeo[0] = gen_lead_arc( *geoCurve, context, lead_side, data.ArcAng(), radius );
				}
				break;

			case LEAD_LINE:
				{
					double length = data.LineLen();
					leadgeo[0] = gen_lead_line( *geoCurve, context, lead_side, data.LineAng(), length );
				}
				break;

			case LEAD_LINELINE:
				{
					double length = data.LineLen();
					leadgeo[0] = gen_lead_line( *geoCurve, context, lead_side, 0.0, length );
				}
				break;
			}

			delete geoCurve;
			geoCurve = leadgeo[0];

			if ((data.Type() == LEAD_LINEARC) || (data.Type() == LEAD_LINELINE))
			{
				double length = data.LineLen();

				leadgeo[1] = gen_lead_line( *geoCurve, context, lead_side, data.LineAng(), length );
			}
		}
	}
	
	// Apply the lead now
	CDbFeature* dblead = nullptr;

	if (!(context & LEAD_X_OUT)	|| (data.UsePierce() != PIERCE_ONLY))
	{
		// Convert to database entities.. gotta create the children first
		CDbEntity* leaddb[2] = { nullptr, nullptr };

		for (indx=0; indx<2; indx++)
		{
			if (leadgeo[indx])
			{
				if (leadgeo[indx]->Type() == GEOARC)
				{
					m_model->EntityCreate( DBARC, &leaddb[indx] );
					if (leaddb[indx])
						((CDbArc*)leaddb[indx])->Init( dbtool, dbwork, (CGeoArc&)*leadgeo[indx] );
				}
				else // GEOLINE
				{
					m_model->EntityCreate( DBLINE, &leaddb[indx] );
					if (leaddb[indx])
						((CDbLine*)leaddb[indx])->Init( dbtool, dbwork, (CGeoLine&)*leadgeo[indx] );
				}

				AttribsApply( *dbcurve, dbtool, leaddb[indx] );
			}
		}

		// Package the leads such that:
		// 1. The lead-curves are inserted into a lead-feature
		//    in the correct order.
		// 2. The lead-feature is inserted into the owner at the
		//    correct position relative to the reference geoCurve.
		// 3. The lead-curves are inserted into any existing
		//    sequence-objects at the correct position relative
		//    to the reference geoCurve.
		//
		m_model->EntityCreate( DBFEATURE, (CDbEntity**)&dblead );

		if (context & LEAD_X_OUT)
			dblead->StringSet( STR_TYPE, "_lead_out" );
		else
			dblead->StringSet( STR_TYPE, "_lead_in" );
		
		dblead->IntSet("_lead_setup", m_setup);

		AttribsApply( *dbcurve, dbtool, dblead );

		if (context & LEAD_X_OUT)
		{
			// Put the lead-out feature in the correct place
			if (dbowner != nullptr && dbowner != dbprof)
				dbowner->InsertAfter( dbprof, dblead );

			// Prepare sequencing information.
			indx = dbprof->Count() - 1;
			refCurve = (*dbprof)[indx];
			dbSequence = refCurve->Sequence();
			pos = ((dbSequence) ? dbSequence->Position( refCurve ) : -1);

			for (indx = 0; indx < 2; ++indx)
			{
				leadCurve = leaddb[indx];
				if (leadCurve)
				{
					// Add the lead-out geoCurve to the lead-out feature.
					dblead->Append( leadCurve );
					if (pos >= 0)
					{
						// Sequence the lead-out geoCurve.
						dbSequence->InsertAfter( pos, leadCurve );
						++pos;
					}
				}
			}

			// Now, move the profile-based @ commands forward...
			// Todo:  take them all??
			CVar* var = dbprof->Attrib().getVar( "@DROPSTOP" );
			if (var)
			{
				indx = dblead->Count() - 1;
				refCurve = (*dblead)[indx];
				refCurve->pAttrib()->setVar( *var );
				dbprof->AttribDelete( "@DROPSTOP" );
			}
		}
		else  // LEAD_X_IN
		{
			// Put the lead-in feature in the correct place
			if (dbowner != nullptr && dbowner != dbprof)
				dbowner->InsertBefore( dbprof, dblead );

			// Prepare sequencing information.
			refCurve = (*dbprof)[0];
			dbSequence = refCurve->Sequence();
			pos = ((dbSequence) ? dbSequence->Position( refCurve ) : -1);

			for (indx = 0; indx < 2; ++indx)
			{
				leadCurve = leaddb[indx];
				if (leadCurve)
				{
					// Add the lead-in geoCurve to the lead-in feature.
					dblead->Prepend( leadCurve );
					if (pos >= 0)
					{
						// Sequence the lead-out geoCurve.
						dbSequence->InsertBefore( pos, leadCurve );
					}
				}
			}
		}
	}

	// Create PIERCE feature, and stuff a pierce into it...
	if (!(context & LEAD_X_OUT)
		&& (data.UsePierce() != PIERCE_NONE) )
	{
		int poke = ((leadgeo[1]) ? 1 : 0);

		CDbHole* hole = gen_lead_pierce( *leadgeo[poke], dbwork, context, data );

		if (hole)
		{
			if (dbowner != nullptr && dbowner != dbprof)
			{
				if (dblead != nullptr)
				{ dbowner->InsertBefore( dblead, hole ); }
				else
				{ dbowner->InsertBefore( dbprof, hole ); }
			}
		}
	}

	// Tidy and done
	for (indx=0; indx<2; indx++)
	{
		if (leadgeo[indx])
			delete leadgeo[indx];
	}

	return status;
}


// ==================================================================
//	Given a curve and some lead parameters, create a lead
//
//	TODO:  Merge heavy overlap with lead_prof... break out common functions
//
CReturn CLead::lead_curve(
	CDbCurve*			dbcurve,
	const CLeadData&	data,
	int					context )
{
	CReturn			ret;
	CString			name;
	CDbSequence*	dbSequence;
	CDbEntity*		leadCurve;
	CDbEntity*		leaddb[2] = { nullptr, nullptr };
	CGeoCurve*		ref_curve;
	CGeoCurve*		leadgeo[2] = { nullptr, nullptr };
	int				pos, indx;

	if (data.Type() == LEAD_NONE)
		return ret;

	// IF there is an owner, get it...
	CDbContainer* dbowner = dynamic_cast<CDbContainer*>(dbcurve->Owner());
	if (!dbowner)
		return CReturn( STATUS_ERROR );


	CDbWorkplane* dbwork = dbcurve->Workplane();
	CDbTool* dbtool = dbcurve->Tool();
	if (dbtool == nullptr || dbtool->IsLayer())
		return CReturn( STATUS_ERROR );

	// Where to place it?
	eLeadSide lead_side = ((context & LEAD_X_RIGHT) ? LEAD_RIGHT : LEAD_LEFT);

	ref_curve = dbcurve->Curve();

	// Generate some geometry
	if ( data.Type() == LEAD_RAMP )
	{
		leadgeo[0] = gen_lead_ramp(
			(*ref_curve), context, lead_side, data.LineAng(), data.UseTilt() );
	}
	else
	{
		if ((data.Type() == LEAD_ARC) || (data.Type() == LEAD_LINEARC))
		{
			double radius = data.ArcRad();

			leadgeo[0] = gen_lead_arc(
				(*ref_curve), context, lead_side, data.ArcAng(), radius );
		}

		if ((data.Type() == LEAD_LINE) || (data.Type() == LEAD_LINELINE))
		{
			double length = data.LineLen();

			leadgeo[0] = gen_lead_line(
				(*ref_curve), context, lead_side, data.LineAng(), length );
		}

		// curve = leadgeo[0];

		if ((data.Type() == LEAD_LINEARC) || (data.Type() == LEAD_LINELINE))
		{
			double length = data.LineLen();

			leadgeo[1] = gen_lead_line(
				(*leadgeo[0]), context, lead_side, data.LineAng(), length );
		}

	}

	delete ref_curve;


	// Apply the lead now
	//
	// Convert to database entities.. gotta create the children first
	//
	for (indx=0; indx<2; indx++)
	{
		if (leadgeo[indx])
		{
			if (leadgeo[indx]->Type() == GEOARC)
			{
				m_model->EntityCreate( DBARC, &leaddb[indx] );
				if (leaddb[indx])
					((CDbArc*)leaddb[indx])->Init( dbtool, dbwork, (CGeoArc&)*leadgeo[indx] );
			}
			else // GEOLINE
			{
				m_model->EntityCreate( DBLINE, &leaddb[indx] );
				if (leaddb[indx])
					((CDbLine*)leaddb[indx])->Init( dbtool, dbwork, (CGeoLine&)*leadgeo[indx] );
			}

			AttribsApply( *dbcurve, dbtool, leaddb[indx] );
		}
	}

	// Package the leads such that:
	// 1. The lead-curves are inserted into a lead-feature
	//    in the correct order.
	// 2. The lead-feature is inserted into the owner at the
	//    correct position relative to the reference curve.
	// 3. The lead-curves are inserted into any existing
	//    sequence-objects at the correct position relative
	//    to the reference curve.
	//
	CDbFeature* dblead = nullptr;
	m_model->EntityCreate( DBFEATURE, (CDbEntity**)&dblead );

	if (context & LEAD_X_OUT)
		dblead->StringSet( STR_TYPE, "_lead_out" );
	else
		dblead->StringSet( STR_TYPE, "_lead_in" );

	AttribsApply( *dbcurve, dbtool, dblead );

	dbSequence = dbcurve->Sequence();
	pos = ((dbSequence) ? dbSequence->Position( dbcurve ) : -1);

	if (context & LEAD_X_OUT)
	{
		dbowner->InsertAfter( dbcurve, dblead );

		for (indx = 0; indx < 2; ++indx)
		{
			leadCurve = leaddb[indx];
			if (leadCurve)
			{
				dblead->Append( leadCurve );
				if (pos >= 0)
				{
					dbSequence->InsertAfter( pos, leadCurve );
					++pos;
				}
			}
		}
	}
	else // LEAD_X_IN
	{
		dbowner->InsertBefore( dbcurve, dblead );

		for (indx = 0; indx < 2; ++indx)
		{
			leadCurve = leaddb[indx];
			if (leadCurve)
			{
				dblead->Prepend( leadCurve );
				if (pos >= 0)
				{
					dbSequence->InsertBefore( pos, leadCurve );
				}
			}
		}
	}

	// Tidy and done
	for (indx=0; indx<2; indx++)
		if (leadgeo[indx]) delete leadgeo[indx];

	return ret;
}


// ==================================================================
//		LEAD METHODS on an individual element scale
// ==================================================================

// ==================================================================
//	Create a geometric curve that acts as an arc lead, on the passed
//	curve, as indicated by the parameters
CGeoArc* CLead::gen_lead_arc( 
	const CGeoCurve&	curve, 
	int					context, 
	eLeadSide			lead_side, 
	double				angle, 
	double				radius )
{
	CGeoArc* arc = nullptr;

	eLeadContext econtext = (eLeadContext) context;

	if ((fabs(angle) > SMALL) && (radius >= SMALL))
	{
		int sign = ((lead_side == RIGHT) ? 1 : -1);

		double incl_ang = DEG2RAD * angle;

		arc = new CGeoArc();

		if (context & LEAD_X_OUT)
		{
			// Lead-out
			double radians = curve.EndTan().Radians();

			// Point 'vec' towards the arc center point.
			C2dUnitVec vec = C2dUnitVec( radians - (HALFPI * sign) );

			C3dCoord ps = curve.EndPt();
			C3dCoord pc = ps + (vec * radius);

			// Rotate 'vec' by the included angle and reverse its direction.
			vec -= ((incl_ang * sign) + PI);

			C3dCoord pe = pc + (vec * radius);

			arc->Init( ps, pe, pc, -sign );
		}
		else
		{
			// Lead-in
			double radians = curve.StartTan().Radians() + PI;

			// Point 'vec' towards the arc center point.
			C2dUnitVec vec = C2dUnitVec( radians + (HALFPI * sign) );

			C3dCoord pe = curve.StartPt();
			C3dCoord pc = pe + (vec * radius);

			// Rotate 'vec' by the included angle and reverse its direction.
			vec += ((incl_ang * sign) + PI);

			C3dCoord ps = pc + (vec * radius);

			arc->Init( ps, pe, pc, -sign );
		}
	}

	return arc;
}

// ==================================================================
//	Create a geometric curve that acts as a line lead, on the passed
//	curve, as indicated by the parameters
CGeoLine* CLead::gen_lead_line( 
	const CGeoCurve&	curve, 
	int					context, 
	eLeadSide			lead_side, 
	double				angle,			// in Degrees
	double				length )		// in Units
{
	CGeoLine* line = nullptr;

	eLeadContext econtext = (eLeadContext) context;

	if (length >= SMALL)
	{
		int sign = ((lead_side == RIGHT) ? 1 : -1);

		double perp_ang = DEG2RAD * angle;

		line = new CGeoLine();

		if (context & LEAD_X_OUT)
		{
			// Lead-out
			double radians = curve.EndTan().Radians();
			radians -= ((DEG2RAD * angle) * sign);

			C2dVec vec = C2dUnitVec( radians ) * length;

			C3dCoord ps = curve.EndPt();
			C3dCoord pe = ps + C3dVec( vec.X(), vec.Y(), 0.0 );

			line->StartPt( ps );
			line->EndPt( pe );
		}
		else
		{
			// Lead-in
			double radians = curve.StartTan().Radians() + PI;
			radians += ((DEG2RAD * angle) * sign);

			C2dVec vec = C2dUnitVec( radians ) * length;

			C3dCoord pe = curve.StartPt();
			C3dCoord ps = pe + C3dVec( vec.X(), vec.Y(), 0.0 );

			line->StartPt( ps );
			line->EndPt( pe );
		}
	}

	return line;
}


// ==================================================================
//	Create a geometric curve that acts as a ramped lead, on the passed
//	curve, as indicated by the parameters
//
//	NOTES:
//	1. Lead-ins start at Z0, and lead-outs end at Z0
//	2. A 1 degree offset is applied to prevent witness marks (this
//	   causes problems when doing channel work, however rare).
CGeoLine* CLead::gen_lead_ramp( 
	const CGeoCurve&	curve, 
	int					context, 
	eLeadSide			lead_side, 
	double				angle,		// in Degrees
	bool				tilt )
{
	CGeoLine* line = nullptr;
	double radang = angle * DEG2RAD;

	eLeadContext econtext = (eLeadContext) context;

	if (context & LEAD_X_OUT)
	{
		C3dCoord st = curve.EndPt();

		double length = (cos( radang ) * fabs( st.Z() )) / sin( radang );

		if (length >= SMALL)
		{
			double tan_ang = curve.EndTan().Radians();
			double off_ang = tan_ang ;
			if (tilt)
				off_ang -= (DEG2RAD * lead_side);

			C2dUnitVec	en_vec( off_ang );
			C2dVec		en_offset = en_vec * length;

			C3dCoord	en = st + C3dVec( en_offset.X(), en_offset.Y(), 0.0 );
			en.Z( 0.0 );

			line = new CGeoLine( st, en );
		}
	}
	else // LEAD_X_IN
	{
		C3dCoord en = curve.StartPt();

		double length = (cos( radang ) * fabs( en.Z() )) / sin( radang );

		if (length >= SMALL)
		{
			double tan_ang = curve.EndTan().Radians();
			double off_ang = tan_ang;
			if (tilt)
				off_ang += (DEG2RAD * lead_side);

			C2dUnitVec	st_vec( off_ang );
			C2dVec		st_offset = st_vec * -length;

			C3dCoord	st = en + C3dVec( st_offset.X(), st_offset.Y(), 0.0 );
			st.Z( 0.0 );

			line = new CGeoLine( st, en );
		}
	}

	return line;
}


// Since we don't have a CGeoHole class, the
// returned arc simply represents the pierce hole.
// PRECONDITION: gen_lead_pierce() is applicable only to lead-in.
CGeoArc* CLead::gen_lead_pierce( 
	const CGeoCurve&	curve, 
	const CLeadData&	data)
{
	CGeoArc* pierceHole = nullptr;

	CDbWorkplane* dbWork;
	m_model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	CDbTool* dbTool = PierceTool( data, dbWork );
	if (dbTool != nullptr)
	{
		double offset = data.PierceOffset();
		double radius = 0.5 * dbTool->EffectiveDiameter();
		double dist = radius - offset;

		C3dCoord base = curve.StartPt();
		C3dCoord pc = curve.StartPt() - (curve.StartTan() * dist);

		pierceHole = new CGeoArc( pc, radius, CCW );
		pierceHole->IntSet( "~pierce", dbTool->Id() );
	}

	return pierceHole;
}

// =======================================================================
//	Generate a pierce hole
CDbHole* CLead::gen_lead_pierce( 
	const CGeoCurve&	curve, 
	CDbWorkplane*		dbwork,
	int					context, 
	const CLeadData&	data)
{
	double offset = data.PierceOffset();

	if (!(context & LEAD_X_OUT))
	{
		CDbTool* tool = PierceTool( data, dbwork );
		if (!tool)
			return nullptr;

		CDbHole* hole;
		m_model->EntityCreate( DBHOLE, (CDbEntity**)&hole );

		hole->Tool( tool );

		double dist = (tool->EffectiveDiameter()*0.5 - offset);

		C3dCoord	base = curve.StartPt();
		C2dVec		st_vec = curve.StartTan() * -dist;

		hole->Init( tool, dbwork, (base + st_vec), tool->EffectiveDiameter(), 0 );
		hole->StringSet( STR_TYPE, "_pierce" );
		// hole->ColorSet( tool->ColorGet( DCOLOR_BLUE ) );

		return hole;
	}

	return nullptr;
}

// ==================================================================
//	Apply relevant feature attributes to the destination entity.
void CLead::AttribsApply( 
	const CDbEntity&	src_dbent,
	const CDbTool*		dbTool,
	CDbEntity*			dst_dbent )
{
	const CVarList&	src = src_dbent.Attrib();
	CVarList*		dst = dst_dbent->pAttrib();

	int		ival;
	double	dval;

	if (dbTool != nullptr)
	{
		ival = dbTool->ColorGet( DCOLOR_RED );
		dst->setColor( ival );
	}

	ival = src.getInt( STR_CUTSIDE, IUNDEFINED );	// ??? appropriate for leads ??
	if (ival < IUNDEFINED)
		dst->setInt( STR_CUTSIDE, ival );

	ival = src.getInt( LOFF, IUNDEFINED );
	if (ival < IUNDEFINED)
		dst->setInt( LOFF, ival );

	ival = src.getInt( DOFF, IUNDEFINED );
	if (ival < IUNDEFINED)
		dst->setInt( DOFF, ival );

	dval = src.getReal( STR_SPEED, UNDEFINED );
	if (dval < UNDEFINED)
		dst->setReal( STR_SPEED, dval );

	dval = src.getReal( STR_FEED, UNDEFINED );
	if (dval < UNDEFINED)
		dst->setReal( STR_FEED, dval );
}


// ==================================================================
//		SPECIAL CASEs for REPO SUPPORT
// ==================================================================

// ==================================================================
//	Special-case auto-lead for Open Profiles.  Created for use by
//	the Repo system; where it breaks up profiles that need to be
//	led.
//
//	Assumes that the profiles carry marks for interior/exterior
//	as well as CW/CCW -- as derived from the parent closed profile.
//
//	Does NOT delete pre-existing leads.
//
//	2004.07.29 (PE) -- introduced use_winding_flag. When using the
//	applications Create/Auto_Lead function, we do not need to consider
//	the 'winding' flag.  The nesting engine does have to consider
//	the flag, however.
//
CReturn	CLead::AutoOpen(
	const CString&	configdb,	// Path and Name of configuration DB
	int				setup,		// index of lead setup table, or <=0 if none
	bool			use_winding_flag,
	CSelector*		selector )
{
	CReturn			ret;
	CDbEntityList	proflist;

	if (setup > 0)
	{
		if (setup != m_setup)
			ret += load_setup( configdb, setup );

		if (ret.isOkay())
			ret += ReduceOpen( selector, use_winding_flag, &proflist );

		// Generate actual lead sub-features
		if (ret.isOkay())
			ret += gen_lead( &proflist, FALSE, 0 );
	}

	return ret;
}

// ==================================================================
//	Reduces the set of profiles to just the set of open profiles
//	having special lead-control markers.  
//
//	Specialized reduce for AutoOpen, for Repo
//
//	2004.07.29 (PE) -- introduced use_winding_flag. When using the
//	applications Create/Auto_Lead function, we do not need to consider
//	the 'winding' flag.  The nesting engine does have to consider
//	the flag, however.
CReturn CLead::ReduceOpen( 
			CSelector*		selector, 
			bool			use_winding_flag,
			CDbEntityList*	dstlist )
{
	CReturn ret;
	bool keep;

	int num = selector->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* dbent = (*selector)[idx];

		CDbProfile* dbprof = dynamic_cast<CDbProfile*>(dbent);
		if (dbprof && (dbprof->Count() > 0))
		{
			if ( !dbprof->Tool()->IsLayer() && !dbprof->IsClosed() )
			{
				if (use_winding_flag)
					keep = (dbprof->IntGet("winding",IUNDEFINED) != IUNDEFINED);
				else
					keep = true;

				if (keep)
				{
					dstlist->Append( dbprof );
					dbprof->AttribDelete("winding");
				}
			}
		}
	}

	return ret;
}


// ==================================================================
//	A lead setup will have a maximum distance the leads can possibly
//	protrude from the lead point...
//
//	This function (over) estimates that distance and returns it.
double CLead::LeadWidth( const CString& configdb, int setup )
{
	CReturn ret;

	if (setup <= 0)
		return 0.0;

	if (setup != m_setup)
	{ ret += load_setup( configdb, setup ); }

	double width = 0.0;
	for (int idx=0; idx<LEAD_CONTEXT_NUM; idx++)
	{ width = max( width, m_context[idx].LeadWidth() ); }

	return width;
}

CDbTool* CLead::PierceTool( 
	const CString&	configdb,	// Path and Name of configuration DB
	int				setup,		// index of lead setup table, or <0 if pre-loaded
	int				context,	// Lead context to get pierce for
	CDbWorkplane*	dbwork,
	double*			out_dia )
{
	if (setup > 0)
	{
		if (setup != m_setup)
		{ load_setup( configdb, setup ); }
	}

	return PierceTool(m_context[context&LEAD_CONTEXT_MASK], dbwork, out_dia);
}

CDbTool* CLead::PierceTool( 
	const CLeadData& data,
	CDbWorkplane* dbwork,
	double* out_dia )
{
	CDbTool* pierceTool = nullptr;

	int id = data.PierceID();
	if (id > 0)
	{
		m_model->EntityFind(id, (CDbEntity**) &pierceTool, DBTOOL, DBTOOL);
		if (pierceTool != nullptr)
			return pierceTool;
	}

	double diam = data.PierceDiameter();
	int tooltype = data.PierceType();

	if (out_dia)
		(*out_dia) = diam;

	CAutoMod mod;
	CDbEntityList candidateTools;

	double ptol = m_model->Header().getReal( "HolePlusTol", 0.001 );
	double mtol = m_model->Header().getReal( "HoleMinusTol", 0.001 );
	double depth = 0.0;	// Bogus depth; don't have info yet.  TODO: fix

	mod.Init( nullptr, m_model );
	mod.SimilarToolsFind( tooltype, &candidateTools );

	pierceTool = mod.AutoToolFind( candidateTools, ptol, mtol, dbwork, diam, depth );
	if ((pierceTool == nullptr) && (CLead::MasterModel() != nullptr))
	{
		candidateTools.BenignFlush();

		mod.Init( nullptr, CLead::MasterModel() );
		mod.SimilarToolsFind( tooltype, &candidateTools );

		pierceTool = mod.AutoToolFind( candidateTools, ptol, mtol, dbwork, diam, depth );
		if (pierceTool != nullptr)
		{
			// Flag the tool so the client knows what to do with it.
			pierceTool->IntSet( "~create", 1 );
		}
	}

	return pierceTool;
}

// ==================================================================
/**	Locate a good place to put a lead
 *
 *	Operates on the selection set, puts a lead in the clock direction
 *	specified by dir, and uses split as a 0.0 (never) to 1.0 (always)
 *	splitting allowability.  Fills the lead_at point with the new
 *	lead position.
 *
 *	Returns array of coordinates.  This must be destructed when done.
 */
CReturn CLead::Locate( 
	CDbProfile*	db_prof,
	int dir,					///< O'Clock 1-12
	double split, 
	CLeadPointArray* candidate )
{
	CReturn ret;
	//
	//
	if (CReturn::Debug() >= 5)
	{
		CString note;
		CReturn ret;
		note.Format( "====== Locate Leads =======---------" );
		ret.Diagnostic( note );
	}

	// 2006.12.19 (PE) -- Sunflower reported that leads were not in the
	// correct location when using 2o'clock/no-split. So here were simply
	// avoid creating split candidates when 'No Split' is the selected option.
	//
	// An whilst thinking about this, maybe we have to treat full circles
	// in a special manner? For instance, if a circle starts at 180 degrees,
	// it is contradictory to specify 3o'clock/no-split. However, for machines
	// such as a Whitney, it would be necessary to place the lead at 3o'clock.
	// I guess we'll just postpone addressing this special case until someone
	// reports the need. It is also relatively unlikely to become an issue
	// because most imported circles start at 0 degrees.
	//
	// Ugh, 'split' equates back to CLeadData::m_tolerance which, in turn,
	// equates back to the CfgMgr's Lead Parameters / Split Location.
	// See also CLeadData::Load().
	if (split < 0.5)
		ret += split_candidates( candidate, db_prof, dir );

	ret += profile_candidates( candidate, db_prof );

	ret += reduce_candidates( candidate, db_prof, split );
	ret += sort_candidates( candidate, db_prof, dir );

	return ret;
}

// ----------------------------------------------------------------------------
/** Generate Candidates for this path through the part, by splitting the profile
 *	with a dividing line.
 *
 * These may or may not be used, depending on their relevence.
 */
CReturn CLead::split_candidates(
	CLeadPointArray* candidate,		///< Array of candidates to fill.
	CDbProfile*	db_prof,			///< Profile to find split candidates on
	int dir )						///< O'Clock direction where we want leads
{
	CReturn		status;
	C3dCoord	int_pt[2];
	CDbCurve*	dbcurve;
	CGeoCurve*	curve;
	CLeadPoint*	split_pt;
	int			nint, jndx;
	int			count, indx;

	C3dBox& prof_ext = db_prof->Box();
	C2dCoord mid( prof_ext.Xc(), prof_ext.Yc() );

double FUDGE = min( prof_ext.Dx(), prof_ext.Dy() ) / 100.0;
	CGeoLine split_line;
	switch (dir)
	{
	case 12:	// Up
		split_line.StartPt( mid.X(), prof_ext.Ymin()+FUDGE, 0.0 );
		split_line.EndPt( mid.X(), prof_ext.Ymax()+FUDGE, 0.0 );
		break;

	case 1:		// Up-Right
	case 2:		// Right-Up
		split_line.StartPt( prof_ext.Xmin()+FUDGE, prof_ext.Ymin()+FUDGE, 0.0 );
		split_line.EndPt( prof_ext.Xmax()+FUDGE, prof_ext.Ymax()+FUDGE, 0.0 );
		break;

	case 3:		// Right
		split_line.StartPt( prof_ext.Xmin()+FUDGE, mid.Y(), 0.0 );
		split_line.EndPt( prof_ext.Xmax()+FUDGE, mid.Y(), 0.0 );
		break;

	case 4:		// Right-Down
	case 5:		// Down-Right
		split_line.StartPt( prof_ext.Xmin()+FUDGE, prof_ext.Ymax()-FUDGE, 0.0 );
		split_line.EndPt( prof_ext.Xmax()+FUDGE, prof_ext.Ymin()-FUDGE, 0.0 );
		break;

	case 6:		// Down
		split_line.StartPt( mid.X(), prof_ext.Ymax()-FUDGE, 0.0 );
		split_line.EndPt( mid.X(), prof_ext.Ymin()-FUDGE, 0.0 );
		break;

	case 7:		// Down-Left
	case 8:		// Left-Down
		split_line.StartPt( prof_ext.Xmax()-FUDGE, prof_ext.Ymax()-FUDGE, 0.0 );
		split_line.EndPt( prof_ext.Xmin()-FUDGE, prof_ext.Ymin()-FUDGE, 0.0 );
		break;

	case 9:		// Left
		split_line.StartPt( prof_ext.Xmax()-FUDGE, mid.Y(), 0.0 );
		split_line.EndPt( prof_ext.Xmin()-FUDGE, mid.Y(), 0.0 );
		break;

	case 10:	// Left-Up
	case 11:	// Up-Left
		split_line.StartPt( prof_ext.Xmax()-FUDGE, prof_ext.Ymin()+FUDGE, 0.0 );
		split_line.EndPt( prof_ext.Xmin()-FUDGE, prof_ext.Ymax()+FUDGE, 0.0 );
		break;

	default:
		return CReturn( STATUS_ERROR );
	}

	// Find intersection on profile, at end of our split line
	//	TO_DO: Break out the intersect/split functionality to a new method
	count = db_prof->Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbcurve = (CDbCurve*) ((*db_prof)[indx]);
		curve = dbcurve->Curve();

		nint = CSolution::Intersect( split_line, (*curve), TRUE, int_pt );
		delete curve;

		// 2006.12.19 (PE) -- Sunflower reported that leads were not in the
		// correct location when using 2o'clock/no-split.
		for (jndx = 0; jndx < nint; ++jndx)
		{
			split_pt = new CLeadPoint();

			split_pt->XYZ( int_pt[jndx].X(), int_pt[jndx].Y(), int_pt[jndx].Z() );
			split_pt->Index( indx );		// Index to the intersected entity.
			split_pt->Score( -UNDEFINED );	// Default bogus score (unscored).
			split_pt->doSplit( true );		// Mark as a split point.

			candidate->Append( split_pt );
		}
	}

	return status;
}

// ----------------------------------------------------------------------------
/**	 Create candidates from the natural endpoints of the profile.
 * Note there may be gaps in the profile
 *
 * @todo Add split points to the quadrant points on arcs?
 */
CReturn CLead::profile_candidates(
	CLeadPointArray* candidate,		///< Candidate list to expand with profile candidates
	CDbProfile*	db_prof )			///< Profile to tease points from
{
	CReturn		status;
	C3dCoord	ptA;
	C3dCoord	ptB;
	CDbCurve*	dbCurve;
	CLeadPoint* new_pt;
	int			count, indx;

	count = db_prof->Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbCurve = (CDbCurve*) ((*db_prof)[indx]);

		// NOTE: On the first iteration, ptB is undefined.
		ptA = dbCurve->StartPt();
		if ( !ptA.WithinTol( ptB, SMALL ) )
		{
			new_pt = new CLeadPoint();

			new_pt->XYZ( ptA.X(), ptA.Y(), ptA.Z() );		// Coordinate in space
			new_pt->Index( indx  );		// Entity it came from
			new_pt->Score( -1.0 );		// Dummy start score
			new_pt->doSplit( false );	// No, not a split point
			new_pt->AtCorner( true );

			candidate->Append( new_pt );
		}

		// ASSUMPTION: There must always be separation between the
		// start & end point of a given curve.  If not, the curve is
		// either a circle or a zero-arc-length curve.
		ptB = dbCurve->EndPt();

		new_pt = new CLeadPoint();

		new_pt->XYZ( ptB.X(), ptB.Y(), ptB.Z() );		// Coordinate in space
#if BEFORE_V18
		new_pt->Index( indx  );		// Entity it came from
#else
		// 2006.09.09 (PE) -- During beta-testing, Sunflower reported that
		// a 2o'clock lead was being placed at 10o'clock on a square part.
		// So, we attempt to adjust the curve index.

		new_pt->Index( (((indx+1) >= count) ? 0 : indx+1) );  // Entity it came from
#endif
		new_pt->Score( -1.0 );		// Dummy start score
		new_pt->doSplit( false );	// No, not a split point
		new_pt->AtCorner( true );

		candidate->Append( new_pt );

	}

	return status;
}

// ----------------------------------------------------------------------------
/**	Reduce the number of candidates to use.  Basically, if a candidate is
 *	"too close" to an endpoint, remove it.
 */
CReturn CLead::reduce_candidates(
	CLeadPointArray* candidate,		///< List of candidates to reduce
	CDbProfile*	db_prof,			///< Profile that the candidates apply to
	double split )					///< Split tolerance; 0.0 never, 1.0 always
{
	CReturn		status;
	CDbCurve*	dbcurve;
	CGeoCurve*	curve;
	CLeadPoint*	lpt;
	double		uparam;
	int			indx, which_end;

	indx = candidate->Count() - 1;
	while (indx >= 0)
	{
		lpt = (*candidate)[indx];
		if ( lpt->doSplit() )
		{
			dbcurve = (CDbCurve*) ((*db_prof)[ lpt->Index() ]);
			curve = dbcurve->Curve();

			// Determine proximity of split point to curve end point.
			// Reflect so always in range [0.0 .. 0.5].
			uparam = curve->PointUparam( *lpt );

			which_end = ((uparam > 0.5) ? 1 : 0);
			if (which_end == 1)
				uparam = 1.0 - uparam;

			// Eliminate candidates that are to close to an end point.
#if ORIGINAL_CODE
			if (split >= (uparam-SMALL))
				candidate->Remove( indx );
#else
			if (split >= (uparam-SMALL))
			{
				if (which_end == 1)
				{
					int new_index = lpt->Index() + 1;
					
					if (new_index >= db_prof->Count())
						new_index = 0;

					lpt->Index( new_index );
				}

				lpt->AtCorner( true );
			}
#endif

			delete curve;
		}

		--indx;
	}

	return status;
}

/** Test the junction between two curves and see if it is an outside
 * corner (e.g. obtuse or tangent).
 */
bool CLead::is_outside_corner( 
	CGeoCurve* first,	///< Curve leading in to the corner
	CGeoCurve* second,	///< Curve leading out from the corner
	int winding )		///< Winding of the profile.  Important for the test.
{
	C2dUnitVec at_vec = first->EndTan();
	C2dUnitVec next_vec = second->EndTan();

	int turn = SGN(at_vec.PerpDot( next_vec ));

	return (turn == winding);
}

// ----------------------------------------------------------------------------
/** Score and sort candidates in order of best fit to worst fit,
 * according to the specified direction.
 *
 * Here is how the scoring is done.  There are two directions to
 * score, the primary and the secondary.  Primary scores count for
 * more.  Note that the cardinal directions don't have a secondary score.
 */
static const double PRIMARY_FACTOR = 1.0;
static const double SECONDARY_FACTOR = 0.5;
//
static C2dVec g_clock_score[13] =
{
	C2dVec( 0.0, 0.0 ),								// 0 n/a
	C2dVec( SECONDARY_FACTOR, PRIMARY_FACTOR ),		// 1	Y+ X+
	C2dVec( PRIMARY_FACTOR, SECONDARY_FACTOR ),		// 2	X+ Y+
	C2dVec( PRIMARY_FACTOR, 0.0 ),					// 3	X+
	C2dVec( PRIMARY_FACTOR, -SECONDARY_FACTOR ),	// 4	X+ Y-
	C2dVec( SECONDARY_FACTOR, -PRIMARY_FACTOR ),	// 5	Y- X+
	C2dVec( 0.0, -PRIMARY_FACTOR ),					// 6	Y-
	C2dVec( -SECONDARY_FACTOR, -PRIMARY_FACTOR ),	// 7	Y- X-
	C2dVec( -PRIMARY_FACTOR, -SECONDARY_FACTOR ),	// 8	X- Y-
	C2dVec( -PRIMARY_FACTOR, 0.0 ),					// 9	X-
	C2dVec( -PRIMARY_FACTOR, SECONDARY_FACTOR ),	// 10	X- Y+
	C2dVec( -SECONDARY_FACTOR, PRIMARY_FACTOR ),	// 11	Y+ X-
	C2dVec( 0.0, PRIMARY_FACTOR )					// 12	Y+
};
//
//
int compare_candidate( const void* u, const void* v )
{
	CLeadPoint* a = *((CLeadPoint**)u);
	CLeadPoint* b = *((CLeadPoint**)v);

	int score = -1;
	if (a->Score() < b->Score())
	{ score = 1; }

	if (CReturn::Debug() >= 5)
	{
		CString note;
		CReturn ret;
		note.Format( "QSORT %f vs %f tests %d", a->Score(), b->Score(), score );
		ret.Diagnostic( note );
	}

	return score;
}

CReturn CLead::sort_candidates(
	CLeadPointArray* candidate,		///< List of candidates to score and sort
	CDbProfile*	dbProfile,			///< Profile to score candidates against
	int dir )						///< Direction we want to go
{
	CReturn		status;
	CGeoLine	hands;
	C2dVec		vecA;
	C2dVec		vecB;
	C2dUnitVec	uvecA;
	C2dUnitVec	uvecB;
	C2dCoord	ps;
	C2dCoord	pe;
	CLeadPoint*	lead;
	double		dot, udot, diam;
	double		dx, dy;
	int			count, indx;

	hands = ClockVector( (*dbProfile), dir );

	ps = hands.StartPt();
	pe = hands.EndPt();

	dx = pe.X() - ps.X();
	dy = pe.Y() - ps.Y();

	vecA.Init( dx, dy );
	uvecA.Init( dx, dy );

	diam = vecA.Length();

	count = candidate->Count();
	for (indx = 0; indx < count; ++indx)
	{
		lead = (*candidate)[indx];

		dx = lead->X() - ps.X();
		dy = lead->Y() - ps.Y();

		vecB.Init( dx, dy );
		uvecB.Init( dx, dy );

		dot = vecA * vecB;
		udot = uvecA * uvecB;
		if (lead->doSplit() && (udot > 0.95))  // arbitrary
			dot += (diam * diam);

		lead->Score( dot );
	}

	candidate->Qsort( compare_candidate );

	return status;
}

// ============================================================================
//	Actually, winding is just a value we use to determine which side to lead.
//	We now prefer to use the entity's cutside attribute, so we check for that first.
int CLead::get_winding( const CDbProfile& db_prof )
{
	int winding = 0;

	if (db_prof.Count() > 0)
	{
		winding = db_prof.IntGet( "Winding", 0 );
		if (!winding)
		{
			CDbEntity* db_ent = db_prof[0];
			winding = db_ent->IntGet( STR_CUTSIDE, 0 );
		}
	}

	return winding;
}


// ============================================================================

int CLead::get_cutside( const CDbProfile& db_prof )
{
	int	cut_side = 0;

	if (db_prof.Count() > 0)
	{
		CDbEntity* db_ent = ActualStartCurve( db_prof );

		// (1) Left / (0) None / (-1) Right
		cut_side = db_ent->IntGet( STR_CUTSIDE, 0 );
	}

	return cut_side;
}

// Because I'm too lazy to derive the mapping function ....
double g_clock_map[] =
{
    90.,  // 12 o'clock  -- not used?
    60.,  //  1 o'clock
    30.,  //  2 o'clock
     0.,  //  3 o'clock
   330.,  //  4 o'clock
   300.,  //  5 o'clock
   270.,  //  6 o'clock
   240.,  //  7 o'clock
   210.,  //  8 o'clock
   180.,  //  9 o'clock
   150.,  // 10 o'clock
   120.,  // 11 o'clock
   90.    // 12 o'clock
};

CGeoLine CLead::ClockVector( const CDbProfile& dbProfile, int dir )
{
	C3dBox		box;
	C2dCoord	pc;
	C2dUnitVec	uvec;
	CGeoLine	cvec;
	double		dx, dy, rad;
	double		xp, yp;

	box = dbProfile.Box();
	pc.XY( box.Xc(), box.Yc() );

	dx = box.Dx();
	dy = box.Dy();
	rad = sqrt( dx*dx + dy*dy );

	uvec.Radians( DEG2RAD * g_clock_map[dir] );

	xp = pc.X() + (rad * -uvec.X());
	yp = pc.Y() + (rad * -uvec.Y());
	cvec.StartPt( xp, yp, 0. );

	xp = pc.X() + (rad * uvec.X());
	yp = pc.Y() + (rad * uvec.Y());
	cvec.EndPt( xp, yp, 0. );

	return cvec;
}

// ASSUMPTION: The profile will *never* have consecutive
// gap-filling entities and the profile will *never* be
// a single gap-filling entity.
CDbCurve* CLead::ActualStartCurve( const CDbProfile& dbProfile ) const
{
	CDbCurve* dbCurve = (CDbCurve*) dbProfile[0];
	if ( dbCurve->IntGet( "is_gap_elem", FALSE ) )
		dbCurve = (CDbCurve*) dbProfile[1];

	return dbCurve;
}

// ASSUMPTION: The profile will *never* have consecutive
// gap-filling entities and the profile will *never* be
// a single gap-filling entity.
CDbCurve* CLead::ActualEndCurve( const CDbProfile& dbProfile ) const
{
	int count = dbProfile.Count();
	
	CDbCurve* dbCurve = (CDbCurve*) dbProfile[count-1];
	if ( dbCurve->IntGet( "is_gap_elem", FALSE ) )
		dbCurve = (CDbCurve*) dbProfile[count-2];

	return dbCurve;
}

void CLead::LeadInRemove( CDbFeature* dbFeature, const CDbProfile* dbProfile )
{
	// NOTE: All of this checking is probably unnecessary, but ...
	int indx = dbFeature->Position( dbProfile ) - 1;
	while (indx >= 0)
	{
		// dbEntity is the entity immediately preceding the profile.
		CDbEntity* dbEntity = dbFeature->GetAt( indx );

		if (dbEntity->Type() == DBFEATURE)
		{
			CDbFeature* dbLead = (CDbFeature*) dbEntity;
			if ( !dbLead->IsLeadIn() )
				break;

			dbLead->Delete();  // removes from parent feature too
		}
		else if (dbEntity->Type() == DBHOLE)
		{
			CDbHole* dbHole = (CDbHole*) dbEntity;
			if ( !dbHole->IsPierce() )
				break;

			dbHole->Delete();  // removes from parent feature too
		}
		else
		{
			break;
		}

		--indx;
	}
}

void CLead::LeadOutRemove( CDbFeature* dbFeature, const CDbProfile* dbProfile )
{
	int indx = dbFeature->Position( dbProfile ) + 1;

	if ((indx > 0) && (indx < dbFeature->Count()))
	{
		// dbEntity is the entity immediately following the profile.
		CDbEntity* dbEntity = dbFeature->GetAt( indx );

		CDbFeature*	dbLead = dynamic_cast<CDbFeature*>( dbEntity );
		if ((dbLead != nullptr) && dbLead->IsLeadOut())
			dbLead->Delete();  // removes from parent feature too
	}
}

CReturn CLead::LeadExperiment( CDbProfile* dbProfile, bool split )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CReturn status;

	// Convert the profile to a polygon being careful to maintain gap-curves.
	CGeoPoly geoPoly;
	ProfileToGeopoly( *dbProfile, &geoPoly );

	int count = geoPoly.Count();
	if (count > 0)
	{
		TLeadCandidates candidates;
		CLeadCandidate* soln = nullptr;

		int cutSide = geoPoly.IntGet( STR_CUTSIDE, 0 );
		if ((cutSide == 0) && geoPoly.IsClosed( SMALL ))
		{
			bool outer = ((geoPoly.IntGet( STR_PROFILE_DEPTH, 0 ) & 0x01) == 0);
			int winding = geoPoly.Winding();

			// Convention: (cw) winding < 0 / (ccw) winding > 0
			if ( outer )
				cutSide = ((winding < 0) ? LEAD_LEFT : LEAD_RIGHT);
			else
				cutSide = ((winding > 0) ? LEAD_LEFT : LEAD_RIGHT);

			geoPoly.IntSet( STR_CUTSIDE, cutSide );
		}

		// TODO: A potential (minor) optimization is to defer determination
		// of convexity until truly necessary, at which point, 'isConvex'
		// can be cached.
		bool isConvex = geoPoly.IsConvex();

		// Build a list of candidate lead positions.
		CandidatePositionsCalc( geoPoly, &candidates );

		int count = candidates.Count();
		if (count > 0)
		{
			CLeadCandidate* candidate = nullptr;

			int indx;
			for (indx = 0; indx < count; ++indx)
			{
				candidate = candidates.GetAt( indx );

				// Get the corresponding Lead Parameter set based upon
				// the type of curve associated with candidate.
				LeadDataGet( geoPoly, candidate );

				// An "unusable basis" is a basis-candidate whose split u-param
				// is zero. It is deemed unusable because its split point
				// represents the point at its clock-position and not the
				// point at its u-param position. It is ok to ignore such a
				// basis-candidate because another candidate will represent
				// the (no split) corner.
				if ( !candidate->IsUsable() )
					continue;

				// Build the virtual lead entities.
				VirtualLeadsCreate( geoPoly, candidate );

				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				// Check for interference between the lead entities
				// and the polygon curves.
				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

				if (isConvex && candidate->IsExternal())
					break;  // no need to check (assuming good lead params)

				if ( IsLeadOkay( geoPoly, *candidate ) )
					break;
			}

			if (indx < count)
			{
				if ( candidate->IsClosedSoln() )
				{
					PolyReorder( *candidate, &geoPoly );

					if (candidate->LeadInParamsGet().Junction() != LEAD_EXACT)
						LeadsPolyModify( *candidate, &geoPoly );
				}

				// 2017.01.26 (PE) -- Leads on triangular part for MFS York resulted
				// in a split arc at bottom right corner of part, causing problems.
				// PolyReorder() will have reordered the curves so the lead in/out
				// will be respectively at the 0/Nth curves. Therefore, this call
				// call to ArcsReduce() *does not* apply to the fencepost curves.
				geoPoly.ArcsReduce( false, SMALL );

				// LeadsCreate() creates all of the database entities representing
				// the pierce hole, leads and modified profile. NOTE: It replaces
				// the contents of 'dbProfile'.
				status = LeadsCreate( geoPoly, *candidate, dbProfile );
			}
			else
			{
				CString which = status.String(candidate->IsExternal() ? IDS_EXTERNAL : IDS_INTERNAL);
				CString leadParamsName = candidate->ParamsName();
				CString msg = status.Format( IDS_ERROR, IDS_FAILED_LEAD,
					which, m_part_name, leadParamsName );

				status.Diagnostic( (LPCSTR) msg );
			}
		}

		candidates.DestructiveFlush();
	}

	return status;
}

void CLead::ProfileToGeopoly( const CDbProfile& dbProfile, CGeoPoly* geoPoly )
{
	int count = dbProfile.Count();

	if ((geoPoly != nullptr) && (count > 0))
	{
		const CVarList& attribs = dbProfile.Attrib();
		geoPoly->IntSet( STR_PROFILE_DEPTH, attribs.getInt( STR_PROFILE_DEPTH, 0 ) );
		geoPoly->IntSet( STR_CUTSIDE, attribs.getInt( STR_CUTSIDE, 0 ) );

		for (int indx = 0; indx < count; ++indx)
		{
			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbProfile.GetAt(indx) );
			if (dbCurve != nullptr)
			{
				CGeoCurve* geoCurve = dbCurve->Curve();
				if ( dbCurve->HasAttribs() )
				{
					CVarList* attribs = geoCurve->pAttrib();
					(*attribs) = dbCurve->Attrib();
					attribs->setInt( "~dbcurve", (int) dbCurve );
				}

				if (geoCurve->Type() == GEOARC)
				{
					// 2015.01.17 (PE) -- CGeoPoly::PtInPoly() failed on an interior (oblong) profile comprised
					// of two lines and two arcs because the arcs were not decomposed into quadrant-arcs. The
					// solution employed here is a little ugly but it appears to work.
					geoPoly->CopyAppend( *geoCurve );
					delete geoCurve;
				}
				else
				{
					geoPoly->Append( geoCurve );
				}
			}
		}

		if (attribs.getInt( "_is_overlapped", 0 ) != 0)
		{
			// ASSUMPTION: The profile has not been modified since leads were last applied.
			// Because it is the start point that is modified when the lead is created.
			const C3dCoord& pe = geoPoly->EndPt();
			geoPoly->StartPt( pe );
		}
		else if (attribs.getInt( "_is_gapped", 0 ) != 0)
		{
			CInt2d intsect;
			intsect.OnSeg( FALSE );

			// ASSUMPTION: The profile has not been modified since leads were last applied.
			const CGeoCurve& geoCurveA = geoPoly->GetAt(0);
			const CGeoCurve& geoCurveB = geoPoly->GetAt( geoPoly->Count() - 1 );

			const C3dCoord& ps = geoCurveA.StartPt();
			const C3dCoord& pe = geoCurveB.EndPt();
 			C3dCoord mid( (0.5 * (ps.X() + pe.X())), (0.5 * (ps.Y() + pe.Y())), ps.Z() );

			int nint = intsect.CrvCrv( geoCurveA, geoCurveB );
			if (nint > 0)
			{
				int bestIndx = -1;
				double bestDist = UNDEFINED;

				for (int indx = 0; indx < nint; ++indx)
				{
					C3dCoord pt = intsect.Point( indx );
					double dist = pt.DistXY( mid );
					if (dist < bestDist)
					{
						bestIndx = indx;
						bestDist = dist;
					}
				}

				mid = intsect.Point( bestIndx );
				mid.Z( ps.Z() );

				geoPoly->StartPt( mid );
				geoPoly->EndPt( mid );
			}
			else if ((geoCurveA.Type() == GEOLINE) && (geoCurveB.Type() == GEOLINE))
			{
				geoPoly->StartPt( mid );
				geoPoly->EndPt( mid );
			}
		}
	}
}

void CLead::PolyReorder( const CLeadCandidate& candidate, CGeoPoly* geoPoly )
{
	if ( geoPoly->IsCircle( SMALL ) )
	{
		geoPoly->StartPt( candidate.Pt() );
		geoPoly->EndPt( candidate.Pt() );
	}
	else
	{
		int gndx = candidate.CurveIndex();

		// Split the poly curve as necessary.
		if ( geoPoly->Split( gndx, candidate.Pt() ) )
			++gndx;

		// Reorder the polygon so that is starts at the best location.
		geoPoly->Shift( -gndx );
	}
}

void CLead::LeadsPolyModify( const CLeadCandidate& candidate, CGeoPoly* geoPoly )
{
	// This feels so dirty ....

	const CGeoCurve& startCurve = geoPoly->GetAt( 0 );
	CGeoCurve* geoCurveA = (CGeoCurve*) &startCurve;
	geoCurveA->StartPt( candidate.StartPointGet() );

	const CGeoCurve& endCurve = geoPoly->GetAt( geoPoly->Count()-1 );
	CGeoCurve* geoCurveB = (CGeoCurve*) &endCurve;
	geoCurveB->EndPt( candidate.EndPointGet() );
}

// LeadsCreate() creates all of the database entities representing
// the pierce hole, leads and modified profile. NOTE: It replaces
// the contents of 'dbProfile'.
CReturn CLead::LeadsCreate( const CGeoPoly& geoPoly, const CLeadCandidate& candidate, CDbProfile* dbProfile )
{
	CReturn status;

	CDbContainer* dbOwner = dynamic_cast<CDbContainer*>( dbProfile->Owner() );
	if (dbOwner == nullptr)
	{
		// Instead of wimping out, CREATE an owner so that the
		// leads get placed correctly with respect to the profile.
		//
		CDbFeature* new_owner;
		m_model->EntityCreate( DBFEATURE, (CDbEntity**) &new_owner);
		AttribsApply( *dbProfile, nullptr, new_owner );

		new_owner->Append( dbProfile );
		dbOwner = new_owner;
	}
	else
	{
		CDbFeature* dbParent = dynamic_cast<CDbFeature*>( dbOwner );
		if (dbParent == nullptr)
		{
			status.Diagnostic("CLead::LeadsCreate() -- dbParent is NULL");
			status.LastStatusMsgSet( "The parent of some profile is not a feature." );
			return (CReturn(STATUS_ERROR));  // as much as I dislike early returns .....
		}
		else
		{
			// Remove any existing lead
			// BEWARE: LEAD_X_IN is defined as 0x00 :-(
			LeadOutRemove( dbParent, dbProfile );
			LeadInRemove( dbParent, dbProfile );
		}
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Ugly but necessary ....
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	CDbCurve* dbRefCurve = (CDbCurve*) candidate.RefDbCurve();

	CEntityDb* db = dbRefCurve->Db();
	db->PrepareCopy( db );

	CDbCurve* dbTempRefCurve;
	status = db->Copy( *dbRefCurve, (CDbEntity**) &dbTempRefCurve ) ;
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// NOTE: Wrt an open profile, it isn't really necessary to replace
	// the contents of the profile because it wont be reordered (as a
	// closed profile will most likely be). That said, doing so simplifies
	// the source code.
	dbProfile->DestructiveFlush();
	status = ContainerAppend( geoPoly.Curves(), *dbTempRefCurve, dbProfile );

	ProfileAttribsModify( candidate, dbProfile );

	CDbHole* dbPierceHole = nullptr;
	if (status.IsOk() && (candidate.LeadInGeoGet().Count() > 0))
		dbPierceHole = PierceHoleCreate( candidate.LeadInGeoGet() );

	CDbFeature* dbLeadIn = nullptr;
	if (status.IsOk() && (candidate.LeadInGeoGet().Count() > 0))
	{
		m_model->EntityCreate( DBFEATURE, (CDbEntity**) &dbLeadIn );
		status = ContainerAppend( candidate.LeadInGeoGet(), *dbTempRefCurve, dbLeadIn );
	}

	CDbFeature* dbLeadOut = nullptr;
	if (status.IsOk() && (candidate.LeadOutGeoGet().Count() > 0))
	{
		m_model->EntityCreate( DBFEATURE, (CDbEntity**) &dbLeadOut );
		status = ContainerAppend( candidate.LeadOutGeoGet(), *dbTempRefCurve, dbLeadOut );
	}

	if ( status.IsOk() )
	{
		// Arrange the entities in the feature.
		int indx = dbOwner->Position( dbProfile );
		if (indx >= 0)
		{
			CDbEntityArray chInput;
			bool createHull = (IsNestingContext() && candidate.IsClosedSoln());

			if (dbPierceHole != nullptr)
			{
				dbOwner->InsertBefore( indx, dbPierceHole );
				++indx;

				if ( createHull )
					chInput.Append( dbPierceHole );
			}

			if (dbLeadOut != nullptr)
			{
				dbLeadOut->StringSet( STR_TYPE, "_lead_out" );
				AttribsApply( *dbTempRefCurve, nullptr, dbLeadOut );
				dbOwner->InsertAfter( indx, dbLeadOut );

				if ( createHull )
					chInput.Append( dbLeadOut );
			}

			if (dbLeadIn != nullptr)
			{
				dbLeadIn->StringSet( STR_TYPE, "_lead_in" );
				AttribsApply( *dbTempRefCurve, nullptr, dbLeadIn );
				dbOwner->InsertBefore( indx, dbLeadIn );

				if ( createHull )
					chInput.Append( dbLeadIn );
			}

			if (chInput.Count() > 0)
			{
				// Create a CCW convex hull about the lead geomtry. The final
				// hull is used (instead of the lead geomtry) to generate the
				// kerf offset for a nesting part. This is done because juncture
				// of lead and part geometry causes problems for the booleans
				// that create the kerf offset.
				CDbProfile* dbConvexHull = LeadHullCreate( chInput, *dbTempRefCurve );
				if (dbConvexHull != nullptr)
				{
					if (dbPierceHole != nullptr)
						--indx;

					int cutSide = dbTempRefCurve->IntGet( STR_CUTSIDE, 0 );

					// Failure to make the winding direction of the convex hull
					// consistent with the winding direction of the profile
					// causes nesting to overlap parts.
					if (candidate.IsExternal() && (cutSide == LEFT))
						dbConvexHull->Reverse();
					else if (!candidate.IsExternal() && (cutSide == RIGHT))
						dbConvexHull->Reverse();
					
					int pd = dbProfile->IntGet( STR_PROFILE_DEPTH, IUNDEFINED );
					if (pd == 0)
						dbConvexHull->IntSet( STR_LEAD_HULL, -1 );  // outer
					else if (pd == 1)
						dbConvexHull->IntSet( STR_LEAD_HULL, 1 );   // inner

					dbOwner->InsertBefore( indx, dbConvexHull );
				}
			}
		}
	}

	dbTempRefCurve->Delete();

	return status;
}

CReturn CLead::ContainerAppend(
	const CGeoCurveArray& geoCurves,
	const CDbCurve& dbRefCurve,
	CDbContainer* dbContainer )
{
	CReturn status;

	CDbTool* dbTool = dbRefCurve.Tool();
	CDbWorkplane* dbWork = dbRefCurve.Workplane();

	int count = geoCurves.Count();
	if (count > 0)
	{
		CEntityDb& db = m_model->Db();

		for (int indx = 0; indx < count; ++indx)
		{
			const CGeoCurve* geoCurve = geoCurves.GetAt( indx );
			if ( !IsPierceHoleArc( *geoCurve ) )
			{
				CDbEntity* dbEntity = db.GeoConvert( *geoCurve, dbTool, dbWork );
				if (dbEntity == nullptr)
				{
					status.Internal( IDS_INTERNAL_ERROR, "CLead::ContainerAppend()" );
					break;
				}

				AttribsApply( dbRefCurve, dbTool, dbEntity );
				dbContainer->Append( dbEntity, FALSE );
			}
		}
	}

	return status;
}

bool CLead::IsPierceHoleArc( const CGeoCurve& geoCurve )
{
	int toolID = ((geoCurve.Type() == GEOARC) ? geoCurve.IntGet( "~pierce", 0 ) : -1);
	return (toolID > 0);
}

CDbHole* CLead::PierceHoleCreate( const CGeoCurveArray& geoCurves )
{
	CDbHole* dbHole = nullptr;

	CGeoCurve* geoCurve = geoCurves.GetAt(0);
	if ((geoCurve != nullptr) && IsPierceHoleArc( *geoCurve ))
	{
		int toolID = geoCurve->IntGet( "~pierce", 0 );

		CDbTool* dbTool;
		m_model->EntityFind( toolID, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

		CDbWorkplane* dbWork;
		m_model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

		m_model->EntityCreate( DBHOLE, (CDbEntity**) &dbHole );
		dbHole->Init( dbTool, dbWork, ((CGeoArc*) geoCurve)->CenterPt(),
			(0.5 * dbTool->EffectiveDiameter()), 0 );

		// So the hole appears as "Pierce" in the list view.
		dbHole->StringSet( STR_TYPE, "_pierce" );
	}

	return dbHole;
}

double CLead::ClockToRadians( int clockPosition )
{
	// 30 = 360 / 12
	double delta = 30.;
	// 12 o'clock = 90
	double degrees = 90. - ((clockPosition % 12) * delta);
	return (DEG2RAD * degrees);
}

bool CLead::IsGapCurve( const CGeoCurve& geoCurve )
{
	return ( geoCurve.IntGet( "is_gap_elem", 0 ) > 0);
}

void CLead::CandidatePositionsCalc( const CGeoPoly& geoPoly, TLeadCandidates* candidates )
{
	if ( geoPoly.IsClosed( SMALL ) )
		ClosedPositionsCalc( geoPoly, candidates );
	else
		OpenPositionsCalc( geoPoly, candidates );
}

// Build a list of candidate lead positions where each point is calculated
// using the Split Location parameter and where each point records the
// index of the associated polyline segment, being careful to avoid gap-curves.
//
// Sort the candidate lead positions in an "alternating manner" so there
// is no bias in the direction for alternate solutions. For example,
// assume the prefered position is 2 o'clock and that we have alternate
// solutions at exact clock positions. The candidates will be ordered by
// their clock position yielding 2,1,3,12,4, ....
//
// NOTE: Be careful to weed-out multiple solutions having identical clock
//  positions (say 2 o'clock) because we want the outermost solution.
void CLead::ClosedPositionsCalc( const CGeoPoly& geoPoly, TLeadCandidates* candidates )
{
	static double OPPOSITE = -(1. - 1.e-6);
	static double TOL = 1.e-4;  // arbitary

	const CLeadData params = BaseLeadParamsGet( geoPoly );

	const C2dBox& box = geoPoly.Extent();
	double len = ((box.Dx() > box.Dy()) ? box.Dx() : box.Dy());

	double basisRadians = ClockToRadians( params.ClockPosition() );
	C2dUnitVec basisVec( basisRadians );

	// Get the center of the polygon bounding box because this
	// is used as the basis for calculating clock positions.
	C3dCoord pc = geoPoly.Center();
	pc.Z( 0. );  // otherwise lead geometry Z-ordinates are bad.

	C3dCoord ps = pc - (basisVec * len);
	C3dCoord pe = pc + (basisVec * len);

	// A line representing a vector towards the "prefered" solution.
	CGeoLine basisLine( ps, pe );

	// Find the outer-most intersection. Said intersection is used
	// as the basis for sorting subsequent candidate positions.
	CLeadCandidate* basis = LeadBasisGet( basisLine, geoPoly );

	if (basis == nullptr)
	{
		// Something is very wrong.
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CLead::CandidatePositionsCalc(#2)" );
	}
	else
	{
		TLeadCandidates cwCandidates;
		TLeadCandidates ccwCandidates;
		double oppositeRadians = basisRadians + PI;

		CLeadCandidate* candidate = nullptr;

		// Use the corners as seeds for alternate solutions.
		int count = geoPoly.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			const CGeoCurve& geoCurve = geoPoly.GetAt(indx);
			if ( !IsGapCurve( geoCurve ) )
			{
				// TODO: Account for end point of non-gap-curve that is
				// adjacent to a gap-curve.

				// Get the Lead Parameter set for this type of curve.
				//  -- see CLead::autolead_prof() to see how a Lead Param set is obtained.
				CLeadCandidate* candidate = new CLeadCandidate();

				C3dCoord pt = (geoPoly.IsCircle( SMALL )
					? basis->Pt() : geoCurve.PointAtUparam( params.SplitUParam() ));

				C2dUnitVec candidateVec( (pt.X() - pc.X()), (pt.Y() - pc.Y()) );

				double cross = basisVec ^ candidateVec;
				double dot = basisVec * candidateVec;

				double delta = ((dot < OPPOSITE) ? -PI : (acos(dot) * SGN(cross)));

				candidate->IsClosedSoln( true );
				candidate->CurveIndex( indx );
				candidate->Pt( pt );
				candidate->Radians( delta );

				// A dirty little hack introduced for v21.0.181.0
				CDbCurve* dbCurve = (CDbCurve*) geoCurve.IntGet( "~dbcurve", 0 );
				candidate->RefDbCurve( dbCurve );

				if (cross > 0)
					ccwCandidates.Append( candidate );
				else
					cwCandidates.Append( candidate );
			}
		}

		ccwCandidates.Qsort( &CLeadCandidate::IncreasingDelta );
		cwCandidates.Qsort( &CLeadCandidate::DecreasingDelta );

		// Merge the candidates so as to reduce ordering bias.
		candidates->Append( basis );
		while (1)
		{
			CLeadCandidate* candidateA = ccwCandidates.GetAt(0);
			CLeadCandidate* candidateB = cwCandidates.GetAt(0);

			double deltaA = ((candidateA == nullptr) ? UNDEFINED : fabs( candidateA->Radians() ));
			double deltaB = ((candidateB == nullptr) ? UNDEFINED : fabs( candidateB->Radians() ));

			if ((deltaA == UNDEFINED) && (deltaB == UNDEFINED))
				break;  // we've exhausted the candidates.

			if (deltaA <= (deltaB + TOL))
			{
				candidateA = ccwCandidates.Remove(0);
				if (basis->IsUsable() && (deltaA < TOL))  // a minor optimization
					delete candidateA;
				else
					candidates->Append( candidateA );
			}
			else
			{
				candidateB = cwCandidates.Remove(0);
				if (basis->IsUsable() && (deltaB < TOL))  // a minor optimization
					delete candidateB;
				else
					candidates->Append( candidateB );
			}
		}
	}
}

// Find the outer-most intersection. Said intersection is used
// as the basis for sorting subsequent candidate positions.
CLeadCandidate* CLead::LeadBasisGet( const CGeoLine& basisLine, const CGeoPoly& geoPoly )
{
	CLeadCandidate* basis = nullptr;

	C3dCoord refPt = basisLine.StartPt();

	int count = geoPoly.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CGeoCurve& geoCurve = geoPoly.GetAt(indx);
		if ( !IsGapCurve( geoCurve ) )
		{
			C3dCoord intPts[2];
			int nint = CSolution::Intersect( basisLine, geoCurve, TRUE, intPts );

			if (nint > 0)
			{
				double distA, distB;
				int intIndx = 0;

				if (nint == 2)
				{
					// Retain the intersection that is further
					// away (ie. more towards the outside).
					distA = intPts[0].DistXY( refPt );
					distB = intPts[1].DistXY( refPt );
					intIndx = ((distA > distB) ? 0 : 1 );
				}

				bool update = false;
				if (basis == nullptr)
				{
					basis = new CLeadCandidate();
					basis->BasisSet( true );
					
					// A dirty little hack introduced for v21.0.181.0
					CDbCurve* dbCurve = (CDbCurve*) geoCurve.IntGet( "~dbcurve", 0 );
					basis->RefDbCurve( dbCurve );

					update = true;
				}
				else
				{
					distA = basis->Pt().DistXY( refPt );
					distB = intPts[intIndx].DistXY( refPt );
					update = (distB > distA);
				}

				if ( update )
				{
					basis->IsClosedSoln( true );
					basis->CurveIndex( indx );
					basis->Pt( intPts[intIndx] );
					basis->Radians( 0. );
				}
			}
		}
	}

	if ((basis != nullptr) && !geoPoly.IsCircle( SMALL ))
	{
		// Account for splitting at some u-param as necessary.
		const CLeadData baseParams = BaseLeadParamsGet( geoPoly );

		double uparam = baseParams.SplitUParam();
		if (uparam > 0.)
		{
			const CGeoCurve& geoCurve = geoPoly.GetAt( basis->CurveIndex() );
			C3dCoord pt = geoCurve.PointAtUparam( uparam );
			basis->Pt( pt );
		}
	}

	return basis;
}

void CLead::OpenPositionsCalc( const CGeoPoly& geoPoly, TLeadCandidates* candidates )
{
	CLeadCandidate* candidate = new CLeadCandidate();
	candidate->IsClosedSoln( false );
	candidate->CurveIndex( 0 );

	// A dirty little hack introduced for v21.0.181.0 (but not caught till v21.0.193.0 via a crash).
	CDbCurve* dbCurve = (CDbCurve*) geoPoly[0].IntGet( "~dbcurve", 0 );
	candidate->RefDbCurve( dbCurve );

	candidates->Append( candidate );
}

const CLeadData CLead::BaseLeadParamsGet( const CGeoPoly& geoPoly )
{
	int depth = geoPoly.IntGet( STR_PROFILE_DEPTH, -999 );
	int context = ((depth & 0x01) ? LEAD_X_INTERNAL: LEAD_X_EXTERNAL);

	// Per conversation with Gary (2012.04.21) we agreed the way lead data
	// is presented in the CfgMan *is not* correct. In particular, the
	// Lead Location & Split Location should be presented only once each
	// for both the inside and outside use-cases. As currently presented,
	// it is possible to provide incompatible data. As such, we have agreed
	// the Internal Line In and External Line In parameters establish the
	// precedence for the Lead Location & Split Locations of their respective
	// use-cases.
	eLeadContext baseContext = (eLeadContext) (context | LEAD_X_IN | LEAD_X_LINE);

	if (m_context[baseContext].Context() == LEAD_CONTEXT_INVALID)
	{
		eLeadContext trialContext = (eLeadContext) (context | LEAD_X_IN | LEAD_X_ARC);
		if (m_context[trialContext].Context() != LEAD_CONTEXT_INVALID)
			baseContext = trialContext;
	}

	CLeadData leadParams = m_context[baseContext];
	ConditionalSplitParamOverride( geoPoly, &leadParams );

	return leadParams;
}

const CLeadData CLead::LeadInParamsGet(
	const CGeoPoly& geoPoly, const CLeadCandidate& candidate )
{
	int depth = geoPoly.IntGet( STR_PROFILE_DEPTH, -999 );
	int context = ((depth & 0x01) ? LEAD_X_INTERNAL: LEAD_X_EXTERNAL);

	// Get the reference curve.
	const CGeoCurve& geoCurve = geoPoly.GetAt( candidate.CurveIndex() );
	int curveContext = ((geoCurve.Type() == GEOARC) ? LEAD_X_ARC : LEAD_X_LINE);

	eLeadContext leadContext = (eLeadContext) (context | LEAD_X_IN | curveContext);

	CLeadData leadParams = m_context[leadContext];
	ConditionalSplitParamOverride( geoPoly, &leadParams );

	return leadParams;
}

const CLeadData CLead::LeadOutParamsGet(
	const CGeoPoly& geoPoly, const CLeadCandidate& candidate )
{
	int depth = geoPoly.IntGet( STR_PROFILE_DEPTH, -999 );
	int context = ((depth & 0x01) ? LEAD_X_INTERNAL: LEAD_X_EXTERNAL);

	// Get the reference curve.
	const CGeoCurve& geoCurve = geoPoly.GetAt( candidate.CurveIndex() );
	int curveContext = ((geoCurve.Type() == GEOARC) ? LEAD_X_ARC : LEAD_X_LINE);

	eLeadContext leadContext = (eLeadContext) (context | LEAD_X_OUT | curveContext);

	CLeadData leadParams = m_context[leadContext];
	ConditionalSplitParamOverride( geoPoly, &leadParams );

	return leadParams;
}
	
void CLead::ConditionalSplitParamOverride( const CGeoPoly& geoPoly, CLeadData* leadParams )
{
	if (geoPoly.IsCircle( SMALL ) && (leadParams->SplitUParam() == 0.5))
	{
		// For a circle, "always split" is non-sensical because it is
		// contrary to the specified clock-position. A such, we override
		// the split behavior with "at interesection" aka "never split".
		leadParams->SplitUParam( 0. );
	}
}

// ASSUMPTION: candidate.CurveIndex() does not point to a gap-curve.
void CLead::LeadDataGet( const CGeoPoly& geoPoly, CLeadCandidate* candidate )
{
	int depth = geoPoly.IntGet( STR_PROFILE_DEPTH, -999 );
	if (depth >= 0)
	{
		CLeadData baseParams = BaseLeadParamsGet( geoPoly );
		CLeadData leadInParams = LeadInParamsGet( geoPoly, *candidate );
		CLeadData leadOutParams = LeadOutParamsGet( geoPoly, *candidate );

		if ( !HaveCompatibleLeads( leadInParams, leadOutParams ) )
		{
			CReturn status;
			status.User( IDS_MISMATCHED_CLOCK_POSITIONS );

			leadInParams.Context( LEAD_CONTEXT_INVALID );
			leadOutParams.Context( LEAD_CONTEXT_INVALID );
		}

		if ( !geoPoly.IsClosed( SMALL ) )
			((CLeadData&) leadInParams).Junction( LEAD_EXACT );  // Otherwise crashes occur downstream.

		candidate->LeadInParamsSet( leadInParams );
		candidate->LeadOutParamsSet( leadOutParams );
		candidate->BaseParamsOverride( baseParams );
	}
}

bool CLead::HaveCompatibleLeads(
	const CLeadData& leadInParams, const CLeadData& leadOutParams )
{
	bool compatible = true;

	if ((leadInParams.Context() != LEAD_CONTEXT_INVALID) &&
		(leadOutParams.Context() != LEAD_CONTEXT_INVALID))
	{
		compatible = (leadInParams.ClockPosition() == leadOutParams.ClockPosition());
	}

	return compatible;
}

void CLead::VirtualLeadsCreate( const CGeoPoly& geoPoly, CLeadCandidate* candidate )
{
	VirtualLeadJunction( geoPoly, candidate );
	VirtualLeadInCreate( geoPoly, candidate );
	VirtualLeadOutCreate( geoPoly, candidate );
}

void CLead::VirtualLeadJunction( const CGeoPoly& geoPoly, CLeadCandidate* candidate )
{
	const CLeadData& inParams = candidate->LeadInParamsGet();

	candidate->StartPointSet( candidate->Pt() );
	candidate->EndPointSet( candidate->Pt() );

	if (inParams.Junction() != LEAD_EXACT)
	{
		const C3dCoord& splitPt = candidate->Pt();
		double dist = inParams.Distance();

		// TODO: Must be careful (everywhere) to avoid using gap-entities.

		int indx = candidate->CurveIndex();
		const CGeoCurve& scrv = geoPoly.GetAt( indx );

		if ( scrv.StartPt().WithinTolXY( splitPt, SMALL) )
			--indx;

		if (indx < 0)
			indx = geoPoly.Count() - 1;

		const CGeoCurve& ecrv = geoPoly.GetAt( indx );

		switch (inParams.Junction())
		{
		case LEAD_GAP:
			{
				C3dCoord closestPt;
				double u;

				C2dUnitVec vec = scrv.TanAtPt( splitPt );
				C3dCoord gapPt = splitPt + (vec * (0.5 * dist));
				if (scrv.Type() == GEOARC)
				{
					const CGeoArc& geoArc = (const CGeoArc&) scrv;
					geoArc.PointClosest( gapPt, &closestPt, &u );
					gapPt = closestPt;
				}

				candidate->StartPointSet( gapPt );

				vec = ecrv.TanAtPt( splitPt );
				gapPt = splitPt - (vec * (0.5 * dist));
				if (ecrv.Type() == GEOARC)
				{
					const CGeoArc& geoArc = (const CGeoArc&) ecrv;
					geoArc.PointClosest( gapPt, &closestPt, &u );
					gapPt = closestPt;
				}

				candidate->EndPointSet( gapPt );
			}
			break;

		case LEAD_OVERLAP:
			if ( IsValidJunction( scrv, ecrv ) )
			{
				// We have two parallel lines so overlap the start one.
				C2dUnitVec vec = scrv.TanAtPt( splitPt );

				double minOverlap = inParams.MinOverlap();
				if ((minOverlap > SMALL) && (dist < minOverlap))
					dist = minOverlap;

				C3dCoord pt = splitPt - (vec * dist);
				candidate->StartPointSet( pt );
			}
			break;

		case LEAD_TAB:
			if ( IsValidJunction( scrv, ecrv ) )
			{
				// TODO: Verify this is a wood-only setting !!!!
				C2dUnitVec svec = scrv.StartTan();
				C2dUnitVec evec = ecrv.EndTan();

			}
			break;
		}
	}
}

bool CLead::IsValidJunction( const CGeoCurve& scrv, const CGeoCurve& ecrv )
{
	bool isValid = false;

	if ((scrv.Type() == GEOLINE) && (ecrv.Type() == GEOLINE))
	{
		C2dUnitVec svec = scrv.StartTan();
		C2dUnitVec evec = ecrv.EndTan();

		double dot = svec * evec;
		isValid = ((1. - dot) < VECTOR_SMALL);
	}

	return isValid;
}

void CLead::VirtualLeadInCreate( const CGeoPoly& geoPoly, CLeadCandidate* candidate )
{
	const CLeadData& inParams = candidate->LeadInParamsGet();

	CGeoCurve* refCurve = nullptr;
	if ( candidate->IsClosedSoln() )
	{
		int indx = candidate->CurveIndex();

		// NOTE: I have a sneaking suspicion the way we obtain the
		// curve-type and the associated lead parameters may have to
		// be changed if overlap (or gap?) is in play.
		const CGeoCurve& tmp = geoPoly.GetAt( indx );
		const C3dCoord& ps = candidate->StartPointGet();
		if ( tmp.EndPt().WithinTolXY( ps, SMALL ) )
		{
			++indx;
			if (indx >= geoPoly.Count())
				indx = 0;

			refCurve = (CGeoCurve*) geoPoly.GetAt( indx ).Clone( false );
		}
		else
		{
			refCurve = (CGeoCurve*) geoPoly.GetAt( indx ).Clone( false );
			refCurve->StartPt( ps );
		}
	}
	else
	{
		refCurve = (CGeoCurve*) geoPoly.GetAt(0).Clone( false );
	}

	CGeoCurveArray leadIn;
	CGeoCurve* leadCurve;

	eLeadContext leadInContext = inParams.Context();
	eLeadSide leadInSide = (eLeadSide) geoPoly.IntGet( STR_CUTSIDE, LEFT );

	if ((inParams.Type() == LEAD_ARC) || (inParams.Type() == LEAD_LINEARC))
	{
		double radius = inParams.ArcRad();

		leadCurve = gen_lead_arc(
			*refCurve, leadInContext, leadInSide, inParams.ArcAng(), radius );

		if (leadCurve != nullptr)
			leadIn.Append( leadCurve );
	}
	else if ((inParams.Type() == LEAD_LINE) || (inParams.Type() == LEAD_LINELINE))
	{
		double length = inParams.LineLen();
		double angle = ((inParams.Type() == LEAD_LINELINE) ? 0. : inParams.LineAng());

		leadCurve = gen_lead_line(
			*refCurve, leadInContext, leadInSide, angle, length );

		if (leadCurve != nullptr)
			leadIn.Append( leadCurve );
	}

	delete refCurve;


	if ((inParams.Type() == LEAD_LINEARC) || (inParams.Type() == LEAD_LINELINE))
	{
		double length = inParams.LineLen();

		leadCurve = leadIn.GetAt(0);
		leadCurve = gen_lead_line(
			(*leadCurve), leadInContext, leadInSide, inParams.LineAng(), length );

		if (leadCurve != nullptr)
			leadIn.Prepend( leadCurve );
	}


	if ((inParams.UsePierce() != PIERCE_NONE) && (leadIn.Count() > 0))
	{
		leadCurve = leadIn.GetAt(0);

		CGeoArc* pierceHole = gen_lead_pierce( (*leadCurve), inParams );

		if (pierceHole != nullptr)
			leadIn.Prepend( pierceHole );
	}

	candidate->LeadInGeoSet( &leadIn );
}

void CLead::VirtualLeadOutCreate( const CGeoPoly& geoPoly, CLeadCandidate* candidate )
{
	const CLeadData& outParams = candidate->LeadOutParamsGet();

	CGeoCurve* refCurve = nullptr;
	if ( candidate->IsClosedSoln() )
	{
		int indx = candidate->CurveIndex();

		// NOTE: I have a sneaking suspicion the way we obtain the
		// curve-type and the associated lead parameters may have to
		// be changed if overlap (or gap?) is in play.
		const CGeoCurve& tmp = geoPoly.GetAt( indx );
		const C3dCoord& pe = candidate->EndPointGet();
		if ( tmp.StartPt().WithinTolXY( pe, SMALL ) )
		{
			--indx;
			if (indx < 0)
				indx = geoPoly.Count() - 1;

			refCurve = (CGeoCurve*) geoPoly.GetAt( indx ).Clone( false );
		}
		else
		{
			refCurve = (CGeoCurve*) geoPoly.GetAt( indx ).Clone( false );
			refCurve->EndPt( pe );
		}
	}
	else
	{
		int count = geoPoly.Count();
		refCurve = (CGeoCurve*) geoPoly.GetAt(count-1).Clone( false );
	}

	CGeoCurveArray leadOut;
	CGeoCurve* leadCurve;

	eLeadContext leadOutContext = outParams.Context();
	eLeadSide leadOutSide = (eLeadSide) geoPoly.IntGet( STR_CUTSIDE, LEFT );

	if ((outParams.Type() == LEAD_ARC) || (outParams.Type() == LEAD_LINEARC))
	{
		double radius = outParams.ArcRad();

		leadCurve = gen_lead_arc(
			*refCurve, leadOutContext, leadOutSide, outParams.ArcAng(), radius );

		if (leadCurve != nullptr)
			leadOut.Append( leadCurve );
	}
	else if ((outParams.Type() == LEAD_LINE) || (outParams.Type() == LEAD_LINELINE))
	{
		double length = outParams.LineLen();
		double angle = ((outParams.Type() == LEAD_LINELINE) ? 0. : outParams.LineAng());

		leadCurve = gen_lead_line(
			*refCurve, leadOutContext, leadOutSide, angle, length );

		if (leadCurve != nullptr)
			leadOut.Append( leadCurve );
	}

	delete refCurve;


	if ((outParams.Type() == LEAD_LINEARC) || (outParams.Type() == LEAD_LINELINE))
	{
		double length = outParams.LineLen();

		leadCurve = leadOut.GetAt(0);
		leadCurve = gen_lead_line(
			(*leadCurve), leadOutContext, leadOutSide, outParams.LineAng(), length );

		if (leadCurve != nullptr)
			leadOut.Append( leadCurve );
	}

	candidate->LeadOutGeoSet( &leadOut );
}

// IsLeadOkay() is not robust but it should be sufficient. It determines whether
// a candidate lead intersects the part; an intersection is, of course, a bad thing.
// When no intersections are found, for closed profiles, it also determines whether
// any end point of a lead curve is "inside the part".
bool CLead::IsLeadOkay( const CGeoPoly& geoPoly, const CLeadCandidate& candidate )
{
	CInt2d intersector;

	int pndx, pcnt = geoPoly.Count();
	int indx, icnt = candidate.LeadInGeoGet().Count();
	int ondx, ocnt = candidate.LeadOutGeoGet().Count();

	for (pndx = 0; pndx < pcnt; ++pndx)
	{
		const CGeoCurve& pcrv = geoPoly.GetAt( pndx );

		for (indx = (icnt-1); indx >= 0; --indx)
		{
			const CGeoCurve* icrv = candidate.LeadInGeoGet().GetAt( indx );

			int num = intersector.CrvCrv( pcrv, *icrv );
			if (num > 0)
			{
				// There should only be one intersection *ever* and that
				// should be with the end-point of the Nth lead-in curve.
				if (indx < (icnt-1))
					break;

				if (num > 1)
					break;

				if ( !intersector.Point(0).WithinTol( icrv->EndPt(), SMALL ) )
					break;
			}
		}

		if (indx >= 0)
			break;

		for (ondx = 0; ondx < ocnt; ++ondx)
		{
			const CGeoCurve* ocrv = candidate.LeadOutGeoGet().GetAt( ondx );

			int num = intersector.CrvCrv( pcrv, *ocrv );
			if (num > 0)
			{
				// There should only be one intersection *ever* and that
				// should be with the start-point of the 0th lead-out curve.
				if (ondx > 0)
					break;

				if (num > 1)
					break;

				if ( !intersector.Point(0).WithinTol( ocrv->StartPt(), SMALL ) )
					break;
			}
		}

		if (ondx < ocnt)
			break;
	}

	bool ok = (pndx >= pcnt);
	if (ok && geoPoly.IsClosed( SMALL ))
	{
		for (indx = 0; indx < icnt; ++indx)
		{
			const CGeoCurve* icrv = candidate.LeadInGeoGet().GetAt( indx );

			bool inside = geoPoly.PtInPoly( icrv->StartPt() );

			ok = (candidate.IsExternal() ? !inside : inside);
			if ( !ok )
				break;
		}

		if ( ok )
		{
			for (ondx = 0; ondx < ocnt; ++ondx)
			{
				const CGeoCurve* ocrv = candidate.LeadOutGeoGet().GetAt( ondx );

				bool inside = geoPoly.PtInPoly( ocrv->EndPt() );

				ok = (candidate.IsExternal() ? !inside : inside);
				if ( !ok )
					break;
			}
		}
	}

	return ok;
}

int CLead::get_cutside( const CGeoPoly& geoPoly )
{
	int	cutSide = 0;

	if (geoPoly.Count() > 0)
	{
		// CDbEntity* db_ent = ActualStartCurve( db_prof );
		const CGeoCurve& geoCurve = geoPoly.GetAt( 0 );

		// (1) Left / (0) None / (-1) Right
		cutSide = geoCurve.IntGet( STR_CUTSIDE, 0 );
	}

	return cutSide;
}

void CLead::ProfileAttribsModify( const CLeadCandidate& candidate, CDbProfile* dbProfile )
{
	CVarList* attribs = dbProfile->pAttrib();
	if (attribs != nullptr)
	{
		dbProfile->AttribDelete( "_is_overlapped" );
		dbProfile->AttribDelete( "_is_gapped" );

		switch ( candidate.LeadInParamsGet().Junction() )
		{
		case LEAD_OVERLAP: dbProfile->IntSet( "_is_overlapped", 1 );  break;
		case LEAD_GAP:     dbProfile->IntSet( "_is_gapped", 1 );      break;
		}
	}
}

CDbProfile* CLead::LeadHullCreate( const CDbEntityArray& chInput, const CDbCurve& dbRefCurve )
{
	static double CHORD_TOL = 1.e-3;  // 1.e-3
	CDbProfile* dbConvexHull = CModelUtil::CHPartOutlineCreate( chInput, CHORD_TOL, m_model );
	if (dbConvexHull != nullptr)
	{
		CDbTool* dbTool = dbRefCurve.Tool();
		AttribsApply( dbRefCurve, dbTool, dbConvexHull );

		// Values for STR_LEAD_HULL (-1) applied to outside profile / (1) applied to inside profile.
		// NOTE: The value of STR_LEAD_HULL is updated by the client of LeadHullCreate().
		dbConvexHull->IntSet( STR_LEAD_HULL, 1 );

		int count = dbConvexHull->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbConvexHull->GetAt( indx ) );
			dbCurve->Tool( dbTool );
			AttribsApply( dbRefCurve, dbTool, dbCurve );
		}
	}

	return dbConvexHull;
}

CLeadCandidate::CLeadCandidate()
{
	m_is_basis = false;
	m_is_closed = true;
	m_dbcurve = nullptr;
}

CLeadCandidate::~CLeadCandidate()
{
	m_leadInGeo.DestructiveFlush();
	m_leadOutGeo.DestructiveFlush();
}

bool CLeadCandidate::IsUsable() const
{
	return (m_is_basis ? (m_leadInParams.SplitUParam() > SMALL) : true);
}

// NOTE: Will always return false unless 'm_leadInParams' is initialized.
bool CLeadCandidate::IsExternal() const
{
	bool isExternal = false;

	eLeadContext context = m_leadInParams.Context();
	if (context != LEAD_CONTEXT_INVALID)
		isExternal = ((context & LEAD_X_EXTERNAL) != 0);

	return isExternal;
}

void CLeadCandidate::LeadInParamsSet( const CLeadData& leadInParams )
{
	m_leadInParams = leadInParams;
}

void CLeadCandidate::LeadOutParamsSet( const CLeadData& leadOutParams )
{
	m_leadOutParams = leadOutParams;
}

void CLeadCandidate::BaseParamsOverride( const CLeadData& baseParams )
{
	m_leadInParams.ClockPosition( baseParams.ClockPosition() );
	m_leadInParams.SplitUParam( baseParams.SplitUParam() );

	m_leadOutParams.ClockPosition( baseParams.ClockPosition() );
	m_leadOutParams.SplitUParam( baseParams.SplitUParam() );
}

void CLeadCandidate::LeadInGeoSet( CGeoCurveArray* geoCurves )
{
	m_leadInGeo.DestructiveFlush();
	
	int count = geoCurves->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = geoCurves->Replace( indx, nullptr );
		m_leadInGeo.Append( curve );
	}
}

void CLeadCandidate::LeadOutGeoSet( CGeoCurveArray* geoCurves )
{
	m_leadOutGeo.DestructiveFlush();
	
	int count = geoCurves->Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = geoCurves->Replace( indx, nullptr );
		m_leadOutGeo.Append( curve );
	}
}

int CLeadCandidate::IncreasingDelta( const void* ptrA, const void* ptrB )
{
	CLeadCandidate* candidateA = (*(CLeadCandidate**) ptrA);
	CLeadCandidate* candidateB = (*(CLeadCandidate**) ptrB);

	double diff = candidateB->Radians() - candidateA->Radians();
	if (fabs(diff) < SMALL)  // arbitrary tolerance
		return 0;

	return ((diff > 0) ? -1 : 1);
}

int CLeadCandidate::DecreasingDelta( const void* ptrA, const void* ptrB )
{
	CLeadCandidate* candidateA = (*(CLeadCandidate**) ptrA);
	CLeadCandidate* candidateB = (*(CLeadCandidate**) ptrB);

	double diff = candidateB->Radians() - candidateA->Radians();
	if (fabs(diff) < SMALL)  // arbitrary tolerance
		return 0;

	return ((diff > 0) ? 1 : -1);
}
