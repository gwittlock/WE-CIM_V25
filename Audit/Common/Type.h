
#ifndef _TYPE_H
#define _TYPE_H

// typedef DWORD ID;
typedef unsigned long ID;

#if (_CI || _NST)
	
	// For use by (and of) class CCivd
	//
	// Defined here (instead of CiType.h. for instance) to limit
	// #include dependencies (ie, do not want Portal to depend
	// upon Civd).
	//
	typedef enum
	{
		CI_MAIN	= 0,
		CI_AUX	= 1,
		CI_UNDO	= 2
	} eCiHookup;
#endif

#endif
