#if !defined(_ZONE_H)
#define _ZONE_H

// ==================================================================
//		Reposition Zone
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <stdafx.h>

#include "Common.h"
#include "2dBox.h"
#include "IndxList.h"
#include "DbFeature.h"

// ==================================================================

enum eZoneType
{
	ZONE_PUNCH,
	ZONE_BURN
};

class dllExport CZone
{
public:
	CZone();
	~CZone();

	eZoneType Type() const		{ return m_type; }
	void Type(eZoneType type)	{ m_type = type; }

	double Offset() const		{ return m_offset; }
	void Offset(double offset)	{ m_offset = offset; }

	double Repo() const			{ return m_repo; }
	void Repo(double repo)		{ m_repo = repo; }

	bool Major() const			{ return m_major; }
	void Major(bool set)		{ m_major = set; }

	C2dCoord* Hold()				{ return &m_hold; }
	void Hold(C2dCoord& pt)			{ m_hold = pt; }
	void Hold(double x, double y)	{ m_hold.XY(x, y); }

	CDbFeature* Feature() const		{ return m_feature; }
	void Feature( CDbFeature* feat) { m_feature = feat; }

	C2dBox* pExtent()			{ return &m_extent; }
	C2dBoxArray* pExclude()		{ return &m_exclude_array; }

	void Dump();

private:
	eZoneType	m_type;
	double		m_repo;
	double		m_offset;
	bool		m_major;
	C2dBox		m_extent;
	C2dBoxArray	m_exclude_array;	// For handy reference

	CDbFeature*	m_feature;
	C2dCoord	m_hold;
};

// ==================================================================

typedef CIndxList<CZone*> CZoneList;

#endif

