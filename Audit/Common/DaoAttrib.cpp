// ==================================================================
//		DaoAttrib
// ==================================================================
//
//	For easy access to tables that use the attribute format --
//	extract a value from the CValue field given the CVarName.
//
//	This is a convenience class tying together DaoDb and DaoQuery
//
// ==================================================================

#include "stdafx.h"

#include "DaoAttrib.h"

CString g_buffer;


// ==================================================================

CDaoAttrib::CDaoAttrib( CDaoDB* database, const CString& table )
{
	m_db = database;
	m_table = table;
}

CDaoAttrib::~CDaoAttrib()
{
}

// ==================================================================

int	
CDaoAttrib::Int( const CString& name, int def )
{
	CString question;
	question.Format( "SELECT CValue FROM %s WHERE (CVarName=\"%s\")", m_table, name );

	CDaoQuery query;
	query.Init( m_db->Database(), question );

	int val = ((query.RecordCount() > 0) ? query.IntGet("CValue") : def);

	if (CReturn::Debug() >= 1)
	{
		g_buffer.Format( "%s = %d", name, val );
		EWMDao( (LPCSTR) g_buffer );
	}

	return val;
}

double 
CDaoAttrib::Double( const CString& name, double def )
{
	CString question;
	question.Format( "SELECT CValue FROM %s WHERE (CVarName=\"%s\")", m_table, name );

	CDaoQuery query;
	query.Init( m_db->Database(), question );

	double val = ((query.RecordCount() > 0) ? query.DoubleGet("CValue") : def);

	if (CReturn::Debug() >= 1)
	{
		g_buffer.Format( "%s = %g", name, val );
		EWMDao( (LPCSTR) g_buffer );
	}

	return val;
}

CString 
CDaoAttrib::String( const CString& name, const CString& def )
{
	CString question;
	question.Format( "SELECT CValue FROM %s WHERE (CVarName=\"%s\")", m_table, name );

	CDaoQuery query;
	query.Init( m_db->Database(), question );

	CString val = ((query.RecordCount() > 0) ? query.StringGet("CValue") : def);

	if (CReturn::Debug() >= 1)
	{
		g_buffer.Format( "%s = %s", name, val );
		EWMDao( (LPCSTR) g_buffer );
	}

	return val;
}

// ==================================================================

int	
CDaoAttrib::ciInt( const CString& name, int def )
{
	CDaoQuery query;
	CString	question;
	int		val;

	question.Format( "SELECT Value FROM %s WHERE (InternalName=\"%s\")", m_table, name );

	query.Init( m_db->Database(), question );

	val = ((query.RecordCount() > 0) ? query.IntGet("Value") : def);

	if (CReturn::Debug() >= 1)
	{
		CString note;
		CReturn ret;

		note.Format( "%s = %d", name, val );
		ret.Diagnostic( note );
	}

	return val;
}

double 
CDaoAttrib::ciDouble( const CString& name, double def )
{
	CDaoQuery query;
	CString	question;
	double	val;

	question.Format( "SELECT Value FROM %s WHERE (InternalName=\"%s\")", m_table, name );

	query.Init( m_db->Database(), question );

	val = ((query.RecordCount() > 0) ? query.DoubleGet("Value") : def);

	if (CReturn::Debug() >= 1)
	{
		CString note;
		CReturn ret;

		note.Format( "%s = %g", name, val );
		ret.Diagnostic( note );
	}

	return val;
}

CString 
CDaoAttrib::ciString( const CString& name, const CString& def )
{
	CDaoQuery query;
	CString question;
	CString val;

	question.Format( "SELECT Value FROM %s WHERE (InternalName=\"%s\")", m_table, name );

	query.Init( m_db->Database(), question );

	val = ((query.RecordCount() > 0) ? query.StringGet("Value") : def);

	if (CReturn::Debug() >= 1)
	{
		CString note;
		CReturn ret;

		note.Format( "%s = %s", name, val );
		ret.Diagnostic( note );
	}

	return val;
}

// ==================================================================

int	
CDaoAttrib::ciInt( const CString& name, const CString& idName, int id, int def )
{
	CDaoQuery query;
	CString question;
	int		val;

	question.Format("SELECT Value FROM %s WHERE ((InternalName=\"%s\") AND (%s=%d))", m_table, name, idName, id);

	query.Init( m_db->Database(), question );

	val = ((query.RecordCount() > 0) ? query.IntGet("Value") : def);

	if (CReturn::Debug() >= 1)
	{
		CString note;
		CReturn ret;

		note.Format( "%s = %d", name, val );
		ret.Diagnostic( note );
	}

	return val;
}

double 
CDaoAttrib::ciDouble( const CString& name, const CString& idName, int id, double def )
{
	CDaoQuery query;
	CString question;
	double	val;

	question.Format("SELECT Value FROM %s WHERE ((InternalName=\"%s\") AND (%s=%d))", m_table, name, idName, id);

	query.Init( m_db->Database(), question );

	val = ((query.RecordCount() > 0) ? query.DoubleGet("Value") : def);

	if (CReturn::Debug() >= 1)
	{
		CString note;
		CReturn ret;

		note.Format( "%s = %g", name, val );
		ret.Diagnostic( note );
	}

	return val;
}

CString 
CDaoAttrib::ciString( const CString& name, const CString& idName, int id, const CString& def )
{
	CDaoQuery query;
	CString question;
	CString val;

	question.Format("SELECT Value FROM %s WHERE ((InternalName=\"%s\") AND (%s=%d))", m_table, name, idName, id);

	query.Init( m_db->Database(), question );

	val = ((query.RecordCount() > 0) ? query.StringGet("Value") : def);

	if (CReturn::Debug() >= 1)
	{
		CString note;
		CReturn ret;

		note.Format( "%s = %s", name, val );
		ret.Diagnostic( note );
	}

	return val;
}

