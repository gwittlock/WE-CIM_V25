
#include "Reposition.h"

#include <Math.h>
#include "StringConst.h"
#include "DbIterator.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbHole.h"
#include "DbLine.h"
#include "DbProfile.h"
#include "DbPattern.h"
#include "DbSequence.h"
#include "ModelUtil.h"
#include "Solution.h"
#include "Lead.h"
#include "Selector.h"
#include "Profile.h"
#include "ProfileBuilder.h"
#include "Conversion.h"


// ============================================================================

#define REPO_ZONE_NAME	"_repo_zone_%d"
#define CLAMP_ZONE_NAME	"_repo_zone_%d"

// ============================================================================

CReposition::CReposition( CModel* model, CNestConfig* config)
{
	m_model = model;
	m_config = config;
	m_clamp_right = 0.0;
}

CReposition::~CReposition()
{
}

// ============================================================================

CReturn CReposition::Create()
{
	CReturn ret = collect();
	if (m_config->Progressive())
	{ return build_progressive(); }


	ret += build();

	ret += large();

	ret += split();

	ret += assign();
	//
	//
	CDbEntityList burn_orphan_list;
	CDbEntityList punch_orphan_list;
	manage_orphans(&m_burn_list, &burn_orphan_list);
	manage_orphans(&m_punch_list, &punch_orphan_list);

	if ( burn_orphan_list.Count()
		|| punch_orphan_list.Count() )
	{
		ret.Block(IDS_REPO_LINT);
	}

	// TODO: Roll together zone repos before the purge
	ret += CModelUtil::EmptyContainers( (*m_model), true );

	return ret;
}

// ============================================================================
// Collect all Punch entities into one list, and all Burn entities into
// another list.  Note that patterns are marked for punch/burn as well.
// While doing this, clear the entity's repo flags as well.
//
//	Specifically recognizes:
//		Hole
//		Curve (not in profile)
//		Profile
//		Pattern Instance
//		Feature
//
//	Fills the lists m_punch_list and m_burn_list
//
CReturn CReposition::collect()
{
	CReturn ret;

	m_punch_list.BenignFlush();
	m_burn_list.BenignFlush();

	bool scribe_with_torch = (m_model->Header().getInt("ScribeWithTorch", 0) == 1);

	CDbEntity::NewAction();

	CDbIterator iter;
	iter.Init(m_model->Db(), DBLINE);
	while (true)
	{
		CDbEntity* db_ent = iter();
		if (!db_ent)
		{ break; }
		iter.Next();

		if (db_ent->IsDeleted())
		{ continue; }

		db_ent = parent(db_ent);
		if (db_ent->DidAction())
		{ continue; }

		CDbContainer* db_contain = dynamic_cast<CDbContainer*>(db_ent);
		if (db_contain)
		{ content_do_action(db_contain); }
		db_ent->DoAction();
		//
		//
		if (m_model->is_hidden(db_ent))
		{ continue; }

		db_ent->AttribDelete(ZONE_CLEAR);
		db_ent->AttribDelete(ZONE_BLOCKED);
		
		switch (db_ent->Type())
		{
			case DBLINE:
			case DBARC:
			case DBHOLE:
			case DBPROFILE:
			case DBFEATURE:
			{
				CDbTool* db_tool = tool(db_ent);
				if (!db_tool)
				{ m_burn_list.Append(db_ent); }
				else
				if (!db_tool->IsLayer())
				{
					if (db_tool->IsPunchTool())
					{ m_punch_list.Append(db_ent); }
					else
					{ m_burn_list.Append(db_ent); }
				}
			}
			break;

			case DBCOMMAND:
			{
				CDbPattern* db_pat = get_pattern((CDbCommand*)db_ent);
				if (db_pat != NULL)
				{
					if ( db_pat->IsA(PATTERN_MARK_BURN)
						|| db_pat->IsA(PATTERN_MARK_ALL) )
					{ m_burn_list.Append(db_ent); }
					else
					if (db_pat->IsA(PATTERN_MARK_PUNCH))
					{ m_punch_list.Append(db_ent); }
					else
					if (db_pat->IsA(PATTERN_MARK_OTHER))
					{
						if (scribe_with_torch)
						{ m_burn_list.Append(db_ent); }
						else
						{ m_punch_list.Append(db_ent); }
					}
					else
					{
						// Un-marked pattern, so we assume its a burn list.
						// User-defined patterns will often be un-marked.
						m_burn_list.Append(db_ent);
					}
				}
			}
			break;
		}
	}

	return ret;
}

// ============================================================================
// Given an entity, see if it belongs in a profile.  If it does, return the
// profile it belongs in.  If it does NOT, just return the entity.
//
CDbEntity* CReposition::parent( CDbEntity* db_ent)
{
	CDbEntity* owner = db_ent->Owner();

	CDbProfile* db_prof = dynamic_cast<CDbProfile*>(owner);
	if (db_prof)
	{ 
		CDbEntity* root = CModelUtil::FindLeadIn(db_prof);
		if (root->Owner() == db_prof)
		{ return db_prof; }

		return parent(root); 
	}
	//
	// Special processing for leads and their profile
	CDbFeature* db_feat = dynamic_cast<CDbFeature*>(owner);
	if ( db_feat
		&& db_feat->IsLeadIn() )
	{
		db_feat = dynamic_cast<CDbFeature*>(db_feat->Owner());
		if (db_feat)
		{
			content_do_action(db_feat);
			return db_feat;
		}
	}

	return db_ent;
}

//
// Mark the DoAction() flag for the contents of this container...
// but NOT the container itself.
//
void CReposition::content_do_action( CDbContainer* contain)
{
	int num = contain->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* ent = (*contain)[idx];
		ent->DoAction();

		CDbContainer* sub_contain = dynamic_cast<CDbContainer*>(ent);
		if (sub_contain)
		{ content_do_action(sub_contain); }
	}
}

// ============================================================================
// TODO:  Merge this blob of code into model, db, policy, or SOMEWHERE common
CDbPattern* CReposition::get_pattern( CDbCommand* db_cmd)
{
	if (!db_cmd->IsInstance())
	{ return NULL; }

	ID patID = db_cmd->IntGet( "patid", 0 );

	// cast away const.
	CDbPattern*	dbPattern = NULL;
	m_model->Db().Find( patID, (CDbEntity**)&dbPattern, DBPATTERN, DBPATTERN );

	return dbPattern;
}


