#pragma once

dllExport void EWMInitialize();
dllExport void EWMTerminate();

dllExport void EWMRegistryBitsSet( int bits );
dllExport void EWMBitsInitFromRegistry();

dllExport int EWMBitsGet();
dllExport int EWMBitsSet( int flags );

// EWMMessageSend() is a lowel-level function and is not intended for general use.
dllExport void EWMMessageSend( const char* text );

// Fatal messages are issued regardless.
dllExport void EWMFatal( const char* text );

// NOTE: The EMWxxxAllow() functions should be used to avoid unnecessary
// message formatting and data collecting when the corresponding message
// type is known to be irrelevant.

dllExport bool EWMWarningAllow();
dllExport void EWMWarning( const char* text );

dllExport bool EWMUserAllow();
dllExport void EWMUser( const char* text );

dllExport bool EWMInternalAllow();
dllExport void EWMInternal( const char* text );

dllExport bool EWMDiagnosticAllow();
dllExport void EWMDiagnostic( const char* text );

dllExport bool EWMPortalAllow();
dllExport void EWMPortal( const char* text );

dllExport bool EWMUiAllow();
dllExport void EWMUi( const char* text );

dllExport bool EWMDaoAllow();
dllExport void EWMDao( const char* text );

dllExport bool EWMNestingAllow();
dllExport void EWMNesting( const char* text );

dllExport bool EWMSeedAllow();
dllExport void EWMSeed( const char* text );

dllExport bool EWMFileAllow();
dllExport void EWMFile( const char* text );
