
#ifndef _OUTPUTHANDLER_H
#define _OUTPUTHANDLER_H

#include "stdafx.h"


class dllExport COutputHandler
{
public:

	COutputHandler()   { };

	virtual ~COutputHandler()   { };

	virtual int Init() = 0;

	virtual int Term() = 0;

	virtual int Type() = 0;

	virtual CString Path()	{ return ""; }

	virtual int Open( bool asBinary, bool append )		{ return FALSE; }

	virtual void Close()	{ };

	virtual int Put( const char* text ) = 0;

private:

};

#endif
