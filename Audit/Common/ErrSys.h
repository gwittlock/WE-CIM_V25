
#ifndef _ERRSYS_H
#define _ERRSYS_H

#include "stdafx.h"

INT_DLL  ErrSysInit();
VOID_DLL ErrSysTerm();
BOOL_DLL ErrSysIsActive();
INT_DLL  ErrSysAdd( const char* errMsg );
INT_DLL  ErrSysCount();

#endif
