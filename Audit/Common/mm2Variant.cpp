// ==================================================================
//		Variant
//
//		Microsoft variant wrapper
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"
#include "MathConst.h"
#include "mm2Variant.h"

// =======================================================================

CString
CVariant::getFieldName( CDaoRecordset* in_record, int fieldIndex )
{
	CReturn	ret;
	CString name;

	try
	{
		CDaoFieldInfo daoFieldInfo;
		in_record->GetFieldInfo( fieldIndex, daoFieldInfo, AFX_DAO_PRIMARY_INFO );
		name = daoFieldInfo.m_strName;
	}
	catch( CDaoException* e )
	{
		ret = issueError( "CVariant::getFieldName()", e );
	}

	return name;
}

CReturn
CVariant::getField( CDaoRecordset* in_record, int fieldIndex )
{
	CReturn	ret;

	try
	{
		m_var = in_record->GetFieldValue( fieldIndex );
	}
	catch( CDaoException* e )
	{
		ret = issueError( fieldIndex, e );
	}
	return ret;
}

CReturn
CVariant::getField( 
	CDaoRecordset*	in_record,
	const CString&	in_name )
{
	CReturn	ret;

	try
	{
		m_var = in_record->GetFieldValue( in_name );
	}
	catch( CDaoException* e )
	{
		ret = issueError( in_name, e );
	}
	return ret;
}

CReturn
CVariant::setField( 
	CDaoRecordset*	in_record,
	const CString&	in_name )
{
	CReturn	ret;

	try
	{
		in_record->SetFieldValue( in_name, m_var );
	}
	catch( CDaoException* e )
	{
		ret = issueError( in_name, e );
	}
	return ret;
}

// ==================================================================

CString
CVariant::getString( void )
{
	CString	str;
	double	d_val;

	str.Empty();

	switch(m_var.vt)
	{
		case VT_I2:
			str.Format( "%d", m_var.iVal );
			break;
		case VT_I4:
			str.Format( "%ld", m_var.lVal );
			break;

		case VT_R4:
			str.Format( "%f", m_var.fltVal );
			break;
		case VT_R8:
			str.Format( "%f", m_var.dblVal );
			break;

		case VT_CY:
			d_val = (m_var.cyVal.Hi*65536.0 + m_var.cyVal.Lo)/10000.0;
			str.Format( "%15.2f", d_val );
			break;

		case VT_BSTR:
			str = (char*)m_var.bstrVal;
//			strcpy( global_var_string, (char*)m_var.bstrVal );
			break;

		case VT_BOOL:
			str = ((m_var.boolVal) ? "1" : "0");
			break;
	}

	return str;
}


CString
CVariant::getString( CDaoRecordset* in_record, int fieldIndex )
{
	if (getField( in_record, fieldIndex ).isOkay())
		return getString();

	return CString("");
}

CString
CVariant::getString( 
	CDaoRecordset*	in_record, 
	const CString&	in_name )
{
	if (getField( in_record, in_name ).isOkay())
		return getString();

	return CString("");
}

// ==================================================================

void
CVariant::setString( 
	const CString&	in_str,
	bool			in_other )	// DON'T *EVEN* get me started...
{
	if (in_other)
	{
		m_var.Clear();
//		m_var.bstrVal = SysAllocString( (WORD*)(LPCSTR)in_str );
//		m_var.vt = VT_BSTR;
		m_var = COleVariant( (LPCSTR)in_str, VT_BSTRT );
	}
	else
		m_var = (LPCSTR)in_str;
}

void
CVariant::setString(
	CDaoRecordset*	in_record,
	const CString&	in_name,
	const CString&	in_str,
	bool			in_other )
{
	setString( in_str, in_other );
	setField( in_record, in_name );
}

// ==================================================================

void
CVariant::setInt(
	int	in_int )
{
	m_var = (long)in_int;
//	m_var.Clear();
//	m_var.iVal = in_int;
//	m_var.vt = VT_I2;
}

void
CVariant::setInt(
	CDaoRecordset*	in_record,
	const CString&	in_name,
	int				in_int )
{
	setInt( in_int );
	setField( in_record, in_name );
}


// ==================================================================

void
CVariant::setDouble(
	double	in_double )
{
	m_var = in_double;
//	m_var.Clear();
//	m_var.dblVal = in_double;
//	m_var.vt = VT_R8;
}

void
CVariant::setDouble(
	CDaoRecordset*	in_record,
	const CString&	in_name,
	double			in_double )
{
	setDouble( in_double );
	setField( in_record, in_name );
}

// ==================================================================

void
CVariant::setMoney(
	double	in_money )
{
	m_var.Clear();

	m_var.cyVal.Hi = 0;
	m_var.cyVal.Lo = (long)(in_money * 10000);
	m_var.vt = VT_CY;
}