// ============================================================================
// Create each reposition zone and mark the entities as to which zone they 
// belong to.  
// A maximum of 32 zones are supported by the zone flags.
//
CReturn CReposition::build()
{
	CReturn ret;
	CString note;
	
	m_zone_list.DestructiveFlush();

	// Startup the zones
	double sheet_dx = m_model->Header().getReal("length", 0.0);

	double torch_offset = m_config->MachineReal("Torch Offset in X", 0.0);
	double punch_left = m_config->MachineReal("Min_Travel_Limit_X", 0.0);
	double burn_left = punch_left + torch_offset;

	bool repo_to_torch = m_config->RepoToBurn();
	if ( repo_to_torch
		&& EQUAL(burn_left, punch_left) )
	{ repo_to_torch = false; }

	double offset = punch_left;
	bool clamp_repo = false;

	double machine_dx = m_config->MachineRepoTravel();
	if (ZERO(machine_dx))
	{ machine_dx = m_config->MachineReal( "Max_Travel_Limit_X", 0.0 ) 
					- m_config->MachineReal("Min_Travel_Limit_X", 0.0); }
	machine_dx -= m_config->RepoOverlap();

	if (machine_dx < SMALL)
	{
		ret.UserWarn(IDS_REPO_OVERLAP);
		return ret;
	}

	// Data needed to repo around the clamp
	build_clamps(ZONE_PUNCH);
	build_clamps(ZONE_BURN);

	double clamp_buffer = m_config->MachineReal( "Clamp_Buffer", 0.0 );
	double clamp_step = sheet_dx;
	if (!m_punch_clamp_array.Count())
	{
		clamp_step = 0.0;
		// This test is actually redundant, since we will never get an 
		// exclusion if there are no clamps.  But I prefer to be explicit, 
		// in case something changes later.
	}
	else
	{
		if (m_config->RepoSmall())
		{
			double clamp_dx = max( m_config->MachineReal("Punch_Clamp_Deadzone_Length", 0.0),
										m_config->MachineReal("Torch_Clamp_Deadzone_Length", 0.0) );
			clamp_dx += clamp_buffer*2.0;

			clamp_step = clamp_dx;
		}
		else
		{
			for (int cidx=0; cidx<m_punch_clamp_array.Count()-1; cidx++)
			{
				C2dBox* c1_box = m_punch_clamp_array[cidx];
				(*c1_box) += (*m_burn_clamp_array[cidx]);

				C2dBox* c2_box = m_punch_clamp_array[cidx+1];
				(*c2_box) += (*m_burn_clamp_array[cidx+1]);

				double delta = c2_box->Xc() - c1_box->Xc();
				clamp_step = min(clamp_step, delta);
			}
			clamp_step = (clamp_step * 0.5);
		}
	}

	// Generate zones and assign entities to them
	if (CReturn::Debug() >= 3)
	{
		note.Format("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		ret.Diagnostic(note);
		note.Format("Build Zones;  machine dx %f, clamp dx %f", machine_dx, clamp_step);
		ret.Diagnostic(note);
	}

	bool done = false;
	while ( !done || clamp_repo )
	{
		if (CReturn::Debug() >= 3)
		{
			note.Format("----(TOP)----");
			ret.Diagnostic(note);
		}
		// Process current zones
		//
		CZone* zone = build_zone(offset, ZONE_PUNCH);
		zone->Major(!clamp_repo);

		if (CReturn::Debug() >= 3)
		{ zone->Dump(); }

		int zone_num = m_zone_list.Append(zone);
		bool excluded = mark_zone(zone, zone_num);

		if (!repo_to_torch)
		{
			zone = build_zone(offset, ZONE_BURN);
			zone->Major(!clamp_repo);

			if (CReturn::Debug() >= 3)
			{ zone->Dump(); }

			zone_num = m_zone_list.Append(zone);
			if (mark_zone(zone, zone_num))
			{ excluded = true; }
		}
		else
		{ 
			// Move from punch to burn
			double torch_offset = offset +  (punch_left - burn_left);

			// Do pickup-punching
			zone = build_zone(torch_offset, ZONE_PUNCH);
			zone->Major(false);

			if (CReturn::Debug() >= 3)
			{
				note.Format("Torch offset to %f.  Do pickup punching.", torch_offset);
				ret.Diagnostic(note);
				zone->Dump();
			}

			zone_num = m_zone_list.Append(zone);
			if (mark_zone(zone, zone_num))
			{ excluded = true; }

			zone = build_zone(torch_offset, ZONE_BURN);
			zone->Major(!clamp_repo);

			if (CReturn::Debug() >= 3)
			{ zone->Dump(); }

			zone_num = m_zone_list.Append(zone);
			if (mark_zone(zone, zone_num))
			{ excluded = true; }
		}

		if (zone->pExtent()->Xmax() >= (sheet_dx-SMALL))	
		{ done = true; }

		// Perform reposition move
		// Clamp move if we just did a major zone, and we had clamp interference
		// Major move if we just did a clamp move, or there is no interference
		m_clamp_right -= offset;
		if (clamp_repo)
		{ 
			clamp_repo = false;
			offset -= clamp_step;
		}
		else
		if ( m_config->ClampUnder()
			&& excluded
			&& clamp_step )
		{ 
			clamp_repo = true;
			offset += clamp_step;

			if (CReturn::Debug() >= 3)
			{
				note.Format("Clamp offset to %f.", offset);
				ret.Diagnostic(note);
			}
		}
		if (!clamp_repo)
		{ 
			offset += machine_dx;

			if (CReturn::Debug() >= 3)
			{
				note.Format("Major offset to %f.", offset);
				ret.Diagnostic(note);
			}
		}
		m_clamp_right += offset;
		
		// Verify that this move did not put the clamp off the sheet
		double off_sheet = m_clamp_right - sheet_dx;
		if (off_sheet > SMALL)
		{ 
			offset -= off_sheet;

			if (CReturn::Debug() >= 3)
			{
				note.Format("(offsheet, backup to %f)", offset);
				ret.Diagnostic(note);
			}
		}

		// TODO: Negative reposition around the last clamp?
	}

	return ret;
}

// ============================================================================
// Create each reposition zone and mark the entities as to which zone they 
// belong to.  Specific to the Progressive reposition algorithm.
//
//	Stolen DIRECTLY from the old repo system.
//
CReturn CReposition::build_progressive()
{
	CReturn ret;

	double torch_offset = m_config->MachineReal("Torch Offset in X", 0.0);
	double step = m_config->MachineRepoTravel();
	if (ZERO(step))
	{ step = m_config->MachineReal( "Max_Travel_Limit_X", 0.0 ) 
					- m_config->MachineReal("Min_Travel_Limit_X", 0.0); }

	double sheet_dx = m_model->Header().getReal("length", 0.0);
	double width = m_model->Header().getReal("width", 0.0);

	if ( ZERO(step) 		|| ZERO(torch_offset) )
	{
		if (torch_offset < -SMALL)
		{
			ret.UserWarn( IDS_PROG_ZEROSTEP );
			return ret;
		}
	}

	// First, explode the patterns
	//	Stolen from CPPattern's explode
	CDbIterator	iter;
	iter.Init( m_model->Db(), DBPATTERN );
	while (true)
	{
		CDbPattern* dbPattern = dynamic_cast<CDbPattern*>( iter() );
		if (dbPattern == NULL)
			break;

		iter.Next();

		ret += CModelUtil::PatternExplode( (*dbPattern) );
	}

	// Now remove all of the existing zones
	//	Stolen from CPZone's explode
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

			// Find any associated sequence object.
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

	// Different areas for burn (not-holes) and punch (holes)
	double burn_right = step;
	double punch_right = step;

	if (torch_offset < -SMALL)
	{ punch_right = -(torch_offset + step); }
	else
	{ burn_right = torch_offset - step; }

	double zone_left = 0.0;
	double zone_right = max(burn_right, punch_right);

	C3dBox burn_column( 0.0, 0.0, -LARGE, burn_right, width, LARGE );
	C3dBox punch_column( 0.0, 0.0, -LARGE, punch_right, width, LARGE );

	// Selectors.  Let's see if we can run two simultaneously.
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
	punch_sel.Filter( DBCOMMAND, 1 );	// Because this is what Franklin needs
	punch_sel.Restrictions( FALSE );

	// Now expand our zones and collect geometry until done
	int burn_first = 0;
	int punch_first = 0;
	int zone_idx = 0;
	int zone_num = (int)ceil(sheet_dx / step) + 1;
	for (int cnt=0; cnt<zone_num; cnt++)
	{
		// Create the repo-zone feature
		CDbFeature* db_zone = NULL;
		m_model->EntityCreate( DBFEATURE, (CDbEntity**)&db_zone );

		CString name;
		name.Format( REPO_ZONE_NAME, zone_idx+1 );
		name = db_zone->NameConvert( name );
		db_zone->SystemName( name );

		// Zone numbers are [1..N] to make VB-side easier.
		db_zone->Owner( NULL );
		db_zone->IntSet( "_zone_num", (zone_idx+1) );

		//	Some zone attributes...
		db_zone->StringSet( STR_TYPE, "_zone" );
		db_zone->DoubleSet( "_zone_left", 0.0 );
		db_zone->DoubleSet( "_zone_top", 0.0 );
		db_zone->DoubleSet( "_zone_right", zone_right );
		db_zone->DoubleSet( "_zone_bottom", width );

		// Now, fill this zone
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

		// Advance to the next column and zone
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

	CModelUtil::EmptyContainers( (*m_model), true );

	return ret;
}

// ============================================================================
// Create the definition for the clamps; generic, relative to the zone.
void CReposition::build_clamps( eZoneType type)
{
	double clamp_dx;
	double clamp_dy;
	C2dBoxArray* clamp_array;
	m_clamp_right = 0.0;

	if (type == ZONE_PUNCH)
	{
		clamp_dx = m_config->MachineReal("Punch_Clamp_Deadzone_Length", 0.0);
		clamp_dy = m_config->MachineReal("Punch_Clamp_Deadzone_Width", 0.0);
		clamp_array = &m_punch_clamp_array;
	}
	else // ZONE_BURN
	{
		clamp_dx = m_config->MachineReal("Torch_Clamp_Deadzone_Length", 0.0);
		clamp_dy = m_config->MachineReal("Torch_Clamp_Deadzone_Width", 0.0);
		clamp_array = &m_burn_clamp_array;
	}
	clamp_dx *= 0.5;
	clamp_array->DestructiveFlush();

	double clamp_top;
	double clamp_btm;
	if (m_config->YNegative())
	{ 
		clamp_top = m_model->Header().getReal("width", 0.0);
		clamp_btm = clamp_top - clamp_dy;
	}
	else
	{
		clamp_top = clamp_dy;
		clamp_btm = 0.0; 
	}

	for (int indx = 0; indx < m_config->ClampCountGet(); indx++)
	{
		if (m_config->UseClamp(indx))
		{
			C2dBox* clamp = new C2dBox();
			clamp->Xmin(m_config->ClampPos(indx) - clamp_dx);

			m_clamp_right = m_config->ClampPos(indx) + clamp_dx;
			clamp->Xmax(m_clamp_right);

			clamp->Ymax(clamp_top);
			clamp->Ymin(clamp_btm);

			clamp_array->Append(clamp);
		}
	}
}

// ============================================================================
// Create a Zone structure at the given offset position.  Includes offset-adjusted
//	clamp exlusion boxes.
//
CZone* CReposition::build_zone(
	double offset,	///< Reposition offset for this zone
	eZoneType type)
{
	CZone* zone = new CZone();

	// Primary zone info
	zone->Type(type);
	zone->Offset(offset);
	zone->Repo(UNDEFINED);	// The actual Repo amt is the difference between successive offsets

	// TODO:  Refacteo.  These various values and calculates are getting messy from tuning...
	double clamp_offset = m_config->MachineReal("Min_Travel_Limit_X", 0.0);
	if (type == ZONE_BURN)
	{ 
		double torch = m_config->MachineReal("Torch Offset in X", 0.0);
		offset += torch;
		clamp_offset -= torch;
	}

	C2dBox* extent = zone->pExtent();
	extent->Xmin( m_config->MachineReal("Min_Travel_Limit_X", 0.0) + offset );

	double machine_dx = m_config->MachineRepoTravel();
	if (!ZERO(machine_dx))
	{ extent->Xmax(extent->Xmin() + machine_dx); }
	else
	{ extent->Xmax(m_config->MachineReal("Max_Travel_Limit_X", 0.0) + offset); }

	extent->Ymin( 0.0 );
	extent->Ymax( m_model->Header().getReal("width", 0.0) );

	// Clamp exclusions
	C2dBoxArray* clamp_array;
	if (type == ZONE_PUNCH)
	{ clamp_array = &m_punch_clamp_array; }
	else // ZONE_BURN
	{ clamp_array = &m_burn_clamp_array; }

	C2dBoxArray* exclude = zone->pExclude();
	for (int idx=0; idx<clamp_array->Count(); idx++)
	{
		C2dBox* clamp = new C2dBox((*(*clamp_array)[idx]));
		clamp->Shift(offset+clamp_offset, 0.0);
		m_clamp_right = clamp->Xmax();

		exclude->Append(clamp);
	}

	// If the clamp is off the edge of the zone, ADJUST the damn zone
	// so the clamp is inside it.
	if (m_clamp_right > extent->Xmax())
	{ extent->Xmax(m_clamp_right); }

	return zone;
}

// ============================================================================
// Find and mark all entities that fit in the zone
//
// Entities in a zone are flagged on "_clear_zone"
// Contained but blocked are "_blocked_zone"
//	2^n where n=zone index (0+)
//
//	For a given zone, traverse entities and mark the entities.
//
bool CReposition::mark_zone( CZone* zone, int zone_num )
{
	bool excluded = false;

	CDbEntityList* ent_list = ((zone->Type() == ZONE_PUNCH) ? &m_punch_list : &m_burn_list);

	int zone_bit = (int)pow( (double) 2, (double) (zone_num-1));
	//
	//
	C2dBoxArray* clamp_array = zone->pExclude();

//	C2dBox* zone_extent = zone->pExtent();
	C2dBox zone_extent(*(zone->pExtent()));
	zone_extent.Ymin(zone_extent.Ymin()-1);
	zone_extent.Ymax(zone_extent.Ymax()+1);

	int num = ent_list->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*ent_list)[idx];
		// 
		// Where does the entity lie?
		//
		C2dBox extent;
		if (db_ent->Type() == DBCOMMAND)
		{
			CDbCommand* db_cmd = (CDbCommand*)db_ent;
			CDbPattern* pattern = get_pattern(db_cmd);
			extent = pattern->Box();
			// ??? Instance Rotation!!!
			// double angle = DEG2RAD * command->DoubleGet( "instang", 0.0 );
			// ???

			C3dCoord pt = db_cmd->Coord(0);
			extent.Shift(pt.X(), pt.Y());
		}
		else
		{ extent = db_ent->Box(0); }
		//
		// Is it in our zone?
		//
		if (zone_extent.Contains(extent, SMALL))
		{
			// Is it exluded by a clamp?
			//
			bool under_clamp = false;
			for (int idx=0; idx<clamp_array->Count(); idx++)
			{
				C2dBox* clamp = (*clamp_array)[idx];
				if (clamp->Intersects(extent, SMALL))
				{
					under_clamp = true;
					excluded = true;
					break;
				}
			}
			//
			//
			CString name;
			if (under_clamp)
			{ name = ZONE_BLOCKED; }
			else
			{ name = ZONE_CLEAR; }

			int flag = db_ent->IntGet(name, 0);
			flag |= zone_bit;
			db_ent->IntSet(name, flag);
		}
	}

	return excluded;
}

