
#include "stdafx.h"

#include "Msg.h"
#include "CodeGeoXref.h"
#include "CodeGenOH.h"


CCodeGenOH::CCodeGenOH()
{
	Init( "", NULL );
}

CCodeGenOH::~CCodeGenOH()
{
	Term();  // in case someone forgets to call Term().
}

int
CCodeGenOH::Init( const CString& fileName, CCodeGeoXref* xref )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_fileName = fileName;
	m_xref = xref;
	m_file = NULL;
	m_type = DEVICE_NONE;

	if (m_xref != NULL)
		m_xref->Flush();

	return 0;
}

int
CCodeGenOH::Term()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	Close();

	return 0;
}

int
CCodeGenOH::Open( bool asBinary, bool append )
{
	m_type = DEVICE_EDITOR;

	if (m_fileName.GetLength() > 0)
	{
		CString mode;
		if ( asBinary )
			mode = (append ? "ab+" : "wb");
		else
			mode = (append ? "a+" : "w");

		m_file = fopen( m_fileName, mode );

		if (m_file == NULL)
		{
			m_type = DEVICE_NONE;
			return 0;
		}
		else
		{
			m_type = DEVICE_FILE;
			return 1;
		}
	}

	return -1;
}

void
CCodeGenOH::Close()
{
	if (m_file != NULL)
	{
		fclose( m_file );
		m_file = NULL;
		m_type = DEVICE_NONE;
	}
}

int
CCodeGenOH::Put( const char* text )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_file != NULL)
	{
		fprintf( m_file, "%s\n", text );
	}

	if (m_xref != NULL)
	{
		// BIG ASSUMPTION: The current position in the file has
		// not changed between the time the code was generated
		// and the code was output.  If this assumption is not
		// sufficient, then the code generator must somehow tell
		// the cross reference what clfile record is associated
		// with a given block of code.  This could place an
		// additional burden on the end-user wrt. writing code
		// generators.

		m_xref->Append( text );
	}

	return 0;
}

