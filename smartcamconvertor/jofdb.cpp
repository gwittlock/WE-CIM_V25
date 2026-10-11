
#include "stdafx.h"
#include "cmn_resource.h"
#include "ToolConst.h"
#include "JofDB.h"

int DEFAULT_COLOR = RGB(255,255,255);

const CString STR_TOOL_CRIB( "Tool Crib" );
const CString STR_STATION_CONFIGURATION( "Station Configuration" );
const CString STR_TOOL_SETUP_MEMBERS( "Tool Setup Members" );

const double DEF_REAL = 0.;
// const double DEF_REAL = UNDEFINED;  // or if you want errors to 'stand out' ...


////////////////////////////////////////////////////////////////////////

CJofDB::CJofDB()
{
	IndicesInvalidate();
}

CJofDB::~CJofDB()
{
}

// If return status is okay, db will be left in "open" state.
CReturn
CJofDB::Create( const CString& filepath )
{
	CReturn status;
	CDaoDatabase db;

	try
	{
		db.Create( filepath );
		db.Close();
	}
	catch( CDaoException* e )
	{
		int max = e->GetErrorCount();
		for (int idx=0; idx<max; idx++)
		{
			e->GetErrorInfo( idx );
			status.Internal( IDS_INTERNAL_ERROR, e->m_pErrorInfo->m_strDescription );
		}

		e->Delete();
	}

	if ( status.IsOk() )
		status = TablesCreate( filepath );

	if ( status.IsOk() )
		status = Open( filepath );

	return status;
}

CReturn
CJofDB::Open( const CString& filepath )
{
	CReturn status;

	if ( !m_db.IsOpen() )
	{
		m_tblToolCrib	= m_db.addTable( STR_TOOL_CRIB );

		status = m_db.Open( filepath );

		if ( !status.IsOk() )
			IndicesInvalidate();
	}

	return status;
}

void
CJofDB::Close()
{
	if ( m_db.IsOpen() )
	{
		m_db.Close();

		IndicesInvalidate();
	}
}

CReturn
CJofDB::Update( const CVarList& attribs )
{
	CReturn status;
	int toolID;

	if ( m_db.IsOpen() )
	{
		toolID = ToolCribAdd( attribs );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CJofDB::Update() -- database not open" );
	}

	return status;
}

CReturn
CJofDB::TablesCreate( const CString& filepath )
{
	CReturn status;

	status = m_db.Open( filepath );

	if ( status.IsOk() )
		status = ToolCribCreate();

	m_db.Close();

	return status;
}

