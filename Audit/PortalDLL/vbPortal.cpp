// ==================================================================
//		Visual Basic Portal
//
//	Though technically, this portal can be used by C++ applications,
//	it is a functional and not an object interface so it is really
//	focused towards the needs of VB parents.
//
//	All DLL entry-points go through here.
//
// ==================================================================

#include "stdafx.h"

#include "register.h"
#include "3dCoord.h"
#include "Portal.h"
#include "vbPortal.h"

#if 1   // otherwise, the dlls are simply stubs (to test whether app resolves the dlls)

#define GARY 0

//	Visual Basic interface to register command-recipients
BOOL DLL_DECL vbInit(
	const char* regRootPath,
	const char* dllPath,
	const char* classPath,
	const char* storagePath,
	int suppressUndoBuffer )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	//MessageBox(NULL, regRootPath, "Debug", MB_OK);

	CString dll_path( dllPath );
	CString class_path( classPath );
	CString storage_path( storagePath );

	CString hkey( "HKEY_CURRENT_USER" );
	CString reg_root;
	// 'regRootPath' should be provided as (eg.) "HKEY_CURRENT_USER\Software\WE-CIM\..."
	CString path( regRootPath );
	if (0)
		path = CString("HKEY_CURRENT_USER\\") + CString( regRootPath );

	if (path.Find( "HKEY_" ) == 0)
	{
		int indx = path.Find( '\\' );
		hkey = path.Mid( 0, indx );
		reg_root = path.Mid( indx+1 );
	}
	else
	{
		// We should *never* arrive here but ....
		reg_root = regRootPath;
	}

	// The options were introduce for ITI. A standard installation uses HKEY_CURRENT_USER
	// and a server installation (aka "silent mode") uses HKEY_LOCAL_MACHINE.
	CRegister::HKEYSet( (hkey == "HKEY_LOCAL_MACHINE") ? LOCAL_MACHINE : CURRENT_USER );

	// CRITICAL!
	CRegister::SetRootPath( reg_root );

	CString over_ride = CRegister::StringGetV( "Java", "override", "" );
	if ( !over_ride.IsEmpty() )
		class_path = over_ride;

#if GARY
	CString msg;
	msg.Format( "Message from vbInit():\n\nhkey <%s>\n\nregRootPath <%s>\n\ndllpath <%s>\n\nclassPath <%s>\n\nstoragePath <%s>\n\nsuppressUndoBuffer <%d>",
		hkey, reg_root, dll_path, class_path, storage_path, suppressUndoBuffer );
	MessageBox( NULL, msg, "Debug", MB_OK );
#endif

	CString jvmDllFolder = CRegister::StringGetV("Java", "jre", "");

	CReturn status = CPortal::Init( dll_path, jvmDllFolder, class_path, storage_path, suppressUndoBuffer );

	return status.IsOk();
}

#if (_CI || _NST)
	// which (0) main / (1) auxillary / (2) undo
	BOOL DLL_DECL
	ciHookup(
			LPDISPATCH	docObject,
			LPDISPATCH	cmdObject,
			int			which )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CReturn status = CPortal::Hookup( docObject, cmdObject, (eCiHookup) which );

		return status.IsOk();
	}
#else
	BOOL DLL_DECL
	ciHookup(
			LPDISPATCH	docObject,
			LPDISPATCH	cmdObject,
			int			which )
	{
		return 0;
	}
#endif

// ==================================================================
//	vbTerminate
//
BOOL DLL_DECL vbTerminate()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	EWMTerminate();

	CReturn status = CPortal::Terminate();
	return status.IsOk();
}

// ==================================================================
//	vbRegister
//
//	Visual Basic interface to register command-recipients... for 
//	additional command locations, not covered in vbInit().
//
BOOL DLL_DECL vbRegister( const char* in_dllPath )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status = CPortal::Register( in_dllPath );
	return status.IsOk();
}

// ==================================================================
//	vbExecute
//
//	Visual Basic interface to execute commands
//
BOOL DLL_DECL vbExecute( const char* in_command )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status = CPortal::Execute( in_command );
	return status.IsOk();
}

// ==================================================================
//	vbVerify
//
//	Visual Basic interface to verify command routing
//
BOOL DLL_DECL vbVerify( const char* in_command )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return CPortal::Verify( in_command );
}

// ==================================================================
//	vbParse
//
//	Visual Basic interface to parse commands into routelist & variables
//
BOOL DLL_DECL vbParse( const char* in_command )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//CReturn msg;
//msg.Note( in_command );

	CReturn status = CPortal::Parse( in_command );
	return status.IsOk();
}

// ==================================================================
BOOL DLL_DECL vbGetInt(
	const char* in_name,
	int*			io_val )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status = CPortal::GetInt( in_name, io_val );

	return status.IsOk();
}

// ==================================================================
BOOL DLL_DECL vbGetReal( 
	const char* in_name,
	double*		io_val )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status = CPortal::GetReal( in_name, io_val );

	return status.IsOk();
}

#if (_CI || _NST)

	// ==================================================================
	int DLL_DECL vbGetString( 
		const char* in_name,
		char*		io_val,
		int			in_buflen )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		io_val[0] = '\0';
		CReturn status = CPortal::GetString( in_name, io_val, in_buflen );
		return ( strlen( io_val ) );
	}

	// ==================================================================
	// Same as vbGetString in this case; included to satisfy the .def.
	int DLL_DECL vbGetString2( 
		const char* in_name,
		char*		io_val,
		int			in_buflen )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		io_val[0] = '\0';
		CReturn status = CPortal::GetString( in_name, io_val, in_buflen );
		return ( strlen( io_val ) );
	}

