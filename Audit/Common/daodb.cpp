// ==================================================================
//		DaoDB
//
//
//	Generic DAO database manager... extend as needed.
//	Use as-is, or subclass as desired.  Just add water.  Use
//	only as directed.  Individual results may vary.  Side
//	effects may include headaches, nausea, hair loss, and ocular
//	bleeding.  If bleeding persists, see doctor immediately.
//
// NOTE: In our system, database field names always use ' ' to
// separate words in a field name.  In the C++ code, convention
// dictates that we use '_' to separate words, however.  This
// latter convention exists primarily to simplify the command
// parser (CCommand) and attribute names (CVar).  As such,
// this class incurs a little overhead to convert ' ' to '_'.
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"
#include "mm2Variant.h"

#include "DaoQuery.h"
#include "DaoDB.h"

// WARNING!  MFC's AfxDaoTerm() must not be called
// until all DAO objects are closed.  Otherwise,
// exceptions will be thrown.
static int g_count = 0;

// ==================================================================

CDaoDB::CDaoDB()
{
	m_isOpen = FALSE;

	m_database = NULL;
	m_query_def = NULL;
	m_query_rec = NULL;

	++g_count;
}

CDaoDB::~CDaoDB()
{
	Close();

	--g_count;
	if (g_count == 0)
		AfxDaoTerm();

	m_table.DestructiveFlush();
	m_record.DestructiveFlush();

	delete m_query_def;
	delete m_query_rec;
	delete m_database;
}


// =======================================================================
//		addTable
//
//	Returns index number for this table, to be used in all other
//	accesses.
//
//  NOTE: addTable() only works if called before Open() !!!!!
int
CDaoDB::addTable( const CString& in_name )
{
	int	idx = m_tablename.Add( in_name );

	m_table.Append( NULL );
	m_record.Append( NULL );

	return idx;
}

