#if !defined(_DAODB_H)
#define _DAODB_H

// ==================================================================
//		DaoDB
// ==================================================================
//
//	Generic DAO database manager... extend as needed.
//	Use as-is, or subclass as desired.  Just add water.
//
//	General operating sequence to read a table:
//		1.  addTable() to name relevant tables in the database
//		2.	Open() to open the database and named tables
//		3.	find, count, get, etc. to access records in database
//		4.	Close() when done!
//
//	To delete a table:
//		1. Open()
//		2. delTable()
//		3. Close()

//	To create a table:
//		1. Open()
//		2. createTable() to create a new table
//		3. newField() to add fields to the table.  Rinse, Repeat
//		4. commitTable() to complete the field creation
//		5. setIndex() to identify indices in the table
//		6. Close()
//		NOTE:  Can not access records in a table-create session
//		TODO:  Make it smarter, so records can be opened after the table is created?
//
// NOTE: In our system, database field names always use ' ' to
// separate words in a field name.  In the C++ code, convention
// dictates that we use '_' to separate words, however.  This
// latter convention exists primarily to simplify the command
// parser (CCommand) and attribute names (CVar).  As such,
// this class incurs a little overhead to convert ' ' to '_'.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include <afxext.h>
#include <afxdao.h>

#include "Return.h"
#include "DynamicArray.h"

// To stop vs2010 from bitching about depricated CDao*** stuff.
// According to some webpage I encountered, the correct solution
// is to convert from Jet to OleDb. But for now ....
#pragma warning(disable : 4995)

class CDaoQuery;


// ==================================================================

class dllExport CDaoDB
{
public:

	CDaoDB();

	virtual ~CDaoDB();

	//
	// Database Operations... top level stuff
	//
	void	Reset( void );

	CReturn	Create( const CString& in_file );
	CReturn	Open( const CString& in_file );
	CReturn	Close( void );

	bool IsOpen() const	{ return m_isOpen; }
	//
	// Table operations -- get existing, delete, create, populate
	//
	int		addTable( const CString& in_name );
	CReturn	delTable( const CString& in_name );
	bool	TableExists( const CString& tableName );

	int		createTable( const CString& in_name );
	CReturn	commitTable( int in_table );

	int loadTable( const CString& tableName );

	CReturn	newDouble( int in_table, const CString& in_field );
	CReturn	newInt( int in_table, const CString& in_field, bool in_auto );
	CReturn	newString( int in_table, const CString& in_field, int in_len );
	CReturn	newTime( int in_table, const CString& in_field );

	CReturn	setIndex( int in_table, const CString& in_field, bool in_primary );

	//
	// Record operations -- find, count, move around, access
	//
	int		findRecord( int in_table, const CString& in_index, const CString& in_value );
	int		findRecord( int in_table, const CString& in_index, int in_value );
	void	findClose( void );
	
	bool	isEOF( int in_table ) const						{ return (m_record[in_table]->IsEOF() != 0); }
	bool	isBOF( int in_table ) const						{ return (m_record[in_table]->IsBOF() != 0); }

	int		countRecord( int in_table ) const;
	int		atRecord( int in_table ) const;

	// Methods for moving to a record and making it the current record.
	CReturn	moveToRecord( int in_table, int index );
	CReturn	moveRecord( int in_table, int in_dir );

	CReturn moveFirst( int in_table );
	CReturn moveLast( int in_table );

	// Delete the current record.
	CReturn	delRecord( int in_table );

	// Methods for getting a value from the current record.
	double	getDouble( int in_table, const CString& in_field ) const;
	int		getInt( int in_table, const CString& in_field ) const;
	CString	getString( int in_table, const CString& in_field ) const;
	CTime	getTime( int in_table, const CString& in_field ) const;

	CReturn	createRecord( int in_table );
	CReturn editRecord( int in_table );
	// NOTE: commitRecord() closes the current record, making it unavailable.
	CReturn commitRecord( int in_table );

	CReturn	setDouble( int in_table, const CString& in_field, double in_val );
	CReturn	setInt( int in_table, const CString& in_field, int in_val );
	CReturn	setString( int in_table, const CString& in_field, const CString& in_val );
	CReturn	setTime( int in_table, const CString& in_field );
	CReturn	setTime( int in_table, const CString& in_field, const CTime& date );

	//
	// Stuff (introduced for) use by Java class AccessDb
	//

	int TableCount() const;
	CString TableName( int indx ) const;

	//
	// Access stuff
	//

	CDaoDatabase*	Database( void ) const			{ return m_database; }

	CDaoQuery* QueryExecute( const CString& sqlQuery ) const;

private:

	int GetTableIndex( const CString& tableName );
	int TableNameFind( const CString& tableName );

	CReturn ReportException( CDaoException* e );

private:  // Disabled.

	CDaoDB( const CDaoDB& );
	const CDaoDB& operator = ( const CDaoDB& );
	int operator == ( const CDaoDB& ) const;
	int operator != ( const CDaoDB& ) const;

private:

	CDaoDatabase*					m_database;

	bool							m_isOpen;

	CStringArray					m_tablename;
	CDynamicArray<CDaoTableDef*>	m_table;
	CDynamicArray<CDaoRecordset*>	m_record;

	CDaoQueryDef*	m_query_def;
	CDaoRecordset*	m_query_rec;
};

#endif


