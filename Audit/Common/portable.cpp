
#include <errno.h>
#include "portable.h"

#if (_MSC_VER == 1200)

int vsprintf_s(
   char *buffer,
   size_t numberOfElements,
   const char *format,
   va_list argptr )
{
	return vsprintf( buffer, format, argptr );
}

int sprintf_s(
   char *buffer,
   size_t sizeOfBuffer,
   const char *format, ... )
{
	va_list params;
	va_start( params, format );

	return vsprintf( buffer, format, params );
}

errno_t strcpy_s(
   char *strDestination,
   size_t numberOfElements,
   const char *strSource )
{
	char* result = strcpy( strDestination, strSource );
	return ((result == NULL) ? EINVAL : 0);
}

errno_t strncpy_s(
   char *strDest,
   size_t numberOfElements,
   const char *strSource,
   size_t count )
{
	char* result = strncpy( strDest, strSource, count );
	return ((result == NULL) ? EINVAL : 0);
}

errno_t strcat_s(
   char *strDestination,
   size_t numberOfElements,
   const char *strSource )
{
	char* result = strcat( strDestination, strSource );
	return ((result == NULL) ? EINVAL : 0);
}

#else

static void fodder()
{
	// fodder to prevent the compiler from complaining.
}

#endif
