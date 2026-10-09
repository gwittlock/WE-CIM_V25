#if !defined(_VB_PORTAL_H)
#define _VB_PORTAL_H

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

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================
//
//	Any VB-compatible structures must be declared here, between the
//	"pack" pragmas.
//
#pragma pack(4)

struct vbPoint
{
	double	x;
	double	y;
	double	z;
};

#pragma pack()

// ==================================================================
//
//	Portal commands.
//
BOOL_DLL vbInit(
	const char* regRootPath,
	const char* dllPath,
	const char* classPath,
	const char* storagePath,
	int suppressUndoBuffer );

BOOL_DLL vbTerminate();

#if (_CI || _NST)
BOOL_DLL vbHookup(
			LPDISPATCH	docObject,
			LPDISPATCH	cmdObject,
			int			which );	// (0) main / (1) auxillary
#endif

BOOL_DLL vbRegister( const char* in_subdir );
BOOL_DLL vbExecute( const char* in_command );
BOOL_DLL vbVerify( const char* in_command );
BOOL_DLL vbParse( const char* in_command );

BOOL_DLL vbGetInt( const char* in_name, int* io_val );
BOOL_DLL vbGetReal( const char* in_name, double* io_val );

BOOL_DLL vbModelSwap( int use_file_preview_model );

#if (_CI || _NST)

	INT_DLL vbGetString( const char* in_name, char* io_val, int in_buflen );
	INT_DLL vbGetString2( const char* in_name, char* io_val, int in_buflen );

#else // WE_CIM

	INT_DLL vbGetString( const char* in_name, char* io_val, int in_buflen );
	INT_DLL vbGetString2( const char* in_name, char* io_val, int in_buflen );

#endif

BOOL_DLL vbScreenToWorld( vbPoint* io_point );
BOOL_DLL vbWorldToScreen( vbPoint* io_point );

BOOL_DLL vbWorldToRef( vbPoint* io_point );
BOOL_DLL vbRefToWorld( vbPoint* io_point );

BOOL_DLL vbViewToScreen( vbPoint* io_point );

// THESE ARE TEMPORARY, FOR TESTING ONLY
BOOL_DLL vbSnap( vbPoint* io_point );

#endif

