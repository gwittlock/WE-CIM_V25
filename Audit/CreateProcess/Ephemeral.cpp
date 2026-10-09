// ==================================================================
// Ephemeral.cpp : Non-corporeal creations
//
// ==================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "cmn_resource.h"

#include "dbWorkplane.h"
#include "DbTool.h"

#include "ViewMgr.h"

#include "CreateProcess.h"

// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// DEPRICATED
CReturn 
CCreateProcessApp::Layer( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return Tool( io_cmd );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// All forms of the Portal command Create:Tool: have the side-effect
// of setting the model's active tool to be the indicated tool.
//
// Set the model's active tool to indicated tool (should use Model:ActiveTool: id=%d)
//    Create:Tool: id=%d
//
// Create/Find and set model's active tool to indicated tool
// If wp is specified, associates the tool with that workplane.
// Otherwise, the tool is associated with the model's active workplane.
//    Create:Tool: name=%d [,wp=%d]
//
// Creates and unnamed tool and set model's active tool to indicated tool
// If wp is specified, associates the tool with that workplane.
// Otherwise, the tool is associated with the model's active workplane.
//    Create:Tool: [wp=%d]
//
CReturn 
CCreateProcessApp::Tool( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;

	// ret.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Layer() is obsolete." );

	// -----------------------------------------------------
	//		Extract command
	//
	ID wp = io_cmd->VarList().getInt( "wp", 0 );
	ID id = io_cmd->VarList().getInt( "id", 0 );
	CString name = io_cmd->VarList().getString( "name", "" );

	if (id > 0 && name.GetLength() > 0)
	{
		// Can't have it both ways ...
		ret.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Tool()" );
	}
	else
	{
		CDbWorkplane*	dbWork;
		CDbTool*		dbTool;
		bool			createTool;
		
		dbWork = NULL;
		dbTool = NULL;

		if (wp > 0 )
			model.EntityFind( wp, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

		if (dbWork == NULL)
			dbWork = model.ActiveWorkplane();

		if (id > 0)
		{
			ret = model.EntityFind( id, (CDbEntity**)&dbTool, DBTOOL, DBTOOL );
			createTool = FALSE;
		}
		else if (name.GetLength() > 0)
		{
			ret = model.EntityFind( name, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
			createTool = (dbTool == NULL);
		}
		else
		{
			createTool = TRUE;
		}

		if ( createTool )
		{
			ret = model.EntityCreate( DBTOOL, (CDbEntity**)&dbTool );
			dbTool->Workplane( dbWork );

			if (ret.IsOk() && name.GetLength() > 0)
			{
				if ( !dbTool->Name( name ) )
					ret = STATUS_ERROR;  // name is invalid or not unique
			}
		}

		if (ret.isOkay())
		{
			model.ActiveTool( dbTool );
			io_cmd->setInt( "id", dbTool->Id() );
		}
	}

	return ret;
}


// ==================================================================
// Create:Plane:
//			id
//			[name]
//			ix, iy, iz		// Define by vectors
//			jx, jy, jz
//			kx, ky, kz	or  ks (z sign, +-1)
//			tx, ty, tz
//			up				// determines if tool up is same s k vector +-1
//
//	TODO:  Clean up logic; I'm not thinking clearly, but this needs to
//		handle creation, selection, and *modification*
//
CReturn 
CCreateProcessApp::Plane( 
	CCommand*	io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CModel&	model = io_cmd->getModel();
	CReturn			ret;
	CString			name;
	ID				id;
	// Bools for debugging, tracking, and for my fuzzy brain
	bool			select = FALSE;
	bool			create = FALSE;
	bool			modify = FALSE;

	// -----------------------------------------------------
	//		Extract command
	//
	id = 0;
	io_cmd->getInt( "id", (int*)&id );
	io_cmd->getString( "name", &name );

	if (id > 0 && name.GetLength() > 0)
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Plane()" );
		return ret;
	}

	// -----------------------------------------------------
	//		Check for selection
	//
	CDbWorkplane*	plane = NULL;
	if (id > 0)
	{
		model.EntityFind( id, (CDbEntity**)&plane, DBWORKPLANE, DBWORKPLANE );
		ASSERT( plane != NULL);
		if ( plane->Type() != DBWORKPLANE )
		{
			ret.Internal( IDS_SELECT_TYPE );
			return ret;
		}
	}
	else if (name.GetLength() > 0)
	{
		model.EntityFind( name, (CDbEntity**)&plane, DBWORKPLANE, DBWORKPLANE );
	}

	if (plane)
	{
		select = TRUE;
	}
	else
	{
		ret += model.EntityCreate( DBWORKPLANE, (CDbEntity**)&plane );
		create = TRUE;
	}

	if ( !plane )
	{
		ret.Internal( IDS_CREATE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	// Get the remaining parameters...
	//
	double	ix, iy, iz;
	C3dVec	ivec;
	double	jx, jy, jz;
	C3dVec	jvec;
	double	kx, ky, kz;
	C3dVec	kvec;
	double	tx, ty, tz;
	int		ks;
	CReturn parm;

	parm += io_cmd->getReal( "ix", &ix );
	parm += io_cmd->getReal( "iy", &iy );
	parm += io_cmd->getReal( "iz", &iz );
	if (parm.isOkay())
	{
		ivec = C3dVec( ix, iy , iz );
		ivec = ivec * (1/ivec.Length());
	}

	parm += io_cmd->getReal( "jx", &jx );
	parm += io_cmd->getReal( "jy", &jy );
	parm += io_cmd->getReal( "jz", &jz );
	if (parm.isOkay())
	{
		jvec = C3dVec( jx, jy , jz );
		jvec = jvec * (1/jvec.Length());
	}

	if (io_cmd->getInt( "ks", &ks ).isOkay())
	{
		kvec = ivec ^ jvec;

		if (!ks) ks = -1;

		jvec = kvec ^ ivec;
		kvec = (ivec ^ jvec) * ks;

		ivec = ivec * (1 / ivec.Length() );
		jvec = jvec * (1 / jvec.Length() );
		kvec = kvec * (1 / kvec.Length() );
	}
	else
	{
		parm += io_cmd->getReal( "kx", &kx );
		parm += io_cmd->getReal( "ky", &ky );
		parm += io_cmd->getReal( "kz", &kz );

		if (parm.isOkay())
		{
			kvec = C3dVec( kx, ky, kz );
			kvec = kvec * (1/kvec.Length());
		}
	}

	parm += io_cmd->getReal( "tx", &tx );
	parm += io_cmd->getReal( "ty", &ty );
	parm += io_cmd->getReal( "tz", &tz );

	int up = 1;
	io_cmd->getInt( "up", &up );

	if (parm.isOkay())
		modify = TRUE;

	if ( create
		&& !modify )
	{
		ret.Internal( IDS_CREATE_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//
	if (modify)
		plane->Init( ivec, jvec, kvec, C3dCoord( tx, ty, tz ), up );

	if (name.GetLength() > 0)
	{
		if ( !plane->Name( name ) )
			ret = STATUS_ERROR;  // name is invalid or not unique
	}

	if ( ret.IsOk() )
	{
		id = plane->Id();
		io_cmd->setInt( "id", id );

		CDbWorkplane* last_plane = model.ActiveWorkplane();
		model.ActiveWorkplane( plane );

#if OKAY  // The following code sequence causes crash during RTL run
		if (last_plane)
			io_cmd->getViewMgr().Regenerate( model, last_plane->Id() );

		io_cmd->getViewMgr().Regenerate( model, plane->Id() );
		io_cmd->getViewMgr().Clear();
		io_cmd->getViewMgr().Refresh();
#endif
	}

	return ret;
}