// ============================================================================
//	The inverse of mark_zone.
//	For a given entity, traverse zones and mark the entity.
CReturn CReposition::mark_entity( CDbEntity* db_ent, eZoneType type)
{
	CReturn ret;

	db_ent->AttribDelete(ZONE_CLEAR);
	db_ent->AttribDelete(ZONE_BLOCKED);


	int num = m_zone_list.Count();

	for (int idx=0; idx<num; idx++)
	{
		CZone* zone = m_zone_list[idx];

		if (zone->Type() != type)
		{ continue; }

		int zone_bit = (int)pow( (double) 2, (double) idx );
		//
		//
		C2dBoxArray* clamp_array = zone->pExclude();

		C2dBox zone_extent(*(zone->pExtent()));
		zone_extent.Ymin(zone_extent.Ymin()-1);
		zone_extent.Ymax(zone_extent.Ymax()+1);

		C2dBox extent;
		if (db_ent->Type() == DBCOMMAND)
		{
			CDbCommand* db_cmd = (CDbCommand*)db_ent;
			CDbPattern* pattern = get_pattern(db_cmd);
			extent = pattern->Box();
			// ??? Instance Rotation!!!
			// double angle = DEG2RAD * command->DoubleGet( "instang", 0.0 );
			// ???

			C3dCoord pt = db_cmd->Coord(0);
			extent.Shift(pt.X(), pt.Y());
		}
		else
		{ extent = db_ent->Box(0); }

		// Is it in our zone?
		if (zone_extent.Contains(extent, SMALL))
		{
			// Is it exluded by a clamp?
			bool under_clamp = false;
			for (int idx=0; idx<clamp_array->Count(); idx++)
			{
				C2dBox* clamp = (*clamp_array)[idx];

				if (clamp->Intersects(extent, SMALL))
				{
					under_clamp = true;
					break;
				}
			}

			CString name;
			if (under_clamp)
			{ name = ZONE_BLOCKED; }
			else
			{ name = ZONE_CLEAR; }

			int flag = db_ent->IntGet(name, 0);
			flag |= zone_bit;
			db_ent->IntSet(name, flag);
		}

	}

	return ret;
}