// =======================================================================
//		Create
//
CReturn
CDaoDB::Create( const CString& in_file )
{
	CReturn ret;

#if (!_CI && !_NST)
	// Force DAO 3.6 and Jet 4.0...
	if (AfxGetModuleState()->m_dwVersion < 0x0601)
		AfxGetModuleState()->m_dwVersion = 0x0601;
#endif

	Close();

	try
	{
		m_database = new CDaoDatabase();
		m_database->Create( in_file );

		m_isOpen = TRUE;
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

// =======================================================================
//		Open
//
CReturn
CDaoDB::Open( const CString& in_file )
{
	CReturn		ret;
	int			idx;

#if (!_CI && !_NST)
	// Force DAO 3.6 and Jet 4.0...
	if (AfxGetModuleState()->m_dwVersion < 0x0601)
		AfxGetModuleState()->m_dwVersion = 0x0601;
#endif

	Close();

	try
	{
		m_database = new CDaoDatabase();
		m_database->Open( in_file );

		// ---------------------

		for (idx=0; idx<m_tablename.GetSize(); idx++)
		{
			m_table.Replace( idx, new CDaoTableDef( m_database ) );
			m_table[idx]->Open( m_tablename[idx] );

			m_record.Replace( idx, new CDaoRecordset( m_database ) );
			m_record[idx]->Open( m_table[idx] );
		}

		m_isOpen = TRUE;
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

// =======================================================================
//		Close
//
CReturn
CDaoDB::Close( void )
{
	CReturn	ret;
	int		idx;

	findClose();

	m_isOpen = FALSE;

	if ( (m_tablename.GetSize() < 1)
		|| (m_record.Count() < 1)
		|| (m_table.Count() < 1 ) )
		return ret;

	for (idx=0; idx<m_tablename.GetSize(); idx++)
	{
		CDaoRecordset* record = m_record[idx];
		if (record)
		{

			if (record->IsOpen())
				record->Close();
			m_record.Replace( idx, NULL );
			delete record;
		}

		CDaoTableDef* table = m_table[idx];
		if (table)
		{
			if (table->IsOpen())
				table->Close();
			m_table.Replace( idx, NULL );

			delete table;
		}
	}
//	m_record.BenignFlush();
//	m_table.BenignFlush();

	// -----------------------

	if (m_database)
	{
		if (m_database->IsOpen())
			m_database->Close();
		delete m_database; m_database = NULL;
	}

	return ret;
}

// =======================================================================
//		moveToRecord
//		
//		Move to a specific record.
//
CReturn
CDaoDB::moveToRecord( int in_table, int index )
{
	CReturn	ret;

	try
	{
		m_record[in_table]->MoveFirst();
		if (index)
		{ m_record[in_table]->Move(index); }
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
	}

	return ret;
}

// =======================================================================
//		moveRecord
//		
//		Move a given direction in the specified table
//
CReturn
CDaoDB::moveRecord( int in_table, int in_dir )  // 0 for first, +1, -1 direction
{
	CReturn	ret;

	try
	{
		switch (in_dir)
		{
		case 0:
			m_record[in_table]->MoveFirst();
			break;

		case 1:
			if (m_record[in_table]->IsEOF())
				ret.Internal( IDS_DAO_EOF, m_tablename[in_table] );
			else
				m_record[in_table]->MoveNext();
			break;

		case -1:
			if (m_record[in_table]->IsBOF())
				ret.Internal( IDS_DAO_BOF, m_tablename[in_table] );
			else
				m_record[in_table]->MovePrev();
			break;
		}
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
	}

	return ret;
}

CReturn
CDaoDB::moveFirst( int in_table )
{
	CReturn status;

	try
	{
		m_record[in_table]->MoveFirst();
	}
	catch( CDaoException* e )
	{
		status = ReportException( e );
	}

	return status;
}

CReturn
CDaoDB::moveLast( int in_table )
{
	CReturn status;

	try
	{
		m_record[in_table]->MoveLast();
	}
	catch( CDaoException* e )
	{
		status = ReportException( e );
		e->Delete();
	}

	return status;
}

// Delete the current record of the given table.
CReturn
CDaoDB::delRecord( int in_table )
{
	CReturn status;

	try
	{
		m_record[in_table]->Delete();
	}
	catch( CDaoException* e )
	{
		status = ReportException( e );
		e->Delete();
	}

	return status;
}

// =======================================================================
//		getDouble
//
//		Get a double value from a table
//
double	
CDaoDB::getDouble(
	int				in_table,
	const CString&	in_field ) const
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	if (m_query_rec)
		return data.getDouble( m_query_rec, alias );
	else
		return data.getDouble( m_record[in_table], alias );
}


// =======================================================================
//		getInt
//
//		Get an integer value from a table
//
int
CDaoDB::getInt(
	int				in_table,
	const CString&	in_field ) const
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	if (m_query_rec)
		return data.getInt( m_query_rec, alias );
	else
		return data.getInt( m_record[in_table], alias );
}

// =======================================================================
//		getString
//
//		Get a string value from a table
//
CString
CDaoDB::getString( 
	int				in_table,
	const CString&	in_field ) const
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	if (m_query_rec)
		return data.getString( m_query_rec, alias );
	else
		return data.getString( m_record[in_table], alias );
}

// =======================================================================
//		getTime
//
//		Get an Time (eg: CTime) value from a table
//
CTime
CDaoDB::getTime(
	int				in_table,
	const CString&	in_field ) const
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	if (m_query_rec)
		return data.getTime( m_query_rec, alias );
	else
		return data.getTime( m_record[in_table], alias );
}

// =======================================================================
//		setDouble, setInt, setString
//
CReturn	
CDaoDB::setDouble( 
	int				in_table, 
	const CString&	in_field, 
	double			in_val )
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	data.setDouble( m_record[in_table], alias, in_val );
	return CReturn( STATUS_OKAY );
}

CReturn	
CDaoDB::setInt( 
	int				in_table, 
	const CString&	in_field, 
	int				in_val )
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	data.setInt( m_record[in_table], alias, in_val );
	return CReturn( STATUS_OKAY );
}

CReturn	
CDaoDB::setString( 
	int				in_table, 
	const CString&	in_field, 
	const CString&	in_val )
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	data.setString( m_record[in_table], alias, in_val, TRUE );
	return CReturn( STATUS_OKAY );
}

CReturn	
CDaoDB::setTime( 
	int				in_table, 
	const CString&	in_field )		// Set date/time to NOW
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	data.setTime( m_record[in_table], alias );
	return CReturn( STATUS_OKAY );
}

CReturn	
CDaoDB::setTime( 
	int				in_table, 
	const CString&	in_field, 
	const CTime&	in_val )
{
	CVariant	data;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	data.setTime( m_record[in_table], alias, in_val );
	return CReturn( STATUS_OKAY );
}


