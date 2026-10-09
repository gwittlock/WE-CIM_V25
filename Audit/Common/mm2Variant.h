#ifndef _MM2VARIANT_H
#define _MM2VARIANT_H

// ==================================================================
//		Variant
//
//		Microsoft variant wrapper... 'cause MS Variants *SUCK*
//
// ==================================================================

// To stop vs2010 from bitching about depricated CDao*** stuff.
// According to some webpage I encountered, the correct solution
// is to convert from Jet to OleDb. But for now ....
#pragma warning(disable : 4995)

#include "stdafx.h"

#include <afxdisp.h>
#include <afxext.h>
#include <afxdao.h>

#include "Return.h"

// ==================================================================

class dllExport CVariant
{
public:

	CString			getFieldName( CDaoRecordset* in_record, int fieldIndex );

	CReturn			getField( CDaoRecordset* in_record, int fieldIndex );
	CReturn			getField( CDaoRecordset* in_record, const CString& in_name );
	CReturn			setField( CDaoRecordset* in_record, const CString& in_name );
	// -----------
	void			setVariant( const COleVariant& in_var )		{ m_var = in_var; }
	COleVariant		getVariant( void )							{ return m_var; }
	// -----------
	void			setString( const CString& in_str, bool in_other=FALSE );
	void			setString( CDaoRecordset* in_record, const CString& in_name, const CString& in_str, bool in_other=FALSE );

	CString			getString( void );
	CString			getString( CDaoRecordset* in_record, int fieldIndex );
	CString			getString( CDaoRecordset* in_record, const CString& in_name );
	// -----------
	void			setInt( int in_int );
	void			setInt( CDaoRecordset* in_record, const CString& in_name, int in_int );

	int				getInt( void );
	int				getInt( CDaoRecordset* in_record, int fieldIndex );
	int				getInt( CDaoRecordset* in_record, const CString& in_name );
	// -----------
	void			setDouble( double in_double );
	void			setDouble( CDaoRecordset* in_record, const CString& in_name, double in_double );

	double			getDouble( void );
	double			getDouble( CDaoRecordset* in_record, int fieldIndex );
	double			getDouble( CDaoRecordset* in_record, const CString& in_name );
	// -----------
	void			setMoney( double in_money );
	void			setMoney( CDaoRecordset* in_record, const CString& in_name, double in_money );
	// -----------
	void			setTime( void );
	void			setTime( const CTime& time );

	void			setTime( CDaoRecordset* in_record, const CString& in_name );
	void			setTime( CDaoRecordset* in_record, const CString& in_name, const CTime& time );

	CTime			getTime( void );
	CTime			getTime( CDaoRecordset* in_record, int fieldIndex );
	CTime			getTime( CDaoRecordset* in_record, const CString& in_name );
	// -----------

protected:
	COleVariant		m_var;

private:

	CReturn issueError( int fieldIndex, CDaoException* e );
	CReturn issueError( const CString& in_name, CDaoException* e );

};

#endif