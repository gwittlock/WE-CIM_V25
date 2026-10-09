#if !defined(_ROUTELIST_H)
#define _ROUTELIST_H

// ==================================================================
//		RouteList
//
//	List of commands (routing names) with associated recipient
//	object.  Used to register commands, and then route command
//	objects to them.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

class CRouteList;

// ==================================================================

#include <afxtempl.h>

#include "Return.h"

#include "Var.h"
#include "VarList.h"
#include "Command.h"
#include "DynamicArray.h"
#include "text_file.h"

#include "CmdProcess.h"

// ==================================================================
//
//	We have two choices for the route list:
//		1) the technique used here, is to have static functions that
//			have their addresses stored in the list.
//
//		2) the other choice is to store addresses of objects, all of
//			which are descended from CCmdProcess, with an Execute() method.
//
//	TODO:  think about our choices, before we get in too deep.
//

typedef CReturn (*pProcess)(CCommand*);		// Command execution
typedef CReturn (*pRegister)(CRouteList*);	// Routing registration

class CRouteEntry;

// ==================================================================

class dllExport CRouteList
{
public:

	CRouteList();

	virtual ~CRouteList();

	void	Reset( void );

	CReturn	addProcess( const CString& in_name, pProcess in_process );

	CReturn	addSubrouter( const CString& in_name, CRouteList* in_subrouter );

	CReturn	Dispatch( CCommand* io_cmd );
	bool	Verify( CCommand* io_cmd );

	void	Dump( const CString& filepath );
	void	Dump( CTextFile& file, CString& prefix );

	// Use very sparingly.
	// See also CiViewProcess::~CiViewProcess() :-(
	void	BenignFlush()	{ m_routelist.BenignFlush(); }

protected:

private:
	// Disabled.
	CRouteList( const CRouteList& );
	const CRouteList& operator = ( const CRouteList& );
	int operator == ( const CRouteList& ) const;
	int operator != ( const CRouteList& ) const;

private:
//	int				count( void ) const					{ return m_routelist.Count(); }
//	CRouteEntry*	entry( int idx )					{ return m_routelist[idx]; }

//	int				count_route() const					{ return m_route_list.GetSize(); }
//	const CString	get_name( int in_idx ) const		{ return m_route_list.GetAt(in_idx); }
//	pProcess		get_process( int in_idx ) const		{ return m_process_list.GetAt(in_idx); }
//	CRouteList*		get_subrouter( int in_idx ) const	{ return m_subrouter_list.GetAt(in_idx); }

	//
	// These two arrays must be kept in synch
	//
	CDynamicArray<CRouteEntry*>		m_routelist;
//	CStringArray					m_route_list;
//	CArray<pProcess,pProcess>		m_process_list;
//	CArray<CRouteList*,CRouteList*>	m_subrouter_list;
};

#endif