void
CVariant::setMoney(
	CDaoRecordset*	in_record,
	const CString&	in_name,
	double			in_money )
{
	setMoney( in_money );
	setField( in_record, in_name );
}

// ==================================================================
void			
CVariant::setTime( void )
{
	setTime( CTime::GetCurrentTime() );
}

void			
CVariant::setTime( 
	const CTime& time )
{
	setString( time.Format( "%m/%d/%y %I:%M:%S %p" ), TRUE );
}

void
CVariant::setTime( 
	CDaoRecordset*	in_record, 
	const CString&	in_name, 
	const CTime&	time )
{
	setTime( time );
	setField( in_record, in_name );
}

void
CVariant::setTime( 
	CDaoRecordset*	in_record, 
	const CString&	in_name )
{
	setTime();
	setField( in_record, in_name );
}


// ==================================================================

int
CVariant::getInt( void )
{
	switch(m_var.vt)
	{
		case VT_EMPTY:
		case VT_NULL:
			return 0;

		case VT_I2:
			return m_var.iVal;
		case VT_I4:
			return m_var.lVal;

		case VT_R4:
			return (int)m_var.fltVal;
		case VT_R8:
			return (int)m_var.dblVal;

		case VT_BSTR:
			return atoi( (char*)m_var.bstrVal );

		case VT_BOOL:
			return m_var.boolVal;
	}

	return 0;
}

int
CVariant::getInt( CDaoRecordset* in_record, int fieldIndex )
{
	if (getField( in_record, fieldIndex ).isOkay())
		return getInt();

	return IUNDEFINED;
}

int
CVariant::getInt( 
	CDaoRecordset*	in_record, 
	const CString&	in_name )
{
	if (getField( in_record, in_name ).isOkay())
		return getInt();

	return 0;
}

// ==================================================================

double
CVariant::getDouble( void )
{
	switch(m_var.vt)
	{
		case VT_EMPTY:
		case VT_NULL:
			return 0.0;

		case VT_I2:
			return (double)m_var.iVal;
		case VT_I4:
			return (double)m_var.lVal;

		case VT_R4:
			return m_var.fltVal;
		case VT_R8:
			return m_var.dblVal;

		case VT_CY:
			return (m_var.cyVal.Hi*65536.0 + m_var.cyVal.Lo)/10000.0;

		case VT_BSTR:
			return atof( (char*)m_var.bstrVal );

		case VT_BOOL:
			return ((m_var.boolVal) ? 1.0 : 0.0);
	}

	return 0.0;
}

double
CVariant::getDouble( CDaoRecordset* in_record, int fieldIndex )
{
	if (getField( in_record, fieldIndex ).isOkay())
		return getDouble();

	return UNDEFINED;
}

double
CVariant::getDouble( 
	CDaoRecordset*	in_record, 
	const CString&	in_name )
{
	if (getField( in_record, in_name ).isOkay())
		return getDouble();

	return 0.0;
}

// ==================================================================
CTime
CVariant::getTime( void )
{
	switch(m_var.vt)
	{
		case VT_DATE:
			COleDateTime date = m_var.date;
			SYSTEMTIME	systime;
			if (date.GetAsSystemTime( systime ))
			{
				return CTime(systime);
			}
			break;
	}

	CTime bogus;
	return bogus;
}

CTime
CVariant::getTime( 
	CDaoRecordset*	in_record, 
	int				fieldIndex )
{
	if (getField( in_record, fieldIndex ).isOkay())
		return getTime();

	CTime bogus;
	return bogus;
}

CTime
CVariant::getTime( 
	CDaoRecordset*	in_record, 
	const CString&	in_name )
{
	if (getField( in_record, in_name ).isOkay())
		return getTime();

	CTime bogus;
	return bogus;
}


// ==================================================================

CReturn
CVariant::issueError( int fieldIndex, CDaoException* e )
{
	CReturn status;
	CString	error;

	int max = e->GetErrorCount();
	for (int idx = 0; idx < max; ++idx)
	{
		e->GetErrorInfo( idx );
		error.Format( "%d: %s", fieldIndex, e->m_pErrorInfo->m_strDescription );
		status.Internal( IDS_INTERNAL_ERROR, error );
	}

	e->Delete();

	return status;
}

CReturn
CVariant::issueError( const CString& in_name, CDaoException* e )
{
	CReturn status;
	CString	error;

	int max = e->GetErrorCount();
	for (int idx = 0; idx < max; ++idx)
	{
		e->GetErrorInfo( idx );
		error.Format( "%s: %s", in_name, e->m_pErrorInfo->m_strDescription );
		status.Internal( IDS_INTERNAL_ERROR, error );
	}

	e->Delete();

	return status;
}