// ============================================================================
// For each entity that does NOT have a ZONE_CLEAR membership, we need to 
//	split it based on the clamps on the zone it was blocked from.
//
//	The split bits are then re-assembled and allocated as needed.
//
CReturn CReposition::split()
{
	CReturn ret;

	CDbEntityList burn_orphan_list;
	manage_orphans(&m_burn_list, &burn_orphan_list);

	CDbEntityList punch_orphan_list;
	manage_orphans(&m_punch_list, &punch_orphan_list);

	// 2007.04.19 (PE) -- Not sure if we should use 'true' or 'false' here.
	// Since the code seems to have worked prior to changing EmptyContainers(),
	// we use 'true' because that matches the previous behavior.
	ret += CModelUtil::EmptyContainers( (*m_model), true );
	//
	// TODO:  Delete anything that is too small to split
	//
	if ( burn_orphan_list.Count()
		|| punch_orphan_list.Count() )
	{
		// Re-do our previous work to manage the newly exploded stuff
		//
		ret += collect();
		ret += build();
		//
		// Now, do the REAL splitting
		//
		ret += manage_orphans(&m_burn_list, &burn_orphan_list);
		ret += manage_orphans(&m_punch_list, &punch_orphan_list);

		ret += profile_strip(&burn_orphan_list);
		ret += profile_strip(&punch_orphan_list);

		ret += profile_split(&burn_orphan_list);
		ret += profile_split(&punch_orphan_list);
	}

	return ret;
}


