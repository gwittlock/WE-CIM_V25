// ==================================================================
//		Process Manager
//
//	Finds and registers external Process DLL's.
//	The Process Manager is a command recipient.
//
// ==================================================================

#include "stdafx.h"

#include "cmn_resource.h"
#include "ProcessMgr.h"

// ==================================================================

CProcessMgr::CProcessMgr()
{
	// Just because ...
	m_dll_list.SetSize( 0, 16 );
}

CProcessMgr::~CProcessMgr()
{
	Reset();
}

void CProcessMgr::Reset( void )
{
	while (m_dll_list.GetSize())
	{
		HINSTANCE dll = m_dll_list.GetAt( 0 );
		FreeLibrary( dll );
		m_dll_list.RemoveAt( 0 );
	}
}


// ==================================================================
// Find any process DLL's, load them, and register their
// functions on the routing list.
CReturn CProcessMgr::Register( 
	const CString&	in_subdir,		// Path name with ending slash
	CRouteList*		io_route )		// Routing list to register with
{
	// Make it robust wrt. the ending slash
	CString path = in_subdir;
	int idx = path.ReverseFind( '/' );
	if (idx < 0)
		idx = path.ReverseFind( '\\' );

	if ((idx < 0) || (idx != (path.GetLength()-1)))
		path += "\\";
	
	CString pattern = path + "*.dll";

	// Find the names of all DLL's in the target directory
	CFileSnoop	snoop;
	snoop.findAllFile( pattern );

	int num = snoop.countFile();
	for (idx=0; idx<num; idx++)
	{
		CString file = snoop.getFile( idx );
		CString filepath = path + file;

		// Load the DLL and register it...
		HINSTANCE dll = LoadLibrary( filepath );
		if (dll)
		{
			pRegister reg = (pRegister)GetProcAddress( dll, "RegisterProcess" );

			if ( reg )
			{
				m_dll_list.Add( dll );
				reg( io_route );
			}
			else
			{
				CReturn	msg;
				msg.SystemError();
				FreeLibrary( dll );
			}
		}
		else
		{
			CString	note;
			CReturn	msg;
			msg.SystemError();
			msg.Internal( IDS_INTERNAL_ERROR, "CProcessMgr::Register()" );

			note.Format( "file: '%s'", filepath );
			msg.Diagnostic( note );
		}
	}

	return CReturn( STATUS_OKAY );
}

