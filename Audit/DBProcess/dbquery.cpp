
#include "stdafx.h"

#include "cmn_resource.h"
#include "MathConst.h"

#include "DynamicArray.h"
#include "DaoDb.h"
#include "DaoQuery.h"
#include "DbProcess.h"

static CDynamicArray<CDaoQuery*> m_queryArray;

// NOTE: The address of the query serves as its ID.
// Therefore, QueryCreate() is responsible for creating a
// properly ordered list.
static int QueryCompareFunc( const void* myItem, const void* arrayItem )
{
	CDaoQuery* myQuery = (CDaoQuery*) myItem;
	CDaoQuery* arQuery = (CDaoQuery*) arrayItem;

	return ((int) arQuery - (int) myQuery);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
CDaoQuery* CDbProcessApp::QueryGet( int id )
{
	if (id <= 0)
		return NULL;

	int indx;
	bool found = m_queryArray.BinarySearch( (void*) id, &QueryCompareFunc, &indx );

	return ((found) ? m_queryArray[indx] : NULL);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryCreate:
// Returns id = %d
//
CReturn 
CDbProcessApp::QueryCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int dbId = -1;
	status += io_cmd->getInt( "db", &dbId );

	if (dbId <= 0)
	{
		status = STATUS_ERROR;
	}
	else
	{
		CDaoQuery* query = new CDaoQuery();

		if (query == NULL)
		{
			status = STATUS_ERROR;
		}
		else
		{
			int indx, count = m_queryArray.Count();
			for (indx = 0; indx < count; ++indx)
			{
				CDaoQuery* tmp = m_queryArray[indx];
				if ((int) tmp > (int) query)
					break;
			}
			m_queryArray.InsertBefore( indx, query );
			io_cmd->setInt( "id", (int) query );
		}
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryCreate()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryDestroy: id = %d
//
CReturn 
CDbProcessApp::QueryDestroy( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	status += io_cmd->getInt( "id", &id );

	if (id <= 0)
	{
		status = STATUS_ERROR;
	}
	else
	{
		int indx;
		bool found = m_queryArray.BinarySearch( (void*) id, &QueryCompareFunc, &indx );

		if ( found )
			delete ( m_queryArray.Remove( indx ) );
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryDestroy()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryExecute: id = %d, db = %d, name = %s
// Returns count = %d
//
CReturn 
CDbProcessApp::QueryExecute( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString sqlQuery;
	int id = -1;
	int dbId = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "dbid", &dbId );
	status += io_cmd->getString( "sql", &sqlQuery );

	CDaoQuery* query = QueryGet( id );
	CDaoDB* db = DatabaseGet( dbId );

	if (query == NULL || db == NULL || sqlQuery.GetLength() < 1)
	{
		status = STATUS_ERROR;
	}
	else
	{
		status = query->Init( db->Database(), sqlQuery );

		if ( status.IsOk() )
			io_cmd->setInt( "count", query->RecordCount() );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryTable()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryCount: id = %d
// Returns count = %d
//
CReturn 
CDbProcessApp::QueryCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	status += io_cmd->getInt( "id", &id );

	CDaoQuery* query = QueryGet( id );

	if (query == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryCount()" );
	else
		io_cmd->setInt( "count", query->RecordCount() );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryMoveIncr: id = %d, table = %d, delta = %d
// Returns bool = %d
//
CReturn 
CDbProcessApp::QueryMoveIncr( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	int delta = 0;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "delta", &delta );

	CDaoQuery* query = QueryGet( id );

	if (query == NULL)
	{
		status = STATUS_ERROR;
	}
	else
	{
		if (delta == 0)
		{
			status = query->Move( 0 );
		}
		else
		{
			int ub = ((delta < 0) ? -delta : delta);

			for(int indx = 0; indx < ub; ++indx )
			{
				status = query->Move( ((delta < 0) ? -1 : 1) );
				if ( !status.IsOk() )
					break;
			}

			io_cmd->setInt( "bool", (( status.IsOk() ) ? 1 : 0) );
		}
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryMoveIncr()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryIntGet: id = %d, table = %d, field = %s
// Returns val = %d
//
CReturn 
CDbProcessApp::QueryIntGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString field;
	int id = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "field", &field );

	CDaoQuery* query = QueryGet( id );

	if (query == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryIntGet()" );
	else
		io_cmd->setInt( "val", query->IntGet( field ) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryDblGet: id = %d, table = %d, field = %s
// Returns val = %f
//
CReturn 
CDbProcessApp::QueryDblGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString field;
	int id = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "field", &field );

	CDaoQuery* query = QueryGet( id );

	if (query == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryDoubleGet()" );
	else
		io_cmd->setReal( "val", query->DoubleGet( field ) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:QueryStrGet: id = %d, table = %d, field = %s
// Returns val = %s
//
CReturn 
CDbProcessApp::QueryStrGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString field;
	int id = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "field", &field );

	CDaoQuery* query = QueryGet( id );

	if (query == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::QueryStringGet()" );
	else
		io_cmd->setString( "val", query->StringGet( field ) );

	return status;
}