// ============================================================================
// Create the actual work zones and assign entities to them.
CReturn CReposition::assign()
{
	CReturn ret;

	CDbEntity::NewAction();

	CZone* prev_zone = NULL;
	int znum = m_zone_list.Count();
	for (int zidx=0; zidx<znum; zidx++)
	{
		CZone* zone = m_zone_list[zidx];

		generate_zone(zone, zidx);
		CDbFeature* db_zone = zone->Feature();

		//	A REPO command...
		if (prev_zone)
			db_zone->DoubleSet( "_zone_repo", zone->Offset() - prev_zone->Offset() );
		else // first zone
			db_zone->DoubleSet( "_zone_repo", zone->Offset() );

		// --------------------------------------------------------------------
		// Now make assignments into it
		//
		CDbEntityList* entity_list;
		if (zone->Type() == ZONE_PUNCH)
		{ entity_list = &m_punch_list; }
		else
		{ entity_list = &m_burn_list; }

		for (int jdx=0; jdx<entity_list->Count(); jdx++)
		{
			CDbEntity* db_ent = (*entity_list)[jdx];
			if (db_ent->DidAction())
			{ continue; }
				
			int flag = db_ent->IntGet(ZONE_CLEAR, 0);
			bool do_assign = false;
			if (m_config->RepoLate())
			{ do_assign = (zone_late(flag) == zidx); }
			else
			{ do_assign = (zone_early(flag) == zidx); }

			if (do_assign)
			{
				db_zone->Append(db_ent);
				db_ent->DoAction();
			}
		}

		if (db_zone->Count())
		{ prev_zone = zone; }
	}

	return ret;
}


// ============================================================================
// Create the repo-zone feature
CReturn CReposition::generate_zone( CZone* zone, int zidx)
{
	CReturn ret;

	CDbFeature* db_zone = NULL;
	m_model->EntityCreate( DBFEATURE, (CDbEntity**)&db_zone );

	CString name;
	name.Format( REPO_ZONE_NAME, zidx+1 );
	name = db_zone->NameConvert( name );
	db_zone->SystemName( name );

	// Zone numbers are [1..N] to make VB-side easier.
	db_zone->Owner( NULL );
	db_zone->IntSet( "_zone_num", (zidx+1) );

	// Zone attributes...
	C2dBox* extent = zone->pExtent();
	db_zone->StringSet( STR_TYPE, "_zone" );
	db_zone->DoubleSet( "_zone_left", extent->Xmin() );
	db_zone->DoubleSet( "_zone_top", extent->Ymin() );
	db_zone->DoubleSet( "_zone_right", extent->Xmax() );
	db_zone->DoubleSet( "_zone_bottom", extent->Ymax() );

	// Clamp attributes...
	C2dBoxArray* clamp_array;
	if (zone->Type() == ZONE_PUNCH)
		clamp_array = &m_punch_clamp_array;
	else // ZONE_BURN
		clamp_array = &m_burn_clamp_array;

	db_zone->IntSet( "_clamp_num", clamp_array->Count() );
	for (int idx=0; idx<clamp_array->Count(); idx++)
	{
		C2dBox* clamp = new C2dBox((*(*clamp_array)[idx]));

		double clamp_y = (m_config->YNegative() ? clamp->Ymax() : clamp->Ymin());
		double clamp_x = clamp->Xc();

		int num = idx+1;
		CString varname;
		varname.Format( "_clamp%d_x", num );
		db_zone->DoubleSet(varname, clamp_x + zone->Offset());

		varname.Format( "_clamp%d_y", num );
		db_zone->DoubleSet(varname, clamp_y);
		if (!idx)
		{
			//                                  TR(xmax,ymax)
			//                 ---------------*
			//                 |              |
			//                 |              |
			//                 |              |
			//                 *---------------
			//   LL(xmin,ymin)
			//
			db_zone->DoubleSet( "_clamp_lldx", clamp->Xmin() - clamp_x );
			db_zone->DoubleSet( "_clamp_lldy", clamp->Ymin() - clamp_y );
			db_zone->DoubleSet( "_clamp_trdx", clamp->Xmax() - clamp_x );
			db_zone->DoubleSet( "_clamp_trdy", clamp->Ymax() - clamp_y );
		}
	}

	zone->Feature(db_zone);

	return ret;
}


// ============================================================================
//	Find all entities that do not have a clear_zone and stuff them into the
//	list.  Flattens them as a by-product.
//
CReturn CReposition::manage_orphans(
	CDbEntityList* entity_list,								
	CDbEntityList* orphan_list)
{
	CReturn ret;

	orphan_list->BenignFlush();
	
	int num = entity_list->Count();
	if (!num)
	{ return ret; }

	CDbEntity::NewAction();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*entity_list)[idx];
			
		int flag = db_ent->IntGet(ZONE_CLEAR, 0);
		if ( !flag
			&& !db_ent->DidAction() )
		{ 
			orphan_list->Append(db_ent);
			db_ent->DoAction();
		}
	}

	if (orphan_list->Count())
		flatten_list(orphan_list, true);

	return ret;
}

// ============================================================================
//	Take all collections in the list and flatten them, but retain a parent
//	feature as needed to preserve order for leads.
//
//	 Note that codegen ALSO flattens things... and we probably do it somewhere
//	else, too.  I assume each application has special needs... but we could
//	look into joining them anyway.  A nice CDbEntityIterator might help a LOT/
//
CReturn CReposition::flatten_list( CDbEntityList* src_list, bool mark_down )
{
	CReturn ret;
	CDbEntityList dst_list;

	int idx, num = src_list->Count();
	if (!num)
	{ return ret; }

	for (idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*src_list)[idx];
		ret += flatten_entity(db_ent, &dst_list, mark_down);
	}

	ret += join_leads(&dst_list);

	src_list->BenignFlush();
	num = dst_list.Count();
	for (idx=0; idx<num; idx++)
	{ src_list->Append( dst_list[idx] ); }

	dst_list.BenignFlush();

	return ret;
}

// ----------------------------------------------------------------------------

CReturn CReposition::flatten_entity(
	CDbEntity* db_ent,
	CDbEntityList* dst_list,
	bool mark_down )
{
	CReturn ret;

	if (db_ent->IsDeleted())
		return ret;

	switch (db_ent->Type())
	{
		case DBLINE:
		case DBARC:
		case DBHOLE:
		case DBPROFILE:
			dst_list->Append(db_ent);
			break;

		case DBCOMMAND:
		{
			CDbCommand* db_cmd = (CDbCommand*)db_ent;
			if (db_cmd->IsInstance())
			{
				CDbFeature* boom;
				CModelUtil::PatternExplode( &db_cmd, &boom );
				flatten_entity( boom, dst_list, mark_down );
			}
			else
			{
				dst_list->Append(db_ent);
			}
		}
		break;

		case DBFEATURE:
		{
			CDbFeature* db_feat = (CDbFeature*)db_ent;

			int clear = db_ent->IntGet(ZONE_CLEAR, 0);
			int blocked = db_ent->IntGet(ZONE_BLOCKED, 0);

			// Don't flatten leads; they get processed specially later
			if (db_feat->IsLead())
			{
				dst_list->Append(db_ent);
			}
			else
			{
				int num = db_feat->Count();
				for (int idx=0; idx<num; idx++)
				{
					CDbEntity* sub_ent = (*db_feat)[idx];
					
					if (mark_down)
					{
						sub_ent->IntSet(ZONE_CLEAR, sub_ent->IntGet(ZONE_BLOCKED, 0) | clear);
						sub_ent->IntSet(ZONE_BLOCKED, sub_ent->IntGet(ZONE_BLOCKED, 0) | blocked);
					}

					flatten_entity( sub_ent, dst_list, mark_down );
				}

				db_feat->BenignFlush();
			}
		}
		break;
	}

	return ret;
}


