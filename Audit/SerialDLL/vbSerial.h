#if !defined(_VB_SERIAL_H)
#define _VB_SERIAL_H
#pragma once

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

// ==================================================================
//
//	Any VB-compatible structures must be declared here, between the
//	"pack" pragmas.
//
#pragma pack(4)

#pragma pack()


// ==================================================================
//
//	Portal commands.
//
BOOL_DLL vbSerialInit( void );

BOOL_DLL vbSerialConfigure( const char* portname );

BOOL_DLL vbSerialOpen( const char* portname );
BOOL_DLL vbSerialClose( void );

BOOL_DLL vbSerialSendStr( const char* string );

#endif

