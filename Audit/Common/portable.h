#pragma once

/*
MSVC++ 11.0 _MSC_VER = 1700 (Visual Studio 2012)
MSVC++ 10.0 _MSC_VER = 1600 (Visual Studio 2010)
MSVC++ 9.0  _MSC_VER = 1500 (Visual Studio 2008)
MSVC++ 8.0  _MSC_VER = 1400 (Visual Studio 2005)
MSVC++ 7.1  _MSC_VER = 1310 (Visual Studio 2003)
MSVC++ 7.0  _MSC_VER = 1300
MSVC++ 6.0  _MSC_VER = 1200
MSVC++ 5.0  _MSC_VER = 1100
*/

#if (_MSC_VER == 1200)

#include "stdafx.h"

// This is pilfered from Microsoft.
#ifndef _ERRNO_T_DEFINED
#define _ERRNO_T_DEFINED
typedef int errno_t;
#endif

dllExport int vsprintf_s(
   char *buffer,
   size_t numberOfElements,
   const char *format,
   va_list argptr ); 

dllExport int sprintf_s(
   char *buffer,
   size_t sizeOfBuffer,
   const char *format, ... );

dllExport errno_t strcpy_s(
   char *strDestination,
   size_t numberOfElements,
   const char *strSource );

dllExport errno_t strncpy_s(
   char *strDest,
   size_t numberOfElements,
   const char *strSource,
   size_t count );

dllExport errno_t strcat_s(
   char *strDestination,
   size_t numberOfElements,
   const char *strSource );

#endif