// =======================================================================
//		countRecord
//
int
CDaoDB::countRecord( int in_table ) const
{
	if (m_query_rec)
		return m_query_rec->GetRecordCount();
	else
		return m_record[in_table]->GetRecordCount();
}

// =======================================================================
//		atRecord
//
int
CDaoDB::atRecord( int in_table ) const
{
	return m_record[in_table]->GetAbsolutePosition();
}

// =======================================================================
//		createRecord, commitRecord
//
//	Add a new record, and commit its changes
//
CReturn
CDaoDB::createRecord( int in_table )
{
	CReturn	ret;

	try
	{
		if (m_record[in_table]->GetRecordCount())
			m_record[in_table]->MoveLast();
		m_record[in_table]->AddNew();
	}

	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

CReturn
CDaoDB::editRecord( int in_table )
{
	CReturn	ret;

	try
	{
		m_record[in_table]->Edit();
	}

	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

// NOTE: commitRecord() closes the current record, making it unavailable.
CReturn
CDaoDB::commitRecord( int in_table )
{
	CReturn	ret;

	try
	{
		m_record[in_table]->Update();
	}

	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

// =======================================================================
//		findRecord
//
//		Given an index name and value, find a record in the given table
//		and make it current.
//
//		Returns (-1) failure / (?) record number of matching record.
//
int
CDaoDB::findRecord( 
	int				in_table, 
	const CString&	in_index, 
	const CString&	in_value )
{
	CReturn	status;
	int record_indx = -1;  // assume failure

	try
	{
		int idx, num = countRecord( in_table );
		status = moveRecord( in_table, 0 );

		if ( status.IsOk() )
		{
			for (idx=0; idx<num; idx++)
			{
				CString rec_val = getString( in_table, in_index );
				if (rec_val.CompareNoCase( in_value ) == 0)
					break;

				moveRecord( in_table, 1 );
			}

			if (idx < num)
				record_indx = idx;
		}
	}
	catch( CDaoException* e )
	{
		status = ReportException( e );
		Close();
	}

	return record_indx;
}

int
CDaoDB::findRecord( 
	int				in_table, 
	const CString&	in_index, 
	int				in_value )
{
	CReturn	status;
	int record_indx = -1;  // assume failure

	try
	{
		int idx, num = countRecord( in_table );
		status = moveRecord( in_table, 0 );

		if ( status.IsOk() )
		{
			for (idx=0; idx<num; idx++)
			{
				int rec_val = getInt( in_table, in_index );
				if (rec_val == in_value)
					break;

				moveRecord( in_table, 1 );
			}

			if (idx < num)
				record_indx = idx;
		}
	}
	catch( CDaoException* e )
	{
		status = ReportException( e );
		Close();
	}

	return record_indx;
}


// ==================================================================
void
CDaoDB::findClose( void )
{
	if (m_query_rec)
	{
		m_query_rec->Close();
		delete m_query_rec; m_query_rec = NULL;
	}

	if (m_query_def)
	{
		m_query_def->Close();
		delete m_query_def; m_query_def = NULL;
	}
}


// ==================================================================
CReturn	
CDaoDB::delTable( const CString& in_name )
{
	CReturn	ret;

	try
	{
		CDaoTableDefInfo	table_info;

		// Prepare for deletion; close and clear entries
		int idx, num=m_tablename.GetSize();
		for (idx=0; idx<num; idx++)
		{
			if (m_tablename[idx].CompareNoCase(in_name) == 0)
			{
				if (m_table[idx])
				{
					if (m_table[idx]->IsOpen())
						m_table[idx]->Close();
					delete m_table[idx];
					m_table.Replace( idx, NULL );
				}
				m_tablename[idx] = "";
			}
		}

		// Track this table down... don't want to delete it if it
		//	doesn't exist (errors and other icky stuff).
		num = m_database->GetTableDefCount();
		for (idx=0; idx<num; idx++)
		{
			m_database->GetTableDefInfo( idx, table_info );
			if (table_info.m_strName.CompareNoCase( in_name ) == 0)
			{
				// Zowie! It's Gone!
				m_database->DeleteTableDef( in_name );
				break;
			}
		}
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}


// ==================================================================
int
CDaoDB::createTable( const CString& in_name )
{
	int idx = -1;  // assume failure
	try
	{
		int table_idx = m_tablename.Add( in_name );

		CDaoTableDef* table = new CDaoTableDef( m_database );
		table->Create( in_name );

		m_table.Append( table );
		m_record.Append( NULL );

		idx = table_idx;
	}
	catch( CDaoException* e )
	{
		CReturn	ret = ReportException( e );
		Close();
	}

	return idx;
}

CReturn	
CDaoDB::commitTable( int in_table )
{
	CReturn	ret;

	try
	{
		m_table[in_table]->Append();

		m_record.Replace( in_table, new CDaoRecordset( m_database ) );
		m_record[in_table]->Open( m_table[in_table] );
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

int
CDaoDB::loadTable( const CString& tableName )
{
	int indx = TableNameFind( tableName );
	if (indx >= 0)
	{
		// ERROR: The table has already been loaded/added.
		indx = -1;
	}
	else
	{
		indx = GetTableIndex( tableName );
		if (indx < 0)
		{
			// ERROR: The table does not exist in the DAO database.
		}
		else
		{
			try
			{
				// NOTE: There is a one-to-one correspondence
				// between m_tablename, m_table and m_record.
				indx = m_tablename.Add( tableName );

				m_table.Append( new CDaoTableDef( m_database ) );
				m_table[indx]->Open( m_tablename[indx] );

				m_record.Append( new CDaoRecordset( m_database ) );
				m_record[indx]->Open( m_table[indx] );
			
			}
			catch( CDaoException* e )
			{
				CReturn	ret = ReportException( e );
				Close();

				indx = -1;
			}
		}
	}

	return indx;
}

// ==================================================================
//		newField
//
//	Create new field types in the table
//
CReturn	
CDaoDB::newDouble( 
	int				in_table, 
	const CString&	in_field )
{
	CReturn	ret;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	try
	{
		m_table[in_table]->CreateField( alias, dbDouble, 0, dbUpdatableField );
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

CReturn	
CDaoDB::newInt( 
	int				in_table, 
	const CString&	in_field,
	bool			in_auto )		// TRUE if auto-increment
{
	CReturn	ret;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	try
	{
		if (in_auto)
			m_table[in_table]->CreateField( alias, dbLong, 0, dbAutoIncrField );
		else
			m_table[in_table]->CreateField( alias, dbLong, 0, dbUpdatableField );
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

#if (_CI || _NST)

	CReturn	
	CDaoDB::newString( 
		int				in_table, 
		const CString&	in_field,
		int				in_len )		// -1 == Memo
	{
		CReturn	status;

		CString alias( in_field );
		alias.Replace( '_', ' ' );

		try
		{
			if (in_len < 0)
			{
				m_table[in_table]->CreateField( alias, dbMemo, 0, dbUpdatableField );
			}
			else
			{
				CDaoFieldInfo field_info;

				// Of course MicroSoft, in their ~!@#$%^&* wisdom
				// could not make m_bAllowZeroLength an attribute
				// like dbUpdatableField, so we have to set all
				// of the data in a brute force manner.
				//
				field_info.m_strName = in_field;
				field_info.m_nType = dbText;
				field_info.m_lSize = min( in_len, 255 );  // ie. no bigger than 255
				field_info.m_lAttributes = dbUpdatableField;
				field_info.m_nOrdinalPosition = 0;
				field_info.m_bRequired = FALSE;
				field_info.m_bAllowZeroLength = TRUE;
				field_info.m_lCollatingOrder = 0;
				field_info.m_strForeignName = "";
				field_info.m_strSourceField = "";
				field_info.m_strSourceTable = "";
				field_info.m_strValidationRule = "";
				field_info.m_strValidationText = "";
				field_info.m_strDefaultValue = "";

				m_table[in_table]->CreateField( field_info );
			}
		}
		catch( CDaoException* e )
		{
			status = ReportException( e );
			Close();
		}

		return status;
	}

#else

	CReturn	
	CDaoDB::newString( 
		int				in_table, 
		const CString&	in_field,
		int				in_len )		// -1 == Memo
	{
		CReturn	ret;

		CString alias( in_field );
		alias.Replace( '_', ' ' );

		try
		{
			if (in_len < 0)
				m_table[in_table]->CreateField( alias, dbMemo, 0, dbUpdatableField );
			else
			{
				int len = max( min( 1, in_len ), 255 );
				m_table[in_table]->CreateField( alias, dbText, len, dbUpdatableField );
			}
			return ret;
		}
		catch( CDaoException* e )
		{
			ret = ReportException( e );
			Close();
		}

		return ret;
	}

#endif

CReturn	
CDaoDB::newTime( 
	int				in_table, 
	const CString&	in_field )
{
	CReturn	ret;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	try
	{
		m_table[in_table]->CreateField( alias, dbDate, 0, dbUpdatableField );
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}

// ==================================================================
//		setIndex
//
//	Identify a field as a primary or secondary index field.
//
CReturn	
CDaoDB::setIndex( 
	int				in_table, 
	const CString&	in_field, 
	bool			in_primary )
{
	CReturn	ret;

	CString alias( in_field );
	alias.Replace( '_', ' ' );

	try
	{
		CDaoIndexInfo		idx_info;
		CDaoIndexFieldInfo	idx_field;

		idx_field.m_strName		= alias;
		idx_field.m_bDescending	= FALSE;

		idx_info.m_strName		= alias;
		idx_info.m_pFieldInfos	= &idx_field;
		idx_info.m_nFields		= 1;
		idx_info.m_bPrimary		= in_primary;
		idx_info.m_bUnique		= in_primary;
		idx_info.m_bClustered	= FALSE;
		idx_info.m_bIgnoreNulls = FALSE;
		idx_info.m_bRequired	= TRUE;
		idx_info.m_bForeign		= FALSE;

		m_table[in_table]->CreateIndex( idx_info );
	}
	catch( CDaoException* e )
	{
		ret = ReportException( e );
		Close();
	}

	return ret;
}


int
CDaoDB::TableCount() const
{
	int count = 0;

	try
	{
		if (m_database != NULL)
			count = m_database->GetTableDefCount();
	}
	catch( CDaoException* e )
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CDaoDB::TableCount()" );
		e->Delete();
	}

	return count;
}

CString
CDaoDB::TableName( int indx ) const
{
	CDaoTableDefInfo defInfo;
	CString tableName;

	try
	{
		if (m_database != NULL)
		{
			m_database->GetTableDefInfo( indx, defInfo, AFX_DAO_PRIMARY_INFO );
			tableName = defInfo.m_strName;
		}
	}
	catch( CDaoException* e )
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CDaoDB::TableName()" );
		e->Delete();
	}

	return tableName;
}
	
CDaoQuery*
CDaoDB::QueryExecute( const CString& sqlQuery ) const
{
	CReturn status;

	CDaoQuery* daoQuery = new CDaoQuery;
	status = daoQuery->Init( Database(), sqlQuery );

	if ( status.IsOk() )
	{
		daoQuery->Move( 0 );

		int recordCount = daoQuery->RecordCount();

		if (recordCount < 1)
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
	{
		CString errMsg;
		errMsg.Format( "CDaoDB::QueryExecute( \"%s\" )", sqlQuery );
		status.Internal( IDS_INTERNAL_ERROR, errMsg );

		delete daoQuery;
		daoQuery = NULL;
	}

	return daoQuery;
}

bool
CDaoDB::TableExists( const CString& tableName )
{
	int indx = GetTableIndex( tableName );
	return (indx >= 0);
}

// Searches the DAO database.
int
CDaoDB::GetTableIndex( const CString& tableName )
{
	int indx, count = TableCount();
	for (indx = 0; indx < count; ++indx)
	{
		if (TableName(indx).CompareNoCase( tableName ) == 0)
			break;  // found a match
	}

	return ((indx < count) ? indx : -1);
}

// Searches m_tablename[].
int
CDaoDB::TableNameFind( const CString& tableName )
{
	int indx, count = m_tablename.GetSize();
	for (indx = 0; indx < count; ++indx)
	{
		if (m_tablename[indx].CompareNoCase(tableName) == 0)
			break;
	}

	return ((indx < count) ? indx : -1);
}

CReturn
CDaoDB::ReportException( CDaoException* e )
{
	CReturn status;

	int count = e->GetErrorCount();
	for (int indx=0; indx<count; indx++)
	{
		e->GetErrorInfo( indx );
		status.Internal( IDS_INTERNAL_ERROR, e->m_pErrorInfo->m_strDescription );
	}

	e->Delete();

	return status;
}
