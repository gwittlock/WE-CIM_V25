// ==================================================================
//		DaoDB
//
//
//	Generic DAO database manager... extend as needed.
//	Use as-is, or subclass as desired.  Just add water.  Use
//	only as directed.  Individual results may vary.  Side
//	effects may include headaches, naseau, hair loss, and ocular
//	bleeding.  If bleeding persists, see doctor immediately.
//
// NOTE: In our system, database field names always use ' ' to
// separate words in a field name.  In the C++ code, convention
// dictates that we use '_' to separate words, however.  This
// latter convention exists primarily to simplify the command
// parser (CCommand) and attribute names (CVar).  As such,
// this class incurs a little overhead to convert ' ' to '_'.
// ==================================================================

#include "stdafx.h"
#include <afxext.h>
#include <afxdao.h>

#include "cmn_resource.h"
#include "mm2Variant.h"

#include "DaoQuery.h"


// ==================================================================

CDaoQuery::CDaoQuery()
	: m_db( NULL ),
	  m_queryDef( NULL ),
	  m_queryResults( NULL ),
	  m_closeDb( FALSE )
{
}

CDaoQuery::~CDaoQuery()
{
	if ( m_closeDb )
		m_db->Close();

	Terminate();

	if (m_closeDb)
		AfxDaoTerm();
}

