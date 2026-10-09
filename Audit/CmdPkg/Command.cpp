// ==================================================================
//		Command
//
//	Self-parsing Command object.  When given a string-based
//	command, it can extract one or more routing sub-strings, 
//	plus any number of tagged parameter values.
//
//	Commands take the form:
//
//		"name1:name2:a=1,b=2.0,c='text'"
//
//	Where:	name1 and name2 are routing instructions (1 or more)
//			a is an integer attribute
//			b is a floating attribute
//			c is a string attribute
//
//	':' is used to terminate routing instructions
//	'=' assigns attributes
//	',' is the attribute separator
//
// ==================================================================

#include "stdafx.h"

#include "CmdParser.h"
#include "Command.h"
extern int CmdParse( const char* cmd_string, CCommand* cmd_object );

#include "cmn_resource.h"

// ==================================================================

CCommand::CCommand()
{
	m_route_idx = -1;
	m_model = NULL;
	m_view_mgr = NULL;
}

CCommand::~CCommand()
{
	Reset();
}

// ==================================================================
void CCommand::Reset( void )
{
	m_route_idx = -1;
	m_model = NULL;
	m_view_mgr = NULL;

	if (m_varlist.countVar()) m_varlist.Reset();
	if (m_route_list.GetSize()) m_route_list.RemoveAll();
}

// ==================================================================
//	setCommand
//
//	Given a string command, break it down into useable 
//	command information.
//
CReturn CCommand::setCommand( 
	const CString& in_cmd,
	CModel*			in_model,
	CViewMgr*		in_view_mgr )
{
	Reset();
	
	return addCommand( in_cmd, in_model, in_view_mgr );
}


CReturn CCommand::addCommand( 
	const CString& in_cmd,
	CModel*			in_model,
	CViewMgr*		in_view_mgr )
{
	CReturn	ret;

	if (in_cmd.GetLength() < 1)
	{
		ret.Internal( IDS_CMD_EMPTY_COMMAND );
		return ret;
	}

	m_model = in_model;
	m_view_mgr = in_view_mgr;

	if (CmdParse( in_cmd, this ) != 0)
	{
		// Simply return an error code because CmdParse()
		// has already echoed the offending statement.
		return STATUS_ERROR;
	}

	if (count_route() > 0)
		m_route_idx = 0;

	return ret;
}

// ==================================================================
//		nextRoute
//
//	Return current routing information.  Steps to 
//	the next routing step after the current route is returned.
//
//	Returns STATUS_DONE if there is no further routing data.
//
CReturn CCommand::nextRoute( CString* io_route )
{
	if (m_route_idx < 0)
		m_route_idx = 0;

	if ( m_route_idx >= count_route() )
		return CReturn( STATUS_DONE );

	*io_route = get_route( m_route_idx );
	m_route_idx++;

	return CReturn( STATUS_OKAY );
}

// ==================================================================
//	If possible, back up one route index...
CReturn	CCommand::Backup( void )
{
	CReturn	ret;

	if (m_route_idx < 1)
	{
		ret.Internal( IDS_CMD_BACKUP_ERROR );
		return ret;
	}

	m_route_idx--;
	return ret;
}

// ==================================================================
// Used by CmdParser.cpp
CReturn CCommand::add_route( const CString& in_route )
{
	m_route_list.Add( in_route );
	return CReturn( STATUS_OKAY );
}


// ==================================================================
//	Debugging diagnostic
void CCommand::Dump( void )
{
	int count = count_route();
	if (count > 0)
	{
		CString	note;

		for (int idx = 0; idx < count; ++idx)
		{
			note.Format( "Route %d = '%s'", idx, get_route(idx) );
			EWMDiagnostic( (LPCSTR) note );
		}
	
		m_varlist.Dump();
	}
}

