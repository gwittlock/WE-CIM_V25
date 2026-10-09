// ==================================================================
//		Visual Basic Serial Port Interface
//
//	Though technically, this portal can be used by C++ applications,
//	it is a functional and not an object interface so it is really
//	focused towards the needs of VB parents.
//
//	All DLL entry-points go through here.
//
// ==================================================================

#include "stdafx.h"

#include "Serial.h"
#include "vbSerial.h"

// ==================================================================

static CSerial global_serial;

// ==================================================================
BOOL DLL_DECL 
vbSerialInit( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return global_serial.Init();
}

// ==================================================================
BOOL DLL_DECL
vbSerialConfigure( 
	const char* portname )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return global_serial.ConfigDialog( portname );
}


// ==================================================================
BOOL DLL_DECL
vbSerialOpen( 
	const char* portname )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	global_serial.PortName( portname );
	return global_serial.Open();
}

// ==================================================================
BOOL DLL_DECL
vbSerialClose( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return global_serial.Close();
}

// ==================================================================
BOOL DLL_DECL
vbSerialSendStr( 
	const char* string )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return global_serial.Send( CString(string) );
}

