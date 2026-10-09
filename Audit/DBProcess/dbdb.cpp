
#include "stdafx.h"

#include "cmn_resource.h"
#include "MathConst.h"

#include "DynamicArray.h"
#include "DaoDb.h"
#include "DbProcess.h"

static CDynamicArray<CDaoDB*> m_dbArray;

// NOTE: The address of the database serves as its ID.
// Therefore, DbCreate() is responsible for creating a
// properly ordered list.
static int DbCompareFunc( const void* myItem, const void* arrayItem )
{
	CDaoDB* myDb = (CDaoDB*) myItem;
	CDaoDB* arDb = (CDaoDB*) arrayItem;

	return ((int)arDb - (int)myDb);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
CDaoDB*
CDbProcessApp::DatabaseGet( int id )
{
	if (id <= 0)
		return NULL;

	int indx;
	bool found = m_dbArray.BinarySearch( (void*) id, &DbCompareFunc, &indx );

	return ((found) ? m_dbArray[indx] : NULL);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbCreate:
// Returns id = %d
//
CReturn 
CDbProcessApp::DbCreate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDaoDB* db = new CDaoDB();

	if (db == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbCreate()" );
	}
	else
	{
		int indx, count = m_dbArray.Count();
		for (indx = 0; indx < count; ++indx)
		{
			CDaoDB* tmp = m_dbArray[indx];
			if ((int) tmp > (int) db)
				break;
		}
		m_dbArray.InsertBefore( indx, db );
		io_cmd->setInt( "id", (int) db );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbDestroy: id = %d
//
CReturn 
CDbProcessApp::DbDestroy( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = 0;
	status += io_cmd->getInt( "id", &id );

	if ( status.IsOk() )
	{
		int indx;
		bool found = m_dbArray.BinarySearch( (void*) id, &DbCompareFunc, &indx );

		if ( found )
			delete ( m_dbArray.Remove( indx ) );
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbDestroy()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbOpen: id = %d, file = %s
// Returns bool = %d
//
CReturn 
CDbProcessApp::DbOpen( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString file;
	int id = 0;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "file", &file );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL)
	{
		status = STATUS_ERROR;
	}
	else
	{
		status += db->Open( file );
		
		io_cmd->setInt( "bool", (( status.IsOk() ) ? 1 : 0) );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbOpen()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbClose: id = %d
//
CReturn 
CDbProcessApp::DbClose( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = 0;
	status += io_cmd->getInt( "id", &id );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbClose()" );
	else
		db->Close();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbTable: id = %d, name = %s
// Returns index = %d
//
CReturn 
CDbProcessApp::DbTable( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name;
	int id = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "name", &name );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL)
	{
		status = STATUS_ERROR;
	}
	else
	{
		int tableNo = db->addTable( name );

		if (tableNo < 0)
			status = STATUS_ERROR;
		else
			io_cmd->setInt( "table", tableNo );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbTable()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbCount: id = %d, table = %d
// Returns count = %d
//
CReturn 
CDbProcessApp::DbCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	int tableNo = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "table", &tableNo );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbCount()" );
	else
		io_cmd->setInt( "count", db->countRecord( tableNo ) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbMoveAbs: id = %d, table = %d, recno = %d
// Returns bool = %d
//
CReturn 
CDbProcessApp::DbMoveAbs( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name;
	int id = -1;
	int tableNo = -1;
	int recNo = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "table", &tableNo );
	status += io_cmd->getInt( "recno", &recNo );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL || tableNo < 0)
	{
		status = STATUS_ERROR;
	}
	else
	{
		int count = db->countRecord( tableNo );

		if (recNo < 0 || recNo >= count)
		{
			status = STATUS_ERROR;
		}
		else
		{
			for(int indx = 0; indx < recNo; ++indx )
			{
				status = db->moveRecord( tableNo, ((indx == 0) ? 0 : 1) );
				if ( !status.IsOk() )
					break;
			}
		}

		io_cmd->setInt( "bool", (( status.IsOk() ) ? 1 : 0) );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbMoveAbs()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbMoveIncr: id = %d, table = %d, delta = %d
// Returns bool = %d
//
CReturn 
CDbProcessApp::DbMoveIncr( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name;
	int id = -1;
	int tableNo = -1;
	int delta = 0;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "table", &tableNo );
	status += io_cmd->getInt( "delta", &delta );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL || delta == 0)
	{
		status = STATUS_ERROR;
	}
	else
	{
		int ub = ((delta < 0) ? -delta : delta);

		for(int indx = 0; indx < ub+1; ++indx )
		{
			status = db->moveRecord( tableNo, ((delta < 0) ? -1 : 1) );
			if ( !status.IsOk() )
				break;
		}

		io_cmd->setInt( "bool", (( status.IsOk() ) ? 1 : 0) );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbMoveIncr()" );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbMoveIncr: id = %d, table = %d, field = %s, value = %s
// Returns recno = %d
//
CReturn 
CDbProcessApp::DbMove( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString fieldName;
	CString fieldValue;
	int id = -1;
	int tableNo = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "table", &tableNo );
	status += io_cmd->getString( "field", &fieldName );
	status += io_cmd->getString( "value", &fieldValue );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL || fieldName.GetLength() < 1)
	{
		status = STATUS_ERROR;
	}
	else
	{
		int recNo = db->findRecord( tableNo, fieldName, fieldValue );
		if (recNo < 0)
			status = STATUS_ERROR;

		io_cmd->setInt( "recno", recNo );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbMoveIncr()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbIntGet: id = %d, table = %d, field = %s
// Returns val = %d
//
CReturn 
CDbProcessApp::DbIntGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString field;
	int id = -1;
	int tableNo = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "table", &tableNo );
	status += io_cmd->getString( "field", &field );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbIntGet()" );
	else
		io_cmd->setInt( "val", db->getInt( tableNo, field ) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbDblGet: id = %d, table = %d, field = %s
// Returns val = %f
//
CReturn 
CDbProcessApp::DbDblGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString field;
	int id = -1;
	int tableNo = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "table", &tableNo );
	status += io_cmd->getString( "field", &field );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbDblGet()" );
	else
		io_cmd->setReal( "val", db->getDouble( tableNo, field ) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Db:DbStrGet: id = %d, table = %d, field = %s
// Returns val = %s
//
CReturn 
CDbProcessApp::DbStrGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString field;
	int id = -1;
	int tableNo = -1;

	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "table", &tableNo );
	status += io_cmd->getString( "field", &field );

	CDaoDB* db = DatabaseGet( id );

	if (db == NULL)
		status.Internal( IDS_INTERNAL_ERROR, "CDbProcessApp::DbStrGet()" );
	else
		io_cmd->setString( "val", db->getString( tableNo, field ) );

	return status;
}

