
#ifndef _MSG_H
#define _MSG_H

#include "stdafx.h"

VOID_DLL MsgInit();
VOID_DLL MsgTerm();
BOOL_DLL MsgIsActive();
VOID_DLL MsgDisplay( const char* msg );
INT_DLL  FatalMsg( int status, const char* msgTemplate, const char* arg );

#endif