// ============================================================================
//	Scan through the list and, when you find a LeadIn, make a feature and stuff
//	the lead in, profile (or curve), and optional lead out into it.  In the process,
//	compress the list a bit.
//
enum eLeadState
{
	LEADSTATE_NONE,
	LEADSTATE_PIERCE,
	LEADSTATE_IN,
	LEADSTATE_BODY
};

CReturn CReposition::join_leads( CDbEntityList* entity_list)
{
	CReturn ret;

	eLeadState state = LEADSTATE_NONE;
	CDbFeature* lead_feature = NULL;

	int idx = 0;
	while (idx < entity_list->Count())
	{
		CDbEntity* db_ent = (*entity_list)[idx];
		CDbFeature* db_feat = dynamic_cast<CDbFeature*>(db_ent);
		CDbHole* db_hole = dynamic_cast<CDbHole*>(db_ent);

		switch (state)
		{
			case LEADSTATE_NONE:
				if ( db_hole
					&& db_hole->IsPierce())
				{
					m_model->EntityCreate(DBFEATURE, (CDbEntity**)&lead_feature);
					lead_feature->Append(db_hole);
					entity_list->Replace(idx, lead_feature);

					state = LEADSTATE_PIERCE;
				}
				else
				if ( db_feat
					&& db_feat->IsLeadIn() )
				{
					m_model->EntityCreate(DBFEATURE, (CDbEntity**)&lead_feature);
					lead_feature->Append(db_feat);
					entity_list->Replace(idx, lead_feature);

					state = LEADSTATE_IN;
				}
				idx++;
				break;

			case LEADSTATE_PIERCE:
				lead_feature->Append(db_ent);
				entity_list->Remove(idx);

				state = LEADSTATE_IN;
				break;

			case LEADSTATE_IN:
				// TODO:  Allow more than one entity in the lead body...
				lead_feature->Append(db_ent);
				entity_list->Remove(idx);

				state = LEADSTATE_BODY;
				break;

			case LEADSTATE_BODY:
				if ( db_feat
					&& db_feat->IsLeadOut() )
				{
					lead_feature->Append(db_ent);
					entity_list->Remove(idx);
				}
				state = LEADSTATE_NONE;
				lead_feature = NULL;
				break;
		}
	}

	return ret;
}

// ============================================================================

CDbTool* CReposition::tool( CDbEntity* db_ent)
{
	CDbFeature* db_feat = dynamic_cast<CDbFeature*>(db_ent);
	if ( db_feat
		&& db_feat->Count() )
	{ return tool((*db_feat)[0]); }

	return db_ent->Tool();
}


// ============================================================================
// Traverse the list and reduce it to curves and profiles.  Be sure to carry
//	any zone flags from the feature down to the guts.
//
CReturn CReposition::profile_strip( CDbEntityList* entity_list)
{
	CReturn ret;
	CDbEntityList dst_list;

	int idx, num = entity_list->Count();
	if (!num)
	{ return ret; }

	for (idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*entity_list)[idx];
		ret += flatten_entity(db_ent, &dst_list, false);
	}
	//
	//
	entity_list->BenignFlush();

	CLead lead(*m_model);
	double bridge = SMALL;
	if (m_config->LeadSetup() > 0)
		bridge = lead.LeadWidth( m_config->CmdbPath(), m_config->LeadSetup() );

	idx = 0;
	while (idx < dst_list.Count())
	{
		CDbEntity* db_ent = dst_list[idx];
		switch (db_ent->Type())
		{
		case DBFEATURE:
			// Capture lead setup if we don't have it already.
			// Back-door approach.  Not optimal.  Will normally
			// specify lead setup in the m_config->
			if (m_config->LeadSetup() < 0)
			{
				int lead_setup = -1;

				bool islead = ((CDbFeature*)db_ent)->IsLead();
				if (islead)
				{ lead_setup = db_ent->IntGet("_lead_setup", -1); }

				if (lead_setup >= 0)
				{ 
					m_config->LeadSetup(lead_setup);
					bridge = lead.LeadWidth(m_config->CmdbPath(), lead_setup);
				}
			}

			db_ent->Delete();
			dst_list.Remove(idx);
			break;

		case DBHOLE:
			if (((CDbHole*)db_ent)->IsPierce())
			{
				db_ent->Delete();
				dst_list.Remove(idx);
			}
			else
			{
				entity_list->Append( db_ent );
				idx++;
			}
			break;

		case DBPROFILE:
			{
				CDbProfile* db_prof = (CDbProfile*)db_ent;
				// At this point, we are trying to wipe out leads.  Leads,
				// typically, only exist on closed profiles.  If we don't test
				// as closed, we may have gap or overlap...
				// If a reasonable size, adjust it.
				//
				if (!db_prof->IsClosed())
				{ profile_bridge(db_prof, bridge); }
			}
			entity_list->Append( db_ent );
			idx++;
			break;

		default:
			entity_list->Append( db_ent );
			idx++;
			break;
		}
	}

	dst_list.BenignFlush();

	return ret;
}

// ============================================================================

CReturn CReposition::profile_bridge(
	CDbProfile* db_prof,
	double bridge )			///< Maximum bridge distance, otherwise fails
{
	CReturn ret;

	int num = db_prof->Count();
	if (!num)
	{ return ret; }

	CDbCurve* cs = (CDbCurve*)(*db_prof)[ 0 ];
	CDbCurve* ce = (CDbCurve*)(*db_prof)[ num-1 ];

	C3dCoord ps = cs->StartPt( 0 );
	C3dCoord pe = ce->EndPt( 0 );

	double gap = (ps - pe).Length();
	if (gap > bridge)
	{ return ret; }
	//
	//
	if ( (cs->Type() != DBLINE)
		&& (ce->Type() != DBLINE) )
	{ return ret; }

	CDbLine* line;
	if (cs->Type() == DBLINE)
	{ 
		line = (CDbLine*)cs;
		line->Init( line->Tool(), line->Workplane(), ce->EndPt(), line->EndPt() );
	}
	else
	{ 
		line = (CDbLine*)ce;
		line->Init( line->Tool(), line->Workplane(), line->StartPt(), cs->StartPt() );
	}
	
	return ret;
}

