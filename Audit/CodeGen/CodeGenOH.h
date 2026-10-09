
#ifndef _CODEGENOH_H
#define _CODEGENOH_H

#ifndef _OUTPUTHANDLER_H
#include "OutputHandler.h"
#endif

class CCodeGeoXref;


const int DEVICE_NONE	= 0;
const int DEVICE_EDITOR	= 1;
const int DEVICE_FILE	= 2;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CCodeGenOH : public COutputHandler
{
public:

	CCodeGenOH();

	virtual int Init( const CString& fileName, CCodeGeoXref* xref );

	virtual int Init()		{ return 0; }  // compiler fodder

	virtual int Term();

	virtual int Type()		{ return m_type; }

	virtual CString Path()	{ return m_fileName; }

	virtual int Open( bool asBinary, bool append );

	virtual void Close();

	virtual int Put( const char* text );

	virtual ~CCodeGenOH();

private:

	CCodeGeoXref* m_xref;
	CString  m_fileName;
	FILE* m_file;

	int m_type;
};

#endif
