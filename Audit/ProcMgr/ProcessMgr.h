#if !defined(_PROCESSMGR_H)
#define _PROCESSMGR_H

// ==================================================================
//		Process Manager
//
//	Finds and registers external Process DLL's.
//	The Process Manager is a command recipient.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Return.h"

#include "Var.h"
#include "VarList.h"
#include "Command.h"

#include "RouteList.h"
#include "FileSnoop.h"

// ==================================================================

class dllExport CProcessMgr
{
public:
	CProcessMgr();
	virtual ~CProcessMgr();

	void	Reset( void );

	CReturn Register( const CString& in_subdir, CRouteList* io_route );

protected:

private:
	// Disabled.
	CProcessMgr( const CProcessMgr& );
	const CProcessMgr& operator = ( const CProcessMgr& );
	int operator == ( const CProcessMgr& ) const;
	int operator != ( const CProcessMgr& ) const;

private:
	CArray<HINSTANCE, HINSTANCE>	m_dll_list;
};

#endif