CReturn
CDaoQuery::Init( CDaoDatabase* db, const CString& sqlQuery )
{
	CReturn status;

#if (!_CI && !_NST)
	// Force DAO 3.6 and Jet 4.0...
	if (AfxGetModuleState()->m_dwVersion < 0x0601)
		AfxGetModuleState()->m_dwVersion = 0x0601;
#endif

	Terminate();

	if (db == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDaoQuery::Init()" );

	if ( status.IsOk() )
	{
		m_db = db;

		// NOTE: The following call to CDaoQueryDef() will
		// open the database if it is not already open.
		m_closeDb = !m_db->IsOpen();


		// Create a temporary query object.
		// TODO: Apparently, some efficiency can be
		// gained by storing queries in the database.
		try
		{
			m_queryDef = new CDaoQueryDef( db );
			m_queryDef->Create( NULL, sqlQuery );
		}
		catch( CException* e1 ) // CATCH( CDaoException, CMemoryException )
		{
			e1->ReportError();
			e1->Delete();		//Must NEVER user standard delete on CExceptions
			status.Internal( IDS_INTERNAL_ERROR, "CDaoQuery::Init()" );
		}
	}

	if ( status.IsOk() )
	{
		// The CDaoRecordset() constructor will attempt to
		// the given database, if it is not already open.
		try
		{
			m_queryResults = new CDaoRecordset( db );
			m_queryResults->Open( m_queryDef );

			if ( m_queryResults->IsEOF()
				&& m_queryResults->IsBOF() )
			{
				status.setStatus( STATUS_ERROR );
			}
			else
			{
				// ~!@#$%^&* MicroSoft!
				// The record count will be wrong if this is not done after the query,
				m_queryResults->MoveLast();		// Reset
				m_queryResults->MoveFirst();	// Go top
			}
		}
		catch( CException* e2 )  // CATCH( CDaoException, CMemoryException )
		{
			e2->Delete();		//Must NEVER user standard delete on CExceptions
			status.Internal( IDS_INTERNAL_ERROR, "CDaoQuery::Init()" );
			return status;
		}
	}

	if ( !status.IsOk() )
	{
		m_db = NULL;
		delete m_queryDef; m_queryDef = NULL;
		delete m_queryResults; m_queryResults = NULL;
	}

	return status;
}

// 2004/01/21 -- By making this public, we can now reuse
// query objects that have been pushed on the stack.
void
CDaoQuery::Terminate(void)
{
	if (m_queryResults != NULL)
	{
		if (m_queryResults->IsOpen())
		{ m_queryResults->Close(); }

		delete m_queryResults;
		m_queryResults = NULL;
	}

	if (m_queryDef != NULL)
	{
		delete m_queryDef;
		m_queryDef = NULL;
	}
}

int
CDaoQuery::RecordCount()
{
	return ((m_queryResults == NULL) ? 0 : m_queryResults->GetRecordCount());
}

int
CDaoQuery::FieldCount()
{
	return ((m_queryResults == NULL) ? 0 : m_queryResults->GetFieldCount());
}

// (-) backward one record / (0) to start / (+) forward one record
CReturn
CDaoQuery::Move( int dir )
{
	CReturn status;

	if (dir == 0)
	{
		try
		{
			m_queryResults->MoveLast();		// Reset
			m_queryResults->MoveFirst();	// Go top
		}
		catch( CException* e )  // CATCH( CDaoException, CMemoryException )
		{
			e->Delete();		//Must NEVER user standard delete on CExceptions
			status.Internal( IDS_INTERNAL_ERROR, "CDaoQuery::Move()" );
		}
	}
	else
	{
		try
		{
			m_queryResults->Move( ((dir < 0) ? -1 : 1) );
		}
		catch( CException* e )  // CATCH( CDaoException, CMemoryException )
		{
			e->Delete();		//Must NEVER user standard delete on CExceptions
			status.Internal( IDS_INTERNAL_ERROR, "CDaoQuery::Move()" );
		}
	}

	return status;
}

CReturn
CDaoQuery::MoveFirst()
{
	CReturn	status;

	try
	{
		m_queryResults->MoveFirst();
	}
	catch( CException* e )  // CATCH( CDaoException, CMemoryException )
	{
		e->Delete();		//Must NEVER user standard delete on CExceptions
		status.Internal( IDS_INTERNAL_ERROR, "CDaoQuery::MoveFirst()" );
	}

	return status;
}

CReturn
CDaoQuery::MoveLast()
{
	CReturn	status;

	try
	{
		m_queryResults->MoveLast();
	}
	catch( CException* e )  // CATCH( CDaoException, CMemoryException )
	{
		e->Delete();		//Must NEVER user standard delete on CExceptions
		status.Internal( IDS_INTERNAL_ERROR, "CDaoQuery::MoveLast()" );
	}

	return status;
}

/*
bool
CDaoQuery::Find( long lFindType, LPCTSTR lpszFilter )
{
	return ((m_queryResults == NULL) ? 0 : m_queryResults->Find( lFindType, lpszFilter ));
}
*/

CString
CDaoQuery::FieldNameGet( int indx )
{
	CVariant data;

	CString value = data.getFieldName( m_queryResults, indx );

	return value;
}

int
CDaoQuery::IntGet( int indx )
{
	CVariant data;

	int value = data.getInt( m_queryResults, indx );

	return value;
}

int
CDaoQuery::IntGet( const CString& fieldName )
{
	CVariant data;

	CString alias( fieldName );
	alias.Replace( '_', ' ' );

	int value = data.getInt( m_queryResults, alias );

	return value;
}

double
CDaoQuery::DoubleGet( int indx )
{
	CVariant data;

	double value = data.getDouble( m_queryResults, indx );

	return value;
}

double
CDaoQuery::DoubleGet( const CString& fieldName )
{
	CVariant data;

	CString alias( fieldName );
	alias.Replace( '_', ' ' );

	double value = data.getDouble( m_queryResults, alias );

	return value;
}

CString
CDaoQuery::StringGet( int indx )
{
	CVariant data;

	CString value = data.getString( m_queryResults, indx );

	return value;
}

CString
CDaoQuery::StringGet( const CString& fieldName )
{
	CVariant data;

	CString alias( fieldName );
	alias.Replace( '_', ' ' );

	CString value = data.getString( m_queryResults, alias );

	return value;
}

// Copies all of the attributes of the current
// database record to the given attribute list.
void
CDaoQuery::AttributesCopy( CVarList* attribs, int flags )
{
	int fieldCount = FieldCount();

	for (int indx = 0; indx < fieldCount; ++indx)
	{
		CString attribName = FieldNameGet( indx );
		CString attribValue = StringGet( indx );

		bool accept = TRUE;
		if (flags & DAO_IGNORE_COMPOUND_NAMES)
			accept = (attribName.Find('.') < 0);

		if ( accept )
		{
			if (flags & DAO_REPLACE_WHITE_SPACE)
				attribName.Replace( ' ', '_' );

			attribs->setString( attribName, attribValue );
		}
	}
}