// ============================================================================

int CReposition::zone_late(int flag)
{
	if (!flag)
	{ return -1; }

	int zone = 0;
	while (true)
	{
		flag = flag / 2;
		if (!flag)
		{ return zone; }

		zone++;
	}
}

int CReposition::zone_early(int flag)
{
	if (!flag)
	{ return -1; }

	int zone = 0;
	while (true)
	{
		if ( (flag & 0x01)
			|| !flag)
		{ return zone; }

		flag = flag / 2;
		zone++;
	}
}

// ============================================================================
//	Split any curves or profiles in the list against the clamps in it's 
// blocked zone.
//
CReturn CReposition::profile_split( CDbEntityList* entity_list )
{
	CReturn ret;

	int num = entity_list->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*entity_list)[idx];
		int blocked = db_ent->IntGet(ZONE_BLOCKED, 0);

		int zone_idx;
		if (m_config->RepoLate())
			zone_idx = zone_late(blocked);
		else
			zone_idx = zone_early(blocked);

		if (zone_idx < 0)
			continue;
		
		// TODO:  Split out to the end of the loop in a small_split() method.
		CZone* zone = m_zone_list[zone_idx];

		CDbProfile* db_prof = dynamic_cast<CDbProfile*>(db_ent);
		if (db_prof)
		{ 
			ret += split_at_zone(db_prof, zone);
			ret += profile_clamp_rebuild(db_prof, zone);
		}
		else
		{
			CDbCurve* db_curve = dynamic_cast<CDbCurve*>(db_ent);
			if (db_curve)
			{ ret += split_at_zone(db_curve, zone); }
		}
	}

	return ret;
}

// ============================================================================
//	Break large profiles up into small profiles
CReturn CReposition::large()
{
	CReturn ret;

	CDbEntityList burn_orphan_list;
	manage_orphans(&m_burn_list, &burn_orphan_list);

	CDbEntityList punch_orphan_list;
	manage_orphans(&m_punch_list, &punch_orphan_list);

	// 2007.04.19 (PE) -- Not sure if we should use 'true' or 'false' here.
	// Since the code seems to have worked prior to changing EmptyContainers(),
	// we use 'true' because that matches the previous behavior.
	ret += CModelUtil::EmptyContainers( (*m_model), true );

	ret += profile_strip(&burn_orphan_list);
	ret += profile_strip(&punch_orphan_list);

	ret += profile_large(&burn_orphan_list, ZONE_BURN);
	ret += profile_large(&punch_orphan_list, ZONE_PUNCH);

collect();
build();

	return ret;
}

// ============================================================================
//	Split any large parts down into smaller bits.
CReturn CReposition::profile_large( CDbEntityList* entity_list, eZoneType type)
{
	CReturn ret;

	int num = entity_list->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*entity_list)[idx];
		int blocked = db_ent->IntGet(ZONE_BLOCKED, 0);

		int zone_idx;
		if (m_config->RepoLate())
		{ zone_idx = zone_late(blocked); }
		else
		{ zone_idx = zone_early(blocked); }

		if (zone_idx > 0)
		{ continue; }

		ret += large_split(entity_list, db_ent, type);
	}

	return ret;
}

CReturn CReposition::large_split(
	CDbEntityList* entity_list,
	CDbEntity* db_ent,
	eZoneType type)
{
	CReturn ret;
	//
	// Work up the zone split boxes
	//
	C2dBoxArray zone_split;

	C2dBox* prev_box = NULL;
	int znum = m_zone_list.Count();
	for (int zidx=0; zidx<znum; zidx++)
	{
		CZone* zone = m_zone_list[zidx];
		if (zone->Type() != type)
		{ continue; }

		if (!zone->Major())
		{ continue; }

		C2dBox* this_box = new C2dBox(*zone->pExtent());
		zone_split.Append(this_box);

		if (prev_box)
		{
			double left = this_box->Xmin();
			double right = prev_box->Xmax();
			double split = (left + right) * 0.5;

			prev_box->Xmax(split);
			this_box->Xmin(split);
		}
		prev_box = this_box;
	}

	// Ignore large non-profiles.  We shouldn't get any, anyway.
	CDbProfile* db_prof = dynamic_cast<CDbProfile*>(db_ent);
	if (db_prof)
	{ 
		// Like split_at_zone()
		//
		CProfile prof;
		CDbWorkplane* profWork = db_prof->Workplane();
		CConversion::Convert( profWork, db_prof, &prof );
		int winding = SGN(prof.Area());
		prof.DestructiveFlush();

		int num = db_prof->Count();
		for (int idx=0; idx<num; idx++)
		{
			CDbEntity* db_ent = (*db_prof)[idx];
			db_ent->IntSet( "winding", winding );
		}

		int snum = zone_split.Count();
		for (int sidx=0; sidx<snum; sidx++)
		{
			C2dBox* box = zone_split[sidx];
			for (int idx=0; idx<num; idx++)
			{
				CDbCurve* db_curve = (CDbCurve*)(*db_prof)[idx];
				ret += split_at_box(db_curve, box, 0.0);
			}
		}

		ret += profile_zone_rebuild(entity_list, db_prof, type);
	}

	zone_split.DestructiveFlush();

	return ret;
}


// ============================================================================
CReturn CReposition::split_at_zone( CDbProfile* db_prof, CZone* zone )
{
	CReturn ret;

	CProfile prof;
	CDbWorkplane* profWork = db_prof->Workplane();
	CConversion::Convert( profWork, db_prof, &prof );
	int winding = SGN(prof.Area());
	prof.DestructiveFlush();

	int idx, num = db_prof->Count();
	for (idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*db_prof)[idx];
		db_ent->IntSet( "winding", winding );
	}

	for (idx=0; idx<num; idx++)
	{
		CDbCurve* db_curve = (CDbCurve*)(*db_prof)[idx];
		ret += split_at_zone(db_curve, zone);
	}
	return ret;
}


CReturn CReposition::split_at_zone( CDbCurve* db_curve, CZone* zone )
{
	CReturn ret;

	C2dBoxArray* clamp_array = zone->pExclude();

	int num = clamp_array->Count();
	for (int idx=0; idx<num; idx++)
	{
		ret += split_at_box(db_curve, (*clamp_array)[idx], m_config->MachineReal( "Clamp_Buffer", 0.0 ));
	}

	return ret;
}

// ============================================================================

