#if !defined(_REPOSITION_H)
#define _REPOSITION_H

// ==================================================================
//		Reposition
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <stdafx.h>

#include "Common.h"
#include "Model.h"
#include "NestConfig.h"
#include "DbCurveList.h"
#include "Zone.h"

// ==================================================================

#define ZONE_CLEAR "_zone_clear"
#define ZONE_BLOCKED "_zone_blocked"

// ==================================================================


class dllExport CReposition
{
public:
	CReposition( CModel* model, CNestConfig* config);
	virtual ~CReposition();

	CReturn Create();

protected:
	CReturn collect();
	CReturn build();
	CReturn build_progressive();
	CReturn large();
	CReturn split();
	CReturn assign();

	CDbEntity* parent(CDbEntity* db_ent);
	CDbPattern* get_pattern(CDbCommand* db_cmd);

	void build_clamps(eZoneType type);
	CZone* build_zone(double offset, eZoneType type);
	bool mark_zone(CZone* zone, int zone_num);
	CReturn mark_entity(CDbEntity* db_ent, eZoneType type);
	CReturn generate_zone(CZone* zone, int zidx);
	void content_do_action(CDbContainer* contain);

	CDbTool* tool(CDbEntity* db_ent);
	int zone_late(int flag);
	int zone_early(int flag);

	CReturn manage_orphans(CDbEntityList* entity_list, CDbEntityList* orphan_list);
	CReturn flatten_list(CDbEntityList* src_list, bool mark_down);
	CReturn flatten_entity(CDbEntity* db_ent, CDbEntityList* dst_list, bool mark_down );
	CReturn join_leads(CDbEntityList* entity_list);

	CReturn profile_strip(CDbEntityList* src_list);
	CReturn profile_split(CDbEntityList* src_list);
	CReturn profile_large(CDbEntityList* src_list, eZoneType type);
	CReturn split_at_zone(CDbProfile* db_prof, CZone* zone);
	CReturn split_at_zone(CDbCurve* db_curve, CZone* zone);
	CReturn split_at_box(CDbCurve* db_curve, C2dBox* clamp, double buffer);
	CReturn large_split(CDbEntityList* entity_list, CDbEntity* db_ent, eZoneType type);

	CReturn profile_clamp_rebuild(CDbProfile* db_prof, CZone* zone);
	CReturn profile_zone_rebuild(CDbEntityList* entity_list, CDbProfile* db_prof, eZoneType type);

	CDbProfile* profile_create(CDbCurveList* db_curve_list);
	CDbContainer* profile_lead(CDbProfile* db_prof);
	CReturn profile_bridge(CDbProfile* db_prof, double bridge );

private:
	CModel*			m_model;
	CNestConfig*	m_config;

	CDbEntityList	m_punch_list;		// All punch entities, easy reference
	CDbEntityList	m_burn_list;		// All burn entities, easy reference
	CZoneList		m_zone_list;

	C2dBoxArray		m_punch_clamp_array;	// For handy reference
	C2dBoxArray		m_burn_clamp_array;		// For handy reference
	double			m_clamp_right;		// Right-most edge of the right-most clamp
};

#endif

