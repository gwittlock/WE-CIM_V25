// ==================================================================
//		LeadData
//
// ==================================================================

#include "stdafx.h"
#include "LeadData.h"
#include "MathConst.h"

// ==================================================================

CLeadData::CLeadData()
{
	m_context = LEAD_CONTEXT_INVALID;

	// Defaults turn of leads...
	m_type = LEAD_NONE;
	m_junction = LEAD_EXACT;
	m_clock = -1;  // 0..11
	m_usplit = 0.;
	m_locate = 1;

	// .. and other stuff, just to be complete.
	m_length = 0.0;
	m_angle = 0.0;
	m_radius = 0.0;
	m_included = 0.0;
	m_distance = 0.0;
	m_tabthick = 0.0;
	m_tolerance = 0.30;

	m_usepierce = PIERCE_NONE;
	m_pierceoffset = 0.0;
	m_piercediam = 0.0;
	m_piercetype = TTYPE_NONE;
	m_pierceid = -1;

	m_minoverlap = 0.0;
}

CLeadData::~CLeadData()
{
}

CLeadData::CLeadData( const CLeadData& rhs )
{
	(*this) = rhs;
}

const CLeadData& CLeadData::operator = ( const CLeadData& rhs )
{
	m_name = rhs.m_name;
	m_context = rhs.m_context;

	m_type = rhs.m_type;
	m_junction = rhs.m_junction;

	m_clock = rhs.m_clock;
	m_usplit = rhs.m_usplit;

	m_locate = rhs.m_locate;

	m_length = rhs.m_length;
	m_angle = rhs.m_angle;
	m_radius = rhs.m_radius;
	m_included = rhs.m_included;
	m_distance = rhs.m_distance;
	m_tabthick = rhs.m_tabthick;
	m_tolerance = rhs.m_tolerance;

	m_usepierce = rhs.m_usepierce;
	m_pierceoffset = rhs.m_pierceoffset;
	m_piercediam = rhs.m_piercediam;
	m_piercetype = rhs.m_piercetype;
	m_pierceid = rhs.m_pierceid;

	m_usetilt = rhs.m_usetilt;

	m_minoverlap = rhs.m_minoverlap;

	return (*this);
}


// ==================================================================
//	Load one lead-data parameter table
CReturn CLeadData::Load( const CString& configdb, int index )
{
	CDaoDB db;

	int table = db.addTable( "Lead Parameter" );

	CReturn status = db.Open( configdb );

	if (status.isOkay())
		status += Load( db, table, index );

	db.Close();

	return status;
}

CReturn CLeadData::Load( CDaoDB& db, int table, int index )
{
	CReturn	status;

	if (db.findRecord( table, "ID", index ) < 0)
	{
		status.Internal( IDS_DB_RECORD, "CMDB", "Lead Parameter", index );
		return status;
	}

	m_name = db.getString( table, "Description" );

	m_context = (eLeadContext) index;

	m_type		= (eLeadType)db.getInt( table, "Type_ID" );
	m_junction	= (eLeadJunction)db.getInt( table, "Junction" );

	int location = db.getInt( table, "Location" );
	m_clock = (location + 1) % 12;

	m_locate	= location + 1;

	m_length	= db.getDouble( table, "Length" );
	m_angle		= db.getDouble( table, "Angle" );
	m_radius	= db.getDouble( table, "Radius" );
	m_included	= db.getDouble( table, "Included_Angle" );
	m_distance	= db.getDouble( table, "Distance" );
	m_tabthick	= db.getDouble( table, "Tab_Thickness" );

	// Ugh, 'split' equates back to CLeadData::m_tolerance which, in turn,
	// equates back to the CfgMgr's Lead Parameters / Split Location.
	int ival = db.getInt( table, "Tolerance" );
	m_usplit = SplitValue( ival );

	m_tolerance = 1.0 / ival;
	if (m_tolerance > 0.5)  // ie. No split.
		m_tolerance = 0.5;

	m_usepierce		= (ePierceType)db.getInt( table, "Use_Pierce" );
	m_pierceoffset	= db.getDouble( table, "Pierce_Offset" );
	m_piercediam	= db.getDouble( table, "Pierce_Diameter" );
	m_piercetype	= (eToolType) db.getInt( table, "Pierce_Type_ID" );

	m_usetilt		= db.getInt( table, "Use_Tilt" ) != 0;

	return status;
}




// ============================================================================
//	Return the larger of the lead radius, length, and/or gap/overlap distance.
//
//	Useful as a rule-of-thumb value to determine the scale of the leads
//
double CLeadData::LeadWidth() const
{
	double val = max(m_length, m_radius);
	val = max(val, m_distance);
	val = max(val, m_piercediam);

	return val;
}

double CLeadData::SplitValue( int cmdb_value ) const
{
	switch (cmdb_value)
	{
	case 1:  return 0.;
	case 3:  return 0.3;
	case 5:  return 0.2;
	default: return 0.5;  // always split at midpt
	}
}