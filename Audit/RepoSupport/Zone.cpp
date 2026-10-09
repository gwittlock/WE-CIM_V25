// ==================================================================
//		Reposition Zone
//
// ==================================================================

#include "zone.h"


CZone::CZone()
{
	m_major = false;
	m_feature = NULL;
}

CZone::~CZone()
{
	m_exclude_array.DestructiveFlush();
}

void
CZone::Dump()
{
	CString note;
	CReturn ret;

	note.Format("%s %s Zone %f, %f to %f, %f", 
				(m_major?"Major":"minor"),
				(m_type==ZONE_PUNCH?"Punch":"Burn"),
				m_extent.Xmin(), m_extent.Ymin(),
				m_extent.Xmax(), m_extent.Ymax() );
	ret.Diagnostic(note);

	note.Format("   Offset %f, repo %f", m_offset, m_repo);
	ret.Diagnostic(note);

	int num = m_exclude_array.Count();
	note.Format("   %d Clamps", num );
	ret.Diagnostic(note);

	for (int idx=0; idx<num; idx++)
	{
		C2dBox* clamp = m_exclude_array[idx];
		note.Format("      %d: %f, %f to %f, %f",
				idx, 
				clamp->Xmin(), clamp->Ymin(),
				clamp->Xmax(), clamp->Ymax() );
		ret.Diagnostic(note);
	}
}