#else

	// ==================================================================
	int DLL_DECL vbGetString( 
		const char* in_name,
		char*		io_val,
		int			in_buflen )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CReturn	status;
		int		len = 0;

		try
		{
			io_val[0] = '\0';
			status = CPortal::GetString( in_name, io_val, in_buflen );
			len = strlen( io_val );
		}
		catch(...)
		{
			CString	msg;
			msg.Format( "vbGetString( \"%s\", 0x%x, %d )",
				in_name, io_val, in_buflen );

			status.Exception( msg );
		}

		return len;
	}

	// ==================================================================
	int DLL_DECL vbGetString2( 
		const char* in_name,
		char*		io_val,
		int			in_buflen )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CReturn	status;
		int		len = 0;

		try
		{
			io_val[0] = '\0';
			status = CPortal::GetString( in_name, io_val, in_buflen );
			len = strlen( io_val );
		}
		catch(...)
		{
			CString	msg;
			msg.Format( "vbGetString2( \"%s\", 0x%x, %d )",
				in_name, io_val, in_buflen );

			status.Exception( msg );
		}

		return len;
	}

#endif

BOOL DLL_DECL vbModelSwap( int use_file_preview_model )
{
	CPortal::ModelSwap( (use_file_preview_model != 0) );
	return TRUE;
}

// ==================================================================

BOOL DLL_DECL vbScreenToWorld( vbPoint* io_point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3dCoord point( io_point->x, io_point->y, io_point->z );

	BOOL status = CPortal::ScreenToWorld( &point );

	io_point->x = point.X();
	io_point->y = point.Y();
	io_point->z = point.Z();

	return TRUE;
}

BOOL DLL_DECL vbWorldToScreen( vbPoint* io_point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint	screen;
	C3dCoord world( io_point->x, io_point->y, io_point->z );

	BOOL status = CPortal::WorldToScreen( world, &screen );

	io_point->x = screen.x;
	io_point->y = screen.y;
	io_point->z = 0;

	return TRUE;
}

// ==================================================================

BOOL DLL_DECL vbRefToWorld( vbPoint* io_point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3dCoord point( io_point->x, io_point->y, io_point->z );

	BOOL status = CPortal::RefToWorld( &point );

	io_point->x = point.X();
	io_point->y = point.Y();
	io_point->z = point.Z();

	return TRUE;
}

BOOL DLL_DECL vbWorldToRef( vbPoint* io_point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3dCoord point( io_point->x, io_point->y, io_point->z );

	BOOL status = CPortal::WorldToRef( &point );

	io_point->x = point.X();
	io_point->y = point.Y();
	io_point->z = point.Z();

	return TRUE;
}

// ==================================================================

BOOL DLL_DECL vbViewToScreen( vbPoint* io_point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPoint	screen;
	C3dCoord view( io_point->x, io_point->y, io_point->z );

	BOOL status = CPortal::ViewToScreen( view, &screen );

	io_point->x = screen.x;
	io_point->y = screen.y;
	io_point->z = 0;

	return TRUE;
}



// ==================================================================
// TEMPORARY, FOR TESTING
// ==================================================================

// ==================================================================
//	vbSnap
//
//	Dummy snap function, snap to 10x grid
//
BOOL DLL_DECL vbSnap( vbPoint* io_point )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	io_point->x = ((int)(io_point->x / 10)) * 10;
	io_point->y = ((int)(io_point->y / 10)) * 10;
	io_point->z = ((int)(io_point->z / 10)) * 10;

	return TRUE;
}


#else

BOOL_DLL vbInit(
	const char* regRootPath,
	const char* dllPath,
	const char* classPath,
	const char* storagePath,
	int suppressUndoBuffer )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#if 1  // this works
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	CReturn status;
	MessageBox( NULL, (status.IsOk() ? "okay" : "not okay"), "caption", MB_OK );
#else

	// CRITICAL!
	// CRegister::SetRootPath( regRootPath );

	// CReturn status = CPortal::Init( dllPath, classPath, storagePath, suppressUndoBuffer );
	CReturn status;
	MessageBox( NULL, (status.IsOk() ? "okay" : "not okay"), "caption", MB_OK );
#endif
	return FALSE;
}

BOOL DLL_DECL ciHookup(
			LPDISPATCH	docObject,
			LPDISPATCH	cmdObject,
			int			which )
{
	return FALSE;
}

BOOL_DLL vbTerminate()
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


BOOL_DLL vbRegister( const char* in_subdir )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}

BOOL_DLL vbExecute( const char* in_command )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}

BOOL_DLL vbVerify( const char* in_command )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}

BOOL_DLL vbParse( const char* in_command )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


BOOL_DLL vbGetInt( const char* in_name, int* io_val )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}

BOOL_DLL vbGetReal( const char* in_name, double* io_val )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


BOOL_DLL vbModelSwap( int use_file_preview_model )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


INT_DLL vbGetString( const char* in_name, char* io_val, int in_buflen )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}

INT_DLL vbGetString2( const char* in_name, char* io_val, int in_buflen )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}



BOOL_DLL vbScreenToWorld( vbPoint* io_point )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}

BOOL_DLL vbWorldToScreen( vbPoint* io_point )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


BOOL_DLL vbWorldToRef( vbPoint* io_point )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}

BOOL_DLL vbRefToWorld( vbPoint* io_point )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


BOOL_DLL vbViewToScreen( vbPoint* io_point )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


// THESE ARE TEMPORARY, FOR TESTING ONLY
BOOL_DLL vbSnap( vbPoint* io_point )
{
	MessageBox( NULL, __FUNCTION__, "caption", MB_OK );
	return FALSE;
}


#endif