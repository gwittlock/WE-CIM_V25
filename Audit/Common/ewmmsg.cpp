
#include "stdafx.h"
#include "Register.h"
#include "IpcSender.h"
#include "ewmconst.h"
#include "ewmmsg.h"

#define BIT( bit ) (g_ewm_bits & bit)


// 'g_ipc_sender' -- sends messages to the EWM application.
static CIpcSender g_ipc_sender;

// 'g_ewm_bits' -- control which messages get send to the EWM application.
static bool g_ewm_enabled = false;
static int g_ewm_bits = 0;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// CRITICAL: Since EWMRegistryBitsSet() uses CRegister, the client
// must first set the registry path using CRegister::SetRootPath().
void EWMRegistryBitsSet( int bits )
{
	CRegister::IntSetV( "Debug", "ewmbits", bits );
}

// CRITICAL: Since EWMBitsInitFromRegistry() uses CRegister, the client
// must first set the registry path using CRegister::SetRootPath().
void EWMBitsInitFromRegistry()
{
	int bits = CRegister::IntGetV( "Debug", "ewmbits", 0 );
	EWMBitsSet( bits );
}

void EWMInitialize()
{
	EWMBitsInitFromRegistry();
	g_ewm_enabled = true;
	g_ipc_sender.Initialize();
}

void EWMTerminate()
{
	if (g_ewm_enabled && g_ipc_sender.IsInitialized())
	{
		g_ipc_sender.Terminate();
		g_ewm_enabled = false;
	}
}

int EWMBitsGet()
{
	return g_ewm_bits;
}

int EWMBitsSet( int flags )
{
	int prev = g_ewm_bits;
	g_ewm_bits = flags;
	return prev;
}

void EWMMessageSend( const char* text )
{
	if (g_ewm_enabled && g_ipc_sender.IsInitialized())
		g_ipc_sender.SendMsg( MSGCMD_TEXT, text );
}

void EWMFatal( const char* text )
{
	EWMMessageSend( text );
}

bool EWMWarningAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_WARNING))) != 0);
}

void EWMWarning( const char* text )
{
	if ( EWMWarningAllow() )
		EWMMessageSend( text );
}

bool EWMUserAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_USER))) != 0);
}

void EWMUser( const char* text )
{
	if ( EWMUserAllow() )
		EWMMessageSend( text );
}

bool EWMInternalAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_INTERNAL))) != 0);
}

void EWMInternal( const char* text )
{
	if ( EWMInternalAllow() )
		EWMMessageSend( text );
}

bool EWMDiagnosticAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_DIAGNOSTIC))) != 0);
}

void EWMDiagnostic( const char* text )
{
	if ( EWMDiagnosticAllow() )
		EWMMessageSend( text );
}

bool EWMPortalAllow()
{
	// return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_PORTAL))) != 0);
	return true;
}

void EWMPortal( const char* text )
{
	if ( EWMPortalAllow() )
		EWMMessageSend( text );
}

bool EWMUiAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_UI))) != 0);
}

void EWMUi( const char* text )
{
	if ( EWMUiAllow() )
		EWMMessageSend( text );
}

bool EWMDaoAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_DAO))) != 0);
}

// DAO Database.
void EWMDao( const char* text )
{
	if ( EWMDaoAllow() )
		EWMMessageSend( text );
}

bool EWMNestingAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_NESTING))) != 0);
}

void EWMNesting( const char* text )
{
	if ( EWMNestingAllow() )
		EWMMessageSend( text );
}

bool EWMSeedAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_SEED))) != 0);
}

void EWMSeed( const char* text )
{
	if ( EWMSeedAllow() )
		EWMMessageSend( text );
}

bool EWMFileAllow()
{
	return ((BIT(EWM_ACTIVE) && (!BIT(EWM_FILTERED) || BIT(EWM_FILE))) != 0);
}

void EWMFile( const char* text )
{
	if ( EWMFileAllow() )
		EWMMessageSend( text );
}