CReturn CReposition::split_at_box(
	CDbCurve* db_curve,
	C2dBox* box,
	double buffer)
{
	CReturn ret;
	
	// Create split lines to whack bad geometry with
	C2dBox split_box( box->Xmin()-buffer,
					  box->Ymin()-buffer,
					  box->Xmax()+buffer,
					  box->Ymax()+buffer);

	CGeoCurveArray	split_array;

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
	//
	//
	CGeoCurve*	geo_curve = db_curve->Curve(0);
	int winding = db_curve->IntGet("winding", 0);
	C3dCoord	intpt[2];

	// Try to split this curve against all of the split lines
	int snum = split_array.Count();
	for (int sidx=0; sidx<snum; sidx++)
	{
		int inum = CSolution::Intersect( *geo_curve, *split_array[sidx], TRUE, intpt );
		if (inum > 0)
		{
			for (int iidx=0; iidx<inum; iidx++)
			{
				// If two splits, will they come out right?  I don't know.
				C3dCoord local_pt;
				db_curve->Workplane()->Inverse().TransformTo(intpt[iidx], &local_pt);
				CDbEntity* db_split = db_curve->Split( local_pt, 0.0 );
				if (db_split)
				{ db_split->IntSet("winding", winding); }
			}
		}
	}
	delete geo_curve;

	return ret;
}

CReturn CReposition::profile_clamp_rebuild( CDbProfile* db_prof, CZone* zone)
{
	CReturn ret;

	// TODO:  Incorporate the clamp buffer into the clamp geometry itself??
	double clamp_buffer = m_config->MachineReal( "Clamp_Buffer", 0.0 );

	C2dBoxArray* clamp_array = zone->pExclude();
	int cnum = clamp_array->Count();

	CDbCurveList master_prof;
	CDbCurveList* clamp_prof;
	clamp_prof = new CDbCurveList[clamp_array->Count()];

	CDbContainer* owner = dynamic_cast<CDbContainer*>(db_prof->Owner());
	int num = db_prof->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*db_prof)[idx];
		C2dBox ent_box = db_ent->Box(0);

		CDbCurveList* dst_prof = &master_prof;
		for (int cidx=0; cidx<cnum; cidx++)
		{
			C2dBox* clamp = (*clamp_array)[cidx];
			
			// TODO:  Expand the box rather than expand the tolerance
			if (clamp->Contains(ent_box, clamp_buffer+SMALL))
			{ dst_prof = &(clamp_prof[cidx]); }
		}

		dst_prof->Append((CDbCurve*)db_ent);
	}

	CDbEntityList* ent_list;
	if (zone->Type() == ZONE_PUNCH)
	{ ent_list = &m_punch_list; }
	else
	{ ent_list = &m_burn_list; }

	int blocked = db_prof->IntGet(ZONE_BLOCKED, 0);
	db_prof->BenignFlush();

	CDbContainer* root;
	while (master_prof.Count())
	{
		CDbProfile* new_prof = profile_create(&master_prof);
		if (new_prof)
		{ 
			root = profile_lead(new_prof);

			root->IntSet(ZONE_CLEAR, blocked);
			root->IntSet(ZONE_BLOCKED, 0);

			ent_list->Append(root);
		}
	}

	for (int cidx=0; cidx<cnum; cidx++)
	{ 
		CDbCurveList* clamp_list = &(clamp_prof[cidx]);
		while (clamp_list->Count())
		{
			CDbProfile* new_prof = profile_create(clamp_list);
			if (new_prof)
			{ 
				ret += mark_entity(new_prof, zone->Type());
				int clear = new_prof->IntGet(ZONE_CLEAR, 0);

				root = profile_lead(new_prof);
				root->IntSet(ZONE_CLEAR, clear);

				ent_list->Append(root);
			}
		}
	}


	return ret;
}

CReturn CReposition::profile_zone_rebuild(
	CDbEntityList* entity_list,
	CDbProfile* db_prof,
	eZoneType type)
{
	CReturn ret;

	CDbCurveList* zone_prof;
	int znum = m_zone_list.Count();
	zone_prof = new CDbCurveList[znum];

	CDbContainer* owner = dynamic_cast<CDbContainer*>(db_prof->Owner());
	int num = db_prof->Count();
	for (int idx=0; idx<num; idx++)
	{
		CDbEntity* db_ent = (*db_prof)[idx];

		CDbCurveList* dst_prof = NULL;
		for (int zidx=0; zidx<znum; zidx++)
		{
			CZone* zone = m_zone_list[zidx];
			if (zone->Type() != type)
			{ continue; }

			if (!zone->Major())
			{ continue; }

			C2dBox* box = zone->pExtent();
			if (box->Contains(db_ent->Box(0), 2*SMALL))
			{ dst_prof = &(zone_prof[zidx]); }
		}
		if (dst_prof)
		{ dst_prof->Append((CDbCurve*)db_ent); }
	}

	db_prof->BenignFlush();
	db_prof->Delete();

	for (int zidx=0; zidx<znum; zidx++)
	{ 
		CZone* zone = m_zone_list[zidx];
		if (zone->Type() != type)
		{ continue; }

		CDbCurveList* zone_list = &(zone_prof[zidx]);
		while (zone_list->Count())
		{
			CDbProfile* new_prof = profile_create(zone_list);
			if (new_prof)
			{ 
				ret += mark_entity(new_prof, type);
				int clear = new_prof->IntGet(ZONE_CLEAR, 0);
				int blocked = new_prof->IntGet(ZONE_BLOCKED, 0);

				CDbContainer* root = profile_lead(new_prof);
				root->IntSet(ZONE_CLEAR, clear);
				root->IntSet(ZONE_BLOCKED, blocked);

				entity_list->Append(root);
			}
		}
	}

	return ret;
}

CDbProfile* CReposition::profile_create( CDbCurveList* db_curve_list)
{
	CReturn ret;

	int num = db_curve_list->Count();
	if (!num)
	{ return NULL; }

	CDbCurve* db_seed = (*db_curve_list)[0];

	CDbCurveList dst_curves;
	ret += CProfileBuilder::ProfileGrow( db_seed, SMALL, db_curve_list, &dst_curves );
	if ( !ret.IsOk()
		|| (dst_curves.Count() < 1) )
	{ return NULL; };

	CDbProfile* db_prof = NULL;
	m_model->EntityCreate(DBPROFILE, (CDbEntity**)&db_prof);
	db_prof->Append(dst_curves);

	int winding = db_seed->IntGet("winding", 0);
	db_prof->IntSet("winding", winding);

	return db_prof;
}

CDbContainer* CReposition::profile_lead( CDbProfile* db_prof)
{
	CSelector selector(*m_model);
	selector.Add(db_prof, false);

	CLead lead(*m_model);
	lead.AutoOpen(m_config->CmdbPath(), m_config->LeadSetup(), true, &selector);

	CDbContainer* root = lead.Root();
	if (!root)
		root = db_prof;

	return root;
}

