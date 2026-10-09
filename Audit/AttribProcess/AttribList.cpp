
#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"
#include "DynamicArray.h"
#include "VarList.h"
#include "AttribProcess.h"

// ==================================================================

const char STRING_PREFIX = '$';
const char INT_PREFIX = '#';

static CDynamicArray<CVarList*> m_attribListArray;

// NOTE: The address of the attribute list serves as its ID.
// Therefore, AttribListNew() is responsible for creating a
// properly ordered list.
static int AttribCompareFunc( const void* myItem, const void* arrayItem )
{
	CVarList* myAttribs = (CVarList*) myItem;
	CVarList* arAttribs = (CVarList*) arrayItem;
	
	return (arAttribs - myAttribs);
}


// ==================================================================

// Attrib:List:New:
CReturn 
CAttribProcessApp::AttribListNew( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CVarList* attribs = new CVarList();

	if (attribs == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::AttribListNew()" );
	}
	else
	{
		int indx, count = m_attribListArray.Count();
		for (indx = 0; indx < count; ++indx)
		{
			CVarList* tmp = m_attribListArray[indx];
			if (tmp > attribs)
				break;
		}
		m_attribListArray.InsertBefore( indx, attribs );
		io_cmd->setInt( "id", (int) attribs );
	}

	return status;
}

// Attrib:List:Destroy: id = %d
CReturn 
CAttribProcessApp::AttribListDestroy( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	status += io_cmd->getInt( "id", &id );

	if ( status.IsOk() )
	{
		int indx;
		bool found = m_attribListArray.BinarySearch( (void*) id, &AttribCompareFunc, &indx );

		if ( found )
			delete ( m_attribListArray.Remove( indx ) );
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::AttribListFlush()" );

	return status;
}

// Attrib:List:Flush: id = %d
CReturn 
CAttribProcessApp::AttribListFlush( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	status += io_cmd->getInt( "id", &id );

	if ( status.IsOk() )
	{
		int indx;
		bool found = m_attribListArray.BinarySearch( (void*) id, &AttribCompareFunc, &indx );

		if ( found )
		{
			CVarList* attribs = m_attribListArray[indx];
			attribs->Reset();
		}
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::AttribListFlush()" );

	return status;
}

// Attrib:List:Count: id = %d
CReturn 
CAttribProcessApp::AttribListCount( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	status += io_cmd->getInt( "id", &id );

	if ( status.IsOk() )
	{
		int indx;
		bool found = m_attribListArray.BinarySearch( (void*) id, &AttribCompareFunc, &indx );

		if ( found )
		{
			CVarList* attribs = m_attribListArray[indx];
			io_cmd->setInt( "count", attribs->countVar() );
		}
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::AttribListFlush()" );

	return status;
}

// Attrib:List:Name: id = %d, indx = %d
CReturn 
CAttribProcessApp::AttribListName( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int id = -1;
	int indx = -1;
	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getInt( "indx", &indx );

	if ( status.IsOk() )
	{
		int jndx;
		bool found = m_attribListArray.BinarySearch( (void*) id, &AttribCompareFunc, &jndx );

		if ( found )
		{
			CVarList* attribs = m_attribListArray[jndx];
			CVar* var = attribs->getVar( indx );
			io_cmd->setString( "name", var->getName() );
		}
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::AttribListName()" );

	return status;
}

// Attrib:List:Get: id = %d, name = %s
CReturn 
CAttribProcessApp::AttribListGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name;
	int id = -1;
	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "name", &name );

	if ( status.IsOk() )
	{
		int indx;
		bool found = m_attribListArray.BinarySearch( (void*) id, &AttribCompareFunc, &indx );

		if ( found )
		{
			CString val;
			CVarList* attribs = m_attribListArray[indx];
			status = attribs->getString( name, &val );
			if ( status.IsOk() )
				io_cmd->setString( "val", val );
		}
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
	{
		CString msg;
		msg.Format( "CAttribProcessApp::AttribListGet( name = '%s' )", name );
		status.Internal( IDS_INTERNAL_ERROR, msg );
	}

	return status;
}

// Attrib:List:Set: id = %d, name = %s, val = %s
CReturn 
CAttribProcessApp::AttribListSet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name;
	CString val;
	int id = -1;
	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "name", &name );
	status += io_cmd->getString( "val", &val );

	if ( status.IsOk() )
	{
		int indx;
		bool found = m_attribListArray.BinarySearch( (void*) id, &AttribCompareFunc, &indx );

		if ( found )
		{
			CVarList* attribs = m_attribListArray[indx];
			attribs->setString( name, val );
		}
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::AttribListSet()" );

	return status;
}

// Attrib:List:Del: id = %d, name = %s
CReturn 
CAttribProcessApp::AttribListDel( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CString name;
	int id = -1;
	status += io_cmd->getInt( "id", &id );
	status += io_cmd->getString( "name", &name );

	if ( status.IsOk() )
	{
		int indx;
		bool found = m_attribListArray.BinarySearch( (void*) id, &AttribCompareFunc, &indx );

		if ( found )
		{
			CVarList* attribs = m_attribListArray[indx];
			attribs->deleteVar( name );
		}
		else
			status = STATUS_ERROR;
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAttribProcessApp::AttribListDel()" );

	return status;
}