CReturn
CJofDB::ToolCribCreate()
{
	CReturn status;

	m_tblToolCrib = m_db.createTable( STR_TOOL_CRIB );

	status += m_db.newInt(		m_tblToolCrib, "ID", TRUE );
	status += m_db.newString(	m_tblToolCrib, "Description", 50 );
	status += m_db.newInt(		m_tblToolCrib, "Type ID", FALSE );
	status += m_db.newInt(		m_tblToolCrib, "Color", FALSE );
	status += m_db.newInt(		m_tblToolCrib, "Loff", FALSE );
	status += m_db.newInt(		m_tblToolCrib, "Doff", FALSE );
	status += m_db.newDouble(	m_tblToolCrib, "Length" );
	status += m_db.newDouble(	m_tblToolCrib, "Width" );
	status += m_db.newDouble(	m_tblToolCrib, "Diameter" );
	status += m_db.newDouble(	m_tblToolCrib, "Kerf" );
	status += m_db.newDouble(	m_tblToolCrib, "Corner Radius" );
	status += m_db.newDouble(	m_tblToolCrib, "Angle" );
	status += m_db.newDouble(	m_tblToolCrib, "Pitch" );
	status += m_db.newDouble(	m_tblToolCrib, "Radius" );
	status += m_db.newDouble(	m_tblToolCrib, "Web" );
	status += m_db.newDouble(	m_tblToolCrib, "Major Diameter" );
	status += m_db.newDouble(	m_tblToolCrib, "Minor Diameter" );
	status += m_db.newDouble(	m_tblToolCrib, "Shear" );
	status += m_db.newDouble(	m_tblToolCrib, "Cost" );
	status += m_db.newDouble(	m_tblToolCrib, "Life" );
	status += m_db.newString(	m_tblToolCrib, "Filename", 255 );
	status += m_db.newDouble(	m_tblToolCrib, "Reqd Station Size" );
	status += m_db.newInt(		m_tblToolCrib, "Reqd Auto Index", FALSE );
	status += m_db.newInt(		m_tblToolCrib, "NC Code Number", FALSE );
	status += m_db.newInt(		m_tblToolCrib, "Fixed Station", FALSE);
	status += m_db.newDouble(	m_tblToolCrib, "Index Angle" );
	status += m_db.newInt(		m_tblToolCrib, "Auto Index", FALSE );

	status += m_db.setIndex(	m_tblToolCrib, "ID", TRUE ); // set as primary field

	status += m_db.commitTable(	m_tblToolCrib );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: There is a painful dichotomy in attribute naming conventions.
// Whereas the application uses underscores as word-separators, the
// mdb uses spaces.  Therefore, all attributes must use the underscore
// convention but we must use the space convention when creating an mdb.
//
int
CJofDB::ToolCribAdd( const CVarList& attribs )
{
	CReturn status;
	int toolID = 0;

	CString descrip = attribs.getString( "Description", "" );  // good for debugging
	int sncs = attribs.getInt( "section", 0 );

	m_db.createRecord( m_tblToolCrib );

	// status += m_db.setInt(		m_tblToolCrib, "ID", TRUE );	// because auto-field

	status += m_db.setString(	m_tblToolCrib, "Description",		descrip );
	status += m_db.setInt(		m_tblToolCrib, "Type ID",			attribs.getInt( "Type_ID", -1 ) );
	status += m_db.setInt(		m_tblToolCrib, "Color",				attribs.getInt( "Color", -1 ) );
	status += m_db.setInt(		m_tblToolCrib, "Loff",				attribs.getInt( "Loff", -1 ) );
	status += m_db.setInt(		m_tblToolCrib, "Doff",				attribs.getInt( "Doff", -1 ) );
	status += m_db.setDouble(	m_tblToolCrib, "Length",			attribs.getReal( "Length", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Width",				attribs.getReal( "Width", DEF_REAL )  );
	status += m_db.setDouble(	m_tblToolCrib, "Diameter",			attribs.getReal( "Diameter", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Kerf",				attribs.getReal( "Kerf", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Corner Radius",		attribs.getReal( "Corner_Radius", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Angle",				attribs.getReal( "Index_Angle", 0. ) );
	status += m_db.setDouble(	m_tblToolCrib, "Pitch",				attribs.getReal( "Pitch", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Radius",			attribs.getReal( "Radius", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Web",				attribs.getReal( "Radius", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Major Diameter",	attribs.getReal( "Major_Diameter", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Minor Diameter",	attribs.getReal( "Minor_Diameter", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Shear",				attribs.getReal( "Shear", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Cost",				attribs.getReal( "Cost", DEF_REAL ) );
	status += m_db.setDouble(	m_tblToolCrib, "Life",				attribs.getReal( "Life", DEF_REAL ) );
	status += m_db.setString(	m_tblToolCrib, "Filename",			attribs.getString( "Filename", "-" ) );
	status += m_db.setDouble(	m_tblToolCrib, "Reqd Station Size", attribs.getReal( "Reqd_Station_Size", DEF_REAL ) );
	status += m_db.setInt(		m_tblToolCrib, "Reqd Auto Index",	attribs.getInt( "Reqd_Auto Index", 0 ) );
	status += m_db.setInt(		m_tblToolCrib, "NC Code Number",	attribs.getInt( "NC_Code_Number", -1 ) );
	status += m_db.setInt(		m_tblToolCrib, "Fixed Station",		attribs.getInt( "Fixed_Station", -1 ) );
	status += m_db.setDouble(	m_tblToolCrib, "Index Angle",		attribs.getReal( "Index_Angle", UNDEFINED ) );
	status += m_db.setInt(		m_tblToolCrib, "Auto Index",		attribs.getInt( "Auto_Index", 0 ) );

	status += m_db.commitRecord( m_tblToolCrib );

	if ( status.IsOk() )
	{
		// NOTE: ~!@#$%^&* Microsoft -- have to set current record to get ID.
		status += m_db.moveFirst( m_tblToolCrib );
		status += m_db.moveLast( m_tblToolCrib );
		toolID = m_db.getInt( m_tblToolCrib, "ID" );
	}

	return toolID;
}

void
CJofDB::IndicesInvalidate()
{
	m_tblToolCrib = -1;
}


